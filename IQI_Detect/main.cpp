#pragma once
#include "mex.h"
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"
#include "C_DumpAll.h"

#include	"coeff.h"
#include	"interpol.h"

C_DumpAll DumpAll("iqi.out");
// znajduje preciki na obrzie, przy za³ozeniu ¿e podaje siê tylko wycinek spawu
// spaw - wycinek zawierajacy spaw
// parametry - wspó³rzêdne œrodka spawu itp
// out - wspó³rzêdne prostok¹ta w którym znajduj¹siê prêciki
// mode - tryb rozpoznawania
//	0 - tylko box z prêcikami na wyœjciu
// parametry[0] - srodek - wsp x
// filter_coef - wspo³czynniki licznika filtru wygenerowanego w MAtlabie (wczytywane w funkcji)
// size - rozmiar wzorca w mm
// res - iloœæ pixeli na mm
// pattern - uœredniony profil wzorca na podstawie wielu zdjêæ (wczytywane w funkcji)
void cISAR_IQI_Detect(C_Image_Container& spaw,
				C_Matrix_Container& parametry, 
				char mode,
				double size,
				double res,
				C_Matrix_Container& out,
				C_Error& perr);

// robi sredni¹ z linii w zakresie sredek-ile:srodek+ile wokó³ œrodek
bool AverageLine(C_Image_Container* spaw,unsigned int srodek,int ile,C_Matrix_Container* out);
void LinearRegresion(double* in,unsigned int ile,double& a, double& b);	// robi regresjê i zwraca wspó³czynniki prostej
void LinearDetrend(double* in,double* pattern,unsigned int ile);	// robi detrend danych i umieszcza je na wejœciu
void Scale01(double* in,unsigned int ile);	// sklauje
double corrcoef(double* x, double* y, double meanx, double meany, double stdx, double stdy, unsigned int ile);	// http://en.wikipedia.org/wiki/Correlation
inline double std(double* x,double srednia_z_x,unsigned int ile);
inline double mean(double* x,unsigned int ile);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs,
                 const mxArray *prhs[])
{
	double *p_spaw,*p_parametry;
	char mode;
	double size;
	double res;
	C_Image_Container spaw;
	C_Matrix_Container parametry;
	C_Matrix_Container filter_coef;
	C_Matrix_Container pattern;
	C_Matrix_Container out;
	C_Error err;

	p_spaw = mxGetPr(prhs[0]);
	p_parametry = mxGetPr(prhs[1]);
	mode = (char)mxGetScalar(prhs[2]);
	size = mxGetScalar(prhs[3]);
	res = mxGetScalar(prhs[4]);

	spaw.ImportFromMatlab(p_spaw,(unsigned int)mxGetM(prhs[0]),(unsigned int)mxGetN(prhs[0]));
	parametry.ImportFromMatlab(p_parametry,(unsigned int)mxGetM(prhs[1]),(unsigned int)mxGetN(prhs[1]));
//	filter_coef.ImportFromMatlab(p_filter_coef,(unsigned int)mxGetM(prhs[2]),(unsigned int)mxGetN(prhs[2]));
//	pattern.ImportFromMatlab(p_pattern,(unsigned int)mxGetM(prhs[3]),(unsigned int)mxGetN(prhs[3]));



#ifdef _DEBUG 
	DumpAll.AddEntry(&spaw,"spaw");	DumpAll.AddEntry(&parametry,"parametry");
#endif
	
	cISAR_IQI_Detect(spaw,parametry, mode, size, res, out,err);
	if(err.status)	{
		mexPrintf(err.error);
		plhs[0] = mxCreateDoubleScalar(-1);
		return;
	}

	
	plhs[0] = mxCreateDoubleScalar(out.data[0]);
	

//	plhs[0] = mxCreateDoubleScalar(SNR);
}

