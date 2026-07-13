/**
 * @file		__jdspro_dpc.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_DPC_H_
#define _IO_PRO_DPC_H_

/*  structure of DPCTRG (2840_9100h)    */
union io_dpctrg{
    unsigned long       word;
    struct {
        unsigned long   DPCTRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of DPCPAEN    (2840_9104h)    */
union io_dpcpaen{
    unsigned long       word;
    struct {
        unsigned long   DPAEN   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of DPCGWP (2840_9108h)    */
union io_dpcgwp{
    unsigned long       word;
    struct {
        unsigned long   DPCGH   :13;
        unsigned long           :3;
        unsigned long   DPCGV   :14;
        unsigned long           :2;
    }bit;
};

/*  structure of DPCGWS (2840_910Ch)    */
union io_dpcgws{
    unsigned long       word;
    struct {
        unsigned long   DPCGHW  :13;
        unsigned long           :3;
        unsigned long   DPCGVW  :14;
        unsigned long           :2;
    }bit;
};

/*  structure of DPCWP  (2840_9110h)    */
union io_dpcwp{
    unsigned long       word;
    struct {
        unsigned long   DPCH    :13;
        unsigned long           :3;
        unsigned long   DPCV    :14;
        unsigned long           :2;
    }bit;
};

/*  structure of DPCWS  (2840_9114h)    */
union io_dpcws{
    unsigned long       word;
    struct {
        unsigned long   DPCHW   :13;
        unsigned long           :3;
        unsigned long   DPCVW   :14;
        unsigned long           :2;
    }bit;
};

/*  structure of DPCMD  (2840_9118h)    */
union io_dpcmd{
    unsigned long       word;
    struct {
        unsigned long   DPCMD   :3;
        unsigned long           :1;
        unsigned long   DPCRTZ  :1;
        unsigned long           :27;
    }bit;
};

/*  structure of DPCBLP (2840_911Ch)    */
union io_dpcblp{
    unsigned long       word;
    struct {
        unsigned long   DPCBLSP :14;
        unsigned long           :2;
        unsigned long   DPCBLWP :4;
        unsigned long           :12;
    }bit;
};

/*  structure of DPCBLM (2840_9120h)    */
union io_dpcblm{
    unsigned long       word;
    struct {
        unsigned long   DPCBLSM :14;
        unsigned long           :2;
        unsigned long   DPCBLWM :4;
        unsigned long           :12;
    }bit;
};

/*  structure of DFCTL  (2840_9124h)    */
union io_dfctl{
    unsigned long       word;
    struct {
        unsigned long   DFGRGB  :1;
        unsigned long   DFMDG   :1;
        unsigned long   DFMDRB  :1;
        unsigned long           :1;
        unsigned long   DFDCNT  :3;
        unsigned long           :1;
        unsigned long   DFRBUG  :1;
        unsigned long   DFAGND  :1;
        unsigned long           :22;
    }bit;
};

/*  structure of DFS    (2840_9130h)    */
union io_dfs_1{
    unsigned long      word;
    struct {
        unsigned long  DFSRM   :14;
        unsigned long          :2;
        unsigned long  DFSRP   :14;
        unsigned long          :2;
    }bit;
};
union io_dfs_2{
    unsigned long      word;
    struct {
        unsigned long  DFSGM   :14;
        unsigned long          :2;
        unsigned long  DFSGP   :14;
        unsigned long          :2;
    }bit;
};
union io_dfs_3{
    unsigned long       word;
    struct {
        unsigned long   DFSBM   :14;
        unsigned long           :2;
        unsigned long   DFSBP   :14;
        unsigned long           :2;
    }bit;
};
struct io_dfs{
    union io_dfs_1  DFS1;
    union io_dfs_2  DFS2;
    union io_dfs_3  DFS3;
};

/*  structure of DFO    (2840_9140h)    */
union io_dfo_1{
    unsigned long      word;
    struct {
        unsigned long  DFORM   :14;
        unsigned long          :2;
        unsigned long  DFORP   :14;
        unsigned long          :2;
    }bit;
};
union io_dfo_2{
    unsigned long      word;
    struct {
        unsigned long  DFOGM   :14;
        unsigned long          :2;
        unsigned long  DFOGP   :14;
        unsigned long          :2;
    }bit;
};
union io_dfo_3{
    unsigned long       word;
    struct {
        unsigned long   DFOBM   :14;
        unsigned long           :2;
        unsigned long   DFOBP   :14;
        unsigned long           :2;
    }bit;
};
struct io_dfo{
    union io_dfo_1  DFO1;
    union io_dfo_2  DFO2;
    union io_dfo_3  DFO3;
};

/*  structure of DFK    (2840_9150h)    */
union io_dfk_1{
    unsigned long      word;
    struct {
        unsigned long  DFKRM   :6;
        unsigned long          :10;
        unsigned long  DFKRP   :6;
        unsigned long          :10;
    }bit;
};
union io_dfk_2{
    unsigned long      word;
    struct {
        unsigned long  DFKGM   :6;
        unsigned long          :10;
        unsigned long  DFKGP   :6;
        unsigned long          :10;
    }bit;
};
union io_dfk_3{
    unsigned long       word;
    struct {
        unsigned long   DFKBM   :6;
        unsigned long           :10;
        unsigned long   DFKBP   :6;
        unsigned long           :10;
    }bit;
};
struct io_dfk{
    union io_dfk_1  DFK1;
    union io_dfk_2  DFK2;
    union io_dfk_3  DFK3;
};

/*  structure of DFH    (2840_9160h)    */
union io_dfh_1{
    unsigned long      word;
    struct {
        unsigned long   DFHRM   :14;
        unsigned long           :2;
        unsigned long   DFHRP   :14;
        unsigned long           :2;
    }bit;
};
union io_dfh_2{
    unsigned long      word;
    struct {
        unsigned long   DFHGM   :14;
        unsigned long           :2;
        unsigned long   DFHGP   :14;
        unsigned long           :2;
    }bit;
};
union io_dfh_3{
    unsigned long       word;
    struct {
        unsigned long   DFHBM   :14;
        unsigned long           :2;
        unsigned long   DFHBP   :14;
        unsigned long           :2;
    }bit;
};
struct io_dfh{
    union io_dfh_1  DFH1;
    union io_dfh_2  DFH2;
    union io_dfh_3  DFH3;
};

/*  structure of DFCSR  (2840_9170h)    */
union io_dfcsr{
    unsigned long       word;
    struct {
        unsigned long   DFCSRM  :6;
        unsigned long           :2;
        unsigned long           :8;
        unsigned long   DFCSRP  :6;
        unsigned long           :10;
    }bit;
};

/*  structure of DFLSTH (2840_9178h)    */
union io_dflsth_1{
    unsigned long      word;
    struct {
        unsigned long   DFLSTHG :12;
        unsigned long           :4;
        unsigned long   DFLSTHR :12;
        unsigned long           :4;
    }bit;
};
union io_dflsth_2{
    unsigned long      word;
    struct {
        unsigned long           :8;
        unsigned long           :8;
        unsigned long   DFLSTHB :12;
        unsigned long           :4;
    }bit;
};
struct io_dflsth{
    union io_dflsth_1  DFLSTH1;
    union io_dflsth_2  DFLSTH2;
};

/*  structure of DFAGEF (2840_9180h)    */
union io_dfagef{
    unsigned long       word;
    struct {
        unsigned long   DFAGEFM :3;
        unsigned long           :5;
        unsigned long           :8;
        unsigned long   DFAGEFP :3;
        unsigned long           :13;
    }bit;
};

/*  structure of DFAGEF2    (2840_9184h)    */
union io_dfagef2{
    unsigned long       word;
    struct {
        unsigned long   DFAGEFM2    :2;
        unsigned long               :6;
        unsigned long               :8;
        unsigned long   DFAGEFP2    :2;
        unsigned long               :14;
    }bit;
};

/*  structure of DFAGTH (2840_9188h)    */
union io_dfagth{
    unsigned long       word;
    struct {
        unsigned long   DFAGTHM :10;
        unsigned long           :6;
        unsigned long   DFAGTHP :10;
        unsigned long           :6;
    }bit;
};

/*  structure of DFAGTHK    (2840_918Ch)    */
union io_dfagthk{
    unsigned long       word;
    struct {
        unsigned long   DFAGTHKM    :6;
        unsigned long               :2;
        unsigned long               :8;
        unsigned long   DFAGTHKP    :6;
        unsigned long               :10;
    }bit;
};

/*  structure of DMCTL  (2840_9190h)    */
union io_dmctl{
    unsigned long       word;
    struct {
        unsigned long   DMGUG   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of DMWKS  (2840_9194h)    */
union io_dmwks{
    unsigned long       word;
    struct {
        unsigned long   DMWKSM  :10;
        unsigned long           :6;
        unsigned long   DMWKSP  :10;
        unsigned long           :6;
    }bit;
};

/*  structure of DMWKD  (2840_9198h)    */
union io_dmwkd{
    unsigned long       word;
    struct {
        unsigned long   DMWKDM  :10;
        unsigned long           :6;
        unsigned long   DMWKDP  :10;
        unsigned long           :6;
    }bit;
};

/*  structure of DMAGK  (2840_919Ch)    */
union io_dmagk{
    unsigned long       word;
    struct {
        unsigned long   DMAGKM  :6;
        unsigned long           :2;
        unsigned long           :8;
        unsigned long   DMAGKP  :6;
        unsigned long           :10;
    }bit;
};

/*  structure of DECTL  (2840_91A0h)    */
union io_dectl{
    unsigned long       word;
    struct {
        unsigned long   DEGRGB  :2;
        unsigned long   DERBUG  :1;
        unsigned long   DEGUG   :1;
        unsigned long           :28;
    }bit;
};

/*  structure of DENSS  (2840_91A4h)    */
union io_denss{
    unsigned long       word;
    struct {
        unsigned long   DENSS   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of DENSO  (2840_91A8h)    */
union io_denso{
    unsigned long       word;
    struct {
        unsigned long   DENSO   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of DENSK  (2840_91ACh)    */
union io_densk{
    unsigned long       word;
    struct {
        unsigned long   DENSK   :8;
        unsigned long           :24;
    }bit;
};

/*  structure of DENSH  (2840_91B0h)    */
union io_densh{
    unsigned long       word;
    struct {
        unsigned long   DENSH   :14;
        unsigned long           :18;
    }bit;
};

/*  structure of DENSMGN    (2840_91B4h)    */
union io_densmgn{
    unsigned long       word;
    struct {
        unsigned long   DENSMGN :3;
        unsigned long           :29;
    }bit;
};

/*  structure of DENDCTH    (2840_91B8h)    */
union io_dendcth{
    unsigned long       word;
    struct {
        unsigned long   DENDCTH :14;
        unsigned long           :18;
    }bit;
};

/*  structure of DEAGK  (2840_91BCh)    */
union io_deagk{
    unsigned long       word;
    struct {
        unsigned long   DEAGKM  :6;
        unsigned long           :2;
        unsigned long           :8;
        unsigned long   DEAGKP  :6;
        unsigned long           :10;
    }bit;
};

/*  structure of DCAGEF (2840_91C0h)    */
union io_dcagef{
    unsigned long       word;
    struct {
        unsigned long   DCAGEFM :3;
        unsigned long           :5;
        unsigned long           :8;
        unsigned long   DCAGEFP :3;
        unsigned long           :13;
    }bit;
};

/*  structure of DCAGTH (2840_91C4h)    */
union io_dcagth{
    unsigned long       word;
    struct {
        unsigned long   DCAGTHM :14;
        unsigned long           :2;
        unsigned long   DCAGTHP :14;
        unsigned long           :2;
    }bit;
};

/* Define i/o mapping */
struct io_dpc{
    /* JDSIMG */
    union  io_dpctrg    DPCTRG;     /* 2840_(9100 - 9103h) */
    union  io_dpcpaen   DPCPAEN;    /* 2840_(9104 - 9107h) */
    union  io_dpcgwp    DPCGWP;     /* 2840_(9108 - 910Bh) */
    union  io_dpcgws    DPCGWS;     /* 2840_(910C - 910Fh) */
    union  io_dpcwp     DPCWP;      /* 2840_(9110 - 9113h) */
    union  io_dpcws     DPCWS;      /* 2840_(9114 - 9117h) */
    union  io_dpcmd     DPCMD;      /* 2840_(9118 - 911Bh) */
    union  io_dpcblp    DPCBLP;     /* 2840_(911C - 911Fh) */
    union  io_dpcblm    DPCBLM;     /* 2840_(9120 - 9123h) */
    union  io_dfctl     DFCTL;      /* 2840_(9124 - 9127h) */

