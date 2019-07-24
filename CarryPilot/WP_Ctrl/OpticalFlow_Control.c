#include "Headfile.h"
#include "OpticalFlow_Control.h"


//�������Ʊ�������
extern Vector2f OpticalFlow_Position;
extern Vector2f OpticalFlow_Speed;
extern Vector2f OpticalFlow_Speed_Err;
extern Vector2f OpticalFlow_Position_Err;

Vector2f OpticalFlow_Pos_Ctrl_Expect={0};
Vector2f OpticalFlow_Pos_Ctrl_Err={0};
Vector2f OpticalFlow_Pos_Ctrl_Integrate={0};
Vector2f OpticalFlow_Pos_Ctrl_Output={0};

Vector2f OpticalFlow_Ctrl_Expect={0};
Vector2f OpticalFlow_Ctrl_Err={0};
Vector2f OpticalFlow_Ctrl_Integrate={0};
Vector2f OpticalFlow_Ctrl_Output={0};


//�����ߵ����ݸ�λ
void OpticalFlow_SINS_Reset(void)
{
  OpticalFlow_SINS.Position[_PITCH]=0;
  OpticalFlow_SINS.Speed[_PITCH]=0;
  OpticalFlow_SINS.Position[_ROLL]=0;
  OpticalFlow_SINS.Speed[_ROLL]=0;
  OpticalFlow_Position.x=0;
  OpticalFlow_Position.y=0;
}


//�����������ݸ�λ
void OpticalFlow_Ctrl_Reset(void)
{
  OpticalFlow_Ctrl_Integrate.x=0.0f;
  OpticalFlow_Ctrl_Integrate.y=0.0f;
  OpticalFlow_Pos_Ctrl_Integrate.x=0.0f;
  OpticalFlow_Pos_Ctrl_Integrate.y=0.0f;
  OpticalFlow_Pos_Ctrl_Expect.x=0;
  OpticalFlow_Pos_Ctrl_Expect.y=0;
}


//SDKλ�ÿ������ݸ�λ
void SDK_Pos_Ctrl_Reset(void)
{ 
	Total_Controller.SDK_Pitch_Position_Control.Integrate=0;
	Total_Controller.SDK_Pitch_Position_Control.Last_Err=0;
	Total_Controller.SDK_Pitch_Position_Control.Err=0;
	Total_Controller.SDK_Pitch_Position_Control.Last_Err_LPF=0;		

	Total_Controller.SDK_Roll_Position_Control.Integrate=0;
	Total_Controller.SDK_Roll_Position_Control.Last_Err=0;
	Total_Controller.SDK_Roll_Position_Control.Err=0;
	Total_Controller.SDK_Roll_Position_Control.Last_Err_LPF=0;
}


//��
float OpticalFlow_Expect_Speed_Mapping(float input,uint16_t input_max,float output_max)
{
  float output_speed=0;
  
	float temp_scale=(float)(input/input_max);
  temp_scale=constrain_float(temp_scale,-1.0f, 1.0f);
	
	//ƽ����
  if(temp_scale>=0) output_speed=(float)(output_max*temp_scale*temp_scale);
  else output_speed=(float)(-output_max*temp_scale*temp_scale); 
	
  return output_speed;
}


//��������ˮƽĿ��λ��
void OpticalFlow_Set_Target_Point(Vector2f target)
{
  OpticalFlow_Pos_Ctrl_Expect.x=target.x;
  OpticalFlow_Pos_Ctrl_Expect.y=target.y;
}


//��������ˮƽĿ���ٶ�
void OpticalFlow_Set_Target_Vel(Vector2f target)
{
  OpticalFlow_Ctrl_Expect.x=target.x;
  OpticalFlow_Ctrl_Expect.y=target.y;
}


