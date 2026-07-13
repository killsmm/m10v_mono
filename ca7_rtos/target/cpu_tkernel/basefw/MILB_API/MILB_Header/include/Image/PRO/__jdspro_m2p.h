/**
 * @file		__jdspro_m2p.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_M2P_H_
#define _IO_PRO_M2P_H_

/*  structure of M2PTRG (2840_AA00h)    */
union io_m2ptrg{
    unsigned long       word;
    struct {
        unsigned long   M2PTRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of M2PPAEN    (2840_AA04h)    */
union io_m2ppaen{
    unsigned long       word;
    struct {
        unsigned long   MPPAEN  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of M2PCTL (2840_AA08h)    */
union io_m2pctl{
    unsigned long       word;
    struct {
        unsigned long   M2PTYP  :2;
        unsigned long           :2;
        unsigned long   MDKNMD  :2;
        unsigned long           :26;
    }bit;
};

/*  structure of M2PBSFT    (2840_AA14h)    */
union io_m2pbsft{
    unsigned long       word;
    struct {
        unsigned long   M2PBSFT :4;
        unsigned long   M2PBMD  :1;
        unsigned long           :3;
        unsigned long   M2PBLR  :1;
        unsigned long           :7;
        unsigned long   M2PBT   :5;
        unsigned long           :11;
    }bit;
};

/*  structure of M2POFS (2840_AA18h)    */
union io_m2pofs{
    unsigned long       word;
    struct {
        long            M2POFS  :16;
        unsigned long           :16;
    }bit;
};

/*  structure of M2PCLIP    (2840_AA1Ch)    */
union io_m2pclip{
    unsigned long       word;
    struct {
        long            M2PCLIPLW   :16;
        unsigned long   M2PCLIPHI   :16;
    }bit;
};

/* Define i/o mapping */
struct io_m2p{
    /* JDSIMG */
    union  io_m2ptrg     M2PTRG;     /* 2840_(AA00 - AA03h) */
    union  io_m2ppaen    M2PPAEN;    /* 2840_(AA04 - AA07h) */
    union  io_m2pctl     M2PCTL;     /* 2840_(AA08 - AA0Bh) */

    unsigned char dmy_AA0C_AA13[0xAA14-0xAA0C]; /* 2840_(AA0C - AA13h) */

    union  io_m2pbsft    M2PBSFT;    /* 2840_(AA14 - AA17h) */
    union  io_m2pofs     M2POFS;     /* 2840_(AA18 - AA1Bh) */
    union  io_m2pclip    M2PCLIP;    /* 2840_(AA1C - AA1Fh) */

    unsigned char dmy_AA20_AAFF[0xAB00-0xAA20]; /* 2840_(AA20 - AAFFh) */

};

#endif	// _IO_PRO_M2P_H_
