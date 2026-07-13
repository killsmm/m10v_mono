/**
 * @file		_jdsdisp_top.h
 * @brief		Definition JDSDISP3A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "__jdsdisp_lch.h"
#include "__jdsdisp_dcore.h"
#include "__jdsdisp_grch.h"
#include "__jdsdisp_dbg.h"
#include "__jdsdisp_dsi.h"
#include "__jdsdisp_hdmi.h"
#include "__jdspro_p2m.h"
#include "__jdspro_pwch.h"
#include "__f_hdmitx2p0_lp.h"
#include "__f_mipidsi11tx_lp.h"

/* Define i/o mapping */

/* MAIN (Common block 0/1) */
struct io_jdsdisp_main{
    struct io_jdslch_reg    LCH;                        /* 2890_(0000 - 0FFFh) */
    struct io_jdsdcore_reg  DCORE;                      /* 2890_(1000 - 1FFFh) */
    struct io_jdsgrch_reg   GRCH[2];                    /* 2890_(2000 - 3FFFh) */
};

/* DISP TOP */
struct io_jdsdisp_top{
    /* JDSIMG */
    struct io_jdsdisp_main      MAIN[2];                /* 2890_(0000 - 7FFFh) */

    unsigned char dmy_8000_AFFF[0xB000-0x8000];         /* 2890_(8000 - AFFFh) */

    struct io_jdsddbg_reg       DBGCTL;                 /* 2890_(B000 - BFFFh) */
    struct io_f_mipidsi11tx_lp  MIPI_DSI;               /* 2890_(C000 - CFFFh) */
    struct io_jdsdisp_dsi       MDSCTL;                 /* 2890_(D000 - DFFFh) */
    struct io_jddisp_hdmi       JDDISP_HDMI_HDMIC;      /* 2890_(E000 - EFFFh) */

    unsigned char dmy_2890F000_2890FFFF[0x28910000-0x2890F000]; /* 2890_F000 - 2890_FFFFh */

    struct io_p2m               P2M[2];                 /* 2891_(0000 - 01FFh) */
    struct io_pwch              PWCH[2];                /* 2891_(0200 - 03FFh) */

    unsigned char dmy_28910400_2891FFFF[0x28920000-0x28910400]; /* 2891_0400 - 2891_FFFFh */

    struct io_f_hdmitx2p0_lp    HDMI_TX;                /* 2892_0000 - 2893_FFFFh */
};

extern volatile struct io_jdsdisp_top	IO_DISP;

