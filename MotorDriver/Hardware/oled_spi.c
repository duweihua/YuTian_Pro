//#include "main.h"
//#include "oledfont.h"


////OLED�ֱ��ʣ�128*64

////OLED�Դ��Ÿ�ʽ:
////
////		[0]0 1 2 3 ... 127	
////		[1]0 1 2 3 ... 127	
////		[2]0 1 2 3 ... 127	
////		[3]0 1 2 3 ... 127	
////		[4]0 1 2 3 ... 127	
////		[5]0 1 2 3 ... 127	
////		[6]0 1 2 3 ... 127	
////		[7]0 1 2 3 ... 127 

//u16 OLED_GRAM[132][8];	 


////�����Դ�	 
//void OLED_Refresh_Gram(void)
//{
//	u8 i,n;		    
//	for(i=0;i<8;i++)  
//	{
//		//����ҳ��ַ(0~7)
//		//8����С�е�λ����ҳ
//		OLED_WR_Byte (0xb0+i,OLED_CMD);  
//		//������ʾ��ַ 
//		//Ϊ�˸���ȫ�� �˴����õ�ַΪ0
//		//�е͵�ַ(��4λ)
//		OLED_WR_Byte (0x00,OLED_CMD);  
//		//�иߵ�ַ(��4λ) 
//		OLED_WR_Byte (0x10,OLED_CMD); 
//		for(n=0;n<131;n++)
//			OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
//	}
//}


////���ߴ���ģʽ

////��SSD1306д��һ���ֽ�
////����ʱ��ͼ���õ�ƽʵ��
////dat:	Ҫд�������/����
////cmd:	����/�����־ 
////			0����ʾ����
////			1����ʾ����
//void OLED_WR_Byte(u8 dat,u8 cmd)
//{	
//	u8 i;			  
//	if(cmd)
//	  OLED_RS_Set();	
//	else 
//	  OLED_RS_Clr();
////	OLED_CS_Clr();		
//	for(i=0;i<8;i++)
//	{
//		OLED_SCLK_Clr();
//		//���λ����
//		//����������������ʱ����
//		if(dat&0x80)
//		   OLED_SDIN_Set();
//		else 
//		   OLED_SDIN_Clr();
//		OLED_SCLK_Set();
//		dat<<=1;   
//	}			
////	OLED_CS_Set();
//	OLED_RS_Set();   	  
//}

//	  	  
////����OLED��ʾ    
//void OLED_Display_On(void)
//{
//	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
//	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
//	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
//}

////�ر�OLED��ʾ
//void OLED_Display_Off(void)
//{
//	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
//	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
//	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
//}		   	


////��������
////������ ������Ļ�Ǻ�ɫ�� ��û����һ��	  
//void OLED_Clear(void)
//{  
//	u8 i,n;  
//	for(i=0;i<8;i++)
//		for(n=0;n<130;n++)
//			OLED_GRAM[n][i]=0X00;  
//	OLED_Refresh_Gram();
//}


////���㺯��
////���꣺
////		x��0~127
////		y��0~63
////��ɫ��
////		t=1����� 
////		t=0�����				   
//void OLED_DrawPoint(u8 x,u8 y,u8 t)
//{
//	u8 pos 	= 0;
//	u8 bx 	= 0;
//	u8 temp = 0;
//	//������Χ
//	if(x>131||y>63)
//		return;
//	//һ�β���8λ
//	//Ҫ����Ӱ������λ
//	pos=7-y/8;
//	bx=y%8;
//	temp=1<<(7-bx);
//	if(t)
//		OLED_GRAM[x][pos]|=temp;
//	else 
//		OLED_GRAM[x][pos]&=~temp;	    
//}


