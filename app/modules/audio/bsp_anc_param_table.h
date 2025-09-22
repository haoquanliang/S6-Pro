#ifndef BSP_ANC_PARAM_TABLE_H
#define BSP_ANC_PARAM_TABLE_H

typedef struct {
    u32 *addr;
    u32 *len;
} anc_param;

typedef struct {
    u32 addr;
    u32 len;
} anc_exparam;

extern const anc_param anc_fffb_eq_tbl[8];
extern const anc_param anc_fffb_drc_tbl[4];
extern const anc_param anc_fffb_drc_eq_tbl[4];
extern const anc_param anc_hybrid_eq_tbl[2][8];
extern const anc_param anc_hybrid_drc_tbl[2][4];
extern const anc_param anc_hybrid_drc_eq_tbl[2][4];

extern const anc_exparam anc_fffb_eq_tbl2[8];
extern const anc_exparam anc_fffb_drc_tbl2[4];
extern const anc_exparam anc_fffb_drc_eq_tbl2[4];
extern const anc_exparam anc_hybrid_eq_tbl2[2][8];
extern const anc_exparam anc_hybrid_drc_tbl2[2][4];
extern const anc_exparam anc_hybrid_drc_eq_tbl2[2][4];

extern const anc_param anc_aem_ff_eq_tbl[18];
extern const anc_param anc_aem_fb_eq_tbl[4];
#endif
