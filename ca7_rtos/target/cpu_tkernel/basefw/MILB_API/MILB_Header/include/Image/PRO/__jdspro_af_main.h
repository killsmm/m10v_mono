/**
 * @file		__jdspro_af_main.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_AF_MAIN_H_
#define _IO_PRO_AF_MAIN_H_

/*  structure of AFMTRXRR   (2886_2080h)    */
union io_afmtrxrr{
    unsigned long       word;
    struct {
        long            AFMTRXRR    :10;
        unsigned long               :22;
    }bit;
};

/*  structure of AFMTRXGR   (2886_2084h)    */
union io_afmtrxgr{
    unsigned long       word;
    struct {
        long            AFMTRXGR    :10;
        unsigned long               :22;
    }bit;
};

/*  structure of AFMTRXGB   (2886_2088h)    */
union io_afmtrxgb{
    unsigned long       word;
    struct {
        long            AFMTRXGB    :10;
        unsigned long               :22;
    }bit;
};

/*  structure of AFMTRXBB   (2886_208Ch)    */
union io_afmtrxbb{
    unsigned long       word;
    struct {
        long            AFMTRXBB    :10;
        unsigned long               :22;
    }bit;
};

/*  structure of AFMDEN (2886_2090h)    */
union io_afmden{
    unsigned long       word;
    struct {
        unsigned long   AFMDEN  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of AFKNEN (2886_2094h)    */
union io_afknen{
    unsigned long       word;
    struct {
        unsigned long   AFKNEN  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of AFKNB  (2886_20A0h)    */
union io_afknb_1{
    unsigned long       word;
    struct {
        unsigned long   AFKNB_1 :14;
        unsigned long           :2;
        unsigned long   AFKNB_2 :14;
        unsigned long           :2;
    }bit;
};
union io_afknb_2{
    unsigned long       word;
    struct {
        unsigned long   AFKNB_3 :14;
        unsigned long           :2;
        unsigned long   AFKNB_4 :14;
        unsigned long           :2;
    }bit;
};
union io_afknb_3{
    unsigned long       word;
    struct {
        unsigned long   AFKNB_5 :14;
        unsigned long           :2;
        unsigned long   AFKNB_6 :14;
        unsigned long           :2;
    }bit;
};
union io_afknb_4{
    unsigned long       word;
    struct {
        unsigned long   AFKNB_7 :14;
        unsigned long           :2;
        unsigned long           :16;
    }bit;
};
struct io_afknb{
    union io_afknb_1  AFKNB1;
    union io_afknb_2  AFKNB2;
    union io_afknb_3  AFKNB3;
    union io_afknb_4  AFKNB4;
};

/*  structure of AFKNINC    (2886_20B0h)    */
union io_afkninc_1{
    unsigned long       word;
    struct {
        unsigned long   AFKNINC_0   :12;
        unsigned long               :4;
        unsigned long   AFKNINC_1   :12;
        unsigned long               :4;
    }bit;
};
union io_afkninc_2{
    unsigned long       word;
    struct {
        unsigned long   AFKNINC_2   :12;
        unsigned long               :4;
        unsigned long   AFKNINC_3   :12;
        unsigned long               :4;
    }bit;
};
union io_afkninc_3{
    unsigned long       word;
    struct {
        unsigned long   AFKNINC_4   :12;
        unsigned long               :4;
        unsigned long   AFKNINC_5   :12;
        unsigned long               :4;
    }bit;
};
union io_afkninc_4{
    unsigned long       word;
    struct {
        unsigned long   AFKNINC_6   :12;
        unsigned long               :4;
        unsigned long   AFKNINC_7   :12;
        unsigned long               :4;
    }bit;
};
struct io_afkninc{
    union io_afkninc_1    AFKNINC1;
    union io_afkninc_2    AFKNINC2;
    union io_afkninc_3    AFKNINC3;
    union io_afkninc_4    AFKNINC4;
};

/*  structure of AFKNOFS    (2886_20C0h)    */
union io_afknofs_1{
    unsigned long       word;
    struct {
        unsigned long   AFKNOFS_0   :12;
        unsigned long               :4;
        unsigned long   AFKNOFS_1   :12;
        unsigned long               :4;
    }bit;
};
union io_afknofs_2{
    unsigned long       word;
    struct {
        unsigned long   AFKNOFS_2   :12;
        unsigned long               :4;
        unsigned long   AFKNOFS_3   :12;
        unsigned long               :4;
    }bit;
};
union io_afknofs_3{
    unsigned long       word;
    struct {
        unsigned long   AFKNOFS_4   :12;
        unsigned long               :4;
        unsigned long   AFKNOFS_5   :12;
        unsigned long               :4;
    }bit;
};
union io_afknofs_4{
    unsigned long       word;
    struct {
        unsigned long   AFKNOFS_6   :12;
        unsigned long               :4;
        unsigned long   AFKNOFS_7   :12;
        unsigned long               :4;
    }bit;
};
struct io_afknofs{
    union io_afknofs_1    AFKNOFS1;
    union io_afknofs_2    AFKNOFS2;
    union io_afknofs_3    AFKNOFS3;
    union io_afknofs_4    AFKNOFS4;
};

/* Define i/o mapping */
struct io_afmain{
    /* JDSIMG */
    union  io_afmtrxrr    AFMTRXRR;   /* 2886_(2080 - 2083h) */
    union  io_afmtrxgr    AFMTRXGR;   /* 2886_(2084 - 2087h) */
    union  io_afmtrxgb    AFMTRXGB;   /* 2886_(2088 - 208Bh) */
    union  io_afmtrxbb    AFMTRXBB;   /* 2886_(208C - 208Fh) */
    union  io_afmden      AFMDEN;     /* 2886_(2090 - 2093h) */
    union  io_afknen      AFKNEN;     /* 2886_(2094 - 2097h) */

    unsigned char dmy_2098_209F[0x20A0-0x2098]; /* 2886_(2098 - 209Fh) */

    struct io_afknb       AFKNB;      /* 2886_(20A0 - 20AFh) */
    struct io_afkninc     AFKNINC;    /* 2886_(20B0 - 20BFh) */
    struct io_afknofs     AFKNOFS;    /* 2886_(20C0 - 20CFh) */

    unsigned char dmy_20D0_20FF[0x2100-0x20D0]; /* 2886_(20D0 - 20FFh) */

};

#endif	// _IO_PRO_AF_MAIN_H_
