
/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/
/*----------------------------------------------------------------------------------------------------------------------/
*               ������ֻ��������ѧϰʹ�ã���Ȩ����Ȩ���������ƴ��Ŷӣ������ƴ��Ŷӽ��ɿس���Դ���ṩ�������ߣ�
*               ������ҪΪ�����ƴ��Ŷ��ṩ������δ��������ɣ����ý�Դ�����ṩ�����ˣ����ý�Դ����ŵ����Ϲ�����������أ� 
*               �������Դ�����Ĳ�����緢��������Ϊ�������ƴ��Ŷӽ���֮�Է��ɽ��������
-----------------------------------------------------------------------------------------------------------------------/
*               ������Ϣ���ܶ���ֹ��ǰ�����������˳���������
*               ��Դ���ף���ѧ����ϧ��ף������Ϯ�����׳ɹ�������
*               ѧϰ�����ߣ��������Ƽ���DJI��ZEROTECH��XAG��AEE��GDU��AUTEL��EWATT��HIGH GREAT�ȹ�˾��ҵ
*               ��ְ�����뷢�ͣ�15671678205@163.com���豸ע��ְ����λ����λ��������
*               �����ƴ���Դ�ɿ�QQȺ��540707961
*               CSDN���ͣ�http://blog.csdn.net/u011992534
*               �ſ�ID��NamelessCotrun����С��
*               Bվ��ѧ��Ƶ��https://space.bilibili.com/67803559/#/video
*               �ͻ�ʹ���ĵá��Ľ������������http://www.openedv.com/forum.php?mod=viewthread&tid=234214&extra=page=1
*               �Ա����̣�https://shop348646912.taobao.com/?spm=2013.1.1000126.2.5ce78a88ht1sO2
*               �ٶ�����:�����ƴ���Դ�ɿ�
*               ��˾����:www.nameless.tech
*               �޸�����:2019/4/12
*               �汾����Ӯ�ߡ���CarryPilot_V1.0
*               ��Ȩ���У�����ؾ���
*               Copyright(C) 2017-2025 �人�������¿Ƽ����޹�˾ 
*               All rights reserved
*               ��Ҫ��ʾ��
*               �����Ա�����ת�ֵķɿء��������ѡ�����ѧ�ܵĶ����Խ��ۺ�Ⱥѧϰ������
*               ����ֱ�����������������������ϣ��������´�����������Ȩ�����˲��ý�
*               ���ϴ��봫���Ϲ��������أ�������ı��ΪĿ���������ϴ��룬�����д˲�
*               ���ߣ���˾����ǰ��֪����1���ڼ�ʱ�����������ѧУ����λ����������
*               ������ַ��Ϣ�ᱻ�����ڹ�˾�������ٷ�΢�Ź���ƽ̨���ٷ��������͡�֪��
*               ר���Լ��Ա�������ҳ���Թ�ʾ���棬����������Ϊ�����Ϊ�����۵㣬Ӱ��
*               ��ѧ���ҹ���������������ܿ�ͺ������˻����������������ء�
*               �����Ϊ����˾����ش���ʧ�ߣ����Է���;���������л���ĺ�����лл������
----------------------------------------------------------------------------------------------------------------------*/
#include "Headfile.h"
#include "Earth_Declination.h"
#include "SINS.h"

