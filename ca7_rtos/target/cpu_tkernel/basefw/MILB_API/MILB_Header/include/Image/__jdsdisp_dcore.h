/**
 * @file		__jdsdisp_dcore.h
 * @brief		Definition JDSDISP3A DCORE Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*  structure of RESET  (2890_1000h)    */
union io_jdsdcore_reg_reset{
    unsigned long       word;
    struct {
        unsigned long   SR  :1;
        unsigned long       :31;
    }bit;
};

/*  structure of IFS    (2890_1100h)    */
union io_jdsdcore_reg_ifs{
    unsigned long       word;
    struct {
        unsigned long   IFS :2;
        unsigned long       :30;
    }bit;
};

/*  structure of TRG    (2890_1200h)    */
union io_jdsdcore_reg_trg{
    unsigned long       word;
    struct {
        unsigned long   TRG     :2;
        unsigned long           :2;
        unsigned long   TGKMD   :1;
        unsigned long           :27;
    }bit;
};

/*  structure of TGKST  (2890_1204h)    */
union io_jdsdcore_reg_tgkst{
    unsigned long       word;
    struct {
        unsigned long   TGKDL   :12;
        unsigned long           :20;
    }bit;
};

/*  structure of TOCTL  (2890_1208h)    */
union io_jdsdcore_reg_toctl{
    unsigned long       word;
    struct {
        unsigned long   CON     :1;
        unsigned long           :3;
        unsigned long           :2;
        unsigned long           :2;
        unsigned long   CKOEN   :1;
        unsigned long   CKICEN  :1;
        unsigned long           :2;
        unsigned long           :4;
        unsigned long   WBEN    :1;
        unsigned long           :15;
    }bit;
};

