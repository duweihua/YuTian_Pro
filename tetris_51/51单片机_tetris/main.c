#include <reg51.h>        
#include "12864.h"

static unsigned long Seed = 1;

#define A 48271L
#define M 2147483647L
#define Q (M / A)//44488
#define R (M % A)//3399

sbit K1=P3^0;	 //��
sbit K2=P3^1;	 //��
sbit K3=P3^2;	 //��
sbit K4=P3^3;	 //��

//���Ʊ߿������
//��Ϸ��������
unsigned int idata num[19+2]=
{
0xfff,					//��1�У�������
0x801,0x801,0x801,0x801,
0x801,0x801,0x801,0x801,
0x801,0x801,0x801,0x801,
0x801,0x801,0x801,0x801,
0x801,0x801,0x801,		//��2�е���20�й�19��
0xfff					//��21�У�������
};

//���干21�У�����num[0]Ϊ��ǽ���У�num[20]Ϊ��ǽ����,ÿ��12������һ��Ϊ��ǽ���У�����һ��Ϊ��ǽ����
/*
			0 1	2 3	4 5	6 7 8 9	10 11
        0   �� �� �� �� �� �� �� �� �� �� �� ��
		1   �� �� �� �� �� �� �� �� �� �� �� ��
		2   �� �� �� �� �� �� �� �� �� �� �� ��
		3   �� �� �� �� �� �� �� �� �� �� �� ��
		4   �� �� �� �� �� �� �� �� �� �� �� ��
		5   �� �� �� �� �� �� �� �� �� �� �� ��
		6   �� �� �� �� �� �� �� �� �� �� �� ��
		7   �� �� �� �� �� �� �� �� �� �� �� ��
		8   �� �� �� �� �� �� �� �� �� �� �� ��		   
		9   �� �� �� �� �� �� �� �� �� �� �� ��
	    10  �� �� �� �� �� �� �� �� �� �� �� ��
	    11  �� �� �� �� �� �� �� �� �� �� �� ��
	    12  �� �� �� �� �� �� �� �� �� �� �� ��
 	    13	�� �� �� �� �� �� �� �� �� �� �� ��
        14	�� �� �� �� �� �� �� �� �� �� �� ��
	    15	�� �� �� �� �� �� �� �� �� �� �� ��
	    16	�� �� �� �� �� �� �� �� �� �� �� ��
	    17	�� �� �� �� �� �� �� �� �� �� �� ��
	    18	�� �� �� �� �� �� �� �� �� �� �� ��
	    19	�� �� �� �� �� �� �� �� �� �� �� ��
	    20	�� �� �� �� �� �� �� �� �� �� �� ��


*/

//��ά���鶨��������״�����з���
//��4*4�ķ���Ϊһ����Ԫ����������״ת��Ϊ����
unsigned char code Block[28][2]={
/*
*   ��     	  �ڿڿ�   	   �ڿ�
*   ��        ��             ��           ��
*   �ڿ�    	             ��  	  �ڿڿ�      
*/
{0x88,0xc0},{0xe8,0x00},{0x62,0x20},{0x02,0xe0},
/*
*     ��                    �ڿ�      �ڿڿ�
*     ��      ��            ��            ��
*   �ڿ�      �ڿڿ�        ��    
*/
{0x22,0x60},{0x08,0xe0},{0xc8,0x80},{0xe2,0x00},  
/*
*   ��
*   �ڿ�         �ڿ�
*     ��       �ڿ�
*/
{0x8c,0x40},{0x6c,0x00},{0x8c,0x40},{0x6c,0x00},
/*
*     ��       �ڿ�
*   �ڿ�         �ڿ�
*   ��
*/
{0x4c,0x80},{0xc6,0x00},{0x4c,0x80},{0xc6,0x00},  
/*
*               ��                       ��
*    ��         �ڿ�       �ڿڿ�      �ڿ�
*  �ڿڿ�       ��           ��          ��
*/
{0x04,0xe0},{0x8c,0x80},{0xe4,0x00},{0x26,0x20},
/*   ��
*    ��
*    ��       �ڿڿڿ�
*    ��
*/
{0x44,0x44},{0x0f,0x00},{0x44,0x44},{0x0f,0x00},  
/*
*   �ڿ�
*   �ڿ�
*/  
{0x06,0x60},{0x06,0x60},{0x06,0x60},{0x06,0x60}
};

