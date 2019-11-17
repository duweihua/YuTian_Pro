//********************12864.h�ļ�***************************
//******************************************************
#include<intrins.h>

sbit RS = P2^1;    // ���е�ָ��/����ѡ���ź� H:���� L:����      ���е�Ƭѡ�ź�
sbit RW = P2^3;    // ���еĶ�дѡ��ѡ���ź� H:��ģʽ L��дģʽ          ���е����ݿ�
sbit E  = P2^5;    // ���е�ʹ���ź� H/L 
//sbit PSB= P2^1;   // ����/����ѡ��λ  H������ L������
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
��LCDд���ֽ�����
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
��LCD�ж�������
************************************/
unsigned char Lcd_ReadData(void)
{
	unsigned char Temp;
	while(Lcd_CheckBusy());
 	LcdData=0xff;  //�ͷ�������
 	RS=1;		   //ָ��Ϊ������
	RW=1;
	E=1;
	_nop_();
   	Temp=LcdData;  //��ȡ����
   	E=0;
   	return Temp;
}

/*************************************
��LCD��д��ָ�����
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
��LCDָ����ʼλ��д��һ���ַ���
yΪ�����꣨�У� xΪ�����꣨�У�
*************************************/
void Lcd_WriteStr(unsigned char x,unsigned char y,unsigned char *Str)
{
	if((y>3)||(x>7))
		return;//���ָ��λ�ò�����ʾ�����ڣ������κ�д��ֱ�ӷ���
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
Ϊ�����߼���������õ�����������������ռ����ȡʱ��İ취
***************************************/
code unsigned int LcdMaskTab[]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
							 	0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};

/***************************************
��LCDָ������д��һ������,������ɫ�����֣�0����ף�����ʾ����1����ڣ�����ʾ��
****************************************/
void Lcd_PutPixel(unsigned char x,unsigned char y,unsigned char Color)
{						//Ϊʲôx��1��ʾ16λ��y��1��ʾ1λ������
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)  //���س����߽� ��д��
		return;
	Color=Color%2;	   //ֵ���䣿
	w=15-x%16;//ȷ��������ֵĵڶ���λ���в���
	x=x/16;	//��16λΪһ��������λ
			//ȷ��Ϊһ���ϵĵڼ��� ��궨λ�������֮ǰ

	if(y<32) //���Ϊ��ҳ
		z=0x80;
	else     //�������Ϊ��ҳ
		z=0x88;

	y=y%32;	  //���㴦�ڸ�ҳ�ĵڼ���
	EA=0;
	Lcd_WriteCmd(0x36);
	Lcd_WriteCmd(y+0x80);  //�е�ַ
	Lcd_WriteCmd(x+z);     //Ҫ������λ�ĺ����� 
	Temp=Lcd_ReadData();   //�ȿն�һ��
	Temp=(unsigned int)Lcd_ReadData()<<8;//�ٶ�����8λ ��Ϊreaddataÿ��ֻ����8λ����
	Temp|=(unsigned int)Lcd_ReadData();  //�ٶ�����8λ
										 //����Ҫ������λ��16λ����
	EA=1;
	if(Color==1) //���д����ɫΪ1
		Temp|=LcdMaskTab[w]; //�ڴ˴����ʵ�ּ��� ֻ��temp�ĸ�λ��1
	else         //���д����ɫΪ0
		Temp&=~LcdMaskTab[w];//�ڴ˴����ʵ�ּ��� ֻ��temp�ĸ�λ��0
	EA=0;
	Lcd_WriteCmd(y+0x80);  //�е�ַ
	Lcd_WriteCmd(x+z);     //�е�ַ
    Lcd_WriteData(Temp>>8);		//��д���8λ
    Lcd_WriteData(Temp&0x00ff);	//��д���8λ
	Lcd_WriteCmd(0x30);
	EA=1;
}


/******************************************
��LCDָ�������ȡ������ɫֵ
*******************************************/
unsigned char Lcd_ReadPixel(unsigned char x,unsigned char y)
{
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)
		return 0;
	w=15-x%16;//ȷ��������ֵĵڶ���λ���в���
	x=x/16;	  //ȷ��Ϊһ���ϵĵڼ���

	if(y<32) //���Ϊ��ҳ
		z=0x80;
	else     //�������Ϊ��ҳ
		z=0x88;

	y=y%32;
	EA=0;
	Lcd_WriteCmd(0x36);
	Lcd_WriteCmd(y+0x80);  //�е�ַ
	Lcd_WriteCmd(x+z);     //�е�ַ 
	Temp=Lcd_ReadData();//�ȿն�һ��
	Temp=(unsigned int)Lcd_ReadData()<<8;//�ٶ�����8λ
	Temp|=(unsigned int)Lcd_ReadData();  //�ٶ�����8λ
	EA=1;
	if((Temp&&LcdMaskTab[w])==0)
		return 0;
	else
		return 1;
}

/***************************************
��LCDָ��λ�û�һ������ΪLength��ָ����ɫ��ˮƽ��
****************************************/
void Lcd_HoriLine(unsigned char x,unsigned char y,unsigned char Length,unsigned char Color)
{
	unsigned char i;
	if(Length==0)
		return;
	for(i=0;i<Length;i++)
	{
		Lcd_PutPixel(x+i,y,Color); //��������
	}
}

/***************************************
��LCDָ��λ�û�һ������ΪLength��ָ����ɫ�Ĵ�ֱ��
****************************************/
void Lcd_VertLine(unsigned char x,unsigned char y,unsigned char Length,unsigned char Color)
{
	unsigned char i;
	if(Length==0)
		return;
	for(i=0;i<Length;i++)
	{
		Lcd_PutPixel(x,y+i,Color);//��������
	}
}

/*******************************************
��LCDָ�����Ͻ���������½����껭һ��ָ����ɫ�ľ���
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
	Lcd_HoriLine(x0,y1,x1-x0+1,Color);	//������Ȧ����һ������
}

/*****************************************
���Lcdȫ����������ģʽModeΪ0����Ϊȫ�����Ϊ��ɫ0�����κ���ʾ��
����Ϊȫ�����Ϊ��ɫ1(ȫ�������ʾ)
******************************************/
void Lcd_Clear(unsigned char Mode)
{
	unsigned char x,y,ii;
	unsigned char Temp;
	if(Mode%2==0)
		Temp=0x00;
	else
		Temp=0xff;
	Lcd_WriteCmd(0x36);//����ָ�� ��ͼ��ʾ
	for(ii=0;ii<9;ii+=8)   
		for(y=0;y<0x20;y++)     
			for(x=0;x<8;x++)
			{ 	
				EA=0;
				Lcd_WriteCmd(y+0x80);        //�е�ַ
				Lcd_WriteCmd(x+0x80+ii);     //�е�ַ     
				Lcd_WriteData(Temp); //д���� D15��D8 
				Lcd_WriteData(Temp); //д���� D7��D0 
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
LCD��ʼ��
*****************************************/
void Lcd_Reset()
{  
//	PSB=1;
	RST=1;
	RST=0;
	_nop_();
	_nop_();					 
	RST=1;
	Lcd_WriteCmd(0x30);       //ѡ�����ָ�
	Lcd_WriteCmd(0x06);       //ָ�������ϵĶ�ȡ��д��ʱ���趨�α���ƶ�����ָ����ʾ����λ
	Lcd_WriteCmd(0x01);       //�����ʾ�������趨��ַָ��Ϊ00H
	Lcd_WriteCmd(0x0c);       
	Lcd_WriteCmd(0x02);		  //��ַ����
}