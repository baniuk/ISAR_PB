#pragma once
#include "mex.h"
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"


void cISAR_SNR_Eval(C_Image_Container& input, double spatial, double& SNR_out, C_Error& err);

// SNR = SNR_Eval(image, spatial);
void mexFunction(int nlhs, mxArray *plhs[], int nrhs,
                 const mxArray *prhs[])
{
	double *p_input;
	double spatial,SNR;
	C_Image_Container input;
	C_Error err;

	p_input = mxGetPr(prhs[0]);
	spatial = mxGetScalar(prhs[1]);

	input.ImportFromMatlab(p_input,mxGetM(prhs[0]),mxGetN(prhs[0]));
//input.DumpBinary("input.dat");
	
	cISAR_SNR_Eval(input,spatial,SNR,err);

	plhs[0] = mxCreateDoubleScalar(SNR);
}

void cISAR_SNR_Eval(C_Image_Container& input, double spatial, double& SNR_out, C_Error& err)
{
	int ile_grup,a,start=0;
	double imeas,sigmapsl;
	C_Matrix_Container grupa;
	C_Matrix_Container m;
	C_Matrix_Container s;
	ile_grup = (int)floor((double)input.GetNumofElements()/20.0);
	if( (ile_grup<55)||(input._cols<20) ) {
		err.SetError("SNR_Eval: Obszar pomiarowy za maly lub za malo kolumn");
		return;
	}
	grupa.AllocateData(20,1);
	m.AllocateData(ile_grup,1);
	s.AllocateData(ile_grup,1);
	do
	{
		for(a=0;a<20;a++)
			grupa.data[a] = input.data[start+a];
		start+=20;
//grupa.Dump("grupa.m");
		m.data[ile_grup-1] = grupa.Mean(NULL,0);
		s.data[ile_grup-1] = 1.0179*grupa.std();

	} while(--ile_grup);
//m.Dump("m.m");
	imeas = m.Median();
	sigmapsl = s.Median()*(spatial/88.6);
	if(sigmapsl!=0.0)
		SNR_out = imeas/sigmapsl;
	else
	{
		SNR_out = -1;
		err.SetError("SNR_Eval: sgma=0 - obraz jednorodny, nie mo¿na policzyæ SNR");
	}

}