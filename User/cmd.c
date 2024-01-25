/*
 * P1620 loader, command process function 
 * create by kenny 12-08-2008
 * modified by roger  13/07/2011
 */
 
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include "main.h"
#include "cmd.h"
#include "./usart/bsp_usart.h"
#include "string.h"
#include "loader.h"



// #include "Stm32f4xx_usart.h"         //roger 20230617 mark
// #include "loader.h"


#define FLOATING_POINT

#ifdef FLOATING_POINT
#include <locale.h>
#include <math.h>
#define TEMPSIZE 64
#else
#define TEMPSIZE 24
#endif
#define IS_DIGIT(c) ((INT16U)((c) - '0') <= 9)
#define DIGIT_VAL(c) ((c) - '0')
const INT8S digit[16] = "0123456789ABCDEF";



//leo +++
#ifndef MAXEXP/* 11-bit exponent (VAX G floating point) is 308 decimal digits */
#define	MAXEXP		308
#endif /*MAXEXP*/
#ifndef MAXFRACT/* 128 bit fraction takes up 39 decimal digits; max reasonable precision */
#define	MAXFRACT	39
#endif /*MAXFRACT*/
#define	DEFPREC		6
#define	ALT		0x001		/* alternate form */

/*
 * global variable, for input message buffer
 */
INT8U cmdbuf[4096]={0};
volatile INT32S put=0,get=0,full=0, last=0;
/*record last command*/
INT8U lastcmd[MAX_LAST_CMD][CMD_LEN];

//extern VOID OutC( register INT8U c);



extern cmd_tbl_t CmdTable[];
//extern VOID Enable(unsigned stat);
extern void Enable(void);
//extern VOID Disable(unsigned *stat);
extern VOID Disable(void);
extern INT32S stricmp( const INT8S* s1 , const INT8S* s2 );
//INT32S __cvt_double(FP64 number, register INT32S prec, INT32S flags, INT32S *signp, INT32S fmtch, INT8S *startp, INT8S *endp);

static VOID move_left( INT32S len );
static VOID move_right( INT8U* line , INT32S cursor , INT32S len );



#if 1

INT32S getch()
{
	INT32S c=-1;

	/*not wait data from command buffer*/
	//Disable(&cpusr);
	Disable();
	if (full || put != get)
	{
		full=0;
		c = (INT32S)cmdbuf[get++];
		if (get >=4096)
			get = 0;
	}
	//Enable(cpusr);	
	Enable();
	return c;
}

#endif





INT8U kbhit()
{
	INT8U flag=0;
	
	////Disable(&cpusr);
	Disable();
	if (full || put != get)
		flag = 1;
	////Enable(cpusr);	
	Enable();
	return flag;	
}


static VOID move_left( INT32S len )
{
    while ( len-- )
    {
        putc( '\b' );
    }
} 

static VOID move_right( INT8U* line , INT32S cursor , INT32S len )
{
    while ( len-- )
    {
        putc( line[cursor++] );
    }
} 

static VOID delete_move( INT8U* line ,
                         INT32S cursor ,
                         INT32S free ,
                         INT32S count )
{
    INT32S i;

    /* Setup and print modifications */
    for ( i = cursor; i < free; i++ )
    {
        line[i] = ( i + count < free ) ? line[i + count] : ' ';
        putc( line[i] );
    }

    /* Move cursor back */
    move_left( free - cursor );
}

static VOID insert_move( INT8U * line ,
                         INT32S  cursor ,
                         INT32S  free ,
                         INT8U ch )
{
    INT32S i;
    INT8U old;

    for ( i = cursor; i <= free; i++ )
    {
        old = line[i];
        line[i] = ch;
        putc( ch );
        ch = old;
    }

    /* Move cursor back */
    move_left( free - cursor );
}

/*
 *=============================================================================
 *  Function name: SYS_ReplaceLine
 *
 *  Parameter:
 *
 *  Return value:
 *      None
 *
 *  History:
 *       Create first version by kenny ver. 0.1                    04/11/2007
 *       modified by roger							   05/15/2011
 *=============================================================================
 */