#define PASSSCORE 2


struct Jimu
{
	unsigned int dat;
 	char x;
	unsigned char y;
	unsigned char type;
	unsigned char change;
}Sign[3];//��ľ�ṹ��

unsigned char SysFlag=0;

#define NEWSIGNFLAG 0
#define DEADFLAG 1
#define PAUSEFLAG 2

unsigned char Score=0;
unsigned char Level=1;
unsigned char DelayCnt=5;
unsigned char upflag=0;


/*********************************************************/
#define N 25
/************************************
α�����������  
����һ����������� �����µĻ�ľ
*************************************/
double Random(void)
{
	long TmpSeed;
	TmpSeed=A*(Seed%Q)-R*(Seed/Q);
	if(TmpSeed>=0)
		Seed=TmpSeed;
	else
		Seed=TmpSeed+M ;
	return (double)Seed/M;
}


/**************************************
Ϊα���������������
***************************************/
void InitRandom(unsigned long InitVal)
{
	Seed=InitVal;
}


//��ʱ�ӳ���
void Delay(unsigned int t)
{  
	unsigned int i,j;
	for(i=0;i<t;i++)
		for(j=0;j<10;j++);    
}


/*********************************
��ʼ��MPU ������
**********************************/
void InitCpu(void)
{
 	TMOD=0x0;
	TH0=0;
	TL0=0;
	TR0=1;
	ET0=1;		
	EX1=1;
	EA=1;
    TCON|=0x04;
}



/******************************��ǽ�ڣ���ʼ������*******************************/
void DrawBoard(void)
{
	unsigned char n;
	for(n=0;n<12;n++)
	{
		Lcd_Rectangle(3*n,0,3*n+2,2,1);	  //���±߿�
		Lcd_Rectangle(3*n,60,3*n+2,62,1); //���ϱ߿�
	}
	for(n=0;n<20;n++)
	{
		Lcd_Rectangle(0,3*n,2,3*n+2,1);	  //����߿�
		Lcd_Rectangle(33,3*n,35,3*n+2,1); //���ұ߿�
	}
	Lcd_WriteStr(4,0,"WELCOME");  //��3�е�0��
	Lcd_WriteStr(3,2,"Score:");	   //��3�е�2��
	Lcd_WriteStr(3,3,"Level:");	   //��3�е�3��
}


/***********************************
��Ϸ��������
************************************/
void GameOver(void)
{
	unsigned char Str[3];

	Str[0]=(Score/10)|0x30;	  //��0x30�ǽ�����ת��Ϊascii��
	Str[1]=(Score%10)|0x30;	  
	Str[2]=0;

	Lcd_Clearall();

	if((SysFlag&(1<<DEADFLAG))!=0)
	{
		Lcd_WriteStr(2,2,"Score:");	
		Lcd_WriteStr(5,2,Str);
		Lcd_WriteStr(2,1,"You Fail");
	}
	else
	{
		Lcd_WriteStr(2,2,"Score:");	
		Lcd_WriteStr(5,2,Str);
		Lcd_WriteStr(2,1,"You Pass");
	}
}


unsigned int code MaskTab[16]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
							   0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};


/**********************************
���ݻ�ľͼ����������X��Y��������ľͼ��
***********************************/
void DrawSign(struct Jimu Temp,unsigned char DrawMode)
{
	unsigned char m,n;
	for(m=0;m<4;m++)
		for(n=0;n<4;n++)	//4��4��Ϊһ����Ԫ
		{
			if((Temp.dat&MaskTab[4*m+n])!=0)  //��λ��Ӧ�û�һ��С����
				Lcd_Rectangle(Temp.x+n*3,Temp.y-2-3*m,Temp.x+n*3+2,Temp.y-3*m,DrawMode);
		}
}


