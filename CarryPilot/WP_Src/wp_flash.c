
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

#include "eeprom.h"
#include "flash.h"
#include "sysctl.h"
#include "wp_flash.h"

//FLASH��ʼ��ַ
#define WP_FLASH_BASE PARAMETER_TABLE_STARTADDR 	//STM32 FLASH����ʼ��ַ

FLIGHT_PARAMETER Table_Parameter;
/*
struct E2PROM
{
uint8_t value1;
uint8_t value2;
uint16_t value3;
uint8_t value4[12];
}; 
*/
//struct E2PROM e2prom_write_value = {5,7,9826, "Hello World"};
//struct E2PROM e2prom_read_value =  {0,0,0,""};


void EEPROM_Init(void)
{
  /* EEPROM SETTINGS */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0); // EEPROM activate
  EEPROMInit(); // EEPROM start
}

void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)	
{ 
  EEPROMProgram(pBuffer,WriteAddr,NumToWrite);
}

void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)   	
{
  EEPROMRead(pBuffer,ReadAddr,NumToRead);
}


void ReadFlashParameterALL(FLIGHT_PARAMETER *WriteData)
{
  EEPROMRead((uint32_t *)(&WriteData->Parameter_Table),WP_FLASH_BASE,FLIGHT_PARAMETER_TABLE_NUM*4);
}


void ReadFlashParameterOne(uint16_t Label,float *ReadData)
{
  EEPROMRead((uint32_t *)(ReadData),WP_FLASH_BASE+4*Label,4);
}

void ReadFlashParameterTwo(uint16_t Label,float *ReadData1,float *ReadData2)
{
  EEPROMRead((uint32_t *)(ReadData1),WP_FLASH_BASE+4*Label,4);;
  EEPROMRead((uint32_t *)(ReadData2),WP_FLASH_BASE+4*Label+4,4);
}

void ReadFlashParameterThree(uint16_t Label,float *ReadData1,float *ReadData2,float *ReadData3)
{
  EEPROMRead((uint32_t *)(ReadData1),WP_FLASH_BASE+4*Label,4);;
  EEPROMRead((uint32_t *)(ReadData2),WP_FLASH_BASE+4*Label+4,4);
  EEPROMRead((uint32_t *)(ReadData3),WP_FLASH_BASE+4*Label+8,4);
}

void WriteFlashParameter(uint16_t Label,
                         float WriteData,
                         FLIGHT_PARAMETER *Table)
{
  ReadFlashParameterALL(Table);//�Ȱ�Ƭ���ڵ��������ݶ�������ֵ
  Table->Parameter_Table[Label]=WriteData;//����Ҫ���ĵ��ֶθ���
  EEPROMProgram((uint32_t *)(&Table->Parameter_Table[Label]),WP_FLASH_BASE+4*Label,4);
}

void WriteFlashParameter_Two(uint16_t Label,
                             float WriteData1,
                             float WriteData2,
                             FLIGHT_PARAMETER *Table)
{
  ReadFlashParameterALL(Table);//�Ȱ�Ƭ���ڵ��������ݶ�������
  Table->Parameter_Table[Label]=WriteData1;//����Ҫ���ĵ��ֶθ���ֵ
  Table->Parameter_Table[Label+1]=WriteData2;//����Ҫ���ĵ��ֶθ���ֵ
  EEPROMProgram((uint32_t *)(&Table->Parameter_Table[Label]),WP_FLASH_BASE+4*Label,8);
}

void WriteFlashParameter_Three(uint16_t Label,
                               float WriteData1,
                               float WriteData2,
                               float WriteData3,
                               FLIGHT_PARAMETER *Table)
{
  ReadFlashParameterALL(Table);//�Ȱ�Ƭ���ڵ��������ݶ�������
  Table->Parameter_Table[Label]=WriteData1;//����Ҫ���ĵ��ֶθ���ֵ
  Table->Parameter_Table[Label+1]=WriteData2;//����Ҫ���ĵ��ֶθ���ֵ
  Table->Parameter_Table[Label+2]=WriteData3;//����Ҫ���ĵ��ֶθ���ֵ
  EEPROMProgram((uint32_t *)(&Table->Parameter_Table[Label]),WP_FLASH_BASE+4*Label,12);
}


/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/
