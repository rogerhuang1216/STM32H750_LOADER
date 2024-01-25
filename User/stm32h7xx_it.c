/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "stm32h7xx.h"
#include "stm32h7xx_it.h"
/* FreeRTOS头文件 */
//#include "FreeRTOS.h"
//#include "task.h"
//#include "./sai/bsp_sai.h" 
//#include "bsp_mpu_exti.h"
//#include "emXGUI.h"
#include "./usart/bsp_usart.h"
#include "./systick/bsp_SysTick.h"

#include "loader.h"
#include "stm32h7xx_hal_gpio.h"


//#include "./JPEG./decode_dma.h"
//extern RTC_HandleTypeDef hrtc;
//extern SD_HandleTypeDef uSdHandle;
//extern volatile uint8_t video_timeout;//视频播放引入
//extern __IO uint32_t LocalTime;//以太网提供tick
//extern void gyro_data_ready_cb(void);
//extern DCMI_HandleTypeDef DCMI_Handle;
//extern DMA_HandleTypeDef DMA_Handle_dcmi;
/* External variables --------------------------------------------------------*/

extern PCD_HandleTypeDef hpcd;
extern INT32U sys_event;
extern INT8U cmdbuf[4096];
extern volatile INT32S put,get,full;
uint32_t Ticks = 0;


extern __IO uint32_t 	LocalTime; 	/* this variable is used to create a time reference incremented by 10ms */


/******************************************************************************/
/*            Cortex Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */

	printf("\n\r [NMI_Handler] NMI Occur ! \n\r");		 // roger 20230801 deactivate, test spi_fw_upgrade, due to I assign an indirect addr 0x9000_0000
	while(1);
  
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
//	GUI_ERROR("****************Hard Fault!***************\r\n");

	printf("\n\r [HardFault_Handler] Hard Fault \n\r");		 // roger 20230801 deactivate, test spi_fw_upgrade, due to I assign an indirect addr 0x9000_0000
	while(1);
   
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */

	printf("\n\r [MemManage_Handler] MemManage Fault \n\r");	
	while(1);    

  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Pre-fetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */

	printf("\n\r [BusFault_Handler] Bus Fault \n\r");	
	while(1);

  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}




/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

#if 0
void SDMMC1_IRQHandler(void)
{
//  uint32_t ulReturn;
//  /* 进入临界段，临界段可以嵌套 */
//  ulReturn = taskENTER_CRITICAL_FROM_ISR(); 
  HAL_SD_IRQHandler(&uSdHandle);
		
  /* 退出临界段 */
//  taskEXIT_CRITICAL_FROM_ISR( ulReturn );  
}
#endif

/**
* @brief This function handles System tick timer.
*/
extern void xPortSysTickHandler(void);



ulong GetTick(VOID)
{
	return Ticks;
}




void SysTick_Handler(void)
{	
#if 0
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
      {
    #endif  /* INCLUDE_xTaskGetSchedulerState */  
 //       xPortSysTickHandler();
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      }
    #endif  /* INCLUDE_xTaskGetSchedulerState */
#endif

//    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);      // roger 20230728 add, observe Ticks Interval
//    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);      // roger 20230728 add, observe Ticks Interval
//    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);      // roger 20230728 add, observe Ticks Interval

    
	// printf("\r\n Enter SysTick_Handler \r\n");   // can enter, mark at 2023/7/25

	/* USER CODE BEGIN SysTick_IRQn 0 */

    #if 0
	/* USER CODE END SysTick_IRQn 0 */
	HAL_SYSTICK_IRQHandler();
	/* USER CODE BEGIN SysTick_IRQn 1 */
	TimingDelay_Decrement();
	/* USER CODE END SysTick_IRQn 1 */
    #endif

    #if 1	
	Time_Update();					  
    #endif

	Ticks ++;                       // current Ticks interval is 2ms	
	sys_event |= TIMER_EVENT;		// Roger 20150422, TIMER_EVENT = 0x02; 

	#if 0
	//if ((Ticks%50)==0)	// per 50ms check
	/* handle periodic timers for LwIP */
	LwIP_Periodic_Handle(LocalTime);
	#endif

	

}



