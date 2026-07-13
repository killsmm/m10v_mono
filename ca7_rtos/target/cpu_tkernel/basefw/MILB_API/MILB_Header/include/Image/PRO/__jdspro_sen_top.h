/**
 * @file		__jdspro_sen_top.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_SEN_TOP_H_
#define _IO_PRO_SEN_TOP_H_

// --- REMOVE_BUZ BEGIN
#define CO_USE_BUZ
// --- REMOVE_BUZ END

/*  structure of SR (2800_8000h)    */
union io_sensr{
    unsigned long       word;
    struct {
        unsigned long   SR  :1;
        unsigned long       :31;
    }bit;
};

/*  structure of CLKSTP (2800_8008h)    */
union io_senclkstp_1{
    unsigned long      word;
    struct {
        unsigned long  PSTP1   :1;
        unsigned long          :7;
        unsigned long  PSTP2   :1;
        unsigned long          :7;
        unsigned long  PSTP3   :1;
        unsigned long          :7;
        unsigned long  PSTP4   :1;
        unsigned long          :7;
    }bit;
};
union io_senclkstp_2{
    unsigned long      word;
    struct {
        unsigned long  PSTP5   :1;
        unsigned long          :31;
    }bit;
};
struct io_senclkstp{
    union io_senclkstp_1  CLKSTP1;
    union io_senclkstp_2  CLKSTP2;
};

/*  structure of SENTOPCTL1 (2800_8010h)    */
union io_sentopctl1_1{
    unsigned long      word;
    struct {
        unsigned long  SENIN0      :3;
        unsigned long              :1;
        unsigned long  SENIN1      :3;
        unsigned long              :1;
        unsigned long  SENIN2      :3;
        unsigned long              :1;
        unsigned long  SENIN3      :3;
        unsigned long              :1;
        unsigned long  BLOBSET0    :3;
        unsigned long              :5;
        unsigned long  BLOBSET1    :3;
        unsigned long              :5;
    }bit;
};
union io_sentopctl1_2{
    unsigned long      word;
    struct {
        unsigned long  BLOBSET2    :3;
        unsigned long              :5;
        unsigned long  BLOBSET3    :3;
        unsigned long              :5;
        unsigned long  PWSW0       :4;
        unsigned long              :4;
        unsigned long  PWSW1       :4;
        unsigned long              :4;
    }bit;
};
union io_sentopctl1_3{
    unsigned long      word;
    struct {
        unsigned long  PWSW2       :4;
        unsigned long              :4;
        unsigned long  PWSW3       :4;
        unsigned long              :4;
        unsigned long  PIPE2S      :1;
        unsigned long              :3;
        unsigned long  SENRAMSW    :2;
        unsigned long              :2;
        unsigned long  LDIVRAMSW   :2;
        unsigned long              :2;
        unsigned long  LDIVMCH     :2;
        unsigned long              :2;
    }bit;
};
union io_sentopctl1_4{
    unsigned long      word;
    struct {
        unsigned long              :32;
    }bit;
};
struct io_sentopctl1{
    union io_sentopctl1_1  SENTOPCTL11;
    union io_sentopctl1_2  SENTOPCTL12;
    union io_sentopctl1_3  SENTOPCTL13;
    union io_sentopctl1_4  SENTOPCTL14;
};

/*  structure of SENTOPCTL2 (2800_8020h)    */
union io_sentopctl2_1{
    unsigned long      word;
    struct {
        unsigned long  ORG00   :2;
        unsigned long          :6;
        unsigned long  ORG01   :2;
        unsigned long          :6;
        unsigned long  ORG10   :2;
        unsigned long          :6;
        unsigned long  ORG11   :2;
        unsigned long          :6;
    }bit;
};
union io_sentopctl2_2{
    unsigned long      word;
    struct {
        unsigned long  ORG20   :2;
        unsigned long          :6;
        unsigned long  ORG21   :2;
        unsigned long          :6;
        unsigned long  ORG30   :2;
        unsigned long          :6;
        unsigned long  ORG31   :2;
        unsigned long          :6;
    }bit;
};
struct io_sentopctl2{
    union io_sentopctl2_1  SENTOPCTL2_1;
    union io_sentopctl2_2  SENTOPCTL2_2;
};

/*  structure of BSCTL  (2800_8028h)    */
union io_senbsctl_1{
    unsigned long      word;
    struct {
        unsigned long  BSAMNT0 :3;
        unsigned long          :1;
        unsigned long  BSAMNT1 :3;
        unsigned long          :1;
        unsigned long  BSAMNT2 :3;
        unsigned long          :1;
        unsigned long  BSAMNT3 :3;
        unsigned long          :1;
        unsigned long  BSLMD0  :1;
        unsigned long          :3;
        unsigned long  BSLMD1  :1;
        unsigned long          :3;
        unsigned long  BSLMD2  :1;
        unsigned long          :3;
        unsigned long  BSLMD3  :1;
        unsigned long          :3;
    }bit;
};
union io_senbsctl_2{
    unsigned long      word;
    struct {
        unsigned long  BSLR0   :1;
        unsigned long          :3;
        unsigned long  BSLR1   :1;
        unsigned long          :3;
        unsigned long  BSLR2   :1;
        unsigned long          :3;
        unsigned long  BSLR3   :1;
        unsigned long          :19;
    }bit;
};
struct io_senbsctl{
    union io_senbsctl_1  BSCTL1;
    union io_senbsctl_2  BSCTL2;
};

/*  structure of INTMON1    (2800_8030h)    */
union io_senintmon1{
    unsigned long       word;
    struct {
        unsigned long   PWF0    :1;
        unsigned long   PWF1    :1;
        unsigned long   PWF2    :1;
        unsigned long   PWF3    :1;
        unsigned long   PWF4    :1;
        unsigned long   PWF5    :1;
        unsigned long   PWF6    :1;
        unsigned long   PWF7    :1;
        unsigned long   OBDF00  :1;
        unsigned long   OBDF01  :1;
        unsigned long   OBDF10  :1;
        unsigned long   OBDF11  :1;
        unsigned long   OBDF20  :1;
        unsigned long   OBDF21  :1;
        unsigned long   OBDF30  :1;
        unsigned long   OBDF31  :1;
        unsigned long   OBTF00  :1;
        unsigned long   OBTF01  :1;
        unsigned long   OBTF10  :1;
        unsigned long   OBTF11  :1;
        unsigned long   OBTF20  :1;
        unsigned long   OBTF21  :1;
        unsigned long   OBTF30  :1;
        unsigned long   OBTF31  :1;
        unsigned long           :8;
    }bit;
};

/*  structure of INTMON2    (2800_8034h)    */
union io_senintmon2{
    unsigned long       word;
    struct {
        unsigned long   PWXF0   :1;
        unsigned long   PWXF1   :1;
        unsigned long   PWXF2   :1;
        unsigned long   PWXF3   :1;
        unsigned long   PWXF4   :1;
        unsigned long   PWXF5   :1;
        unsigned long   PWXF6   :1;
        unsigned long   PWXF7   :1;
        unsigned long   PWEF0   :1;
        unsigned long   PWEF1   :1;
        unsigned long   PWEF2   :1;
        unsigned long   PWEF3   :1;
        unsigned long   PWEF4   :1;
        unsigned long   PWEF5   :1;
        unsigned long   PWEF6   :1;
        unsigned long   PWEF7   :1;
        unsigned long   LDIVEF0 :1;
        unsigned long   LDIVEF1 :1;
        unsigned long   LDIVEF2 :1;
        unsigned long   LDIVEF3 :1;
        unsigned long           :12;
    }bit;
};

