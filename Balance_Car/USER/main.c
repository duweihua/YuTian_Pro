#include "main.h"


/*
	��ʹ�����ţ�
	
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

//��̬�������
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

//����������������������PID����������������������
	
//����ʧ�ܲ���
//*****************************************
	//ֱ����δ��0.6ʱ����(4/11)
//	Balance_Kp = -360.0f;
//	Balance_Kd = -1050.0f;//-1700.0f;
	
	//ֱ����������������С�����(4/11)
//	Balance_Kp = -216.0f;//-360*0.6
//	Balance_Kd = -630.0f;//-1050*0.6

	//ֱ����δ��0.6ʱ����(4/12)
//	Balance_Kp = -320.0f;
//	Balance_Kd = -900.0f;

	//ֱ����������������С�����(4/12)
//	Balance_Kp = -192.0f;
//	Balance_Kd = -540.0f;

//	//�ٶȻ�����(4/12)
//	Velocity_Kp = -80.0f;
////	Velocity_Ki = 0.0f;
//	Velocity_Ki = Velocity_Kp/200.0f;
//******************************************

//4/12�ڶ��ε���****************************
	/*˵��������50ms�ٶȻ�����
			PWM���� 6900
	*/
	/*Ч��������ƽ������ʱ��
			�Ⱥ�ζ���С
			��Щ��ת
	*/
//	Balance_Kp = -280.0f;
//	Balance_Kd = -420.0f;

//	Velocity_Kp = -100.0f;
//	Velocity_Ki = -0.6f;
	
//	PITCH_MID = -7.5f;
//******************************************
	
//4/12�����ε���****************************
	//˵��������5ms�ٶȻ�����
	/*Ч���������Žϲ�
			��ת�ϴ�
	*/
//	Balance_Kp = -170.0f;
//	Balance_Kd = -800.0f;//-800

//	Velocity_Kp = -60.0f;//-62
//	Velocity_Ki = -0.2f;
//	
//	PITCH_MID = -7.0f;
//******************************************

//4/13��һ�ε���****************************
	//˵��������5ms�ٶȻ�����
	/*Ч���������Ż�����
			��ת�ϴ�
	*/
//	Balance_Kp = -140.0f;//-160.0f
//	Balance_Kd = -800.0f;//-800

//	Velocity_Kp = -70.0f;//-65.0f
//	Velocity_Ki = -0.12f;//-0.2f
//	
//	PITCH_MID = -7.0f;
//******************************************

//4/13�ڶ��ε���****************************
	//˵��������5ms�ٶȻ�����
	/*Ч�����񵴽�С
			��ת������
	*/
//	Balance_Kp = -140.0f;//-160.0f
//	Balance_Kd = -900.0f;//-800

//	Velocity_Kp = -72.0f;//-65.0f
//	Velocity_Ki = -0.15f;//-0.2f
//	
//	PITCH_MID = -7.0f;
//******************************************

//4/13�����ε���****************************
	//˵��������50ms�ٶȻ�����
	/*Ч��������Ϻ�
			�����������Բ�
			��ʱ����ܻᵹ
	*/
//	Balance_Kp = -350.0f;//-300
//	Balance_Kd = -540.0f;//-400

//	Velocity_Kp = -90.0f;
//	Velocity_Ki = -0.6f;

//	PITCH_MID = -7.0f;
//******************************************

//4/13���Ĵε���****************************
	//˵��������50ms�ٶȻ�����
	/*Ч������ǿ����ǰ��+����
			��ʱ��������
			ԭ���񵴽ϴ�
	*/
//	Balance_Kp = -320.0f;//-300
//	Balance_Kd = -560.0f;//-400

//	Velocity_Kp = -92.5f;
//	Velocity_Ki = -0.6f;

//	PITCH_MID = -7.0f;
//******************************************

//4/14��һ�ε���****************************
	//˵��������50ms�ٶȻ�����
	/*Ч������������300�����ȶ�
			�н��䶶���ܴ�
			ԭ���Զ��Դ�
			������������ֱ��
	*/
//	Balance_Kp = -300.0f;//-300
//	Balance_Kd = -580.0f;//-400

//	Velocity_Kp = -83.5f;
//	Velocity_Ki = -0.5f;

//	PITCH_MID = -7.0f;
//******************************************

//4/16��һ�ε���****************************
	//˵��������50ms�ٶȻ�����
	/*Ч��������300/240
			�н����п��ܶ���
			ԭ���Զ����Խ���
			���г�ɲ��������Խ���
			������ת��
			ת����������н�
			(������)
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

//4/25��һ�ε���****************************
	//˵��������50ms�ٶȻ�����
	/*Ч����
			�����ݶȼ�С������
			����ʵ��ֱ������Ϻ�
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

//����������������������PID����������������������

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




