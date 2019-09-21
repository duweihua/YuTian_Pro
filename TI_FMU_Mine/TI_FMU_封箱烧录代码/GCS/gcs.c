/**
  ******************************************************************************
  * Copyright (c) 2018,北京中科浩电科技有限公司
  * All rights reserved.
  * 文件名称：gcs.c
  * 摘    要：
  *
  * 当前版本：V1.0
  * 作    者：北京中科浩电科技有限公司研发部 
  * 完成日期：    
  * 修改说明：
  * 
  *
  * 历史版本：
  *
  *
  *******************************************************************************/

/*==============================================================================
                         ##### How to use this driver #####
==============================================================================
地面站驱动可按如下方式使用：
1.调用 gcs_init() 函数，以初始化指针指向；
2.将地面站发送到飞控的数据写入函数 ZKHD_Link_GCS_To_FMU_Handle() 中；
3.以一定周期调用如下函数即可在地面站中看到飞控当前信息；
UpdateToGCSPidInfo();
UpdateToGCSAttitude();
UpdateToGCSAltitude();
UpdateToGCSMPU6050();
UpdateToGCSMotor();
UpdateToGCSHardwareInfo();
UpdateToGCSFreq();
*/
//外部文件引用
#include "gcs.h"
#include "ZKHD_Link.h"
#include "height_control.h"
#include "battery.h"
#include "Ano_OF.h"
#include "HARDWARE_uart.h"
#include "ANO_DT.h"
//宏定义区



//Extern引用
extern Queue_t USB_Send_Queue;
extern PIDInfo_t *(pPidObject[]);
extern int16_t motor[4];
extern _ano_of_st ANO_OF;
//私有函数区



//私有变量区
UAV_info_t g_UAVinfo;
uint8_t PID_select;
PID_Config_t *PID_Config_Info;
float *base = 0;

/******************************************************************************
  * 函数名称：gcs_init
  * 函数描述：初始化地面站相关指针指向
  * 输    入：void
  * 输    出：void
  * 返    回：void
  * 备    注：null    
  *    
  *
******************************************************************************/
void gcs_init(void)
{
    g_UAVinfo.Alt = &HeightInfo;
    g_UAVinfo.AngE = &g_Attitude;
    g_UAVinfo.Firmware_Ver = FIRMWARE_INFO;
    g_UAVinfo.Motor = motor;
    g_UAVinfo.Mpu6050 = &g_MPUManager;
#ifdef NRF24L01        
    g_UAVinfo.NRF_MannagerPtr = &g_NRFManager;
#endif
    g_UAVinfo.Remote = &Remote;
    g_UAVinfo.FMUflg = &g_FMUflg;
    g_UAVinfo.BatteryInfoPtr = &g_BatteryInfo;
}

/******************************************************************************
  * 函数名称：gcs_ReceiveHandle
  * 函数描述：地面站接收处理函数
  * 输    入：ZKHD_Link_Head_t *ZKHD_Link_Info：ZKHD_Link协议头
              uint8_t *ptr：实际数据地址
              uint8_t length：实际数据长度
  * 输    出：void
  * 返    回：void
  * 备    注：null   
  *
  *
******************************************************************************/
void gcs_ReceiveHandle(ZKHD_Link_Head_t *ZKHD_Link_Info, uint8_t *ptr, uint8_t length)
{    
    switch(ZKHD_Link_Info->Message_ID)
    {
        case MsgID_0_Set_PID:
            PID_Config_Info = (PID_Config_t*)ptr;
        
            base = &PIDGroup[PID_Config_Info->PID_Select + 3 * PID_Config_Info->PID_Pos_Rate].kp;
            base += PID_Config_Info->PID_Offset;
            *base += PID_Config_Info->PID_Set / 10.0f;
            break;
        case MsgID_1_Select_PID:
            PID_select = *ptr;
            break;
        case MsgID_2_Set_RxAddr:
            g_UAVinfo.NRF_MannagerPtr->update_rx_Addr(ptr);
            break;
        case MsgID_3_Set_RxFreq:
            g_UAVinfo.NRF_MannagerPtr->update_frequency(ptr[0]);
            break;
        case MsgID_4_Set_TxAddr:
            g_UAVinfo.NRF_MannagerPtr->update_tx_Addr(ptr);
            break;
        default:
            break;
    }
}

