/**
 * @file		__jdsdisp_lch.h
 * @brief		Definition JDSDISP3A LCH Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*  structure of LRST   (2890_0000h)    */
union io_jdslch_reg_lrst{
    unsigned long       word;
    struct {
        unsigned long   SR  :1;
        unsigned long       :31;
    }bit;
};

/*  structure of LTRG   (2890_0100h)    */
union io_jdslch_reg_ltrg{
    unsigned long       word;
    struct {
        unsigned long   TRG :2;
        unsigned long       :30;
    }bit;
};

/*  structure of LRPGCTL    (2890_0110h)    */
union io_jdslch_reg_lrpgctl{
    unsigned long       word;
    struct {
        unsigned long   RPGTMG  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of LIDT0  (2890_0200h)    */
union io_jdslch_reg_lidt0{
    unsigned long       word;
    struct {
        unsigned long   IFMT    :5;
        unsigned long           :3;
        unsigned long   NBT     :2;
        unsigned long           :2;
        unsigned long   IFEW    :2;
        unsigned long           :2;
        unsigned long   LVFM    :1;
        unsigned long           :15;
    }bit;
};

/*  structure of LIDT1  (2890_0204h)    */
union io_jdslch_reg_lidt1{
    unsigned long       word;
    struct {
        unsigned long   PKGDV   :4;
        unsigned long   CACHE   :4;
        unsigned long   PROT    :3;
        unsigned long           :1;
        unsigned long   YSLVSL  :1;
        unsigned long   CSLVSL  :1;
        unsigned long           :18;
    }bit;
};

/*  structure of LISIZE (2890_0208h)    */
union io_jdslch_reg_lisize{
    unsigned long       word;
    struct {
        unsigned long   IHSIZE  :16;
        unsigned long   IVSIZE  :14;
        unsigned long           :2;
    }bit;
};

/*  structure of LYSA0-3  (2890_0210h)    */
union io_jdslch_reg_lysa{
    unsigned long       word;
    struct {
        unsigned long   YSA    :32;
    }bit;
};

/*  structure of LCSA0-3  (2890_0220h)    */
union io_jdslch_reg_lcsa{
    unsigned long       word;
    struct {
        unsigned long   CSA     :32;
    }bit;
};

/*  structure of LYHGA  (2890_0240h)    */
union io_jdslch_reg_lyhga{
    unsigned long       word;
    struct {
        unsigned long   YHGA    :16;
        unsigned long           :16;
    }bit;
};

/*  structure of LCHGA  (2890_0250h)    */
union io_jdslch_reg_lchga{
    unsigned long       word;
    struct {
        unsigned long   CHGA    :16;
        unsigned long           :16;
    }bit;
};

/*  structure of LIBCTL (2890_0260h)    */
union io_jdslch_reg_libctl{
    unsigned long       word;
    struct {
        unsigned long   IBEN    :1;
        unsigned long           :3;
        unsigned long           :4;
        unsigned long   IBSET   :2;
        unsigned long           :2;
        unsigned long           :4;
        unsigned long   IBMNT   :2;
        unsigned long           :2;
        unsigned long           :4;
        unsigned long           :8;
    }bit;
};

/*  structure of LRSZ0  (2890_0400h)    */
union io_jdslch_reg_lrsz0{
    unsigned long       word;
    struct {
        unsigned long   RSZSL   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of LRSZ1  (2890_0404h)    */
union io_jdslch_reg_lrsz1{
    unsigned long       word;
    struct {
        unsigned long   HRSZM   :5;
        unsigned long           :3;
        unsigned long   HRSZN   :5;
        unsigned long           :3;
        unsigned long           :7;
        unsigned long           :1;
        unsigned long   HRSZOF  :5;
        unsigned long           :3;
    }bit;
};

/*  structure of LRSZ2  (2890_0408h)    */
union io_jdslch_reg_lrsz2{
    unsigned long       word;
    struct {
        unsigned long   VRSZM   :5;
        unsigned long           :3;
        unsigned long   VRSZN   :5;
        unsigned long           :3;
        unsigned long           :8;
        unsigned long   VRSZOF  :5;
        unsigned long           :3;
    }bit;
};

/*  structure of LRSZ3  (2890_040Ch)    */
union io_jdslch_reg_lrsz3{
    unsigned long       word;
    struct {
        unsigned long   HCSTA   :6;
        unsigned long           :2;
        unsigned long   VCSTA   :6;
        unsigned long           :18;
    }bit;
};

/*  structure of LYHLPCTL   (2890_0410h)    */
union io_jdslch_reg_lyhlpctl{
    unsigned long       word;
    struct {
        unsigned long   YHLPEN  :1;
        unsigned long           :3;
        unsigned long           :4;
        unsigned long   YHLPMD  :2;
        unsigned long           :22;
    }bit;
};

/*  structure of LYHLPK0    (2890_0414h)    */
union io_jdslch_reg_lyhlpk0{
    unsigned long       word;
    struct {
        unsigned long   YHLPK0  :9;
        unsigned long           :7;
        unsigned long   YHLPK1  :8;
        unsigned long           :8;
    }bit;
};

/*  structure of LYHLPK1    (2890_0418h)    */
union io_jdslch_reg_lyhlpk1{
    unsigned long       word;
    struct {
        unsigned long   YHLPK2  :8;
        unsigned long           :8;
        unsigned long   YHLPK3  :8;
        unsigned long           :8;
    }bit;
};

/*  structure of LYHLPOL    (2890_041Ch)    */
union io_jdslch_reg_lyhlpol{
    unsigned long       word;
    struct {
        unsigned long   YHLPCOR :16;
        unsigned long   YHLPTH  :16;
    }bit;
};

/*  structure of LYHLPCLR   (2890_0420h)    */
union io_jdslch_reg_lyhlpclr{
    unsigned long long      dword;
    struct {
        unsigned long long  YHLPY   :16;
        unsigned long long  YHLPCB  :16;
        unsigned long long  YHLPCR  :16;
        unsigned long long          :16;
    }bit;
};

/*  structure of LYWCTL (2890_0430h)    */
union io_jdslch_reg_lywctl{
    unsigned long       word;
    struct {
        unsigned long   YWEN    :1;
        unsigned long           :3;
        unsigned long           :4;
        unsigned long   YWSL    :2;
        unsigned long           :22;
    }bit;
};

/*  structure of LYW0TH (2890_0434h)    */
union io_jdslch_reg_lyw0th{
    unsigned long       word;
    struct {
        unsigned long   YW0THH  :16;
        unsigned long   YW0THL  :16;
    }bit;
};

/*  structure of LYW1TH (2890_0438h)    */
union io_jdslch_reg_lyw1th{
    unsigned long       word;
    struct {
        unsigned long   YW1THH  :16;
        unsigned long   YW1THL  :16;
    }bit;
};

/*  structure of LYW0ST (2890_043Ch)    */
union io_jdslch_reg_lyw0st{
    unsigned long       word;
    struct {
        unsigned long   YW0MD   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of LYW0CA (2890_0440h)    */
union io_jdslch_reg_lyw0ca{
    unsigned long long      dword;
    struct {
        unsigned long long  YW0AY   :16;
        unsigned long long  YW0ACB  :16;
        unsigned long long  YW0ACR  :16;
        unsigned long long  YW0AA   :8;
        unsigned long long          :8;
    }bit;
};

/*  structure of LYW0CB (2890_0448h)    */
union io_jdslch_reg_lyw0cb{
    unsigned long long      dword;
    struct {
        unsigned long long  YW0BY   :16;
        unsigned long long  YW0BCB  :16;
        unsigned long long  YW0BCR  :16;
        unsigned long long  YW0BA   :8;
        unsigned long long          :8;
    }bit;
};

/*  structure of LYW1ST (2890_0450h)    */
union io_jdslch_reg_lyw1st{
    unsigned long       word;
    struct {
        unsigned long   YW1MD   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of LYW1CA (2890_0458h)    */
union io_jdslch_reg_lyw1ca{
    unsigned long long      dword;
    struct {
        unsigned long long  YW1AY   :16;
        unsigned long long  YW1ACB  :16;
        unsigned long long  YW1ACR  :16;
        unsigned long long  YW1AA   :8;
        unsigned long long          :8;
    }bit;
};

/*  structure of LYW1CB (2890_0460h)    */
union io_jdslch_reg_lyw1cb{
    unsigned long long      dword;
    struct {
        unsigned long long  YW1BY   :16;
        unsigned long long  YW1BCB  :16;
        unsigned long long  YW1BCR  :16;
        unsigned long long  YW1BA   :8;
        unsigned long long          :8;
    }bit;
};

/*  structure of LBLTMR (2890_0468h)    */
union io_jdslch_reg_lbltmr{
    unsigned long       word;
    struct {
        unsigned long   BTIMH   :8;
        unsigned long   BTIML   :8;
        unsigned long   BTIMD   :1;
        unsigned long           :15;
    }bit;
};

/*  structure of LBOST  (2890_046Ch)    */
union io_jdslch_reg_lbost{
    unsigned long       word;
    struct {
        unsigned long   BOMD    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of LZBWID (2890_0470h)    */
union io_jdslch_reg_lzbwid{
    unsigned long       word;
    struct {
        unsigned long   ZBHH    :8;
        unsigned long   ZBHL    :8;
        unsigned long           :16;
    }bit;
};

/*  structure of LZBV   (2890_0474h)    */
union io_jdslch_reg_lzbv{
    unsigned long       word;
    struct {
        unsigned long   ZBVSTA  :1;
        unsigned long           :3;
        unsigned long           :4;
        unsigned long   ZBVCYC  :1;
        unsigned long           :23;
    }bit;
};

/*  structure of LZBPT  (2890_0478h)    */
union io_jdslch_reg_lzbpt{
    unsigned long       word;
    struct {
        unsigned long   ZBDIR   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of LY2R0/1/2  (2890_04A0h)    */
union io_jdslch_reg_ly2r{
    unsigned long long      dword;
    struct {
        unsigned long long  YR0     :16;
        unsigned long long  YR1     :16;
        unsigned long long  YR2     :16;
        unsigned long long          :16;
    }bit;
};

/*  structure of LTBLASET   (2890_04C0h)    */
union io_jdslch_reg_ltblaset{
    unsigned long       word;
    struct {
        unsigned long   IGTAEN  :1;
        unsigned long   IGEN    :1;
        unsigned long   IGTSL   :1;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long           :2;
        unsigned long   GTAEN   :1;
        unsigned long   GMEN    :1;
        unsigned long   GMTSL   :1;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   IGTMN   :1;
        unsigned long           :3;
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   GMTMN   :1;
        unsigned long           :7;
    }bit;
};

/*  structure of LCC0-2   (2890_04D0h)    */
union io_jdslch_reg_lcc{
    unsigned long long      dword;
    struct {
        unsigned long long  CC0     :12;
        unsigned long long          :4;
        unsigned long long  CC1     :12;
        unsigned long long          :4;
        unsigned long long  CC2     :12;
        unsigned long long          :20;
    }bit;
};

/*  structure of LGMMD  (2890_04F0h)    */
union io_jdslch_reg_lgmmd{
    unsigned long       word;
    struct {
        unsigned long   GMMD    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of LALP   (2890_0500h)    */
union io_jdslch_reg_lalp{
    unsigned long       word;
    struct {
        unsigned long   ALP :8;
        unsigned long       :24;
    }bit;
};

/*  structure of LDSTA  (2890_0600h)    */
union io_jdslch_reg_ldsta{
    unsigned long       word;
    struct {
        unsigned long   DSH :16;
        unsigned long   DSV :14;
        unsigned long       :2;
    }bit;
};

/*  structure of LREVDISP   (2890_0604h)    */
union io_jdslch_reg_lrevdisp{
    unsigned long       word;
    struct {
        unsigned long   HREV    :1;
        unsigned long           :3;
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   VREV    :1;
        unsigned long           :23;
    }bit;
};

/* Define i/o mapping */
struct io_jdslch_reg{
    /* JDSIMG */
    union  io_jdslch_reg_lrst       LRST;       /* 2890_(0000 - 0003h) */

