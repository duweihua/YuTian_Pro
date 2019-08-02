/*
本程序完成任务：
1. 自动起飞
2. 向右前方移动并寻找B3点
3. 在B3点上方悬停2s以上
4. 向左前方(偏左)移动并寻找C1点
5. 在C1点上方悬停2s以上
6. 向右前方移动并寻找D2点
7. 在D2点上方悬停2s以上
8. 向后移动并寻找A2点
9. 在A2点上方悬停2s以上
10.降落
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
bool ActionFormChange(int8_t HoldTime, FormType_t TargetFormType, FSMList_t NextAction);
void UpdateDebugInfo(void);
void HoldCurrentPostion(float dt);

FollowManager_t FollowManager;
SonarManager_t SonarManager;

#define PosVertically_Offset 		15
#define PosHorizontall_Offset 	0


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

    PIDGroup[emPID_FolloLineSpdVertically].kp = 0.45f;//0.45f;
    PIDGroup[emPID_FolloLineSpdVertically].ki = 0.13f;
//    PIDGroup[emPID_FolloLineSpdVertically].kd = 0.014f;

    PIDGroup[emPID_FolloLineSpdHorizontally].kp = 0.45f;//0.45f;
    PIDGroup[emPID_FolloLineSpdHorizontally].ki = 0.13f;
//    PIDGroup[emPID_FolloLineSpdHorizontally].kd = 0.014f;

    PIDGroup[emPID_FolloLinePosVertically].desired = 120 / 2 + PosVertically_Offset;
    PIDGroup[emPID_FolloLinePosHorizontally].desired = 160 / 2 + PosHorizontall_Offset;

    FollowManager.ptrPIDInfoH->OutLimitHigh = 20;
    FollowManager.ptrPIDInfoH->OutLimitLow = -20;
    FollowManager.ptrPIDInfoV->OutLimitHigh = 20;
    FollowManager.ptrPIDInfoV->OutLimitLow = -20;

    FollowManager.CountDownNumMs = MAX_COUNTDOWN;
    FollowManager.TargetAltitudeCM = TARGETALTITUDECM;

		//读取当前检测到的类型和坐标(采用指针)
    FollowManager.ptrFrame = (OpenMVFrame_t *)UsartGroup[UART_A3].RxBuff;
		//读取当前飞控总状态(姿态、高度、遥控等)
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


uint8_t my_text[] = {0xFF};
extern FollowManager_t FollowManager;

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
		
//		USART_TX(&UsartGroup[UART_A1],FollowManager.ptrFrame->FormType,1);
//		USART_TX(&UsartGroup[UART_A1],my_text,1);
	
//		ZKHD_Printf(FollowManager.ptrFrame->FormType);
//		ZKHD_Printf("\r\n");
//		ZKHD_Printf(UsartGroup[UART_A3].RxBuff[1]);
		
    UpdateAction(dt);
		
}


extern UAV_info_t g_UAVinfo;
extern uint8_t finish_flag;
extern LedManager_t g_LedManager;
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
								
								EXP_TAKEOFF_ALT_CM = 70;
            }
        }
        break;

        case ActionTakeOff:
        {
						//此处时间为起飞时间
						ActionHoldPoint(MAX_HOVER_ERR, Take_off_Time, ActionHoverStartPoint);
        }
        break;

        case ActionHoverStartPoint:
						ActionHoldPoint(MAX_HOVER_ERR,200,ActionGoForwardRight);//起飞悬停 后 向右前移动
            break;

        case ActionGoForwardRight:
						ActionFormChange(MAX_FORMCHANGE_TIME,B3type,ActionHoldB3);//寻找B3 后 B3悬停
						break;
				
//				case ActionHoldB3_Static:
//						ActionHoldPoint(MAX_HOVER_ERR,Static_Hold_Time,ActionHoldB3);
//						break;

				case ActionHoldB3:
						g_LedManager.emLEDStatus = StatusOn;
						ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoForward);
						break;
						
				case ActionGoForward:
						g_LedManager.emLEDStatus = StatusOff;
						ActionFormChange(MAX_FORMCHANGE_TIME,C3type,ActionHoldC3);
						break;
						
				case ActionHoldC3:
						g_LedManager.emLEDStatus = StatusOn;
						ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoLeft);
						break;
				
				case ActionGoLeft:
						g_LedManager.emLEDStatus = StatusOff;
						ActionFormChange(BIGGER_FORMCHANGE_TIME,C2type,ActionHoldC2);
						break;

				case ActionHoldC2:
						g_LedManager.emLEDStatus = StatusOn;
						ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoLeft_Again);	//C1悬停 后 再次向右前移动
						break;

				case ActionGoLeft_Again:
						g_LedManager.emLEDStatus = StatusOff;
						ActionFormChange(MAX_FORMCHANGE_TIME,C1type,ActionHoldC1);
						break;

				case ActionHoldC1:
						g_LedManager.emLEDStatus = StatusOn;
						ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoForwardRight_Again);	//C1悬停 后 再次向右前移动
						break;
				
				case ActionGoForwardRight_Again:
						g_LedManager.emLEDStatus = StatusOff;
						ActionFormChange(50,D2type,ActionHoldD2);//寻找D2 后 D2悬停
						break;
				
				case ActionHoldD2:
						g_LedManager.emLEDStatus = StatusOn;
						ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoBack);	//D2悬停 后 向后移动
						break;
				
				case ActionGoBack:
						g_LedManager.emLEDStatus = StatusOff;
						ActionFormChange(BIGGER_FORMCHANGE_TIME,A2type,ActionHoldA2);//寻找A2 后 A2悬停
						break;
				
				case ActionHoldA2:
						g_LedManager.emLEDStatus = StatusOn;
						ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionLand);	//A2悬停 后 降落
						break;
				
        case ActionLand:
						{
								static int Cnt = MAX_TIMEOUT1;
								if (Cnt-- < 0)
										FollowManager.ActionList = ActionLock;
						}
						break;

        case ActionLock:
            FollowManager.ActionList = ActionWaitting;
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
						
				case ActionGoForwardRight:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);

								program_ctrl.vel_cmps_h[X] = 20;	//前
								program_ctrl.vel_cmps_h[Y] = -20;	//右
						}
						break;
										
				case ActionGoForwardRight_Again:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);

								program_ctrl.vel_cmps_h[X] = 20;	//前
								program_ctrl.vel_cmps_h[Y] = -20;	//右
						}
						break;
						
				case ActionGoLeft:
				case ActionGoRight:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);
							
								if(FollowManager.ActionList == ActionGoLeft)
										program_ctrl.vel_cmps_h[Y] = 20;
								else if(FollowManager.ActionList == ActionGoRight)
										program_ctrl.vel_cmps_h[Y] = -20;
						}
						break;
						
				case ActionGoLeft_Again:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[Y] = 20;
						}
						break;
						
				case ActionGoForward:
				case ActionGoBack:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);
							
								if(FollowManager.ActionList == ActionGoForward)
										program_ctrl.vel_cmps_h[X] = 20;
								else if(FollowManager.ActionList == ActionGoBack)
										program_ctrl.vel_cmps_h[X] = -20;
						}
						break;
						
				case ActionHoldB3:
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
						break;

				case ActionHoldC3:
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
						break;
						
				case ActionHoldC2:
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
						break;

				case ActionHoldC1:
						if(FollowManager.ptrFrame->FormType == C1type)
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
						break;
						
				case ActionHoldD2:
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
						break;
								
				case ActionHoldA2:
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
				ZKHD_Printf("C1    ");
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
				ZKHD_Printf("无目标    ");				
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
        CountDown--;
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











