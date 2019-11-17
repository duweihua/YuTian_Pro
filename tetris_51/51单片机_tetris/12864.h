//********************12864.h文件***************************
//******************************************************
#include<intrins.h>

sbit RS = P2^1;    // 并行的指令/数据选择信号 H:数据 L:命令      串行的片选信号
sbit RW = P2^3;    // 并行的读写选择选择信号 H:读模式 L：写模式          串行的数据口
sbit E  = P2^5;    // 并行的使能信号 H/L 
//sbit PSB= P2^1;   // 并行/串行选择位  H：并行 L：串行
sbit RST= P2^7;

#define  LcdData P0

unsigned char Lcd_CheckBusy(void)
{
    unsigned char Busy;
	LcdData=0xff;
    RS=0;
    RW=1;
    E=1;
    _nop_(); 
    Busy=LcdData&0x80;
    E=0;
    return Busy;
}

/*********************************
向LCD写入字节数据
**********************************/
void Lcd_WriteData(unsigned char Data)
{  
	while(Lcd_CheckBusy());
	RS=1;
	RW=0;
	E=0;
	_nop_();  
	_nop_(); 
	LcdData=Data;
	E=1;
	_nop_();
	_nop_();
	E=0;
}

/***********************************
从LCD中读出数据
************************************/
unsigned char Lcd_ReadData(void)
{
	unsigned char Temp;
	while(Lcd_CheckBusy());
 	LcdData=0xff;  //释放数据线
 	RS=1;		   //指明为读操作
	RW=1;
	E=1;
	_nop_();
   	Temp=LcdData;  //读取数据
   	E=0;
   	return Temp;
}

/*************************************
向LCD中写入指令代码
**************************************/
void Lcd_WriteCmd(unsigned char CmdCode)
{  
	while(Lcd_CheckBusy());
   	RS=0;
   	RW=0;
   	E=0;
   	_nop_();  
	_nop_();
   	LcdData=CmdCode;
   	_nop_(); 
	_nop_();
   	E=1;
   	_nop_();  
	_nop_();
   	E=0;
}

/*************************************
向LCD指定起始位置写入一个字符串
y为纵坐标（行） x为横坐标（列）
*************************************/
void Lcd_WriteStr(unsigned char x,unsigned char y,unsigned char *Str)
{
	if((y>3)||(x>7))
		return;//如果指定位置不在显示区域内，则不做任何写入直接返回
	EA=0;
	switch(y)
	{
		case 0:
				Lcd_WriteCmd(0x80+x);
				break;
		case 1:
				Lcd_WriteCmd(0x90+x);
				break;				
		case 2:
				Lcd_WriteCmd(0x88+x);
				break;
		case 3:
				Lcd_WriteCmd(0x98+x);
				break;
	}
	while(*Str>0)
	{  
		Lcd_WriteData(*Str);
  		Str++;     
	}
	EA=1;
}



/**************************************
为加速逻辑运算而设置的掩码表，这是以牺牲空间而换取时间的办法
***************************************/
code unsigned int LcdMaskTab[]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
							 	0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};

/***************************************
向LCD指定坐标写入一个象素,象素颜色有两种，0代表白（无显示），1代表黑（有显示）
****************************************/
void Lcd_PutPixel(unsigned char x,unsigned char y,unsigned char Color)
{						//为什么x的1表示16位，y的1表示1位？？？
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)  //像素超出边界 则不写入
		return;
	Color=Color%2;	   //值不变？
	w=15-x%16;//确定对这个字的第多少位进行操作
	x=x/16;	//以16位为一个操作单位
			//确定为一行上的第几字 光标定位在这个字之前

	if(y<32) //如果为上页
		z=0x80;
	else     //否则如果为下页
		z=0x88;

	y=y%32;	  //计算处于该页的第几行
	EA=0;
	Lcd_WriteCmd(0x36);
	Lcd_WriteCmd(y+0x80);  //行地址
	Lcd_WriteCmd(x+z);     //要操作单位的横坐标 
	Temp=Lcd_ReadData();   //先空读一次
	Temp=(unsigned int)Lcd_ReadData()<<8;//再读出高8位 因为readdata每次只读出8位数据
	Temp|=(unsigned int)Lcd_ReadData();  //再读出低8位
										 //读出要操作单位的16位数据
	EA=1;
	if(Color==1) //如果写入颜色为1
		Temp|=LcdMaskTab[w]; //在此处查表实现加速 只将temp的该位置1
	else         //如果写入颜色为0
		Temp&=~LcdMaskTab[w];//在此处查表实现加速 只将temp的该位置0
	EA=0;
	Lcd_WriteCmd(y+0x80);  //行地址
	Lcd_WriteCmd(x+z);     //列地址
    Lcd_WriteData(Temp>>8);		//先写入高8位
    Lcd_WriteData(Temp&0x00ff);	//再写入低8位
	Lcd_WriteCmd(0x30);
	EA=1;
}


