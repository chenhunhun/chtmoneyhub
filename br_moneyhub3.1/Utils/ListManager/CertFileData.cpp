#include "stdafx.h"
#include "../CryptHash/base64.h"
#include "Requirement.h"
#include "ResourceManager.h"
#include "CertFileData.h"
#include "ConvertBase.h"
#include "StringHelper.h"
#include "../RecordProgram/RecordProgram.h"

CCertFileData::CCertFileData(const CRequirement *pRequire, const CWebsiteData *pWebsiteData) : m_pRequire(pRequire), m_pWebsiteData(pWebsiteData) {}


bool CCertFileData::InstallCert()
{
	for (size_t i = 0; i < m_pRequire->GetFileData()->GetFileList().size(); i++)
	{
		std::wstring strFileName = m_pRequire->GetFileData()->GetFileList()[i];
		strFileName = CResourceManager::_()->GetFilePath(m_pWebsiteData->GetWebsiteType(), m_pWebsiteData->GetID(), strFileName.c_str());
		HANDLE hFile = ::CreateFile(strFileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwFileSize = ::GetFileSize(hFile, NULL);
			char *pData = new char[dwFileSize + 1];
			::ReadFile(hFile, pData, dwFileSize, &dwFileSize, NULL);
			::CloseHandle(hFile);
			pData[dwFileSize] = 0;

			std::string strFull = pData;

			std::wstring szData = AToW(strFull);

			WriteCertificate(szData);

			delete pData;
			/*for (size_t i = 0; i < strFull.length();)
			{
				size_t p = strFull.find_first_of("\r\n", i);
				std::string strLine;
				if (p != std::string::npos)
				{
					strLine = strFull.substr(i, p - i);
					while (strFull[p] == '\r' || strFull[p] == '\n')
						p++;
					i = p;
				}
				else
				{
					strLine = strFull.substr(i);
					i = strFull.length();
				}
				if (strLine.substr(0, 5) == "-----")
					continue;
				strBase64 += strLine;
			}
			delete pData;

			int iBinaryLen = strBase64.length() * 4 / 3 + 10;
			BYTE *pBinary = new BYTE[iBinaryLen];
			base64_decode((BYTE*)strBase64.c_str(), strBase64.length(), pBinary, &iBinaryLen);
			PCCERT_CONTEXT cc = ::CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, pBinary, iBinaryLen);

			if (cc == NULL)
			{
				delete pBinary;
				return false;
			}

			BYTE hash[128];
			DWORD dwLen = 128;
			::CryptHashCertificate(0, 0, 0, cc->pbCertEncoded, cc->cbCertEncoded, hash, &dwLen);
			::CertFreeCertificateContext(cc);*/
			// 这里应当增加安装证书的功能，但应当将证书放入系统中？
			// 读入数据直接写到注册表里
//			char szFingerPrintChar
//			AtlHexEncode


			//delete pBinary;
		}
	}

	return true;
}

void CCertFileData::WriteCertificate(wstring& szData)
{
	wstring key,valuename;
	HKEY rootkey;
	unsigned char* m_value = NULL;
	wstring strLine;
	DWORD iDataLength = 0;

	for (size_t n = 0; n < szData.length();)
	{
		size_t nLineEnd = szData.find_first_of('\n', n);
		wstring strCurrentLine;
		if (nLineEnd == std::wstring::npos)
		{
			strCurrentLine = szData.substr(n);
			n = szData.length();
		}
		else
		{
			strCurrentLine = szData.substr(n, nLineEnd - n - 1);
			n = nLineEnd + 1;
		}
		TrimString(strCurrentLine);
		if (strCurrentLine.empty())
			continue;
		if (strCurrentLine[strCurrentLine.length() - 1] == '\\')
		{
			// 说明这一行没完，要找到一行不以“\”结尾的行，然后把每行的内容 拼起来
			strCurrentLine.erase(strCurrentLine.length() - 1);
			TrimString(strCurrentLine);
			strLine += strCurrentLine;
			continue;
		}
		strLine += strCurrentLine;

		if (strLine[0] == '[')
		{
			// 这一行是key的名称
			size_t nEndKeyName = strLine.find_first_of(']', 1);
			std::wstring strKeyName = strLine.substr(1, nEndKeyName - 1);
			TrimString(strKeyName);

			size_t nEnd = strKeyName.find_first_of('\\');
			std::wstring strRootKey = strKeyName.substr(0, nEnd);

			if(strRootKey == L"HKEY_CURRENT_USER")
				rootkey = HKEY_CURRENT_USER;
			else if (strRootKey == _T("HKEY_LOCAL_MACHINE"))
				rootkey = HKEY_LOCAL_MACHINE;

			key = strKeyName.substr(nEnd + 1, strKeyName.size());
		}
		else if (key.size() > 0)
		{
			// 这一行是key目录下面item的数据
			size_t nEqual = strLine.find_first_of('=');
			if (nEqual != std::wstring::npos)
			{
				std::wstring strValue = strLine.substr(nEqual + 1);
				TrimString(strValue);

				valuename = strLine.substr(0, nEqual);
				TrimString(valuename, BLANKS_WITH_QUOTATION);

				if (_tcsnicmp(strValue.c_str(), _T("hex:"), 4) == 0)
				{
					//m_uRegValueType = ;
					StrVecW strVec;
					if (::SplitStringW(strValue.c_str() + 4, strVec, ',') > 0)
					{
						m_value = new BYTE[strVec.size()];
						for (size_t i = 0; i < strVec.size(); i++)
							m_value[i] = (BYTE)_tcstol(strVec[i].c_str(), NULL, 16);
					}
					iDataLength = strVec.size();
				}
			}
		}
		strLine.clear();
	}

	if(iDataLength  > 0 )
	{
		DWORD dwType;
		unsigned char dwValue[16000];
		DWORD dwReturnBytes = 16000;
		if(::SHGetValueW(rootkey, key.c_str(), valuename.c_str(), &dwType, dwValue, &dwReturnBytes) != ERROR_SUCCESS)//已经安装了，就不再进行写入了
		{
			if(::SHSetValueW(rootkey, key.c_str(), valuename.c_str(), REG_BINARY, m_value, iDataLength) != ERROR_SUCCESS)
			{
				int error = GetLastError();
				CRecordProgram::GetInstance()->FeedbackError(L"INSTALLCERT", error, key.c_str());
				if (error == ERROR_ACCESS_DENIED)
				{
					USES_CONVERSION;
					std::string appid = CFavBankOperator::GetBankIDOrBankName(W2A(m_pWebsiteData->GetID()),false);
					CWebsiteData::StartUAC(A2W(appid.c_str()));
				}
			}
		}
	}

	if(m_value)
		delete[] m_value;
}