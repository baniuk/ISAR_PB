/* Na podstawie funkcji z projektu cf z RadiographySoftware */
#pragma once
#define _USE_MATH_DEFINES
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"
#include "C_DumpAll.h"
#include <math.h>
#include "stdlib.h"
#include <stdio.h>
#include <tchar.h>
#include <crtdbg.h>
#include <process.h>

typedef long int integer;
HANDLE hThread1[8];

#define NUMFEAT 71
#define MAX_NUM_FLAW 1000 // max ilosc obiektów
#define MIN_PIXEL_NUM 5	// minimalna iloœæ pikseli

// w³¹cza  dumping
#define NO_VERBOSE

C_DumpAll DumpAllCF("cf.out");	// do debugowania

struct E	{
	double *offset_data;
	double *x_coord_data;
	double *y_coord_data;
	C_Image_Container *original;
	C_Image_Container *indexed;
	C_Image_Container *bw;
	double *features;
	unsigned int num_of_flaws;
	unsigned int start_flaw;
	unsigned int end_flaw;
	unsigned int start_poz;	// pozycja startowa w macierzy features
	char deb;
};

E EX[8];

void  func(void *data);
void cov(C_Matrix_Container* X, C_Matrix_Container* Y);
unsigned int unique(C_Matrix_Container* input,C_Matrix_Container* licznosc);
BOOL isintable(double* tab,double val,unsigned int tab_size);
void RemoveVal(C_Matrix_Container* input,double val);
void GetMinMax(double* data, double& min, double& max,unsigned int ile);
void CutImage(C_Image_Container*im,C_Image_Container *out, double *xc, double *yc, unsigned int ile);
BOOL MatrixMulti(C_Matrix_Container* in1, C_Matrix_Container* in2, C_Matrix_Container* out);

void PB_feat1(unsigned int current_flaw, unsigned int current_flaw_offset,C_Image_Container *res, C_Matrix_Container *x_coord, C_Matrix_Container *y_coord, unsigned int current_flaw_size,double* out1,double* out2,double* out6,double* out7_1,double* out7_2,double* out7_3,double* out7_4,double* out7_5,double* out8,double* out9);
void PB_feat2(	unsigned int current_flaw, unsigned int current_flaw_offset,C_Image_Container *res,C_Image_Container *org, C_Matrix_Container *x_coord, C_Matrix_Container *y_coord, unsigned int current_flaw_size,
				double Mout[]);
void PB_texture(unsigned int current_flaw, unsigned int current_flaw_offset,C_Image_Container *org, C_Matrix_Container *x_coord, C_Matrix_Container *y_coord, unsigned int current_flaw_size,double* out1,double* out2);
double PB_stdcirc(C_Image_Container *in,double gx,double gy);
double PB_getIndexNumber(C_Image_Container *res);
void featuresMC(C_Image_Container* BwIm,C_Image_Container* BwPer,double Area,double Perimeter,double* Cx,double* Cy,double *Width,double *Length,double *Elongation,double *Rectangulity,double *Compactness,double *Iacf);

unsigned long LN_Area(C_Image_Container*);
double LN_Perimeter(C_Image_Container *in, C_Image_Container *out);
double LN_wspMal(double A, double p);
double LN_wspBB(C_Image_Container *in, double A, double Gx, double Gy);
double LN_wspDan(C_Image_Container *in, double A);
double LN_wspHar(C_Image_Container *in, double gx, double gy, double ilePikseli);
void LN_morf(C_Matrix_Container* F, int M[3][3], bool bit);
void LN_CenterOfGravity(C_Image_Container *in, double A, double *Gx, double *Gy);
void PCA(double* a,integer Na,integer LDA,double *D);


