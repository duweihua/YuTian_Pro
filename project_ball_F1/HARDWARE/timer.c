#include "timer.h"
#include "led.h"

//��������
extern unsigned char state_global;

extern int stay_time;
extern int stay_time1;
extern int stay_time4;
extern int stay_time5;

extern float Kp_x;
extern float Ki_x;
extern float Kd_x;
          
extern float Kp_y;
extern float Ki_y;
extern float Kd_y;

extern float X_MID; 	
extern float Y_MID; 	

extern float X; 			
extern float Y; 			
extern float X_LAST;	
extern float Y_LAST;	

extern float Aim_X;				
extern float Aim_Y;	

extern float Err_X;				
extern float Err_Y;				
extern float Err_X_LAST;	
extern float Err_Y_LAST;

extern float X_sumerror;
extern float Y_sumerror;

extern float sum_max;
extern float sum_min;

extern u16 angle_x_max;
extern u16 angle_x_min;
extern u16 angle_y_max;
extern u16 angle_y_min;

extern uint32_t PWM_X; 	
extern uint32_t PWM_Y;

extern u16 serve_x_init_pwm;
extern u16 serve_y_init_pwm;

extern unsigned char PLACE_DATA[4];

//debugר��ȫ�ֱ���
extern u16 angle_x;
extern u16 angle_y;

extern float Pout_x; 
extern float Iout_x; 
extern float Dout_x; 
             
extern float Pout_y; 
extern float Iout_y; 
extern float Dout_y; 

extern float DIF_X;
extern float DIF_Y;

//TIM4��ʱ���ж��п��ƶ��
void TIM4_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
	NVIC_InitTypeDef 					NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

	TIM_Cmd(TIM4, ENABLE); 
}


