/**
 * @file		__jdspro_flck.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_FLCK_H_
#define _IO_PRO_FLCK_H_

/*  structure of FLTRG  (2886_6400h)    */
union io_fltrg{
    unsigned long       word;
    struct {
        unsigned long   FLTRG   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of FLCKCTL    (2886_6408h)    */
union io_flckctl{
    unsigned long       word;
    struct {
        unsigned long   FLWPMD  :1;
        unsigned long           :3;
        unsigned long   FLWLMD  :1;
        unsigned long           :27;
    }bit;
};

/*  structure of FLV    (2886_640Ch)    */
union io_flv{
    unsigned long       word;
    struct {
        unsigned long   FLV :14;
        unsigned long       :18;
    }bit;
};

/*  structure of FLH    (2886_6410h)    */
union io_flh{
    unsigned long       word;
    struct {
        unsigned long   FLH :14;
        unsigned long       :18;
    }bit;
};

/*  structure of FLVW   (2886_6414h)    */
union io_flvw{
    unsigned long       word;
    struct {
        unsigned long   FLVW    :11;
        unsigned long           :5;
        unsigned long   FLCVW   :11;
        unsigned long           :5;
    }bit;
};

/*  structure of FLHW   (2886_6418h)    */
union io_flhw{
    unsigned long       word;
    struct {
        unsigned long   FLHW    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FLBC   (2886_641Ch)    */
union io_flbc{
    unsigned long       word;
    struct {
        unsigned long   FLBC    :13;
        unsigned long           :19;
    }bit;
};

/*  structure of FLYKRR (2886_6420h)    */
union io_flykrr{
    unsigned long       word;
    struct {
        long            FLYKRR  :9;
        unsigned long           :23;
    }bit;
};

/*  structure of FLYKGR (2886_6424h)    */
union io_flykgr{
    unsigned long       word;
    struct {
        long            FLYKGR  :9;
        unsigned long           :23;
    }bit;
};

/*  structure of FLYKGB (2886_6428h)    */
union io_flykgb{
    unsigned long       word;
    struct {
        long            FLYKGB  :9;
        unsigned long           :23;
    }bit;
};

/*  structure of FLYKBB (2886_642Ch)    */
union io_flykbb{
    unsigned long       word;
    struct {
        long            FLYKBB  :9;
        unsigned long           :23;
    }bit;
};

/*  structure of FLTDY  (2886_6430h)    */
union io_fltdy{
    unsigned long       word;
    struct {
        unsigned long   FLTDUY  :8;
        unsigned long   FLTDLY  :8;
        unsigned long           :16;
    }bit;
};

/*  structure of FLTDR  (2886_6434h)    */
union io_fltdr{
    unsigned long       word;
    struct {
        unsigned long   FLTDUR  :8;
        unsigned long   FLTDLR  :8;
        unsigned long           :16;
    }bit;
};

/*  structure of FLTDG  (2886_6438h)    */
union io_fltdg{
    unsigned long       word;
    struct {
        unsigned long   FLTDUG  :8;
        unsigned long   FLTDLG  :8;
        unsigned long           :16;
    }bit;
};

/*  structure of FLTDB  (2886_643Ch)    */
union io_fltdb{
    unsigned long       word;
    struct {
        unsigned long   FLTDUB  :8;
        unsigned long   FLTDLB  :8;
        unsigned long           :16;
    }bit;
};

/*  structure of FLMLCY (2886_6440h)    */
union io_flmlcy{
    unsigned long       word;
    struct {
        unsigned long   FLMLUCY :8;
        unsigned long   FLMLLCY :8;
        unsigned long           :16;
    }bit;
};

/*  structure of FLMLCB (2886_6444h)    */
union io_flmlcb{
    unsigned long       word;
    struct {
        unsigned long   FLMLUCB :8;
        unsigned long   FLMLLCB :8;
        unsigned long           :16;
    }bit;
};

/*  structure of FLMLCR (2886_6448h)    */
union io_flmlcr{
    unsigned long       word;
    struct {
        unsigned long   FLMLUCR :8;
        unsigned long   FLMLLCR :8;
        unsigned long           :16;
    }bit;
};

/*  structure of FLMC   (2886_6460h)    */
union io_flmc_1{
    unsigned long      word;
    struct {
        long           FLMC00  :9;
        unsigned long          :7;
        long           FLMC01  :9;
        unsigned long          :7;
    }bit;
};
union io_flmc_2{
    unsigned long      word;
    struct {
        long           FLMC02  :9;
        unsigned long          :7;
        long           FLMC10  :9;
        unsigned long          :7;
    }bit;
};
union io_flmc_3{
    unsigned long      word;
    struct {
        long           FLMC11  :9;
        unsigned long          :7;
        long           FLMC12  :9;
        unsigned long          :7;
    }bit;
};
union io_flmc_4{
    unsigned long      word;
    struct {
        long           FLMC20  :9;
        unsigned long          :7;
        long           FLMC21  :9;
        unsigned long          :7;
    }bit;
};
union io_flmc_5{
    unsigned long       word;
    struct {
        long            FLMC22  :9;
        unsigned long           :23;
    }bit;
};
struct io_flmc{
    union io_flmc_1 FLMC1;
    union io_flmc_2 FLMC2;
    union io_flmc_3 FLMC3;
    union io_flmc_4 FLMC4;
    union io_flmc_5 FLMC5;
};

/* Define i/o mapping */
struct io_flck{
    /* JDSIMG */
    union  io_fltrg     FLTRG;      /* 2886_(6400 - 6403h) */

    unsigned char dmy_6404_6407[0x6408-0x6404]; /* 2886_(6404 - 6407h) */

    union  io_flckctl   FLCKCTL;    /* 2886_(6408 - 640Bh) */
    union  io_flv       FLV;        /* 2886_(640C - 640Fh) */
    union  io_flh       FLH;        /* 2886_(6410 - 6413h) */
    union  io_flvw      FLVW;       /* 2886_(6414 - 6417h) */
    union  io_flhw      FLHW;       /* 2886_(6418 - 641Bh) */
    union  io_flbc      FLBC;       /* 2886_(641C - 641Fh) */
    union  io_flykrr    FLYKRR;     /* 2886_(6420 - 6423h) */
    union  io_flykgr    FLYKGR;     /* 2886_(6424 - 6427h) */
    union  io_flykgb    FLYKGB;     /* 2886_(6428 - 642Bh) */
    union  io_flykbb    FLYKBB;     /* 2886_(642C - 642Fh) */
    union  io_fltdy     FLTDY;      /* 2886_(6430 - 6433h) */
    union  io_fltdr     FLTDR;      /* 2886_(6434 - 6437h) */
    union  io_fltdg     FLTDG;      /* 2886_(6438 - 643Bh) */
    union  io_fltdb     FLTDB;      /* 2886_(643C - 643Fh) */
    union  io_flmlcy    FLMLCY;     /* 2886_(6440 - 6443h) */
    union  io_flmlcb    FLMLCB;     /* 2886_(6444 - 6447h) */
    union  io_flmlcr    FLMLCR;     /* 2886_(6448 - 644Bh) */

    unsigned char dmy_644C_645F[0x6460-0x644C]; /* 2886_(644C - 645Fh) */

    struct io_flmc      FLMC;       /* 2886_(6460 - 6473h) */

    unsigned char dmy_6474_64FF[0x6500-0x6474]; /* 2886_(6474 - 64FFh) */

};

#endif	// _IO_PRO_FLCK_H_
