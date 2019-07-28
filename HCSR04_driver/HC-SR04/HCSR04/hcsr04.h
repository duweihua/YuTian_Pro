#ifdef __HCSR04_H
#define __HCSR04_H

#include "sys.h"

void HCSR_Init(void);       

void HCSR_Strat(void);      

float Hcsr04GetLength(void);

void TIM3_LJW_Init(void);

void LED_Init(void);

#endif


