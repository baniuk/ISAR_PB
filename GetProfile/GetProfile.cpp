// GetProfile.cpp : Defines the entry point for the console application.
//
/* Projekt startowy dla funkcji GetProfile. PóŸniej zosta³ skopiowany do funkcji ju¿ matlabowej */

#include "stdafx.h"
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
	double a; // wspó³czynnik kierunkowy
	double b; // wyraz sta³y
	BOOL status;	// jeœli status TRUE to prosta jest pionowa
};

/*
	Funkcja interpoluj¹ca NN
	Pobiera:
		wskaŸnik do obrazka jako Matrix_Container
		wspó³rzêdn¹ x (kolumny) oraz y (rzêdy) dla której zwraca wartoœæ interpolowan¹
*/
double NNInterp(C_Image_Container*,double, double);
inline int round(double);
void getLineCoefs(PUNKT*,PUNKT*,PROSTA*);	// zwraca wspó³czynniki prostej przechodzacej przez dwa punkty y=ax+b oraz informacjê czy prosta jest pionowa
void getLineCoefs(PUNKT*,PROSTA*,PROSTA*);	// zwraca wspó³czynniki prostej prostopad³ej do przostej i przechodz¹c¹ przez punkt oraz informacjê czy prosta jest pionowa
void DumpAll_Convert(PUNKT**,int,int,char*,char*); // funkcja do konwersji tablicy zawierajacej struktury POINT aby mo¿na by³o j¹ nagraæ 
void SwapPoints(PUNKT*,PUNKT*);					// sortuje punkty tak aby pierwszy by³ zawsze z mniejszym x lub y gdy s¹ pionowe
void LineCutsCircle(PUNKT,PROSTA,double,PUNKT*,PUNKT*);							// zwraca dwa punkty przeciêcia okrêgu o œrodku P i promieniy res_y i prostej przechodz¹cej przez P
void getPointsonLine(PUNKT,PUNKT,PROSTA,int,PUNKT*);			// zwraca punkty w zakresie od P0 do P1 le¿¹ce na prostej l

void cISAR_GetProfile(C_Image_Container *p_image,	// obraz wejciowy
					  double x0,					// wspó³rzêdna x pierwszego punktu
					  double y0,					// wspó³rzêdna y pierwszego punktu
					  double x1,					// wspó³rzêdna x drugiego punktu
					  double y1,					// wspó³rzêdna y drugiego punktu
					  unsigned int Nlen,			// iloœæ punktów na odcinek P0P1
					  unsigned int Nszer,			// po³owa gruboœci linii (jeœli 0 to jednal linia, 2 to trzy linie itd)
					  char method,					// metoda: 0 - NN, 1 - spline
					  C_Matrix_Container *p_out,	// macierz z profilem jasnoœci
					  C_Error* err);

int _tmain(int argc, _TCHAR* argv[])
{
	C_Image_Container image;
	C_Matrix_Container mean;		// srednia z interpolowanych linii
	C_Error perr;



/* --------------- dane wejœciowe -----------------------*/
/*	P1.x = 100;		// punkt pocz¹tkowy [x]
	P1.y = 100;		// punkt pocz¹tkowy [y]
	P0.x = 200;		// punkt koñcowy [x]
	P0.y = 100;		// punkt koñcowy [y]
	res_x = 100;	// rozdzielczoœæ linii wzd³u¿	
	r_y = 1;		// gruboœæ po³owy linii interpoluj¹cej - w praktyce oznacza wygenerowanie 2*res_y+1 linii równoleg³ych do g³ównej. Jest to odleg³oœæ od linii g³ównej do najdalszego punktu po³o¿onego na linii prostopad³ej do niej
	interp = 0;		// 0 - NN, 1 - spline
*/
	

	image.ReadBinary("moon.dat");
#ifdef _DEBUG
	dump.AddEntry(&image,"image");
#endif

	cISAR_GetProfile(&image,	// obraz wejciowy
					 200,					// wspó³rzêdna x pierwszego punktu
					 100,					// wspó³rzêdna y pierwszego punktu
					 100,					// wspó³rzêdna x drugiego punktu
					 100,					// wspó³rzêdna y drugiego punktu
					 100,			// iloœæ punktów na odcinek P0P1
					 1,			// po³owa gruboœci linii (jeœli 0 to jednal linia, 2 to trzy linie itd)
					 0,					// metoda: 0 - NN, 1 - spline
					 &mean,	// macierz z profilem jasnoœci
					 &perr);
#ifdef _DEBUG
	dump.AddEntry(&mean,"mean");
#endif	
	return 0;
}

