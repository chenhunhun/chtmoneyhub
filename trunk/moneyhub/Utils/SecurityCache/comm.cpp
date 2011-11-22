#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include "../../Encryption/AES/aes.h"
#include "../../Encryption/SHA1/sha.h"
#include "../../Encryption/SHA1/Sha256Calc.h"
using namespace std;

static const unsigned char inter_secert[512]= {
	0xcaU, 0x34U, 0x17U, 0xbbU, 0xd4U, 0xaeU, 0x21U, 0x73U, 
	0x6fU, 0xd2U, 0x19U, 0xa3U, 0xd3U, 0x06U, 0x98U, 0x13U, 
	0xfbU, 0x6eU, 0x03U, 0x28U, 0x03U, 0xc0U, 0x0aU, 0x5aU, 
	0xd5U, 0x54U, 0x74U, 0xe7U, 0x04U, 0x7bU, 0x1fU, 0xc9U, 
	0xf8U, 0x12U, 0x42U, 0xe6U, 0x4cU, 0x57U, 0xf6U, 0x88U, 
	0x74U, 0xfbU, 0xf5U, 0x0cU, 0xa3U, 0x78U, 0xa1U, 0xdeU, 
	0xebU, 0xa4U, 0x48U, 0xa5U, 0xa7U, 0x86U, 0xa6U, 0xbaU, 
	0x16U, 0x69U, 0xabU, 0xdfU, 0x48U, 0x30U, 0x7fU, 0x2eU, 
	0x3eU, 0xe9U, 0xc4U, 0x4fU, 0x4dU, 0xa5U, 0x1bU, 0xeeU, 
	0xc3U, 0x86U, 0xe9U, 0x6cU, 0xcdU, 0x19U, 0x5bU, 0xd5U, 
	0x98U, 0xe6U, 0xa7U, 0x13U, 0xb7U, 0x46U, 0x95U, 0x61U, 
	0xc5U, 0x75U, 0x3fU, 0x02U, 0x4bU, 0xe8U, 0x14U, 0x32U, 
	0xe3U, 0xe1U, 0x23U, 0x5dU, 0x9eU, 0x40U, 0x97U, 0x8fU, 
	0xa4U, 0x9cU, 0x7dU, 0x2cU, 0x1aU, 0x92U, 0xceU, 0xe1U, 
	0x49U, 0x5dU, 0xa9U, 0xdaU, 0xfcU, 0xa7U, 0xe0U, 0x35U, 
	0x29U, 0x9fU, 0xb6U, 0xb7U, 0xd4U, 0x4cU, 0xe7U, 0xbeU, 
	0x31U, 0x21U, 0xe9U, 0x77U, 0x09U, 0xd1U, 0x72U, 0x51U, 
	0x5eU, 0x65U, 0x3aU, 0xb2U, 0x52U, 0x8bU, 0x02U, 0xe8U, 
	0x45U, 0x31U, 0xd5U, 0x62U, 0x3eU, 0x52U, 0x8eU, 0x20U, 
	0x8cU, 0x10U, 0x9aU, 0x68U, 0xadU, 0x04U, 0xffU, 0xbbU, 
	0x6eU, 0xd2U, 0x9dU, 0x07U, 0x54U, 0x00U, 0xb4U, 0x1eU, 
	0x3bU, 0x08U, 0xa7U, 0x67U, 0x3dU, 0xabU, 0x00U, 0xd3U, 
	0xd5U, 0x89U, 0xb3U, 0x13U, 0x44U, 0xefU, 0xa7U, 0x09U, 
	0x34U, 0xf0U, 0x72U, 0x7cU, 0x9cU, 0xb7U, 0x65U, 0x0fU, 
	0xe3U, 0x1bU, 0xc8U, 0x74U, 0x48U, 0x74U, 0x66U, 0xddU, 
	0x81U, 0xadU, 0x4dU, 0xb3U, 0xa2U, 0x9bU, 0xcfU, 0x8bU, 
	0x41U, 0x8dU, 0xccU, 0x56U, 0xd8U, 0x25U, 0x34U, 0xd7U, 
	0x6aU, 0x66U, 0xc7U, 0x5bU, 0x6aU, 0x0dU, 0x1fU, 0xa4U, 
	0xd8U, 0x25U, 0xf1U, 0x26U, 0xadU, 0xd6U, 0x90U, 0x76U, 
	0x79U, 0x7eU, 0xb2U, 0xffU, 0x4bU, 0x03U, 0x77U, 0xf7U, 
	0xd1U, 0x67U, 0xa7U, 0x91U, 0xc0U, 0x9cU, 0x3bU, 0x74U, 
	0x76U, 0x9aU, 0x20U, 0x6dU, 0xdfU, 0xb0U, 0x37U, 0x60U, 
	0x95U, 0x17U, 0xa0U, 0x85U, 0x4bU, 0xcdU, 0x38U, 0xd0U, 
	0x6bU, 0xa0U, 0x60U, 0xb1U, 0xfeU, 0x89U, 0x01U, 0xfeU, 
	0xcdU, 0x3cU, 0xcdU, 0x2eU, 0xc5U, 0xfdU, 0xc7U, 0xc7U, 
	0xa0U, 0xb5U, 0x05U, 0x19U, 0xc2U, 0x45U, 0xb4U, 0x2dU, 
	0x61U, 0x1aU, 0x5eU, 0xf9U, 0xe9U, 0x02U, 0x68U, 0xd5U, 
	0x9fU, 0x3eU, 0xdfU, 0x33U, 0x84U, 0xd8U, 0x74U, 0x8aU, 
	0x7cU, 0x37U, 0xc4U, 0x93U, 0xafU, 0xf0U, 0xddU, 0xb8U, 
	0x30U, 0xdfU, 0xfeU, 0xcaU, 0xddU, 0x77U, 0x9fU, 0xf2U, 
	0x86U, 0x56U, 0xb1U, 0xeaU, 0x51U, 0x1cU, 0x27U, 0x6cU, 
	0x5cU, 0x7eU, 0xb5U, 0xebU, 0xa5U, 0x96U, 0xd8U, 0x82U, 
	0x27U, 0x7dU, 0x16U, 0x2aU, 0x46U, 0x1cU, 0x87U, 0x2fU, 
	0x6dU, 0x3fU, 0x94U, 0xe5U, 0xf6U, 0xebU, 0xffU, 0x97U, 
	0x4aU, 0xf2U, 0x24U, 0xc0U, 0x49U, 0xc4U, 0x7eU, 0x7dU, 
	0xecU, 0x88U, 0x6cU, 0x43U, 0x28U, 0x6cU, 0x35U, 0xcdU, 
	0x16U, 0x39U, 0x4aU, 0x59U, 0x52U, 0x2aU, 0xccU, 0x14U, 
	0xa1U, 0xfeU, 0x4eU, 0xd3U, 0xd6U, 0x4cU, 0xdcU, 0x03U, 
	0xf6U, 0x17U, 0x3cU, 0xe3U, 0x9aU, 0xa2U, 0x73U, 0xf1U, 
	0x95U, 0x85U, 0x8cU, 0xa1U, 0xd7U, 0x01U, 0x94U, 0x55U, 
	0x91U, 0x91U, 0xe9U, 0x8aU, 0x9aU, 0xc1U, 0xb5U, 0x4fU, 
	0x12U, 0x43U, 0xb5U, 0xfcU, 0x45U, 0x40U, 0x3fU, 0x20U, 
	0xd1U, 0xecU, 0x83U, 0xbbU, 0x0cU, 0x5dU, 0x11U, 0xaeU, 
	0xa0U, 0x9dU, 0x9bU, 0x6fU, 0x78U, 0xfeU, 0xfcU, 0x01U, 
	0xe0U, 0xadU, 0x7aU, 0x24U, 0xe7U, 0x8aU, 0x48U, 0xc9U, 
	0x08U, 0x37U, 0x4fU, 0xc8U, 0x0aU, 0x6fU, 0x2fU, 0xd5U, 
	0x25U, 0x9aU, 0x80U, 0xb1U, 0x66U, 0x9eU, 0x5dU, 0x9dU, 
	0x55U, 0xf7U, 0x23U, 0x14U, 0xd4U, 0x0aU, 0x76U, 0xb9U, 
	0x4aU, 0xb6U, 0x85U, 0x8fU, 0x01U, 0x2dU, 0x8fU, 0x68U, 
	0xcdU, 0x02U, 0xa7U, 0xa0U, 0xefU, 0x83U, 0xb4U, 0x0aU, 
	0x38U, 0x48U, 0xbbU, 0x2bU, 0x72U, 0x0cU, 0x61U, 0xa6U, 
	0xfbU, 0xbcU, 0xabU, 0xf8U, 0xb3U, 0xceU, 0x09U, 0x65U, 
	0x18U, 0xd4U, 0x93U, 0x32U, 0xb0U, 0x51U, 0x92U, 0x15U, 
	0xa8U, 0xcbU, 0x42U, 0xeaU, 0xbaU, 0x21U, 0xd6U, 0xa7U
};


