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
void quickSort(double *Array,int *ArrayIndex, int array_size);
void q_sort(double *Array,int *ArrayIndex, int left, int right);
bool quickSortfast(double *arr, unsigned int elements);
bool quickSortfastint(int *arr, unsigned int elements);


void cISAR_FastMedian(C_Image_Container *p_input, C_Matrix_Container *p_MaskRows, C_Matrix_Container *p_MaskCols, C_Image_Container* p_out,char NUMOFPROC,C_Error* err)
{
	C_Matrix_Container* global_proc;

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
	int *temp_index,*temp2_index,*temp_next_index,i,j,a,b,x,y,Imrows,Imcols,ArrayLength,DevModMid,m_mid_pos_row,m_mid_pos_col,z;
	int Imrows_temp,Imcols_temp;
	double *temp,*temp2,*temp_next_MaskCols,pixel,*Im1,median;
	// Maks must be odd ...then that wroks properly and LargeMaskSize/SmallMaskSize = integer
	// Create new temp tab
	Imrows = scr->_rows;
	Imcols = scr->_cols;

	ArrayLength = MaskRows*MaskCols;
	
	// getting the midle position in vector ArrayLength
	DevModMid=ArrayLength%2;
	DevModMid=(ArrayLength-DevModMid)/2;
	// getting the midle position in mask rows and columns
	m_mid_pos_row=MaskRows%2;
	m_mid_pos_row=(MaskRows-m_mid_pos_row)/2;

	m_mid_pos_col=MaskCols%2;
	m_mid_pos_col=(MaskCols-m_mid_pos_col)/2;

	if(MaskRows>=MaskCols)
    z=MaskRows;
	else if(MaskRows<MaskCols)
    z=MaskCols;
	_ASSERT(z!=0); // z do allokacji tablicy
//	BOOL test;

	int size_temp_table = ArrayLength;
	temp = new double[size_temp_table]; _ASSERT(temp!=NULL);
	temp_index = new int[size_temp_table]; _ASSERT(temp_index!=NULL);

	int size_temp2_table = ArrayLength-MaskRows;
	temp2 = new double[size_temp2_table]; _ASSERT(temp2!=NULL);
	temp2_index = new int[size_temp2_table]; _ASSERT(temp2_index!=NULL);

	int size_temp_next_table = z;
	temp_next_MaskCols = new double[size_temp_next_table]; _ASSERT(temp_next_MaskCols!=NULL);
	temp_next_index = new int[size_temp_next_table]; _ASSERT(temp_next_index!=NULL);

	Imrows_temp = Imrows + 2*m_mid_pos_row;
	Imcols_temp = Imcols + 2*m_mid_pos_col;
	int size_Im1_table = Imrows_temp*Imcols_temp;
	Im1 = new double[size_Im1_table]; _ASSERT(Im1!=NULL);

	
	for(i=0;i<Imrows_temp;i++)
	{
		for(j=0;j<Imcols_temp;j++)
		{
			if((i>=m_mid_pos_row && i<Imrows_temp-m_mid_pos_row) && (j>=m_mid_pos_col && j<Imcols_temp-m_mid_pos_col))
			{
				scr->GetPixel(i-m_mid_pos_row,j-m_mid_pos_col,pixel);
				_ASSERT((i)*Imcols_temp+j < size_Im1_table);
				*(Im1+(i)*Imcols_temp+j)=pixel;
			}
			
			else if(i<m_mid_pos_row)
			{
				if(j>=m_mid_pos_col && j<Imcols_temp-m_mid_pos_col)
				{
					scr->GetPixel(0,j-m_mid_pos_col,pixel);
					_ASSERT((i)*Imcols_temp+j < size_Im1_table);
					*(Im1+(i)*Imcols_temp+j)=pixel;
				}
				else if(j<m_mid_pos_col)
				{
					scr->GetPixel(0,0,pixel);
					_ASSERT((i)*Imcols_temp+j < size_Im1_table);
					*(Im1+(i)*Imcols_temp+j)=pixel;
				}
				else if(j>=Imcols_temp-m_mid_pos_col)
				{
					scr->GetPixel(0,Imcols-1,pixel);// chyba b³¹d
					_ASSERT((i)*Imcols_temp+j < size_Im1_table);
					*(Im1+(i)*Imcols_temp+j)=pixel;
				}
			}
			else if(i>=Imrows_temp-m_mid_pos_row)
			{
				if(j>=m_mid_pos_col && j<Imcols_temp-m_mid_pos_col)
				{
					scr->GetPixel(Imrows-1,j-m_mid_pos_col,pixel);
					_ASSERT((i)*Imcols_temp+j < size_Im1_table);
					*(Im1+(i)*Imcols_temp+j)=pixel;
				}
				else if(j<m_mid_pos_col)
				{
					scr->GetPixel(Imrows-1,0,pixel);
					_ASSERT((i)*Imcols_temp+j < size_Im1_table);
					*(Im1+(i)*Imcols_temp+j)=pixel;
				}
				else if(j>=Imcols_temp-m_mid_pos_col)
				{
					scr->GetPixel(Imrows-1,Imcols-1,pixel);
					_ASSERT((i)*Imcols_temp+j < size_Im1_table);
					*(Im1+(i)*Imcols_temp+j)=pixel;
				}
			}
			else if(j<m_mid_pos_row &&(i>=m_mid_pos_row && i<Imrows_temp-m_mid_pos_row))
			{
				scr->GetPixel(i-m_mid_pos_row,0,pixel);
				_ASSERT((i)*Imcols_temp+j < size_Im1_table);
				*(Im1+(i)*Imcols_temp+j)=pixel;
			}
			else if(j>Imcols_temp-m_mid_pos_col &&(i>=m_mid_pos_row && i<Imrows_temp-m_mid_pos_row))
			{
				scr->GetPixel(i-m_mid_pos_row,Imcols-1,pixel);
				_ASSERT((i)*Imcols_temp+j < size_Im1_table);
				*(Im1+(i)*Imcols_temp+j)=pixel;
			}
			
		}
	}
	


//********************************************************************************************************************
//********************************************************************************************************************
//********************************************************************************************************************

// moving the mask accros the image and looking for a median value
for (i=0;i<Imrows_temp-MaskRows+1;i++)
{  //LLL1       
     for (j=0;j<Imcols_temp-MaskCols+1;j++)
             { //LL1
                 
                 
// *********************************************************
// ************************ Start For Moving mask **********
// *********************************************************       
               
                    if(j==0)
                            {//for j=0
                            for(a=0;a<MaskRows;a++)
                                for(b=0;b<MaskCols;b++)
                                 {
									 _ASSERT(a*MaskCols+b < size_temp_table);
									 _ASSERT((i+a)*Imcols_temp+j+b < size_Im1_table);
                                    *(temp+a*MaskCols+b)=*(Im1+(i+a)*Imcols_temp+j+b);
									//*(temp_index+a*MaskCols+b)=a*MaskCols+b+1;
                                    *(temp_index+a*MaskCols+b)=a+b*MaskRows+1;
                                 }
                                        quickSort(temp,temp_index,ArrayLength);
										/*test=quickSortfast(temp,ArrayLength);
										_ASSERT(test);
										quickSortfastint(temp_index,ArrayLength);
										_ASSERT(test);*/
                                        median=*(temp+DevModMid);
										dst->SetPixel(i,j,median);

                                //*(Im2+(i+m_mid_pos_row)*Imcols+j+m_mid_pos_col)=median;
                         }// for j==0
                    if(j>0)
                        { // for j = MaskCols
                            for(a=0;a<MaskRows;a++)
                            {
								_ASSERT(a < size_temp_next_table);
								_ASSERT((i+a)*Imcols_temp+j+MaskCols-1 < size_Im1_table);
                                *(temp_next_MaskCols+a)=*(Im1+(i+a)*Imcols_temp+j+MaskCols-1);
                                *(temp_next_index+a)=a+1+ArrayLength;
                            }

                                quickSort(temp_next_MaskCols,temp_next_index,MaskRows);
								/*test = quickSortfastint(temp_next_index,MaskRows);
								_ASSERT(test);
								test = quickSortfastint(temp_next_index,MaskRows);
								_ASSERT(test);*/
                 // removing previous elements from temp2 and writing it to temp
                                y=0;
                                for(x=0;x<ArrayLength;x++)
                                    {// for L1
                                        if(*(temp_index+x)>MaskRows)
                                            { // L1.1
												_ASSERT(x-y < size_temp2_table);
												_ASSERT(x < size_temp_table);
                                                *(temp2+x-y)=*(temp+x);
                                                *(temp2_index+x-y)=*(temp_index+x);
                                            } // R1.1
                                        else if(*(temp_index+x)<=MaskRows)
                                            {//L1.2
                                                y=y+1;
                                            }//R1.2
                                    }// for R1
                                a=0;
                                b=0;
                // Conecting old vector and new mask column to one sorted vector                
                                while(a+b<ArrayLength)
                                    {//while L1
                                        if(a<(ArrayLength-MaskRows) && b<MaskRows)
                                            {//L1
                                                if(*(temp2+a)>*(temp_next_MaskCols+b))
                                                    {//L1.1
														_ASSERT(a+b < size_temp_table);
														_ASSERT(b < size_temp_next_table);
                                                       *(temp+a+b)= *(temp_next_MaskCols+b);
                                                       *(temp_index+a+b)=*(temp_next_index+b)-MaskRows;
                                                        b=b+1;
                                                    }//R1.1
                                                else if(*(temp2+a)<*(temp_next_MaskCols+b)) 
                                                    {//L1.2
														_ASSERT(a+b < size_temp_table);
														_ASSERT(a < size_temp2_table);
                                                       *(temp+a+b)= *(temp2+a);
                                                       *(temp_index+a+b)=*(temp2_index+a)-MaskRows;
                                                        a=a+1;
                                                    }//R1.2
                                                else if(*(temp2+a)==*(temp_next_MaskCols+b))
                                                    {//L1.3
														_ASSERT(a+b < size_temp_table);
														_ASSERT(b < size_temp_next_table);
                                                       *(temp+a+b)= *(temp_next_MaskCols+b);
                                                       *(temp_index+a+b)=*(temp_next_index+b)-MaskRows;
                                                        b=b+1;
														_ASSERT(a < size_temp2_table);
                                                       *(temp+a+b)= *(temp2+a);
                                                       *(temp_index+a+b)=*(temp2_index+a)-MaskRows;
                                                        a=a+1;  
                                                    }//R1.3    
                                            }//R1
                                        else if(a==(ArrayLength-MaskRows) && b<MaskRows)
                                            {//L2
												_ASSERT(a+b < size_temp_table);
												_ASSERT(b < size_temp_next_table);
                                                *(temp+a+b)= *(temp_next_MaskCols+b);
                                                *(temp_index+a+b)=*(temp_next_index+b)-MaskRows;
                                                 b=b+1;
                                            }//R2
                                        else if(a<(ArrayLength-MaskRows) && b==MaskRows)
                                            {//L3
												_ASSERT(a+b < size_temp_table);
												_ASSERT(a < size_temp2_table);
                                                *(temp+a+b)= *(temp2+a);
                                                *(temp_index+a+b)=*(temp2_index+a)-MaskRows;
                                                 a=a+1;
                                            }//R3

                                    }//while R1
                // looking for a median value                

                                median=*(temp+DevModMid);
								dst->SetPixel(i,j,median);

                                //*(Im2+(i+m_mid_pos_row)*Imcols+j-MaskCols+1+m_mid_pos_col)=median;

                        }// for j=MaskCols
             }//RR1
    }//RRR1

//********************************************************************************************************************
//********************************************************************************************************************
//********************************************************************************************************************
	SAFE_DELETE(temp);
	SAFE_DELETE(temp2);
	SAFE_DELETE(temp_index);
	SAFE_DELETE(temp2_index);
	SAFE_DELETE(temp_next_MaskCols);
	SAFE_DELETE(temp_next_index);
	SAFE_DELETE(Im1);
	_endthread();
}



