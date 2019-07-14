#include "Headfile.h"
#include "NamelessCotrun_SDK.h"


//SDK本模式需要在光流模式下使用


//
bool auto_altland(float taret_climb_rate,float target_climb_alt)
{
  return land_althold(taret_climb_rate,target_climb_alt);
}


uint8_t move_with_speed_target(float x_target,float y_target,float delta,SDK_Status *Status,uint16_t number)
{
  static float end_time=0;
  Vector2f vel_target;
  Testime dt;
  vel_target.x=x_target;
  vel_target.y=y_target;
  Test_Period(&dt);
	
  ncq_control_althold();//高度控制依然进行
  
	if(Status->Status[number].Start_Flag==1
     &&Status->Status[number].Execute_Flag==1
       &&Status->Status[number].End_flag==1)
  {
      OpticalFlow_Control_Pure(0);//完成之后进行光流悬停
      return 1;
  }
  
	else
  {
    if(Status->Status[number].Start_Flag==0) 
    {
      end_time=dt.Now_Time+delta;//单位ms 
      Status->Status[number].Start_Flag=1;
    }
		
    if(dt.Now_Time>end_time)
    {
      Status->Status[number].Execute_Flag=1;
      Status->Status[number].End_flag=1;
			
      OpticalFlow_Control_Pure(1);//完成之后进行光流悬停
      
			OpticalFlow_Pos_Ctrl_Expect.x=0;
      OpticalFlow_Pos_Ctrl_Expect.y=0;
			
      end_time=0;
      
			Status->Transition_Time[number]=400;//过渡时间？400*5ms=2s
      
			return 1;//返回完成
    }
    else
    { 
      OpticalFlow_Pos_Ctrl_Expect.x=0;
      OpticalFlow_Pos_Ctrl_Expect.y=0;
			
      Status->Status[number].Execute_Flag=1;
			
			OpticalFlow_Vel_Control(vel_target); //设置期望速度
			
      return 0;
    }
  }
}


uint8_t move_with_xy_target(float pos_x_target,float pos_y_target,SDK_Status *Status,uint16_t number)
{
  ncq_control_althold();//高度控制依然进行
	
  if(Status->Status[number].Start_Flag==0) 
  {
    OpticalFlow_Pos_Ctrl_Expect.x=OpticalFlow_SINS.Position[_PITCH]+pos_x_target;
    OpticalFlow_Pos_Ctrl_Expect.y=OpticalFlow_SINS.Position[_ROLL]+pos_y_target;
    Status->Status[number].Start_Flag=1;
  }
  
  if(Status->Status[number].Start_Flag==1
     &&Status->Status[number].Execute_Flag==1
       &&Status->Status[number].End_flag==1)
  {
     OpticalFlow_Control_Pure(0);//完成之后进行光流悬停
     return 1;
  }
  else
  {    
		//定位精度
    if(pythagorous2(OpticalFlow_Pos_Ctrl_Expect.x-OpticalFlow_SINS.Position[_PITCH],
                    OpticalFlow_Pos_Ctrl_Expect.y-OpticalFlow_SINS.Position[_ROLL])<=20.0f)
    {
      Status->Status[number].Execute_Flag=1;
      Status->Status[number].End_flag=1;

      OpticalFlow_Pos_Ctrl_Expect.x=0;
      OpticalFlow_Pos_Ctrl_Expect.y=0;
			
      Status->Transition_Time[number]=200;//200*5ms=1s
      
			OpticalFlow_Control_Pure(1);//完成之后 进行光流悬停
      
			OpticalFlow_Pos_Ctrl_Expect.x=0;
      OpticalFlow_Pos_Ctrl_Expect.y=0;
			
      return 1;
    }
    else
    {
      Status->Status[number].Execute_Flag=1; 
			
			//光流速度位置双环控制
      OpticalFlow_Pos_Control();
      OpticalFlow_Vel_Control(OpticalFlow_Pos_Ctrl_Output);
      return 0;
    }
  }
}


