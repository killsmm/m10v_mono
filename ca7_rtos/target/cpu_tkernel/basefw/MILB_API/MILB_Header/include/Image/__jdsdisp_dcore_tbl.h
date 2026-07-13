/**
 * @file		__jdsdisp_dcore_tbl.h
 * @brief		Definition JDSDISP3A AHB DCORE Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*  structure of GTBLxy (298E_5000h) x=A/B, y=R/G/B    */
union io_jdsdcore_gtbl{
    unsigned short      hword[33];
    struct {
        unsigned short  GM      :8;
        unsigned short          :8;
    }bit[33];
};

/*  structure of GTBLAR ~ GTBLAB / GTBLBR ~ GTBLBB */
struct io_jdsdcore_gtbl_rgb{
    union  io_jdsdcore_gtbl     GTBLR;     /* 000 - 041h */

    unsigned char dmy_0042_007F[0x0080-0x0042];  /* 042 - 07Fh */

    union  io_jdsdcore_gtbl     GTBLG;     /* 080 - 0C1h */

    unsigned char dmy_00C2_00FF[0x0100-0x00C2];  /* 0C2 - 0FFh) */

    union  io_jdsdcore_gtbl     GTBLB;     /* 100 - 141h */

    unsigned char dmy_0142_01FF[0x0200-0x0142];  /* 142 - 1FFh */
};

/*  structure of YSTBLAYT (298E_6000h) / YSTBLACG (298E_6080h) / YSTBLBYT (298E_6100h) / YSTBLBCG (298E_6180h) */
union io_jdsdcore_ystbl{
    unsigned short      hword[33];
    struct {
        unsigned short  YSAT    :8;
        unsigned short          :8;
    }bit[33];
};

/* Define i/o mapping */
struct io_jdsdcore_tbl{
    /* JDSIMG */

    unsigned char dmy_4000_4FFF[0x5000-0x4000]; /* 298E_(4000 - 4FFFh) */

    struct io_jdsdcore_gtbl_rgb     GTBLA;      /* 298E_(5000 - 51FFh) */

    struct io_jdsdcore_gtbl_rgb     GTBLB;      /* 298E_(5200 - 53FFh) */

    unsigned char dmy_5342_5FFF[0x6000-0x5400]; /* 298E_(5400 - 5FFFh) */

    union  io_jdsdcore_ystbl        YSTBLAYT;   /* 298E_(6000 - 6041h) */

    unsigned char dmy_6042_607F[0x6080-0x6042]; /* 298E_(6042 - 607Fh) */

    union  io_jdsdcore_ystbl        YSTBLACG;   /* 298E_(6080 - 60C1h) */

    unsigned char dmy_60C2_60FF[0x6100-0x60C2]; /* 298E_(60C2 - 60FFh) */

    union  io_jdsdcore_ystbl        YSTBLBYT;   /* 298E_(6100 - 6141h) */

    unsigned char dmy_6142_617F[0x6180-0x6142]; /* 298E_(6142 - 617Fh) */

    union  io_jdsdcore_ystbl        YSTBLBCG;   /* 298E_(6180 - 61C1h) */

    unsigned char dmy_61C2_7FFF[0x8000-0x61C2]; /* 298E_(61C2 - 7FFFh) */

};

