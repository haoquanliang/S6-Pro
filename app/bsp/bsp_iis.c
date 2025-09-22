#include "include.h"

#if IIS_EN
#define IIS_TEST_DEMO           1
#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

iis_cfg_t *iis_libcfg;

///计算IIS模块时钟分频比的方法
//计算方法：mclk = 参考时钟/(mdiv+1)  //这里的参考时钟即PLL1DIV
//          bclk = mclk/(bdiv+1)(不能1分频)
//          lrclk = 采样率 = bclk/(位数*2)

//fs实际上是指 MCLK/LRC  // MCLK = fs * LRC
//IISBAUD[6:0]   mclk_div
//IISBAUD[11:7]  bclk_div

//IIS时钟源默认和DAC是一样的，更改IIS采样率(IIS_LRC)需要参考当前DAC的采样率DAC_OUT_SPR (设置为44.1K或48K)
//16 BIT时钟分频配置
//LRC为44.1K时:
//16bit: BCLK = 44.1K*32 = 1.4112M
//64fs:  44.1K * 32 *2 (bdiv) = 2.8224M   = MCLK //(MCLK * mdiv_8) = 22.5792M
//128fs: 44.1K * 32 *4(bdiv)  = 5.644.8M  = MCLK //(MCLK * mdiv_4) = 22.5792M
//256fs: 44.1K * 32 *8 (bdiv) = 11. 2896M = MCLK //(MCLK * mdiv_2) = 22.5792M
//LRC为48K时:
//16bit:   BCLK = 48K*32 = 1.536M
//64fs:  48K * 32 *2 (bdiv) = 3.072M  =  MCLK //(MCLK * mdiv_8) = 24.576M
//128fs: 48K * 32 *4 (bdiv) = 6.144M  =  MCLK //(MCLK * mdiv_4) = 24.576M
//256fs: 48K * 32 *8 (bdiv) = 12.288M =  MCLK //(MCLK * mdiv_2) = 24.576M
u8 i2s_16bit_clk_div[3][2] = {
  //bclkdiv  ////mclk_div
    {2-1, 8-1}, //64fs
    {4-1, 4-1}, //128fs
    {8-1, 6-1}, //16k, no 256fs for 48k,
};
//32 BIT时钟分频配置
//LRC为44.1K时:
//32bit: BCLK = 44.1K*32 = 2.8224M
//64fs:  44.1K * 64 *8 (bdiv) =  22.5792M = MCLK  //(MCLK * mdiv_1) = 22.5792M
//128fs: 44.1K * 64 *2 (bdiv) = 5.644.8M  = MCLK  //(MCLK * mdiv_4) = 22.5792M
//256fs: 44.1K * 64 *4 (bdiv) = 11.289.6M = MCLK  //(MCLK * mdiv_2) = 22.5792M
//LRC为48K时:
//32bit:  BCLK = 48K * 64 = 3.072M
//64fs:  48K * 64 *8 (bdiv) = 24.576M = MCLK //(MCLK * mdiv_1) = 24.576M
//128fs: 48K * 64 *2 (bdiv) = 6.144M  = MCLK //(MCLK * mdiv_1) = 24.576M
//256fs: 48K * 64 *4 (bdiv) = 12.288M = MCLK //(MCLK * mdiv_1) = 24.576M
u8 i2s_32bit_clk_div[3][2] = {
 //bclkdiv //mclk_div
    {8-1,1-1}, //64fs
    {2-1,4-1}, //128fs
    {4-1,2-1}, //256fs
};
///--------------------------------------------------------

#if TRACE_EN
AT(.com_text.iis_cst)
const char str_iistx_info[] = "IISTX: master = %d, samples = %d, isrcnt = %d (SR_%d)\n";

