#include "Headfile.h"
#include "WP_Ctrl.h"
#include "control_config.h"


/*****************遥控器行程设置**********************/
//六个电机输出PWM
uint16 Motor_PWM_1,Motor_PWM_2,Motor_PWM_3,Motor_PWM_4,Motor_PWM_5,Motor_PWM_6;
//上次六个电机输出PWM
uint16 Last_Motor_PWM_1,Last_Motor_PWM_2,Last_Motor_PWM_3,Last_Motor_PWM_4,Last_Motor_PWM_5,Last_Motor_PWM_6;
//控制标志
uint8_t Controler_High_Mode=1,Last_Controler_High_Mode=1;
uint8_t Controler_Horizontal_Mode=1,Last_Controler_Horizontal_Mode=1;
uint8_t Controler_Land_Mode=1,Last_Controler_Land_Mode=1;
uint8_t Reserve_Mode=0,Last_Reserve_Mode=0;
uint8_t Reserve_Mode_Fast_Exchange_Flag=0,Reserve_Mode_Fast_Exchange_Cnt=0;
uint16_t Reserve_Mode_Cnt=0;
uint8_t Control_Mode_Change=0;
uint16_t High_Hold_Throttle=0;
uint8_t  Pos_Hold_SetFlag=0;
uint8_t SDK_Ctrl_Mode=0;


