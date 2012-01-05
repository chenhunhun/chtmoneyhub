#pragma once
#include <string>
#include <atlstr.h>

#define AToW AstrToWstr
#define WToA WstrToAstr	

enum CodePage{
	CP_Ansi = CP_ACP,	// ANSI code page
	CP_Mac = CP_MACCP,	// 苹果公司mac系统 code page
	CP_Oem = CP_OEMCP,	// OEM code page
	CP_Symbol = CP_SYMBOL,	// Windows 2000/XP:Symbol code page 
	CP_Thread_Ansi = CP_THREAD_ACP, // Windows 2000/XP: Current thread's ANSI code page
	CP_Utf7 = CP_UTF7, // 转换UTF-7
	CP_Utf8 = CP_UTF8  // 转换UTF-8
};

inline std::wstring AstrToWstr(__in LPCSTR szSrc , __in int iSrcCount = -1 , __in DWORD cpCodePage = CP_UTF8 ) ;
inline std::wstring AstrToWstr(const std::string& cstr,__in DWORD cpCodePage = CP_UTF8);
inline LPCWSTR AstrToWstr( __in LPCSTR szSrc, __out LPWSTR wszDst, __in const int iDstCount, __in DWORD cpCodePage = CP_UTF8 );
inline LPCWSTR AstrToWstr( __in LPCSTR szSrc, __in const int iSrcCount , __out LPWSTR wszDst , __in const int iDstCount , __in DWORD cpCodePage = CP_UTF8 ) ;
inline LPCWSTR AstrToWstr( __in LPCSTR szSrc, __out std::wstring& wsDst, __in DWORD cpCodePage = CP_UTF8 );
inline LPCWSTR AstrToWstr( __in LPCSTR szSrc, __in const int iSrcCount , __out std::wstring& wsDst , __in DWORD cpCodePage = CP_UTF8 ) ;


inline std::string WstrToAstr(__in LPCWSTR wszSrc,__in const int iSrcCount = -1 , __in DWORD cpCodePage = CP_UTF8);
inline std::string WstrToAstr(__in const std::wstring& cwstr,__in DWORD cpCodePage = CP_UTF8);
inline LPCSTR WstrToAstr( __in LPCWSTR wszSrc, __out LPSTR pchDest,  __in const int iDstCount, __in DWORD cpCodePage =CP_UTF8 );
inline LPCSTR WstrToAstr( __in LPCWSTR wszSrc, __in const int iSrcCount, __out LPSTR pchDest,  __in const int iDstCount, __in DWORD cpCodePage =CP_UTF8 );
inline LPCSTR WstrToAstr( __in LPCWSTR wszSrc, __out std::string& sDest, __in DWORD cpCodePage = CP_UTF8);
inline LPCSTR WstrToAstr( __in LPCWSTR wszSrc, __in const int iSrcCount, __out std::string& sDest, __in DWORD cpCodePage = CP_UTF8);


// 
// // ANSIC字符串转化为UNICODE字符串
// std::wstring AstrToWstr(LPCSTR szSrc , __in DWORD cpCodePage )
// {
// 	if ( NULL == szSrc )
// 		return std::wstring() ;
// 
// 	std::wstring wsDst; 
// 	LPCWSTR l_pwstr = AstrToWstr(szSrc,-1,wsDst,cpCodePage);
// 	if ( NULL!=l_pwstr )
// 		return wsDst;
// 	else
// 		return std::wstring();
// }

std::wstring AstrToWstr(__in LPCSTR szSrc , __in int iSrcCount , __in DWORD cpCodePage )
{
	if ( NULL == szSrc )
		return std::wstring() ;

	std::wstring wsDst; 
	LPCWSTR l_pwstr = AstrToWstr(szSrc,iSrcCount,wsDst,cpCodePage);
	if ( NULL!=l_pwstr )
		return wsDst;
	else
		return std::wstring();
}

std::wstring AstrToWstr(const std::string& cstr,__in DWORD cpCodePage)
{
	return AstrToWstr(cstr.c_str(),cstr.size()+1, cpCodePage) ;
}

//************************************************************************
// 功能说明: 多字符转换为UNICODE
//
// 函数名称: AstrToWstr
//
// 访问权限: public
//
// 返回值类型: LPCWSTR	
//
// 参数1: LPCSTR	szSrc   传入参数,指向要转换的源字符串
//
// 参数2: LPWSTR	 wszDst   传出参数，字符串转换后存在它指向的内存中，指向的空间
//								必须足够大，用以存储转换后的字符串。
//
// 参数3: const DWORD iDstCount  传出参数指针l_pchDest所指向的内存块的大小
// 
// 参数4: CodePage cpCodePage 要转换的源字符串的类型,默认是CP_UTF8,其它值参见CodePage
// 
//************************************************************************
LPCWSTR AstrToWstr( __in LPCSTR szSrc, __out LPWSTR wszDst, __in const int iDstCount, __in DWORD cpCodePage )
{
	return AstrToWstr( szSrc , -1 , wszDst , iDstCount , cpCodePage ) ;
}

inline LPCWSTR AstrToWstr( __in LPCSTR szSrc, __in const int iSrcCount , __out LPWSTR wszDst , __in const int iDstCount , __in DWORD cpCodePage ) 
{
	if ( NULL == szSrc )
		return NULL ;

	int iCount = MultiByteToWideChar (cpCodePage, 0, szSrc, iSrcCount, wszDst, iDstCount);
	if ( 0==iCount )
		return NULL ;
	else
		return wszDst;

}

