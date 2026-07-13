/**
 * @file		_jdsiip4b.h
 * @brief		Definition JDSIIP4B Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "__jdsiip4b_pixfmttbl.h"
#include "__jdsiip4b_uinftbl.h"
#include "__jdsiip4b_ram.h"

/*  structure of IZACTL0    (2888_0000h)    */
union io_iip_izactl0{
    unsigned long       word;
    struct {
        unsigned long   SR  :1;
        unsigned long       :31;
    }bit;
};

/*  structure of IZACTL1    (2888_0004h)    */
union io_iip_izactl1{
    unsigned long       word;
    struct {
        unsigned long   TRG             :2;
        unsigned long   SLALE           :1;
        unsigned long                   :1;
        unsigned long   FRM             :1;
        unsigned long                   :3;
        unsigned long   USEBANK         :2;
        unsigned long   EXEBKNO         :2;
        unsigned long   STARTBK         :3;
        unsigned long   LKEN            :1;
        unsigned long   LKSL            :4;
        unsigned long   OVR128          :1;
        unsigned long   DSINI           :1;
        unsigned long   DBEN            :1;
        unsigned long                   :1;
        unsigned long   SELRDREG_UNIT   :6;
        unsigned long                   :2;
    }bit;
};

/*  structure of IZACTL2    (2888_0008h)    */
union io_iip_izactl2{
    unsigned long       word;
    struct {
        unsigned long   ABORT   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of INTEN0 (2888_000Ch)    */
union io_iip_inten0{
    unsigned long       word;
    struct {
        unsigned long   IZAEN   :1;
        unsigned long           :1;
        unsigned long   AXEEN   :1;
        unsigned long           :29;
    }bit;
};

/*  structure of INTEN1 (2888_0010h)    */
union io_iip_inten1{
    unsigned long       word;
    struct {
        unsigned long   SL0FEN  :1;
        unsigned long   SL1FEN  :1;
        unsigned long   SL2FEN  :1;
        unsigned long   SL3FEN  :1;
        unsigned long   SL4FEN  :1;
        unsigned long   SL5FEN  :1;
        unsigned long   SL6FEN  :1;
        unsigned long   SL7FEN  :1;
        unsigned long   SL8FEN  :1;
        unsigned long           :3;
        unsigned long           :4;
        unsigned long   SL0MEN  :1;
        unsigned long   SL1MEN  :1;
        unsigned long   SL2MEN  :1;
        unsigned long   SL3MEN  :1;
        unsigned long   SL4MEN  :1;
        unsigned long   SL5MEN  :1;
        unsigned long   SL6MEN  :1;
        unsigned long   SL7MEN  :1;
        unsigned long   SL8MEN  :1;
        unsigned long           :7;
    }bit;
};

/*  structure of INTEN2 (2888_0014h)    */
union io_iip_inten2{
    unsigned long       word;
    struct {
        unsigned long   REAF0E  :1;
        unsigned long   REAF1E  :1;
        unsigned long   REAF2E  :1;
        unsigned long   REAF3E  :1;
        unsigned long   REFR0E  :1;
        unsigned long   REFR1E  :1;
        unsigned long   REFLTE  :1;
        unsigned long           :25;
    }bit;
};

/*  structure of INTIZ0 (2888_0018h)    */
union io_iip_intiz0{
    unsigned long       word;
    struct {
        unsigned long   __IZAFLG  :1;
        unsigned long             :1;
        unsigned long   __AXEFLG  :1;
        unsigned long             :29;
    }bit;
};

/*  structure of INTIZ1 (2888_001Ch)    */
union io_iip_intiz1{
    unsigned long       word;
    struct {
        unsigned long   __SL0FF   :1;
        unsigned long   __SL1FF   :1;
        unsigned long   __SL2FF   :1;
        unsigned long   __SL3FF   :1;
        unsigned long   __SL4FF   :1;
        unsigned long   __SL5FF   :1;
        unsigned long   __SL6FF   :1;
        unsigned long   __SL7FF   :1;
        unsigned long   __SL8FF   :1;
        unsigned long             :3;
        unsigned long             :1;
        unsigned long             :3;
        unsigned long   __SL0MF   :1;
        unsigned long   __SL1MF   :1;
        unsigned long   __SL2MF   :1;
        unsigned long   __SL3MF   :1;
        unsigned long   __SL4MF   :1;
        unsigned long   __SL5MF   :1;
        unsigned long   __SL6MF   :1;
        unsigned long   __SL7MF   :1;
        unsigned long   __SL8MF   :1;
        unsigned long             :7;
    }bit;
};

/*  structure of INTIZ2 (2888_0020h)    */
union io_iip_intiz2{
    unsigned long       word;
    struct {
        unsigned long   __REAF0F  :1;
        unsigned long   __REAF1F  :1;
        unsigned long   __REAF2F  :1;
        unsigned long   __REAF3F  :1;
        unsigned long   __REFR0F  :1;
        unsigned long   __REFR1F  :1;
        unsigned long   __REFLTF  :1;
        unsigned long             :25;
    }bit;
};

/*  structure of PFTREWTFLG (2888_0028h)    */
union io_iip_pftrewtflg{
    unsigned long       word;
    struct {
        unsigned long   PFTFLG  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of PFTREWT    (2888_002Ch)    */
union io_iip_pftrewt{
    unsigned long       word;
    struct {
        unsigned long   PFTREWT :4;
        unsigned long           :28;
    }bit;
};

/*  structure of PDCCTL (2888_0030h)    */
union io_iip_pdcctl{
    unsigned long       word;
    struct {
        unsigned long   FLHALL          :1;
        unsigned long                   :3;
        unsigned long   EXTBIT          :1;
        unsigned long                   :3;
        unsigned long   PDCPKG          :1;
        unsigned long                   :3;
        unsigned long   PDCARCACHE      :4;
        unsigned long   PDCARPROT       :3;
        unsigned long                   :13;
    }bit;
};

/*  structure of PDCRAC (2888_0040h)    */
union io_iip_pdcrac{
    unsigned long       word;
    struct {
        unsigned long   PDCRAC      :32;
    }bit;
};

/*  structure of PDCRMC (2888_0050h)    */
union io_iip_pdcrmc{
    unsigned long       word;
    struct {
        unsigned long   PDCRMC      :32;
    }bit;
};

/*  structure of GENLINE    (2888_00C0h)    */
union io_iip_genline_1{
    unsigned long      word[2];
    struct {
        unsigned long  B2Y_OUTLINENO   :14;
        unsigned long                  :2;
        unsigned long  AFN0_OUTLINENO  :14;
        unsigned long                  :2;
        unsigned long  AFN1_OUTLINENO  :14;
        unsigned long                  :2;
        unsigned long  AFN2_OUTLINENO  :14;
        unsigned long                  :2;
    }bit;
};
union io_iip_genline_2{
    unsigned long      word[2];
    struct {
        unsigned long  AFN3_OUTLINENO  :14;
        unsigned long                  :2;
        unsigned long  FR0_OUTLINENO   :14;
        unsigned long                  :2;
        unsigned long  FR1_OUTLINENO   :14;
        unsigned long                  :2;
        unsigned long  LD0_OUTLINENO   :14;
        unsigned long                  :2;
    }bit;
};
union io_iip_genline_3{
    unsigned long      word[2];
    struct {
        unsigned long  LD1_OUTLINENO   :14;
        unsigned long                  :2;
        unsigned long  LD2_OUTLINENO   :14;
        unsigned long                  :2;
        unsigned long  LD3_OUTLINENO   :14;
        unsigned long                  :2;
        unsigned long  LD4_OUTLINENO   :14;
        unsigned long                  :2;
    }bit;
};
union io_iip_genline_4{
    unsigned long      word[2];
    struct {
        unsigned long  LD5_OUTLINENO   :14;
        unsigned long                  :2;
        unsigned long  LD6_OUTLINENO   :14;
        unsigned long                  :2;
        unsigned long  FLT_OUTLINENO   :14;
        unsigned long                  :18;
    }bit;
};
struct io_iip_genline{
    union io_iip_genline_1    GENLINE1;
    union io_iip_genline_2    GENLINE2;
    union io_iip_genline_3    GENLINE3;
    union io_iip_genline_4    GENLINE4;
};

/*  structure of LKSL_CTL   (2888_00E0h)    */
union io_iip_lksl_ctl{
    unsigned long       word;
    struct {
        unsigned long   VSIZE_COP       :14;
        unsigned long                   :2;
        unsigned long   LINE_VAL_COP    :8;
        unsigned long                   :8;
    }bit;
};

/*  structure of LINE_VAL_INT_SL0   (2888_00E4h)    */
/*  structure of LINE_VAL_INT_SL1   (2888_00E8h)    */
/*  structure of LINE_VAL_INT_SL2   (2888_00ECh)    */
/*  structure of LINE_VAL_INT_SL3   (2888_00F0h)    */
/*  structure of LINE_VAL_INT_SL4   (2888_00F4h)    */
/*  structure of LINE_VAL_INT_SL5   (2888_00F8h)    */
/*  structure of LINE_VAL_INT_SL6   (2888_00FCh)    */
/*  structure of LINE_VAL_INT_SL7   (2888_0100h)    */
/*  structure of LINE_VAL_INT_SL8   (2888_0104h)    */
union io_iip_line_val_int_sl{
    unsigned long       word;
    struct {
        unsigned long   LINE_VAL_INT_SL     :14;
        unsigned long                       :18;
    }bit;
};

/*  structure of DPAXICTL   (2888_010Ch)    */
union io_iip_dpaxictl{
    unsigned long       word;
    struct {
        unsigned long   DPRS        :1;
        unsigned long               :3;
        unsigned long               :1;
        unsigned long               :3;
        unsigned long   DPARCACHE   :4;
        unsigned long   DPARPROT    :3;
        unsigned long               :17;
    }bit;
};

/*  structure of AFNAXICTL  (2888_0110h)    */
union io_iip_afnaxictl{
    unsigned long      word[2];
    struct {
        unsigned long  AFN0RS      :1;
        unsigned long              :3;
        unsigned long  AFN0PK      :1;
        unsigned long              :3;
        unsigned long  AFN0ARCACHE :4;
        unsigned long  AFN0ARPROT  :3;
        unsigned long              :1;
        unsigned long  AFN1RS      :1;
        unsigned long              :3;
        unsigned long  AFN1PK      :1;
        unsigned long              :3;
        unsigned long  AFN1ARCACHE :4;
        unsigned long  AFN1ARPROT  :3;
        unsigned long              :1;
        unsigned long  AFN2RS      :1;
        unsigned long              :3;
        unsigned long  AFN2PK      :1;
        unsigned long              :3;
        unsigned long  AFN2ARCACHE :4;
        unsigned long  AFN2ARPROT  :3;
        unsigned long              :1;
        unsigned long  AFN3RS      :1;
        unsigned long              :3;
        unsigned long  AFN3PK      :1;
        unsigned long              :3;
        unsigned long  AFN3ARCACHE :4;
        unsigned long  AFN3ARPROT  :3;
        unsigned long              :1;
    }bit;
};

/*  structure of FRAXICTL   (2888_0118h)    */
union io_iip_fraxictl{
    unsigned long       word;
    struct {
        unsigned long   FR0RS       :1;
        unsigned long               :3;
        unsigned long   FR0PKG      :1;
        unsigned long               :3;
        unsigned long   FR0ARCACHE  :4;
        unsigned long   FR0ARPROT   :3;
        unsigned long               :1;
        unsigned long   FR1RS       :1;
        unsigned long               :3;
        unsigned long   FR1PKG      :1;
        unsigned long               :3;
        unsigned long   FR1ARCACHE  :4;
        unsigned long   FR1ARPROT   :3;
        unsigned long               :1;
    }bit;
};

/*  structure of GPCAXICTL  (2888_011Ch)    */
union io_iip_gpcaxictl{
    unsigned long       word;
    struct {
        unsigned long   GPCRS       :1;
        unsigned long               :3;
        unsigned long               :1;
        unsigned long               :3;
        unsigned long   GPCARCACHE  :4;
        unsigned long   GPCARPROT   :3;
        unsigned long               :17;
    }bit;
};

/*  structure of SLAXICTL   (2888_0120h)    */
union io_iip_slaxictl_1{
    unsigned long       word;
    struct {
        unsigned long               :4;
        unsigned long   SL0PKG      :2;
        unsigned long               :2;
        unsigned long   SL0AWCACHE  :4;
        unsigned long   SL0AWPROT   :3;
        unsigned long               :1;
        unsigned long               :4;
        unsigned long   SL1PKG      :2;
        unsigned long               :2;
        unsigned long   SL1AWCACHE  :4;
        unsigned long   SL1AWPROT   :3;
        unsigned long               :1;
    }bit;
};
union io_iip_slaxictl_2{
    unsigned long       word;
    struct {
        unsigned long               :4;
        unsigned long   SL2PKG      :2;
        unsigned long               :2;
        unsigned long   SL2AWCACHE  :4;
        unsigned long   SL2AWPROT   :3;
        unsigned long               :1;
        unsigned long               :4;
        unsigned long   SL3PKG      :2;
        unsigned long               :2;
        unsigned long   SL3AWCACHE  :4;
        unsigned long   SL3AWPROT   :3;
        unsigned long               :1;
    }bit;
};
union io_iip_slaxictl_3{
    unsigned long       word;
    struct {
        unsigned long               :4;
        unsigned long   SL4PKG      :2;
        unsigned long               :2;
        unsigned long   SL4AWCACHE  :4;
        unsigned long   SL4AWPROT   :3;
        unsigned long               :1;
        unsigned long               :4;
        unsigned long   SL5PKG      :2;
        unsigned long               :2;
        unsigned long   SL5AWCACHE  :4;
        unsigned long   SL5AWPROT   :3;
        unsigned long               :1;
    }bit;
};
union io_iip_slaxictl_4{
    unsigned long       word;
    struct {
        unsigned long               :4;
        unsigned long   SL6PKG      :2;
        unsigned long               :2;
        unsigned long   SL6AWCACHE  :4;
        unsigned long   SL6AWPROT   :3;
        unsigned long               :1;
        unsigned long               :4;
        unsigned long   SL7PKG      :2;
        unsigned long               :2;
        unsigned long   SL7AWCACHE  :4;
        unsigned long   SL7AWPROT   :3;
        unsigned long               :1;
    }bit;
};
union io_iip_slaxictl_5{
    unsigned long       word;
    struct {
        unsigned long               :4;
        unsigned long   SL8PKG      :2;
        unsigned long               :2;
        unsigned long   SL8AWCACHE  :4;
        unsigned long   SL8AWPROT   :3;
        unsigned long               :17;
    }bit;
};
struct io_iip_slaxictl{
    union io_iip_slaxictl_1   SLAXICTL1;
    union io_iip_slaxictl_2   SLAXICTL2;
    union io_iip_slaxictl_3   SLAXICTL3;
    union io_iip_slaxictl_4   SLAXICTL4;
    union io_iip_slaxictl_5   SLAXICTL5;
};

/*  structure of AXRSTS (2888_0140h)    */
union io_iip_axrsts{
    unsigned long       word;
    struct {
        unsigned long   DPRRESP     :2;
        unsigned long               :2;
        unsigned long   PDC0RRESP   :2;
        unsigned long   PDC1RRESP   :2;
        unsigned long   PDC2RRESP   :2;
        unsigned long   AFN0RRESP   :2;
        unsigned long   AFN1RRESP   :2;
        unsigned long   AFN2RRESP   :2;
        unsigned long   AFN3RRESP   :2;
        unsigned long   FR0RRESP    :2;
        unsigned long   FR1RRESP    :2;
        unsigned long   GPCRRESP    :2;
        unsigned long               :8;
    }bit;
};

/*  structure of AXWSTS (2888_0144h)    */
union io_iip_axwsts{
    unsigned long       word;
    struct {
        unsigned long   SL0WRESP    :2;
        unsigned long   SL1WRESP    :2;
        unsigned long   SL2WRESP    :2;
        unsigned long   SL3WRESP    :2;
        unsigned long   SL4WRESP    :2;
        unsigned long   SL5WRESP    :2;
        unsigned long   SL6WRESP    :2;
        unsigned long   SL7WRESP    :2;
        unsigned long   SL8WRESP    :2;
        unsigned long               :14;
    }bit;
};

/*  structure of AHBSTS (2888_0148h)    */
union io_iip_ahbsts{
    unsigned long       word;
    struct {
        unsigned long   LUTAST  :1;
        unsigned long   LUTBST  :1;
        unsigned long   LUTCST  :1;
        unsigned long   LUTDST  :1;
        unsigned long   LUTEST  :1;
        unsigned long   LUTFST  :1;
        unsigned long           :2;
        unsigned long   HISSTS  :1;
        unsigned long           :23;
    }bit;
};

/*  structure of ACESEN0    (2888_014Ch)    */
union io_iip_acesen0{
    unsigned long       word;
    struct {
        unsigned long   IZARAC  :1;
        unsigned long           :3;
        unsigned long   CA1RAC  :1;
        unsigned long   CA2RAC  :1;
        unsigned long           :2;
        unsigned long   LUARAC  :1;
        unsigned long   LUBRAC  :1;
        unsigned long   LUCRAC  :1;
        unsigned long   LUDRAC  :1;
        unsigned long   LUERAC  :1;
        unsigned long   LUFRAC  :1;
        unsigned long           :2;
        unsigned long   MO0RAC  :1;
        unsigned long   MO1RAC  :1;
        unsigned long   MO2RAC  :1;
        unsigned long   MO3RAC  :1;
        unsigned long   SL0RAC  :1;
        unsigned long   SL1RAC  :1;
        unsigned long   SL2RAC  :1;
        unsigned long   SL3RAC  :1;
        unsigned long   SL4RAC  :1;
        unsigned long   SL5RAC  :1;
        unsigned long   SL6RAC  :1;
        unsigned long   SL7RAC  :1;
        unsigned long   SL8RAC  :1;
        unsigned long           :3;
    }bit;
};

/*  structure of DTOUT_MON  (2888_0178h)    */
union io_iip_dtout_mon{
    unsigned long      word[2];
    struct {
        unsigned long  DTOUT_MON_LO   :32;
        unsigned long  DTOUT_MON_HI   :32;
    }bit;
};

/*  structure of WAIT_MON   (2888_0180h)    */
union io_iip_wait_mon{
    unsigned long      word[2];
    struct {
        unsigned long  WAIT_MON_LO    :32;
        unsigned long  WAIT_MON_HI    :32;
    }bit;
};

/*  structure of PDC_DTOUT_MON  (2888_0188h)    */
union io_iip_pdc_dtout_mon_1{
    unsigned long       word;
    struct {
        unsigned long   PDC_DTOUT_MON_0 :15;
        unsigned long                   :1;
        unsigned long   PDC_DTOUT_MON_1 :15;
        unsigned long                   :1;
    }bit;
};
union io_iip_pdc_dtout_mon_2{
    unsigned long       word;
    struct {
        unsigned long   PDC_DTOUT_MON_2 :15;
        unsigned long                   :1;
        unsigned long                   :16;
    }bit;
};
struct io_iip_pdc_dtout_mon{
    union io_iip_pdc_dtout_mon_1  PDC_DTOUT_MON1;
    union io_iip_pdc_dtout_mon_2  PDC_DTOUT_MON2;
};

/*  structure of PDC_RDY_MON    (2888_0190h)    */
union io_iip_pdc_rdy_mon_1{
    unsigned long       word;
    struct {
        unsigned long   PDC_RDY_MON_0   :15;
        unsigned long                   :1;
        unsigned long   PDC_RDY_MON_1   :15;
        unsigned long                   :1;
    }bit;
};
union io_iip_pdc_rdy_mon_2{
    unsigned long       word;
    struct {
        unsigned long   PDC_RDY_MON_2   :15;
        unsigned long                   :1;
        unsigned long                   :16;
    }bit;
};
struct io_iip_pdc_rdy_mon{
    union io_iip_pdc_rdy_mon_1    PDC_RDY_MON1;
    union io_iip_pdc_rdy_mon_2    PDC_RDY_MON2;
};

/*  structure of HISTMAX    (2888_0200h)    */
union io_iip_histmax_1{
    unsigned long       word;
    struct {
        unsigned long   HISTMAX_0   :8;
        unsigned long   HISTMAX_1   :8;
        unsigned long   HISTMAX_2   :8;
        unsigned long   HISTMAX_3   :8;
    }bit;
};
struct io_iip_histmax{
    union io_iip_histmax_1    HISTMAX1;
};

/*  structure of MAXMON (2888_0220h)    */
union io_iip_maxmon{
    unsigned long      word[2];
    struct {
        unsigned long  HPOINTMAX   :14;
        unsigned long              :2;
        unsigned long  VPOINTMAX   :14;
        unsigned long              :2;
        long           MAXVAL      :15;
        unsigned long              :17;
    }bit;
};

/*  structure of MINMON (2888_0240h)    */
union io_iip_minmon{
    unsigned long      word[2];
    struct {
        unsigned long  HPOINTMIN   :14;
        unsigned long              :2;
        unsigned long  VPOINTMIN   :14;
        unsigned long              :2;
        long           MINVAL      :15;
        unsigned long              :17;
    }bit;
};

/*  structure of ADDMON (2888_0260h)    */
union io_iip_addmon{
    unsigned long      word[2];
    struct {
        unsigned long  HPOINTADD   :14;
        unsigned long              :2;
        unsigned long  VPOINTADD   :14;
        unsigned long              :2;
        long           ADDVAL      :32;
    }bit;
};

/*  structure of MONINIT    (2888_0280h)    */
union io_iip_moninit{
    unsigned long       word;
    struct {
        unsigned long   INIT    :1;
        unsigned long           :31;
    }bit;
};

/* Define i/o mapping */
struct io_iip{
    /* JDSIMG */
    union  io_iip_izactl0             IZACTL0;                                /* 2888_(0000 - 0003h) */
    union  io_iip_izactl1             IZACTL1;                                /* 2888_(0004 - 0007h) */
    union  io_iip_izactl2             IZACTL2;                                /* 2888_(0008 - 000Bh) */
    union  io_iip_inten0              INTEN0;                                 /* 2888_(000C - 000Fh) */
    union  io_iip_inten1              INTEN1;                                 /* 2888_(0010 - 0013h) */
    union  io_iip_inten2              INTEN2;                                 /* 2888_(0014 - 0017h) */
    union  io_iip_intiz0              INTIZ0;                                 /* 2888_(0018 - 001Bh) */
    union  io_iip_intiz1              INTIZ1;                                 /* 2888_(001C - 001Fh) */
    union  io_iip_intiz2              INTIZ2;                                 /* 2888_(0020 - 0023h) */

