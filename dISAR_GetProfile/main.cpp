#pragma once
#include "mex.h"
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"
#include "C_DumpAll.h"

#include	"coeff.h"
#include	"interpol.h"

#ifdef _DEBUG 
C_DumpAll dump("debug.out");
#endif

struct PUNKT {
	double x;
	double y;
};

struct PROSTA {
	double a; // wsp�czynnik kierunkowy
	double b; // wyraz sta�y
	BOOL status;	// je�li status TRUE to prosta jest pionowa
};
/* ------------------------------------------------------------------------------------------------------------------------------ */
void cISAR_GetProfile(C_Image_Container *p_image,	// INPUT: obraz wejciowy
					  double x0,					// INPUT: wsp�rz�dna x pierwszego punktu
					  double y0,					// INPUT: wsp�rz�dna y pierwszego punktu
					  double x1,					// INPUT: wsp�rz�dna x drugiego punktu
					  double y1,					// INPUT: wsp�rz�dna y drugiego punktu
					  unsigned int Nlen,			// INPUT: ilo�� punkt�w na odcinek P0P1
					  unsigned int Nszer,			// INPUT: po�owa grubo�ci linii (je�li 0 to jednal linia, 2 to trzy linie itd)
					  char method,					// INPUT: metoda: 0 - NN, 1 - spline
					  C_Matrix_Container *p_out,	// OUTPUT: macierz z profilem jasno�ci
					  C_Error* err);
/* ------------------------------------------------------------------------------------------------------------------------------ */

/*
	Funkcja interpoluj�ca NN
	Pobiera:
		wska�nik do obrazka jako Matrix_Container
		wsp�rz�dn� x (kolumny) oraz y (rz�dy) dla kt�rej zwraca warto�� interpolowan�
*/
double NNInterp(C_Matrix_Container*,double, double);
inline int round(double);
void getLineCoefs(PUNKT*,PUNKT*,PROSTA*);	// zwraca wsp�czynniki prostej przechodzacej przez dwa punkty y=ax+b oraz informacj� czy prosta jest pionowa
void getLineCoefs(PUNKT*,PROSTA*,PROSTA*);	// zwraca wsp�czynniki prostej prostopad�ej do przostej i przechodz�c� przez punkt oraz informacj� czy prosta jest pionowa
void DumpAll_Convert(PUNKT**,int,int,char*,char*); // funkcja do konwersji tablicy zawierajacej struktury POINT aby mo�na by�o j� nagra� 
void SwapPoints(PUNKT*,PUNKT*);					// sortuje punkty tak aby pierwszy by� zawsze z mniejszym x lub y gdy s� pionowe
void LineCutsCircle(PUNKT,PROSTA,double,PUNKT*,PUNKT*);							// zwraca dwa punkty przeci�cia okr�gu o �rodku P i promieniy res_y i prostej przechodz�cej przez P
void getPointsonLine(PUNKT,PUNKT,PROSTA,int,PUNKT*);			// zwraca punkty w zakresie od P0 do P1 le��ce na prostej l


/*
	M-file: out = mISAR_GetProfile(...)
	[0] - obraz wej�ciowy
	[1] - wsp�rz�dna x pierwszego punktu
	[2] - wsp�rz�dna y pierwszego punktu
	[3] - wsp�rz�dna x drugiego punktu
	[4] - wsp�rz�dna y drugiego punktu
	[5] - ilo�� punkt�w na odcinek P0P1
	[6] - po�owa szeroko�ci odcinka
	[7] - metoda
*/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs,
                 const mxArray *prhs[])
{
	C_Image_Container image;
	C_Matrix_Container out;
	double x0,y0,x1,y1,Nlen,Nszer;
	char method;
	double *p_image,*p_out;
	C_Error err;

	p_image = mxGetPr(prhs[0]);
	x0 = (double)mxGetScalar(prhs[1]);
	y0 = (double)mxGetScalar(prhs[2]);
	x1 = (double)mxGetScalar(prhs[3]);
	y1 = (double)mxGetScalar(prhs[4]);
	Nlen = (double)mxGetScalar(prhs[5]);
	Nszer = (double)mxGetScalar(prhs[6]);
	method = (char)mxGetScalar(prhs[7]);

	image.ImportFromMatlab(p_image,(unsigned int)mxGetM(prhs[0]),(unsigned int)mxGetN(prhs[0]));

	cISAR_GetProfile(&image,	// obraz wejciowy
					 x0,		// wsp�rz�dna x pierwszego punktu
					 y0,		// wsp�rz�dna y pierwszego punktu
					 x1,		// wsp�rz�dna x drugiego punktu
					 y1,		// wsp�rz�dna y drugiego punktu
					 Nlen,		// ilo�� punkt�w na odcinek P0P1
					 Nszer,		// po�owa grubo�ci linii (je�li 0 to jednal linia, 2 to trzy linie itd)
					 method,	// metoda: 0 - NN, 1 - spline
					 &out,		// macierz z profilem jasno�ci
					 &err);

	plhs[0] = mxCreateDoubleMatrix(out._rows,out._cols,mxREAL);
	p_out = mxGetPr(plhs[0]);
	out.ExportToMatlab(p_out);	
}

