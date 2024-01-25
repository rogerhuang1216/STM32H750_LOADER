/**
  *********************************************************************
  * @file    miscu.c
  * @author  roger
  * @version V1.0
  * @date    2023-07-10
  * @brief   
  *********************************************************************
  * @attention
  *
  * HW Platform: Fire Challenger V2
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
                               Header                                   
*************************************************************************
*/

 
#include "main.h"
#include "loader.h"
#include "cmd.h" 
#include "sha1.h"
#include <ctype.h>
#include "flash.h"
//#include "stm32f4xx.h"
//#include "stm32f4xx_flash.h" 
#include "./flash/bsp_qspi_flash.h"
#include "./led/bsp_led.h"


/* should compatible to syscfg.h */		//Franky modify to support FW info query
extern const INT8S PlatformID[LEN_PLATFORMID];
//extern INT32S Cert_Idx;

///extern INT32S FLH_Write(INT8U  *addr, INT8U  *pdata, INT32U len);
//extern INT32S FLH_Erase(INT8U  *addr, INT32S len);
extern INT32S xmodemReceive(INT8U *dest, INT32S destsz);
extern INT32S image_check(INT32S type, INT8U *start, INT32S size, INT8U *sha1_crc);

INT32S SPI_FW_Upgrade(INT8U *addr, INT32S total);
extern INT32S SPI_FLH_GetBlock(INT8U  *addr, SPI_FLH_BLOCK *block);
extern INT32S SPI_FLH_Empty(INT8U *addr, INT32S len);
extern INT32U bootcnt;
extern void Delay_indir(__IO uint32_t nCount);
extern INT32S Do_Qspimemmap(INT32S argc, INT8S *argv[]);
extern INT32S Do_QSPI_ResetMemory(INT32S argc, INT8S *argv[]);



// Read ID storage location

#if 1
__IO extern uint32_t DeviceID_indir ;
__IO extern uint32_t FlashID_indir ;
#endif




/* define loader/app position & size */ // pls pay attention to, FW upgrade use indirect addressing, memory dump / comparison use direct addressing (ps. 0x9000_0000L) 

const IMAGE_POS_T ImagePosition[]=   
{
	{LOADER_TYPE    ,		0  ,	FLH_LOADER_START       ,  FLH_LOADER_SIZE                             },   // 128KB for Bootloader
	{HEADER_TYPE    ,		1  ,    DM_SPI_FLH_APP1_HEADER ,  0x40			                              },   //   64B for APP-1 header
	{FIRMWARE_TYPE  , 		1  ,	DM_SPI_FLH_APP1_START  ,  DM_SPI_FLH_APP1_END-DM_SPI_FLH_APP1_START   },   //   2MB for APP-1
	{NULL           ,		0  ,    0                      ,  0                                           }
};


const TFTP_IMAGE_POS_T tftp_ImagePosition[]=
{
	{FIRMWARE_TYPE  ,       0  ,	FLH_APP3_START         ,  0x40				                          },   //  64B	
	{NULL           ,	    0  ,    0                      ,  0                                           }
};




/*
 *===========================================================
 *  Function Name: Get_Info
 *
 *  Description: Get firmware image information, 
 *	
 *  Input: image type or NULL (list all)
 *                 
 *  Output: output message in a buffer
 *
 *  Return value: 
 *
 *  History: 
 *
 * 		roger huang	modify 01/25/2010
 *===========================================================
*/ 
const CHAR *subname[4]={"","Root CA","Host CA","Private Key"};
const CHAR *checktype[3]={"none","crc","sha1"};


#if 1

INT32S Get_Info(CHAR *image_type, CHAR *info_buf, INT32S buf_len)
{
	IMG_HEADER hdr;
	//INT8U *addr = NULL;
	CHAR *p, *end;//, tmp_buf[1024] = {0}, tmp_str[8] = {0};
	INT32S i = 0;
	IMAGE_POS_T *image;

	/*clear buffer*/	
	memset(info_buf, 0, buf_len);
	p = info_buf;
    end = info_buf + buf_len - 1;
    
	/* to upper case */
	if (image_type != NULL)
	{	
		while (i < strlen(image_type))
		{
			image_type[i] = toupper(image_type[i]);
			i ++;
		}
		image_type[i] = 0;
	}
	image = (IMAGE_POS_T *)ImagePosition;
	while(image->name!=NULL)
	{
		
		if (image_type == NULL ||
			strncmp((CHAR *)image_type, image->name, strlen(image->name)) == 0)
		{	
			memcpy((CHAR *)&hdr, (CHAR *)(image->base+MAGIC_LEN), sizeof(IMG_HEADER));
			/* check magic string */
			if (*(INT32U *)image->base == VENDOR_MAGIC &&
				strncmp((CHAR *)hdr.platform_id, (CHAR *)PlatformID, LEN_PLATFORMID)==0) 
			{
				p += snprintf(p, end-p,"%s image info --\r\n", hdr.image_type);
				p += snprintf(p, end-p,"\tPlatform ID : %s, Version : %lx\r\n"	, hdr.platform_id, hdr.version);
				p += snprintf(p, end-p,"\tImage Type : %s %s, Image Size : %ld(0x%lx)\r\n"	, 
					hdr.image_type, subname[image->subid], hdr.image_size, hdr.image_size);
				p += snprintf(p, end-p,"\tEntry Addr : 0x%lx, Flash Addr : 0x%lx\r\n", hdr.entry_addr, image->base);
				p += snprintf(p, end-p,"\tBuild Time : %d/%d/%d %d:%d:%d\r\n", 
					hdr.build_time.year	, hdr.build_time.month,
					hdr.build_time.day	, hdr.build_time.hour,
					hdr.build_time.min	, hdr.build_time.sec);
				p += snprintf(p, end-p,"\tCheck Type : %s\r\n\r\n",checktype[hdr.check_type]);
			}
		}
		image++;	/*found next one*/
	}	/*end of while*/
	return 1;
}


#endif



/*
 *===========================================================
 *  Function Name: Cal_Crc
 *
 *  Description  : 
 *	
 *  Input        : None
 *                 
 *  Output       : None
 *
 *  Return value : 
 *
 *  History      : 
 *
 * 		roger huang	modify 01/25/2010
 *===========================================================
*/

#if 1

VOID Cal_Crc(INT8U *start_addr, INT32S len, INT8U *crc_val)
{
	CRC_CONTEXT crc;
	INT8U crc_buf[1024];
	INT32S i = 0, index = 0;
	
	crc_starts(&crc);
	while (i < len) 
	{
		memset(crc_buf, 0, sizeof(crc_buf));
		if ((len - i) < 1024) 
			index = len - i;
		else
			index = 1024;

		memcpy(crc_buf, &start_addr[i], index);
		crc_update(&crc, (INT8U *)crc_buf, index);
		i += index;
	}				
	crc_finish(&crc, (unsigned long *)crc_val);
}

#endif


/*
 *===========================================================
 *  Function Name: FW_CheckHeader
 *
 *  Description  : 
 *	
 *  Input        : None
 *                 
 *  Output       : None
 *
 *  Return value : 
 *
 *  History      : 
 *
 * 		roger huang	modify 01/25/2010
 *===========================================================
*/

#if 0

INT32S FW_CheckHeader(INT8U *addr, INT32S len)
{
	IMG_HEADER *phdr;
	//INT32U size, src, dest;
	INT32U size;
	INT8U *psha1_crc, crc_val[CRC_LEN];
	extern const INT8S PlatformID[LEN_PLATFORMID];	
	
	phdr = (IMG_HEADER *)addr;
	
	/* check platform ID */
	if (strncmp((INT8S*)phdr->platform_id, PlatformID, LEN_PLATFORMID)!=0) 
	{		
		extern INT8U TFTPS_CrcChk;
		printf("Platform ID check fail ! \n");
	
		/* calculate crc */
		if (TFTPS_CrcChk == 1)
		{
			Cal_Crc(addr, len, crc_val);
			printf("[CRC : %02x %02x %02x %02x] \r\n", crc_val[0], crc_val[1], crc_val[2], crc_val[3]);
			TFTPS_CrcChk = 0;
		}
		return -1;
	}		
	
	/* check crc or sha1 */
	size = sizeof(IMG_HEADER)+phdr->image_size;
	psha1_crc = (INT8U *)(addr + size);
	if (image_check(phdr->check_type, (INT8U *)addr, size, psha1_crc) != 0)
	{
		printf("CRC or SHA1 check fail ! \nParadom>");
		return -1;
	}	
	return 0;
}


#endif


/*
 *===========================================================
 *  Function Name: BuildDateCheck
 *  Description  : compare two image, and return the new one's 
 *				   position
 *	 
 *  Input        : type 0: return older one, 1 return newest one
 *                 IMG_HEADER[2]
 * 
 *  Return value : start of firmware image
 *
 *  History      : 
 * 		roger huang modify 01/20/2010
 *===========================================================
*/
INT32U BuildDateCheck(INT32S type, IMG_HEADER *image_hdr0, IMG_HEADER *image_hdr1,
	INT32U base0, INT32U base1)
{
	/*compare build year*/	
	if (image_hdr0->build_time.year<image_hdr1->build_time.year)
		return (type?base1:base0);
	else if (image_hdr0->build_time.year>image_hdr1->build_time.year)
		return (type?base0:base1);
			
	/*compare build month*/	
	if (image_hdr0->build_time.month<image_hdr1->build_time.month)
		return (type?base1:base0);
	else if (image_hdr0->build_time.month>image_hdr1->build_time.month)
		return (type?base0:base1);
		
	/*compare build day*/	
	if (image_hdr0->build_time.day<image_hdr1->build_time.day)
		return (type?base1:base0);
	else if (image_hdr0->build_time.day>image_hdr1->build_time.day)
		return (type?base0:base1);
		
	/*compare build hours*/	
	if (image_hdr0->build_time.hour<image_hdr1->build_time.hour)
		return (type?base1:base0);
	else if (image_hdr0->build_time.hour>image_hdr1->build_time.hour)
		return (type?base0:base1);
		
	/*compare build minutes*/	
	if (image_hdr0->build_time.min<image_hdr1->build_time.min)
		return (type?base1:base0);
	else if (image_hdr0->build_time.min>image_hdr1->build_time.min)
		return (type?base0:base1);
		
	/*compare build second*/	
	if (image_hdr0->build_time.sec<image_hdr1->build_time.sec)
		return (type?base1:base0);
	else if (image_hdr0->build_time.sec>image_hdr1->build_time.sec)
		return (type?base0:base1);
	/*all equal, update first one*/	
	return base0;
}


