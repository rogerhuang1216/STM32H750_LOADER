/**
  *********************************************************************
  * @file    main.c
  * @author  roger huang
  * @version V5.0
  * @date    2023/10/16
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
 
/*
*************************************************************************
*                             Header 
*************************************************************************
*/ 
/* FreeRTOS header */
//#include "FreeRTOS.h"
//#include "task.h"
/* Fire Board header */
#include "board.h"
#include "string.h"
//#include <cm_backtrace.h>
//#include "diskio.h"
//#include "Backend_RGBLED.h" 
#include "cmd.h"   
//#include "misc.h"
#include "main.h"
#include "loader.h"
#include "misc.h"
#include "./sdram/bsp_sdram.h"
#include "./flash/bsp_qspi_flash.h"
#include "./systick/bsp_SysTick.h"
#include "./mpu/bsp_mpu.h" 
#include "flash.h" 
#include "./CV2_board/CV2_board.h"
#include "stm32h7xx_it.h"



volatile uint8_t Rxflag=0;
uint8_t ucTemp;



/* required hardfault tracer definition  */
#define HARDWARE_VERSION               "V1.0.0"
#define SOFTWARE_VERSION               "V11"

/**************************** Task Handler  ********************************/
/* 

 The task handle is a pointer used to point to a task. When the task is created, it has a task handle
 In the future, if we want to operate this task, we need to pass this task handle. If it is our own task to operate ourselves, then
 This handle can be NULL.

 */

/********************************** Kernel Object Handler *********************************/
/*
* Semaphores, message queues, event flag groups, and software timers are all kernel objects. If you want to use these kernels
* The object must be created first, and a corresponding handle will be returned after the creation is successful. In fact, it is a pointer, follow-up I
* We can manipulate these kernel objects through this handle.
*
* To put it bluntly, the kernel object is a kind of global data structure, through which we can realize the communication between tasks,
* various functions such as event synchronization between tasks. As for the realization of these functions, we call the functions of these kernel objects
* to complete
*/

/******************************* Global Variables ************************************/
/*

 */

const INT8S *ProjectName = 				"STM32H750XBH6 FIRE_V2_EVK_LOADER"   		;
const INT8S *BootVersion = 				"LOADER-V11"  								;
const INT8S *ReleaseDate = 				"2023/10/16"                                ;
const INT8S *Author 	 = 				"Roger Huang"                               ;
const INT8S PlatformID[LEN_PLATFORMID]=	"Fire Challenger V2"                        ;


INT32S LoadBufAddr=RAM_START, LoadBufSize=RAM_END-RAM_START;
INT32S TftpBufAddr=FLH_APP3_START;			// Roger 20151109 add for tftp upgrade mcu flash
INT32U sys_event=0;
struct  SYS_VENDOR_DEFINE SYS_Vendor;	

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

//#define SYSTEMTICK_PERIOD_MS  10         // 10ms interrupt
#define SYSTEMTICK_PERIOD_MS  1         // 1ms interrupt

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static __IO uint32_t TimingDelay ;
static __IO uint32_t TimingDelay_10ms ;

__IO uint32_t 	LocalTime = 0 ; 	/* this variable is used to create a time reference incremented by 10ms */
     uint32_t 	timingdelay ;
//SD_Error 			error;


// Read ID storage location

#if 1
__IO extern uint32_t DeviceID_indir ;
__IO extern uint32_t FlashID_indir ;
#endif

uint8_t key2_pb = 0 ;       		// roger 	20230911 add
uint8_t key1_pb = 0 ;       		// roger 	20230915 add


USBD_HandleTypeDef USBD_Device;


/*

************************************************************************************************
*                              Functions
************************************************************************************************
*/

//static void GUI_Thread_Entry(void* pvParameters); /* Test_Task Implementation */
//static void DEBUG_Thread_Entry(void* parameter);
//static void MPU_Config(void);
//void vApplicationStackOverflowHook(TaskHandle_t xTask,char * pcTaskName);

