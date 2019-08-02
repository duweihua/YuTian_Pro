
//外部文件引用
#include "include.h"
#include "communication.h"
#include "gcs.h"
#include "speed_estimator.h"
#include "led.h"
#include "battery.h"
#include "stdio.h"
#include "HARDWARE_uart.h"
#include "SPL06.h"
#include "timer_drv.h"
#include "Ano_OF.h"
#include "pos_ctrl.h"
#include "program_ctrl.h"
#include "ANO_GCS_DT.h"
#include "FollowLine.h"
//宏定义区
#define InitCheck        if(!InitComplete)\
                        {\
                            P2OUT |= GPIO_PIN1;\
                            P2OUT |= GPIO_PIN2;\
                            P2OUT |= GPIO_PIN0;\
                            P1OUT |= GPIO_PIN0;\
                             return ;\
                        }\

#define KernelRunningCheck  if(!KernelRunning)\
                                return;\
//Extern引用

//私有函数区
void Update(void);
void UpdateUSBQueue(void);

//私有变量区
bool KernelRunning = false;
bool InitComplete = false;
int16_t Angle_Int16[3];
uint8_t Buff[20];
int sum = 0;


uint32_t test_time[5];
uint32_t test_runtime[3];
uint8_t data[5] = {0xAA,0xBB,0xCC,0xDD,0xEE};

void KernelPolling()
{
    static uint32_t Cnt = 0;

    //初始化检查，如果失败，则所有LED亮起
    InitCheck;

    //核心运行检查
    KernelRunningCheck;

    KernelRunning = false;
        
    //查询是否有数据需要解析
//    ANO_DT_Data_Receive_Anl_Task();
    
    //时间段轮询计数
    Cnt++;
    
    //333Hz
    if (Cnt % 3 == 0)                    
    {
        //获取MPU6050的原始数据
        GetMPU6050Data();
        
        //Z轴数据预估
        WZ_Est_Calcu(0.003f);

        //姿态更新——四元数
        ATT_Update(&g_MPUManager,&g_Attitude, 0.003f); 
        
        //飞行PID控制器更新
        FlightPidControl(0.003f);
        
        //电机逻辑控制
        MotorControl();
    }
    
    //125Hz
    if (Cnt % 8 == 0)
    {
        //更新欧拉角
        GetAngle(&g_Attitude);
    }

    //91Hz
    if (Cnt % 11 == 0)
    {
        //光流状态更新任务
        AnoOF_State_Task(11);
    }
    
    //100Hz
    if (Cnt % 10 == 0)
    {
        //轮训GCS任务
        PollingGCS();
        
				//程控任务轮询
        UpdateCentControl(0.01f);
    }
    
    //50Hz任务
    if (Cnt % 20 == 0)
    {
        //气压计更新
        UpdateSPL06Info();

        //观测传感器数据计算
        WZ_Obs_Calcu(0.02f);
        
        //Z轴数据互补修正
        WZ_Fix_Calcu(0.02f);
        
        //高度控制器
        ALT_Ctrl(0.02f);        
        
        //位置控制器
        POS_Ctrl(0.02f);
    }
    
    //20Hz任务
    if (Cnt % 50 == 0)
    {            
        //一键控制程序
        One_Key_Take_off_Land_Ctrl_Task(50);
        
        //程控任务
        Program_Ctrl_Task(50); 
    }

    //LED轮训函数
    PollingLED();
    
    //串口轮训函数
    PollingUSART();
		
//		LED测试代码段		
//		if(Cnt % 2000 == 0)
//		{
//				g_LedManager.emLEDStatus = StatusOn;
//		}
//		else if(Cnt % 1000 == 0)
//		{
//				g_LedManager.emLEDStatus = StatusOff;
//		}
		
//		//光报警测试
//		g_LedManager.emLEDStatus = StatusOn;
//		
//		//声报警测试
//		MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN6);
}