static VOID replace_line(INT8U * line , INT32S *cursor , INT32S * free )
{
    INT32S i;

    move_left( *cursor );

    *cursor = strlen( (INT8S*)line );

    for ( i = 0; i < MAX( *cursor , *free ); i++ )
    {
        putc( ( i < *cursor ) ? line[i] : ' ' );
    }

    /* Move cursor back */
    if ( *free > *cursor )
    {
        move_left( *free - *cursor );
    }

    *free = *cursor;
}	/*end of replace line*/

INT32S readline(INT8U line[])
{
	INT8U ch, *cmd;
    static INT32S cursor , free , c;
	static INT32S esc_state = 0;
	static INT32S line_state=0;
repeat:	
	switch(line_state)
	{	  
		case 0:	/*initial state*/
			memset(line,0,CMD_LEN);
    		cursor = free = 0;
    		line_state = 1;
		case 1:	/*get character state*/
			
			//printf(" Enter readline switch line_state case 1\n\r");
			
			c = getch();
			if (c < 0)	/*no data*/
				break;
			ch = (INT8S)c;
			switch (esc_state)
			{
				case 1:	
					if (ch == 0x5b) /*'['*/
						esc_state = 2;
					else
						esc_state = 0;
					break;	/*wait next char, exit*/
				case 2:				
					switch(ch)
					{
						case 'A':
							ch = CTRL_P;
							break;
						case 'B':
							ch = CTRL_N;
							break;
						case 'C':
							ch = CTRL_F;
							break;
						case 'D':
							ch = CTRL_B;
							break;
					}	/*end switch*/
					esc_state = 0;
					break;
			}	/*end of esc_state*/
			
			switch (ch)
			{
				/* Control characters */
 				case CTRL_A:
 					move_left(cursor);
 					cursor=0;
 					break;
	 			case CTRL_E:
 					move_right( line , cursor , free - cursor );
				    cursor = free;
 					break;
	 			case CTRL_B:
 					if ( cursor > 0 )
					{
					move_left( 1 );
					cursor--;
					}
 					break;
 				case CTRL_D:
					if ( cursor < free )
					{
					delete_move( line , cursor , free , 1 );
					free--;
					}
        	        		break;
 				case CTRL_F:
					if ( cursor < free )
					{
					move_right( line , cursor , 1 );
					cursor++;
					}
		                	break;
	 			case CTRL_K:
					delete_move( line , cursor , free , free - cursor );
					free = cursor;
					break;
	 			case CTRL_N:	/*next command*/
 					cmd = lastcmd[(++last%MAX_LAST_CMD)];
 					memcpy(line, cmd, CMD_LEN);
 					replace_line(line, &cursor, &free);
 					break;
	 			case CTRL_P:	/*prev command*/
 					cmd = lastcmd[(last%MAX_LAST_CMD)];
 					memcpy(line, cmd, CMD_LEN);
 					replace_line(line, &cursor, &free);
 					last--;
 					break;
	 			case CTRL_C:
 				case CTRL_U:
					move_left( cursor );
					cursor = 0;
					delete_move( line , cursor , free , free );
					free = 0;
            	    			break;
 				case CTRL_H:
 					/*same as del*/
	 			case DEL:
					if ( cursor > 0 )
					{
					move_left( 1 );
					cursor--;
					delete_move( line , cursor , free , 1 );
					free--;
					}
            	    			break;
 				case TAB:
 				case CR:
	 			case LF:
					/*copy to last command buffer*/
					cmd = lastcmd[(++last%MAX_LAST_CMD)];
					memcpy(cmd, line, CMD_LEN);
					line_state = 0;
					break;
	 			case ESC:
 					esc_state = 1;
 					break;
 				case SP:
 				case CSI:
	 			default:
					if ( ch )
					{
						if ( free < CMD_LEN )
						{
						insert_move( line , cursor , free , ch );
						cursor++;
						free++;
						}
					}
    	            			break;
			}	/*end switch*/
			break;
	}	/*end of line state switch*/
	
	if (line_state == 0)
	{
		if (( ch == CR && free == 0 ) || ( ch == CTRL_C ) )
	    	{
	        	line[free] = '\0';
	        	/*copy to last command buffer*/
	        	if (line[0]!='\0')
	        	{
			cmd = lastcmd[(++last%MAX_LAST_CMD)];
			memcpy(cmd, line, CMD_LEN);
	        	}
	        	if ( ch == CTRL_C)
	            	return CTRL_C;
	        	else
	            	return 1;
	    	}
    		if (ch == CR || ch == LF)
			printf("\n");
		return 1;
      }

	if(put > get)
		goto repeat;
	
	return 0;
}	/*end of read line*/


