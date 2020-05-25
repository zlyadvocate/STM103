#ifndef  __USART_H
#define  __USART_H

#include<stm32f10x_type.h>

#define USART_1 1
#define USART_2 2
#define USART_3 3

//定义串口是否接收到数据的一种状态
#define USART_REC 1
#define USART_EMP 0

//定义北斗一次数据是否接收完成的一种状态
#define BD_REC_COM 1  //接收完成
#define BD_REC_NCOM 0  //接收未完成

//定义驾驶员IC卡一次数据是否接收完成的一种状态
#define IC_CARD_REC_COM 1  //接收完成
#define IC_CARD_REC_NCOM 0  //接收未完成

#define USART2_MAX_RECV_LEN  1024
#define USART3_MAX_RECV_LEN  150
#define UPDATE_MAX_LEN		32*1024	 

extern u16 USART1_RX_STA;
extern u8  USART1_RX_BUF[256];

extern u16 UPDATE_RX_LEN;
extern u8  UPDATE_RX_BUF[UPDATE_MAX_LEN];
	 
extern u16 USART2_RX_STA;
extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN];
extern u8  BD_Rec_Statue;

extern u8  USART3_RX_STA;
extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN];
extern u8  IC_Card_Statue;  

extern u8  USART_MODE;
extern u8  INFO_PORT;


void itoa(u32 num, u8 *p , u8 ch);
void usart_init(u8 USART_x,u32 BOUND);
void USART_It_Init(u8 USART_x, u16 USART_IT);
void BD_Init(void);
void UART_SENDSTR(USART_TypeDef* USARTx, u8 *UART_STRING,u16 len);


#endif