//控制模式选择函数
void Controler_Mode_Select()
{
	//保存上一次的控制模式
  Last_Controler_High_Mode=Controler_High_Mode;//上次高度控制模式
  Last_Controler_Horizontal_Mode=Controler_Horizontal_Mode;//上次位置控制模式
  Last_Controler_Land_Mode=Controler_Land_Mode;//上次返航模式
  Last_Reserve_Mode=Reserve_Mode;
	
	//遥控器5通道选择定高模式
  if(PPM_Databuf[4]>=(RC_Calibration[4].max-RC_Calibration[4].deadband))//高位    
		Controler_High_Mode=2;//气压计 超声波定高
  else if(PPM_Databuf[4]<=(RC_Calibration[4].min+RC_Calibration[4].deadband))//低位
		Controler_High_Mode=1;//纯姿态自稳 无定高
  
	//遥控器6通道选择GPS定点模式
  if(PPM_Databuf[5]>=(RC_Calibration[5].max-RC_Calibration[5].deadband))         
		Controler_Horizontal_Mode=2;//GPS控制水平位置
  else if(PPM_Databuf[5]<=(RC_Calibration[5].min+RC_Calibration[5].deadband))   
		Controler_Horizontal_Mode=1;//无GPS控制水平位置
  
	//遥控器7通道高低位选择返航模式
  if(PPM_Databuf[6]>=(RC_Calibration[6].max-RC_Calibration[6].deadband))            
		Controler_Land_Mode=2;//返航模式
  else if(PPM_Databuf[6]<=(RC_Calibration[6].middle+RC_Calibration[6].deadband))   
		Controler_Land_Mode=1;//非返航模式
  
	//遥控器7通道中位选择SDK模式开关
  if(PPM_Databuf[6]>=(RC_Calibration[6].middle-RC_Calibration[6].deadband)
     &&PPM_Databuf[6]<=(RC_Calibration[6].middle+RC_Calibration[6].deadband))//中位  
    SDK_Ctrl_Mode=1;//开启SDK模式
  else
  {
    SDK_Ctrl_Mode=0;//关闭SDK模式
		NCQ_SDK_Reset();//SDK任务标志清除
  }
  
	//遥控器8通道 (保留通道)
  if(PPM_Databuf[7]>=(RC_Calibration[7].max-RC_Calibration[7].deadband))      
		Reserve_Mode=2;//保留模式2——光流模式
  else if(PPM_Databuf[7]<=(RC_Calibration[7].min+RC_Calibration[7].deadband)) 
  {
    Reserve_Mode=1;//保留模式1
		//光流复位
    OpticalFlow_SINS_Reset();
    OpticalFlow_Ctrl_Reset();
  }
  
	//8通道快速切换计时
  if(Reserve_Mode_Cnt>=1) Reserve_Mode_Cnt--;
  if(Reserve_Mode_Cnt==0) Reserve_Mode_Fast_Exchange_Cnt=0;
	
	//保留模式切换
  if(Last_Reserve_Mode!=Reserve_Mode)
  {
		//保留模式切换至光流模式
    if(Reserve_Mode==2)
    {
      
    }
		//保留模式切换至1
    if(Reserve_Mode==1)
    {
      Reserve_Mode_Cnt=600;	//3s倒计时
      Reserve_Mode_Fast_Exchange_Cnt++;	//快速切换计数
			
			//短时间连续切换保留通道3次(每次到达保留模式1(低位)才算一次快速切换)
      if(Reserve_Mode_Fast_Exchange_Cnt>=3)
      {
        Reserve_Mode_Fast_Exchange_Flag=1;	//标志快速切换完成
        Reserve_Mode_Fast_Exchange_Cnt=0;		
				
				//设置自动起飞高度
        Total_Controller.High_Position_Control.Expect=NamelessQuad.Position[_YAW]+Auto_Launch_Target;
        //屏蔽自锁
				Unwanted_Lock_Flag=0;
      }
    }
  }
  
	//8通达3次快速切换完成 则自动起飞
  if(Reserve_Mode_Fast_Exchange_Flag==1)
  {
		//设置较大的参数为了完成自动起飞
    Total_Controller.High_Acce_Control.Scale_Kp=1.0f;
    Total_Controller.High_Acce_Control.Scale_Ki=1.2f; 
    Total_Controller.High_Speed_Control.Scale_Kp=1.2;
    Total_Controller.High_Speed_Control.Scale_Ki=1.2f;
    Total_Controller.High_Position_Control.Scale_Kp=2.0;
    Total_Controller.High_Position_Control.Scale_Ki=1.0f; 
		
		//当反馈高度大于期望高度(快速起飞完成)则恢复正常参数
    if(Total_Controller.High_Position_Control.Expect<=NamelessQuad.Position[_YAW])
      Reserve_Mode_Fast_Exchange_Flag=0;   
  }
  else 
  { 
		//非快速起飞模式 正常参数
    Total_Controller.High_Acce_Control.Scale_Kp=1.0f;
    Total_Controller.High_Acce_Control.Scale_Ki=1.0f; 
    Total_Controller.High_Speed_Control.Scale_Kp=1.0;
    Total_Controller.High_Speed_Control.Scale_Ki=1.0f; 
    Total_Controller.High_Position_Control.Scale_Kp=1.0;
    Total_Controller.High_Position_Control.Scale_Ki=1.0f; 
  }
  
	//允许自动上锁(定高模式解锁后无任何操作)
  if(Unwanted_Lock_Flag==1)
  {
		//获取油门推动状态
    Thr_Push_Over_State=Thr_Push_Over_Deadband();
		//(自动模式)从中位推到高位 则屏蔽自动上锁
    if(Thr_Push_Over_State==2)
      Unwanted_Lock_Flag=0;
    else
    {
      Take_Off_Reset();//清积分
      //Throttle_Control_Reset();//清积分
    }
  }
  
	//返航模式切换
  if(Controler_Land_Mode!=Last_Controler_Land_Mode)
  {
    if(Controler_Land_Mode==1)  //非返航模式
      land_reset();
		//将开关拨动瞬间的惯导高度设置为期望高度
    Total_Controller.High_Position_Control.Expect=NamelessQuad.Position[_YAW];
  }
  
	//定高模式切换
  if(Controler_High_Mode!=Last_Controler_High_Mode)
  {
		//自稳切定高 设置基准油门值和悬停高度
    if(Controler_High_Mode==2)  Control_Mode_Change=1;
		//定高切自稳
    if(Controler_High_Mode==1)  Control_Mode_Change=1;
  }
  
	//GPS模式切换
  if(Controler_Horizontal_Mode!=Last_Controler_Horizontal_Mode)
  {
		//自稳切定点 设置悬停点
    if(Controler_Horizontal_Mode==2)  
		{
			Control_Mode_Change=2;
			Pos_Hold_SetFlag=0;
		}
		//定点切自稳
    if(Controler_Horizontal_Mode==1)  
		{
			Control_Mode_Change=2;
			Pos_Hold_SetFlag=1;
		}
  }
  
	//定高模式切换的处理
  if(Control_Mode_Change==1)
  {
		//本次为定高模式(2) 即自稳切定高
    if(Controler_High_Mode==High_Hold_Mode)
    {
			//保存当前油门值(只存一次)
      High_Hold_Throttle=Throttle_Control;
      //将开关拨动瞬间的惯导高度设置为期望高度(高度只设置一次)
      Total_Controller.High_Position_Control.Expect=NamelessQuad.Position[_YAW];
    }
    else	//定高切自稳
    {
      //Throttle_Control_Reset();
    }
		//将模式切换位置0 ->仅处理一次
    Control_Mode_Change=0;
  }
	//定点模式切换的处理
  else if(Control_Mode_Change==2)
  {
		//本次为定点模式 即切换到定点
    if(Controler_Horizontal_Mode==Pos_Hold_Mode)
    {
			//满足设置悬停点条件(当前未设置悬停点 且 GPS有效)
      if(Pos_Hold_SetFlag==0&&(GPS_ok()==TRUE))
      {
        //将当前惯导水平位置估计作为目标悬停点
        Total_Controller.Latitude_Position_Control.Expect=NamelessQuad.Position[_ROLL];
        Total_Controller.Longitude_Position_Control.Expect=NamelessQuad.Position[_PITCH];
				
				//清空水平速度和位置控制的积分
        PID_Integrate_Reset(&Total_Controller.Latitude_Speed_Control);		
        PID_Integrate_Reset(&Total_Controller.Latitude_Position_Control);	
        PID_Integrate_Reset(&Total_Controller.Longitude_Speed_Control);		
        PID_Integrate_Reset(&Total_Controller.Longitude_Position_Control);
				
        Pos_Hold_SetFlag=1;
      }
    }
    else	//关闭定点
    {
			//清空水平速度和位置控制的积分			
      PID_Integrate_Reset(&Total_Controller.Latitude_Speed_Control);			
      PID_Integrate_Reset(&Total_Controller.Latitude_Position_Control);		
      PID_Integrate_Reset(&Total_Controller.Longitude_Speed_Control);			
      PID_Integrate_Reset(&Total_Controller.Longitude_Position_Control);	
    }
		//标志定点切换响应完成
    Control_Mode_Change=0;
  }
  
  //当前档位为定点模式 但显示悬停点未设置 说明之前未满足设置定点条件
	//有三种情况：
  //	1、初始通过开关切定点模式时 GPS状态未满足悬停条件；
  //	2、初始通过开关切定点模式时 GPS状态未满足悬停条件 之后持续检测仍然未满足GPS定点条件；
  //	3、之前GPS状态满足悬停条件 但由于GPS信号质量变差 自动切换至不满足GPS定点条件；
  //处理策略：
	//	重新判断当下是否满足定点条件 如满足条件更新悬停点 允许进入定点模式
  if(Controler_Horizontal_Mode==2)
  {
		//首次切定点不满足定点条件 之后又满足定点条件
    if(GPS_ok()==TRUE)
    {
			//未设置悬停点(注意仅设置一次)
      if(Pos_Hold_SetFlag==0)
      {
        //将当前惯导水平位置估计作为目标悬停点
        Total_Controller.Latitude_Position_Control.Expect=NamelessQuad.Position[_ROLL];
        Total_Controller.Longitude_Position_Control.Expect=NamelessQuad.Position[_PITCH];
				
				//清空水平速度和位置控制的积分
        PID_Integrate_Reset(&Total_Controller.Latitude_Speed_Control);
        PID_Integrate_Reset(&Total_Controller.Latitude_Position_Control);
        PID_Integrate_Reset(&Total_Controller.Longitude_Speed_Control);
        PID_Integrate_Reset(&Total_Controller.Longitude_Position_Control);
				
        Pos_Hold_SetFlag=1;
      }
    }
    else	
    {
			//定点档位处于定点模式 但未满足定点条件 
			//将Pos_Hold_SetFlag置0 等待满足时再设置悬停点
      Pos_Hold_SetFlag=0;
			
			//清空水平速度和位置控制的积分
      PID_Integrate_Reset(&Total_Controller.Latitude_Speed_Control);		
      PID_Integrate_Reset(&Total_Controller.Latitude_Position_Control);	
      PID_Integrate_Reset(&Total_Controller.Longitude_Speed_Control);		
      PID_Integrate_Reset(&Total_Controller.Longitude_Position_Control);
    }
  }
}


