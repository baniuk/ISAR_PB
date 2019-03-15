// fastMedian_Huang.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <C_DumpAll.h>


void FastMedian_Huang(	unsigned short *tabin,
						unsigned short *tabout, 
						unsigned int rows,
						unsigned int cols,
						unsigned short mask);
inline unsigned short getPoint(const unsigned short *tab, unsigned int r, unsigned int k, unsigned int cols);
unsigned short getMedian(const unsigned short *tab, unsigned int size);

int _tmain(int argc, _TCHAR* argv[])
{
	C_Image_Container input;
	C_Matrix_Container MaskRows;
	C_Matrix_Container MaskCols;
	C_Image_Container out;
	C_Error err;

	C_DumpAll dump("test.dat");

	input.ReadBinary("testimag4.dat");
	MaskRows.AllocateData(1,1); MaskRows.SetPixel(0,0,201);
	MaskCols.AllocateData(1,1); MaskCols.SetPixel(0,0,201);

	dump.AddEntry(&input,"in");
	dump.AddEntry(&MaskRows,"maskrows");
	dump.AddEntry(&MaskCols,"maskcols");


//	dump.AddEntry(&out,"out");

	return 0;
}

// ilo�� poziom�w szaro�ci w analizowanym obrazie
#define GRAYSCALE 65536
/** 
 * Filtruje obraz median�
 * Na podstawie Huang, A Fast Two-Dimensional Median Filtering Algorithm 
 * @param[in] tabin		wskaxnik na tablice wej�ciow� z obrazem
 * @param[out] tabout	wska�nik na tablice wyj�ciow� z przefiltrowanym obrazem o rozmiarze tabin
 * @param[in] rows		rz�dy obrazu wej�ciowego
 * @param[in] cols		kolumny obrazu wej�ciowego
 * @param[in] mask		rozmiar maski, maska nieparzysta i kwadratowa
*/
void FastMedian_Huang(	const unsigned short *tabin,
						unsigned short *tabout, 
						unsigned int rows,
						unsigned int cols,
						unsigned short mask)
{
	unsigned int *hist=NULL;	// histogram obszaru filtrowanego
	unsigned short *window = NULL; // dane z okna maski
	unsigned short mdm;			// warto�� mediany w oknie
	unsigned short lmdm;		// liczba pr��k�w histogramu o warto�ciach wi�kszych od mdm
	unsigned short *left_column = NULL;	// lewa kolumna okna
	unsigned short *right_column = NULL;	// prawa kolumna okna
	unsigned int r,k;			// indeksy bierz�cej pozycji okna na obrazie (�rodka)
	unsigned short wr,wk;			// indeksy wewn�trz okna
	unsigned short mask_center = (mask+1)/2;	// �rodek maski (indeks)
	unsigned short bok_maski = (mask-1)/2;	// rozmiar boku maski ca�a maska to 2*bok + 1 
	unsigned int l;	// licznik

	hist = new unsigned int[GRAYSCALE];	// zak�adam g��bi� 16 bit
	left_column = new unsigned short[mask];		// lewa kolumna poprzedniej pozycji maski (maska jest zawsze kwadratowa)
	right_column = new unsigned short[mask];	// prawa kolumna bierzacej maski
	window = new unsigned short[mask*mask];
	/* 
	 * Przegl�danie obrazu po rz�dach a procedura szybkiej filtracji po 
	 * kolumnach. Dla kazdego nowego rz�du powtarza si� wszystko od pocz�tku.
	 */
	for (r = mask_center;r<rows-mask_center-1;r++)	// g��wna p�tla po rz�dach obrazu
	{
		// -------------------- inicjalizacja parametr�w dla ka�dego rz�du --------------------------
		memset(hist, 0, GRAYSCALE*sizeof(unsigned int));	// szybkie zerowanie pami�ci histogramu (dla kazdego rz�du procedura dzia�a od pocz�tku)
		// obliczanie histogramu dla pierwszej pozycji okna w rz�dzie
		for (wr = r-bok_maski-1,l=0;wr<r+bok_maski;wr++)	// r - �rodek okna
			for (wk = mask_center-bok_maski-1;wk<mask_center+bok_maski;wk++) // tu zawsze zaczynamy od pierwszej pozycji okna w kolumnach
			{
				window[l] = getPoint(tabin,wr,wk,cols);	// kopiowanie danych z obrazu do osobnej tablicy reprezentuj�cej okno
				hist[window[l]]++;	// obliczam histogram
				l++;				
			}
		mdm = getMedian(window,mask*mask);	// znajduj� median� z okna

		for (k = mask_center+1;k<cols-mask_center-1;k++)	// g��wna p�tla po kolumnach obrazu, dla pierwszej pozycji k=mask_center obliczane jest osobno
		{
			// obliczanie histogramu dla ka�dej pozycji okna w rz�dzie
		}



	}

	SAFE_DELETE(hist);
	SAFE_DELETE(left_column);
	SAFE_DELETE(right_column);
	SAFE_DELETE(window);
}

/** 
 * Zwraca warto�� z tablicy na pozycji [r,k], przy za�ozeniu �e tablica jest
 * interpretowana jako 2D
 * @param[in] tab		wskaxnik na tablice wej�ciow� z obrazem
 * @param[in] r			rz�d
 * @param[in] k			kolumna
 * @param[in] cols		ilo�� kolumn w tablicy
 * \return warto�� na pozycji tab[r,k]
 * \warning funkcja nie posiada zabezpiecze� przeciw przekroczeniu tablicy
 * \remarks W tablicy tab dane uk�adane sa rzedami, zgodnie z C_MatrixContainer
*/
inline unsigned short getPoint(const unsigned short *tab, unsigned int r, unsigned int k, unsigned int cols)
{
	return tab[r*cols+k];
}

/** 
 * Zwraca median� z okna
 * @param[in] tab		tablica z oknem
 * @param[in] size			rozmiar tablicy
 * \return warto�� mediany
*/
unsigned short getMedian(const unsigned short *tab, unsigned int size)
{
	std::vector<unsigned int> v( tab, tab+size );
	std::nth_element( v.begin(), v.begin()+size/2,v.end() );
	return v[size/2];
}