void cISAR_CalcFeatures(C_Image_Container& original, C_Image_Container& indexed, C_Image_Container& bw,
						C_Matrix_Container& out,
						char NUMOFPROC,
						char deb,
						C_Error& err)
{

	unsigned int num_of_points=0; // ile jest wszystkich punktów
	unsigned int a,x,y,o,licznik;
	unsigned int pixel;
	unsigned int num_of_flaws; // ile jest zaindeksowanych wad
	unsigned int delta,dd;

	C_Matrix_Container usorted;	// pojedyncze wartosci
	C_Matrix_Container licznosc; // ilosc pixeli kazdego rodzaju
	C_Matrix_Container x_coord;	// wsp x punktów
	C_Matrix_Container y_coord; // wsp y punktów
	C_Matrix_Container offset;	// offset od poczatku tablicy x,y_coord skazuj¹cy na kolejne wady
	C_Matrix_Container lastpoz;	// ostatnia pozycja dl kazdej wady w tablicy indexów
	C_Matrix_Container features;
	C_Matrix_Container* global_proc;
	C_Matrix_Container ind_tmp;
#ifdef _DEBUG
	DumpAllCF.AddEntry(&original,"original");
	DumpAllCF.AddEntry(&indexed,"indexed");
	DumpAllCF.AddEntry(&bw,"bw");
#endif
	if(original._cols!=indexed._cols)
		err.SetError("cf: Input images differ in size - results may be improper");
	else
		if(original._cols!=bw._cols)
			err.SetError("cf: Input images differ in size - results may be improper");
		else
			if(original._rows!=indexed._rows)
				err.SetError("cf: Input images differ in size - results may be improper");
			else
				if(original._rows!=bw._rows)
					err.SetError("cf: Input images differ in size - results may be improper");

	// konwersja indeksed do postaci liniowej, gdzie wady sa numerowane od 0 do N

	ind_tmp.AllocateData(1,65536);
	ind_tmp.Zeros();
	for(a=0;a<indexed.GetNumofElements();a++)
	{
		_ASSERT((unsigned int)indexed.data[a]<65536);
		ind_tmp.data[(unsigned int)indexed.data[a]] = indexed.data[a];	// w ind_tmp na okreœlonych indeksach s¹ indeksy wad (indeks w tej tablicy jest taki sam jak zawartoœæ pod tym indeksem)
	}
#ifdef _DEBUG
	DumpAllCF.AddEntry(&ind_tmp,"ind_tmp");
#endif
	// przegl¹danie ca³ej tablicy ind_tmp i numeracja niezerowych elementów od 1 do N
	unsigned int bn = 1;	// numer bierz¹cy
	for (a=1;a<ind_tmp.GetNumofElements();a++)	// bez pierwszego
		if(ind_tmp.data[a]!=0)	// jesli nie jest 0 to zmieniam na kolejny numer od 1
			ind_tmp.data[a] = bn++;
#ifdef _DEBUG
	DumpAllCF.AddEntry(&ind_tmp,"ind_tmpmod");
#endif
	// modyfikacja orygina³u - pod indeks zgodny z ind_tmp wstawiam wartoœæ która pod nim jest w tej tablicy
	for(unsigned int tmpa,a=0;a<indexed.GetNumofElements();a++)
	{
		tmpa = (unsigned int)indexed.data[a];
		_ASSERT(tmpa<ind_tmp.GetNumofElements());
		indexed.data[a] = ind_tmp.data[ tmpa ];
	}
#ifdef _DEBUG
	DumpAllCF.AddEntry(&indexed,"indexedmod");
#endif


//-------------------------------------------------------------------------------------
	num_of_flaws = unique(&indexed,&licznosc)-1; // bo w indexed jeszcze tlo jest
	if(num_of_flaws>MAX_NUM_FLAW)	{
		err.SetError("cf: Number of objects exceed allowed limit !!");
		num_of_flaws = MAX_NUM_FLAW;
	}
	if(num_of_flaws<1) {
		err.SetError("cf: Za malo wad - obiekt jednorodny");
		return;
	}

	for(a=1;a<licznosc.GetNumofElements();a++)	// od jedeynki zeby zer nie liczyc
		num_of_points+=(unsigned int)licznosc.data[a];
#ifdef _DEBUG
	DumpAllCF.AddEntry(&licznosc,"licznosc");
#endif

	offset.AllocateData(1,65536);
	offset.Zeros();

	for(o=1;o<licznosc.GetNumofElements();o++)
		if(licznosc.data[o]!=0)
			for(a=1;a<o;a++)
				offset.data[o]+=licznosc.data[a];
		else
			break;
	offset.data[o]=offset.data[o-1]+licznosc.data[a];	// ostatnia wada 

// wspolrzedne wad
	unsigned int tmppoz;
	lastpoz.AllocateData(1,65536);
	usorted.AllocateData(1,65536);
	lastpoz.Zeros();
	usorted.Zeros();
	licznik = 0;
	x_coord.AllocateData(1,num_of_points);
	y_coord.AllocateData(1,num_of_points);
	for(y=0;y<indexed._rows;y++)
		for(x=0;x<indexed._cols;x++)	{
			pixel = (unsigned int)indexed.GetPixel(y,x);
			if(pixel!=0)	{
				_ASSERT(pixel<65536);
				usorted.data[pixel] = pixel;
				tmppoz = (unsigned int)offset.data[pixel]+(unsigned int)lastpoz.data[pixel];
				_ASSERT(tmppoz<x_coord.GetNumofElements());
				x_coord.data[ tmppoz ] = x;
				y_coord.data[ tmppoz] = y;
				lastpoz.data[pixel]++;
			}
		}

#ifdef _DEBUG
		DumpAllCF.AddEntry(&offset,"offsetbr");
		DumpAllCF.AddEntry(&usorted,"usortedbr");
#endif	
	RemoveVal(&offset,0);
	RemoveVal(&usorted,0);
	features.AllocateData(num_of_flaws,NUMFEAT);
	features.Zeros();
// Main loop

#ifdef _DEBUG
	usorted.DumpBinary("usorted.dat");
	DumpAllCF.AddEntry(&offset,"offset");
	DumpAllCF.AddEntry(&usorted,"usorted");
	DumpAllCF.AddEntry(&x_coord,"x_coord");
	DumpAllCF.AddEntry(&y_coord,"y_coord");
#endif

//_RPT0(_CRT_WARN, "Use the assert macros to evaluate the expression p1 == p2.\n");
	
//	x_coord.CloneObject(&x_coord2);
//	y_coord.CloneObject(&y_coord2);

	if(num_of_flaws<10)
		NUMOFPROC = 1;
	delta = (unsigned int)floor((double)num_of_flaws/NUMOFPROC);
	for(dd=0;dd<(unsigned int)NUMOFPROC;dd++)	{
		EX[dd].bw = &bw;
		EX[dd].features = features.data;
		EX[dd].indexed = &indexed;
		EX[dd].start_flaw = dd*delta;
		EX[dd].end_flaw = EX[dd].start_flaw+delta-1;
		if(dd==NUMOFPROC-1) EX[dd].end_flaw = num_of_flaws-1;
		EX[dd].num_of_flaws = EX[dd].end_flaw-EX[dd].start_flaw+1;
		EX[dd].start_poz = EX[dd].start_flaw*NUMFEAT;	// 21-ilosc cech
		EX[dd].offset_data = offset.data;
		EX[dd].original = &original;
		EX[dd].x_coord_data = x_coord.data;
		EX[dd].y_coord_data = y_coord.data;
		EX[dd].deb = deb;
	}

	for(char thread=0;thread<NUMOFPROC;thread++)
		hThread1[thread] = (HANDLE)_beginthread(func, 0,(void*)&EX[thread]);

	WaitForMultipleObjects(NUMOFPROC,hThread1,TRUE,INFINITE);

	features.CloneObject(&out);
}
void func(void *data)
{
	char str[50];
	E *ex1;
	ex1 = (E*)data;
	unsigned int current_flaw,current_flaw_offset,current_flaw_size;
	double LN_area,LN_peri,LN_mal,LN_gx,LN_gy,LN_bb,LN_dan,LN_har;
	C_Image_Container res;		// wycieta wada z indexed
	C_Image_Container org;		// wycieta wada z oryginalu
	C_Image_Container black;		// wycieta wada z BW
	unsigned int l,ll=0,ff=ex1->start_poz;
	C_Matrix_Container fxcoord;
	C_Matrix_Container fycoord;
	double PBf1,PBf2,PBf6,PBf7_1,PBf7_2,PBf7_3,PBf7_4,PBf7_5,PBf8,PBf9;	
	double PBt1,PBt2;
	double MC_Cx,MC_Cy,MC_Width,MC_Length,MC_Elongation,MC_Rectangulity,MC_Compactness,MC_Iacf;
	C_Image_Container B;
	double t_xmin, t_xmax, t_ymin, t_ymax;

#define MOMENTS 35
	double Mout[MOMENTS]; // momenty do funkco pb_feat2

#ifdef _DEBUG
//	ex1->indexed.DumpBinary("indexed.out");
#endif

	for(current_flaw=ex1->start_flaw;current_flaw<=ex1->end_flaw;current_flaw++)
	{
		// Cutting the flaw
		LN_gx = LN_gy = 0;
		if(current_flaw==0)	{
			current_flaw_offset = 0;
			current_flaw_size = (unsigned int)ex1->offset_data[0];
		} else {
			current_flaw_offset = (unsigned int)ex1->offset_data[current_flaw-1];
			current_flaw_size = (unsigned int)(ex1->offset_data[current_flaw]-ex1->offset_data[current_flaw-1]);
		}

		CutImage(ex1->indexed,	&res,	&ex1->x_coord_data[current_flaw_offset],&ex1->y_coord_data[current_flaw_offset], current_flaw_size);
		CutImage(ex1->original, &org,	&ex1->x_coord_data[current_flaw_offset],&ex1->y_coord_data[current_flaw_offset], current_flaw_size);
		CutImage(ex1->bw,		&black, &ex1->x_coord_data[current_flaw_offset],&ex1->y_coord_data[current_flaw_offset], current_flaw_size);
		GetMinMax(&ex1->x_coord_data[current_flaw_offset], t_xmin, t_xmax, current_flaw_size);
		GetMinMax(&ex1->y_coord_data[current_flaw_offset], t_ymin, t_ymax, current_flaw_size);

	/* USER FUCTIONS HERE
		Usefull parameters:
			unsigned int current_flaw			- current flaw index
			unsigned int current_flaw_offset	- offset in coordinates tables
			C_Image_Container res				- extracted flaw (indexed)
			C_Image_Container org				- extracted flaw (original)
			C_Matrix_Container x_coord			- x coordinates of flaws
			C_Matrix_Container y_coord			- y coordinates of flaws
			unsigned int current_flaw_size		- number of pixels in current flaws
	*/
	/* How to extract coordinates for given flaw: 
			x_coord.data[current_flaw_offset:current_flaw_offset+current_flaw_size]
		*/
// do debugowania i wyswietlania w matlabie
	if(ex1->deb==1)
	{
		sprintf_s(str,50,"org%d.out",current_flaw);
		org.DumpBinary(str);
		sprintf_s(str,50,"ind%d.out",current_flaw);
		res.DumpBinary(str);
	}


		ll=0;
		fxcoord.AllocateData(current_flaw_size,1);
		fycoord.AllocateData(current_flaw_size,1);
		for(l=current_flaw_offset;l<current_flaw_offset+current_flaw_size;l++)	{
			fxcoord.data[ll] = ex1->x_coord_data[l];
			fycoord.data[ll++] = ex1->y_coord_data[l];
		}
// nagrywanie dla kompatybilosci z wskclassifier
/*		sprintf_s(str,50,"x%d.out",current_flaw);
		fxcoord.DumpBinary(str);
		sprintf_s(str,50,"y%d.out",current_flaw);
		fycoord.DumpBinary(str);
*/
		if(current_flaw_size<MIN_PIXEL_NUM)	// wada za ma³a - wszystko na -1
		{
			// dwa pierwsze bez zmian
			ex1->features[ff++] = current_flaw;	// number
			ex1->features[ff++] = PB_getIndexNumber(&res);	// indexed number
			for(unsigned int a=2;a<NUMFEAT;a++)
				ex1->features[ff++] = -1;
			continue;
		}
// ******************************************* PB *************************************
		PB_feat1(current_flaw,current_flaw_offset,&res,&fxcoord,&fycoord,current_flaw_size,&PBf1,&PBf2,&PBf6,&PBf7_1,&PBf7_2,&PBf7_3,&PBf7_4,&PBf7_5,&PBf8,&PBf9);
		PB_feat2(current_flaw,current_flaw_offset,&res,ex1->original,&fxcoord,&fycoord,current_flaw_size,Mout);
		
		PB_texture(current_flaw,current_flaw_offset,&org,&fxcoord,&fycoord,current_flaw_size,&PBt1,&PBt2);
		ex1->features[ff++] = current_flaw;	// number
		ex1->features[ff++] = PB_getIndexNumber(&res);	// indexed number
		ex1->features[ff++] = PBf1;	// data lenghtening
		ex1->features[ff++] = PBf2;	// angle
		ex1->features[ff++] = PBf6;	// Principal lenghtening
		ex1->features[ff++] = PBf7_1;	// Longer axis
		ex1->features[ff++] = PBf7_2;	// shorter axis
		ex1->features[ff++] = PBf7_3;	// pole-punktyin
		ex1->features[ff++] = PBf7_4;	// punkty out
		ex1->features[ff++] = PBf7_5;	// angle ellip nr10
// ******************************************* LN *************************************
		ex1->features[ff++] = LN_area=LN_Area(&res);
		B.AllocateData(res._rows, res._cols); B.Zeros();
		ex1->features[ff++] = LN_peri=LN_Perimeter(&res, &B);
		ex1->features[ff++] = LN_mal=LN_wspMal(LN_area, LN_peri);
		LN_CenterOfGravity(&res, LN_area, &LN_gx, &LN_gy);
		ex1->features[ff++] = LN_gx;
		ex1->features[ff++] = LN_gy;
		ex1->features[ff++] = LN_bb=LN_wspBB(&res, LN_area,LN_gx,LN_gy);
		ex1->features[ff++] = LN_dan=LN_wspDan(&res, LN_area);
		ex1->features[ff++] = LN_har=LN_wspHar(&B, LN_gx,LN_gy,LN_peri);// 18
// ******************************************* PB *************************************
		ex1->features[ff++] = PB_stdcirc(&B,LN_gx,LN_gy);
		ex1->features[ff++] = PBt1;	//20
		ex1->features[ff++] = PBt2;
// ******************************************* MC *************************************
		featuresMC(&black, &B, LN_area, LN_peri, &MC_Cx, &MC_Cy, &MC_Width, &MC_Length, &MC_Elongation, &MC_Rectangulity, &MC_Compactness, &MC_Iacf);
		ex1->features[ff++] = MC_Cx;
		ex1->features[ff++] = MC_Cy;
		ex1->features[ff++] = MC_Width;
		ex1->features[ff++] = MC_Length;
		ex1->features[ff++] = MC_Elongation;
		ex1->features[ff++] = MC_Rectangulity;
		ex1->features[ff++] = MC_Compactness;
		ex1->features[ff++] = MC_Iacf;

		ex1->features[ff++] = PBf8;		// dlugosc wady po osi dluzszej (30)
		ex1->features[ff++] = PBf9;		// dlugosc wady po osi krotszej
		ex1->features[ff++] = t_xmin;	// wsp x minimalna wady - raczej do celów testowych do rysowania elips na radiogramie
		ex1->features[ff++] = t_xmax;	// wsp x minimalna wady - raczej do celów testowych do rysowania elips na radiogramie
		ex1->features[ff++] = t_ymin;	// wsp x minimalna wady - raczej do celów testowych do rysowania elips na radiogramie
		ex1->features[ff++] = t_ymax;	// wsp x minimalna wady - raczej do celów testowych do rysowania elips na radiogramie
// **************************************** PB ****************************************
		ex1->features[ff++] = Mout[0];	// ff36
		ex1->features[ff++] = Mout[1];
		ex1->features[ff++] = Mout[2];
		ex1->features[ff++] = Mout[3];
		ex1->features[ff++] = Mout[4];	//ff40
		ex1->features[ff++] = Mout[5];
		ex1->features[ff++] = Mout[6];
		ex1->features[ff++] = Mout[7];
		ex1->features[ff++] = Mout[8];
		ex1->features[ff++] = Mout[9];
		ex1->features[ff++] = Mout[10];
		ex1->features[ff++] = Mout[11];
		ex1->features[ff++] = Mout[12];
		ex1->features[ff++] = Mout[13];
		ex1->features[ff++] = Mout[14]; //ff50
		ex1->features[ff++] = Mout[15];
		ex1->features[ff++] = Mout[16];
		ex1->features[ff++] = Mout[17];
		ex1->features[ff++] = Mout[18];
		ex1->features[ff++] = Mout[19];
		ex1->features[ff++] = Mout[20];
		ex1->features[ff++] = Mout[21];
		ex1->features[ff++] = Mout[22];
		ex1->features[ff++] = Mout[23];
		ex1->features[ff++] = Mout[24];	// ff60
		ex1->features[ff++] = Mout[25];
		ex1->features[ff++] = Mout[26];
		ex1->features[ff++] = Mout[27];
		ex1->features[ff++] = Mout[28];
		ex1->features[ff++] = Mout[29];
		ex1->features[ff++] = Mout[30];
		ex1->features[ff++] = Mout[31];
		ex1->features[ff++] = Mout[32];
		ex1->features[ff++] = Mout[33];
		ex1->features[ff++] = Mout[34];
		ex1->features[ff++] = Mout[35];

	}
	
	_endthread();
}
double PB_getIndexNumber(C_Image_Container *res)
{
	// zwraca numer indeksu dla danej wady z funkcji index
	unsigned long a;
	for(a=0;a<res->GetNumofElements();a++)
		if(res->data[a]>0)
			return res->data[a];
	return -1;

}
double PB_stdcirc(C_Image_Container *in,double gx,double gy)
{
	unsigned int r,k,licznik=0;
	C_Matrix_Container data;
	C_Matrix_Container data1;
	C_Matrix_Container out;
//	double srx,sry;
//	sry  = floor((double)(in->_rows+1)/2 + 0.5);    // poniewaz jest odjeta srednia wiec srodkiem jest zero, czyli polowa dlogosc we wspolrzednych obrazka
//	srx = floor((double)(in->_cols+1)/2 + 0.5);
	data.AllocateData(in->GetNumofElements(),1);
	data.Zeros();
//in->DumpBinary("c:\\b.out");
	for(r=0;r<in->_rows;r++)
		for(k=0;k<in->_cols;k++)
			if(in->GetPixel(r,k)!=0)
				data.data[licznik++] = sqrt(((r+1)-gy)*((r+1)-gy) + ((k+1)-gx)*((k+1)-gx) );

	
	data1.AllocateData(--licznik,1);
	for(r=0;r<licznik;r++)
		data1.data[r] = data.data[r];
//data1.DumpBinary("c:\\dd.out");
	data1.stdcol(&out);
	return out.data[0];
}