const IIS_MODE_INFO_TBL iis_mode_info_tbl[10] = {
    {IIS_MASTER_SRCTX,         "IIS_MASTER_SRCTX",},
    {IIS_MASTER_RAMTX,         "IIS_MASTER_RAMTX",},
    {IIS_MASTER_RAMTX_RAMRX,   "IIS_MASTER_RAMTX_RAMRX",},
    {IIS_MASTER_SRCTX_RAMRX,   "IIS_MASTER_SRCTX_RAMRX",},
    {IIS_MASTER_RAMRX,         "IIS_MASTER_RAMRX",},
    {IIS_MASTER_RAMRX_ONEWIRE, "IIS_MASTER_RAMRX_ONEWIRE"},
    {IIS_SLAVE_RAMRX,          "IIS_SLAVE_RAMRX",},
    {IIS_SLAVE_RAMRX_ONEWIRE,  "IIS_SLAVE_RAMRX_ONEWIRE",},
    {IIS_SLAVE_RAMTX,          "IIS_SLAVE_RAMTX",},
    {IIS_SLAVE_RAMTX_RAMRX,    "IIS_SLAVE_RAMTX_RAMRX",},
};

void iis_mode_info_print(void)
{
    for (int i = 0; i < 10; i++) {
        if (iis_mode_info_tbl[i].iis_mode == iis_libcfg->mode) {
            TRACE("iis_mode[0x%X] = %s\n", iis_libcfg->mode, iis_mode_info_tbl[i].iis_str_info);
            break;
        }
    }
}

void iis_sfr_info_dump(void)
{
    TRACE("\ndump iis sfr info:\n");
    TRACE("IISCON0     = 0x%X\n",IISCON0);
    TRACE("IISBAUD     = 0x%X, IISDMACNT   = 0x%X_%d\n", IISBAUD, IISDMACNT, IISDMACNT);
    TRACE("IISDMAOADR0 = 0x%X, IISDMAOADR1 = 0x%X\n", IISDMAOADR0, IISDMAOADR1);
    TRACE("IISDMAIADR0 = 0x%X, IISDMAIADR1 = 0x%X\n", IISDMAIADR0, IISDMAIADR1);
    TRACE("DACDIGCON   = 0x%X\n", DACDIGCON0);
    if(iis_libcfg) {
        TRACE("iis_dma_buf : 0x%X\n", iis_libcfg->dma_cfg.dmabuf_ptr);
    }
}
#endif

