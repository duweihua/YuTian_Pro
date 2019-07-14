#include "Headfile.h"
#include "Filter.h"
#include "PID.h"

/*
1ƫ���޷���־��  2�����޷���־��3���ַ����־��   4������
5����            6ƫ�        7�ϴ�ƫ�       8ƫ���޷�ֵ��
9���ַ���ƫ��ֵ��10����ֵ       11�����޷�ֵ��    12���Ʋ���Kp��
13���Ʋ���Ki��   14���Ʋ���Kd�� 15�������������  16�ϴο����������
17������޷��ȣ� 18����ֿ���ʱ�Ļ�������
*/
const float Control_Unit[18][20]=
{
  //                                         Kp        Ki      Kd            
  //1  2  3  4  5  6   7  8    9   10  11    12        13      14    15  16  17   18
  //��ӯ����20AĬ�Ϲ̼�F330 ���KV1100
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 80,   4.00   ,0.0000  ,0.00  ,0  ,0 , 800,  1 ,  1 ,  1 },//Pitch_Angle;ƫ���Ƕ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,600 ,0  ,0 , 300,  0.60   ,1.5000  ,0.80  ,0  ,0 , 500,  1 ,  1 ,  1 },//Pitch_Gyro;ƫ�����ٶ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 80,   4.00   ,0.0000  ,0.00  ,0  ,0 , 800,  1 ,  1 ,  1 },//Roll_Angle;�����
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,600 ,0  ,0 , 300,  0.60   ,1.5000  ,0.80  ,0  ,0 , 500,  1 ,  1 ,  1 },//Roll_Gyro;������ٶ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,45  ,0  ,0 , 150 , 4.00   ,0.0000  ,0.00  ,0  ,0 , 800,  1 ,  1 ,  1 },//Yaw_Angle;ƫ����
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,300 ,0  ,0 , 200,  1.20   ,0.5000  ,0.00  ,0  ,0 , 500,  1 ,  1 ,  1 },//Yaw_Gyro;ƫ�����ٶ� 	

/*
	//��ӯ����20AĬ�Ϲ̼�NC330 ���KV930
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 80,   4.00   ,0.0000  ,0.00  ,0  ,0 , 800,  1 ,  1 ,  1 },//Pitch_Angle;ƫ���Ƕ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,600 ,0  ,0 , 300,  0.80   ,1.5000  ,1.20  ,0  ,0 , 500,  1 ,  1 ,  1 },//Pitch_Gyro;ƫ�����ٶ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 80,   4.00   ,0.0000  ,0.00  ,0  ,0 , 800,  1 ,  1 ,  1 },//Roll_Angle;�����
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,600 ,0  ,0 , 300,  0.80   ,1.5000  ,1.20  ,0  ,0 , 500,  1 ,  1 ,  1 },//Roll_Gyro;������ٶ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,45  ,0  ,0 , 150 , 4.00   ,0.0000  ,0.00  ,0  ,0 , 800,  1 ,  1 ,  1 },//Yaw_Angle;ƫ����
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,300 ,0  ,0 , 200,  1.20   ,0.5000  ,0.00  ,0  ,0 , 500,  1 ,  1 ,  1 },//Yaw_Gyro;ƫ�����ٶ� 
*/ 
		
  /*
  //��ӯ����20AĬ�Ϲ̼�F330 ���KV1400
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 80,   7.500   ,0.0000  ,0.00  ,0  ,0 , 600,  1 ,  1 ,  1 },//Pitch_Angle;ƫ���Ƕ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,600 ,0  ,0 , 300,  0.475   ,1.0000  ,2.75  ,0  ,0 , 500,  1 ,  1 ,  1 },//Pitch_Gyro;ƫ�����ٶ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 80,   7.500   ,0.0000  ,0.00  ,0  ,0 , 600,  1 ,  1 ,  1 },//Roll_Angle;�����
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,600 ,0  ,0 , 300,  0.475   ,1.0000  ,2.75  ,0  ,0 , 500,  1 ,  1 ,  1 },//Roll_Gyro;������ٶ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,45  ,0  ,0 , 150 , 7.500   ,0.0000  ,0.00  ,0  ,0 , 300,  1 ,  1 ,  1 },//Yaw_Angle;ƫ����
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,300 ,0  ,0 , 200,  0.800   ,0.2000  ,0.00  ,0  ,0 , 500,  1 ,  1 ,  1 },//Yaw_Gyro;ƫ�����ٶ�
	*/ 
  /*		
	//��ӯ����20AĬ�Ϲ̼�NC330 ���KV2300  2204  7����Ҷ��
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 80,   6.500   ,0.0000  ,0.00  ,0  ,0 , 600,  1 ,  1 ,  1 },//Pitch_Angle;ƫ���Ƕ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,600 ,0  ,0 , 300,  0.520   ,2.5000  ,2.75  ,0  ,0 , 500,  1 ,  1 ,  1 },//Pitch_Gyro;ƫ�����ٶ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 80,   6.500   ,0.0000  ,0.00  ,0  ,0 , 600,  1 ,  1 ,  1 },//Roll_Angle;�����
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,600 ,0  ,0 , 300,  0.520   ,2.5000  ,2.75  ,0  ,0 , 500,  1 ,  1 ,  1 },//Roll_Gyro;������ٶ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,45  ,0  ,0 , 150 , 6.500   ,0.0000  ,0.00  ,0  ,0 , 300,  1 ,  1 ,  1 },//Yaw_Angle;ƫ����
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,300 ,0  ,0 200,  1.100   ,0.3000  ,0.00  ,0  ,0 , 500,  1 ,  1 ,  1 },//Yaw_Gyro;ƫ�����ٶ�	'
  */
  //���߲���
  //�߶ȵ���������ƣ���ƫ���޷����������Ϊ����������½��ٶ�400cm/s
  //Z���ٶȱ���+���ֿ��ƣ���ƫ���޷�
