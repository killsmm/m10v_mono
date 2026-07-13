/**
 * @file		__jdspro_stat_top_ahb.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_STAT_TOP_AHB_H_
#define _IO_PRO_STAT_TOP_AHB_H_

/*  structure of HIST0RDATA (2981_0000h)    */
/*  structure of HIST1RDATA (2981_1000h)    */
/*  structure of HIST2RDATA (2981_2000h)    */
/*  structure of HIST3RDATA (2981_3000h)    */
union io_histrdata{
    unsigned long       word[256];
    struct {
        unsigned long   HISTRDATA  :25;
        unsigned long               :7;
    }bit[256];
};

/*  structure of HIST0GDATA (2981_0400h)    */
/*  structure of HIST1GDATA (2981_1400h)    */
/*  structure of HIST2GDATA (2981_2400h)    */
/*  structure of HIST3GDATA (2981_3400h)    */
union io_histgdata{
    unsigned long       word[256];
    struct {
        unsigned long   HISTGDATA  :25;
        unsigned long               :7;
    }bit[256];
};

/*  structure of HIST0BDATA (2981_0800h)    */
/*  structure of HIST1BDATA (2981_1800h)    */
/*  structure of HIST2BDATA (2981_2800h)    */
/*  structure of HIST3BDATA (2981_3800h)    */
union io_histbdata{
    unsigned long       word[256];
    struct {
        unsigned long   HISTBDATA  :25;
        unsigned long               :7;
    }bit[256];
};

/*  structure of HIST0YDATA (2981_0C00h)    */
/*  structure of HIST1YDATA (2981_1C00h)    */
/*  structure of HIST2YDATA (2981_2C00h)    */
/*  structure of HIST3YDATA (2981_3C00h)    */
union io_histydata{
    unsigned long       word[256];
    struct {
        unsigned long   HISTYDATA  :25;
        unsigned long               :7;
    }bit[256];
};

struct io_histdata{
    union  io_histrdata  HISTRDATA;
    union  io_histgdata  HISTGDATA;
    union  io_histbdata  HISTBDATA;
    union  io_histydata  HISTYDATA;
};


/* Define i/o mapping */
struct io_stat_ahb{
    /* JDSIMG */
    struct io_histdata HISTDATA[4];   /* 2981_(0000 - 03FFh) */
                                      /* 2981_(0400 - 07FFh) */
                                      /* 2981_(0800 - 0BFFh) */
                                      /* 2981_(0C00 - 0FFFh) */
                                      /* 2981_(1000 - 13FFh) */
                                      /* 2981_(1400 - 17FFh) */
                                      /* 2981_(1800 - 1BFFh) */
                                      /* 2981_(1C00 - 1FFFh) */
                                      /* 2981_(2000 - 23FFh) */
                                      /* 2981_(2400 - 27FFh) */
                                      /* 2981_(2800 - 2BFFh) */
                                      /* 2981_(2C00 - 2FFFh) */
                                      /* 2981_(3000 - 33FFh) */
                                      /* 2981_(3400 - 37FFh) */
                                      /* 2981_(3800 - 3BFFh) */
                                      /* 2981_(3C00 - 3FFFh) */
};

#endif	// _IO_PRO_STAT_TOP_AHB_H_