//此函数专用于定点巡航模式下的水平移动
//特点：有一个标志位数组 用于判断条件依次执行各个任务 从而完成巡航任务
//			没有Transition_Time			
uint8_t move_with_target(float pos_x_target,float pos_y_target,Duty_Status *Status,uint8_t *Start_flag)
{
  ncq_control_althold();//高度控制依然进行
	
  if(*Start_flag==1)
    *Start_flag=0;
  
  if(Status->Start_Flag==0) 
  {
    OpticalFlow_Pos_Ctrl_Expect.x=OpticalFlow_SINS.Position[_PITCH]+pos_x_target;
    OpticalFlow_Pos_Ctrl_Expect.y=OpticalFlow_SINS.Position[_ROLL]+pos_y_target;
		
    Status->Start_Flag=1;
  }
  
  if(Status->Start_Flag==1
     &&Status->Execute_Flag==1
       &&Status->End_flag==1)
  {
     OpticalFlow_Control_Pure(0);//完成之后进行光流悬停
     return 1;
  }
  else
  {    
		//定位精度
    if(pythagorous2(OpticalFlow_Pos_Ctrl_Expect.x-OpticalFlow_SINS.Position[_PITCH],
                OpticalFlow_Pos_Ctrl_Expect.y-OpticalFlow_SINS.Position[_ROLL])<=8.0f)
    {
      Status->Execute_Flag=1;
      Status->End_flag=1;

      OpticalFlow_Control_Pure(1);//完成之后进行光流悬停
			
      OpticalFlow_Pos_Ctrl_Expect.x=0;
      OpticalFlow_Pos_Ctrl_Expect.y=0;
			
      return 1;
    }
    else
    { 
      Status->Execute_Flag=1; 

			//光流速度位置双环控制
      OpticalFlow_Pos_Control();
			OpticalFlow_Vel_Control(OpticalFlow_Pos_Ctrl_Output);
      return 0;
    }
  }
}


uint8_t move_with_z_target(float z_target,float z_vel,float delta,SDK_Status *Status,uint16_t number)
{
  static float target_rate=0;
  static float target_alt=0;
  static uint8_t end_flag=0;
  static float end_time=0;
  Testime dt;
  Test_Period(&dt);
	
  OpticalFlow_Control_Pure(0);//水平位置控制依然进行
  
  if(Status->Status[number].Start_Flag==1
     &&Status->Status[number].Execute_Flag==1
       &&Status->Status[number].End_flag==1)
  {
     ncq_control_althold();
     return 1;
  }
  else
  {
      if(Status->Status[number].Start_Flag==0) 
      {
				//速度模式(一次任务中只设置一次)
        if(z_target==0)
        {
          target_rate=z_vel;
          target_alt=0;
          end_flag=1;//设置结束标志位 以区分速度模式和位置模式的结束
        }
        else	//位置模式(一次任务中只设置一次)
        {
          target_rate=0;
          target_alt=NamelessQuad.Position[_YAW]+z_target;
          end_flag=2;//设置结束标志位
        }
				
        end_time=dt.Now_Time+delta;//单位ms 
        Status->Status[number].Start_Flag=1;
				
				Unwanted_Lock_Flag=0;//允许自动上锁
				
				OpticalFlow_Pos_Ctrl_Expect.x=0;
				OpticalFlow_Pos_Ctrl_Expect.y=0;
      }

			if(end_flag==1)	//速度模式结束
			{  
				auto_altland(target_rate,target_alt);
				if(dt.Now_Time>end_time)
				{ 
					end_flag=0;
					target_rate=0;
					target_alt=0;

					end_time=0;
					Status->Status[number].Execute_Flag=1;
					Status->Status[number].End_flag=1;
					Status->Transition_Time[number]=100;//0.5s
					
					OpticalFlow_Control_Pure(1);//完成之后进行光流悬停
					OpticalFlow_Pos_Ctrl_Expect.x=0;
					OpticalFlow_Pos_Ctrl_Expect.y=0;
					
					Total_Controller.High_Position_Control.Expect=0;
					return 1;//返回完成
				}
				Status->Status[number].Execute_Flag=1;
			}
    else if(end_flag==2)
    {
      auto_altland(target_rate,target_alt);    
      if(ABS(target_alt-NamelessQuad.Position[_YAW])<=10.0f)
      {
        end_flag=0;
        target_rate=0;
        target_alt=0;
        //z_base=0;
        end_time=0;
        Status->Status[number].Execute_Flag=1;
        Status->Status[number].End_flag=1;
        Status->Transition_Time[number]=200;
				
				OpticalFlow_Control_Pure(1);//完成之后，进行光流悬停
				OpticalFlow_Pos_Ctrl_Expect.x=0;
				OpticalFlow_Pos_Ctrl_Expect.y=0;
				
				Total_Controller.High_Position_Control.Expect=0;
        return 1;
      }
      Status->Status[number].Execute_Flag=1;
    } 
  }
  return 0;
}


