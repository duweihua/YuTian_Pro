/**
  ******************************************************************************
  * Copyright (c) 2018,�����пƺƵ�Ƽ����޹�˾
  * All rights reserved.
  * �ļ����ƣ�include.h
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
#ifndef __INCLUDE_H
#define __INCLUDE_H
//�ⲿ�ļ�����
//��һ�������õ�ͷ�ļ�
#include "include.h"
#include "stdint.h"
#include "stdbool.h"
#include "driverlib.h"

//DataBase
#include "queue.h"
#include "fmuConfig.h"
#include "hardware.h"
#include "motor.h"

//�궨����
#define SUCCESS 0
#define FAILED  1


//���ݽṹ����



//Extern����



//��������
void KernelPolling(void);

#endif

/******************* (C) ��Ȩ���� 2018 �����пƺƵ�Ƽ����޹�˾ *******************/
