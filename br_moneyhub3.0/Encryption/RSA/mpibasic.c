/*******************************************************************
  mpibasic.c

    Basic Arithmetics for U_WORD
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    You are encouraged to add you own code, especially assemblies,
    to enhance performance.
    PLEASE ADD THE CODE TO WHERE YOU ARE TOLD TO.
 *******************************************************************/

#ifndef __SS_BASIC_ARITHMETIC__
#define __SS_BASIC_ARITHMETIC__ 1

#include    "mpidefs.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifndef _MPI_NON_ANSI
/*******************************************************************
    ANY MODIFICATION MADE TO THIS SECTION IS AT YOUR OWN RISK.
 *******************************************************************/

U_WORD                  *q, xh, xl, y;
/* Macro to declare temp variables for UWORD_MUL & UWORD_SQR
 */
#define UWORD_MUL_DECLARE \
    U_WORD              __tmp1, __tmp2, __tmp3, __tmp4; \
    register U_WORD     __rtmp1, __rtmp2;

/* Macro to declare temp variables for UWORD_DIV
 */
#define UWORD_DIV_DECLARE

/* Macro to declare temp variables for MPI_UWORD_MUL, MPI_UWORD_MUL_ADD,
   MPI_UWORD_MUL_SUB
 */
#define MPI_UWORD_MUL_DECLARE \
    UWORD_MUL_DECLARE \
    register U_WORD     __tmp_c, __tmp_d; \
    register int        __tmp_i;

/* Macro to declare temp variables for MPI_UWORD_SQR, MPI_UWORD_SQR_ADD
 */
#define MPI_UWORD_SQR_DECLARE \
    UWORD_MUL_DECLARE \
    register U_WORD     __tmp_c, __tmp_d, __tmp_t, __tmp_u; \
    register int        __tmp_i;


static U_WORD __UWORD_DIVF PROTO_LIST ((U_WORD *, U_WORD, U_WORD, U_WORD));
static U_WORD __UWORD_DIV PROTO_LIST ((U_WORD *, U_WORD, U_WORD, U_WORD));


/***************************************************************
 * Basic Arithmetics in ANSI C for MPI Algorithms
 */

/******** ADD ********
 Permitted:     &c == &c0, &s == &x
 Forbidden:     &s = &y
                c, c0 MUST be simple variables !!! (NOT *a, a[1], a+b, ...)
 */
/* s = x + y
   c = carry
   Fastest when y is simple addressed (NOT array, etc.)
 */
#define UWORD_ADD3(c, s, x, y) \
    c = ((s = (x)+(y)) < (y)) ? 1 : 0

/* s = x + y
   c += carry   (c can be greater than 1)
   Fastest when y is simple addressed (NOT array, etc.)
 */
#define UWORD_ADD3_ADDC(c, s, x, y) \
    if ((s = (x)+(y)) < (y)) { \
        ++c; \
    }

/* x += y
   c = carry
   Fastest when y is simple addressed (NOT array, etc.)
   (&x != &y !!!)
 */
#define UWORD_ADD(c, x, y) \
    c = ((x += (y)) < (y)) ? 1 : 0

/* x += y
   c += carry   (c can be greater than 1)
   Fastest when y is simple addressed (NOT array, etc.)
   (&x != &y !!!)
 */
#define UWORD_ADD_ADDC(c, x, y) \
    if ((x += (y)) < (y)) { \
        ++c; \
    }

/* s = x + y + c0
   c = carry    (c, c0 == 0 or 1)
   Fastest when c0, y is simple addressed (NOT array, etc.)
 */
#define UWORD_ADC3(c, s, x, y, c0) \
    if ((s = (x)+(c0)) < (c0)) { \
        s = (y), c = 1; \
    } \
    else { \
        c = ((s += (y)) < (y)) ? 1 : 0; \
    }

/* s = x + y + c0
   c += carry   (c & c0 can be greater than 1)
   Fastest when c0, y is simple addressed (NOT array, etc.)
 */
#define UWORD_ADC3_ADDC(c, s, x, y, c0) \
    if ((s = (x)+(c0)) < (c0)) { \
        ++c; \
    } \
    if ((s += (y)) < (y)) { \
        ++c; \
    }

