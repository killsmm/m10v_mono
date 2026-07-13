/**
 * @file		__jdspro_fshd.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_FSHD_H_
#define _IO_PRO_FSHD_H_

/*  structure of FSHDTRG    (2841_0A00h)    */
union io_fshdtrg{
    unsigned long       word;
    struct {
        unsigned long   FSHDTRG :2;
        unsigned long           :30;
    }bit;
};

/*  structure of FSHDPAEN   (2841_0A04h)    */
union io_fshdpaen{
    unsigned long       word;
    struct {
        unsigned long   FSPAEN  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of FSHDCTL    (2841_0A08h)    */
union io_fshdctl{
    unsigned long       word;
    struct {
        unsigned long   FSMODE      :2;
        unsigned long               :2;
        unsigned long   FWMODE      :1;
        unsigned long               :3;
        unsigned long   FSHDPTMD    :1;
        unsigned long               :23;
    }bit;
};

/*  structure of FSHDV  (2841_0A0Ch)    */
union io_fshdv{
    unsigned long       word;
    struct {
        unsigned long   FSHDV   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FSHDH  (2841_0A10h)    */
union io_fshdh{
    unsigned long       word;
    struct {
        unsigned long   FSHDH   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FSHDVW (2841_0A14h)    */
union io_fshdvw{
    unsigned long       word;
    struct {
        unsigned long   FSHDVW  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FSHDHW (2841_0A18h)    */
union io_fshdhw{
    unsigned long       word;
    struct {
        unsigned long   FSHDHW  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FSHDHWPH   (2841_0A1Ch)    */
union io_fshdhwph{
    unsigned long       word;
    struct {
        unsigned long   FSHDHWPH    :14;
        unsigned long               :18;
    }bit;
};

/*  structure of FSHDBC (2841_0A20h)    */
union io_fshdbc{
    unsigned long       word;
    struct {
        long            FSHDBC0 :10;
        unsigned long           :6;
        long            FSHDBC1 :10;
        unsigned long           :6;
    }bit;
};

/*  structure of FSUL1  (2841_0A24h)    */
union io_fsul1{
    unsigned long       word;
    struct {
        unsigned long   FSULRR  :12;
        unsigned long           :4;
        unsigned long   FSULGR  :12;
        unsigned long           :4;
    }bit;
};

/*  structure of FSUL2  (2841_0A28h)    */
union io_fsul2{
    unsigned long       word;
    struct {
        unsigned long   FSULGB  :12;
        unsigned long           :4;
        unsigned long   FSULBB  :12;
        unsigned long           :4;
    }bit;
};

/*  structure of FSCTL  (2841_0A30h)    */
union io_fsctl{
    unsigned long       word;
    struct {
        unsigned long   FSFMT   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of FSSUBPR    (2841_0A34h)    */
union io_fssubpr{
    unsigned long       word;
    struct {
        unsigned long   FSSUBPRH    :11;
        unsigned long               :5;
        unsigned long   FSSUBPRV    :11;
        unsigned long               :5;
    }bit;
};

/*  structure of FSSUBSP    (2841_0A38h)    */
union io_fssubsp{
    unsigned long       word;
    struct {
        unsigned long   FSSUBSPH    :10;
        unsigned long               :6;
        unsigned long   FSSUBSPV    :10;
        unsigned long               :6;
    }bit;
};

/*  structure of FSSUBDR    (2841_0A3Ch)    */
union io_fssubdr{
    unsigned long       word;
    struct {
        unsigned long   FSSUBDRM    :16;
        unsigned long   FSSUBDRE    :4;
        unsigned long               :12;
    }bit;
};

/*  structure of FSSUBST    (2841_0A40h)    */
union io_fssubst{
    unsigned long       word;
    struct {
        unsigned long   FSSUBSTH    :10;
        unsigned long               :6;
        unsigned long   FSSUBSTV    :10;
        unsigned long               :6;
    }bit;
};

/*  structure of FSA1   (2841_0A44h)    */
union io_fsa1{
    unsigned long       word;
    struct {
        unsigned long   FSARR   :9;
        unsigned long           :7;
        unsigned long   FSAGR   :9;
        unsigned long           :7;
    }bit;
};

/*  structure of FSA2   (2841_0A48h)    */
union io_fsa2{
    unsigned long       word;
    struct {
        unsigned long   FSAGB   :9;
        unsigned long           :7;
        unsigned long   FSABB   :9;
        unsigned long           :7;
    }bit;
};

/*  structure of CSCTL1 (2841_0A50h)    */
union io_csctl1{
    unsigned long       word;
    struct {
        unsigned long   CSSTBL  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of CSCTL2 (2841_0A54h)    */
union io_csctl2{
    unsigned long       word;
    struct {
        unsigned long   CSFMT       :1;
        unsigned long               :3;
        unsigned long   CSHDLIR     :3;
        unsigned long               :1;
        unsigned long   CSHDPARH    :7;
        unsigned long               :1;
        unsigned long   CSHDPARV    :7;
        unsigned long               :9;
    }bit;
};

/*  structure of CSTBLST    (2841_0A58h)    */
union io_cstblst{
    unsigned long       word;
    struct {
        unsigned long   CSTBST  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of CSTBLS1    (2841_0A5Ch)    */
union io_cstbls1{
    unsigned long       word;
    struct {
        unsigned long   CSTBLSRR    :10;
        unsigned long               :6;
        unsigned long   CSTBLSGR    :10;
        unsigned long               :6;
    }bit;
};

/*  structure of CSTBLS2    (2841_0A60h)    */
union io_cstbls2{
    unsigned long       word;
    struct {
        unsigned long   CSTBLSGB    :10;
        unsigned long               :6;
        unsigned long   CSTBLSBB    :10;
        unsigned long               :6;
    }bit;
};

/*  structure of CSHDD  (2841_0A64h)    */
union io_cshdd{
    unsigned long       word;
    struct {
        unsigned long   CSHDDO  :14;
        unsigned long           :2;
        unsigned long   CSHDDM  :14;
        unsigned long           :2;
    }bit;
};

/*  structure of CSHDOA (2841_0A68h)    */
union io_cshdoa{
    unsigned long       word;
    struct {
        long            CSHDOAH :15;
        unsigned long           :1;
        long            CSHDOAV :15;
        unsigned long           :1;
    }bit;
};

/*  structure of CSA1   (2841_0A6Ch)    */
union io_csa1{
    unsigned long       word;
    struct {
        unsigned long   CSARR   :9;
        unsigned long           :7;
        unsigned long   CSAGR   :9;
        unsigned long           :7;
    }bit;
};

/*  structure of CSA2   (2841_0A70h)    */
union io_csa2{
    unsigned long       word;
    struct {
        unsigned long   CSAGB   :9;
        unsigned long           :7;
        unsigned long   CSABB   :9;
        unsigned long           :7;
    }bit;
};

/*  structure of FSHDHCCTL  (2841_0A80h)    */
union io_fshdhcctl{
    unsigned long       word;
    struct {
        unsigned long   FSHCEN  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of FSSLP1 (2841_0A84h)    */
union io_fsslp1{
    unsigned long       word;
    struct {
        unsigned long   FSSLPRR :12;
        unsigned long           :4;
        unsigned long   FSSLPGR :12;
        unsigned long           :4;
    }bit;
};

/*  structure of FSSLP2 (2841_0A88h)    */
union io_fsslp2{
    unsigned long       word;
    struct {
        unsigned long   FSSLPGB :12;
        unsigned long           :4;
        unsigned long   FSSLPBB :12;
        unsigned long           :4;
    }bit;
};

/*  structure of FSWRIC (2841_0A90h)    */
union io_fswric{
    unsigned long       word;
    struct {
        unsigned long   FSWRIC  :13;
        unsigned long           :19;
    }bit;
};

/* Define i/o mapping */
struct io_fshd{
    /* JDSIMG */
    union  io_fshdtrg   FSHDTRG;    /* 2841_(0A00 - 0A03h) */
    union  io_fshdpaen  FSHDPAEN;   /* 2841_(0A04 - 0A07h) */
    union  io_fshdctl   FSHDCTL;    /* 2841_(0A08 - 0A0Bh) */
    union  io_fshdv     FSHDV;      /* 2841_(0A0C - 0A0Fh) */
    union  io_fshdh     FSHDH;      /* 2841_(0A10 - 0A13h) */
    union  io_fshdvw    FSHDVW;     /* 2841_(0A14 - 0A17h) */
    union  io_fshdhw    FSHDHW;     /* 2841_(0A18 - 0A1Bh) */
    union  io_fshdhwph  FSHDHWPH;   /* 2841_(0A1C - 0A1Fh) */
    union  io_fshdbc    FSHDBC;     /* 2841_(0A20 - 0A23h) */
    union  io_fsul1     FSUL1;      /* 2841_(0A24 - 0A27h) */
    union  io_fsul2     FSUL2;      /* 2841_(0A28 - 0A2Bh) */

