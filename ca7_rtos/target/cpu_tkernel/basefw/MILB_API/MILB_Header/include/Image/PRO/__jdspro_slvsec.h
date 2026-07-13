/**
 * @file        __jdspro_slvsec.h
 * @brief       Definition JDSPRO Macro I/O register
 * @note        None
 * @attention   None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_SLVSEC_H_
#define _IO_PRO_SLVSEC_H_

/*  structure of SR  (2800_9000h)    */
union io_slvsec_sr{
    unsigned long       word;
    struct {
        unsigned long   INITA   :1;
        unsigned long           :3;
        unsigned long   UPDA    :1;
        unsigned long           :11;
        unsigned long   INITB   :1;
        unsigned long           :3;
        unsigned long   UPDB    :1;
        unsigned long           :11;
    }bit;
};

/*  structure of PHYCR0  (2800_9004h)    */
union io_slvsec_phycr0{
    unsigned long       word;
    struct {
        unsigned long   ICPCODE :4;
        unsigned long   REFSEL  :1;
        unsigned long           :27;
    }bit;
};

/*  structure of PHYCR1  (2800_9008h)    */
union io_slvsec_phycr1{
    unsigned long       word;
    struct {
        unsigned long   IFVDCNT :3;
        unsigned long           :5;
        unsigned long   INVDATA :8;
        unsigned long           :16;
    }bit;
};

/*  structure of PHYCR2  (2800_900Ch)    */
union io_slvsec_phycr2{
    unsigned long       word;
    struct {
        unsigned long   IRXPDX0 :1;
        unsigned long   IRXPDX1 :1;
        unsigned long   IRXPDX2 :1;
        unsigned long   IRXPDX3 :1;
        unsigned long   IRXPDX4 :1;
        unsigned long   IRXPDX5 :1;
        unsigned long   IRXPDX6 :1;
        unsigned long   IRXPDX7 :1;
        unsigned long   IPDPLLX :1;
        unsigned long           :3;
        unsigned long   PDX     :1;
        unsigned long           :19;
    }bit;
};

/*  structure of ATTR1  (2800_9014h)    */
union io_slvsec_attr1{
    unsigned long       word;
    struct {
        unsigned long   StandbySymbol		:8;
        unsigned long   StandbySymbolType	:1;
        unsigned long           			:7;
        unsigned long   StandbyLength		:8;
        unsigned long           			:8;
    }bit;
};

/*  structure of ATTR2  (2800_9018h)    */
union io_slvsec_attr2{
    unsigned long       word;
    struct {
        unsigned long   DeskewSymbol		:8;
        unsigned long   DeskewSymbolType	:1;
        unsigned long           			:7;
        unsigned long   DeskewLength		:8;
        unsigned long   DeskewInterval		:8;
    }bit;
};

/*  structure of ATTR3  (2800_901Ch)    */
union io_slvsec_attr3{
    unsigned long       word;
    struct {
        unsigned long   IdleSymbol0		:8;
        unsigned long   IdleSymbolType0	:1;
        unsigned long           		:7;
        unsigned long   IdleSymbol1		:8;
        unsigned long   IdleSymbolType1	:1;
        unsigned long           		:7;
    }bit;
};

/*  structure of ATTR4  (2800_9020h)    */
union io_slvsec_attr4{
    unsigned long       word;
    struct {
        unsigned long   IdleSymbol2		:8;
        unsigned long   IdleSymbolType2	:1;
        unsigned long           		:7;
        unsigned long   IdleSymbol3		:8;
        unsigned long   IdleSymbolType3	:1;
        unsigned long           		:7;
    }bit;
};

/*  structure of PEINTST  (2800_9024h)    */
union io_slvsec_peintst{
    unsigned long       word;
    struct {
        unsigned long   __IPC0	:1;
        unsigned long   __IPC1	:1;
        unsigned long   __IPC2	:1;
        unsigned long   __IPC3	:1;
        unsigned long   __IPC4	:1;
        unsigned long   __IPC5	:1;
        unsigned long   __IPC6	:1;
        unsigned long   __IPC7	:1;
        unsigned long   __RDE0	:1;
        unsigned long   __RDE1	:1;
        unsigned long   __RDE2	:1;
        unsigned long   __RDE3	:1;
        unsigned long   __RDE4	:1;
        unsigned long   __RDE5	:1;
        unsigned long   __RDE6	:1;
        unsigned long   __RDE7	:1;
        unsigned long   __IC0	:1;
        unsigned long   __IC1	:1;
        unsigned long   __IC2	:1;
        unsigned long   __IC3	:1;
        unsigned long   __IC4	:1;
        unsigned long   __IC5	:1;
        unsigned long   __IC6	:1;
        unsigned long   __IC7	:1;
        unsigned long           :8;
    }bit;
};

