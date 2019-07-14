#include "main.h"


#define PWM_MAX	 6900
#define PWM_MIN -6900


extern float PITCH_MID;

extern ANGLE_T angle;

extern float gyro[3],acc[3];

extern int Encoder_Left,Encoder_Right;

extern int Balance_Pwm,Velocity_Pwm,Turn_Pwm;

extern int Motor1,Motor2;

extern float Balance_Kp,Balance_Kd;

extern float Velocity_Kp,Velocity_Ki;

extern float Turn_Kp,Turn_Kd;

extern float Straight_Kp;

extern u8 Flag_Front,Flag_Back,Flag_Left,Flag_Right;

extern u8 speed_control_flag;

extern u8 Front_Back_Change;

u8 Flag_Stop;


void all_control(void)
{
	int Motor1_temp,Motor2_temp;
	
	//�˴���Ҫʹ�����������ݼ�����ͬ
	Encoder_Left  = encoder_read(1);
	Encoder_Right = encoder_read(2);

 	Balance_Pwm  = balance_control(angle.Pitch,gyro[0]);
	
	//�ٶȻ�Ϊ������
	//50ms�ٶȻ�
	if(speed_control_flag == 1)
	{
		Velocity_Pwm = velocity_control(Encoder_Left,Encoder_Right); 
		speed_control_flag = 0;
	}

	//5ms�ٶȻ�
//	Velocity_Pwm = velocity_control(Encoder_Left,Encoder_Right);  
	
	//ֱ����ת�� ֱ���޷���֤
//	Turn_Pwm = turn_control(Encoder_Left,Encoder_Right,gyro[2]);
	
	//ԭ���޷�ת��
//	if((Flag_Front == 1)||(Flag_Back == 1))
//		Turn_Pwm = turn_control(Encoder_Left,Encoder_Right,gyro[2]);
//	else
//		Turn_Pwm = straight_control(Encoder_Left,Encoder_Right,gyro[2]);
	
	//�Ľ�
	if((Flag_Right != 0)||(Flag_Left != 0))
		Turn_Pwm = turn_control(Encoder_Left,Encoder_Right,gyro[2]);
	else
		Turn_Pwm = straight_control(Encoder_Left,Encoder_Right,gyro[2]);
	
	//��������PID
	Motor1_temp = Balance_Pwm + Velocity_Pwm - Turn_Pwm;
	Motor2_temp = Balance_Pwm + Velocity_Pwm + Turn_Pwm;

	//˫��PID
//	Motor1_temp = Balance_Pwm + Velocity_Pwm;
//	Motor2_temp = Balance_Pwm + Velocity_Pwm;

	Motor1 = LIMIT_MAX_MIN(Motor1_temp,PWM_MAX,PWM_MIN);
	Motor2 = LIMIT_MAX_MIN(Motor2_temp,PWM_MAX,PWM_MIN);
	
	//���С������/����
	if(pick_up(acc[2],angle.Pitch,Encoder_Left,Encoder_Right))
		Flag_Stop=1;
	if(put_down(angle.Pitch,Encoder_Left,Encoder_Right))
		Flag_Stop=0;

	if(abnormal_control(angle.Pitch)==0)
		Set_Pwm(Motor1,Motor2);                            
}


//ֱ��PD����
int balance_control(float Pitch_angle,float Pitch_speed)
{
	float diff;
	int balance;
	float real_mid;
	
	if(Flag_Front == 1)
		real_mid = PITCH_MID - 0;
	else if(Flag_Back == 1)
		real_mid = PITCH_MID + 0;
	else
		real_mid = PITCH_MID;
	
	//PITCH_MIDΪ�ͻ�е��ص�ֱ������
	diff = Pitch_angle - real_mid;       
	
	balance = Balance_Kp * diff + Pitch_speed * Balance_Kd;  
	
	return balance;
}


