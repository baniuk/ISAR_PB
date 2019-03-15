#pragma once
#include "mex.h"
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"

/*  za³o¿enie jest takie aby w³asciwa procedura by³a umieszczona w osobnej funkcji,
	dziêki czemu bêdzie j¹ mo¿na póŸniej ³atwo przetransporotowac do innego programu.
	Funkcja ta musi pobieraæ i zwracaæ dane w formacie ju¿ klas C_Matrix itp
	(oczywiœcie jeœli jest taka potrzeba) */

//	moja funkcja - dodaje dwie macierze do siebie (in1+in2) i zwraca wynik out. Oczywiœcie jest 
// i wskaŸnik do C_Error
void Add(C_Matrix_Container* in1,C_Matrix_Container* in2, C_Matrix_Container* out, C_Error* perr);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs,
                 const mxArray *prhs[])
{
	double *p_in1, *p_in2, *p_out;	// wskaŸniki na dane w formacie matlaba
	C_Matrix_Container in1;		// wejscie do funkcji
	C_Matrix_Container in2;
	C_Matrix_Container out;		// wyjœcie z funkcji
	C_Error err;				// obs³uga b³êdów

//	¿eby nie komplikowaæ nie sprawdzam poprawnosci danych - zak³adam ¿e w matlabie bedzie wszystko dobrze wywo³ane
//	Najwy¿ej mo¿na dorobiæ jakiegoœ m-file który to bêdzie robi³. W funkcji w³asciwej i tak trzeba to zaimplementowaæ
//  ¿eby unikn¹æ problemów gdy bêdzie ona pracowaæ w innym œrodowisku
	
	p_in1 = mxGetPr(prhs[0]);
	p_in2 = mxGetPr(prhs[1]);
	plhs[0] = mxCreateDoubleMatrix(mxGetM(prhs[0]),mxGetN(prhs[0]), mxREAL);	// to jest macierz wyjœciowa w formacie matlaba o rozmiarze pierwszego parametru
	p_out = mxGetPr(plhs[0]);							// tu jej wskaŸnik

//	konwersja danych z matlaba na postaæ akceptowalna przez moj¹ funkcjê
	in1.ImportFromMatlab(p_in1,mxGetM(prhs[0]),mxGetN(prhs[0]));	// importowanie danych z matlaba
	in2.ImportFromMatlab(p_in2,mxGetM(prhs[1]),mxGetN(prhs[1]));	// importowanie danych z matlaba

	Add(&in1,&in2,&out,&err);	// wywo³anie funkcji
	if(err.status)		// obs³uga b³êdu zg³oszonego przez moj¹ funkcjê
	{
		mexPrintf("%s\n",err.error);
		return;
	}
//	konwersja danych z mojej funkcji do matlaba
	out.ExportToMatlab(p_out);
}

void Add(C_Matrix_Container* in1,C_Matrix_Container* in2, C_Matrix_Container* out, C_Error* perr)
{
// w³aœciwa funkcja
	int a;
	if( (in1->_cols!=in2->_cols) || (in1->_rows!=in2->_rows))	// kontrola poprawnoœci danych
	{
		perr->SetError("Niekompatybilny rozmiar macierzy"); // obs³uga b³êdu
		return;
	}

	out->AllocateData(in1->_rows,in1->_cols);
	for(a=0;a<in1->GetNumofElements();a++)
		out->data[a] = in1->data[a] + in2->data[a];
}