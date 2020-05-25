#include "timer.h"
#include "sys.h"
#include "clock_init.h"
#include <stm32f10x_gpio.h>
#include <stm32f10x_pwr.h>
#include <stm32f10x_exti.h>
#include "can.h"
#include "ad.h"
//#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//ͨ�ö�ʱ�� ��������			   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/03
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
u8 TIM3_STA=0;
u8 OUT_ACC_Count = 0;
u8 GPIOB_12_STATUE = 0;
//u8 BD_Count = 0;

void itoa(u32 num, u8 *p , u8 ch)
{
	u8 i,k;
	u32 j;
	j=num;
	for(i=1;i<12;i++)
	{
		if(j/ch)
		{
			j/=ch;
		}
		else
			break;
	}
	j=num;
	for(k=1;k<i+1;k++)
	{
		if((j%ch)>9)
			p[i-k]=j%ch+'A'-10;
		else
		p[i-k]=j%ch+'0';
		j=j/ch;
	}
	p[i]=0;
}

//ͨ�������ⲿ�¼�������CPU
void My_EXTI_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB , GPIO_PinSource3);
	
	//--------����PB3Ϊ�������룬50MHz------------//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void TIM_INIT(TIM_TypeDef* TIMx, u16 arr,u16 psc)          //TIM2,TIM3
{
		
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
	
    if (TIMx == TIM1)
    {
    	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    }     
    else if (TIMx == TIM2)
    {
    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);;
    }
    else if (TIMx == TIM3)
    {
    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    }
    else if (TIMx == TIM4)
    {
    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    } 
    else if (TIMx == TIM5)
    {
    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    } 
    else if (TIMx == TIM6)
    {
    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    } 
    else if (TIMx == TIM7)
    {
    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    }
	TIM_DeInit(TIMx);
    TIM_TimeBaseInitStruct.TIM_Period = arr;					//������
    TIM_TimeBaseInitStruct.TIM_Prescaler = psc;				//��Ƶ��   ��ʱֵΪ72MHZ/(arr+1)(psc+1)
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;
	TIM_TimeBaseInit(TIMx,&TIM_TimeBaseInitStruct);
	TIM_ClearFlag(TIMx, TIM_FLAG_Update); //�������жϱ�־ 
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE); 
//	TIM_Cmd(TIMx, ENABLE); //����ʱ��
}

void NVIC_INIT(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the Priority Grouping with 1 bit */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;                      //10S��ʱ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;  	//USART1 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;		//USART2���ձ���
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;				//5
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;   //��ʱ��2�﷢�ͱ�������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQChannel;  //USART3����ˢ������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQChannel; //��ʱ��5�﷢��ˢ������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN_RX0_IRQChannel; //CAN���ݵĽ��պͷ���
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQChannel; //��ʱ��6�ﶨʱ�ر�IN_ACC
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQChannel; // ˢ����ʱ����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQChannel; 
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
}
 


void TIM2_IRQHandler(void)					//USART2 ��������
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	BD_Rec_Statue = BD_REC_COM;
	USART2_RX_BUF[USART2_RX_STA]='\0'; 	//&0xfff		   
	TIM2->CNT=0;
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);		//����жϱ�־λ		   
	TIM_Cmd(TIM2, DISABLE);		//�ر�TIM2  
	USART2_RX_STA = 0;
}

void TIM3_IRQHandler(void)					 //USART1  �������	
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	USART_MODE=3;		
	TIM3->CNT=0; 
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);		//����жϱ�־λ		   
	TIM_Cmd(TIM3, DISABLE);			//�ر�TIM3 
}

void TIM4_IRQHandler(void)             //USART1  �������
{
	u8 p[10];
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
 	USART1_RX_BUF[USART1_RX_STA&0XFF]='\0';
//	printf("%s",USART1_RX_BUF);
	USART1_RX_STA|=0x8000;	//��ǽ������
	if(USART_MODE==2)
	{
		USART_MODE=4;
		itoa(UPDATE_RX_LEN,p,10);
		printf("<UPDATELENGTH:%s>\r\n",	(char*)p);
		itoa(CRC_Check(UPDATE_RX_BUF,UPDATE_RX_LEN),chCRC,16);
		printf("<UPDATELENGTH:%s>\r\n",	(char*)chCRC);
	}
	TIM4->CNT=0; 
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);		//����жϱ�־λ		   
	TIM_Cmd(TIM4, DISABLE);			//�ر�TIM4 
}

void TIM5_IRQHandler(void)									//USART3  ˢ������
{
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	IC_Card_Statue = IC_CARD_REC_COM;
	USART3_RX_BUF[USART3_RX_STA]=0; 			   
	TIM5->CNT=0;
	TIM_Cmd(TIM5, DISABLE);			//�ر�TIM2  
}

void TIM6_IRQHandler(void)         //��ʱ,IN_ACC
{
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	OUT_ACC_Count++;
}

//void TIM7_IRQHandler(void)         //��ʱ��������
//{
//	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
//	BD_Init();
//	printf("\rBD_INIT\r\n");
//}

void My_Sleep(u8* count)
{
	if(*count>=65)
	{	
		//�ص�����GPIOB_3�˿��ⲿ�жϵ����������ж�
//		CAN_ClearITPendingBit(CAN_IT_FMP0);
//		CAN_ITConfig(CAN_IT_FMP0, DISABLE);
//															   
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
//		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
//		
//		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
//		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
//		
//		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
//		USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
//		
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//		TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
//		
//		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//		TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
//		
//		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
//		TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE);
//		
//		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
//		TIM_ITConfig(TIM5,TIM_IT_Update,DISABLE);
//		
//		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
//		TIM_ITConfig(TIM6,TIM_IT_Update,DISABLE);
//		
//		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
//		TIM_ITConfig(TIM7,TIM_IT_Update,DISABLE);		
			
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
		GPIOB_12_STATUE = 1;
		*count = 0;
//		My_EXTI_Init();
//		PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
//		RCC_Configuration();
//		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
//		Ad_Init();
	}
	else 
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
		GPIOB_12_STATUE = 0;
	}	
}



















