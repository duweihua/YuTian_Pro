/*
本程序完成任务：
1. 按键Key1 执行基本任务1：A2->B3->C1->D2->A2
2. 按键Key2 执行基本任务2：A2->A3->D3->D1->A1->A2->C2->A2
*/

#include "FollowLine.h"
#include "HARDWARE_uart.h"
#include "stdbool.h"
#include "pid.h"
#include "timer_drv.h"
#include "myMath.h"
#include "gcs.h"
#include "program_ctrl.h"
#include "led.h"
#include "Ano_OF.h"

extern Usart_t UsartGroup[Num_USART];
extern PIDInfo_t PIDGroup[emNum_Of_PID_List];
extern UAV_info_t g_UAVinfo;
extern u16 val, spd;
extern _program_ctrl_st program_ctrl;
bool Key1 = false;
bool Key2 = false;

void proControl(int16_t Distance,int16_t Speed);
void TimeoutCheck(void);
void UpdateStatus(void);
void UpdateAction(float dt);
void UpdateButton(void);
void ActionHoldPoint_Judge(int8_t Err, int16_t HoldTime, FSMList_t NextAction);
void ActionHoldPoint_NoJudge(int8_t Err, int16_t HoldTime, FSMList_t NextAction);
void ActionHoldPoint_Judge_Back(int8_t Err, int16_t HoldTime, FSMList_t NextAction);
bool ActionFormChange_Clear(int8_t HoldTime, FormType_t TargetFormType, FSMList_t NextAction);
bool ActionFormChange_NoClear(int8_t HoldTime, FormType_t TargetFormType, FSMList_t NextAction);
void UpdateDebugInfo(void);
void HoldCurrentPostion(float dt);
void HoldCurrentPostion_side(float dt);
void HoldCurrentPostion_Angle(float dt);


FollowManager_t FollowManager;
SonarManager_t SonarManager;

/*
        |+X
        |
        |
+Y------------- -Y
        |
        |
        |-X
*/

void Follow_Init()
{
    FollowManager.ptrPIDInfoH = &PIDGroup[emPID_FolloLinePosHorizontally];
    FollowManager.ptrPIDInfoV = &PIDGroup[emPID_FolloLinePosVertically];

		FollowManager.ptrPIDInfoH->kp = 1.5f;//0.1f
    FollowManager.ptrPIDInfoH->kd = 0.0f;

		FollowManager.ptrPIDInfoV->kp = 1.5f;
    FollowManager.ptrPIDInfoV->kd = 0.0f;

		FollowManager.ptrPIDInfoH->DeathArea = 3;
		FollowManager.ptrPIDInfoV->DeathArea = 3;

		PIDGroup[emPID_FolloLineSpdHorizontally].kp = 0.45f;//0.45
//    PIDGroup[emPID_FolloLineSpdHorizontally].ki = 0.13f;
    PIDGroup[emPID_FolloLineSpdHorizontally].kd = 0.014f;

    PIDGroup[emPID_FolloLineSpdVertically].kp = 0.45f;//0.45
//    PIDGroup[emPID_FolloLineSpdVertically].ki = 0.13f;
    PIDGroup[emPID_FolloLineSpdVertically].kd = 0.014f;

//		PIDGroup[emPID_FolloLineOpenMV].desired = 50;
    PIDGroup[emPID_FolloLinePosHorizontally].desired = 160/2;
    PIDGroup[emPID_FolloLinePosVertically].desired = 120/2;

		FollowManager.ptrPIDInfoH->OutLimitHigh = 20;
    FollowManager.ptrPIDInfoH->OutLimitLow = -20;
    FollowManager.ptrPIDInfoV->OutLimitHigh = 20;
    FollowManager.ptrPIDInfoV->OutLimitLow = -20;

		FollowManager.CountDownNumMs = MAX_COUNTDOWN;
    FollowManager.TargetAltitudeCM = TARGETALTITUDECM;

    FollowManager.ptrFrame = (OpenMVFrame_t *)UsartGroup[UART_A3].RxBuff;
    FollowManager.ptrUAVInfo = &g_UAVinfo;

    P1DIR &= ~(1 << BIT1);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    P1DIR &= ~(1 << BIT4);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);

    for (int i = 0; i < 3; i++)
    {
        StandardControl.StandardControlDirction[i].Speed = 0;
    }
}


