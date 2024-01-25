
/*
 *===========================================================================
 *  Function name: Do_Mem
 *      The purpose of the function is to display or modify the memory content.
 *      Command format:
 *      mem [rwdft][bwl] <address> [length] <data>
 *
 *  Parameter:
 *      argc: number or arguments
 *      argv: pointer point to argument lists
 *
 *  Return value:
 *      None
 *
 *  History:
 *       Create first version by Roger ver. 0.1                   20150428
 *===========================================================================
 */

#include "main.h"
#include "loader.h"
#include "cmd.h"
#include "./sdram/bsp_sdram.h"
#include "./led/bsp_led.h"



/*
************************************************************************************************
*                              Functions
************************************************************************************************
*/


INT32S Do_SDRAM_Test (INT32S argc, INT8S *argv[]);
INT32S Do_SDRAM_Clr(INT32S argc, INT8S *argv[]);
INT32S Do_Mem( INT32S argc , INT8S** argv );

void SDRAM_Check(void);
INT32S Do_New_SDRAM_Test (INT32S argc, INT8S *argv[]);



uint32_t RadomBuffer[10000];
uint32_t ReadBuffer[10000];

uint32_t *pSDRAM;

RNG_HandleTypeDef hrng;


INT32UL *PKBuf0; 
INT32UL *buf;
//extern INT32U top_of_stacks;     /* defined in stack.s */



