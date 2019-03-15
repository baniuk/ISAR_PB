#include "C_Error.h"
// Ver 1.1 by PB

C_Error::C_Error(void)
{
	status = false;
	userdata = false;
}
C_Error::C_Error(char* perror)
{
//	error = perror;
	strcpy_s(error,maxerror,perror);
	status = true;
	userdata = false;
}

C_Error::~C_Error(void)
{
}
void C_Error::SetError(char *perror)
{
//	error = perror;
	strcpy_s(error,maxerror,perror);
	status = true;
}

void C_Error::SetUserData(char *perror)
{
	strcpy_s(user,maxerror,perror);
	userdata = true;
}
