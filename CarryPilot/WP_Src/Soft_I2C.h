#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

void I2C_GPIO_Config(void);
void Single_WriteI2C0(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);
unsigned char Single_ReadI2C0(unsigned char SlaveAddress,unsigned char REG_Address);


#endif