/***********************************************************
@��������Vector_From_BodyFrame2EarthFrame
@��ڲ�����Vector3f *bf,Vector3f *ef
@���ڲ�������
��������������ϵ�򵼺�ϵת��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Vector_From_BodyFrame2EarthFrame(Vector3f *bf,Vector3f *ef)
{
  ef->x=rMat[0][0]*bf->x+rMat[0][1]*bf->y+rMat[0][2]*bf->z;
  ef->y=rMat[1][0]*bf->x+rMat[1][1]*bf->y+rMat[1][2]*bf->z;
  ef->z=rMat[2][0]*bf->x+rMat[2][1]*bf->y+rMat[2][2]*bf->z;
}

/***********************************************************
@��������Vector_From_EarthFrame2BodyFrame
@��ڲ�����Vector3f *ef,Vector3f *bf
@���ڲ�������
��������������ϵ������ϵת��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Vector_From_EarthFrame2BodyFrame(Vector3f *ef,Vector3f *bf)
{
  bf->x=rMat[0][0]*ef->x+rMat[1][0]*ef->y+rMat[2][0]*ef->z;
  bf->y=rMat[0][1]*ef->x+rMat[1][1]*ef->y+rMat[2][1]*ef->z;
  bf->z=rMat[0][2]*ef->x+rMat[1][2]*ef->y+rMat[2][2]*ef->z;
}

SINS NamelessQuad;
SINS Origion_NamelessQuad;
Butter_BufferData accel_feedback_filter_buf[3];
float Acceleration_Feedback[3];
Vector2f SINS_Accel_Body={0,0};
/***********************************************************
@��������SINS_Prepare
@��ڲ�������
@���ڲ�������
�����������ߵ����ٶ�׼��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void  SINS_Prepare(void)
{
  Vector2f SINS_Accel_Earth={0,0};
  Vector3f Body_Frame,Earth_Frame;
  /*Z-Y-Xŷ����ת�������Ҿ���
  //Pitch Roll  Yaw �ֱ��Ӧ�� �� ��  
  X����ת����
  R������
  {1      0        0    }
  {0      cos��    sin��}
  {0    -sin��    cos�� }
  
  Y����ת����
  R���ȣ�
  {cos��     0        -sin��}
  {0         1        0     }
  {sin��     0        cos��}
  
  Z����ת����
  R���ȣ�
  {cos��      sin��       0}
  {-sin��     cos��       0}
  {0          0           1 }
  
  ��Z-Y-X˳����:
  ��������ϵ����������ϵ����ת����R(b2n)
  R(b2n) =R(��)^T*R(��)^T*R(��)^T
  
  R=
  {cos��*cos��     -cos��*sin��+sin��*sin��*cos��        sin��*sin��+cos��*sin��*cos��}
  {cos��*sin��     cos��*cos�� +sin��*sin��*sin��       -cos��*sin��+cos��*sin��*sin��}
  {-sin��          cos��sin ��                          cos��cos��                   }
  */
  Body_Frame.x=ins_accel_filter.x;
  Body_Frame.y=ins_accel_filter.y;
  Body_Frame.z=ins_accel_filter.z;
  Vector_From_BodyFrame2EarthFrame(&Body_Frame,&Earth_Frame);
  Origion_NamelessQuad.Acceleration[_YAW]=Earth_Frame.z;
  Origion_NamelessQuad.Acceleration[_PITCH]=Earth_Frame.x;
  Origion_NamelessQuad.Acceleration[_ROLL]=Earth_Frame.y;
  
  
  Origion_NamelessQuad.Acceleration[_YAW]*=AcceGravity/AcceMax;
  Origion_NamelessQuad.Acceleration[_YAW]-=AcceGravity;//��ȥ�������ٶ�
  Origion_NamelessQuad.Acceleration[_YAW]*=100;//���ٶ�cm/s^2
  
  Origion_NamelessQuad.Acceleration[_PITCH]*=AcceGravity/AcceMax;
  Origion_NamelessQuad.Acceleration[_PITCH]*=100;//���ٶ�cm/s^2
  
  Origion_NamelessQuad.Acceleration[_ROLL]*=AcceGravity/AcceMax;
  Origion_NamelessQuad.Acceleration[_ROLL]*=100;//���ٶ�cm/s^2
  
  /******************************************************************************/
  //�����˻��ڵ�������ϵ�µ���������������������˶����ٶ���ת����ǰ������˶����ٶ�:��ͷ(����)+���
  
  SINS_Accel_Earth.x=Origion_NamelessQuad.Acceleration[_PITCH];//�ص�������ϵ�����������˶����ٶ�,��λΪCM
  SINS_Accel_Earth.y=Origion_NamelessQuad.Acceleration[_ROLL];//�ص�������ϵ�����������˶����ٶ�,��λΪCM
  
  SINS_Accel_Body.x=SINS_Accel_Earth.x*Cos_Yaw+SINS_Accel_Earth.y*Sin_Yaw;  //��������˶����ٶ�  X������
  SINS_Accel_Body.y=-SINS_Accel_Earth.x*Sin_Yaw+SINS_Accel_Earth.y*Cos_Yaw; //��ͷ�����˶����ٶ�  Y������
  
  
  Body_Frame.x=LPButterworth(accel.x,&accel_feedback_filter_buf[0],&Butter_80HZ_Parameter_Acce);
  Body_Frame.y=LPButterworth(accel.y,&accel_feedback_filter_buf[1],&Butter_80HZ_Parameter_Acce);;
  Body_Frame.z=LPButterworth(accel.z,&accel_feedback_filter_buf[2],&Butter_80HZ_Parameter_Acce);;
  Vector_From_BodyFrame2EarthFrame(&Body_Frame,&Earth_Frame);
  Acceleration_Feedback[_YAW]=Earth_Frame.z;
  Acceleration_Feedback[_PITCH]=Earth_Frame.x;
  Acceleration_Feedback[_ROLL]=Earth_Frame.y;
  Acceleration_Feedback[_YAW]*=AcceGravity/AcceMax;
  Acceleration_Feedback[_YAW]-=AcceGravity;//��ȥ�������ٶ�
  Acceleration_Feedback[_YAW]*=100;//���ٶ�cm/s^2
  Acceleration_Feedback[_PITCH]*=AcceGravity/AcceMax;
  Acceleration_Feedback[_PITCH]*=100;//���ٶ�cm/s^2
  Acceleration_Feedback[_ROLL]*=AcceGravity/AcceMax;
  Acceleration_Feedback[_ROLL]*=100;//���ٶ�cm/s^2
}





