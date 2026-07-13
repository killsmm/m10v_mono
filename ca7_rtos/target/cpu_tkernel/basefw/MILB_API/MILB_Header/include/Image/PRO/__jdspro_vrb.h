/**
 * @file		__jdspro_vrb.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_VRB_H_
#define _IO_PRO_VRB_H_

/*  structure of VRBTRG (2840_9B00h)    */
union io_vrbtrg{
    unsigned long       word;
    struct {
        unsigned long   VRBTRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of VRBV   (2840_9B04h)    */
union io_vrbv{
    unsigned long       word;
    struct {
        unsigned long   VRBV    :13;
        unsigned long           :19;
    }bit;
};

/*  structure of VRBH   (2840_9B08h)    */
union io_vrbh{
    unsigned long       word;
    struct {
        unsigned long   VRBH    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of VRBVW  (2840_9B0Ch)    */
union io_vrbvw{
    unsigned long       word;
    struct {
        unsigned long   VRBVW   :13;
        unsigned long           :19;
    }bit;
};

/*  structure of VRBHW  (2840_9B10h)    */
union io_vrbhw{
    unsigned long       word;
    struct {
        unsigned long   VRBHW   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of VRBSCYC    (2840_9B14h)    */
union io_vrbscyc{
    unsigned long       word;
    struct {
        unsigned long   VRBSCYC :4;
        unsigned long           :28;
    }bit;
};

/*  structure of VRBPAEN    (2840_9B18h)    */
union io_vrbpaen{
    unsigned long       word;
    struct {
        unsigned long   PAEN    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of VRB0K  (2840_9B40h)    */
union io_vrb0k_1{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_0_0   :16;
        unsigned long   VRB0K_0_1   :16;
    }bit;
};
union io_vrb0k_2{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_0_2   :16;
        unsigned long   VRB0K_1_0   :16;
    }bit;
};
union io_vrb0k_3{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_1_1   :16;
        unsigned long   VRB0K_1_2   :16;
    }bit;
};
union io_vrb0k_4{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_2_0   :16;
        unsigned long   VRB0K_2_1   :16;
    }bit;
};
union io_vrb0k_5{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_2_2   :16;
        unsigned long   VRB0K_3_0   :16;
    }bit;
};
union io_vrb0k_6{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_3_1   :16;
        unsigned long   VRB0K_3_2   :16;
    }bit;
};
union io_vrb0k_7{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_4_0   :16;
        unsigned long   VRB0K_4_1   :16;
    }bit;
};
union io_vrb0k_8{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_4_2   :16;
        unsigned long   VRB0K_5_0   :16;
    }bit;
};
union io_vrb0k_9{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_5_1   :16;
        unsigned long   VRB0K_5_2   :16;
    }bit;
};
union io_vrb0k_10{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_6_0   :16;
        unsigned long   VRB0K_6_1   :16;
    }bit;
};
union io_vrb0k_11{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_6_2   :16;
        unsigned long   VRB0K_7_0   :16;
    }bit;
};
union io_vrb0k_12{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_7_1   :16;
        unsigned long   VRB0K_7_2   :16;
    }bit;
};
union io_vrb0k_13{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_8_0   :16;
        unsigned long   VRB0K_8_1   :16;
    }bit;
};
union io_vrb0k_14{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_8_2   :16;
        unsigned long   VRB0K_9_0   :16;
    }bit;
};
union io_vrb0k_15{
    unsigned long       word;
    struct {
        unsigned long   VRB0K_9_1   :16;
        unsigned long   VRB0K_9_2   :16;
    }bit;
};
struct io_vrb0k{
    union io_vrb0k_1     VRB0K1;
    union io_vrb0k_2     VRB0K2;
    union io_vrb0k_3     VRB0K3;
    union io_vrb0k_4     VRB0K4;
    union io_vrb0k_5     VRB0K5;
    union io_vrb0k_6     VRB0K6;
    union io_vrb0k_7     VRB0K7;
    union io_vrb0k_8     VRB0K8;
    union io_vrb0k_9     VRB0K9;
    union io_vrb0k_10    VRB0K10;
    union io_vrb0k_11    VRB0K11;
    union io_vrb0k_12    VRB0K12;
    union io_vrb0k_13    VRB0K13;
    union io_vrb0k_14    VRB0K14;
    union io_vrb0k_15    VRB0K15;
};