INT32S Do_Mem( INT32S argc , INT8S** argv )
{
	INT8U* addr , func, *start, *cp, tbuf[17]={0};
	INT32S i = 1 , attrib , flag , /*buf, */size, j;
	INT32U data;
	INT32S	dump_len;
	INT8U mask;
	
	/*Display system memory using status */
	if ( argc == 1 )
	{
		goto mem_op;
	}
	/*control or dump memory content */

	func = argv[i][0];
	switch ( func )
	{
	  case 'd':
		attrib = 0;
		break;	/* dump memory */
	  case 'r':
		attrib = 1;
		break;	/* read memory */
	  case 'w':
		attrib = 2;
		break;	/* write memory */
	  case 'f':
		attrib = 3;
		break;	/* fill memory */
	  case 't':
		attrib = 4;
		break;	/* test memory */
	  default:
		attrib = 0;
		i -= 1;
		break;
	}
	i += 1;

	if ( argc > i )
	{
		if ( strcmp( argv[i] , "w" ) == 0 )
		{
			flag = 1;
			i += 1;
		}
		else if ( strcmp( argv[i] , "b" ) == 0 )
		{
			flag = 0;
			i += 1;
		}
		else if ( strcmp( argv[i] , "l" ) == 0 )  // Ivan add, for DW access
		{
			flag = 2;
			i += 1;
		}
		else
		{
			flag = 0;
		}
	}
	else
	{
		return 0;
	}

	/* address field, physical address in 1M bytes range */
	if ( argc > i )
	{
		addr = ( INT8U * ) atoh( argv[i] );
		i += 1;
	}
	else
	{
		return 0;
	}

	/* Data or length field */
	if ( argc > i )
	{
		data = ( INT32U ) atoh( argv[i] );
	}
	else
	{
		data = 0;
	}
mem_op:
	switch ( attrib )
	{
	  case 1:
		/* read memory operation */
		switch ( flag )
		{
		  case 1:
			/* display word (16-bit) */
			printf( "[MEM] 0x%x -> %2x%2x\n" ,
					addr ,
					*( INT8U * ) ( addr + 1 ) ,
					*( INT8U * ) addr );
			break;
		  case 2:
			/* display double word (32-bit) */
			printf( "[MEM] 0x%x -> %2x%2x%2x%2x\n" ,
					addr ,
					*( INT8U * ) ( addr + 3 ) ,
					*( INT8U * ) ( addr + 2 ) ,
					*( INT8U * ) ( addr + 1 ) ,
					*( INT8U * ) addr );
			break;
		  default:
			printf( "[MEM] 0x%x -> %2x\n" , addr , *( INT8U * ) addr );
			break;
		}
		break;

		
	  case 2:
		/* write memory operation */
		switch ( flag )
		{
		  case 1:
			/* write word */
			mask = ( INT32U ) addr & 0x01;
			switch ( mask )
			{
			  case 0x00:
				/* word boundary: */
				*( INT16U * ) addr = ( INT16U ) data;
				printf( "\n\r [MEM] 0x%x -> %4x\n" , addr , *( INT16U * ) addr );
				break;

			  case 0x01:
				/* byte boundary: */
				*addr = ( INT8U ) ( ( INT16U ) data & 0x00FF );
				*( addr + 1 ) = ( INT8U )
				( ( ( INT16U ) data & 0xFF00 ) >> 8 );
				printf( "\n\r [MEM] 0x%x -> %2x%2x\n" ,
						addr ,
						*( INT8U * ) ( addr + 1 ) ,
						*( INT8U * ) addr );
				break;
			}
			break;

		  case 2:
			/* write double word */
			mask = ( INT32U ) addr & 0x03;
			switch(mask)
			{
				case 0:
					*( INT32U * ) addr = ( INT32U ) data;
					printf( "\n\r [MEM] 0x%x -> %4x\n" , addr , *( INT32U * ) addr );
					break;
				case 1:
					*addr = ( INT8U ) ( ( INT32U ) data & 0x000000FF );
					*(addr+1) = ( INT8U ) (( ( INT32U ) data & 0x0000FF00 )>>8);
					*(INT16U *)(addr+2) = ( INT16U ) ( ( ( INT32U ) data & 0xFFFF0000 )>>16);
					printf( "\n\r [MEM] 0x%x -> %x%x%2x\n" , addr , *( INT8U * ) addr, *( INT8U * ) (addr+1), *( INT16U * ) (addr+2));
					break;							
				case 2:
					*(INT16U *)addr = ( INT16U ) ( ( INT32U ) data & 0x0000FFFF );
					*(INT16U *)(addr+2) = ( INT16U ) ( ( ( INT32U ) data & 0xFFFF0000 )>>16);
					printf( "\n\r [MEM] 0x%x -> %2x%2x\n" , addr , *( INT16U * ) addr, *( INT16U * ) (addr+2));
					break;
				case 3:
					*(INT16U *)addr = ( INT16U ) ( ( INT32U ) data & 0x0000FFFF );
					*(addr+2) = ( INT8U ) (( ( INT32U ) data & 0x00FF0000 )>>16);
					*(addr+3) = ( INT8U ) (( ( INT32U ) data & 0xFF000000 )>>24);
					printf( "\n\r [MEM] 0x%x -> %2x%2x\n" , addr , *( INT16U * ) addr, *( INT8U * ) (addr+2), *( INT8U * ) (addr+3));
					break;
			}
			break;


		  default:
			/* write byte */
			*addr = ( INT8U ) data;
			printf( "\n\r [MEM] 0x%x -> %2x\n" , addr , *( INT8U * ) addr );
			break;
		}
		break;

		
	  case 3:
		/* Fill memory */

		
	  case 4:
		/* Test memory */
		if ( data == 0 )
		{
			dump_len = 0x100;
		}
		else
		{
			dump_len = data;
		}
		/*start address*/
		start = addr;
		if ( attrib == 3 )
		{
			printf( "\n\r [MEM] Fill from 0x%x to 0x%x " , addr , ( addr + dump_len ) );
		}


		#if 0
		else
		{
			if ((INT32U) addr < top_of_stacks)
			{
				printf("overlap with program\n");
				break;
			}	
			else	
				printf( "Test from %p to %p " , addr , ( addr + dump_len ) );
		}
        #endif

		
		i += 1;
		/* Data field */
		if ( argc > i )
		{
			cp = (INT8U *)( argv[i] );
			size = strlen(argv[i]);
		}
		else
		{
			if ( attrib == 3 )
			{
				*(INT32U *)tbuf = 0;
				cp = tbuf;
			}
			else
			{
				*(INT32U *)tbuf = 0xaa5500FF;
				cp = tbuf;
			}
			size = 4;
		}
		/*write loop*/
		for ( i = 0, j=0; i < dump_len; )
		{
			switch ( flag )
			{
				case 1:
					/* write half word */
					*( INT16U * ) addr = ( INT16U ) cp[j];
					addr += 2;
					i += 2;
					j+=2;
					break;
				default:
					*addr = ( INT8U ) cp[j];
					addr += 1;
					i += 1;
					j+=1;
					break;
			}	/* end of switch*/
			j %= size;
			if ( ( i & 0x7FFF ) == 0 )
			{
				printf( "\n\r [MEM] w" );
			}
		}	/*end fo fill loop*/
		if (attrib == 4)
		{
			j=0;
			addr = ( INT8U * ) start;
			for ( i = 0, j=0; i < dump_len; i++)
			{	 /*Memory test*/
				if ( *( INT8U * ) addr != cp[j] )
				{
					printf( "\n\r [MEM] Memory test fail at 0x%x: write %x, read %x\n" ,
							addr ,
							cp[j] ,
							*( INT16U * ) addr );
					break;	/* exit loop */
				}
				addr += 1;
				j += 1;
				j %= size;
				if ( ( i & 0x7FFF ) == 0 )
				{
					printf( "." );
				}
			}	 /*end of for loop */
		}  
		printf( "\n\r [MEM] OK" );
		break;
		
	  default:
		/* dump memory operation */
		printf( "\n\r [MEM] Start address: 0x%x" , addr );
		if ( data == 0 )
		{
			dump_len = 0x100;
		}
		else
		{
			dump_len = data;
		}
		j=0;
		for ( i = 0; i < dump_len; )
		{
			if ((i%16)==0)
				printf( "\n\r [MEM] 0x%x  " , addr );
			
			switch ( flag )
			{
			  case 1:
				/* Dump words */
				printf( "\n\r [MEM] %2x%2x " ,
						*( INT8U * ) ( addr + 1 ) ,
						*( INT8U * ) addr );
				tbuf[j] = *( INT8U * ) addr;
				tbuf[j+1] = *( INT8U * ) (addr+1);
				addr += 2;
				i += 2;
				j += 2;
				break;
			  case 2:
				/* Dump double words */
				printf( "\n\r [MEM] %2x%2x%2x%2x " ,
						*( INT8U * ) ( addr + 3 ) ,
						*( INT8U * ) ( addr + 2 ) ,
						*( INT8U * ) ( addr + 1 ) ,
						*( INT8U * ) addr );
				tbuf[j] = *( INT8U * ) addr;
				tbuf[j+1] = *( INT8U * ) (addr+1);
				tbuf[j+2] = *( INT8U * ) (addr+2);
				tbuf[j+3] = *( INT8U * ) (addr+3);
				addr += 4;
				i += 4;
				j += 4;
				break;
			  default:
				/* Dump bytes */
				printf( "\n\r [MEM] %2x " , *( INT8U * ) addr );
				tbuf[j] = *( INT8U * ) addr;
				addr += 1;
				i += 1;
				j += 1;
				break;
			}	/*end of switch */

			#if 1
			if ( j >= 16 )
			{
				for ( j = 0; j < 16; j++ )
				{	// roger 20150608 mark to avoid gibberish  
					//tbuf[j] = ( tbuf[j] < 0x20 ) ? '.' : tbuf[j];    
					//printf("\n\r %c",tbuf[j]);
				}
				j=0;
			}
			#endif
			
		}	/* end of for loop */
		printf( "\n\r" );
		break;
	}	/* end of attrib switch */
	return 0;
}