/*
 *===========================================================
 *  Function Name: GetUpgradePostiion
 *  Description  : supporting function for firmware upgrade,
 *				   check dat and get the oldest one to update
 *	 
 *  Input        : firmware type
 *                 
 *  Return value : start address of firmware
 *
 *  History      : 
 * 		           roger 2010/01/20 create 
 *                 roger 2015/06/23 modify
 *===========================================================
*/

/* confirm APP image info. on the MCU or SPi-Flash*/

INT32U GetFirmwarePosition(CHAR *type, INT32U ver)
{
	IMG_HEADER hdr,*image_hdr[3]={NULL,NULL,NULL};
	IMAGE_POS_T *image;
	INT32U count=0,image_base[3]={0,0,0};

    //===================================================================
	Do_Qspimemmap(1,NULL); 
    //===================================================================

    printf("\n\r [GetFW-1] ImagePosition Mapping Start \n");
	image = (IMAGE_POS_T *)ImagePosition;
    printf("\n\r [GetFW-2] ImagePosition Mapping End \n");

	
	while(image->name!=NULL)
	{
		/*match type*/
		if (strncmp(type, image->name, strlen(image->name)) == 0)
		{	
			memcpy((CHAR *)&hdr, (CHAR *)(image->base+MAGIC_LEN), sizeof(IMG_HEADER));		
			      printf("\n\r [GetFW-3] Memory Copy Complete\n");
			/*if certification file*/
			//if (strcmp(type, CERT_TYPE) == 0)    		// roger 20150623 mark
			if (strcmp(type, FIRMWARE_TYPE) == 0)	   	// roger 20150623 add 
			{
			      printf("\n\r [GetFW-4] Enter type = FIRMWARE_TYPE\n");
			      printf("\n\r [GetFW-5] ver = %d\n",ver);	
			      printf("\n\r [GetFW-6] image->subid -- 0x%lx\n",image->subid);

				//while(1);  
				
				if (ver == image->subid)  // miscu.c "const IMAGE_POS_T ImagePosition[]"
					{				      // check if the "the version of image header" is the same as "subid of const IMAGE_POS_T ImagePosition[]"
					printf("\n\r [GetFW-7] GetFirmwarePosition -- 0x%lx\n",image->base);   

					//while(1);

					switch (image->base)	// the initial addr value should be spi-flash address 0x0 (indirect mode)
					  {
						  case DM_SPI_FLH_APP1_START:
								image->base=DM_SPI_FLH_APP1_START;
								break;	
			              #if 0
						  case FLH_APP2_HEADER:
								image->base=FLH_APP2_START;
								break;			
						  case FLH_APP3_HEADER:
								image->base=FLH_APP3_START;
								break;	  
						  case FLH_APP4_HEADER:
								image->base=FLH_APP4_START;
								break;
						  case FLH_APP5_HEADER:
								image->base=FLH_APP5_START;
								break;
						  #endif		
					  }				
					return image->base;
					}
				image++;	/*move to next on*/
				continue;
			}
			
			/* check magic string */
			#if 0
			if (*(INT32U *)image->base == VENDOR_MAGIC &&
				strncmp((CHAR *)hdr.platform_id, (CHAR *)PlatformID, LEN_PLATFORMID)==0)
            #endif
				   
		    #if 1 		   
			if (((*(INT32U *)image->base) & 0x2FFE0000)== 0x20000000)
            #endif			
			{
				//image_hdr[count]=(IMG_HEADER *)(image->base+MAGIC_LEN);    // roger 20150615 mark
				image_base[count]=image->base;
				count++;
				printf("\n\r [GetFW-8] GetFirmwarePosition -- count = %d\n", count);  
			}
			else	/*no image or incorrect, return this address*/
				{
				 printf("\n\r [GetFW-9] no image or incorrect -- image->base = 0x%lx\n",image->base);  
				 return image->base;
				}  
		}	
		image++;	/*move to next on*/
	}	/*end of while*/
	
	if (count == 1)
		{
		printf("\n\r [GetFW-10] only one image -- image->base = 0x%lx\n",image_base[0]);  
		return image_base[0]; /*only one image*/
		}
	/*get oldest one position*/
	return BuildDateCheck(0, image_hdr[0],image_hdr[1],image_base[0],image_base[1]);
}	/*end of get position*/


/*
 *===========================================================
 *  Function Name: FW_Upgrade
 *  Description  : upgrade related firmware, certificate file, configure file, etc
 *
 *  Input        : start address of load firmware
 *                 length of load firmware
 *  Output       : None
 *
 *  Return value : Result from server
 *
 *  History      : 
 *
 *     Franky   Create   04/06/2009
 * 	   roger huang	modify 01/20/2010
 *===========================================================
*/

#if 0

