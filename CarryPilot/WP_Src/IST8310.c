
/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/
/*----------------------------------------------------------------------------------------------------------------------/
*               ������ֻ��������ѧϰʹ�ã���Ȩ����Ȩ���������ƴ��Ŷӣ������ƴ��Ŷӽ��ɿس���Դ���ṩ�������ߣ�
*               ������ҪΪ�����ƴ��Ŷ��ṩ������δ��������ɣ����ý�Դ�����ṩ�����ˣ����ý�Դ����ŵ����Ϲ�����������أ� 
*               �������Դ�����Ĳ�����緢��������Ϊ�������ƴ��Ŷӽ���֮�Է��ɽ��������
-----------------------------------------------------------------------------------------------------------------------/
*               ������Ϣ���ܶ���ֹ��ǰ�����������˳���������
*               ��Դ���ף���ѧ����ϧ��ף������Ϯ�����׳ɹ�������
*               ѧϰ�����ߣ��������Ƽ���DJI��ZEROTECH��XAG��AEE��GDU��AUTEL��EWATT��HIGH GREAT�ȹ�˾��ҵ
*               ��ְ�����뷢�ͣ�15671678205@163.com���豸ע��ְ����λ����λ��������
*               �����ƴ���Դ�ɿ�QQȺ��540707961
*               CSDN���ͣ�http://blog.csdn.net/u011992534
*               �ſ�ID��NamelessCotrun����С��
*               Bվ��ѧ��Ƶ��https://space.bilibili.com/67803559/#/video
*               �ͻ�ʹ���ĵá��Ľ������������http://www.openedv.com/forum.php?mod=viewthread&tid=234214&extra=page=1
*               �Ա����̣�https://shop348646912.taobao.com/?spm=2013.1.1000126.2.5ce78a88ht1sO2
*               �ٶ�����:�����ƴ���Դ�ɿ�
*               ��˾����:www.nameless.tech
*               �޸�����:2019/4/12
*               �汾����Ӯ�ߡ���CarryPilot_V1.0
*               ��Ȩ���У�����ؾ���
*               Copyright(C) 2017-2025 �人�������¿Ƽ����޹�˾ 
*               All rights reserved
*               ��Ҫ��ʾ��
*               �����Ա�����ת�ֵķɿء��������ѡ�����ѧ�ܵĶ����Խ��ۺ�Ⱥѧϰ������
*               ����ֱ�����������������������ϣ��������´�����������Ȩ�����˲��ý�
*               ���ϴ��봫���Ϲ��������أ�������ı��ΪĿ���������ϴ��룬�����д˲�
*               ���ߣ���˾����ǰ��֪����1���ڼ�ʱ�����������ѧУ����λ����������
*               ������ַ��Ϣ�ᱻ�����ڹ�˾�������ٷ�΢�Ź���ƽ̨���ٷ��������͡�֪��
*               ר���Լ��Ա�������ҳ���Թ�ʾ���棬����������Ϊ�����Ϊ�����۵㣬Ӱ��
*               ��ѧ���ҹ���������������ܿ�ͺ������˻����������������ء�
*               �����Ϊ����˾����ش���ʧ�ߣ����Է���;���������л���ĺ�����лл������
----------------------------------------------------------------------------------------------------------------------*/
#include "Headfile.h"
#include "I2C.h"
#include "WP_DataType.h"
#include "IST8310.h"


