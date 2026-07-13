/**
 * @file		__jdspro_pas_top_apb.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_PAS_TOP_APB_H_
#define _IO_PRO_PAS_TOP_APB_H_

#include "__jdspro_pas_top.h"
#include "__jdspro_pg.h"
#include "__jdspro_lnr.h"
#include "__jdspro_pgain.h"
#include "__jdspro_spt.h"

/* Define i/o mapping */
struct io_pas_top_apb{
    /* JDSIMG */
    struct io_pastop                PASTOP;    /* 2886_(8000 - 80FFh) */
    struct io_pg                    PG;        /* 2886_(8100 - 81FFh) */
    struct io_lnr                   LNR;       /* 2886_(8200 - 82FFh) */
    struct io_pgain                 PGAIN;     /* 2886_(8300 - 83FFh) */
    struct io_spt                   SPT;       /* 2886_(8400 - 84FFh) */
    struct io_p2m                   P2M0;      /* 2886_(8500 - 85FFh) */
    struct io_pwch                  PWCH0;     /* 2886_(8600 - 86FFh) */
    struct io_m2p                   M2P0;      /* 2886_(8700 - 87FFh) */
    struct io_prch                  PRCH0;     /* 2886_(8800 - 88FFh) */

    unsigned char dmy_8900_8FFF[0x9000-0x8900]; /* 2886_(8900 - 8FFFh) */

};

#endif	// _IO_PRO_PAS_TOP_APB_H_
