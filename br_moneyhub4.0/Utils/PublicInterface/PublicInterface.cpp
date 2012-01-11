#include "stdafx.h"
#include "PublicInterface.h"

// 将字符串strSour按分隔字串截取，并保存到setStor容器中， strSep是strSour的分隔字符串
bool PublicInterface::SeparateStringBystr(std::vector<std::string>& setStor, std::string strSour, const std::string& strSep)
{
	if (strSour.length () <= 0 || strSep.length() < 0)
		return false;

	int nIndex;

	while((nIndex = strSour.find(strSep)) != string::npos)
	{
		string strT = strSour.substr(0, nIndex);
		strSour = strSour.substr(nIndex + strSep.length(), strSour.length());
		setStor.push_back(strT);
	}

	return true;
}