/* x += y + c0
   c = carry    (c, c0 == 0 or 1)
   Fastest when c0, y is simple addressed (NOT array, etc.)
   (&x != &y !!!)
 */
#define UWORD_ADC(c, x, y, c0) \
    if ((x += (c0)) < (c0)) { \
        x = (y), c = 1; \
    } \
    else { \
        c = ((x += (y)) < (y)) ? 1 : 0; \
    }

/* x += y + c0
   c += carry   (c & c0 can be greater than 1)
   Fastest when c0, y is simple addressed (NOT array, etc.)
   (&x != &y !!!)
 */
#define UWORD_ADC_ADDC(c, x, y, c0) \
    if ((x += (c0)) < (c0)) { \
        ++c; \
    } \
    if ((x += (y)) < (y)) { \
        ++c; \
    }

/******** SUB ********
 Permitted:     &b == &b0, &d == &x
 Forbidden:     &d == &y
                b, b0 MUST be simple variables !!! (NOT *a, a[1], a+b, ...)
 */
/* d = x - y
   b = borrow
   Fastest when y is simple addressed (NOT array, etc.)
   (d = x-y) > MPI_MAXUWORD-y is better than (d = x-y) > x
   because it allows &d == &x !
 */
#define UWORD_SUB3(b, d, x, y) \
    b = ((d = (x)-(y)) > MPI_MAXUWORD-(y)) ? 1 : 0

/* d = x - y
   b += borrow  (b can be greater than 1)
   Fastest when y is simple addressed (NOT array, etc.)
   (d = x-y) > MPI_MAXUWORD-y is better than (d = x-y) > x
   because it allows &d == &x !
 */
#define UWORD_SUB3_ADDB(b, d, x, y) \
    if ((d = (x)-(y)) > MPI_MAXUWORD-(y)) { \
        ++b; \
    }

/* x -= y
   b = borrow
   Fastest when y is simple addressed (NOT array, etc.)
   (&x != &y !!!)
 */
#define UWORD_SUB(b, x, y) \
    b = ((x -= (y)) > MPI_MAXUWORD-(y)) ? 1 : 0

/* x -= y
   b += borrow  (b can be greater than 1)
   Fastest when y is simple addressed (NOT array, etc.)
   (&x != &y !!!)
 */
#define UWORD_SUB_ADDB(b, x, y) \
    if ((x -= (y)) > MPI_MAXUWORD-(y)) { \
        ++b; \
    }

/* d = x - y - b0
   b = borrow   (b, b0 == 0 or 1)
   Fastest when b0, y is simple addressed (NOT array, etc.)
   (d = x-y) > MPI_MAXUWORD-y is better than (d = x-y) > x
   because it allows &d == &x !
 */
#define UWORD_SBB3(b, d, x, y, b0) \
    if ((d = (x)-(b0)) > MPI_MAXUWORD-(b0)) { \
        d -= (y), b = 1; \
    } \
    else { \
        b = ((d -= (y)) > MPI_MAXUWORD-(y)) ? 1 : 0; \
    }

/* d = x - y - b0
   b += borrow  (b & b0 can be greater than 1)
   Fastest when b0, y is simple addressed (NOT array, etc.)
   (d = x-y) > MPI_MAXUWORD-y is better than (d = x-y) > x
   because it allows &d == &x !
 */
#define UWORD_SBB3_ADDB(b, d, x, y, b0) \
    if ((d = (x)-(b0)) > MPI_MAXUWORD-(b0)) { \
        ++b; \
    } \
    if ((d -= (y)) > MPI_MAXUWORD-(y)) { \
        ++b; \
    }

/* x -= (y + b0)
   b = borrow   (b, b0 == 0 or 1)
   Fastest when b0, y is simple addressed (NOT array, etc.)
   (d = x-y) > MPI_MAXUWORD-y is better than (d = x-y) > x
   because it allows &d == &x !
   (&x != &y !!!)
 */
#define UWORD_SBB(b, x, y, b0) \
    if ((x -= (b0)) > MPI_MAXUWORD-(b0)) { \
        x -= (y), b = 1; \
    } \
    else { \
        b = ((x -= (y)) > MPI_MAXUWORD-(y)) ? 1 : 0; \
    }

