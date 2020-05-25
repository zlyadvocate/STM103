#include "sys.h"

u32 cnCRC32=0X04C11DB7;
u8 chCRC[10];
u32 Reflect(u32 ref,u8 ch)
{
	u32 value=0; //?交换bit0和bit7，bit1和bit6，类推??
	u8 i;
	for(i=1;i<(ch+1);i++)
	{
		if(ref&1)
			value|=1<<(ch-i);
		ref>>=1;
	}
	return value;
}

void Build_CRCTable(u32 crcpoly)
{
	u32 i32,j32,ndata32,nAccum32;
	if(*(u8*)CRC_ADR!=0xFF)
		return;
	for(i32=0;i32<256;i32++)
	{
		ndata32=i32<<24;
		nAccum32=0;
		for(j32=0;j32<8;j32++)
		{
			if((ndata32^nAccum32)&0x80000000)
				nAccum32=(nAccum32<<1)^crcpoly;
			else
				nAccum32<<=1;
			ndata32<<=1;
		}
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		FLASH_ProgramWord(CRC_ADR+i32*4,nAccum32);
//		UART_SENDSTR(USART1, (u8*)&nAccum32,4);
	}
	FLASH_ProgramWord(CRC_ADR+i32*4,0);
	FLASH_Lock();
//	for(i32=0;i32<256*4;i32++)
//	{
//		USART_SendData(USART1,((u8*)CRC_ADR)[i32]);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)== RESET);
//	}
}

u32 CRC_Check(u8 *buf ,u16 len)
{
	u32 CRC32_1=0xFFFFFFFF;
	u16 i;
	u8 tmp,*p;
	p=buf;
	for(i=0;i<len;i++)
	{
		tmp=*p++;
		tmp=(u8)Reflect(tmp,8);
		CRC32_1=(CRC32_1<<8)^((u32*)CRC_ADR)[(CRC32_1>>24)^tmp];
	}
	CRC32_1=Reflect(CRC32_1,32);
	CRC32_1=CRC32_1^0xFFFFFFFF;
	return CRC32_1;
}
