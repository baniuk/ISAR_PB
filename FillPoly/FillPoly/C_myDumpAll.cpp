#include "C_myDumpAll.h"



/**
* FullName:  C_myDumpAll::C_myDumpAll\n
* Access:    public\n
* \param[in] *filename Nazwa pliku wyjœciowego
*/
C_myDumpAll::C_myDumpAll(char* filename):C_DumpAll(filename)
{

}

/**
* Rozszerzenie klasy bazowej C_DumpAll\n
* FullName:  C_myDumpAll::POINTAddEntry\n
* Access:    public\n
* Dodaje funkcjonalnoœæ do klasy C_DumpAll poprzez umo¿liwienie zgrania tablicy punktów PUNKT
* 
* \param[in] *data Tablica punktów
* \param[in] size Rozmiar tablicy punktów
* \param[in] *name Nazwa zmiennej - zostanie rozszerzona o _x i _y
* \return void
* \note W workspace pojawi¹ siê dwie zmienne o nazwie name rozszerzone o _x i _y odpowiednio dla wspó³rzêdnej x i y
*/
void C_myDumpAll::POINTAddEntry( PUNKT* data, unsigned int size, char* name )
{
	unsigned int l;
	double *x, *y;
	std::string _x(name);
	std::string _y(name);
	char *cstr;
	x = new double[size];
	y = new double[size];
	// kopiowanie do tablicy tymczasowej
	for(l=0;l<size;l++)
	{
		x[l] = data[l].x;
		y[l] = data[l].y;
	}
	// nowe nazwy na bazie bazy
	_x += "_X";
	_y += "_Y";
	/** Tworzenie stringów c http://www.cplusplus.com/reference/string/string/c_str/
	*	\code
	* 	cstr = new char[_x.size()+1];
	*	strcpy(cstr, _x.c_str());
	*	C_DumpAll::AddEntry(x,size,cstr);
	*	\endcode
	*/
	cstr = new char[_x.size()+1];
	strcpy(cstr, _x.c_str());
	C_DumpAll::AddEntry(x,size,cstr);
	strcpy(cstr, _y.c_str());
	C_DumpAll::AddEntry(y,size,cstr);


	delete[] x;
	delete[] y;
	delete[] cstr;
}

C_myDumpAll::~C_myDumpAll(void)
{

}