/*  structure of PEINTEN  (2800_9028h)    */
union io_slvsec_peinten{
    unsigned long       word;
    struct {
        unsigned long   IPC0	:1;
        unsigned long   IPC1	:1;
        unsigned long   IPC2	:1;
        unsigned long   IPC3	:1;
        unsigned long   IPC4	:1;
        unsigned long   IPC5	:1;
        unsigned long   IPC6	:1;
        unsigned long   IPC7	:1;
        unsigned long   RDE0	:1;
        unsigned long   RDE1	:1;
        unsigned long   RDE2	:1;
        unsigned long   RDE3	:1;
        unsigned long   RDE4	:1;
        unsigned long   RDE5	:1;
        unsigned long   RDE6	:1;
        unsigned long   RDE7	:1;
        unsigned long   IC0		:1;
        unsigned long   IC1		:1;
        unsigned long   IC2		:1;
        unsigned long   IC3		:1;
        unsigned long   IC4		:1;
        unsigned long   IC5		:1;
        unsigned long   IC6		:1;
        unsigned long   IC7		:1;
        unsigned long           :8;
    }bit;
};

/*  structure of RAMPD  (2800_9100h)    */
union io_slvsec_rampd{
    unsigned long       word;
    struct {
        unsigned long   RAMPD	:1;
        unsigned long           :31;
    }bit;
};

/*  structure of TRGR  (2800_9104h)    */
union io_slvsec_trgr{
    unsigned long       word;
    struct {
        unsigned long   VDEN	:1;
        unsigned long           :13;
        unsigned long   TRG		:2;
        unsigned long           :16;
    }bit;
};

/*  structure of INTST  (2800_9108h)    */
union io_slvsec_intst{
    unsigned long       word;
    struct {
        unsigned long   STBY	:1;
        unsigned long   RDY		:1;
        unsigned long           :2;
        unsigned long   FEO		:1;
        unsigned long   FEI		:1;
        unsigned long   FSO		:1;
        unsigned long   FSI		:1;
        unsigned long           :24;
    }bit;
};

/*  structure of INTEN  (2800_910Ch)    */
union io_slvsec_inten{
    unsigned long       word;
    struct {
        unsigned long   STBY	:1;
        unsigned long   RDY		:1;
        unsigned long           :2;
        unsigned long   FEO		:1;
        unsigned long   FEI		:1;
        unsigned long   FSO		:1;
        unsigned long   FSI		:1;
        unsigned long           :24;
    }bit;
};

/*  structure of LEINTST  (2800_9110h)    */
union io_slvsec_leintst{
    unsigned long       word;
    struct {
        unsigned long   LLE		:1;
        unsigned long   PCRCE	:1;
        unsigned long   HCRC1C	:1;
        unsigned long   HCRC2C	:1;
        unsigned long   HCRCE	:1;
        unsigned long   ECC1C	:1;
        unsigned long   ECC2C	:1;
        unsigned long   ECCE	:1;
        unsigned long   LNE		:1;
        unsigned long   LBOVF	:1;
        unsigned long           :22;
    }bit;
};

/*  structure of LEINTEN  (2800_9114h)    */
union io_slvsec_leinten{
    unsigned long       word;
    struct {
        unsigned long   LLE		:1;
        unsigned long   PCRCE	:1;
        unsigned long   HCRC1C	:1;
        unsigned long   HCRC2C	:1;
        unsigned long   HCRCE	:1;
        unsigned long   ECC1C	:1;
        unsigned long   ECC2C	:1;
        unsigned long   ECCE	:1;
        unsigned long   LNE		:1;
        unsigned long   LBOVF	:1;
        unsigned long           :22;
    }bit;
};

/*  structure of MEINTST0  (2800_9118h)    */
union io_slvsec_meintst0{
    unsigned long       word;
    struct {
        unsigned long   ECL		:1;
        unsigned long   		:7;
        unsigned long   SCL		:1;
        unsigned long   		:7;
        unsigned long   DCL		:1;
        unsigned long   		:7;
        unsigned long   DSD		:1;
        unsigned long   		:7;
    }bit;
};

