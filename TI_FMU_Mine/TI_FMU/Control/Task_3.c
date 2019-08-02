/*
本程序完成任务：
1. 自动起飞(低空)
2. 识别正下方颜色(得到当前位置)
3. 升高
4. 寻找周围目标点(若找到则飞向目标点)
5. 若没有找到目标点则向(前/左/后/右)移动(根据当前位置判断可移动方向)
6. 
7. 在A1点上方悬停1s
8. 向右移动并寻找A3点
9. 在A3上方悬停1s
10.再次向前移动并寻找D3点
11.在D3点上方悬停1s
12.降落
*/

#include "FollowLine.h"
#include "HARDWARE_uart.h"
#include "stdbool.h"
#include "pid.h"
#include "timer_drv.h"
#include "myMath.h"
#include "gcs.h"
#include "program_ctrl.h"

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

    FollowManager.ptrPIDInfoV->kp = 2.5f;
    FollowManager.ptrPIDInfoH->kp = 2.0f;
	
    FollowManager.ptrPIDInfoH->DeathArea = 3;
    FollowManager.ptrPIDInfoV->DeathArea = 3;

    PIDGroup[emPID_FolloLineSpdVertically].kp = 0.5f;//0.45
    PIDGroup[emPID_FolloLineSpdVertically].ki = 0.13f;
//    PIDGroup[emPID_FolloLineSpdVertically].kd = 0.014f;

    PIDGroup[emPID_FolloLineSpdHorizontally].kp = 0.5f;//0.45
    PIDGroup[emPID_FolloLineSpdHorizontally].ki = 0.13f;
//    PIDGroup[emPID_FolloLineSpdHorizontally].kd = 0.014f;

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
		
		//更新当前位置
}

extern UAV_info_t g_UAVinfo;
extern uint8_t finish_flag;
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
								
								EXP_TAKEOFF_ALT_CM = 80;
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
						ActionHoldPoint(MAX_HOVER_ERR,Short_Hold_Time,ActionGoForward);//起飞悬停1s 后 向前移动
            break;

        case ActionGoForward:
						ActionFormChange(MAX_FORMCHANGE_TIME,D2type,ActionHoldD2);//寻找D2 后 D2悬停
						break;

				case ActionHoldD2:
						ActionHoldPoint(MAX_HOVER_ERR,Short_Hold_Time,ActionGoLeft);	//D2悬停 后 向左移动
						//注：ActionHoldPoint里的ERR没有用
						break;
						
				case ActionGoLeft:
						ActionFormChange(MAX_FORMCHANGE_TIME,D1type,ActionHoldD1);//寻找D1 后 D1悬停
						break;
							
				case ActionHoldD1:
						ActionHoldPoint(MAX_HOVER_ERR,Short_Hold_Time,ActionGoBack);	//D1悬停 后 向后移动
						break;
				
				case ActionGoBack:
						ActionFormChange(MAX_FORMCHANGE_TIME,A1type,ActionHoldA1);//寻找A1 后 A1悬停
						break;
				
				case ActionHoldA1:
						ActionHoldPoint(MAX_HOVER_ERR,Short_Hold_Time,ActionGoRight);	//A1悬停 后 向右移动
						break;
				
				case ActionGoRight:
						ActionFormChange(MAX_FORMCHANGE_TIME,A3type,ActionHoldA3);//寻找A3 后 A3悬停
						break;
				
				case ActionHoldA3:
						ActionHoldPoint(MAX_HOVER_ERR,Short_Hold_Time,ActionGoForward_Again);	//A3悬停 后 再次向前移动
						break;
				
				case ActionGoForward_Again:
						ActionFormChange(MAX_FORMCHANGE_TIME,D3type,ActionHoldD3);//寻找D3 后 D3悬停
						break;
				
				case ActionHoldD3:
						ActionHoldPoint(MAX_HOVER_ERR,Short_Hold_Time,ActionLand);//D3悬停 后 降落
				
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
				
				case ActionGoUp:
						UpdateCMD(30,15,CmdUp);
						break;
				
				case ActionGoDown:
						UpdateCMD(-30,15,CmdDown);
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

								program_ctrl.vel_cmps_h[X] = 20;	//前
						}
						break;
						
				case ActionGoBack:
				case ActionGoBack_Again:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);

								program_ctrl.vel_cmps_h[X] = -20;	//后
						}
						break;

				case ActionGoLeft:
				case ActionGoLeft_Again:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[Y] = 20;	//左
						}
						break;

				case ActionGoRight:
				case ActionGoRight_Again:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);

								program_ctrl.vel_cmps_h[Y] = -20;	//右
						}
						break;
				
				case ActionGoForwardRight:		
				case ActionGoForwardRight_Again:
						{
								program_ctrl.vel_cmps_h[Y] = -20;	//右
								program_ctrl.vel_cmps_h[X] = 20;	//前
						}

				case ActionGoForwardLeft_MoreLeft:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);

								program_ctrl.vel_cmps_h[X] = 10;	//前
								program_ctrl.vel_cmps_h[Y] = 20;	//左
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
				case ActionHoldA1:
				case ActionHoldA2:
				case ActionHoldA3:
				case ActionHoldB1:
				case ActionHoldB2:
				case ActionHoldB3:
				case ActionHoldC1:
				case ActionHoldC2:
				case ActionHoldC3:
				case ActionHoldD1:
				case ActionHoldD2:
				case ActionHoldD3:
						{
								//设置期望值 即期望悬停目标点在屏幕的中心
								PIDGroup[emPID_FolloLinePosVertically].desired = 120/2;
								PIDGroup[emPID_FolloLinePosHorizontally].desired = 160/2;
						
								HoldCurrentPostion(dt);
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
}


#define PosVertically_Offset 		6
#define PosHorizontall_Offset 	0

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











