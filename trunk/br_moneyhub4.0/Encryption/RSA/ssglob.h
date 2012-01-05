/********************************************************************
 *                                                                  *
 *          Symbols, Types, Macros, defined by Zhuang Hao           *
 *             (S.S.* is the symbol of Mr. Zhuang Hao)              *
 *                  Written by Zhuang Hao  S.S.*                    *
 *          Re-written for Readable & Portable C program code       *
 *                                                                  *
 ********************************************************************/

#ifndef __SS_SSGLOB_DEFINED__
#define __SS_SSGLOB_DEFINED__ 1

#include    <memory.h>                      /* Modify */
#include    <string.h>                      /* Modify */
#include    <time.h>                        /* Modify */


/* Define my const  -------------------------------------------------------*/

#ifndef BELL
#define BELL            (7)
#endif

#ifndef BKSPC
#define BKSPC           (8)
#endif

#ifndef TAB
#define TAB             (9)
#endif

#ifndef LF
#define LF              (10)
#endif

#ifndef CR
#define CR              (13)
#endif

#ifndef ESC
#define ESC             (27)
#endif

#ifndef TRUE
#define TRUE            ((BOOL)1)
#endif

#ifndef FALSE
#define FALSE           ((BOOL)0)
#endif

#ifndef NULL_PTR
#define NULL_PTR        ((POINTER)0)
#endif


/* Define my type  --------------------------------------------------------*/

#ifndef _INC_WINDOWS

typedef unsigned char       BYTE;           /* 1 Octet */
typedef unsigned short int  WORD;           /* 2 Octets */
typedef unsigned long int   DWORD;          /* 4 Octets */
typedef int                 BOOL;           /* Variable size */
typedef unsigned int        UINT;           /* Variable size */

#endif

typedef int                 INT;            /* Variable size */
typedef long int            LONG;           /* Variable size */
typedef unsigned long int   ULONG;          /* Variable size */
typedef short int           INT2;           /* 2 Octets */
typedef long int            INT4;           /* 4 Octets */
typedef unsigned short int  UINT2;          /* 2 Octets */
typedef unsigned long int   UINT4;          /* 4 Octets */

#ifndef _SS_STRICT
#ifdef _OLD_COMPILER
typedef unsigned char *     POINTER;        /* Memory model & Size independent */
#else
typedef void *              POINTER;        /* Memory model & Size independent */
#endif
#else
typedef struct __tagSTRQWERTY__ { int i; } __STRANGEQWERTY__;
typedef __STRANGEQWERTY__ * POINTER;        /* Memory model & Size independent */
#endif


/* Define my macro  -------------------------------------------------------*/

#ifdef _OLD_COMPILER
#define PROTO_LIST(list)    ()
#else
#define PROTO_LIST(list)    list
#endif
#define UNUSED_ARG(x)       x = *(&x)       /* Modify */


#define T_memset    memset                  /* Modify */
#define T_memcpy    memcpy                  /* Modify */
#define T_memmove   memmove                 /* Modify */
#define T_memcmp    memcmp                  /* Modify */
#define T_malloc    malloc                  /* Modify */
#define T_realloc   realloc                 /* Modify */
#define T_free      free                    /* Modify */
#define T_strcat    strcat                  /* Modify */
#define T_strncat   strncat                 /* Modify */
#define T_strcpy    strcpy                  /* Modify */
#define T_strncpy   strncpy                 /* Modify */
#define T_strcmp    strcmp                  /* Modify */
#define T_strncmp   strncmp                 /* Modify */
#define T_strlen    strlen                  /* Modify */
#define T_time      time                    /* Modify */


/* End of define  ---------------------------------------------------------*/

#endif