LPCWSTR AstrToWstr( __in LPCSTR szSrc, __out std::wstring& wsDst, __in DWORD cpCodePage)
{
	return AstrToWstr(szSrc , -1 , wsDst , cpCodePage) ;
}

LPCWSTR AstrToWstr( __in LPCSTR szSrc, __in const int iSrcCount , __out std::wstring& wsDst , __in DWORD cpCodePage ) 
{
	if ( NULL == szSrc )
		return NULL ;
	try{
		int srcCount = iSrcCount==-1?strlen(szSrc)+1:iSrcCount ;
		int dstCount = 	::MultiByteToWideChar(cpCodePage,0 , szSrc , srcCount , 0 , 0) ;
		dstCount += 10;
		wchar_t *wBuf = new wchar_t[dstCount] ;
		int iConvert = ::MultiByteToWideChar (cpCodePage, 0, szSrc, srcCount, wBuf, dstCount);
		wsDst = iConvert > 0 ?  wBuf : L"";
		delete[] wBuf;
		return wsDst.c_str ();
	}catch (...){}
	return NULL;
}

//************************************************************************
// 功能说明: UNICODE字符转为多字符
//
// 函数名称: WstrToAstr
//
// 访问权限: public
//
// 返回值类型: LPCSTR	转换后的字符串，如果失败返回 NULL
//
// 参数1: LPCWSTR cpwchSource   传入参数,指向要转换的UNICODE源字符串
//
// 参数2: LPSTR	 pchDest   传出参数，字符串转换后存在它指向的内存中，指向的空间
//								必须足够大，用以存储转换后的字符串。
//
// 参数3: const DWORD cdwDestSize  传出参数指针l_pchDest所指向的内存块的大小
// 
// 参数4: CodePage cpCodePage 要转换的源字符串的类型,默认是CP_Ansi,其它值参见CodePage
// 
//************************************************************************
LPCSTR WstrToAstr( __in LPCWSTR wszSrc, 
	__out LPSTR szDst, 
	__in const int iDstCount, 
	__in DWORD cpCodePage )
{
	return WstrToAstr(wszSrc , -1 , szDst , iDstCount , cpCodePage) ;
}

LPCSTR WstrToAstr( __in LPCWSTR wszSrc, __in const int iSrcCount, __out LPSTR szDst,  __in const int iDstCount, __in DWORD cpCodePage )
{
	if ( NULL == wszSrc )
		return NULL ;

	int iCount = WideCharToMultiByte(cpCodePage,0,wszSrc,iSrcCount,szDst,iDstCount,NULL,FALSE);
	if ( 0==iCount )
		return NULL ;
	else
		return szDst;
}


//************************************************************************
// 功能说明: UNICODE字符转为多字符
//
// 函数名称: WstrToAstr
//
// 访问权限: public
//
// 返回值类型: LPCSTR	
//
// 参数1: LPCWSTR cpwchSource   传入参数,指向要转换的UNICODE源字符串
//
// 参数2: std::wstring& sDest 储存转换后的结果
// 
// 参数4: CodePage cpCodePage 要转换的源字符串的类型,默认是CP_Ansi,其它值参见CodePage
// 
//************************************************************************
LPCSTR WstrToAstr( __in LPCWSTR wszSrc, 
	__out std::string& sDest, 
	__in DWORD cpCodePage )
{
	return WstrToAstr(wszSrc , -1 , sDest , cpCodePage) ;
}

LPCSTR WstrToAstr( __in LPCWSTR wszSrc, __in const int iSrcCount, __out std::string& sDest, __in DWORD cpCodePage)
{
	if ( NULL == wszSrc )
		return NULL ;

	// modify by wujian
	// 某一个主框架的崩溃发生在这，初步怀疑是因为buffer开的不够大导致
	// 所以开大一些，然后转码后补个0
	try{
		int srcCount = iSrcCount==-1?wcslen(wszSrc)+1:iSrcCount ;
		int dstCount = ::WideCharToMultiByte(cpCodePage , 0 , wszSrc , iSrcCount , 0 , 0 , 0 ,0) ;
		dstCount += 10;
		char *buf = new char[dstCount];
		int iConvert = ::WideCharToMultiByte(cpCodePage,0,wszSrc,srcCount,buf,dstCount,NULL,FALSE);
		sDest = iConvert > 0 ? buf : "";
		delete[] buf;
		return sDest.c_str ();
	}catch (...){}
	return NULL;
}


//************************************************************************
// 功能说明：把UNICODE字符串转化为ANSI字符串
//
// 函数名称: WStrToAStr
//
// 访问权限: public
//
// 返回值类型: std::string	返回转换后的ANSI字符串
//
// 参数1: LPCTSTR wszSrc	要转换的字符串
//
//************************************************************************
// 把UNICODE字符串转化为ANSI字符串

std::string WstrToAstr(__in LPCWSTR wszSrc,__in const int iSrcCount , __in DWORD cpCodePage)
{
	if ( NULL == wszSrc )
		return std::string() ;

	std::string l_str;
	LPCSTR l_pstr = WstrToAstr(wszSrc,iSrcCount,l_str,cpCodePage);
	if ( NULL!=l_pstr )
		return l_str;
	else
		return std::string();
}

std::string WstrToAstr(__in const std::wstring& cwstr,__in DWORD cpCodePage)
{
	return WstrToAstr(cwstr.c_str(),cwstr.size()+1,cpCodePage);
}
