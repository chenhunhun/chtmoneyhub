#ifndef HEADER_COMM_H
#define HEADER_COMM_H


#include <stddef.h>

int DownloadPack(unsigned char *in, int length, unsigned char *out);
int DownloadUnPack(unsigned char *in, int length, unsigned char *out);
int UploadPack(unsigned char *in, int length, unsigned char *out);
int UploadUnPack(unsigned char *in, int length, unsigned char *out);

#endif



