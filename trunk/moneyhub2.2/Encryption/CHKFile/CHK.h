#ifndef HEADER_CHK_H
#define HEADER_CHK_H

#include <stddef.h>

//#ifdef  __cplusplus
//extern "C" {
//#endif

int packCHK(unsigned char *in, unsigned int length, unsigned char *out);
int unPackCHK(unsigned char *in, unsigned int length, unsigned char *out);

//#ifdef  __cplusplus
//}
//#endif


#endif

