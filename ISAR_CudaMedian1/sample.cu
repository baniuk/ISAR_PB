/********************************************************************
*  sample.cu
*  This is a example of the CUDA program.
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <cutil_inline.h>
#include "definitions.h"

#define float_lt(a,b) ((*a)<(*b))

__device__ void int_qsort(float *arr, unsigned n);
__device__ void torben(float m[], int n, float* med);

/// filtruje œrodek Obrazka
__global__ static void CUDAMedianMain(Matrix din, Matrix dout, Mask Okno, unsigned int startw, unsigned int starth)
{
	unsigned int r,c,rb,cb;
	float* rowin;
	float* rowout;

	int less, greater, equal;
	int n = Okno.e;
	int row,col;
	float min, max, guess, maxltguess, mingtguess, ret;

	r = blockIdx.y*blockDim.y + threadIdx.y + Okno.py + starth;	// srodek okna
	c = blockIdx.x*blockDim.x + threadIdx.x + Okno.px + startw;
	rb = r - Okno.py;	// lewy górny róg okna
	cb = c - Okno.px;

	if(r>=din.height-Okno.py || c>=din.width-Okno.px)
		return;

	rowin = (float*)((char*)din.data + rb*din.pitch);
	min = max = rowin[0];
	for (row=rb ; row<rb+Okno.wy ; row++)
		for (col=cb ; col<cb+Okno.wx ; col++)
		{
			rowin = (float*)((char*)din.data + row*din.pitch);
			if (rowin[col]<min) min=rowin[col];
			if (rowin[col]>max) max=rowin[col];
		}
		while (1) {
			guess = (min+max)/2;
			less = 0; greater = 0; equal = 0;
			maxltguess = min ;
			mingtguess = max ;
			for (row=rb; row<rb+Okno.wy; row++)
				for (col=cb; col<cb+Okno.wx; col++)
				{
					rowin = (float*)((char*)din.data + row*din.pitch);
					if (rowin[col]<guess) {
						less++;
						if (rowin[col]>maxltguess) maxltguess = rowin[col] ;
					} else if (rowin[col]>guess) {
						greater++;
						if (rowin[col]<mingtguess) mingtguess = rowin[col] ;
					} else equal++;
				}
				if (less <= (n+1)/2 && greater <= (n+1)/2) break ;
				else if (less>greater) max = maxltguess ;
				else min = mingtguess;
		}
		if (less >= (n+1)/2) ret = maxltguess;
		else if (less+equal >= (n+1)/2) ret = guess;
		else ret =  mingtguess;

		rowout = (float*)((char*)dout.data + r*dout.pitch);
		rowout[c] = ret;
}

///
/// dxx, dyy - po³owa z wielkoœci nieparzystego okna - po to aby zacz¹æ obróbkê w pierwszm przebiegu od tych fragmentów obrazu które nie s¹ brzegami (mieszcz¹ ca³e okno)
/// startw, starth - indexy od których powinien zacz¹æ siê wykonywaæ grid (bêd¹ dodawane do tych obliczonych)
void RunCUDA(Matrix din, Matrix dout, dim3 tpb, dim3 nob, Mask Okno, unsigned int startw, unsigned int starth)
{
//	CUDAMedianMain<<<nob, tpb, Okno.e*sizeof(float)>>>(din, dout, Okno, startw, starth);
	CUDAMedianMain<<<nob, tpb>>>(din, dout, Okno, startw, starth);
}



/*
__device__ void torben(float m[], int n, float* med)
{
	int i, less, greater, equal;
	float min, max, guess, maxltguess, mingtguess;
	min = max = m[0] ;
	for (i=1 ; i<n ; i++) {
		if (m[i]<min) min=m[i];
		if (m[i]>max) max=m[i];
	}
	while (1) {
		guess = (min+max)/2;
		less = 0; greater = 0; equal = 0;
		maxltguess = min ;
		mingtguess = max ;
		for (i=0; i<n; i++) {
			if (m[i]<guess) {
				less++;
				if (m[i]>maxltguess) maxltguess = m[i] ;
			} else if (m[i]>guess) {
				greater++;
				if (m[i]<mingtguess) mingtguess = m[i] ;
			} else equal++;
		}
		if (less <= (n+1)/2 && greater <= (n+1)/2) break ;
		else if (less>greater) max = maxltguess ;
		else min = mingtguess;
	}
	if (less >= (n+1)/2) *(med)= maxltguess;
	else if (less+equal >= (n+1)/2) *(med) = guess;
	else *(med) = mingtguess;
}*/
