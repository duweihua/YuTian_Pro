/* Includes */
#include "data_solve.h"
#include "delay.h"
#include "DJI_guidance.h"
#include "string.h"

/* data define */
#define GuidanceDataLength	16
imu imu_data;
ultrasonic_data ultrasonic;
//velocity vo;
velocity_new vo;
obstacle_distance oa;
uint8_t Guidancedata[GuidanceDataLength] = {0};
uint8_t realdata[200] = {0};
uint16_t realdata_count = 0;
uint8_t recedata_ok = 0;

void Guidance_DT_Data_Receive_Prepare(u8 data)
{
	static uint16_t _data_len = 0;
	static u8 state = 0;
	static u8 resever_count = 0;
	static u8 crc_count1 = 0;
	static u8 crc_count2 = 0;
	
	if(state==0&&data==0xAA)	//帧头0xAA
	{
		state=1;
		Guidancedata[0]=data;
	}
	else if(state==1) //有效数据长度+协议版本
	{
		state=2;
		Guidancedata[1]=data;
	}
	else if(state==2)
	{
		state = 3;
		Guidancedata[2] = data;
//		_data_len = (( ((uint16_t)(Guidancedata[1]<<8)) | ((uint16_t)Guidancedata[2]) ) & (0x03FF));
		_data_len = ((uint16_t)Guidancedata[2])*4;
		_data_len = _data_len - 16;
		realdata_count = 0;
	}
	else if(state==3)	//保留字段
	{
		resever_count++;
		Guidancedata[2+resever_count] = data;
		if(resever_count == 5)
		{
			resever_count = 0;
			state = 4;
		}
	}
	else if(state==4) //序列号+CRC
	{
		crc_count1++;
		Guidancedata[7+crc_count1] = data;
		if(crc_count1 == 4)
		{
			crc_count1 = 0;
			state = 5;
		}
	}
	else if(state==5&&_data_len>0&&_data_len<1007) //数据 
	{
		_data_len--;
		realdata[realdata_count++] = data;
		if(_data_len==0)
		{
			state = 6;
			realdata_count = 0;
		}
	}
	else if(state==6)
	{
		crc_count2++;
		Guidancedata[11+crc_count2] = data;
		if(crc_count2 == 4)
		{
			crc_count2 = 0;
			GuidancedataProcess();
			state = 0;
//			recedata_ok = 1;
		}
	}
	else
		state = 0;
}

/* Guidance data solve */
void GuidancedataProcess(void)
{
	uint8_t cmdid = 0xFF;
		
	/*read data id */
	cmdid = realdata[1];
	
	/* data process */
	{
		/* imu data */
		if ( e_imu == cmdid )
		{
			memcpy( &imu_data, realdata + 2, sizeof(imu_data) );
		}
		/* ultrasonic data */
		if ( e_ultrasonic == cmdid )
		{
			memcpy( &ultrasonic, realdata + 2, sizeof(ultrasonic) );
		}
		/* velocity data */
		if ( e_velocity == cmdid )
		{
			memcpy( &vo, realdata + 2, sizeof(vo) );
		}
		/* obstacle_distance data */
		if ( e_obstacle_distance == cmdid )
		{
			memcpy( &oa, realdata + 2, sizeof(oa) );
		}
	}
}









