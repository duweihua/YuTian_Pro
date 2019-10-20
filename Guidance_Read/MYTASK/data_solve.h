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

/* Exported functions */
void Guidance_DT_Data_Receive_Prepare(u8 data);
void GuidancedataProcess(void);

#endif
