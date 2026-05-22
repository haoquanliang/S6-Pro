#include "include.h"

#if EQ_MODE_EN
#if APP_EQ_SET

const eq_param music_eq_tbl[MUSIC_EQ_TBL_LEN] = {
    {&RES_BUF_EQ_NORMAL_EQ,          &RES_LEN_EQ_NORMAL_EQ}, //经典
    {&RES_BUF_EQ_MUSIC_1_EQ,         &RES_LEN_EQ_MUSIC_1_EQ},//爵士   
    {&RES_BUF_EQ_MUSIC_2_EQ,         &RES_LEN_EQ_MUSIC_2_EQ},//电子  
    {&RES_BUF_EQ_MUSIC_3_EQ,         &RES_LEN_EQ_MUSIC_3_EQ},//流行
    {&RES_BUF_EQ_MUSIC_4_EQ,         &RES_LEN_EQ_MUSIC_4_EQ},//古典
    {&RES_BUF_EQ_MUSIC_5_EQ,         &RES_LEN_EQ_MUSIC_5_EQ},//摇滚
    {&RES_BUF_EQ_MUSIC_6_EQ,         &RES_LEN_EQ_MUSIC_6_EQ},//低音增强   
    {&RES_BUF_EQ_MUSIC_7_EQ,         &RES_LEN_EQ_MUSIC_7_EQ},//高音增强  
    {&RES_BUF_EQ_MUSIC_8_EQ,         &RES_LEN_EQ_MUSIC_8_EQ},  
    {&RES_BUF_EQ_MUSIC_9_EQ,         &RES_LEN_EQ_MUSIC_9_EQ},
    {&RES_BUF_EQ_MUSIC_10_EQ,        &RES_LEN_EQ_MUSIC_10_EQ},   
    {&RES_BUF_EQ_MUSIC_11_EQ,        &RES_LEN_EQ_MUSIC_11_EQ},   
    {&RES_BUF_EQ_MUSIC_12_EQ,        &RES_LEN_EQ_MUSIC_12_EQ},  
    {&RES_BUF_EQ_MUSIC_13_EQ,        &RES_LEN_EQ_MUSIC_13_EQ},  
    {&RES_BUF_EQ_MUSIC_14_EQ,        &RES_LEN_EQ_MUSIC_14_EQ},   
};

#else


const eq_param music_eq_tbl[MUSIC_EQ_TBL_LEN] = {
    {&RES_BUF_EQ_NORMAL_EQ,     &RES_LEN_EQ_NORMAL_EQ}, //normal
    {&RES_BUF_EQ_POP_EQ,        &RES_LEN_EQ_POP_EQ},    //pop
    {&RES_BUF_EQ_ROCK_EQ,       &RES_LEN_EQ_ROCK_EQ},   //rock
    {&RES_BUF_EQ_JAZZ_EQ,       &RES_LEN_EQ_JAZZ_EQ},   //jazz
    {&RES_BUF_EQ_CLASSIC_EQ,    &RES_LEN_EQ_CLASSIC_EQ},//classic
    {&RES_BUF_EQ_COUNTRY_EQ,    &RES_LEN_EQ_COUNTRY_EQ},//country
    {&RES_BUF_EQ_BASS_EQ,       &RES_LEN_EQ_BASS_EQ},//bass
    {&RES_BUF_EQ_TREBLE_EQ,     &RES_LEN_EQ_TREBLE_EQ},//treble
};

#endif
#else

const eq_param music_eq_tbl[MUSIC_EQ_TBL_LEN] = {
    {&RES_BUF_EQ_NORMAL_EQ,     &RES_LEN_EQ_NORMAL_EQ}, //normal
};

#endif