VOID parse_token(INT8U *buf, INT32S *num, INT8S **arg)
{
	INT32S i=0, state=0;
	
	 *num = 0;
	/*scan all  line*/ 
	for(i=0;buf[i]!='\0';i++)
	{
		switch(state)
		{
			case 0:
				if (isspace(buf[i])||buf[i]=='\r' || buf[i]=='\n')
				{
				    buf[i]='\0';
					continue;
				}
	 			arg[*num]= (INT8S *)&buf[i];
	 			*num+=1;
				if (*num>=MAX_TOKEN)
					return;
				if (buf[i]=='\"' || buf[i]=='\'')
					state = 2;
				else
	 				state = 1;
	 		break;
	 		case 1:
	 			if (isspace(buf[i]))
	 			{
	 				buf[i]='\0';
	 				state = 0;
	 			}	
	 		break;
	 		case 2:
				if (buf[i]=='\"' || buf[i]=='\'')
					state = 1;				
	 		break;
		}	/*end of switch*/
	}	/*end for loop */
}

/*
 *===========================================================================
 *  Public Function: SYS_AtoH
 *      Translation form ASCII string to Hex if leading digit is 0x or 0X
 *      Otherwise, it will translate to a integer
 *  Parameter:
 *      Character string pointer
 *  Return value:
 *      return value (integer)
 *
 *  History:
 *       Create first version by kenny ver. 0.1                    04/12/2007
 *       modified by roger huang                    			   10/15/2011
 *===========================================================================
 */
INT32U atoh( const INT8S* cp )
{
    INT32U i = 0 , j;

    if ( *cp == '0' && ( *( cp + 1 ) == 'X' || *( cp + 1 ) == 'x' ) )
    {
        cp += 2;
        while ( *cp != '\0' )
        {
            if ( *cp >= 'A' && *cp <= 'F' )
            {
                j = *cp - 'A' + 10;
            }
            else if ( *cp >= 'a' && *cp <= 'f' )
            {
                j = *cp - 'a' + 10;
            }
            else if ( *cp >= '0' && *cp <= '9' )
            {
                j = *cp - '0';
            }
            else
            {
                return 0;
            }
            i = i * 16 + j;
            cp++;
        }
        return i;
    }
    else
    {
		return ( INT32U ) atol( cp );
    }
} 



INT64U atolh( const INT8S* cp )
{
    INT64U i = 0 , j;

    if ( *cp == '0' && ( *( cp + 1 ) == 'X' || *( cp + 1 ) == 'x' ) )
    {
        cp += 2;
        while ( *cp != '\0' )
        {
            if ( *cp >= 'A' && *cp <= 'F' )
            {
                j = *cp - 'A' + 10;
            }
            else if ( *cp >= 'a' && *cp <= 'f' )
            {
                j = *cp - 'a' + 10;
            }
            else if ( *cp >= '0' && *cp <= '9' )
            {
                j = *cp - '0';
            }
            else
            {
                return 0;
            }
            i = i * 16 + j;
            cp++;
        }
        return i;
    }
    else
    {
		return ( INT64U ) atoll( cp );
    }
} 





/*put comamnd string into cmdbuf*/
/*for tftp*/
VOID put_cmd(INT8S *str)
{
	INT8S *cp= str;
	
	////Disable(&cpusr);
	Disable();
	while(*cp)
	{
  		if(full)
		{
			cp++;
			continue;
		}
		cmdbuf[put++] = *cp++;
  		if (put>=4096)
  			put=0;
  		if (put==get)
  			full=1;	
  	}	/*end of while*/
	////Enable(cpusr);	
	Enable();
}	/*end of put command*/

