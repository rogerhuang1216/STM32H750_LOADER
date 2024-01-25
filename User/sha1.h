#ifndef __SHA1__
#define __SHA1__

typedef unsigned long CRC_CONTEXT;

VOID crc_starts(CRC_CONTEXT* ctx);
VOID crc_update(CRC_CONTEXT *ctx, INT8U *input, INT32S ilen);
VOID crc_finish(CRC_CONTEXT *ctx, unsigned long *output);

typedef struct _SHA1_CONTEXT {
	unsigned long total[2];		/*!< number of bytes processed  */
	unsigned long state[5];		/*!< intermediate digest state  */
	INT8U buffer[64];	/*!< data block being processed */
	//INT8U ipad[64];		/*!< HMAC: inner padding        */
	//INT8U opad[64];		/*!< HMAC: outer padding        */
} SHA1_CONTEXT;

VOID sha1_starts(SHA1_CONTEXT* ctx);
VOID sha1_update(SHA1_CONTEXT *ctx, INT8U *input, INT32S ilen);
VOID sha1_finish(SHA1_CONTEXT *ctx, INT8U *output);

#endif