void cISAR_IQI_Detect(C_Image_Container& spaw,
				C_Matrix_Container& parametry, 
				char mode,
				double size,
				double res,
				C_Matrix_Container& out,
				C_Error& perr)
{

	C_Matrix_Container line;
	C_Matrix_Container filter_coef;
	C_Matrix_Container pattern;
	float* line_filt;
	float* fpattern; 
	double* in;
	double* pattern_interp;
	double* corr_res;
	unsigned int a,b,bb,licz,poz;
	unsigned int licznik;
	double *tmp;	// do filtracji
	double b1, delta, min_cor;
	double meanwzorca,stdwzorca,meanin,std_in;
	int Error;
	BOOL blad;
	unsigned int size_in_pix = (unsigned int)floor(size*res);	// rozmiar w pixelach
	// wczytanie danych

	blad = filter_coef.ReadBinary("numerator.dat");
	if(blad==FALSE)	{
		perr.SetError("File numerator.dat not found");
		return;
	}
	pattern.ReadBinary("pattern.dat");
	if(blad==FALSE)	{
		perr.SetError("File pattern.dat not found");
		return;
	}

#ifdef _DEBUG 
	DumpAll.AddEntry(&pattern,"pattern");	DumpAll.AddEntry(&filter_coef,"filter_coef");
#endif

	switch(mode)
	{
	case 0:
		tmp = new double[filter_coef.GetNumofElements()];
		if(AverageLine(&spaw,(unsigned int)parametry.data[0],50,&line)) {	perr.SetError("IQI_Detect->AverageLine->Error"); return; } //inicjalizacja line
		line_filt = new float[line.GetNumofElements()];
		fpattern = new float[pattern.GetNumofElements()];
		#ifdef _DEBUG
		DumpAll.AddEntry(&line,"line");
		#endif
//--------------------- filtracja START -------------------------------------------------------
		for(a=filter_coef.GetNumofElements();a<line.GetNumofElements();a++)
		{
			licz = 1;
			tmp[0] = 1;
			poz = a-filter_coef.GetNumofElements();
			do
			{
				tmp[licz++] = line.data[poz++];
			} while(licz<filter_coef.GetNumofElements());
			line_filt[a] = 0;
			for(b=0;b<filter_coef.GetNumofElements();b++)
				line_filt[a]+=(float)(filter_coef.data[b]*tmp[b]);
		}
		#ifdef _DEBUG
		DumpAll.AddEntry(line_filt,line.GetNumofElements(),"line_filt");
		#endif
//--------------------- filtracja END -------------------------------------------------------
//--------------------- skalowanie wzorca START ---------------------------------------------
		for(b=0;b<pattern.GetNumofElements();b++)	// skalowanie do float
			fpattern[b] = (float)pattern.data[b];
		Error = SamplesToCoefficients(fpattern, pattern.GetNumofElements(), 1, 2);
		#ifdef _DEBUG
//		DumpAll.AddEntry(fpattern,pattern.GetNumofElements(),"fpattern");
		#endif
		if(Error)
		{
			perr.SetError("IQI_Detect->SamplesToCoefficients->Error");
			delete[] tmp;
			delete[] fpattern;
			delete[] line_filt;
			return;
		}
//	size_in_pix					- rozmiar tego czego szukamy w pix
//	pattern.GetNumofElements()	- rozmiar tego co mamy w pix 
		pattern_interp = new double[size_in_pix+1];	// 1 na wszelki wypadek
		licz = 0;
		delta = (double)pattern.GetNumofElements()/(double)size_in_pix;	// krok
		for(b1=0;b1<(double)pattern.GetNumofElements();b1+=delta)
		{
			_ASSERT(licz<size_in_pix+1);
			pattern_interp[licz] = InterpolatedValue(fpattern,
											pattern.GetNumofElements(),
											1,
											b1,
											1,
											2);
			licz++;	// liczymy ile rzeczywistych punktów mamy (ze wzgledu na double)
		}
		--licz;		// rozmiar wzorca
		#ifdef _DEBUG
		DumpAll.AddEntry(pattern_interp,licz,"pattern_interp");
		#endif
		Scale01(pattern_interp,licz);	// przeskalowany do 01
		#ifdef _DEBUG
		DumpAll.AddEntry(pattern_interp,licz,"pattern_interp01");
		#endif 
//--------------------- skalowanie wzorca END ---------------------------------------------
		in = new double[licz];
		corr_res = new double[line.GetNumofElements()-licz];
		meanwzorca = mean(pattern_interp,licz);
		stdwzorca = std(pattern_interp,meanwzorca,licz);
		for(b=filter_coef.GetNumofElements();b<line.GetNumofElements()-licz;b++)
		{
			licznik = 0;
			for(bb=b;bb<b+licz;bb++) {
				_ASSERT(bb<line.GetNumofElements());
				_ASSERT(licznik<licz);
				in[licznik++] = line_filt[bb];
			}
			Scale01(in,licz);
//			#ifdef _DEBUG
//			DumpAll.AddEntry(in,licz,"in_scaled");
//			#endif
			LinearDetrend(in,pattern_interp,licz); Scale01(in,licz);
			meanin = mean(in,licz);
			std_in = std(in,meanin,licz);
//			#ifdef _DEBUG
//			DumpAll.AddEntry(in,licz,"in_scaled_detrend");
//			DumpAll.AddEntry(&meanin,1,"meanin");
//			DumpAll.AddEntry(&std_in,1,"std_in");
//			double b1 = b;
//			DumpAll.AddEntry(&b1,1,"b");
//			#endif
			corr_res[b] = corrcoef(in, pattern_interp, meanin, meanwzorca, std_in, stdwzorca, licz);
//break;
		}
		#ifdef _DEBUG
		DumpAll.AddEntry(corr_res,line.GetNumofElements()-licz,"corr_res");
		#endif

// szukanie min korelacji
		out.AllocateData(1,1);
		min_cor = -1e306;
		for(b=filter_coef.GetNumofElements();b<line.GetNumofElements()-licz;b++)
			if(corr_res[b]>min_cor)	{
				_ASSERT(b<line.GetNumofElements()-licz);
				out.data[0] = b;
				min_cor = corr_res[b];
			}

		
	
		delete[] corr_res;
		delete[] in;
		delete[] tmp;
		delete[] fpattern;
		delete[] line_filt;
		delete[] pattern_interp;
		break;
	default:
		perr.SetError("IQI_Detect: mode not supported");
		return;
	}
}

