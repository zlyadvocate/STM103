#include "sys.h"
u8 FLASHPAGE=0;		//当前存储页
u8 PAGEADR=0;			//当前页存储位置
u8 SENDFLASHPAGE=0;
u8 SENDPAGEADR=0;
u8 MDN[15]="123456789012";   //不大于13位
u8 SMS_INFO[150];

u16 Flash_Write_Without_check(u32 iAddress, u8 *buf, u16 iNumByteToWrite) {
    u16 i;
    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
    i = 0;
    
//    FLASH_UnlockBank1();
    while((i < iNumByteToWrite) && (FLASHStatus == FLASH_COMPLETE))
    {
			
      FLASHStatus = FLASH_ProgramHalfWord(iAddress, *(u16*)buf);
      i = i+2;
      iAddress = iAddress + 2;
      buf = buf + 2;
    }
    
    return iNumByteToWrite;
}
/**
  * @brief  Programs a half word at a specified Option Byte Data address.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Address: specifies the address to be programmed.
  * @param  buf: specifies the data to be programmed.
  * @param  iNbrToWrite: the number to write into flash
  * @retval if success return the number to write, -1 if error
  *  
  */
int Flash_Write(u32 iAddress, u8 *buf, u32 iNbrToWrite) {
                /* Unlock the Flash Bank1 Program Erase controller */
  u32 secpos;
  u32 iNumByteToWrite = iNbrToWrite;
	u16 secoff;
	u16 secremain;  
  u16 i = 0;    
  u8 tmp[FLASH_PAGE_SIZE];
	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
        
  FLASH_Unlock();
	secpos=iAddress & (~(FLASH_PAGE_SIZE -1 )) ;//扇区地址 
	secoff=iAddress & (FLASH_PAGE_SIZE -1);     //在扇区内的偏移
	secremain=FLASH_PAGE_SIZE-secoff;           //扇区剩余空间大小 

        
   if(iNumByteToWrite<=secremain) secremain = iNumByteToWrite;//不大于2048个字节
        
   while(1) 
		 {		
          Flash_Read(secpos, tmp, FLASH_PAGE_SIZE);   //读出整个扇区
					
          for(i=0;i<secremain;i++) 
					{       //校验数据
						if(tmp[secoff+i]!=0XFF)	break;       //需要擦除 
					}
           if(i<secremain) 
						{  //需要擦除
                FLASHStatus = FLASH_ErasePage(secpos); //擦除这个扇区
                if(FLASHStatus != FLASH_COMPLETE)
                  return -1;
								
                for(i=0;i<secremain;i++) 
								{   //复制
                 tmp[i+secoff]=buf[i];   
								}
                Flash_Write_Without_check(secpos ,tmp ,FLASH_PAGE_SIZE);//写入整个扇区  
								
            } 
						else 
						{
                Flash_Write_Without_check(iAddress,buf,secremain);//写已经擦除了的,直接写入扇区剩余区间.
							
            }
            
            if(iNumByteToWrite==secremain) //写入结束了
                break;
            else {
                secpos += FLASH_PAGE_SIZE;
                secoff = 0;//偏移位置为0 
                buf += secremain;  //指针偏移
                iAddress += secremain;//写地址偏移    
                iNumByteToWrite -= secremain;  //字节数递减
                if(iNumByteToWrite>FLASH_PAGE_SIZE) secremain=FLASH_PAGE_SIZE;//下一个扇区还是写不完
                else secremain = iNumByteToWrite;  //下一个扇区可以写完了
            }
            
         }
        
        FLASH_Lock();
        return iNbrToWrite; 
}






/**
  * @brief  Programs a half word at a specified Option Byte Data address.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Address: specifies the address to be programmed.
  * @param  buf: specifies the data to be programmed.
  * @param  iNbrToWrite: the number to read from flash
  * @retval if success return the number to write, without error
  *  
  */
int Flash_Read(u32 iAddress, u8 *buf, u32 iNbrToRead) 
{
        int i=0;
//		u8 ch1[10];
//	u32 ch2;
				u8* ptr;
				FLASH_Unlock();
/*				ch2=iAddress;
			for(i=0;i<8;i++)
			{
				if(ch2%16>9)
				{
					ch1[7-i]=ch2%16+'A'-10;
				}
				else
			ch1[7-i]=ch2%16+'0';
			ch2/=16;
			}
		ch1[8]='\0';
		UART_SENDSTR(USART1,ch1);*/
				ptr=(u8*)iAddress;

				for(i=0;i<iNbrToRead;i++)
				{
					buf[i]= ptr[i];
					
				}
        return i;
}