/*  structure of INTCTL (2800_8038h)    */
union io_senintctl{
    unsigned long       word;
    struct {
        unsigned long   INTMD   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of VHDINTENB  (2800_8040h)    */
union io_vhdintenb_1{
    unsigned long      word;
    struct {
        unsigned long  VDE00   :1;
        unsigned long          :3;
        unsigned long  VDE01   :1;
        unsigned long          :3;
        unsigned long  VDE10   :1;
        unsigned long          :3;
        unsigned long  VDE11   :1;
        unsigned long          :3;
        unsigned long  VDE20   :1;
        unsigned long          :3;
        unsigned long  VDE21   :1;
        unsigned long          :3;
        unsigned long  VDE30   :1;
        unsigned long          :3;
        unsigned long  VDE31   :1;
        unsigned long          :3;
    }bit;
};
union io_vhdintenb_2{
    unsigned long      word;
    struct {
        unsigned long  HDE00   :1;
        unsigned long          :3;
        unsigned long  HDE01   :1;
        unsigned long          :3;
        unsigned long  HDE10   :1;
        unsigned long          :3;
        unsigned long  HDE11   :1;
        unsigned long          :3;
        unsigned long  HDE20   :1;
        unsigned long          :3;
        unsigned long  HDE21   :1;
        unsigned long          :3;
        unsigned long  HDE30   :1;
        unsigned long          :3;
        unsigned long  HDE31   :1;
        unsigned long          :3;
    }bit;
};
struct io_vhdintenb{
    union io_vhdintenb_1  VHDINTENB1;
    union io_vhdintenb_2  VHDINTENB2;
};

/*  structure of VHDINTFLG  (2800_8048h)    */
union io_vhdintflg_1{
    unsigned long      word;
    struct {
        unsigned long  __VDF00 :1;
        unsigned long          :3;
        unsigned long  __VDF01 :1;
        unsigned long          :3;
        unsigned long  __VDF10 :1;
        unsigned long          :3;
        unsigned long  __VDF11 :1;
        unsigned long          :3;
        unsigned long  __VDF20 :1;
        unsigned long          :3;
        unsigned long  __VDF21 :1;
        unsigned long          :3;
        unsigned long  __VDF30 :1;
        unsigned long          :3;
        unsigned long  __VDF31 :1;
        unsigned long          :3;
    }bit;
};
union io_vhdintflg_2{
    unsigned long      word;
    struct {
        unsigned long  __HDF00   :1;
        unsigned long            :3;
        unsigned long  __HDF01   :1;
        unsigned long            :3;
        unsigned long  __HDF10   :1;
        unsigned long            :3;
        unsigned long  __HDF11   :1;
        unsigned long            :3;
        unsigned long  __HDF20   :1;
        unsigned long            :3;
        unsigned long  __HDF21   :1;
        unsigned long            :3;
        unsigned long  __HDF30   :1;
        unsigned long            :3;
        unsigned long  __HDF31   :1;
        unsigned long            :3;
    }bit;
};
struct io_vhdintflg{
    union io_vhdintflg_1  VHDINTFLG1;
    union io_vhdintflg_2  VHDINTFLG2;
};

/*  structure of VHSET0 (2800_8050h)    */
union io_vhset0_1{
    unsigned long      word;
    struct {
        unsigned long  VHSET00 :1;
        unsigned long          :31;
    }bit;
};
union io_vhset0_2{
    unsigned long      word;
    struct {
        unsigned long  VHSET01 :1;
        unsigned long          :31;
    }bit;
};
struct io_vhset0{
    union io_vhset0_1  VHSET0_1;
    union io_vhset0_2  VHSET0_2;
};

/*  structure of VHSET1 (2800_8058h)    */
union io_vhset1_1{
    unsigned long      word;
    struct {
        unsigned long  VHSET10 :1;
        unsigned long          :31;
    }bit;
};
union io_vhset1_2{
    unsigned long      word;
    struct {
        unsigned long  VHSET11 :1;
        unsigned long          :31;
    }bit;
};
struct io_vhset1{
    union io_vhset1_1  VHSET1_1;
    union io_vhset1_2  VHSET1_2;
};

/*  structure of VHSET2 (2800_8060h)    */
union io_vhset2_1{
    unsigned long      word;
    struct {
        unsigned long  VHSET20 :1;
        unsigned long          :31;
    }bit;
};
union io_vhset2_2{
    unsigned long      word;
    struct {
        unsigned long  VHSET21 :1;
        unsigned long          :31;
    }bit;
};
struct io_vhset2{
    union io_vhset2_1  VHSET2_1;
    union io_vhset2_2  VHSET2_2;
};

/*  structure of VHSET3 (2800_8068h)    */
union io_vhset3_1{
    unsigned long      word;
    struct {
        unsigned long  VHSET30 :1;
        unsigned long          :31;
    }bit;
};
union io_vhset3_2{
    unsigned long      word;
    struct {
        unsigned long  VHSET31 :1;
        unsigned long          :31;
    }bit;
};
struct io_vhset3{
    union io_vhset3_1  VHSET3_1;
    union io_vhset3_2  VHSET3_2;
};

/*  structure of VMIVADR    (2800_8070h)    */
union io_vmivadr_1{
    unsigned long      word;
    struct {
        unsigned long  VMIVADR00   :14;
        unsigned long              :2;
        unsigned long  VMIVADR01   :14;
        unsigned long              :2;
    }bit;
};
union io_vmivadr_2{
    unsigned long      word;
    struct {
        unsigned long  VMIVADR10   :14;
        unsigned long              :2;
        unsigned long  VMIVADR11   :14;
        unsigned long              :2;
    }bit;
};
union io_vmivadr_3{
    unsigned long      word;
    struct {
        unsigned long  VMIVADR20   :14;
        unsigned long              :2;
        unsigned long  VMIVADR21   :14;
        unsigned long              :2;
    }bit;
};
union io_vmivadr_4{
    unsigned long      word;
    struct {
        unsigned long  VMIVADR30   :14;
        unsigned long              :2;
        unsigned long  VMIVADR31   :14;
        unsigned long              :2;
    }bit;
};
struct io_vmivadr{
    union io_vmivadr_1 VMIVADR1;
    union io_vmivadr_2 VMIVADR2;
    union io_vmivadr_3 VMIVADR3;
    union io_vmivadr_4 VMIVADR4;
};

/*  structure of VMIHADR    (2800_8080h)    */
union io_vmihadr_1{
    unsigned long      word;
    struct {
        unsigned long  VMIHADR00   :14;
        unsigned long              :2;
        unsigned long  VMIHADR01   :14;
        unsigned long              :2;
    }bit;
};
union io_vmihadr_2{
    unsigned long      word;
    struct {
        unsigned long  VMIHADR10   :14;
        unsigned long              :2;
        unsigned long  VMIHADR11   :14;
        unsigned long              :2;
    }bit;
};
union io_vmihadr_3{
    unsigned long      word;
    struct {
        unsigned long  VMIHADR20   :14;
        unsigned long              :2;
        unsigned long  VMIHADR21   :14;
        unsigned long              :2;
    }bit;
};
union io_vmihadr_4{
    unsigned long      word;
    struct {
        unsigned long  VMIHADR30   :14;
        unsigned long              :2;
        unsigned long  VMIHADR31   :14;
        unsigned long              :2;
    }bit;
};
struct io_vmihadr{
    union io_vmihadr_1 VMIHADR1;
    union io_vmihadr_2 VMIHADR2;
    union io_vmihadr_3 VMIHADR3;
    union io_vmihadr_4 VMIHADR4;
};

/*  structure of HMIHADR    (2800_8090h)    */
union io_hmihadr_1{
    unsigned long      word;
    struct {
        unsigned long  HMIHADR00   :14;
        unsigned long              :2;
        unsigned long  HMIHADR01   :14;
        unsigned long              :2;
    }bit;
};
union io_hmihadr_2{
    unsigned long      word;
    struct {
        unsigned long  HMIHADR10   :14;
        unsigned long              :2;
        unsigned long  HMIHADR11   :14;
        unsigned long              :2;
    }bit;
};
union io_hmihadr_3{
    unsigned long      word;
    struct {
        unsigned long  HMIHADR20   :14;
        unsigned long              :2;
        unsigned long  HMIHADR21   :14;
        unsigned long              :2;
    }bit;
};
union io_hmihadr_4{
    unsigned long      word;
    struct {
        unsigned long  HMIHADR30   :14;
        unsigned long              :2;
        unsigned long  HMIHADR31   :14;
        unsigned long              :2;
    }bit;
};
struct io_hmihadr{
    union io_hmihadr_1 HMIHADR1;
    union io_hmihadr_2 HMIHADR2;
    union io_hmihadr_3 HMIHADR3;
    union io_hmihadr_4 HMIHADR4;
};

/*  structure of SGVHDINTENB    (2800_80A0h)    */
union io_sgvhdintenb{
    unsigned long       word;
    struct {
        unsigned long   VDE_SG0 :1;
        unsigned long           :3;
        unsigned long   HDE_SG0 :1;
        unsigned long           :3;
        unsigned long   VDE_SG1 :1;
        unsigned long           :3;
        unsigned long   HDE_SG1 :1;
        unsigned long           :3;
        unsigned long   VDE_SG2 :1;
        unsigned long           :3;
        unsigned long   HDE_SG2 :1;
        unsigned long           :3;
        unsigned long   VDE_SG3 :1;
        unsigned long           :3;
        unsigned long   HDE_SG3 :1;
        unsigned long           :3;
    }bit;
};

/*  structure of SGVHDINTFLG    (2800_80A4h)    */
union io_sgvhdintflg{
    unsigned long       word;
    struct {
        unsigned long   __VDF_SG0 :1;
        unsigned long             :3;
        unsigned long   __HDF_SG0 :1;
        unsigned long             :3;
        unsigned long   __VDF_SG1 :1;
        unsigned long             :3;
        unsigned long   __HDF_SG1 :1;
        unsigned long             :3;
        unsigned long   __VDF_SG2 :1;
        unsigned long             :3;
        unsigned long   __HDF_SG2 :1;
        unsigned long             :3;
        unsigned long   __VDF_SG3 :1;
        unsigned long             :3;
        unsigned long   __HDF_SG3 :1;
        unsigned long             :3;
    }bit;
};

/*  structure of VHDSGCTL   (2800_80A8h)    */
union io_vhdsgctl_1{
    unsigned long      word;
    struct {
        unsigned long  SGVDEG0 :1;
        unsigned long          :7;
        unsigned long  SGHDEG0 :1;
        unsigned long          :7;
        unsigned long  SGVDEG1 :1;
        unsigned long          :7;
        unsigned long  SGHDEG1 :1;
        unsigned long          :7;
    }bit;
};
union io_vhdsgctl_2{
    unsigned long      word;
    struct {
        unsigned long  SGVDEG2 :1;
        unsigned long          :7;
        unsigned long  SGHDEG2 :1;
        unsigned long          :7;
        unsigned long  SGVDEG3 :1;
        unsigned long          :7;
        unsigned long  SGHDEG3 :1;
        unsigned long          :7;
    }bit;
};
struct io_vhdsgctl{
    union io_vhdsgctl_1 VHDSGCTL1;
    union io_vhdsgctl_2 VHDSGCTL2;
};

/*  structure of OBTCDCSET  (2800_80B0h)    */
union io_obtcdcset{
    unsigned long       word;
    struct {
        unsigned long   OBTCDCSET   :1;
        unsigned long               :31;
    }bit;
};

/*  structure of OBTCDCCTL1 (2800_80B4h)    */
union io_obtcdcctl1{
    unsigned long       word;
    struct {
        unsigned long   OBTCPAT :2;
        unsigned long           :30;
    }bit;
};

/*  structure of OBTCDCCTL2 (2800_80B8h)    */
union io_obtcdcctl2_1{
    unsigned long      word;
    struct {
        unsigned long  OBTDWC00    :8;
        unsigned long  OBTDWC01    :8;
        unsigned long  OBTDWC10    :8;
        unsigned long  OBTDWC11    :8;
    }bit;
};
union io_obtcdcctl2_2{
    unsigned long      word;
    struct {
        unsigned long  OBTDWC20    :8;
        unsigned long  OBTDWC21    :8;
        unsigned long  OBTDWC30    :8;
        unsigned long  OBTDWC31    :8;
    }bit;
};
struct io_obtcdcctl2{
    union io_obtcdcctl2_1 OBTCDCCTL2_1;
    union io_obtcdcctl2_2 OBTCDCCTL2_2;
};

/*  structure of LVDSCTL    (2800_80C0h)    */
union io_lvdsctl{
    unsigned long       word;
    struct {
        unsigned long   LVDSCKPD    :4;
        unsigned long               :12;
        unsigned long   LVDSDLPD    :16;
    }bit;
};

/*  structure of SLVSECCTL  (2800_80C4h)    */
union io_slvsecctl{
    unsigned long       word;
    struct {
        unsigned long   SLVSECSEL       :1;
        unsigned long                   :3;
        unsigned long   PHY0_RXPDCK     :1;
        unsigned long                   :3;
        unsigned long   PHY1_RXPDCK     :1;
        unsigned long                   :3;
        unsigned long   PHY2_RXPDCK     :1;
        unsigned long                   :3;
        unsigned long   PHY3_RXPDCK     :1;
        unsigned long                   :3;
        unsigned long   PHY0_RXPCLK2AEN :1;
        unsigned long                   :3;
        unsigned long   PHY1_RXPCLK2AEN :1;
        unsigned long                   :7;
    }bit;
};

/*  structure of SGOUTCTL   (2800_80C8h)    */
union io_sgoutctl{
    unsigned long       word;
    struct {
        unsigned long   SGSEL0  :2;
        unsigned long           :2;
        unsigned long   SGSEL1  :2;
        unsigned long           :2;
        unsigned long   SGSEL2  :2;
        unsigned long           :2;
        unsigned long   SGSEL3  :2;
        unsigned long           :18;
    }bit;
};

/*  structure of GINTENB    (2800_80CCh)    */
union io_gintenb{
    unsigned long       word;
    struct {
        unsigned long   SOFE0   :1;
        unsigned long           :3;
        unsigned long   SOFE1   :1;
        unsigned long           :3;
        unsigned long   SOFE2   :1;
        unsigned long           :3;
        unsigned long   SOFE3   :1;
        unsigned long           :3;
        unsigned long   EOFE0   :1;
        unsigned long           :3;
        unsigned long   EOFE1   :1;
        unsigned long           :3;
        unsigned long   EOFE2   :1;
        unsigned long           :3;
        unsigned long   EOFE3   :1;
        unsigned long           :3;
    }bit;
};

/*  structure of GINTFLG    (2800_80D0h)    */
union io_gintflg{
    unsigned long       word;
    struct {
        unsigned long   __SOFF0   :1;
        unsigned long             :3;
        unsigned long   __SOFF1   :1;
        unsigned long             :3;
        unsigned long   __SOFF2   :1;
        unsigned long             :3;
        unsigned long   __SOFF3   :1;
        unsigned long             :3;
        unsigned long   __EOFF0   :1;
        unsigned long             :3;
        unsigned long   __EOFF1   :1;
        unsigned long             :3;
        unsigned long   __EOFF2   :1;
        unsigned long             :3;
        unsigned long   __EOFF3   :1;
        unsigned long             :3;
    }bit;
};

/*  structure of GSET0  (2800_80D4h)    */
union io_gset0{
    unsigned long       word;
    struct {
        unsigned long   GSET0   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of GSET1  (2800_80D8h)    */
union io_gset1{
    unsigned long       word;
    struct {
        unsigned long   GSET1   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of GSET2  (2800_80DCh)    */
union io_gset2{
    unsigned long       word;
    struct {
        unsigned long   GSET2   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of GSET3  (2800_80E0h)    */
union io_gset3{
    unsigned long       word;
    struct {
        unsigned long   GSET3   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of GVADR  (2800_80E8h)    */
union io_gvadr_1{
    unsigned long      word;
    struct {
        unsigned long  GVADR0  :14;
        unsigned long          :2;
        unsigned long  GVADR1  :14;
        unsigned long          :2;
    }bit;
};
union io_gvadr_2{
    unsigned long      word;
    struct {
        unsigned long  GVADR2  :14;
        unsigned long          :2;
        unsigned long  GVADR3  :14;
        unsigned long          :2;
    }bit;
};
struct io_gvadr{
    union io_gvadr_1 GVADR1;
    union io_gvadr_2 GVADR2;
};

/*  structure of GHADR  (2800_80F0h)    */
union io_ghadr_1{
    unsigned long      word;
    struct {
        unsigned long  GHADR0  :14;
        unsigned long          :2;
        unsigned long  GHADR1  :14;
        unsigned long          :2;
    }bit;
};
union io_ghadr_2{
    unsigned long      word;
    struct {
        unsigned long  GHADR2  :14;
        unsigned long          :2;
        unsigned long  GHADR3  :14;
        unsigned long          :2;
    }bit;
};
struct io_ghadr{
    union io_ghadr_1 GHADR1;
    union io_ghadr_2 GHADR2;
};

/*  structure of PHYCIFCTL  (2800_8100h)    */
union io_phycifctl_1{
    unsigned long      word;
    struct {
        unsigned long  PHY_PREGCNT     :8;
        unsigned long  PHY_FREQCNT     :8;
        unsigned long  PHY_PDCCNT      :1;
        unsigned long                  :3;
        unsigned long  PHY_SETDF       :1;
        unsigned long                  :3;
        unsigned long  PHY_PICODETST   :6;
        unsigned long                  :2;
    }bit;
};
union io_phycifctl_2{
    unsigned long      word;
    struct {
        unsigned long  PHY_PMONSEL     :2;
        unsigned long                  :2;
        unsigned long  PHY_DMONSEL     :1;
        unsigned long                  :3;
        unsigned long  PHY_CNTSEL      :8;
        unsigned long  PHY_MULGAINF    :2;
        unsigned long                  :2;
        unsigned long  PHY_MULGAINP    :2;
        unsigned long                  :10;
    }bit;
};
union io_phycifctl_3{
    unsigned long       word;
    struct {
        unsigned long   PHY_CDRFUNC     :25;
        unsigned long                   :7;
    }bit;
};
struct io_phycifctl{
    union io_phycifctl_1   PHYCIFCTL1;
    union io_phycifctl_2   PHYCIFCTL2;
    union io_phycifctl_3   PHYCIFCTL3;
};

/*  structure of PHY0CLKCTL (2800_8110h)    */
union io_phy0clkctl{
    unsigned long       word;
    struct {
        unsigned long   PHY0_CLKLANESEL :2;
        unsigned long                   :2;
        unsigned long   PHY0_CLKMODESEL :3;
        unsigned long                   :1;
        unsigned long   PHY0_CLKOUTREN  :1;
        unsigned long                   :3;
        unsigned long   PHY0_CLKOUTLEN  :1;
        unsigned long                   :19;
    }bit;
};

/*  structure of PHY1CLKCTL (2800_8114h)    */
union io_phy1clkctl{
    unsigned long       word;
    struct {
        unsigned long   PHY1_CLKLANESEL :2;
        unsigned long                   :2;
        unsigned long   PHY1_CLKMODESEL :3;
        unsigned long                   :1;
        unsigned long   PHY1_CLKOUTREN  :1;
        unsigned long                   :3;
        unsigned long   PHY1_CLKOUTLEN  :1;
        unsigned long                   :19;
    }bit;
};

/*  structure of PHY2CLKCTL (2800_8118h)    */
union io_phy2clkctl{
    unsigned long       word;
    struct {
        unsigned long   PHY2_CLKLANESEL :2;
        unsigned long                   :2;
        unsigned long   PHY2_CLKMODESEL :3;
        unsigned long                   :1;
        unsigned long   PHY2_CLKOUTREN  :1;
        unsigned long                   :3;
        unsigned long   PHY2_CLKOUTLEN  :1;
        unsigned long                   :19;
    }bit;
};

/*  structure of PHY3CLKCTL (2800_811Ch)    */
union io_phy3clkctl{
    unsigned long       word;
    struct {
        unsigned long   PHY3_CLKLANESEL :2;
        unsigned long                   :2;
        unsigned long   PHY3_CLKMODESEL :3;
        unsigned long                   :1;
        unsigned long   PHY3_CLKOUTREN  :1;
        unsigned long                   :3;
        unsigned long   PHY3_CLKOUTLEN  :1;
        unsigned long                   :19;
    }bit;
};

/*  structure of PHY0MDCTL  (2800_8120h)    */
union io_phy0mdctl{
    unsigned long       word;
    struct {
        unsigned long   PHY0_BITWIDTHSEL    :3;
        unsigned long                       :5;
        unsigned long   PHY0_RXMODE         :5;
        unsigned long                       :3;
        unsigned long   PHY0_PIMODE         :2;
        unsigned long                       :2;
        unsigned long   PHY0_PICNT          :2;
        unsigned long                       :10;
    }bit;
};

/*  structure of PHY1MDCTL  (2800_8124h)    */
union io_phy1mdctl{
    unsigned long       word;
    struct {
        unsigned long   PHY1_BITWIDTHSEL    :3;
        unsigned long                       :5;
        unsigned long   PHY1_RXMODE         :5;
        unsigned long                       :3;
        unsigned long   PHY1_PIMODE         :2;
        unsigned long                       :2;
        unsigned long   PHY1_PICNT          :2;
        unsigned long                       :10;
    }bit;
};

/*  structure of PHY2MDCTL  (2800_8128h)    */
union io_phy2mdctl{
    unsigned long       word;
    struct {
        unsigned long   PHY2_BITWIDTHSEL    :3;
        unsigned long                       :5;
        unsigned long   PHY2_RXMODE         :5;
        unsigned long                       :3;
        unsigned long   PHY2_PIMODE         :2;
        unsigned long                       :2;
        unsigned long   PHY2_PICNT          :2;
        unsigned long                       :10;
    }bit;
};

/*  structure of PHY3MDCTL  (2800_812Ch)    */
union io_phy3mdctl{
    unsigned long       word;
    struct {
        unsigned long   PHY3_BITWIDTHSEL    :3;
        unsigned long                       :5;
        unsigned long   PHY3_RXMODE         :5;
        unsigned long                       :3;
        unsigned long   PHY3_PIMODE         :2;
        unsigned long                       :2;
        unsigned long   PHY3_PICNT          :2;
        unsigned long                       :10;
    }bit;
};

/*  structure of PHY0MDCPCTL    (2800_8130h)    */
union io_phy0mdcpctl_1{
    unsigned long       word;
    struct {
        unsigned long   PHY0_ZEROP3UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY0_ZEROP4UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY0_ZEROP5UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY0_ZEROP6UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY0_DETCNT_CP0     :8;
        unsigned long   PHY0_EQ_EN_CP0      :1;
        unsigned long                       :3;
        unsigned long   PHY0_EQ_SET_CP0     :3;
        unsigned long                       :1;
    }bit;
};
union io_phy0mdcpctl_2{
    unsigned long       word;
    struct {
        unsigned long   PHY0_ZEROP3UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY0_ZEROP4UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY0_ZEROP5UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY0_ZEROP6UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY0_DETCNT_CP1     :8;
        unsigned long   PHY0_EQ_EN_CP1      :1;
        unsigned long                       :3;
        unsigned long   PHY0_EQ_SET_CP1     :3;
        unsigned long                       :1;
    }bit;
};
union io_phy0mdcpctl_3{
    unsigned long       word;
    struct {
        unsigned long   PHY0_ZEROP3UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY0_ZEROP4UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY0_ZEROP5UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY0_ZEROP6UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY0_DETCNT_CP2     :8;
        unsigned long   PHY0_EQ_EN_CP2      :1;
        unsigned long                       :3;
        unsigned long   PHY0_EQ_SET_CP2     :3;
        unsigned long                       :1;
    }bit;
};
struct io_phy0mdcpctl{
    union io_phy0mdcpctl_1 PHY0MDCPCTL1;
    union io_phy0mdcpctl_2 PHY0MDCPCTL2;
    union io_phy0mdcpctl_3 PHY0MDCPCTL3;
};

/*  structure of PHY1MDCPCTL    (2800_8140h)    */
union io_phy1mdcpctl_1{
    unsigned long       word;
    struct {
        unsigned long   PHY1_ZEROP3UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY1_ZEROP4UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY1_ZEROP5UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY1_ZEROP6UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY1_DETCNT_CP0     :8;
        unsigned long   PHY1_EQ_EN_CP0      :1;
        unsigned long                       :3;
        unsigned long   PHY1_EQ_SET_CP0     :3;
        unsigned long                       :1;
    }bit;
};
union io_phy1mdcpctl_2{
    unsigned long       word;
    struct {
        unsigned long   PHY1_ZEROP3UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY1_ZEROP4UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY1_ZEROP5UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY1_ZEROP6UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY1_DETCNT_CP1     :8;
        unsigned long   PHY1_EQ_EN_CP1      :1;
        unsigned long                       :3;
        unsigned long   PHY1_EQ_SET_CP1     :3;
        unsigned long                       :1;
    }bit;
};
union io_phy1mdcpctl_3{
    unsigned long       word;
    struct {
        unsigned long   PHY1_ZEROP3UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY1_ZEROP4UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY1_ZEROP5UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY1_ZEROP6UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY1_DETCNT_CP2     :8;
        unsigned long   PHY1_EQ_EN_CP2      :1;
        unsigned long                       :3;
        unsigned long   PHY1_EQ_SET_CP2     :3;
        unsigned long                       :1;
    }bit;
};
struct io_phy1mdcpctl{
    union io_phy1mdcpctl_1 PHY1MDCPCTL1;
    union io_phy1mdcpctl_2 PHY1MDCPCTL2;
    union io_phy1mdcpctl_3 PHY1MDCPCTL3;
};

/*  structure of PHY2MDCPCTL    (2800_8150h)    */
union io_phy2mdcpctl_1{
    unsigned long       word;
    struct {
        unsigned long   PHY2_ZEROP3UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY2_ZEROP4UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY2_ZEROP5UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY2_ZEROP6UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY2_DETCNT_CP0     :8;
        unsigned long   PHY2_EQ_EN_CP0      :1;
        unsigned long                       :3;
        unsigned long   PHY2_EQ_SET_CP0     :3;
        unsigned long                       :1;
    }bit;
};
union io_phy2mdcpctl_2{
    unsigned long       word;
    struct {
        unsigned long   PHY2_ZEROP3UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY2_ZEROP4UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY2_ZEROP5UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY2_ZEROP6UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY2_DETCNT_CP1     :8;
        unsigned long   PHY2_EQ_EN_CP1      :1;
        unsigned long                       :3;
        unsigned long   PHY2_EQ_SET_CP1     :3;
        unsigned long                       :1;
    }bit;
};
union io_phy2mdcpctl_3{
    unsigned long       word;
    struct {
        unsigned long   PHY2_ZEROP3UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY2_ZEROP4UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY2_ZEROP5UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY2_ZEROP6UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY2_DETCNT_CP2     :8;
        unsigned long   PHY2_EQ_EN_CP2      :1;
        unsigned long                       :3;
        unsigned long   PHY2_EQ_SET_CP2     :3;
        unsigned long                       :1;
    }bit;
};
struct io_phy2mdcpctl{
    union io_phy2mdcpctl_1 PHY2MDCPCTL1;
    union io_phy2mdcpctl_2 PHY2MDCPCTL2;
    union io_phy2mdcpctl_3 PHY2MDCPCTL3;
};

/*  structure of PHY3MDCPCTL    (2800_8160h)    */
union io_phy3mdcpctl_1{
    unsigned long       word;
    struct {
        unsigned long   PHY3_ZEROP3UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY3_ZEROP4UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY3_ZEROP5UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY3_ZEROP6UICP0    :1;
        unsigned long                       :3;
        unsigned long   PHY3_DETCNT_CP0     :8;
        unsigned long   PHY3_EQ_EN_CP0      :1;
        unsigned long                       :3;
        unsigned long   PHY3_EQ_SET_CP0     :3;
        unsigned long                       :1;
    }bit;
};
union io_phy3mdcpctl_2{
    unsigned long       word;
    struct {
        unsigned long   PHY3_ZEROP3UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY3_ZEROP4UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY3_ZEROP5UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY3_ZEROP6UICP1    :1;
        unsigned long                       :3;
        unsigned long   PHY3_DETCNT_CP1     :8;
        unsigned long   PHY3_EQ_EN_CP1      :1;
        unsigned long                       :3;
        unsigned long   PHY3_EQ_SET_CP1     :3;
        unsigned long                       :1;
    }bit;
};
union io_phy3mdcpctl_3{
    unsigned long       word;
    struct {
        unsigned long   PHY3_ZEROP3UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY3_ZEROP4UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY3_ZEROP5UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY3_ZEROP6UICP2    :1;
        unsigned long                       :3;
        unsigned long   PHY3_DETCNT_CP2     :8;
        unsigned long   PHY3_EQ_EN_CP2      :1;
        unsigned long                       :3;
        unsigned long   PHY3_EQ_SET_CP2     :3;
        unsigned long                       :1;
    }bit;
};
struct io_phy3mdcpctl{
    union io_phy3mdcpctl_1 PHY3MDCPCTL1;
    union io_phy3mdcpctl_2 PHY3MDCPCTL2;
    union io_phy3mdcpctl_3 PHY3MDCPCTL3;
};

/*  structure of PHY0DLCTL  (2800_8170h)    */
union io_phy0dlctl{
    unsigned long       word;
    struct {
        unsigned long   PHY0_D0RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY0_D1RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY0_D2RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY0_D3RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY0_SKWSEL_CP0 :2;
        unsigned long                   :2;
        unsigned long   PHY0_SKWSEL_CP1 :2;
        unsigned long                   :2;
        unsigned long   PHY0_SKWSEL_CP2 :2;
        unsigned long                   :6;
    }bit;
};

/*  structure of PHY1DLCTL  (2800_8174h)    */
union io_phy1dlctl{
    unsigned long       word;
    struct {
        unsigned long   PHY1_D0RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY1_D1RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY1_D2RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY1_D3RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY1_SKWSEL_CP0 :2;
        unsigned long                   :2;
        unsigned long   PHY1_SKWSEL_CP1 :2;
        unsigned long                   :2;
        unsigned long   PHY1_SKWSEL_CP2 :2;
        unsigned long                   :6;
    }bit;
};

/*  structure of PHY2DLCTL  (2800_8178h)    */
union io_phy2dlctl{
    unsigned long       word;
    struct {
        unsigned long   PHY2_D0RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY2_D1RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY2_D2RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY2_D3RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY2_SKWSEL_CP0 :2;
        unsigned long                   :2;
        unsigned long   PHY2_SKWSEL_CP1 :2;
        unsigned long                   :2;
        unsigned long   PHY2_SKWSEL_CP2 :2;
        unsigned long                   :6;
    }bit;
};

/*  structure of PHY3DLCTL  (2800_817Ch)    */
union io_phy3dlctl{
    unsigned long       word;
    struct {
        unsigned long   PHY3_D0RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY3_D1RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY3_D2RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY3_D3RXSPEED  :3;
        unsigned long                   :1;
        unsigned long   PHY3_SKWSEL_CP0 :2;
        unsigned long                   :2;
        unsigned long   PHY3_SKWSEL_CP1 :2;
        unsigned long                   :2;
        unsigned long   PHY3_SKWSEL_CP2 :2;
        unsigned long                   :6;
    }bit;
};

/*  structure of PHY0IFCTL  (2800_8180h)    */
union io_phy0ifctl_1{
    unsigned long       word;
    struct {
        unsigned long  PHY0_CLKSETSEL       :8;
        unsigned long  PHY0_D0SETSEL        :8;
        unsigned long  PHY0_D1SETSEL        :8;
        unsigned long  PHY0_D2SETSEL        :8;
    }bit;
};
union io_phy0ifctl_2{
    unsigned long       word;
    struct {
        unsigned long  PHY0_D3SETSEL        :8;
        unsigned long  PHY0_CPHYSET_CP0     :8;
        unsigned long  PHY0_CPHYSET_CP1     :8;
        unsigned long  PHY0_CPHYSET_CP2     :8;
    }bit;
};
union io_phy0ifctl_3{
    unsigned long       word;
    struct {
        unsigned long  PHY0_FUNCSELCK       :16;
        unsigned long                       :16;
    }bit;
};
union io_phy0ifctl_4{
    unsigned long       word;
    struct {
        unsigned long  PHY0_FUNCSELD0       :32;
    }bit;
};
union io_phy0ifctl_5{
    unsigned long       word;
    struct {
        unsigned long  PHY0_FUNCSELD1       :32;
    }bit;
};
union io_phy0ifctl_6{
    unsigned long       word;
    struct {
        unsigned long  PHY0_FUNCSELD2       :32;
    }bit;
};
union io_phy0ifctl_7{
    unsigned long       word;
    struct {
        unsigned long   PHY0_FUNCSELD3      :32;
    }bit;
};
struct io_phy0ifctl{
    union io_phy0ifctl_1   PHY0IFCTL1;
    union io_phy0ifctl_2   PHY0IFCTL2;
    union io_phy0ifctl_3   PHY0IFCTL3;
    union io_phy0ifctl_4   PHY0IFCTL4;
    union io_phy0ifctl_5   PHY0IFCTL5;
    union io_phy0ifctl_6   PHY0IFCTL6;
    union io_phy0ifctl_7   PHY0IFCTL7;
};

/*  structure of PHY1IFCTL  (2800_81A0h)    */
union io_phy1ifctl_1{
    unsigned long       word;
    struct {
        unsigned long  PHY1_CLKSETSEL       :8;
        unsigned long  PHY1_D0SETSEL        :8;
        unsigned long  PHY1_D1SETSEL        :8;
        unsigned long  PHY1_D2SETSEL        :8;
    }bit;
};
union io_phy1ifctl_2{
    unsigned long       word;
    struct {
        unsigned long  PHY1_D3SETSEL        :8;
        unsigned long  PHY1_CPHYSET_CP0     :8;
        unsigned long  PHY1_CPHYSET_CP1     :8;
        unsigned long  PHY1_CPHYSET_CP2     :8;
    }bit;
};
union io_phy1ifctl_3{
    unsigned long       word;
    struct {
        unsigned long  PHY1_FUNCSELCK       :16;
        unsigned long                       :16;
    }bit;
};
union io_phy1ifctl_4{
    unsigned long       word;
    struct {
        unsigned long  PHY1_FUNCSELD0       :32;
    }bit;
};
union io_phy1ifctl_5{
    unsigned long       word;
    struct {
        unsigned long  PHY1_FUNCSELD1       :32;
    }bit;
};
union io_phy1ifctl_6{
    unsigned long       word;
    struct {
        unsigned long  PHY1_FUNCSELD2       :32;
    }bit;
};
union io_phy1ifctl_7{
    unsigned long       word;
    struct {
        unsigned long   PHY1_FUNCSELD3      :32;
    }bit;
};
struct io_phy1ifctl{
    union io_phy1ifctl_1   PHY1IFCTL1;
    union io_phy1ifctl_2   PHY1IFCTL2;
    union io_phy1ifctl_3   PHY1IFCTL3;
    union io_phy1ifctl_4   PHY1IFCTL4;
    union io_phy1ifctl_5   PHY1IFCTL5;
    union io_phy1ifctl_6   PHY1IFCTL6;
    union io_phy1ifctl_7   PHY1IFCTL7;
};

/*  structure of PHY2IFCTL  (2800_81C0h)    */
union io_phy2ifctl_1{
    unsigned long       word;
    struct {
        unsigned long   PHY2_CLKSETSEL      :8;
        unsigned long   PHY2_D0SETSEL       :8;
        unsigned long   PHY2_D1SETSEL       :8;
        unsigned long   PHY2_D2SETSEL       :8;
    }bit;
};
union io_phy2ifctl_2{
    unsigned long       word;
    struct {
        unsigned long   PHY2_D3SETSEL       :8;
        unsigned long   PHY2_CPHYSET_CP0    :8;
        unsigned long   PHY2_CPHYSET_CP1    :8;
        unsigned long   PHY2_CPHYSET_CP2    :8;
    }bit;
};
union io_phy2ifctl_3{
    unsigned long       word;
    struct {
        unsigned long   PHY2_FUNCSELCK      :16;
        unsigned long                       :16;
    }bit;
};
union io_phy2ifctl_4{
    unsigned long       word;
    struct {
        unsigned long   PHY2_FUNCSELD0      :32;
    }bit;
};
union io_phy2ifctl_5{
    unsigned long       word;
    struct {
        unsigned long   PHY2_FUNCSELD1      :32;
    }bit;
};
union io_phy2ifctl_6{
    unsigned long       word;
    struct {
        unsigned long   PHY2_FUNCSELD2      :32;
    }bit;
};
union io_phy2ifctl_7{
    unsigned long       word;
    struct {
        unsigned long   PHY2_FUNCSELD3      :32;
    }bit;
};
struct io_phy2ifctl{
    union io_phy2ifctl_1   PHY2IFCTL1;
    union io_phy2ifctl_2   PHY2IFCTL2;
    union io_phy2ifctl_3   PHY2IFCTL3;
    union io_phy2ifctl_4   PHY2IFCTL4;
    union io_phy2ifctl_5   PHY2IFCTL5;
    union io_phy2ifctl_6   PHY2IFCTL6;
    union io_phy2ifctl_7   PHY2IFCTL7;
};

/*  structure of PHY3IFCTL  (2800_81E0h)    */
union io_phy3ifctl_1{
    unsigned long       word;
    struct {
        unsigned long   PHY3_CLKSETSEL      :8;
        unsigned long   PHY3_D0SETSEL       :8;
        unsigned long   PHY3_D1SETSEL       :8;
        unsigned long   PHY3_D2SETSEL       :8;
    }bit;
};
union io_phy3ifctl_2{
    unsigned long       word;
    struct {
        unsigned long   PHY3_D3SETSEL       :8;
        unsigned long   PHY3_CPHYSET_CP0    :8;
        unsigned long   PHY3_CPHYSET_CP1    :8;
        unsigned long   PHY3_CPHYSET_CP2    :8;
    }bit;
};
union io_phy3ifctl_3{
    unsigned long       word;
    struct {
        unsigned long   PHY3_FUNCSELCK      :16;
        unsigned long                       :16;
    }bit;
};
union io_phy3ifctl_4{
    unsigned long       word;
    struct {
        unsigned long   PHY3_FUNCSELD0      :32;
    }bit;
};
union io_phy3ifctl_5{
    unsigned long       word;
    struct {
        unsigned long   PHY3_FUNCSELD1      :32;
    }bit;
};
union io_phy3ifctl_6{
    unsigned long       word;
    struct {
        unsigned long   PHY3_FUNCSELD2      :32;
    }bit;
};
union io_phy3ifctl_7{
    unsigned long       word;
    struct {
        unsigned long   PHY3_FUNCSELD3      :32;
    }bit;
};
struct io_phy3ifctl{
    union io_phy3ifctl_1   PHY3IFCTL1;
    union io_phy3ifctl_2   PHY3IFCTL2;
    union io_phy3ifctl_3   PHY3IFCTL3;
    union io_phy3ifctl_4   PHY3IFCTL4;
    union io_phy3ifctl_5   PHY3IFCTL5;
    union io_phy3ifctl_6   PHY3IFCTL6;
    union io_phy3ifctl_7   PHY3IFCTL7;
};

/*  structure of PHY0DSKCTL (2800_8200h)    */
union io_phy0dskctl{
    unsigned long       word;
    struct {
        unsigned long   PHY0_RXSPDEN    :2;
        unsigned long                   :2;
        unsigned long   PHY0_DCNTNUM    :3;
        unsigned long                   :1;
        unsigned long   PHY0_GRDWIDTH   :1;
        unsigned long                   :3;
        unsigned long   PHY0_RDWAIT     :2;
        unsigned long                   :2;
        unsigned long   PHY0_VLDPRD     :2;
        unsigned long                   :14;
    }bit;
};

/*  structure of PHY1DSKCTL (2800_8204h)    */
union io_phy1dskctl{
    unsigned long       word;
    struct {
        unsigned long   PHY1_RXSPDEN    :2;
        unsigned long                   :2;
        unsigned long   PHY1_DCNTNUM    :3;
        unsigned long                   :1;
        unsigned long   PHY1_GRDWIDTH   :1;
        unsigned long                   :3;
        unsigned long   PHY1_RDWAIT     :2;
        unsigned long                   :2;
        unsigned long   PHY1_VLDPRD     :2;
        unsigned long                   :14;
    }bit;
};

/*  structure of PHY2DSKCTL (2800_8208h)    */
union io_phy2dskctl{
    unsigned long       word;
    struct {
        unsigned long   PHY2_RXSPDEN    :2;
        unsigned long                   :2;
        unsigned long   PHY2_DCNTNUM    :3;
        unsigned long                   :1;
        unsigned long   PHY2_GRDWIDTH   :1;
        unsigned long                   :3;
        unsigned long   PHY2_RDWAIT     :2;
        unsigned long                   :2;
        unsigned long   PHY2_VLDPRD     :2;
        unsigned long                   :14;
    }bit;
};

/*  structure of PHY3DSKCTL (2800_820Ch)    */
union io_phy3dskctl{
    unsigned long       word;
    struct {
        unsigned long   PHY3_RXSPDEN    :2;
        unsigned long                   :2;
        unsigned long   PHY3_DCNTNUM    :3;
        unsigned long                   :1;
        unsigned long   PHY3_GRDWIDTH   :1;
        unsigned long                   :3;
        unsigned long   PHY3_RDWAIT     :2;
        unsigned long                   :2;
        unsigned long   PHY3_VLDPRD     :2;
        unsigned long                   :14;
    }bit;
};

/*  structure of PLL0CTL    (2800_8210h)    */
union io_pll0ctl{
    unsigned long       word;
    struct {
        unsigned long   PLL0_IBCNT          :3;
        unsigned long                       :1;
        unsigned long   PLL0_ILFSEL         :1;
        unsigned long                       :3;
        unsigned long   PLL0_IOCLKEN        :1;
        unsigned long                       :3;
        unsigned long   PLL0_ITXPCLK2AEN    :1;
        unsigned long                       :19;
    }bit;
};

/*  structure of BIASCTL    (2800_8214h)    */
union io_biasctl{
    unsigned long       word;
    struct {
        unsigned long   PHY0_XBGRPD :1;
        unsigned long               :3;
        unsigned long   PHY1_XBGRPD :1;
        unsigned long               :3;
        unsigned long   PHY2_XBGRPD :1;
        unsigned long               :3;
        unsigned long   PHY3_XBGRPD :1;
        unsigned long               :19;
    }bit;
};

/*  structure of PHY0DSKMONI    (2800_81F8h)    */
union io_phy0dskmoni_1{
    unsigned long      word;
    struct {
        unsigned long  PHY0_DTYPEMON0  :2;
        unsigned long                  :2;
        unsigned long  PHY0_DTYPEMON1  :2;
        unsigned long                  :2;
        unsigned long  PHY0_DTYPEMON2  :2;
        unsigned long                  :2;
        unsigned long  PHY0_DTYPEMON3  :2;
        unsigned long                  :2;
        unsigned long  PHY0_VLDWINDOW0 :7;
        unsigned long                  :1;
        unsigned long  PHY0_VLDWINDOW1 :7;
        unsigned long                  :1;
    }bit;
};
union io_phy0dskmoni_2{
    unsigned long      word;
    struct {
        unsigned long  PHY0_VLDWINDOW2 :7;
        unsigned long                  :1;
        unsigned long  PHY0_VLDWINDOW3 :7;
        unsigned long                  :17;
    }bit;
};
struct io_phy0dskmoni{
    union io_phy0dskmoni_1   PHY0DSKMONI1;
    union io_phy0dskmoni_2   PHY0DSKMONI2;
};

/*  structure of PHY1DSKMONI    (2800_8200h)    */
union io_phy1dskmoni_1{
    unsigned long      word;
    struct {
        unsigned long  PHY1_DTYPEMON0  :2;
        unsigned long                  :2;
        unsigned long  PHY1_DTYPEMON1  :2;
        unsigned long                  :2;
        unsigned long  PHY1_DTYPEMON2  :2;
        unsigned long                  :2;
        unsigned long  PHY1_DTYPEMON3  :2;
        unsigned long                  :2;
        unsigned long  PHY1_VLDWINDOW0 :7;
        unsigned long                  :1;
        unsigned long  PHY1_VLDWINDOW1 :7;
        unsigned long                  :1;
    }bit;
};
union io_phy1dskmoni_2{
    unsigned long      word;
    struct {
        unsigned long  PHY1_VLDWINDOW2 :7;
        unsigned long                  :1;
        unsigned long  PHY1_VLDWINDOW3 :7;
        unsigned long                  :17;
    }bit;
};
struct io_phy1dskmoni{
    union io_phy1dskmoni_1   PHY1DSKMONI1;
    union io_phy1dskmoni_2   PHY1DSKMONI2;
};

/*  structure of PHY2DSKMONI    (2800_8208h)    */
union io_phy2dskmoni_1{
    unsigned long      word;
    struct {
        unsigned long  PHY2_DTYPEMON0  :2;
        unsigned long                  :2;
        unsigned long  PHY2_DTYPEMON1  :2;
        unsigned long                  :2;
        unsigned long  PHY2_DTYPEMON2  :2;
        unsigned long                  :2;
        unsigned long  PHY2_DTYPEMON3  :2;
        unsigned long                  :2;
        unsigned long  PHY2_VLDWINDOW0 :7;
        unsigned long                  :1;
        unsigned long  PHY2_VLDWINDOW1 :7;
        unsigned long                  :1;
    }bit;
};
union io_phy2dskmoni_2{
    unsigned long      word;
    struct {
        unsigned long  PHY2_VLDWINDOW2 :7;
        unsigned long                  :1;
        unsigned long  PHY2_VLDWINDOW3 :7;
        unsigned long                  :17;
    }bit;
};
struct io_phy2dskmoni{
    union io_phy2dskmoni_1   PHY2DSKMONI1;
    union io_phy2dskmoni_2   PHY2DSKMONI2;
};

/*  structure of PHY3DSKMONI    (2800_8210h)    */
union io_phy3dskmoni_1{
    unsigned long      word;
    struct {
        unsigned long  PHY3_DTYPEMON0  :2;
        unsigned long                  :2;
        unsigned long  PHY3_DTYPEMON1  :2;
        unsigned long                  :2;
        unsigned long  PHY3_DTYPEMON2  :2;
        unsigned long                  :2;
        unsigned long  PHY3_DTYPEMON3  :2;
        unsigned long                  :2;
        unsigned long  PHY3_VLDWINDOW0 :7;
        unsigned long                  :1;
        unsigned long  PHY3_VLDWINDOW1 :7;
        unsigned long                  :1;
    }bit;
};
union io_phy3dskmoni_2{
    unsigned long      word;
    struct {
        unsigned long  PHY3_VLDWINDOW2 :7;
        unsigned long                  :1;
        unsigned long  PHY3_VLDWINDOW3 :7;
        unsigned long                  :17;
    }bit;
};
struct io_phy3dskmoni{
    union io_phy3dskmoni_1   PHY3DSKMONI1;
    union io_phy3dskmoni_2   PHY3DSKMONI2;
};

/*  structure of PHY0DLMONI (2800_8240h)    */
union io_phy0dlmoni_1{
    unsigned long       word;
    struct {
        unsigned long   PHY0_CODE_CP0   :18;
        unsigned long                   :14;
    }bit;
};
union io_phy0dlmoni_2{
    unsigned long       word;
    struct {
        unsigned long   PHY0_CODE_CP1   :18;
        unsigned long                   :14;
    }bit;
};
union io_phy0dlmoni_3{
    unsigned long       word;
    struct {
        unsigned long   PHY0_CODE_CP2   :18;
        unsigned long                   :14;
    }bit;
};
struct io_phy0dlmoni{
    union io_phy0dlmoni_1  PHY0DLMONI1;
    union io_phy0dlmoni_2  PHY0DLMONI2;
    union io_phy0dlmoni_3  PHY0DLMONI3;
};

/*  structure of PHY1DLMONI (2800_8250h)    */
union io_phy1dlmoni_1{
    unsigned long       word;
    struct {
        unsigned long   PHY1_CODE_CP0   :18;
        unsigned long                   :14;
    }bit;
};
union io_phy1dlmoni_2{
    unsigned long       word;
    struct {
        unsigned long   PHY1_CODE_CP1   :18;
        unsigned long                   :14;
    }bit;
};
union io_phy1dlmoni_3{
    unsigned long       word;
    struct {
        unsigned long   PHY1_CODE_CP2   :18;
        unsigned long                   :14;
    }bit;
};
struct io_phy1dlmoni{
    union io_phy1dlmoni_1  PHY1DLMONI1;
    union io_phy1dlmoni_2  PHY1DLMONI2;
    union io_phy1dlmoni_3  PHY1DLMONI3;
};

/*  structure of PHY2DLMONI (2800_8260h)    */
union io_phy2dlmoni_1{
    unsigned long       word;
    struct {
        unsigned long  PHY2_CODE_CP0    :18;
        unsigned long                   :14;
    }bit;
};
union io_phy2dlmoni_2{
    unsigned long       word;
    struct {
        unsigned long  PHY2_CODE_CP1    :18;
        unsigned long                   :14;
    }bit;
};
union io_phy2dlmoni_3{
    unsigned long       word;
    struct {
        unsigned long   PHY2_CODE_CP2   :18;
        unsigned long                   :14;
    }bit;
};
struct io_phy2dlmoni{
    union io_phy2dlmoni_1  PHY2DLMONI1;
    union io_phy2dlmoni_2  PHY2DLMONI2;
    union io_phy2dlmoni_3  PHY2DLMONI3;
};

/*  structure of PHY3DLMONI (2800_8270h)    */
union io_phy3dlmoni_1{
    unsigned long       word;
    struct {
        unsigned long  PHY3_CODE_CP0    :18;
        unsigned long                   :14;
    }bit;
};
union io_phy3dlmoni_2{
    unsigned long       word;
    struct {
        unsigned long  PHY3_CODE_CP1    :18;
        unsigned long                   :14;
    }bit;
};
union io_phy3dlmoni_3{
    unsigned long       word;
    struct {
        unsigned long   PHY3_CODE_CP2   :18;
        unsigned long                   :14;
    }bit;
};
struct io_phy3dlmoni{
    union io_phy3dlmoni_1  PHY3DLMONI1;
    union io_phy3dlmoni_2  PHY3DLMONI2;
    union io_phy3dlmoni_3  PHY3DLMONI3;
};

/*  structure of PHYCMONI   (2800_8280h)    */
union io_phycmoni_1{
    unsigned long long      dword;
    struct {
        unsigned long long  PHY0_CDRMON :64;
    }bit;
};
union io_phycmoni_2{
    unsigned long long      dword;
    struct {
        unsigned long long  PHY1_CDRMON :64;
    }bit;
};
union io_phycmoni_3{
    unsigned long long      dword;
    struct {
        unsigned long long  PHY2_CDRMON :64;
    }bit;
};
union io_phycmoni_4{
    unsigned long long      dword;
    struct {
        unsigned long long  PHY3_CDRMON :64;
    }bit;
};
struct io_phycmoni{
    union io_phycmoni_1    PHYCMONI1;
    union io_phycmoni_2    PHYCMONI2;
    union io_phycmoni_3    PHYCMONI3;
    union io_phycmoni_4    PHYCMONI4;
};

// --- REMOVE_BUZ BEGIN
#ifdef CO_USE_BUZ
union io_pseudo_vd{
    unsigned long       word;
    struct {
        unsigned long   P_VD:1;
        unsigned long       :31;
    }bit;
};
#endif // CO_USE_BUZ
// --- REMOVE_BUZ END

/* Define i/o mapping */
struct io_sentop{
    /* JDSIMG */
    union  io_sensr        SR;                  /* 2800_(8000 - 8003h) */

