#include "StdAfx.h"
#include "DownloadOperation.h"
#include "../ThirdParty/tinyxml/tinyxml.h"
#include "../Encryption/CHKFile/CHK.h"
#include "Updater.h"
#include "UpdateMgr.h"
#include "..\ThirdParty\CabExtract\CabExtract.h"
#include "..//Utils/VersionManager/versionManager.h"
#include "../Security/Authentication/encryption/md5.h"

CDownloadOperation::CDownloadOperation(void)
{
	m_vecSpecialDName.push_back(L"Html.cab");
	m_vecSpecialDName.push_back(L"Config.cab");

	//黑白名单
	m_vecWBFileName.push_back(L"BlackList.chk");
	m_vecWBFileName.push_back(L"CloudCheck.chk");
	m_vecWBFileName.push_back(L"syslist.chk");
}

CDownloadOperation::~CDownloadOperation(void)
{
}


std::wstring CDownloadOperation::getModulePath()
{
	wchar_t lpPath[255];
	GetModuleFileNameW(NULL,lpPath,sizeof(lpPath) );
	LPWSTR p = wcsrchr(lpPath,L'\\');
	*(p+1)=L'\0';
	std::wstring wStr = lpPath;

	return wStr;
}

std::wstring CDownloadOperation::getAppDataPath()
{
	return getModulePath();
}

/**
*  content : xml content
*  version : moneyhub.exe version
   moduleName : Main
*/
bool  CDownloadOperation::getXMLInfo(OUT LPSTR content,IN DWORD dwlen,IN LPSTR version ,IN LPSTR moduleName)    
{
	LPSTR  pModule = XMLMODULEINFO;
 	char buf[255];

	VMMAPDEF map_version;
	USES_CONVERSION;
	versionManager::getHinstance()->getAllVersion(map_version,ALLVERSION, true);
	for(VMMAPDEF::iterator it = map_version.begin(); it != map_version.end(); it++)
	{
		sprintf_s(buf,sizeof(buf),pModule, W2A( it->first.c_str() ), W2A( it->second.c_str() ));
		strcat_s(content, dwlen, buf);
	}

	return true;
}

void  CDownloadOperation::deleteDirectoryW(LPWSTR path)
{
	WIN32_FIND_DATAW  fw;
	std::wstring wstrPath = path ;
	wstrPath              += L"*.*";
	HANDLE hFind= FindFirstFileW(wstrPath.c_str(), &fw);

	if(hFind == INVALID_HANDLE_VALUE)
		return ;

	do
	{
		if(wcscmp(fw.cFileName,L".") == 0 || wcscmp(fw.cFileName,L"..") == 0 || wcscmp(fw.cFileName,L".svn") == 0)
			continue;

		if(fw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			std::wstring wstrDirectory = path;
			wstrDirectory             += fw.cFileName;
			wstrDirectory             += L"\\";
	
			deleteDirectoryW((LPWSTR)wstrDirectory.c_str());
			RemoveDirectoryW(wstrDirectory.c_str() );
		}
		else
		{
			std::wstring existingFile = path;
			existingFile += fw.cFileName;
			DeleteFileW(existingFile.c_str() );
		}
	}
	while( FindNextFile(hFind,&fw) );  

	FindClose(hFind);
}

/**
*         获得下载目录 
*/
std::wstring CDownloadOperation::getBankCachePath(bool bChoose)
{
	LPWSTR lpszTempPath = new wchar_t[MAX_PATH + 1];
	int nLength = 0;

	if( bChoose )
		nLength = GetTempPathW(MAX_PATH, lpszTempPath);
	else
		nLength = ExpandEnvironmentStringsW(L"%appdata%", lpszTempPath, MAX_PATH);


	while (nLength > MAX_PATH)
	{
		delete[] lpszTempPath;
		lpszTempPath = new wchar_t[nLength + 1];

		if( bChoose )
			nLength = GetTempPathW(nLength, lpszTempPath);
		else
			nLength = ExpandEnvironmentStringsW(L"%appdata%", lpszTempPath, nLength);
	}

	wchar_t tmp[500];
	wcscpy_s(tmp,_countof(tmp),lpszTempPath);	
	GetLongPathNameW(tmp,lpszTempPath,MAX_PATH+1 );
	wcscpy_s(lpszTempPath,MAX_PATH+1,tmp);

	lpszTempPath[nLength] = L'\0';
	std::wstring path = lpszTempPath;	
	delete[] lpszTempPath;

	if (path[path.size() - 1] != L'\\')
		path += L"\\";
	path += L"BankUpdate";
	path += L'\\';

	if( true == bChoose)
		CreateDirectory(path.c_str(), NULL);

	return path;
}