extern _ano_of_st ANO_OF;
extern float pos_out_trans[2];
uint16_t find_yellowblock_flag = 0;
uint16_t inflag = 0;

//以100hz的速度轮询 10ms
void UpdateCentControl(float dt)
{
    if (FollowManager.ptrFrame->CentPoint.x1 > 200 || FollowManager.ptrFrame->CentPoint.y1 > 200)
        return;
		
		if(FollowManager.ptrFrame->FormType == YellowBlock)
		{
				static int cntup = 0;
				
				cntup++;
			
				if(cntup%200 == 0)
				{
						g_LedManager.emLEDStatus = StatusOn;
				}
				else if(cntup%100 == 0)
				{
						g_LedManager.emLEDStatus = StatusOff;
				}
		}
		else if(FollowManager.ptrFrame->FormType == HorizontalLine)
		{
				g_LedManager.emLEDStatus = StatusOn;
		}
		else
		{
				g_LedManager.emLEDStatus = StatusOff;
		}
		
		if(FollowManager.ptrFrame->FormType == YellowBlock)
		{
				MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN6);
		}
		else
		{
				MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN6);
		}
				
		//解放遥控器部分
    if(ANO_OF.STATE.of_fus != 0 && g_UAVinfo.UAV_Mode == AutoMode)
    {
        PIDGroup[emPID_Pitch_Pos].desired = pos_out_trans[0];     //将位置控制输出为飞行角度的期望值  
        PIDGroup[emPID_Roll_Pos].desired  = pos_out_trans[1];
    }

    UpdateButton();
    UpdateStatus();
    UpdateAction(dt);
}


extern UAV_info_t g_UAVinfo;								//飞行器状态结构体
extern FollowManager_t FollowManager;				//程控功能控制结构体
extern LedManager_t g_LedManager;						//LED控制结构体
extern float PIDGroup_desired_yaw_pos_tmp;	//Yaw轴角度调整量
s32 EXP_TAKEOFF_ALT_CM = 75;								//自动起飞高度(倒计时结束可以区分任务赋值)
uint16_t forward_time = 700;
uint16_t judge_flag = 0;

