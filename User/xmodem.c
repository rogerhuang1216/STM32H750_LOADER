/*
	Copyright 2001, 2002 Georges Menie (www.menie.org)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* this code needs standard functions memcpy() and memset()
   and input/output functions _inbyte() and _outbyte().

   the prototypes of the input/output functions are:
     INT32S _inbyte(INT16U timeout); // msec timeout
     VOID _outbyte(INT32S c);

 */
 
#include "main.h"
#include "cmd.h"
//#include "stm32h7xx_hal.h"
#include "./delay/core_delay.h"


#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

//#define DLY_1S 100     // 10ms/tick ? 
#define DLY_1S 1000   //  1ms/tick ?      roger 20150505 add


#define MAXRETRANS 25

extern ulong GetTick(VOID);
//extern void Delay(__IO uint32_t nTime);



#if 1

static INT32S _inbyte(INT32S tick) 
{ 
    ulong timeout = tick + GetTick();         // roger 20150514 mark      
    //ulong timeout = tick+(10*GetTick());	    // roger 20150514 add
    INT32S c;

	//printf("\n\r [_inbyte] timeout 0x%lx OK \r\n",timeout);
	
	//printf("\n\r [_inbyte] HAL_GetTick 0x%lx OK \r\n",HAL_GetTick());

    while(timeout > GetTick())			  // roger 20150514 mark     
    //while(timeout > (GetTick()/2))		  // roger 20150514 add
    {
    	if (kbhit())
    	{
			c = getch(); 
			if (c<0)
				continue;
			
			/// HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);      // roger 20230728 add, observe _inbyte Interval
    		// return c;             // roger 20230728 marked
			return c & 0x0FF;        // roger 20230728 add
    	}
	}
	return -1;
} 

 
static VOID _outbyte(INT8U c) 
{ 
	putc(c);
} 


#endif




#if 0

static INT32S _inbyte(INT32S tick) 
{ 
	
    ulong timeout = tick + HAL_GetTick();      // roger 20230725 modify to use HAL_GetTick  
    //ulong timeout = tick + (10*HAL_GetTick());       // roger 20230725 modify to use HAL_GetTick  

    INT32S c;

	//printf("\n\r [_inbyte] timeout 0x%lx OK \r\n",timeout);
	
	//printf("\n\r [_inbyte] HAL_GetTick 0x%lx OK \r\n",HAL_GetTick());

    while(timeout > HAL_GetTick())			  // roger 20230725 modify to use HAL_GetTick  
    //while(timeout > (GetTick()/2))		  // roger 20150514 add
    {
    	if (kbhit())
    	{
			c = getch(); 
			if (c<0)
				continue;
    		return c;
    	}
	}
	return -1;
} 

 
static VOID _outbyte(INT8U c) 
{ 
	putc(c);
} 


#endif



static INT16U crc16_ccitt(const INT8U *buf, INT32S sz) 
{ 
	INT16U crc = 0; 
    while (--sz >= 0) 
    { 
    	INT32S i; 
        crc ^= (INT16U) *buf++ << 8; 
        for (i = 0; i < 8; i++) 
        	if (crc & 0x8000) 
            	crc = crc << 1 ^ 0x1021; 
            else 
                crc <<= 1; 
    } 
    return crc; 
} 	/*end of crc16 */


static INT32S check(INT32S crc, const INT8U *buf, INT32S sz)
{
	if (crc) {
		INT16U crc = crc16_ccitt(buf, sz);
		INT16U tcrc = (buf[sz]<<8)+buf[sz+1];
		if (crc == tcrc)
			return 1;
	}
	else {
		INT32S i;
		INT8U cks = 0;
		for (i = 0; i < sz; ++i) {
			cks += buf[i];
		}
		if (cks == buf[sz])
		return 1;
	}

	return 0;
}


static VOID flushinput(VOID)
{
	while (_inbyte(((DLY_1S)*3)>>1) >= 0)
		;
}


