/***************************************************************************
 *
 * Copyright 2017-2020 BAIDU.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BAIDU.
 *
 * Use of this work is governed by a license granted by BAIDU.
 * This work contains confidential and proprietary information of
 * BAIDU. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
/***
* queue  c language circle queue
*/
// #include "include.h"

#ifndef DMA_UTILS_H
#define DMA_UTILS_H

#if defined(__cplusplus)
extern "C" {
#endif



typedef unsigned char DMA_ITEM_TYPE;

#define DMA_OK    0
#define DMA_ERR   -1

#define AES_MAXNR 14

typedef struct {
    int32_t read;
    int32_t write;
    int32_t size;
    int32_t len;
    DMA_ITEM_TYPE* base;
} DMA_QUEUE;

struct aes_key_st {
    uint32_t rd_key[4 * (AES_MAXNR + 1)];
    int rounds;
};

typedef struct aes_key_st AES_KEY;

/* Init Queue */
int32_t dma_init_queue(DMA_QUEUE* queue, uint32_t size, DMA_ITEM_TYPE* buf);
/* Is Queue Empty */
int32_t dma_is_empty_queue(DMA_QUEUE* queue);
/* Filled Length Of Queue */
int32_t dma_length_of_queue(DMA_QUEUE* queue);
/* Empty Length Of Queue */
int32_t dma_available_of_queue(DMA_QUEUE* queue);
/* Push Data Into Queue (Tail) */
int32_t dma_enqueue(DMA_QUEUE* queue, DMA_ITEM_TYPE* e, uint32_t len);
/* Pop Data Data From Queue (Front) */
int32_t dma_dequeue(DMA_QUEUE* queue, DMA_ITEM_TYPE* e, uint32_t len);
/* Peek But Not Pop Data From Queue (Front) */
int32_t dma_peek_queque(DMA_QUEUE* queue, uint32_t len_want, DMA_ITEM_TYPE** e1, uint32_t* len1, DMA_ITEM_TYPE** e2,
                        uint32_t* len2);
/* Peek data to buf e, But Not Pop Data From Queue (Front) */
int32_t dma_peek_queue_2_buf(DMA_QUEUE* queue, DMA_ITEM_TYPE* e, uint32_t len);
/* Dump Queue */
int32_t dma_dump_queue(DMA_QUEUE* queue);

void dma_reset_queue(DMA_QUEUE* queue);

void dma_get_random_string(char* rand_string, int32_t len);

uint32_t dma_merge_5strings(char* new_string, char* string1, char* string2,
                            const char* string3, const char* string4, char* string5);

uint32_t dma_merge_4strings(char* new_string, char* string1, const char* string2,
                            const char* string3, char* string4);

void dma_convert_byte_to_char(void* byte, char* sha256_char, uint32_t len);

char dma_hex_to_char(int num);

int32_t dma_aes_set_encrypt_key(const unsigned char* userKey, const int bits, AES_KEY* key);

bool dma_aes_encrypt(const unsigned char* in, unsigned char* out, AES_KEY* key);

void dma_base64_encode(const unsigned char* bindata, char* base64, int binlength);

#if defined(__cplusplus)
}
#endif

#endif /* DMA_UTILS_H */

