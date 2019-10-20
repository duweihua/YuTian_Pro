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

	RCC_APB1PeriphClockCmd ( RCC_APB1Periph_USART2, ENABLE ); //����USART2ʱ��
	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOD, ENABLE );

	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init ( &NVIC_InitStructure );

	GPIO_PinAFConfig ( GPIOD, GPIO_PinSource5, GPIO_AF_USART2 );
	GPIO_PinAFConfig ( GPIOD, GPIO_PinSource6, GPIO_AF_USART2 );

	//����PD5��ΪUSART2��Tx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init ( GPIOD, &GPIO_InitStructure );
	//����PD6��ΪUSART2��Rx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init ( GPIOD, &GPIO_InitStructure );

	//����USART2
	//�жϱ�������
	USART_InitStructure.USART_BaudRate = 115200;       //�����ʿ���ͨ������վ����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8λ����
	USART_InitStructure.USART_StopBits = USART_StopBits_1;   //��֡��β����1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;    //������żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //���͡�����ʹ��
	//����USART2ʱ��
	USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;  //ʱ�ӵ͵�ƽ�
	USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;  //SLCK������ʱ������ļ���->�͵�ƽ
	USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;  //ʱ�ӵڶ������ؽ������ݲ���
	USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable; //���һλ���ݵ�ʱ�����岻��SCLK���

	USART_Init ( USART2, &USART_InitStructure );
	USART_ClockInit ( USART2, &USART_ClockInitStruct );

	//ʹ��USART2�����ж�
	USART_ITConfig ( USART2, USART_IT_RXNE, ENABLE );
	
	/* IDLE interrupt can not go in */
//	USART_ITConfig ( USART2, USART_IT_IDLE, ENABLE );
	
	//ʹ��USART2
	USART_Cmd ( USART2, ENABLE );
}

u8 count = 0;
u8 Rx_Buf[256];	//���ڽ��ջ���
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
