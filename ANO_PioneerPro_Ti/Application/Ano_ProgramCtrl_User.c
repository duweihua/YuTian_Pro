#include "Ano_ProgramCtrl_User.h"
#include "Ano_Math.h"
#include "Ano_FlightCtrl.h"
#include "Ano_FlyCtrl.h"
#include <stdio.h>


#define MAX_PC_XYVEL_CMPS  200
#define MAX_PC_ZVEL_CMPS   150
#define MAX_PC_PAL_DPS     100


#define USER_TASK_NUM  3   //�û�������

void Program_Ctrl_User_Set_HXYcmps(float hx_vel_cmps,float hy_vel_cmps);
void Program_Ctrl_User_Set_YAWdps(float yaw_pal_dps);
void Program_Ctrl_User_Set_Zcmps(float z_vel_cmps);
void UserCtrlReset();

_pc_user_st pc_user;

_user_cntrl_word user_cntrl_word;//�û�������

/*-----------------------------------------------------------------------------*/
//�û�����

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
user_task_t user_task[USER_TASK_NUM]=  //�û������б�һ����ɺ��Զ����У�
{
  {user_task_1,10000,0,0},
  {user_task_2,10000,0,0},
  {user_task_3,4000,0,0},
};





//˳��ִ���û�����
static u8 task_index=0;//���ڼ�¼����ִ�е�����

void User_Ctrl(u32 dT_ms)
{
	
  if(user_cntrl_word.takeoff_en==1)
	{
			printf("take off\r\n");
			FlyCtrlReset();
			one_key_take_off();
			user_cntrl_word.takeoff_en=0;
			user_cntrl_word.user_task_running=1;//������ֹ���ֶ�ģʽ�½���usertask
		  task_index=0;
		
			for(int i=0;i<USER_TASK_NUM;i++)
			{
				user_task[i].end_flag=0;//����������������־λ
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
		static u32 running_time=0;//����������ʱ��
	  if(task_index<USER_TASK_NUM)	
		{
				if(user_task[task_index].start_flag==1)//ִ�е�task_index������
						{
							user_task[task_index].task_func(dT_ms);
							running_time+=dT_ms;//��¼�ۼ�ʱ��
						}
						
			  if(user_task[task_index].end_flag==1)//׼����ʼ��һ������
						{
							task_index++;
							running_time=0;
							FlyCtrlReset();
							UserCtrlReset();
						}
				if(running_time==0)//������task_index������
						{
							user_task[task_index].start_flag=1;
						}
				else if(running_time>=user_task[task_index].run_time)//�жϵ�ǰ�����Ƿ����
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



//=====1������ˮƽ����ϵ�̿��ٶȹ��ܽӿں���=====
/**********************************************************************************************************
*�� �� ��: Program_Ctrl_User_Set_HXYcmps
*����˵��: �̿ع��ܣ�����ˮƽ����ϵ���ٶ��趨��ʵʱ���ƣ�
*��    ��: X�ٶȣ�����ÿ�룬��Ϊǰ������Ϊ���ˣ�Y�ٶȣ�����ÿ�룬��Ϊ���ƣ���Ϊ���ƣ�
*�� �� ֵ: ��
**********************************************************************************************************/
void Program_Ctrl_User_Set_HXYcmps(float hx_vel_cmps,float hy_vel_cmps)
{
	//
	pc_user.vel_cmps_set_h[0] = hx_vel_cmps;
	pc_user.vel_cmps_set_h[1] = hy_vel_cmps;
	//����XY�ٶ�ģ��
	length_limit(&pc_user.vel_cmps_set_h[0],&pc_user.vel_cmps_set_h[1],MAX_PC_XYVEL_CMPS,pc_user.vel_cmps_set_h);
}


//=====2����ͷģʽ�ο�����ϵ�̿ع��ٶ��ܽӿں��������ޣ��Ժ󿪷������߲ο���λ���̿ع��ܣ�=====
//
//
//

//=====3��ͨ�ó̿��ٶȹ��ܽӿں���=====
/**********************************************************************************************************
*�� �� ��: Program_Ctrl_User_Set_WHZcmps
*����˵��: �̿ع��ܣ������½��ٶ��趨��ʵʱ���ƣ�
*��    ��: �ٶȣ�����ÿ�룬��Ϊ��������Ϊ�½�)
*�� �� ֵ: ��
**********************************************************************************************************/
void Program_Ctrl_User_Set_Zcmps(float z_vel_cmps)
{
	//
	pc_user.vel_cmps_set_z = z_vel_cmps;
	//�޷�
	pc_user.vel_cmps_set_z = LIMIT(pc_user.vel_cmps_set_z,-MAX_PC_ZVEL_CMPS,MAX_PC_ZVEL_CMPS);
}
/**********************************************************************************************************
*�� �� ��: Program_Ctrl_User_Set_YAWdps
*����˵��: �̿ع��ܣ������ٶ��趨��ʵʱ���ƣ�
*��    ��: �ٶȣ���ÿ�룬��Ϊ��ת����Ϊ��ת��
*�� �� ֵ: ��
**********************************************************************************************************/
void Program_Ctrl_User_Set_YAWdps(float yaw_pal_dps)
{
	//
	pc_user.pal_dps_set = yaw_pal_dps;
	//�޷�
	pc_user.pal_dps_set = LIMIT(pc_user.pal_dps_set,-MAX_PC_PAL_DPS,MAX_PC_PAL_DPS);
}


void UserCtrlReset()
{
	 Program_Ctrl_User_Set_HXYcmps(0,0);
	 Program_Ctrl_User_Set_YAWdps(0);
	 Program_Ctrl_User_Set_Zcmps(0);
}

