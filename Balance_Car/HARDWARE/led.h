#ifndef __LED_H__
#define __LED_H__


#define LED_OFF()  		GPIO_SetBits(GPIOC,GPIO_Pin_15)
#define LED_ON()   		GPIO_ResetBits(GPIOC,GPIO_Pin_15)
#define LED_TOGGLE() 	GPIOC->ODR ^= GPIO_Pin_15


void led_init(void);


#endif 

