/**
  ******************************************************************************
  * Copyright (c) 2018,�����пƺƵ�Ƽ����޹�˾
  * All rights reserved.
  * �ļ����ƣ�ZKHD_Link.c
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

/*==============================================================================
                         ##### How to use this driver #####
==============================================================================



*/
//�ⲿ�ļ�����
#include "ZKHD_Link.h"
#include "string.h"
#include "gcs.h"


//�궨����



//Extern����



//˽�к�����
void ZKHD_Link_GCS_To_Remote_Handle(ZKHD_Link_Head_t *,uint8_t *ptr, uint8_t length);
void ZKHD_Link_GCS_To_FMU_Handle(ZKHD_Link_Head_t *,uint8_t *ptr, uint8_t length);


//˽�б�����

/******************************************************************************
  * �������ƣ�ZKHD_Link_MakeFrame
  * ������������һ��������λ����֡
  * ��    �룺uint8_t Send_ID��������ID
            uint8_t Receive_ID:������ID
            uint8_t Msg_ID����ϢID
            uint8_t *ptr:��Ҫ���͵����ݵ�ַ
            uint16_t Length����Ҫ���͵����ݵĳ���
            
  * ��    ����uint8_t *Tx_Buff��������ɵ�֡��ʼ��ַ
              uint8_t *Tx_Buff_Length��������ɵ�֡�ĳ���
  * ��    �أ�void 
  * ��    ע��null 
  *    
  *
******************************************************************************/
void ZKHD_Link_MakeFrame(uint8_t Send_ID,
                        uint8_t Receive_ID,
                        uint8_t Msg_ID,
                        uint8_t *ptr,
                        uint16_t Length,
                        uint8_t *Tx_Buff,
                        uint8_t *Tx_Buff_Length)
{
    uint8_t Buff[100];
    int sum = 0;
    
    ZKHD_Link_Head_t ZKHD_Link_Head;
    
    ZKHD_Link_Head.Send_ID = Send_ID;
    ZKHD_Link_Head.Receive_ID = Receive_ID;
    ZKHD_Link_Head.Start = 0xAA;
    ZKHD_Link_Head.Length = Length;
    ZKHD_Link_Head.Message_ID = Msg_ID;
    
    memcpy(Buff,&ZKHD_Link_Head.Start, sizeof(ZKHD_Link_Head_t));
    memcpy(Buff + sizeof(ZKHD_Link_Head_t),ptr, Length);
    
    for(int i = 0; i < Length + 6; i++)
    {
        sum += Buff[i];
    }
    
    Buff[Length + sizeof(ZKHD_Link_Head_t)] = sum & 0xFF;
    
    memcpy(Tx_Buff, Buff, Length + sizeof(ZKHD_Link_Head_t) + 1);
    *Tx_Buff_Length = Length + sizeof(ZKHD_Link_Head_t) + 1;
}

/******************************************************************************
  * �������ƣ�ZKHD_Link_Handle
  * ��������������������λ�������ݣ�������
  * ��    �룺uint8_t *Rx_Buff���������ݵĵ�ַ
  * ��    ����void
  * ��    �أ�void 
  * ��    ע�����ڳ�����Ϣ�������˽������ݵĳ����У����Կ��Ե���ָ����������    
  *    
  *
******************************************************************************/
void ZKHD_Link_Handle(uint8_t *Rx_Buff)
{
    ZKHD_Link_Head_t *ZKHD_Link_Head_Ptr;
    
    ZKHD_Link_Head_Ptr = (ZKHD_Link_Head_t*)Rx_Buff;
    
    if(ZKHD_Link_Head_Ptr->Send_ID == Device_GCS)
    {
        if(ZKHD_Link_Head_Ptr->Receive_ID == Device_Remote)
        {
            ZKHD_Link_GCS_To_Remote_Handle(ZKHD_Link_Head_Ptr,&ZKHD_Link_Head_Ptr->Message_ID + 1, ZKHD_Link_Head_Ptr->Length);
        }else if(ZKHD_Link_Head_Ptr->Receive_ID == Device_FMU)
        {
            ZKHD_Link_GCS_To_FMU_Handle(ZKHD_Link_Head_Ptr,&ZKHD_Link_Head_Ptr->Message_ID + 1, ZKHD_Link_Head_Ptr->Length);
        }
    }
}

/******************************************************************************
  * �������ƣ�ZKHD_Link_GCS_To_Remote_Handle
  * ������������λ�����͵�ң�����Ĵ�����
  * ��    �룺ZKHD_Link_Head_t *ZKHD_Link_Head��ZKHD_LinkЭ��ͷ
  *           uint8_t *ptr���������ݵĵ�ַ��
  *           uint8_t length���������ݵĳ��ȣ�
  * ��    ����void
  * ��    �أ�void 
  * ��    ע��null      
  *    
  *
******************************************************************************/
void ZKHD_Link_GCS_To_Remote_Handle(ZKHD_Link_Head_t *ZKHD_Link_Head, uint8_t *ptr, uint8_t length)
{
    
}

/******************************************************************************
  * �������ƣ�ZKHD_Link_GCS_To_FMU_Handle
  * ������������λ�����͵��ɿصĴ�����
  * ��    �룺ZKHD_Link_Head_t *ZKHD_Link_Head��ZKHD_LinkЭ��ͷ
  *           uint8_t *ptr���������ݵĵ�ַ��
  *           uint8_t length���������ݵĳ��ȣ�
  * ��    ����void
  * ��    �أ�void 
  * ��    ע��null    
  *    
  *
******************************************************************************/
void ZKHD_Link_GCS_To_FMU_Handle(ZKHD_Link_Head_t *ZKHD_Link_Head, uint8_t *ptr, uint8_t length)
{
    gcs_ReceiveHandle(ZKHD_Link_Head, ptr, length);
} 


/******************* (C) ��Ȩ���� 2018 �����пƺƵ�Ƽ����޹�˾ *******************/
