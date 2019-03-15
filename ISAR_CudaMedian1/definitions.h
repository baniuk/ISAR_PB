typedef struct  
{
	unsigned int width;
	unsigned int height;
	float* data;
	size_t pitch;
} Matrix;

typedef struct  
{
	unsigned int wx;
	unsigned int wy;
	unsigned int px;	// po�owa d�ugosci okna
	unsigned int py;
	unsigned int e;	// ilosc element�w
	unsigned int cenr;	// elemnt srodkowy w liniowym zapisie (np 13 przy oknie 5x5)
} Mask;