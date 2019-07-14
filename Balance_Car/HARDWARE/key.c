#include "main.h"


//外部中断初始化函数
void key_init(void)
{
	GPIO_InitTypeDef	gpio;	
	EXTI_InitTypeDef 	exti;
	NVIC_InitTypeDef 	nvic;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	gpio.GPIO_Pin  = GPIO_Pin_14;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
 	GPIO_Init(GPIOC, &gpio);

	//GPIOC.14
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource14);

	exti.EXTI_Line=EXTI_Line14;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;	
	//上升沿触发(松手生效)
	exti.EXTI_Trigger = EXTI_Trigger_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);	

	//优先级问题隐患
	nvic.NVIC_IRQChannel = EXTI15_10_IRQn;			
	nvic.NVIC_IRQChannelPreemptionPriority = 0;	
	nvic.NVIC_IRQChannelSubPriority = 0;				
	nvic.NVIC_IRQChannelCmd = ENABLE;						
	NVIC_Init(&nvic);  	  
}


void EXTI15_10_IRQHandler(void)
{
	delay_ms(10);  		 
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)==0)
	{

	}
	EXTI_ClearITPendingBit(EXTI_Line14);
}






