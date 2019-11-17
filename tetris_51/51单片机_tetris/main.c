#include <reg51.h>        
#include "12864.h"

static unsigned long Seed = 1;

#define A 48271L
#define M 2147483647L
#define Q (M / A)//44488
#define R (M % A)//3399

sbit K1=P3^0;	 //上
sbit K2=P3^1;	 //下
sbit K3=P3^2;	 //左
sbit K4=P3^3;	 //右

//绘制边框的数组
//游戏界面数组
unsigned int idata num[19+2]=
{
0xfff,					//第1行，最下面
0x801,0x801,0x801,0x801,
0x801,0x801,0x801,0x801,
0x801,0x801,0x801,0x801,
0x801,0x801,0x801,0x801,
0x801,0x801,0x801,		//第2行到第20行共19行
0xfff					//第21行，最上面
};

//定义共21行，其中num[0]为下墙壁行，num[20]为上墙壁行,每行12格，最左一格为左墙壁列，最右一格为右墙壁列
/*
			0 1	2 3	4 5	6 7 8 9	10 11
        0   ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■
		1   ■ □ □ □ □ □ □ □ □ □ □ ■
		2   ■ □ □ □ □ □ □ □ □ □ □ ■
		3   ■ □ □ □ □ □ □ □ □ □ □ ■
		4   ■ □ □ □ □ □ □ □ □ □ □ ■
		5   ■ □ □ □ □ □ □ □ □ □ □ ■
		6   ■ □ □ □ □ □ □ □ □ □ □ ■
		7   ■ □ □ □ □ □ □ □ □ □ □ ■
		8   ■ □ □ □ □ □ □ □ □ □ □ ■		   
		9   ■ □ □ □ □ □ □ □ □ □ □ ■
	    10  ■ □ □ □ □ □ □ □ □ □ □ ■
	    11  ■ □ □ □ □ □ □ □ □ □ □ ■
	    12  ■ □ □ □ □ □ □ □ □ □ □ ■
 	    13	■ □ □ □ □ □ □ □ □ □ □ ■
        14	■ □ □ □ □ □ □ □ □ □ □ ■
	    15	■ □ □ □ □ □ □ □ □ □ □ ■
	    16	■ □ □ □ □ □ □ □ □ □ □ ■
	    17	■ □ □ □ □ □ □ □ □ □ □ ■
	    18	■ □ □ □ □ □ □ □ □ □ □ ■
	    19	■ □ □ □ □ □ □ □ □ □ □ ■
	    20	■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■


*/