int CachePack(unsigned char *in, int length, unsigned char *out)
{
	unsigned char cacheAESKey[20];
	unsigned char cacheHMACKey[20];
	unsigned char iv[20];
	AES_KEY key;
	unsigned char md[20];
	int md_len;
	unsigned char *buffer;
	int buffer_len;
	int ret;

	assert(in && out);

	buffer=NULL;

	memcpy(cacheHMACKey,&inter_secert[86],4);
	memcpy(&cacheHMACKey[8],&inter_secert[382],8);
	memcpy(&cacheHMACKey[4],&inter_secert[154],4);

	ret=HMAC_SHA1(in,length,cacheHMACKey,16,md);
	
	memset(cacheHMACKey,0x1,20);

	if (ret==0)
	{
		md_len=HMAC_SHA1_LEN;
		buffer_len=9+4+length+md_len;
		buffer=(unsigned char *)malloc(buffer_len);
		if (buffer!=NULL)
		{
			memcpy(buffer,"HashCache",9);
			buffer[9]=length&0xff;
			buffer[10]=(length>>8)&0xff;
			buffer[11]=(length>>16)&0xff;
			buffer[12]=(length>>24)&0xff;

			memcpy(&buffer[13],in,length);
			memcpy(&buffer[13+length],md,md_len);


			memcpy(cacheAESKey,&inter_secert[312],4);
			memcpy(&cacheAESKey[12],&inter_secert[36],4);
			memcpy(&cacheAESKey[4],&inter_secert[176],8);

			AES_set_encrypt_key(cacheAESKey,128,&key);

			memcpy(iv,&inter_secert[162],8);
			memcpy(&iv[8],&inter_secert[252],8);

			ret=AES_cbc_encrypt(buffer,out, buffer_len, &key, iv,AES_ENCRYPT);

			memset(cacheAESKey,1,20);
			memset(iv,1,20);
			memset(&key,0,sizeof(AES_KEY));
		} else {
			return -1001;
		}
	} else {
		return -1002;
	}

	if (buffer!=NULL)
	{
		free(buffer);
	}

	return ret;

}

