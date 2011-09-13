
#include "stdafx.h"
#include <stdio.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>
#include "sha.h"

#ifdef __MAIN_TEST__

int _tmain(int argc, char* argv[])
{
	unsigned char data[1024];
	unsigned char md[20];
	unsigned char key[16];
	SHA_CTX	c;

	memset(data,0xc3,1024);

	SHA1_Init(&c);
	SHA1_Update(&c,(const void *)data,128);
	SHA1_Final(md, &c);

	{
		int i,n;
		struct _timeb start,end;

		_ftime_s(&start);
		for (n=0;n<10;n++)
		{
			for (i=0;i<1000000;i++)
			{
				SHA1_Init(&c);
				SHA1_Update(&c,(const void *)data,1000);
				SHA1_Final(md, &c);
			}
		}
		_ftime_s(&end);
		printf("start from %d:%d\n",start.time,start.millitm);
		printf("end to %d:%d\n",end.time,end.millitm);
	}

	memset(key,0x3e,16);
	HMAC_SHA1(data,128,key,16,md);

	return 0;
}

#endif