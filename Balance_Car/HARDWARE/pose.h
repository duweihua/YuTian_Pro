#ifndef __POSE_H__
#define __POSE_H__


float invSqrt(float x);

void complement_filter(float gx,float gy,float gz,float ax,float ay,float az);
	
void MahonyAHRSupdateIMU(float gx,float gy,float gz,float ax,float ay,float az);
void MahonyAHRSupdate(float gx,float gy,float gz,float ax,float ay,float az,float mx,float my,float mz);

void confuse(float gx,float gy,float gz);

#endif