    unsigned char dmy_8004_8007[0x8008-0x8004]; /* 2800_(8004 - 8007h) */

    struct io_senclkstp    CLKSTP;              /* 2800_(8008 - 800Fh) */
    struct io_sentopctl1   SENTOPCTL1;          /* 2800_(8010 - 801Fh) */
    struct io_sentopctl2   SENTOPCTL2;          /* 2800_(8020 - 8027h) */
    struct io_senbsctl     BSCTL;               /* 2800_(8028 - 802Fh) */
    union  io_senintmon1   INTMON1;             /* 2800_(8030 - 8033h) */
    union  io_senintmon2   INTMON2;             /* 2800_(8034 - 8037h) */
    union  io_senintctl    INTCTL;              /* 2800_(8038 - 803Bh) */

    unsigned char dmy_803C_803F[0x8040-0x803C]; /* 2800_(803C - 803Fh) */

    struct io_vhdintenb    VHDINTENB;           /* 2800_(8040 - 8047h) */
    struct io_vhdintflg    VHDINTFLG;           /* 2800_(8048 - 804Fh) */
    struct io_vhset0       VHSET0;              /* 2800_(8050 - 8057h) */
    struct io_vhset1       VHSET1;              /* 2800_(8058 - 805Fh) */
    struct io_vhset2       VHSET2;              /* 2800_(8060 - 8067h) */
    struct io_vhset3       VHSET3;              /* 2800_(8068 - 806Fh) */
    struct io_vmivadr      VMIVADR;             /* 2800_(8070 - 807Fh) */
    struct io_vmihadr      VMIHADR;             /* 2800_(8080 - 808Fh) */
    struct io_hmihadr      HMIHADR;             /* 2800_(8090 - 809Fh) */
    union  io_sgvhdintenb  SGVHDINTENB;         /* 2800_(80A0 - 80A3h) */
    union  io_sgvhdintflg  SGVHDINTFLG;         /* 2800_(80A4 - 80A7h) */
    struct io_vhdsgctl     VHDSGCTL;            /* 2800_(80A8 - 80AFh) */
    union  io_obtcdcset    OBTCDCSET;           /* 2800_(80B0 - 80B3h) */
    union  io_obtcdcctl1   OBTCDCCTL1;          /* 2800_(80B4 - 80B7h) */
    struct io_obtcdcctl2   OBTCDCCTL2;          /* 2800_(80B8 - 80BFh) */
    union  io_lvdsctl      LVDSCTL;             /* 2800_(80C0 - 80C3h) */
    union  io_slvsecctl    SLVSECCTL;           /* 2800_(80C4 - 80C7h) */
    union  io_sgoutctl     SGOUTCTL;            /* 2800_(80C8 - 80CBh) */
    union  io_gintenb      GINTENB;             /* 2800_(80CC - 80CFh) */
    union  io_gintflg      GINTFLG;             /* 2800_(80D0 - 80D3h) */
    union  io_gset0        GSET0;               /* 2800_(80D4 - 80D7h) */
    union  io_gset1        GSET1;               /* 2800_(80D8 - 80DBh) */
    union  io_gset2        GSET2;               /* 2800_(80DC - 80DFh) */
    union  io_gset3        GSET3;               /* 2800_(80E0 - 80E3h) */

