/*******************************************************************
  mpialgoc.c

    Multi-Precision Integer (MPI) Number Theory Algorithms in ANSI C
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    ANY MODIFICATION MADE TO THIS SOURCE CODE IS AT YOUR OWN RISK.
    Please ask the author for the permission.
 *******************************************************************/

#include    "mpialgo.h"
#include    "mpialgoc.h"


#define PRIMENUM        100
#define MILLERDEPTH     40
#define C               1024
#define DELTA           4
#define K               5678    /* ((U_WORD)(0.6932*C*(1 << (DELTA-1)))) */

static U_WORD   s_prime[PRIMENUM] = {
                      3,   5,   7,  11,  13,  17,  19,  23,  29,  31,
                     37,  41,  43,  47,  53,  59,  61,  67,  71,  73,
                     79,  83,  89,  97, 101, 103, 107, 109, 113, 127,
                    131, 137, 139, 149, 151, 157, 163, 167, 173, 179,
                    181, 191, 193, 197, 199, 211, 223, 227, 229, 233,
                    239, 241, 251, 257, 263, 269, 271, 277, 281, 283,
                    293, 307, 311, 313, 317, 331, 337, 347, 349, 353,
                    359, 367, 373, 379, 383, 389, 397, 401, 409, 419,
                    421, 431, 433, 439, 443, 449, 457, 461, 463, 467,
                    479, 487, 491, 499, 503, 509, 521, 523, 541, 547 };
static signed char  s_F[8] = {0, 1, 0,-1, 1, 0,-1, 0};
static int          s_C[8] = {0, 0, 0, 4, 0, 4, 4, 4};


/* a >= b >= 0 and if computes inverse, a MUST be an ODD MPI !!!
   invB = (b^-1)*(2^exp2) mod a
   if gcd == (MPI)0 then the gcd value is discarded
   if invB == (MPI)0 then the inverse value is discarded
   returns TRUE if inverse of b mod a exists.
   
   invB or gcd == b is permitted
   gcd == a is permitted if invB is NOT needed
 */
BOOL MPI_RightShiftAlgorithm (a, b, gcd, invB, exp2)
CONST_MPI               a, b;
MPI                     gcd, invB;
int                     exp2;
{
    U_WORD              r[MPI_MAXLEN+2], s[MPI_MAXLEN+1],
                        u[MPI_MAXLEN+1], v[MPI_MAXLEN+1], x;
    int                 k, m, t, t0;

    if (! MPI_Len (b)) {
        if (gcd) {
            MPI_Assign (gcd, a);
        }
        return (FALSE);
    }
    MPI_Assign (u, a);
    MPI_Assign (v, b);
    MPI_Assign0 (r);
    MPI_AssignUWord (s, 1);
    for (x = 0, m = 0; m < (int)*u && m < (int)*v && ! (x = u[1+m]|v[1+m]); ++m);
    t0 = m*MPI_UWORDBITS;
    for (; ! (x&1); ++t0, x >>= 1);
    MPI_Shr (u, t0);
    MPI_Shr (v, t0);
    k = 0;
    while (MPI_Len (v)) {
        if ((u[1]&1) == 0) {
            for (m = 0; m < (int)*u && ! (x = u[1+m]); ++m);
            t = m*MPI_UWORDBITS;
            for (; ! (x&1); ++t, x >>= 1);
            MPI_Shr (u, t);
            MPI_Shl (s, t);
            k += t;
        }
        if ((v[1]&1) == 0) {
            for (m = 0; m < (int)*v && ! (x = v[1+m]); ++m);
            t = m*MPI_UWORDBITS;
            for (; ! (x&1); ++t, x >>= 1);
            MPI_Shr (v, t);
            MPI_Shl (r, t);
            k += t;
        }
        if (MPI_Cmp (u, v) > 0) {
            MPI_Sub (u, v);
            MPI_Shr (u, 1);
            MPI_Add (r, s);
            MPI_Shl (s, 1);
            ++k;
        }
        else {
            MPI_Sub (v, u);
            MPI_Shr (v, 1);
            MPI_Add (s, r);
            MPI_Shl (r, 1);
            ++k;
        }
    }
    if (t0) {
        MPI_Shl (u, t0);
    }
    if (gcd) {
        MPI_Assign (gcd, u);
    }
    if (t0 || MPI_CmpUWord (u, 1)) {
        return (FALSE);
    }
    if (invB) {
        if (MPI_Cmp (r, a) >= 0) {
            MPI_Sub (r, a);
        }
        for (k -= exp2; k > 0; ) {
            if ((r[1]&1) == 0) {
                for (m = 0; m < (int)*r && ! (x = r[1+m]); ++m);
                t = m*MPI_UWORDBITS;
                for (; ! (x&1); ++t, x >>= 1);
                if (t > k) {
                    t = k;
                }
                MPI_Shr (r, t);
                k -= t;
            }
            else {
                MPI_Add (r, a);
                MPI_Shr (r, 1);
                --k;
            }
        }
        for (; k < 0; ++k) {
            MPI_Shl (r, 1);
            if (MPI_Cmp (r, a) >= 0) {
                MPI_Sub (r, a);
            }
        }
        MPI_Sub3 (invB, a, r);
    }
    return (TRUE);
}

