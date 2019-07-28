//*****************************************************************************
//
// MSP432 main.c template - P1.0 and P2.0-2 ports toggle
//
//****************************************************************************

#include "msp.h"
#include "stdbool.h"
#include <math.h>
volatile uint32_t msTicks;                      /* counts 1ms timeTicks       */


/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;
}

/*----------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *----------------------------------------------------------------------------*/
void Delay (uint32_t dlyTicks) {                                              
  uint32_t curTicks;
  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
	
}

int main(void)
{
    float ii;
    float fCalculate;
    bool flipFlop;
    
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;           // Stop watchdog timer

    // The following code configures P1.0 and P2.0-2 ports
    P1->DIR |= BIT0;                      // Configure P1.0 as output  LED Port 1
		P2->DIR |= 7;                      		// Configure P2.0 - 2 as output  LED Port 2
		P1->OUT = 0;													// Turn LED P1 off
		P2->OUT = 0;													// Turn LED P2 off

		SystemCoreClockUpdate();
	  SysTick_Config(SystemCoreClock / 1000);       /* Systick interrupt each 1ms */
	
    while(1)
    {
        P1->OUT ^= BIT0;                  // LED1 Red ON P1.0
        Delay(500);												// Delay 500 msec (500 times 1 SysTick interrupt)
        P1->OUT = 0;                  		// LED1 Red OFF P1.0
        P2->OUT ^= BIT0;                  // LED2 Red ON P2.0
        Delay(500);
        P2->OUT = BIT1;                  	// LED2 Green ON P2.1
        Delay(500);
        P2->OUT = BIT2;                  	// LED2 Blue ON P2.2
        Delay(500);
        P2->OUT = 0;                  		// LED2 Blue OFF P2.2
    }
}
