#pragma once
#include "mex.h"
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"
#include "C_DumpAll.h"

#define ROUND(x) floor(x+0.5)

C_DumpAll DumpAll("index.out");
// normalizuje histogram obrazka

// pobiera obrazek wejœciowy oraz typ algorytmu oraz parametry, zwraca obraz po normalizacji oraz strukturê b³êdu
void cISAR_HistEqual(C_Image_Container& im,
					 C_Matrix_Container& parametry,
				     char mode,
					 C_Image_Container& out,
					 C_Error& perr);


void mexFunction(int nlhs, mxArray *plhs[], int nrhs,
                 const mxArray *prhs[])
{
	double *p_image, *p_out,*p_parametry;
	char mode;
	C_Image_Container image;
	C_Matrix_Container parametry;
	C_Image_Container out;
	C_Error err;

	p_image = mxGetPr(prhs[0]);
	p_parametry = mxGetPr(prhs[1]);
	mode = (char)mxGetScalar(prhs[2]);

	image.ImportFromMatlab(p_image,(unsigned int)mxGetM(prhs[0]),(unsigned int)mxGetN(prhs[0]));
	parametry.ImportFromMatlab(p_parametry,(unsigned int)mxGetM(prhs[1]),(unsigned int)mxGetN(prhs[1]));
	plhs[0] = mxCreateDoubleMatrix(mxGetM(prhs[0]),mxGetN(prhs[0]),mxREAL);

#ifdef _DEBUG 
	DumpAll.AddEntry(&image,"input");
	DumpAll.AddEntry(&parametry,"parametry");
#endif
	
	cISAR_HistEqual(image,
					parametry,
				    mode,
					out,
					err);

	if(err.status)	{
		mexPrintf(err.error);
		return;
	}

#ifdef _DEBUG 
	DumpAll.AddEntry(&out,"output");
#endif	

	p_out = mxGetPr(plhs[0]);
	out.ExportToMatlab(p_out);	
	
}

void cISAR_HistEqual(C_Image_Container& im,
					 C_Matrix_Container& parametry,
				     char mode,
					 C_Image_Container& out,
					 C_Error& perr)
{
	C_Matrix_Container* hist;
	C_Matrix_Container* lookup;
	double sum = 0;
	unsigned int a;
	unsigned int x0,y0,width,height;
	if(parametry.GetNumofElements()<4) {
		perr.SetError("dISAR_HistEqual: Za ma³o parametrów");
		return;
	}
	x0 = (unsigned int)parametry.data[0];
	y0 = (unsigned int)parametry.data[1];
	width = (unsigned int)parametry.data[2];
	height = (unsigned int)parametry.data[3];

	// lokalne min i max
	unsigned int r,k;
	double min,max,delta;
	min=max=out.GetPixel(y0,x0);
	for(k=x0; k<x0+width; k++)
		for(r=y0; r<y0+height; r++)
		{
			if(out.GetPixel(r,k)<min)
				min = out.GetPixel(r,k);
			if(out.GetPixel(r,k)>max)
				max = out.GetPixel(r,k);
		}

	switch(mode)
	{
	case 0:	
		// tylko rozciaganie liniowe do wybranego zakresu
		// wymaga dwóch parametrów: wartoœci min i wartoœci max
		if(parametry.GetNumofElements()<6)	{
			perr.SetError("dISAR_HistEqual: Za ma³o parametrów dla trybu 0");
			return;
		}
		if(parametry.data[4]>=parametry.data[5])	{
			perr.SetError("dISAR_HistEqual: P1<P2 musi byæ");
			return;
		}
		im.CloneObject(&out);

		delta = abs(parametry.data[5]-parametry.data[4]);
		for(r=0;r<out.GetNumofElements();r++)
			out.data[r] = delta*out.data[r]/(max-min)-min*delta/(max-min);

		out.Normalize((unsigned short)parametry.data[4],(unsigned short)parametry.data[5]);
		break;
	case 1:
		// mnozenie przez funkcjê kwadratow¹
		// wymaga jednego parametru - wyk³adnika
		if(parametry.GetNumofElements()<5)	{
			perr.SetError("dISAR_HistEqual: Za ma³o parametrów dla trybu 1");
			return;
		}
		im.CloneObject(&out);
		for(a=0;a<out.GetNumofElements();a++)
			out.data[a]= pow(out.data[a],parametry.data[4]);
		out.Normalize(0,1);
		break;
	case 2:
		// wyrównywanie histogramu
		// bez parametrów
		// http://fourier.eng.hmc.edu/e161/lectures/contrast_transform/node3.html
		im.CloneObject(&out);
		out.Normalize(0,65535);
		hist = new C_Matrix_Container;
		lookup = new C_Matrix_Container;
		hist->AllocateData(1,65536); hist->Zeros();
		lookup->AllocateData(1,65536);

		for(a=0;a<out.GetNumofElements();a++)
			hist->data[(unsigned int)ROUND(out.data[a])]++;

		for(a=0;a<65536;a++)	{
			sum+=hist->data[a];
			lookup->data[a] = sum*65535+0.5;
		}

		for(a=0;a<out.GetNumofElements();a++)
			out.data[a] = lookup->data[(unsigned int)out.data[a]];
		out.Normalize(0,65535);

		delete hist;
		delete lookup;
		break;
	default:
		perr.SetError("dISAR_HistEqual: mode not supported");
		return;
	}



}