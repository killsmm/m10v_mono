/**
 * @file		__jdsdisp_grch.h
 * @brief		Definition JDSDISP3A GRCH Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*  structure of GRRST  (2890_2000h)    */
union io_jdsgrch_reg_grrst{
    unsigned long       word;
    struct {
        unsigned long   SR  :1;
        unsigned long       :31;
    }bit;
};

/*  structure of GRTRG  (2890_2100h)    */
union io_jdsgrch_reg_grtrg{
    unsigned long       word;
    struct {
        unsigned long   TRG :2;
        unsigned long       :30;
    }bit;
};

/*  structure of GRRPGCTL   (2890_2110h)    */
union io_jdsgrch_reg_grrpgctl{
    unsigned long       word;
    struct {
        unsigned long   RPGTMG  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of GRIDT  (2890_2200h)    */
union io_jdsgrch_reg_gridt{
    unsigned long       word;
    struct {
        unsigned long   IFMT    :3;
        unsigned long           :1;
        unsigned long   NBT     :2;
        unsigned long           :2;
        unsigned long           :8;
        unsigned long   CACHE   :4;
        unsigned long   PROT    :3;
        unsigned long           :1;
        unsigned long   SLVSL   :1;
        unsigned long   ASLVSL  :1;
        unsigned long           :2;
        unsigned long   IFBTMU  :1;
        unsigned long           :3;
    }bit;
};

/*  structure of GRTISIZE   (2890_2204h)    */
union io_jdsgrch_reg_grtisize{
    unsigned long       word;
    struct {
        unsigned long   TIHSIZE :16;
        unsigned long   TIVSIZE :14;
        unsigned long           :2;
    }bit;
};

/*  structure of GRTDSTA    (2890_2208h)    */
union io_jdsgrch_reg_grtdsta{
    unsigned long       word;
    struct {
        unsigned long   TDSH    :16;
        unsigned long   TDSV    :14;
        unsigned long           :2;
    }bit;
};

/*  structure of GRIPO  (2890_2210h)    */
union io_jdsgrch_reg_gripo{
    unsigned long       word;
    struct {
        unsigned long   IPO1    :2;
        unsigned long           :2;
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   IPO2    :2;
        unsigned long           :2;
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   IPO3    :2;
        unsigned long           :2;
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   IPO4    :2;
        unsigned long           :6;
    }bit;
};

/*  structure of GRSCCTL    (2890_2214h)    */
union io_jdsgrch_reg_grscctl{
    unsigned long       word;
    struct {
        unsigned long   SCEN    :2;
        unsigned long           :2;
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   IDSET   :2;
        unsigned long           :2;
        unsigned long           :1;
        unsigned long           :3;
        unsigned long   IDM     :2;
        unsigned long           :14;
    }bit;
};

/*  structure of GRERCV (2890_2218h)    */
union io_jdsgrch_reg_grercv{
    unsigned long       word;
    struct {
        unsigned long   ERCV    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of GRISIZE    (2890_2400h)    */
union io_jdsgrch_reg_grisize{
    unsigned long       word;
    struct {
        unsigned long   IHSIZE      :16;
        unsigned long   IVSIZE      :14;
        unsigned long               :2;
    }bit;
};

/*  structure of GRSA0  (2890_2440h)    */
union io_jdsgrch_reg_grsa0{
    unsigned long       word;
    struct {
        unsigned long   SA0   :32;
    }bit;
};

/*  structure of GRSA   (2890_2480h)    */
union io_jdsgrch_reg_grsa{
    unsigned long       word;
    struct {
        unsigned long   SA      :32;
    }bit;
};

/*  structure of GRASA  (2890_24C0h)    */
union io_jdsgrch_reg_grasa{
    unsigned long       word;
    struct {
        unsigned long   ASA     :32;
    }bit;
};

/*  structure of GRHGA  (2890_2500h)    */
union io_jdsgrch_reg_grhga{
    unsigned long       word;
    struct {
        unsigned long   HGA     :16;
        unsigned long           :16;
    }bit;
};

/*  structure of GRAHGA (2890_2540h)    */
union io_jdsgrch_reg_grahga{
    unsigned long       word;
    struct {
        unsigned long   AHGA    :16;
        unsigned long           :16;
    }bit;
};

/*  structure of GRDSTA (2890_2580h)    */
union io_jdsgrch_reg_grdsta{
    unsigned long       word;
    struct {
        unsigned long   DSH     :16;
        unsigned long   DSV     :14;
        unsigned long           :2;
    }bit;
};

/*  structure of GRAREN (2890_25C0h)    */
union io_jdsgrch_reg_graren{
    unsigned long       word;
    struct {
        unsigned long   AREN    :10;
        unsigned long           :22;
    }bit;
};

/*  structure of GRBSL  (2890_25C4h)    */
union io_jdsgrch_reg_grbsl{
    unsigned long       word;
    struct {
        unsigned long   BSL :10;
        unsigned long       :22;
    }bit;
};

/*  structure of GRBLINK    (2890_25E0h)    */
union io_jdsgrch_reg_grblink{
    unsigned long       word[5];
    struct {
        unsigned long   BTIMH_0 :6;
        unsigned long           :2;
        unsigned long   BTIML_0 :6;
        unsigned long           :2;
        unsigned long   BTIMH_1 :6;
        unsigned long           :2;
        unsigned long   BTIML_1 :6;
        unsigned long           :2;
        unsigned long   BTIMH_2 :6;
        unsigned long           :2;
        unsigned long   BTIML_2 :6;
        unsigned long           :2;
        unsigned long   BTIMH_3 :6;
        unsigned long           :2;
        unsigned long   BTIML_3 :6;
        unsigned long           :2;
        unsigned long   BTIMH_4 :6;
        unsigned long           :2;
        unsigned long   BTIML_4 :6;
        unsigned long           :2;
        unsigned long   BTIMH_5 :6;
        unsigned long           :2;
        unsigned long   BTIML_5 :6;
        unsigned long           :2;
        unsigned long   BTIMH_6 :6;
        unsigned long           :2;
        unsigned long   BTIML_6 :6;
        unsigned long           :2;
        unsigned long   BTIMH_7 :6;
        unsigned long           :2;
        unsigned long   BTIML_7 :6;
        unsigned long           :2;
        unsigned long   BTIMH_8 :6;
        unsigned long           :2;
        unsigned long   BTIML_8 :6;
        unsigned long           :2;
        unsigned long   BTIMH_9 :6;
        unsigned long           :2;
        unsigned long   BTIML_9 :6;
        unsigned long           :2;
    }bit;
};

/*  structure of GRBINIT    (2890_2600h)    */
union io_jdsgrch_reg_grbinit{
    unsigned long       word;
    struct {
        unsigned long   BINIT   :10;
        unsigned long           :22;
    }bit;
};

/*  structure of GRBITRG    (2890_2604h)    */
union io_jdsgrch_reg_grbitrg{
    unsigned long       word;
    struct {
        unsigned long   BITRG   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of GRRSZ0 (2890_2810h)    */
union io_jdsgrch_reg_grrsz0{
    unsigned long       word;
    struct {
        unsigned long   RSZSL   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of GRRSZ1 (2890_2814h)    */
union io_jdsgrch_reg_grrsz1{
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

/*  structure of GRRSZ2 (2890_2818h)    */
union io_jdsgrch_reg_grrsz2{
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

/*  structure of GRRSZ3 (2890_281Ch)    */
union io_jdsgrch_reg_grrsz3{
    unsigned long       word;
    struct {
        unsigned long   HCSTA   :6;
        unsigned long           :2;
        unsigned long   VCSTA   :6;
        unsigned long           :18;
    }bit;
};

/*  structure of GRY2R0 - GRY2R2 (2890_2840 h)    */
union io_jdsgrch_reg_gry2r{
    unsigned long       word;
    struct {
        unsigned long   YR0     :8;
        unsigned long   YR1     :8;
        unsigned long   YR2     :8;
        unsigned long           :8;
    }bit;
};

/*  structure of GRALP  (2890_2880h)    */
union io_jdsgrch_reg_gralp{
    unsigned long       word;
    struct {
        unsigned long   ALP :8;
        unsigned long       :24;
    }bit;
};

/* Define i/o mapping */
struct io_jdsgrch_reg{
    /* JDSIMG */
    union  io_jdsgrch_reg_grrst     GRRST;      /* 2890_(2000 - 2003h) */

