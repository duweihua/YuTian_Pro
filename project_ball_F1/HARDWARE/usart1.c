#include "stm32f10x.h"
#include "stdio.h"
#include "usart1.h"
#include "remote_control.h"
#include "data_solve.h"

//视觉数据数组(小球坐标)
volatile unsigned char Place_Data[4];

void usart_init()
{
	GPIO_InitTypeDef 		gpio;
	USART_InitTypeDef 	usart2;
	NVIC_InitTypeDef 		nvic;
	DMA_InitTypeDef			dma;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,  ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,   ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, 		ENABLE);
		
	//GPIOA.3_Init
	gpio.GPIO_Pin = GPIO_Pin_3;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//浮空输入
	GPIO_Init(GPIOA, &gpio);
  
  //USART2_Init
	usart2.USART_BaudRate = 115200;
	usart2.USART_WordLength = USART_WordLength_8b;
	usart2.USART_StopBits = USART_StopBits_1;
	usart2.USART_Parity = USART_Parity_Even;
	usart2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart2.USART_Mode = USART_Mode_Rx;	
	USART_Init(USART2, &usart2);
	
	//NVIC_Init
	nvic.NVIC_IRQChannel = USART2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 1;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//串口空闲中断
	USART_Cmd(USART2,ENABLE);
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
	
	//DMA_Init
	DMA_DeInit(DMA1_Channel6);
		
	dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);	
	dma.DMA_MemoryBaseAddr = (uint32_t)Place_Data;				
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma.DMA_BufferSize = 4;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_VeryHigh;
	dma.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA1_Channel6, &dma);

	DMA_Cmd(DMA1_Channel6, ENABLE);
}


void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)		
	{
    (void)USART2->SR;	
		(void)USART2->DR;	

	  DMA_Cmd(DMA1_Channel6,DISABLE);	
		
		Place_Data_Slove(Place_Data);
		
		DMA_SetCurrDataCounter(DMA1_Channel6,4);	
		
		DMA_Cmd(DMA1_Channel6,ENABLE);
  }
}



