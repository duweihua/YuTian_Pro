
/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/
/*----------------------------------------------------------------------------------------------------------------------/
*               ������ֻ��������ѧϰʹ�ã���Ȩ����Ȩ���������ƴ��Ŷӣ������ƴ��Ŷӽ��ɿس���Դ���ṩ�������ߣ�
*               ������ҪΪ�����ƴ��Ŷ��ṩ������δ��������ɣ����ý�Դ�����ṩ�����ˣ����ý�Դ����ŵ����Ϲ�����������أ� 
*               �������Դ�����Ĳ�����緢��������Ϊ�������ƴ��Ŷӽ���֮�Է��ɽ��������
-----------------------------------------------------------------------------------------------------------------------/
*               ������Ϣ���ܶ���ֹ��ǰ�����������˳���������
*               ��Դ���ף���ѧ����ϧ��ף������Ϯ�����׳ɹ�������
*               ѧϰ�����ߣ��������Ƽ���DJI��ZEROTECH��XAG��AEE��GDU��AUTEL��EWATT��HIGH GREAT�ȹ�˾��ҵ
*               ��ְ�����뷢�ͣ�15671678205@163.com���豸ע��ְ����λ����λ��������
*               �����ƴ���Դ�ɿ�QQȺ��540707961
*               CSDN���ͣ�http://blog.csdn.net/u011992534
*               �ſ�ID��NamelessCotrun����С��
*               Bվ��ѧ��Ƶ��https://space.bilibili.com/67803559/#/video
*               �ͻ�ʹ���ĵá��Ľ������������http://www.openedv.com/forum.php?mod=viewthread&tid=234214&extra=page=1
*               �Ա����̣�https://shop348646912.taobao.com/?spm=2013.1.1000126.2.5ce78a88ht1sO2
*               �ٶ�����:�����ƴ���Դ�ɿ�
*               ��˾����:www.nameless.tech
*               �޸�����:2019/4/12
*               �汾����Ӯ�ߡ���CarryPilot_V1.0
*               ��Ȩ���У�����ؾ���
*               Copyright(C) 2017-2025 �人�������¿Ƽ����޹�˾ 
*               All rights reserved
*               ��Ҫ��ʾ��
*               �����Ա�����ת�ֵķɿء��������ѡ�����ѧ�ܵĶ����Խ��ۺ�Ⱥѧϰ������
*               ����ֱ�����������������������ϣ��������´�����������Ȩ�����˲��ý�
*               ���ϴ��봫���Ϲ��������أ�������ı��ΪĿ���������ϴ��룬�����д˲�
*               ���ߣ���˾����ǰ��֪����1���ڼ�ʱ�����������ѧУ����λ����������
*               ������ַ��Ϣ�ᱻ�����ڹ�˾�������ٷ�΢�Ź���ƽ̨���ٷ��������͡�֪��
*               ר���Լ��Ա�������ҳ���Թ�ʾ���棬����������Ϊ�����Ϊ�����۵㣬Ӱ��
*               ��ѧ���ҹ���������������ܿ�ͺ������˻����������������ء�
*               �����Ϊ����˾����ش���ʧ�ߣ����Է���;���������л���ĺ�����лл������
----------------------------------------------------------------------------------------------------------------------*/
#include "Headfile.h"
#include "uart.h"
#include "Usart.h"
#include "Ringbuf.h"


//����ѭ�����л������ݶ���
RingBuff_t COM0_Rx_Buf,COM1_Rx_Buf,COM2_Rx_Buf,COM3_Rx_Buf,COM4_Rx_Buf,COM5_Rx_Buf,COM6_Rx_Buf,COM7_Rx_Buf;
/***********************************************************
@��������UART0_IRQHandler
@��ڲ�������
@���ڲ�������
��������������0���ݽ���
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void UART0_IRQHandler(void)//UART0�жϺ���
{	
  //��ȡ�жϱ�־ ԭʼ�ж�״̬ �������жϱ�־		
  uint32_t flag = UARTIntStatus(UART0_BASE,1);
  //����жϱ�־	
  UARTIntClear(UART0_BASE,flag);		
  //�ж�FIFO�Ƿ�������		
  while(UARTCharsAvail(UART0_BASE))		
  {			
    RingBuf_Write(UARTCharGet(UART0_BASE),&COM0_Rx_Buf,32);//�����ζ�������д����
    //RDroneStudio_Receive(UARTCharGet(UART0_BASE));		
  }
}

/***********************************************************
@��������ConfigureUART0
@��ڲ�������
@���ڲ�������
��������������0����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ConfigureUART0(void)//����0��ʼ��
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);//ʹ��GPIO����		
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);//ʹ��UART����
  GPIOPinConfigure(GPIO_PA0_U0RX);//GPIOģʽ���� PA0--RX PA1--TX 
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);//GPIO��UARTģʽ����
  UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
  UARTStdioConfig(0, 115200, 16000000);
  //UARTЭ������ ������115200 8λ 1ֹͣλ  ��У��λ	
  //UART����FIFO Ĭ��FIFO LevelΪ4/8 �Ĵ�����8�ֽں�����ж�	//���ú����1λ�Ͳ����ж�	
  UARTFIFODisable(UART0_BASE);//ʹ��UART0�ж�	IntEnable(INT_UART0);	
  UARTIntEnable(UART0_BASE,UART_INT_RX);//ʹ��UART0�����ж�		
  UARTIntRegister(UART0_BASE,UART0_IRQHandler);//UART�жϵ�ַע��	
  IntPrioritySet(INT_UART0, USER_INT2);
}

/***********************************************************
@��������USART0_Send
@��ڲ�����uint8_t *pui8Buffer, uint32_t ui32Count
@���ڲ�������
��������������0����N���ֽ�����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void USART0_Send(uint8_t *pui8Buffer, uint32_t ui32Count)//����N���ֽڳ��ȵ�����
{
  while(ui32Count--)
  {
    UARTCharPut(UART0_BASE, *pui8Buffer++);
  }
}
/***********************************************************
@��������wust_sendware
@��ڲ�����unsigned char *wareaddr, int16_t waresize
@���ڲ�������
����������ɽ������ʾ��������
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void wust_sendware(unsigned char *wareaddr, int16_t waresize)//ɽ�ⷢ�Ͳ���
{
#define CMD_WARE     3
  uint8 cmdf[2] = {CMD_WARE, ~CMD_WARE};//֡ͷ
  uint8 cmdr[2] = {~CMD_WARE, CMD_WARE};//֡β
  USART0_Send(cmdf, sizeof(cmdf));
  USART0_Send(wareaddr, waresize);
  USART0_Send(cmdr, sizeof(cmdr));
}

/***********************************************************
@��������UART1_IRQHandler
@��ڲ�������
@���ڲ�������
��������������1���ݽ���
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void UART1_IRQHandler(void)//UART1�жϺ���
{				
  uint32_t flag = UARTIntStatus(UART1_BASE,1);//��ȡ�жϱ�־ ԭʼ�ж�״̬ �����жϱ�־	
  UARTIntClear(UART1_BASE,flag);//����жϱ�־	
  while(UARTCharsAvail(UART1_BASE))//�ж�FIFO�Ƿ�������		
  {			
    //RingBuf_Write(UARTCharGet(UART1_BASE),&COM1_Rx_Buf,50);//�����ζ�������д����	
    ANO_DT_Data_Receive_Prepare(UARTCharGet(UART1_BASE)); 		
  }
}


/***********************************************************
@��������USART1_Send
@��ڲ�����uint8_t *pui8Buffer, uint32_t ui32Count
@���ڲ�������
��������������1����N���ֽ�����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void USART1_Send(uint8_t *pui8Buffer, uint32_t ui32Count)//����N���ֽڳ��ȵ�����
{
  while(ui32Count--)
  {
    UARTCharPut(UART1_BASE, *pui8Buffer++);
  }
}
/***********************************************************
@��������ConfigureUART1
@��ڲ�������
@���ڲ�������
��������������1����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ConfigureUART1(void)//����1��ʼ��
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);//ʹ��GPIO����		
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);//ʹ��UART����
  GPIOPinConfigure(GPIO_PB0_U1RX);//GPIOģʽ���� PB0--RX PB1--TX 
  GPIOPinConfigure(GPIO_PB1_U1TX);
  GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);//GPIO��UARTģʽ����
  UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);
  UARTStdioConfig(1, 115200, 16000000);
  UARTFIFODisable(UART1_BASE);//ʹ��UART1�ж�	
  UARTIntEnable(UART1_BASE,UART_INT_RX);//ʹ��UART1�����ж�		
  UARTIntRegister(UART1_BASE,UART1_IRQHandler);//UART1�жϵ�ַע��	
  IntPrioritySet(INT_UART1, USER_INT4);
}


#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{
  USART1_Send((u8 *)&ch,1);
}

#endif

//-------------------------------------------------------------------//
//2019/7/17����
//�û�Fyra-BH�������
//���ڴ��ڵ���

u8 lora_rx_buf[20];



/*-------------------------------------------*/
//�������ݽ��պ���
//@para   uint8_t dat,���յ�������
//@return ��
void Usart_DebuggerGetOneByte(uint8_t dat)
{
   static u8 sta=0;//���ڼ�¼����״̬
	 static u8 data_index=0;// 
  	if(dat==0xaa)
		{
		  if(data_index==0)
				{
				  
				}
			else
				data_index=0;
		}
}