INT32S FW_Upgrade(INT8U *addr, INT32S total)
{

      printf ("\n\r [1] start FW_Upgrade \n");
	  
#if 1

	IMG_HEADER *phdr;	
	//INT32S size, src, dest, offset, retval = 0, len, magic_len=0;
	INT32S size, src, dest, offset, retval = 0, len;
	//INT8U *psha1_crc, *buf = NULL, *start;
//	INT8U *buf1 = NULL;                                                              // roger 20160315 add
//	INT8U *buf2 = NULL;                                                              // roger 20160315 add
	INT8U *psha1_crc, *start;
	//INT8U *buf1 = NULL, *buf2 = NULL, *buf3 = NULL, *buf4 = NULL;
	//INT8U *psha1_crc, crc_val[CRC_LEN], *start;       			// roger  20150511 add

	extern const INT8S PlatformID[LEN_PLATFORMID];	
	FLH_SECTOR sec;
	//FLASH_Status interflash_status;
	INT32S hdr_src;

do_again:
		
	phdr = (IMG_HEADER *)addr;
	/* check platform ID */
	if (strncmp((INT8S*)phdr->platform_id, PlatformID, LEN_PLATFORMID)!=0) 
	{		
	      #if 0
		extern INT8U TFTPS_CrcChk;
		printf("\n\rPlatform ID check fail ! \n");
	
		/* calculate crc */
		if (TFTPS_CrcChk == 1)
		{
			Cal_Crc(addr, len, crc_val);
			printf("\n\r[CRC : %02x %02x %02x %02x] \r\n", crc_val[0], crc_val[1], crc_val[2], crc_val[3]);
			TFTPS_CrcChk = 0;
		}
		#endif
		
		retval = -1;
		goto end_upgrade; 
	}
	
	/* check crc or sha1 */
	size = sizeof(IMG_HEADER)+phdr->image_size;
	psha1_crc = (INT8U *)(addr + size);


	#if 1        // check image crc value
	if (image_check(phdr->check_type, (INT8U *)addr, size, psha1_crc) != 0)
	{
		retval = -2;
		goto end_upgrade; 
	}	
      #endif

	
	if (strcmp((INT8S*)phdr->image_type, LOADER_TYPE) == 0 ||
		strcmp((INT8S*)phdr->image_type, CERT_TYPE) == 0 ||
		strcmp((INT8S*)phdr->image_type, CALI_TYPE) == 0 ||
		strcmp((INT8S*)phdr->image_type, CONFIGFILE_TYPE) == 0)
	{
		if (memcmp((INT8S*)phdr->compress, "zip", 4) == 0)
		{
			/*unzip to another memory?*/
			retval = -5;
			goto end_upgrade; 				 
		}
	}	/*end if */
	
	
	if (strcmp((INT8S*)phdr->image_type, LOADER_TYPE) == 0 ||
		strcmp((INT8S*)phdr->image_type, FIRMWARE_TYPE) == 0 ||   // roger 20150525 add
		strcmp((INT8S*)phdr->image_type, CONFIGFILE_TYPE) == 0)
		{
		src = (INT32U)addr+sizeof(IMG_HEADER);
             printf("\n\r [2] Image src addr is 0x%lx \n", src);   
		}
	else
		{
		src = (INT32U)addr;
             printf("\n\r [2] Image src addr is 0x%lx \n", src);   
		}

	/*get real programming position*/  // check memory space of "APP-1~5" 
	dest = GetFirmwarePosition((CHAR *)phdr->image_type, phdr->version);
      printf("\n\r [2] Image dest addr is 0x%lx \n", dest);   

      // write APP1~5 Header to 0x8020000, roger 20150716 add	  
			  
	#if 1		 
		  
		if (strcmp((INT8S*)phdr->image_type, FIRMWARE_TYPE) == 0)
		{
			switch (dest)			// mapping the "header of APP" to the dedicated addr which will be upadted  
			  {
			  case FLH_APP1_START:
				hdr_src=FLH_APP1_HEADER;
				break;  		
			  }
		}

		//printf("\n\r [2] Image dest addr is 0x%lx \n", dest);
		printf("\n\r [2] APP Header Destination Addr is 0x%lx \n", hdr_src);

		// check APP1~5 HEADER status at Flash Sector 5, roger 20150716 add
   	      #if 0
		
			if (FLH_Header_Empty((INT8U *)FLH_APP1_HEADER, 128)== FALSE)
			{
			   printf("\n\r [TFTP-1] Flash Sector 5 is not empty\n"); 	
		
			   buf1 = malloc(64);
			   buf2 = malloc(64);
		
				 if ((buf1==NULL)||(buf2==NULL))
				 {
					 retval = -7;
					 goto end_upgrade; 
				 }	
				 
				 // Backup sector 5 "APP header" to SRAM, if the previous header exist
				 memcpy(buf1, (INT8U *)FLH_APP1_HEADER, 64);
				 memcpy(buf2, (INT8U *)FLH_APP2_HEADER, 64);
				 printf("\n\r [2] FLH_APP_HEADER backup to SRAM complete\n");		
			  }	
		
   	      #endif

			 
             // erase sector 5

///		if ((FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3))!=FLASH_COMPLETE)
			printf("\n\r [2] FLASH_Erase Sector 5 fail\n");

             // write the header of "updated APP" to sector5 
		 
///		if (FLH_Write((INT8U *)hdr_src, (INT8U *)addr, 64)!= 0)
			{
				printf("\n\r [2] Enter retval = -4\n");
				printf("\n\r [2] APP Header Destination Addr is 0x%lx \n", hdr_src);
				printf("\n\r [2] APP Header Source Addr = 0x%lx \n", addr);	
				retval = -4;
				goto end_upgrade; 
			}
	  
		printf("\n\r [2] APP Header Destination Addr is 0x%lx \n", hdr_src);
		printf("\n\r [2] APP Header Source Addr = 0x%lx \n", addr);	

			
		// write back the other header (not updated) in SRAM to sector 5 

		
	#endif


	if (dest == 0)
	{
		retval = -13;
		goto end_upgrade; 
	}
	
	/*move pointer to next one*/	
	if (phdr->check_type == IMG_CHECK_SHA1)
		size += SHA1_LEN;
	else if (phdr->check_type == IMG_CHECK_CRC)
		size += CRC_LEN;
	total -= size;
	addr += size;
	
	/* loader, and config will not write header to flash  */
	if ((strcmp((INT8S*)phdr->image_type, LOADER_TYPE) != 0) &&
		(strcmp((INT8S*)phdr->image_type, CONFIGFILE_TYPE) != 0))
	{	
		//magic_len = MAGIC_LEN;
	}
	else
		size = phdr->image_size;
																		                    // dest: reflash starting addr 
	printf("\n\r [3] Start to write %s image to 0x%lx, len=%ld \n", phdr->image_type, dest, size);    // size: reflash size
	
	start = (INT8U *)dest;
       
      printf("\n\r [3] Start = 0x%lx \n", start);
	
	// dest += magic_len;	     // disable write vendor magic,  roger 20150525 

      
	#if 1
	//// FLASH_Unlock();
	#endif

/////////////////////////////////////////////////////////////////////////////

	printf("\n\r [3] sec.start = 0x%lx \n", sec.start); 
	printf("\n\r [3] sec.end = 0x%lx \n", sec.end); 
	printf("\n\r [3] sec.num = 0x%lx \n", sec.num); 
	printf("\n\r [3] sec.size = 0x%lx \n", sec.size); 


      #if 1

	while (size>0)
	{	   
		if (FLH_GetSector((INT8U *)dest, &sec)<0)
		{
			retval = -6;
			goto end_upgrade; 
		}

		offset = (INT32U)dest - sec.start;
		printf("\n\r [4] dest addr = 0x%lx \n", dest); 
		printf("\n\r [4] sec.start addr = 0x%lx \n", sec.start); 		
             printf("\n\r [4] offset addr = 0x%lx \n", offset); 

		len = sec.size - offset;
             printf("\n\r [4] len = 0x%lx \n", len); 		
		printf("\n\r [4] sec.size = 0x%lx \n", sec.size); 
		
		
		if (len > size)
			len = size;
		
		if (FLH_Empty((INT8U *)dest, len)== FALSE)      // if this sector is put last version image, need to erase then program
		{	
                   #if 1
		
			/*not empty, need backup this sector?*/
			if ((dest != sec.start)||(len != sec.size))
			{	
				printf("\n\r [5] dest addr= 0x%lx \n", dest); 
				printf("\n\r [5] sec.start addr= 0x%lx \n", sec.start);		
				printf("\n\r [5] len = 0x%lx \n", len); 		
				printf("\n\r [5] sec.size = 0x%lx \n", sec.size); 			                                                				
			}

			#endif
			
			/*erase the sector*/
		     #if 1	   // roger 20150511 add
			 
		      if (FLH_Erase((INT8U *)sec.start, sec.size) < 0) 
			{
			      printf("\n\r [8] Enter retval = -8\n");
				retval = -8;
				goto end_upgrade; 
			}

			
			/*program again*/
		///	if(FLH_Write((INT8U *)dest, (INT8U *)src, len) != 0)
			{
			      printf("\n\r [8] Enter retval = -11\n");
				retval = -11;
				goto end_upgrade; 
			}			  
			  
                  #endif
			
		}
		
		else										  // sector is empty, program directly
		{	/*empty flash, directly write data*/
		       printf("\n\r [11] FLH_Empty((INT8U *)dest, len)== TRUE)\n");	
		///	if(FLH_Write((INT8U *)dest, (INT8U *)src, len) != 0)
			{
			      printf("\n\r [11] Enter retval = -11\n");
				retval = -11;
				goto end_upgrade; 
			}
		}
		
		dest += len;
		src += len;
		size -=len;	
	}	/*end of while*/


      #if 0    // disable write vendor magic, roger 20150525 add
	/* write magic number */
	if ((strcmp((INT8S*)phdr->image_type, LOADER_TYPE) != 0) &&
		(strcmp((INT8S*)phdr->image_type, CONFIGFILE_TYPE) != 0))	
	{				
	      printf("\n\r [12] Write VENDOR_MAGIC = 0x%lx\n",VENDOR_MAGIC);	
		magic_str = VENDOR_MAGIC;
			
		if (FLH_Write(start, (INT8U *)&magic_str, MAGIC_LEN) != 0)
		{
		      printf("\n\r [12] Enter retval = -12\n");
			retval = -12;
			goto end_upgrade; 
		}			
	}	
      #endif

	
	printf("\n\r [13] %s upgrade success!\n", phdr->image_type);
	
	/*if not end, several file compond in a file to upload*/
	if (total>0)
		goto do_again;

    #endif
	

#if 1

end_upgrade:
////	if (buf)
////		free(buf);
	if (retval)
		printf("\n\r [14] upgrade firmware fail %d ! \nFIT>",retval);
	return retval;

#endif



#endif

	
}

#endif



/*
 *===========================================================
 *  Function Name: SPI-FW_Upgrade
 *  Description  : upgrade related firmware, certificate file, configure file, etc
 *
 *  Input        : start address of load firmware
 *                 length of load firmware
 *  Output       : None
 *
 *  Return value : Result from server
 *
 *  History      : 
 *
 *                 roger   Create   2023/07/31 create
 *===========================================================
*/

