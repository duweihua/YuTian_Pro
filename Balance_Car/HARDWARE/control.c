#include "main.h"


extern u8 ConfigOK,DataOK;

u8 speed_control_flag;


void control_init(void)
{
	TIM_TimeBaseInitTypeDef		tim;
	NVIC_InitTypeDef 			nvic;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	tim.TIM_Period=1000;
	tim.TIM_Prescaler=(72-1);
	tim.TIM_ClockDivision=TIM_CKD_DIV1;
	tim.TIM_CounterMode=TIM_CounterMode_Up;
	//不加这句导致TIM1定时不准
	//好像自动重新计数
	tim.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&tim);
	
	nvic.NVIC_IRQChannel = TIM1_UP_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	TIM_ARRPreloadConfig(TIM1,ENABLE);
	
	TIM_ClearFlag(TIM1,TIM_FLAG_Update);
	TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	
	TIM_Cmd(TIM1,ENABLE);
}


void TIM1_UP_IRQHandler()
{
	static uint16_t sys_count;
	
	if(TIM_GetITStatus(TIM1,TIM_IT_Update))
	{
		if((ConfigOK == 1)&&(DataOK == 1))
		{
			sys_count ++;
			
			//5ms——读陀螺仪
			//	 ——控制周期
			if((sys_count%5) == 0)
			{
				imu_read();

				all_control();
			}
			
			//50ms——可选速度环控制周期
			if((sys_count%50) == 0)
			{
				speed_control_flag = 1;
			}
			
			//500ms
			if((sys_count%500) == 0)
			{
				LED_TOGGLE();
			}
			
			//1000ms
			if(sys_count>=1000)
			{
				sys_count = 0;
			}
			TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		}
		else 
			TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}







