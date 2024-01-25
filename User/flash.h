#ifndef _FLASH_H_
#define _FLASH_H_


// STM32F429IGT6
#define FLH_BASE_ADDR    0x08000000

//#define FLASH_SIZE		 0x00100000	/* 1M bytes */

//#define FLH_ERASE_ALL     0xffffffffL

///////////////////////////////////////////////////////////////////////////////////
//
// Winbond SPI Flash, W25Q256JV, 32MB (for direct addressing mode)
//
///////////////////////////////////////////////////////////////////////////////////


// Winbond W25Q256

#define SPI_FLH_BASE_ADDR          0x90000000L    // roger 20230810, [direct memory - 0x9000_0000 = indirect memroy]  
#define SPI_FLASH_SIZE		       0x02000000L	  /* 32M bytes */
#define SPIFLH_ERASE_ALL           0xFFFFFFFFL

#define SPI_FLASH_Block_B0         0x0L           /*!< Block Number    0  */     // 64K Bytes per Blocks
#define SPI_FLASH_Block_B0_S15X    0xF000L        /*!< Block Number    0  */     // Sector 15   // 64K Bytes per Blocks, 4K Bytes per Sectors
#define SPI_FLASH_Block_B1X        0x10000L       /*!< Block Number    1  */     // 64K Bytes per Blocks
#define SPI_FLASH_Block_B2X        0x20000L       /*!< Block Number    2  */     // 64K Bytes per Blocks
#define SPI_FLASH_Block_B3X        0x30000L       /*!< Block Number    1  */     // 64K Bytes per Blocks
#define SPI_FLASH_Block_B4X        0x40000L       /*!< Block Number    2  */     // 64K Bytes per Blocks
//              .
//              .
//              .
#define SPI_FLASH_Block_B509_S0X   0x1FD0000L     /*!< Sector Number 509  */     // Sector 0   // 64K Bytes per Blocks, 4K Bytes per Sectors
#define SPI_FLASH_Block_B510_S0X   0x1FE0000L     /*!< Sector Number 510  */                                         .
#define SPI_FLASH_Block_B511X      0x1FF0000L     /*!< Sector Number 511  */     // can not erase, root cause is ambigous

// parameter memory map setting

#define Bootime_P                  0x1FE0000L     // addr 0x1FE0000L, data 0xXX (0~0xFE), 0~254 sec, default value 0x5 (5 sec) 




#if 0
#define SPI_FLASH_Block_0      ((uint32_t)0x0      ) /*!< Block Number    0  */     // 64K Bytes per Blocks
#define SPI_FLASH_Block_1      ((uint32_t)0x10000  ) /*!< Block Number    1  */     // 64K Bytes per Blocks
//                                     .
//                                     .
//                                     .
#define SPI_FLASH_Block_510    ((uint32_t)0x1FE0000) /*!< Sector Number 510  */                                         .
#define SPI_FLASH_Block_511    ((uint32_t)0x1FF0000) /*!< Sector Number 511  */     // can not erase, root cause is ambigous
#endif


#define SHIFT 			 0

#define _16KB 			(16*1024)
#define _64KB 			(64*1024)
#define _128KB 			(128*1024)




typedef struct flash_sector
{
    INT32U  	start	;  	/* start address of flash sector */
    INT32U		end		;   /* end address of flash sector*/	
    INT32U		num		;  	/* sector number*/	
    INT32U  	size	;   /* size of each sector */
}FLH_SECTOR;



typedef struct spiflash_block
{
    uint32_t  	start  ;   /* start address of spi-flash block */
    uint32_t	end    ;   /* end address of spi-flash sector  */	
    uint32_t	num    ;   /* spi-flash block number           */	
    uint32_t  	size   ;   /* size of each spi-flash block     */
}SPI_FLH_BLOCK;





#endif  /* _FLASH_H_ */

