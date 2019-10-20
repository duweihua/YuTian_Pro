/* Includes */
#include "uart.h"
#include "data_solve.h"
#include "delay.h"

/* data define */

/* initializes the USART2 */
void Usart2_Init (void)
{
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd ( RCC_APB1Periph_USART2, ENABLE ); //开启USART2时钟
	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOD, ENABLE );

	//串口中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init ( &NVIC_InitStructure );

	GPIO_PinAFConfig ( GPIOD, GPIO_PinSource5, GPIO_AF_USART2 );
	GPIO_PinAFConfig ( GPIOD, GPIO_PinSource6, GPIO_AF_USART2 );

	//配置PD5作为USART2　Tx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init ( GPIOD, &GPIO_InitStructure );
	//配置PD6作为USART2　Rx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init ( GPIOD, &GPIO_InitStructure );

	//配置USART2
	//中断被屏蔽了
	USART_InitStructure.USART_BaudRate = 115200;       //波特率可以通过地面站配置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8位数据
	USART_InitStructure.USART_StopBits = USART_StopBits_1;   //在帧结尾传输1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;    //禁用奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //发送、接收使能
	//配置USART2时钟
	USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;  //时钟低电平活动
	USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;  //SLCK引脚上时钟输出的极性->低电平
	USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;  //时钟第二个边沿进行数据捕获
	USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable; //最后一位数据的时钟脉冲不从SCLK输出

	USART_Init ( USART2, &USART_InitStructure );
	USART_ClockInit ( USART2, &USART_ClockInitStruct );

	//使能USART2接收中断
	USART_ITConfig ( USART2, USART_IT_RXNE, ENABLE );
	
	/* IDLE interrupt can not go in */
//	USART_ITConfig ( USART2, USART_IT_IDLE, ENABLE );
	
	//使能USART2
	USART_Cmd ( USART2, ENABLE );
}

u8 count = 0;
u8 Rx_Buf[256];	//串口接收缓存
void USART2_IRQHandler ( void )
{
	u8 com_data;

	/* receive interrupt */
	if(USART_GetITStatus(USART2,USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);

		com_data = USART2->DR;
		
		Guidance_DT_Data_Receive_Prepare(com_data);
	}
	
//	if ( USART_GetITStatus ( USART2, USART_IT_IDLE ) )
//	{
//		(void)USART2->DR;
//		(void)USART2->SR;
//	}
}

/* end */