    unsigned char dmy_2004_20FF[0x2100-0x2004]; /* 2890_(2004 - 20FFh) */

    union  io_jdsgrch_reg_grtrg     GRTRG;      /* 2890_(2100 - 2103h) */

    unsigned char dmy_2104_210F[0x2110-0x2104]; /* 2890_(2104 - 210Fh) */

    union  io_jdsgrch_reg_grrpgctl  GRRPGCTL;   /* 2890_(2110 - 2113h) */

    unsigned char dmy_2114_21FF[0x2200-0x2114]; /* 2890_(2114 - 21FFh) */

    union  io_jdsgrch_reg_gridt     GRIDT;      /* 2890_(2200 - 2203h) */
    union  io_jdsgrch_reg_grtisize  GRTISIZE;   /* 2890_(2204 - 2207h) */
    union  io_jdsgrch_reg_grtdsta   GRTDSTA;    /* 2890_(2208 - 220Bh) */

    unsigned char dmy_220C_220F[0x2210-0x220C]; /* 2890_(220C - 220Fh) */

    union  io_jdsgrch_reg_gripo     GRIPO;      /* 2890_(2210 - 2213h) */
    union  io_jdsgrch_reg_grscctl   GRSCCTL;    /* 2890_(2214 - 2217h) */
    union  io_jdsgrch_reg_grercv    GRERCV;     /* 2890_(2218 - 221Bh) */

