/*
�������������
1. ����Key1 ִ�л�������1��A2->B3->C1->D2->A2
2. ����Key2 ִ�л�������2��A2->A3->D3->D1->A1->A2->C2->A2
3. ����Key  ִ�л������� 
4. ����Key  ִ�л������� 
5. ����Key  ִ�л������� 
6. ����Key  ִ�л������� 
7. ����Key  ִ�л������� 
8. ����Key  ִ�л������� 
9. ����Key  ִ�л������� 
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
void ActionHoldPoint_Judge(int8_t Err, int16_t HoldTime, FSMList_t NextAction);
void ActionHoldPoint_NoJudge(int8_t Err, int16_t HoldTime, FSMList_t NextAction);
void ActionHoldPoint_Judge_Back(int8_t Err, int16_t HoldTime, FSMList_t NextAction);
bool ActionFormChange_Clear(int8_t HoldTime, FormType_t TargetFormType, FSMList_t NextAction);
bool ActionFormChange_NoClear(int8_t HoldTime, FormType_t TargetFormType, FSMList_t NextAction);
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

		//���治ͬ���������˲�ͬ�ľ�ͷƫ��
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
    if (FollowManager.ptrFrame->CentPoint.x1 > 200 || FollowManager.ptrFrame->CentPoint.y1 > 200)
        return;

    UpdateButton();
    UpdateStatus();
    UpdateAction(dt);
}


extern UAV_info_t g_UAVinfo;								//������״̬�ṹ��
extern FollowManager_t FollowManager;				//�̿ع��ܿ��ƽṹ��
extern LedManager_t g_LedManager;						//LED���ƽṹ��
extern float PIDGroup_desired_yaw_pos_tmp;	//Yaw��Ƕȵ�����
s32 EXP_TAKEOFF_ALT_CM = 80;								//�Զ���ɸ߶�(����ʱ����������������ֵ)

void UpdateStatus()
{	
		static uint16_t forward_distance = 0;
		static uint16_t turnleft_cnt = 0;
	
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
								ActionHoldPoint_NoJudge(MAX_HOVER_ERR,Take_off_Time,ActionHoverStartPoint);
						}
						break;

        case ActionHoverStartPoint:
						{
								ActionHoldPoint_NoJudge(MAX_HOVER_ERR,Take_off_Hold_Time,ActionTurnLeft);
						}
            break;
						
        case ActionTurnLeft:
						{
								turnleft_cnt++;
							
								if(turnleft_cnt>=900)
								{
										turnleft_cnt = 900;
										
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


void UpdateAction(float dt)
{
    switch (FollowManager.ActionList)
    {
				case ActionWaitting:
						break;

				case ActionTakeOff:
						UpdateCMD(0,0,CmdTakeOff);
						break;

				case ActionHoverStartPoint://���ٶ���ͣ
						{
								program_ctrl.vel_cmps_h[Y] = 0;
								program_ctrl.vel_cmps_h[X] = 0;
						}
						break;
						
				case ActionGoForward:
				case ActionGoForward_Again:
				case ActionGoForward_Third:
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[X] = +Straight_Forward_Speed;					//ǰ
						}
						break;
						
				case ActionGoBack:
				case ActionGoBack_Again:
				case ActionGoBack_Third:
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[X] = -Straight_Back_Speed;						//��
						}
						break;

				case ActionGoLeft:
				case ActionGoLeft_Again:
				case ActionGoLeft_Third:					
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[Y] = +Straight_Left_Speed;						//��
						}
						break;

				case ActionGoRight:
				case ActionGoRight_Again:
				case ActionGoRight_Third:
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[Y] = -Straight_Right_Speed;						//��
						}
						break;
						
				case ActionGoUp:
				case ActionGoUp_Again:
				case ActionGoUp_Third:
						{
								program_ctrl.vel_cmps_h[Z] = +Straight_Up_Speed;							//��
						}
						break;
						
				case ActionGoDown:
				case ActionGoDown_Again:
				case ActionGoDown_Third:
						{
								program_ctrl.vel_cmps_h[Z] = -Straight_Down_Speed;						//��
						}
						break;

				case ActionGoForwardRight:		
				case ActionGoForwardRight_Again:
				case ActionGoForwardRight_Third:
						{
								program_ctrl.vel_cmps_h[Y] = -ForwardRight_Speed;							//��ǰ
								program_ctrl.vel_cmps_h[X] = +ForwardRight_Speed;	
						}
						break;

				case ActionGoForwardLeft:		
				case ActionGoForwardLeft_Again:
				case ActionGoForwardLeft_Third:
						{
								program_ctrl.vel_cmps_h[Y] = +ForwardLeft_Speed;							//��ǰ
								program_ctrl.vel_cmps_h[X] = +ForwardLeft_Speed;								
						}
						break;
						
				case ActionGoBackRight:		
				case ActionGoBackRight_Again:
				case ActionGoBackRight_Third:
						{
								program_ctrl.vel_cmps_h[Y] = -BackRight_Speed;								//�Һ�
								program_ctrl.vel_cmps_h[X] = -BackRight_Speed;
						}
						break;

				case ActionGoBackLeft:		
				case ActionGoBackLeft_Again:
				case ActionGoBackLeft_Third:
						{
								program_ctrl.vel_cmps_h[Y] = +BackLeft_Speed;									//���
								program_ctrl.vel_cmps_h[X] = -BackLeft_Speed;	
						}
						break;
						
				case ActionGoForwardLeft_MoreLeft:
						{
								program_ctrl.vel_cmps_h[X] = +ForwardLeft_MoreLeft_Forward_Speed;				//��ǰƫ��
								program_ctrl.vel_cmps_h[Y] = +ForwardLeft_MoreLeft_Left_Speed;	
						}
						break;
						
				case ActionGoForwardLeft_MoreForward:
						{							
								program_ctrl.vel_cmps_h[X] = +ForwardLeft_MoreForward_Forward_Speed;		//��ǰƫǰ
								program_ctrl.vel_cmps_h[Y] = +ForwardLeft_MoreForward_Left_Speed;	
						}
						break;
						
				case ActionGoForwardRight_MoreRight:
						{							
								program_ctrl.vel_cmps_h[X] = +ForwardRight_MoreRight_Forward_Speed;			//��ǰƫ��
								program_ctrl.vel_cmps_h[Y] = -ForwardRight_MoreRight_Right_Speed;	
						}
						break;
						
				case ActionGoForwardRight_MoreForward:
						{							
								program_ctrl.vel_cmps_h[X] = +ForwardRIght_MoreForward_Forward_Speed;		//��ǰƫǰ
								program_ctrl.vel_cmps_h[Y] = -ForwardRight_MoreForward_Right_Speed;	
						}
						break;
						
				case ActionGoBackLeft_MoreLeft:
						{							
								program_ctrl.vel_cmps_h[X] = -BackLeft_MoreLeft_Back_Speed;							//���ƫ��
								program_ctrl.vel_cmps_h[Y] = +BackLeft_MoreLeft_Left_Speed;		
						}
						break;
						
				case ActionGoBackLeft_MoreBack:
						{							
								program_ctrl.vel_cmps_h[X] = -BackLeft_MoreBack_Back_Speed;							//���ƫ��
								program_ctrl.vel_cmps_h[Y] = +BackLeft_MoreBack_Left_Speed;	
						}
						break;
						
				case ActionGoBackRight_MoreRight:
						{							
								program_ctrl.vel_cmps_h[X] = -BackRight_MoreRight_Back_Speed;						//�Һ�ƫ��
								program_ctrl.vel_cmps_h[Y] = -BackRight_MoreRight_Right_Speed;	
						}
						break;
						
				case ActionGoBackRight_MoreBack:
						{							
								program_ctrl.vel_cmps_h[X] = -BackRight_MoreBack_Back_Speed;						//�Һ�ƫ��
								program_ctrl.vel_cmps_h[Y] = -BackRight_MoreBack_Right_Speed;	
						}
						break;
						
				case ActionTurnLeft:
				case ActionTurnLeft_Again:
				case ActionTurnLeft_Third:
						{
								PIDGroup_desired_yaw_pos_tmp += TurnLeft_Speed;					//��ת
						}
						break;
					
				case ActionTurnRight:
				case ActionTurnRight_Again:
				case ActionTurnRight_Third:
						{
								PIDGroup_desired_yaw_pos_tmp -= TurnRight_Speed;				//��ת
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
										//��������ֵ ��������ͣĿ�������Ļ������
										PIDGroup[emPID_FolloLinePosVertically].desired = 120/2 + PosVertically_Offset;
										PIDGroup[emPID_FolloLinePosHorizontally].desired = 160/2 + PosHorizontall_Offset;
								
										HoldCurrentPostion(dt);
								}
								else
								{
										program_ctrl.vel_cmps_h[X] = 0;
										program_ctrl.vel_cmps_h[Y] = 0;
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
				
		//��ӡ������Ϣ
		if(FollowManager.ptrFrame->FormType == 0xFF)
				ZKHD_Printf("��Ŀ��      ");	
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
    
    UpdatePID(FollowManager.ptrPIDInfoH,dt);
    UpdatePID(FollowManager.ptrPIDInfoV,dt);
    
    PIDGroup[emPID_FolloLineSpdVertically].desired = FollowManager.ptrPIDInfoV->out;
    PIDGroup[emPID_FolloLineSpdHorizontally].desired = FollowManager.ptrPIDInfoH->out;
    
    UpdatePID(&PIDGroup[emPID_FolloLineSpdHorizontally],dt);
    UpdatePID(&PIDGroup[emPID_FolloLineSpdVertically],dt);

    program_ctrl.vel_cmps_h[Y] = PIDGroup[emPID_FolloLineSpdHorizontally].out;
    program_ctrl.vel_cmps_h[X] = PIDGroup[emPID_FolloLineSpdVertically].out;
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

        //�����߽��뵹��ʱ״̬
        if (CloseGate)
        {
            CloseGate = false;
            FollowManager.ActionList = ActionCountdown;
        }
    }
}