extern uint8_t Mine_SDK_OpenMV_Flag;
//�������ƺ���
void OpticalFlow_Control(uint8_t force_brake_flag)
{
  static uint16_t SDK_Ctrl_Cnt=0;
	
	//7ͨ��������λ ��SDKģʽ ������������
  if(SDK_Ctrl_Mode==0)
  {
    if(Roll_Control==0
			&&Pitch_Control==0)//��ˮƽң����
    {
      //����λ�ÿ���������Ϊ0
      if(OpticalFlow_Pos_Ctrl_Expect.x==0
				&&OpticalFlow_Pos_Ctrl_Expect.y==0)
      {
				//��˻��к� ���ݵ�ǰ�ٶ� ����ж��ܷ������ͣ
				//������ͣ����
        if(force_brake_flag==1	//ǿ��ɲ��
					||(rMat[2][2]>=0.95f	//��Ǻ�С��rMat[2][2] = Cos_Pitch*Cos_Roll
						&&pythagorous2(OpticalFlow_SINS.Speed[_PITCH],OpticalFlow_SINS.Speed[_ROLL])<=40))	//��ǰ�ٶȽ�С
        {
          OpticalFlow_Pos_Ctrl_Expect.x=OpticalFlow_SINS.Position[_PITCH];
          OpticalFlow_Pos_Ctrl_Expect.y=OpticalFlow_SINS.Position[_ROLL];
        }
				
        else  //��˻���δ������ͣ���� ��ֻ�����ٶȿ��� ���ã�
        {
          OpticalFlow_Pos_Ctrl_Output.x=0;
          OpticalFlow_Pos_Ctrl_Output.y=0;
        }
      }
      else  OpticalFlow_Pos_Control();
      
      //��ֻ��Ҫ�ٶȿ���ʱ��������ע��(���޵���ʱ��)
      //OpticalFlow_Pos_Ctrl_Output.x=0;
      //OpticalFlow_Pos_Ctrl_Output.y=0;
			
      OpticalFlow_Vel_Control(OpticalFlow_Pos_Ctrl_Output);//�ٶȿ���
    }
    else
    { 
			//����ģ�͵ļ��ٶ�-��̬��ӳ��
			//���ֱ�Ӹ���̬ ������������20������
      OpticalFlow_Pos_Ctrl_Output.x=OpticalFlow_Expect_Speed_Mapping(-Target_Angle[1],Pit_Rol_Max,OpticalFlow_Speed_Control_Max);
      OpticalFlow_Pos_Ctrl_Output.y=OpticalFlow_Expect_Speed_Mapping(-Target_Angle[0],Pit_Rol_Max,OpticalFlow_Speed_Control_Max); 
			
      if(OpticalFlow_Speed_Control_Mode==1) //�궨��ֱ������
        OpticalFlow_Vel_Control(OpticalFlow_Pos_Ctrl_Output);//�ٶȿ���
      else	//���ٶȿ���ģʽ(�Ƕȿ���ģʽ)
      {
        Total_Controller.Pitch_Angle_Control.Expect=Target_Angle[0];
        Total_Controller.Roll_Angle_Control.Expect=Target_Angle[1];      
      }
			
      OpticalFlow_Pos_Ctrl_Expect.x=0;
      OpticalFlow_Pos_Ctrl_Expect.y=0;
    }
		
    PID_LPF_Reset(&Total_Controller.SDK_Roll_Position_Control,SDK_Roll_Position_Controler);
    PID_LPF_Reset(&Total_Controller.SDK_Pitch_Position_Control,SDK_Pitch_Position_Controler);
  }
  else	//SDKģʽ
  {
    if(Roll_Control==0
			&&Pitch_Control==0)//��ˮƽң����
    {
      if(SDK_Line.line_ctrl_enable==1)//�߼��ʹ��
      {
        SDK_Ctrl_Cnt++;
        if(SDK_Ctrl_Cnt>=4)//20ms
        {
          Total_Controller.SDK_Roll_Position_Control.Expect=0;
          Total_Controller.SDK_Roll_Position_Control.FeedBack=SDK_Target.x;
					
          PID_Control_SDK_Err_LPF(&Total_Controller.SDK_Roll_Position_Control,1);
					
					//��ģʽˮƽֻ��Ҫ����roll(��x����)
          accel_target.x=constrain_float(Total_Controller.SDK_Roll_Position_Control.Control_OutPut,
																					-Total_Controller.Optical_Speed_Control.Control_OutPut_Limit,
																						Total_Controller.Optical_Speed_Control.Control_OutPut_Limit);  
					
          Total_Controller.Roll_Angle_Control.Expect=constrain_float(fast_atan(accel_target.x/(GRAVITY_MSS*100))*RAD2DEG,-30,30); 
					
          SDK_Ctrl_Cnt=0;
        }
				
				if(ABS(SDK_Target_Yaw_Gyro)<=10) 			OpticalFlow_Y_Vel_Control(5);
        else if(ABS(SDK_Target_Yaw_Gyro)<=20) OpticalFlow_Y_Vel_Control(3);
        else if(ABS(SDK_Target_Yaw_Gyro)<=50) OpticalFlow_Y_Vel_Control(2);
        else OpticalFlow_Y_Vel_Control(1);
				
        OpticalFlow_Pos_Ctrl_Expect.x=0;
        OpticalFlow_Pos_Ctrl_Expect.y=0;
				
        force_brake_flag=1;	//ǿ��ɲ����
      }
      else if(SDK_Point.flag==1)//���ٵ�ģʽ
      {
        SDK_Ctrl_Cnt++;
        if(SDK_Ctrl_Cnt>=4)//20ms
        {
          Total_Controller.SDK_Roll_Position_Control.Expect=0;
          Total_Controller.SDK_Roll_Position_Control.FeedBack=SDK_Target.x;
					
          PID_Control_SDK_Err_LPF(&Total_Controller.SDK_Roll_Position_Control,SDK_Point.trust_flag);
          
          Total_Controller.SDK_Pitch_Position_Control.Expect=0;
          Total_Controller.SDK_Pitch_Position_Control.FeedBack=SDK_Target.y;
          PID_Control_SDK_Err_LPF(&Total_Controller.SDK_Pitch_Position_Control,SDK_Point.trust_flag);
          
					//��ģʽˮƽ��Ҫ����roll��pitch(��x�����y����)
          accel_target.x=constrain_float(Total_Controller.SDK_Roll_Position_Control.Control_OutPut,
																					-Total_Controller.Optical_Speed_Control.Control_OutPut_Limit,
																						Total_Controller.Optical_Speed_Control.Control_OutPut_Limit);
					
          Total_Controller.Roll_Angle_Control.Expect=constrain_float(fast_atan(accel_target.x/(GRAVITY_MSS*100))*RAD2DEG,-30,30);//roll
          
          
          accel_target.y=constrain_float(Total_Controller.SDK_Pitch_Position_Control.Control_OutPut,
																					-Total_Controller.Optical_Speed_Control.Control_OutPut_Limit,
																						Total_Controller.Optical_Speed_Control.Control_OutPut_Limit);
					
          Total_Controller.Pitch_Angle_Control.Expect=constrain_float(fast_atan(accel_target.y*Cos_Roll/(GRAVITY_MSS*100))*RAD2DEG,-30,30);//pitch
          
          SDK_Ctrl_Cnt=0;
        }
				
        OpticalFlow_Pos_Ctrl_Expect.x=0;
        OpticalFlow_Pos_Ctrl_Expect.y=0;
        
				force_brake_flag=1;
      }
      else if(Mine_SDK_OpenMV_Flag==1)//SDK�Զ�����ģʽ
			{
        SDK_Ctrl_Cnt++;
        if(SDK_Ctrl_Cnt>=4)//20ms��������
        {
					//����Mine_SDK_OpenMVģʽ����õ���SDK_Target.xy���п���
          Total_Controller.SDK_Roll_Position_Control.Expect=0;
          Total_Controller.SDK_Roll_Position_Control.FeedBack=SDK_Target.x;
					
          PID_Control_SDK_Err_LPF(&Total_Controller.SDK_Roll_Position_Control,SDK_Point.trust_flag);
          
          Total_Controller.SDK_Pitch_Position_Control.Expect=0;
          Total_Controller.SDK_Pitch_Position_Control.FeedBack=SDK_Target.y;
          PID_Control_SDK_Err_LPF(&Total_Controller.SDK_Pitch_Position_Control,SDK_Point.trust_flag);
          
					//��ģʽˮƽ��Ҫ����roll��pitch(��x�����y����)
          accel_target.x=constrain_float(Total_Controller.SDK_Roll_Position_Control.Control_OutPut,
																					-Total_Controller.Optical_Speed_Control.Control_OutPut_Limit,
																						Total_Controller.Optical_Speed_Control.Control_OutPut_Limit);
					
          Total_Controller.Roll_Angle_Control.Expect=constrain_float(fast_atan(accel_target.x/(GRAVITY_MSS*100))*RAD2DEG,-30,30);//roll
          
          
          accel_target.y=constrain_float(Total_Controller.SDK_Pitch_Position_Control.Control_OutPut,
																					-Total_Controller.Optical_Speed_Control.Control_OutPut_Limit,
																						Total_Controller.Optical_Speed_Control.Control_OutPut_Limit);
					
          Total_Controller.Pitch_Angle_Control.Expect=constrain_float(fast_atan(accel_target.y*Cos_Roll/(GRAVITY_MSS*100))*RAD2DEG,-30,30);//pitch
          
          SDK_Ctrl_Cnt=0;
        }
				
				//���ֶ���
        OpticalFlow_Pos_Ctrl_Expect.x=0;
        OpticalFlow_Pos_Ctrl_Expect.y=0;
        
				force_brake_flag=1;
			}
			else	//SDKģʽ�� δ����������
      {
        //����λ�ÿ���
        if(OpticalFlow_Pos_Ctrl_Expect.x==0
					&&OpticalFlow_Pos_Ctrl_Expect.y==0)
        {
					//��˻��к� ���ݵ�ǰ�ٶ� ����ж��ܷ������ͣ
          if(force_brake_flag==1
						||(rMat[2][2]>=0.95f
							&&pythagorous2(OpticalFlow_SINS.Speed[_PITCH],OpticalFlow_SINS.Speed[_ROLL])<=40))
          {
            OpticalFlow_Pos_Ctrl_Expect.x=OpticalFlow_SINS.Position[_PITCH];
            OpticalFlow_Pos_Ctrl_Expect.y=OpticalFlow_SINS.Position[_ROLL];
          }
          else  //��˻���δ������ͣ����ʱ ֻ�����ٶȿ��� 
          {
            OpticalFlow_Pos_Ctrl_Output.x=0;
            OpticalFlow_Pos_Ctrl_Output.y=0;
          }
        }
        else  OpticalFlow_Pos_Control();
				
        OpticalFlow_Vel_Control(OpticalFlow_Pos_Ctrl_Output);//�ٶȿ���       
        
				PID_LPF_Reset(&Total_Controller.SDK_Roll_Position_Control,SDK_Roll_Position_Controler);
        PID_LPF_Reset(&Total_Controller.SDK_Pitch_Position_Control,SDK_Pitch_Position_Controler);
				
				SDK_Pos_Ctrl_Reset();
			}
    }
    else	//SDKģʽ�´����ֶ���˲���ʱ ��������Դ��ң����
    {
      Total_Controller.Pitch_Angle_Control.Expect=Target_Angle[0];
      Total_Controller.Roll_Angle_Control.Expect=Target_Angle[1];
			
      OpticalFlow_Pos_Ctrl_Expect.x=0;
      OpticalFlow_Pos_Ctrl_Expect.y=0;
      
			force_brake_flag=1;
			
			PID_LPF_Reset(&Total_Controller.SDK_Roll_Position_Control,SDK_Roll_Position_Controler);
			PID_LPF_Reset(&Total_Controller.SDK_Pitch_Position_Control,SDK_Pitch_Position_Controler);
			
			SDK_Pos_Ctrl_Reset();
    }
    SDK_Recieve_Flag=0;
		
    OpticalFlow_Pos_Ctrl_Expect.x=0;
    OpticalFlow_Pos_Ctrl_Expect.y=0;
  }
}


