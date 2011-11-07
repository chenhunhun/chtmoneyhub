/*
 * rsa_genkey.c
 *
 * This program uses the fucntion "RSA_GenerateKeys" in file rsa.c to
 * generate a RSA key pair.
 *
 * Shijianjun, 1998.09.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "ssglob.h"
#include "mpidefs.h"
#include "rsa.h"
#include "pkcs_rsa.h"
//#define  __MAIN_TEST__
#ifdef __MAIN_TEST__

#ifndef O_BINARY
#define O_BINARY 0
#endif
#define SNSIZE 8*32
char usage[] = "Syntax: rsa_genkey <modulus_bitlen> <output_file>\n";

char border[] = "<BORDER>";

static U_WORD   n[MPI_MAXLEN+1], e[MPI_MAXLEN+1], d[MPI_MAXLEN+1];
static U_WORD   p[MPI_MAXLEN+1], q[MPI_MAXLEN+1], kdp[MPI_MAXLEN+1];
static U_WORD   kdq[MPI_MAXLEN+1], A[MPI_MAXLEN+1];

int bitLen = 1024; /* default value */
U_WORD Sn[SNSIZE] = {0};//生成大小为Sn的文件//此时为固定大小，请将数值设为1024
int pr()
{
	printf( "." );
	fflush( stdout );

	return 0;
}

void main( int argc, char *argv[] )
{
	char filename[260] = {'s','n',0};
	char *snfname;
	int	rc;
	int	fd;
	FILE *ffd;
	int	lenBorder;
	int i;
	char *psn = (char*)Sn;
	char temp;

	if( argc != 3 ) {
		printf( "%s", usage );
		print_error( NULL );
		}

	bitLen = atoi( argv[1] );

	printf( "Modulus bit length = %d\n", bitLen );
	printf( "Output filename = %s\n", argv[2] );

	srand( time( NULL ) );

	if( ( rc = RSA_GenerateKeys( n, e, d, p, q, kdp, kdq, A, bitLen, 
		RSA_GK_STRONGKEY|RSA_GK_LARGEEXP, rand, (GP_PROC)pr )) < 0 ) {
		CAPI_perror( rc );
		print_error( "RSA_GenerateKeys failed\n" );
		}
	else
		printf( " OK\n" );

	printf( "Checking RSA key ..." );
	if( ( rc = RSA_CheckKeys( n, e, d, p, q, kdp, kdq, A )) < 0 ) {
		CAPI_perror( rc );
		print_error( "RSA_CheckKeys failed\n" );
		}
	else
		printf( " OK\n" );

	/* save key into file */
	
	if( ( fd = open( argv[2], O_CREAT|O_WRONLY|O_BINARY|O_TRUNC,0660))< 0 )
		print_error( "Cannot open file" );

	lenBorder = strlen( border );

	if(	write( fd, & bitLen, sizeof(bitLen) ) != sizeof(bitLen) ||
		write( fd, n, sizeof(n) ) != sizeof(n) ||
		write( fd, border, lenBorder ) != lenBorder ||
		write( fd, e, sizeof(e) ) != sizeof(e) ||
		write( fd, border, lenBorder ) != lenBorder ||
		write( fd, d, sizeof(d) ) != sizeof(d) ||
		write( fd, border, lenBorder ) != lenBorder ||
		write( fd, p, sizeof(p) ) != sizeof(p) ||
		write( fd, border, lenBorder ) != lenBorder ||
		write( fd, q, sizeof(q) ) != sizeof(q) ||
		write( fd, border, lenBorder ) != lenBorder ||
		write( fd, kdp, sizeof(kdp) ) != sizeof(kdp) ||
		write( fd, border, lenBorder ) != lenBorder ||
		write( fd, kdq, sizeof(kdq) ) != sizeof(kdq) ||
		write( fd, border, lenBorder ) != lenBorder ||
		write( fd, A, sizeof(A) ) != sizeof(A) )
		print_error( "write failed\n" );

	close( fd );

	srand( time( NULL ) );

	i = 0;

	while(i <  sizeof(Sn)/sizeof(char))
	{
		temp = (unsigned char)rand() % 0xFF;

		memcpy(psn + i,&temp,sizeof(char)); // 产生随机数
		i ++;
	}

	Sn[0] = n[31];
	Sn[3] = n[27];
	Sn[5] = n[28];
	Sn[14] = n[18];
	Sn[30] = n[12];
	Sn[33] = n[5];
	Sn[48] = n[26];
	Sn[64] = n[4];
	Sn[82] = n[13];
	Sn[109] = n[24];
	Sn[112] = n[2];
	Sn[134] = n[25];
	Sn[136] = n[32];
	Sn[138] = n[1];
	Sn[140] = n[9];
	Sn[154] = n[16];
	Sn[155] = n[15];
	Sn[161] = n[22];
	Sn[176] = n[19];
	Sn[179] = n[7];
	Sn[185] = n[30];
	Sn[189] = n[23];
	Sn[191] = n[3];
	Sn[192] = n[29];
	Sn[212] = n[6];
	Sn[224] = n[10];
	Sn[225] = n[20];
	Sn[230] = n[21];
	Sn[243] = n[8];
	Sn[245] = n[17];
	Sn[249] = n[11];
	Sn[252] = n[14];

	ffd = 0;

	snfname = strcat(filename, argv[2]);
	ffd = fopen( snfname, "w+" );


	for(i = 0; i  < SNSIZE;i ++)
	{
		fprintf(ffd,"0x%08x,",Sn[i]);
	}
	fclose(ffd);

	printf( "Key saved in %s OK\n", argv[2] );
	exit( 0 );
}

print_error( char * info ) 
{
	if( info )
		printf( "%s\n", info );
	exit( 0 );
}

int print_mpi( MPI mpi )
{
	U_WORD	i;

	for( i=1; i<=mpi[0]; i++ )
		printf( " %8x", mpi[i] );
	
	printf( "\n" );
	return 0;
}

#endif