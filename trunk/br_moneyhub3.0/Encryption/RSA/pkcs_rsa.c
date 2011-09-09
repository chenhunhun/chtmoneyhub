/*******************************************************************
  pkcs_rsa.c

    RSA Implementation complying to PKCS#1 Version 1.5 (Nov. 1993)
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    ANY MODIFICATION MADE TO THIS SOURCE CODE IS AT YOUR OWN RISK.
 *******************************************************************/

#include    <stdio.h>
#include    "asn.h"
#include    "mpialgo.h"
#include    "mpialgoc.h"
#include    "rsa.h"
#include    "pkcs_rsa.h"

#define RETRYKEYGEN             6


#if 0

/*  U_WORD should be at least 32-bit.
    plainTxt == cipherTxt is permitted.
 */
void RSA_Encrypt (plainTxt, n, ke, cipherTxt)
CONST_MPI               ke, n, plainTxt;
MPI                     cipherTxt;
{
    U_WORD              n0, s[2*MPI_MAXLEN+1], t[2*MPI_MAXLEN+1];

    if (! MPI_CmpUWord (ke, (U_WORD)65537)) {
        n0 = MPI_MonN0 (n);
        MPI_NResidue (t, plainTxt, n);
        MPI_MonSqr (s, t, n, n0);           /* Sqr 1  */
        MPI_MonSqr (t, s, n, n0);           /* Sqr 2  */
        MPI_MonSqr (s, t, n, n0);           /* Sqr 3  */
        MPI_MonSqr (t, s, n, n0);           /* Sqr 4  */
        MPI_MonSqr (s, t, n, n0);           /* Sqr 5  */
        MPI_MonSqr (t, s, n, n0);           /* Sqr 6  */
        MPI_MonSqr (s, t, n, n0);           /* Sqr 7  */
        MPI_MonSqr (t, s, n, n0);           /* Sqr 8  */
        MPI_MonSqr (s, t, n, n0);           /* Sqr 9  */
        MPI_MonSqr (t, s, n, n0);           /* Sqr 10 */
        MPI_MonSqr (s, t, n, n0);           /* Sqr 11 */
        MPI_MonSqr (t, s, n, n0);           /* Sqr 12 */
        MPI_MonSqr (s, t, n, n0);           /* Sqr 13 */
        MPI_MonSqr (t, s, n, n0);           /* Sqr 14 */
        MPI_MonSqr (s, t, n, n0);           /* Sqr 15 */
        MPI_MonSqr (t, s, n, n0);           /* Sqr 16 */
        MPI_MonMul (s, t, plainTxt, n, n0);
    }
    else {
        MPI_PowerMod (s, plainTxt, ke, n);
    }
    MPI_Assign (cipherTxt, s);
}

/* plainTxt == cipherTxt is permitted.
 */
void RSA_Decrypt (cipherTxt, p, q, kdp, kdq, A, plainTxt)
CONST_MPI               A, cipherTxt, kdp, kdq, p, q;
MPI                     plainTxt;
{
    U_WORD              c1[MPI_MAXLEN+1], c2[MPI_MAXLEN+1],
                        m1[2*MPI_MAXLEN+1], m2[2*MPI_MAXLEN+1];

    MPI_Assign (c1, cipherTxt);
    MPI_Assign (c2, cipherTxt);
    MPI_Mod (c1, p);
    MPI_Mod (c2, q);
    MPI_PowerModPrime (m1, c1, kdp, p);
    MPI_PowerModPrime (m2, c2, kdq, q);
    if (MPI_Cmp (m1, m2) >= 0) {
        MPI_Sub (m1, m2);
        MPI_Mul (c1, m1, A);
        MPI_Mod (c1, p);
        MPI_Mul (c2, c1, q);
    }
    else {
        MPI_Sub3 (c1, m2, m1);
        MPI_Mul (c2, c1, A);
        MPI_Mod (c2, p);
        MPI_Assign (c1, p);
        MPI_Sub (c1, c2);
        MPI_Mul (c2, c1, q);
    }
    MPI_Add3 (plainTxt, m2, c2);
}

