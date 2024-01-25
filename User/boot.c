/*do boot, check the kernel image at specified address and */
/*decompress and load to specified address then boot up*/
#include "main.h"
#include "cmd.h"
#include "loader.h"
#include "inflate.h"
#include "flash.h"
//#include "linux.h"


#if 1    // Roger
typedef  void (*pFunction)(void);
pFunction JumpToApplication;

#endif

#if 0    // Roger
pFunction Jump_To_Application_Boot;
uint32_t JumpAddress_Boot;
#endif



//#define LOADER_INFO_BASE	0x40

extern const INT8S PlatformID[LEN_PLATFORMID];
//static struct atag *params; /* used to point at the current tag */
//const  char cmdline[] ="root=/dev/ram0 console=ttyS0,115200";
//VOID Boot_Linux();
extern INT32S TftpBufAddr;        // Roger 20151109 add


INT32U RamBoot=0; 		/*bit 0: kernel ready, bit 1 filesystem ready*/	
/// extern INT32U BuildDateCheck(INT32S type, IMG_HEADER *image_hdr0, IMG_HEADER *image_hdr1, INT32U base0, INT32U base1);
extern INT32U GetFirmwarePosition(CHAR *type, INT32U ver);
extern struct	SYS_VENDOR_DEFINE SYS_Vendor;		

extern int strnicmp(const char *s1, const char *s2, int len);




/**
  * @brief  ����CPU L1-Cache.
  * @param  ��
  * @retval ��
  */
static void CPU_CACHE_Disable(void)
{
  /* ��ֹI-Cache */
  SCB_DisableICache();

  /* ��ֹD-Cache */
  SCB_DisableDCache();
}




	
/*image integrated check*/
INT32S image_check(INT32S type, INT8U *start, INT32S size, INT8U *sha1_crc)
{
	INT8U chk[LEN_SHA1];
	CRC_CONTEXT crc;
	SHA1_CONTEXT sha;
	
	printf("\n\r checking at 0x%lx...\n", (INT32S)start);
	switch(type)
	{
		case IMG_CHECK_NONE:
		break;
		case IMG_CHECK_SHA1:			
			sha1_starts(&sha);
			sha1_update(&sha, start, size);
			sha1_finish(&sha, chk);
			if (memcmp(sha1_crc, chk, LEN_SHA1)!=0) 
			{
				printf("\n\r The SHA1 check of image is incorrect\n");
				return -2;
			}
		break;
		case IMG_CHECK_CRC:
			crc_starts(&crc);
			crc_update(&crc, start, size);
			crc_finish(&crc, (unsigned long *)chk);
			if(memcmp(sha1_crc, chk, LEN_CRC) != 0)
			{
				printf("\n\r The CRC check of image is incorrect\n");
				return -3;
			}
		break;
		default:	
		printf("n\r image type is incorrect\n");
		return -4;
	}	/*end of switch*/
	/*OK*/
	printf("n\r image check ok\n");
	return 0;
}	/*end of check image*/



#if 0

/*do decompress image*/
INT32S decompress(INT8U *dest, INT8U *src, INT32S size)
{
	INT32S retval;
	/*initial global which used in decompress*/
	variable_init(dest, src, size);

	makecrc();
	
	/* decompress the kernel */
    if ((retval = gunzip()) < 0)
    {
    	printf("Unzip error\n");
        return retval; /* decompression fail */
    }
    return 0;
}

#endif



#if 1