//����ж�
void TIM4_IRQHandler(void)  
{
	if (TIM_GetITStatus(TIM4,TIM_IT_Update)!= RESET) 
	{
		LED1=!LED1;	//����ָʾ�������Ƶ��
		
		//ģʽѡ��
		//����һ**********************
		if(state_global==1)
		{
			Aim_X = 240;
			Aim_Y = 240;

			//��е���һ�δ�ĺ����
			Kp_x	= 1.7;		
			Ki_x	= 0.025;	
			Kd_x	= 16;//9.5
						
			Kp_y	= 1.7;	
			Ki_y	= 0.015;	
			Kd_y	= 16;//9.5	
		}
		
		//�����**********************
		if(state_global==2)
		{
//				Aim_X = 240;
//				Aim_Y = 405;

			Aim_X = 405;
			Aim_Y = 240;

			//��е���һ�δ�ĺ����
			Kp_x	= 1.7;//1.7
			Ki_x	= 0.015;	
			Kd_x	= 16;//16			
						
			Kp_y	= 1.7;//1.7	
			Ki_y	= 0.015;
			Kd_y	= 16;//16
		}
				
		//������**********************
		if(state_global==3)	
		{
			Aim_X = 240;
			Aim_Y = 240;
	
			
//			if(stay_time<2000)
//			{
//					Aim_X = 405;
//					Aim_Y = 240;
				
				Aim_X = 240;
				Aim_Y = 240;
				
				//�ж�ʱ����4��5
//				if((Err_X*Err_X+Err_Y*Err_Y)<=19*19)
//					stay_time += 100;
//				if((Err_X*Err_X+Err_Y*Err_Y)> 19*19)
//					stay_time = 0;
//			}
//			else if(stay_time>=2000)
//			{					
//				Aim_X = 405;
//				Aim_Y = 405;
//			}

			//��е���һ�δ�ĺ����
			//D���� -> ����
			Kp_x	= 1.7;
			Ki_x	= 0.025;	
			Kd_x	= 20;	//16		
						
			Kp_y	= 1.7;	
			Ki_y	= 0.025;	
			Kd_y	= 20;	//16
		}
	
		if(state_global==4)
		{
			if(stay_time4<2000)
			{
//					Aim_X = 405;
//					Aim_Y = 240;
				
				Aim_X = 240;
				Aim_Y = 75;
				
				//�ж�ʱ����4��5
				if((Err_X*Err_X+Err_Y*Err_Y)<=19*19)
					stay_time4 += 100;
				if((Err_X*Err_X+Err_Y*Err_Y)> 19*19)
					stay_time4 = 0;
			}
			else if(stay_time4>=2000)
			{					
				if(stay_time5<2000)
				{
					Aim_X = 240;
					Aim_Y = 240;
					
					//�ж�ʱ����5��1
					if((Err_X*Err_X+Err_Y*Err_Y)<=19*19)
						stay_time5 += 100;
					if((Err_X*Err_X+Err_Y*Err_Y)> 19*19)
						stay_time5 = 0;						
				}
				else if(stay_time5>=2000)
				{
					if(stay_time1<2000)
					{
						Aim_X = 405;
						Aim_Y = 75;
						
					//�ж�ʱ����5��1
					if((Err_X*Err_X+Err_Y*Err_Y)<=19*19)
						stay_time1 += 100;
					if((Err_X*Err_X+Err_Y*Err_Y)> 19*19)
						stay_time1 = 0;									
					}
					else if(stay_time1>=2000)
					{
						Aim_X = 240;
						Aim_Y = 75;
						stay_time1 = 0;
						stay_time4 = 0;
						stay_time5 = 0;
					}
					
				}
			}
			
			//��е���һ�δ�ĺ����
			//D���� -> ����
			Kp_x	= 1.6;//1.8
			Ki_x	= 0.025;	//0.025
			Kd_x	= 16;//16	
						
			Kp_y	= 1.6;//1.8	
			Ki_y	= 0.015;	
			Kd_y	= 16;//16
		}

		//ERROR˵����
		//X����Err>0 -> ��ƫ�� -> ���Ҳ�̧�� -> PWM_X+
		//Y����Err>0 -> ��ƫ�� -> ��ǰ������ -> PWM_Y-
		//X����Err<0 -> ��ƫ�� -> ���Ҳཱུ�� -> PWM_X-
		//Y����Err<0 -> ��ƫǰ -> ��ǰ��̧�� -> PWM_Y+
		
		Err_X = X-Aim_X;
		Err_Y = Y-Aim_Y;
		
		//PID->I�����⴦��
		//��Ȧ		���ۼ�
		//ERROR��	���ۼ�
//			if(((Err_X*Err_X+Err_Y*Err_Y)>=11*11)&(Err_X*Err_X+Err_Y*Err_Y<=60*60))
		if(((Err_X*Err_X+Err_Y*Err_Y)>=18*18)&(Err_X<90)&(Err_Y<60))
		{
			X_sumerror += (5*Err_X);
			Y_sumerror += (5*Err_Y);
		}
		
		//��ֹ���ֱ���
		//�˴����ù���ķ���
		if(X_sumerror>sum_max)
			X_sumerror = 0;
		if(X_sumerror<=sum_min)
			X_sumerror = 0;
		if(Y_sumerror>sum_max)
			Y_sumerror = 0;
		if(Y_sumerror<=sum_min)
			Y_sumerror = 0;


		//����Ȧ���ϸ΢����
		//����һ**********************
		if(state_global==1)
		{
			if((Err_X <100)&(Err_Y<100))
			{
				//��е���һ�δ�ĺ����
				if(Err_X*Err_X+Err_Y*Err_Y<=28*28)
				{
					Kp_x = 1.5;//1.5
					Kp_x = 1.5;//1.5
					Kd_x = 15;//20//14
					Kd_x = 15;//20//14
				}
			}
		}
		
		//�����**********************
		if(state_global==2)
		{
			if((Err_X<100)&(Err_Y<100))
			{
				//��е���һ�δ�ĺ����
				if(Err_X*Err_X+Err_Y*Err_Y<=28*28)
				{
					Kp_x = 1.3;//2
					Kp_x = 1.3;//2
					Kd_x = 14.5;//20
					Kd_x = 14.5;//20
	//				Kp_x = 2;//2
	//				Kp_x = 2;//2
	//				Kd_x = 20;//20
	//				Kd_x = 20;//20				
				}
			}
		}
		
		//������**********************
		if(state_global==3)
		{
			//��е���һ�δ�ĺ����
			if((Err_X*Err_X+Err_Y*Err_Y)<=(28*28))
			{
				Kp_x = 1.3;//2.0
				Kp_x = 1.3;//2.0
				Kd_x = 13;//16
				Kd_x = 13;//16
//				Kp_x = 1.3;//2
//				Kp_x = 1.3;//2
//				Kd_x = 14.5;//20
//				Kd_x = 14.5;//20
			}
		}
		
		//������**********************
		if(state_global==4)
		{
			//��е���һ�δ�ĺ����
			if(Err_X*Err_X+Err_Y*Err_Y<=28*28)
			{
				Kp_x = 1.5;//2
				Kp_x = 1.5;//2
				Kd_x = 12;//20
				Kd_x = 12;//20
			}
		}
		
//			if((Err_X*Err_X+Err_Y*Err_Y)<=12*12)
//			{
//				Kp_x = 0;
//				Kp_y = 0;
//				Ki_x = 0;
//				Ki_y = 0;
//				Kd_x = 0;
//				Kd_y = 0;
//			}

		//PID_calculate
		if(state_global == 3)
			PWM_X = serve_x_init_pwm-50+(Err_X*Kp_x+X_sumerror*Ki_x+(Err_X-Err_X_LAST)*Kd_x);
		else 
			PWM_X = serve_x_init_pwm+(Err_X*Kp_x+X_sumerror*Ki_x+(Err_X-Err_X_LAST)*Kd_x);
		if(state_global == 2)
			PWM_Y = serve_y_init_pwm-30-(Err_Y*Kp_y+Y_sumerror*Ki_y+(Err_Y-Err_Y_LAST)*Kd_y);
		else
			PWM_Y = serve_y_init_pwm-(Err_Y*Kp_y+Y_sumerror*Ki_y+(Err_Y-Err_Y_LAST)*Kd_y);
		
		//�����ù۲����
		Pout_x = Err_X*Kp_x;
		Iout_x = X_sumerror*Ki_x;
		Dout_x = (Err_X-Err_X_LAST)*Kd_x;
		
		Pout_y = Err_Y*Kp_y;
		Iout_y = Y_sumerror*Ki_y;
		Dout_y = (Err_Y-Err_Y_LAST)*Kd_y;
		
		DIF_X = Err_X-Err_X_LAST;
		DIF_Y = Err_Y-Err_Y_LAST;
		
		//����������
		if(PWM_Y>angle_y_max)
			PWM_Y = angle_y_max;
		if(PWM_Y<angle_y_min)
			PWM_Y = angle_y_min;
		if(PWM_X>angle_x_max)
			PWM_X = angle_x_max;
		if(PWM_X<angle_x_min)
			PWM_X = angle_x_min;
		
		//ƫ���
		X_LAST = X;
		Y_LAST = Y;
		Err_X_LAST = Err_X;
		Err_Y_LAST = Err_Y;
		
		//������
		TIM_SetCompare1(TIM3,PWM_Y);
		TIM_SetCompare1(TIM2,PWM_X);	

		//������ʱ�Ĵ���
		//X = Aim_X;
		//Y = Aim_Y;
		
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  
	}
}






