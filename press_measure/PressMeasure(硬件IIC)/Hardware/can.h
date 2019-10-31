#ifndef __CAN_H__
#define __CAN_H__

#include "stm32f10x.h"

#define CAN_ID_CRASH 0x64

void CAN_Configuration(void);
void CANWriteData(unsigned char add,char *s);
void CANSendData(short data1, short data2);

#endif 
