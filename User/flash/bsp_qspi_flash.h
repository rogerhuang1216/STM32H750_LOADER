/**
  ******************************************************************************
  * @file    bsp_qspi_flash.h

  *
  ******************************************************************************
  */ 



#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32h7xx.h"
#include <stdio.h>


/* Private typedef -----------------------------------------------------------*/
#define  sFLASH_ID                         0XEF4019     //W25Q256JVEM

/* QSPI Error codes */
#define QSPI_OK            ((uint8_t)0x00)
#define QSPI_ERROR         ((uint8_t)0x01)
#define QSPI_BUSY          ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED ((uint8_t)0x04)
#define QSPI_SUSPENDED     ((uint8_t)0x08)


/* W25Q256JV Micron memory */
/* Size of the flash */
#define QSPI_FLASH_SIZE            24     /* Address bus width accesses the entire memory space */
#define QSPI_PAGE_SIZE             256

/* QSPI Info */
typedef struct {
  uint32_t FlashSize;          /*!< Flash size */
  uint32_t EraseSectorSize;    /*!< sector size for erase operation */
  uint32_t EraseSectorsNumber; /*!< Number of Sectors for Erase Operation */
  uint32_t ProgPageSize;       /*!< Page size for programming operations */
  uint32_t ProgPagesNumber;    /*!< Number of pages for programming operations */
} QSPI_Info;

/* Private define ------------------------------------------------------------*/
/* command definition - beginning *******************************/
/** 
  * @brief  W25Q256JV configuration
  */  

#if 1
#define W25Q256JV_FLASH_SIZE                  0x2000000 /* 32MBytes, */
#define W25Q256JV_SECTOR_SIZE                 0x10000   /* 64KBytes, total 512 blocks */
#define W25Q256JV_SUBSECTOR_SIZE              0x1000    /* 4kBytes, total 8192 sectors */
#define W25Q256JV_PAGE_SIZE                   0x100     /* 256 bytes, total 131,072 pages */
#endif




#if 0
#define W25Q256JV_FLASH_SIZE                  0x2000000 /* 32MBytes, */
#define W25Q256JV_BLOCK_SIZE                  0x10000   /* 64KBytes, total 512 blocks */
#define W25Q256JV_SECTOR_SIZE                 0x1000    /* 4kBytes, total 8192 sectors */
#define W25Q256JV_PAGE_SIZE                   0x100     /* 256 bytes, total 131,072 pages */
#endif


#define W25Q256JV_DUMMY_CYCLES_READ           4
#define W25Q256JV_DUMMY_CYCLES_READ_QUAD      10

#define W25Q256JV_BULK_ERASE_MAX_TIME         250000
#define W25Q256JV_SECTOR_ERASE_MAX_TIME       3000

//#define W25Q256JV_SUBSECTOR_ERASE_MAX_TIME    800
//#define W25Q256JV_SUBSECTOR_ERASE_MAX_TIME    1600    // roger 202300829 modify to try to solve block erase random fail
#define W25Q256JV_SUBSECTOR_ERASE_MAX_TIME    2000    // roger 202300829 modify to try to solve block erase random fail


/** 
  * @brief  W25Q256JV instruction
  */  
/* reset operation */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* identification operation */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* read operation */
#define READ_CMD_4BYTE                       0x03
#define QUAD_INOUT_FAST_READ_CMD_4BYTE       0xEC

/* write operation */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* register operation */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11
/* program operation */
#define QUAD_INPUT_PAGE_PROG_CMD_4BYTE        0x34
#define EXT_QUAD_IN_FAST_PROG_CMD_4BYTE       0x12

/* erase operation */
#define SECTOR_ERASE_CMD_4BYTE                0x21 
#define BLOCK64K_ERASE_CMD_4BYTE              0xDC 
#define CHIP_ERASE_CMD                        0xC7

//#define PROG_ERASE_RESUME_CMD                 0x7A
//#define PROG_ERASE_SUSPEND_CMD                0x75

#define ENTER_4_BYTE_ADDR_MODE_CMD            0xB7

/* Status Register Flags */
#define W25Q256JV_FSR_BUSY                    ((uint8_t)0x01)    /*!< busy */
#define W25Q256JV_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define W25Q256JV_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */
/* command definition - end *******************************/


/* QSPI interface definition - beginning ****************************/
#define QSPI_FLASH                         QUADSPI
#define QSPI_FLASH_CLK_ENABLE()            __QSPI_CLK_ENABLE()

#define QSPI_MDMA_CLK_ENABLE()         		__HAL_RCC_MDMA_CLK_ENABLE()

#define QSPI_FORCE_RESET()             		__HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()           		__HAL_RCC_QSPI_RELEASE_RESET()

#define QSPI_FLASH_CLK_PIN                 GPIO_PIN_2                  
#define QSPI_FLASH_CLK_GPIO_PORT           GPIOB                       
#define QSPI_FLASH_CLK_GPIO_ENABLE()       __GPIOB_CLK_ENABLE()
#define QSPI_FLASH_CLK_GPIO_AF             GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK1_IO0_PIN             GPIO_PIN_8                
#define QSPI_FLASH_BK1_IO0_PORT            GPIOF                   
#define QSPI_FLASH_BK1_IO0_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO0_AF              GPIO_AF10_QUADSPI

#define QSPI_FLASH_BK1_IO1_PIN             GPIO_PIN_9                
#define QSPI_FLASH_BK1_IO1_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO1_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO1_AF              GPIO_AF10_QUADSPI

#define QSPI_FLASH_BK1_IO2_PIN             GPIO_PIN_7                
#define QSPI_FLASH_BK1_IO2_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO2_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO2_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK1_IO3_PIN             GPIO_PIN_6                
#define QSPI_FLASH_BK1_IO3_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO3_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO3_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_CS_PIN                 GPIO_PIN_6              
#define QSPI_FLASH_CS_GPIO_PORT           GPIOG                   
#define QSPI_FLASH_CS_GPIO_CLK_ENABLE()   __GPIOG_CLK_ENABLE()
#define QSPI_FLASH_CS_GPIO_AF             GPIO_AF10_QUADSPI





void QSPI_FLASH_Init(void);
uint8_t BSP_QSPI_Init(void);
uint8_t BSP_QSPI_Erase_Block(uint32_t BlockAddress);
uint8_t BSP_QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t BSP_QSPI_FastRead(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t BSP_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);

uint8_t BSP_QSPI_Erase_Chip(void);
static uint8_t QSPI_WriteEnable          (void);
static uint8_t QSPI_AutoPollingMemReady  (uint32_t Timeout);

uint32_t QSPI_FLASH_ReadDeviceID(void);
uint32_t QSPI_FLASH_ReadID(void);
uint32_t QSPI_FLASH_ReadStatusReg(uint8_t reg);
uint32_t QSPI_FLASH_WriteStatusReg(uint8_t reg,uint8_t regvalue);
void QSPI_Set_WP_High(void);
void QSPI_Set_WP_TO_QSPI_IO(void);
void QSPI_FLASH_Wait_Busy(void);
uint32_t QSPI_EnableMemoryMappedMode(void); // Enable direct mapped mode
uint8_t QSPI_ResetMemory(void);



#endif /* __SPI_FLASH_H */

