/* Includes */
#include "delay.h"
#include "uart.h"


/* data define */


int main( void )
{
	vDelayConfig();
	
	vUart1Config();
	
	vUart3Config();
	
	while( 1 )
	{		
		;
	}
}




