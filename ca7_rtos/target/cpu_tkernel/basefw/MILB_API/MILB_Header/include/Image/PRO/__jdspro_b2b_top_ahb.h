/**
 * @file		__jdspro_b2b_top_ahb.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_B2B_TOP_AHB_H_
#define _IO_PRO_B2B_TOP_AHB_H_

#include "__jdspro_ahb_common.h"

/*  structure of FSHDCTBL0  (2948_0000h)    */
/*  structure of FSHDCTBL1  (2948_0800h)    */
union io_b2b_fshdctbl{
    unsigned short      hword[1024];
    struct {
        unsigned short  FSHDCTBL0   :16;
    }bit[1024];
};

/* Define i/o mapping */
struct io_b2b_ahb{
    /* JDSIMG */
    union  io_b2b_fshdctbl     FSHDCTBL[2];  /* 2948_(0000 - 07FFh) */
                                             /* 2948_(0800 - 0FFFh) */

    union  io_p2mkntbl         P2M0KNTBL;    /* 2948_(1000 - 1201h) */

    unsigned char dmy_1202_13FF[0x1400-0x1202]; /* 2948_(1202 - 13FFh) */

    union  io_p2mkntbl         P2M1KNTBL;    /* 2948_(1400 - 1601h) */

    unsigned char dmy_1602_17FF[0x1800-0x1602]; /* 2948_(1602 - 17FFh) */

    union  io_m2pdkntbl        M2P0DKNTBL;   /* 2948_(1800 - 1A01h) */

    unsigned char dmy_1A02_1BFF[0x1C00-0x1A02]; /* 2948_(1A02 - 1BFFh) */

    union  io_m2pdkntbl        M2P1DKNTBL;   /* 2948_(1C00 - 1E01h) */

    unsigned char dmy_1E02_1FFF[0x2000-0x1E02]; /* 2948_(1E02 - 1FFFh) */

};

#endif	// _IO_PRO_B2B_TOP_AHB_H_
