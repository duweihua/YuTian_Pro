#ifndef _US_100_H_
#define _US_100_H_


//US100
#define User_Height_Max  240//�û�ʹ�õ����߶�ֵ����λΪcm����ҪԤ��һ��ԣ�ȸ���ѹ��/�������л�����



#define US_100_Distance_CMD    0x55
#define US_100_Temperature_CMD 0x50

void US_100_Statemachine(void);



extern uint8_t US_100_Update_Flag;
extern float US_Distance,Last_US_Distance,US_Distance_Div,US_Distance_Acc;
#endif