#define MAG_EXTERNAL_ENABLE  1//�������ⲿ������ʱʹ��1
uint8_t ts[4]={0};
Vector2f magn={0,0};
Vector3f mag_offset;
Butter_Parameter Mag_Parameter;
/***********************************************************
@��������IST8310_Init
@��ڲ�������
@���ڲ�������
���������������Ƴ�ʼ������
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
void IST8310_Init(void)//�����Ƴ�ʼ������
{
#if (!MAG_EXTERNAL_ENABLE)//����ӳ���������������
	Single_WriteI2C(IST8310_SLAVE_ADDRESS,0x41,0x24);//����16x�ڲ�ƽ��
	Single_WriteI2C(IST8310_SLAVE_ADDRESS,0x42,0xC0);//Set/Reset�ڲ�ƽ��
#else
	I2C_GPIO_Config();
	Delay_Ms(10);
	Single_WriteI2C0(IST8310_SLAVE_ADDRESS,0x41,0x24);//����16x�ڲ�ƽ��
	Single_WriteI2C0(IST8310_SLAVE_ADDRESS,0x42,0xC0);//Set/Reset�ڲ�ƽ��
#endif	
  Set_Cutoff_Frequency(40, 18,&Mag_Parameter);//������У׼�Ӽ��˲�ֵ
}

/***********************************************************
@��������IST8310_Read_Data
@��ڲ�����vector3f *mag
@���ڲ�������
�������������������ݲɼ�״̬��
@���ߣ�����С��
@���ڣ�2019��01��27��
*************************************************************/
uint8_t IST8310_Read_Data(vector3f *mag)//��ȡ����������״̬��
{
  static uint16_t ist8310_sampling_cnt=0;
  uint8_t buf[6];
  ist8310_sampling_cnt++;
  if(ist8310_sampling_cnt==1)
  {
#if (!MAG_EXTERNAL_ENABLE)//����ӳ���������������
    Single_WriteI2C(IST8310_SLAVE_ADDRESS,IST8310_REG_CNTRL1,0x01);//Single Measurement Mode
#else	
		Single_WriteI2C0(IST8310_SLAVE_ADDRESS,IST8310_REG_CNTRL1,0x01);//Single Measurement Mode
#endif		
  }
  else if(ist8310_sampling_cnt==6)
  {
#if (!MAG_EXTERNAL_ENABLE)//����ӳ���������������
    buf[0]=Single_ReadI2C(IST8310_SLAVE_ADDRESS,0x03);//OUT_X_L_A
    buf[1]=Single_ReadI2C(IST8310_SLAVE_ADDRESS,0x04);//OUT_X_H_A
    buf[2]=Single_ReadI2C(IST8310_SLAVE_ADDRESS,0x05);//OUT_Y_L_A
    buf[3]=Single_ReadI2C(IST8310_SLAVE_ADDRESS,0x06);//OUT_Y_H_A
    buf[4]=Single_ReadI2C(IST8310_SLAVE_ADDRESS,0x07);//OUT_Z_L_A
    buf[5]=Single_ReadI2C(IST8310_SLAVE_ADDRESS,0x08);//OUT_Z_H_A
    mag->x= (int16_t)((buf[1]<<8)|buf[0]);
    mag->y=-(int16_t)((buf[3]<<8)|buf[2]);
    mag->z= (int16_t)((buf[5]<<8)|buf[4]);
#else	
    buf[0]=Single_ReadI2C0(IST8310_SLAVE_ADDRESS,0x03);//OUT_X_L_A
    buf[1]=Single_ReadI2C0(IST8310_SLAVE_ADDRESS,0x04);//OUT_X_H_A
    buf[2]=Single_ReadI2C0(IST8310_SLAVE_ADDRESS,0x05);//OUT_Y_L_A
    buf[3]=Single_ReadI2C0(IST8310_SLAVE_ADDRESS,0x06);//OUT_Y_H_A
    buf[4]=Single_ReadI2C0(IST8310_SLAVE_ADDRESS,0x07);//OUT_Z_L_A
    buf[5]=Single_ReadI2C0(IST8310_SLAVE_ADDRESS,0x08);//OUT_Z_H_A
    mag->x= (int16_t)((buf[1]<<8)|buf[0]);
    mag->y= (int16_t)((buf[3]<<8)|buf[2]);
    mag->z=-(int16_t)((buf[5]<<8)|buf[4]);
#endif			
    ist8310_sampling_cnt=0;
    return 1;
  }
  return 0;
}

/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/



