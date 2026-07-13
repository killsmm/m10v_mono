/**
 * @file        __jdspro_sen_top_apb.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_SEN_TOP_APB_H_
#define _IO_PRO_SEN_TOP_APB_H_

#include "__jdspro_sen_top.h"
#include "__jdspro_sg.h"
#include "__jdspro_slvsec.h"
#include "__jdspro_lvds.h"
#include "__jdspro_mpid.h"
#include "__jdspro_mpic.h"
#include "__jdspro_moni.h"
#include "__jdspro_ldiv.h"
#include "__jdspro_lnr.h"
#include "__jdspro_obd_top.h"
#include "__jdspro_obt_top.h"
#include "__jdspro_p2m.h"
#include "__jdspro_pwch.h"

/* Define i/o mapping */
struct io_sen_top_apb{
    /* JDSIMG */
    struct io_sentop     SENTOP;                /* 2800_(8000 - 83FFh) */
    struct io_sg         SG[4];                 /* 2800_(8400 - 84FFh) */
                                                /* 2800_(8500 - 85FFh) */
                                                /* 2800_(8600 - 86FFh) */
                                                /* 2800_(8700 - 87FFh) */
    struct io_lvds       LVDS[4];               /* 2800_(8800 - 89FFh) */
                                                /* 2800_(8A00 - 8BFFh) */
                                                /* 2800_(8C00 - 8DFFh) */
                                                /* 2800_(8E00 - 8FFFh) */
    struct io_slvs       SLVSEC;                /* 2800_(9000 - 93FFh) */
    unsigned char dmy_9400_97FF[0x9800-0x9400]; /* 2800_(9400 - 97FFh) */
    struct io_mpid       MIPID[4];              /* 2800_(9800 - 98FFh) */
                                                /* 2800_(9900 - 99FFh) */
                                                /* 2800_(9A00 - 9AFFh) */
                                                /* 2800_(9B00 - 9BFFh) */
    struct io_mpic       MIPIC[4];              /* 2800_(9C00 - 9DFFh) */
                                                /* 2800_(9E00 - 9FFFh) */
                                                /* 2800_(A000 - A1FFh) */
                                                /* 2800_(A200 - A3FFh) */
    struct io_moni       MONI[4];               /* 2800_(A400 - A4FFh) */
                                                /* 2800_(A500 - A5FFh) */
                                                /* 2800_(A600 - A6FFh) */
                                                /* 2800_(A700 - A7FFh) */
    struct io_ldiv       LDIV[4];               /* 2800_(A800 - A8FFh) */
    struct io_lnr        LNR[4];                /* 2800_(AC00 - ACFFh) */
                                                /* 2800_(AD00 - ADFFh) */
                                                /* 2800_(AE00 - AEFFh) */
                                                /* 2800_(AF00 - AFFFh) */
    struct io_obd_top    OBD[4][2];             /* 2800_(B000 - B0FFh) */
                                                /* 2800_(B100 - B1FFh) */
                                                /* 2800_(B200 - B2FFh) */
                                                /* 2800_(B300 - B3FFh) */
                                                /* 2800_(B400 - B4FFh) */
                                                /* 2800_(B500 - B5FFh) */
                                                /* 2800_(B600 - B6FFh) */
                                                /* 2800_(B700 - B7FFh) */
    struct io_obt_top    OBT[4][2];             /* 2800_(B800 - B8FFh) */
                                                /* 2800_(B900 - B9FFh) */
                                                /* 2800_(BA00 - BAFFh) */
                                                /* 2800_(BB00 - BBFFh) */
                                                /* 2800_(BC00 - BCFFh) */
                                                /* 2800_(BD00 - BDFFh) */
                                                /* 2800_(BE00 - BEFFh) */
                                                /* 2800_(BF00 - BFFFh) */
    struct io_p2m        P2M[8];                /* 2800_(C000 - C0FFh) */
                                                /* 2800_(C100 - C1FFh) */
                                                /* 2800_(C200 - C2FFh) */
                                                /* 2800_(C300 - C3FFh) */
                                                /* 2800_(C400 - C4FFh) */
                                                /* 2800_(C500 - C5FFh) */
                                                /* 2800_(C600 - C6FFh) */
                                                /* 2800_(C700 - C7FFh) */
    struct io_pwch       PWCH[8];               /* 2800_(C800 - C8FFh) */
                                                /* 2800_(C900 - C9FFh) */
                                                /* 2800_(CA00 - CAFFh) */
                                                /* 2800_(CB00 - CBFFh) */
                                                /* 2800_(CC00 - CCFFh) */
                                                /* 2800_(CD00 - CDFFh) */
                                                /* 2800_(CE00 - CEFFh) */
                                                /* 2800_(CF00 - CFFFh) */

    unsigned char dmy_2800D000_2800FFFF[0x28010000-0x2800D000]; /* 2800_D000 - 2800_FFFFh */

};

#endif  // _IO_PRO_SEN_TOP_APB_H_