/*  structure of INTC   (2890_120Ch)    */
union io_jdsdcore_reg_intc{
    unsigned long       word;
    struct {
        unsigned long   VFTMG   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of INTE   (2890_1210h)    */
union io_jdsdcore_reg_inte{
    unsigned long       word;
    struct {
        unsigned long   LEE     :1;
        unsigned long           :3;
        unsigned long   GR0EE   :1;
        unsigned long   GR1EE   :1;
        unsigned long   GA0EE   :1;
        unsigned long   GA1EE   :1;
        unsigned long   LREE    :1;
        unsigned long           :3;
        unsigned long   GR0REE  :1;
        unsigned long   GR1REE  :1;
        unsigned long   GA0REE  :1;
        unsigned long   GA1REE  :1;
        unsigned long   VE      :1;
        unsigned long           :3;
        unsigned long   VE2     :1;
        unsigned long           :11;
    }bit;
};

/*  structure of INTF   (2890_1214h)    */
union io_jdsdcore_reg_intf{
    unsigned long       word;
    struct {
        unsigned long   LEF     :1;
        unsigned long           :3;
        unsigned long   GR0EF   :1;
        unsigned long   GR1EF   :1;
        unsigned long   GA0EF   :1;
        unsigned long   GA1EF   :1;
        unsigned long   LREF    :1;
        unsigned long           :3;
        unsigned long   GR0REF  :1;
        unsigned long   GR1REF  :1;
        unsigned long   GA0REF  :1;
        unsigned long   GA1REF  :1;
        unsigned long   VF      :1;
        unsigned long           :3;
        unsigned long   VF2     :1;
        unsigned long           :11;
    }bit;
};

/*  structure of AXISTS (2890_1218h)    */
union io_jdsdcore_reg_axists{
    unsigned long       word;
    struct {
        unsigned long   LRESP       :2;
        unsigned long               :2;
        unsigned long               :2;
        unsigned long               :2;
        unsigned long               :2;
        unsigned long               :2;
        unsigned long               :2;
        unsigned long               :2;
        unsigned long   GR0RESP     :2;
        unsigned long               :2;
        unsigned long   GR1RESP     :2;
        unsigned long               :2;
        unsigned long   GRA0RESP    :2;
        unsigned long               :2;
        unsigned long   GRA1RESP    :2;
        unsigned long               :2;
    }bit;
};

/*  structure of RPGCTL (2890_1220h)    */
union io_jdsdcore_reg_rpgctl{
    unsigned long       word;
    struct {
        unsigned long   RPGTMG  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of RPGEN  (2890_1224h)    */
union io_jdsdcore_reg_rpgen{
    unsigned long       word;
    struct {
        unsigned long   RPGEN   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of POLSEL (2890_1300h)    */
union io_jdsdcore_reg_polsel{
    unsigned long       word;
    struct {
        unsigned long   VDPS    :1;
        unsigned long           :3;
        unsigned long   HDPS    :1;
        unsigned long           :3;
        unsigned long   VEPS    :1;
        unsigned long           :3;
        unsigned long   HEPS    :1;
        unsigned long           :3;
        unsigned long   CKEG    :1;
        unsigned long           :15;
    }bit;
};

/*  structure of TSL    (2890_1304h)    */
union io_jdsdcore_reg_tsl{
    unsigned long       word;
    struct {
        unsigned long   TSL :1;
        unsigned long       :31;
    }bit;
};

/*  structure of VCYC   (2890_1308h)    */
union io_jdsdcore_reg_vcyc{
    unsigned long       word;
    struct {
        unsigned long   VCYC1   :14;
        unsigned long           :2;
        unsigned long   VCYC2   :14;
        unsigned long           :1;
        unsigned long   VDSFT   :1;
    }bit;
};

/*  structure of HCYC   (2890_130Ch)    */
union io_jdsdcore_reg_hcyc{
    unsigned long       word;
    struct {
        unsigned long   HCYC    :16;
        unsigned long           :16;
    }bit;
};

/*  structure of OVPW   (2890_1310h)    */
union io_jdsdcore_reg_ovpw{
    unsigned long       word;
    struct {
        unsigned long   OVPW    :10;
        unsigned long           :2;
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   OVPWU   :1;
        unsigned long           :15;
    }bit;
};

/*  structure of HPW    (2890_1314h)    */
union io_jdsdcore_reg_hpw{
    unsigned long       word;
    struct {
        unsigned long   HPW :10;
        unsigned long       :22;
    }bit;
};

/*  structure of VBLK   (2890_1318h)    */
union io_jdsdcore_reg_vblk{
    unsigned long       word;
    struct {
        unsigned long   VBLK1   :8;
        unsigned long   VBLK2   :8;
        unsigned long           :16;
    }bit;
};

/*  structure of HBLK   (2890_131Ch)    */
union io_jdsdcore_reg_hblk{
    unsigned long       word;
    struct {
        unsigned long   HBLK    :10;
        unsigned long           :22;
    }bit;
};

/*  structure of VDLY   (2890_1320h)    */
union io_jdsdcore_reg_vdly{
    unsigned long       word;
    struct {
        unsigned long   VDLY    :8;
        unsigned long           :24;
    }bit;
};

/*  structure of HDLY   (2890_1324h)    */
union io_jdsdcore_reg_hdly{
    unsigned long       word;
    struct {
        unsigned long   HDLY    :8;
        unsigned long           :24;
    }bit;
};

/*  structure of OVSIZE (2890_1328h)    */
union io_jdsdcore_reg_ovsize{
    unsigned long       word;
    struct {
        unsigned long   OVSIZE  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of OHSIZE (2890_132Ch)    */
union io_jdsdcore_reg_ohsize{
    unsigned long       word;
    struct {
        unsigned long   OHSIZE  :16;
        unsigned long           :16;
    }bit;
};

/*  structure of VRFCTL (2890_1330h)    */
union io_jdsdcore_reg_vrfctl{
    unsigned long       word;
    struct {
        unsigned long   VENS    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of HRFCTL (2890_1338h)    */
union io_jdsdcore_reg_hrfctl{
    unsigned long       word;
    struct {
        unsigned long   HENS    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of HABLK  (2890_1340h)    */
union io_jdsdcore_reg_hablk{
    unsigned long       word;
    struct {
        unsigned long   HABLK   :12;
        unsigned long           :20;
    }bit;
};

/*  structure of DOMD   (2890_1400h)    */
union io_jdsdcore_reg_domd{
    unsigned long       word;
    struct {
        unsigned long   MODE    :8;
        unsigned long           :8;
        unsigned long   OBPSL   :1;
        unsigned long           :3;
        unsigned long           :4;
        unsigned long   OBUSL   :2;
        unsigned long           :6;
    }bit;
};

/*  structure of FDOEN  (2890_1410h)    */
union io_jdsdcore_reg_fdoen{
    unsigned long       word;
    struct {
        unsigned long   FDEN    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of FODATA (2890_1414h)    */
union io_jdsdcore_reg_fodata{
    unsigned long       word;
    struct {
        unsigned long   FOD_0   :8;
        unsigned long   FOD_1   :8;
        unsigned long   FOD_2   :8;
        unsigned long           :8;
    }bit;
};

/*  structure of BLANKDT    (2890_1420h)    */
union io_jdsdcore_reg_blankdt_1{
    unsigned long       word;
    struct {
        unsigned long   BDT_0   :16;
        unsigned long   BDT_1   :16;
    }bit;
};
union io_jdsdcore_reg_blankdt_2{
    unsigned long       word;
    struct {
        unsigned long   BDT_2   :16;
        unsigned long           :16;
    }bit;
};
struct io_jdsdcore_reg_blankdt{
    union io_jdsdcore_reg_blankdt_1 BLANKDT1;
    union io_jdsdcore_reg_blankdt_2 BLANKDT2;
};

/*  structure of CLBHSIZE   (2890_1430h)    */
union io_jdsdcore_reg_clbhsize{
    unsigned long       word;
    struct {
        unsigned long   CLBHSIZE    :16;
        unsigned long               :16;
    }bit;
};

/*  structure of CLBDT  (2890_1440h)    */
union io_jdsdcore_reg_clbdt{
    unsigned long       word;
    struct {
        unsigned long   CLBR    :8;
        unsigned long   CLBG    :8;
        unsigned long   CLBB    :8;
        unsigned long           :8;
    }bit;
};

/*  structure of BLDCTL (2890_1480h)    */
union io_jdsdcore_reg_bldctl{
    unsigned long       word;
    struct {
        unsigned long   BLD1    :2;
        unsigned long           :2;
        unsigned long   BLD2    :2;
        unsigned long           :2;
        unsigned long   BLD3    :2;
        unsigned long           :2;
        unsigned long   BLD4    :2;
        unsigned long           :18;
    }bit;
};

/*  structure of R2Y0/R2Y1/R2Y2   (2890_1490h)    */
union io_jdsdcore_reg_r2y{
    unsigned long long      dword;
    struct {
        unsigned long long  RY0     :16;
        unsigned long long  RY1     :16;
        unsigned long long  RY2     :16;
        unsigned long long          :16;
    }bit;
};

/*  structure of YCAL   (2890_14B0h)    */
union io_jdsdcore_reg_ycal{
    unsigned long long      dword;
    struct {
        unsigned long long  YGAIN   :17;
        unsigned long long          :15;
        unsigned long long  YOFS    :13;
        unsigned long long          :19;
    }bit;
};

/*  structure of YCLIP  (2890_14B8h)    */
union io_jdsdcore_reg_yclip{
    unsigned long       word;
    struct {
        unsigned long   YCPH    :16;
        unsigned long   YCPL    :16;
    }bit;
};

/*  structure of CBCAL  (2890_14BCh)    */
union io_jdsdcore_reg_cbcal{
    unsigned long       word;
    struct {
        unsigned long   CBGAIN  :16;
        unsigned long   CBOFS   :14;
        unsigned long           :2;
    }bit;
};

/*  structure of CBCLIP (2890_14C0h)    */
union io_jdsdcore_reg_cbclip{
    unsigned long       word;
    struct {
        unsigned long   CBCPH   :16;
        unsigned long   CBCPL   :16;
    }bit;
};

/*  structure of CRCAL  (2890_14C4h)    */
union io_jdsdcore_reg_crcal{
    unsigned long       word;
    struct {
        unsigned long   CRGAIN  :16;
        unsigned long   CROFS   :14;
        unsigned long           :2;
    }bit;
};

/*  structure of CRCLIP (2890_14C8h)    */
union io_jdsdcore_reg_crclip{
    unsigned long       word;
    struct {
        unsigned long   CRCPH   :16;
        unsigned long   CRCPL   :16;
    }bit;
};

/*  structure of DOCTL0 (2890_14D0h)    */
union io_jdsdcore_reg_doctl0{
    unsigned long       word;
    struct {
        unsigned long   DTO1    :2;
        unsigned long           :2;
        unsigned long   DTO2    :2;
        unsigned long           :2;
        unsigned long   DTO3    :2;
        unsigned long           :2;
        unsigned long   DTO4    :2;
        unsigned long           :2;
        unsigned long   DTO5    :2;
        unsigned long           :2;
        unsigned long   DTO6    :2;
        unsigned long           :10;
    }bit;
};

/*  structure of DOCTL1 (2890_14D4h)    */
union io_jdsdcore_reg_doctl1{
    unsigned long       word;
    struct {
        unsigned long   SOO     :1;
        unsigned long           :3;
        unsigned long   SOE     :1;
        unsigned long           :3;
        unsigned long   ODO0    :1;
        unsigned long   ODO1    :1;
        unsigned long   ODO2    :1;
        unsigned long           :1;
        unsigned long   ODE0    :1;
        unsigned long   ODE1    :1;
        unsigned long   ODE2    :1;
        unsigned long           :17;
    }bit;
};

/*  structure of DOCTL2 (2890_14D8h)    */
union io_jdsdcore_reg_doctl2{
    unsigned long       word;
    struct {
        unsigned long   CLVS    :1;
        unsigned long           :3;
        unsigned long   CTOH    :2;
        unsigned long           :2;
        unsigned long   CTOV    :1;
        unsigned long           :3;
        unsigned long           :4;
        unsigned long   TRSOS   :2;
        unsigned long           :14;
    }bit;
};

/*  structure of TRSCODE0   (2890_14E0h)    */
union io_jdsdcore_reg_trscode0{
    unsigned long       word;
    struct {
        unsigned long   EAV00   :8;
        unsigned long   SAV00   :8;
        unsigned long   EAV01   :8;
        unsigned long   SAV01   :8;
    }bit;
};

/*  structure of TRSCODE1   (2890_14E4h)    */
union io_jdsdcore_reg_trscode1{
    unsigned long       word;
    struct {
        unsigned long   EAV10   :8;
        unsigned long   SAV10   :8;
        unsigned long   EAV11   :8;
        unsigned long   SAV11   :8;
    }bit;
};

/*  structure of Y2R0-2   (2890_14F0h)    */
union io_jdsdcore_reg_y2r{
    unsigned long long      dword;
    struct {
        unsigned long long  YR0     :16;
        unsigned long long  YR1     :16;
        unsigned long long  YR2     :16;
        unsigned long long          :16;
    }bit;
};

/*  structure of TBLASET    (2890_1510h)    */
union io_jdsdcore_reg_tblaset{
    unsigned long       word;
    struct {
        unsigned long   GMTAEN  :1;
        unsigned long   GMEN    :1;
        unsigned long   GMTSL   :1;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   YSATTA  :1;
        unsigned long   YSATEN  :1;
        unsigned long   YSATSL  :1;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   GMTMN   :1;
        unsigned long           :3;
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   YSATMN  :1;
        unsigned long           :7;
    }bit;
};

/*  structure of GHDSTA (2890_1600h)    */
union io_jdsdcore_reg_ghdsta{
    unsigned long       word;
    struct {
        unsigned long   GHDSH   :16;
        unsigned long   GHDSV   :14;
        unsigned long           :2;
    }bit;
};

/*  structure of GVDSTA (2890_1604h)    */
union io_jdsdcore_reg_gvdsta{
    unsigned long       word;
    struct {
        unsigned long   GVDSH   :16;
        unsigned long   GVDSV   :14;
        unsigned long           :2;
    }bit;
};

/*  structure of GLENGTH    (2890_1608h)    */
union io_jdsdcore_reg_glength{
    unsigned long       word;
    struct {
        unsigned long   GHLEN   :16;
        unsigned long   GVLEN   :14;
        unsigned long           :2;
    }bit;
};

/*  structure of GWIDTH (2890_160Ch)    */
union io_jdsdcore_reg_gwidth{
    unsigned long       word;
    struct {
        unsigned long   GHWID   :6;
        unsigned long           :2;
        unsigned long           :8;
        unsigned long   GVWID   :6;
        unsigned long           :10;
    }bit;
};

/*  structure of GITVL  (2890_1610h)    */
union io_jdsdcore_reg_gitvl{
    unsigned long       word;
    struct {
        unsigned long   GHITV   :14;
        unsigned long           :2;
        unsigned long   GVITV   :16;
    }bit;
};

/*  structure of GNUM   (2890_1614h)    */
union io_jdsdcore_reg_gnum{
    unsigned long       word;
    struct {
        unsigned long   GHNUM   :3;
        unsigned long           :5;
        unsigned long           :8;
        unsigned long   GVNUM   :3;
        unsigned long           :13;
    }bit;
};

/*  structure of GDCTL  (2890_1618h)    */
union io_jdsdcore_reg_gdctl{
    unsigned long       word;
    struct {
        unsigned long   GCLRR   :8;
        unsigned long   GCLRG   :8;
        unsigned long   GCLRB   :8;
        unsigned long   GALP    :2;
        unsigned long           :6;
    }bit;
};

/*  structure of GDISPEN    (2890_1620h)    */
union io_jdsdcore_reg_gdispen{
    unsigned long       word;
    struct {
        unsigned long   GDEN    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of FFDSTA (2890_1800h)    */
union io_jdsdcore_reg_ffdsta{
    unsigned long       word;
    struct {
        unsigned long   FFDSH       :16;
        unsigned long   FFDSV       :14;
        unsigned long               :2;
    }bit;
};

/*  structure of FFSIZE (2890_1900h)    */
union io_jdsdcore_reg_ffsize{
    unsigned long       word;
    struct {
        unsigned long   FFHSZ       :16;
        unsigned long   FFVSZ       :14;
        unsigned long               :2;
    }bit;
};

/*  structure of FFWIDTH    (2890_1A00h)    */
union io_jdsdcore_reg_ffwidth{
    unsigned long       word;
    struct {
        unsigned long   FFHWID      :6;
        unsigned long               :2;
        unsigned long               :8;
        unsigned long   FFVWID      :6;
        unsigned long               :10;
    }bit;
};

/*  structure of FFCLR  (2890_1B00h)    */
union io_jdsdcore_reg_ffclr{
    unsigned long       word;
    struct {
        unsigned long   FFCLRR      :8;
        unsigned long   FFCLRG      :8;
        unsigned long   FFCLRB      :8;
        unsigned long               :8;
    }bit;
};

/*  structure of FFDISPEN   (2890_1C00h)    */
union io_jdsdcore_reg_ffdispen{
    unsigned long long      dword;
    struct {
        unsigned long long  FFDEN   :42;
        unsigned long long          :22;
    }bit;
};

/*  structure of FFDO   (2890_1C08h)    */
union io_jdsdcore_reg_ffdo{
    unsigned long       word;
    struct {
        unsigned long   MSFF    :6;
        unsigned long           :26;
    }bit;
};

/* Define i/o mapping */
struct io_jdsdcore_reg{
    /* JDSIMG */
    union  io_jdsdcore_reg_reset    RESET;      /* 2890_(1000 - 1003h) */

