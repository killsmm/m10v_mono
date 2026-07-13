/**
 * @file        __jdspro_ff.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_FF_H_
#define _IO_PRO_FF_H_

/*  structure of FFTRG  (2840_8400h)    */
union io_fftrg{
    unsigned long       word;
    struct {
        unsigned long   FFTRG   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of FFV    (2840_8408h)    */
union io_ffv{
    unsigned long       word;
    struct {
        unsigned long   FFV     :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FFH    (2840_840Ch)    */
union io_ffh{
    unsigned long       word;
    struct {
        unsigned long   FFH     :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FFVW   (2840_8410h)    */
union io_ffvw{
    unsigned long       word;
    struct {
        unsigned long   FFVW    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FFHW   (2840_8414h)    */
union io_ffhw{
    unsigned long       word;
    struct {
        unsigned long   FFHW    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FFLPB0 (2840_8418h)    */
union io_fflpb0{
    unsigned long       word;
    struct {
        unsigned long   FFLPB0  :8;
        unsigned long           :24;
    }bit;
};

/*  structure of FFLPB1 (2840_841Ch)    */
union io_fflpb1{
    unsigned long       word;
    struct {
        unsigned long   FFLPB1  :8;
        unsigned long           :24;
    }bit;
};

/*  structure of FFLPB2 (2840_8420h)    */
union io_fflpb2{
    unsigned long       word;
    struct {
        unsigned long   FFLPB2  :8;
        unsigned long           :24;
    }bit;
};

/*  structure of FFDEFTS (2840_8424h)    */
union io_ffdefts{
    unsigned long       word;
    struct {
        unsigned long   FFDEFTS :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FFMNWGT (2840_8424h)    */
union io_ffmnwgt{
    unsigned long       word;
    struct {
        long            FFMNWGT :10;
        unsigned long           :22;
    }bit;
};

/*  structure of FFSBWGT (2840_8424h)    */
union io_ffsbwgt{
    unsigned long       word;
    struct {
        long            FFSBWGT :10;
        unsigned long           :22;
    }bit;
};


/* Define i/o mapping */
struct io_ff{
    /* JDSIMG */
    union  io_fftrg      FFTRG;                 /* 2840_(8400 - 8403h) */

    unsigned char dmy_8404_8408[0x8408-0x8404]; /* 2840_(8404 - 8407h) */

    union  io_ffv        FFV;                   /* 2840_(8408 - 840Bh) */
    union  io_ffh        FFH;                   /* 2840_(840C - 840Fh) */
    union  io_ffvw       FFVW;                  /* 2840_(8410 - 8413h) */
    union  io_ffhw       FFHW;                  /* 2840_(8414 - 8417h) */
    union  io_fflpb0     FFLPB0;                /* 2840_(8418 - 841Bh) */
    union  io_fflpb1     FFLPB1;                /* 2840_(841C - 841Fh) */
    union  io_fflpb2     FFLPB2;                /* 2840_(8420 - 8423h) */
    union  io_ffdefts    FFDEFTS;               /* 2840_(8424 - 8427h) */
    union  io_ffmnwgt    FFMNWGT;               /* 2840_(8428 - 842Bh) */
    union  io_ffsbwgt    FFSBWGT;               /* 2840_(842C - 842Fh) */

    unsigned char dmy_8430_84FF[0x8500-0x8430]; /* 2840_(8430 - 84FFh) */

};

#endif  // _IO_PRO_FF_H_
