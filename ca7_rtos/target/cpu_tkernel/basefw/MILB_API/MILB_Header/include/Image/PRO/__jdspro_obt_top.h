/**
 * @file        __jdspro_obt_top.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_OBT_TOP_H_
#define _IO_PRO_OBT_TOP_H_

#include "__jdspro_obt.h"

/*  structure of OBTTRG (2800_B800h)    */
union io_obttrg{
    unsigned long       word;
    struct {
        unsigned long   OBTTRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of OBTINTENB  (2800_B804h)    */
union io_obtintenb{
    unsigned long       word;
    struct {
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   OBTE    :1;
        unsigned long           :27;
    }bit;
};

/*  structure of OBTINTFLG  (2800_B808h)    */
union io_obtintflg{
    unsigned long       word;
    struct {
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   __OBTF  :1;
        unsigned long           :27;
    }bit;
};

/* Define i/o mapping */
struct io_obt_top{
    /* JDSIMG */
    union  io_obttrg      OBTTRG;               /* 2800_(B800 - B803h) */
    union  io_obtintenb   OBTINTENB;            /* 2800_(B804 - B807h) */
    union  io_obtintflg   OBTINTFLG;            /* 2800_(B808 - B80Bh) */

    unsigned char dmy_B80C_B8BF[0xB8C0-0xB80C]; /* 2800_(B80C - B8BFh) */

    struct io_obt         OBTCORE;              /* 2800_(B8C0 - B8FFh) */
};

#endif  // _IO_PRO_OBT_TOP_H_
