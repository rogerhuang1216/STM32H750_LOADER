#include "main.h"
#include <string.h>
#include <stdio.h>


#define PRINT_PACKET	0
#define ET_DEBUG		0


// roger 20150617 modify

#define FLH_LOADER_START               0x8000000L

#if 1
#define FLH_LOADER_SIZE                0x20000L       // 128KB
#endif


#if 0
#define FLH_LOADER_SIZE                0x40000L       // 256KB
#endif


#if 0
#define FLH_APP1_HEADER                0x8020000L
#define FLH_APP2_HEADER		           0x8020040L	
#define FLH_BORDER_START               0x8020140L	  // address to save border fune tune, roger 20151224 add, 20160308 modify
#endif


#if 1
#define SPI_FLH_APP1_HEADER            0xFFC0L       // roger 20230807 modify, indirect mode for update spi-flash contents
#define SPI_FLH_APP1_HEADER_B          0x1FD0000L    // roger 20230807 modify, indirect mode for update spi-flash contents
//#define SPI_FLH_APP1_HEADER_B          0x1FC0000L    // roger 20230807 modify, indirect mode for update spi-flash contents

#define DM_SPI_FLH_APP1_HEADER         0X9000FFC0L   // roger 20230808 add
#define DM_SPI_FLH_APP1_HEADER_B       0X91FD0000L   // roger 20230808 add
#endif


#if 1
#define FLH_APP1_HEADER                0x8040000L
#define FLH_APP2_HEADER		           0x8040040L	
#define FLH_BORDER_START               0x8040140L	  // address to save border fune tune, roger 20151224 add, 20160308 modify
#endif


#if 0
#define FLH_APP1_START                 0x8040000L     // 0x8040000L ~ 0x8100000L, 768KB
#define FLH_APP1_END		           0x8100000L     

#define FLH_APP3_START		           0x803FFC0L	  // TFTP Header Start Location
#define FLH_TFTP_START		           0x8040000L
#endif

//#define FLH_APP3_START		           0x803FFC0L	  // TFTP Header Start Location


#if 1
#define FLH_APP1_START                 0x8060000L     // 0x8060000L ~ 0x8100000L, 640KB
#define FLH_APP1_END		           0x8100000L     

#define FLH_APP3_START		           0x805FFC0L	  // TFTP Header Start Location
#define FLH_TFTP_START		           0x8060000L
#endif


#if 1
//#define SPI_FLH_APP1_START             0x0L           // 0x0L ~ 0x20000L, indirect mode ,reserve 128KB memory for APP-1, roger 20230801 add
//#define SPI_FLH_APP1_END		       0x20000L   

//#define SPI_FLH_APP1_START             0x10000L           // 0x0L ~ 0x20000L, indirect mode ,reserve 128KB memory for APP-1, roger 20230801 add
//#define SPI_FLH_APP1_END		       0x30000L   

//#define DM_SPI_FLH_APP1_START          0x90000000L    // 0x90000000L ~ 0x90020000L, indirect mode ,reserve 128KB memory for APP-1, roger 20230801 add
//#define DM_SPI_FLH_APP1_END		       0x90020000L   

#define DM_SPI_FLH_APP1_START          0x90010000L    // 0x90000000L ~ 0x90020000L, indirect mode ,reserve 128KB memory for APP-1, roger 20230801 add
//#define DM_SPI_FLH_APP1_END		       0x90030000L 
#define DM_SPI_FLH_APP1_END		       0x90210000L 



#endif


/*reserve 2MB SDRAM buffer for upload firmware image*/

#if 0
#define RAM_START				       0xD0000000L
//#define RAM_END					    0xD01FFFFFL    // size: 2MB
//#define RAM_END					    0xD0030000L    // size: 192KB, // Roger 20220918 Mark
#define RAM_END					       0xD00C8000L     // Http Server size 600KB, reserve 800KB as RAM buffer
#endif


#if 1
#define RAM_START				       0xD0400000L
//#define RAM_END					       0xD0500000L     // Http Server size 600KB, reserve 800KB as RAM buffer

#define RAM_END					       0xD0700000L         // Reserve 3MB as RAM buffer

#define BF_MCUFlash_START			   0x08020000L	       // store USB DFU data download from CubeProgrammer


#endif




//#define HEADER_RAM_START              0xD0040000L     // 20220918 Roger Mark                                 


#define VENDOR_MAGIC		          0x20002BB8L

