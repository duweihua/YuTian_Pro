#include "Drv_Bsp.h"
#include "pwm.h"
#include "Drv_RcIn.h"
#include "Drv_Spi.h"
#include "Drv_Led.h"
#include "Drv_Paramter.h"
#include "Drv_icm20602.h"
#include "drv_ak8975.h"
#include "drv_spl06.h"
#include "Drv_PwmOut.h"
#include "Drv_Adc.h"
#include "Drv_Uart.h"
#include "Ano_FcData.h"
#include "Ano_Sensor_Basic.h"
#include "Ano_RC.h"
#include "Ano_FlightCtrl.h"
#include "Drv_gps.h"
#include "ano_usb.h"

static uint64_t SysRunTimeMs = 0;

void SysTick_Init(void )
{
	ROM_SysTickPeriodSet(ROM_SysCtlClockGet()/1000);
	ROM_SysTickIntEnable();
	ROM_SysTickEnable();
}
void SysTick_Handler(void)
{
	SysRunTimeMs++;
}
uint32_t GetSysRunTimeMs(void)
{
	return SysRunTimeMs;
}
uint32_t GetSysRunTimeUs(void)
{
	return SysRunTimeMs*1000 + (SysTick->LOAD - SysTick->VAL) * 1000 / SysTick->LOAD;
}

void MyDelayMs(u32 time)
{
	ROM_SysCtlDelay(80000 * time /3);
}

void Drv_SenserCsPinInit(void)
{
	Drv_Icm20602CSPinInit();
	Drv_AK8975CSPinInit();
	Drv_SPL06CSPinInit();
	
	ROM_SysCtlPeripheralEnable(FLASH_CSPIN_SYSCTL);
	ROM_GPIOPinTypeGPIOOutput(FLASH_CS_PORT,FLASH_CS_PIN);
	ROM_GPIOPinWrite(FLASH_CS_PORT, FLASH_CS_PIN,FLASH_CS_PIN);
}

void Drv_BspInit(void)
{
	/*����ϵͳ��ƵΪ80M*/
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |SYSCTL_OSC_MAIN);
	/*�ж����ȼ��������*/
	NVIC_SetPriorityGrouping(0x03);
	/*�����������㵥Ԫ*/	
	ROM_FPULazyStackingEnable();
	ROM_FPUEnable();
	
	//���ݳ�ʼ��
	Dvr_ParamterInit();
	//��ȡ��ʼ����
	Para_Data_Init();
	//�ƹ��ʼ��
	Dvr_LedInit();
	//����USB���⴮�ڳ�ʼ��
	AnoUsbCdcInit();
	
	//ң�ؽ���ģʽ��ʼ��
	Remote_Control_Init();

	//spiͨ�ų�ʼ��
	Drv_Spi0Init();
	Drv_SenserCsPinInit();
	//��ʼ��ICM
	sens_hd_check.acc_ok = sens_hd_check.gyro_ok =
	Drv_Icm20602Init();
	//��ʼ����ѹ��
	sens_hd_check.baro_ok = Drv_Spl0601Init();
	//�������OK���������̲�������㣨ע���˴�û���������Ƿ������ļ�����
	sens_hd_check.mag_ok = 1;       //	
	
	//��������ʼ��
	Drv_PwmOutInit();
	//ADC��ʼ��
	Drv_AdcInit();
	//�δ�ʱ�ӳ�ʼ��
	SysTick_Init();	
	//���ڳ�ʼ��
	Drv_Uart4Init(500000);	//�ӹ���
	Drv_Uart2Init(500000);	//������
	Drv_Uart1Init(115200);	//������
	Drv_GpsPin_Init();
	
	//====fc
	//�ɿش����������ʼ��
	Sensor_Basic_Init();	
	//�ɿ�PID��ʼ��
	All_PID_Init();
}




