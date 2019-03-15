#ifndef __C_Error__
#define __C_Error__

#define maxerror 256
#include <string.h>

// Ver 1.1 by PB

class C_Error
{
public:
	C_Error(void);
	C_Error(char* perror);
//	char* error;
	char error[maxerror];
	char user[maxerror];
	void SetError(char* perror);
	void SetUserData(char* pdata);
	bool status;		// FALSE - bez errorów
	bool userdata;		// FALSE - bez danych
public:
	~C_Error(void);
};

#endif