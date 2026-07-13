/**
 * @file        __jdspro_m9m_buz.h
 * @brief       Definition JDSPRO_M9M Macro I/O register
 * @note        None
 * @attention   This is for Kubota Only
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_BUZ_H_
#define _IO_PRO_BUZ_H_

/*  structure of SR    (00h)    */
union io_buz_sr{
    unsigned long       word;
    struct {
        unsigned long	SR :1;
        unsigned long      :31;
    }bit;
};

/*  structure of PAEN  (04h)    */
union io_buz_paen{
    unsigned long       word;
    struct {
        unsigned long	PAEN :1;
        unsigned long        :31;
    }bit;
};

/*  structure of BUZV  (08h)    */
union io_buz_buzv{
    unsigned long       word;
    struct {
        unsigned long	BUZV :14;
        unsigned long        :18;
    }bit;
};

/*  structure of BUZH  (0Ch)    */
union io_buz_buzh{
    unsigned long       word;
    struct {
        unsigned long	BUZH :12;
        unsigned long        :20;
    }bit;
};

/*  structure of BUZVW (10h)    */
union io_buz_buzvw{
    unsigned long       word;
    struct {
        unsigned long	BUZVW :12;
        unsigned long         :20;
    }bit;
};

/*  structure of BUZHW (14h)    */
union io_buz_buzhw{
    unsigned long       word;
    struct {
        unsigned long	BUZHW :12;
        unsigned long         :20;
    }bit;
};

/*  structure of BUZOHW(18h)    */
union io_buz_buzohw{
    unsigned long       word;
    struct {
        unsigned long	BUZOHW:12;
        unsigned long         :20;
    }bit;
};

/* Define i/o mapping */
struct io_buz
{
    union  io_buz_sr		SR;				/* 28x0_C4(00-03h) */
    union  io_buz_paen		PAEN;			/* 28x0_C4(04-07h) */
    union  io_buz_buzv		BUZV;			/* 28x0_C4(08-0Bh) */
    union  io_buz_buzh		BUZH;			/* 28x0_C4(0C-0Fh) */
    union  io_buz_buzvw		BUZVW;			/* 28x0_C4(10-13h) */
    union  io_buz_buzhw		BUZHW;			/* 28x0_C4(14-17h) */
    union  io_buz_buzohw	BUZOHW;			/* 28x0_C4(18-1Bh) */
    unsigned char dmy_1C_FF[0x100-0x01C];	/* 28x0_C4(1C-FFh) */
};
#endif  // _IO_PRO_BUZ_H_

