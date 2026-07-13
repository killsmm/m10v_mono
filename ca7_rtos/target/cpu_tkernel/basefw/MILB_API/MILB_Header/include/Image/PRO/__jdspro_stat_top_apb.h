/**
 * @file		__jdspro_stat_top_apb.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_STAT_TOP_APB_H_
#define _IO_PRO_STAT_TOP_APB_H_

#include "__jdspro_stat_top.h"
#include "__jdspro_af_top.h"
#include "__jdspro_aeawb.h"
#include "__jdspro_flck.h"
#include "__jdspro_hist.h"
#include "__jdspro_pwch.h"

/* Define i/o mapping */
struct io_stat_top_apb{
    /* JDSIMG */
    struct io_stattop               STATTOP;    /* 2886_(0000 - 00FFh) */

    unsigned char dmy_0100_1FFF[0x2000-0x0100]; /* 2886_(0100 - 1FFFh) */

    struct io_aftop                 AF[2];      /* 2886_(2000 - 3FFFh) */
                                                /* 2886_(4000 - 5FFFh) */
    struct io_aeawb                 AEAWB[4];   /* 2886_(6000 - 60FFh) */
                                                /* 2886_(6100 - 61FFh) */
                                                /* 2886_(6200 - 62FFh) */
                                                /* 2886_(6300 - 63FFh) */
    struct io_flck                  FLCK[2];    /* 2886_(6400 - 64FFh) */
                                                /* 2886_(6500 - 65FFh) */
    struct io_hist                  HIST[4];    /* 2886_(6600 - 66FFh) */
                                                /* 2886_(6700 - 67FFh) */
                                                /* 2886_(6800 - 68FFh) */
                                                /* 2886_(6900 - 69FFh) */
    struct io_pwch                  PWCH[6];    /* 2886_(6A00 - 6AFFh) */
                                                /* 2886_(6B00 - 6BFFh) */
                                                /* 2886_(6C00 - 6CFFh) */
                                                /* 2886_(6D00 - 6DFFh) */
                                                /* 2886_(6E00 - 6EFFh) */
                                                /* 2886_(6F00 - 6FFFh) */

    unsigned char dmy_7000_7FFF[0x8000-0x7000]; /* 2886_(7000 - 7FFFh) */

};

#endif	// _IO_PRO_STAT_TOP_APB_H_
