/**
 * @file        _jdspro.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_H_
#define _IO_PRO_H_

#include "__jdspro_sen_top_apb.h"
#include "__jdspro_sro_top_apb.h"
#include "__jdspro_b2b_top_apb.h"
#include "__jdspro_stat_top_apb.h"
#include "__jdspro_pas_top_apb.h"
#include "__jdspro_tbl.h"

struct io_pro_pipe{
    unsigned char dmy_0x08000[0x8000];                          /* 28x0_0000 - 28x0_7FFF */

    struct io_sro_top_apb       SRO;                            /* 28x0_(8000 - FFFFh) */
    struct io_b2b_top_apb       B2B;                            /* 28x1_(0000 - 1FFFh) */

    unsigned char dmy_0xEE000[0xEE000];                         /* 28x1_2000 - 28xF_FFFF */
};

/* Define i/o mapping */
struct io_pro{
    /* JDSIMG */
    // IMG_APB0
    struct io_sen_top_apb       SEN;                            /* 2800_(8000 - FFFFh) */

    unsigned char dmy_28010000_280FFFFF[0x28100000-0x28010000]; /* 2801_0000 - 280F_FFFF */

    unsigned char dmy_28100000_283FFFFF[0x28400000-0x28100000]; /* 2810_0000 - 283F_FFFF */

    // IMG_APB1(PIPE[1] / PIPE[2] / PIPE[3])
    struct io_pro_pipe          IMG_PIPE[3];                    /* 2840_0000 - 286F_FFFF */

    unsigned char dmy_28700000_287FFFFF[0x28800000-0x28700000]; /* 2870_0000 - 287F_FFFF */

    // IMG_APB3
    unsigned char dmy_28800000_2885FFFF[0x28860000-0x28800000]; /* 2880_0000 - 2885_FFFF */

    struct io_stat_top_apb      STAT;                           /* 2886_(0000 - 7FFFh) */
    struct io_pas_top_apb       PAS;                            /* 2886_(8000 - 8FFFh) */
};

extern volatile struct io_pro       IO_PRO;

#endif  // _IO_PRO_H_
