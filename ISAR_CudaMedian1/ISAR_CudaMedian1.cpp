// ISAR_CudaMedian1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <cutil_inline.h>
#include <shrUtils.h>
#include <C_DumpAll.h>
#include "definitions.h"


bool InitCUDA(void);
void RunCUDA(Matrix din, Matrix dout, dim3 tpb, dim3 nob, Mask Okno, unsigned int startw, unsigned int starth);
C_DumpAll dump("debug.out");

int _tmain(int argc, _TCHAR* argv[])
{
	FILE *stream;
	fopen_s( &stream, "raport.log", "wt" );

	float* localdata;	// lokalne dane zamienione na float 
	cudaError_t ret;
	C_Matrix_Container testmatrix;
	C_Matrix_Container result;
	unsigned int timer = 0;

	Matrix data_in;		// dane wejœciowe ju¿ w pamiêci urz¹dzenia
	Matrix data_out;		// dane wyjœciowe w pamieci device
	Matrix localresult;		// dane wyjsciowe w pamieci hosta

	// parametry okna
	Mask Okno;
	Okno.wx = 61;
	Okno.wy = 61;
	Okno.px = (unsigned int)floor(Okno.wx/2.0);
	Okno.py = (unsigned int)floor(Okno.wy/2.0);
	Okno.e = Okno.wx*Okno.wy;
	Okno.cenr = (unsigned int)floor(Okno.e/2.0);	// jest tak bo i tak adresowanie jest od 0 wiêc o jeden mniej

	// do wylicznia parametrów grida
	unsigned int w_grid, h_grid;	// rozmiary siatki, wysokoœæ obliczana z proporcji
	double ratio;
	unsigned int ile_w, ile_h;	// ile gridów mieœci siê w obrazku
	unsigned int pw,ph, startw, starth;

	testmatrix.ReadBinary("testmatrix.dat");
	dump.AddEntry(&testmatrix,"testmatrix");	// kontrola czy poprawnie zaimportowa³o siê

	localresult.data = new float[testmatrix.GetNumofElements()];
	// kopiowanie do float
	localdata = new float[testmatrix.GetNumofElements()];
	for(unsigned int a=0;a<testmatrix.GetNumofElements();a++)
		localdata[a] = (float)testmatrix.data[a];

	const unsigned int &h = testmatrix._rows;	// rozmiary obrazka
	const unsigned int &w = testmatrix._cols;	// rozmiary obrazka

	// obliczanie rozmiaru grid oraz  ile gridów miesci siê w obrazku
//	ratio = w/h;
//	h_grid = (unsigned int)floor((double)w_grid/ratio);
//	ile_w = (unsigned int)floor((w-Okno.wx)/(double)w_grid);    // ile gridów mieœci siê po d³ugoœci ale bez brzegów,
//	ile_h = (unsigned int)floor((h-Okno.wy)/(double)h_grid);


	if(!InitCUDA()) {
		return 0;
	}
	cutCreateTimer( &timer);
	try
	{
		ret = cudaMallocPitch(	(void**)&data_in.data,&data_in.pitch,
			(size_t)(testmatrix._cols*sizeof(float)),
			(size_t)testmatrix._rows);	if (ret!=cudaSuccess)	throw ret;
		ret = cudaMallocPitch(	(void**)&data_out.data,&data_out.pitch,
			(size_t)(testmatrix._cols*sizeof(float)),
			(size_t)testmatrix._rows);	if (ret!=cudaSuccess)	throw ret;
		// przepisanie wartoœci z C_MatriŸ do cudy
		ret = cudaMemcpy2D(	data_in.data,
							data_in.pitch,
							localdata,
							testmatrix._cols*sizeof(float),
							(size_t)testmatrix._cols*sizeof(float),
							(size_t)testmatrix._rows,
							cudaMemcpyHostToDevice); if (ret!=cudaSuccess)	throw ret;
		data_in.width = testmatrix._cols;
		data_in.height = testmatrix._rows;

		dim3 threadsperblock(16,16);	// cols x rows 1 na 1 ze wzgêdu na szared memory kóra jest w ka¿dym bloku taka sama
		dim3 numofblocks(	60,
							60);

		ile_w = (unsigned int)ceil((float)data_in.width/(16*60));	/*tyle bloków na obrazek*/
		ile_h = (unsigned int)ceil((float)data_in.height/(16*60));


// start CUDA
		fprintf( stream, "Okno h=%d w=%d\n", Okno.wy, Okno.wx );
//		fprintf( stream, "Grid h=%d w=%d\n", h_grid, w_grid );
		fprintf( stream, "Ile h=%d w=%d\n", ile_h, ile_w );
		fprintf( stream, "Data h=%d w=%d\n\n", h, w );

		cutStartTimer( timer);

		startw = starth = 0;
				for(ph=0;ph<ile_h;ph++)
				{
					for(pw=0;pw<ile_w;pw++)
					{
						RunCUDA(data_in,data_out,threadsperblock,numofblocks, Okno, startw, starth);
						cudaThreadSynchronize();
						startw+=16*60;
					}
					starth+=16*60;
					startw = 0;
				}
		

		// pamiêæ shared jest widziana przez wszystkie w¹tki w bloku i na przyk³¹d wywo³anie threadsperblock(2,2); dla blocks(1,1) daje z³e wyniki (bo zawsze ten sma fragment pamieci dla ka¿dego w¹tku) a takie samo wywo³anie threadsperblock(1,1); i blocks(2,2) ju¿ jest poprawne
		
		ret = cudaMemcpy2D(	localresult.data,
							testmatrix._cols*sizeof(float),
							data_out.data,
							data_out.pitch,
							(size_t)testmatrix._cols*sizeof(float),
							(size_t)testmatrix._rows,
							cudaMemcpyDeviceToHost); if (ret!=cudaSuccess)	throw ret;
		cutStopTimer( timer);
		fprintf(stream, "Processing time: %f (ms)\n", cutGetTimerValue( timer));
		cutDeleteTimer( timer);

	
		result.AllocateData(testmatrix._rows,testmatrix._cols);
		for (unsigned int a = 0; a < testmatrix.GetNumofElements() ; a++)
			result.data[a] = localresult.data[a];

		dump.AddEntry(&result,"wynik");

		ret = cudaFree(data_in.data);	if (ret!=cudaSuccess)	throw ret;
		ret = cudaFree(data_out.data);	if (ret!=cudaSuccess)	throw ret;
		delete[] localresult.data;
		delete[] localdata;
		fclose( stream );

	}
	catch(cudaError_t err)	{
		printf("\nError: %s\n",cudaGetErrorString(err));
		return 0;
	}
	return 0;
}

#if __DEVICE_EMULATION__

bool InitCUDA(void){return true;}

#else
bool InitCUDA(void)
{
	int count = 0;
	int i = 0;
	cudaDeviceProp prop;

	cudaGetDeviceCount(&count);
	if(count == 0) {
		fprintf(stderr, "There is no device.\n");
		return false;
	}

	for(i = 0; i < count; i++) {
		if(cudaGetDeviceProperties(&prop, i) == cudaSuccess) {
			if(prop.major >= 1) {
				break;
			}
		}
	}
	if(i == count) {
		fprintf(stderr, "There is no device supporting CUDA.\n");
		return false;
	}
	cudaGetDeviceProperties(&prop, i);
	printf("CUDA device [%s] has %d Multi-Processors and %d threads per block\n", prop.name, prop.multiProcessorCount,prop.maxThreadsPerBlock);
	cudaSetDevice(i);

	printf("CUDA initialized.\n");
	return true;
}

#endif
