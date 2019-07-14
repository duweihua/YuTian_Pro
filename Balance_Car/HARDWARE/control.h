#ifndef __CONTROL_H
#define __CONTROL_H


#define Motor1_Pwm   TIM3->CCR1  //PA6
#define Motor2_Pwm   TIM3->CCR2  //PA7


void control_init(void);


#endif


