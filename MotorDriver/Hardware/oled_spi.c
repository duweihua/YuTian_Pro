//#include "main.h"
//#include "oledfont.h"


////OLED分辨率：128*64

////OLED显存存放格式:
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


////更新显存	 
//void OLED_Refresh_Gram(void)
//{
//	u8 i,n;		    
//	for(i=0;i<8;i++)  
//	{
//		//设置页地址(0~7)
//		//8个最小行单位构成页
//		OLED_WR_Byte (0xb0+i,OLED_CMD);  
//		//设置显示地址 
//		//为了更新全屏 此处设置地址为0
//		//列低地址(第4位)
//		OLED_WR_Byte (0x00,OLED_CMD);  
//		//列高地址(高4位) 
//		OLED_WR_Byte (0x10,OLED_CMD); 
//		for(n=0;n<131;n++)
//			OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
//	}
//}


////四线串行模式

////向SSD1306写入一个字节
////根据时序图配置电平实现
////dat:	要写入的数据/命令
////cmd:	数据/命令标志 
////			0：表示命令
////			1：表示数据
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
//		//最高位非零
//		//则依次拉高数据线时钟线
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
////开启OLED显示    
//void OLED_Display_On(void)
//{
//	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
//	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
//	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
//}

////关闭OLED显示
//void OLED_Display_Off(void)
//{
//	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
//	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
//	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
//}		   	


////清屏函数
////清屏后 整个屏幕是黑色的 和没点亮一样	  
//void OLED_Clear(void)
//{  
//	u8 i,n;  
//	for(i=0;i<8;i++)
//		for(n=0;n<130;n++)
//			OLED_GRAM[n][i]=0X00;  
//	OLED_Refresh_Gram();
//}


////画点函数
////坐标：
////		x：0~127
////		y：0~63
////颜色：
////		t=1：填充 
////		t=0：清空				   
//void OLED_DrawPoint(u8 x,u8 y,u8 t)
//{
//	u8 pos 	= 0;
//	u8 bx 	= 0;
//	u8 temp = 0;
//	//超出范围
//	if(x>131||y>63)
//		return;
//	//一次操作8位
//	//要避免影响其他位
//	pos=7-y/8;
//	bx=y%8;
//	temp=1<<(7-bx);
//	if(t)
//		OLED_GRAM[x][pos]|=temp;
//	else 
//		OLED_GRAM[x][pos]&=~temp;	    
//}


////填充区域
////对角坐标：
////			x1,y1,x2,y2
////注意确保：
////			x1<=x2
////			y1<=y2 
////			0<=x1<=127 
////			0<=y1<=63	 	 
////颜色dot:
////			0：清空
////			1：填充	  
//void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
//{
//	u8 x,y;  
//	//画点
//	for(x=x1;x<=x2;x++)
//		for(y=y1;y<=y2;y++)
//			OLED_DrawPoint(x,y,dot);	
//	//更新显示
//	OLED_Refresh_Gram();
//}


////在指定位置显示一个字符
////只包含部分字符
////坐标：
////		x:0~127
////		y:0~63
////mode:
////		0：反白显示
////		1：正常显示
////字体size:
////		 	16/12 
//void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
//{
//	u8 temp,t,t1;
//	u8 y0=y;
//	//字符表中第一个字符为' '
//	//做差得到偏移值
//	//有ASCII转换为字符表中地地址
//	chr=chr-' ';	   
//    for(t=0;t<size;t++)
//    {
//		//根据字体选择字符表
//		if(size==12)	//1206
//			temp=oled_asc2_1206[chr][t]; 
//		else 			//1608
//			temp=oled_asc2_1608[chr][t];
//		//根据字符表填充字符
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


////求幂函数 (m^n)
//u32 oled_pow(u8 m,u8 n)
//{
//	u32 result=1;	 
//	while(n--)
//		result*=m;    
//	return result;
//}


////显示数字
////起点坐标：x,y  
////数字长度：len 
////字体大小：size
////数值：num(0~4294967295)		  
//void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
//{         	
//	u8 t,temp;
//	u8 enshow=0;						   
//	for(t=0;t<len;t++)
//	{
//		//按位分解数字
//		temp=(num/oled_pow(10,len-t-1))%10;
//		if((enshow==0)&&(t<(len-1)))
//		{
//			//次位无数字 则显示空格
//			if(temp==0)
//			{
//				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
//				continue;
//			}
//			else 
//				enshow=1; 
//		}
//		//根据0的ASCII码将数字转化为ASCII码显示
//	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
//	}
//} 


