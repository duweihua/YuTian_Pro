#include "main.h"

//ϵͳ����
u16 sys_count = 0;
u16 second_count = 0;

extern u8 ConfigOK;
extern u8 DataOK;

//PID�ṹ��
PID pid_Position1;
PID pid_Speed1;
PID pid_Current1;

PID pid_Position2;
PID pid_Speed2;
PID pid_Current2;

float K_Current_Set = 1.0f;

//PWM���
short PWM_Pre1;
short PWM_Pre2;

short PWM_Out1;
short PWM_Out2;

short M1_1;
short M1_2;

short M2_1;
short M2_2;

float K_pwm = 1.66;	//����С�Ƕ�
			//2.0f; //��ǶȽϺ�

//���������

//ÿȦ��Ӧ��������
u16 PulsePreCircle = 3500;

short speed1;
short speed2;

short lastspeed1;
short lastspeed2;

float realspeed1;
float realspeed2;

short pos1 = 0;
short pos2 = 0;

float realpos1;
float realpos2;

float Kspeed_filter = 0.6873f;

//ADC���
float nowcurrent1;
float nowcurrent2;

float Global_SetCurrent1;
float Global_SetCurrent2;

extern float CHECK_Voltage;

//CAN���
short CAN_Data1 = 0x1111;

CanTxMsg TxMessage;

//ң����
RC_t rc;


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


void GetData_Encoder(void)
{
	//nowspeed��λ��Ȧ/s
	/*��ʵ�⣺
			AB˫��ɼ�ʱ
				���ÿ��תһȦ����3500������
			����˵������Ŀ˵��������
			�˴�ѡ������ʵ������
	*/
	Encoder1_read();
	Encoder2_read();

	//�����������ٶ��˲�
	speed1 = speed1 * Kspeed_filter + lastspeed1 * (1.0f - Kspeed_filter);	
	speed2 = speed2 * Kspeed_filter + lastspeed2 * (1.0f - Kspeed_filter);

	//��λ��ת/��
	realspeed1 = (100.0f/(float)PulsePreCircle*speed1);
	realspeed2 = (100.0f/(float)PulsePreCircle*speed2);

	pos1 += speed1;
	pos2 += speed2;

	//��λ��Ȧ
	realpos1 = pos1/(float)PulsePreCircle;
	realpos2 = pos2/(float)PulsePreCircle;
}


void Pid_Control(void)
{
//	//Motor1 Control======================================
//	pid_Speed1.SetPoint = PID_Calc(&pid_Position1,pos1);
//	pid_Current1.SetPoint = K_Current_Set*PID_Calc(&pid_Speed1,speed1);
//	
////	pid_Current2.SetPoint = pid_Current2.SetPoint*K_Current + pid_Current2.SetPointLast*(1-K_Current);
//	
//	//�������� 100mA����
//	if(pid_Current1.SetPoint>=650.0f)
//		pid_Current1.SetPoint = 650.0f;
//	else if(pid_Current1.SetPoint<=-650.0f)
//		pid_Current1.SetPoint = -650.0f;
//	
//	if(nowcurrent1>=650.0f)
//		pid_Current1.SetPoint = 650.0f;
//	else if(nowcurrent1<=-650.0f)
//		pid_Current1.SetPoint = -650.0f;
//	
//	Global_SetCurrent1 = pid_Current1.SetPoint;

//	//Ϊ����С�ǶȺʹ�Ƕ�
//	//�����д����� �Ȳ���
////	if((pid_Position1.PreError>1500)||(pid_Position1.PreError<-1500))
////		K_pwm = 2.0f;
////	else if((pid_Position1.PreError<=1500)&&(pid_Position1.PreError>=-1500))
////		K_pwm = 1.5f;

//	PWM_Pre1 = K_pwm*(PID_Calc(&pid_Current1,nowcurrent1));
//	PWM_Out1 = LIMIT_MAX_MIN(PWM_Pre1,2000,-2000);//+-2000

//	M1_1 = 1000 - PWM_Out1/2;
//	M1_2 = 1000 + PWM_Out1/2;

//	TIM_SetCompare1(TIM4,1000 - PWM_Out1/2);
//	TIM_SetCompare2(TIM4,1000 + PWM_Out1/2);

	//Motor2 Control======================================
	pid_Speed2.SetPoint = PID_Calc(&pid_Position2,pos2);
	pid_Current2.SetPoint = K_Current_Set*PID_Calc(&pid_Speed2,speed2);
	
//	pid_Current2.SetPoint = pid_Current2.SetPoint*K_Current + pid_Current2.SetPointLast*(1-K_Current);
	
	//�������� 100mA����
	if(pid_Current2.SetPoint>=350.0f)
		pid_Current2.SetPoint = 350.0f;
	else if(pid_Current2.SetPoint<=-350.0f)
		pid_Current2.SetPoint = -350.0f;
	
	if(nowcurrent2>=350.0f)
		pid_Current2.SetPoint = 350.0f;
	else if(nowcurrent2<=-350.0f)
		pid_Current2.SetPoint = -350.0f;
	
	Global_SetCurrent2 = pid_Current2.SetPoint;

	//Ϊ����С�ǶȺʹ�Ƕ�
	//�����д����� �Ȳ���
//	if((pid_Position2.PreError>1500)||(pid_Position2.PreError<-1500))
//		K_pwm = 2.0f;
//	else if((pid_Position2.PreError<=1500)&&(pid_Position2.PreError>=-1500))
//		K_pwm = 1.5f;

	PWM_Pre2 = K_pwm*(PID_Calc(&pid_Current2,nowcurrent2));
	PWM_Out2 = LIMIT_MAX_MIN(PWM_Pre2,2000,-2000);//+-2000

	M2_1 = 1000 - PWM_Out2/2;
	M2_2 = 1000 + PWM_Out2/2;

	TIM_SetCompare3(TIM4,1000 - PWM_Out2/2);
	TIM_SetCompare4(TIM4,1000 + PWM_Out2/2);
}


void test_mode(void)
{
	if(second_count == 2)
		pid_Position2.SetPoint = (PulsePreCircle/2);
	if(second_count == 4)
		pid_Position2.SetPoint = (PulsePreCircle/2) + PulsePreCircle*2;
}


void TIM1_UP_IRQHandler()
{
	if(TIM_GetITStatus(TIM1,TIM_IT_Update))
	{
		if((ConfigOK == 1)&&(DataOK == 1))
		{
			sys_count ++;
			
			//10ms
			if((sys_count%10) == 0)
			{
				USART1_Dataset();
				
//				test_mode();

				GetData_Encoder();
		
				ADC_read();
								
				Pid_Control();
			}
			
			//50ms
			if((sys_count%50) == 0)
			{
//				USART1_Dataset();
				
				//�����ú���
//				test_mode();
			}
			
			//500ms
			if((sys_count%500) == 0)
			{
				LED1_TOGGLE();
			
//				Get_Adc_Check();
				
//				CANSendData(CAN_Data1,0);
			}
			
			//1000ms
			if(sys_count>=1000)
			{
				//�����ü�ʱ
//				if(second_count<=4)
//					second_count++;
				sys_count = 0;
			}
			TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		}
		else 
			TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}