u8 iis_io_init(TYPE_IIS_IO io_map, TYPE_IIS_MODE iis_mode, TYPE_MCLK_OUT_SEL mclk_out_en)
{
    TRACE("%s: ",__func__);
    FUNCMCON2 = 0x0F;
    if (IIS_G1 == io_map) {
        TRACE("IIS_G1, MCLK_PA4, BCLK_PA5, LRC_PA6, DO_PA7, DI_PB3\n");
        FUNCMCON2 |= 0x01;
        if (iis_mode & IISCFG_MASTER) {  //MASTER,BCLK,LRC,MCLK out
            TRACE("MASTER ");
            GPIOAFEN |= (BIT(5) | BIT(6));
            GPIOADE  |= (BIT(5) | BIT(6));
            GPIOADIR &= ~(BIT(5) | BIT(6));
            if (mclk_out_en) {
                TRACE("MCLK_OUT_EN ");
                GPIOAFEN |= BIT(4);
                GPIOADE  |= BIT(4);
                GPIOADIR &= ~ BIT(4);
            } else {
                TRACE("MCLK_OUT_DIS ");
            }
        } else {                         //SLAVE: BCLK,LRC  in
            TRACE("SLAVE ");
            GPIOAFEN |= (BIT(5) | BIT(6));
            GPIOADE  |= (BIT(5) | BIT(6));
            GPIOADIR |= (BIT(5) | BIT(6));
            GPIOAPU  |= (BIT(5) | BIT(6));
        }
        if (iis_mode & IISCFG_TX) {      //DO out
            TRACE("TX_DO_OUT ");
            GPIOAFEN |= BIT(7);
            GPIOADE  |= BIT(7);
            GPIOADIR &= ~BIT(7);
        }
        if (iis_mode & IISCFG_RX) {      //DI in
            TRACE("RX_DI_IN ");
            if (iis_mode & IISCFG_ONEWIRE)//单线模式
            {
                TRACE("iis one wire io");
                GPIOAFEN |= BIT(7);
                GPIOADE  |= BIT(7);
                GPIOADIR |= BIT(7);
                GPIOAPU  |= BIT(7);
            } else {
                GPIOBFEN |= BIT(3);
                GPIOBDE  |= BIT(3);
                GPIOBDIR |= BIT(3);
                GPIOBPU  |= BIT(3);
            }
        }
    } else if (IIS_G2 == io_map) {
        TRACE("IIS_G2, MCLK_PE4, BCLK_PE5, LRC_PE6, DO_PE7, DI_PE0\n");
        FUNCMCON2 |= 0x02;
        if (iis_mode & IISCFG_MASTER) {   //MASTER,BCLK,LRC,MCLK out
            TRACE("MASTER ");
            GPIOEFEN |= (BIT(5) | BIT(6));
            GPIOEDE  |= (BIT(5) | BIT(6));
            GPIOEDIR &= ~(BIT(5) | BIT(6));
            if (mclk_out_en) {
                TRACE("MCLK_OUT_EN ");
                GPIOEFEN |=  BIT(4);
                GPIOEDE  |=  BIT(4);
                GPIOEDIR &= ~BIT(4);
            } else {
                TRACE("MCLK_OUT_DIS ");
            }
        } else {                          //SLAVE: BCLK,LRC  in
            TRACE("SLAVE ");
            GPIOEFEN |= (BIT(5) | BIT(6));
            GPIOEDE  |= (BIT(5) | BIT(6));
            GPIOEDIR |= (BIT(5) | BIT(6));
            GPIOEPU  |= (BIT(5) | BIT(6));
        }
        if (iis_mode & IISCFG_TX) {       //DO out
            TRACE("TX_DO_OUT ");
            GPIOEFEN |= BIT(7);
            GPIOEDE  |= BIT(7);
            GPIOEDIR &= ~BIT(7);
        }
        if (iis_mode & IISCFG_RX) {       //DI in
            TRACE("RX_DI_IN ");
            if (iis_mode & IISCFG_ONEWIRE)//单线模式
            {
                TRACE("iis one wire io");
                GPIOEFEN |= BIT(7);
                GPIOEDE  |= BIT(7);
                GPIOEDIR |= BIT(7);
                GPIOEPU  |= BIT(7);
            } else {
                GPIOEFEN |= BIT(0);
                GPIOEDE  |= BIT(0);
                GPIOEDIR |= BIT(0);
                GPIOEPU  |= BIT(0);
            }
        }
    } else if (IIS_G3 == io_map) {
        TRACE("IIS_G3, MCLK_PB4, BCLK_PB0, LRC_PB1, DO_PB2, DI_PB5\n");
        FUNCMCON2 |= 0x03;
        if (iis_mode & IISCFG_MASTER) {  //MASTER,BCLK,LRC,MCLK out
            TRACE("MASTER ");
            GPIOBFEN |= (BIT(0) | BIT(1));
            GPIOBDE  |= (BIT(0) | BIT(1));
            GPIOBDIR &= ~(BIT(0) | BIT(1));
            if (mclk_out_en) {
                TRACE("MCLK_OUT_EN ");
                GPIOBFEN |=  BIT(4);
                GPIOBDE  |=  BIT(4);
                GPIOBDIR &= ~BIT(4);
            } else {
                TRACE("MCLK_OUT_DIS ");
            }
        } else {                         //SLAVE: BCLK,LRC  in
            TRACE("SLAVE ");
            GPIOBFEN |= (BIT(0) | BIT(1));
            GPIOBDE  |= (BIT(0) | BIT(1));
            GPIOBDIR |= (BIT(0) | BIT(1));
            GPIOBPU  |= (BIT(0) | BIT(1));
        }
        if (iis_mode & IISCFG_TX) {       //DO out
            TRACE("TX_DO_OUT ");
            GPIOBFEN |= BIT(2);
            GPIOBDE  |= BIT(2);
            GPIOBDIR &= ~BIT(2);
        }
        if (iis_mode & IISCFG_RX) {        //DI in
            TRACE("RX_DI_IN ");
            if (iis_mode & IISCFG_ONEWIRE) //单线模式
            {
                TRACE("iis one wire io");
                GPIOBFEN |= BIT(2);
                GPIOBDE  |= BIT(2);
                GPIOBDIR |= BIT(2);
                GPIOBPU  |= BIT(2);
            } else {
                GPIOBFEN |= BIT(5);
                GPIOBDE  |= BIT(5);
                GPIOBDIR |= BIT(5);
                GPIOBPU  |= BIT(5);
            }
        }
    }
    TRACE("\n");
    return 0;
}

