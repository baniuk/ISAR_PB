/*****************************************************************************************
Program: Implementacja progowania globalnego iteracyjnego
Autor: Lech Napiera�a
Parametry wej�ciowe:
	owe - obraz w odcieniach szaro�ci do sprogowania
Parametry wyj�ciowe:
	owy - obraz dwukolorowy po progowaniu
Wersja: 1.0
*****************************************************************************************/

#pragma once
#include "mex.h"
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"
//#include <windows.h>
#include <math.h>
#include <process.h>
#include <stdio.h>

//----------------------------------------------------------------------------------------
// VER. 1.3
//----------------------------------------------------------------------------------------

/*  za�o�enie jest takie aby w�asciwa procedura by�a umieszczona w osobnej funkcji,
	dzi�ki czemu b�dzie j� mo�na p�niej �atwo przetransporotowac do innego programu.
	Funkcja ta musi pobiera� i zwraca� dane w formacie ju� klas C_Matrix itp
	(oczywi�cie je�li jest taka potrzeba) */



//	moja funkcja - dodaje dwie macierze do siebie (in1+in2) i zwraca wynik out
void ISAR_TH_Iterative(C_Image_Container* owe, C_Image_Container* owy, C_Error* perr);
//bool Add(C_Matrix_Container* in1,C_Matrix_Container* in2, C_Matrix_Container* out);

//	moja funkcja - dodaje dwie macierze do siebie (in1+in2) i zwraca wynik out. Oczywi�cie jest 
// i wska�nik do C_Error
// void Add(C_Matrix_Container* in1,C_Matrix_Container* in2, C_Matrix_Container* out, C_Error* perr);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs,
                 const mxArray *prhs[])
{


	double *p_in1, *p_out;//, *p_in2;			// wska�niki na dane w formacie matlaba
	//int a;
	//double *N, *w, *k;//, *xx, *yy;		//Zmienne do przechowywania parametr�w z matlaba
	//double N = 2, w = 15, k = 0.2;
	//bool err;
	C_Image_Container in1;			// wejscie do funkcji
	C_Image_Container out;// = new C_Image_Container;
	//C_Image_Container in2;			// wejscie do funkcji
	//C_Matrix_Container out;		// wyj�cie z funkcji
	C_Error errr;				// obs�uga b��d�w

//	�eby nie komplikowa� nie sprawdzam poprawnosci danych - zak�adam �e w matlabie bedzie wszystko dobrze wywo�ane
//	Najwy�ej mo�na dorobi� jakiego� m-file kt�ry to b�dzie robi�. W funkcji w�asciwej i tak trzeba to zaimplementowa�
//  �eby unikn�� problem�w gdy b�dzie ona pracowa� w innym �rodowisku
	
	p_in1 = mxGetPr(prhs[0]);
	//xx = mxGetPr(prhs[1]);
	//yy = mxGetPr(prhs[2]);
	
	/*
	w = mxGetPr(prhs[1]);
	k = mxGetPr(prhs[2]);
	N = mxGetPr(prhs[3]);
	*/
	
	//p_in2 = mxGetPr(prhs[1]);

	plhs[0] = mxCreateDoubleMatrix(mxGetM(prhs[0]),mxGetN(prhs[0]), mxREAL);	// to jest macierz wyj�ciowa w formacie matlaba o rozmiarze pierwszego parametru
	p_out = mxGetPr(plhs[0]);							// tu jej wska�nik

//	konwersja danych z matlaba na posta� akceptowalna przez moj� funkcj�
	in1.ImportFromMatlab(p_in1,mxGetM(prhs[0]),mxGetN(prhs[0]));	// importowanie danych z matlaba
	//in2.ImportFromMatlab(p_in2,mxGetM(prhs[1]),mxGetN(prhs[1]));	// importowanie danych z matlaba


	ISAR_TH_Iterative(&in1, &out, &errr);	// wywo�anie funkcji
	//Sauv(&in1, w, k, N, &out, &errr);	// wywo�anie funkcji
	//Add(&in1,&in2,&out,&errr);	// wywo�anie funkcji
	if(errr.status)		// obs�uga b��du zg�oszonego przez moj� funkcj�
	{
		mexPrintf("%s\n",errr.error);
		return;
	}

	out.ExportToMatlab(p_out);
}



//ODT�D W Dӣ MOJE

void ISAR_TH_Iterative(C_Image_Container* owe, C_Image_Container* owy, C_Error* perr)
{
	int i, j = 0, pom, pom1;
	double h[65536], d[65536], d2[65536];              //histogram, dystrybuanta
	double Tno, Tst;                     //pr�g nowy i stary
	double max, min;

	
	
	owy->AllocateData(owe->_rows, owe->_cols);
	owy->Zeros();
	
	//T - startowy pr�g
	owe->getMinMax(min, max);
	Tno = ((max - min) / 2.0);
	Tst = Tno - 100;


	
	//Zerowanie h, d, d2
	for(i = 0; i < 65536; i++)
	{
        h[i] = 0;
		d[i] = 0;
		d2[i] = 0;
	}

	//Wyliczanie histogramu
	for(i = 0; i < (owe->_cols * owe->_rows); i++)
	{
        h[(int)owe->data[i]] += 1;
	}

	
	//fprintf(plik, "Wyrzucam na ekran h i d\n");
	//Wyliczanie dystrybuanty z histogramu
	d[0] = h[0];
	d2[0] = 0;
	for(i = 1; i < 65536; i++)
	{
        d[i] = h[i] + d[i - 1];
		//if(i < 256) fprintf(plik, "%f  %f\n", h[i], d[i]);
		d2[i] = h[i] * i + d2[i - 1];
	}

	

	while(fabs(Tno - Tst) > 0.05)
	{
        Tst = Tno;
        pom = (int)Tst;
        Tno = ((d2[pom] / (float)d[pom]) + ((d2[65535] - d2[pom]) / (float)(d[65535] - d[pom]))) / 2.0;
		
    }
	


	//Progowanie
	for(i = 0; i < (owe->_cols * owe->_rows); i++)
	{
        if(owe->data[i] < Tno) owy->data[i] = 0;
        else owy->data[i] = 1;
	}
}
