
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
#include "Time.h"
#include "string.h"
#include "oledfont.h"
#include "oled.h"
#include "stdlib.h"


//#define  OLED_WORK_MODE_I2C  1//OLEDʹ��I2Cģʽ������������SPIģʽʱ��ע�ͱ��м���

#define XLevelL  	0x00
#define XLevelH  	0x10
#define XLevel    ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column 	128
#define Max_Row  	64
#define Brightness 	0xCF
#define X_WIDTH 	128
#define Y_WIDTH 	64
/***********************************************************
@��������OLED_GPIO_Init
@��ڲ�������
@���ڲ�������
����������OLED��ʼ��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void OLED_GPIO_Init(void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIO_OLED);// Enable the GPIO port that is used for the on-board LED.
  GPIOPinTypeGPIOOutput(GPIO_OLED, GPIO_PIN_5);
  GPIOPinTypeGPIOOutput(GPIO_OLED, GPIO_PIN_4);
  Delay_Ms(2);
  GPIOPinWrite(GPIO_OLED, GPIO_PIN_5, GPIO_PIN_5);//���ø�
  GPIOPinWrite(GPIO_OLED, GPIO_PIN_4, GPIO_PIN_4);//���ø�
}

/***********************************************************
@��������SDA_OUT
@��ڲ�������
@���ڲ�������
����������SDA����Ϊ���
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void SDA_OUT(void)
{
  GPIOPinTypeGPIOOutput(GPIO_OLED, GPIO_PIN_4);// Enable the GPIO pins for the LED (PF2 & PF3).
}

/***********************************************************
@��������SDA_IN
@��ڲ�������
@���ڲ�������
����������SDA����Ϊ����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void SDA_IN(void)
{
  GPIOPinTypeGPIOInput(GPIO_OLED, GPIO_PIN_4);// Enable the GPIO pins for the LED (PF2 & PF3).
}

/***************************
������:	void IIC_Start(void)
˵��:	����IIC��ʼ�ź�
���:	��
����:	��
��ע:	��
****************************/
void OLED_IIC_Start(void)
{
  SDA_OUT();		// SDA�����
  IIC_SDA_H;
  IIC_SCL_H;
  //Delay_Us(4);
  IIC_SDA_L;
  //Delay_Us(1);
  IIC_SCL_L;		// ǯסI2C����,׼�����ͻ�������� 
}

/***************************
������:	void IIC_Stop(void)
˵��:	����IICֹͣ�ź�
���:	��
����:	��
��ע:	��
****************************/
void OLED_IIC_Stop(void)
{
  SDA_OUT();			// SDA�����
  IIC_SDA_L;
  //Delay_Us(4);
  IIC_SCL_H;
  //Delay_Us(1);
  IIC_SDA_H;			// ����I2C���߽����ź�
  //Delay_Us(5);		// �ٴ�������Ҫ4.7us
}

/**********************************
������:	uint8_t IIC_Wait_Ack(void)
˵��:	�ȴ�Ӧ���źŵ���
���:	��
����:	uint8_t 1,����Ӧ��ʧ��
0,����Ӧ��ɹ�
��ע:	��
**********************************/
uint8_t IIC_Wait_Ack(void)
{
  uint8_t ucErrTime = 0;
  SDA_IN();					// SDA����Ϊ����  
  IIC_SDA_H;
  //Delay_Us(1);		
  IIC_SCL_H;
  //Delay_Us(1);	 
  while (READ_SDA)			// SDAΪ��,�ȴ�IIC��������
  {
    ucErrTime++;
    if (ucErrTime > 250)	//	40*250=1msδ��,IIC����ֹͣ�ź�
    {
      OLED_IIC_Stop();
      return 1;
    }
  }
  IIC_SCL_L;
  return 0;
}

/**************************
������:	void IIC_Ack(void)
˵��:	����ACKӦ��
���:	��
����:	��
��ע:	��
**************************/
void IIC_Ack(void)
{
  IIC_SCL_L;
  SDA_OUT();
  IIC_SDA_L;
  //Delay_Us(1);
  IIC_SCL_H;
  //Delay_Us(1);
  IIC_SCL_L;
}


/***************************
������:	void IIC_NAck(void)
˵��:	������ACKӦ��
���:	��
����:	��
��ע:	��
***************************/
void IIC_NAck(void)
{
  IIC_SCL_L;
  SDA_OUT();
  IIC_SDA_H;
  //Delay_Us(1);
  IIC_SCL_H;
  //Delay_Us(1);
  IIC_SCL_L;
}

