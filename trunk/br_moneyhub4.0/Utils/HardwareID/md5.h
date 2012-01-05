#ifndef MD5_H
#define MD5_H

#ifdef __cplusplus
extern "C" {
#endif 

typedef unsigned long int UINT4;

/* Data structure for MD5 (Message Digest) computation */
typedef struct {
	UINT4 i[2];                   /* number of _bits_ handled mod 2^64 */
	UINT4 buf[4];                                    /* scratch buffer */
	unsigned char in[64];                              /* input buffer */
	unsigned char digest[16];     /* actual digest after MD5Final call */
} MD5_CTX;
	
void MD5Final (MD5_CTX *mdContext,unsigned char *digest);
void MD5Update (MD5_CTX *mdContext, unsigned char * inBuf, unsigned int inLen);
void MD5Init (MD5_CTX *mdContext);
#ifdef __cplusplus
 };
#endif 


#endif