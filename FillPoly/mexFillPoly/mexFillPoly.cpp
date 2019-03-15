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

/// Definicja eksportu. Zak�adam podej�cie bez pliku def. Eksport z kodu �r�d�owego za pomoc� makra MS
#define DllExport __declspec( dllexport )

/// Makro do kasowania tablic
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

/// Funkcja wej�ciowa Matlaba
/** 
 \brief G��wna funkcja eksportowa, zgodna z dokumentacj� Matlaba
 * @param[in] nlhs		ilo�� parametr�w wyj�ciowych przekazanych z Matlaba
 * @param[in] plhs		wska�nik na parametry wyj�ciowe
 * @param[in] nrhs		ilo�� parametr�w wej�ciowych przekazanych z Matlaba
 * @param[in] prhs		wska�nik na parametry wej�ciowe
*/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	/** 
	 * Parametry przekazywane z Matlaba to:\n
	 * \li Tablica punkt�w do sprawdzenia w formacie xp, yp (osobno)\n
	 * \li Ilo�� punkt�w na kraw�d�. Ca�kowita ilo�� punkt�w zwr�conych to N^2\n
	 * <b> Sk�adnia wywo�ania w MAtlabie:</b>\n
	 * \code
	 * out = FillPoly(xp,yp,N)
	 * \endcode
	 * \return	Tablica [Nx2] ze wsp�rz�dnymi punkt�w kt�re le�� wewn�trz figury
	 * \warning Wierzcho�ki musza by� w okre�lonej kolejno�ci CW lub CCW. NA chwil� obecn� tylko 4 sa wspierane. Wsp�rz�dne punkt�w musz� by� w kolumnach\n
	*/
	
		double *p_xp, *p_yp, *p_out;	// wska�niki na dane z matlaba
		unsigned int licznik, p, Nszer,Nwys; // Nszer - ilosc punkt�w pomi�dzy punktami krawedzi g��wne P0 P1, Nwys - ilosc punkt�w pomi�dzy punktem kraw�dzi g��wnej a punktem po przesunieciu do srodka lub na zewnatrz
		unsigned int mrows_xp, ncols_xp;
		PUNKT *pIn, *pOut;
		bool ret;
	
		p_xp = mxGetPr(prhs[0]);
		p_yp = mxGetPr(prhs[1]);
		Nszer = (unsigned int)mxGetScalar(prhs[2]);
		Nwys = (unsigned int)mxGetScalar(prhs[3]);
	
		// zak�adam �e dane s� sp�jne i maj� odpowiednie rozmiary, czyli wektory a nie tablice
		mrows_xp = (unsigned int)mxGetM(prhs[0]);
		ncols_xp = (unsigned int)mxGetN(prhs[0]);
	
		pIn = new PUNKT[mrows_xp];
		pOut = new PUNKT[Nszer*Nwys];

		for(p=0;p<mrows_xp;p++)
		{
			pIn[p].x = p_xp[p];
			pIn[p].y = p_yp[p];
		}
	
		ret = FillPoly(pIn,4,Nszer,Nwys,pOut); // w pOut punkty s� pouk�adane po wysoko�ci
		if (ret)
		{
			/// \todo Obs�uga b��d�w tutaj
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