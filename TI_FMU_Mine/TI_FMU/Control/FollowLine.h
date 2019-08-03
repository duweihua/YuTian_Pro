#ifndef __FOLLOW_LINE_H
#define __FOLLOW_LINE_H

#include "stdint.h"
#include "stdbool.h"
#include "pid.h"
#include "gcs.h"
#include "program_ctrl.h"


//����غ궨��(ʱ�䵥λ:10ms)
#define MAX_COUNTDOWN       		500		//��ɵ���ʱʱ��
#define Take_off_Time						200		//���ʱ��
#define Take_off_Hold_Time			150		//�����ͣʱ��
#define MAX_TIMEOUT1        		400		//���������ʱ��
extern s32 EXP_TAKEOFF_ALT_CM;				//�Զ���ɸ߶�(Task_2019.c�ж���)


//��ͣ��غ궨��(ʱ�䵥λ:10ms)
#define Move_Hold_Time_Shrot		30		//�ƶ���ͣʱ��
#define Move_Hold_Time					100		
#define Move_NoJudge_Hold_Time	400
#define Static_Hold_Time 				150		//������ͣʱ��
#define Judge_Err								30		//��ͣ�������


//ȷ�ϼ�������غ궨��
#define Short_Formchange_Time 	3			//�������ȷ��Ŀ�����
#define Middle_Formchange_Time 	15
#define Long_Formchange_Time 		50


//OpenMV��ͷ������غ궨��
#define PosVertically_Offset 		15		//ǰ����ͷ����(��ֵʹ�ɻ�����ƫǰ)
#define PosHorizontall_Offset 	0			//���ҷ���ͷ����(��ֵʹ�ɻ�����ƫ��)


//�ƶ��ٶ���غ궨��
#define Straight_Forward_Speed			32
#define Straight_Back_Speed					32
#define Straight_Left_Speed					32
#define Straight_Right_Speed				32
#define Straight_Up_Speed						8
#define Straight_Down_Speed					8

#define ForwardLeft_Speed						25
#define ForwardRight_Speed					25
#define BackLeft_Speed							25
#define BackRight_Speed							25

//��ǰƫ��
#define ForwardLeft_MoreLeft_Left_Speed						25
#define ForwardLeft_MoreLeft_Forward_Speed				13
//��ǰƫǰ
#define ForwardLeft_MoreForward_Left_Speed				13
#define ForwardLeft_MoreForward_Forward_Speed			25
//��ǰƫ��
#define ForwardRight_MoreRight_Right_Speed				25
#define ForwardRight_MoreRight_Forward_Speed			13
//��ǰƫǰ
#define ForwardRight_MoreForward_Right_Speed			13
#define ForwardRIght_MoreForward_Forward_Speed		25
//���ƫ��
#define BackLeft_MoreLeft_Left_Speed							25
#define BackLeft_MoreLeft_Back_Speed							13
//���ƫ��
#define BackLeft_MoreBack_Left_Speed							13
#define BackLeft_MoreBack_Back_Speed							25
//�Һ�ƫ��
#define BackRight_MoreRight_Right_Speed						25
#define BackRight_MoreRight_Back_Speed						13
//�Һ�ƫ��
#define BackRight_MoreBack_Right_Speed						13
#define BackRight_MoreBack_Back_Speed							25

#define TurnLeft_Speed					0.2f		//(20deg/s)
#define TurnRight_Speed					0.2f		//(20deg/s)


//���ú궨��
#define TARGETALTITUDECM    		50
#define MAX_HOVER_ERR       		10
#define MAX_HOVER_TIME      		300
#define MAX_TIMEOUT2        		1500
#define MAX_ALT_ERR         		10


//�����
typedef enum
{
    CmdNone = 0,
    CmdTakeOff,
    CmdLand,
    CmdUp,
    CmdDown,
    CmdForward,
    CmdBack,
    CmdLeft,
    CmdRight,
    CmdTurnLeft,		
    CmdTurnRight,		
    CmdEmergencyShutDown,
    CmdSpeeedControl,
    
    NumofCmd,
}FMUCmd_t;


//ͼ�ζ���
typedef enum
{
    Vertical  = 0,      
    Horizontal ,        
    Cross,              
    Ttype,              
    TurnTtype,          
    Ltype,              
    MirrorFlipLtype,    
    TurnLtype,          
    MirrorFlipTurnLtype,
    LeftTtype,
	
    ApriTag = 100,
  
    NumofForm,
}FormType_t;


