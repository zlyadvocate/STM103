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


//--------���ڷ��͵ĳ���״̬�궨��--------
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
	u8 global_Statue = NORMAL; //ȫ��״̬����
	u8 current_Statue = NORMAL; //�洢������ǰ����״̬����
	static u8 previous_Statue = NORMAL;//�洢����ǰһ������״̬����
	u8 power_Statue = NORMAL;//�洢���״̬����
	u8 driving_statue = NORMAL;//�洢������ʻ״̬����
	u8 t = 0;
	
	u8 IC_HEAD[256] = {"JSDIC"};
	u8 CAN_HEAD[30] = {"JSDCAN"};
	//---------����ģ���ʼ��-----------
	
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
	
	TIM_INIT(TIM2,1999,3599);		//��ʱ100ms ����2 ����ͨѶ
//	TIM_INIT(TIM7,19999,35999);		//��ʱ10S�����10s��û�н��յ��������ݣ���ʼ������ģ�顣
//	TIM_Cmd(TIM7, ENABLE);
	TIM_INIT(TIM5,3999,3599);       //��ʱ300ms ����3 ��ͨѶ
	TIM_INIT(TIM3,19999,35999);		//��ʱ10S	����ģʽ��ʱ
	TIM_INIT(TIM4,999,3599);        //50ms
	TIM_INIT(TIM6,19999,35999);		//��ʱ10S���ж�90�Σ���ʱ15����
	
	Ad_Init();
	My_CAN_Init();
	BD_Init();
	
	Build_CRCTable(cnCRC32);
		
	//--------��ʼ�����ٶȴ�����---------------
	while(ADXL345_Init() && t<200) 
	{
		t++;
	}
	t = 0;
		
	//--------�����Զ�У׼���ٶȴ�����---------
	ADXL345_AUTO_Adjust();
	
	while(1)
	{	
		IAP_PROC(USART1);

		JUDGE_OUT_ACC();
		
		power_Statue = JUDGE_BatteryStatue(); //��ȡ���״̬
		
		driving_statue = JUDGE_DriningStatue();//��ȡ������ʻ״̬
		
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












