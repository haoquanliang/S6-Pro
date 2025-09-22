#ifndef __DUEROS_DMA_EXTERN_H_
#define __DUEROS_DMA_EXTERN_H_

#include "dma_wrapper.h"
typedef void (* dueros_dma_thread_process_register_t)(uint32_t timeout_ms);

void dma_free(void *ptr);
void *dma_malloc(uint32_t size);
void dma_mm_init(uint32_t free_memory_start, uint32_t free_memory_end);
void os_dueros_dma_thread_create(void);
void os_dueros_dma_thread_process_register(dueros_dma_thread_process_register_t process_func);
int32_t os_dueros_dma_thread_sem_signal(void);
int32_t os_dueros_dma_thread_sem_wait(uint32_t timeout_ms);
int32_t os_dueros_dma_thread_mutex_unlock(int32_t mutex_id);
int32_t os_dueros_dma_thread_mutex_lock(DMA_MUTEX_ID mutex_id);
#endif