#if (YAW_Pos_Control_Accel_Disable==1)
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,200 ,0  ,0 ,50 ,   1.0     ,0.000   ,0    ,0   ,0 ,400,  1 ,  1 ,  1 },//High_Position;���θ߶�λ��
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,400 ,0  ,0 ,400 ,  2.0     ,10.00   ,0.15  ,0  ,0 ,600,  1 ,  1 ,  1 },//High_Speed;���������ٶ�
#else
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,200 ,0  ,0 ,100 ,  1.00     ,0.000   ,0    ,0  ,0 ,400,  1 ,  1 ,  1 },//High_Position;���θ߶�λ��
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,400 ,0  ,0 ,500 ,  5.00     ,0.000   ,0    ,0  ,0 ,500,  1 ,  1 ,  1 },//High_Speed;���������ٶ�
#endif
  /*
  1ƫ���޷���־��  2�����޷���־��3���ַ����־��    4������
  5����            6ƫ�        7�ϴ�ƫ�       8ƫ���޷�ֵ��
  9���ַ���ƫ��ֵ��10����ֵ       11�����޷�ֵ��    12���Ʋ���Kp��
  13���Ʋ���Ki��   14���Ʋ���Kd�� 15�������������  16�ϴο����������
  17������޷���
  */
  /*                                       Kp        Ki        Kd            */
  /*1  2  3  4  5  6   7  8   9   10  11    12        13        14  15  16  17*/
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,180 ,0  ,0 ,8,   0.200    ,0.000    ,0    ,0    ,0 ,150,  1 ,  1 ,  1 },//Longitude_Position;ˮƽ����λ��
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,300 ,0  ,0 ,240, 1.800    ,0.450    ,0    ,0    ,0 ,500,  1 ,  1 ,  1 },//Longitude_Speed;ˮƽ�����ٶ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,180 ,0  ,0 ,8,   0.200    ,0.000    ,0    ,0    ,0 ,150,  1 ,  1 ,  1 },//Latitude_Position;ˮƽγ��λ��
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,300 ,0  ,0 ,240, 1.800    ,0.450    ,0    ,0    ,0 ,500,  1 ,  1 ,  1 },//Latitude_Speed;ˮƽγ���ٶ�
  /*************���ٶȿ�����****************/
  //���������ٶ�500cm/s^2
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,1000 ,0  ,0 ,750,0.12    ,1.0000    ,0.0  ,0   ,0 ,1000,  1 ,  1 ,  1 },//��ֱ���ٶȿ�����
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,100  ,0  ,0 ,3,  0.32    ,0.0000    ,0    ,0   ,0 ,150,  1 ,  1 ,  1 },//ˮƽ���ȷ�����ٶȿ�����
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,100  ,0  ,0 ,15, 0.45    ,0.0000    ,0.0  ,0   ,0 ,25 ,  1 ,  1 ,  1 },//ˮƽά�ȷ�����ٶȿ�����
  /*************����λ�á��ٶȿ�����****************/
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,100   ,15  ,0 ,15,  0.25  ,0.00   ,0    ,0   ,0 ,30,   1 ,  1 ,  1 },//ˮƽ���ȷ�����ٶȿ�����
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,100   ,30  ,0 ,200, 4.50  ,0.10   ,0.0  ,0   ,0 ,450 , 1 ,  1 ,  1 },//ˮƽά�ȷ�����ٶȿ�����
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30    ,30  ,0 ,10,  2.50  ,0.00   ,150.0  ,0    ,0 ,500  , 1 ,  1 ,  1 },//ˮƽά�ȷ�����ٶȿ�����
};

/*
Butter_Parameter Control_Device_Div_LPF_Parameter={
//200---20hz
1,    -1.14298050254,   0.4128015980962,
0.06745527388907,   0.1349105477781,  0.06745527388907
};

Butter_Parameter Control_Device_Err_LPF_Parameter={
//200hz---2hz
1,   -1.911197067426,   0.9149758348014,
0.0009446918438402,  0.00188938368768,0.0009446918438402
};
*/
Butter_Parameter Control_Device_Div_LPF_Parameter_For_Gyro;
Butter_Parameter Control_Device_Div_LPF_Parameter;
Butter_Parameter Control_Device_Err_LPF_Parameter;
Butter_Parameter Control_Device_SDK_Err_LPF_Parameter;
AllControler Total_Controller;//ϵͳ�ܿ�����


void PID_Init(PID_Controler *Controler,Controler_Label Label)
{
  Controler->Err_Limit_Flag=(uint8)(Control_Unit[Label][0]);//1ƫ���޷���־
  Controler->Integrate_Limit_Flag=(uint8)(Control_Unit[Label][1]);//2�����޷���־
  Controler->Integrate_Separation_Flag=(uint8)(Control_Unit[Label][2]);//3���ַ����־
  Controler->Expect=Control_Unit[Label][3];//4����
  Controler->FeedBack=Control_Unit[Label][4];//5����ֵ
  Controler->Err=Control_Unit[Label][5];//6ƫ��
  Controler->Last_Err=Control_Unit[Label][6];//7�ϴ�ƫ��
  Controler->Err_Max=Control_Unit[Label][7];//8ƫ���޷�ֵ
  Controler->Integrate_Separation_Err=Control_Unit[Label][8];//9���ַ���ƫ��ֵ
  Controler->Integrate=Control_Unit[Label][9];//10����ֵ
  Controler->Integrate_Max=Control_Unit[Label][10];//11�����޷�ֵ
  Controler->Kp=Control_Unit[Label][11];//12���Ʋ���Kp
  Controler->Ki=Control_Unit[Label][12];//13���Ʋ���Ki
  Controler->Kd=Control_Unit[Label][13];//14���Ʋ���Ki
  Controler->Control_OutPut=Control_Unit[Label][14];//15�����������
  Controler->Last_Control_OutPut=Control_Unit[Label][15];//16�ϴο����������
  Controler->Control_OutPut_Limit=Control_Unit[Label][16];//17�ϴο����������
  Controler->Scale_Kp=Control_Unit[Label][17];
  Controler->Scale_Ki=Control_Unit[Label][18];
  Controler->Scale_Kd=Control_Unit[Label][19];

}


//PID��ʼ��
void Total_PID_Init(void)
{
	//��̬PID
  PID_Init(&Total_Controller.Pitch_Angle_Control,Pitch_Angle_Controler);
  PID_Init(&Total_Controller.Pitch_Gyro_Control,Pitch_Gyro_Controler);  
	PID_Init(&Total_Controller.Roll_Angle_Control,Roll_Angle_Controler);
  PID_Init(&Total_Controller.Roll_Gyro_Control,Roll_Gyro_Controler);  
	PID_Init(&Total_Controller.Yaw_Angle_Control,Yaw_Angle_Controler);
  PID_Init(&Total_Controller.Yaw_Gyro_Control,Yaw_Gyro_Controler);
	
	//�߶�PID
  PID_Init(&Total_Controller.High_Position_Control,High_Position_Controler);
  PID_Init(&Total_Controller.High_Speed_Control,High_Speed_Controler);
  PID_Init(&Total_Controller.High_Acce_Control,High_Acce_Controler);	
  
	//GPS����PID
	PID_Init(&Total_Controller.Longitude_Position_Control,Longitude_Position_Controler);
  PID_Init(&Total_Controller.Longitude_Speed_Control,Longitude_Speed_Controler);
  PID_Init(&Total_Controller.Latitude_Position_Control,Latitude_Position_Controler);
  PID_Init(&Total_Controller.Latitude_Speed_Control,Latitude_Speed_Controler);
  PID_Init(&Total_Controller.Longitude_Acce_Control,Longitude_Acce_Controler);
  PID_Init(&Total_Controller.Latitude_Acce_Control,Latitude_Acce_Controler);
  
	//��������PID
  PID_Init(&Total_Controller.Optical_Position_Control,Optical_Position_Controler);
  PID_Init(&Total_Controller.Optical_Speed_Control,Optical_Speed_Controler);
  
	//SDK PID
	PID_Init(&Total_Controller.SDK_Roll_Position_Control,SDK_Roll_Position_Controler);
  PID_Init(&Total_Controller.SDK_Pitch_Position_Control,SDK_Roll_Position_Controler);
  
  Set_Cutoff_Frequency(100, 5 ,&Control_Device_SDK_Err_LPF_Parameter);
  Set_Cutoff_Frequency(WP_Duty_Freq, 2 ,&Control_Device_Err_LPF_Parameter);
  Set_Cutoff_Frequency(WP_Duty_Freq, 20,&Control_Device_Div_LPF_Parameter);
  Set_Cutoff_Frequency(WP_Duty_Freq, 10,&Control_Device_Div_LPF_Parameter_For_Gyro);
}


