#ifndef __SENSOR_H
#define __SENSOR_H


//∫Í∂®“Â
#define Sensor_Gpio GPIOA

#define DATA GPIO_Pin_0
#define CLK  GPIO_Pin_1

#define Sensor_Clock RCC_APB2Periph_GPIOA


void Sensor_Init(void);
unsigned long Sensor_Read(void);
void test_mode(void);
void Get_No_Lode(void);
void Get_Weight(void);


#endif
