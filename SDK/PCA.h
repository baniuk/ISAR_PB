#define _USE_MATH_DEFINES
extern "C" {
#include <f2c.h>
#include <blaswrap.h>
#include <clapack.h>
}
//#include <stdafx.h>

void PCA(double* a,integer Na,integer LDA,double *D);