void UpdateStatus()
{
		static uint16_t forward_prepare_cnt = 0;
		static uint16_t forward_cnt = 0;
		static uint16_t forward_again_cnt = 0;
		static uint16_t forward_third_cnt = 0;
		static uint16_t right_cnt = 0;
		static uint16_t forward_thrid_cnt = 0;
		static uint16_t left_cnt = 0;
		static uint16_t turn_cnt = 0;
		static uint16_t forward_forth_cnt = 0;
		static uint16_t hold_time = 0;
		static uint16_t up_cnt = 0;
		static uint16_t forward_add_cnt = 0;
		static uint16_t yellow_photo_cnt = 0;
		static uint16_t apriltag_photo_cnt = 0;
		static uint16_t forward_prepare_cnt2 = 0;
	
    switch (FollowManager.ActionList)
    {
        case ActionWaitting:
            break;

        case ActionCountdown:
						{
								FollowManager.CountDownNumMs--;

								if (FollowManager.CountDownNumMs <= 0)
								{
										FollowManager.ActionList = ActionTakeOff;
								}
						}
						break;

        case ActionTakeOff:
						{
								if((Key1 == true)&&(Key2 == false))
								{
										ActionHoldPoint_NoJudge(MAX_HOVER_ERR,Take_off_Time,ActionHoverStartPoint);
								}
								else if((Key2 == true)&&(Key1 == false))
								{
										ActionHoldPoint_NoJudge(MAX_HOVER_ERR,Take_off_Time,ActionHoldRedPoint);
								}
						}
						break;

				case ActionHoldRedPoint:
						{
								ActionHoldPoint_NoJudge(MAX_HOVER_ERR,1800,ActionLand);
						}
						break;
						
        case ActionHoverStartPoint:
						{
								ActionHoldPoint_NoJudge(MAX_HOVER_ERR,Take_off_Hold_Time,ActionGoForward_Prepare);
						}
            break;
						
				case ActionGoForward_Prepare:
						{
								forward_prepare_cnt++;
							
								if(forward_prepare_cnt >= 400)
								{
										forward_prepare_cnt = 400;
										
										FollowManager.ActionList = ActionGoLeft_Prepare;
								}
						}
						break;
																
				
						
				case ActionGoLeft_Prepare:
						{
								ActionFormChange_NoClear(Middle_Formchange_Time,HorizontalLine,ActionGoForward);
						}
						break;
						
        case ActionGoForward:
						{
								ActionFormChange_NoClear(Short_Formchange_Time,YellowBlock,ActionHoldYellowBlock);
						}
            break;
	
				case ActionHoldYellowBlock:
						{							
								ActionHoldPoint_NoJudge(MAX_HOVER_ERR,300,ActionGoForward_Second);
							
								yellow_photo_cnt++;
							
								if(yellow_photo_cnt == 20)
								{
										MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN4);//拉高拍照
								}
								if(yellow_photo_cnt == 70)
								{
										MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN4);//拉低准备
								}
								if(yellow_photo_cnt == 120)
								{
										MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN4);//拉高拍照
								}
								if(yellow_photo_cnt == 150)
								{
										MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN4);//拉低准备
								}
								if(yellow_photo_cnt == 200)
								{
										MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN4);//拉高拍照
								}
								if(yellow_photo_cnt == 250)
								{
										MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN4);//拉低结束
								}
						}
            break;
	
        case ActionGoForward_Second:
						{
								ActionFormChange_NoClear(Middle_Formchange_Time,VerticalPole,ActionHoldVerticalPole);
						}
            break;
						
				case ActionHoldVerticalPole:
						{
								ActionHoldPoint_NoJudge(MAX_HOVER_ERR,300,ActionGoForward_Again);
							
								apriltag_photo_cnt++;
							
								if(apriltag_photo_cnt == 20)
								{
										MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN4);//拉高拍照
								}
								if(apriltag_photo_cnt == 70)
								{
										MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN4);//拉低准备
								}
								if(apriltag_photo_cnt == 120)
								{
										MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN4);//拉高拍照
								}
								if(apriltag_photo_cnt == 150)
								{
										MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN4);//拉低准备
								}
								if(apriltag_photo_cnt == 200)
								{
										MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN4);//拉高拍照
								}
								if(apriltag_photo_cnt == 250)
								{
										MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN4);//拉低结束
								}
						}
            break;

        case ActionGoForward_Again:
						{
								forward_again_cnt++;
								
								if(forward_again_cnt >= 400)
								{
										forward_again_cnt = 400;
										
										FollowManager.ActionList = ActionGoLeft;
								}
						}
            break;
						
				case ActionGoLeft:
						{
								left_cnt++;
								
								if(left_cnt >= 350)
								{
										left_cnt = 350;
										
										FollowManager.ActionList = ActionTurnLeft;
								}
						}
            break;

				case ActionTurnLeft:
						{
								turn_cnt++;
								
								if(turn_cnt>=900)
								{
										left_cnt = 900;
										
										FollowManager.ActionList = ActionHoldsometime;
								}
						}
            break;
				
				case ActionHoldsometime:
						{
								hold_time++;
								
								if(hold_time >= 300)
								{
										hold_time = 300;
										
										FollowManager.ActionList = ActionGoForward_Add;
								}
						}
						break;
						
				case ActionGoForward_Add:
						{
								forward_add_cnt++;
							
								if(forward_add_cnt >= 400)
								{
										forward_add_cnt = 400;
									
										FollowManager.ActionList = ActionGoForward_Third;
								}
						}
						break;
					
				case ActionGoUp:
						{
								up_cnt++;
							
								if(up_cnt >= 200)
								{
										up_cnt = 200;
										
										FollowManager.ActionList = ActionGoForward_Third;
								}
						}
						break;
						
				case ActionGoForward_Third:
						{
								forward_third_cnt++;
								
								if(forward_third_cnt>=300)
								{
										forward_third_cnt = 300;
									
										judge_flag = 1;
									
										ActionFormChange_NoClear(Short_Formchange_Time,YellowBlock,ActionHoldYellowBlock_Again);
								}
						}
            break;
							
				case ActionHoldYellowBlock_Again:
						{
								FollowManager.ActionList = ActionGoForward_Forth;
						}
						break;
						
				case ActionGoForward_Forth:
						{
								ActionFormChange_NoClear(Long_Formchange_Time,VerticalPole,ActionHoldVerticalPole_Again);
						}
            break;
					
				case ActionHoldVerticalPole_Again:
						{
								ActionHoldPoint_NoJudge(MAX_HOVER_ERR,500,ActionGoRight);
						}
						break;
						
				case ActionGoRight:
						{
								right_cnt++;
								
								if(right_cnt>=200)
								{
										right_cnt = 200;
										
										FollowManager.ActionList = ActionLand;
								}
						}
            break;
						
        case ActionLand:
						{
								static int Cnt = MAX_TIMEOUT1;
								if (Cnt-- < 0)
										FollowManager.ActionList = ActionLock;
						}
						break;

        case ActionLock:
						{
								FollowManager.ActionList = ActionWaitting;
						}
						break;

        default:
            break;
    }
}


