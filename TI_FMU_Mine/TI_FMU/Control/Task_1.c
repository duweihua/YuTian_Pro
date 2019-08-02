/*
�������������
1. �Զ����
2. ����ǰ���ƶ���Ѱ��B3��
3. ��B3���Ϸ���ͣ2s����
4. ����ǰ��(ƫ��)�ƶ���Ѱ��C1��
5. ��C1���Ϸ���ͣ2s����
6. ����ǰ���ƶ���Ѱ��D2��
7. ��D2���Ϸ���ͣ2s����
8. ����ƶ���Ѱ��A2��
9. ��A2���Ϸ���ͣ2s����
10.����
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

		//��ȡ��ǰ��⵽�����ͺ�����(����ָ��)
    FollowManager.ptrFrame = (OpenMVFrame_t *)UsartGroup[UART_A3].RxBuff;
		//��ȡ��ǰ�ɿ���״̬(��̬���߶ȡ�ң�ص�)
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
						//�˴�ʱ��Ϊ���ʱ��
						ActionHoldPoint(MAX_HOVER_ERR, Take_off_Time, ActionHoverStartPoint);
        }
        break;

        case ActionHoverStartPoint:
						ActionHoldPoint(MAX_HOVER_ERR,200,ActionGoForwardRight);//�����ͣ �� ����ǰ�ƶ�
            break;

        case ActionGoForwardRight:
						ActionFormChange(MAX_FORMCHANGE_TIME,B3type,ActionHoldB3);//Ѱ��B3 �� B3��ͣ
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
						ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoLeft_Again);	//C1��ͣ �� �ٴ�����ǰ�ƶ�
						break;

				case ActionGoLeft_Again:
						g_LedManager.emLEDStatus = StatusOff;
						ActionFormChange(MAX_FORMCHANGE_TIME,C1type,ActionHoldC1);
						break;

				case ActionHoldC1:
						g_LedManager.emLEDStatus = StatusOn;
						ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoForwardRight_Again);	//C1��ͣ �� �ٴ�����ǰ�ƶ�
						break;
				
				case ActionGoForwardRight_Again:
						g_LedManager.emLEDStatus = StatusOff;
						ActionFormChange(50,D2type,ActionHoldD2);//Ѱ��D2 �� D2��ͣ
						break;
				
				case ActionHoldD2:
						g_LedManager.emLEDStatus = StatusOn;
						ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoBack);	//D2��ͣ �� ����ƶ�
						break;
				
				case ActionGoBack:
						g_LedManager.emLEDStatus = StatusOff;
						ActionFormChange(BIGGER_FORMCHANGE_TIME,A2type,ActionHoldA2);//Ѱ��A2 �� A2��ͣ
						break;
				
				case ActionHoldA2:
						g_LedManager.emLEDStatus = StatusOn;
						ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionLand);	//A2��ͣ �� ����
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
								//���в�ѭ��
								//HoldCurrentPostion(dt);

								program_ctrl.vel_cmps_h[X] = 20;	//ǰ
								program_ctrl.vel_cmps_h[Y] = -20;	//��
						}
						break;
										
				case ActionGoForwardRight_Again:
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);

								program_ctrl.vel_cmps_h[X] = 20;	//ǰ
								program_ctrl.vel_cmps_h[Y] = -20;	//��
						}
						break;
						
				case ActionGoLeft:
				case ActionGoRight:
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);
							
								if(FollowManager.ActionList == ActionGoLeft)
										program_ctrl.vel_cmps_h[Y] = 20;
								else if(FollowManager.ActionList == ActionGoRight)
										program_ctrl.vel_cmps_h[Y] = -20;
						}
						break;
						
				case ActionGoLeft_Again:
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[Y] = 20;
						}
						break;
						
				case ActionGoForward:
				case ActionGoBack:
						{
								//���в�ѭ��
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
								//��������ֵ ������С������Ļ������
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
								//��������ֵ ������С������Ļ������
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
								//��������ֵ ������С������Ļ������
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
								//��������ֵ ������С������Ļ������
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
								//��������ֵ ������С������Ļ������
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
								//��������ֵ ������С������Ļ������
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
				ZKHD_Printf("��Ŀ��    ");				
}


void HoldCurrentPostion(float dt)
{
    static float OldPos[2];
    
    //���²�����
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

        //�����߽��뵹��ʱ״̬
        if (CloseGate)
        {
            CloseGate = false;
            FollowManager.ActionList = ActionCountdown;
        }
    }
}