void quickSort(double *Array,int *ArrayIndex, int array_size)
{
  q_sort(Array,ArrayIndex, 0, array_size - 1);
}


void q_sort(double *Array,int *ArrayIndex, int left, int right)
{
  int  l_hold, r_hold,pivot_index;
  double pivot;

  l_hold = left;
  r_hold = right;
  pivot = *(Array+left);
  pivot_index = *(ArrayIndex+left);
  while (left < right)
  {
    while ((*(Array+right) >= pivot) && (left < right))
      right--;
    if (left != right)
    {
      *(Array+left) = *(Array+right);
	  *(ArrayIndex+left) = *(ArrayIndex+right);		
      left++;
    }
    while ((*(Array+left) <= pivot) && (left < right))
      left++;
    if (left != right)
    {
      *(Array+right) = *(Array+left);
	  *(ArrayIndex+right) = *(ArrayIndex+left);
      right--;
    }
  }
  *(Array+left) = pivot;
  *(ArrayIndex+left) = 	pivot_index;	
  pivot_index = left;
  left = l_hold;
  right = r_hold;
  if (left < pivot_index)
    q_sort(Array,ArrayIndex, left, pivot_index-1);
  if (right > pivot_index)
    q_sort(Array,ArrayIndex ,pivot_index+1, right);
}