/******************************************
从LCD指定坐标读取象素颜色值
*******************************************/
unsigned char Lcd_ReadPixel(unsigned char x,unsigned char y)
{
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)
		return 0;
	w=15-x%16;//确定对这个字的第多少位进行操作
	x=x/16;	  //确定为一行上的第几字

	if(y<32) //如果为上页
		z=0x80;
	else     //否则如果为下页
		z=0x88;

	y=y%32;
	EA=0;
	Lcd_WriteCmd(0x36);
	Lcd_WriteCmd(y+0x80);  //行地址
	Lcd_WriteCmd(x+z);     //列地址 
	Temp=Lcd_ReadData();//先空读一次
	Temp=(unsigned int)Lcd_ReadData()<<8;//再读出高8位
	Temp|=(unsigned int)Lcd_ReadData();  //再读出低8位
	EA=1;
	if((Temp&&LcdMaskTab[w])==0)
		return 0;
	else
		return 1;
}

/***************************************
向LCD指定位置画一条长度为Length的指定颜色的水平线
****************************************/
void Lcd_HoriLine(unsigned char x,unsigned char y,unsigned char Length,unsigned char Color)
{
	unsigned char i;
	if(Length==0)
		return;
	for(i=0;i<Length;i++)
	{
		Lcd_PutPixel(x+i,y,Color); //连续画点
	}
}

/***************************************
向LCD指定位置画一条长度为Length的指定颜色的垂直线
****************************************/
void Lcd_VertLine(unsigned char x,unsigned char y,unsigned char Length,unsigned char Color)
{
	unsigned char i;
	if(Length==0)
		return;
	for(i=0;i<Length;i++)
	{
		Lcd_PutPixel(x,y+i,Color);//连续画点
	}
}

/*******************************************
向LCD指定左上角坐标和右下角坐标画一个指定颜色的矩形
********************************************/
void Lcd_Rectangle(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char Color)
{
	unsigned char Temp;
	if(x0>x1)
	{
		Temp=x0;
		x0=x1;
		x1=Temp;
	}	
	if(y0>y1)
	{
		Temp=y0;
		y0=y1;
		y1=Temp;
	}
	Lcd_VertLine(x0,y0,y1-y0+1,Color);
	Lcd_VertLine(x1,y0,y1-y0+1,Color);
	Lcd_HoriLine(x0,y0,x1-x0+1,Color);
	Lcd_HoriLine(x0,y1,x1-x0+1,Color);	//四条线圈起来一个矩形
}

/*****************************************
清除Lcd全屏，如果清除模式Mode为0，则为全屏清除为颜色0（无任何显示）
否则为全屏清除为颜色1(全屏填充显示)
******************************************/
void Lcd_Clear(unsigned char Mode)
{
	unsigned char x,y,ii;
	unsigned char Temp;
	if(Mode%2==0)
		Temp=0x00;
	else
		Temp=0xff;
	Lcd_WriteCmd(0x36);//扩充指令 绘图显示
	for(ii=0;ii<9;ii+=8)   
		for(y=0;y<0x20;y++)     
			for(x=0;x<8;x++)
			{ 	
				EA=0;
				Lcd_WriteCmd(y+0x80);        //行地址
				Lcd_WriteCmd(x+0x80+ii);     //列地址     
				Lcd_WriteData(Temp); //写数据 D15－D8 
				Lcd_WriteData(Temp); //写数据 D7－D0 
				EA=1;
			}
	Lcd_WriteCmd(0x30);
}


void Lcd_Clearall(void)
{
	Lcd_WriteCmd(0x30);   
	Lcd_WriteCmd(0x01);

//    unsigned char i,j;
//
//    Lcd_WriteCmd(0x34);
//    Lcd_WriteCmd(0x36);
//
//
//    for(i=0;i<64;i++){
//	    if(i<32){
//		    Lcd_WriteCmd(0x80+i);
//		    Lcd_WriteCmd(0x80);
//	    }
//		else {
//		    Lcd_WriteCmd(0x80+(i-32));
//		    Lcd_WriteCmd(0x88);
//    	}
//
//    for(j=0;j<16;j++)
//            Lcd_WriteData(Mode);
//    }
}

/****************************************
LCD初始化
*****************************************/
void Lcd_Reset()
{  
//	PSB=1;
	RST=1;
	RST=0;
	_nop_();
	_nop_();					 
	RST=1;
	Lcd_WriteCmd(0x30);       //选择基本指令集
	Lcd_WriteCmd(0x06);       //指定在资料的读取及写入时，设定游标的移动方向及指定显示的移位
	Lcd_WriteCmd(0x01);       //清除显示，并且设定地址指针为00H
	Lcd_WriteCmd(0x0c);       
	Lcd_WriteCmd(0x02);		  //地址归零
}