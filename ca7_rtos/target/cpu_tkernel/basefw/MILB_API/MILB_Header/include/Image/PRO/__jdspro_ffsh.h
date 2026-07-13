/**
 * @file        __jdspro_ffsh.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_FFSH_H_
#define _IO_PRO_FFSH_H_

/*  structure of FFSHTRG  (2840_8200h)    */
union io_ffshtrg{
    unsigned long       word;
    struct {
        unsigned long   FFSHTRG :2;
        unsigned long           :30;
    }bit;
};

/*  structure of FFSHPAEN (2840_8204h)    */
union io_ffshpaen{
    unsigned long       word;
    struct {
        unsigned long   FFSHPAEN    :1;
        unsigned long               :31;
    }bit;
};

/*  structure of FFSHV    (2840_820Ch)    */
union io_ffshv{
    unsigned long       word;
    struct {
        unsigned long   FFSHV   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FFSHH    (2840_8210h)    */
union io_ffshh{
    unsigned long       word;
    struct {
        unsigned long   FFSHH   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FFSHVW   (2840_8214h)    */
union io_ffshvw{
    unsigned long       word;
    struct {
        unsigned long   FFSHVW  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FFSHHW   (2840_8218h)    */
union io_ffshhw{
    unsigned long       word;
    struct {
        unsigned long   FFSHHW  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FFSHBC   (2840_821Ch)    */
union io_ffshbc{
    unsigned long       word;
    struct {
        long            FFSHBC0 :10;
        unsigned long           :6;
        long            FFSHBC1 :10;
        unsigned long           :6;
    }bit;
};

/*  structure of FFSHSUBPR    (2840_8230h)    */
union io_ffshsubpr{
    unsigned long       word;
    struct {
        unsigned long   FFSHSUBPRH  :11;
        unsigned long               :5;
        unsigned long   FFSHSUBPRV  :11;
        unsigned long               :5;
    }bit;
};

/*  structure of FFSHSUBSP    (2840_8234h)    */
union io_ffshsubsp{
    unsigned long       word;
    struct {
        unsigned long   FFSHSUBSPH  :10;
        unsigned long               :6;
        unsigned long   FFSHSUBSPV  :10;
        unsigned long               :6;
    }bit;
};

/*  structure of FFSHSUBDR    (2840_8238h)    */
union io_ffshsubdr{
    unsigned long       word;
    struct {
        unsigned long   FFSHSUBDRM  :16;
        unsigned long   FFSHSUBDRE  :4;
        unsigned long               :12;
    }bit;
};

/*  structure of FFSHSUBST    (2840_823Ch)    */
union io_ffshsubst{
    unsigned long       word;
    struct {
        unsigned long   FFSHSUBSTH  :10;
        unsigned long               :6;
        unsigned long   FFSHSUBSTV  :10;
        unsigned long               :6;
    }bit;
};

/*  structure of FFSHWRIC (2840_8280h)    */
union io_ffshwric{
    unsigned long       word;
    struct {
        unsigned long   FFSHWRIC    :13;
        unsigned long               :19;
    }bit;
};

/* Define i/o mapping */
struct io_ffsh{
    /* JDSIMG */
    union  io_ffshtrg      FFSHTRG;             /* 2840_(8200 - 8203h) */
    union  io_ffshpaen     FFSHPAEN;            /* 2840_(8204 - 8207h) */

    unsigned char dmy_8208_820B[0x820C-0x8208]; /* 2840_(8208 - 820Bh) */

    union  io_ffshv        FFSHV;               /* 2840_(820C - 820Fh) */
    union  io_ffshh        FFSHH;               /* 2840_(8210 - 8213h) */
    union  io_ffshvw       FFSHVW;              /* 2840_(8214 - 8217h) */
    union  io_ffshhw       FFSHHW;              /* 2840_(8218 - 821Bh) */
    union  io_ffshbc       FFSHBC;              /* 2840_(821C - 821Fh) */

    unsigned char dmy_8220_822F[0x8230-0x8220]; /* 2840_(8220 - 822Fh) */

    union  io_ffshsubpr    FFSHSUBPR;           /* 2840_(8230 - 8233h) */
    union  io_ffshsubsp    FFSHSUBSP;           /* 2840_(8234 - 8237h) */
    union  io_ffshsubdr    FFSHSUBDR;           /* 2840_(8238 - 823Bh) */
    union  io_ffshsubst    FFSHSUBST;           /* 2840_(823C - 824Fh) */

    unsigned char dmy_8240_827F[0x8280-0x8240]; /* 2840_(8240 - 827Fh) */

    union  io_ffshwric     FFSHWRIC;            /* 2840_(8280 - 8283h) */

    unsigned char dmy_8284_82FF[0x8300-0x8284]; /* 2840_(8284 - 82FFh) */

};

#endif  // _IO_PRO_FFSH_H_