//��
void OpticalFlow_Control_Pure(uint8_t force_brake_flag)
{
	if(Roll_Control==0
		&&Pitch_Control==0)//��ˮƽң����
	{ 
		//����λ�ÿ���
		if((OpticalFlow_Pos_Ctrl_Expect.x==0&&OpticalFlow_Pos_Ctrl_Expect.y==0)||force_brake_flag==1)
		{
			OpticalFlow_Pos_Ctrl_Expect.x=OpticalFlow_SINS.Position[_PITCH];
			OpticalFlow_Pos_Ctrl_Expect.y=OpticalFlow_SINS.Position[_ROLL];
		}
		else  OpticalFlow_Pos_Control();
	 
		OpticalFlow_Vel_Control(OpticalFlow_Pos_Ctrl_Output);//�ٶȿ���
	}
	else
	{
		OpticalFlow_Pos_Ctrl_Output.x=OpticalFlow_Expect_Speed_Mapping(-Target_Angle[1],Pit_Rol_Max,OpticalFlow_Speed_Control_Max);
		OpticalFlow_Pos_Ctrl_Output.y=OpticalFlow_Expect_Speed_Mapping(-Target_Angle[0],Pit_Rol_Max,OpticalFlow_Speed_Control_Max); 
		if(OpticalFlow_Speed_Control_Mode==1)
			OpticalFlow_Vel_Control(OpticalFlow_Pos_Ctrl_Output);//�ٶ�����
		else
		{
			Total_Controller.Pitch_Angle_Control.Expect=Target_Angle[0];
			Total_Controller.Roll_Angle_Control.Expect=Target_Angle[1];  
		}
		OpticalFlow_Pos_Ctrl_Expect.x=0;
		OpticalFlow_Pos_Ctrl_Expect.y=0;
	}
	PID_LPF_Reset(&Total_Controller.SDK_Roll_Position_Control,SDK_Roll_Position_Controler);
	PID_LPF_Reset(&Total_Controller.SDK_Pitch_Position_Control,SDK_Pitch_Position_Controler);
}


