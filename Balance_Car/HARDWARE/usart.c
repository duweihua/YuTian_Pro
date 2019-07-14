#include "main.h"


#define usart2_dma_send_buffersize 13
#define usart2_dma_rece_buffersize 11

uint8_t usart2_dma_send_buffer[usart2_dma_send_buffersize];
uint8_t usart2_dma_rece_buffer[usart2_dma_rece_buffersize];

RC_t rc;

u8 Flag_Front,Flag_Back,Flag_Left,Flag_Right;

u8 Front_Back_Change;

extern u8 received_flag;


//重定义fputc函数 
int fputc(int ch, FILE *f)
{
	//循环发送,直到发送完毕   
	while((USART1->SR&0X40)==0);
	
    USART1->DR = (u8) ch;      
	
	return ch;
}


void usart1_init(void)
{
	GPIO_InitTypeDef		gpio;
	USART_InitTypeDef 		uart;
	NVIC_InitTypeDef 		nvic;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_9; 
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOA, &gpio);

	nvic.NVIC_IRQChannel = USART1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority=3;
	nvic.NVIC_IRQChannelSubPriority = 3;		
	nvic.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&nvic);	
  
	uart.USART_BaudRate = 115200;
	uart.USART_WordLength = USART_WordLength_8b;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Tx;
	USART_Init(USART1,&uart); 				

	USART_Cmd(USART1,ENABLE);      
}


void usart2_init(void)
{
	GPIO_InitTypeDef 	gpio;
	USART_InitTypeDef 	usart2;
	NVIC_InitTypeDef 	nvic;
	DMA_InitTypeDef		dma;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	gpio.GPIO_Pin = GPIO_Pin_3;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_Init(GPIOA,&gpio);

	//蓝牙模块要求波特率9600
	usart2.USART_BaudRate = 9600;
	usart2.USART_WordLength = USART_WordLength_8b;
	usart2.USART_StopBits = USART_StopBits_1;
	usart2.USART_Parity = USART_Parity_Even;
	usart2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart2.USART_Mode = USART_Mode_Rx;	
	USART_Init(USART2,&usart2);

	nvic.NVIC_IRQChannel = USART2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	USART_ITConfig(USART2, USART_IT_IDLE,ENABLE);
	
	USART_Cmd(USART2,ENABLE);
	
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	
	DMA_DeInit(DMA1_Channel6);
	
	dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);	
	dma.DMA_MemoryBaseAddr = (uint32_t)usart2_dma_rece_buffer;				
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma.DMA_BufferSize = usart2_dma_rece_buffersize;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_VeryHigh;
	dma.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel6,&dma);
	
	nvic.NVIC_IRQChannel = DMA1_Channel6_IRQn;           
	nvic.NVIC_IRQChannelPreemptionPriority = 0;          
	nvic.NVIC_IRQChannelSubPriority = 0; 
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);

	//很多时候先不使能
	//此处直接使能
	DMA_Cmd(DMA1_Channel6,ENABLE);
}


u8 RC_Data_Solve(void)
{
	//遥控器数据解码
	rc.x = ((uint16_t)usart2_dma_rece_buffer[0]<<8)|((uint16_t)usart2_dma_rece_buffer[1]<<0);
	rc.y = ((uint16_t)usart2_dma_rece_buffer[2]<<8)|((uint16_t)usart2_dma_rece_buffer[3]<<0);
	
	rc.Key_F  = usart2_dma_rece_buffer[4];
	rc.Key_B  = usart2_dma_rece_buffer[5];
	rc.Key_L  = usart2_dma_rece_buffer[6];
	rc.Key_R  = usart2_dma_rece_buffer[7];
	rc.Key_SW = usart2_dma_rece_buffer[8];
	
	rc.Check1  = usart2_dma_rece_buffer[9];
	rc.Check2  = usart2_dma_rece_buffer[10];
	
	if((rc.x>(2048-20))&&(rc.x<(2048+20)))
		rc.x = 2048;

	if((rc.y>(2048-20))&&(rc.y<(2048+20)))
		rc.y = 2048;
	
	if(rc.Key_F != rc.Key_F_Last)
	{
		if(Flag_Front == 1)
			Flag_Front = 0;
		else if(Flag_Front == 0)
			Flag_Front = 1;
		
		Flag_Back = 0;
//		Flag_Left = 0;
//		Flag_Right = 0;
		
		Front_Back_Change = 1;
		
		rc.Key_F_Last = rc.Key_F;
	}
	
	if(rc.Key_B != rc.Key_B_Last)
	{
		if(Flag_Back == 1)
			Flag_Back = 0;
		else if(Flag_Back == 0)
			Flag_Back = 1;
		
		Flag_Front = 0;
//		Flag_Left = 0;
//		Flag_Right = 0;
		
		Front_Back_Change = 1;
		
		rc.Key_B_Last = rc.Key_B;
	}

	//左右按键按下时 收到数据为0
	if(rc.Key_L == 0)
	{
		Flag_Left = 1;
//		Flag_Right = 0;
	}
	else if(rc.Key_L == 1)
	{
		Flag_Left = 0;
//		Flag_Right = 0;
	}
	
	if(rc.Key_R == 0)
	{
		Flag_Right = 1;
//		Flag_Left = 0;
	}
	else if (rc.Key_R == 1)
	{
//		Flag_Left = 0;
		Flag_Right = 0;
	}

//	if(rc.Key_L != rc.Key_L_Last)
//	{
//		Flag_Front = 0;
//		Flag_Back = 0;
//		Flag_Left = 1;
//		Flag_Right = 0;
//		
//		Front_Back_Change = 1;
//		
//		rc.Key_L_Last = rc.Key_L;
//	}
//	
//	if(rc.Key_R != rc.Key_R_Last)
//	{
//		Flag_Front = 0;
//		Flag_Back = 0;
//		Flag_Left = 0;
//		Flag_Right = 1;
//		
//		Front_Back_Change = 1;
//		
//		rc.Key_R_Last = rc.Key_R;
//	}
	
	if((rc.Check1 == 0x0A)&&(rc.Check2 == 0x0D))
		return 0;
	else
		return 1;
}


//DMA接收完成中断
void DMA1_Channel6_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC6))
	{
		if((usart2_dma_rece_buffer[9] == 0x0A)&&(usart2_dma_rece_buffer[10] == 0x0D))
		{
			RC_Data_Solve();
			
			received_flag = 1;
		}
		
		//重置DMA
		else
		{
			received_flag = 0;

			//延时——数据自动对齐
			//可能DMA无延时重置会被忽略
			delay_ms(20);
			
			DMA_Cmd(DMA1_Channel6,DISABLE);
			DMA_SetCurrDataCounter(DMA1_Channel6,usart2_dma_rece_buffersize);	
			DMA_Cmd(DMA1_Channel6,ENABLE);
		}
		
		DMA_ClearFlag(DMA1_IT_TC6);
		DMA_ClearITPendingBit(DMA1_IT_TC6);     
	}
}


void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		(void)USART2->SR;
		(void)USART2->DR;

//		DMA_Cmd(DMA1_Channel6,DISABLE);	
//				
//		DMA_SetCurrDataCounter(DMA1_Channel6,usart2_dma_buffer_size);	
//		
//		DMA_Cmd(DMA1_Channel6,ENABLE);
  }
}