int CacheUnPack(unsigned char *in, int length, unsigned char *out)
{
	unsigned char cacheAESKey[20];
	unsigned char cacheHMACKey[20];
	unsigned char iv[20];
	AES_KEY key;
	unsigned char md[20];
	unsigned char *buffer;
	int buffer_len;
	unsigned int data_len;
	int ret;

	assert(in && out);

	buffer=NULL;

	if (length<32)
	{
		return -1003;
	}

	buffer=(unsigned char *) malloc(length);
	if (buffer!=NULL)
	{
		memcpy(cacheAESKey,&inter_secert[312],4);
		memcpy(&cacheAESKey[12],&inter_secert[36],4);
		memcpy(&cacheAESKey[4],&inter_secert[176],8);

		AES_set_decrypt_key(cacheAESKey,128,&key);

		memcpy(iv,&inter_secert[162],8);
		memcpy(&iv[8],&inter_secert[252],8);

		buffer_len=AES_cbc_encrypt(in,buffer, length, &key, iv,AES_DECRYPT);

		memset(cacheAESKey,1,20);
		memset(iv,1,20);
		memset(&key,0,sizeof(AES_KEY));

		if (memcmp(buffer,"HashCache",9)!=0)
		{
			free(buffer);
			return -1006;
		}

		data_len=buffer[9]+(buffer[10]<<8)+(buffer[11]<<16)+(buffer[12]<<24);

		if ((int)(data_len+9+4+20) > length)
		{
			free(buffer);
			return -1004;

		}
		

		memcpy(cacheHMACKey,&inter_secert[86],4);
		memcpy(&cacheHMACKey[8],&inter_secert[382],8);
		memcpy(&cacheHMACKey[4],&inter_secert[154],4);

		ret=HMAC_SHA1(&buffer[13],data_len,cacheHMACKey,16,md);
	
		memset(cacheHMACKey,0x1,20);

		if (ret==0)
		{
			ret=memcmp(md,&buffer[13+data_len],HMAC_SHA1_LEN);
			if (ret!=0)
			{
				ret=-1005;
			} else {
				ret=data_len;
				memcpy(out,&buffer[13],data_len);
			}
		} else {
			ret= -1002;
		}
	} else {
		ret=-1001;
	}

	if (buffer!=NULL)
	{
		free(buffer);
	}

	return ret;
}


