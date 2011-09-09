#ifndef HEADER_COMM_H
#define HEADER_COMM_H


#include <stddef.h>

int CacheUnPack(unsigned char *in, int length, unsigned char *out);
int CachePack(unsigned char *in, int length, unsigned char *out);
int DataBaseUnPack(unsigned char *in, int length, unsigned char *out);
int DataBasePack(unsigned char *in, int length, unsigned char *out);
int JSFilePack(unsigned char *in, int length, unsigned char *out);
int JSFileUnPack(unsigned char *in, int length, unsigned char *out);

#endif




