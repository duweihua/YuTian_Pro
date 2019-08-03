#include "program_ctrl.h"
#include "myMath.h"
#include "speed_estimator.h"
#include "height_control.h"
#include "gcs.h"
#include "Ano_OF.h"

//������ֲӳ���
//=====mapping=====
//STATE
#define MOTOR_PREPARATION    (1)
#define FC_STATE_FLYING      (fc_state_take_off)
#define FC_STATE_UNLOKED     (g_FMUflg.unlock)
#define RC_STATE_FAILSAFE    (0)
#define FC_STATE_FLIGHT_MODE (g_UAVinfo.UAV_Mode >= Altitude_Hold)//��ǰΪ����ģʽ
//EN
#define FC_STATE_POS_HOLD_EN (ANO_OF.STATE.of_fus)                //������Ч��ʹ�ܶ��㡣
#define AUTO_TAKE_OFF_EN     (g_UAVinfo.UAV_Mode >= Altitude_Hold) //��ǰΪ����ģʽ��ʹ���Զ���ɹ���

//SET_DATA
//�Զ����Ŀ��߶�
//#define EXP_TAKEOFF_ALT_CM   (80)   //CM

//�Զ���ɵ��ٶ�
#define AUTO_TAKE_OFF_SPEED  (100)  //CMPS

//�Զ������ٶ�
#define AUTO_LAND_SPEED      (30)   //CMPS
#define MAX_VELOCITY_XY      (200)  //CMPS
#define MAX_VELOCITY_ZP      (200)  //CMPS
#define MAX_VELOCITY_ZN      (150)  //CMPS
#define MAX_PAL_YAW          (100)  //DPS

//STA_DATA
#define ALT_CM             (HeightInfo.Z_Postion)
//FUNCTION
#define FC_CMD_UNLOCK      (FC_Unlock_Fun())
#define FC_CMD_LOCK        (g_FMUflg.unlock = 0)
//=====mapping=====

static void FC_Unlock_Fun()
{
    g_FMUflg.unlock = 1;
}

StandardControl_t StandardControl;
_program_ctrl_st program_ctrl;
_atol_sta_enum auto_takeoff_land_state;

static u8 one_key_takeoff_f;

void One_Key_Takeoff()
{
    //����
    FC_CMD_UNLOCK ;
    //�����Ҫ���
    if(one_key_takeoff_f == 0)
    {
        one_key_takeoff_f = 1;
    }
}


void One_Key_Land()
{
    //
    if(auto_takeoff_land_state == AUTO_TAKE_OFF_FINISH)
    {
        auto_takeoff_land_state = AUTO_LAND ;
    }
}


extern s32 EXP_TAKEOFF_ALT_CM;
	
void One_Key_Take_off_Land_Ctrl_Task(u8 dT_ms)
{
    static s32 ref_height;
    static s16 exp_time_ms,runtime_ms;
    static s16 time_compensate_ms;
    
    //������ͨ��һ����ɺ�����λ
    if(one_key_takeoff_f != 0)
    {
        //���׼���ã����ʱ��׼����
        if(MOTOR_PREPARATION)
        {
            //�ж��Զ���ɽ���״̬�Ƿ�Ϊ�Զ��з�null
            if(auto_takeoff_land_state == AUTO_TAKE_OFF_NULL)
            {
                //����ǣ�������Զ����״̬
                auto_takeoff_land_state = AUTO_TAKE_OFF;
            }                
        }
    }
    else    //
    {
        //����Զ����״̬Ϊ��ʼ���ҷɻ��Ѿ���ɣ�����Ϊ�Ѿ������ɣ��ֶ���ɣ�
        if(auto_takeoff_land_state == AUTO_TAKE_OFF_NULL && FC_STATE_FLYING != 0)
        {
            auto_takeoff_land_state = AUTO_TAKE_OFF_FINISH;
        }
    }
    
    //==state and ctrl
    if(auto_takeoff_land_state == AUTO_TAKE_OFF_NULL)
    {
        //
        ref_height = ALT_CM;
        //
        runtime_ms = 0;
        //
        program_ctrl.auto_takeoff_land_velocity = 0;
    }
    //
    else if(auto_takeoff_land_state == AUTO_TAKE_OFF)
    {
        //
        exp_time_ms = (EXP_TAKEOFF_ALT_CM - ref_height) * 1000 / AUTO_TAKE_OFF_SPEED;
        //�������ʱ������Ϊ0-5��
        exp_time_ms = LIMIT(exp_time_ms,0,5000);
        //
        runtime_ms += dT_ms;
        //
        program_ctrl.auto_takeoff_land_velocity = AUTO_TAKE_OFF_SPEED;
        //
        if(runtime_ms>exp_time_ms)
        {
            //
            runtime_ms = 0;
            //
            auto_takeoff_land_state = AUTO_TAKE_OFF_FINISH;
        }
    }
    //
    else if(auto_takeoff_land_state == AUTO_TAKE_OFF_FINISH)
    {
        //
        program_ctrl.auto_takeoff_land_velocity = 0;
    }
    else if(auto_takeoff_land_state == AUTO_LAND)
    {
        //
        program_ctrl.auto_takeoff_land_velocity = -AUTO_LAND_SPEED;
    }
    //==reset
    if(FC_STATE_UNLOKED == 0)
    {
        //
        program_ctrl.auto_takeoff_land_velocity = 0;
        //
        auto_takeoff_land_state = AUTO_TAKE_OFF_NULL ;
        //
        one_key_takeoff_f = 0;
    }
}


