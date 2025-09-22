loadresdir(res);
loadresdir(res_user);
makeres(res_buf);
makeresdef(res.h);
xcopy(res.h, ../../res.h);
save(res_buf, res.bin);