/*
*============================================================================
*   Public Function: SYS_DoFormat
*       Format the input message string into a ASCII string
*
*   Interface parameter:
*       Input:  point to output buffer
*               message string
*               val_list
*       Return value: none
*
*   History:
*       Create first version by roger huang ver. 0.1                     06/11/2010
*============================================================================
*/
static VOID SYS_DoFormat( register const INT8S* f ,
                          va_list ap )
{
    register INT8S c;
    INT8S temp[TEMPSIZE];
    INT8S *t;
    INT16S flushleft;
    INT16S sharp;
    INT16S width;
    INT16S prec;
    INT16S islong;
    INT8S* p;
    INT16S sign;
    INT32S l;
    INT32U num;
    INT16U base;
    INT16S len;
    INT16S size;
    //INT32S size;	
    INT16S pad;
    INT8S escap_ch = 0;
    INT8S dp;


	
#ifdef  FLOATING_POINT
    INT8S * decimal_point = localeconv()->decimal_point;
    INT8S softsign;     /* temporary negative sign for floats */
    union
    {
        INT32S     i; FP64                   d;
    } _double_ =
    {
        0
    };
#define _fpvalue (_double_.d)

//#define isinf(a)    (a==_FPE_ZERODIVIDE?1:0)

#endif  




//leo +++
	register INT32S flags;	/* flags as above */
	FP64 _double;
	//INT32S expt;		/* integer value of exponent */
	//register INT8S *cp;	/* handy char pointer (short term usage) */
	//register INT32S ch;	/* character from fmt */
//leo ---

    while ( ( c = *f++ ) != '\0' )
    {
        if ( c != '%' )
        {
            if ( c == '\n' )
            {
                // OutC( '\r');
		   		USART_SendData(UARTx, '\r');	
            }
            if ( c == '\\' )
            {
                escap_ch = 1;
                continue;
            }
            // OutC( c );
	        USART_SendData(UARTx, c);
            continue;
        }
        if ( escap_ch )
        {
            // OutC( c );
	        USART_SendData(UARTx, c);	
			
            escap_ch = 0;
            continue;
        }

        flushleft = 0;
        sharp = 0;
        width = 0;
        prec = -1;
        islong = 0;
        dp = 0;
        for ( ; ; )
        {
            if ( *f == '-' )
            {
                flushleft++;
            }
            else if ( *f == '#' )
            {
                sharp++;
            }
            else
            {
                break;
            }
            f++;
        }

        if ( *f == '*' )
        {
            width = va_arg( ap , INT32S );
            f++;
        }
        else
        {
            while ( IS_DIGIT( *f ) )
            {
                width = 10 * width + DIGIT_VAL( *f++ );
            }
        }

        if ( *f == '.' )
        {
            if ( *++f == '*' )
            {
                prec = va_arg( ap , INT32S );
                f++;
            }
            else
            {
                prec = 0;
                while ( IS_DIGIT( *f ) )
                {
                    prec = 10 * prec + DIGIT_VAL( *f++ );
                }
            }
        }

        if ( *f == 'l' )
        {
            islong++;
            f++;
            if ( *f == 'l' )  /*long long for 64bits variable*/
            {
                islong++;
                f++;
            }
        }

		
		flags = 0;
		prec = -1;
		//ch = *f;
        switch ( *f )
        {
          case 'd':
            if ( islong )
            {
                l = va_arg( ap , long );
            }
            else
            {
                l = va_arg( ap , INT32S );
            }
            sign = 0;
            num = l;
            if ( l < 0 )
            {
                num = -l;
                sign = 1;
            }
            base = 10;
            goto number;
          case 'u':
            base = 10;
            goto uns_number;
          case 'o':
            base = 8;
            goto uns_number;
#ifdef FLOATING_POINT
//          case 'e':
//          case 'E':
          case 'f':
          case 'F':
//          case 'g':
//          case 'G':
            //_fpvalue = ( FP64 ) va_arg( ap , FP64 );
            /*if (isinf (_fpvalue)) 
                    {
                        if (_fpvalue < 0)
                            OutC('-',dest);
                        OutC('I',dest);
                        OutC('N',dest);
                        OutC('F',dest);
                        break;
                    }
                    if (isnan (_fpvalue)) 
                    {
                        OutC('N',dest);
                        OutC('A',dest);
                        OutC('N',dest);
                        break;
                    }*/
			_double = va_arg(ap, FP64);			
			/*			 * don't do unrealistic precision; just pad it with			 
			* zeroes later, so buffer size stays rational.			 
			*/
			sign=0;	
			prec = 3;
            		*temp= '\0';
			size = __cvt_double(_double, prec, flags, &softsign, *f,temp ,temp+sizeof(temp));

			if (softsign)				
				sign = '-';
			if(sign)
				// OutC( sign );
			    USART_SendData(UARTx, sign);	
			
			t = *temp ? temp : temp + 1;
			while (--size >= 0)				
			{					
				// OutC( *t++ );
				USART_SendData(UARTx, *t++);
			}	
            break;
#endif                  
          case 'p':
            dp = 1; /*display pointer*/
            islong = 1;
            if ( width < 9 )
            {
                width = 9;
            }
          case 'x':
            /* we don't implement 'x'; treat like 'X' */
          case 'X':
            base = 16;
            uns_number:     /* an unsigned number */
            sign = 0;
            if ( islong )
            {
                num = va_arg( ap , INT32U);
            }
            else
            {
                num = va_arg( ap , INT16U );
            }
            number:         /* process a number */
            p = temp + TEMPSIZE - 1;
            *p = '\0';
            while ( num )
            {
                *--p = ( INT8S ) digit[( INT8U ) ( num % base )];
                num /= base;
                if ( dp )
                {
                    if ( dp == 4 )
                    {
                        *--p = ':';
                    }
                    dp++;
                }
            }
            while ( dp > 0 && dp <= 5 )
            {
                if ( dp == 5 )
                {
                    *--p = ':';
                }
                else
                {
                    *--p = '0';
                }
                dp++;
            }
            len = ( INT16S )
                  ( ( ( INT32U) temp + TEMPSIZE - 1 ) - ( INT32U) p );
            if ( prec < 0 )
            {
                prec = 1;
            }
            if ( sharp && *f == 'o' && prec <= len )
            {
                prec = len + 1;
            }
            pad = 0;
            if ( width )
            {
                size = len;
                if ( size < prec )
                {
                    size = prec;
                }
                size += sign;
                pad = width - size;
                if ( flushleft == 0 )
                {
                    while ( --pad >= 0 )
                    {
                        if ( base == 16 || base == 8 )
                        {
                            // OutC( '0' );
				  			USART_SendData(UARTx, '0');			
                        }
                        else
                        {
                            // OutC( ' ' );
                            USART_SendData(UARTx,  ' ' );	
                        }
                    }
                }
            }
            if ( sign )
            {
                // OutC( '-' );
		        USART_SendData(UARTx, '-');	
            }
            prec -= len;
            while ( --prec >= 0 )
            {
                // OutC( '0' );
		        USART_SendData(UARTx, '0');					
            }
            while ( *p )
            {
                // OutC( *p++ );
		        USART_SendData(UARTx, *p++);	
            }
            while ( --pad >= 0 )
            {
                if ( base == 16 || base == 8 )
                {
                    // OutC( '0' );
			        USART_SendData(UARTx,  '0' );	
                }
                else
                {
					// OutC( ' ' );
					USART_SendData(UARTx,  ' ' );	
                }
            }
            break;
          case 's':
            p = va_arg( ap , INT8S * );
            pad = 0;
            if ( width )
            {
                {
                    len = strlen( p );
                }
                if ( prec >= 0 && len > prec )
                {
                    len = prec;
                }
                pad = width - len;
                if ( flushleft == 0 )
                {
                    while ( --pad >= 0 )
                    {
						// OutC( ' ' );
						USART_SendData(UARTx,  ' ' );	
                    }
                }
            }
            prec++;
			
            {
                while ( --prec != 0 && *p )
                {
 				  // OutC( *p++ );
 				  USART_SendData(UARTx,  *p++ );	
                }
            }
            while ( --pad >= 0 )
            {
				// OutC( ' ' );
				USART_SendData(UARTx,  ' ' );	
            }
            break;

          case 'c':
            c = va_arg( ap , INT32S );
			// OutC( c );
			USART_SendData(UARTx,  c );	 	
            break;
          default:
            // OutC( *f );
	        USART_SendData(UARTx,  *f );		       
            break;
        }
        f++;
    }
}   /*end of SYS_DoFormat*/


