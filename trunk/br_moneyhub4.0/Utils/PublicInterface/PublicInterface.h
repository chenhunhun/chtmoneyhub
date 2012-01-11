#pragma once
using namespace std;

namespace PublicInterface
{
	// 将字符串strSour按分隔字符串截取，并保存到setStor容器中， strSep是strSour的分隔字符串
	bool SeparateStringBystr(std::vector<std::string>& setStor, std::string strSour, const std::string& strSep);
}