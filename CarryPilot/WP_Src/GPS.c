
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
#include "GPS.h"
#include "Usart.h"
#include <math.h>
#include <string.h>

uint8_t GPS_Update_Flag;
double Last_Longitude=0,Last_Latitude=0;
int32_t Longitude_Origion=0,Latitude_Origion=0;//PVT����������ԭʼ��γ�ȣ��൱������deg*10^7
double Longitude,Latitude;
double Longitude_Deg,Latitude_Deg,Longitude_Min,Latitude_Min;
float GPS_Ground_Speed=0;//�Ե��ٶ�
float GPS_Yaw=0;//�˶�����
float GPS_Quality=0;//��λ����
uint16 GPS_Sate_Num=0,Last_GPS_Sate_Num=0;//���붨λ������
float GPS_Speed_Resolve[2]={0,0};//GPS�۲��ٶ�
u16 TimeBeijing[3];//����ʱ��
char TimeStr[8];
Vector3_Nav Last_GPS_Vel={0};//NED���ٶ�
Vector3_Nav GPS_Vel={0};//NED���ٶ�
Vector3_Nav GPS_Vel_Div={0};//GPS���NED���ٶ�΢�֣����ٶȣ�
float GPS_Pos_DOP=0;//��λ��������//��λ����
uint8 GPS_FixType=0;//��λ����
uint8 GPS_Fix_Flag[4]={0};//��λ��־
float Horizontal_Acc_Est=0;//ˮƽλ�þ���
int16 Vertical_Acc_Est=0;//��ֱλ�þ���
int16 Speed_Acc_Est=0;//��ֱλ�þ���
float High_GPS=0;//���θ߶�
Testime GPS_PVT_Parse_Time_Delta;
uint16_t GPS_Data_Offset=0;
#define Ublox_Data COM2_Rx_Buf.Ring_Buff
/***********************************************************
@��������GPS_PVT_Parse
@��ڲ�������
@���ڲ�������
����������GPS���ݽ��������M8N�����ֲ�
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void GPS_PVT_Parse()
{
  Test_Period(&GPS_PVT_Parse_Time_Delta);//GPS���ݽ������ڲ���
  Last_Longitude=Longitude;
  Last_Latitude=Latitude;
  
  Last_GPS_Vel.E=GPS_Vel.E;
  Last_GPS_Vel.N=GPS_Vel.N;
  Last_GPS_Vel.U=GPS_Vel.U;
  //����ʱ��
  TimeBeijing[0]=Ublox_Data[GPS_Data_Offset+8]+8;//ʱ
  TimeBeijing[1]=Ublox_Data[GPS_Data_Offset+9];//��
  TimeBeijing[2]=Ublox_Data[GPS_Data_Offset+10];//��
  
  GPS_FixType=Ublox_Data[GPS_Data_Offset+20];//��λ����
  
  GPS_Fix_Flag[0]=Ublox_Data[GPS_Data_Offset+21]&0x01;//�Ƿ���Ч��λ
  GPS_Fix_Flag[1]=(Ublox_Data[GPS_Data_Offset+21]&0x02)>>1;//���ģʽ�Ƿ���(DGPS)
  GPS_Fix_Flag[2]=(Ublox_Data[GPS_Data_Offset+21]&0x3A)>>2;//ʡ��ģʽ��δ��
  GPS_Fix_Flag[3]=Ublox_Data[GPS_Data_Offset+21]&0x20;//�����Ƿ���Ч
  Last_GPS_Sate_Num=GPS_Sate_Num;
  GPS_Sate_Num=Ublox_Data[GPS_Data_Offset+23];//���붨λ���Ǹ���
  
  Longitude_Origion=Ublox_Data[GPS_Data_Offset+24]//����*10^7
    +(Ublox_Data[GPS_Data_Offset+25]<<8)
      +(Ublox_Data[GPS_Data_Offset+26]<<16)
        +(Ublox_Data[GPS_Data_Offset+27]<<24);
  Longitude=Longitude_Origion*0.0000001f;//deg
  
  Latitude_Origion=Ublox_Data[GPS_Data_Offset+28]//γ��*10^7
    +(Ublox_Data[GPS_Data_Offset+29]<<8)
      +(Ublox_Data[GPS_Data_Offset+30]<<16)
        +(Ublox_Data[GPS_Data_Offset+31]<<24);
  Latitude=Latitude_Origion*0.0000001f;//deg
  
  
  Longitude_Deg=(int)(Longitude);//��γ�ȴ�������OLED��ʾ
  Longitude_Min=((int)((Longitude-Longitude_Deg)*10000000));
  Latitude_Deg=(int)(Latitude);
  Latitude_Min=((int)((Latitude-Latitude_Deg)*10000000));
  
  
  High_GPS=Ublox_Data[GPS_Data_Offset+36]//GPS��ȡ���θ߶�
    +(Ublox_Data[GPS_Data_Offset+37]<<8)
      +(Ublox_Data[GPS_Data_Offset+38]<<16)
        +(Ublox_Data[GPS_Data_Offset+39]<<24);
  High_GPS/=1000;//m
  
  
  Horizontal_Acc_Est=Ublox_Data[GPS_Data_Offset+40]//ˮƽλ�ù��ƾ���
    +(Ublox_Data[GPS_Data_Offset+41]<<8)
      +(Ublox_Data[GPS_Data_Offset+42]<<16)
        +(Ublox_Data[GPS_Data_Offset+43]<<24);
        Horizontal_Acc_Est*=0.01f;//m
        
        
        Vertical_Acc_Est=Ublox_Data[GPS_Data_Offset+44]//��ֱλ�ù��ƾ���
          +(Ublox_Data[GPS_Data_Offset+45]<<8)
            +(Ublox_Data[GPS_Data_Offset+46]<<16)
              +(Ublox_Data[GPS_Data_Offset+47]<<24);;
              Vertical_Acc_Est*=0.01f;//m
              
              
              GPS_Vel.N=Ublox_Data[GPS_Data_Offset+48]//GPS��ȡ���ص���ϵ�����ٶ�
                +(Ublox_Data[GPS_Data_Offset+49]<<8)
                  +(Ublox_Data[GPS_Data_Offset+50]<<16)
                    +(Ublox_Data[GPS_Data_Offset+51]<<24);
              GPS_Vel.N/=10;//cm/s  N
              
              
              GPS_Vel.E=Ublox_Data[GPS_Data_Offset+52]//GPS��ȡ���ص���ϵ�������ٶ�
                +(Ublox_Data[GPS_Data_Offset+53]<<8)
                  +(Ublox_Data[GPS_Data_Offset+54]<<16)
                    +(Ublox_Data[GPS_Data_Offset+55]<<24);//mm/s
              GPS_Vel.E/=10;//cm/s  E
              
              
              GPS_Vel.U=Ublox_Data[GPS_Data_Offset+56]//GPS��ȡ���ص���ϵ '��' ���ٶ�
                +(Ublox_Data[GPS_Data_Offset+56]<<8)
                  +(Ublox_Data[GPS_Data_Offset+58]<<16)
                    +(Ublox_Data[GPS_Data_Offset+59]<<24);
              GPS_Vel.U/=(-10);//cm/s  D
              
              GPS_Speed_Resolve[0]=GPS_Vel.N;//Y  Axis
              GPS_Speed_Resolve[1]=GPS_Vel.E;//X  Axis
              
              
              GPS_Ground_Speed=Ublox_Data[60]//����Ե��ٶ�
                +(Ublox_Data[GPS_Data_Offset+61]<<8)
                  +(Ublox_Data[GPS_Data_Offset+62]<<16)
                    +(Ublox_Data[GPS_Data_Offset+63]<<24);
              GPS_Ground_Speed/=10;//cm/s
              
              
              GPS_Yaw=Ublox_Data[GPS_Data_Offset+64]//�����˶������
                +(Ublox_Data[GPS_Data_Offset+65]<<8)
                  +(Ublox_Data[GPS_Data_Offset+66]<<16)
                    +(Ublox_Data[GPS_Data_Offset+67]<<24);
              GPS_Yaw*=0.00001f;//deg
              
              Speed_Acc_Est=Ublox_Data[GPS_Data_Offset+68]//�ٶȹ��ƾ���
                +(Ublox_Data[GPS_Data_Offset+69]<<8)
                  +(Ublox_Data[GPS_Data_Offset+70]<<16)
                    +(Ublox_Data[GPS_Data_Offset+71]<<24);
              Speed_Acc_Est*=0.1f;//cm/s
              
              
              GPS_Pos_DOP=Ublox_Data[GPS_Data_Offset+76]
                +(Ublox_Data[GPS_Data_Offset+77]<<8);
              GPS_Pos_DOP*=0.01f;
              GPS_Quality=GPS_Pos_DOP;//λ�þ�������
              
              GPS_Vel_Div.E=1000*(GPS_Vel.E-Last_GPS_Vel.E)/(GPS_PVT_Parse_Time_Delta.Time_Delta);//��λcm/s^2
              GPS_Vel_Div.N=1000*(GPS_Vel.N-Last_GPS_Vel.N)/(GPS_PVT_Parse_Time_Delta.Time_Delta);//��λcm/s^2
              GPS_Vel_Div.U=1000*(GPS_Vel.U-Last_GPS_Vel.U)/(GPS_PVT_Parse_Time_Delta.Time_Delta);//��λcm/s^2
}


/***********************************************************
@��������GPS_Data_Prase
@��ڲ�������
@���ڲ�������
����������GPS���ݽ���״̬��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void GPS_Data_Prase(void)
{
  static uint16_t gps_prase_cnt=0;
  gps_prase_cnt++;
  if(gps_prase_cnt>=WP_Duty_Freq/10)//100ms
  {
    if(COM2_Rx_Buf.Tail<99)//0-99����λ���ڴ���
    {
      GPS_Data_Offset=106;
    }
    else//100-199����λ���ڴ���
    {
      GPS_Data_Offset=6;
    }
    GPS_PVT_Parse();
    Set_GPS_Home();
    GPS_Update_Flag=1;
    gps_prase_cnt=0;
  }
}
const uint8_t NEMA_Disable_Header[6]={0xB5,0x62,0x06,0x01,0x03,0x00};//
const uint8_t NEMA_Disable_Buf[20][5]={
  0xF0,0x0A,0x00,0x04,0x23,
  0xF0,0x09,0x00,0x03,0x21,
  0xF0,0x00,0x00,0xFA,0x0F,
  0xF0,0x01,0x00,0xFB,0x11,
  0xF0,0x0D,0x00,0x07,0x29,
  0xF0,0x06,0x00,0x00,0x1B,
  0xF0,0x02,0x00,0xFC,0x13,
  0xF0,0x07,0x00,0x01,0x1D,
  0xF0,0x03,0x00,0xFD,0x15,
  0xF0,0x04,0x00,0xFE,0x17,
  0xF0,0x0E,0x00,0x08,0x2B,
  0xF0,0x0F,0x00,0x09,0x2D,
  0xF0,0x05,0x00,0xFF,0x19,
  0xF0,0x08,0x00,0x02,0x1F,//7
  0xF1,0x00,0x00,0xFB,0x12,
  0xF1,0x01,0x00,0xFC,0x14,
  0xF1,0x03,0x00,0xFE,0x18,
  0xF1,0x04,0x00,0xFF,0x1A,
  0xF1,0x05,0x00,0x00,0x1C,
  0xF1,0x06,0x00,0x01,0x1E,
};
const uint8_t Ublox_Disable_Header[6]={0xB5,0x62,0x06,0x01,0x03,0x00};//
const uint8_t Ublox_Disable_Buf[61][5]={
  0x0B ,0x30 ,0x00 ,0x45 ,0xC0,
  0x0B ,0x50 ,0x00 ,0x65 ,0x00,
  0x0B ,0x33 ,0x00 ,0x48 ,0xC6,
  0x0B ,0x31 ,0x00 ,0x46 ,0xC2,
  0x0B ,0x00 ,0x00 ,0x15 ,0x60,
  0x10 ,0x02 ,0x00 ,0x1C ,0x73,
  0x10 ,0x02 ,0x00 ,0x1C ,0x73,
  0x10 ,0x02 ,0x00 ,0x1C ,0x73,
  0x10 ,0x10 ,0x00 ,0x2A ,0x8F,
  0x21 ,0x0E ,0x00 ,0x39 ,0xBE,
  0x21 ,0x08 ,0x00 ,0x33 ,0xB2,
  0x21 ,0x09 ,0x00 ,0x34 ,0xB4,
  0x21 ,0x0B ,0x00 ,0x36 ,0xB8,
  0x21 ,0x0F ,0x00 ,0x3A ,0xC0,
  0x21 ,0x0D ,0x00 ,0x38 ,0xBC,
  0x0A ,0x05 ,0x00 ,0x19 ,0x67,
  0x0A ,0x09 ,0x00 ,0x1D ,0x6F,
  0x0A ,0x0B ,0x00 ,0x1F ,0x73,
  0x0A ,0x02 ,0x00 ,0x16 ,0x61,
  0x0A ,0x06 ,0x00 ,0x1A ,0x69,
  0x0A ,0x07 ,0x00 ,0x1B ,0x6B,
  0x0A ,0x21 ,0x00 ,0x35 ,0x9F,
  0x0A ,0x2E ,0x00 ,0x42 ,0xB9,
  0x0A ,0x08 ,0x00 ,0x1C ,0x6D,
  0x01 ,0x60 ,0x00 ,0x6B ,0x02,
  0x01 ,0x22 ,0x00 ,0x2D ,0x86,
  0x01 ,0x31 ,0x00 ,0x3C ,0xA4,
  0x01 ,0x04 ,0x00 ,0x0F ,0x4A,
  0x01 ,0x40 ,0x00 ,0x4B ,0xC2,
  0x01 ,0x61 ,0x00 ,0x6C ,0x04,
  0x01 ,0x09 ,0x00 ,0x14 ,0x54,
  0x01 ,0x34 ,0x00 ,0x3F ,0xAA,
  0x01 ,0x01 ,0x00 ,0x0C ,0x44,
  0x01 ,0x02 ,0x00 ,0x0D ,0x46,
  0x01 ,0x07 ,0x00 ,0x12 ,0x50,
  0x01 ,0x35 ,0x00 ,0x40 ,0xAC,
  0x01 ,0x32 ,0x00 ,0x3D ,0xA6,
  0x01 ,0x06 ,0x00 ,0x11 ,0x4E,
  0x01 ,0x03 ,0x00 ,0x0E ,0x48,
  0x01 ,0x30 ,0x00 ,0x3B ,0xA2,
  0x01 ,0x24 ,0x00 ,0x2F ,0x8A,
  0x01 ,0x23 ,0x00 ,0x2E ,0x88,
  0x01 ,0x20 ,0x00 ,0x2B ,0x82,
  0x01 ,0x21 ,0x00 ,0x2C ,0x84,
  0x01 ,0x11 ,0x00 ,0x1C ,0x64,
  0x01 ,0x12 ,0x00 ,0x1D ,0x66,
  0x02 ,0x30 ,0x00 ,0x3C ,0xA5,
  0x02 ,0x31 ,0x00 ,0x3D ,0xA7,
  0x02 ,0x10 ,0x00 ,0x1C ,0x65,
  0x02 ,0x15 ,0x00 ,0x21 ,0x6F,
  0x02 ,0x11 ,0x00 ,0x1D ,0x67,
  0x02 ,0x13 ,0x00 ,0x1F ,0x6B,
  0x02 ,0x20 ,0x00 ,0x2C ,0x85,
  0x0D ,0x11 ,0x00 ,0x28 ,0x88,
  0x0D ,0x16 ,0x00 ,0x2D ,0x92,
  0x0D ,0x13 ,0x00 ,0x2A ,0x8C,
  0x0D ,0x04 ,0x00 ,0x1B ,0x6E,
  0x0D ,0x03 ,0x00 ,0x1A ,0x6C,
  0x0D ,0x12 ,0x00 ,0x29 ,0x8A,
  0x0D ,0x01 ,0x00 ,0x18 ,0x68,
  0x0D ,0x06 ,0x00 ,0x1D ,0x72
};

const uint8_t PVT_Enable_Buf[11]={0xB5,0x62,0x06,0x01,0x03,0x00,0x01,0x07,0x01,0x13,0x51};
//const uint8_t Output_Rate_10hz_2[14]={0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x00,0x00,0x79,0x10};
const uint8_t Output_Rate_10hz_2[14]={0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12};
const uint8_t Output_Rate_10hz_1[8]={0xB5,0x62,0x06,0x08,0x00,0x00,0x0E,0x30};


//38400
//B5 62 06 00 14 00 01 00 00 00 D0 08 00 00 00 96 00 00 01 00 01 00 00 00 00 00 8B 54
//B5 62 06 00 01 00 01 08 22
const uint8_t Output_Baud_38400_2[28]={0xB5 ,0x62 ,0x06 ,0x00 ,0x14 ,0x00 ,0x01 ,0x00 ,0x00 ,
0x00 ,0xD0 ,0x08 ,0x00 ,0x00 ,0x00 ,0x96 ,0x00 ,0x00 ,
0x01 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x8B ,0x54};
const uint8_t Output_Baud_38400_1[9]={0xB5 ,0x62 ,0x06 ,0x00 ,0x01 ,0x00 ,0x01 ,0x08 ,0x22};
//115200
//B5 62 06 00 14 00 01 00 00 00 D0 08 00 00 00 C2 01 00 01 00 01 00 00 00 00 00 B8 42
//B5 62 06 00 01 00 01 08 22
const uint8_t Output_Baud_115200_2[28]={0xB5 ,0x62 ,0x06 ,0x00 ,0x14 ,0x00 ,0x01 ,0x00 ,0x00 ,
0x00 ,0xD0 ,0x08 ,0x00 ,0x00 ,0x00 ,0xC2 ,0x01 ,0x00 ,
0x01 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xB8 ,0x42};
const uint8_t Output_Baud_115200_1[9]={0xB5 ,0x62 ,0x06 ,0x00 ,0x01 ,0x00 ,0x01 ,0x08 ,0x22};
//921600
//B5 62 06 00 14 00 01 00 00 00 D0 08 00 00 00 10 0E 00 01 00 01 00 00 00 00 00 13 1E
//B5 62 06 00 01 00 01 08 22
const uint8_t Output_Baud_921600_2[28]={0xB5 ,0x62 ,0x06 ,0x00 ,0x14 ,0x00 ,0x01 ,0x00 ,0x00 ,
0x00 ,0xD0 ,0x08 ,0x00 ,0x00 ,0x00 ,0x10 ,0x0E ,0x00 ,
0x01 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x13 ,0x1E};
const uint8_t Output_Baud_921600_1[9]={0xB5 ,0x62 ,0x06 ,0x00 ,0x01 ,0x00 ,0x01 ,0x08 ,0x22};



const uint8_t GPS_Save_Config[21]={0xB5,0x62,0x06,0x09,0x0D,0x00,0x00,0x00,0x00,
0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x17,0x31,0xBF};



#define Ublox_Set_By_NamelessCotrunQuad//���GPS����FLash��ֻ��Ҫ����һ�κ�ע�ͱ��м���
#define Default_Baud_Is_Know  38400//Ĭ�ϲ�������֪ʱ����ʡ��ʼ����ѯʱ��
const unsigned long NCQ_Prase_Baud=921600;

const unsigned long Baud_Poll[8]={9600,19200,38400,
57600,115200,230400,460800,921600};
void Set_GPS_USART(void)
{
#ifdef  Ublox_Set_By_NamelessCotrunQuad
#ifdef Default_Baud_Is_Know
  Ublox_Set_Output_PVT_10hz_Baud_Set(Default_Baud_Is_Know);//��Ĭ�ϲ���������һ�μ���
#else
  for(uint16_t i=0;i<8;i++)
  {
    Ublox_Set_Output_PVT_10hz_Baud_Set(Baud_Poll[i]);//δ֪��������ѯ����
  }
#endif
#endif
}


/***********************************************************
@��������USART2_Init
@��ڲ�����unsigned long bound
@���ڲ�������
����������GPS���ݽ������ڳ�ʼ��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void USART2_Init(unsigned long bound)
{
  ConfigureUART2(bound);
}

/***********************************************************
@��������Ublox_Set_Output_PVT_10hz_Baud_Set
@��ڲ�����unsigned long Baud
@���ڲ�������
��������������GPSģ�鹤��ģʽ
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Ublox_Set_Output_PVT_10hz_Baud_Set(unsigned long Baud)
{
  uint16_t i=0,j=0;
  USART2_Init(Baud);
  for(i=0;i<20;i++)//��������NEMA������
  {
    for(j=0;j<3;j++)
    {
      USART2_Send((unsigned char *)NEMA_Disable_Header,sizeof(NEMA_Disable_Header));
      USART2_Send((unsigned char *)NEMA_Disable_Buf[i],5);
    }
    //delay_ms(50);
  }
  delay_ms(10);
  for(i=0;i<61;i++)//��������ublox������
  {
    for(j=0;j<3;j++)
    {
      USART2_Send((unsigned char *)Ublox_Disable_Header,sizeof(Ublox_Disable_Header));
      USART2_Send((unsigned char *)Ublox_Disable_Buf[i],5);
    }
    //delay_ms(10);
  }
  delay_ms(10);
  for(j=0;j<3;j++)
  {
    USART2_Send((unsigned char *)PVT_Enable_Buf,11);//��PVT
    //delay_ms(5);
  }
  delay_ms(10);
  for(j=0;j<5;j++)
  {
    USART2_Send((unsigned char *)Output_Rate_10hz_2,14);//����10hzˢ��1
    USART2_Send((unsigned char *)Output_Rate_10hz_1,8);//����10hzˢ��2
    //delay_ms(10);
  }
  delay_ms(10);
  if(NCQ_Prase_Baud==921600)
  {
    for(j=0;j<5;j++)
    {
      USART2_Send((unsigned char *)Output_Baud_921600_2,28);
      USART2_Send((unsigned char *)Output_Baud_921600_1,9);
      //delay_ms(10);
    }
  }
  else if(NCQ_Prase_Baud==115200)
  {
    for(j=0;j<5;j++)
    {
      USART2_Send((unsigned char *)Output_Baud_115200_2,28);
      USART2_Send((unsigned char *)Output_Baud_115200_1,9);
      //delay_ms(10);
    }
  }
  else
  {
    for(j=0;j<5;j++)
    {
      USART2_Send((unsigned char *)Output_Baud_38400_2,28);
      USART2_Send((unsigned char *)Output_Baud_38400_1,9);
      //delay_ms(10);
    }
  }
  USART2_Init(NCQ_Prase_Baud);
  delay_ms(10);
  for(j=0;j<5;j++)
  {
    USART2_Send((unsigned char *)GPS_Save_Config,21);
    //delay_ms(10);
  }
}


/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/