    unsigned char dmy_1004_10FF[0x1100-0x1004]; /* 2890_(1004 - 10FFh) */

    union  io_jdsdcore_reg_ifs      IFS;        /* 2890_(1100 - 1103h) */

    unsigned char dmy_1104_11FF[0x1200-0x1104]; /* 2890_(1104 - 11FFh) */

    union  io_jdsdcore_reg_trg      TRG;        /* 2890_(1200 - 1203h) */
    union  io_jdsdcore_reg_tgkst    TGKST;      /* 2890_(1204 - 1207h) */
    union  io_jdsdcore_reg_toctl    TOCTL;      /* 2890_(1208 - 120Bh) */
    union  io_jdsdcore_reg_intc     INTC;       /* 2890_(120C - 120Fh) */
    union  io_jdsdcore_reg_inte     INTE;       /* 2890_(1210 - 1213h) */
    union  io_jdsdcore_reg_intf     INTF;       /* 2890_(1214 - 1217h) */
    union  io_jdsdcore_reg_axists   AXISTS;     /* 2890_(1218 - 121Bh) */

    unsigned char dmy_121C_121F[0x1220-0x121C]; /* 2890_(121C - 121Fh) */

    union  io_jdsdcore_reg_rpgctl   RPGCTL;     /* 2890_(1220 - 1223h) */
    union  io_jdsdcore_reg_rpgen    RPGEN;      /* 2890_(1224 - 1227h) */