int RSA_GenerateKeys (n, e, d, p, q, kdp, kdq, A, bitLen, opt, rnd, pr)
GP_PROC                 pr;
RND_PROC                rnd;
MPI                     A, d, e, kdp, kdq, n, p, q;
int                     bitLen, opt;
{
    MPI                 tp, tq;
    U_WORD              c1[MPI_MAXLEN+1], c2[MPI_MAXLEN+1], m[MPI_MAXLEN+1], x;
    int                 bitLenP, bitLenQ, i, tm;

    if (((opt&RSA_GK_STRONGKEY) && bitLen < RSA_STRONGKEYBITS)
     || ((opt&RSA_GK_LARGEEXP) && bitLen < RSA_LARGEEXPBITS)) {
        return (CAPI_ERR_BADARG);
    }
    /* Set bit-length of p & q so that their lengths are the nearest.
     */
    bitLenP = (bitLen+1)/2;
    bitLenQ = bitLen-bitLenP;
    /* Generate keys
     */
    for (tm = 0; tm < RETRYKEYGEN; ++tm) {
        /* Generate random probable prime p & q
         */
        MPI_GenRandomMPI (c1, (bitLenP+MPI_UWORDBITS-1)/MPI_UWORDBITS, rnd);
        MPI_GenRandomMPI (c2, (bitLenQ+MPI_UWORDBITS-1)/MPI_UWORDBITS, rnd);
        if ((opt&RSA_GK_STRONGKEY)) {
            if (! MPI_GenStrongProbPrime (c1, bitLenP, MPI_GSPP_RSA, pr)) {
                continue;
            }
            if (! MPI_GenStrongProbPrime (c2, bitLenQ, MPI_GSPP_RSA, pr)) {
                continue;
            }
        }
        else {
            MPI_GenProbPrime (c1, bitLenP, pr);
            MPI_PowerOf2 (m, bitLen-1);
            MPI_Div (m, c1, c2);
            MPI_AddUWord (c2, 1);
            MPI_GenProbPrime (c2, bitLenQ, pr);
        }
        if (! (i = MPI_Cmp (c1, c2))) {
            continue;
        }
        /* Compute n = pq and check if the bit-length requirement is
            satisfied.
         */
        MPI_Mul (n, c1, c2);
        if (! (n[(bitLen+MPI_UWORDBITS-1)/MPI_UWORDBITS]&((U_WORD)1 <<
          ((bitLen-1)%MPI_UWORDBITS)))) {
            continue;
        }
        /* Assign p and q
         */
        MPI_Assign (p, c1);
        MPI_Assign (q, c2);
        /* Check if the difference of p & q is large enough
         */
        if (bitLen >= RSA_STRONGKEYBITS) {
            if (i > 0) {
                tp = c1, tq = c2;
            }
            else {
                tp = c2, tq = c1;
            }
            MPI_Sub (tp, tq);
            MPI_PowerOf2 (tq, RSA_STRONGKEYDIFFER);
            if (MPI_Cmp (tp, tq) < 0) {
                continue;
            }
        }
        /* Compute A = q^-1 mod p
         */
        if (! MPI_Inverse (A, q, p)) {
            continue;
        }
        /* Generate exponent e, and compute d = e^-1 mod phi(n)
         */
        MPI_Assign (c1, p);
        MPI_Assign (c2, q);
        MPI_SubUWord (c1, 1);
        MPI_SubUWord (c2, 1);
        MPI_Mul (m, c1, c2);                /* m = (p-1)(q-1) = phi(n) */
        x = 0;
        if ((opt&RSA_GK_LARGEEXP)) {
            MPI_GenRandomMPI
              (e, (RSA_LARGEEXPBITS+MPI_UWORDBITS-1)/MPI_UWORDBITS, rnd);
            e[1] |= 1;
            x = (U_WORD)1 << ((RSA_LARGEEXPBITS-1)%MPI_UWORDBITS);
        }
        else {
            MPI_AssignUWord (e, (U_WORD)65535);
        }
        do {
            MPI_AddUWord (e, 2);
            if ((opt&RSA_GK_LARGEEXP)) {
                e[(RSA_LARGEEXPBITS-1)/MPI_UWORDBITS] =
                  (e[(RSA_LARGEEXPBITS-1)/MPI_UWORDBITS]&((x << 1)-1))|x;
            }
        } while (! MPI_Inverse (d, e, m));
        /* Compute kdp = d mod (p-1) & kdq = d mod (q-1)
         */
        MPI_Assign (m, d);
        MPI_Mod (m, c1);
        MPI_Assign (kdp, m);
        MPI_Assign (m, d);
        MPI_Mod (m, c2);
        MPI_Assign (kdq, m);
        return (0);
    }
    return (CAPI_ERR_KEYGENFAILED);
}

#define CHECK_MPI_LENGTH(n) \
    if ((s = MPI_Len (n)) <= 0 || s > MPI_MAXLEN) { \
        continue; \
    }

