/*****************************************************************************************
Program: Implementacja progowania Sauvola z wykorzystaniem wielo¹tkowoœci
Autor: Lech Napiera³a
Parametry wejœciowe:
	owe - obraz w odcieniach szaroœci do sprogowania
	w - rozmiar okna progowania (koniecznie nieparzysty!)
	k - parametr 'k' progowania (standardowa wartoœæ to 0.2)
Parametry wyjœciowe:
	owy - obraz dwukolorowy po progowaniu
Wersja: 1.0
*****************************************************************************************/

#pragma once
#include "mex.h"
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"
#include <process.h>
#include <stdio.h>

#define N 400	//maksymalna liczba procesorów

/*  za³o¿enie jest takie aby w³asciwa procedura by³a umieszczona w osobnej funkcji,
	dziêki czemu bêdzie j¹ mo¿na póŸniej ³atwo przetransporotowac do innego programu.
	Funkcja ta musi pobieraæ i zwracaæ dane w formacie ju¿ klas C_Matrix itp
	(oczywiœcie jeœli jest taka potrzeba) */



//	moja funkcja - dodaje dwie macierze do siebie (in1+in2) i zwraca wynik out
void Sauv(C_Image_Container* owe, double w, double k, double n, C_Image_Container* owy, C_Error* perr);
//bool Add(C_Matrix_Container* in1,C_Matrix_Container* in2, C_Matrix_Container* out);

//	moja funkcja - dodaje dwie macierze do siebie (in1+in2) i zwraca wynik out. Oczywiœcie jest 
// i wskaŸnik do C_Error
//void Add(C_Matrix_Container* in1,C_Matrix_Container* in2, C_Matrix_Container* out, C_Error* perr);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs,
                 const mxArray *prhs[])
{

	double *p_in1, *p_out;//, *p_in2;			// wskaŸniki na dane w formacie matlaba
	//int a;
	double *pN, *w, *k;//, *xx, *yy;		//Zmienne do przechowywania parametrów z matlaba
	//double N = 2, w = 15, k = 0.2;
	//bool err;
	C_Image_Container in1;			// wejscie do funkcji
	C_Image_Container out;// = new C_Image_Container;
	//C_Image_Container in2;			// wejscie do funkcji
	//C_Matrix_Container out;		// wyjœcie z funkcji
	C_Error errr;				// obs³uga b³êdów

//	¿eby nie komplikowaæ nie sprawdzam poprawnosci danych - zak³adam ¿e w matlabie bedzie wszystko dobrze wywo³ane
//	Najwy¿ej mo¿na dorobiæ jakiegoœ m-file który to bêdzie robi³. W funkcji w³asciwej i tak trzeba to zaimplementowaæ
//  ¿eby unikn¹æ problemów gdy bêdzie ona pracowaæ w innym œrodowisku
	
	p_in1 = mxGetPr(prhs[0]);
	//xx = mxGetPr(prhs[1]);
	//yy = mxGetPr(prhs[2]);
	
	w = mxGetPr(prhs[1]);
	k = mxGetPr(prhs[2]);
	pN = mxGetPr(prhs[3]);
	
	//p_in2 = mxGetPr(prhs[1]);

	plhs[0] = mxCreateDoubleMatrix(mxGetM(prhs[0]),mxGetN(prhs[0]), mxREAL);	// to jest macierz wyjœciowa w formacie matlaba o rozmiarze pierwszego parametru
	p_out = mxGetPr(plhs[0]);							// tu jej wskaŸnik

//	konwersja danych z matlaba na postaæ akceptowalna przez moj¹ funkcjê
	in1.ImportFromMatlab(p_in1,mxGetM(prhs[0]),mxGetN(prhs[0]));	// importowanie danych z matlaba
	//in2.ImportFromMatlab(p_in2,mxGetM(prhs[1]),mxGetN(prhs[1]));	// importowanie danych z matlaba

	Sauv(&in1, *w, *k, *pN, &out, &errr);	// wywo³anie funkcji
	//Sauv(&in1, w, k, N, &out, &errr);	// wywo³anie funkcji
	//Add(&in1,&in2,&out,&errr);	// wywo³anie funkcji
	if(errr.status)		// obs³uga b³êdu zg³oszonego przez moj¹ funkcjê
	{
		mexPrintf("%s\n",errr.error);
		return;
	}
//	konwersja danych z mojej funkcji do matlaba
	out.ExportToMatlab(p_out);
}





