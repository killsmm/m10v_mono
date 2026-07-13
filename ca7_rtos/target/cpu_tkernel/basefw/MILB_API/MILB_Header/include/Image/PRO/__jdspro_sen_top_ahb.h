/**
 * @file		__jdspro_sen_top_ahb.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_SEN_TOP_AHB_H_
#define _IO_PRO_SEN_TOP_AHB_H_

#include "__jdspro_ahb_common.h"

/* Define i/o mapping */
struct io_sen_ahb{
    /* JDSIMG */
    union  io_p2mkntbl    P2M0KNTBL;            /* 2900_(0000 - 0201h) */

    unsigned char dmy_0202_03FF[0x0400-0x0202]; /* 2900_(0202 - 03FFh) */

    union  io_p2mkntbl    P2M1KNTBL;            /* 2900_(0400 - 0601h) */

    unsigned char dmy_0602_07FF[0x0800-0x0602]; /* 2900_(0602 - 07FFh) */

    union  io_p2mkntbl    P2M2KNTBL;            /* 2900_(0800 - 0A01h) */

    unsigned char dmy_0A02_0BFF[0x0C00-0x0A02]; /* 2900_(0A02 - 0BFFh) */

    union  io_p2mkntbl    P2M3KNTBL;            /* 2900_(0C00 - 0E01h) */

    unsigned char dmy_0E02_0FFF[0x1000-0x0E02]; /* 2900_(0E02 - 0FFFh) */

    union  io_p2mkntbl    P2M4KNTBL;            /* 2900_(1000 - 1201h) */

    unsigned char dmy_1202_13FF[0x1400-0x1202]; /* 2900_(1202 - 13FFh) */

    union  io_p2mkntbl    P2M5KNTBL;            /* 2900_(1400 - 1601h) */

    unsigned char dmy_1602_17FF[0x1800-0x1602]; /* 2900_(1602 - 17FFh) */

    union  io_p2mkntbl    P2M6KNTBL;            /* 2900_(1800 - 1A01h) */

    unsigned char dmy_1A02_1BFF[0x1C00-0x1A02]; /* 2900_(1A02 - 1BFFh) */

    union  io_p2mkntbl    P2M7KNTBL;            /* 2900_(1C00 - 1E01h) */

    unsigned char dmy_1E02_3FFF[0x4000-0x1E02]; /* 2900_(1E02 - 3FFFh) */

};

#endif	// _IO_PRO_P2M_H_
