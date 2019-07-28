/**
  ******************************************************************************
  * Copyright (c) 2018,�����пƺƵ�Ƽ����޹�˾
  * All rights reserved.
  * �ļ����ƣ�gcs.h
  * ժ    Ҫ��
  *
  * ��ǰ�汾��V1.0
  * ��    �ߣ������пƺƵ�Ƽ����޹�˾�з��� 
  * ������ڣ�    
  * �޸�˵����
  * 
  *
  * ��ʷ�汾��
  *
  *
  *******************************************************************************/
#ifndef _GCS_H
#define _GCS_H
//�ⲿ�ļ�����
#include "include.h"
#include "ZKHD_Link.h"
#include "nrf24l01.h"
#include "height_control.h"
#include "remote.h"
#include "imu.h"
#include "battery.h"
#include "control.h"

//�궨����
#define RATE_5HZ        (5)
#define RATE_10HZ       (10)
#define RATE_50HZ       (50)
#define RATE_100HZ      (100)

#define ISRATE(hz)      (Cnt % hz == 0)

//���ݽṹ����
typedef enum
{
    Stabilize_Mode = 0,
    Altitude_Hold,
    AutoLand,
    
    Num_Of_Mode,
}Mode_t;

typedef enum
{
    Connect_Port_USB = 0,
    Connect_Port_NRF,
    
    Num_Connect_Port,
}Connect_Port_t;

typedef enum
{
    MsgID_0_Set_PID = 0,
    MsgID_1_Select_PID,
    MsgID_2_Set_RxAddr,
    MsgID_3_Set_RxFreq,
    MsgID_4_Set_TxAddr,
    
    Num_Msg,
}emMsg_GCS_TO_FMU_t;

typedef struct 
{
    int8_t PID_Select;
    int8_t PID_Pos_Rate;
    int8_t PID_Offset;
    int8_t PID_Set;
}PID_Config_t;

typedef struct 
{
    uint32_t            Runtime;                           //�������ϵ�ʱ��      ��λ��s
    uint8_t             Firmware_Ver;                      //�̼��汾
    Mode_t              UAV_Mode;                          //��ǰ����ģʽ        0������ 1������
    int16_t             *Motor;                            //������ָ��
    Connect_Port_t      Connect_Port;                      //���Ӷ˿�            0:USB     1:NRF
    NRF24L01_Manager_t  *NRF_MannagerPtr;                  //NRF24L01����ָ��
    HeightInfo_t        *Alt;                              //�߶���Ϣָ��
    MPU6050Manager_t    *Mpu6050;                          //����������ָ��
    Remote_t            *Remote;                           //ң��������ָ��
    Attitude_t          *AngE;                             //��̬����ָ��
    FMUflg_t            *FMUflg;
    BatteryInfo_t       *BatteryInfoPtr;
}UAV_info_t;

//Extern����
extern UAV_info_t g_UAVinfo;


//��ʼ���ʹ�����
void PollingGCS(void);
void gcs_init(void);
void gcs_ReceiveHandle(ZKHD_Link_Head_t *ZKHD_Link_Info, uint8_t *ptr, uint8_t length);
void UpdateUSBQueue(void);

//���ݸ��º���
void UpdateToGCSPidInfo(void);
void UpdateToGCSAttitude(void);
void UpdateToGCSAltitude(void);
void UpdateToGCSMPU6050(void);
void UpdateToGCSMotor(void);
void UpdateToGCSHardwareInfo(void);
void UpdateToGCSFreq(void);
void UpdateToGCSLine(int16_t l1, int16_t l2, int16_t l3, int16_t l4, int16_t l5, int16_t l6, int16_t l7, int16_t l8);
void UpdateToGCSLine2(int16_t l1, int16_t l2, int16_t l3, int16_t l4, int16_t l5, int16_t l6, int16_t l7, int16_t l8);
#endif

/******************* (C) ��Ȩ���� 2018 �����пƺƵ�Ƽ����޹�˾ *******************/
