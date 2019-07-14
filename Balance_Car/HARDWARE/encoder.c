#include "main.h"


void encoder1_init(void)
{
	GPIO_InitTypeDef 			gpio;	
	TIM_TimeBaseInitTypeDef 	base;  
	TIM_ICInitTypeDef 			ic;  
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	gpio.GPIO_Pin = GPIO_Pin_0;				
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOA,&gpio);					

	gpio.GPIO_Pin = GPIO_Pin_1;				
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOA,&gpio);					

	TIM_TimeBaseStructInit(&base);
	base.TIM_Prescaler = 0; 
	base.TIM_Period = 0XFFFF; 		
	base.TIM_ClockDivision = TIM_CKD_DIV1;		
	base.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&base);
	
	TIM_EncoderInterfaceConfig(TIM2,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	
	TIM_ICStructInit(&ic);
	//输入捕获滤波系数：
	//	作用：定时器时钟经时钟分频因子(TIM_ClockDivision)分频后得到滤波器采样时钟(fSAMPLING)
	//		  且配置数字滤波器的采样分频为N(TIM_ICFilter)
	//		  则滤波的长度：t = 1/fSAMPLING*N
	//		  这样捕获通道的滤波器就会滤掉低 t 的脉冲干扰
	//		  也就是说滤波器会以fSAMPLING的频率采样8次
	//		  如果8次都为高电平则确定为上升沿触发
	ic.TIM_ICFilter = 10;
	TIM_ICInit(TIM2,&ic);
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

	TIM_SetCounter(TIM2,0);
	TIM_Cmd(TIM2,ENABLE); 
}


void encoder2_init(void)
{
	GPIO_InitTypeDef 			gpio;
	TIM_TimeBaseInitTypeDef		base;  
	TIM_ICInitTypeDef 			ic; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	gpio.GPIO_Pin = GPIO_Pin_6;	
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&gpio);					

	gpio.GPIO_Pin = GPIO_Pin_7;				
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&gpio);					

	TIM_TimeBaseStructInit(&base);
	base.TIM_Prescaler = 0; 
	base.TIM_Period = 0XFFFF;		
	base.TIM_ClockDivision = TIM_CKD_DIV1;		
	base.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4,&base);
	
	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_BothEdge,TIM_ICPolarity_BothEdge);
	
	TIM_ICStructInit(&ic);
	ic.TIM_ICFilter = 10;
	TIM_ICInit(TIM4,&ic);
	
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);

	TIM_SetCounter(TIM4,0);
	TIM_Cmd(TIM4,ENABLE); 
}


void encoder_init(void)
{
	encoder1_init();
	encoder2_init();
}


int encoder_read(u8 EncoderX)
{
    int encoder_value;   
	
	switch(EncoderX)
	{
		case 1:  encoder_value = (short)TIM2->CNT;	TIM2->CNT=0;	break;
		case 2:  encoder_value = (short)TIM4->CNT;	TIM4->CNT=0;	break;	
		
		default: encoder_value=0;
	}
	return encoder_value;
}


void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM_GetITStatus(TIM2,TIM_IT_Update))
	{
		
	}				   
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}


void TIM4_IRQHandler(void)
{ 		    		  			    
	if(TIM_GetITStatus(TIM4,TIM_IT_Update))
	{
		
	}				   
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}





