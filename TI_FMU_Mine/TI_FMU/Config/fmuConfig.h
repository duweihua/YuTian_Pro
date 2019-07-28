/**
  ******************************************************************************
  * Copyright (c) 2018,�����пƺƵ�Ƽ����޹�˾
  * All rights reserved.
  * �ļ����ƣ�fmuConfig.h
  * ժ    Ҫ��
  *
  * ��ǰ�汾��V1.0
  * ��    �ߣ������пƺƵ�Ƽ����޹�˾�з��� 
  * ������ڣ�    
  * �޸�˵����
  * 
  *
  * ��ʷ�汾��
  *         �̼��ţ�100
  *         ԭʼ�汾
  *
  *         �̼��ţ�101
  *         ���£�
  *         1.֧���޸ķ��ͽ��յ�ַ��
  *         2.���շ���Ϣ��ΪUSB�ӿ�
  *
  *         �̼��ţ�102
  *         ���£�
  *         1.֧�ֶ��߹��ܣ�
  *         2.֧�ִ�Ҷ��
  *         3.����LED��ʾ��
  *         4.����YAW����ƣ�
  *         5.���¶����·ŷ��󴥹��ܣ�
  *         
  *******************************************************************************/
#ifndef __FMU_CONFIG_H
#define __FMU_CONFIG_H
//�ⲿ�ļ�����
#include "include.h"


//�궨����
/*�������Ӳ��*/
//�豸ѡ����
#define ZKHD_HAWK_I2C_ENABLE
#define ZKHD_HAWK_SPI_ENABLE
#define ZKHD_HAWK_USART1_ENABLE
#define ZKHD_HAWK_USART2_ENABLE
#define ZKHD_HAWK_MOTOR_LED_ENABLE
#define ZKHD_HAWK_RGB_LED_ENABLE
#define ZKHD_HAWK_ADC_ENABLE
#define ZKHD_HAWK_FLASH_ENABLE
#define ZKHD_HAWK_GPIO_ENABLE

/*�豸����ѡ����*/
#ifdef HARDWARE_NRF24L01
#define NRF_RW  Hardware_SPI1_RW
#define SET_NRF_CSN                 Hardware_SPI1_Device1_CSN_H
#define CLR_NRF_CSN                 Hardware_SPI1_Device1_CSN_L
#define NRF24L01CE_H                GPIO_Pin1_Set(1)
#define NRF24L01CE_L                GPIO_Pin1_Set(0)
#define READ_NRF24L01_IRQ           (GPIOA->IDR&GPIO_PIN_15)          //IRQ������������ PA15
#endif

/*��оƬѡ��*/
#define MSP432

#ifdef MSP432
#define RADIO
#define SBUS

#endif

//#define STM32
#ifdef STM32
#deine NRF24L01

#endif


#define DEBUG_PORT             A2
#define DEBUG
#define FIRMWARE_INFO          102

/*�߶ȿ��Ƴ���*/

#define MAX_ISFD_ATTITUDE      40
#define MIN_ALT_CM             50
#define MAX_ALT_CM             200
#define MAX_ALT_RATE           0.05
#define FIX_ALT_RATE           20
#define MAX_REMOTE_THROTTLE    2000
#define THROTTLE_DEAD_ZONE     0.2f

#define MOTOR_MAX_INIT_VALUE   850
#define MOTOR_MAX_VALUE   1000
#define NRF24L01_FREQ          0

//���ݽṹ����



//Extern����



//��������

#endif

/******************* (C) ��Ȩ���� 2018 �����пƺƵ�Ƽ����޹�˾ *******************/