//�ٶ�PI����
int velocity_control(int encoder_left,int encoder_right)
{
	static float Velocity,Encoder_new,Encoder,Movement;
	static float Encoder_Integral,Target_Velocity=115;
	
	//����Ϊֱ������ר��
	//�ݶȼ�С������-->����ļ���
	//���4/25��һ�ε��Բ���ʹ��
	//3s 7s����
	if(Front_Back_Change == 1)
	{
//		Encoder_Integral = Encoder_Integral/2.5f;
		
		if(Encoder_Integral<0)
		{
			Encoder_Integral += 5;//5
		}
		else if(Encoder_Integral>0)
		{
			Encoder_Integral -= 5;//5
		}
		if((Encoder_Integral<0+3)||(Encoder_Integral>0-3))
		{
//			Encoder_Integral = 0;
			Front_Back_Change = 0;
		}
//		if((Encoder_Integral<40.0f)||(Encoder_Integral>-40.0f))
//			Front_Back_Change = 0;
		
//		Encoder_Integral = 0;
	}

//	if(Front_Back_Change == 1)
//	{
//		Encoder_Integral = Encoder_Integral/2.5f;
//	
//		if((Encoder_Integral<0+1)||(Encoder_Integral>0-1))
//		{
//			//ע������������
////			Encoder_Integral = 0;
//			Front_Back_Change = 0;
//		}
//	}

	if(Flag_Front == 1)
		Movement = -Target_Velocity;
	else if(Flag_Back == 1)
		Movement = Target_Velocity;
	else
		Movement = 0;

	//�н����Ƶ��ӵ��ٶ��� ����ɲ���ص�
	//�н����Ƶ��ӵ�λ���� ����ɲ������
	//���ͬʱ�� һ���̶����໥���� ������ɲ������
	//���ᵼ�¼���н� �Ҷ����� ��ʱ����
	
	//�ٶȻ�PI������
	//�����ٶ�ƫ������ٶ�(���ұ�����֮��)-Ŀ���ٶ�(�˴�Ϊ��)
	//ǰ�����˿�����Ϣ���ӵ��ٶ���
	Encoder_new = (Encoder_Left+Encoder_Right)-Movement*0.00f;//0.05f
	//�ٶȵ�ͨ�˲���
	Encoder *= 0.7;
	Encoder += Encoder_new*0.3;

	Encoder_Integral += (Encoder*2.0f);//2.0f
	
//	Encoder_Integral += Encoder;
	
	//����ǰ������
	//��������Ϣ���ӵ��ٶȵĻ�����
	//�൱�ڿ���λ��
	Encoder_Integral = Encoder_Integral-Movement;

	if(Encoder_Integral>15000)
		Encoder_Integral=15000;
	if(Encoder_Integral<-15000)
		Encoder_Integral=-15000;

	Velocity = Encoder*Velocity_Kp+Encoder_Integral*Velocity_Ki;  

	//�쳣�رյ�����ٶȻ���������
	if((abnormal_control(angle.Pitch)==1)||(Flag_Stop==1))
		Encoder_Integral=0;
	
	return Velocity;
}


//just for straight
int straight_control(int encodor_left,int encodor_right,float az)
{
	float Turn,diff;
	
	diff = az - 0;
	
	Turn = -diff*Straight_Kp;
	
	return Turn;
}


//for turn by myself
int turn_control(int encoder_left,int encoder_right,float az)
{
	static float Turn_Target,Turn;
	static float Turn_Convert = 0.2f;//0.08f
	static float Turn_Amplitude = 0.7f;//1.0f

	if(Flag_Left == 1)
		Turn_Target += Turn_Convert;
	else if(Flag_Right == 1)
		Turn_Target -= Turn_Convert;
	else 
		Turn_Target = 0;
	
	Turn_Target = LIMIT_MAX_MIN(Turn_Target,Turn_Amplitude,-Turn_Amplitude);
	
	if((Flag_Front == 1)||(Flag_Back == 1))  
		Turn_Kd = 0.6;
	else 
		Turn_Kd = 0;
	
//	Turn_Kd = 0.0f;
	
	Turn = Turn_Target*Turn_Kp + az*Turn_Kd;
	
//	Turn = Turn_Target*Turn_Kp;
	
	return Turn;
}