u16 val, spd;

void Program_Ctrl_DataAnl(u8 *data)
{

}


static u8 cmd_take_off_f;

void Program_Ctrl_Task(u8 dT_ms)
{
    if(program_ctrl.cmd_state[0] != program_ctrl.cmd_state[1])
    {
        //ָ����£���λ����,ֹ֮ͣǰ����
        FlyCtrlReset();
        //
        if(!RC_STATE_FAILSAFE && FC_STATE_FLIGHT_MODE && FC_STATE_POS_HOLD_EN)
        {
            program_ctrl.state_ok = 1;
        }
        else
        {
            program_ctrl.state_ok = 0;
                
						//�����ַ���

            //��λָ��״̬
            program_ctrl.cmd_state[0] = 0;
        }
    }
    
    switch(program_ctrl.cmd_state[0])
    {
        case (0x01)://���
						{   
								if(program_ctrl.state_ok != 0)
								{
										//���״̬Ϊ��ʼ״̬����ң�����ź�,�ҹ�������GPS��Ч
										if(auto_takeoff_land_state == AUTO_TAKE_OFF_NULL)
										{
												if(cmd_take_off_f == 0)
												{
														//
														cmd_take_off_f = 1;
													
														//�����ַ���
														//ANO_DT_SendString("Take off!");
													
														//һ�����
														One_Key_Takeoff();
												}
										}
										else if(auto_takeoff_land_state == AUTO_TAKE_OFF_FINISH)
										{
												//�����ַ���
												//ANO_DT_SendString("Take off OK!");
											
												//��λָ��״̬
												program_ctrl.cmd_state[0] = 0;
										}
										else if(auto_takeoff_land_state > AUTO_TAKE_OFF_FINISH)
										{
												//�����ַ���
												//ANO_DT_SendString("CMD Error!");
											
												//��λָ��״̬
												program_ctrl.cmd_state[0] = 0;                
										}
								}
						}
						break;
				
        case (0x02):    //����
						{
								if(auto_takeoff_land_state == AUTO_TAKE_OFF_FINISH)
								{
										//��λָ��״̬
										//ANO_DT_SendString("Landing!");
									
										//һ������
										One_Key_Land();
								}
								else if(auto_takeoff_land_state == AUTO_TAKE_OFF_NULL)
								{
										//�����ַ���
										//ANO_DT_SendString("Landing OK!");
									
										//
										program_ctrl.cmd_state[0] = 0;
								}
						}
						break;
				
        case (0xA0):    //����ͣ��
						{
								if(FC_STATE_UNLOKED)
								{
										//
										//ANO_DT_SendString("Emergency stop OK!");
									
										//����
										FC_CMD_LOCK;
									
										//
										program_ctrl.cmd_state[0] = 0;
								}
						}
						break;
				
        case (0x0B):    //ˮƽ����
						break;
        
        default:
						break;
    }
    
    //��λ����
    if(program_ctrl.cmd_state[0] == 0)
    {
        FlyCtrlReset();
    }
    
    //��¼��ʷֵ
    program_ctrl.cmd_state[1] = program_ctrl.cmd_state[0];
    
    //��������������
    program_ctrl.vel_cmps_h[Z] = program_ctrl.vel_cmps_ref[Z];
    program_ctrl.vel_cmps_h[X] = program_ctrl.vel_cmps_ref[X];
    program_ctrl.vel_cmps_h[Y] = program_ctrl.vel_cmps_ref[Y];
}


void StdControl(StandardControl_t *ptrS)
{
    for(int i = 0;i<NumofVectorDir;i++)
    {
        StandardControl.StandardControlDirction[i].Exp_Time = (StandardControl.StandardControlDirction[i].Distance * 1000) / LIMIT(StandardControl.StandardControlDirction[i].Speed,-MAX_VELOCITY_XY,MAX_VELOCITY_XY);
        StandardControl.StandardControlDirction[i].Cnt = 0;
    }
}


void UpdateCMD(uint16_t Distance, uint16_t Speed, uint8_t Cmd)
{
    val = Distance;
    spd = Speed;
    program_ctrl.cmd_state[0] = Cmd;
}


void FlyCtrlReset()
{
    //
    cmd_take_off_f = 0;
    //
    for(u8 i = 0;i<4;i++)
    {
        if(i<4)
        {
            //
            program_ctrl.vel_cmps_ref[i] = 0;
            program_ctrl.vel_cmps_w[i] = 0;
            program_ctrl.vel_cmps_h[i] = 0;
        }
        else
        {
            program_ctrl.yaw_pal_dps = 0;
        }
        
        program_ctrl.exp_process_t_ms[i] = 0;
        program_ctrl.fb_process_t_ms[i] = 0;
    }
}




















