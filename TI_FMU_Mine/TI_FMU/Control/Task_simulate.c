/*
本程序完成任务：
1. 按键Key1 执行基本任务1：A2->B3->C1->D2->A2
2. 按键Key2 执行基本任务2：A2->A3->D3->D1->A1->A2->C2->A2
3. 按键Key  执行基本任务 
4. 按键Key  执行基本任务 
5. 按键Key  执行基本任务 
6. 按键Key  执行基本任务 
7. 按键Key  执行基本任务 
8. 按键Key  执行基本任务 
9. 按键Key  执行基本任务 
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
void ActionHoldPoint(int8_t Err, int16_t HoldTime, FSMList_t NextAction);
void ActionHoldPoint_NoJudge(int8_t Err, int16_t HoldTime, FSMList_t NextAction);
void ActionHoldPoint_Back(int8_t Err, int16_t HoldTime, FSMList_t NextAction);
bool ActionFormChange(int8_t HoldTime, FormType_t TargetFormType, FSMList_t NextAction);
void UpdateDebugInfo(void);
void HoldCurrentPostion(float dt);

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
    FollowManager.ptrPIDInfoV = &PIDGroup[emPID_FolloLinePosVertically];
    FollowManager.ptrPIDInfoH = &PIDGroup[emPID_FolloLinePosHorizontally];

    FollowManager.ptrPIDInfoV->kp = 1.5f;
    FollowManager.ptrPIDInfoH->kp = 1.5f;
	
    FollowManager.ptrPIDInfoH->DeathArea = 3;
    FollowManager.ptrPIDInfoV->DeathArea = 3;

    PIDGroup[emPID_FolloLineSpdVertically].kp = 0.45f;//0.45
    PIDGroup[emPID_FolloLineSpdVertically].ki = 0.13f;
//    PIDGroup[emPID_FolloLineSpdVertically].kd = 0.014f;

    PIDGroup[emPID_FolloLineSpdHorizontally].kp = 0.45f;//0.45
    PIDGroup[emPID_FolloLineSpdHorizontally].ki = 0.13f;
//    PIDGroup[emPID_FolloLineSpdHorizontally].kd = 0.014f;

		//后面不同动作加设了不同的镜头偏置
    PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2;
    PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2;

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


//以100hz的速度轮询 10ms
void UpdateCentControl(float dt)
{
    //判断OpenMV返回的数据是否可用，有的时候OpenMV会返回无效数据
    if (FollowManager.ptrFrame->CentPoint.x1 > 200 || FollowManager.ptrFrame->CentPoint.y1 > 200)
        return;

    //更新按钮控制实践
    UpdateButton();

    //更新程控状态线
    UpdateStatus();

    //更新程控动作线
    UpdateAction(dt);
}


extern UAV_info_t g_UAVinfo;
extern uint8_t finish_flag;
extern FollowManager_t FollowManager;
extern LedManager_t g_LedManager;
//自动起飞高度(此处为定义)(倒计时结束可以区分任务赋值)
s32 EXP_TAKEOFF_ALT_CM = 120;

void UpdateStatus()
{
		static uint16_t left_cnt = 0;
	
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
										
										EXP_TAKEOFF_ALT_CM = 70;//基础任务要求飞行高度不低于50cm
								}
						}
						break;

        case ActionTakeOff:
						{
								//此处时间为起飞时间
								ActionHoldPoint_NoJudge(MAX_HOVER_ERR,Take_off_Time,ActionHoldA2);
						}
						break;

				//基本任务一开始
        case ActionHoldA2:
						{
								if((Key1==true)&&(Key2==false))	//基本任务1		
										ActionHoldPoint(MAX_HOVER_ERR,Take_off_Hold_Time,ActionGoForwardRight);//起飞悬停 后 向右前移动
								if((Key1==false)&&(Key2==true))	//基本任务2
										ActionHoldPoint(MAX_HOVER_ERR,Take_off_Hold_Time,ActionGoRight);//起飞悬停 后 向右移动
						}
            break;

        case ActionGoForwardRight:
						{
								if((Key1==true)&&(Key2==false))	//基本任务1
										ActionFormChange(Short_Formchange_Time,B3type,ActionHoldB3);//寻找B3 后 B3悬停
						}
						break;

				case ActionHoldB3:
						{
								if((Key1==true)&&(Key2==false))	//基本任务1
										ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoForwardLeft_MoreLeft);	//B3悬停 后 向左前偏左移动
						}
						break;
					
				case ActionGoForwardLeft_MoreLeft:
						{
								if((Key1==true)&&(Key2==false))	//基本任务1
										ActionFormChange(50,C1type,ActionHoldC1);//寻找C1 后 C1悬停
						}
						break;
						
				case ActionHoldC1:
						{							
								if((Key1==true)&&(Key2==false))	//基本任务1
								{
										ActionHoldPoint(MAX_HOVER_ERR,600,ActionGoForwardRight_Again);	//C1悬停 后 再次向右前移动
								}
						}
						break;

				case ActionGoForwardRight_Again:
						{
								if((Key1==true)&&(Key2==false))	//基本任务1
										ActionFormChange(Long_Formchange_Time,D2type,ActionHoldD2);//寻找D2 后 D2悬停
						}
						break;
				
				case ActionHoldD2:
						{
								if((Key1==true)&&(Key2==false))	//基本任务1
										ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoBack);	//D2悬停 后 向后移动
						}
						break;

				case ActionGoBack:
						{
								if((Key1==true)&&(Key2==false))	//基本任务1	
										ActionFormChange(Long_Formchange_Time,A2type,ActionHoldA2_Again);//寻找A2 后 再次A2悬停
								if((Key1==false)&&(Key2==true))	//基本任务2	
										ActionFormChange(Short_Formchange_Time,A1type,ActionHoldA1);//寻找A1 后 A1悬停
						}
						break;

				case ActionHoldA2_Again:
						{
								if((Key1==true)&&(Key2==false))	//基本任务1
										ActionHoldPoint_Back(MAX_HOVER_ERR,Move_Hold_Time,ActionLand);	//A2悬停 后 降落
								if((Key1==false)&&(Key2==true))	//基本任务2	
										ActionHoldPoint_Back(MAX_HOVER_ERR,Move_Hold_Time,ActionGoForward_Again);//A2悬停 后 再次向前移动
						}
						break;
						
				//基本任务二开始
        case ActionGoRight:
						{
								if((Key1==false)&&(Key2==true))	//基本任务2
										ActionFormChange(Short_Formchange_Time,A3type,ActionHoldA3);//寻找A3 后 A3悬停
						}
						break;

				case ActionHoldA3:
						{
								if((Key1==false)&&(Key2==true))	//基本任务2
										ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoForward);	//A3悬停 后 向前移动
						}
						break;
						
				case ActionGoForward:
						{
								if((Key1==false)&&(Key2==true))	//基本任务2
										ActionFormChange(Long_Formchange_Time_D3,D3type,ActionHoldD3);//寻找D3 后 D3悬停
						}
						break;

				case ActionHoldD3:
						{
								if((Key1==false)&&(Key2==true))	//基本任务2
										ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time_D3,ActionGoLeft);	//D3悬停 后 向左移动
						}
						break;

				case ActionGoLeft:
						{
								if((Key1==false)&&(Key2==true))	//基本任务2
										ActionFormChange(Long_Formchange_Time,D1type,ActionHoldD1);//寻找D1 后 D1悬停
						}
						break;

				case ActionHoldD1:
						{
								if((Key1==false)&&(Key2==true))	//基本任务2
										ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoBack);	//D1悬停 后 向后移动
						}
						break;
						
				case ActionHoldA1:
						{
								if((Key1==false)&&(Key2==true))	//基本任务2
										ActionHoldPoint_Back(MAX_HOVER_ERR,Move_Hold_Time_A1,ActionGoRight_Again);//A1悬停 后 再次向右移动
						}
						break;
						
				case ActionGoRight_Again:
						{
								if((Key1==false)&&(Key2==true))	//基本任务2
										ActionFormChange(Long_Formchange_Time,A2type,ActionHoldA2_Again);//寻找A2 后 再次A2悬停
						}
						break;

				case ActionGoForward_Again:
						{
								if((Key1==false)&&(Key2==true))	//基本任务2
										ActionFormChange(Middle_Formchange_Time,C2type,ActionHoldC2);//寻找C2 后 C2悬停
						}
						break;
						
				case ActionHoldC2:
						{
								if((Key1==false)&&(Key2==true))	//基本任务2
										ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoBack_Again);//C2悬停 后 再次向后移动
						}
						break;
						
				case ActionGoBack_Again:
						{
								if((Key1==false)&&(Key2==true))	//基本任务2
										ActionFormChange(Long_Formchange_Time,A2type,ActionHoldA2_Third);//寻找A2 后 第三次A2悬停
						}
						break;

				case ActionHoldA2_Third:
						{
								if((Key1==false)&&(Key2==true))	//基本任务2
										ActionHoldPoint_Back(MAX_HOVER_ERR,Move_Hold_Time,ActionLand);//A2悬停 后 降落
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


void UpdateAction(float dt)
{
    switch (FollowManager.ActionList)
    {
				case ActionWaitting:
						break;

				case ActionTakeOff:
						UpdateCMD(0,0,CmdTakeOff);
						break;

				case ActionHoverStartPoint:
						{
								program_ctrl.vel_cmps_h[Y] = 0;
								program_ctrl.vel_cmps_h[X] = 0;
						}
						break;
						
				case ActionGoForward:
				case ActionGoForward_Again:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[X] = 32;	//前
						}
						break;
						
				case ActionGoBack:
				case ActionGoBack_Again:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[X] = -32;//-38;	//后
						}
						break;

				case ActionGoLeft:
				case ActionGoLeft_Again:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[Y] = 24;	//左
						}
						break;

				case ActionGoRight:
				case ActionGoRight_Again:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[Y] = -32;	//右
						}
						break;
				
				case ActionGoForwardRight:		
				case ActionGoForwardRight_Again:
						{
								program_ctrl.vel_cmps_h[Y] = -25;	//右
								program_ctrl.vel_cmps_h[X] = 25;	//前
						}
						break;

				case ActionGoForwardLeft:		
				case ActionGoForwardLeft_Again:
						{
								program_ctrl.vel_cmps_h[Y] = 25;	//左
								program_ctrl.vel_cmps_h[X] = 25;	//前
						}
						break;
						
				case ActionGoBackRight:		
				case ActionGoBackRight_Again:
						{
								program_ctrl.vel_cmps_h[Y] = -25;	//右
								program_ctrl.vel_cmps_h[X] = -25;	//后
						}
						break;

				case ActionGoBackLeft:		
				case ActionGoBackLeft_Again:
						{
								program_ctrl.vel_cmps_h[Y] = 25;	//左
								program_ctrl.vel_cmps_h[X] = -25;	//后
						}
						break;
						
				case ActionGoForwardLeft_MoreLeft://左前偏左
						{							
								if(FollowManager.ptrFrame->FormType == B2type)
								{
										program_ctrl.vel_cmps_h[X] = 25;	//前
										program_ctrl.vel_cmps_h[Y] = 25;	//左
								}
								else if(FollowManager.ptrFrame->FormType == C2type)
								{
										program_ctrl.vel_cmps_h[X] = 0;		//前
										program_ctrl.vel_cmps_h[Y] = 25;	//左
								}
								else
								{
										program_ctrl.vel_cmps_h[X] = 15;	//前
										program_ctrl.vel_cmps_h[Y] = 30;	//左
								}
						}
						break;
						
				case ActionGoUp:
				case ActionGoUp_Again:
						{
								program_ctrl.vel_cmps_h[Z] = 5;
						}
						break;
						
				case ActionGoDown:
				case ActionGoDown_Again:
						{
								program_ctrl.vel_cmps_h[Z] = -5;
						}
						break;
						
				case ActionHoldLtype:
				case ActionHoldMirrorFlipLtype:
				case ActionHoldTurnLtype:
				case ActionHoldMirrorFlipTurnLtype:
				case ActionHoldCross:
				case ActionHoldTtype:
				case ActionHoldTurnTtype:
				case ActionHoldFeaturePoint:
				case ActionHoldApriTag:
						{
								if(FollowManager.ptrFrame->FormType == ApriTag)
								{
										//设置期望值 即期望悬停目标点在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120/2 + PosVertically_Offset;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160/2;
								
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[X] = 0;
										program_ctrl.vel_cmps_h[Y] = 0;
								}
						}
						break;
										
				case ActionHoldA1:	//A1取消前后方向上的镜头偏置(后退终点)
						{
								if(FollowManager.ptrFrame->FormType == A1type)
								{
										//设置期望值 即期望小车在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + 0;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;
										
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;

				case ActionHoldA2:	//A2前两次悬停非后退 不取消镜头偏置(使定位点偏前)
				case ActionHoldA2_Again:
						{
								if(FollowManager.ptrFrame->FormType == A2type)
								{
										//设置期望值 即期望小车在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + PosVertically_Offset;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;
										
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;

				case ActionHoldA2_Third:	//A2第三次悬停在后退过程中 取消镜头偏置(使定位点偏后)		
						{
								if(FollowManager.ptrFrame->FormType == A2type)
								{
										//设置期望值 即期望小车在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + 0;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;
										
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;
						
				case ActionHoldA3:	//A3悬停不在后退过程中 保持镜头偏置(使定位偏前)
						{
								if(FollowManager.ptrFrame->FormType == A3type)
								{
										//设置期望值 即期望小车在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + PosVertically_Offset;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;
										
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;
						
				case ActionHoldB1:
						{
								if(FollowManager.ptrFrame->FormType == B1type)
								{
										//设置期望值 即期望小车在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + PosVertically_Offset;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;
										
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;
				
				case ActionHoldB2:
						{
								if(FollowManager.ptrFrame->FormType == B2type)
								{
										//设置期望值 即期望小车在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + PosVertically_Offset;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;
										
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;
						
				case ActionHoldB3:
						{
								if(FollowManager.ptrFrame->FormType == B3type)
								{
										//设置期望值 即期望小车在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + PosVertically_Offset;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;
										
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;
						
				case ActionHoldC1:
						{
								if(FollowManager.ptrFrame->FormType == C1type)
								{
										//设置期望值 即期望小车在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + PosVertically_Offset;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;
										
										HoldCurrentPostion(dt);
								}
								else if(FollowManager.ptrFrame->FormType == C2type)
								{
										program_ctrl.vel_cmps_h[Y] = 20;
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;
						
				case ActionHoldC2:
						{
								if(FollowManager.ptrFrame->FormType == C2type)
								{
										//设置期望值 即期望小车在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + PosVertically_Offset;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;
										
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;
						
				case ActionHoldC3:
						{
								if(FollowManager.ptrFrame->FormType == C3type)
								{
										//设置期望值 即期望小车在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + PosVertically_Offset;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;
										
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;
						
				case ActionHoldD1:
						{
								if(FollowManager.ptrFrame->FormType == D1type)
								{
										//设置期望值 即期望小车在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + PosVertically_Offset;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;
										
										HoldCurrentPostion(dt);
								}
								else if(FollowManager.ptrFrame->FormType == D2type)
								{
										program_ctrl.vel_cmps_h[Y] = 20;
										program_ctrl.vel_cmps_h[X] = 0;
								}
								else if(FollowManager.ptrFrame->FormType == 0xFF)
								{
										program_ctrl.vel_cmps_h[Y] = 10;
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;
						
				case ActionHoldD2:
						{
								if(FollowManager.ptrFrame->FormType == D2type)
								{
										//设置期望值 即期望小车在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + PosVertically_Offset;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;
										
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;
						
				case ActionHoldD3:
						{
								if(FollowManager.ptrFrame->FormType == D3type)
								{
										//设置期望值 即期望小车在屏幕的中心
										PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + PosVertically_Offset;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;
										
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[Y] = 0;
										program_ctrl.vel_cmps_h[X] = 0;
								}
						}
						break;

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
				
		if(FollowManager.ptrFrame->FormType == 0xA1)
				ZKHD_Printf("A1    ");
		if(FollowManager.ptrFrame->FormType == 0xA2)
				ZKHD_Printf("A2    ");
		if(FollowManager.ptrFrame->FormType == 0xA3)
				ZKHD_Printf("A3    ");
		if(FollowManager.ptrFrame->FormType == 0xB1)
				ZKHD_Printf("B1    ");
		if(FollowManager.ptrFrame->FormType == 0xB2)
				ZKHD_Printf("B2    ");
		if(FollowManager.ptrFrame->FormType == 0xB3)
				ZKHD_Printf("B3    ");
		if(FollowManager.ptrFrame->FormType == 0xC1)
		{
				ZKHD_Printf("C1");
				if(FollowManager.ptrFrame->CentPoint.x1 >=80 )
				{
					ZKHD_Printf("大   ");
				}
				else
				{
					ZKHD_Printf("小   ");
				}
		}
		if(FollowManager.ptrFrame->FormType == 0xC2)
				ZKHD_Printf("C2    ");
		if(FollowManager.ptrFrame->FormType == 0xC3)
				ZKHD_Printf("C3    ");
		if(FollowManager.ptrFrame->FormType == 0xD1)
				ZKHD_Printf("D1    ");
		if(FollowManager.ptrFrame->FormType == 0xD2)
				ZKHD_Printf("D2    ");
		if(FollowManager.ptrFrame->FormType == 0xD3)
				ZKHD_Printf("D3    ");
		if(FollowManager.ptrFrame->FormType == 0xFF)
				ZKHD_Printf("无目标      ");	
}

void HoldCurrentPostion(float dt)
{
    static float OldPos[2];
    
    //更新测量点
    PIDGroup[emPID_FolloLinePosVertically].measured = FollowManager.ptrFrame->CentPoint.y1;
    PIDGroup[emPID_FolloLinePosHorizontally].measured = FollowManager.ptrFrame->CentPoint.x1;

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


bool ActionFormChange(int8_t HoldTime, FormType_t TargetFormType, FSMList_t NextAction)
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

void ActionHoldPoint(int8_t Err, int16_t HoldTime, FSMList_t NextAction)
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
				if((FollowManager.ptrFrame->CentPoint.x1 >=(80+PosHorizontall_Offset-Judge_Err))
						&&(FollowManager.ptrFrame->CentPoint.x1 <= (80+PosHorizontall_Offset+Judge_Err))
							&&(FollowManager.ptrFrame->CentPoint.y1 >= (60+PosVertically_Offset-Judge_Err))
								&&((FollowManager.ptrFrame->CentPoint.y1 <= 60+PosVertically_Offset+Judge_Err)))
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


void ActionHoldPoint_Back(int8_t Err, int16_t HoldTime, FSMList_t NextAction)
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