/**************************************
������:	void IIC_Send_Byte(uint8_t txd)
˵��:	IIC����һ���ֽ�
���:	uint8_t	txd
����:	��
��ע:	��
**************************************/
void Write_IIC_Byte(uint8_t txd)
{
  uint8_t t;
  
  SDA_OUT();
  IIC_SCL_L;				// ����ʱ�ӿ�ʼ���ݴ���
  for (t = 0; t < 8; t++)
  {
    if ((txd&0x80) >> 7)
    {
      IIC_SDA_H;
    }
    else
    {
      IIC_SDA_L;
    }
    txd <<= 1;
    //Delay_Us(1);
    IIC_SCL_H;
    //Delay_Us(1); 
    IIC_SCL_L;
  }
  IIC_Wait_Ack();
}

/*********************OLEDд����************************************/ 
void OLED_WrDat(unsigned char IIC_Data)
{
  OLED_IIC_Start();
  Write_IIC_Byte(0x78);
  Write_IIC_Byte(0x40);			//write data
  Write_IIC_Byte(IIC_Data);
  OLED_IIC_Stop();
}
/*********************OLEDд����************************************/
void OLED_WrCmd(unsigned char IIC_Command)
{
  OLED_IIC_Start();
  Write_IIC_Byte(0x78);            //Slave address,SA0=0
  Write_IIC_Byte(0x00);			//write command
  Write_IIC_Byte(IIC_Command);
  OLED_IIC_Stop();
}
/*********************OLED ��������************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
  OLED_WrCmd(0xb0+y);
  OLED_WrCmd(((x&0xf0)>>4)|0x10);
  OLED_WrCmd((x&0x0f)|0x01);
} 
/*********************OLEDȫ��************************************/
void OLED_Fill(unsigned char bmp_dat) 
{
  unsigned char y,x;
  for(y=0;y<8;y++)
  {
    OLED_WrCmd(0xb0+y);
    OLED_WrCmd(0x01);
    OLED_WrCmd(0x10);
    for(x=0;x<X_WIDTH;x++)
      OLED_WrDat(bmp_dat);
  }
}
/*********************OLED��λ************************************/
void OLED_CLS(void)
{
  unsigned char y,x;
  for(y=0;y<8;y++)
  {
    OLED_WrCmd(0xb0+y);
    OLED_WrCmd(0x01);
    OLED_WrCmd(0x10);
    for(x=0;x<X_WIDTH;x++)
      OLED_WrDat(0);
  }
}
/*********************OLED��ʼ��************************************/
void OLED_Init_I2C(void)
{
  Delay_Ms(100);//��ʼ��֮ǰ����ʱ����Ҫ��
  OLED_WrCmd(0xae);//--turn off oled panel
  OLED_WrCmd(0x00);//---set low column address
  OLED_WrCmd(0x10);//---set high column address
  OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  OLED_WrCmd(0x81);//--set contrast control register
  OLED_WrCmd(Brightness); // Set SEG Output Current Brightness
  OLED_WrCmd(0xa0);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
  OLED_WrCmd(0xc0);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
  OLED_WrCmd(0xa6);//--set normal display
  OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
  OLED_WrCmd(0x3f);//--1/64 duty
  OLED_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
  OLED_WrCmd(0x00);//-not offset
  OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
  OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
  OLED_WrCmd(0xd9);//--set pre-charge period
  OLED_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  OLED_WrCmd(0xda);//--set com pins hardware configuration
  OLED_WrCmd(0x12);
  OLED_WrCmd(0xdb);//--set vcomh
  OLED_WrCmd(0x40);//Set VCOM Deselect Level
  OLED_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_WrCmd(0x02);//
  OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
  OLED_WrCmd(0x14);//--set(0x10) disable
  OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
  OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
  OLED_WrCmd(0xaf);//--turn on oled panel
  OLED_Fill(0x00); //��ʼ����
  OLED_Set_Pos(0,0);
} 

