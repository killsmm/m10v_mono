/**
 * @file		__jdspro_af_top.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_AF_TOP_H_
#define _IO_PRO_AF_TOP_H_

#include "__jdspro_af_global.h"
#include "__jdspro_af_main.h"
#include "__jdspro_af_vs.h"
#include "__jdspro_af_hs.h"
#include "__jdspro_af_win.h"

/* Define i/o mapping */
struct io_aftop{
    /* JDSIMG */
    struct io_afgbal                AFGBAL;    /* 2886_(2000 - 203Fh) */

    unsigned char dmy_2040_207F[0x2080-0x2040]; /* 2886_(2040 - 207Fh) */

    struct io_afmain                AFMAIN[3]; /* 2886_(2080 - 20FFh) */
											   /* 2886_(2100 - 217Fh) */
											   /* 2886_(2180 - 21FFh) */
    struct io_afvs                  AFVS;      /* 2886_(2200 - 223Fh) */

    unsigned char dmy_2240_227F[0x2280-0x2240]; /* 2886_(2240 - 227Fh) */

    struct io_afhs                  AFHS[3];   /* 2886_(2280 - 22FFh) */
											   /* 2886_(2300 - 237Fh) */
											   /* 2886_(2380 - 23FFh) */
    struct io_afwin                 AFWIN[15]; /* 2886_(2400 - 247Fh) */
											   /* 2886_(2480 - 24FFh) */
											   /* 2886_(2500 - 257Fh) */
											   /* 2886_(2580 - 25FFh) */
											   /* 2886_(2600 - 267Fh) */
											   /* 2886_(2680 - 26FFh) */
											   /* 2886_(2700 - 277Fh) */
											   /* 2886_(2780 - 27FFh) */
											   /* 2886_(2800 - 287Fh) */
											   /* 2886_(2880 - 28FFh) */
											   /* 2886_(2900 - 297Fh) */
											   /* 2886_(2980 - 29FFh) */
											   /* 2886_(2A00 - 2A7Fh) */
											   /* 2886_(2A80 - 2AFFh) */
											   /* 2886_(2B00 - 2B7Fh) */

    unsigned char dmy_2B80_3FFF[0x4000-0x2B80]; /* 2886_(2B80 - 3FFFh) */

};

#endif	// _IO_PRO_AF_TOP_H_
