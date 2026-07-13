/**
 * @file		__jdspro_cag.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_CAG_H_
#define _IO_PRO_CAG_H_

/*  structure of CAGTRG (2840_9800h)    */
union io_cagtrg{
    unsigned long       word;
    struct {
        unsigned long   CAGTRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of CAGPAEN    (2840_9804h)    */
union io_cagpaen{
    unsigned long       word;
    struct {
        unsigned long   CPAEN   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of CAGGWP (2840_9808h)    */
union io_caggwp{
    unsigned long       word;
    struct {
        unsigned long   CAGGH   :13;
        unsigned long           :3;
        unsigned long   CAGGV   :14;
        unsigned long           :2;
    }bit;
};

/*  structure of CAGGWS (2840_980Ch)    */
union io_caggws{
    unsigned long       word;
    struct {
        unsigned long   CAGGHW  :13;
        unsigned long           :3;
        unsigned long   CAGGVW  :14;
        unsigned long           :2;
    }bit;
};

/*  structure of CAGWP  (2840_9810h)    */
union io_cagwp{
    unsigned long       word;
    struct {
        unsigned long   CAGH    :13;
        unsigned long           :3;
        unsigned long   CAGV    :14;
        unsigned long           :2;
    }bit;
};

/*  structure of CAGWS  (2840_9814h)    */
union io_cagws{
    unsigned long       word;
    struct {
        unsigned long   CAGHW   :13;
        unsigned long           :3;
        unsigned long   CAGVW   :14;
        unsigned long           :2;
    }bit;
};

/*  structure of CAGMD  (2840_9818h)    */
union io_cagmd{
    unsigned long       word;
    struct {
        unsigned long   GCAMD   :1;
        unsigned long           :3;
        unsigned long   CAGPDD  :1;
        unsigned long           :3;
        unsigned long   CAMD    :2;
        unsigned long           :2;
        unsigned long   CLPSEL  :1;
        unsigned long           :19;
    }bit;
};

/*  structure of RBR    (2840_981Ch)    */
union io_rbr{
    unsigned long       word;
    struct {
        unsigned long   RBRH    :12;
        unsigned long           :4;
        unsigned long   RBRV    :12;
        unsigned long           :4;
    }bit;
};

/*  structure of ABOARV (2840_9820h)    */
union io_aboarv{
    unsigned long       word;
    struct {
        long            ABOARV  :16;
        unsigned long           :16;
    }bit;
};

/*  structure of ABOARH (2840_9824h)    */
union io_aboarh{
    unsigned long       word;
    struct {
        long            ABOARH  :16;
        unsigned long           :16;
    }bit;
};

/*  structure of ABOAGV (2840_9828h)    */
union io_aboagv{
    unsigned long       word;
    struct {
        long            ABOAGV  :16;
        unsigned long           :16;
    }bit;
};

/*  structure of ABOAGH (2840_982Ch)    */
union io_aboagh{
    unsigned long       word;
    struct {
        long            ABOAGH  :16;
        unsigned long           :16;
    }bit;
};

/*  structure of ABOABV (2840_9830h)    */
union io_aboabv{
    unsigned long       word;
    struct {
        long            ABOABV  :16;
        unsigned long           :16;
    }bit;
};

/*  structure of ABOABH (2840_9834h)    */
union io_aboabh{
    unsigned long       word;
    struct {
        long            ABOABH  :16;
        unsigned long           :16;
    }bit;
};

/*  structure of ABGAR  (2840_9838h)    */
union io_abgar{
    unsigned long       word;
    struct {
        long            ABGAR   :13;
        unsigned long           :19;
    }bit;
};

/*  structure of ABGAG  (2840_983Ch)    */
union io_abgag{
    unsigned long       word;
    struct {
        long            ABGAG   :13;
        unsigned long           :19;
    }bit;
};

/*  structure of ABGAB  (2840_9840h)    */
union io_abgab{
    unsigned long       word;
    struct {
        long            ABGAB   :13;
        unsigned long           :19;
    }bit;
};

/*  structure of ABLRV  (2840_9844h)    */
union io_ablrv{
    unsigned long       word;
    struct {
        long            ABLRVU  :12;
        unsigned long           :4;
        long            ABLRVL  :12;
        unsigned long           :4;
    }bit;
};

/*  structure of ABLRH  (2840_9848h)    */
union io_ablrh{
    unsigned long       word;
    struct {
        long            ABLRHU  :13;
        unsigned long           :3;
        long            ABLRHL  :13;
        unsigned long           :3;
    }bit;
};

/*  structure of ABLGV  (2840_984Ch)    */
union io_ablgv{
    unsigned long       word;
    struct {
        long            ABLGVU  :12;
        unsigned long           :4;
        long            ABLGVL  :12;
        unsigned long           :4;
    }bit;
};

/*  structure of ABLGH  (2840_9850h)    */
union io_ablgh{
    unsigned long       word;
    struct {
        long            ABLGHU  :13;
        unsigned long           :3;
        long            ABLGHL  :13;
        unsigned long           :3;
    }bit;
};

/*  structure of ABLBV  (2840_9854h)    */
union io_ablbv{
    unsigned long       word;
    struct {
        long            ABLBVU  :12;
        unsigned long           :4;
        long            ABLBVL  :12;
        unsigned long           :4;
    }bit;
};

/*  structure of ABLBH  (2840_9858h)    */
union io_ablbh{
    unsigned long       word;
    struct {
        long            ABLBHU  :13;
        unsigned long           :3;
        long            ABLBHL  :13;
        unsigned long           :3;
    }bit;
};

/*  structure of ABOFSRV    (2840_985Ch)    */
union io_abofsrv{
    unsigned long       word;
    struct {
        long            ABOFSRV :11;
        unsigned long           :21;
    }bit;
};

/*  structure of ABOFSRH    (2840_9860h)    */
union io_abofsrh{
    unsigned long       word;
    struct {
        long            ABOFSRH :12;
        unsigned long           :20;
    }bit;
};

/*  structure of ABOFSGV    (2840_9864h)    */
union io_abofsgv{
    unsigned long       word;
    struct {
        long            ABOFSGV :11;
        unsigned long           :21;
    }bit;
};

/*  structure of ABOFSGH    (2840_9868h)    */
union io_abofsgh{
    unsigned long       word;
    struct {
        long            ABOFSGH :12;
        unsigned long           :20;
    }bit;
};

/*  structure of ABOFSBV    (2840_986Ch)    */
union io_abofsbv{
    unsigned long       word;
    struct {
        long            ABOFSBV :11;
        unsigned long           :21;
    }bit;
};

/*  structure of ABOFSBH    (2840_9870h)    */
union io_abofsbh{
    unsigned long       word;
    struct {
        long            ABOFSBH :12;
        unsigned long           :20;
    }bit;
};

/*  structure of ABNLTHR1   (2840_9874h)    */
union io_abnlthr1{
    unsigned long       word;
    struct {
        unsigned long   ABNLTHR1    :16;
        unsigned long               :16;
    }bit;
};

/*  structure of ABNLTHG1   (2840_9878h)    */
union io_abnlthg1{
    unsigned long       word;
    struct {
        unsigned long   ABNLTHG1    :16;
        unsigned long               :16;
    }bit;
};

/*  structure of ABNLTHB1   (2840_987Ch)    */
union io_abnlthb1{
    unsigned long       word;
    struct {
        unsigned long   ABNLTHB1    :16;
        unsigned long               :16;
    }bit;
};

/*  structure of ABNLGAR1   (2840_9880h)    */
union io_abnlgar1{
    unsigned long       word;
    struct {
        long            ABNLGAR1    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of ABNLGAG1   (2840_9884h)    */
union io_abnlgag1{
    unsigned long       word;
    struct {
        long            ABNLGAG1    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of ABNLGAB1   (2840_9888h)    */
union io_abnlgab1{
    unsigned long       word;
    struct {
        long            ABNLGAB1    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of ABNLTHR2   (2840_988Ch)    */
union io_abnlthr2{
    unsigned long       word;
    struct {
        unsigned long   ABNLTHR2    :16;
        unsigned long               :16;
    }bit;
};

/*  structure of ABNLTHG2   (2840_9890h)    */
union io_abnlthg2{
    unsigned long       word;
    struct {
        unsigned long   ABNLTHG2    :16;
        unsigned long               :16;
    }bit;
};

/*  structure of ABNLTHB2   (2840_9894h)    */
union io_abnlthb2{
    unsigned long       word;
    struct {
        unsigned long   ABNLTHB2    :16;
        unsigned long               :16;
    }bit;
};

/*  structure of ABNLGAR2   (2840_9898h)    */
union io_abnlgar2{
    unsigned long       word;
    struct {
        long            ABNLGAR2    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of ABNLGAG2   (2840_989Ch)    */
union io_abnlgag2{
    unsigned long       word;
    struct {
        long            ABNLGAG2    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of ABNLGAB2   (2840_98A0h)    */
union io_abnlgab2{
    unsigned long       word;
    struct {
        long            ABNLGAB2    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of ABNLTHR3   (2840_98A4h)    */
union io_abnlthr3{
    unsigned long       word;
    struct {
        unsigned long   ABNLTHR3    :16;
        unsigned long               :16;
    }bit;
};

/*  structure of ABNLTHG3   (2840_98A8h)    */
union io_abnlthg3{
    unsigned long       word;
    struct {
        unsigned long   ABNLTHG3    :16;
        unsigned long               :16;
    }bit;
};

/*  structure of ABNLTHB3   (2840_98ACh)    */
union io_abnlthb3{
    unsigned long       word;
    struct {
        unsigned long   ABNLTHB3    :16;
        unsigned long               :16;
    }bit;
};

/*  structure of ABNLGAR3   (2840_98B0h)    */
union io_abnlgar3{
    unsigned long       word;
    struct {
        long            ABNLGAR3    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of ABNLGAG3   (2840_98B4h)    */
union io_abnlgag3{
    unsigned long       word;
    struct {
        long            ABNLGAG3    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of ABNLGAB3   (2840_98B8h)    */
union io_abnlgab3{
    unsigned long       word;
    struct {
        long            ABNLGAB3    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of ABNLTHR4   (2840_98BCh)    */
union io_abnlthr4{
    unsigned long       word;
    struct {
        unsigned long   ABNLTHR4    :16;
        unsigned long               :16;
    }bit;
};

/*  structure of ABNLTHG4   (2840_98C0h)    */
union io_abnlthg4{
    unsigned long       word;
    struct {
        unsigned long   ABNLTHG4    :16;
        unsigned long               :16;
    }bit;
};

/*  structure of ABNLTHB4   (2840_98C4h)    */
union io_abnlthb4{
    unsigned long       word;
    struct {
        unsigned long   ABNLTHB4    :16;
        unsigned long               :16;
    }bit;
};

/*  structure of ABNLGAR4   (2840_98C8h)    */
union io_abnlgar4{
    unsigned long       word;
    struct {
        long            ABNLGAR4    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of ABNLGAG4   (2840_98CCh)    */
union io_abnlgag4{
    unsigned long       word;
    struct {
        long            ABNLGAG4    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of ABNLGAB4   (2840_98D0h)    */
union io_abnlgab4{
    unsigned long       word;
    struct {
        long            ABNLGAB4    :15;
        unsigned long               :17;
    }bit;
};

/*  structure of BFGLMT (2840_98D4h)    */
union io_bfglmt{
    unsigned long       word;
    struct {
        unsigned long   BFGLMTL :14;
        unsigned long           :2;
        unsigned long   BFGLMTH :14;
        unsigned long           :2;
    }bit;
};

/*  structure of BFGTH  (2840_98D8h)    */
union io_bfgth{
    unsigned long       word;
    struct {
        unsigned long   BFGTHK  :8;
        unsigned long           :8;
        unsigned long   BFGTHA  :14;
        unsigned long           :2;
    }bit;
};

/* Define i/o mapping */
struct io_cag{
    /* JDSIMG */
    union  io_cagtrg    CAGTRG;     /* 2840_(9800 - 9803h) */
    union  io_cagpaen   CAGPAEN;    /* 2840_(9804 - 9807h) */
    union  io_caggwp    CAGGWP;     /* 2840_(9808 - 980Bh) */
    union  io_caggws    CAGGWS;     /* 2840_(980C - 980Fh) */
    union  io_cagwp     CAGWP;      /* 2840_(9810 - 9813h) */
    union  io_cagws     CAGWS;      /* 2840_(9814 - 9817h) */
    union  io_cagmd     CAGMD;      /* 2840_(9818 - 981Bh) */
    union  io_rbr       RBR;        /* 2840_(981C - 981Fh) */
    union  io_aboarv    ABOARV;     /* 2840_(9820 - 9823h) */
    union  io_aboarh    ABOARH;     /* 2840_(9824 - 9827h) */
    union  io_aboagv    ABOAGV;     /* 2840_(9828 - 982Bh) */
    union  io_aboagh    ABOAGH;     /* 2840_(982C - 982Fh) */
    union  io_aboabv    ABOABV;     /* 2840_(9830 - 9833h) */
    union  io_aboabh    ABOABH;     /* 2840_(9834 - 9837h) */
    union  io_abgar     ABGAR;      /* 2840_(9838 - 983Bh) */
    union  io_abgag     ABGAG;      /* 2840_(983C - 983Fh) */
    union  io_abgab     ABGAB;      /* 2840_(9840 - 9843h) */
    union  io_ablrv     ABLRV;      /* 2840_(9844 - 9847h) */
    union  io_ablrh     ABLRH;      /* 2840_(9848 - 984Bh) */
    union  io_ablgv     ABLGV;      /* 2840_(984C - 984Fh) */
    union  io_ablgh     ABLGH;      /* 2840_(9850 - 9853h) */
    union  io_ablbv     ABLBV;      /* 2840_(9854 - 9857h) */
    union  io_ablbh     ABLBH;      /* 2840_(9858 - 985Bh) */
    union  io_abofsrv   ABOFSRV;    /* 2840_(985C - 985Fh) */
    union  io_abofsrh   ABOFSRH;    /* 2840_(9860 - 9863h) */
    union  io_abofsgv   ABOFSGV;    /* 2840_(9864 - 9867h) */
    union  io_abofsgh   ABOFSGH;    /* 2840_(9868 - 986Bh) */
    union  io_abofsbv   ABOFSBV;    /* 2840_(986C - 986Fh) */
    union  io_abofsbh   ABOFSBH;    /* 2840_(9870 - 9873h) */
    union  io_abnlthr1  ABNLTHR1;   /* 2840_(9874 - 9877h) */
    union  io_abnlthg1  ABNLTHG1;   /* 2840_(9878 - 987Bh) */
    union  io_abnlthb1  ABNLTHB1;   /* 2840_(987C - 987Fh) */
    union  io_abnlgar1  ABNLGAR1;   /* 2840_(9880 - 9883h) */
    union  io_abnlgag1  ABNLGAG1;   /* 2840_(9884 - 9887h) */
    union  io_abnlgab1  ABNLGAB1;   /* 2840_(9888 - 988Bh) */
    union  io_abnlthr2  ABNLTHR2;   /* 2840_(988C - 988Fh) */
    union  io_abnlthg2  ABNLTHG2;   /* 2840_(9890 - 9893h) */
    union  io_abnlthb2  ABNLTHB2;   /* 2840_(9894 - 9897h) */
    union  io_abnlgar2  ABNLGAR2;   /* 2840_(9898 - 989Bh) */
    union  io_abnlgag2  ABNLGAG2;   /* 2840_(989C - 989Fh) */
    union  io_abnlgab2  ABNLGAB2;   /* 2840_(98A0 - 98A3h) */
    union  io_abnlthr3  ABNLTHR3;   /* 2840_(98A4 - 98A7h) */
    union  io_abnlthg3  ABNLTHG3;   /* 2840_(98A8 - 98ABh) */
    union  io_abnlthb3  ABNLTHB3;   /* 2840_(98AC - 98AFh) */
    union  io_abnlgar3  ABNLGAR3;   /* 2840_(98B0 - 98B3h) */
    union  io_abnlgag3  ABNLGAG3;   /* 2840_(98B4 - 98B7h) */
    union  io_abnlgab3  ABNLGAB3;   /* 2840_(98B8 - 98BBh) */
    union  io_abnlthr4  ABNLTHR4;   /* 2840_(98BC - 98BFh) */
    union  io_abnlthg4  ABNLTHG4;   /* 2840_(98C0 - 98C3h) */
    union  io_abnlthb4  ABNLTHB4;   /* 2840_(98C4 - 98C7h) */
    union  io_abnlgar4  ABNLGAR4;   /* 2840_(98C8 - 98CBh) */
    union  io_abnlgag4  ABNLGAG4;   /* 2840_(98CC - 98CFh) */
    union  io_abnlgab4  ABNLGAB4;   /* 2840_(98D0 - 98D3h) */
    union  io_bfglmt    BFGLMT;     /* 2840_(98D4 - 98D7h) */
    union  io_bfgth     BFGTH;      /* 2840_(98D8 - 98DBh) */

    unsigned char dmy_98DC_98FF[0x9900-0x98DC]; /* 2840_(98DC - 98FFh) */

};

#endif	// _IO_PRO_CAG_H_