    unsigned char dmy_0024_0027[0x0028-0x0024]; /* 2888_(0024 - 0027h) */

    union  io_iip_pftrewtflg          PFTREWTFLG;                             /* 2888_(0028 - 002Bh) */
    union  io_iip_pftrewt             PFTREWT;                                /* 2888_(002C - 002Fh) */
    union  io_iip_pdcctl              PDCCTL[3];                              /* 2888_(0030 - 003Bh) */

    unsigned char dmy_003C_003F[0x0040-0x003C]; /* 2888_(003C - 003Fh) */

    union  io_iip_pdcrac              PDCRAC[3];                              /* 2888_(0040 - 004Bh) */

    unsigned char dmy_004C_004F[0x0050-0x004C]; /* 2888_(004C - 004Fh) */

    union  io_iip_pdcrmc              PDCRMC[3];                              /* 2888_(0050 - 005Bh) */

    unsigned char dmy_005C_00BF[0x00C0-0x005C]; /* 2888_(005C - 00BFh) */

    struct io_iip_genline             GENLINE;                                /* 2888_(00C0 - 00DFh) */
    union  io_iip_lksl_ctl            LKSL_CTL;                               /* 2888_(00E0 - 00E3h) */
    union  io_iip_line_val_int_sl     LINE_VAL_INT_SL0;                       /* 2888_(00E4 - 00E7h) */
    union  io_iip_line_val_int_sl     LINE_VAL_INT_SL1;                       /* 2888_(00E8 - 00EBh) */
    union  io_iip_line_val_int_sl     LINE_VAL_INT_SL2;                       /* 2888_(00EC - 00EFh) */
    union  io_iip_line_val_int_sl     LINE_VAL_INT_SL3;                       /* 2888_(00F0 - 00F3h) */
    union  io_iip_line_val_int_sl     LINE_VAL_INT_SL4;                       /* 2888_(00F4 - 00F7h) */
    union  io_iip_line_val_int_sl     LINE_VAL_INT_SL5;                       /* 2888_(00F8 - 00FBh) */
    union  io_iip_line_val_int_sl     LINE_VAL_INT_SL6;                       /* 2888_(00FC - 00FFh) */
    union  io_iip_line_val_int_sl     LINE_VAL_INT_SL7;                       /* 2888_(0100 - 0103h) */
    union  io_iip_line_val_int_sl     LINE_VAL_INT_SL8;                       /* 2888_(0104 - 0107h) */