void PB_texture(unsigned int current_flaw, unsigned int current_flaw_offset,
				C_Image_Container *org, C_Matrix_Container *x_coord,
				C_Matrix_Container *y_coord, unsigned int current_flaw_size,
				double* out1,double* out2)
{
	double mi,ma,pol,meano1,meano2;
	double liczo1,liczo2,datao1,datao2,mean;
	unsigned long a;
	C_Matrix_Container std;
	liczo1=liczo2=datao1=datao2=mean=0;

	org->std(&std);
	*(out1) = std.data[0];
//...................................
	mi = 65535*2;
	ma = -65535*2;
	
	for(a=0;a<org->GetNumofElements();a++)	{
		mean+=org->data[a];
		if(org->data[a]>0)	{
			if(org->data[a]<mi) mi = org->data[a];
			if(org->data[a]>ma) ma = org->data[a];
		}
	}
	mean/=org->GetNumofElements();
	pol = (ma-mi)/2+mi;
	for(a=0;a<org->GetNumofElements();a++)	{
		if(org->data[a]>pol)	{
			datao1+=org->data[a];
			liczo1++;
		}
		if(org->data[a]<pol && org->data[a]>0)	{
			datao2+=org->data[a];
			liczo2++;
		}
	}
	if(liczo1>0)
		meano1 = datao1/liczo1;
	else
		meano1 = 0;
	if(liczo2>0)
		meano2 = datao2/liczo2;
	else
		meano2 = 0;
	
	if(mean==0)
		*(out2) = -1;
	else
		*(out2) = abs(meano2-meano1)/mean;

}
void featuresMC(C_Image_Container* BwIm,C_Image_Container* BwPer,double Area,double Perimeter,double* Cx,double* Cy,double *Width,double *Length,double *Elongation,double *Rectangulity,double *Compactness,double *Iacf)
{
	// C_Image_Container* BwIm - obraz wejœciowy czarno-bia³y (0-1) zawieraj¹cy wadê
	// C_Image_Container* BwPer - obraz wejœciowy czarno-bia³y (0-1) zawieraj¹cy obwód wady
	// double Area - parametr wejœciowy, pole powierzchni wady ( zliczone pixele bêd¹ce wad¹)
	// double Perimeter - parametr wejœciowy, pole powierzchni obwodu wady, czyli obwód (zliczone pixele bêd¹ce obwodem)
	// double* Cx - parametr wyjœciowy, œrodek ciê¿koœci wzglêdem x
	// double* Cy - parametr wyjœciowy, œrodek ciê¿koœci wzglêdem y
	// double *Width - parametr wyjœciowy, szerokoœæ wady ( zawsze krótsza ni¿ d³ugoœæ)
	// double *Length - parametr wyjœciowy, d³ugoœæ wady ( zawsze d³u¿sza ni¿ szerokoœæ)
	// double *Elongation - parametr wyjœciowy, wyd³u¿enie (stosunek szerokoœci do d³ugoœci)
	// double *Rectangulity - parametr wyjœciowy, stosunek Area/(Length*Width);
	// double *Compactness - parametr wyjœciowy, stosunek (Perimeter*Perimeter)/(4*PI*Area)
	// double *Iacf - parametr wyjœciowy, Lengthering index, stosunek Ia=(Dmax*Dmax*PI)/(4*Area), gdzie Dmax naj³u¿sz œrednica wady


	double CGx,CGy,pix,temp,W,L,Elong,Rec,Comp,Ia,D,Dmax,xdim,ydim;
	const double PI = 3.1415926535;
		
	int i,j,xs,xe,ys,ye;
		
	CGx = 0;
	CGy = 0;

	xs=BwIm->_rows-1;
	xe=0;
	ys=BwIm->_cols-1;
	ye=0;

	W =0;
	L = 0;
	Elong = 0;
	Rec = 0;
	Comp = 0;
	Ia=0;
	D=0;
	Dmax=0;

	
	for(i=0;i<BwIm->_rows;i++)
	{
		for(j=0;j<BwIm->_cols;j++)
		{
			BwIm->GetPixel(i,j,pix);
			// Centroid
			if(pix ==1)
			{
				CGx=CGx+i;
				CGy=CGy+j;
			}
			// Width, Length and Elongation coefficient
			if(i<xs)
			{
				xs=i;
			}
			if(i>xe)
			{
				xe=i;
			}
			if(j<ys)
			{
				ys=j;
			}
			if(j>ye)
			{
				ye=j;
			}
		}
	}
	// Width, Length and Elongation coefficient

	W=(double)(xe-xs+1);
	L=(double)(ye-ys+1);
	 
	if(W>L)
	{
		temp=W;
		W=L;
		L=temp;
	}
	 Elong = W/L;
	 if(W<3 && L<3)
	 {
		 Elong = -1;
	 }

	 *Width=W;
	 *Length=L;
	 *Elongation=Elong;

	// Centroid
	if(Area<5)
	{
		CGx=-1;
		CGy=-1;
	}
	else
	{
		CGx = CGx / Area;
		CGy = CGy / Area;
	}

	*Cx = CGx;
	*Cy = CGy;

	//Recatngulity
	Rec = Area/(L*W);
	if(Area<5 || (W<3 && L<3))
	{
		Rec=-1;
	}
	*Rectangulity=Rec;

	//Compactness
	Comp = (Perimeter*Perimeter)/(4*PI*Area);
	if(Perimeter<4)
	{
		Comp = -1;
	}
	*Compactness = Comp;

	//Lengthening index
	
	for(i=0;i<BwPer->_rows;i++)
	{
		for(j=0;j<BwPer->_cols;j++)
		{
			BwPer->GetPixel(i,j,pix);
			if(pix==1)
			{
				xdim = CGx - i;
				ydim = CGy - j;
				D=2*(sqrt((xdim*xdim)+(ydim*ydim)));
				if(D>Dmax)
				{
					Dmax=D;
				}
			}
		}
	}
	Ia=(Dmax*Dmax*PI)/(4*Area);
	if(Dmax<2 || Area<5)
	{
		Ia=-1;
	}
	*Iacf=Ia;	
}