extern uint8_t Auto_Relock_Flag_Set;
//第一步：无怠速解锁操作
uint8_t Duty1(SDK_Status *Status,uint16_t number)
{
	Controler_State=Unlock_Controler;
	
	if(Controler_High_Mode==2)//如果是在定高模式下解锁 
	{  
		Unwanted_Lock_Flag=1;   //屏蔽自动上锁
		
#if (YAW_Pos_Control_Accel_Disable==1)	//三环定高模式
			Total_Controller.High_Speed_Control.Integrate=-Total_Controller.High_Speed_Control.Integrate_Max;
#else	//双环定高模式
			Total_Controller.High_Acce_Control.Integrate=-Total_Controller.High_Acce_Control.Integrate_Max;
#endif
	}
	else Unwanted_Lock_Flag=0;	//允许自动上锁
	
	Lock_Makesure_Cnt=0;
	Unlock_Makesure_Cnt=0;
	Forced_Lock_Makesure_Cnt=0;
	
	//控制LED
	Bling_Set(&Light_1,5000,2000,0.9,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
	Bling_Set(&Light_2,5000,2000,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
	Bling_Set(&Light_3,5000,2000,0.1,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
	Bling_Mode=0;
	
	Page_Number=0;//OLED恢复首页
	
	//清除传感器校准标志位
	Reset_Mag_Calibartion(1);
	Reset_Accel_Calibartion(1);
	Reset_RC_Calibartion(1);
	
	Auto_ReLock_Cnt=200*6;//持续6S
	Auto_Relock_Flag_Set=0;//关闭自动上锁
	
	Take_Off_Reset();//清积分
		
	Status->Status[number].Start_Flag=1;
	Status->Status[number].End_flag=1;
	Status->Status[number].Execute_Flag=1;
	
	Status->Transition_Time[number]=200;//1s
	
	return 1;
}


extern Point SDK_Real_Point;
uint8_t Mine_SDK_OpenMV_Flag=0;
uint8_t Finish_Point_Flag=0;
uint16_t Judge_Time=0;
uint8_t Mine_SDK_OpenMV_Duty(SDK_Status *Status,uint16_t number)
{
	//清空点模式、线模式数据 防止干扰自定义模式
//	SDK_Line_DT_Reset();
//	SDK_Point_DT_Reset();

  ncq_control_althold();//高度控制依然进行
	
	Judge_Time++;//计时用来判断起点终点
	if(Judge_Time>=2000)	//10s之后
	{
		Finish_Point_Flag = 1;//认为确信的真实点是终点
		Judge_Time = 2000;
	}
	else Finish_Point_Flag =0;

	if(Status->Status[number].Start_Flag==0) 
  {
		Mine_SDK_OpenMV_Flag=1;	//切换光流控制模式
    Status->Status[number].Start_Flag=1;	//标志任务开始
  }
  
  if(Status->Status[number].Start_Flag==1
     &&Status->Status[number].Execute_Flag==1
       &&Status->Status[number].End_flag==1)
  {
		Mine_SDK_OpenMV_Flag=0;//清除标志位 恢复正常光流控制
    OpticalFlow_Control_Pure(0);//完成之后进行光流悬停
    return 1;
  }
	else	//未结束
  {    
		//定位精度
//    if(pythagorous2(OpticalFlow_Pos_Ctrl_Expect.x-OpticalFlow_SINS.Position[_PITCH],
//                    OpticalFlow_Pos_Ctrl_Expect.y-OpticalFlow_SINS.Position[_ROLL])<=10.0f)
		if(pythagorous2(SDK_Real_Point.x-80,SDK_Real_Point.y-60)<=30.0f)
    {
      Status->Status[number].Execute_Flag=1;
      Status->Status[number].End_flag=1;	//标志结束

      OpticalFlow_Pos_Ctrl_Expect.x=0;
      OpticalFlow_Pos_Ctrl_Expect.y=0;
			
      Status->Transition_Time[number]=100;//0.5s
      
			OpticalFlow_Control_Pure(1);	//完成之后进行光流悬停
      
			OpticalFlow_Pos_Ctrl_Expect.x=0;
      OpticalFlow_Pos_Ctrl_Expect.y=0;
			
      return 1;
    }
    else
    {
      Status->Status[number].Execute_Flag=1; 
			
			//光流速度位置双环控制
      OpticalFlow_Pos_Control();
      OpticalFlow_Vel_Control(OpticalFlow_Pos_Ctrl_Output);
      return 0;
    }
  }
}


//#define NCQ_SDK_DUTY1 move_with_speed_target(10,0,2000 ,&SDK_Duty_Status,1-1)//左
//#define NCQ_SDK_DUTY2 move_with_speed_target(0,10,2000 ,&SDK_Duty_Status,2-1)//前
//#define NCQ_SDK_DUTY3 move_with_speed_target(-10,0,2000,&SDK_Duty_Status,3-1)//右
//#define NCQ_SDK_DUTY4 move_with_speed_target(0,-10,2000,&SDK_Duty_Status,4-1)//后


#define NCQ_SDK_DUTY_MAX   4
#define NCQ_SDK_DUTY1 Duty1(&SDK_Duty_Status,1-1)
#define NCQ_SDK_DUTY2 move_with_z_target(50,0,0,&SDK_Duty_Status,2-1)
#define NCQ_SDK_DUTY3 Mine_SDK_OpenMV_Duty(&SDK_Duty_Status,3-1)
#define NCQ_SDK_DUTY4 move_with_z_target(-80,0,0,&SDK_Duty_Status,4-1)

//#define NCQ_SDK_DUTY3 move_with_xy_target(0,200,&SDK_Duty_Status,3-1)
//#define NCQ_SDK_DUTY4 move_with_z_target(-80,0,0,&SDK_Duty_Status,4-1)


SDK_Status SDK_Duty_Status;
uint16_t SDK_Duty_Cnt=0;
uint16_t SDK_Transition_Time=0;
void NCQ_SDK_Run(void)
{
  if(SDK_Duty_Status.Transition_Time[SDK_Duty_Cnt]>=1) 
    SDK_Duty_Status.Transition_Time[SDK_Duty_Cnt]--;//过渡时间倒计时

  if(SDK_Duty_Status.Status[SDK_Duty_Cnt].Start_Flag==1
     &&SDK_Duty_Status.Status[SDK_Duty_Cnt].Execute_Flag==1
       &&SDK_Duty_Status.Status[SDK_Duty_Cnt].End_flag==1
         &&SDK_Duty_Status.Transition_Time[SDK_Duty_Cnt]==0)
    SDK_Duty_Cnt++;	//进行下一个任务
    
	//避免任务越界
  if(SDK_Duty_Cnt>=NCQ_SDK_DUTY_MAX) SDK_Duty_Cnt=NCQ_SDK_DUTY_MAX;
  
  if(SDK_Duty_Cnt==0)        NCQ_SDK_DUTY1;
  else if(SDK_Duty_Cnt==1)   NCQ_SDK_DUTY2;
  else if(SDK_Duty_Cnt==2)   NCQ_SDK_DUTY3;
  else if(SDK_Duty_Cnt==3)   NCQ_SDK_DUTY4;
  //else if(SDK_Duty_Cnt==4)   NCQ_SDK_DUTY5;
  //else if(SDK_Duty_Cnt==5)   NCQ_SDK_DUTY6;
  //else if(SDK_Duty_Cnt==6)   NCQ_SDK_DUTY7;
  else
  {
    ncq_control_althold();//高度控制
    OpticalFlow_Control(0);//位置控制
  }
}


void NCQ_SDK_Reset(void)
{
  uint16_t i=0;
  for(i=0;i<SDK_Duty_Max;i++)
  {
    SDK_Duty_Status.Status[i].Start_Flag=0;
    SDK_Duty_Status.Status[i].Execute_Flag=0;
    SDK_Duty_Status.Status[i].End_flag=0;
  }
  SDK_Duty_Cnt=0;
}


#define Circle_Piont_Max  24
#define Circle_R  50
static Duty_Status circle_status[Circle_Piont_Max+1];
static uint8_t Circle_Start_Flag=1;
float circle_pos_x_delta[Circle_Piont_Max+1]={0},circle_pos_y_delta[Circle_Piont_Max+1]={0};
static Vector2f circle_pos_base; 
uint8_t NCQ_SDK_Circle(void)
{
  static uint16_t circle_cnt=0;
  static float circle_angle=0;
  uint16_t i=0;
  
  if(Circle_Start_Flag==1)
  {
    circle_pos_base.x=OpticalFlow_SINS.Position[_PITCH];
    circle_pos_base.y=OpticalFlow_SINS.Position[_ROLL];
    for(i=0;i<Circle_Piont_Max+1;i++)
    {
      circle_angle=15*i;
      circle_pos_x_delta[i]=circle_pos_base.x+(1-cos(circle_angle*DEG2RAD))*Circle_R;
      circle_pos_y_delta[i]=circle_pos_base.y+sin(circle_angle*DEG2RAD)*Circle_R;
    }
  }
  else if(Circle_Start_Flag==2)
  {
    ncq_control_althold();
    OpticalFlow_Control(0);//结束后的水平位置控制
    return 1;
  }
  
  if(move_with_target(circle_pos_x_delta[circle_cnt],circle_pos_y_delta[circle_cnt],&circle_status[circle_cnt],&Circle_Start_Flag))
  {
    circle_cnt++;
    if(circle_cnt==Circle_Piont_Max+1)
    {
      circle_cnt=0;
      for(uint16_t i=0;i<Circle_Piont_Max+1;i++)
      {
        circle_status[i].End_flag=0;
        circle_status[i].Execute_Flag=0;
        circle_status[i].Start_Flag=0;
        //circle_pos_x_delta[i]=0;
        //circle_pos_y_delta[i]=0;
        circle_angle=0;
      }
      Circle_Start_Flag=2;
      return 1;
    }
  }
  return 0;
}


unsigned char sdk_data_to_send[50];
void SDK_DT_Send_Data(unsigned char *dataToSend , unsigned char length)
{
  USART3_Send(sdk_data_to_send,length);
}


void SDK_DT_Send_Check(unsigned char mode)
{
  sdk_data_to_send[0]=0xAA;
  sdk_data_to_send[1]=0xAF;
  sdk_data_to_send[2]=0xFC;
  sdk_data_to_send[3]=2;
  sdk_data_to_send[4]=mode;
  sdk_data_to_send[5]=0
    ;
  u8 sum = 0;
  for(u8 i=0;i<6;i++) sum += sdk_data_to_send[i];
  sdk_data_to_send[6]=sum;
  SDK_DT_Send_Data(sdk_data_to_send, 7);
}

uint8_t SDK_Now_Mode=0x00;
uint8_t SDK_Mode_Set=0x02;
#define SDK_TARGET_X_OFFSET  0
#define SDK_TARGET_Y_OFFSET  0//-12
Line  SDK_Line;
Point SDK_Point;
uint8_t SDK_Recieve_Flag=0;
Vector2f SDK_Target,SDK_Target_Offset;
float SDK_Target_Yaw_Gyro=0;
#define  Pixel_Size    0.0048
#define  Focal_Length  0.42


void SDK_Line_DT_Reset()
{
  SDK_Line.data=0;  
  SDK_Line.x=0;
  SDK_Line.y=0;
  SDK_Line.data=0;
  SDK_Line.line_angle=0;
  SDK_Line.up_ok=0;
  SDK_Line.down_ok=0;
  SDK_Line.left_ok=0;
  SDK_Line.right_ok=0;
  SDK_Line.flag=0;
  SDK_Line.line_ctrl_enable=0;
}

void SDK_Point_DT_Reset()
{
  SDK_Point.x=0;
  SDK_Point.y=0;
  SDK_Point.Pixel=0;
  SDK_Point.flag=0;
}


void Openmv_Data_Receive_Anl(u8 *data_buf,u8 num)
{
  //u8 sum = 0;
  //for(u8 i=0;i<(num-1);i++)  sum += *(data_buf+i);
  //if(!(sum==*(data_buf+num-1)))	return;	                //不满足和校验条件	
	
  if(!(*(data_buf)==0xAA && *(data_buf+1)==0xAF))	return;	//不满足帧头条件	
	
  if(*(data_buf+2)==0XC0)//色块检测
    SDK_Recieve_Flag=1;
		
  else if(*(data_buf+2)==0XF3)//线检测
  {
    SDK_Now_Mode=0x02;
    SDK_Line.x=*(data_buf+4)<<8|*(data_buf+5);
    SDK_Line.line_angle=(*(data_buf+6)<<8)|*(data_buf+7);
    SDK_Line.flag=*(data_buf+8);   
		
    //line.flag从低位到高位分别对应镜头视野中上下左右四个区域，检测到有直线为1，否则为0
    SDK_Line.up_ok    = (uint8_t)((SDK_Line.flag & 0x01)>>0);
    SDK_Line.down_ok  = (uint8_t)((SDK_Line.flag & 0x02)>>1);
    SDK_Line.left_ok  = (uint8_t)((SDK_Line.flag & 0x04)>>2);
    SDK_Line.right_ok = (uint8_t)((SDK_Line.flag & 0x08)>>3);
    
    if(SDK_Line.up_ok==1||SDK_Line.down_ok==1)
    {
      SDK_Line.line_ctrl_enable=1;
      SDK_Target.x=(Pixel_Size*(40-SDK_Line.x)*NamelessQuad.Position[_YAW])/Focal_Length
        +NamelessQuad.Position[_YAW]*tan(Roll* DEG2RAD);
      if(SDK_Line.line_angle>0&&SDK_Line.line_angle<90)
        SDK_Target_Yaw_Gyro=-SDK_Line.line_angle;
      if(SDK_Line.line_angle<180&&SDK_Line.line_angle>90)
        SDK_Target_Yaw_Gyro=180-SDK_Line.line_angle;
      
      SDK_Recieve_Flag=1;
    }
    
    SDK_Point_DT_Reset();
  }
  else if(*(data_buf+2)==0XF2)//点检测
  {
    SDK_Now_Mode=0x01;
    SDK_Point.x=*(data_buf+4)<<8|*(data_buf+5);
    SDK_Point.y=*(data_buf+6)<<8|*(data_buf+7);
    SDK_Point.Pixel=*(data_buf+8)<<8|*(data_buf+9);//像素
    SDK_Point.flag=*(data_buf+10);
		
		if(SDK_Point.flag!=0)
		{
			if(SDK_Point.trust_Cnt<=20)	 SDK_Point.trust_Cnt++;
		}
		else SDK_Point.trust_Cnt/=2;
		
		if(SDK_Point.trust_Cnt>=10)   SDK_Point.trust_flag=1; 
		
		else SDK_Point.trust_flag=0;
		
    SDK_Recieve_Flag=1;	//
    
    SDK_Target_Offset.x=SDK_TARGET_X_OFFSET;
    SDK_Target_Offset.y=SDK_TARGET_Y_OFFSET;
    
    SDK_Target.x=(Pixel_Size*(40-SDK_Point.x)*NamelessQuad.Position[_YAW])/Focal_Length
      +NamelessQuad.Position[_YAW]*tan(Roll* DEG2RAD)-SDK_Target_Offset.x;
    SDK_Target.y=(Pixel_Size*(30-SDK_Point.y)*NamelessQuad.Position[_YAW])/Focal_Length
      +NamelessQuad.Position[_YAW]*tan(Pitch* DEG2RAD)-SDK_Target_Offset.y;  
		
    SDK_Line_DT_Reset(); 
  }
  else if(*(data_buf+2)==0XC3)//二维码
  {
    SDK_Recieve_Flag=1;
  }
  //串口数传SDK模式
  else if(*(data_buf+2)==0X01)//高度位置控制
  {
    if(*(data_buf+4)==0X01)//上升  
    {
      Total_Controller.High_Position_Control.Expect+=*(data_buf+5);
      Unwanted_Lock_Flag=0;
    }
    else if(*(data_buf+4)==0X02)//下降
    {
      Total_Controller.High_Position_Control.Expect-=*(data_buf+5);
    }
    else if(*(data_buf+4)==0X03)//地面起飞，上升到目标高度
    {
      Total_Controller.High_Position_Control.Expect+=*(data_buf+5);
      Unwanted_Lock_Flag=0;
      Reserve_Mode_Fast_Exchange_Flag=1;
    }
    SDK_Recieve_Flag=1;
  }
  else
  {
    SDK_Recieve_Flag=0;
  }
}


//
Point Maybe_SDK_Point1,Maybe_SDK_Point2;
Point SDK_Real_Point;
//真实点检测到标志位
uint8_t SDK_RealPoint_Flag=0;
//确信真实点标志位
uint8_t Trust_RealPoint_flag=0;
//OpenMV参与的SDK模式(自用)
void Openmv_Data_Receive_Mine(u8 *data_buf,u8 num)
{
	static uint8_t RealPoint_Cnt;//真实点检测计数
	
//	SDK_Recieve_Flag=0;
	
	//校验数据帧头
	if(!(*(data_buf)==0xAA && *(data_buf+1)==0xAA))	{SDK_Recieve_Flag=0; return;}
	
	//校验数据帧尾
	if(!(*(data_buf+8)==0x00 && *(data_buf+9)==0x00 && *(data_buf+10)==0x00 && *(data_buf+11)==0x00))	{SDK_Recieve_Flag=0; return;}	
	
	SDK_Recieve_Flag=1;	//标志SDK数据接收成功
	
	SDK_Now_Mode = 0x03;
	
	Maybe_SDK_Point1.x = *(data_buf+2);
	Maybe_SDK_Point1.y = *(data_buf+3);
	Maybe_SDK_Point2.y = *(data_buf+4);
	Maybe_SDK_Point2.y = *(data_buf+5);
	
	SDK_Real_Point.x = *(data_buf+6);
	SDK_Real_Point.y = *(data_buf+7);
	
	//真实点坐标越界 或 通过时间判断不可能到达终点附近 则判定未检测到真实点 
	//这种情况下按照直线上的虚拟点行进
	if(SDK_Real_Point.x>160||SDK_Real_Point.y>120||Finish_Point_Flag==0)
	{
		RealPoint_Cnt = 0;
		Trust_RealPoint_flag = 0;
	}
	else	//检测到真实点 开始计数 若确保能持续检测到 则忽略虚拟点 
	{
		RealPoint_Cnt++;
	}
	
	if(RealPoint_Cnt>=20)//连续20次检测到真实点 则认为不是噪声
	{
		Trust_RealPoint_flag = 1;//确信标志位
		RealPoint_Cnt = 0;//计时清零 
	}
		
	if(Trust_RealPoint_flag==1) //确信检测到真实点
	{
		//设定真实点为目标点 忽略虚拟点
		SDK_Target.x=(Pixel_Size*(80-SDK_Real_Point.x)*(-1)*NamelessQuad.Position[_YAW])/Focal_Length
      +NamelessQuad.Position[_YAW]*tan(Roll* DEG2RAD)-SDK_Target_Offset.x;
    SDK_Target.y=(Pixel_Size*(60-SDK_Real_Point.y)*NamelessQuad.Position[_YAW])/Focal_Length
      +NamelessQuad.Position[_YAW]*tan(Pitch* DEG2RAD)-SDK_Target_Offset.y;  
	}
	else	//未确信检测到真实点
	{
		//选择前方的虚拟点
		if(Maybe_SDK_Point1.y<=Maybe_SDK_Point2.y)//y坐标小的在前方
		{
			//设定前方的虚拟点为目标点(Point1)
			SDK_Target.x=(Pixel_Size*(80-Maybe_SDK_Point1.x)*(-1)*NamelessQuad.Position[_YAW])/Focal_Length
				+NamelessQuad.Position[_YAW]*tan(Roll* DEG2RAD)-SDK_Target_Offset.x;
			SDK_Target.y=(Pixel_Size*(60-Maybe_SDK_Point1.y)*NamelessQuad.Position[_YAW])/Focal_Length
				+NamelessQuad.Position[_YAW]*tan(Pitch* DEG2RAD)-SDK_Target_Offset.y;  
		}
		else
		{
			//设定前方的虚拟点为目标点(Point2)
			SDK_Target.x=(Pixel_Size*(80-Maybe_SDK_Point2.x)*(-1)*NamelessQuad.Position[_YAW])/Focal_Length
				+NamelessQuad.Position[_YAW]*tan(Roll* DEG2RAD)-SDK_Target_Offset.x;
			SDK_Target.y=(Pixel_Size*(60-Maybe_SDK_Point2.y)*NamelessQuad.Position[_YAW])/Focal_Length
				+NamelessQuad.Position[_YAW]*tan(Pitch* DEG2RAD)-SDK_Target_Offset.y;  
		}
	}
}


static u8 state_mine = 0;
u8 RxBuffer_mine[20];
//SDK数据接收OpenMV函数(自用)
void SDK_Data_Receive_Prepare_Mine(u8 data)
{
  static u8 _data_len = 6,_data_cnt = 0;
	
	//校验帧头
  if(state_mine==0&&data==0xAA)//帧头1
  {
    state_mine=1;
    RxBuffer_mine[0]=data;
  }
  else if(state_mine==1&&data==0xAA)//帧头2
  {
    state_mine=2;
    RxBuffer_mine[1]=data;
  }
	
	//接收有效数据
  else if(state_mine==2&&_data_len>0)
  {
    _data_len--;
    RxBuffer_mine[2+_data_cnt++]=data;
		
    if(_data_len==0) state_mine = 3;
  }
	
	//接收帧尾
  else if(state_mine==3&&data==0x00)//0x00
  {
    state_mine = 4;
    RxBuffer_mine[2+_data_cnt]=data;
  }
	else if(state_mine==4&&data==0x00)//0x00
	{
		state_mine = 5;
    RxBuffer_mine[2+_data_cnt+1]=data;
	}
	else if(state_mine==5&&data==0x00)//0x00
	{
		state_mine = 6;
    RxBuffer_mine[2+_data_cnt+2]=data;
	}
	else if(state_mine==6&&data==0x00)//0x00
	{
		state_mine = 7;
    RxBuffer_mine[2+_data_cnt+3]=data;
		Openmv_Data_Receive_Mine(RxBuffer_mine,_data_len+6);
	}

  else state_mine = 0;	
}


//自用SDK模式数据复位
void SDK_Mine_DT_Reset()
{
	SDK_Recieve_Flag = 0;
	
	Maybe_SDK_Point1.x = 0;
	Maybe_SDK_Point1.y = 0;
	Maybe_SDK_Point2.y = 0;
	Maybe_SDK_Point2.y = 0;
	
	SDK_Real_Point.x = 0;
  SDK_Real_Point.y = 0;
	
	Trust_RealPoint_flag = 0;
}


static u8 state = 0;
u8 RxBuffer[50];
//原始SDK接收数据函数
void SDK_Data_Receive_Prepare(u8 data)
{
  static u8 _data_len = 0,_data_cnt = 0;
  if(state==0&&data==0xAA)//帧头1
  {
    state=1;
    RxBuffer[0]=data;
  }
  else if(state==1&&data==0xAF)//帧头2
  {
    state=2;
    RxBuffer[1]=data;
  }
  else if(state==2&&data<0XFF)//功能字节
  {
    state=3;
    RxBuffer[2]=data;
  }
  else if(state==3&&data<50)//数据长度
  {
    state = 4;
    RxBuffer[3]=data;
    _data_len = data;
    _data_cnt = 0;
  }
  else if(state==4&&_data_len>0)//有多少数据长度就存多少个
  {
    _data_len--;
    RxBuffer[4+_data_cnt++]=data;
    if(_data_len==0) state = 5;
  }
  else if(state==5)//最后接收数据校验和
  {
    state = 0;
    RxBuffer[4+_data_cnt]=data;
    Openmv_Data_Receive_Anl(RxBuffer,_data_cnt+5);
  }
  else state = 0;
}


uint16_t SDK_Data_Offset=0;
//SDK读取串口数据
void SDK_Data_Prase(void)
{
  static uint16_t sdk_prase_cnt=0;
  uint16_t i=0;
  sdk_prase_cnt++;
  if(sdk_prase_cnt>=2)//5*2=10ms
  {
    if(COM3_Rx_Buf.Tail<12)//0-11数据位正在传输
    {
      SDK_Data_Offset=12;
    }
    else//12-23数据位正在传输
    {
      SDK_Data_Offset=0;
    }
    for(i=0;i<12;i++)
    {
			//默认SDK模式串口数据接收
      //SDK_Data_Receive_Prepare(COM3_Rx_Buf.Ring_Buff[SDK_Data_Offset+i]);
			//自用SDK模式串口数据接收
			SDK_Data_Receive_Prepare_Mine(COM3_Rx_Buf.Ring_Buff[SDK_Data_Offset+i]);
    }
    sdk_prase_cnt=0;
  }
}


//SDK模式初始化
void SDK_Init(void)
{
  float sdk_mode_default=0;
	
  SDK_Line_DT_Reset();//复位SDK线检测数据
  SDK_Point_DT_Reset();//复位SDK点检测数据
	SDK_Mine_DT_Reset();//复位自用SDK模式数据
	
  ReadFlashParameterOne(SDK_MODE_DEFAULT,&sdk_mode_default);
	
  if(isnan(sdk_mode_default)==0)
  {
		//初始化opemmv工作模式 默认以上次工作状态配置
    SDK_Mode_Set=(uint8_t)(sdk_mode_default);
    SDK_DT_Send_Check(SDK_Mode_Set);
  } 
}


