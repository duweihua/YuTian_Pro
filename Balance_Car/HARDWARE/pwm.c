#include "main.h"


//int Deadzone1 = 230-15;
//int Deadzone2 = 210-15;

//比较直
int Deadzone1 = 300;
int Deadzone2 = 300;//240

//int Deadzone1 = 400;
//int Deadzone2 = 400;

void pwm_init(void)
{  
	GPIO_InitTypeDef 			gpio;
	TIM_TimeBaseInitTypeDef 	base;
	TIM_OCInitTypeDef  			oc;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 

	//PWM
	gpio.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; 
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;  
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio);
	
	//AIN1+2
	gpio.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5; 
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;  
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio);
	
	//BIN1+2
	gpio.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11; 
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;  
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gpio);

	base.TIM_Period = 7200-1; 
	base.TIM_Prescaler = 1-0; 
	base.TIM_ClockDivision = 1-0;
	base.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3,&base); 

	oc.TIM_OCMode = TIM_OCMode_PWM1; 
	oc.TIM_OutputState = TIM_OutputState_Enable;
	oc.TIM_Pulse = 3600; 
	oc.TIM_OCPolarity = TIM_OCPolarity_High; 	
	
	TIM_OC1Init(TIM3,&oc); 
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);  

	TIM_OC2Init(TIM3,&oc); 
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);  

	TIM_CtrlPWMOutputs(TIM3,ENABLE);
	
	TIM_Cmd(TIM3,ENABLE);  
}


int myabs(int a)
{ 		   
	int temp;

	if(a<0)  
		temp=-a;  	
	else 
		temp=a;
	
	return temp;
}


void Set_Pwm(int motor1,int motor2)
{
	//左右轮死区(略不相同)
//	int Deadzone1 = 275;
//	int Deadzone2 = 200;

//	int Deadzone1 = 255;
//	int Deadzone2 = 240;

	if(motor1<0)
	{
		AIN2 = 1;
		AIN1 = 0;
	}
	else if(motor1>0)
	{
		AIN2 = 0;
		AIN1 = 1;
	}
	else
	{
		AIN2 = 0;
		AIN1 = 0;
	}
	
	Motor1_Pwm = myabs(motor1)+Deadzone1;
	
	if(motor2<0)
	{
		BIN1 = 1;
		BIN2 = 0;
	}
	else if(motor2>0)
	{
		BIN1 = 0;
		BIN2 = 1;
	}
	else
	{
		BIN1 = 0;
		BIN2 = 0;
	}
	
	Motor2_Pwm = myabs(motor2)+Deadzone2;	
}





