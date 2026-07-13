/**
 * @file		__jdspro_fshdl.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_FSHDL_H_
#define _IO_PRO_FSHDL_H_

/*  structure of FFSHTRG    (2840_9300h)    */
union io_fshdltrg{
    unsigned long       word;
    struct {
        unsigned long   FFSHTRG :2;
        unsigned long           :30;
    }bit;
};

/*  structure of FFSHPAEN   (2840_9304h)    */
union io_fshdlpaen{
    unsigned long       word;
    struct {
        unsigned long   FFSHPAEN    :1;
        unsigned long               :31;
    }bit;
};

/*  structure of FFSHV  (2840_930Ch)    */
union io_fshdlv{
    unsigned long       word;
    struct {
        unsigned long   FFSHV   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FFSHH  (2840_9310h)    */
union io_fshdlh{
    unsigned long       word;
    struct {
        unsigned long   FFSHH   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FFSHVW (2840_9314h)    */
union io_fshdlvw{
    unsigned long       word;
    struct {
        unsigned long   FFSHVW  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FFSHHW (2840_9318h)    */
union io_fshdlhw{
    unsigned long       word;
    struct {
        unsigned long   FFSHHW  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of FFSHBC (2840_931Ch)    */
union io_fshdlbc{
    unsigned long       word;
    struct {
        long            FFSHBC0 :10;
        unsigned long           :6;
        long            FFSHBC1 :10;
        unsigned long           :6;
    }bit;
};

/*  structure of FFSHUL1    (2840_9320h)    */
union io_fshdlul1{
    unsigned long       word;
    struct {
        unsigned long   FFSHULRR    :14;
        unsigned long               :2;
        unsigned long   FFSHULGR    :14;
        unsigned long               :2;
    }bit;
};

/*  structure of FFSHUL2    (2840_9324h)    */
union io_fshdlul2{
    unsigned long       word;
    struct {
        unsigned long   FFSHULGB    :14;
        unsigned long               :2;
        unsigned long   FFSHULBB    :14;
        unsigned long               :2;
    }bit;
};

/*  structure of FFSHGFMT   (2840_9328h)    */
union io_fshdlgfmt{
    unsigned long       word;
    struct {
        unsigned long   FFSHGFMT    :3;
        unsigned long               :29;
    }bit;
};

/*  structure of FFSHSUBPR  (2840_9330h)    */
union io_fshdlsubpr{
    unsigned long       word;
    struct {
        unsigned long   FFSHSUBPRH  :11;
        unsigned long               :5;
        unsigned long   FFSHSUBPRV  :11;
        unsigned long               :5;
    }bit;
};

/*  structure of FFSHSUBSP  (2840_9334h)    */
union io_fshdlsubsp{
    unsigned long       word;
    struct {
        unsigned long   FFSHSUBSPH  :10;
        unsigned long               :6;
        unsigned long   FFSHSUBSPV  :10;
        unsigned long               :6;
    }bit;
};

/*  structure of FFSHSUBDR  (2840_9338h)    */
union io_fshdlsubdr{
    unsigned long       word;
    struct {
        unsigned long   FFSHSUBDRM  :16;
        unsigned long   FFSHSUBDRE  :4;
        unsigned long               :12;
    }bit;
};

/*  structure of FFSHSUBST  (2840_933Ch)    */
union io_fshdlsubst{
    unsigned long       word;
    struct {
        unsigned long   FFSHSUBSTH  :10;
        unsigned long               :6;
        unsigned long   FFSHSUBSTV  :10;
        unsigned long               :6;
    }bit;
};

/*  structure of FFSHCTL    (2840_9340h)    */
union io_fshdlctl{
    unsigned long       word;
    struct {
        unsigned long   FFSHPTMD    :1;
        unsigned long               :31;
    }bit;
};

/*  structure of FFSHHWPH   (2840_9344h)    */
union io_fshdlhwph{
    unsigned long       word;
    struct {
        unsigned long   FFSHHWPH    :14;
        unsigned long               :18;
    }bit;
};

/*  structure of FFSHCCTL   (2840_9350h)    */
union io_fshdlcctl{
    unsigned long       word;
    struct {
        unsigned long   FFSHCEN :1;
        unsigned long           :31;
    }bit;
};

/*  structure of FFSHLP1    (2840_9354h)    */
union io_fshdllp1{
    unsigned long       word;
    struct {
        unsigned long   FFSHLPRR    :12;
        unsigned long               :4;
        unsigned long   FFSHLPGR    :12;
        unsigned long               :4;
    }bit;
};

/*  structure of FFSHLP2    (2840_9358h)    */
union io_fshdllp2{
    unsigned long       word;
    struct {
        unsigned long   FFSHLPGB    :12;
        unsigned long               :4;
        unsigned long   FFSHLPBB    :12;
        unsigned long               :4;
    }bit;
};

/*  structure of FFSHWRIC   (2840_9380h)    */
union io_fshdlwric{
    unsigned long       word;
    struct {
        unsigned long   FFSHWRIC    :13;
        unsigned long               :19;
    }bit;
};

/* Define i/o mapping */
struct io_fshdl{
    /* JDSIMG */
    union  io_fshdltrg      FFSHTRG;    		/* 2840_(9300 - 9303h) */
    union  io_fshdlpaen     FFSHPAEN;   		/* 2840_(9304 - 9307h) */

