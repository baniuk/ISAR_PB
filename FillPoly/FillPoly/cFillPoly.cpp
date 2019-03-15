/** \mainpage Fillpoly
* \section
* Procedura generuj�ca r�wno roz�o�one punkty wewnatrz figury zdefinowanej przez cztery wierzcho�ki\n
* rocedura testowana fukcj� fillpoly_debug.m\n
* \section Versions
* \li <b>VER-1.0</b>	Wersja pocz�tkowa, zwraca wsp�rzedne punkt�w mieszcz�cych si� w zadanym polygonie\n
*/

/**
* @page VER-1.0
* Figura musi by� zadana tak aby wierzcho�ki by�y w kolejno�ci CW lub CCW. 
*/

#include "Geom.h"
#include <fstream>
#include <iostream>

/// Zwraca wsp�czynniki prostej przechodzacej przez dwa punkty
void DefineLine2Points(PUNKT*,PUNKT*,LINIA*);
/// Generuje liniowo N warto�ci w przedziale <a;b>
void Linspace(double,double,double*,int);
/// Zwraca punkty le��ce na zadanym odcinku
void getSection(PUNKT,PUNKT,LINIA,int,PUNKT*);
/// zwraca punkt przeci�cia prostych
bool LineCutsLine(LINIA*,LINIA*,PUNKT*);
/// Procedura zwracaj�ca punkty w zadanym polygonie
bool FillPoly(PUNKT*,unsigned int,unsigned int, unsigned int,PUNKT*);

/**
* Zwraca r�wnanie prostej przechodz�cej przez dwa punkty. Je�li prosta jest pionowa to 
* ustawiany jest znacznik pion, a wsp�czynnik b ma warto�� wsp�rz�dnej x punkt�w
* \param[in] *P1
* \param[in] *P2
* \param[out] *L
* \return void
*/
void DefineLine2Points( PUNKT *P1,PUNKT *P2,LINIA *L)
{
	double a,b;
	bool status;
	if (P2->x - P1->x == 0)
	{
		a = b = P1->x;
		status = true; // prosta pionowa
	} else
	{
		a = (P2->y - P1->y)/(P2->x - P1->x);
		b = P2->y - a*P2->x;
		status = false;
	}
	L->a = a;
	L->b = b;
	L->pion = status;
}

/**
* Generuje liniowo roz�o�one warto�ci pomi�dzy zadanymi. Warto�ci kra�cowe zawieraj� si� w zbiorze wyj�ciowym.*
* \param[in] x1 Warto�� pocz�kowa
* \param[in] x2 Warto�� ko�cowa
* \param[out] *out Wektor z warto�ciami wyj�ciowymi o rozmiarze N
* \param[in] N Ilo�� punkt�w pomi�dzy x1 i x2
* \return void
*/
void Linspace( double x1,double x2,double *out,unsigned int N)
{
	int a; // musi byc int zeby sie nie wywala�o
	int Ntmp = (int)N;
	for (a=0;a<=Ntmp-2;a++)
		out[a] = x1 + a*(x2-x1)/(Ntmp-1);
	out[a] = x2;
}

/**
* Zwraca punkty na odcinku P0P1 nale��cym do linii L.
* \param[in] P0 Punkt pocz�tkowy odcinka
* \param[in] P1 Punkt ko�cowy odcinka
* \param[in] L Definicja linii na kt�rej lezy odcinek
* \param[in] N Ilo�� punkt�w razem z P0 i P1
* \param[out] *out Tablica o rozmiarze N z punktami na odcinku
* \return void
* \warning Funkcja nie sprawdza czy punkty PUNKT::P0 i PUNKT::P1 nale�a do prostej LINIA::L, wa�ne s� tylko koordynaty x tych punkt�w. Ma to znaczenie
* Tylko w przypadku linii pionowej
*/
void getSection( PUNKT P0,PUNKT P1,LINIA L,unsigned int N,PUNKT *out)
{
	unsigned int a;
	double *px = new double[N];	// tablica na punkty z linspace
	if (L.pion)
	{// Pionowa
		Linspace(P0.y,P1.y,px,N);
		for(a=0;a<N;a++) 
		{
			out[a].x = P0.x; /**<Zak�adam �e punkty le�� na linii */ 
			out[a].y = px[a];
		}		
	} 
	else
	{// Inna
		Linspace(P0.x,P1.x,px,N);
		for (a=0;a<N;a++)
		{
			out[a].x = px[a];
			out[a].y = L.a*px[a]+L.b;
		}
	}
	delete[] px;
}

