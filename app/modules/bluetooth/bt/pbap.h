#ifndef __PBAP_H
#define __PBAP_H

struct pbap_buf_t{
    char name[160];
    char anum[20];
    char bnum[20];
};

void bt_pbap_data_callback(u8 type, void *item);
#endif // __PBAP_H