void PB_feat1(unsigned int current_flaw, unsigned int current_flaw_offset,
			  C_Image_Container *res, C_Matrix_Container *x_coord,
			  C_Matrix_Container *y_coord, unsigned int current_flaw_size,
			  double* out1,double* out2,
			  double* out6,
			  double* out7_1,double* out7_2,double* out7_3,double* out7_4,
			  double* out7_5,double* out8,double* out9)
{
	double mean_x=0,mean_y=0;
	unsigned int l, punktyin, punktyout,licznik,pozycja;
	double DD[2];
	double tmp[2];
	double minx,maxx,miny,maxy;
	double cosadl,alfadl,alfadl_copy;
	double dluga_os,krotka_os,ratiod,ratiopkt,osa,osb,dl_osa,dl_osb,delta_osa,delta_osb,pole,e,odl1,odl2,tmp1,tmp2,sk1,sk2,sk3;
	C_Matrix_Container F1,F2, Ftmp, Zbiorcza, wyn;
	C_Matrix_Container xy;
	C_Matrix_Container covariance;
	C_Matrix_Container v_org;
	C_Matrix_Container tmpm;
	C_Matrix_Container rotation;
	char secondvector = 1;
	char mainvector;
    C_Matrix_Container punkty_rot;

	xy.AllocateData(2,x_coord->_rows);	// aby ³atwiej kopiowaæ by³o
	covariance.AllocateData(2,2);
	
	for(l=0;l<x_coord->GetNumofElements();l++)	{
		mean_x+=x_coord->data[l];
		mean_y+=y_coord->data[l];
	}
	mean_x/=x_coord->GetNumofElements();
	mean_y/=y_coord->GetNumofElements();
	
	for(l=0;l<x_coord->_rows;l++)
		xy.data[l] = y_coord->data[l]-mean_y;
	for(l=x_coord->_rows;l<2*x_coord->_rows;l++)
		xy.data[l] = x_coord->data[l-x_coord->_rows]-mean_x;
	xy.Transpose(); //rx2
//xy.DumpBinary("c:\\xy.dat");	
	cov(&xy,&covariance);
//covariance.DumpBinary("c:\\covariance.out");	// ok 
	PCA(covariance.data,2,2,DD);
//covariance.DumpBinary("c:\\pca.out");		//ok
	
	covariance.CloneObject(&v_org);
	if(DD[0]>DD[1])	
	{	mainvector = 0; secondvector = 1;}
	else
	{	mainvector = 1; secondvector = 0;}

	if(mainvector>0)	{
		tmp[0] = covariance.data[0];	tmp[1] = covariance.data[2];	//organizacja - kolumny z matlaba s¹ liniowo po kolei
		covariance.data[0] = covariance.data[1];
		covariance.data[2] = covariance.data[3];
		covariance.data[1] = tmp[0];	covariance.data[3] = tmp[1];
	}		// ok
//covariance.DumpBinary("c:\\pca2.out");
	covariance.Transpose();
	xy.Transpose();
	punkty_rot.AllocateData(xy._rows,xy._cols);
	MatrixMulti(&covariance,&xy,&punkty_rot);	//ok
//xy.DumpBinary("c:\\xy1.out");
//covariance.DumpBinary("c:\\pca1.out");
//punkty_rot.DumpBinary("c:\\punktyrot.out");
	tmpm.AllocateData(1,punkty_rot._cols);
	for(l=0;l<punkty_rot._cols;l++)
		tmpm.data[l] = punkty_rot.GetPixel(0,l);
	tmpm.getMinMax(minx,maxx);
	for(l=0;l<punkty_rot._cols;l++)
		tmpm.data[l] = punkty_rot.GetPixel(1,l);
	tmpm.getMinMax(miny,maxy);
	if(maxy==miny)	{
		*(out1) = -1;
		*(out8) = -1;
		*(out9) = -1;
	}
	else	{
		*(out1) = (maxx-minx)/(maxy-miny);
		*(out8) = maxx-minx;
		*(out9) = maxy-miny;
	}

// ************************************ FEAT2 ******************************************************
	
	cosadl = (v_org.GetPixel(0,mainvector))/
		(sqrt(v_org.GetPixel(0,mainvector)*v_org.GetPixel(0,mainvector)+v_org.GetPixel(1,mainvector)*v_org.GetPixel(1,mainvector) ) );
	alfadl = alfadl_copy = abs(acos(cosadl)*180/M_PI);
	if(alfadl>90)
		alfadl = abs(alfadl-180)/90;
	else
		alfadl/=90;
	*(out2) = alfadl;

// ************************************ FEAT6 ******************************************************
	
	if(DD[secondvector]==0)
		*(out6) = -1;
	else
		*(out6) = DD[mainvector]/DD[secondvector];

// ************************************ FEAT7 ******************************************************

	alfadl = alfadl_copy;
	alfadl = acos(cosadl);
	if(v_org.GetPixel(1,mainvector)<0) alfadl = -alfadl;
	rotation.AllocateData(2,2);
	rotation.SetPixel(0,0,cos(-alfadl)); rotation.SetPixel(0,1,-sin(-alfadl));
	rotation.SetPixel(1,0,sin(-alfadl)); rotation.SetPixel(1,1,cos(-alfadl));
	xy.Transpose();punkty_rot.AllocateData(xy._rows,xy._cols);
	MatrixMulti(&xy,&rotation,&punkty_rot);	
//punkty_rot.DumpBinary("c:\\punktyrot7.out");
//rotation.Dump("c:\\rotation.m");
//xy.DumpBinary("c:\\xy.dat");
	ratiod = DD[mainvector]/DD[secondvector];
	tmpm.AllocateData(1,punkty_rot._rows);
	for(l=0;l<punkty_rot._rows;l++)
		tmpm.data[l] = punkty_rot.GetPixel(l,0);
	tmpm.getMinMax(minx,maxx);
	for(l=0;l<punkty_rot._rows;l++)
		tmpm.data[l] = punkty_rot.GetPixel(l,1);
	tmpm.getMinMax(miny,maxy);
	ratiopkt = (maxx-minx)/(maxy-miny);
	osa = ratiod*sqrt(v_org.GetPixel(0,mainvector)*v_org.GetPixel(0,mainvector)+
					  v_org.GetPixel(1,mainvector)*v_org.GetPixel(1,mainvector));
	osb = sqrt(v_org.GetPixel(0,secondvector)*v_org.GetPixel(0,secondvector)+
			   v_org.GetPixel(1,secondvector)*v_org.GetPixel(1,secondvector));
	delta_osa = (2*maxx - 0.01*maxx)/50;
	delta_osb = (2*maxy - 0.01*maxy)/50;
	licznik = 0;
	F1.AllocateData(1,2);
	F2.AllocateData(1,2);
	Ftmp.AllocateData(1,2);
	Zbiorcza.AllocateData(51*51,4);	//[dl_osa dl_osb pktyinout pktyinout]
	wyn.AllocateData(51*51,1); wyn.Zeros();
	for(dl_osa=0.01*maxx;dl_osa<2*maxx;dl_osa+=delta_osa)
		for(dl_osb=0.01*maxy;dl_osb<2*maxy;dl_osb+=delta_osb)
		{
			pole = M_PI*dl_osa*dl_osb;
			if(dl_osa>dl_osb)	{
				dluga_os = dl_osa;
				krotka_os = dl_osb;
			} else {
             dluga_os = dl_osb;
             krotka_os = dl_osa;
			}
			e = sqrt(1-(krotka_os*krotka_os)/(dluga_os*dluga_os));
			Ftmp.data[0] = -e*dluga_os; Ftmp.data[1] = 0;
			MatrixMulti(&Ftmp,&rotation,&F1);	
			Ftmp.data[0] = e*dluga_os; Ftmp.data[1] = 0;
			MatrixMulti(&Ftmp,&rotation,&F2);	
			punktyin = 0; punktyout = 0;
			for(l=0;l<xy._rows;l++)
			{
				tmp1 = xy.GetPixel(l,0)-F1.data[0];
				tmp2 = xy.GetPixel(l,1)-F1.data[1];
				odl1 = sqrt(tmp1*tmp1+tmp2*tmp2);
				tmp1 = xy.GetPixel(l,0)-F2.data[0];
				tmp2 = xy.GetPixel(l,1)-F2.data[1];
				odl2 = sqrt(tmp1*tmp1+tmp2*tmp2);
				if(odl1+odl2<=2*dluga_os)
					punktyin++;
				else
					punktyout++;
			}
			if(punktyin!=0 && punktyout!=0)
			{
				Zbiorcza.SetPixel(licznik,0,dluga_os); Zbiorcza.SetPixel(licznik,1,krotka_os);
				sk1 = abs( (pole-punktyin)/punktyout);
				sk2 = abs( dluga_os/krotka_os-ratiopkt );
				sk3 = ((double)punktyin+(double)punktyout)/(double)punktyin;
				wyn.data[licznik] = (0.5*sk1+0.5*sk3+0.01*sk2)/(0.5+0.5+0.01);
				Zbiorcza.SetPixel(licznik,2,pole-punktyin); Zbiorcza.SetPixel(licznik,3,punktyout);
				licznik++;		
			}
		}
//wyn.DumpBinary("c:\\wyn.dat");
		minx = 1e303;

		bool hits=FALSE;
		for(l=0;l<licznik;l++)	{
			if(wyn.data[l]<minx){
				minx = wyn.data[l];
				pozycja = l;
				hits = true;	// oznacza ze choc raz program tu wszed³, czyli jest jakieœ minimum
			}
		}

		if(hits)	{
			*(out7_1) = Zbiorcza.GetPixel((unsigned long)pozycja,0);
			*(out7_2) = Zbiorcza.GetPixel((unsigned long)pozycja,1);
			*(out7_3) = Zbiorcza.GetPixel((unsigned long)pozycja,2);
			*(out7_4) = Zbiorcza.GetPixel((unsigned long)pozycja,3);
			*(out7_5) = alfadl;
		} else {
			*(out7_1) = -1;
			*(out7_2) = -1;
			*(out7_3) = -1;
			*(out7_4) = -1;
			*(out7_5) = alfadl;
		}

	
}

