#include "include.h"

#if DOUBAO_AUTH_EN
void ai_heap_init(void *heap_buf, uint16_t heap_size);
void *ai_malloc(uint32_t size);
void ai_free(void* mem_ptr);

u8 *getcfg_doubao_soft_key(void)
{
    return xcfg_cb.soft_key2;
}

/**
 * Alloc memory.
 *
 * @param[in]  size  size of the mem to malloc.
 *
 * @return  NULL: error.
 */
void *stb_malloc(size_t size)
{
    return ai_malloc(size);
}



/**
 * Free memory.
 *
 * @param[in]  ptr  address point of the mem.
 *
 * @return  none.
 */
void stb_free(void *mem)
{
    ai_free(mem);
}




/**
 * Get current time in milliseconds.
 *
 * @return  elapsed time in milliseconds from system starting.
 */
uint64_t stb_now_ms(void)
{
    return tick_get();
}



/**
 * rand function.
 *
 * @return  random value.
 */
int32_t stb_rand(void)
{
    u32 rand = get_random(0xffff);
    rand = (rand<<16)| get_random(0xff);
    return rand;
}

void stb_msleep(uint32_t ms)
{
    printf("stb_msleep!\n");
}

#endif
