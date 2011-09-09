#pragma once

#define		SECURE_HASH_BUCKET_SIZE		256
#define     HASH_SIZE                   16

extern HANDLE g_lookupThreadHandle;
struct SECURE_HASH
{
	unsigned char Hash[HASH_SIZE];
	struct SECURE_HASH* next;
};


class LookUpHash
{
public:
	LookUpHash(void);
	~LookUpHash(void);

	inline UCHAR HashFunc(const unsigned char* hash);
	bool AddSecureHash(const unsigned char* hash);
	bool IsHashSecure(const unsigned char* hash);
	bool ClearHash();

	bool IsAdd();
	static HWND         m_mainHwnd;
	DWORD     m_elpseTime;
private:
	SECURE_HASH* m_secuHash[SECURE_HASH_BUCKET_SIZE] ;
	bool         m_isAdd;
	


};


unsigned _stdcall lookupDll( void * p);

bool CheckLoadModule(LookUpHash& luh);