/* firmware types */
#define HTML_TYPE			          "HTML"
#define FIRMWARE_TYPE		  	      "KERNEL"
#define CONFIGFILE_TYPE		  	      "CONFIG"
#define LOADER_TYPE			  	      "LOADER"
#define CERT_TYPE				      "CERT"
#define LINUX_KERNEL_TYPE		      "LINUX"
#define LINUX_FS_TYPE			      "EXT2"
#define CALI_TYPE				      "CALI"
#define HEADER_TYPE                   "HEADER"          // roger 20150617 add 



/*
*	For STM32F429IGT6 MCU 1MB Flash
*
*     Sector  0 0x0800 0000 +----------------------------------- + 
*			 	   	                  |            16 Kbytes     (LOADER)                
*                                     |                                                          
*     Sector  1 0x0800 4000 +------------------------------------+
*                                     |            16 Kbytes     (LOADER)
*                                     |
*     Sector  2 0x0800 8000 +------------------------------------+
*                                     |            16 Kbytes     (LOADER)
*                                     |
*     Sector  3 0x0800 C000 +------------------------------------+
*                                     |            16 Kbytes     (LOADER) 
*                                     |
*     Sector  4 0x0801 0000 +------------------------------------+
*                                     |            64 Kbytes     (LOADER) 
*                                     |
*     Sector  5 0x0802 0000 +------------------------------------+
*                                     |           128 Kbytes     (LOADER)
*                                     |
*     Sector  6 0x0804 0000 +------------------------------------+
*                                     |           128 Kbytes     (APP HEADER, Borader Finetune Value)
*                                     |
*     Sector  7 0x0806 0000 +------------------------------------+
*                                     |           128 Kbytes     (APP-1)
*                                     |
*     Sector  8 0x0808 0000 +------------------------------------+
*                                     |           128 Kbytes     (APP-2)
*                                     |
*     Sector  9 0x080A 0000 +------------------------------------+
*                                     |           128 Kbytes     (APP-3)
*                                     |
*     Sector 10 0x080C 0000 +------------------------------------+
*                                     |           128 Kbytes     (APP-4)
*                                     |
*     Sector 11 0x080E 0000 +------------------------------------+
*                                     |           128 Kbytes     (APP-5)
*                                     |
*               0x080F FFFF +------------------------------------+
*
*
*    LOADER can jump to APP according to selection
*
*
*/



#ifndef image_pos_t

typedef struct image_pos_t
{
	const CHAR *name;	/*image name*/
	INT32U		subid;	/*sub-id*/
	INT32U		base;	/*base address*/
	INT32U		size;	/*size of image*/
	
}IMAGE_POS_T;

typedef struct tftp_image_pos_t
{
	const CHAR *name;	/*image name*/
	INT32U		subid;	/*sub-id*/
	INT32U		base;	/*base address*/
	INT32U		size;	/*size of image*/
	
}TFTP_IMAGE_POS_T;

#endif


extern const IMAGE_POS_T ImagePosition[];
extern const TFTP_IMAGE_POS_T tftp_ImagePosition[];		// Roger 20151109 add


#ifndef CONFIG_COMMANDS
#define CFG_CMD_NET			0x00000080						/* bootp, tftpboot, rarpboot	*/
#define CFG_CMD_PING			0x00010000						/* ping support	*/
#define CONFIG_COMMANDS 		(CFG_CMD_PING | CFG_CMD_NET)	
#endif


#define	LOAD_ADDR		0x00800000	/* default load address	*/



#ifndef MAX
#define MAX(a, b)	(a>b?a:b)
#endif	
#ifndef MIN
#define MIN(a, b)	(a<b?a:b)
#endif
#ifndef NULL
#define NULL	0
#endif
#ifndef TRUE
#define	TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif


#ifndef __BYTE_ORDER
#ifndef __LITTLE_ENDIAN
#ifndef __BIG_ENDIAN
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#endif /* __BIG_ENDIAN */
#endif /* __LITTLE_ENDIAN */
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif /* __BYTE_ORDER */


#if __BYTE_ORDER == __BIG_ENDIAN	 //#ifdef __BIG_ENDIAN	

#define ___swab16(x) \
	((unsigned int)( \
	(((unsigned int)(x) & (unsigned int)0x00ffU) << 8) | \
	(((unsigned int)(x) & (unsigned int)0xff00U) >> 8) ))

