功能：同时读取Guidance数据（光流的速度数据和超声波的高度数据）
	  和匿名光流的数据(采用北醒大功率激光)
	  
说明：使用开发板：STM32F407VET6

	  使用串口：USART2（guidance）、UART4（ANO_OF）
	  
	  guidance说明：
		电源：3~6S
		接口：TX口接开发板的PD6（USART2_RX）
		自制转接线：白线 GND
					棕线 PD6
		串口波特率：115200
		
	  ANO_OF说明：
		电源：5V
		接口：TX口接开发板的PA1（UART4_RX）
		自制转接线：黄色 GND
					红色 PA1
		串口波特率：500000（要求500000以上）
	  