    unsigned char dmy_0A2C_0A2F[0x0A30-0x0A2C]; /* 2841_(0A2C - 0A2Fh) */

    union  io_fsctl     FSCTL;      /* 2841_(0A30 - 0A33h) */
    union  io_fssubpr   FSSUBPR;    /* 2841_(0A34 - 0A37h) */
    union  io_fssubsp   FSSUBSP;    /* 2841_(0A38 - 0A3Bh) */
    union  io_fssubdr   FSSUBDR;    /* 2841_(0A3C - 0A3Fh) */
    union  io_fssubst   FSSUBST;    /* 2841_(0A40 - 0A43h) */
    union  io_fsa1      FSA1;       /* 2841_(0A44 - 0A47h) */
    union  io_fsa2      FSA2;       /* 2841_(0A48 - 0A4Bh) */

    unsigned char dmy_0A4C_0A4F[0x0A50-0x0A4C]; /* 2841_(0A4C - 0A4Fh) */

    union  io_csctl1    CSCTL1;     /* 2841_(0A50 - 0A53h) */
    union  io_csctl2    CSCTL2;     /* 2841_(0A54 - 0A57h) */
    union  io_cstblst   CSTBLST;    /* 2841_(0A58 - 0A5Bh) */
    union  io_cstbls1   CSTBLS1;    /* 2841_(0A5C - 0A5Fh) */
    union  io_cstbls2   CSTBLS2;    /* 2841_(0A60 - 0A63h) */
    union  io_cshdd     CSHDD;      /* 2841_(0A64 - 0A67h) */
    union  io_cshdoa    CSHDOA;     /* 2841_(0A68 - 0A6Bh) */
    union  io_csa1      CSA1;       /* 2841_(0A6C - 0A6Fh) */
    union  io_csa2      CSA2;       /* 2841_(0A70 - 0A73h) */

    unsigned char dmy_0A74_0A7F[0x0A80-0x0A74]; /* 2841_(0A74 - 0A7Fh) */

    union  io_fshdhcctl FSHDHCCTL;  /* 2841_(0A80 - 0A83h) */
    union  io_fsslp1    FSSLP1;     /* 2841_(0A84 - 0A87h) */
    union  io_fsslp2    FSSLP2;     /* 2841_(0A88 - 0A8Bh) */

    unsigned char dmy_0A8C_0A8F[0x0A90-0x0A8C]; /* 2841_(0A8C - 0A8Fh) */

    union  io_fswric    FSWRIC;     /* 2841_(0A90 - 0A93h) */

    unsigned char dmy_0A94_0AFF[0x0B00-0x0A94]; /* 2841_(0A94 - 0AFFh) */

};

#endif	// _IO_PRO_FSHD_H_
