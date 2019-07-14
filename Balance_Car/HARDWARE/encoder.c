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
	//���벶���˲�ϵ����
	//	���ã���ʱ��ʱ�Ӿ�ʱ�ӷ�Ƶ����(TIM_ClockDivision)��Ƶ��õ��˲�������ʱ��(fSAMPLING)
	//		  �����������˲����Ĳ�����ƵΪN(TIM_ICFilter)
	//		  ���˲��ĳ��ȣ�t = 1/fSAMPLING*N
	//		  ��������ͨ�����˲����ͻ��˵��� t ���������
	//		  Ҳ����˵�˲�������fSAMPLING��Ƶ�ʲ���8��
	//		  ���8�ζ�Ϊ�ߵ�ƽ��ȷ��Ϊ�����ش���
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





