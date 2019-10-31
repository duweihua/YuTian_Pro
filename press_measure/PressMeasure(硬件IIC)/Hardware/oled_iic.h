
#ifndef __OLED_I2C_H
#define	__OLED_I2C_H

#include "stm32f10x.h"

#define OLED_ADDRESS	0x78 //ͨ������0R����,������0x78��0x7A������ַ -- Ĭ��0x78

//* Function List:

	//* 1. void I2C_Configuration(void) 							-- ����CPU��Ӳ��I2C
	//* 2. void I2C_WriteByte(uint8_t addr,uint8_t data) 			-- ��Ĵ�����ַдһ��byte������
	//* 3. void WriteCmd(unsigned char I2C_Command) 				-- д����
	//* 4. void WriteDat(unsigned char I2C_Data) 					-- д����
	//* 5. void OLED_Init(void) 									-- OLED����ʼ��	
	//* 6. void OLED_SetPos(unsigned char x, unsigned char y) 		-- ������ʼ������
	//* 7. void OLED_Fill(unsigned char fill_Data) 					-- ȫ�����
	//* 8. void OLED_CLS(void) 										-- ����
	//* 9. void OLED_ON(void) 										-- ����
	//* 10.void OLED_OFF(void) 										-- ˯��
	//* 11.void OLED_ShowStr(u16 x,u16 y,u16 ch[],u16 TextSize) 	-- ��ʾ�ַ���(�����С��6*8��8*16����)
	//* 12.void OLED_ShowCN(u16 x,u16 y,u16 N) 						-- ��ʾ����(������Ҫ��ȡģ��Ȼ��ŵ�codetab.h��)
	//* 13.void OLED_DrawBMP(u16 x0,u16 y0,u16 x1,u16 y1,u16 BMP[]) -- BMPͼƬ

void I2C_Configuration(void);
void I2C_WriteByte(uint8_t addr,uint8_t data);
void WriteCmd(unsigned char I2C_Command);
void WriteDat(unsigned char I2C_Data);
void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);

#endif


