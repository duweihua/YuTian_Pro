
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
#include "RDroneSudio.h"

/* ��������ʮ������ת�� */
/* floatNum�������ͱ��� */
/* byteArry���ֽ�� 4 ���ֽڵ��׵�ַ */
void FloatToByte(float floatNum,unsigned char* byteArry)
{
	int i;
	char* pchar=(char*)&floatNum;
	for(i=0;i<sizeof(float);i++)
	{
		*byteArry=*pchar;
		pchar++;
		byteArry++;
	}
}

void RDroneStudio_Send_Attitude(float rd_pitch,float rd_roll,float rd_yaw)
{
  unsigned char rd_buf[16];
	rd_buf[0]=0x55;
	rd_buf[1]=0xAA;
  rd_buf[2]=0x00;
  FloatToByte(rd_pitch,&rd_buf[3]);
  FloatToByte(rd_roll,&rd_buf[7]);
  FloatToByte(rd_yaw,&rd_buf[11]);
	rd_buf[15]=0x55;
	USART0_Send(rd_buf, 16);
}
void RDroneStudio_Send_Pos(float pos_z,float pos_x,float pos_y)
{
  unsigned char rd_buf[16];
	rd_buf[0]=0x55;
	rd_buf[1]=0xAA;
  rd_buf[2]=0x01;
  FloatToByte(pos_z,&rd_buf[3]);
  FloatToByte(pos_x,&rd_buf[7]);
  FloatToByte(pos_y,&rd_buf[11]);
	rd_buf[15]=0x55;
	USART0_Send(rd_buf, 16);
}
void RDroneStudio_Send_Gyro(float rd_pitch_gyro,float rd_roll_gyro,float rd_yaw_gyro)
{
  unsigned char rd_buf[16];
	rd_buf[0]=0x55;
	rd_buf[1]=0xAA;
  rd_buf[2]=0x20;
  FloatToByte(rd_pitch_gyro,&rd_buf[3]);
  FloatToByte(rd_roll_gyro,&rd_buf[7]);
  FloatToByte(rd_yaw_gyro,&rd_buf[11]);
	rd_buf[15]=0x00;
	USART0_Send(rd_buf, 16);
}

void RDroneStudio_Send_AcceData(float x,float y,float z)
{
  unsigned char rd_buf[16];
	rd_buf[0]=0x55;
	rd_buf[1]=0xAA;
  rd_buf[2]=0x21;
  FloatToByte(x,&rd_buf[3]);
  FloatToByte(y,&rd_buf[7]);
  FloatToByte(z,&rd_buf[11]);
	rd_buf[15]=0x00;
	USART0_Send(rd_buf, 16);
}

void RDroneStudio_Send_MagData(float x,float y,float z)
{
  unsigned char rd_buf[16];
	rd_buf[0]=0x55;
	rd_buf[1]=0xAA;
  rd_buf[2]=0x22;
  FloatToByte(x,&rd_buf[3]);
  FloatToByte(y,&rd_buf[7]);
  FloatToByte(z,&rd_buf[11]);
	rd_buf[15]=0x00;
	USART0_Send(rd_buf, 16);
}

void RDroneStudio_Send_PID(unsigned char label,float kp,float ki,float kd)
{
  unsigned char rd_buf[16];
	rd_buf[0]=0x55;
	rd_buf[1]=0xAA;
  rd_buf[2]=label;
  FloatToByte(kp,&rd_buf[3]);
  FloatToByte(ki,&rd_buf[7]);
  FloatToByte(kd,&rd_buf[11]);
	rd_buf[15]=0x00;
	USART0_Send(rd_buf, 16);
}

void RDroneStudio_Send_PID_ALL(void)
{
	RDroneStudio_Send_PID(0x02,Total_Controller.Pitch_Angle_Control.Kp
	                          ,Total_Controller.Pitch_Angle_Control.Ki
                            ,Total_Controller.Pitch_Angle_Control.Kd);
 	RDroneStudio_Send_PID(0x03,Total_Controller.Roll_Angle_Control.Kp
	                          ,Total_Controller.Roll_Angle_Control.Ki
                            ,Total_Controller.Roll_Angle_Control.Kd);
	RDroneStudio_Send_PID(0x04,Total_Controller.Pitch_Gyro_Control.Kp
	                          ,Total_Controller.Pitch_Gyro_Control.Ki
                            ,Total_Controller.Pitch_Gyro_Control.Kd);
 	RDroneStudio_Send_PID(0x05,Total_Controller.Roll_Gyro_Control.Kp
	                          ,Total_Controller.Roll_Gyro_Control.Ki
                            ,Total_Controller.Roll_Gyro_Control.Kd);	
  RDroneStudio_Send_PID(0x06,Total_Controller.Yaw_Angle_Control.Kp
	                          ,Total_Controller.Yaw_Angle_Control.Ki
                            ,Total_Controller.Yaw_Angle_Control.Kd);
  RDroneStudio_Send_PID(0x07,Total_Controller.Yaw_Gyro_Control.Kp
	                          ,Total_Controller.Yaw_Gyro_Control.Ki
                            ,Total_Controller.Yaw_Gyro_Control.Kd);
	
	RDroneStudio_Send_PID(0x08,Total_Controller.High_Position_Control.Kp
	                          ,Total_Controller.High_Position_Control.Ki
                            ,Total_Controller.High_Position_Control.Kd);
  RDroneStudio_Send_PID(0x09,Total_Controller.High_Speed_Control.Kp
	                          ,Total_Controller.High_Speed_Control.Ki
                            ,Total_Controller.High_Speed_Control.Kd);
	
  RDroneStudio_Send_PID(0x0A,Total_Controller.Latitude_Position_Control.Kp
	                          ,Total_Controller.Latitude_Position_Control.Ki
                            ,Total_Controller.Latitude_Position_Control.Kd);
  RDroneStudio_Send_PID(0x0B,Total_Controller.Latitude_Speed_Control.Kp
	                          ,Total_Controller.Latitude_Speed_Control.Ki
                            ,Total_Controller.Latitude_Speed_Control.Kd);
	RDroneStudio_Send_PID(0x0C,Total_Controller.Longitude_Position_Control.Kp
	                          ,Total_Controller.Longitude_Position_Control.Ki
                            ,Total_Controller.Longitude_Position_Control.Kd);
  RDroneStudio_Send_PID(0x0D,Total_Controller.Longitude_Speed_Control.Kp
	                          ,Total_Controller.Longitude_Speed_Control.Ki
                            ,Total_Controller.Longitude_Speed_Control.Kd);													
	
}  

