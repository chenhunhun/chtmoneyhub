#pragma once
using namespace std;



enum SystemType
{
	sAll = 0,
	sUnSupported = 1,
	sWinNT = 2,
	sWin2000 = 3,
	sWinXP = 4,
	sWin2003 = 5,
	sWinVista = 6,
	sWin2008 = 7,
	sWin7 = 8
};
// 在程序中用到的usb设备的特征值
struct USBEigenvalue
{
	USBEigenvalue()
	{
		hasMid = FALSE;
	}
	~USBEigenvalue()
	{
	}
	USHORT  VendorID;
	USHORT  ProductID;
	USHORT  VersionNumber;//里面包含vid、pid
	string  SerierlNumber; //里面记录sn的数据

	// MID值通过指定目录下的dll的函数接口获得，这个需要厂家直接提供，我们将他们提供的做法统一定制为dll接口
	// 值类型
	bool	hasMid;//记录是否含有mid
	// 值数据存储位置
	DWORD m_midDword;

	wstring midDLLName;
	wstring midFucName;	
};

struct USBHardwareInfo
{
	USBHardwareInfo()
	{
		bNeedInstall = false;
		bNeedForbidRun = false;
	}

	~USBHardwareInfo()
	{
	}
	USBEigenvalue eig;	

	wstring		version;//硬件版本信息
	wstring		manufacturer;//生产商名称
	wstring		goodsname;//商品名称
	string		volumnname;//卷标

	wstring		inmodel;//内部编号
	wstring		model;//型号
	wstring		finaninstitution;//金融机构名称
	string		financeid;//金融机构编号存为string类型是为了与数据库中一致，不再进行转换
	bool		bNeedInstall;//是否需要我们自己对驱动软件信息进行安装，如果USBKey已经进行了自安装，我们就不用管了
	bool		bNeedForbidRun;//是否需要将该usb设备的自动运行功能去掉
};
//记录安装关键程序信息
enum ProgramType
{
	pService = 0,	//服务进程
	pNormal = 1,	//一般程序
	pDriver = 2,	//需要该驱动
};
struct  RegInfo
{
	HKEY rootkey;
	wstring subkey;
};
struct USBSoftwareInfo
{
	SystemType		ostype;								//	记录操作系统类型
	int				osarchite;							//	记录操作系统架构，32,64
	bool			bNeedUpdate;						//	记录是否有自己升级的功能
	bool			bNeedReboot;						//  记录是否需要重启系统
	wstring			url;								//	下载安装包的url
	wstring			backurl;							//	备份下载地址
	DWORD			version;							//	软件版本
	list<RegInfo>	reginfo;							//	需要的注册表信息,只检测注册表关键键的存在与否，否则太复杂
	std::list<std::wstring>  files;						//	需要的程序文件,安装之后必有的文件
	std::map<std::wstring,ProgramType> programinfo;		//	需要的信息，filename记录全路径
};

struct USBKeyInfo
{
	USBHardwareInfo hardware;	//硬件信息是唯一的
	USBSoftwareInfo software;	//软件根据系统是不同的
	bool bSupport;				//记录当前系统是否支持该usb设备
};