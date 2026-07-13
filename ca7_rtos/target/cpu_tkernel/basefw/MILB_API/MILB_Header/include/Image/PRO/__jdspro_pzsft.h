/**
 * @file		__jdspro_pzsft.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_PZSFT_H_
#define _IO_PRO_PZSFT_H_

/*  structure of PZSFTTRG   (2840_9200h)    */
union io_pzsfttrg{
    unsigned long       word;
    struct {
        unsigned long   PZSFTTRG    :2;
        unsigned long               :30;
    }bit;
};

/*  structure of PZOFST (2840_9204h)    */
union io_pzofst{
    unsigned long       word;
    struct {
        long            PZOFST  :16;
        unsigned long           :16;
    }bit;
};

/* Define i/o mapping */
struct io_pzsft{
    /* JDSIMG */
    union  io_pzsfttrg PZSFTTRG;   /* 2840_(9200 - 9203h) */
    union  io_pzofst   PZOFST;     /* 2840_(9204 - 9207h) */

    unsigned char dmy_9008_90FF[0x9100-0x9008]; /* 2840_(9208 - 92FFh) */

};

#endif	// _IO_PRO_PZSFT_H_
