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
	//������䵼��TIM1��ʱ��׼
	//�����Զ����¼���
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
			
			//10ms����
			if((sys_count%10) == 0)
			{
				u8 i;					

				unsigned char str[20]; 

				//��ʾ����
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
			
			//100ms������ȡ��������OLED��ʾ
			if((sys_count%100) == 0)
			{
				if(No_Load_Flag == 0)
				{
					//����
					OLED_CLS();
					
					//�״ζ�ȡ����ʾ��
					Get_No_Lode();	
				}
				else if(No_Load_Flag == 1)
				{
					//HX711ģ����10Hz��80Hz����ģʽ
					//����ģ��Ӳ����ֱ������Ϊ10Hzģʽ
					//����Ƶ�����Ҳ��ȴ�Ӳ����ȡ
					Get_Weight();
				}
			}
			
			//500ms��������
			if((sys_count%500) == 0)
			{
				LED_TOGGLE();
//				GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			}
			
			//1000ms������
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


