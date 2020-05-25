#ifndef   __DATAFLASH_H__
#define   __DATAFLASH_H__


//#include "stm32f10x.h"
#include "stm32f10x_flash.h"
//#include "gpio.h"
#define FLASH_RWADR ((u32)0x08022000) //22000-40000 共120KB 60页 08000000--08040000
#define SAVEPAGES	60
#define APP_ADDR								((u32)0x4000) 	//应用程序开始空间
#define UPDATESTA_ADDR            ((u32)0x08012000) 	//更新程序起始空间
#define UPDATEEND_ADDR            ((u32)0x08020000) 	//更新程序起始空间
//#define FLASH_ADDR            ((u32)0x08010000)

#define CRC_ADR  (u32)0X8021000
#define INFO_ADR (u32)0X8020800
extern u8 FLASHPAGE;
extern u8 PAGEADR;
extern u8 SENDFLASHPAGE;
extern u8 SENDPAGEADR;
extern u8 MDN[15];
extern u8 SMS_INFO[150];

#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((u16)0x800)
  
#else
  #define FLASH_PAGE_SIZE    ((u16)0x400)
#endif




int Flash_Read(u32 iAddress, u8 *buf, u32 iNbrToRead) ;
int Flash_Write(u32 iAddress, u8 *buf, u32 iNbrToWrite);
void Set_WRFlag(u8 page);
void Set_SendFlag(u8 page,u8 num);
void Get_FlashADR(void);
void Flash_WRData(u8 *buf);
void Flash_EarseData(void);
u8 Flash_RDSend(u8 *buf);
u16 Flash_Write_Without_check(u32 iAddress, u8 *buf, u16 iNumByteToWrite);
void Save_INFO(void);
void Load_INFO(void);
void Flash_Ear_Save(void);
#endif

