#ifndef HEADER_COMM_H
#define HEADER_COMM_H


#include <stddef.h>
using namespace std;

int CacheUnPack(unsigned char *in, int length, unsigned char *out);
int CachePack(unsigned char *in, int length, unsigned char *out);
int DataBaseUnPack(unsigned char *in, int length, unsigned char *out);
int DataBasePack(unsigned char *in, int length, unsigned char *out);
int JSFilePack(unsigned char *in, int length, unsigned char *out);
int JSFileUnPack(unsigned char *in, int length, unsigned char *out);
int UserKekPack(unsigned char *in, int length, unsigned char *out);
int UserEdekPack(unsigned char *in, int length, unsigned char* pKey, unsigned char *out);
int UserEdekUnPack(unsigned char *in, int length, unsigned char* pKey, unsigned char *out);
bool FormatHEXString(char *pData, int nLen, string& strEncode);
bool FormatDecVal(const char* pSour, char* pData, int& nLen);
#endif




