#ifndef __USARTx_H
#define	__USARTx_H

#include "stm32h7xx.h"
#include <stdio.h>

//引脚定义
/*******************************************************/

#define UARTx                             USART1
#define UARTx_CLK_ENABLE()                __USART1_CLK_ENABLE();

#define RCC_PERIPHCLK_UARTx               RCC_PERIPHCLK_USART1
#define RCC_UARTxCLKSOURCE_SYSCLK         RCC_USART1CLKSOURCE_SYSCLK


#define UARTx_RX_GPIO_PORT                GPIOA
#define UARTx_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define UARTx_RX_PIN                      GPIO_PIN_9
#define UARTx_RX_AF                       GPIO_AF7_USART1


#define UARTx_TX_GPIO_PORT                GPIOA
#define UARTx_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define UARTx_TX_PIN                      GPIO_PIN_10
#define UARTx_TX_AF                       GPIO_AF7_USART1


#define UARTx_IRQHandler                  USART1_IRQHandler
#define UARTx_IRQ                 		  USART1_IRQn


/************************************************************/


//串口波特率
#define UARTx_BAUDRATE                    115200

void UARTx_Config(void);
//int fputc(int ch, FILE *f);
extern UART_HandleTypeDef UartHandle;
#endif /* __USART1_H */



void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);

uint16_t USART_ReceiveData(USART_TypeDef* USARTx);



/**
  * @brief  Transmits single data through the USARTx peripheral.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  Data: the data to transmit.
  * @retval None
  */

#define USART_IT_ORE_RX                      ((uint16_t)0x0325) /* In case interrupt is generated if the RXNEIE bit is set */

#define USART_IT_LBD                         ((uint16_t)0x0846)
#define USART_IT_CTS                         ((uint16_t)0x096A)
//#define USART_IT_ERR                         ((uint16_t)0x0060)
#define USART_IT_ORE_ER                      ((uint16_t)0x0360) /* In case interrupt is generated if the EIE bit is set */



#define IS_USART_ALL_PERIPH(PERIPH) (((PERIPH) == USART1) || \
										((PERIPH) == USART2) || \
										((PERIPH) == USART3) || \
										((PERIPH) == UART4)  || \
										((PERIPH) == UART5)  || \
										((PERIPH) == USART6) || \
										((PERIPH) == UART7)  || \
										((PERIPH) == UART8))

#define IS_USART_1236_PERIPH(PERIPH) (((PERIPH) == USART1) || \
									   ((PERIPH) == USART2) || \
									   ((PERIPH) == USART3) || \
									   ((PERIPH) == USART6))


#define IS_USART_CONFIG_IT(IT) (((IT) == USART_IT_PE) || ((IT) == USART_IT_TXE) || \
															 ((IT) == USART_IT_TC) || ((IT) == USART_IT_RXNE) || \
															 ((IT) == USART_IT_IDLE) || ((IT) == USART_IT_LBD) || \
															 ((IT) == USART_IT_CTS) || ((IT) == USART_IT_ERR))


#define IS_USART_GET_IT(IT) (((IT) == USART_IT_PE) || ((IT) == USART_IT_TXE) || \
                             ((IT) == USART_IT_TC) || ((IT) == USART_IT_RXNE) || \
                             ((IT) == USART_IT_IDLE) || ((IT) == USART_IT_LBD) || \
                             ((IT) == USART_IT_CTS) || ((IT) == USART_IT_ORE) || \
                             ((IT) == USART_IT_ORE_RX) || ((IT) == USART_IT_ORE_ER) || \
                             ((IT) == USART_IT_NE) || ((IT) == USART_IT_FE))

#define IS_USART_CLEAR_IT(IT) (((IT) == USART_IT_TC) || ((IT) == USART_IT_RXNE) || \
                               ((IT) == USART_IT_LBD) || ((IT) == USART_IT_CTS))


#define IS_USART_DATA(DATA) ((DATA) <= 0x1FF)




#if 0

/** @defgroup USART_Word_Length 
  * @{
  */ 
  
#define USART_WordLength_8b                  ((uint16_t)0x0000)
#define USART_WordLength_9b                  ((uint16_t)0x1000)



/** @defgroup USART_Stop_Bits 
  * @{
  */ 
  
#define USART_StopBits_1                     ((uint16_t)0x0000)
#define USART_StopBits_0_5                   ((uint16_t)0x1000)
#define USART_StopBits_2                     ((uint16_t)0x2000)
#define USART_StopBits_1_5                   ((uint16_t)0x3000)
#define IS_USART_STOPBITS(STOPBITS) (((STOPBITS) == USART_StopBits_1) || \
                                     ((STOPBITS) == USART_StopBits_0_5) || \
                                     ((STOPBITS) == USART_StopBits_2) || \
                                     ((STOPBITS) == USART_StopBits_1_5))



/** @defgroup USART_Parity 
  * @{
  */ 
  
#define USART_Parity_No                      ((uint16_t)0x0000)
#define USART_Parity_Even                    ((uint16_t)0x0400)
#define USART_Parity_Odd                     ((uint16_t)0x0600) 
#define IS_USART_PARITY(PARITY) (((PARITY) == USART_Parity_No) || \
                                 ((PARITY) == USART_Parity_Even) || \
                                 ((PARITY) == USART_Parity_Odd))



/** @defgroup USART_Hardware_Flow_Control 
  * @{
  */ 
#define USART_HardwareFlowControl_None       ((uint16_t)0x0000)
#define USART_HardwareFlowControl_RTS        ((uint16_t)0x0100)
#define USART_HardwareFlowControl_CTS        ((uint16_t)0x0200)
#define USART_HardwareFlowControl_RTS_CTS    ((uint16_t)0x0300)
#define IS_USART_HARDWARE_FLOW_CONTROL(CONTROL)\
                              (((CONTROL) == USART_HardwareFlowControl_None) || \
                               ((CONTROL) == USART_HardwareFlowControl_RTS) || \
                               ((CONTROL) == USART_HardwareFlowControl_CTS) || \
                               ((CONTROL) == USART_HardwareFlowControl_RTS_CTS))


/** @defgroup USART_Mode 
  * @{
  */ 
  
#define USART_Mode_Rx                        ((uint16_t)0x0004)
#define USART_Mode_Tx                        ((uint16_t)0x0008)
#define IS_USART_MODE(MODE) ((((MODE) & (uint16_t)0xFFF3) == 0x00) && ((MODE) != (uint16_t)0x00))


#endif



