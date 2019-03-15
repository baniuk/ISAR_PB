#pragma once
#include <stdio.h>
#include <crtdbg.h>
#include <string.h>
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"

#define MAX_ENTRY 256
#define DOUBLE 1
#define C_MATRIX 2
#define C_IMAGE 3
#define FLOAT 4

class C_DumpAll
{
public:
	C_DumpAll(const char* filename);
	void AddEntry(const double* data, unsigned int size, char* name);
	void AddEntry(float* data, unsigned int size, char* name);
	void AddEntry(C_Matrix_Container* data, char* name);
	void AddEntry(C_Image_Container* data, char* name);
	~C_DumpAll(void);
private:
	FILE *stream;
	bool openfileerror;
	int lastpozindex;				//index ostatniego wpisu w offset
	unsigned long offset[MAX_ENTRY]; //offsety kolejnych wpisów
};
