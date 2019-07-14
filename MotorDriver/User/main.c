#include "main.h"

//程序卡在B.处可能是使能了中断但是忘记做中断处理

/*已使用IO：

	ENCODER1
		TIM2_CH1	--> PA0
		TIM2_CH2	-->	PA1

	ADC
		ADC1_CH1	--> PA2
		ADC1_CH2	--> PA3
		ADC1_CH3	--> PA4

	ENCODER2
		TIM3_CH1	--> PA6
		TIM3_CH2 	-->	PA7

	USART
		USART_TX	--> PA9
		USART_RX	--> PA10

	CAN
		CAN_RX		--> PA11
		CAN_TX		--> PA12

	PWM 
		TIM4_CH1	--> PB6
		TIM4_CH2 	-->	PB7
		TIM4_CH3  	-->	PB8
		TIM4_CH4 	-->	PB9
		
	LED
		LED1 		--> PB12
		LED2		--> PB13
*/


u8 ConfigOK = 0;
u8 DataOK = 0;

extern u8 received_flag;


int main(void)
{
	system_Init();
	delay_ms(100);
	data_init();
	delay_ms(100);
	while(1);
}


void system_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	LED_Configuration();
	usart_init();
	CAN_Configuration();
	ADC_Configuration();
	Tim1_configration();
	Encoder_init();
	TIM4_PWM_Init();
	ConfigOK = 1;
}


void data_init(void)
{
	while(!received_flag);
	PID1_Init();
	PID2_Init();
	DataOK = 1;
}
	








