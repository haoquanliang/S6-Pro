#ifndef _API_PWR_H_
#define _API_PWR_H_

#define WKO_10SRST_EN(x)       { RTCCON10 = BIT(10);\
                                RTCCON1 &= ~BIT(8);\
                                if (x) {RTCCON1 |= BIT(8);}\
                                RTCCON12 = (RTCCON12 & ~(0x03 << 8 | 0x03)) | (1 << 8); }

#define WKO_10SRST_DIS()        RTCCON12 |= 0x03

void pmu_cpu1_core_off(void);
void pmu_cpu1_ram_off(void);
void pmu_cpu1_ram_on(void);
void pmu_cpu1_core_on(void);

void pmu_init(u8 cfg);
void pmu_set_mode(u8 buck_en);
u16 pmu_get_vbg(void);
u16 pmu_get_vddio(void);

#endif // _API_PWR_H_

