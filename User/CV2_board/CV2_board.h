/**
  ******************************************************************************
  * @file    CV2_board.h

  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32H743I_EVAL_H
#define __STM32H743I_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32H743I_EVAL
  * @{
  */

/** @defgroup STM32H743I_EVAL_LOW_LEVEL LOW LEVEL
  * @{
  */

/** @defgroup STM32H743I_EVAL_LOW_LEVEL_Exported_Types LOW LEVEL Exported Types
  * @{
  */


typedef enum
{
  BUTTON_WAKEUP = 0,
  BUTTON_TAMPER = 1,
  BUTTON_KEY = 2
}Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
}ButtonMode_TypeDef;


/**
  * @}
  */

/** @defgroup STM32H743I_EVAL_LOW_LEVEL_Exported_Constants LOW LEVEL Exported Constants
  * @{
  */

/**
  * @brief  Define for STM32H743I_EVAL board
  */

/** @addtogroup STM32H743I_EVAL_LOW_LEVEL_LED
  * @{
  */


/**
  * @}
  */

/**
  * @brief MFX_IRQOUt pin
  */


/** @addtogroup STM32H743I_EVAL_LOW_LEVEL_BUTTON
  * @{
  */
/* Joystick pins are connected to IO Expander (accessible through I2C1 interface) */
#define BUTTONn                             ((uint8_t)3)

/**
  * @brief Wakeup push-button
  */
#define WAKEUP_BUTTON_PIN                   GPIO_PIN_0
#define WAKEUP_BUTTON_GPIO_PORT             GPIOA
#define WAKEUP_BUTTON_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define WAKEUP_BUTTON_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define WAKEUP_BUTTON_EXTI_IRQn             EXTI0_IRQn

/**
  * @brief Tamper push-button
  */
#define TAMPER_BUTTON_PIN                    GPIO_PIN_13
#define TAMPER_BUTTON_GPIO_PORT              GPIOC
#define TAMPER_BUTTON_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define TAMPER_BUTTON_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOC_CLK_DISABLE()
#define TAMPER_BUTTON_EXTI_IRQn              EXTI15_10_IRQn

/**
  * @brief Key push-button
  */
#define KEY_BUTTON_PIN                       GPIO_PIN_13
#define KEY_BUTTON_GPIO_PORT                 GPIOC
#define KEY_BUTTON_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define KEY_BUTTON_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOC_CLK_DISABLE()
#define KEY_BUTTON_EXTI_IRQn                 EXTI15_10_IRQn

#define BUTTONx_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == 0) WAKEUP_BUTTON_GPIO_CLK_ENABLE(); else\
                                                   if((__INDEX__) == 1) TAMPER_BUTTON_GPIO_CLK_ENABLE(); else\
                                                KEY_BUTTON_GPIO_CLK_ENABLE(); } while(0)

#define BUTTONx_GPIO_CLK_DISABLE(__INDEX__)    (((__INDEX__) == 0) ? WAKEUP_BUTTON_GPIO_CLK_DISABLE() :\
                                                ((__INDEX__) == 1) ? TAMPER_BUTTON_GPIO_CLK_DISABLE() : KEY_BUTTON_GPIO_CLK_DISABLE())
/**
  * @}
  */

/** @addtogroup STM32H743I_EVAL_LOW_LEVEL_COM
  * @{
  */
#define COMn                             ((uint8_t)1)

/**
 * @brief Definition for COM port1, connected to USART1
 */
#define EVAL_COM1                          USART1
#define EVAL_COM1_CLK_ENABLE()             __HAL_RCC_USART1_CLK_ENABLE()
#define EVAL_COM1_CLK_DISABLE()            __HAL_RCC_USART1_CLK_DISABLE()

#define EVAL_COM1_TX_PIN                   GPIO_PIN_14
#define EVAL_COM1_TX_GPIO_PORT             GPIOB
#define EVAL_COM1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define EVAL_COM1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOB_CLK_DISABLE()
#define EVAL_COM1_TX_AF                    GPIO_AF4_USART1

#define EVAL_COM1_RX_PIN                   GPIO_PIN_15
#define EVAL_COM1_RX_GPIO_PORT             GPIOB
#define EVAL_COM1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define EVAL_COM1_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOB_CLK_DISABLE()
#define EVAL_COM1_RX_AF                    GPIO_AF4_USART1

#define EVAL_COM1_IRQn                     USART1_IRQn

#define EVAL_COMx_CLK_ENABLE(__INDEX__)            do { if((__INDEX__) == COM1) EVAL_COM1_CLK_ENABLE(); } while(0)
#define EVAL_COMx_CLK_DISABLE(__INDEX__)           (((__INDEX__) == 0) ? EVAL_COM1_CLK_DISABLE() : 0)

#define EVAL_COMx_TX_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == COM1) EVAL_COM1_TX_GPIO_CLK_ENABLE(); } while(0)
#define EVAL_COMx_TX_GPIO_CLK_DISABLE(__INDEX__)   (((__INDEX__) == 0) ? EVAL_COM1_TX_GPIO_CLK_DISABLE() : 0)

