#include "main.h"

u16 NUM = 4000;

u16 ADC12_Buffer[4000]={0};

float CHECK_Voltage = 1.65f;

float CHECK_Value;

float CurrentOffset = 14.8f;

extern float nowcurrent1;
extern float nowcurrent2;

float test_voltage;

//平滑因子计算公式
//K_filter = dt /(1/(2*Π*f) + dt)
//dt = 0.05 (s)
//f  = ？ (Hz)
//公式不好使
float Kcurrent_filter = 0.15f;


//0.02Ω采样电阻
void ADC_Configuration(void)
{
	GPIO_InitTypeDef 	gpio;
    ADC_InitTypeDef   	adc;
	DMA_InitTypeDef   	dma;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);	

//    gpio.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
//    gpio.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_4;
//    gpio.GPIO_Pin = GPIO_Pin_4;
	gpio.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
    gpio.GPIO_Mode = GPIO_Mode_AIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&gpio);
	
	DMA_DeInit(DMA1_Channel1);
	dma.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));
	dma.DMA_MemoryBaseAddr =(uint32_t)ADC12_Buffer;
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma.DMA_BufferSize = NUM;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_High;
	dma.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1,&dma);
	
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	//ADC1
	ADC_DeInit(ADC1);
    adc.ADC_Mode = ADC_Mode_Independent;
    adc.ADC_ScanConvMode = ENABLE;
    adc.ADC_ContinuousConvMode = ENABLE;
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  
    adc.ADC_DataAlign = ADC_DataAlign_Right;
    adc.ADC_NbrOfChannel = 2;
    ADC_Init(ADC1,&adc);
    
	//8分频 ADC时钟9MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	//ADC1
//    ADC_RegularChannelConfig(ADC1,ADC_Channel_2,1,ADC_SampleTime_28Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_3,1,ADC_SampleTime_28Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_4,2,ADC_SampleTime_28Cycles5);
	
	ADC_DMACmd(ADC1,ENABLE);
//	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
    ADC_Cmd(ADC1,ENABLE);
  	ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);	
}


void ADC_read(void)
{
	int adc_i = 0;
	
	float M1_ADC_Value = 0;
	float M2_ADC_Value = 0;

	float current1_temp;
	float current2_temp;
	
	float lastcurrent1;	
	float lastcurrent2;
	
	lastcurrent1 = nowcurrent1;
	lastcurrent2 = nowcurrent2;

	for(adc_i=0;adc_i<(NUM/2);adc_i++)
	{
		M1_ADC_Value += ADC12_Buffer[2*adc_i];
		
		M2_ADC_Value += ADC12_Buffer[2*adc_i+1];
	}
	
	M1_ADC_Value = M1_ADC_Value / (NUM/2);
	M2_ADC_Value = M2_ADC_Value / (NUM/2);
	
	//解算为电压 减去基准电压
	current1_temp = (M1_ADC_Value/4096.0f*3.3f - CHECK_Voltage);	
	current2_temp = (M2_ADC_Value/4096.0f*3.3f - CHECK_Voltage);

	//解算为电流(单位：毫安)
	nowcurrent1 = current1_temp*1000.0f;// /50.0f*50.0f
	nowcurrent2 = current2_temp*1000.0f;// /50.0f*50.0f
	
//	nowcurrent1 -= (CurrentOffset);	
	nowcurrent2 -= (CurrentOffset);
	
	nowcurrent1 = nowcurrent1 * Kcurrent_filter + lastcurrent1 * (1.0f - Kcurrent_filter);	
	nowcurrent2 = nowcurrent2 * Kcurrent_filter + lastcurrent2 * (1.0f - Kcurrent_filter);
	
	if((nowcurrent1>-10.0f)&&(nowcurrent1<10.0f))
		nowcurrent1 = 0.0f;
	if((nowcurrent2>-15.0f)&&(nowcurrent2<15.0f))
		nowcurrent2 = 0.0f;
}


void Get_Adc_Check(void)
{
//    ADC_Cmd(ADC1,DISABLE);

	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,1,ADC_SampleTime_28Cycles5);

    ADC_Cmd(ADC1,ENABLE);
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);			
	 
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));	
	//返回最近一次ADC转换结果
	CHECK_Value = ADC_GetConversionValue(ADC1);	

	CHECK_Voltage = CHECK_Value/4096.0f*3.3f;
	
//	ADC_Cmd(ADC1,DISABLE);
	
//	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,1,ADC_SampleTime_28Cycles5);

//  ADC_Cmd(ADC1,ENABLE);
}