//幅值限制
uint16 Value_Limit(uint16 Min,uint16 Max,uint16 Data)
{
  if(Data>=Max) Data=Max;
  else if(Data<=Min) Data=Min;
  return Data;
}


//角度控制
void Angle_Control()
{
  static uint16 Yaw_Cnt=0;
  
	//平衡角反馈及控制
  Total_Controller.Pitch_Angle_Control.FeedBack=(Pitch-Pitch_Offset);
  PID_Control(&Total_Controller.Pitch_Angle_Control);
  Total_Controller.Roll_Angle_Control.FeedBack =(Roll-Roll_Offset);
  PID_Control(&Total_Controller.Roll_Angle_Control);
  
  if(Yaw_Control==0)//偏航杆置于中位
  {
		//无头模式
		//飞机上电后一段时间锁定偏航角 
		//磁力计 陀螺仪融合需要一段时间 这里取500
    if(Yaw_Cnt<=500)
      Yaw_Cnt++;
		
    if(Total_Controller.Yaw_Angle_Control.Expect==0	//偏航无角度控制量 
				||Yaw_Cnt<=500															//无头模式
         ||Landon_Earth_Flag==1)										//满足触地条件
			//将当前偏航角设置为期望值
      Total_Controller.Yaw_Angle_Control.Expect=Yaw;
		
		//偏航角反馈及控制
    Total_Controller.Yaw_Angle_Control.FeedBack=Yaw;
    PID_Control_Yaw(&Total_Controller.Yaw_Angle_Control);
		
		//偏航角速度环期望 来源于偏航角度控制器(外环)输出
    Total_Controller.Yaw_Gyro_Control.Expect=Total_Controller.Yaw_Angle_Control.Control_OutPut;
    
		//SDK巡线模式下 偏航受SDK控制
    if(SDK_Line.line_ctrl_enable==1&&SDK_Ctrl_Mode==1)
    {
			//SDK仅控制偏航角速度 而不直接进行角度控制
      Total_Controller.Yaw_Gyro_Control.Expect=3*SDK_Target_Yaw_Gyro;
      Total_Controller.Yaw_Angle_Control.Expect=0;
      //SDK_Line.flag=0;
    }
  }
  else	//波动偏航方向杆后
  {
		//仅控制偏航角速度 不对角度进行控制
    Total_Controller.Yaw_Angle_Control.Expect=0;
    Total_Controller.Yaw_Gyro_Control.Expect=Yaw_Control;
  }
}