#define ___swab32(x) \
	((unsigned long)( \
	(((unsigned long)(x) & (unsigned long)0x000000ffUL) << 24) | \
	(((unsigned long)(x) & (unsigned long)0x0000ff00UL) <<	8) | \
	(((unsigned long)(x) & (unsigned long)0x00ff0000UL) >>	8) | \
	(((unsigned long)(x) & (unsigned long)0xff000000UL) >> 24) ))

	#define htonl(x) (x)
	#define htons(x) (x)
	#define ntohl(x) (x)
	#define ntohs(x) (x)
#else
	#define ___swab16(x) (x)
	#define ___swab32(x) (x)
	
	#define ____swab16(x) \
		((unsigned int)( \
		(((unsigned int)(x) & (unsigned int)0x00ffU) << 8) | \
		(((unsigned int)(x) & (unsigned int)0xff00U) >> 8) ))

	#define ____swab32(x) \
		((unsigned long)( \
		(((unsigned long)(x) & (unsigned long)0x000000ffUL) << 24) | \
		(((unsigned long)(x) & (unsigned long)0x0000ff00UL) <<	8) | \
		(((unsigned long)(x) & (unsigned long)0x00ff0000UL) >>	8) | \
		(((unsigned long)(x) & (unsigned long)0xff000000UL) >> 24) ))

//	#define htonl(x) ____swab32(x)
//	#define ntohl(x) ____swab32(x)
//	#define htons(x) ____swab16(x)
//	#define ntohs(x) ____swab16(x)
#endif





//for axd
#define PACK				__packed
#define PRE_PACK		__packed
#define POST_PACK 
#define __inline__		__inline



/*define HW event*/
#define UART_EVENT		0x01
#define TIMER_EVENT		0x02
#define ETH_EVENT		0x04
#define FW_EVENT		0x08
#define CAM_EVENT                0x10 

//#define EVENT_MASK  		0x0F		// roger 20170525 mark
#define EVENT_MASK  		0x1F		// roger 20170525 add

extern INT32U sys_event;
extern INT32S LoadBufAddr, LoadBufSize;


#define MAX_EVENT_NUM			4
#define MAX_ADDR_UPLOAD_FILE	0x80000000

#define LEN_PLATFORMID		    32

#define LEN_IMAGETYPE			8
#define LEN_SHA1				20
#define LEN_CRC				    4
#define MAXNAMELEN				48  /* max length of hostname or name for auth */




#define IMG_CHECK_NONE		    0x00
#define IMG_CHECK_CRC			0x01
#define IMG_CHECK_SHA1			0x02

#define CRC_LEN                     4
#define SHA1_LEN                   20
#define MAGIC_LEN                   4


//=====		Roger 20150428 start for define SDRAM option  				=====//

#define SDRAM_OPTION      			WINBOND_9825G6EHX2

#define ESMT_M12L256X2 				1      // 16M x 16bits x 2 = 64MB     (32bits mode)
#define WINBOND_9825G6EHX2		    2	   // 16M x 16bits x 2 = 64MB     (32bits mode)	
#define ISSI_IS42S324				3      //  4M x 32bits x 1 = 16MB     (32bits mode)
#define ESMT_M12L256		 		4      // 16M x 16bits x 1 = 32MB     (16bits mode)    
#define MICRON_MT48LC16M16A2        5      // 16M x 16bits x 1 = 32MB     (16bits mode) 


//=====		Roger 20150428 end for define SDRAM option   				=====//


//=====		Roger 20150519 start for define IMAGE option  				=====//

#define IMAGE_OPTION      			LOADER

#define LOADER 						1      
#define APP							2	 


//=====		Roger 20150519 end for define IMAGE option  				=====//




//=====		 Roger 20150428 start for define memory Test Item     			=====//



#define MEMORY_POOL_START_ADDR                 ((uint32_t)0xD0000000)          // SDRAM Memory Addr   
																	    // MICRON_MT48LC16M16A2 addressable range 
																	    // 0xD0000000 ~ 0xD0FFFFFF = 16M, 16bits mode 
																	    // LCD BUFFER_OFFSET    ((uint32_t)0x177000)  

#define BUFFER_POOL_START_ADDR                  ((uint32_t)0xD0040000)          // SDRAM Buffer for xmodem receive   




// 32 bit mode
#define WORD_TEST_0x55555555				1
#define WORD_TEST_0xAAAAAAAA				1
#define WORD_TEST_16252000          		1

// 16 bit mode
#define SHORT_TEST_0x5555					1
#define SHORT_TEST_0xAAAA					1	
#define SHORT_TEST_65534					1 
#define SHORT_TEST_0x0000					1 