INT32S SPI_FW_Upgrade(INT8U *addr, INT32S total)
{

    printf ("\n\r [1] start SPI_FW_Upgrade \n\r");
	  

	IMG_HEADER *phdr;	
	//INT32S size, src, dest, offset, retval = 0, len;
	INT32S size, src, dest, offset, retval = 0;
	uint32_t B1;
	//uint32_t B[512];	
	//uint32_t BLK;
	uint8_t B0_cnt,B0_S15X_cnt,B509_S0X_cnt,retry_time=11;
	//uint8_t wr_cnt;
	//INT32S dest_ind;	
	//INT8U *psha1_crc, *start;
	INT8U *psha1_crc;	
	INT32U len=0;

	extern const INT8S PlatformID[LEN_PLATFORMID];	
	//FLH_SECTOR sec;
    SPI_FLH_BLOCK blk; 
	
	//FLASH_Status interflash_status;
//	INT32S hdr_src;
//	uint32_t spiflash_waddr = SPI_FLASH_Block_B511X ;
	

do_again:
		
	phdr = (IMG_HEADER *)addr;
	
	/* check platform ID */
	if (strncmp((INT8S*)phdr->platform_id, PlatformID, LEN_PLATFORMID)!=0) 
	{			
		retval = -1;
		goto end_upgrade; 
	}
	
	/* check crc or sha1 */
	size = sizeof(IMG_HEADER)+phdr->image_size;
	psha1_crc = (INT8U *)(addr + size);


	#if 1        // check image crc value
	if (image_check(phdr->check_type, (INT8U *)addr, size, psha1_crc) != 0)
	{
		retval = -2;
		goto end_upgrade; 
	}	
      #endif

	
	if (strcmp((INT8S*)phdr->image_type, LOADER_TYPE) == 0 ||
		strcmp((INT8S*)phdr->image_type, CERT_TYPE) == 0 ||
		strcmp((INT8S*)phdr->image_type, CALI_TYPE) == 0 ||
		strcmp((INT8S*)phdr->image_type, CONFIGFILE_TYPE) == 0)
	{
		if (memcmp((INT8S*)phdr->compress, "zip", 4) == 0)
		{
			/*unzip to another memory?*/
			retval = -5;
			goto end_upgrade; 				 
		}
	}	/*end if */
	
	
	if (strcmp((INT8S*)phdr->image_type, LOADER_TYPE) == 0 ||
		strcmp((INT8S*)phdr->image_type, FIRMWARE_TYPE) == 0 ||   // roger 20150525 add
		strcmp((INT8S*)phdr->image_type, CONFIGFILE_TYPE) == 0)
		{
		src = (INT32U)addr+sizeof(IMG_HEADER);
             printf("\n\r [2] Image src addr is 0x%lx \n\r", src);     // src is the address that image [data] temporarily put on SDRAM
		}
	else
		{
		src = (INT32U)addr;
             printf("\n\r [2] Image src addr is 0x%lx \n\r", src);   
		}

    //while(1);

	/* get the address that APP will be programmed */  // check memory space of "APP-1~5" 
	/* would like to get FW image info. from SPI-Flash directly, that means must switch to direct memory mode, if the start addr. is not 0x0 */
	dest = GetFirmwarePosition((CHAR *)phdr->image_type, phdr->version); 
    printf("\n\r [2] Image dest addr is 0x%lx \n\r", dest);   

    //while(1);    // roger 20230808 add for debug

      // write APP1~5 Header to 0x8020000, roger 20150716 add	  
			  
	#if 1		 
		  
		if (strcmp((INT8S*)phdr->image_type, FIRMWARE_TYPE) == 0)
		{
			switch (dest)			// mapping the "header of APP" to the dedicated addr which will be upadted  
			  {
			    case DM_SPI_FLH_APP1_START:
				//   hdr_src=SPI_FLASH_Block_510;
				break;  		
			  }
		}


	   //===================================================================
	   // After execute "GetFirmwarePosition" function, must switch from direct memory mode to indirect memory mode before program SPI-Flash 
	   Do_QSPI_ResetMemory(1,NULL); 
	   Do_QSPI_ResetMemory(1,NULL);    
	   Do_QSPI_ResetMemory(1,NULL);    
	   //===================================================================
	   
       // spiflash_waddr = SPI_FLASH_Block_510 ;
		  
       // Erase SPI-Flash Block 510, roger 20230801 modify

	    #if 0
        if ((BSP_QSPI_Erase_Block((uint32_t)SPI_FLASH_Block_B0_S15X))==QSPI_OK)
			printf("\n\r [2] SPI-FLASH_Erase Block-0 (APP-1 Header) success \n\r");	
		else
			printf("\n\r [2] SPI-FLASH_Erase Block-0 (APP-1 Header) fail \n\r");
		
		if ((BSP_QSPI_Erase_Block((uint32_t)SPI_FLASH_Block_B509_S0X))==QSPI_OK)
			printf("\n\r [2] SPI-FLASH_Erase Block-510 (APP-1 Backup Header) success \n\r");
		else
			printf("\n\r [2] SPI-FLASH_Erase Block-510 (APP-1 Backup Header) fail \n\r");	
        #endif


	    #if 1
        while ((BSP_QSPI_Erase_Block((uint32_t)SPI_FLASH_Block_B0_S15X))!=QSPI_OK)
        	{
        	  if (B0_S15X_cnt=1, B0_S15X_cnt < retry_time, B0_S15X_cnt++)
        	  {
               printf("\n\r [2] SPI-FLASH_Erase Block-0 (APP-1 Header) retry %d times \n\r", B0_cnt);
        	  }
			    printf("\n\r [2] SPI-FLASH_Erase Block-0 (APP-1 Header) fail \n\r");	
				retval = -3;
				goto end_upgrade; 
        	}
			  printf("\n\r [2] SPI-FLASH_Erase Block-0 (APP-1 Header) success \n\r");	
	
		while ((BSP_QSPI_Erase_Block((uint32_t)SPI_FLASH_Block_B509_S0X))!=QSPI_OK)
		   {
		      if (B509_S0X_cnt=1, B509_S0X_cnt < retry_time, B509_S0X_cnt++)
			  {
			   printf("\n\r [2] SPI-FLASH_Erase Block-509 (APP-1 Backup Header) retry %d times \n\r", B509_S0X_cnt);
			  }
			    printf("\n\r [2] SPI-FLASH_Erase Block-509 (APP-1 Backup Header) fail \n\r");	  
			    retval = -3;
			    goto end_upgrade; 
			}
		      printf("\n\r [2] SPI-FLASH_Erase Block-509 (APP-1 Backup Header) success \n\r");   

		
        #endif		

		

        // write the header of "updated APP-1" to Block511 


        /* APP-1 Header */

		//if (BSP_QSPI_Write((INT8U *)((uint32_t)(0xD0400000L)), (uint32_t)(0xFFC0L), (uint32_t)(0x40))!= QSPI_OK)
		if (BSP_QSPI_Write((INT8U *)(RAM_START), (uint32_t)SPI_FLH_APP1_HEADER, sizeof(IMG_HEADER))!= QSPI_OK)			
			// uint8_t BSP_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size) 
			// BSP_QSPI_Write(Tx_Buffer_indir, addr, BufferSize_indir);
			{
				printf("\n\r [2] APP-1 Header Program fail, Enter retval = -4\n");
				printf("\n\r [2] APP-1 Header Destination Addr = 0x%lx \n", (uint32_t)SPI_FLH_APP1_HEADER);  // the location that APP-1 header will be programmed
				printf("\n\r [2] APP-1 Header Source Addr = 0x%lx \n\r", addr); 			  // the location that APP-1 header put on SDRAM 
				retval = -4;
				goto end_upgrade; 
			}
		//else if (BSP_QSPI_Write((INT8U *)addr, spiflash_waddr, 64)== QSPI_OK) 			 // Program APP-1 header to 0x1FE_0000
		//else if (BSP_QSPI_Write((INT8U *)((uint32_t)(0xD0400000L)), (uint32_t)(0xFFC0L), (uint32_t)(0x40))== QSPI_OK)				 // Program APP-1 header to 0x1FE_0000		  
        else if (BSP_QSPI_Write((INT8U *)(RAM_START), (uint32_t)SPI_FLH_APP1_HEADER, sizeof(IMG_HEADER))== QSPI_OK)
			{
				printf("\n\r [2] APP-1 Header Program Success ! \n\r"); 		
				printf("\n\r [2] APP-1 Header Destination Addr is 0x%lx \n\r", (uint32_t)SPI_FLH_APP1_HEADER);
				printf("\n\r [2] APP-1 Header Source Addr = 0x%lx \n\r", addr); 
			}
			

        /* APP-1 Backup Header */
		 
		//if (BSP_QSPI_Write((INT8U *)addr, spiflash_waddr, 64)!= QSPI_OK)
		//if (BSP_QSPI_Write((INT8U *)((uint32_t)(0xD0400000L)), (uint32_t)(0x1FE0000L), (uint32_t)(0x40))!= QSPI_OK)		
		if (BSP_QSPI_Write((INT8U *)(RAM_START), (uint32_t)SPI_FLH_APP1_HEADER_B, sizeof(IMG_HEADER))!= QSPI_OK)			
			// uint8_t BSP_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size) 
			// BSP_QSPI_Write(Tx_Buffer_indir, addr, BufferSize_indir);
			{
				printf("\n\r [2] APP-1 Backup Header Program fail, Enter retval = -4\n");
				printf("\n\r [2] APP-1 Backup Header Destination Addr = 0x%lx \n", (uint32_t)SPI_FLH_APP1_HEADER_B);  // the location that APP-1 header will be programmed
				printf("\n\r [2] APP-1 Backup Header Source Addr = 0x%lx \n\r", addr);	              // the location that APP-1 header put on SDRAM 
				retval = -4;
				goto end_upgrade; 
			}
        //else if (BSP_QSPI_Write((INT8U *)addr, spiflash_waddr, 64)== QSPI_OK)	             // Program APP-1 header to 0x1FE_0000
        //else if (BSP_QSPI_Write((INT8U *)((uint32_t)(0xD0400000L)), (uint32_t)(0x1FE0000L), (uint32_t)(0x40))== QSPI_OK)	             // Program APP-1 header to 0x1FE_0000  
        else if (BSP_QSPI_Write((INT8U *)(RAM_START), (uint32_t)SPI_FLH_APP1_HEADER_B, sizeof(IMG_HEADER))== QSPI_OK)	
        	{
		        printf("\n\r [2] APP-1 Backup Header Program Success ! \n\r");			
				printf("\n\r [2] APP-1 Backup Header Destination Addr is 0x%lx \n\r", (uint32_t)SPI_FLH_APP1_HEADER_B);
				printf("\n\r [2] APP-1 Backup Header Source Addr = 0x%lx \n\r", addr);	
        	}		

	
	#endif


	if (dest == FLH_LOADER_START)    //  prohibit to cover the loader image
	{
		retval = -13;
		goto end_upgrade; 
	}
	
	/*move pointer to next one*/	
	if (phdr->check_type == IMG_CHECK_SHA1)
		size += SHA1_LEN;
	else if (phdr->check_type == IMG_CHECK_CRC)
		size += CRC_LEN;
	total -= size;
	addr += size;
	
	/* loader, and config will not write header to flash  */
	if ((strcmp((INT8S*)phdr->image_type, LOADER_TYPE) != 0) &&
		(strcmp((INT8S*)phdr->image_type, CONFIGFILE_TYPE) != 0))
	{	
		//magic_len = MAGIC_LEN;
	}
	else
		size = phdr->image_size;
																		                              // dest: reflash starting addr 
	printf("\n\r [3] Start to write %s image to 0x%lx, size=%ld \n", phdr->image_type, dest, size);    // size: reflash size
	
	 /// start = (INT8U *)dest;
       
     /// printf("\n\r [3] Start = 0x%lx \n", start);
	
	// dest += magic_len;	     // disable write vendor magic,  roger 20150525 

      
/////////////////////////////////////////////////////////////////////////////

    #if 1

    //============================================== 
	// due to must spi-flash write must be operated on indirect memory mode, so the dest. addr need to substract 0x9000_0000
    //==============================================

	//dest_ind = ((INT32U)dest - (INT32U)SPI_FLH_BASE_ADDR) ; 

   if (size>0)
   {
	#if 0
   
		for (BLK=1; BLK<=64; BLK=BLK+1)    
		   for (B[BLK]=0x10000+(BLK-1)*0x10000; B[BLK]<0x20000+(BLK-1)*0x10000; B[BLK]=B[BLK]+0x1000)	 // for 2MB APP 			   
			   {
				 while ((BSP_QSPI_Erase_Block((uint32_t)B[BLK])!=QSPI_OK) ) 	
				  {
					if (erase_cnt=1, erase_cnt < retry_time, erase_cnt++)
					{
					 printf("\n\r [5] SPI-Flash Block[%d] Erase retry %d times \n\r", BLK, erase_cnt);
					}
					   printf("\n\r [5] SPI-Flash Block[%d] Sector [%d] Erase Fail, Enter retval = -8\n", BLK, ((B[BLK]-(BLK-1)*0x10000)/0x1000)-16);	 
					   retval = -8;
					   goto end_upgrade;					
				  }
				   /// printf("\n\r [5] SPI-Flash Block[%d] Sector [%d] Erase OK \n\r", BLK, ((B[BLK]-(BLK-1)*0x10000)/0x1000)-16); 	// roger 20230901 mark	   
			   }
		   
	#endif		

   }

	

	while (size>0)
	{	   
		if (SPI_FLH_GetBlock((INT8U *)dest, &blk)<0)          // dest: The image programmed addr. of SPI-Flash, 0x0 currently
		{
			retval = -6;
			goto end_upgrade; 
		}

		offset = (INT32U)dest - blk.start;
		printf("\n\r [4] dest addr = 0x%lx \n", dest); 
		printf("\n\r [4] blk.start addr = 0x%lx \n", blk.start); 		
        printf("\n\r [4] offset addr = 0x%lx \n", offset); 

		len = blk.size - offset;
        printf("\n\r [4] len = 0x%lx \n", len); 		
		printf("\n\r [4] blk.size = 0x%lx \n", blk.size); 
		
		
		if (len > size)
			len = size;        // for ex: 0x10000 > 57,732


       //=============================================================================
	   // "SPI_FLH_Empty" use pointer, must operate at direct memory mode
	   Do_Qspimemmap(1,NULL); 
	   Do_Qspimemmap(1,NULL);    
	   Do_Qspimemmap(1,NULL);    
	   //============================================================================= 
		
		
       if (SPI_FLH_Empty((INT8U *)dest, len)== FALSE)      // if this sector is put last version image, need to erase then program
       {		
			if ((dest != blk.start)||(len != blk.size))
			{	
				printf("\n\r [5] dest addr= 0x%lx \n", dest); 
				printf("\n\r [5] blk.start addr= 0x%lx \n", blk.start);		
				printf("\n\r [5] len = %d \n", len); 		
				printf("\n\r [5] blk.size = 0x%lx \n", blk.size); 			                                                				
			}

	       //=============================================================================
	       // Prepare to erase / write SPI-Flash, switch back to indirect memory mode
		   Do_QSPI_ResetMemory(1,NULL); 
		   Do_QSPI_ResetMemory(1,NULL);    
		   Do_QSPI_ResetMemory(1,NULL);    
		   //============================================================================= 
   

			/*erase the sector*/

		    #if 0		 
			if (BSP_QSPI_Erase_Block((uint32_t)blk.start-(uint32_t)SPI_FLH_BASE_ADDR)!=QSPI_OK) 
			{
			printf("\n\r [5] Enter retval = -8\n");
			retval = -8;
			goto end_upgrade; 
			}	
			#endif


			#if 1	
				
				for (B1=((uint32_t)blk.start-(uint32_t)SPI_FLH_BASE_ADDR); B1<(((uint32_t)blk.start)-(uint32_t)SPI_FLH_BASE_ADDR+0x10000); B1=B1+0x1000)
					{
					  #if 0
					  while ((BSP_QSPI_Erase_Block((uint32_t)B1)!=QSPI_OK) )	 
					   {
						 if (B1_cnt=1, B1_cnt < retry_time, B1_cnt++)
						 {
						  printf("\n\r [5] SPI-Flash Sector Erase retry %d times \n\r", B1_cnt);
						 }
							printf("\n\r [5] SPI-Flash Sector [%d] Erase Fail, Enter retval = -8\n", (B1-0x10000)/0x1000);	  
							retval = -8;
							goto end_upgrade;					 
					   }
					  #endif

						while ((BSP_QSPI_Erase_Block((uint32_t)B1)!=QSPI_OK));	   

					  
						printf("\n\r [5] SPI-Flash Sector [%d] Erase OK \n\r", (B1-0x10000)/0x1000);			
					}
				
			#endif


			/*program again*/

			if (BSP_QSPI_Write((INT8U *)src, dest-(INT32U)SPI_FLH_BASE_ADDR, len)!= QSPI_OK)	
			{
			    printf("\n\r [5] SPI-Flash Program Fail, Enter retval = -11\n");
				retval = -11;
				goto end_upgrade; 
			}			  		  

	    }

        else
        {
	       //=============================================================================
	       // Prepare to erase / write SPI-Flash, switch back to indirect memory mode
		   Do_QSPI_ResetMemory(1,NULL); 
		   Do_QSPI_ResetMemory(1,NULL);    
		   Do_QSPI_ResetMemory(1,NULL);    
		   //============================================================================= 

		   
			if (BSP_QSPI_Write((INT8U *)src, dest-(INT32U)SPI_FLH_BASE_ADDR, len)!= QSPI_OK)	
			{
			    printf("\n\r [5] SPI-Flash Program Fail, Enter retval = -11\n");
				retval = -11;
				goto end_upgrade; 
			}		   


        }
		
			dest += len;
			src += len;
			size -=len;	
			
		}	/*end of while*/


	      #if 0    // disable write vendor magic, roger 20150525 add
			/* write magic number */
			if ((strcmp((INT8S*)phdr->image_type, LOADER_TYPE) != 0) &&
				(strcmp((INT8S*)phdr->image_type, CONFIGFILE_TYPE) != 0))	
			{				
			      printf("\n\r [12] Write VENDOR_MAGIC = 0x%lx\n",VENDOR_MAGIC);	
				magic_str = VENDOR_MAGIC;
					
				if (FLH_Write(start, (INT8U *)&magic_str, MAGIC_LEN) != 0)
				{
				      printf("\n\r [12] Enter retval = -12\n");
					retval = -12;
					goto end_upgrade; 
				}			
			}	
	      #endif

		
			printf("\n\r [13] %s upgrade success!\n", phdr->image_type);
			
			/*if not end, several file compond in a file to upload*/
			if (total>0)
				goto do_again;


	

	#if 1

	end_upgrade:
	////	if (buf)
	////		free(buf);
		if (retval)
			printf("\n\r [14] upgrade firmware fail %d ! \nFIT>",retval);
		return retval;

	#endif



#endif

	
}