void OpticalFlow_Pos_Control(void)
{
  static uint16_t OpticalFlow_Pos_Ctrl_Cnt=0;
	
  OpticalFlow_Pos_Ctrl_Cnt++;
  
	//50ms����һ���ٶ� ��������Ƶ�ʹ��� ϵͳ��Ӧ������
	if(OpticalFlow_Pos_Ctrl_Cnt>=10)
  {
    //����λ��ƫ��  
    OpticalFlow_Pos_Ctrl_Err.x=OpticalFlow_Pos_Ctrl_Expect.x-OpticalFlow_SINS.Position[_PITCH];
    OpticalFlow_Pos_Ctrl_Err.y=OpticalFlow_Pos_Ctrl_Expect.y-OpticalFlow_SINS.Position[_ROLL];
    
		//����λ�ÿ������
    OpticalFlow_Pos_Ctrl_Output.x=Total_Controller.Optical_Position_Control.Kp*OpticalFlow_Pos_Ctrl_Err.x;
    OpticalFlow_Pos_Ctrl_Output.y=Total_Controller.Optical_Position_Control.Kp*OpticalFlow_Pos_Ctrl_Err.y;
    
		OpticalFlow_Pos_Ctrl_Cnt=0;
  }
}


void OpticalFlow_Vel_Control(Vector2f target)
{
  static uint16_t OpticalFlow_Vel_Ctrl_Cnt=0;
	
	//�ٶ�����
  OpticalFlow_Ctrl_Expect.x= target.x;
  OpticalFlow_Ctrl_Expect.y= target.y;
  
	OpticalFlow_Vel_Ctrl_Cnt++;
	
	//20ms����һ���ٶ� ��������Ƶ�ʹ��� ϵͳ��Ӧ������
  if(OpticalFlow_Vel_Ctrl_Cnt>=4)
  {
    OpticalFlow_Ctrl_Err.x=constrain_float(OpticalFlow_Ctrl_Expect.x-OpticalFlow_SINS.Speed[_PITCH],-Total_Controller.Optical_Speed_Control.Err_Max,Total_Controller.Optical_Speed_Control.Err_Max);//30
    OpticalFlow_Ctrl_Err.y=constrain_float(OpticalFlow_Ctrl_Expect.y-OpticalFlow_SINS.Speed[_ROLL] ,-Total_Controller.Optical_Speed_Control.Err_Max,Total_Controller.Optical_Speed_Control.Err_Max);
    
    OpticalFlow_Ctrl_Integrate.x+=Total_Controller.Optical_Speed_Control.Ki*OpticalFlow_Ctrl_Err.x;
    OpticalFlow_Ctrl_Integrate.y+=Total_Controller.Optical_Speed_Control.Ki*OpticalFlow_Ctrl_Err.y;
    
    OpticalFlow_Ctrl_Integrate.x=constrain_float(OpticalFlow_Ctrl_Integrate.x,-Total_Controller.Optical_Speed_Control.Integrate_Max,Total_Controller.Optical_Speed_Control.Integrate_Max);
    OpticalFlow_Ctrl_Integrate.y=constrain_float(OpticalFlow_Ctrl_Integrate.y,-Total_Controller.Optical_Speed_Control.Integrate_Max,Total_Controller.Optical_Speed_Control.Integrate_Max);
    
    OpticalFlow_Ctrl_Output.x=OpticalFlow_Ctrl_Integrate.x+Total_Controller.Optical_Speed_Control.Kp*OpticalFlow_Ctrl_Err.x;
    OpticalFlow_Ctrl_Output.y=OpticalFlow_Ctrl_Integrate.y+Total_Controller.Optical_Speed_Control.Kp*OpticalFlow_Ctrl_Err.y;
    
    accel_target.y=-constrain_float(OpticalFlow_Ctrl_Output.y,-Total_Controller.Optical_Speed_Control.Control_OutPut_Limit,Total_Controller.Optical_Speed_Control.Control_OutPut_Limit);
    accel_target.x=-constrain_float(OpticalFlow_Ctrl_Output.x,-Total_Controller.Optical_Speed_Control.Control_OutPut_Limit,Total_Controller.Optical_Speed_Control.Control_OutPut_Limit);//�����˶����ٶ�
    
		accel_to_lean_angles(accel_target,&angle_target);//�����˶����ٶ�ת������̬���
    
		Total_Controller.Pitch_Angle_Control.Expect=angle_target.y;
    Total_Controller.Roll_Angle_Control.Expect=angle_target.x;
    
		OpticalFlow_Vel_Ctrl_Cnt=0;
  }
}


