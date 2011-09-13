/*******************************************************************
  mpialgo.c

    Multi-Precision Integer (MPI) Basic Arithmetics in ANSI C
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    ANY MODIFICATION MADE TO THIS SOURCE CODE IS AT YOUR OWN RISK.
    Please ask the author for the permission.
 *******************************************************************/

#include    "mpidefs.h"
#include    "mpibasic.c"
#include    "mpialgoc.h"		/* Shijianjun adds this line */
					/* for definition MPI_MAXLEN */
#include    "mpialgo.h"


static int Mpi_CmpArray PROTO_LIST ((const U_WORD *, const U_WORD *, int));
static U_WORD Mpi_SubArray PROTO_LIST ((U_WORD *, const U_WORD *, int));
static void Mpi_MonAlgorithm PROTO_LIST ((MPI, MPI, CONST_MPI, U_WORD));


static int Mpi_CmpArray (n1, n2, len)
register const U_WORD   *n1, *n2;
register int            len;
{
    do {
        --len;
        if ((n1[len]-n2[len])) {
            break;
        }
    } while (len > 0);
    if (n1[len] > n2[len]) {
        return (1);
    }
    else if (n1[len] < n2[len]) {
        return (-1);
    }
    return (0);
}

static U_WORD Mpi_SubArray (s1, s2, len)
register U_WORD         *s1;
register const U_WORD   *s2;
register int            len;
{
    register U_WORD     borrow;
    register int        i;

    borrow = 0;
    i = 0;
    do {
        UWORD_SBB (borrow, s1[i], s2[i], borrow);
    } while (++i < len);
    return (borrow);
}

/* len is the length of byteStr in unit of BYTE.
   return:  length of MPI in unit of U_WORD.
   (mpi == byteStr is permitted)
 */
int MPI_ByteStringToMPI (mpi, byteStr, len)
register MPI            mpi;
register const BYTE     *byteStr;
register int            len;
{
    register U_WORD     u;
    register int        i, j, k;

    for (; len > 0 && ! *byteStr; ++byteStr, --len);
    if (! len) {
        return ((int)(*mpi = 0));
    }
    for (j = 0, k = (len-1)%MPI_UWORDBYTES, i = (len+MPI_UWORDBYTES-1)/MPI_UWORDBYTES;
      i > 0; mpi[i--] = u, k = MPI_UWORDBYTES-1) {
        for (u = 0; k >= 0; --k, u = (u << 8)|byteStr[j++]);
    }
    return ((int)(*mpi = (U_WORD)((len+MPI_UWORDBYTES-1)/MPI_UWORDBYTES)));
}

/* len is the length of byteStr in unit of BYTE.
   return:  length of MPI in unit of U_WORD.
   (mpi == byteStr is permitted)
 */
int MPI_MPIToByteString (byteStr, mpi, len)
register BYTE           *byteStr;
register CONST_MPI      mpi;
register int            len;
{
    register U_WORD     u;
    register int        i, j, k, m;

    if ((i = (len+MPI_UWORDBYTES-1)/MPI_UWORDBYTES) <= (int)*mpi) {
        j = 0, m = (len-1)%MPI_UWORDBYTES;
    }
    else {
	/*
	 * Now j will be the starting point in buffer byteStr where the
	 * value saved in mpi will be filled in. BUT THERE IS AN ERROR
	 * in the original coding, Brother Zhuang hao used this code:
	 *
         *	j = (i-(int)*mpi)*MPI_UWORDBYTES;
	 *
	 * But j will not be the exact value. I think the right coding is
	 *
	 *	j = len - MPI_UWORDBYTES*(int)*mpi;
	 *
	 * 					Shijianjun, 98/09
	 */
	j = len - MPI_UWORDBYTES*(int)*mpi;
        T_memset (byteStr, 0, j);
        m = MPI_UWORDBYTES-1, i = (int)*mpi;
    }
    for (; i > 0; --i, j += m+1, m = MPI_UWORDBYTES-1) {
        for (u = mpi[i], k = m; k >= 0; byteStr[j+(k--)] = (BYTE)u, u >>= 8);
    }
    return ((len+MPI_UWORDBYTES-1)/MPI_UWORDBYTES);
}