void cISAR_GetProfile(C_Image_Container *p_image,	// obraz wejciowy
					  double x0,					// wsp�rz�dna x pierwszego punktu
					  double y0,					// wsp�rz�dna y pierwszego punktu
					  double x1,					// wsp�rz�dna x drugiego punktu
					  double y1,					// wsp�rz�dna y drugiego punktu
					  unsigned int Nlen,			// ilo�� punkt�w na odcinek P0P1
					  unsigned int Nszer,			// po�owa grubo�ci linii (je�li 0 to jednal linia, 2 to trzy linie itd)
					  char method,					// metoda: 0 - NN, 1 - spline
					  C_Matrix_Container *p_out,	// macierz z profilem jasno�ci
					  C_Error *p_err)
{
	unsigned int &res_x=Nlen;	// nie chce mi si� popraiwia� nazw zmiennych
	C_Matrix_Container line;		// wynik interpolacji
	unsigned int res_y;
	PUNKT P0, P1;		// punkty pocz�tkowe oraz ko�cowe
	float *data;	// obraz w postaci float - do funkcji splajnowej
	int Error;
	unsigned int a,y,x;
	double p;
	PUNKT **tabp;	// tablica 2D zawierajaca wsp�rz�dne punkt�w le��cych na prostych
	PUNKT *tabL2,*tabL3;	// tablice zawieraj�ce punkty le��ce na liniach grubo�ci L2 i L3
	PUNKT P0L2, P1L2;	// punkty nale��ce do prostej L2 i le��ce w odleg�o�ci res_y od punktu P0
	PUNKT P0L3, P1L3;	// punkty nale��ce do prostej L3 i le��ce w odleg�o�ci res_y od punktu P1
	PROSTA l;		// prosta ��cz�ca punkt P0 i P1 
	PROSTA l2;		// prosta prostopad�a do l i przechodz�ca przez P0
	PROSTA l3;		// prosta prostopad�a do l i przechodz�ca przez P1
	PROSTA ltmp;

	res_y = 2*Nszer+1;
	P0.x = x0; P0.y = y0;
	P1.x = x1; P1.y = y1;

	if(method>1 || method<0)
	{
		p_err->SetError("GetProfile->Unknown Interpolation");
		return;
	}
	/* ----- Sortowanie punkt�w P0 - lewy, P1 - prawy, g�ra d� ----- */
	SwapPoints(&P0,&P1);
/* ----- konwersja obrazka do wartosci float - wymagane przez funkcj� InterpolatedValue ----- */
	data = new float[p_image->GetNumofElements()];
	for(a=0;a<p_image->GetNumofElements();a++)
		data[a] = (float)p_image->data[a];
	Error = SamplesToCoefficients(data, p_image->_cols, p_image->_rows, 2);
	if(Error)
	{
		p_err->SetError("GetProfile->SamplesToCoefficients->Error");
		return;
	}
	/* ----- Obliczanie wsp�czynnik�w prostej pomi�dzy punktami P0 i P1 ----- */
	getLineCoefs(&P0,&P1,&l);
/* ----- Obliczenie wsp�czynnik�w prostej prostopad�ej l2 do l i przechodzacej przez P0 ----- */
	getLineCoefs(&P0,&l,&l2);
/* ----- Obliczenie wsp�czynnik�w prostej prostopad�ej l3 do l i przechodzacej przez P1 ----- */
	getLineCoefs(&P1,&l,&l3);
/* ----- Obliczenie wsp punkt�w P0L2 i P1L2 le��cych na prostej l2 w odleg�o�ci r_y od niej ----- */
	LineCutsCircle(P0,l2,Nszer,&P0L2,&P1L2);
	SwapPoints(&P0L2,&P1L2);
/* ----- Obliczenie wsp punkt�w P0L3 i P1L3 le��cych na prostej l3 w odleg�o�ci r_y od niej ----- */
	LineCutsCircle(P1,l3,Nszer,&P0L3,&P1L3);
	SwapPoints(&P0L3,&P1L3);
#ifdef _DEBUG
	dump.AddEntry(&l.a,1,"l_a");
	dump.AddEntry(&l.b,1,"l_b");
	dump.AddEntry(&l2.a,1,"l2_a");
	dump.AddEntry(&l2.b,1,"l2_b");
	dump.AddEntry(&l3.a,1,"l3_a");
	dump.AddEntry(&l3.b,1,"l3_b");
	dump.AddEntry(&P0L2.x,1,"P0L2_x");dump.AddEntry(&P0L2.y,1,"P0L2_y");
	dump.AddEntry(&P1L2.x,1,"P1L2_x");dump.AddEntry(&P1L2.y,1,"P1L2_y");
	dump.AddEntry(&P0L3.x,1,"P0L3_x");dump.AddEntry(&P0L3.y,1,"P0L3_y");
	dump.AddEntry(&P1L3.x,1,"P1L3_x");dump.AddEntry(&P1L3.y,1,"P1L3_y");
#endif	
	/* ----- Allokacja tablicy na punkty oraz obliczanie punkt�w ----- */
	tabL2 = new PUNKT[res_y];
	tabL3 = new PUNKT[res_y];
	tabp = new PUNKT*[res_y];		// je�li r_y = 0 to tylko jedna linia
	for(a=0;a<res_y;a++)
		tabp[a] = new PUNKT[res_x];
	if(res_y==1)			// tylko jedna linia i to musi by� dok�adnie ta �rodkowa
		getPointsonLine(P0,P1,l,res_x,tabp[0]);
	else	// wi�cej linii - generuje punkty na L2 i L3 i dopiero p�niej wyznaczam r�wnania linii pomi�dzy tymi punktami i punkty do tabp
	{
		getPointsonLine(P0L2,P1L2,l2,res_y,tabL2);
		getPointsonLine(P0L3,P1L3,l3,res_y,tabL3);
		for(a=0;a<res_y;a++)
		{
			SwapPoints(&tabL2[a],&tabL3[a]);
			getLineCoefs(&tabL2[a],&tabL3[a],&ltmp);	// prosta pomi�dzy kolejnymi punktami na prostych L2 i L3
			getPointsonLine(tabL2[a],tabL3[a],ltmp,res_x,tabp[a]);
		}
	}
#ifdef _DEBUG
		DumpAll_Convert(tabp,res_x,res_y,"tabp_x","tabp_y");
		DumpAll_Convert(&tabL2,res_y,1,"tabL2_x","tabL2_y"); 
		DumpAll_Convert(&tabL3,res_y,1,"tabL3_x","tabL3_y");
#endif	
/* ----- Wykonanie interplolacji dla punkt�w ----- */	
	line.AllocateData(res_y,res_x);
	for(y=0;y<res_y;y++)	// po szeroko�ci
		for(x=0;x<res_x;x++)	// po d�ugo�ci
		{
			if(method==0)
			{
				p = NNInterp(p_image,tabp[y][x].x,tabp[y][x].y);
				line.SetPixel(y,x,p);
			}
			if(method==1)
			{
				p = InterpolatedValue(data,
									  p_image->_cols,
									  p_image->_rows,
									  tabp[y][x].x,
									  tabp[y][x].y,
									  2);
				line.SetPixel(y,x,p);
			}
		}
#ifdef _DEBUG
	dump.AddEntry(&line,"line");
#endif	
/* ----- Usrednianie wynik�w po kolumnach ----- */	
	line.Mean(p_out,0);

	for(a=0;a<res_y;a++)
		delete[] tabp[a];
	delete[] tabp;
	delete[] tabL2;
	delete[] tabL3;
}