AT(.com_text.iis_code)
u32 iis_tx_dma_addr_inc(void)   //得到可用地址后,自增  //buf结构: TX_RX同时存在时前一半是TX,后一版半是RX, 如果只有TX或RX,则全部用于TX或RX
{
    u32 buf_idx = iis_libcfg->dma_cfg.txbuf_idx;
    u8* buf_start = iis_libcfg->dma_cfg.txbuf_start_addr;
    u32 addr = (u32)(buf_start+buf_idx);
    if (buf_idx) {
        iis_libcfg->dma_cfg.txbuf_idx = 0;
    } else {
        iis_libcfg->dma_cfg.txbuf_idx = IISDMACNT*4;
    }
    return addr;
}

AT(.com_text.iis_code)
u32 iis_rx_dma_addr_inc(void)    //得到可用地址后,自增  //buf结构: TX_RX同时存在时前一半是TX,后一版半是RX, 如果只有TX或RX,则全部用于TX或RX
{
    u32 buf_idx = iis_libcfg->dma_cfg.rxbuf_idx;
    u8* buf_start = iis_libcfg->dma_cfg.rxbuf_start_addr;
    u32 addr = (u32)(buf_start+buf_idx);
    if (buf_idx) {
        iis_libcfg->dma_cfg.rxbuf_idx = 0;
    } else {
        iis_libcfg->dma_cfg.rxbuf_idx = IISDMACNT*4;
    }
    return addr;
}

//iis_clk_ch:   2_adda_clk
//iis_clk_div:  iis_clk_ch 配置为3时可以选分频系数
void iis_clk_set(u32 iis_clk_ch, u32 iis_clk_div)
{
    CLKCON1 = (CLKCON1 & ~(0x03<<28)) | (iis_clk_ch << 28);          //select addc_clk
    CLKDIVCON0 = (CLKDIVCON0 & ~(0x0F<<28)) | (iis_clk_div << 28);   //iis_div 1
    CLKGAT0 |= BIT(10);
}

void iis_clk_div_set(u16 spr_value, u16 mclk_fs, u8 bitmode)
{
    u8 mclk_div = 0, bclk_div = 0, bits_in_sample =(bitmode?64:32);
    u32 interface_clk = 24576000;
    if (spr_value % 1000) {                         //adpll输出44.1k
        adpll_spr_set(0);
    } else {                                        //adpll输出48k
        adpll_spr_set(1);
    }
    if (spr_value == IIS_SPR_44100) {
        spr_value = IIS_SPR_48000;                  //44.1k按照48k计算
    }

    mclk_div = interface_clk / (spr_value*mclk_fs);
    bclk_div = mclk_fs / bits_in_sample;

    if (bclk_div == 1) {
        printf("\nERROR:FS in the current bitmode is supported\n\n");
        WDT_RST();
    }
    TRACE("mclk_div:%d, bclk_div:%d\n",mclk_div,bclk_div);
    IISBAUD = (((bclk_div-1)&0x1f)<<7 | ((mclk_div-1)&0x3f));
}

