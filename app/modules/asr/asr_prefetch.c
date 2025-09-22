#include "include.h"
#include "asr.h"


#if ASR_PREFETCH_EN && ASR_EN

///x0,sum矩阵只可以使用ram0（0x50000~0x54000）和ram1（0x54000~0x59000），y0矩阵可以使用weight0-ram1(0x40000~~0x59000)，且均要4字节对齐
///并且x0和y0不能使用同一块ram
void matrix_hw(int32_t* sum, int8_t* x0, int8_t *y0, s16 loop);
void npu_memcpy_int16(int16_t *c, int16_t *a, int32_t a_size);
void npu_matrix_kick_wait(void);

#if NPU_CONTIN_CAL_EN
AT(.ws_asr.ram0.sum)
int32_t sum_buffer[MAX_NPU_MATRIX * 16];
#else
AT(.ws_asr.ram0.sum.s)
int sum_buffer[0x1e8];
AT(.ws_asr.ram0.sum)
int32_t matrix_sum ;
#endif

AT(.ws_asr.offset)
int8_t asr_offset[12];	            //让asr库里数组对齐16字节
AT(.npu_matrix.ram0.x)
int8_t npu_matrix_x[256 * MAX_NPU_MATRIX];

typedef struct {
    u16 all_frame;
    u16 frame_len;
    u8 load_frame;
} tdnn_prenum_t;

AT(.com_rodata.tdnn)
const tdnn_prenum_t tdnn_prenum[7] = {
    {128, 120, 68},
    {128, 256, 32},
    {128, 256, 32},
    {128, 256, 32},
    {128, 256, 32},
    {128, 128, 64},
    {488, 128, 64},
};

typedef struct {
    u8 load_frame;
    u8 *buf;
} tdnn_cache_t;

typedef struct {
    u16 all_frame;
    u8 index;
    u8 toggle;
    u32 load_addr;
    tdnn_cache_t cache[2];
} tdnn_pretetch_t;

extern const int16_t tdnn_mean_1[128];
extern const int16_t tdnn_bias_1[128];
extern const float tdnn_var_1[128];
extern const int16_t tdnn_mean_2[128];
extern const int16_t tdnn_bias_2[128];
extern const float tdnn_var_2[128];
extern const int16_t tdnn_mean_3[128];
extern const int16_t tdnn_bias_3[128];
extern const float tdnn_var_3[128];
extern const int16_t tdnn_mean_4[128];
extern const int16_t tdnn_bias_4[128];
extern const float tdnn_var_4[128];
extern const int16_t tdnn_mean_5[128];
extern const int16_t tdnn_bias_5[128];
extern const float tdnn_var_5[128];
extern const int16_t tdnn_mean_6[128];
extern const int16_t tdnn_bias_6[128];
extern const float tdnn_var_6[128];
extern const int16_t tdnn_bias_7[488];
extern const float tdnn_scale_7[488];