/* x -= (y + b0)
   b += borrow  (b & b0 can be greater than 1)
   Fastest when b0, y is simple addressed (NOT array, etc.)
   (d = x-y) > MPI_MAXUWORD-y is better than (d = x-y) > x
   because it allows &d == &x !
   (&x != &y !!!)
 */
#define UWORD_SBB_ADDB(b, x, y, b0) \
    if ((x -= (b0)) > MPI_MAXUWORD-(b0)) { \
        ++b; \
    } \
    if ((x -= (y)) > MPI_MAXUWORD-(y)) { \
        ++b; \
    }

/******** MUL ********
 ph MUST be simple variable !!! (NOT *a, a[1], a+b, ...)
 */
/* Macro to multiply 2 U_WORDs and generate a product
     of 2 U_WORDs long.
   (ph, pl) = x * y;
   pl, ph, x, y, rtmp1 ~ 2, tmp1 ~ 4 should all be type of U_WORD.
   It is recommended that rtmp1, rtmp2 be registers.
   x & y can be non-registers.
 */
#define UWORD_MUL(ph, pl, x, y) \
    __tmp1 = MPI_LOHUWORD (x), __tmp2 = MPI_HIHUWORD (x); \
    __tmp3 = MPI_LOHUWORD (y), __tmp4 = MPI_HIHUWORD (y); \
    pl = __tmp1*__tmp3, ph = __tmp2*__tmp4; \
    __rtmp1 = __tmp1*__tmp4, __rtmp2 = __tmp2*__tmp3; \
    if ((__rtmp1 += __rtmp2) < __rtmp2) { \
        ph += MPI_HIHUWORD1; \
    } \
    __rtmp2 = (__rtmp1 << MPI_HUWORDBITS); \
    if ((pl += __rtmp2) < __rtmp2) { \
        ++ph; \
    } \
    ph += MPI_HIHUWORD (__rtmp1)

/* Macro to square a U_WORD and generate a product
     of 2 U_WORDs long.
   (ph, pl) = x * x;
   pl, ph, x, rtmp1 ~ 2, tmp1 ~ 2 should all be type of U_WORD.
   It is recommended that rtmp1, rtmp2 be registers.
   x can be non-registers.
 */
#define UWORD_SQR(ph, pl, x) \
    __tmp1 = MPI_LOHUWORD (x), __tmp2 = MPI_HIHUWORD (x); \
    pl = __tmp1*__tmp1, ph = __tmp2*__tmp2; \
    __rtmp1 = __tmp1*__tmp2; \
    if ((__rtmp1 & MPI_UWORDMSB)) { \
        ph += MPI_HIHUWORD1; \
    } \
    __rtmp2 = (__rtmp1 <<= 1) << MPI_HUWORDBITS; \
    if ((pl += __rtmp2) < __rtmp2) { \
        ++ph; \
    } \
    ph += MPI_HIHUWORD (__rtmp1)


/******** DIV ********
 q MUST be simple variable !!! (NOT *a, a[1], a+b, ...)
 */
/* Divide only
 */
#define UWORD_DIVF(q, xh, xl, y) \
    __UWORD_DIVF (&(q), xh, xl, y)
#define UWORD_DIV(q, xh, xl, y) \
    __UWORD_DIV (&(q), xh, xl, y)
/* Divide with modular
 */
#define UWORD_DIVFMOD(r, q, xh, xl, y) \
    r = __UWORD_DIVF (&(q), xh, xl, y)
#define UWORD_DIVMOD(r, q, xh, xl, y) \
    r = __UWORD_DIV (&(q), xh, xl, y)


/* Return:  (xh, xl) % y
   Output:  *q = (xh, xl)/y
   xh MUST be less than y !!!
   Normally fast, but is sensitive to some singularitis which
     makes it VERY SLOW !
   The speed is the fastest when y is normalized.
 */