//Struktury do przekazywania danych w¹tkom:
struct CIC
{
	C_Image_Container *o1;
	C_Image_Container *o2;
} cic[2];

struct SAV
{
	int imin, imax;
	int jmin, jmax;
	int w;
	double k;
	double R;
	C_Image_Container *owe;
	C_Image_Container *owy;
	C_Image_Container *II;
	C_Image_Container *II2;
} sav[N];

//Compute Integral Image - liczy Integral Image z tego co dostanie na wejœciu
//z obrwe robimy integrala i zapisujemy w obrwy
unsigned int WINAPI CII(void* param)
{
	struct CIC *c;

	c = (struct CIC *)param;

	C_Image_Container *obrwe = c->o1;
	C_Image_Container *obrwy = c->o2;

	int i, j;

	for(i = 1; i < obrwe->_cols; i++) 
	{
		obrwy->data[i] = obrwe->data[i] + obrwy->data[i - 1]; //2
	}
	for(i = 1; i < obrwe->_rows; i++) //3
	{
		obrwy->data[i * obrwe->_cols] = obrwe->data[i * obrwe->_cols] + obrwy->data[(i - 1) * obrwe->_cols];
	}
		
	for(i = 1; i < obrwe->_rows; i++) //4
	{
		for(j = 1; j < obrwe->_cols; j++)
		{
			obrwy->data[i * (obrwe->_cols) + j] =
				obrwy->data[(i - 1) * obrwe->_cols + j] + obrwy->data[i * obrwe->_cols + j - 1] - 
				obrwy->data[(i - 1) * obrwe->_cols + j - 1] + obrwe->data[i * obrwe->_cols + j];
		}
	}
	return 0;
}

unsigned int WINAPI SAUVOLA(void* param)
{
	struct SAV *s;
	s = (struct SAV *)param;

	int i, j, imin, imax, jmin, jmax, fW, w;
	double k, R;
	double wspW, wspW2, srednia, suma1, odSt, t;
	C_Image_Container *II, *II2, *owe, *owy;
	unsigned long w2;
	imin = s->imin;
	imax = s->imax;
	jmin = s->jmin;
	jmax = s->jmax;
	w = s->w;
	k = s->k;
	R = s->R;
	II = s->II;
	II2 = s->II2;
	owe = s->owe;
	owy = s->owy;
	wspW = 1 / pow(w, 2.0);
	wspW2 = 1 / (pow(w, 2.0) - 1);
	w2 = pow(w, 2.0);
	fW = int(w / 2.0);

	for(i = imin; i <= imax; i++)
	{
		for(j = jmin; j <= jmax; j++)
		{


			srednia = 
				(II->data[(i + fW) * owe->_cols + j + fW] +
				II->data[(i - fW - 1) * owe->_cols + j - fW - 1] -
				II->data[(i - fW - 1) * owe->_cols + j + fW] -
				II->data[(i + fW) * owe->_cols + j - fW - 1]) *
				wspW;

			
			suma1 =
				II2->data[(i + fW) * owe->_cols + j + fW] +
				II2->data[(i - fW - 1) * owe->_cols + j - fW - 1] -
				II2->data[(i - fW - 1) * owe->_cols + j + fW] -
				II2->data[(i + fW) * owe->_cols + j - fW - 1];

		

			odSt = sqrt(wspW2 * (suma1 - w2 * pow(srednia, 2)));
			t = srednia * (1 + k * ((odSt / R) - 1));

			
			
			if(owe->data[i * (owe->_cols) + j] > t)
				owy->data[i * (owe->_cols) + j] = 1;
			
			
		}
	}
	return(0);
}