// 8 bit mode
#define CHAR_TEST_0x55						1
#define CHAR_TEST_0xAA						1
#define CHAR_TEST_254						1	 


//=====		Roger 20150428 end for define memory Test Item     			=====//



//=====		Roger 20150428 start for define return value     				=====//

#define WORD_TEST_0x55555555_PATTERN        1
#define WORD_TEST_0xAAAAAAAA_PATTERN       2
#define WORD_TEST_16252000_PATTERN            3
#define SHORT_TEST_0x5555_PATTERN			4
#define SHORT_TEST_0xAAAA_PATTERN			5	
#define SHORT_TEST_65534_PATTERN			6 
#define CHAR_TEST_0x55_PATTERN				7
#define CHAR_TEST_0xAA_PATTERN			8
#define CHAR_TEST_254_PATTERN				9 
#define SHORT_TEST_0x0000_PATTERN		     10               // roger 20151221 add

//=====	      Roger 20150428 end for define return value     					=====//

#ifndef _IMG_TIME

typedef __packed struct _IMG_TIME {
	INT16U  year;
	INT8U   month;
	INT8U   day;
	INT8U   hour;
	INT8U   min;
	INT8U   sec;

} IMG_TIME;

#endif


#ifndef _IMG_HEADER

typedef __packed struct _IMG_HEADER {
	
	CHAR  	  platform_id[LEN_PLATFORMID];	// platform ID	
	INT32U 	  version;						// version
	CHAR 	  image_type[LEN_IMAGETYPE];		// Image Type
	INT32U 	  image_size;					// Image Data Size
	INT32U 	  entry_addr;					// Entry Point Address
	//INT32U 	flash_addr;					// Data load to Flash Address
	IMG_TIME  build_time;						// Image Creation Timestamp
	INT8U 	  check_type;					// 0:crc, 1:SHA1
	INT8U 	  compress[4];					// zip / uzip
	
} IMG_HEADER;



typedef __packed struct _TFTP_IMG_HEADER {
	
	CHAR  	  platform_id[LEN_PLATFORMID];	// platform ID	
	INT32U 	  version;						// version
	CHAR 	  image_type[LEN_IMAGETYPE];		// Image Type
	INT32U 	  image_size;					// Image Data Size
	INT32U 	  entry_addr;					// Entry Point Address
	//INT32U 	flash_addr;					// Data load to Flash Address
	IMG_TIME  build_time;						// Image Creation Timestamp
	INT8U 	  check_type;					// 0:crc, 1:SHA1
	INT8U 	  compress[4];					// zip / uzip
	
} TFTP_IMG_HEADER;

#endif 


struct SYS_VENDOR_DEFINE
{
	INT32U  magic;              	/*magic string for version control*/
	INT8U   mac[2][6];          	/*MAC 0/1 address */
	INT8U   lan_ip[4];          	/*LAN IP Address */
	INT8U   lan_mask[4];
	INT8U   lan_gw[4];
	/*Upper field will be used in loader, so do not chenge */
	/*it relationship. (loader.c) */
	INT8U   dhcps_pool[2];              /*start and stop ID */
	INT8U   wins_ip[4];
	CHAR    host_name[MAXNAMELEN];
	INT8U   dod;                        			/*0 disable dial on demand, 1 enable*/
	INT32U  ppp_idle_time;              		/*ppp idle time, 0 is always on*/
	CHAR    service_name[MAXNAMELEN];   	/*pppoe service name*/
	INT8U   primary_DNS[4];
	INT8U   secondary_DNS[4];
	INT8U   remote_manage;              		/*remote management 0 disable, 1 enable*/
	INT8U   ping;                       			/*ping from WAN */
	INT16U  http_port;                  			/*http server port*/
	INT16U  tftp_port;                  			/*tftp server port*/
	CHAR    smb_group[16];              		/*SMB group name */
	CHAR    smb_name[16];               		/*smb host name */
	CHAR    essid[MAXNAMELEN];
	CHAR    code;						/*file system default code page */
	INT8U   boot_sys;                   			/*0:ucOSII, 1:Linux, 2: RAM test boot */

	CHAR	hw_version[16];
	CHAR	sw_version[16];
	INT32U	freq_start;
	INT32U	freq_end;

};

extern INT32S image_check(INT32S type, INT8U *start, INT32S size, INT8U *sha1_crc);