/* If gcd == (MPI)0 then the gcd value is discarded.
   if invB == (MPI)0 then the inverse value is discarded
   
   invA or gcd == a is permitted
   gcd == n is permitted if invA is NOT needed
 */
BOOL MPI_ExtEuclidean (n, a, gcd, invA)
CONST_MPI               a, n;
MPI                     gcd, invA;
{
    U_WORD              s[2][MPI_MAXLEN+1], t[2][MPI_MAXLEN+1];
    U_WORD              r[2][MPI_MAXLEN+1], q[MPI_MAXLEN+1];
    U_WORD              tmp[MPI_MAXLEN+1];
    register int        i, j;

    MPI_AssignUWord (s[0], 1);
    MPI_Assign0 (s[1]);
    MPI_Assign0 (t[0]);
    MPI_AssignUWord (t[1], 1);
    MPI_Assign (r[0], n);
    MPI_Assign (r[1], a);
    for (i=0, j=1; ; i^=1, j^=1) {
        MPI_Div (r[i], r[j], q);
        if (! MPI_Len (r[i])) {
            break;
        }
        MPI_Mul (tmp, s[j], q);
        MPI_Add (s[i], tmp);
        MPI_Mul (tmp, t[j], q);
        MPI_Add (t[i], tmp);
    }
    if (gcd) {
        MPI_Assign (gcd, r[j]);
    }
    if (MPI_CmpUWord (r[j], 1)) {
        return (FALSE);
    }
    if (invA) {
        if (i) {
            MPI_Sub3 (invA, n, t[j]);
        }
        else {
            MPI_Assign (invA, t[j]);
        }
    }
    return (TRUE);
}

/* result <- (a ^ c) % n
   a < n !!!
   n MUST be an ODD MPI s.t. gcd (a, n) = 1 !!!
   Space of result MUST be at least 2*length of n !!!
   (result == a is permissible)
 */
