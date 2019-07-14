#include "remote_control.h"


/* ----------------------- Data Struct ------------------------------------- */
typedef __packed struct		
{											//�ڼ�����ڴ��У��ṹ������Ĵ洢ͨ���ǰ��ֳ������
											//32λ�������У���������4���ֽڶ��룬�����ĸ��ֽڵı����ᵼ���ڴ���ֿ�ȱ
											//__packed �ؼ��ֿ���ǿ�Ʊ������ֽڶ��룬�Ӷ�û���ڴ��ȱ
	__packed struct 
	{
		uint16_t ch0;			//ң�������˵�4��ͨ��
		uint16_t ch1;			
		uint16_t ch2;
		uint16_t ch3;		
		uint8_t s1;				//ң��������������
		uint8_t s2;
	}rc;								//rc����ң����
	
	__packed struct
	{
		int16_t x;				//����ڸ������᷽���ƶ��ٶ�
		int16_t y;
		int16_t z;
		uint8_t press_l;	//������Ҽ�״̬
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
	//ң�ز��ˡ���ÿ��ͨ��ȡ11λ
	RC_CtrlData.rc.ch0 =  ((int16_t)pData[0] 				| ((int16_t)pData[1] << 8)) 														 & 0x07FF;		
	RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) 	| ((int16_t)pData[2] << 5)) 														 & 0x07FF;
	RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6)	| ((int16_t)pData[3] << 2)  | ((int16_t)pData[4] << 10)) & 0x07FF;
	RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) 	| ((int16_t)pData[5] << 7)) 														 & 0x07FF;
	
	//ң��������������
	RC_CtrlData.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
	RC_CtrlData.rc.s2 = ((pData[5] >> 4) & 0x0003);
	
	//����ٶ�
	RC_CtrlData.mouse.x = ((int16_t)pData[6])  | ((int16_t)pData[7]  << 8);
	RC_CtrlData.mouse.y = ((int16_t)pData[8])  | ((int16_t)pData[9]  << 8);
	RC_CtrlData.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);
	
	//��갴��
	RC_CtrlData.mouse.press_l = pData[12];
	RC_CtrlData.mouse.press_r = pData[13];
	
	//����
	RC_CtrlData.key.w 	  =  pData[14]		&0x01; 
	RC_CtrlData.key.s   	= (pData[14]>>1)&0x01;
	RC_CtrlData.key.a 	  = (pData[14]>>2)&0x01;
	RC_CtrlData.key.d 	  = (pData[14]>>3)&0x01;
	RC_CtrlData.key.q 	  =	(pData[14]>>4)&0x01;
	RC_CtrlData.key.e 	  = (pData[14]>>5)&0x01;
	RC_CtrlData.key.shift = (pData[14]>>6)&0x01;
	RC_CtrlData.key.ctrl  = (pData[14]>>7)&0x01;	
	//pData[15]
	//your control code ��.

}


//�������ṹ�帴λ����
void RC_Rst(void)
{
		RC_CtrlData.rc.ch0 = 1024;
		RC_CtrlData.rc.ch1 = 1024;
		RC_CtrlData.rc.ch2 = 1024;
		RC_CtrlData.rc.ch3 = 1024;				//0��1��2��3���ǲ��� ��λ���м�ֵ
		RC_CtrlData.mouse.x = 0;
		RC_CtrlData.mouse.y = 0;
		RC_CtrlData.mouse.z = 0;					//���ֵ��ʾ��ָ�������ϵ��ƶ��ٶ� 0��ʾ��ֹ
		RC_CtrlData.mouse.press_l = 0;                     		                           
		RC_CtrlData.mouse.press_r = 0;		//��갴����λ��û���£�
	
		RC_CtrlData.key.w = 0;
		RC_CtrlData.key.s = 0;                            
		RC_CtrlData.key.a = 0;
		RC_CtrlData.key.d = 0;
		RC_CtrlData.key.q = 0;
		RC_CtrlData.key.e = 0;
		RC_CtrlData.key.shift = 0;
		RC_CtrlData.key.ctrl = 0;				//����������λ��û���£�
	
	  RC_CtrlData.rc.s1 = 2;
		RC_CtrlData.rc.s2 = 2;						//S1��S2���ظ�λ��2�������£�	
}   
