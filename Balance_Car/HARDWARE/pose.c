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


//姿态解算：
//			一阶互补滤波——PITCH轴调好
//			AHRS解算算法——未调好
//			DMP 数据读取——未尝试
//			卡尔曼滤波法——未尝试


//一阶互补滤波
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


//快速开方函数
//return：平方的倒数
float invSqrt(float x)
{
	float xhalf = 0.5 * x;
	int i = *(int*)&x;
	i = 0x5f3759df - (i >> 1); 
	x = *(float*)&i;
	x = x * (1.5 - xhalf * x * x); 
	return x;
}


//mahony姿态融合算法(6轴)
/*
  对于一个确定的向量n
  用不同的坐标系表示时 他们所表示的大小和方向一定是相同的
  但是由于这两个坐标系的转换矩阵存在误差 
  那么当一个向量经过这么一个有误差存在的旋转矩阵变换后
  在另一个坐标系中肯定和理论值是有偏差的
  我们通过这个偏差来修正这个旋转矩阵
 
  这个旋转矩阵的元素是四元数
  这就是说我们修正的就是四元数
  于是乎我们的姿态就这样被修正了

  我们的姿态解算求的是四元数
  我们是通过修正旋转矩阵中的四元数来达到姿态解算的目的
  而不要以为通过加速度计和地磁计来修正姿态
  加速度计和地磁计只是测量工具和载体
  通过这两个器件表征旋转矩阵的误差存在
  然后通过算法修正误差 修正四元数修正姿态
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

	//加计各轴数据非0(不同时为0)
	//如果加速度计处于自由落体状态可能会出现这种情况
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
	{
		//归一化
		//意义在于在变更了加速度计的量程之后不需要修改Kp参数 因为这里归一化了
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		//根据当前四元数的姿态值来估算出各重力加速度分量
		//四元数 --> 旋转矩阵(方向余弦矩阵)
		//根据旋转矩阵和欧拉角的定义
		//地理坐标系的重力向量转到机体坐标系
		//正好是旋转矩阵第三行的三个元素
		//旋转矩阵的第三行和原坐标相乘得到转换后坐标的z轴分量
		//相当于是一个坐标系中向量向另一个坐标系的z轴投影
		//所以3个系数就是地球坐标系中z轴的加速度(重力加速度)向机体坐标系各轴的投影分量
		//注意 原代码中取了其中的1/2 此处没有取
		halfvx = 2.0f*(q1 * q3 - q0 * q2);
		halfvy = 2.0f*(q0 * q1 + q2 * q3);
		halfvz = 2.0f*(q0 * q0 - 0.5f + q3 * q3);

		//叉积来计算估算的重力向量和实际测量的重力向量之间的误差
		halfex = (ay * halfvz - az * halfvy);
		halfey = (az * halfvx - ax * halfvz);
		halfez = (ax * halfvy - ay * halfvx);

		//添加积分项修正
		//加计与陀螺仪偏差的积分
		if((imuKi > 0.0f)||(imuKi < 0.0f))
		{
			integraIMUx += imuKi * halfex * dt; 
			integraIMUy += imuKi * halfey * dt;
			integraIMUz += imuKi * halfez * dt;

			gx += integraIMUx;
			gy += integraIMUy;
			gz += integraIMUz;
		}
		//不加积分项
		else 
		{
			integraIMUx = 0.0f;
			integraIMUy = 0.0f;
			integraIMUz = 0.0f;
		}

		//添加比例项修正
		gx += imuKp * halfex;
		gy += imuKp * halfey;
		gz += imuKp * halfez;
	}

	//修正后的数据整合为四元数形式
	//传入的陀螺仪数据为角速度
	//直接将PI修正叠加到角速度数据上
	//积分时即同时将完成了数据修正
	//以下为四元数微分方程 
	//将陀螺仪和四元数结合起来是姿态更新的核心算子	
	//计算方法由矩阵运算推导而来	
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
	//解出来的是dq(微分) 需要积分
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
	
	//归一化	
	//意义在于单位化四元数在空间旋转时是不会拉伸的
	//仅有旋转角度 这类似与线性代数里面的正交变换
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
	
	//yaw轴解算有问题 故舍弃融合直接积分
//	angle.Yaw 	= atan2(2*q1*q2 + 2*q0*q3, - 2*q2*q2 - 2*q3*q3 + 1) * 57.3;
	angle.Roll  = asin(-2*q1*q3 + 2*q0*q2) * 57.3; 
	angle.Pitch = atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1)* 57.3;
	
//	yaw   = angle.Yaw;	
	pitch = angle.Pitch;
	roll  = angle.Roll;
}







