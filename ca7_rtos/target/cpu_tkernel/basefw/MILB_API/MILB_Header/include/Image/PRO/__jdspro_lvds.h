/**
 * @file        __jdspro_lvds.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_LVDS_H_
#define _IO_PRO_LVDS_H_

/*  structure of LVDSTRG    (2800_8800h)    */
union io_lvds_trg{
    unsigned long       word;
    struct {
        unsigned long   LVDSTRG :2;
        unsigned long           :30;
    }bit;
};

/*  structure of LVDSINTENB0    (2800_8804h)    */
union io_lvds_intenb0{
    unsigned long       word;
    struct {
        unsigned long   SOFE0   :1;
        unsigned long           :3;
        unsigned long   EOFE0   :1;
        unsigned long           :3;
        unsigned long   SOLE0   :1;
        unsigned long           :3;
        unsigned long   EOLE0   :1;
        unsigned long           :19;
    }bit;
};

/*  structure of LVDSINTENB1    (2800_8808h)    */
union io_lvds_intenb1{
    unsigned long       word;
    struct {
        unsigned long   SOFE1   :1;
        unsigned long           :3;
        unsigned long   EOFE1   :1;
        unsigned long           :3;
        unsigned long   SOLE1   :1;
        unsigned long           :3;
        unsigned long   EOLE1   :1;
        unsigned long           :19;
    }bit;
};

/*  structure of LVDSINTENB2    (2800_880Ch)    */
union io_lvds_intenb2{
    unsigned long       word;
    struct {
        unsigned long   SOFE2   :1;
        unsigned long           :3;
        unsigned long   EOFE2   :1;
        unsigned long           :3;
        unsigned long   SOLE2   :1;
        unsigned long           :3;
        unsigned long   EOLE2   :1;
        unsigned long           :19;
    }bit;
};

/*  structure of LVDSINTFLG0    (2800_8810h)    */
union io_lvds_intflg0{
    unsigned long       word;
    struct {
        unsigned long   __SOFF0   :1;
        unsigned long             :3;
        unsigned long   __EOFF0   :1;
        unsigned long             :3;
        unsigned long   __SOLF0   :1;
        unsigned long             :3;
        unsigned long   __EOLF0   :1;
        unsigned long             :19;
    }bit;
};

/*  structure of LVDSINTFLG1    (2800_8814h)    */
union io_lvds_intflg1{
    unsigned long       word;
    struct {
        unsigned long   __SOFF1   :1;
        unsigned long             :3;
        unsigned long   __EOFF1   :1;
        unsigned long             :3;
        unsigned long   __SOLF1   :1;
        unsigned long             :3;
        unsigned long   __EOLF1   :1;
        unsigned long             :19;
    }bit;
};

/*  structure of LVDSINTFLG2    (2800_8818h)    */
union io_lvds_intflg2{
    unsigned long       word;
    struct {
        unsigned long   __SOFF2   :1;
        unsigned long             :3;
        unsigned long   __EOFF2   :1;
        unsigned long             :3;
        unsigned long   __SOLF2   :1;
        unsigned long             :3;
        unsigned long   __EOLF2   :1;
        unsigned long             :19;
    }bit;
};

/*  structure of LVDSINTADR0    (2800_881Ch)    */
union io_lvds_intadr0{
    unsigned long       word;
    struct {
        unsigned long   SOLVADR_0   :14;
        unsigned long               :2;
        unsigned long   EOLVADR_0   :14;
        unsigned long               :2;
    }bit;
};

/*  structure of LVDSINTADR1    (2800_8820h)    */
union io_lvds_intadr1{
    unsigned long       word;
    struct {
        unsigned long   SOLVADR_1   :14;
        unsigned long               :2;
        unsigned long   EOLVADR_1   :14;
        unsigned long               :2;
    }bit;
};

/*  structure of LVDSINTADR2    (2800_8824h)    */
union io_lvds_intadr2{
    unsigned long       word;
    struct {
        unsigned long   SOLVADR_2   :14;
        unsigned long               :2;
        unsigned long   EOLVADR_2   :14;
        unsigned long               :2;
    }bit;
};