BOOL RSA_CheckKeys (n, e, d, p, q, kdp, kdq, A)
CONST_MPI               A, d, e, kdp, kdq, n, p, q;
{
    U_WORD              c[2*MPI_MAXLEN+1], c1[MPI_MAXLEN+1], c2[MPI_MAXLEN+1];
    int                 s;

    do {
        CHECK_MPI_LENGTH (n);
        CHECK_MPI_LENGTH (e);
        CHECK_MPI_LENGTH (d);
        CHECK_MPI_LENGTH (p);
        CHECK_MPI_LENGTH (q);
        CHECK_MPI_LENGTH (kdp);
        CHECK_MPI_LENGTH (kdq);
        CHECK_MPI_LENGTH (A);
        MPI_Mul (c, p, q);                  /* Check if n == pq */
        if (MPI_Cmp (c, n)) {
            continue;
        }
        MPI_SubUWord3 (c1, p, 1);
        MPI_SubUWord3 (c2, q, 1);
        MPI_Assign (c, d);                  /* Check if kdp == d mod (p-1) */
        MPI_Mod (c, c1);
        if (MPI_Cmp (c, kdp)) {
            continue;
        }
        MPI_Assign (c, d);                  /* Check if kdq == d mod (q-1) */
        MPI_Mod (c, c2);
        if (MPI_Cmp (c, kdq)) {
            continue;
        }
        MPI_Mul (c, c1, c2);                /* c = (p-1)(q-1) = phi(n) */
        if (! MPI_Inverse (c1, e, c)) {
            continue;
        }
        if (MPI_Cmp (c1, d)) {              /* Check if d == e^-1 mod phi(n) */
            continue;
        }
        if (! MPI_Inverse (c, q, p)) {      /* Check if A == q^-1 mod p */
            continue;
        }
        if (MPI_Cmp (c, A)) {
            continue;
        }
        return (TRUE);
    } while (FALSE);
    return (FALSE);
}

#endif 


#define GET_MODULUS_BYTE_LENGTH(n, k) \
    if ((k = MPI_ByteLen (n)) > MPI_MAXLEN*MPI_UWORDBYTES) { \
        return (CAPI_ERR_BADMPI); \
    } \
    if (k < 12) { \
        return (CAPI_ERR_BADMODULUS); \
    }


static const char   *s_ERRORINFO[] = {
    "ok",
    "RSA key generation failed",
    "bad input argument(s)",
    "invalid MPI (length too large, etc.)",
    "bad input cipher text (not less than modulus n)",
    "incorrect format",
    "bad modulus (byte-length < 12)",
    "invalid signature string",
    "signature too large to be handled by this algorithm",
    "unsupported algorithm",
    "bad algorithm parameter(s)",

    /* When a invalid error code is found
     */
    "bad error code (FATAL ERROR)"
};

const BYTE  rsaEncryption_OBJECTID[] = {
    0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01
/*              ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  ^^^^  ^^^^  ^^^^
                 ISO US (ANSI) RSA Data Secu Inc    PKCS   #1   rsaEncrypt
    pkcs-1 OBJECT IDENTIFIER ::=
        { iso(1) member-body(2) US(840) rsadsi(113549) pkcs(1) 1 }
    rsaEncryption OBJECT IDENTIFIER ::= 
        { pkcs-1 1 } ::= { 1 2 840 113549 1 1 1 }
 */
};
const BYTE  md2WithRSAEncryption_OBJECTID[] = {
    0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x02
/*      { pkcs-1 2 } ::= { 1 2 840 113549 1 1 2 }
 */
};
const BYTE  md4WithRSAEncryption_OBJECTID[] = {
    0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x03
/*      { pkcs-1 3 } ::= { 1 2 840 113549 1 1 3 }
 */
};
const BYTE  md5WithRSAEncryption_OBJECTID[] = {
    0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x04
/*      { pkcs-1 4 } ::= { 1 2 840 113549 1 1 4 }
 */
};
const BYTE  sha1WithRSAEncryption_OBJECTID[] = {
    0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05
/*      { pkcs-1 5 } ::= { 1 2 840 113549 1 1 5 }
 */
};

const BYTE  md2_OBJECTID[] = {
    0x06, 0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x02
/*      { rsadsi digestAlgorithm(2) 2} ::= { 1 2 840 113549 2 2 }
 */
};
const BYTE  md4_OBJECTID[] = {
    0x06, 0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x04
/*      { rsadsi digestAlgorithm(2) 4} ::= { 1 2 840 113549 2 4 }
 */
};
const BYTE  md5_OBJECTID[] = {
    0x06, 0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x05
/*      { rsadsi digestAlgorithm(2) 5} ::= { 1 2 840 113549 2 5 }
 */
};
const BYTE  sha1_OBJECTID[] = {
    0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a
/*      { iso(1) identified-organization(3) iow(14) secsig(3)
          algorithms(2) 26}
 */
};

void CAPI_fperror (fp, code)
FILE                    *fp;
int                     code;
{
    if (code >= 0) {
        fprintf (fp, "%s !\n", s_ERRORINFO[0]);
    }
    else {
        if (code < CAPI_MAXERR) {
            code = CAPI_MAXERR;
        }
        fprintf (fp, "PKCS_RSA ERROR %d: %s.\n", code, s_ERRORINFO[-code]);
    }
}

const char *CAPI_GetErrorString (code)
int                     code;
{
    if (code >= 0) {
        return (s_ERRORINFO[0]);
    }
    if (code >= CAPI_MAXERR) {
        code = CAPI_MAXERR-1;
    }
    return (s_ERRORINFO[-code]);
}

