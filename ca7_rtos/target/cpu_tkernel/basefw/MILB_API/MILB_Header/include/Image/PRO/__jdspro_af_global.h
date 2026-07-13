/**
 * @file		__jdspro_af_global.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_AF_GLOBAL_H_
#define _IO_PRO_AF_GLOBAL_H_

/*  structure of AFTRG  (2886_2000h)    */
union io_aftrg{
    unsigned long       word;
    struct {
        unsigned long   AFTRG   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of AFEBEN (2886_2004h)    */
union io_afeben{
    unsigned long       word;
    struct {
        unsigned long   ENM0    :1;
        unsigned long   ENM1    :1;
        unsigned long   ENM2    :1;
        unsigned long   ENV0    :1;
        unsigned long   ENH0    :1;
        unsigned long   ENH1    :1;
        unsigned long   ENH2    :1;
        unsigned long   ENW0    :1;
        unsigned long   ENW1    :1;
        unsigned long   ENW2    :1;
        unsigned long   ENW3    :1;
        unsigned long   ENW4    :1;
        unsigned long   ENW5    :1;
        unsigned long   ENW6    :1;
        unsigned long   ENW7    :1;
        unsigned long   ENW8    :1;
        unsigned long   ENW9    :1;
        unsigned long   ENW10   :1;
        unsigned long   ENW11   :1;
        unsigned long   ENW12   :1;
        unsigned long   ENW13   :1;
        unsigned long   ENW14   :1;
        unsigned long           :10;
    }bit;
};

/*  structure of AFINTENB   (2886_2008h)    */
union io_afintenb{
    unsigned long       word;
    struct {
        unsigned long   AFE :1;
        unsigned long       :31;
    }bit;
};

/*  structure of AFINTFLG   (2886_200Ch)    */
union io_afintflg{
    unsigned long       word;
    struct {
        unsigned long   __AFF :1;
        unsigned long         :31;
    }bit;
};

/*  structure of AFGV   (2886_2014h)    */
union io_afgv{
    unsigned long       word;
    struct {
        unsigned long   AFGV    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of AFGH   (2886_2018h)    */
union io_afgh{
    unsigned long       word;
    struct {
        unsigned long   AFGH    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of AFGVW  (2886_201Ch)    */
union io_afgvw{
    unsigned long       word;
    struct {
        unsigned long   AFGVW   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of AFGHW  (2886_2020h)    */
union io_afghw{
    unsigned long       word;
    struct {
        unsigned long   AFGHW   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of AFPAEN (2886_2024h)    */
union io_afpaen{
    unsigned long       word;
    struct {
        unsigned long   AFPAEN  :1;
        unsigned long           :31;
    }bit;
};

/* Define i/o mapping */
struct io_afgbal{
    /* JDSIMG */
    union  io_aftrg       AFTRG;      /* 2886_(2000 - 2003h) */
    union  io_afeben      AFEBEN;     /* 2886_(2004 - 2007h) */
    union  io_afintenb    AFINTENB;   /* 2886_(2008 - 200Bh) */
    union  io_afintflg    AFINTFLG;   /* 2886_(200C - 200Fh) */

    unsigned char dmy_2010_2013[0x2014-0x2010]; /* 2886_(2010 - 2013h) */

    union  io_afgv        AFGV;       /* 2886_(2014 - 2017h) */
    union  io_afgh        AFGH;       /* 2886_(2018 - 201Bh) */
    union  io_afgvw       AFGVW;      /* 2886_(201C - 201Fh) */
    union  io_afghw       AFGHW;      /* 2886_(2020 - 2023h) */
    union  io_afpaen      AFPAEN;     /* 2886_(2024 - 2027h) */

    unsigned char dmy_2028_203F[0x2040-0x2028]; /* 2886_(2028 - 203Fh) */

};

#endif	// _IO_PRO_AF_GLOBAL_H_
