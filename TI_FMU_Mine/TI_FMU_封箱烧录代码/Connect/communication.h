/**
  ******************************************************************************
  * Copyright (c) 2018,�����пƺƵ�Ƽ����޹�˾
  * All rights reserved.
  * �ļ����ƣ�communication.h
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
#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H
//�ⲿ�ļ�����
#include "include.h"
#include "gcs.h"
#include "nrf24l01.h"


//�궨����



//���ݽṹ����
typedef enum
{
    Rx_Mode = 0,
    Tx_Mode,
    Unknow_Mode,
}mode_t;

typedef struct
{
    uint16_t Tx_Success_Count;
    uint16_t Tx_Fail_Count;
    float Tx_Packet_Loss_Rate;
    
    uint16_t Rx_Success_Count;
}communication_info_t;

typedef struct
{
    NRF24L01_Manager_t *Hardware_Mannager;
    Queue_t qTx;
    Queue_t qRx;
    mode_t mode;
    communication_info_t communication_info;
}NRF_Mannager_t;


//Extern����
extern NRF_Mannager_t NRF_Mannager;


//��������
void NRF_Radio_Init(void);
void NRF_Radio_Send(uint8_t *ptr,uint8_t length);
void PollingNRF(void);
void SwitchPort(uint8_t Connect_Port,uint8_t *ptr);
#endif

/******************* (C) ��Ȩ���� 2018 �����пƺƵ�Ƽ����޹�˾ *******************/
