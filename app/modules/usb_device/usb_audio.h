#ifndef _USB_AUDIO_H
#define _USB_AUDIO_H

typedef enum{
    MIC_NORMAL,     //原声
    MIC_KTV,        //Reverb/Echo
    MIC_ELEC,       //电音
    MIC_PITCH,      //魔音
    MIC_MODE_NUM,
}mic_mode_t;

typedef enum{
    DAC_NORMAL,        //原声
    DAC_3D,            //3D音效
    DAC_BASS,          //音乐高低音
    DAC_VBASS,         //虚拟低音
    DAC_CHOURS,        //合唱
    DAC_PINGPONG,      //乒乓效果
    DAC_AUTOWAH,       //哇音
    DAC_VOCAL_REMOVER, //人声消除
    DAC_MODE_NUM,
}dac_mode_t;

typedef struct{
    u8 mic_mode;
    s8 dac_mode;
}usb_effect_t;
#endif
