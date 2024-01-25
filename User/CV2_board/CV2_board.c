/*
  ******************************************************************************
  * @file    CV2_board.c

  ******************************************************************************
*/


/* Includes ------------------------------------------------------------------*/
#include "CV2_board.h"


GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {WAKEUP_BUTTON_GPIO_PORT,
                                      TAMPER_BUTTON_GPIO_PORT,
                                      KEY_BUTTON_GPIO_PORT};

const uint16_t BUTTON_PIN[BUTTONn] = {WAKEUP_BUTTON_PIN,   // GPIOA-0, Wakeup, SW1, KEY1
                                      TAMPER_BUTTON_PIN,   // GPIOC-13, Tamper, SW3, KEY2
                                      KEY_BUTTON_PIN};     // 



const uint16_t BUTTON_IRQn[BUTTONn] = {WAKEUP_BUTTON_EXTI_IRQn,
                                       TAMPER_BUTTON_EXTI_IRQn,
                                       KEY_BUTTON_EXTI_IRQn};




void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable the BUTTON clock */
  BUTTONx_GPIO_CLK_ENABLE(Button);

  if(ButtonMode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    GPIO_InitStruct.Pin = BUTTON_PIN[Button];
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStruct);
  }

  if(ButtonMode == BUTTON_MODE_EXTI)
  {
    /* Configure Button pin as input with External interrupt */
    GPIO_InitStruct.Pin = BUTTON_PIN[Button];
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    if(Button != BUTTON_WAKEUP)
    {
      GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    }
    else
    {
      GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    }

    HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStruct);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((IRQn_Type)(BUTTON_IRQn[Button]), 0x0F, 0x00);
    HAL_NVIC_EnableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  }
}

/**
  * @brief  Push Button DeInit.
  * @param  Button: Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  *            @arg  BUTTON_TAMPER: Tamper Push Button
  *            @arg  BUTTON_KEY: Key Push Button
  * @note On STM32H743I-EVAL evaluation board, the three buttons (Wakeup, Tamper
  *       and key buttons) are mapped on the same push button named "Wakeup/Tamper"
  *       on the board serigraphy.
  * @note PB DeInit does not disable the GPIO clock
  * @retval None
  */
void BSP_PB_DeInit(Button_TypeDef Button)
{
    GPIO_InitTypeDef gpio_init_structure;

    gpio_init_structure.Pin = BUTTON_PIN[Button];
    HAL_NVIC_DisableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
    HAL_GPIO_DeInit(BUTTON_PORT[Button], gpio_init_structure.Pin);
}


/**
  * @brief  Returns the selected button state.
  * @param  Button: Button to be checked
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  *            @arg  BUTTON_TAMPER: Tamper Push Button
  *            @arg  BUTTON_KEY: Key Push Button
  * @note On STM32H743I-EVAL evaluation board, the three buttons (Wakeup, Tamper
  *       and key buttons) are mapped on the same push button named "Wakeup/Tamper"
  *       on the board serigraphy.
  * @retval The Button GPIO pin value
  */
uint32_t BSP_PB_GetState(Button_TypeDef Button)
{
  return HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}







/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
