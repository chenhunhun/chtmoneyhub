#include "StdAfx.h"
#include "FavBankOperator.h"
#include"../BankData/BankData.h" // gao 2010-12-14
#include "../../ThirdParty/tinyxml/tinyxml.h"

std::map<std::string, std::string> CFavBankOperator::m_mapBankNameAndID;
CFavBankOperator::CFavBankOperator(void)
{
	ReadFavBankID (m_setFavBankID);
}

CFavBankOperator::~CFavBankOperator(void)
{
}


// gao 2010-12-14
// 修改程序启动时加载银行所有驱动的BUG
bool CFavBankOperator::SeparateStringByChar(std::set<std::string>& setStor, const std::string& strSour, char chSep)
{
	if (strSour.length () <= 0)
		return false;

	std::string strT;

	// 将字符串按chSep字符分隔开
	for (std::set<std::string>::size_type i = 0; i < strSour.length (); i ++)
	{
		if (chSep == strSour[i]) // 当是分隔符时，添加到容器中
		{
			setStor.insert (strT);
			strT.clear ();
			continue;
		}

		strT += strSour[i];

		if (i == strSour.length () - 1)
		{
			setStor.insert (strT);
			strT.clear ();
		}
	}

	return true;
}

std::string CFavBankOperator::GetBankIDOrBankName(const std::string& strCondition, bool bGetBName)
{
	ATLASSERT (!strCondition.empty ());
	if (strCondition.empty ())
		return "";

	if (m_mapBankNameAndID.size () <= 0)
	{
		ReadBankConfigData ();
	}

	std::map<std::string,std::string>::const_iterator itFind;
	if (bGetBName)
	{
		itFind = m_mapBankNameAndID.find (strCondition);
		if (itFind == m_mapBankNameAndID.end ())
			return "";
		return itFind->second;
	}
	else
	{
		itFind = m_mapBankNameAndID.begin ();
		// 银行不可能会出现重名的情况
		for (; itFind != m_mapBankNameAndID.end (); itFind ++)
		{
			if (strCondition == itFind->second)
				break;
		}
		
		if (itFind == m_mapBankNameAndID.end ())
			return "";

		return itFind->first;
	}

}

bool CFavBankOperator::ReadFavBankID(std::set<std::string>& setBankID)
{
	setBankID.clear ();

	CBankData* pBankData = CBankData::GetInstance();
	ATLASSERT (NULL != pBankData);
	if (NULL == pBankData)
		return false;

	std::string strBankID;
	pBankData->GetFav (strBankID);
	strBankID = strBankID.substr (1, strBankID.length ());
	strBankID = strBankID.substr (0, strBankID.length () - 1);
	if (strBankID.empty ())
		return false;

	std::set<std::string> setBankItem;
	SeparateStringByChar (setBankItem, strBankID, '}');

	if (setBankItem.size () < 0)
		return false;

	std::set<std::string>::const_iterator it;
	for (it = setBankItem.begin (); it != setBankItem.end (); it ++)
	{
		std::string strTemp = *it;
		std::string strBkID;
	
		if (ReadBankIDFromBankItem (strTemp, strBkID))
			setBankID.insert (strBkID);
		
	}

	return true;
}

// gao 2010-12-17
bool CFavBankOperator::IsInFavBankSet(const std::string& strChk)
{ 	
	std::set<std::string>::const_iterator begin;
	begin = m_setFavBankID.begin ();

	// 遍历所有项
	for (; begin != m_setFavBankID.end (); begin ++)
	{
		if (strChk == *begin)
			return true;
	}

	return false;
}

void CFavBankOperator::UpDateFavBankID(std::string& strBkID, BOOL bAdd)
{
	if (bAdd)
		m_setFavBankID.insert (strBkID);
	else
		m_setFavBankID.erase(strBkID);
}

// 用来比较新收藏的BankID和以前收藏的BankID是否相同，如果返回值大于0，表示增加了收藏，如果小于0，表示删除收藏
// strDif返回增加或删除的BankID
int CFavBankOperator::FavBankCompare2Old(const std::set<std::string>& setStor, std::string& strDif)
{
	std::set<std::string> setDif;
	int nBack = CompareTwoSet (setStor, m_setFavBankID, setDif);
	strDif = *(setDif.begin ());
	return nBack;
}

// 如果返回值大于0，表示增加了收藏，如果小于0，表示删除收藏, =0表示一样
int CFavBankOperator::CompareTwoSet(const std::set<std::string>& setF, const std::set<std::string>& setSec, std::set<std::string>& setDiff)
{
	setDiff.clear ();
	int nBack = 0;
	std::set<std::string> Temp;
	if (setF.size () < setSec.size ())
	{
		setDiff = setSec;
		Temp = setF;
		nBack = -1;
	}
	else
	{
		setDiff = setF;
		Temp = setSec;
		nBack = 1;
	}

	std::set<std::string>::iterator itTemp;
	for (itTemp = Temp.begin (); itTemp != Temp.end (); itTemp ++)
	{
		std::set<std::string>::iterator itIn;
		for (itIn = setDiff.begin (); itIn != setDiff.end (); itIn ++)
		{
			if (*itIn == *itTemp)
			{
				setDiff.erase (itIn);
				break;
			}
		}
	}

	return nBack;

}

