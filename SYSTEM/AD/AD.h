#ifndef __AD_H_
#define __AD_H_

#include <stm32f10x_type.h>
#include <stm32f10x_map.h>

void Ad_Init(void);
u8 JUDGE_BatteryStatue(void);
void JUDGE_OUT_ACC(void);
u16 GET_Adc(ADC_TypeDef* ADCx, u8 ADC_Channel);

#endif
