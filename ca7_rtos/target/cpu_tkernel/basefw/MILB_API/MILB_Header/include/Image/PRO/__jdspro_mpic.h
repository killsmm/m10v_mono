/**
 * @file		__jdspro_mpic.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_MPIC_H_
#define _IO_PRO_MPIC_H_

/*  structure of MPICSR (2800_9C00h)    */
union io_mpicsr{
    unsigned long       word;
    struct {
        unsigned long   SR  :1;
        unsigned long       :31;
    }bit;
};

/*  structure of MPICTRG    (2800_9C04h)    */
union io_mpictrg{
    unsigned long       word;
    struct {
        unsigned long   MPICTRG :2;
        unsigned long           :30;
    }bit;
};

/*  structure of MPICPAEN0  (2800_9C08h)    */
union io_mpicpaen0{
    unsigned long       word;
    struct {
        unsigned long   PAEN0   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of MPICPAEN1  (2800_9C0Ch)    */
union io_mpicpaen1{
    unsigned long       word;
    struct {
        unsigned long   PAEN1   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of MPICINTE1  (2800_9C10h)    */
union io_mpicinte1{
    unsigned long       word;
    struct {
        unsigned long               :1;
        unsigned long   CECE2       :1;
        unsigned long   CECE1       :1;
        unsigned long   CECE0       :1;
        unsigned long               :1;
        unsigned long   CEEE2       :1;
        unsigned long   CEEE1       :1;
        unsigned long   CEEE0       :1;
        unsigned long               :1;
        unsigned long   CESSHE2     :1;
        unsigned long   CESSHE1     :1;
        unsigned long   CESSHE0     :1;
        unsigned long               :1;
        unsigned long   CESHE2      :1;
        unsigned long   CESHE1      :1;
        unsigned long   CESHE0      :1;
        unsigned long   FEO1E       :1;
        unsigned long   FEO0E       :1;
        unsigned long   FEI1E       :1;
        unsigned long   FEI0E       :1;
        unsigned long   FSO1E       :1;
        unsigned long   FSO0E       :1;
        unsigned long   FSI1E       :1;
        unsigned long   FSI0E       :1;
        unsigned long               :1;
        unsigned long   LZEE2       :1;
        unsigned long   LZEE1       :1;
        unsigned long   LZEE0       :1;
        unsigned long   EFSE        :1;
        unsigned long   CRCDTEE     :1;
        unsigned long   PHCRC2EE    :1;
        unsigned long   PHCRC1EE    :1;
    }bit;
};

/*  structure of MPICINTE2  (2800_9C14h)    */
union io_mpicinte2{
    unsigned long       word;
    struct {
        unsigned long               :1;
        unsigned long   CICHSE2     :1;
        unsigned long   CICHSE1     :1;
        unsigned long   CICHSE0     :1;
        unsigned long   PHCRCAEE    :1;
        unsigned long               :27;
    }bit;
};

/*  structure of MPICINTF1  (2800_9C18h)    */
union io_mpicintf1{
    unsigned long       word;
    struct {
        unsigned long               :1;
        unsigned long   __CECF2     :1;
        unsigned long   __CECF1     :1;
        unsigned long   __CECF0     :1;
        unsigned long               :1;
        unsigned long   __CEEF2     :1;
        unsigned long   __CEEF1     :1;
        unsigned long   __CEEF0     :1;
        unsigned long               :1;
        unsigned long   __CESSHF2   :1;
        unsigned long   __CESSHF1   :1;
        unsigned long   __CESSHF0   :1;
        unsigned long               :1;
        unsigned long   __CESHF2    :1;
        unsigned long   __CESHF1    :1;
        unsigned long   __CESHF0    :1;
        unsigned long   __FEO1F     :1;
        unsigned long   __FEO0F     :1;
        unsigned long   __FEI1F     :1;
        unsigned long   __FEI0F     :1;
        unsigned long   __FSO1F     :1;
        unsigned long   __FSO0F     :1;
        unsigned long   __FSI1F     :1;
        unsigned long   __FSI0F     :1;
        unsigned long               :1;
        unsigned long   __LZEF2     :1;
        unsigned long   __LZEF1     :1;
        unsigned long   __LZEF0     :1;
        unsigned long   __EFSF      :1;
        unsigned long   __CRCDTEF   :1;
        unsigned long   __PHCRC2EF  :1;
        unsigned long   __PHCRC1EF  :1;
    }bit;
};

/*  structure of MPICINTF2  (2800_9C1Ch)    */
union io_mpicintf2{
    unsigned long       word;
    struct {
        unsigned long               :1;
        unsigned long   __CICHSF2   :1;
        unsigned long   __CICHSF1   :1;
        unsigned long   __CICHSF0   :1;
        unsigned long   __PHCRCAEF  :1;
        unsigned long               :27;
    }bit;
};

/*  structure of MPICLMODE  (2800_9C20h)    */
union io_mpiclmode{
    unsigned long       word;
    struct {
        unsigned long   LANE    :2;
        unsigned long           :30;
    }bit;
};

/*  structure of MPICIOMD0  (2800_9C24h)    */
union io_mpiciomd0{
    unsigned long       word;
    struct {
        unsigned long   DTMD0   :3;
        unsigned long           :1;
        unsigned long   OUTMD0  :3;
        unsigned long           :1;
        unsigned long   VCOUT0  :3;
        unsigned long           :1;
        unsigned long   DTCOMD0 :1;
        unsigned long           :3;
        unsigned long   DTCO0   :6;
        unsigned long           :10;
    }bit;
};

/*  structure of MPICIOMD1  (2800_9C28h)    */
union io_mpiciomd1{
    unsigned long       word;
    struct {
        unsigned long   DTMD1   :3;
        unsigned long           :1;
        unsigned long   OUTMD1  :3;
        unsigned long           :1;
        unsigned long   VCOUT1  :3;
        unsigned long           :1;
        unsigned long   DTCOMD1 :1;
        unsigned long           :3;
        unsigned long   DTCO1   :6;
        unsigned long           :10;
    }bit;
};

/*  structure of MPICPVR    (2800_9C2Ch)    */
union io_mpicpvr{
    unsigned long       word;
    struct {
        unsigned long           :29;
        unsigned long   SSL2    :1;
        unsigned long   SSL1    :1;
        unsigned long   SSL0    :1;
    }bit;
};

/*  structure of MPICHDPDSEL    (2800_9C34h)    */
union io_mpichdpdsel{
    unsigned long       word;
    struct {
        unsigned long   HDPDSEL0    :2;
        unsigned long               :2;
        unsigned long   HDPDSEL1    :2;
        unsigned long               :26;
    }bit;
};

/*  structure of MPICCPI1   (2800_9C38h)    */
union io_mpiccpi1{
    unsigned long       word;
    struct {
        unsigned long   CPEN    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of MPICCPI2   (2800_9C3Ch)    */
union io_mpiccpi2{
    unsigned long       word;
    struct {
        unsigned long   CP_HS_SETTLECNT :4;
        unsigned long                   :4;
        unsigned long   CP_HS_STOPCNT   :5;
        unsigned long                   :3;
        unsigned long   CP_HS_SYNCMODE  :1;
        unsigned long                   :15;
    }bit;
};

/*  structure of MPICPRLRLCONF  (2800_9C44h)    */
union io_mpicprlrlconf{
    unsigned long       word;
    struct {
        unsigned long   CP_RLR_LCONFIG  :8;
        unsigned long                   :24;
    }bit;
};

/*  structure of MPICPRLRLTPSEL (2800_9C48h)    */
union io_mpicprlrltpsel{
    unsigned long       word;
    struct {
        unsigned long   CP_RLR_TPSEL    :8;
        unsigned long                   :24;
    }bit;
};

/*  structure of MPICPRLRPRBS   (2800_9C4Ch)    */
union io_mpicprlrprbs{
    unsigned long       word;
    struct {
        unsigned long   CP_RLR_PRBSSP   :18;
        unsigned long                   :14;
    }bit;
};

/*  structure of MPICPRLRLST    (2800_9C50h)    */
union io_mpicprlrlst{
    unsigned long       word;
    struct {
        unsigned long   CP_RLR_LSTATUS0 :8;
        unsigned long   CP_RLR_LSTATUS1 :8;
        unsigned long   CP_RLR_LSTATUS2 :8;
        unsigned long                   :8;
    }bit;
};

/*  structure of MPICPRLRWECNT0 (2800_9C54h)    */
union io_mpicprlrwecnt0{
    unsigned long       word;
    struct {
        unsigned long   CP_RLR_WERRCNT0 :8;
        unsigned long                   :24;
    }bit;
};

/*  structure of MPICPRLRWECNT1 (2800_9C58h)    */
union io_mpicprlrwecnt1{
    unsigned long       word;
    struct {
        unsigned long   CP_RLR_WERRCNT1 :8;
        unsigned long                   :24;
    }bit;
};

/*  structure of MPICPRLRWECNT2 (2800_9C5Ch)    */
union io_mpicprlrwecnt2{
    unsigned long       word;
    struct {
        unsigned long   CP_RLR_WERRCNT2 :8;
        unsigned long                   :24;
    }bit;
};

/*  structure of MPICPRLRWCNT0  (2800_9C60h)    */
union io_mpicprlrwcnt0{
    unsigned long long      dword;
    struct {
        unsigned long long  CP_RLR_WCNT0    :48;
        unsigned long long                  :16;
    }bit;
};

/*  structure of MPICPRLRWCNT1  (2800_9C68h)    */
union io_mpicprlrwcnt1{
    unsigned long long      dword;
    struct {
        unsigned long long  CP_RLR_WCNT1    :48;
        unsigned long long                  :16;
    }bit;
};

/*  structure of MPICPRLRWCNT2  (2800_9C70h)    */
union io_mpicprlrwcnt2{
    unsigned long long      dword;
    struct {
        unsigned long long  CP_RLR_WCNT2    :48;
        unsigned long long                  :16;
    }bit;
};

/*  structure of MPICPRLRSERRCNT0   (2800_9C78h)    */
union io_mpicprlrserrcnt0{
    unsigned long       word;
    struct {
        unsigned long   CP_RLR_SYERRCNT0    :8;
        unsigned long                       :24;
    }bit;
};

/*  structure of MPICPRLRSERRCNT1   (2800_9C7Ch)    */
union io_mpicprlrserrcnt1{
    unsigned long       word;
    struct {
        unsigned long   CP_RLR_SYERRCNT1    :8;
        unsigned long                       :24;
    }bit;
};

/*  structure of MPICPRLRSERRCNT2   (2800_9C80h)    */
union io_mpicprlrserrcnt2{
    unsigned long       word;
    struct {
        unsigned long   CP_RLR_SYERRCNT2    :8;
        unsigned long                       :24;
    }bit;
};

/*  structure of MPICPRLRSERRLOC0   (2800_9C88h)    */
union io_mpicprlrserrloc0{
    unsigned long long      dword;
    struct {
        unsigned long long  CP_RLR_SYERRLOC0    :48;
        unsigned long long                      :16;
    }bit;
};

/*  structure of MPICPRLRSERRLOC1   (2800_9C90h)    */
union io_mpicprlrserrloc1{
    unsigned long long      dword;
    struct {
        unsigned long long  CP_RLR_SYERRLOC1    :48;
        unsigned long long                      :16;
    }bit;
};

/*  structure of MPICPRLRSERRLOC2   (2800_9C98h)    */
union io_mpicprlrserrloc2{
    unsigned long long      dword;
    struct {
        unsigned long long  CP_RLR_SYERRLOC2    :48;
        unsigned long long                      :16;
    }bit;
};

/*  structure of MPICPPROGSEQ   (2800_9CA0h)    */
union io_mpicpprogseq{
    unsigned long long      dword;
    struct {
        unsigned long long  CP_TGR_PROGSEQ  :42;
        unsigned long long                  :22;
    }bit;
};

/*  structure of MPICPFRXMODE   (2800_9CA8h)    */
union io_mpicpfrxmode{
    unsigned long       word;
    struct {
        unsigned long                   :1;
        unsigned long   CP_FORCERXMODE2 :1;
        unsigned long   CP_FORCERXMODE1 :1;
        unsigned long   CP_FORCERXMODE0 :1;
        unsigned long                   :28;
    }bit;
};

/* Define i/o mapping */
struct io_mpic{
    /* JDSIMG */
    union  io_mpicsr             MPICSR;            /* 2800_(9C00 - 9C03h) */
    union  io_mpictrg            MPICTRG;           /* 2800_(9C04 - 9C07h) */
    union  io_mpicpaen0          MPICPAEN0;         /* 2800_(9C08 - 9C0Bh) */
    union  io_mpicpaen1          MPICPAEN1;         /* 2800_(9C0C - 9C0Fh) */
    union  io_mpicinte1          MPICINTE1;         /* 2800_(9C10 - 9C13h) */
    union  io_mpicinte2          MPICINTE2;         /* 2800_(9C14 - 9C17h) */
    union  io_mpicintf1          MPICINTF1;         /* 2800_(9C18 - 9C1Bh) */
    union  io_mpicintf2          MPICINTF2;         /* 2800_(9C1C - 9C1Fh) */
    union  io_mpiclmode          MPICLMODE;         /* 2800_(9C20 - 9C23h) */
    union  io_mpiciomd0          MPICIOMD0;         /* 2800_(9C24 - 9C27h) */
    union  io_mpiciomd1          MPICIOMD1;         /* 2800_(9C28 - 9C2Bh) */
    union  io_mpicpvr            MPICPVR;           /* 2800_(9C2C - 9C2Fh) */