    unsigned char dmy_221C_23FF[0x2400-0x221C]; /* 2890_(221C - 23FFh) */

    union  io_jdsgrch_reg_grisize   GRISIZE[10]; /* 2890_(2400 - 2427h) */

    unsigned char dmy_2428_243F[0x2440-0x2428]; /* 2890_(2428 - 243Fh) */

    union  io_jdsgrch_reg_grsa0     GRSA0[4];   /* 2890_(2440 - 244Fh) */

    unsigned char dmy_2450_247F[0x2480-0x2450]; /* 2890_(2450 - 247Fh) */

    union  io_jdsgrch_reg_grsa      GRSA[9];    /* 2890_(2480 - 24A3h) */

    unsigned char dmy_24A4_24BF[0x24C0-0x24A4]; /* 2890_(24A4 - 24BFh) */

    union  io_jdsgrch_reg_grasa     GRASA[10];  /* 2890_(24C0 - 24E7h) */

    unsigned char dmy_24E8_24FF[0x2500-0x24E8]; /* 2890_(24E8 - 24FFh) */

    union  io_jdsgrch_reg_grhga     GRHGA[10];  /* 2890_(2500 - 2527h) */

    unsigned char dmy_2528_253F[0x2540-0x2528]; /* 2890_(2528 - 253Fh) */

    union  io_jdsgrch_reg_grahga    GRAHGA[10]; /* 2890_(2540 - 2567h) */

    unsigned char dmy_2568_257F[0x2580-0x2568]; /* 2890_(2568 - 257Fh) */

    union  io_jdsgrch_reg_grdsta    GRDSTA[10]; /* 2890_(2580 - 25A7h) */

    unsigned char dmy_25A8_25BF[0x25C0-0x25A8]; /* 2890_(25A8 - 25BFh) */

    union  io_jdsgrch_reg_graren    GRAREN;     /* 2890_(25C0 - 25C3h) */
    union  io_jdsgrch_reg_grbsl     GRBSL;      /* 2890_(25C4 - 25C7h) */

    unsigned char dmy_25C8_25DF[0x25E0-0x25C8]; /* 2890_(25C8 - 25DFh) */

    union  io_jdsgrch_reg_grblink   GRBLINK;    /* 2890_(25E0 - 25F3h) */

    unsigned char dmy_25F4_25FF[0x2600-0x25F4]; /* 2890_(25F4 - 25FFh) */

    union  io_jdsgrch_reg_grbinit   GRBINIT;    /* 2890_(2600 - 2603h) */
    union  io_jdsgrch_reg_grbitrg   GRBITRG;    /* 2890_(2604 - 2607h) */

    unsigned char dmy_2608_280F[0x2810-0x2608]; /* 2890_(2608 - 280Fh) */

    union  io_jdsgrch_reg_grrsz0    GRRSZ0;     /* 2890_(2810 - 2813h) */
    union  io_jdsgrch_reg_grrsz1    GRRSZ1;     /* 2890_(2814 - 2817h) */
    union  io_jdsgrch_reg_grrsz2    GRRSZ2;     /* 2890_(2818 - 281Bh) */
    union  io_jdsgrch_reg_grrsz3    GRRSZ3;     /* 2890_(281C - 281Fh) */

    unsigned char dmy_2820_283F[0x2840-0x2820]; /* 2890_(2820 - 283Fh) */

    union  io_jdsgrch_reg_gry2r     GRY2R[3];   /* 2890_(2840 - 284Bh) */

    unsigned char dmy_284C_287F[0x2880-0x284C]; /* 2890_(284C - 287Fh) */

    union  io_jdsgrch_reg_gralp     GRALP;      /* 2890_(2880 - 2883h) */

    unsigned char dmy_2884_2FFF[0x3000-0x2884]; /* 2890_(2884 - 2FFFh) */

};