double corrcoef(double* x, double* y, double meanx, double meany, double stdx, double stdy, unsigned int ile)
{
/*	http://en.wikipedia.org/wiki/Correlation */
	double s1;
	unsigned int l;
	s1 = 0;
	for(l=0;l<ile;l++)
		s1 += (x[l]-meanx)*(y[l]-meany);

	return s1/( (ile-1)*stdx*stdy);
}


inline double std(double* x,double srednia_z_x,unsigned int ile)
{
/* srednia z x podawana z zewnatrz zeby przyspieszyc, bo i tak jest liczona */

	unsigned int a;
	double suma;
	
	suma = 0;
		
	for(a=0;a<ile;a++)
		suma+=pow( (x[a] - srednia_z_x),2 );
	suma = sqrt(suma/(ile-1));
	return suma;
}

inline double mean(double* x,unsigned int ile)
{
	double meanx;
	unsigned int l;
	meanx = 0;
	for(l=0;l<ile;l++)
		meanx += x[l];
	meanx/=ile;
	return meanx;
}

void Scale01(double* in,unsigned int ile)
{
/* modyfikuje dane wejœciowe */

	double mi,ma;
	unsigned int l;

	mi = 1e306;
	ma = -1e306;
	for(l=0;l<ile;l++)
		if(in[l]<mi)
			mi = in[l];
	
	for(l=0;l<ile;l++)
		in[l]-=mi;
	
	for(l=0;l<ile;l++)
		if(in[l]>ma)
			ma = in[l];

	for(l=0;l<ile;l++)
		in[l]/=ma;

}

void LinearDetrend(double* in,double* pattern, unsigned int ile)
{
/*	Najpierw odejmowanie od danych wzorca i dopiero z tego regresja
	PóŸniej z regresji odejmowanie od danych, tak jak w FindBar.m 
	Zak³ada ¿e wejscia s¹ przeskalowamne do 0-1	*/

	double a,b;
	double *tmp;
	unsigned int l;

	tmp = new double[ile];
	for(l=0;l<ile;l++)
		tmp[l] = in[l]-pattern[l];	// oba przeskalowane do 01
	LinearRegresion(tmp,ile,a,b);
	for(l=0;l<ile;l++)
		in[l]-=a*l+b;

//	#ifdef _DEBUG
//	DumpAll.AddEntry(tmp,ile,"reg_tmp");
//	DumpAll.AddEntry(&a,1,"reg_a");
//	DumpAll.AddEntry(&b,1,"reg_b");
//	#endif

	delete[] tmp;
}

void LinearRegresion(double* in,unsigned int ile, double& a, double& b)
{
/* za³o¿enie ze x jest zawsze od 0:ile a y to in
   http://dept.physics.upenn.edu/~uglabs/Least-squares-fitting-with-Excel.pdf */

	unsigned int aa;
	double c1,c2,c3,c4;

	c1=c2=c3=c4=0.0;

	for(aa=0;aa<ile;aa++)
	{
		c1 += aa*aa;			// sum x2
		c2 += in[aa];		// sum y
		c3 += aa;			// sum a
		c4 += aa*in[aa];		// sum a*y
	}
	b = (c1*c2-c3*c4)/(ile*c1-c3*c3);
	a = (ile*c4-c3*c2)/(ile*c1-c3*c3);
}

bool AverageLine(C_Image_Container* spaw,unsigned int srodek,int ile,C_Matrix_Container* out)
{
	unsigned int a;
	int b;
	double aveg;
	if( ((srodek-ile)<0) || ((srodek+ile)>spaw->_rows-1) )
		return TRUE;	// error

	out->AllocateData(1,spaw->_cols);
	for(a=0;a<spaw->_cols;a++)
	{
		aveg = 0.0;
		for(b=-ile;b<ile;b++)
			aveg+=spaw->GetPixel(srodek+b,a);
		out->data[a] = aveg/(2*ile);
	}
	return FALSE;
}