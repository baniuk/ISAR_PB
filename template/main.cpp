#pragma once
#include "mex.h"
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"

/*  za�o�enie jest takie aby w�asciwa procedura by�a umieszczona w osobnej funkcji,
	dzi�ki czemu b�dzie j� mo�na p�niej �atwo przetransporotowac do innego programu.
	Funkcja ta musi pobiera� i zwraca� dane w formacie ju� klas C_Matrix itp
	(oczywi�cie je�li jest taka potrzeba) */

//	moja funkcja - dodaje dwie macierze do siebie (in1+in2) i zwraca wynik out. Oczywi�cie jest 
// i wska�nik do C_Error
void Add(C_Matrix_Container* in1,C_Matrix_Container* in2, C_Matrix_Container* out, C_Error* perr);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs,
                 const mxArray *prhs[])
{
	double *p_in1, *p_in2, *p_out;	// wska�niki na dane w formacie matlaba
	C_Matrix_Container in1;		// wejscie do funkcji
	C_Matrix_Container in2;
	C_Matrix_Container out;		// wyj�cie z funkcji
	C_Error err;				// obs�uga b��d�w

//	�eby nie komplikowa� nie sprawdzam poprawnosci danych - zak�adam �e w matlabie bedzie wszystko dobrze wywo�ane
//	Najwy�ej mo�na dorobi� jakiego� m-file kt�ry to b�dzie robi�. W funkcji w�asciwej i tak trzeba to zaimplementowa�
//  �eby unikn�� problem�w gdy b�dzie ona pracowa� w innym �rodowisku
	
	p_in1 = mxGetPr(prhs[0]);
	p_in2 = mxGetPr(prhs[1]);
	plhs[0] = mxCreateDoubleMatrix(mxGetM(prhs[0]),mxGetN(prhs[0]), mxREAL);	// to jest macierz wyj�ciowa w formacie matlaba o rozmiarze pierwszego parametru
	p_out = mxGetPr(plhs[0]);							// tu jej wska�nik

//	konwersja danych z matlaba na posta� akceptowalna przez moj� funkcj�
	in1.ImportFromMatlab(p_in1,mxGetM(prhs[0]),mxGetN(prhs[0]));	// importowanie danych z matlaba
	in2.ImportFromMatlab(p_in2,mxGetM(prhs[1]),mxGetN(prhs[1]));	// importowanie danych z matlaba

	Add(&in1,&in2,&out,&err);	// wywo�anie funkcji
	if(err.status)		// obs�uga b��du zg�oszonego przez moj� funkcj�
	{
		mexPrintf("%s\n",err.error);
		return;
	}
//	konwersja danych z mojej funkcji do matlaba
	out.ExportToMatlab(p_out);
}

void Add(C_Matrix_Container* in1,C_Matrix_Container* in2, C_Matrix_Container* out, C_Error* perr)
{
// w�a�ciwa funkcja
	int a;
	if( (in1->_cols!=in2->_cols) || (in1->_rows!=in2->_rows))	// kontrola poprawno�ci danych
	{
		perr->SetError("Niekompatybilny rozmiar macierzy"); // obs�uga b��du
		return;
	}

	out->AllocateData(in1->_rows,in1->_cols);
	for(a=0;a<in1->GetNumofElements();a++)
		out->data[a] = in1->data[a] + in2->data[a];
}