extern INT32UL *initMemoryPool(INT32UL *addr);
extern VOID Disable(void);
extern VOID Enable(void);
void STM32H750_ISR_Setup (void);
extern void Delay_indir(__IO uint32_t nCount);
extern INT32S Do_Qspimemmap(INT32S argc, INT8S *argv[]);
extern INT32S Boot(IMG_HEADER *hdr, INT32S addr, INT32S wait);
extern VOID SelectBoot(INT32S type, INT32S count, IMG_HEADER **image_hdr, INT32U *image_base);
extern INT32U ListAvailableFirmware(CHAR *type[], INT32U image_base[], IMG_HEADER *image_hdr[]);



/**
  * @brief  Disable NAND FLASH
  * @param  none
  * @param  none
  * @retval none
  */

#if 0

static void NAND_CE_INIT(void)
{
    /* define a structure of GPIO_InitTypeDef */
    GPIO_InitTypeDef GPIO_InitStruct;
    /* enable gpio clock */	
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /* select the gpio to control */															   
    GPIO_InitStruct.Pin = GPIO_PIN_9;	
    /* set the output type of the pin to push-pull output*/
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;      
    /* set gpio as pullup */
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    /* set gpio as fast speed */    
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
    /* call the library function to initialize GPIO using above configured GPIO_InitStructure */
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    /* disable wi-fi*/
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_9,GPIO_PIN_SET);  
}

#endif

#if 1


void STM32H750_ISR_Setup (void) 					// roger 20230620 add
{
NVIC_InitTypeDef  NVIC_Usart1Structure;
//NVIC_InitTypeDef  NVIC_EthStructure;
//NVIC_InitTypeDef  NVIC_SpiStructure;
//NVIC_InitTypeDef  NVIC_CanStructure;



// [1] SPI Interrupt Config



// [2] UART Interrupt Config

NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					// roger 20170526 add
//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	


USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

/* Enable and set USART1 Interrupt priority */
NVIC_Usart1Structure.NVIC_IRQChannel = USART1_IRQn;      			// 37      
NVIC_Usart1Structure.NVIC_IRQChannelPreemptionPriority = 3;
//NVIC_Usart1Structure.NVIC_IRQChannelPreemptionPriority = 2;
//NVIC_Usart1Structure.NVIC_IRQChannelPreemptionPriority = 4;

NVIC_Usart1Structure.NVIC_IRQChannelSubPriority = 0;
NVIC_Usart1Structure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_Usart1Structure); 




// [3] CAN Interrupt Config



// [4] Ethernet Interrupt Config



}

#endif


#if 0

/**
  * @brief  Inserts a delay time (not accurate timing).
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
  
static void Delay( __IO uint32_t nCount)
{
  for (; nCount != 0; nCount--);
}

#endif



#if 1
void Delay(__IO uint32_t nTime)
{
  /* Capture the current local time */
  timingdelay = LocalTime + nTime;  

   /* wait until the desired delay finish */  
   while(timingdelay > LocalTime)
   {	  
   }
}
#endif




void BSP_Init(void);    /* initialize board related resource */
/***********************************************************************
  * @ function name: BSP_Init
  * @ function description: Board-level peripheral initialization, all initialization on the board can be placed in this function
  * @ parameter: none   
  * @ return: none
  *********************************************************************/
