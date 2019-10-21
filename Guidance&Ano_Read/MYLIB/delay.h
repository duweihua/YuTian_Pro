/* Define to prevent recursive inclusion */
#ifndef __STM32F4_DELAY_H
#define __STM32F4_DELAY	_H

/* Includes */
#include "stm32f4xx.h"

/* Exported types */

/* Exported functions */
void delay_init(void);
void delay_us(u32 ntime);
void delay_ms(u32 ntime);

#endif

