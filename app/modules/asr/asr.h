#ifndef ASR_ASR_H_
#define ASR_ASR_H_

#define     NPU_CONTIN_CAL_EN   1   //npu是否支持一次性执行多条计算指令
#define     NPU_MEMCPY_EN       1   //是否使用npu执行memcpy, NPU_CONTIN_CAL_EN要打开
#define     MAX_NPU_MATRIX      48  //npu一次性最多执行多少条指令

#ifdef __cplusplus
extern "C" {
#endif

#define ASR_PREFETCH_EN     1
#define ASR_LOG_EN          1

int  Wanson_ASR_Init();

void Wanson_ASR_Reset();

int  Wanson_ASR_Recog(short *buf, int buf_len, const char **text, float *score);

void Wanson_ASR_Release();

#ifdef __cplusplus
}
#endif

u32 asr_alg_process(short *ptr);
void asr_alg_start(void);
void asr_alg_init(void);
void asr_alg_stop(void);
#endif
