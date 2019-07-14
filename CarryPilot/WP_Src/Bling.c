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
#include "Bling.h"

/***************************************************
������: void Bling_Init(void)
˵��:	LED״ָ̬ʾ�Ƴ�ʼ��
���:	��
����:	��
��ע:	�ϵ��ʼ��������һ��
****************************************************/
void Bling_Init()
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  HWREG(GPIO_PORTC_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY; 
  HWREG(GPIO_PORTC_BASE + GPIO_O_CR) = 0x000000FF;
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2 |GPIO_PIN_3);
  GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_2);
}


Bling_Light Light_1,Light_2,Light_3,Light_4;
uint16_t Bling_Mode=0;
/***************************************************
������: void Bling_Set(Bling_Light *Light,
uint32_t Continue_time,//����ʱ��
uint16_t Period,//����100ms~1000ms
float Percent,//0~100%
uint16_t  Cnt,
GPIO_TypeDef* Port,
uint16_t Pin
,uint8_t Flag)
˵��:	״ָ̬ʾ�����ú���
���:	ʱ�䡢���ڡ�ռ�ձȡ��˿ڵ�
����:	��
��ע:	�����ʼ����ʼ������
****************************************************/
void Bling_Set(Bling_Light *Light,
               uint32_t Continue_time,//����ʱ��
               uint16_t Period,//����100ms~1000ms
               float Percent,//0~100%
               uint16_t  Cnt,
               uint32_t Port,
               uint16_t Pin
                 ,uint8_t Flag)
{
  Light->Bling_Contiune_Time=(uint16_t)(Continue_time/5);//����ʱ��
  Light->Bling_Period=Period;//����
  Light->Bling_Percent=Percent;//ռ�ձ�
  Light->Bling_Cnt=Cnt;
  Light->Port=Port;//�˿�
  Light->Pin=Pin;//����
  Light->Endless_Flag=Flag;//�޾�ģʽ
}

/***************************************************
������: void Bling_Process(Bling_Light *Light)//��˸�����߳�
˵��:	״ָ̬ʾ��ʵ��
���:	״̬�ƽṹ��
����:	��
��ע:	�����ʼ����ʼ������
****************************************************/
void Bling_Process(Bling_Light *Light)//��˸�����߳�
{
  if(Light->Bling_Contiune_Time>=1)  Light->Bling_Contiune_Time--;
  else GPIOPinWrite(Light->Port,Light->Pin,Light->Pin);//�ø�,��
  if(Light->Bling_Contiune_Time!=0//��ʱ��δ��0
     ||Light->Endless_Flag==1)//�ж��޾�ģʽ�Ƿ���
  {
    Light->Bling_Cnt++;
    if(5*Light->Bling_Cnt>=Light->Bling_Period) Light->Bling_Cnt=0;//��������
    if(5*Light->Bling_Cnt<=Light->Bling_Period*Light->Bling_Percent)
      GPIOPinWrite(Light->Port,Light->Pin,Light->Pin);//�øߣ���
    else GPIOPinWrite(Light->Port,Light->Pin,0);//�õͣ���
  }
}


void GPIO_SetBits(Bling_Light *Light)
{
  GPIOPinWrite(Light->Port,Light->Pin,Light->Pin);//�øߣ���
}

void GPIO_ResetBits(Bling_Light *Light)
{
  GPIOPinWrite(Light->Port,Light->Pin,0);//�õͣ���
}
/***************************************************
������: Bling_Working(uint16 bling_mode)
˵��:	״ָ̬ʾ��״̬��
���:	��ǰģʽ
����:	��
��ע:	�����ʼ����ʼ������
****************************************************/
void Bling_Working(uint16 bling_mode)
{
  if(bling_mode==0)//ȫ��
  {
    Bling_Process(&Light_1);
    Bling_Process(&Light_2);
    Bling_Process(&Light_3);
  }
  else if(bling_mode==1)//���ٶȼ�6��У׼ģʽ
  {
    if(flight_direction==0)//��һ��У׼׼��
    {
      Bling_Process(&Light_1);
      GPIO_SetBits(&Light_2);
      GPIO_SetBits(&Light_3);
    }
    else if(flight_direction==1)//�ڶ���У׼׼��
    {
      Bling_Process(&Light_2);
      GPIO_SetBits(&Light_1);
      GPIO_SetBits(&Light_3);
    }
    else if(flight_direction==2)//������У׼׼��
    {
      Bling_Process(&Light_1);
      Bling_Process(&Light_2);
      GPIO_SetBits(&Light_3);
    }
    else if(flight_direction==3)//������У׼׼��
    {
      Bling_Process(&Light_3);
      GPIO_SetBits(&Light_1);
      GPIO_SetBits(&Light_2);
    }
    else if(flight_direction==4)//������У׼׼��
    {
      Bling_Process(&Light_1);
      Bling_Process(&Light_3);
      GPIO_SetBits(&Light_2);
    }
    else if(flight_direction==5)//������У׼׼��
    {
      GPIO_SetBits(&Light_1);
      Bling_Process(&Light_2);
      Bling_Process(&Light_3);
    }
    else
    {
      Bling_Process(&Light_1);
      Bling_Process(&Light_2);
      Bling_Process(&Light_3);
    }
  }
  else if(bling_mode==2)//������У׼ģʽ
  {
    if(Mag_Calibration_Mode==0)//ˮƽ��У׼
    {
      Bling_Process(&Light_1);
      GPIO_SetBits(&Light_2);
      GPIO_SetBits(&Light_3);
    }
    else if(Mag_Calibration_Mode==1)////��ֱƽ��У׼
    {
      GPIO_SetBits(&Light_1);
      Bling_Process(&Light_2);
      GPIO_SetBits(&Light_3);
    }
    else if(Mag_Calibration_Mode==2)////��ֱƽ��У׼
    {
      GPIO_SetBits(&Light_1);
      GPIO_SetBits(&Light_2);
      Bling_Process(&Light_3);
    }
    else
    {
      Bling_Process(&Light_1);
      Bling_Process(&Light_2);
      Bling_Process(&Light_3);
    }
  }
  else if(bling_mode==3)//ȫ��
  {
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,GPIO_PIN_3);
  }
  
  if(((Last_GPS_Sate_Num!=GPS_Sate_Num)&&GPS_Sate_Num<=5)
     ||Filter_Defeated_Flag==1)//�����һֱ����
    Bling_Set(&Light_4,1000,1000,0.5,0,GPIO_PORTC_BASE,GPIO_PIN_2,1);
  else if((Last_GPS_Sate_Num!=GPS_Sate_Num)&&GPS_Sate_Num<=9)
    Bling_Set(&Light_4,2000,2000,0.5,0,GPIO_PORTC_BASE,GPIO_PIN_2,1);
  else if((Last_GPS_Sate_Num!=GPS_Sate_Num)&&GPS_Sate_Num>=10)
    Bling_Set(&Light_4,2000,5000,0.5,0,GPIO_PORTC_BASE,GPIO_PIN_2,1);
  Bling_Process(&Light_4);
  
  
}


/***************************************************
������: void Quad_Start_Bling(void)
˵��:	LED��ʼ���󿪻���˸
���:	��
����:	��
��ע:	�ϵ��ʼ��������һ��
****************************************************/
void Quad_Start_Bling()
{
  Bling_Set(&Light_1,2000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
  Bling_Set(&Light_2,2000,500,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
  Bling_Set(&Light_3,2000,800,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
  Bling_Set(&Light_4,2000,1000,0.5,0,GPIO_PORTC_BASE,GPIO_PIN_2,1);
}
/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/


