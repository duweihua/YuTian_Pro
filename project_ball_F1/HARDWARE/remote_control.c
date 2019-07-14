#include "remote_control.h"


/* ----------------------- Data Struct ------------------------------------- */
typedef __packed struct		
{											//在计算机内存中，结构体变量的存储通常是按字长对齐的
											//32位处理器中，变量按照4个字节对齐，则不足四个字节的变量会导致内存出现空缺
											//__packed 关键字可以强制变量单字节对齐，从而没有内存空缺
	__packed struct 
	{
		uint16_t ch0;			//遥控器拨杆的4个通道
		uint16_t ch1;			
		uint16_t ch2;
		uint16_t ch3;		
		uint8_t s1;				//遥控器的自锁开关
		uint8_t s2;
	}rc;								//rc代表遥控器
	
	__packed struct
	{
		int16_t x;				//鼠标在各坐标轴方向移动速度
		int16_t y;
		int16_t z;
		uint8_t press_l;	//鼠标左右键状态
		uint8_t press_r;
	}mouse;
	
	__packed struct KEY
	{
		uint16_t w;
		uint16_t s;
		uint16_t a;
		uint16_t d;
		uint16_t q;
		uint16_t e;
		uint16_t shift;
		uint16_t ctrl;
	}key;
	
}RC_Ctl;

RC_Ctl RC_CtrlData;


/******************************************************************************
* @fn RemoteDataProcess
** @brief resolution rc protocol data.
* @pData a point to rc receive buffer.
* @return None.
* @note RC_CtrlData is a global variable.you can deal with it in other place.
*/
void RemoteDataProcess(uint8_t *pData)
{
//	if(pData == NULL)
//	{
//		return;
//	}
//	
	//遥控拨杆——每个通道取11位
	RC_CtrlData.rc.ch0 =  ((int16_t)pData[0] 				| ((int16_t)pData[1] << 8)) 														 & 0x07FF;		
	RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) 	| ((int16_t)pData[2] << 5)) 														 & 0x07FF;
	RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6)	| ((int16_t)pData[3] << 2)  | ((int16_t)pData[4] << 10)) & 0x07FF;
	RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) 	| ((int16_t)pData[5] << 7)) 														 & 0x07FF;
	
	//遥控自锁三级拨杆
	RC_CtrlData.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
	RC_CtrlData.rc.s2 = ((pData[5] >> 4) & 0x0003);
	
	//鼠标速度
	RC_CtrlData.mouse.x = ((int16_t)pData[6])  | ((int16_t)pData[7]  << 8);
	RC_CtrlData.mouse.y = ((int16_t)pData[8])  | ((int16_t)pData[9]  << 8);
	RC_CtrlData.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);
	
	//鼠标按键
	RC_CtrlData.mouse.press_l = pData[12];
	RC_CtrlData.mouse.press_r = pData[13];
	
	//键盘
	RC_CtrlData.key.w 	  =  pData[14]		&0x01; 
	RC_CtrlData.key.s   	= (pData[14]>>1)&0x01;
	RC_CtrlData.key.a 	  = (pData[14]>>2)&0x01;
	RC_CtrlData.key.d 	  = (pData[14]>>3)&0x01;
	RC_CtrlData.key.q 	  =	(pData[14]>>4)&0x01;
	RC_CtrlData.key.e 	  = (pData[14]>>5)&0x01;
	RC_CtrlData.key.shift = (pData[14]>>6)&0x01;
	RC_CtrlData.key.ctrl  = (pData[14]>>7)&0x01;	
	//pData[15]
	//your control code ….

}


//控制器结构体复位函数
void RC_Rst(void)
{
		RC_CtrlData.rc.ch0 = 1024;
		RC_CtrlData.rc.ch1 = 1024;
		RC_CtrlData.rc.ch2 = 1024;
		RC_CtrlData.rc.ch3 = 1024;				//0、1、2、3都是拨杆 复位到中间值
		RC_CtrlData.mouse.x = 0;
		RC_CtrlData.mouse.y = 0;
		RC_CtrlData.mouse.z = 0;					//鼠标值表示在指定方向上的移动速度 0表示静止
		RC_CtrlData.mouse.press_l = 0;                     		                           
		RC_CtrlData.mouse.press_r = 0;		//鼠标按键复位（没按下）
	
		RC_CtrlData.key.w = 0;
		RC_CtrlData.key.s = 0;                            
		RC_CtrlData.key.a = 0;
		RC_CtrlData.key.d = 0;
		RC_CtrlData.key.q = 0;
		RC_CtrlData.key.e = 0;
		RC_CtrlData.key.shift = 0;
		RC_CtrlData.key.ctrl = 0;				//操作按键复位（没按下）
	
	  RC_CtrlData.rc.s1 = 2;
		RC_CtrlData.rc.s2 = 2;						//S1、S2开关复位到2（拨向下）	
}   
