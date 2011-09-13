/*******************************************************************
  mpiio.h

    Multi-Precision Integer (MPI) Input/Output Procedures
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    ANY MODIFICATION MADE TO THIS SOURCE CODE IS AT YOUR OWN RISK.
 *******************************************************************/

#ifndef __SS_IOLONG_DEFINED__
#define __SS_IOLONG_DEFINED__ 1

#include    <stdio.h>
#include    "ssglob.h"
#include    "mpidefs.h"

#ifdef __cplusplus
extern  "C" {
#endif


#define MPI_printf(n, radix)    MPI_fprintf (stdout, (n), (radix))
#define MPI_scanf(n, radix)     MPI_fscanf (stdin, (n), (radix))
#define MPI_fprintfln(fp, nn, radix) \
    MPI_fprintf ((fp), (nn), (radix)); \
    fprintf ((fp), "\n")
#define MPI_printfln(nn, radix) \
    MPI_fprintf (stdout, (nn), (radix)); \
    printf ("\n")


/* (fp, n, radix)
 */
extern void MPI_fprintf PROTO_LIST ((FILE *, CONST_MPI, UINT));

/* (fp, n, radix)
 */
extern void MPI_fscanf PROTO_LIST ((FILE *, MPI, UINT));


#ifdef __cplusplus
}
#endif

#endif
