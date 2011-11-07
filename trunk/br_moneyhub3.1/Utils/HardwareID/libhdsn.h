#pragma once

extern bool GetFirstHDSerial_NT_PhysicalDriveAdmin(char *buf, size_t len);

extern bool GetFirstHDSerial_NT_PhysicalDriveZeroRight(char *buf, size_t len);

extern bool GetFirstHDSerial_NT_IDEasSCSI(char *buf, size_t len);

extern bool GetFirstHDSerial_9X_SMART(char *buf, size_t len);

extern bool GetFirstHDSerial(char *buf, size_t len);

#ifdef USE_PORT_DRIVER
extern bool GetFirstHDSerial_9X_Ports(char *buf, size_t len);
#endif 

extern DWORD MapSerialToDword(char *serial);