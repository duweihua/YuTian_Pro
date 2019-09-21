#include "ANO_GCS_DT.h"
#include "HARDWARE_uart.h"

//���ݲ�ֺ궨�壬�ڷ��ʹ���1�ֽڵ���������ʱ������int16��float�ȣ���Ҫ�����ݲ�ֳɵ����ֽڽ��з���
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)		) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )

#define MYHWADDR	0x05
#define SWJADDR		0xAF

#define PARNUM		100
//s32 ParValList[100];		//�����б�

//dt_flag_t f;					//��Ҫ�������ݵı�־
u8 data_to_send[50];	//�������ݻ���
u8 checkdata_to_send,checksum_to_send;

//=====��������=====
void ANO_DT_Data_Receive_Anl(u8 *data_buf,u8 num)
{
	u8 sum = 0;
	for(u8 i=0;i<(num-1);i++)
		sum += *(data_buf+i);
	if(!(sum==*(data_buf+num-1)))		return;		//�ж�sum
	if(!(*(data_buf)==0xAA && *(data_buf+1)==0xAF))		return;		//�ж�֡ͷ
	
	if(*(data_buf+2)==MYHWADDR)
	{
		if(*(data_buf+3)==0XE0)			//����E0
		{
			switch(*(data_buf+5))		//FUN
			{
				case 0x01:
//					if(*(data_buf+6)==0x00 && *(data_buf+7)==0x01)
//						sensor.acc_CALIBRATE = 1;
//					if(*(data_buf+6)==0x00 && *(data_buf+7)==0x02)
//						sensor.gyr_CALIBRATE = 1;
//					if(*(data_buf+6)==0x00 && *(data_buf+7)==0x04)
//						mag.mag_CALIBRATE = 1;
//					if(*(data_buf+6)==0x00 && *(data_buf+7)==0xB0)//��ȡ�汾��Ϣ
//						f.send_version = 1;
					break;
				case 0x02:
					if(*(data_buf+6)==0x00 && *(data_buf+7)==0xAA)	//�ָ�Ĭ��PID
					{
//						PID_Rest();
//						All_PID_Init();
//						data_save();
					}
					if(*(data_buf+6)==0x00 && *(data_buf+7)==0xAB)	//�ָ�Ĭ�ϲ���
					{
//						Parame_Reset();
//						data_save();
					}
					if(*(data_buf+6)==0x00 && *(data_buf+7)==0xAF)	//�ָ����в���
					{
//						PID_Rest();
//						All_PID_Init();
//						Parame_Reset();
//						data_save();
					}
					break;
				case 0xE1:
//					f.paraToSend = (u16)(*(data_buf+6)<<8)|*(data_buf+7);	//��ȡ����
					break;
				case 0x10:
					Program_Ctrl_DataAnl(data_buf+5);
					break;
				case 0x11:
					break;
				default:
					break;
			}
			ANO_DT_SendCmd(SWJADDR,*(data_buf+5),(u16)(*(data_buf+6)<<8)|*(data_buf+7),(u16)(*(data_buf+8)<<8)|*(data_buf+9),(u16)(*(data_buf+10)<<8)|*(data_buf+11),(u16)(*(data_buf+12)<<8)|*(data_buf+13),(u16)(*(data_buf+14)<<8)|*(data_buf+15));
		}
		else if(*(data_buf+3)==0XE1)	//���ò���
		{
//			u16 _paraNum = (u16)(*(data_buf+5)<<8)|*(data_buf+6);
//			s32 _paraVal = (s32)(((*(data_buf+7))<<24) + ((*(data_buf+8))<<16) + ((*(data_buf+9))<<8) + (*(data_buf+10)));
//			ANO_DT_GetParame(_paraNum,_paraVal);
		}
	}
}

