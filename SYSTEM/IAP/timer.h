#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//通用定时器 驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/12/03
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
extern u8 TIM3_STA;
extern u8 OUT_ACC_Count; 
extern u8 GPIOB_12_STATUE;
//extern u8 BD_Count;

void TIM_INIT(TIM_TypeDef* TIMx, u16 arr,u16 psc);
void itoa(u32 num, u8 *p , u8 ch);
void Timerx_Init(u16 arr,u16 psc); 
void NVIC_INIT(void);
//void My_EXTI_Init(void);
void My_Sleep(u8* count);
 
#endif