void iis_cfg_init(iis_cfg_t *cfg)
{
    TRACE("%s\n", __func__);
    u32 iisconsfr = 0;
    iis_libcfg = cfg;
    iis_io_init(iis_libcfg->iomap,iis_libcfg->mode,iis_libcfg->mclk_out_en);
    iis_clk_set(2, 1);     //i2s clk sel dac_clk
    SETB(IISCON0, 16);     //clear tx pending
    SETB(IISCON0, 17);     //clear rx pending
    IISCON0 = 0;
    if (iis_libcfg->mode & IISCFG_DMA) {
        iis_irq_init();
        if (iis_libcfg->bit_mode == IIS_32BIT) {
            IISDMACNT = iis_libcfg->dma_cfg.samples * 2;    //32bit模式时一个samples对应64bit
        } else if (iis_libcfg->bit_mode == IIS_16BIT) {
            IISDMACNT = iis_libcfg->dma_cfg.samples;
        }
        if (((iis_libcfg->mode & IISCFG_TXMASK) == IISCFG_RAMTX) && ((iis_libcfg->mode & IISCFG_RXMASK) == IISCFG_RAMRX)) {
            u32 required_len = IISDMACNT*4*2*2;
            if (required_len > iis_libcfg->dma_cfg.dmabuf_len) {
                printf("\nERROR:dmabuf_len is no enough\n\n");
                WDT_RST();
            }
            iis_libcfg->dma_cfg.txbuf_start_addr = iis_libcfg->dma_cfg.dmabuf_ptr;
            iis_libcfg->dma_cfg.rxbuf_start_addr = iis_libcfg->dma_cfg.dmabuf_ptr + IISDMACNT*4*2;
            iis_libcfg->dma_cfg.txbuf_idx = 0;
            iis_libcfg->dma_cfg.rxbuf_idx = 0;
            IISDMAOADR0 = iis_tx_dma_addr_inc();
            IISDMAOADR1 = iis_tx_dma_addr_inc();
            IISDMAIADR0 = iis_rx_dma_addr_inc();
            IISDMAIADR1 = iis_rx_dma_addr_inc();
        } else if (((iis_libcfg->mode & IISCFG_TXMASK) == IISCFG_RAMTX) && ((iis_libcfg->mode & IISCFG_RXMASK) != IISCFG_RAMRX)) {
            u32 required_len = IISDMACNT*4*2;
            if (required_len > iis_libcfg->dma_cfg.dmabuf_len) {
                printf("\nERROR:dmabuf_len is no enough\n\n");
                WDT_RST();
            }
            iis_libcfg->dma_cfg.txbuf_start_addr = iis_libcfg->dma_cfg.dmabuf_ptr;
            iis_libcfg->dma_cfg.txbuf_idx = 0;
            IISDMAOADR0 = iis_tx_dma_addr_inc();
            IISDMAOADR1 = iis_tx_dma_addr_inc();
        } else if (((iis_libcfg->mode & IISCFG_TXMASK) != IISCFG_RAMTX) && ((iis_libcfg->mode & IISCFG_RXMASK) == IISCFG_RAMRX)){
            u32 required_len = IISDMACNT*4*2;
            if (required_len > iis_libcfg->dma_cfg.dmabuf_len) {
                printf("\nERROR:dmabuf_len is no enough\n\n");
                WDT_RST();
            }
            iis_libcfg->dma_cfg.rxbuf_start_addr = iis_libcfg->dma_cfg.dmabuf_ptr;
            iis_libcfg->dma_cfg.rxbuf_idx = 0;
            IISDMAIADR0 = iis_rx_dma_addr_inc();
            IISDMAIADR1 = iis_rx_dma_addr_inc();
            if(iis_libcfg->mode & IISCFG_ONEWIRE){ //单线
                TRACE("iis one wire\n");
                SETB(iisconsfr, 12);
                SETB(iisconsfr, 13);
            }else{
                TRACE("iis two wire\n");
                CLRB(iisconsfr, 12);
                CLRB(iisconsfr, 13);
            }
        }
    }
    iis_clk_div_set(iis_libcfg->spr_sel, iis_libcfg->mclk_sel, iis_libcfg->bit_mode);

    if (IIS_16BIT == iis_libcfg->bit_mode) {
        CLRB(iisconsfr, 2);     //0: iis bit mode (0:16bit) at master function
    } else if(IIS_32BIT == iis_libcfg->bit_mode) {
        SETB(iisconsfr, 2);     //1: iis bit mode (1:32bit) at master function
    }
    if (IIS_DATA_LEFT_JUSTIFIED == iis_libcfg->data_mode) {
        CLRB(iisconsfr, 3);     //0: left-justified mode (data delay 0 clock after WS change)
    } else if (IIS_DATA_NORMAL == iis_libcfg->data_mode) {
        SETB(iisconsfr, 3);     //1: IIS normal mode  (data delay 1 clock after WS change)
    }

    SETB(iisconsfr, 10);         //dma out requet mask delay eanble (system very fast,need set this)
    if (iis_libcfg->mode & IISCFG_MASTER) {
        CLRB(iisconsfr, 1);      //0 iis is master mode
    } else {
        SETB(iisconsfr, 1);      //1 iis is slave mode
    }

    if (iis_libcfg->mclk_out_en) {
        SETB(iisconsfr, 9);
    }

    if (iis_libcfg->mode & IISCFG_DMA) {
        if ((iis_libcfg->mode & IISCFG_TXMASK) == IISCFG_RAMTX) {
            TRACE("iis ram tx int en\n");
            SETB(iisconsfr, 5);      //iis DMA output enable
            SETB(iisconsfr, 7);      //dma output interrupt enable
            SETB(iisconsfr, 4);      //data OUT source select: RAM
        }

        if ((iis_libcfg->mode & IISCFG_RXMASK) == IISCFG_RAMRX) {
            TRACE("iis ram rx int en\n");
            SETB(iisconsfr, 6);      //iis DMA input enable
            SETB(iisconsfr, 8);      //dma input interrupt enable
        }

        if ((iis_libcfg->mode & IISCFG_TXRXMASK) == IISCFG_RAMRX) { //只有RAMRX 需要把这位置起来才会KICK起来,同时有打开SRCTX时则可以不用设置它
            TRACE("iis only ram rx,bit4 set\n");
            SETB(iisconsfr, 4);
        }
    }
    CLRB(iisconsfr, 0);      //IIS EN,先屏蔽，由iis_start在需要时才打开

    if (iis_libcfg->mode & IISCFG_SRC) {
        TRACE("iis src out en\n");
        DACDIGCON0 |= BIT(23);
    } else {
        DACDIGCON0 &= ~BIT(23);
    }
    IISCON0 = iisconsfr;     //config iis sfor
#if TRACE_EN
    iis_mode_info_print();
    iis_sfr_info_dump();
#endif
}

