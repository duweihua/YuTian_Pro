
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
#include "WP_ADC.h"
#include "hw_adc.h"
#include "adc.h"

uint32_t value[8];
double value_filter;
/***********************************************************
@��������ADC_Init
@��ڲ�������
@���ڲ�������
������������ѹ�����˿�PE3����ΪADC0��ͨ��0��ʼ�������ж�ʽ�ɼ�
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ADC_Init(void)//ADC��ʼ������   
{    
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);// Enable the ADC1 module.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));// Wait for the ADC1 module to be ready.	
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    
  GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
  //ADCReferenceSet(ADC0_BASE, ADC_REF_EXT_3V);
  //Enable the first sample sequencer to capture the value of channel 0 when
  //the processor trigger occurs.  
  ADCSequenceConfigure(ADC0_BASE,0,ADC_TRIGGER_PROCESSOR, 0); 
  //ADCHardwareOversampleConfigure(ADC0_BASE, 8);	
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0 |ADC_CTL_END | ADC_CTL_IE);    
	//ADCIntClear(ADC0_BASE, 0);
	ADCSequenceEnable(ADC0_BASE, 0);    
  //ADCIntEnable(ADC0_BASE, 0);   
} 


float Battery_Voltage;
/***********************************************************
@��������Get_Battery_Voltage
@��ڲ�������
@���ڲ�������
����������������ص�ѹ���ɿ�Ĭ�Ϸ�ѹ��λ11���ʲ�����ѹ��Ҫ����
3.3V*11=36.6V����������ĵ�ѹ�����������Ϸ�ѹ������ֵ����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void Get_Battery_Voltage(void)//ADC��ȡ   
{
	ADCProcessorTrigger(ADC0_BASE, 0);   
	while(!ADCIntStatus(ADC0_BASE, 0, false)) {;}
	ADCIntClear(ADC0_BASE, 0);	
	ADCSequenceDataGet(ADC0_BASE, 0, value);   
	//value[0] =  HWREG(ADC0_BASE+ ADC_SEQ + (ADC_SEQ_STEP*0) + ADC_SSFIFO);
	value_filter=(double)(0.7*value_filter+10.89*value[0]/4095.0f);
	Battery_Voltage=value_filter;		
}

/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/

