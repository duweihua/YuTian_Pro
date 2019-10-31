#include "main.h"

/*
	说明：
	
		HX711模块5V供电
	
		每次改变测试架状态按REST键调零
*/


/*已使用IO：
		
	HX711
		DATA		--> PA0
		SCK			--> PA1
		
	USART
		USART_TX	--> PA9
		USART_RX	--> PA10
					
	OLED
		IIC_SCL		--> PB10
		IIC_SDA		--> PB11

	LED
		LED			--> PC13

*/

u8 ConfigOK = 0;
u8 DataOK = 0;

int main(void)
{
	system_Init();
	delay_ms(100);
	data_init();
	delay_ms(100);
	OLED_Fill(0x00);
	
		
	while(1)
	{
//		Get_Weight();
//		delay_ms(100);
//		LED_TOGGLE();
	}
}


void system_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	LED_Configuration();
	usart_init();
	Sensor_Init();
	I2C_Configuration();
	OLED_Init();
//	Get_No_Lode();
	Tim1_configration();
	ConfigOK = 1;
}


void data_init(void)
{
	DataOK = 1;
}
	








