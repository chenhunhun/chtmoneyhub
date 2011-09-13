/*******************************************************************
  mpidefs.h

    Type definitions of MPI Unit Word.
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    You are encouraged to modify the code for the advanced platform
    to enhance performance.
    PLEASE MODIFY THE CODE WHICH YOU ARE TOLD TO.
 *******************************************************************/

#ifndef __SS_MPIDEFS_DEFINED__
#define __SS_MPIDEFS_DEFINED__ 1

#ifdef __cplusplus
extern "C" {
#endif

#include    "ssglob.h"

#ifndef _MPI_NON32BIT
/*******************************************************************
    ANY MODIFICATION MADE TO THIS SECTION IS AT YOUR OWN RISK.
 *******************************************************************/

typedef UINT4           U_WORD;             /* Type of Unit Word */
typedef UINT2           H_UWORD;            /* Type of Half Unit Word */

#define MPI_MAXUWORD    ((U_WORD)0xffffffff)/* Max Unit Word value */
#define MPI_MAXHUWORD   ((H_UWORD)0xffff)   /* Max Half Unit Word value */
#define MPI_UWORDMSB    ((U_WORD)0x80000000)/* MSB of Unit Word */
#define MPI_HUWORDMSB   ((H_UWORD)0x8000)   /* MSB of Half Unit Word */
#define MPI_UWORDBITS   32                  /* U_WORD bit-length */
#define MPI_UWORDBYTES  (MPI_UWORDBITS/8)
#define MPI_HUWORDBITS  16                  /* H_UWORD bit-length */
#define MPI_HUWORDBYTES (MPI_HUWORDBITS/8)
#define MPI_HUWORDMASK  ((U_WORD)0xffff)    /* Bit-mask of H_UWORD */
#define MPI_HIHUWORD1   ((U_WORD)0x10000)   /* 1 in the high half U_WORD */
#define MPI_POWERMODMSK ((U_WORD)0xc0000000)/* Mask value used by PowerMod */

#else
/*******************************************************************
   YOU ARE ENCOURAGED TO MODIFY THE CODE HERE
 *******************************************************************/
#ifdef  _MPI_VC64BIT

typedef unsigned __int64    U_WORD;
typedef UINT4               H_UWORD;

#define MPI_MAXUWORD    ((U_WORD)0xffffffffffffffff)
#define MPI_MAXHUWORD   ((H_UWORD)0xffffffff)
#define MPI_UWORDMSB    ((U_WORD)0x8000000000000000)
#define MPI_HUWORDMSB   ((H_UWORD)0x80000000)
#define MPI_UWORDBITS   64
#define MPI_UWORDBYTES  (MPI_UWORDBITS/8)
#define MPI_HUWORDBITS  32
#define MPI_HUWORDBYTES (MPI_HUWORDBITS/8)
#define MPI_HUWORDMASK  ((U_WORD)0xffffffff)
#define MPI_HIHUWORD1   ((U_WORD)0x100000000)
#define MPI_POWERMODMSK ((U_WORD)0xc000000000000000)

#endif

#endif

/* Type define for MPI
 */
typedef U_WORD *            MPI;
typedef const U_WORD *      CONST_MPI;

/* Operations of Unit Word
 */
#define MPI_LOHUWORD(x)     ((x) & MPI_HUWORDMASK)
#define MPI_HIHUWORD(x)     ((x) >> MPI_HUWORDBITS)


#ifdef __cplusplus
}
#endif

#endif
