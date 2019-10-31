#include "main.h"


u16 sys_count = 0;

extern u8 ConfigOK;
extern u8 DataOK;

u8 No_Load_Flag = 0;

unsigned char print_group[6];
unsigned char against_group[6];

extern float Weight_Real;


void Tim1_configration(void)
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
	if(TIM_GetITStatus(TIM1,TIM_IT_Update))
	{
		if((ConfigOK == 1)&&(DataOK == 1))
		{
			sys_count ++;
			
			//10ms——
			if((sys_count%10) == 0)
			{
				u8 i;					

				unsigned char str[20]; 

				//显示标题
				for(i=0;i<6;i++)
					OLED_ShowCN(12+i*16,0,i);
				
				sprintf(str,"  %6.3f    ",Weight_Real); 
				
				OLED_ShowStr(0,4,str,2); 

				OLED_ShowStr(100,4,"g ",2);	
			}
			
			/* read noload data */			
			if(((sys_count%100) == 0)&&(No_Load_Flag == 0))
			{
				Get_No_Lode();
			}
			
			//100ms——读取拉力——OLED显示
			if((sys_count%100) == 0)
			{
				if(No_Load_Flag == 0)
				{
					//清屏
					OLED_CLS();
					
					//首次读取空载示数
					Get_No_Lode();	
				}
				else if(No_Load_Flag == 1)
				{
					//HX711模块有10Hz和80Hz两种模式
					//但此模块硬件上直接设置为10Hz模式
					//采样频率提高也会等待硬件读取
					Get_Weight();
				}
			}
			
			//500ms——闪灯
			if((sys_count%500) == 0)
			{
				LED_TOGGLE();
//				GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			}
			
			//1000ms——无
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


