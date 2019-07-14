#include "main.h"
#include "oledtable.h"


void oled_iic_init(void)
{
	I2C_InitTypeDef  	iic;
	GPIO_InitTypeDef 	gpio; 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(OLED_RCC_IIC_PORT,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,DISABLE);

	gpio.GPIO_Pin = OLED_IIC_SCL_PIN|OLED_IIC_SDA_PIN;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	//I2C必须开漏输出
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(OLED_GPIO_PORT_IIC,&gpio);

	I2C_DeInit(I2C2);
	iic.I2C_Mode = I2C_Mode_I2C;
	iic.I2C_DutyCycle = I2C_DutyCycle_2;
	iic.I2C_OwnAddress1 = 0x30;
	iic.I2C_Ack = I2C_Ack_Enable;
	iic.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	iic.I2C_ClockSpeed = 400000;

	I2C_Cmd(I2C2,ENABLE);
	I2C_Init(I2C2,&iic);
}


void OLED_IIC_WriteByte(uint8_t addr,uint8_t data)
{
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C2,ENABLE);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C2,OLED_ADDRESS,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C2,addr);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2C2,data);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C2,ENABLE);
}


void OLED_WriteCmd(unsigned char I2C_Command)
{
	OLED_IIC_WriteByte(OLED_CMD_ADDRESS,I2C_Command);
}


void OLED_WriteDat(unsigned char I2C_Data)
{
	OLED_IIC_WriteByte(OLED_DAT_ADDRESS,I2C_Data);
}




void oled_init(void)
{
	//这里的延时很重要
	delay_ms(100); 
	//display off
	OLED_WriteCmd(0xAE);
	//Set Memory Addressing Mode	
	OLED_WriteCmd(0x20);
	//00：Horizontal Addressing Mode
	//01：Vertical 	 Addressing Mode
	//10：Page 		 Addressing Mode(RESET)
	//11：Invalid
	OLED_WriteCmd(0x10);	
	//Set Page Start Address for Page Addressing Mode(0-7)
	OLED_WriteCmd(0xb0);	
	//Set COM Output Scan Direction
	OLED_WriteCmd(0xc8);	
	//set low column address
	OLED_WriteCmd(0x00); 
	//set high column address
	OLED_WriteCmd(0x10);
	//set start line address
	OLED_WriteCmd(0x40); 
	//set contrast control register
	OLED_WriteCmd(0x81); 
	//set brightness(0x00~0xff)
	OLED_WriteCmd(0xff); 
	//set segment re-map(0~127)
	OLED_WriteCmd(0xa1); 
	//set normal display
	OLED_WriteCmd(0xa6); 
	//set multiplex ratio(1~64)
	OLED_WriteCmd(0xa8); 
	OLED_WriteCmd(0x3F); 
	//0xa4：Output follows RAM content
	//0xa5：Output ignores RAM content
	OLED_WriteCmd(0xa4); 
	//set display offset
	OLED_WriteCmd(0xd3);
	//not offset
	OLED_WriteCmd(0x00); 
	//set display clock divide ratio/oscillator frequency
	OLED_WriteCmd(0xd5); 
	//set divide ratio
	OLED_WriteCmd(0xf0);
	//set pre-charge period
	OLED_WriteCmd(0xd9); 
	OLED_WriteCmd(0x22); 
	//set com pins hardware configuration
	OLED_WriteCmd(0xda);
	OLED_WriteCmd(0x12);
	//set vcomh
	OLED_WriteCmd(0xdb); 
	//0x20：0.77xVcc
	OLED_WriteCmd(0x20); 
	//set DC-DC enable
	OLED_WriteCmd(0x8d); 
	OLED_WriteCmd(0x14); 
	//turn on oled panel
	OLED_WriteCmd(0xaf); 
}


//设置起始点坐标
void OLED_SetPos(unsigned char x, unsigned char y) 
{ 
	OLED_WriteCmd(0xb0+y);
	OLED_WriteCmd(((x&0xf0)>>4)|0x10);
	OLED_WriteCmd((x&0x0f)|0x01);
}