// 数据库加密和数据库解密
int DataBasePack(unsigned char *in, int length, unsigned char *out)
{
	unsigned char cacheAESKey[20];
	unsigned char cacheHMACKey[20];
	unsigned char iv[20];
	AES_KEY key;
	unsigned char md[20];
	int md_len;
	unsigned char *buffer;
	int buffer_len;
	int ret;

	assert(in && out);

	buffer=NULL;

	memcpy(cacheHMACKey,&inter_secert[86],4);
	memcpy(&cacheHMACKey[8],&inter_secert[129],8);
	memcpy(&cacheHMACKey[4],&inter_secert[333],4);

	ret=HMAC_SHA1(in,length,cacheHMACKey,16,md);
	
	memset(cacheHMACKey,0x1,20);

	if (ret==0)
	{
		md_len=HMAC_SHA1_LEN;
		buffer_len=9+4+length+md_len;
		buffer=(unsigned char *)malloc(buffer_len);
		if (buffer!=NULL)
		{
			memcpy(buffer,"HashCache",9);
			buffer[9]=length&0xff;
			buffer[10]=(length>>8)&0xff;
			buffer[11]=(length>>16)&0xff;
			buffer[12]=(length>>24)&0xff;

			memcpy(&buffer[13],in,length);
			memcpy(&buffer[13+length],md,md_len);


			memcpy(cacheAESKey,&inter_secert[312],4);
			memcpy(&cacheAESKey[12],&inter_secert[36],4);
			memcpy(&cacheAESKey[4],&inter_secert[176],8);

			AES_set_encrypt_key(cacheAESKey,128,&key);

			memcpy(iv,&inter_secert[162],8);
			memcpy(&iv[8],&inter_secert[252],8);

			ret=AES_cbc_encrypt(buffer,out, buffer_len, &key, iv,AES_ENCRYPT);

			memset(cacheAESKey,1,20);
			memset(iv,1,20);
			memset(&key,0,sizeof(AES_KEY));
		} else {
			return -1001;
		}
	} else {
		return -1002;
	}

	if (buffer!=NULL)
	{
		free(buffer);
	}

	return ret;

}

