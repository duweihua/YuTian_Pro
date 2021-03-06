#include "Ano_ProgramCtrl_User.h"
#include "Ano_Math.h"
#include "Ano_FlightCtrl.h"
#include "Ano_FlyCtrl.h"
#include <stdio.h>


#define MAX_PC_XYVEL_CMPS  200
#define MAX_PC_ZVEL_CMPS   150
#define MAX_PC_PAL_DPS     100


#define USER_TASK_NUM  3   //用户任务数

void Program_Ctrl_User_Set_HXYcmps(float hx_vel_cmps,float hy_vel_cmps);
void Program_Ctrl_User_Set_YAWdps(float yaw_pal_dps);
void Program_Ctrl_User_Set_Zcmps(float z_vel_cmps);
void UserCtrlReset();

_pc_user_st pc_user;

_user_cntrl_word user_cntrl_word;//用户控制字

/*-----------------------------------------------------------------------------*/
//用户任务

void user_task_1(u32 dT_us)
{
//	static u32 t=0;
//    printf("t1=%d\r\n",t++);
}

void user_task_2(u32 dT_us)
{
 Program_Ctrl_User_Set_HXYcmps(20,0);
}


void user_task_3(u32 dT_us)
{
	Program_Ctrl_User_Set_YAWdps(45);
}


/*------------------------------------------------------------------------------*/
user_task_t user_task[USER_TASK_NUM]=  //用户任务列表（一键起飞后自动进行）
{
  {user_task_1,10000,0,0},
  {user_task_2,10000,0,0},
  {user_task_3,4000,0,0},
};





//顺序执行用户任务
static u8 task_index=0;//用于记录正在执行的任务

void User_Ctrl(u32 dT_ms)
{
	
  if(user_cntrl_word.takeoff_en==1)
	{
			printf("take off\r\n");
			FlyCtrlReset();
			one_key_take_off();
			user_cntrl_word.takeoff_en=0;
			user_cntrl_word.user_task_running=1;//用来防止纯手动模式下进入usertask
		  task_index=0;
		
			for(int i=0;i<USER_TASK_NUM;i++)
			{
				user_task[i].end_flag=0;//清空所有任务结束标志位
			}
	}
	else if(user_cntrl_word.land_en)
	{
			printf("land\r\n");
			FlyCtrlReset();
			one_key_land();
			user_cntrl_word.land_en=0;
	}
	else if(flag.flying==1&&user_cntrl_word.user_task_running)
	{
		static u32 running_time=0;//任务已运行时间
	  if(task_index<USER_TASK_NUM)	
		{
				if(user_task[task_index].start_flag==1)//执行第task_index个任务
						{
							user_task[task_index].task_func(dT_ms);
							running_time+=dT_ms;//记录累计时间
						}
						
			  if(user_task[task_index].end_flag==1)//准备开始下一个任务
						{
							task_index++;
							running_time=0;
							FlyCtrlReset();
							UserCtrlReset();
						}
				if(running_time==0)//启动第task_index个任务
						{
							user_task[task_index].start_flag=1;
						}
				else if(running_time>=user_task[task_index].run_time)//判断当前任务是否完成
						{
							user_task[task_index].start_flag=0;
							user_task[task_index].end_flag=1;
						}
		}
		else
		{
		  user_cntrl_word.land_en=1;
			user_cntrl_word.user_task_running=0;
			UserCtrlReset();
		} 
	}
}



//=====1、航向水平坐标系程控速度功能接口函数=====
/**********************************************************************************************************
*函 数 名: Program_Ctrl_User_Set_HXYcmps
*功能说明: 程控功能，航向水平坐标系下速度设定（实时控制）
*形    参: X速度（厘米每秒，正为前进，负为后退，Y速度（厘米每秒，正为左移，负为右移）
*返 回 值: 无
**********************************************************************************************************/
void Program_Ctrl_User_Set_HXYcmps(float hx_vel_cmps,float hy_vel_cmps)
{
	//
	pc_user.vel_cmps_set_h[0] = hx_vel_cmps;
	pc_user.vel_cmps_set_h[1] = hy_vel_cmps;
	//限制XY速度模长
	length_limit(&pc_user.vel_cmps_set_h[0],&pc_user.vel_cmps_set_h[1],MAX_PC_XYVEL_CMPS,pc_user.vel_cmps_set_h);
}


//=====2、无头模式参考坐标系程控功速度能接口函数（暂无，稍后开发，或者参考上位机程控功能）=====
//
//
//

//=====3、通用程控速度功能接口函数=====
/**********************************************************************************************************
*函 数 名: Program_Ctrl_User_Set_WHZcmps
*功能说明: 程控功能，上升下降速度设定（实时控制）
*形    参: 速度（厘米每秒，正为上升，负为下降)
*返 回 值: 无
**********************************************************************************************************/
void Program_Ctrl_User_Set_Zcmps(float z_vel_cmps)
{
	//
	pc_user.vel_cmps_set_z = z_vel_cmps;
	//限幅
	pc_user.vel_cmps_set_z = LIMIT(pc_user.vel_cmps_set_z,-MAX_PC_ZVEL_CMPS,MAX_PC_ZVEL_CMPS);
}
/**********************************************************************************************************
*函 数 名: Program_Ctrl_User_Set_YAWdps
*功能说明: 程控功能，航向速度设定（实时控制）
*形    参: 速度（度每秒，正为右转，负为左转）
*返 回 值: 无
**********************************************************************************************************/
void Program_Ctrl_User_Set_YAWdps(float yaw_pal_dps)
{
	//
	pc_user.pal_dps_set = yaw_pal_dps;
	//限幅
	pc_user.pal_dps_set = LIMIT(pc_user.pal_dps_set,-MAX_PC_PAL_DPS,MAX_PC_PAL_DPS);
}


void UserCtrlReset()
{
	 Program_Ctrl_User_Set_HXYcmps(0,0);
	 Program_Ctrl_User_Set_YAWdps(0);
	 Program_Ctrl_User_Set_Zcmps(0);
}

