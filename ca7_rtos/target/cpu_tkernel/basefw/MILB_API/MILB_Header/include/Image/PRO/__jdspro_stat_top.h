/**
 * @file		__jdspro_stat_top.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_STAT_TOP_H_
#define _IO_PRO_STAT_TOP_H_

/*  structure of SR (2886_0000h)    */
union io_statsr{
    unsigned long       word;
    struct {
        unsigned long   SR  :1;
        unsigned long       :31;
    }bit;
};

/*  structure of CLKSTP (2886_0004h)    */
union io_statclkstp{
    unsigned long       word;
    struct {
        unsigned long   PSTP1   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of STATTOPCTL1    (2886_0008h)    */
union io_stattopctl1{
    unsigned long long      dword;
    struct {
        unsigned long long  AFSW0   :4;
        unsigned long long  AFSW1   :4;
        unsigned long long  FLSW0   :4;
        unsigned long long  FLSW1   :4;
        unsigned long long  AESW0   :4;
        unsigned long long  AESW1   :4;
        unsigned long long  AESW2   :4;
        unsigned long long  AESW3   :4;
        unsigned long long  HISW0   :4;
        unsigned long long  HISW1   :4;
        unsigned long long  HISW2   :4;
        unsigned long long  HISW3   :4;
        unsigned long long          :16;
    }bit;
};

/*  structure of INTMON1    (2886_0010h)    */
union io_statintmon1{
    unsigned long       word;
    struct {
        unsigned long   AFF0    :1;
        unsigned long   AFF1    :1;
        unsigned long   LINEF0  :1;
        unsigned long   LINEF1  :1;
        unsigned long   LINEF2  :1;
        unsigned long   LINEF3  :1;
        unsigned long   HISTF0  :1;
        unsigned long   HISTF1  :1;
        unsigned long   HISTF2  :1;
        unsigned long   HISTF3  :1;
        unsigned long   PWF0    :1;
        unsigned long   PWF1    :1;
        unsigned long   PWF2    :1;
        unsigned long   PWF3    :1;
        unsigned long   PWF4    :1;
        unsigned long   PWF5    :1;
        unsigned long           :16;
    }bit;
};

/*  structure of INTMON2    (2886_0014h)    */
union io_statintmon2{
    unsigned long       word;
    struct {
        unsigned long   PWXF0   :1;
        unsigned long   PWXF1   :1;
        unsigned long   PWXF2   :1;
        unsigned long   PWXF3   :1;
        unsigned long   PWXF4   :1;
        unsigned long   PWXF5   :1;
        unsigned long   PWEF0   :1;
        unsigned long   PWEF1   :1;
        unsigned long   PWEF2   :1;
        unsigned long   PWEF3   :1;
        unsigned long   PWEF4   :1;
        unsigned long   PWEF5   :1;
        unsigned long           :20;
    }bit;
};

/*  structure of INTCTL (2886_0018h)    */
union io_statintctl{
    unsigned long       word;
    struct {
        unsigned long   INTMD   :2;
        unsigned long           :30;
    }bit;
};

/* Define i/o mapping */
struct io_stattop{
    /* JDSIMG */
    union  io_statsr          SR;             /* 2886_(0000 - 0003h) */
    union  io_statclkstp      CLKSTP;         /* 2886_(0004 - 0007h) */
    union  io_stattopctl1     STATTOPCTL1;    /* 2886_(0008 - 000Fh) */
    union  io_statintmon1     INTMON1;        /* 2886_(0010 - 0013h) */
    union  io_statintmon2     INTMON2;        /* 2886_(0014 - 0017h) */
    union  io_statintctl      INTCTL;         /* 2886_(0018 - 001Bh) */

    unsigned char dmy_001C_00FF[0x0100-0x001C]; /* 2886_(001C - 00FFh) */

};

#endif	// _IO_PRO_STAT_TOP_H_