/* Compares MPI n1 with n2
   return:  0 -- n1 == n2;  > 0 -- n1 > n2;  < 0 -- n1 < n2
 */
int MPI_Cmp (n1, n2)
register CONST_MPI      n1, n2;
{
    register int        l;

    if (*n1 != *n2) {
        return ((int)((*n1)-(*n2)));
    }
    for (l = (int)*n1; l > 1 && n1[l] == n2[l]; --l);
    if (n1[l] > n2[l]) {
        return (1);
    }
    else if (n1[l] < n2[l]) {
        return (-1);
    }
    return (0);
}

/* Compares MPI n1 with (U_WORD)n2
   return:  0 -- n1 == n2;  > 0 -- n1 > n2;  < 0 -- n1 < n2
 */
int MPI_CmpUWord (n1, n2)
register CONST_MPI      n1;
register U_WORD         n2;
{
    if (! *n1) {
        if (n2) {
            return (-1);
        }
        return (0);
    }
    if (*n1 > 1) {
        return (1);
    }
    if (n1[1] > n2) {
        return (1);
    }
    else if (n1[1] < n2) {
        return (-1);
    }
    return (0);
}

/* Get the byte-length of MPI. (omitting the leading zeros)
   MPI_ByteLen (n) == 0 is equivalent to n == 0
 */
int MPI_ByteLen (n)
register CONST_MPI      n;
{
    U_WORD              u;
    int                 l;

    if ((l = MPI_Len (n))) {
        for (u = n[l], l = (l-1)*MPI_UWORDBYTES; u; ++l, u >>= 8);
    }
    return (l);
}

/* Get the bit-length of MPI. (omitting the leading zeros)
   MPI_BitLen (n) == 0 is equivalent to n == 0
 */
int MPI_BitLen (n)
register CONST_MPI      n;
{
    U_WORD              u;
    int                 l;

    if ((l = MPI_Len (n))) {
        for (u = n[l], l = (l-1)*MPI_UWORDBITS; u; ++l, u >>= 1);
    }
    return (l);
}

/* a1 <- a1 + a2
 */
U_WORD MPI_Add (a1, a2)
register CONST_MPI      a2;
register MPI            a1;
{
    register U_WORD     carry;
    register int        i, ls;

    for (ls = (int)(*a1 < *a2 ? *a1 : *a2), carry = 0, i = 1; i <= ls; ++i) {
        UWORD_ADC (carry, a1[i], a2[i], carry);
    }
    for (; i <= (int)*a1 && carry; ++i) {
        if (++a1[i]) {
            carry = 0;
        }
    }
    for (; i <= (int)*a2 && carry; ++i) {
        if ((a1[i] = a2[i]+1)) {
            carry = 0;
        }
    }
    for (; i <= (int)*a2; a1[i] = a2[i], ++i);
    if (*a1 < *a2) {
        *a1 = *a2;
    }
    if (carry) {
        a1[(int)(++(*a1))] = 1;
    }
    return (carry);
}

/* s <- a1 + a2
 */
U_WORD MPI_Add3 (s, a1, a2)
register CONST_MPI      a1, a2;
register MPI            s;
{
    register U_WORD     carry;
    register int        i, ls;

    for (ls = (int)(*a1 < *a2 ? *a1 : *a2), carry = 0, i = 1; i <= ls; ++i) {
        UWORD_ADC3 (carry, s[i], a1[i], a2[i], carry);
    }
    for (; i <= (int)*a1 && carry; ++i) {
        if ((s[i] = a1[i]+1)) {
            carry = 0;
        }
    }
    for (; i <= (int)*a1; s[i] = a1[i], ++i);
    for (; i <= (int)*a2 && carry; ++i) {
        if ((s[i] = a2[i]+1)) {
            carry = 0;
        }
    }
    for (; i <= (int)*a2; s[i] = a2[i], ++i);
    *s = *a1 > *a2 ? *a1 : *a2;
    if (carry) {
        s[(int)(++(*s))] = 1;
    }
    return (carry);
}

/* a1 <- a1 + (U_WORD)a2
 */
