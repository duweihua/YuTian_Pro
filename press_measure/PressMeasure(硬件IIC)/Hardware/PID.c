#include "main.h"


extern PID pid_Position1;
extern PID pid_Speed1;
extern PID pid_Current1;

extern PID pid_Position2;
extern PID pid_Speed2;
extern PID pid_Current2;


float PID_Calc(PID *P, float ActualValue)
{
	P->PreError = P->SetPoint - ActualValue;
	P->dError = P->PreError - P->LastError;

	P->SumError += P->PreError;
	P->LastError = P->PreError;
	
	if(P->SumError >= P->IMax)
		P->SumError = P->IMax;
	else if(P->SumError <= -P->IMax)
		P->SumError = -P->IMax;
	
	P->POut = P->P * P->PreError;
	P->IOut = P->I * P->SumError;
	P->DOut = P->D * P->dError;
	
	return P->POut+P->IOut+P->DOut;
}


void PID1_Init(void)
{
	pid_Position1.P = 0.04f;
	pid_Position1.I = 0.002f;
	pid_Position1.D = 0.01f;
	pid_Position1.IMax = 1000.0f;
	pid_Position1.SetPoint =0.0f;

	pid_Speed1.P = 25.0f;
	pid_Speed1.I = 7.5f;
	pid_Speed1.D = 2.5f;
	pid_Speed1.IMax = 1000.0f;
	pid_Speed1.SetPoint =0.0f;
	
	pid_Current1.P = 0.0f;
	pid_Current1.I = 0.0f;
	pid_Current1.D = 0.0f;
	pid_Current1.IMax = 1000.0f;
	pid_Current1.SetPoint =0.0f;	
}


void PID2_Init(void)
{
	pid_Position2.P = 0.18f;//1.6f;
	pid_Position2.I = 0.0f;
	pid_Position2.D = 0.0f;
	pid_Position2.IMax = 1000.0f;
	pid_Position2.SetPoint =0.0f;

	pid_Speed2.P = 3.0f;//2.0f;
	pid_Speed2.I = 0.18f;//-0.8f;
	pid_Speed2.D = 1.2f;//0.0f;
	pid_Speed2.IMax = 1000.0f;
	pid_Speed2.SetPoint =0.0f;

	pid_Current2.P = -1.35;//-0.65f;
	pid_Current2.I = -2.0f;//-2.5f;//-2.51f;//-1.45f;
	pid_Current2.D = -0.1f;//0.0f;
	pid_Current2.IMax = 200.0f;
	pid_Current2.SetPoint =0.0f;	
}




