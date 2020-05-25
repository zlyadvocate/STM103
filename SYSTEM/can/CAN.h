#ifndef __CAN_H
#define __CAN_H

#include <stm32f10x_can.h>

//--------CAN总线是否接到数据-------------
#define CAN_REC 1
#define CAN_EMP 0

void My_CAN_Init(void);
void CAN_TxData(CanTxMsg* TxMessage);
void IT_Config(void);

extern u8 CAN_Statue;
extern CanTxMsg store_Message;

#endif 