INT32S Do_SDRAM_Test (INT32S argc, INT8S *argv[])
{

INT32UL *arrayint	= (INT32UL*) PKBuf0;
INT16U	*arrayshort = (INT16U*)	PKBuf0;
INT8U	*arraychar	= (INT8U*)	PKBuf0;

INT32UL  i;
INT16U	 j;
INT8U	 k;
INT32UL  patternint1   = 0x55555555;
INT32UL  patternint2   = 0xAAAAAAAA;
INT16U	 patternshort1 = 0x5555;
INT16U	 patternshort2 = 0xAAAA;
INT8U	 patternchar1  = 0x55;
INT8U	 patternchar2  = 0xAA;


printf("\n\r [SDRAM] PKBuf0 Start Address = 0x%lx\n\r", PKBuf0);    // PKBuf0 is the starting addr of SDRAM test

/*
	 STM32F429IGT6 EVB now use 1pcs Micron MT48LC16M16A2 SDRAM, and its size is 16Mx16bit (256Mbits = 32MB), 

	 FOR MT48LC16M16A2 (16Mx16 bits = 32MB = 8388608 words), 
*/


#if (WORD_TEST_0x55555555==1)


// word Testing 1

printf("\n\r [SDRAM] (1) run SDR word  testing program pattern=0x55555555\n\r");


#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784;i++) 	
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000;i++) 	
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i< 3453000;i++) 	
#endif
	{	
	 arrayint[i]=patternint1;
 #if 1
	 if ((i%1000000)==0)
		{
		 printf("\n\r [SDRAM] arrayint[%d]=0x55555555\n\r",i);
		}
 #endif
	}
  