static void ANO_DT_SendCmd(u8 dest, u8 fun, u16 cmd1, u16 cmd2, u16 cmd3, u16 cmd4, u16 cmd5)
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=MYHWADDR;
	data_to_send[_cnt++]=dest;
	data_to_send[_cnt++]=0xE0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=fun;
	data_to_send[_cnt++]=BYTE1(cmd1);
	data_to_send[_cnt++]=BYTE0(cmd1);
	data_to_send[_cnt++]=BYTE1(cmd2);
	data_to_send[_cnt++]=BYTE0(cmd2);
	data_to_send[_cnt++]=BYTE1(cmd3);
	data_to_send[_cnt++]=BYTE0(cmd3);
	data_to_send[_cnt++]=BYTE1(cmd4);
	data_to_send[_cnt++]=BYTE0(cmd4);
	data_to_send[_cnt++]=BYTE1(cmd5);
	data_to_send[_cnt++]=BYTE0(cmd5);
	
	data_to_send[4] = _cnt-5;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;

	ANO_DT_Send_Data(data_to_send, _cnt);
}

/////////////////////////////////////////////////////////////////////////////////////
//=====��������=====
//Data_Receive_Prepare������Э��Ԥ����������Э��ĸ�ʽ�����յ������ݽ���һ�θ�ʽ�Խ�������ʽ��ȷ�Ļ��ٽ������ݽ���
//��ֲʱ���˺���Ӧ���û���������ʹ�õ�ͨ�ŷ�ʽ���е��ã����紮��ÿ�յ�һ�ֽ����ݣ�����ô˺���һ��
//�˺������������ϸ�ʽ������֡�󣬻����е������ݽ�������
static u8 DT_RxBuffer[256],DT_data_cnt = 0,ano_dt_data_ok;

//��ѯһ�������Ƿ������ɣ�Ȼ�������
void ANO_DT_Data_Receive_Anl_Task()
{
	if(ano_dt_data_ok)
	{
		ANO_DT_Data_Receive_Anl(DT_RxBuffer,DT_data_cnt+6);
		ano_dt_data_ok = 0;
	}
}

//BYTE���ݽ��գ��ɷ��봮�ڽ����ж�
void ANO_DT_Data_Receive_Prepare(u8 data)
{
	static u8 _data_len = 0;
	static u8 state = 0;
	
	if(state==0&&data==0xAA)	//֡ͷ0xAA
	{
		state=1;
		DT_RxBuffer[0]=data;
	}
	else if(state==1&&data==0xAF)	//����Դ��0xAF��ʾ����������λ��
	{
		state=2;
		DT_RxBuffer[1]=data;
	}
	else if(state==2)		//����Ŀ�ĵ�
	{
		state=3;
		DT_RxBuffer[2]=data;
	}
	else if(state==3)		//������
	{
		state=4;
		DT_RxBuffer[3]=data;
	}
	else if(state==4)		//���ݳ���
	{
		state = 5;
		DT_RxBuffer[4]=data;
		_data_len = data;
		DT_data_cnt = 0;
	}
	else if(state==5&&_data_len>0)
	{
		_data_len--;
		DT_RxBuffer[5+DT_data_cnt++]=data;
		if(_data_len==0)
			state = 6;
	}
	else if(state==6)
	{
		state = 0;
		DT_RxBuffer[5+DT_data_cnt]=data;
		ano_dt_data_ok = 1;//ANO_DT_Data_Receive_Anl(DT_RxBuffer,DT_data_cnt+5);
	}
	else
		state = 0;
}
/////////////////////////////////////////////////////////////////////////////////////
//=====��������=====
//Send_Data������Э�������з������ݹ���ʹ�õ��ķ��ͺ���
//��ֲʱ���û�Ӧ��������Ӧ�õ����������ʹ�õ�ͨ�ŷ�ʽ��ʵ�ִ˺���
static void ANO_DT_Send_Data(u8 *dataToSend , u8 length)
{
	//usb����
#ifdef ANO_DT_USE_USB_HID
	Usb_Hid_Adddata(data_to_send,length);
#endif
	//���ڷ���
#ifdef ANO_DT_USE_USART2
    USART_TX(&UsartGroup[UART_A3], data_to_send, length);
	//Usart2_Send(data_to_send, length);
#endif
	
}