//void CFavBankOperator::MyTranslateBetweenBankIDAndInt(std::string& strBankID, int& nBankID, bool bToInt)
//{
//	if (bToInt)
//	{
//		std::string strFav = strBankID;
//		nBankID = strFav[0];
//		strFav = strFav.substr (1, strFav.length ());
//		nBankID *= 1000;
//		nBankID += atoi (strFav.c_str ());
//		
//	}
//	else
//	{
//		int nTp = nBankID;
//		int nSub = nTp % 1000;
//		nTp = nTp / 1000;
//		
//		CString strBk;
//		strBk.Format (L"%03d", nSub);
//
//		strBankID += nTp;
//		strBankID += CW2A (strBk);
//	}
//}

// 从一个结构中读取bankID
bool CFavBankOperator::ReadBankIDFromBankItem(const std::string& strBankItem, std::string& bankID)
{
	std::string strSep = "\"id\":\"";
	if (strBankItem.find (strSep) < 0)
		return false;

	bankID = strBankItem.substr (strBankItem.find (strSep) + strSep.length (), strBankItem.length ());
	bankID = bankID.substr (0, 4);
	return true;
}

bool CFavBankOperator::ReadBankConfigData()
{
	std::string strSQL = "select id, ShortName from sysBank";
	std::string ret = CBankData::GetInstance()->QuerySQL(strSQL, "SysDB");

	int nLen = ret.size();
	int nPos = 0;
	//bool bFirst = false;
	std::string strID, strName;
	while(nPos < nLen)
	{
		if (nPos + 5 < nLen)
		{
			if(strncmp(ret.c_str() + nPos, "id\":\"", 5) == 0)
			{
				nPos += 5;
				while(strncmp(ret.c_str() + nPos, "\"", 1) != 0 && nPos < nLen)
				{
					strID += *(ret.c_str() + nPos);
					nPos ++;
					
				}		
			}
			else if(strncmp(ret.c_str() + nPos, "me\":\"", 5) == 0)
			{
				nPos += 5;
				while(strncmp(ret.c_str() + nPos, "\"", 1) != 0 && nPos < nLen)
				{
					strName += *(ret.c_str() + nPos);
					nPos ++;
				}

				if (!strID.empty())
				{
					m_mapBankNameAndID.insert (std::make_pair (strID, strName));
					strID.clear();
					strName.clear();
				}
			}
		}
		nPos ++;

		
	}
	return true;
}

//void CFavBankOperator::ReadConfigFilePath(std::wstring& strPath)
//{
//	TCHAR szPath[MAX_PATH];
//
//    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
//    {
//		ATLASSERT (false);
//        return;
//    }
//
//	strPath = szPath;
//	size_t nIndex = strPath.rfind ('\\');
//	ATLASSERT (nIndex >= 0 && nIndex < strPath.length ());
//	if (nIndex < 0 || nIndex >= strPath.length ())
//		return;
//
//	strPath = strPath.substr (0, nIndex);
//	if (strPath.rfind ('\\') != strPath.length () - 1)
//		strPath += L"\\";
//
//	strPath += _BANKIDMAP_SUB_DIRECTORY;
//
//}
//
//bool CFavBankOperator::ReadBankConfigXml()
//{
//	std::wstring strFileName;
//	ReadConfigFilePath (strFileName);
//
//	HANDLE hFile = CreateFile(strFileName.c_str (), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (hFile == INVALID_HANDLE_VALUE)
//	{
//		return false;
//	}
//
//	DWORD dwLowSize = GetFileSize(hFile, NULL);
//	if (dwLowSize == INVALID_FILE_SIZE)
//	{
//		CloseHandle(hFile);
//
//		return false;
//	}
//
//	DWORD dwRead = 0;
//	char* pContent = new char[dwLowSize + 1];
//	if (!ReadFile(hFile, pContent, dwLowSize, &dwRead, NULL))
//	{
//		delete[] pContent;
//		CloseHandle(hFile);
//		return false;
//	}
//
//	CloseHandle(hFile);
//	*(pContent+dwLowSize) = '\0';
//
//	TiXmlDocument xmlDoc;
//	xmlDoc.Parse(pContent); // 参数是文件内容
//	if (xmlDoc.Error())
//		return false;
//
//	const TiXmlNode* pRoot = xmlDoc.FirstChild("moneyhub"); // ANSI string 
//	if (NULL == pRoot)
//		return false;
//
//	const TiXmlNode* pCategory = pRoot->FirstChild("category");
//	const TiXmlElement* pCategoryElement = pCategory->ToElement ();
//	std::string strCateName;
//	if (pCategoryElement->Attribute("name"))
//			strCateName = pCategoryElement->Attribute("name");
//	
//	if ("config" != strCateName)
//		return false;
//
//	const char* pText = NULL;
//
//	for (const TiXmlNode *pNode = pRoot->FirstChild("node"); pNode != NULL; pNode = pRoot->IterateChildren("node", pNode))
//	{
//		const TiXmlNode* pID = pNode->FirstChild ("ID");
//		const TiXmlElement* pIDElement = pID->ToElement ();
//		if (NULL == pIDElement)
//			continue;
//
//		pText = pIDElement->GetText ();
//		std::string strID = pText;
//
//		const TiXmlNode* pName = pNode->FirstChild ("name");
//		const TiXmlElement* pNameElement = pName->ToElement ();
//		pText = pNameElement->GetText ();
//		std::string strName = pText;
//
//		m_mapBankNameAndID.insert (std::make_pair (strID, strName));
//
//	}
//
//
//	delete[] pContent;
//
//	return true;
//}