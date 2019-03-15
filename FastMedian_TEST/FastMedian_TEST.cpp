// FastMedian_TEST.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <C_DumpAll.h>
#include <Lib2.cpp>


int _tmain(int argc, _TCHAR* argv[])
{
	C_Image_Container input;
	C_Matrix_Container MaskRows;
	C_Matrix_Container MaskCols;
	C_Image_Container out;
	C_Error err;

	C_DumpAll dump("test.dat");

	input.ReadBinary("testimag4.dat");
	MaskRows.AllocateData(1,1); MaskRows.SetPixel(0,0,201);
	MaskCols.AllocateData(1,1); MaskCols.SetPixel(0,0,201);

	dump.AddEntry(&input,"in");
	dump.AddEntry(&MaskRows,"maskrows");
	dump.AddEntry(&MaskCols,"maskcols");

	cISAR_FastMedian(&input,
		&MaskRows,
		&MaskCols,
		&out,
		2,
		&err);

	dump.AddEntry(&out,"out");

	return 0;
}