#define Ra  0.5f//0.1   0.5  30
#define KALMAN_DT 0.005f
float R[2]={0.5f*Ra*KALMAN_DT*KALMAN_DT,Ra*KALMAN_DT};//{1.0e-6f,5.0e-4f};{5.0e-6f,6.0e-4f}
float Q=500;//30  100   150  300
#define  Observation_Err_Max   500//cm
float Acce_Bias_Gain[3]={
  0.0005,//0.001
  0.0005,//0.001
  0.0005,//0.001
};
float Pre_conv[4]=
{
  0.18,0.1,//0.001,0,
  0.1,0.18//0,0.001
};//��һ��Э����
/***********************************************************
@��������KalmanFilter
@��ڲ�����float Observation,//λ�ù۲���
					 uint16 Pos_Delay_Cnt,//�۲⴫������ʱ��
					 SINS *Ins_Kf,//�ߵ��ṹ��
					 float System_drive,//ϵͳԭʼ���������ߵ����ٶ�
					 float *R,
					 float Q,
					 float dt,
					 uint16 N,
					 uint8_t update_flag
@���ڲ�������
�����������߶ȷ���ߵ��������˲�
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
float observation_div=0;
float last_observation=0;
float observation_acc;
uint8_t observation_reset_flag=0;
void  KalmanFilter(float Observation,//λ�ù۲���
                   uint16 Pos_Delay_Cnt,//�۲⴫������ʱ��
                   SINS *Ins_Kf,//�ߵ��ṹ��
                   float System_drive,//ϵͳԭʼ���������ߵ����ٶ�
                   float *R,
                   float Q,
                   float dt,
                   uint16 N,
                   uint8_t update_flag)
{
  uint16 Cnt=0;
  static uint16 Speed_Sync_Cnt=0;
	float Q_Temp=0;
  float Temp_conv[4]={0};//����Э����
  float Conv_Z=0,Z_Cor=0;
  float k[2]={0};//�������
  float Ctemp=0;

	if(Controler_High_Mode==2)
	{
	  	if((ABS(observation_div)<=30*ABS(Ins_Kf->Vel_History[N][Pos_Delay_Cnt]))
						&&ABS(observation_acc)<=5000//5000cm/s^2Լ����5g���ٶȣ���������װ�ﲻ��
							&&ABS(observation_div)<=1500)//15m/s			
			{
				last_observation=Observation;				
			}
			else 
			{
				Observation=last_observation;
				observation_reset_flag=1;
			}
	}
  else if(Controler_High_Mode==1)
	{
	  	if((ABS(observation_div)<=30*ABS(Ins_Kf->Vel_History[N][Pos_Delay_Cnt])))
					 last_observation=Observation;
			else Observation=last_observation;
	}
	else Observation=last_observation;
	
	Q_Temp=constrain_float(Q,0,1500);
  //����״̬
  Ins_Kf->Acceleration[N]=System_drive;
  Ins_Kf->Acceleration[N]=Ins_Kf->Acce_Bias[N]+Ins_Kf->Acceleration[N];
  Ins_Kf->Position[N] +=Ins_Kf->Speed[N]*dt+(Ins_Kf->Acceleration[N]*dt*dt)/2.0f;
  Ins_Kf->Speed[N]+=Ins_Kf->Acceleration[N]*dt;
  if(update_flag==1)
  {
    //����Э����
    Ctemp=Pre_conv[1]+Pre_conv[3]*dt;
    Temp_conv[0]=Pre_conv[0]+Pre_conv[2]*dt+Ctemp*dt+R[0];
    Temp_conv[1]=Ctemp;
    Temp_conv[2]=Pre_conv[2]+Pre_conv[3]*dt;;
    Temp_conv[3]=Pre_conv[3]+R[1];
    //���㿨��������
    Conv_Z=Temp_conv[0]+Q_Temp;
    k[0]=Temp_conv[0]/Conv_Z;
    k[1]=Temp_conv[2]/Conv_Z;
    //�ں��������
    //Z_Cor=Observation-Ins_Kf->Pos_History[N][Pos_Delay_Cnt];
    //Z_Cor=Positional-*Position;
		Z_Cor=constrain_float(Observation-Ins_Kf->Pos_History[N][Pos_Delay_Cnt],-Observation_Err_Max,Observation_Err_Max);	
		
		if(observation_reset_flag==1&&ABS(Z_Cor)>=100)  
    {
			Z_Cor=0;
			observation_reset_flag=0;
		}
		
    Ins_Kf->Position[N] +=k[0]*Z_Cor;
    Ins_Kf->Speed[N] +=k[1]*Z_Cor;
    Ins_Kf->Acce_Bias[N]+=Acce_Bias_Gain[_YAW]*Z_Cor;
		Ins_Kf->Acce_Bias[N]=constrain_float(Ins_Kf->Acce_Bias[N],-50,50);
		
    //����״̬Э�������
    Pre_conv[0]=(1-k[0])*Temp_conv[0];
    Pre_conv[1]=(1-k[0])*Temp_conv[1];
    Pre_conv[2]=Temp_conv[2]-k[1]*Temp_conv[0];
    Pre_conv[3]=Temp_conv[3]-k[1]*Temp_conv[1]; 
  }
  
  for(Cnt=Num-1;Cnt>0;Cnt--)//2.5ms����һ��
  {
    Ins_Kf->Pos_History[N][Cnt]=Ins_Kf->Pos_History[N][Cnt-1];
  }
  Ins_Kf->Pos_History[N][0]=Ins_Kf->Position[N];
  
  Speed_Sync_Cnt++;
  if(Speed_Sync_Cnt>=1)//100ms����һ��
  {
    for(Cnt=Num-1;Cnt>0;Cnt--)
    {
      Ins_Kf->Vel_History[N][Cnt]=Ins_Kf->Vel_History[N][Cnt-1];
    }
    Speed_Sync_Cnt=0;
  }
  Ins_Kf->Vel_History[N][0]=Ins_Kf->Speed[N];
}



#define SPL06_Sync_Cnt  25//30
#define HCSR04_Sync_Cnt 20//0
uint16 High_Delay_Cnt=0;
uint8_t Observation_Update_Flag=0;
float Observation_Altitude=0;
/***********************************************************
@��������Observation_Tradeoff
@��ڲ�����uint8_t HC_SR04_Enable
@���ڲ�������
�����������߶ȷ���۲⴫���������л�
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Observation_Tradeoff(uint8_t HC_SR04_Enable)
{
  static uint8_t observation_flag=1,last_observation_flag=1;
  uint16 Cnt=0;
  last_observation_flag=observation_flag;
  if(HC_SR04_Enable==0)//�޳���������
  {
    Observation_Altitude=WP_Sensor.baro_altitude;//�߶ȹ۲���
    High_Delay_Cnt=SPL06_Sync_Cnt;
    Observation_Update_Flag=1;
    observation_flag=1;
		observation_div=WP_Sensor.baro_altitude_div;
		observation_acc=WP_Sensor.baro_altitude_acc;
  }
  else//�г���������
  {    
    if(Sensor_Flag.Hcsr04_Health==1)
    {
      Observation_Altitude=US_Distance;
      High_Delay_Cnt=HCSR04_Sync_Cnt;
      observation_flag=2;
      if(US_100_Update_Flag==1)
      {
        Observation_Update_Flag=US_100_Update_Flag;
        US_100_Update_Flag=0;
      }
			observation_div=US_Distance_Div;
			observation_acc=US_Distance_Acc;
    }
    else
    {
      Observation_Altitude=WP_Sensor.baro_altitude;//�߶ȹ۲���
      High_Delay_Cnt=SPL06_Sync_Cnt;
      Observation_Update_Flag=1;
      observation_flag=1;
			observation_div=WP_Sensor.baro_altitude_div;
			observation_acc=WP_Sensor.baro_altitude_acc;
    } 
    
    if(observation_flag==2&&last_observation_flag==1)//��ѹ���г�����
    {
      NamelessQuad.Position[_YAW]=US_Distance;
      for(Cnt=Num-1;Cnt>0;Cnt--){NamelessQuad.Pos_History[_YAW][Cnt]=US_Distance;}
      NamelessQuad.Pos_History[_YAW][0]=US_Distance;
      Total_Controller.High_Position_Control.Expect=US_Distance;//���ߵ��߶�����Ϊ�����߶ȣ����ҽ�����һ��
      
      Origion_NamelessQuad.Position[_YAW]=US_Distance;
      Origion_NamelessQuad.Speed[_YAW]=0;
    }
    else if(observation_flag==1&&last_observation_flag==2)//����������ѹ��
    {
      NamelessQuad.Position[_YAW]=Observation_Altitude;
      for(Cnt=Num-1;Cnt>0;Cnt--){NamelessQuad.Pos_History[_YAW][Cnt]=Observation_Altitude;}
      NamelessQuad.Pos_History[_YAW][0]=Observation_Altitude;
      Total_Controller.High_Position_Control.Expect=Observation_Altitude;//���ߵ��߶�����Ϊ�����߶ȣ����ҽ�����һ��
      Origion_NamelessQuad.Position[_YAW]=Observation_Altitude;
      Origion_NamelessQuad.Speed[_YAW]=0;
    }
  }
}

Testime SINS_High_Delta;
float Delta_T;
/***********************************************************
@��������Strapdown_INS_High_Kalman
@��ڲ�������
@���ڲ�������
�����������߶ȷ��򿨶����˲�
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Strapdown_INS_High_Kalman(void)
{
  Test_Period(&SINS_High_Delta);
  Delta_T=SINS_High_Delta.Time_Delta/1000.0f;
  if(Delta_T>2*WP_Duty_Dt||Delta_T<WP_Duty_Dt||isnan(Delta_T)!=0)   Delta_T=WP_Duty_Dt;
  Observation_Tradeoff(1);
  KalmanFilter(Observation_Altitude,//λ�ù۲���
               High_Delay_Cnt,//�۲⴫������ʱ��
               &NamelessQuad,//�ߵ��ṹ��
               Origion_NamelessQuad.Acceleration[_YAW],//ϵͳԭʼ���������ߵ����ٶ�
               R,
               Q,
               Delta_T,
               _YAW,
               1);
}

/*************************���¼�������ͶӰ��������Դ��APM3.2 AP.Math.c�ļ�******************************/
/***********************************************************
@��������longitude_scale
@��ڲ�����Location loc
@���ڲ�������
��������������ͶӰ
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
float longitude_scale(Location loc)
{
  static int32_t last_lat;
  static float scale = 1.0;
  //�Ƚ�����γ�����ֵ�������ظ��������Һ���
  if (ABS(last_lat - loc.lat) < 100000) {
    // we are within 0.01 degrees (about 1km) of the
    // same latitude. We can avoid the cos() and return
    // the same scale factor.
    return scale;
  }
  scale = cosf(loc.lat * 1.0e-7f * DEG_TO_RAD);
  scale = constrain_float(scale, 0.01f, 1.0f);
  last_lat = loc.lat;
  return scale;
}
/*
return the distance in meters in North/East plane as a N/E vector
from loc1 to loc2
*/
Vector2f location_diff(Location loc1,Location loc2)
{
  Vector2f Location_Delta;
  Location_Delta.x=(loc2.lat - loc1.lat) * LOCATION_SCALING_FACTOR;//���뵥λΪm
  Location_Delta.y=(loc2.lng - loc1.lng) * LOCATION_SCALING_FACTOR * longitude_scale(loc2);//���뵥λΪm
  return Location_Delta;
}