    unsigned char dmy_9C30_9C33[0x9C34-0x9C30];     /* 2800_(9C30 - 9C33h) */

    union  io_mpichdpdsel        MPICHDPDSEL;       /* 2800_(9C34 - 9C37h) */
    union  io_mpiccpi1           MPICCPI1;          /* 2800_(9C38 - 9C3Bh) */
    union  io_mpiccpi2           MPICCPI2;          /* 2800_(9C3C - 9C3Fh) */

    unsigned char dmy_9C40_9C43[0x9C44-0x9C40];     /* 2800_(9C40 - 9C43h) */

    union  io_mpicprlrlconf      MPICPRLRLCONF;     /* 2800_(9C44 - 9C47h) */
    union  io_mpicprlrltpsel     MPICPRLRLTPSEL;    /* 2800_(9C48 - 9C4Bh) */
    union  io_mpicprlrprbs       MPICPRLRPRBS;      /* 2800_(9C4C - 9C4Fh) */
    union  io_mpicprlrlst        MPICPRLRLST;       /* 2800_(9C50 - 9C53h) */
    union  io_mpicprlrwecnt0     MPICPRLRWECNT0;    /* 2800_(9C54 - 9C57h) */
    union  io_mpicprlrwecnt1     MPICPRLRWECNT1;    /* 2800_(9C58 - 9C5Bh) */
    union  io_mpicprlrwecnt2     MPICPRLRWECNT2;    /* 2800_(9C5C - 9C5Fh) */
    union  io_mpicprlrwcnt0      MPICPRLRWCNT0;     /* 2800_(9C60 - 9C67h) */
    union  io_mpicprlrwcnt1      MPICPRLRWCNT1;     /* 2800_(9C68 - 9C6Fh) */
    union  io_mpicprlrwcnt2      MPICPRLRWCNT2;     /* 2800_(9C70 - 9C77h) */
    union  io_mpicprlrserrcnt0   MPICPRLRSERRCNT0;  /* 2800_(9C78 - 9C7Bh) */
    union  io_mpicprlrserrcnt1   MPICPRLRSERRCNT1;  /* 2800_(9C7C - 9C7Fh) */
    union  io_mpicprlrserrcnt2   MPICPRLRSERRCNT2;  /* 2800_(9C80 - 9C83h) */

    unsigned char dmy_9C84_9C87[0x9C88-0x9C84];     /* 2800_(9C84 - 9C87h) */

    union  io_mpicprlrserrloc0   MPICPRLRSERRLOC0;	/* 2800_(9C88 - 9C8Fh) */
    union  io_mpicprlrserrloc1   MPICPRLRSERRLOC1;  /* 2800_(9C90 - 9C97h) */
    union  io_mpicprlrserrloc2   MPICPRLRSERRLOC2;  /* 2800_(9C98 - 9C9Fh) */
    union  io_mpicpprogseq       MPICPPROGSEQ;      /* 2800_(9CA0 - 9CA7h) */
    union  io_mpicpfrxmode       MPICPFRXMODE;      /* 2800_(9CA8 - 9CABh) */

    unsigned char dmy_9CAC_9DFF[0x9E00-0x9CAC];     /* 2800_(9CAC - 9DFFh) */

};

#endif	// _IO_PRO_MPIC_H_