////�������
////�Խ����꣺
////			x1,y1,x2,y2
////ע��ȷ����
////			x1<=x2
////			y1<=y2 
////			0<=x1<=127 
////			0<=y1<=63	 	 
////��ɫdot:
////			0�����
////			1�����	  
//void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
//{
//	u8 x,y;  
//	//����
//	for(x=x1;x<=x2;x++)
//		for(y=y1;y<=y2;y++)
//			OLED_DrawPoint(x,y,dot);	
//	//������ʾ
//	OLED_Refresh_Gram();
//}


////��ָ��λ����ʾһ���ַ�
////ֻ���������ַ�
////���꣺
////		x:0~127
////		y:0~63
////mode:
////		0��������ʾ
////		1��������ʾ
////����size:
////		 	16/12 
//void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
//{
//	u8 temp,t,t1;
//	u8 y0=y;
//	//�ַ����е�һ���ַ�Ϊ' '
//	//����õ�ƫ��ֵ
//	//��ASCIIת��Ϊ�ַ����еص�ַ
//	chr=chr-' ';	   
//    for(t=0;t<size;t++)
//    {
//		//��������ѡ���ַ���
//		if(size==12)	//1206
//			temp=oled_asc2_1206[chr][t]; 
//		else 			//1608
//			temp=oled_asc2_1608[chr][t];
//		//�����ַ�������ַ�
//        for(t1=0;t1<8;t1++)
//		{
//			if(temp&0x80)
//				OLED_DrawPoint(x,y,mode);
//			else 
//				OLED_DrawPoint(x,y,!mode);
//			temp<<=1;
//			y++;
//			if((y-y0)==size)
//			{
//				y=y0;
//				x++;
//				break;
//			}
//		}
//    }        
//}


////���ݺ��� (m^n)
//u32 oled_pow(u8 m,u8 n)
//{
//	u32 result=1;	 
//	while(n--)
//		result*=m;    
//	return result;
//}


////��ʾ����
////������꣺x,y  
////���ֳ��ȣ�len 
////�����С��size
////��ֵ��num(0~4294967295)		  
//void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
//{         	
//	u8 t,temp;
//	u8 enshow=0;						   
//	for(t=0;t<len;t++)
//	{
//		//��λ�ֽ�����
//		temp=(num/oled_pow(10,len-t-1))%10;
//		if((enshow==0)&&(t<(len-1)))
//		{
//			//��λ������ ����ʾ�ո�
//			if(temp==0)
//			{
//				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
//				continue;
//			}
//			else 
//				enshow=1; 
//		}
//		//����0��ASCII�뽫����ת��ΪASCII����ʾ
//	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
//	}
//} 


////��ʾ�ַ���
////������꣺x,y  
////�ַ����׵�ַ��*p
////(Ĭ��ʹ��16����)
////��'\0'��β
//void OLED_ShowString(u8 x,u8 y,const u8 *p)
//{
//	#define MAX_CHAR_POSX 126
//	#define MAX_CHAR_POSY 58   
//	//�жϽ�β��־�ַ�
//    while(*p!='\0')
//    {
//		//�������ת����ʾ
//        if(x>MAX_CHAR_POSX)
//		{
//			x=0;
//			y+=16;
//		}
//		//�������������ͷ��ʼ��ʾ
//        if(y>MAX_CHAR_POSY)
//		{
//			y=x=0;
//			OLED_Clear();
//		}
//        OLED_ShowChar(x,y,*p,16,1);	 
//        x+=8;
//        p++;
//    }
//}


////��ʼ��SSD1306
//void OLED_Init(void)
//{
// 	GPIO_InitTypeDef  gpio;
// 	
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	

//	gpio.GPIO_Pin = GPIO_Pin_0;	 
// 	gpio.GPIO_Mode = GPIO_Mode_Out_PP; 		
//	gpio.GPIO_Speed = GPIO_Speed_50MHz;		
// 	GPIO_Init(GPIOB,&gpio);	
//	
//	gpio.GPIO_Pin = GPIO_Pin_1;
// 	GPIO_Init(GPIOB,&gpio);	

