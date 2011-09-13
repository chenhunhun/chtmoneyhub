#include "StdAfx.h"
#include "CHKWriter.h"
#include "../encryption/AES.h"
#include "../encryption/md5.h"

CCHKWriter::CCHKWriter(void)
{
	m_FileHashes = NULL;
	m_signature = NULL;
	m_siglen = 0;
}

CCHKWriter::~CCHKWriter(void)
{
	for(FileVerifyMap::const_iterator it = m_fileVerify.begin(); it != m_fileVerify.end(); it++)
	{
		const ByteStream& stream = it->second;
		delete stream.stream;
	}

	if(m_signature)
		delete m_signature;
}

void CCHKWriter::AddFileVerify(const std::string& name, unsigned char* hash, int size)
{
	ByteStream stream;
	stream.size = size;
	stream.stream = new unsigned char[size];
	memcpy(stream.stream, hash, size);

	m_fileVerify.insert(std::make_pair(name, stream));
}

void CCHKWriter::SetSignature(unsigned char* sig, int length)
{
	if(m_signature)
		delete m_signature;

	m_siglen = length;
	m_signature = new unsigned char[m_siglen];
	memcpy(m_signature, sig, m_siglen);
}

const unsigned char* CCHKWriter::GetFileHashes(int& length)
{
	length = m_fileVerify.size() * MD5_HASH_SIZE;

	if(m_FileHashes)
		delete m_FileHashes;

	m_FileHashes = new unsigned char[length];
	
	unsigned char* pHash = m_FileHashes;

	for(FileVerifyMap::const_iterator it = m_fileVerify.begin(); it != m_fileVerify.end(); it++)
	{
		memcpy(pHash, it->second.stream, it->second.size);
		pHash += it->second.size;
	}

	return m_FileHashes;
}

unsigned char* CCHKWriter::_commit(int& size)
{
	size = 4;

	for(FileVerifyMap::const_iterator it = m_fileVerify.begin(); it != m_fileVerify.end(); it++)
	{
		size += 4;
		size += it->first.length() + 1;
		size += it->second.size;
	}

	size += 4;
	size += m_siglen;

	if(m_FileHashes)
		delete m_FileHashes;

	m_FileHashes = new unsigned char[size];

	unsigned char* pHash = m_FileHashes;

	int FileNum = m_fileVerify.size();
	memcpy(pHash, &FileNum, 4);
	pHash += 4;

	for(FileVerifyMap::const_iterator it = m_fileVerify.begin(); it != m_fileVerify.end(); it++)
	{
		memcpy(pHash, it->second.stream, it->second.size);
		pHash += it->second.size;
	}

	memcpy(pHash, &m_siglen, 4);
	pHash += 4;
	memcpy(pHash, m_signature, m_siglen);

	return m_FileHashes;
}

void CCHKWriter::Commit(const std::string& filename, unsigned char* Key, int length)
{
	CAES aes;
	int commitLen = 0;
	unsigned char* commitBuf = _commit(commitLen);

	int cypherlen = 0;
	aes.SetKey(Key, length);
	unsigned char* cyphertext = aes.Cipher(commitBuf, commitLen, cypherlen);

	for(int i = 0; i < cypherlen; i++)
		printf("%02x", cyphertext[i]);
	printf("\n");

	FILE* fp;
	fopen_s(&fp,filename.c_str(), "wb");
	fwrite(cyphertext, 1, cypherlen, fp);
	fclose(fp);
}