/********************************
����ľͼ��ֵ����num������
Ҳ���ѻ�ľͼ��̶����޷����½�
Ҳ���ǰѵ�λ�Ļ�ľ��ɱ���
*********************************/
void FixSign(void)
{
	unsigned char m,n;
	for(m=0;m<4;m++)//��ѭ��
		for(n=0;n<4;n++)//��ѭ��
		{
			if((Sign[0].dat&MaskTab[4*m+n])!=0)	 //�����ľ��ĳһλ����ʵ��
			{
				num[20-(Sign[0].y-2)/3+m]|=MaskTab[11-Sign[0].x/3-n]; //���ڱ�������Ӧλ�û�ʵ��
						//����3����Ϊ��С�ķ�����3*3������	
						//��������ÿ����Ԫ�����½���Ϊ��׼
						//�����½Ƕ�Ӧm���n��С��������ʽ+m/-n
			}
		}
}

/********************************
�жϻ�ľͼ���з����Ƿ����ϰ������غ�
*********************************/
unsigned char CheckIf(void)
{
	unsigned char m,n;
	for(m=0;m<4;m++)//��ѭ��
		for(n=0;n<4;n++)//��ѭ��
		{
			if((Sign[1].dat & MaskTab[4*m+n])!=0)//��ľ��ĳһλ����ʵ��
			{
				if((num[20-(Sign[1].y-2)/3+m] & MaskTab[11-Sign[1].x/3-n])!=0)//�������Ļ�ľ����ǰ�ı������غ���
					return 0;
			}
		}
	return 1;
}

/********************************
�жϻ�ľͼ���Ƿ���Լ����½�һ��
********************************/
unsigned char CheckIfDown(void)
{
	Sign[1]=Sign[0];//
	Sign[1].y+=3;//�����½�һ��	
	return CheckIf();
	
}

/********************************
�жϻ�ľͼ���Ƿ���������ƶ�
*********************************/
unsigned char CheckIfLeft(void)
{
	Sign[1]=Sign[0];
	Sign[1].x-=3;	
	return CheckIf();
}

/********************************
�жϻ�ľͼ���Ƿ���������ƶ�
*********************************/
unsigned char CheckIfRight(void)
{
	Sign[1]=Sign[0];
	Sign[1].x+=3;	
	return CheckIf();
}

/********************************
�ж��Ƿ������ת
*********************************/
unsigned char CheckIfRoll(void)
{
	unsigned char i;
	unsigned int Temp;
	Sign[1]=Sign[0];
	if(++Sign[1].change>3)	//ÿ����ľ�����ת4�ξͻḴԭ			  
		Sign[1].change=0; 	//����change=4�����
   	i=Sign[1].type*4+Sign[1].change;//ÿ�ֻ�ľռ�ĸ�һά����
									//����change��ֵ������ǰ���ĸ�����
	Temp=(unsigned int)Block[i][0]<<8;	
	Temp=Temp|Block[i][1];
	Sign[1].dat=Temp;	 //������ת
	
	return CheckIf();	 //�ټ���Ƿ����
}

/********************************
Ѱ��������в�����������
���Ѱ��4�����в�������
*********************************/
void DelFull(void)
{
	unsigned char m,n;
	unsigned char Temp;
	unsigned char Flag=0;
	Temp=(Sign[0].y-2)/3;
	if(Temp>=20)//��ֹԽ�����±߽�
		Temp=1;
	else
		Temp=20-Temp; //����������ǵ�һ��
	for(n=Temp+3;n>=Temp;n--)//��ľͼ�����п�ʼѰ�����бȽ�����������
	{
		if(num[n]==0xfff)	 //�ҵ�����
		{
			Flag=1;			 //��־λ��1
			for(m=n+1;m<=19;m++) //��ȥ��Χ�߿�
			{
				num[m-1]=num[m];//����һ�еķ����Ƶ������һ��
			}
			num[m]=0x801;	//�����һ��ֻ������߿�
			Score++;//ÿ�ҵ�һ�����У��������1
		}
	}
	if(Flag)//���������б仯���ػ���ľ����
	{
		for(m=Temp;m<=19;m++)//����Ҫ�ص�һ���ػ���ֻ��Ҫ�ӻ�ľͼ�������п�ʼ���ϵ��ػ�
			for(n=1;n<=10;n++)
			{
				if((num[m]&MaskTab[n])==0)//���ĳ��Ӧ��Ϊ0 ��ȡ���ز�Ϊ0 ����ȥ����ľ���
				{	
					if(Lcd_ReadPixel(30-(n-1)*3,57-(m-1)*3)!=0)//������
					{
						Lcd_Rectangle(30-(n-1)*3,57-(m-1)*3,30-(n-1)*3+2,57-(m-1)*3+2,0);
					}
				}
			   	else  //�����������ط���Ӧ�õ���0 ��ȡ����ȴ����0 �������ﻭ����
				{
					if(Lcd_ReadPixel(30-(n-1)*3,57-(m-1)*3)==0)//������
					{
						Lcd_Rectangle(30-(n-1)*3,57-(m-1)*3,30-(n-1)*3+2,57-(m-1)*3+2,1);
					}
				}
			}
	}
}