/*  structure of LVDSINTSET0    (2800_8828h)    */
union io_lvds_intset0{
    unsigned long       word;
    struct {
        unsigned long   ADRSET0 :1;
        unsigned long           :31;
    }bit;
};

/*  structure of LVDSINTSET1    (2800_882Ch)    */
union io_lvds_intset1{
    unsigned long       word;
    struct {
        unsigned long   ADRSET1 :1;
        unsigned long           :31;
    }bit;
};

/*  structure of LVDSINTSET2    (2800_8830h)    */
union io_lvds_intset2{
    unsigned long       word;
    struct {
        unsigned long   ADRSET2 :1;
        unsigned long           :31;
    }bit;
};

/*  structure of LVDSCTL1   (2800_8834h)    */
union io_lvds_ctl1{
    unsigned long       word;
    struct {
        unsigned long   LMD     :1;
        unsigned long           :3;
        unsigned long   LCH     :2;
        unsigned long           :2;
        unsigned long   LANE    :4;
        unsigned long   LDW     :3;
        unsigned long           :1;
        unsigned long   LSFT    :4;
        unsigned long   SYNT    :1;
        unsigned long           :3;
        unsigned long   SZEN    :1;
        unsigned long           :3;
        unsigned long   SYN16   :1;
        unsigned long           :3;
    }bit;
};

/*  structure of LVDSCTL2   (2800_8838h)    */
union io_lvds_ctl2{
    unsigned long       word;
    struct {
        unsigned long   DIVOT   :2;
        unsigned long           :2;
        unsigned long   LMIR    :1;
        unsigned long           :3;
        unsigned long   DOLMD   :2;
        unsigned long           :6;
        unsigned long   LMSK    :16;
    }bit;
};

/*  structure of LVDSCLKS   (2800_883Ch)    */
union io_lvds_clks{
    unsigned long       word;
    struct {
        unsigned long   CLKS0   :3;
        unsigned long           :1;
        unsigned long   CLKS1   :3;
        unsigned long           :1;
        unsigned long   CLKS2   :3;
        unsigned long           :1;
        unsigned long   CLKS3   :3;
        unsigned long           :1;
        unsigned long   CLKS4   :3;
        unsigned long           :1;
        unsigned long   CLKS5   :3;
        unsigned long           :1;
        unsigned long   CLKS6   :3;
        unsigned long           :1;
        unsigned long   CLKS7   :3;
        unsigned long           :1;
    }bit;
};

/*  structure of LVDSDATS   (2800_8840h)    */
union io_lvds_dats{
    unsigned long long      dword;
    struct {
        unsigned long long  DATS00  :4;
        unsigned long long  DATS01  :4;
        unsigned long long  DATS10  :4;
        unsigned long long  DATS11  :4;
        unsigned long long  DATS20  :4;
        unsigned long long  DATS21  :4;
        unsigned long long  DATS30  :4;
        unsigned long long  DATS31  :4;
        unsigned long long  DATS40  :4;
        unsigned long long  DATS41  :4;
        unsigned long long  DATS50  :4;
        unsigned long long  DATS51  :4;
        unsigned long long  DATS60  :4;
        unsigned long long  DATS61  :4;
        unsigned long long  DATS71  :4;
        unsigned long long  DATS70  :4;
    }bit;
};