void BSP_Init(void)
{
	SCB->CACR|=1<<2;   // Force D-Cache to write through, if it is not enabled, various problems may be encountered in actual use	  

	/* system clock initialize to 400MHz */
	/* set SDRAM to Normal, disable sharing mode, write-through mode */  
	//Board_MPU_Config(0,MPU_Normal_WT,0xD0000000,MPU_32MB);
	/* Set AXI RAM to Normal type, disable sharing, write-through mode */ 
	//Board_MPU_Config(1,MPU_Normal_WT,0x20000000,MPU_128KB);
	//Board_MPU_Config(2,MPU_Normal_WT,0x00000000,MPU_64KB);
	//Board_MPU_Config(3,MPU_Normal_WT,0x24000000,MPU_512KB);
	//Board_MPU_Config(4,MPU_Normal_WT,0x08000000,MPU_2MB);
	//Board_MPU_Config(5,MPU_Normal_WT,0x90000000,MPU_16MB);
	//MPU_Config();	
	
	
	/*
	* The STM32 interrupt priority group is 4, that is, 4 bits are used to indicate the preemption priority, and the range is: 0~15
	* Priority grouping only needs to be grouped once. If there are other tasks that need to be interrupted in the future,
	* Use this priority grouping uniformly, do not group again, avoid by all means.
	*/
	 
	 
  	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	STM32H750_ISR_Setup();   // roger 20230726 add ISR config for STM32H750

	/* HW BSP initializati, for ex LED/UART/LCD...etc */
	
	SDRAM_Init();           // roger 20230623 activate SDRAM 
	initMemoryPool((INT32UL *)MEMORY_POOL_START_ADDR);	    // roger 20230726, specify PKBuf0 starting addr on SDRAM
	
	QSPI_FLASH_Init();      // roger 20230710 add

    // QSPI_EnableMemoryMappedMode();     // roger 20230711 add, set up SPI Flash to direct addressing mode  *****
	
	/* LED initialization */
	LED_GPIO_Config();	

    /* Configure Key Button */
    BSP_PB_Init(BUTTON_TAMPER, BUTTON_MODE_EXTI);	// roger 20230911 add, // interrupt mod   // GPIO PC13
    BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_EXTI);	// roger 20230915 add, // interrupt mod   // GPIO PA0
	
	/* usart initialization */
    UARTx_Config();
  
    printf("\r\n prompt \r\n"
           "If it is a 4.3/5-inch RGB screen, the touch chip used may be GT1151QM \r\n"
           "If the touch or display is abnormal, please confirm the type of screen touch chip used,"
           "Modify the macro LCD_TOUCH_IC_GT1151QM in the bsp_i2c_touch.h file \r\n\r\n");

  
   	
  /* Basic Timer initialization	*/
	// TIM_Basic_Init();      // roger 20230630 add, must disable, otherwise will hang, root casue tbd

    printf("\r\n [01] TIM_Basic_Init Complete \r\n");  
	
    // Disable NAND FLASH
    // NAND_CE_INIT();

	printf("\r\n [02] Disable NAND Flash Complete \r\n");  

    
	/* wm8978 player initialization	*/
  #if 0	
	if (wm8978_Init()==0)
  {
    printf("WM8978 chip is not detected !!!\n");
  }
  #endif	
  
  #if 0
	#ifdef USE_RTC
	RTC_CLK_Config();
	if ( HAL_RTCEx_BKUPRead(&Rtc_Handle,RTC_BKP_DRX) != 0x32F2)
	{
		/* Set Time and Date */
		RTC_TimeAndDate_Set();
	}
	else
	{
		/* Check Power Reset status */
		if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
		{
			printf("\r\n power reset occurred....\r\n");
		}
		/* Check for external reset */
		else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
		{
			printf("\r\n external reset occurred....\r\n");
		}
		printf("\r\n not require to reconfig RTC....\r\n");    
		/* Enable PWR clock */
		__HAL_RCC_RTC_ENABLE();
		/* PWR_CR:DBF is set to 1 to enable access to RTC, RTC backup registers and backup SRAM access */
		HAL_PWR_EnableBkUpAccess();
		/* Wait RTC APB register synchronization */
		HAL_RTC_WaitForSynchro(&Rtc_Handle);
	} 
    #endif
  #endif	
	
	MODIFY_REG(FMC_Bank1_R->BTCR[0],FMC_BCRx_MBKEN,0); // turn-off FMC_Bank1, otherwise LCD will flick.
	
  /* hardfault tracer initialization */ 
//  cm_backtrace_init("LOADER", HARDWARE_VERSION, SOFTWARE_VERSION);
  
}