    unsigned char dmy_0108_010B[0x010C-0x0108]; /* 2888_(0108 - 010Bh) */

    union  io_iip_dpaxictl            DPAXICTL;                               /* 2888_(010C - 010Fh) */
    union  io_iip_afnaxictl           AFNAXICTL;                              /* 2888_(0110 - 0117h) */
    union  io_iip_fraxictl            FRAXICTL;                               /* 2888_(0118 - 011Bh) */
    union  io_iip_gpcaxictl           GPCAXICTL;                              /* 2888_(011C - 011Fh) */
    struct io_iip_slaxictl            SLAXICTL;                               /* 2888_(0120 - 0133h) */

    unsigned char dmy_0134_013F[0x0140-0x0134]; /* 2888_(0134 - 013Fh) */

    union  io_iip_axrsts              AXRSTS;                                 /* 2888_(0140 - 0143h) */
    union  io_iip_axwsts              AXWSTS;                                 /* 2888_(0144 - 0147h) */
    union  io_iip_ahbsts              AHBSTS;                                 /* 2888_(0148 - 014Bh) */
    union  io_iip_acesen0             ACESEN0;                                /* 2888_(014C - 014Fh) */

    unsigned char dmy_0150_0177[0x0178-0x0150]; /* 2888_(0150 - 0177h) */

