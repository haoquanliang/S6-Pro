#include "config.h"
#if RES_USERBIN_EN
depend(0x01030900);
setresmode(2, USERBIN_START_ADDR, RES_);
loadresdir(res_user);
makeres(user_buf);
makeresdef(res_userbin.h);
xcopy(res_userbin.h, ../../res_userbin.h);
save(user_buf, res_user.bin);
#endif