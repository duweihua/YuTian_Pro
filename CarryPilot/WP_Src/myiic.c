/* Copyright (C) 2015 Kristian Sloth Lauszus. All rights reserved.

This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").

Contact information
-------------------

Kristian Sloth Lauszus
Web      :  http://www.lauszus.com
e-mail   :  lauszus@gmail.com
*/

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
#include <stdint.h>
#include <stdbool.h>

#include "I2C.h"

#include "hw_memmap.h"
#include "gpio.h"
#include "pin_map.h"
#include "myiic.h"
#include "sysctl.h"
/***********************************************************
@��������Init_I2C
@��ڲ�������
@���ڲ�������
����������TM4CӲ��I2C��ʼ��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Init_I2C(void) {
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1); // Enable I2C1 peripheral
  SysCtlDelay(2); // Insert a few cycles after enabling the peripheral to allow the clock to be fully activated
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // Enable GPIOA peripheral
  SysCtlDelay(2); // Insert a few cycles after enabling the peripheral to allow the clock to be fully activated
  
  // Use alternate function
  GPIOPinConfigure(GPIO_PA6_I2C1SCL);
  GPIOPinConfigure(GPIO_PA7_I2C1SDA);
  
  GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6); // Use pin with I2C SCL peripheral
  GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7); // Use pin with I2C peripheral
	
  I2CMasterInitExpClk(I2C1_BASE, 400*100000,true); // Enable and set frequency to 400 kHz  100
  //I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(),true);          
  SysCtlDelay(2); // Insert a few cycles after enabling the I2C to allow the clock to be fully activated
}

/***********************************************************
@��������i2cWriteData
@��ڲ�����uint8_t addr, uint8_t regAddr, uint8_t *data, uint8_t length
@���ڲ�������
����������I2Cд����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void i2cWriteData(uint8_t addr, uint8_t regAddr, uint8_t *data, uint8_t length) {
  I2CMasterSlaveAddrSet(I2C1_BASE, addr, false); // Set to write mode
  
  I2CMasterDataPut(I2C1_BASE, regAddr); // Place address into data register
  I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START); // Send start condition
  while (I2CMasterBusy(I2C1_BASE)); // Wait until transfer is done
  
  for (uint8_t i = 0; i < length - 1; i++) {
    I2CMasterDataPut(I2C1_BASE, data[i]); // Place data into data register
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_CONT); // Send continues condition
    while (I2CMasterBusy(I2C1_BASE)); // Wait until transfer is done
  }
  
  I2CMasterDataPut(I2C1_BASE, data[length - 1]); // Place data into data register
  I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); // Send finish condition
  while (I2CMasterBusy(I2C1_BASE)); // Wait until transfer is done
}

/***********************************************************
@��������i2cRead
@��ڲ�����uint8_t addr, uint8_t regAddr
@���ڲ�������
����������I2C������
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
uint8_t i2cRead(uint8_t addr, uint8_t regAddr) {
  I2CMasterSlaveAddrSet(I2C1_BASE, addr, false); // Set to write mode
  
  I2CMasterDataPut(I2C1_BASE, regAddr); // Place address into data register
  I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_SEND); // Send data
  while (I2CMasterBusy(I2C1_BASE)); // Wait until transfer is done
  
  I2CMasterSlaveAddrSet(I2C1_BASE, addr, true); // Set to read mode
  
  I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE); // Tell master to read data
  while (I2CMasterBusy(I2C1_BASE)); // Wait until transfer is done
  return I2CMasterDataGet(I2C1_BASE); // Read data
}

/***********************************************************
@��������i2cWrite
@��ڲ�����uint8_t addr, uint8_t regAddr, uint8_t data
@���ڲ�������
����������I2Cд����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void i2cWrite(uint8_t addr, uint8_t regAddr, uint8_t data) {
  i2cWriteData(addr, regAddr, &data, 1);
}


/***********************************************************
@��������i2cReadData
@��ڲ�����uint8_t addr, uint8_t regAddr, uint8_t *data,
uint8_t length
@���ڲ�������
����������I2C������
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void i2cReadData(uint8_t addr, uint8_t regAddr, uint8_t *data, uint8_t length) {
  I2CMasterSlaveAddrSet(I2C1_BASE, addr, false); // Set to write mode
  I2CMasterDataPut(I2C1_BASE, regAddr); // Place address into data register
  I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_SEND); // Send data
  while (I2CMasterBusy(I2C1_BASE)); // Wait until transfer is done 
  I2CMasterSlaveAddrSet(I2C1_BASE, addr, true); // Set to read mode
  I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START); // Send start condition
  while (I2CMasterBusy(I2C1_BASE)); // Wait until transfer is done
  data[0] = I2CMasterDataGet(I2C1_BASE); // Place data into data register 
  for (uint8_t i = 1; i < length - 1; i++) 
	{
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT); // Send continues condition
    while (I2CMasterBusy(I2C1_BASE)); // Wait until transfer is done
    data[i] = I2CMasterDataGet(I2C1_BASE); // Place data into data register
  }
  I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH); // Send finish condition
  while (I2CMasterBusy(I2C1_BASE)); // Wait until transfer is done
  data[length - 1] = I2CMasterDataGet(I2C1_BASE); // Place data into data register
}


void Single_WriteI2C(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)
{
  i2cWrite(SlaveAddress,REG_Address,REG_data);
}	

unsigned char Single_ReadI2C(unsigned char SlaveAddress,unsigned char REG_Address)
{
  return i2cRead(SlaveAddress,REG_Address);
}

short int Double_ReadI2C(unsigned char SlaveAddress,unsigned char REG_Address)
{
  unsigned char msb , lsb ;
  msb = i2cRead(SlaveAddress,REG_Address);
  lsb = i2cRead(SlaveAddress,REG_Address+1);
  return ( ((short int)msb) << 8 | lsb) ;
}

////////////////////////
/***********************************************************
@��������Init_I2C2
@��ڲ�������
@���ڲ�������
����������TM4CӲ��I2C��ʼ��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Init_I2C0(void) {
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0); // Enable I2C0 peripheral
  SysCtlDelay(2); // Insert a few cycles after enabling the peripheral to allow the clock to be fully activated
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // Enable GPIOB peripheral
  SysCtlDelay(2); // Insert a few cycles after enabling the peripheral to allow the clock to be fully activated
  
  // Use alternate function
  GPIOPinConfigure(GPIO_PB2_I2C0SCL);
  GPIOPinConfigure(GPIO_PB3_I2C0SDA);
  
  GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2); // Use pin with I2C SCL peripheral
  GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3); // Use pin with I2C peripheral
  
  I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(),true); // Enable and set frequency to 400 kHz
  
  SysCtlDelay(2); // Insert a few cycles after enabling the I2C to allow the clock to be fully activated
}

/***********************************************************
@��������i2cWriteData
@��ڲ�����uint8_t addr, uint8_t regAddr, uint8_t *data, uint8_t length
@���ڲ�������
����������I2Cд����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void i2c0WriteData(uint8_t addr, uint8_t regAddr, uint8_t *data, uint8_t length) {
  I2CMasterSlaveAddrSet(I2C0_BASE, addr, false); // Set to write mode
  
  I2CMasterDataPut(I2C0_BASE, regAddr); // Place address into data register
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START); // Send start condition
  while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
  
  for (uint8_t i = 0; i < length - 1; i++) {
    I2CMasterDataPut(I2C0_BASE, data[i]); // Place data into data register
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT); // Send continues condition
    while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
  }
  
  I2CMasterDataPut(I2C0_BASE, data[length - 1]); // Place data into data register
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); // Send finish condition
  while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
}

/***********************************************************
@��������i2cRead
@��ڲ�����uint8_t addr, uint8_t regAddr
@���ڲ�������
����������I2C������
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
uint8_t i2c0Read(uint8_t addr, uint8_t regAddr) {
  I2CMasterSlaveAddrSet(I2C0_BASE, addr, false); // Set to write mode
  
  I2CMasterDataPut(I2C0_BASE, regAddr); // Place address into data register
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND); // Send data
  while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
  
  I2CMasterSlaveAddrSet(I2C0_BASE, addr, true); // Set to read mode
  
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE); // Tell master to read data
  while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
  return I2CMasterDataGet(I2C0_BASE); // Read data
}

/***********************************************************
@��������i2cWrite
@��ڲ�����uint8_t addr, uint8_t regAddr, uint8_t data
@���ڲ�������
����������I2Cд����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void i2c0Write(uint8_t addr, uint8_t regAddr, uint8_t data) {
  i2c0WriteData(addr, regAddr, &data, 1);
}


/***********************************************************
@��������i2cReadData
@��ڲ�����uint8_t addr, uint8_t regAddr, uint8_t *data,
uint8_t length
@���ڲ�������
����������I2C������
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void i2c0ReadData(uint8_t addr, uint8_t regAddr, uint8_t *data, uint8_t length) {
  I2CMasterSlaveAddrSet(I2C0_BASE, addr, false); // Set to write mode
  I2CMasterDataPut(I2C0_BASE, regAddr); // Place address into data register
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND); // Send data
  while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done 
  I2CMasterSlaveAddrSet(I2C0_BASE, addr, true); // Set to read mode
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START); // Send start condition
  while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
  data[0] = I2CMasterDataGet(I2C0_BASE); // Place data into data register 
  for (uint8_t i = 1; i < length - 1; i++) 
	{
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT); // Send continues condition
    while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
    data[i] = I2CMasterDataGet(I2C0_BASE); // Place data into data register
  }
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH); // Send finish condition
  while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
  data[length - 1] = I2CMasterDataGet(I2C0_BASE); // Place data into data register
}

/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/