/**
*
*    解压缩文件
*/

void CDownloadOperation::uncompressFile()
{
	std::wstring wcsInstallPath = getBankCachePath();
	wcsInstallPath += L"*.*";
	uncompressTraverse((LPWSTR)wcsInstallPath.c_str(),1);

	deleteAllUpdataFile((LPWSTR)getBankCachePath(true).c_str());
}

/**
*   return true,is not creating directory
*/
bool CDownloadOperation::isSpecialCab(wchar_t * wcsP)
{
	for(int i=0; i<m_vecSpecialDName.size(); i++)
	{
		if( 0 == wcscmp(m_vecSpecialDName[i].c_str(), wcsP) )
			return true;
	}

	return false;
}
/**
*           bIndex == 1     遍历指定文件夹并解压缩
*           数据升级Config因为包括黑白名单缓存，需特殊处理
*/
void CDownloadOperation::uncompressTraverse(IN LPWSTR path ,IN BYTE bIndex, IN OUT LPWSTR parentDirectory )
{
	USES_CONVERSION;
	WIN32_FIND_DATAW  fw;
	HANDLE hFind= FindFirstFileW(path,&fw);

	if(hFind == INVALID_HANDLE_VALUE)
		return ;

	WBRETURN bIsWBCab = NONE;

	do
	{
		if(wcscmp(fw.cFileName,L".") == 0 || wcscmp(fw.cFileName,L"..") == 0 || wcscmp(fw.cFileName,L".svn") == 0)
			continue;

		if(fw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(bIndex == 1)
			{	

			}
		}
		else
		{
			if(bIndex == 1 && strstr(W2A(fw.cFileName),"cab") != NULL)
			{
				std::wstring newFile ;
				bool bIsSC = isSpecialCab(fw.cFileName);

				newFile = this->getAppDataPath();

				if( !bIsSC )
					newFile += BANKINFOPATHNOFIND;

				WBRETURN bWBCab = IsNewTBCacheFile(fw.cFileName);
				if( bWBCab != NONE)
					if( bWBCab != bIsWBCab && bIsWBCab != ALL )
					{
						if( bIsWBCab == NONE)
							bIsWBCab = bWBCab;
						else
							bIsWBCab = ALL;
					}
				

				std::wstring existingFile ;
				existingFile = getBankCachePath();
				existingFile += fw.cFileName;

				//没有通过验证的升级包不允许升级
				if( _UpdateMgr.isSafeFile((wchar_t *)existingFile.c_str() ) )
				{				
					moveFiles(fw.cFileName, true);	

					CCabExtract tt;
					if(tt.ExtractFile(existingFile.c_str(), newFile.c_str() ) )
						OutputDebugStringW(L"extractFile is OK!");
					else
					{
						moveFiles(fw.cFileName, false);
					}
				}
			}
		}
	}
	while( FindNextFile(hFind,&fw) );  

	this->UpdateWBFile(bIsWBCab);
	FindClose(hFind);
}


void CDownloadOperation::deleteAllUpdataFile(wchar_t* wcsPath)
{	
	std::wstring wStr ;
	if( wcsPath )
		wStr = wcsPath;
	else
		wStr= getBankCachePath();

	this->deleteDirectoryW((LPWSTR)wStr.c_str() );
}


int CDownloadOperation::isValid()
{
	int index = _UpdateMgr.checkFileValid();
	std::wstring wstrDes;

	switch(index)
	{
	case 0:
		this->deleteAllUpdataFile();
		return false;
		break;
	case 1:
		break;
	case 2:
		break;
	default:
		break;
	}

	return index;
}


void CDownloadOperation::ShowMessage()
{
	std::string wstrContent ;
	bool bShow = false;

	for(int i=0; i < _UpdateMgr.m_vecUpgradeContent.size(); i++)
	{
		bShow = true;
		wstrContent += _UpdateMgr.m_vecUpgradeContent[i].c_str();
		wstrContent += "\n";
	}
	
	if( true == bShow )
		MessageBoxA(NULL, wstrContent.c_str(), "财金汇升级提示", MB_OK);
}