    unsigned char dmy_0004_00FF[0x0100-0x0004]; /* 2890_(0004 - 00FFh) */

    union  io_jdslch_reg_ltrg       LTRG;       /* 2890_(0100 - 0103h) */

    unsigned char dmy_0104_010F[0x0110-0x0104]; /* 2890_(0104 - 010Fh) */

    union  io_jdslch_reg_lrpgctl    LRPGCTL;    /* 2890_(0110 - 0113h) */

    unsigned char dmy_0114_01FF[0x0200-0x0114]; /* 2890_(0114 - 01FFh) */

    union  io_jdslch_reg_lidt0      LIDT0;      /* 2890_(0200 - 0203h) */
    union  io_jdslch_reg_lidt1      LIDT1;      /* 2890_(0204 - 0207h) */
    union  io_jdslch_reg_lisize     LISIZE;     /* 2890_(0208 - 020Bh) */

    unsigned char dmy_020C_020F[0x0210-0x020C]; /* 2890_(020C - 020Fh) */

    union  io_jdslch_reg_lysa       LYSA[4];    /* 2890_(0210 - 021Fh) */
    union  io_jdslch_reg_lcsa       LCSA[4];    /* 2890_(0220 - 022Fh) */

    unsigned char dmy_0230_023F[0x0240-0x0230]; /* 2890_(0230 - 023Fh) */

