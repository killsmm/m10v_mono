/**
 * @file		__jdspro_pas_top_ahb.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_PAS_TOP_AHB_H_
#define _IO_PRO_PAS_TOP_AHB_H_

#include "__jdspro_ahb_common.h"

/*  structure of SPTTBL0    (2981_4000h)    */
union io_pas_spttbl0{
    unsigned long       word[256];
    struct {
        unsigned long   SPTGD0  :16;
        unsigned long           :16;
    }bit[256];
};

/*  structure of SPTTBL1    (2981_4400h)    */
union io_pas_spttbl1{
    unsigned long       word[256];
    struct {
        unsigned long   SPTGD1  :16;
        unsigned long           :16;
    }bit[256];
};

/* Define i/o mapping */
struct io_pas_ahb{
    /* JDSIMG */
    union  io_pas_spttbl0      SPTTBL0;    /* 2981_(4000 - 43FFh) */
    union  io_pas_spttbl1      SPTTBL1;    /* 2981_(4400 - 47FFh) */
    union  io_p2mkntbl         P2M0KNTBL;  /* 2981_(4800 - 4A01h) */

    unsigned char dmy_4A02_4BFF[0x4C00-0x4A02]; /* 2981_(4A02 - 4BFFh) */

    union  io_m2pdkntbl        M2P0DKNTBL; /* 2981_(4C00 - 4E01h) */

    unsigned char dmy_4E02_4FFF[0x5000-0x4E02]; /* 2981_(4E02 - 4FFFh) */

};

#endif	// _IO_PRO_PAS_TOP_AHB_H_