int DataBaseUnPack(unsigned char *in, int length, unsigned char *out)
{
	unsigned char cacheAESKey[20];
	unsigned char cacheHMACKey[20];
	unsigned char iv[20];
	AES_KEY key;
	unsigned char md[20];
	unsigned char *buffer;
	int buffer_len;
	unsigned int data_len;
	int ret;

	assert(in && out);

	buffer=NULL;

	if (length<32)
	{
		return -1003;
	}

	buffer=(unsigned char *) malloc(length);
	if (buffer!=NULL)
	{
		memcpy(cacheAESKey,&inter_secert[312],4);
		memcpy(&cacheAESKey[12],&inter_secert[36],4);
		memcpy(&cacheAESKey[4],&inter_secert[176],8);

		AES_set_decrypt_key(cacheAESKey,128,&key);

		memcpy(iv,&inter_secert[162],8);
		memcpy(&iv[8],&inter_secert[252],8);

		buffer_len=AES_cbc_encrypt(in,buffer, length, &key, iv,AES_DECRYPT);

		memset(cacheAESKey,1,20);
		memset(iv,1,20);
		memset(&key,0,sizeof(AES_KEY));

		if (memcmp(buffer,"HashCache",9)!=0)
		{
			free(buffer);
			return -1006;
		}

		data_len=buffer[9]+(buffer[10]<<8)+(buffer[11]<<16)+(buffer[12]<<24);

		if ((int)(data_len+9+4+20) > length)
		{
			free(buffer);
			return -1004;

		}
		

		memcpy(cacheHMACKey,&inter_secert[86],4);
		memcpy(&cacheHMACKey[8],&inter_secert[129],8);
		memcpy(&cacheHMACKey[4],&inter_secert[333],4);

		ret=HMAC_SHA1(&buffer[13],data_len,cacheHMACKey,16,md);
	
		memset(cacheHMACKey,0x1,20);

		if (ret==0)
		{
			ret=memcmp(md,&buffer[13+data_len],HMAC_SHA1_LEN);
			if (ret!=0)
			{
				ret=-1005;
			} else {
				ret=data_len;
				memcpy(out,&buffer[13],data_len);
			}
		} else {
			ret= -1002;
		}
	} else {
		ret=-1001;
	}

	if (buffer!=NULL)
	{
		free(buffer);
	}

	return ret;
}


// JS文件的加密和解密
int JSFilePack(unsigned char *in, int length, unsigned char *out)
{
	unsigned char cacheAESKey[20];
	unsigned char cacheHMACKey[20];
	unsigned char iv[20];
	AES_KEY key;
	unsigned char md[20];
	int md_len;
	unsigned char *buffer;
	int buffer_len;
	int ret;

	assert(in && out);

	buffer=NULL;

	memcpy(cacheHMACKey,&inter_secert[77],4);
	memcpy(&cacheHMACKey[8],&inter_secert[125],8);
	memcpy(&cacheHMACKey[4],&inter_secert[255],4);

	ret=HMAC_SHA1(in,length,cacheHMACKey,16,md);
	
	memset(cacheHMACKey,0x1,20);

	if (ret==0)
	{
		md_len=HMAC_SHA1_LEN;
		buffer_len=9+4+length+md_len;
		buffer=(unsigned char *)malloc(buffer_len);
		if (buffer!=NULL)
		{
			memcpy(buffer,"HashCache",9);
			buffer[9]=length&0xff;
			buffer[10]=(length>>8)&0xff;
			buffer[11]=(length>>16)&0xff;
			buffer[12]=(length>>24)&0xff;

			memcpy(&buffer[13],in,length);
			memcpy(&buffer[13+length],md,md_len);


			memcpy(cacheAESKey,&inter_secert[212],4);
			memcpy(&cacheAESKey[12],&inter_secert[136],4);
			memcpy(&cacheAESKey[4],&inter_secert[176],8);

			AES_set_encrypt_key(cacheAESKey,128,&key);

			memcpy(iv,&inter_secert[162],8);
			memcpy(&iv[8],&inter_secert[252],8);

			ret=AES_cbc_encrypt(buffer,out, buffer_len, &key, iv,AES_ENCRYPT);

			memset(cacheAESKey,1,20);
			memset(iv,1,20);
			memset(&key,0,sizeof(AES_KEY));
		} else {
			return -1001;
		}
	} else {
		return -1002;
	}

	if (buffer!=NULL)
	{
		free(buffer);
	}

	return ret;

}