/*  structure of MEINTST1  (2800_911Ch)    */
union io_slvsec_meintst1{
    unsigned long       word;
    struct {
        unsigned long   FOVF0	:1;
        unsigned long   FOVF1	:1;
        unsigned long   FOVF2	:1;
        unsigned long   FOVF3	:1;
        unsigned long   FOVF4	:1;
        unsigned long   FOVF5	:1;
        unsigned long   FOVF6	:1;
        unsigned long   FOVF7	:1;
        unsigned long   FUNF0	:1;
        unsigned long   FUNF1	:1;
        unsigned long   FUNF2	:1;
        unsigned long   FUNF3	:1;
        unsigned long   FUNF4	:1;
        unsigned long   FUNF5	:1;
        unsigned long   FUNF6	:1;
        unsigned long   FUNF7	:1;
        unsigned long   		:16;
    }bit;
};

/*  structure of MEINTEN0  (2800_9120h)    */
union io_slvsec_meinten0{
    unsigned long       word;
    struct {
        unsigned long   ECL		:1;
        unsigned long   		:7;
        unsigned long   SCL		:1;
        unsigned long   		:7;
        unsigned long   DCL		:1;
        unsigned long   		:7;
        unsigned long   DSD		:1;
        unsigned long   		:7;
    }bit;
};

/*  structure of MEINTEN1  (2800_9124h)    */
union io_slvsec_meinten1{
    unsigned long       word;
    struct {
        unsigned long   FOVF0	:1;
        unsigned long   FOVF1	:1;
        unsigned long   FOVF2	:1;
        unsigned long   FOVF3	:1;
        unsigned long   FOVF4	:1;
        unsigned long   FOVF5	:1;
        unsigned long   FOVF6	:1;
        unsigned long   FOVF7	:1;
        unsigned long   FUNF0	:1;
        unsigned long   FUNF1	:1;
        unsigned long   FUNF2	:1;
        unsigned long   FUNF3	:1;
        unsigned long   FUNF4	:1;
        unsigned long   FUNF5	:1;
        unsigned long   FUNF6	:1;
        unsigned long   FUNF7	:1;
        unsigned long   		:16;
    }bit;
};

/*  structure of CFGR0  (2800_9128h)    */
union io_slvsec_cfgr0{
    unsigned long       word;
    struct {
        unsigned long   LaneNum	:4;
        unsigned long   PixelBit:4;
        unsigned long   ECCCRC	:4;
        unsigned long   		:20;
    }bit;
};

/*  structure of CFGR1  (2800_912Ch)    */
union io_slvsec_cfgr1{
    unsigned long       word;
    struct {
        unsigned long   LineLength	:16;
        unsigned long   			:16;
    }bit;
};

/*  structure of OUTMD  (2800_9130h)    */
union io_slvsec_outmd{
    unsigned long       word;
    struct {
        unsigned long   OUTMD	:2;
        unsigned long   		:2;
        unsigned long   TRMG	:1;
        unsigned long   		:17;
    }bit;
};

/*  structure of LSWAP  (2800_9134h)    */
union io_slvsec_lswap{
    unsigned long       word;
    struct {
        unsigned long   L0OUT	:4;
        unsigned long   L1OUT	:4;
        unsigned long   L2OUT	:4;
        unsigned long   L3OUT	:4;
        unsigned long   L4OUT	:4;
        unsigned long   L5OUT	:4;
        unsigned long   L6OUT	:4;
        unsigned long   L7OUT	:4;
    }bit;
};

/*  structure of VLANE  (2800_9138h)    */
union io_slvsec_vlane{
    unsigned long       word;
    struct {
        unsigned long   LANE0	:1;
        unsigned long   LANE1	:1;
        unsigned long   LANE2	:1;
        unsigned long   LANE3	:1;
        unsigned long   LANE4	:1;
        unsigned long   LANE5	:1;
        unsigned long   LANE6	:1;
        unsigned long   LANE7	:1;
        unsigned long   		:24;
    }bit;
};

/*  structure of HSADD  (2800_913Ch)    */
union io_slvsec_hsadd{
    unsigned long       word;
    struct {
        unsigned long   HSADD	:16;
        unsigned long   		:16;
    }bit;
};

/*  structure of HSIZE  (2800_9140h)    */
union io_slvsec_hsize{
    unsigned long       word;
    struct {
        unsigned long   HSIZE	:16;
        unsigned long   		:16;
    }bit;
};

/*  structure of EBDST  (2800_9144h)    */
union io_slvsec_ebdst{
    unsigned long       word;
    struct {
        unsigned long   LNUM	:13;
        unsigned long   		:2;
        unsigned long   EBD		:1;
        unsigned long   		:16;
    }bit;
};

