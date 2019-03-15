/********************************************************************
	created:	2011/11/28
	created:	28:11:2011   23:09
	filename: 	C:\Users\asus\Documents\Visual Studio 2010\Projects\FillPoly\mexFillPoly\mexFillPoly.cpp
	file path:	C:\Users\asus\Documents\Visual Studio 2010\Projects\FillPoly\mexFillPoly
	file base:	mexFillPoly
	file ext:	cpp
	author:		Piotr Baniukiewicz
	
	purpose:	testy wykoanane w FillPoly.cpp
*********************************************************************/

#include <mex.h>
#include <cFillPoly.cpp>

/// Definicja eksportu. Zak³adam podejœcie bez pliku def. Eksport z kodu Ÿród³owego za pomoc¹ makra MS
#define DllExport __declspec( dllexport )

/// Makro do kasowania tablic
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

/// Funkcja wejœciowa Matlaba
/** 
 \brief G³ówna funkcja eksportowa, zgodna z dokumentacj¹ Matlaba
 * @param[in] nlhs		iloœæ parametrów wyjœciowych przekazanych z Matlaba
 * @param[in] plhs		wskaŸnik na parametry wyjœciowe
 * @param[in] nrhs		iloœæ parametrów wejœciowych przekazanych z Matlaba
 * @param[in] prhs		wskaŸnik na parametry wejœciowe
*/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	/** 
	 * Parametry przekazywane z Matlaba to:\n
	 * \li Tablica punktów do sprawdzenia w formacie xp, yp (osobno)\n
	 * \li Iloœæ punktów na krawêdŸ. Ca³kowita iloœæ punktów zwróconych to N^2\n
	 * <b> Sk³adnia wywo³ania w MAtlabie:</b>\n
	 * \code
	 * out = FillPoly(xp,yp,N)
	 * \endcode
	 * \return	Tablica [Nx2] ze wspó³rzêdnymi punktów które le¿¹ wewn¹trz figury
	 * \warning Wierzcho³ki musza byæ w okreœlonej kolejnoœci CW lub CCW. NA chwilê obecn¹ tylko 4 sa wspierane. Wspó³rzêdne punktów musz¹ byæ w kolumnach\n
	*/
	
		double *p_xp, *p_yp, *p_out;	// wskaŸniki na dane z matlaba
		unsigned int licznik, p, Nszer,Nwys; // Nszer - ilosc punktów pomiêdzy punktami krawedzi g³ówne P0 P1, Nwys - ilosc punktów pomiêdzy punktem krawêdzi g³ównej a punktem po przesunieciu do srodka lub na zewnatrz
		unsigned int mrows_xp, ncols_xp;
		PUNKT *pIn, *pOut;
		bool ret;
	
		p_xp = mxGetPr(prhs[0]);
		p_yp = mxGetPr(prhs[1]);
		Nszer = (unsigned int)mxGetScalar(prhs[2]);
		Nwys = (unsigned int)mxGetScalar(prhs[3]);
	
		// zak³adam ¿e dane s¹ spójne i maj¹ odpowiednie rozmiary, czyli wektory a nie tablice
		mrows_xp = (unsigned int)mxGetM(prhs[0]);
		ncols_xp = (unsigned int)mxGetN(prhs[0]);
	
		pIn = new PUNKT[mrows_xp];
		pOut = new PUNKT[Nszer*Nwys];

		for(p=0;p<mrows_xp;p++)
		{
			pIn[p].x = p_xp[p];
			pIn[p].y = p_yp[p];
		}
	
		ret = FillPoly(pIn,4,Nszer,Nwys,pOut); // w pOut punkty s¹ pouk³adane po wysokoœci
		if (ret)
		{
			/// \todo Obs³uga b³êdów tutaj
			return;
		}
		plhs[0] = mxCreateDoubleMatrix(2,Nszer*Nwys, mxREAL);
		p_out = mxGetPr(plhs[0]);
		for(p=0,licznik=0;p<Nszer*Nwys*2;p+=2,licznik++)
		{
			_ASSERT(p<Nszer*Nwys*2);
			p_out[p] = pOut[licznik].x;
			_ASSERT(p+1<Nszer*Nwys*2);
			p_out[p+1] = pOut[licznik].y;
		}
	
		SAFE_DELETE_ARRAY(pIn);
		SAFE_DELETE_ARRAY(pOut);
}