///* 用于统计运行时间 */

volatile uint32_t CPU_RunTime = 0UL;
extern TIM_HandleTypeDef TIM_Base;

void BASIC_TIM_IRQHandler(void)
{
  //  HAL_TIM_IRQHandler(&TIM_Base);
}

#if 0
/**
  * @brief  定时器更新中断回调函数
  * @param  htim : TIM句柄
  * @retval 无
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6)
      xPortGetFreeHeapSize(); 
      CPU_RunTime++;
    if(htim->Instance == TIM3)
    {
        video_timeout = 1;
//			  LocalTime+=10;
//      LED1_TOGGLE;
    }
}
#endif


//void DMA1_Stream2_IRQHandler(void)
//{
////  uint32_t ulReturn;
////  /* 进入临界段，临界段可以嵌套 */
////  ulReturn = taskENTER_CRITICAL_FROM_ISR(); 
//  I2Sx_TX_DMA_STREAM_IRQFUN();
////  taskEXIT_CRITICAL_FROM_ISR( ulReturn );  
//}

/*
void RTC_Alarm_IRQHandler(void)
{
  HAL_RTC_AlarmIRQHandler(&Rtc_Handle);
}
*/

void DMA1_Stream2_IRQHandler(void)
{
//  SAI_TX_DMA_STREAM_IRQFUN();
}
void DMA1_Stream3_IRQHandler(void)
{
//  SAI_RX_DMA_STREAM_IRQFUN();
}

#if 0
void EXTI3_IRQHandler(void)
{
	if (__HAL_GPIO_EXTI_GET_IT(MPU_INT_GPIO_PIN) != RESET) //确保是否产生了EXTI Line中断
	{
		/* Handle new gyro*/
		gyro_data_ready_cb();

		__HAL_GPIO_EXTI_CLEAR_IT(MPU_INT_GPIO_PIN);     //清除中断标志位
	}
}
#endif

#if 0
/**
  * @brief  DMA中断服务函数
  * @param  None
  * @retval None
  */
void DMA2_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&DMA_Handle_dcmi);
  
}
#endif

#if 0
/**
  * @brief  DCMI中断服务函数
  * @param  None
  * @retval None
  */
void DCMI_IRQHandler(void)
{
  HAL_DCMI_IRQHandler(&DCMI_Handle);
  
}
#endif

//extern PCD_HandleTypeDef hpcd_USB_OTG_HS;
/**
  * @brief This function handles USB On The Go HS global interrupt.
  */
void OTG_HS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_HS_IRQn 0 */

  /* USER CODE END OTG_HS_IRQn 0 */
//  HAL_PCD_IRQHandler(&hpcd_USB_OTG_HS);
  /* USER CODE BEGIN OTG_HS_IRQn 1 */

  /* USER CODE END OTG_HS_IRQn 1 */
}
///**
//  * @brief  This function handles JPEG interrupt request.
//  * @param  None
//  * @retval None
//  */

//void JPEG_IRQHandler(void)
//{
////  HAL_JPEG_IRQHandler(&JPEG_Handle);
//}

///**
//  * @brief  This function handles MDMA interrupt request.
//  * @param  None
//  * @retval None
//  */

//void MDMA_IRQHandler()
//{
//  /* Check the interrupt and clear flag */
////  HAL_MDMA_IRQHandler(JPEG_Handle.hdmain);
////  HAL_MDMA_IRQHandler(JPEG_Handle.hdmaout);  
//}


// roger 20230911    add

extern uint8_t key2_pb;       		// roger 	20220911 add
extern uint8_t key1_pb;       		// roger 	20220915 add



#if 1

     
// #define WAKEUP_BUTTON_EXTI_IRQn         EXTI0_IRQn       // SW1

void EXTI0_IRQHandler(void)
{
	if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET) 
	{
		printf("\n\r Enter [EXTI0_IRQHandler] \n\r");
		
		//key1_pb=!key1_pb;
		key1_pb=1;

		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);     
	}
}

#endif


// #define TAMPER_BUTTON_EXTI_IRQn         EXTI15_10_IRQn     // SW3

