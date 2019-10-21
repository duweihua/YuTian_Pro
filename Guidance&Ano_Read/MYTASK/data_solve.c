/* Includes */
#include "data_solve.h"
#include "delay.h"
#include "DJI_guidance.h"
#include "string.h"

/* guidance part */

/* guidance structure data define */
imu imu_data;
ultrasonic_data ultrasonic;
velocity_new vo;
obstacle_distance oa;

/* guidance head and end data length */
#define GuidanceDataLength	16

/* guidance head and end data */
uint8_t Guidancedata[GuidanceDataLength] = {0};

/* guidance valid data */
uint8_t realdata[200] = {0};
uint16_t realdata_count = 0;

/* guidance veicity data to watch */
float vx_cm,vy_cm,vz_cm = 0;

/* guidance data prepare */
void Guidance_DT_Data_Receive_Prepare(u8 data)
{
	static uint16_t _data_len = 0;
	static u8 state = 0;
	static u8 resever_count = 0;
	static u8 crc_count1 = 0;
	static u8 crc_count2 = 0;
	
	/* head : 0xAA */
	if(state==0&&data==0xAA)	
	{
		state=1;
		Guidancedata[0]=data;
	}
	/* valid data length + protocol version */
	else if(state==1) 
	{
		state=2;
		Guidancedata[1]=data;
	}
	/* valid data length + protocol version */
	/* at this I see only the Guidancedata[2] as data length */
	/* calculate method: move to left two bit and then (- 16) */
	else if(state==2)
	{
		state = 3;
		Guidancedata[2] = data;
		_data_len = ((uint16_t)Guidancedata[2])*4;
		_data_len = _data_len - 16;
		realdata_count = 0;
	}
	/* reserve byte */
	else if(state==3)	
	{
		resever_count++;
		Guidancedata[2+resever_count] = data;
		if(resever_count == 5)
		{
			resever_count = 0;
			state = 4;
		}
	}
	/* data num + CRC16 */
	else if(state==4) //ÐòÁÐºÅ+CRC
	{
		crc_count1++;
		Guidancedata[7+crc_count1] = data;
		if(crc_count1 == 4)
		{
			crc_count1 = 0;
			state = 5;
		}
	}
	/* valid data */
	else if(state==5&&_data_len>0&&_data_len<1007)
	{
		_data_len--;
		realdata[realdata_count++] = data;
		if(_data_len==0)
		{
			state = 6;
			realdata_count = 0;
		}
	}
	/* CRC32 */
	else if(state==6)
	{
		crc_count2++;
		Guidancedata[11+crc_count2] = data;
		if(crc_count2 == 4)
		{
			crc_count2 = 0;
			GuidancedataProcess();
			state = 0;
		}
	}
	/* error */
	else
		state = 0;
}

/* Guidance data solve */
/* vo.vx\vy\vz is velcity data after fix (mm/s)
	 vx_cm\vy_cm\vz_cm is velcity data to watch (cm/s)
   oa.distance[0] is altitude data after fix (cm) */
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
			
			vx_cm = vo.vx/10.0f;
			vy_cm = vo.vy/10.0f;
			vz_cm = vo.vz/10.0f;
		}
		/* obstacle_distance data */
		if ( e_obstacle_distance == cmdid )
		{
			memcpy( &oa, realdata + 2, sizeof(oa) );
		}
	}
}

/* ANO_OF part */

/* ANO_OF data define */
uint8_t		OF_QUA,OF_LIGHT;
int8_t		OF_DX,OF_DY;
int16_t		OF_DX2,OF_DY2,OF_DX2FIX,OF_DY2FIX;
uint16_t	OF_ALT,OF_ALT2;
int16_t		OF_GYR_X,OF_GYR_Y,OF_GYR_Z;
int16_t		OF_GYR_X2,OF_GYR_Y2,OF_GYR_Z2;
int16_t		OF_ACC_X,OF_ACC_Y,OF_ACC_Z;
int16_t		OF_ACC_X2,OF_ACC_Y2,OF_ACC_Z2;
float		OF_ATT_ROL,OF_ATT_PIT,OF_ATT_YAW;
float		OF_ATT_S1,OF_ATT_S2,OF_ATT_S3,OF_ATT_S4;

/* check states structure */
_sensor_hd_check_st sens_hd_check;