/*
 *===========================================================
 *  Function Name: SPI_USBFW_Upgrade
 *  Description  : copy MCU Flash "0x0802_0000" data to the specified SPI Flash address
 *                 upgrade related firmware, certificate file, configure file, etc
 *  Input        : start address of load firmware
 *                 length of load firmware
 *  Output       : None
 *
 *  Return value : Result from server
 *
 *  History      : 
 *
 *                 roger   Create   2023/10/18 create
 *===========================================================
*/

INT32S SPI_USBFW_Upgrade(INT8U *addr, INT32S total)
{

    printf ("\n\r [1] start SPI_FW_Upgrade \n\r");
	  

	IMG_HEADER *phdr;	
	//INT32S size, src, dest, offset, retval = 0, len;
	INT32S size, src, dest, offset, retval = 0;
	uint32_t B1;
	//uint32_t B[512];	
	//uint32_t BLK;
	uint8_t B0_cnt,B0_S15X_cnt,B509_S0X_cnt,retry_time=11;
	//uint8_t wr_cnt;
	//INT32S dest_ind;	
	//INT8U *psha1_crc, *start;
	INT8U *psha1_crc;	
	INT32U len=0;

	extern const INT8S PlatformID[LEN_PLATFORMID];	
	//FLH_SECTOR sec;
    SPI_FLH_BLOCK blk; 
	
	//FLASH_Status interflash_status;
//	INT32S hdr_src;
//	uint32_t spiflash_waddr = SPI_FLASH_Block_B511X ;

	

do_again:
		
	phdr = (IMG_HEADER *)addr;
	
	/* check platform ID */
	if (strncmp((INT8S*)phdr->platform_id, PlatformID, LEN_PLATFORMID)!=0) 
	{			
		retval = -1;
		goto end_upgrade; 
	}
	
	/* check crc or sha1 */
	size = sizeof(IMG_HEADER)+phdr->image_size;
	psha1_crc = (INT8U *)(addr + size);


	#if 1        // check image crc value
	if (image_check(phdr->check_type, (INT8U *)addr, size, psha1_crc) != 0)
	{
		retval = -2;
		goto end_upgrade; 
	}	
      #endif

	
	if (strcmp((INT8S*)phdr->image_type, LOADER_TYPE) == 0 ||
		strcmp((INT8S*)phdr->image_type, CERT_TYPE) == 0 ||
		strcmp((INT8S*)phdr->image_type, CALI_TYPE) == 0 ||
		strcmp((INT8S*)phdr->image_type, CONFIGFILE_TYPE) == 0)
	{
		if (memcmp((INT8S*)phdr->compress, "zip", 4) == 0)
		{
			/*unzip to another memory?*/
			retval = -5;
			goto end_upgrade; 				 
		}
	}	/*end if */
	
	
	if (strcmp((INT8S*)phdr->image_type, LOADER_TYPE) == 0 ||
		strcmp((INT8S*)phdr->image_type, FIRMWARE_TYPE) == 0 ||   // roger 20150525 add
		strcmp((INT8S*)phdr->image_type, CONFIGFILE_TYPE) == 0)
		{
		src = (INT32U)addr+sizeof(IMG_HEADER);
             printf("\n\r [2] Image src addr is 0x%lx \n\r", src);     // src is the address that image [data] temporarily put on SDRAM
		}
	else
		{
		src = (INT32U)addr;
             printf("\n\r [2] Image src addr is 0x%lx \n\r", src);   
		}

    //while(1);

	/* get the address that APP will be programmed */  // check memory space of "APP-1~5" 
	/* would like to get FW image info. from SPI-Flash directly, that means must switch to direct memory mode, if the start addr. is not 0x0 */
	dest = GetFirmwarePosition((CHAR *)phdr->image_type, phdr->version); 
    printf("\n\r [2] Image dest addr is 0x%lx \n\r", dest);   

    //while(1);    // roger 20230808 add for debug

      // write APP1~5 Header to 0x8020000, roger 20150716 add	  
			  
	#if 1		 
		  
		if (strcmp((INT8S*)phdr->image_type, FIRMWARE_TYPE) == 0)
		{
			switch (dest)			// mapping the "header of APP" to the dedicated addr which will be upadted  
			  {
			    case DM_SPI_FLH_APP1_START:
				//   hdr_src=SPI_FLASH_Block_510;
				break;  		
			  }
		}


	   //===================================================================
	   // After execute "GetFirmwarePosition" function, must switch from direct memory mode to indirect memory mode before program SPI-Flash 
	   Do_QSPI_ResetMemory(1,NULL); 
	   Do_QSPI_ResetMemory(1,NULL);    
	   Do_QSPI_ResetMemory(1,NULL);    
	   //===================================================================
	   
       // spiflash_waddr = SPI_FLASH_Block_510 ;
		  
       // Erase SPI-Flash Block 510, roger 20230801 modify

	    #if 0
        if ((BSP_QSPI_Erase_Block((uint32_t)SPI_FLASH_Block_B0_S15X))==QSPI_OK)
			printf("\n\r [2] SPI-FLASH_Erase Block-0 (APP-1 Header) success \n\r");	
		else
			printf("\n\r [2] SPI-FLASH_Erase Block-0 (APP-1 Header) fail \n\r");
		
		if ((BSP_QSPI_Erase_Block((uint32_t)SPI_FLASH_Block_B509_S0X))==QSPI_OK)
			printf("\n\r [2] SPI-FLASH_Erase Block-510 (APP-1 Backup Header) success \n\r");
		else
			printf("\n\r [2] SPI-FLASH_Erase Block-510 (APP-1 Backup Header) fail \n\r");	
        #endif


	    #if 1
        while ((BSP_QSPI_Erase_Block((uint32_t)SPI_FLASH_Block_B0_S15X))!=QSPI_OK)
        	{
        	  if (B0_S15X_cnt=1, B0_S15X_cnt < retry_time, B0_S15X_cnt++)
        	  {
               printf("\n\r [2] SPI-FLASH_Erase Block-0 (APP-1 Header) retry %d times \n\r", B0_cnt);
        	  }
			    printf("\n\r [2] SPI-FLASH_Erase Block-0 (APP-1 Header) fail \n\r");	
				retval = -3;
				goto end_upgrade; 
        	}
			  printf("\n\r [2] SPI-FLASH_Erase Block-0 (APP-1 Header) success \n\r");	
	
		while ((BSP_QSPI_Erase_Block((uint32_t)SPI_FLASH_Block_B509_S0X))!=QSPI_OK)
		   {
		      if (B509_S0X_cnt=1, B509_S0X_cnt < retry_time, B509_S0X_cnt++)
			  {
			   printf("\n\r [2] SPI-FLASH_Erase Block-509 (APP-1 Backup Header) retry %d times \n\r", B509_S0X_cnt);
			  }
			    printf("\n\r [2] SPI-FLASH_Erase Block-509 (APP-1 Backup Header) fail \n\r");	  
			    retval = -3;
			    goto end_upgrade; 
			}
		      printf("\n\r [2] SPI-FLASH_Erase Block-509 (APP-1 Backup Header) success \n\r");   

		
        #endif		

		

        // write the header of "updated APP-1" to Block511 


        /* APP-1 Header */

		//if (BSP_QSPI_Write((INT8U *)((uint32_t)(0xD0400000L)), (uint32_t)(0xFFC0L), (uint32_t)(0x40))!= QSPI_OK)
		if (BSP_QSPI_Write((INT8U *)(BF_MCUFlash_START), (uint32_t)SPI_FLH_APP1_HEADER, sizeof(IMG_HEADER))!= QSPI_OK)			
			// uint8_t BSP_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size) 
			// BSP_QSPI_Write(Tx_Buffer_indir, addr, BufferSize_indir);
			{
				printf("\n\r [2] APP-1 Header Program fail, Enter retval = -4\n");
				printf("\n\r [2] APP-1 Header Destination Addr = 0x%lx \n", (uint32_t)SPI_FLH_APP1_HEADER);  // the location that APP-1 header will be programmed
				printf("\n\r [2] APP-1 Header Source Addr = 0x%lx \n\r", addr); 			  // the location that APP-1 header put on SDRAM 
				retval = -4;
				goto end_upgrade; 
			}
		//else if (BSP_QSPI_Write((INT8U *)addr, spiflash_waddr, 64)== QSPI_OK) 			 // Program APP-1 header to 0x1FE_0000
		//else if (BSP_QSPI_Write((INT8U *)((uint32_t)(0xD0400000L)), (uint32_t)(0xFFC0L), (uint32_t)(0x40))== QSPI_OK)				 // Program APP-1 header to 0x1FE_0000		  
        else if (BSP_QSPI_Write((INT8U *)(BF_MCUFlash_START), (uint32_t)SPI_FLH_APP1_HEADER, sizeof(IMG_HEADER))== QSPI_OK)
			{
				printf("\n\r [2] APP-1 Header Program Success ! \n\r"); 		
				printf("\n\r [2] APP-1 Header Destination Addr is 0x%lx \n\r", (uint32_t)SPI_FLH_APP1_HEADER);
				printf("\n\r [2] APP-1 Header Source Addr = 0x%lx \n\r", addr); 
			}
			

        /* APP-1 Backup Header */
		 
		//if (BSP_QSPI_Write((INT8U *)addr, spiflash_waddr, 64)!= QSPI_OK)
		//if (BSP_QSPI_Write((INT8U *)((uint32_t)(0xD0400000L)), (uint32_t)(0x1FE0000L), (uint32_t)(0x40))!= QSPI_OK)		
		if (BSP_QSPI_Write((INT8U *)(BF_MCUFlash_START), (uint32_t)SPI_FLH_APP1_HEADER_B, sizeof(IMG_HEADER))!= QSPI_OK)			
			// uint8_t BSP_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size) 
			// BSP_QSPI_Write(Tx_Buffer_indir, addr, BufferSize_indir);
			{
				printf("\n\r [2] APP-1 Backup Header Program fail, Enter retval = -4\n");
				printf("\n\r [2] APP-1 Backup Header Destination Addr = 0x%lx \n", (uint32_t)SPI_FLH_APP1_HEADER_B);  // the location that APP-1 header will be programmed
				printf("\n\r [2] APP-1 Backup Header Source Addr = 0x%lx \n\r", addr);	              // the location that APP-1 header put on SDRAM 
				retval = -4;
				goto end_upgrade; 
			}
        //else if (BSP_QSPI_Write((INT8U *)addr, spiflash_waddr, 64)== QSPI_OK)	             // Program APP-1 header to 0x1FE_0000
        //else if (BSP_QSPI_Write((INT8U *)((uint32_t)(0xD0400000L)), (uint32_t)(0x1FE0000L), (uint32_t)(0x40))== QSPI_OK)	             // Program APP-1 header to 0x1FE_0000  
        else if (BSP_QSPI_Write((INT8U *)(BF_MCUFlash_START), (uint32_t)SPI_FLH_APP1_HEADER_B, sizeof(IMG_HEADER))== QSPI_OK)	
        	{
		        printf("\n\r [2] APP-1 Backup Header Program Success ! \n\r");			
				printf("\n\r [2] APP-1 Backup Header Destination Addr is 0x%lx \n\r", (uint32_t)SPI_FLH_APP1_HEADER_B);
				printf("\n\r [2] APP-1 Backup Header Source Addr = 0x%lx \n\r", addr);	
        	}		

	
	#endif


	if (dest == FLH_LOADER_START)    //  prohibit to cover the loader image
	{
		retval = -13;
		goto end_upgrade; 
	}
	
	/*move pointer to next one*/	
	if (phdr->check_type == IMG_CHECK_SHA1)
		size += SHA1_LEN;
	else if (phdr->check_type == IMG_CHECK_CRC)
		size += CRC_LEN;
	total -= size;
	addr += size;
	
	/* loader, and config will not write header to flash  */
	if ((strcmp((INT8S*)phdr->image_type, LOADER_TYPE) != 0) &&
		(strcmp((INT8S*)phdr->image_type, CONFIGFILE_TYPE) != 0))
	{	
		//magic_len = MAGIC_LEN;
	}
	else
		size = phdr->image_size;
																		                              // dest: reflash starting addr 
	printf("\n\r [3] Start to write %s image to 0x%lx, size=%ld \n", phdr->image_type, dest, size);    // size: reflash size
	
	 /// start = (INT8U *)dest;
       
     /// printf("\n\r [3] Start = 0x%lx \n", start);
	
	// dest += magic_len;	     // disable write vendor magic,  roger 20150525 

      
/////////////////////////////////////////////////////////////////////////////

    #if 1

    //============================================== 
	// due to must spi-flash write must be operated on indirect memory mode, so the dest. addr need to substract 0x9000_0000
    //==============================================

	//dest_ind = ((INT32U)dest - (INT32U)SPI_FLH_BASE_ADDR) ; 

   if (size>0)
   {
	#if 0
   
		for (BLK=1; BLK<=64; BLK=BLK+1)    
		   for (B[BLK]=0x10000+(BLK-1)*0x10000; B[BLK]<0x20000+(BLK-1)*0x10000; B[BLK]=B[BLK]+0x1000)	 // for 2MB APP 			   
			   {
				 while ((BSP_QSPI_Erase_Block((uint32_t)B[BLK])!=QSPI_OK) ) 	
				  {
					if (erase_cnt=1, erase_cnt < retry_time, erase_cnt++)
					{
					 printf("\n\r [5] SPI-Flash Block[%d] Erase retry %d times \n\r", BLK, erase_cnt);
					}
					   printf("\n\r [5] SPI-Flash Block[%d] Sector [%d] Erase Fail, Enter retval = -8\n", BLK, ((B[BLK]-(BLK-1)*0x10000)/0x1000)-16);	 
					   retval = -8;
					   goto end_upgrade;					
				  }
				   /// printf("\n\r [5] SPI-Flash Block[%d] Sector [%d] Erase OK \n\r", BLK, ((B[BLK]-(BLK-1)*0x10000)/0x1000)-16); 	// roger 20230901 mark	   
			   }
		   
	#endif		

   }

	

	while (size>0)
	{	   
		if (SPI_FLH_GetBlock((INT8U *)dest, &blk)<0)          // dest: The image programmed addr. of SPI-Flash, 0x0 currently
		{
			retval = -6;
			goto end_upgrade; 
		}

		offset = (INT32U)dest - blk.start;
		printf("\n\r [4] dest addr = 0x%lx \n", dest); 
		printf("\n\r [4] blk.start addr = 0x%lx \n", blk.start); 		
        printf("\n\r [4] offset addr = 0x%lx \n", offset); 

		len = blk.size - offset;
        printf("\n\r [4] len = 0x%lx \n", len); 		
		printf("\n\r [4] blk.size = 0x%lx \n", blk.size); 
		
		
		if (len > size)
			len = size;        // for ex: 0x10000 > 57,732


       //=============================================================================
	   // "SPI_FLH_Empty" use pointer, must operate at direct memory mode
	   Do_Qspimemmap(1,NULL); 
	   Do_Qspimemmap(1,NULL);    
	   Do_Qspimemmap(1,NULL);    
	   //============================================================================= 
		
		
       if (SPI_FLH_Empty((INT8U *)dest, len)== FALSE)      // if this sector is put last version image, need to erase then program
       {		
			if ((dest != blk.start)||(len != blk.size))
			{	
				printf("\n\r [5] dest addr= 0x%lx \n", dest); 
				printf("\n\r [5] blk.start addr= 0x%lx \n", blk.start);		
				printf("\n\r [5] len = %d \n", len); 		
				printf("\n\r [5] blk.size = 0x%lx \n", blk.size); 			                                                				
			}

	       //=============================================================================
	       // Prepare to erase / write SPI-Flash, switch back to indirect memory mode
		   Do_QSPI_ResetMemory(1,NULL); 
		   Do_QSPI_ResetMemory(1,NULL);    
		   Do_QSPI_ResetMemory(1,NULL);    
		   //============================================================================= 
   

			/*erase the sector*/

		    #if 0		 
			if (BSP_QSPI_Erase_Block((uint32_t)blk.start-(uint32_t)SPI_FLH_BASE_ADDR)!=QSPI_OK) 
			{
			printf("\n\r [5] Enter retval = -8\n");
			retval = -8;
			goto end_upgrade; 
			}	
			#endif


			#if 1	
				
				for (B1=((uint32_t)blk.start-(uint32_t)SPI_FLH_BASE_ADDR); B1<(((uint32_t)blk.start)-(uint32_t)SPI_FLH_BASE_ADDR+0x10000); B1=B1+0x1000)
					{
					  #if 0
					  while ((BSP_QSPI_Erase_Block((uint32_t)B1)!=QSPI_OK) )	 
					   {
						 if (B1_cnt=1, B1_cnt < retry_time, B1_cnt++)
						 {
						  printf("\n\r [5] SPI-Flash Sector Erase retry %d times \n\r", B1_cnt);
						 }
							printf("\n\r [5] SPI-Flash Sector [%d] Erase Fail, Enter retval = -8\n", (B1-0x10000)/0x1000);	  
							retval = -8;
							goto end_upgrade;					 
					   }
					  #endif

						while ((BSP_QSPI_Erase_Block((uint32_t)B1)!=QSPI_OK));	   

					  
						printf("\n\r [5] SPI-Flash Sector [%d] Erase OK \n\r", (B1-0x10000)/0x1000);			
					}
				
			#endif


			/*program again*/

			if (BSP_QSPI_Write((INT8U *)src, dest-(INT32U)SPI_FLH_BASE_ADDR, len)!= QSPI_OK)	
			{
			    printf("\n\r [5] SPI-Flash Program Fail, Enter retval = -11\n");
				retval = -11;
				goto end_upgrade; 
			}			  		  

	    }

        else
        {
	       //=============================================================================
	       // Prepare to erase / write SPI-Flash, switch back to indirect memory mode
		   Do_QSPI_ResetMemory(1,NULL); 
		   Do_QSPI_ResetMemory(1,NULL);    
		   Do_QSPI_ResetMemory(1,NULL);    
		   //============================================================================= 

		   
			if (BSP_QSPI_Write((INT8U *)src, dest-(INT32U)SPI_FLH_BASE_ADDR, len)!= QSPI_OK)	
			{
			    printf("\n\r [5] SPI-Flash Program Fail, Enter retval = -11\n");
				retval = -11;
				goto end_upgrade; 
			}		   


        }
		
			dest += len;
			src += len;
			size -=len;	
			
		}	/*end of while*/


	      #if 0    // disable write vendor magic, roger 20150525 add
			/* write magic number */
			if ((strcmp((INT8S*)phdr->image_type, LOADER_TYPE) != 0) &&
				(strcmp((INT8S*)phdr->image_type, CONFIGFILE_TYPE) != 0))	
			{				
			      printf("\n\r [12] Write VENDOR_MAGIC = 0x%lx\n",VENDOR_MAGIC);	
				magic_str = VENDOR_MAGIC;
					
				if (FLH_Write(start, (INT8U *)&magic_str, MAGIC_LEN) != 0)
				{
				      printf("\n\r [12] Enter retval = -12\n");
					retval = -12;
					goto end_upgrade; 
				}			
			}	
	      #endif

		
			printf("\n\r [13] %s upgrade success!\n", phdr->image_type);
		    LED_GREEN;
			
			/*if not end, several file compond in a file to upload*/
			if (total>0)
				goto do_again;


	

	#if 1

	end_upgrade:
	////	if (buf)
	////		free(buf);
		if (retval)
			printf("\n\r [14] upgrade firmware fail %d ! \nFIT>",retval);
		return retval;

	#endif



#endif

	
}






