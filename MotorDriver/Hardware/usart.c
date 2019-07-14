#include "main.h"


#define USART1_DMA_send_buffersize 13
#define USART1_DMA_rece_buffersize 4

uint8_t USART1_DMA_send_buffer[USART1_DMA_send_buffersize];
uint8_t USART1_DMA_rece_buffer[USART1_DMA_rece_buffersize];

extern short pos1;
extern short pos2;

extern float nowcurrent1;
extern float nowcurrent2;

extern float Global_SetCurrent1;
extern float Global_SetCurrent2;

uint16_t USART1_Rece_Data;

extern RC_t rc;

u8 received_flag;

extern PID pid_Position1;
extern PID pid_Position2;


void usart_init()
{
	GPIO_InitTypeDef 	gpio;
	USART_InitTypeDef 	uart;
	NVIC_InitTypeDef 	nvic;
	DMA_InitTypeDef 	dma;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_9; 
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_10;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&gpio);

	nvic.NVIC_IRQChannel = USART1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority=0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	uart.USART_BaudRate = 9600;//为适应蓝牙模块//115200
	uart.USART_WordLength = USART_WordLength_8b;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;

	USART_Init(USART1,&uart);
	USART_Cmd(USART1,ENABLE);
	//串口空闲中断 
//	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);

	//DMA——TX
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);	
	
	DMA_DeInit(DMA1_Channel4);
	dma.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));
	dma.DMA_MemoryBaseAddr = (uint32_t)USART1_DMA_send_buffer;
	//存储器到外设
	dma.DMA_DIR = DMA_DIR_PeripheralDST;
	dma.DMA_BufferSize = USART1_DMA_send_buffersize;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	//循环传输(发送)?
	dma.DMA_Mode = DMA_Mode_Circular;		
	dma.DMA_Priority = DMA_Priority_VeryHigh;
	dma.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4,&dma);
	
	nvic.NVIC_IRQChannel = DMA1_Channel4_IRQn;           
	nvic.NVIC_IRQChannelPreemptionPriority = 1;          
	nvic.NVIC_IRQChannelSubPriority = 3;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	//测试用中断
	DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);
	
	//很多时候先不使能
	//此处直接使能
	DMA_Cmd(DMA1_Channel4,ENABLE);
	
	//DMA——RX
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
	
	DMA_DeInit(DMA1_Channel5);
	dma.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));
	dma.DMA_MemoryBaseAddr = (uint32_t)USART1_DMA_rece_buffer;
	//存储器到外设
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma.DMA_BufferSize = USART1_DMA_rece_buffersize;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	//循环传输(接收)
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_VeryHigh;
	dma.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5,&dma);

	nvic.NVIC_IRQChannel = DMA1_Channel5_IRQn;           
	nvic.NVIC_IRQChannelPreemptionPriority = 0;          
	nvic.NVIC_IRQChannelSubPriority = 0; 
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	//测试用中断
	DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);
	
	//很多时候先不使能
	//此处直接使能
	DMA_Cmd(DMA1_Channel5,ENABLE);
}


int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0);
	USART1->DR = (u8)ch;
	return ch;
}


void USART1_Dataset(void)
{
	//经Code Block测试：
	//short --> uint16_t --> short 数值不变
	//float --> uint16_t --> short 数值取整
	uint16_t send_pos1 = pos1;
	uint16_t send_pos2 = pos2;

	short send_cur1 = nowcurrent1;
	short send_cur2 = nowcurrent2;
	short send_scu1 = Global_SetCurrent1;
	short send_scu2 = Global_SetCurrent2;
	
	send_cur1 = (uint16_t)send_cur1;
	send_cur2 = (uint16_t)send_cur2;
	send_scu1 = (uint16_t)send_scu1;
	send_scu2 = (uint16_t)send_scu2;

	USART1_DMA_send_buffer[0]  = (send_pos1>>8);
	USART1_DMA_send_buffer[1]  = (send_pos1>>0);
	USART1_DMA_send_buffer[2]  = (send_pos2>>8);
	USART1_DMA_send_buffer[3]  = (send_pos2>>0);
	USART1_DMA_send_buffer[4]  = (send_cur1>>8);
	USART1_DMA_send_buffer[5]  = (send_cur1>>0);
	USART1_DMA_send_buffer[6]  = (send_cur2>>8);
	USART1_DMA_send_buffer[7]  = (send_cur2>>0);
	USART1_DMA_send_buffer[8]  = (send_scu1>>8);
	USART1_DMA_send_buffer[9]  = (send_scu1>>0);
	USART1_DMA_send_buffer[10] = (send_scu2>>8);
	USART1_DMA_send_buffer[11] = (send_scu2>>0);

	//检验位
	USART1_DMA_send_buffer[12] = 0x0A;
}


