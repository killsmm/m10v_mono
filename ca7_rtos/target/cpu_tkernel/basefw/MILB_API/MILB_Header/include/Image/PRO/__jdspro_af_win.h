/**
 * @file		__jdspro_af_win.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_AF_WIN_H_
#define _IO_PRO_AF_WIN_H_

/*  structure of AFWINCTL   (2886_2400h)    */
union io_afwinctl{
    unsigned long       word;
    struct {
        unsigned long   AFSLWH0 :3;
        unsigned long           :1;
        unsigned long   AFSLWH1 :3;
        unsigned long           :1;
        unsigned long   AFSLWH2 :3;
        unsigned long           :1;
        unsigned long   AFSLWC  :2;
        unsigned long           :18;
    }bit;
};

/*  structure of AFCLVGE    (2886_2404h)    */
union io_afclvge{
    unsigned long       word;
    struct {
        unsigned long   AFCLVGE :12;
        unsigned long           :20;
    }bit;
};

/*  structure of AFCLVL (2886_2408h)    */
union io_afclvl{
    unsigned long       word;
    struct {
        unsigned long   AFCLVL  :12;
        unsigned long           :20;
    }bit;
};

/*  structure of AFV    (2886_240Ch)    */
union io_afv{
    unsigned long       word;
    struct {
        unsigned long   AFV :14;
        unsigned long       :18;
    }bit;
};

/*  structure of AFH    (2886_2410h)    */
union io_afh{
    unsigned long       word;
    struct {
        unsigned long   AFH :14;
        unsigned long       :18;
    }bit;
};

/*  structure of AFVW   (2886_2414h)    */
union io_afvw{
    unsigned long       word;
    struct {
        unsigned long   AFVW    :12;
        unsigned long           :20;
    }bit;
};

/*  structure of AFHW   (2886_2418h)    */
union io_afhw{
    unsigned long       word;
    struct {
        unsigned long   AFHW    :12;
        unsigned long           :20;
    }bit;
};

/*  structure of AFDATAS    (2886_241Ch)    */
union io_afdatas{
    unsigned long       word;
    struct {
        unsigned long   AFDATAS :32;
    }bit;
};

/*  structure of AFDATAGE   (2886_2420h)    */
union io_afdatage{
    unsigned long       word;
    struct {
        unsigned long   AFDATAGE    :22;
        unsigned long               :10;
    }bit;
};

/*  structure of AFDATAL    (2886_2424h)    */
union io_afdatal{
    unsigned long       word;
    struct {
        unsigned long   AFDATAL :22;
        unsigned long           :10;
    }bit;
};

/*  structure of AFDATAH0   (2886_2428h)    */
union io_afdatah0{
    unsigned long       word;
    struct {
        unsigned long   AFDATAH0    :32;
    }bit;
};

/*  structure of AFDATAH1   (2886_242Ch)    */
union io_afdatah1{
    unsigned long       word;
    struct {
        unsigned long   AFDATAH1    :32;
    }bit;
};

/*  structure of AFDATAH2   (2886_2430h)    */
union io_afdatah2{
    unsigned long       word;
    struct {
        unsigned long   AFDATAH2    :32;
    }bit;
};

/*  structure of AFDATAV    (2886_2434h)    */
union io_afdatav{
    unsigned long       word;
    struct {
        unsigned long   AFDATAV :32;
    }bit;
};

/*  structure of AFSLWM (2886_2438h)    */
union io_afslwm{
    unsigned long       word;
    struct {
        unsigned long   AFSLWM  :4;
        unsigned long           :28;
    }bit;
};

/*  structure of AFDATAMAX  (2886_243Ch)    */
union io_afdatamax{
    unsigned long       word;
    struct {
        unsigned long   AFDATAMAX   :12;
        unsigned long               :4;
        unsigned long   AFDATAMAXE  :12;
        unsigned long               :4;
    }bit;
};

/*  structure of AFDATAMIN  (2886_2440h)    */
union io_afdatamin{
    unsigned long       word;
    struct {
        unsigned long   AFDATAMIN   :12;
        unsigned long               :4;
        unsigned long   AFDATAMINE  :12;
        unsigned long               :4;
    }bit;
};

/*  structure of AFDATAMH0  (2886_2444h)    */
union io_afdatamh0{
    unsigned long       word;
    struct {
        unsigned long   AFDATAMH0   :23;
        unsigned long               :9;
    }bit;
};

/*  structure of AFDATAMH1  (2886_2448h)    */
union io_afdatamh1{
    unsigned long       word;
    struct {
        unsigned long   AFDATAMH1   :23;
        unsigned long               :9;
    }bit;
};

/*  structure of AFDATAMH2  (2886_244Ch)    */
union io_afdatamh2{
    unsigned long       word;
    struct {
        unsigned long   AFDATAMH2   :23;
        unsigned long               :9;
    }bit;
};

/* Define i/o mapping */
struct io_afwin{
    /* JDSIMG */
    union  io_afwinctl     AFWINCTL;   /* 2886_(2400 - 2403h) */
    union  io_afclvge      AFCLVGE;    /* 2886_(2404 - 2407h) */
    union  io_afclvl       AFCLVL;     /* 2886_(2408 - 240Bh) */
    union  io_afv          AFV;        /* 2886_(240C - 240Fh) */
    union  io_afh          AFH;        /* 2886_(2410 - 2413h) */
    union  io_afvw         AFVW;       /* 2886_(2414 - 2417h) */
    union  io_afhw         AFHW;       /* 2886_(2418 - 241Bh) */
    union  io_afdatas      AFDATAS;    /* 2886_(241C - 241Fh) */
    union  io_afdatage     AFDATAGE;   /* 2886_(2420 - 2423h) */
    union  io_afdatal      AFDATAL;    /* 2886_(2424 - 2427h) */
    union  io_afdatah0     AFDATAH0;   /* 2886_(2428 - 242Bh) */
    union  io_afdatah1     AFDATAH1;   /* 2886_(242C - 242Fh) */
    union  io_afdatah2     AFDATAH2;   /* 2886_(2430 - 2433h) */
    union  io_afdatav      AFDATAV;    /* 2886_(2434 - 2437h) */
    union  io_afslwm       AFSLWM;     /* 2886_(2438 - 243Bh) */
    union  io_afdatamax    AFDATAMAX;  /* 2886_(243C - 243Fh) */
    union  io_afdatamin    AFDATAMIN;  /* 2886_(2440 - 2443h) */
    union  io_afdatamh0    AFDATAMH0;  /* 2886_(2444 - 2447h) */
    union  io_afdatamh1    AFDATAMH1;  /* 2886_(2448 - 244Bh) */
    union  io_afdatamh2    AFDATAMH2;  /* 2886_(244C - 244Fh) */

    unsigned char dmy_2450_247F[0x2480-0x2450]; /* 2886_(2450 - 247Fh) */

};

#endif	// _IO_PRO_AF_WIN_H_