U_WORD MPI_AddUWord (a1, a2)
U_WORD                  a2;
register MPI            a1;
{
    register U_WORD     carry;
    register int        i;

    if (! *a1) {
        MPI_AssignUWord (a1, a2);
        return (0);
    }
    UWORD_ADD (carry, a1[1], a2);
    for (i = 2; i <= (int)*a1 && carry; ++i) {
        if ((++a1[i])) {
            carry = 0;
        }
    }
    if (carry) {
        a1[(int)(++(*a1))] = 1;
    }
    return (carry);
}

/* s <- a1 + (U_WORD)a2
 */
U_WORD MPI_AddUWord3 (s, a1, a2)
U_WORD                  a2;
register CONST_MPI      a1;
register MPI            s;
{
    register U_WORD     carry;
    register int        i;

    if (! *a1) {
        MPI_AssignUWord (s, a2);
        return (0);
    }
    *s = *a1;
    UWORD_ADD3 (carry, s[1], a1[1], a2);
    for (i = 2; i <= (int)*a1 && carry; ++i) {
        if ((s[i] = a1[i]+1)) {
            carry = 0;
        }
    }
    for (; i <= (int)*a1; s[i] = a1[i], ++i);
    if (carry) {
        s[(int)(++(*s))] = 1;
    }
    return (carry);
}

/* s1 <- s1 - s2
 */
U_WORD MPI_Sub (s1, s2)
register CONST_MPI      s2;
register MPI            s1;
{
    register U_WORD     borrow;
    register int        i, ls;

    for (ls = (int)(*s1 < *s2 ? *s1 : *s2), borrow = 0, i = 1; i <= ls; ++i) {
        UWORD_SBB (borrow, s1[i], s2[i], borrow);
    }
    for (; i <= (int)*s1 && borrow; ++i) {
        if (--s1[i] != MPI_MAXUWORD) {
            borrow = 0;
        }
    }
    for (; i <= (int)*s2 && ! borrow; ++i) {
        if ((s1[i] = 0-s2[i])) {
            borrow = 1;
        }
    }
    for (; i <= (int)*s2; s1[i] = ~s2[i], ++i);
    if (! borrow) {
        for (; i > 1 && ! s1[i-1]; --i);
    }
    if (*s1 < *s2) {
        *s1 = *s2;
    }
    for (; *s1 > 0 && ! s1[(int)*s1]; --*s1);
    return (borrow);
}

/* d <- s1 - s2
 */
U_WORD MPI_Sub3 (d, s1, s2)
register CONST_MPI      s1, s2;
register MPI            d;
{
    register U_WORD     borrow;
    register int        i, ls;

    for (ls = (int)(*s1 < *s2 ? *s1 : *s2), borrow = 0, i = 1; i <= ls; ++i) {
        UWORD_SBB3 (borrow, d[i], s1[i], s2[i], borrow);
    }
    for (; i <= (int)*s1 && borrow; ++i) {
        if ((d[i] = s1[i]-1) != MPI_MAXUWORD) {
            borrow = 0;
        }
    }
    for (; i <= (int)*s1; d[i] = s1[i], ++i);
    for (; i <= (int)*s2 && ! borrow; ++i) {
        if ((d[i] = 0-s2[i])) {
            borrow = 1;
        }
    }
    for (; i <= (int)*s2; d[i] = ~s2[i], ++i);
    if (! borrow) {
        for (; i > 1 && ! d[i-1]; --i);
    }
    *d = *s1 > *s2 ? *s1 : *s2;
    for (; *d > 0 && ! d[(int)*d]; --*d);
    return (borrow);
}

/* s1 <- s1 - (U_WORD)s2
 */
U_WORD MPI_SubUWord (s1, s2)
U_WORD                  s2;
register MPI            s1;
{
    register U_WORD     borrow;
    register int        i;

    if (! *s1) {
        MPI_AssignUWord (s1, 0-s2);
        return (s2 ? 1 : 0);
    }
    UWORD_SUB (borrow, s1[1], s2);
    for (i = 2; i <= (int)*s1 && borrow; ++i) {
        if (--s1[i] != MPI_MAXUWORD) {
            borrow = 0;
        }
    }
    if (! s1[(int)*s1]) {
        --*s1;
    }
    return (borrow);
}

/* d <- s1 - (U_WORD)s2
 */