uint16_t Yaw_Control_Fault_Cnt=0;
float Last_Yaw_Gyro_Control_Expect=0,Yaw_Gyro_Control_Expect_Delta=0;
float Last_Pitch_Gyro_Control_Expect=0,Pitch_Gyro_Control_Expect_Delta=0;
float Last_Roll_Gyro_Control_Expect=0,Roll_Gyro_Control_Expect_Delta=0;
//平衡角前馈控制参数(依据期望)(此处无)
float Pitch_Roll_Feedforward_Kp=0.0f,Pitch_Roll_Feedforward_Kd=0.0f;//0.45
//偏航角前馈控制参数
float Yaw_Feedforward_Kp=0.0f,Yaw_Feedforward_Kd=0.05f;
//角速度控制
void Gyro_Control()
{
  if(GYRO_CONTROL_MODE==PID_MODE)//俯仰、横滚方向姿态内环角速度控制器采用PID控制器
  {
    //角速度环期望
    Total_Controller.Pitch_Gyro_Control.Expect=Total_Controller.Pitch_Angle_Control.Control_OutPut;
    Total_Controller.Roll_Gyro_Control.Expect=Total_Controller.Roll_Angle_Control.Control_OutPut;
    
    //角速度环反馈
    Total_Controller.Pitch_Gyro_Control.FeedBack=Pitch_Gyro;
    Total_Controller.Roll_Gyro_Control.FeedBack=Roll_Gyro;
     
    //角速度环控制 (微分参数动态调整)
    PID_Control_Div_LPF_For_Gyro(&Total_Controller.Pitch_Gyro_Control);
    PID_Control_Div_LPF_For_Gyro(&Total_Controller.Roll_Gyro_Control);
    
		//平衡角速度控制器期望的差分
    Pitch_Gyro_Control_Expect_Delta=1000*(Total_Controller.Pitch_Gyro_Control.Expect-Last_Pitch_Gyro_Control_Expect
                                          /Total_Controller.Pitch_Gyro_Control.PID_Controller_Dt.Time_Delta);
    Roll_Gyro_Control_Expect_Delta=1000*(Total_Controller.Roll_Gyro_Control.Expect-Last_Roll_Gyro_Control_Expect
                                         /Total_Controller.Roll_Gyro_Control.PID_Controller_Dt.Time_Delta);
    
		//保存上一次平衡角速度期望
    Last_Pitch_Gyro_Control_Expect=Total_Controller.Pitch_Gyro_Control.Expect;
    Last_Roll_Gyro_Control_Expect=Total_Controller.Roll_Gyro_Control.Expect;
    
		//PITCH轴前馈控制
    Total_Controller.Pitch_Gyro_Control.Control_OutPut+=Pitch_Roll_Feedforward_Kd*Pitch_Gyro_Control_Expect_Delta//微分
      +Pitch_Roll_Feedforward_Kp*Total_Controller.Pitch_Gyro_Control.Expect;//比例
		//输出限幅
    Total_Controller.Pitch_Gyro_Control.Control_OutPut=constrain_float(Total_Controller.Pitch_Gyro_Control.Control_OutPut,
                                                                       -Total_Controller.Pitch_Gyro_Control.Control_OutPut_Limit,
                                                                       Total_Controller.Pitch_Gyro_Control.Control_OutPut_Limit);
    
		//ROLL轴前馈控制
    Total_Controller.Roll_Gyro_Control.Control_OutPut+=Pitch_Roll_Feedforward_Kd*Roll_Gyro_Control_Expect_Delta
      +Pitch_Roll_Feedforward_Kp*Total_Controller.Roll_Gyro_Control.Expect;
		//输出限幅
    Total_Controller.Roll_Gyro_Control.Control_OutPut=constrain_float(Total_Controller.Roll_Gyro_Control.Control_OutPut,
                                                                      -Total_Controller.Roll_Gyro_Control.Control_OutPut_Limit,
                                                                      Total_Controller.Roll_Gyro_Control.Control_OutPut_Limit);
  }
	//平衡角速度控制采用ADRC控制器
  else if(GYRO_CONTROL_MODE==ADRC_MODE)
  {
    
  }
  else	//(测试用)正常只选择一种模式
  {
    //内环角速度期望
    Total_Controller.Pitch_Gyro_Control.Expect=Total_Controller.Pitch_Angle_Control.Control_OutPut;
    Total_Controller.Roll_Gyro_Control.Expect=Total_Controller.Roll_Angle_Control.Control_OutPut;
    //内环角速度反馈
    Total_Controller.Pitch_Gyro_Control.FeedBack=Pitch_Gyro;
    Total_Controller.Roll_Gyro_Control.FeedBack=Roll_Gyro;
    //内环角速度控制
    PID_Control_Div_LPF(&Total_Controller.Pitch_Gyro_Control);
    PID_Control_Div_LPF(&Total_Controller.Roll_Gyro_Control);
  }
  
	//角速度环反馈
  //Total_Controller.Yaw_Gyro_Control.FeedBack=Yaw_Gyro;
  Total_Controller.Yaw_Gyro_Control.FeedBack=Yaw_Gyro_Earth_Frame;//加速度计计算得到的偏航角速度
  
  PID_Control_Div_LPF(&Total_Controller.Yaw_Gyro_Control);
	
	//偏航角速度控制器期望的差分
  Yaw_Gyro_Control_Expect_Delta=1000*(Total_Controller.Yaw_Gyro_Control.Expect-Last_Yaw_Gyro_Control_Expect)
    /Total_Controller.Yaw_Gyro_Control.PID_Controller_Dt.Time_Delta;
	
  //偏航角前馈控制
  Total_Controller.Yaw_Gyro_Control.Control_OutPut+=Yaw_Feedforward_Kp*Total_Controller.Yaw_Gyro_Control.Expect
    +Yaw_Feedforward_Kd*Yaw_Gyro_Control_Expect_Delta;
  Total_Controller.Yaw_Gyro_Control.Control_OutPut=constrain_float(Total_Controller.Yaw_Gyro_Control.Control_OutPut,
                                                                   -Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit,
                                                                   Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit);
	
  Last_Yaw_Gyro_Control_Expect=Total_Controller.Yaw_Gyro_Control.Expect;
  
	//偏航控制异常情况判断：
	//	即偏航控制量很大时 偏航角速度很小 如此时为强外力干扰、已着地等
  if(ABS(Total_Controller.Yaw_Gyro_Control.Control_OutPut)>Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit/2	//偏航控制输出相对较大
     &&ABS(Yaw_Gyro)<=30.0f)	//偏航角速度相对很小
  {
		//特殊情况计时
    Yaw_Control_Fault_Cnt++;
    if(Yaw_Control_Fault_Cnt>=500) Yaw_Control_Fault_Cnt=500;
  }
  else Yaw_Control_Fault_Cnt/=2;//不满足 快速削减至0
  
	//特殊情况持续5ms*400=2S 特殊处理
  if(Yaw_Control_Fault_Cnt>=400)
  {
		//清空偏航控制积分
    PID_Integrate_Reset(&Total_Controller.Yaw_Gyro_Control);
    PID_Integrate_Reset(&Total_Controller.Yaw_Angle_Control);
		//将当前偏航角作为期望偏航角
    Total_Controller.Yaw_Angle_Control.Expect=Yaw;
    Yaw_Control_Fault_Cnt=0;
  }
}


