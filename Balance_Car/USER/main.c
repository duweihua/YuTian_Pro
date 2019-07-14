#include "main.h"


/*
	已使用引脚：
	
	PA:
		ENCODER1
			TIM2_CH1	--> PA0
			TIM2_CH2	--> PA1

		TB6612(H_Bridge)
			AIN1		--> PA5
			AIN2		--> PA4
			BIN1		--> PB10
			BIN2		--> PB11

		USART2
			USART2_RX	--> PA3

		PWMA
			TIM3_CH1	--> PA6

		PWMB
			TIM3_CH2	--> PA7	

		USART1
			USART1_TX	--> PA9

	PB:
		ENCODER2
			TIM4_CH1	--> PB6
		    TIM4_CH2    --> PB7
			
		IMU
			IIC_SDA		--> PB14
			IIC_SCL		--> PB15

		OLED
			IIC_SCL		--> PB10
			IIC_SDA		--> PB11

	PC:
		LED
			LED_USER	--> PC15

		KEY
			KEY_USER	--> PC14		
*/


u8 ConfigOK,DataOK;

float dt;

//姿态相关数据
ANGLE_T angle;

float pitch,yaw,roll;

float q0,q1,q2,q3;

float imuKi,imuKp;

float gyro[3];
float acc[3];

int Encoder_Left,Encoder_Right;

int Balance_Pwm,Velocity_Pwm,Turn_Pwm;

int Motor1,Motor2;

float Balance_Kp,Balance_Kd;

float Velocity_Kp,Velocity_Ki;

float Turn_Kp,Turn_Kd;

float Straight_Kp;

float K_pitch,K_roll;

float PITCH_MID;

u8 received_flag;


void system_init(void)
{
 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	delay_init();

	led_init();
	
	key_init();
	
	usart1_init();

	usart2_init();

//	oled_iic_init();

//	oled_init();
	
//	while(!oled_iic_init());

	imu_iic_gpio_init();
	
	imu_init();
	
//	while(!imu_iic_init());

	encoder_init();
	
	pwm_init();
	
	control_init();
	
//	OLED_ON();
	
	ConfigOK = 1;
}


