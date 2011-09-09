/*******************************************************************
  pkcs_rsa.h

    RSA Implementation complying to PKCS#1 Version 1.5 (Nov. 1993)
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    ANY MODIFICATION MADE TO THIS SOURCE CODE IS AT YOUR OWN RISK.
 *******************************************************************/


#ifndef __SS_PKCS_RSA_DEFINED__
#define __SS_PKCS_RSA_DEFINED__ 1

#include    <stdio.h>
#include    "mpidefs.h"


#ifdef __cplusplus
extern "C" {
#endif

/* These error code must be negative.
 */
#define CAPI_ERR_KEYGENFAILED       -1
#define CAPI_ERR_BADARG             -2
#define CAPI_ERR_BADMPI             -3
#define CAPI_ERR_BADCIPHER          -4
#define CAPI_ERR_BADFORMAT          -5
#define CAPI_ERR_BADMODULUS         -6
#define CAPI_ERR_BADSIG             -7
#define CAPI_ERR_SIGTOOLARGE        -8
#define CAPI_ERR_UNSUPPALGO         -9
#define CAPI_ERR_BADALGOPARAM       -10

#define CAPI_MAXERR                 -11

#define CAPI_perror(code)       CAPI_fperror (stdout, (code))


/* rsaEncryption OBJECT IDENTIFIER
 */
extern const BYTE   rsaEncryption_OBJECTID[],           /* { pkcs-1 1 } */
                    md2WithRSAEncryption_OBJECTID[],    /* { pkcs-1 2 } */
                    md4WithRSAEncryption_OBJECTID[],    /* { pkcs-1 3 } */
                    md5WithRSAEncryption_OBJECTID[],    /* { pkcs-1 4 } */
                    sha1WithRSAEncryption_OBJECTID[];   /* { pkcs-1 5 } */

extern const BYTE	md2_OBJECTID[],						/*      { rsadsi digestAlgorithm(2) 2} ::= { 1 2 840 113549 2 2 }  */
					md4_OBJECTID[],						/*      { rsadsi digestAlgorithm(2) 4} ::= { 1 2 840 113549 2 4 }  */
					md5_OBJECTID[],						/*      { rsadsi digestAlgorithm(2) 5} ::= { 1 2 840 113549 2 5 }  */
					sha1_OBJECTID[];					/*      { iso(1) identified-organization(3) iow(14) secsig(3) algorithms(2) 26}  */


/* (fp, code)
 */
extern void CAPI_fperror PROTO_LIST ((FILE *, int));

/* (code)
 */
extern const char *CAPI_GetErrorString PROTO_LIST ((int));

/* (plainTxt, txtLen, n, ke, cipherTxt, rnd)
   (plainTxt, txtLen) specifies the data to be encrypted and its length
    counted in bytes.
   Output cipherTxt of k bytes long. k is byte-length of n, as specified
    in PKCS#1
   Return value is k > 0 when successful. Otherwise, a negative error
    code is returned.
 */
extern int PKCS_RSA_EncryptPK PROTO_LIST
    ((const BYTE *, int, CONST_MPI, CONST_MPI, BYTE *, RND_PROC));

/* (plainTxt, txtLen, n, p, q, kdp, kdq, A, bt, cipherTxt)
   (plainTxt, txtLen) specifies the data to be encrypted and its length
    counted in bytes.
   bt is the block type specified in PKCS#1
   Output cipherTxt of k bytes long. k is byte-length of n, as specified
    in PKCS#1
   Return value is k > 0 when successful. Otherwise, a negative error
    code is returned.
 */
extern int PKCS_RSA_EncryptSK PROTO_LIST
    ((const BYTE *, int, CONST_MPI, CONST_MPI, CONST_MPI, CONST_MPI,
      CONST_MPI, CONST_MPI, int, BYTE *));

/* (cipherTxt, n, p, q, kdp, kdq, A, plainTxt)
   cipherTxt specifies the data to be decrypted. It MUST be of k bytes
    long. k the byte-length of modulus n, as specified in PKCS#1
   Returns the non-negative byte-length of plainTxt if successful,
    otherwise a negative error code.
 */
extern int PKCS_RSA_DecryptSK PROTO_LIST
    ((const BYTE *, CONST_MPI, CONST_MPI, CONST_MPI, CONST_MPI, CONST_MPI,
      CONST_MPI, BYTE *));

/* (cipherTxt, n, ke, bt, plainTxt, txtLen)
   cipherTxt specifies the data to be decrypted. It MUST be of k bytes
    long. k is the byte-length of modulus n, as specified in PKCS#1
   bt is used to verify if the decrypted block type is correct. A negative
    value means the block type is not cared.
   txtLen is used to parse type 0 block or verify type 1 block. Not cared
    if it is negative. Refer to PKCS#1
   Returns the non-negative byte-length of plainTxt if successful,
    otherwise a negative error code.
 */
extern int PKCS_RSA_DecryptPK PROTO_LIST
    ((const BYTE *, CONST_MPI, CONST_MPI, int, BYTE *, int));

/* (asn, n, e)
   DER encode the public key into ASN.1 format specified in PKCS#1
   If asn == (BYTE *)0, then only returns the byte-length.
   Returns a positive byte-length of the encoded data if successful,
    otherwise a negative error code of PKCS_RSA_BADARG.
 */
extern int PKCS_RSA_PublicKeyEncode PROTO_LIST
    ((BYTE *, CONST_MPI, CONST_MPI));

/* (asn, n, e, d, p, q, kdp, kdq, A)
   DER encode the private key into ASN.1 format specified in PKCS#1
   If asn == (BYTE *)0, then only returns the byte-length.
   Returns a positive byte-length of the encoded data if successful,
    otherwise a negative error code of PKCS_RSA_BADARG.
 */
extern int PKCS_RSA_PrivateKeyEncode PROTO_LIST
    ((BYTE *, CONST_MPI, CONST_MPI, CONST_MPI, CONST_MPI, CONST_MPI,
      CONST_MPI, CONST_MPI, CONST_MPI));

/* (n, e, asn, getValue)
   BER decode the public key from ASN.1 format specified in PKCS#1
   If getValue == FALSE, then the respective MPI is not decoded, only their
    U_WORD-lengths are stored in *n & *e.
   Returns a positive byte-length of the whole BER encoded key if
    successful, otherwise a negative error code of PKCS_RSA_BADFORMAT.
 */
extern int PKCS_RSA_PublicKeyDecode PROTO_LIST
    ((MPI, MPI, const BYTE *, BOOL));

/* (n, e, d, p, q, kdp, kdq, A, asn, getValue)
   BER decode the private key from ASN.1 format specified in PKCS#1
   If getValue == FALSE, then the respective MPI is not decoded, only their
    U_WORD-lengths are stored in *n, *e, *d, ..., *A.
   Returns a positive byte-length of the whole BER encoded key if
    successful, otherwise a negative error code of PKCS_RSA_BADFORMAT.
 */
extern int PKCS_RSA_PrivateKeyDecode PROTO_LIST
    ((MPI, MPI, MPI, MPI, MPI, MPI, MPI, MPI, const BYTE *, BOOL));

/* (asn, algorithm, param)
   Constructs a DER encoded AlgorithmIdentifier. If param == (BYTE *)0 or
    paramLen == 0, then the parameter is absent from the sequence.
   If asn == (BYTE *)0, then the sequence is not stored.
   Returns the byte-length of AlgorithmIdentifier if successful, otherwise
    0 indicating an error.
 */
extern int PKCS_SetAlgorithmID PROTO_LIST
    ((BYTE *, const BYTE *, const BYTE *));

/* (algorithm, param, asn)
   Get algorithm OBJECT IDENTIFIER and parameter from an
    AlgorithmIdentifier. If *param == (BYTE *)0, then parameter is
    absent from the sequence.
   Returns the byte-length of AlgorithmIdentifier if successful, otherwise
    0 indicating an error.
 */
extern int PKCS_GetAlgorithm PROTO_LIST
    ((const BYTE **, const BYTE **, const BYTE *));

/* (n)
   Returns the byte-length of signature
 */
extern int PKCS_RSA_SignatureLen PROTO_LIST ((CONST_MPI));

/* (digest, len, algorithmID, n, p, q, kdp, kdq, A, signature)
   Compute the signature out of a message digest.
   (digest, len) is the raw signature byte string.
   algorithmID is of the ASN.1 type DigestAlgorithmIdentifier.
   Outputs signature in ASN.1 type 'BIT STRING'.
   Returns the byte-length of signature if successful, otherwise a negative
    error code.
 */
extern int PKCS_RSA_Sign PROTO_LIST
    ((const BYTE *, int, const BYTE *, const BYTE *, CONST_MPI,
      CONST_MPI, CONST_MPI, CONST_MPI, CONST_MPI, CONST_MPI, BYTE *));


/* (signature, n, e, digest, md_init, md_update, md_final)
   Analyze signature and pick out digest string, its byte-length, and
    the corresponding message digest functions.
   Returns byte-length of the message digest when successful, otherwise
    a negative error code.
 */
extern int PKCS_RSA_AnalyzeSignature PROTO_LIST
    ((const BYTE *, CONST_MPI, CONST_MPI, BYTE *));

/* (n, e, d, p, q, kdp, kdq, A, bitLen, opt, rnd, pr)
 */
extern int RSA_GenerateKeys PROTO_LIST
    ((MPI, MPI, MPI, MPI, MPI, MPI, MPI, MPI, int, int, RND_PROC, GP_PROC));

/* (n, e, d, p, q, kdp, kdq, A)
   Returns TRUE if the key checking is passed, otherwise FALSE.
 */
extern BOOL RSA_CheckKeys PROTO_LIST
    ((CONST_MPI, CONST_MPI, CONST_MPI, CONST_MPI, CONST_MPI, CONST_MPI,
      CONST_MPI, CONST_MPI));
#ifdef __cplusplus
}
#endif

#endif