U_WORD MPI_SubUWord3 (d, s1, s2)
U_WORD                  s2;
register CONST_MPI      s1;
register MPI            d;
{
    register U_WORD     borrow;
    register int        i;

    if (! *s1) {
        MPI_AssignUWord (d, 0-s2);
        return (s2 ? 1 : 0);
    }
    *d = *s1;
    UWORD_SUB3 (borrow, d[1], s1[1], s2);
    for (i = 2; i <= (int)*s1 && borrow; ++i) {
        if ((d[i] = s1[i]-1) != MPI_MAXUWORD) {
            borrow = 0;
        }
    }
    for (; i <= (int)*s1; d[i] = s1[i], ++i);
    if (! d[(int)*d]) {
        --*d;
    }
    return (borrow);
}

/* product <- m1 * m2
 */
void MPI_Mul (product, m1, m2)
CONST_MPI               m1, m2;
MPI                     product;
{
    MPI_UWORD_MUL_DECLARE
    MPI                 p;
    register U_WORD     c;
    register int        k;

    if (! *m1 || ! *m2) {
        *product = 0;
    }
    else {
        *product = (*m1)+(*m2);
        p = product+1;
        MPI_UWORD_MUL (p, m1+1, m2[1], (int)*m1, c);
        for (k = (int)*m2; --k > 0; ) {
            p[(int)(*m1)] = c;
            ++p;
            ++m2;
            MPI_UWORD_MUL_ADD (p, m1+1, m2[1], (int)*m1, c);
        }
        if (c) {
            p[(int)(*m1)] = c;
        }
        else {
            --*product;
        }
    }
}

/* product <- m1 * (U_WORD)m2
 */
void MPI_MulUWord (product, m1, m2)
CONST_MPI               m1;
MPI                     product;
U_WORD                  m2;
{
    MPI_UWORD_MUL_DECLARE
    register U_WORD     c;

    MPI_UWORD_MUL (product+1, m1+1, m2, (int)*m1, c);
    if (c) {
        *product = 1+(*m1), product[1+(int)(*m1)] = c;
    }
    else {
        *product = *m1;
    }
}

/* product <- m * m
 */
void MPI_Sqr (product, m)
CONST_MPI               m;
MPI                     product;
{
    MPI_UWORD_SQR_DECLARE
    MPI                 p;
    int                 len;
    register U_WORD     hc;

    if ((*product = (*m)+(*m))) {
        p = product+1;
        len = (int)*m, ++m;
        MPI_UWORD_SQR (p, m, len, hc);
        while (--len > 0) {
            p += 2;
            ++m;
            p[len] = hc;
            MPI_UWORD_SQR_ADD (p, m, len, hc);
        }
        if (! p[1]) {
            --*product;
        }
    }
}

/* n *= 2^k
 */
void MPI_Shl (n, k)
register MPI            n;
register int            k;
{
    U_WORD              x;
    register int        i, m;

    if (*n) {
        if ((m = k/MPI_UWORDBITS)) {
            for (i = (int)*n; i > 0; n[i+m] = n[i], ++i);
            for (i = 0; i < m; n[++i] = 0);
            *n += (U_WORD)m;
        }
        if ((k %= MPI_UWORDBITS) && *n) {
            x = n[i = (int)*n] >> (MPI_UWORDBITS-k);
            for (; i > 1; --i) {
                UWORD_SHL (n[i], n[i], n[i-1], k);
            }
            n[1] <<= k;
            if (x) {
                n[(int)(++(*n))] = x;
            }
        }
    }
}

/* n /= 2^k
 */
U_WORD MPI_Shr (n, k)
register MPI            n;
register int            k;
{
    U_WORD              r;
    register int        i, m;

    r = 0;
    if ((m = k/MPI_UWORDBITS) && *n) {
        if (m < (int)*n) {
            r = n[m];
            for (i = 0; i < (int)(*n); n[1+i] = n[1+i+m], ++i);
            *n -= (U_WORD)m;
        }
        else {
            *n = 0;
        }
    }
    if ((k %= MPI_UWORDBITS) && *n) {
        r = (r >> k) | (n[1] << (MPI_UWORDBITS-k));
        for (i = 1; i < (int)*n; ++i) {
            UWORD_SHR (n[i], n[i+1], n[i], k);
        }
        if (! (n[i] >>= k)) {
            --(*n);
        }
    }
    return (r);
}