//��������
typedef enum
{
		//����������1
    ActionWaitting = 0,
    ActionCountdown,
    ActionTakeOff,
    ActionHoverStartPoint,
    
    //��ͣ����
    ActionHoldLtype,
    ActionHoldMirrorFlipLtype,
    ActionHoldTurnLtype,
    ActionHoldMirrorFlipTurnLtype,
    ActionHoldCross,
    ActionHoldTtype,
    ActionHoldTurnTtype,
    ActionHoldFeaturePoint,
    ActionHoldLeftTtype,
	
    ActionHoldApriTag,
			
		//���ж���
    ActionGoForward,									//ǰ
		ActionGoForward_Again,			
		ActionGoForward_Third,			
    ActionGoBack,											//��
		ActionGoBack_Again,						
		ActionGoBack_Third,						
    ActionGoLeft,											//��
		ActionGoLeft_Again,					
		ActionGoLeft_Third,					
    ActionGoRight,										//��
		ActionGoRight_Again,				
		ActionGoRight_Third,		
		ActionGoUp,												//��
		ActionGoUp_Again,								
		ActionGoUp_Third,	
		ActionGoDown,											//��
		ActionGoDown_Again,							
		ActionGoDown_Third,			
		ActionGoForwardRight,							//��ǰ
		ActionGoForwardRight_Again,			
		ActionGoForwardRight_Third,			
		ActionGoForwardLeft,							//��ǰ
		ActionGoForwardLeft_Again,				
		ActionGoForwardLeft_Third,			
		ActionGoBackRight,								//�Һ�
		ActionGoBackRight_Again,				
		ActionGoBackRight_Third,			
		ActionGoBackLeft,									//���
		ActionGoBackLeft_Again,
		ActionGoBackLeft_Third,
		ActionGoForwardLeft_MoreLeft,			//��ǰƫ��
		ActionGoForwardLeft_MoreForward,	//��ǰƫǰ
		ActionGoForwardRight_MoreRight,		//��ǰƫ��
		ActionGoForwardRight_MoreForward,	//��ǰƫǰ
		ActionGoBackLeft_MoreLeft,				//���ƫ��
		ActionGoBackLeft_MoreBack,				//���ƫ��
		ActionGoBackRight_MoreRight,			//�Һ�ƫ��
		ActionGoBackRight_MoreBack,				//�Һ�ƫ��
		
		//ת��
		ActionTurnLeft,
		ActionTurnLeft_Again,
		ActionTurnLeft_Third,
		ActionTurnRight,
		ActionTurnRight_Again,
		ActionTurnRight_Third,

		//�����ƶ�(û��Ҫ����)
		ActionGoForward_Distance,
		ActionGoBack_Distance,
		ActionGoLeft_Distance,
		ActionGoRight_Distance,
		ActionGoUp_Distance,
		ActionGoDown_Distance,

		//����������2
    ActionHoverStopPoint,
    ActionFollowTarget,
    ActionLostTargetInfo,
    ActionLand,
    ActionLock,
    ActionTest,
    ActionSonar,
    ActionEmergencyLand,

    NumofActionList,
}FSMList_t;


//����ͬһ������������
typedef enum
{
    ActionTimes1 = 0,
    ActionTimes2,
    ActionTimes3,
    ActionTimes4,
}ActionTimes_t;


//Ѳ��������궨��
typedef struct
{
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
}Line_t;


//Ѱ��������궨��
typedef struct
{
    int16_t x1;
    int16_t y1;
}Point_t;


//���ݽṹ����
#pragma pack (1)
typedef struct
{
    int16_t Start;
    uint16_t Cnt;
    FormType_t FormType;
    Point_t CentPoint;
    int16_t End;
}OpenMVFrame_t;
#pragma pack ()    


typedef struct
{
    uint16_t SonarF;
    uint16_t SonarB;
    uint16_t SonarL;
    uint16_t SonarR;
    
}SonarManager_t;


typedef struct
{
    OpenMVFrame_t *ptrFrame;
    UAV_info_t *ptrUAVInfo;
    FSMList_t ActionList;
    int16_t CountDownNumMs;
    int16_t TargetAltitudeCM;
    int16_t WatchDogCnt;
    PIDInfo_t   *ptrPIDInfoV;
    PIDInfo_t   *ptrPIDInfoH;
}FollowManager_t;


extern FollowManager_t FollowManager;
extern SonarManager_t SonarManager;

void UpdateCentControl(float dt);
void Follow_Init(void);

#endif