//二维数组定义所有形状的所有方向
//以4*4的方块为一个单元，将各个形状转化为编码
unsigned char code Block[28][2]={
/*
*   口     	  口口口   	   口口
*   口        口             口           口
*   口口    	             口  	  口口口      
*/
{0x88,0xc0},{0xe8,0x00},{0x62,0x20},{0x02,0xe0},
/*
*     口                    口口      口口口
*     口      口            口            口
*   口口      口口口        口    
*/
{0x22,0x60},{0x08,0xe0},{0xc8,0x80},{0xe2,0x00},  
/*
*   口
*   口口         口口
*     口       口口
*/
{0x8c,0x40},{0x6c,0x00},{0x8c,0x40},{0x6c,0x00},
/*
*     口       口口
*   口口         口口
*   口
*/
{0x4c,0x80},{0xc6,0x00},{0x4c,0x80},{0xc6,0x00},  
/*
*               口                       口
*    口         口口       口口口      口口
*  口口口       口           口          口
*/
{0x04,0xe0},{0x8c,0x80},{0xe4,0x00},{0x26,0x20},
/*   口
*    口
*    口       口口口口
*    口
*/
{0x44,0x44},{0x0f,0x00},{0x44,0x44},{0x0f,0x00},  
/*
*   口口
*   口口
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
}Sign[3];//积木结构体

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
伪随机数发生器  
产生一个随机数用来 生成新的积木
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
为伪随机数发生器播种
***************************************/
void InitRandom(unsigned long InitVal)
{
	Seed=InitVal;
}


//延时子程序
void Delay(unsigned int t)
{  
	unsigned int i,j;
	for(i=0;i<t;i++)
		for(j=0;j<10;j++);    
}


/*********************************
初始化MPU ？？？
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



/******************************画墙壁，初始化界面*******************************/
void DrawBoard(void)
{
	unsigned char n;
	for(n=0;n<12;n++)
	{
		Lcd_Rectangle(3*n,0,3*n+2,2,1);	  //画下边框
		Lcd_Rectangle(3*n,60,3*n+2,62,1); //画上边框
	}
	for(n=0;n<20;n++)
	{
		Lcd_Rectangle(0,3*n,2,3*n+2,1);	  //画左边框
		Lcd_Rectangle(33,3*n,35,3*n+2,1); //画右边框
	}
	Lcd_WriteStr(4,0,"WELCOME");  //第3列第0行
	Lcd_WriteStr(3,2,"Score:");	   //第3列第2行
	Lcd_WriteStr(3,3,"Level:");	   //第3列第3行
}


/***********************************
游戏结束处理
************************************/
void GameOver(void)
{
	unsigned char Str[3];

	Str[0]=(Score/10)|0x30;	  //或0x30是将数字转换为ascii码
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
根据积木图标左下坐标X，Y来画出积木图标
***********************************/
void DrawSign(struct Jimu Temp,unsigned char DrawMode)
{
	unsigned char m,n;
	for(m=0;m<4;m++)
		for(n=0;n<4;n++)	//4行4列为一个单元
		{
			if((Temp.dat&MaskTab[4*m+n])!=0)  //此位置应该画一个小方块
				Lcd_Rectangle(Temp.x+n*3,Temp.y-2-3*m,Temp.x+n*3+2,Temp.y-3*m,DrawMode);
		}
}


/********************************
将积木图标值融入num数据中
也即把积木图标固定，无法再下降
也就是把到位的积木变成背景
*********************************/
void FixSign(void)
{
	unsigned char m,n;
	for(m=0;m<4;m++)//行循环
		for(n=0;n<4;n++)//列循环
		{
			if((Sign[0].dat&MaskTab[4*m+n])!=0)	 //如果积木在某一位置有实体
			{
				num[20-(Sign[0].y-2)/3+m]|=MaskTab[11-Sign[0].x/3-n]; //则在背景的相应位置画实体
						//除以3是因为最小的方块是3*3的像素	
						//由于是以每个单元的左下角作为基准
						//而左下角对应m最大，n最小，所以上式+m/-n
			}
		}
}

/********************************
判断积木图标中方块是否与障碍方块重合
*********************************/
unsigned char CheckIf(void)
{
	unsigned char m,n;
	for(m=0;m<4;m++)//行循环
		for(n=0;n<4;n++)//列循环
		{
			if((Sign[1].dat & MaskTab[4*m+n])!=0)//积木在某一位置有实体
			{
				if((num[20-(Sign[1].y-2)/3+m] & MaskTab[11-Sign[1].x/3-n])!=0)//刚下来的积木和以前的背景块重合了
					return 0;
			}
		}
	return 1;
}

/********************************
判断积木图标是否可以继续下降一格
********************************/
unsigned char CheckIfDown(void)
{
	Sign[1]=Sign[0];//
	Sign[1].y+=3;//假设下降一格	
	return CheckIf();
	
}

/********************************
判断积木图标是否可以向左移动
*********************************/
unsigned char CheckIfLeft(void)
{
	Sign[1]=Sign[0];
	Sign[1].x-=3;	
	return CheckIf();
}

/********************************
判断积木图标是否可以向右移动
*********************************/
unsigned char CheckIfRight(void)
{
	Sign[1]=Sign[0];
	Sign[1].x+=3;	
	return CheckIf();
}

/********************************
判断是否可以旋转
*********************************/
unsigned char CheckIfRoll(void)
{
	unsigned char i;
	unsigned int Temp;
	Sign[1]=Sign[0];
	if(++Sign[1].change>3)	//每个积木最多旋转4次就会复原			  
		Sign[1].change=0; 	//所以change=4则归零
   	i=Sign[1].type*4+Sign[1].change;//每种积木占四个一维数组
									//根据change的值决定当前是哪个方向
	Temp=(unsigned int)Block[i][0]<<8;	
	Temp=Temp|Block[i][1];
	Sign[1].dat=Temp;	 //先做旋转
	
	return CheckIf();	 //再检查是否干涉
}

/********************************
寻找满格的行并做消除处理
最多寻找4个满行并做消除
*********************************/
void DelFull(void)
{
	unsigned char m,n;
	unsigned char Temp;
	unsigned char Flag=0;
	Temp=(Sign[0].y-2)/3;
	if(Temp>=20)//防止越过了下边界
		Temp=1;
	else
		Temp=20-Temp; //变成最下面是第一行
	for(n=Temp+3;n>=Temp;n--)//积木图标的最顶行开始寻找满行比较有利于运算
	{
		if(num[n]==0xfff)	 //找到满行
		{
			Flag=1;			 //标志位置1
			for(m=n+1;m<=19;m++) //除去外围边框
			{
				num[m-1]=num[m];//把上一行的方块移到下面的一行
			}
			num[m]=0x801;	//上面的一行只留两侧边框
			Score++;//每找到一个满行，则分数加1
		}
	}
	if(Flag)//有满格消行变化则重画积木界面
	{
		for(m=Temp;m<=19;m++)//不必要重第一行重画起，只需要从积木图标最下行开始往上的重画
			for(n=1;n<=10;n++)
			{
				if((num[m]&MaskTab[n])==0)//如果某地应该为0 读取像素不为0 就消去这里的矩形
				{	
					if(Lcd_ReadPixel(30-(n-1)*3,57-(m-1)*3)!=0)//读象素
					{
						Lcd_Rectangle(30-(n-1)*3,57-(m-1)*3,30-(n-1)*3+2,57-(m-1)*3+2,0);
					}
				}
			   	else  //否则如果这个地方不应该等于0 读取像素却等于0 就在这里画矩形
				{
					if(Lcd_ReadPixel(30-(n-1)*3,57-(m-1)*3)==0)//读象素
					{
						Lcd_Rectangle(30-(n-1)*3,57-(m-1)*3,30-(n-1)*3+2,57-(m-1)*3+2,1);
					}
				}
			}
	}
}


/*******************************
随机产生一个积木图标放到预产生区域并显示出来
********************************/
void CreatSign(void)
{
	unsigned char n;
	unsigned int Temp;

	DrawSign(Sign[2],0);//先清除

	n=Random()*28;
	Temp=(unsigned int)Block[n][0]<<8;
	Temp=Temp|Block[n][1];
	Sign[2].dat=Temp;
	Sign[2].x=45;
	Sign[2].y=4*3+2;
	Sign[2].type=n/4;
	Sign[2].change=n%4;
	DrawSign(Sign[2],1);//后画出
}


//显示当前分数
void PrintScore(void)
{
	unsigned char Str[3];
//	Str[0]=3|0x30;
//	Str[1]=3|0x30;
//	Str[2]=3|0x30;
	Str[0]=(Score/10)|0x30;	  //或0x30是将数字转换为ascii码
	Str[1]=(Score%10)|0x30;	  
	Str[2]=0;				  //空格对应的ascii码为0
	Lcd_WriteStr(6,2,Str);
}


//显示当前等级
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
游戏的具体过程，也是俄罗斯方块算法的关键部分
*********************************/
void GamePlay(void)
{
	unsigned char m,n; 
	unsigned int Temp;
	SysFlag|=1<<NEWSIGNFLAG;//刚开始初始化为需要产生新的积木图标	  sysflag=1
	InitRandom(TL0);	//产生随机数（TL0为定时器0的低8位，作为随机数的种子）
	Lcd_WriteStr(3,1,"Playing");
	PrintScore();
	PrintLevel();
	CreatSign();
	while(1)
	{	
		if((SysFlag&(1<<NEWSIGNFLAG))==1)//判断是否需要产生新的积木图标
		{
			SysFlag&=~(1<<NEWSIGNFLAG);			//sysflag=0
			Sign[0]=Sign[2];  	//将要出现的积木等于预产生的积木
			CreatSign();   		//在预产生区显示下一个积木
			Sign[0].x=12;		//为刚产生的积木设置坐标
			Sign[0].y=14;

			for(m=0;m<4;m++)//行循环
			{
				for(n=0;n<4;n++)//列循环
				{
					unsigned int what=Sign[0].dat&MaskTab[15-m*4-n];//从最高位
					if(what!=0)	  //直接用与的算式判断不成功，赋值给unsigned int就好了
					{
						upflag=1;
						break;
				}	}
				if(n==4)
					Sign[0].y-=3;  //如果上面有空行上移一个单位
				if(upflag)		   //防止下面的空行也导致上移
				{
					break;
					upflag=1;
				}
			}//将积木图标出现置顶
			
			for(m=0;m<4;m++)//行循环
				for(n=0;n<4;n++)//列循环
				{
					if((Sign[0].dat&MaskTab[4*m+n])!=0)	//新积木的某个坐标有方块
					{
						if((num[20-(Sign[0].y-2)/3+m]&MaskTab[11-Sign[0].x/3-n])!=0)//且背景的相应坐标也有方块
							SysFlag|=1<<DEADFLAG;		 //则游戏失败
					}
				}
			if((SysFlag&(1<<DEADFLAG))!=0)
				break;//如果产生新的积木图标中的方块与已固定好的方块重合，则死亡。游戏结束
			DrawSign(Sign[0],1);	
		}
		if((CheckIfLeft())&&(K3==0))	 //左
		{
			DrawSign(Sign[0],0);
			Sign[0].x-=3;
			DrawSign(Sign[0],1);
		}			  
		if((CheckIfRight())&&(K4==0))	  //右
		{
			DrawSign(Sign[0],0);
			Sign[0].x+=3;
			DrawSign(Sign[0],1);	
		}
		if((CheckIfDown())&&(K2==0))//下
		{
			DrawSign(Sign[0],0);
			Sign[0].y+=3;
			DrawSign(Sign[0],1);	
		} 
		if((CheckIfRoll())&&(K1==0)) //翻转
		{
			Delay(100);	 //去抖
			if((CheckIfRoll())&&(K1==0))
			{	
				while(K1==0)
				{
				 	Delay(5000);   //防止按住up键相当于游戏暂停
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
		if(++DelayCnt>=2*(11-Level))//delay计数器 等级越高需要delay次数越少 
		{
			DelayCnt=0;		 //延时计数器清零
			if(CheckIfDown())//判断是否能继续下降一格
			{
				DrawSign(Sign[0],0);
				Sign[0].y+=3;
				DrawSign(Sign[0],1);  //删旧画新
			}
			else
			{						  
				FixSign();		//固定
				DelFull();		//删满
				PrintScore();	//显示成绩
				if(Score>=PASSSCORE*Level)
				{
					SysFlag&=~(1<<DEADFLAG);
					Level++;
					PrintLevel();
					if(Level>5)
					{
						break;
					}	
				//	break;//跳出玩游戏过程
				}
				SysFlag|=1<<NEWSIGNFLAG;//新的积木图标产生标志置1
			}
		}
	}
	
}

void main()
{  
//  	Lcd_Clearall();//清屏   
//Lcd_WriteStr(2,2,"Score:");	
	InitCpu();//初始化CPU
	Lcd_Reset(); //初始化LCD屏
	Lcd_Clear(0);//清屏   	
	Lcd_Reset(); //初始化LCD屏
	Lcd_Clear(0);//清屏
	//Lcd_WriteStr(2,2,"Score:");	
	DrawBoard();//画界面

	GamePlay();//玩游戏		
	GameOver();//游戏结束
	while(1);
//	Lcd_Reset(); //初始化LCD屏
//	Lcd_Clear(0);//清屏		
	
}
