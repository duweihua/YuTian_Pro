#include "main.h"
#include "math.h"

extern float q0,q1,q2,q3;

extern float imuKi;
extern float imuKp;

extern ANGLE_T angle;

extern float pitch;
extern float yaw;
extern float roll;

extern float dt;

u8 init_flag = 0;

extern float K_pitch,K_roll;


//��̬���㣺
//			һ�׻����˲�����PITCH�����
//			AHRS�����㷨����δ����
//			DMP ���ݶ�ȡ����δ����
//			�������˲�������δ����


//һ�׻����˲�
void complement_filter(float gx,float gy,float gz,float ax,float ay,float az)
{
	float acc_pitch,acc_roll;
	float gyro_pitch,gyro_roll,gyro_yaw;
	
	acc_pitch = atan2(ay,az)*57.3f;
	acc_roll  = atan2(ax,az)*57.3f;

	gyro_pitch = (angle.Pitch + gx*57.3f*dt);
	gyro_roll = (angle.Roll + gy*57.3f*dt);
	gyro_yaw = (angle.Yaw + gz*57.3f*dt);
	
	if(init_flag == 0)
	{
		gyro_pitch = acc_pitch;
		gyro_roll = acc_roll;
//		gyro_yaw = acc_yaw;
		init_flag = 1;
	}

	angle.Pitch = K_pitch * gyro_pitch + (1-K_pitch) * acc_pitch;
	angle.Roll = K_roll * gyro_roll + (1-K_roll) * acc_roll;
	angle.Yaw = 1.0f * gyro_yaw;
	
	pitch = angle.Pitch;
	roll = angle.Roll;
	yaw = angle.Yaw;
}


//���ٿ�������
//return��ƽ���ĵ���
float invSqrt(float x)
{
	float xhalf = 0.5 * x;
	int i = *(int*)&x;
	i = 0x5f3759df - (i >> 1); 
	x = *(float*)&i;
	x = x * (1.5 - xhalf * x * x); 
	return x;
}


//mahony��̬�ں��㷨(6��)
/*
  ����һ��ȷ��������n
  �ò�ͬ������ϵ��ʾʱ ��������ʾ�Ĵ�С�ͷ���һ������ͬ��
  ������������������ϵ��ת������������ 
  ��ô��һ������������ôһ���������ڵ���ת����任��
  ����һ������ϵ�п϶�������ֵ����ƫ���
  ����ͨ�����ƫ�������������ת����
 
  �����ת�����Ԫ������Ԫ��
  �����˵���������ľ�����Ԫ��
  ���Ǻ����ǵ���̬��������������

  ���ǵ���̬�����������Ԫ��
  ������ͨ��������ת�����е���Ԫ�����ﵽ��̬�����Ŀ��
  ����Ҫ��Ϊͨ�����ٶȼƺ͵شż���������̬
  ���ٶȼƺ͵شż�ֻ�ǲ������ߺ�����
  ͨ������������������ת�����������
  Ȼ��ͨ���㷨������� ������Ԫ��������̬
*/
void MahonyAHRSupdateIMU(float gx,float gy,float gz,float ax,float ay,float az) 
{
	float recipNorm;
	
	float halfvx, halfvy, halfvz;
	float halfex, halfey, halfez;
	
	float qa,qb,qc;
	
	static float integraIMUx;
	static float integraIMUy;
	static float integraIMUz;

	//�ӼƸ������ݷ�0(��ͬʱΪ0)
	//������ٶȼƴ�����������״̬���ܻ�����������
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
	{
		//��һ��
		//���������ڱ���˼��ٶȼƵ�����֮����Ҫ�޸�Kp���� ��Ϊ�����һ����
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		//���ݵ�ǰ��Ԫ������ֵ̬����������������ٶȷ���
		//��Ԫ�� --> ��ת����(�������Ҿ���)
		//������ת�����ŷ���ǵĶ���
		//��������ϵ����������ת����������ϵ
		//��������ת��������е�����Ԫ��
		//��ת����ĵ����к�ԭ������˵õ�ת���������z�����
		//�൱����һ������ϵ����������һ������ϵ��z��ͶӰ
		//����3��ϵ�����ǵ�������ϵ��z��ļ��ٶ�(�������ٶ�)���������ϵ�����ͶӰ����
		//ע�� ԭ������ȡ�����е�1/2 �˴�û��ȡ
		halfvx = 2.0f*(q1 * q3 - q0 * q2);
		halfvy = 2.0f*(q0 * q1 + q2 * q3);
		halfvz = 2.0f*(q0 * q0 - 0.5f + q3 * q3);

		//�����������������������ʵ�ʲ�������������֮������
		halfex = (ay * halfvz - az * halfvy);
		halfey = (az * halfvx - ax * halfvz);
		halfez = (ax * halfvy - ay * halfvx);

		//��ӻ���������
		//�Ӽ���������ƫ��Ļ���
		if((imuKi > 0.0f)||(imuKi < 0.0f))
		{
			integraIMUx += imuKi * halfex * dt; 
			integraIMUy += imuKi * halfey * dt;
			integraIMUz += imuKi * halfez * dt;

			gx += integraIMUx;
			gy += integraIMUy;
			gz += integraIMUz;
		}
		//���ӻ�����
		else 
		{
			integraIMUx = 0.0f;
			integraIMUy = 0.0f;
			integraIMUz = 0.0f;
		}

		//��ӱ���������
		gx += imuKp * halfex;
		gy += imuKp * halfey;
		gz += imuKp * halfez;
	}

	//���������������Ϊ��Ԫ����ʽ
	//���������������Ϊ���ٶ�
	//ֱ�ӽ�PI�������ӵ����ٶ�������
	//����ʱ��ͬʱ���������������
	//����Ϊ��Ԫ��΢�ַ��� 
	//�������Ǻ���Ԫ�������������̬���µĺ�������	
	//���㷽���ɾ��������Ƶ�����	
	//	 .  1      	
	//	q = - * q x Omega    
	//	    2	
	//	  .	
	//	[q0] 	[ 0  -wx  -wy  -wz]	 [q0]	
	//	  .							
	//	[q1]	[wx	   0   wz  -wy]	 [q1]	1
	//	  .  =  			       * 	  * -
	//	[q2]	[wy  -wz    0   wx]	 [q2]	2
	//	  . 						
	//	[q3]	[wz   wy  -wx    0]	 [q3]
	//���������dq(΢��) ��Ҫ����
	gx *= (0.5f * dt);
	gy *= (0.5f * dt);
	gz *= (0.5f * dt);
	
	qa = q0;
	qb = q1;
	qc = q2;
	
	q0 += (-qb * gx - qc * gy - q3 * gz);
	q1 += ( qa * gx + qc * gz - q3 * gy);
	q2 += ( qa * gy - qb * gz + q3 * gx);
	q3 += ( qa * gz + qb * gy - qc * gx);	
	
	//��һ��	
	//�������ڵ�λ����Ԫ���ڿռ���תʱ�ǲ��������
	//������ת�Ƕ� �����������Դ�������������任
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
	
	//yaw����������� �������ں�ֱ�ӻ���
//	angle.Yaw 	= atan2(2*q1*q2 + 2*q0*q3, - 2*q2*q2 - 2*q3*q3 + 1) * 57.3;
	angle.Roll  = asin(-2*q1*q3 + 2*q0*q2) * 57.3; 
	angle.Pitch = atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1)* 57.3;
	
//	yaw   = angle.Yaw;	
	pitch = angle.Pitch;
	roll  = angle.Roll;
}