uint16 Throttle=0,Last_Throttle=0;
//主导控制器
void Main_Leading_Control(void)
{
  //飞控模式
  if(Controler_Land_Mode==1)//非返航着陆模式
  {
		//自稳模式
    if(Controler_High_Mode==1						//姿态自稳定模式
       &&Controler_Horizontal_Mode==1)	//GPS定点未开启
    {
			//平衡角度直接来自遥控器
      Total_Controller.Pitch_Angle_Control.Expect=Target_Angle[0];
      Total_Controller.Roll_Angle_Control.Expect=Target_Angle[1];
      
      if(Throttle_Control<=1000)   Throttle=1000;
			//油门直接来源于遥控器油门给定
      else Throttle=Throttle_Control;
			
			//保存上一次油门值
      Last_Throttle=Throttle;
    }
    else if(Controler_High_Mode==2		//定高模式
			&&Controler_Horizontal_Mode==1)	//GPS定点未开启
    {
     //定高模式
#if (Optical_Enable==0)
			//无光流则水平角度直接来自遥控器
			Total_Controller.Pitch_Angle_Control.Expect=Target_Angle[0];
			Total_Controller.Roll_Angle_Control.Expect=Target_Angle[1];
#else 
		 //光流定点模式
			//超声波有效且存在光流外设时 才允许进入光流模式 
			if(Reserve_Mode==2&&Sensor_Flag.Hcsr04_Health==1&&OpticalFlow_Is_Work==1) 
      {  
				//无限数传和OpenMV参与的SDK模式
        if(SDK_Take_Over_Ctrl==1)       
        {
					//光流定点
          OpticalFlow_Control(0);
          //高度控制
					ncq_control_althold();
        }
				//用户自定义的SDK模式
        else if(SDK_Take_Over_Ctrl==2)  
        {
					//自定义SDK模式1
					if(SDK_Ctrl_Mode==1)
						NCQ_SDK_Run();
          else //无SDK模式
					{
						OpticalFlow_Control_Pure(0);
						ncq_control_althold();
					}
        }
      }
      else //不进入光流模式
      {
        Total_Controller.Pitch_Angle_Control.Expect=Target_Angle[0];
        Total_Controller.Roll_Angle_Control.Expect=Target_Angle[1];
      }
#endif
      ncq_control_althold();//高度控制
    }
    else if(Controler_High_Mode==2		//定高模式
			&&Controler_Horizontal_Mode==2)	//GPS定点开启
    {
      ncq_control_althold();//高度控制
      ncq_control_poshold();//位置控制
    }
    else	//其它
    {
      Total_Controller.Pitch_Angle_Control.Expect=Target_Angle[0];
      Total_Controller.Roll_Angle_Control.Expect=Target_Angle[1];
			
      if(Throttle_Control<=1000)   Throttle=1000;
			else Throttle=Throttle_Control;
      Last_Throttle=Throttle;
    }
    land_state_check();
  }
  else //炸鸡紧急减速模式
  {
		//此模式原本为自动返航/自动降落模式
		//land_run();
		
		//PWMGenDisable(PWM0_BASE, PWM_GEN_0);
		//PWMGenDisable(PWM0_BASE, PWM_GEN_1);
		//PWMGenDisable(PWM0_BASE, PWM_GEN_2);
		//PWMGenDisable(PWM0_BASE, PWM_GEN_3);
		
		PWM_Output(Thr_Min,Thr_Min,Thr_Min,Thr_Min,Thr_Min,Thr_Min,0,0);
  }
}


