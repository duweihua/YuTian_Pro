
//�ⲿ�ļ�����
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
//�궨����
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
//Extern����

//˽�к�����
void Update(void);
void UpdateUSBQueue(void);

//˽�б�����
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

    //��ʼ����飬���ʧ�ܣ�������LED����
    InitCheck;

    //�������м��
    KernelRunningCheck;

    KernelRunning = false;
        
    //��ѯ�Ƿ���������Ҫ����
//    ANO_DT_Data_Receive_Anl_Task();
    
    //ʱ�����ѯ����
    Cnt++;
    
    //333Hz
    if (Cnt % 3 == 0)                    
    {
        //��ȡMPU6050��ԭʼ����
        GetMPU6050Data();
        
        //Z������Ԥ��
        WZ_Est_Calcu(0.003f);

        //��̬���¡�����Ԫ��
        ATT_Update(&g_MPUManager,&g_Attitude, 0.003f); 
        
        //����PID����������
        FlightPidControl(0.003f);
        
        //����߼�����
        MotorControl();
    }
    
    //125Hz
    if (Cnt % 8 == 0)
    {
        //����ŷ����
        GetAngle(&g_Attitude);
    }

    //91Hz
    if (Cnt % 11 == 0)
    {
        //����״̬��������
        AnoOF_State_Task(11);
    }
    
    //100Hz
    if (Cnt % 10 == 0)
    {
        //��ѵGCS����
        PollingGCS();
        
				//�̿�������ѯ
        UpdateCentControl(0.01f);
    }
    
    //50Hz����
    if (Cnt % 20 == 0)
    {
        //��ѹ�Ƹ���
        UpdateSPL06Info();

        //�۲⴫�������ݼ���
        WZ_Obs_Calcu(0.02f);
        
        //Z�����ݻ�������
        WZ_Fix_Calcu(0.02f);
        
        //�߶ȿ�����
        ALT_Ctrl(0.02f);        
        
        //λ�ÿ�����
        POS_Ctrl(0.02f);
    }
    
    //20Hz����
    if (Cnt % 50 == 0)
    {            
        //һ�����Ƴ���
        One_Key_Take_off_Land_Ctrl_Task(50);
        
        //�̿�����
        Program_Ctrl_Task(50); 
    }

    //LED��ѵ����
    PollingLED();
    
    //������ѵ����
    PollingUSART();
		
//		LED���Դ����		
//		if(Cnt % 2000 == 0)
//		{
//				g_LedManager.emLEDStatus = StatusOn;
//		}
//		else if(Cnt % 1000 == 0)
//		{
//				g_LedManager.emLEDStatus = StatusOff;
//		}
		
//		//�ⱨ������
//		g_LedManager.emLEDStatus = StatusOn;
//		
//		//����������
//		MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN6);
}