/*  structure of LNEST  (2800_9148h)    */
union io_slvsec_lnest{
    unsigned long       word;
    struct {
        unsigned long   DETLN	:13;
        unsigned long   		:3;
        unsigned long   EXPLN	:13;
        unsigned long   		:3;
    }bit;
};

/*  structure of MMODE  (2800_9150h)    */
union io_slvsec_mmode{
    unsigned long       word;
    struct {
        unsigned long   LNMAXCLP:1;
        unsigned long   		:31;
    }bit;
};

struct io_slvs_common{
    union  io_slvsec_sr			SR;				/* 2800_(9000 - 9003h) */
    union  io_slvsec_phycr0		PHYCR0;			/* 2800_(9004 - 9007h) */
    union  io_slvsec_phycr1		PHYCR1;			/* 2800_(9008 - 900Bh) */
    union  io_slvsec_phycr2		PHYCR2;			/* 2800_(900C - 900Fh) */

    unsigned char dmy_9010_9014[0x9014-0x9010];	/* 2840_(9010 - 9013h) */

    union  io_slvsec_attr1		ATTR1;			/* 2800_(9014 - 9017h) */
    union  io_slvsec_attr2		ATTR2;			/* 2800_(9018 - 901Bh) */
    union  io_slvsec_attr3		ATTR3;			/* 2800_(901C - 901Fh) */
    union  io_slvsec_attr4		ATTR4;			/* 2800_(9020 - 9023h) */
    union  io_slvsec_peintst	PEINTST;		/* 2800_(9024 - 9027h) */
    union  io_slvsec_peinten	PEINTEN;		/* 2800_(9028 - 902Bh) */

    unsigned char dmy_902C_9100[0x9100-0x902C];	/* 2800_(902C - 90FFh) */
};

struct io_slvs_stream{
    union  io_slvsec_rampd		RAMPD;			/* 2800_(9100 - 9103h) */
    union  io_slvsec_trgr		TRGR;			/* 2800_(9104 - 9107h) */
    union  io_slvsec_intst		INTST;			/* 2800_(9108 - 910Bh) */
    union  io_slvsec_inten		INTEN;			/* 2800_(910C - 910Fh) */
    union  io_slvsec_leintst	LEINTST;		/* 2800_(9110 - 9113h) */
    union  io_slvsec_leinten	LEINTEN;		/* 2800_(9114 - 9117h) */
    union  io_slvsec_meintst0	MEINTST0;		/* 2800_(9118 - 911Bh) */
    union  io_slvsec_meintst1	MEINTST1;		/* 2800_(911C - 911Fh) */
    union  io_slvsec_meinten0	MEINTEN0;		/* 2800_(9120 - 9123h) */
    union  io_slvsec_meinten1	MEINTEN1;		/* 2800_(9124 - 9127h) */
    union  io_slvsec_cfgr0		CFGR0;			/* 2800_(9128 - 912Bh) */
    union  io_slvsec_cfgr1		CFGR1;			/* 2800_(912C - 912Fh) */
    union  io_slvsec_outmd		OUTMD;			/* 2800_(9130 - 9133h) */
    union  io_slvsec_lswap		LSWAP;			/* 2800_(9134 - 9137h) */
    union  io_slvsec_vlane		VLANE;			/* 2800_(9138 - 913Bh) */
    union  io_slvsec_hsadd		HSADD;			/* 2800_(913C - 913Fh) */
    union  io_slvsec_hsize		HSIZE;			/* 2800_(9140 - 9143h) */
    union  io_slvsec_ebdst		EBDST;			/* 2800_(9144 - 9147h) */
    union  io_slvsec_lnest		LNEST;			/* 2800_(9148 - 914Bh) */
    unsigned char dmy_914C_9150[0x9150-0x914C];	/* 2800_(914C - 914Fh) */
    union  io_slvsec_mmode		MMODE;			/* 2800_(9150 - 9153h) */
    unsigned char dmy_9154_9200[0x9200-0x9154];	/* 2800_(9154 - 91FFh) */
};


/* Define i/o mapping */
struct io_slvs{
    /* JDSIMG */
    struct io_slvs_common		COMMON;			/* 2800_(9000 - 90FFh) */
	struct io_slvs_stream		STREAM[2];		/* 2800_(9100 - 91FFh) */
												/* 2800_(9200 - 92FFh) */
    unsigned char dmy_9300_93FF[0x9400-0x9300];	/* 2800_(9300 - 93FFh) */

};

#endif  // _IO_PRO_SLVSEC_H_