    unsigned char dmy_80E4_80E7[0x80E8-0x80E4]; /* 2800_(80E4 - 80E7h) */

    struct io_gvadr        GVADR;               /* 2800_(80E8 - 80EFh) */
    struct io_ghadr        GHADR;               /* 2800_(80F0 - 80F7h) */

    unsigned char dmy_80F8_80FF[0x8100-0x80F8]; /* 2800_(80F8 - 80FFh) */

    struct io_phycifctl    PHYCIFCTL;           /* 2800_(8100 - 810Bh) */

    unsigned char dmy_810C_810F[0x8110-0x810C]; /* 2800_(810C - 810Fh) */

    union  io_phy0clkctl   PHY0CLKCTL;          /* 2800_(8110 - 8113h) */
    union  io_phy1clkctl   PHY1CLKCTL;          /* 2800_(8114 - 8117h) */
    union  io_phy2clkctl   PHY2CLKCTL;          /* 2800_(8118 - 811Bh) */
    union  io_phy3clkctl   PHY3CLKCTL;          /* 2800_(811C - 811Fh) */
    union  io_phy0mdctl    PHY0MDCTL;           /* 2800_(8120 - 8123h) */
    union  io_phy1mdctl    PHY1MDCTL;           /* 2800_(8124 - 8127h) */
    union  io_phy2mdctl    PHY2MDCTL;           /* 2800_(8128 - 812Bh) */
    union  io_phy3mdctl    PHY3MDCTL;           /* 2800_(812C - 812Fh) */
    struct io_phy0mdcpctl  PHY0MDCPCTL;         /* 2800_(8130 - 813Bh) */

