#include "stdafx.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>

#include "../RSA/ssglob.h"
#include "../RSA/mpidefs.h"
#include "../RSA/rsa.h"
#include "../RSA/pkcs_rsa.h"
#include "../SHA1/sha.h"
#include "../AES/aes.h"
#include "ChkSnManager.h"

int packCHK(unsigned char *in, unsigned int length, unsigned char *out)
{
	_ChkSnManager.GetKey();
	if(!_ChkSnManager.bKInit)
		return -1;
	unsigned char *buffer;
	unsigned char hash[20];
	SHA_CTX	c;
	AES_KEY key;
	unsigned char iv[16];
	unsigned int signatureLen;
	unsigned char signature[256];
	unsigned int data_len;
	
	assert(in && out);

	buffer=(unsigned char *)malloc(length+256);
	if (buffer==NULL)
	{
		return -2000;
	}

	SHA1_Init(&c);
	SHA1_Update(&c,(const void *)in,length);
	SHA1_Final(hash, &c);
	
	signatureLen=PKCS_RSA_Sign( hash, 20,sha1_OBJECTID, NULL,_ChkSnManager.snstruct.n, _ChkSnManager.snstruct.p, _ChkSnManager.snstruct.q, _ChkSnManager.snstruct.kdp, _ChkSnManager.snstruct.kdq, _ChkSnManager.snstruct.A, signature );

	buffer[0]=length&0xff;
	buffer[1]=(length>>8)&0xff;
	buffer[2]=(length>>16)&0xff;
	buffer[3]=(length>>24)&0xff;

	memcpy(&buffer[4],in,length);

	buffer[4+length]=signatureLen&0xff;
	buffer[4+1+length]=(signatureLen>>8)&0xff;
	buffer[4+2+length]=(signatureLen>>16)&0xff;
	buffer[4+3+length]=(signatureLen>>24)&0xff;

	memcpy(&buffer[8+length],signature,signatureLen);


	SHA1_Init(&c);
	SHA1_Update(&c,(const void *)_ChkSnManager.snstruct.n,132);
	SHA1_Final(hash, &c);

	AES_set_encrypt_key(hash,128,&key);
	memset(iv,0,16);

	data_len=AES_cbc_encrypt(buffer,out, 8+length+signatureLen, &key, iv,AES_ENCRYPT);

	free(buffer);

	return data_len;
}


	