    union  io_jdslch_reg_lyhga      LYHGA;      /* 2890_(0240 - 0243h) */

    unsigned char dmy_0244_024F[0x0250-0x0244]; /* 2890_(0244 - 024Fh) */

    union  io_jdslch_reg_lchga      LCHGA;      /* 2890_(0250 - 0253h) */

    unsigned char dmy_0254_025F[0x0260-0x0254]; /* 2890_(0254 - 025Fh) */

    union  io_jdslch_reg_libctl     LIBCTL;     /* 2890_(0260 - 0263h) */

    unsigned char dmy_0264_03FF[0x0400-0x0264]; /* 2890_(0264 - 03FFh) */

    union  io_jdslch_reg_lrsz0      LRSZ0;      /* 2890_(0400 - 0403h) */
    union  io_jdslch_reg_lrsz1      LRSZ1;      /* 2890_(0404 - 0407h) */
    union  io_jdslch_reg_lrsz2      LRSZ2;      /* 2890_(0408 - 040Bh) */
    union  io_jdslch_reg_lrsz3      LRSZ3;      /* 2890_(040C - 040Fh) */
    union  io_jdslch_reg_lyhlpctl   LYHLPCTL;   /* 2890_(0410 - 0413h) */
    union  io_jdslch_reg_lyhlpk0    LYHLPK0;    /* 2890_(0414 - 0417h) */
    union  io_jdslch_reg_lyhlpk1    LYHLPK1;    /* 2890_(0418 - 041Bh) */
    union  io_jdslch_reg_lyhlpol    LYHLPOL;    /* 2890_(041C - 041Fh) */
    union  io_jdslch_reg_lyhlpclr   LYHLPCLR;   /* 2890_(0420 - 0427h) */

