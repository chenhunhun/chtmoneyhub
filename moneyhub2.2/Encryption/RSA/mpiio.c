/*******************************************************************
  mpiio.c

    Multi-Precision Integer (MPI) Input/Output Procedures in ANSI C
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    ANY MODIFICATION MADE TO THIS SOURCE CODE IS AT YOUR OWN RISK.
    Please ask the author for the permission.
 *******************************************************************/

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    "ssglob.h"
#include    "mpialgo.h"
#include    "mpialgoc.h"
#include    "mpiio.h"


void MPI_fprintf (fp, n, radix)
CONST_MPI               n;
FILE                    *fp;
UINT                    radix;
{
    U_WORD              a[MPI_MAXLEN+1], r;
    char                c[MPI_MAXLEN*MPI_UWORDBITS/2];
    int                 k;

    MPI_Assign (a, n);
    k = -1;
    do {
        r = MPI_DivUWord (a, radix, a);
        c[++k] = (char)((r < 10) ? r+'0' : r+'A'-10);
    } while (MPI_Len (a));
    for (; k >= 0; --k) {
        fprintf (fp, "%c", c[k]);
    }
}

void MPI_fscanf (fp, n, radix)
FILE                    *fp;
MPI                     n;
UINT                    radix;
{
    U_WORD              a[MPI_MAXLEN], d;
    char                c[MPI_MAXLEN*MPI_UWORDBITS/2];
    int                 k, l;

    MPI_Assign0 (a);
    fscanf_s (fp, "%s", c);
    l = strlen ((char *)c);
    for (k = 0; k < l; ++k) {
        if (c[k] >= '0' && c[k] <= '9') {
            d = c[k]-'0';
        }
        else if (c[k] >= 'A' && c[k] <= 'F') {
            d = c[k]-'A'+10;
        }
        else if (c[k] >= 'a' && c[k] <= 'f') {
            d = c[k]-'a'+10;
        }
        else {
            break;
        }
        if (d >= radix) {
            break;
        }
        MPI_MulUWord (a, a, radix);
        MPI_AddUWord (a, d);
    }
    MPI_Assign (n, a);
}
