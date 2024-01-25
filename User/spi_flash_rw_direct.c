/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   QSPI FLASH Basic RW Example
  ******************************************************************
  * @attention
  *
  * HW Platform: Fire Challenger V2
  * Forum: http://www.firebbs.cn
  * Taobao:https://fire-stm32.taobao.com
  *
  ******************************************************************
  */  


#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h"
//#include "./usart/bsp_debug_usart.h"
#include "./flash/bsp_qspi_flash.h" 
#include "./delay/core_delay.h"
#include <string.h>
#include <stdio.h>
//#include "bsp_qspi_flash_user.h"


typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
/* Get the length of the buffer */
#define TxBufferSize1_dir   (countof(Tx_Buffer_dir) - 1)
#define RxBufferSize1_dir   (countof(Tx_Buffer_dir) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize_dir     (countof(Tx_Buffer_dir)-1)


#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize     (countof(Tx_Buffer)-1)


#define  FLASH_WriteAddress     0
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress


uint8_t Tx_Buffer_dir[] = "http://firestm32.taobao.com Thank for you choosing Fire stm32 EVB\r\nhttp://firestm32.taobao.com Thank for you choosing Fire stm32 EVB\r\nhttp://firestm32.taobao.com Thank for you choosing Fire stm32 EVB\r\nhttp://firestm32.taobao.com Thank for you choosing Fire stm32 EVB\r\nhttp://firestm32.taobao.com Thank for you choosing Fire stm32 EVB";
uint8_t Tx_Buffer2_dir[] ={0XCC,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A, 0x28, 0x0A,0XFF,0XAA,0XBB,0XCC,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A, 0x28, 0x0A};
uint8_t Rx_Buffer_dir[BufferSize_dir];


uint8_t Tx_Buffer[] = "http://firestm32.taobao.com Thank for you choosing Fire stm32 EVB\r\nhttp://firestm32.taobao.com Thank for you choosing Fire stm32 EVB\r\nhttp://firestm32.taobao.com Thank for you choosing Fire stm32 EVB\r\nhttp://firestm32.taobao.com Thank for you choosing Fire stm32 EVB\r\nhttp://firestm32.taobao.com Thank for you choosing Fire stm32 EVB";
uint8_t Tx_Buffer2[] ={0XCC,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A, 0x28, 0x0A,0XFF,0XAA,0XBB,0XCC,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A, 0x28, 0x0A};
uint8_t Rx_Buffer[BufferSize];


// Read ID storage location
__IO uint32_t DeviceID_dir = 0;
__IO uint32_t FlashID_dir = 0;
__IO TestStatus TransferStatus1_dir = FAILED;


__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;
__IO TestStatus TransferStatus1 = FAILED;



// function prototype declaration
void Delay_dir(__IO uint32_t nCount);
TestStatus Buffercmp_dir(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);

TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);


INT32S Do_Spi_FLASH_RW_Direct (INT32S argc, INT8S *argv[]);
INT32S Do_Qspimemmap (INT32S argc, INT8S *argv[]);



uint8_t state_dir = QSPI_ERROR;


extern QSPI_HandleTypeDef QSPIHandle;




///////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @brief SPI Flash RW Test by Direct Access
  * @retval
  *  None
  */



#if 1
  
