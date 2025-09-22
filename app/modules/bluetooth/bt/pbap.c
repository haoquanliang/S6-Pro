#include "include.h"

#if BT_PBAP_EN
//static char qp[] = {"=12=34=56=78=9A=BC=DE=F0=12=34=56=78"};  //for test

//获取IOS的电话本，需要在bt_get_class_of_device()中把COD改为0x240408

static u8 qp_convert_do(char *in)
{
    u8 utf8_cod = 0;
    for(u8 i = 0;i < 2;i++){
        if(*in > '@' && *in < 'G'){
            utf8_cod |= (0x0A + *in - 'A') << 4*(1-i);
        }else if(*in > '/' && *in < ':'){
            utf8_cod |= (*in - '0') << 4*(1-i);
        }
        in++;
    }
    return utf8_cod;
}

static void qp_convert(char *out)
{
    u8 cnt = strlen(out);
    u8 utf8_len = 0;
    for(u8 i = 0;i < cnt;i ++){
       if(out[i] != '='){
         out[utf8_len++] = qp_convert_do(&out[i]);
         i++;
       }
    }
    out[utf8_len] = '\0';
}

//type:本地号码[0], 来电号码[1], 去电号码[2], 未接号码[3]
void bt_pbap_data_callback(u8 type, void *item)
{
    //注意函数内不要进行耗时大的操作，会影响电话本获取的速度
    struct pbap_buf_t *p = (struct pbap_buf_t *)item;
    //memcpy(p->name,qp,sizeof(qp));
    if(p->name[0] == '='){
        qp_convert(p->name);
    }
    /*printf("name:");
    for(u8 i = 0;i < strlen(p->name);i++){
         printf("%X ",p->name[i]);

    }*/

    printf("[%d] [name:%s]  ", type, p->name);
    printf("[tele:%s]  ", p->anum);
    if (type) {
        printf("[date:%s]\n", p->bnum);
    } else {
        if (p->bnum[0]) {
            printf("[%s]\n", p->bnum);
        } else {
            printf("\n");
        }
    }
}
#endif //BT_PBAP_EN