/*  structure of VRB1K  (2840_9B80h)    */
union io_vrb1k_1{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_0_0   :16;
        unsigned long   VRB1K_0_1   :16;
    }bit;
};
union io_vrb1k_2{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_0_2   :16;
        unsigned long   VRB1K_1_0   :16;
    }bit;
};
union io_vrb1k_3{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_1_1   :16;
        unsigned long   VRB1K_1_2   :16;
    }bit;
};
union io_vrb1k_4{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_2_0   :16;
        unsigned long   VRB1K_2_1   :16;
    }bit;
};
union io_vrb1k_5{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_2_2   :16;
        unsigned long   VRB1K_3_0   :16;
    }bit;
};
union io_vrb1k_6{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_3_1   :16;
        unsigned long   VRB1K_3_2   :16;
    }bit;
};
union io_vrb1k_7{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_4_0   :16;
        unsigned long   VRB1K_4_1   :16;
    }bit;
};
union io_vrb1k_8{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_4_2   :16;
        unsigned long   VRB1K_5_0   :16;
    }bit;
};
union io_vrb1k_9{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_5_1   :16;
        unsigned long   VRB1K_5_2   :16;
    }bit;
};
union io_vrb1k_10{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_6_0   :16;
        unsigned long   VRB1K_6_1   :16;
    }bit;
};
union io_vrb1k_11{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_6_2   :16;
        unsigned long   VRB1K_7_0   :16;
    }bit;
};
union io_vrb1k_12{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_7_1   :16;
        unsigned long   VRB1K_7_2   :16;
    }bit;
};
union io_vrb1k_13{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_8_0   :16;
        unsigned long   VRB1K_8_1   :16;
    }bit;
};
union io_vrb1k_14{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_8_2   :16;
        unsigned long   VRB1K_9_0   :16;
    }bit;
};
union io_vrb1k_15{
    unsigned long       word;
    struct {
        unsigned long   VRB1K_9_1   :16;
        unsigned long   VRB1K_9_2   :16;
    }bit;
};
struct io_vrb1k{
    union io_vrb1k_1     VRB1K1;
    union io_vrb1k_2     VRB1K2;
    union io_vrb1k_3     VRB1K3;
    union io_vrb1k_4     VRB1K4;
    union io_vrb1k_5     VRB1K5;
    union io_vrb1k_6     VRB1K6;
    union io_vrb1k_7     VRB1K7;
    union io_vrb1k_8     VRB1K8;
    union io_vrb1k_9     VRB1K9;
    union io_vrb1k_10    VRB1K10;
    union io_vrb1k_11    VRB1K11;
    union io_vrb1k_12    VRB1K12;
    union io_vrb1k_13    VRB1K13;
    union io_vrb1k_14    VRB1K14;
    union io_vrb1k_15    VRB1K15;
};

/* Define i/o mapping */
struct io_vrb{
    /* JDSIMG */
    union  io_vrbtrg     VRBTRG;     /* 2840_(9B00 - 9B03h) */
    union  io_vrbv       VRBV;       /* 2840_(9B04 - 9B07h) */
    union  io_vrbh       VRBH;       /* 2840_(9B08 - 9B0Bh) */
    union  io_vrbvw      VRBVW;      /* 2840_(9B0C - 9B0Fh) */
    union  io_vrbhw      VRBHW;      /* 2840_(9B10 - 9B13h) */
    union  io_vrbscyc    VRBSCYC;    /* 2840_(9B14 - 9B17h) */
    union  io_vrbpaen    VRBPAEN;    /* 2840_(9B18 - 9B1Bh) */

    unsigned char dmy_9B1C_9B3F[0x9B40-0x9B1C]; /* 2840_(9B1C - 9B3Fh) */

    struct io_vrb0k      VRB0K;      /* 2840_(9B40 - 9B7Bh) */

    unsigned char dmy_9B7C_9B7F[0x9B80-0x9B7C]; /* 2840_(9B7C - 9B7Fh) */

    struct io_vrb1k      VRB1K;      /* 2840_(9B80 - 9BBBh) */

    unsigned char dmy_9BBC_9BFF[0x9C00-0x9BBC]; /* 2840_(9BBC - 9BFFh) */

};

#endif	// _IO_PRO_VRB_H_
