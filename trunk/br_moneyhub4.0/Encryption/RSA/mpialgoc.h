/*******************************************************************
  mpialgoc.h

    Multi-Precision Integer (MPI) Number Theory Algorithms
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    ANY MODIFICATION MADE TO THIS SOURCE CODE IS AT YOUR OWN RISK.
 *******************************************************************/

#ifndef __SS_MPIALGOC_DEFINED__
#define __SS_MPIALGOC_DEFINED__ 1

#include    "ssglob.h"
#include    "mpidefs.h"

#ifdef __cplusplus
extern "C" {
#endif


#define MPI_MAXLEN      64      /* Max U_WORDs in MPI, MUST be EVEN !! */
#define MPI_GSPP_RSA    1       /* Generate strong prime for RSA */


/* a >= b >= 0
   gcd == a or b is permitted
 */
#define MPI_GCD(gcd, a, b) \
    MPI_RightShiftAlgorithm (a, b, gcd, (MPI)0, 0)
/* inv == a is permitted
 */
#define MPI_Inverse(inv, a, n) \
    MPI_ExtEuclidean (n, a, (MPI)0, inv)
/* inv == a is permitted
 */
#define MPI_MonInverse(inv, a, n) \
    MPI_RightShiftAlgorithm (n, a, (MPI)0, inv, (int)(*(n))*MPI_UWORDBITS)


typedef void (*GP_PROC) PROTO_LIST ((int));
typedef int (*RND_PROC) PROTO_LIST ((void));


/* (a, b, gcd, invB, exp2)
   a > b > 0, a is odd
   invB = (b^-1)*(2^exp2) mod a
   if gcd == (MPI)0 then the gcd value is discarded
   if invB == (MPI)0 then the inverse value is discarded
   returns TRUE if gcd (a, b) = 1
 */
extern BOOL MPI_RightShiftAlgorithm PROTO_LIST
    ((CONST_MPI, CONST_MPI, MPI, MPI, int));

/* (n, a, gcd, InvA)
 */
extern BOOL MPI_ExtEuclidean PROTO_LIST ((CONST_MPI, CONST_MPI, MPI, MPI));

/* (result, a, c, n)
   result MUST be at least 2*nLen U_WORDs long
   result may point to the same memory as a
   n MUST be an ODD MPI s.t. gcd (a, n) = 1 !!!
 */
extern void MPI_PowerModPrime PROTO_LIST
    ((MPI, CONST_MPI, CONST_MPI, CONST_MPI));

/* (result, a, c, n)
   result MUST be at least 2*nLen U_WORDs long
   result may point to the same memory as a
   n MUST be a ODD MPI !
 */
extern void MPI_PowerMod PROTO_LIST
    ((MPI, CONST_MPI, CONST_MPI, CONST_MPI));

/* (p, n)
   p MUST be a ODD number
 */
extern BOOL MPI_MillerTest PROTO_LIST ((CONST_MPI, int));

/* (p, nBitLen, pr)
   p should be randomized as seed for the probable-prime
   The input value of p is the start point for incremental prime search.
 */
extern void MPI_GenProbPrime PROTO_LIST ((MPI, int, GP_PROC));

/* (p, nBitLen, pr)
   p should be randomized as seed for the probable-prime
 */
extern BOOL MPI_GenStrongProbPrime PROTO_LIST ((MPI, int, int, GP_PROC));

/* (mpi, len, rnd)
   Generate a random MPI
 */
extern void MPI_GenRandomMPI PROTO_LIST ((MPI, int, RND_PROC));


#ifdef __cplusplus
}
#endif


#endif
