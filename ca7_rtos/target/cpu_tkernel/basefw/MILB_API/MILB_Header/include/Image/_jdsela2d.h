/**
 * @file		_jdsela2d.h
 * @brief		Definition JDSELA2D Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/*  structure of EARCACHE   (288B_0000h)    */
union io_ela_earcache{
    unsigned long       word;
    struct {
        unsigned long   EARCACHE_0  :4;
        unsigned long               :4;
        unsigned long   EARCACHE_1  :4;
        unsigned long               :4;
        unsigned long   EARCACHE_2  :4;
        unsigned long               :4;
        unsigned long               :8;
    }bit;
};

/*  structure of EAWCACHE   (288B_0004h)    */
union io_ela_eawcache{
    unsigned long       word;
    struct {
        unsigned long   EAWCACHE_0  :4;
        unsigned long               :4;
        unsigned long   EAWCACHE_1  :4;
        unsigned long               :4;
        unsigned long   EAWCACHE_2  :4;
        unsigned long               :4;
        unsigned long               :8;
    }bit;
};

/*  structure of EARPROT    (288B_0008h)    */
union io_ela_earprot{
    unsigned long       word;
    struct {
        unsigned long   EARPROT_0   :3;
        unsigned long               :5;
        unsigned long   EARPROT_1   :3;
        unsigned long               :5;
        unsigned long   EARPROT_2   :3;
        unsigned long               :5;
        unsigned long               :8;
    }bit;
};

/*  structure of EAWPROT    (288B_000Ch)    */
union io_ela_eawprot{
    unsigned long       word;
    struct {
        unsigned long   EAWPROT_0   :3;
        unsigned long               :5;
        unsigned long   EAWPROT_1   :3;
        unsigned long               :5;
        unsigned long   EAWPROT_2   :3;
        unsigned long               :5;
        unsigned long               :8;
    }bit;
};

/*  structure of EARAXSTS   (288B_0010h)    */
union io_ela_earaxsts{
    unsigned long       word;
    struct {
        unsigned long   RRESP_0 :2;
        unsigned long           :6;
        unsigned long   RRESP_1 :2;
        unsigned long           :6;
        unsigned long   RRESP_2 :2;
        unsigned long           :6;
        unsigned long           :8;
    }bit;
};

/*  structure of EAWAXSTS   (288B_0014h)    */
union io_ela_eawaxsts{
    unsigned long       word;
    struct {
        unsigned long   BRESP_0 :2;
        unsigned long           :6;
        unsigned long   BRESP_1 :2;
        unsigned long           :6;
        unsigned long   BRESP_2 :2;
        unsigned long           :6;
        unsigned long           :8;
    }bit;
};

/*  structure of EARREQMSK  (288B_0020h)    */
union io_ela_earreqmsk_1{
    unsigned long       word;
    struct {
        unsigned long   EARREQMSK_0 :12;
        unsigned long               :4;
        unsigned long   EARREQMSK_1 :12;
        unsigned long               :4;
    }bit;
};
union io_ela_earreqmsk_2{
    unsigned long       word;
    struct {
        unsigned long   EARREQMSK_2 :12;
        unsigned long               :4;
        unsigned long               :16;
    }bit;
};
struct io_ela_earreqmsk{
    union io_ela_earreqmsk_1    EARREQMSK1;
    union io_ela_earreqmsk_2    EARREQMSK2;
};

/*  structure of EAWREQMSK  (288B_0028h)    */
union io_ela_eawreqmsk_1{
    unsigned long       word;
    struct {
        unsigned long   EAWREQMSK_0 :12;
        unsigned long               :4;
        unsigned long   EAWREQMSK_1 :12;
        unsigned long               :4;
    }bit;
};
union io_ela_eawreqmsk_2{
    unsigned long       word;
    struct {
        unsigned long   EAWREQMSK_2 :12;
        unsigned long               :4;
        unsigned long               :16;
    }bit;
};
struct io_ela_eawreqmsk{
    union io_ela_eawreqmsk_1    EAWREQMSK1;
    union io_ela_eawreqmsk_2    EAWREQMSK2;
};

