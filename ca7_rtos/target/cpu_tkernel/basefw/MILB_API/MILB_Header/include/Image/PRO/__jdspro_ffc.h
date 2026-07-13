/**
 * @file		__jdspro_ffc.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_FFC_H_
#define _IO_PRO_FFC_H_

/*  structure of FCTRG  (2840_9700h)    */
union io_ffc_fctrg{
    unsigned long       word;
    struct {
        unsigned long   FCTRG   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of FCPAEN (2840_9704h)    */
union io_ffc_fcpaen{
    unsigned long       word;
    struct {
        unsigned long   FCPAEN  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of FCV    (2840_970Ch)    */
union io_ffc_fcv{
    unsigned long       word;
    struct {
        unsigned long   FCV :14;
        unsigned long       :18;
    }bit;
};

/*  structure of FCH    (2840_9710h)    */
union io_ffc_fch{
    unsigned long       word;
    struct {
        unsigned long   FCH :14;
        unsigned long       :18;
    }bit;
};

/*  structure of FCVW   (2840_9714h)    */
union io_ffc_fcvw{
    unsigned long       word;
    struct {
        unsigned long   FCVW    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FCHW   (2840_9718h)    */
union io_ffc_fchw{
    unsigned long       word;
    struct {
        unsigned long   FCHW    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FCBC   (2840_971Ch)    */
union io_ffc_fcbc{
    unsigned long       word;
    struct {
        long            FCBC0   :10;
        unsigned long           :6;
        long            FCBC1   :10;
        unsigned long           :6;
    }bit;
};

/*  structure of FCSUBPR    (2840_9734h)    */
union io_ffc_fcsubpr{
    unsigned long       word;
    struct {
        unsigned long   FCSUBPRH    :11;
        unsigned long               :5;
        unsigned long   FCSUBPRV    :11;
        unsigned long               :5;
    }bit;
};

/*  structure of FCSUBSP    (2840_9738h)    */
union io_ffc_fcsubsp{
    unsigned long       word;
    struct {
        unsigned long   FCSUBSPH    :10;
        unsigned long               :6;
        unsigned long   FCSUBSPV    :10;
        unsigned long               :6;
    }bit;
};

/*  structure of FCSUBDR    (2840_973Ch)    */
union io_ffc_fcsubdr{
    unsigned long       word;
    struct {
        unsigned long   FCSUBDRM    :16;
        unsigned long   FCSUBDRE    :4;
        unsigned long               :12;
    }bit;
};

/*  structure of FCSUBST    (2840_9740h)    */
union io_ffc_fcsubst{
    unsigned long       word;
    struct {
        unsigned long   FCSUBSTH    :10;
        unsigned long               :6;
        unsigned long   FCSUBSTV    :10;
        unsigned long               :6;
    }bit;
};

/*  structure of FCWRIC (2840_9788h)    */
union io_ffc_fcwric{
    unsigned long       word;
    struct {
        unsigned long   FCWRIC  :13;
        unsigned long           :19;
    }bit;
};

/* Define i/o mapping */
struct io_ffc{
    /* JDSIMG */
    union  io_ffc_fctrg      FCTRG;      /* 2840_(9700 - 9703h) */
    union  io_ffc_fcpaen     FCPAEN;     /* 2840_(9704 - 9707h) */

    unsigned char dmy_9708_970B[0x970C-0x9708]; /* 2840_(9708 - 970Bh) */

    union  io_ffc_fcv        FCV;        /* 2840_(970C - 970Fh) */
    union  io_ffc_fch        FCH;        /* 2840_(9710 - 9713h) */
    union  io_ffc_fcvw       FCVW;       /* 2840_(9714 - 9717h) */
    union  io_ffc_fchw       FCHW;       /* 2840_(9718 - 971Bh) */
    union  io_ffc_fcbc       FCBC;       /* 2840_(971C - 971Fh) */

    unsigned char dmy_9720_9733[0x9734-0x9720]; /* 2840_(9720 - 9733h) */

    union  io_ffc_fcsubpr    FCSUBPR;    /* 2840_(9734 - 9737h) */
    union  io_ffc_fcsubsp    FCSUBSP;    /* 2840_(9738 - 973Bh) */
    union  io_ffc_fcsubdr    FCSUBDR;    /* 2840_(973C - 973Fh) */
    union  io_ffc_fcsubst    FCSUBST;    /* 2840_(9740 - 9743h) */

    unsigned char dmy_9744_9787[0x9788-0x9744]; /* 2840_(9744 - 9787h) */

    union  io_ffc_fcwric     FCWRIC;     /* 2840_(9788 - 978Bh) */

    unsigned char dmy_978C_97FF[0x9800-0x978C]; /* 2840_(978C - 97FFh) */

};

#endif	// _IO_PRO_FFC_H_