    unsigned char dmy_9308_930B[0x930C-0x9308]; /* 2840_(9308 - 930Bh) */

    union  io_fshdlv        FFSHV;      		/* 2840_(930C - 930Fh) */
    union  io_fshdlh        FFSHH;      		/* 2840_(9310 - 9313h) */
    union  io_fshdlvw       FFSHVW;     		/* 2840_(9314 - 9317h) */
    union  io_fshdlhw       FFSHHW;     		/* 2840_(9318 - 931Bh) */
    union  io_fshdlbc       FFSHBC;     		/* 2840_(931C - 931Fh) */
    union  io_fshdlul1      FFSHUL1;    		/* 2840_(9320 - 9323h) */
    union  io_fshdlul2      FFSHUL2;    		/* 2840_(9324 - 9327h) */
    union  io_fshdlgfmt     FFSHGFMT;   		/* 2840_(9328 - 932Bh) */

    unsigned char dmy_932C_932F[0x9330-0x932C]; /* 2840_(932C - 932Fh) */

    union  io_fshdlsubpr    FFSHSUBPR;  		/* 2840_(9330 - 9333h) */
    union  io_fshdlsubsp    FFSHSUBSP;  		/* 2840_(9334 - 9337h) */
    union  io_fshdlsubdr    FFSHSUBDR;  		/* 2840_(9338 - 933Bh) */
    union  io_fshdlsubst    FFSHSUBST;  		/* 2840_(933C - 933Fh) */
    union  io_fshdlctl      FFSHCTL;    		/* 2840_(9340 - 9343h) */
    union  io_fshdlhwph     FFSHHWPH;   		/* 2840_(9344 - 9347h) */

    unsigned char dmy_9348_934F[0x9350-0x9348]; /* 2840_(9348 - 934Fh) */

    union  io_fshdlcctl     FFSHCCTL;   		/* 2840_(9350 - 9353h) */
    union  io_fshdllp1      FFSHLP1;    		/* 2840_(9354 - 9357h) */
    union  io_fshdllp2      FFSHLP2;    		/* 2840_(9358 - 935Bh) */

    unsigned char dmy_935C_937F[0x9380-0x935C]; /* 2840_(935C - 937Fh) */

    union  io_fshdlwric     FFSHWRIC;   		/* 2840_(9380 - 9383h) */

    unsigned char dmy_9384_93FF[0x9400-0x9384]; /* 2840_(9384 - 93FFh) */

};

#endif	// _IO_PRO_FSHDL_H_
