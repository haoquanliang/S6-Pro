#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

///CM Init时，判断该Page参数是否有效，有效则加载
bool cm_loadparam(void *buff, uint page)
{
    return true;
}

///CM Init时，如果找不到有效的参数，则进行出厂值设置
void cm_factory(void *buff, uint page)
{
    u8 *buf = buff;
    memset(buf, 0, 250);

    TRACE("cm_factory: %d\n", page);
    if (page == SYS_CM_PAGE_NUM) {
        //系统参数初始化
        f_msc.file_num = 1;
        sys_cb.vol = SYS_INIT_VOLUME;
        sys_cb.hfp_vol = SYS_INIT_VOLUME / sys_cb.hfp2sys_mul;

#if MUSIC_BREAKPOINT_EN
        memset(&f_msc.brkpt, 0, 10);
#endif // MUSIC_BREAKPOINT_EN

        if (xcfg_cb.lang_id == 2) {
            sys_cb.lang_id = 0;             //出厂默认英文
        } else if (xcfg_cb.lang_id == 3) {
            sys_cb.lang_id = 1;             //出厂默认中文
            printf("44444444444444444444444444444444444444\r\n");
        } else {
            sys_cb.lang_id = xcfg_cb.lang_id;
        }

        PUT_LE16(buf + PARAM_MSC_NUM_SD, f_msc.file_num);
        PUT_LE16(buf + PARAM_MSC_NUM_USB, f_msc.file_num);

        buf[PARAM_SYS_VOL] = sys_cb.vol;
        buf[PARAM_HSF_VOL] = sys_cb.hfp_vol;
        buf[PARAM_LANG_ID] = sys_cb.lang_id;
        PUT_LE32(buf + PARAM_RANDOM_KEY, sys_get_rand_key());
    }
}

#if !ANC_EQ_RES2_EN && FLASH_RESERVE_SIZE >= 0x6000

#define INFO_LOCAL_ADDR     0
#define INFO_MST_ADDR       (INFO_LOCAL_ADDR + 6)
#define INFO_TWS_LINK       (INFO_MST_ADDR + 6)
#define INFO_OPEN_ERR       (INFO_TWS_LINK + 0x3e)
#define INFO_ERR_ITEM       (INFO_OPEN_ERR + 6+12)

//RES2没启用时，存到RES2的flash地址
//初次下载一定要擦除全片

u32 save_buf[0x40];
void cm_info_write(u8 *buf, u16 offset, u8 len)
{
    u8 rlen;
    u8 rbuf[32];
    u32 addr = FLASH_SIZE-0x6000 + offset;
//    if(!bt_tws_is_ms_switch()) {
//        return;
//    }
    rlen = (len>32)? 32 : len;
    os_spiflash_read(rbuf, addr, rlen);

    if (INFO_TWS_LINK == offset) {
        u8 page_num = 10;
        while (page_num) {
            u8 add_flag = 0;
            os_spiflash_read(rbuf, addr, 20);
//            printf("old page[%x]:\n", 10-page_num);
//            print_r(rbuf, 16);
            for (int i = 0; i < 20; i++) {
                if (rbuf[i] != 0xff) {
                    add_flag = 1;
                    break;
                }
            }

            if (add_flag == 0 || memcmp(rbuf, buf, 20) == 0) {  //如果是全ff或者是同样的数据，就直接在原位置写了
                break;
            } else {
                addr += 256;                                    //因为都是组队绑定，没必要排序，按顺序写就好
            }
            page_num--;
        }
    } else {
        for(uint i=0; i<rlen; i++) {
            if(rbuf[i] != 0xff) {
                TRACE("cm_ignore[%04x]: %d\n", addr, len);
                TRACE_R(buf, len);
                return;
            }
        }
    }

    TRACE("cm_save[%04x]: %d\n", addr, len);
    TRACE_R(buf, len);

    memcpy(save_buf, buf, len);
    os_spiflash_program(save_buf, addr, len);
}
//
//bool cm_info_read(u8 *buf, u16 offset, u8 len)
//{
//    u32 addr = FLASH_SIZE-0x6000 + offset;
//
//    memset(buf, 0x55, len);
//    os_spiflash_read(buf, addr, len);
//    return true;
//}
#endif