void getPointsonLine(PUNKT P0,PUNKT P1,PROSTA l,int res_x,PUNKT *out)
{
	/* ----- obliczenie okreslonej ilo�ci warto�ci w zadanym zakresie <a;b>. Warto�� b mo�e nie zosta� osi�gni�ta ----- */
	int licz = 0;
	double xtmp,dx;
	if(l.status)	// je�li prosta pionowa to wszystko po y dla ustalonego x
	{
		xtmp = P0.y;
		dx = (P1.y - P0.y)/(res_x-1);
		while(licz<res_x)
		{
			out[licz].x = P0.x;	// x sie nie zmienia i jest taki sam dla obu punkt�w
			out[licz].y = xtmp;
			xtmp+=dx;
			licz++;
		}
	} else // prosta niepionowa
	{
		xtmp = P0.x;
		dx = (P1.x - P0.x)/(res_x-1);
		while(licz<res_x)
		{
			out[licz].x = xtmp;
			out[licz].y = l.a*xtmp + l.b;
			xtmp+=dx;
			licz++;
		}
	}
}

void LineCutsCircle(PUNKT P0,PROSTA l,double R,PUNKT *P1,PUNKT *P2)
/*
	P0 - srodek okr�gu
	l - prosta przechodz�ca przez P0
	R - promie� okr�gu
	P1, P2 - wyj�ciowy punkty przeci�cia
*/
{
	double a;
	if(!l.status)
	{
		a = l.a;
		P1->x = P0.x+R*sqrt(1/(a*a+1));
		P1->y = P0.y+R*a*sqrt(1/(a*a+1));

		P2->x = P0.x-R*sqrt(1/(a*a+1));
		P2->y = P0.y-R*a*sqrt(1/(a*a+1));
	} else
	{
		P1->x = P0.x;
		P1->y = P0.y - R;

		P2->x = P0.x;
		P2->y = P0.y + R;
	}
}

