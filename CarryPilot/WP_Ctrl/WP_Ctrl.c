#include "Headfile.h"
#include "WP_Ctrl.h"
#include "control_config.h"


/*****************ң�����г�����**********************/
//����������PWM
uint16 Motor_PWM_1,Motor_PWM_2,Motor_PWM_3,Motor_PWM_4,Motor_PWM_5,Motor_PWM_6;
//�ϴ�����������PWM
uint16 Last_Motor_PWM_1,Last_Motor_PWM_2,Last_Motor_PWM_3,Last_Motor_PWM_4,Last_Motor_PWM_5,Last_Motor_PWM_6;
//���Ʊ�־
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


//����ģʽѡ����
void Controler_Mode_Select()
{
	//������һ�εĿ���ģʽ
  Last_Controler_High_Mode=Controler_High_Mode;//�ϴθ߶ȿ���ģʽ
  Last_Controler_Horizontal_Mode=Controler_Horizontal_Mode;//�ϴ�λ�ÿ���ģʽ
  Last_Controler_Land_Mode=Controler_Land_Mode;//�ϴη���ģʽ
  Last_Reserve_Mode=Reserve_Mode;
	
	//ң����5ͨ��ѡ�񶨸�ģʽ
  if(PPM_Databuf[4]>=(RC_Calibration[4].max-RC_Calibration[4].deadband))//��λ    
		Controler_High_Mode=2;//��ѹ�� ����������
  else if(PPM_Databuf[4]<=(RC_Calibration[4].min+RC_Calibration[4].deadband))//��λ
		Controler_High_Mode=1;//����̬���� �޶���
  
	//ң����6ͨ��ѡ��GPS����ģʽ
  if(PPM_Databuf[5]>=(RC_Calibration[5].max-RC_Calibration[5].deadband))         
		Controler_Horizontal_Mode=2;//GPS����ˮƽλ��
  else if(PPM_Databuf[5]<=(RC_Calibration[5].min+RC_Calibration[5].deadband))   
		Controler_Horizontal_Mode=1;//��GPS����ˮƽλ��
  
	//ң����7ͨ���ߵ�λѡ�񷵺�ģʽ
  if(PPM_Databuf[6]>=(RC_Calibration[6].max-RC_Calibration[6].deadband))            
		Controler_Land_Mode=2;//����ģʽ
  else if(PPM_Databuf[6]<=(RC_Calibration[6].middle+RC_Calibration[6].deadband))   
		Controler_Land_Mode=1;//�Ƿ���ģʽ
  
	//ң����7ͨ����λѡ��SDKģʽ����
  if(PPM_Databuf[6]>=(RC_Calibration[6].middle-RC_Calibration[6].deadband)
     &&PPM_Databuf[6]<=(RC_Calibration[6].middle+RC_Calibration[6].deadband))//��λ  
    SDK_Ctrl_Mode=1;//����SDKģʽ
  else
  {
    SDK_Ctrl_Mode=0;//�ر�SDKģʽ
		NCQ_SDK_Reset();//SDK�����־���
  }
  
	//ң����8ͨ�� (����ͨ��)
  if(PPM_Databuf[7]>=(RC_Calibration[7].max-RC_Calibration[7].deadband))      
		Reserve_Mode=2;//����ģʽ2��������ģʽ
  else if(PPM_Databuf[7]<=(RC_Calibration[7].min+RC_Calibration[7].deadband)) 
  {
    Reserve_Mode=1;//����ģʽ1
		//������λ
    OpticalFlow_SINS_Reset();
    OpticalFlow_Ctrl_Reset();
  }
  
	//8ͨ�������л���ʱ
  if(Reserve_Mode_Cnt>=1) Reserve_Mode_Cnt--;
  if(Reserve_Mode_Cnt==0) Reserve_Mode_Fast_Exchange_Cnt=0;
	
	//����ģʽ�л�
  if(Last_Reserve_Mode!=Reserve_Mode)
  {
		//����ģʽ�л�������ģʽ
    if(Reserve_Mode==2)
    {
      
    }
		//����ģʽ�л���1
    if(Reserve_Mode==1)
    {
      Reserve_Mode_Cnt=600;	//3s����ʱ
      Reserve_Mode_Fast_Exchange_Cnt++;	//�����л�����
			
			//��ʱ�������л�����ͨ��3��(ÿ�ε��ﱣ��ģʽ1(��λ)����һ�ο����л�)
      if(Reserve_Mode_Fast_Exchange_Cnt>=3)
      {
        Reserve_Mode_Fast_Exchange_Flag=1;	//��־�����л����
        Reserve_Mode_Fast_Exchange_Cnt=0;		
				
				//�����Զ���ɸ߶�
        Total_Controller.High_Position_Control.Expect=NamelessQuad.Position[_YAW]+Auto_Launch_Target;
        //��������
				Unwanted_Lock_Flag=0;
      }
    }
  }
  
	//8ͨ��3�ο����л���� ���Զ����
  if(Reserve_Mode_Fast_Exchange_Flag==1)
  {
		//���ýϴ�Ĳ���Ϊ������Զ����
    Total_Controller.High_Acce_Control.Scale_Kp=1.0f;
    Total_Controller.High_Acce_Control.Scale_Ki=1.2f; 
    Total_Controller.High_Speed_Control.Scale_Kp=1.2;
    Total_Controller.High_Speed_Control.Scale_Ki=1.2f;
    Total_Controller.High_Position_Control.Scale_Kp=2.0;
    Total_Controller.High_Position_Control.Scale_Ki=1.0f; 
		
		//�������߶ȴ��������߶�(����������)��ָ���������
    if(Total_Controller.High_Position_Control.Expect<=NamelessQuad.Position[_YAW])
      Reserve_Mode_Fast_Exchange_Flag=0;   
  }
  else 
  { 
		//�ǿ������ģʽ ��������
    Total_Controller.High_Acce_Control.Scale_Kp=1.0f;
    Total_Controller.High_Acce_Control.Scale_Ki=1.0f; 
    Total_Controller.High_Speed_Control.Scale_Kp=1.0;
    Total_Controller.High_Speed_Control.Scale_Ki=1.0f; 
    Total_Controller.High_Position_Control.Scale_Kp=1.0;
    Total_Controller.High_Position_Control.Scale_Ki=1.0f; 
  }
  
	//�����Զ�����(����ģʽ���������κβ���)
  if(Unwanted_Lock_Flag==1)
  {
		//��ȡ�����ƶ�״̬
    Thr_Push_Over_State=Thr_Push_Over_Deadband();
		//(�Զ�ģʽ)����λ�Ƶ���λ �������Զ�����
    if(Thr_Push_Over_State==2)
      Unwanted_Lock_Flag=0;
    else
    {
      Take_Off_Reset();//�����
      //Throttle_Control_Reset();//�����
    }
  }
  
	//����ģʽ�л�
  if(Controler_Land_Mode!=Last_Controler_Land_Mode)
  {
    if(Controler_Land_Mode==1)  //�Ƿ���ģʽ
      land_reset();
		//�����ز���˲��Ĺߵ��߶�����Ϊ�����߶�
    Total_Controller.High_Position_Control.Expect=NamelessQuad.Position[_YAW];
  }
  
	//����ģʽ�л�
  if(Controler_High_Mode!=Last_Controler_High_Mode)
  {
		//�����ж��� ���û�׼����ֵ����ͣ�߶�
    if(Controler_High_Mode==2)  Control_Mode_Change=1;
		//����������
    if(Controler_High_Mode==1)  Control_Mode_Change=1;
  }
  
	//GPSģʽ�л�
  if(Controler_Horizontal_Mode!=Last_Controler_Horizontal_Mode)
  {
		//�����ж��� ������ͣ��
    if(Controler_Horizontal_Mode==2)  
		{
			Control_Mode_Change=2;
			Pos_Hold_SetFlag=0;
		}
		//����������
    if(Controler_Horizontal_Mode==1)  
		{
			Control_Mode_Change=2;
			Pos_Hold_SetFlag=1;
		}
  }
  
	//����ģʽ�л��Ĵ���
  if(Control_Mode_Change==1)
  {
		//����Ϊ����ģʽ(2) �������ж���
    if(Controler_High_Mode==High_Hold_Mode)
    {
			//���浱ǰ����ֵ(ֻ��һ��)
      High_Hold_Throttle=Throttle_Control;
      //�����ز���˲��Ĺߵ��߶�����Ϊ�����߶�(�߶�ֻ����һ��)
      Total_Controller.High_Position_Control.Expect=NamelessQuad.Position[_YAW];
    }
    else	//����������
    {
      //Throttle_Control_Reset();
    }
		//��ģʽ�л�λ��0 ->������һ��
    Control_Mode_Change=0;
  }
	//����ģʽ�л��Ĵ���
  else if(Control_Mode_Change==2)
  {
		//����Ϊ����ģʽ ���л�������
    if(Controler_Horizontal_Mode==Pos_Hold_Mode)
    {
			//����������ͣ������(��ǰδ������ͣ�� �� GPS��Ч)
      if(Pos_Hold_SetFlag==0&&(GPS_ok()==TRUE))
      {
        //����ǰ�ߵ�ˮƽλ�ù�����ΪĿ����ͣ��
        Total_Controller.Latitude_Position_Control.Expect=NamelessQuad.Position[_ROLL];
        Total_Controller.Longitude_Position_Control.Expect=NamelessQuad.Position[_PITCH];
				
				//���ˮƽ�ٶȺ�λ�ÿ��ƵĻ���
        PID_Integrate_Reset(&Total_Controller.Latitude_Speed_Control);		
        PID_Integrate_Reset(&Total_Controller.Latitude_Position_Control);	
        PID_Integrate_Reset(&Total_Controller.Longitude_Speed_Control);		
        PID_Integrate_Reset(&Total_Controller.Longitude_Position_Control);
				
        Pos_Hold_SetFlag=1;
      }
    }
    else	//�رն���
    {
			//���ˮƽ�ٶȺ�λ�ÿ��ƵĻ���			
      PID_Integrate_Reset(&Total_Controller.Latitude_Speed_Control);			
      PID_Integrate_Reset(&Total_Controller.Latitude_Position_Control);		
      PID_Integrate_Reset(&Total_Controller.Longitude_Speed_Control);			
      PID_Integrate_Reset(&Total_Controller.Longitude_Position_Control);	
    }
		//��־�����л���Ӧ���
    Control_Mode_Change=0;
  }
  
  //��ǰ��λΪ����ģʽ ����ʾ��ͣ��δ���� ˵��֮ǰδ�������ö�������
	//�����������
  //	1����ʼͨ�������ж���ģʽʱ GPS״̬δ������ͣ������
  //	2����ʼͨ�������ж���ģʽʱ GPS״̬δ������ͣ���� ֮����������Ȼδ����GPS����������
  //	3��֮ǰGPS״̬������ͣ���� ������GPS�ź�������� �Զ��л���������GPS����������
  //������ԣ�
	//	�����жϵ����Ƿ����㶨������ ����������������ͣ�� ������붨��ģʽ
  if(Controler_Horizontal_Mode==2)
  {
		//�״��ж��㲻���㶨������ ֮�������㶨������
    if(GPS_ok()==TRUE)
    {
			//δ������ͣ��(ע�������һ��)
      if(Pos_Hold_SetFlag==0)
      {
        //����ǰ�ߵ�ˮƽλ�ù�����ΪĿ����ͣ��
        Total_Controller.Latitude_Position_Control.Expect=NamelessQuad.Position[_ROLL];
        Total_Controller.Longitude_Position_Control.Expect=NamelessQuad.Position[_PITCH];
				
				//���ˮƽ�ٶȺ�λ�ÿ��ƵĻ���
        PID_Integrate_Reset(&Total_Controller.Latitude_Speed_Control);
        PID_Integrate_Reset(&Total_Controller.Latitude_Position_Control);
        PID_Integrate_Reset(&Total_Controller.Longitude_Speed_Control);
        PID_Integrate_Reset(&Total_Controller.Longitude_Position_Control);
				
        Pos_Hold_SetFlag=1;
      }
    }
    else	
    {
			//���㵵λ���ڶ���ģʽ ��δ���㶨������ 
			//��Pos_Hold_SetFlag��0 �ȴ�����ʱ��������ͣ��
      Pos_Hold_SetFlag=0;
			
			//���ˮƽ�ٶȺ�λ�ÿ��ƵĻ���
      PID_Integrate_Reset(&Total_Controller.Latitude_Speed_Control);		
      PID_Integrate_Reset(&Total_Controller.Latitude_Position_Control);	
      PID_Integrate_Reset(&Total_Controller.Longitude_Speed_Control);		
      PID_Integrate_Reset(&Total_Controller.Longitude_Position_Control);
    }
  }
}