/***********************************************************
@��������UART2_IRQHandler
@��ڲ�������
@���ڲ�������
��������������2���ݽ���
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void UART2_IRQHandler(void)
{
  uint32_t flag = UARTIntStatus(UART2_BASE,1);//��ȡ�жϱ�־ ԭʼ�ж�״̬ �����жϱ�־		
  UARTIntClear(UART2_BASE,flag);//����жϱ�־	
  while(UARTCharsAvail(UART2_BASE))//�ж�FIFO�Ƿ�������				
  {		
    //����õ�������
    RingBuf_Write(UARTCharGet(UART2_BASE),&COM2_Rx_Buf,200);//�����ζ�������д����
    if(COM2_Rx_Buf.Ring_Buff[0]!=0XB5)
    {
      COM2_Rx_Buf.Head=1;
      COM2_Rx_Buf.Tail=0; 
    }		
  }
}

/***********************************************************
@��������USART2_Send
@��ڲ�����uint8_t *pui8Buffer, uint32_t ui32Count
@���ڲ�������
��������������2����N���ֽ�����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void USART2_Send(uint8_t *pui8Buffer, uint32_t ui32Count)//����N���ֽڳ��ȵ�����
{
  while(ui32Count--)
  {
    UARTCharPut(UART2_BASE, *pui8Buffer++);
  }
}
/***********************************************************
@��������ConfigureUART2
@��ڲ�������
@���ڲ�������
��������������2����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ConfigureUART2(unsigned long bound)//����2��ʼ��
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);//ʹ��GPIO����		
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);//ʹ��UART����
  
  HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;//����PD6
  HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x80;//ȷ��
  HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;//��������
  
  GPIOPinConfigure(GPIO_PD6_U2RX);//GPIOģʽ���� PD6--RX PD7--TX 
  GPIOPinConfigure(GPIO_PD7_U2TX);
  GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);//GPIO��UARTģʽ����
  UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), bound,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));
  UARTFIFODisable(UART2_BASE);//ʹ��UART2�ж�	
  UARTIntEnable(UART2_BASE,UART_INT_RX |UART_INT_RT);//ʹ��UART0�����ж�		
  UARTIntRegister(UART2_BASE,UART2_IRQHandler);//UART�жϵ�ַע��	
  IntPrioritySet(INT_UART2, USER_INT1);
}

/***********************************************************
@��������UART3_IRQHandler
@��ڲ�������
@���ڲ�������
��������������3���ݽ���
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void UART3_IRQHandler(void)
{		
  uint32_t flag = UARTIntStatus(UART3_BASE,1);//��ȡ�жϱ�־ ԭʼ�ж�״̬ �����жϱ�־		
  UARTIntClear(UART3_BASE,flag);//����жϱ�־			
  while(UARTCharsAvail(UART3_BASE))//�ж�FIFO�Ƿ�������		
  {			
    RingBuf_Write(UARTCharGet(UART3_BASE),&COM3_Rx_Buf,24);//�����ζ�������д����
    //SDK_Data_Receive_Prepare(UARTCharGet(UART3_BASE));		
  }
}

/***********************************************************
@��������USART3_Send
@��ڲ�����uint8_t *pui8Buffer, uint32_t ui32Count
@���ڲ�������
��������������3����N���ֽ�����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void USART3_Send(uint8_t *pui8Buffer, uint32_t ui32Count)//����N���ֽڳ��ȵ�����
{
  while(ui32Count--)
  {
    UARTCharPut(UART3_BASE, *pui8Buffer++);
  }
}

/***********************************************************
@��������ConfigureUART3
@��ڲ�������
@���ڲ�������
��������������3����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ConfigureUART3(void)//����3��ʼ��
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);//ʹ��GPIO����		
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);//ʹ��UART����
  GPIOPinConfigure(GPIO_PC6_U3RX);//GPIOģʽ���� PC6--RX PC7--TX 
  GPIOPinConfigure(GPIO_PC7_U3TX);
  GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);//GPIO��UARTģʽ����
  UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 115200,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));
  UARTFIFODisable(UART3_BASE);//ʹ��UART0�ж�	
  UARTIntEnable(UART3_BASE,UART_INT_RX);//ʹ��UART3�����ж�		
  UARTIntRegister(UART3_BASE,UART3_IRQHandler);//UART3�жϵ�ַע��	
  IntPrioritySet(INT_UART3, USER_INT3);
}

/***********************************************************
@��������UART6_IRQHandler
@��ڲ�������
@���ڲ�������
��������������6���ݽ���
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void UART6_IRQHandler(void)
{		
  uint32_t flag = UARTIntStatus(UART6_BASE,1);//��ȡ�жϱ�־ ԭʼ�ж�״̬ �����жϱ�־		
  UARTIntClear(UART6_BASE,flag);//����жϱ�־	
  while(UARTCharsAvail(UART6_BASE))//�ж�FIFO�Ƿ�������		
  {			
    RingBuf_Write(UARTCharGet(UART6_BASE),&COM6_Rx_Buf,FLOW_BUF_CNT);//�����ζ�������д����		
  }
}

/***********************************************************
@��������USART6_Send
@��ڲ�����uint8_t *pui8Buffer, uint32_t ui32Count
@���ڲ�������
��������������6����N���ֽ�����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void USART6_Send(uint8_t *pui8Buffer, uint32_t ui32Count)//����N���ֽڳ��ȵ�����
{
  while(ui32Count--)
  {
    UARTCharPut(UART6_BASE, *pui8Buffer++);
  }
}

/***********************************************************
@��������ConfigureUART6
@��ڲ�������
@���ڲ�������
��������������6����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ConfigureUART6(void)//����6��ʼ��
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);//ʹ��GPIO����		
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);//ʹ��UART����
  GPIOPinConfigure(GPIO_PD4_U6RX);//GPIOģʽ���� PD4--RX PD5--TX 
  GPIOPinConfigure(GPIO_PD5_U6TX);
  GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);//GPIO��UARTģʽ����
  UARTConfigSetExpClk(UART6_BASE, SysCtlClockGet(), 19200,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));
  UARTFIFODisable(UART6_BASE);//ʹ��UART0�ж�	
  UARTIntEnable(UART6_BASE,UART_INT_RX);//ʹ��UART6�����ж�		
  OpticalFlow_Init();//�����˲�������ʼ��
  OpticalFlow_Is_Work=Config_Init_Uart();//������������ʼ��
  UARTIntRegister(UART6_BASE,UART6_IRQHandler);//UART6�жϵ�ַע��	
  IntPrioritySet(INT_UART6, USER_INT5);
}

/***********************************************************
@��������UART7_IRQHandler
@��ڲ�������
@���ڲ�������
��������������7���ݽ���
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void UART7_IRQHandler(void)//UART2�жϺ���
{		
  uint32_t flag = UARTIntStatus(UART7_BASE,1);//��ȡ�жϱ�־ ԭʼ�ж�״̬ �����жϱ�־	
  UARTIntClear(UART7_BASE,flag);//����жϱ�־		
  while(UARTCharsAvail(UART7_BASE))//�ж�FIFO�Ƿ�������			
  {			
    //����õ�������			
    //UARTCharPut(UART1_BASE,UARTCharGet(UART1_BASE));
    RingBuf_Write(UARTCharGet(UART7_BASE),&COM7_Rx_Buf,4);//�����ζ�������д����		
  }
}

/***********************************************************
@��������USART7_Send
@��ڲ�����uint8_t *pui8Buffer, uint32_t ui32Count
@���ڲ�������
��������������7����N���ֽ�����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void USART7_Send(uint8_t *pui8Buffer, uint32_t ui32Count)//����N���ֽڳ��ȵ�����
{
  while(ui32Count--)
  {
    UARTCharPut(UART7_BASE, *pui8Buffer++);
  }
}

/***********************************************************
@��������ConfigureUART7
@��ڲ�������
@���ڲ�������
��������������7����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ConfigureUART7(void)//����7��ʼ��
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);//ʹ��GPIO����		
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);//ʹ��UART����
  GPIOPinConfigure(GPIO_PE0_U7RX);//GPIOģʽ���� PE0--RX PE1--TX 
  GPIOPinConfigure(GPIO_PE1_U7TX);
  GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);//GPIO��UARTģʽ����
  UARTConfigSetExpClk(UART7_BASE, SysCtlClockGet(), 9600,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));
  UARTFIFODisable(UART7_BASE);//ʹ��UART0�ж�	
  UARTIntEnable(UART7_BASE,UART_INT_RX);//ʹ��UART0�����ж�		
  UARTIntRegister(UART7_BASE,UART7_IRQHandler);//UART�жϵ�ַע��	
  IntPrioritySet(INT_UART7, USER_INT6);
}

/***********************************************************/
uint8_t data_to_send[50];//ANO����վ�������ݻ���
uint8_t ANO_Send_PID_Flag[6]={0};//PID���ͱ�־λ
#define vs16 int16_t
#define vs32 int32_t
#define s16  int16_t
/***********************************************************
@��������ANO_DT_Data_Receive_Prepare
@��ڲ�����u8 data
@���ڲ�������
������������������վ�������ݽ���
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ANO_DT_Data_Receive_Prepare(u8 data)//ANO����վ���ݽ���
{
  static u8 RxBuffer[50];
  static u8 _data_len = 0,_data_cnt = 0;
  static u8 state = 0;
  if(state==0&&data==0xAA)//֡ͷ1
  {
    state=1;
    RxBuffer[0]=data;
  }
  else if(state==1&&data==0xAF)//֡ͷ2
  {
    state=2;
    RxBuffer[1]=data;
  }
  else if(state==2&&data<0XF1)//�����ֽ�
  {
    state=3;
    RxBuffer[2]=data;
  }
  else if(state==3&&data<50)//��Ч���ݳ���
  {
    state = 4;
    RxBuffer[3]=data;
    _data_len = data;
    _data_cnt = 0;
  }
  else if(state==4&&_data_len>0)//���ݽ���
  {
    _data_len--;
    RxBuffer[4+_data_cnt++]=data;
    if(_data_len==0)
      state = 5;
  }
  else if(state==5)//У���
  {
    state = 0;
    RxBuffer[4+_data_cnt]=data;
    ANO_DT_Data_Receive_Anl(RxBuffer,_data_cnt+5);//���ݽ���
  }
  else state = 0;
}

/***********************************************************
@��������ANO_DT_Send_Check
@��ڲ�������
@���ڲ�������
������������������վ��У�麯��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
static void ANO_DT_Send_Check(u8 head, u8 check_sum)//ANO����վ��У��
{
  u8 sum = 0,i=0;
  data_to_send[0]=0xAA;
  data_to_send[1]=0xAA;
  data_to_send[2]=0xEF;
  data_to_send[3]=2;
  data_to_send[4]=head;
  data_to_send[5]=check_sum;
  for(i=0;i<6;i++)
    sum += data_to_send[i];
  data_to_send[6]=sum;
  USART1_Send(data_to_send, 7);
}

/***********************************************************
@��������ANO_DT_Data_Receive_Anl
@��ڲ�������
@���ڲ�������
������������������վ֡���ݽ�������
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ANO_DT_Data_Receive_Anl(u8 *data_buf,u8 num)//ANO���ݽ���
{
  u8 sum = 0,i=0;
  for(i=0;i<(num-1);i++)  sum += *(data_buf+i);
  if(!(sum==*(data_buf+num-1)))    return; //�ж�sum
  if(!(*(data_buf)==0xAA && *(data_buf+1)==0xAF))     return;//�ж�֡ͷ
  if(*(data_buf+2)==0X01)
  {
    if(*(data_buf+4)==0X01) {;}
    if(*(data_buf+4)==0X02) {;}
    if(*(data_buf+4)==0X03) {;}
  }
  
  if(*(data_buf+2)==0X02)
  {
    if(*(data_buf+4)==0X01)		//��ȡ��ǰPID����
    {
      ANO_Send_PID_Flag[0]=1;
      ANO_Send_PID_Flag[1]=1;
      ANO_Send_PID_Flag[2]=1;
      ANO_Send_PID_Flag[3]=1;
      ANO_Send_PID_Flag[4]=1;
      ANO_Send_PID_Flag[5]=1;
      Bling_Set(&Light_1,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
      Bling_Set(&Light_2,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
      Bling_Set(&Light_3,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
    }
    if(*(data_buf+4)==0X02)
    {
      ;
    }
    if(*(data_buf+4)==0XA0)     //��ȡ�汾��Ϣ
    {
      ;
    }
    if(*(data_buf+4)==0XA1)     //�ָ�Ĭ�ϲ���
    {
      Sort_PID_Flag=3;
      Bling_Set(&Light_1,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
      Bling_Set(&Light_2,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
      Bling_Set(&Light_3,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
    }
  }
  
  if(*(data_buf+2)==0X10)                             //����PID1
  {
    Total_Controller.Roll_Gyro_Control.Kp  = 0.001*( (vs16)(*(data_buf+4)<<8)|*(data_buf+5) );
    Total_Controller.Roll_Gyro_Control.Ki  = 0.001*( (vs16)(*(data_buf+6)<<8)|*(data_buf+7) );
    Total_Controller.Roll_Gyro_Control.Kd  = 0.01*( (vs16)(*(data_buf+8)<<8)|*(data_buf+9) );
    Total_Controller.Pitch_Gyro_Control.Kp   = 0.001*( (vs16)(*(data_buf+10)<<8)|*(data_buf+11) );
    Total_Controller.Pitch_Gyro_Control.Ki   = 0.001*( (vs16)(*(data_buf+12)<<8)|*(data_buf+13) );
    Total_Controller.Pitch_Gyro_Control.Kd   = 0.01*( (vs16)(*(data_buf+14)<<8)|*(data_buf+15) );
    Total_Controller.Yaw_Gyro_Control.Kp    = 0.001*( (vs16)(*(data_buf+16)<<8)|*(data_buf+17) );
    Total_Controller.Yaw_Gyro_Control.Ki    = 0.001*( (vs16)(*(data_buf+18)<<8)|*(data_buf+19) );
    Total_Controller.Yaw_Gyro_Control.Kd    = 0.01*( (vs16)(*(data_buf+20)<<8)|*(data_buf+21) );
    ANO_DT_Send_Check(*(data_buf+2),sum);
  }
  if(*(data_buf+2)==0X11)                             //����PID2
  {
    Total_Controller.Roll_Angle_Control.Kp  = 0.001*( (vs16)(*(data_buf+4)<<8)|*(data_buf+5) );
    Total_Controller.Roll_Angle_Control.Ki  = 0.001*( (vs16)(*(data_buf+6)<<8)|*(data_buf+7) );
    Total_Controller.Roll_Angle_Control.Kd  = 0.01*( (vs16)(*(data_buf+8)<<8)|*(data_buf+9) );
    Total_Controller.Pitch_Angle_Control.Kp   = 0.001*( (vs16)(*(data_buf+10)<<8)|*(data_buf+11) );
    Total_Controller.Pitch_Angle_Control.Ki   = 0.001*( (vs16)(*(data_buf+12)<<8)|*(data_buf+13) );
    Total_Controller.Pitch_Angle_Control.Kd   = 0.01*( (vs16)(*(data_buf+14)<<8)|*(data_buf+15) );
    Total_Controller.Yaw_Angle_Control.Kp    = 0.001*( (vs16)(*(data_buf+16)<<8)|*(data_buf+17) );
    Total_Controller.Yaw_Angle_Control.Ki    = 0.001*( (vs16)(*(data_buf+18)<<8)|*(data_buf+19) );
    Total_Controller.Yaw_Angle_Control.Kd    = 0.01*( (vs16)(*(data_buf+20)<<8)|*(data_buf+21) );
    ANO_DT_Send_Check(*(data_buf+2),sum); 
  }
  if(*(data_buf+2)==0X12)                             //����PID3
  {
    Total_Controller.High_Speed_Control.Kp    = 0.001*( (vs16)(*(data_buf+4)<<8)|*(data_buf+5) );
    Total_Controller.High_Speed_Control.Ki    = 0.001*( (vs16)(*(data_buf+6)<<8)|*(data_buf+7) );
    Total_Controller.High_Speed_Control.Kd    = 0.01*( (vs16)(*(data_buf+8)<<8)|*(data_buf+9) );
    Total_Controller.High_Position_Control.Kp = 0.001*( (vs16)(*(data_buf+10)<<8)|*(data_buf+11) );
    Total_Controller.High_Position_Control.Ki = 0.001*( (vs16)(*(data_buf+12)<<8)|*(data_buf+13) );
    Total_Controller.High_Position_Control.Kd = 0.01*( (vs16)(*(data_buf+14)<<8)|*(data_buf+15) );
    Total_Controller.Latitude_Speed_Control.Kp= 0.001*( (vs16)(*(data_buf+16)<<8)|*(data_buf+17) );
    Total_Controller.Latitude_Speed_Control.Ki= 0.001*( (vs16)(*(data_buf+18)<<8)|*(data_buf+19) );
    Total_Controller.Latitude_Speed_Control.Kd= 0.01*( (vs16)(*(data_buf+20)<<8)|*(data_buf+21) );
    /***********************λ�ÿ��ƣ�λ�á��ٶȲ�������һ��PID����**********************************************************/
    Total_Controller.Longitude_Speed_Control.Kp=Total_Controller.Latitude_Speed_Control.Kp;
    Total_Controller.Longitude_Speed_Control.Ki=Total_Controller.Latitude_Speed_Control.Ki;
    Total_Controller.Longitude_Speed_Control.Kd=Total_Controller.Latitude_Speed_Control.Kd;
    ANO_DT_Send_Check(*(data_buf+2),sum); 
  }
  if(*(data_buf+2)==0X13)                             //����PID4
  {
    Total_Controller.Latitude_Position_Control.Kp    = 0.001*( (vs16)(*(data_buf+4)<<8)|*(data_buf+5) );
    Total_Controller.Latitude_Position_Control.Ki    = 0.001*( (vs16)(*(data_buf+6)<<8)|*(data_buf+7) );
    Total_Controller.Latitude_Position_Control.Kd    = 0.01*( (vs16)(*(data_buf+8)<<8)|*(data_buf+9) );
    Total_Controller.High_Acce_Control.Kp            = 0.001*( (vs16)(*(data_buf+10)<<8)|*(data_buf+11) );
    Total_Controller.High_Acce_Control.Ki            = 0.001*( (vs16)(*(data_buf+12)<<8)|*(data_buf+13) );
    Total_Controller.High_Acce_Control.Kd            = 0.01*( (vs16)(*(data_buf+14)<<8)|*(data_buf+15) );
    /***********************λ�ÿ��ƣ�λ�á��ٶȲ�������һ��PID����**********************************************************/
    Total_Controller.Longitude_Position_Control.Kp=Total_Controller.Latitude_Position_Control.Kp;
    Total_Controller.Longitude_Position_Control.Ki=Total_Controller.Latitude_Position_Control.Ki;
    Total_Controller.Longitude_Position_Control.Kd=Total_Controller.Latitude_Position_Control.Kd;
    ANO_DT_Send_Check(*(data_buf+2),sum);
  }
  if(*(data_buf+2)==0X14)                             //����PID5
  {
    Total_Controller.Optical_Position_Control.Kp = 0.001*( (vs16)(*(data_buf+4)<<8)|*(data_buf+5) );
    Total_Controller.Optical_Position_Control.Ki = 0.001*( (vs16)(*(data_buf+6)<<8)|*(data_buf+7) );
    Total_Controller.Optical_Position_Control.Kd = 0.01*( (vs16)(*(data_buf+8)<<8)|*(data_buf+9) );
    Total_Controller.Optical_Speed_Control.Kp = 0.001*( (vs16)(*(data_buf+10)<<8)|*(data_buf+11) );
    Total_Controller.Optical_Speed_Control.Ki = 0.001*( (vs16)(*(data_buf+12)<<8)|*(data_buf+13) );
    Total_Controller.Optical_Speed_Control.Kd = 0.01*( (vs16)(*(data_buf+14)<<8)|*(data_buf+15) );
    
    Total_Controller.SDK_Roll_Position_Control.Kp = 0.001*( (vs16)(*(data_buf+16)<<8)|*(data_buf+17) );
    Total_Controller.SDK_Roll_Position_Control.Ki = 0.001*( (vs16)(*(data_buf+18)<<8)|*(data_buf+19) );
    Total_Controller.SDK_Roll_Position_Control.Kd = 0.01*( (vs16)(*(data_buf+20)<<8)|*(data_buf+21) );
    Total_Controller.SDK_Pitch_Position_Control.Kp = 0.001*( (vs16)(*(data_buf+16)<<8)|*(data_buf+17) );
    Total_Controller.SDK_Pitch_Position_Control.Ki = 0.001*( (vs16)(*(data_buf+18)<<8)|*(data_buf+19) );
    Total_Controller.SDK_Pitch_Position_Control.Kd = 0.01*( (vs16)(*(data_buf+20)<<8)|*(data_buf+21) );   
    ANO_DT_Send_Check(*(data_buf+2),sum);
  }
  if(*(data_buf+2)==0X15)                             //����PID6
  {
    ANO_DT_Send_Check(*(data_buf+2),sum);
    Sort_PID_Cnt++;
    Sort_PID_Flag=1;
    Bling_Set(&Light_1,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
    Bling_Set(&Light_2,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
    Bling_Set(&Light_3,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
  }
}

/***********************************************************
@��������ANO_Data_Send_Status
@��ڲ�������
@���ڲ�������
������������������վ������̬����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ANO_Data_Send_Status(void)//���ͻ�����Ϣ����̬������״̬��
{
  u8 _cnt=0;
  vs16 _temp;
  vs32 _temp2;
  u8 sum = 0;
  u8 i;
  data_to_send[_cnt++]=0xAA;
  data_to_send[_cnt++]=0xAA;
  data_to_send[_cnt++]=0x01;
  data_to_send[_cnt++]=0;
  
  _temp = (int)(Roll*100);
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = (int)(Pitch*100);
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = (int)(-Yaw_Temp*100);
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  
  _temp2 = (vs32)(100*NamelessQuad.Position[_YAW]);//��λcm
  data_to_send[_cnt++]=BYTE3(_temp2);
  data_to_send[_cnt++]=BYTE2(_temp2);
  data_to_send[_cnt++]=BYTE1(_temp2);
  data_to_send[_cnt++]=BYTE0(_temp2);
  
  data_to_send[_cnt++]=0x01;//����ģʽ
  data_to_send[_cnt++]=Controler_State;//����0������1
  
  data_to_send[3] = _cnt-4;
  sum = 0;
  for(i=0;i<_cnt;i++)
    sum += data_to_send[i];
  data_to_send[_cnt++]=sum;
  USART1_Send(data_to_send, _cnt);
}

/***********************************************************
@��������ANO_DT_Send_Senser
@��ڲ�����s16 a_x,s16 a_y,s16 a_z,s16 g_x,s16 g_y,s16 g_z,
s16 m_x,s16 m_y,s16 m_z6
@���ڲ�������
������������������վ���ʹ�����ԭʼ����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ANO_DT_Send_Senser(s16 a_x,s16 a_y,s16 a_z,s16 g_x,s16 g_y,s16 g_z,s16 m_x,s16 m_y,s16 m_z)//���ʹ�����ԭʼ������
{
  u8 _cnt=0;
  vs16 _temp;
  u8 sum = 0;
  u8 i=0;
  data_to_send[_cnt++]=0xAA;
  data_to_send[_cnt++]=0xAA;
  data_to_send[_cnt++]=0x02;
  data_to_send[_cnt++]=0;
  
  _temp = a_x;
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = a_y;
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = a_z;
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  
  _temp = g_x;
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = g_y;
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = g_z;
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  
  _temp = m_x;
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = m_y;
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = m_z;
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  
  data_to_send[3] = _cnt-4;
  
  sum = 0;
  for(i=0;i<_cnt;i++)
    sum += data_to_send[i];
  data_to_send[_cnt++] = sum;
  USART1_Send(data_to_send, _cnt);
}

/***********************************************************
@��������ANO_DT_Send_RCData
@��ڲ�����u16 thr,u16 yaw,u16 rol,u16 pit,u16 aux1,u16 aux2,
u16 aux3,u16 aux4,u16 aux5,u16 aux6
@���ڲ�������
������������������վ����ң��������ͨ������
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ANO_DT_Send_RCData(u16 thr,u16 yaw,u16 rol,u16 pit,u16 aux1,u16 aux2,u16 aux3,u16 aux4,u16 aux5,u16 aux6)//����ң����ͨ������
{
  u8 _cnt=0;
  u8 i=0;
  u8 sum = 0;
  data_to_send[_cnt++]=0xAA;
  data_to_send[_cnt++]=0xAA;
  data_to_send[_cnt++]=0x03;
  data_to_send[_cnt++]=0;
  data_to_send[_cnt++]=BYTE1(thr);
  data_to_send[_cnt++]=BYTE0(thr);
  data_to_send[_cnt++]=BYTE1(yaw);
  data_to_send[_cnt++]=BYTE0(yaw);
  data_to_send[_cnt++]=BYTE1(rol);
  data_to_send[_cnt++]=BYTE0(rol);
  data_to_send[_cnt++]=BYTE1(pit);
  data_to_send[_cnt++]=BYTE0(pit);
  data_to_send[_cnt++]=BYTE1(aux1);
  data_to_send[_cnt++]=BYTE0(aux1);
  data_to_send[_cnt++]=BYTE1(aux2);
  data_to_send[_cnt++]=BYTE0(aux2);
  data_to_send[_cnt++]=BYTE1(aux3);
  data_to_send[_cnt++]=BYTE0(aux3);
  data_to_send[_cnt++]=BYTE1(aux4);
  data_to_send[_cnt++]=BYTE0(aux4);
  data_to_send[_cnt++]=BYTE1(aux5);
  data_to_send[_cnt++]=BYTE0(aux5);
  data_to_send[_cnt++]=BYTE1(aux6);
  data_to_send[_cnt++]=BYTE0(aux6);
  
  data_to_send[3] = _cnt-4;
  
  sum = 0;
  for(i=0;i<_cnt;i++)
    sum += data_to_send[i];
  
  data_to_send[_cnt++]=sum;
  USART1_Send(data_to_send, _cnt);
}

/***********************************************************
@��������ANO_DT_Send_GPSData
@��ڲ�������
@���ڲ�������
������������������վ����GPS��γ������
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ANO_DT_Send_GPSData(u8 Fixstate,
                         u8 GPS_Num,
                         u32 log,
                         u32 lat,
                         int16 gps_head)//����GPS������Ϣ
{
  u8 sum = 0;
  u8 _cnt=0;
  u8 i=0;
  data_to_send[_cnt++]=0xAA;
  data_to_send[_cnt++]=0xAA;
  data_to_send[_cnt++]=0x04;
  data_to_send[_cnt++]=0;
  data_to_send[_cnt++]=Fixstate;
  data_to_send[_cnt++]=GPS_Num;
  
  data_to_send[_cnt++]=BYTE3(log);
  data_to_send[_cnt++]=BYTE2(log);
  data_to_send[_cnt++]=BYTE1(log);
  data_to_send[_cnt++]=BYTE0(log);
  
  data_to_send[_cnt++]=BYTE3(lat);
  data_to_send[_cnt++]=BYTE2(lat);
  data_to_send[_cnt++]=BYTE1(lat);
  data_to_send[_cnt++]=BYTE0(lat);
  
  data_to_send[_cnt++]=BYTE1(gps_head);
  data_to_send[_cnt++]=BYTE0(gps_head);
  
  data_to_send[3] = _cnt-4;
  
  sum = 0;
  for(i=0;i<_cnt;i++)
    sum += data_to_send[i];
  
  data_to_send[_cnt++]=sum;
  USART1_Send(data_to_send, _cnt);
}

/***********************************************************
@��������ANO_SEND_StateMachine
@��ڲ�������
@���ڲ�������
������������������վ����PID����״̬��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ANO_DT_Send_PID(u8 group,float p1_p,float p1_i,float p1_d,float p2_p,float p2_i,float p2_d,float p3_p,float p3_i,float p3_d)//����PID����
{
  u8 _cnt=0;
  u8 sum = 0,i=0;
  int16_t _temp;
  
  data_to_send[_cnt++]=0xAA;
  data_to_send[_cnt++]=0xAA;
  data_to_send[_cnt++]=0x10+group-1;
  data_to_send[_cnt++]=0;
  
  
  _temp = (int16_t)(p1_p * 1000);
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = (int16_t)(p1_i  * 1000);
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = (int16_t)(p1_d  * 100);
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = (int16_t)(p2_p  * 1000);
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = (int16_t)(p2_i  * 1000);
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = (int16_t)(p2_d * 100);
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = (int16_t)(p3_p  * 1000);
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = (int16_t)(p3_i  * 1000);
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  _temp = (int16_t)(p3_d * 100);
  data_to_send[_cnt++]=BYTE1(_temp);
  data_to_send[_cnt++]=BYTE0(_temp);
  
  data_to_send[3] = _cnt-4;
  
  
  for(i=0;i<_cnt;i++)
    sum += data_to_send[i];
  
  data_to_send[_cnt++]=sum;
  USART1_Send(data_to_send, _cnt);
}

/***********************************************************
@��������ANO_SEND_StateMachine
@��ڲ�������
@���ڲ�������
������������������վ�����ݷ��ͺ�������������̬״̬��ԭʼ����
����������ң����ͨ����GPS��γ�ȡ�PID���ݵȣ���Ҫ������վ֡Э
��������
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void ANO_SEND_StateMachine(void)//��������ѭ������
{
	static int16_t ANO_Cnt=0;
  ANO_Cnt++;
  if(ANO_Cnt==1)
  {
    ANO_Data_Send_Status();
  }
  else if(ANO_Cnt==2)
  {
    ANO_DT_Send_Senser((int16_t)WP_Sensor.accel_raw.x,(int16_t)WP_Sensor.accel_raw.y,(int16_t)WP_Sensor.accel_raw.z,
                       (int16_t)WP_Sensor.gyro_raw.x,(int16_t)WP_Sensor.gyro_raw.y,(int16_t)WP_Sensor.gyro_raw.z,
                       (int16_t)WP_Sensor.mag_raw.x,(int16_t)WP_Sensor.mag_raw.y,(int16_t)WP_Sensor.mag_raw.z);
  }
  else if(ANO_Cnt==3)
  {
    ANO_DT_Send_RCData(PPM_Databuf[2],PPM_Databuf[3],
                       PPM_Databuf[0],PPM_Databuf[1],
                       PPM_Databuf[4],PPM_Databuf[5],
                       PPM_Databuf[6],PPM_Databuf[7],0,0);
  }
  else if(ANO_Cnt==4
          &&ANO_Send_PID_Flag[0]==0
            &&ANO_Send_PID_Flag[1]==0
              &&ANO_Send_PID_Flag[2]==0
                &&ANO_Send_PID_Flag[3]==0
                  &&ANO_Send_PID_Flag[4]==0
                    &&ANO_Send_PID_Flag[5]==0)//��ǰ��ֹ���Ͷ���
  {
    ANO_DT_Send_GPSData(1,GPS_Sate_Num,Longitude_Origion,Latitude_Origion,10);
    ANO_Cnt=0;
  }
  else if(ANO_Cnt==4)
  {
    ANO_DT_Send_GPSData(1,GPS_Sate_Num,Longitude_Origion,Latitude_Origion,10);
  }
  else if(ANO_Cnt==5&&ANO_Send_PID_Flag[0]==1)
  {
    ANO_DT_Send_PID(1,Total_Controller.Roll_Gyro_Control.Kp,
                    Total_Controller.Roll_Gyro_Control.Ki,
                    Total_Controller.Roll_Gyro_Control.Kd,
                    Total_Controller.Pitch_Gyro_Control.Kp,
                    Total_Controller.Pitch_Gyro_Control.Ki,
                    Total_Controller.Pitch_Gyro_Control.Kd,
                    Total_Controller.Yaw_Gyro_Control.Kp,
                    Total_Controller.Yaw_Gyro_Control.Ki,
                    Total_Controller.Yaw_Gyro_Control.Kd);
    ANO_Send_PID_Flag[0]=0;
  }
  else if(ANO_Cnt==6&&ANO_Send_PID_Flag[1]==1)
  {
    ANO_DT_Send_PID(2,Total_Controller.Roll_Angle_Control.Kp,
                    Total_Controller.Roll_Angle_Control.Ki,
                    Total_Controller.Roll_Angle_Control.Kd,
                    Total_Controller.Pitch_Angle_Control.Kp,
                    Total_Controller.Pitch_Angle_Control.Ki,
                    Total_Controller.Pitch_Angle_Control.Kd,
                    Total_Controller.Yaw_Angle_Control.Kp,
                    Total_Controller.Yaw_Angle_Control.Ki,
                    Total_Controller.Yaw_Angle_Control.Kd);
    ANO_Send_PID_Flag[1]=0;
  }
  else if(ANO_Cnt==7&&ANO_Send_PID_Flag[2]==1)
  {
    ANO_DT_Send_PID(3,Total_Controller.High_Speed_Control.Kp,
                    Total_Controller.High_Speed_Control.Ki,
                    Total_Controller.High_Speed_Control.Kd,
                    Total_Controller.High_Position_Control.Kp,
                    Total_Controller.High_Position_Control.Ki,
                    Total_Controller.High_Position_Control.Kd,
                    Total_Controller.Latitude_Speed_Control.Kp,
                    Total_Controller.Latitude_Speed_Control.Ki,
                    Total_Controller.Latitude_Speed_Control.Kd);
    ANO_Send_PID_Flag[2]=0;
  }
  else if(ANO_Cnt==8&&ANO_Send_PID_Flag[3]==1)
  {
    ANO_DT_Send_PID(4,Total_Controller.Latitude_Position_Control.Kp,
                    Total_Controller.Latitude_Position_Control.Ki,
                    Total_Controller.Latitude_Position_Control.Kd,
                    Total_Controller.High_Acce_Control.Kp,
                    Total_Controller.High_Acce_Control.Ki,
                    Total_Controller.High_Acce_Control.Kd,
                    0,0,0);
    ANO_Send_PID_Flag[3]=0;
  }
  else if(ANO_Cnt==9&&ANO_Send_PID_Flag[4]==1)
  {
    ANO_DT_Send_PID(5,Total_Controller.Optical_Position_Control.Kp
                    ,Total_Controller.Optical_Position_Control.Ki
                      ,Total_Controller.Optical_Position_Control.Kd
                        ,Total_Controller.Optical_Speed_Control.Kp
                          ,Total_Controller.Optical_Speed_Control.Ki
                            ,Total_Controller.Optical_Speed_Control.Kd
                              ,Total_Controller.SDK_Roll_Position_Control.Kp
                                ,Total_Controller.SDK_Roll_Position_Control.Ki
                                  ,Total_Controller.SDK_Roll_Position_Control.Kd);
    ANO_Send_PID_Flag[4]=0;
  }
  else if(ANO_Cnt==10&&ANO_Send_PID_Flag[5]==1)
  {
    ANO_DT_Send_PID(6,0,0,0,
                    0,0,0,
                    0,0,0);
    ANO_Send_PID_Flag[5]=0;
    ANO_Cnt=0;
  }
}

/***********************************************************
@��������Vcan_Send
@��ڲ�������
@���ڲ�������
����������ɽ�����ݷ��ͺ�����Ĭ�Ϸ���8��ͨ������������Ϊfloat
��ÿ��ͨ�����ݿ����Լ�����
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
extern Point SDK_Real_Point;
extern u8 RxBuffer_mine[20];
float Point_x = 0;
float Point_y = 0;
void Vcan_Send(void)//ɽ�����վ����
{
  static float DataBuf[8];	
	
	Point_x = (float)SDK_Real_Point.x;
	Point_y = (float)SDK_Real_Point.y;

/*
	DataBuf[0]=Pitch;//�ߵ��߶�
  DataBuf[1]=Roll;//�ߵ��ٶ�
  DataBuf[2]=Yaw;;//�ߵ����ٶ�
  DataBuf[3]=Pitch_Observation;//��ѹ�Ƹ߶�
  DataBuf[4]=Roll_Observation;
  DataBuf[5]=Yaw_Observation;
  DataBuf[6]=0;
  DataBuf[7]=0;

  DataBuf[0]=NamelessQuad.Position[_YAW];//�ߵ��߶�
  DataBuf[1]=NamelessQuad.Speed[_YAW];//�ߵ��ٶ�
  DataBuf[2]=observation_div;//NamelessQuad.Acceleration[_YAW];;//�ߵ����ٶ�
  DataBuf[3]=Observation_Altitude;//��ѹ�Ƹ߶�
	DataBuf[4]=observation_acc;
  DataBuf[5]=NamelessQuad.Vel_History[_YAW][30];
  DataBuf[6]=WP_Sensor.baro_altitude_div;
	DataBuf[7]=NamelessQuad.Acceleration[_YAW];*/
	

//	DataBuf[0]=Total_Controller.Pitch_Angle_Control.Expect;
//  DataBuf[1]=Total_Controller.Pitch_Gyro_Control.Expect;
//  DataBuf[2]=Total_Controller.Pitch_Angle_Control.FeedBack;
//  DataBuf[3]=Total_Controller.Pitch_Gyro_Control.FeedBack;
//	DataBuf[4]=Total_Controller.Roll_Angle_Control.Expect;
//  DataBuf[5]=Total_Controller.Roll_Gyro_Control.Expect;
//  DataBuf[6]=Total_Controller.Roll_Angle_Control.FeedBack;
//  DataBuf[7]=Total_Controller.Roll_Gyro_Control.FeedBack;

//	DataBuf[0]=RxBuffer_mine[0];
//  DataBuf[1]=RxBuffer_mine[1];
//  DataBuf[2]=RxBuffer_mine[2];
//  DataBuf[3]=RxBuffer_mine[3];
//	DataBuf[4]=RxBuffer_mine[4];
//  DataBuf[5]=RxBuffer_mine[5];
//  DataBuf[6]=RxBuffer_mine[6];
//  DataBuf[7]=RxBuffer_mine[7];

	DataBuf[0]=0;
  DataBuf[1]=0;
  DataBuf[2]=0;
  DataBuf[3]=0;
	DataBuf[4]=0;
  DataBuf[5]=0;
  DataBuf[6]=0;
  DataBuf[7]=0;

//		DataBuf[0]=
//		DataBuf[1]=
//		DataBuf[2]=
//		DataBuf[3]=
//		DataBuf[4]=
//		DataBuf[5]=
//		DataBuf[6]=
//	  DataBuf[7]=
	/*
  
  DataBuf[0]=GPS_Vel_Div.E;//�ߵ��߶�
  DataBuf[1]=GPS_Vel_Div.N;//�ߵ��ٶ�
  DataBuf[2]=NamelessQuad.Acceleration[_PITCH];;//�ߵ����ٶ�
  DataBuf[3]=NamelessQuad.Acceleration[_ROLL];;//�ߵ����ٶ�
  DataBuf[4]=0;//�ߵ��߶�;
  DataBuf[5]=0;
  DataBuf[6]=0;
  DataBuf[7]=0;   
  
  //2      
  
  DataBuf[0]=gyro_filter_data.y;
  DataBuf[1]=opt_gyro_filter_data.y;
  DataBuf[2]=OpticalFlow_Speed.y;
  DataBuf[3]=OpticalFlow_SINS.Speed[_ROLL];
  DataBuf[4]=gyro_filter_data.y;
  DataBuf[5]=opt_gyro_data.y;
  DataBuf[6]=opt_origin_data.pixel_flow_y_integral;
  DataBuf[7]=opt_filter_data.y;   
  
  DataBuf[0]=gyro_filter_data.x;//Pitch X
  DataBuf[1]=gyro_filter_data.y;//Roll R
  DataBuf[2]=opt_gyro_data.x;
  DataBuf[3]=opt_gyro_data.y;
  DataBuf[4]=opt_filter_data.x;
  DataBuf[5]=opt_filter_data.y;
  DataBuf[6]=opt_gyro_filter_data.x;
  DataBuf[7]=opt_gyro_filter_data.y;
  
  
  DataBuf[0]=PPM_Databuf[0];
  DataBuf[1]=PPM_Databuf[1];
  DataBuf[2]=PPM_Databuf[2];
  DataBuf[3]=PPM_Databuf[3];
  DataBuf[4]=PPM_LPF_Databuf[0];
  DataBuf[5]=PPM_LPF_Databuf[1];
  DataBuf[6]=Total_Controller.Pitch_Gyro_Control.Dis_Err;
  DataBuf[7]=Total_Controller.Pitch_Gyro_Control.Dis_Error_History[0];
  */
  //3
   /*
  DataBuf[0]=NamelessQuad.Position[_PITCH];
  DataBuf[1]=NamelessQuad.Speed[_PITCH];
  DataBuf[2]=GPS_Vel.E;
  DataBuf[3]=Earth_Frame_To_XYZ.E;
  DataBuf[4]=NamelessQuad.Position[_ROLL];
  DataBuf[5]=NamelessQuad.Speed[_ROLL];
  DataBuf[6]=GPS_Vel.N;
  DataBuf[7]=Earth_Frame_To_XYZ.N;
 
  DataBuf[0]=NamelessQuad.Position[_PITCH];
  DataBuf[1]=0;
  DataBuf[2]=NamelessQuad.Position[_ROLL];
  DataBuf[3]=0;
  DataBuf[4]=GPS_Ground_Speed;
  DataBuf[5]=sqrt(NamelessQuad.Speed[_PITCH]*NamelessQuad.Speed[_PITCH]+NamelessQuad.Speed[_ROLL]*NamelessQuad.Speed[_ROLL]);
  DataBuf[6]=0;
  DataBuf[7]=NamelessQuad.Position[_YAW];//�ߵ��߶�
  */
  wust_sendware((unsigned char *)DataBuf,sizeof(DataBuf));
}



/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/


