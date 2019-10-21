/* Includes */
#include "config.h"

/* delay延时初始化 */
void delay_init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择HCLK八分频=168MHZ/8
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	SysTick->LOAD=20;	//21-1
}			


/* systick滴答定时器实现us级延时 */
void delay_us(u32 ntime)
{
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;		//使能systick
	
	while(ntime--)
	{
		while((SysTick->CTRL&0x010000)==0);		//systick计数到0 -> 1us
	}
	SysTick->CTRL&= ~SysTick_CTRL_ENABLE_Msk;	//失能systick
}


/* systick滴答定时器实现ms级延时 */
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