/*boot up kernel, when loader up*/
INT32S Boot(IMG_HEADER *hdr, INT32S addr, INT32S wait)
{
//	VOID (*entry)(VOID);
	//INT32U size,type;
	INT32U size;
	//INT8U *sha1_crc;

	printf("\n\r ===== Start to boot up APP =====\n");
	//printf("\n\r !!! Press any key to exit boot up procedure!!!\n");

	#if 0
	addr += MAGIC_LEN;
      #endif


      #if 1
	if (strncmp((INT8S*)hdr->platform_id, PlatformID, LEN_PLATFORMID)!=0) 
	{
		printf("\n\r kernel image is incorrect at 0x%lx\n",addr);
		return -1;
	}
      #endif


	#if 1
	//type = hdr->check_type;
	size = sizeof(IMG_HEADER)+hdr->image_size;
	//sha1_crc = (INT8U *)(addr + size);
	printf("\n\r %s ",hdr->image_type);
    #endif

	#if 0    // so far, header & APP image are not put addr continuously. So the CRC check will error. Turnoff the crc check temporarily
	if (image_check(type, (INT8U *)addr, size, sha1_crc) != 0)
	{
		printf("\n\r firmware image is incorrect at 0x%lx,\n",addr);
		return -2;	  
	}
	#endif


	/*disable interrupt*/
	//Disable();

      switch (addr)   // the initial addr value should be 8020000
      	{
		 case DM_SPI_FLH_APP1_HEADER:
		      addr=DM_SPI_FLH_APP1_START;
		 break;	   
      	}
		

	#if  0

		 printf("\n\r addr is [0x%lx]\n",addr);	 

		 //#define APPLICATION_ADDRESS	 ((INT8S*)addr)   //  roger 20150507 add	 
	
		  if (((*(__IO uint32_t*)((INT8S*)addr)) & 0x2FFE0000 ) == 0x20000000)  // if ISR not exist, this program won't jump to 0x08010000
		 {	   
			/* Initialize user application's Stack Pointer */
			__set_MSP(*(__IO uint32_t*) ((INT8S*)addr));

			#if 0
			JumpAddress = *(__IO uint32_t*) (((INT8S*)addr) + 4);
			Jump_To_Application = (pFunction) JumpAddress;
			Jump_To_Application();	
			#endif

			#if 1
			JumpAddress_Boot = *(__IO uint32_t*) (((INT8S*)addr) + 4);
			Jump_To_Application_Boot = (pFunction) JumpAddress_Boot;
			Jump_To_Application_Boot();
			#endif
			
		 }		 

	#endif


	#if  1

			printf("\n\r addr is [0x%lx]\n",addr);	 

			/* Disable CPU L1 cache before jumping to the QSPI code execution */
			CPU_CACHE_Disable();
			/* Disable Systick interrupt */
			SysTick->CTRL = 0;
			/* Initialize user application's Stack Pointer & Jump to user application */
			JumpToApplication = (pFunction) (*(__IO uint32_t*) (((INT8S*)addr) + 4));
			__set_MSP(*(__IO uint32_t*) ((INT8S*)addr));
			JumpToApplication();


	#endif	

      //Enable();
	
    return 0;

}	/*end Boot*/


