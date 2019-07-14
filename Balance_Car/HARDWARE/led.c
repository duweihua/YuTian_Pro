#include "main.h"


void led_init(void)
{
	GPIO_InitTypeDef gpio;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	gpio.GPIO_Mode=GPIO_Mode_Out_PP;
	gpio.GPIO_Pin=GPIO_Pin_15;
	gpio.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC,&gpio);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_15);	
}