/*
 * M jest tablica przez któr¹ bêd¹ zwracane wartoœci, zgodnie z zalaczona dokumentacja
 * Mout[0] - m00
 * Mout[1] - m10
 * Mout[2] - m01
 * Mout[3] - ii
 * Mout[4] - jj
 * Mout[5] - M11 momenty centralne
 * Mout[6] - M20
 * Mout[7] - M02
 * Mout[8] - M21
 * Mout[9] - M12
 * Mout[10] - M30
 * Mout[11] - M03
 * Mout[12] - N11 momenty centralne unormowane
 * Mout[13] - N20
 * Mout[14] - N02
 * Mout[15] - N21
 * Mout[16] - N12
 * Mout[17] - N30
 * Mout[18] - N03
 * Mout[19] - I1 niezmienniki momentowe Hu
 * Mout[20] - I2
 * Mout[21] - I3
 * Mout[22] - I4
 * Mout[23] - I5
 * Mout[24] - I6
 * Mout[25] - I7
 * Mout[26] - NM1 niezmienniki omentowe wg tadeusiewicza
 * Mout[27] - NM2
 * Mout[28] - NM3
 * Mout[29] - NM4
 * Mout[30] - NM5
 * Mout[31] - NM6
 * Mout[32] - NM7
 * Mout[33] - NM8
 * Mout[34] - NM9
 * Mout[35] - NM10
 * */
