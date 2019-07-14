#ifndef __CONTROL_H
#define __CONTROL_H

typedef struct
{
	unsigned short x;
	unsigned short y;
	unsigned char Key_F;
	unsigned char Key_B;
	unsigned char Key_L;
	unsigned char Key_R;
	unsigned char Key_SW;
	unsigned char Check1;
	unsigned char Check2;
	
	unsigned char Key_F_last;
	unsigned char Key_B_last;
	
}RC_t;


void Tim1_configration(void);
void GetData_Encoder(void);
void test_mode(void);


#endif


