#ifndef __IIC_H
#define __IIC_H

#include "main.h"


//IMU_IIC_PORT
#define IMU_GPIO_PORT_IIC	GPIOB						
#define IMU_RCC_IIC_PORT 	RCC_APB2Periph_GPIOB		
#define IMU_IIC_SCL_PIN		GPIO_Pin_14			
#define IMU_IIC_SDA_PIN		GPIO_Pin_15

#define IMU_IIC_SCL_1()  GPIO_SetBits(IMU_GPIO_PORT_IIC,IMU_IIC_SCL_PIN)				
#define IMU_IIC_SCL_0()  GPIO_ResetBits(IMU_GPIO_PORT_IIC,IMU_IIC_SCL_PIN)				

#define IMU_IIC_SDA_1()  GPIO_SetBits(IMU_GPIO_PORT_IIC,IMU_IIC_SDA_PIN)				
#define IMU_IIC_SDA_0()  GPIO_ResetBits(IMU_GPIO_PORT_IIC,IMU_IIC_SDA_PIN)				

#define IMU_IIC_SDA_READ()  GPIO_ReadInputDataBit(IMU_GPIO_PORT_IIC,IMU_IIC_SDA_PIN)	


//¶ÁÐ´¿ØÖÆÎ»
#define IIC_WR	0
#define IIC_RD	1	


//OLED_IIC_PORT
#define OLED_GPIO_PORT_IIC	GPIOB						
#define OLED_RCC_IIC_PORT 	RCC_APB2Periph_GPIOB		
#define OLED_IIC_SCL_PIN	GPIO_Pin_10			
#define OLED_IIC_SDA_PIN	GPIO_Pin_11

#define OLED_IIC_SCL_1() GPIO_SetBits(OLED_GPIO_PORT_IIC,OLED_IIC_SCL_PIN)
#define OLED_IIC_SCL_0() GPIO_ResetBits(OLED_GPIO_PORT_IIC,OLED_IIC_SCL_PIN)

#define OLED_IIC_SDA_1() GPIO_SetBits(OLED_GPIO_PORT_IIC,OLED_IIC_SDA_PIN)		
#define OLED_IIC_SDA_0() GPIO_ResetBits(OLED_GPIO_PORT_IIC,OLED_IIC_SDA_PIN)		


//imu_founctions
void imu_iic_Start(void);
void imu_iic_Stop(void);
void imu_iic_WriteByte(uint8_t _ucByte);
uint8_t imu_iic_ReadByte(uint8_t ack);
uint8_t imu_iic_WaitAck(void);
void imu_iic_Ack(void);
void imu_iic_NAck(void);
void imu_iic_gpio_init(void);

uint8_t imu_iic_init(void);


#endif