u8 RC_Data_Solve(void)
{
//	//遥控器数据解码
//	rc.x = ((uint16_t)USART1_DMA_rece_buffer[0]<<8)|((uint16_t)USART1_DMA_rece_buffer[1]<<0);
//	rc.y = ((uint16_t)USART1_DMA_rece_buffer[2]<<8)|((uint16_t)USART1_DMA_rece_buffer[3]<<0);
//	
//	rc.Key_F  = USART1_DMA_rece_buffer[4];
//	rc.Key_B  = USART1_DMA_rece_buffer[5];
//	rc.Key_L  = USART1_DMA_rece_buffer[6];
//	rc.Key_R  = USART1_DMA_rece_buffer[7];
//	rc.Key_SW = USART1_DMA_rece_buffer[8];
//	
//	rc.Check1  = USART1_DMA_rece_buffer[9];
//	rc.Check2  = USART1_DMA_rece_buffer[10];
//	
//	if((rc.y>(2008-20))&&(rc.y<(2008+20)))
//		rc.y = 2008;
//	
////	pid_Position2.SetPoint += ((float)rc.y - 2008.0f)*0.01;

//	if((rc.Check1 == 0x0A)&&(rc.Check2 == 0x0D))
//		return 0;
//	else
//		return 1;

	//战舰板数据解码
	rc.Key_F  = USART1_DMA_rece_buffer[0];
	rc.Key_B  = USART1_DMA_rece_buffer[1];
	
	rc.Check1  = USART1_DMA_rece_buffer[2];
	rc.Check2  = USART1_DMA_rece_buffer[3];
	
	if(rc.Key_F != rc.Key_F_last)
	{
		pid_Position2.SetPoint += (1750*4);
		rc.Key_F_last = rc.Key_F;
	}
	if(rc.Key_B != rc.Key_B_last)
	{
		pid_Position2.SetPoint -= (1750*4);
		rc.Key_B_last = rc.Key_B;
	}
	
	if((rc.Check1 == 0x0A)&&(rc.Check2 == 0x0D))
		return 0;
	else 
		return 1;
}


//串口空闲中断
void USART1_IRQHandler(void)
{
	static int DATA_LENGTH=0;

	if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)
	{
		(void)USART1->SR;
		(void)USART1->DR;

		DMA_Cmd(DMA1_Channel5,DISABLE);
		DATA_LENGTH = USART1_DMA_rece_buffersize-DMA_GetCurrDataCounter(DMA1_Channel5);
		if(DATA_LENGTH==4)
		{
//			RC_Data_Solve();
		}
		DMA_SetCurrDataCounter(DMA1_Channel5,USART1_DMA_rece_buffersize);	
		DMA_Cmd(DMA1_Channel5,ENABLE);
	}
}


//DMA发送完成中断
void DMA1_Channel4_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC4))
	{
		DMA_ClearFlag(DMA1_IT_TC4);
		DMA_ClearITPendingBit(DMA1_IT_TC4);
	}
}


//DMA接收完成中断
void DMA1_Channel5_IRQHandler(void)
{	
	if(DMA_GetITStatus(DMA1_IT_TC5))
	{		
		if((USART1_DMA_rece_buffer[2] == 0x0A)&&(USART1_DMA_rece_buffer[3] == 0x0D))
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
			
			DMA_Cmd(DMA1_Channel5,DISABLE);
			DMA_SetCurrDataCounter(DMA1_Channel5,USART1_DMA_rece_buffersize);	
			DMA_Cmd(DMA1_Channel5,ENABLE);
		}
		
		DMA_ClearFlag(DMA1_IT_TC5);
		DMA_ClearITPendingBit(DMA1_IT_TC5);     
	}
}


