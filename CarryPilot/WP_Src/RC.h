#ifndef __RC_H__
#define __RC_H__

#define  Scale_Pecent_Max  0.75   //��������ֵ��������
#define  Pit_Rol_Max 30           //��������������
#define  Yaw_Max     200          //���ƫ������
#define  Buttom_Safe_Deadband  50 //���ŵײ���ȫ����


#define Lock_Controler  0
#define Unlock_Controler  1
extern uint16 Controler_State;
extern int16_t Throttle_Rate;
extern uint16_t Auto_ReLock_Cnt;//�Զ�����������
extern uint16_t Unlock_Makesure_Cnt,Lock_Makesure_Cnt,Forced_Lock_Makesure_Cnt;
extern uint16_t Unwanted_Lock_Flag;
extern uint16 Throttle_Control;
extern int16 Pitch_Control,Roll_Control,Yaw_Control;
extern uint16_t PPM_LPF_Databuf[4];
extern uint16 RC_NewData[4];
extern int16 Target_Angle[2];
void Remote_Control(void);
#endif

