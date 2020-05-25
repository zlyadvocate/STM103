#include "AD.h"
#include "delay.h"
#include "timer.h"
#include "can.h"
#include <stm32f10x_exti.h>
#include <stm32f10x_map.h>
#include <stm32f10x_conf.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_nvic.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_tim.h>

#define NORMAL               0
#define VOLTAGE_LOW          5
#define POWER_OFF            9
/*******************************************
函数名称：Ad_Init
功 能：初始化STM32的ADC模块：
参 数：无
返 回 值：无
********************************************/
void Ad_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//定义GPIO初始化结构体
	ADC_InitTypeDef ADC_InitStructure;//定义ADC初始化结构体
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE );//使能GPIOC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOB的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
// 改变指定管脚的映射 GPIO_Remap_SWJ_Disable SWJ 完全禁用（JTAG+SW-DP）
	
	//为GPIO初始化结构体赋值
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//选择0口，PC4为ADC1 的通道 14
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//-------配置选择开关的GPIOB.3为10MHz浮空输入    OUT_ACC1---------
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//-------配置选择开关的GPIOB.12为10MHz推挽输出   IN_ACC---------
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//使能ADC1时钟
	ADC_DeInit(ADC1);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC转换时钟12MHz
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC 工作模式:独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; //AD 单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //AD 单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC 数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1; //顺序进行规则转换的 ADC 通道的数目 1
	ADC_Init(ADC1, &ADC_InitStructure); //根据指定的参数初始化外设 ADC1	
	ADC_Cmd(ADC1, ENABLE); //使能指定的 ADC1
	ADC_ResetCalibration(ADC1);//执行复位校准
	while(ADC_GetResetCalibrationStatus(ADC1)); //等待复位校准结束
	ADC_StartCalibration(ADC1); //开启 AD 校准
	while(ADC_GetCalibrationStatus(ADC1)); //等待校准结束
	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); 
}

/*******************************************
函数名称：GET_Adc
功 能：获取ADC结果
参 数：ADCx：x 可以是 1 或者 2 来选择 ADC 外设 ADC1 或 ADC2；ADC_Channel：被设置的 ADC 通道
返 回 值：返回16位的ADC结果
********************************************/
u16 GET_Adc(ADC_TypeDef* ADCx, u8 ADC_Channel)
{
	u32 temp_val=0;
	u8 t;
	
	ADC_RegularChannelConfig(ADCx, ADC_Channel, 1, ADC_SampleTime_7Cycles5);//采样时间为7.5周期
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);//使能指定的ADC1的软件转换启动功能
	while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC ));//等待转换结束
	for(t=0;t<10;t++)
	{
		temp_val+=ADC_GetConversionValue(ADCx);
		delay_ms(5);
	}
	return temp_val/10;
	//return ADC_GetConversionValue(ADCx);//返回最近一次ADC1规则组的转换结果
}

/*******************************************
函数名称：JUDGE_BatteryStatue
功 能：判断电池电压的状态
参 数：无
返 回 值：电池电压低于10V返回3，电压低于6V返回4，正常返回0
********************************************/
u8 JUDGE_BatteryStatue()
{
	float ADC_Result;
	ADC_Result = (float)GET_Adc(ADC1, ADC_Channel_14)*(3.3/4096);
	if(ADC_Result<0.78)
	{
		if(ADC_Result<0.275)   
		{
			return POWER_OFF;
		}
		else
		{
			return VOLTAGE_LOW;
		}
	}
	else
	{
		return NORMAL;
	}										  
}

/*******************************************
函数名称：JUDGE_BatteryStatue
功 能：判断OUT_ACC状态,并根据其状态是否打开定时器6
参 数：无
返 回 值：无
********************************************/
void JUDGE_OUT_ACC(void)
{	
//	GPIO_InitTypeDef GPIO_InitStructure;//定义GPIO初始化结构体
	
	//-------配置选择开关的GPIOB.3为10MHz浮空输入    OUT_ACC1---------
//	if(GPIOB_3_STATUE == 1)
//	{
//		GPIOB_3_STATUE = 0;
//		
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//		GPIO_Init(GPIOB, &GPIO_InitStructure);
//		
//		CAN_ITConfig(CAN_IT_FMP0, ENABLE);
//		
//		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//		
//		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//		
//		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
//		
//		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
//		
//		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
//		
//		TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
//		
//		TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
//		
//		TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
//		
//		TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);	
//	} 
	if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 1) && (GPIOB_12_STATUE == 0)) //out_acc无，in_acc有
	{
		TIM_Cmd(TIM6, ENABLE);	 	//使能定时器6的中断 
		My_Sleep(&OUT_ACC_Count);
	}
	else
	{
		if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 1) && (GPIOB_12_STATUE == 1))//out_acc无，in_acc无
		{
			OUT_ACC_Count = 0;
			TIM_Cmd(TIM6, DISABLE);	 	//失能定时器6的中断	
		}
		else
		{
			if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0) && (GPIOB_12_STATUE == 1))//out_acc有，in_acc无
			{
				OUT_ACC_Count = 0;
				TIM_Cmd(TIM6, DISABLE);	 	//失能定时器6的中断
				My_Sleep(&OUT_ACC_Count);	
			}
			else   //out_acc有，in_acc有
			{
				OUT_ACC_Count = 0;
				TIM_Cmd(TIM6, DISABLE);	 	//失能定时器6的中断		
			}	
		}
	}
}






























