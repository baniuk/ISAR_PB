/********************************************************************
	created:	2011/11/28
	created:	28:11:2011   13:03
	filename: 	c:\Users\asus\documents\visual studio 2010\Projects\FillPoly\FillPoly\C_myDumpAll.h
	file path:	c:\Users\asus\documents\visual studio 2010\Projects\FillPoly\FillPoly
	file base:	C_myDumpAll
	file ext:	h
	author:		Piotr Baniukiewicz
	
	purpose:	Klasa dziedziczona c C_DumpAll aby do�o�y� nowe ,funkcjinalno�ci takie jak zgrywanie tablic punkt�w
*********************************************************************/
#pragma once
#include "C_DumpAll.h"
#include "Geom.h"
#include "iostream"
#include <string.h>


class C_myDumpAll :
	public C_DumpAll
{
public:
	/// Konstruktor inicjalizuj�cy obie klasy
	C_myDumpAll(char* filename);
	/// Zgrywa tablice struktur PUNKT
	void POINTAddEntry(PUNKT* data, unsigned int size, char* name);
	virtual ~C_myDumpAll(void);

};