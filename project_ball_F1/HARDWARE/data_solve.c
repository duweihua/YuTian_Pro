#include "data_solve.h"

extern float X;
extern float Y;

void Place_Data_Slove(unsigned char *PC_DATA)
{
	X = ((u16)PC_DATA[0]<<8)|(u16)PC_DATA[1];
	Y = ((u16)PC_DATA[2]<<8)|(u16)PC_DATA[3];
}