/* data receive temp variables */
static uint8_t _datatemp[50];
static u8 _data_cnt = 0;
static u8 anoof_data_ok;

/* loop selfcheck task */
/* this project do not use 
   you can call it with a frequency */
void AnoOF_DataAnl_Task(u8 dT_ms)
{
	AnoOF_Check(dT_ms);
}

/* ANO_OF data prepare */
void AnoOF_GetOneByte(uint8_t data)
{

	static u8 _data_len = 0;
	static u8 state = 0;
	
	/* head : AA */
	if(state==0&&data==0xAA)
	{
		state=1;
		_datatemp[0]=data;
	}
	/* surce address */
	else if(state==1&&data==0x22)	
	{
		state=2;
		_datatemp[1]=data;
	}
	/* destination address */
	else if(state==2)
	{
		state=3;
		_datatemp[2]=data;
	}
	/* data id data */
	else if(state==3)	
	{
		state = 4;
		_datatemp[3]=data;
	}
	/* data length */
	else if(state==4)
	{
		state = 5;
		_datatemp[4]=data;
		_data_len = data;
		_data_cnt = 0;
	}
	/* valid data */
	else if(state==5&&_data_len>0)
	{
		_data_len--;
		_datatemp[5+_data_cnt++]=data;
		if(_data_len==0)
			state = 6;
	}
	/* end and solve data */
	else if(state==6)
	{
		state = 0;
		_datatemp[5+_data_cnt]=data;
		AnoOF_DataAnl(_datatemp,_data_cnt+6);
	}
	else
		state = 0;
}

/* selfcheck data define */
static u8 of_check_f[2];
static u16 of_check_cnt[2] = { 10000,10000 };
/* selfcheck function */
void AnoOF_Check(u8 dT_ms)
{
	for(u8 i=0;i<2;i++)
	{
		if(of_check_f[i] == 0 )
		{
			if(of_check_cnt[i]<10000)
			{
				of_check_cnt[i] += dT_ms;	
			}
		}
		else
		{
			of_check_cnt[i] = 0;
		}
		of_check_f[i] = 0;
	}
	
	if(of_check_cnt[0] > 1000 || of_check_cnt[1] > 1000)
	{
		sens_hd_check.of_ok = 0;
	}
	else
	{
		sens_hd_check.of_ok = 1;
	}
}

/* ANO_OF data solve */
/* DX2\DY2 is velcity data after fix (cm/s)
   fit to do velcity circle control */
/* OF_ALT is original altitude data (cm)
	 OF_ALT2 is altitude data after fix (cm) but can not read */
void AnoOF_DataAnl(uint8_t *data_buf,uint8_t num)
{
	u8 sum = 0;
	for(u8 i=0;i<(num-1);i++)
		sum += *(data_buf+i);
	if(!(sum==*(data_buf+num-1)))		return;		
	
	/* OF data */
	if(*(data_buf+3)==0X51)
	{
		/* original OF data */
		if(*(data_buf+5)==0)
		{
			OF_QUA 		= *(data_buf+6);
			OF_DX  		= *(data_buf+7);
			OF_DY  		= *(data_buf+8);
			OF_LIGHT  = *(data_buf+9);
		}
		/* fix OF data */
		else if(*(data_buf+5)==1)
		{
			OF_QUA 		= *(data_buf+6);
			OF_DX2		= (int16_t)(*(data_buf+7)<<8)|*(data_buf+8) ;
			OF_DY2		= (int16_t)(*(data_buf+9)<<8)|*(data_buf+10) ;
			OF_DX2FIX	= (int16_t)(*(data_buf+11)<<8)|*(data_buf+12) ;
			OF_DY2FIX	= (int16_t)(*(data_buf+13)<<8)|*(data_buf+14) ;
			OF_LIGHT  = *(data_buf+19);
			
			of_check_f[0] = 1;
		}
	}
	/* altitude data */
	if(*(data_buf+3)==0X52)
	{
		/* original altitude data */
		if(*(data_buf+5)==0)
		{
			OF_ALT = (uint16_t)(*(data_buf+6)<<8)|*(data_buf+7) ;
			of_check_f[1] = 1;
		}
		/* fix altitude data */
		else if(*(data_buf+5)==1)
		{
			OF_ALT2 = (uint16_t)(*(data_buf+6)<<8)|*(data_buf+7) ;
		}
	}
}








