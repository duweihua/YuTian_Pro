#ifndef __ANO_PROGRAMCTRL_USER_H
#define __ANO_PROGRAMCTRL_USER_H

//==引用
#include "sysconfig.h"
#include "Ano_FcData.h"

//==定义
typedef struct
{
	//
	float vel_cmps_set_h[2];
	float vel_cmps_set_w[2];
	float vel_cmps_set_ref[2];
	//
	float vel_cmps_set_z;
	float pal_dps_set;
}_pc_user_st;
extern _pc_user_st pc_user;

//==数据声明

typedef struct
{
 u8 takeoff_en;
 u8 land_en;
 u8 user_task_running;
 u8 user_task_end;
 u8 turn_left_en;
 u8 turn_right_en;
}_user_cntrl_word;



typedef struct//用户任务结构体
{
  void(*task_func)(u32 dT_us);
	u32 run_time;//要执行的时间ms
	u8  start_flag;
	u8  end_flag;
}user_task_t;

extern _user_cntrl_word user_cntrl_word;
//==函数声明

//static
void User_Ctrl(u32 dT_us);

//public





#endif