    union  io_iip_dtout_mon           DTOUT_MON;                              /* 2888_(0178 - 017Fh) */
    union  io_iip_wait_mon            WAIT_MON;                               /* 2888_(0180 - 0187h) */
    struct io_iip_pdc_dtout_mon       PDC_DTOUT_MON;                          /* 2888_(0188 - 018Fh) */
    struct io_iip_pdc_rdy_mon         PDC_RDY_MON;                            /* 2888_(0190 - 0197h) */

    unsigned char dmy_0198_01FF[0x0200-0x0198]; /* 2888_(0198 - 01FFh) */

    struct io_iip_histmax             HISTMAX;                                /* 2888_(0200 - 0203h) */

    unsigned char dmy_0204_021F[0x0220-0x0204]; /* 2888_(0204 - 021Fh) */

    union  io_iip_maxmon              MAXMON[4];                              /* 2888_(0220 - 023Fh) */
    union  io_iip_minmon              MINMON[4];                              /* 2888_(0240 - 025Fh) */
    union  io_iip_addmon              ADDMON[4];                              /* 2888_(0260 - 027Fh) */
    union  io_iip_moninit             MONINIT;                                /* 2888_(0280 - 0283h) */

    unsigned char dmy_0284_02FF[0x0300-0x0284]; /* 2888_(0284 - 02FFh) */