    unsigned char dmy_813C_813F[0x8140-0x813C]; /* 2800_(813C - 813Fh) */

    struct io_phy1mdcpctl  PHY1MDCPCTL;         /* 2800_(8140 - 814Bh) */

    unsigned char dmy_814C_814F[0x8150-0x814C]; /* 2800_(814C - 814Fh) */

    struct io_phy2mdcpctl  PHY2MDCPCTL;         /* 2800_(8150 - 815Bh) */

    unsigned char dmy_815C_815F[0x8160-0x815C]; /* 2800_(815C - 815Fh) */

    struct io_phy3mdcpctl  PHY3MDCPCTL;         /* 2800_(8160 - 816Bh) */

    unsigned char dmy_816C_816F[0x8170-0x816C]; /* 2800_(816C - 816Fh) */

    union  io_phy0dlctl    PHY0DLCTL;           /* 2800_(8170 - 8173h) */
    union  io_phy1dlctl    PHY1DLCTL;           /* 2800_(8174 - 8177h) */
    union  io_phy2dlctl    PHY2DLCTL;           /* 2800_(8178 - 817Bh) */
    union  io_phy3dlctl    PHY3DLCTL;           /* 2800_(817C - 817Fh) */
    struct io_phy0ifctl    PHY0IFCTL;           /* 2800_(8180 - 819Bh) */

