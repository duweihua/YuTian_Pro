//#ifndef __OLED_H
//#define __OLED_H			  	 

//#include "main.h"


////-----------------OLED端口定义----------------  					   

////CS引脚已经接地
////#define OLED_CS_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_5)
////#define OLED_CS_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_5)


////四线驱动
//#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_0)
//#define OLED_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_0)

//#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_1)
//#define OLED_SDIN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_1)

//#define OLED_RST_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_3)
//#define OLED_RST_Set() GPIO_SetBits(GPIOB,GPIO_Pin_3)

////RS即D/C
//#define OLED_RS_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_4)
//#define OLED_RS_Set() GPIO_SetBits(GPIOB,GPIO_Pin_4)


//#define OLED_CMD  0	//写命令
//#define OLED_DATA 1	//写数据


//void OLED_WR_Byte(u8 dat,u8 cmd);	    
//void OLED_Display_On(void);
//void OLED_Display_Off(void);
//void OLED_Refresh_Gram(void);		   

//void OLED_Init(void);
//void OLED_Clear(void);
//void OLED_DrawPoint(u8 x,u8 y,u8 t);
//void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
//void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
//void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
//void OLED_ShowString(u8 x,u8 y,const u8 *p); 

//#endif  
//	 



