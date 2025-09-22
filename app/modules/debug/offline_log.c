#include "include.h"

#if OFFLINE_LOG_EN
#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define PAGE_SIZE                   256
#define BLOCK_SIZE                  4096
#define NPAGE_PER_BLOCK             16

struct offline_log_t {
    uint32_t flash_start_addr;
    uint8_t block_size;
    uint8_t page_offset;
    uint8_t page_idex;
    uint8_t w_block;
    uint8_t e_block;
    uint8_t d_block;
    uint8_t used_block_cnt;
    uint8_t save_block_size;
    uint8_t page_write_flag;
    uint8_t page_cache[2][PAGE_SIZE];
    uint8_t init;
};

static struct offline_log_t offline_log;

/// 加载flash
static void offline_log_flash_read(u32 flash_addr, u8 *buf, uint len)
{
    os_spiflash_read(buf, flash_addr, len);
//    printf("flash_addr:0x%x\n", flash_addr);
//    print_kr(buf, len);
//    WDT_CLR();
}

/// 写入flash
static void offline_log_flash_write(u32 flash_addr, u8 *buf)
{
    os_spiflash_program(buf, flash_addr, PAGE_SIZE);
}

/// 擦除flash
static void offline_log_flash_erase(uint flash_addr)
{
    os_spiflash_erase(flash_addr);
}

int offline_log_get_size(void)
{
    int size;
    if (!offline_log.page_idex && !offline_log.page_offset) {
        size = offline_log.used_block_cnt * BLOCK_SIZE;
    } else {
        size = offline_log.used_block_cnt ? (offline_log.used_block_cnt - 1) * BLOCK_SIZE : 0;
        size += offline_log.page_idex * PAGE_SIZE + offline_log.page_offset;
    }
    return size;
}

int offline_log_get(uint32_t addr, uint8_t *buf, uint32_t size)
{
    int block_idex;
    int page_idex;
    int page_offset;

    if (addr == 0) {
        offline_log.d_block = offline_log.e_block;
    }

    block_idex = offline_log.d_block + addr / BLOCK_SIZE;
    block_idex = block_idex % offline_log.block_size;
    page_idex = addr % BLOCK_SIZE / PAGE_SIZE;
    page_offset = addr % PAGE_SIZE;

//    printf("\nD %d %d %d %d\n", addr, block_idex, page_idex, page_offset);
    if ((block_idex == offline_log.w_block) && (page_idex == offline_log.page_idex)) {
        u8 *page_cache = offline_log.page_cache[offline_log.page_idex & 0x1];
        if ((page_offset + size) <= offline_log.page_offset) {
            memcpy(buf, page_cache + page_offset, size);
            return size;
        } else {
            memcpy(buf, page_cache + page_offset, offline_log.page_offset - page_offset);
            return offline_log.page_offset - page_offset;
        }
    }

    if ((page_offset + size) > PAGE_SIZE) {
        uint32_t read_size = PAGE_SIZE - page_offset;
        uint32_t remain_size = size - read_size;
        offline_log_flash_read(offline_log.flash_start_addr + block_idex * BLOCK_SIZE + page_idex * PAGE_SIZE + page_offset, buf, read_size);
        page_idex = page_idex + 1;
        if (page_idex >= NPAGE_PER_BLOCK) {
            page_idex -= NPAGE_PER_BLOCK;
            block_idex = (block_idex + 1) % offline_log.block_size;
        }
        if ((block_idex == offline_log.w_block) && (page_idex == offline_log.page_idex)) {
            u8 *page_cache = offline_log.page_cache[offline_log.page_idex & 0x1];
            if (remain_size > offline_log.page_offset) {
                memcpy(buf + read_size, page_cache, offline_log.page_offset);
//                printf("end %d\n", read_size + offline_log.page_offset);
                return read_size + offline_log.page_offset;
            } else {
                memcpy(buf + read_size, page_cache, remain_size);
            }
        } else {
            offline_log_flash_read(offline_log.flash_start_addr + block_idex * BLOCK_SIZE + page_idex * PAGE_SIZE, buf + read_size, remain_size);
        }
    } else {
        offline_log_flash_read(offline_log.flash_start_addr + block_idex * BLOCK_SIZE + page_idex * PAGE_SIZE + page_offset, buf, size);
    }

    return size;
}


