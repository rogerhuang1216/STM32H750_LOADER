 /**
  ******************************************************************************
  * @file    bsp_qspi_flash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   qspi flash bsp 
  ******************************************************************************
  * @attention
  *
  * HW Platform: Fire Challenger V2
  * Forum: http://www.firebbs.cn
  * Taobao:https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./flash/bsp_qspi_flash.h"


QSPI_HandleTypeDef QSPIHandle;

/**
  * @brief  QSPI_FLASH Pin Initialization
  * @param  NA
  * @retval NA
  */  
void QSPI_FLASH_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Enable QSPI and GPIO clocks */
	QSPI_FLASH_CLK_ENABLE();
	QSPI_FLASH_CLK_GPIO_ENABLE();
	QSPI_FLASH_BK1_IO0_CLK_ENABLE();
	QSPI_FLASH_BK1_IO1_CLK_ENABLE();
	QSPI_FLASH_BK1_IO2_CLK_ENABLE();
	QSPI_FLASH_BK1_IO3_CLK_ENABLE();
	QSPI_FLASH_CS_GPIO_CLK_ENABLE();
	
	// Setup Pin
	/*!< Configure the QSPI_FLASH pin: CLK */
	GPIO_InitStruct.Pin = QSPI_FLASH_CLK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = QSPI_FLASH_CLK_GPIO_AF;

	HAL_GPIO_Init(QSPI_FLASH_CLK_GPIO_PORT, &GPIO_InitStruct);

	/*!< Configure the QSPI_FLASH pin: IO0 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO0_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO0_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO0_PORT, &GPIO_InitStruct);

	/*!< Configure the QSPI_FLASH pin: IO1 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO1_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO1_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO1_PORT, &GPIO_InitStruct);

	/*!< Configure the QSPI_FLASH pin: IO2 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO2_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO2_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO2_PORT, &GPIO_InitStruct);

	/*!< Configure the QSPI_FLASH pin: IO3 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO3_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO3_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO3_PORT, &GPIO_InitStruct);

	/*!< Configure the QSPI_FLASH pin: NCS */
	GPIO_InitStruct.Pin = QSPI_FLASH_CS_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_CS_GPIO_AF;
	HAL_GPIO_Init(QSPI_FLASH_CS_GPIO_PORT, &GPIO_InitStruct);
	
	HAL_QSPI_DeInit(&QSPIHandle);
	/* QSPI_FLASH mode configuration */
	QSPIHandle.Instance = QUADSPI;
	/* Divided by two, the clock is 240/(1+1)=120MHz */
	QSPIHandle.Init.ClockPrescaler = 1;
	
	/* FIFO threshold is 4 bytes */
	//QSPIHandle.Init.FifoThreshold = 24;
	QSPIHandle.Init.FifoThreshold = 1;        // roger 20230724 add	
	
	/* Sample shifted by half period */
	QSPIHandle.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
	
	/* Flash size is 32M bytes, 2^25, so the weight value is 25-1=24 */
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	
	//QSPIHandle.Init.FlashSize = 24;             // roger 20230823 modify
	QSPIHandle.Init.FlashSize = 25;	
	
    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	
	/* Chip select high level hold time, at least 50ns, corresponding to the number of cycles 6*9.2ns =55.2ns */
	QSPIHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_6_CYCLE;
	/* Clock mode selection mode 0, when nCS is high level (chip select release), CLK must remain low level */
	QSPIHandle.Init.ClockMode = QSPI_CLOCK_MODE_3;
	/* Select the first piece of Flash according to the hardware connection */
	QSPIHandle.Init.FlashID = QSPI_FLASH_ID_1;
	QSPIHandle.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
	
	HAL_QSPI_Init(&QSPIHandle);
	
	/* Initialize QSPI */
	BSP_QSPI_Init();
}

/**
  * @brief  Configure QSPI as memory-mapped mode
  * @retval QSPI memory status
  */
