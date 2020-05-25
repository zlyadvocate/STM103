#include<stm32f10x_type.h>
#include<stm32f10x_map.h>
#include "usart.h"
#include <stm32f10x_usart.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <stdio.h>
#include "sys.h"
#include "delay.h"


u16 USART1_RX_STA=0;
u8  USART1_RX_BUF[256] = {0};

u16 USART2_RX_STA=0;
u8  USART2_RX_BUF[USART2_MAX_RECV_LEN] = {0};	// 北斗数据

u8  USART3_RX_STA=0;
u8  USART3_RX_BUF[USART3_MAX_RECV_LEN] = {0}; //刷卡数据

u16 UPDATE_RX_LEN;
u8  UPDATE_RX_BUF[UPDATE_MAX_LEN];

u8  USART_MODE=0;
u8  INFO_PORT=0;

//北斗数据接收状态变量
u8 BD_Rec_Statue = BD_REC_NCOM;

//驾驶员IC卡数据接收状态变量
u8 IC_Card_Statue = IC_CARD_REC_NCOM;

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
//	USART_SendData(USART1, (u8)ch);
//	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) ;
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}

void usart_init(u8 USART_x,u32 BOUND)
{
	 //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	switch(USART_x)
	{
		case USART_1:   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
					 	USART_DeInit(USART1);  //复位串口1
						//USART1_TX   PA.9
					    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
					    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
					    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
					    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
						   
					    //USART1_RX	  PA.10
					    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
					    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
					    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10					
						
					    //USART 初始化设置
						USART_InitStructure.USART_BaudRate = BOUND;//一般设置为9600;
						USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
						USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
						USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
						USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
						USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
					
					    USART_Init(USART1, &USART_InitStructure); //初始化串口
						USART_Cmd(USART1, ENABLE); break;
							
		case USART_2 :  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2，GPIOA时钟
					 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
						USART_DeInit(USART2);  //复位串口2
						 //USART2_TX   PA.2
					    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
					    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
					    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
					    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
						   
					    //USART2_RX	  PA.3
					    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
					    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
					    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10					
					
					   //USART 初始化设置
						USART_InitStructure.USART_BaudRate = BOUND;//一般设置为9600;
						USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
						USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
						USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
						USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
						USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
						
					    USART_Init(USART2, &USART_InitStructure); //初始化串口
						USART_Cmd(USART2, ENABLE); break;
							
		case USART_3 :  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3，GPIOB时钟
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
					 	USART_DeInit(USART3);  //复位串口3
						 //USART3_TX   PB.10
					    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
					    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
					    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
					    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PA2
						   
					    //USART3_RX	  PB.11
					    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
					    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
					    GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PA10					
						
					   //USART 初始化设置
						USART_InitStructure.USART_BaudRate = BOUND;//一般设置为9600;
						USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
						USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
						USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
						USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
						USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
						
					    USART_Init(USART3, &USART_InitStructure); //初始化串口
						USART_Cmd(USART3, ENABLE); break;
		default : break;
	}
}

/*******************************************
函数名称：USART_It_Init
功 能：初始化串口中断
参 数：USART_x：x 可以是 1 、2或者 3 来选择 USART 外设 USART1、USART2 或 USART3；USART_IT：要打开的中断种类
返 回 值：无
********************************************/
void USART_It_Init(u8 USART_x, u16 USART_IT)
{
	switch(USART_x)
	{
		case USART_1 : USART_ITConfig(USART1, USART_IT, ENABLE);break;
		case USART_2 : USART_ITConfig(USART2, USART_IT, ENABLE);break;
		case USART_3 : USART_ITConfig(USART3, USART_IT, ENABLE);break;
	}
}


void USART1_IRQHandler(void)          //电脑通信
{
	u8 res;
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	TIM4->CNT=0;
	TIM3->CNT=0;
	TIM_Cmd(TIM4, ENABLE);
	res=USART_ReceiveData(USART1);
	if((USART_MODE==0)||(USART_MODE==4))
	{
		if((USART1_RX_STA&0x8000)==0) 
		{
			USART1_RX_BUF[USART1_RX_STA&0XFF]=res;
			USART1_RX_STA++;
			USART1_RX_STA&=0XFF;
		}
	}
	else
	{
		UPDATE_RX_BUF[UPDATE_RX_LEN&0X7FFF]=res;
		UPDATE_RX_LEN++;
		UPDATE_RX_LEN&=0X7FFF;
	}
} 


void USART2_IRQHandler(void)                   	 //北斗通信
{
	u8 res;	
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	BD_Rec_Statue = BD_REC_NCOM;
	res=USART_ReceiveData(USART2);
	if(USART2_RX_STA<USART2_MAX_RECV_LEN)		//还可以接收数据
	{
		TIM2->CNT = 0;         					//计数器清空
		TIM_Cmd(TIM2, ENABLE);	 				//使能定时器2的中断 
		USART2_RX_BUF[USART2_RX_STA]= res;		//记录接收到的值	
		USART2_RX_STA++;
	}
	else 
	{
		USART2_RX_STA|=1<<15;					//强制标记接收完成
	} 
} 
//
void USART3_IRQHandler(void)             //刷卡      	 
{
	u8 res;	    
	USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	IC_Card_Statue = IC_CARD_REC_NCOM;
	res=USART_ReceiveData(USART3); 			 
	if(USART3_RX_STA<USART3_MAX_RECV_LEN)		//还可以接收数据
	{
		TIM5->CNT=0;         					//计数器清空
		TIM_Cmd(TIM5, ENABLE);	 	//使能定时器2的中断 
		USART3_RX_BUF[USART3_RX_STA]=res;		//记录接收到的值	
		USART3_RX_STA++;
	}
	else 
	{
		USART3_RX_STA|=1<<7;					//强制标记接收完成
	} 
} 

void UART_SENDSTR(USART_TypeDef* USARTx, u8 *UART_STRING,u16 len)
{
	u8 *p;
	u16 i;
	p=UART_STRING;
	USART_GetFlagStatus(USARTx,USART_FLAG_TC);
	for(i=0;i<(len&0x7FFF);i++)
	{
		USART_SendData(USARTx,*p);
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)== RESET);
		p++;
	}
	if(len&1<<15)
	{
	USART_SendData(USARTx,'\r');
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)== RESET);
	USART_SendData(USARTx,'\n');
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)== RESET);
	}
}


void BD_Init()
{
	//----------定义北斗模块命令语句-------
	u8 BOUNDRATE[] = "$PCAS01,5*19\r\n";  //配置北斗模块波特率为115200命令语句
	u8 i = 0;
//	const u8 SAVE[] = "$PCAS00*01\r\n";         //将当前配置信息保存到 FLASH 中的命令语句
	u8* p = BOUNDRATE;
	for(i=0;i<7;i++)
	{
		delay_ms(1000);
	}
	USART_GetFlagStatus(USART2,USART_FLAG_TC);
	while(*p != '\0')
	{
		USART_SendData(USART2, *p);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)== RESET);
		p++;
	}
//	p = SAVE;
//	while(*p != '\0')
//	{
//		USART_SendData(USART2, *p);
//		p++;		
//	}
	USART_Cmd(USART2, DISABLE);
	usart_init(USART_2,115200);
	USART_It_Init(USART_2, USART_IT_RXNE);
	USART_Cmd(USART2, ENABLE);
}
