//  quickSort
//
//  This public-domain C implementation by Darel Rex Finley.
//
//  * Returns YES if sort was successful, or NO if the nested
//    pivots went too deep, in which case your array will have
//    been re-ordered, but probably not sorted correctly.
//
//  * This function assumes it is called with valid parameters.
//
//  * Example calls:
//    quickSort(&myArray[0],5); // sorts elements 0, 1, 2, 3, and 4
//    quickSort(&myArray[3],5); // sorts elements 3, 4, 5, 6, and 7

bool quickSortfast(double *arr, unsigned int elements) {

  #define  MAX_LEVELS  10000

  int  beg[MAX_LEVELS], end[MAX_LEVELS],i=0, L, R ;
  double piv;

  beg[0]=0; end[0]=elements;
  while (i>=0) {
    L=beg[i]; R=end[i]-1;
    if (L<R) {
      piv=arr[L]; if (i==MAX_LEVELS-1) return false;
      while (L<R) {
        while (arr[R]>=piv && L<R) R--; if (L<R) arr[L++]=arr[R];
        while (arr[L]<=piv && L<R) L++; if (L<R) arr[R--]=arr[L]; }
      arr[L]=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L; }
    else {
      i--; }}
  return true; }

bool quickSortfastint(int *arr, unsigned int elements) {

  #define  MAX_LEVELS  10000

  int  beg[MAX_LEVELS], end[MAX_LEVELS],i=0, L, R ;
  int piv;

  beg[0]=0; end[0]=elements;
  while (i>=0) {
    L=beg[i]; R=end[i]-1;
    if (L<R) {
      piv=arr[L]; if (i==MAX_LEVELS-1) return false;
      while (L<R) {
        while (arr[R]>=piv && L<R) R--; if (L<R) arr[L++]=arr[R];
        while (arr[L]<=piv && L<R) L++; if (L<R) arr[R--]=arr[L]; }
      arr[L]=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L; }
    else {
      i--; }}
  return true; }