INT32S Do_Spi_FLASH_RW_Direct (INT32S argc, INT8S *argv[])
{
  uint32_t addr = FLASH_WriteAddress ;
  __IO uint8_t* qspi_addr = (__IO uint8_t*)(0x90000000);
	int state = QSPI_ERROR;

	#if 0
	/* enable instruction cache */
	SCB_EnableICache();
	/* enable data storage */
	SCB_EnableDCache();
	/* The system clock is initialized to 480MHz */
	SystemClock_Config();
	#endif    

	
	/* LED GPIO initialization */
	LED_GPIO_Config();	
	LED_BLUE;

	/* Config USART1 as: 115200 8-N-1*/
	//DEBUG_USART_Config();
  
	printf("\r\n [01] This is a 32M SPI flash (W25Q256) memory mapping mode RW experiment (QSPI driver)\r\n");
	
	/* 32 spi flash W25Q256 initialization */
	QSPI_FLASH_Init(); // roger 20230710, move to BSP Init.
	if (1)
	{	
		printf("\r\n [02] QSPI FLASH W25Q256 is detected !\r\n");
		printf("\r\n [03] The content of 0x%x~0x%x is being erased by the chip !\r\n", addr, addr+W25Q256JV_PAGE_SIZE);   

    state = BSP_QSPI_Erase_Block(addr);
    if(state == QSPI_OK)
      printf("\r\n [04] erase success !\r\n");
    else
    {
      LED_RED;
      printf("\r\n [04] erase fail !\r\n");
      while(1);
      
    }
    printf("\r\n [05] Writing data to chip address 0x%x, the size is %d !\r\n", addr, BufferSize);
		/* Write the data in the send buffer to flash */
		BSP_QSPI_Write(Tx_Buffer, addr, BufferSize);
    printf("\r\n [06] write succeess !\r\n");  
		
    /* QSPI memory reset */
    if (QSPI_ResetMemory() != QSPI_OK)
    {
      return QSPI_ERROR;
    }    
    if( QSPI_EnableMemoryMappedMode() != QSPI_OK )
    {
      return QSPI_ERROR;
    }
    printf("\r\n [07] Use the memcpy function to read the contents of QPSI---\r\n");
    memcpy(Rx_Buffer,(uint8_t *)qspi_addr,BufferSize);  
		/* Check if the written data is equal to the read data */
		TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);

 		#if 1
		 printf("\n\r [A] Tx_Buffer[]: \n\r");		 
		 for (int i = 0; i < BufferSize; i++) 
		 {
		 printf("%c", Tx_Buffer[i]);	 
		 }
		 
		 printf("\n\r [B] Rx_Buffer[]: \n\r");			  
		 for (int j = 0; j < BufferSize; j++) 
		 {
		 printf("%c", Rx_Buffer[j]);	 
		 }
		#endif
		
		
		if( PASSED == TransferStatus1 )
		{    
			LED_GREEN;
			printf("\r\n [08] RW (memcpy) test succeess !\n\r");
		}
		else
		{        
			LED_RED;
			printf("\r\n [08] RW (memcpy) test fail !");
		}
		
    printf("\r\n [09] Use a pointer to read QSPI content---");
		
    memset(Rx_Buffer,0,BufferSize);
		
    for(int i = 0; i < BufferSize; i++)
    { 
      Rx_Buffer[i] = *qspi_addr;
      qspi_addr++;
    }
		
		/* Check if the written data is equal to the read data */
		TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);
		
		if( PASSED == TransferStatus1 )
		{    
			LED_GREEN;
			printf("\r\n [10] RW (pointer operation) test success !\n\r");
		}
		else
		{        
			LED_RED;
			printf("\r\n [10] RW (pointer operation) test fail !\n\r");
		}    
    
	}// if (FlashID == sFLASH_ID)

	return 1;
	//while(1);  
}


#endif








/*
 * Function: Buffercmp
 * Description: Compare the data in two buffers for equality
 * Input: -pBuffer1     src buffer pointer
 *        -pBuffer2     dst buffer pointer
 *        -BufferLength buffer length 
 * Output: None
 * Return: PASSED pBuffer1 is equal to pBuffer2
 *         FAILED pBuffer1 is different from pBuffer2
 */
TestStatus Buffercmp_dir(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}



TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}




