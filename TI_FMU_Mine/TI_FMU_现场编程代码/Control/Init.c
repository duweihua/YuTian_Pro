
//硬件初始化驱动使用时只需要调用Hadrware_Init即可


//外部文件引用
#include "include.h"
#include "gcs.h"
#include "led.h"
#include "spl06.h"
#include "communication.h"
#include "battery.h"
#include "FollowLine.h"
#include "Remote.h"

//宏定义区
#define HARDWARE_CHECK_LED    LED_STATUS_ON;LED_POWER_ON;Delay_ms(100);\
                              LED_STATUS_OFF;LED_POWER_OFF;Delay_ms(100);\
                              LED_STATUS_ON;LED_POWER_ON;Delay_ms(100);\
                              LED_STATUS_OFF;LED_POWER_OFF;Delay_ms(100);

//Extern引用
extern bool InitComplete;

//私有函数区
void PID_Init(void); 

//私有变量区
uint32_t SysTick_count; //系统时间计数
Queue_t USB_Send_Queue;


void BEEPInit(void)
{
	  //设置为输出
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4,GPIO_PIN6);
	
		//初始拉低 蜂鸣器不响
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN6);
}

void MagnetInit(void)
{
	  //设置为输出
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4,GPIO_PIN4);
	
		//初始拉低 电磁铁不吸引
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN4);
}


void Hadrware_Init(void)
{
    LEDInit();                      //LED闪灯初始化
		BEEPInit();											//蜂鸣器初始化
		MagnetInit();										//电磁铁初始化
    MPU6050Init();                  //6050数据初始化
    SPL06Init();                    //SPL06初始化
    PID_Init();                     //PID参数初始化
//		Remote_Init();									//遥控器解放模式初始化
    Follow_Init();									//跟踪参数初始化
    
    //判断MPU6050和SPL06初始化是否成功
    if(g_MPUManager.Check && device_SPL06.Device.bEnable)
    {
        InitComplete = true;
    }
}


void PID_Init(void)
{
    PIDGroup[emPID_Pitch_Pos].kp   = 8.0f;
    PIDGroup[emPID_Pitch_Spd].kp   = 1.1f;
    PIDGroup[emPID_Pitch_Spd].ki   = 1.1f;
    PIDGroup[emPID_Pitch_Spd].kd   = 0.06f;
    PIDGroup[emPID_Pitch_Spd].IntegLimitHigh = 10;
    PIDGroup[emPID_Pitch_Spd].IntegLimitLow = -10;

    PIDGroup[emPID_Roll_Pos].kp    = 8.0f;
    PIDGroup[emPID_Roll_Spd].kp    = 1.1f;
    PIDGroup[emPID_Roll_Spd].ki    = 1.0f;
    PIDGroup[emPID_Roll_Spd].kd    = 0.04f;
    PIDGroup[emPID_Roll_Spd].IntegLimitHigh = 10;
    PIDGroup[emPID_Roll_Spd].IntegLimitLow = -10;

    PIDGroup[emPID_Yaw_Pos].kp     = 8.0f;
    PIDGroup[emPID_Yaw_Spd].kp     = 2.0f;
    PIDGroup[emPID_Yaw_Spd].kd     = 0.00f;
    PIDGroup[emPID_Yaw_Spd].OutLimitHigh = 100;
    PIDGroup[emPID_Yaw_Spd].OutLimitLow = -100;

    PIDGroup[emPID_Roll_Spd].IntegLimitHigh = 50; 
    PIDGroup[emPID_Roll_Spd].IntegLimitLow = -50;
}