// return distance in meters between two locations
float get_distance(Location loc1,Location loc2){
  float dlat              = (float)(loc2.lat - loc1.lat);
  float dlong             = ((float)(loc2.lng - loc1.lng)) * longitude_scale(loc2);
  return pythagorous2(dlat, dlong) * LOCATION_SCALING_FACTOR;
}
/*************************���ϼ�������ͶӰ��������Դ��APM3.2 AP.Math.c�ļ�******************************/
float Body_Frame_To_XYZ[3]={0,0,0};//��������������ϵ��������(Y��)������(X��)����λ��ƫ��
Vector3_Nav Earth_Frame_To_XYZ={0,0,0};//��������������ϵ���� ������������������λ��ƫ��
Vector2i GPSData_To_XY_Home={1143637460,306276000};//����γ��
Location GPS_Home;//��ʼ��λ�ɹ�����Ϣ
Location GPS_Present;//��ǰλ�õ���Ϣ
uint8_t GPS_Home_Set=0;
float Declination=0;//�����ƫ��
/***********************************************************
@��������Set_GPS_Home
@��ڲ�������
@���ڲ�������
��������������GPSԭ��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Set_GPS_Home(void)//����Home��
{
	static uint16_t gps_home_cnt=0;
  if(GPS_Home_Set==0
     &&GPS_Sate_Num>=7//�������ڵ���7
	    &&Horizontal_Acc_Est<=10)//ˮƽλ�ù��ƾ���С��10m
	 {
		 if(gps_home_cnt<=30) gps_home_cnt++;
	 }//ˢ��10hz������3S����

   if(GPS_Home_Set==0&&gps_home_cnt==30)//ȫ��ֻ����һ��
  {
    GPSData_To_XY_Home.x=Longitude_Origion;//Longitude;
    GPSData_To_XY_Home.y=Latitude_Origion;//Latitude;
    GPS_Home_Set=1;//�趨������
    GPS_Home.lng=GPSData_To_XY_Home.x;
    GPS_Home.lat=GPSData_To_XY_Home.y; 
    Strapdown_INS_Reset(&NamelessQuad,_PITCH,Earth_Frame_To_XYZ.E,0);//��λ�ߵ��ں�
    Strapdown_INS_Reset(&NamelessQuad,_ROLL,Earth_Frame_To_XYZ.N,0);//��λ�ߵ��ں�
    Declination=get_declination(0.0000001f*GPS_Home.lat,0.0000001f*GPS_Home.lng);//��ȡ���ش�ƫ��
  }
}

/***********************************************************
@��������GPSData_Sort
@��ڲ�������
@���ڲ�������
��������������GPSԭ�㣬�õ���γ�ȷ����λ��ƫ��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void GPSData_Sort()
{
  Vector2f location_delta={0,0};
  GPS_Present.lng=Longitude_Origion;//���µ�ǰ��γ��
  GPS_Present.lat=Latitude_Origion;
  location_delta=location_diff(GPS_Home,GPS_Present);//���ݵ�ǰGPS��λ��Ϣ��Home��λ����Ϣ������������������λ��ƫ��
  /***********************************************************************************
  ��ȷ�µ���ϵ������������������Ϊ������:
  ������������������,�����˻����home�㣬���������ƶ�ʱ����ʱGPS_Present.lng>GPS_Home.lng���õ���location_delta.x����0;
  ����������γ������,�����˻����home�㣬���������ƶ�ʱ����ʱGPS_Present.lat>GPS_Home.lat���õ���location_delta.y����0;
  ******************************************************************************/
  location_delta.x*=100;//�ص�������ϵ����������λ��ƫ��,��λΪCM
  location_delta.y*=100;//�ص�������ϵ����������λ��ƫ��,��λΪCM
  Earth_Frame_To_XYZ.E=location_delta.y;//����ϵ�����Home������λ��ƫ�ƣ���λΪCM
  Earth_Frame_To_XYZ.N=location_delta.x;//����ϵ�����Home������λ��ƫ�ƣ���λΪCM
  //�����˻��ڵ�������ϵ�µ��������������������λ��ƫ����ת����ǰ�����λ��ƫ��:��ͷ(����)+���
  Body_Frame_To_XYZ[_PITCH]=location_delta.x*Cos_Yaw+location_delta.y*Sin_Yaw;//��ͷ����λ��ƫ��  Y������
  Body_Frame_To_XYZ[_ROLL]=-location_delta.x*Sin_Yaw+location_delta.y*Cos_Yaw;  //�������λ��ƫ��  X������
}



