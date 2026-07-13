/**
 * @file		__jdspro_sg.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_SG_H_
#define _IO_PRO_SG_H_

/*  structure of SGTRG  (2800_8400h)    */
union io_sgtrg{
    unsigned long       word;
    struct {
        unsigned long   SGTRG   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of SGCTL1 (2800_8404h)    */
union io_sgctl1{
    unsigned long       word;
    struct {
        unsigned long   VHDO    :1;
        unsigned long           :7;
        unsigned long   VDOT    :1;
        unsigned long           :7;
        unsigned long   VDINV   :1;
        unsigned long           :7;
        unsigned long   HDINV   :1;
        unsigned long           :7;
    }bit;
};

/*  structure of SGCTL2 (2800_8408h)    */
union io_sgctl2{
    unsigned long       word;
    struct {
        unsigned long   HDCHG   :1;
        unsigned long           :7;
        unsigned long   VDDLY   :8;
        unsigned long           :16;
    }bit;
};

/*  structure of VDOCYC (2800_840Ch)    */
union io_vdocyc{
    unsigned long       word;
    struct {
        unsigned long   VDOCYC  :24;
        unsigned long           :8;
    }bit;
};

/*  structure of VDOW   (2800_8410h)    */
union io_vdow{
    unsigned long       word;
    struct {
        unsigned long   VDOW    :9;
        unsigned long           :23;
    }bit;
};

/*  structure of HDOCYC (2800_8414h)    */
union io_hdocyc{
    unsigned long       word;
    struct {
        unsigned long   HDOCYC  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of HDOW   (2800_8418h)    */
union io_hdow{
    unsigned long       word;
    struct {
        unsigned long   HDOW    :9;
        unsigned long           :23;
    }bit;
};

/*  structure of HDOCYC2    (2800_841Ch)    */
union io_hdocyc2{
    unsigned long       word;
    struct {
        unsigned long   HDOCYC2 :14;
        unsigned long           :18;
    }bit;
};

/*  structure of HDOW2  (2800_8420h)    */
union io_hdow2{
    unsigned long       word;
    struct {
        unsigned long   HDOW2   :9;
        unsigned long           :23;
    }bit;
};

/*  structure of VDHDMON    (2800_8424h)    */
union io_vdhdmon{
    unsigned long       word;
    struct {
        unsigned long   HDIN    :1;
        unsigned long           :3;
        unsigned long   VDIN    :1;
        unsigned long           :27;
    }bit;
};

/* Define i/o mapping */
struct io_sg{
    /* JDSIMG */
    union  io_sgtrg   SGTRG;      /* 2800_(8400 - 8403h) */
    union  io_sgctl1  SGCTL1;     /* 2800_(8404 - 8407h) */
    union  io_sgctl2  SGCTL2;     /* 2800_(8408 - 840Bh) */
    union  io_vdocyc  VDOCYC;     /* 2800_(840C - 840Fh) */
    union  io_vdow    VDOW;       /* 2800_(8410 - 8413h) */
    union  io_hdocyc  HDOCYC;     /* 2800_(8414 - 8417h) */
    union  io_hdow    HDOW;       /* 2800_(8418 - 841Bh) */
    union  io_hdocyc2 HDOCYC2;    /* 2800_(841C - 841Fh) */
    union  io_hdow2   HDOW2;      /* 2800_(8420 - 8423h) */
    union  io_vdhdmon VDHDMON;    /* 2800_(8424 - 8427h) */

    unsigned char dmy_8428_84FF[0x8500-0x8428]; /* 2800_(8428 - 84FFh) */

};

#endif	// _IO_PRO_SG_H_