/******************************************************************************
  * 函数名称：UpdateToGCSPidInfo
  * 函数描述：更新飞控PID信息到地面站中
  * 输    入：void
  * 输    出：void
  * 返    回：void
  * 备    注：null     
  *    
  *
******************************************************************************/
void UpdateToGCSPidInfo()
{
    static uint8_t yaobai = 0;
    int16_t RawBuff[8];
    uint8_t SendBuff[32];
    uint8_t SendLength;
    
    float *ptr = &PIDGroup[PID_select + yaobai * 3].kp;

    if(yaobai)
    {
        yaobai = 0;
    }
    else
    {
        yaobai = 1;
    }
    
    RawBuff[0] = PID_select;
    RawBuff[0] |= yaobai << 8;
    
    for(int i = 0; i < 7; i++)
    {
        RawBuff[i + 1] = (int16_t)(*ptr++ * 100);
    }
    
    ZKHD_Link_MakeFrame( Device_FMU,
                        Device_GCS,
                        MsgID_6_PID_Info,
                        (uint8_t*)RawBuff,
                        16,
                        SendBuff,
                        &SendLength
                        );

    enQueue(&USB_Send_Queue, SendBuff, SendLength);
}

/******************************************************************************
  * 函数名称：PollingGCS
  * 函数描述：轮询发送GCS的数据
  * 输    入：void
  * 输    出：void
  * 返    回：void 
  * 备    注：固定10ms运行一次
  *    
  *
******************************************************************************/
int16_t FbAlt = 0;
int16_t ExpAlt = 0;
void PollingGCS()
{
    static uint32_t Cnt = 0;
   
    if(Cnt % 50 == 0)
    {
//        UpdateToGCSHardwareInfo();
//        UpdateToGCSFreq();
//        UpdateToGCSPidInfo();
//        ANO_DT_Send_RCData(Remote.thr,Remote.yaw,Remote.roll,Remote.pitch,Remote.AUX1,Remote.AUX2,Remote.AUX3,Remote.AUX4,Remote.AUX5,Remote.AUX6);
//        ANO_DT_Send_MotoPWM(g_UAVinfo.Motor[0],g_UAVinfo.Motor[1],g_UAVinfo.Motor[2],g_UAVinfo.Motor[3],0,0,0,0);
//        
    }
    
    if(Cnt % 10 == 0)
    {
        ANO_DT_Send_Senser(g_UAVinfo.Mpu6050->accX,g_UAVinfo.Mpu6050->accY,g_UAVinfo.Mpu6050->accZ,g_UAVinfo.Mpu6050->gyroX,g_UAVinfo.Mpu6050->gyroY,g_UAVinfo.Mpu6050->gyroZ,0,0,0);
        ANO_DT_Send_Status(g_UAVinfo.AngE->roll, g_UAVinfo.AngE->pitch, g_UAVinfo.AngE->yaw, HeightInfo.Z_Postion, g_UAVinfo.UAV_Mode, 0);
    }
    
    Cnt++;
    UpdateUSBQueue();
}

/******************************************************************************
  * 函数名称：UpdateUSBQueue
  * 函数描述：从USB队列中取出数据发送
  * 输    入：void
  * 输    出：void
  * 返    回：void 
  * 备    注：固定周期运行    
  *    
  *
******************************************************************************/
void UpdateUSBQueue()
{
#ifdef USB_CONNECT
    uint8_t Buff[QUEUE_DATA_MAXLENGTH];
    uint8_t length = 0;
    
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
    
    if (hcdc->TxState == 0)
    {
        if (deQueue(&USB_Send_Queue, Buff,&length))
        {
            CDC_Transmit_FS(Buff, length);
        }
    }
#endif
#ifdef RADIO
    uint8_t Buff[MAX_RECEIVE_CNT];
    uint8_t length = 0;
    
    if (deQueue(&USB_Send_Queue, Buff,&length))
    {
        USART_TX(&UsartGroup[UART_GCS], Buff, length);
    }
    
#endif
}