void offline_log_dump(void (*dump_cb)(uint8_t *buf, uint16_t len))
{
    uint8_t *page_cache = offline_log.page_cache[(offline_log.page_idex + 1) & 0x1];
#if 0
    for (uint8_t block_idex = offline_log.e_block, block_cnt = offline_log.used_block_cnt; block_cnt; block_cnt--) {
        for (uint8_t page_idex = 0; page_idex < NPAGE_PER_BLOCK; page_idex++) {
            if ((block_idex == offline_log.w_block) && (page_idex == offline_log.page_idex)) {
                if (dump_cb) {
                    page_cache = offline_log.page_cache[offline_log.page_idex & 0x1];
                    printf("dump %d\n", offline_log.page_offset);
                    dump_cb(page_cache, offline_log.page_offset);
                }
                return;
            }
            offline_log_flash_read(offline_log.flash_start_addr + block_idex * BLOCK_SIZE + page_idex * PAGE_SIZE, page_cache, PAGE_SIZE);
            if (dump_cb) {
                dump_cb(page_cache, PAGE_SIZE);
            }

            printf("dump log %d %d %d\n", block_cnt, block_idex, offline_log.w_block);
        }
        block_idex = (block_idex + 1) % offline_log.block_size;
    }
#else
    int total = offline_log_get_size();
    int read_size = 0;

    TRACE("offline log used %d max %d start: %d end %d pidx %d offset %d total %d\n",
             offline_log.used_block_cnt, offline_log.block_size, offline_log.e_block, offline_log.w_block, offline_log.page_idex, offline_log.page_offset, total);
    for (int addr = 0; addr < total; addr += read_size) {
        read_size = offline_log_get(addr, page_cache, 240);
        if (read_size) {
            if (dump_cb) {
                dump_cb(page_cache, read_size);
            }
        } else {
            TRACE("dump log end %d\n", addr);
            break;
        }
    }
#endif
}

ALIGNED(256) NO_INLINE
static void offline_log_write_proccess_do(void)
{
    uint8_t wpage_idex;
    offline_log.page_write_flag = 0;
    if (offline_log.used_block_cnt == offline_log.block_size) {
        if (offline_log.page_idex >= NPAGE_PER_BLOCK) { //最后一个page保留
            TRACE("offline log full\n");
            GLOBAL_INT_DISABLE();
            offline_log.page_idex = NPAGE_PER_BLOCK - 1;
            GLOBAL_INT_RESTORE();

            uint8_t *page_cache = offline_log.page_cache[offline_log.page_idex & 0x1];
            memcpy(page_cache, "#overflow...#", 13);
            offline_log.page_offset = 13;
            return;
        }
    }

    wpage_idex = (offline_log.page_idex - 1) % NPAGE_PER_BLOCK;
    offline_log_flash_write(wpage_idex * PAGE_SIZE + offline_log.w_block * BLOCK_SIZE + offline_log.flash_start_addr, offline_log.page_cache[wpage_idex & 0x1]);
    GLOBAL_INT_DISABLE();
    if (offline_log.page_idex >= NPAGE_PER_BLOCK) {
        offline_log.page_idex -= NPAGE_PER_BLOCK;
        offline_log.w_block++;
        if (offline_log.page_offset) {
            offline_log.used_block_cnt++;
        }
        if (offline_log.w_block >= offline_log.block_size) {
            offline_log.w_block = 0;
        }
    }
    GLOBAL_INT_RESTORE();
    TRACE("offline log write %d (%d %d) %d\n", offline_log.page_idex, offline_log.e_block, offline_log.w_block, offline_log.used_block_cnt);
}

AT(.text.func.process.log)
void offline_log_write_proccess(void)
{
    if (offline_log.page_write_flag) {
        offline_log_write_proccess_do();
    }
}

ALIGNED(128)
void offline_log_write_cache_page(void)
{
    uint8_t *page_cache;
    if (offline_log.page_offset) {
        offline_log.page_write_flag = 1;
        GLOBAL_INT_DISABLE();
        offline_log.page_idex++;
        GLOBAL_INT_RESTORE();
        offline_log.page_offset = 0;
        offline_log_write_proccess_do();
        page_cache = offline_log.page_cache[offline_log.page_idex & 0x1];
        memset(page_cache, 0, PAGE_SIZE);
    }
}

ALIGNED(128) NO_INLINE
static void offline_log_erase_proccess_do(void)
{
    offline_log_flash_erase(offline_log.e_block * BLOCK_SIZE + offline_log.flash_start_addr);
    GLOBAL_INT_DISABLE();
    offline_log.e_block = (offline_log.e_block + 1) % offline_log.block_size;
    offline_log.used_block_cnt--;
    GLOBAL_INT_RESTORE();
    TRACE("offline_log_erase (%d %d) %d\n", offline_log.e_block, offline_log.w_block, offline_log.used_block_cnt);
}

AT(.text.func.process.log)
void offline_log_erase_proccess(void)
{
    if (offline_log.used_block_cnt > offline_log.save_block_size) {
        offline_log_erase_proccess_do();
    }
}

ALIGNED(256)
void offline_log_put(u8 *buf, u8 len)
{
    uint8_t *page_cache = offline_log.page_cache[offline_log.page_idex & 0x1];

    if (!len || !offline_log.init || len >= PAGE_SIZE) {
        return;
    }
    if (offline_log.page_offset == 0 && offline_log.page_idex == 0) { //之前没有用到当前block
        offline_log.used_block_cnt++;
    }
    if ((offline_log.page_offset + len) < PAGE_SIZE) {
        memcpy(page_cache + offline_log.page_offset, buf, len);
        offline_log.page_offset += len;
    } else {
        memcpy(page_cache + offline_log.page_offset, buf, PAGE_SIZE - offline_log.page_offset);
        if (offline_log.page_write_flag) {
            offline_log_write_proccess_do();
        }
        GLOBAL_INT_DISABLE();
        offline_log.page_idex++;
        page_cache = offline_log.page_cache[offline_log.page_idex & 0x1];
        GLOBAL_INT_RESTORE();
        len = len + offline_log.page_offset - PAGE_SIZE;
        memcpy(page_cache, buf + PAGE_SIZE - offline_log.page_offset, len);
        offline_log.page_offset = len;
        memset(page_cache + len, 0, PAGE_SIZE - len);

        offline_log.page_write_flag = 1;
    }
}