    unsigned char dmy_1228_12FF[0x1300-0x1228]; /* 2890_(1228 - 12FFh) */

    union  io_jdsdcore_reg_polsel   POLSEL;     /* 2890_(1300 - 1303h) */
    union  io_jdsdcore_reg_tsl      TSL;        /* 2890_(1304 - 1307h) */
    union  io_jdsdcore_reg_vcyc     VCYC;       /* 2890_(1308 - 130Bh) */
    union  io_jdsdcore_reg_hcyc     HCYC;       /* 2890_(130C - 130Fh) */
    union  io_jdsdcore_reg_ovpw     OVPW;       /* 2890_(1310 - 1313h) */
    union  io_jdsdcore_reg_hpw      HPW;        /* 2890_(1314 - 1317h) */
    union  io_jdsdcore_reg_vblk     VBLK;       /* 2890_(1318 - 131Bh) */
    union  io_jdsdcore_reg_hblk     HBLK;       /* 2890_(131C - 131Fh) */
    union  io_jdsdcore_reg_vdly     VDLY;       /* 2890_(1320 - 1323h) */
    union  io_jdsdcore_reg_hdly     HDLY;       /* 2890_(1324 - 1327h) */
    union  io_jdsdcore_reg_ovsize   OVSIZE;     /* 2890_(1328 - 132Bh) */
    union  io_jdsdcore_reg_ohsize   OHSIZE;     /* 2890_(132C - 132Fh) */
    union  io_jdsdcore_reg_vrfctl   VRFCTL;     /* 2890_(1330 - 1333h) */

