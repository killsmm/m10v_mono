/**
 * @file		__jdspro_pas_top.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_PAS_TOP_H_
#define _IO_PRO_PAS_TOP_H_

/*  structure of SR (2886_8000h)    */
union io_pas_sr{
    unsigned long       word;
    struct {
        unsigned long   SR  :1;
        unsigned long       :31;
    }bit;
};

/*  structure of CLKSTP (2886_8004h)    */
union io_pas_clkstp{
    unsigned long       word;
    struct {
        unsigned long   PSTP1   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of PASTOPCTL1 (2886_8008h)    */
union io_pastopctl1{
    unsigned long       word;
    struct {
        unsigned long   ORG :2;
        unsigned long       :30;
    }bit;
};

/*  structure of INTMON1    (2886_800Ch)    */
union io_pas_intmon1{
    unsigned long       word;
    struct {
        unsigned long   PRF0    :1;
        unsigned long   PWF0    :1;
        unsigned long           :30;
    }bit;
};

/*  structure of INTMON2    (2886_8010h)    */
union io_pas_intmon2{
    unsigned long       word;
    struct {
        unsigned long   PRXF0   :1;
        unsigned long   PREF0   :1;
        unsigned long   PWXF0   :1;
        unsigned long   PWEF0   :1;
        unsigned long           :28;
    }bit;
};

/*  structure of INTCTL (2886_8014h)    */
union io_pas_intctl{
    unsigned long       word;
    struct {
        unsigned long   INTMD   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of VHDINTENB  (2886_8018h)    */
union io_pas_vhdintenb{
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

/*  structure of VHDINTFLG  (2886_801Ch)    */
union io_pas_vhdintflg{
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

/*  structure of VHSET  (2886_8020h)    */
union io_pas_vhset{
    unsigned long long      dword;
    struct {
        unsigned long long  VHSET0  :1;
        unsigned long long          :31;
        unsigned long long  VHSET1  :1;
        unsigned long long          :31;
    }bit;
};

/*  structure of VMIVADR    (2886_8028h)    */
union io_pas_vmivadr{
    unsigned long       word;
    struct {
        unsigned long   VMIVADR0    :14;
        unsigned long               :2;
        unsigned long   VMIVADR1    :14;
        unsigned long               :2;
    }bit;
};

/*  structure of VMIHADR    (2886_802Ch)    */
union io_pas_vmihadr{
    unsigned long       word;
    struct {
        unsigned long   VMIHADR0    :14;
        unsigned long               :2;
        unsigned long   VMIHADR1    :14;
        unsigned long               :2;
    }bit;
};

/*  structure of HMIHADR    (2886_8030h)    */
union io_pas_hmihadr{
    unsigned long       word;
    struct {
        unsigned long   HMIHADR0    :14;
        unsigned long               :2;
        unsigned long   HMIHADR1    :14;
        unsigned long               :2;
    }bit;
};

/* Define i/o mapping */
struct io_pastop{
    /* JDSIMG */
    union  io_pas_sr           SR;         /* 2886_(8000 - 8003h) */
    union  io_pas_clkstp       CLKSTP;     /* 2886_(8004 - 8007h) */
    union  io_pastopctl1       PASTOPCTL1; /* 2886_(8008 - 800Bh) */
    union  io_pas_intmon1      INTMON1;    /* 2886_(800C - 800Fh) */
    union  io_pas_intmon2      INTMON2;    /* 2886_(8010 - 8013h) */
    union  io_pas_intctl       INTCTL;     /* 2886_(8014 - 8017h) */
    union  io_pas_vhdintenb    VHDINTENB;  /* 2886_(8018 - 801Bh) */
    union  io_pas_vhdintflg    VHDINTFLG;  /* 2886_(801C - 801Fh) */
    union  io_pas_vhset        VHSET;      /* 2886_(8020 - 8027h) */
    union  io_pas_vmivadr      VMIVADR;    /* 2886_(8028 - 802Bh) */
    union  io_pas_vmihadr      VMIHADR;    /* 2886_(802C - 802Fh) */
    union  io_pas_hmihadr      HMIHADR;    /* 2886_(8030 - 8033h) */

    unsigned char dmy_8034_80FF[0x8100-0x8034]; /* 2886_(8034 - 80FFh) */

};

#endif	// _IO_PRO_PAS_TOP_H_
