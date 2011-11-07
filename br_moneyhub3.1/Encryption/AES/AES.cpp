
#include "stdafx.h"
#include <stdio.h>
#include <memory.h>

#include "aes.h"

#ifdef __TEST_MAIN__

int _tmain(int argc, char* argv[])
{
	unsigned char key_content[16];
	unsigned char iv[16];
	AES_KEY key;
	int ret;
	unsigned char plainData[128];
	unsigned char encrypt[128];
	unsigned char decrypt[128];
	int n;

	memset((void *) key_content,0xa7,16);
	memset((void *)iv,0,16);

	for (n=0;n<126;n++)
	{
		plainData[n]=n;
	}
	ret=AES_set_encrypt_key(key_content,128,&key);
	if (ret==0)
	{
		ret=AES_cbc_encrypt(plainData,encrypt,126,&key,iv,AES_ENCRYPT);
		printf("The encrtpt length is %d\n",ret);

		memset((void *)iv,0,16);
		ret=AES_set_decrypt_key(key_content,128,&key);
		if (ret==0)
		{
			ret=AES_cbc_encrypt(encrypt,decrypt,126,&key,iv,AES_DECRYPT);
			printf("The decrtpt length is %d\n",ret);
			if (memcmp(plainData,decrypt,126)==0)
			{
				printf("OK\n");
			} else {
				printf("Error\n");
			}
		} else {
			printf("Set Decrypt Key Error %d\n",ret);
		}
	} else {
		printf("Set Encrypt Key Error %d\n",ret);
	}

	return 0;
}

#endif