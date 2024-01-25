/**
  *********************************************************************
  * @file    flash.c
  * @author  roger huang
  * @version V1.0
  * @date    2023/07/12
  * @brief   FreeRTOS V9.0.0  + STM32 FW Library Example
  *********************************************************************
  * @attention
  *
  * HW Platform: Fire Challenger V2
  * Forum: http://www.firebbs.cn
  * Taobao:https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 


#include "main.h"
#include "loader.h"
#include "flash.h"
#include "cmd.h"
#include "stm32h7xx_hal_flash.h"
#include "stm32h7xx_hal.h"
//#include "./Legacy/stm32_hal_legacy.h"





////////////////////////

void FLASH_ProgramDoubleWord(uint32_t address, uint64_t data1, uint64_t data2, uint64_t data3, uint64_t data4);


/////////////////////////




void FLASH_ProgramDoubleWord(uint32_t address, uint64_t data1, uint64_t data2, uint64_t data3, uint64_t data4)
{

  //uint64_t data_s[4] = { 0x1112131415161718, 0x2122232425262728, 0x3132333435363738, 0x4142434445464748 };

  uint64_t data_s[4] = { data1, data2, data3, data4 };   // Divide into 4 times, each send 64 bits, a total of 256 bits

  if (HAL_FLASH_Unlock()==HAL_OK)
  {
	  printf("\r\n [1] START FLASH_ProgramDoubleWord !\r\n");	  
	  
	  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, address, ((uint32_t)(data_s))) == HAL_OK)		
		{
//           address = address + 0x4; /* increment for the next Flash word*/
             printf("\r\n [2] START Write Flash Success !\r\n");     
		}
  }          

  else 	       
       printf("\r\n [3] HAL_FLASH_Unlock Fail !\r\n");   
  	  
}






INT32S Do_Flash(INT32S argc, INT8S *argv[])
{
	INT32U dest;
//	INT32U dest, src, len;	
//	INT32S retval;
//	uint8_t bytedata;
//	uint16_t halfworddata;
//	uint32_t worddata1, worddata2, worddata3, worddata4;
	uint64_t doubleworddata1, doubleworddata2, doubleworddata3, doubleworddata4;	
	//long long int doubleworddata1, doubleworddata2, doubleworddata3, doubleworddata4;
	volatile  unsigned char *flashaddr;
//	volatile  unsigned char *flashdata;	
	

	if (strcmp("writedoubleword",argv[1])==0)
	{
		if (argc < 7)
		{
		  printf("\n\r [FLASH] argument not enough\n");
		  return 0;
		}
		dest = atoh(argv[2]);							// The Flash addr will be programmed

        #if 0
		doubleworddata1 = 0x1112131415161718;
		doubleworddata2 = 0x2122232425262728;
		doubleworddata3 = 0x3132333435363738;
		doubleworddata4 = 0x4142434445464748;
        #endif


		#if 1
		doubleworddata1 = atolh(argv[3]);				// Data which will be programmed into Flash		
		doubleworddata2 = atolh(argv[4]);				// Data which will be programmed into Flash
		doubleworddata3 = atolh(argv[5]);				// Data which will be programmed into Flash
		doubleworddata4 = atolh(argv[6]);				// Data which will be programmed into Flash		
        #endif
		
		flashaddr  = (unsigned char *)dest;
				
		FLASH_ProgramDoubleWord((uint32_t) flashaddr, doubleworddata1, doubleworddata2, doubleworddata3, doubleworddata4);    // program word 		
		
		printf("\n\r [FLASH] FLASH_ProgramWord Addr = 0x%lx \n", dest);
		printf("\n\r [FLASH] FLASH_ProgramWord DoubleWordData1 = 0x%llx \n", doubleworddata1);
		printf("\n\r [FLASH] FLASH_ProgramWord DoubleWordData2 = 0x%llx \n", doubleworddata2);
		printf("\n\r [FLASH] FLASH_ProgramWord DoubleWordData3 = 0x%llx \n", doubleworddata3);
		printf("\n\r [FLASH] FLASH_ProgramWord DoubleWordData4 = 0x%llx \n", doubleworddata4);

	}	
	
    else
	    {	
	     printf("\n\r [FLASH] unknown command %s!\n",argv[1]);
	    }	
		
	return 0;
	
}	/*end flash command*/


