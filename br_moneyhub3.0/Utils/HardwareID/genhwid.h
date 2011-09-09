#pragma once

#pragma pack(push,1)

#include <string>

//typedef unsigned char HDSERIAL[6];

typedef struct _HWIDSTRUCT
{
	DWORD dw1;
	DWORD dw2;
	DWORD dw3;
	DWORD dw4;
} HWIDSTRUCT, *PHWIDSTRUCT;
#pragma pack(pop)

extern "C" bool GenHWID(HWIDSTRUCT &id);
extern std::string GenHWID2(bool bUrlEncode=true);
extern std::string aes_encode(std::string info);
extern void InitHardwareId();