int JSFileUnPack(unsigned char *in, int length, unsigned char *out)
{
	unsigned char cacheAESKey[20];
	unsigned char cacheHMACKey[20];
	unsigned char iv[20];
	AES_KEY key;
	unsigned char md[20];
	unsigned char *buffer;
	int buffer_len;
	unsigned int data_len;
	int ret;

	assert(in && out);

	buffer=NULL;

	if (length<32)
	{
		return -1003;
	}

	buffer=(unsigned char *) malloc(length);
	if (buffer!=NULL)
	{
		memcpy(cacheAESKey,&inter_secert[212],4);
		memcpy(&cacheAESKey[12],&inter_secert[136],4);
		memcpy(&cacheAESKey[4],&inter_secert[176],8);

		AES_set_decrypt_key(cacheAESKey,128,&key);

		memcpy(iv,&inter_secert[162],8);
		memcpy(&iv[8],&inter_secert[252],8);

		buffer_len=AES_cbc_encrypt(in,buffer, length, &key, iv,AES_DECRYPT);

		memset(cacheAESKey,1,20);
		memset(iv,1,20);
		memset(&key,0,sizeof(AES_KEY));

		if (memcmp(buffer,"HashCache",9)!=0)
		{
			free(buffer);
			return -1006;
		}

		data_len=buffer[9]+(buffer[10]<<8)+(buffer[11]<<16)+(buffer[12]<<24);

		if ((int)(data_len+9+4+20) > length)
		{
			free(buffer);
			return -1004;

		}
		

		memcpy(cacheHMACKey,&inter_secert[77],4);
		memcpy(&cacheHMACKey[8],&inter_secert[125],8);
		memcpy(&cacheHMACKey[4],&inter_secert[255],4);

		ret=HMAC_SHA1(&buffer[13],data_len,cacheHMACKey,16,md);
	
		memset(cacheHMACKey,0x1,20);

		if (ret==0)
		{
			ret=memcmp(md,&buffer[13+data_len],HMAC_SHA1_LEN);
			if (ret!=0)
			{
				ret=-1005;
			} else {
				ret=data_len;
				memcpy(out,&buffer[13],data_len);
			}
		} else {
			ret= -1002;
		}
	} else {
		ret=-1001;
	}

	if (buffer!=NULL)
	{
		free(buffer);
	}

	return ret;
}


int UserKekPack(unsigned char *in, int length, unsigned char *out)
{
	assert(NULL != in && length > 0);
	if (NULL == in || length<= 0)
		return 0;

	const int nSize = 32; // sha数组大小
	const int nSATL1Len = 8; // SATL1大小

	char* pChTemp = new char[nSize + 1];
	memset(pChTemp, 0, nSize + 1);

	memcpy(pChTemp, &inter_secert[162], nSATL1Len); // SATL1值

	if (length + nSATL1Len > nSize)
		memcpy(pChTemp + nSATL1Len, in, nSize - nSATL1Len);
	else
		memcpy(pChTemp + nSATL1Len, in, length);

	for (int i = 0;i < 100; i ++)
	{
		Sha256Calc s1;

		Sha256Calc_reset(&s1);
		Sha256Calc_calculate(&s1, (SZ_CHAR *)pChTemp, nSize);

		memset(pChTemp, 0, nSize);
		memcpy(pChTemp, s1.Value, nSize);
	}
	
	memcpy(out, pChTemp, nSize);
	delete[] pChTemp;

	return 0;
}