/**
* FullName:  LineCutsLine\n
* Access:    public \n
* Zwraca punkt przeci�cia dw�ch linii. Jesli linie nie przecinaj� si� wtedy P ma warto�� nieokre�lon� a funkcja zwraca false
* \param[in] L0
* \param[in] L1
* \param[out] *P Punkt przeci�cia si� linii
* \return bool \li true jesli linie si� przecinaj�
* \li flase je�li si� nie przecinaj� i wtedy PUNKT P ma warto�� nieokreslon�
* \warning Je�li linie si� pokryj� to nie wiadomo co b�dzie
*/
bool LineCutsLine( LINIA *L0,LINIA *L1,PUNKT *P)
{
	double xtmp=0,ytmp=0;
	if (L0->pion && L1->pion) // obie pionowe
		return false;
	if (L0->pion && !L1->pion) // pierwsza pionowa druga nie
	{
		xtmp = L0->b;
		ytmp = L1->a*xtmp+L1->b;
	}
	if (!L0->pion && L1->pion)	// druga pionowa, piersza normalna
	{
		xtmp = L1->b;
		ytmp = L0->a*xtmp+L0->b;
	}
	if (!L0->pion && !L1->pion)	// obie niepionowe
	{
		xtmp = (L1->b - L0->b)/(L0->a - L1->a);
		ytmp = L0->a*xtmp+L0->b;
	}
	P->x = xtmp;
	P->y = ytmp;
	return true;
}

/**
* G��wna procedura zwracaj�ca punkty wewn�trz figury\n
* FullName:  FillPoly\n
* Access:    public \n
* G��wna procedura dokonuj�ca podzia�u obszaru wyznaczonego przez wierzcho�ki w tablicy p_in na okre�lon� ilo�� punkt�w r�wnomiernie
* roz�o�onych. 
* \param[in] *p_in Tablica wierzcho�k�w
* \param[in] ile_w Rozmiar tablicy p_in
* \param[in] g_szer Ilo�� punkt�w na szerokosc (liczona jako Pa i PB)
* \param[in] g_wys oraz wysokosc (PB PC) .
* \param[out] *p_out Tablica z punktami roz�o�onymi na figurze o rozmiarze g*g
* \return bool Zwraca true dla b��du i false dla jego braku
* \note Ta wersja kodu zak��da istnienie tylko czterech wierzcho�k�w w polygonie!! Dla wersji o wi�kszej ilo�ci mog� by� problemy z kierunkami prostych
*/
bool FillPoly( PUNKT *p_in,unsigned int ile_w, unsigned int g_szer,unsigned int g_wys,PUNKT *p_out)
{
	PUNKT PA, PB, PC, PD; // punkty wierzcho�k�w
	LINIA L_AB, L_BC, L_CD, L_DA; // linie kraw�dzi
	PUNKT *P_L_AB, *P_L_BC, *P_L_CD, *P_L_DA; // tablice zawieraj�ce punkty le��ce na kraw�dziech - wykorzystane p�niej do generowania linii r�wnoleg�ych do kraw�dzi
	LINIA *L_PION, *L_POZIOM;	// tablice zawieraj�ce linie pomi�dzy naprzeciwleg�ym �ciankami (ich punktami z tablic P_L_XX)
	bool ret;
	unsigned int a,b,licznik;
	PUNKT P;

	PA = p_in[0]; PB = p_in[1]; PC = p_in[2]; PD = p_in[3];
	DefineLine2Points(&PA,&PB,&L_AB);
	DefineLine2Points(&PB,&PC,&L_BC);
	DefineLine2Points(&PD,&PC,&L_CD);
	DefineLine2Points(&PA,&PD,&L_DA); /**< Czemu tu jest na odwr�t? Bo naprzeciwleg�e kraw�dzie musza by� w tym samym kierunku*/

	// podzia� linii na t� sam� ilo�� punkt�w
	P_L_AB = new PUNKT[g_szer]; P_L_BC = new PUNKT[g_wys]; P_L_CD = new PUNKT[g_szer]; P_L_DA = new PUNKT[g_wys];
	getSection(PA,PB,L_AB,g_szer,P_L_AB);
	getSection(PB,PC,L_BC,g_wys,P_L_BC);
	getSection(PD,PC,L_CD,g_szer,P_L_CD);
	getSection(PA,PD,L_DA,g_wys,P_L_DA);

	// generowanie naprzeciwleg�ych linii
	L_PION = new LINIA[g_szer];
	L_POZIOM = new LINIA[g_wys];
	for (a=0;a<g_szer;a++)
		DefineLine2Points(P_L_AB+a,P_L_CD+a,&L_PION[a]);
	for (a=0;a<g_wys;a++)
		DefineLine2Points(P_L_BC+a,P_L_DA+a,&L_POZIOM[a]);
		


	// generowanie punkt�w przeci�cia
	licznik = 0;
	for(a=0;a<g_szer;a++)
		for (b=0;b<g_wys;b++)
		{
			ret = LineCutsLine(L_PION+a,L_POZIOM+b,&P);
			if(!ret)	// je�li si� nie przetn�
			{
				std::cerr << "Linie si� nie przecie�y\n";
				return true;
			}
			p_out[licznik++] = P;
		}
		

	delete[] P_L_AB; delete[] P_L_BC; delete[] P_L_CD; delete[] P_L_DA;
	delete[] L_POZIOM; delete[] L_PION;
	return false;
}