#define EVAL_COMx_RX_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == COM1) EVAL_COM1_RX_GPIO_CLK_ENABLE(); } while(0)
#define EVAL_COMx_RX_GPIO_CLK_DISABLE(__INDEX__)   (((__INDEX__) == 0) ? EVAL_COM1_RX_GPIO_CLK_DISABLE() : 0)

/**
 * @brief Definition for Potentiometer, connected to ADC1
 */
#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC12_CLK_ENABLE()
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()

#define ADCx_FORCE_RESET()              __HAL_RCC_ADC12_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC12_RELEASE_RESET()

/* Definition for ADCx Channel Pin */
#define ADCx_CHANNEL_PIN                GPIO_PIN_0
#define ADCx_CHANNEL_GPIO_PORT          GPIOA

/* Definition for ADCx's Channel */
#define ADCx_CHANNEL                    ADC_CHANNEL_0
#define ADCx_POLL_TIMEOUT               10



/* Exported constant IO ------------------------------------------------------*/

/*  The MFX_I2C_ADDR input pin selects the MFX I2C device address
        MFX_I2C_ADDR input pin     MFX I2C device address
            0                           b: 1000 010x    (0x84)
            1                           b: 1000 011x    (0x86)
   This input is sampled during the MFX firmware startup.  */
#define IO_I2C_ADDRESS                   ((uint16_t)0x84)  /*mfx MFX_I2C_ADDR 0*/
#define IO_I2C_ADDRESS_2                 ((uint16_t)0x86)  /*mfx MFX_I2C_ADDR 1*/
#define TS_I2C_ADDRESS                   ((uint16_t)0x82)  /*stmpe811 used on MB1046 board */
#define TS3510_I2C_ADDRESS               ((uint16_t)0x80)
#define EXC7200_I2C_ADDRESS              ((uint16_t)0x08)
#define AUDIO_I2C_ADDRESS                ((uint16_t)0x34)
#define EEPROM_I2C_ADDRESS_A01           ((uint16_t)0xA0)
#define EEPROM_I2C_ADDRESS_A02           ((uint16_t)0xA6)
/* I2C clock speed configuration (in Hz)
   WARNING:
   Make sure that this define is not already declared in other files (ie.
   stm32h743i_eval.h file). It can be used in parallel by other modules. */
#ifndef I2C_SPEED
 #define I2C_SPEED                        ((uint32_t)100000)
#endif /* I2C_SPEED */

/* User can use this section to tailor I2Cx/I2Cx instance used and associated
   resources */
/* Definition for I2Cx clock resources */
#define EVAL_I2Cx                             I2C1
#define EVAL_I2Cx_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define EVAL_DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define EVAL_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

#define EVAL_I2Cx_FORCE_RESET()               __HAL_RCC_I2C1_FORCE_RESET()
#define EVAL_I2Cx_RELEASE_RESET()             __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define EVAL_I2Cx_SCL_PIN                     GPIO_PIN_6
#define EVAL_I2Cx_SCL_SDA_GPIO_PORT           GPIOB
#define EVAL_I2Cx_SCL_SDA_AF                  GPIO_AF4_I2C1
#define EVAL_I2Cx_SDA_PIN                     GPIO_PIN_7

/* I2C interrupt requests */
#define EVAL_I2Cx_EV_IRQn                     I2C1_EV_IRQn
#define EVAL_I2Cx_ER_IRQn                     I2C1_ER_IRQn

/* I2C TIMING Register define when I2C clock source is SYSCLK */
/* I2C TIMING is calculated from APB1 source clock = 50 MHz */
/* 0x40912732 takes in account the big rising and aims a clock of 100khz */
/* this value might be adapted when next Rev Birdie board is available */
#ifndef EVAL_I2Cx_TIMING
#define EVAL_I2Cx_TIMING                      ((uint32_t)0x40912732)
#endif /* EVAL_I2Cx_TIMING */



/**
  * @}
  */


/** @addtogroup STM32H743I_EVAL_LCD_TIM
  * @{
  */
/**
 * @brief Definition for LCD Timer used to control the Brightnes
 */
#define LCD_TIMx                           TIM3
#define LCD_TIMx_CLK_ENABLE()              __HAL_RCC_TIM3_CLK_ENABLE()
#define LCD_TIMx_CLK_DISABLE()             __HAL_RCC_TIM3_CLK_DISABLE()
#define LCD_TIMx_CHANNEL                   TIM_CHANNEL_1
#define LCD_TIMx_CHANNEL_AF                GPIO_AF2_TIM3
#define LCD_TIMX_PERIOD_VALUE              ((uint32_t)50000) /* Period Value    */
#define LCD_TIMX_PRESCALER_VALUE           ((uint32_t)4)     /* Prescaler Value */

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup STM32H743I_EVAL_LOW_LEVEL_Exported_Macros LOW LEVEL Exported Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32H743I_EVAL_LOW_LEVEL_Exported_Functions LOW LEVEL Exported Functions
  * @{
  */

void             BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
void             BSP_PB_DeInit(Button_TypeDef Button);
uint32_t         BSP_PB_GetState(Button_TypeDef Button);



/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32H743I_EVAL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
