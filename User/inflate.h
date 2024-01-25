/************************************************************************/
/* Module Name: inflate.h                                               			*/
/* Description: The include file of decompression driver                	*/
/*                                                                      				*/
/* History:     roger huang: Modified                   12/10/2008         	*/
/************************************************************************/

#ifndef _INFLATE_H_
#define _INFLATE_H_
#include "sha1.h"

/* gzip flag byte */
#define ASCII_FLAG   0x01   /* bit 0 set: file probably ASCII text */
#define CONTINUATION 0x02   /* bit 1 set: continuation of multi-part gzip file*/
#define EXTRA_FIELD  0x04   /* bit 2 set: extra field present */
#define ORIG_NAME    0x08   /* bit 3 set: original file name present */
#define COMMENT      0x10   /* bit 4 set: file comment present */
#define ENCRYPTED    0x20   /* bit 5 set: file is encrypted */
#define RESERVED     0xC0   /* bit 6,7:   reserved */

#define memzero(s, n)   memset ((s), 0, (n))
#define WSIZE   0x8000  /* Window size must be at least 32k, */
                        /* and a power of two */
#define CRC_VALUE (crc ^ 0xffffffffL)

VOID variable_init(INT8U *dest, INT8U *src, INT32S size);
INT32S gunzip(VOID);


extern VOID makecrc(VOID);
#endif

/* End of inflate.h */