/* quo <- dvdnd / dvdr
   dvdnd <- dvdnd % dvdr
      & high order U_WORDs of dvdnd are all cleared with 0.
 */
/* int MPI_Div (dvdnd, dvdr, quo) -- this is the version of ZHUANGHAO */
int MPI_Div (dvdnd, /* dvdr */ divider, quo)  /* Version of SJJ */
MPI                     quo;
register CONST_MPI      /* dvdr */ divider;
register MPI            dvdnd;
{
    UWORD_DIV_DECLARE
    MPI_UWORD_MUL_DECLARE
    U_WORD              d, q, u;
    int                 dvdndLen, dvdrLen, i, k, n;
    register U_WORD     b;
    /***********************************************************************
     * Because MPI_Div changes the dvdr value during the processing, so it *
     * can not be used in multi-thread program. Now i use a copy of dvdr   *
     * to be the real divider.                                             *
     *                                           Shijianjun, 98.10.05      *
     ***********************************************************************/
    U_WORD		dvdr[ MPI_MAXLEN + 1 ];	  /* SJJ adds this line */

    MPI_Assign( dvdr, divider );

    /* The following codes remain untouched. They are what actually presented
     * in ZHUANGHAO's original mpialgo.c. But, the variable "dvdr" is a 
     * local variable now, so MPI_Div is multi-thread safe.
     *                                           Shijianjun, 98.10.05
     */

    if ((dvdrLen = MPI_Len (dvdr)) == 0) {
        return (1);
    }
    if ((i = MPI_Cmp (dvdnd, dvdr)) < 0) {
        *quo = 0;
        return (0);
    }
    else if (! i) {
        *quo = quo[1] = 1, *dvdnd = 0;
        return (0);
    }
    dvdndLen = MPI_Len (dvdnd);
    for (u = dvdr[dvdrLen], k = 0; (u & MPI_UWORDMSB) == 0; u <<= 1, ++k);
    MPI_Shl ((MPI)dvdr, k);
    u = 0;
    if (k) {
        u = dvdnd[i = dvdndLen] >> (MPI_UWORDBITS-k);
        for (; i > 1; --i) {
            UWORD_SHL (dvdnd[i], dvdnd[i], dvdnd[i-1], k);
        }
        dvdnd[1] <<= k;
    }
    d = dvdr[dvdrLen];
    n = dvdndLen;
    *quo = (U_WORD)(i = dvdndLen-dvdrLen+1);
    do {
        if (d == MPI_MAXUWORD) {
            q = u;
        }
        else {
            UWORD_DIVF (q, u, dvdnd[n], d+1);
        }
        MPI_UWORD_MUL_SUB (dvdnd+i, dvdr+1, q, dvdrLen, b);
        u -= b;
        while (u > 0 || Mpi_CmpArray (dvdnd+i, dvdr+1, dvdrLen) >= 0) {
            ++q;
            u -= Mpi_SubArray (dvdnd+i, dvdr+1, dvdrLen);
        }
        u = dvdnd[n];
        dvdnd[n] = 0;
        quo[i] = q;
        --n;
    } while (--i > 0);
    dvdnd[n+1] = u;
    *dvdnd = dvdrLen;
    MPI_Shr (dvdnd, k);
    MPI_Shr ((MPI)dvdr, k);
    for (; *dvdnd > 0 && ! dvdnd[(int)*dvdnd]; --*dvdnd);
    for (; *quo > 0 && ! quo[(int)*quo]; --*quo);
    return (0);
}

/* quo <- dvdnd / (U_WORD)dvdr
   return: dvdnd % (U_WORD)dvdr ( == MPI_MAXUWORD if divided by 0 !)
 */