void MPI_PowerModPrime (result, a, c, n)
CONST_MPI               a, c, n;
MPI                     result;
{
    U_WORD              m[5][MPI_MAXLEN+1], m1[4][MPI_MAXLEN+1], n0,
                        *t, *t0, *t1;
    signed char         f[MPI_MAXLEN*MPI_UWORDBITS+1];
    int                 ci, f0, f2, h, i, j, k, s;

    if (! MPI_Len (a)) {
        MPI_Assign0 (result);
    }
    else if ((h = MPI_Len (c)) == 0) {
        MPI_AssignUWord (result, 1);
    }
    else {
        /* The following 2 for-loops compute the canonical signed-digital
           representation of exponent c
         */
        for (ci = 0, k = i = 0; i < h-1; ++i) {
            for (n0 = c[1+i], j = 0; j < MPI_UWORDBITS-1; n0 >>= 1, ++j) {
                s = ci+(int)(n0&3);
                f[k++] = s_F[s], ci = s_C[s];
            }
            s = ci+(int)((n0&1)+((c[1+i+1]&1) << 1));
            f[k++] = s_F[s], ci = s_C[s];
        }
        for (n0 = c[1+i], j = 0; j <= MPI_UWORDBITS; n0 >>= 1, ++j) {
            s = ci+(int)(n0&3);
            f[k++] = s_F[s], ci = s_C[s];
        }

        /* Pre-calculate factors:
            m1[0] = (-1),  m1[1] = 1
            m[0] = (-1)0(-1),  m[1] = (-1)01,  m[2] = 10(-1), m[3] = 101
         */
        n0 = MPI_MonN0 (n);
        MPI_MonInverse (m1[0], a, n);
        MPI_NResidue (m1[1], a, n);
        MPI_MonSqr (m1[2], m1[0], n, n0);
        MPI_MonSqr (result, m1[2], n, n0);
        MPI_MonMul (m[0], result, m1[0], n, n0);
        MPI_MonMul (m[1], result, m1[1], n, n0);
        MPI_MonSqr (m1[2], m1[1], n, n0);
        MPI_MonSqr (result, m1[2], n, n0);
        MPI_MonMul (m[2], result, m1[0], n, n0);
        MPI_MonMul (m[3], result, m1[1], n, n0);

        for (--k; k >= 0 && f[k] == 0; --k);
        f0 = (int)f[k];
        if (k >= 2 && (f2 = (int)f[k-2]) != 0) {
            MPI_Assign (result, m[f0+1+((f2+1)>>1)]);
            k -= 3;
        }
        else {
            MPI_Assign (result, m1[(f0+1)>>1]);
            --k;
        }

        t0 = result, t1 = m1[2];
        for (; k >= 0; ) {
            MPI_MonSqr (t1, t0, n, n0);
            if ((f0 = (int)f[k]) == 0) {
                t = t0, t0 = t1, t1 = t;
                --k;
            }
            else {
                if (k >= 2 && (f2 = (int)f[k-2]) != 0) {
                    MPI_MonSqr (t0, t1, n, n0);
                    MPI_MonSqr (t1, t0, n, n0);
                    MPI_MonMul (t0, t1, m[f0+1+((f2+1)>>1)], n, n0);
                    k -= 3;
                }
                else {
                    MPI_MonMul (t0, t1, m1[(f0+1)>>1], n, n0);
                    --k;
                }
            }
        }
        MPI_MonConvert (result, t0, n, n0);
    }
}

/* result <- (a ^ c) % n
   n MUST be an ODD MPI !!!
   Space of result MUST be at least 2*length of n !!!
   (result == a is permissible)
 */
void MPI_PowerMod (result, a, c, n)
CONST_MPI               a, c, n;
MPI                     result;
{
    U_WORD              e, et, m[5][MPI_MAXLEN+1], n0, *t, *t0, *t1;
    int                 j, k;

    if ((k = MPI_Len (c)) == 0) {
        MPI_AssignUWord (result, 1);
    }
    else {
        n0 = MPI_MonN0 (n);
        MPI_NResidue (m[0], a, n);
        MPI_MonSqr (m[1], m[0], n, n0);
        MPI_MonMul (m[2], m[0], m[1], n, n0);
        t0 = result, t1 = m[3];
        for (e = c[k--], j = 0; (et = e&MPI_POWERMODMSK) == 0
          && j < MPI_UWORDBITS/2; e <<= 2, ++j);
        MPI_Assign (t0, m[(et>>(MPI_UWORDBITS-2))-1]);
        e <<= 2;
        do {
            while (++j < MPI_UWORDBITS/2) {
                MPI_MonSqr (t1, t0, n, n0);
                MPI_MonSqr (t0, t1, n, n0);
                if ((et = e&MPI_POWERMODMSK)) {
                    MPI_MonMul
                      (t1, t0, m[(et>>(MPI_UWORDBITS-2))-1], n, n0);
                    t = t0; t0 = t1; t1 = t;
                }
                e <<= 2;
            }
            if (k > 0) {
                e = c[k], j = -1;
            }
            else {
                break;
            }
            --k;
        } while (TRUE);
        MPI_MonConvert (result, t0, n, n0);
    }
}