/* (plainTxt, txtLen) specifies the data to be encrypted and its length
    counted in bytes.
   Output cipherTxt of k bytes long. k is byte-length of n, as specified
    in PKCS#1
   Return value is k > 0 when successful. Otherwise, a negative error
    code is returned.
 */
int PKCS_RSA_EncryptPK (plainTxt, txtLen, n, ke, cipherTxt, rnd)
BYTE                    *cipherTxt;
RND_PROC                rnd;
const BYTE              *plainTxt;
CONST_MPI               ke, n;
int                     txtLen;
{
    BYTE                eb[MPI_MAXLEN*MPI_UWORDBYTES];
    U_WORD              d[MPI_MAXLEN+1];
    int                 i, k;

    /* Verify parameter
     */
    GET_MODULUS_BYTE_LENGTH (n, k);
    if (txtLen < 0 || txtLen > k-11) {
        return (CAPI_ERR_BADARG);
    }
    /* Encryption-block formatting
     */
    eb[0] = 0, eb[1] = 2;
    for (i = 0; i < k-3-txtLen; ++i) {
        while (! (eb[i+2] = (BYTE)(*rnd)()));
    }
    eb[i+2] = 0;
    for (i = 0; i < txtLen; ++i) {
        eb[k-txtLen+i] = plainTxt[i];
    }
    /* Octet-string-to-integer conversion
     */
    MPI_ByteStringToMPI (d, eb, k);
    /* RSA computation
     */
    RSA_Encrypt (d, n, ke, d);
    /* Integer-to-octet-string conversion
     */
    MPI_MPIToByteString (cipherTxt, d, k);
    return (k);
}

/* (plainTxt, txtLen) specifies the data to be encrypted and its length
    counted in bytes.
   bt is the block type specified in PKCS#1
   Output cipherTxt of k bytes long. k is byte-length of n, as specified
    in PKCS#1
   Return value is k > 0 when successful. Otherwise, a negative error
    code is returned.
 */
int PKCS_RSA_EncryptSK
  (plainTxt, txtLen, n, p, q, kdp, kdq, A, bt, cipherTxt)
BYTE                    *cipherTxt;
const BYTE              *plainTxt;
CONST_MPI               A, kdp, kdq, n, p, q;
int                     bt, txtLen;
{
    BYTE                eb[MPI_MAXLEN*MPI_UWORDBYTES];
    U_WORD              d[MPI_MAXLEN+1];
    int                 i, k;

    /* Verify parameter
     */
    if (bt != 0 && bt != 1) {
        return (CAPI_ERR_BADARG);
    }
    GET_MODULUS_BYTE_LENGTH (n, k);
    if (txtLen < 0 || txtLen > k-11) {
        return (CAPI_ERR_BADARG);
    }
    /* Encryption-block formatting
     */
    eb[0] = 0, eb[1] = (BYTE)bt;
    for (i = 0; i < k-3-txtLen; ++i) {
        eb[i+2] = (bt == 0) ? 0 : (BYTE)0xff;
    }
    eb[i+2] = 0;
    for (i = 0; i < txtLen; ++i) {
        eb[k-txtLen+i] = plainTxt[i];
    }
    /* Octet-string-to-integer conversion
     */
    MPI_ByteStringToMPI (d, eb, k);
    /* RSA computation
     */
    RSA_Decrypt (d, p, q, kdp, kdq, A, d);
    /* Integer-to-octet-string conversion
     */
    MPI_MPIToByteString (cipherTxt, d, k);
    return (k);
}

/* cipherTxt specifies the data to be decrypted. It MUST be of k bytes
    long. k the byte-length of modulus n, as specified in PKCS#1
   Returns the non-negative byte-length of plainTxt if successful,
    otherwise a negative error code.
 */
int PKCS_RSA_DecryptSK
  (cipherTxt, n, p, q, kdp, kdq, A, plainTxt)
BYTE                    *plainTxt;
const BYTE              *cipherTxt;
CONST_MPI               A, kdp, kdq, n, p, q;
{
    BYTE                eb[MPI_MAXLEN*MPI_UWORDBYTES];
    U_WORD              d[MPI_MAXLEN+1];
    int                 i, k;

    /* Verify parameter
     */
    GET_MODULUS_BYTE_LENGTH (n, k);
    /* Octet-string-to-integer conversion
     */
    MPI_ByteStringToMPI (d, cipherTxt, k);
    if (MPI_Cmp (n, d) <= 0) {
        return (CAPI_ERR_BADCIPHER);
    }
    /* RSA computation
     */
    RSA_Decrypt (d, p, q, kdp, kdq, A, d);
    /* Integer-to-octet-string conversion
     */
    MPI_MPIToByteString (eb, d, k);
    /* Encryption-block parsing
     */
    if (eb[0] != 0 || eb[1] != 2) {
        return (CAPI_ERR_BADFORMAT);
    }
    for (i = 0; i < 8; ++i) {
        if (! eb[2+i]) {
            return (CAPI_ERR_BADFORMAT);
        }
    }
    for (i = 10; i < k && eb[i]; ++i);
    if (i == k) {
        return (CAPI_ERR_BADFORMAT);
    }
    if (k > i+1) {
        T_memcpy (plainTxt, eb+i+1, k-i-1);
    }
    return (k-i-1);
}

