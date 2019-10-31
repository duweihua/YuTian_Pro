#include "main.h"

//����ѹ���������ͺţ�	HX711

//����ϵ��ȷ�����ݣ�
//	42500 	-->  160g --> 265.65
//	80000 	-->  285g --> 280.7
//	115000 	-->  405g --> 283.95
//	405000 	--> 1400g --> 289.29
//���ֱ�ߣ�
// y = 291.92x-3580.2 (���Խؾ�)

float Kp_Weight = 293.0f;


unsigned long HX711_Buffer = 0;
unsigned long Weight_No_Lode = 0;
unsigned long Weight_Lode = 0;

float Weight_Real = 0;

extern u8 No_Load_Flag;


void Sensor_Init(void)
{
	GPIO_InitTypeDef	gpio;  

	RCC_APB2PeriphClockCmd(Sensor_Clock,ENABLE);
	
	//ʱ�����������
	gpio.GPIO_Pin = CLK;	
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Sensor_Gpio,&gpio);

	//�����߸�������
	gpio.GPIO_Pin = DATA;	
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Sensor_Gpio,&gpio);
}


unsigned long Sensor_Read(void)
{
	unsigned long value;
	unsigned char i;
	
	//ÿ�ζ�ȡ����ǰ��֤�����ߵ�ƽ�ȶ�
	//�˴�ֻ��Ϊ���ȶ���ƽ ���߻�����Ч����ͬ
//	GPIO_ResetBits(Sensor_Gpio,DATA);
	GPIO_SetBits(Sensor_Gpio,DATA);
	
	//Ϊ�˵ȴ������ƽ�ȶ�
	//��ÿ��һ������ƽʱ��΢С��ʱ
	delay_us(2);
	
	//ʱ�������� ����ʱʱ���߱��ֵ͵�λ
	GPIO_ResetBits(Sensor_Gpio,CLK);
	
	delay_us(2);	
	
	//�ȴ�ADת������
	while(GPIO_ReadInputDataBit(Sensor_Gpio,DATA));
	
	for(i=0;i<24;i++)
	{
		//ʱ�������� ��ʼ����ʱ������
		GPIO_SetBits(Sensor_Gpio,CLK);
		
		delay_us(2);
		
		//����λ �Ҳಹ�� �ȴ���������
		value = value << 1;
		
		//ʱ��������
		GPIO_ResetBits(Sensor_Gpio,CLK);
		
		delay_us(2);
		
		//��ȡһλ����
		if(GPIO_ReadInputDataBit(Sensor_Gpio,DATA))
			value ++;
	}
	
	//��25������
	GPIO_SetBits(Sensor_Gpio,CLK);
	
	delay_us(2);

	//��25�������½��ص���ʱ ת������
	//�˴�˵����
	//			HX711��һ��24λ��ADת��оƬ
	//			���λ�Ƿ���λ ����Ϊ��Чλ
	//			���������Сֵ0x800000
	//					���ֵ0x7FFFFF
	//������㣺
	//			��ͬΪ0 
	//			��ͬΪ1
	//���ݴ���˵����
	//			֮���Իᷢ�� INPA-INNA < 0mv �����
	//			����Ϊ���������Ư��
	//			������������ݾ��ǳ�ʼ״̬INPA-INNA = -0.5mv
	//			Ȼ���������������ӻᷢ�������
	//			��ʱ���ֱ��ʹ�ö�ȡ�������ݾͻᷢ������
	//			��Ϊ��ȡ������С��0�Ķ����Ʋ���
	//			�ǲ���ֱ��ʹ�õ� ��Ҫת������ԭ��

	//			�Ƚϼ򵥵Ĵ��������Ƕ���������ֱ�Ӻ�0x800000�������
	//			��ʱ���λ���Կ�������Чλ
	//			���������λ���������һλ�Ľ�λ
	//			�������ݻ�һֱ�������� 
	//			���ǿ���ֱ����������ʹ��
	value = value^0x800000;
//	value = value&0x7FFFFF;
	
	//��25���������
	GPIO_ResetBits(Sensor_Gpio,CLK);
	
	delay_us(2);
	
	return value;
}


void Get_No_Lode(void)
{
	HX711_Buffer = Sensor_Read();
	
	Weight_No_Lode = HX711_Buffer;
	
	No_Load_Flag = 1;
}


void Get_Weight(void)
{
	HX711_Buffer = Sensor_Read();
	
	Weight_Lode = HX711_Buffer;
		
	//�жϷǿ���
	if(Weight_Lode > Weight_No_Lode)
	{
		Weight_Real = (Weight_Lode - Weight_No_Lode)/Kp_Weight;
	}
	else if(Weight_Lode <= Weight_No_Lode)
		Weight_Real = 0.0f;
	
	//�����ﵽһ����ֵ �򴮿���ʾ
//	if(Weight_Real>100.0f)
//		printf("��ǰ����%u\r\n\r\n",Weight_Real);
}