void PB_feat2( unsigned int current_flaw, unsigned int current_flaw_offset,C_Image_Container *res, C_Image_Container *org, C_Matrix_Container *x_coord, C_Matrix_Container *y_coord, unsigned int current_flaw_size, double Mout[] )
{
	unsigned int a;
	double x,y,pix;
	double ii, jj;
	double N11,N20,N02,N21,N12,N30,N03;
	double M11,M21,M12,M02,M20,M30,M03,m00;

	for(a=0;a<MOMENTS;a++)
		Mout[a] = 0.0;
	// M[0-2]
	for(a=0;a<x_coord->GetNumofElements();a++)
	{
		x = x_coord->data[a];
		y = y_coord->data[a];
		pix = org->GetPixel(y,x);	
		x++; y++;
		Mout[0] += pix;
		Mout[1] += pix*x;	// zakladam liczenie od 1 wspolrzêdnych
		Mout[2] += pix*y;
	}
	m00 = Mout[0];

	// M[3]
	Mout[3] = Mout[1]/Mout[0];
	// M[4]
	Mout[4] = Mout[2]/Mout[0];

	ii = Mout[3];
	jj = Mout[4];
	for(a=0;a<x_coord->GetNumofElements();a++)
	{
		x = x_coord->data[a];
		y = y_coord->data[a];
		pix = org->GetPixel(y,x);
		x++;y++;	// zakladam liczenie od 1 wspolrzêdnych
		Mout[5]+=pow((x-ii),1)*pow((y-jj),1)*pix;
		Mout[6]+=pow((x-ii),2)*pow((y-jj),0)*pix;
		Mout[7]+=pow((x-ii),0)*pow((y-jj),2)*pix;
		Mout[8]+=pow((x-ii),2)*pow((y-jj),1)*pix;
		Mout[9]+=pow((x-ii),1)*pow((y-jj),2)*pix;
		Mout[10]+=pow((x-ii),3)*pow((y-jj),0)*pix;
		Mout[11]+=pow((x-ii),0)*pow((y-jj),3)*pix;
	}
	M11 = Mout[5];
	M20 = Mout[6];
	M02 = Mout[7];
	M21 = Mout[8];
	M12 = Mout[9];
	M30 = Mout[10];
	M03 = Mout[11];

	Mout[12] = N11 = Mout[5]/pow(Mout[0],(1+1)/2+1);	// N11
	Mout[13] = N20 = Mout[6]/pow(Mout[0],(2+0)/2+1);	// N20
	Mout[14] = N02 = Mout[7]/pow(Mout[0],(0+2)/2+1);	// N02
	Mout[15] = N21 = Mout[8]/pow(Mout[0],(2+1)/2+1);	// N21
	Mout[16] = N12 = Mout[9]/pow(Mout[0],(1+2)/2+1);	// N12
	Mout[17] = N30 = Mout[10]/pow(Mout[0],(3+0)/2+1);	// N30
	Mout[18] = N03 = Mout[11]/pow(Mout[0],(0+3)/2+1); // N03

	Mout[19] = N20 + N02;
	Mout[20] = (N20-N02)*(N20-N02) + 4*N11*N11;
	Mout[21] = (N30-3*N12)*(N30-3*N12) + (3*N12-N03)*(3*N12-N03);
	Mout[22] = (N30+N12)*(N30+N12) + (N21+N03)*(N21+N03);
	Mout[23] = (N30-2*N12)*(N30+N12)*( pow(N30+N12,2) - 3*pow(N21+N03,2) ) + (3*N21-N03)*(N21+N03)*( 3*pow(N03+N12,2) - pow(N21+N03,2) );
	Mout[24] = (N20-N02)*( pow(N30+N12,2) - pow(N21+N03,2) ) + 4*N11*(N30+N12)*(N21+N03);
	Mout[25] = ( 3*N21-N03)*(N30+N12)*( pow(N30+N12,2) - 3*pow(N21+N03,2) ) - (N30-3*N21)*(N21+N03)*( 3*pow(N30+N12,2) - pow(N21+N03,2) );

	Mout[26] = (M20+M02)/pow(m00,2);
	Mout[27] = ( pow(M20+M02,2) + 4*M11*M11 )/pow(m00,4);
	Mout[28] = ( pow(M30+3*M12,2) + pow(3*M21-M03,2) )/pow(m00,5);
	Mout[29] = ( pow(M30+M12,2) + pow(M21-M03,2) )/pow(m00,5);
	Mout[30] = ( (M30-3*M12)*(M30+M12)*( pow(M30+M12,2) - 3*pow(M21+M03,2) )+(3*M21-M03)*(M21+M03)*( 3*pow(M30+M12,2) - pow(M21+M03,2) ) )/pow(m00,10);
	Mout[31] = ( (M20-M02)*( pow(M30+M12,2) - pow(M21+M03,2) ) + 4*M11*(M30+M12)*(M21+M03) )/pow(m00,7);
	Mout[32] = (M20*M02-pow(M11,2))/pow(m00,4);
	Mout[33] = (M30*M12 + M21*M03 - M12*M12 - M21*M21)/pow(m00,5);
	Mout[34] = ( M20*(M21*M03-M12*M12)+M02*(M03*M12-M21*M21)-M11*(M30*M03-M21*M12) )/pow(m00,7);
	Mout[35] = ( pow(M30*M03 - M12*M21,2) - 4*(M30*M12-M21*M21)*(M03*M21-M12) )/pow(m00,10);


}


