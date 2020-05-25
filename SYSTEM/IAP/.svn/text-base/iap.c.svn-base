#include "sys.h"
#include "iap.h"
#include "flash.h"
#include "timer.h"
//#include "core_cm3.h"
//#define SCB_AIRCR_VECTKEY_Pos              16  
//#define SCB_AIRCR_PRIGROUP_Pos              8                                             /*!< SCB AIRCR: PRIGROUP Position */
//#define SCB_AIRCR_PRIGROUP_Msk             (7ul << SCB_AIRCR_PRIGROUP_Pos)
//#define SCB_AIRCR_SYSRESETREQ_Pos           2                                             /*!< SCB AIRCR: SYSRESETREQ Position */
//#define SCB_AIRCR_SYSRESETREQ_Msk          (1ul << SCB_AIRCR_SYSRESETREQ_Pos) 
//#define __INLINE         __inline
//static __INLINE void NVIC_SystemReset(void)
//{
//  SCB->AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos)      | 
//                 (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) | 
//                 SCB_AIRCR_SYSRESETREQ_Msk);                   /* Keep priority group unchanged */
//  __DSB();                                                     /* Ensure completion of memory access */              
//  while(1);                                                    /* wait until reset */
//}
//
//static __INLINE void __set_FAULTMASK(u32 faultMask)
//{
//  register u32 __regFaultMask   __ASM("faultmask");
//  __regFaultMask = (faultMask & 1);
//}

__asm void SystemReset(void)  
{  
	MOV R0, #1           //;     
	MSR FAULTMASK, R0    //; 清除FAULTMASK 禁止一切中断产生
	LDR R0, =0xE000ED0C  //;  
	LDR R1, =0x05FA0004  //;    
	STR R1, [R0]         //; 系统软件复位
//	NOP
deadloop
	B deadloop        //; 死循环使程序运行不到下面的代码
} 


void IAP_PROC(USART_TypeDef* USARTx)
{
   	u8 *p,*p1,USART_BUF[256];
	u32 UART_STA;
	if(USART_MODE==3)                          //切换到正常模式
	{
		TIM2->CNT=0;
		TIM3->CNT=0;
		TIM4->CNT=0;
		TIM5->CNT=0;
		TIM6->CNT=0;
//		TIM7->CNT=0;
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);
		TIM_ClearFlag(TIM4, TIM_FLAG_Update);
		TIM_ClearFlag(TIM5, TIM_FLAG_Update);
		TIM_ClearFlag(TIM6, TIM_FLAG_Update);
//		TIM_ClearFlag(TIM7, TIM_FLAG_Update);
		TIM_Cmd(TIM2, DISABLE);
		TIM_Cmd(TIM3, DISABLE);
		TIM_Cmd(TIM4, DISABLE);
		TIM_Cmd(TIM5, DISABLE);
		TIM_Cmd(TIM6, DISABLE);
//		TIM_Cmd(TIM7, DISABLE);
		USART_Cmd(USART2, DISABLE);
		USART_Cmd(USART3, DISABLE);
		USART_Cmd(USART2, ENABLE);
		USART_Cmd(USART3, ENABLE);	
		USART_MODE=0;	
		USART1_RX_STA=0;
		UPDATE_RX_LEN=0;
	}
	if(USARTx==USART1)
	{
		UART_STA=USART1_RX_STA;
		strcpy((char*)USART_BUF,(char*)USART1_RX_BUF);
	}
