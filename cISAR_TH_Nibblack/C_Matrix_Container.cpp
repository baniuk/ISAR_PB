#include "C_Matrix_Container.h"
// Ver 1.3 by PB
// Ver 1.3.1 - dodano assert
// Ver 1.3.2 - poprawiono b³¹d w readbinary
// Ver 1.3.3 - zmieniono readbinary - zachowana kompatybilnoœæ z wersja poprzedni¹

C_Matrix_Container::C_Matrix_Container(void)
{
	data = NULL;
}

C_Matrix_Container::C_Matrix_Container( unsigned int rows, unsigned int cols )
{
	data = NULL;
	this->AllocateData(rows,cols);
}

C_Matrix_Container::~C_Matrix_Container(void)
{
	if(data) {delete[] (double*)data;     data=NULL;}
}
void C_Matrix_Container::AllocateData(unsigned int rows, unsigned int cols)
{
	if(data) {delete[] (double*)data;     data=NULL;}
	data = new double[rows*cols];
	_rows = rows;
	_cols = cols;
}
void C_Matrix_Container::FreeData(void)
{
	if(data) {delete[] (double*)data;     data=NULL;}
}
void C_Matrix_Container::ExportToMatlab(double* out)
{
	// exportuje dane do wskaznika matlaba - do u¿ycia w mexach bo w matylabie macierze s¹ w kolumnach a w Containerach w rzêdach
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	unsigned int r,c;
	unsigned long a=0;
	for(c=0;c<_cols;c++)
		for(r=0;r<_rows;r++)
			out[a++] = this->GetPixel(r,c);

}

void C_Matrix_Container::ImportFromMatlab(double* in,unsigned int row, unsigned int col)
{
	// importuje dane z wskaznika matlaba - do u¿ycia w mexach bo w matylabie macierze s¹ w kolumnach a w Containerach w rzêdach
	unsigned int r,c;
	unsigned long a=0;
	AllocateData(row,col);
	for(c=0;c<_cols;c++)
		for(r=0;r<_rows;r++)
			this->SetPixel(r,c,in[a++]);
}

void C_Matrix_Container::CloneObject(C_Matrix_Container* dest) const
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	dest->_cols = _cols;
	dest->_rows = _rows;
	dest->FreeData();
	dest->AllocateData(dest->_rows,dest->_cols);
	memcpy(dest->data,data,sizeof(double)*dest->_rows*dest->_cols);
}
void C_Matrix_Container::Zeros(void)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	unsigned long a;
	for(a=0;a<GetNumofElements();a++)
		data[a] = 0.0;
}

void C_Matrix_Container::Ones(void)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	unsigned long a;
	for(a=0;a<GetNumofElements();a++)
		data[a] = 1.0;
}

inline unsigned long C_Matrix_Container::GetNumofElements(void) const
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	return _rows*_cols;
}
void C_Matrix_Container::GetPixel(unsigned int row, unsigned int col,double &pixel) const
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
#ifndef _EXTERNALDLL
	_ASSERT(row<_rows);
	_ASSERT(col<_cols);
#endif
	pixel = data[row*_cols+col];
}

inline double C_Matrix_Container::GetPixel(unsigned int row, unsigned int col) const
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
#ifndef _EXTERNALDLL
	_ASSERT(row<_rows);
	_ASSERT(col<_cols);
#endif
	return data[row*_cols+col];
}
unsigned int C_Matrix_Container::coord2lin(unsigned int row, unsigned int col)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	return row*_cols+col;
}

void C_Matrix_Container::SetPixel(unsigned int row, unsigned int col,double pixel)
{	
	/*
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	*/
	_ASSERT( data != NULL );
#ifndef _EXTERNALDLL
	_ASSERT(row<_rows);
	_ASSERT(col<_cols);
#endif
	data[row*_cols+col] = pixel;
}

BOOL C_Matrix_Container::Add(C_Matrix_Container* matrix)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	unsigned long r;
	if(_rows!=matrix->_rows || _cols!=matrix->_cols)
		return false;
	for(r=0;r<GetNumofElements();r++)
		data[r]+=matrix->data[r];
	return true;
}

BOOL C_Matrix_Container::Sub(C_Matrix_Container* matrix)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	unsigned long r;
	if(_rows!=matrix->_rows || _cols!=matrix->_cols)
		return false;
	for(r=0;r<GetNumofElements();r++)
		data[r]-=matrix->data[r];
	return true;
}