static bool offline_log_check_block(uint32_t start_addr, uint8_t *used_page_idex)
{
    uint8_t page_idex;
    uint8_t *page_cache = offline_log.page_cache[0];

    for (page_idex = 0; page_idex < NPAGE_PER_BLOCK; page_idex++) {
        offline_log_flash_read(start_addr + page_idex * PAGE_SIZE, page_cache, PAGE_SIZE);
        for (int i = 0; i < PAGE_SIZE; i++) {
            if (page_cache[i] != 0xff) {
                if (used_page_idex) {
                   *used_page_idex = page_idex + 1;
                } else {
                    return true;
                }
            }
        }
    }
    return false;
}

static void offline_log_used_check(void)
{
    uint8_t block_idex;
    uint8_t cur_used = 0;
    uint8_t used_block = 0;

    for (block_idex = 0; block_idex < offline_log.block_size; block_idex++) {
        if (offline_log_check_block(block_idex * BLOCK_SIZE + offline_log.flash_start_addr, NULL)) {
            if (block_idex && !offline_log.e_block) {
                if (!cur_used) { //前一个是未使用，当前块是数据开始
                    offline_log.e_block = block_idex;
                    TRACE("start %d\n", block_idex);
                }
            }
            cur_used = 1;
            offline_log.used_block_cnt++;
        } else {
            if (block_idex) {
                if (cur_used && !offline_log.w_block) { //前一个已使用，当前块前一个是数据尾部
                    TRACE("end %d\n", block_idex);
                    offline_log.w_block = block_idex - 1;
                    offline_log_check_block(offline_log.w_block * BLOCK_SIZE + offline_log.flash_start_addr, &offline_log.page_idex);
                    if (offline_log.page_idex >= NPAGE_PER_BLOCK) { //当前block用完，指向下一个
                        offline_log.page_idex = 0;
                        offline_log.w_block++;
                    }
                }
            }
            cur_used = 0;
        }
    }


    if (offline_log.used_block_cnt == offline_log.block_size) { //用满
        for (block_idex = 0; block_idex < offline_log.block_size; block_idex++) {
            offline_log_check_block(block_idex * BLOCK_SIZE + offline_log.flash_start_addr, &offline_log.page_idex);
            if (offline_log.page_idex == (NPAGE_PER_BLOCK - 1)) {
                offline_log.w_block = block_idex;
                offline_log.e_block = (block_idex + 1) % offline_log.block_size;
                TRACE("offline_log.page_idex %d\n", offline_log.page_idex);
                break;
            } else {
                offline_log.w_block = offline_log.block_size - 1;
                offline_log.page_idex = NPAGE_PER_BLOCK - 2;
                offline_log.e_block = 0;
              //  printf("offline_log.page_idex %d\n", offline_log.page_idex);
            }
        }
    }

    if (offline_log.w_block >  offline_log.e_block) {
        used_block = offline_log.w_block -  offline_log.e_block;
    } else {
        used_block = offline_log.w_block + offline_log.block_size -  offline_log.e_block;
    }
    if (offline_log.page_idex){
        used_block++;
    }

    TRACE("offline log used %d %d max %d start: %d end %d pidx %d\n",
          offline_log.used_block_cnt, used_block, offline_log.block_size, offline_log.e_block, offline_log.w_block, offline_log.page_idex);

    if ((offline_log.used_block_cnt != used_block) && (offline_log.e_block != offline_log.w_block)) { //计算的大小跟时间使用不一致，按头尾计算
        printf("offline log err block cnt %d %d\n", offline_log.used_block_cnt, used_block);
        offline_log.used_block_cnt = used_block;
    }
}

void offline_log_init(uint32_t start_addr, uint32_t size, uint32_t save_size)
{
    if (size > 0x80000) {
        printf("offline log err size %d\n", size);
        return;
    }
    TRACE("offline log init:%x %x %x\n", start_addr, size, save_size);
    memset(&offline_log, 0, sizeof(struct offline_log_t));
    offline_log.flash_start_addr = start_addr;
    offline_log.block_size = size / BLOCK_SIZE;
    offline_log.save_block_size = save_size / BLOCK_SIZE;

    offline_log_used_check();
    offline_log.init = true;

    RTCCON2 = 31250;
    RTCCON0 = (RTCCON0 & ~(3 << 8)) | (3 << 8);   //XOSC DIV12
//    offline_log_dump();
    offline_log_exception_check();
}
#endif