/*  structure of ELAINTE    (288B_0100h)    */
union io_ela_elainte{
    unsigned long       word;
    struct {
        unsigned long           :1;
        unsigned long   ENWE    :1;
        unsigned long           :2;
        unsigned long   AXRERE  :1;
        unsigned long   AXWERE  :1;
        unsigned long           :26;
    }bit;
};

/*  structure of ELAINTF    (288B_0104h)    */
union io_ela_elaintf{
    unsigned long       word;
    struct {
        unsigned long             :1;
        unsigned long   __ENWF    :1;
        unsigned long             :2;
        unsigned long   __AXRERF  :1;
        unsigned long   __AXWERF  :1;
        unsigned long             :26;
    }bit;
};

/*  structure of SRAMACT    (288B_0108h)    */
union io_ela_sramact{
    unsigned long       word;
    struct {
        unsigned long   SRAMACT :2;
        unsigned long           :30;
    }bit;
};

/*  structure of ELATRG (288B_0110h)    */
union io_ela_elatrg{
    unsigned long       word;
    struct {
        unsigned long   ELATRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of ELACTL (288B_0114h)    */
union io_ela_elactl{
    unsigned long       word;
    struct {
        unsigned long   ELACNT  :3;
        unsigned long           :1;
        unsigned long   ELAMD   :2;
        unsigned long           :1;
        unsigned long   ELAACT  :1;
        unsigned long           :24;
    }bit;
};

/*  structure of ELABYR (288B_0118h)    */
union io_ela_elabyr{
    unsigned long       word;
    struct {
        unsigned long   EIRTYP  :2;
        unsigned long           :2;
        unsigned long   EIRSFT  :2;
        unsigned long           :2;
        unsigned long   EIWSFT  :2;
        unsigned long           :2;
        unsigned long   EIWTYP  :2;
        unsigned long           :2;
        unsigned long   EIWOUT  :2;
        unsigned long           :2;
        unsigned long   DKNEN   :1;
        unsigned long   EOBAEN  :1;
        unsigned long           :2;
        unsigned long           :4;
        unsigned long   DKNACT  :1;
        unsigned long           :3;
    }bit;
};

/*  structure of NSLMD  (288B_011Ch)    */
union io_ela_nslmd{
    unsigned long       word;
    struct {
        unsigned long   NSLMD   :1;
        unsigned long           :3;
        unsigned long   NSLRES  :2;
        unsigned long           :2;
        unsigned long   NSLKNE  :1;
        unsigned long           :23;
    }bit;
};

/*  structure of OBOF   (288B_0120h)    */
union io_ela_obof_1{
    unsigned long       word;
    struct {
                 long   EOBRR   :11;
        unsigned long           :5;
                 long   EOBBB   :11;
        unsigned long           :5;
    }bit;
};
union io_ela_obof_2{
    unsigned long       word;
    struct {
                  long  EOBGR   :11;
        unsigned  long          :5;
                  long  EOBGB   :11;
        unsigned  long          :5;
    }bit;
};
struct io_ela_obof{
    union io_ela_obof_1    OBOF1;
    union io_ela_obof_2    OBOF2;
};


/*  structure of EZSFTA1    (288B_0128h)    */
union io_ela_ezsfta1{
    unsigned long       word;
    struct {
                 long   EZSFTA1 :11;
        unsigned long           :21;
    }bit;
};

/*  structure of NSLNGLMT   (288B_012Ch)    */
union io_ela_nslnglmt{
    unsigned long       word;
    struct {
        unsigned long   NSLNGLMM    :10;
        unsigned long               :6;
        unsigned long   NSLNGLMP    :10;
        unsigned long               :6;
    }bit;
};

/*  structure of EIRA   (288B_0130h)    */
union io_ela_eira{
    unsigned long       word;
    struct {
        unsigned long   EIRA    :32;
    }bit;
};

/*  structure of EIROFS (288B_0134h)    */
union io_ela_eirofs{
    unsigned long       word;
    struct {
        unsigned long   EIROFS  :3;
        unsigned long           :29;
    }bit;
};

/*  structure of EIRDEF (288B_0138h)    */
union io_ela_eirdef{
    unsigned long       word;
    struct {
        unsigned long   EIRDEF  :15;
        unsigned long           :17;
    }bit;
};

/*  structure of EIRHSIZ    (288B_013Ch)    */
union io_ela_eirhsiz{
    unsigned long       word;
    struct {
        unsigned long   EIRHSIZ :14;
        unsigned long           :18;
    }bit;
};

/*  structure of EIRVSIZ    (288B_0140h)    */
union io_ela_eirvsiz{
    unsigned long       word;
    struct {
        unsigned long   EIRVSIZ :14;
        unsigned long           :18;
    }bit;
};

/*  structure of EIWA   (288B_0150h)    */
union io_ela_eiwa{
    unsigned long       word[4];
    struct {
        unsigned long   EIWA    :32;
    }bit[4];
};

/*  structure of EIWDEF (288B_0160h)    */
union io_ela_eiwdef{
    unsigned long       word;
    struct {
        unsigned long   EIWDEF  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of ENWA   (288B_0164h)    */
union io_ela_enwa{
    unsigned long       word;
    struct {
        unsigned long   ENWA    :32;
    }bit;
};

/*  structure of ENWDEF (288B_0168h)    */
union io_ela_enwdef{
    unsigned long       word;
    struct {
        unsigned long   ENWDEF  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of ENRA   (288B_0180h)    */
union io_ela_enra{
    unsigned long       word[16];
    struct {
        unsigned long   ENRA    :32;
    }bit[16];
};

/*  structure of ENRHSIZ    (288B_01C0h)    */
union io_ela_enrhsiz{
    unsigned long       word;
    struct {
        unsigned long   ENRHSIZ :14;
        unsigned long           :18;
    }bit;
};

/*  structure of ENRVSIZ    (288B_01C4h)    */
union io_ela_enrvsiz{
    unsigned long       word;
    struct {
        unsigned long   ENRVSIZ :14;
        unsigned long           :18;
    }bit;
};

/*  structure of ENSA   (288B_01C8h)    */
union io_ela_ensa{
    unsigned long       word;
    struct {
        unsigned long   ENSA    :32;
    }bit;
};

/*  structure of ENSDEF (288B_01CCh)    */
union io_ela_ensdef{
    unsigned long       word;
    struct {
        unsigned long   ENSDEF  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of ENSHSIZ    (288B_01D0h)    */
union io_ela_enshsiz{
    unsigned long       word;
    struct {
        unsigned long   ENSHSIZ :14;
        unsigned long           :18;
    }bit;
};

/*  structure of ENSVSIZ    (288B_01D4h)    */
union io_ela_ensvsiz{
    unsigned long       word;
    struct {
        unsigned long   ENSVSIZ :14;
        unsigned long           :18;
    }bit;
};

/*  structure of BYRTYP (288B_0200h)    */
union io_ela_byrtyp{
    unsigned long       word;
    struct {
        unsigned long   ORG     :2;
        unsigned long           :2;
        unsigned long   ELADCMD :2;
        unsigned long           :26;
    }bit;
};

/*  structure of ELFZP  (288B_0204h)    */
union io_ela_elfzp{
    unsigned long       word;
    struct {
        unsigned long   ELFZP   :12;
        unsigned long           :20;
    }bit;
};

/*  structure of ELFL   (288B_0208h)    */
union io_ela_elfl_1{
    unsigned long       word;
    struct {
        unsigned long   ELFL_1  :12;
        unsigned long           :4;
        unsigned long   ELFL_2  :12;
        unsigned long           :4;
    }bit;
};
union io_ela_elfl_2{
    unsigned long       word;
    struct {
        unsigned long   ELFL_3   :12;
        unsigned long            :4;
        unsigned long   ELFL_4   :12;
        unsigned long            :4;
    }bit;
};
struct io_ela_elfl{
    union io_ela_elfl_1    ELFL1;
    union io_ela_elfl_2    ELFL2;
};

/*  structure of ANBOR  (288B_0210h)    */
union io_ela_anbor_1{
    unsigned long       word;
    struct {
        unsigned long   ANBOR_0 :12;
        unsigned long           :4;
        unsigned long   ANBOR_1 :12;
        unsigned long           :4;
    }bit;
};
union io_ela_anbor_2{
    unsigned long       word;
    struct {
        unsigned long   ANBOR_2 :12;
        unsigned long           :4;
        unsigned long   ANBOR_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_ela_anbor{
    union io_ela_anbor_1    ANBOR1;
    union io_ela_anbor_2    ANBOR2;
};

/*  structure of ANBKR  (288B_0218h)    */
union io_ela_anbkr_1{
    unsigned long       word;
    struct {
        unsigned long   ANBKR_0 :12;
        unsigned long           :4;
        unsigned long   ANBKR_1 :12;
        unsigned long           :4;
    }bit;
};
union io_ela_anbkr_2{
    unsigned long       word;
    struct {
        unsigned long   ANBKR_2 :12;
        unsigned long           :4;
        unsigned long   ANBKR_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_ela_anbkr{
    union io_ela_anbkr_1    ANBKR1;
    union io_ela_anbkr_2    ANBKR2;
};

/*  structure of ANBDR  (288B_0220h)    */
union io_ela_anbdr_1{
    unsigned long       word;
    struct {
        unsigned long   ANBDR_0 :12;
        unsigned long           :4;
        unsigned long   ANBDR_1 :12;
        unsigned long           :4;
    }bit;
};
union io_ela_anbdr_2{
    unsigned long       word;
    struct {
        unsigned long   ANBDR_2 :12;
        unsigned long           :4;
        unsigned long   ANBDR_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_ela_anbdr{
    union io_ela_anbdr_1    ANBDR1;
    union io_ela_anbdr_2    ANBDR2;
};

/*  structure of ANBOG  (288B_0228h)    */
union io_ela_anbog_1{
    unsigned long       word;
    struct {
        unsigned long   ANBOG_0 :12;
        unsigned long           :4;
        unsigned long   ANBOG_1 :12;
        unsigned long           :4;
    }bit;
};
union io_ela_anbog_2{
    unsigned long       word;
    struct {
        unsigned long   ANBOG_2 :12;
        unsigned long           :4;
        unsigned long   ANBOG_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_ela_anbog{
    union io_ela_anbog_1    ANBOG1;
    union io_ela_anbog_2    ANBOG2;
};

/*  structure of ANBKG  (288B_0230h)    */
union io_ela_anbkg_1{
    unsigned long       word;
    struct {
        unsigned long   ANBKG_0 :12;
        unsigned long           :4;
        unsigned long   ANBKG_1 :12;
        unsigned long           :4;
    }bit;
};
union io_ela_anbkg_2{
    unsigned long       word;
    struct {
        unsigned long   ANBKG_2 :12;
        unsigned long           :4;
        unsigned long   ANBKG_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_ela_anbkg{
    union io_ela_anbkg_1    ANBKG1;
    union io_ela_anbkg_2    ANBKG2;
};

/*  structure of ANBDG  (288B_0238h)    */
union io_ela_anbdg_1{
    unsigned long       word;
    struct {
        unsigned long   ANBDG_0 :12;
        unsigned long           :4;
        unsigned long   ANBDG_1 :12;
        unsigned long           :4;
    }bit;
};
union io_ela_anbdg_2{
    unsigned long       word;
    struct {
        unsigned long   ANBDG_2 :12;
        unsigned long           :4;
        unsigned long   ANBDG_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_ela_anbdg{
    union io_ela_anbdg_1    ANBDG1;
    union io_ela_anbdg_2    ANBDG2;
};

/*  structure of ANBOB  (288B_0240h)    */
union io_ela_anbob_1{
    unsigned long       word;
    struct {
        unsigned long   ANBOB_0 :12;
        unsigned long           :4;
        unsigned long   ANBOB_1 :12;
        unsigned long           :4;
    }bit;
};
union io_ela_anbob_2{
    unsigned long       word;
    struct {
        unsigned long   ANBOB_2 :12;
        unsigned long           :4;
        unsigned long   ANBOB_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_ela_anbob{
    union io_ela_anbob_1    ANBOB1;
    union io_ela_anbob_2    ANBOB2;
};

/*  structure of ANBKB  (288B_0248h)    */
union io_ela_anbkb_1{
    unsigned long       word;
    struct {
        unsigned long   ANBKB_0 :12;
        unsigned long           :4;
        unsigned long   ANBKB_1 :12;
        unsigned long           :4;
    }bit;
};
union io_ela_anbkb_2{
    unsigned long       word;
    struct {
        unsigned long   ANBKB_2 :12;
        unsigned long           :4;
        unsigned long   ANBKB_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_ela_anbkb{
    union io_ela_anbkb_1    ANBKB1;
    union io_ela_anbkb_2    ANBKB2;
};

/*  structure of ANBDB  (288B_0250h)    */
union io_ela_anbdb_1{
    unsigned long       word;
    struct {
        unsigned long   ANBDB_0 :12;
        unsigned long           :4;
        unsigned long   ANBDB_1 :12;
        unsigned long           :4;
    }bit;
};
union io_ela_anbdb_2{
    unsigned long       word;
    struct {
        unsigned long   ANBDB_2 :12;
        unsigned long           :4;
        unsigned long   ANBDB_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_ela_anbdb{
    union io_ela_anbdb_1    ANBDB1;
    union io_ela_anbdb_2    ANBDB2;
};

/*  structure of LGTW   (288B_0258h)    */
union io_ela_lgtw_1{
    unsigned long       word;
    struct {
        unsigned long   LGTWR   :10;
        unsigned long           :6;
        unsigned long   LGTWG   :10;
        unsigned long           :6;
    }bit;
};

union io_ela_lgtw_2{
    unsigned long       word;
    struct {
        unsigned long   LGTWB   :10;
        unsigned long           :22;
    }bit;
};
struct io_ela_lgtw{
    union io_ela_lgtw_1    LGTW1;
    union io_ela_lgtw_2    LGTW2;
};

/*  structure of LGT    (288B_0260h)    */
union io_ela_lgt{
    unsigned long       word;
    struct {
        unsigned long   LGTR    :5;
        unsigned long           :3;
        unsigned long   LGTG    :5;
        unsigned long           :3;
        unsigned long   LGTB    :5;
        unsigned long           :3;
        unsigned long   LGTC    :5;
        unsigned long           :3;
    }bit;
};

/*  structure of LGTE   (288B_0264h)    */
union io_ela_lgte{
    unsigned long       word;
    struct {
        unsigned long   LGTER   :5;
        unsigned long           :3;
        unsigned long   LGTEG   :5;
        unsigned long           :3;
        unsigned long   LGTEB   :5;
        unsigned long           :11;
    }bit;
};

/*  structure of EDG2ANBA   (288B_0268h)    */
union io_ela_edg2anba{
    unsigned long       word;
    struct {
        unsigned long   EDG2ANBA    :8;
        unsigned long               :24;
    }bit;
};

/*  structure of EDGSCLA    (288B_026Ch)    */
union io_ela_edgscla{
    unsigned long       word;
    struct {
        unsigned long   EDGSCLA :10;
        unsigned long           :22;
    }bit;
};

/*  structure of EDG2ANBB   (288B_0274h)    */
union io_ela_edg2anbb{
    unsigned long       word;
    struct {
        unsigned long   EDG2ANBB    :10;
        unsigned long               :22;
    }bit;
};

/*  structure of EDGSCLB    (288B_0278h)    */
union io_ela_edgsclb{
    unsigned long       word;
    struct {
        unsigned long   EDGSCLB :10;
        unsigned long           :22;
    }bit;
};

/*  structure of ALNGE  (288B_0280h)    */
union io_ela_alnge_1{
    unsigned long       word;
    struct {
        unsigned long   ALNGE_1  :12;
        unsigned long            :4;
        unsigned long   ALNGE_2  :12;
        unsigned long            :4;
    }bit;
};
union io_ela_alnge_2{
    unsigned long       word;
    struct {
        unsigned long   ALNGE_3  :12;
        unsigned long            :20;
    }bit;
};
struct io_ela_alnge{
    union io_ela_alnge_1    ALNGE1;
    union io_ela_alnge_2    ALNGE2;
};

/*  structure of ALNG   (288B_02A0h)    */
union io_ela_alng_1{
    unsigned long       word;
    struct {
        unsigned long   ALNGL1E1N   :9;
        unsigned long               :7;
        unsigned long   ALNGL1E2N   :9;
        unsigned long               :7;
    }bit;
};
union io_ela_alng_2{
    unsigned long       word;
    struct {
        unsigned long   ALNGL1E3N   :9;
        unsigned long               :7;
        unsigned long   ALNGL2E1N   :9;
        unsigned long               :7;
    }bit;
};
union io_ela_alng_3{
    unsigned long       word;
    struct {
        unsigned long   ALNGL2E2N   :9;
        unsigned long               :7;
        unsigned long   ALNGL2E3N   :9;
        unsigned long               :7;
    }bit;
};
union io_ela_alng_4{
    unsigned long       word;
    struct {
        unsigned long   ALNGL3E1N   :9;
        unsigned long               :7;
        unsigned long   ALNGL3E2N   :9;
        unsigned long               :7;
    }bit;
};
union io_ela_alng_5{
    unsigned long       word;
    struct {
        unsigned long   ALNGL3E3N   :9;
        unsigned long               :7;
        unsigned long   ALNGL4E1N   :9;
        unsigned long               :7;
    }bit;
};
union io_ela_alng_6{
    unsigned long       word;
    struct {
        unsigned long   ALNGL4E2N   :9;
        unsigned long               :7;
        unsigned long   ALNGL4E3N   :9;
        unsigned long               :7;
    }bit;
};
struct io_ela_alng{
    union io_ela_alng_1 ALNG1;
    union io_ela_alng_2 ALNG2;
    union io_ela_alng_3 ALNG3;
    union io_ela_alng_4 ALNG4;
    union io_ela_alng_5 ALNG5;
    union io_ela_alng_6 ALNG6;
};

/*  structure of LFTH   (288B_02C0h)    */
union io_ela_lfth{
    unsigned long       word;
    struct {
        unsigned long   LFTH    :12;
        unsigned long           :20;
    }bit;
};

/*  structure of LCOME  (288B_02C8h)    */
union io_ela_lcome{
    unsigned long       word;
    struct {
        unsigned long   LCOME1  :12;
        unsigned long           :4;
        unsigned long   LCOME2  :12;
        unsigned long           :4;
    }bit;
};

/*  structure of LCOMEN (288B_02CCh)    */
union io_ela_lcomen{
    unsigned long       word;
    struct {
        unsigned long   LCOME1N :9;
        unsigned long           :7;
        unsigned long   LCOME2N :9;
        unsigned long           :7;
    }bit;
};

/*  structure of DKNTBL (288B_0400h)    */
union io_ela_dkntbl{
    unsigned long       word[128];
    struct {
        unsigned long   DKNTBL_0    :12;
        unsigned long               :4;
        unsigned long   DKNTBL_1    :12;
        unsigned long               :4;
    }bit[128];
};

/* Define i/o mapping */
struct io_ela{
    /* JDSIMG */
    union  io_ela_earcache  EARCACHE;   /* 288B_(0000 - 1003h) */
    union  io_ela_eawcache  EAWCACHE;   /* 288B_(0004 - 1007h) */
    union  io_ela_earprot   EARPROT;    /* 288B_(0008 - 100Bh) */
    union  io_ela_eawprot   EAWPROT;    /* 288B_(000C - 100Fh) */
    union  io_ela_earaxsts  EARAXSTS;   /* 288B_(0010 - 1013h) */
    union  io_ela_eawaxsts  EAWAXSTS;   /* 288B_(0014 - 1017h) */

