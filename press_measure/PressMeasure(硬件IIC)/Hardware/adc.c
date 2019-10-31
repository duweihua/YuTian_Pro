#include "main.h"

u16 NUM = 4000;

u16 ADC12_Buffer[4000]={0};

float CHECK_Voltage;
float CHECK_Value;

float CurrentOffset = 14.8f;//-3.8f;

extern float nowcurrent1;
extern float nowcurrent2;

float test_voltage;

//平滑因子计算公式
//K_filter = dt /(1/(2*Π*f) + dt)
//dt = 0.05 (s)
//f  = 10 (Hz)
float Kcurrent_filter = 0.2f;//0.6873;

//0.02Ω采样电阻
//ADC采样电压计算：ADC_Value/4096*3.3
void ADC_Configuration(void)
{
	GPIO_InitTypeDef 	gpio;
    ADC_InitTypeDef   	adc;
//	NVIC_InitTypeDef 	nvic;
	DMA_InitTypeDef   	dma;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);	

//    gpio.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
//    gpio.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_4;
    gpio.GPIO_Pin = GPIO_Pin_4;
    gpio.GPIO_Mode = GPIO_Mode_AIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&gpio);
	
//	nvic.NVIC_IRQChannel = ADC1_2_IRQn;  
//	nvic.NVIC_IRQChannelPreemptionPriority = 1;  
//	nvic.NVIC_IRQChannelSubPriority = 1;  
//	nvic.NVIC_IRQChannelCmd = ENABLE; 
//	NVIC_Init(&nvic);
	    
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
    adc.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1,&adc);
    
	//8分频 ADC时钟9MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	//ADC1
//    ADC_RegularChannelConfig(ADC1,ADC_Channel_2,1,ADC_SampleTime_28Cycles5);
//    ADC_RegularChannelConfig(ADC1,ADC_Channel_3,2,ADC_SampleTime_28Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_4,1,ADC_SampleTime_28Cycles5);
	
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
//	ADC_Check = ((float)ADC12_Buffer[0]/4096*3.3);
//	nowcurrent1 = ((float)ADC12_Buffer[1]/4096*3.3*20.0f);
    
	int adc_i = 0;
	
	float ADC_Value = 0;
	
//	float CHECK_Value = 0;
	
//	float CHECK_Voltage = 0;
	
	float current2_temp;
	
	float lastcurrent2;
	
	lastcurrent2 = nowcurrent2;

	for(adc_i=0;adc_i<NUM;adc_i++)
	{
		ADC_Value += ADC12_Buffer[adc_i];
		
//		CHECK_Value += ADC12_Buffer[2*adc_i];
	}
	ADC_Value = ADC_Value / NUM;
//	CHECK_Value = CHECK_Value / 10;
	
//	CHECK_Voltage = CHECK_Value/4096.0f*3.3f;
	
//	test_voltage = ADC_Value/4096.0f*3.3f;
	
	//解算为电压 减去基准电压
	current2_temp = (ADC_Value/4096.0f*3.3f - CHECK_Voltage);
//	current2_temp = (-current2_temp);

//	if(current2_temp<0)
//		current2_temp = 0.0f;
	//解算为电流(单位：毫安)
//	nowcurrent2 = (current2_temp*0.2f*1000.0f);
	
	nowcurrent2 = current2_temp*1000.0f;// /50.0f*50.0f
	
	nowcurrent2 -= (CurrentOffset);
	
	nowcurrent2 = nowcurrent2 * Kcurrent_filter + lastcurrent2 * (1.0f - Kcurrent_filter);
	
	if((nowcurrent2>-8.0f)&&(nowcurrent2<8.0f))
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

//    ADC_Cmd(ADC1,ENABLE);
	
}


//void ADC_IRQHandler(void)
//{
//	if (ADC_GetITStatus(ADC1,ADC_IT_EOC)==SET)
//	{
//		ADC_Check = ADC_GetConversionValue(ADC1);
//		ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);	
//	}
//	if (ADC_GetITStatus(ADC2,ADC_IT_EOC)==SET)
//	{
//		nowcurrent1 = ADC_GetConversionValue(ADC2);
//		ADC_ClearITPendingBit(ADC2,ADC_IT_EOC);
//	}
//	if (ADC_GetITStatus(ADC3,ADC_IT_EOC)==SET)
//	{
//		nowcurrent2 = ADC_GetConversionValue(ADC3);
//		ADC_ClearITPendingBit(ADC3,ADC_IT_EOC);
//	}
//}



