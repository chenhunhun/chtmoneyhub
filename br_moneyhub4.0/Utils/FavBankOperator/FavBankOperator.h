#pragma once
#include <map>
class CFavBankOperator
{
public:
	CFavBankOperator(void);
	~CFavBankOperator(void);
	
	// gao 2010-12-13
	// 用来检验strChk是否存在容器中
	bool IsInFavBankSet(const std::string& strChk);

	void UpDateFavBankID(std::string& strBkID, BOOL bAdd = true);

	// 临时添加用来测试
	static std::string GetBankIDOrBankName(const std::string& strCondition, bool bGetBName = true);

	// 用来比较新收藏的BankID和以前收藏的BankID是否相同，如果返回值大于0，表示增加了收藏，如果小于0，表示删除收藏, =0表示一样
	// strDif返回增加或删除的BankID
	int FavBankCompare2Old(const std::set<std::string>& setStor, std::string& strDif);

	// 暂时只用来做BankID和int之间的转换，sendmessage比较好用
//	static void MyTranslateBetweenBankIDAndInt(std::string& strBankID, int& nBankID, bool bToInt = true);

	bool ReadFavBankID(std::set<std::string>& setBankID);
private:
	// 如果返回值大于0，表示增加了收藏，如果小于0，表示删除收藏, =0表示一样
	int CompareTwoSet(const std::set<std::string>& setF, const std::set<std::string>& setSec, std::set<std::string>& setDiff);


	// 从一个结构中读取bankID
	bool ReadBankIDFromBankItem(const std::string& strBankItem, std::string& bankID);

	// 读取数据库中的银行ID和银行名称
	static bool ReadBankConfigData();

private:
	std::set<std::string> m_setFavBankID; // 用来记录用户收藏了的银行ID // gao 2010-12-17
	static std::map<std::string, std::string> m_mapBankNameAndID;
};