    unsigned char dmy_1018_101F[0x1020-0x1018]; /* 288B_(0018 - 101Fh) */

    struct io_ela_earreqmsk EARREQMSK;  /* 288B_(0020 - 1027h) */
    struct io_ela_eawreqmsk EAWREQMSK;  /* 288B_(0028 - 102Fh) */

    unsigned char dmy_1030_10FF[0x1100-0x1030]; /* 288B_(0030 - 10FFh) */

    union  io_ela_elainte   ELAINTE;    /* 288B_(0100 - 1103h) */
    union  io_ela_elaintf   ELAINTF;    /* 288B_(0104 - 1107h) */
    union  io_ela_sramact   SRAMACT;    /* 288B_(0108 - 110Bh) */

    unsigned char dmy_110C_110F[0x1110-0x110C]; /* 288B_(010C - 110Fh) */

    union  io_ela_elatrg    ELATRG;     /* 288B_(0110 - 1113h) */
    union  io_ela_elactl    ELACTL;     /* 288B_(0114 - 1117h) */
    union  io_ela_elabyr    ELABYR;     /* 288B_(0118 - 111Bh) */
    union  io_ela_nslmd     NSLMD;      /* 288B_(011C - 111Fh) */
    struct io_ela_obof      OBOF;       /* 288B_(0120 - 1127h) */
    union  io_ela_ezsfta1   EZSFTA1;    /* 288B_(0128 - 112Bh) */
    union  io_ela_nslnglmt  NSLNGLMT;   /* 288B_(012C - 112Fh) */
    union  io_ela_eira      EIRA;       /* 288B_(0130 - 1133h) */
    union  io_ela_eirofs    EIROFS;     /* 288B_(0134 - 1137h) */
    union  io_ela_eirdef    EIRDEF;     /* 288B_(0138 - 113Bh) */
    union  io_ela_eirhsiz   EIRHSIZ;    /* 288B_(013C - 113Fh) */
    union  io_ela_eirvsiz   EIRVSIZ;    /* 288B_(0140 - 1143h) */

