#ifndef __OLED_I2C_H
#define	__OLED_I2C_H


#include "main.h"


//通过调整0R电阻 
//液晶可以切换0x78和0x7A两个地址
//默认0x78
#define OLED_ADDRESS		0x78 

#define OLED_CMD_ADDRESS	0X00
#define OLED_DAT_ADDRESS	0X40

#define OLED_GPIO_PORT_IIC	GPIOB						
#define OLED_RCC_IIC_PORT 	RCC_APB2Periph_GPIOB		
#define OLED_IIC_SCL_PIN	GPIO_Pin_10					
#define OLED_IIC_SDA_PIN	GPIO_Pin_11


//* Function List:

	//* 1. void oled_iic_init(void);								-- 初始化硬件IIC2
	//* 2. void OLED_IIC_WriteByte(u8 addr,u8 data);				-- IIC2基础写函数
	//* 3. void OLED_WriteCmd(unsigned char I2C_Command);			-- IIC写命令函数
	//* 4. void OLED_WriteDat(unsigned char I2C_Data);				-- IIC写数据函数
	//* 5. void oled_init(void) 									-- OLED屏初始化	
	//* 6. void OLED_SetPos(unsigned char x, unsigned char y) 		-- 设置起始点坐标
	//* 7. void OLED_Fill(unsigned char fill_Data) 					-- 全屏填充
	//* 8. void OLED_CLS(void) 										-- 清屏
	//* 9. void OLED_ON(void) 										-- 唤醒
	//* 10.void OLED_OFF(void) 										-- 睡眠
	//* 11.void OLED_ShowStr(u16 x,u16 y,u16 ch[],u16 TextSize) 	-- 显示字符串(字体大小有6*8和8*16两种)
	//* 12.void OLED_ShowCN(u16 x,u16 y,u16 N) 						-- 显示中文(中文需要先取模，然后放到codetab.h中)
	//* 13.void OLED_DrawBMP(u16 x0,u16 y0,u16 x1,u16 y1,u16 BMP[]) -- BMP图片
	//* 14.void oled_show(void);									-- 实际运行中的液晶显示函数

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


