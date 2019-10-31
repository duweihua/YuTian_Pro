#include "main.h"


//PWM输出初始化
//PB6
//PB7
//PB8
//PB9
void TIM4_PWM_Init(void)
{
	GPIO_InitTypeDef 			gpio;
	TIM_TimeBaseInitTypeDef		base;
	TIM_OCInitTypeDef  			oc;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	//TIM4_CH1
	gpio.GPIO_Pin = GPIO_Pin_6; 
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;  
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gpio);
	
	//TIM4_CH2
	gpio.GPIO_Pin = GPIO_Pin_7; 
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;  
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gpio);

	//TIM4_CH3
	gpio.GPIO_Pin = GPIO_Pin_8; 
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;  
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gpio);

	//TIM4_CH4
	gpio.GPIO_Pin = GPIO_Pin_9; 
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;  
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gpio);

	//72MHz 2分频
	base.TIM_Period = 2000-1;//20KHz//2000-1; 
	base.TIM_Prescaler = 2-1;
	base.TIM_ClockDivision = 0;
	base.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM4,&base); 

	oc.TIM_OCMode = TIM_OCMode_PWM1; 
	oc.TIM_OutputState = TIM_OutputState_Enable;
	oc.TIM_Pulse = 0;//0;
	oc.TIM_OCPolarity = TIM_OCPolarity_High; 
	
	TIM_OC1Init(TIM4, &oc); 
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);  
	TIM_OC2Init(TIM4, &oc); 
	TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);  
	TIM_OC3Init(TIM4, &oc); 
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);  
	TIM_OC4Init(TIM4, &oc); 
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);  

	TIM_CtrlPWMOutputs(TIM4,ENABLE);
	
	TIM_Cmd(TIM4, ENABLE);  
}


void TIM4_PWM_OUTPUT(u16 DR1,u16 DR2,u16 DR3,u16 DR4)
{
	TIM_SetCompare1(TIM4,DR1);
	TIM_SetCompare2(TIM4,DR2);
	TIM_SetCompare3(TIM4,DR3);
	TIM_SetCompare4(TIM4,DR4);
}





