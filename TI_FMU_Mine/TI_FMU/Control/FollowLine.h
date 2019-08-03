#ifndef __FOLLOW_LINE_H
#define __FOLLOW_LINE_H

#include "stdint.h"
#include "stdbool.h"
#include "pid.h"
#include "gcs.h"
#include "program_ctrl.h"


//起降相关宏定义(时间单位:10ms)
#define MAX_COUNTDOWN       		500		//起飞倒计时时间
#define Take_off_Time						200		//起飞时间
#define Take_off_Hold_Time			150		//起飞悬停时间
#define MAX_TIMEOUT1        		400		//降落后上锁时间
extern s32 EXP_TAKEOFF_ALT_CM;				//自动起飞高度(Task_2019.c中定义)


//悬停相关宏定义(时间单位:10ms)
#define Move_Hold_Time_Shrot		30		//移动悬停时间
#define Move_Hold_Time					100		
#define Move_NoJudge_Hold_Time	400
#define Static_Hold_Time 				150		//定点悬停时间
#define Judge_Err								30		//悬停允许误差


//确认检测次数相关宏定义
#define Short_Formchange_Time 	3			//连续检测确认目标次数
#define Middle_Formchange_Time 	15
#define Long_Formchange_Time 		50


//OpenMV镜头矫正相关宏定义
#define PosVertically_Offset 		15		//前后方向镜头矫正(正值使飞机定点偏前)
#define PosHorizontall_Offset 	0			//左右方向镜头矫正(正值使飞机定点偏左)


//移动速度相关宏定义
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

//左前偏左
#define ForwardLeft_MoreLeft_Left_Speed						25
#define ForwardLeft_MoreLeft_Forward_Speed				13
//左前偏前
#define ForwardLeft_MoreForward_Left_Speed				13
#define ForwardLeft_MoreForward_Forward_Speed			25
//右前偏右
#define ForwardRight_MoreRight_Right_Speed				25
#define ForwardRight_MoreRight_Forward_Speed			13
//右前偏前
#define ForwardRight_MoreForward_Right_Speed			13
#define ForwardRIght_MoreForward_Forward_Speed		25
//左后偏左
#define BackLeft_MoreLeft_Left_Speed							25
#define BackLeft_MoreLeft_Back_Speed							13
//左后偏后
#define BackLeft_MoreBack_Left_Speed							13
#define BackLeft_MoreBack_Back_Speed							25
//右后偏右
#define BackRight_MoreRight_Right_Speed						25
#define BackRight_MoreRight_Back_Speed						13
//右后偏后
#define BackRight_MoreBack_Right_Speed						13
#define BackRight_MoreBack_Back_Speed							25

#define TurnLeft_Speed					0.2f		//(20deg/s)
#define TurnRight_Speed					0.2f		//(20deg/s)


//无用宏定义
#define TARGETALTITUDECM    		50
#define MAX_HOVER_ERR       		10
#define MAX_HOVER_TIME      		300
#define MAX_TIMEOUT2        		1500
#define MAX_ALT_ERR         		10


//命令定义
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


//图形定义
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


//动作定义
typedef enum
{
		//基本动作组1
    ActionWaitting = 0,
    ActionCountdown,
    ActionTakeOff,
    ActionHoverStartPoint,
    
    //悬停动作
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
			
		//飞行动作
    ActionGoForward,									//前
		ActionGoForward_Again,			
		ActionGoForward_Third,			
    ActionGoBack,											//后
		ActionGoBack_Again,						
		ActionGoBack_Third,						
    ActionGoLeft,											//左
		ActionGoLeft_Again,					
		ActionGoLeft_Third,					
    ActionGoRight,										//右
		ActionGoRight_Again,				
		ActionGoRight_Third,		
		ActionGoUp,												//上
		ActionGoUp_Again,								
		ActionGoUp_Third,	
		ActionGoDown,											//下
		ActionGoDown_Again,							
		ActionGoDown_Third,			
		ActionGoForwardRight,							//右前
		ActionGoForwardRight_Again,			
		ActionGoForwardRight_Third,			
		ActionGoForwardLeft,							//左前
		ActionGoForwardLeft_Again,				
		ActionGoForwardLeft_Third,			
		ActionGoBackRight,								//右后
		ActionGoBackRight_Again,				
		ActionGoBackRight_Third,			
		ActionGoBackLeft,									//左后
		ActionGoBackLeft_Again,
		ActionGoBackLeft_Third,
		ActionGoForwardLeft_MoreLeft,			//左前偏左
		ActionGoForwardLeft_MoreForward,	//左前偏前
		ActionGoForwardRight_MoreRight,		//右前偏右
		ActionGoForwardRight_MoreForward,	//右前偏前
		ActionGoBackLeft_MoreLeft,				//左后偏左
		ActionGoBackLeft_MoreBack,				//左后偏后
		ActionGoBackRight_MoreRight,			//右后偏右
		ActionGoBackRight_MoreBack,				//右后偏后
		
		//转向
		ActionTurnLeft,
		ActionTurnLeft_Again,
		ActionTurnLeft_Third,
		ActionTurnRight,
		ActionTurnRight_Again,
		ActionTurnRight_Third,

		//定长移动(没必要开发)
		ActionGoForward_Distance,
		ActionGoBack_Distance,
		ActionGoLeft_Distance,
		ActionGoRight_Distance,
		ActionGoUp_Distance,
		ActionGoDown_Distance,

		//基本动作组2
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


//进入同一动作次数定义
typedef enum
{
    ActionTimes1 = 0,
    ActionTimes2,
    ActionTimes3,
    ActionTimes4,
}ActionTimes_t;


//巡线相关坐标定义
typedef struct
{
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
}Line_t;


//寻点相关坐标定义
typedef struct
{
    int16_t x1;
    int16_t y1;
}Point_t;


//数据结构声明
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