/****************SPI***************************/
/*------------дһ�����ݣ�Ƭѡ����-----------*/
void LCD_WrDat(unsigned char dat)
{
  unsigned char i=8;
  LCD_DCout(Bit_SET);
  for(i=0;i<8;i++)				//����8λ���ݣ�ʱ����������Ч
  {
    LCD_SCLout(Bit_RESET);
    if(dat&0x80)	LCD_SDAout(Bit_SET);
    else	LCD_SDAout(Bit_RESET);
    LCD_SCLout(Bit_SET);
    dat<<=1;
  }
}
/*------------д���Ƭѡ����-------------*/
void LCD_WrCmd(unsigned char cmd)
{
  unsigned char i=8;
  LCD_DCout(Bit_RESET);
  for(i=0;i<8;i++)			//����8λ���ݣ�ʱ����������Ч
  {
    LCD_SCLout(Bit_RESET);
    if(cmd&0x80)	LCD_SDAout(Bit_SET);
    else LCD_SDAout(Bit_RESET);
    LCD_SCLout(Bit_SET);
    cmd<<=1;
  }
  LCD_DCout(Bit_SET);
}
/*----------------��������------------------*/
void LCD_Set_Pos(unsigned char x, unsigned char y)
{
  LCD_WrCmd(0xb0+y);
  LCD_WrCmd(((x&0xf0)>>4)|0x10);
  LCD_WrCmd((x&0x0f)|0x01);
}
/*----------------ȫ����ʾ-----------------*/
void LCD_Fill(unsigned char bmp_dat)
{
  unsigned char y,x;
  for(y=0;y<8;y++)
  {
    LCD_WrCmd(0xb0+y);
    LCD_WrCmd(0x01);
    LCD_WrCmd(0x10);
    for(x=0;x<X_WIDTH;x++)
    {
      LCD_WrDat(bmp_dat);
    }
  }
}
/*---------------LCD��λ-----------------*/
void LCD_CLS(void)
{
  unsigned char y,x;
#ifdef OLED_WORK_MODE_I2C
  for(y=0;y<8;y++)
  {
    OLED_WrCmd(0xb0+y);
    OLED_WrCmd(0x01);
    OLED_WrCmd(0x10);
    for(x=0;x<X_WIDTH;x++)
      OLED_WrDat(0); 
  }
#else  
  for(y=0;y<8;y++)
  {
    LCD_WrCmd(0xb0+y);
    LCD_WrCmd(0x01);
    LCD_WrCmd(0x10);
    for(x=0;x<X_WIDTH;x++)
      LCD_WrDat(0); 
  }
#endif
  
}
/*------��ʾ6X8һ���׼��ASCII�ַ�������ʾ����Ϊ��x��y��------*/
void LCD_P6x8Str(unsigned char x,unsigned char  y,unsigned char ch[])
{
  unsigned char c=0,i=0,j=0;
  while (ch[j]!='\0')
  {
    c =ch[j]-32;
#ifdef OLED_WORK_MODE_I2C
    if(x>126){x=0;y++;}
    OLED_Set_Pos(x,y);
    for(i=0;i<6;i++)
      OLED_WrDat(F6x8[c][i]);
#else    
    if(x>126){x=0;y++;}
    LCD_Set_Pos(x,y);
    for(i=0;i<6;i++)
      LCD_WrDat(F6x8[c][i]);
#endif
    x+=6;
    j++;
  }
}
//��ʾһ��6X8���ַ�
void LCD_P6x8Char(unsigned char x,unsigned char  y,unsigned char ucData)
{
  unsigned char i, ucDataTmp;
  ucDataTmp = ucData-32;
  if(x > 126)
  {
    x= 0;
    y++;
  }
#ifdef OLED_WORK_MODE_I2C
  OLED_Set_Pos(x, y);
  for(i = 0; i < 6; i++)
  {
    OLED_WrDat(F6x8[ucDataTmp][i]);
  }
#else
  LCD_Set_Pos(x, y);
  for(i = 0; i < 6; i++)
  {
    LCD_WrDat(F6x8[ucDataTmp][i]);
  }
#endif
}
/*--------------��ʾ6X8�ĸ�����--------------*/
void write_6_8_number(unsigned char x,unsigned char y, float number)
{
  unsigned char i=0;
  unsigned char temp[16];
  unsigned char *point=temp;
  float decimal;
  int data;
  if(number<0)
  {
    temp[0]='-';
    LCD_P6x8Char(x,y,temp[0]);
    x+=6;
    number=-number;
  }
  data=(int)number;
  decimal=number-data;					//С������
  
  if(data>=1000000000)           //�Ƿ��ܱ�10^9����
  {
    temp[i]=48+data/1000000000;
    data=data%1000000000;
    i++;
  }
  if(data>=100000000)           //�Ƿ��ܱ�10^8����
  {
    temp[i]=48+data/100000000;
    data=data%100000000;
    i++;
  }
  else
    if(data<100000000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=10000000)           //�Ƿ��ܱ�10^7����
  {
    temp[i]=48+data/10000000;
    data=data%10000000;
    i++;
  }
  else
    if(data<10000000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=1000000)           //�Ƿ��ܱ�10^6����
  {
    temp[i]=48+data/1000000;
    data=data%1000000;
    i++;
  }
  else
    if(data<1000000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=100000)           //�Ƿ��ܱ�100000����
  {
    temp[i]=48+data/100000;
    data=data%100000;
    i++;
  }
  else
    if(data<100000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=10000)           //�Ƿ��ܱ�10000����
  {
    temp[i]=48+data/10000;
    data=data%10000;
    i++;
  }
  else
    if(data<10000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=1000)           //�Ƿ��ܱ�1000����
  {
    temp[i]=48+data/1000;
    data=data%1000;
    i++;
  }
  else
    if(data<1000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=100)              //�Ƿ��ܱ�100����
  {
    temp[i]=48+data/100;
    data=data%100;
    i++;
  }
  else
    if(data<100&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=10)                  //�Ƿ��ܱ�10����
  {
    temp[i]=48+data/10;
    data=data%10;
    i++;
  }
  else
    if(data<10&&i!=0)
    {
      temp[i]=48;
      i++;
    }
  temp[i]=48+data;
  if(decimal>=0.0001f)           //�ж��Ƿ���С������
  {
    i++;
    temp[i]='.';                //��ʾС����
    i++;
    data=(int)(decimal*1000);
    temp[i]=48+data/100;
    data=data%100;
    i++;
    if(data>0)
    {
      temp[i]=48+data/10;
      data=data%10;
    }
    if(data>=0)
    {
      i++;
      temp[i]=data+48;
    }
  }
  i++;
  temp[i]='\0';
  LCD_P6x8Str(x,y,point);
}
/*------��ʾ8X16һ���׼��ASCII�ַ�������ʾ����Ϊ��x��y��------*/
void LCD_P8x16Str(unsigned char x,unsigned char  y,unsigned char ch[])
{
  unsigned char c=0,i=0,j=0;
  while (ch[j]!='\0')
  {
    c =ch[j]-32;
    if(x>120){x=0;y++;}
#ifdef OLED_WORK_MODE_I2C   
    OLED_Set_Pos(x,y);
    for(i=0;i<8;i++)
      OLED_WrDat(F8X16[c*16+i]);
    OLED_Set_Pos(x,y+1);
    for(i=0;i<8;i++)
      OLED_WrDat(F8X16[c*16+i+8]);
#else
    LCD_Set_Pos(x,y);
    for(i=0;i<8;i++)
      LCD_WrDat(F8X16[c*16+i]);
    LCD_Set_Pos(x,y+1);
    for(i=0;i<8;i++)
      LCD_WrDat(F8X16[c*16+i+8]);    
#endif    
    x+=8;
    j++;
  }
}
//��ʾһ��8X16���ַ�
void LCD_P8x16Char(unsigned char x,unsigned char  y,unsigned char ch)
{
  unsigned char c=0,i=0,j=0;
  c =ch-32;
  if(x>120){x=0;y++;}
#ifdef OLED_WORK_MODE_I2C    
  OLED_Set_Pos(x,y);
  for(i=0;i<8;i++)
    OLED_WrDat(F8X16[c*16+i]);
  OLED_Set_Pos(x,y+1);
  for(i=0;i<8;i++)
    OLED_WrDat(F8X16[c*16+i+8]);
#else
  LCD_Set_Pos(x,y);
  for(i=0;i<8;i++)
    LCD_WrDat(F8X16[c*16+i]);
  LCD_Set_Pos(x,y+1);
  for(i=0;i<8;i++)
    LCD_WrDat(F8X16[c*16+i+8]);  
#endif   
  x+=8;
  j++;
}
/*---------------------��ʾ8X16�ĸ�����--------------------*/
void write_8_16_number(unsigned char x,unsigned char y, float number)
{
  unsigned char i=0;
  unsigned char temp[16];
  unsigned char *point=temp;
  float decimal;
  int data;
  
  if(number<0)
  {
    temp[0]='-';
    LCD_P8x16Char(x,y,temp[0]);
    x+=1;
    number=-number;
  }
  data=(int)number;
  decimal=number-data;     //С������
  if(data>=1000)           //�Ƿ�ɱ�1000����
  {
    temp[i]=48+data/1000;
    data=data%1000;
    i++;
  }
  if(data>=100)              //�ɷ�100����
  {
    temp[i]=48+data/100;
    data=data%100;
    i++;
  }
  else
    if(data<100&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=10)                  //�ɷ�10����
  {
    temp[i]=48+data/10;
    data=data%10;
    i++;
  }
  else
    if(data<10&&i!=0)
    {
      temp[i]=48;
      i++;
    }
  temp[i]=48+data;
  if(decimal>=0.0001f)           //�ж��ǹ�ΪС��
  {
    i++;
    temp[i]='.';                //��ʾС����
    i++;
    data=(int)(decimal*1000);
    temp[i]=48+data/100;
    data=data%100;
    i++;
    if(data>0)
    {
      temp[i]=48+data/10;
      data=data%10;
    }
    if(data>=0)
    {
      i++;
      temp[i]=data+48;
    }
  }
  i++;
  temp[i]='\0';
  LCD_P8x16Str(x,y,point);
}

