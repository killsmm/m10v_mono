/**
 * @file        __jdspro_obt.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_OBT_H_
#define _IO_PRO_OBT_H_

/*  structure of OBTCTL (2800_B8C0h)    */
union io_obtctl{
    unsigned long       word;
    struct {
        unsigned long   OBTEN   :1;
        unsigned long           :3;
        unsigned long   OBTMD   :1;
        unsigned long           :3;
        unsigned long   OBTSL   :1;
        unsigned long           :3;
        unsigned long   OBTCEN  :1;
        unsigned long           :19;
    }bit;
};

/*  structure of OBTV   (2800_B8C4h)    */
union io_obtv{
    unsigned long       word;
    struct {
        unsigned long   OBTV    :13;
        unsigned long           :19;
    }bit;
};

/*  structure of OBTH   (2800_B8C8h)    */
union io_obth{
    unsigned long       word;
    struct {
        unsigned long   OBTH    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of OBTVW  (2800_B8CCh)    */
union io_obtvw{
    unsigned long       word;
    struct {
        unsigned long   OBTVW   :7;
        unsigned long           :25;
    }bit;
};

/*  structure of OBTHW  (2800_B8D0h)    */
union io_obthw{
    unsigned long       word;
    struct {
        unsigned long   OBTHW   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of OBTTHBIT   (2800_B8D4h)    */
union io_obtthbit{
    unsigned long       word;
    struct {
        unsigned long   OBTHTHBIT   :14;
        unsigned long               :2;
        unsigned long   OBTLTHBIT   :14;
        unsigned long               :2;
    }bit;
};

/*  structure of OBTDATAR   (2800_B8D8h)    */
union io_obtdatar{
    unsigned long       word;
    struct {
        unsigned long   OBTDATAR    :14;
        unsigned long               :18;
    }bit;
};

/*  structure of OBTDATAGR  (2800_B8DCh)    */
union io_obtdatagr{
    unsigned long       word;
    struct {
        unsigned long   OBTDATAGR   :14;
        unsigned long               :18;
    }bit;
};

/*  structure of OBTDATAGB  (2800_B8E0h)    */
union io_obtdatagb{
    unsigned long       word;
    struct {
        unsigned long   OBTDATAGB   :14;
        unsigned long               :18;
    }bit;
};

/*  structure of OBTDATAB   (2800_B8E4h)    */
union io_obtdatab{
    unsigned long       word;
    struct {
        unsigned long   OBTDATAB    :14;
        unsigned long               :18;
    }bit;
};

/*  structure of OBTOFSR    (2800_B8E8h)    */
union io_obtofsr{
    unsigned long       word;
    struct {
        long            OBTOFSR :15;
        unsigned long           :17;
    }bit;
};

/*  structure of OBTOFSGR   (2800_B8ECh)    */
union io_obtofsgr{
    unsigned long       word;
    struct {
        long            OBTOFSGR    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of OBTOFSGB   (2800_B8F0h)    */
union io_obtofsgb{
    unsigned long       word;
    struct {
        long            OBTOFSGB    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of OBTOFSB    (2800_B8F4h)    */
union io_obtofsb{
    unsigned long       word;
    struct {
        long            OBTOFSB :15;
        unsigned long           :17;
    }bit;
};

/* Define i/o mapping */
struct io_obt{
    /* JDSIMG */
    union  io_obtctl     OBTCTL;                /* 2800_(B8C0 - B8C3h) */
    union  io_obtv       OBTV;                  /* 2800_(B8C4 - B8C7h) */
    union  io_obth       OBTH;                  /* 2800_(B8C8 - B8CBh) */
    union  io_obtvw      OBTVW;                 /* 2800_(B8CC - B8CFh) */
    union  io_obthw      OBTHW;                 /* 2800_(B8D0 - B8D3h) */
    union  io_obtthbit   OBTTHBIT;              /* 2800_(B8D4 - B8D7h) */
    union  io_obtdatar   OBTDATAR;              /* 2800_(B8D8 - B8DBh) */
    union  io_obtdatagr  OBTDATAGR;             /* 2800_(B8DC - B8DFh) */
    union  io_obtdatagb  OBTDATAGB;             /* 2800_(B8E0 - B8E3h) */
    union  io_obtdatab   OBTDATAB;              /* 2800_(B8E4 - B8E7h) */
    union  io_obtofsr    OBTOFSR;               /* 2800_(B8E8 - B8EBh) */
    union  io_obtofsgr   OBTOFSGR;              /* 2800_(B8EC - B8EFh) */
    union  io_obtofsgb   OBTOFSGB;              /* 2800_(B8F0 - B8F3h) */
    union  io_obtofsb    OBTOFSB;               /* 2800_(B8F4 - B8F7h) */

    unsigned char dmy_B8F8_B8FF[0xB900-0xB8F8]; /* 2800_(B8F8 - B8FFh) */
};

#endif  // _IO_PRO_OBT_H_