#if 1

INT8U halSpiFlashGetFrameNum(INT32U nPhyAddr, INT32U *pnFrameNum)
{
	if (nPhyAddr >= SPI_FLASH_SIZE)
		return (0);

    #if 0
	if (nPhyAddr < 0x10000)        // 0x10000=64KB, 1 block = 64K
	{
		*pnFrameNum = 1;		
	}
	else if ((nPhyAddr >= 0x10000)&&(nPhyAddr < SPI_FLASH_SIZE))   // 0x10000L =< nPhyAddr <  0x2000000L
	{
		*pnFrameNum = 1+ nPhyAddr/(_64KB << SHIFT);
	}
	#endif

	*pnFrameNum = nPhyAddr/(_64KB << SHIFT);
	

	return (1);
}


INT8U halSpiFlashGetFrameOffsetAddr(INT32U nFrameNum, INT32U *pnStartAddr)
{
    #if 0
	if (nFrameNum ==1)
	{
		*pnStartAddr = 0 ;
	}
	else if (nFrameNum >1)
	{
		*pnStartAddr = 0x10000 + (nFrameNum - 1) * _64KB;
		*pnStartAddr = (*pnStartAddr) << SHIFT;
	}	
	#endif

	*pnStartAddr = nFrameNum * _64KB;

	return (1);
	
}


INT8U halSpiFlashGetFrameSize(INT32U nFrameNum, INT32U *fsize)
{
    #if 0
	if (nFrameNum == 1)
	{
		*fsize = _64KB << SHIFT;
	}
	else 
	{
		*fsize = _128KB << SHIFT;
	}

	return (1);
	#endif

   *fsize = _64KB << SHIFT;
	
}

#endif


INT32S SPI_FLH_GetBlock(INT8U  *addr, SPI_FLH_BLOCK *block)
{
   INT32U image_size;

    if (((INT32U)addr - SPI_FLH_BASE_ADDR)>=SPI_FLASH_SIZE)   // the image written to spi-flash can not be larger than spi-flash size
    	{    
    	  printf("\r\n [SPI_FLH_GetBlock-1] ===== image size is larger than spi-flash ===== \n\r");
		  printf("\n\r [SPI_FLH_GetBlock-2] addr to be written is 0x%lx \n\r", addr);	    
		  printf("\n\r [SPI_FLH_GetBlock-3] image_size is 0x%lx byte \n\r", image_size);	

		  return -1;		  
    	}
	
			halSpiFlashGetFrameNum((INT32U)addr-SPI_FLH_BASE_ADDR, &block->num);	
			halSpiFlashGetFrameOffsetAddr(block->num, &block->start);			
			halSpiFlashGetFrameSize(block->num, &block->size);
			

			block->start += SPI_FLH_BASE_ADDR;
			block->end = block->start + block->size -1;

			printf("\n\r[SPI_FLH_GetBlock-1] block->start = 0x%lx \n", block->start );       // roger 20230801 add
			printf("\n\r[SPI_FLH_GetBlock-2] block->end = 0x%lx \n",   block->end   );       // roger 20230801 add
			printf("\n\r[SPI_FLH_GetBlock-3] block->num = 0x%lx \n",   block->num   );       // roger 20230801 add
			printf("\n\r[SPI_FLH_GetBlock-4] block->size = 0x%lx \n",  block->size  );       // roger 20230801 add	

			return 0;
		
}



/*check spi-flash memory block if it is empty?*/
INT32S SPI_FLH_Empty(INT8U *addr, INT32S len)
{
	INT8U *cp = (INT8U *)addr;
	while(len--)
	{
		if (*cp++ != 0xFF)
			return FALSE;
	}
	return TRUE;
}