/* cipherTxt specifies the data to be decrypted. It MUST be of k bytes
    long. k is the byte-length of modulus n, as specified in PKCS#1
   txtLen is used to parse type 0 block or verify type 1 block. Not cared
    if it is negative. Refer to PKCS#1
   bt is used to verify if the decrypted block type is correct. A negative
    value means the block type is not cared.
   Returns the non-negative byte-length of plainTxt if successful,
    otherwise a negative error code.
 */
int PKCS_RSA_DecryptPK (cipherTxt, n, ke, bt, plainTxt, txtLen)
BYTE                    *plainTxt;
const BYTE              *cipherTxt;
CONST_MPI               ke, n;
int                     bt, txtLen;
{
    BYTE                eb[MPI_MAXLEN*MPI_UWORDBYTES];
    U_WORD              d[MPI_MAXLEN+1];
    int                 i, k;

    /* Verify parameter
     */
    if (bt > 1) {
        return (CAPI_ERR_BADARG);
    }
    GET_MODULUS_BYTE_LENGTH (n, k);
    if (txtLen > k-11) {
        return (CAPI_ERR_BADARG);
    }
    /* Octet-string-to-integer conversion
     */
    MPI_ByteStringToMPI (d, cipherTxt, k);
    if (MPI_Cmp (n, d) <= 0) {
        return (CAPI_ERR_BADCIPHER);
    }
    /* RSA computation
     */
    RSA_Encrypt (d, n, ke, d);
    /* Integer-to-octet-string conversion
     */
    MPI_MPIToByteString (eb, d, k);
    /* Encryption-block parsing
     */
    if (eb[0] != 0 || (bt < 0 && eb[1] != 0 && eb[1] != 1)
     || (bt >= 0 && eb[1] != (BYTE)bt)) {
        return (CAPI_ERR_BADFORMAT);
    }
    if (eb[1] == 1) {
        for (i = 2; i < k && eb[i] == (BYTE)0xff; ++i);
        if (i >= k || eb[i] || i < 10 || (txtLen >= 0 && k-i-1 != txtLen)) {
            return (CAPI_ERR_BADFORMAT);
        }
        ++i;
    }
    else {
        for (i = 2; i < k && ! eb[i]; ++i);
        if ((i >= k && txtLen < 0) || (txtLen >= 0 && k-i > txtLen) || i < 11) {
            return (CAPI_ERR_BADFORMAT);
        }
        if (txtLen >= 0) {
            i = k-txtLen;
        }
    }
    if (k > i) {
        T_memcpy (plainTxt, eb+i, k-i);
    }
    return (k-i);
}

/* DER encode the public key into ASN.1 format specified in PKCS#1
   If asn == (BYTE *)0, then only returns the byte-length.
   Returns a positive byte-length of the encoded data if successful,
    otherwise a negative error code of PKCS_RSA_BADARG.
 */
int PKCS_RSA_PublicKeyEncode (asn, n, e)
BYTE                    *asn;
CONST_MPI               e, n;
{
    int                 ble, bln, m;

    do {
        if (! (bln = ASN_MPIToASNInteger ((BYTE *)0, n))) {
            continue;
        }
        if (! (ble = ASN_MPIToASNInteger ((BYTE *)0, e))) {
            continue;
        }
        if (! (m = ASN_LengthEncode (asn, bln+ble))) {
            continue;
        }
        if (asn) {
            asn[0] = (BYTE)0x30;
            ASN_MPIToASNInteger (asn+1+m, n);
            ASN_MPIToASNInteger (asn+1+m+bln, e);
        }
        return (1+m+bln+ble);
    } while (FALSE);
    return (CAPI_ERR_BADARG);
}

/* DER encode the private key into ASN.1 format specified in PKCS#1
   If asn == (BYTE *)0, then only returns the byte-length.
   Returns a positive byte-length of the encoded data if successful,
    otherwise a negative error code of PKCS_RSA_BADARG.
 */
