/**
 * @file        __jdspro_sro_top.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_SRO_TOP_H_
#define _IO_PRO_SRO_TOP_H_

// --- REMOVE_BUZ BEGIN
#define CO_USE_BUZ
// --- REMOVE_BUZ END

/*  structure of SR (2840_8000h)    */
union io_srosr{
    unsigned long       word;
    struct {
        unsigned long   SR  :1;
        unsigned long       :31;
    }bit;
};

/*  structure of CLKSTP (2840_8004h)    */
union io_sroclkstp{
    unsigned long       word;
    struct {
        unsigned long   PSTP1   :1;
        unsigned long           :7;
        unsigned long   PSTP2   :1;
        unsigned long           :23;
    }bit;
};

/*  structure of SROTOPCTL1 (2840_8008h)    */
union io_srotopctl1{
    unsigned long       word;
    struct {
        unsigned long   SROISW  :1;
        unsigned long           :3;
        unsigned long   SROMD   :2;
        unsigned long           :2;
        unsigned long   CAGSL   :1;
        unsigned long           :3;
        unsigned long   BRPSW0  :2;
        unsigned long           :2;
        unsigned long   BRPSW1  :1;
        unsigned long           :3;
        unsigned long   BRPSW2  :1;
        unsigned long           :11;
    }bit;
};

/*  structure of SROTOPCTL2 (2840_800Ch)    */
union io_srotopctl2{
    unsigned long       word;
    struct {
        unsigned long   ORG0    :2;
        unsigned long           :6;
        unsigned long   ORG1    :2;
        unsigned long           :22;
    }bit;
};

/*  structure of INTMON1    (2840_8010h)    */
union io_srointmon1{
    unsigned long       word;
    struct {
        unsigned long   SDCF0   :1;
        unsigned long   SDCF1   :1;
        unsigned long   PRF0    :1;
        unsigned long   PRF1    :1;
        unsigned long   PRF2    :1;
        unsigned long   PRF3    :1;
        unsigned long   PRF4    :1;
        unsigned long   PRF5    :1;
        unsigned long   PRF6    :1;
        unsigned long   PRF7    :1;
        unsigned long   PRF8    :1;
        unsigned long   PRF9    :1;
        unsigned long   PWF0    :1;
        unsigned long   PWF1    :1;
        unsigned long   PWF2    :1;
        unsigned long   PWF3    :1;
        unsigned long   PWF4    :1;
        unsigned long   PWF5    :1;
        unsigned long           :14;
    }bit;
};

/*  structure of INTMON2    (2840_8014h)    */
union io_srointmon2{
    unsigned long       word;
    struct {
        unsigned long   PRXF0   :1;
        unsigned long   PRXF1   :1;
        unsigned long   PRXF2   :1;
        unsigned long   PRXF3   :1;
        unsigned long   PRXF4   :1;
        unsigned long   PRXF5   :1;
        unsigned long   PRXF6   :1;
        unsigned long   PRXF7   :1;
        unsigned long   PRXF8   :1;
        unsigned long   PRXF9   :1;
        unsigned long   PREF0   :1;
        unsigned long   PREF1   :1;
        unsigned long   PREF2   :1;
        unsigned long   PREF3   :1;
        unsigned long   PREF4   :1;
        unsigned long   PREF5   :1;
        unsigned long   PREF6   :1;
        unsigned long   PREF7   :1;
        unsigned long   PREF8   :1;
        unsigned long   PREF9   :1;
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
    }bit;
};