/******************************************************************************
  * 函数名称：UpdateToGCSAttitude
  * 函数描述：更新飞控姿态信息到地面站中
  * 输    入：void
  * 输    出：void
  * 返    回：void
  * 备    注：null      
  *    
  *
******************************************************************************/
void UpdateToGCSAttitude(void)
{
    uint8_t Buff[20];
    int16_t Angle_Int16[3];
    int sum = 0;
    Buff[0] = 0xAA; //帧头
    Buff[1] = 6;   //长度低八位
    Buff[2] = 0;    //长度高八位
    Buff[3] = 1;    //发送者ID
    Buff[4] = 0;    //接收者ID
    Buff[5] = 0;    //消息ID

    Angle_Int16[0] = (int16_t)(-g_Attitude.pitch * 100);
    Angle_Int16[1] = (int16_t)(-g_Attitude.roll * 100);
    Angle_Int16[2] = (int16_t)(g_Attitude.yaw * 100);

    memcpy(Buff + 6,(uint8_t*)Angle_Int16, 6);

    for(int i = 0; i<((Buff[1]|Buff[2] <<8) + 6); i++)
    {
        sum += Buff[i];
    }

    Buff[(Buff[1]|Buff[2] << 8) + 6] = sum & 0xFF;
    sum = 0;
    
    enQueue(&USB_Send_Queue, Buff,(Buff[1]|Buff[2] << 8) + 7);
}

/******************************************************************************
  * 函数名称：UpdateToGCSAltitude
  * 函数描述：更新飞控高度信息到地面站中
  * 输    入：void
  * 输    出：void
  * 返    回：void
  * 备    注：null      
  *    
  *
******************************************************************************/
void UpdateToGCSAltitude(void)
{
    uint8_t DataBuff[20];
    uint8_t SendBuff[20];
    uint8_t SendLength;
    volatile int16_t Alt = 0;
    
    Alt = (int16_t)(g_UAVinfo.Alt->Alt * 100);
    
    memcpy(DataBuff,(uint8_t *)&Alt, 2);

    ZKHD_Link_MakeFrame( Device_FMU,
                    Device_GCS,
                    MsgID_4_Alt_Info,
                    DataBuff,
                    2,
                    SendBuff,
                    &SendLength
                   );
    
    enQueue(&USB_Send_Queue, SendBuff, SendLength);
}

/******************************************************************************
  * 函数名称：UpdateToGCSMPU6050
  * 函数描述：更新g_MPUManager传感器信息到地面站中
  * 输    入：void
  * 输    出：void
  * 返    回：void
  * 备    注：null      
  *    
  *
******************************************************************************/
void UpdateToGCSMPU6050(void)
{
    uint8_t DataBuff[20];
    uint8_t SendBuff[20];
    uint8_t SendLength;
    
    memcpy(DataBuff,(uint8_t*)g_UAVinfo.Mpu6050, sizeof(MPU6050Manager_t));

    ZKHD_Link_MakeFrame( Device_FMU,
                    Device_GCS,
                    MsgID_2_Sensor_Info,
                    DataBuff,
                    12,
                    SendBuff,
                    &SendLength
                   );
    
    enQueue(&USB_Send_Queue, SendBuff, SendLength);
}

/******************************************************************************
  * 函数名称：UpdateToGCSMotor
  * 函数描述：更新电机输出信息到地面站中
  * 输    入：void
  * 输    出：void
  * 返    回：void
  * 备    注：null      
  *
  *
******************************************************************************/
void UpdateToGCSMotor(void)
{
    uint8_t DataBuff[8];
    uint8_t SendBuff[20];
    uint8_t SendLength;
    
    memcpy(DataBuff, (uint8_t *)motor, sizeof(int16_t)*4);

    ZKHD_Link_MakeFrame( Device_FMU,
                    Device_GCS,
                    MsgID_1_Motor_Info,
                    DataBuff,
                    8,
                    SendBuff,
                    &SendLength
                   );
//    if(u)
    enQueue(&USB_Send_Queue, SendBuff, SendLength);
}

