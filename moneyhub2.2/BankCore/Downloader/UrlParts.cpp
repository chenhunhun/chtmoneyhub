
#include "stdafx.h"
#include "UrlParts.h"

CUrlParts::CUrlParts()
{
	Allocated = false;
	dwScheme = (DWORD)0;
	dwHostName = (DWORD)0;
	dwUserName = (DWORD)0;
	dwPassword = (DWORD)0;
	dwUrlPath = (DWORD)0;
	dwExtraInfo = (DWORD)0;
	dwFileName = (DWORD)0;
	dwFileExtension = (DWORD)0;
	dwPort = (DWORD)0;
	lnScheme = INTERNET_SCHEME_DEFAULT;
}

CUrlParts::~CUrlParts()
{
	ResetBuffers();
}

void CUrlParts::ResetBuffers()
{
	if (Allocated)
	{
		free(szScheme);
		free(szHostName);
		free(szUserName);
		free(szPassword);
		free(szUrlPath);
		free(szExtraInfo);
		free(szFileName);
		free(szFileExtension);
	
		dwScheme = (DWORD)0;
		dwHostName = (DWORD)0;
		dwUserName = (DWORD)0;
		dwPassword = (DWORD)0;
		dwUrlPath = (DWORD)0;
		dwExtraInfo = (DWORD)0;
		dwFileName = (DWORD)0;
		dwFileExtension = (DWORD)0;
		dwPort = (DWORD)0;
	}

	Allocated = false;
}

bool CUrlParts::AllocateBuffers(int iNum)
{
	if(Allocated == true)
		ResetBuffers();
	szScheme = (LPTSTR) malloc( (iNum+1) * sizeof(TCHAR));
	//Check
	if(!szScheme)
		return false;
	szHostName = (LPTSTR) malloc((iNum+1) * sizeof(TCHAR));
	szUserName = (LPTSTR) malloc((iNum+1) * sizeof(TCHAR));
	szPassword = (LPTSTR) malloc((iNum+1) * sizeof(TCHAR));
	szUrlPath = (LPTSTR) malloc((iNum+1) * sizeof(TCHAR));
	szExtraInfo = (LPTSTR) malloc((iNum+1) * sizeof(TCHAR));
	szFileName = (LPTSTR) malloc((iNum+1) * sizeof(TCHAR));
	szFileExtension = (LPTSTR) malloc((iNum+1) * sizeof(TCHAR));
	//Check the last one
	if(!szFileExtension)
		return false;
	szScheme[iNum] = _T('\0');
	szHostName[iNum] = _T('\0');
	szUserName[iNum] = _T('\0');
	szPassword[iNum] = _T('\0');
	szUrlPath[iNum] = _T('\0');
	szExtraInfo[iNum] = _T('\0');
	szFileName[iNum] = _T('\0');
	szFileExtension[iNum] = _T('\0');
	return true;
}

bool CUrlParts::SplitUrl(BSTR bUrl)
{
	if(bUrl == NULL)
		return false;

	DWORD ilen = ::SysStringLen(bUrl);
	if (ilen == 0)
		return false;

	if (!AllocateBuffers(ilen))
		return false;

	URL_COMPONENTS URLComponentsOut;
	ZeroMemory((LPVOID)&URLComponentsOut, sizeof(URLComponentsOut));
	URLComponentsOut.dwStructSize = sizeof(URLComponentsOut);

	URLComponentsOut.lpszScheme = szScheme;
	URLComponentsOut.dwSchemeLength = ilen;
	
	URLComponentsOut.lpszHostName = szHostName;
	URLComponentsOut.dwHostNameLength = ilen;
	
	URLComponentsOut.lpszUserName = szUserName;
	URLComponentsOut.dwUserNameLength = ilen;
	
	URLComponentsOut.lpszPassword = szPassword;
	URLComponentsOut.dwPasswordLength = ilen;
	
	URLComponentsOut.lpszUrlPath = szUrlPath;
	URLComponentsOut.dwUrlPathLength = ilen;
    
	URLComponentsOut.lpszExtraInfo = szExtraInfo;
    URLComponentsOut.dwExtraInfoLength = ilen;

	DWORD dwFlags = ICU_DECODE;

	USES_CONVERSION;
	if (!InternetCrackUrl(OLE2T(bUrl), 0, dwFlags, &URLComponentsOut))
		return false;

	dwScheme = URLComponentsOut.dwSchemeLength;
	dwHostName = URLComponentsOut.dwHostNameLength;
	dwUserName = URLComponentsOut.dwUserNameLength;
	dwPassword = URLComponentsOut.dwPasswordLength;
	dwUrlPath = URLComponentsOut.dwUrlPathLength;
	dwExtraInfo = URLComponentsOut.dwExtraInfoLength;
	
	lnScheme = URLComponentsOut.nScheme;

	if (dwUrlPath > 0)
	{
		//Look for filename here
		TCHAR *lpStr1 = szUrlPath;
		lpStr1 += _tcslen(lpStr1);
    	if (*lpStr1 == _T('/'))
    		--lpStr1;
    	while (*lpStr1 != _T('/'))
		{			
			if ((*lpStr1 == _T('.')) && (dwFileExtension == (DWORD)0))
			{
				dwFileExtension = _tcslen(lpStr1);
				if (dwFileExtension > 0)
				{
					memcpy(szFileExtension, lpStr1, dwFileExtension * sizeof(TCHAR) );
					szFileExtension[dwFileExtension] = _T('\0');
				}
			}

    		--lpStr1;
		}

		lpStr1++;
		dwFileName = _tcslen(lpStr1);
		if (dwFileName > 0)
		{
			memcpy(szFileName, lpStr1, dwFileName * sizeof(TCHAR));
			szFileName[dwFileName] = _T('\0');
		}
	}

	dwPort = URLComponentsOut.nPort;
	return true;
}