//姿态总控制器
void Attitude_Control(void)
{
  Angle_Control();//角度控制
  Gyro_Control();	//角速度控制
}


float Active_Para1,Active_Para2;
//总控制器
void Total_Control(void)
{
  Main_Leading_Control();	//主导控制器		
  Attitude_Control();			//姿态控制器
}


//飞行器控制
void CarryPilot_Control(void)
{
  static uint8_t ctrl_cnt=0;
  ctrl_cnt++;  

	//控制模式选择  
	if(ctrl_cnt>=4)
  {  
    Controler_Mode_Select();
    ctrl_cnt=0;
  }
	
	//总控制器
	//水平位置+水平速度+姿态(角度+角速度)控制
	//高度位置+高度速度+高度加速度控制	
  Total_Control();
	
	//控制量总输出
  Control_Output();
}


uint16_t Throttle_Output=0;
//油门倾角补偿
//	因为高度控制计算出的结果为竖直方向上的期望升力
//	需要投影到机体坐标系得到旋翼产生的总升力
//	控制环节：1/CosPitch_CosRoll
void Throttle_Angle_Compensate()
{
  float CosPitch_CosRoll=ABS(Cos_Pitch*Cos_Roll);
  float Throttle_Makeup=0;
  float Temp=0;
	
  if(CosPitch_CosRoll>=0.999999f) CosPitch_CosRoll=0.999999f;
  if(CosPitch_CosRoll<=0.000001f) CosPitch_CosRoll=0.000001f;
  if(CosPitch_CosRoll<=0.50f)  		CosPitch_CosRoll=0.50f;//Pitch,Roll约等于30度
  
	if(Throttle>=Thr_Start)//大于起转油门量
  {
    Temp=(uint16_t)(MAX(ABS(100*Pitch),ABS(100*Roll)));
    Temp=constrain_float(9000-Temp,0,3000)/(3000*CosPitch_CosRoll);
		
		//油门倾角补偿
    Throttle_Makeup=(Throttle-Thr_Start)*Temp;
		
    Throttle_Output=(uint16_t)(Thr_Start+Throttle_Makeup);
    Throttle_Output=(uint16_t)(constrain_float(Throttle_Output,Thr_Start,2000));
  }
  else Throttle_Output=Throttle;
}