BOOL C_Matrix_Container::DotMulti(C_Matrix_Container* matrix)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "DotMulti::Matrix not initialized!!\n");
	if(matrix->data==NULL)
		_RPTF0(_CRT_ASSERT, "DotMulti::Param matrix not initialized!!\n");
	#endif
	unsigned long r;
	if(_rows!=matrix->_rows || _cols!=matrix->_cols)
		return false;
	for(r=0;r<GetNumofElements();r++)
		data[r]*=matrix->data[r];
	return true;
}

void C_Matrix_Container::getMinMax( double& min, double& max ) const
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	unsigned long r;
	min = 1.7e308;
	max = -1.7e308;;
	for(r=0;r<GetNumofElements();r++)	{
		if(data[r]<min)	min = data[r];
		if(data[r]>max) max = data[r];
	}
}
void C_Matrix_Container::Transpose(void)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	C_Matrix_Container tmp;
	tmp.AllocateData(_cols,_rows);
	unsigned long c,r;
	for(c=0;c<_cols;c++)
		for(r=0;r<_rows;r++)
			tmp.SetPixel(c,r,GetPixel(r,c));
	for(c=0;c<tmp.GetNumofElements();c++)
		data[c] = tmp.data[c];
	_rows = tmp._rows;
	_cols = tmp._cols;
}

BOOL C_Matrix_Container::Dump(char *filename)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	FILE *stream;
	unsigned long actrow, actcol;
	double liczba;

	if( fopen_s( &stream, filename, "w" ) == 0 )
	{
		if(data==NULL) {fprintf_s(stream,"Empty Matrix"); return FALSE;}
		fprintf_s(stream,"[");
		for(actrow=0;actrow<_rows;actrow++)
			for(actcol=0;actcol<_cols;actcol++)	{
				GetPixel(actrow,actcol,liczba);
				fprintf_s(stream,"%+030.20f",liczba);
				if(actcol==_cols-1)
					fprintf_s(stream,";\n");
				else
					fprintf_s(stream,",");
			}
		fpos_t position;
		fgetpos(stream,&position);
		position-=3;
		fsetpos(stream,&position);
		fprintf_s(stream,"];");
		fclose( stream );
		return TRUE;
	}
	else		
		return FALSE;
}

BOOL C_Matrix_Container::DumpBinary(char *filename)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	FILE *stream;
	unsigned long actrow;

	if( fopen_s( &stream, filename, "w" ) == 0 )
	{
		if(data==NULL) {fprintf_s(stream,"Empty Matrix"); return FALSE;}
		fprintf_s(stream,"%ld\n",_rows);
		fprintf_s(stream,"%ld\n",_cols);
		for(actrow=0;actrow<GetNumofElements();actrow++)
			fprintf_s(stream,"%+030.20f\n",data[actrow]);
			
		fclose( stream );
		return TRUE;
	}
	else		
		return FALSE;
}

BOOL C_Matrix_Container::ReadBinary(const char *filename)
{
	FILE *stream;
	unsigned int rows,cols;
	//float tmp;

	if( fopen_s( &stream, filename, "rb" ) == 0 )
	{
		fread(&rows, 4, 1,stream);	
		fread(&cols, 4, 1,stream);
		C_Matrix_Container::AllocateData(rows,cols);
		fread(data,8,rows*cols,stream);
		fclose( stream );
		return TRUE;
	}
	else	{
		#ifdef _DEBUG
		if(data==NULL)	{
			 char debugtab[512];
			 strcpy_s(debugtab,512,"File not opened correctly!! ");
			 strcat_s(debugtab,512,filename);
			_RPTF0(_CRT_ASSERT, debugtab);
		}
		#endif
		return FALSE;
	}
}


double C_Matrix_Container::Mean(C_Matrix_Container* out,unsigned int col)
// liczy œredni¹ z kolumny col.
// Jeœli *out = NULL to liczy z kolumny col i zwraca
// W przeciwnym wypadku wynik jest umieszczany w *out, col jest ignorowane a funkcja zwraca 0
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	unsigned long r,c;
	if(out==NULL)	{
		double suma=0,srednia=0;
		for(r=0;r<_rows;r++)
			srednia+=GetPixel(r,col);
		srednia/=_rows;
		return srednia;
	} else {
		out->AllocateData(1,_cols);
		for(c=0;c<_cols;c++)	{
			double suma=0,srednia=0;
			for(r=0;r<_rows;r++)
				srednia+=GetPixel(r,c);
			srednia/=_rows;
			out->data[c] = srednia;
		}
		return 0;
	}
}