int16_t tdnn_ram_mean1[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias1[128] AT(.ws_asr.test);
float tdnn_ram_var1[128] AT(.ws_asr.test);
int16_t tdnn_ram_mean2[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias2[128] AT(.ws_asr.test);
float tdnn_ram_var2[128] AT(.ws_asr.test);
int16_t tdnn_ram_mean3[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias3[128] AT(.ws_asr.test);
float tdnn_ram_var3[128] AT(.ws_asr.test);
int16_t tdnn_ram_mean4[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias4[128] AT(.ws_asr.test);
float tdnn_ram_var4[128] AT(.ws_asr.test);
int16_t tdnn_ram_mean5[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias5[128] AT(.ws_asr.test);
float tdnn_ram_var5[128] AT(.ws_asr.test);
int16_t tdnn_ram_mean6[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias6[128] AT(.ws_asr.test);
float tdnn_ram_var6[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias7[488] AT(.ws_asr.test);
float tdnn_ram_scale7[488] AT(.ws_asr.test);

u8 tdnn_buffer[2][0x2000] AT(.npu_matrix.ram0.tdnn);
tdnn_pretetch_t tdnn_pretetch AT(.ws_asr.test);

u8 asr_prefetch_kisck;

AT(.com_text.tdnn)
void tdnn_compute_float_cal(float *out_buf, const int16_t *tdnn_mean, const float *tdnn_scale,
                            const float *tdnn_var, const int16_t *tdnn_bias, float in_scale, u16 matrix_cnt_t, u16 float_cnt_t, int last_layer)
{

    int32_t sum_curr = 0, z = 0;
    for (int cnt = float_cnt_t; cnt < (matrix_cnt_t + float_cnt_t); cnt++) {
        z = sum_buffer[(sum_curr<<4)] * in_scale + tdnn_bias[cnt];
        if (!last_layer) {
            if (z <= 0) {
                z = 0;
            }
            out_buf[cnt] = (z - tdnn_mean[cnt]) * tdnn_var[cnt];
        } else {
            out_buf[cnt] = z * tdnn_scale[cnt];
        }
        sum_curr++;
    }
}

AT(.com_text.tdnn)
void tdnn_compute(int8_t *in_buf, float in_scale, int in_dim, int out_dim,
                  const int8_t *tdnn_weight, const int16_t *tdnn_bias,
                  const int16_t *tdnn_mean, const float *tdnn_var, const float *tdnn_scale,
                  float *out_buf, int last_layer) {
    int i;
    if (asr_prefetch_kisck) {
        return;
    }

    tdnn_pretetch_t *t = &tdnn_pretetch;
    const tdnn_prenum_t *p = &tdnn_prenum[t->index];
    u32 frame_len = p->frame_len * p->load_frame;
//    printf("tdnn_weight:%x out_dim:%d in_dim:%d index:%d frame_len:%d\n", tdnn_weight, out_dim, in_dim, t->index, frame_len);

    if (tdnn_mean == tdnn_mean_1) {
        tdnn_mean = tdnn_ram_mean1;
        tdnn_bias = tdnn_ram_bias1;
        tdnn_var = tdnn_ram_var1;
    } else if (tdnn_mean == tdnn_mean_2) {
        tdnn_mean = tdnn_ram_mean2;
        tdnn_bias = tdnn_ram_bias2;
        tdnn_var = tdnn_ram_var2;
    } else if (tdnn_mean == tdnn_mean_3) {
        tdnn_mean = tdnn_ram_mean3;
        tdnn_bias = tdnn_ram_bias3;
        tdnn_var = tdnn_ram_var3;
    } else if (tdnn_mean == tdnn_mean_4) {
        tdnn_mean = tdnn_ram_mean4;
        tdnn_bias = tdnn_ram_bias4;
        tdnn_var = tdnn_ram_var4;
    } else if (tdnn_mean == tdnn_mean_5) {
        tdnn_mean = tdnn_ram_mean5;
        tdnn_bias = tdnn_ram_bias5;
        tdnn_var = tdnn_ram_var5;
    } else if (tdnn_mean == tdnn_mean_6) {
        tdnn_mean = tdnn_ram_mean6;
        tdnn_bias = tdnn_ram_bias6;
        tdnn_var = tdnn_ram_var6;
    } else {
        tdnn_scale = tdnn_ram_scale7;
        tdnn_bias = tdnn_ram_bias7;
    }
//    GPIOBSET |= BIT(4);
    spiflash_lock();
//    GPIOBSET = BIT(1);///

#if NPU_CONTIN_CAL_EN
    u8 matrix_cnt = 0;
    u16 float_cnt = 0;
    memset(sum_buffer, 0, 4 * MAX_NPU_MATRIX * 16);
#endif

    for (i = 0; i < out_dim; i++) {
        u16 offset = (i * in_dim) % frame_len;
        //load下一帧
        if (offset == 0) {
            u16 load_frame = 0;
            t->toggle ^= 1;
            if (t->all_frame == 0) {
                t->index++;
                if (t->index >= 7) {
                    t->index = 0;
                }
                p = &tdnn_prenum[t->index];
                t->all_frame = p->all_frame;
                load_frame = p->load_frame;
            } else {
                if (t->all_frame >= p->load_frame) {
                    load_frame = p->load_frame;
                } else {
                    load_frame = t->all_frame;
                }
            }
            u32 load_len = load_frame*p->frame_len;

            spiflash_read_wait();

//            GPIOESET |= BIT(4);///
            spiflash_read_kick(t->cache[t->toggle ^ 1].buf, t->load_addr, load_len);
//            GPIOECLR |= BIT(4);///
            t->all_frame -= load_frame;
            t->load_addr += load_len;
            if (t->load_addr >= (ASR_BASE_ADDR + ASR_BASE_LEN)) {
                t->load_addr = ASR_BASE_ADDR;
            }
        }

        int8_t *tdnn_buffer_t = (int8_t *)t->cache[t->toggle].buf + offset;
#if !NPU_CONTIN_CAL_EN
        matrix_sum = 0;
        u8 mod = ((int)tdnn_buffer_t % 16);
        if (mod) {  //地址对齐16bytes
            memcpy(npu_matrix_x, tdnn_buffer_t, in_dim);
            matrix_hw(&matrix_sum, npu_matrix_x, in_buf, in_dim);
        } else {
            matrix_hw(&matrix_sum, tdnn_buffer_t, in_buf, in_dim);
        }
        npu_matrix_kick_wait();
        sum_buffer[i] = matrix_sum;
#else
    #if NPU_MEMCPY_EN
        u8 mod = ((int)tdnn_buffer_t % 16);
        if (mod) {  //地址对齐16bytes
            memcpy(&npu_matrix_x[matrix_cnt * 256], tdnn_buffer_t, in_dim);
        } else {
            npu_memcpy_int16((int16_t*)&npu_matrix_x[matrix_cnt * 256], (int16_t*)tdnn_buffer_t, 128);
        }
    #else
        memcpy(&npu_matrix_x[matrix_cnt * 256], tdnn_buffer_t, in_dim);
    #endif
        matrix_hw(&sum_buffer[matrix_cnt * 16], &npu_matrix_x[matrix_cnt * 256], in_buf, in_dim);
        matrix_cnt++;

        if (out_dim == i + 1) {
            npu_matrix_kick_wait();
            tdnn_compute_float_cal(out_buf, tdnn_mean, tdnn_scale, tdnn_var, tdnn_bias, in_scale, matrix_cnt, float_cnt, last_layer);
            float_cnt += matrix_cnt;
            memset(sum_buffer, 0, 4 * MAX_NPU_MATRIX * 16);
            matrix_cnt = 0;
        } else {
            if (matrix_cnt == MAX_NPU_MATRIX) {
                npu_matrix_kick_wait();
                tdnn_compute_float_cal(out_buf, tdnn_mean, tdnn_scale, tdnn_var, tdnn_bias, in_scale, matrix_cnt, float_cnt, last_layer);
                float_cnt += matrix_cnt;
                memset(sum_buffer, 0, 4 * MAX_NPU_MATRIX * 16);
                matrix_cnt = 0;

            }
        }
#endif
    }

    spiflash_read_wait();
    spiflash_unlock();
#if !NPU_CONTIN_CAL_EN
    for (i = 0; i < out_dim; i++) {
        matrix_sum = sum_buffer[i];
        int32_t z = matrix_sum * in_scale + tdnn_bias[i];
        if (!last_layer) {
            if (z <= 0) {
                z = 0;
            }
            out_buf[i] = (z - tdnn_mean[i]) * tdnn_var[i];
        } else {
            out_buf[i] = z * tdnn_scale[i];
        }
    }
#endif
}

ALIGNED(512)
void asr_prefetch_init_do(void)
{
    tdnn_pretetch_t *t = &tdnn_pretetch;
    memset(t, 0, sizeof(tdnn_pretetch_t));
    const tdnn_prenum_t *p = &tdnn_prenum[t->index];
    t->load_addr = ASR_BASE_ADDR;
    t->cache[0].buf = tdnn_buffer[0];
    t->cache[1].buf = tdnn_buffer[1];

    //先读
    u32 load_len = p->load_frame*p->frame_len;
    spiflash_lock();
    spiflash_read_kick(t->cache[0].buf, t->load_addr, load_len);
    spiflash_read_wait();
    t->cache[0].load_frame = p->load_frame;
    t->all_frame = p->all_frame - p->load_frame;        //已经load了一帧，减掉
    t->load_addr += load_len;
    spiflash_unlock();

    memcpy(tdnn_ram_mean1,     tdnn_mean_1,     sizeof(tdnn_mean_1));
    memcpy(tdnn_ram_bias1,     tdnn_bias_1,     sizeof(tdnn_bias_1));
    memcpy(tdnn_ram_var1,      tdnn_var_1,      sizeof(tdnn_var_1));
    memcpy(tdnn_ram_mean2,     tdnn_mean_2,     sizeof(tdnn_mean_2));
    memcpy(tdnn_ram_bias2,     tdnn_bias_2,     sizeof(tdnn_bias_2));
    memcpy(tdnn_ram_var2,      tdnn_var_2,      sizeof(tdnn_var_2));
    memcpy(tdnn_ram_mean3,     tdnn_mean_3,     sizeof(tdnn_mean_3));
    memcpy(tdnn_ram_bias3,     tdnn_bias_3,     sizeof(tdnn_bias_3));
    memcpy(tdnn_ram_var3,      tdnn_var_3,      sizeof(tdnn_var_3));
    memcpy(tdnn_ram_mean4,     tdnn_mean_4,     sizeof(tdnn_mean_4));
    memcpy(tdnn_ram_bias4,     tdnn_bias_4,     sizeof(tdnn_bias_4));
    memcpy(tdnn_ram_var4,      tdnn_var_4,      sizeof(tdnn_var_4));
    memcpy(tdnn_ram_mean5,     tdnn_mean_5,     sizeof(tdnn_mean_5));
    memcpy(tdnn_ram_bias5,     tdnn_bias_5,     sizeof(tdnn_bias_5));
    memcpy(tdnn_ram_var5,      tdnn_var_5,      sizeof(tdnn_var_5));
    memcpy(tdnn_ram_mean6,     tdnn_mean_6,     sizeof(tdnn_mean_6));
    memcpy(tdnn_ram_bias6,     tdnn_bias_6,     sizeof(tdnn_bias_6));
    memcpy(tdnn_ram_var6,      tdnn_var_6,      sizeof(tdnn_var_6));
    memcpy(tdnn_ram_bias7,     tdnn_bias_7,     sizeof(tdnn_bias_7));
    memcpy(tdnn_ram_scale7,    tdnn_scale_7,    sizeof(tdnn_scale_7));
}

void asr_prefetch_init(void)
{
    memset(asr_offset,0,12);
    asr_prefetch_init_do();
    asr_prefetch_kisck = 5;
}

#endif
