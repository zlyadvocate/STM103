#include <stm32f10x_map.h>
#include <stm32f10x_type.h>
#include <stm32f10x_nvic.h>

#include "clock_init.h"
#include "sys.h"

ErrorStatus HSEStartUpStatus;
//void System_Clock_Init()
//{
//	RCC->CR = (1<<16);//打开HSE
//	while(!(RCC->CR>>17));//等待HSE准备好
//	RCC->CFGR &= 0xffffff7f;//AHB=SYSCLK不分频
//	RCC->CFGR |= (1<<10);//	APB1=HCLK 2分频
//	RCC->CFGR &= 0xffffffef;//APB2=HCLK 
//	RCC->CFGR |= 0x001D0000;//HSE时钟作为PLL输入时钟,PLL 9倍频输出=72MHz
//	RCC->CR |= (1<<24);//使能PLL
//	while(!(RCC->CR>>25));//等待PLL时钟就绪
//	FLASH->ACR|=0x32;//打开FLASH预取缓冲区，设置FLASH等待周期数为2		
//	RCC->CFGR |= 0x02;//PLL输出作为系统时钟
//	RCC->CR = (1<<16); //关闭除了HSE之外的时钟源
//	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);//0x4000);
//}


void RCC_Configuration(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
}

