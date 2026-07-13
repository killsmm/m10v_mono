/**
 * @file        __jdspro_lnr.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_LNR_H_
#define _IO_PRO_LNR_H_

/*  structure of LNRTRG (2800_AC00h)    */
union io_lnrtrg{
    unsigned long       word;
    struct {
        unsigned long   LNRTRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of LNRBR  (2800_AC10h)    */
union io_lnrbr_1{
    unsigned long       word;
    struct {
        unsigned long   LNRBR_1 :14;
        unsigned long           :2;
        unsigned long   LNRBR_2 :14;
        unsigned long           :2;
    }bit;
};
union io_lnrbr_2{
    unsigned long       word;
    struct {
        unsigned long   LNRBR_3 :14;
        unsigned long           :2;
        unsigned long   LNRBR_4 :14;
        unsigned long           :2;
    }bit;
};
union io_lnrbr_3{
    unsigned long       word;
    struct {
        unsigned long   LNRBR_5 :14;
        unsigned long           :2;
        unsigned long   LNRBR_6 :14;
        unsigned long           :2;
    }bit;
};
union io_lnrbr_4{
    unsigned long       word;
    struct {
        unsigned long   LNRBR_7 :14;
        unsigned long           :2;
        unsigned long           :16;
    }bit;
};
struct io_lnrbr{
    union io_lnrbr_1 LNRBR1;
    union io_lnrbr_2 LNRBR2;
    union io_lnrbr_3 LNRBR3;
    union io_lnrbr_4 LNRBR4;
};

/*  structure of LNRINCR    (2800_AC20h)    */
union io_lnrincr_1{
    unsigned long       word;
    struct {
        unsigned long   LNRINCR_0   :10;
        unsigned long               :6;
        unsigned long   LNRINCR_1   :10;
        unsigned long               :6;
    }bit;
};
union io_lnrincr_2{
    unsigned long       word;
    struct {
        unsigned long   LNRINCR_2   :10;
        unsigned long               :6;
        unsigned long   LNRINCR_3   :10;
        unsigned long               :6;
    }bit;
};
union io_lnrincr_3{
    unsigned long       word;
    struct {
        unsigned long   LNRINCR_4   :10;
        unsigned long               :6;
        unsigned long   LNRINCR_5   :10;
        unsigned long               :6;
    }bit;
};
union io_lnrincr_4{
    unsigned long       word;
    struct {
        unsigned long   LNRINCR_6   :10;
        unsigned long               :6;
        unsigned long   LNRINCR_7   :10;
        unsigned long               :6;
    }bit;
};
struct io_lnrincr{
    union io_lnrincr_1   LNRINCR1;
    union io_lnrincr_2   LNRINCR2;
    union io_lnrincr_3   LNRINCR3;
    union io_lnrincr_4   LNRINCR4;
};

/*  structure of LNROFSR    (2800_AC30h)    */
union io_lnrofsr_1{
    unsigned long       word;
    struct {
        unsigned long   LNROFSR_0   :14;
        unsigned long               :2;
        unsigned long   LNROFSR_1   :14;
        unsigned long               :2;
    }bit;
};
union io_lnrofsr_2{
    unsigned long       word;
    struct {
        unsigned long   LNROFSR_2   :14;
        unsigned long               :2;
        unsigned long   LNROFSR_3   :14;
        unsigned long               :2;
    }bit;
};
union io_lnrofsr_3{
    unsigned long       word;
    struct {
        unsigned long   LNROFSR_4   :14;
        unsigned long               :2;
        unsigned long   LNROFSR_5   :14;
        unsigned long               :2;
    }bit;
};
union io_lnrofsr_4{
    unsigned long       word;
    struct {
        unsigned long   LNROFSR_6   :14;
        unsigned long               :2;
        unsigned long   LNROFSR_7   :14;
        unsigned long               :2;
    }bit;
};
struct io_lnrofsr{
    union io_lnrofsr_1   LNROFSR1;
    union io_lnrofsr_2   LNROFSR2;
    union io_lnrofsr_3   LNROFSR3;
    union io_lnrofsr_4   LNROFSR4;
};

/*  structure of LNRBGR (2800_AC40h)    */
union io_lnrbgr_1{
    unsigned long       word;
    struct {
        unsigned long   LNRBGR_1    :14;
        unsigned long               :2;
        unsigned long   LNRBGR_2    :14;
        unsigned long               :2;
    }bit;
};
union io_lnrbgr_2{
    unsigned long       word;
    struct {
        unsigned long   LNRBGR_3    :14;
        unsigned long               :2;
        unsigned long   LNRBGR_4    :14;
        unsigned long               :2;
    }bit;
};
union io_lnrbgr_3{
    unsigned long       word;
    struct {
        unsigned long   LNRBGR_5    :14;
        unsigned long               :2;
        unsigned long   LNRBGR_6    :14;
        unsigned long               :2;
    }bit;
};
union io_lnrbgr_4{
    unsigned long       word;
    struct {
        unsigned long   LNRBGR_7    :14;
        unsigned long               :2;
        unsigned long               :16;
    }bit;
};
struct io_lnrbgr{
    union io_lnrbgr_1    LNRBGR1;
    union io_lnrbgr_2    LNRBGR2;
    union io_lnrbgr_3    LNRBGR3;
    union io_lnrbgr_4    LNRBGR4;
};

/*  structure of LNRINCGR   (2800_AC50h)    */
union io_lnrincgr_1{
    unsigned long       word;
    struct {
        unsigned long   LNRINCGR_0  :10;
        unsigned long               :6;
        unsigned long   LNRINCGR_1  :10;
        unsigned long               :6;
    }bit;
};
union io_lnrincgr_2{
    unsigned long       word;
    struct {
        unsigned long   LNRINCGR_2  :10;
        unsigned long               :6;
        unsigned long   LNRINCGR_3  :10;
        unsigned long               :6;
    }bit;
};
union io_lnrincgr_3{
    unsigned long       word;
    struct {
        unsigned long   LNRINCGR_4  :10;
        unsigned long               :6;
        unsigned long   LNRINCGR_5  :10;
        unsigned long               :6;
    }bit;
};
union io_lnrincgr_4{
    unsigned long       word;
    struct {
        unsigned long   LNRINCGR_6  :10;
        unsigned long               :6;
        unsigned long   LNRINCGR_7  :10;
        unsigned long               :6;
    }bit;
};
struct io_lnrincgr{
    union io_lnrincgr_1  LNRINCGR1;
    union io_lnrincgr_2  LNRINCGR2;
    union io_lnrincgr_3  LNRINCGR3;
    union io_lnrincgr_4  LNRINCGR4;
};

/*  structure of LNROFSGR   (2800_AC60h)    */
union io_lnrofsgr_1{
    unsigned long       word;
    struct {
        unsigned long   LNROFSGR_0  :14;
        unsigned long               :2;
        unsigned long   LNROFSGR_1  :14;
        unsigned long               :2;
    }bit;
};
union io_lnrofsgr_2{
    unsigned long       word;
    struct {
        unsigned long   LNROFSGR_2  :14;
        unsigned long               :2;
        unsigned long   LNROFSGR_3  :14;
        unsigned long               :2;
    }bit;
};
union io_lnrofsgr_3{
    unsigned long       word;
    struct {
        unsigned long   LNROFSGR_4  :14;
        unsigned long               :2;
        unsigned long   LNROFSGR_5  :14;
        unsigned long               :2;
    }bit;
};
union io_lnrofsgr_4{
    unsigned long       word;
    struct {
        unsigned long   LNROFSGR_6  :14;
        unsigned long               :2;
        unsigned long   LNROFSGR_7  :14;
        unsigned long               :2;
    }bit;
};
struct io_lnrofsgr{
    union io_lnrofsgr_1  LNROFSGR1;
    union io_lnrofsgr_2  LNROFSGR2;
    union io_lnrofsgr_3  LNROFSGR3;
    union io_lnrofsgr_4  LNROFSGR4;
};

/*  structure of LNRBGB (2800_AC70h)    */
union io_lnrbgb_1{
    unsigned long       word;
    struct {
        unsigned long   LNRBGB_1    :14;
        unsigned long               :2;
        unsigned long   LNRBGB_2    :14;
        unsigned long               :2;
    }bit;
};
union io_lnrbgb_2{
    unsigned long       word;
    struct {
        unsigned long   LNRBGB_3    :14;
        unsigned long               :2;
        unsigned long   LNRBGB_4    :14;
        unsigned long               :2;
    }bit;
};
union io_lnrbgb_3{
    unsigned long       word;
    struct {
        unsigned long   LNRBGB_5    :14;
        unsigned long               :2;
        unsigned long   LNRBGB_6    :14;
        unsigned long               :2;
    }bit;
};
union io_lnrbgb_4{
    unsigned long       word;
    struct {
        unsigned long   LNRBGB_7    :14;
        unsigned long               :2;
        unsigned long               :16;
    }bit;
};
struct io_lnrbgb{
    union io_lnrbgb_1    LNRBGB1;
    union io_lnrbgb_2    LNRBGB2;
    union io_lnrbgb_3    LNRBGB3;
    union io_lnrbgb_4    LNRBGB4;
};

/*  structure of LNRINCGB   (2800_AC80h)    */
union io_lnrincgb_1{
    unsigned long       word;
    struct {
        unsigned long   LNRINCGB_0  :10;
        unsigned long               :6;
        unsigned long   LNRINCGB_1  :10;
        unsigned long               :6;
    }bit;
};
union io_lnrincgb_2{
    unsigned long       word;
    struct {
        unsigned long   LNRINCGB_2  :10;
        unsigned long               :6;
        unsigned long   LNRINCGB_3  :10;
        unsigned long               :6;
    }bit;
};
union io_lnrincgb_3{
    unsigned long       word;
    struct {
        unsigned long   LNRINCGB_4  :10;
        unsigned long               :6;
        unsigned long   LNRINCGB_5  :10;
        unsigned long               :6;
    }bit;
};
union io_lnrincgb_4{
    unsigned long       word;
    struct {
        unsigned long   LNRINCGB_6  :10;
        unsigned long               :6;
        unsigned long   LNRINCGB_7  :10;
        unsigned long               :6;
    }bit;
};
struct io_lnrincgb{
    union io_lnrincgb_1  LNRINCGB1;
    union io_lnrincgb_2  LNRINCGB2;
    union io_lnrincgb_3  LNRINCGB3;
    union io_lnrincgb_4  LNRINCGB4;
};

/*  structure of LNROFSGB   (2800_AC90h)    */
union io_lnrofsgb_1{
    unsigned long       word;
    struct {
        unsigned long   LNROFSGB_0  :14;
        unsigned long               :2;
        unsigned long   LNROFSGB_1  :14;
        unsigned long               :2;
    }bit;
};
union io_lnrofsgb_2{
    unsigned long       word;
    struct {
        unsigned long   LNROFSGB_2  :14;
        unsigned long               :2;
        unsigned long   LNROFSGB_3  :14;
        unsigned long               :2;
    }bit;
};
union io_lnrofsgb_3{
    unsigned long       word;
    struct {
        unsigned long   LNROFSGB_4  :14;
        unsigned long               :2;
        unsigned long   LNROFSGB_5  :14;
        unsigned long               :2;
    }bit;
};
union io_lnrofsgb_4{
    unsigned long       word;
    struct {
        unsigned long   LNROFSGB_6  :14;
        unsigned long               :2;
        unsigned long   LNROFSGB_7  :14;
        unsigned long               :2;
    }bit;
};
struct io_lnrofsgb{
    union io_lnrofsgb_1  LNROFSGB1;
    union io_lnrofsgb_2  LNROFSGB2;
    union io_lnrofsgb_3  LNROFSGB3;
    union io_lnrofsgb_4  LNROFSGB4;
};

/*  structure of LNRBB  (2800_ACA0h)    */
union io_lnrbb_1{
    unsigned long       word;
    struct {
        unsigned long   LNRBB_1 :14;
        unsigned long           :2;
        unsigned long   LNRBB_2 :14;
        unsigned long           :2;
    }bit;
};
union io_lnrbb_2{
    unsigned long       word;
    struct {
        unsigned long   LNRBB_3 :14;
        unsigned long           :2;
        unsigned long   LNRBB_4 :14;
        unsigned long           :2;
    }bit;
};
union io_lnrbb_3{
    unsigned long       word;
    struct {
        unsigned long   LNRBB_5 :14;
        unsigned long           :2;
        unsigned long   LNRBB_6 :14;
        unsigned long           :2;
    }bit;
};
union io_lnrbb_4{
    unsigned long       word;
    struct {
        unsigned long   LNRBB_7 :14;
        unsigned long           :2;
        unsigned long           :16;
    }bit;
};
struct io_lnrbb{
    union io_lnrbb_1 LNRBB1;
    union io_lnrbb_2 LNRBB2;
    union io_lnrbb_3 LNRBB3;
    union io_lnrbb_4 LNRBB4;
};

/*  structure of LNRINCB    (2800_ACB0h)    */
union io_lnrincb_1{
    unsigned long       word;
    struct {
        unsigned long   LNRINCB_0   :10;
        unsigned long               :6;
        unsigned long   LNRINCB_1   :10;
        unsigned long               :6;
    }bit;
};
union io_lnrincb_2{
    unsigned long       word;
    struct {
        unsigned long   LNRINCB_2   :10;
        unsigned long               :6;
        unsigned long   LNRINCB_3   :10;
        unsigned long               :6;
    }bit;
};
union io_lnrincb_3{
    unsigned long       word;
    struct {
        unsigned long   LNRINCB_4   :10;
        unsigned long               :6;
        unsigned long   LNRINCB_5   :10;
        unsigned long               :6;
    }bit;
};
union io_lnrincb_4{
    unsigned long       word;
    struct {
        unsigned long   LNRINCB_6   :10;
        unsigned long               :6;
        unsigned long   LNRINCB_7   :10;
        unsigned long               :6;
    }bit;
};
struct io_lnrincb{
    union io_lnrincb_1   LNRINCB1;
    union io_lnrincb_2   LNRINCB2;
    union io_lnrincb_3   LNRINCB3;
    union io_lnrincb_4   LNRINCB4;
};

/*  structure of LNROFSB    (2800_ACC0h)    */
union io_lnrofsb_1{
    unsigned long       word;
    struct {
        unsigned long   LNROFSB_0   :14;
        unsigned long               :2;
        unsigned long   LNROFSB_1   :14;
        unsigned long               :2;
    }bit;
};
union io_lnrofsb_2{
    unsigned long       word;
    struct {
        unsigned long   LNROFSB_2   :14;
        unsigned long               :2;
        unsigned long   LNROFSB_3   :14;
        unsigned long               :2;
    }bit;
};
union io_lnrofsb_3{
    unsigned long       word;
    struct {
        unsigned long   LNROFSB_4   :14;
        unsigned long               :2;
        unsigned long   LNROFSB_5   :14;
        unsigned long               :2;
    }bit;
};
union io_lnrofsb_4{
    unsigned long       word;
    struct {
        unsigned long   LNROFSB_6   :14;
        unsigned long               :2;
        unsigned long   LNROFSB_7   :14;
        unsigned long               :2;
    }bit;
};
struct io_lnrofsb{
    union io_lnrofsb_1   LNROFSB1;
    union io_lnrofsb_2   LNROFSB2;
    union io_lnrofsb_3   LNROFSB3;
    union io_lnrofsb_4   LNROFSB4;
};

/* Define i/o mapping */
struct io_lnr{
    /* JDSIMG */
    union  io_lnrtrg     LNRTRG;                /* 2800_(AC00 - AC03h) */

