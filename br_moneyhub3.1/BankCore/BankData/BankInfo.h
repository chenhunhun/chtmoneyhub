#include <map>
#include <list>
using namespace std;

// 记录任务类型
enum TaskType
{
	ttActiveX,
	ttUSB
};
class TaskInfo
{
	TaskType m_type;	//任务的类型，控件或者usb

	string m_address;	//下载地址
	
	int m_process;		//下载任务的进度

	int vid;
	int pid;
	DWORD mid;
	string 
};
class BankControl
{
	// 注意同步
	int process;// 总的下载进度
	// 这里要增加一个算法把银行下面的下载进度计算出来
	// 等待所有的下载完成然后集中提示usbkey可以使用

	list<TaskInfo> banktask; //所有的下载任务

	void AddTask();//增加一个任务

	void DelTask();// 删除一个任务
};
class CBankTask
{
private:
	map<string appId, BankControl bankctl>  m_banktask;// 根据银行的id记录所有的信息

	HWND m_homepage;
public:
	void AddTask(string appId, TaskInfo task);// 执行插入一条新任务的处理

	void DelTask(string appId);	//删除任务 ，删除下面所有的任务

	void PauseTask(string appId);	//暂停任务

	list<TaskInfo>& GetTask(string appId);//根据appId 获得现有appid下的所有任务信息

	void SetDisplay(HWND home)
	{
		m_homepage = home;
	}

private:

	void SetDisplayInfo(string appId, )

};

// 这个类用来存储所有正在更新的下载控制类
class CBankInfo
{
private:
	CBankTask m_task;
public:
	void AddUSB(int vid, int pid, DWORD mid);//增加一个usbkey的下载任务
	void AddBankActiveX(string appId);//增加一个银行控件的下载任务

	void SetDisplay(HWND home)
	{
		m_task.SetDisplay(home);
	}
};

extern CBankInfo g_bankinfo;