    unsigned char dmy_1334_1337[0x1338-0x1334]; /* 2890_(1334 - 1337h) */

    union  io_jdsdcore_reg_hrfctl   HRFCTL;     /* 2890_(1338 - 133Bh) */

    unsigned char dmy_133C_133F[0x1340-0x133C]; /* 2890_(133C - 133Fh) */

    union  io_jdsdcore_reg_hablk    HABLK;      /* 2890_(1340 - 1343h) */

    unsigned char dmy_1344_13FF[0x1400-0x1344]; /* 2890_(1344 - 13FFh) */

    union  io_jdsdcore_reg_domd     DOMD;       /* 2890_(1400 - 1403h) */

    unsigned char dmy_1404_140F[0x1410-0x1404]; /* 2890_(1404 - 140Fh) */

    union  io_jdsdcore_reg_fdoen    FDOEN;      /* 2890_(1410 - 1413h) */
    union  io_jdsdcore_reg_fodata   FODATA;     /* 2890_(1414 - 1417h) */

    unsigned char dmy_1418_141F[0x1420-0x1418]; /* 2890_(1418 - 141Fh) */

    struct io_jdsdcore_reg_blankdt  BLANKDT;    /* 2890_(1420 - 1427h) */

    unsigned char dmy_1428_142F[0x1430-0x1428]; /* 2890_(1428 - 142Fh) */