/*
 *===========================================================
 *  Function Name: Do_Upgrade
 *  Description  : upgrade firmware, certification, etc, by xmodmem
 *					ptotocol via serial port
 *	
 *  Input        : None
 *                 
 *  Output       : None
 *
 *  Return value : 
 *
 *  History      : 
 *
 * 		roger huang	modify 01/25/2010
 *===========================================================
*/
INT32S Do_Upgrade (INT32S argc, INT8S *argv[])
{
	INT32S ret, len, size=0, max_size = LoadBufSize;
	INT8U *start = (INT8U *)LoadBufAddr, *paddr;
	IMG_HEADER *phdr;
	
	printf("\n\r [UPGRADE] Please send file by Xmodem ...\r\n");

	//(*((volatile unsigned *)(USART1 + 0x04)))=0x000001FF;
	//(*((volatile unsigned *)(USART1 + 0x04)))=0xFFFFFFFF;


	printf ("\n\r [UPGRADE] buffered image start address 0x%lx\n\r", start);      // roger 20230726 add
	ret = xmodemReceive(start, max_size);


	
	if (ret < 0) 
	{
		printf ("\n\r [UPGRADE] Xmodem receive error: status: %d\n\r", ret);
		return ret;
	}
	else  
		printf ("\n\r [UPGRADE] Xmodem successfully received at 0x%lx, %d bytes\n\r", (unsigned)paddr, ret);
	/*trim the tail bytes*/


#if 1	                 /* parasing the image header located on SDRAM to get image info. (size...etc) */
	
	paddr = start;
	while(ret>0)
	{
		phdr = (IMG_HEADER *)paddr;
		if (strncmp((INT8S*)phdr->platform_id, PlatformID, LEN_PLATFORMID)!=0) 
			break;
		len = (sizeof(IMG_HEADER)+phdr->image_size);
		if (phdr->check_type == IMG_CHECK_SHA1)
			len += SHA1_LEN;
		else if (phdr->check_type == IMG_CHECK_CRC)
			len += CRC_LEN;
		if (ret < len)
			break;
		ret -=len;
		size += len;
		paddr += len;

		printf ("\n\r [UPGRADE] buffered image start address 0x%lx\n\r", start);
		printf ("\n\r [UPGRADE] image size %d bytes\n\r", size);
		
	}
	if (size)
		//  ret = FW_Upgrade(start, size);    // roger 20230731 deactivate
		ret = SPI_FW_Upgrade(start, size);    // roger 20230731 add
		
	return ret;

#endif
	
}





