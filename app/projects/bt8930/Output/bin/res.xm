#include "config.h"
loadresdir(res);
#if !RES_USERBIN_EN
loadresdir(res_user);
#endif
makeres(res_buf);
makeresdef(res.h);
xcopy(res.h, ../../res.h);
save(res_buf, res.bin);