#ifndef __C_Image_Container__
#define __C_Image_Container__

#include "c_matrix_container.h"
// Ver 1.3.1 by PB

class C_Image_Container :
	public C_Matrix_Container
{
public:
	C_Image_Container(void);
	void FreeData(void);
	char* ReturnIPLBuffor(unsigned short w1, unsigned short w2);
	char* ReturnIPLBuffor(void);
	void CloneObject(C_Image_Container* dest);
//	void InheritProperties(C_Image_Container* dest);
	void Normalize(unsigned short w1, unsigned short w2);
	void Hist(unsigned int val,C_Matrix_Container &out);
	BOOL isBinary;
	unsigned long xorigin, yorigin;
	virtual ~C_Image_Container(void);
	void CreateHeaderBuffor(unsigned int size);
	char* header;
	unsigned int header_size;
private:
	char* _iplbuffor;
};

#endif