int PKCS_RSA_PrivateKeyEncode (asn, n, e, d, p, q, kdp, kdq, A)
BYTE                    *asn;
CONST_MPI               A, d, e, kdp, kdq, n, p, q;
{
    BYTE                *pa;
    U_WORD              version[2];
    int                 blA, bld, ble, blkdp, blkdq, bln, blp, blq, blv, m;

    do {
        MPI_Assign0 (version);
        if (! (blv = ASN_MPIToASNInteger ((BYTE *)0, version))) {
            continue;
        }
        if (! (bln = ASN_MPIToASNInteger ((BYTE *)0, n))) {
            continue;
        }
        if (! (ble = ASN_MPIToASNInteger ((BYTE *)0, e))) {
            continue;
        }
        if (! (bld = ASN_MPIToASNInteger ((BYTE *)0, d))) {
            continue;
        }
        if (! (blp = ASN_MPIToASNInteger ((BYTE *)0, p))) {
            continue;
        }
        if (! (blq = ASN_MPIToASNInteger ((BYTE *)0, q))) {
            continue;
        }
        if (! (blkdp = ASN_MPIToASNInteger ((BYTE *)0, kdp))) {
            continue;
        }
        if (! (blkdq = ASN_MPIToASNInteger ((BYTE *)0, kdq))) {
            continue;
        }
        if (! (blA = ASN_MPIToASNInteger ((BYTE *)0, A))) {
            continue;
        }
        if (! (m = ASN_LengthEncode
          (asn, blA += blv+bln+ble+bld+blp+blq+blkdp+blkdq))) {
            continue;
        }
        if (asn) {
            asn[0] = (BYTE)0x30;
            ASN_MPIToASNInteger ((pa = asn+1+m), version);
            ASN_MPIToASNInteger ((pa += blv), n);
            ASN_MPIToASNInteger ((pa += bln), e);
            ASN_MPIToASNInteger ((pa += ble), d);
            ASN_MPIToASNInteger ((pa += bld), p);
            ASN_MPIToASNInteger ((pa += blp), q);
            ASN_MPIToASNInteger ((pa += blq), kdp);
            ASN_MPIToASNInteger ((pa += blkdp), kdq);
            ASN_MPIToASNInteger ((pa += blkdq), A);
        }
        return (1+m+blA);
    } while (FALSE);
    return (CAPI_ERR_BADARG);
}

/* BER decode the public key from ASN.1 format specified in PKCS#1
   If getValue == FALSE, then the respective MPI is not decoded, only their
    U_WORD-lengths are stored in *n & *e.
   Returns a positive byte-length of the whole BER encoded key if
    successful, otherwise a negative error code of PKCS_RSA_BADFORMAT.
 */
int PKCS_RSA_PublicKeyDecode (n, e, asn, getValue)
BOOL                    getValue;
MPI                     e, n;
const BYTE              *asn;
{
    BOOL                indefinite;
    int                 ble, bln, k, lcontent, m;

    do {
        if (asn[0] != (BYTE)0x30) {
            continue;
        }
        if (! (m = ASN_LengthDecode (&lcontent, asn))) {
            continue;
        }
        indefinite = FALSE;
        if (m == -1) {
            m = 1, indefinite = TRUE;
        }
        if (! (bln = ASN_ASNIntegerToMPI ((MPI)0, &k, asn+1+m))) {
            continue;
        }
        *n = (U_WORD)k;
        if (! (ble = ASN_ASNIntegerToMPI ((MPI)0, &k, asn+1+m+bln))) {
            continue;
        }
        *e = (U_WORD)k;
        if ((! indefinite && lcontent != bln+ble)
         || (indefinite && (asn[1+m+bln+ble]|asn[2+m+bln+ble]))) {
            continue;
        }
        if (getValue) {
            ASN_ASNIntegerToMPI (n, &k, asn+1+m);
            ASN_ASNIntegerToMPI (e, &k, asn+1+m+bln);
        }
        if (indefinite) {
            m += 2;
        }
        return (1+m+bln+ble);
    } while (FALSE);
    return (CAPI_ERR_BADFORMAT);
}

/* BER decode the private key from ASN.1 format specified in PKCS#1
   If getValue == FALSE, then the respective MPI is not decoded, only their
    U_WORD-lengths are stored in *n, *e, *d, ..., *A.
   Returns a positive byte-length of the whole BER encoded key if
    successful, otherwise a negative error code of PKCS_RSA_BADFORMAT.
 */