//�ο�����
//int turn_control(int encoder_left,int encoder_right,float az)
//{
//    static float Turn_Target,Turn,Encoder_temp,Turn_Count;
//	static float Turn_Convert = 0.7;
//	float Turn_Amplitude = 50;
//    
//	//������תǰ���ٶȵ����ٶȵ���ʼ�ٶ� ����С������Ӧ��
//  	if((Flag_Left == 1)||(Flag_Right == 1))                    
//	{
//		if(++Turn_Count == 1)
//			Encoder_temp = myabs(encoder_left+encoder_right);
//		
//		Turn_Convert = 50/Encoder_temp;
//		
//		if(Turn_Convert < 0.4)
//			Turn_Convert = 0.4;
//		if(Turn_Convert > 1)
//			Turn_Convert = 1;
//	}
//	else
//	{
//		Turn_Convert = 0.7;
//		Turn_Count = 0;
//		Encoder_temp = 0;
//	}
//	
//	if(Flag_Left == 1)
//		Turn_Target -= Turn_Convert;
//	else if(Flag_Right == 1)
//		Turn_Target += Turn_Convert;
//	else
//		Turn_Target = 0;

//    if(Turn_Target > Turn_Amplitude)
//		Turn_Target = Turn_Amplitude;
//	if(Turn_Target < -Turn_Amplitude) 
//		Turn_Target = -Turn_Amplitude;

//	if((Flag_Front == 1)||(Flag_Back == 1))  
//		Turn_Kd = 0.6;   
//	else 
//		Turn_Kd = 0;                                   

//	Turn = Turn_Target*Turn_Kp + az*Turn_Kd;
//	  
//	return Turn;
//}


u8 abnormal_control(float angle)
{
	u8 flag_abnormal;
	
	//�쳣���1����ǹ���(��ǰ��ֵ45��)
	if(angle<-35||angle>35)
	{
		flag_abnormal = 1;   

		//�رյ��
		AIN1=0;
		AIN2=0;
		BIN1=0;
		BIN2=0;
    }
	
	else if(Flag_Stop==1)
	{
		flag_abnormal = 2;
		
		AIN1=0;
		AIN2=0;
		BIN1=0;
		BIN2=0;
	}
	
	//�������������쳣����
	else
      flag_abnormal = 0;
	
	return flag_abnormal;			
}


unsigned char pick_up(float gz,float Angle,int encoder_left,int encoder_right)
{ 		   
	static u16 flag,count0,count1,count2;
	 
	if(flag==0)
	{
		//����1��С���ӽ���ֹ
		//����1��Ϊ�κ�ʱ��
		if(myabs(encoder_left)+myabs(encoder_right)<300)
			count0++;
		else 
			count0=0;
	
		//����10�νӽ���ֹ �������1
		if(count0>10)
		{		
			flag=1;
			count0=0; 
		}
	}
	
	if(flag==1)
	{
		//��ʱ���ٵȴ�(2000ms)
		if(++count1>400)
		{
			count1=0;
			flag=0;                                 
		}

		//����2��С���Ƿ���0�ȸ���������
		//��Ϊ�κ�ʱ�򶼿�������
//		if(gz>7.0f&&(Angle>(-20+PITCH_MID))&&(Angle<(20+PITCH_MID)))
			flag=2; 
	}
	
	if(flag==2)
	{
		//��ʱ���ٵȴ�(1000ms)
		if(++count2>200)       
		{
			count2=0;
			flag=0;
		}
		
		//����3�����������������ﵽ���ת��
		//��⵽С��������(return 1)
		if(myabs(encoder_left+encoder_right)>150)   
		{
			flag=0;      
			return 1;                                                              
		}
	}
	return 0;
}


unsigned char put_down(float Angle,int encoder_left,int encoder_right)
{ 		   
	static u16 flag,count;	 
	
	//����ֹͣ״̬��Ч ��ֹ���
	if(Flag_Stop==0)
		return 0;
	
	if(flag==0)                                               
	{
		//����1��С���Ƿ���0�ȸ���
		if(Angle>(-10+PITCH_MID)&&Angle<(10+PITCH_MID)&&encoder_left==0&&encoder_right==0)
		flag=1; 
	}
	
	if(flag==1)
	{
		//��ʱ���ٵȴ�(500ms)
		if(++count>100)
		{
			count=0;
			flag=0;
		}
		
		//����2��������δ�ϵ��ʱ����Ϊת��
		//��⵽С��������(return 1)
		if((encoder_left<-2)&&(encoder_right<-2)&&(encoder_left>-30)&&(encoder_right>-30))
		{
			flag=0;
			return 1;
		}
	}
	return 0;
}









