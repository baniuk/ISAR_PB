// DumpAll.cpp : Defines the entry point for the console application.
//
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"

#include "stdafx.h"



int _tmain(int argc, _TCHAR* argv[])
{
	double a[3]={3.14,2.14,1.14};
	double aa=4.567;
	int i;
	C_Matrix_Container b;
	C_DumpAll test("test1.out");

	b.AllocateData(3,2);
	for(i=0;i<b.GetNumofElements();i++)
		b.data[i] = i;
#ifdef _DEBUG 
	test.AddEntry(a,3,"a");
	test.AddEntry(&aa,1,"aa");
	test.AddEntry(&b,"b");
#endif
	return 0;
}