#define	to_digit(c)	((c) - '0')
#define	to_char(n)	((n) + '0')

/*
 *===========================================================================
 *  Function name: round_lll
 *      
 *
 *  Parameter:
 *      
 *
 *  Return value:
 *      size
 *
 *  History:
 *      Kenny chen      04-10-2007          Create first version v0.1
 *===========================================================================
 */
static INT8S * round_lll(FP64 fract, INT32S *exp,
		    register INT8S *start, register INT8S *end,
		    INT8S ch, INT32S *signp)
{
	FP64 tmp;

	if (fract)
	(VOID)modf(fract * 10, &tmp);
	else
		tmp = to_digit(ch);
	if (tmp > 4)
		for (;; --end) {
			if (*end == '.')
				--end;
			if (++*end <= '9')
				break;
			*end = '0';
			if (end == start) {
				if (exp) {	/* e/E; increment exponent */
					*end = '1';
					++*exp;
				}
				else {		/* f; add extra digit */
				*--end = '1';
				--start;
				}
				break;
			}
		}
	/* ``"%.3f", (FP64)-0.0004'' gives you a negative 0. */
	else if (*signp == '-')
		for (;; --end) {
			if (*end == '.')
				--end;
			if (*end != '0')
				break;
			if (end == start)
				*signp = 0;
		}
	return (start);
}