void cISAR_GetProfile(C_Image_Container *p_image,	// obraz wejciowy
					  double x0,					// wspó³rzêdna x pierwszego punktu
					  double y0,					// wspó³rzêdna y pierwszego punktu
					  double x1,					// wspó³rzêdna x drugiego punktu
					  double y1,					// wspó³rzêdna y drugiego punktu
					  unsigned int Nlen,			// iloœæ punktów na odcinek P0P1
					  unsigned int Nszer,			// po³owa gruboœci linii (jeœli 0 to jednal linia, 2 to trzy linie itd)
					  char method,					// metoda: 0 - NN, 1 - spline
					  C_Matrix_Container *p_out,	// macierz z profilem jasnoœci
					  C_Error *p_err)
{
	unsigned int &res_x=Nlen;	// nie chce mi siê popraiwiaæ nazw zmiennych
	C_Matrix_Container line;		// wynik interpolacji
	unsigned int res_y;
	PUNKT P0, P1;		// punkty pocz¹tkowe oraz koñcowe
	float *data;	// obraz w postaci float - do funkcji splajnowej
	int Error;
	unsigned int a,y,x;
	double p;
	PUNKT **tabp;	// tablica 2D zawierajaca wspó³rzêdne punktów le¿¹cych na prostych
	PUNKT *tabL2,*tabL3;	// tablice zawieraj¹ce punkty le¿¹ce na liniach gruboœci L2 i L3
	PUNKT P0L2, P1L2;	// punkty nale¿¹ce do prostej L2 i le¿¹ce w odleg³oœci res_y od punktu P0
	PUNKT P0L3, P1L3;	// punkty nale¿¹ce do prostej L3 i le¿¹ce w odleg³oœci res_y od punktu P1
	PROSTA l;		// prosta ³¹cz¹ca punkt P0 i P1 
	PROSTA l2;		// prosta prostopad³a do l i przechodz¹ca przez P0
	PROSTA l3;		// prosta prostopad³a do l i przechodz¹ca przez P1
	PROSTA ltmp;

	res_y = 2*Nszer+1;
	P0.x = x0; P0.y = y0;
	P1.x = x1; P1.y = y1;

	if(method>1 || method<0)
	{
		p_err->SetError("GetProfile->Unknown Interpolation");
		return;
	}
	/* ----- Sortowanie punktów P0 - lewy, P1 - prawy, góra dó³ ----- */
	SwapPoints(&P0,&P1);
/* ----- konwersja obrazka do wartosci float - wymagane przez funkcjê InterpolatedValue ----- */
	data = new float[p_image->GetNumofElements()];
	for(a=0;a<p_image->GetNumofElements();a++)
		data[a] = (float)p_image->data[a];
	Error = SamplesToCoefficients(data, p_image->_cols, p_image->_rows, 2);
	if(Error)
	{
		p_err->SetError("GetProfile->SamplesToCoefficients->Error");
		return;
	}
	/* ----- Obliczanie wspó³czynników prostej pomiêdzy punktami P0 i P1 ----- */
	getLineCoefs(&P0,&P1,&l);
/* ----- Obliczenie wspó³czynników prostej prostopad³ej l2 do l i przechodzacej przez P0 ----- */
	getLineCoefs(&P0,&l,&l2);
/* ----- Obliczenie wspó³czynników prostej prostopad³ej l3 do l i przechodzacej przez P1 ----- */
	getLineCoefs(&P1,&l,&l3);
/* ----- Obliczenie wsp punktów P0L2 i P1L2 le¿¹cych na prostej l2 w odleg³oœci r_y od niej ----- */
	LineCutsCircle(P0,l2,Nszer,&P0L2,&P1L2);
	SwapPoints(&P0L2,&P1L2);
/* ----- Obliczenie wsp punktów P0L3 i P1L3 le¿¹cych na prostej l3 w odleg³oœci r_y od niej ----- */
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
	/* ----- Allokacja tablicy na punkty oraz obliczanie punktów ----- */
	tabL2 = new PUNKT[res_y];
	tabL3 = new PUNKT[res_y];
	tabp = new PUNKT*[res_y];		// jeœli r_y = 0 to tylko jedna linia
	for(a=0;a<res_y;a++)
		tabp[a] = new PUNKT[res_x];
	if(res_y==1)			// tylko jedna linia i to musi byæ dok³adnie ta œrodkowa
		getPointsonLine(P0,P1,l,res_x,tabp[0]);
	else	// wiêcej linii - generuje punkty na L2 i L3 i dopiero póŸniej wyznaczam równania linii pomiêdzy tymi punktami i punkty do tabp
	{
		getPointsonLine(P0L2,P1L2,l2,res_y,tabL2);
		getPointsonLine(P0L3,P1L3,l3,res_y,tabL3);
		for(a=0;a<res_y;a++)
		{
			SwapPoints(&tabL2[a],&tabL3[a]);
			getLineCoefs(&tabL2[a],&tabL3[a],&ltmp);	// prosta pomiêdzy kolejnymi punktami na prostych L2 i L3
			getPointsonLine(tabL2[a],tabL3[a],ltmp,res_x,tabp[a]);
		}
	}
#ifdef _DEBUG
		DumpAll_Convert(tabp,res_x,res_y,"tabp_x","tabp_y");
		DumpAll_Convert(&tabL2,res_y,1,"tabL2_x","tabL2_y"); 
		DumpAll_Convert(&tabL3,res_y,1,"tabL3_x","tabL3_y");
#endif	
/* ----- Wykonanie interplolacji dla punktów ----- */	
	line.AllocateData(res_y,res_x);
	for(y=0;y<res_y;y++)	// po szerokoœci
		for(x=0;x<res_x;x++)	// po d³ugoœci
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
/* ----- Usrednianie wyników po kolumnach ----- */	
	line.Mean(p_out,0);

	for(a=0;a<res_y;a++)
		delete[] tabp[a];
	delete[] tabp;
	delete[] tabL2;
	delete[] tabL3;
	delete[] data;
}

