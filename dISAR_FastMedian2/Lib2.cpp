#pragma once
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"
#include "C_DumpAll.h"

// This is an empty project to build DLL library of interface version 2.0
#include <windows.h>
#include <process.h>
#include <crtdbg.h>
#include <math.h>
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)

using namespace std;
using namespace cv;

#define maxthreads 16
HANDLE hThread1[maxthreads];

struct E	{
	int MaskRows;
	int MaskCols;
	C_Image_Container* dst;
	C_Image_Container* scr;
};

E EX[maxthreads];

void  func(void *data);


void cISAR_FastMedian2(C_Image_Container *p_input, C_Matrix_Container *p_MaskRows, C_Matrix_Container *p_MaskCols, C_Image_Container* p_out,char NUMOFPROC,C_Error* err)
{

	C_Image_Container *SRC[maxthreads];
	C_Image_Container *DST[maxthreads];
	C_Image_Container *srctmp;
	unsigned int START[maxthreads];
	unsigned int STOP[maxthreads];
	unsigned int DELTA[maxthreads]; // iloœæ kolumn w obrazie dla ka¿dego watku
	unsigned int suma;
	BOOL czytranspose = false;

	if(NUMOFPROC>maxthreads)
		NUMOFPROC = maxthreads;

	int MaskRows, MaskCols;
	unsigned int r,k,k1,delta;
	char thread;
	C_Image_Container ImageCopy;

	MaskRows = (int)p_MaskRows->data[0];
	MaskCols = (int)p_MaskCols->data[0];
	
	srctmp = new C_Image_Container;
	p_input->CloneObject(srctmp);

	if(srctmp->_rows>srctmp->_cols && NUMOFPROC!=1)	{
		srctmp->Transpose();
		czytranspose = true;
	}

	for(thread=0;thread<NUMOFPROC;thread++)	{
		DST[thread] = new C_Image_Container;
		SRC[thread] = new C_Image_Container;
	}
//	srctmp->Normalize(0,65535);
	DST[0]->AllocateData(srctmp->_rows,srctmp->_cols);

	suma = 0;
	delta = (unsigned int)floor(srctmp->_cols/(double)NUMOFPROC);
	for(thread=0;thread<NUMOFPROC-1;thread++)	{
		DELTA[thread] = delta;
		suma+=delta;
	}
	DELTA[thread] = srctmp->_cols - suma;

	START[0] = 0;
	STOP[NUMOFPROC-1] = srctmp->_cols-1;
	for(thread=0;thread<NUMOFPROC;thread++)	{
		if(thread>0)
			START[thread] = START[thread-1] + DELTA[thread-1];
		if(thread<NUMOFPROC)
			STOP[thread] = START[thread]+DELTA[thread]-1;
	}

	if(NUMOFPROC>1)	{
		SRC[0]->AllocateData(srctmp->_rows,DELTA[0]+MaskCols);
		SRC[NUMOFPROC-1]->AllocateData(srctmp->_rows,DELTA[NUMOFPROC-1]+MaskCols);
		DST[NUMOFPROC-1]->AllocateData(SRC[NUMOFPROC-1]->_rows, SRC[NUMOFPROC-1]->_cols);
	} else
		SRC[0]->AllocateData(srctmp->_rows,srctmp->_cols);

	for(thread=1;thread<NUMOFPROC-1;thread++)	{
		SRC[thread]->AllocateData(srctmp->_rows,DELTA[thread]+2*MaskCols);
		DST[thread]->AllocateData(SRC[thread]->_rows, SRC[thread]->_cols);
	}

	for(thread=0;thread<NUMOFPROC;thread++)	{
		EX[thread].dst = DST[thread];
		EX[thread].scr = SRC[thread];
		EX[thread].MaskCols = MaskCols;
		EX[thread].MaskRows = MaskRows;

	}



/*	src1 = new C_Image_Container;
	src2 = new C_Image_Container;
	
	x1 = (unsigned int)floor(srctmp->_cols/2.0)+MaskCols;
	x2 = srctmp->_cols-(unsigned int)floor(srctmp->_cols/2.0)+MaskCols;
	src1->AllocateData(srctmp->_rows,x1);
	src2->AllocateData(srctmp->_rows,x2);*/

	if(NUMOFPROC>1)	{
		k1=0;
		for(r=0;r<srctmp->_rows;r++)	{
			for(k=START[0];k<=STOP[0]+MaskCols;k++)	{
				SRC[0]->SetPixel(r,k1++,srctmp->GetPixel(r,k));
			}
			k1 = 0;
		} 
		k1=0;
		for(r=0;r<srctmp->_rows;r++)	{
			for(k=START[NUMOFPROC-1]-MaskCols;k<=STOP[NUMOFPROC-1];k++)	{
				SRC[NUMOFPROC-1]->SetPixel(r,k1++,srctmp->GetPixel(r,k));
			}
			k1 = 0;
		} 
		for(thread=1;thread<NUMOFPROC-1;thread++)	{
			k1=0;
			for(r=0;r<srctmp->_rows;r++)	{
				for(k=START[thread]-MaskCols;k<=STOP[thread]+MaskCols;k++)	{
					SRC[thread]->SetPixel(r,k1++,srctmp->GetPixel(r,k));
				}
				k1 = 0;
			}
		}
	} else
		srctmp->CloneObject(SRC[0]);


	SAFE_DELETE(srctmp);


	for(thread=0;thread<NUMOFPROC;thread++)
		hThread1[thread] = (HANDLE)_beginthread(func, 0,(void*)&EX[thread]);

	WaitForMultipleObjects(NUMOFPROC,hThread1,TRUE,INFINITE);


	k1 = MaskCols;
	for(thread=1;thread<NUMOFPROC;thread++)
		for(r=0;r<SRC[thread]->_rows;r++)	{
			for(k=START[thread];k<=STOP[thread];k++)
				DST[0]->SetPixel(r,k,DST[thread]->GetPixel(r,k1++));
			k1 = MaskCols;
		}

	if(czytranspose)
		DST[0]->Transpose();

	DST[0]->CloneObject(p_out);

	for(thread=0;thread<NUMOFPROC;thread++)	{
		SAFE_DELETE(SRC[thread]);
		SAFE_DELETE(DST[thread]);
	}
}

void func(void *data)
{
	E *ex;
	ex = (E*)data;
	int MaskRows = ex->MaskCols;
	int MaskCols = ex->MaskCols;
	C_Image_Container* dst = ex->dst;
	C_Image_Container* scr = ex->scr;
	int Imrows,Imcols;

	// Maks must be odd ...then that wroks properly and LargeMaskSize/SmallMaskSize = integer
	// Create new temp tab
	Imrows = scr->_rows;
	Imcols = scr->_cols;

	
	_endthread();
}

