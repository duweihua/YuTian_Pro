/*
�������������
1. �Զ����(�Ϳ�)
2. ʶ�����·���ɫ(�õ���ǰλ��)
3. ����
4. Ѱ����ΧĿ���(���ҵ������Ŀ���)
5. ��û���ҵ�Ŀ�������(ǰ/��/��/��)�ƶ�(���ݵ�ǰλ���жϿ��ƶ�����)
6. 
7. ��A1���Ϸ���ͣ1s
8. �����ƶ���Ѱ��A3��
9. ��A3�Ϸ���ͣ1s
10.�ٴ���ǰ�ƶ���Ѱ��D3��
11.��D3���Ϸ���ͣ1s
12.����
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


//��100hz���ٶ���ѯ 10ms
void UpdateCentControl(float dt)
{
    //�ж�OpenMV���ص������Ƿ���ã��е�ʱ��OpenMV�᷵����Ч����
    if (FollowManager.ptrFrame->CentPoint.x1 > 200 || FollowManager.ptrFrame->CentPoint.y1 > 200)
        return;

    //���°�ť����ʵ��
    UpdateButton();

    //���³̿�״̬��
    UpdateStatus();

    //���³̿ض�����
    UpdateAction(dt);
		
		//���µ�ǰλ��
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
						//�˴�ʱ��Ϊ���ʱ��
						ActionHoldPoint(MAX_HOVER_ERR, Take_off_Time, ActionHoverStartPoint);
        }
        break;

        case ActionHoverStartPoint:
						ActionHoldPoint(MAX_HOVER_ERR,Short_Hold_Time,ActionGoForward);//�����ͣ1s �� ��ǰ�ƶ�
            break;

        case ActionGoForward:
						ActionFormChange(MAX_FORMCHANGE_TIME,D2type,ActionHoldD2);//Ѱ��D2 �� D2��ͣ
						break;

				case ActionHoldD2:
						ActionHoldPoint(MAX_HOVER_ERR,Short_Hold_Time,ActionGoLeft);	//D2��ͣ �� �����ƶ�
						//ע��ActionHoldPoint���ERRû����
						break;
						
				case ActionGoLeft:
						ActionFormChange(MAX_FORMCHANGE_TIME,D1type,ActionHoldD1);//Ѱ��D1 �� D1��ͣ
						break;
							
				case ActionHoldD1:
						ActionHoldPoint(MAX_HOVER_ERR,Short_Hold_Time,ActionGoBack);	//D1��ͣ �� ����ƶ�
						break;
				
				case ActionGoBack:
						ActionFormChange(MAX_FORMCHANGE_TIME,A1type,ActionHoldA1);//Ѱ��A1 �� A1��ͣ
						break;
				
				case ActionHoldA1:
						ActionHoldPoint(MAX_HOVER_ERR,Short_Hold_Time,ActionGoRight);	//A1��ͣ �� �����ƶ�
						break;
				
				case ActionGoRight:
						ActionFormChange(MAX_FORMCHANGE_TIME,A3type,ActionHoldA3);//Ѱ��A3 �� A3��ͣ
						break;
				
				case ActionHoldA3:
						ActionHoldPoint(MAX_HOVER_ERR,Short_Hold_Time,ActionGoForward_Again);	//A3��ͣ �� �ٴ���ǰ�ƶ�
						break;
				
				case ActionGoForward_Again:
						ActionFormChange(MAX_FORMCHANGE_TIME,D3type,ActionHoldD3);//Ѱ��D3 �� D3��ͣ
						break;
				
				case ActionHoldD3:
						ActionHoldPoint(MAX_HOVER_ERR,Short_Hold_Time,ActionLand);//D3��ͣ �� ����
				
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
								//���в�ѭ��
								//HoldCurrentPostion(dt);

								program_ctrl.vel_cmps_h[X] = 20;	//ǰ
						}
						break;
						
				case ActionGoBack:
				case ActionGoBack_Again:
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);

								program_ctrl.vel_cmps_h[X] = -20;	//��
						}
						break;

				case ActionGoLeft:
				case ActionGoLeft_Again:
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[Y] = 20;	//��
						}
						break;

				case ActionGoRight:
				case ActionGoRight_Again:
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);

								program_ctrl.vel_cmps_h[Y] = -20;	//��
						}
						break;
				
				case ActionGoForwardRight:		
				case ActionGoForwardRight_Again:
						{
								program_ctrl.vel_cmps_h[Y] = -20;	//��
								program_ctrl.vel_cmps_h[X] = 20;	//ǰ
						}

				case ActionGoForwardLeft_MoreLeft:
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);

								program_ctrl.vel_cmps_h[X] = 10;	//ǰ
								program_ctrl.vel_cmps_h[Y] = 20;	//��
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
								//��������ֵ ��������ͣĿ�������Ļ������
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
    
    //���²�����
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

        //�����߽��뵹��ʱ״̬
        if (CloseGate)
        {
            CloseGate = false;
            FollowManager.ActionList = ActionCountdown;
        }
    }
}