static U_WORD __UWORD_DIVF (q, xh, xl, y)
U_WORD                  *q, xh, xl, y;
{
    register U_WORD     u;
    register H_UWORD    qh, ql, yh, yl;

    yh = (H_UWORD)MPI_HIHUWORD (y);
    yl = (H_UWORD)MPI_LOHUWORD (y);

    /* High half quotient
     */
    if (yh == MPI_MAXHUWORD) {
        qh = (H_UWORD)MPI_HIHUWORD (xh);
    }
    else {
        qh = (H_UWORD)(xh/(yh+1));
    }
    u = (U_WORD)qh*yl;
    xh -= ((U_WORD)qh*yh);
    xh -= MPI_HIHUWORD (u);
    u <<= MPI_HUWORDBITS;
    if ((xl -= u) > (MPI_MAXUWORD-u)) {
        --xh;
    }
    u = yl;
    u <<= MPI_HUWORDBITS;
    while (xh > yh || (xh == yh && xl > u)) {
        if ((xl -= u) > (MPI_MAXUWORD-u)) {
            --xh;
        }
        xh -= yh;
        ++qh;
    }

    /* Low half quotient
     */
    if (yh == MPI_MAXHUWORD) {
        ql = (H_UWORD)xh;
    }
    else {
        ql = (H_UWORD)(((xh << MPI_HUWORDBITS) | (xl >> MPI_HUWORDBITS))
            /(yh+1));
    }
    u = (U_WORD)ql*yl;
    if ((xl -= u) > (MPI_MAXUWORD-u)) {
        --xh;
    }
    u = (U_WORD)ql*yh;
    xh -= MPI_HIHUWORD (u);
    u <<= MPI_HUWORDBITS;
    if ((xl -= u) > (MPI_MAXUWORD-u)) {
        --xh;
    }
    while (xh > 0 || xl >= y) {
        if ((xl -= y) > (MPI_MAXUWORD-y)) {
          -- xh;
        }
        ++ql;
    }
    *q = (((U_WORD)qh) << MPI_HUWORDBITS) | ql;
    return (xl);
}

/* Return:  (xh, xl) % y
   Output:  *r = (xh, xl)/y
   y MUST NOT be 0 !!!
   Normalize y first, then call UWORD_DIV to perform the division.
   Thus, it has a moderate speed.
 */
static U_WORD __UWORD_DIV (q, xh, xl, y)
U_WORD              xh, xl, y;
register U_WORD     *q;
{
    UWORD_MUL_DECLARE
    int             k;
    register U_WORD u, v;

    if (y == 0) {
        return (MPI_MAXUWORD);
    }
    for (k = 0; (y & MPI_UWORDMSB) == 0; y <<= 1, ++k);
    if (y <= xh) {
        return (MPI_MAXUWORD);
    }
    if (! k) {
        return (__UWORD_DIVF (q, xh, xl, y));
    }
    xh <<= k;
    xh |= xl >> (MPI_UWORDBITS-k);
    xl <<= k;
    if (y == MPI_MAXUWORD) {
        *q = xh;
    }
    else {
        __UWORD_DIVF (q, xh, xl, y+1);
    }
    UWORD_MUL (u, v, *q, y);
    xh -= u;
    if ((xl -= v) > (MPI_MAXUWORD-v)) {
        --xh;
    }
    while (xh != 0 || xl >= y) {
        ++*q;
        if ((xl -= y) > (MPI_MAXUWORD-y)) {
            --xh;
        }
    }
    return (xl >> k);
}

/******** SHIFT ********
 n MUST be simple variable !!! (NOT *a, a[1], a+b, ...)
 */
#define UWORD_SHL(r, xl, xr, n) \
    r = (((xl) << (n))|((xr) >> (MPI_UWORDBITS-(n))))

#define UWORD_SHR(r, xl, xr, n) \
    r = (((xl) << (MPI_UWORDBITS-(n)))|((xr) >> (n)))

/******** MPI MUL ********
 */
/* p = mpi * m (only len U_WORDs are stored, the last is stored in c)
   c stores the most significant U_WORD of the result !
   len MUST be simple variable !!! (NOT *a, a[1], a+b ...)
 */
#define MPI_UWORD_MUL(p, mpi, m, len, c) \
    __tmp_c = 0; \
    __tmp_i = 0; \
    do { \
        UWORD_MUL (c, __tmp_d, *((mpi)+__tmp_i), m); \
        UWORD_ADD3_ADDC (c, *((p)+__tmp_i), __tmp_c, __tmp_d); \
        __tmp_c = c; \
    } while (++__tmp_i < (len))