/*
 *===========================================================
 *  Function Name: Do_SpiFlash_Update
 *  Description  : upgrade spi flash data
 *					
 *	
 *  Input        : None
 *                 
 *  Output       : None
 *
 *  Return value : 
 *
 *  History      : 
 *
 * 		roger huang	2023/10/18 create
 *===========================================================
*/

#if 0

INT32S Do_SpiFlash_Update (INT32S argc, INT8S *argv[])
{
	INT8U *start;
    INT32U dest,size;
	INT32S ret;

	printf("\n\r [Do_SpiFlash_Update] Enter Do_SpiFlash_Update \r\n");


	if (argc < 3)
	{
		printf("\n\r [Do_SpiFlash_Update] argument not enough\n");
		return -1;
	}	

    //
	dest = atoh(argv[1]);						// The Flash addr will be programmed
	start  = (unsigned char *)dest;
	printf("\n\r [Do_SpiFlash_Update] [1] spi flash addr to be written = 0x%lx \n", start);
	
    //
	size = atoh(argv[2]);						// The Flash addr will be programmed
	printf("\n\r [Do_SpiFlash_Update] [2] data size written to spi flash is = %ld \n", size);
                 	
	
	if (size)
		ret = SPI_USBFW_Upgrade(start, size);    // roger 20230731 add
		
	return ret;

	
}

#endif



