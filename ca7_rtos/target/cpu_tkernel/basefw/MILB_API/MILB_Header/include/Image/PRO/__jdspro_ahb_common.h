/**
 * @file		__jdspro_ahb_common.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_AHB_COMMON_H_
#define _IO_PRO_AHB_COMMON_H_

union io_p2mkntbl{
    unsigned short      hword[257];
    struct {
        unsigned short  P2MKNTBL   :10;
        unsigned short             :6;
    }bit[257];
};

union io_m2pdkntbl{
    unsigned short      hword[257];
    struct {
        unsigned short  M2PDKNTBL  :12;
        unsigned short             :4;
    }bit[257];
};

#endif	// _IO_PRO_SRO_TOP_AHB_H_