void iis_start(void)
{
    TRACE("-->%s\n",__func__);
    dac_fade_in();
    SETB(IISCON0, 0);
}

void iis_stop(void)
{
    TRACE("-->%s\n",__func__);
    dac_fade_out();
    CLRB(IISCON0, 0);
}

AT(.com_text.iis_code)
u8 iis_mode_cfg_get(void)
{
    if (iis_libcfg) {
        return iis_libcfg->mode;
    } else {
        return 0;
    }
}

AT(.com_text.iis_code)
void iis_isr_func(void)
{
    u32 cache_addr;
    u32 iiscon0 = IISCON0 & (~(BIT(16)|BIT(17)));
    if (IISCON0 & BIT(16)) {        //TX ISR
        IISCON0 = iiscon0 | BIT(16);
        cache_addr = iis_tx_dma_addr_inc();
        IISDMAOADR1 = cache_addr;
        if (iis_libcfg->dma_cfg.iis_isr_tx_callbck) {
            iis_libcfg->dma_cfg.iis_isr_tx_callbck((void*)cache_addr,iis_libcfg->dma_cfg.samples,IISCON0 & BIT(2));
        }
    }

    if (IISCON0 & BIT(17)) {        //RX ISR
        IISCON0 = iiscon0 | BIT(17);
        cache_addr = iis_rx_dma_addr_inc();
        IISDMAIADR1 = cache_addr;
        if (iis_libcfg->dma_cfg.iis_isr_rx_callbck) {
            iis_libcfg->dma_cfg.iis_isr_rx_callbck((void*)cache_addr,iis_libcfg->dma_cfg.samples,IISCON0 & BIT(2));
        }
    }

}

void iis_irq_init(void)
{
    TRACE("%s\n", __func__);
    register_isr(IRQ_I2S_VECTOR, iis_isr_func);
    PICPR &= ~BIT(IRQ_I2S_VECTOR);
	PICEN |= BIT(IRQ_I2S_VECTOR);
}