//for(i=0;i<16252000;i++)		// 16252000 word =	65008000 byte ~= 62MB, so 62MB+2MB = 64MB

#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784;i++) 	// 15990784 word =	63963136 byte ~= 61MB 
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000;i++) 	
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i<3453000;i++)	
#endif

	{	
		if(arrayint[i]!=patternint1)
			{	
				if(arrayint[i]!=patternint1)
				{printf("\n\r [SDRAM] !!!error occur when SDR word write 1: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arrayint[i],patternint1);} 	 
				else
				{printf("\n\r [SDRAM] !!!error occur when SDR word read 1: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arrayint[i],patternint1);}
				 
				return WORD_TEST_0x55555555_PATTERN;
			}		
	}


#endif




#if (WORD_TEST_0xAAAAAAAA==1)


// word Testing 2

printf("\n\r [SDRAM] (2) run SDR word  testing program pattern=0xAAAAAAAA\n\r");


#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784;i++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000;i++) 	
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i<3453000;i++)	
#endif
	{	
	 arrayint[i]=patternint2;
 #if 1
	 if ((i%1000000)==0)
		{
		 printf("\n\r [SDRAM] arrayint[%d]=0xAAAAAAAA\n\r",i);
		}
 #endif
	}


#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784;i++) 
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000;i++) 	
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i<3453000;i++)
#endif

	{	
		if(arrayint[i]!=patternint2)
			{	
				if(arrayint[i]!=patternint2)
				{printf("\n\r [SDRAM] !!!error occur when SDR word write 2: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arrayint[i],patternint2);}
				else
				{printf("\n\r [SDRAM] !!!error occur when SDR word read 2: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arrayint[i],patternint2);}

				return WORD_TEST_0xAAAAAAAA_PATTERN;
			 
			}		
	}



#endif




#if (WORD_TEST_16252000==1)


// word Testing 3

printf("\n\r [SDRAM] (3) run SDR word  testing program pattern=0~16252000\n\r");


#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784;i++) 
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000;i++) 	
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i<3453000;i++)	
#endif
	{	
	 arrayint[i]=i;
 #if 1
	 if ((i%1000000)==0)
		{
		 printf("\n\r [SDRAM] arrayint[%d]=%d\n\r",i,i);
		}
 #endif
	}
	
#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784;i++) 
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000;i++) 		
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i<3453000;i++)	
#endif

	{	
		if(arrayint[i]!=i)
			{	
				if(arrayint[i]!=i)
				{printf("\n\r [SDRAM] !!!error occur when SDR word write3 : ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arrayint[i],i);}
				else
				{printf("\n\r [SDRAM] !!!error occur when SDR word read3:  ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arrayint[i],i);}
		 
				 return WORD_TEST_16252000_PATTERN;
			 
			}		
	}


#endif



#if (SHORT_TEST_0x5555==1)


// short Testing 1

printf("\n\r [SDRAM] (4) run SDR short testing program pattern=0x5555\n\r");


#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784*2;i++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000*2;i++)
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i<3453000*2;i++)	
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2)
for(i=0;i< 4194304*2;i++) 		
#endif

	{	
	 arrayshort[i]=patternshort1;	
	}

#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784*2;i++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000*2;i++)
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i<3453000*2;i++)
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2)	
for(i=0;i< 4194304*2;i++)	
#endif


	{	
		if(arrayshort[i]!=patternshort1)
			{	
				if(arrayshort[i]!=patternshort1)
				{printf("\n\r [SDRAM] !!!error occur when SDR short write 1: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arrayshort[i],patternshort1);}
				else
				{printf("\n\r [SDRAM] !!!error occur when SDR short read 1: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arrayshort[i],patternshort1);}

				return SHORT_TEST_0x5555_PATTERN;
			 
			}		
	}


#endif



#if (SHORT_TEST_0xAAAA==1)


// short Testing 2

printf("\n\r [SDRAM] (5) run SDR short testing program pattern=0xAAAA\n\r");


#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784*2;i++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000*2;i++)
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i<3453000*2;i++)	
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2) 	
for(i=0;i< 4194304*2;i++)	
#endif


	{	
	 arrayshort[i]=patternshort2;	
	}


#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784*2;i++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000*2;i++)
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i<3453000*2;i++)
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2)	
for(i=0;i< 4194304*2;i++)	
#endif

	{	
		if(arrayshort[i]!=patternshort2)
			{	
				if(arrayshort[i]!=patternshort2)
				{printf("\n\r [SDRAM] !!!error occur when SDR short write 2: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arrayshort[i],patternshort2);}
				else
				{printf("\n\r [SDRAM] !!!error occur when SDR short read 2: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arrayshort[i],patternshort2);}

				return SHORT_TEST_0xAAAA_PATTERN;
			 
			}		
	}


#endif



#if (SHORT_TEST_65534==1)


// short Testing 3

printf("\n\r [SDRAM] (6) run SDR short testing program pattern=0~65534\n\r");

#if (SDRAM_OPTION == ESMT_M12L256X2)
for(j=0;j<65535;j++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(j=0;j<65535;j++)	
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(j=0;j<65535;j++)
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2)
for(i=0;i< 65535;i++) 	
#endif

	{	
	 arrayshort[j]=j;	
	}


#if (SDRAM_OPTION == ESMT_M12L256X2)
for(j=0;j<65535;j++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(j=0;j<65535;j++)	
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(j=0;j<65535;j++)
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2)
for(i=0;i< 65535;i++) 	
#endif

	{	
		if(arrayshort[j]!=j)
			{	
				if(arrayshort[j]!=j)
				{printf("\n\r [SDRAM] !!!error occur when SDR short write 3: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",j,arrayshort[j],j);}
				else
				{printf("\n\r [SDRAM] !!!error occur when SDR short read 3: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",j,arrayshort[j],j);}

				return SHORT_TEST_65534_PATTERN;
			 
			}		
	}


#endif



#if (CHAR_TEST_0x55==1)


// char Testing 1

printf("\n\r [SDRAM] (7) run SDR char  testing program pattern=0x55\n\r");


#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784*4;i++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000*4;i++)	
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i<3453000*4/4;i++)
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2)	
for(i=0;i< 4194304*4;i++)	
#endif

	{	
	 arraychar[i]=patternchar1; 
	}



#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784*4;i++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000*4;i++)	
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i<3453000*4/4;i++)	
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2) 	
for(i=0;i< 4194304*4;i++)		
#endif

	{	
		if(arraychar[i]!=patternchar1)
			{	
				if(arraychar[i]!=patternshort1)
				{printf("\n\r [SDRAM] !!!error occur when SDR char write 1: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arraychar[i],patternchar1);}
				else
				{printf("\n\r [SDRAM] !!!error occur when SDR char read 1: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arraychar[i],patternchar1);}

				return CHAR_TEST_0x55_PATTERN;
			 
			}		
	}


#endif



#if (CHAR_TEST_0xAA==1)

	
// char Testing 2

printf("\n\r [SDRAM] (8) run SDR char  testing program pattern=0xAA\n\r");


#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784*4;i++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000*4;i++)		
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i<3453000*4/4;i++)	
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2)	
for(i=0;i< 4194304*4;i++)		
#endif

	{	
	 arraychar[i]=patternchar2; 
	}


#if (SDRAM_OPTION == ESMT_M12L256X2)
for(i=0;i<15990784*4;i++)	
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(i=0;i<15980000*4;i++)		
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(i=0;i<3453000*4/4;i++)	
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2)
for(i=0;i< 4194304*4;i++)		
#endif

	{	
		if(arraychar[i]!=patternchar2)
			{	
				if(arraychar[i]!=patternshort1)
				{printf("\n\r [SDRAM] !!!error occur when SDR char write 2: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arraychar[i],patternchar2);}
				else
				{printf("\n\r [SDRAM] !!!error occur when SDR char read 2: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",i,arraychar[i],patternchar2);}

				return CHAR_TEST_0xAA_PATTERN;
			 
			}		
	}


#endif




#if (CHAR_TEST_254==1) 


// char Testing 3

printf("\n\r [SDRAM] (9) run SDR char  testing program pattern=0~254\n\r");

#if (SDRAM_OPTION == ESMT_M12L256X2)
for(k=0;k<255;k++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(k=0;k<255;k++)
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(k=0;k<255;k++)
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2)
for(i=0;i< 255;i++) 	
#endif

	{	
	 arraychar[k]=k;	
	}


#if (SDRAM_OPTION == ESMT_M12L256X2)
for(k=0;k<255;k++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(k=0;k<255;k++)
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(k=0;k<255;k++)
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2)
for(i=0;i< 255;i++) 	
#endif
	
	{	
		if(arraychar[k]!=k)
			{	
				if(arraychar[k]!=k)
				{printf("\n\r [SDRAM] !!!error occur when SDR char write 3: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",k,arraychar[k],k);}
				else
				{printf("\n\r [SDRAM] !!!error occur when SDR char read 3: ");
				 printf("\n\r [SDRAM] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",k,arraychar[k],k);}

				return CHAR_TEST_254_PATTERN;
			 
			}		
	}


#endif


printf("\n\r");


return 0;


}



INT32S Do_SDRAM_Clr (INT32S argc, INT8S *argv[])
{

INT16U	*arrayshortzero  = (INT16U*)	PKBuf0;
INT32UL   m;
INT16U    patternzero     =0;

printf("\n\r [SDRAM Clear] PKBuf0 Start Address = 0x%lx\n\r", PKBuf0);    // PKBuf0 is the starting addr of SDRAM test

/*
	 STM32F429IGT6 EVB now use 1pcs Micron MT48LC16M16A2 SDRAM, and its size is 16Mx16bit (256Mbits = 32MB), 

	 FOR MT48LC16M16A2 (16Mx16 bits = 32MB = 8388608 words), 
*/


#if (SHORT_TEST_0x0000==1)

// short Testing 1

printf("\n\r [SDRAM Clear] (0) run SDR short testing program pattern=0\n\r");


#if (SDRAM_OPTION == ESMT_M12L256X2)
for(m=0;m<15990784*2;m++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(m=0;m<15980000*2;m++)
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(m=0;m<3453000*2;m++)	
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2)
for(m=0;m< 4194304*2;m++) 		
#endif

	{	
	 arrayshortzero[m]=patternzero;	
	}

#if (SDRAM_OPTION == ESMT_M12L256X2)
for(m=0;m<15990784*2;m++)
#elif (SDRAM_OPTION == WINBOND_9825G6EHX2)
for(m=0;m<15980000*2;m++)
#elif (SDRAM_OPTION == ISSI_IS42S324)
for(m=0;m<3453000*2;m++)
#elif (SDRAM_OPTION == MICRON_MT48LC16M16A2)	
for(m=0;m< 4194304*2;m++)	
#endif

	{	
	    if(arrayshortzero[m]!=patternzero)
		{	
		    if(arrayshortzero[m]!=patternzero)
		    {
		     printf("\n\r [SDRAM Clear] !!!error occur when SDR short write 1: ");
		     printf("\n\r [SDRAM Clear] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",m,arrayshortzero[m],patternzero);
		    }
		    else
		    {
		     printf("\n\r [SDRAM Clear] !!!error occur when SDR short read 1: ");
		     printf("\n\r [SDRAM Clear] PKBuf0[%d] 0x%08lx != 0x%08lx \n\r",m,arrayshortzero[m],patternzero);
		    }			  
		    return SHORT_TEST_0x0000_PATTERN;		 
		}		
	}

#endif

printf("\n\r");

return 0;

}


INT32UL *initMemoryPool(INT32UL *addr)  
{
	PKBuf0               = (INT32UL *)addr;  // Addr of PKBuf0 assign to addr; 
  return PKBuf0;   
}



////////////////////////////////////////////////////////////////////////////////


void SDRAM_Check(void)
{
	long long count=0,sdram_count=0;
 	pSDRAM=(uint32_t*)SDRAM_BANK_ADDR;
	printf("\r\n [SDRAM_Check] Write data into SDRAM \r\n");
	
	for(sdram_count=0;sdram_count<SDRAM_SIZE/4;sdram_count++)
	{
		*pSDRAM=RadomBuffer[count];
		count++;
		pSDRAM++;
		if(count>=10000)

		{
			count=0;
		}
	}
	
	printf("\r\n [SDRAM_Check] Total bytes written :%d \r\n",(uint32_t)pSDRAM-SDRAM_BANK_ADDR);

	count=0;
	pSDRAM=(uint32_t*)SDRAM_BANK_ADDR;
	printf("\r\n [SDRAM_Check] Start to read SDRAM and compare with the original random number \r\n");
	sdram_count=0;
	for(;sdram_count<SDRAM_SIZE/4;sdram_count++)
	{
		if(*pSDRAM != RadomBuffer[count])
		{
			printf("\r\n [SDRAM_Check] Data comparison error - exit ~ \r\n");
			break;
		}
		count++;
		pSDRAM++;
		if(count>=10000)
		{
			count=0;
		}
	}

	printf("\r\n [SDRAM_Check] Compare By Total Bytes: %d \r\n",(uint32_t)pSDRAM-SDRAM_BANK_ADDR);

	if(sdram_count == SDRAM_SIZE/4)
	{
		LED_GREEN;
		printf("\r\n [SDRAM_Check] SDRAM test successful \r\n");
	}
	else
	{
		LED_RED;
		printf("\r\n [SDRAM_Check] SDRAM test fail \r\n");
	}   
}


//////////////////////////////////////////////////////////////////////////////////////////////////////


INT32S Do_New_SDRAM_Test (INT32S argc, INT8S *argv[])
{
	long long count=0;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;


	/* The blue LED is on, indicating that the SDRAM is being RW test. */
	LED_BLUE;
   
	/* Select PLL output as Random Number Generator (RNG) clock source */
	/*
	   In most STM32 microcontrollers, the RNG module is clocked by the internal RC oscillator (HSI) divided by a prescaler. 

	*/

#if 1

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RNG;
	PeriphClkInitStruct.RngClockSelection = RCC_RNGCLKSOURCE_PLL;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	/* Enable RNG Clock*/
	__HAL_RCC_RNG_CLK_ENABLE();
	/* Initialize the RNG module to generate random numbers */
	hrng.Instance = RNG;
	HAL_RNG_Init(&hrng);

#endif

	SDRAM_Test();

#if 1	

	printf("\r\n [2] [Random_Numbers] Start generating 10000 SDRAM test random numbers \r\n");	 
	for(count=0;count<10000;count++)

	{
		HAL_RNG_GenerateRandomNumber(&hrng,&RadomBuffer[count]);

	}	 
	printf("\r\n [2] [Random_Numbers] 10,000 SDRAM test random numbers are generated \r\n");

	SDRAM_Check();

#endif

}