/*******************************
�������һ����ľͼ��ŵ�Ԥ����������ʾ����
********************************/
void CreatSign(void)
{
	unsigned char n;
	unsigned int Temp;

	DrawSign(Sign[2],0);//�����

	n=Random()*28;
	Temp=(unsigned int)Block[n][0]<<8;
	Temp=Temp|Block[n][1];
	Sign[2].dat=Temp;
	Sign[2].x=45;
	Sign[2].y=4*3+2;
	Sign[2].type=n/4;
	Sign[2].change=n%4;
	DrawSign(Sign[2],1);//�󻭳�
}


//��ʾ��ǰ����
void PrintScore(void)
{
	unsigned char Str[3];
//	Str[0]=3|0x30;
//	Str[1]=3|0x30;
//	Str[2]=3|0x30;
	Str[0]=(Score/10)|0x30;	  //��0x30�ǽ�����ת��Ϊascii��
	Str[1]=(Score%10)|0x30;	  
	Str[2]=0;				  //�ո��Ӧ��ascii��Ϊ0
	Lcd_WriteStr(6,2,Str);
}


//��ʾ��ǰ�ȼ�
void PrintLevel(void)
{
	unsigned char Str[3];
//	Str[0]=(Level/10);
//	Str[1]=(Level%10);
	Str[0]=(Level/10)|0x30;
	Str[1]=(Level%10)|0x30;
	Str[2]=0;
	Lcd_WriteStr(6,3,Str);
}