#if IIS_TEST_DEMO
#define IIS_DMA_SAMPLES    32
#define IIS_DMABUF_LEN     (IIS_DMA_SAMPLES * 8 * 2 * 2)     //(samples:240)*(sample_len:8)*(dmabuf_switching:2)*(txrx:2)
iis_cfg_t iis_cfg;
u8 iis_dmabuf[IIS_DMABUF_LEN] AT(.iis_dmabuf);           //若iis_cfg.mode中有RAMTX或RAMRX,需要该dmabuf做中断缓存
AT(.com_text.iis_code)
void aubuf_adjust(void)
{
    u16 au_size = (u16)AUBUF0SIZE >> 2;  //1/4 AUBUFSIZE
    u16 aubuf_fifo_cnt = AUBUF0FIFOCNT & 0xffff;
    if(CKB0(DACDIGCON0,6)) {             //phasecomp sync enable
        SETB(DACDIGCON0,6);
    }
    if (aubuf_fifo_cnt <= au_size) {
        PHASECOMP = 0xFFFE;              //低16位有符号数调节SRC0 FIFO SPEED
    } else if (aubuf_fifo_cnt >= (au_size*3)) {
        PHASECOMP = 0x0001;
    } else {
        PHASECOMP = 0;
    }
}

AT(.com_text.iis_code)
void iis_tx_process_test(void *buf, u32 samples, bool iis_32bit)
{
    static u8 cnt = 0;
    u32 *ptr32 = (u32*)buf;
    u16 *ptr16 = (u16*)buf;

    for (int i = 0; i< samples;i++) {
        if (iis_32bit) {
            ptr32[2*i]   = cnt;     //16->32位扩展
            ptr32[2*i+1] = cnt;
        } else {
            ptr16[2*i]   = cnt;     //16->32位扩展
            ptr16[2*i+1] = cnt;
        }
    }
    cnt++;
#if TRACE_EN
    static u32 ticks = 0;
    static u32 isr_cnt = 0;
    isr_cnt++;
    if (tick_check_expire(ticks,1000)) {
        printk(str_iistx_info,IS_IIS_MASTER(),samples, isr_cnt, samples*isr_cnt);
        isr_cnt = 0;
        ticks = tick_get();
    }
#endif
}

AT(.com_text.iis_code)
void iis_rx_process_test(void *buf, u32 samples, bool iis_32bit)
{
    u32 *ptr32 = (u32*)buf;
    u16 *ptr16 = (u16*)buf;

    if (!(iis_mode_cfg_get() & IISCFG_MASTER)) {  //slave rx需要进行调速
        aubuf_adjust();
    }
    for (int i = 0; i < samples; i++) {
        if (iis_32bit) {
            dac_put_sample_24bit(ptr32[2*i]>>8, ptr32[2*i+1]>>8);
        } else {
            dac_put_sample_16bit(ptr16[2*i], ptr16[2*i+1]);
        }
    }
}

void bsp_iis_test_init(void)
{
    printf("%s\n",__func__);
    memset(&iis_cfg, 0x00, sizeof(iis_cfg));
    iis_cfg.mode        = IIS_MASTER_RAMTX_RAMRX;
    iis_cfg.iomap       = IIS_G3;
    iis_cfg.bit_mode    = IIS_32BIT;
    iis_cfg.data_mode   = IIS_DATA_NORMAL;
    iis_cfg.spr_sel     = IIS_SPR_48000;
    iis_cfg.mclk_sel    = IIS_MCLK_128FS;
    iis_cfg.mclk_out_en = IIS_MCLK_OUT_DIS;

    if (iis_cfg.mode & IISCFG_DMA) {
        printf("iis_dma config run\n");
        iis_cfg.dma_cfg.samples = IIS_DMA_SAMPLES;
        iis_cfg.dma_cfg.dmabuf_ptr = iis_dmabuf;
        iis_cfg.dma_cfg.dmabuf_len = IIS_DMABUF_LEN;
        iis_cfg.dma_cfg.iis_isr_rx_callbck = iis_rx_process_test;  //iis_rx接收完一个DMA后起中断,回调该函数,可以从buf中取出接收到数据
        iis_cfg.dma_cfg.iis_isr_tx_callbck = iis_tx_process_test;  //iis_tx发送完一个DMA后起中断,要求向buf中填入数据,以备下一次发送
    }

    dac_aubuf_init();
    dac_spr_set(SPR_48000);           //设置采样率
    dac_fade_in();                    //dac淡入
    dac_vol_set(0x3000);              //首次进入i2s,dac的默认音量, 可填0~0x7fff
    dac_put_zero(256);                //初始化256个样点

    iis_cfg_init(&iis_cfg);           //初始化iis配置
    iis_start();
}
#endif

