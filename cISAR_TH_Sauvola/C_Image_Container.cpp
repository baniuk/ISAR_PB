#include "C_Image_Container.h"
//#endif
// Ver 1.3.1 by PB

C_Image_Container::C_Image_Container(void)
{
//	data = NULL;
	_iplbuffor = NULL;
	isBinary = FALSE;
	xorigin = yorigin = 0;
//	header = NULL;
	header_size = 0;
}

C_Image_Container::~C_Image_Container(void)
{
	SAFE_DELETE(_iplbuffor);
//	SAFE_DELETE(header);
}
char* C_Image_Container::ReturnIPLBuffor(void)
{
	unsigned long licznik, licznikdata;
	unsigned short reszta;

	licznik = 0;
	licznikdata = 0;
	SAFE_DELETE(_iplbuffor);
	_iplbuffor = new char[_cols*2*_rows];
	do	{
		reszta = (unsigned short)data[licznikdata] % 256;
		_iplbuffor[licznik] = (char)reszta;
		_iplbuffor[licznik+1] = (char)(((unsigned short)data[licznikdata++] - reszta)/256);
		licznik += 2;
	} while(licznik<_cols*2*_rows);
	return _iplbuffor;
}

char* C_Image_Container::ReturnIPLBuffor(unsigned short w1, unsigned short w2)
{
	unsigned long licznik, licznikdata;
	unsigned short reszta;
	C_Image_Container tmp;
	this->CloneObject(&tmp);
	tmp.Normalize(w1,w2);
	licznik = 0;
	licznikdata = 0;
	SAFE_DELETE(_iplbuffor);
	_iplbuffor = new char[tmp._cols*2*tmp._rows];
	do	{
		reszta = (unsigned short)tmp.data[licznikdata] % 256;
		_ASSERT(licznik<tmp._cols*2*tmp._rows);
		_iplbuffor[licznik] = (char)reszta;
		_ASSERT(licznik+1<tmp._cols*2*tmp._rows);
		_iplbuffor[licznik+1] = (char)(((unsigned short)tmp.data[licznikdata++] - reszta)/256);
		licznik += 2;
	} while(licznik<tmp._cols*2*tmp._rows);
	return _iplbuffor;
}

void C_Image_Container::Normalize(unsigned short w1, unsigned short w2)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	double min, max, delta;
	unsigned long r;
	getMinMax(min,max);
	if(min==max)
		return;
	if(min==w1 && max==w2)
		return;				// bez normalizacji
	delta = abs(w2-w1);
	for(r=0;r<GetNumofElements();r++)
		data[r] = delta*data[r]/(max-min)-min*delta/(max-min);

}

void C_Image_Container::Hist(unsigned int val,C_Matrix_Container &out)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
//	C_Matrix_Container out;
//	double min, max;
	unsigned long a;
	double delta,tmpval;
	unsigned short start,l,ll,licznik;
//	getMinMax(min,max);
	C_Image_Container tmp;
	C_Image_Container tmpout;;
	CloneObject(&tmp);
	out.AllocateData(1,val);
	out.Zeros();
	if(val!=65536)	{
		tmpout.AllocateData(1,65536);
		tmpout.Zeros();
		for(a=0;a<GetNumofElements();a++)
			tmpout.data[(unsigned short)tmp.data[a]]+=1;
		delta = floor(65535.0/val);
		licznik = 0;
		ll = 0;
		start = 0;
		do	{
			tmpval = 0;
			for(l=start;l<start+delta;l++)
				tmpval += tmpout.data[l];
			out.data[licznik++] = tmpval/delta;
			start += (unsigned short)delta;
			ll++;
		} while(ll<val);
	}
	else	{
		for(a=0;a<GetNumofElements();a++)
			out.data[(unsigned short)tmp.data[a]]+=1;
	}
//	return out;

}
void C_Image_Container::CloneObject(C_Image_Container* dest)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	dest->_cols = _cols;
	dest->_rows = _rows;
	dest->isBinary = isBinary;
	dest->FreeData();
	dest->AllocateData(dest->_rows,dest->_cols);
//	InheritProperties(dest);
	memcpy(dest->data,data,sizeof(double)*dest->_rows*dest->_cols);
}

/*void C_Image_Container::InheritProperties(C_Image_Container* dest)
{
	if(header!=NULL)	{
		dest->CreateHeaderBuffor(header_size);
		memcpy(dest->header,header,sizeof(char)*dest->header_size);
	} else
		dest->header = NULL;
}*/

void C_Image_Container::CreateHeaderBuffor(unsigned int size)
{
	SAFE_DELETE(header);	
	header = new char[size];
	header_size = size;
}

void C_Image_Container::FreeData(void)
{
	SAFE_DELETE((double*)data);
//	SAFE_DELETE(header);
	SAFE_DELETE(_iplbuffor);
}