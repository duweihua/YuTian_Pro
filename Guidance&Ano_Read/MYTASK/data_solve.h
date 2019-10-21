/* Define to prevent recursive inclusion */
#ifndef DATA_SOLVE_H
#define DATA_SOLVE_H

/* Includes */
#include "stm32f4xx.h"

/* Exported types */
#pragma pack(1)
typedef struct _protocal_sdk_uart_header
{
	unsigned char         m_header;
	unsigned short        m_version : 6;
	unsigned short        m_length : 10;
	unsigned char         m_R : 2;
	unsigned char         m_A : 1;
	unsigned char         m_session_id :5;
	unsigned char         m_enc_type : 3;
	unsigned char         m_padding : 5;
	unsigned char         m_reserved[3];
	unsigned short        m_seq_num;
	unsigned short        m_header_checksum;
}protocal_sdk_uart_header;

#pragma pack()
typedef struct _VO_OUTPUT
{
	short cnt;

	short vx;
	short vy;
	short vz;

	float x;
	float y;
	float z;

	float reserved[12];

	float height;
	float uncertainty_height;

	unsigned char reserve[4];
	
	unsigned int  m_frame_index;
	unsigned int  m_time_stamp;
	unsigned int  m_reserved[9];
}VO_OUTPUT;

typedef struct
{
	u8 gyro_ok;
	u8 acc_ok;
	u8 mag_ok;
	u8 baro_ok;
	u8 gps_ok;
	u8 sonar_ok;
	u8 tof_ok;
	u8 of_ok;
} _sensor_hd_check_st;

extern uint8_t 	OF_STATE,OF_QUALITY;
extern int8_t	OF_DX,OF_DY;
extern int16_t	OF_DX2,OF_DY2,OF_DX2FIX,OF_DY2FIX;
extern uint16_t	OF_ALT,OF_ALT2;
extern int16_t	OF_GYR_X,OF_GYR_Y,OF_GYR_Z;
extern int16_t	OF_GYR_X2,OF_GYR_Y2,OF_GYR_Z2;
extern int16_t	OF_ACC_X,OF_ACC_Y,OF_ACC_Z;
extern int16_t	OF_ACC_X2,OF_ACC_Y2,OF_ACC_Z2;
extern float	OF_ATT_ROL,OF_ATT_PIT,OF_ATT_YAW;
extern float	OF_ATT_S1,OF_ATT_S2,OF_ATT_S3,OF_ATT_S4;

/* Exported functions */
void Guidance_DT_Data_Receive_Prepare(u8 data);
void GuidancedataProcess(void);
void AnoOF_GetOneByte(uint8_t data);
void AnoOF_Check(u8 dT_ms);
void AnoOF_DataAnl(uint8_t *data_buf,uint8_t num);

#endif
