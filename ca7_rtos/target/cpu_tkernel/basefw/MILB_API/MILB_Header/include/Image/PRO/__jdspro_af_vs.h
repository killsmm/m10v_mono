/**
 * @file		__jdspro_af_vs.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_AF_VS_H_
#define _IO_PRO_AF_VS_H_

/*  structure of AFVSCTL    (2886_2200h)    */
union io_afvsctl{
    unsigned long       word;
    struct {
        unsigned long   AFVSCYC :4;
        unsigned long   AFSLV   :2;
        unsigned long           :26;
    }bit;
};

/*  structure of AFVSH  (2886_2204h)    */
union io_afvsh{
    unsigned long       word;
    struct {
        unsigned long   AFVSH   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of AFVSHW (2886_2208h)    */
union io_afvshw{
    unsigned long       word;
    struct {
        unsigned long   AFVSHW  :11;
        unsigned long           :21;
    }bit;
};

/*  structure of AFIIRVB    (2886_2210h)    */
union io_afiirvb_1{
    unsigned long      word;
    struct {
        long           AFIIRVK     :15;
        unsigned long              :1;
        long           AFIIRVA0    :8;
        long           AFIIRVA1    :8;
    }bit;
};
union io_afiirvb_2{
    unsigned long      word;
    struct {
        long           AFIIRVA2    :8;
        unsigned long              :8;
        long           AFIIRVB1    :15;
        unsigned long              :1;
    }bit;
};
union io_afiirvb_3{
    unsigned long      word;
    struct {
        long           AFIIRVB2    :15;
        unsigned long              :17;
    }bit;
};
struct io_afiirvb{
    union io_afiirvb_1  AFIIRVB1;
    union io_afiirvb_2  AFIIRVB2;
    union io_afiirvb_3  AFIIRVB3;
};

/*  structure of AFCOR2V    (2886_2220h)    */
union io_afcor2v{
    unsigned long       word;
    struct {
        unsigned long   AFCOR2LVV   :12;
        unsigned long               :4;
        unsigned long   AFCOR2V     :12;
        unsigned long               :4;
    }bit;
};

/*  structure of AFFIRVB    (2886_2224h)    */
union io_affirvb{
    unsigned long       word;
    struct {
        long            AFFIRVB1    :8;
        long            AFFIRVB2    :8;
        long            AFFIRVB3    :8;
        unsigned long               :8;
    }bit;
};

/* Define i/o mapping */
struct io_afvs{
    /* JDSIMG */
    union  io_afvsctl   AFVSCTL;    /* 2886_(2200 - 2203h) */
    union  io_afvsh     AFVSH;      /* 2886_(2204 - 2207h) */
    union  io_afvshw    AFVSHW;     /* 2886_(2208 - 220Bh) */

    unsigned char dmy_220C_220F[0x2210-0x220C]; /* 2886_(220C - 220Fh) */

    struct io_afiirvb   AFIIRVB;    /* 2886_(2210 - 221Bh) */

    unsigned char dmy_221C_221F[0x2220-0x221C]; /* 2886_(221C - 221Fh) */

    union  io_afcor2v   AFCOR2V;    /* 2886_(2220 - 2223h) */
    union  io_affirvb   AFFIRVB;    /* 2886_(2224 - 2227h) */

    unsigned char dmy_2228_223F[0x2240-0x2228]; /* 2886_(2228 - 223Fh) */

};

#endif	// _IO_PRO_AF_VS_H_
