/**
 * @file		__jdsltm1a_rbk.h
 * @brief		Definition JDSLTM1A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


/*	structure of RBKSR	(2841_3000h)	*/
union io_ltm_ch_rbk_rbksr{
	unsigned long		word;
	struct {
		unsigned long	SR	:1;
		unsigned long		:31;
	}bit;
};

/*	structure of RSRAMEN	(2841_3004h)	*/
union io_ltm_ch_rbk_rsramen{
	unsigned long		word;
	struct {
		unsigned long	SRAMAEN		:1;
		unsigned long				:3;
		unsigned long	RMAENLB1	:1;
		unsigned long	RMAENLB2	:1;
		unsigned long	RMAENHST	:1;
		unsigned long	RMAENSRO	:1;
		unsigned long	RMAENWCH	:1;
		unsigned long	RMAENRCH	:1;
		unsigned long				:22;
	}bit;
};

/*	structure of RTRG	(2841_3008h)	*/
union io_ltm_ch_rbk_rtrg{
	unsigned long		word;
	struct {
		unsigned long	TRG	:2;
		unsigned long		:30;
	}bit;
};

/*	structure of RFRSTP	(2841_300Ch)	*/
union io_ltm_ch_rbk_rfrstp{
	unsigned long		word;
	struct {
		unsigned long	FRSTP	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of RPARMSK	(2841_3010h)	*/
union io_ltm_ch_rbk_rparmsk{
	unsigned long		word;
	struct {
		unsigned long	PARMSK	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of RINT0	(2841_301Ch)	*/
union io_ltm_ch_rbk_rint0{
	unsigned long		word;
	struct {
		unsigned long	FINEN	:1;
		unsigned long			:3;
		unsigned long	FRMEN	:1;
		unsigned long			:3;
		unsigned long	HINIEN	:1;
		unsigned long			:3;
		unsigned long	LCNTEN	:1;
		unsigned long			:3;
		unsigned long	WAITEN	:1;
		unsigned long			:3;
		unsigned long	AXREEN	:1;
		unsigned long	AXWEEN	:1;
		unsigned long			:2;
		unsigned long	HSTEEN	:1;
		unsigned long			:7;
	}bit;
};

/*	structure of RINT1	(2841_3020h)	*/
union io_ltm_ch_rbk_rint1{
	unsigned long		word;
	struct {
		unsigned long	__FINFL		:1;
		unsigned long				:3;
		unsigned long	__FRMFL		:1;
		unsigned long				:3;
		unsigned long	__HINIFL	:1;
		unsigned long				:3;
		unsigned long	__LCNTFL	:1;
		unsigned long				:3;
		unsigned long	__WAITFL	:1;
		unsigned long				:3;
		unsigned long	__AXREFL	:1;
		unsigned long	__AXWEFL	:1;
		unsigned long				:2;
		unsigned long	__HSTEFL	:1;
		unsigned long				:7;
	}bit;
};

/*	structure of RLINTLV	(2841_3024h)	*/
union io_ltm_ch_rbk_rlintlv{
	unsigned long		word;
	struct {
		unsigned long	LINTLV	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of RAXICTLR1	(2841_3028h)	*/
union io_ltm_ch_rbk_raxictlr1{
	unsigned long		word;
	struct {
		unsigned long	ARCACHE_R	:4;
		unsigned long	ARPROT_R	:3;
		unsigned long				:1;
		unsigned long	ARCACHE_G	:4;
		unsigned long	ARPROT_G	:3;
		unsigned long				:1;
		unsigned long	ARCACHE_B	:4;
		unsigned long	ARPROT_B	:3;
		unsigned long				:9;
	}bit;
};

/*	structure of RAXIRERR	(2841_302Ch)	*/
union io_ltm_ch_rbk_raxirerr{
	unsigned long		word;
	struct {
		unsigned long	RRESP_R	:2;
		unsigned long	RRESP_G	:2;
		unsigned long	RRESP_B	:2;
		unsigned long			:26;
	}bit;
};

/*	structure of RAXICTLW1	(2841_3030h)	*/
union io_ltm_ch_rbk_raxictlw1{
	unsigned long		word;
	struct {
		unsigned long	AWCACHE	:4;
		unsigned long	AWPROT	:3;
		unsigned long			:25;
	}bit;
};

/*	structure of RAXIWERR	(2841_3034h)	*/
union io_ltm_ch_rbk_raxiwerr{
	unsigned long		word;
	struct {
		unsigned long	WRESP	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of RAXIBSIZ	(2841_3038h)	*/
union io_ltm_ch_rbk_raxibsiz{
	unsigned long		word;
	struct {
		unsigned long	BURSTRR	:1;
		unsigned long	BURSTRG	:1;
		unsigned long	BURSTRB	:1;
		unsigned long			:5;
		unsigned long	BURSTW	:1;
		unsigned long			:7;
		unsigned long	WSTBMSK	:1;
		unsigned long			:15;
	}bit;
};

/*	structure of RSDRHSZ	(2841_3080h)	*/
union io_ltm_ch_rbk_rsdrhsz{
	unsigned long		word;
	struct {
		unsigned long	SDRHSZ	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of RSDRDEF	(2841_3084h)	*/
union io_ltm_ch_rbk_rsdrdef{
	unsigned long		word;
	struct {
		unsigned long	SDRDEF	:16;
		unsigned long			:16;
	}bit;
};

/*	structure of RSDRVSZ	(2841_3088h)	*/
union io_ltm_ch_rbk_rsdrvsz{
	unsigned long		word;
	struct {
		unsigned long	SDRVSZ	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of RSDRADR	(2841_308Ch)	*/
union io_ltm_ch_rbk_rsdradr{
	unsigned long		word;
	struct {
		unsigned long	SDRADR	:32;
	}bit;
};

/*	structure of RSDRADG	(2841_3090h)	*/
union io_ltm_ch_rbk_rsdradg{
	unsigned long		word;
	struct {
		unsigned long	SDRADG	:32;
	}bit;
};

/*	structure of RSDRADB	(2841_3094h)	*/
union io_ltm_ch_rbk_rsdradb{
	unsigned long		word;
	struct {
		unsigned long	SDRADB	:32;
	}bit;
};

/*	structure of RSDWAD	(2841_3098h)	*/
union io_ltm_ch_rbk_rsdwad{
	unsigned long		word;
	struct {
		unsigned long	SDWAD	:32;
	}bit;
};

/*	structure of RSDWDEF	(2841_309Ch)	*/
union io_ltm_ch_rbk_rsdwdef{
	unsigned long		word;
	struct {
		unsigned long	SDWDEF	:16;
		unsigned long			:16;
	}bit;
};

/*	structure of RIOCTL	(2841_30A0h)	*/
union io_ltm_ch_rbk_rioctl{
	unsigned long		word;
	struct {
		unsigned long	INSEL	:2;
		unsigned long			:2;
		unsigned long	BAYER0	:2;
		unsigned long	BAYER1	:2;
		unsigned long			:4;
		unsigned long	SROMD	:1;
		unsigned long			:3;
		unsigned long	OUTSEL	:1;
		unsigned long			:3;
		unsigned long	W0DTYP	:2;
		unsigned long			:10;
	}bit;
};

/*	structure of BTHSTA0	(2841_30A4h)	*/
union io_ltm_ch_rbk_bthsta0{
	unsigned long		word;
	struct {
		unsigned long	BTHSTA0	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of BTVSTA0	(2841_30A8h)	*/
union io_ltm_ch_rbk_btvsta0{
	unsigned long		word;
	struct {
		unsigned long	BTVSTA0	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of BTHSIZ0	(2841_30ACh)	*/
union io_ltm_ch_rbk_bthsiz0{
	unsigned long		word;
	struct {
		unsigned long	BTHSIZ0	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of BTVSIZ0	(2841_30B0h)	*/
union io_ltm_ch_rbk_btvsiz0{
	unsigned long		word;
	struct {
		unsigned long	BTVSIZ0	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of BTHSTA1	(2841_30B4h)	*/
union io_ltm_ch_rbk_bthsta1{
	unsigned long		word;
	struct {
		unsigned long	BTHSTA1	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of BTVSTA1	(2841_30B8h)	*/
union io_ltm_ch_rbk_btvsta1{
	unsigned long		word;
	struct {
		unsigned long	BTVSTA1	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of BTHSIZ1	(2841_30BCh)	*/
union io_ltm_ch_rbk_bthsiz1{
	unsigned long		word;
	struct {
		unsigned long	BTHSIZ1	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of BTVSIZ1	(2841_30C0h)	*/
union io_ltm_ch_rbk_btvsiz1{
	unsigned long		word;
	struct {
		unsigned long	BTVSIZ1	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of RSFEN	(2841_30C4h)	*/
union io_ltm_ch_rbk_rsfen{
	unsigned long		word;
	struct {
		unsigned long	RSFEN0	:1;
		unsigned long	RSFEN1	:1;
		unsigned long			:2;
		unsigned long	RSFEN	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of HVEN	(2841_30C8h)	*/
union io_ltm_ch_rbk_hven{
	unsigned long		word;
	struct {
		unsigned long	HVEN	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of RHSTA	(2841_30CCh)	*/
union io_ltm_ch_rbk_rhsta{
	unsigned long		word;
	struct {
		unsigned long	HSTA	:20;
		unsigned long			:12;
	}bit;
};

/*	structure of RHPIT	(2841_30D0h)	*/
union io_ltm_ch_rbk_rhpit{
	unsigned long		word;
	struct {
		unsigned long	HPIT	:16;
		unsigned long			:16;
	}bit;
};

/*	structure of RVSTA	(2841_30D4h)	*/
union io_ltm_ch_rbk_rvsta{
	unsigned long		word;
	struct {
		unsigned long	VSTA	:20;
		unsigned long			:12;
	}bit;
};

/*	structure of RVPIT	(2841_30D8h)	*/
union io_ltm_ch_rbk_rvpit{
	unsigned long		word;
	struct {
		unsigned long	VPIT	:16;
		unsigned long			:16;
	}bit;
};

/*	structure of RDCMD	(2841_30DCh)	*/
union io_ltm_ch_rbk_rdcmd{
	unsigned long		word;
	struct {
		unsigned long	RDCEN	:1;
		unsigned long			:3;
		unsigned long	RDCMD	:2;
		unsigned long			:26;
	}bit;
};

/*	structure of RTEN	(2841_30E0h)	*/
union io_ltm_ch_rbk_rten{
	unsigned long		word;
	struct {
		unsigned long	RTEN	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of RTHSTA	(2841_30E4h)	*/
union io_ltm_ch_rbk_rthsta{
	unsigned long		word;
	struct {
		unsigned long	RTHSTA	:10;
		unsigned long			:22;
	}bit;
};

/*	structure of RTVSTA	(2841_30E8h)	*/
union io_ltm_ch_rbk_rtvsta{
	unsigned long		word;
	struct {
		unsigned long	RTVSTA	:10;
		unsigned long			:22;
	}bit;
};

/*	structure of RTHSIZ	(2841_30ECh)	*/
union io_ltm_ch_rbk_rthsiz{
	unsigned long		word;
	struct {
		unsigned long	RTHSIZ	:10;
		unsigned long			:22;
	}bit;
};

/*	structure of RTVSIZ	(2841_30F0h)	*/
union io_ltm_ch_rbk_rtvsiz{
	unsigned long		word;
	struct {
		unsigned long	RTVSIZ	:10;
		unsigned long			:22;
	}bit;
};

/* Define i/o mapping */
struct io_ltm_ch_rbk{
	/* JDSLTM */
	union	io_ltm_ch_rbk_rbksr			RBKSR;		/* 2841_(3000 - 3003h) */
	union	io_ltm_ch_rbk_rsramen		RSRAMEN;	/* 2841_(3004 - 3007h) */
	union	io_ltm_ch_rbk_rtrg			RTRG;		/* 2841_(3008 - 300Bh) */
	union	io_ltm_ch_rbk_rfrstp		RFRSTP;		/* 2841_(300C - 300Fh) */
	union	io_ltm_ch_rbk_rparmsk		RPARMSK;	/* 2841_(3010 - 3013h) */

