/*----------------------------------------------------------------------------------------------------------------------/
*               ������ֻ��������ѧϰʹ�ã���Ȩ����Ȩ���������ƴ��Ŷӣ������ƴ��Ŷӽ��ɿس���Դ���ṩ�������ߣ�
*               ������ҪΪ�����ƴ��Ŷ��ṩ������δ��������ɣ����ý�Դ�����ṩ�����ˣ����ý�Դ����ŵ����Ϲ�����������أ� 
*               �������Դ�����Ĳ�����緢��������Ϊ�������ƴ��Ŷӽ���֮�Է��ɽ��������
-----------------------------------------------------------------------------------------------------------------------/
*               ������Ϣ���ܶ���ֹ��ǰ�����������˳���������
*               ��Դ���ף���ѧ����ϧ��ף������Ϯ�����׳ɹ�������
*               ѧϰ�����ߣ��������Ƽ���DJI��ZEROTECH��XAG��AEE��GDU��AUTEL��EWATT��HIGH GREAT�ȹ�˾��ҵ
*               ��ְ�����뷢�ͣ�15671678205@163.com���豸ע��ְ����λ����λ��������
*               �����ƴ���Դ�ɿ�QQȺ��540707961
*               CSDN���ͣ�http://blog.csdn.net/u011992534
*               �ſ�ID��NamelessCotrun����С��
*               Bվ��ѧ��Ƶ��https://space.bilibili.com/67803559/#/video
*               �ͻ�ʹ���ĵá��Ľ������������http://www.openedv.com/forum.php?mod=viewthread&tid=234214&extra=page=1
*               �Ա����̣�https://shop348646912.taobao.com/?spm=2013.1.1000126.2.5ce78a88ht1sO2
*               �ٶ�����:�����ƴ���Դ�ɿ�
*               �޸�����:2018/8/30
*               �汾���۷��ߡ���V1.0.1
*               ��Ȩ���У�����ؾ���
*               Copyright(C) �人�Ƽ���ѧ�����ƴ��Ŷ� 2017-2025
*               All rights reserved
----------------------------------------------------------------------------------------------------------------------*/
#ifndef _CONTROL_POSHOLD_H
#define _CONTROL_POSHOLD_H

float ncq_speed_mapping(float input,uint16_t input_max,float output_max);
void ncq_control_poshold(void);
uint8_t get_stopping_point_xy(Vector3f *stopping_point);
void accel_to_lean_angles(Vector2f _accel_target,Vector2f *target_angle);


extern Vector3_Nav Earth_Frame_Accel_Target;   //��������������ϵ����������������Ŀ���˶����ٶ�����
extern Vector3_Nav Earth_Frame_Pos_Err;        //��������������ϵ����������������wλ��ƫ��
extern Vector2_Ang Body_Frame_Accel_Target;      //��������������ϵ��������(Y��)������(X��)����Ŀ���˶����ٶ�����
extern Vector2_Ang Body_Frame_Speed_Feedback;    //��������������ϵ��������(Y��)������(X��)����Ŀ���˶��ٶȷ���
extern Vector2_Ang Body_Frame_Pos_Err;           //���巽����λ��ƫ��
extern Vector2_Ang Body_Frame_Brake_Speed;       //���巽����ɲ���ٶ�
extern uint8 GPS_Speed_Control_Mode;
extern Vector2f accel_target,angle_target;
#endif

