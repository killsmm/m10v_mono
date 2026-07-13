/**
 * @file        __jdspro_moni.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_MONI_H_
#define _IO_PRO_MONI_H_

/*  structure of MONITRG    (2800_A400h)    */
union io_monitrg{
    unsigned long       word;
    struct {
        unsigned long   MONITRG :2;
        unsigned long           :30;
    }bit;
};

/*  structure of MONICTL    (2800_A404h)    */
union io_monictl{
    unsigned long       word;
    struct {
        unsigned long   MONIMD  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of MONIINTFLG (2800_A408h)    */
union io_moniintflg{
    unsigned long       word;
    struct {
        unsigned long   __MONIHDF     :1;
        unsigned long                 :3;
        unsigned long   __MONIEPFF    :1;
        unsigned long                 :3;
        unsigned long   __MONIEPLF    :1;
        unsigned long                 :3;
        unsigned long   __MONIBLLMINF :1;
        unsigned long                 :3;
        unsigned long   __MONIBLLMAXF :1;
        unsigned long                 :15;
    }bit;
};

/*  structure of MONIINTENB (2800_A40Ch)    */
union io_moniintenb{
    unsigned long       word;
    struct {
        unsigned long   MONIHDE     :1;
        unsigned long               :3;
        unsigned long   MONIEPFE    :1;
        unsigned long               :3;
        unsigned long   MONIEPLE    :1;
        unsigned long               :3;
        unsigned long   MONIBLLMINE :1;
        unsigned long               :3;
        unsigned long   MONIBLLMAXE :1;
        unsigned long               :15;
    }bit;
};

/*  structure of MONIVD (2800_A410h)    */
union io_monivd{
    unsigned long       word;
    struct {
        unsigned long   MONIVD  :16;
        unsigned long           :16;
    }bit;
};

/*  structure of MONIHD (2800_A414h)    */
union io_monihd{
    unsigned long       word;
    struct {
        unsigned long   MONIHD  :16;
        unsigned long           :16;
    }bit;
};

/*  structure of MONIEPF    (2800_A418h)    */
union io_moniepf{
    unsigned long       word;
    struct {
        unsigned long   MONIEPF :32;
    }bit;
};

/*  structure of MONIEPLMIN (2800_A41Ch)    */
union io_monieplmin{
    unsigned long       word;
    struct {
        unsigned long   MONIEPLMIN  :16;
        unsigned long               :16;
    }bit;
};

/*  structure of MONIEPLMAX (2800_A420h)    */
union io_monieplmax{
    unsigned long       word;
    struct {
        unsigned long   MONIEPLMAX  :16;
        unsigned long               :16;
    }bit;
};

/*  structure of MONIBLF    (2800_A428h)    */
union io_moniblf{
    unsigned long long      dword;
    struct {
        unsigned long long  MONIBLF :64;
    }bit;
};

/*  structure of MONIBLLMIN (2800_A430h)    */
union io_monibllmin{
    unsigned long       word;
    struct {
        unsigned long   MONIBLLMIN  :32;
    }bit;
};

/*  structure of MONIBLLMAX (2800_A434h)    */
union io_monibllmax{
    unsigned long       word;
    struct {
        unsigned long   MONIBLLMAX  :32;
    }bit;
};

/*  structure of MONIHDEXP  (2800_A438h)    */
union io_monihdexp{
    unsigned long       word;
    struct {
        unsigned long   MONIHDEXP   :16;
        unsigned long               :16;
    }bit;
};

/*  structure of MONIEPFEXP (2800_A43Ch)    */
union io_moniepfexp{
    unsigned long       word;
    struct {
        unsigned long   MONIEPFEXP  :32;
    }bit;
};

/*  structure of MONIBLLMINEXP  (2800_A440h)    */
union io_monibllminexp{
    unsigned long       word;
    struct {
        unsigned long   MONIBLLMINEXP   :32;
    }bit;
};

/*  structure of MONIBLLMAXEXP  (2800_A444h)    */
union io_monibllmaxexp{
    unsigned long       word;
    struct {
        unsigned long   MONIBLLMAXEXP   :32;
    }bit;
};

/*  structure of MONIADHD   (2800_A448h)    */
union io_moniadhd{
    unsigned long       word;
    struct {
        unsigned long   MONIADHD    :12;
        unsigned long               :20;
    }bit;
};

/*  structure of MONIADHW   (2800_A450h)    */
union io_moniadhw{
    unsigned long long      dword;
    struct {
        unsigned long long  MONIADHW    :48;
        unsigned long long              :16;
    }bit;
};

/* Define i/o mapping */
struct io_moni{
    /* JDSIMG */
    union  io_monitrg           MONITRG;        /* 2800_(A400 - A403h) */
    union  io_monictl           MONICTL;        /* 2800_(A404 - A407h) */
    union  io_moniintflg        MONIINTFLG;     /* 2800_(A408 - A40Bh) */
    union  io_moniintenb        MONIINTENB;     /* 2800_(A40C - A40Fh) */
    union  io_monivd            MONIVD;         /* 2800_(A410 - A413h) */
    union  io_monihd            MONIHD;         /* 2800_(A414 - A417h) */
    union  io_moniepf           MONIEPF;        /* 2800_(A418 - A41Bh) */
    union  io_monieplmin        MONIEPLMIN;     /* 2800_(A41C - A41Fh) */
    union  io_monieplmax        MONIEPLMAX;     /* 2800_(A420 - A423h) */

    unsigned char dmy_A024_A027[0xA028-0xA024]; /* 2800_(A424 - A427h) */

    union  io_moniblf           MONIBLF;        /* 2800_(A428 - A42Fh) */
    union  io_monibllmin        MONIBLLMIN;     /* 2800_(A430 - A433h) */
    union  io_monibllmax        MONIBLLMAX;     /* 2800_(A434 - A437h) */
    union  io_monihdexp         MONIHDEXP;      /* 2800_(A438 - A43Bh) */
    union  io_moniepfexp        MONIEPFEXP;     /* 2800_(A43C - A43Fh) */
    union  io_monibllminexp     MONIBLLMINEXP;  /* 2800_(A440 - A443h) */
    union  io_monibllmaxexp     MONIBLLMAXEXP;  /* 2800_(A444 - A447h) */
    union  io_moniadhd          MONIADHD;       /* 2800_(A448 - A44Bh) */

    unsigned char dmy_A44C_A44F[0xA450-0xA44C]; /* 2800_(A44C - A44Fh) */

    union  io_moniadhw          MONIADHW;       /* 2800_(A450 - A457h) */

    unsigned char dmy_A458_A4FF[0xA500-0xA458];  /* 2800_(A458 - A4FFh) */
};

#endif  // _IO_PRO_MONI_H_
