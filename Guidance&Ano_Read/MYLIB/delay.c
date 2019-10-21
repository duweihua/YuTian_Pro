/* Includes */
#include "config.h"

/* delay��ʱ��ʼ�� */
void delay_init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ��HCLK�˷�Ƶ=168MHZ/8
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	SysTick->LOAD=20;	//21-1
}			


/* systick�δ�ʱ��ʵ��us����ʱ */
void delay_us(u32 ntime)
{
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;		//ʹ��systick
	
	while(ntime--)
	{
		while((SysTick->CTRL&0x010000)==0);		//systick������0 -> 1us
	}
	SysTick->CTRL&= ~SysTick_CTRL_ENABLE_Msk;	//ʧ��systick
}


/* systick�δ�ʱ��ʵ��ms����ʱ */
void delay_ms(u32 ntime)
{
	ntime*=1000;
	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	
	while(ntime--)
	{
		while((SysTick->CTRL&0x010000)==0);
	}
	SysTick->CTRL&= ~SysTick_CTRL_ENABLE_Msk;
}

/* end */


