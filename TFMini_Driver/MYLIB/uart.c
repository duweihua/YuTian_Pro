/* include */ 
#include "stm32f10x.h"
#include "stdio.h"
#include "uart.h"

#define USART3_DMA_rece_buffersize	20
uint8_t USART3_DMA_rece_buffer[USART3_DMA_rece_buffersize] = {0};

uint16_t distance = 0;

/* define */

void vUart1Config(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_Init(USART1,&USART_InitStructure);
	
	USART_Cmd(USART1,ENABLE);
}

int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0);
	USART1->DR = (u8)ch;
	return ch;
}

void vUart3Config(void)
{  
  GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;
	DMA_InitTypeDef		DMA_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	//USART3_RX	  GPIOB.11		(using)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  

	//USART3_TX   GPIOB.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART3, &USART_InitStructure);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART3, ENABLE); 
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//	//DMA¡ª¡ªRX
//	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
//	
//	DMA_DeInit(DMA1_Channel3);
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART3->DR));
//	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART3_DMA_rece_buffer;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//	DMA_InitStructure.DMA_BufferSize = USART3_DMA_rece_buffersize;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel3,&DMA_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;           
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	
//	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
//	
//	DMA_Cmd(DMA1_Channel3,ENABLE);
}

uint8_t temp_data = 0;
uint8_t TFmini_low,TFmini_high = 0;
uint8_t flag = 0;
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{
		temp_data = USART_ReceiveData(USART3);
		
		if((temp_data == 0x59) && (flag == 0))
		{
			flag = 1;
		}
		else if((temp_data == 0x59) && (flag == 1))
		{
			flag = 2;
		}
		else if(flag == 2)
		{
			TFmini_low = temp_data;
			
			flag = 3;
		}
		else if(flag == 3)
		{
			TFmini_high = temp_data;
			
			distance = ((uint16_t)TFmini_high<<8) | ((uint16_t)TFmini_low);
			
			printf("receive once£º  %d cm \r\n",distance);
			
			flag = 0;
		}
	}
}


//void USART3_IRQHandler(void)
//{
//	static int DATA_LENGTH=0;

//	if(USART_GetITStatus(USART3,USART_IT_IDLE) != RESET)
//	{
//		(void)USART3->SR;
//		(void)USART3->DR;

//		DMA_Cmd(DMA1_Channel3,DISABLE);
//		
//		DATA_LENGTH = USART3_DMA_rece_buffersize - DMA_GetCurrDataCounter(DMA1_Channel3);
//		
//		if(DATA_LENGTH == 9)
//		{
//			if((USART3_DMA_rece_buffer[0] == 0x59) && (USART3_DMA_rece_buffer[1] == 0x59))
//			{
//				distance = ((uint16_t)USART3_DMA_rece_buffer[3]<<8)|((uint16_t)USART3_DMA_rece_buffer[2]);
//			}
//		}
//		
//		DMA_SetCurrDataCounter(DMA1_Channel3,USART3_DMA_rece_buffersize);
//		
//		DMA_Cmd(DMA1_Channel3,ENABLE);
//	}
//}

//void DMA1_Channel3_IRQHandler(void)
//{
//	if(DMA_GetITStatus(DMA1_IT_TC3))
//	{
//		DMA_ClearFlag(DMA1_IT_TC3);
//		DMA_ClearITPendingBit(DMA1_IT_TC3);
//	}
//}





/* end */