u8 RDroneStudio_RxBuffer[50];	
void RDroneStudio_Data_Anl(unsigned char *PCData);
void RDroneStudio_Receive(u8 data)
{	
	static u8 _data_cnt = 0;
	static u8 state = 0;
	if(state==0&&data==0x55)
	{
		state=1;
		RDroneStudio_RxBuffer[0]=data;
	}
	else if(state==1&&data==0xAA)
	{
		state=2;
		RDroneStudio_RxBuffer[1]=data;
	}
	else if(state==2)
	{
		state=3;
		RDroneStudio_RxBuffer[2]=data;
	}
	else if(state==3&&_data_cnt<12)
	{
		RDroneStudio_RxBuffer[_data_cnt++]=data;
		state = 4;
	}
	else if(state==4)
	{
		state = 0;
		RDroneStudio_RxBuffer[15]=data;
		//RDroneStudio_Data_Anl(RDroneStudio_RxBuffer);
	}
	else state = 0;
}



uint8_t RDroneStudio_Send_PID_FLAG=0;
void RDroneStudio_Send(void)
{
	RDroneStudio_Send_Attitude(Pitch,Roll,Yaw);
	RDroneStudio_Send_Gyro(Pitch_Gyro,Roll_Gyro,Yaw_Gyro);
	RDroneStudio_Send_AcceData(accel.x/AcceMax,accel.y/AcceMax,accel.z/AcceMax);
	RDroneStudio_Send_MagData(mag.x,mag.y,mag.z);
	RDroneStudio_Send_Pos(NamelessQuad.Position[_YAW]/100.0f,NamelessQuad.Position[_PITCH]/100.0f,NamelessQuad.Position[_ROLL]/100.0f);
	if(RDroneStudio_Send_PID_FLAG==1)
	{
		RDroneStudio_Send_PID_ALL();
		RDroneStudio_Send_PID_FLAG=0;
	}
}

float_union HexToFloat;
float UnsignedcharToFloat(unsigned char *DataofPC,unsigned char sequence)
{
    HexToFloat.sv[0] = (((uint16_t)DataofPC[sequence+1]) << 8 | DataofPC[sequence]);
    HexToFloat.sv[1] = (((uint16_t)DataofPC[sequence+3]) << 8 | DataofPC[sequence+2]);
    return HexToFloat.fv;
}

