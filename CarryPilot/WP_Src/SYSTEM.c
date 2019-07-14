
/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/
/*----------------------------------------------------------------------------------------------------------------------/
*               本程序只供购买者学习使用，版权著作权属于无名科创团队，无名科创团队将飞控程序源码提供给购买者，
*               购买者要为无名科创团队提供保护，未经作者许可，不得将源代码提供给他人，不得将源代码放到网上供他人免费下载， 
*               更不能以此销售牟利，如发现上述行为，无名科创团队将诉之以法律解决！！！
-----------------------------------------------------------------------------------------------------------------------/
*               生命不息、奋斗不止；前人栽树，后人乘凉！！！
*               开源不易，且学且珍惜，祝早日逆袭、进阶成功！！！
*               学习优秀者，简历可推荐到DJI、ZEROTECH、XAG、AEE、GDU、AUTEL、EWATT、HIGH GREAT等公司就业
*               求职简历请发送：15671678205@163.com，需备注求职意向单位、岗位、待遇等
*               无名科创开源飞控QQ群：540707961
*               CSDN博客：http://blog.csdn.net/u011992534
*               优酷ID：NamelessCotrun无名小哥
*               B站教学视频：https://space.bilibili.com/67803559/#/video
*               客户使用心得、改进意见征集贴：http://www.openedv.com/forum.php?mod=viewthread&tid=234214&extra=page=1
*               淘宝店铺：https://shop348646912.taobao.com/?spm=2013.1.1000126.2.5ce78a88ht1sO2
*               百度贴吧:无名科创开源飞控
*               公司官网:www.nameless.tech
*               修改日期:2019/4/12
*               版本：躺赢者——CarryPilot_V1.0
*               版权所有，盗版必究。
*               Copyright(C) 2017-2025 武汉无名创新科技有限公司 
*               All rights reserved
*               重要提示：
*               正常淘宝咸鱼转手的飞控、赠送朋友、传给学弟的都可以进售后群学习交流，
*               不得直接在网上销售无名创新资料，无名创新代码有声明版权，他人不得将
*               资料代码传网上供他人下载，不得以谋利为目的销售资料代码，发现有此操
*               作者，公司会提前告知，请1天内及时处理，否则你的学校、单位、姓名、电
*               话、地址信息会被贴出在公司官网、官方微信公众平台、官方技术博客、知乎
*               专栏以及淘宝店铺首页予以公示公告，此种所作所为，会成为个人污点，影响
*               升学、找工作、社会声誉、很快就很在无人机界出名，后果很严重。
*               因此行为给公司造成重大损失者，会以法律途径解决，感谢您的合作，谢谢！！！
----------------------------------------------------------------------------------------------------------------------*/
#include "Headfile.h"
#include "SYSTEM.h"

/***********************************************************
@函数名：HardWave_Init
@入口参数：无
@出口参数：无
功能描述：对系统芯片资源常规硬件资源进行一系列初始化，具体见
各初始化后的注释
@作者：无名小哥
@日期：2019年01月27日
*************************************************************/
void HardWave_Init(void)
{
  ConfigureUART0();//串口0初始化，山外地面站、锡月地面站
	ADC_Init();//ADC初始化，检测外部电池电压
  OLED_Init();//OLED显示屏初始化
	Version_Declaration();//版本说明
  Butterworth_Parameter_Init();//滤波器参数初始化
  PPM_Init();//PPM解析初始化
  Init_PWM();//PWM初始化
  Bling_Init();//LED状态指示灯初始化
  Quad_Start_Bling();//开机LED预显示
  Key_Init();//板载按键初始化
  RC_Calibration_Trigger();//遥控器行程标定检查
  Horizontal_Calibration_Init();//机架水平标定初始化
  PID_Paramter_Init_With_Flash();//PID参数初始化
  Init_I2C();//硬件I2C初始化
  Init_MPU6050();//MPU6050陀螺仪、加速度计配置初始化
  IST8310_Init();//IST8310磁力计配置初始化
  SPL06_Init();//SPL06气压计配置初始化
  Parameter_Init();//加速度计、磁力计校准参数初始化
  WP_Quad_Init();//根据观测传感器加速度计、磁力计对姿态四元数初始化
  ConfigureUART1();//串口1初始化，匿名地面站
  ConfigureUART3();//串口3初始化，OPENMV、树莓派视觉模组、SDK模式串口
  Set_GPS_USART();//串口2初始化，使用UBLOX协议配置GPS模块
  ConfigureUART7();//串口7初始化.超声波模块/TOF解析串口	
  ConfigureUART6();//串口6初始化，光流模块LC306数据解析
  SDK_Init();//SDK模式初始化
  Time_init();//飞控调度定时器初始化
  delay_ms(200);//初始化延时	
}


/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/