// ara generalnie nie potrzebne bo jest lizone z parametrów wad
unsigned long LN_Area(C_Image_Container* ic)
{
	unsigned long i, j;
	unsigned long pole = 0;
	for(i = 0; i < ic->_rows; i++)
	{
		for(j = 0; j < ic->_cols; j++)
		{
			if(ic->GetPixel(i, j) != 0)
			{
				pole = pole + 1;
			}
		}
	}
	return(pole);
}

double LN_Perimeter(C_Image_Container *in, C_Image_Container *out)
{
	int M[3][3] = { {7, 1, 7}, {1, 1, 1}, {7, 1, 7} };
	int i, j;
	double obwod = 0;
	//int maks, miin;
	C_Image_Container* pom = new C_Image_Container();
//	pom->AllocateData(in->_rows, in->_cols);
	//in->getMinMax(maks, miin);
	
	//przepisanie in do pom
	in->CloneObject(pom);
/*	for(i = 0; i < in->_rows; i++)
	{
		for(j = 0; j < in->_cols; j++)
		{
			pom->SetPixel(i, j, in->GetPixel(i, j));
		}
	}*/
	
	LN_morf(pom, M, false);
	
	for(i = 0; i < in->_rows; i++)
	{
		for(j = 0; j < in->_cols; j++)
		{
			out->SetPixel(i, j, in->GetPixel(i, j) - pom->GetPixel(i, j));
			if(out->GetPixel(i, j) != 0) obwod++;
		}
	}

	pom->FreeData();
	delete(pom);
	return(obwod);
}

void LN_morf(C_Matrix_Container* F, int M[3][3], bool bit)
{
	int p1 = 0, p2 = 0, p = 0, c, pasuje = 1, m, n;
	unsigned int i, j;
	double pom0, pom1; //Te dwie zmienne s³u¿¹ jako zero i jeden, bo macie¿ jest dziwnie wyskalowana
	//int piksel1, piksel2;
	C_Matrix_Container B;
	C_Matrix_Container A;
	B.AllocateData(F->_rows + 2, F->_cols + 2);
	A.AllocateData(F->_rows + 2, F->_cols + 2);
	B.Zeros();
	A.Zeros();
	for(i = 0; i < F->_rows; i++)
	{
		for(j = 0; j < F->_cols; j++)
		{
			A.SetPixel(i + 1, j + 1, F->GetPixel(i, j));
		}
	}
	A.getMinMax(pom0, pom1);
//A.DumpBinary("C:\\a.out");
//F->DumpBinary("C:\\f.out");
	//double B[A->_rows][A->_cols];
	//c = int((M->_cols) / 2);	
	c = 1;
	if(bit)	//wybrano pogrubianie/œcienianie
	{
		for(i = c; i < A._rows - c; i++)
		{
			for(j = c; j < A._cols - c; j++)
			{
				pasuje = 1;
				for(m = -c; m <= c; m++)
				{
					for(n = -c; n<= c; n++)
					{	
						if((A.GetPixel(i + m, j + n) != pom1 && M[m + c][n + c] == 1) || 
							(A.GetPixel(i + m, j + n) != pom0 && M[m + c][n + c] == 0))
							pasuje = 0;
					}
				}
				if(pasuje == 1)
				{
					if(M[c][c] == 1)
						B.SetPixel(i, j, 0);
					else if(M[c][c] == 0)
						B.SetPixel(i, j, 1);
					else
					{
						if(B.GetPixel(i, j) == 0) B.SetPixel(i, j, pom1);
						else B.SetPixel(i, j, pom0);
					}
				}
			}
		}
	}
	else //wybrano "hit-or-miss"
	{
		for(i = c; i < A._rows - c; i++)
		{
			for(j = c; j < A._cols - c; j++)
			{
				pasuje = 1;
				for(m = -c; m <= c; m++)
				{
					for(n = -c; n<= c; n++)
					{	
						if((A.GetPixel(i + m, j + n) != pom1 && M[m + c][n + c] == 1) ||
							(A.GetPixel(i + m, j + n) != pom0 && M[m + c][n + c] == 0))
							pasuje = 0;
					}
				}
				if(pasuje == 1) B.SetPixel(i, j, pom1);
				else B.SetPixel(i, j, pom0);
			}
		}
	}
	
	//doPliku("dumpB.txt", B);
//B.DumpBinary("C:\\b.out");
	for(i = 0; i < F->_rows; i++)
	{
		for(j = 0; j < F->_cols; j++)
		{
			F->SetPixel(i, j, B.GetPixel(i + 1, j + 1));
		}
	}
}

//Funkcja licz¹ca œrodek ciê¿koœci obiektu.
//A - powierzchnia
//Gx i Gy to wynik - wyliczone œrodki ciê¿kosci.
void LN_CenterOfGravity(C_Image_Container *in, double A, double *Gx, double *Gy)
{
	int i, j;
	for(i = 0; i < in->_rows; i++)
	{
		for(j = 0; j < in->_cols; j++)
		{
			if(in->GetPixel(i, j) != 0)
			{
				(*Gx) += j + 1;
				(*Gy) += i + 1;
			}
		}
	}
	//+1 ¿eby zachowaæ zgodnoœæ z matlabem
	if(A > 0)
	{
		(*Gx) = *(Gx) / A;
		(*Gy) = *(Gy) / A;
	}
	else
	{
		(*Gx) = 0;
		(*Gy) = 0;
	}
}
double LN_wspMal(double A, double p)
{
	if(A > 0) return((p / (2 * sqrt(3.14125 * A))) - 1);
	else return(-1);
}

//Wspó³czynnik Blaira-Blissa
//A - powierzchnia
//Gx - œrodek ciê¿koœci w kierunku x
//Gy - œrodek ciê¿koœci w kierunku y
double LN_wspBB(C_Image_Container *in, double A, double Gx, double Gy)
{
	int i, j;
	double pom = 0;
	for(i = 0; i < in->_rows; i++)
	{
		for(j = 0; j < in->_cols; j++)
		{
			if(in->GetPixel(i, j) != 0)
			{
				pom = pom + pow(abs(i - Gy), 2) + pow(abs(j - Gx), 2);
			}
		}
	}
	if(pom > 0) return(A / sqrt(2 * 3.14125 * pom));
	else return(-1);
}

