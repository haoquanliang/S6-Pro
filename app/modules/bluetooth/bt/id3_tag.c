#include "id3_tag.h"

#if BT_ID3_TAG_EN

#define BT_ID3_TITLE        1
#define BT_ID3_ARTIST       2
#define BT_ID3_ALBUM        3
#define BT_ID3_NUMBER       4
#define BT_ID3_TOTAL        5
#define BT_ID3_GENRE        6
#define BT_ID3_TIME         7

#define ID3_BUF_LEN         512
u8 id3_buf[ID3_BUF_LEN] AT(.app.buf);
uint8_t cfg_bt_a2dp_id3_en = BT_ID3_TAG_EN;

//packet_type 0:single, 1:start, 2:continue, 3:end（处type0，其他需要拼包）
void bt_id3_tag_callback(u8 packet_type, u8 *buf, u16 size)
{
    // printf("size:%d\n",size);
    //拼包
    static u16 ptr = 0;
    if (packet_type != 0) {
        if(packet_type == 3){
            memcpy(&id3_buf[ptr-1], buf, size);
            ptr += (size-1);
        } else {
            memcpy(&id3_buf[ptr], buf, size);
            ptr += size;
        }
        if (packet_type != 3) {
            return;
        }
        buf = id3_buf;
    }
    ptr = 0;

    //格式
    //byte[0]   : num
    //byte[4:1] : index n
    //byte[6:5] : 编码格式（0x6a:UTF8）
    //byte[8:7] : lenght
    //byte[n:9] : value
    //...
    u16 offset = 0;
    u8 att_num = buf[offset++];

    while (att_num) {
        u32 index = ((u32)buf[offset + 3]) | (((u32)buf[offset + 2]) << 8) | (((u32)buf[offset + 1]) << 16) | (((u32)buf[offset + 0]) << 24);
        index = index;
        offset += 4;
        u16 character = ((u32)buf[offset + 1]) | (((u32)buf[offset + 0]) << 8);
        character = character;
        offset += 2;
        u16 length = ((u32)buf[offset + 1]) | (((u32)buf[offset + 0]) << 8);
        offset += 2;
//        printf("bt id3:%d %d %x %x\n", att_num, index, character, length);
        switch (index) {
            case BT_ID3_TITLE:
                printf("name: %s\n", &buf[offset]);
                print_r(&buf[offset], length + 1);
                #if AB_MATE_APP_EN
                ab_mate_music_info_notify(CMD_MUSIC_TITLE, offset, &buf[0], length + 1);
                #endif
                break;

            case BT_ID3_ARTIST:
                printf("artist: %s\n", &buf[offset]);
//                ab_mate_music_info_notify(CMD_MUSIC_ARTIST, offset, &buf[0], length + 1);
                break;

            default:
                break;
        }

        app_ctrl_id3_tag_process(index, &buf[offset], length);
        offset += length;
        att_num--;
        if((offset >= size) && (packet_type == 0)){
            break;
        }
        character = character;
    }
}

//bt_music_paly_status_info() 调用此函数，就会返回下面的信息。
//buf[0:3]  歌曲时长单位MS，buf[4:7] -> 当前播放位置， buf[8] -> 播放状态0x00:stopped, 0x01:playing, 0x02:paused, 0x03:fwd_seek, 0x04:rev_seek, 0xff:error
void bt_get_paly_status_info_callback(u8 *buf, u16 size)
{
//    uint32_t tmp = 0;
//
//    tmp = big_endian_read_32(buf, 0);
//    TRACE("song time[%2d:%2d]", (tmp / 1000 / 60), (tmp / 1000 % 60));
//
//    tmp = big_endian_read_32(buf, 4);
//    TRACE("--->[%2d:%2d]", (tmp / 1000 / 60), (tmp / 1000 % 60));
//
//    tmp = buf[8];
//    TRACE("sta[%d]\n", tmp);
}
#endif