    struct io_iip_pixfmttbl           PIXFMTTBL[12];                          /* 2888_(0300 - 05FFh) */

    unsigned char dmy_0600_07FF[0x0800-0x0600]; /* 2888_(0600 - 07FFh) */

    struct io_iip_uinftbl             UNITINFTBL_LD0;                         /* 2888_(0800 - 081Fh) */
    struct io_iip_uinftbl             UNITINFTBL_LD1;                         /* 2888_(0820 - 083Fh) */
    struct io_iip_uinftbl             UNITINFTBL_LD2;                         /* 2888_(0840 - 085Fh) */
    struct io_iip_uinftbl             UNITINFTBL_LD3;                         /* 2888_(0860 - 087Fh) */
    struct io_iip_uinftbl             UNITINFTBL_FLT;                         /* 2888_(0880 - 089Fh) */
    struct io_iip_uinftbl             UNITINFTBL_AFN0;                        /* 2888_(08A0 - 08BFh) */
    struct io_iip_uinftbl             UNITINFTBL_AFN1;                        /* 2888_(08C0 - 08DFh) */
    struct io_iip_uinftbl             UNITINFTBL_AFN2;                        /* 2888_(08E0 - 08FFh) */
    struct io_iip_uinftbl             UNITINFTBL_FRC0;                        /* 2888_(0900 - 091Fh) */
    struct io_iip_uinftbl             UNITINFTBL_MON;                         /* 2888_(0920 - 093Fh) */
    struct io_iip_uinftbl             UNITINFTBL_SL0;                         /* 2888_(0940 - 095Fh) */
    struct io_iip_uinftbl             UNITINFTBL_SL1;                         /* 2888_(0960 - 097Fh) */
    struct io_iip_uinftbl             UNITINFTBL_SL2;                         /* 2888_(0980 - 099Fh) */
    struct io_iip_uinftbl             UNITINFTBL_SL3;                         /* 2888_(09A0 - 09BFh) */
    struct io_iip_uinftbl             UNITINFTBL_SL4;                         /* 2888_(09C0 - 09DFh) */
    struct io_iip_uinftbl             UNITINFTBL_SL5;                         /* 2888_(09E0 - 09FFh) */
    struct io_iip_uinftbl             UNITINFTBL_CSC0;                        /* 2888_(0A00 - 0A1Fh) */
    struct io_iip_uinftbl             UNITINFTBL_CSC1;                        /* 2888_(0A20 - 0A3Fh) */
    struct io_iip_uinftbl             UNITINFTBL_LUT;                         /* 2888_(0A40 - 0A5Fh) */
    struct io_iip_uinftbl             UNITINFTBL_GPC;                         /* 2888_(0A60 - 0A7Fh) */
    struct io_iip_uinftbl             UNITINFTBL_BLD0;                        /* 2888_(0A80 - 0A9Fh) */
    struct io_iip_uinftbl             UNITINFTBL_BLD1;                        /* 2888_(0AA0 - 0ABFh) */
    struct io_iip_uinftbl             UNITINFTBL_FRC1;                        /* 2888_(0AC0 - 0ADFh) */
    struct io_iip_uinftbl             UNITINFTBL_CFL0;                        /* 2888_(0AE0 - 0AFFh) */

