//这个是调用usb模块的接口进行usb检测的类
#include "../../USBControl/USBControl.h"
#include "../../USBControl/USBCheckor.h"
#include "../../USBControl/info.h"
#include "BkInfoDownload.h"
#include <string>

class CBankDownInterface:public IBankDownInterface
{
public:
	bool CheckServerXmlFile(bool bUpdate,  USBRECORD& ur);
	bool USBFinalTest(int vid, int pid, DWORD mid, bool& bSetup);
};


class CUSBMonitor:public IUSBTaskInterface
{
private:
	CUSBMonitor();
	~CUSBMonitor();

	static CUSBMonitor* m_Instance;
public:
	void InitParam();
	static CUSBMonitor* GetInstance();
private:
public:
	bool StartUSBMonitor();

	bool AddUSBTask(int vid, int pid, DWORD mid);
};