//
float PID_Control(PID_Controler *Controler)
{
  float controller_dt=0;
  Test_Period(&Controler->PID_Controller_Dt);
  controller_dt=Controler->PID_Controller_Dt.Time_Delta/1000.0000f;
  if(controller_dt<0.001f) return 0;
  
	//������һ�ε�ƫ��
  Controler->Last_Err=Controler->Err;
	//ƫ�����
  Controler->Err=Controler->Expect-Controler->FeedBack;
	
	//ƫ���޷�
  if(Controler->Err_Limit_Flag==1)//ƫ���޷��ȱ�־λ
  {
    if(Controler->Err>=Controler->Err_Max)   Controler->Err= Controler->Err_Max;
    if(Controler->Err<=-Controler->Err_Max)  Controler->Err=-Controler->Err_Max;
  }
  
	//���ּ���
  if(Controler->Integrate_Separation_Flag==1)//���ַ����־λ
  {
    if(ABS(Controler->Err)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
 
	//�����޷�
  if(Controler->Integrate_Limit_Flag==1)//�����޷���־
  {
    if(Controler->Integrate>=Controler->Integrate_Max)
      Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)
      Controler->Integrate=-Controler->Integrate_Max ;
  }
  
	//���������
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//������һ�ε������
  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err + Controler->Integrate + Controler->Kd*(Controler->Err-Controler->Last_Err);
  
	//������޷�
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  
  return Controler->Control_OutPut;
}



float PID_Control_Yaw(PID_Controler *Controler)
{
  float controller_dt=0;
  Test_Period(&Controler->PID_Controller_Dt);
  controller_dt=Controler->PID_Controller_Dt.Time_Delta/1000.0000f;;
  if(controller_dt<0.001f) return 0;
  /*******ƫ�����*********************/
  Controler->Last_Err=Controler->Err;//�����ϴ�ƫ��
  Controler->Err=Controler->Expect-Controler->FeedBack;//������ȥ�����õ�ƫ��
  /***********************ƫ����ƫ���+-180����*****************************/
  if(Controler->Err<-180)  Controler->Err=Controler->Err+360;
  if(Controler->Err>180)  Controler->Err=Controler->Err-360;
  
  if(Controler->Err_Limit_Flag==1)//ƫ���޷��ȱ�־λ
  {
    if(Controler->Err>=Controler->Err_Max)   Controler->Err= Controler->Err_Max;
    if(Controler->Err<=-Controler->Err_Max)  Controler->Err=-Controler->Err_Max;
  }
  /*******���ּ���*********************/
  if(Controler->Integrate_Separation_Flag==1)//���ַ����־λ
  {
    if(ABS(Controler->Err)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  /*******�����޷�*********************/
  if(Controler->Integrate_Limit_Flag==1)//�������Ʒ��ȱ�־
  {
    if(Controler->Integrate>=Controler->Integrate_Max)
      Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)
      Controler->Integrate=-Controler->Integrate_Max ;
  }
  /*******���������*********************/
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//���ֵ����
  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err//����
    +Controler->Integrate//����
      +Controler->Kd*(Controler->Err-Controler->Last_Err);//΢��
  /*******������޷�*********************/
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  /*******���������*********************/
  return Controler->Control_OutPut;
}



float Control_Device_LPF(float curr_inputer,Butter_BufferData *Buffer,Butter_Parameter *Parameter)
{
  /* ���ٶȼ�Butterworth�˲� */
  /* ��ȡ����x(n) */
  Buffer->Input_Butter[2]=curr_inputer;
  /* Butterworth�˲� */
  Buffer->Output_Butter[2]=
    Parameter->b[0] * Buffer->Input_Butter[2]
      +Parameter->b[1] * Buffer->Input_Butter[1]
	+Parameter->b[2] * Buffer->Input_Butter[0]
          -Parameter->a[1] * Buffer->Output_Butter[1]
            -Parameter->a[2] * Buffer->Output_Butter[0];
  /* x(n) ���б��� */
  Buffer->Input_Butter[0]=Buffer->Input_Butter[1];
  Buffer->Input_Butter[1]=Buffer->Input_Butter[2];
  /* y(n) ���б��� */
  Buffer->Output_Butter[0]=Buffer->Output_Butter[1];
  Buffer->Output_Butter[1]=Buffer->Output_Butter[2];
  return (Buffer->Output_Butter[2]);
}

float PID_Control_Div_LPF(PID_Controler *Controler)
{
  int16  i=0;
  float tempa,tempb,tempc,max,min;//���ڷ������˲�
  float controller_dt=0;
  Test_Period(&Controler->PID_Controller_Dt);
  controller_dt=Controler->PID_Controller_Dt.Time_Delta/1000.0000f;;
  if(controller_dt<0.001f) return 0;
  /*******ƫ�����*********************/
  Controler->Last_Err=Controler->Err;//�����ϴ�ƫ��
  Controler->Err=Controler->Expect-Controler->FeedBack;//������ȥ�����õ�ƫ��
  Controler->Dis_Err=Controler->Err-Controler->Last_Err;//ԭʼ΢��
  
  /******************************************/
  //��ֵ�˲�����֤�õ����ݲ����䣬����������Ծʱ��΢������쳣
  tempa=Controler->Pre_Last_Dis_Err_LPF;
  tempb=Controler->Last_Dis_Err_LPF;
  tempc=Controler->Dis_Err;
  max = tempa > tempb ? tempa:tempb;
  max = max > tempc ? max:tempc;
  min = tempa < tempb ? tempa:tempb;
  min = min < tempc ? min:tempc;
  if(tempa > min && tempa < max)    Controler->Dis_Err = tempa;
  if(tempb > min  && tempb < max )  Controler->Dis_Err = tempb;
  if(tempc > min  &&  tempc < max)  Controler->Dis_Err = tempc;
  Controler->Pre_Last_Dis_Err_LPF = Controler->Last_Dis_Err_LPF;
  Controler->Last_Dis_Err_LPF = Controler->Dis_Err;
  /*****************************************/
  
  for(i=4;i>0;i--)//���ֵ�ͨ��΢�����
  {
    Controler->Dis_Error_History[i]=Controler->Dis_Error_History[i-1];
  }
  Controler->Dis_Error_History[0]=Control_Device_LPF(Controler->Dis_Err,
                                                     &Controler->Control_Device_LPF_Buffer,
                                                     &Control_Device_Div_LPF_Parameter);//������˹��ͨ��õ���΢����,20hz
  
  if(Controler->Err_Limit_Flag==1)//ƫ���޷��ȱ�־λ
  {
    if(Controler->Err>=Controler->Err_Max)   Controler->Err= Controler->Err_Max;
    if(Controler->Err<=-Controler->Err_Max)  Controler->Err=-Controler->Err_Max;
  }
  /*******���ּ���*********************/
  if(Controler->Integrate_Separation_Flag==1)//���ַ����־λ
  {
    if(ABS(Controler->Err)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  /*******�����޷�*********************/
  if(Controler->Integrate_Limit_Flag==1)//�������Ʒ��ȱ�־
  {
    if(Controler->Integrate>=Controler->Integrate_Max)
      Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)
      Controler->Integrate=-Controler->Integrate_Max ;
  }
  /*******���������*********************/
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//���ֵ����
  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err//����
    +Controler->Integrate//����
      //+Controler->Kd*Controler->Dis_Err;//΢��
      +Controler->Kd*Controler->Dis_Error_History[0];//΢������Դ�ڰ�����˹��ͨ�˲���
  /*******������޷�*********************/
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  /*******���������*********************/
  return Controler->Control_OutPut;
}


float PID_Control_Err_LPF(PID_Controler *Controler)
{
  float controller_dt=0;
  Test_Period(&Controler->PID_Controller_Dt);
  controller_dt=Controler->PID_Controller_Dt.Time_Delta/1000.0000f;;
  if(controller_dt<0.001f) return 0;
  /*******ƫ�����*********************/
  Controler->Last_Err=Controler->Err;//�����ϴ�ƫ��
  Controler->Err=Controler->Expect-Controler->FeedBack;//������ȥ�����õ�ƫ��
  Controler->Dis_Err=Controler->Err-Controler->Last_Err;//ԭʼ΢��
  
  Controler->Last_Err_LPF=Controler->Err_LPF;
  Controler->Err_LPF=Control_Device_LPF(Controler->Err,
                                        &Controler->Control_Device_LPF_Buffer,
                                        &Control_Device_Err_LPF_Parameter);//������˹��ͨ��õ���΢����,20hz
  
  Controler->Dis_Err_LPF=Controler->Err_LPF-Controler->Last_Err_LPF;//ƫ�����ͨ���΢����
  
  if(Controler->Err_Limit_Flag==1)//ƫ���޷��ȱ�־λ
  {
    if(Controler->Err_LPF>=Controler->Err_Max)   Controler->Err_LPF= Controler->Err_Max;
    if(Controler->Err_LPF<=-Controler->Err_Max)  Controler->Err_LPF=-Controler->Err_Max;
  }
  /*******���ּ���*********************/
  if(Controler->Integrate_Separation_Flag==1)//���ַ����־λ
  {
    if(ABS(Controler->Err_LPF)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki
        *Controler->Ki
          *Controler->Err_LPF*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki
      *Controler->Ki
        *Controler->Err_LPF*controller_dt;
  }
  /*******�����޷�*********************/
  if(Controler->Integrate_Limit_Flag==1)//�������Ʒ��ȱ�־
  {
    if(Controler->Integrate>=Controler->Integrate_Max)
      Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)
      Controler->Integrate=-Controler->Integrate_Max ;
  }
  /*******���������*********************/
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//���ֵ����
  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err_LPF//����
    +Controler->Integrate//����
      +Controler->Kd*Controler->Dis_Err_LPF;//�Ѷ�ƫ���ͨ���˴����ٶ�΢�������ͨ
  /*******������޷�*********************/
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  /*******���������*********************/
  return Controler->Control_OutPut;
}


float PID_Control_SDK_Err_LPF(PID_Controler *Controler,uint8_t Differential_Enable_Flag)
{
  float controller_dt=0;
  Test_Period(&Controler->PID_Controller_Dt);
  controller_dt=Controler->PID_Controller_Dt.Time_Delta/1000.0f;
  if(controller_dt<0.001f) return 0;
  /*******ƫ�����*********************/
  Controler->Last_Err=Controler->Err;//�����ϴ�ƫ��
  Controler->Err=Controler->Expect-Controler->FeedBack;//������ȥ�����õ�ƫ��
  
	Controler->Dis_Err=Controler->Err-Controler->Last_Err;//ԭʼ΢��
  
  Controler->Last_Err_LPF=Controler->Err_LPF;
  Controler->Err_LPF=Control_Device_LPF(Controler->Err,
                                        &Controler->Control_Device_LPF_Buffer,
                                        &Control_Device_SDK_Err_LPF_Parameter);//������˹��ͨ��õ���΢����,20hz
  if(Differential_Enable_Flag==0)
	{
		Controler->Control_Device_LPF_Buffer.Input_Butter[0]=0;
		Controler->Control_Device_LPF_Buffer.Input_Butter[1]=0;
		Controler->Control_Device_LPF_Buffer.Input_Butter[2]=0;
		Controler->Control_Device_LPF_Buffer.Output_Butter[0]=0;
		Controler->Control_Device_LPF_Buffer.Output_Butter[1]=0;
		Controler->Control_Device_LPF_Buffer.Output_Butter[2]=0;
		Controler->Last_Err_LPF=0;
		Controler->Err_LPF=0;
	}  
  Controler->Dis_Err_LPF=Controler->Err_LPF-Controler->Last_Err_LPF;//ƫ�����ͨ���΢����

	
  if(Controler->Err_Limit_Flag==1)//ƫ���޷��ȱ�־λ
  {
    if(Controler->Err_LPF>=Controler->Err_Max)   Controler->Err_LPF= Controler->Err_Max;
    if(Controler->Err_LPF<=-Controler->Err_Max)  Controler->Err_LPF=-Controler->Err_Max;
  }
  /*******���ּ���*********************/
  if(Controler->Integrate_Separation_Flag==1)//���ַ����־λ
  {
    if(ABS(Controler->Err_LPF)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki
        *Controler->Ki
          *Controler->Err_LPF*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki
      *Controler->Ki
        *Controler->Err_LPF*controller_dt;
  }
  /*******�����޷�*********************/
  if(Controler->Integrate_Limit_Flag==1)//�������Ʒ��ȱ�־
  {
    if(Controler->Integrate>=Controler->Integrate_Max)
      Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)
      Controler->Integrate=-Controler->Integrate_Max ;
  }
  /*******���������*********************/
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//���ֵ����

  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err_LPF//����
														+Controler->Integrate//����	
		+													+Controler->Kd*Controler->Dis_Err_LPF;//�Ѷ�ƫ���ͨ���˴����ٶ�΢�������ͨ
  /*******������޷�*********************/
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  /*******���������*********************/
  return Controler->Control_OutPut;
}

void PID_LPF_Reset(PID_Controler *Controler,Controler_Label Label)
{
  Controler->Control_Device_LPF_Buffer.Input_Butter[0]=0;
  Controler->Control_Device_LPF_Buffer.Input_Butter[1]=0;
  Controler->Control_Device_LPF_Buffer.Input_Butter[2]=0;
  Controler->Control_Device_LPF_Buffer.Output_Butter[0]=0;
  Controler->Control_Device_LPF_Buffer.Output_Butter[1]=0;
  Controler->Control_Device_LPF_Buffer.Output_Butter[2]=0;
  Controler->Last_Err_LPF=0;
  Controler->Err_LPF=0;
}


float Differential_Forward_PID_Control_Div_LPF(PID_Controler *Controler)//΢������PID������
{
  int16  i=0;
  float tempa,tempb,tempc,max,min;//���ڷ������˲�
  float controller_dt=0;
  Test_Period(&Controler->PID_Controller_Dt);
  controller_dt=Controler->PID_Controller_Dt.Time_Delta/1000.0000f;;
  if(controller_dt<0.001f) return 0;
  /*******ƫ�����*********************/
  Controler->Last_Err=Controler->Err;//�����ϴ�ƫ��
  Controler->Err=Controler->Expect-Controler->FeedBack;//������ȥ�����õ�ƫ��
  Controler->Dis_Err=-(Controler->FeedBack-Controler->Last_FeedBack);//ֻ�Է����ź�΢��
  Controler->Last_FeedBack=Controler->FeedBack;//��¼�ϴη���ֵ
  /******************************************/
  //��ֵ�˲�����֤�õ����ݲ����䣬����������Ծʱ��΢������쳣
  tempa=Controler->Pre_Last_Dis_Err_LPF;
  tempb=Controler->Last_Dis_Err_LPF;
  tempc=Controler->Dis_Err;
  max = tempa > tempb ? tempa:tempb;
  max = max > tempc ? max:tempc;
  min = tempa < tempb ? tempa:tempb;
  min = min < tempc ? min:tempc;
  if(tempa > min && tempa < max)    Controler->Dis_Err = tempa;
  if(tempb > min  && tempb < max )  Controler->Dis_Err = tempb;
  if(tempc > min  &&  tempc < max)  Controler->Dis_Err = tempc;
  Controler->Pre_Last_Dis_Err_LPF = Controler->Last_Dis_Err_LPF;
  Controler->Last_Dis_Err_LPF = Controler->Dis_Err;
  /*****************************************/
  
  for(i=4;i>0;i--)//���ֵ�ͨ��΢�����
  {
    Controler->Dis_Error_History[i]=Controler->Dis_Error_History[i-1];
  }
  Controler->Dis_Error_History[0]=Control_Device_LPF(Controler->Dis_Err,
                                                     &Controler->Control_Device_LPF_Buffer,
                                                     &Control_Device_Div_LPF_Parameter);//������˹��ͨ��õ���΢����,20hz
  
  if(Controler->Err_Limit_Flag==1)//ƫ���޷��ȱ�־λ
  {
    if(Controler->Err>=Controler->Err_Max)   Controler->Err= Controler->Err_Max;
    if(Controler->Err<=-Controler->Err_Max)  Controler->Err=-Controler->Err_Max;
  }
  /*******���ּ���*********************/
  if(Controler->Integrate_Separation_Flag==1)//���ַ����־λ
  {
    if(ABS(Controler->Err)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  /*******�����޷�*********************/
  if(Controler->Integrate_Limit_Flag==1)//�������Ʒ��ȱ�־
  {
    if(Controler->Integrate>=Controler->Integrate_Max)
      Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)
      Controler->Integrate=-Controler->Integrate_Max ;
  }
  /*******���������*********************/
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//���ֵ����
  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err//����
    +Controler->Integrate//����
      //+Controler->Kd*Controler->Dis_Err;//΢��
      +Controler->Kd*Controler->Dis_Error_History[0];//΢������Դ�ڰ�����˹��ͨ�˲���
  /*******������޷�*********************/
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  /*******���������*********************/
  return Controler->Control_OutPut;
}


//���ٶȻ�PID(΢�ֲ�����̬����)
float PID_Control_Div_LPF_For_Gyro(PID_Controler *Controler)
{
  int16  i=0;
  float tempa,tempb,tempc,max,min;//���ڷ������˲�
  float controller_dt=0;
  Test_Period(&Controler->PID_Controller_Dt);
  controller_dt=Controler->PID_Controller_Dt.Time_Delta/1000.0000f;;
  if(controller_dt<0.001f) return 0;
  /*******ƫ�����*********************/
  Controler->Pre_Last_Err=Controler->Last_Err;//���ϴ�ƫ��
  Controler->Last_Err=Controler->Err;//�����ϴ�ƫ��
  Controler->Err=Controler->Expect-Controler->FeedBack;//������ȥ�����õ�ƫ��
  Controler->Dis_Err=(Controler->Err-Controler->Pre_Last_Err);//�����һ�β�����΢��
  /******************************************/
  //��ֵ�˲�����֤�õ����ݲ����䣬����������Ծʱ��΢������쳣
  tempa=Controler->Pre_Last_Dis_Err_LPF;
  tempb=Controler->Last_Dis_Err_LPF;
  tempc=Controler->Dis_Err;
  max = tempa > tempb ? tempa:tempb;
  max = max > tempc ? max:tempc;
  min = tempa < tempb ? tempa:tempb;
  min = min < tempc ? min:tempc;
  if(tempa > min && tempa < max)    Controler->Dis_Err = tempa;
  if(tempb > min  && tempb < max )  Controler->Dis_Err = tempb;
  if(tempc > min  &&  tempc < max)  Controler->Dis_Err = tempc;
  Controler->Pre_Last_Dis_Err_LPF = Controler->Last_Dis_Err_LPF;
  Controler->Last_Dis_Err_LPF = Controler->Dis_Err;
  /*****************************************/
  for(i=4;i>0;i--)//���ֵ�ͨ��΢�����
  {
    Controler->Dis_Error_History[i]=Controler->Dis_Error_History[i-1];
  }
  Controler->Dis_Error_History[0]=Control_Device_LPF(Controler->Dis_Err,
                                                     &Controler->Control_Device_LPF_Buffer,
                                                     &Control_Device_Div_LPF_Parameter_For_Gyro);//������˹��ͨ��õ���΢����,30hz
  Controler->Dis_Error_History[0]=constrain_float(Controler->Dis_Error_History[0],-250,250);//΢�����޷�
  Controler->Adaptable_Kd=Controler->Kd*(1+Controler->Dis_Error_History[0]/250.0f);//����Ӧ΢�ֲ���
  
  if(Controler->Err_Limit_Flag==1)//ƫ���޷��ȱ�־λ
  {
    if(Controler->Err>=Controler->Err_Max)   Controler->Err= Controler->Err_Max;
    if(Controler->Err<=-Controler->Err_Max)  Controler->Err=-Controler->Err_Max;
  }
  /*******���ּ���*********************/
  if(Controler->Integrate_Separation_Flag==1)//���ַ����־λ
  {
    if(ABS(Controler->Err)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  /*******�����޷�*********************/
  if(Controler->Integrate_Limit_Flag==1)//�������Ʒ��ȱ�־
  {
    if(Controler->Integrate>=Controler->Integrate_Max)
      Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)
      Controler->Integrate=-Controler->Integrate_Max ;
  }
  /*******���������*********************/
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//���ֵ����
  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err//����
    +Controler->Integrate//����
      //+Controler->Kd*Controler->Dis_Err;//΢��
      //+Controler->Kd*Controler->Dis_Error_History[0];//΢������Դ�ڰ�����˹��ͨ�˲���
      +Controler->Adaptable_Kd*Controler->Dis_Error_History[0];//΢������Դ�ڰ�����˹��ͨ�˲���
  /*******������޷�*********************/
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  /*******���������*********************/
  return Controler->Control_OutPut;
}


void  PID_Integrate_Reset(PID_Controler *Controler)  {Controler->Integrate=0.0f;}


//���ǰ���λ���
void Take_Off_Reset(void)
{
	//�����̬����
  PID_Integrate_Reset(&Total_Controller.Roll_Gyro_Control);
  PID_Integrate_Reset(&Total_Controller.Pitch_Gyro_Control);
  PID_Integrate_Reset(&Total_Controller.Yaw_Gyro_Control);
  PID_Integrate_Reset(&Total_Controller.Pitch_Angle_Control);
  PID_Integrate_Reset(&Total_Controller.Roll_Angle_Control);
  PID_Integrate_Reset(&Total_Controller.Yaw_Angle_Control);
  
	//���λ�ÿ��Ƶ��ٶȻ�����
  PID_Integrate_Reset(&Total_Controller.Longitude_Speed_Control);
  PID_Integrate_Reset(&Total_Controller.Latitude_Speed_Control);
}


void Throttle_Control_Reset(void)
{
  PID_Integrate_Reset(&Total_Controller.High_Acce_Control);
  PID_Integrate_Reset(&Total_Controller.High_Speed_Control);
  PID_Integrate_Reset(&Total_Controller.High_Position_Control);
	
	PID_LPF_Reset(&Total_Controller.High_Acce_Control,High_Acce_Controler);
	
  Total_Controller.High_Acce_Control.Expect=0;							//4����
  Total_Controller.High_Acce_Control.FeedBack=0;						//5����ֵ
  Total_Controller.High_Acce_Control.Err=0;									//6ƫ��
  Total_Controller.High_Acce_Control.Last_Err=0;						//7�ϴ�ƫ��
  Total_Controller.High_Acce_Control.Integrate=0;						//10����ֵ
  Total_Controller.High_Acce_Control.Control_OutPut=0;			//15�����������
  Total_Controller.High_Acce_Control.Last_Control_OutPut=0;	//16�ϴο����������
}

#define PID_USE_NUM  14
Vector3f_pid PID_Parameter[PID_USE_NUM]={0};
uint8_t Sort_PID_Cnt=0;
uint8_t Sort_PID_Flag=0;
/***************************************************
������: void Save_Or_Reset_PID_Parameter(void)
˵��:	PID�����ڲ�Flash����д����
���:	��
����:	��
��ע:	��ϵ���վʹ��
ע���ߣ�����С��
****************************************************/
void Save_Or_Reset_PID_Parameter()
{
  if(Sort_PID_Flag==1)//������վ����PID����д��Flash
  {
    PID_Parameter[0].p=Total_Controller.Pitch_Gyro_Control.Kp;
    PID_Parameter[0].i=Total_Controller.Pitch_Gyro_Control.Ki;
    PID_Parameter[0].d=Total_Controller.Pitch_Gyro_Control.Kd;
    
    PID_Parameter[1].p=Total_Controller.Roll_Gyro_Control.Kp;
    PID_Parameter[1].i=Total_Controller.Roll_Gyro_Control.Ki;
    PID_Parameter[1].d=Total_Controller.Roll_Gyro_Control.Kd;
    
    PID_Parameter[2].p=Total_Controller.Yaw_Gyro_Control.Kp;
    PID_Parameter[2].i=Total_Controller.Yaw_Gyro_Control.Ki;
    PID_Parameter[2].d=Total_Controller.Yaw_Gyro_Control.Kd;
    
    PID_Parameter[3].p=Total_Controller.Pitch_Angle_Control.Kp;
    PID_Parameter[3].i=Total_Controller.Pitch_Angle_Control.Ki;
    PID_Parameter[3].d=Total_Controller.Pitch_Angle_Control.Kd;
    
    PID_Parameter[4].p=Total_Controller.Roll_Angle_Control.Kp;
    PID_Parameter[4].i=Total_Controller.Roll_Angle_Control.Ki;
    PID_Parameter[4].d=Total_Controller.Roll_Angle_Control.Kd;
    
    PID_Parameter[5].p=Total_Controller.Yaw_Angle_Control.Kp;
    PID_Parameter[5].i=Total_Controller.Yaw_Angle_Control.Ki;
    PID_Parameter[5].d=Total_Controller.Yaw_Angle_Control.Kd;
    
    PID_Parameter[6].p=Total_Controller.High_Speed_Control.Kp;
    PID_Parameter[6].i=Total_Controller.High_Speed_Control.Ki;
    PID_Parameter[6].d=Total_Controller.High_Speed_Control.Kd;
    
    PID_Parameter[7].p=Total_Controller.High_Position_Control.Kp;
    PID_Parameter[7].i=Total_Controller.High_Position_Control.Ki;
    PID_Parameter[7].d=Total_Controller.High_Position_Control.Kd;
    
    PID_Parameter[8].p=Total_Controller.Latitude_Speed_Control.Kp;
    PID_Parameter[8].i=Total_Controller.Latitude_Speed_Control.Ki;
    PID_Parameter[8].d=Total_Controller.Latitude_Speed_Control.Kd;
    
    PID_Parameter[9].p=Total_Controller.Latitude_Position_Control.Kp;
    PID_Parameter[9].i=Total_Controller.Latitude_Position_Control.Ki;
    PID_Parameter[9].d=Total_Controller.Latitude_Position_Control.Kd;
    
    PID_Parameter[10].p=Total_Controller.High_Acce_Control.Kp;
    PID_Parameter[10].i=Total_Controller.High_Acce_Control.Ki;
    PID_Parameter[10].d=Total_Controller.High_Acce_Control.Kd;
    
    PID_Parameter[11].p=Total_Controller.Optical_Position_Control.Kp;
    PID_Parameter[11].i=Total_Controller.Optical_Position_Control.Ki;
    PID_Parameter[11].d=Total_Controller.Optical_Position_Control.Kd;
    
    PID_Parameter[12].p=Total_Controller.Optical_Speed_Control.Kp;
    PID_Parameter[12].i=Total_Controller.Optical_Speed_Control.Ki;
    PID_Parameter[12].d=Total_Controller.Optical_Speed_Control.Kd;
    
    PID_Parameter[13].p=Total_Controller.SDK_Roll_Position_Control.Kp;
    PID_Parameter[13].i=Total_Controller.SDK_Roll_Position_Control.Ki;
    PID_Parameter[13].d=Total_Controller.SDK_Roll_Position_Control.Kd;
    Save_PID_Parameter();
    
    Sort_PID_Flag=0;
  }
  else if(Sort_PID_Flag==2)//����λPID��������д��Flash
  {
    Total_PID_Init();//��PID��������Ϊ����Control_Unit���������
    PID_Parameter[0].p=Total_Controller.Pitch_Gyro_Control.Kp;
    PID_Parameter[0].i=Total_Controller.Pitch_Gyro_Control.Ki;
    PID_Parameter[0].d=Total_Controller.Pitch_Gyro_Control.Kd;
    
    PID_Parameter[1].p=Total_Controller.Roll_Gyro_Control.Kp;
    PID_Parameter[1].i=Total_Controller.Roll_Gyro_Control.Ki;
    PID_Parameter[1].d=Total_Controller.Roll_Gyro_Control.Kd;
    
    PID_Parameter[2].p=Total_Controller.Yaw_Gyro_Control.Kp;
    PID_Parameter[2].i=Total_Controller.Yaw_Gyro_Control.Ki;
    PID_Parameter[2].d=Total_Controller.Yaw_Gyro_Control.Kd;
    
    PID_Parameter[3].p=Total_Controller.Pitch_Angle_Control.Kp;
    PID_Parameter[3].i=Total_Controller.Pitch_Angle_Control.Ki;
    PID_Parameter[3].d=Total_Controller.Pitch_Angle_Control.Kd;
    
    PID_Parameter[4].p=Total_Controller.Roll_Angle_Control.Kp;
    PID_Parameter[4].i=Total_Controller.Roll_Angle_Control.Ki;
    PID_Parameter[4].d=Total_Controller.Roll_Angle_Control.Kd;
    
    PID_Parameter[5].p=Total_Controller.Yaw_Angle_Control.Kp;
    PID_Parameter[5].i=Total_Controller.Yaw_Angle_Control.Ki;
    PID_Parameter[5].d=Total_Controller.Yaw_Angle_Control.Kd;
    
    PID_Parameter[6].p=Total_Controller.High_Speed_Control.Kp;
    PID_Parameter[6].i=Total_Controller.High_Speed_Control.Ki;
    PID_Parameter[6].d=Total_Controller.High_Speed_Control.Kd;
    
    PID_Parameter[7].p=Total_Controller.High_Position_Control.Kp;
    PID_Parameter[7].i=Total_Controller.High_Position_Control.Ki;
    PID_Parameter[7].d=Total_Controller.High_Position_Control.Kd;
    
    PID_Parameter[8].p=Total_Controller.Latitude_Speed_Control.Kp;
    PID_Parameter[8].i=Total_Controller.Latitude_Speed_Control.Ki;
    PID_Parameter[8].d=Total_Controller.Latitude_Speed_Control.Kd;
    
    PID_Parameter[9].p=Total_Controller.Latitude_Position_Control.Kp;
    PID_Parameter[9].i=Total_Controller.Latitude_Position_Control.Ki;
    PID_Parameter[9].d=Total_Controller.Latitude_Position_Control.Kd;
    
    PID_Parameter[10].p=Total_Controller.High_Acce_Control.Kp;
    PID_Parameter[10].i=Total_Controller.High_Acce_Control.Ki;
    PID_Parameter[10].d=Total_Controller.High_Acce_Control.Kd;
    
    PID_Parameter[11].p=Total_Controller.Optical_Position_Control.Kp;
    PID_Parameter[11].i=Total_Controller.Optical_Position_Control.Ki;
    PID_Parameter[11].d=Total_Controller.Optical_Position_Control.Kd;
    
    PID_Parameter[12].p=Total_Controller.Optical_Speed_Control.Kp;
    PID_Parameter[12].i=Total_Controller.Optical_Speed_Control.Ki;
    PID_Parameter[12].d=Total_Controller.Optical_Speed_Control.Kd;
    
    PID_Parameter[13].p=Total_Controller.SDK_Roll_Position_Control.Kp;
    PID_Parameter[13].i=Total_Controller.SDK_Roll_Position_Control.Ki;
    PID_Parameter[13].d=Total_Controller.SDK_Roll_Position_Control.Kd;
    Save_PID_Parameter();
    
    Sort_PID_Flag=0;
    
    ANO_Send_PID_Flag[0]=1;//�ظ�Ĭ�ϲ����󣬽����µ����ݷ����õ���վ
    ANO_Send_PID_Flag[1]=1;
    ANO_Send_PID_Flag[2]=1;
    ANO_Send_PID_Flag[3]=1;
    ANO_Send_PID_Flag[4]=1;
    ANO_Send_PID_Flag[5]=1;
  }
  else if(Sort_PID_Flag==3)//����λPID��������д��Flash
  {
    Total_PID_Init();//��PID��������Ϊ����Control_Unit���������
    PID_Parameter[0].p=Total_Controller.Pitch_Gyro_Control.Kp;
    PID_Parameter[0].i=Total_Controller.Pitch_Gyro_Control.Ki;
    PID_Parameter[0].d=Total_Controller.Pitch_Gyro_Control.Kd;
    
    PID_Parameter[1].p=Total_Controller.Roll_Gyro_Control.Kp;
    PID_Parameter[1].i=Total_Controller.Roll_Gyro_Control.Ki;
    PID_Parameter[1].d=Total_Controller.Roll_Gyro_Control.Kd;
    
    PID_Parameter[2].p=Total_Controller.Yaw_Gyro_Control.Kp;
    PID_Parameter[2].i=Total_Controller.Yaw_Gyro_Control.Ki;
    PID_Parameter[2].d=Total_Controller.Yaw_Gyro_Control.Kd;
    
    PID_Parameter[3].p=Total_Controller.Pitch_Angle_Control.Kp;
    PID_Parameter[3].i=Total_Controller.Pitch_Angle_Control.Ki;
    PID_Parameter[3].d=Total_Controller.Pitch_Angle_Control.Kd;
    
    PID_Parameter[4].p=Total_Controller.Roll_Angle_Control.Kp;
    PID_Parameter[4].i=Total_Controller.Roll_Angle_Control.Ki;
    PID_Parameter[4].d=Total_Controller.Roll_Angle_Control.Kd;
    
    PID_Parameter[5].p=Total_Controller.Yaw_Angle_Control.Kp;
    PID_Parameter[5].i=Total_Controller.Yaw_Angle_Control.Ki;
    PID_Parameter[5].d=Total_Controller.Yaw_Angle_Control.Kd;
    
    PID_Parameter[6].p=Total_Controller.High_Speed_Control.Kp;
    PID_Parameter[6].i=Total_Controller.High_Speed_Control.Ki;
    PID_Parameter[6].d=Total_Controller.High_Speed_Control.Kd;
    
    PID_Parameter[7].p=Total_Controller.High_Position_Control.Kp;
    PID_Parameter[7].i=Total_Controller.High_Position_Control.Ki;
    PID_Parameter[7].d=Total_Controller.High_Position_Control.Kd;
    
    PID_Parameter[8].p=Total_Controller.Latitude_Speed_Control.Kp;
    PID_Parameter[8].i=Total_Controller.Latitude_Speed_Control.Ki;
    PID_Parameter[8].d=Total_Controller.Latitude_Speed_Control.Kd;
    
    PID_Parameter[9].p=Total_Controller.Latitude_Position_Control.Kp;
    PID_Parameter[9].i=Total_Controller.Latitude_Position_Control.Ki;
    PID_Parameter[9].d=Total_Controller.Latitude_Position_Control.Kd;
    
    PID_Parameter[10].p=Total_Controller.High_Acce_Control.Kp;
    PID_Parameter[10].i=Total_Controller.High_Acce_Control.Ki;
    PID_Parameter[10].d=Total_Controller.High_Acce_Control.Kd;
    
    PID_Parameter[11].p=Total_Controller.Optical_Position_Control.Kp;
    PID_Parameter[11].i=Total_Controller.Optical_Position_Control.Ki;
    PID_Parameter[11].d=Total_Controller.Optical_Position_Control.Kd;
    
    PID_Parameter[12].p=Total_Controller.Optical_Speed_Control.Kp;
    PID_Parameter[12].i=Total_Controller.Optical_Speed_Control.Ki;
    PID_Parameter[12].d=Total_Controller.Optical_Speed_Control.Kd;
    
    PID_Parameter[13].p=Total_Controller.SDK_Roll_Position_Control.Kp;
    PID_Parameter[13].i=Total_Controller.SDK_Roll_Position_Control.Ki;
    PID_Parameter[13].d=Total_Controller.SDK_Roll_Position_Control.Kd;
    Save_PID_Parameter();
    
    Sort_PID_Flag=0;
    
    ANO_Send_PID_Flag[0]=1;//�ظ�Ĭ�ϲ����󣬽����µ����ݷ����õ���վ
    ANO_Send_PID_Flag[1]=1;
    ANO_Send_PID_Flag[2]=1;
    ANO_Send_PID_Flag[3]=1;
    ANO_Send_PID_Flag[4]=1;
    ANO_Send_PID_Flag[5]=1;
  }
}


typedef struct
{
  uint8_t No_0xFF[PID_USE_NUM];
}PID_Parameter_Flag;
PID_Parameter_Flag PID_Parameter_Read_Flag;
void PID_Paramter_Init_With_Flash()
{
  uint16_t i=0;
  Total_PID_Init();
  for(i=0;i<PID_USE_NUM;i++)
  {
    ReadFlashParameterThree(PID1_PARAMETER_KP+3*i,
                            &PID_Parameter[i].p,
                            &PID_Parameter[i].i,
                            &PID_Parameter[i].d);
    if(isnan(PID_Parameter[i].p)==0
       &&isnan(PID_Parameter[i].i)==0
         &&isnan(PID_Parameter[i].d)==0)
      PID_Parameter_Read_Flag.No_0xFF[i]=0x01;
  }
  if(PID_Parameter_Read_Flag.No_0xFF[0]!=0x00
     &&PID_Parameter_Read_Flag.No_0xFF[1]!=0x00
       &&PID_Parameter_Read_Flag.No_0xFF[2]!=0x00
         &&PID_Parameter_Read_Flag.No_0xFF[3]!=0x00
           &&PID_Parameter_Read_Flag.No_0xFF[4]!=0x00
             &&PID_Parameter_Read_Flag.No_0xFF[5]!=0x00
               &&PID_Parameter_Read_Flag.No_0xFF[6]!=0x00
                 &&PID_Parameter_Read_Flag.No_0xFF[7]!=0x00
                   &&PID_Parameter_Read_Flag.No_0xFF[8]!=0x00
                     &&PID_Parameter_Read_Flag.No_0xFF[9]!=0x00
                       &&PID_Parameter_Read_Flag.No_0xFF[10]!=0x00)//Flash����������������PID����ֵ
  {
    Total_Controller.Pitch_Gyro_Control.Kp=PID_Parameter[0].p;
    Total_Controller.Pitch_Gyro_Control.Ki=PID_Parameter[0].i;
    Total_Controller.Pitch_Gyro_Control.Kd=PID_Parameter[0].d;
    
    Total_Controller.Roll_Gyro_Control.Kp=PID_Parameter[1].p;
    Total_Controller.Roll_Gyro_Control.Ki=PID_Parameter[1].i;
    Total_Controller.Roll_Gyro_Control.Kd=PID_Parameter[1].d;
    
    Total_Controller.Yaw_Gyro_Control.Kp=PID_Parameter[2].p;
    Total_Controller.Yaw_Gyro_Control.Ki=PID_Parameter[2].i;
    Total_Controller.Yaw_Gyro_Control.Kd=PID_Parameter[2].d;
    
    Total_Controller.Pitch_Angle_Control.Kp=PID_Parameter[3].p;
    Total_Controller.Pitch_Angle_Control.Ki=PID_Parameter[3].i;
    Total_Controller.Pitch_Angle_Control.Kd=PID_Parameter[3].d;
    
    Total_Controller.Roll_Angle_Control.Kp=PID_Parameter[4].p;
    Total_Controller.Roll_Angle_Control.Ki=PID_Parameter[4].i;
    Total_Controller.Roll_Angle_Control.Kd=PID_Parameter[4].d;
    
    Total_Controller.Yaw_Angle_Control.Kp=PID_Parameter[5].p;
    Total_Controller.Yaw_Angle_Control.Ki=PID_Parameter[5].i;
    Total_Controller.Yaw_Angle_Control.Kd=PID_Parameter[5].d;
    
    Total_Controller.High_Speed_Control.Kp=PID_Parameter[6].p;
    Total_Controller.High_Speed_Control.Ki=PID_Parameter[6].i;
    Total_Controller.High_Speed_Control.Kd=PID_Parameter[6].d;
    
    Total_Controller.High_Position_Control.Kp=PID_Parameter[7].p;
    Total_Controller.High_Position_Control.Ki=PID_Parameter[7].i;
    Total_Controller.High_Position_Control.Kd=PID_Parameter[7].d;
    
    Total_Controller.Latitude_Speed_Control.Kp=PID_Parameter[8].p;
    Total_Controller.Latitude_Speed_Control.Ki=PID_Parameter[8].i;
    Total_Controller.Latitude_Speed_Control.Kd=PID_Parameter[8].d;
    
    Total_Controller.Latitude_Position_Control.Kp=PID_Parameter[9].p;
    Total_Controller.Latitude_Position_Control.Ki=PID_Parameter[9].i;
    Total_Controller.Latitude_Position_Control.Kd=PID_Parameter[9].d;
    /***********************λ�ÿ��ƣ�λ�á��ٶȲ�������һ��PID����**********************************************************/
    Total_Controller.Longitude_Speed_Control.Kp=PID_Parameter[8].p;
    Total_Controller.Longitude_Speed_Control.Ki=PID_Parameter[8].i;
    Total_Controller.Longitude_Speed_Control.Kd=PID_Parameter[8].d;
    
    Total_Controller.Longitude_Position_Control.Kp=PID_Parameter[9].p;
    Total_Controller.Longitude_Position_Control.Ki=PID_Parameter[9].i;
    Total_Controller.Longitude_Position_Control.Kd=PID_Parameter[9].d;
    
    Total_Controller.High_Acce_Control.Kp=PID_Parameter[10].p;
    Total_Controller.High_Acce_Control.Ki=PID_Parameter[10].i;
    Total_Controller.High_Acce_Control.Kd=PID_Parameter[10].d;
    
    Total_Controller.Optical_Position_Control.Kp=PID_Parameter[11].p;
    Total_Controller.Optical_Position_Control.Ki=PID_Parameter[11].i;
    Total_Controller.Optical_Position_Control.Kd=PID_Parameter[11].d;
    
    Total_Controller.Optical_Speed_Control.Kp=PID_Parameter[12].p;
    Total_Controller.Optical_Speed_Control.Ki=PID_Parameter[12].i;
    Total_Controller.Optical_Speed_Control.Kd=PID_Parameter[12].d;
    
    Total_Controller.SDK_Roll_Position_Control.Kp=PID_Parameter[13].p;
    Total_Controller.SDK_Roll_Position_Control.Ki=PID_Parameter[13].i;
    Total_Controller.SDK_Roll_Position_Control.Kd=PID_Parameter[13].d;
    
    Total_Controller.SDK_Pitch_Position_Control.Kp=PID_Parameter[13].p;
    Total_Controller.SDK_Pitch_Position_Control.Ki=PID_Parameter[13].i;
    Total_Controller.SDK_Pitch_Position_Control.Kd=PID_Parameter[13].d;
  }
}

void Save_PID_Parameter(void)
{
  int16_t i=0;
  for(i=0;i<PID_USE_NUM;i=i+1)
  {
    WriteFlashParameter_Three(PID1_PARAMETER_KP+i*3,
                              PID_Parameter[i].p,
                              PID_Parameter[i].i,
                              PID_Parameter[i].d,
                              &Table_Parameter);
  }
}

/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/


