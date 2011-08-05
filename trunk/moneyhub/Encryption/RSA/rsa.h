/*******************************************************************
  rsa.h

    RSA Basic Encryption/Decryption and Key-Generation
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    ANY MODIFICATION MADE TO THIS SOURCE CODE IS AT YOUR OWN RISK.
 *******************************************************************/

#ifndef __SS_RSA_DEFINED__
#define __SS_RSA_DEFINED__ 1

#include    "ssglob.h"
#include    "mpialgoc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RSA_LARGEEXPBITS                64
#define RSA_STRONGKEYBITS               320
#define RSA_STRONGKEYDIFFER             128     /* Difference should be no */
                                                /*  less than 2^128.       */
#define RSA_GK_STRONGKEY                1
#define RSA_GK_LARGEEXP                 0


/* (plainTxt, n, ke, cipherTxt)
   plainTxt == cipherTxt is permitted.
 */
extern void RSA_Encrypt PROTO_LIST((CONST_MPI, CONST_MPI, CONST_MPI, MPI));

/* (cipherTxt, p, q, kdp, kdq, A, plainTxt)
   plainTxt == cipherTxt is permitted.
 */
extern void RSA_Decrypt PROTO_LIST((CONST_MPI, CONST_MPI, CONST_MPI, CONST_MPI, CONST_MPI, CONST_MPI, MPI));




#ifdef __cplusplus
}
#endif

#endif
