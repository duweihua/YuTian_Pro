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
#ifndef _CONTROL_LAND_H
#define _CONTROL_LAND_H

#define Faraway_Distance 600//����home��Զ����ΪA����
#define Near_Distance 150//����home��Զ����ΪB����
#define Arrive_Distance 0//�ﵽhome�㣬��ΪC����


#define First_Nav_Rate   100//��A->B������һ��Ѳ���ٶȣ���λcm
#define Second_Nav_Rate  80//��B->C����������Ѳ���ٶȣ���λcm
#define Third_Nav_Rate   60//����C�㸽������������Ѳ���ٶ�, ��λcm
#define Default_Nav_rate 50//Ĭ��Ѳ���ٶ�


#define Nav_Safety_Height 800//���뷵�����Զʱ����������İ�ȫѲ���߶ȣ���λcm
#define Nav_Climb_Rate  80//�����뷵�����Զʱ���ҵ�ǰ�߶�С��Nav_Safety_Heightʱ��ԭ������ʱ���ٶ�
#define Nav_Decline_Rate -40//��������home�����Ϸ�ʱ��ԭ���½�������ʱ���ٶ�
#define Nav_Rapid_Decline_Rate -80//�����½�������ʱ���ٶ�


#define Nav_Transition_Period 400//5ms*400=2S


void land_state_check(void);
void land_reset(void);
void landon_earth_check(void);
void land_run(void);
bool GPS_ok(void);
bool land_althold(float taret_climb_rate,float target_climb_alt);


#endif

