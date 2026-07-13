/**
 * @file        __jdspro_obd.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_OBD_H_
#define _IO_PRO_OBD_H_

/*  structure of OBDV   (2800_B080h)    */
union io_obdv{
    unsigned long       word;
    struct {
        unsigned long   OBDV    :13;
        unsigned long           :19;
    }bit;
};

/*  structure of OBDH   (2800_B084h)    */
union io_obdh{
    unsigned long       word;
    struct {
        unsigned long   OBDH    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of OBDVW  (2800_B088h)    */
union io_obdvw{
    unsigned long       word;
    struct {
        unsigned long   OBDVW   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of OBDHW  (2800_B08Ch)    */
union io_obdhw{
    unsigned long       word;
    struct {
        unsigned long   OBDHW   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of OBDTHBIT   (2800_B090h)    */
union io_obdthbit{
    unsigned long       word;
    struct {
        unsigned long   OBDLTHBIT   :14;
        unsigned long               :2;
        unsigned long   OBDHTHBIT   :14;
        unsigned long               :2;
    }bit;
};

/*  structure of OBDDATAR   (2800_B094h)    */
union io_obddatar{
    unsigned long       word;
    struct {
        unsigned long   OBDDATAR    :32;
    }bit;
};

/*  structure of OBDDATAGR  (2800_B098h)    */
union io_obddatagr{
    unsigned long       word;
    struct {
        unsigned long   OBDDATAGR   :32;
    }bit;
};

/*  structure of OBDDATAGB  (2800_B09Ch)    */
union io_obddatagb{
    unsigned long       word;
    struct {
        unsigned long   OBDDATAGB   :32;
    }bit;
};

/*  structure of OBDDATAB   (2800_B0A0h)    */
union io_obddatab{
    unsigned long       word;
    struct {
        unsigned long   OBDDATAB    :32;
    }bit;
};

/*  structure of OBDCNTR    (2800_B0A4h)    */
union io_obdcntr{
    unsigned long       word;
    struct {
        unsigned long   OBDCNTR :20;
        unsigned long           :12;
    }bit;
};

/*  structure of OBDCNTGR   (2800_B0A8h)    */
union io_obdcntgr{
    unsigned long       word;
    struct {
        unsigned long   OBDCNTGR    :20;
        unsigned long               :12;
    }bit;
};

/*  structure of OBDCNTGB   (2800_B0ACh)    */
union io_obdcntgb{
    unsigned long       word;
    struct {
        unsigned long   OBDCNTGB    :20;
        unsigned long               :12;
    }bit;
};

/*  structure of OBDCNTB    (2800_B0B0h)    */
union io_obdcntb{
    unsigned long       word;
    struct {
        unsigned long   OBDCNTB :20;
        unsigned long           :12;
    }bit;
};

/* Define i/o mapping */
struct io_obd{
    /* JDSIMG */
    union  io_obdv       OBDV;                  /* 2800_(B080 - B083h) */
    union  io_obdh       OBDH;                  /* 2800_(B084 - B087h) */
    union  io_obdvw      OBDVW;                 /* 2800_(B088 - B08Bh) */
    union  io_obdhw      OBDHW;                 /* 2800_(B08C - B08Fh) */
    union  io_obdthbit   OBDTHBIT;              /* 2800_(B090 - B093h) */
    union  io_obddatar   OBDDATAR;              /* 2800_(B094 - B097h) */
    union  io_obddatagr  OBDDATAGR;             /* 2800_(B098 - B09Bh) */
    union  io_obddatagb  OBDDATAGB;             /* 2800_(B09C - B09Fh) */
    union  io_obddatab   OBDDATAB;              /* 2800_(B0A0 - B0A3h) */
    union  io_obdcntr    OBDCNTR;               /* 2800_(B0A4 - B0A7h) */
    union  io_obdcntgr   OBDCNTGR;              /* 2800_(B0A8 - B0ABh) */
    union  io_obdcntgb   OBDCNTGB;              /* 2800_(B0AC - B0AFh) */
    union  io_obdcntb    OBDCNTB;               /* 2800_(B0B0 - B0B3h) */

    unsigned char dmy_B0B4_B0BF[0xB0C0-0xB0B4]; /* 2800_(B0B4 - B0BFh) */
};

#endif  // _IO_PRO_OBD_H_