/**
*
*/
WBRETURN CDownloadOperation::IsNewTBCacheFile( wchar_t * pFileName )
{
	WBRETURN bReturn = NONE;

	//黑白名单位于 Config文件夹内
	if( wcscmp(L"Config.cab", pFileName) != 0 )
		return bReturn;
	
	std::wstring newFile ;

	newFile = this->getBankCachePath( true );


	std::wstring existingFile ;
	existingFile = getBankCachePath();
	existingFile += pFileName;

			
	CCabExtract tt;
	if(tt.ExtractFile(existingFile.c_str(), newFile.c_str() ) )
	{
		OutputDebugStringW(L"extractFile is OK!");
	}


#define BUFSIZEFORMD5         4 * 1024 * 1024
	unsigned char * pBuf = new unsigned char[BUFSIZEFORMD5];
	unsigned char * pCheckE = new unsigned char[16];
	unsigned char * pCheckN = new unsigned char[16];
	DWORD dwTmp = 0;

	if( pBuf && pCheckN && pCheckE )
	{
		for( int i=0; i < m_vecWBFileName.size(); i++)
		{
				std::wstring wsNewFile = newFile;
				std::wstring wsExistingFile = getModulePath();

				wsNewFile += L"Config\\";
				wsExistingFile += L"Config\\";

				wsNewFile += m_vecWBFileName[i].c_str();
				wsExistingFile += m_vecWBFileName[i].c_str();

				HANDLE hFileE = CreateFile(wsExistingFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING ,FILE_ATTRIBUTE_NORMAL, NULL);
				HANDLE hFileN = CreateFile(wsNewFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING ,FILE_ATTRIBUTE_NORMAL, NULL);
				if( !(INVALID_HANDLE_VALUE == hFileE || INVALID_HANDLE_VALUE == hFileN) )
				{
					/////对缓存文件进行MD5计算
					ZeroMemory(pBuf, BUFSIZEFORMD5);
					CMD5 md5E;
					while(1)
					{
						if( ReadFile(hFileE, pBuf, BUFSIZEFORMD5, &dwTmp, NULL) )
						{
							if( 0 == dwTmp)	
								break;

							md5E.MD5Update(pBuf, dwTmp);
						}
					}
					md5E.MD5Final(pCheckE);
					/////对黑白名单文件文件进行MD5计算
					ZeroMemory(pBuf, BUFSIZEFORMD5);
					CMD5 md5N;
					while(1)
					{
						CMD5 md5;
						if( ReadFile(hFileN, pBuf, BUFSIZEFORMD5, &dwTmp, NULL) )
						{
							if( 0 == dwTmp)
								break;

							md5N.MD5Update(pBuf, dwTmp);
						}
					}	
					md5N.MD5Final(pCheckN);
				}
			
				CloseHandle(hFileE);
				CloseHandle(hFileN);

			if( 0 != memcmp(pCheckN, pCheckE, 16) )
			{
				if( i==0 )
				{	
					if( bReturn == WHITEUPDATE )
					{
						bReturn = ALL;
						break;
					}
					bReturn = BLACKUPDATE;
				}
				else
				{	
					if( bReturn == BLACKUPDATE )
					{
						bReturn = ALL;
						break;
					}
					bReturn = WHITEUPDATE;
				}
			}
		}
	}

	return bReturn;
}


