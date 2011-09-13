#ifndef HEADER_COMM_H
#define HEADER_COMM_H


#include <stddef.h>

int CacheUnPack(unsigned char *in, int length, unsigned char *out);
int CachePack(unsigned char *in, int length, unsigned char *out);

#endif