/*check and list available firmware*/
INT32U ListAvailableFirmware(CHAR *type[], INT32U image_base[], IMG_HEADER *image_hdr[])
{
	IMAGE_POS_T *image;
	IMG_HEADER hdr;
	INT32U count=0;
	INT32U addr_acc=0;


	//INT8U *start = (INT8U *)LoadBufAddr, *paddr;
	//INT32UL  *arrayint	= (INT32UL*) PKBuf0;

		
	image = (IMAGE_POS_T *)ImagePosition;    
	
	printf("\n\r image name is %s,\n",image->name);  
	printf("\n\r image address is 0x%lx,\n",&image); 

	
	//printf("filesystem image incorrect at 0x%lx,\n",addr);
    //while(1);      // roger 20230808 add for debug
	
	
	while(image->name!=NULL)
	{
		/*match type*/
		#if 0
		if (strnicmp(type[0], image->name, strlen(image->name)) == 0 ||
		    strnicmp(type[1], image->name, strlen(image->name)) == 0)
             #endif

		#if 1
		if (strnicmp(type[2], image->name, strlen(image->name)) == 0)   // if type[2] == HEADER
             #endif			 
		
		{	
			#if 1      // print out image default value
			printf("\n\r =====================================================\n");	
			printf("\n\r [0] image address is 0x%lx\n",image);
			printf("\n\r [0] type[0] is %s,\n",type[0]);
			printf("\n\r [0] type[1] is %s,\n",type[1]);  
			printf("\n\r [0] type[2] is %s,\n",type[2]);  
			printf("\n\r [0] image name is %s,\n",image->name);
			printf("\n\r [0] image subid is 0x%lx,\n",image->subid);
			printf("\n\r [0] image base addr is 0x%lx,\n",image->base);
			printf("\n\r [0] image size is 0x%lx,\n",image->size);
			printf("\n\r *****************************************************\n");	
			#endif

            #if 0	
			memcpy((CHAR *)&hdr, (CHAR *)(image->base+MAGIC_LEN), sizeof(IMG_HEADER));
			/* check magic string */			
            #endif 

            //while(1);      // roger 20230808 add for debug

			#if 1	
		      memcpy((CHAR *)&hdr, (CHAR *)(image->base), sizeof(IMG_HEADER));      // copy APP-1 header from spi-flash 0x91fe_0000L
			  // void * memcpy(void * destination, const void * source, size_t num);
		   
	   		#endif 
			   
			
            #if 1
			//printf("\n\r [0] memory copy source addr is 0x%lx,\n",(CHAR *)(image->base+MAGIC_LEN));   // roger 20150618 mark
			printf("\n\r [0] memory copy source addr is 0x%lx,\n",(CHAR *)(image->base));                       // roger 20150618 add
			printf("\n\r [0] memory copy destination addr is 0x%lx,\n",(CHAR *)&hdr);
			printf("\n\r [0] memory copy image_header size is 0x%lx byte,\n",sizeof(IMG_HEADER));
			printf("\n\r =====================================================\n");	
            #endif

			#if 1	
			printf("\n\r [0] *(INT32U *)image->base addr =0x%lx\n",image->base);
			printf("\n\r [0] *(INT32U *)image->base data =0x%lx\n",*(INT32U *)image->base);
			printf("\n\r [0] VENDOR_MAGIC No =0x%lx\n",VENDOR_MAGIC);	   
			#endif

			printf("\n\r [0] (CHAR *)hdr.platform_id = %s\n",(CHAR *)hdr.platform_id);
			printf("\n\r [0] (CHAR *)PlatformID = %s\n",(CHAR *)PlatformID); 	  


            #if 1        // roger 20230808 add, header info. parsing from SPI-Flash APP-1 header
			printf("\n\r [0] hdr.platform_id = %s\n",(CHAR *)hdr.platform_id);
			printf("\n\r [0] hdr.version = %lx\n",(INT32U *)hdr.version);
		    printf("\n\r [0] hdr.image_type = %s\n",(CHAR *)hdr.image_type);			
			printf("\n\r [0] hdr.image_size = %d\n",(INT32U *)hdr.image_size);
			printf("\n\r [0] hdr.entry_addr = 0x%lx\n",(INT32U *)hdr.entry_addr);
		    printf("\n\r [0] hdr.check_type = %d\n",(INT8U *)hdr.check_type);	

            #endif

			
		    #if 0    // because program is executed at SDRAM, so we don't check hdr.platform_id here	
			/* check magic string */ 
			if (*(INT32U *)image->base == VENDOR_MAGIC &&
				strncmp((CHAR *)hdr.platform_id, (CHAR *)PlatformID, LEN_PLATFORMID)==0)
			#endif


            #if 0    // roger 20150618 mark, to parse header content directly instead of parse ISR                   
			if (((*(INT32U *)image->base) & 0x2FFE0000)== 0x20000000)
            #endif 

            // while(1);      // roger 20230808 add for debug

	   		#if 1   // roger 20150618 add			
		    // check PlatformID in header is exactly the same as default
			if (strnicmp((CHAR *)hdr.platform_id, (CHAR *)PlatformID, strlen((CHAR *)PlatformID)) == 0)   	
	   		#endif 
			   
			{
	  			#if 0
				printf("\n\r [0] (CHAR *)hdr.platform_id = %s\n",(CHAR *)hdr.platform_id);
				printf("\n\r [0] (CHAR *)PlatformID = %s\n",(CHAR *)PlatformID); 	  
  				#endif		   
			
				//image_hdr[count]=(IMG_HEADER *)(image->base+MAGIC_LEN);		// roger 20150618 mark
				image_hdr[count]=(IMG_HEADER *)(image->base);    				// roger 20150618 add
				image_base[count]=image->base;

				//while(1); 	 // roger 20230808 add for debug


				#if 1		   // if crc calculate is correct, recognize the correctness of this image 
				
				// INT32S image_check(INT32S type, INT8U *start, INT32S size, INT8U *sha1_crc), (check type => CRC or SHA, APP image addr, APP image size, crc 4 byte starting addr.)
				
				//if (image_check(hdr.check_type, (hdr.entry_addr-sizeof(IMG_HEADER)), hdr.image_size, 0) == 0)
				
				//if (image_check(type, (INT8U *)(addr-sizeof(IMG_HEADER)), size, sha1_crc) != 0)
				
				//==============================================================================================
				//==============================================================================================
				//==============================================================================================

				// INT32S image_check(INT32S type, INT8U *start, INT32S size, INT8U *sha1_crc)
                //
                // INT32S image_check ( crc/sha1, header starting addr, header_image data size, crc data addr)
                //
				
			    if (image_check(hdr.check_type, (INT8U *)(hdr.entry_addr-sizeof(IMG_HEADER)), (sizeof(IMG_HEADER)+hdr.image_size), (INT8U *)(hdr.entry_addr+hdr.image_size)) == 0)						
                   {
				   count++;
				   printf("\n\r [0] correct APP count = %d\n",count); 	  
                   }
				else 
				   printf("\n\r [0] No correct APP available ! \n\r");	   
				   	

                #endif

				#if 0
				count++;
				printf("\n\r [0] count = %d\n",count); 
                #endif
				
			}              
			
		}

		else   // roger 2014/8/6add
			{
				addr_acc++;
                printf("\n\r [1] addr_acc_times %d,\n",addr_acc);
			}		
		
		image++;	/*move to next on*/
		
	}	/*end of while*/
	
	return count;   // the FW numbers which meet requirement  
	
}





