/** \mainpage Fillpoly
* \section
* Procedura generuj¹ca równo roz³o¿one punkty wewnatrz figury zdefinowanej przez cztery wierzcho³ki\n
* rocedura testowana fukcj¹ fillpoly_debug.m\n
* \section Versions
* \li <b>VER-1.0</b>	Wersja pocz¹tkowa, zwraca wspó³rzedne punktów mieszcz¹cych siê w zadanym polygonie\n
*/

/**
* @page VER-1.0
* Figura musi byæ zadana tak aby wierzcho³ki by³y w kolejnoœci CW lub CCW. 
*/

#include "Geom.h"
#include <fstream>
#include <iostream>

/// Zwraca wspó³czynniki prostej przechodzacej przez dwa punkty
void DefineLine2Points(PUNKT*,PUNKT*,LINIA*);
/// Generuje liniowo N wartoœci w przedziale <a;b>
void Linspace(double,double,double*,int);
/// Zwraca punkty le¿¹ce na zadanym odcinku
void getSection(PUNKT,PUNKT,LINIA,int,PUNKT*);
/// zwraca punkt przeciêcia prostych
bool LineCutsLine(LINIA*,LINIA*,PUNKT*);
/// Procedura zwracaj¹ca punkty w zadanym polygonie
bool FillPoly(PUNKT*,unsigned int,unsigned int, unsigned int,PUNKT*);

/**
* Zwraca równanie prostej przechodz¹cej przez dwa punkty. Jeœli prosta jest pionowa to 
* ustawiany jest znacznik pion, a wspó³czynnik b ma wartoœæ wspó³rzêdnej x punktów
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
* Generuje liniowo roz³o¿one wartoœci pomiêdzy zadanymi. Wartoœci krañcowe zawieraj¹ siê w zbiorze wyjœciowym.*
* \param[in] x1 Wartoœæ pocz¹kowa
* \param[in] x2 Wartoœæ koñcowa
* \param[out] *out Wektor z wartoœciami wyjœciowymi o rozmiarze N
* \param[in] N Iloœæ punktów pomiêdzy x1 i x2
* \return void
*/
void Linspace( double x1,double x2,double *out,unsigned int N)
{
	int a; // musi byc int zeby sie nie wywala³o
	int Ntmp = (int)N;
	for (a=0;a<=Ntmp-2;a++)
		out[a] = x1 + a*(x2-x1)/(Ntmp-1);
	out[a] = x2;
}