    unsigned char dmy_0B00_0B5F[0x0B60-0x0B00]; /* 2888_(0B00 - 0B5Fh) */

    struct io_iip_uinftbl             UNITINFTBL_CSC2;                        /* 2888_(0B60 - 0B7Fh) */
    struct io_iip_uinftbl             UNITINFTBL_CSC3;                        /* 2888_(0B80 - 0B9Fh) */
    struct io_iip_uinftbl             UNITINFTBL_SL6;                         /* 2888_(0BA0 - 0BBFh) */
    struct io_iip_uinftbl             UNITINFTBL_SL7;                         /* 2888_(0BC0 - 0BDFh) */
    struct io_iip_uinftbl             UNITINFTBL_SL8;                         /* 2888_(0BE0 - 0BFFh) */
    struct io_iip_uinftbl             UNITINFTBL_LD4;                         /* 2888_(0C00 - 0C1Fh) */
    struct io_iip_uinftbl             UNITINFTBL_LD5;                         /* 2888_(0C20 - 0C3Fh) */
    struct io_iip_uinftbl             UNITINFTBL_LD6;                         /* 2888_(0C40 - 0C5Fh) */
    struct io_iip_uinftbl             UNITINFTBL_LD7;                         /* 2888_(0C60 - 0C7Fh) */
    struct io_iip_uinftbl             UNITINFTBL_AFN3;                        /* 2888_(0C80 - 0C9Fh) */
    struct io_iip_uinftbl             UNITINFTBL_MFT;                         /* 2888_(0CA0 - 0CBFh) */

    unsigned char dmy_28880CC0_2888FFFF[0x28890000-0x28880CC0]; /* 2888_0CC0 - 2888_FFFFh */

    unsigned long                     UNITDEBUG[0x10000 / 4];                 /* 2889_(0000 - FFFFh) */

};

extern volatile struct io_iip	IO_IIP;

