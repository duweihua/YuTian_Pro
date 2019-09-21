/*
本程序完成一个基本历程动作：
1.倒计时；
2.自动起飞；
3.悬停；
4.前进寻找小车；
5.在小车上方悬停；
6.向左移动
7.降落
8.上锁
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
bool FollowLine = false;
bool FollowApriTag = false;

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
	
//	  FollowManager.ptrPIDInfoV->ki = 0.05f;//0
//    FollowManager.ptrPIDInfoH->ki = 0.05f;//0

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
}

extern UAV_info_t g_UAVinfo;
extern uint8_t finish_flag;
s32 EXP_TAKEOFF_ALT_CM = 120;

//此函数只做状态判断和状态更新
void UpdateStatus()
{
		static uint16_t left_cnt = 0;
	
    //根据ActionList的内容，进入不同的状态
    switch (FollowManager.ActionList)
    {
        //判断
        case ActionWaitting:
            //Do nothing;
            break;

        //倒计时状态
        case ActionCountdown:
        {
            //倒计时，数据初始填充位于Follow_Init中
            FollowManager.CountDownNumMs--;

            //当倒计时结束时候，状态变更为ActionTakeOff
            if (FollowManager.CountDownNumMs <= 0)
            {
                FollowManager.ActionList = ActionTakeOff;
								
								//任务1
								if((FollowLine==true)&&(FollowApriTag==false))
								{
										EXP_TAKEOFF_ALT_CM = 120;
								}
								else //任务2或3
								{
										EXP_TAKEOFF_ALT_CM = 80;
								}
            }
        }
        break;

        //自动起飞状态
        case ActionTakeOff:
        {
						//如果按下S1 未按下S2 则进入任务1
						if((FollowLine==true)&&(FollowApriTag==false))
						{
								//自动起飞动作持续时间为5s 然后跳到ActionHoverStartPoint动作；
								ActionHoldPoint(MAX_HOVER_ERR, 500, ActionHoverStartPoint);
						}
						else	//任务2或3
						{	
								ActionHoldPoint(MAX_HOVER_ERR, 300, ActionHoverStartPoint);
						}							
        }
        break;

        //
        case ActionHoverStartPoint:
						//如果按下S1 未按下S2 则进入任务1
						if((FollowLine==true)&&(FollowApriTag==false))
						{
								//任务1：起点悬停5s后降落
								ActionHoldPoint(MAX_HOVER_ERR, 600, ActionLand);
						}
						//如果按下S2 未按下S1 则进入任务2
						if((FollowLine==false)&&(FollowApriTag==true))
						{
								//任务2：起点悬停后前进寻找小车 找到后在小车上方悬停5s 然后向左移动再降落
								ActionHoldPoint(MAX_HOVER_ERR, 50, ActionGoForward);
						}
						//如果按下S1 也按下S2 则进入任务3
						if((FollowLine==true)&&(FollowApriTag==true))
						{
								//任务3：起点悬停后前进寻找小车 找到后跟踪小车 直到小车停止运动后5s(收到信号) 然后向左移动再降落
								ActionHoldPoint(MAX_HOVER_ERR, 100, ActionGoForward);
						}
            break;

				//
        case ActionGoForward:
						ActionFormChange(MAX_FORMCHANGE_TIME, ApriTag, ActionHoldApriTag);            
						break;

				case ActionHoldApriTag://动作：悬停ApriTag
						
						//任务2：起点悬停后前进寻找小车 找到后在小车上方悬停5s 然后向左移动再降落
						if((FollowLine==false)&&(FollowApriTag==true))
						{
								ActionHoldPoint(MAX_HOVER_ERR,700,ActionGoLeft);
						}
						//任务3：起点悬停后前进寻找小车 找到后跟踪小车 直到小车停止运动后5s(收到信号) 然后向左移动再降落
						if((FollowLine==true)&&(FollowApriTag==true))
						{
								if(finish_flag==1)//收到小车信号
								{
										//马上向左移动 准备降落
										ActionHoldPoint(MAX_HOVER_ERR,50,ActionGoLeft);
								}
						}
						break;
						
				case ActionGoLeft:	//动作：向左
						{
								left_cnt++;
							
								if(left_cnt>=200)
								{
										left_cnt=200;
									
										FollowManager.ActionList = ActionHoverStopPoint;
								}
						}
						break;
							
				case ActionHoverStopPoint://动作：
						{
								//
								ActionHoldPoint(MAX_HOVER_ERR,100,ActionLand);
						}
						break;
		
        //自动降落状态倒计时结束以后 进入上锁动作
        case ActionLand:
						{
								//倒计时逻辑
								static int Cnt = MAX_TIMEOUT1;
								
								//如果在任务1 自动赋值finish_flag
								if((FollowLine==true)&&(FollowApriTag==false))
								{
										finish_flag = 1;
								}
								
								if (Cnt-- < 0)
								{
										FollowManager.ActionList = ActionLock;
								}
						}
						break;

        //上锁动作
        case ActionLock:
            FollowManager.ActionList = ActionWaitting;
            break;

        default:
            break;
    }
}


//只执行动作
void UpdateAction(float dt)
{
    switch (FollowManager.ActionList)
    {
				//倒计时命令
				case ActionWaitting:
						//Do nothing
						break;

				//自动起飞命令
				case ActionTakeOff:
						UpdateCMD(0, 0, CmdTakeOff);
						break;

				//悬停命令
				case ActionHoverStartPoint:
						//起飞
						{
								program_ctrl.vel_cmps_h[Y] = 0;
								program_ctrl.vel_cmps_h[X] = 0;
						}
						break;
						
				//前后飞行
				case ActionGoForward:
				case ActionGoBack:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);
								
								if(FollowManager.ActionList == ActionGoForward)
								{
										program_ctrl.vel_cmps_h[X] = 30;
								}
								else if(FollowManager.ActionList == ActionGoBack)
								{
										program_ctrl.vel_cmps_h[X] = -30;
								}
						}
						break;
				
				//悬停动作
				case ActionHoldLtype:
				case ActionHoldMirrorFlipLtype:
				case ActionHoldTurnLtype:
				case ActionHoldMirrorFlipTurnLtype:
				case ActionHoldCross:
				case ActionHoldTtype:
				case ActionHoldTurnTtype:
				case ActionHoldFeaturePoint:
				case ActionHoldApriTag:
						if(FollowManager.ptrFrame->FormType == ApriTag)
						{
								//设置期望值 即期望小车在屏幕的中心
								//这里只有HoldAprilTag模式才具有跟踪的功能
								PIDGroup[emPID_FolloLinePosVertically].desired = 120/2;
								PIDGroup[emPID_FolloLinePosHorizontally].desired = 160/2;
								
								HoldCurrentPostion(dt);
						}
						else
						{
								program_ctrl.vel_cmps_h[Y] = 0;
								program_ctrl.vel_cmps_h[X] = 0;
						}
						break;
						
				//左右飞行
				case ActionGoLeft:
				case ActionGoRight:
						{
								//飞行不循线
								//HoldCurrentPostion(dt);
							
								if(FollowManager.ActionList == ActionGoLeft)
								{
										program_ctrl.vel_cmps_h[Y] = 30;
								}
								else if(FollowManager.ActionList == ActionGoRight)
								{
										program_ctrl.vel_cmps_h[Y] = -30;
								}
						}
						break;
				
				//
				case ActionHoverStopPoint:
						{
								program_ctrl.vel_cmps_h[Y] = 0;
								program_ctrl.vel_cmps_h[X] = 0;
						}
						break;
						
				//自动降落
				case ActionLand:
						//降落命令
						UpdateCMD(0, 0, CmdLand);
						break;

				//上锁动作
				case ActionLock:
						//注意上锁要清除此标志位
						g_FMUflg.unlock=0;
						//另外一个标志位(g_UAVinfo.FMUflg->unlock)只会使飞机保持怠速
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
    //判定两个输入是否有效，其实是判断左右两个按键
    volatile static uint8_t input = 0;
    volatile static uint8_t input2 = 0;
    input = P1IN & BIT1;
    input2 = P1IN & BIT4;

    //判断巡线按钮是否按下
    if (input)
    {
			
    }
    else
    {
        FollowLine = true;
    }

    //判断寻找ApriTag按钮是否按下
    if (input2)
    {
			
    }
    else
    {
        FollowApriTag = true;
    }

    //判断当前是否被多按
    if (FollowApriTag == false && FollowLine == false)
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











