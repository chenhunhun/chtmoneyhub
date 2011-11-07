#include <windows.h>
#include "libbios.h"

typedef struct _UNICODE_STRING 
{ 
    USHORT Length;			//长度 
    USHORT MaximumLength;	//最大长度 
    PWSTR Buffer;			//缓存指针 
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES 
{ 
    ULONG Length;					//长度 18h 
    HANDLE RootDirectory;			// 00000000 
    PUNICODE_STRING ObjectName;		//指向对象名的指针 
    ULONG Attributes;				//对象属性00000040h 
    PVOID SecurityDescriptor;		// Points to type SECURITY_DESCRIPTOR，0 
    PVOID SecurityQualityOfService;	// Points to type SECURITY_QUALITY_OF_SERVICE，0 
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

//函数指针变量类型
typedef DWORD  (__stdcall *ZWOS )( PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES); 
typedef DWORD  (__stdcall *ZWMV )( HANDLE,HANDLE,PVOID,ULONG,ULONG,PLARGE_INTEGER,PSIZE_T,DWORD,ULONG,ULONG); 
typedef DWORD  (__stdcall *ZWUMV )( HANDLE,PVOID);

typedef UINT(WINAPI * pGetSystemFirmwareTable) ( DWORD FirmwareTableProviderSignature,
												DWORD FirmwareTableID,
												PVOID pFirmwareTableBuffer,
												DWORD BufferSize);

UINT FindAmiBios( BYTE** ppBiosAddr );
UINT FindAwardBios( BYTE** ppBiosAddr );
UINT FindPhoenixBios( BYTE** ppBiosAddr );

/*DWORD MapBiosToDword(char *bios)
{
	DWORD result = 0;
	char *p = bios + lstrlen(bios) -1 ;
	for (int i=0;i<5 && p >= bios ;i++,p--)
	{
		unsigned int c = (unsigned int)(*p);
		if (c >='0' && c<='9')
			c = c - '0' ;   // 数字占用 0-9
		else if (c >='A' && c <= 'Z')
			c = c - 'A' + 10 ;  // 大写字母占用 10-36
		else if (c >='a' && c <= 'z')
			c = c - 'a' + 10 ;  // 小写字母同样占用 10-36
		else
			c = 0; // 除了大写字母和数字的都赋值为 0 
		result += c;
		result <<= 6; // 6 bit 
	}
	return result;
}*/

BOOL isWin98()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx( &osvi );
	if (osvi.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS) //win95/win98/winMe
	{
		return TRUE;
	}
	return FALSE;
}

bool GetBIOS(char *buf, size_t len, size_t *filled)
{
	bool result       = false;
	BYTE* pBiosSerial = 0;
	SIZE_T ssize      = 0xffff; 

	if (isWin98())
	{
		pBiosSerial = (BYTE*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, ssize );
		if (pBiosSerial)
			CopyMemory(pBiosSerial, (BYTE*)0xf0000, ssize);
	}
	else
	{
		LARGE_INTEGER so; 
		so.LowPart        = 0x000f0000;
		so.HighPart       = 0x00000000; 
		wchar_t strPH[30] = L"\\device\\physicalmemory"; 
		DWORD ba          = 0;
		
		UNICODE_STRING struniph; 
		struniph.Buffer   = strPH; 
		struniph.Length   = 0x2c; 
		struniph.MaximumLength = 0x2e; 
		
		OBJECT_ATTRIBUTES obj_ar; 
		obj_ar.Attributes = 64;
		obj_ar.Length     = 24;
		obj_ar.ObjectName = &struniph;
		obj_ar.RootDirectory            = 0; 
		obj_ar.SecurityDescriptor       = 0; 
		obj_ar.SecurityQualityOfService = 0; 

		HMODULE hinstLib = LoadLibraryA("ntdll.dll"); 
		ZWOS ZWopenS   = (ZWOS)GetProcAddress(hinstLib,"ZwOpenSection"); 
		ZWMV ZWmapV    = (ZWMV)GetProcAddress(hinstLib,"ZwMapViewOfSection"); 
		ZWUMV ZWunmapV = (ZWUMV)GetProcAddress(hinstLib,"ZwUnmapViewOfSection"); 

		//调用函数，对物理内存进行映射 
		HANDLE hSection; 
		if( ZWopenS && ZWmapV && ZWunmapV && 0==ZWopenS(&hSection,4,&obj_ar) && 
			0==ZWmapV( 
			( HANDLE )hSection,   //打开Section时得到的句柄 
			( HANDLE )0xFFFFFFFF, //将要映射进程的句柄， 
			&ba,                  //映射的基址 
			0,
			0xFFFF,               //分配的大小 
			&so,                  //物理内存的地址 
			&ssize,               //指向读取内存块大小的指针 
			1,                    //子进程的可继承性设定 
			0,                    //分配类型 
			2                     //保护类型 
			) )
			//执行后会在当前进程的空间开辟一段64k的空间，并把f000:0000到f000:ffff处的内容映射到这里 
			//映射的基址由ba返回,如果映射不再有用,应该用ZwUnmapViewOfSection断开映射 
		{
			pBiosSerial = (BYTE*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, ssize );
			if (pBiosSerial)
				CopyMemory(pBiosSerial, (BYTE*)ba, ssize);
			ZWunmapV( ( HANDLE )0xFFFFFFFF, ( void* )ba );
		}
		else
		{
			//上面的方法是win2k和winxp起作用的，从win2003 sp1到vista不再可用
			//这里改用vista新的系统API做尝试
			HINSTANCE m_hInst = LoadLibraryA("Kernel32.dll");
			if (m_hInst >= (HINSTANCE) HINSTANCE_ERROR)
			{
				pGetSystemFirmwareTable m_GetSystemFirmwareTable = (pGetSystemFirmwareTable) GetProcAddress(m_hInst, "GetSystemFirmwareTable");
				
				if (m_GetSystemFirmwareTable)
				{
					BYTE *pBuf = NULL;
					DWORD dwLen = 0;
					UINT ret = m_GetSystemFirmwareTable('FIRM', 0xE0000, pBuf, dwLen);
					if (ret)
					{
						DWORD dwError = GetLastError();
						if (dwError == ERROR_INSUFFICIENT_BUFFER)
						{
							dwLen = ret;
							pBuf = (PBYTE)LocalAlloc(0, ret);
							if (pBuf)
							{
								ret = m_GetSystemFirmwareTable('FIRM', 0xE0000, pBuf, dwLen);
								if (ret && ret<=dwLen && ret>=0x1ffff)
								{
									pBiosSerial = (BYTE*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, ssize );
									if (pBiosSerial)
										CopyMemory(pBiosSerial, pBuf+0x10000, ssize);
								}
								LocalFree(pBuf);
							}
						}
					}
				}
				FreeLibrary(m_hInst);
			}
		}
	}

	if (pBiosSerial)
	{
		BYTE *pBiosSerialStep = pBiosSerial;
		UINT uBiosSerialLen = FindAwardBios( &pBiosSerialStep );
		if( uBiosSerialLen == 0U )
		{
			uBiosSerialLen = FindAmiBios( &pBiosSerialStep );
			if( uBiosSerialLen == 0U )
			{
				uBiosSerialLen = FindPhoenixBios( &pBiosSerialStep );
			}
		}

		if( uBiosSerialLen != 0U )
		{
			UINT minlen = uBiosSerialLen<len ? uBiosSerialLen:len;
			if (minlen>0)
			{
				MoveMemory( buf, pBiosSerialStep, minlen );
				if (filled)
					*filled = minlen;
				result = true;
			}
		}
		HeapFree(GetProcessHeap(), 0, pBiosSerial);
	}
	return result;
}


UINT FindAwardBios( BYTE** ppBiosAddr )
{
    BYTE* pBiosAddr = * ppBiosAddr + 0xEC71;

    BYTE szBiosData[128] = {0};
    CopyMemory( szBiosData, pBiosAddr, 127 );
    szBiosData[127] = 0;
    
    int iLen = lstrlenA( ( char* )szBiosData );
    if( iLen > 0 && iLen < 128 )
    {
        //AWard:         07/08/2002-i845G-ITE8712-JF69VD0CC-00 
        //Phoenix-Award: 03/12/2002-sis645-p4s333
        if( szBiosData[2] == '/' && szBiosData[5] == '/' )
        {
            BYTE* p = szBiosData;
            while( * p )
            {
                if( * p < ' ' || * p >= 127 )
                {
                    break;
                }
                ++ p;
            }
            if( * p == 0 )
            {
                * ppBiosAddr = pBiosAddr;
                return ( UINT )iLen;
            }
        }
    }
    return 0;
}

UINT FindAmiBios( BYTE** ppBiosAddr )
{
    BYTE* pBiosAddr = * ppBiosAddr + 0xF478;
    
    BYTE szBiosData[128] = {0};
    CopyMemory( szBiosData, pBiosAddr, 127 );
    szBiosData[127] = 0;
    
    int iLen = lstrlenA( ( char* )szBiosData );
    if( iLen > 0 && iLen < 128 )
    {
        // Example: "AMI: 51-2300-000000-00101111-030199-"
        if( szBiosData[2] == '-' && szBiosData[7] == '-' )
        {
            BYTE* p = szBiosData;
            while( * p )
            {
                if( * p < ' ' || * p >= 127 )
                {
                    break;
                }
                ++ p;
            }
            if( * p == 0 )
            {
                * ppBiosAddr = pBiosAddr;
                return ( UINT )iLen;
            }
        }
    }
    return 0;
}

UINT FindPhoenixBios( BYTE** ppBiosAddr )
{
    UINT uOffset[3] = { 0x6577, 0x7196, 0x7550 };
    for( UINT i = 0; i < 3; ++ i )
    {
        BYTE* pBiosAddr = * ppBiosAddr + uOffset[i];

        BYTE szBiosData[128] = {0};
        CopyMemory( szBiosData, pBiosAddr, 127 );
        szBiosData[127] = 0;

        int iLen = lstrlenA( ( char* )szBiosData );
        if( iLen > 0 && iLen < 128 )
        {
            // Example: Phoenix "NITELT0.86B.0044.P11.9910111055"
            if( szBiosData[7] == '.' && szBiosData[11] == '.' )
            {
                BYTE* p = szBiosData;
                while( * p )
                {
                    if( * p < ' ' || * p >= 127 )
                    {
                        break;
                    }
                    ++ p;
                }
                if( * p == 0 )
                {
                    * ppBiosAddr = pBiosAddr;
                    return ( UINT )iLen;
                }
            }
        }
    }
    return 0;
}