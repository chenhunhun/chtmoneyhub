#pragma once
#include <string>
#include <iostream>

class CCPUID
{
public:
	CCPUID()
	{
		m_strName = m_strStepping = "unknown";

		if (QueryInfoFromCPUID())
			return;
		
		QueryInfoFromRegistry();
	}

public:
	std::string GetName()
	{
		return m_strName.size() > 0 ? m_strName : "Unknown...";
	}

	// CPU Stepping 与 CPU Revision 的关系:
	// (1) Stepping 是 cpuid(1).eax[3:0]
	// (2) Revision 是 A0/B1/M0等形式
	// (3) 两者有对应关系，但是需要查对应表，没有办法计算
	// (4) 对应表可参考 http://www.bubik.cz/cpu/
	// (5) 低版本cpu-z不能读取正确revision就是这个原因
	std::string GetSteppingId()
	{
		return m_strStepping.size() > 0 ? m_strStepping : "Unknown...";
	}

protected:
	void asm_cpuid(DWORD ex)
	{
		DWORD deax;
		DWORD debx;
		DWORD decx;
		DWORD dedx;

		__asm
		{
			mov eax, ex
			cpuid
			mov deax, eax
			mov debx, ebx
			mov decx, ecx
			mov dedx, edx
		}

		m_eax = deax;
		m_ebx = debx;
		m_ecx = decx;
		m_edx = dedx;
	}

	void SkipMoreBlank(char* lpszBuffer, size_t nLength)
	{
		int idx = 0;
		bool bBlank = true;
		for (size_t i = 0; i < nLength; i++)
		{
			if (lpszBuffer[i] == ' ')
			{
				if (bBlank)
					continue;
				else
					bBlank = true;
			}
			else
				bBlank = false;

			lpszBuffer[idx++] = lpszBuffer[i];
		}
	}

	bool QueryInfoFromCPUID()
	{
		bool bCPUID = true;
		__try
		{
			__asm
			{
				xor eax, eax
				cpuid
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			bCPUID = false;
		}

		if (!bCPUID)
			return false;

		// name

		char szBuffer[128];
		memset(szBuffer, 0, sizeof(szBuffer));

		DWORD exData = 0x80000002;
		for (DWORD i = 0; i < 3; i++)
		{
			asm_cpuid(exData + i);			
			memcpy(szBuffer + i * 16, &m_eax, 16);
		}
		
		SkipMoreBlank(szBuffer, sizeof(szBuffer));
		m_strName = szBuffer;


		// stepping

		memset(szBuffer, 0, sizeof(szBuffer));

		asm_cpuid(1);
		sprintf_s(szBuffer, sizeof(szBuffer), "%02xh", m_eax & 0x0F);

		m_strStepping = szBuffer;
		
		return true;
	}

	bool QueryInfoFromRegistry()
	{
		HKEY hKey;
		DWORD dwType;
		DWORD dwSize;
		LONG ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
			"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 
			0,
			KEY_READ,
			&hKey);

		if (ret != ERROR_SUCCESS)
			return false;

		// name

		dwSize = 0;
		ret = RegQueryValueExA(hKey, "ProcessorNameString", NULL, &dwType, NULL, &dwSize);
		char* szName = new char[dwSize + 1];
		ret = RegQueryValueExA(hKey, "ProcessorNameString", NULL, &dwType, (LPBYTE)szName, &dwSize);

		if(ret != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			delete []szName;
			return false;
		}
		
		SkipMoreBlank(szName, dwSize + 1);

		m_strName = szName;
		delete []szName;

		// stepping

		dwSize = 0;
		ret = RegQueryValueExA(hKey, "Identifier", NULL, &dwType, NULL,	&dwSize);
		char* szIdentifier = new char[dwSize + 1];
	
		ret = RegQueryValueExA(hKey, "Identifier", NULL, &dwType, (LPBYTE)szIdentifier, &dwSize);

		if(ret != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			delete []szIdentifier;

			return false;
		}

		std::string sIdentifier = szIdentifier;
		size_t index = sIdentifier.find("Family");
		sIdentifier = sIdentifier.substr(index, sIdentifier.length() - index);

		char szStepping[10];
		const char* lpszIdentifier = sIdentifier.c_str();
		sscanf_s(lpszIdentifier, "%*s%*s%*s%*s%*s%s", szStepping, sizeof(szStepping));
		sprintf_s(szStepping, sizeof(szStepping), "%02xh", atoi(szStepping));
		m_strStepping = szStepping;

		RegCloseKey(hKey);

		return true;
	}

protected:
	DWORD m_eax;
	DWORD m_ebx;
	DWORD m_ecx;
	DWORD m_edx;

protected:
	std::string m_strName;
	std::string m_strStepping;

protected:
	bool m_bCPUIDSupported;
};