//��ֵ����
uint16 Value_Limit(uint16 Min,uint16 Max,uint16 Data)
{
  if(Data>=Max) Data=Max;
  else if(Data<=Min) Data=Min;
  return Data;
}


//�Ƕȿ���
void Angle_Control()
{
  static uint16 Yaw_Cnt=0;
  
	//ƽ��Ƿ���������
  Total_Controller.Pitch_Angle_Control.FeedBack=(Pitch-Pitch_Offset);
  PID_Control(&Total_Controller.Pitch_Angle_Control);
  Total_Controller.Roll_Angle_Control.FeedBack =(Roll-Roll_Offset);
  PID_Control(&Total_Controller.Roll_Angle_Control);
  
  if(Yaw_Control==0)//ƫ����������λ
  {
		//��ͷģʽ
		//�ɻ��ϵ��һ��ʱ������ƫ���� 
		//������ �������ں���Ҫһ��ʱ�� ����ȡ500
    if(Yaw_Cnt<=500)
      Yaw_Cnt++;
		
    if(Total_Controller.Yaw_Angle_Control.Expect==0	//ƫ���޽Ƕȿ����� 
				||Yaw_Cnt<=500															//��ͷģʽ
         ||Landon_Earth_Flag==1)										//���㴥������
			//����ǰƫ��������Ϊ����ֵ
      Total_Controller.Yaw_Angle_Control.Expect=Yaw;
		
		//ƫ���Ƿ���������
    Total_Controller.Yaw_Angle_Control.FeedBack=Yaw;
    PID_Control_Yaw(&Total_Controller.Yaw_Angle_Control);
		
		//ƫ�����ٶȻ����� ��Դ��ƫ���Ƕȿ�����(�⻷)���
    Total_Controller.Yaw_Gyro_Control.Expect=Total_Controller.Yaw_Angle_Control.Control_OutPut;
    
		//SDKѲ��ģʽ�� ƫ����SDK����
    if(SDK_Line.line_ctrl_enable==1&&SDK_Ctrl_Mode==1)
    {
			//SDK������ƫ�����ٶ� ����ֱ�ӽ��нǶȿ���
      Total_Controller.Yaw_Gyro_Control.Expect=3*SDK_Target_Yaw_Gyro;
      Total_Controller.Yaw_Angle_Control.Expect=0;
      //SDK_Line.flag=0;
    }
  }
  else	//����ƫ������˺�
  {
		//������ƫ�����ٶ� ���ԽǶȽ��п���
    Total_Controller.Yaw_Angle_Control.Expect=0;
    Total_Controller.Yaw_Gyro_Control.Expect=Yaw_Control;
  }
}


