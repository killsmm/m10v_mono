/**
 * @file		__jdspro_af_hs.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_AF_HS_H_
#define _IO_PRO_AF_HS_H_

/*  structure of AFHSCTLA   (2886_2280h)    */
union io_afhsctla{
    unsigned long       word;
    struct {
        unsigned long   AFIHA   :1;
        unsigned long           :3;
        unsigned long   AFFHA   :1;
        unsigned long           :3;
        unsigned long   AFSLHA  :2;
        unsigned long           :22;
    }bit;
};

/*  structure of AFHSCTLB   (2886_2284h)    */
union io_afhsctlb{
    unsigned long       word;
    struct {
        unsigned long   AFIHB   :1;
        unsigned long           :3;
        unsigned long   AFFHB   :1;
        unsigned long           :3;
        unsigned long   AFSLHB  :2;
        unsigned long           :22;
    }bit;
};

/*  structure of AFFIRHBA   (2886_2288h)    */
union io_affirhba_1{
    unsigned long       word;
    struct {
        long            AFFIRHBA_1  :8;
        long            AFFIRHBA_2  :8;
        long            AFFIRHBA_3  :8;
        long            AFFIRHBA_4  :8;
    }bit;
};
union io_affirhba_2{
    unsigned long       word;
    struct {
        long            AFFIRHBA_5  :8;
        unsigned long               :24;
    }bit;
};
struct io_affirhba{
    union io_affirhba_1 AFFIRHBA1;
    union io_affirhba_2 AFFIRHBA2;
};

/*  structure of AFFIRHBB   (2886_2290h)    */
union io_affirhbb_1{
    unsigned long       word;
    struct {
        long            AFFIRHBB_1  :8;
        long            AFFIRHBB_2  :8;
        long            AFFIRHBB_3  :8;
        long            AFFIRHBB_4  :8;
    }bit;
};
union io_affirhbb_2{
    unsigned long       word;
    struct {
        long            AFFIRHBB_5  :8;
        unsigned long               :24;
    }bit;
};
struct io_affirhbb{
    union io_affirhbb_1 AFFIRHBB1;
    union io_affirhbb_2 AFFIRHBB2;
};

/*  structure of AFCOR1HA   (2886_2298h)    */
union io_afcor1ha{
    unsigned long       word;
    struct {
        unsigned long   AFCOR1HA    :12;
        unsigned long               :20;
    }bit;
};

/*  structure of AFCOR1HB   (2886_229Ch)    */
union io_afcor1hb{
    unsigned long       word;
    struct {
        unsigned long   AFCOR1HB    :12;
        unsigned long               :20;
    }bit;
};

/*  structure of AFGAINHA   (2886_22A0h)    */
union io_afgainha{
    unsigned long       word;
    struct {
        unsigned long   AFGAINHA    :8;
        unsigned long               :24;
    }bit;
};

/*  structure of AFGAINHB   (2886_22A4h)    */
union io_afgainhb{
    unsigned long       word;
    struct {
        unsigned long   AFGAINHB    :8;
        unsigned long               :24;
    }bit;
};

/*  structure of AFIIRHBA   (2886_22B0h)    */
union io_afiirhba_1{
    unsigned long      word;
    struct {
        long           AFIIRHKA    :15;
        unsigned long              :1;
        long           AFIIRHA0A   :8;
        long           AFIIRHA1A   :8;
    }bit;
};
union io_afiirhba_2{
    unsigned long      word;
    struct {
        long           AFIIRHA2A   :8;
        unsigned long              :8;
        long           AFIIRHB1A   :15;
        unsigned long              :1;
    }bit;
};
union io_afiirhba_3{
    unsigned long       word;
    struct {
        long            AFIIRHB2A   :15;
        unsigned long               :17;
    }bit;
};
struct io_afiirhba{
    union io_afiirhba_1 AFIIRHBA1;
    union io_afiirhba_2 AFIIRHBA2;
    union io_afiirhba_3 AFIIRHBA3;
};

/*  structure of AFIIRHBB   (2886_22C0h)    */
union io_afiirhbb_1{
    unsigned long      word;
    struct {
        long           AFIIRHKB    :15;
        unsigned long              :1;
        long           AFIIRHA0B   :8;
        long           AFIIRHA1B   :8;
    }bit;
};
union io_afiirhbb_2{
    unsigned long      word;
    struct {
        long           AFIIRHA2B   :8;
        unsigned long              :8;
        long           AFIIRHB1B   :15;
        unsigned long              :1;
    }bit;
};
union io_afiirhbb_3{
    unsigned long       word;
    struct {
        long           AFIIRHB2B   :15;
        unsigned long               :17;
    }bit;
};
struct io_afiirhbb{
    union io_afiirhbb_1 AFIIRHBB1;
    union io_afiirhbb_2 AFIIRHBB2;
    union io_afiirhbb_3 AFIIRHBB3;
};

/*  structure of AFCORBHA   (2886_22D0h)    */
union io_afcorbha_1{
    unsigned long       word;
    struct {
        unsigned long   AFCORBHA_1  :12;
        unsigned long               :4;
        unsigned long   AFCORBHA_2  :12;
        unsigned long               :4;
    }bit;
};
union io_afcorbha_2{
    unsigned long       word;
    struct {
        unsigned long   AFCORBHA_3  :12;
        unsigned long               :4;
        unsigned long               :16;
    }bit;
};
struct io_afcorbha{
    union io_afcorbha_1 AFCORBHA1;
    union io_afcorbha_2 AFCORBHA2;
};

