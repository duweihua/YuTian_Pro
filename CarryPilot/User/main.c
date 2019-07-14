#include "Headfile.h"

//while（1）里面非主要任务
//非主要任务是指对周期没有严格要求或者执行时间严重耗时的子函数
//例如：电压采集、按键扫描、显示屏刷新、地面站发送、加速度计标定、磁力计标定、遥控器行程标定、参数保存等
int main(void)
{
	//芯片资源 外设初始化
  WP_Init();
	
  while(1)
  {	
    Get_Battery_Voltage();//测量电池电压		
    Key_Scan(Key_Right_Release);//按键扫描
    QuadShow();//OLED显示
    Vcan_Send();//山外地面站（多功能调试助手）
    ANO_SEND_StateMachine();//ANO地面站发送
    Accel_Calibartion();//加速度计6面校准
    Mag_Calibartion_LS(&WP_Sensor.mag_raw,Circle_Angle);//磁力计椭球校准
    RC_Calibration_Check(PPM_Databuf);//遥控器行程校准
    Save_Or_Reset_PID_Parameter();//运用地面站修改控制参数
  }
}


