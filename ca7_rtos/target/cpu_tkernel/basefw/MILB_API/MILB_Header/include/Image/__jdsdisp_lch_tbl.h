/**
 * @file		__jdsdisp_lch_tbl.h
 * @brief		Definition JDSDISP3A AHB LCH Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*  structure of IGTBLxy (298E_0000h) x=A/B, y=R/G/B */
union io_jdslch_igtbl{
    unsigned short      hword[32];
    struct {
        unsigned short  IG      :12;
        unsigned short          :4;
    }bit[32];
};

/*  structure of IGTBLAR ~ IGTBLAB / IGTBLBR ~ IGTBLBB */
struct io_jdslch_igtbl_rgb{
    union  io_jdslch_igtbl    IGTBLR;
    union  io_jdslch_igtbl    IGTBLG;
    union  io_jdslch_igtbl    IGTBLB;
};

/*  structure of GTBLxy (298E_2000h) x=A/B, y=R/G/B */
union io_jdslch_gtbl{
    unsigned short      hword[32];
    struct {
        unsigned short  GM      :12;
        unsigned short          :4;
    }bit[32];
};

/*  structure of GTBLAR ~ GTBLAB / GTBLBR ~ GTBLBB */
struct io_jdslch_gtbl_rgb{
    union  io_jdslch_gtbl       GTBLR;
    union  io_jdslch_gtbl       GTBLG;
    union  io_jdslch_gtbl       GTBLB;
};

/* Define i/o mapping */
struct io_jdslch_tbl{
    /* JDSIMG */
    struct io_jdslch_igtbl_rgb      IGTBLA;     /* 298E_(0000 - 00BFh) */

    unsigned char dmy_00C0_00FF[0x0100-0x00C0]; /* 298E_(00C0 - 00FFh) */

    struct io_jdslch_igtbl_rgb      IGTBLB;     /* 298E_(0100 - 01BFh) */

    unsigned char dmy_01C0_1FFF[0x2000-0x01C0]; /* 298E_(01C0 - 1FFFh) */

    struct io_jdslch_gtbl_rgb       GTBLA;      /* 298E_(2000 - 20BFh) */

    unsigned char dmy_20C0_20FF[0x2100-0x20C0]; /* 298E_(20C0 - 20FFh) */

    struct io_jdslch_gtbl_rgb       GTBLB;      /* 298E_(2100 - 21BFh) */

    unsigned char dmy_21C0_3FFF[0x4000-0x21C0]; /* 298E_(21C0 - 3FFFh) */
};