//--------------------------------------------------------------
// Prototype      : void write_16_16_CN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); N:������codetab.h�е�����
// Description    : ��ʾcodetab.h�еĺ���,16*16����
//--------------------------------------------------------------
void write_16_16_CN(unsigned char x, unsigned char y, unsigned char N)
{
  unsigned char wm=0;
  unsigned int  adder=32*N;
#ifdef OLED_WORK_MODE_I2C    
  OLED_Set_Pos(x , y);
  for(wm = 0;wm < 16;wm++)
  {
    OLED_WrDat(F16x16[adder]);
    adder += 1;
  }
  OLED_Set_Pos(x,y + 1);
  for(wm = 0;wm < 16;wm++)
  {
    OLED_WrDat(F16x16[adder]);
    adder += 1;
  }
#else
  LCD_Set_Pos(x , y);
  for(wm = 0;wm < 16;wm++)
  {
    LCD_WrDat(F16x16[adder]);
    adder += 1;
  }
  LCD_Set_Pos(x,y + 1);
  for(wm = 0;wm < 16;wm++)
  {
    LCD_WrDat(F16x16[adder]);
    adder += 1;
  }
#endif  		
}

void LCD_clear_L(unsigned char x,unsigned char y)
{
#ifdef OLED_WORK_MODE_I2C  
  OLED_WrCmd(0xb0+y);
  OLED_WrCmd(0x01);
  OLED_WrCmd(0x10);
  OLED_Set_Pos(x,y);
  for(;x<X_WIDTH;x++)
  {
    OLED_WrDat(0);
  }
#else
  LCD_WrCmd(0xb0+y);
  LCD_WrCmd(0x01);
  LCD_WrCmd(0x10);
  LCD_Set_Pos(x,y);
  for(;x<X_WIDTH;x++)
  {
    LCD_WrDat(0);
  }        
#endif
}

