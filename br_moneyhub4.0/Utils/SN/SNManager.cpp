#include "stdafx.h"
#include "SNManager.h"

#define REGEDIT_MONHUB_SN "Software\\Finantech\\MoneyHub\\Setting"
#define MONHUB_SN_KEY	"InstallSN"
#define SN_LENGTH 20

#define CRC16_POLY          0x8005 
#define CRC_INIT    0x0 

CSNManager* CSNManager::m_Instance = NULL;

CSNManager* CSNManager::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CSNManager();
	return m_Instance;
}

CSNManager::CSNManager()
{
	m_sn.clear();

	CHAR ptszValueName[50] = { 0 };
	DWORD dType, dReturnBytes = 50;

	// 在注册表中读取看以前是否存在SN
	if (ERROR_SUCCESS == ::SHGetValueA(HKEY_LOCAL_MACHINE, REGEDIT_MONHUB_SN,MONHUB_SN_KEY, &dType, &ptszValueName, &dReturnBytes))
	{
		if(dReturnBytes == SN_LENGTH + 1)
		{
			char sn[SN_LENGTH];
			memcpy(sn,ptszValueName,SN_LENGTH);

			//查校验位是否正确
			MakeCheckCode((char *)sn);
			if(memcmp(ptszValueName + SN_LENGTH - 4,sn + SN_LENGTH - 4, 4) == 0)
			{			
				string strsn(ptszValueName);
				m_sn = strsn;
			}

		}
	}
	
}
CSNManager::~CSNManager()
{

}

// 向注册表写入SN，生成SN的过程
void CSNManager::MakeSN()
{
	// 获得随机种子，产生一个16位的随机数和一个4位的CRC校验数值
	int i = 0;
	char sn[SN_LENGTH + 1] = {0};
	srand((unsigned)time(NULL));
	int scope = 10;
	while( i <  (SN_LENGTH - 4))
	{
		sn[i] = rand() % scope + '0';// 产生0-9的随机数
		i ++;
	}
	MakeCheckCode((char *)sn);	

	// 写入注册表
	if (ERROR_SUCCESS == ::SHSetValueA(HKEY_LOCAL_MACHINE, REGEDIT_MONHUB_SN,MONHUB_SN_KEY, REG_SZ, sn, SN_LENGTH))
	{
		string tp(sn);
		m_sn = sn;
		return;
	}
}
// 生成校验码
void CSNManager::MakeCheckCode(char *sn)
{
	// 生成校验值，每4位生成一个校验位，然后取10的余数作为校验码
	int  i = 0;
	int checksum[4];
	for(; i < 4 ;i ++)
	{
		checksum[i] = GenCRC16((char *)sn + i * 4, 4);
		sn[SN_LENGTH - 4 + i] = checksum[i] % 10 + '0';
	}
}
	// 向注册表获得SN的过程，如果不存在，返回空字符串
string CSNManager::GetSN()
{
	if(m_sn == "")
		MakeSN();// 生成一个SN
	return m_sn;
}

unsigned short CSNManager::GenCRC16(const char *pdata, int size) 
{ 
	int i,j; 
	char Data; 

	unsigned short checksum = CRC_INIT; 
	for(i = 0; i < size; i++)
	{ 
		Data = *pdata ++; 
		for (j = 0; j < 8; j++) 
		{ 
			if (((checksum & 0x8000) >> 8) ^ (Data & 0x80)) 
				checksum= (checksum << 1) ^ CRC16_POLY; 
			else 
				checksum = (checksum << 1); 

			Data <<= 1; 
		} 
	} 

	return checksum; 
} 
