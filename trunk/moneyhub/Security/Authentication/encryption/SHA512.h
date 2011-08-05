#ifndef _SHA512_H
#define _SHA512_H

#include "windows.h"
#include "atlstr.h"

typedef UINT64	uint64_t;
typedef UINT	uint32_t;
typedef UCHAR	uint8_t;

#define SHA512_HASH_SIZE 64

/* Hash size in 64-bit words */
#define SHA512_HASH_WORDS 8

class CSHA512
{
	struct SHA512Context 
	{
		uint64_t totalLength[2];
		uint64_t hash[SHA512_HASH_WORDS];
		uint32_t bufferLength;
		union 
		{
			uint64_t words[16];
			uint8_t bytes[128];
		} buffer;
#ifdef RUNTIME_ENDIAN
		int littleEndian;
#endif /* RUNTIME_ENDIAN */
	};

	SHA512Context * m_pContext;

	void burnStack (int size);
	void SHA512Guts (const uint64_t *cbuf);
	
	uint8_t m_hash[SHA512_HASH_SIZE];
public:
	CSHA512();
	~CSHA512();

	void SHA512Update (const void *data, uint32_t len);
	uint8_t* SHA512Final ();
};
#endif /* !_SHA512_H */
