/**
 * @file        __jdspro_obd.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _IO_PRO_P2M_H_
#define _IO_PRO_P2M_H_

/*  structure of P2MTRG (2800_C000h)    */
union io_p2mtrg{
    unsigned long       word;
    struct {
        unsigned long   P2MTRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of P2MPAEN    (2800_C004h)    */
union io_p2mpaen{
    unsigned long       word;
    struct {
        unsigned long   PAEN    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of P2MCTL (2800_C008h)    */
union io_p2mctl{
    unsigned long       word;
    struct {
        unsigned long   PKNEMD  :2;
        unsigned long           :2;
        unsigned long   PLPF    :2;
        unsigned long           :2;
        unsigned long   P2MTYP  :2;
        unsigned long           :22;
    }bit;
};

/*  structure of P2MOFS (2800_C00Ch)    */
union io_p2mofs{
    unsigned long       word;
    struct {
        long            P2MOFS  :15;
        unsigned long           :17;
    }bit;
};

/*  structure of PBSFT  (2800_C010h)    */
union io_pbsft{
    unsigned long       word;
    struct {
        unsigned long   PBSFT   :4;
        unsigned long           :28;
    }bit;
};

/*  structure of PCLPH  (2800_C014h)    */
union io_pclph{
    unsigned long       word;
    struct {
        unsigned long   PCLPH   :16;
        unsigned long           :16;
    }bit;
};

/*  structure of PTRMV  (2800_C050h)    */
union io_ptrmv{
    unsigned long       word;
    struct {
        unsigned long   PTRMV   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of PTRMH  (2800_C054h)    */
union io_ptrmh{
    unsigned long       word;
    struct {
        unsigned long   PTRMH   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of PTRMVW (2800_C058h)    */
union io_ptrmvw{
    unsigned long       word;
    struct {
        unsigned long   PTRMVW  :16;
        unsigned long           :16;
    }bit;
};

/*  structure of PTRMHW (2800_C05Ch)    */
union io_ptrmhw{
    unsigned long       word;
    struct {
        unsigned long   PTRMHW  :24;
        unsigned long           :8;
    }bit;
};

/*  structure of PTRMXVCYC  (2800_C060h)    */
union io_ptrmxvcyc{
    unsigned long       word;
    struct {
        unsigned long   PTRMXVCYC   :6;
        unsigned long               :26;
    }bit;
};

/*  structure of PTRMXHCYC  (2800_C064h)    */
union io_ptrmxhcyc{
    unsigned long       word;
    struct {
        unsigned long   PTRMXHCYC   :6;
        unsigned long               :26;
    }bit;
};

/*  structure of PTRMXVEN   (2800_C068h)    */
union io_ptrmxven{
    unsigned long long      dword;
    struct {
        unsigned long long  PTRMXVEN    :64;
    }bit;
};

/*  structure of PTRMXHEN   (2800_C070h)    */
union io_ptrmxhen{
    unsigned long long      dword;
    struct {
        unsigned long long  PTRMXHEN    :64;
    }bit;
};

/*  structure of P2MMIR (2800_C078h)    */
union io_p2mmir{
    unsigned long       word;
    struct {
        unsigned long   P2MMIR  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of P2M2PMD    (2800_C080h)    */
union io_p2m2pmd{
    unsigned long       word;
    struct {
        unsigned long   P2M2PMD :1;
        unsigned long           :31;
    }bit;
};

/* Define i/o mapping */
struct io_p2m{
    /* JDSIMG */
    union  io_p2mtrg     P2MTRG;                /* 2800_(C000 - C003h) */
    union  io_p2mpaen    P2MPAEN;               /* 2800_(C004 - C007h) */
    union  io_p2mctl     P2MCTL;                /* 2800_(C008 - C00Bh) */
    union  io_p2mofs     P2MOFS;                /* 2800_(C00C - C00Fh) */
    union  io_pbsft      PBSFT;                 /* 2800_(C010 - C013h) */
    union  io_pclph      PCLPH;                 /* 2800_(C014 - C017h) */

    unsigned char dmy_C018_C04F[0xC050-0xC018]; /* 2800_(C018 - C04Fh) */

    union  io_ptrmv      PTRMV;                 /* 2800_(C050 - C053h) */
    union  io_ptrmh      PTRMH;                 /* 2800_(C054 - C057h) */
    union  io_ptrmvw     PTRMVW;                /* 2800_(C058 - C05Bh) */
    union  io_ptrmhw     PTRMHW;                /* 2800_(C05C - C05Fh) */
    union  io_ptrmxvcyc  PTRMXVCYC;             /* 2800_(C060 - C063h) */
    union  io_ptrmxhcyc  PTRMXHCYC;             /* 2800_(C064 - C067h) */
    union  io_ptrmxven   PTRMXVEN;              /* 2800_(C068 - C06Fh) */
    union  io_ptrmxhen   PTRMXHEN;              /* 2800_(C070 - C077h) */
    union  io_p2mmir     P2MMIR;                /* 2800_(C078 - C07Bh) */

    unsigned char dmy_C07C_C07F[0xC080-0xC07C]; /* 2800_(C07C - C07Fh) */

    union  io_p2m2pmd    P2M2PMD;               /* 2800_(C080 - C083h) */

    unsigned char dmy_C084_C0FF[0xC100-0xC084]; /* 2800_(C084 - C0FFh) */
};

#endif  // _IO_PRO_P2M_H_
