#pragma once
#include "mex.h"
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"
#include "C_DumpAll.h"


void cISAR_FastMedian(C_Image_Container *p_input,
					  C_Matrix_Container *p_MaskRows,
					  C_Matrix_Container *p_MaskCols,
					  C_Image_Container *p_out,
					  char NUMOFPROC,
						C_Error* err);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs,
                 const mxArray *prhs[])
{
	double *p_input, *p_out;
	double mMaskRows, mMaskCols;
	char num_of_proc;
	C_Image_Container input;
	C_Matrix_Container MaskRows;
	C_Matrix_Container MaskCols;
	C_Image_Container out;
	C_Error err;

	p_input = mxGetPr(prhs[0]);
	mMaskRows = (double)mxGetScalar(prhs[1]);
	mMaskCols = (double)mxGetScalar(prhs[2]);
	num_of_proc = (char)mxGetScalar(prhs[3]);

	input.ImportFromMatlab(p_input,(unsigned int)mxGetM(prhs[0]),(unsigned int)mxGetN(prhs[0]));
	MaskRows.AllocateData(1,1); MaskRows.SetPixel(0,0,mMaskRows);
	MaskCols.AllocateData(1,1); MaskCols.SetPixel(0,0,mMaskCols);
	
	cISAR_FastMedian(	&input,
						&MaskRows,
						&MaskCols,
						&out,
						num_of_proc,
						&err);


	plhs[0] = mxCreateDoubleMatrix(out._rows,out._cols,mxREAL);
	p_out = mxGetPr(plhs[0]);
	out.ExportToMatlab(p_out);
}