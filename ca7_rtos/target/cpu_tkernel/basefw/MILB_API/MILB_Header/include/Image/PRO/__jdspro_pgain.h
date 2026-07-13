/**
 * @file		__jdspro_pgain.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_PGAIN_H_
#define _IO_PRO_PGAIN_H_

/*  structure of PGAINTRG   (2840_8600h)    */
union io_pgaintrg{
    unsigned long       word;
    struct {
        unsigned long   PGAINTRG    :2;
        unsigned long               :30;
    }bit;
};

/*  structure of PGAINR (2840_8604h)    */
union io_pgainr{
    unsigned long       word;
    struct {
        unsigned long   PGAINR  :13;
        unsigned long           :19;
    }bit;
};

/*  structure of PGAINGR    (2840_8608h)    */
union io_pgaingr{
    unsigned long       word;
    struct {
        unsigned long   PGAINGR :13;
        unsigned long           :19;
    }bit;
};

/*  structure of PGAINGB    (2840_860Ch)    */
union io_pgaingb{
    unsigned long       word;
    struct {
        unsigned long   PGAINGB :13;
        unsigned long           :19;
    }bit;
};

/*  structure of PGAINB (2840_8610h)    */
union io_pgainb{
    unsigned long       word;
    struct {
        unsigned long   PGAINB  :13;
        unsigned long           :19;
    }bit;
};

/*  structure of POFSR  (2840_8614h)    */
union io_pofsr{
    unsigned long       word;
    struct {
        long            POFSR   :13;
        unsigned long           :19;
    }bit;
};

/*  structure of POFSGR (2840_8618h)    */
union io_pofsgr{
    unsigned long       word;
    struct {
        long            POFSGR  :13;
        unsigned long           :19;
    }bit;
};

/*  structure of POFSGB (2840_861Ch)    */
union io_pofsgb{
    unsigned long       word;
    struct {
        long            POFSGB  :13;
        unsigned long           :19;
    }bit;
};

/*  structure of POFSB  (2840_8620h)    */
union io_pofsb{
    unsigned long       word;
    struct {
        long            POFSB   :13;
        unsigned long           :19;
    }bit;
};

/*  structure of PGAINDP    (2840_8624h)    */
union io_pgaindp{
    unsigned long       word;
    struct {
        unsigned long   PGAINDP :3;
        unsigned long           :29;
    }bit;
};

/* Define i/o mapping */
struct io_pgain{
    /* JDSIMG */
    union  io_pgaintrg PGAINTRG;   /* 2840_(8600 - 8603h) */
    union  io_pgainr   PGAINR;     /* 2840_(8604 - 8607h) */
    union  io_pgaingr  PGAINGR;    /* 2840_(8608 - 860Bh) */
    union  io_pgaingb  PGAINGB;    /* 2840_(860C - 860Fh) */
    union  io_pgainb   PGAINB;     /* 2840_(8610 - 8613h) */
    union  io_pofsr    POFSR;      /* 2840_(8614 - 8617h) */
    union  io_pofsgr   POFSGR;     /* 2840_(8618 - 861Bh) */
    union  io_pofsgb   POFSGB;     /* 2840_(861C - 861Fh) */
    union  io_pofsb    POFSB;      /* 2840_(8620 - 8623h) */
    union  io_pgaindp  PGAINDP;    /* 2840_(8624 - 8627h) */

    unsigned char dmy_8628_86FF[0x8700-0x8628]; /* 2840_(8628 - 86FFh) */

};

#endif	// _IO_PRO_PGAIN_H_
