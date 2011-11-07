/*******************************************************************
  rsa.c

    RSA Basic Encryption/Decryption and Key-Generation in ANSI C
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    ANY MODIFICATION MADE TO THIS SOURCE CODE IS AT YOUR OWN RISK.
    Please ask the author for the permission.
 *******************************************************************/

#include    "mpialgo.h"
#include    "mpialgoc.h"
#include    "rsa.h"
#include    "pkcs_rsa.h"


#define RETRYKEYGEN             6


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
