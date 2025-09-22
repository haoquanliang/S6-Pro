#include "include.h"
#include "asr.h"

#if ASR_PREFETCH_EN && ASR_EN
AT(.ws_text.strong)
const int8_t tdnn_weight_1[1] = {0};
AT(.ws_text.strong)
const int8_t tdnn_weight_2[1] = {0};
AT(.ws_text.strong)
const int8_t tdnn_weight_3[1] = {0};
AT(.ws_text.strong)
const int8_t tdnn_weight_4[1] = {0};
AT(.ws_text.strong)
const int8_t tdnn_weight_5[1] = {0};
AT(.ws_text.strong)
const int8_t tdnn_weight_6[1] = {0};
AT(.ws_text.strong)
const int8_t tdnn_weight_7[1] = {0};
#endif

#if ASR_EN

#define tdnn_abs(x)   ((x)>=0?(x):(-x))
#define tdnn_round(x) ((x)>=0?((x)+0.5):((x)-0.5))

AT(.com_text.tdnn)
void tdnn_quant(float *in_buf, int in_dim, int T, int skip,
                     int8_t *out_buf, float *out_scale) {
    int i, j;
    float abs_max = 0.0;
    float scale;
    int len = in_dim * T;


    for (i = 0; i < len; i++) {
        if (skip && i == in_dim) i = in_dim << 1;
        float abs_val = tdnn_abs(in_buf[i]);
        if (abs_val > abs_max) abs_max = abs_val;
    }

    scale = 127.0 / abs_max;

    for (i = 0, j = 0; i < len; i++, j++) {
        if (skip && i == in_dim) i = in_dim << 1;
        out_buf[j] = (int8_t)(in_buf[i] * scale);
    }

    *out_scale = 1.0 / scale;
}
#endif