    union  io_jdsdcore_reg_clbhsize CLBHSIZE;   /* 2890_(1430 - 1433h) */

    unsigned char dmy_1434_143F[0x1440-0x1434]; /* 2890_(1434 - 143Fh) */

    union  io_jdsdcore_reg_clbdt    CLBDT[16];  /* 2890_(1440 - 147Fh) */
    union  io_jdsdcore_reg_bldctl   BLDCTL;     /* 2890_(1480 - 1483h) */

    unsigned char dmy_1484_148F[0x1490-0x1484]; /* 2890_(1484 - 148Fh) */

    union  io_jdsdcore_reg_r2y      R2Y[3];     /* 2890_(1490 - 14A7h) */

    unsigned char dmy_14A8_14AF[0x14B0-0x14A8]; /* 2890_(14A8 - 14AFh) */

    union  io_jdsdcore_reg_ycal     YCAL;       /* 2890_(14B0 - 14B7h) */
    union  io_jdsdcore_reg_yclip    YCLIP;      /* 2890_(14B8 - 14BBh) */
    union  io_jdsdcore_reg_cbcal    CBCAL;      /* 2890_(14BC - 14BFh) */
    union  io_jdsdcore_reg_cbclip   CBCLIP;     /* 2890_(14C0 - 14C3h) */
    union  io_jdsdcore_reg_crcal    CRCAL;      /* 2890_(14C4 - 14C7h) */
    union  io_jdsdcore_reg_crclip   CRCLIP;     /* 2890_(14C8 - 14CBh) */

