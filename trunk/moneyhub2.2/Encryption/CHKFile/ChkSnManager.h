#pragma once
#include "../RSA/mpidefs.h"
#include "../RSA/mpialgoc.h"
#include <string>
using namespace std;

struct SnStruct
{
	U_WORD	n[MPI_MAXLEN+1];
	U_WORD	e[MPI_MAXLEN+1];
	U_WORD	d[MPI_MAXLEN+1];
	U_WORD  p[MPI_MAXLEN+1];
	U_WORD  q[MPI_MAXLEN+1];
	U_WORD  kdp[MPI_MAXLEN+1];
	U_WORD  kdq[MPI_MAXLEN+1];
	U_WORD  A[MPI_MAXLEN+1];
};
class CChkSnManager
{
public:
	CChkSnManager();
public:
	SnStruct snstruct;
	static U_WORD Sn[8 * 32];
public:
	void GetKey();// ≥ı ºªØsnstruct

	bool bKInit; 
	bool bSInit; 
};
extern CChkSnManager _ChkSnManager;