/*  structure of LHSADD (2800_8848h)    */
union io_lvds_lhsadd{
    unsigned long       word;
    struct {
        unsigned long   LHSADD  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of LHSIZE (2800_884Ch)    */
union io_lvds_lhsize{
    unsigned long       word;
    struct {
        unsigned long   LHSIZE  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of SYNCSEL    (2800_8850h)    */
union io_lvds_syncsel{
    unsigned long       word;
    struct {
        unsigned long   SYNCSEL :8;
        unsigned long           :24;
    }bit;
};

/*  structure of LPDCHG (2800_8858h)    */
union io_lvds_lpdchg{
    unsigned long long      dword;
    struct {
        unsigned long long  SELA    :3;
        unsigned long long          :1;
        unsigned long long  SELB    :3;
        unsigned long long          :1;
        unsigned long long  SELC    :3;
        unsigned long long          :1;
        unsigned long long  SELD    :3;
        unsigned long long          :1;
        unsigned long long  SELE    :3;
        unsigned long long          :1;
        unsigned long long  SELF    :3;
        unsigned long long          :1;
        unsigned long long  SELG    :3;
        unsigned long long          :1;
        unsigned long long  SELH    :3;
        unsigned long long          :1;
        unsigned long long  SELI    :3;
        unsigned long long          :1;
        unsigned long long  SELJ    :3;
        unsigned long long          :1;
        unsigned long long  SELK    :3;
        unsigned long long          :1;
        unsigned long long  SELL    :3;
        unsigned long long          :1;
        unsigned long long  SELM    :3;
        unsigned long long          :1;
        unsigned long long  SELN    :3;
        unsigned long long          :1;
        unsigned long long  SELO    :3;
        unsigned long long          :1;
        unsigned long long  SELP    :3;
        unsigned long long          :1;
    }bit;
};

/*  structure of SAV0   (2800_8860h)    */
union io_lvds_sav0_1{
    unsigned long       word;
    struct {
        unsigned long   SAV0_0  :16;
        unsigned long   SAV0_1  :16;
    }bit;
};
union io_lvds_sav0_2{
    unsigned long       word;
    struct {
        unsigned long   SAV0_2  :16;
        unsigned long   SAV0_3  :16;
    }bit;
};
struct io_lvds_sav0{
    union io_lvds_sav0_1 SAV01;
    union io_lvds_sav0_2 SAV02;
};

/*  structure of SAV1   (2800_8868h)    */
union io_lvds_sav1_1{
    unsigned long       word;
    struct {
        unsigned long   SAV1_0  :16;
        unsigned long   SAV1_1  :16;
    }bit;
};
union io_lvds_sav1_2{
    unsigned long       word;
    struct {
        unsigned long   SAV1_2  :16;
        unsigned long   SAV1_3  :16;
    }bit;
};
struct io_lvds_sav1{
    union io_lvds_sav1_1 SAV11;
    union io_lvds_sav1_2 SAV12;
};

/*  structure of SAV2   (2800_8870h)    */
union io_lvds_sav2_1{
    unsigned long       word;
    struct {
        unsigned long   SAV2_0  :16;
        unsigned long   SAV2_1  :16;
    }bit;
};
union io_lvds_sav2_2{
    unsigned long       word;
    struct {
        unsigned long   SAV2_2  :16;
        unsigned long   SAV2_3  :16;
    }bit;
};
struct io_lvds_sav2{
    union io_lvds_sav2_1 SAV21;
    union io_lvds_sav2_2 SAV22;
};

/*  structure of EAV0   (2800_8878h)    */
union io_lvds_eav0_1{
    unsigned long       word;
    struct {
        unsigned long   EAV0_0  :16;
        unsigned long   EAV0_1  :16;
    }bit;
};
union io_lvds_eav0_2{
    unsigned long       word;
    struct {
        unsigned long   EAV0_2  :16;
        unsigned long   EAV0_3  :16;
    }bit;
};
struct io_lvds_eav0{
    union io_lvds_eav0_1 EAV01;
    union io_lvds_eav0_2 EAV02;
};

/*  structure of EAV1   (2800_8880h)    */
union io_lvds_eav1_1{
    unsigned long       word;
    struct {
        unsigned long   EAV1_0  :16;
        unsigned long   EAV1_1  :16;
    }bit;
};
union io_lvds_eav1_2{
    unsigned long       word;
    struct {
        unsigned long   EAV1_2  :16;
        unsigned long   EAV1_3  :16;
    }bit;
};
struct io_lvds_eav1{
    union io_lvds_eav1_1 EAV11;
    union io_lvds_eav1_2 EAV12;
};

/*  structure of EAV2   (2800_8888h)    */
union io_lvds_eav2_1{
    unsigned long       word;
    struct {
        unsigned long   EAV2_0  :16;
        unsigned long   EAV2_1  :16;
    }bit;
};
union io_lvds_eav2_2{
    unsigned long       word;
    struct {
        unsigned long   EAV2_2  :16;
        unsigned long   EAV2_3  :16;
    }bit;
};
struct io_lvds_eav2{
    union io_lvds_eav2_1 EAV21;
    union io_lvds_eav2_2 EAV22;
};

/*  structure of SAB0   (2800_8890h)    */
union io_lvds_sab0_1{
    unsigned long       word;
    struct {
        unsigned long   SAB0_0  :16;
        unsigned long   SAB0_1  :16;
    }bit;
};
union io_lvds_sab0_2{
    unsigned long       word;
    struct {
        unsigned long   SAB0_2  :16;
        unsigned long   SAB0_3  :16;
    }bit;
};
struct io_lvds_sab0{
    union io_lvds_sab0_1 SAB01;
    union io_lvds_sab0_2 SAB02;
};

/*  structure of SAB1   (2800_8898h)    */
union io_lvds_sab1_1{
    unsigned long       word;
    struct {
        unsigned long   SAB1_0  :16;
        unsigned long   SAB1_1  :16;
    }bit;
};
union io_lvds_sab1_2{
    unsigned long       word;
    struct {
        unsigned long   SAB1_2  :16;
        unsigned long   SAB1_3  :16;
    }bit;
};
struct io_lvds_sab1{
    union io_lvds_sab1_1 SAB11;
    union io_lvds_sab1_2 SAB12;
};

/*  structure of SAB2   (2800_88A0h)    */
union io_lvds_sab2_1{
    unsigned long       word;
    struct {
        unsigned long   SAB2_0  :16;
        unsigned long   SAB2_1  :16;
    }bit;
};
union io_lvds_sab2_2{
    unsigned long       word;
    struct {
        unsigned long   SAB2_2  :16;
        unsigned long   SAB2_3  :16;
    }bit;
};
struct io_lvds_sab2{
    union io_lvds_sab2_1 SAB21;
    union io_lvds_sab2_2 SAB22;
};

/*  structure of EAB0   (2800_88A8h)    */
union io_lvds_eab0_1{
    unsigned long       word;
    struct {
        unsigned long   EAB0_0  :16;
        unsigned long   EAB0_1  :16;
    }bit;
};
union io_lvds_eab0_2{
    unsigned long       word;
    struct {
        unsigned long   EAB0_2  :16;
        unsigned long   EAB0_3  :16;
    }bit;
};
struct io_lvds_eab0{
    union io_lvds_eab0_1 EAB01;
    union io_lvds_eab0_2 EAB02;
};

/*  structure of EAB1   (2800_88B0h)    */
union io_lvds_eab1_1{
    unsigned long       word;
    struct {
        unsigned long   EAB1_0  :16;
        unsigned long   EAB1_1  :16;
    }bit;
};
union io_lvds_eab1_2{
    unsigned long       word;
    struct {
        unsigned long   EAB1_2  :16;
        unsigned long   EAB1_3  :16;
    }bit;
};
struct io_lvds_eab1{
    union io_lvds_eab1_1 EAB11;
    union io_lvds_eab1_2 EAB12;
};

/*  structure of EAB2   (2800_88B8h)    */
union io_lvds_eab2_1{
    unsigned long       word;
    struct {
        unsigned long   EAB2_0  :16;
        unsigned long   EAB2_1  :16;
    }bit;
};
union io_lvds_eab2_2{
    unsigned long       word;
    struct {
        unsigned long   EAB2_2  :16;
        unsigned long   EAB2_3  :16;
    }bit;
};
struct io_lvds_eab2{
    union io_lvds_eab2_1 EAB21;
    union io_lvds_eab2_2 EAB22;
};

/*  structure of MSAV0  (2800_88C0h)    */
union io_lvds_msav0_1{
    unsigned long       word;
    struct {
        unsigned long   MSAV0_0 :16;
        unsigned long   MSAV0_1 :16;
    }bit;
};
union io_lvds_msav0_2{
    unsigned long       word;
    struct {
        unsigned long   MSAV0_2 :16;
        unsigned long   MSAV0_3 :16;
    }bit;
};
struct io_lvds_msav0{
    union io_lvds_msav0_1    MSAV01;
    union io_lvds_msav0_2    MSAV02;
};

/*  structure of MSAV1  (2800_88C8h)    */
union io_lvds_msav1_1{
    unsigned long       word;
    struct {
        unsigned long   MSAV1_0 :16;
        unsigned long   MSAV1_1 :16;
    }bit;
};
union io_lvds_msav1_2{
    unsigned long       word;
    struct {
        unsigned long   MSAV1_2 :16;
        unsigned long   MSAV1_3 :16;
    }bit;
};
struct io_lvds_msav1{
    union io_lvds_msav1_1    MSAV11;
    union io_lvds_msav1_2    MSAV12;
};

/*  structure of MSAV2  (2800_88D0h)    */
union io_lvds_msav2_1{
    unsigned long       word;
    struct {
        unsigned long   MSAV2_0 :16;
        unsigned long   MSAV2_1 :16;
    }bit;
};
union io_lvds_msav2_2{
    unsigned long       word;
    struct {
        unsigned long   MSAV2_2 :16;
        unsigned long   MSAV2_3 :16;
    }bit;
};
struct io_lvds_msav2{
    union io_lvds_msav2_1    MSAV21;
    union io_lvds_msav2_2    MSAV22;
};

/*  structure of MEAV0  (2800_88D8h)    */
union io_lvds_meav0_1{
    unsigned long       word;
    struct {
        unsigned long   MEAV0_0 :16;
        unsigned long   MEAV0_1 :16;
    }bit;
};
union io_lvds_meav0_2{
    unsigned long       word;
    struct {
        unsigned long   MEAV0_2 :16;
        unsigned long   MEAV0_3 :16;
    }bit;
};
struct io_lvds_meav0{
    union io_lvds_meav0_1    MEAV01;
    union io_lvds_meav0_2    MEAV02;
};

/*  structure of MEAV1  (2800_88E0h)    */
union io_lvds_meav1_1{
    unsigned long       word;
    struct {
        unsigned long   MEAV1_0 :16;
        unsigned long   MEAV1_1 :16;
    }bit;
};
union io_lvds_meav1_2{
    unsigned long       word;
    struct {
        unsigned long   MEAV1_2 :16;
        unsigned long   MEAV1_3 :16;
    }bit;
};
struct io_lvds_meav1{
    union io_lvds_meav1_1    MEAV11;
    union io_lvds_meav1_2    MEAV12;
};

/*  structure of MEAV2  (2800_88E8h)    */
union io_lvds_meav2_1{
    unsigned long       word;
    struct {
        unsigned long   MEAV2_0 :16;
        unsigned long   MEAV2_1 :16;
    }bit;
};
union io_lvds_meav2_2{
    unsigned long       word;
    struct {
        unsigned long   MEAV2_2 :16;
        unsigned long   MEAV2_3 :16;
    }bit;
};
struct io_lvds_meav2{
    union io_lvds_meav2_1    MEAV21;
    union io_lvds_meav2_2    MEAV22;
};

/*  structure of MSAB0  (2800_88F0h)    */
union io_lvds_msab0_1{
    unsigned long       word;
    struct {
        unsigned long   MSAB0_0 :16;
        unsigned long   MSAB0_1 :16;
    }bit;
};
union io_lvds_msab0_2{
    unsigned long       word;
    struct {
        unsigned long   MSAB0_2 :16;
        unsigned long   MSAB0_3 :16;
    }bit;
};
struct io_lvds_msab0{
    union io_lvds_msab0_1    MSAB01;
    union io_lvds_msab0_2    MSAB02;
};

/*  structure of MSAB1  (2800_88F8h)    */
union io_lvds_msab1_1{
    unsigned long       word;
    struct {
        unsigned long   MSAB1_0 :16;
        unsigned long   MSAB1_1 :16;
    }bit;
};
union io_lvds_msab1_2{
    unsigned long       word;
    struct {
        unsigned long   MSAB1_2 :16;
        unsigned long   MSAB1_3 :16;
    }bit;
};
struct io_lvds_msab1{
    union io_lvds_msab1_1    MSAB11;
    union io_lvds_msab1_2    MSAB12;
};

/*  structure of MSAB2  (2800_8900h)    */
union io_lvds_msab2_1{
    unsigned long       word;
    struct {
        unsigned long   MSAB2_0 :16;
        unsigned long   MSAB2_1 :16;
    }bit;
};
union io_lvds_msab2_2{
    unsigned long       word;
    struct {
        unsigned long   MSAB2_2 :16;
        unsigned long   MSAB2_3 :16;
    }bit;
};
struct io_lvds_msab2{
    union io_lvds_msab2_1    MSAB21;
    union io_lvds_msab2_2    MSAB22;
};

/*  structure of MEAB0  (2800_8908h)    */
union io_lvds_meab0_1{
    unsigned long       word;
    struct {
        unsigned long   MEAB0_0 :16;
        unsigned long   MEAB0_1 :16;
    }bit;
};
union io_lvds_meab0_2{
    unsigned long       word;
    struct {
        unsigned long   MEAB0_2 :16;
        unsigned long   MEAB0_3 :16;
    }bit;
};
struct io_lvds_meab0{
    union io_lvds_meab0_1    MEAB01;
    union io_lvds_meab0_2    MEAB02;
};

/*  structure of MEAB1  (2800_8910h)    */
union io_lvds_meab1_1{
    unsigned long       word;
    struct {
        unsigned long   MEAB1_0 :16;
        unsigned long   MEAB1_1 :16;
    }bit;
};
union io_lvds_meab1_2{
    unsigned long       word;
    struct {
        unsigned long   MEAB1_2 :16;
        unsigned long   MEAB1_3 :16;
    }bit;
};
struct io_lvds_meab1{
    union io_lvds_meab1_1    MEAB11;
    union io_lvds_meab1_2    MEAB12;
};

/*  structure of MEAB2  (2800_8918h)    */
union io_lvds_meab2_1{
    unsigned long       word;
    struct {
        unsigned long   MEAB2_0 :16;
        unsigned long   MEAB2_1 :16;
    }bit;
};
union io_lvds_meab2_2{
    unsigned long       word;
    struct {
        unsigned long   MEAB2_2 :16;
        unsigned long   MEAB2_3 :16;
    }bit;
};
struct io_lvds_meab2{
    union io_lvds_meab2_1    MEAB21;
    union io_lvds_meab2_2    MEAB22;
};

/*  structure of LVDSPAEN   (2800_8920h)    */
union io_lvds_paen{
    unsigned long       word;
    struct {
        unsigned long   PAEN    :1;
        unsigned long           :31;
    }bit;
};

/* Define i/o mapping */
struct io_lvds{
    /* JDSIMG */
    union  io_lvds_trg            LVDSTRG;          /* 2800_(8800 - 8803h) */
    union  io_lvds_intenb0        LVDSINTENB0;      /* 2800_(8804 - 8807h) */
    union  io_lvds_intenb1        LVDSINTENB1;      /* 2800_(8808 - 880Bh) */
    union  io_lvds_intenb2        LVDSINTENB2;      /* 2800_(880C - 880Fh) */
    union  io_lvds_intflg0        LVDSINTFLG0;      /* 2800_(8810 - 8813h) */
    union  io_lvds_intflg1        LVDSINTFLG1;      /* 2800_(8814 - 8817h) */
    union  io_lvds_intflg2        LVDSINTFLG2;      /* 2800_(8818 - 881Bh) */
    union  io_lvds_intadr0        LVDSINTADR0;      /* 2800_(881C - 881Fh) */
    union  io_lvds_intadr1        LVDSINTADR1;      /* 2800_(8820 - 8823h) */
    union  io_lvds_intadr2        LVDSINTADR2;      /* 2800_(8824 - 8827h) */
    union  io_lvds_intset0        LVDSINTSET0;      /* 2800_(8828 - 882Bh) */
    union  io_lvds_intset1        LVDSINTSET1;      /* 2800_(882C - 882Fh) */
    union  io_lvds_intset2        LVDSINTSET2;      /* 2800_(8830 - 8833h) */
    union  io_lvds_ctl1           LVDSCTL1;         /* 2800_(8834 - 8837h) */
    union  io_lvds_ctl2           LVDSCTL2;         /* 2800_(8838 - 883Bh) */
    union  io_lvds_clks           LVDSCLKS;         /* 2800_(883C - 883Fh) */
    union  io_lvds_dats           LVDSDATS;         /* 2800_(8840 - 8847h) */
    union  io_lvds_lhsadd         LHSADD;           /* 2800_(8848 - 884Bh) */
    union  io_lvds_lhsize         LHSIZE;           /* 2800_(884C - 884Fh) */
    union  io_lvds_syncsel        SYNCSEL;          /* 2800_(8850 - 8853h) */

