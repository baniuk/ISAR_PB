// algtest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

typedef float elem_type ;


/*
* The following code is public domain.
* Algorithm by Torben Mogensen, implementation by N. Devillard.
* This code in public domain.
*/
elem_type torben(elem_type **m, int r, int c)
{
	int less, greater, equal;
	int n = r*c;
	int row,col;
	elem_type min, max, guess, maxltguess, mingtguess;
	min = max = m[0][0] ;
	for (row=0 ; row<r ; row++)
		for (col=0 ; col<c ; col++)
		{
			if (m[row][col]<min) min=m[row][col];
			if (m[row][col]>max) max=m[row][col];
		}
		while (1) {
			guess = (min+max)/2;
			less = 0; greater = 0; equal = 0;
			maxltguess = min ;
			mingtguess = max ;
			for (row=0; row<r; row++)
				for (col=0; col<c; col++)
				{
					if (m[row][col]<guess) {
						less++;
						if (m[row][col]>maxltguess) maxltguess = m[row][col] ;
					} else if (m[row][col]>guess) {
						greater++;
						if (m[row][col]<mingtguess) mingtguess = m[row][col] ;
					} else equal++;
				}
				if (less <= (n+1)/2 && greater <= (n+1)/2) break ;
				else if (less>greater) max = maxltguess ;
				else min = mingtguess;
		}
		if (less >= (n+1)/2) return maxltguess;
		else if (less+equal >= (n+1)/2) return guess;
		else return mingtguess;
}

int _tmain(int argc, _TCHAR* argv[])
{
	elem_type tab[3][3] = {1,2,3,4,5,6,7,8,9};
	elem_type med;

	med = torben(&(tab),3,3);

	return 0;
}


