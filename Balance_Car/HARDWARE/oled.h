#ifndef __OLED_I2C_H
#define	__OLED_I2C_H


#include "main.h"


//ͨ������0R���� 
//Һ�������л�0x78��0x7A������ַ
//Ĭ��0x78
#define OLED_ADDRESS		0x78 

#define OLED_CMD_ADDRESS	0X00
#define OLED_DAT_ADDRESS	0X40

#define OLED_GPIO_PORT_IIC	GPIOB						
#define OLED_RCC_IIC_PORT 	RCC_APB2Periph_GPIOB		
#define OLED_IIC_SCL_PIN	GPIO_Pin_10					
#define OLED_IIC_SDA_PIN	GPIO_Pin_11


//* Function List:

	//* 1. void oled_iic_init(void);								-- ��ʼ��Ӳ��IIC2
	//* 2. void OLED_IIC_WriteByte(u8 addr,u8 data);				-- IIC2����д����
	//* 3. void OLED_WriteCmd(unsigned char I2C_Command);			-- IICд�����
	//* 4. void OLED_WriteDat(unsigned char I2C_Data);				-- IICд���ݺ���
	//* 5. void oled_init(void) 									-- OLED����ʼ��	
	//* 6. void OLED_SetPos(unsigned char x, unsigned char y) 		-- ������ʼ������
	//* 7. void OLED_Fill(unsigned char fill_Data) 					-- ȫ�����
	//* 8. void OLED_CLS(void) 										-- ����
	//* 9. void OLED_ON(void) 										-- ����
	//* 10.void OLED_OFF(void) 										-- ˯��
	//* 11.void OLED_ShowStr(u16 x,u16 y,u16 ch[],u16 TextSize) 	-- ��ʾ�ַ���(�����С��6*8��8*16����)
	//* 12.void OLED_ShowCN(u16 x,u16 y,u16 N) 						-- ��ʾ����(������Ҫ��ȡģ��Ȼ��ŵ�codetab.h��)
	//* 13.void OLED_DrawBMP(u16 x0,u16 y0,u16 x1,u16 y1,u16 BMP[]) -- BMPͼƬ
	//* 14.void oled_show(void);									-- ʵ�������е�Һ����ʾ����

void oled_iic_init(void);
void OLED_IIC_WriteByte(u8 addr,u8 data);
void OLED_WriteCmd(unsigned char I2C_Command);
void OLED_WriteDat(unsigned char I2C_Data);
void oled_init(void);
void OLED_SetPos(unsigned char x,unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x,unsigned char y,unsigned char ch[],unsigned char TextSize);
void OLED_ShowCN(unsigned char x,unsigned char y,unsigned char N);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void oled_show(void);


#endif


