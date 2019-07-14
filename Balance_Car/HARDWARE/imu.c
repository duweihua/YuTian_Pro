#include "main.h"

#define _DEG_TO_RAD    	0.0174532f 		

float _accel_scale = 0.00059814453f;//9.8/16384
float _gyro_scale = 0.0005326456496f;//1/32.767*0.0174532

extern float gyro[3];
extern float acc[3];
extern float temperture;

extern ANGLE_T angle;

extern float pitch,yaw,roll;

extern float dt;


//д���ݵ�MPU6050�Ĵ���
void MPU6050_WriteReg(u8 reg_add,u8 reg_dat)
{
	imu_iic_Start();
	imu_iic_WriteByte(MPU6050_SLAVE_ADDRESS);
	imu_iic_WaitAck();
	imu_iic_WriteByte(reg_add);
	imu_iic_WaitAck();
	imu_iic_WriteByte(reg_dat);
	imu_iic_WaitAck();
	imu_iic_Stop();
}



//��MPU6050�Ĵ�����ȡ����
void MPU6050_ReadData(u8 reg_add,unsigned char*Read,u8 num)
{
	unsigned char i;
	
	imu_iic_Start();
	imu_iic_WriteByte(MPU6050_SLAVE_ADDRESS);
	imu_iic_WaitAck();
	imu_iic_WriteByte(reg_add);
	imu_iic_WaitAck();

	imu_iic_Start();
	imu_iic_WriteByte(MPU6050_SLAVE_ADDRESS+1);
	imu_iic_WaitAck();
	
	for(i=0;i<(num-1);i++)
	{
		*Read = imu_iic_ReadByte(1);
		Read++;
	}
	*Read = imu_iic_ReadByte(0);
	imu_iic_Stop();
}


//��ʼ��MPU6050оƬ
void imu_init(void)
{
	int i=0,j=0;
	
	//�ڳ�ʼ��֮ǰҪ��ʱһ��ʱ�� 
	//��û����ʱ �ϵ�����ϵ����ݿ��ܻ����
	for(i=0;i<1000;i++)
		for(j=0;j<1000;j++);
	
	 //�������״̬
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1,0x00);	   
	//�����ǲ����� 1KHz
	MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV,0x07);	
	//��ͨ�˲��������� ��ֹƵ����1K ������5K
	MPU6050_WriteReg(MPU6050_RA_CONFIG,0x06);	  
	//ʹ���ж���� 5ms����(����Ϊϵͳ�ж�Դ)
	//����ʧ����
	MPU6050_WriteReg(MPU6050_RA_INT_PIN_CFG,0X90);
	MPU6050_WriteReg(MPU6050_RA_INT_ENABLE,0x00);
	//�Ӽ�2Gģʽ ���Լ�
	MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG,0x00);
	//������1000deg/s ���Լ�
	MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG,0x10);     
}


//��ȡMPU6050��ID
uint8_t MPU6050ReadID(void)
{
	unsigned char Re = 0;
	
    MPU6050_ReadData(MPU6050_RA_WHO_AM_I,&Re,1);    //��������ַ
	
	if(Re != 0x68)
	{
		printf("MPU6050 dectected error!\r\n��ⲻ��MPU6050ģ�飬����ģ���뿪����Ľ���");
		return 0;
	}
	else
	{
		printf("MPU6050 ID = %d\r\n",Re);
		return 1;
	}
}


//��ȡMPU6050�ļ��ٶ�����
void MPU6050ReadAcc(float *accData)
{
    u8 buf[6];
	short acc_adc[3];
	
    MPU6050_ReadData(MPU6050_ACC_OUT,buf,6);
    
	acc_adc[0] = (buf[0] << 8) | buf[1];
    acc_adc[1] = (buf[2] << 8) | buf[3];
    acc_adc[2] = (buf[4] << 8) | buf[5];
	
	accData[0] = acc_adc[0] * _accel_scale;
	accData[1] = acc_adc[1] * _accel_scale;
	accData[2] = acc_adc[2] * _accel_scale;
}


//��ȡMPU6050�ĽǼ��ٶ�����
void MPU6050ReadGyro(float *gyroData)
{
    u8 buf[6];
	short gyro_adc[3];
	
    MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6);
    
	gyro_adc[0] = (buf[0] << 8) | buf[1];
    gyro_adc[1] = (buf[2] << 8) | buf[3];
    gyro_adc[2] = (buf[4] << 8) | buf[5];
	
	gyroData[0] = gyro_adc[0] * _gyro_scale;
	gyroData[1] = gyro_adc[1] * _gyro_scale;
	gyroData[2] = gyro_adc[2] * _gyro_scale;	
}



//��ȡMPU6050��ԭʼ�¶�����
void MPU6050ReadTemp(short *tempData)
{
	u8 buf[2];
	
    MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);     //��ȡ�¶�ֵ
    
	*tempData = (buf[0] << 8) | buf[1];
}



//��ȡMPU6050���¶����� ת�������϶�
void MPU6050_ReturnTemp(float *Temperature)
{
	short temp3;
	u8 buf[2];
	
	MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);     //��ȡ�¶�ֵ
 
	temp3= (buf[0] << 8) | buf[1];	
	
	*Temperature=((double)temp3/340.0)+36.53;
}


void imu_read(void)
{
	if(MPU6050ReadID() == 1)
	{
		MPU6050ReadAcc(acc);

		MPU6050ReadGyro(gyro);
		
//		angle.Pitch += (57.3f*gyro[0])*dt;
//		
//		angle.Roll += (57.3f*gyro[1])*dt;

//		angle.Yaw += (57.3f*gyro[2])*dt;		
//		
//		pitch = angle.Pitch;
//		
//		yaw = angle.Yaw;
//		
//		roll = angle.Roll;
		
		complement_filter(gyro[0],gyro[1],gyro[2],acc[0],acc[1],acc[2]);
		
//		MahonyAHRSupdateIMU(gyro[0],gyro[1],gyro[2],acc[0],acc[1],acc[2]);
	}
}