//Wspó³czynnik Danielssona
double LN_wspDan(C_Image_Container *in, double A)
{
//	FILE *plik;
	int i, j;
	int M[3][3] = { {1, 1, 1}, {1, 1, 1}, {1, 1, 1} };
	unsigned long li = 0; //suma po wszystkich odleg³oœciach
	bool isZeros = false;
	C_Matrix_Container* pom2 = new C_Image_Container();
	C_Matrix_Container* fO = new C_Image_Container(); // Funkcja odleg³oœci
	pom2->AllocateData(in->_rows, in->_cols);
	pom2->Zeros();
	fO->AllocateData(in->_rows, in->_cols);
	fO->Zeros();
	for(i=0;i<pom2->GetNumofElements();i++)
		if(in->data[i] != 0)
			pom2->data[i] = 1;

/*	for(i = 0; i < in->_rows; i++)
	{
		for(j = 0; j < in->_cols; j++)
		{
			if(in->GetPixel(i, j) != 0)
			{
				pom2->SetPixel(i, j, 1);
			}
		}
	}*/
	//Tutaj pom zawiera jedynki tam, gdzie in mia³ niewiadomo co.
	//Teraz trzeba zrobiæ kolejne erozje i dodawaæ wyniki.
	while(!isZeros)//dopóki kolejne erozje zmieniaj¹ coœ w obrazie
	{
		//Erozja pom2
		LN_morf(pom2, M, false);
//		pom2->DumpBinary("c:\\pom.bin");
		//doPliku("pom2.txt", pom2);
		//Sprawdzenie, czy pom2 jest z³o¿one z samych zer. Je¿eli tak, to break.
		isZeros = true;
		for(i = 1; i < pom2->_rows - 1; i++)
		{
			for(j = 1; j < pom2->_cols - 1; j++)
			{
				if(pom2->GetPixel(i, j) != 0)
				{
					isZeros = false;
					break;
				}
			}
			if(!isZeros) break;
		}

		//Dodanie pom2 do fO
		for(i = 1; i < pom2->_rows - 1; i++)
		{
			for(j = 1; j < pom2->_cols - 1; j++)
			{
				fO->SetPixel(i, j, fO->GetPixel(i, j) + pom2->GetPixel(i, j));
			}
		}
	}
	for(i = 1; i < pom2->_rows - 1; i++)
	{
		for(j = 1; j < pom2->_cols - 1; j++)
		{
			if(fO->GetPixel(i, j) != 0) 
			{
				li += fO->GetPixel(i, j);
			}
		}
	}
	delete pom2;
	delete fO;
	if(li > 0) return(pow((double)A, 3) / pow((double)li, 2));
	else return(-1);
}

double LN_wspHar(C_Image_Container *in, double gx, double gy, double ilePikseli)
{
	int i, j;
	double suma1 = 0, suma2 = 0;
	for(i = 0; i < in->_rows; i++)
	{
		for(j = 0; j < in->_cols; j++)
		{
			if(in->GetPixel(i, j) != 0) 
			{
				suma1 += sqrt(pow((gx - j + 1), 2) + pow((gy - i + 1), 2));
				suma2 += pow((gx - j + 1), 2) + pow((gy - i + 1), 2);
			}
		}
	}
	if(ilePikseli * suma2 != 1) return(sqrt(pow(suma1, 2) / (ilePikseli * suma2 - 1)));
	else return(-1);
}

void GetMinMax(double* data, double& min, double& max,unsigned int ile)
{
	unsigned long r;
	min = 1.7e308;
	max = -1.7e308;;
	for(r=0;r<ile;r++)	{
		if(data[r]<min)	min = data[r];
		if(data[r]>max) max = data[r];
	}

}
void CutImage(C_Image_Container *im, C_Image_Container *out, double *xc, double *yc, unsigned int ile)
{
	unsigned int i;
	double minx,maxx,miny,maxy;
	GetMinMax(xc, minx, maxx,ile);
	GetMinMax(yc, miny, maxy,ile);
	out->AllocateData((unsigned int)(maxy-miny+1),(unsigned int)(maxx-minx+1));
	out->Zeros();
	for(i=0;i<ile;i++)
		out->SetPixel((unsigned int)(yc[i]-miny),(unsigned int)(xc[i]-minx),im->GetPixel((unsigned int)yc[i],(unsigned int)xc[i]));
}

void cov(C_Matrix_Container* X, C_Matrix_Container* Y)
// liczy covariancje macierzy X (po kolumnach)
{
	double sredniax, sredniay;
	unsigned long row,colx,coly;
	double covariance;

	for(colx=0;colx<X->_cols;colx++)
		for(coly=0;coly<X->_cols;coly++)	{
			covariance = 0;
			sredniax = X->Mean(NULL,colx);
			sredniay = X->Mean(NULL,coly);
			for(row=0;row<X->_rows;row++)
				covariance+=(X->GetPixel(row,colx) - sredniax)*(X->GetPixel(row,coly) - sredniay);
			covariance/=X->_rows-1;
			Y->SetPixel(colx,coly,covariance);
		}
}

unsigned int unique(C_Matrix_Container* input,C_Matrix_Container* licznosc)
// w licznosc znajduje sie informacja ile bylo pixeli kazdego rodzaju. 
{
	unsigned int a,ile=0,licznik=0;
	licznosc->AllocateData(1,65536);
	licznosc->Zeros();
	double tmp;
	for(a=0;a<input->GetNumofElements();a++)	{
		tmp = input->data[a];
		_ASSERT((unsigned int)tmp<65536);
		licznosc->data[(unsigned int)tmp]++;
	}

	for(a=0;a<licznosc->GetNumofElements();a++)
		if(licznosc->data[a]!=0)
			ile++;	// ile roznych pixeli

	return ile;
}

BOOL isintable(double* tab,double val,unsigned int tab_size)
{
	unsigned int a;
	for(a=0;a<tab_size;a++)
		if(tab[a]==val)	{
			return TRUE;
			break;
		}
	return FALSE;
}


void RemoveVal(C_Matrix_Container* input,double val)
{
// kasuje wartoœc val z vektora 1xn
	unsigned int a,ile=0,licznik=0;
	C_Matrix_Container tmp;

	for(a=0;a<input->GetNumofElements();a++)
		if(input->data[a]!=val)
			ile++;

	tmp.AllocateData(1,ile);
	for(a=0;a<input->GetNumofElements();a++)
		if(input->data[a]!=val)
			tmp.data[licznik++] = input->data[a];

	tmp.CloneObject(input);
}
BOOL MatrixMulti(C_Matrix_Container* in1, C_Matrix_Container* in2, C_Matrix_Container* out)
{
	unsigned int w,k,i;
	double wartosc,st;
	if(in1->_cols!=in2->_rows)
		return FALSE;
	if( (out->_cols!=in2->_cols)||(out->_rows!=in1->_rows) )
		return FALSE;

	out->Zeros();
	for(w=0; w<out->_rows; w++) 
		for(k=0; k<out->_cols; k++)
			for(i=0; i<in1->_cols; i++)	{
				wartosc = in1->GetPixel(w,i)*in2->GetPixel(i,k);
				st = out->GetPixel(w,k);
				out->SetPixel(w,k,st+wartosc); 
			}
	return TRUE;
}