int PKCS_RSA_PrivateKeyDecode (n, e, d, p, q, kdp, kdq, A, asn, getValue)
BOOL                    getValue;
MPI                     A, d, e, kdp, kdq, n, p, q;
const BYTE              *asn;
{
    BOOL                indefinite;
    const BYTE          *pa;
    int                 blA, bld, ble, blkdp, blkdq, bln, blp, blq, blv,
                        k, lcontent, m;

    do {
        if (asn[0] != (BYTE)0x30
         || ! (m = ASN_LengthDecode (&lcontent, asn))) {
            continue;
        }
        indefinite = FALSE;
        if (m == -1) {
            m = 1, indefinite = TRUE;
        }
        if (ASN_ASNIntegerByteLen (pa = asn+1+m)) {
            continue;
        }
        if (! (blv = ASN_ASNIntegerToMPI ((MPI)0, &k, pa))) {
            continue;
        }
        if (! (bln = ASN_ASNIntegerToMPI ((MPI)0, &k, pa += blv))) {
            continue;
        }
        *n = (U_WORD)k;
        if (! (ble = ASN_ASNIntegerToMPI ((MPI)0, &k, pa += bln))) {
            continue;
        }
        *e = (U_WORD)k;
        if (! (bld = ASN_ASNIntegerToMPI ((MPI)0, &k, pa += ble))) {
            continue;
        }
        *d = (U_WORD)k;
        if (! (blp = ASN_ASNIntegerToMPI ((MPI)0, &k, pa += bld))) {
            continue;
        }
        *p = (U_WORD)k;
        if (! (blq = ASN_ASNIntegerToMPI ((MPI)0, &k, pa += blp))) {
            continue;
        }
        *q = (U_WORD)k;
        if (! (blkdp = ASN_ASNIntegerToMPI ((MPI)0, &k, pa += blq))) {
            continue;
        }
        *kdp = (U_WORD)k;
        if (! (blkdq = ASN_ASNIntegerToMPI ((MPI)0, &k, pa += blkdp))) {
            continue;
        }
        *kdq = (U_WORD)k;
        if (! (blA = ASN_ASNIntegerToMPI ((MPI)0, &k, pa += blkdq))) {
            continue;
        }
        *A = (U_WORD)k;
        blA += blv+bln+ble+bld+blp+blq+blkdp+blkdq;
        if ((! indefinite && lcontent != blA)
         || (indefinite && (asn[1+m+blA]|asn[2+m+blA]))) {
            continue;
        }
        if (getValue) {
            pa = asn+1+m+blv;
            ASN_ASNIntegerToMPI (n, &k, pa);
            ASN_ASNIntegerToMPI (e, &k, pa += bln);
            ASN_ASNIntegerToMPI (d, &k, pa += ble);
            ASN_ASNIntegerToMPI (p, &k, pa += bld);
            ASN_ASNIntegerToMPI (q, &k, pa += blp);
            ASN_ASNIntegerToMPI (kdp, &k, pa += blq);
            ASN_ASNIntegerToMPI (kdq, &k, pa += blkdp);
            ASN_ASNIntegerToMPI (A, &k, pa += blkdq);
        }
        if (indefinite) {
            m += 2;
        }
        return (1+m+blA);
    } while (FALSE);
    return (CAPI_ERR_BADFORMAT);
}

/* Constructs a DER encoded AlgorithmIdentifier. If param == (BYTE *)0,
    then the parameter is absent from the sequence.
   If asn == (BYTE *)0, then the sequence is not stored.
   Returns the byte-length of AlgorithmIdentifier if successful, otherwise
    0 indicating an error.
 */
int PKCS_SetAlgorithmID (asn, algorithm, param)
BYTE                    *asn;
const BYTE              *algorithm, *param;
{
    int                 j, k, m, paramLen;

    if (algorithm[0] != (BYTE)6) {
        return (0);
    }
    if (! (j = k = ASN_Len (algorithm))) {
        return (0);
    }
    paramLen = 0;
    if (param) {
        if (! (paramLen = ASN_Len (param))) {
            return (0);
        }
        j += paramLen;
    }
    if (! (m = ASN_LengthEncode (asn, j))) {
        return (0);
    }
    if (asn) {
        asn[0] = (BYTE)0x30;
        T_memcpy (asn+1+m, algorithm, k);
        if (j > k) {
            T_memcpy (asn+1+m+k, param, paramLen);
        }
    }
    return (1+m+j);
}

/* Get algorithm OBJECT IDENTIFIER and parameter from an
    AlgorithmIdentifier. If *param == (BYTE *)0, then parameter is
    absent from the sequence.
   Returns the byte-length of AlgorithmIdentifier if successful, otherwise
    0 indicating an error.
 */
int PKCS_GetAlgorithm (algorithm, param, asn)
const BYTE              **algorithm, *asn, **param;
{
    int                 i, j, k, m, n;

    if (asn[0] != (BYTE)0x30 || ! (n = ASN_LengthDecode (&k, asn))) {
        return (0);
    }
    m = n < 0 ? 1 : n;
    if (asn[1+m] != (BYTE)6) {
        return (0);
    }
    if (! (j = ASN_Len (*algorithm = asn+1+m))) {
        return (0);
    }
    if (n > 0 && k < j) {
        return (0);
    }
    *param = (BYTE *)0;
    if ((n > 0 && k > j) || (n < 0 && (asn[2+j]|asn[3+j]))) {
        if (! (i = ASN_Len (*param = asn+1+m+j))) {
            return (0);
        }
        j += i;
    }
    if ((n > 0 && k != j) || (n < 0 && (asn[2+j]|asn[3+j]))) {
        return (0);
    }
    m = n < 0 ? m+2 : m;
    return (1+m+j);
}

/* Returns the byte-length of signature
 */
int PKCS_RSA_SignatureLen (n)
CONST_MPI               n;
{
    int                 k;

    k = MPI_ByteLen (n);
    return (2+ASN_LengthEncode ((BYTE *)0, k)+k);
}