#define Dealt_Update 0.1 //100ms����һ��
#define Process_Noise_Constant 1.0//1.0
#define Pos_Process_Noise  0.5*Process_Noise_Constant*Dealt_Update*Dealt_Update//0.005
#define Vel_Process_Noise  Process_Noise_Constant*Dealt_Update  //0.1
float R_GPS[2]={Pos_Process_Noise,Vel_Process_Noise};
float Q_GPS[2]={0.015,3.0};//{0.075,3.0};{0.015,3.0}
float R_Acce_bias[2]={0.0001,0};
double Pre_conv_GPS[2][4]=
{
  0.018 ,    0.005,  0.005    , 0.5,//0.0001 ,    0.00001,  0.00001    , 0.003,
  0.018 ,    0.005,  0.005    , 0.5,
};//��һ��Э����
double K_GPS[2][2]={0};//�������
float Acce_Bias[2]={0,0.001};
/***********************************************************
@��������KalmanFilter_Horizontal_GPS
@��ڲ�����float Position_GPS,float Vel_GPS,float Position_Last,
					 float Vel_Last,
					 float *Position,float *Vel,
					 float *Acce,float *R,
					 float *Q,float dt,uint8_t Axis
@���ڲ�������
����������GPSˮƽ���򿨶����˲��ں�
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void   KalmanFilter_Horizontal_GPS(float Position_GPS,float Vel_GPS,float Position_Last,float Vel_Last,
                                   float *Position,float *Vel,
                                   float *Acce,float *R,
                                   float *Q,float dt,uint8_t Axis)
{
  float Conv_Z=0;
  float Z_Delta[2]={0};
  float Conv_Temp=0;
  double Temp_conv[4]={0};//����Э����
  uint8 Label=0;
  if(Axis=='X') Label=0;
  else Label=1;
  //����״̬
  *Position +=*Vel*dt+((*Acce+Acce_Bias[Label])*dt*dt)/2.0f;
  *Vel+=(*Acce+Acce_Bias[Label])*dt;
  //����Э����
  Conv_Temp=Pre_conv_GPS[Label][1]+Pre_conv_GPS[Label][3]*dt;
  Temp_conv[0]=Pre_conv_GPS[Label][0]+Pre_conv_GPS[Label][2]*dt+Conv_Temp*dt+R_GPS[0];
  Temp_conv[1]=Conv_Temp;
  Temp_conv[2]=Pre_conv_GPS[Label][2]+Pre_conv_GPS[Label][3]*dt;
  Temp_conv[3]=Pre_conv_GPS[Label][3]+R_GPS[1];
  //���㿨��������
  Conv_Z=1.0/((Temp_conv[0]+Q_GPS[0]*GPS_Quality)*(Temp_conv[3]+Q_GPS[1]*GPS_Quality)-Temp_conv[1]*Temp_conv[2]);
  //��������
  K_GPS[0][0]=( Temp_conv[0]*(Temp_conv[3]+Q_GPS[1]*GPS_Quality)-Temp_conv[1]*Temp_conv[2])*Conv_Z;
  K_GPS[0][1]=(Temp_conv[1]*Q_GPS[0]*GPS_Quality)*Conv_Z;
  K_GPS[1][0]=(Temp_conv[2]*Q_GPS[1]*GPS_Quality)*Conv_Z;
  K_GPS[1][1]=(-Temp_conv[1]*Temp_conv[2]+Temp_conv[3]*(Temp_conv[0]+Q_GPS[0]*GPS_Quality))*Conv_Z;
  //�ں��������
  Z_Delta[0]=Position_GPS-Position_Last;
  Z_Delta[1]=Vel_GPS-Vel_Last;
	
  *Position +=K_GPS[0][0]*Z_Delta[0]
    +K_GPS[0][1]*Z_Delta[1];
  *Vel +=K_GPS[1][0]*Z_Delta[0]
    +K_GPS[1][1]*Z_Delta[1];
  Acce_Bias[Label]+=R_Acce_bias[0]*Z_Delta[0]
    +R_Acce_bias[1]*Z_Delta[1];
		
	Acce_Bias[Label]=constrain_float(Acce_Bias[Label],-50,50);
	
  //����״̬Э�������
  Pre_conv_GPS[Label][0]=(1-K_GPS[0][0])*Temp_conv[0]-K_GPS[0][1]*Temp_conv[2];
  Pre_conv_GPS[Label][1]=(1-K_GPS[0][0])*Temp_conv[1]-K_GPS[0][1]*Temp_conv[3];
  Pre_conv_GPS[Label][2]=(1-K_GPS[1][1])*Temp_conv[2]-K_GPS[1][0]*Temp_conv[0];
  Pre_conv_GPS[Label][3]=(1-K_GPS[1][1])*Temp_conv[3]-K_GPS[1][0]*Temp_conv[1];
}

void Strapdown_INS_Reset(SINS *Ins,uint8_t Axis,float Pos_Target,float Vel_Target)
{
  uint16_t Cnt=0;
  Ins->Position[Axis]=Pos_Target;//λ������
  Ins->Speed[Axis]=Vel_Target;//�ٶ�����
  Ins->Acceleration[Axis]=0.0f;//���ٶ�����
  Ins->Acce_Bias[Axis]=0.0f;
  for(Cnt=Num-1;Cnt>0;Cnt--)//��ʷλ��ֵ��ȫ��װ��Ϊ��ǰ�۲�ֵ
  {
    Ins->Pos_History[Axis][Cnt]=Pos_Target;
  }
  Ins->Pos_History[Axis][0]=Pos_Target;
  for(Cnt=Num-1;Cnt>0;Cnt--)//��ʷ�ٶ�ֵ��ȫ��װ��Ϊ��ǰ�۲�ֵ
  {
    Ins->Vel_History[Axis][Cnt]=Vel_Target;
  }
  Ins->Vel_History[Axis][0]=Vel_Target;
}


#define X_Axis  0
#define Y_Axis  1
#define NUM_BUF 50
float Position_History[2][NUM_BUF]={0};
float Vel_History[2][NUM_BUF]={0};
uint16 GPS_Vel_Delay_Cnt=5;// 5
uint16 GPS_Pos_Delay_Cnt=10;//10
int16 GPS_Position_Cnt=0;
float Acce_bias[2]={0};
Testime SINS_Horizontal_Delta;
float Horizontal_Delta_T=0;
uint8_t Filter_Defeated_Flag=0;
void KalmanFilter_Horizontal(void)
{
  int16 i=0;
  if(GPS_Home_Set==1)
  {
    GPSData_Sort();
    Test_Period(&SINS_Horizontal_Delta);
    Horizontal_Delta_T=(float)(SINS_Horizontal_Delta.Time_Delta/1000.0f);
    if(Horizontal_Delta_T>2*WP_Duty_Dt||Horizontal_Delta_T<WP_Duty_Dt||isnan(Horizontal_Delta_T)!=0)   Horizontal_Delta_T=WP_Duty_Dt;
    GPS_Position_Cnt++;
    if(GPS_Position_Cnt>=2)//ÿ10ms����һ��
    {
      for(i=NUM_BUF-1;i>0;i--)
      {
        Position_History[X_Axis][i]=Position_History[X_Axis][i-1];
        Position_History[Y_Axis][i]=Position_History[Y_Axis][i-1];
        Vel_History[X_Axis][i]=Vel_History[X_Axis][i-1];
        Vel_History[Y_Axis][i]=Vel_History[Y_Axis][i-1];
      }
      Position_History[X_Axis][0]=NamelessQuad.Position[_PITCH];
      Position_History[Y_Axis][0]=NamelessQuad.Position[_ROLL];
      Vel_History[X_Axis][0]=NamelessQuad.Speed[_PITCH];
      Vel_History[Y_Axis][0]=NamelessQuad.Speed[_ROLL];
      GPS_Position_Cnt=0;
    }
    NamelessQuad.Acceleration[_PITCH]=Origion_NamelessQuad.Acceleration[_PITCH];
    NamelessQuad.Acceleration[_ROLL]=Origion_NamelessQuad.Acceleration[_ROLL];
    if(GPS_Update_Flag==1)
    {
      KalmanFilter_Horizontal_GPS(Earth_Frame_To_XYZ.E,
                                  GPS_Speed_Resolve[1],
                                  Position_History[X_Axis][GPS_Pos_Delay_Cnt],
                                  Vel_History[X_Axis][GPS_Vel_Delay_Cnt],
                                  &NamelessQuad.Position[_PITCH],
                                  &NamelessQuad.Speed[_PITCH],
                                  &Origion_NamelessQuad.Acceleration[_PITCH],
                                  R_GPS,Q_GPS,Horizontal_Delta_T,'X');
      KalmanFilter_Horizontal_GPS(Earth_Frame_To_XYZ.N,
                                  GPS_Speed_Resolve[0],
                                  Position_History[Y_Axis][GPS_Pos_Delay_Cnt],
                                  Vel_History[Y_Axis][GPS_Vel_Delay_Cnt],
                                  &NamelessQuad.Position[_ROLL],
                                  &NamelessQuad.Speed[_ROLL],
                                  &Origion_NamelessQuad.Acceleration[_ROLL],
                                  R_GPS,Q_GPS,Horizontal_Delta_T,'Y');
      GPS_Update_Flag=0;
    }
    else
    {
      NamelessQuad.Position[_PITCH] +=NamelessQuad.Speed[_PITCH]*Horizontal_Delta_T
        +((NamelessQuad.Acceleration[_PITCH]+Acce_Bias[0])*Horizontal_Delta_T*Horizontal_Delta_T)/2.0f;
			NamelessQuad.Speed[_PITCH]+=(NamelessQuad.Acceleration[_PITCH]+Acce_Bias[0])*Horizontal_Delta_T;
      
      NamelessQuad.Position[_ROLL] +=NamelessQuad.Speed[_ROLL]*Horizontal_Delta_T
        +((NamelessQuad.Acceleration[_ROLL]+Acce_Bias[1])*Horizontal_Delta_T*Horizontal_Delta_T)/2.0f;
      NamelessQuad.Speed[_ROLL]+=(NamelessQuad.Acceleration[_ROLL]+Acce_Bias[1])*Horizontal_Delta_T;
    }
    
    if(ABS(NamelessQuad.Position[_PITCH]-Earth_Frame_To_XYZ.E)>10000
       ||ABS(NamelessQuad.Position[_ROLL]-Earth_Frame_To_XYZ.N)>10000
         ||ABS(NamelessQuad.Speed[_PITCH]-GPS_Speed_Resolve[1])>10000
           ||ABS(NamelessQuad.Speed[_ROLL]-GPS_Speed_Resolve[0])>10000
             )
    {
      Filter_Defeated_Flag=1;//��ʼʱ���ں�ʧ�ܱ�־
    }
  }
}


/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/