BOOL MPI_MillerTest (p, n)
CONST_MPI               p;
int                     n;
{
    U_WORD              a[2], m[MPI_MAXLEN+1], p_1[MPI_MAXLEN+1],
                        t[2][2*MPI_MAXLEN+1];
    int                 i, j, k, l, s;

    if (! MPI_Len (p)) {
        return (FALSE);
    }
    if (! MPI_CmpUWord (p, 1)) {
        return (FALSE);
    }
    if (! MPI_CmpUWord (p, 2)) {
        return (TRUE);
    }
    if (! (p[1]&1)) {
        return (FALSE);
    }
    for (i = 0; i < PRIMENUM; ++i) {
        if (! MPI_ModUWord (p, s_prime[i])) {
            return (FALSE);
        }
    }
    MPI_SubUWord3 (p_1, p, 1);
    MPI_Assign (m, p_1);
    for (j = 0; ! (a[0] = m[1+j]); ++j);
    for (j *= MPI_UWORDBITS; ! (a[0]&1); a[0] >>= 1, ++j);
    MPI_Shr (m, j);
    MPI_AssignUWord (a, 2);
    for (s = 0; s < n; ++s) {
        MPI_PowerMod (t[0], a, m, p);
        if ((MPI_CmpUWord (t[0], 1) && MPI_Cmp (t[0], p_1))) {
            for (k = 0, l = i = 1; i < j; ++i, k ^= 1, l ^= 1) {
                MPI_Sqr (t[l], t[k]);
                MPI_Mod (t[l], p);
                if (! MPI_CmpUWord (t[l], 1)) {
                    return (FALSE);
                }
                else if (! MPI_Cmp (t[l], p_1)) {
                    break;
                }
            }
            if (i == j) {
                return (FALSE);
            }
        }
        MPI_AssignUWord (a, s_prime[s]);
    }
    return (TRUE);
}

/* p should be randomized as seed while calling this function
   The input value of p is the start point for incremental prime search.
   bitLen >= 2
 */
void MPI_GenProbPrime (p, bitLen, pr)
GP_PROC                 pr;
MPI                     p;
int                     bitLen;
{
    U_WORD              highUWordMask, highestBit, step[PRIMENUM];
    int                 k, len, m;

    *p = (U_WORD)(len = (bitLen+MPI_UWORDBITS-1)/MPI_UWORDBITS);
    highestBit = ((U_WORD)1 << ((bitLen-1)%MPI_UWORDBITS));
    highUWordMask = (highestBit << 1)-1;
    p[1] |= 1;
    p[len] = (p[len] & highUWordMask) | highestBit;
    for (m = 0; m < PRIMENUM; ++m) {
        step[m] = MPI_ModUWord (p, s_prime[m]);
    }
    k = 0;
    do {
        if (! (k%5) && pr != (GP_PROC)0) {
            (*pr) ('.');
        }
        if (len > 1 || p[1] > s_prime[PRIMENUM-1]) {
            for (m = 0; m < PRIMENUM && step[m]; ++m);
            if (m == PRIMENUM) {
                if (MPI_MillerTest (p, MILLERDEPTH)) {
                    (*pr) ('*');
                    break;
                }
            }
            MPI_AddUWord (p, 2);
            if ((p[len] & highestBit)) {
                for (m = 0; m < PRIMENUM; ++m) {
                    step[m] += 2;
                    step[m] %= s_prime[m];
                }
            }
            else {
                p[len] = (p[len] & highUWordMask) | highestBit;
                for (m = 0; m < PRIMENUM; ++m) {
                    step[m] = MPI_ModUWord (p, s_prime[m]);
                }
            }
        }
        else {
            for (m = 0; m < PRIMENUM && p[1] > s_prime[m]; ++m);
            if (m < PRIMENUM && p[1] == s_prime[m]) {
                (*pr) ('*');
                break;
            }
            p[1] += 2;
            if (! (p[1] & highestBit)) {
                p[1] = highestBit | 1;
            }
        }
        k++;
    } while (TRUE);
}

/* p should be randomized as seed while calling this function
   lp is bit-length of p, lp >= 64 !!!
 */