U_WORD MPI_DivUWord (dvdnd, dvdr, quo)
MPI                     quo;
register CONST_MPI      dvdnd;
register U_WORD         dvdr;
{
    UWORD_DIV_DECLARE
    U_WORD              u, v;
    int                 i;

    if (dvdr == 0) {
        return (MPI_MAXUWORD);
    }
    if ((i = MPI_Len (dvdnd)) == 1) {
        u = dvdnd[1]%dvdr;                      /* Allows quo == dvdnd */
        *quo = (quo[1] = dvdnd[1]/dvdr) ? 1 : 0;
        return (u);
    }
    *quo = (U_WORD)i;
    for (u = 0; i > 0; --i) {
        UWORD_DIVMOD (u, v, u, dvdnd[i], dvdr);
        quo[i] = v;
    }
    for (; *quo > 0 && ! quo[(int)*quo]; --*quo);
    return (u);
}

/* dvdnd <- dvdnd % dvdr
      & high order U_WORDs of dvdnd are all cleared with 0.
 */
/* int MPI_Mod (dvdnd, dvdr) -- Version of Zhuanghao */
int MPI_Mod (dvdnd, /* dvdr */ divider )  /* Version of SJJ */
register CONST_MPI      /* dvdr */ divider ;
register MPI            dvdnd;
{
    UWORD_DIV_DECLARE
    MPI_UWORD_MUL_DECLARE
    U_WORD              d, q, u;
    int                 dvdndLen, dvdrLen, i, k, n;
    register U_WORD     b;

    /***********************************************************************
     * Because MPI_Mod changes the dvdr value during the processing, so it *
     * can not be used in multi-thread program. Now i use a copy of dvdr   *
     * to be the real divider.                                             *
     *                                           Shijianjun, 98.10.05      *
     ***********************************************************************/
    U_WORD		dvdr[ MPI_MAXLEN + 1 ];	  /* SJJ adds this line */

    MPI_Assign( dvdr, divider );

    /* The following codes remain untouched. They are what actually presented
     * in ZHUANGHAO's original mpialgo.c. But, the variable "dvdr" is a 
     * local variable now, so MPI_Div is multi-thread safe.
     *                                           Shijianjun, 98.10.05
     */
    if ((dvdrLen = MPI_Len (dvdr)) == 0) {
        return (1);
    }
    if ((i = MPI_Cmp (dvdnd, dvdr)) < 0) {
        return (0);
    }
    else if (! i) {
        *dvdnd = 0;
        return (0);
    }
    dvdndLen = MPI_Len (dvdnd);
    for (u = dvdr[dvdrLen], k = 0; (u & MPI_UWORDMSB) == 0; u <<= 1, ++k);
    MPI_Shl ((MPI)dvdr, k);
    u = 0;
    if (k) {
        u = dvdnd[i = dvdndLen] >> (MPI_UWORDBITS-k);
        for (; i > 1; --i) {
            UWORD_SHL (dvdnd[i], dvdnd[i], dvdnd[i-1], k);
        }
        dvdnd[1] <<= k;
    }
    d = dvdr[dvdrLen];
    n = dvdndLen;
    i = dvdndLen-dvdrLen+1;
    do {
        if (d == MPI_MAXUWORD) {
            q = u;
        }
        else {
            UWORD_DIVF (q, u, dvdnd[n], d+1);
        }
        MPI_UWORD_MUL_SUB (dvdnd+i, dvdr+1, q, dvdrLen, b);
        u -= b;
        while (u > 0 || Mpi_CmpArray (dvdnd+i, dvdr+1, dvdrLen) >= 0) {
            u -= Mpi_SubArray (dvdnd+i, dvdr+1, dvdrLen);
        }
        u = dvdnd[n];
        dvdnd[n] = 0;
        --n;
    } while (--i > 0);
    dvdnd[n+1] = u;
    *dvdnd = dvdrLen;
    MPI_Shr (dvdnd, k);
    MPI_Shr ((MPI)dvdr, k);
    for (; *dvdnd > 0 && ! dvdnd[(int)*dvdnd]; --*dvdnd);
    return (0);
}

/* return: dvdnd % (U_WORD)dvdr ( == MPI_MAXUWORD if divided by 0 !)
 */