void EXTI15_10_IRQHandler(void)
{
	if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != RESET) 
	{
		printf("\n\r Enter [EXTI15_10_IRQHandler] \n\r");
		
		//key2_pb=!key2_pb;
		key2_pb=1;

		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);     
	}
}





/*Disable interrupt*/
VOID Disable(void)
{
//printf("\n\r Enter Disable_IRQ \n");	 // roger 20230620 add

__disable_irq();

//printf("\n\r Leave Disable_IRQ \n");	 // roger 20230620 add

}


/*enable interrupt*/
VOID Enable(void)
{
//printf("\n\r Enter Enable_IRQ \n");	 	// roger 20230620 add

__enable_irq();

//printf("\n\r Leave Enable_IRQ \n");	 	// roger 20230620 add

}





/*!< USART Interrupts mask */
#define IT_MASK                   ((uint16_t)0x001F)


/**
  * @brief  Enables or disables the specified USART interrupts.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  USART_IT: specifies the USART interrupt sources to be enabled or disabled.
  *          This parameter can be one of the following values:
  *            @arg USART_IT_CTS:  CTS change interrupt
  *            @arg USART_IT_LBD:  LIN Break detection interrupt
  *            @arg USART_IT_TXE:  Transmit Data Register empty interrupt
  *            @arg USART_IT_TC:   Transmission complete interrupt
  *            @arg USART_IT_RXNE: Receive Data register not empty interrupt
  *            @arg USART_IT_IDLE: Idle line detection interrupt
  *            @arg USART_IT_PE:   Parity Error interrupt
  *            @arg USART_IT_ERR:  Error interrupt(Frame error, noise error, overrun error)
  * @param  NewState: new state of the specified USARTx interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState)
{
  uint32_t usartreg = 0x00, itpos = 0x00, itmask = 0x00;
  uint32_t usartxbase = 0x00;
  /* Check the parameters */
  assert_param(IS_USART_ALL_PERIPH(USARTx));
  assert_param(IS_USART_CONFIG_IT(USART_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* The CTS interrupt is not available for UART4 and UART5 */
  if (USART_IT == USART_IT_CTS)
  {
    assert_param(IS_USART_1236_PERIPH(USARTx));
  } 
    
  usartxbase = (uint32_t)USARTx;

  /* Get the USART register index */
  usartreg = (((uint8_t)USART_IT) >> 0x05);

  /* Get the interrupt position */
  itpos = USART_IT & IT_MASK;
  itmask = (((uint32_t)0x01) << itpos);
    
  if (usartreg == 0x01) /* The IT is in CR1 register */
  {
    usartxbase += 0x0C;
  }
  else if (usartreg == 0x02) /* The IT is in CR2 register */
  {
    usartxbase += 0x10;
  }
  else /* The IT is in CR3 register */
  {
    usartxbase += 0x14; 
  }
  if (NewState != DISABLE)
  {
    *(__IO uint32_t*)usartxbase  |= itmask;
  }
  else
  {
    *(__IO uint32_t*)usartxbase &= ~itmask;
  }
}




/**
  * @brief  Checks whether the specified USART interrupt has occurred or not.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  USART_IT: specifies the USART interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg USART_IT_CTS:  CTS change interrupt (not available for UART4 and UART5)
  *            @arg USART_IT_LBD:  LIN Break detection interrupt
  *            @arg USART_IT_TXE:  Transmit Data Register empty interrupt
  *            @arg USART_IT_TC:   Transmission complete interrupt
  *            @arg USART_IT_RXNE: Receive Data register not empty interrupt
  *            @arg USART_IT_IDLE: Idle line detection interrupt
  *            @arg USART_IT_ORE_RX : OverRun Error interrupt if the RXNEIE bit is set
  *            @arg USART_IT_ORE_ER : OverRun Error interrupt if the EIE bit is set  
  *            @arg USART_IT_NE:   Noise Error interrupt
  *            @arg USART_IT_FE:   Framing Error interrupt
  *            @arg USART_IT_PE:   Parity Error interrupt
  * @retval The new state of USART_IT (SET or RESET).
  */
ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT)
{
  uint32_t bitpos = 0x00, itmask = 0x00, usartreg = 0x00;
  ITStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_USART_ALL_PERIPH(USARTx));
  assert_param(IS_USART_GET_IT(USART_IT)); 

  /* The CTS interrupt is not available for UART4 and UART5 */ 
  if (USART_IT == USART_IT_CTS)
  {
    assert_param(IS_USART_1236_PERIPH(USARTx));
  } 
    
  /* Get the USART register index */
  usartreg = (((uint8_t)USART_IT) >> 0x05);
  /* Get the interrupt position */
  itmask = USART_IT & IT_MASK;
  itmask = (uint32_t)0x01 << itmask;
  
  if (usartreg == 0x01) /* The IT  is in CR1 register */
  {
    itmask &= USARTx->CR1;
  }
  else if (usartreg == 0x02) /* The IT  is in CR2 register */
  {
    itmask &= USARTx->CR2;
  }
  else /* The IT  is in CR3 register */
  {
    itmask &= USARTx->CR3;
  }
  
  bitpos = USART_IT >> 0x08;
  bitpos = (uint32_t)0x01 << bitpos;

  //bitpos &= USARTx->SR;
  bitpos &= USARTx->ISR;              // roger 20230617 add
	
  if ((itmask != (uint16_t)RESET)&&(bitpos != (uint16_t)RESET))
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  
  return bitstatus;  
}