INT32S xmodemReceive(INT8U *dest, INT32S destsz)
{
	INT8U xbuff[1030]={0}; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
	INT8U *p;
	INT32S bufsz, crc = 0;
	INT8U trychar = 'C';	
	INT8U packetno = 1;
	INT32S i, c, len = 0;
	INT32S retry, retrans = MAXRETRANS;


//    printf("\r\n Enter xmodemReceive function \r\n");


	for(;;) 
	{
		//for( retry = 0; retry < 2; ++retry)    // the original version is retry <16
		for( retry = 0; retry < 16; ++retry)    // the original version is retry <16
		{
//		    printf("\r\n Enter xmodemReceive retry %d times \r\n", retry);
			
			if (trychar) 
				_outbyte(trychar);

			if ((c = _inbyte((DLY_1S)<<1)) >= 0) 
			{
				switch (c) 
				{
				case SOH:
					bufsz = 128;
					goto start_recv;
				case STX:
					bufsz = 1024;
					goto start_recv;
				case EOT:
					flushinput();
					_outbyte(ACK);
					return len; /* normal end */
				case CAN:
					if ((c = _inbyte(DLY_1S)) == CAN) 
					{
						flushinput();
						_outbyte(ACK);
						return -1; /* canceled by remote */
					}
					break;
				default:
					break;
				}
			}

		}

		
		if (trychar == 'C') 
		{ 
			trychar = NAK; 
			continue; 
		}
		
		flushinput();
		_outbyte(CAN);
		_outbyte(CAN);
		_outbyte(CAN);
		return -2; /* sync error */

	start_recv:

	  //printf("\r\n Enter xmodemReceive Function [start_recv] \r\n");

      //HAL_Delay(10);    // delay 10ms
      //Delay(20);    // delay 20ms
		
		if (trychar == 'C') 
			crc = 1;
		trychar = 0;
		p = xbuff;
		*p++ = c;

		for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i) 
		{
			if ((c = _inbyte(DLY_1S)) < 0) 
				goto reject;
			*p++ = c;
		}
		
		if (xbuff[1] == (INT8U)(~xbuff[2]) && 
			(xbuff[1] == packetno || xbuff[1] == (INT8U)packetno-1) &&
			check(crc, &xbuff[3], bufsz)) 
		{
			if (xbuff[1] == packetno)	
			{
				register INT32S count = destsz - len;
				if (count > bufsz) 
					count = bufsz;
				if (count > 0) 
				{
					memcpy (&dest[len], &xbuff[3], count);
					len += count;
				}
				else
					printf("\n\r buffer full\n");
				++packetno;
				retrans = MAXRETRANS+1;
			}
			if (--retrans <= 0) 
			{
				flushinput();
				_outbyte(CAN);
				_outbyte(CAN);
				_outbyte(CAN);
				return -3; /* too many retry error */
			}
			_outbyte(ACK);
			continue;
		}
	reject:

//		printf("\r\n Enter xmodemReceive Function [reject] \r\n");

		flushinput();
		_outbyte(NAK);

		
	}
}