void RDroneStudio_Data_Anl(unsigned char *PCData)
{
        /*��ȡPID*/
        if(PCData[2]==0x03)
        {
           RDroneStudio_Send_PID_FLAG=1;
        }
        /* ����Pitch��PID���� */
        else if(PCData[2]==0x07)
				{
            Total_Controller.Pitch_Angle_Control.Kp= UnsignedcharToFloat(PCData,3);
            Total_Controller.Pitch_Angle_Control.Ki = UnsignedcharToFloat(PCData,7);
            Total_Controller.Pitch_Angle_Control.Kd = UnsignedcharToFloat(PCData,11);
            RDroneStudio_Send_PID_FLAG=1;
            //Write_Config();
        }
        /* ����ROll��PID���� */
        else if(PCData[2]==0x08)
				{
            Total_Controller.Roll_Angle_Control.Kp = UnsignedcharToFloat(PCData,3);
            Total_Controller.Roll_Angle_Control.Ki = UnsignedcharToFloat(PCData,7);
            Total_Controller.Roll_Angle_Control.Kd = UnsignedcharToFloat(PCData,11);
            RDroneStudio_Send_PID_FLAG=1;
            //Write_Config();
        }
        /* ����Yaw��PID���� */
        else if(PCData[2]==0x09)
				{
            Total_Controller.Yaw_Angle_Control.Kp = UnsignedcharToFloat(PCData,3);
            Total_Controller.Yaw_Angle_Control.Ki = UnsignedcharToFloat(PCData,7);
            Total_Controller.Yaw_Angle_Control.Kd = UnsignedcharToFloat(PCData,11);
            RDroneStudio_Send_PID_FLAG=1;
            //Write_Config();
        }
        /* ����Height��PID���� */
        else if(PCData[2]==0x0A)
				{
            Total_Controller.High_Position_Control.Kp = UnsignedcharToFloat(PCData,3);
            Total_Controller.High_Position_Control.Ki = UnsignedcharToFloat(PCData,7);
            Total_Controller.High_Position_Control.Kd = UnsignedcharToFloat(PCData,11);
            RDroneStudio_Send_PID_FLAG=1;
            //Write_Config();
        }
        /* ����accPitch��PID���� */
        else if(PCData[2]==0x0B)
				{
            Total_Controller.Pitch_Gyro_Control.Kp = UnsignedcharToFloat(PCData,3);
            Total_Controller.Pitch_Gyro_Control.Ki = UnsignedcharToFloat(PCData,7);
            Total_Controller.Pitch_Gyro_Control.Kd = UnsignedcharToFloat(PCData,11);
            RDroneStudio_Send_PID_FLAG=1;
            //Write_Config();
        }
        /* ����accRoll��PID���� */
        else if(PCData[2]==0x0C)
				{
            Total_Controller.Roll_Gyro_Control.Kp = UnsignedcharToFloat(PCData,3);
            Total_Controller.Roll_Gyro_Control.Ki = UnsignedcharToFloat(PCData,7);
            Total_Controller.Roll_Gyro_Control.Kd = UnsignedcharToFloat(PCData,11);
            RDroneStudio_Send_PID_FLAG=1;
            //Write_Config();
        }
        /* ����accYaw��PID���� */
        else if(PCData[2]==0x0D)
				{
            Total_Controller.Yaw_Gyro_Control.Kp = UnsignedcharToFloat(PCData,3);
            Total_Controller.Yaw_Gyro_Control.Ki = UnsignedcharToFloat(PCData,7);
            Total_Controller.Yaw_Gyro_Control.Kd = UnsignedcharToFloat(PCData,11);
            RDroneStudio_Send_PID_FLAG=1;
            //Write_Config();
        }
        /* ����accHeight��PID���� */
        else if(PCData[2]==0x0E)
				{
            Total_Controller.High_Speed_Control.Kp = UnsignedcharToFloat(PCData,3);
            Total_Controller.High_Speed_Control.Ki = UnsignedcharToFloat(PCData,7);
            Total_Controller.High_Speed_Control.Kd = UnsignedcharToFloat(PCData,11);
            RDroneStudio_Send_PID_FLAG=1;
            //Write_Config();
        }
        /* ����PositionX��PID���� */
        else if(PCData[2]==0x12)
				{
            Total_Controller.Latitude_Position_Control.Kp = UnsignedcharToFloat(PCData,3);
            Total_Controller.Latitude_Position_Control.Ki = UnsignedcharToFloat(PCData,7);
            Total_Controller.Latitude_Position_Control.Kd = UnsignedcharToFloat(PCData,11);
            RDroneStudio_Send_PID_FLAG=1;
            //Write_Config();
        }
        /* ����PositionY��PID���� */
        else if(PCData[2]==0x13)
				{
            Total_Controller.Longitude_Position_Control.Kp = UnsignedcharToFloat(PCData,3);
            Total_Controller.Longitude_Position_Control.Ki = UnsignedcharToFloat(PCData,7);
            Total_Controller.Longitude_Position_Control.Kd = UnsignedcharToFloat(PCData,11);
            RDroneStudio_Send_PID_FLAG=1;
            //Write_Config();
        }
        /* ����PositionVX��PID���� */
        else if(PCData[2]==0x14)
				{
            Total_Controller.Latitude_Speed_Control.Kp = UnsignedcharToFloat(PCData,3);
            Total_Controller.Latitude_Speed_Control.Ki = UnsignedcharToFloat(PCData,7);
            Total_Controller.Latitude_Speed_Control.Kd = UnsignedcharToFloat(PCData,11);
            RDroneStudio_Send_PID_FLAG=1;
            //Write_Config();
        }
        /* ����PositionVY��PID���� */
        else if(PCData[2]==0x15)
				{
            Total_Controller.Longitude_Speed_Control.Kp = UnsignedcharToFloat(PCData,3);
            Total_Controller.Longitude_Speed_Control.Ki = UnsignedcharToFloat(PCData,7);
            Total_Controller.Longitude_Speed_Control.Kd = UnsignedcharToFloat(PCData,11);
            RDroneStudio_Send_PID_FLAG=1;
            //Write_Config();
        }
        /*������У׼*/
        else if(PCData[2]==0x30)
        {
            //OffsetData.GyroX  = RT_Info.GyroX ;
            //OffsetData.GyroY  = RT_Info.GyroY ;
           // OffsetData.GyroZ  = RT_Info.GyroZ ;
           // Write_Config();
        }
}

/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/