INT32S Do_SpiFlash_Update (INT32S argc, INT8S *argv[])
{
    INT32U size;
	INT32S ret;
    IMG_HEADER *usb_phdr;	
	INT32S LoadBufAddr = BF_MCUFlash_START;
	INT8U *start = (INT8U *)LoadBufAddr, *paddr;
	

	printf("\n\r [Do_SpiFlash_Update] Enter Do_SpiFlash_Update \r\n");

	paddr = start;
	
    printf("\n\r [Do_SpiFlash_Update] USB image is copied from MCU flash addr is 0x%lx \n\r", (unsigned)paddr);   

	usb_phdr = (IMG_HEADER *)paddr;

    size  = usb_phdr->image_size + sizeof(IMG_HEADER) + 0x4;    // CRC: 4 byte

    printf("\n\r [Do_SpiFlash_Update] data size written to spi flash is = %ld \n", size);   
	
	
	if (size)
		ret = SPI_USBFW_Upgrade(start, size);    // roger 20230731 add
		
	return ret;

	
}





/*
 *===========================================================
 *  Function Name: Do_Bootime
 *
 *  Description  : 
 *	
 *  Input        : None
 *                 
 *  Output       : None
 *
 *  Return value : 
 *
 *  History      : roger huang 2023/08/25 create
 *
 *===========================================================
*/

INT32S Do_Bootime (INT32S argc, INT8S *argv[])
{

	volatile  unsigned char *flashdata_b1;    // roger 20230829 add	

	uint8_t flashdata_rb[1];				  // roger 20230829 add		

    //INT8U warning_flag = 0;
	uint8_t boot_cnt,retry_time=11;	

		
		if (argc < 2)
		{
			printf("\n\r [Bootime] [01] argument not enough\n");
			return -1;
		}	

		if (strcmp("r",argv[1])==0)
		{
		    printf("\n\r [Bootime] [01] Bootime spi-flash setting addr to be read = 0x%lx \n", Bootime_P);
			
			/* 32 spi flash W25Q256 initialization */
			//QSPI_FLASH_Init();	// roger 20230710, move to BSP Init.
			
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
			printf("\r\n [Bootime] [01] FlashID is 0x%X,	Manufacturer Device ID is 0x%X\r\n", FlashID_indir, DeviceID_indir);
			printf("\r\n [Bootime] [01] Flash Status Reg1 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(1));	
			printf("\r\n [Bootime] [01] Flash Status Reg2 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(2));
			printf("\r\n [Bootime] [01] Flash Status Reg3 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(3));
			QSPI_Set_WP_TO_QSPI_IO();
			

	        while (BSP_QSPI_FastRead(flashdata_rb, (uint32_t) Bootime_P, 1)==QSPI_OK) 
	        {
				printf("\n\r [Bootime] [01] SPI-FLASH byte addr = 0x%lx, data = 0x%2x \n",Bootime_P, flashdata_rb[0]);
	            break;
	        }			
			
			printf("\n\r [Bootime] [01] Bootloader to application jump time is [%d] second \n\r", flashdata_rb[0]);

	    }
		
		else if(strcmp("ws",argv[1])==0)
		{
			if (argc != 3)
			{
				printf("\n\r [Bootime] [02] argument not enough\n");
				return -1;
			}

           // Confirm flash value can be programmed before set new bootime

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
		   printf("\r\n [Bootime] [01] FlashID is 0x%X,    Manufacturer Device ID is 0x%X\r\n", FlashID_indir, DeviceID_indir);
		   printf("\r\n [Bootime] [01] Flash Status Reg1 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(1));    
		   printf("\r\n [Bootime] [01] Flash Status Reg2 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(2));
		   printf("\r\n [Bootime] [01] Flash Status Reg3 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(3));
		   QSPI_Set_WP_TO_QSPI_IO();

		   while (BSP_QSPI_FastRead(flashdata_rb, (uint32_t) Bootime_P, 1)!=QSPI_OK);
		   
		   if (flashdata_rb[0]!=0xFF)
              while (BSP_QSPI_Erase_Block((uint32_t)Bootime_P)!=QSPI_OK);	
		
           // set new boot time 

		    flashdata_b1 = (unsigned char *)atoh(argv[2]);	
			uint8_t Input_databuff_b[]={(uint8_t)flashdata_b1};
			
			while (BSP_QSPI_Write(Input_databuff_b, (uint32_t)Bootime_P, sizeof(IMG_HEADER))!= QSPI_OK)
			{
		      if (boot_cnt=1, boot_cnt < retry_time, boot_cnt++)
			  {
			   printf("\n\r [Bootime] [02] Program jump time retry %d times \n\r", boot_cnt);
			  }
			    printf("\n\r [Bootime] [02] Program jump time fail \n\r");	  
			    break;
			}	
		    printf("\n\r [Bootime] [02] Bootloader to application jump time is set as [%d] second \n\r", flashdata_b1);			
		}
     
        else
        {
			printf("\n\r [Bootime] [03] ambigous Erase operation, pls reinput the erase function you want \n");
		    //warning_flag = 0;
			return -2;
        }


    	
	return 0;
}



/*
 *===========================================================
 *  Function Name: Do_Crc
 *
 *  Description  : command to calculate specified memory's CRC value
 *	 
 *  Input        : argv[1] start address
 *                 argv[2] length
 * 
 *  Return value : CRC value (4 bytes)
 *
 *  History      : 
 * 		roger huang create 01/25/2010
 *===========================================================
*/ 

#if 0

INT32S Do_Crc (INT32S argc, INT8S *argv[])
{
	INT8U crc_val[CRC_LEN];
	INT32U start_addr = 0L;
	INT32S len = 0;
	
	start_addr = atoh(argv[1]);
	len = atoh(argv[2]);
	
	Cal_Crc((INT8U *)start_addr, len, crc_val);
	printf("[CRC : %02x %02x %02x %02x] \r\n", crc_val[0], crc_val[1], crc_val[2], crc_val[3]);
					
	return 0;
}

#endif


/*
 *===========================================================
 *  Function Name: Do_Erase
 *
 *  Description  : Erase specfied firmware or configure file
 *	
 *  Input        : None
 *                 
 *  Output       : None
 *
 *  Return value : 
 *
 *  History      : 
 *
 * 		roger huang	modify 01/25/2010
 *===========================================================
*/

#if 0

INT32S Do_Erase (INT32S argc, INT8S *argv[])	
{
	FLH_SECTOR sec;
	IMG_HEADER hdr;
	IMAGE_POS_T *image;
	INT8U *buf = NULL, *dest, ch;
	INT32S id,i=0,offset, len, retval = 0;
	INT32U size;
	
	image = (IMAGE_POS_T *)&ImagePosition;
	if (argc ==1)
	{
		/*list all available firmware or configure file*/
		while(image->name!=NULL)
		{
			memcpy((CHAR *)&hdr, (CHAR *)(image->base+MAGIC_LEN), sizeof(IMG_HEADER));
			/* check magic string */
			if (*(INT32U *)image->base == VENDOR_MAGIC &&
				strncmp((CHAR *)hdr.platform_id, (CHAR *)PlatformID, LEN_PLATFORMID)==0)
			{
				printf("(%d) %s version %d ",i, hdr.image_type, hdr.version);
				printf("build date %d/%d/%d %d:%d:%d\n",
					hdr.build_time.year,hdr.build_time.month, hdr.build_time.day,
					hdr.build_time.hour,hdr.build_time.min, hdr.build_time.sec);
				i++;
			}
			image++;	/*move to next on*/
		}
		return 0;
	}	/*end of while*/
	
	id = atoh(argv[1]);
	/*list all available firmware or configure file*/
	while(image->name!=NULL)
	{
		memcpy((CHAR *)&hdr, (CHAR *)(image->base+MAGIC_LEN), sizeof(IMG_HEADER));
		/* check magic string */
		if (*(INT32U *)image->base == VENDOR_MAGIC &&
			strncmp((CHAR *)hdr.platform_id, (CHAR *)PlatformID, LEN_PLATFORMID)==0)
		{
			if (id!=i++)
			{	
				image++;	/*move to next on*/
				continue;
			}
			/*find*/	
			printf("Erase (%d) %s version %d? (y/n) ",id, hdr.image_type, hdr.version);
			while(1)
			{
				if (kbhit())
				{	
					ch = getch();			
					printf("%c\n",ch);
					if (ch != 'y')
						return;
					break;	
				}
			}
			dest = (INT8U *)image->base;
			size = image->size;
			while(size)
			{
				/*get sector*/
				if (FLH_GetSector(dest, &sec)<0)
				{
					retval = -1;
					goto fail_erase; 
				}
				offset = (INT32U)dest - sec.start;
				len = sec.size - offset;
				if (len > size)
					len = size;
				/* if not in boundary or not all sector erase
				 * it need backup unused area
				 */
				if ((INT32U)dest != sec.start || len != sec.size)
				{
					buf = malloc(sec.size);
					if (buf == NULL)
					{
						retval = -2;
						goto fail_erase; 
					}	
					memcpy(buf, (INT8U *)sec.start, sec.size);
					memset(buf+offset,0xff,len);
				}			
				/*if sector empty?*/
				if (FLH_Empty((INT8U *)sec.start, sec.size )!= TRUE)
				{	/*erase while sector*/	
					if (FLH_Erase((INT8U *)sec.start, sec.size) < 0)
					{
						retval = -3;
						goto fail_erase; 
					}
				}
				if (buf)
				{
					/*write back*/
					if(FLH_Write((INT8U *)sec.start, (INT8U *)buf, sec.size) != 0)
					{
						retval = -4;
						goto fail_erase; 
					}				
					free(buf);
					buf = NULL;
				}	
				size -= len;
				dest += len;
			}	/*end of while*/
			return;
		}	
		image++;	/*move to next on*/
	}	/*end of while*/
	printf("Not found, erase nothing\n");
	return 0;
fail_erase:
	printf("erase firmware fail %d\n",retval);	
	return retval;
}


#endif
