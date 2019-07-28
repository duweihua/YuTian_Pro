#include "stm32f10x.h"
#include "hcsr04.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

#define Trig GPIO_Pin_4

#define Echo GPIO_Pin_6

float Distance;

void HCSR_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitSture;
	EXTI_InitTypeDef  EXTI_InitSture;
	NVIC_InitTypeDef  NVIC_InitSture;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA,ENABLE);
	
	
	//配置IO端口
	GPIO_InitSture.GPIO_Mode=GPIO_Mode_Out_PP;   
	GPIO_InitSture.GPIO_Pin=Trig;                //将PA4于Trig相连
	GPIO_InitSture.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA,&GPIO_InitSture);
	
	GPIO_InitSture.GPIO_Mode=GPIO_Mode_IPD;      
	GPIO_InitSture.GPIO_Pin=Echo;                //将PA6于Echo相连
	GPIO_InitSture.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA,&GPIO_InitSture);
	
	//中断和6端口映射一起
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource6);
	
	//外部中断配置
	EXTI_InitSture.EXTI_Line=EXTI_Line6;
	EXTI_InitSture.EXTI_LineCmd=ENABLE;
	EXTI_InitSture.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitSture.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitSture);
	
	//中断优先级管理
	NVIC_InitSture.NVIC_IRQChannel=EXTI9_5_IRQn;
	NVIC_InitSture.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitSture.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitSture.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitSture);
}

void EXTI9_5_IRQHandler(void)
{
	delay_us(10);
	
	if(EXTI_GetITStatus(EXTI_Line6)!=RESET)
	{
		TIM_SetCounter(TIM3,0);
		TIM_Cmd(TIM3,ENABLE);
		
		while(GPIO_ReadInputDataBit(GPIOA,Echo));  //等待低电平
		
		TIM_Cmd(TIM3,DISABLE);
		
		Distance=TIM_GetCounter(TIM3)*34/2000;
		
		TIM_SetCounter(TIM3,0);
		
		if(Distance>0)
		{
			printf("Distance:%0.1f cm\r\n",Distance);
		}
			
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
}


void HCSR_Strat(void)
{
	GPIO_SetBits(GPIOA,Trig);   //将Trig设置为高电平
	delay_us(20);               //持续大于10us触发，触发超声波模块工作
	GPIO_ResetBits(GPIOA,Trig); 
}


void TIM3_LJW_Init() //TIM3初始化
{
//	NVIC_InitTypeDef NVIC_Struct;                          //定义NVIC结构体
	TIM_TimeBaseInitTypeDef TIM_Struct;                   //定义TIM结构体
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 ,ENABLE);   //初始化TIM3时钟
	
	TIM_Struct.TIM_ClockDivision=TIM_CKD_DIV1;             //这里用不到 随便写的
	TIM_Struct.TIM_CounterMode=TIM_CounterMode_Up;         //向上计数
	TIM_Struct.TIM_Period=65535;                            //重装载值4999
                                                                        // 72M/(7199+1)=10000->0.1ms * (4999+1) = 500ms   
                                                                        // (PSC+1)(ARR+1)/CK_PSC = 时间
	TIM_Struct.TIM_Prescaler=71;                         //预分频值7199         
	TIM_TimeBaseInit(TIM3,&TIM_Struct);                    //初始化TIM3
}








