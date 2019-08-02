#ifndef __FOLLOW_LINE_H
#define __FOLLOW_LINE_H

#include "stdint.h"
#include "stdbool.h"
#include "pid.h"
#include "gcs.h"

//��λ:10ms
#define MAX_COUNTDOWN       		500
#define TARGETALTITUDECM    		50
#define MAX_HOVER_ERR       		10
#define MAX_HOVER_TIME      		300
#define MAX_TIMEOUT1        		400
#define MAX_TIMEOUT2        		1500
#define MAX_ALT_ERR         		10

#define Short_Formchange_Time 	3
#define Middle_Formchange_Time 	15
#define Long_Formchange_Time 		50
#define Long_Formchange_Time_D3 100

#define Take_off_Time						200		//���ʱ��
#define Take_off_Hold_Time			150		//�����ͣʱ��

#define Move_Hold_Time_Shrot		30
#define Move_Hold_Time					100
#define Move_Hold_Time_A1				30
#define Move_Hold_Time_D3				240
#define Move_Hold_Time_Wood			300
#define Move_NoJudge_Hold_Time	400

#define Static_Hold_Time 				150

#define Judge_Err								30

#define PosVertically_Offset 		15
#define PosHorizontall_Offset 	0

/*

*/

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
    CmdTurnLeft,		//��ת
    CmdTurnRight,		//��ת
    CmdEmergencyShutDown,
    CmdSpeeedControl,
    
    NumofCmd,
}FMUCmd_t;

typedef enum
{
    Vertical  = 0,      //����
    Horizontal ,        //����
    Cross,              //ʮ��
    Ttype,              //T����
    TurnTtype,          //��T����
    Ltype,              //L����
    MirrorFlipLtype,    //����תL����
    TurnLtype,          //��L����
    MirrorFlipTurnLtype,//����ת��L����
    LeftTtype,
	
    ApriTag = 100,
  
		A1type = 0xA1,
		A2type = 0xA2,
		A3type = 0xA3,
		B1type = 0xB1,
		B2type = 0xB2,
		B3type = 0xB3,
		C1type = 0xC1,
		C2type = 0xC2,
		C3type = 0xC3,
		D1type = 0xD1,
		D2type = 0xD2,
		D3type = 0xD3,
		
		Woodtype = 0xBB,
	
    NumofForm,
}FormType_t;

typedef enum
{
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
		ActionHoldA1,
		ActionHoldA2,
		ActionHoldA2_Again,
		ActionHoldA2_Third,
		ActionHoldA3,
		ActionHoldB1,
		ActionHoldB2,
		ActionHoldB3,
		ActionHoldC1,
		ActionHoldC2,
		ActionHoldC3,
		ActionHoldD1,
		ActionHoldD2,
		ActionHoldD3,
		ActionHoldA1_Static,
		ActionHoldA2_Static,
		ActionHoldA3_Static,
		ActionHoldB1_Static,
		ActionHoldB2_Static,
		ActionHoldB3_Static,
		ActionHoldC1_Static,
		ActionHoldC2_Static,
		ActionHoldC3_Static,
		ActionHoldD1_Static,
		ActionHoldD2_Static,
		ActionHoldD3_Static,
		
		ActionHoldWood,
		ActionHoldWood_Again,
		
		//���ж���
    ActionGoForward,							//ǰ
		ActionGoForward_Again,		
    ActionGoBack,									//��
		ActionGoBack_Again,			
    ActionGoLeft,									//��
		ActionGoLeft_Again,			
    ActionGoRight,								//��
		ActionGoRight_Again,	
		ActionGoForwardRight,					//��ǰ
		ActionGoForwardRight_Again	,
		ActionGoForwardLeft,					//��ǰ
		ActionGoForwardLeft_Again,	
		ActionGoBackRight,						//�Һ�
		ActionGoBackRight_Again,	
		ActionGoBackLeft,							//���
		ActionGoBackLeft_Again,
		ActionGoForwardLeft_MoreLeft,	//��ǰƫ��
		ActionGoUp,										
		ActionGoUp_Again,							//��
		ActionGoDown,		
		ActionGoDown_Again,						//��
		
		//ת��(δ����)
		ActionTurnLeft,
		ActionTurnRight,

		//�����ƶ�(δ����)
		ActionGoForward_Distance,
		ActionGoBack_Distance,
		ActionGoLeft_Distance,
		ActionGoRight_Distance,
		ActionGoUp_Distance,
		ActionGoDown_Distance,

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

typedef enum
{
    ActionTimes1 = 0,
    ActionTimes2,
    ActionTimes3,
    ActionTimes4,
}ActionTimes_t;

typedef struct
{
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
}Line_t;

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