void Sauv(C_Image_Container* owe, double w, double k, double n, C_Image_Container* owy, C_Error* perr)
{
	C_Image_Container *owe2 = new C_Image_Container;//obraz wejœciowy do kwadratu
	C_Image_Container *II = new C_Image_Container;  //Integral image
	C_Image_Container *II2 = new C_Image_Container; //Integral image kwadratów
	
	int i, j = 0, r, d, g, pom;		//wiêkszoœæ do wyznacznia przedzia³ów
	double R;
	bool xlonger = false;
	double max, min;

	w = (int)w;

	//owy = new C_Image_Container();
	

	//Zmienne do w¹tków
	HANDLE hThread[N];
	owy->AllocateData(owe->_rows, owe->_cols);
	owy->Zeros();
	owe2->AllocateData(owe->_rows, owe->_cols);
	II->AllocateData(owe->_rows, owe->_cols);
	II->Zeros();
	II2->AllocateData(owe->_rows, owe->_cols);
	II2->Zeros();

	owe->getMinMax(min, max);
	R = ((max - min) / 2.0);

	//Wyliczanie owe2 (kwadraty wejœcia)
	for(i = 0; i < (owe->_cols * owe->_rows); i++)
	{
		owe2->data[i] = pow(owe->data[i], 2);
	}
	
	//Liczenie Integral Images na w¹tkach:
	cic[0].o1 = owe2;
	cic[0].o2 = II2;
	cic[1].o1 = owe;
	cic[1].o2 = II;
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, CII, (void*)&cic[0], 0, NULL);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, CII, (void*)&cic[1], 0, NULL);
	//CII(NULL);
	//WaitForSingleObject(hThread[0], INFINITE);
	//WaitForSingleObject(hThread[1], INFINITE);
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	//DZIA£A!!!!! : )))


	//LICZENIE SAMEGO PROGOWANIA SAUVOLA NA W¥TKACH:


	//Podzia³ obrazu
	if(w < owe->_cols && w < owe->_rows) //Sprawdzamy czy obraz jest wiêkszy od maski, inaczej dalsze liczenie nie ma sensu
	{
		if(owe->_cols - w < n || owe->_rows - w < n) n = 1;
		if(owe->_cols > owe->_rows) 
		{
			pom = owe->_cols - w;
			xlonger = true;
		}
		else
		{
			pom = owe->_rows - w;
			xlonger = false;
		}
		if(n <= 0) n = 1;
		d = (int)(pom / n);
		r = pom % (int)n;
		for(i = 0; i < n; i++)
 		{
	 		g = j + d - 1;
			  
			if(r > 0)
			{
				r--;
				g++;
			}
			if(xlonger)
			{
				sav[i].imin = (int)(w / 2.0) + 1;
				sav[i].imax = owe->_rows - (int)(w / 2.0) - 1;
				sav[i].jmin = j + (int)(w / 2.0);
				sav[i].jmax = g + (int)(w / 2.0);
			}
			else
			{
				sav[i].imin = j + (int)(w / 2.0) + 1;
				sav[i].imax = g + (int)(w / 2.0) + 1;
				sav[i].jmin = (int)(w / 2.0) + 2;
				sav[i].jmax = owe->_cols - (int)(w / 2.0);
			}
			sav[i].w = (int)w;
			sav[i].k = k;
			sav[i].R = R;
			sav[i].II = II;
			sav[i].II2 = II2;
			sav[i].owe = owe;
			sav[i].owy = owy;
	        
			j = g + 1;
		}
		sav[0].imin = (int)(w / 2.0) + 1;
		sav[0].jmin = (int)(w / 2.0) + 1;

		
		for(i = 0; i < n; i++)
		{
			hThread[i] = (HANDLE)_beginthreadex(NULL, 0, SAUVOLA, (void*)&sav[i], 0, NULL);
		}
		
		WaitForMultipleObjects(n, hThread, TRUE, INFINITE);
		
		for(i = 0; i < n; i++)
		{
			CloseHandle(hThread[i]);
		}
	}
	//owy->CloneObject(out);
	SAFE_DELETE(owe2);
	SAFE_DELETE(II);
	SAFE_DELETE(II2);
}