/********************************
��Ϸ�ľ�����̣�Ҳ�Ƕ���˹�����㷨�Ĺؼ�����
*********************************/
void GamePlay(void)
{
	unsigned char m,n; 
	unsigned int Temp;
	SysFlag|=1<<NEWSIGNFLAG;//�տ�ʼ��ʼ��Ϊ��Ҫ�����µĻ�ľͼ��	  sysflag=1
	InitRandom(TL0);	//�����������TL0Ϊ��ʱ��0�ĵ�8λ����Ϊ����������ӣ�
	Lcd_WriteStr(3,1,"Playing");
	PrintScore();
	PrintLevel();
	CreatSign();
	while(1)
	{	
		if((SysFlag&(1<<NEWSIGNFLAG))==1)//�ж��Ƿ���Ҫ�����µĻ�ľͼ��
		{
			SysFlag&=~(1<<NEWSIGNFLAG);			//sysflag=0
			Sign[0]=Sign[2];  	//��Ҫ���ֵĻ�ľ����Ԥ�����Ļ�ľ
			CreatSign();   		//��Ԥ��������ʾ��һ����ľ
			Sign[0].x=12;		//Ϊ�ղ����Ļ�ľ��������
			Sign[0].y=14;

			for(m=0;m<4;m++)//��ѭ��
			{
				for(n=0;n<4;n++)//��ѭ��
				{
					unsigned int what=Sign[0].dat&MaskTab[15-m*4-n];//�����λ
					if(what!=0)	  //ֱ���������ʽ�жϲ��ɹ�����ֵ��unsigned int�ͺ���
					{
						upflag=1;
						break;
				}	}
				if(n==4)
					Sign[0].y-=3;  //��������п�������һ����λ
				if(upflag)		   //��ֹ����Ŀ���Ҳ��������
				{
					break;
					upflag=1;
				}
			}//����ľͼ������ö�
			
			for(m=0;m<4;m++)//��ѭ��
				for(n=0;n<4;n++)//��ѭ��
				{
					if((Sign[0].dat&MaskTab[4*m+n])!=0)	//�»�ľ��ĳ�������з���
					{
						if((num[20-(Sign[0].y-2)/3+m]&MaskTab[11-Sign[0].x/3-n])!=0)//�ұ�������Ӧ����Ҳ�з���
							SysFlag|=1<<DEADFLAG;		 //����Ϸʧ��
					}
				}
			if((SysFlag&(1<<DEADFLAG))!=0)
				break;//��������µĻ�ľͼ���еķ������ѹ̶��õķ����غϣ�����������Ϸ����
			DrawSign(Sign[0],1);	
		}
		if((CheckIfLeft())&&(K3==0))	 //��
		{
			DrawSign(Sign[0],0);
			Sign[0].x-=3;
			DrawSign(Sign[0],1);
		}			  
		if((CheckIfRight())&&(K4==0))	  //��
		{
			DrawSign(Sign[0],0);
			Sign[0].x+=3;
			DrawSign(Sign[0],1);	
		}
		if((CheckIfDown())&&(K2==0))//��
		{
			DrawSign(Sign[0],0);
			Sign[0].y+=3;
			DrawSign(Sign[0],1);	
		} 
		if((CheckIfRoll())&&(K1==0)) //��ת
		{
			Delay(100);	 //ȥ��
			if((CheckIfRoll())&&(K1==0))
			{	
				while(K1==0)
				{
				 	Delay(5000);   //��ֹ��סup���൱����Ϸ��ͣ
					break;
				}
			  	DrawSign(Sign[0],0);
				if(++Sign[0].change>3)
				Sign[0].change=0;
		   		m=Sign[0].type*4+Sign[0].change;
				Temp=(unsigned int)Block[m][0]<<8;
				Temp=Temp|Block[m][1];
				Sign[0].dat=Temp;
				DrawSign(Sign[0],1);
			}
			
		}
//		if((SysFlag&(1<<PAUSEFLAG))!=0)
//			continue;
		Delay(500);
//		if(Level==1)
//		{
//			Delay(500);
//		}
//		else if(Level==2)
//		{
//		 	Delay(400);
//		}
//		else if(Level==3)
//		{
//		 	Delay(300);
//		}
//		else if(Level==4)
//		{
//		 	Delay(200);
//		}
//		else if(Level==5)
//		{
//		 	Delay(100);
//		}
//		else 
//			Delay(100);
		if(++DelayCnt>=2*(11-Level))//delay������ �ȼ�Խ����Ҫdelay����Խ�� 
		{
			DelayCnt=0;		 //��ʱ����������
			if(CheckIfDown())//�ж��Ƿ��ܼ����½�һ��
			{
				DrawSign(Sign[0],0);
				Sign[0].y+=3;
				DrawSign(Sign[0],1);  //ɾ�ɻ���
			}
			else
			{						  
				FixSign();		//�̶�
				DelFull();		//ɾ��
				PrintScore();	//��ʾ�ɼ�
				if(Score>=PASSSCORE*Level)
				{
					SysFlag&=~(1<<DEADFLAG);
					Level++;
					PrintLevel();
					if(Level>5)
					{
						break;
					}	
				//	break;//��������Ϸ����
				}
				SysFlag|=1<<NEWSIGNFLAG;//�µĻ�ľͼ�������־��1
			}
		}
	}
	
}

void main()
{  
//  	Lcd_Clearall();//����   
//Lcd_WriteStr(2,2,"Score:");	
	InitCpu();//��ʼ��CPU
	Lcd_Reset(); //��ʼ��LCD��
	Lcd_Clear(0);//����   	
	Lcd_Reset(); //��ʼ��LCD��
	Lcd_Clear(0);//����
	//Lcd_WriteStr(2,2,"Score:");	
	DrawBoard();//������

	GamePlay();//����Ϸ		
	GameOver();//��Ϸ����
	while(1);
//	Lcd_Reset(); //��ʼ��LCD��
//	Lcd_Clear(0);//����		
	
}
