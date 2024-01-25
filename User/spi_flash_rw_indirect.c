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
//#include "./bsp/flash/bsp_qspi_flash.h" 
#include "./delay/core_delay.h"
//#include "bsp_qspi_flash_user.h"
#include "flash.h"
#include "cmd.h"



typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
/* Get the length of the buffer */
#define TxBufferSize1_indir   (countof(Tx_Buffer_indir) - 1)
#define RxBufferSize1_indir   (countof(Tx_Buffer_indir) - 1)
#define countof(a)      	  (sizeof(a) / sizeof(*(a)))
#define  BufferSize_indir     (countof(Tx_Buffer_indir)-1)

#define  FLASH_WriteAddress     0
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
   

#if 1				  // roger 20230627 mark 
uint8_t Tx_Buffer_indir[] = "http://firestm32.taobao.com Thank for you choosing Fire stm32 EVB\r\nhttp://firestm32.taobao.com Thank for you choosing Fire stm32 EVB\r\nhttp://firestm32.taobao.com Thank for you choosing Fire stm32 EVB\r\nhttp://firestm32.taobao.com Thank for you choosing Fire stm32 EVB\r\nhttp://firestm32.taobao.com Thank for you choosing Fire stm32 EVB";
uint8_t Tx_Buffer2_indir[] ={0XCC,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A, 0x28, 0x0A,0XFF,0XAA,0XBB,0XCC,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A, 0x28, 0x0A};
uint8_t Rx_Buffer_indir[BufferSize_indir];
#endif


// Read ID storage location

#if 1
__IO uint32_t DeviceID_indir = 0;
__IO uint32_t FlashID_indir = 0;
__IO TestStatus TransferStatus1_indir = FAILED;
#endif



// function prototype declaration



#if 1
void Delay_indir(__IO uint32_t nCount);
TestStatus Buffercmp_indir(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);
#endif


INT32S Do_Spi_FLASH_RW_Indirect (INT32S argc, INT8S *argv[]);


uint8_t state_indir = QSPI_ERROR;





/**
  * @brief SPI Flash RW Test by Indirect Access
  * @retval
  *  None
  */