/*

X型安装方式 电机序号与姿态角关系
                   -
                 Pitch
					3#             1#
					   *          *
-   Roll          *         Roll   +
						 *          *
					2#             4#
				         Pitch
								   +
加速度传感器轴向与载体X、Y、Z同轴
沿轴向原点看 逆时针旋转角度为+
	Y Aixs
	*
	*
	*
	*
	*
	*
	* * * * * * * *   X Axis

*/
uint16_t Idel_Cnt=0;
//怠速递增间隔时间 10*5=50ms
#define Idel_Transition_Gap 10
//怠速启动过渡过程 最大计数器 50ms*100=5s
#define Idel_Transition_Period (Thr_Idle-Thr_Min)
uint16_t Thr_Idle_Transition_Cnt=0;
//控制器输出
void Control_Output()
{
  Throttle_Angle_Compensate();//油门倾角补偿
  landon_earth_check();//着陆条件自检
	
	//控制器处于解锁状态
  if(Controler_State==Unlock_Controler)
  {
		//检测到着陆条件
    if(Landon_Earth_Flag==1)
    {
      if(Last_Motor_PWM_1<=Thr_Min
         &&Last_Motor_PWM_2<=Thr_Min
           &&Last_Motor_PWM_3<=Thr_Min
             &&Last_Motor_PWM_4<=Thr_Min
               //&&Last_Motor_PWM_5<=Thr_Min
								//&&Last_Motor_PWM_6<=Thr_Min
									)//只有上锁后再解锁时才会满足
      {
        //如果上次油门输出值为最低位 进入怠速时 则安排过渡过程
        Thr_Idle_Transition_Cnt=Idel_Transition_Period;
      }
      else	//其他时刻检测到着陆条件
      {
				//上次为起飞状态 本次为着陆状态 则上锁电机
        if(Last_Landon_Earth_Flag==0)
        {
          Controler_State=Lock_Controler;
					
					//LED控制
          Bling_Set(&Light_1,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
          Bling_Set(&Light_2,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
          Bling_Set(&Light_3,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
        }
      }
      
      Idel_Cnt++;//怠速计时
			
			//到达怠速递增间隔时间(50ms)
      if(Idel_Cnt>=Idel_Transition_Gap)
      {
        if(Thr_Idle_Transition_Cnt>=1)
          Thr_Idle_Transition_Cnt--;//实现速度递增的怠速
        Idel_Cnt=0;
      }
			
			//油门递增的怠速
      Motor_PWM_1=Thr_Min+(Idel_Transition_Period-Thr_Idle_Transition_Cnt)*(Thr_Idle-Thr_Min)/Idel_Transition_Period;
      Motor_PWM_2=Thr_Min+(Idel_Transition_Period-Thr_Idle_Transition_Cnt)*(Thr_Idle-Thr_Min)/Idel_Transition_Period;
      Motor_PWM_3=Thr_Min+(Idel_Transition_Period-Thr_Idle_Transition_Cnt)*(Thr_Idle-Thr_Min)/Idel_Transition_Period;
      Motor_PWM_4=Thr_Min+(Idel_Transition_Period-Thr_Idle_Transition_Cnt)*(Thr_Idle-Thr_Min)/Idel_Transition_Period;
      Motor_PWM_5=Thr_Min+(Idel_Transition_Period-Thr_Idle_Transition_Cnt)*(Thr_Idle-Thr_Min)/Idel_Transition_Period;
      Motor_PWM_6=Thr_Min+(Idel_Transition_Period-Thr_Idle_Transition_Cnt)*(Thr_Idle-Thr_Min)/Idel_Transition_Period;
			
      Take_Off_Reset();//清积分
			
			//清除光流残留数据
      OpticalFlow_SINS_Reset();
      OpticalFlow_Ctrl_Reset();
    }
		
    else  //解锁后不满足着陆条件 默认起飞
    {
      if(Controler_High_Mode==1)//姿态档位
      {
        if(Throttle>=Thr_Fly_Start)//大于起飞油门
        {
          if(GYRO_CONTROL_MODE==PID_MODE)//水平姿态环角速度控制器来源于PID
          {
            Motor_PWM_1=Int_Sort(Moter1_Thr_Scale*Throttle_Output
                                   +Moter1_Roll_Scale*Total_Controller.Roll_Gyro_Control.Control_OutPut
                                     +Moter1_Pitch_Scale*Total_Controller.Pitch_Gyro_Control.Control_OutPut
                                       +Moter1_Yaw_Scale*Total_Controller.Yaw_Gyro_Control.Control_OutPut);
            Motor_PWM_2=Int_Sort(Moter2_Thr_Scale*Throttle_Output
                                   +Moter2_Roll_Scale*Total_Controller.Roll_Gyro_Control.Control_OutPut
                                     +Moter2_Pitch_Scale*Total_Controller.Pitch_Gyro_Control.Control_OutPut
                                       +Moter2_Yaw_Scale*Total_Controller.Yaw_Gyro_Control.Control_OutPut);
            Motor_PWM_3=Int_Sort(Moter3_Thr_Scale*Throttle_Output
                                   +Moter3_Roll_Scale*Total_Controller.Roll_Gyro_Control.Control_OutPut
                                     +Moter3_Pitch_Scale*Total_Controller.Pitch_Gyro_Control.Control_OutPut
                                       +Moter3_Yaw_Scale*Total_Controller.Yaw_Gyro_Control.Control_OutPut);
            Motor_PWM_4=Int_Sort(Moter4_Thr_Scale*Throttle_Output
                                   +Moter4_Roll_Scale*Total_Controller.Roll_Gyro_Control.Control_OutPut
                                     +Moter4_Pitch_Scale*Total_Controller.Pitch_Gyro_Control.Control_OutPut
                                       +Moter4_Yaw_Scale*Total_Controller.Yaw_Gyro_Control.Control_OutPut);
            Motor_PWM_5=Int_Sort(Moter5_Thr_Scale*Throttle_Output
                                   +Moter5_Roll_Scale*Total_Controller.Roll_Gyro_Control.Control_OutPut
                                     +Moter5_Pitch_Scale*Total_Controller.Pitch_Gyro_Control.Control_OutPut
                                       +Moter5_Yaw_Scale*Total_Controller.Yaw_Gyro_Control.Control_OutPut);
            Motor_PWM_6=Int_Sort(Moter6_Thr_Scale*Throttle_Output
                                   +Moter6_Roll_Scale*Total_Controller.Roll_Gyro_Control.Control_OutPut
                                     +Moter6_Pitch_Scale*Total_Controller.Pitch_Gyro_Control.Control_OutPut
                                       +Moter6_Yaw_Scale*Total_Controller.Yaw_Gyro_Control.Control_OutPut);
          }
					
          else//水平姿态环角速度读控制器来源于ADRC
          {
            
          }
					
        }
        else	//小于起飞油门 则油门不受姿态控制
        {
          Motor_PWM_1=Int_Sort(Throttle_Output);
          Motor_PWM_2=Int_Sort(Throttle_Output);
          Motor_PWM_3=Int_Sort(Throttle_Output);
          Motor_PWM_4=Int_Sort(Throttle_Output);
          Motor_PWM_5=Int_Sort(Throttle_Output);
          Motor_PWM_6=Int_Sort(Throttle_Output);
					
          Take_Off_Reset();//清积分
        }
				
				//PWM输出限幅
        Motor_PWM_1=Value_Limit(Thr_Idle,2000,Motor_PWM_1);
        Motor_PWM_2=Value_Limit(Thr_Idle,2000,Motor_PWM_2);
        Motor_PWM_3=Value_Limit(Thr_Idle,2000,Motor_PWM_3);
        Motor_PWM_4=Value_Limit(Thr_Idle,2000,Motor_PWM_4);
        Motor_PWM_5=Value_Limit(Thr_Idle,2000,Motor_PWM_5);
        Motor_PWM_6=Value_Limit(Thr_Idle,2000,Motor_PWM_6);
				
				Throttle_Control_Reset();
      }
			
			//定高档位 油门托管
      else if(Controler_High_Mode==2)
      {
				//相比于自稳档位 仅没有判断是否大于起飞油门
				
        if(GYRO_CONTROL_MODE==PID_MODE)//水平姿态环角速度读控制器来源于PID
        {
          Motor_PWM_1=Int_Sort(Moter1_Thr_Scale*Throttle_Output
                                 +Moter1_Roll_Scale*Total_Controller.Roll_Gyro_Control.Control_OutPut
                                   +Moter1_Pitch_Scale*Total_Controller.Pitch_Gyro_Control.Control_OutPut
                                     +Moter1_Yaw_Scale*Total_Controller.Yaw_Gyro_Control.Control_OutPut);
          Motor_PWM_2=Int_Sort(Moter2_Thr_Scale*Throttle_Output
                                 +Moter2_Roll_Scale*Total_Controller.Roll_Gyro_Control.Control_OutPut
                                   +Moter2_Pitch_Scale*Total_Controller.Pitch_Gyro_Control.Control_OutPut
                                     +Moter2_Yaw_Scale*Total_Controller.Yaw_Gyro_Control.Control_OutPut);
          Motor_PWM_3=Int_Sort(Moter3_Thr_Scale*Throttle_Output
                                 +Moter3_Roll_Scale*Total_Controller.Roll_Gyro_Control.Control_OutPut
                                   +Moter3_Pitch_Scale*Total_Controller.Pitch_Gyro_Control.Control_OutPut
                                     +Moter3_Yaw_Scale*Total_Controller.Yaw_Gyro_Control.Control_OutPut);
          Motor_PWM_4=Int_Sort(Moter4_Thr_Scale*Throttle_Output
                                 +Moter4_Roll_Scale*Total_Controller.Roll_Gyro_Control.Control_OutPut
                                   +Moter4_Pitch_Scale*Total_Controller.Pitch_Gyro_Control.Control_OutPut
                                     +Moter4_Yaw_Scale*Total_Controller.Yaw_Gyro_Control.Control_OutPut);
          Motor_PWM_5=Int_Sort(Moter5_Thr_Scale*Throttle_Output
                                 +Moter5_Roll_Scale*Total_Controller.Roll_Gyro_Control.Control_OutPut
                                   +Moter5_Pitch_Scale*Total_Controller.Pitch_Gyro_Control.Control_OutPut
                                     +Moter5_Yaw_Scale*Total_Controller.Yaw_Gyro_Control.Control_OutPut);
          Motor_PWM_6=Int_Sort(Moter6_Thr_Scale*Throttle_Output
                                 +Moter6_Roll_Scale*Total_Controller.Roll_Gyro_Control.Control_OutPut
                                   +Moter6_Pitch_Scale*Total_Controller.Pitch_Gyro_Control.Control_OutPut
                                     +Moter6_Yaw_Scale*Total_Controller.Yaw_Gyro_Control.Control_OutPut);
        }
				
        else//水平姿态环角速度读控制器来源于ADRC
        { 
          
        }
				
				//PWM输出限幅
        Motor_PWM_1=Value_Limit(Thr_Idle,2000,Motor_PWM_1);
        Motor_PWM_2=Value_Limit(Thr_Idle,2000,Motor_PWM_2);
        Motor_PWM_3=Value_Limit(Thr_Idle,2000,Motor_PWM_3);
        Motor_PWM_4=Value_Limit(Thr_Idle,2000,Motor_PWM_4);
        Motor_PWM_5=Value_Limit(Thr_Idle,2000,Motor_PWM_5);
        Motor_PWM_6=Value_Limit(Thr_Idle,2000,Motor_PWM_6);
      }
    }
  }
  else	//未解锁 油门置最低 电机停转
  {
		Motor_PWM_1=Thr_Min;
		Motor_PWM_2=Thr_Min;
		Motor_PWM_3=Thr_Min;
		Motor_PWM_4=Thr_Min;
		Motor_PWM_5=Thr_Min;
		Motor_PWM_6=Thr_Min;
		
		Take_Off_Reset();//清积分
		
		Throttle_Control_Reset();
	}
	
	//保存上一次的PWM输出
  Last_Motor_PWM_1=Motor_PWM_1;
  Last_Motor_PWM_2=Motor_PWM_2;
  Last_Motor_PWM_3=Motor_PWM_3;
  Last_Motor_PWM_4=Motor_PWM_4;
  Last_Motor_PWM_5=Motor_PWM_5;
  Last_Motor_PWM_6=Motor_PWM_6;
  
	//PWM总输出限幅
  Motor_PWM_1=Value_Limit(0,2000,Motor_PWM_1);
  Motor_PWM_2=Value_Limit(0,2000,Motor_PWM_2);
  Motor_PWM_3=Value_Limit(0,2000,Motor_PWM_3);
  Motor_PWM_4=Value_Limit(0,2000,Motor_PWM_4);
  Motor_PWM_5=Value_Limit(0,2000,Motor_PWM_5);
  Motor_PWM_6=Value_Limit(0,2000,Motor_PWM_6);
	
	//输出增益？
  PWM_Output(1.25*Motor_PWM_1,1.25*Motor_PWM_2,1.25*Motor_PWM_3,1.25*Motor_PWM_4,1.25*Motor_PWM_5,1.25*Motor_PWM_6,0,0);
}





