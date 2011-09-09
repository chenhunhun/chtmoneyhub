/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  AES.h
 *      说明：  AES加密
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.10.22	融信恒通	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */
#pragma once

class CAES
{
	unsigned char* m_key;
	long m_keylen;

	unsigned char* m_output;
	long m_outlen;

public:
	CAES(void);
	~CAES(void);

	void SetKey(unsigned char* key, int len);

	unsigned char* Cipher(const unsigned char* input, int ilen, int& olen);
	unsigned char* DeCipher(const unsigned char* input, int ilen, int& olen);
};