    unsigned char dmy_AC04_AC0F[0xAC10-0xAC04]; /* 2800_(AC04 - AC0Fh) */

    struct io_lnrbr      LNRBR;                 /* 2800_(AC10 - AC1Fh) */
    struct io_lnrincr    LNRINCR;               /* 2800_(AC20 - AC2Fh) */
    struct io_lnrofsr    LNROFSR;               /* 2800_(AC30 - AC3Fh) */
    struct io_lnrbgr     LNRBGR;                /* 2800_(AC40 - AC4Fh) */
    struct io_lnrincgr   LNRINCGR;              /* 2800_(AC50 - AC5Fh) */
    struct io_lnrofsgr   LNROFSGR;              /* 2800_(AC60 - AC6Fh) */
    struct io_lnrbgb     LNRBGB;                /* 2800_(AC70 - AC7Fh) */
    struct io_lnrincgb   LNRINCGB;              /* 2800_(AC80 - AC8Fh) */
    struct io_lnrofsgb   LNROFSGB;              /* 2800_(AC90 - AC9Fh) */
    struct io_lnrbb      LNRBB;                 /* 2800_(ACA0 - ACAFh) */
    struct io_lnrincb    LNRINCB;               /* 2800_(ACB0 - ACBFh) */
    struct io_lnrofsb    LNROFSB;               /* 2800_(ACC0 - ACCFh) */

    unsigned char dmy_ACD0_ACFF[0xAD00-0xACD0]; /* 2800_(ACD0 - ACFFh) */
};

#endif  // _IO_PRO_LNR_H_