/**
*  根据需求，如果更新文件有白名单更改文件，忽略白名单缓存重建
*/
bool CDownloadOperation::UpdateWBFile(WBRETURN wb)
{
	bool bReturn = true;
	//更新黑白名单缓存
	std::wstring wsModulePath = getModulePath();
	wsModulePath += L"Moneyhub_Svc.exe";//-reblack -rebuild

	HANDLE hWait[2] = {0};

	switch( wb )
	{
	case BLACKUPDATE:
		hWait[0] = CreateEventW(NULL,FALSE, FALSE, L"MONEYHUBEVENT_BLACKUPDATE");
		if( hWait[0] )
		{
			ShellExecuteW(GetDesktopWindow(), L"open", wsModulePath.c_str(), L"-reblack",NULL,SW_SHOW );
			WaitForMultipleObjects(1,hWait,TRUE, 30000);
			CloseHandle(hWait[0]);
		}
		break;
	case WHITEUPDATE:
		hWait[1] = CreateEventW(NULL,FALSE, FALSE, L"MONEYHUBEVENT_WHITEUPDATE");
		if( hWait[0] )
		{
			//ShellExecuteW(GetDesktopWindow(), L"open", wsModulePath.c_str(), L"-rebuild",NULL,SW_SHOW );
			//WaitForMultipleObjects(1,hWait,TRUE, 50000);
			CloseHandle(hWait[1]);
		}
		break;
	case ALL:
		hWait[0] = CreateEventW(NULL,FALSE, FALSE, L"MONEYHUBEVENT_BLACKUPDATE");
		hWait[1] = CreateEventW(NULL,FALSE, FALSE, L"MONEYHUBEVENT_WHITEUPDATE");
		if( hWait[0] && hWait[1])
		{
			ShellExecuteW(GetDesktopWindow(), L"open", wsModulePath.c_str(), L"-reblack",NULL,SW_SHOW );
			//ShellExecuteW(GetDesktopWindow(), L"open", wsModulePath.c_str(), L"-rebuild",NULL,SW_SHOW );
			WaitForMultipleObjects(1,hWait,TRUE, 30000);
			CloseHandle(hWait[0]);
			CloseHandle(hWait[1]);
		}
		break;
	default:
		break;
	}

	return bReturn;
}
/**
*
*/
void CDownloadOperation::moveFiles(LPWSTR wcsFileName, bool bDirection)
{
	std::wstring newFile ;
	std::wstring existingFile ;

	newFile = this->getAppDataPath();//execute filePath
	existingFile = getBankCachePath(true);//bank cache(%temp%)

	bool bIsSC = isSpecialCab(wcsFileName);
	if( !bIsSC )
		newFile += BANKINFOPATHNOFIND;

	wchar_t wcsBuf[MAX_PATH] = {0};
	wcscpy_s(wcsBuf, _countof(wcsBuf), wcsFileName);
	if( wcsrchr(wcsBuf, L'.') )
	{
		*(wchar_t*)(wcsrchr(wcsBuf, L'.')) = L'\0';

		newFile += wcsBuf;
		newFile += L"\\";
		existingFile += wcsBuf;
		existingFile += L"\\";

		if(bDirection)		
		{
			CreateDirectoryW(existingFile.c_str(), NULL);
			moveFileTraverse((LPWSTR)newFile.c_str(), (LPWSTR)newFile.c_str(), (LPWSTR)existingFile.c_str() );
		}
		else		
		{
			CreateDirectoryW(newFile.c_str(), NULL);
			deleteAllUpdataFile((LPWSTR) newFile.c_str());
			moveFileTraverse((LPWSTR)existingFile.c_str(), (LPWSTR)existingFile.c_str(), (LPWSTR)newFile.c_str() );
			deleteAllUpdataFile((LPWSTR)getBankCachePath(true).c_str());
		}
	}
}
/**
*              
*                          
*/
void CDownloadOperation::moveFileTraverse(IN LPWSTR path, IN  LPWSTR wcsExsitingFile, IN  LPWSTR wcsNewFile)
{
	USES_CONVERSION;
	WIN32_FIND_DATAW  fw;
	std::wstring      wstrpath = path;
	wstrpath                   +=L"*.*";
	HANDLE hFind= FindFirstFileW(wstrpath.c_str(), &fw);

	if(hFind == INVALID_HANDLE_VALUE)
		return ;

	do
	{
		if(wcscmp(fw.cFileName,L".") == 0 || wcscmp(fw.cFileName,L"..") == 0 || wcscmp(fw.cFileName,L".svn") == 0)
			continue;

		if(fw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			std::wstring wstrParentDirectory = path;
			wstrParentDirectory += fw.cFileName;
			wstrParentDirectory += L"\\";
	
			std::wstring wstrNewFile = wcsNewFile;
			wstrNewFile += fw.cFileName;
			wstrNewFile += L"\\";

			std::wstring wstrExistingFile = wcsExsitingFile;
			wstrExistingFile += fw.cFileName;
			wstrExistingFile += L"\\";

			CreateDirectoryW(wstrExistingFile.c_str(), NULL);
			moveFileTraverse((LPWSTR)wstrParentDirectory.c_str(), (LPWSTR)wstrExistingFile.c_str(), (LPWSTR)wstrNewFile.c_str() );
		}
		else
		{
			std::wstring newFile      = wcsNewFile;
			std::wstring existingFile = wcsExsitingFile;
			newFile      += fw.cFileName;
			existingFile += fw.cFileName;

			MoveFileExW(existingFile.c_str(), newFile.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);		
		}
	}
	while( FindNextFile(hFind,&fw) );  

	FindClose(hFind);
}