void data_init(void)
{
//	while(!received_flag);
	
	angle.Yaw 	= 0.0f;
	angle.Pitch = 0.0f;
	angle.Roll 	= 0.0f;

	K_pitch	= 0.98f;
	K_roll = 0.98f;

	dt = 0.005;

//↓↓↓↓↓↓↓↓↓↓↓PID↓↓↓↓↓↓↓↓↓↓↓
	
//调试失败参数
//*****************************************
	//直立环未乘0.6时参数(4/11)
//	Balance_Kp = -360.0f;
//	Balance_Kd = -1050.0f;//-1700.0f;
	
	//直立环参数按比例缩小后参数(4/11)
//	Balance_Kp = -216.0f;//-360*0.6
//	Balance_Kd = -630.0f;//-1050*0.6

	//直立环未乘0.6时参数(4/12)
//	Balance_Kp = -320.0f;
//	Balance_Kd = -900.0f;

	//直立环参数按比例缩小后参数(4/12)
//	Balance_Kp = -192.0f;
//	Balance_Kd = -540.0f;

//	//速度环参数(4/12)
//	Velocity_Kp = -80.0f;
////	Velocity_Ki = 0.0f;
//	Velocity_Ki = Velocity_Kp/200.0f;
//******************************************

//4/12第二次调试****************************
	/*说明：适用50ms速度环控制
			PWM限制 6900
	*/
	/*效果：保持平衡无限时间
			先后晃动较小
			有些自转
	*/
//	Balance_Kp = -280.0f;
//	Balance_Kd = -420.0f;

//	Velocity_Kp = -100.0f;
//	Velocity_Ki = -0.6f;
	
//	PITCH_MID = -7.5f;
//******************************************
	
//4/12第三次调试****************************
	//说明：适用5ms速度环控制
	/*效果：抗干扰较差
			自转较大
	*/
//	Balance_Kp = -170.0f;
//	Balance_Kd = -800.0f;//-800

//	Velocity_Kp = -60.0f;//-62
//	Velocity_Ki = -0.2f;
//	
//	PITCH_MID = -7.0f;
//******************************************

//4/13第一次调试****************************
	//说明：适用5ms速度环控制
	/*效果：抗干扰还可以
			自转较大
	*/
//	Balance_Kp = -140.0f;//-160.0f
//	Balance_Kd = -800.0f;//-800

//	Velocity_Kp = -70.0f;//-65.0f
//	Velocity_Ki = -0.12f;//-0.2f
//	
//	PITCH_MID = -7.0f;
//******************************************

//4/13第二次调试****************************
	//说明：适用5ms速度环控制
	/*效果：振荡较小
			自转很明显
	*/
//	Balance_Kp = -140.0f;//-160.0f
//	Balance_Kd = -900.0f;//-800

//	Velocity_Kp = -72.0f;//-65.0f
//	Velocity_Ki = -0.15f;//-0.2f
//	
//	PITCH_MID = -7.0f;
//******************************************

//4/13第三次调试****************************
	//说明：适用50ms速度环控制
	/*效果：定点较好
			抗干扰性能略差
			长时间可能会倒
	*/
//	Balance_Kp = -350.0f;//-300
//	Balance_Kd = -540.0f;//-400

//	Velocity_Kp = -90.0f;
//	Velocity_Ki = -0.6f;

//	PITCH_MID = -7.0f;
//******************************************

//4/13第四次调试****************************
	//说明：适用50ms速度环控制
	/*效果：勉强可以前进+后退
			有时抖动过大
			原地振荡较大
	*/
//	Balance_Kp = -320.0f;//-300
//	Balance_Kd = -560.0f;//-400

//	Velocity_Kp = -92.5f;
//	Velocity_Ki = -0.6f;

//	PITCH_MID = -7.0f;
//******************************************

//4/14第一次调试****************************
	//说明：适用50ms速度环控制
	/*效果：死区给到300定点稳定
			行进间抖动很大
			原地自抖略大
			基本可以无限直立
	*/
//	Balance_Kp = -300.0f;//-300
//	Balance_Kd = -580.0f;//-400

//	Velocity_Kp = -83.5f;
//	Velocity_Ki = -0.5f;

//	PITCH_MID = -7.0f;
//******************************************

//4/16第一次调试****************************
	//说明：适用50ms速度环控制
	/*效果：死区300/240
			行进间有可能抖动
			原地自抖可以接受
			短行程刹车距离可以接受
			加入了转向环
			转向结束后间断行进
			(待调整)
	*/
//	Balance_Kp = -280.0f;//-300
//	Balance_Kd = -600.0f;//-560

//	Velocity_Kp = -88.5f;//-88.5
//	Velocity_Ki = -0.75f;//-0.65
//	
//	Straight_Kp = 550.0f;

//	Turn_Kp = 550.0f;
//	Turn_Kd = 0.0f;

//	PITCH_MID = -7.0f;
//******************************************

//4/25第一次调试****************************
	//说明：适用50ms速度环控制
	/*效果：
			搭配梯度减小积分项
			用来实现直线任务较好
	*/
	Balance_Kp = -280.0f;//-300
	Balance_Kd = -600.0f;//-600

	Velocity_Kp = -88.5f;//-88.5
	Velocity_Ki = -0.55f;//-0.75
	
	Straight_Kp = 550.0f;

	Turn_Kp = 550.0f;
	Turn_Kd = 0.0f;

	PITCH_MID = -7.0f;
//******************************************

//↑↑↑↑↑↑↑↑↑↑↑PID↑↑↑↑↑↑↑↑↑↑↑

	DataOK = 1;
}


int main(void)
{
	system_init();
	
	delay_ms(100);
	
	data_init();
	
	delay_ms(100);
	
	while(1)
	{
//		oled_show();

//		delay_ms(5);
	}
}