	unsigned char dmy_3014_301B[0x301C-0x3014];		/* 2841_(3014 - 301Bh) */

	union	io_ltm_ch_rbk_rint0			RINT0;		/* 2841_(301C - 301Fh) */
	union	io_ltm_ch_rbk_rint1			RINT1;		/* 2841_(3020 - 3023h) */
	union	io_ltm_ch_rbk_rlintlv		RLINTLV;	/* 2841_(3024 - 3027h) */
	union	io_ltm_ch_rbk_raxictlr1		RAXICTLR1;	/* 2841_(3028 - 302Bh) */
	union	io_ltm_ch_rbk_raxirerr		RAXIRERR;	/* 2841_(302C - 302Fh) */
	union	io_ltm_ch_rbk_raxictlw1		RAXICTLW1;	/* 2841_(3030 - 3033h) */
	union	io_ltm_ch_rbk_raxiwerr		RAXIWERR;	/* 2841_(3034 - 3037h) */
	union	io_ltm_ch_rbk_raxibsiz		RAXIBSIZ;	/* 2841_(3038 - 303Bh) */

	unsigned char dmy_303C_307F[0x3080-0x303C];		/* 2841_(303C - 307Fh) */

	union	io_ltm_ch_rbk_rsdrhsz		RSDRHSZ;	/* 2841_(3080 - 3083h) */
	union	io_ltm_ch_rbk_rsdrdef		RSDRDEF;	/* 2841_(3084 - 3087h) */
	union	io_ltm_ch_rbk_rsdrvsz		RSDRVSZ;	/* 2841_(3088 - 308Bh) */
	union	io_ltm_ch_rbk_rsdradr		RSDRADR;	/* 2841_(308C - 308Fh) */
	union	io_ltm_ch_rbk_rsdradg		RSDRADG;	/* 2841_(3090 - 3093h) */
	union	io_ltm_ch_rbk_rsdradb		RSDRADB;	/* 2841_(3094 - 3097h) */
	union	io_ltm_ch_rbk_rsdwad		RSDWAD;		/* 2841_(3098 - 309Bh) */
	union	io_ltm_ch_rbk_rsdwdef		RSDWDEF;	/* 2841_(309C - 309Fh) */
	union	io_ltm_ch_rbk_rioctl		RIOCTL;		/* 2841_(30A0 - 30A3h) */
	union	io_ltm_ch_rbk_bthsta0		BTHSTA0;	/* 2841_(30A4 - 30A7h) */
	union	io_ltm_ch_rbk_btvsta0		BTVSTA0;	/* 2841_(30A8 - 30ABh) */
	union	io_ltm_ch_rbk_bthsiz0		BTHSIZ0;	/* 2841_(30AC - 30AFh) */
	union	io_ltm_ch_rbk_btvsiz0		BTVSIZ0;	/* 2841_(30B0 - 30B3h) */
	union	io_ltm_ch_rbk_bthsta1		BTHSTA1;	/* 2841_(30B4 - 30B7h) */
	union	io_ltm_ch_rbk_btvsta1		BTVSTA1;	/* 2841_(30B8 - 30BBh) */
	union	io_ltm_ch_rbk_bthsiz1		BTHSIZ1;	/* 2841_(30BC - 30BFh) */
	union	io_ltm_ch_rbk_btvsiz1		BTVSIZ1;	/* 2841_(30C0 - 30C3h) */
	union	io_ltm_ch_rbk_rsfen			RSFEN;		/* 2841_(30C4 - 30C7h) */
	union	io_ltm_ch_rbk_hven			HVEN;		/* 2841_(30C8 - 30CBh) */
	union	io_ltm_ch_rbk_rhsta			RHSTA;		/* 2841_(30CC - 30CFh) */
	union	io_ltm_ch_rbk_rhpit			RHPIT;		/* 2841_(30D0 - 30D3h) */
	union	io_ltm_ch_rbk_rvsta			RVSTA;		/* 2841_(30D4 - 30D7h) */
	union	io_ltm_ch_rbk_rvpit			RVPIT;		/* 2841_(30D8 - 30DBh) */
	union	io_ltm_ch_rbk_rdcmd			RDCMD;		/* 2841_(30DC - 30DFh) */
	union	io_ltm_ch_rbk_rten			RTEN;		/* 2841_(30E0 - 30E3h) */
	union	io_ltm_ch_rbk_rthsta		RTHSTA;		/* 2841_(30E4 - 30E7h) */
	union	io_ltm_ch_rbk_rtvsta		RTVSTA;		/* 2841_(30E8 - 30EBh) */
	union	io_ltm_ch_rbk_rthsiz		RTHSIZ;		/* 2841_(30EC - 30EFh) */
	union	io_ltm_ch_rbk_rtvsiz		RTVSIZ;		/* 2841_(30F0 - 30F3h) */

	unsigned char dmy_30F4_31FF[0x3200-0x30F4];		/* 2841_(30F4 - 31FFh) */

};
