#include "Headfile.h"

//while��1���������Ҫ����
//����Ҫ������ָ������û���ϸ�Ҫ�����ִ��ʱ�����غ�ʱ���Ӻ���
//���磺��ѹ�ɼ�������ɨ�衢��ʾ��ˢ�¡�����վ���͡����ٶȼƱ궨�������Ʊ궨��ң�����г̱궨�����������
int main(void)
{
	//оƬ��Դ �����ʼ��
  WP_Init();
	
  while(1)
  {	
    Get_Battery_Voltage();//������ص�ѹ		
    Key_Scan(Key_Right_Release);//����ɨ��
    QuadShow();//OLED��ʾ
    Vcan_Send();//ɽ�����վ���๦�ܵ������֣�
    ANO_SEND_StateMachine();//ANO����վ����
    Accel_Calibartion();//���ٶȼ�6��У׼
    Mag_Calibartion_LS(&WP_Sensor.mag_raw,Circle_Angle);//����������У׼
    RC_Calibration_Check(PPM_Databuf);//ң�����г�У׼
    Save_Or_Reset_PID_Parameter();//���õ���վ�޸Ŀ��Ʋ���
  }
}


