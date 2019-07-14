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
#include "Calibration.h"
#include "CalibrationRoutines.h"

int16_t Mag_Offset[3]={0,0,0};
float Mag_Data[3]={0};
Vector2f MagN={0,0};
/***************���ٶȼ�6��������ο�APM���룬���ң���������ֳ�����**************************/
/***********************************************************
@��������Calibrate_Reset_Matrices
@��ڲ�����float dS[6], float JS[6][6]
@���ڲ�������
@�����������������ݸ�λ
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Calibrate_Reset_Matrices(float dS[6], float JS[6][6])
{
  int16_t j,k;
  for( j=0; j<6; j++ )
  {
    dS[j] = 0.0f;
    for( k=0; k<6; k++ )
    {
      JS[j][k] = 0.0f;
    }
  }
}


/***********************************************************
@��������Calibrate_Find_Delta
@��ڲ�����float dS[6], float JS[6][6], float delta[6]
@���ڲ�������
@���������������󷽳�JS*x = dS����һ���Ѿ�����������
��JS���ڵ����·���ȫ����Ϊ0��Ȼ��ش��õ����Է��̵Ľ�
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Calibrate_Find_Delta(float dS[6], float JS[6][6], float delta[6])
{
  //Solve 6-d matrix equation JS*x = dS
  //first put in upper triangular form
  int16_t i,j,k;
  float mu;
  //make upper triangular
  for( i=0; i<6; i++ ) {
    //eliminate all nonzero entries below JS[i][i]
    for( j=i+1; j<6; j++ ) {
      mu = JS[i][j]/JS[i][i];
      if( mu != 0.0f ) {
        dS[j] -= mu*dS[i];
        for( k=j; k<6; k++ ) {
          JS[k][j] -= mu*JS[k][i];
        }
      }
    }
  }
  //back-substitute
  for( i=5; i>=0; i-- ) {
    dS[i] /= JS[i][i];
    JS[i][i] = 1.0f;
    
    for( j=0; j<i; j++ ) {
      mu = JS[i][j];
      dS[j] -= mu*dS[i];
      JS[i][j] = 0.0f;
    }
  }
  for( i=0; i<6; i++ ) {
    delta[i] = dS[i];
  }
}


void Calibrate_Update_Matrices(float dS[6],
                               float JS[6][6],
                               float beta[6],
                               float data[3])
{
  int16_t j, k;
  float dx, b;
  float residual = 1.0;
  float jacobian[6];
  for(j=0;j<3;j++)
  {
    b = beta[3+j];
    dx = (float)data[j] - beta[j];
    residual -= b*b*dx*dx;
    jacobian[j] = 2.0f*b*b*dx;
    jacobian[3+j] = -2.0f*b*dx*dx;
  }
  
  for(j=0;j<6;j++)
  {
    dS[j]+=jacobian[j]*residual;
    for(k=0;k<6;k++)
    {
      JS[j][k]+=jacobian[j]*jacobian[k];
    }
  }
}



uint8 Calibrate_accel(Acce_Unit accel_sample[6],
                      Acce_Unit *accel_offsets,
                      Acce_Unit *accel_scale)
{
  int16_t i;
  int16_t num_iterations = 0;
  float eps = 0.000000001;
  float change = 100.0;
  float data[3]={0};
  float beta[6]={0};
  float delta[6]={0};
  float ds[6]={0};
  float JS[6][6]={0};
  bool success = TRUE;
  // reset
  beta[0] = beta[1] = beta[2] = 0;
  beta[3] = beta[4] = beta[5] = 1.0f/GRAVITY_MSS;
  while( num_iterations < 20 && change > eps ) {
    num_iterations++;
    Calibrate_Reset_Matrices(ds, JS);
    for( i=0; i<6; i++ ) {
      data[0] = accel_sample[i].x;
      data[1] = accel_sample[i].y;
      data[2] = accel_sample[i].z;
      Calibrate_Update_Matrices(ds, JS, beta, data);
    }
    Calibrate_Find_Delta(ds, JS, delta);
    change =    delta[0]*delta[0] +
      delta[0]*delta[0] +
        delta[1]*delta[1] +
          delta[2]*delta[2] +
            delta[3]*delta[3] / (beta[3]*beta[3]) +
              delta[4]*delta[4] / (beta[4]*beta[4]) +
                delta[5]*delta[5] / (beta[5]*beta[5]);
    for( i=0; i<6; i++ ) {
      beta[i] -= delta[i];
    }
  }
  // copy results out
  accel_scale->x = beta[3] * GRAVITY_MSS;
  accel_scale->y = beta[4] * GRAVITY_MSS;
  accel_scale->z = beta[5] * GRAVITY_MSS;
  accel_offsets->x = beta[0] * accel_scale->x;
  accel_offsets->y = beta[1] * accel_scale->y;
  accel_offsets->z = beta[2] * accel_scale->z;
  
  // sanity check scale
  if(fabsf(accel_scale->x-1.0f) > 0.5f
     || fabsf(accel_scale->y-1.0f) > 0.5f
       || fabsf(accel_scale->z-1.0f) > 0.5f )
  {
    success = FALSE;
  }
  // sanity check offsets (3.5 is roughly 3/10th of a G, 5.0 is roughly half a G)
  if(fabsf(accel_offsets->x) > 5.0f
     || fabsf(accel_offsets->y) > 5.0f
       || fabsf(accel_offsets->z) > 5.0f )
  {
    success = FALSE;
  }
  // return success or failure
  return success;
}



float Aoco[6]={1,1,1};
float Aoc[6][6]={1,1,1};
Acce_Unit new_offset={
  0,0,0,
};
Acce_Unit new_scales={
  1.0,1.0,1.0,
};

Acce_Unit Accel_Offset_Read={
  0,0,0,
};
Acce_Unit Accel_Scale_Read={
  0,0,0,
};
uint8_t Cal_Flag=0;
void Calibrationer(void)
{
  uint16 i=0;
  Acce_Unit Test_Calibration[6]=
  {
    20,     21,    4152,
    4062,  -24,      78,
    -4082, 1,        -8,
    -45,   -4071,   30,
    20,    4035,    -8,
    30,     -60,   -3980
  };
  for(i=0;i<6;i++)
  {
    Test_Calibration[i].x *=ACCEL_TO_1G;
    Test_Calibration[i].y *=ACCEL_TO_1G;
    Test_Calibration[i].z *=ACCEL_TO_1G;
  }
  
  Cal_Flag=Calibrate_accel(Test_Calibration,
                           &new_offset,
                           &new_scales);
}


uint8_t flight_direction=6;
uint8_t Accel_Calibration_Flag=0;//���ٶȼ�У׼ģʽ
uint8_t Accel_Calibration_Finished[6]={0,0,0,0,0,0};//��Ӧ��У׼��ɱ�־λ
uint8_t Accel_Calibration_All_Finished=0;//6��У׼ȫ��У׼��ɱ�־λ
uint16_t Accel_Calibration_Makesure_Cnt=0;
uint16_t Accel_flight_direction_cnt=0;
/***********************************************************
@��������Accel_Calibration_Check
@��ڲ�������
@���ڲ�������
@�������������ٶȼƱ궨����ң��������λ���
����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Accel_Calibration_Check(void)
{
  uint16_t  i=0;
  if(Throttle_Control==1000
     &&Yaw_Control>=Yaw_Max*Scale_Pecent_Max
       &&Roll_Control<=-Pit_Rol_Max*Scale_Pecent_Max
         &&Pitch_Control>=Pit_Rol_Max*Scale_Pecent_Max)
  {
    Accel_Calibration_Makesure_Cnt++;
  }
  if(Throttle_Control==1000
     &&Yaw_Control>=Yaw_Max*Scale_Pecent_Max
       &&Roll_Control<=-Pit_Rol_Max*Scale_Pecent_Max
         &&Pitch_Control>=Pit_Rol_Max*Scale_Pecent_Max
           &&Accel_Calibration_Makesure_Cnt>=200*3//��������
             &&Controler_State==Lock_Controler)//����Ϊ����״̬�ſ��Խ���У׼ģʽ
  {
    Bling_Mode=1;
    Accel_Calibration_Flag=1;//���ٶ�У׼ģʽ
    Cal_Flag=0;	
    Bling_Set(&Light_1,1000,100,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_1,1);
    Bling_Set(&Light_2,1000,100,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,1);
    Bling_Set(&Light_3,1000,100,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_3,1);
    flight_direction=6;
    Accel_Calibration_All_Finished=0;//ȫ��У׼��ɱ�־λ����
    Accel_Calibration_Makesure_Cnt=0;
    for(i=0;i<6;i++)
    {
      Accel_Calibration_Finished[i]=0;//��Ӧ���־λ����
      acce_sample[i].x=0; //��ն�Ӧ��ļ��ٶȼ���
      acce_sample[i].y=0; //��ն�Ӧ��ļ��ٶȼ���
      acce_sample[i].z=0; //��ն�Ӧ��ļ��ٶȼ���
    }
    Page_Number=10;//OLED���ٶȼƽ���ҳ��
    Reset_Mag_Calibartion(1);
    Reset_RC_Calibartion(1);
    Forced_Lock_Makesure_Cnt=0;
  }
  
  if(Accel_Calibration_Flag==1)
  {
    if(Throttle_Control==1000&&Yaw_Control<=-Yaw_Max*Scale_Pecent_Max&&Roll_Control==0&&Pitch_Control==0)
    {
      Accel_flight_direction_cnt++;
      if(Accel_flight_direction_cnt>=20)//100ms
      {
        flight_direction=0;
        Unlock_Makesure_Cnt=0;
        Lock_Makesure_Cnt=0;
      }
      
    }
    else if(Throttle_Control==1000&&Yaw_Control==0&&Roll_Control>=Pit_Rol_Max*Scale_Pecent_Max&&Pitch_Control==0)
    {
      Accel_flight_direction_cnt++;
      if(Accel_flight_direction_cnt>=20)//100ms
      {
        flight_direction=1;
        Unlock_Makesure_Cnt=0;
        Lock_Makesure_Cnt=0;
      }
    }
    else if(Throttle_Control==1000&&Yaw_Control==0&&Roll_Control<=-Pit_Rol_Max*Scale_Pecent_Max&&Pitch_Control==0)
    {
      Accel_flight_direction_cnt++;
      if(Accel_flight_direction_cnt>=20)//100ms
      {
        flight_direction=2;
        Unlock_Makesure_Cnt=0;
        Lock_Makesure_Cnt=0;
      }
    }
    else if(Throttle_Control==1000&&Yaw_Control==0&&Roll_Control==0&&Pitch_Control>=Pit_Rol_Max*Scale_Pecent_Max)
    {
      Accel_flight_direction_cnt++;
      if(Accel_flight_direction_cnt>=20)//100ms
      {
        flight_direction=3;
        Unlock_Makesure_Cnt=0;
        Lock_Makesure_Cnt=0;
      }
    }
    else if(Throttle_Control==1000&&Yaw_Control==0&&Roll_Control==0&&Pitch_Control<=-Pit_Rol_Max*Scale_Pecent_Max)
    {
      Accel_flight_direction_cnt++;
      if(Accel_flight_direction_cnt>=20)//100ms
      {
        flight_direction=4;
        Unlock_Makesure_Cnt=0;
        Lock_Makesure_Cnt=0;
      }
    }
    else if(Throttle_Control==1000&&Yaw_Control>Yaw_Max*Scale_Pecent_Max&&Roll_Control==0&&Pitch_Control==0)
    {
      Accel_flight_direction_cnt++;
      if(Accel_flight_direction_cnt>=20)//100ms
      {
        flight_direction=5;
        Unlock_Makesure_Cnt=0;
        Lock_Makesure_Cnt=0;
      }
    }
    else
    {
      Accel_flight_direction_cnt/=2;
    }
    
    if(Accel_flight_direction_cnt>=200)  Accel_flight_direction_cnt=0;
    
  }
  
}

Acce_Unit acce_sample[6]={0};//����6�У�����6�����������
uint8_t Flash_Buf[12]={0};
/***********************************************************
@��������Accel_Calibartion
@��ڲ�������
@���ڲ�������
@�������������ٶȱ궨������ң����ֱ�ӽ���
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
uint8_t Accel_Calibartion(void)
{
  uint16 i,j=0;
  float acce_sample_sum[3]={0,0,0};//���ٶȺ�����
  /*��һ��ɿ�ƽ�ţ�Z�����������Ϸ���Z axis is about 1g,X��Y is about 0g*/
  /*�ڶ���ɿ�ƽ�ţ�X�����������Ϸ���X axis is about 1g,Y��Z is about 0g*/
  /*������ɿ�ƽ�ţ�X�����������·���X axis is about -1g,Y��Z is about 0g*/
  /*������ɿ�ƽ�ţ�Y�����������·���Y axis is about -1g,X��Z is about 0g*/
  /*������ɿ�ƽ�ţ�Y�����������Ϸ���Y axis is about 1g,X��Z is about 0g*/
  /*������ɿ�ƽ�ţ�Z�����������·���Z axis is about -1g,X��Y is about 0g*/
  if(flight_direction<=5)//��⵽��Ӧ������
  {
    uint16_t num_samples=0;
    while(num_samples<1000)//����200��
    {
      if(Gyro_Length<=20.0f
         &&WP_Sensor.imu_updtate_flag==1)//ͨ��������ģ����ȷ�����徲ֹ
      {
        for(j=0;j<3;j++){
          acce_sample_sum[j]+=WP_Sensor.acce_filter[j]*ACCEL_TO_1G;//���ٶȼ�ת��Ϊ1g������
        }
        //delay_ms(4);//���10ms��1s������ȡƽ��
        num_samples++;
        WP_Sensor.imu_updtate_flag=0;
      }
      Accel_Calibration_Finished[flight_direction]=1;//��Ӧ��У׼��ɱ�־λ��1
    }
    acce_sample[flight_direction].x=acce_sample_sum[0]/num_samples; //�����Ӧ��ļ��ٶȼ���
    acce_sample[flight_direction].y=acce_sample_sum[1]/num_samples; //�����Ӧ��ļ��ٶȼ���
    acce_sample[flight_direction].z=acce_sample_sum[2]/num_samples; //�����Ӧ��ļ��ٶȼ���
    flight_direction=6;//����������
  }
  
  if((Accel_Calibration_Finished[0]
      &Accel_Calibration_Finished[1]
        &Accel_Calibration_Finished[2]
          &Accel_Calibration_Finished[3]
            &Accel_Calibration_Finished[4]
              &Accel_Calibration_Finished[5])
     &&Accel_Calibration_All_Finished==0)//6��ȫ��У׼���
  {
    Accel_Calibration_All_Finished=1;//���ٶȼ�6��У׼��ɱ�־
    Accel_Calibration_Flag=0;//���ٶȼ�У׼�������ͷ�ң�в���
    Cal_Flag=Calibrate_accel(acce_sample,
                             &new_offset,
                             &new_scales);//������6������
    for(i=0;i<6;i++)
    {
      Accel_Calibration_Finished[i]=0;//��Ӧ���־λ����
    }
    if(Cal_Flag==TRUE)//���ٶȼ�У׼�ɹ�
    {
      WriteFlashParameter_Three(ACCEL_X_OFFSET,
                                new_offset.x,
                                new_offset.y,
                                new_offset.z,
                                &Table_Parameter);
      WriteFlashParameter_Three(ACCEL_X_SCALE,
                                new_scales.x,
                                new_scales.y,
                                new_scales.z,
                                &Table_Parameter);
      
      Parameter_Init();//��ȡд�����
      Bling_Mode=0;//�ָ�����ָʾģʽ
      Bling_Set(&Light_1,3000,100,0.3,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
      Bling_Set(&Light_2,3000,100,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
      Bling_Set(&Light_3,3000,100,0.8,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
    }
    else//���ٶȼ�У׼ʧ��
    {
      Bling_Mode=0;//�ָ�����ָʾģʽ
      Bling_Set(&Light_1,5000,200,0.3,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
      Bling_Set(&Light_2,5000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
      Bling_Set(&Light_3,5000,200,0.8,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
      Page_Number=0;//OLED�ָ���ҳ
    }
    return TRUE;
  }
  return FALSE;
}


/***********************************************************
@��������Reset_Accel_Calibartion
@��ڲ�����uint8_t Type
@���ڲ�������
@�������������ٶȱ궨���������ǿ�Ƹ�λ
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Reset_Accel_Calibartion(uint8_t Type)
{
  uint16 i=0;
  for(i=0;i<6;i++)
  {
    Accel_Calibration_Finished[i]=0;//��Ӧ���־λ����
    acce_sample[i].x=0; //��ն�Ӧ��ļ��ٶȼ���
    acce_sample[i].y=0; //��ն�Ӧ��ļ��ٶȼ���
    acce_sample[i].z=0; //��ն�Ӧ��ļ��ٶȼ���
  }
  Accel_Calibration_All_Finished=0;//ȫ��У׼��ɱ�־λ����
  if(Type==1)  Accel_Calibration_Flag=0;
}


typedef struct
{
  uint8_t accel_off;
  uint8_t accel_scale;
  uint8_t mag;
}Parameter_Flag;

Parameter_Flag Parameter_Read_Flag;
/***********************************************************
@��������Parameter_Init
@��ڲ�������
@���ڲ�������
@����������������У׼������ʼ��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
bool Parameter_Init(void)//���ٶȼơ�������У׼������ȡ
{
  bool success=TRUE;
  /************���ٶȼ���ƫ����ֵ*******/
  ReadFlashParameterThree(ACCEL_X_OFFSET,
                          &Accel_Offset_Read.x,
                          &Accel_Offset_Read.y,
                          &Accel_Offset_Read.z);
  
  ReadFlashParameterThree(ACCEL_X_SCALE,
                          &Accel_Scale_Read.x,
                          &Accel_Scale_Read.y,
                          &Accel_Scale_Read.z);
  /************��������ƫ****************/
  ReadFlashParameterThree(MAG_X_OFFSET,
                          &Mag_Offset_Read.x,
                          &Mag_Offset_Read.y,
                          &Mag_Offset_Read.z);
  // sanity check scale
  if(ABS(Accel_Scale_Read.x-1.0f)>0.5f
     || ABS(Accel_Scale_Read.y-1.0f)>0.5f
       || ABS(Accel_Scale_Read.z-1.0f)>0.5f)
  {
    success = FALSE;
  }
  // sanity check offsets (3.5 is roughly 3/10th of a G, 5.0 is roughly half a G)
  if(ABS(Accel_Offset_Read.x) > 5.0f
     || ABS(Accel_Offset_Read.y) > 5.0f
       || ABS(Accel_Offset_Read.z) > 5.0f)
  {
    success = FALSE;
  }
  
  
  if(isnan(Accel_Offset_Read.x)==0
     &&isnan(Accel_Offset_Read.y)==0
       &&isnan(Accel_Offset_Read.z)==0
         &&isnan(Accel_Scale_Read.x)==0
           &&isnan(Accel_Scale_Read.y)==0
             &&isnan(Accel_Scale_Read.z)==0)//Flash���������������¼��ٶ�У��ֵ
  {    
    B[0]=Accel_Offset_Read.x;
    B[1]=Accel_Offset_Read.y;
    B[2]=Accel_Offset_Read.z;
    K[0]=Accel_Scale_Read.x;
    K[1]=Accel_Scale_Read.y;
    K[2]=Accel_Scale_Read.z;	
  }
  else if(success==TRUE)
  {
    B[0]=0;
    B[1]=0;
    B[2]=0;
    K[0]=1;
    K[1]=1;
    K[2]=1;
  }
  /**********����������ƫִ��ȡ************/
  if(isnan(Mag_Offset_Read.x)==0
     &&isnan(Mag_Offset_Read.y)==0
       &&isnan(Mag_Offset_Read.z==0))
  {
    mag_offset.x=(int16_t)(Mag_Offset_Read.x);
    mag_offset.y=(int16_t)(Mag_Offset_Read.y);
    mag_offset.z=(int16_t)(Mag_Offset_Read.z);
  }
  else
  {
    mag_offset.x=0;
    mag_offset.y=0;
    mag_offset.z=0;    
  }
  return success;
}
/************���ٶȼ�6���������***********************/


/***********���������Ľ�����ȡ���������Сֵƽ��******/
uint8_t Mag_Calibration_Flag=0,Mag_Calibration_All_Finished;
uint8_t Mag_Calibration_Finished[3]={0};
uint16_t Mag_Calibration_Makesure_Cnt=0;
uint8_t  Mag_Calibration_Mode=3;
uint16_t Mag_Calibration_Cnt=0;
float Yaw_Correct=0;
/*********************************************/
const int16_t Mag_360_define[36]={
  0,10,20,30,40,50,60,70,80,90,
  100,110,120,130,140,150,160,170,180,190,
  200,210,220,230,240,250,260,270,280,290,
  300,310,320,330,340,350
};//�����ƽ��������Ƕȣ�ȷ�����ݲɼ����
uint8_t Last_Mag_360_Flag[3][36]={0};
uint8_t Mag_360_Flag[3][36]={0};
uint16_t Mag_Is_Okay_Flag[3];
Calibration Mag;
Mag_Unit DataMag;
Mag_Unit Mag_Offset_Read={
  0,0,0,
};
/***********************************************************
@��������Mag_Calibration_Check
@��ڲ�������
@���ڲ�������
@���������������Ʊ궨����ң��������λ�������
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Mag_Calibration_Check(void)
{
  uint16_t  i=0,j=0;
  if(Throttle_Control==1000
     &&Yaw_Control>=Yaw_Max*Scale_Pecent_Max
       &&Roll_Control>=Pit_Rol_Max*Scale_Pecent_Max
         &&Pitch_Control>=Pit_Rol_Max*Scale_Pecent_Max)
    Mag_Calibration_Makesure_Cnt++;
  
  if(Throttle_Control==1000
     &&Yaw_Control>=Yaw_Max*Scale_Pecent_Max
       &&Roll_Control>=Pit_Rol_Max*Scale_Pecent_Max
         &&Pitch_Control>=Pit_Rol_Max*Scale_Pecent_Max
           &&Mag_Calibration_Makesure_Cnt>200*5//����5S
             &&Controler_State==Lock_Controler)//����Ϊ����״̬�ſ��Խ���У׼ģʽ
    //���������У׼ģʽ
  {
    Bling_Mode=2;
    Mag_Calibration_Flag=1;//������У׼ģʽ
    Mag_Calibration_Mode=3;
    Bling_Set(&Light_1,1000,500,0.2,0,GPIO_PORTF_BASE,GPIO_PIN_1,1);
    Bling_Set(&Light_2,1000,500,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,1);
    Bling_Set(&Light_3,1000,500,0.7,0,GPIO_PORTF_BASE,GPIO_PIN_3,1);
    
    Mag_Calibration_Makesure_Cnt=0;
    Mag_Calibration_All_Finished=0;//ȫ��У׼��ɱ�־λ����
    for(i=0;i<3;i++)
    {
      Mag_Calibration_Finished[i]=0;//��Ӧ���־λ����
      for(j=0;j<36;j++) {Mag_360_Flag[i][j]=0;}
    }
    Page_Number=11;
    Reset_Accel_Calibartion(1);
    Reset_RC_Calibartion(1);
    Forced_Lock_Makesure_Cnt=0;
  }
  
  if(Mag_Calibration_Flag==1)
  {
    if(Throttle_Control==1000
       &&Yaw_Control<=-Yaw_Max*Scale_Pecent_Max
         &&Roll_Control==0
           &&Pitch_Control==0) //��һ�����
    {
      Mag_Calibration_Cnt++;
      if(Mag_Calibration_Cnt>=20)
      {
        Mag_Calibration_Mode=0;
        Mag_Is_Okay_Flag[0]=0;//�������ݲɼ���ɱ�־λ��0
        Mag_Is_Okay_Flag[1]=0;//�������ݲɼ���ɱ�־λ��0
        Mag_Is_Okay_Flag[2]=0;//�������ݲɼ���ɱ�־λ��0
        for(i=0;i<36;i++) Mag_360_Flag[0][i]=0;//��ղɼ��Ǳ������ݵ�
        for(i=0;i<36;i++) Mag_360_Flag[1][i]=0;//��ղɼ��Ǳ������ݵ�
        for(i=0;i<36;i++) Mag_360_Flag[2][i]=0;//��ղɼ��Ǳ������ݵ�
        LS_Init(&Mag_LS);//�������м����
        Unlock_Makesure_Cnt=0;
        Lock_Makesure_Cnt=0;
      }
      
    }
    else if(Throttle_Control==1000
            &&Yaw_Control>Yaw_Max*Scale_Pecent_Max
              &&Roll_Control==0
                &&Pitch_Control==0) //�ڶ������
    {
      Mag_Calibration_Cnt++;
      if(Mag_Calibration_Cnt>=20)
      {
        Mag_Calibration_Mode=1;
        Mag_Is_Okay_Flag[0]=0;//�������ݲɼ���ɱ�־λ��0
        Mag_Is_Okay_Flag[1]=0;//�������ݲɼ���ɱ�־λ��0
        Mag_Is_Okay_Flag[2]=0;//�������ݲɼ���ɱ�־λ��0
        for(i=0;i<36;i++) Mag_360_Flag[0][i]=0;//��ղɼ��Ǳ������ݵ�
        for(i=0;i<36;i++) Mag_360_Flag[1][i]=0;//��ղɼ��Ǳ������ݵ�
        for(i=0;i<36;i++) Mag_360_Flag[2][i]=0;//��ղɼ��Ǳ������ݵ�
        LS_Init(&Mag_LS);//�������м����
        Unlock_Makesure_Cnt=0;
        Lock_Makesure_Cnt=0;
      }
    }
    else
    {
      Mag_Calibration_Cnt/=2;
    }
    if(Mag_Calibration_Cnt>=200)  Mag_Calibration_Cnt=200;
    
  }
  
}

/***********************************************************
@��������Reset_Mag_Calibartion
@��ڲ�����uint8_t Type
@���ڲ�������
@���������������Ʊ궨���������ǿ�Ƹ�λ
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Reset_Mag_Calibartion(uint8_t Type)
{
  uint16 i=0;
  for(i=0;i<36;i++)
  {
    Mag_360_Flag[0][i]=0;//��ղɼ��ǵ�
    Mag_360_Flag[1][i]=0;//��ղɼ��ǵ�
    Mag_360_Flag[2][i]=0;//��ղɼ��ǵ�
  }
  Mag_Is_Okay_Flag[0]=0;
  Mag_Is_Okay_Flag[1]=0;
  Mag_Is_Okay_Flag[2]=0;
  Mag_Calibration_Mode=3;
  if(Type==1)  Mag_Calibration_Flag=0;
}


uint8_t Check_Plane_Sampling_Okay(uint8_t plane_number)
{
  uint8_t finished_flag=0;
  if(Mag_360_Flag[plane_number][0]&Mag_360_Flag[plane_number][1]&Mag_360_Flag[plane_number][2]
     &Mag_360_Flag[plane_number][3]&Mag_360_Flag[plane_number][4]&Mag_360_Flag[plane_number][5]
       &Mag_360_Flag[plane_number][6]&Mag_360_Flag[plane_number][7]&Mag_360_Flag[plane_number][8]
         &Mag_360_Flag[plane_number][9]&Mag_360_Flag[plane_number][10]&Mag_360_Flag[plane_number][11]
           &Mag_360_Flag[plane_number][12]&Mag_360_Flag[plane_number][13]&Mag_360_Flag[plane_number][14]
             &Mag_360_Flag[plane_number][15]&Mag_360_Flag[plane_number][16]&Mag_360_Flag[plane_number][17]
               &Mag_360_Flag[plane_number][18]&Mag_360_Flag[plane_number][19]&Mag_360_Flag[plane_number][20]
                 &Mag_360_Flag[plane_number][21]&Mag_360_Flag[plane_number][22]&Mag_360_Flag[plane_number][23]
                   &Mag_360_Flag[plane_number][24]&Mag_360_Flag[plane_number][25]&Mag_360_Flag[plane_number][26]
                     &Mag_360_Flag[plane_number][27]&Mag_360_Flag[plane_number][28]&Mag_360_Flag[plane_number][29]
                       &Mag_360_Flag[plane_number][30]&Mag_360_Flag[plane_number][31]&Mag_360_Flag[plane_number][32]
                         &Mag_360_Flag[plane_number][33]&Mag_360_Flag[plane_number][34]&Mag_360_Flag[plane_number][35])
    finished_flag=1;
  return finished_flag;
}

/***********************************************************
@��������Mag_Calibartion
@��ڲ����������ǻ��ֽǶ�ֵ�����������ԭʼֵ
@���ڲ�������
@�������������������ı궨������ң����ֱ�ӽ���
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
uint8_t Mag_Calibartion(Vector3f *magdata,Vector3f_Body Circle_Angle_Calibartion)
{
  uint16 i=0;
  for(i=0;i<36;i++)
  {
    Last_Mag_360_Flag[0][i]=Mag_360_Flag[0][i];
    Last_Mag_360_Flag[1][i]=Mag_360_Flag[1][i];
    Last_Mag_360_Flag[2][i]=Mag_360_Flag[2][i];
  }
  /********��һ��Z�����������Ϸ���
  ��ʼ����ֱ����ת��Z axis is about 1g,X��Y is about 0g*/
  /********�ڶ���Y�����������Ϸ���
  ��ʼ����ֱ����ת��Y axis is about 1g,X��Z is about 0g*/
  if(Mag_Calibration_Mode<3)//��⵽��Ӧ������
  {
    for(i=0;i<36;i++)
    {
      if(ABS(Circle_Angle_Calibartion.Yaw-Mag_360_define[i])<=5.0f
         &&Mag_Calibration_Mode==0
           &&WP_Sensor.acce_filter[2]>=AcceMax_1G/2)//Z�������ֱ
      {
        Mag_360_Flag[0][i]=1;
      }
      
      if(ABS(Circle_Angle_Calibartion.Rol-Mag_360_define[i])<=5.0f
         &&Mag_Calibration_Mode==1
           &&WP_Sensor.acce_filter[1]>=AcceMax_1G/2)//Y�������ֱ
      {
        Mag_360_Flag[1][i]=1;
      }
      
      if(ABS(Circle_Angle_Calibartion.Pit-Mag_360_define[i])<=5.0f
         &&Mag_Calibration_Mode==2
           &&WP_Sensor.acce_filter[0]>=AcceMax_1G/2)//X�������ֱ
      {
        Mag_360_Flag[2][i]=1;
      }
    }
    if(magdata->x >= Mag.x_max)   Mag.x_max = (int16_t)(magdata->x);
    if(magdata->x <  Mag.x_min)   Mag.x_min = (int16_t)(magdata->x);
    if(magdata->y >= Mag.y_max)   Mag.y_max = (int16_t)(magdata->y);
    if(magdata->y <  Mag.y_min)   Mag.y_min = (int16_t)(magdata->y);
    if(magdata->z >= Mag.z_max)   Mag.z_max = (int16_t)(magdata->z);
    if(magdata->z <  Mag.z_min)   Mag.z_min = (int16_t)(magdata->z);
  }
  if(Check_Plane_Sampling_Okay(0))
  {
    Mag_Is_Okay_Flag[0]=1;//�������ݲɼ���ɱ�־λ��1
    for(i=0;i<36;i++)  Mag_360_Flag[0][i]=0;//��ղɼ��ǵ�
    if(Mag_Is_Okay_Flag[1]==0)//����һ������δ���
      Mag_Calibration_Mode=1;//�Զ�������һ�����ݲɼ�ģʽ
    else Mag_Calibration_Mode=3;//
  }
  
  if(Check_Plane_Sampling_Okay(1))
  {
    Mag_Is_Okay_Flag[1]=1;//�������ݲɼ���ɱ�־λ��1
    for(i=0;i<36;i++) Mag_360_Flag[1][i]=0;//��ղɼ��ǵ�
    if(Mag_Is_Okay_Flag[2]==0)//����һ������δ���
      Mag_Calibration_Mode=2;//�Զ�������һ�����ݲɼ�ģʽ
    else Mag_Calibration_Mode=3;
  }
  
  if(Check_Plane_Sampling_Okay(2))
  {
    Mag_Is_Okay_Flag[2]=1;//�������ݲɼ���ɱ�־λ��1
    for(i=0;i<36;i++) Mag_360_Flag[2][i]=0;//��ղɼ��ǵ�
    if(Mag_Is_Okay_Flag[0]==0)//����һ������δ���
      Mag_Calibration_Mode=0;//�Զ�������һ�����ݲɼ�ģʽ
    else Mag_Calibration_Mode=3;
  }
  
  
  
  if(Mag_Calibration_Mode==0)  Yaw_Correct=Circle_Angle_Calibartion.Yaw;
  else if(Mag_Calibration_Mode==1)  Yaw_Correct=Circle_Angle_Calibartion.Rol;
  else if(Mag_Calibration_Mode==2)  Yaw_Correct=Circle_Angle_Calibartion.Pit;
  else Yaw_Correct=0;
  
  
  
  if(Mag_Is_Okay_Flag[0]==1
     &&Mag_Is_Okay_Flag[1]==1
       &&Mag_Is_Okay_Flag[2]==1)//��������ȫ���ɼ���ϣ�������������
  {
    Mag.x_offset=(Mag.x_min+Mag.x_max)/2.0;
    Mag.y_offset=(Mag.y_min+Mag.y_max)/2.0;
    Mag.z_offset=(Mag.z_min+Mag.z_max)/2.0;
    Mag_Offset_Read.x=Mag.x_offset;
    Mag_Offset_Read.y=Mag.y_offset;
    Mag_Offset_Read.z=Mag.z_offset;
    Mag_Is_Okay_Flag[0]=0;
    Mag_Is_Okay_Flag[1]=0;
    Mag_Is_Okay_Flag[2]=0;
    Mag_Calibration_Flag=0;//������У׼�������ͷ�ң�в���
    Bling_Mode=0;//�ָ�����ָʾģʽ
    
    Mag_Offset[0]=(int16_t)(Mag_Offset_Read.x);
    Mag_Offset[1]=(int16_t)(Mag_Offset_Read.y);
    Mag_Offset[2]=(int16_t)(Mag_Offset_Read.z);
    
    Bling_Set(&Light_1,5000,1000,0.3,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
    Bling_Set(&Light_2,5000,1000,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
    Bling_Set(&Light_3,5000,1000,0.8,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
    
    
    //д����ٶ����ƫִ�����������ƫִ 
    WriteFlashParameter_Three(MAG_X_OFFSET,
                              Mag.x_offset,
                              Mag.y_offset,
                              Mag.z_offset,
                              &Table_Parameter);
    
    return TRUE;
  }
  return FALSE;
}


void Mag_LS_Init()
{
  LS_Init(&Mag_LS);
}
float mag_a,mag_b,mag_c,mag_r;
/***********************************************************
@��������Mag_Calibartion_LS
@��ڲ����������ǻ��ֽǶ�ֵ�����������ԭʼֵ
@���ڲ�������
@������������������С���˷�������桢����ң����ֱ�ӽ���
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
uint8_t Mag_Calibartion_LS(Vector3f *magdata,Vector3f_Body Circle_Angle_Calibartion)
{
  uint16 i=0;
  for(i=0;i<36;i++)
  {
    Last_Mag_360_Flag[0][i]=Mag_360_Flag[0][i];
    Last_Mag_360_Flag[1][i]=Mag_360_Flag[1][i];
    Last_Mag_360_Flag[2][i]=Mag_360_Flag[2][i];
  }
  
  /********��һ��Z�����������Ϸ���
  ��ʼ����ֱ����ת��Z axis is about 1g,X��Y is about 0g*/
  /********�ڶ���Y�����������Ϸ���
  ��ʼ����ֱ����ת��Y axis is about 1g,X��Z is about 0g*/
  if(Mag_Calibration_Mode<3)//��⵽��Ӧ������
  {
    for(i=0;i<36;i++)
    {
      if(ABS(Circle_Angle_Calibartion.Yaw-Mag_360_define[i])<=5.f
         &&Mag_Calibration_Mode==0
           &&WP_Sensor.accel_raw.z>=AcceMax_1G/2)//Z�������ֱ
      {
        Mag_360_Flag[0][i]=1;
      }
      
      if(ABS(Circle_Angle_Calibartion.Rol-Mag_360_define[i])<=5.0f
         &&Mag_Calibration_Mode==1
           &&WP_Sensor.accel_raw.y>=AcceMax_1G/2)//Y�������ֱ
      {
        Mag_360_Flag[1][i]=1;
      }
      
      if(ABS(Circle_Angle_Calibartion.Pit-Mag_360_define[i])<=5.0f
         &&Mag_Calibration_Mode==2
           &&WP_Sensor.accel_raw.x>=AcceMax_1G/2)//X�������ֱ
      {
        Mag_360_Flag[2][i]=1;
      }
    }
    
    for(i=0;i<36;i++)
    {
      if((Last_Mag_360_Flag[0][i]==0&&Mag_360_Flag[0][i]==1)
         ||(Last_Mag_360_Flag[1][i]==0&&Mag_360_Flag[1][i]==1)
           ||(Last_Mag_360_Flag[2][i]==0&&Mag_360_Flag[2][i]==1))
      {
        LS_Accumulate(&Mag_LS, magdata->x,magdata->y,magdata->z);
        //LS_Calculate(&Mag_LS,36*3,0.0f,&mag_a, &mag_b, &mag_c,&mag_r);
      }
    }
  }
  
  if(Check_Plane_Sampling_Okay(0))
  {
    Mag_Is_Okay_Flag[0]=1;//�������ݲɼ���ɱ�־λ��1
    for(i=0;i<36;i++)  Mag_360_Flag[0][i]=0;//��ղɼ��ǵ�
    if(Mag_Is_Okay_Flag[1]==0)//����һ������δ���
      Mag_Calibration_Mode=1;//�Զ�������һ�����ݲɼ�ģʽ
    else Mag_Calibration_Mode=3;//
  }
  
  if(Check_Plane_Sampling_Okay(1))
  {
    Mag_Is_Okay_Flag[1]=1;//�������ݲɼ���ɱ�־λ��1
    for(i=0;i<36;i++) Mag_360_Flag[1][i]=0;//��ղɼ��ǵ�
    if(Mag_Is_Okay_Flag[2]==0)//����һ������δ���
      Mag_Calibration_Mode=2;//�Զ�������һ�����ݲɼ�ģʽ
    else Mag_Calibration_Mode=3;
  }
  
  if(Check_Plane_Sampling_Okay(2))
  {
    Mag_Is_Okay_Flag[2]=1;//�������ݲɼ���ɱ�־λ��1
    for(i=0;i<36;i++) Mag_360_Flag[2][i]=0;//��ղɼ��ǵ�
    if(Mag_Is_Okay_Flag[0]==0)//����һ������δ���
      Mag_Calibration_Mode=0;//�Զ�������һ�����ݲɼ�ģʽ
    else Mag_Calibration_Mode=3;
  }
  
  if(Mag_Calibration_Mode==0)  			Yaw_Correct=Circle_Angle_Calibartion.Yaw;
  else if(Mag_Calibration_Mode==1)  Yaw_Correct=Circle_Angle_Calibartion.Rol;
  else if(Mag_Calibration_Mode==2)  Yaw_Correct=Circle_Angle_Calibartion.Pit;
  else Yaw_Correct=0;
  
  if(Mag_Is_Okay_Flag[0]==1
     &&Mag_Is_Okay_Flag[1]==1
       &&Mag_Is_Okay_Flag[2]==1)//��������ȫ���ɼ���ϣ�������������
  {
    LS_Calculate(&Mag_LS,36*3,0.0f,&mag_a, &mag_b, &mag_c,&mag_r);
    Mag_Offset_Read.x=mag_a;
    Mag_Offset_Read.y=mag_b;
    Mag_Offset_Read.z=mag_c;
    Mag_Is_Okay_Flag[0]=0;
    Mag_Is_Okay_Flag[1]=0;
    Mag_Is_Okay_Flag[2]=0;
    Mag_Calibration_Flag=0;//������У׼�������ͷ�ң�в���
    Bling_Mode=0;//�ָ�����ָʾģʽ
    
    Mag_Offset[0]=(int16_t)(Mag_Offset_Read.x);
    Mag_Offset[1]=(int16_t)(Mag_Offset_Read.y);
    Mag_Offset[2]=(int16_t)(Mag_Offset_Read.z);
    
    Bling_Set(&Light_1,5000,1000,0.3,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
    Bling_Set(&Light_2,5000,1000,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
    Bling_Set(&Light_3,5000,1000,0.8,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
    WriteFlashParameter_Three(MAG_X_OFFSET,
                              Mag_Offset_Read.x,
                              Mag_Offset_Read.y,
                              Mag_Offset_Read.z,
                              &Table_Parameter);
    return TRUE;
  }
  return FALSE;
}



#define  RC_TOP_DEFAULT       2000
#define  RC_BUTTOM_DEFAULT    1000
#define  RC_MIDDLE_DEFAULT    1500
#define  RC_DEADBAND_DEFAULT  100
#define  RC_DEADBAND_PERCENT  0.1f
#define  RC_RESET_DEFAULT  1500

Vector_RC  RC_Calibration[8]={0};
uint8_t RC_Read_Flag[8];
/***********************************************************
@��������RC_Calibration_Init
@��ڲ�������
@���ڲ�������
@����������ң�����г̱궨��ʼ��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void RC_Calibration_Init()
{
  uint16_t i=0;
  float max_read[8]={0},min_read[8]={0};
  for(i=0;i<8;i++)
  {
    ReadFlashParameterTwo(RC_CH1_MAX+2*i,&max_read[i],&min_read[i]);
    if(isnan(max_read[i])==0&&isnan(min_read[i])==0)  	
      RC_Read_Flag[i]=0x01;
  }
  
  if(RC_Read_Flag[0]!=0x00
     &&RC_Read_Flag[1]!=0x00
       &&RC_Read_Flag[2]!=0x00
         &&RC_Read_Flag[3]!=0x00
           &&RC_Read_Flag[4]!=0x00
             &&RC_Read_Flag[5]!=0x00
               &&RC_Read_Flag[6]!=0x00
                 &&RC_Read_Flag[7]!=0x00)//flash�д�������
  {
    for(i=0;i<8;i++)
    {
      RC_Calibration[i].max=max_read[i];
      RC_Calibration[i].min=min_read[i];
      RC_Calibration[i].middle=(float)((max_read[i]+min_read[i])/2.0f);
      RC_Calibration[i].deadband=(float)((max_read[i]-min_read[i])*RC_DEADBAND_PERCENT/1.0f);
    }
  }
  else//flash�в���������
  {
    for(i=0;i<8;i++)
    {
      RC_Calibration[i].max=RC_TOP_DEFAULT;
      RC_Calibration[i].min=RC_BUTTOM_DEFAULT;
      RC_Calibration[i].middle=RC_MIDDLE_DEFAULT;
      RC_Calibration[i].deadband=RC_DEADBAND_DEFAULT;
    }
  }
}
/***********************************************************
@��������RC_Calibration_RESET
@��ڲ�������
@���ڲ�������
@����������ң�����г̱궨��λ
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void RC_Calibration_RESET()
{
  uint16_t i=0;
  for(i=0;i<8;i++)
  {
    RC_Calibration[i].max=RC_RESET_DEFAULT;
    RC_Calibration[i].min=RC_RESET_DEFAULT;
  }
}


uint8_t RC_Calibration_Trigger_Flag=0;
/***********************************************************
@��������RC_Calibration_Trigger
@��ڲ�������
@���ڲ�������
@����������ң�����г̱궨����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void RC_Calibration_Trigger(void)
{
  if(QuadKey2==0)
  {
    delay_ms(500);
    if(QuadKey2==0)
    {
      while(QuadKey2==0);
      RC_Calibration_RESET();//��λң�����г�ֵ���ȴ�У׼���
      RC_Calibration_Trigger_Flag=1;
      Page_Number=13;
      Key_Right_Release=1;
    }
  }
  else
  {
    RC_Calibration_Init();//ֱ�Ӵ�flash���棨����DEFAULTֵ����ȡң�����г����
    RC_Calibration_Trigger_Flag=0;
  }
}

/***********************************************************
@��������RC_Calibration_Check
@��ڲ�����uint16 *rc_date
@���ڲ�������
@����������ң�����г�У׼��⣬�漰�ڲ�Flash����д����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
bool RC_Calibration_Check(uint16 *rc_date)
{
  uint16_t i=0;
  bool success_flag=FALSE;
  if(RC_Calibration_Trigger_Flag==0) return success_flag;
  for(i=0;i<8;i++)
  {
    if(rc_date[i] >= RC_Calibration[i].max)   RC_Calibration[i].max = rc_date[i];//����г�ֵ
    if(rc_date[i] <  RC_Calibration[i].min)   RC_Calibration[i].min = rc_date[i];//��С�г�ֵ
    RC_Calibration[i].middle=(float)((RC_Calibration[i].max+RC_Calibration[i].min)/2.0f);//�г���λ
    RC_Calibration[i].deadband=(float)((RC_Calibration[i].max-RC_Calibration[i].min)*RC_DEADBAND_PERCENT/1.0f);//���������̵İٷ�֮RC_DEADBAND_PERCENTΪ��λ����
  }
  
  if(QuadKey2==0)//ң�����궨��ɺ�ͨ�������������궨����
  {
    delay_ms(2000);
    if(QuadKey2==0)
    {
      while(QuadKey2==0);
      RC_Calibration_Trigger_Flag=0;
      Key_Right_Release=0;
      success_flag=TRUE;
      for(i=0;i<8;i=i+1)
      {
        WriteFlashParameter_Two(RC_CH1_MAX+2*i,
                                RC_Calibration[i].max,
                                RC_Calibration[i].min,
                                &Table_Parameter);
      }
    }
  }
  return success_flag;
}

/***********************************************************
@��������Reset_RC_Calibartion
@��ڲ�����uint8_t Type
@���ڲ�������
@����������ң�����г�У׼ǿ�Ƹ�λ
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Reset_RC_Calibartion(uint8_t Type)
{
  if(Type==1)  
  {
    RC_Calibration_Trigger_Flag=0;
    Key_Right_Release=0;
  }
}
uint16_t ESC_Calibration_Makesure_Cnt=0;
float ESC_Calibration_Flag=0;
void ESC_Calibration_Check(void)
{
  if(Throttle_Control==1000
     &&Yaw_Control>=Yaw_Max*Scale_Pecent_Max
       &&Roll_Control>=Pit_Rol_Max*Scale_Pecent_Max
         &&Pitch_Control<=-Pit_Rol_Max*Scale_Pecent_Max)
    ESC_Calibration_Makesure_Cnt++;
  //else ESC_Calibration_Makesure_Cnt/=2;
  
  if(Throttle_Control==1000
     &&Yaw_Control>=Yaw_Max*Scale_Pecent_Max
       &&Roll_Control>=Pit_Rol_Max*Scale_Pecent_Max
         &&Pitch_Control<=-Pit_Rol_Max*Scale_Pecent_Max
           &&ESC_Calibration_Makesure_Cnt>200*5//����5S
             &&Controler_State==Lock_Controler)//����Ϊ����״̬�ſ��Խ���У׼ģʽ
    //����ESCУ׼ģʽ
  {
    ESC_Calibration_Flag=1;
    ESC_Calibration_Makesure_Cnt=0;
    Forced_Lock_Makesure_Cnt=0;
    WriteFlashParameter(ESC_CALIBRATION_FLAG,
                        ESC_Calibration_Flag,
                        &Table_Parameter);
    Bling_Set(&Light_1,5000,500,0.2,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
    Bling_Set(&Light_2,5000,500,0.2,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
    Bling_Set(&Light_3,5000,500,0.2,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
    Page_Number=14;
  }
}

#define Thr_Chl_Num  2
/***********************************************************
@��������ESC_Calibration
@��ڲ�������
@���ڲ�������
@��������������г�У׼
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ESC_Calibration(void)
{
  PWM_Output(1.25*PPM_Databuf[Thr_Chl_Num],1.25*PPM_Databuf[Thr_Chl_Num],1.25*PPM_Databuf[Thr_Chl_Num],1.25*PPM_Databuf[Thr_Chl_Num],
             1.25*PPM_Databuf[Thr_Chl_Num],1.25*PPM_Databuf[Thr_Chl_Num],1.25*PPM_Databuf[Thr_Chl_Num],1.25*PPM_Databuf[Thr_Chl_Num]);
}

/***********************************************************
@��������Check_Calibration_Flag
@��ڲ�������
@���ڲ�������
@������������ǰУ׼ģʽ���
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
uint8_t Check_Calibration_Flag(void)
{
  uint8_t cal_flag=0x00; 
  if(Key_Right_Release==1)      cal_flag|=0x01;//ң����У׼
  if(Accel_Calibration_Flag==1) cal_flag|=0x02;//���ٶȼ�У׼
  if(Mag_Calibration_Flag==1)   cal_flag|=0x04;//������У׼
  return cal_flag;
}

float Pitch_Offset=0,Roll_Offset=0;
float Hor_Accel_Offset[3]={0};
uint16_t Horizontal_Calibration_Makesure_Cnt=0;
/***********************************************************
@��������Horizontal_Calibration_Check
@��ڲ�������
@���ڲ�������
@��������������ˮƽУ׼
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Horizontal_Calibration_Check(void)
{
  float acce_sample_sum[3]={0,0,0};//���ٶȺ�����
  if(Throttle_Control==1000&&Yaw_Control>=Yaw_Max*Scale_Pecent_Max&&Roll_Control<=-Pit_Rol_Max*Scale_Pecent_Max&&Pitch_Control<=-Pit_Rol_Max*Scale_Pecent_Max)
  {
    Horizontal_Calibration_Makesure_Cnt++;
  }
  if(Throttle_Control==1000
     &&Yaw_Control>=Yaw_Max*Scale_Pecent_Max
       &&Roll_Control<=-Pit_Rol_Max*Scale_Pecent_Max
         &&Pitch_Control<=-Pit_Rol_Max*Scale_Pecent_Max
           &&Horizontal_Calibration_Makesure_Cnt>=200*5//��������
             &&Controler_State==Lock_Controler)//����Ϊ����״̬�ſ��Խ���У׼ģʽ
  {
    Pitch_Offset=Pitch;
    Roll_Offset=Roll;
    uint16_t num_samples=0;
    while(num_samples<200)//����200��
    {
      if(Gyro_Length<=20.0f)//ͨ��������ģ����ȷ�����徲ֹ
      {
        for(int16_t j=0;j<3;j++){
          acce_sample_sum[j]+=WP_Sensor.acce_filter[j];//���ٶȼ�ת��Ϊ1g������
        }
        delay_ms(5);//���10ms��1s������ȡƽ��
        num_samples++;
      }
    }
    Hor_Accel_Offset[0]=acce_sample_sum[0]/num_samples; //�����Ӧ��ļ��ٶȼ���
    Hor_Accel_Offset[1]=acce_sample_sum[1]/num_samples; //�����Ӧ��ļ��ٶȼ���
    Hor_Accel_Offset[2]=acce_sample_sum[2]/num_samples; //�����Ӧ��ļ��ٶȼ���
    
    Horizontal_Calibration_Makesure_Cnt=0;
    WriteFlashParameter(PITCH_OFFSET,Pitch_Offset,&Table_Parameter);
    WriteFlashParameter(ROLL_OFFSET,Roll_Offset,&Table_Parameter);
    Bling_Set(&Light_1,500,100,0.2,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
    Bling_Set(&Light_2,500,100,0.2,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
    Bling_Set(&Light_3,500,100,0.2,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
  }
}

/***********************************************************
@��������Horizontal_Calibration_Init
@��ڲ�������
@���ڲ�������
@��������������ˮƽУ׼��ʼ��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Horizontal_Calibration_Init(void)
{
  float pitch_offset_temp=0,roll_offset_temp=0;	
  ReadFlashParameterOne(PITCH_OFFSET,&pitch_offset_temp);
  ReadFlashParameterOne(ROLL_OFFSET,&roll_offset_temp);
  
  if(isnan(pitch_offset_temp)==0)   Pitch_Offset=pitch_offset_temp;
  if(isnan(roll_offset_temp)==0)    Roll_Offset=roll_offset_temp;
}


uint16_t Headless_Mode_Makesure_Cnt=0;
float Headless_Mode_Yaw=0.0f;
/***********************************************************
@��������Headless_Mode_Calibration_Check
@��ڲ�������
@���ڲ�������
@������������ͷģʽ��ʼƫ���ǻ�ȡ
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Headless_Mode_Calibration_Check(void)
{
  if(Throttle_Control==1000
     &&Yaw_Control>=Yaw_Max*Scale_Pecent_Max
       &&Roll_Control<=-Pit_Rol_Max*Scale_Pecent_Max
         &&Pitch_Control==0)
  {
    Headless_Mode_Makesure_Cnt++;
  }
  if(Throttle_Control==1000
     &&Yaw_Control>=Yaw_Max*Scale_Pecent_Max
       &&Roll_Control<=-Pit_Rol_Max*Scale_Pecent_Max
         &&Pitch_Control==0
           &&Headless_Mode_Makesure_Cnt>=200*5//��������
             &&Controler_State==Lock_Controler)//����Ϊ����״̬�ſ��Խ���У׼ģʽ
  {    
    Headless_Mode_Yaw=Yaw;
    Horizontal_Calibration_Makesure_Cnt=0;
    WriteFlashParameter(HEADLESS_MODE_YAW,Headless_Mode_Yaw,&Table_Parameter);
    Bling_Set(&Light_1,500,100,0.2,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
    Bling_Set(&Light_2,500,100,0.2,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
    Bling_Set(&Light_3,500,100,0.2,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
  }
}

uint8_t Gyro_Safety_Calibration_Flag=0;
uint8_t Gyro_Calibration_Check(vector3f *gyro)
{
	static vector3f offset;
	static uint16_t offset_cnt=0;
  static vector3f last_gyro;
   if(gyro->x-last_gyro.x<=50
		   &&gyro->y-last_gyro.y<=50
	      &&gyro->z-last_gyro.z<=50)
	{
	  offset.x+=gyro->x;
		offset.y+=gyro->y;
		offset.z+=gyro->z;
		offset_cnt++;
	}
  else
	{
		offset.x=0;
		offset.y=0;
		offset.z=0;
		offset_cnt=0;
	}
  last_gyro.x=gyro->x;
	last_gyro.y=gyro->y;
	last_gyro.z=gyro->z;
	
	if(offset_cnt>=200)//��������1s
	{
		gyro_offset.x =(offset.x/offset_cnt);//�õ��궨ƫ��
		gyro_offset.y =(offset.y/offset_cnt);
		gyro_offset.z =(offset.z/offset_cnt);
		Euler_Angle_Init_Again();
		Bling_Set(&Light_1,2000,100,0.2,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
		Bling_Set(&Light_2,2000,100,0.2,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
		Bling_Set(&Light_3,2000,100,0.2,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
	  return 1;
	}
	return 0;
}

/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/
