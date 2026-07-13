/**
 * @file        __jdspro_prch.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_PRCH_H_
#define _IO_PRO_PRCH_H_

/*  structure of PRCHTRG    (2840_B200h)    */
union io_prchtrg{
    unsigned long       word;
    struct {
        unsigned long   PRCHTRG :2;
        unsigned long           :30;
    }bit;
};

/*  structure of PRCHCTL    (2840_B204h)    */
union io_prchctl{
    unsigned long       word;
    struct {
        unsigned long   PRBMD   :2;
        unsigned long           :2;
        unsigned long   PRLV    :4;
        unsigned long           :24;
    }bit;
};

/*  structure of PRCHRS (2840_B208h)    */
union io_prchrs{
    unsigned long       word;
    struct {
        unsigned long   PRS :1;
        unsigned long       :31;
    }bit;
};

/*  structure of PRAXCTL    (2840_B20Ch)    */
union io_praxctl{
    unsigned long       word;
    struct {
        unsigned long   ARCACHE :4;
        unsigned long           :4;
        unsigned long   ARPROT  :3;
        unsigned long           :21;
    }bit;
};

/*  structure of PRCHINTENB (2840_B210h)    */
union io_prchintenb{
    unsigned long       word;
    struct {
        unsigned long   PRE     :1;
        unsigned long           :3;
        unsigned long   PREE    :1;
        unsigned long           :3;
        unsigned long   PRXE    :1;
        unsigned long           :23;
    }bit;
};

/*  structure of PRCHINTFLG (2840_B214h)    */
union io_prchintflg{
    unsigned long       word;
    struct {
        unsigned long   __PRF     :1;
        unsigned long             :3;
        unsigned long   __PREF    :1;
        unsigned long             :3;
        unsigned long   __PRXF    :1;
        unsigned long             :23;
    }bit;
};

/*  structure of PRCHBRESP  (2840_B218h)    */
union io_prchbresp{
    unsigned long       word;
    struct {
        unsigned long   PRCHBRESP   :2;
        unsigned long               :30;
    }bit;
};

/*  structure of PRSA   (2840_B21Ch)    */
union io_prsa{
    unsigned long       word;
    struct {
        unsigned long   PRSA    :32;
    }bit;
};

/*  structure of PRLSIZE    (2840_B220h)    */
union io_prlsize{
    unsigned long       word;
    struct {
        unsigned long   PRLSIZE :32;
    }bit;
};

/*  structure of PRVSIZE    (2840_B224h)    */
union io_prvsize{
    unsigned long       word;
    struct {
        unsigned long   PRVSIZE :14;
        unsigned long           :18;
    }bit;
};

/*  structure of PRHSIZE    (2840_B228h)    */
union io_prhsize{
    unsigned long       word;
    struct {
        unsigned long   PRHSIZE :24;
        unsigned long           :8;
    }bit;
};

/* Define i/o mapping */
struct io_prch{
    /* JDSIMG */
    union  io_prchtrg       PRCHTRG;            /* 2840_(B200 - B203h) */
    union  io_prchctl       PRCHCTL;            /* 2840_(B204 - B207h) */
    union  io_prchrs        PRCHRS;             /* 2840_(B208 - B20Bh) */
    union  io_praxctl       PRAXCTL;            /* 2840_(B20C - B20Fh) */
    union  io_prchintenb    PRCHINTENB;         /* 2840_(B210 - B213h) */
    union  io_prchintflg    PRCHINTFLG;         /* 2840_(B214 - B217h) */
    union  io_prchbresp     PRCHBRESP;          /* 2840_(B218 - B21Bh) */
    union  io_prsa          PRSA;               /* 2840_(B21C - B21Fh) */
    union  io_prlsize       PRLSIZE;            /* 2840_(B220 - B223h) */
    union  io_prvsize       PRVSIZE;            /* 2840_(B224 - B227h) */
    union  io_prhsize       PRHSIZE;            /* 2840_(B228 - B22Bh) */

    unsigned char dmy_B22C_B2FF[0xB300-0xB22C]; /* 2840_(B22C - B2FFh) */
};

#endif  // _IO_PRO_PRCH_H_