void Delay_dir(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

/**
  * @brief  System Clock Config
  *         system Clock Config are as follow: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 480000000 (CPU Clock)
	*            HCLK(Hz)             = 240000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  120MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  120MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  120MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  120MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 192
	*            PLL_P                = 2
	*            PLL_Q                = 4
	*            PLL_R                = 2
	*            VDD(V)               = 3.3
	*            Flash Latency(WS)    = 4
  * @param  None
  * @retval None
  */


#if 0


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Enable power configuration updates
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Config the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Initialize CPU, AHB and APB bus clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
		while(1);
  }
  /** Initialize CPU, AHB and APB bus clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
		while(1);
  }
}


#endif



INT32S Do_Qspimemmap(INT32S argc, INT8S *argv[])
{
  //	uint32_t addr = FLASH_WriteAddress ;
  //	__IO uint8_t* qspi_addr = (__IO uint8_t*)(0x90000000);
  //	int state = QSPI_ERROR;


	/* LED GPIO initialization */
	LED_GPIO_Config();	
	LED_BLUE;

	/* Config USART1 as: 115200 8-N-1*/
	//DEBUG_USART_Config();
  
	printf("\r\n [01] This is a 32M SPI flash (W25Q256) memory mapping mode RW experiment (QSPI driver)\r\n");
	
	/* 32 spi flash W25Q256 initialization */
	QSPI_FLASH_Init(); // roger 20230710, move to BSP Init.
	
//		printf("\r\n [02] QSPI FLASH W25Q256 is detected !\r\n");
//		printf("\r\n [03] The content of 0x%x~0x%x is being erased by the chip !\r\n", addr, addr+W25Q256JV_PAGE_SIZE);   
	
	/* QSPI memory reset */
	if (QSPI_ResetMemory() != QSPI_OK)
	{
		return QSPI_ERROR;
	}	 
	
	if( QSPI_EnableMemoryMappedMode() != QSPI_OK )
	{
		return QSPI_ERROR;
	}

    return QSPI_OK;

}



INT32S Do_QSPI_ResetMemory(INT32S argc, INT8S *argv[])
#if 0
{
  //	uint32_t addr = FLASH_WriteAddress ;
  //	__IO uint8_t* qspi_addr = (__IO uint8_t*)(0x90000000);
	int state = QSPI_ERROR;


	/* LED GPIO initialization */
	LED_GPIO_Config();	
	LED_BLUE;

	/* Config USART1 as: 115200 8-N-1*/
	//DEBUG_USART_Config();
  
	printf("\r\n [01] This is a 32M SPI flash (W25Q256) memory mapping mode RW experiment (QSPI driver)\r\n");
	
	/* 32 spi flash W25Q256 initialization */
	QSPI_FLASH_Init(); // roger 20230710, move to BSP Init.
	if (1)
	{	
//		printf("\r\n [02] QSPI FLASH W25Q256 is detected !\r\n");
//		printf("\r\n [03] The content of 0x%x~0x%x is being erased by the chip !\r\n", addr, addr+W25Q256JV_PAGE_SIZE);   
	
		/* QSPI memory reset */
		if (QSPI_ResetMemory() != QSPI_OK)
		{
		  printf("\r\n [02] QSPI_ResetMemory Fail \r\n");		  
		  return QSPI_ERROR;
		}
		else if (QSPI_ResetMemory() == QSPI_OK)
		{
          printf("\r\n [02] QSPI_ResetMemory Success \r\n"); 		
          return QSPI_OK;
		}
    }

}
#endif

#if 1
{

	/* LED GPIO initialization */
	LED_GPIO_Config();	
	LED_BLUE;

	/* Config USART1 as: 115200 8-N-1*/
	//DEBUG_USART_Config();
  
	printf("\r\n [01] This is a 32M SPI flash (W25Q256) memory mapping mode RW experiment (QSPI driver)\r\n");
	
	/* 32 spi flash W25Q256 initialization */
	QSPI_FLASH_Init(); // roger 20230710, move to BSP Init.

    if (HAL_QSPI_Abort(&QSPIHandle)==HAL_OK)
	{
	  printf("\r\n [02] QSPI_ResetMemory Success \r\n");		
	  return QSPI_OK;
	}
    else
	{
	  printf("\r\n [02] QSPI_ResetMemory Fail \r\n");		  
	  return QSPI_ERROR;	  
	}

}
#endif



/****************************END OF FILE***************************/