uint16_t Yaw_Control_Fault_Cnt=0;
float Last_Yaw_Gyro_Control_Expect=0,Yaw_Gyro_Control_Expect_Delta=0;
float Last_Pitch_Gyro_Control_Expect=0,Pitch_Gyro_Control_Expect_Delta=0;
float Last_Roll_Gyro_Control_Expect=0,Roll_Gyro_Control_Expect_Delta=0;
//ƽ���ǰ�����Ʋ���(��������)(�˴���)
float Pitch_Roll_Feedforward_Kp=0.0f,Pitch_Roll_Feedforward_Kd=0.0f;//0.45
//ƫ����ǰ�����Ʋ���
float Yaw_Feedforward_Kp=0.0f,Yaw_Feedforward_Kd=0.05f;
//���ٶȿ���
void Gyro_Control()
{
  if(GYRO_CONTROL_MODE==PID_MODE)//���������������̬�ڻ����ٶȿ���������PID������
  {
    //���ٶȻ�����
    Total_Controller.Pitch_Gyro_Control.Expect=Total_Controller.Pitch_Angle_Control.Control_OutPut;
    Total_Controller.Roll_Gyro_Control.Expect=Total_Controller.Roll_Angle_Control.Control_OutPut;
    
    //���ٶȻ�����
    Total_Controller.Pitch_Gyro_Control.FeedBack=Pitch_Gyro;
    Total_Controller.Roll_Gyro_Control.FeedBack=Roll_Gyro;
     
    //���ٶȻ����� (΢�ֲ�����̬����)
    PID_Control_Div_LPF_For_Gyro(&Total_Controller.Pitch_Gyro_Control);
    PID_Control_Div_LPF_For_Gyro(&Total_Controller.Roll_Gyro_Control);
    
		//ƽ����ٶȿ����������Ĳ��
    Pitch_Gyro_Control_Expect_Delta=1000*(Total_Controller.Pitch_Gyro_Control.Expect-Last_Pitch_Gyro_Control_Expect
                                          /Total_Controller.Pitch_Gyro_Control.PID_Controller_Dt.Time_Delta);
    Roll_Gyro_Control_Expect_Delta=1000*(Total_Controller.Roll_Gyro_Control.Expect-Last_Roll_Gyro_Control_Expect
                                         /Total_Controller.Roll_Gyro_Control.PID_Controller_Dt.Time_Delta);
    
		//������һ��ƽ����ٶ�����
    Last_Pitch_Gyro_Control_Expect=Total_Controller.Pitch_Gyro_Control.Expect;
    Last_Roll_Gyro_Control_Expect=Total_Controller.Roll_Gyro_Control.Expect;
    
		//PITCH��ǰ������
    Total_Controller.Pitch_Gyro_Control.Control_OutPut+=Pitch_Roll_Feedforward_Kd*Pitch_Gyro_Control_Expect_Delta//΢��
      +Pitch_Roll_Feedforward_Kp*Total_Controller.Pitch_Gyro_Control.Expect;//����
		//����޷�
    Total_Controller.Pitch_Gyro_Control.Control_OutPut=constrain_float(Total_Controller.Pitch_Gyro_Control.Control_OutPut,
                                                                       -Total_Controller.Pitch_Gyro_Control.Control_OutPut_Limit,
                                                                       Total_Controller.Pitch_Gyro_Control.Control_OutPut_Limit);
    
		//ROLL��ǰ������
    Total_Controller.Roll_Gyro_Control.Control_OutPut+=Pitch_Roll_Feedforward_Kd*Roll_Gyro_Control_Expect_Delta
      +Pitch_Roll_Feedforward_Kp*Total_Controller.Roll_Gyro_Control.Expect;
		//����޷�
    Total_Controller.Roll_Gyro_Control.Control_OutPut=constrain_float(Total_Controller.Roll_Gyro_Control.Control_OutPut,
                                                                      -Total_Controller.Roll_Gyro_Control.Control_OutPut_Limit,
                                                                      Total_Controller.Roll_Gyro_Control.Control_OutPut_Limit);
  }
	//ƽ����ٶȿ��Ʋ���ADRC������
  else if(GYRO_CONTROL_MODE==ADRC_MODE)
  {
    
  }
  else	//(������)����ֻѡ��һ��ģʽ
  {
    //�ڻ����ٶ�����
    Total_Controller.Pitch_Gyro_Control.Expect=Total_Controller.Pitch_Angle_Control.Control_OutPut;
    Total_Controller.Roll_Gyro_Control.Expect=Total_Controller.Roll_Angle_Control.Control_OutPut;
    //�ڻ����ٶȷ���
    Total_Controller.Pitch_Gyro_Control.FeedBack=Pitch_Gyro;
    Total_Controller.Roll_Gyro_Control.FeedBack=Roll_Gyro;
    //�ڻ����ٶȿ���
    PID_Control_Div_LPF(&Total_Controller.Pitch_Gyro_Control);
    PID_Control_Div_LPF(&Total_Controller.Roll_Gyro_Control);
  }
  
	//���ٶȻ�����
  //Total_Controller.Yaw_Gyro_Control.FeedBack=Yaw_Gyro;
  Total_Controller.Yaw_Gyro_Control.FeedBack=Yaw_Gyro_Earth_Frame;//���ٶȼƼ���õ���ƫ�����ٶ�
  
  PID_Control_Div_LPF(&Total_Controller.Yaw_Gyro_Control);
	
	//ƫ�����ٶȿ����������Ĳ��
  Yaw_Gyro_Control_Expect_Delta=1000*(Total_Controller.Yaw_Gyro_Control.Expect-Last_Yaw_Gyro_Control_Expect)
    /Total_Controller.Yaw_Gyro_Control.PID_Controller_Dt.Time_Delta;
	
  //ƫ����ǰ������
  Total_Controller.Yaw_Gyro_Control.Control_OutPut+=Yaw_Feedforward_Kp*Total_Controller.Yaw_Gyro_Control.Expect
    +Yaw_Feedforward_Kd*Yaw_Gyro_Control_Expect_Delta;
  Total_Controller.Yaw_Gyro_Control.Control_OutPut=constrain_float(Total_Controller.Yaw_Gyro_Control.Control_OutPut,
                                                                   -Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit,
                                                                   Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit);
	
  Last_Yaw_Gyro_Control_Expect=Total_Controller.Yaw_Gyro_Control.Expect;
  
	//ƫ�������쳣����жϣ�
	//	��ƫ���������ܴ�ʱ ƫ�����ٶȺ�С ���ʱΪǿ�������š����ŵص�
  if(ABS(Total_Controller.Yaw_Gyro_Control.Control_OutPut)>Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit/2	//ƫ�����������Խϴ�
     &&ABS(Yaw_Gyro)<=30.0f)	//ƫ�����ٶ���Ժ�С
  {
		//���������ʱ
    Yaw_Control_Fault_Cnt++;
    if(Yaw_Control_Fault_Cnt>=500) Yaw_Control_Fault_Cnt=500;
  }
  else Yaw_Control_Fault_Cnt/=2;//������ ����������0
  
	//�����������5ms*400=2S ���⴦��
  if(Yaw_Control_Fault_Cnt>=400)
  {
		//���ƫ�����ƻ���
    PID_Integrate_Reset(&Total_Controller.Yaw_Gyro_Control);
    PID_Integrate_Reset(&Total_Controller.Yaw_Angle_Control);
		//����ǰƫ������Ϊ����ƫ����
    Total_Controller.Yaw_Angle_Control.Expect=Yaw;
    Yaw_Control_Fault_Cnt=0;
  }
}


