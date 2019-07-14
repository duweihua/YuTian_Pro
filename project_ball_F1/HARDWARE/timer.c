#include "timer.h"
#include "led.h"

//数据声明
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

//debug专用全局变量
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

//TIM4定时器中断中控制舵机
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


//溢出中断
void TIM4_IRQHandler(void)  
{
	if (TIM_GetITStatus(TIM4,TIM_IT_Update)!= RESET) 
	{
		LED1=!LED1;	//闪灯指示舵机控制频率
		
		//模式选择
		//任务一**********************
		if(state_global==1)
		{
			Aim_X = 240;
			Aim_Y = 240;

			//机械最后一次大改后参数
			Kp_x	= 1.7;		
			Ki_x	= 0.025;	
			Kd_x	= 16;//9.5
						
			Kp_y	= 1.7;	
			Ki_y	= 0.015;	
			Kd_y	= 16;//9.5	
		}
		
		//任务二**********************
		if(state_global==2)
		{
//				Aim_X = 240;
//				Aim_Y = 405;

			Aim_X = 405;
			Aim_Y = 240;

			//机械最后一次大改后参数
			Kp_x	= 1.7;//1.7
			Ki_x	= 0.015;	
			Kd_x	= 16;//16			
						
			Kp_y	= 1.7;//1.7	
			Ki_y	= 0.015;
			Kd_y	= 16;//16
		}
				
		//任务三**********************
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
				
				//判断时机从4到5
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

			//机械最后一次大改后参数
			//D给大 -> 加速
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
				
				//判断时机从4到5
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
					
					//判断时机从5到1
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
						
					//判断时机从5到1
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
			
			//机械最后一次大改后参数
			//D给大 -> 加速
			Kp_x	= 1.6;//1.8
			Ki_x	= 0.025;	//0.025
			Kd_x	= 16;//16	
						
			Kp_y	= 1.6;//1.8	
			Ki_y	= 0.015;	
			Kd_y	= 16;//16
		}

		//ERROR说明：
		//X方向Err>0 -> 球偏右 -> 需右侧抬高 -> PWM_X+
		//Y方向Err>0 -> 球偏后 -> 需前方降低 -> PWM_Y-
		//X方向Err<0 -> 球偏左 -> 需右侧降低 -> PWM_X-
		//Y方向Err<0 -> 球偏前 -> 需前方抬高 -> PWM_Y+
		
		Err_X = X-Aim_X;
		Err_Y = Y-Aim_Y;
		
		//PID->I项特殊处理
		//进圈		不累加
		//ERROR大	不累加
//			if(((Err_X*Err_X+Err_Y*Err_Y)>=11*11)&(Err_X*Err_X+Err_Y*Err_Y<=60*60))
		if(((Err_X*Err_X+Err_Y*Err_Y)>=18*18)&(Err_X<90)&(Err_Y<60))
		{
			X_sumerror += (5*Err_X);
			Y_sumerror += (5*Err_Y);
		}
		
		//防止积分饱和
		//此处采用归零的方法
		if(X_sumerror>sum_max)
			X_sumerror = 0;
		if(X_sumerror<=sum_min)
			X_sumerror = 0;
		if(Y_sumerror>sum_max)
			Y_sumerror = 0;
		if(Y_sumerror<=sum_min)
			Y_sumerror = 0;


		//靠近圈后的细微调整
		//任务一**********************
		if(state_global==1)
		{
			if((Err_X <100)&(Err_Y<100))
			{
				//机械最后一次大改后参数
				if(Err_X*Err_X+Err_Y*Err_Y<=28*28)
				{
					Kp_x = 1.5;//1.5
					Kp_x = 1.5;//1.5
					Kd_x = 15;//20//14
					Kd_x = 15;//20//14
				}
			}
		}
		
		//任务二**********************
		if(state_global==2)
		{
			if((Err_X<100)&(Err_Y<100))
			{
				//机械最后一次大改后参数
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
		
		//任务三**********************
		if(state_global==3)
		{
			//机械最后一次大改后参数
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
		
		//任务四**********************
		if(state_global==4)
		{
			//机械最后一次大改后参数
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
		
		//调试用观察变量
		Pout_x = Err_X*Kp_x;
		Iout_x = X_sumerror*Ki_x;
		Dout_x = (Err_X-Err_X_LAST)*Kd_x;
		
		Pout_y = Err_Y*Kp_y;
		Iout_y = Y_sumerror*Ki_y;
		Dout_y = (Err_Y-Err_Y_LAST)*Kd_y;
		
		DIF_X = Err_X-Err_X_LAST;
		DIF_Y = Err_Y-Err_Y_LAST;
		
		//舵机输出限制
		if(PWM_Y>angle_y_max)
			PWM_Y = angle_y_max;
		if(PWM_Y<angle_y_min)
			PWM_Y = angle_y_min;
		if(PWM_X>angle_x_max)
			PWM_X = angle_x_max;
		if(PWM_X<angle_x_min)
			PWM_X = angle_x_min;
		
		//偏差传递
		X_LAST = X;
		Y_LAST = Y;
		Err_X_LAST = Err_X;
		Err_Y_LAST = Err_Y;
		
		//舵机输出
		TIM_SetCompare1(TIM3,PWM_Y);
		TIM_SetCompare1(TIM2,PWM_X);	

		//无数据时的处理
		//X = Aim_X;
		//Y = Aim_Y;
		
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  
	}
}






