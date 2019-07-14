
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
#include "WP_PWM.h"
#define PWM_Period_MAX  3125//2.5ms��������400hz
static uint16_t period;
/***********************************************************
@��������Init_PWM
@��ڲ�������
@���ڲ�������
@����������PWM��ʼ��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Init_PWM(void)
{
  SysCtlPWMClockSet(SYSCTL_PWMDIV_64); // Set divider to 80M/8=10M=0.1us
  SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); // Enable PWM peripheral
  SysCtlDelay(2); // Insert a few cycles after enabling the peripheral to allow the clock to be fully activated
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // Enable GPIOB peripheral
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); // Enable GPIOC peripheral
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // Enable GPIOE peripheral
  SysCtlDelay(2); // Insert a few cycles after enabling the peripheral to allow the clock to be fully activated
  // Use alternate function
  GPIOPinConfigure(GPIO_PB6_M0PWM0);
  GPIOPinConfigure(GPIO_PB7_M0PWM1);
  GPIOPinConfigure(GPIO_PB4_M0PWM2);
  GPIOPinConfigure(GPIO_PB5_M0PWM3);
  
  GPIOPinConfigure(GPIO_PE4_M0PWM4);
  GPIOPinConfigure(GPIO_PE5_M0PWM5);
  GPIOPinConfigure(GPIO_PC4_M0PWM6);
  GPIOPinConfigure(GPIO_PC5_M0PWM7);
  // Use pin with PWM peripheral
  GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);//M0PWM0
  GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_7);//M0PWM1
  GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_4);//M0PWM2
  GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_5);//M0PWM3
  
  GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_4);//M0PWM4
  GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_5);//M0PWM5
  GPIOPinTypePWM(GPIO_PORTC_BASE, GPIO_PIN_4);//M0PWM6
  GPIOPinTypePWM(GPIO_PORTC_BASE, GPIO_PIN_5);//M0PWM7
  // Configure the PWM generator for count down mode with immediate updates to the parameters
  PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
  PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
  
  PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
  PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
  // The period is set to 2.5ms (400 Hz)
  period = PWM_Period_MAX; 
  PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, period); // Set the period
  PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, period);
  
  PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, period); // Set the period
  PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, period);
  // Start the timers in generator 0 and 1
  PWMGenEnable(PWM0_BASE, PWM_GEN_0);
  PWMGenEnable(PWM0_BASE, PWM_GEN_1);
  
  PWMGenEnable(PWM0_BASE, PWM_GEN_2);
  PWMGenEnable(PWM0_BASE, PWM_GEN_3);
  // Enable the outputs
  PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT | PWM_OUT_2_BIT | PWM_OUT_3_BIT
                 | PWM_OUT_4_BIT | PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);
  PWM_Output(1250,1250,1250,1250,1250,1250,1250,1250);
}


/***********************************************************
@��������PWM_Output
@��ڲ�����uint16_t width1,uint16_t width2,uint16_t width3,
uint16_t width4,uint16_t width5,uint16_t width6,
uint16_t width7,uint16_t width8
@���ڲ�������
@����������PWM���
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void PWM_Output(uint16_t width1,uint16_t width2,uint16_t width3,uint16_t width4,
                uint16_t width5,uint16_t width6,uint16_t width7,uint16_t width8)
{
  PWMPulseWidthSet(PWM0_BASE,PWM_OUT_6,width1);//PC4 
  PWMPulseWidthSet(PWM0_BASE,PWM_OUT_7,width2);//PC5  
  PWMPulseWidthSet(PWM0_BASE,PWM_OUT_1,width3);//PB7
  PWMPulseWidthSet(PWM0_BASE,PWM_OUT_0,width4);//PB6
  PWMPulseWidthSet(PWM0_BASE,PWM_OUT_5,width5);//PE5
  PWMPulseWidthSet(PWM0_BASE,PWM_OUT_4,width6);//P43
  PWMPulseWidthSet(PWM0_BASE,PWM_OUT_2,width7);//PB4
  PWMPulseWidthSet(PWM0_BASE,PWM_OUT_3,width8);//PB5
}


/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/

