#include "stm32f10x.h"
#include "hcsr04.h"
#include "delay.h"
#include "usart.h"


int main(void)
{
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	TIM3_LJW_Init();
	HCSR_Init();
	
	
	while(1)
	{
		HCSR_Strat();
		delay_ms(200);
	}
}
