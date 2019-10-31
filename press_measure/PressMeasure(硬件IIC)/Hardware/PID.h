#ifndef __PID_H
#define __PID_H

#include "main.h"

typedef struct PID
{
	float SetPoint;			
	
	float P;				
	float I;				
	float D;				
	
	float LastError;		
	float PreError;			
	float SumError;			
	float dError;

	float IMax;				
	
	float POut;				
	float IOut;				
	float DOut;				
	
}PID;			


void PID1_Init(void);
void PID2_Init(void);

float PID_Calc(PID *P,float ActualValue);

#endif