/*
 *===========================================================================
 *  Function name: __cvt_double
 *      The output message function. It will convert the double into ASCII
 *
 *  Parameter:
 *     
 *
 *  Return value:
 *      size
 *
 *  History:
 *      Kenny chen      04-10-2007          Create first version v0.1
 *===========================================================================
 */
INT32S __cvt_double(FP64 number, register INT32S prec, INT32S flags, INT32S *signp,
		 INT32S fmtch, INT8S *startp, INT8S *endp)
{
	register INT8S *p, *t;
	register FP64 fract;
	INT32S expcnt;
	FP64 integer, tmp;

	expcnt = 0;
	if (number < 0) {
		number = -number;
		*signp = '-';
	} else
		*signp = 0;

	fract = modf(number, &integer);

	/* get an extra slot for rounding. */
	t = ++startp;

	/*
	 * get integer portion of number; put into the end of the buffer; the
	 * .01 is added for modf(356.0 / 10, &integer) returning .59999999...
	 */
	for (p = endp - 1; p >= startp && integer; ++expcnt) {
		tmp = modf(integer / 10, &integer);
		*p-- = to_char((INT32S)((tmp + .01) * 10));
	}
	switch (fmtch) {
	case 'f':
	case 'F':
		/* reverse integer into beginning of buffer */
		if (expcnt)
			for (; ++p < endp; *t++ = *p);
		else
			*t++ = '0';
		/*
		 * if precision required or alternate flag set, add in a
		 * decimal point.
		 */
		if (prec || flags&ALT)
			*t++ = '.';
		/* if requires more precision and some fraction left */
		if (fract) {
			if (prec)
				do {
					fract = modf(fract * 10, &tmp);
					*t++ = to_char((INT32S)tmp);
				} while (--prec && fract);
			if (fract)
				startp = round_lll(fract, (INT32S *)NULL, startp,
				    t - 1, (INT8S)0, signp);
		}
		for (; prec--; *t++ = '0');
		break;

	}
	return (t - startp);
}
/*printf function*/
INT32S SYS_Printf( const INT8S* fmt , ... )
{
    va_list ap;

    va_start( ap , fmt );
    SYS_DoFormat( fmt , ap );
    va_end( ap );
    return 1;
}   /*end of SSYS_Printf*/

/*search command table, get comamnd entry*/
cmd_tbl_t *find_cmd (const INT8S *cmd)
{
	cmd_tbl_t *entry=&CmdTable[0];
	
	while(entry->func!=NULL)
	{
		if (stricmp(cmd, entry->name)==0)
			return entry;
		entry++;
	}
	return 0;
}

VOID print_frame( INT8U * frame , INT32S len )
{
    INT32S  i;
    INT8U * p = frame , ch[16 + 1] = {0};

    if ( len <= 0 )
    {
        printf( "size overrun %u\n" , len );
        len &= 0x7fff;
    }
    while ( len > 0 )
    {
        printf("%04lx: " , ( p - frame ) );
        printf("%02X %02X %02X %02X %02X %02X %02X %02X-%02X %02X %02X %02X %02X %02X %02X %02X | " ,
                p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13],p[14],p[15] );
        for ( i = 0; i < 16; i++ )
        {
            ch[i] = ( p[i] < 0x20 ) ? '.' : p[i];
	        printf( "%c" , ch[i] );
        }
        printf("\n");

        p += 16;
        len -= 16;
    }/* End of for */
}    /* print_frame */
