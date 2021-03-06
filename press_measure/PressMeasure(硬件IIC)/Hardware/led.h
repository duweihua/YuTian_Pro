#ifndef __LED_H__
#define __LED_H__


#define LED_OFF()  GPIO_SetBits(GPIOC,GPIO_Pin_13)
#define LED_ON()   GPIO_ResetBits(GPIOC,GPIO_Pin_13)
#define LED_TOGGLE()  GPIOC->ODR ^= GPIO_Pin_13


void LED_Configuration(void);


#endif 
