/* Includes */
#include "uart.h"
#include "data_solve.h"
#include "delay.h"

/* data define */


/* initializes the UART2 for reading guidance data */
/* UART2_Tx！！PD5
	 UART2_Rx！！PD6 */
void Usart2_Init (void)
{
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd ( RCC_APB1Periph_USART2, ENABLE );
	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOD, ENABLE );

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init ( &NVIC_InitStructure );

	GPIO_PinAFConfig ( GPIOD, GPIO_PinSource5, GPIO_AF_USART2 );
	GPIO_PinAFConfig ( GPIOD, GPIO_PinSource6, GPIO_AF_USART2 );

	/* UART2_Tx！！PD5 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init ( GPIOD, &GPIO_InitStructure );
	
	/* UART2_Rx！！PD6 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init ( GPIOD, &GPIO_InitStructure );

	USART_InitStructure.USART_BaudRate = 115200;      					
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 			
	USART_InitStructure.USART_Parity = USART_Parity_No;   			
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx; 

	USART_ClockInitStruct.USART_Clock = USART_Clock_Disable; 		
	USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;  				
	USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge; 				
	USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable;

	USART_Init ( USART2, &USART_InitStructure );
	USART_ClockInit ( USART2, &USART_ClockInitStruct );

	USART_ITConfig ( USART2, USART_IT_RXNE, ENABLE );

	USART_Cmd ( USART2, ENABLE );
}

/* Uart2 rece interrupt 
	 function: read and solve guidance data */
void USART2_IRQHandler ( void )
{
	u8 com_data;

	/* read one byte each time */
	if(USART_GetITStatus(USART2,USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);

		com_data = USART2->DR;
		
		Guidance_DT_Data_Receive_Prepare(com_data);
	}
}

/* initializes the UART4 for reading ANO_OF data */
/* UART4_Tx！！PA0
	 UART4_Rx！！PA1 */
void Uart4_Init (void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd ( RCC_APB1Periph_UART4, ENABLE );
	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA, ENABLE );

	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init ( &NVIC_InitStructure );

	GPIO_PinAFConfig ( GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
	GPIO_PinAFConfig ( GPIOA, GPIO_PinSource1, GPIO_AF_UART4);

	/* UART4_Tx！！PA0 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init ( GPIOA, &GPIO_InitStructure );
	
	/* UART4_Rx！！PA1 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init ( GPIOA, &GPIO_InitStructure );

	USART_InitStructure.USART_BaudRate = 500000;       															
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  										
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  												
	USART_InitStructure.USART_Parity = USART_Parity_No;   													
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  								
	USART_Init ( UART4, &USART_InitStructure );

	USART_ITConfig ( UART4, USART_IT_RXNE, ENABLE );
	
	USART_Cmd ( UART4, ENABLE );
}

u8 Tx4Buffer[256];
u8 Tx4Counter = 0;
u8 count4 = 0;
/* Uart4 rece interrupt 
	 function: read and solve ANO_OF data */
void UART4_IRQHandler ( void )
{
	u8 com_data;

	if ( USART_GetITStatus ( UART4, USART_IT_RXNE ) )
	{
		USART_ClearITPendingBit ( UART4, USART_IT_RXNE ); 

		com_data = UART4->DR;

		AnoOF_GetOneByte ( com_data );
	}
}


/* end */
