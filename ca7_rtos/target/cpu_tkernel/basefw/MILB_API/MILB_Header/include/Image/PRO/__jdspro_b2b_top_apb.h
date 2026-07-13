/**
 * @file		__jdspro_b2b_top_apb.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_B2B_TOP_APB_H_
#define _IO_PRO_B2B_TOP_APB_H_

#include "__jdspro_b2b_top.h"
#include "__jdspro_pg.h"
#include "__jdspro_pgain.h"
#include "__jdspro_pzsft.h"
#include "__jdspro_elf.h"
#include "__jdspro_fshd.h"
#include "__jdspro_p2m.h"
#include "__jdspro_pwch.h"
#include "__jdspro_m2p.h"
#include "__jdspro_prch.h"

/* Define i/o mapping */
struct io_b2b_top_apb{
    /* JDSIMG */
    struct io_b2btop                B2BTOP;     /* 2841_(0000 - 00FFh) */
    struct io_pg                    PG;         /* 2841_(0100 - 01FFh) */
    struct io_pgain                 PGAIN1;     /* 2841_(0200 - 02FFh) */
    struct io_pzsft                 PZSFTA1;    /* 2841_(0300 - 03FFh) */
    struct io_elf                   ELF;        /* 2841_(0400 - 07FFh) */
    struct io_pzsft                 PZSFTA2;    /* 2841_(0800 - 08FFh) */
    struct io_pzsft                 PZSFTA2A;   /* 2841_(0900 - 09FFh) */
    struct io_fshd                  FSHD;       /* 2841_(0A00 - 0AFFh) */
    struct io_pgain                 PGAIN2;     /* 2841_(0B00 - 0BFFh) */
    struct io_p2m                   P2M[2];     /* 2841_(0C00 - 0CFFh) */
                                                /* 2841_(0D00 - 0DFFh) */
    struct io_pwch                  PWCH[2];    /* 2841_(0E00 - 0EFFh) */
                                                /* 2841_(0F00 - 0FFFh) */
    struct io_m2p                   M2P[4];     /* 2841_(1000 - 10FFh) */
                                                /* 2841_(1100 - 11FFh) */
                                                /* 2841_(1200 - 12FFh) */
                                                /* 2841_(1300 - 13FFh) */
    struct io_prch                  PRCH[5];    /* 2841_(1400 - 14FFh) */
                                                /* 2841_(1500 - 15FFh) */
                                                /* 2841_(1600 - 16FFh) */
                                                /* 2841_(1700 - 17FFh) */
                                                /* 2841_(1800 - 18FFh) */

    unsigned char dmy_1900_1FFF[0x2000-0x1900]; /* 2841_(1900 - 1FFFh) */

};

#endif	// _IO_PRO_B2B_TOP_APB_H_