/******************************************************************************
  * 函数名称：UpdateToGCSHardwareInfo
  * 函数描述：更新硬件输出信息到地面站中
  * 输    入：void
  * 输    出：void
  * 返    回：void
  * 备    注：null      
  *
  *
******************************************************************************/
void UpdateToGCSHardwareInfo(void)
{
    uint8_t DataBuff[13];
    uint8_t SendBuff[20];
    uint8_t SendLength;
    
    DataBuff[0] = g_UAVinfo.Firmware_Ver;//版本信息
    memcpy(DataBuff + 1, g_UAVinfo.NRF_MannagerPtr->Rx_Addr, 5);//发送地址
    memcpy(DataBuff + 1 + 5, g_UAVinfo.NRF_MannagerPtr->Tx_Addr, 5);//发送地址
    DataBuff[11] = g_UAVinfo.Runtime&0xFF;
    DataBuff[12] = g_UAVinfo.Runtime>>8;

    ZKHD_Link_MakeFrame( Device_FMU,
                        Device_GCS,
                        MsgID_5_Hardware_Info,
                        DataBuff,
                        13,
                        SendBuff,
                        &SendLength
                       );
    
    enQueue(&USB_Send_Queue, SendBuff, SendLength);
}

/******************************************************************************
  * 函数名称：UpdateToGCSFreq
  * 函数描述：更新飞控输出NRF24L01频率信息到地面站中
  * 输    入：void
  * 输    出：void
  * 返    回：void
  * 备    注：null      
  *
  *
******************************************************************************/
void UpdateToGCSFreq()
{
    uint8_t SendBuff[20];
    uint8_t SendLength;
    
    ZKHD_Link_MakeFrame( Device_FMU,
                    Device_GCS,
                    MsgID_7_Freq,
                    &g_UAVinfo.NRF_MannagerPtr->RC_Frequency,
                    1,
                    SendBuff,
                    &SendLength
                    );
    
    enQueue(&USB_Send_Queue, SendBuff, SendLength);
}

/******************************************************************************
  * 函数名称：UpdateToGCSLine
  * 函数描述：更新曲线绘制信息到地面站中
  * 输    入：int16_t l1:曲线1
              int16_t l2:曲线2
              int16_t l3:曲线3
              int16_t l2:曲线4
              int16_t l3:曲线5
              int16_t l1:曲线6
              int16_t l2:曲线7
              int16_t l3:曲线8
  * 输    出：void
  * 返    回：void
  * 备    注：null      
  *
  *
******************************************************************************/
void UpdateToGCSLine(int16_t l1, int16_t l2, int16_t l3, int16_t l4, int16_t l5, int16_t l6, int16_t l7, int16_t l8)
{
    int16_t DataBuff[8];
    uint8_t SendBuff[30];
    uint8_t SendLength;
    
    DataBuff[0] = l1;
    DataBuff[1] = l2;
    DataBuff[2] = l3;
    DataBuff[3] = l4;
    DataBuff[4] = l5;
    DataBuff[5] = l6;
    DataBuff[6] = l7;
    DataBuff[7] = l8;
    

    ZKHD_Link_MakeFrame( Device_FMU,
                    Device_GCS,
                    MsgID_3_Line_Info,
                    (uint8_t*)DataBuff,
                    16,
                    SendBuff,
                    &SendLength
                    );
    
    enQueue(&USB_Send_Queue, SendBuff, SendLength);
}

/******************************************************************************
  * 函数名称：UpdateToGCSLine2
  * 函数描述：更新曲线绘制信息到地面站中，此函数适用于山外调试助手
  * 输    入：int16_t l1:曲线1
              int16_t l2:曲线2
              int16_t l3:曲线3
              int16_t l2:曲线4
              int16_t l3:曲线5
              int16_t l1:曲线6
              int16_t l2:曲线7
              int16_t l3:曲线8
  * 输    出：void
  * 返    回：void
  * 备    注：null  
  *
******************************************************************************/
void UpdateToGCSLine2(int16_t l1, int16_t l2, int16_t l3, int16_t l4, int16_t l5, int16_t l6, int16_t l7, int16_t l8)
{
    int16_t DataBuff[8];
    uint8_t SendBuff[30];
    
    DataBuff[0] = l1;
    DataBuff[1] = l2;
    DataBuff[2] = l3;
    DataBuff[3] = l4;
    DataBuff[4] = l5;
    DataBuff[5] = l6;
    DataBuff[6] = l7;
    DataBuff[7] = l8;
    
    SendBuff[0] = 03;
    SendBuff[1] = 0xFC;
    SendBuff[18] = 0xFC;
    SendBuff[19] = 0x03;
    memcpy(SendBuff + 2,(uint8_t*)DataBuff, 16);
    enQueue(&UsartGroup[UART_A1].qTx, SendBuff, 20);
}

/******************* (C) 版权所有 2018 北京中科浩电科技有限公司 *******************/
