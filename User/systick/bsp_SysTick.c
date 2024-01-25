/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   System tick clock 10us interrupt function library, interrupt time can be freely configured, 
  *          commonly used 1us 10us 1ms interrupt
  *              
  ******************************************************************************
  * @attention
  *
  * HW Platform: Fire Challenger V2
  * Forum: http://www.firebbs.cn
  * Taobao:https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./systick/bsp_SysTick.h"

static __IO u32 TimingDelay;
 
/**
  * @brief  Start the system tick
  * @param  NA
  * @retval NA
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    Interrupt every 1ms
	 * SystemFrequency / 100000	 Interrupt every 10us
	 * SystemFrequency / 1000000 Interrupt every 1us
	 */
	//if (HAL_SYSTICK_Config(SystemCoreClock / 100000))   // original
	//if (HAL_SYSTICK_Config(SystemCoreClock / 2000))	  // roger 20230728 activate, Ticks; 0.5ms	
	if (HAL_SYSTICK_Config(SystemCoreClock / 1000))	     // roger 20230728 activate, Ticks: 1ms
	//if (HAL_SYSTICK_Config(SystemCoreClock / 500))	  // roger 20230728 activate, Ticks; 2ms	
	{ 
		/* Capture error */ 
		while (1);
	}
}

/**
  * @brief   us delay program, 10us is a unit
  * @param  
  *	@arg nTime: Delay_us( 1 ) Then the realized delay is 1 * 10us = 10us
  * @retval  NA
  */
void Delay_us(__IO u32 nTime)
{ 
	TimingDelay = nTime;	

	while(TimingDelay != 0);
}

/**
  * @brief  Get Tick Function
  * @param  NA
  * @retval NA
  * @attention  SysTick call by SysTick_Handler()
  */
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}
/*********************************************END OF FILE**********************/