/* p += mpi * m (only len U_WORDs are added, the last adder is stored in c)
   c stores the carry in most significant U_WORD
   len MUST be simple variable !!! (NOT *a, a[1], a+b ...)
 */
#define MPI_UWORD_MUL_ADD(p, mpi, m, len, c) \
    __tmp_c = 0; \
    __tmp_i = 0; \
    do { \
        UWORD_MUL (c, __tmp_d, *((mpi)+__tmp_i), m); \
        UWORD_ADC_ADDC (c, *((p)+__tmp_i), __tmp_c, __tmp_d); \
        __tmp_c = c; \
    } while (++__tmp_i < (len))

/* p -= mpi * m (only len U_WORDs are subtracted, the last subtracter is stored in b)
   b stores the borrow in the most significant U_WORD
   len MUST be simple variable !!! (NOT *a, a[1], a+b ...)
 */
#define MPI_UWORD_MUL_SUB(p, mpi, m, len, b) \
    __tmp_i = 0; \
    b = 0; \
    do { \
        UWORD_MUL (__tmp_c, __tmp_d, *((mpi)+__tmp_i), m); \
        UWORD_SBB_ADDB (__tmp_c, *((p)+__tmp_i), __tmp_d, b); \
        b = __tmp_c; \
    } while (++__tmp_i < len)

/* Square Set (1st round)
   (p, m: U_WORD *;  tc: U_WORD;  len: int)
   tc stores the carry in the most significant U_WORD
 */
#define MPI_UWORD_SQR(p, m, len, tc) \
    UWORD_SQR (__tmp_t, __tmp_d, *(m)); \
    tc = 0; \
    *(p) = __tmp_d; \
    for (__tmp_i = 1; __tmp_i < len; ++__tmp_i) { \
        UWORD_MUL (__tmp_c, __tmp_d, *((m)+__tmp_i), *(m)); \
        tc += __tmp_c;  /* No carry here for __tmp_c <= MAXUWORD-1 && tc <= 1 */ \
        UWORD_ADC3_ADDC (__tmp_c, *(p+__tmp_i), __tmp_t, __tmp_d, __tmp_d); \
        UWORD_ADD3 (tc, __tmp_t, tc, __tmp_c); \
    } \
    *((p)+__tmp_i) = __tmp_t

/* Square Add (2nd ~ len round)
   (p, m: U_WORD *;  tc: U_WORD;  len: int)
   tc stores the carry in the most significant U_WORD
 */
#define MPI_UWORD_SQR_ADD(p, m, len, tc) \
    UWORD_SQR (__tmp_t, __tmp_d, *(m)); \
    tc = 0; \
    UWORD_ADD_ADDC (__tmp_t, *(p), __tmp_d);    /* No carry of t */ \
    for (__tmp_i = 1; __tmp_i < len; ++__tmp_i) { \
        UWORD_MUL (__tmp_c, __tmp_d, *((m)+__tmp_i), *(m)); \
        tc += __tmp_c;  /* No carry here for __tmp_c <= MAXUWORD-1 && tc <= 1 */ \
        UWORD_ADC3_ADDC (__tmp_c, __tmp_u, __tmp_t, __tmp_d, __tmp_d); \
        UWORD_ADD (__tmp_t, *((p)+__tmp_i), __tmp_u); \
        __tmp_u = 0; \
        UWORD_ADC_ADDC (__tmp_u, __tmp_t, __tmp_c, tc); \
        tc = __tmp_u; \
    } \
    UWORD_ADD_ADDC (tc, *((p)+__tmp_i), __tmp_t)

#else
/*******************************************************************
   YOU ARE ENCOURAGED TO ADD YOU OWN CODE HERE
 *******************************************************************/
#if defined (_WIN32) && _M_IX86 >= 300 && _M_IX86 <= 500
#include    "mpw32i86.c"
#endif  /* _WIN32 && _M_IX86 */

#ifdef  _SCO_UNIX_I386
#include    "mpscoi86.c"
#endif  /* _SCO_UNIX_I386 */

#endif  /* ! _MPI_NO_ANSI */


#ifdef __cplusplus
}
#endif

#endif
