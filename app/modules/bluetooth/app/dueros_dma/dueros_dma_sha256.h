#ifndef _DUEROS_DMA_SHA256_H
#define _DUEROS_DMA_SHA256_H

/*************************** HEADER FILES ***************************/
#include "typedef.h"

/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

typedef struct {
	BYTE data[64];
	DWORD datalen;
	unsigned long long bitlen;
	DWORD state[8];
} sha256_ctx_t;


/*********************** FUNCTION DECLARATIONS **********************/
void sha256_init(sha256_ctx_t *ctx);
void sha256_update(sha256_ctx_t *ctx, const BYTE data[], size_t len);
void sha256_final(sha256_ctx_t *ctx, BYTE hash[]);

#endif   // _DUEROS_DMA_SHA256_H