    unsigned char dmy_9128_912F[0x9130-0x9128]; /* 2840_(9128 - 912Fh) */

    struct io_dfs       DFS;        /* 2840_(9130 - 913Bh) */

    unsigned char dmy_913C_913F[0x9140-0x913C]; /* 2840_(913C - 913Fh) */

    struct io_dfo       DFO;        /* 2840_(9140 - 914Bh) */

    unsigned char dmy_914C_914F[0x9150-0x914C]; /* 2840_(914C - 914Fh) */

    struct io_dfk       DFK;        /* 2840_(9150 - 915Bh) */

    unsigned char dmy_915C_915F[0x9160-0x915C]; /* 2840_(915C - 915Fh) */

    struct io_dfh       DFH;        /* 2840_(9160 - 916Bh) */

    unsigned char dmy_916C_916F[0x9170-0x916C]; /* 2840_(916C - 916Fh) */

    union  io_dfcsr     DFCSR;      /* 2840_(9170 - 9173h) */

    unsigned char dmy_9174_9177[0x9178-0x9174]; /* 2840_(9174 - 9177h) */

    struct io_dflsth    DFLSTH;     /* 2840_(9178 - 917Fh) */
    union  io_dfagef    DFAGEF;     /* 2840_(9180 - 9183h) */
    union  io_dfagef2   DFAGEF2;    /* 2840_(9184 - 9187h) */
    union  io_dfagth    DFAGTH;     /* 2840_(9188 - 918Bh) */
    union  io_dfagthk   DFAGTHK;    /* 2840_(918C - 918Fh) */
    union  io_dmctl     DMCTL;      /* 2840_(9190 - 9193h) */
    union  io_dmwks     DMWKS;      /* 2840_(9194 - 9197h) */
    union  io_dmwkd     DMWKD;      /* 2840_(9198 - 919Bh) */
    union  io_dmagk     DMAGK;      /* 2840_(919C - 919Fh) */
    union  io_dectl     DECTL;      /* 2840_(91A0 - 91A3h) */
    union  io_denss     DENSS;      /* 2840_(91A4 - 91A7h) */
    union  io_denso     DENSO;      /* 2840_(91A8 - 91ABh) */
    union  io_densk     DENSK;      /* 2840_(91AC - 91AFh) */
    union  io_densh     DENSH;      /* 2840_(91B0 - 91B3h) */
    union  io_densmgn   DENSMGN;    /* 2840_(91B4 - 91B7h) */
    union  io_dendcth   DENDCTH;    /* 2840_(91B8 - 91BBh) */
    union  io_deagk     DEAGK;      /* 2840_(91BC - 91BFh) */
    union  io_dcagef    DCAGEF;     /* 2840_(91C0 - 91C3h) */
    union  io_dcagth    DCAGTH;     /* 2840_(91C4 - 91C7h) */

    unsigned char dmy_91C8_91FF[0x9200-0x91C8]; /* 2840_(91C8 - 91FFh) */

};

#endif	// _IO_PRO_DPC_H_