BOOL MPI_GenStrongProbPrime (p, lp, opt, pr)
GP_PROC                 pr;
MPI                     p;
int                     lp, opt;
{
    U_WORD              i, i0, i1, j, j0, j1, p0[MPI_MAXLEN+1], r[MPI_MAXLEN/2+1],
                        s[MPI_MAXLEN/2+2], t[MPI_MAXLEN/2+1], u[MPI_MAXLEN+2],
                        v[MPI_MAXLEN+1], x, z[2];
    int                 li, lj, lr, ls, lt;

    lr = lp/2-5;
    for (x = K*(U_WORD)lr, li = 0; x; x >>= 1, ++li);
    for (x = K*(U_WORD)lp, lj = 0; x; x >>= 1, ++lj);
    ++li, ++lj;
    ls = lp-lr-lj-1, lt = lr-li-1;
    MPI_MemCpy (s+1, p+(lp-ls)/MPI_UWORDBITS+1, (ls+MPI_UWORDBITS-1)/MPI_UWORDBITS);
    MPI_GenProbPrime (s, ls, pr);
    /* Generate prime r. Note that ls > lt here, thus s > t is hold.
     */
    MPI_MemCpy (t+1, p+1, (lt+MPI_UWORDBITS-1)/MPI_UWORDBITS);
    do {
        MPI_GenProbPrime (t, lt, pr);
    } while (! MPI_GCD ((MPI)0, s, t));
    MPI_PowerOf2 (u, lr-1);
    MPI_SubUWord (u, 1);
    MPI_Div (u, t, z);
    i0 = z[1]/2+1;
    MPI_PowerOf2 (u, lr);
    MPI_SubUWord (u, 2);
    MPI_Div (u, t, z);
    i1 = z[1]/2;
    for (i = i0; i >= i0 && i <= i1; ++i) {
        if (! (i%10) && pr != (GP_PROC)0) {
            (*pr) ('-');
        }
        MPI_MulUWord (r, t, i);
        MPI_Shl (r, 1);
        ++r[1];
        if (MPI_MillerTest (r, MILLERDEPTH)) {
            (*pr) ('#');
            break;
        }
    }
    if (i < i0 || i > i1) {
        return (FALSE);
    }
    /* Compute p0
     */
    MPI_Assign (u, s);
    MPI_Mod (u, r);
    MPI_RightShiftAlgorithm (r, u, (MPI)0, v, 0);   /* v = s^-1 mod r */
    MPI_Mul (p0, v, s);
    MPI_Shl (p0, 1);
    MPI_SubUWord (p0, 1);
    /* Generate prime p
     */
    MPI_Mul (v, r, s);
    MPI_PowerOf2 (u, lp-1);
    MPI_Sub (u, p0);
    MPI_Div (u, v, z);
    j0 = z[1]/2+1;
    MPI_PowerOf2 (u, lp);
    MPI_SubUWord (u, 1);
    MPI_Sub (u, p0);
    MPI_Div (u, v, z);
    j1 = z[1]/2;
    for (j = j1; j >= j0 && j <= j1; --j) {     /* Count down to make the  */
        if (! (j%10) && pr != (GP_PROC)0) {     /*  prime # big enough to  */
            (*pr) ('-');                        /*  satisfy the bit-length */
        }                                       /*  requirement.           */
        MPI_MulUWord (u, v, j);
        MPI_Shl (u, 1);
        MPI_Add (u, p0);
        if (opt == MPI_GSPP_RSA && ! (u[1]&2)) {
            continue;
        }
        if (MPI_MillerTest (u, MILLERDEPTH)) {
            (*pr) ('#');
            break;
        }
    }
    if (j < j0 || j > j1) {
        return (FALSE);
    }
    MPI_Assign (p, u);
    return (TRUE);
}

void MPI_GenRandomMPI (mpi, len, rnd)
RND_PROC                rnd;
MPI                     mpi;
int                     len;
{
    U_WORD              u;
    int                 i, k;

    for (u = 0, i = 0; i < len; mpi[++i] = u) {
        for (u = 0, k = 0; k < MPI_UWORDBYTES; ++k, u = (u << 8)|(BYTE)(*rnd)());
    }
    while (! u) {
        for (u = 0, k = 0; k < MPI_UWORDBYTES; ++k, u = (u << 8)|(BYTE)(*rnd)());
        mpi[len] = u;
    }
    *mpi = (U_WORD)len;
}
