#ifndef _LGY_MD5_H
#define _LGY_MD5_H

#define		MD5_HASH_SIZE	16

/* MD5 Class. */
class CMD5 {
public:
	CMD5();
	virtual ~CMD5();
	void MD5Update (const unsigned char *input, unsigned int inputLen);
	void MD5Final (unsigned char digest[16]);

private:
	unsigned long int state[4];					/* state (ABCD) */
	unsigned long int count[2];					/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];       /* input buffer */
	unsigned char PADDING[64];		/* What? */

private:
	void MD5Init ();
	void MD5Transform (unsigned long int state[4], const unsigned char block[64]);
	void MD5_memcpy (unsigned char* output, unsigned char* input,unsigned int len);
	void Encode (unsigned char *output, unsigned long int *input,unsigned int len);
	void Decode (unsigned long int *output, const unsigned char *input, unsigned int len);
	void MD5_memset (unsigned char* output,int value,unsigned int len);
};

#endif