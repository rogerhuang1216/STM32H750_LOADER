/*
 * P1620 loader, boot loader command list 
 * create by kenny 12-08-2008
 * modified by roger huang 02/22/2012
 */
#include "main.h"
#include "cmd.h"



INT32S Do_Help (INT32S argc, INT8S *argv[]);
extern INT32S Do_Reboot (INT32S argc, INT8S *argv[]);
extern INT32S Do_Flash(INT32S argc, INT8S *argv[]);
extern INT32S Do_Qspimemmap(INT32S argc, INT8S *argv[]);
extern INT32S Do_QSPI_ResetMemory(INT32S argc, INT8S *argv[]);
extern INT32S Do_SpiFlash(INT32S argc, INT8S *argv[]);
extern INT32S Do_Spi_FLASH_RW_Direct (INT32S argc, INT8S *argv[]);
extern INT32S Do_Spi_FLASH_RW_Indirect (INT32S argc, INT8S *argv[]);
extern INT32S Do_SDRAM_Test (INT32S argc, INT8S *argv[]);
extern INT32S Do_SDRAM_Clr (INT32S argc, INT8S *argv[]);
extern INT32S Do_New_SDRAM_Test (INT32S argc, INT8S *argv[]);
extern INT32S Do_Mem(INT32S argc, INT8S *argv[]);
extern INT32S Do_Upgrade (INT32S argc, INT8S *argv[]);
extern INT32S Do_Bootime (INT32S argc, INT8S *argv[]);
extern INT32S Do_SpiFlash_Update (INT32S argc, INT8S *argv[]);



#if 0

struct cmd_tbl_s 
{
	INT8S	*name;						/* Command Name	*/
	INT32S	maxargs;						/* maximum number of arguments */
	INT32S	minargs;						/* minimum number of arguments */
	INT32S	repeatable;					/* autorepeat allowed? */			
	INT32S	(*func)(INT32S, INT8S *[]);	/* Implementation function	*/
	INT8S	*usage;						/* Usage message (short) */
	INT8S	*help;						/* Help message	(long) */
};

#endif



extern INT32S stricmp( const INT8S* s1 , const INT8S* s2 );


cmd_tbl_t CmdTable[]=
{
	{
		"?",   1, 1, 0,	Do_Help,
		"? [command name] \n\r",
		"display supported commands, if not specified, list all commands \n\r"
	},
	
	{
		"help",   1, 1, 0, Do_Help,
		"help [command name] \n\r",
		"display supported commands, if not specified, list all commands \n\r"
	},

	{
		"reboot",  1, 1, 0, Do_Reboot,
		"Type reboot directly \n\r",
		"restart system \n\r"
	},		

	{
		"flash",   7, 7, 0, Do_Flash,
		"flash [writedoubleword] [start address] [doubleworddata1] [doubleworddata2] [doubleworddata3] [doubleworddata4] \n\r",
	 	"erase flash or program data into specified address \n\r"
	},

	{
		"qspimemmap",   1, 1, 0, Do_Qspimemmap,
		"Type qspimemmap directly \n\r",
	 	"Configure QSPI as memory-mapped mode \n\r"
	},

	{
		"qspirstmem",   1, 1, 0, Do_QSPI_ResetMemory,
		"Type qspirstmem directly \n\r",
	 	"Reset QSPI memory interface \n\r"
	},

	{
		"spiflash",   7, 3, 0, Do_SpiFlash,
		"spiflash [erase | writebyte | writehalfword | writeword | readbyte | readhalfword | readword] [start address] [data_b1, data_b2, data_b3, data_b4] \n\r",
	 	"erase spiflash or program data into specified address \n\r"
	},
		
	{
		"spiflashrwd",   1, 1, 0, Do_Spi_FLASH_RW_Direct,
		"Type spiflashrwd directly \n\r",
		"spi flash R/W direct test \n\r"
	},

	{
		"spiflashrwind",   1, 1, 0, Do_Spi_FLASH_RW_Indirect,
		"Type spiflashrwind directly \n\r",
		"spi flash R/W indirect test \n\r"
	},

	{
	 	"sdrnewtest",  1, 1, 0, Do_New_SDRAM_Test,
	 	"Type sdrnewtest directly \n\r",
	 	"sdram 8/16/32 bits R/W test \n\r"
	},	

	{
	 	"sdrtest",  1, 1, 0, Do_SDRAM_Test,
	 	"Type sdrtest directly \n\r",
	 	"sdram 8/16 bits R/W test \n\r"
	},

	{
		"sdrclr",  1, 1, 0, Do_SDRAM_Clr,
		"Type sdrclr directly \n\r",
		"clear sdram data to all zero \n\r"
	},		

	{
		"mem",   5, 3, 0, Do_Mem,
		"mem [d|f|w|r|t] <l|w|b> [start address][length] \n\r",
	 	"dump/fill the content of sdram, read/write/test specified address \n\r"
	},
		
	{
		"upgrade",  1, 1, 0, Do_Upgrade,
		"Type upgrade directly \n\r",
		"upgrade FW with dedicate file received from serial port via Xmodem \n\r"
	},	

	{
		"bootime",  3, 2, 0, Do_Bootime,
		"bootime [r|ws] [jump time] \n\r",
		"adjust automatic jump time from loader to application \n\r"
	},			

	{
		"spiflashud",  1, 1, 0, Do_SpiFlash_Update,          
		"Type spiflashud directly \n\r",
		"copy MCU embedded flash data to external spi flash \n\r"
	},		

	{	/*end of table*/
		"",		0,	0, 0,	NULL,	"",	""
	}
};



/*help command*/
INT32S Do_Help (INT32S argc, INT8S *argv[])
{
	INT32S listall=0,count=0;
	cmd_tbl_t *entry=&CmdTable[0];
	
	if (argc==1)
		listall=1;
	while(entry->func!=NULL)
	{
		if (listall || stricmp(entry->name,argv[1])==0)
		{
			if (listall)
			{	
				printf("\n\r %-10s",entry->name);
				if (++count>5)
				{	
					printf("\n\r");
					count = 0;
				}
			}
			else
			{		
				printf("\n\r %s: %s",entry->name, entry->help);
				printf("\n\r usage: %s",entry->usage);
				break;
			}	
		}		
		entry++;
	}	
	printf("\n\r");
	return 0;
}	/*Do_Help*/





