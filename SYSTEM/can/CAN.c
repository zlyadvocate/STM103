#include <stm32f10x_map.h>
#include <stm32f10x_conf.h>
#include <stm32f10x_can.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_can.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_nvic.h>
#include <stdio.h>
#include "delay.h"
#include "usart.h"
#include "CAN.h"

//--------CAN总线状态变量，全局变量-------------
u8 CAN_Statue = CAN_EMP;

CanTxMsg store_Message = {0x00,0x1234,CAN_ID_EXT,CAN_RTR_DATA,7,"CAN_REC"};

void My_CAN_Init()
{
	CAN_InitTypeDef CAN_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);//使能CAN的时钟
	
	//--------配置GPIOA.11为50MHz上拉输入-----------
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//--------配置GPIOA.12为50MHz复用推挽输出-----------
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//-------
	CAN_DeInit();
	CAN_StructInit(&CAN_InitStructure);
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_4tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler = 18;//250kb波特率  CAN波特率 = 36MHZ/CAN_Prescaler/(1+CAN_BS1+CAN_BS2)
	CAN_Init(&CAN_InitStructure);
	
	CAN_FilterInitStructure.CAN_FilterNumber = 1;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig(CAN_IT_FMP0, ENABLE);
}

void CAN_TxData(CanTxMsg* TxMessage)
{
	u8 i = 0;
    CanTxMsg Message;
	Message.StdId = TxMessage->StdId;
	Message.ExtId = TxMessage->ExtId;
	Message.IDE = TxMessage->IDE;
	Message.RTR = TxMessage->RTR;
	Message.DLC = TxMessage->DLC;
	for (i=0; i<Message.DLC; i++)
	{
		Message.Data[i] = TxMessage->Data[i];
	}
	CAN_Transmit(&Message);	
}

void USB_LP_CAN_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
    u8 i = 0;
    u8 u8_RxLen = 0;
	
	CAN_ClearITPendingBit(CAN_IT_FMP0);
		
	CAN_Statue = CAN_REC;
	
    RxMessage.StdId = 0x00;
    RxMessage.ExtId = 0x00;
    RxMessage.IDE = 0;
    RxMessage.RTR = 0;
    RxMessage.DLC = 0;
    RxMessage.FMI = 0;
    for( i=0;i<8;i++)
	{
        RxMessage.Data[i]=0x00; 
    } 
    CAN_Receive(CAN_FilterFIFO0, &RxMessage);
    u8_RxLen = RxMessage.DLC; 
	for( i=0;i<u8_RxLen;i++)
	{
        store_Message.Data[i]=RxMessage.Data[i]; 
    } 
	store_Message.StdId = RxMessage.StdId;
	store_Message.ExtId = RxMessage.ExtId;
	store_Message.IDE = RxMessage.IDE;
	store_Message.RTR = RxMessage.RTR;
	store_Message.DLC = RxMessage.DLC;
//	CAN_TxData(&store_Message);
}