    unsigned char dmy_819C_819F[0x81A0-0x819C]; /* 2800_(819C - 819Fh) */

    struct io_phy1ifctl    PHY1IFCTL;           /* 2800_(81A0 - 81BBh) */

    unsigned char dmy_81BC_81BF[0x81C0-0x81BC]; /* 2800_(81BC - 81BFh) */

    struct io_phy2ifctl    PHY2IFCTL;           /* 2800_(81C0 - 81DBh) */

    unsigned char dmy_81DC_81DF[0x81E0-0x81DC]; /* 2800_(81DC - 81DFh) */

    struct io_phy3ifctl    PHY3IFCTL;           /* 2800_(81E0 - 81FBh) */

    unsigned char dmy_81FC_81FF[0x8200-0x81FC]; /* 2800_(81FC - 81FFh) */

    union  io_phy0dskctl   PHY0DSKCTL;          /* 2800_(8200 - 8203h) */
    union  io_phy1dskctl   PHY1DSKCTL;          /* 2800_(8204 - 8207h) */
    union  io_phy2dskctl   PHY2DSKCTL;          /* 2800_(8208 - 820Bh) */
    union  io_phy3dskctl   PHY3DSKCTL;          /* 2800_(820C - 820Fh) */
    union  io_pll0ctl      PLL0CTL;             /* 2800_(8210 - 8213h) */
    union  io_biasctl      BIASCTL;             /* 2800_(8214 - 8217h) */
    struct io_phy0dskmoni  PHY0DSKMONI;         /* 2800_(8218 - 821Fh) */
    struct io_phy1dskmoni  PHY1DSKMONI;         /* 2800_(8220 - 8227h) */
    struct io_phy2dskmoni  PHY2DSKMONI;         /* 2800_(8228 - 822Fh) */
    struct io_phy3dskmoni  PHY3DSKMONI;         /* 2800_(8230 - 8237h) */