    unsigned char dmy_14CC_14CF[0x14D0-0x14CC]; /* 2890_(14CC - 14CFh) */

    union  io_jdsdcore_reg_doctl0   DOCTL0;     /* 2890_(14D0 - 14D3h) */
    union  io_jdsdcore_reg_doctl1   DOCTL1;     /* 2890_(14D4 - 14D7h) */
    union  io_jdsdcore_reg_doctl2   DOCTL2;     /* 2890_(14D8 - 14DBh) */

    unsigned char dmy_14DC_14DF[0x14E0-0x14DC]; /* 2890_(14DC - 14DFh) */

    union  io_jdsdcore_reg_trscode0 TRSCODE0;   /* 2890_(14E0 - 14E3h) */
    union  io_jdsdcore_reg_trscode1 TRSCODE1;   /* 2890_(14E4 - 14E7h) */

    unsigned char dmy_14E8_14EF[0x14F0-0x14E8]; /* 2890_(14E8 - 14EFh) */

    union  io_jdsdcore_reg_y2r      Y2R[3];     /* 2890_(14F0 - 1507h) */

    unsigned char dmy_1508_150F[0x1510-0x1508]; /* 2890_(1508 - 150Fh) */

    union  io_jdsdcore_reg_tblaset  TBLASET;    /* 2890_(1510 - 1513h) */

    unsigned char dmy_1514_15FF[0x1600-0x1514]; /* 2890_(1514 - 15FFh) */

    union  io_jdsdcore_reg_ghdsta   GHDSTA;     /* 2890_(1600 - 1603h) */
    union  io_jdsdcore_reg_gvdsta   GVDSTA;     /* 2890_(1604 - 1607h) */
    union  io_jdsdcore_reg_glength  GLENGTH;    /* 2890_(1608 - 160Bh) */
    union  io_jdsdcore_reg_gwidth   GWIDTH;     /* 2890_(160C - 160Fh) */
    union  io_jdsdcore_reg_gitvl    GITVL;      /* 2890_(1610 - 1613h) */
    union  io_jdsdcore_reg_gnum     GNUM;       /* 2890_(1614 - 1617h) */
    union  io_jdsdcore_reg_gdctl    GDCTL;      /* 2890_(1618 - 161Bh) */

    unsigned char dmy_161C_161F[0x1620-0x161C]; /* 2890_(161C - 161Fh) */

    union  io_jdsdcore_reg_gdispen  GDISPEN;    /* 2890_(1620 - 1623h) */

    unsigned char dmy_1624_17FF[0x1800-0x1624]; /* 2890_(1624 - 17FFh) */

    union  io_jdsdcore_reg_ffdsta   FFDSTA[42]; /* 2890_(1800 - 18A7h) */

    unsigned char dmy_18A8_18FF[0x1900-0x18A8]; /* 2890_(18A8 - 18FFh) */

    union  io_jdsdcore_reg_ffsize   FFSIZE[42]; /* 2890_(1900 - 19A7h) */

    unsigned char dmy_19A8_19FF[0x1A00-0x19A8]; /* 2890_(19A8 - 19FFh) */

    union  io_jdsdcore_reg_ffwidth  FFWIDTH[42];/* 2890_(1A00 - 1AA7h) */

    unsigned char dmy_1AA8_1AFF[0x1B00-0x1AA8]; /* 2890_(1AA8 - 1AFFh) */

    union  io_jdsdcore_reg_ffclr    FFCLR[42];  /* 2890_(1B00 - 1BA7h) */

    unsigned char dmy_1BA8_1BFF[0x1C00-0x1BA8]; /* 2890_(1BA8 - 1BFFh) */

    union  io_jdsdcore_reg_ffdispen FFDISPEN;   /* 2890_(1C00 - 1C07h) */
    union  io_jdsdcore_reg_ffdo     FFDO;       /* 2890_(1C08 - 1C0Bh) */

    unsigned char dmy_1C0C_1FFF[0x2000-0x1C0C]; /* 2890_(1C0C - 1FFFh) */

};

