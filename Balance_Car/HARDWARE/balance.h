#ifndef __BALANCE_H__
#define __BALANCE_H__


#include "main.h"


void all_control(void);
int balance_control(float Pitch_angle,float Pitch_speed);
int velocity_control(int encoder_left,int encoder_right);
int straight_control(int encodor_left,int encodor_right,float az);
int turn_control(int encoder_left,int encoder_right,float az);
u8 abnormal_control(float angle);
unsigned char pick_up(float gz,float Angle,int encoder_left,int encoder_right);
unsigned char put_down(float Angle,int encoder_left,int encoder_right);



#endif 