/**
* Zwraca punkty na odcinku P0P1 nale¿¹cym do linii L.
* \param[in] P0 Punkt pocz¹tkowy odcinka
* \param[in] P1 Punkt koñcowy odcinka
* \param[in] L Definicja linii na której lezy odcinek
* \param[in] N Iloœæ punktów razem z P0 i P1
* \param[out] *out Tablica o rozmiarze N z punktami na odcinku
* \return void
* \warning Funkcja nie sprawdza czy punkty PUNKT::P0 i PUNKT::P1 nale¿a do prostej LINIA::L, wa¿ne s¹ tylko koordynaty x tych punktów. Ma to znaczenie
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
			out[a].x = P0.x; /**<Zak³adam ¿e punkty le¿¹ na linii */ 
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
* Zwraca punkt przeciêcia dwóch linii. Jesli linie nie przecinaj¹ siê wtedy P ma wartoœæ nieokreœlon¹ a funkcja zwraca false
* \param[in] L0
* \param[in] L1
* \param[out] *P Punkt przeciêcia siê linii
* \return bool \li true jesli linie siê przecinaj¹
* \li flase jeœli siê nie przecinaj¹ i wtedy PUNKT P ma wartoœæ nieokreslon¹
* \warning Jeœli linie siê pokryj¹ to nie wiadomo co bêdzie
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
* G³ówna procedura zwracaj¹ca punkty wewn¹trz figury\n
* FullName:  FillPoly\n
* Access:    public \n
* G³ówna procedura dokonuj¹ca podzia³u obszaru wyznaczonego przez wierzcho³ki w tablicy p_in na okreœlon¹ iloœæ punktów równomiernie
* roz³o¿onych. 
* \param[in] *p_in Tablica wierzcho³ków
* \param[in] ile_w Rozmiar tablicy p_in
* \param[in] g_szer Iloœæ punktów na szerokosc (liczona jako Pa i PB)
* \param[in] g_wys oraz wysokosc (PB PC) .
* \param[out] *p_out Tablica z punktami roz³o¿onymi na figurze o rozmiarze g*g
* \return bool Zwraca true dla b³êdu i false dla jego braku
* \note Ta wersja kodu zak³¹da istnienie tylko czterech wierzcho³ków w polygonie!! Dla wersji o wiêkszej iloœci mog¹ byæ problemy z kierunkami prostych
*/
bool FillPoly( PUNKT *p_in,unsigned int ile_w, unsigned int g_szer,unsigned int g_wys,PUNKT *p_out)
{
	PUNKT PA, PB, PC, PD; // punkty wierzcho³ków
	LINIA L_AB, L_BC, L_CD, L_DA; // linie krawêdzi
	PUNKT *P_L_AB, *P_L_BC, *P_L_CD, *P_L_DA; // tablice zawieraj¹ce punkty le¿¹ce na krawêdziech - wykorzystane póŸniej do generowania linii równoleg³ych do krawêdzi
	LINIA *L_PION, *L_POZIOM;	// tablice zawieraj¹ce linie pomiêdzy naprzeciwleg³ym œciankami (ich punktami z tablic P_L_XX)
	bool ret;
	unsigned int a,b,licznik;
	PUNKT P;

	PA = p_in[0]; PB = p_in[1]; PC = p_in[2]; PD = p_in[3];
	DefineLine2Points(&PA,&PB,&L_AB);
	DefineLine2Points(&PB,&PC,&L_BC);
	DefineLine2Points(&PD,&PC,&L_CD);
	DefineLine2Points(&PA,&PD,&L_DA); /**< Czemu tu jest na odwrót? Bo naprzeciwleg³e krawêdzie musza byæ w tym samym kierunku*/

	// podzia³ linii na t¹ sam¹ iloœæ punktów
	P_L_AB = new PUNKT[g_szer]; P_L_BC = new PUNKT[g_wys]; P_L_CD = new PUNKT[g_szer]; P_L_DA = new PUNKT[g_wys];
	getSection(PA,PB,L_AB,g_szer,P_L_AB);
	getSection(PB,PC,L_BC,g_wys,P_L_BC);
	getSection(PD,PC,L_CD,g_szer,P_L_CD);
	getSection(PA,PD,L_DA,g_wys,P_L_DA);

	// generowanie naprzeciwleg³ych linii
	L_PION = new LINIA[g_szer];
	L_POZIOM = new LINIA[g_wys];
	for (a=0;a<g_szer;a++)
		DefineLine2Points(P_L_AB+a,P_L_CD+a,&L_PION[a]);
	for (a=0;a<g_wys;a++)
		DefineLine2Points(P_L_BC+a,P_L_DA+a,&L_POZIOM[a]);
		


	// generowanie punktów przeciêcia
	licznik = 0;
	for(a=0;a<g_szer;a++)
		for (b=0;b<g_wys;b++)
		{
			ret = LineCutsLine(L_PION+a,L_POZIOM+b,&P);
			if(!ret)	// jeœli siê nie przetn¹
			{
				std::cerr << "Linie siê nie przecie³y\n";
				return true;
			}
			p_out[licznik++] = P;
		}
		

	delete[] P_L_AB; delete[] P_L_BC; delete[] P_L_CD; delete[] P_L_DA;
	delete[] L_POZIOM; delete[] L_PION;
	return false;
}