/* USER CODE BEGIN 1 */
extern uint8_t Rxflag;
extern uint8_t ucTemp;




#if 0

void USART1_IRQHandler(void)		    // roger 20230727 comments, xmodem can't work may be caused by HAL_UART_Receive size setting ? Xmodem Fail
{
	if(__HAL_UART_GET_IT( &UartHandle, UART_IT_RXNE ) != RESET)
	{		
        Rxflag=1;	
		
        HAL_UART_Receive(&UartHandle, (uint8_t *)&ucTemp, 1, 1000);  
	 	// HAL_UART_Receive(&UartHandle, (uint8_t *)&ucTemp, 1000, 1000);  
        // HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)		

     	if(!full)
  		{			
			cmdbuf[put++] = ucTemp;
			if (put>=4096)
				put=0;
			if (put==get) 
				full=1;	
			#if 0
			if ((cmdbuf[put]==0x6B))   // jump out tftp server command is tftpe
			ubTftpExit=!ubTftpExit;
			#endif
	  	}  /*end of check full*/    
		
	}
    
    HAL_UART_IRQHandler(&UartHandle);	

  	sys_event |= UART_EVENT;	

}	/*end of uart isr*/

#endif



#if 1

void USART1_IRQHandler(void)				// roger 20150415 add, roger 20230727 add Xmodem OK, yes
{
	/*read characters from fifo*/
	INT8S rcvdata;
	//INT8S lsr = UART_LSR(EVAL_COM1);

	/////sys_event |= UART_EVENT;     // roger 20230727, force to give UART_EVENT = 0x01;

	while (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)     // RESET = 0, USART read data register not empty interruption
	{
  		rcvdata = USART_ReceiveData(USART1);
  		if(!full)
  		{			
			cmdbuf[put++] = rcvdata;
			if (put>=4096)
				put=0;
			if (put==get)
				full=1;	
		    #if 0
	          if ((cmdbuf[put]==0x6B))   // jump out tftp server command is tftpe
				ubTftpExit=!ubTftpExit;
			#endif
	  	}  /*end of check full*/

        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);      // roger 20230728 add, observe USART1_IRQHandler Interval
  		sys_event |= UART_EVENT;     // UART_EVENT = 0x01;

		// printf("\n\r [USART1_IRQHandler] sys_event = %d \n\r",sys_event);

  	}	/*end of while*/
}	/*end of uart isr*/

#endif


/******************************************************************************/
/* stm32h7xx Peripherals Interrupt Handlers */
/* Add here the Interrupt Handler for the used peripheral(s) (PPP), for the */
/* available peripheral interrupt handler's name please refer to the startup */
/* file (startup_stm32h7xx.s).  */
/******************************************************************************/

/**
  * @brief  This function handles USB-On-The-Go FS/HS global interrupt request.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
  HAL_PCD_IRQHandler(&hpcd);
}

