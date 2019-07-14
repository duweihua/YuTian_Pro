#ifndef __LED_H__
#define __LED_H__

#define LED1_OFF()  GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define LED1_ON()   GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define LED1_TOGGLE()  GPIOB->ODR ^= GPIO_Pin_12

#define LED2_OFF()  GPIO_SetBits(GPIOB,GPIO_Pin_13)
#define LED2_ON()   GPIO_ResetBits(GPIOB,GPIO_Pin_13)
#define LED2_TOGGLE()  GPIOB->ODR ^= GPIO_Pin_13

void LED_Configuration(void);

#endif 