extern float pos_velocity_err_i[2];

void UpdateAction(float dt)
{
    switch (FollowManager.ActionList)
    {
				case ActionWaitting:
						break;

				case ActionTakeOff:
						UpdateCMD(0,0,CmdTakeOff);
						break;

				case ActionHoverStartPoint://零速度悬停
						{
								program_ctrl.vel_cmps_h[Y] = 0;
								program_ctrl.vel_cmps_h[X] = 0;
						}
						break;
						
				case ActionGoForwardLeft_Prepare:
						{
								program_ctrl.vel_cmps_h[Y] = 7;
								program_ctrl.vel_cmps_h[X] = 20;
						}
						break;
						
				case ActionHoldVerticalPole_Prepare:
						{
								if(FollowManager.ptrFrame->FormType == VerticalPole)
								{
										PIDGroup[emPID_FolloLinePosVertically].desired = 160/2 + FollowLine_Offset;
										HoldCurrentPostion_side(dt);
								}
								else if(FollowManager.ptrFrame->FormType == HorizontalLine)
								{
										program_ctrl.vel_cmps_h[X] = 0;
										program_ctrl.vel_cmps_h[Y] = 0;
								}
								else
								{
										program_ctrl.vel_cmps_h[X] = 12;
										program_ctrl.vel_cmps_h[Y] = 0;
								}
						}
						break;
						
				case ActionGoForward_Prepare2:
						{
									program_ctrl.vel_cmps_h[Y] = 0;
									program_ctrl.vel_cmps_h[X] = 20;
						}					
						break;

						
				case ActionHoldRedPoint:
						{
								if(FollowManager.ptrFrame->FormType == RedPoint)
								{
										//设置期望值 
										PIDGroup[emPID_FolloLinePosVertically].desired = 120/2;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160/2;
										
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;
												
				case ActionGoForward_Prepare:
						{
								program_ctrl.vel_cmps_h[X] = 20;
								program_ctrl.vel_cmps_h[Y] = 0;
						}
						break;
				
				case ActionGoLeft_Prepare:
						{
								program_ctrl.vel_cmps_h[X] = 0;
								program_ctrl.vel_cmps_h[Y] = +15;						
						}
						break;
						
				case ActionGoForward:
				case ActionGoForward_Second:
				case ActionGoForward_Forth:
						{
								program_ctrl.vel_cmps_h[X] = 15;	//前进速度
							
								if(FollowManager.ptrFrame->FormType == HorizontalLine)
								{
										HoldCurrentPostion_Angle(dt);
								}
								else if(FollowManager.ptrFrame->FormType == YellowBlock)
								{
										program_ctrl.vel_cmps_h[Y] = 0;
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;

//										PIDGroup[emPID_Roll_Pos].desired = -6.0f;		
								}
						}
						break;
						
				case ActionGoForward_Again:
						{
								program_ctrl.vel_cmps_h[X] = 20;	
								program_ctrl.vel_cmps_h[Y] = 0;	
						}
						break;
						
				case ActionGoForward_Third:
				case ActionGoForward_Add:
						{
								program_ctrl.vel_cmps_h[X] = 15;	//前进速度
							
								if(FollowManager.ptrFrame->FormType == HorizontalLine)
								{
										HoldCurrentPostion_Angle(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;

										if(judge_flag == 1)
										{
//												PIDGroup[emPID_Roll_Pos].desired = -5.0f;
										}
								}
						}
						break;
			
				case ActionGoBack:
				case ActionGoBack_Again:
				case ActionGoBack_Third:
						{
								program_ctrl.vel_cmps_h[X] = -20;						//后
						}
						break;

				case ActionGoLeft:
				case ActionGoLeft_Again:
				case ActionGoLeft_Third:	
						{
								program_ctrl.vel_cmps_h[X] = 0;
								program_ctrl.vel_cmps_h[Y] = +20;						//左
						}
						break;
						
				case ActionGoRight:
				case ActionGoRight_Again:
				case ActionGoRight_Third:
						{
								program_ctrl.vel_cmps_h[Y] = -20;						//右	
								program_ctrl.vel_cmps_h[X] = 0;
						}
						break;
						
				case ActionGoUp:
				case ActionGoUp_Again:
				case ActionGoUp_Third:
						{
								program_ctrl.vel_cmps_h[Z] = +3;							//上
								program_ctrl.vel_cmps_h[X] = 0;
								program_ctrl.vel_cmps_h[Y] = 0;
						}
						break;
						
				case ActionGoDown:
				case ActionGoDown_Again:
				case ActionGoDown_Third:
						{
								program_ctrl.vel_cmps_h[Z] = -Straight_Down_Speed;						//下
						}
						break;
	
				case ActionTurnLeft:
				case ActionTurnLeft_Again:
				case ActionTurnLeft_Third:
						{
								PIDGroup_desired_yaw_pos_tmp += TurnLeft_Speed;					//左转
						}
						break;
					
				case ActionTurnRight:
				case ActionTurnRight_Again:
				case ActionTurnRight_Third:
						{
								PIDGroup_desired_yaw_pos_tmp -= TurnRight_Speed;				//右转
						}
						break;
						
				case ActionHoldsometime:
						{
								program_ctrl.vel_cmps_h[X] = 0;
								program_ctrl.vel_cmps_h[Y] = 0;
						}
						break;
						
        case ActionHoldYellowBlock:
				case ActionHoldYellowBlock_Again:
						{
//								if(FollowManager.ptrFrame->FormType == YellowBlock)
//								{
//										PIDGroup[emPID_FolloLinePosVertically].desired = 160/2 + FollowLine_Offset;
//										HoldCurrentPostion_side(dt);
//								}
//								else
//								{
//										program_ctrl.vel_cmps_h[X] = 0;
//										program_ctrl.vel_cmps_h[Y] = 0;

//										PIDGroup[emPID_Roll_Pos].desired = 0.0f;		
//								}
							
								if(FollowManager.ptrFrame->FormType == HorizontalLine)
								{
										HoldCurrentPostion_Angle(dt);
								}
								else
								{
//										pos_velocity_err_i[0] = 0; 
									  pos_velocity_err_i[1] = 0;
									
										program_ctrl.vel_cmps_h[X] = 0;
										program_ctrl.vel_cmps_h[Y] = 0;	
								}

//								program_ctrl.vel_cmps_h[X] = 0;
//								program_ctrl.vel_cmps_h[Y] = 0;
						}
						break;

        case ActionHoldVerticalPole:
						{
//								if(FollowManager.ptrFrame->FormType == VerticalPole)
//								{
//										PIDGroup[emPID_FolloLinePosVertically].desired = 160/2 + FollowLine_Offset;
//										HoldCurrentPostion_side(dt);
//								}
//								else if(FollowManager.ptrFrame->FormType == HorizontalLine)
//								{
//										program_ctrl.vel_cmps_h[X] = 15;
//								}
//								else
//								{
//										program_ctrl.vel_cmps_h[X] = 0;
//										program_ctrl.vel_cmps_h[Y] = 0;

//										PIDGroup[emPID_Roll_Pos].desired = 0.0f;		
//								}
							
//								pos_velocity_err_i[0] = 0; 
								pos_velocity_err_i[1] = 0;
							
								program_ctrl.vel_cmps_h[X] = 0;
								program_ctrl.vel_cmps_h[Y] = 0;
						}
						break;

				case ActionHoldVerticalPole_Again:
						{
//								if(FollowManager.ptrFrame->FormType == VerticalPole)
//								{
//										PIDGroup[emPID_FolloLinePosVertically].desired = 160/2 + FollowLine_Offset;
//										HoldCurrentPostion_side(dt);
//								}
//								else if(FollowManager.ptrFrame->FormType == HorizontalLine)
//								{
//										program_ctrl.vel_cmps_h[X] = 15;
//								}
//								else
//								{
//										program_ctrl.vel_cmps_h[X] = 0;
//										program_ctrl.vel_cmps_h[Y] = 0;

//										PIDGroup[emPID_Roll_Pos].desired = 0.0f;		
//								}
							
								pos_velocity_err_i[1] = 0;
							
								program_ctrl.vel_cmps_h[X] = 0;
								program_ctrl.vel_cmps_h[Y] = 0;
						}

				case ActionHoverStopPoint:
						{
								program_ctrl.vel_cmps_h[Y] = 0;
								program_ctrl.vel_cmps_h[X] = 0;
						}
						break;
						
				case ActionLand:
						UpdateCMD(0,0,CmdLand);
						break;

				case ActionLock:
						g_FMUflg.unlock=0;
						break;
				
				default:
						break;
    }
				
		//打印调试信息
		if(FollowManager.ptrFrame->FormType == 0xA1)
				ZKHD_Printf("竖杆     ");	
		if(FollowManager.ptrFrame->FormType == 0xA2)
		{
//				if(program_ctrl.vel_cmps_h[Y] > 0)
//				{
//						ZKHD_Printf("向左\r\n");
//				}
//				else if(program_ctrl.vel_cmps_h[Y] < 0)
//				{
//						ZKHD_Printf("向右\r\n");
//				}
//				else
//				{
//						ZKHD_Printf("无调整\r\n");
//				}				
							
			if(FollowManager.ptrFrame->CentPoint.y1 >= 90)
			{
					ZKHD_Printf("飞机偏左\r\n");
			}
			else if(FollowManager.ptrFrame->CentPoint.y1 <= 70)
			{
					ZKHD_Printf("飞机偏右\r\n");
			}
			else
			{
					ZKHD_Printf("飞机正中\r\n");
			}
		}
		if(FollowManager.ptrFrame->FormType == 0xB1)
				ZKHD_Printf("黄块     ");	
		if(FollowManager.ptrFrame->FormType == 0xB2)
				ZKHD_Printf("红点     ");	
		if(FollowManager.ptrFrame->FormType == 0xFF)
				ZKHD_Printf("无目标      ");	
}


float PID_Angle_measured = 0;
float PID_Angle_desired = 0;
float PID_Angle_err = 0;
float PID_Angle_deta = 0;
float PID_Angle_out = 0;
float PID_Angle_Kp = 0.02f;
float PID_Angle_Ki = 0.012f;
float PID_Angle_Kd = 0;
float PID_Angle_Ingre = 0;

void HoldCurrentPostion_Angle(float dt)
{
    static float OldPos_Angle;
    
		if(FollowManager.ptrFrame->FormType == HorizontalLine)
		{
				PID_Angle_measured = FollowManager.ptrFrame->CentPoint.y1;
	
				PID_Angle_deta = PID_Angle_measured - OldPos_Angle;
	
				PID_Angle_desired = 70;
    
				PID_Angle_err = PID_Angle_desired - PID_Angle_measured;
			
				PID_Angle_Ingre += PID_Angle_err;
	
				if((PID_Angle_err <= 3)&&(PID_Angle_err >= -3))
				{
						PID_Angle_err = 0;
					
						PID_Angle_Ingre = 0;
				}
	
//				if(PID_Angle_Ingre >= 500)
//				{
//						PID_Angle_Ingre = 500;
//				}
//				else if(PID_Angle_Ingre <= -500)
//				{
//						PID_Angle_Ingre = -500;
//				}
				
				OldPos_Angle = PID_Angle_measured;
				
				PID_Angle_out	= -(PID_Angle_Kp * PID_Angle_err + PID_Angle_Ki * PID_Angle_Ingre + PID_Angle_Kd * PID_Angle_deta);
		
				if(PID_Angle_out >= 6.0f)
				{
						PID_Angle_out = 6.0f;
				}
				else if(PID_Angle_out <= -6.0f)
				{
						PID_Angle_out = -6.0f;
				}
			
				PIDGroup[emPID_Roll_Pos].desired = PID_Angle_out;
		}
		
		if(PID_Angle_out > 0.0f)
		{
				ZKHD_Printf("正在向右调整!!\r\n");
		}
		else if(PID_Angle_out < 0.0f)
		{
				ZKHD_Printf("正在向左调整!!\r\n");
		}
}


void HoldCurrentPostion(float dt)
{
    static float OldPos[2];
    
    //更新测量点
    PIDGroup[emPID_FolloLinePosVertically].measured = FollowManager.ptrFrame->CentPoint.y1 - PosVertically_Offset;
    PIDGroup[emPID_FolloLinePosHorizontally].measured = FollowManager.ptrFrame->CentPoint.x1 - PosHorizontall_Offset;

    PIDGroup[emPID_FolloLineSpdVertically].measured = (FollowManager.ptrFrame->CentPoint.y1 - OldPos[0]);
    PIDGroup[emPID_FolloLineSpdHorizontally].measured = (FollowManager.ptrFrame->CentPoint.x1 - OldPos[1]);
    
    OldPos[0] = FollowManager.ptrFrame->CentPoint.y1;
    OldPos[1] = FollowManager.ptrFrame->CentPoint.x1;
    
    UpdatePID(FollowManager.ptrPIDInfoH,dt);  //PID
    UpdatePID(FollowManager.ptrPIDInfoV,dt);  //PID
    
    PIDGroup[emPID_FolloLineSpdVertically].desired = FollowManager.ptrPIDInfoV->out;
    PIDGroup[emPID_FolloLineSpdHorizontally].desired = FollowManager.ptrPIDInfoH->out;
    
    UpdatePID(&PIDGroup[emPID_FolloLineSpdHorizontally], dt);//PID
    UpdatePID(&PIDGroup[emPID_FolloLineSpdVertically], dt);  //PID

    program_ctrl.vel_cmps_h[Y] = PIDGroup[emPID_FolloLineSpdHorizontally].out;
    program_ctrl.vel_cmps_h[X] = PIDGroup[emPID_FolloLineSpdVertically].out;
}


void HoldCurrentPostion_side(float dt)
{
    static float OldPos_side[2];
    
    //更新测量点
    PIDGroup[emPID_FolloLinePosVertically].measured = FollowManager.ptrFrame->CentPoint.x1;

    PIDGroup[emPID_FolloLineSpdVertically].measured = (FollowManager.ptrFrame->CentPoint.x1 - OldPos_side[0]);
//    PIDGroup[emPID_FolloLineSpdHorizontally].measured = (FollowManager.ptrFrame->CentPoint.x1 - OldPos[1]);
    
    OldPos_side[0] = FollowManager.ptrFrame->CentPoint.x1;
//    OldPos[1] = FollowManager.ptrFrame->CentPoint.x1;
    
//    UpdatePID(FollowManager.ptrPIDInfoH,dt);
    UpdatePID(FollowManager.ptrPIDInfoV,dt);
    
    PIDGroup[emPID_FolloLineSpdVertically].desired = FollowManager.ptrPIDInfoV->out;
//    PIDGroup[emPID_FolloLineSpdHorizontally].desired = FollowManager.ptrPIDInfoH->out;
    
//    UpdatePID(&PIDGroup[emPID_FolloLineSpdHorizontally],dt);
    UpdatePID(&PIDGroup[emPID_FolloLineSpdVertically],dt);

//    program_ctrl.vel_cmps_h[Y] = PIDGroup[emPID_FolloLineSpdHorizontally].out;
    program_ctrl.vel_cmps_h[X] = -(PIDGroup[emPID_FolloLineSpdVertically].out);
}


bool ActionFormChange_Clear(int8_t HoldTime, FormType_t TargetFormType, FSMList_t NextAction)
{
    static int cnt = 0;
    bool ChangeFinished = false;

    if (FollowManager.ptrFrame->FormType == TargetFormType)
    {
        cnt++;

        if (cnt > HoldTime)
        {
            cnt = 0;
            FollowManager.ActionList = NextAction;
            ChangeFinished = true;
        }
    }
    else
    {
        cnt = 0;
    }

    return ChangeFinished;
}


bool ActionFormChange_NoClear(int8_t HoldTime, FormType_t TargetFormType, FSMList_t NextAction)
{
    static int cnt = 0;
    bool ChangeFinished = false;

    if (FollowManager.ptrFrame->FormType == TargetFormType)
    {
        cnt++;

        if (cnt > HoldTime)
        {
					  if (FollowManager.ptrFrame->FormType == YellowBlock)
						{
								find_yellowblock_flag = 1;
						}
            cnt = 0;
            FollowManager.ActionList = NextAction;
            ChangeFinished = true;
        }
    }
    else
    {
   
    }

    return ChangeFinished;
}


void ActionHoldPoint_NoJudge(int8_t Err, int16_t HoldTime, FSMList_t NextAction)
{
    static bool Enter = true;
    static uint16_t CountDown = 0;

    if (Enter)
    {
        CountDown = HoldTime;
        Enter = false;
    }
    else
    {
				CountDown--;
    }

    if (CountDown == 0)
    {
        Enter = true;
        FollowManager.ActionList = NextAction;
    }
}


void ActionHoldPoint_Judge(int8_t Err, int16_t HoldTime, FSMList_t NextAction)
{
    static bool Enter = true;
    static uint16_t CountDown = 0;

    if (Enter)
    {
        CountDown = HoldTime;
        Enter = false;
    }
    else
    {
				if((FollowManager.ptrFrame->CentPoint.x1 >= (80 - Judge_Err))
						&&(FollowManager.ptrFrame->CentPoint.x1 <= (80 + Judge_Err)))
				{
						CountDown--;
				}
    }

    if (CountDown == 0)
    {
        Enter = true;
        FollowManager.ActionList = NextAction;
    }
}


void ActionHoldPoint_Judge_Back(int8_t Err, int16_t HoldTime, FSMList_t NextAction)
{
    static bool Enter = true;
    static uint16_t CountDown = 0;

    if (Enter)
    {
        CountDown = HoldTime;
        Enter = false;
    }
    else
    {
				if((FollowManager.ptrFrame->CentPoint.x1 >=(80-Judge_Err))
						&&(FollowManager.ptrFrame->CentPoint.x1 <= (80+Judge_Err))
							&&(FollowManager.ptrFrame->CentPoint.y1 >= (60-Judge_Err))
								&&((FollowManager.ptrFrame->CentPoint.y1 <= 60+Judge_Err)))
				{
						CountDown--;
				}
    }

    if (CountDown == 0)
    {
        Enter = true;
        FollowManager.ActionList = NextAction;
    }
}


void UpdateButton()
{
    volatile static uint8_t input = 0;
    volatile static uint8_t input2 = 0;
    input = P1IN & BIT1;
    input2 = P1IN & BIT4;

    if (input)
    {
			
    }
    else
    {
        Key1 = true;
    }

    if (input2)
    {
			
    }
    else
    {
        Key2 = true;
    }

    if (Key1 == false && Key2 == false)
    {
        return;
    }
    else
    {
        static bool CloseGate = true;

        //动作线进入倒计时状态
        if (CloseGate)
        {
            CloseGate = false;
            FollowManager.ActionList = ActionCountdown;
        }
    }
}