    unsigned char dmy_1144_114F[0x1150-0x1144]; /* 288B_(0144 - 114Fh) */

    union  io_ela_eiwa      EIWA;       /* 288B_(0150 - 115Fh) */
    union  io_ela_eiwdef    EIWDEF;     /* 288B_(0160 - 1163h) */
    union  io_ela_enwa      ENWA;       /* 288B_(0164 - 1167h) */
    union  io_ela_enwdef    ENWDEF;     /* 288B_(0168 - 116Bh) */

    unsigned char dmy_116C_117F[0x1180-0x116C]; /* 288B_(016C - 117Fh) */

    union  io_ela_enra      ENRA;       /* 288B_(0180 - 11BFh) */
    union  io_ela_enrhsiz   ENRHSIZ;    /* 288B_(01C0 - 11C3h) */
    union  io_ela_enrvsiz   ENRVSIZ;    /* 288B_(01C4 - 11C7h) */
    union  io_ela_ensa      ENSA;       /* 288B_(01C8 - 11CBh) */
    union  io_ela_ensdef    ENSDEF;     /* 288B_(01CC - 11CFh) */
    union  io_ela_enshsiz   ENSHSIZ;    /* 288B_(01D0 - 11D3h) */
    union  io_ela_ensvsiz   ENSVSIZ;    /* 288B_(01D4 - 11D7h) */

