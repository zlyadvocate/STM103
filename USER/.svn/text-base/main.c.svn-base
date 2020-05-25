#include "adxl345.h"
#include "AD.h"
#include "clock_init.h"
#include "delay.h"
#include "myiic.h"
#include "usart.h"
#include "CAN.h"
#include "sys.h"
#include "timer.h"
#include <stdio.h>
#include <string.h>
#include <stm32f10x_usart.h>


//--------串口发送的车辆状态宏定义--------
#define NORMAL               0
#define CRASH                1
#define TURNOVER             3
#define VOLTAGE_LOW          5
#define POWER_OFF            9
#define CRASH_VOLTAGE_LOW    6
#define TURNOVER_VOLTAGE_LOW 8
#define CRASH_POWER_OFF      10
#define TURNOVER_POWER_OFF   12

int main()
{	
	u8 global_Statue = NORMAL; //全局状态变量
	u8 current_Statue = NORMAL; //存储车辆当前整体状态变量
	static u8 previous_Statue = NORMAL;//存储车辆前一次整体状态变量
	u8 power_Statue = NORMAL;//存储电池状态变量
	u8 driving_statue = NORMAL;//存储车辆驾驶状态变量
	u8 t = 0;
	
	u8 IC_HEAD[256] = {"JSDIC"};
	u8 CAN_HEAD[30] = {"JSDCAN"};
	//---------各个模块初始化-----------
	
	RCC_Configuration();
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
	NVIC_INIT();
	usart_init(USART_1,9600);
	usart_init(USART_2,9600); 
	usart_init(USART_3,9600); 	        
    delay_init(72); 
	USART_It_Init(USART_1, USART_IT_RXNE);
	USART_It_Init(USART_2, USART_IT_RXNE);
	USART_It_Init(USART_3, USART_IT_RXNE);
	
	TIM_INIT(TIM2,1999,3599);		//定时100ms 串口2 北斗通讯
//	TIM_INIT(TIM7,19999,35999);		//定时10S，如果10s内没有接收到北斗数据，初始化北斗模块。
//	TIM_Cmd(TIM7, ENABLE);
	TIM_INIT(TIM5,3999,3599);       //定时300ms 串口3 打卡通讯
	TIM_INIT(TIM3,19999,35999);		//定时10S	特殊模式计时
	TIM_INIT(TIM4,999,3599);        //50ms
	TIM_INIT(TIM6,19999,35999);		//定时10S，中断90次，计时15分钟
	
	Ad_Init();
	My_CAN_Init();
	BD_Init();
	
	Build_CRCTable(cnCRC32);
		
	//--------初始化加速度传感器---------------
	while(ADXL345_Init() && t<200) 
	{
		t++;
	}
	t = 0;
		
	//--------开机自动校准加速度传感器---------
	ADXL345_AUTO_Adjust();
	
	while(1)
	{	
		IAP_PROC(USART1);

		JUDGE_OUT_ACC();
		
		power_Statue = JUDGE_BatteryStatue(); //获取电池状态
		
		driving_statue = JUDGE_DriningStatue();//获取车辆驾驶状态
		
		current_Statue = power_Statue + driving_statue;
		
		global_Statue = current_Statue + BD_Rec_Statue + IC_Card_Statue + CAN_Statue;
		
		if(global_Statue != NORMAL)	
		{
			if(current_Statue != NORMAL) 
			{	 
				if(current_Statue != previous_Statue)
				{
					switch(current_Statue)
					{
						case 1:  printf("\r$JSDCAR,1*7F\r\n");break;
						case 3:  printf("\r$JSDCAR,2*7F\r\n");break;
						case 5:  printf("\r$JSDPOW,3*7F\r\n");break;
						case 9:  printf("\r$JSDPOW,4*7F\r\n");break;
						case 6:  printf("\r$JSDCAR,1*7F\r\n");printf("$JSDPOW,3*7F\r\n");break;
						case 8:  printf("\r$JSDCAR,2*7F\r\n");printf("$JSDPOW,3*7F\r\n");break;
						case 10: printf("\r$JSDCAR,1*7F\r\n");printf("$JSDPOW,4*7F\r\n");break;
						case 12: printf("\r$JSDCAR,2*7F\r\n");printf("$JSDPOW,4*7F\r\n");break;
						default: break;
					}
				}
				else
				{	
					t++;
					if(t>=30)
					{
						switch(current_Statue)
						{
							case 1:  printf("\r$JSDCAR,1*7F\r\n");break;
							case 3:  printf("\r$JSDCAR,2*7F\r\n");break;
							case 5:  printf("\r$JSDPOW,3*7F\r\n");break;
							case 9:  printf("\r$JSDPOW,4*7F\r\n");break;
							case 6:  printf("\r$JSDCAR,1*7F\r\n");printf("$JSDPOW,3*7F\r\n");break;
							case 8:  printf("\r$JSDCAR,2*7F\r\n");printf("$JSDPOW,3*7F\r\n");break;
							case 10: printf("\r$JSDCAR,1*7F\r\n");printf("$JSDPOW,4*7F\r\n");break;
							case 12: printf("\r$JSDCAR,2*7F\r\n");printf("$JSDPOW,4*7F\r\n");break;
							default: break;
						}
						t = 0;
					}
				}
				previous_Statue = current_Statue;
			}
			
			if(BD_Rec_Statue != BD_REC_NCOM)
			{
				printf("%s",USART2_RX_BUF);
				BD_Rec_Statue = BD_REC_NCOM;
//				TIM7->CNT = 0;
			}
			
			if(IC_Card_Statue != IC_CARD_REC_NCOM)
			{
				if(USART3_RX_BUF[7] == 0x40)
				{	
					memcpy(IC_HEAD+5, (u8*)(USART3_RX_BUF+7),66);
					UART_SENDSTR(USART1, IC_HEAD, 71|1<<15);
				}
				USART3_RX_STA = 0;
				IC_Card_Statue = IC_CARD_REC_NCOM;                                                    
			}
			
			if(CAN_Statue != CAN_EMP)
			{
				memcpy(CAN_HEAD+6, store_Message.Data, store_Message.DLC);
				UART_SENDSTR(USART1,(u8*)CAN_HEAD, (store_Message.DLC+6)|1<<15);
				CAN_Statue = CAN_EMP;
			}
		}
	}
}