U_WORD MPI_ModUWord (dvdnd, dvdr)
register CONST_MPI      dvdnd;
register U_WORD         dvdr;
{
    UWORD_DIV_DECLARE
    U_WORD              t, u;
    int                 i;

    if (dvdr == 0) {
        return (MPI_MAXUWORD);
    }
    if ((i = MPI_Len (dvdnd)) == 1) {
        return (dvdnd[1]%dvdr);
    }
    for (u = 0; i > 0; --i) {
        UWORD_DIVMOD (u, t, u, dvdnd[i], dvdr);
    }
    return (u);
}

/* return: least significant word of the -n^-1
   used by the Montgomery Algorithm
 */
U_WORD MPI_MonN0 (n)
register CONST_MPI      n;
{
    register U_WORD     n0, x, y, z;
    register int        i;

    n0 = n[1];
    x = y = z = 1;
    i = 1;
    do {
        x <<= 1;
        z <<= 1;
        z |= 1;
        if (((n0*y) & z) > x) {
            y += x;
        }
    } while (++i < MPI_UWORDBITS);
    return ((U_WORD)0-y);
}

/* Montgomery Algorithm
   Input:   t       -- input product (length of t MUST NOT exceed twice that
                       of n !!!)
            n       -- module (MUST be ODD !!!)
            n0      -- return value of MonInverse
   Output:  p       -- the result (p == t is permissible)
 */
static void Mpi_MonAlgorithm (p, t, n, n0)
MPI                     p, t;
U_WORD                  n0;
register CONST_MPI      n;
{
    MPI_UWORD_MUL_DECLARE
    register U_WORD     m, c, c1;
    register int        i;

    if (*t < (*n)+(*n)) {
        MPI_MemSet0 (t+1+(int)*t, (int)((*n)+(*n)-(*t)));
    }
    for (++t, c1 = 0, i = 0; i < (int)*n; ++i, ++t) {
        m = (*t)*n0;
        MPI_UWORD_MUL_ADD (t, n+1, m, (int)*n, c);
        UWORD_ADD (c1, c, c1);
        UWORD_ADD_ADDC (c1, t[(int)*n], c);
    }
    for (i = 0; i < (int)*n; p[1+i] = t[i], ++i);
    if (c1 != 0 || Mpi_CmpArray (t, n+1, (int)*n) > 0) {
        Mpi_SubArray (p+1, n+1, (int)*n);
    }
    *p = *n;
    for (; *p > 0 && ! p[(int)*p]; --*p);
}

/* p <- MonPro (a, b)
   Length of a*b MUST NOT exceed twice that of n !!!
   n MUST be an ODD MPI !!!
 */
void MPI_MonMul (p, a, b, n, n0)
CONST_MPI               a, b, n;
MPI                     p;
U_WORD                  n0;
{
    MPI_Mul (p, a, b);
    Mpi_MonAlgorithm (p, p, n, n0);
}

/* p <- MonSqr (a)
   Length of a*a MUST NOT exceed twice that of n !!!
   n MUST be an ODD MPI !!!
 */
void MPI_MonSqr (p, a, n, n0)
CONST_MPI               a, n;
MPI                     p;
U_WORD                  n0;
{
    MPI_Sqr (p, a);
    Mpi_MonAlgorithm (p, p, n, n0);
}

/* na <- n-residue of a
   (na == a is permissible)
 */
void MPI_NResidue (na, a, n)
CONST_MPI               a, n;
MPI                     na;
{
    int                 i, l;

    for (l = (int)((*n)+(*a)), i = 0; i < (int)*a; na[l-i] = a[(int)(*a)-i], ++i);
    MPI_MemSet0 (na+1, (int)*n);
    *na = (U_WORD)l;
    MPI_Mod (na, n);
}

/* p <- orginal MPI of n-residue a
   (p == a is permissible)
 */
void MPI_MonConvert (p, a, n, n0)
CONST_MPI               a, n;
MPI                     p;
U_WORD                  n0;
{
    MPI_Assign (p, a);
    Mpi_MonAlgorithm (p, p, n, n0);
}

/* n = 2^k
 */
void MPI_PowerOf2 (n, k)
MPI                     n;
int                     k;
{
    int                 m;

    if ((m = k/MPI_UWORDBITS)) {
        MPI_MemSet0 (n+1, m);
    }
    k %= MPI_UWORDBITS;
    n[1+m] = (U_WORD)1 << k;
    *n = (U_WORD)(m+1);
}
