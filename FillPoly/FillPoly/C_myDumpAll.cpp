#include "C_myDumpAll.h"



/**
* FullName:  C_myDumpAll::C_myDumpAll\n
* Access:    public\n
* \param[in] *filename Nazwa pliku wyj�ciowego
*/
C_myDumpAll::C_myDumpAll(char* filename):C_DumpAll(filename)
{

}

/**
* Rozszerzenie klasy bazowej C_DumpAll\n
* FullName:  C_myDumpAll::POINTAddEntry\n
* Access:    public\n
* Dodaje funkcjonalno�� do klasy C_DumpAll poprzez umo�liwienie zgrania tablicy punkt�w PUNKT
* 
* \param[in] *data Tablica punkt�w
* \param[in] size Rozmiar tablicy punkt�w
* \param[in] *name Nazwa zmiennej - zostanie rozszerzona o _x i _y
* \return void
* \note W workspace pojawi� si� dwie zmienne o nazwie name rozszerzone o _x i _y odpowiednio dla wsp�rz�dnej x i y
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
	/** Tworzenie string�w c http://www.cplusplus.com/reference/string/string/c_str/
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