#if IIS_RX2SCO_EN
#define IIS_SCO_DMA_SAMPLES    240
#define IIS_SCO_DMABUF_LEN     (IIS_SCO_DMA_SAMPLES * 4 * 2)  //(samples:240)*(sample_len:4)*(dmabuf_switching:2)
iis_cfg_t iis_sco_cfg AT(.bss.iis.sco);
u8 iis_sco_dmabuf[IIS_SCO_DMABUF_LEN] AT(.buf.iis.sco);
struct iis_cvsd_cb_t{
    u8 tmpbuf[240*2];
    u8 wcnt;
} iis_cvsd_cb AT(.buf.iis.sco);

AT(.com_text.iis_code)
void iis_rx_2_sco_process(void *buf, u32 samples, bool iis_32bit)
{
    u8 *output_buf = NULL;
    u16 *ptr16 = (u16*)buf;
    if ((samples != 240)||(iis_32bit ==1)) {    //for only 16bit mode
        return;
    }
    if (!bt_sco_is_msbc() && !bt_sco_dnn_en()) {
        for (int i = 0; i < 120; i++) {
            ptr16[i] = ptr16[4*i];
        }
        memcpy(iis_cvsd_cb.tmpbuf + (iis_cvsd_cb.wcnt++)*120*2, buf, 120*2);
        if (iis_cvsd_cb.wcnt >= 2) {
            iis_cvsd_cb.wcnt = 0;
            output_buf = (u8*)iis_cvsd_cb.tmpbuf;
        }
    } else {
        for (int i = 0; i < 240; i++) {
            ptr16[i] = ptr16[2*i];
        }
        output_buf = (u8*)buf;
    }
    if (output_buf) {
#if BT_AEC_EN || BT_SCO_SMIC_EN || BT_SCO_DMIC_EN
        bt_aec_process(output_buf, 240, 0);
#else
        bt_sco_tx_process(output_buf, 240, 0);
#endif
    }
}

void bsp_iis_sco_init(void)
{
    printf("%s\n",__func__);
    memset(&iis_sco_cfg, 0x00, sizeof(iis_sco_cfg));
    iis_sco_cfg.mode        = IIS_MASTER_RAMRX_ONEWIRE;
    iis_sco_cfg.iomap       = IIS_G3;
    iis_sco_cfg.bit_mode    = IIS_16BIT;
    iis_sco_cfg.data_mode   = IIS_DATA_NORMAL;
    iis_sco_cfg.spr_sel     = IIS_SPR_16000;
    iis_sco_cfg.mclk_sel    = IIS_MCLK_128FS;
    iis_sco_cfg.mclk_out_en = IIS_MCLK_OUT_DIS;

    if (iis_sco_cfg.mode & IISCFG_DMA) {
        iis_sco_cfg.dma_cfg.samples = IIS_SCO_DMA_SAMPLES;
        iis_sco_cfg.dma_cfg.dmabuf_ptr = iis_sco_dmabuf;
        iis_sco_cfg.dma_cfg.dmabuf_len = sizeof(iis_sco_dmabuf);
        iis_sco_cfg.dma_cfg.iis_isr_rx_callbck = iis_rx_2_sco_process;
        iis_sco_cfg.dma_cfg.iis_isr_tx_callbck = NULL;
    }

    iis_cfg_init(&iis_sco_cfg);

    memset(&iis_cvsd_cb, 0x00, sizeof(iis_cvsd_cb));
}
#endif

#endif // I2S_EN