    unsigned char dmy_0428_042F[0x0430-0x0428]; /* 2890_(0428 - 042Fh) */

    union  io_jdslch_reg_lywctl     LYWCTL;     /* 2890_(0430 - 0433h) */
    union  io_jdslch_reg_lyw0th     LYW0TH;     /* 2890_(0434 - 0437h) */
    union  io_jdslch_reg_lyw1th     LYW1TH;     /* 2890_(0438 - 043Bh) */
    union  io_jdslch_reg_lyw0st     LYW0ST;     /* 2890_(043C - 043Fh) */
    union  io_jdslch_reg_lyw0ca     LYW0CA;     /* 2890_(0440 - 0447h) */
    union  io_jdslch_reg_lyw0cb     LYW0CB;     /* 2890_(0448 - 044Fh) */
    union  io_jdslch_reg_lyw1st     LYW1ST;     /* 2890_(0450 - 0453h) */

    unsigned char dmy_0454_0457[0x0458-0x0454]; /* 2890_(0454 - 0457h) */

    union  io_jdslch_reg_lyw1ca     LYW1CA;     /* 2890_(0458 - 045Fh) */
    union  io_jdslch_reg_lyw1cb     LYW1CB;     /* 2890_(0460 - 0467h) */
    union  io_jdslch_reg_lbltmr     LBLTMR;     /* 2890_(0468 - 046Bh) */
    union  io_jdslch_reg_lbost      LBOST;      /* 2890_(046C - 046Fh) */
    union  io_jdslch_reg_lzbwid     LZBWID;     /* 2890_(0470 - 0473h) */
    union  io_jdslch_reg_lzbv       LZBV;       /* 2890_(0474 - 0477h) */
    union  io_jdslch_reg_lzbpt      LZBPT;      /* 2890_(0478 - 047Bh) */

    unsigned char dmy_047C_049F[0x04A0-0x047C]; /* 2890_(047C - 049Fh) */

    union  io_jdslch_reg_ly2r       LY2R[3];    /* 2890_(04A0 - 04B7h) */

    unsigned char dmy_04B8_04BF[0x04C0-0x04B8]; /* 2890_(04B8 - 04BFh) */

    union  io_jdslch_reg_ltblaset   LTBLASET;   /* 2890_(04C0 - 04C3h) */

    unsigned char dmy_04C4_04CF[0x04D0-0x04C4]; /* 2890_(04C4 - 04CFh) */

    union  io_jdslch_reg_lcc        LCC[3];       /* 2890_(04D0 - 04E7h) */

    unsigned char dmy_04E8_04EF[0x04F0-0x04E8]; /* 2890_(04E8 - 04EFh) */

    union  io_jdslch_reg_lgmmd      LGMMD;      /* 2890_(04F0 - 04F3h) */

    unsigned char dmy_04F4_04FF[0x0500-0x04F4]; /* 2890_(04F4 - 04FFh) */

    union  io_jdslch_reg_lalp       LALP;       /* 2890_(0500 - 0503h) */

    unsigned char dmy_0504_05FF[0x0600-0x0504]; /* 2890_(0504 - 05FFh) */

    union  io_jdslch_reg_ldsta      LDSTA;      /* 2890_(0600 - 0603h) */
    union  io_jdslch_reg_lrevdisp   LREVDISP;   /* 2890_(0604 - 0607h) */

    unsigned char dmy_0608_0FFF[0x1000-0x0608]; /* 2890_(0608 - 0FFFh) */

};