uint16 Throttle=0,Last_Throttle=0;
//����������
void Main_Leading_Control(void)
{
  //�ɿ�ģʽ
  if(Controler_Land_Mode==1)//�Ƿ�����½ģʽ
  {
		//����ģʽ
    if(Controler_High_Mode==1						//��̬���ȶ�ģʽ
       &&Controler_Horizontal_Mode==1)	//GPS����δ����
    {
			//ƽ��Ƕ�ֱ������ң����
      Total_Controller.Pitch_Angle_Control.Expect=Target_Angle[0];
      Total_Controller.Roll_Angle_Control.Expect=Target_Angle[1];
      
      if(Throttle_Control<=1000)   Throttle=1000;
			//����ֱ����Դ��ң�������Ÿ���
      else Throttle=Throttle_Control;
			
			//������һ������ֵ
      Last_Throttle=Throttle;
    }
    else if(Controler_High_Mode==2		//����ģʽ
			&&Controler_Horizontal_Mode==1)	//GPS����δ����
    {
     //����ģʽ
#if (Optical_Enable==0)
			//�޹�����ˮƽ�Ƕ�ֱ������ң����
			Total_Controller.Pitch_Angle_Control.Expect=Target_Angle[0];
			Total_Controller.Roll_Angle_Control.Expect=Target_Angle[1];
#else 
		 //��������ģʽ
			//��������Ч�Ҵ��ڹ�������ʱ ������������ģʽ 
			if(Reserve_Mode==2&&Sensor_Flag.Hcsr04_Health==1&&OpticalFlow_Is_Work==1) 
      {  
				//����������OpenMV�����SDKģʽ
        if(SDK_Take_Over_Ctrl==1)       
        {
					//��������
          OpticalFlow_Control(0);
          //�߶ȿ���
					ncq_control_althold();
        }
				//�û��Զ����SDKģʽ
        else if(SDK_Take_Over_Ctrl==2)  
        {
					//�Զ���SDKģʽ1
					if(SDK_Ctrl_Mode==1)
						NCQ_SDK_Run();
          else //��SDKģʽ
					{
						OpticalFlow_Control_Pure(0);
						ncq_control_althold();
					}
        }
      }
      else //���������ģʽ
      {
        Total_Controller.Pitch_Angle_Control.Expect=Target_Angle[0];
        Total_Controller.Roll_Angle_Control.Expect=Target_Angle[1];
      }
#endif
      ncq_control_althold();//�߶ȿ���
    }
    else if(Controler_High_Mode==2		//����ģʽ
			&&Controler_Horizontal_Mode==2)	//GPS���㿪��
    {
      ncq_control_althold();//�߶ȿ���
      ncq_control_poshold();//λ�ÿ���
    }
    else	//����
    {
      Total_Controller.Pitch_Angle_Control.Expect=Target_Angle[0];
      Total_Controller.Roll_Angle_Control.Expect=Target_Angle[1];
			
      if(Throttle_Control<=1000)   Throttle=1000;
			else Throttle=Throttle_Control;
      Last_Throttle=Throttle;
    }
    land_state_check();
  }
  else //ը����������ģʽ
  {
		//��ģʽԭ��Ϊ�Զ�����/�Զ�����ģʽ
		//land_run();
		
		//PWMGenDisable(PWM0_BASE, PWM_GEN_0);
		//PWMGenDisable(PWM0_BASE, PWM_GEN_1);
		//PWMGenDisable(PWM0_BASE, PWM_GEN_2);
		//PWMGenDisable(PWM0_BASE, PWM_GEN_3);
		
		PWM_Output(Thr_Min,Thr_Min,Thr_Min,Thr_Min,Thr_Min,Thr_Min,0,0);
  }
}


