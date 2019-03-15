#pragma once
#include "mex.h"
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"
#include "C_DumpAll.h"

C_DumpAll DumpAll("CalcFeatures.out");	// do debugowania

void cISAR_CalcFeatures(C_Image_Container& original,
						C_Image_Container& indexed,
						C_Image_Container& bw,
						C_Matrix_Container& out,
						char NUMOFPROC,
						char deb,
						C_Error& err);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs,
                 const mxArray *prhs[])
{
	double *p_original, *p_indexed,*p_bw, *p_out;
	char num_of_proc;
	char deb;
	C_Image_Container original;
	C_Image_Container indexed;
	C_Image_Container bw;
	C_Matrix_Container out;
	unsigned int a;
	C_Error err;

	p_original = mxGetPr(prhs[0]);
	p_indexed = mxGetPr(prhs[1]);
	p_bw = mxGetPr(prhs[2]);
	deb = (char)mxGetScalar(prhs[3]);
	num_of_proc = (char)mxGetScalar(prhs[4]);


	original.ImportFromMatlab(p_original,(unsigned int)mxGetM(prhs[0]),(unsigned int)mxGetN(prhs[0]));
	indexed.ImportFromMatlab(p_indexed,(unsigned int)mxGetM(prhs[1]),(unsigned int)mxGetN(prhs[1]));
	bw.ImportFromMatlab(p_bw,(unsigned int)mxGetM(prhs[2]),(unsigned int)mxGetN(prhs[2]));
#ifdef _DEBUG
	DumpAll.AddEntry(&original,"original");
	DumpAll.AddEntry(&indexed,"indexed");
	DumpAll.AddEntry(&bw,"bw");
#endif
	

	cISAR_CalcFeatures(	original,
						indexed,
						bw,
						out,
						num_of_proc,
						deb,
						err);

	if(err.status==TRUE)
	{
		plhs[0] = mxCreateLogicalMatrix(1,1);
	}
	else
	{
		plhs[0] = mxCreateDoubleMatrix(out._rows,out._cols,mxREAL);
		p_out = mxGetPr(plhs[0]);
		out.ExportToMatlab(p_out);
	}

}