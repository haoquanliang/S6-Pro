#include "config.h"
depend(0x01030500);
setflash(1, FLASH_SIZE, FLASH_ERASE_4K, FLASH_DUAL_READ, FLASH_QUAD_READ);
#if ANC_EQ_RES2_EN
setkeepseg(0x1000);
#endif
#if ASR_EN
setuserbin(ASR_BASE_ADDR, ASR_BASE_LEN, weight.bin);
#endif
setspace(FLASH_RESERVE_SIZE);
//setauth(0xDDE05A0D, soft_key1);    //LDAC加密狗
//setauth(0xEF3629A2, soft_key2);    //DOUBAO加密狗
#if (FOT_EN && AB_FOT_TYPE == AB_FOT_TYPE_PACK)
setunpack(unpack.bin);
setpkgarea(AB_FOT_PACK_START_ADDR, AB_FOT_PACK_SIZE);
#endif
setkey(FUNCKEY_TWS);
addkey(FUNCKEY_SNDP);
addkey(FUNCKEY_LHDC);
#if FOT_GUID_CHECK_EN
setid(AB000103-FFFF-FFFF-FFFF-FFFFFFFFFFFF); // 自定义GUID, 用于区分产品，限制相同芯片不同产品固件互相升级
#endif
setver(SW_VERSION);
#if RES_USERBIN_EN
setuserbin(USERBIN_START_ADDR, USERBIN_SIZE, res_user.bin, 0);
#endif
make(dcf_buf, header.bin, app.bin, res.bin, xcfg.bin, updater.bin);
save(dcf_buf, app.dcf);
