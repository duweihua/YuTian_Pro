#include "main.h"


static void iic_Delay(void)
{
	uint8_t i;

	//	�����ʱ����ͨ���������ǲ��Եõ���
	//	CPU��Ƶ72MHzʱ ���ڲ�Flash���� MDK���̲��Ż�
	//	ѭ������Ϊ10ʱ	SCLƵ�� = 205KHz 
	//	ѭ������Ϊ7ʱ	SCLƵ�� = 347KHz	SCL�ߵ�ƽʱ��1.5us	SCL�͵�ƽʱ��2.87us 
	//	ѭ������Ϊ5ʱ	SCLƵ�� = 421KHz	SCL�ߵ�ƽʱ��1.25us	SCL�͵�ƽʱ��2.375us 

	for (i = 0; i < 10; i++);
}


//SCL�� SDA�½��� --> �����ź�
void imu_iic_Start(void)
{
	IMU_IIC_SDA_1();
	IMU_IIC_SCL_1();
	iic_Delay();
	IMU_IIC_SDA_0();
	iic_Delay();
	IMU_IIC_SCL_0();
	iic_Delay();
}


//SCL�� SDA������ --> ֹͣ�ź�
void imu_iic_Stop(void)
{
	IMU_IIC_SDA_0();
	IMU_IIC_SCL_1();
	iic_Delay();
	IMU_IIC_SDA_1();
}


void imu_iic_WriteByte(uint8_t _ucByte)
{
	uint8_t i;

	//�����λ��ʼ����
	for(i = 0; i < 8; i ++)
	{
		if(_ucByte & 0x80)
			IMU_IIC_SDA_1();

		else
			IMU_IIC_SDA_0();

		iic_Delay();
		IMU_IIC_SCL_1();
		iic_Delay();
		IMU_IIC_SCL_0();
		
		if (i == 7)
			//�ͷ�����
			IMU_IIC_SDA_1(); 

		_ucByte <<= 1;
		
		iic_Delay();
	}
}


uint8_t imu_iic_ReadByte(u8 ack)
{
	uint8_t i;
	uint8_t value = 0;

	//���ȶ�����Ϊ���λ
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		
		IMU_IIC_SCL_1();
		iic_Delay();
		
		if (IMU_IIC_SDA_READ())
			value++;

		IMU_IIC_SCL_0();
		iic_Delay();
	}
	
	if(ack==0)
		imu_iic_NAck();
	
	else
		imu_iic_Ack();
	
	return value;
}


uint8_t imu_iic_WaitAck(void)
{
	uint8_t re;

	//CPU�ͷ�SDA����
	IMU_IIC_SDA_1();	
	iic_Delay();
	
	//CPU����SCL=1 
	//��ʱ�����᷵��ACKӦ��
	IMU_IIC_SCL_1();
	iic_Delay();
	
	if (IMU_IIC_SDA_READ())	
		re = 1;

	else
		re = 0;

	IMU_IIC_SCL_0();
	iic_Delay();
	
	return re;
}


void imu_iic_Ack(void)
{
	//CPU����SDA=0
	IMU_IIC_SDA_0();
	iic_Delay();
	
	//CPU����1��ʱ��
	IMU_IIC_SCL_1();	
	iic_Delay();
	IMU_IIC_SCL_0();
	iic_Delay();
	
	//CPU�ͷ�SDA����
	IMU_IIC_SDA_1();	
}


void imu_iic_NAck(void)
{
	//CPU����SDA=1
	IMU_IIC_SDA_1();	
	iic_Delay();
	
	//CPU����1��ʱ��
	IMU_IIC_SCL_1();	
	iic_Delay();
	IMU_IIC_SCL_0();
	iic_Delay();	
}


void imu_iic_gpio_init(void)
{
	GPIO_InitTypeDef 	gpio;

	RCC_APB2PeriphClockCmd(IMU_RCC_IIC_PORT,ENABLE);

	gpio.GPIO_Pin = IMU_IIC_SCL_PIN;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(IMU_GPIO_PORT_IIC,&gpio);
	
	gpio.GPIO_Pin = IMU_IIC_SDA_PIN;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_Out_OD;  
	GPIO_Init(IMU_GPIO_PORT_IIC,&gpio);

	//ֹͣ�ź�
	//��λIIC�����ϵ������豸������ģʽ
	imu_iic_Stop();
}


uint8_t imu_iic_init(void)
{
	uint8_t ucAck;

	imu_iic_gpio_init();
	
	imu_iic_Start();

	//�����豸��ַ+��д����λ
	//��д����λ��(0 = w ; 1 = r)
	imu_iic_WriteByte(MPU6050_SLAVE_ADDRESS|IIC_WR);
	
	ucAck = imu_iic_WaitAck();	

	imu_iic_Stop();

	return ucAck;
}


//PB.6����SCL
//PB.7����SDA
//void oled_iic_init(void)
//{
//	GPIO_InitTypeDef 	gpio;
//	
//	RCC_APB2PeriphClockCmd(OLED_RCC_IIC_PORT,ENABLE);
//	
//	gpio.GPIO_Pin = OLED_IIC_SCL_PIN|OLED_IIC_SDA_PIN;
//    gpio.GPIO_Speed = GPIO_Speed_50MHz;
//    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
//	
//	GPIO_Init(OLED_GPIO_PORT_IIC,&gpio);
//}


//SDA�����������
void oled_SDA_OUT(void)
{
	GPIO_InitTypeDef gpio;
	
    gpio.GPIO_Pin= OLED_IIC_SDA_PIN;
    gpio.GPIO_Speed=GPIO_Speed_50MHz;
    gpio.GPIO_Mode=GPIO_Mode_Out_PP;

    GPIO_Init(OLED_GPIO_PORT_IIC,&gpio);
}


//SDA���롪������
void oled_SDA_IN(void)
{
	GPIO_InitTypeDef gpio;
	
    gpio.GPIO_Pin= OLED_IIC_SDA_PIN;
    gpio.GPIO_Speed=GPIO_Speed_50MHz;
    gpio.GPIO_Mode=GPIO_Mode_IPU;

    GPIO_Init(OLED_GPIO_PORT_IIC,&gpio);
}


void oled_iic_Start(void)
{
	oled_SDA_OUT();
	
	OLED_IIC_SDA_1();
	OLED_IIC_SCL_1();
	delay_us(2);
	
	OLED_IIC_SDA_0();
	delay_us(2);
	
	OLED_IIC_SCL_0();
	delay_us(2);
}


void oled_iic_Stop(void)
{
	oled_SDA_OUT();
	
	OLED_IIC_SCL_1();
	OLED_IIC_SDA_0();
	delay_us(2);
	
	OLED_IIC_SDA_1();
	delay_us(2);
}


//uint8_t oled_iic_Waitask(void)
//{
//	
//}