    unsigned char dmy_8854_8857[0x8858-0x8854];     /* 2800_(8854 - 8857h) */

    union  io_lvds_lpdchg         LPDCHG;           /* 2800_(8858 - 885Fh) */
    struct io_lvds_sav0           SAV0;             /* 2800_(8860 - 8867h) */
    struct io_lvds_sav1           SAV1;             /* 2800_(8868 - 886Fh) */
    struct io_lvds_sav2           SAV2;             /* 2800_(8870 - 8877h) */
    struct io_lvds_eav0           EAV0;             /* 2800_(8878 - 887Fh) */
    struct io_lvds_eav1           EAV1;             /* 2800_(8880 - 8887h) */
    struct io_lvds_eav2           EAV2;             /* 2800_(8888 - 888Fh) */
    struct io_lvds_sab0           SAB0;             /* 2800_(8890 - 8897h) */
    struct io_lvds_sab1           SAB1;             /* 2800_(8898 - 889Fh) */
    struct io_lvds_sab2           SAB2;             /* 2800_(88A0 - 88A7h) */
    struct io_lvds_eab0           EAB0;             /* 2800_(88A8 - 88AFh) */
    struct io_lvds_eab1           EAB1;             /* 2800_(88B0 - 88B7h) */
    struct io_lvds_eab2           EAB2;             /* 2800_(88B8 - 88BFh) */
    struct io_lvds_msav0          MSAV0;            /* 2800_(88C0 - 88C7h) */
    struct io_lvds_msav1          MSAV1;            /* 2800_(88C8 - 88CFh) */
    struct io_lvds_msav2          MSAV2;            /* 2800_(88D0 - 88D7h) */
    struct io_lvds_meav0          MEAV0;            /* 2800_(88D8 - 88DFh) */
    struct io_lvds_meav1          MEAV1;            /* 2800_(88E0 - 88E7h) */
    struct io_lvds_meav2          MEAV2;            /* 2800_(88E8 - 88EFh) */
    struct io_lvds_msab0          MSAB0;            /* 2800_(88F0 - 88F7h) */
    struct io_lvds_msab1          MSAB1;            /* 2800_(88F8 - 88FFh) */
    struct io_lvds_msab2          MSAB2;            /* 2800_(8900 - 8907h) */
    struct io_lvds_meab0          MEAB0;            /* 2800_(8908 - 890Fh) */
    struct io_lvds_meab1          MEAB1;            /* 2800_(8910 - 8917h) */
    struct io_lvds_meab2          MEAB2;            /* 2800_(8918 - 891Fh) */
    union  io_lvds_paen           LVDSPAEN;         /* 2800_(8920 - 8923h) */

    unsigned char dmy_8924_89FF[0x8A00-0x8924];     /* 2800_(8924 - 89FFh) */

};

#endif  // _IO_PRO_LVDS_H_