void OpticalFlow_Y_Vel_Control(float target_y)
{
  static uint16_t OpticalFlow_Vel_Ctrl_Cnt=0;
	
  OpticalFlow_Ctrl_Expect.y= target_y;
  
	OpticalFlow_Vel_Ctrl_Cnt++;
  
	//20ms����һ���ٶ� ��������Ƶ�ʹ��� ϵͳ��Ӧ������
	if(OpticalFlow_Vel_Ctrl_Cnt>=4)
  {
    OpticalFlow_Ctrl_Err.y=constrain_float(OpticalFlow_Ctrl_Expect.y-OpticalFlow_SINS.Speed[_ROLL] ,-Total_Controller.Optical_Speed_Control.Err_Max,Total_Controller.Optical_Speed_Control.Err_Max);
    
    OpticalFlow_Ctrl_Integrate.y+=Total_Controller.Optical_Speed_Control.Ki*OpticalFlow_Ctrl_Err.y;
    
    OpticalFlow_Ctrl_Integrate.y=constrain_float(OpticalFlow_Ctrl_Integrate.y,-Total_Controller.Optical_Speed_Control.Integrate_Max,Total_Controller.Optical_Speed_Control.Integrate_Max);
    
    OpticalFlow_Ctrl_Output.y=OpticalFlow_Ctrl_Integrate.y+Total_Controller.Optical_Speed_Control.Kp*OpticalFlow_Ctrl_Err.y;
    
    accel_target.y=-constrain_float(OpticalFlow_Ctrl_Output.y,-Total_Controller.Optical_Speed_Control.Control_OutPut_Limit,Total_Controller.Optical_Speed_Control.Control_OutPut_Limit);//450
    
		accel_to_lean_angles(accel_target,&angle_target);//�����˶����ٶ�ת������̬���
    
		Total_Controller.Pitch_Angle_Control.Expect=angle_target.y;
    
		OpticalFlow_Vel_Ctrl_Cnt=0;
  }
}








