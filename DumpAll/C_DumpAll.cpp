#include "C_DumpAll.h"

C_DumpAll::C_DumpAll(char* filename)
{
#ifdef _DEBUG
	unsigned int a;
	unsigned long data=0;
	if( fopen_s( &stream, filename, "wb" ) == 0 )
	{
		openfileerror = false;
		lastpozindex = 0;
		fwrite(&data,sizeof(unsigned int),1,stream);	// ilosc wpisów
		for(a=0;a<MAX_ENTRY;a++)
			fwrite(&data,sizeof(unsigned long),1,stream); // przesuwanie wskaznika pliku
	} else	{
		_CrtDbgReport(_CRT_ASSERT, NULL, NULL, "C_DumpAll: Cant open file", NULL);
		openfileerror = true;
	}
#endif
}

void C_DumpAll::AddEntry(C_Matrix_Container* data, char* name)
{
	unsigned int type = C_MATRIX;
	long position,ile;
	unsigned int sl;
	if(openfileerror)
	{
		#ifdef _DEBUG
		_CrtDbgReport(_CRT_ASSERT, NULL, NULL, "C_DumpAll: File not opened", NULL);
		#endif
	}
	if(lastpozindex>=MAX_ENTRY)	{
		#ifdef _DEBUG
		_CrtDbgReport(_CRT_ASSERT, NULL, NULL, "C_DumpAll: File not opened", NULL);
		#endif
		return;
	}
	sl = (unsigned int)strlen(name);
	position = ftell(stream);
	fwrite(&type,sizeof(unsigned int),1,stream);	// typ

	fwrite(&data->_rows,sizeof(unsigned int),1,stream);	// rozmiar r
	fwrite(&data->_cols,sizeof(unsigned int),1,stream);	// rozmiar k
	fwrite(data->data,sizeof(double),data->GetNumofElements(),stream);		// dane
	fwrite(&sl,sizeof(unsigned int),1,stream);		// rozmiar nazwy
	fwrite(name,sizeof(char),sl,stream);			// nazwa
	ile = ftell(stream) - position;
	offset[lastpozindex] = (unsigned long)ile; //ilosc zzpisanych bajtów
	lastpozindex++;
}

void C_DumpAll::AddEntry(C_Image_Container* data, char* name)
{
	unsigned int type = C_IMAGE;
	long position,ile;
	unsigned int sl;
	if(openfileerror)
	{
		#ifdef _DEBUG
		_CrtDbgReport(_CRT_ASSERT, NULL, NULL, "C_DumpAll: File not opened", NULL);
		#endif
	}
	if(lastpozindex>=MAX_ENTRY)	{
		#ifdef _DEBUG
		_CrtDbgReport(_CRT_ASSERT, NULL, NULL, "C_DumpAll: File not opened", NULL);
		#endif
		return;
	}
	sl = (unsigned int)strlen(name);
	position = ftell(stream);
	fwrite(&type,sizeof(unsigned int),1,stream);	// typ

	fwrite(&data->_rows,sizeof(unsigned int),1,stream);	// rozmiar r
	fwrite(&data->_cols,sizeof(unsigned int),1,stream);	// rozmiar k
	fwrite(data->data,sizeof(double),data->GetNumofElements(),stream);		// dane
	fwrite(&sl,sizeof(unsigned int),1,stream);		// rozmiar nazwy
	fwrite(name,sizeof(char),sl,stream);			// nazwa
	ile = ftell(stream) - position;
	offset[lastpozindex] = (unsigned long)ile; //ilosc zzpisanych bajtów
	lastpozindex++;
}

void C_DumpAll::AddEntry(double* data,unsigned int size, char* name)
{
	unsigned int type = DOUBLE;
	long position,ile;
	unsigned int sl;
	if(openfileerror)
	{
		#ifdef _DEBUG
		_CrtDbgReport(_CRT_ASSERT, NULL, NULL, "C_DumpAll: File not opened", NULL);
		#endif
	}
	if(lastpozindex>=MAX_ENTRY)	{
		#ifdef _DEBUG
		_CrtDbgReport(_CRT_ASSERT, NULL, NULL, "C_DumpAll: File not opened", NULL);
		#endif
		return;
	}
	sl = (unsigned int)strlen(name);
	position = ftell(stream);
	fwrite(&type,sizeof(unsigned int),1,stream);	// typ

	fwrite(&size,sizeof(unsigned int),1,stream);	// rozmiar
	fwrite(data,sizeof(double),size,stream);		// dane
	fwrite(&sl,sizeof(unsigned int),1,stream);		// rozmiar nazwy
	fwrite(name,sizeof(char),sl,stream);			// nazwa
	ile = ftell(stream) - position;
	offset[lastpozindex] = (unsigned long)ile; //ilosc zzpisanych bajtów
	lastpozindex++;
}

void C_DumpAll::AddEntry(float* data,unsigned int size, char* name)
{
	unsigned int type = FLOAT;
	long position,ile;
	unsigned int sl;
	if(openfileerror)
	{
		#ifdef _DEBUG
		_CrtDbgReport(_CRT_ASSERT, NULL, NULL, "C_DumpAll: File not opened", NULL);
		#endif
	}
	if(lastpozindex>=MAX_ENTRY)	{
		#ifdef _DEBUG
		_CrtDbgReport(_CRT_ASSERT, NULL, NULL, "C_DumpAll: File not opened", NULL);
		#endif
		return;
	}
	sl = (unsigned int)strlen(name);
	position = ftell(stream);
	fwrite(&type,sizeof(unsigned int),1,stream);	// typ

	fwrite(&size,sizeof(unsigned int),1,stream);	// rozmiar
	fwrite(data,sizeof(float),size,stream);		// dane
	fwrite(&sl,sizeof(unsigned int),1,stream);		// rozmiar nazwy
	fwrite(name,sizeof(char),sl,stream);			// nazwa
	ile = ftell(stream) - position;
	offset[lastpozindex] = (unsigned long)ile; //ilosc zzpisanych bajtów
	lastpozindex++;
}

C_DumpAll::~C_DumpAll(void)
{
#ifdef _DEBUG	
	unsigned int a;
	fseek(stream,0,SEEK_SET);
	fwrite(&lastpozindex,sizeof(unsigned int),1,stream);	// ilosc wpisów
	for(a=0;a<lastpozindex;a++)
		fwrite(&offset[a],sizeof(unsigned long),1,stream);

	fclose( stream );
#endif
}