uint32_t QSPI_EnableMemoryMappedMode()
{
  QSPI_CommandTypeDef      s_command;
  QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

  /* Config Read Command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = QUAD_INOUT_FAST_READ_CMD_4BYTE;
  s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = 6;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Config memory-mapped mode */
  s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
  s_mem_mapped_cfg.TimeOutPeriod     = 0;

  if (HAL_QSPI_MemoryMapped(&QSPIHandle, &s_command, &s_mem_mapped_cfg) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}




/**
  * @brief  Set the QSPI memory to 4-byte address mode
  * @param  NA
  * @retval return status
*/
uint8_t QSPI_EnterFourBytesAddress(void)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = ENTER_4_BYTE_ADDR_MODE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* enable write operation */
  QSPI_WriteEnable();
  
  /* transmission command */
  if (HAL_QSPI_Command(&QSPIHandle, &s_command,HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {          
    return QSPI_ERROR;
  }

	/* Auto-polling mode waits for memory to be ready */  
	if (QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;

}
/**
  * @brief  Initialize QSPI Flash
  * @retval QSPI Flash Status
  */
uint8_t BSP_QSPI_Init(void)
{
	QSPI_CommandTypeDef s_command;
	uint8_t value = 0X06;
	/* write enable */
	if (QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}	
	/* Set the 4 channel enable status register to enable the four-channel IO2 and IO3 pins */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = WRITE_STATUS_REG1_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 1;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	/* config command */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	/* send data */
	if (HAL_QSPI_Transmit(&QSPIHandle, &value,HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	/* Auto-polling mode waits for memory to be ready */  
	if (QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
  /* QSPI memory reset */
  if (QSPI_ResetMemory() != QSPI_OK)
  {
    return QSPI_ERROR;
  }
	QSPI_EnterFourBytesAddress();

	return QSPI_OK;
}

/**
  * @brief  Read large amount of data from QSPI memory
  * @param  pData: pointer to the data to read
  * @param  ReadAddr: read start address
  * @param  Size: size of data to read   
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
	QSPI_CommandTypeDef s_command;
  
  if(Size == 0)
  {
    printf("BSP_QSPI_Read Size = 0");
    return QSPI_OK;
  }

	/* Initialize read command */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_CMD_4BYTE;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.Address           = ReadAddr;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = Size;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* config command */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{      
		return QSPI_ERROR;
	}

	/* receive data */
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}
/**
  * @brief  Read large amount of data from QSPI memory
  * @param  pData: pointer to the data to read
  * @param  ReadAddr: read start addr
  * @param  Size: size of data to read  
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_FastRead(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
	QSPI_CommandTypeDef s_command;

  if(Size == 0)
  {
    printf("BSP_QSPI_FastRead Size = 0");
    return QSPI_OK;
  }
	/* initialize read command */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = QUAD_INOUT_FAST_READ_CMD_4BYTE;
	s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.Address           = ReadAddr;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = 6;
	s_command.NbData            = Size;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* config command */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* receive data */
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
  * @brief  Write large amount of data to QSPI memory
  * @param  pData: pointer to the data to write
  * @param  WriteAddr: write start address
  * @param  Size: size of data to write    
  * @retval QSPI memory status
  */

/////////////////////////////////////////////////////////////////////////////////////
//
//         "BSP_QSPI_Write" can be used only in [Indirect Mode]
//
/////////////////////////////////////////////////////////////////////////////////////


uint8_t BSP_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)      // size unit semms to be byte ?
{
	QSPI_CommandTypeDef s_command;
	uint32_t end_addr, current_size, current_addr;
  
  if(Size == 0)
  {
    printf("BSP_QSPI_Write Size = 0");
    return QSPI_OK;
  }

	/* Calculate the size between the write address and the end of the page */
	current_addr = 0;

	while (current_addr <= WriteAddr)
	{
		current_addr += W25Q256JV_PAGE_SIZE;
	}
	current_size = current_addr - WriteAddr;

	/* Check if the size of the data is smaller than the remaining place in the page */
	if (current_size > Size)
	{
		current_size = Size;
	}

	/* Initialize address variable */
	current_addr = WriteAddr;
	end_addr = WriteAddr + Size;

	/* initializer command */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = QUAD_INPUT_PAGE_PROG_CMD_4BYTE;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* Writes are performed page by page */
	do
	{
		s_command.Address = current_addr;
    if(current_size == 0)   
    {
      printf(" [BSP_QSPI_Write] [0] BSP_QSPI_Write current_size = 0");
      return QSPI_OK;
    }

		s_command.NbData  = current_size;

		/* enable write operation */
		if (QSPI_WriteEnable() != QSPI_OK)			
		{
			printf("\r\n [BSP_QSPI_Write] [1] QSPI_WriteEnable Error \r\n");    		    
			return QSPI_ERROR;
		}

		/* config command */
		if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			printf("\r\n [BSP_QSPI_Write] [2] HAL_QSPI_Command Error \r\n");    		
			return QSPI_ERROR;
		}

		/* send data */
		if (HAL_QSPI_Transmit(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			printf("\r\n [BSP_QSPI_Write] [3] HAL_QSPI_Transmit Error \r\n");  		
			return QSPI_ERROR;
		}

		/* Configure automatic polling mode to wait for the end of the program */  
		if (QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
		{
			printf("\r\n [BSP_QSPI_Write] [4] QSPI_AutoPollingMemReady Error \r\n");  				
			return QSPI_ERROR;
		}

		/* Update address and size variables for next page programming */
		current_addr += current_size;
		pData += current_size;
		current_size = ((current_addr + W25Q256JV_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q256JV_PAGE_SIZE;
	} while (current_addr < end_addr);
	return QSPI_OK;
}

/**
  * @brief  Erase a specified block of QSPI memory
  * @param  BlockAddress: Block address to be erased 
  * @retval QSP memory status
  */

// W25Q256JV, SPI-Flash Block= 64KB

uint8_t BSP_QSPI_Erase_Block(uint32_t BlockAddress)
{
	QSPI_CommandTypeDef s_command;
	/* Initialize the erase command */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = SECTOR_ERASE_CMD_4BYTE;      // 0x21
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.Address           = BlockAddress;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* enable write operation */
	if (QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}
  //QSPI_FLASH_Wait_Busy();
	/* send command  */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
  //QSPI_FLASH_Wait_Busy();
	/* Configure automatic polling mode to wait for the end of the erase */  
	if (QSPI_AutoPollingMemReady(W25Q256JV_SUBSECTOR_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
  
	return QSPI_OK;
}

/**
  * @brief  Erase the entire QSPI memory
  * @retval QSPI memroy status
  */
uint8_t BSP_QSPI_Erase_Chip(void)
{
	QSPI_CommandTypeDef s_command ={0};    // roger 20230724 add

	//QSPI_CommandTypeDef s_command ;
	
	/* Initialize the erase command */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = CHIP_ERASE_CMD;                     // 0xC7
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* write enable */
	if (QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	/* send command */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	} 
	/* Config automatic polling mode to wait for the end of the erase */  
	if (QSPI_AutoPollingMemReady(W25Q256JV_BULK_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
  * @brief  Read the current state of the QSPI memory
  * @retval QSPI memroy status
  */
uint8_t BSP_QSPI_GetStatus(void)
{
	QSPI_CommandTypeDef s_command;
	uint8_t reg;
	/* Initialize read status register command */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_STATUS_REG1_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 1;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* config command */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	/* receive data */
	if (HAL_QSPI_Receive(&QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	} 
	/* check the value of the register */
	if((reg & W25Q256JV_FSR_BUSY) != 0)
	{
		return QSPI_BUSY;
	}
	else
	{
		return QSPI_OK;
	}
}

/**
  * @brief  Returns the configuration of the QSPI memory
  * @param  pInfo: pointer on configuration structure
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_GetInfo(QSPI_Info* pInfo)
{
	/* Configuration Memory Configuration Structure */
	pInfo->FlashSize          = W25Q256JV_FLASH_SIZE;
	pInfo->EraseSectorSize    = W25Q256JV_SUBSECTOR_SIZE;
	pInfo->EraseSectorsNumber = (W25Q256JV_FLASH_SIZE/W25Q256JV_SUBSECTOR_SIZE);
	pInfo->ProgPageSize       = W25Q256JV_PAGE_SIZE;
	pInfo->ProgPagesNumber    = (W25Q256JV_FLASH_SIZE/W25Q256JV_PAGE_SIZE);
	return QSPI_OK;
}

/**
  * @brief  Reset QSPI memory
  * @param  QSPIHandle: QSPI handle
  * @retval NA
  */
uint8_t QSPI_ResetMemory()
{
	QSPI_CommandTypeDef s_command;
	/* Initialize reset enable command */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = RESET_ENABLE_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* send command */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* Send reset memory command */
	s_command.Instruction = RESET_MEMORY_CMD;
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* Configure auto-polling mode to wait for memory to be ready */  
	if (QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}


/**
  * @brief  Send write enable, wait for it to be valid
  * @param  QSPIHandle: QSPI handle
  * @retval NA
  */
static uint8_t QSPI_WriteEnable()
{
	QSPI_CommandTypeDef     s_command;
	QSPI_AutoPollingTypeDef s_config;
	/* enable write operation */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = WRITE_ENABLE_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* Configure auto-polling mode to wait for write enable */  
	s_config.Match           = W25Q256JV_FSR_WREN;
	s_config.Mask            = W25Q256JV_FSR_WREN;
	s_config.MatchMode       = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 1;
	s_config.Interval        = 0x10;
	s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

	s_command.Instruction    = READ_STATUS_REG1_CMD;
	s_command.DataMode       = QSPI_DATA_1_LINE;
	s_command.NbData         = 1;

	if (HAL_QSPI_AutoPolling(&QSPIHandle, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
  * @brief  Read memory's SR and wait for EOP
  * @param  QSPIHandle: QSPI handle
  * @param  Timeout 
  * @retval NA
  */
static uint8_t QSPI_AutoPollingMemReady(uint32_t Timeout)
{
	QSPI_CommandTypeDef     s_command;
	QSPI_AutoPollingTypeDef s_config;
	/* Configure auto-polling mode to wait for memory to be ready */  
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_STATUS_REG1_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	s_config.Match           = 0x00;
	s_config.Mask            = W25Q256JV_FSR_BUSY;
	s_config.MatchMode       = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 1;
	s_config.Interval        = 0x10;                        // different with Wtmec sample code
	s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

	if (HAL_QSPI_AutoPolling(&QSPIHandle, &s_command, &s_config, Timeout) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

 /**
  * @brief  Read FLASH ID
  * @param 	NA
  * @retval FLASH ID
  */
uint32_t QSPI_FLASH_ReadID(void)
{
	QSPI_CommandTypeDef s_command;
	uint32_t Temp = 0;
	uint8_t pData[3];
	/* ∂¡»°JEDEC ID */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_JEDEC_ID_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 3;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* User can add some code here to handle this error */
		while(1)
		{
			
		}
	}
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* User can add some code here to handle this error */
		while(1)
		{
			
		}
	}

	Temp = ( pData[2] | pData[1]<<8 )| ( pData[0]<<16 );

	return Temp;
}

 /**
  * @brief  Read FLASH Device ID
  * @param 	NA
  * @retval FLASH Device ID
  */
uint32_t QSPI_FLASH_ReadDeviceID(void)
{  
	QSPI_CommandTypeDef s_command;
	uint32_t Temp = 0;
	uint8_t pData[3];
	/*## -2- Read device ID test  ###########################################*/
	/* Read Manufacturing/Device ID */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_ID_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.Address           = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 2;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* User can add some code here to handle this error */
		while(1)
		{
			
		}
	}
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* User can add some code here to handle this error */
		while(1)
		{
			
		}
	}

	Temp = pData[1] |( pData[0]<<8 ) ;

	return Temp;
}

 /**
  * @brief  read ReadStatusReg
  * @param 	NA
  * @retval ReadStatusReg
  */
uint32_t QSPI_FLASH_ReadStatusReg(uint8_t reg)
{  
	QSPI_CommandTypeDef s_command;
	uint32_t Temp = 0;
	uint8_t pData[10];

	/* Read Manufacturing/Device ID */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	if(reg == 1)
	s_command.Instruction       = READ_STATUS_REG1_CMD;
	else if(reg == 2)
	s_command.Instruction       = READ_STATUS_REG2_CMD;
	else if(reg == 3)
	s_command.Instruction       = READ_STATUS_REG3_CMD;
	
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.Address           = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 1;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* User can add some code here to handle this error */
		while(1)
		{
			
		}
	}
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* User can add some code here to handle this error */
		while(1)
		{
			
		}
	}

	// The number of bytes read in single flash is pData[0]„ÄÅpData[1]
	// Temp = pData[0] |( pData[1]<<8 ) ;
	
	// The number of bytes read in double flash is pData[0], pData[2]
	// Temp = pData[1] |( pData[0]<<8 ) ;
		Temp = pData[0] ;
	return Temp;
}

 /**
  * @brief  read ReadStatusReg
  * @param 	NA
  * @retval ReadStatusReg
  */
uint32_t QSPI_FLASH_WriteStatusReg(uint8_t reg,uint8_t regvalue)
{  
	QSPI_CommandTypeDef s_command;
	
	/* enable write operation */
	if (QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}	
	/* Read Manufacturing/Device ID */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	if(reg == 1)
	s_command.Instruction       = WRITE_STATUS_REG1_CMD;
	else if(reg == 2)
	s_command.Instruction       = WRITE_STATUS_REG2_CMD;
	else if(reg == 3)
	s_command.Instruction       = WRITE_STATUS_REG3_CMD;
	
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AddressSize       = QSPI_ADDRESS_8_BITS;
	s_command.Address           = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 1;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* User can add some code here to handle this error */
		while(1)
		{
			
		}
	}
	if (HAL_QSPI_Transmit(&QSPIHandle, &regvalue, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* User can add some code here to handle this error */
		while(1)
		{
			
		}
	}
	/* Auto-polling mode waits for memory to be ready */  
	if (QSPI_AutoPollingMemReady(W25Q256JV_SUBSECTOR_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

void QSPI_Set_WP_High(void)
{
	/* Define a structure of type GPIO_InitTypeDef */
	GPIO_InitTypeDef  GPIO_InitStruct;
	__GPIOF_CLK_ENABLE();
	/* Select the GPIO pin to control */															   
	GPIO_InitStruct.Pin = GPIO_PIN_7;	
	/* Set the output type of the pin to push-pull output */
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
	/* Set the pin to pull-up mode */
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	/* Set pin speed to high speed */   
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
	/* Call the library function to initialize GPIO using the GPIO_InitStructure configured above */
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);	
	
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_7,1);
}
void QSPI_Set_WP_TO_QSPI_IO(void)
{
	/* Define a structure of type GPIO_InitTypeDef */
	GPIO_InitTypeDef  GPIO_InitStruct;
	QSPI_FLASH_BK1_IO2_CLK_ENABLE();

	// Set GPIO
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	/*!< Configure QSPI_FLASH pin: IO2 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO2_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO2_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO2_PORT, &GPIO_InitStruct);
}

// wait for idle
void QSPI_FLASH_Wait_Busy(void)   
{   
	while((QSPI_FLASH_ReadStatusReg(1)&0x01)==0x01);   // Wait for the BUSY bit to clear
}   





/*********************************************END OF FILE**********************/

