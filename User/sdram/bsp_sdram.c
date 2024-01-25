/**
  ******************************************************************************
  * @file    bsp_sdram.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   sdram应用函数接口
  ******************************************************************************
  * @attention
  *
  * HW Platform: Fire Challenger STM32H750 EVB V2
  * Forum: http://www.firebbs.cn
  * Taobao:https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./sdram/bsp_sdram.h"  


static FMC_SDRAM_CommandTypeDef Command;
SDRAM_HandleTypeDef hsdram1;
#define sdramHandle hsdram1
/**
  * @brief  delay for a while
  * @param  length of delay
  * @retval None
  */
static void SDRAM_delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = (100000 * nCount); index != 0; index--)
  {
  }
}

/**
  * @brief  Initialize the IO to control SDRAM
  * @param  NA
  * @retval NA
  */
static void SDRAM_GPIO_Config(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* Enable SDRAM related IO clock */
	/* Address Signal */
	FMC_A0_GPIO_CLK();FMC_A1_GPIO_CLK(); FMC_A2_GPIO_CLK();
	FMC_A3_GPIO_CLK();FMC_A4_GPIO_CLK(); FMC_A5_GPIO_CLK();
	FMC_A6_GPIO_CLK();FMC_A7_GPIO_CLK(); FMC_A8_GPIO_CLK();
	FMC_A9_GPIO_CLK();FMC_A10_GPIO_CLK();FMC_A11_GPIO_CLK();
	FMC_A12_GPIO_CLK();
	/* Data Signal */
	FMC_D0_GPIO_CLK(); FMC_D1_GPIO_CLK() ; FMC_D2_GPIO_CLK() ; 
	FMC_D3_GPIO_CLK(); FMC_D4_GPIO_CLK() ; FMC_D5_GPIO_CLK() ;
	FMC_D6_GPIO_CLK(); FMC_D7_GPIO_CLK() ; FMC_D8_GPIO_CLK() ;
	FMC_D9_GPIO_CLK(); FMC_D10_GPIO_CLK(); FMC_D11_GPIO_CLK();
	FMC_D12_GPIO_CLK();FMC_D13_GPIO_CLK(); FMC_D14_GPIO_CLK();
	FMC_D15_GPIO_CLK();FMC_D16_GPIO_CLK(); FMC_D17_GPIO_CLK(); 
	FMC_D18_GPIO_CLK();FMC_D19_GPIO_CLK(); FMC_D20_GPIO_CLK();
	FMC_D21_GPIO_CLK();FMC_D22_GPIO_CLK(); FMC_D23_GPIO_CLK();
	FMC_D24_GPIO_CLK();FMC_D25_GPIO_CLK(); FMC_D26_GPIO_CLK();
	FMC_D27_GPIO_CLK();FMC_D28_GPIO_CLK(); FMC_D29_GPIO_CLK();
	FMC_D30_GPIO_CLK();FMC_D31_GPIO_CLK();	
	/* Control Signal */
	FMC_CS_GPIO_CLK() ; FMC_BA0_GPIO_CLK(); FMC_BA1_GPIO_CLK() ;
	FMC_WE_GPIO_CLK() ; FMC_RAS_GPIO_CLK(); FMC_CAS_GPIO_CLK();
	FMC_CLK_GPIO_CLK(); FMC_CKE_GPIO_CLK(); FMC_UDQM_GPIO_CLK();
	FMC_LDQM_GPIO_CLK();FMC_UDQM2_GPIO_CLK();FMC_LDQM2_GPIO_CLK();
  
	/*-- SDRAM IO Config -----------------------------------------------------*/     
	GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;  
	GPIO_InitStructure.Pull      = GPIO_PULLUP;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Alternate = GPIO_AF12_FMC;

	/* Address signal line for pin configuration */
	GPIO_InitStructure.Pin = FMC_A0_GPIO_PIN; 
	HAL_GPIO_Init(FMC_A0_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_A1_GPIO_PIN; 
	HAL_GPIO_Init(FMC_A1_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_A2_GPIO_PIN; 
	HAL_GPIO_Init(FMC_A2_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_A3_GPIO_PIN; 
	HAL_GPIO_Init(FMC_A3_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_A4_GPIO_PIN; 
	HAL_GPIO_Init(FMC_A4_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_A5_GPIO_PIN; 
	HAL_GPIO_Init(FMC_A5_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_A6_GPIO_PIN; 
	HAL_GPIO_Init(FMC_A6_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_A7_GPIO_PIN; 
	HAL_GPIO_Init(FMC_A7_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_A8_GPIO_PIN; 
	HAL_GPIO_Init(FMC_A8_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_A9_GPIO_PIN; 
	HAL_GPIO_Init(FMC_A9_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_A10_GPIO_PIN; 
	HAL_GPIO_Init(FMC_A10_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_A11_GPIO_PIN; 
	HAL_GPIO_Init(FMC_A11_GPIO_PORT, &GPIO_InitStructure);
	
    GPIO_InitStructure.Pin = FMC_A12_GPIO_PIN; 
	HAL_GPIO_Init(FMC_A12_GPIO_PORT, &GPIO_InitStructure);

	/* Data signal line for pin configuration */
	GPIO_InitStructure.Pin = FMC_D0_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D0_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D1_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D1_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D2_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D2_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D3_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D3_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D4_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D4_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D5_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D5_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D6_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D6_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D7_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D7_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D8_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D8_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D9_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D9_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D10_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D10_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D11_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D11_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D12_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D12_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D13_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D13_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D14_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D14_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D15_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D15_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D16_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D16_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D17_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D17_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D18_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D18_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D19_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D19_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D20_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D20_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D21_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D21_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D22_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D22_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D23_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D23_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D24_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D24_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D25_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D25_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D26_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D26_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D27_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D27_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D28_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D28_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D29_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D29_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D30_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D30_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_D31_GPIO_PIN; 
	HAL_GPIO_Init(FMC_D31_GPIO_PORT, &GPIO_InitStructure);
	/* Control signal line for pin configuration */
	GPIO_InitStructure.Pin = FMC_CS_GPIO_PIN; 
	HAL_GPIO_Init(FMC_CS_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_BA0_GPIO_PIN; 
	HAL_GPIO_Init(FMC_BA0_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_BA1_GPIO_PIN;
	HAL_GPIO_Init(FMC_BA1_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_WE_GPIO_PIN; 
	HAL_GPIO_Init(FMC_WE_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_RAS_GPIO_PIN; 
	HAL_GPIO_Init(FMC_RAS_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_CAS_GPIO_PIN; 
	HAL_GPIO_Init(FMC_CAS_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_CLK_GPIO_PIN; 
	HAL_GPIO_Init(FMC_CLK_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_CKE_GPIO_PIN; 
	HAL_GPIO_Init(FMC_CKE_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_UDQM_GPIO_PIN; 
	HAL_GPIO_Init(FMC_UDQM_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_LDQM_GPIO_PIN; 
	HAL_GPIO_Init(FMC_LDQM_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_UDQM2_GPIO_PIN; 
	HAL_GPIO_Init(FMC_UDQM2_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = FMC_LDQM2_GPIO_PIN; 
	HAL_GPIO_Init(FMC_LDQM2_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  Initialize the configuration of the SDRAM
  * @param  None. 
  * @retval None.
  */
static void SDRAM_InitSequence(void)
{
	uint32_t tmpr = 0;

	/* Step 1 ----------------------------------------------------------------*/
	/* Configuration command: Turn on the clock provided to SDRAM */
	Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
	Command.CommandTarget = FMC_COMMAND_TARGET_BANK;
	Command.AutoRefreshNumber = 1;
	Command.ModeRegisterDefinition = 0;
	/* Send configuration command */
	HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

	/* Step 2: Delay 100us */ 
	SDRAM_delay(1);

	/* Step 3 ----------------------------------------------------------------*/
	/* Configuration command: precharge all banks */ 
	Command.CommandMode = FMC_SDRAM_CMD_PALL;
	Command.CommandTarget = FMC_COMMAND_TARGET_BANK;
	Command.AutoRefreshNumber = 1;
	Command.ModeRegisterDefinition = 0;
	/* Send configuration command */
	HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);   

	/* Step 4 ----------------------------------------------------------------*/
	/* Configuration command: auto refresh */   
	Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	Command.CommandTarget = FMC_COMMAND_TARGET_BANK;
	Command.AutoRefreshNumber = 8;
	Command.ModeRegisterDefinition = 0;
	/* Send configuration command */
	HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

	/* Step 5 ----------------------------------------------------------------*/
	/* Set sdram register configuration */
	tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
				   SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
				   SDRAM_MODEREG_CAS_LATENCY_3           |
				   SDRAM_MODEREG_OPERATING_MODE_STANDARD |
				   SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

	/* Configuration command: set SDRAM register */
	Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
	Command.CommandTarget = FMC_COMMAND_TARGET_BANK;
	Command.AutoRefreshNumber = 1;
	Command.ModeRegisterDefinition = tmpr;
	/* Send Command */
	HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

	/* Step 6 ----------------------------------------------------------------*/

	/* set refresh counter */
	/* refresh period = 64ms/8192行=7.8125us */
	/* COUNT=(7.8125us x Freq) - 20 */
	/* set auto refresh rate */
	HAL_SDRAM_ProgramRefreshRate(&sdramHandle, 824); 
}


/**
  * @brief  Initial configuration using SDRAM FMC and GPIO interface
  *         This function needs to be called before SDRAM read and write operations
  * @param  None
  * @retval None
  */
void SDRAM_Init(void)
{

  FMC_SDRAM_TimingTypeDef SdramTiming;
  RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
	
  /* Configure the GPIO related to the FMC interface */
  SDRAM_GPIO_Config();

  /* Config SDRAM clock source */
  RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FMC;
  RCC_PeriphClkInit.PLL2.PLL2M = 5;
  RCC_PeriphClkInit.PLL2.PLL2N = 144;
  RCC_PeriphClkInit.PLL2.PLL2P = 2;
  RCC_PeriphClkInit.PLL2.PLL2Q = 2;
  RCC_PeriphClkInit.PLL2.PLL2R = 3;
  RCC_PeriphClkInit.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
  RCC_PeriphClkInit.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  RCC_PeriphClkInit.PLL2.PLL2FRACN = 0;
  RCC_PeriphClkInit.FmcClockSelection = RCC_FMCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit) != HAL_OK)
  {
    while(1);
  }
  /* Enable FMC clock */
  __FMC_CLK_ENABLE();

  /*执 Execute the memory initialization sequence for SDRAM1 */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1 struucture initialization */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK2;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
  hsdram1.Init.MemoryDataWidth = SDRAM_MEMORY_WIDTH;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;  // SDRAM clock 120MHz
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;     
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1; 
  /* SDRAM时序 */
  SdramTiming.LoadToActiveDelay = 2;    // Delay between Load Mode Register command and Row Validate or Refresh command
  SdramTiming.ExitSelfRefreshDelay = 8; // Delay between exit self-refresh and row valid command
  SdramTiming.SelfRefreshTime = 5;      // Delay between line valid and precharge command
  SdramTiming.RowCycleDelay = 8;        // Delay between two flush commands or two row valid commands
  SdramTiming.WriteRecoveryTime = 2;    // Delay Between Write Command to Precharge Command
  SdramTiming.RPDelay = 2;              // Delay Between Precharge and Row Valid Command
  SdramTiming.RCDDelay = 2;             // Delay between row valid and column read and write commands

  HAL_SDRAM_Init(&hsdram1, &SdramTiming);  
  /* FMC SDRAM timing initialization */
  SDRAM_InitSequence(); 
  
}



/**
  * @brief  Write data to sdram in units of "words" 
  * @param  pBuffer: pointer to data 
  * @param  uwWriteAddress: SDRAM internal address to be written
  * @param  uwBufferSize: data size to be written
  * @retval None.
  */
void SDRAM_WriteBuffer(uint32_t* pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwWriteAddress;

  /* Disable write protection */
  HAL_SDRAM_WriteProtection_Disable(&hsdram1);
  /* Check SDRAM flag, wait until SDRAM is idle */ 
  while(HAL_SDRAM_GetState(&hsdram1) != RESET)
  {
  }

  /*  */
  for (; uwBufferSize != 0; uwBufferSize--) 
  {
    /* Cyclic write data */
    *(uint32_t *) (SDRAM_BANK_ADDR + write_pointer) = *pBuffer++;

    /* adddress accumulate */
    write_pointer += 4;
  }
    
}

/**
  * @brief  read data from SDRAM
  * @param  pBuffer: Point to the buffer where the data is stored
  * @param  ReadAddress: address of data to be read
  * @param  uwBufferSize: data size to be read
  * @retval None.
  */
void SDRAM_ReadBuffer(uint32_t* pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwReadAddress;
  
   
  /* Check SDRAM flag, wait until SDRAM is free */  
  while ( HAL_SDRAM_GetState(&hsdram1) != RESET)
  {
  }
  
  /* read data */
  for(; uwBufferSize != 0x00; uwBufferSize--)
  {
   *pBuffer++ = *(__IO uint32_t *)(SDRAM_BANK_ADDR + write_pointer );
    
   /* adddress accumulate */
    write_pointer += 4;
  } 
}


/**
  * @brief  Test whether the SDRAM is normal 
  * @param  None
  * @retval Normal return 1, abnormal return 0
  */
uint8_t SDRAM_Test(void)
{
  /* write data counter */
  uint32_t  counter=0;
  
  /* 8 bits data */
  uint8_t ubWritedata_8b = 0, ubReaddata_8b = 0;  
  
  /* 16 bits data */
  uint16_t uhWritedata_16b = 0, uhReaddata_16b = 0; 
	
  /* 32 bits data */
  uint32_t uhWritedata_32b = 0, uhReaddata_32b = 0; 
  
  // SDRAM_INFO("[SDR-TEST(0)] Testing SDRAM, rw sdram in 8-bit, 16-bit, 32-bit mode...");
  printf("\r\n [SDR-TEST(0)] Testing SDRAM, rw sdram in 8-bit, 16-bit, 32-bit mode... \r\n");


  /* rw data in 8-bit format, and verify */
  
  /* Reset all SDRAM data to 0, SDRAM_SIZE is 8 bits */
  for (counter = 0x00; counter < SDRAM_SIZE; counter++)
  {
    *(__IO uint8_t*) (SDRAM_BANK_ADDR + counter) = (uint8_t)0x0;
  }
  
  /* Write data to the entire SDRAM 8 bits */
  for (counter = 0; counter < SDRAM_SIZE; counter++)
  {
    *(__IO uint8_t*) (SDRAM_BANK_ADDR + counter) = (uint8_t)(ubWritedata_8b + counter);
  }
  
  /* Read SDRAM data and test */
  for(counter = 0; counter<SDRAM_SIZE;counter++ )
  {
    ubReaddata_8b = *(__IO uint8_t*)(SDRAM_BANK_ADDR + counter);  // Read data from this address
    
    if(ubReaddata_8b != (uint8_t)(ubWritedata_8b + counter))      // check the data, if they are not equal, jump out of the function and return the result of the test failure
    {
      //SDRAM_ERROR("[SDR-TEST(1)] 8 bits data rw error!, error address: %d",counter);
      printf("\r\n [SDR-TEST(1)] 8 bits data rw error!, error address: %d \r\n",counter);	  
	  
      return 0;
    }
  }
	
  
  /* rw data in 16-bit format, and verify */
  
  /* Reset all SDRAM data to 0 */
  for (counter = 0x00; counter < SDRAM_SIZE/2; counter++)
  {
    *(__IO uint16_t*) (SDRAM_BANK_ADDR + 2*counter) = (uint16_t)0x00;
  }
  
  /* Write data to the entire SDRAM 16 bits */
  for (counter = 0; counter < SDRAM_SIZE/2; counter++)
  {
    *(__IO uint16_t*) (SDRAM_BANK_ADDR + 2*counter) = (uint16_t)(uhWritedata_16b + counter);
  }
  
    /* Read SDRAM data and test */
  for(counter = 0; counter<SDRAM_SIZE/2;counter++ )
  {
    uhReaddata_16b = *(__IO uint16_t*)(SDRAM_BANK_ADDR + 2*counter); // Read data from this address    
    if(uhReaddata_16b != (uint16_t)(uhWritedata_16b + counter))      // check the data, if they are not equal, jump out of the function and return the result of the test failure
    {
      //SDRAM_ERROR("[SDR-TEST(2)] 16 bits data rw error!, error address: %d",counter);
      printf("\r\n [SDR-TEST(2)] 16 bits data rw error!, error address: %d \r\n",counter);

      return 0;
    }
  }

	
  /* rw data in 32-bit format, and verify */
  
  /* Reset all SDRAM data to 0 */
  for (counter = 0x00; counter < SDRAM_SIZE/4; counter++)
  {
    *(__IO uint32_t*) (SDRAM_BANK_ADDR + 4*counter) = (uint32_t)0x00;
  }
  
  /* Write data to the entire SDRAM 32 bits */
  for (counter = 0; counter < SDRAM_SIZE/4; counter++)
  {
    *(__IO uint32_t*) (SDRAM_BANK_ADDR + 4*counter) = (uint32_t)(uhWritedata_32b + counter);
  }
  
    /* Read SDRAM data and test */
  for(counter = 0; counter<SDRAM_SIZE/4;counter++ )
  {
    uhReaddata_32b = *(__IO uint32_t*)(SDRAM_BANK_ADDR + 4*counter);  // Read data from this address  
    
    if(uhReaddata_32b != (uint32_t)(uhWritedata_32b + counter))       // check the data, if they are not equal, jump out of the function and return the result of the test failure
    {
      //SDRAM_ERROR("[SDR-TEST(3)] 32 bits data rw error!, error address: %d",counter);
     printf("\r\n [SDR-TEST(3)] 32 bits data rw error!, error address: %d \r\n",counter);

      return 0;
    }
  }	
	
  
  //SDRAM_INFO("[SDR-TEST(4)] SDRAM RW Test Successful! "); 
  printf("\r\n [SDR-TEST(4)] SDRAM RW Test Successful! \r\n");   
  
  /* Test successfully return 1 */
  return 1;
  

}




/*********************************************END OF FILE**********************/

