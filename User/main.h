#ifndef __MAIN_H
#define __MAIN_H

#include "stm32h7xx_hal.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_dfu.h"
#include "usbd_dfu_flash.h"

static void SystemClock_Config(void);



#if 0
#ifdef __cplusplus
 extern "C" {
#endif
#endif


/* Private define ------------------------------------------------------------*/




/* Exported types ------------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
//void TimingDelay_Decrement(void);
void Time_Update(void);
//void Delay(__IO uint32_t nTime);
//void Delay_10ms(__IO uint32_t nTime_10ms);





/* Exported constants --------------------------------------------------------*/

//#define USE_RTC           /* enable RTC  */


#ifndef INT32U


typedef unsigned char  		BOOLEAN;
typedef unsigned char  		INT8U;				/* Unsigned  8 bit quantity                 */
typedef char  				INT8S;				/* Signed    8 bit quantity                 */
typedef unsigned short  	INT16U;				/* Unsigned 16 bit quantity                 */
typedef short   			INT16S;				/* Signed   16 bit quantity                 */
typedef unsigned int  		INT32U;				/* Unsigned 32 bit quantity                 */
typedef int  				INT32S;				/* Signed   32 bit quantity                 */
typedef float          		FP32;				/* Single precision floating point          */
typedef double         		FP64;				/* Double precision floating point          */
typedef void				VOID;				/* void type								*/
typedef unsigned long long	INT64U;				/* 64 bits									*/
typedef long long			INT64S;				/* 64 bits 									*/
typedef unsigned long  		INT32UL;			/* Unsigned 32 bit, Roger 20150428 add		*/ 

#endif /* INT32U */


//#ifndef BYTE 
//#define BYTE				INT8S
//#endif

#ifndef CHAR 
#define CHAR				INT8S
#endif

#ifndef UBYTE 
#define UBYTE          		INT8U				/* ... to uC/OS V1.xx.  Not actually needed for ...   */
#endif

#ifndef UCHAR
#define UCHAR			INT8U
#endif

//#ifndef WORD
//#define WORD           		INT16S				/* ... uC/OS-II.                                      */
//#endif

#ifndef UWORD
#define UWORD          		INT16U
#endif

//#ifndef LONG
//#define LONG           		INT32S
//#endif

//#ifndef ULONG
//#define ULONG          		INT32U
//#endif

#ifndef uchar
#define uchar 			INT8U
#endif

#ifndef ushort
#define ushort 			INT16U
#endif

#ifndef uint
#define uint				INT32U
#endif

#ifndef ulong
#define ulong				unsigned long
#endif


/* Private variables ---------------------------------------------------------*/
typedef  void (*pFunction)(void);



#endif /* __MAIN_H */