/**
  * @brief  System Clock config
  *         system Clock config: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 480000000 (CPU Clock)
	*            HCLK(Hz)             = 240000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  100MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  100MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  100MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  100MHz)
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


VOID PrintFirmwareInfo(INT32U num, IMG_HEADER *hdr[])
{
	INT32S i=0;
	
	printf("\n\r Please select available kernel to boot, ESC (exit)\n");
	for (i=0; i<num; i++)
	{
		printf("\n\r (%d) %s version %d, size %ld, ",i, hdr[i]->image_type, hdr[i]->version, hdr[i]->image_size);
		printf("\n\r build date %d/%d/%d %d:%d:%d\n",
		hdr[i]->build_time.year,hdr[i]->build_time.month, hdr[i]->build_time.day,
		hdr[i]->build_time.hour,hdr[i]->build_time.min, hdr[i]->build_time.sec);
	}
}




/*****************************************************************
  * @brief  Main function
  * @param  none
  * @retval none
  * @note   1st: challenger board V2 HW initialization 
            2nd: create APP application task
            3rd: run FreeRTOS, start multi-task scheduling
  ****************************************************************/
int main(void)
{	
  INT8U line[CMD_LEN]={0}; 
  INT32S boot_sel = -1;  
  INT32U event, i;  
  uint8_t bootcnt;
  INT8S *argv[MAX_TOKEN];
  INT32S argc;
  cmd_tbl_t *cmdtp;
  INT32U num,image_base[5]={0,0,0,0,0};    	// roger 20150716 modify, support 5 app 
  IMG_HEADER *image_hdr[5]={NULL,NULL,NULL,NULL,NULL};  	// roger 20150716 modify, support 5 app 

  CHAR *type[3]={FIRMWARE_TYPE,LINUX_KERNEL_TYPE,HEADER_TYPE};    // type[0]: FIRMWARE_TYPE
   																  // type[1]: LINUX_KERNEL_TYPE
   																  // type[2]: HEADER_TYPE  
  //bootcnt = 5;								                      // LOADER to APP Delay Time

  uint8_t bootime_rb[1];				                          // roger 20230829 add	
  key2_pb = 0 ; 
  key1_pb = 0 ;   


  #if 0
  BaseType_t xReturn = pdPASS;  /* define a creation information return value, the default is pdPASS */
  #endif

  #if 0
  STM32H750_ISR_Setup();        // roger 20230620 add ISR config for STM32H750 Fire Challenger V2 EVB
  #endif

  printf("\n\r ***         Enter Main Function          *** \n\r"); 
  
	/* Enable I-Cache */
	SCB_EnableICache(); 
	/* Enable D-Cache */
	SCB_EnableDCache();
	/* The system clock is initialized to 480MHz */
	SystemClock_Config();	
	
	SysTick_Init();
	
    /* challenger board V2 HW initialization */
    BSP_Init();  

  
  printf("\n\r");
  printf("\n\r *****************************************************************");
  printf("\n\r *****  Project Name  : %s   ",ProjectName );
  printf("\n\r *****  FW Version	: %s   ",BootVersion );
  printf("\n\r *****  Release Date  : %s   ",ReleaseDate ); 
  printf("\n\r *****  Author		: %s   ",Author      ); 
  //printf("\n\r *****  PlatformID    : %s   ",PlatformID[LEN_PLATFORMID]);   
  printf("\n\r *****  PlatformID: ");
    for (int k = 0; k < LEN_PLATFORMID; k++) 
	{
        printf("%c", PlatformID[k]);
    }
  printf("\n");	
  printf("\n\r *****************************************************************\n\r");

  #if 1 
  /* Get Flash Device ID */
  DeviceID_indir = QSPI_FLASH_ReadDeviceID();
  printf("\r\n [Bootime] [01] QSPI_FLASH_DeviceID is 0x%04X,\r\n", DeviceID_indir);   
  Delay_indir( 200 );
  /* Get SPI Flash ID */
  FlashID_indir = QSPI_FLASH_ReadID();	  
  printf("\r\n [Bootime] [01] QSPI_FLASH_ID is 0x%04X,\r\n", FlashID_indir);	  
  QSPI_Set_WP_High();
  // write status register	  
  QSPI_FLASH_WriteStatusReg(1,0X00);
  QSPI_FLASH_WriteStatusReg(2,0X00);
  QSPI_FLASH_WriteStatusReg(3,0X61);
  printf("\r\n [Bootime] [01] FlashID is 0x%X,	  Manufacturer Device ID is 0x%X\r\n", FlashID_indir, DeviceID_indir);
  printf("\r\n [Bootime] [01] Flash Status Reg1 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(1));	  
  printf("\r\n [Bootime] [01] Flash Status Reg2 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(2));
  printf("\r\n [Bootime] [01] Flash Status Reg3 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(3));
  QSPI_Set_WP_TO_QSPI_IO();
  #endif

  while (BSP_QSPI_FastRead(bootime_rb, (uint32_t) Bootime_P, 1)!=QSPI_OK);

  if (bootime_rb[0]==0xFF)
  	 bootcnt = 5;
  else if (bootime_rb[0]<2)
  	 bootcnt = 2;  
  else
     bootcnt = bootime_rb[0];


  #if 1
  #if (IMAGE_OPTION==LOADER) 
	  /*list all available kernel*/
      
	  Do_Qspimemmap(1,NULL); 					// roger 20170507 add
  
	  num = ListAvailableFirmware(type, image_base, image_hdr);
	  printf("\n\r ListAvailableFirmware return value = %lx\n",num);

      //while(1);
	  
	  if (num)
			  {
				  PrintFirmwareInfo(num, image_hdr);
				  /*waiting loop, wait 5 seconds to boot up*/
				  if (bootcnt)
				  {
					  printf("\n\r wait...%d",bootcnt);
					  
					  for (i=1;i<=bootcnt;i++)	
					  {
						  Delay(1000);      /*delay 1000ms */	// 1000ms x 5        = 5sec						  
						  if (kbhit())
						  {
							  printf("\n\r stop\n");
							  boot_sel = getch();
							  if (boot_sel >= '0')
								  boot_sel = atoh((CHAR *)&boot_sel);	  				// ascii to hex, 
							  printf("\n\r The chosen kernel is [%lx]\n",boot_sel);		// roger, 20140814 add
							  break;
						  }
						  
                          // roger 20230915 add

                          else if ((key2_pb==1)&&(key1_pb==1))  // KEY2(SW3) && KEY1(SW1) are pressed
                          {
						     printf("\n\r KEY2 && KEY 1 are pressed, enter USB DFU (Device Mode)\n");
							 boot_sel = -2;  // keep at loader mode	
							 LED_CYAN;

                             #if 0			// 自製 bootloader, 使用STM32Cube_FW_H7_V1.2.0 範例程式

     						 /* Init Device Library */
     						 USBD_Init(&USBD_Device, &DFU_Desc, 0);
     
     						 /* Add Supported Class */
     						 USBD_RegisterClass(&USBD_Device, USBD_DFU_CLASS);
     
     						 /* Add DFU Media interface */
     						 USBD_DFU_RegisterMedia(&USBD_Device, &USBD_DFU_Flash_fops);
     
     						 /* Start Device Process */
     						 USBD_Start(&USBD_Device);
     
     						 HAL_PWREx_EnableUSBVoltageDetector();
							                            
							 /* Run Application (Interrupt mode) */
							 while (1)
							  {
							  }
							  
							 #endif

                             #if 1          // 使用 built-in bootloader,利用 FW 切換 

					  		 void (*SysMemBootJump)(void);		  /* 声明一个函数指针 */
					  		 __IO uint32_t BootAddr = 0x1FF09800; /* STM32H7的系统BootLoader地址 */            // 經跳線後, 0x1FF09800 辨識到不明裝置, 重新插拔後可以辨識, 但是CubeProgrammer仍然無法連線
					  		 //__IO uint32_t BootAddr = 0x1FF00000; /* STM32H7的系统BootLoader地址 */		        // 經跳線後, 0x1FF09800 辨識到不明裝置			 

							 /* Disable ֹI-Cache */
  							 SCB_DisableICache();

							 /* Disable D-Cache */
							 SCB_DisableDCache();
					  	 
					  		 /* 关闭全局中断 */  
					  		 //__disable_irq(); 
					  	 
					  		 /* 关闭滴答定时器，复位到默认值 */
					  		 SysTick->CTRL = 0;
					  		 //SysTick->LOAD = 0;
					  		 //SysTick->VAL = 0;
					  	 
					  		 /* 设置所有时钟到默认状态，使用HSI时钟 */
					  		 //HAL_RCC_DeInit();
					  	 
					  		 /* 关闭所有中断，清除所有中断挂起标志 */
					  		 //for (i = 0; i < 8; i++)
					  		 //{
					  		 //	 NVIC->ICER[i]=0xFFFFFFFF;
					  		 //	 NVIC->ICPR[i]=0xFFFFFFFF;
					  		 //}	  
					  	 
					  		 /* 使能全局中断 */
					  		 //__enable_irq();
					  	 
					  		 /* 跳转到系统BootLoader，首地址是MSP，地址+4是复位中断服务程序地址 */
					  		 SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));
					  	 
					  		 /* 设置主堆栈指针 */
					  		 __set_MSP(*(uint32_t *)BootAddr);
					  		 
					  		 /* 在RTOS工程，这条语句很重要，设置为特权级模式，使用MSP指针 */
					  		 __set_CONTROL(0);
					  	 
					  		 /* 跳转到系统BootLoader */
					  		 SysMemBootJump(); 

                             #if 0
                            
							 for (int DFU_TEST=0; DFU_TEST<=10; DFU_TEST++) 
							 {
							    printf("\n\r DFU_TEST [%ld]\n",DFU_TEST);	   // roger, 20231016 add
							 }					 

							 #endif
							 
							 while (1)
							 {
							 }

							 #endif
							  
                          }

                          // roger 20230908 add

                          //else if (key2_pb==1)  // only KEY2(SW3) is pressed
                          else if ((key2_pb==1)&&(key1_pb==0))  // only KEY2(SW3) is pressed
                          {
						      printf("\n\r KEY2 is pressed, stay at Loader Mode\n");
							  boot_sel = -2;  // keep at loader mode	
							  LED_YELLOW;
                              break;
                          }

                          //else if (key1_pb==1)  // only KEY1(SW1) is pressed
                          else if ((key2_pb==0)&&(key1_pb==1))  // only KEY1(SW1) is pressed                          
                          {
						      printf("\n\r KEY1 is pressed, start to copy MCU Flash 0x0802_0000 data to SPI-Flash\n");
							  boot_sel = -2;  // keep at loader mode	
							  LED_BLUE;
							  Do_SpiFlash_Update(1,NULL); 
                              break;
                          }						  

 						  else
 						  { 
						      //printf("\n\r Not any KEY is pressed\n"); 						  
 							  LED_WHITE;
 							  //break;
 						  }


						  #if 1 
						    if ((bootcnt-i)<10)                                 // countdown < 10 sec 
						      printf("\r\r\r wait...%d ",bootcnt-i);
							else if (((bootcnt-i)>=10)&&((bootcnt-i)<100))      // 10 < countdown < 100 sec
						      printf("\r\r wait...%d ",bootcnt-i);	
							else                                                // countdown > 100 sec
						      printf("\r wait...%d ",bootcnt-i);								
						  #endif	                  
						  
					  }   /*end for loop*/ 
					  
				  } 		  
				  
				  /*show available firmware*/
				  if (boot_sel>=0 && boot_sel<num)	   // The boot image selected by "Manual"
				  {
					  printf("\n\r *****  Boot	  *****\n"); 
					  Boot(image_hdr[boot_sel], image_base[boot_sel], 0); 
				  } 						  
				  else if (boot_sel==-1)			   // The boot image selected by "Automatic (decided by builddate)"   
				  {
					  printf("\n\r *****  SelectBoot  *****\n"); 
					  /*not input any thing, select newest one to boot*/
					  SelectBoot(1, num, image_hdr, image_base);
				  }   
  
			  }   
  
  #endif
  #endif

  

  HAL_FLASH_Unlock();    // roger 20230712 add
 
  LocalTime=0;    

  #if 0
  
   /* AppTaskCreate */
  xReturn = xTaskCreate((TaskFunction_t )GUI_Thread_Entry,  /* Task Entry Function */
                        (const char*    )"gui",				/* Task Name */
                        (uint16_t       )5*1024,  			/* Task Size */
                        (void*          )NULL,				/* Task Entry Function Parameters */
                        (UBaseType_t    )10, 				/* Task Priority */
                        (TaskHandle_t*  )NULL);				/* Task Handler Pointer */  

//	           xTaskCreate((TaskFunction_t )DEBUG_Thread_Entry,  	/* Task Entry Function */
//                        (const char*    )"DEBUG_Thread_Entry",	/* Task Name */
//                        (uint16_t       )2*1024,  				/* Task Size */
//                        (void*          )NULL,					/* Task Entry Function Parameters */
//                        (UBaseType_t    )15, 						/* Task Priority */
//                        (TaskHandle_t*  )NULL);					/* Task Handler Pointer */
 
  /* Start Task Scheduling */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* run task, start scheduler */
  else
    return -1;  

  #endif
  


	printf("\n\r FIT CPD-M>");		             
	printf("\n\r sys_event = %d\n",sys_event);


  #if 1	 // uart command line test ok, roger 20150415 add
  
  while(1)
	  { 		
	    #if 1
		  if (sys_event==0)
			  continue;
		#endif  
		  /*else hw event*/   
		  
		  Disable();	   // disable interrupt
		  event = sys_event;
		  
		  //printf("\n\r event = %d\n",event);   // roger 20230620 add
		  
		  /*clear all event*/
		  sys_event &= ~EVENT_MASK;
		  Enable();
  
		// UART_Event
		  
		  if (event & UART_EVENT)
		  {
			  // printf("\n\r Has UART_EVENT \n\r"); 
		  
			  /*get character from UART*/
			  if ((readline(line))!=0)
			  {
				  // printf(" Enter readline \n\r"); 
				  parse_token(line,&argc, argv);
				  if (argc==0)
				  {
					  printf("\n\r FIT CPD-M>");
					  continue;
				  }
				  cmdtp = find_cmd(argv[0]);
				  if (cmdtp)
				  {
					  if (argc < cmdtp->minargs)
						  printf ("\n\r Usage:\n%s\n", cmdtp->usage);
					  else if (cmdtp->func(argc, argv)<0)
						  printf ("\n\r Usage:\n%s\n", cmdtp->usage);							  
				  }
				  else
					  printf("\n\r unknown command %s!\n",argv[0]);
				  
				  printf("\n\r FIT CPD-M>");
				  
			  }   /*end of process line command*/
		  }  

		  
	  }   /*end while*/
  
    #endif

  
  ////  while(1);   /* Normally it will not be executed here */  

  
}

//extern void GUI_Startup(void);

/**********************************************************************
  * @ function name: gui_thread_entry
  * @ function description: gui_thread_entry task
  * @ parameter: none   
  * @ return: none 
  ********************************************************************/

#if 0

static void GUI_Thread_Entry(void* parameter)
{	
//  uint8_t CPU_RunInfo[400];		// save task runtime info.
  printf("Fire Challenger V2 emXGUI Demo\n\r");
  /* Executing this function will not return */
	
	GUI_Startup();

  while (1)
  {
    LED1_ON;
    printf("Test_Task Running,LED1_ON\r\n");
    vTaskDelay(500);   /* delay 500 tick */
    
    LED1_OFF;     
    printf("Test_Task Running,LED1_OFF\r\n");
    vTaskDelay(500);   /* delay 500 tick */
  }
}

#endif


#if 0

static void DEBUG_Thread_Entry(void* parameter)
{	
	char tasks_buf[512] = {0};
	
  while (1)
  {

    vTaskDelay(2000);   /* delay 500 tick */
//	  uint16_t test_status = 0;
//		test_status =  disk_status(0);
//		printf("************************ test_status %d ************************\r\n",test_status);
{
	memset(tasks_buf, 0, 512);

	strcat((char *)tasks_buf, "Task Name Run Count Utilization\r\n" );

	strcat((char *)tasks_buf, "---------------------------------------------\r\n");

	/* displays the amount of time each task has spent in the Running state

	* in both absolute and percentage terms. */

	vTaskGetRunTimeStats((char *)(tasks_buf + strlen(tasks_buf)));

	strcat((char *)tasks_buf, "\r\n");
	printf("%s\r\n",tasks_buf);
	
}
	memset(tasks_buf, 0, 512);

	strcat((char *)tasks_buf, "Task name  Running status  Priority  Remaining stack  Task number\r\n" );

	strcat((char *)tasks_buf, "---------------------------------------------\r\n");


{
	vTaskList((char *)(tasks_buf + strlen(tasks_buf)));

	strcat((char *)tasks_buf, "\r\n---------------------------------------------\r\n");


	strcat((char *)tasks_buf, "B : Block, R : Ready, D : Delete, S : Suspend\r\n");
	printf("%s\r\n",tasks_buf);
}
  }
}

#endif



#if 0

static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as Device not cacheable 
     for ETH DMA descriptors */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256B;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER6;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as Cacheable write through 
     for LwIP RAM heap which contains the Tx buffers */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x30044000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_16KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER7;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

#endif


/*
void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                     char * pcTaskName)
{
	printf(" Heap Overflow! Cheak : %s \r\n",pcTaskName);
	while(1);
}
*/


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

#if 0			// 99-freeRTOS+emXGUI_5_7_inch

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

#endif

  
#if 1         // USB_Use external FLASH to simulate U disk（W25Q256）
	
	/**
	  * @brief	System Clock 配置
	  * 		system Clock 配置如下: 
		*			 System Clock source  = PLL (HSE)
		*			 SYSCLK(Hz) 		  = 480000000 (CPU Clock)
		*			 HCLK(Hz)			  = 240000000 (AXI and AHBs Clock)
		*			 AHB Prescaler		  = 2
		*			 D1 APB3 Prescaler	  = 2 (APB3 Clock  120MHz)
		*			 D2 APB1 Prescaler	  = 2 (APB1 Clock  120MHz)
		*			 D2 APB2 Prescaler	  = 2 (APB2 Clock  120MHz)
		*			 D3 APB4 Prescaler	  = 2 (APB4 Clock  120MHz)
		*			 HSE Frequency(Hz)	  = 25000000
		*			 PLL_M				  = 5
		*			 PLL_N				  = 192
		*			 PLL_P				  = 2
		*			 PLL_Q				  = 4
		*			 PLL_R				  = 2
		*			 VDD(V) 			  = 3.3
		*			 Flash Latency(WS)	  = 4
	  * @param	None
	  * @retval None
	*/
	
	{
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	  /**  Enable power configuration updates
	  */
	  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	  /** Configure the main LDO output voltage
	  */
	  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
	
	  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
		  
	  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
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
	
	  /* Initialize UART, USB clocks */
	  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USB;
	  PeriphClkInitStruct.PLL3.PLL3M = 25;
	  PeriphClkInitStruct.PLL3.PLL3N = 384;
	  PeriphClkInitStruct.PLL3.PLL3P = 2;
	  PeriphClkInitStruct.PLL3.PLL3Q = 8;
	  PeriphClkInitStruct.PLL3.PLL3R = 2;
	  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
	  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
	  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
	  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_PLL3;
	  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
	  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	  {
		LED_RED;
	  }
	  /** Enable USB Voltage detector 
	  */
	  HAL_PWREx_EnableUSBVoltageDetector();
	}
#endif
}



/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
void Time_Update(void)
{
  LocalTime += SYSTEMTICK_PERIOD_MS;
}








/********************************END OF FILE****************************/
