/*
 * rsa_encrypt.c
 *
 * This program uses the fucntion "RSA_Encrypt/Decrypt" in file rsa.c to
 * to test.
 *
 * Shijianjun, 1998.09.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "ssglob.h"
#include "mpidefs.h"
#include "mpialgoc.h"
#include "mpialgo.h"
#include "rsa.h"

#ifdef __MAIN_TEST__

#ifndef O_BINARY
#define O_BINARY 0
#endif

char usage[] = "Syntax: rsa_encrypt keyfile repeat_times\n";

char border[] = "<BORDER>";

static U_WORD   n[MPI_MAXLEN+1], e[MPI_MAXLEN+1], d[MPI_MAXLEN+1];
static U_WORD   p[MPI_MAXLEN+1], q[MPI_MAXLEN+1], kdp[MPI_MAXLEN+1];
static U_WORD   kdq[MPI_MAXLEN+1], A[MPI_MAXLEN+1];

int bitLen = 1024; /* default value */


U_WORD plain[MPI_MAXLEN+1];
U_WORD cipher[MPI_MAXLEN+1];
U_WORD decipher[MPI_MAXLEN+1];

main( int argc, char *argv[] )
{
	int	rc;
	int	fd;
	int	lenBorder;
	int	rep_times;
	int	rdnum;
	int	i;

	if( argc != 3 ) {
		printf( "%s", usage );
		print_error( NULL );
		}

	rep_times = atoi( argv[2] );

	if( ( fd = open( argv[1], O_RDONLY|O_BINARY )) < 0 )
		print_error( "Cannot open key file \n" );

	printf( "Read key ..." );
	lenBorder = strlen( border );
	if(	read( fd, & bitLen, sizeof(bitLen) ) != sizeof(bitLen) ||
		read( fd, n, sizeof(n) ) != sizeof(n) ||
		read( fd, border, lenBorder ) != lenBorder ||
		read( fd, e, sizeof(e) ) != sizeof(e) ||
		read( fd, border, lenBorder ) != lenBorder ||
		read( fd, d, sizeof(d) ) != sizeof(d) ||
		read( fd, border, lenBorder ) != lenBorder ||
		read( fd, p, sizeof(p) ) != sizeof(p) ||
		read( fd, border, lenBorder ) != lenBorder ||
		read( fd, q, sizeof(q) ) != sizeof(q) ||
		read( fd, border, lenBorder ) != lenBorder ||
		read( fd, kdp, sizeof(kdp) ) != sizeof(kdp) ||
		read( fd, border, lenBorder ) != lenBorder ||
		read( fd, kdq, sizeof(kdq) ) != sizeof(kdq) ||
		read( fd, border, lenBorder ) != lenBorder ||
		read( fd, A, sizeof(A) ) != sizeof(A) )
		print_error( "read failed\n" );
	printf( " OK\n" );
	close( fd );

	printf( "Modulus bit length = %d\n", bitLen );
	printf( "Now begin testing ( %d times )\n", rep_times );

	for( i=0; i<rep_times; i++ ) {

		printf( "TEST %d\n", i+1 );

		printf( "Getting random MPI ... " );
		srand( time(NULL) );
		rdnum = rand();
		MPI_GenRandomMPI( plain, rdnum%(bitLen/MPI_UWORDBITS), rand );
		print_mpi( plain );
		printf( "OK\n" );

		printf( "Encrypting ... " );
		RSA_Encrypt( plain, n, e, cipher );
		print_mpi( cipher );
		printf( "OK\n" );

		printf( "Decrypting ... " );
		RSA_Decrypt( cipher, p, q, kdp, kdq, A, decipher);
		print_mpi( decipher );
		printf( "OK\n" );

		printf( "Comparing plain text and decipher text ... " );
		if( MPI_Cmp( plain, decipher ) == 0 ) 
			printf( "OK\n" );
		else
			print_error( "Data no match\n" );
		}	
}

print_error( char * info ) 
{
	if( info )
		printf( "%s\n", info );
	exit( 0 );
}

print_mpi( MPI mpi )
{
	U_WORD	i;

	for( i=1; i<=mpi[0]; i++ )
		printf( " (%u)", (unsigned long)(mpi[i]) );
	
	printf( "\n" );
	return 0;
}

#endif 