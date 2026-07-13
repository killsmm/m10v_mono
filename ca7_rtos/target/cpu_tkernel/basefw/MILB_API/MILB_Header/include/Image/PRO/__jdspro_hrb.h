/**
 * @file		__jdspro_hrb.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_HRB_H_
#define _IO_PRO_HRB_H_

/*  structure of HRBTRG (2840_9C00h)    */
union io_hrbtrg{
    unsigned long       word;
    struct {
        unsigned long   HRBTRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of HRBV   (2840_9C04h)    */
union io_hrbv{
    unsigned long       word;
    struct {
        unsigned long   HRBV    :13;
        unsigned long           :19;
    }bit;
};

/*  structure of HRBH   (2840_9C08h)    */
union io_hrbh{
    unsigned long       word;
    struct {
        unsigned long   HRBH    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of HRBVW  (2840_9C0Ch)    */
union io_hrbvw{
    unsigned long       word;
    struct {
        unsigned long   HRBVW   :13;
        unsigned long           :19;
    }bit;
};

/*  structure of HRBHW  (2840_9C10h)    */
union io_hrbhw{
    unsigned long       word;
    struct {
        unsigned long   HRBHW   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of HRBOHW (2840_9C14h)    */
union io_hrbohw{
    unsigned long       word;
    struct {
        unsigned long   HRBOHW  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of HRBSCYC    (2840_9C18h)    */
union io_hrbscyc{
    unsigned long       word;
    struct {
        unsigned long   HRBSCYC :5;
        unsigned long           :27;
    }bit;
};

/*  structure of HRBOSM (2840_9C1Ch)    */
union io_hrbosm{
    unsigned long       word;
    struct {
        unsigned long   HRBOSM  :13;
        unsigned long           :19;
    }bit;
};

/*  structure of HRBPAEN    (2840_9C20h)    */
union io_hrbpaen{
    unsigned long       word;
    struct {
        unsigned long   PAEN    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of HRBK   (2840_9D00h)    */
union io_hrbk_1{
    unsigned long       word;
    struct {
        unsigned long   HRBK_0_0    :16;
        unsigned long   HRBK_0_1    :16;
    }bit;
};
union io_hrbk_2{
    unsigned long       word;
    struct {
        unsigned long   HRBK_0_2    :16;
        unsigned long   HRBK_0_3    :16;
    }bit;
};
union io_hrbk_3{
    unsigned long       word;
    struct {
        unsigned long   HRBK_0_4    :16;
        unsigned long   HRBK_1_0    :16;
    }bit;
};
union io_hrbk_4{
    unsigned long       word;
    struct {
        unsigned long   HRBK_1_1    :16;
        unsigned long   HRBK_1_2    :16;
    }bit;
};
union io_hrbk_5{
    unsigned long       word;
    struct {
        unsigned long   HRBK_1_3    :16;
        unsigned long   HRBK_1_4    :16;
    }bit;
};
union io_hrbk_6{
    unsigned long       word;
    struct {
        unsigned long   HRBK_2_0    :16;
        unsigned long   HRBK_2_1    :16;
    }bit;
};
union io_hrbk_7{
    unsigned long       word;
    struct {
        unsigned long   HRBK_2_2    :16;
        unsigned long   HRBK_2_3    :16;
    }bit;
};
union io_hrbk_8{
    unsigned long       word;
    struct {
        unsigned long   HRBK_2_4    :16;
        unsigned long   HRBK_3_0    :16;
    }bit;
};
union io_hrbk_9{
    unsigned long       word;
    struct {
        unsigned long   HRBK_3_1    :16;
        unsigned long   HRBK_3_2    :16;
    }bit;
};
union io_hrbk_10{
    unsigned long       word;
    struct {
        unsigned long   HRBK_3_3    :16;
        unsigned long   HRBK_3_4    :16;
    }bit;
};
union io_hrbk_11{
    unsigned long       word;
    struct {
        unsigned long   HRBK_4_0    :16;
        unsigned long   HRBK_4_1    :16;
    }bit;
};
union io_hrbk_12{
    unsigned long       word;
    struct {
        unsigned long   HRBK_4_2    :16;
        unsigned long   HRBK_4_3    :16;
    }bit;
};
union io_hrbk_13{
    unsigned long       word;
    struct {
        unsigned long   HRBK_4_4    :16;
        unsigned long   HRBK_5_0    :16;
    }bit;
};
union io_hrbk_14{
    unsigned long       word;
    struct {
        unsigned long   HRBK_5_1    :16;
        unsigned long   HRBK_5_2    :16;
    }bit;
};
union io_hrbk_15{
    unsigned long       word;
    struct {
        unsigned long   HRBK_5_3    :16;
        unsigned long   HRBK_5_4    :16;
    }bit;
};
union io_hrbk_16{
    unsigned long       word;
    struct {
        unsigned long   HRBK_6_0    :16;
        unsigned long   HRBK_6_1    :16;
    }bit;
};
union io_hrbk_17{
    unsigned long       word;
    struct {
        unsigned long   HRBK_6_2    :16;
        unsigned long   HRBK_6_3    :16;
    }bit;
};
union io_hrbk_18{
    unsigned long       word;
    struct {
        unsigned long   HRBK_6_4    :16;
        unsigned long   HRBK_7_0    :16;
    }bit;
};
union io_hrbk_19{
    unsigned long       word;
    struct {
        unsigned long   HRBK_7_1    :16;
        unsigned long   HRBK_7_2    :16;
    }bit;
};
union io_hrbk_20{
    unsigned long       word;
    struct {
        unsigned long   HRBK_7_3    :16;
        unsigned long   HRBK_7_4    :16;
    }bit;
};
union io_hrbk_21{
    unsigned long       word;
    struct {
        unsigned long   HRBK_8_0    :16;
        unsigned long   HRBK_8_1    :16;
    }bit;
};
union io_hrbk_22{
    unsigned long       word;
    struct {
        unsigned long   HRBK_8_2    :16;
        unsigned long   HRBK_8_3    :16;
    }bit;
};
union io_hrbk_23{
    unsigned long       word;
    struct {
        unsigned long   HRBK_8_4    :16;
        unsigned long   HRBK_9_0    :16;
    }bit;
};
union io_hrbk_24{
    unsigned long       word;
    struct {
        unsigned long   HRBK_9_1    :16;
        unsigned long   HRBK_9_2    :16;
    }bit;
};
union io_hrbk_25{
    unsigned long       word;
    struct {
        unsigned long   HRBK_9_3    :16;
        unsigned long   HRBK_9_4    :16;
    }bit;
};
union io_hrbk_26{
    unsigned long       word;
    struct {
        unsigned long   HRBK_10_0   :16;
        unsigned long   HRBK_10_1   :16;
    }bit;
};
union io_hrbk_27{
    unsigned long       word;
    struct {
        unsigned long   HRBK_10_2   :16;
        unsigned long   HRBK_10_3   :16;
    }bit;
};
union io_hrbk_28{
    unsigned long       word;
    struct {
        unsigned long   HRBK_10_4   :16;
        unsigned long   HRBK_11_0   :16;
    }bit;
};
union io_hrbk_29{
    unsigned long       word;
    struct {
        unsigned long   HRBK_11_1   :16;
        unsigned long   HRBK_11_2   :16;
    }bit;
};
union io_hrbk_30{
    unsigned long       word;
    struct {
        unsigned long   HRBK_11_3   :16;
        unsigned long   HRBK_11_4   :16;
    }bit;
};
union io_hrbk_31{
    unsigned long       word;
    struct {
        unsigned long   HRBK_12_0   :16;
        unsigned long   HRBK_12_1   :16;
    }bit;
};
union io_hrbk_32{
    unsigned long       word;
    struct {
        unsigned long   HRBK_12_2   :16;
        unsigned long   HRBK_12_3   :16;
    }bit;
};
union io_hrbk_33{
    unsigned long       word;
    struct {
        unsigned long   HRBK_12_4   :16;
        unsigned long   HRBK_13_0   :16;
    }bit;
};
union io_hrbk_34{
    unsigned long       word;
    struct {
        unsigned long   HRBK_13_1   :16;
        unsigned long   HRBK_13_2   :16;
    }bit;
};
union io_hrbk_35{
    unsigned long       word;
    struct {
        unsigned long   HRBK_13_3   :16;
        unsigned long   HRBK_13_4   :16;
    }bit;
};
union io_hrbk_36{
    unsigned long       word;
    struct {
        unsigned long   HRBK_14_0   :16;
        unsigned long   HRBK_14_1   :16;
    }bit;
};
union io_hrbk_37{
    unsigned long       word;
    struct {
        unsigned long   HRBK_14_2   :16;
        unsigned long   HRBK_14_3   :16;
    }bit;
};
union io_hrbk_38{
    unsigned long       word;
    struct {
        unsigned long   HRBK_14_4   :16;
        unsigned long   HRBK_15_0   :16;
    }bit;
};
union io_hrbk_39{
    unsigned long       word;
    struct {
        unsigned long   HRBK_15_1   :16;
        unsigned long   HRBK_15_2   :16;
    }bit;
};
union io_hrbk_40{
    unsigned long       word;
    struct {
        unsigned long   HRBK_15_3   :16;
        unsigned long   HRBK_15_4   :16;
    }bit;
};
union io_hrbk_41{
    unsigned long       word;
    struct {
        unsigned long   HRBK_16_0   :16;
        unsigned long   HRBK_16_1   :16;
    }bit;
};
union io_hrbk_42{
    unsigned long       word;
    struct {
        unsigned long   HRBK_16_2   :16;
        unsigned long   HRBK_16_3   :16;
    }bit;
};
union io_hrbk_43{
    unsigned long       word;
    struct {
        unsigned long   HRBK_16_4   :16;
        unsigned long   HRBK_17_0   :16;
    }bit;
};
union io_hrbk_44{
    unsigned long       word;
    struct {
        unsigned long   HRBK_17_1   :16;
        unsigned long   HRBK_17_2   :16;
    }bit;
};
union io_hrbk_45{
    unsigned long       word;
    struct {
        unsigned long   HRBK_17_3   :16;
        unsigned long   HRBK_17_4   :16;
    }bit;
};
union io_hrbk_46{
    unsigned long       word;
    struct {
        unsigned long   HRBK_18_0   :16;
        unsigned long   HRBK_18_1   :16;
    }bit;
};
union io_hrbk_47{
    unsigned long       word;
    struct {
        unsigned long   HRBK_18_2   :16;
        unsigned long   HRBK_18_3   :16;
    }bit;
};
union io_hrbk_48{
    unsigned long       word;
    struct {
        unsigned long   HRBK_18_4   :16;
        unsigned long   HRBK_19_0   :16;
    }bit;
};
union io_hrbk_49{
    unsigned long       word;
    struct {
        unsigned long   HRBK_19_1   :16;
        unsigned long   HRBK_19_2   :16;
    }bit;
};
union io_hrbk_50{
    unsigned long       word;
    struct {
        unsigned long   HRBK_19_3   :16;
        unsigned long   HRBK_19_4   :16;
    }bit;
};
struct io_hrbk{
    union io_hrbk_1  HRBK1;
    union io_hrbk_2  HRBK2;
    union io_hrbk_3  HRBK3;
    union io_hrbk_4  HRBK4;
    union io_hrbk_5  HRBK5;
    union io_hrbk_6  HRBK6;
    union io_hrbk_7  HRBK7;
    union io_hrbk_8  HRBK8;
    union io_hrbk_9  HRBK9;
    union io_hrbk_10 HRBK10;
    union io_hrbk_11 HRBK11;
    union io_hrbk_12 HRBK12;
    union io_hrbk_13 HRBK13;
    union io_hrbk_14 HRBK14;
    union io_hrbk_15 HRBK15;
    union io_hrbk_16 HRBK16;
    union io_hrbk_17 HRBK17;
    union io_hrbk_18 HRBK18;
    union io_hrbk_19 HRBK19;
    union io_hrbk_20 HRBK20;
    union io_hrbk_21 HRBK21;
    union io_hrbk_22 HRBK22;
    union io_hrbk_23 HRBK23;
    union io_hrbk_24 HRBK24;
    union io_hrbk_25 HRBK25;
    union io_hrbk_26 HRBK26;
    union io_hrbk_27 HRBK27;
    union io_hrbk_28 HRBK28;
    union io_hrbk_29 HRBK29;
    union io_hrbk_30 HRBK30;
    union io_hrbk_31 HRBK31;
    union io_hrbk_32 HRBK32;
    union io_hrbk_33 HRBK33;
    union io_hrbk_34 HRBK34;
    union io_hrbk_35 HRBK35;
    union io_hrbk_36 HRBK36;
    union io_hrbk_37 HRBK37;
    union io_hrbk_38 HRBK38;
    union io_hrbk_39 HRBK39;
    union io_hrbk_40 HRBK40;
    union io_hrbk_41 HRBK41;
    union io_hrbk_42 HRBK42;
    union io_hrbk_43 HRBK43;
    union io_hrbk_44 HRBK44;
    union io_hrbk_45 HRBK45;
    union io_hrbk_46 HRBK46;
    union io_hrbk_47 HRBK47;
    union io_hrbk_48 HRBK48;
    union io_hrbk_49 HRBK49;
    union io_hrbk_50 HRBK50;
};

/* Define i/o mapping */
struct io_hrb{
    /* JDSIMG */
    union  io_hrbtrg     HRBTRG;     /* 2840_(9C00 - 9C03h) */
    union  io_hrbv       HRBV;       /* 2840_(9C04 - 9C07h) */
    union  io_hrbh       HRBH;       /* 2840_(9C08 - 9C0Bh) */
    union  io_hrbvw      HRBVW;      /* 2840_(9C0C - 9C0Fh) */
    union  io_hrbhw      HRBHW;      /* 2840_(9C10 - 9C13h) */
    union  io_hrbohw     HRBOHW;     /* 2840_(9C14 - 9C17h) */
    union  io_hrbscyc    HRBSCYC;    /* 2840_(9C18 - 9C1Bh) */
    union  io_hrbosm     HRBOSM;     /* 2840_(9C1C - 9C1Fh) */
    union  io_hrbpaen    HRBPAEN;    /* 2840_(9C20 - 9C23h) */

    unsigned char dmy_9C24_9CFF[0x9D00-0x9C24]; /* 2840_(9C24 - 9CFFh) */

    struct io_hrbk       HRBK;       /* 2840_(9D00 - 9DC7h) */

    unsigned char dmy_9DC8_9DFF[0x9E00-0x9DC8]; /* 2840_(9DC8 - 9DFFh) */

};

#endif	// _IO_PRO_HRB_H_
