/**
 * @file		__jdspro_afpb.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_AFPB_H_
#define _IO_PRO_AFPB_H_

/*  structure of AFPBTRG    (2840_9400h)    */
union io_afpbtrg{
    unsigned long       word;
    struct {
        unsigned long   AFPBTRG :2;
        unsigned long           :30;
    }bit;
};

/*  structure of AFPBV  (2840_9404h)    */
union io_afpbv{
    unsigned long       word;
    struct {
        unsigned long   AFPBV   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of AFPBH  (2840_9408h)    */
union io_afpbh{
    unsigned long       word;
    struct {
        unsigned long   AFPBH   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of AFPBVW (2840_940Ch)    */
union io_afpbvw{
    unsigned long       word;
    struct {
        unsigned long   AFPBVW  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of AFPBHW (2840_9410h)    */
union io_afpbhw{
    unsigned long       word;
    struct {
        unsigned long   AFPBHW  :14;
        unsigned long           :18;
    }bit;
};

/* Define i/o mapping */
struct io_afpb{
    /* JDSIMG */
    union  io_afpbtrg   AFPBTRG;    /* 2840_(9400 - 9403h) */
    union  io_afpbv     AFPBV;      /* 2840_(9404 - 9407h) */
    union  io_afpbh     AFPBH;      /* 2840_(9408 - 940Bh) */
    union  io_afpbvw    AFPBVW;     /* 2840_(940C - 940Fh) */
    union  io_afpbhw    AFPBHW;     /* 2840_(9410 - 9413h) */

    unsigned char dmy_9414_94FF[0x9500-0x9414]; /* 2840_(9414 - 94FFh) */

};

#endif	// _IO_PRO_AFPB_H_