void SwapPoints(PUNKT *P0,PUNKT *P1)
{
	PUNKT ptmp;
	if(P0->x>P1->x)
	{
		ptmp = *P0;
		*P0 = *P1;
		*P1 = ptmp;
	} else
		if(P0->x==P1->x)	// sortowanie o y je�li s� pionowo
			if(P0->y>P1->y)
			{
				ptmp = *P0;
				*P0 = *P1;
				*P1 = ptmp;
			}

}
void DumpAll_Convert(PUNKT** p,int sizx,int sizy,char* nx,char* ny)
{
	C_Matrix_Container tmpx;
	C_Matrix_Container tmpy;
	tmpx.AllocateData(sizy,sizx);
	tmpy.AllocateData(sizy,sizx);
	int x,y;
	for(y=0;y<sizy;y++)
		for(x=0;x<sizx;x++)
		{
			tmpx.SetPixel(y,x,p[y][x].x);
			tmpy.SetPixel(y,x,p[y][x].y);
		}
#ifdef _DEBUG 
	dump.AddEntry(&tmpx,nx);
	dump.AddEntry(&tmpy,ny);
#endif
}

void getLineCoefs(PUNKT *P1,PROSTA *lp,PROSTA *l)
{
	double a,b;
	BOOL status;
	if(lp->a==0)	// prosta pozioma, czyli prosta l b�dzie pionowa
	{
		a = 0;
		b = 0;
		status = TRUE;
	} else
	{
		a = -1/lp->a;
		b = P1->y-a*P1->x;
		status = FALSE;
	}
	l->a = a;
	l->b = b;
	l->status = status;
}

void getLineCoefs(PUNKT *P1,PUNKT *P2,PROSTA *l)
{
	double a,b;
	BOOL status;
	if(P2->x-P1->x==0)
	{
		a = 0;
		b = 0;
		status = TRUE;	// prosta pionowa
	} else
	{
		a = (P2->y-P1->y)/(P2->x-P1->x);
		b = P2->y-a*P2->x;
		status = FALSE;
	}
	l->a = a;
	l->b = b;
	l->status = status;
}

double NNInterp(C_Matrix_Container *in,double x, double y)
{
	int xn,yn;
	double w;
	xn = round(x);
	yn = round(y);
	if(xn<0) xn = 0;
	if(yn<0) yn = 0;
	if(xn>=in->_cols) xn = in->_cols-1;
	if(yn>=in->_rows) yn = in->_rows-1;
	w = in->GetPixel(yn,xn);
	return w;
}

inline int round(double in)
{
	return (int)floor(in+0.5);

}