#include "key.h"
#include "delay.h"

extern unsigned char state_global;

//现在是PB8
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag,使能SWD
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
} 


//外部中断初始化函数
void KEY_EXTI_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	KEY_Init();

	//GPIOB.8
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);

	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//上升沿(松手触发)
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	

	//优先级问题隐患
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;				
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						
	NVIC_Init(&NVIC_InitStructure);  	  
}


void EXTI9_5_IRQHandler(void)
{
	delay_ms(10);  		 
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==0)
	{
		state_global++;
		if(state_global>4)
			state_global=1;
	}
		EXTI_ClearITPendingBit(EXTI_Line8);
}






