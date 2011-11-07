/*******************************************************************
  mpialgo.h

    Multi-Precision Integer (MPI) Basic Arithmetics
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    ANY MODIFICATION MADE TO THIS SOURCE CODE IS AT YOUR OWN RISK.
 *******************************************************************/

/*******************************************************************
    Note that:
 -------------------------------------------------------------------
    # The len parameter in MPI functions is the length of operands
      counted in U_WORD.
    # The Product parameter in MPI_Mul function must be at least
      2*len U_WORDs long.
    # The Quotient parameter in MPI_Div function must be at least
      2*len U_WORDs long.
    # The results of most Montgomery functions are 2*len U_WORDs
      long.
 *******************************************************************/

#ifndef __SS_MPIALGO_DEFINED__
#define __SS_MPIALGO_DEFINED__ 1

#include    "ssglob.h"
#include    "mpidefs.h"

#ifdef __cplusplus
extern "C" {
#endif


#define MPI_Assign0(n)  *(MPI)(n) = 0
#define MPI_Assign(dst, src) \
    T_memcpy (dst, src, MPI_UWORDBYTES*((int)*(MPI)(src)+1))
#define MPI_AssignUWord(n, val) \
    if (val) { \
        *(MPI)(n) = 1, *((MPI)(n)+1) = (val); \
    } \
    else { \
        *(MPI)(n) = 0; \
    }
#define MPI_Len(n)      ((int)(*(n)))

#define MPI_MemCpy(dst, src, len) \
    T_memcpy (dst, src, MPI_UWORDBYTES*(len))
#define MPI_MemCpyZX(dst, src, nMvLen, len) \
    T_memcpy (dst, src, MPI_UWORDBYTES*(nMvLen)); \
    if ((len) > (nMvLen)) { \
        T_memset ((MPI)(dst)+(nMvLen), 0, MPI_UWORDBYTES*((len)-(nMvLen))); \
    }
#define MPI_MemSet0(dst, len) \
    T_memset (dst, 0, MPI_UWORDBYTES*(len))

/* (sum, a1, a2)
   sum == a1 or a2 is permitted
 */
extern U_WORD MPI_Add3 PROTO_LIST ((MPI, CONST_MPI, CONST_MPI));
/* (a1, a2)
   a1 == a2 is permitted, which provides a alternative way for double operation.
 */
extern U_WORD MPI_Add PROTO_LIST ((MPI, CONST_MPI));
/* (sum, a1, a2)
   sum == a1 is permitted, but a bit slower
 */
extern U_WORD MPI_AddUWord3 PROTO_LIST ((MPI, CONST_MPI, U_WORD));
/* (a1, a2)
 */
extern U_WORD MPI_AddUWord PROTO_LIST ((MPI, U_WORD));


/* (diff, s1, s2)
   diff == s1 or s2 is permitted
 */
extern U_WORD MPI_Sub3 PROTO_LIST ((MPI, CONST_MPI, CONST_MPI));
/* (s1, s2)
 */
extern U_WORD MPI_Sub PROTO_LIST ((MPI, CONST_MPI));
/* (diff, s1, s2)
   diff == s1 is permitted, but a bit slower
 */
extern U_WORD MPI_SubUWord3 PROTO_LIST ((MPI, CONST_MPI, U_WORD));
/* (s1, s2)
 */
extern U_WORD MPI_SubUWord PROTO_LIST ((MPI, U_WORD));


/* (Product, m1, m2)
   Space of Product MUST NOT be less than the total length of m1 & m2.
 */
extern void MPI_Mul PROTO_LIST ((MPI, CONST_MPI, CONST_MPI));
/* (Product, m1, m2)
   Product may be the same length as or 1 U_WORD longer than m1, the space
    of it MUST NOT be less than 1+length of m1.
   Product may point to the same memory as m1
 */
extern void MPI_MulUWord PROTO_LIST ((MPI, CONST_MPI, U_WORD));
/* (Product, m)
   Space of Product MUST NOT be less than twice the length of m.
 */
extern void MPI_Sqr PROTO_LIST ((MPI, CONST_MPI));


/* (Dividend, Divider, Quotient)
   Return: 0 - Normal, 1 - Divide by zero error
 */
extern int MPI_Div PROTO_LIST ((MPI, CONST_MPI, MPI));
/* (Dividend, Divider, Quotient)
   Return: Remainder of the Division
   Quotient MUST have the same length as Dividend
   Dividend may point to the same memory as Quotient
 */
extern U_WORD MPI_DivUWord PROTO_LIST ((CONST_MPI, U_WORD, MPI));

/* (Dividend, Divider)
   Return: 0 - Normal, 1 - Divide by zero error
 */
extern int MPI_Mod PROTO_LIST ((MPI, CONST_MPI));
/* (Dividend, Divider)
   Return: Remainder
 */
extern U_WORD MPI_ModUWord PROTO_LIST ((CONST_MPI, U_WORD));


/* (n, l): n *= 2^l
 */
extern void MPI_Shl PROTO_LIST ((MPI, int));
/* (n, l): n /= 2^l
 */
extern U_WORD MPI_Shr PROTO_LIST ((MPI, int));


/* (mpi, byteStr, len)
   len is the length of byteStr in unit of BYTE.
   return:  length of MPI in unit of U_WORD.
   (mpi == byteStr is permitted)
 */
extern int MPI_ByteStringToMPI PROTO_LIST ((MPI, const BYTE *, int));
/* (byteStr, mpi, len)
   len is the length of byteStr in unit of BYTE.
   return:  length of MPI in unit of U_WORD.
   (mpi == byteStr is permitted)
 */
extern int MPI_MPIToByteString PROTO_LIST ((BYTE *, CONST_MPI, int));


/* (n1, n2)
 */
extern int MPI_Cmp PROTO_LIST ((CONST_MPI, CONST_MPI));
/* (n1, n2)
 */
extern int MPI_CmpUWord PROTO_LIST ((CONST_MPI,  U_WORD));
/* (n)
 */
extern int MPI_BitLen PROTO_LIST ((CONST_MPI));
/* (n)
 */
extern int MPI_ByteLen PROTO_LIST ((CONST_MPI));


/* (n)
 */
extern U_WORD MPI_MonN0 PROTO_LIST ((CONST_MPI));
/* (prod, a, b, n, n0)
   Length of a*b MUST NOT exceed twice that of n
   Space of prod MUST NOT be less than twice the length of n
 */
extern void MPI_MonMul PROTO_LIST
    ((MPI, CONST_MPI, CONST_MPI, CONST_MPI, U_WORD));
/* (prod, a, n, n0)
   Length of a*a MUST NOT exceed twice that of n
   Space of prod MUST NOT be less than twice the length of n
 */
extern void MPI_MonSqr PROTO_LIST
    ((MPI, CONST_MPI, CONST_MPI, U_WORD));
/* (na, a, n)
   Space of na MUST NOT be less than twice the length of n
   na may point to the same memory as a
 */
extern void MPI_NResidue PROTO_LIST ((MPI, CONST_MPI, CONST_MPI));
/* (p, a, n, n0)
   Space of p MUST NOT be less than twice the length of n
   p <- orginal MPI of n-residue a
   (p == a is permissible)
 */
extern void MPI_MonConvert PROTO_LIST ((MPI, CONST_MPI, CONST_MPI, U_WORD));

/* (n, k): n = 2^k
 */
extern void MPI_PowerOf2 PROTO_LIST ((MPI, int));


#ifdef __cplusplus
}
#endif

#endif
