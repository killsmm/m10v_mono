/**
 * @file		__jdspro_hist.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_HIST_H_
#define _IO_PRO_HIST_H_

/*  structure of HISTTRG    (2886_6600h)    */
union io_histtrg{
    unsigned long       word;
    struct {
        unsigned long   HISTTRG :2;
        unsigned long           :30;
    }bit;
};

/*  structure of HISTINTENB (2886_6604h)    */
union io_histintenb{
    unsigned long       word;
    struct {
        unsigned long   HISTE   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of HISTINTFLG (2886_6608h)    */
union io_histintflg{
    unsigned long       word;
    struct {
        unsigned long   __HISTF :1;
        unsigned long           :31;
    }bit;
};

/*  structure of HISTCTL    (2886_660Ch)    */
union io_histctl{
    unsigned long       word;
    struct {
        unsigned long   HISTCT      :1;
        unsigned long               :3;
        unsigned long   HISTHCYC    :2;
        unsigned long               :2;
        unsigned long   HISTSUMS    :3;
        unsigned long               :21;
    }bit;
};

/*  structure of HISTV  (2886_6610h)    */
union io_histv{
    unsigned long       word;
    struct {
        unsigned long   HISTV   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of HISTH  (2886_6614h)    */
union io_histh{
    unsigned long       word;
    struct {
        unsigned long   HISTH   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of HISTVW (2886_6618h)    */
union io_histvw{
    unsigned long       word;
    struct {
        unsigned long   HISTVW  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of HISTHW (2886_661Ch)    */
union io_histhw{
    unsigned long       word;
    struct {
        unsigned long   HISTHW  :14;
        unsigned long           :18;
    }bit;
};

/*  structure of HISTYKRR   (2886_6620h)    */
union io_histykrr{
    unsigned long       word;
    struct {
        long            HISTYKRR    :9;
        unsigned long               :23;
    }bit;
};

/*  structure of HISTYKGR   (2886_6624h)    */
union io_histykgr{
    unsigned long       word;
    struct {
        long            HISTYKGR    :9;
        unsigned long               :23;
    }bit;
};

/*  structure of HISTYKGB   (2886_6628h)    */
union io_histykgb{
    unsigned long       word;
    struct {
        long            HISTYKGB    :9;
        unsigned long               :23;
    }bit;
};

/*  structure of HISTYKBB   (2886_662Ch)    */
union io_histykbb{
    unsigned long       word;
    struct {
        long            HISTYKBB    :9;
        unsigned long               :23;
    }bit;
};

/*  structure of HISTRMODE  (2886_6630h)    */
union io_histrmode{
    unsigned long       word;
    struct {
        unsigned long   HISTRMODE   :8;
        unsigned long               :24;
    }bit;
};

/*  structure of HISTGMODE  (2886_6634h)    */
union io_histgmode{
    unsigned long       word;
    struct {
        unsigned long   HISTGMODE   :8;
        unsigned long               :24;
    }bit;
};

/*  structure of HISTBMODE  (2886_6638h)    */
union io_histbmode{
    unsigned long       word;
    struct {
        unsigned long   HISTBMODE   :8;
        unsigned long               :24;
    }bit;
};

/*  structure of HISTYMODE  (2886_663Ch)    */
union io_histymode{
    unsigned long       word;
    struct {
        unsigned long   HISTYMODE   :8;
        unsigned long               :24;
    }bit;
};

/*  structure of HISTRSUM   (2886_6644h)    */
union io_histrsum{
    unsigned long       word;
    struct {
        unsigned long   HISTRSUM    :32;
    }bit;
};

/*  structure of HISTGSUM   (2886_6648h)    */
union io_histgsum{
    unsigned long       word;
    struct {
        unsigned long   HISTGSUM    :32;
    }bit;
};

/*  structure of HISTBSUM   (2886_664Ch)    */
union io_histbsum{
    unsigned long       word;
    struct {
        unsigned long   HISTBSUM    :32;
    }bit;
};

/*  structure of HISTYSUM   (2886_6650h)    */
union io_histysum{
    unsigned long       word;
    struct {
        unsigned long   HISTYSUM    :32;
    }bit;
};

/*  structure of HISTWBGR   (2886_6654h)    */
union io_histwbgr{
    unsigned long       word;
    struct {
        unsigned long   HISTWBGR    :10;
        unsigned long               :22;
    }bit;
};

/*  structure of HISTWBGG   (2886_6658h)    */
union io_histwbgg{
    unsigned long       word;
    struct {
        unsigned long   HISTWBGG    :10;
        unsigned long               :22;
    }bit;
};

/*  structure of HISTWBGB   (2886_665Ch)    */
union io_histwbgb{
    unsigned long       word;
    struct {
        unsigned long   HISTWBGB    :10;
        unsigned long               :22;
    }bit;
};

/*  structure of HISTPAEN   (2886_6660h)    */
union io_histpaen{
    unsigned long       word;
    struct {
        unsigned long   PAEN    :1;
        unsigned long           :31;
    }bit;
};

/* Define i/o mapping */
struct io_hist{
    /* JDSIMG */
    union  io_histtrg       HISTTRG;    /* 2886_(6600 - 6603h) */
    union  io_histintenb    HISTINTENB; /* 2886_(6604 - 6607h) */
    union  io_histintflg    HISTINTFLG; /* 2886_(6608 - 660Bh) */
    union  io_histctl       HISTCTL;    /* 2886_(660C - 660Fh) */
    union  io_histv         HISTV;      /* 2886_(6610 - 6613h) */
    union  io_histh         HISTH;      /* 2886_(6614 - 6617h) */
    union  io_histvw        HISTVW;     /* 2886_(6618 - 661Bh) */
    union  io_histhw        HISTHW;     /* 2886_(661C - 661Fh) */
    union  io_histykrr      HISTYKRR;   /* 2886_(6620 - 6623h) */
    union  io_histykgr      HISTYKGR;   /* 2886_(6624 - 6627h) */
    union  io_histykgb      HISTYKGB;   /* 2886_(6628 - 662Bh) */
    union  io_histykbb      HISTYKBB;   /* 2886_(662C - 662Fh) */
    union  io_histrmode     HISTRMODE;  /* 2886_(6630 - 6633h) */
    union  io_histgmode     HISTGMODE;  /* 2886_(6634 - 6637h) */
    union  io_histbmode     HISTBMODE;  /* 2886_(6638 - 663Bh) */
    union  io_histymode     HISTYMODE;  /* 2886_(663C - 663Fh) */

    unsigned char dmy_6640_6643[0x6644-0x6640]; /* 2886_(6640 - 6643h) */

    union  io_histrsum      HISTRSUM;   /* 2886_(6644 - 6647h) */
    union  io_histgsum      HISTGSUM;   /* 2886_(6648 - 664Bh) */
    union  io_histbsum      HISTBSUM;   /* 2886_(664C - 664Fh) */
    union  io_histysum      HISTYSUM;   /* 2886_(6650 - 6653h) */
    union  io_histwbgr      HISTWBGR;   /* 2886_(6654 - 6657h) */
    union  io_histwbgg      HISTWBGG;   /* 2886_(6658 - 665Bh) */
    union  io_histwbgb      HISTWBGB;   /* 2886_(665C - 665Fh) */
    union  io_histpaen      HISTPAEN;   /* 2886_(6660 - 6663h) */

    unsigned char dmy_6664_66FF[0x6700-0x6664]; /* 2886_(6664 - 66FFh) */

};

#endif	// _IO_PRO_HIST_H_
