/**
 * @file        __jdspro_obd_top.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_OBD_TOP_H_
#define _IO_PRO_OBD_TOP_H_

#include "__jdspro_obd.h"

/*  structure of OBDTRG (2800_B000h)    */
union io_obdtrg{
    unsigned long       word;
    struct {
        unsigned long   OBDTRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of OBDINTENB  (2800_B004h)    */
union io_obdintenb{
    unsigned long       word;
    struct {
        unsigned long   OBDE    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of OBDINTFLG  (2800_B008h)    */
union io_obdintflg{
    unsigned long       word;
    struct {
        unsigned long   __OBDF  :1;
        unsigned long           :31;
    }bit;
};

/* Define i/o mapping */
struct io_obd_top{
    /* JDSIMG */
    union  io_obdtrg      OBDTRG;               /* 2800_(B000 - B003h) */
    union  io_obdintenb   OBDINTENB;            /* 2800_(B004 - B007h) */
    union  io_obdintflg   OBDINTFLG;            /* 2800_(B008 - B00Bh) */

    unsigned char dmy_B00C_B07F[0xB080-0xB00C]; /* 2800_(B00C - B07Fh) */

    struct io_obd         OBDCORE;              /* 2800_(B080 - B0BFh) */

    unsigned char dmy_B0C0_B0FF[0xB100-0xB0C0]; /* 2800_(B0C0 - B0FFh) */

};

#endif  // _IO_PRO_OBD_TOP_H_