//��̬�ܿ�����
void Attitude_Control(void)
{
  Angle_Control();//�Ƕȿ���
  Gyro_Control();	//���ٶȿ���
}


float Active_Para1,Active_Para2;
//�ܿ�����
void Total_Control(void)
{
  Main_Leading_Control();	//����������		
  Attitude_Control();			//��̬������
}


//����������
void CarryPilot_Control(void)
{
  static uint8_t ctrl_cnt=0;
  ctrl_cnt++;  

	//����ģʽѡ��  
	if(ctrl_cnt>=4)
  {  
    Controler_Mode_Select();
    ctrl_cnt=0;
  }
	
	//�ܿ�����
	//ˮƽλ��+ˮƽ�ٶ�+��̬(�Ƕ�+���ٶ�)����
	//�߶�λ��+�߶��ٶ�+�߶ȼ��ٶȿ���	
  Total_Control();
	
	//�����������
  Control_Output();
}


uint16_t Throttle_Output=0;
//������ǲ���
//	��Ϊ�߶ȿ��Ƽ�����Ľ��Ϊ��ֱ�����ϵ���������
//	��ҪͶӰ����������ϵ�õ����������������
//	���ƻ��ڣ�1/CosPitch_CosRoll
void Throttle_Angle_Compensate()
{
  float CosPitch_CosRoll=ABS(Cos_Pitch*Cos_Roll);
  float Throttle_Makeup=0;
  float Temp=0;
	
  if(CosPitch_CosRoll>=0.999999f) CosPitch_CosRoll=0.999999f;
  if(CosPitch_CosRoll<=0.000001f) CosPitch_CosRoll=0.000001f;
  if(CosPitch_CosRoll<=0.50f)  		CosPitch_CosRoll=0.50f;//Pitch,RollԼ����30��
  
	if(Throttle>=Thr_Start)//������ת������
  {
    Temp=(uint16_t)(MAX(ABS(100*Pitch),ABS(100*Roll)));
    Temp=constrain_float(9000-Temp,0,3000)/(3000*CosPitch_CosRoll);
		
		//������ǲ���
    Throttle_Makeup=(Throttle-Thr_Start)*Temp;
		
    Throttle_Output=(uint16_t)(Thr_Start+Throttle_Makeup);
    Throttle_Output=(uint16_t)(constrain_float(Throttle_Output,Thr_Start,2000));
  }
  else Throttle_Output=Throttle;
}


