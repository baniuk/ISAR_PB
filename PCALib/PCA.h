#define _USE_MATH_DEFINES
extern "C" {
#include <f2c.h>
#include <blaswrap.h>
#include <clapack.h>
}
#include <stddef.h>
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

void PCA(double* a,integer Na,integer LDA,double *D);