    unsigned char dmy_8238_823F[0x8240-0x8238]; /* 2800_(8238 - 823Fh) */

    struct io_phy0dlmoni   PHY0DLMONI;          /* 2800_(8240 - 824Bh) */

    unsigned char dmy_824C_824F[0x8250-0x824C]; /* 2800_(824C - 824Fh) */

    struct io_phy1dlmoni   PHY1DLMONI;          /* 2800_(8250 - 825Bh) */

    unsigned char dmy_825C_825F[0x8260-0x825C]; /* 2800_(825C - 825Fh) */

    struct io_phy2dlmoni   PHY2DLMONI;          /* 2800_(8260 - 826Bh) */

    unsigned char dmy_826C_826F[0x8270-0x826C]; /* 2800_(826C - 826Fh) */

    struct io_phy3dlmoni   PHY3DLMONI;          /* 2800_(8270 - 827Bh) */

    unsigned char dmy_827C_827F[0x8280-0x827C]; /* 2800_(827C - 827Fh) */

    struct io_phycmoni     PHYCMONI;            /* 2800_(8280 - 829Fh) */

// --- REMOVE_BUZ BEGIN
#ifdef CO_USE_BUZ
	union io_pseudo_vd     PSEUDE_VD;			/* 2800_(82A0 - 82A3h) */
    unsigned char dmy_82A4_83FF[0x8400-0x82A4]; /* 2800_(82A4 - 83FFh) */
#endif // CO_USE_BUZ
// --- REMOVE_BUZ END
// --- REMOVE_BUZ BEGIN
#ifndef CO_USE_BUZ
// --- REMOVE_BUZ END
    unsigned char dmy_82A0_83FF[0x8400-0x82A0]; /* 2800_(82A0 - 83FFh) */
// --- REMOVE_BUZ BEGIN
#endif	// CO_USE_BUZ
// --- REMOVE_BUZ END

};

#endif	// _IO_PRO_SEN_TOP_H_
