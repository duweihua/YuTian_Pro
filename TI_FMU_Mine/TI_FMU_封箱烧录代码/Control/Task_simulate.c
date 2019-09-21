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
    //�ж�OpenMV���ص������Ƿ���ã��е�ʱ��OpenMV�᷵����Ч����
    if (FollowManager.ptrFrame->CentPoint.x1 > 200 || FollowManager.ptrFrame->CentPoint.y1 > 200)
        return;

    //���°�ť����ʵ��
    UpdateButton();

    //���³̿�״̬��
    UpdateStatus();

    //���³̿ض�����
    UpdateAction(dt);
}


extern UAV_info_t g_UAVinfo;
extern uint8_t finish_flag;
extern FollowManager_t FollowManager;
extern LedManager_t g_LedManager;
//�Զ���ɸ߶�(�˴�Ϊ����)(����ʱ����������������ֵ)
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
										
										EXP_TAKEOFF_ALT_CM = 70;//��������Ҫ����и߶Ȳ�����50cm
								}
						}
						break;

        case ActionTakeOff:
						{
								//�˴�ʱ��Ϊ���ʱ��
								ActionHoldPoint_NoJudge(MAX_HOVER_ERR,Take_off_Time,ActionHoldA2);
						}
						break;

				//��������һ��ʼ
        case ActionHoldA2:
						{
								if((Key1==true)&&(Key2==false))	//��������1		
										ActionHoldPoint(MAX_HOVER_ERR,Take_off_Hold_Time,ActionGoForwardRight);//�����ͣ �� ����ǰ�ƶ�
								if((Key1==false)&&(Key2==true))	//��������2
										ActionHoldPoint(MAX_HOVER_ERR,Take_off_Hold_Time,ActionGoRight);//�����ͣ �� �����ƶ�
						}
            break;

        case ActionGoForwardRight:
						{
								if((Key1==true)&&(Key2==false))	//��������1
										ActionFormChange(Short_Formchange_Time,B3type,ActionHoldB3);//Ѱ��B3 �� B3��ͣ
						}
						break;

				case ActionHoldB3:
						{
								if((Key1==true)&&(Key2==false))	//��������1
										ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoForwardLeft_MoreLeft);	//B3��ͣ �� ����ǰƫ���ƶ�
						}
						break;
					
				case ActionGoForwardLeft_MoreLeft:
						{
								if((Key1==true)&&(Key2==false))	//��������1
										ActionFormChange(50,C1type,ActionHoldC1);//Ѱ��C1 �� C1��ͣ
						}
						break;
						
				case ActionHoldC1:
						{							
								if((Key1==true)&&(Key2==false))	//��������1
								{
										ActionHoldPoint(MAX_HOVER_ERR,600,ActionGoForwardRight_Again);	//C1��ͣ �� �ٴ�����ǰ�ƶ�
								}
						}
						break;

				case ActionGoForwardRight_Again:
						{
								if((Key1==true)&&(Key2==false))	//��������1
										ActionFormChange(Long_Formchange_Time,D2type,ActionHoldD2);//Ѱ��D2 �� D2��ͣ
						}
						break;
				
				case ActionHoldD2:
						{
								if((Key1==true)&&(Key2==false))	//��������1
										ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoBack);	//D2��ͣ �� ����ƶ�
						}
						break;

				case ActionGoBack:
						{
								if((Key1==true)&&(Key2==false))	//��������1	
										ActionFormChange(Long_Formchange_Time,A2type,ActionHoldA2_Again);//Ѱ��A2 �� �ٴ�A2��ͣ
								if((Key1==false)&&(Key2==true))	//��������2	
										ActionFormChange(Short_Formchange_Time,A1type,ActionHoldA1);//Ѱ��A1 �� A1��ͣ
						}
						break;

				case ActionHoldA2_Again:
						{
								if((Key1==true)&&(Key2==false))	//��������1
										ActionHoldPoint_Back(MAX_HOVER_ERR,Move_Hold_Time,ActionLand);	//A2��ͣ �� ����
								if((Key1==false)&&(Key2==true))	//��������2	
										ActionHoldPoint_Back(MAX_HOVER_ERR,Move_Hold_Time,ActionGoForward_Again);//A2��ͣ �� �ٴ���ǰ�ƶ�
						}
						break;
						
				//�����������ʼ
        case ActionGoRight:
						{
								if((Key1==false)&&(Key2==true))	//��������2
										ActionFormChange(Short_Formchange_Time,A3type,ActionHoldA3);//Ѱ��A3 �� A3��ͣ
						}
						break;

				case ActionHoldA3:
						{
								if((Key1==false)&&(Key2==true))	//��������2
										ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoForward);	//A3��ͣ �� ��ǰ�ƶ�
						}
						break;
						
				case ActionGoForward:
						{
								if((Key1==false)&&(Key2==true))	//��������2
										ActionFormChange(Long_Formchange_Time_D3,D3type,ActionHoldD3);//Ѱ��D3 �� D3��ͣ
						}
						break;

				case ActionHoldD3:
						{
								if((Key1==false)&&(Key2==true))	//��������2
										ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time_D3,ActionGoLeft);	//D3��ͣ �� �����ƶ�
						}
						break;

				case ActionGoLeft:
						{
								if((Key1==false)&&(Key2==true))	//��������2
										ActionFormChange(Long_Formchange_Time,D1type,ActionHoldD1);//Ѱ��D1 �� D1��ͣ
						}
						break;

				case ActionHoldD1:
						{
								if((Key1==false)&&(Key2==true))	//��������2
										ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoBack);	//D1��ͣ �� ����ƶ�
						}
						break;
						
				case ActionHoldA1:
						{
								if((Key1==false)&&(Key2==true))	//��������2
										ActionHoldPoint_Back(MAX_HOVER_ERR,Move_Hold_Time_A1,ActionGoRight_Again);//A1��ͣ �� �ٴ������ƶ�
						}
						break;
						
				case ActionGoRight_Again:
						{
								if((Key1==false)&&(Key2==true))	//��������2
										ActionFormChange(Long_Formchange_Time,A2type,ActionHoldA2_Again);//Ѱ��A2 �� �ٴ�A2��ͣ
						}
						break;

				case ActionGoForward_Again:
						{
								if((Key1==false)&&(Key2==true))	//��������2
										ActionFormChange(Middle_Formchange_Time,C2type,ActionHoldC2);//Ѱ��C2 �� C2��ͣ
						}
						break;
						
				case ActionHoldC2:
						{
								if((Key1==false)&&(Key2==true))	//��������2
										ActionHoldPoint(MAX_HOVER_ERR,Move_Hold_Time,ActionGoBack_Again);//C2��ͣ �� �ٴ�����ƶ�
						}
						break;
						
				case ActionGoBack_Again:
						{
								if((Key1==false)&&(Key2==true))	//��������2
										ActionFormChange(Long_Formchange_Time,A2type,ActionHoldA2_Third);//Ѱ��A2 �� ������A2��ͣ
						}
						break;

				case ActionHoldA2_Third:
						{
								if((Key1==false)&&(Key2==true))	//��������2
										ActionHoldPoint_Back(MAX_HOVER_ERR,Move_Hold_Time,ActionLand);//A2��ͣ �� ����
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
								//���в�ѭ��
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[X] = 32;	//ǰ
						}
						break;
						
				case ActionGoBack:
				case ActionGoBack_Again:
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[X] = -32;//-38;	//��
						}
						break;

				case ActionGoLeft:
				case ActionGoLeft_Again:
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[Y] = 24;	//��
						}
						break;

				case ActionGoRight:
				case ActionGoRight_Again:
						{
								//���в�ѭ��
								//HoldCurrentPostion(dt);
								program_ctrl.vel_cmps_h[Y] = -32;	//��
						}
						break;
				
				case ActionGoForwardRight:		
				case ActionGoForwardRight_Again:
						{
								program_ctrl.vel_cmps_h[Y] = -25;	//��
								program_ctrl.vel_cmps_h[X] = 25;	//ǰ
						}
						break;

				case ActionGoForwardLeft:		
				case ActionGoForwardLeft_Again:
						{
								program_ctrl.vel_cmps_h[Y] = 25;	//��
								program_ctrl.vel_cmps_h[X] = 25;	//ǰ
						}
						break;
						
				case ActionGoBackRight:		
				case ActionGoBackRight_Again:
						{
								program_ctrl.vel_cmps_h[Y] = -25;	//��
								program_ctrl.vel_cmps_h[X] = -25;	//��
						}
						break;

				case ActionGoBackLeft:		
				case ActionGoBackLeft_Again:
						{
								program_ctrl.vel_cmps_h[Y] = 25;	//��
								program_ctrl.vel_cmps_h[X] = -25;	//��
						}
						break;
						
				case ActionGoForwardLeft_MoreLeft://��ǰƫ��
						{							
								if(FollowManager.ptrFrame->FormType == B2type)
								{
										program_ctrl.vel_cmps_h[X] = 25;	//ǰ
										program_ctrl.vel_cmps_h[Y] = 25;	//��
								}
								else if(FollowManager.ptrFrame->FormType == C2type)
								{
										program_ctrl.vel_cmps_h[X] = 0;		//ǰ
										program_ctrl.vel_cmps_h[Y] = 25;	//��
								}
								else
								{
										program_ctrl.vel_cmps_h[X] = 15;	//ǰ
										program_ctrl.vel_cmps_h[Y] = 30;	//��
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
										//��������ֵ ��������ͣĿ�������Ļ������
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
										
				case ActionHoldA1:	//A1ȡ��ǰ�����ϵľ�ͷƫ��(�����յ�)
						{
								if(FollowManager.ptrFrame->FormType == A1type)
								{
										//��������ֵ ������С������Ļ������
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

				case ActionHoldA2:	//A2ǰ������ͣ�Ǻ��� ��ȡ����ͷƫ��(ʹ��λ��ƫǰ)
				case ActionHoldA2_Again:
						{
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
						}
						break;

				case ActionHoldA2_Third:	//A2��������ͣ�ں��˹����� ȡ����ͷƫ��(ʹ��λ��ƫ��)		
						{
								if(FollowManager.ptrFrame->FormType == A2type)
								{
										//��������ֵ ������С������Ļ������
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
						
				case ActionHoldA3:	//A3��ͣ���ں��˹����� ���־�ͷƫ��(ʹ��λƫǰ)
						{
								if(FollowManager.ptrFrame->FormType == A3type)
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
						}
						break;
						
				case ActionHoldB1:
						{
								if(FollowManager.ptrFrame->FormType == B1type)
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
						}
						break;
				
				case ActionHoldB2:
						{
								if(FollowManager.ptrFrame->FormType == B2type)
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
						}
						break;
						
				case ActionHoldB3:
						{
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
						}
						break;
						
				case ActionHoldC1:
						{
								if(FollowManager.ptrFrame->FormType == C1type)
								{
										//��������ֵ ������С������Ļ������
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
						}
						break;
						
				case ActionHoldC3:
						{
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
						}
						break;
						
				case ActionHoldD1:
						{
								if(FollowManager.ptrFrame->FormType == D1type)
								{
										//��������ֵ ������С������Ļ������
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
						}
						break;
						
				case ActionHoldD3:
						{
								if(FollowManager.ptrFrame->FormType == D3type)
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
					ZKHD_Printf("��   ");
				}
				else
				{
					ZKHD_Printf("С   ");
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

        //�����߽��뵹��ʱ״̬
        if (CloseGate)
        {
            CloseGate = false;
            FollowManager.ActionList = ActionCountdown;
        }
    }
}














