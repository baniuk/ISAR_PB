#include "PCA.h"


void PCA(double* a,integer Na,integer LDA,double *D)
{
//	double a	//z matlaba w kolumnach IMPUT/OUTPUT
													
/* dsytrd_ */
	char uplo = 'U';		// Upper triangle of A is stored
//	integer Na = 3;			// The order of the matrix A.  N >= 0.
//	integer LDA = 3;		// The leading dimension of the array A.  LDA >= max(1,N).
//	double *D = new double[Na];		//  dimension (N) The diagonal elements of the tridiagonal matrix T: D(i) = A(i,i). 
	double *E = new double[Na-1];	// The off-diagonal elements of the tridiagonal matrix T: E(i) = A(i,i+1) if UPLO = 'U', E(i) = A(i+1,i) if UPLO = 'L'.
	double *TAU = new double[Na-1];	// The scalar factors of the elementary reflectors
	double *WORK_dsytrd;
	double worktmp;
	integer LWORK = -1;
	integer INFO;
/* dorgtr_*/
//	uplo
//  N - Na?
//  A = a
//  LDA = LDA
//  TAU = TAU
// WORK = WORK_dsytrd
/* dsteqr_ */
	char compz = 'V';		// Compute eigenvalues and eigenvectors of the tridiagonal matrix.  Z is initialized to the identity matrix. 
//	N = Na
//	D = D
//	E = E
	integer LDZ = Na;
	double *Z = a;
	double *WORK_dsteqr = new double[2*Na-2];
//	INFO = INFO

	dsytrd_(&uplo, &Na, a, &LDA, D, E, TAU, &worktmp, &LWORK, &INFO);
	
	LWORK = (integer)worktmp;
	WORK_dsytrd = new double[LWORK];
	dsytrd_(&uplo, &Na, a, &LDA, D, E, TAU, WORK_dsytrd, &LWORK, &INFO);
	dorgtr_(&uplo, &Na, a, &LDA, TAU, WORK_dsytrd, &LWORK, &INFO);

	dsteqr_(&compz, &Na, D, E, Z, &LDZ, WORK_dsteqr, &INFO);

/*	for(i=0;i<Na;i++)
		printf("\t%f ", D[i]);
	printf("\n");
	for(i=0;i<Na*LDZ;i++)
		printf("\t%f ", Z[i]);

	if( fopen_s( &stream, "out.txt", "w" ) == 0 )
	{
		fprintf_s(stream,"[");
		for(i=0;i<Na*LDZ;i++)
			fprintf_s(stream,"%+030.20f,",Z[i]);
		fpos_t position;
		fgetpos(stream,&position);
		position-=2;
		fsetpos(stream,&position);
		fprintf_s(stream,"];");
		fclose( stream );
	}*/

//	delete[] D;
	SAFE_DELETE(E);
	SAFE_DELETE(TAU);
	SAFE_DELETE(WORK_dsytrd);
	SAFE_DELETE(WORK_dsteqr);
//	delete[] Z;


}