    unsigned char dmy_11D8_11FF[0x1200-0x11D8]; /* 288B_(01D8 - 11FFh) */

    union  io_ela_byrtyp    BYRTYP;     /* 288B_(0200 - 1203h) */
    union  io_ela_elfzp     ELFZP;      /* 288B_(0204 - 1207h) */
    struct io_ela_elfl      ELFL;       /* 288B_(0208 - 120Fh) */
    struct io_ela_anbor     ANBOR;      /* 288B_(0210 - 1217h) */
    struct io_ela_anbkr     ANBKR;      /* 288B_(0218 - 121Fh) */
    struct io_ela_anbdr     ANBDR;      /* 288B_(0220 - 1227h) */
    struct io_ela_anbog     ANBOG;      /* 288B_(0228 - 122Fh) */
    struct io_ela_anbkg     ANBKG;      /* 288B_(0230 - 1237h) */
    struct io_ela_anbdg     ANBDG;      /* 288B_(0238 - 123Fh) */
    struct io_ela_anbob     ANBOB;      /* 288B_(0240 - 1247h) */
    struct io_ela_anbkb     ANBKB;      /* 288B_(0248 - 124Fh) */
    struct io_ela_anbdb     ANBDB;      /* 288B_(0250 - 1257h) */
    struct io_ela_lgtw      LGTW;       /* 288B_(0258 - 125Fh) */
    union  io_ela_lgt       LGT;        /* 288B_(0260 - 1263h) */
    union  io_ela_lgte      LGTE;       /* 288B_(0264 - 1267h) */
    union  io_ela_edg2anba  EDG2ANBA;   /* 288B_(0268 - 126Bh) */
    union  io_ela_edgscla   EDGSCLA;    /* 288B_(026C - 126Fh) */