void C_Matrix_Container::stdcol(C_Matrix_Container* output)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
// oblicza std po wszystkich kolumnach input
	unsigned long a,numofelem,col;
	double srednia;
	double suma;
	output->AllocateData(1,_cols);
	for(col=0;col<_cols;col++)	{
		srednia = 0;
		suma = 0;
		numofelem = _rows;		// ilosc eklementow w kolumnia
		if(numofelem==1)	{
			output->SetPixel(0,col,0);
			continue;
		}
		for(a=0;a<numofelem;a++)
			srednia+=GetPixel(a,col);
		srednia/=numofelem;
	
		for(a=0;a<numofelem;a++)
			suma+=pow( (GetPixel(a,col) - srednia),2 );
		suma = sqrt(suma/(numofelem-1));
		output->SetPixel(0,col,suma);
	}
}

void C_Matrix_Container::std(C_Matrix_Container* output)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
// oblicza std po wszystkich danych
	unsigned long a;
	double srednia;
	double suma;
	output->AllocateData(1,1);
	if(GetNumofElements()==1)	{
		output->SetPixel(0,0,0);
		return;
	}
	srednia = 0;
	suma = 0;
	for(a=0;a<GetNumofElements();a++)
		srednia+=data[a];
	srednia/=GetNumofElements();
	
	for(a=0;a<GetNumofElements();a++)
		suma+=pow( (data[a] - srednia),2 );
	suma = sqrt(suma/(GetNumofElements()-1));
	output->SetPixel(0,0,suma);
}

double C_Matrix_Container::std(void)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
// oblicza std po wszystkich danych
	unsigned long a;
	double srednia;
	double suma;
	double stdtmp;
	if(GetNumofElements()==1)	{
		stdtmp = 0;
		return stdtmp;
	}
	srednia = 0;
	suma = 0;
	for(a=0;a<GetNumofElements();a++)
		srednia+=data[a];
	srednia/=GetNumofElements();
	
	for(a=0;a<GetNumofElements();a++)
		suma+=pow( (data[a] - srednia),2 );
	suma = sqrt(suma/(GetNumofElements()-1));
	stdtmp = suma;
	return stdtmp;
}

double C_Matrix_Container::Median(void)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	// zwraca medianê ze wszystkich danych - dane raczej jako pojedyñczy wektor
	int sr;
	C_Matrix_Container tmp;
	CloneObject(&tmp);
	quickSort(tmp.data,0,tmp.GetNumofElements()-1);
//tmp.Dump("tmp.m");
	if(tmp.GetNumofElements()%2==0) {
		sr = tmp.GetNumofElements()/2;
		return (tmp.data[sr-1]+tmp.data[sr])/2;
	}
	else	{
		sr = (tmp.GetNumofElements()+1)/2;
		return tmp.data[sr-1];
	}
}



/*
 *  This Quickselect routine is based on the algorithm described in
 *  "Numerical recipes in C", Second Edition,
 *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
 *  This code by Nicolas Devillard - 1998. Public domain.
 */
#define ELEM_SWAP(a,b) { register double t=(a);(a)=(b);(b)=t; }
// uwaga - modyfikuje tablicê !! dla parzystych zwraca ni¿szy index
double C_Matrix_Container::quick_select() 
{
    int low, high ;
    int median;
    int middle, ll, hh;
	int n = (int)GetNumofElements();
    low = 0 ; high = n-1 ; median = (low + high) / 2;
    for (;;) {
        if (high <= low) /* One element only */
            return data[median] ;

        if (high == low + 1) {  /* Two elements only */
            if (data[low] > data[high])
                ELEM_SWAP(data[low], data[high]) ;
            return data[median] ;
        }

    /* Find median of low, middle and high items; swap into position low */
    middle = (low + high) / 2;
    if (data[middle] > data[high])    ELEM_SWAP(data[middle], data[high]) ;
    if (data[low] > data[high])       ELEM_SWAP(data[low], data[high]) ;
    if (data[middle] > data[low])     ELEM_SWAP(data[middle], data[low]) ;

    /* Swap low item (now in position middle) into position (low+1) */
    ELEM_SWAP(data[middle], data[low+1]) ;

    /* Nibble from each end towards middle, swapping items when stuck */
    ll = low + 1;
    hh = high;
    for (;;) {
        do ll++; while (data[low] > data[ll]) ;
        do hh--; while (data[hh]  > data[low]) ;

        if (hh < ll)
        break;

        ELEM_SWAP(data[ll], data[hh]) ;
    }

    /* Swap middle item (in position low) back into correct position */
    ELEM_SWAP(data[low], data[hh]) ;

    /* Re-set active partition */
    if (hh <= median)
        low = ll;
        if (hh >= median)
        high = hh - 1;
    }
}

#undef ELEM_SWAP


