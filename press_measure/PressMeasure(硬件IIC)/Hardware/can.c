#include "main.h"

CanRxMsg rx_message;

void CAN_Configuration(void)
{
	CAN_InitTypeDef        can;
	CAN_FilterInitTypeDef  can_filter;
	GPIO_InitTypeDef 	   gpio;
	NVIC_InitTypeDef   	   nvic;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	//CAN_RX
	gpio.GPIO_Pin = GPIO_Pin_11;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio);
	
	//CAN_TX
	gpio.GPIO_Pin = GPIO_Pin_12;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio);

	CAN_DeInit(CAN1);

	can.CAN_TTCM = DISABLE;
	can.CAN_ABOM = ENABLE; 
	can.CAN_AWUM = DISABLE;
	can.CAN_NART = DISABLE;
	can.CAN_RFLM = DISABLE;				
	can.CAN_TXFP = ENABLE; 
	can.CAN_Mode = CAN_Mode_Normal;
//	can.CAN_Mode = CAN_Mode_LoopBack;
	can.CAN_SJW = CAN_SJW_1tq;
	can.CAN_BS1 = CAN_BS1_5tq;
	can.CAN_BS2 = CAN_BS2_3tq;
	can.CAN_Prescaler = 4;     //CAN BaudRate 36/(1+5+3)/4=1Mbps
	
	CAN_Init(CAN1,&can);

	can_filter.CAN_FilterNumber = 0; 
	can_filter.CAN_FilterMode = CAN_FilterMode_IdMask;
	can_filter.CAN_FilterScale = CAN_FilterScale_32bit;
	can_filter.CAN_FilterIdHigh = 0x0000;
	can_filter.CAN_FilterIdLow = 0x0000;
	can_filter.CAN_FilterMaskIdHigh = 0x0000;
	can_filter.CAN_FilterMaskIdLow = 0x0000;
	can_filter.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	can_filter.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&can_filter);
	
	nvic.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
	//������Ӧд�ж� ��������ռ
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
    nvic.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;  
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
    CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
    CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);
}


void CANSendData(short data1,short data2)
{
    CanTxMsg tx_message;
    tx_message.StdId = 0x200;
    tx_message.IDE = CAN_ID_STD;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;

    tx_message.Data[0] = (data1>>8)&0xff;
    tx_message.Data[1] = data1&0xff;
    tx_message.Data[2] = (data2>>8)&0xff;
    tx_message.Data[3] = data2&0xff;
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
    
	CAN_Transmit(CAN1,&tx_message);
}


//�����ж�
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
		CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
		CAN_Receive(CAN1,CAN_FIFO0,&rx_message);
    }
}


//�����ж�
void USB_HP_CAN1_TX_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN1,CAN_IT_TME)!= RESET)
	{
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
    }
}




//#include "main.h"

//void CAN1_Init(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure; 
//    NVIC_InitTypeDef NVIC_InitStructure;
//    CAN_InitTypeDef        CAN_InitStructure;
//    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

//    /* ���ù��ܺ�GPIOB�˿�ʱ��ʹ��*/    
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);                                                                      

//    /* CAN1 ģ��ʱ��ʹ�� */
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); 

//    /* Configure CAN pin: RX */  // Pa12
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //�����������
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       
//	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    /* Configure CAN pin: TX */   // Pa11
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//��������
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    //#define GPIO_Remap_CAN
//    GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);

//    //CAN_NVIC_Configuration(); //CAN�жϳ�ʼ��   
//    /* Configure the NVIC Preemption Priority Bits */  
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

//    /* enabling interrupt */

//    CAN_InitStructure.CAN_TTCM=DISABLE;         //û��ʹ��ʱ�䴥��ģʽ
//    CAN_InitStructure.CAN_ABOM=ENABLE;         //ʹ���Զ����߹���
//    CAN_InitStructure.CAN_AWUM=ENABLE;         //ʹ���Զ�����ģʽ
//    CAN_InitStructure.CAN_NART=ENABLE;         //û��ʹ�ܷ��Զ��ش�ģʽ
//    CAN_InitStructure.CAN_RFLM=ENABLE;         //û��ʹ�ܽ���FIFO����ģʽ
//    //CAN_InitStructure.CAN_TXFP=DISABLE;         //û��ʹ�ܷ���FIFO���ȼ�
////    CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack; //CAN����Ϊ�ػ�ģʽ
//		CAN_InitStructure.CAN_Mode=CAN_Mode_Normal; //CAN����Ϊ��ͨģʽ
//    CAN_InitStructure.CAN_BS1=CAN_BS1_5tq;
//		CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;
//		CAN_InitStructure.CAN_Prescaler=4;
//		CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
//		CAN_Init(CAN1,&CAN_InitStructure);
//    //CAN filter init ���������Ѿ�����Ϊ���⣬����ͨ��ExtId��ʶ�����ִӻ�����
//    CAN_FilterInitStructure.CAN_FilterNumber=0;                     //ָ��������Ϊ0
//    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;   //ָ��������Ϊ��ʶ������λģʽ
//    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;  //������λ��Ϊ32λ
//    CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;                //��������ʶ���ĸ�16λֵ
//    CAN_FilterInitStructure.CAN_FilterIdLow=CAN_ID_EXT|CAN_RTR_DATA;//��������ʶ���ĵ�16λֵ
//    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;            //���������α�ʶ���ĸ�16λֵ
//    CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;             //���������α�ʶ���ĵ�16λֵ
//    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;     //�趨��ָ���������FIFOΪ0
//    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;            //ʹ�ܹ�����
//    CAN_FilterInit(&CAN_FilterInitStructure);                       //������Ĳ�����ʼ��������

//	//�����ж�����
//	CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE); 
//	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	
//	//�ж�����
//	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE); 
//	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//    /* CAN FIFO0 message pending interrupt enable */ 
//}


//void USB_LP_CAN1_RX0_IRQHandler(void)
//{
//	CanRxMsg rx_message;
//	CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
//}


//void USB_HP_CAN1_TX_IRQHandler(void)
//{
//	 if (CAN_GetITStatus(CAN1,CAN_IT_TME)!= RESET)
//	{
//		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
//	}
//}