    unsigned char dmy_1270_1273[0x1274-0x1270]; /* 288B_(0270 - 1273h) */

    union  io_ela_edg2anbb  EDG2ANBB;   /* 288B_(0274 - 1277h) */
    union  io_ela_edgsclb   EDGSCLB;    /* 288B_(0278 - 127Bh) */

    unsigned char dmy_127C_127F[0x1280-0x127C]; /* 288B_(027C - 127Fh) */

    struct io_ela_alnge     ALNGE;      /* 288B_(0280 - 1287h) */

    unsigned char dmy_1288_129F[0x12A0-0x1288]; /* 288B_(0288 - 129Fh) */

    struct io_ela_alng      ALNG;       /* 288B_(02A0 - 12B7h) */

    unsigned char dmy_12B8_12BF[0x12C0-0x12B8]; /* 288B_(02B8 - 12BFh) */

    union  io_ela_lfth      LFTH;       /* 288B_(02C0 - 12C3h) */

    unsigned char dmy_12C4_12C7[0x12C8-0x12C4]; /* 288B_(02C4 - 12C7h) */

    union  io_ela_lcome     LCOME;      /* 288B_(02C8 - 12CBh) */
    union  io_ela_lcomen    LCOMEN;     /* 288B_(02CC - 12CFh) */

    unsigned char dmy_12D0_13FF[0x1400-0x12D0]; /* 288B_(02D0 - 13FFh) */

    union  io_ela_dkntbl    DKNTBL;     /* 288B_(0400 - 15FFh) */

    unsigned char dmy_1600_1FFF[0x2000-0x1600]; /* 288B_(0600 - 1FFFh) */

};

extern volatile struct io_ela		IO_ELA;