////显示字符串
////起点坐标：x,y  
////字符串首地址：*p
////(默认使用16字体)
////以'\0'结尾
//void OLED_ShowString(u8 x,u8 y,const u8 *p)
//{
//	#define MAX_CHAR_POSX 126
//	#define MAX_CHAR_POSY 58   
//	//判断结尾标志字符
//    while(*p!='\0')
//    {
//		//列溢出则转行显示
//        if(x>MAX_CHAR_POSX)
//		{
//			x=0;
//			y+=16;
//		}
//		//行溢出则清屏从头开始显示
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


////初始化SSD1306
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
//	//全部拉高
// 	GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4);	

//	OLED_RST_Clr();
//	delay_ms(100);
//	OLED_RST_Set();
//					 
//	//关闭显示
//	OLED_WR_Byte(0xAE,OLED_CMD); 
//	//设置时钟分频因子 震荡频率
//	//[3:0] 分频因子
//	//[7:4] 震荡频率
//	OLED_WR_Byte(0xD5,OLED_CMD); 
//	OLED_WR_Byte(80,OLED_CMD);
//	//设置驱动路数
//	//默认0X3F(1/64) 
//	OLED_WR_Byte(0xA8,OLED_CMD); 
//	OLED_WR_Byte(0X3F,OLED_CMD);
//	//设置显示偏移
//	//默认为0
//	OLED_WR_Byte(0xD3,OLED_CMD);
//	OLED_WR_Byte(0X00,OLED_CMD);
//	//设置显示开始行 [5:0] 行数.
//	OLED_WR_Byte(0x40,OLED_CMD);
//	//电荷泵设置								    
//	OLED_WR_Byte(0x8D,OLED_CMD); 
//	//bit2 开启/关闭
//	OLED_WR_Byte(0x14,OLED_CMD); 
//	//设置内存地址模式
//	//[1:0]		(默认10)
//	//		00：列地址模式
//	//		01：行地址模式
//	//		10：页地址模式
//	OLED_WR_Byte(0x20,OLED_CMD); 
//	OLED_WR_Byte(0x02,OLED_CMD);
//	//段重定义设置 
//	//bit0:	
//	//		0,0->0
//	//		1,0->127
//	OLED_WR_Byte(0xA1,OLED_CMD); 
//	//设置COM扫描方向
//	//bit3:
//	//		0：普通模式
//	//		1：重定义模式 
//	//COM[N-1]->COM0
//	//N：驱动路数
//	OLED_WR_Byte(0xC0,OLED_CMD); 
//	//设置COM硬件引脚配置
//	//[5:4]配置
//	OLED_WR_Byte(0xDA,OLED_CMD); 
//	OLED_WR_Byte(0x12,OLED_CMD); 
//	//对比度设置	 
//	OLED_WR_Byte(0x81,OLED_CMD); 
//	//亮度设置,越大越亮
//	//(1~255)
//	//默认0X7F
//	OLED_WR_Byte(0xEF,OLED_CMD);
//	//设置预充电周期
//	//[3:0]：PHASE 1
//	//[7:4]：PHASE 2
//	OLED_WR_Byte(0xD9,OLED_CMD);
//	OLED_WR_Byte(0xf1,OLED_CMD); 
//	//设置VCOMH 电压倍率
//	//[6:4]
//	//		000：0.65*vcc
//	//		001：0.77*vcc
//	//		011：0.83*vcc
//	OLED_WR_Byte(0xDB,OLED_CMD); 
//	OLED_WR_Byte(0x30,OLED_CMD); 
//	//全局显示开启
//	//bit0：
//	//		1：开启
//	//		0：关闭
//	//		(白屏/黑屏)
//	OLED_WR_Byte(0xA4,OLED_CMD); 
//	//设置显示方式
//	//bit0：
//	//		1：反相显示
//	//		0：正常显示	
//	OLED_WR_Byte(0xA6,OLED_CMD);   
//	//开启显示	
//	OLED_WR_Byte(0xAF,OLED_CMD);  
//	//清屏
//	OLED_Clear();
//}  















