/**
  ******************************************************************************
  * Copyright (c) 2018,�����пƺƵ�Ƽ����޹�˾
  * All rights reserved.
  * �ļ����ƣ�height_control.h
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
#ifndef __ano_gcs_dt_H
#define __ano_gcs_dt_H
//�ⲿ�ļ�����
#include "include.h"
#include "program_ctrl.h"

//�궨����
#define ANO_DT_USE_USART2

//���ݽṹ����


//Extern����




//��������
//static
static void ANO_DT_SendCmd(u8 dest, u8 fun, u16 cmd1, u16 cmd2, u16 cmd3, u16 cmd4, u16 cmd5);
static void ANO_DT_Send_Data(u8 *dataToSend , u8 length);
//public
void ANO_DT_Data_Receive_Anl_Task(void);//��ѯ�Ƿ���������Ҫ����
void ANO_DT_Data_Receive_Prepare(u8 data);//����BYTE���ݣ����봮�ڽ����жϣ�
#endif

/******************* (C) ��Ȩ���� 2018 �����пƺƵ�Ƽ����޹�˾ *******************/

