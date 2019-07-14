#ifndef __MAIN_H
#define __MAIN_H


#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "delay.h"
#include "led.h"
#include "pwm.h"
#include "encoder.h"
#include "PID.h"
#include "can.h"
#include "adc.h"
#include "control.h"
#include "usart.h"
//#include "oled_spi.h"
#include "oled_iic.h"


#define ABS(x) ((x)>0? (x):(-(x))) 
#define LIMIT_MAX_MIN(x, max, min)	(((x) <= (min)) ? (min):(((x) >= (max)) ? (max) : (x)))

void system_Init(void);
void data_init(void);

#endif



