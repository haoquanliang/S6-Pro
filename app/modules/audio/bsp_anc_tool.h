#ifndef __BSP_ANC_TOOL_H__
#define __BSP_ANC_TOOL_H__


void bsp_anc_tool_dump_start(u32 link, u8 pcm_spr, u8 pcm_bits);
void bsp_anc_tool_dump_stop(void);

void anc_tool_process(void);
bool bsp_anc_tool_rx_done(u8* rx_buf, u8 type);
void bsp_anc_tool_parse_cmd(void);
void anc_tool_spp_connect_callback(void);
void anc_tool_spp_disconnect_callback(void);

enum ANC_TOOL_TODO_FLAG {
    ANC_TOOL_TODO_SPP_DISCONNECT,
    ANC_TOOL_TODO_FB_MSC_SIMU_DATA_FULL,
};

#endif // __BSP_ANC_TOOL_H__