void C_Matrix_Container::CutMatrixCol(C_Matrix_Container* output, C_Matrix_Container* cols)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
// wycina z input kolumny o indeksach kolumny
	C_Matrix_Container tmp;
	unsigned long r,c,kol;
	if(cols->_cols!=1 && cols->_rows!=1)
		return;
	if(cols->_cols==1)
		kol = cols->_rows;
	if(cols->_rows==1)
		kol = cols->_cols;
	tmp.AllocateData(_rows,kol);
	for(c=0;c<kol;c++)
		for(r=0;r<_rows;r++)
			tmp.SetPixel(r,c,GetPixel(r,(unsigned long)cols->data[c]));
	tmp.CloneObject(output);

}

void C_Matrix_Container::CutMatrixRow(C_Matrix_Container* output, C_Matrix_Container* rows)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
// wycina rzedy o indeksach rows i umieszcza je w output. Oryginalan macierz nie jest zmienianan
	C_Matrix_Container tmp;
	unsigned long r,c,kol;
	if(rows->_cols!=1 && rows->_rows!=1)
		return;
	if(rows->_cols==1)
		kol = rows->_rows;
	if(rows->_rows==1)
		kol = rows->_cols;
	tmp.AllocateData(kol,_cols);
	for(c=0;c<_cols;c++)
		for(r=0;r<kol;r++)
			tmp.SetPixel(r,c,GetPixel((unsigned long)rows->data[r],c));
	tmp.CloneObject(output);

}

void C_Matrix_Container::RemoveMatrixRow(C_Matrix_Container* index)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	// usuwa elementy z in o indexach z index
	C_Matrix_Container tmp;
	unsigned long licznik = 0,ilerazy,r,c;
	
	ilerazy = index->GetNumofElements();
	if(index->_cols>1 && index->_rows>1)
		return;
	if((GetNumofElements()-ilerazy)<1)	{
		FreeData();
		return;
	}
	tmp.AllocateData(_rows-ilerazy,_cols);

	for(c=0;c<_cols;c++)	{
		licznik = 0;
		for(r=0;r<_rows;r++)
			if(index->iselement(r)==0)	{
				tmp.SetPixel(licznik,c,this->GetPixel(r,c));
				licznik++;
			}
	}
	tmp.CloneObject(this);
}

void C_Matrix_Container::RemoveMatrixCol(C_Matrix_Container* index)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	// usuwa elementy z in o indexach z index
	C_Matrix_Container tmp;
	unsigned long licznik = 0,ilerazy,r,c;
	
	ilerazy = index->GetNumofElements();
	if(index->_cols>1 && index->_rows>1)
		return;
	if((GetNumofElements()-ilerazy)<1)	{
		FreeData();
		return;
	}
	tmp.AllocateData(_rows,_cols-ilerazy);

	for(r=0;r<_rows;r++)	{
		licznik = 0;
		for(c=0;c<_cols;c++)
			if(index->iselement(c)==0)	{
				tmp.SetPixel(r,licznik,this->GetPixel(r,c));
				licznik++;
			}
	}
	tmp.CloneObject(this);
}

unsigned long C_Matrix_Container::iselement(double element)
{
	#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	#endif
	// ile razy wystepuje element w tablicy in
	unsigned long a,ile=0;
	for(a=0;a<GetNumofElements();a++)
		if(data[a]==element)
			ile++;
	return ile;
}

void C_Matrix_Container::quickSort( double a[], int l, int r)
{
   int j;

   if( l < r ) 
   {
   	// divide and conquer
        j = partition( a, l, r);
       quickSort( a, l, j-1);
       quickSort( a, j+1, r);
   }
	
}

int C_Matrix_Container::partition( double a[], int l, int r)
{
   int i, j;
   double pivot,t;
   pivot = a[l];
   i = l; j = r+1;
		
   while( 1)
   {
   	do ++i; while( a[i] <= pivot && i <= r );
   	do --j; while( a[j] > pivot );
   	if( i >= j ) break;
   	t = a[i]; a[i] = a[j]; a[j] = t;
   }
   t = a[l]; a[l] = a[j]; a[j] = t;
   return j;
}

void C_Matrix_Container::CopyfromTab( const double *src,unsigned int size_src )
{
#ifdef _DEBUG
	if(data==NULL)
		_RPTF0(_CRT_ASSERT, "Matrix not initialized!!\n");
	if(size_src>GetNumofElements())
		_RPTF0(_CRT_ASSERT, "CopyfromTab::Wrong size!!\n");
#endif
	for(unsigned int a=0;a<size_src;++a)
		data[a] = src[a];
}