/* Compute the signature out of a message digest.
   (digest, len) is the raw signature byte string.
   algorithm and param are the same as PKCS_SetAlgorithmID (..).
   Outputs signature in ASN.1 type 'BIT STRING'.
   Returns the byte-length of signature if successful, otherwise a negative
    error code.
 */
int PKCS_RSA_Sign
    (digest, len, algorithm, param, n, p, q, kdp, kdq, A, signature)
BYTE                    *signature;
CONST_MPI               A, kdp, kdq, n, p, q;
const BYTE              *algorithm, *digest, *param;
int                     len;
{
    BYTE                digestInfo[MPI_MAXLEN*MPI_UWORDBYTES];
    int                 i, j, k, m;

    if (! (i = ASN_ByteStringToASNOctetString ((BYTE *)0, digest, len))
     || ! (j = PKCS_SetAlgorithmID ((BYTE *)0, algorithm, param))) {
        return (CAPI_ERR_BADARG);
    }
    if (! (m = ASN_LengthEncode (digestInfo, i+j))) {
        return (CAPI_ERR_BADARG);
    }
    if ((k = 1+m+i+j) > MPI_ByteLen (n)-11) {
        return (CAPI_ERR_BADARG);
    }
    digestInfo[0] = (BYTE)0x30;
    PKCS_SetAlgorithmID (digestInfo+1+m, algorithm, param);
    ASN_ByteStringToASNOctetString (digestInfo+1+m+j, digest, len);
    if ((i = PKCS_RSA_EncryptSK
        (digestInfo, k, n, p, q, kdp, kdq, A, 1, digestInfo)) < 0) {
            return (i);
    }
    if ((i = ASN_BitStringToASNBitString (signature, digestInfo, i<<3))) {
        return (i);
    }
    return (CAPI_ERR_BADARG);
}

/* Analyze signature and pick out digest string, its byte-length, and
    the corresponding message digest functions.
   Returns byte-length of the message digest when successful, otherwise
    a negative error code.
 */
int PKCS_RSA_AnalyzeSignature
    (signature, n, e, digest)
BYTE                    *digest;
CONST_MPI               n, e;
const BYTE              *signature;
{
    BYTE                *algorithm, digestInfo[MPI_MAXLEN*MPI_UWORDBYTES],
                        *param;
    int                 i, j, k, l, len, m, s, t;

    if (! ASN_ASNBitStringToBitString ((BYTE *)0, &k, signature)) {
        return (CAPI_ERR_BADSIG);
    }
    if ((k&7)) {
        return (CAPI_ERR_BADSIG);
    }
    if ((k >>= 3) > MPI_MAXLEN*MPI_UWORDBYTES) {
        return (CAPI_ERR_SIGTOOLARGE);
    }
    if (k != MPI_ByteLen (n)) {
        return (CAPI_ERR_BADSIG);
    }
    ASN_ASNBitStringToBitString (digestInfo, &k, signature);
    if ((i = PKCS_RSA_DecryptPK (digestInfo, n, e, 1, digestInfo, -1)) < 0) {
        return (CAPI_ERR_BADSIG);
    }
    if (ASN_BoundaryCheck (digestInfo, i) != i) {
        return (CAPI_ERR_BADSIG);
    }
    if (digestInfo[0] != 0x30
     || ! (t = ASN_LengthDecode (&s, digestInfo))) {
        return (CAPI_ERR_BADSIG);
    }
    m = t < 0 ? 1 : t;
    if (! (k = PKCS_GetAlgorithm
      ((const BYTE **)&algorithm, (const BYTE **)&param, digestInfo+1+m))) {
        return (CAPI_ERR_BADSIG);
    }
    if (ASN_IsEqualObjectID (algorithm, md5_OBJECTID)) {
        len = 16;
    }
    else if (ASN_IsEqualObjectID (algorithm, md4_OBJECTID)) {
        len = 16;
    }
    else if (ASN_IsEqualObjectID (algorithm, md2_OBJECTID)) {
        len = 16;
    }
    else if (ASN_IsEqualObjectID (algorithm, sha1_OBJECTID)) {
        len = 20;
    }
    else {
        return (CAPI_ERR_UNSUPPALGO);
    }
    if (param && ! ASN_IsNull (param)) {
        return (CAPI_ERR_BADALGOPARAM);
    }
    if (! (j = ASN_ASNOctetStringToByteString
      ((BYTE *)0, &l, digestInfo+1+m+k))) {
        return (CAPI_ERR_BADSIG);
    }
    if (l != len) {
        return (CAPI_ERR_BADSIG);
    }
    ASN_ASNOctetStringToByteString (digest, &l, digestInfo+1+m+k);
    if ((t > 0 && s != j+k)
     || (t < 0 && (digestInfo[2+j+k]|digestInfo[3+j+k]))) {
        return (CAPI_ERR_BADSIG);
    }
    return (len);
}