INT32S Do_Spi_FLASH_RW_Indirect (INT32S argc, INT8S *argv[])
{
//	int t0,t1,i;
	uint32_t addr = FLASH_WriteAddress ;
	int state_indir = QSPI_ERROR;

	
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
  
	printf("\r\n [01] This is a 32M serial flash (W25Q256) indirect mode RW experiment (QSPI driver)\r\n");
	
	/* 32 spi flash W25Q256 initialization */
	QSPI_FLASH_Init();  // roger 20230710, move to BSP Init.
	
	/* Get Flash Device ID */
	DeviceID_indir = QSPI_FLASH_ReadDeviceID();
	printf("\r\n [02] QSPI_FLASH_DeviceID is 0x%04X,\r\n", DeviceID_indir);		
	Delay_indir( 200 );
	/* Get SPI Flash ID */
	FlashID_indir = QSPI_FLASH_ReadID();	
	printf("\r\n [03] QSPI_FLASH_ID is 0x%04X,\r\n", FlashID_indir);		
	QSPI_Set_WP_High();
    // write status register	
	QSPI_FLASH_WriteStatusReg(1,0X00);
	QSPI_FLASH_WriteStatusReg(2,0X00);
	QSPI_FLASH_WriteStatusReg(3,0X61);
	printf("\r\n [04] FlashID is 0x%X,  Manufacturer Device ID is 0x%X\r\n", FlashID_indir, DeviceID_indir);
	printf("\r\n [05] Flash Status Reg1 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(1));	
	printf("\r\n [06] Flash Status Reg2 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(2));
	printf("\r\n [07] Flash Status Reg3 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(3));
	QSPI_Set_WP_TO_QSPI_IO();
	/* Check SPI Flash ID */
	if (FlashID_indir == sFLASH_ID)
	{	
		printf("\r\n [08] Detect QSPI FLASH W25Q256 !\r\n");
		printf("\r\n [09] Erasing the contents of 0x%x~0x%x of the chip !\r\n", addr, addr+W25Q256JV_PAGE_SIZE);
    state_indir = BSP_QSPI_Erase_Block(addr);
    if(state_indir == QSPI_OK)
      printf("\r\n [10] erase success !\r\n");
    else
    {
      LED_RED;
      printf("\r\n [10] erase fail !\r\n");
      while(1);
      
    }
	
	    printf("\r\n [11] Writing data to the chip address 0x%x, the size is %d !\r\n", addr, BufferSize_indir);
		/* Write the data in the send buffer to flash */
		BSP_QSPI_Write(Tx_Buffer_indir, addr, BufferSize_indir);
		printf("\r\n [12] write success !\r\n");
	    
	    printf("\r\n [13] Reading data of the chip address 0x%x, the size is %d !\r\n", addr, BufferSize_indir);
		/* Read the data just written into the receive buffer */
		BSP_QSPI_FastRead(Rx_Buffer_indir, addr, BufferSize_indir);
		printf("\r\n [14] read success !\r\n");	    
    
		/* Check if the written data is equal to the read data */
		TransferStatus1_indir = Buffercmp_indir(Tx_Buffer_indir, Rx_Buffer_indir, BufferSize_indir);
		
		if( PASSED == TransferStatus1_indir )
		{    
			LED_GREEN;
			printf("\r\n [15] RW (pointer operation) test success !\n\r");
		}
		else
		{		 
			LED_RED;
			printf("\r\n [15] RW (pointer operation) test fail !\n\r");

		}
	}// if (FlashID == sFLASH_ID)
	else
	{    
		LED_RED;
		printf("\r\n [16] Unable to get W25Q256 ID !\n\r");
	}
	
	return 1;
	//while(1);  

	
}


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

TestStatus Buffercmp_indir (uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
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


void Delay_indir(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
/**
  * @brief  System Clock ≈‰÷√
  *         system Clock ≈‰÷√»Áœ¬: 
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
  RCC_OscInitStruct.PLL.PLLQ = 4;
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



INT32S Do_SpiFlash(INT32S argc, INT8S *argv[])
{
	INT32U dest, dest_b;
  //	INT32S retval;

	volatile  unsigned char *flashaddr;
    
	//uint32_t r_dest;     					  // roger 20230825 add 
	
	uint32_t flashaddr_r;                     // roger 20230825 add 

	volatile  unsigned char *flashdata_b1;    // roger 20230720 add		
	volatile  unsigned char *flashdata_b2;    // roger 20230720 add	
	volatile  unsigned char *flashdata_b3;    // roger 20230720 add		
	volatile  unsigned char *flashdata_b4;    // roger 20230720 add	

	
	uint8_t flashdata_rb[1];				  // roger 20230825 add	
	uint8_t flashdata_rhw[2];                 // roger 20230825 add
	uint8_t flashdata_rw[4];	              // roger 20230825 add

	
    uint8_t bytestate = QSPI_ERROR;
    uint8_t halfwordstate = QSPI_ERROR;
    uint8_t wordstate = QSPI_ERROR;	

	
	//   QSPI_FLASH_Init(); // roger 20230710, move to BSP Init.
	
	
	if (strcmp("erase",argv[1])==0)
	{
		INT8U warning_flag = 0;
		
		if (argc < 3)
		{
			printf("\n\r [SPI-FLASH] argument not enough\n");
			return 0;
		}	

		if (strcmp("all",argv[2])==0)
		{
		//	dest = SPI_FLH_BASE_ADDR;
		//	len = SPIFLH_ERASE_ALL;
			warning_flag = 1;
	    }
		else if(strcmp("block",argv[2])==0)
		{
			if (argc != 4)
			{
				printf("\n\r [SPI-FLASH] argument not enough\n");
				return -1;
			}
			dest_b = atoh(argv[3]);
			//flashaddr  = (unsigned char *)dest_b;

			//warning_flag = 1;
			warning_flag = 2;			

            #if 0
			/* to warning will erase loader image */
			if (((dest>=FLH_LOADER_START) && (dest<=(FLH_LOADER_START+FLH_LOADER_SIZE)))
			 || (((dest+len)>=FLH_LOADER_START) && ((dest+len)<=(FLH_LOADER_START+FLH_LOADER_SIZE))))
				warning_flag = 1;
			#endif
		}
     
        else
        {
			printf("\n\r [SPI-FLASH] ambigous Erase operation, pls reinput the erase function you want \n");
		    warning_flag = 0;
			return -2;
        }

		
		/* to warning will erase app image */
		//if (warning_flag == 1)
		if (warning_flag >= 1)			
		{
			CHAR line[CMD_LEN] = {0};
			
			printf("\n\r [SPI-FLASH] Erase action will damge APP image!! Continue? (y/n)>");
			while (1)
			{
				//if ((retval=readline((INT8U *)line)) != 0)
				if (readline((INT8U *)line) != 0)					
				{
					if (strcmp(line, "y") == 0 || strcmp(line, "Y") == 0)
					{
						break;
					}
					else if (strcmp(line, "n") == 0 || strcmp(line, "N") == 0)
					{
						return 0;
					}
					else
						printf("\n\r [SPI-FLASH] Erase action will damge APP image!! Continue? (y/n)>");
				}
			}

			if (strcmp("all",argv[2])==0)	
			{
			    QSPI_FLASH_Init();      // roger 20230710 add
				if (BSP_QSPI_Erase_Chip()==QSPI_OK)
					printf("\n\r [SPI-FLASH] Erase Whole SPI-FLASH OK \r\n");
				else 
					printf("\n\r [SPI-FLASH] Erase Whole SPI-FLASH Fail \r\n");
			}
			else if (strcmp("block",argv[2])==0)
			{
				//if (BSP_QSPI_Erase_Block((uint32_t)flashaddr)==QSPI_OK)
				if (BSP_QSPI_Erase_Block((uint32_t)dest_b)==QSPI_OK)					
					//printf("\n\r [SPI-FLASH] Erase SPI-FLASH Block Address 0x%lx OK \r\n",flashaddr);
				    printf("\n\r [SPI-FLASH] Erase SPI-FLASH Block Address 0x%lx OK \r\n",dest_b);				
				else 
					printf("\n\r [SPI-FLASH] Erase SPI-FLASH Fail \r\n");
			}
		
		}
		//FLH_Erase((INT8U *)dest, len);

		else
	        printf("\n\r [SPI-FLASH] Do nothing ! \r\n");
		    	

	}

   ////////////////////////////////////////////////////////////      UNDER CONSTRUCTION

	
     else if (strcmp("writebyte",argv[1])==0)
     {
		if (argc < 4)
		{
		printf("\n\r [SPI-FLASH] [0] argument not enough\n");
		return 0;
		}
		dest = atoh(argv[2]);               		// The Flash addr will be programmed
		flashaddr  = (unsigned char *)dest;


		flashdata_b1 = (unsigned char *)atoh(argv[3]);
		uint8_t Input_databuff_b[]={(uint8_t)flashdata_b1};
		
		
		printf("\n\r [SPI-FLASH] [0] Get flash addr to be written = 0x%lx \n", flashaddr);
		printf("\n\r [SPI-FLASH] [0] Get flash byte data to be written = 0x%lx \n", flashdata_b1);
		

        #if 1
        // Erase dedicated block before writebyte test // 
		
		bytestate = BSP_QSPI_Erase_Block((uint32_t)flashaddr);
		if(bytestate == QSPI_OK)
		{
		LED_GREEN;
		printf("\r\n [SPI-FLASH] [1] Block erase success !\r\n");
		}
		else
		{
		LED_RED;
		printf("\r\n [SPI-FLASH] [1] Block erase fail !\r\n");
		while(1);
		}

        #endif

		if(BSP_QSPI_Write(Input_databuff_b, (uint32_t) flashaddr, 1)==QSPI_OK) 		// roger, 20230717 add			
			{
		     printf("\n\r [SPI-FLASH] [2] SPI-FLASH_ProgramByte Addr = 0x%lx \n", flashaddr);
	      	 printf("\n\r [SPI-FLASH] [2] SPI-FLASH_ProgramByte Data = 0x%lx \n", flashdata_b1);	
		     LED_GREEN;				 
			}
	    else
	    	{
			 printf("\n\r [SPI-FLASH] [2] SPI-FLASH_ProgramByte Fail \n\r");
		     LED_RED;			 
	    	}
    }
	  
	else if (strcmp("writehalfword",argv[1])==0)
	{
		if (argc < 5)
		{
		printf("\n\r [SPI-FLASH] [0] argument not enough\n");
		return 0;
		}
		dest = atoh(argv[2]);               		// The Flash addr will be programmed
		flashaddr  = (unsigned char *)dest;
		flashdata_b1 = (unsigned char *)atoh(argv[3]);
		flashdata_b2 = (unsigned char *)atoh(argv[4]);		
		uint8_t Input_databuff_hw[]={(uint8_t)flashdata_b1, (uint8_t)flashdata_b2};
	
		printf("\n\r [SPI-FLASH] [0] Get flash addr to be written = 0x%lx \n", flashaddr);
		printf("\n\r [SPI-FLASH] [0] Get flash half word data to be written = 0x%lx%lx \n", flashdata_b1, flashdata_b2);
		

        #if 1
        // Erase dedicated block before writebyte test // 
		
		halfwordstate = BSP_QSPI_Erase_Block((uint32_t)flashaddr);
		if(halfwordstate == QSPI_OK)
		{
		LED_GREEN;
		printf("\r\n [SPI-FLASH] [1] Block erase success !\r\n");
		}
		else
		{
		LED_RED;
		printf("\r\n [SPI-FLASH] [1] Block erase fail !\r\n");
		while(1);
		}

        #endif

		if(BSP_QSPI_Write(Input_databuff_hw, (uint32_t) flashaddr, 2)==QSPI_OK) 		// roger, 20230717 add			
			{
		     printf("\n\r [SPI-FLASH] [2] SPI-FLASH_ProgramHalfword Addr = 0x%lx \n", flashaddr);
	      	 printf("\n\r [SPI-FLASH] [2] SPI-FLASH_ProgramHalfword Data = 0x%lx%lx \n", flashdata_b1, flashdata_b2);	
		     LED_GREEN;				 
			}
	    else
	    	{
			 printf("\n\r [SPI-FLASH] [2] SPI-FLASH_ProgramHalfword Fail \n\r");
		     LED_RED;			 
	    	}
    }
	
	else if (strcmp("writeword",argv[1])==0)
	{
		if (argc < 7)
		{
		printf("\n\r [SPI-FLASH] [0] argument not enough\n");
		return 0;
		}
		dest = atoh(argv[2]);               		// The Flash addr will be programmed
		flashaddr  = (unsigned char *)dest;
		flashdata_b1 = (unsigned char *)atoh(argv[3]);
		flashdata_b2 = (unsigned char *)atoh(argv[4]);
		flashdata_b3 = (unsigned char *)atoh(argv[5]);
		flashdata_b4 = (unsigned char *)atoh(argv[6]);

		uint8_t Input_databuff_w[]={(uint8_t)flashdata_b1, (uint8_t)flashdata_b2, (uint8_t)flashdata_b3, (uint8_t)flashdata_b4};
	
		printf("\n\r [SPI-FLASH] [0] Get flash addr to be written = 0x%lx \n", flashaddr);
		printf("\n\r [SPI-FLASH] [0] Get flash word data to be written = 0x%lx%lx%lx%lx \n", flashdata_b1, flashdata_b2, flashdata_b3, flashdata_b4);
		

        #if 1
        // Erase dedicated block before writebyte test // 
		
		wordstate = BSP_QSPI_Erase_Block((uint32_t)flashaddr);
		if(wordstate == QSPI_OK)
		{
		LED_GREEN;
		printf("\r\n [SPI-FLASH] [1] Block erase success !\r\n");
		}
		else
		{
		LED_RED;
		printf("\r\n [SPI-FLASH] [1] Block erase fail !\r\n");
		while(1);
		}

        #endif

		if(BSP_QSPI_Write(Input_databuff_w, (uint32_t) flashaddr, 4)==QSPI_OK) 		// roger, 20230717 add			
			{
		     printf("\n\r [SPI-FLASH] [2] SPI-FLASH_ProgramWord Addr = 0x%lx \n", flashaddr);
	      	 printf("\n\r [SPI-FLASH] [2] SPI-FLASH_ProgramWord Data = 0x%lx%lx%lx%lx \n", flashdata_b1, flashdata_b2, flashdata_b3, flashdata_b4);	
		     LED_GREEN;				 
			}
	    else
	    	{
			 printf("\n\r [SPI-FLASH] [2] SPI-FLASH_ProgramWord Fail \n\r");
		     LED_RED;			 
	    	}
    }	

   	else if (strcmp("readbyte",argv[1])==0)
	{
		if (argc < 3)
		{
		printf("\n\r [SPI-FLASH] [00] argument not enough\n");
		return 0;
		}

		flashaddr_r = atoh(argv[2]);						// The SPI Flash addr will be read		
	

		printf("\n\r [SPI-FLASH] [01] SPI-FLASH addr to be read = 0x%lx \n", flashaddr_r);


        #if 1

		/* 32 spi flash W25Q256 initialization */
		//QSPI_FLASH_Init();	// roger 20230710, move to BSP Init.
		
		/* Get Flash Device ID */
		DeviceID_indir = QSPI_FLASH_ReadDeviceID();
		printf("\r\n [02] QSPI_FLASH_DeviceID is 0x%04X,\r\n", DeviceID_indir); 	
		Delay_indir( 200 );
		/* Get SPI Flash ID */
		FlashID_indir = QSPI_FLASH_ReadID();	
		printf("\r\n [03] QSPI_FLASH_ID is 0x%04X,\r\n", FlashID_indir);		
		QSPI_Set_WP_High();
		// write status register	
		QSPI_FLASH_WriteStatusReg(1,0X00);
		QSPI_FLASH_WriteStatusReg(2,0X00);
		QSPI_FLASH_WriteStatusReg(3,0X61);
		printf("\r\n [04] FlashID is 0x%X,	Manufacturer Device ID is 0x%X\r\n", FlashID_indir, DeviceID_indir);
		printf("\r\n [05] Flash Status Reg1 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(1));	
		printf("\r\n [06] Flash Status Reg2 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(2));
		printf("\r\n [07] Flash Status Reg3 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(3));
		QSPI_Set_WP_TO_QSPI_IO();

		#endif

        #if 1
        while (BSP_QSPI_FastRead(flashdata_rb, (uint32_t) flashaddr_r, 1)==QSPI_OK) 
        {
			printf("\n\r [SPI-FLASH] [08] SPI-FLASH byte addr = 0x%lx, data = 0x%2x \n",flashaddr_r, flashdata_rb[0]);
            break;
        }	
        #endif
		
	}


   	else if (strcmp("readhalfword",argv[1])==0)
	{
		if (argc < 3)
		{
		printf("\n\r [SPI-FLASH] [00] argument not enough\n");
		return 0;
		}

		flashaddr_r = atoh(argv[2]);						// The SPI Flash addr will be read		
	

		printf("\n\r [SPI-FLASH] [01] SPI-FLASH addr to be read = 0x%lx \n", flashaddr_r);


	#if 1

		/* 32 spi flash W25Q256 initialization */
		//QSPI_FLASH_Init();	// roger 20230710, move to BSP Init.
		
		/* Get Flash Device ID */
		DeviceID_indir = QSPI_FLASH_ReadDeviceID();
		printf("\r\n [02] QSPI_FLASH_DeviceID is 0x%04X,\r\n", DeviceID_indir); 	
		Delay_indir( 200 );
		/* Get SPI Flash ID */
		FlashID_indir = QSPI_FLASH_ReadID();	
		printf("\r\n [03] QSPI_FLASH_ID is 0x%04X,\r\n", FlashID_indir);		
		QSPI_Set_WP_High();
		// write status register	
		QSPI_FLASH_WriteStatusReg(1,0X00);
		QSPI_FLASH_WriteStatusReg(2,0X00);
		QSPI_FLASH_WriteStatusReg(3,0X61);
		printf("\r\n [04] FlashID is 0x%X,	Manufacturer Device ID is 0x%X\r\n", FlashID_indir, DeviceID_indir);
		printf("\r\n [05] Flash Status Reg1 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(1));	
		printf("\r\n [06] Flash Status Reg2 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(2));
		printf("\r\n [07] Flash Status Reg3 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(3));
		QSPI_Set_WP_TO_QSPI_IO();

	#endif

	#if 1
        while (BSP_QSPI_FastRead(flashdata_rhw, (uint32_t) flashaddr_r, 2)==QSPI_OK) 
        {
			printf("\n\r [SPI-FLASH] [08] SPI-FLASH halfword addr = 0x%lx, data = 0x%2x, 0x%2x \n",flashaddr_r, flashdata_rhw[0], flashdata_rhw[1]);
            break;
        }	
	#endif
		
	}
	

   	else if (strcmp("readword",argv[1])==0)
	{
		if (argc < 3)
		{
		printf("\n\r [SPI-FLASH] [00] argument not enough\n");
		return 0;
		}

		flashaddr_r = atoh(argv[2]);						// The SPI Flash addr will be read		
	

		printf("\n\r [SPI-FLASH] [01] SPI-FLASH addr to be read = 0x%lx \n", flashaddr_r);


#if 1

		/* 32 spi flash W25Q256 initialization */
		//QSPI_FLASH_Init();	// roger 20230710, move to BSP Init.
		
		/* Get Flash Device ID */
		DeviceID_indir = QSPI_FLASH_ReadDeviceID();
		printf("\r\n [02] QSPI_FLASH_DeviceID is 0x%04X,\r\n", DeviceID_indir); 	
		Delay_indir( 200 );
		/* Get SPI Flash ID */
		FlashID_indir = QSPI_FLASH_ReadID();	
		printf("\r\n [03] QSPI_FLASH_ID is 0x%04X,\r\n", FlashID_indir);		
		QSPI_Set_WP_High();
		// write status register	
		QSPI_FLASH_WriteStatusReg(1,0X00);
		QSPI_FLASH_WriteStatusReg(2,0X00);
		QSPI_FLASH_WriteStatusReg(3,0X61);
		printf("\r\n [04] FlashID is 0x%X,	Manufacturer Device ID is 0x%X\r\n", FlashID_indir, DeviceID_indir);
		printf("\r\n [05] Flash Status Reg1 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(1));	
		printf("\r\n [06] Flash Status Reg2 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(2));
		printf("\r\n [07] Flash Status Reg3 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(3));
		QSPI_Set_WP_TO_QSPI_IO();

#endif

#if 1
        while (BSP_QSPI_FastRead(flashdata_rw, (uint32_t) flashaddr_r, 4)==QSPI_OK) 
        {
			printf("\n\r [SPI-FLASH] [08] SPI-FLASH word addr = 0x%lx, data = 0x%2x, 0x%2x, 0x%2x, 0x%2x \n",flashaddr_r, flashdata_rw[0], flashdata_rw[1], flashdata_rw[2], flashdata_rw[3]);
            break;
        }	
#endif
		
	}


	#if 0
	else if (strcmp("fill",argv[1])==0)
	{
	    INT32S i,offset=0,rep;
	    INT8U *dp, *sp;
	    if (argc <= 4)
		{
			printf("\n\r [SPI-FLASH] argument not enough, fill <dest address> <length> <data>\n");
			return 0;
		}
		dest = atoh(argv[2]);
		dp = (INT8U *)dest;
		len = atoh(argv[3]);
		sp = (INT8U *)argv[4];
		rep = strlen((INT8S*)sp);
		printf("\n\r [SPI-FLASH] programming...\n");
		for (i=0;i<len;i++)
		{
		    offset = offset % rep;
		    retval = FLH_Write((INT8U *)&dp[i], (INT8U *)&sp[offset++],1);	
		    if (retval==-1)
		    {
		        printf("\n\r [SPI-FLASH] flash program fail at %lx\n",&dp[i]);
		        return 0;
		    }
		    if ((i&0x00000FFF)==0)
		    {
		        printf("\n\r [SPI-FLASH] write address %lx ...\r",&dp[i]);
		    }
		}
		/*compare*/
		printf("\n\r [SPI-FLASH] \nok, comparing...\n");
		dp = (INT8U *)dest;
		offset = 0;
		for (i=0;i<len;i++,offset++)
	    {
	        offset = offset % rep;
	        if (dp[i] != sp[offset])
	        {
	            printf("\n\r [SPI-FLASH] fail at %lx, source %x, dest %x\n",&dp[i],dp[i],sp[offset]);
	            return 0;
	        }
	        
		    if ((i&0x00000FFF)==0)
		    {
		        printf("\n\r [SPI-FLASH] compare address %lx ...",&dp[i]);
		    } 
	    }
	    printf("\n\r ok");
    }
    #endif
	
    else
    {	
     printf("\n\r [SPI-FLASH] unknown command %s!\n",argv[1]);
    }	
	return 0;
}





/****************************END OF FILE***************************/








