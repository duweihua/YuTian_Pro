#ifndef __PWM_H
#define __PWM_H


#define AIN1   PAout(5)
#define AIN2   PAout(4)
#define BIN1   PBout(10)
#define BIN2   PBout(11)


void pwm_init(void);
int myabs(int a);
void Set_Pwm(int motor1,int motor2);


#endif