INT32S xmodemTransmit(INT8U *src, INT32S srcsz)
{
	INT8U xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
	INT32S bufsz, crc = -1;
	INT8U packetno = 1;
	INT32S i, c, len = 0;
	INT32S retry;

	for(;;) 
	{
		for( retry = 0; retry < 16; ++retry) 
		{
			if ((c = _inbyte((DLY_1S)<<1)) >= 0) 
			{
				switch (c) 
				{
				case 'C':
					crc = 1;
					goto start_trans;
				case NAK:
					crc = 0;
					goto start_trans;
				case CAN:
					if ((c = _inbyte(DLY_1S)) == CAN) 
					{
						_outbyte(ACK);
						flushinput();
						return -1; /* canceled by remote */
					}
					break;
				default:
					break;
				}
			}
		}
		_outbyte(CAN);
		_outbyte(CAN);
		_outbyte(CAN);
		flushinput();
		return -2; /* no sync */

		for(;;) 
		{
		start_trans:
			xbuff[0] = SOH; bufsz = 128;
			xbuff[1] = packetno;
			xbuff[2] = ~packetno;
			c = srcsz - len;
			if (c > bufsz) 
				c = bufsz;
			if (c >= 0) 
			{
				memset (&xbuff[3], 0, bufsz);
				if (c == 0) 
				{
					xbuff[3] = CTRLZ;
				}
				else 
				{
					memcpy (&xbuff[3], &src[len], c);
					if (c < bufsz) 
						xbuff[3+c] = CTRLZ;
				}
				if (crc) 
				{
					INT16U ccrc = crc16_ccitt(&xbuff[3], bufsz);
					xbuff[bufsz+3] = (ccrc>>8) & 0xFF;
					xbuff[bufsz+4] = ccrc & 0xFF;
				}
				else 
				{
					INT8U ccks = 0;
					for (i = 3; i < bufsz+3; ++i) 
					{
						ccks += xbuff[i];
					}
					xbuff[bufsz+3] = ccks;
				}
				for (retry = 0; retry < MAXRETRANS; ++retry) 
				{
					for (i = 0; i < bufsz+4+(crc?1:0); ++i) 
					{
						_outbyte(xbuff[i]);
					}
					if ((c = _inbyte(DLY_1S)) >= 0 ) 
					{
						switch (c) 
						{
						case ACK:
							++packetno;
							len += bufsz;
							goto start_trans;
						case CAN:
							if ((c = _inbyte(DLY_1S)) == CAN) 
							{
								_outbyte(ACK);
								flushinput();
								return -1; /* canceled by remote */
							}
							break;
						case NAK:
						default:
							break;
						}
					}
				}
				_outbyte(CAN);
				_outbyte(CAN);
				_outbyte(CAN);
				flushinput();
				return -4; /* xmit error */
			}
			else 
			{
				for (retry = 0; retry < 10; ++retry) 
				{
					_outbyte(EOT);
					if ((c = _inbyte((DLY_1S)<<1)) == ACK) 
						break;
				}
				flushinput();
				return (c == ACK)?len:-5;
			}
		}
	}
}




#if 0

/*do xmodem to xmt/rcv data via serial port*/
INT32S Do_Xmodem(INT32S argc, INT8S *argv[])
{
	INT32S max_size = LoadBufSize, st;
	INT8U *paddr = (INT8U *)LoadBufAddr;
	
	if (argc>=3)
		paddr = (INT8U *)atoh(argv[2]);
	if (argc>=4)
		max_size = atoh(argv[3]);
	
	if (strcmp(argv[1],"rcv")==0)	
	{
		printf ("Waiting XMODEM receive at 0x%lx maximum size %ld...\n"
				"Send data using the xmodem protocol from your terminal emulator now...",(INT32S)paddr,max_size);
		st = xmodemReceive(paddr, max_size);
		if (st < 0) 
			printf ("\nXmodem receive error: status: %d\n", st);
		else  
			printf ("\nXmodem successfully received at 0x%lx, %d bytes\n", (unsigned)paddr, st);
	}
	else if (strcmp(argv[1],"xmt")==0)	
	{
		printf ("Start XMODEM transmission at 0x%lx length %ld...\n"
			"Prepare your terminal emulator to receive data now...",(INT32S)paddr, max_size);
			/*lock UART, Is been lock by XMODEM?*/
		st = xmodemTransmit(paddr, max_size);
		if (st < 0) 
			printf ("\nXmodem transmit error: status: %d\n", st);
		else  
			printf ("\nXmodem successfully transmitted 0x%lx, %d bytes\n", (unsigned)paddr, st);
	}
	return 0;
}

#endif

