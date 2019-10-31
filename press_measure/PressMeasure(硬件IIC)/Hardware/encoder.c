#include "main.h"

int EncoderCircle;

extern short pos1;
extern short pos2;

extern short speed1;
extern short speed2;

extern short lastspeed2;


//TIM2 Encoder mode config
void Encoder1_Init(void)
{
	GPIO_InitTypeDef 			gpio;
//	NVIC_InitTypeDef 			nvic;
	TIM_TimeBaseInitTypeDef		base;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	//io
	gpio.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio);
    
	//interupt
//	nvic.NVIC_IRQChannel = TIM2_IRQn;
//	nvic.NVIC_IRQChannelPreemptionPriority = 0;
//	nvic.NVIC_IRQChannelSubPriority = 0;
//	nvic.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&nvic);

	//timebase
    base.TIM_Period = 0xFFFF;
    base.TIM_Prescaler = (72-1);
    base.TIM_ClockDivision =TIM_CKD_DIV1;
    base.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM2,&base);
	
	TIM2->CNT = 0x7FFF;
	
	TIM_EncoderInterfaceConfig(TIM2,TIM_EncoderMode_TI12,TIM_ICPolarity_BothEdge,TIM_ICPolarity_BothEdge);
	
	TIM_Cmd(TIM2, ENABLE);
}


//TIM3 Encoder mode config
void Encoder2_Init(void)
{
	GPIO_InitTypeDef 			gpio;
//	NVIC_InitTypeDef 			nvic;
	TIM_TimeBaseInitTypeDef		base;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	//io
	gpio.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio);
    
	//interupt
//	nvic.NVIC_IRQChannel = TIM3_IRQn;
//	nvic.NVIC_IRQChannelPreemptionPriority = 0;
//	nvic.NVIC_IRQChannelSubPriority = 0;
//	nvic.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&nvic);

	//timebase
    base.TIM_Period = 0xFFFF;
    base.TIM_Prescaler = (72-1);
    base.TIM_ClockDivision =TIM_CKD_DIV1;
    base.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3,&base);
	
	TIM3->CNT = 0x7FFF;
	
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_BothEdge,TIM_ICPolarity_BothEdge);
	
	TIM_Cmd(TIM3, ENABLE);
}


void Encoder_init(void)
{
	Encoder1_Init();
	Encoder2_Init();
}


void Encoder1_read(void)
{
	short count = 0;
	count = (TIM2->CNT-0x7FFF);
	TIM2->CNT = 0x7FFF;
	pos1 += count;
	speed1 = count;
//	return count;
}


void Encoder2_read(void)
{
	short count = 0;
	
	lastspeed2 = speed2;
	
	count = (TIM3->CNT-0x7FFF);
	TIM3->CNT = 0x7FFF;
	
	pos2 += count;
	
	speed2 = count;
//	return count;
}