//全屏填充
void OLED_Fill(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		//page0-page1
		OLED_WriteCmd(0xb0+m);
		//low column start address
		OLED_WriteCmd(0x00);		
		//high column start address
		OLED_WriteCmd(0x10);		
		for(n=0;n<128;n++)
		{
			OLED_WriteDat(fill_Data);
		}
	}
}


//清屏
void OLED_CLS(void)
{
	OLED_Fill(0x00);
}


//唤醒OLED(结束休眠)
void OLED_ON(void)
{
	//设置电荷泵
	OLED_WriteCmd(0X8D);  
	//开启电荷泵
	OLED_WriteCmd(0X14); 
	//OLED唤醒	
	OLED_WriteCmd(0XAF); 
}


//OLED休眠
void OLED_OFF(void)
{
	//设置电荷泵
	OLED_WriteCmd(0X8D);  
	//关闭电荷泵
	OLED_WriteCmd(0X10); 
	//OLED休眠
	OLED_WriteCmd(0XAE);  
}


//显示codetab.h中的ASCII字符
//xy：指定起始坐标
//ch[]：要显示的字符串
//TextSize：6*8/8*16可选
void OLED_ShowStr(u8 x,u8 y,u8 ch[],u8 TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	
	switch(TextSize)
	{
		//小字体：6x8
		case 1:
		{
			//结束符:\0
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				
				//行溢出-->换行
				if(x > 126)
				{
					x = 0;
					y++;
				}
				//设置起点
				OLED_SetPos(x,y);
				
				//画字符
				for(i=0;i<6;i++)
					OLED_WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		
		//大字体：8X16
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				
				OLED_SetPos(x,y);
				
				for(i=0;i<8;i++)
					OLED_WriteDat(F8X16[c*16+i]);
				
				OLED_SetPos(x,y+1);
				
				for(i=0;i<8;i++)
					OLED_WriteDat(F8X16[c*16+i+8]);
				
				x += 8;
				j++;
			}
		}break;
	}
}


//显示codetab.h中的汉字(16*16点阵)
//N：汉字在F16x16中的索引
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm = 0;
	unsigned int  adder = 32*N;
	
	OLED_SetPos(x,y);
	
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WriteDat(F16x16[adder]);
		adder += 1;
	}
	
	OLED_SetPos(x,y + 1);
	
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WriteDat(F16x16[adder]);
		adder += 1;
	}
}


//显示BMP位图
//x0,y0：起始点坐标(x0:0~127	y0:0~7)
//x1,y1：结束点坐标(x1:1~128	y1:1~8)
void OLED_DrawBMP(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

	if(y1%8==0)
		y = y1/8;
	else
		y = y1/8 + 1;
	
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
		for(x=x0;x<x1;x++)
			OLED_WriteDat(BMP[j++]);
	}
}


void oled_show(void)
{
	OLED_ON();
	
//	OLED_CLS();
	
	OLED_ShowStr(0,20,"EncoLEFT",1);
	
//	OLED_ShowStr(0,30,"EncoRIGHT",1);

//	//=============第三行显示编码器1=======================//	
//		                      OLED_ShowString(00,20,"EncoLEFT");
//		if( Encoder_Left<0)		OLED_ShowString(80,20,"-"),
//		                      OLED_ShowNumber(95,20,-Encoder_Left,3,12);
//		else                 	OLED_ShowString(80,20,"+"),
//		                      OLED_ShowNumber(95,20, Encoder_Left,3,12);
//  	//=============第四行显示编码器2=======================//		
//		                      OLED_ShowString(00,30,"EncoRIGHT");
//		if(Encoder_Right<0)		OLED_ShowString(80,30,"-"),
//		                      OLED_ShowNumber(95,30,-Encoder_Right,3,12);
//		else               		OLED_ShowString(80,30,"+"),
//		                      OLED_ShowNumber(95,30,Encoder_Right,3,12);	
//		//=============第五行显示电压=======================//
//		//=============第六行显示角度=======================//
//		                      OLED_ShowString(0,50,"Angle");
//		if(Angle_Balance<0)		OLED_ShowNumber(45,50,Angle_Balance+360,3,12);
//		else					        OLED_ShowNumber(45,50,Angle_Balance,3,12);
//		//=============刷新=======================//
//		OLED_Refresh_Gram();	
}




