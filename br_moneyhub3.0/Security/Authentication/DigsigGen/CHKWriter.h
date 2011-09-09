#pragma once

#include <map>
#include <vector>
#include <string>
#include "../encryption/SHA512.h"

struct ByteStream
{
	unsigned char* stream;
	int size;
};

class CCHKWriter
{
	typedef std::map<std::string, ByteStream> FileVerifyMap;
	FileVerifyMap m_fileVerify;

	unsigned char* m_signature;
	int m_siglen;

	unsigned char* m_FileHashes;

	unsigned char* _commit(int& size);
public:
	CCHKWriter(void);
	~CCHKWriter(void);

	void AddFileVerify(const std::string& name, unsigned char* hash, int len);
	void SetSignature(unsigned char* sig, int length);

	const unsigned char* GetFileHashes(int& size);

	void Commit(const std::string& filename, unsigned char* Key, int length);
};
