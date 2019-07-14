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
	
	//此处需要使两编码器数据极性相同
	Encoder_Left  = encoder_read(1);
	Encoder_Right = encoder_read(2);

 	Balance_Pwm  = balance_control(angle.Pitch,gyro[0]);
	
	//速度环为正反馈
	//50ms速度环
	if(speed_control_flag == 1)
	{
		Velocity_Pwm = velocity_control(Encoder_Left,Encoder_Right); 
		speed_control_flag = 0;
	}

	//5ms速度环
//	Velocity_Pwm = velocity_control(Encoder_Left,Encoder_Right);  
	
	//直接用转向环 直行无法保证
//	Turn_Pwm = turn_control(Encoder_Left,Encoder_Right,gyro[2]);
	
	//原地无法转弯
//	if((Flag_Front == 1)||(Flag_Back == 1))
//		Turn_Pwm = turn_control(Encoder_Left,Encoder_Right,gyro[2]);
//	else
//		Turn_Pwm = straight_control(Encoder_Left,Encoder_Right,gyro[2]);
	
	//改进
	if((Flag_Right != 0)||(Flag_Left != 0))
		Turn_Pwm = turn_control(Encoder_Left,Encoder_Right,gyro[2]);
	else
		Turn_Pwm = straight_control(Encoder_Left,Encoder_Right,gyro[2]);
	
	//三环并联PID
	Motor1_temp = Balance_Pwm + Velocity_Pwm - Turn_Pwm;
	Motor2_temp = Balance_Pwm + Velocity_Pwm + Turn_Pwm;

	//双环PID
//	Motor1_temp = Balance_Pwm + Velocity_Pwm;
//	Motor2_temp = Balance_Pwm + Velocity_Pwm;

	Motor1 = LIMIT_MAX_MIN(Motor1_temp,PWM_MAX,PWM_MIN);
	Motor2 = LIMIT_MAX_MIN(Motor2_temp,PWM_MAX,PWM_MIN);
	
	//检测小车拿起/放下
	if(pick_up(acc[2],angle.Pitch,Encoder_Left,Encoder_Right))
		Flag_Stop=1;
	if(put_down(angle.Pitch,Encoder_Left,Encoder_Right))
		Flag_Stop=0;

	if(abnormal_control(angle.Pitch)==0)
		Set_Pwm(Motor1,Motor2);                            
}


//直立PD控制
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
	
	//PITCH_MID为和机械相关的直立参数
	diff = Pitch_angle - real_mid;       
	
	balance = Balance_Kp * diff + Pitch_speed * Balance_Kd;  
	
	return balance;
}


//速度PI控制
int velocity_control(int encoder_left,int encoder_right)
{
	static float Velocity,Encoder_new,Encoder,Movement;
	static float Encoder_Integral,Target_Velocity=115;
	
	//可作为直线任务专用
	//梯度减小积分项-->定点的假象
	//配合4/25第一次调试参数使用
	//3s 7s换向
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
//			//注意积分项不敢清零
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

	//行进控制叠加到速度上 导致刹车回弹
	//行进控制叠加到位移上 导致刹车超出
	//因此同时打开 一定程度上相互补充 以限制刹车距离
	//但会导致间断行进 且抖动大 暂时舍弃
	
	//速度环PI控制器
	//最新速度偏差：测量速度(左右编码器之和)-目标速度(此处为零)
	//前进后退控制信息叠加到速度上
	Encoder_new = (Encoder_Left+Encoder_Right)-Movement*0.00f;//0.05f
	//速度低通滤波器
	Encoder *= 0.7;
	Encoder += Encoder_new*0.3;

	Encoder_Integral += (Encoder*2.0f);//2.0f
	
//	Encoder_Integral += Encoder;
	
	//控制前进后退
	//讲控制信息叠加到速度的积分上
	//相当于控制位移
	Encoder_Integral = Encoder_Integral-Movement;

	if(Encoder_Integral>15000)
		Encoder_Integral=15000;
	if(Encoder_Integral<-15000)
		Encoder_Integral=-15000;

	Velocity = Encoder*Velocity_Kp+Encoder_Integral*Velocity_Ki;  

	//异常关闭电机后速度积分项清零
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


//参考代码
//int turn_control(int encoder_left,int encoder_right,float az)
//{
//    static float Turn_Target,Turn,Encoder_temp,Turn_Count;
//	static float Turn_Convert = 0.7;
//	float Turn_Amplitude = 50;
//    
//	//根据旋转前的速度调整速度的起始速度 增加小车的适应性
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
	
	//异常情况1：倾角过大(当前阈值45度)
	if(angle<-35||angle>35)
	{
		flag_abnormal = 1;   

		//关闭电机
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
	
	//可添加主板过热异常处理
	else
      flag_abnormal = 0;
	
	return flag_abnormal;			
}


unsigned char pick_up(float gz,float Angle,int encoder_left,int encoder_right)
{ 		   
	static u16 flag,count0,count1,count2;
	 
	if(flag==0)
	{
		//条件1：小车接近静止
		//条件1改为任何时候
		if(myabs(encoder_left)+myabs(encoder_right)<300)
			count0++;
		else 
			count0=0;
	
		//连续10次接近静止 达成条件1
		if(count0>10)
		{		
			flag=1;
			count0=0; 
		}
	}
	
	if(flag==1)
	{
		//超时不再等待(2000ms)
		if(++count1>400)
		{
			count1=0;
			flag=0;                                 
		}

		//条件2：小车是否在0度附近被拿起
		//改为任何时候都可以拿起
//		if(gz>7.0f&&(Angle>(-20+PITCH_MID))&&(Angle<(20+PITCH_MID)))
			flag=2; 
	}
	
	if(flag==2)
	{
		//超时不再等待(1000ms)
		if(++count2>200)       
		{
			count2=0;
			flag=0;
		}
		
		//条件3：轮子因正反馈而达到最大转速
		//检测到小车被拿起(return 1)
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
	
	//仅在停止状态有效 防止误检
	if(Flag_Stop==0)
		return 0;
	
	if(flag==0)                                               
	{
		//条件1：小车是否在0度附近
		if(Angle>(-10+PITCH_MID)&&Angle<(10+PITCH_MID)&&encoder_left==0&&encoder_right==0)
		flag=1; 
	}
	
	if(flag==1)
	{
		//超时不再等待(500ms)
		if(++count>100)
		{
			count=0;
			flag=0;
		}
		
		//条件2：轮子在未上电的时候被人为转动
		//检测到小车被放下(return 1)
		if((encoder_left<-2)&&(encoder_right<-2)&&(encoder_left>-30)&&(encoder_right>-30))
		{
			flag=0;
			return 1;
		}
	}
	return 0;
}









