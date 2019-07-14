#include "stm32f10x.h"
#include "usart1.h"
#include "delay.h"
#include "led.h"
#include "data_solve.h"
#include "pwm.h"
#include "timer.h"
#include "key.h"

/* data define ---------------------------------------------------------------*/
//任务模式选择
unsigned char state_global = 1;
//unsigned char state_global = 2;
//unsigned char state_global = 3;
//unsigned char state_global = 4;

//任务4计时变量
int stay_time = 0;
int stay_time4 = 0;
int stay_time5 = 0;
int stay_time1 = 0;

//PID参数初始化
float Kp_x	= 0;
float Ki_x	= 0;
float Kd_x	= 0;
             
float Kp_y	= 0;
float Ki_y	= 0;
float Kd_y	= 0;

float Aim_X		= 240;
float Aim_Y		= 240;


float X 			= 253;
float Y 			= 227;
float X_LAST	= 240;
float Y_LAST	= 240;

float X_sumerror	= 0;
float Y_sumerror	= 0;
float sum_max 		= 4000;
float sum_min			= -4000;

float Err_X				= 0;
float Err_Y				= 0;
float Err_X_LAST	= 0;
float Err_Y_LAST	= 0;

//机械最后一次大改之后舵机数据初始化
uint32_t PWM_X 	= 0;
uint32_t PWM_Y 	= 0;  

u16 serve_x_init_pwm = 1550;
u16 serve_y_init_pwm = 1400;

u16 angle_x_max = (1550+400);
u16 angle_x_min = (1550-400);
u16 angle_y_max = (1400+400);
u16 angle_y_min = (1400-400);

//debug专用全局变量
u16 angle_x = 1500;
u16 angle_y = 1500;

float Pout_x = 0; 
float Iout_x = 0;
float Dout_x = 0;
               
float Pout_y = 0;
float Iout_y = 0;
float Dout_y = 0;

float DIF_X = 0;
float DIF_Y = 0;

//调试须知：
//*****************************
//定时器 引脚 PWM方向对应关系	*
//TIM2 CH1 -> PA0 -> PWM_X		*
//TIM3 CH1 -> PA6 -> PWM_Y		*
//*****************************
//串口TX -> PA3								*
//*****************************
//angle_x 变大 -> 右边升高		*
//angle_y 变大 -> 前面升高		*
//*****************************

int main(void)
{
 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	delay_init();
	
	LED_Init();
	
	KEY_EXTI_Init();
	
	usart_init();
	
	//100ms中断(频率100KHz)
	TIM4_Init(10000-1,720-1);	//100KHz计数频率

	//PWM初始化	
	TIM2_PWM_Init(20000-1,72-1);
	TIM3_PWM_Init(20000-1,72-1);
	
	//舵机平衡值初始化
	TIM_SetCompare1(TIM2,serve_x_init_pwm);
	TIM_SetCompare1(TIM3,serve_y_init_pwm);		
	
	while(1)
	{
		//确定舵机中值(debug用)
		//TIM_SetCompare1(TIM2,angle_x);
		//TIM_SetCompare1(TIM3,angle_y);
		
//		if(angle_x<=1100)
//			angle_x = 1900;
//		angle_x -= 20;
//		delay_ms(500);
//		TIM_SetCompare1(TIM2,angle_x);
		
//		if(angle_y>=1900)
//			angle_y = 1100;
//		angle_y += 20;
//		delay_ms(500);
//		TIM_SetCompare1(TIM3,angle_y);
//		TIM_SetCompare1(TIM2,angle_y);
	}
}




