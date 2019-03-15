/********************************************************************
	created:	2011/11/28
	created:	28:11:2011   9:32
	filename: 	c:\Users\asus\documents\visual studio 2010\Projects\FillPoly\FillPoly\FillPoly.cpp
	file path:	c:\Users\asus\documents\visual studio 2010\Projects\FillPoly\FillPoly
	file base:	FillPoly
	file ext:	cpp
	author:		Piotr Baniukiewicz
	
	purpose:	Plik testuj¹cy procedury z cFillPoly. Wspó³pracuje z fillpoly_debug.m
*********************************************************************/

#include "cFillPoly.cpp"
#include "C_myDumpAll.h"

#ifdef _DEBUG 
C_myDumpAll dump("debug.out");
#endif

int main()
{
	PUNKT P0;
	PUNKT P1;
	PUNKT P2;
	PUNKT P3;

	// kwadrat o boku 10;10
	P0.x = 0; P0.y = 0;
	P1.x = 10; P1.y = 0;
	P2.x = 10; P2.y = 10;
	P3.x = 0; P3.y = 10;

	// test linspace
	unsigned int N = 100;
	double *out = new double[N];
	Linspace(P0.x,P1.x,out,N);
	dump.AddEntry(out,N,"out_linspace"); //test ok
	delete[] out;

	N = 2;
	out = new double[N];
	Linspace(P0.x,P1.x,out,N);
	dump.AddEntry(out,N,"out_linspace1"); // test ok
	delete[] out;

	N = 1;
	out = new double[N];
	Linspace(P0.x,P1.x,out,N);
	dump.AddEntry(out,N,"out_linspace2"); //test ok
	delete[] out;

	// test defineline2points
	{
		LINIA L1;
		DefineLine2Points(&P0,&P1,&L1);
		dump.AddEntry(&L1.a,1,"L1a");
		dump.AddEntry(&L1.b,1,"L1b");
		double b = (double)L1.pion;
		dump.AddEntry(&b,1,"L1pion"); //ok
	}
	{
		LINIA L2;
		DefineLine2Points(&P1,&P2,&L2);
		dump.AddEntry(&L2.a,1,"L2a");
		dump.AddEntry(&L2.b,1,"L2b");
		double b = (double)L2.pion;
		dump.AddEntry(&b,1,"L2pion");//ok
	}
	{
		LINIA L3;
		DefineLine2Points(&P0,&P2,&L3);
		dump.AddEntry(&L3.a,1,"L3a");
		dump.AddEntry(&L3.b,1,"L3b");
		double b = (double)L3.pion;
		dump.AddEntry(&b,1,"L3pion");//ok
	}

	// test getsection
	{
		unsigned int N = 10;
		PUNKT *out = new PUNKT[N];
		LINIA L1;
		DefineLine2Points(&P0,&P1,&L1);
		getSection(P0,P1,L1,N,out);
		dump.POINTAddEntry(out,N,"S1");//ok
		delete[] out;
	}
	{
		unsigned int N = 20;
		PUNKT *out = new PUNKT[N];
		LINIA L3;
		DefineLine2Points(&P0,&P2,&L3);
		getSection(P0,P1,L3,N,out);
		dump.POINTAddEntry(out,N,"S3");//ok
		delete[] out;
	}

	// test linecutsline
	{
		LINIA L0,L1;
		PUNKT P;
		DefineLine2Points(&P0,&P1,&L0);
		DefineLine2Points(&P1,&P2,&L1);
		LineCutsLine(&L0,&L1,&P);
		dump.POINTAddEntry(&P,1,"P1cut");//ok
	}
	{
		LINIA L0,L1;
		PUNKT P;
		DefineLine2Points(&P0,&P1,&L0);
		DefineLine2Points(&P0,&P2,&L1);
		LineCutsLine(&L0,&L1,&P);
		dump.POINTAddEntry(&P,1,"P2cut");//ok
	}

	// test fillpoly
	{
		PUNKT P_IN[4];
		P_IN[0] = P0; P_IN[1] = P1; P_IN[2] = P2; P_IN[3] = P3;
		unsigned int g = 10;
		PUNKT *P_OUT = new PUNKT[g*g];
		FillPoly(P_IN,4,g,P_OUT);
		dump.POINTAddEntry(P_OUT,g*g,"P_OUT"); //ok
		delete[] P_OUT;
	}
	// skoœny
	P0.x = 0; P0.y = 0;
	P1.x = 10; P1.y = 0;
	P2.x = 15; P2.y = 10;
	P3.x = 5; P3.y = 10;
	{
		PUNKT P_IN[4];
		P_IN[0] = P0; P_IN[1] = P1; P_IN[2] = P2; P_IN[3] = P3;
		unsigned int g = 30;
		PUNKT *P_OUT = new PUNKT[g*g];
		FillPoly(P_IN,4,g,P_OUT);
		dump.POINTAddEntry(P_OUT,g*g,"P1_OUT"); //ok
		delete[] P_OUT;
	}


	return 0;
}