/*

X�Ͱ�װ��ʽ ����������̬�ǹ�ϵ
                   -
                 Pitch
					3#             1#
					   *          *
-   Roll          *         Roll   +
						 *          *
					2#             4#
				         Pitch
								   +
���ٶȴ���������������X��Y��Zͬ��
������ԭ�㿴 ��ʱ����ת�Ƕ�Ϊ+
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
//���ٵ������ʱ�� 10*5=50ms
#define Idel_Transition_Gap 10
//�����������ɹ��� �������� 50ms*100=5s
#define Idel_Transition_Period (Thr_Idle-Thr_Min)
uint16_t Thr_Idle_Transition_Cnt=0;
//���������
void Control_Output()
{
  Throttle_Angle_Compensate();//������ǲ���
  landon_earth_check();//��½�����Լ�
	
	//���������ڽ���״̬
  if(Controler_State==Unlock_Controler)
  {
		//��⵽��½����
    if(Landon_Earth_Flag==1)
    {
      if(Last_Motor_PWM_1<=Thr_Min
         &&Last_Motor_PWM_2<=Thr_Min
           &&Last_Motor_PWM_3<=Thr_Min
             &&Last_Motor_PWM_4<=Thr_Min
               //&&Last_Motor_PWM_5<=Thr_Min
								//&&Last_Motor_PWM_6<=Thr_Min
									)//ֻ���������ٽ���ʱ�Ż�����
      {
        //����ϴ��������ֵΪ���λ ���뵡��ʱ ���Ź��ɹ���
        Thr_Idle_Transition_Cnt=Idel_Transition_Period;
      }
      else	//����ʱ�̼�⵽��½����
      {
				//�ϴ�Ϊ���״̬ ����Ϊ��½״̬ ���������
        if(Last_Landon_Earth_Flag==0)
        {
          Controler_State=Lock_Controler;
					
					//LED����
          Bling_Set(&Light_1,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
          Bling_Set(&Light_2,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
          Bling_Set(&Light_3,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
        }
      }
      
      Idel_Cnt++;//���ټ�ʱ
			
			//���ﵡ�ٵ������ʱ��(50ms)
      if(Idel_Cnt>=Idel_Transition_Gap)
      {
        if(Thr_Idle_Transition_Cnt>=1)
          Thr_Idle_Transition_Cnt--;//ʵ���ٶȵ����ĵ���
        Idel_Cnt=0;
      }
			
			//���ŵ����ĵ���
      Motor_PWM_1=Thr_Min+(Idel_Transition_Period-Thr_Idle_Transition_Cnt)*(Thr_Idle-Thr_Min)/Idel_Transition_Period;
      Motor_PWM_2=Thr_Min+(Idel_Transition_Period-Thr_Idle_Transition_Cnt)*(Thr_Idle-Thr_Min)/Idel_Transition_Period;
      Motor_PWM_3=Thr_Min+(Idel_Transition_Period-Thr_Idle_Transition_Cnt)*(Thr_Idle-Thr_Min)/Idel_Transition_Period;
      Motor_PWM_4=Thr_Min+(Idel_Transition_Period-Thr_Idle_Transition_Cnt)*(Thr_Idle-Thr_Min)/Idel_Transition_Period;
      Motor_PWM_5=Thr_Min+(Idel_Transition_Period-Thr_Idle_Transition_Cnt)*(Thr_Idle-Thr_Min)/Idel_Transition_Period;
      Motor_PWM_6=Thr_Min+(Idel_Transition_Period-Thr_Idle_Transition_Cnt)*(Thr_Idle-Thr_Min)/Idel_Transition_Period;
			
      Take_Off_Reset();//�����
			
			//���������������
      OpticalFlow_SINS_Reset();
      OpticalFlow_Ctrl_Reset();
    }
		
    else  //������������½���� Ĭ�����
    {
      if(Controler_High_Mode==1)//��̬��λ
      {
        if(Throttle>=Thr_Fly_Start)//�����������
        {
          if(GYRO_CONTROL_MODE==PID_MODE)//ˮƽ��̬�����ٶȿ�������Դ��PID
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
					
          else//ˮƽ��̬�����ٶȶ���������Դ��ADRC
          {
            
          }
					
        }
        else	//С��������� �����Ų�����̬����
        {
          Motor_PWM_1=Int_Sort(Throttle_Output);
          Motor_PWM_2=Int_Sort(Throttle_Output);
          Motor_PWM_3=Int_Sort(Throttle_Output);
          Motor_PWM_4=Int_Sort(Throttle_Output);
          Motor_PWM_5=Int_Sort(Throttle_Output);
          Motor_PWM_6=Int_Sort(Throttle_Output);
					
          Take_Off_Reset();//�����
        }
				
				//PWM����޷�
        Motor_PWM_1=Value_Limit(Thr_Idle,2000,Motor_PWM_1);
        Motor_PWM_2=Value_Limit(Thr_Idle,2000,Motor_PWM_2);
        Motor_PWM_3=Value_Limit(Thr_Idle,2000,Motor_PWM_3);
        Motor_PWM_4=Value_Limit(Thr_Idle,2000,Motor_PWM_4);
        Motor_PWM_5=Value_Limit(Thr_Idle,2000,Motor_PWM_5);
        Motor_PWM_6=Value_Limit(Thr_Idle,2000,Motor_PWM_6);
				
				Throttle_Control_Reset();
      }
			
			//���ߵ�λ �����й�
      else if(Controler_High_Mode==2)
      {
				//��������ȵ�λ ��û���ж��Ƿ�����������
				
        if(GYRO_CONTROL_MODE==PID_MODE)//ˮƽ��̬�����ٶȶ���������Դ��PID
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
				
        else//ˮƽ��̬�����ٶȶ���������Դ��ADRC
        { 
          
        }
				
				//PWM����޷�
        Motor_PWM_1=Value_Limit(Thr_Idle,2000,Motor_PWM_1);
        Motor_PWM_2=Value_Limit(Thr_Idle,2000,Motor_PWM_2);
        Motor_PWM_3=Value_Limit(Thr_Idle,2000,Motor_PWM_3);
        Motor_PWM_4=Value_Limit(Thr_Idle,2000,Motor_PWM_4);
        Motor_PWM_5=Value_Limit(Thr_Idle,2000,Motor_PWM_5);
        Motor_PWM_6=Value_Limit(Thr_Idle,2000,Motor_PWM_6);
      }
    }
  }
  else	//δ���� ��������� ���ͣת
  {
		Motor_PWM_1=Thr_Min;
		Motor_PWM_2=Thr_Min;
		Motor_PWM_3=Thr_Min;
		Motor_PWM_4=Thr_Min;
		Motor_PWM_5=Thr_Min;
		Motor_PWM_6=Thr_Min;
		
		Take_Off_Reset();//�����
		
		Throttle_Control_Reset();
	}
	
	//������һ�ε�PWM���
  Last_Motor_PWM_1=Motor_PWM_1;
  Last_Motor_PWM_2=Motor_PWM_2;
  Last_Motor_PWM_3=Motor_PWM_3;
  Last_Motor_PWM_4=Motor_PWM_4;
  Last_Motor_PWM_5=Motor_PWM_5;
  Last_Motor_PWM_6=Motor_PWM_6;
  
	//PWM������޷�
  Motor_PWM_1=Value_Limit(0,2000,Motor_PWM_1);
  Motor_PWM_2=Value_Limit(0,2000,Motor_PWM_2);
  Motor_PWM_3=Value_Limit(0,2000,Motor_PWM_3);
  Motor_PWM_4=Value_Limit(0,2000,Motor_PWM_4);
  Motor_PWM_5=Value_Limit(0,2000,Motor_PWM_5);
  Motor_PWM_6=Value_Limit(0,2000,Motor_PWM_6);
	
	//������棿
  PWM_Output(1.25*Motor_PWM_1,1.25*Motor_PWM_2,1.25*Motor_PWM_3,1.25*Motor_PWM_4,1.25*Motor_PWM_5,1.25*Motor_PWM_6,0,0);
}