int UserEdekPack(unsigned char *in, int length, unsigned char* pKey, unsigned char *out)
{

	if (NULL == in || NULL == pKey || NULL == out)
		return 0;

	//unsigned char cacheAESKey[20] = {0};
	unsigned char iv[20] = {0}; // vi用一个固定值
	AES_KEY key;

	AES_set_encrypt_key(pKey, 256, &key);

	memcpy(iv,&inter_secert[162],8);
	memcpy(&iv[8],&inter_secert[252],8);

	int buffer_len = AES_cbc_encrypt(in, out, length, &key, iv, AES_ENCRYPT);

	return 0;
}

int UserEdekUnPack(unsigned char *in, int length, unsigned char* pKey, unsigned char *out)
{
	if (NULL == in || NULL == pKey || NULL == out)
		return 0;

	//unsigned char cacheAESKey[20] = {0};
	unsigned char iv[20] = {0}; // vi用一个固定值
	AES_KEY key;

	AES_set_decrypt_key(pKey, 256, &key);

	memcpy(iv,&inter_secert[162],8);
	memcpy(&iv[8],&inter_secert[252],8);

	int buffer_len = AES_cbc_encrypt(in, out, length, &key, iv, AES_DECRYPT);

	return 0;

}

// 格式化成十六进制字符串
bool FormatHEXString(char *pData, int nLen, string& strEncode)
{
	ATLASSERT (NULL != pData && nLen > 0);
	if (NULL == pData || nLen <= 0)
		return false;
	strEncode.clear();


	//int dwSize = nLen * 2 + 1;
	//unsigned char* pszOut = new unsigned char[dwSize];
	//base64_encode((LPBYTE)pData, nLen, pszOut, &dwSize);

	for(int i = 0; i < nLen; i ++)
	{
		CString strTp;
		int nTemp = *(pData + i);
		if (nTemp < 0)
			nTemp += 256; // 取该字节的反码

		strTp.Format(L"%02x", nTemp);
		strEncode += CW2A(strTp);


		// 根据PHP代码转换成C代码
		/*int nOrd = *(pData + i);
		strEncode += SingleDecToHex((nOrd - nOrd % 16) / 16);
    	strEncode += SingleDecToHex(nOrd % 16);*/
	}

	return true;
}

// 格式化成十六进制字符串
bool FormatDecVal(const char* pSour, char* pData, int& nLen)
{
	ATLASSERT (NULL != pSour && NULL != pData);
	if (NULL == pSour || NULL == pData)
		return false;

	nLen = 0;
	int nSourLen = strlen(pSour);
	if (nSourLen <= 0)
		return false;
	//nLen = (nSourLen + 1) / 2;

	for (int i = 0; i < nSourLen; i += 2)
	{
		int nTp = *(pSour + i);
		int nTp2 = *(pSour + i + 1);

		if( 47 < nTp && nTp < 58 )
			nTp = nTp - 48;
		 if( 96 < nTp && nTp < 103 ) 
			 nTp = nTp - 87;

		if( 47 < nTp2 && nTp2 < 58 )
			nTp2 = nTp2 - 48;
		 if( 96 < nTp2 && nTp2 < 103 ) 
			 nTp2 = nTp2 - 87;

		 // 一个字节存储的大小为-128至127
		 int nVal = nTp * 16 + nTp2;
		 if (nVal > 127)
			 nVal -= 256;

		*(pData + nLen) = nVal;

		nLen ++;

	}

	return true;
}