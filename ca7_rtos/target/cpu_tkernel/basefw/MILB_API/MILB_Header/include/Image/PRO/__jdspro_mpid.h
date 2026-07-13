/**
 * @file		__jdspro_mpid.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_MPID_H_
#define _IO_PRO_MPID_H_

/*  structure of MPISR  (2800_9800h)    */
union io_mpisr{
    unsigned long       word;
    struct {
        unsigned long   SR  :1;
        unsigned long       :31;
    }bit;
};

/*  structure of MPITRG (2800_9804h)    */
union io_mpitrg{
    unsigned long       word;
    struct {
        unsigned long   MPITRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of MPIPAEN0   (2800_9808h)    */
union io_paen0{
    unsigned long       word;
    struct {
        unsigned long   PAEN0   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of MPIPAEN1   (2800_980Ch)    */
union io_paen1{
    unsigned long       word;
    struct {
        unsigned long   PAEN1   :1;
        unsigned long           :31;
    }bit;
};

/*  structure of MPIINTE    (2800_9810h)    */
union io_mpiinte{
    unsigned long       word;
    struct {
        unsigned long   DECE3   :1;
        unsigned long   DECE2   :1;
        unsigned long   DECE1   :1;
        unsigned long   DECE0   :1;
        unsigned long   DEEE3   :1;
        unsigned long   DEEE2   :1;
        unsigned long   DEEE1   :1;
        unsigned long   DEEE0   :1;
        unsigned long   DESSHE3 :1;
        unsigned long   DESSHE2 :1;
        unsigned long   DESSHE1 :1;
        unsigned long   DESSHE0 :1;
        unsigned long   DESHE3  :1;
        unsigned long   DESHE2  :1;
        unsigned long   DESHE1  :1;
        unsigned long   DESHE0  :1;
        unsigned long   FEO1E   :1;
        unsigned long   FEO0E   :1;
        unsigned long   FEI1E   :1;
        unsigned long   FEI0E   :1;
        unsigned long   FSO1E   :1;
        unsigned long   FSO0E   :1;
        unsigned long   FSI1E   :1;
        unsigned long   FSI0E   :1;
        unsigned long   LZEE3   :1;
        unsigned long   LZEE2   :1;
        unsigned long   LZEE1   :1;
        unsigned long   LZEE0   :1;
        unsigned long   EFSE    :1;
        unsigned long   CRCEE   :1;
        unsigned long   ECCWE   :1;
        unsigned long   ECCEE   :1;
    }bit;
};

/*  structure of MPIINTF    (2800_9814h)    */
union io_mpiintf{
    unsigned long       word;
    struct {
        unsigned long   __DECF3   :1;
        unsigned long   __DECF2   :1;
        unsigned long   __DECF1   :1;
        unsigned long   __DECF0   :1;
        unsigned long   __DEEF3   :1;
        unsigned long   __DEEF2   :1;
        unsigned long   __DEEF1   :1;
        unsigned long   __DEEF0   :1;
        unsigned long   __DESSHF3 :1;
        unsigned long   __DESSHF2 :1;
        unsigned long   __DESSHF1 :1;
        unsigned long   __DESSHF0 :1;
        unsigned long   __DESHF3  :1;
        unsigned long   __DESHF2  :1;
        unsigned long   __DESHF1  :1;
        unsigned long   __DESHF0  :1;
        unsigned long   __FEO1F   :1;
        unsigned long   __FEO0F   :1;
        unsigned long   __FEI1F   :1;
        unsigned long   __FEI0F   :1;
        unsigned long   __FSO1F   :1;
        unsigned long   __FSO0F   :1;
        unsigned long   __FSI1F   :1;
        unsigned long   __FSI0F   :1;
        unsigned long   __LZEF3   :1;
        unsigned long   __LZEF2   :1;
        unsigned long   __LZEF1   :1;
        unsigned long   __LZEF0   :1;
        unsigned long   __EFSF    :1;
        unsigned long   __CRCEF   :1;
        unsigned long   __ECCWF   :1;
        unsigned long   __ECCEF   :1;
    }bit;
};

/*  structure of MPILMODE   (2800_9818h)    */
union io_lmode{
    unsigned long       word;
    struct {
        unsigned long   LANE    :2;
        unsigned long           :30;
    }bit;
};

/*  structure of MPIIOMD0   (2800_981Ch)    */
union io_iomd0{
    unsigned long       word;
    struct {
        unsigned long   DTMD0   :3;
        unsigned long           :1;
        unsigned long   OUTMD0  :3;
        unsigned long           :1;
        unsigned long   VCOUT0  :3;
        unsigned long           :1;
        unsigned long   DTCOMD0 :1;
        unsigned long           :3;
        unsigned long   DTCO0   :6;
        unsigned long           :10;
    }bit;
};

/*  structure of MPIIOMD1   (2800_9820h)    */
union io_iomd1{
    unsigned long       word;
    struct {
        unsigned long   DTMD1   :3;
        unsigned long           :1;
        unsigned long   OUTMD1  :3;
        unsigned long           :1;
        unsigned long   VCOUT1  :3;
        unsigned long           :1;
        unsigned long   DTCOMD1 :1;
        unsigned long           :3;
        unsigned long   DTCO1   :6;
        unsigned long           :10;
    }bit;
};

/*  structure of MPIDPI (2800_9824h)    */
union io_dpi{
    unsigned long       word;
    struct {
        unsigned long   DPEN    :1;
        unsigned long           :3;
        unsigned long   DPSMTC  :4;
        unsigned long   DPSMTD  :4;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long   DPSM    :2;
        unsigned long           :16;
    }bit;
};

/*  structure of MPIDPVR    (2800_9828h)    */
union io_dpvr{
    unsigned long       word;
    struct {
        unsigned long           :27;
        unsigned long   SSC     :1;
        unsigned long   SSD3    :1;
        unsigned long   SSD2    :1;
        unsigned long   SSD1    :1;
        unsigned long   SSD0    :1;
    }bit;
};

/*  structure of MPISCS (2800_982Ch)    */
union io_scs{
    unsigned long       word;
    struct {
        unsigned long           :28;
        unsigned long   SCH3    :1;
        unsigned long   SCH2    :1;
        unsigned long   SCH1    :1;
        unsigned long   SCH0    :1;
    }bit;
};

/*  structure of MPIHDPDSEL (2800_9830h)    */
union io_hdpdsel{
    unsigned long       word;
    struct {
        unsigned long   HDPDSEL0    :2;
        unsigned long               :2;
        unsigned long   HDPDSEL1    :2;
        unsigned long               :26;
    }bit;
};

/* Define i/o mapping */
struct io_mpid{
    /* JDSIMG */
    union  io_mpisr       MPISR;      /* 2800_(9800 - 9803h) */
    union  io_mpitrg      MPITRG;     /* 2800_(9804 - 9807h) */
    union  io_paen0       MPIPAEN0;   /* 2800_(9808 - 980Bh) */
    union  io_paen1       MPIPAEN1;   /* 2800_(980C - 980Fh) */
    union  io_mpiinte     MPIINTE;    /* 2800_(9810 - 9813h) */
    union  io_mpiintf     MPIINTF;    /* 2800_(9814 - 9817h) */
    union  io_lmode       MPILMODE;   /* 2800_(9818 - 981Bh) */
    union  io_iomd0       MPIIOMD0;   /* 2800_(981C - 981Fh) */
    union  io_iomd1       MPIIOMD1;   /* 2800_(9820 - 9823h) */
    union  io_dpi         MPIDPI;     /* 2800_(9824 - 9827h) */
    union  io_dpvr        MPIDPVR;    /* 2800_(9828 - 982Bh) */
    union  io_scs         MPISCS;     /* 2800_(982C - 982Fh) */
    union  io_hdpdsel     MPIHDPDSEL; /* 2800_(9830 - 9833h) */

    unsigned char dmy_9834_98FF[0x9900-0x9834]; /* 2800_(9834 - 98FFh) */

};

#endif	// _IO_PRO_MPID_H_