//	gpio.GPIO_Pin = GPIO_Pin_3;
// 	GPIO_Init(GPIOB,&gpio);	

//	gpio.GPIO_Pin = GPIO_Pin_4;
// 	GPIO_Init(GPIOB,&gpio);	
//		
//	//ȫ������
// 	GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4);	

//	OLED_RST_Clr();
//	delay_ms(100);
//	OLED_RST_Set();
//					 
//	//�ر���ʾ
//	OLED_WR_Byte(0xAE,OLED_CMD); 
//	//����ʱ�ӷ�Ƶ���� ��Ƶ��
//	//[3:0] ��Ƶ����
//	//[7:4] ��Ƶ��
//	OLED_WR_Byte(0xD5,OLED_CMD); 
//	OLED_WR_Byte(80,OLED_CMD);
//	//��������·��
//	//Ĭ��0X3F(1/64) 
//	OLED_WR_Byte(0xA8,OLED_CMD); 
//	OLED_WR_Byte(0X3F,OLED_CMD);
//	//������ʾƫ��
//	//Ĭ��Ϊ0
//	OLED_WR_Byte(0xD3,OLED_CMD);
//	OLED_WR_Byte(0X00,OLED_CMD);
//	//������ʾ��ʼ�� [5:0] ����.
//	OLED_WR_Byte(0x40,OLED_CMD);
//	//��ɱ�����								    
//	OLED_WR_Byte(0x8D,OLED_CMD); 
//	//bit2 ����/�ر�
//	OLED_WR_Byte(0x14,OLED_CMD); 
//	//�����ڴ��ַģʽ
//	//[1:0]		(Ĭ��10)
//	//		00���е�ַģʽ
//	//		01���е�ַģʽ
//	//		10��ҳ��ַģʽ
//	OLED_WR_Byte(0x20,OLED_CMD); 
//	OLED_WR_Byte(0x02,OLED_CMD);
//	//���ض������� 
//	//bit0:	
//	//		0,0->0
//	//		1,0->127
//	OLED_WR_Byte(0xA1,OLED_CMD); 
//	//����COMɨ�跽��
//	//bit3:
//	//		0����ͨģʽ
//	//		1���ض���ģʽ 
//	//COM[N-1]->COM0
//	//N������·��
//	OLED_WR_Byte(0xC0,OLED_CMD); 
//	//����COMӲ����������
//	//[5:4]����
//	OLED_WR_Byte(0xDA,OLED_CMD); 
//	OLED_WR_Byte(0x12,OLED_CMD); 
//	//�Աȶ�����	 
//	OLED_WR_Byte(0x81,OLED_CMD); 
//	//��������,Խ��Խ��
//	//(1~255)
//	//Ĭ��0X7F
//	OLED_WR_Byte(0xEF,OLED_CMD);
//	//����Ԥ�������
//	//[3:0]��PHASE 1
//	//[7:4]��PHASE 2
//	OLED_WR_Byte(0xD9,OLED_CMD);
//	OLED_WR_Byte(0xf1,OLED_CMD); 
//	//����VCOMH ��ѹ����
//	//[6:4]
//	//		000��0.65*vcc
//	//		001��0.77*vcc
//	//		011��0.83*vcc
//	OLED_WR_Byte(0xDB,OLED_CMD); 
//	OLED_WR_Byte(0x30,OLED_CMD); 
//	//ȫ����ʾ����
//	//bit0��
//	//		1������
//	//		0���ر�
//	//		(����/����)
//	OLED_WR_Byte(0xA4,OLED_CMD); 
//	//������ʾ��ʽ
//	//bit0��
//	//		1��������ʾ
//	//		0��������ʾ	
//	OLED_WR_Byte(0xA6,OLED_CMD);   
//	//������ʾ	
//	OLED_WR_Byte(0xAF,OLED_CMD);  
//	//����
//	OLED_Clear();
//}  