void Draw_Logo(void)
{
  unsigned int ii=0;
  unsigned char x,y;
#ifdef OLED_WORK_MODE_I2C    
  for(y=0;y<8;y++)
  {
    OLED_Set_Pos(0,y);
    for(x=0;x<128;x++)
    {
      OLED_WrDat(LOGO128x64[ii++]);
    }
  }
#else
  for(y=0;y<8;y++)
  {
    LCD_Set_Pos(0,y);
    for(x=0;x<128;x++)
    {
      LCD_WrDat(LOGO128x64[ii++]);
    }
  }
#endif    
}

void Draw_Logo1(void)
{
  unsigned int ii=0;
  unsigned char x,y;
#ifdef OLED_WORK_MODE_I2C    
  for(y=0;y<8;y++)
  {
    OLED_Set_Pos(0,y);
    for(x=0;x<128;x++)
    {
      OLED_WrDat(YuYi_TC[ii++]);
      Delay_Ms(1);
    }
  }
#else
  for(y=0;y<8;y++)
  {
    LCD_Set_Pos(0,y);
    for(x=0;x<128;x++)
    {
      LCD_WrDat(YuYi_TC[ii++]);
      Delay_Ms(2);
    }
  }
#endif
  Delay_Ms(500);
  LCD_CLS();
}


