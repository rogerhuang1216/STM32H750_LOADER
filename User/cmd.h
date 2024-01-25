#ifndef _CMD_H_
#define _CMD_H_

/*
 * P1620 loader, command process include file
 * create   by kenny 12-08-2008
 * modified by roger huang 26-09-2011
 */
 
#include "main.h"
//#include "stm324x9i_eval.h"
#include "./usart/bsp_usart.h"




/* Control characters */
#define CTRL_A          ('A'-0x40)
#define CTRL_B          ('B'-0x40)
#define CTRL_C          ('C'-0x40)
#define CTRL_D          ('D'-0x40)
#define CTRL_E          ('E'-0x40)
#define CTRL_F          ('F'-0x40)
#define CTRL_H          ('H'-0x40)
#define CTRL_K          ('K'-0x40)
#define CTRL_N          ('N'-0x40)
#define CTRL_P          ('P'-0x40)
#define CTRL_U          ('U'-0x40)
#define DEL             		0x7F
#define TAB             		0x09
#define CR              		0x0D
#define LF              		0x0A
#define ESC             		0x1B
#define SP              		0x20
#define CSI             		0x9B
#define CMD_LEN			256

#define MAX_TOKEN		20
#define MAX_LAST_CMD	10 
/*
 * Monitor Command Table
 */

struct cmd_tbl_s 
{
	INT8S	*name;		/* Command Name	*/
	INT32S	maxargs;	/* maximum number of arguments */
	INT32S	minargs;	/* minimum number of arguments */
	INT32S	repeatable;	/* autorepeat allowed? */
			/* Implementation function	*/
	INT32S	(*func)(INT32S, INT8S *[]);
	INT8S	*usage;	/* Usage message (short) */
	INT8S	*help;	/* Help message	(long) */
};
 
typedef struct cmd_tbl_s	cmd_tbl_t;

extern INT32S getch(VOID);
//extern u16 getch(VOID);		// roger 20150415 add
extern INT32S readline(INT8U buf[]);
extern VOID parse_token(INT8U *buf, INT32S *num, INT8S **arg);

extern INT32U atoh( const INT8S* cp );
extern INT64U atolh( const INT8S* cp );


//extern VOID OutC( register INT8U c);
extern INT8U kbhit(VOID);
extern VOID put_cmd(INT8S *str);
//extern INT32S SYS_Printf( const INT8S* fmt , ... );
extern cmd_tbl_t *find_cmd (const INT8S *cmd);

extern cmd_tbl_t CmdTable[]; 

//#undef printf
//#define printf SYS_Printf

//#define putc(c) OutC(c)
#define putc(c) USART_SendData(UARTx, c);	 




#endif /* _CMD_H_ */

