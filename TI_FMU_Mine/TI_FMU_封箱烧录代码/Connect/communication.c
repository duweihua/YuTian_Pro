/**
  ******************************************************************************
  * Copyright (c) 2018,�����пƺƵ�Ƽ����޹�˾
  * All rights reserved.
  * �ļ����ƣ�communication.c
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
#include "communication.h"
#include "gcs.h"
#include "nrf24l01.h"


//�궨����
#define NRF_CH_FLASH_ADDR       (uint8_t*)PAGE_ADDRESS
#define NRF_RX_ADDR_FLASH_ADDR  (uint8_t*)(PAGE_ADDRESS + 2)
#define NRF_TX_ADDR_FLASH_ADDR  (uint8_t*)(PAGE_ADDRESS + 2 + 5)


//Extern����
extern uint8_t WriteBuf(uint8_t regaddr, uint8_t *pBuf, uint8_t datalen);    //д������
extern uint8_t ReadBuf(uint8_t regaddr, uint8_t *pBuf, uint8_t datalen);     //��������          
extern uint8_t ReadReg(uint8_t regaddr);                                     //���Ĵ���
extern uint8_t WriteReg(uint8_t regaddr, uint8_t data);                      //д�Ĵ���


//˽�к�����



//˽�б�����
NRF_Mannager_t NRF_Mannager;
uint8_t TX_ADDRESS[]= {0xE1, 0xE2, 0xE3, 0xE4, 0xE5};    //����Ĭ�Ϸ��͵�ַ
uint8_t RX_ADDRESS[]= {0x00, 0x00, 0x00, 0x00, 0xE5};    //����Ĭ�Ͻ��յ�ַ


/******************************************************************************
  * �������ƣ�NRF_Radio_Init
  * ������������ʼ��NRF24L01ͨ�Ż���
  * ��    �룺void
  * ��    ����void
  * ��    �أ�void
  * ��    ע��null
  *    
  *
******************************************************************************/
void NRF_Radio_Init(void)
{
    uint8_t defalut[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    /*�˴�ȱ�ٴ�FLASH����ȡ��ַ*/
    NRF_Mannager.Hardware_Mannager = &g_NRFManager;
    
    //��ʾδ��ʼ��,ʹ��Ĭ��ֵ
    if(memcmp((uint8_t*)NRF_RX_ADDR_FLASH_ADDR, defalut, 5) == 0)
    {
        memcpy(g_NRFManager.Rx_Addr, RX_ADDRESS, 5);
        memcpy(g_NRFManager.Tx_Addr, TX_ADDRESS, 5);
        g_NRFManager.RC_Frequency = NRF24L01_FREQ;
    }else
    {
        
        g_NRFManager.RC_Frequency = *((uint8_t*)NRF_CH_FLASH_ADDR);
        memcpy(g_NRFManager.Rx_Addr, NRF_RX_ADDR_FLASH_ADDR, 5);
        memcpy(g_NRFManager.Tx_Addr, NRF_TX_ADDR_FLASH_ADDR, 5);
    }
    
    g_NRFManager.NRF_Mode = NRF_Mode_RX2;
    NRF24L01Init(&g_NRFManager);
}

/******************************************************************************
  * �������ƣ�NRF_Radio_Send
  * ����������NRF24L01ͨ�ŷ���
  * ��    �룺uint8_t *ptr�����͵����ݵĵ�ַ
  *           uint8_t length�����͵����ݵĳ���
  * ��    ����void
  * ��    �أ�void 
  * ��    ע��null    
  *    
  *
******************************************************************************/
void NRF_Radio_Send(uint8_t *ptr, uint8_t length)
{
    enQueue(&NRF_Mannager.qTx, ptr, length);
}

/******************************************************************************
  * �������ƣ�PollingNRF
  * ����������NRF24L01�ж���ѵ����
  * ��    �룺void
  * ��    ����void
  * ��    �أ�void 
  * ��    ע��null    
  *    
  *
******************************************************************************/
void PollingNRF()
{
    static int rx_succsess_cnt = 0;
    uint8_t state = 0;
    uint8_t rxbuff_temp[RX_PLOAD_WIDTH]; 
    
    state = ReadReg(STATUS);
    WriteReg(SPI_WRITE_REG + STATUS, state);
    
    if(state & MAX_TX)
    {
        WriteReg(STATUS, MAX_TX);
        NRF_Mannager.communication_info.Tx_Success_Count++;
        
        NRF_Mannager.Hardware_Mannager->set_rx_mode();
        NRF_Mannager.mode = Rx_Mode;
    }
   
    if(state & RX_OK)
    {
        rx_succsess_cnt++;
        WriteReg(STATUS, RX_OK);
        state = ReadReg(STATUS);
        ReadBuf(RD_RX_PLOAD, rxbuff_temp, RX_PLOAD_WIDTH);//��ȡ����
        enQueue(&NRF_Mannager.qRx, rxbuff_temp, RX_PLOAD_WIDTH);       
        WriteReg(FLUSH_RX, 0xff);          //���RX FIFO�Ĵ��� 
        NRF_Mannager.communication_info.Rx_Success_Count++;\
        
        SwitchPort(Connect_Port_NRF, rxbuff_temp);
        AnalyRC();
    }
    
    {//��������Ϣ
        uint8_t buff_temp[32];
        uint8_t length;
        
        memset(buff_temp, 0xFF, 32);
        if(deQueue(&NRF_Mannager.qTx, buff_temp,&length))
        {
            switch(g_NRFManager.NRF_Mode)
            {
                case NRF_Mode_TX:
                case NRF_Mode_TX2:
                    NRF24L01TxPacket(buff_temp, length);
                    break;
                case NRF_Mode_RX2:
                    NRF24L01TxPacketAp(buff_temp, length);
                    break;
                default:
                    break;
            }
        }
    }

    //���㶪����
    NRF_Mannager.communication_info.Tx_Packet_Loss_Rate = 
        NRF_Mannager.communication_info.Tx_Fail_Count * 1.0f / (NRF_Mannager.communication_info.Tx_Fail_Count + NRF_Mannager.communication_info.Tx_Success_Count);
}

/******************************************************************************
  * �������ƣ�SwitchPort
  * ����������ͨ�Ŷ˿�ѡ��
  * ��    �룺���ݽ��ն˿ڿ��أ����������˿���Ϣ
  * ��    ����void
  * ��    �أ�void 
  * ��    ע��null    
  *    
  *
******************************************************************************/
void SwitchPort(Connect_Port_t Connect_Port, uint8_t *ptr)
{
    switch(Connect_Port)
    {
        case Connect_Port_USB:
            g_UAVinfo.Connect_Port = Connect_Port_USB;
            break;
        case Connect_Port_NRF:
            g_UAVinfo.Connect_Port = Connect_Port_NRF;
            break;
        default:
            break;
    }
    ZKHD_Link_Handle(ptr);
}

/******************* (C) ��Ȩ���� 2018 �����пƺƵ�Ƽ����޹�˾ *******************/