//��ʾ����ʼ��
void OLEDInit(void)
{
  LCD_RSTout(Bit_RESET);
  Delay_Ms(100);
  LCD_RSTout(Bit_SET);//�ȴ�RC��λ���
  
  LCD_WrCmd(0xae);		//--turn off oled panel
  LCD_WrCmd(0x00);		//---set low column address
  LCD_WrCmd(0x10);		//---set high column address
  LCD_WrCmd(0x40);		//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  LCD_WrCmd(0x81);		//--set contrast control register
  LCD_WrCmd(0xcf); 		// Set SEG Output Current Brightness
  LCD_WrCmd(0xa0);		//--Set SEG/Column Mapping     0xa0???? 0xa1??
  LCD_WrCmd(0xc0);		//Set COM/Row Scan Direction   0xc0???? 0xc8??
  LCD_WrCmd(0xa6);		//--set normal display
  LCD_WrCmd(0xa8);		//--set multiplex ratio(1 to 64)
  LCD_WrCmd(0x3f);		//--1/64 duty
  LCD_WrCmd(0xd3);		//-set display offset Shift Mapping RAM Counter (0x00~0x3F)
  LCD_WrCmd(0x00);		//-not offset
  LCD_WrCmd(0xd5);		//--set display clock divide ratio/oscillator frequency
  LCD_WrCmd(0x80);		//--set divide ratio, Set Clock as 100 Frames/Sec
  LCD_WrCmd(0xd9);		//--set pre-charge period
  LCD_WrCmd(0xf1);		//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  LCD_WrCmd(0xda);		//--set com pins hardware configuration
  LCD_WrCmd(0x12);
  LCD_WrCmd(0xdb);		//--set vcomh
  LCD_WrCmd(0x40);		//Set VCOM Deselect Level
  LCD_WrCmd(0x20);		//-Set Page Addressing Mode (0x00/0x01/0x02)
  LCD_WrCmd(0x02);		//
  LCD_WrCmd(0x8d);		//--set Charge Pump enable/disable
  LCD_WrCmd(0x14);		//--set(0x10) disable
  LCD_WrCmd(0xa4);		// Disable Entire Display On (0xa4/0xa5)
  LCD_WrCmd(0xa6);		// Disable Inverse Display On (0xa6/a7)
  LCD_WrCmd(0xaf);		//--turn on oled panel
  LCD_Fill(0x00);  		//��ʼ����
  LCD_Set_Pos(0,0);
}

/***************************************************
������: void OLED_Init(void)
˵��:	OLED��ʾ����ʼ��
���:	��
����:	��
��ע:	�ϵ��ʼ��������һ��
****************************************************/
void OLED_Init(void)
{
#ifdef OLED_WORK_MODE_I2C 
  OLED_GPIO_Init();
  //OLED_Init_I2C();
  ssd1306_begin(SSD1306_SWITCHCAPVCC);   
#else
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIO_OLED);// Enable the GPIO port that is used for the on-board LED.
  GPIOPinTypeGPIOOutput(GPIO_OLED, GPIO_PIN_2);
  GPIOPinTypeGPIOOutput(GPIO_OLED, GPIO_PIN_3);
  GPIOPinTypeGPIOOutput(GPIO_OLED, GPIO_PIN_4);
  GPIOPinTypeGPIOOutput(GPIO_OLED, GPIO_PIN_5);	
  Delay_Ms(2);
  //OLEDInit();
  LCD_RSTout(Bit_RESET);
  Delay_Ms(100);
  LCD_RSTout(Bit_SET);//�ȴ�RC��λ���
  ssd1306_begin(SSD1306_SWITCHCAPVCC);  
#endif
  Draw_Logo1();
  LCD_CLS();
}


void OLED_Init_Fast(void)
{
#ifdef OLED_WORK_MODE_I2C 
  OLED_GPIO_Init();
  //OLED_Init_I2C();
  ssd1306_begin(SSD1306_SWITCHCAPVCC);  
#else
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIO_OLED);// Enable the GPIO port that is used for the on-board LED.
  GPIOPinTypeGPIOOutput(GPIO_OLED, GPIO_PIN_2);
  GPIOPinTypeGPIOOutput(GPIO_OLED, GPIO_PIN_3);
  GPIOPinTypeGPIOOutput(GPIO_OLED, GPIO_PIN_4);
  GPIOPinTypeGPIOOutput(GPIO_OLED, GPIO_PIN_5);	
  Delay_Ms(10);
  //OLEDInit(); 
  LCD_RSTout(Bit_RESET);
  Delay_Ms(100);
  LCD_RSTout(Bit_SET);//�ȴ�RC��λ���
  ssd1306_begin(SSD1306_SWITCHCAPVCC);  	
#endif
}

/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/