//	else if(USARTx==UART4)
//	{
//		UART_STA=UART4_RX_STA;
//		strcpy((char*)USART_BUF,(char*)UART4_RX_BUF);
//	}
	if(UART_STA&0X8000)
	{
		switch(USART_MODE)
		{
			case 0:
		/*		if(strstr((char*)USART_BUF,"UPDATEUBLOXSTART"))					//更新UBLOX，关闭2G
				{
					USART_MODE=1;		
					TIM3_STA=0;		
					USART_Cmd(USART2, DISABLE);
					TIM_Cmd(TIM2, DISABLE);
					UPDATE_RX_LEN=0;
					printf("GETUBLOXREADY\r\n");
				//	UART_SENDSTR(USARTx, "GETUBLOXREADY\r\n",14);
					TIM_Cmd(TIM3, ENABLE);
				} */
				if(strstr((char*)USART_BUF,"UPDATEPROGRAMSTART"))   //更新程序，关闭2G GPS串口
				{
					
					USART_MODE=2;
					TIM3_STA=0;
					USART_Cmd(USART2, DISABLE);
					TIM_Cmd(TIM2, DISABLE);
					USART_Cmd(USART3, DISABLE);
					TIM_Cmd(TIM5, DISABLE);
					TIM_Cmd(TIM6, DISABLE);
					UPDATE_RX_LEN=0;
					printf("GETPROGRAMREADY\r\n");
				//	UART_SENDSTR(USARTx, "GETPROGRAMREADY\r\n",17);
					TIM_Cmd(TIM3, ENABLE);
				}
				else if(strstr((char*)USART_BUF,"UPDATERESTARTMODE"))   	//重启设备升级       
				{
					FLASH_Unlock();
					FLASH_ProgramHalfWord((UPDATEEND_ADDR-4),0X01);
					FLASH_Lock();
					SystemReset();
				//	__set_FAULTMASK(1);     //关闭所有中断
				//	NVIC_SystemReset();			//重启设备
				}
//				else if(strstr((char*)USART_BUF,"$JSDCARD,"))
//				{
////					Get_information(USARTx);
////					USART3_SEND((char*)USART_BUF[8], (UART_STA&0xFF)-7);
//				
//				}
					
				break;
//		case 1:													//AGPS接收完毕
//			if(strstr((char*)UPDATE_RX_BUF,"UPDATEUBLOXEND"))
//			{
//				USART_MODE=3;
//				printf("GETAUBLOXEND\r\n");
//		//		UART_SENDSTR(USARTx, "GETAUBLOXEND\r\n",12);
//	//			USART_Cmd(USART2, ENABLE);
//			}
//			else
//			{
////				UART_SENDSTR(USART3,UPDATE_RX_BUF,UPDATE_RX_LEN);
//				UPDATE_RX_LEN=0;
//			}
//			break;
		case 4:
			if((p=(u8*)strstr((char*)USART_BUF,"CRC:"))!=NULL)
			{
				p1=(u8*)strstr((char*)p,">");
				if(p1&&(p1<=p+8+4))
				{
					p1[0]=0;
					if(strcmp((char*)chCRC,(char*)(p+4))==0)
					{
						printf("SAVE UPDATE PROGRAM\r\n");
				//		UART_SENDSTR(USARTx,"SAVE UPDATE PROGRAM\r\n",21);
						Flash_Ear_Save();
						SystemReset();
				//		__set_FAULTMASK(1);     //关闭所有中断
				//		NVIC_SystemReset();			//重启设备
					}
					else 
					{
						printf("CRC WRONG\r\n");
						//UART_SENDSTR(USARTx,"CRC WRONG\r\n",11);
						USART_MODE=3;
					}
				}	
				else 
				{	printf("CRC WRONG\r\n");
				//	UART_SENDSTR(USARTx,"CRC WRONG\r\n",11);
					USART_MODE=3;
				}
			}
			if(strstr((char*)USART_BUF,"UPDATEPROGRAMEND"))
			{
				
			//	UART_SENDSTR(USARTx, "GETPROGRAMEND\r\n",15);
			//	UART_SENDSTR(USARTx, "RESETNOW\r\n",10);
				TIM3->CNT=0;
				TIM_ClearFlag(TIM3, TIM_FLAG_Update);
				TIM_Cmd(TIM3, DISABLE);
//				UART_SENDSTR(USART1, UPDATE_RX_BUF,UPDATE_RX_LEN);
				USART_Cmd(USARTx, DISABLE);
				Flash_Ear_Save();
				USART_Cmd(USARTx, ENABLE);
				printf("GETPROGRAMEND\r\nRESETNOW\r\n");
				SystemReset();
			//	__set_FAULTMASK(1);     //关闭所有中断
			//	NVIC_SystemReset();			//重启设备

			}
			break;
//		case 3:
//			break;
		default:
			break;
		}
		if(USARTx==USART1)
			USART1_RX_STA=0;
			UART_STA=0;
//		else if(USARTx==UART4)
//			UART4_RX_STA=0;
	}
}

void Get_information(USART_TypeDef* USARTx)
{
	u8 *p;
	u8 num;
	if(USARTx==USART1)
	{
		p=(u8*)strstr((char*)USART1_RX_BUF,"Get_information");
	}
//	else if(USARTx==UART4)
//	{
//		INFO_PORT=1;
//		p=(u8*)strstr((char*)UART4_RX_BUF,"Get_information");
//	}

//	p=(u8*)strstr((char*)USART1_RX_BUF,"Get_information");
	while(p!=NULL)
	{
		p=(u8*)strstr((char*)p,"::");
		num=*(p+2);
		switch(num)
		{
			case '0'://printf("%s",VER);//UART_SENDSTR(USARTx,VER,strlen((char*)VER));   //版本
				break;
			case '1'://gprs_checkinfo();															//CPOS 和CSQ
				break;
			case '2'://
				break;
			case '3'://
				break;
			case '4':
				break;
			case '5':
				break;
			case '6':
				break;
			case '7'://
				break;
			case '8'://
				break;
			case '9':
				SystemReset();
				break;
			default:
				break;
		}
		p=(u8*)strstr((char*)(p+1),":");
	}
	INFO_PORT=0;
}