/*  structure of AFCORBHB   (2886_22D8h)    */
union io_afcorbhb_1{
    unsigned long       word;
    struct {
        unsigned long   AFCORBHB_1  :12;
        unsigned long               :4;
        unsigned long   AFCORBHB_2  :12;
        unsigned long               :4;
    }bit;
};
union io_afcorbhb_2{
    unsigned long       word;
    struct {
        unsigned long   AFCORBHB_3  :12;
        unsigned long               :4;
        unsigned long               :16;
    }bit;
};
struct io_afcorbhb{
    union io_afcorbhb_1 AFCORBHB1;
    union io_afcorbhb_2 AFCORBHB2;
};

/*  structure of AFCORINCHA (2886_22E0h)    */
union io_afcorincha_1{
    unsigned long       word;
    struct {
        long            AFCORINCHA_0    :12;
        unsigned long                   :4;
        long            AFCORINCHA_1    :12;
        unsigned long                   :4;
    }bit;
};
union io_afcorincha_2{
    unsigned long       word;
    struct {
        long            AFCORINCHA_2    :12;
        unsigned long                   :4;
        long            AFCORINCHA_3    :12;
        unsigned long                   :4;
    }bit;
};
struct io_afcorincha{
    union io_afcorincha_1   AFCORINCHA1;
    union io_afcorincha_2   AFCORINCHA2;
};

/*  structure of AFCORINCHB (2886_22E8h)    */
union io_afcorinchb_1{
    unsigned long       word;
    struct {
        long            AFCORINCHB_0    :12;
        unsigned long                   :4;
        long            AFCORINCHB_1    :12;
        unsigned long                   :4;
    }bit;
};
union io_afcorinchb_2{
    unsigned long       word;
    struct {
        long            AFCORINCHB_2    :12;
        unsigned long                   :4;
        long            AFCORINCHB_3    :12;
        unsigned long                   :4;
    }bit;
};
struct io_afcorinchb{
    union io_afcorinchb_1   AFCORINCHB1;
    union io_afcorinchb_2   AFCORINCHB2;
};

/*  structure of AFCOROFSHA (2886_22F0h)    */
union io_afcorofsha_1{
    unsigned long       word;
    struct {
        unsigned long   AFCOROFSHA_0    :12;
        unsigned long                   :4;
        unsigned long   AFCOROFSHA_1    :12;
        unsigned long                   :4;
    }bit;
};
union io_afcorofsha_2{
    unsigned long       word;
    struct {
        unsigned long   AFCOROFSHA_2    :12;
        unsigned long                   :4;
        unsigned long   AFCOROFSHA_3    :12;
        unsigned long                   :4;
    }bit;
};
struct io_afcorofsha{
    union io_afcorofsha_1   AFCOROFSHA1;
    union io_afcorofsha_2   AFCOROFSHA2;
};

/*  structure of AFCOROFSHB (2886_22F8h)    */
union io_afcorofshb_1{
    unsigned long       word;
    struct {
        unsigned long   AFCOROFSHB_0    :12;
        unsigned long                   :4;
        unsigned long   AFCOROFSHB_1    :12;
        unsigned long                   :4;
    }bit;
};
union io_afcorofshb_2{
    unsigned long       word;
    struct {
        unsigned long   AFCOROFSHB_2    :12;
        unsigned long                   :4;
        unsigned long   AFCOROFSHB_3    :12;
        unsigned long                   :4;
    }bit;
};
struct io_afcorofshb{
    union io_afcorofshb_1   AFCOROFSHB1;
    union io_afcorofshb_2   AFCOROFSHB2;
};

/* Define i/o mapping */
struct io_afhs{
    /* JDSIMG */
    union  io_afhsctla      AFHSCTLA;   /* 2886_(2280 - 2283h) */
    union  io_afhsctlb      AFHSCTLB;   /* 2886_(2284 - 2287h) */
    struct io_affirhba      AFFIRHBA;   /* 2886_(2288 - 228Fh) */
    struct io_affirhbb      AFFIRHBB;   /* 2886_(2290 - 2297h) */
    union  io_afcor1ha      AFCOR1HA;   /* 2886_(2298 - 229Bh) */
    union  io_afcor1hb      AFCOR1HB;   /* 2886_(229C - 229Fh) */
    union  io_afgainha      AFGAINHA;   /* 2886_(22A0 - 22A3h) */
    union  io_afgainhb      AFGAINHB;   /* 2886_(22A4 - 22A7h) */

    unsigned char dmy_22A8_22AF[0x22B0-0x22A8]; /* 2886_(22A8 - 22AFh) */

    struct io_afiirhba      AFIIRHBA;   /* 2886_(22B0 - 22BBh) */

    unsigned char dmy_22BC_22BF[0x22C0-0x22BC]; /* 2886_(22BC - 22BFh) */

    struct io_afiirhbb      AFIIRHBB;   /* 2886_(22C0 - 22CBh) */

    unsigned char dmy_22CC_22CF[0x22D0-0x22CC]; /* 2886_(22CC - 22CFh) */

    struct io_afcorbha      AFCORBHA;   /* 2886_(22D0 - 22D7h) */
    struct io_afcorbhb      AFCORBHB;   /* 2886_(22D8 - 22DFh) */
    struct io_afcorincha    AFCORINCHA; /* 2886_(22E0 - 22E7h) */
    struct io_afcorinchb    AFCORINCHB; /* 2886_(22E8 - 22EFh) */
    struct io_afcorofsha    AFCOROFSHA; /* 2886_(22F0 - 22F7h) */
    struct io_afcorofshb    AFCOROFSHB; /* 2886_(22F8 - 22FFh) */
};

#endif	// _IO_PRO_AF_HS_H_
