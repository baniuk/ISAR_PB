#pragma once
#include "mex.h"
#include "C_Matrix_Container.h"
#include "C_Image_Container.h"
#include "C_Error.h"
#include "C_DumpAll.h"

C_DumpAll DumpAll("index.out");
// dokonuje segmentacji obrazka na podstawie algorytmu z ADDIPa

// pobiera obrazek wejœciowy, zwraca obraz po indexacji oraz strukturê b³êdu
void cISAR_ImIndex(	C_Image_Container& im,
					C_Image_Container& out,
					C_Error& perr);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs,
                 const mxArray *prhs[])
{
	double *p_image, *p_out;
	C_Image_Container image;
	C_Image_Container out;
	C_Error err;

	p_image = mxGetPr(prhs[0]);

	image.ImportFromMatlab(p_image,(unsigned int)mxGetM(prhs[0]),(unsigned int)mxGetN(prhs[0]));
	plhs[0] = mxCreateDoubleMatrix(mxGetM(prhs[0]),mxGetN(prhs[0]),mxREAL);

#ifdef _DEBUG 
	DumpAll.AddEntry(&image,"input");
#endif
	
	cISAR_ImIndex(image,out,err);
	if(err.status)	{
		mexPrintf(err.error);
		return;
	}

#ifdef _DEBUG 
	DumpAll.AddEntry(&out,"output");
#endif	

	p_out = mxGetPr(plhs[0]);
	out.ExportToMatlab(p_out);	
	
}

void cISAR_ImIndex(	C_Image_Container& im,
					C_Image_Container& indek,
					C_Error& perr)
{
	C_Image_Container bin;
	C_Image_Container *bino;
	bino = &im;
	bino->CloneObject(&bin);
    bin.CloneObject(&indek);
    indek.Zeros();
    bin.Normalize(0,1);
    double sklejka[100000];
    unsigned int x=0;
    unsigned int y=0;
    int licz=0;
    unsigned int roz1=indek._rows;
    unsigned int roz2=indek._cols;
    for(x=1;x<roz1-2;x++)           
		for(y=1;y<roz2-2;y++)                                                               
        {
			double war1,war2,war3,war4,war5,war6,war7;
            bin.GetPixel(x,y,war1);
            if(war1==1)
				{
					indek.GetPixel(x-1,y-1,war2);
                    indek.GetPixel(x-1,y,war3);
                    indek.GetPixel(x-1,y+1,war4);                           
                    indek.GetPixel(x,y-1,war5);
                    if(war2==0&&war3==0&&war4==0&&war5==0)
						{
							licz=licz+1;
                            indek.SetPixel(x,y,licz);
                            sklejka[licz]=licz+1;
                        }
					else
						{
							war6=0;
                            war7=0; 
                            if(war2!=0)
								war6=war2;
                            if(war3!=0)
                                war6=war3;
                            if(war4!=0)
                                war6=war4;
                            if(war5!=0)
                                war6=war5;
                            if(war2!=0&&war2!=war6)
                                war7=war2;
                            if(war3!=0&&war3!=war6)
                                war7=war3;
                            if(war4!=0&&war4!=war6)
                                war7=war4;
                            if(war5!=0&&war5!=war6)
                                war7=war5;
                            if(war7==0)
                                indek.SetPixel(x,y,war6);
                            else
                                {
									indek.SetPixel(x,y,min(war6,war7));
                                    sklejka[(int)max(war6,war7)]=min(war6,war7);
                                }
					}
			}
		}




         for(int i=1;i<licz;i++)
                 {

                        int war;
                        war=1+licz-i;
                        for(x=0;x<indek._rows;x++)                                                                        
                                for(y=0;y<indek._cols;y++)                                                                        
                                {
                                        double war1;
                                        indek.GetPixel(x,y,war1);
                                        if(war1==war)
                                        {
                                            _ASSERT(war<1000000);    
											double pom=sklejka[war];
                                                indek.SetPixel(x,y,pom);
                                        }
                                }
                 }
         double obecne=1;
                 double warmax,unused;
                 indek.getMinMax(unused,warmax);         
         for(int pom=1;pom<=warmax;pom++)
                 {
                         int licz1=0;
                         for(x=0;x<indek._rows;x++)
                                 for(y=0;y<indek._cols;y++)
                                 {
                                         double val1;
                                         indek.GetPixel(x,y,val1);
                                         if(val1==pom)
                                         {
                                                 indek.SetPixel(x,y,obecne);
                                                 licz1=licz1+1;

                                         }
                                 }
                        if(licz1>0)
                                obecne=obecne+1;
                 }

}