/*select from available image, get newes or oldest one to boot*/
/*type ==1, newest, 0: oldest*/
VOID SelectBoot(INT32S type, INT32S count, IMG_HEADER **image_hdr, INT32U *image_base)
{
	INT32S i;
	
	for(i=1;i<count;i++)
	{
		/*check firmware date*/	
///		if (BuildDateCheck(type,image_hdr[0],image_hdr[i],
///			image_base[0],image_base[i])==image_base[i])
		{
			image_hdr[0]=image_hdr[i];
			image_base[0]=image_base[i];
		}	
	}

	switch (image_base[0])	// the initial addr value should be 8020000
	  {
	  case FLH_APP1_HEADER:
			image_base[0]=FLH_APP1_START;
			break;    
	  }
	
	printf("\n\r image_base[0] is [0x%lx]\n",image_base[0]);	
	Boot(image_hdr[0], image_base[0], 0);
}

#endif


#if 0

/*boot up command*/
INT32S Do_Boot(INT32S argc, INT8S *argv[])
{
	CHAR *type[2]={NULL,NULL};
	INT32S index = -1,i;
	IMG_HEADER hdr,*image_hdr[4]={NULL,NULL,NULL,NULL};
	IMAGE_POS_T *image;
	INT32U count=0,image_base[4]={0,0,0,0};

	if (argc ==1)	/*not specify boot whom, choice newest one*/
	{
		type[0] = FIRMWARE_TYPE;
		type[1] = LINUX_KERNEL_TYPE;
	} 
	else if (argc >= 2)
	{
		if (stricmp(argv[1], FIRMWARE_TYPE)==0 ||
			stricmp(argv[1], LINUX_KERNEL_TYPE)==0)
			type[0] = argv[1];
		else
		{
			if (SYS_Vendor.boot_sys && RamBoot==0x03 &&
			 	stricmp(argv[1], "debug")==0)
			{
				void (*theKernel)(VOID);
				INT32U exec_at = KERNEL_RAM_ADDR + 0x30;
				
				theKernel = (VOID (*)())exec_at;
				(*theKernel)();				
			}
			else
				printf("boot firmware type mismatch\n");	
			return;
		}
		if (argc==3)
		{	
			index = atoi(argv[2]);
			if (index >=2)
			{
				printf("boot firmware number incorrect\n");	
				return;
			}
		}	
	}	
	/*check flash and list all available firmware*/	
	count = ListAvailableFirmware(type, image_base, image_hdr);
	if (count==0)
	{
		printf("no image available\n");
		return;
	}	
	if (index>=0 && index < count)
		Boot(image_hdr[index], image_base[index], 0);
	else
		SelectBoot(1, count, image_hdr, image_base);
	/*clear RamBoot*/	
	RamBoot=0;
	return 0;
}

#endif




/*reboot command*/
INT32S Do_Reboot(INT32S argc, INT8S *argv[])
{
	/*write reset code to system reset register to do restart*/	
	//SYSC_CHIP_RESET = RESET_CODE;

	*((volatile unsigned *)(0xE000ED0C)) = 0x05FA0004;

	while(1);
	
}