/*
//获取当前已存储位置 和发送位置
void Get_FlashADR(void)
{
	u8 i;
	u32 rddata1;
	FLASHPAGE=0;
	PAGEADR=0;	
	SENDFLASHPAGE=0;
	SENDPAGEADR=0;
	for(i=0;i<SAVEPAGES;i++)
	{
		rddata1=*(u32*)(FLASH_RWADR+FLASH_PAGE_SIZE*(i+1)-4);
		if(rddata1==0xFFFFFFFF)
		{
			FLASHPAGE=i;break;
		}
	}
	for(i=0;i<30;i++)
	{
		rddata1=*(u8*)(FLASH_RWADR+FLASH_PAGE_SIZE*FLASHPAGE+64*i);
		if(rddata1==0xFF)
		{
			PAGEADR=i;break;
		}
	}
	rddata1=*(u32*)(FLASH_RWADR+FLASH_PAGE_SIZE*SAVEPAGES-4);//获取最后一页存储状态
	if(rddata1==1)																		//存储完
	{
		rddata1=*(u32*)(FLASH_RWADR+FLASH_PAGE_SIZE*SAVEPAGES-8);//获取最后一页发送状态
		if(rddata1==0xFFFFFFFF)
		{	
			for(i=FLASHPAGE+1;i<SAVEPAGES;i++)
			{
				rddata1=*(u32*)(FLASH_RWADR+FLASH_PAGE_SIZE*(i+1)-4);
				if(rddata1==1)
				{
					rddata1=*(u32*)(FLASH_RWADR+FLASH_PAGE_SIZE*(i+1)-8);
					if(rddata1==0xFFFFFFFF)
					{
						SENDFLASHPAGE=i;rddata1=1;break;
					}
				}
			}
		}
		else
		{rddata1=0;}
	}
	if((rddata1==0)||(rddata1==0xFFFFFFFF))
	{	
		for(i=0;i<=FLASHPAGE;i++)
			{
				rddata1=*(u32*)(FLASH_RWADR+FLASH_PAGE_SIZE*(i+1)-8);
				if(rddata1==0xFFFFFFFF)
				{
					SENDFLASHPAGE=i;break;
				}
			}
	}
	for(i=0;i<30;i++)
	{
		rddata1=*(u32*)(FLASH_RWADR+FLASH_PAGE_SIZE*SENDFLASHPAGE+64*i+48);
		if(rddata1==0xFFFFFFFF)
		{
			SENDPAGEADR=i;break;
		}			
	}
}

//设置页存储完标志
void Set_WRFlag(u8 page)
{
	FLASH_Unlock();
	FLASH_ProgramWord((FLASH_RWADR+FLASH_PAGE_SIZE*(page+1)-4),1);
	FLASH_Lock();
}

//设置发送完标志 num取值为0-30(30为整页发送完）
void Set_SendFlag(u8 page,u8 num)
{
	FLASH_Unlock();
	if(num==30)
		FLASH_ProgramWord((FLASH_RWADR+FLASH_PAGE_SIZE*(page+1)-8),1);
	else
		FLASH_ProgramWord((FLASH_RWADR+FLASH_PAGE_SIZE*page+64*num+48),1);  
	FLASH_Lock();
}



//数据存储
void Flash_WRData(u8 *buf)
{	//u8 ch1[10];
//	u32 ch2;
//	u8 i;
	u32 dataaddr;
	dataaddr=FLASH_RWADR+FLASH_PAGE_SIZE*FLASHPAGE+PAGEADR*64;
	FLASH_Unlock();
	Flash_Write_Without_check(dataaddr, buf, 44);
//	ch2=dataaddr;
//	for(i=0;i<8;i++)
//	{
//		if(ch2%16>9)
//		{
//			ch1[7-i]=ch2%16+'A'-10;
//		}
//		else
//		ch1[7-i]=ch2%16+'0';
//		ch2/=16;
//	}
//	ch1[8]='\0';
//	UART_SENDSTR(USART1,ch1);
//	Flash_Write(dataaddr, buf, 44);
	PAGEADR++;
	if(PAGEADR==30)
	{
		Set_WRFlag(FLASHPAGE);
		FLASHPAGE++;
		PAGEADR=0;
		if(FLASHPAGE==SAVEPAGES)
		{
			FLASHPAGE=0;
		}
	}
	if((*(u32*)(FLASH_RWADR+FLASH_PAGE_SIZE*(FLASHPAGE+1)-4))==1)
	{
		
		Flash_EarseData();
	}
		FLASH_Lock();
}


//页擦除
void Flash_EarseData(void)
{
	u8 i;
	FLASH_Unlock();
	if(FLASHPAGE<(SAVEPAGES-10))
	{
		for(i=0;i<10;i++)
		{
			FLASH_ErasePage(FLASH_RWADR+FLASH_PAGE_SIZE*(FLASHPAGE+i));		
		}
		if((FLASHPAGE<=SENDFLASHPAGE)&&((FLASHPAGE+10)>SENDFLASHPAGE))
		{
			SENDFLASHPAGE=FLASHPAGE+10;
		}
	}
	else
	{
		for(i=FLASHPAGE;i<SAVEPAGES;i++)
		{
			FLASH_ErasePage(FLASH_RWADR+FLASH_PAGE_SIZE*i);
		}
		if((FLASHPAGE<=SENDFLASHPAGE))
		{
			SENDFLASHPAGE=0;
		}
	}
	FLASH_Lock();
}


u8 Flash_RDSend(u8 *buf)
{
	u8 i,j;
//	u8 ch[2];
	GPS_DATA_STA=0;
	if(SENDFLASHPAGE>FLASHPAGE)
	{	
		for(i=SENDFLASHPAGE;i<SAVEPAGES;i++)
		{
//			GPS_DATA_STA=0;
			UART_SENDSTR(USART1, ">>>>\r\n",6);
			gprs_senddata[0]=0;
//			memset(gprs_senddata,0,1500);
			for(j=SENDPAGEADR;j<30;j++)
			{
				Flash_Read(FLASH_RWADR+FLASH_PAGE_SIZE*i+j*64, buf, 44);
				strcat((char*)gprs_senddata,(char*)buf);
				if((j+1)%15==0)
				{
					if(gprs_send_data(gprs_senddata,strlen((char*)gprs_senddata))) return 1;
					gprs_senddata[0]=0;
//					memset(gprs_senddata,0,1500);
				}
//				strcpy((char*)GPS_DATA1[GPS_DATA_STA],(char*)buf);
//				GPS_DATA_STA++;
//				UART_SENDSTR(USART1, buf);

			}
			Set_SendFlag(i,j);
			
//			if(gprs_send_data(gprs_senddata,strlen((char*)gprs_senddata))) return 1;
//			else Set_SendFlag(i,j);
			SENDFLASHPAGE++;

		}
		if(SENDFLASHPAGE==SAVEPAGES)
			SENDFLASHPAGE=0;
	}
	
	if(SENDFLASHPAGE<FLASHPAGE)
	{	
		for(i=SENDFLASHPAGE;i<FLASHPAGE;i++)
		{	memset(gprs_senddata,0,1500);
			UART_SENDSTR(USART1, "<<<<\r\n",6);
			for(j=SENDPAGEADR;j<30;j++)
			{
				Flash_Read(FLASH_RWADR+FLASH_PAGE_SIZE*i+j*64, buf, 44);
				strcat((char*)gprs_senddata,(char*)buf);
				if((j+1)%15==0)
				{
//					UART_SENDSTR(USART1,gprs_senddata,strlen((char*)gprs_senddata));
					if(gprs_send_data(gprs_senddata,strlen((char*)gprs_senddata))) return 1;
					memset(gprs_senddata,0,1500);
				}
			}
			Set_SendFlag(i,j);
			
//			if(gprs_send_data(gprs_senddata,strlen((char*)gprs_senddata))) return 1;
//			else Set_SendFlag(i,j);
			SENDFLASHPAGE++;
		}
	}
	return 0;
}


//存储参数 
void Save_INFO(void)
{
//	u8 pp[3][50];
//	u8 i;
//	for(i=0;i<150;i++)
//	{
//		SMS_INFO[i]=*(u8*)(UPDATEEND_ADDR+i);
//	}
	FLASH_Unlock();
	FLASH_ErasePage(INFO_ADR);
	FLASH_UnlockBank1();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	Flash_Write_Without_check(INFO_ADR,IPADDR,strlen((char*)IPADDR));
	Flash_Write_Without_check(INFO_ADR+50,MDN,strlen((char*)MDN));
	Flash_Write_Without_check(INFO_ADR+90,GPS_CY,strlen((char*)GPS_CY));
	FLASH_Lock();
	UART_SENDSTR(USART1,GPS_CY,strlen((char*)GPS_CY)|1<<15);

}

void Load_INFO(void)
{
	u8 i;
	if(*(u8*)(INFO_ADR)!=0XFF)
	{
		for(i=0;i<30;i++)
		{
			if(*(u8*)(INFO_ADR+i)==0XFF)
				break;
			else
			IPADDR[i]=*(u8*)(INFO_ADR+i);
		}
		IPADDR[i+1]=0;
	}
	if(*(u8*)(INFO_ADR+50)!=0XFF)
	{
		for(i=0;i<30;i++)
		{
			if(*(u8*)(INFO_ADR+50+i)==0XFF)
				break;
			else
			MDN[i]=*(u8*)(INFO_ADR+50+i);
		}
	MDN[i+1]=0;
	}
	if(*(u8*)(INFO_ADR+90)!=0XFF)
	{
		for(i=0;i<30;i++)
		{
			if(*(u8*)(INFO_ADR+90+i)==0XFF)
				break;
			else
			GPS_CY[i]=*(u8*)(INFO_ADR+90+i);
		}
		GPS_CY[i+1]=0;
		GPS_CYCLE=(u8)atoi((char*)GPS_CY);
	}
}
*/
void Flash_Ear_Save(void)
{
	u8 j;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	for(j = (u8)(((UPDATESTA_ADDR&0XFFFFF)/FLASH_PAGE_SIZE)); j < (u8)(((UPDATEEND_ADDR&0XFFFFF)/FLASH_PAGE_SIZE)); j++)
	{
		FLASH_ErasePage(0x8000000 + (j * FLASH_PAGE_SIZE));
	}
	FLASH_Unlock();
	Flash_Write_Without_check(UPDATESTA_ADDR, UPDATE_RX_BUF, UPDATE_RX_LEN);
	FLASH_ProgramHalfWord((UPDATEEND_ADDR-8),0X01);
	FLASH_Lock();
}