/*  structure of INTCTL (2840_8018h)    */
union io_srointctl{
    unsigned long       word;
    struct {
        unsigned long   INTMD   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of VHDINTENB  (2840_801Ch)    */
union io_srovhdintenb{
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

/*  structure of VHDINTFLG  (2840_8020h)    */
union io_srovhdintflg{
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

/*  structure of VHSET  (2840_8028h)    */
union io_srovhset{
    unsigned long long      dword;
    struct {
        unsigned long long  VHSET0  :1;
        unsigned long long          :31;
        unsigned long long  VHSET1  :1;
        unsigned long long          :31;
    }bit;
};

/*  structure of VMIVADR    (2840_8030h)    */
union io_srovmivadr{
    unsigned long       word;
    struct {
        unsigned long   VMIVADR0    :14;
        unsigned long               :2;
        unsigned long   VMIVADR1    :14;
        unsigned long               :2;
    }bit;
};

/*  structure of VMIHADR    (2840_8034h)    */
union io_srovmihadr{
    unsigned long       word;
    struct {
        unsigned long   VMIHADR0    :14;
        unsigned long               :2;
        unsigned long   VMIHADR1    :14;
        unsigned long               :2;
    }bit;
};

/*  structure of HMIHADR    (2840_8038h)    */
union io_srohmihadr{
    unsigned long       word;
    struct {
        unsigned long   HMIHADR0    :14;
        unsigned long               :2;
        unsigned long   HMIHADR1    :14;
        unsigned long               :2;
    }bit;
};

/*  structure of WAITGENEN  (2840_803Ch)    */
union io_srowaitgenen{
    unsigned long       word;
    struct {
        unsigned long   GENEN0  :1;
        unsigned long           :3;
        unsigned long   GENEN1  :1;
        unsigned long           :27;
    }bit;
};

/*  structure of WAITGENV   (2840_8040h)    */
union io_srowaitgenv{
    unsigned long       word;
    struct {
        unsigned long   GENV0   :14;
        unsigned long           :2;
        unsigned long   GENV1   :14;
        unsigned long           :2;
    }bit;
};

/*  structure of WAITGENH   (2840_8044h)    */
union io_srowaitgenh{
    unsigned long       word;
    struct {
        unsigned long   GENH0   :14;
        unsigned long           :2;
        unsigned long   GENH1   :14;
        unsigned long           :2;
    }bit;
};

// --- REMOVE_BUZ BEGIN
#ifdef CO_USE_BUZ
/*  structure of BRSZSL   (2840_8058h)    */
union io_srobrszsl{
    unsigned long       word;
    struct {
        unsigned long   BRSZSL  :1;
        unsigned long           :31;
    }bit;
};
#endif // CO_USE_BUZ
// --- REMOVE_BUZ END

/* Define i/o mapping */
struct io_srotop{
    /* JDSIMG */
    union  io_srosr           SR;               /* 2840_(8000 - 8003h) */
    union  io_sroclkstp       CLKSTP;           /* 2840_(8004 - 8007h) */
    union  io_srotopctl1      SROTOPCTL1;       /* 2840_(8008 - 800Bh) */
    union  io_srotopctl2      SROTOPCTL2;       /* 2840_(800C - 800Fh) */
    union  io_srointmon1      INTMON1;          /* 2840_(8010 - 8013h) */
    union  io_srointmon2      INTMON2;          /* 2840_(8014 - 8017h) */
    union  io_srointctl       INTCTL;           /* 2840_(8018 - 801Bh) */
    union  io_srovhdintenb    VHDINTENB;        /* 2840_(801C - 801Fh) */
    union  io_srovhdintflg    VHDINTFLG;        /* 2840_(8020 - 8023h) */

    unsigned char dmy_8024_8027[0x8028-0x8024]; /* 2840_(8024 - 8027h) */

    union  io_srovhset        VHSET;            /* 2840_(8028 - 802Fh) */
    union  io_srovmivadr      VMIVADR;          /* 2840_(8030 - 8033h) */
    union  io_srovmihadr      VMIHADR;          /* 2840_(8034 - 8037h) */
    union  io_srohmihadr      HMIHADR;          /* 2840_(8038 - 803Bh) */
    union  io_srowaitgenen    WAITGENEN;        /* 2840_(803C - 803Fh) */
    union  io_srowaitgenv     WAITGENV;         /* 2840_(8040 - 8043h) */
    union  io_srowaitgenh     WAITGENH;         /* 2840_(8044 - 8047h) */

// --- REMOVE_BUZ BEGIN
#ifdef CO_USE_BUZ
    unsigned char dmy_8048_8057[0x8058-0x8048]; /* 2840_(8048 - 8057h) */

	union  io_srobrszsl       BRSZSL;           /* 2840_(8058 - 805Bh) */

    unsigned char dmy_805C_80FF[0x8100-0x805C]; /* 2840_(805C - 80FFh) */
#endif	// CO_USE_BUZ
// --- REMOVE_BUZ END
// --- REMOVE_BUZ BEGIN
#ifndef CO_USE_BUZ
// --- REMOVE_BUZ END
    unsigned char dmy_8048_80FF[0x8100-0x8048]; /* 2840_(8048 - 80FFh) */
// --- REMOVE_BUZ BEGIN
#endif	// CO_USE_BUZ
// --- REMOVE_BUZ END
};

#endif  // _IO_PRO_SRO_TOP_H_