void getPointsonLine(PUNKT P0,PUNKT P1,PROSTA l,int res_x,PUNKT *out)
{
	/* ----- obliczenie okreslonej iloœci wartoœci w zadanym zakresie <a;b>. Wartoœæ b mo¿e nie zostaæ osi¹gniêta ----- */
	int licz = 0;
	double xtmp,dx;
	if(l.status)	// jeœli prosta pionowa to wszystko po y dla ustalonego x
	{
		xtmp = P0.y;
		dx = (P1.y - P0.y)/(res_x-1);
		while(licz<res_x)
		{
			out[licz].x = P0.x;	// x sie nie zmienia i jest taki sam dla obu punktów
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
	P0 - srodek okrêgu
	l - prosta przechodz¹ca przez P0
	R - promieñ okrêgu
	P1, P2 - wyjœciowy punkty przeciêcia
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
		if(P0->x==P1->x)	// sortowanie o y jeœli s¹ pionowo
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
	dump.AddEntry(&tmpx,nx);
	dump.AddEntry(&tmpy,ny);
}

void getLineCoefs(PUNKT *P1,PROSTA *lp,PROSTA *l)
{
	double a,b;
	BOOL status;
	if(lp->a==0)	// prosta pozioma, czyli prosta l bêdzie pionowa
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

double NNInterp(C_Image_Container *in,double x, double y)
{
	unsigned int xn,yn;
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