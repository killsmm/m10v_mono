/**
 * @file		__jdspro_b2b_top.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_B2B_TOP_H_
#define _IO_PRO_B2B_TOP_H_

/*  structure of SR (2841_0000h)    */
union io_b2b_sr{
    unsigned long       word;
    struct {
        unsigned long   SR  :1;
        unsigned long       :31;
    }bit;
};

/*  structure of CLKSTP (2841_0004h)    */
union io_b2b_clkstp{
    unsigned long       word;
    struct {
        unsigned long   PSTP1   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of B2BTOPCTL1 (2841_0008h)    */
union io_b2btopctl1{
    unsigned long       word;
    struct {
        unsigned long   B2BISW  :1;
        unsigned long           :3;
        unsigned long   CAGSL   :1;
        unsigned long           :27;
    }bit;
};

/*  structure of B2BTOPCTL2 (2841_000Ch)    */
union io_b2btopctl2{
    unsigned long       word;
    struct {
        unsigned long   ORG0    :2;
        unsigned long           :6;
        unsigned long   ORG1    :2;
        unsigned long           :22;
    }bit;
};

/*  structure of INTMON1    (2841_0010h)    */
union io_b2b_intmon1{
    unsigned long       word;
    struct {
        unsigned long   PRF0    :1;
        unsigned long   PRF1    :1;
        unsigned long   PRF2    :1;
        unsigned long   PRF3    :1;
        unsigned long   PRF4    :1;
        unsigned long   PWF0    :1;
        unsigned long   PWF1    :1;
        unsigned long           :25;
    }bit;
};

/*  structure of INTMON2    (2841_0014h)    */
union io_b2b_intmon2{
    unsigned long       word;
    struct {
        unsigned long   PRXF0   :1;
        unsigned long   PRXF1   :1;
        unsigned long   PRXF2   :1;
        unsigned long   PRXF3   :1;
        unsigned long   PRXF4   :1;
        unsigned long   PREF0   :1;
        unsigned long   PREF1   :1;
        unsigned long   PREF2   :1;
        unsigned long   PREF3   :1;
        unsigned long   PREF4   :1;
        unsigned long   PWXF0   :1;
        unsigned long   PWXF1   :1;
        unsigned long   PWEF0   :1;
        unsigned long   PWEF1   :1;
        unsigned long           :18;
    }bit;
};

/*  structure of INTCTL (2841_0018h)    */
union io_b2b_intctl{
    unsigned long       word;
    struct {
        unsigned long   INTMD   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of VHDINTENB  (2841_001Ch)    */
union io_b2b_vhdintenb{
    unsigned long       word;
    struct {
        unsigned long   VDE0    :1;
        unsigned long           :3;
        unsigned long   VDE1    :1;
        unsigned long           :3;
        unsigned long   HDE0    :1;
        unsigned long           :3;
        unsigned long   HDE1    :1;
        unsigned long           :19;
    }bit;
};

/*  structure of VHDINTFLG  (2841_0020h)    */
union io_b2b_vhdintflg{
    unsigned long       word;
    struct {
        unsigned long   __VDF0    :1;
        unsigned long             :3;
        unsigned long   __VDF1    :1;
        unsigned long             :3;
        unsigned long   __HDF0    :1;
        unsigned long             :3;
        unsigned long   __HDF1    :1;
        unsigned long             :19;
    }bit;
};

/*  structure of VHSET  (2841_0028h)    */
union io_b2b_vhset{
    unsigned long long      dword;
    struct {
        unsigned long long  VHSET0  :1;
        unsigned long long          :31;
        unsigned long long  VHSET1  :1;
        unsigned long long          :31;
    }bit;
};

/*  structure of VMIVADR    (2841_0030h)    */
union io_b2b_vmivadr{
    unsigned long       word;
    struct {
        unsigned long   VMIVADR0    :14;
        unsigned long               :2;
        unsigned long   VMIVADR1    :14;
        unsigned long               :2;
    }bit;
};

/*  structure of VMIHADR    (2841_0034h)    */
union io_b2b_vmihadr{
    unsigned long       word;
    struct {
        unsigned long   VMIHADR0    :14;
        unsigned long               :2;
        unsigned long   VMIHADR1    :14;
        unsigned long               :2;
    }bit;
};

/*  structure of HMIHADR    (2841_0038h)    */
union io_b2b_hmihadr{
    unsigned long       word;
    struct {
        unsigned long   HMIHADR0    :14;
        unsigned long               :2;
        unsigned long   HMIHADR1    :14;
        unsigned long               :2;
    }bit;
};

/* Define i/o mapping */
struct io_b2btop{
    /* JDSIMG */
    union  io_b2b_sr           SR;         /* 2841_(0000 - 0003h) */
    union  io_b2b_clkstp       CLKSTP;     /* 2841_(0004 - 0007h) */
    union  io_b2btopctl1       B2BTOPCTL1; /* 2841_(0008 - 000Bh) */
    union  io_b2btopctl2       B2BTOPCTL2; /* 2841_(000C - 000Fh) */
    union  io_b2b_intmon1      INTMON1;    /* 2841_(0010 - 0013h) */
    union  io_b2b_intmon2      INTMON2;    /* 2841_(0014 - 0017h) */
    union  io_b2b_intctl       INTCTL;     /* 2841_(0018 - 001Bh) */
    union  io_b2b_vhdintenb    VHDINTENB;  /* 2841_(001C - 001Fh) */
    union  io_b2b_vhdintflg    VHDINTFLG;  /* 2841_(0020 - 0023h) */

    unsigned char dmy_0024_0027[0x0028-0x0024]; /* 2841_(0024 - 0027h) */

    union  io_b2b_vhset        VHSET;      /* 2841_(0028 - 002Fh) */
    union  io_b2b_vmivadr      VMIVADR;    /* 2841_(0030 - 0033h) */
    union  io_b2b_vmihadr      VMIHADR;    /* 2841_(0034 - 0037h) */
    union  io_b2b_hmihadr      HMIHADR;    /* 2841_(0038 - 003Bh) */

    unsigned char dmy_003C_00FF[0x0100-0x003C]; /* 2841_(003C - 00FFh) */

};

#endif	// _IO_PRO_B2B_TOP_H_
