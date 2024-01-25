/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   Use USART1 to redirect the printf function of the C library to the usart port
  ******************************************************************************
  * @attention
  *
  * HW Platform: Fire Challenger V2
  * Forum: http://www.firebbs.cn
  * Taobao:https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_usart.h"


UART_HandleTypeDef UartHandle;
 /**
  * @brief  USARTx GPIO config,115200 8-N-1
  * @param  none
  * @retval none
  */  
void UARTx_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
			
	UARTx_RX_GPIO_CLK_ENABLE();
	UARTx_TX_GPIO_CLK_ENABLE();
	
	/* Config USART 1 Clock Source */
	RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
	RCC_PeriphClkInit.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
	/* Enable UART Clock */
	UARTx_CLK_ENABLE();

	/**USART1 GPIO Configuration    (use uart<-> usb IC)
    PA9     ---> USART1_TX
    PA10    ---> USART1_RX 
	*/
	/* Config the Tx pin as an alternate function */
	GPIO_InitStruct.Pin = UARTx_TX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = UARTx_TX_AF;
	HAL_GPIO_Init(UARTx_TX_GPIO_PORT, &GPIO_InitStruct);
	
	/* Config the Rx pin as an alternate function */
	GPIO_InitStruct.Pin = UARTx_RX_PIN;
	GPIO_InitStruct.Alternate = UARTx_RX_AF;
	HAL_GPIO_Init(UARTx_RX_GPIO_PORT, &GPIO_InitStruct); 
	
	/* Config USARTx Mode */
	UartHandle.Instance = UARTx;
	UartHandle.Init.BaudRate = 115200;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits = UART_STOPBITS_1;
	UartHandle.Init.Parity = UART_PARITY_NONE;
	UartHandle.Init.Mode = UART_MODE_TX_RX;
	UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;                  // roger 20230728 add for debug xmodem
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;			  // roger 20230728 add for debug xmodem	
    UartHandle.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;   // roger 20230728 add for debug xmodem
    UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT; // roger 20230728 add for debug xmodem	 
	HAL_UART_Init(&UartHandle);

    #if 1         // must activate, otherwise can't perform scanf function
	// UART interrupt initialization
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
	// Configure UART receive interrupt
    __HAL_UART_ENABLE_IT(&UartHandle,UART_IT_RXNE);  
    #endif	

}



// Redirect the c library function printf to the serial port USARTx, and the printf function can be used after redirection
int fputc(int ch, FILE *f)
{
    /* Send a byte data to the serial port USARTx */
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);
	return (ch);
}

// Redirect the c library function scanf to the serial port USARTx, rewrite and use scanf, getchar and other functions
int fgetc(FILE *f)
{	
	int ch;
	/* Wait for the serial port to input data */
	while(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_RXNE) == RESET);
	__HAL_UART_CLEAR_OREFLAG(&UartHandle);
	HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);
	return (ch);
}






void USART_SendData(USART_TypeDef* USARTx, uint16_t Data)
{
  /* Check the parameters */
  assert_param(IS_USART_ALL_PERIPH(USARTx));
  assert_param(IS_USART_DATA(Data)); 

  while(USART_IT_TXE==0);  
	
  /* Transmit Data */
  USARTx->TDR = (Data & (uint16_t)0x01FF);
  //USARTx->DR = (Data & (uint16_t)0x01FF);
  //USARTx->DR = (Data & (uint16_t)0x00FF);    // roger 20150421 add
}




/**
  * @brief  Returns the most recent received data by the USARTx peripheral.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @retval The received data.
  */
uint16_t USART_ReceiveData(USART_TypeDef* USARTx)
{
  /* Check the parameters */
  assert_param(IS_USART_ALL_PERIPH(USARTx));
  
  /* Receive Data */
  //return (uint16_t)(USARTx->DR & (uint16_t)0x01FF);   // roger 20150506 mark
  //return (uint16_t)(USARTx->DR & (uint16_t)0x00FF);   // roger 20150506 add
  return (uint16_t)(USARTx->RDR & (uint16_t)0x00FF);   // roger 20230617 add
}




/*********************************************END OF FILE**********************/
