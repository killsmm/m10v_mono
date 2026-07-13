/**
 * @file		__jdspro_sro_top_ahb.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_SRO_TOP_AHB_H_
#define _IO_PRO_SRO_TOP_AHB_H_

#include "__jdspro_ahb_common.h"

/*  structure of SDC0GD0    (2944_0000h)    */
/*  structure of SDC0GD1    (2944_0100h)    */
/*  structure of SDC1GD0    (2944_0200h)    */
/*  structure of SDC1GD1    (2944_0300h)    */
union io_sro_sdcgd{
    unsigned short      hword[128];
    struct {
        unsigned short  SDCGD :8;
        unsigned short        :8;
    }bit[128];
};

/* Define i/o mapping */
struct io_sro_ahb{
    /* JDSIMG */
    union  io_sro_sdcgd      SDCGD[2][2];       /* 2944_(0000 - 00FFh) */
                                                /* 2944_(0100 - 01FFh) */
                                                /* 2944_(0200 - 02FFh) */
                                                /* 2944_(0300 - 03FFh) */
    union  io_p2mkntbl       P2M0KNTBL;         /* 2944_(0400 - 0601h) */

    unsigned char dmy_0602_07FF[0x0800-0x0602]; /* 2944_(0602 - 07FFh) */

    union  io_p2mkntbl       P2M1KNTBL;         /* 2944_(0800 - 0A01h) */

    unsigned char dmy_0A02_0BFF[0x0C00-0x0A02]; /* 2944_(0A02 - 0BFFh) */

    union  io_p2mkntbl       P2M2KNTBL;         /* 2944_(0C00 - 0E01h) */

    unsigned char dmy_0E02_0FFF[0x1000-0x0E02]; /* 2944_(0E02 - 0FFFh) */

    union  io_p2mkntbl       P2M3KNTBL;         /* 2944_(1000 - 1201h) */

    unsigned char dmy_1202_13FF[0x1400-0x1202]; /* 2944_(1202 - 13FFh) */

    union  io_p2mkntbl       P2M4KNTBL;         /* 2944_(1400 - 1601h) */

    unsigned char dmy_1602_17FF[0x1800-0x1602]; /* 2944_(1602 - 17FFh) */

    union  io_p2mkntbl       P2M5KNTBL;         /* 2944_(1800 - 1A01h) */

    unsigned char dmy_1A02_1BFF[0x1C00-0x1A02]; /* 2944_(1A02 - 1BFFh) */

    union  io_m2pdkntbl      M2P0DKNTBL;        /* 2944_(1C00 - 1E01h) */

    unsigned char dmy_1E02_1FFF[0x2000-0x1E02]; /* 2944_(1E02 - 1FFFh) */

    union  io_m2pdkntbl      M2P1DKNTBL;        /* 2944_(2000 - 2201h) */

    unsigned char dmy_2202_23FF[0x2400-0x2202]; /* 2944_(2202 - 23FFh) */

    union  io_m2pdkntbl      M2P6DKNTBL;        /* 2944_(2400 - 2601h) */

    unsigned char dmy_2602_27FF[0x2800-0x2602]; /* 2944_(2602 - 27FFh) */

    union  io_m2pdkntbl      M2P7DKNTBL;        /* 2944_(2800 - 2A01h) */

    unsigned char dmy_29442A02_2947FFFF[0x29480000-0x29442A02]; /* 2944_2A02 - 2947_FFFFh */

};

#endif	// _IO_PRO_SRO_TOP_AHB_H_
