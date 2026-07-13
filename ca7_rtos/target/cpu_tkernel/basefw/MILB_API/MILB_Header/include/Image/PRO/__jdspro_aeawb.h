/**
 * @file		__jdspro_aeawb.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_AEAWB_H_
#define _IO_PRO_AEAWB_H_

/*  structure of AETRG  (2886_6000h)    */
union io_aetrg{
    unsigned long       word;
    struct {
        unsigned long   AETRG   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of AEAWBCTL   (2886_6004h)    */
union io_aeawbctl{
    unsigned long       word;
    struct {
        unsigned long   AEAWBWMD    :3;
        unsigned long               :1;
        unsigned long   AWBGMD      :1;
        unsigned long               :3;
        unsigned long   AEEWMD      :1;
        unsigned long               :23;
    }bit;
};

/*  structure of AEV    (2886_6008h)    */
union io_aev{
    unsigned long       word;
    struct {
        unsigned long   AEV :14;
        unsigned long       :18;
    }bit;
};

/*  structure of AEH    (2886_600Ch)    */
union io_aeh{
    unsigned long       word;
    struct {
        unsigned long   AEH :14;
        unsigned long       :18;
    }bit;
};

/*  structure of AEVW   (2886_6010h)    */
union io_aevw{
    unsigned long       word;
    struct {
        unsigned long   AEVW    :9;
        unsigned long           :23;
    }bit;
};

/*  structure of AETW   (2886_6014h)    */
union io_aetw{
    unsigned long       word;
    struct {
        unsigned long   AETW    :9;
        unsigned long           :23;
    }bit;
};

/*  structure of AEBW   (2886_6018h)    */
union io_aebw{
    unsigned long       word;
    struct {
        unsigned long   AEBW    :9;
        unsigned long           :23;
    }bit;
};

/*  structure of AEHW   (2886_601Ch)    */
union io_aehw{
    unsigned long       word;
    struct {
        unsigned long   AEHW    :10;
        unsigned long           :22;
    }bit;
};

/*  structure of AELW   (2886_6020h)    */
union io_aelw{
    unsigned long       word;
    struct {
        unsigned long   AELW    :10;
        unsigned long           :22;
    }bit;
};

/*  structure of AERW   (2886_6024h)    */
union io_aerw{
    unsigned long       word;
    struct {
        unsigned long   AERW    :10;
        unsigned long           :22;
    }bit;
};

/*  structure of AEVBC  (2886_6028h)    */
union io_aevbc{
    unsigned long       word;
    struct {
        unsigned long   AEVBC   :7;
        unsigned long           :25;
    }bit;
};

/*  structure of AEHBC  (2886_602Ch)    */
union io_aehbc{
    unsigned long       word;
    struct {
        unsigned long   AEHBC   :7;
        unsigned long           :25;
    }bit;
};

/*  structure of AECW1  (2886_6030h)    */
union io_aecw1{
    unsigned long       word;
    struct {
        unsigned long   AECVW   :9;
        unsigned long           :7;
        unsigned long   AECHW   :10;
        unsigned long           :6;
    }bit;
};

/*  structure of AECW2  (2886_6034h)    */
union io_aecw2{
    unsigned long       word;
    struct {
        unsigned long   AECTW   :9;
        unsigned long           :7;
        unsigned long   AECLW   :10;
        unsigned long           :6;
    }bit;
};

/*  structure of AECW3  (2886_6038h)    */
union io_aecw3{
    unsigned long       word;
    struct {
        unsigned long   AECBW   :9;
        unsigned long           :7;
        unsigned long   AECRW   :10;
        unsigned long           :6;
    }bit;
};

/*  structure of AWBCW1 (2886_603Ch)    */
union io_awbcw1{
    unsigned long       word;
    struct {
        unsigned long   AWBCVW  :9;
        unsigned long           :7;
        unsigned long   AWBCHW  :10;
        unsigned long           :6;
    }bit;
};

/*  structure of AWBCW2 (2886_6040h)    */
union io_awbcw2{
    unsigned long       word;
    struct {
        unsigned long   AWBCTW  :9;
        unsigned long           :7;
        unsigned long   AWBCLW  :10;
        unsigned long           :6;
    }bit;
};

/*  structure of AWBCW3 (2886_6044h)    */
union io_awbcw3{
    unsigned long       word;
    struct {
        unsigned long   AWBCBW  :9;
        unsigned long           :7;
        unsigned long   AWBCRW  :10;
        unsigned long           :6;
    }bit;
};

/*  structure of AEYKRR (2886_6048h)    */
union io_aeykrr{
    unsigned long       word;
    struct {
        long            AEYKRR  :9;
        unsigned long           :23;
    }bit;
};

/*  structure of AEYKGR (2886_604Ch)    */
union io_aeykgr{
    unsigned long       word;
    struct {
        long            AEYKGR  :9;
        unsigned long           :23;
    }bit;
};

/*  structure of AEYKGB (2886_6050h)    */
union io_aeykgb{
    unsigned long       word;
    struct {
        long            AEYKGB  :9;
        unsigned long           :23;
    }bit;
};

/*  structure of AEYKBB (2886_6054h)    */
union io_aeykbb{
    unsigned long       word;
    struct {
        long            AEYKBB  :9;
        unsigned long           :23;
    }bit;
};

/*  structure of AWTDY  (2886_6058h)    */
union io_awtdy{
    unsigned long       word;
    struct {
        unsigned long   AWTDUY  :8;
        unsigned long   AWTDLY  :8;
        unsigned long           :16;
    }bit;
};

/*  structure of AWTDR  (2886_605Ch)    */
union io_awtdr{
    unsigned long       word;
    struct {
        unsigned long   AWTDUR  :8;
        unsigned long   AWTDLR  :8;
        unsigned long           :16;
    }bit;
};

/*  structure of AWTDG  (2886_6060h)    */
union io_awtdg{
    unsigned long       word;
    struct {
        unsigned long   AWTDUG  :8;
        unsigned long   AWTDLG  :8;
        unsigned long           :16;
    }bit;
};

/*  structure of AWTDB  (2886_6064h)    */
union io_awtdb{
    unsigned long       word;
    struct {
        unsigned long   AWTDUB  :8;
        unsigned long   AWTDLB  :8;
        unsigned long           :16;
    }bit;
};

/*  structure of AWMLCY (2886_6068h)    */
union io_awmlcy{
    unsigned long       word;
    struct {
        unsigned long   AWMLUCY :8;
        unsigned long   AWMLLCY :8;
        unsigned long           :16;
    }bit;
};

/*  structure of AWMLCB (2886_606Ch)    */
union io_awmlcb{
    unsigned long       word;
    struct {
        unsigned long   AWMLUCB :8;
        unsigned long   AWMLLCB :8;
        unsigned long           :16;
    }bit;
};

/*  structure of AWMLCR (2886_6070h)    */
union io_awmlcr{
    unsigned long       word;
    struct {
        unsigned long   AWMLUCR :8;
        unsigned long   AWMLLCR :8;
        unsigned long           :16;
    }bit;
};

/*  structure of AWMC   (2886_6080h)    */
union io_awmc_1{
    unsigned long      word;
    struct {
        long           AWMC00  :9;
        unsigned long          :7;
        long           AWMC01  :9;
        unsigned long          :7;
    }bit;
};
union io_awmc_2{
    unsigned long      word;
    struct {
        long           AWMC02  :9;
        unsigned long          :7;
        long           AWMC10  :9;
        unsigned long          :7;
    }bit;
};
union io_awmc_3{
    unsigned long      word;
    struct {
        long           AWMC11  :9;
        unsigned long          :7;
        long           AWMC12  :9;
        unsigned long          :7;
    }bit;
};union io_awmc_4{
    unsigned long      word;
    struct {
        long           AWMC20  :9;
        unsigned long          :7;
        long           AWMC21  :9;
        unsigned long          :7;
    }bit;
};
union io_awmc_5{
    unsigned long       word;
    struct {
        long            AWMC22  :9;
        unsigned long           :23;
    }bit;
};
struct io_awmc{
    union io_awmc_1    AWMC1;
    union io_awmc_2    AWMC2;
    union io_awmc_3    AWMC3;
    union io_awmc_4    AWMC4;
    union io_awmc_5    AWMC5;
};

/*  structure of LINEV  (2886_60A0h)    */
union io_linev_1{
    unsigned long       word;
    struct {
        unsigned long   LINEV_0 :15;
        unsigned long           :1;
        unsigned long   LINEV_1 :15;
        unsigned long           :1;
    }bit;
};
union io_linev_2{
    unsigned long       word;
    struct {
        unsigned long   LINEV_2 :15;
        unsigned long           :1;
        unsigned long   LINEV_3 :15;
        unsigned long           :1;
    }bit;
};
struct io_linev{
    union io_linev_1   LINEV1;
    union io_linev_2   LINEV2;
};

/*  structure of LINEDATA   (2886_60B0h)    */
union io_linedata_1{
    unsigned long       word;
    struct {
        unsigned long   LINEDATA_0  :26;
        unsigned long               :6;
    }bit;
};
union io_linedata_2{
    unsigned long       word;
    struct {
        unsigned long   LINEDATA_1  :26;
        unsigned long               :6;
    }bit;
};
union io_linedata_3{
    unsigned long       word;
    struct {
        unsigned long   LINEDATA_2  :26;
        unsigned long               :6;
    }bit;
};
union io_linedata_4{
    unsigned long       word;
    struct {
        unsigned long   LINEDATA_3  :26;
        unsigned long               :6;
    }bit;
};
struct io_linedata{
    union io_linedata_1    LINEDATA1;
    union io_linedata_2    LINEDATA2;
    union io_linedata_3    LINEDATA3;
    union io_linedata_4    LINEDATA4;
};

/*  structure of LINEINTENB (2886_60C0h)    */
union io_lineintenb{
    unsigned long       word;
    struct {
        unsigned long   LINEE   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of LINEINTFLG (2886_60C4h)    */
union io_lineintflg{
    unsigned long       word;
    struct {
        unsigned long   __LINEF :1;
        unsigned long           :31;
    }bit;
};

/*  structure of AEAWBPAEN  (2886_60C8h)    */
union io_aeawbpaen{
    unsigned long       word;
    struct {
        unsigned long   PAEN    :1;
        unsigned long           :31;
    }bit;
};

/* Define i/o mapping */
struct io_aeawb{
    /* JDSIMG */
    union  io_aetrg        AETRG;      /* 2886_(6000 - 6003h) */
    union  io_aeawbctl     AEAWBCTL;   /* 2886_(6004 - 6007h) */
    union  io_aev          AEV;        /* 2886_(6008 - 600Bh) */
    union  io_aeh          AEH;        /* 2886_(600C - 600Fh) */
    union  io_aevw         AEVW;       /* 2886_(6010 - 6013h) */
    union  io_aetw         AETW;       /* 2886_(6014 - 6017h) */
    union  io_aebw         AEBW;       /* 2886_(6018 - 601Bh) */
    union  io_aehw         AEHW;       /* 2886_(601C - 601Fh) */
    union  io_aelw         AELW;       /* 2886_(6020 - 6023h) */
    union  io_aerw         AERW;       /* 2886_(6024 - 6027h) */
    union  io_aevbc        AEVBC;      /* 2886_(6028 - 602Bh) */
    union  io_aehbc        AEHBC;      /* 2886_(602C - 602Fh) */
    union  io_aecw1        AECW1;      /* 2886_(6030 - 6033h) */
    union  io_aecw2        AECW2;      /* 2886_(6034 - 6037h) */
    union  io_aecw3        AECW3;      /* 2886_(6038 - 603Bh) */
    union  io_awbcw1       AWBCW1;     /* 2886_(603C - 603Fh) */
    union  io_awbcw2       AWBCW2;     /* 2886_(6040 - 6043h) */
    union  io_awbcw3       AWBCW3;     /* 2886_(6044 - 6047h) */
    union  io_aeykrr       AEYKRR;     /* 2886_(6048 - 604Bh) */
    union  io_aeykgr       AEYKGR;     /* 2886_(604C - 604Fh) */
    union  io_aeykgb       AEYKGB;     /* 2886_(6050 - 6053h) */
    union  io_aeykbb       AEYKBB;     /* 2886_(6054 - 6057h) */
    union  io_awtdy        AWTDY;      /* 2886_(6058 - 605Bh) */
    union  io_awtdr        AWTDR;      /* 2886_(605C - 605Fh) */
    union  io_awtdg        AWTDG;      /* 2886_(6060 - 6063h) */
    union  io_awtdb        AWTDB;      /* 2886_(6064 - 6067h) */
    union  io_awmlcy       AWMLCY;     /* 2886_(6068 - 606Bh) */
    union  io_awmlcb       AWMLCB;     /* 2886_(606C - 606Fh) */
    union  io_awmlcr       AWMLCR;     /* 2886_(6070 - 6073h) */

    unsigned char dmy_6074_607F[0x6080-0x6074]; /* 2886_(6074 - 607Fh) */

    struct io_awmc         AWMC;       /* 2886_(6080 - 6093h) */

    unsigned char dmy_6094_609F[0x60A0-0x6094]; /* 2886_(6094 - 609Fh) */

    struct io_linev        LINEV;      /* 2886_(60A0 - 60A7h) */

    unsigned char dmy_60A8_60AF[0x60B0-0x60A8]; /* 2886_(60A8 - 60AFh) */

    struct io_linedata     LINEDATA;   /* 2886_(60B0 - 60BFh) */
    union  io_lineintenb   LINEINTENB; /* 2886_(60C0 - 60C3h) */
    union  io_lineintflg   LINEINTFLG; /* 2886_(60C4 - 60C7h) */
    union  io_aeawbpaen    AEAWBPAEN;  /* 2886_(60C8 - 60CBh) */

    unsigned char dmy_60CC_60FF[0x6100-0x60CC]; /* 2886_(60CC - 60FFh) */

};

#endif	// _IO_PRO_AEAWB_H_
