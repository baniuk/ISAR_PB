/********************************************************************
	created:	2011/11/28
	created:	28:11:2011   13:23
	filename: 	c:\Users\asus\documents\visual studio 2010\Projects\FillPoly\FillPoly\Geom.h
	file path:	c:\Users\asus\documents\visual studio 2010\Projects\FillPoly\FillPoly
	file base:	Geom
	file ext:	h
	author:		Piotr Baniukiewicz
	
	purpose:	Plik zawieraj�cy definicje klas geometrycznych
*********************************************************************/
#pragma once

/// Struktura opisuj�ca punkt
struct PUNKT {
	double x;
	double y;
};

/// struktura opisuj�ca prost� w przestrzeni
struct LINIA {
	double a; /**< wsp�czynnik kierunkowy*/
	double b; /**< wyraz wolny*/
	bool pion; /**< Przyjmuje <b>TRUE</b> je�li prosta jest pionowa*/
};

