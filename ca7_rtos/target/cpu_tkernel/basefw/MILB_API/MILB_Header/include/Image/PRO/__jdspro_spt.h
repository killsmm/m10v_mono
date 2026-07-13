/**
 * @file		__jdspro_spt.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_SPT_H_
#define _IO_PRO_SPT_H_

/*  structure of SPTTRG (2886_8400h)    */
union io_spttrg{
    unsigned long       word;
    struct {
        unsigned long   SPTTRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of SPTPAEN    (2886_8404h)    */
union io_sptpaen{
    unsigned long       word;
    struct {
        unsigned long   PAEN    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of SPTTBL (2886_8408h)    */
union io_spttbl{
    unsigned long       word;
    struct {
        unsigned long   SPTTBL  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of SPTBS  (2886_840Ch)    */
union io_sptbs{
    unsigned long       word;
    struct {
        unsigned long   SPTBSH  :8;
        unsigned long   SPTBSV  :8;
        unsigned long           :16;
    }bit;
};

/*  structure of SPTBD  (2886_8410h)    */
union io_sptbd{
    unsigned long       word;
    struct {
        unsigned long   SPTBDM  :9;
        unsigned long           :3;
        unsigned long   SPTBDE  :4;
        unsigned long           :16;
    }bit;
};

/*  structure of SPTV   (2886_8420h)    */
union io_sptv{
    unsigned long       word;
    struct {
        unsigned long   SPTV    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of SPTH   (2886_8424h)    */
union io_spth{
    unsigned long       word;
    struct {
        unsigned long   SPTH    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of SPTVBC (2886_8428h)    */
union io_sptvbc{
    unsigned long       word;
    struct {
        unsigned long   SPTVBC  :4;
        unsigned long           :28;
    }bit;
};

/*  structure of SPTHBC (2886_842Ch)    */
union io_spthbc{
    unsigned long       word;
    struct {
        unsigned long   SPTHBC  :4;
        unsigned long           :28;
    }bit;
};

/*  structure of SPTCLP (2886_8430h)    */
union io_sptclp{
    unsigned long       word;
    struct {
        long            SPTCLPH :15;
        unsigned long           :1;
        long            SPTCLPL :15;
        unsigned long           :1;
    }bit;
};

/*  structure of SPTNOPV    (2886_8434h)    */
union io_sptnopv{
    unsigned long       word;
    struct {
        long            SPTNOPV :15;
        unsigned long           :17;
    }bit;
};

/* Define i/o mapping */
struct io_spt{
    /* JDSIMG */
    union  io_spttrg     SPTTRG;     /* 2886_(8400 - 8403h) */
    union  io_sptpaen    SPTPAEN;    /* 2886_(8404 - 8407h) */
    union  io_spttbl     SPTTBL;     /* 2886_(8408 - 840Bh) */
    union  io_sptbs      SPTBS;      /* 2886_(840C - 840Fh) */
    union  io_sptbd      SPTBD;      /* 2886_(8410 - 8413h) */

    unsigned char dmy_8414_841F[0x8420-0x8414]; /* 2886_(8414 - 841Fh) */

    union  io_sptv       SPTV;       /* 2886_(8420 - 8423h) */
    union  io_spth       SPTH;       /* 2886_(8424 - 8427h) */
    union  io_sptvbc     SPTVBC;     /* 2886_(8428 - 842Bh) */
    union  io_spthbc     SPTHBC;     /* 2886_(842C - 842Fh) */
    union  io_sptclp     SPTCLP;     /* 2886_(8430 - 8433h) */
    union  io_sptnopv    SPTNOPV;    /* 2886_(8434 - 8437h) */

    unsigned char dmy_8438_84FF[0x8500-0x8438]; /* 2886_(8438 - 84FFh) */

};

#endif	// _IO_PRO_SPT_H_
