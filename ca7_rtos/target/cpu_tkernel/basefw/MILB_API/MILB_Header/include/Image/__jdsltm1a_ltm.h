/**
 * @file		__jdsltm1a_ltm.h
 * @brief		Definition JDSLTM1A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


/*	structure of LTMSR	(2841_3800h)	*/
union io_ltm_ch_ltm_ltmsr{
	unsigned long		word;
	struct {
		unsigned long	SR	:1;
		unsigned long		:31;
	}bit;
};

/*	structure of LSRAMEN	(2841_3804h)	*/
union io_ltm_ch_ltm_lsramen{
	unsigned long		word;
	struct {
		unsigned long	SRAMAEN		:1;
		unsigned long				:3;
		unsigned long	RMAENRGB	:1;
		unsigned long	RMAENYN		:1;
		unsigned long				:26;
	}bit;
};

/*	structure of LTRG	(2841_3808h)	*/
union io_ltm_ch_ltm_ltrg{
	unsigned long		word;
	struct {
		unsigned long	TRG	:2;
		unsigned long		:30;
	}bit;
};

/*	structure of LFRSTP (2841_380Ch)	*/
union io_ltm_ch_ltm_lfrstp{
	unsigned long		word;
	struct {
		unsigned long	FRSTP	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of LPARMSK	(2841_3810h)	*/
union io_ltm_ch_ltm_lparmsk{
	unsigned long		word;
	struct {
		unsigned long	PARMSK	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of LINT0	(2841_3818h)	*/
union io_ltm_ch_ltm_lint0{
	unsigned long		word;
	struct {
		unsigned long	FINEN	:1;
		unsigned long			:3;
		unsigned long	EXIPEN	:1;
		unsigned long			:7;
		unsigned long	LCNTEN	:1;
		unsigned long			:7;
		unsigned long	AXREEN	:1;
		unsigned long	AXWEEN	:1;
		unsigned long			:3;
		unsigned long	RAMEEN	:1;
		unsigned long			:6;
	}bit;
};

/*	structure of LINT1	(2841_381Ch)	*/
union io_ltm_ch_ltm_lint1{
	unsigned long		word;
	struct {
		unsigned long	__FINFL		:1;
		unsigned long				:3;
		unsigned long	__EXIPFL	:1;
		unsigned long				:7;
		unsigned long	__LCNTFL	:1;
		unsigned long				:7;
		unsigned long	__AXREFL	:1;
		unsigned long	__AXWEFL	:1;
		unsigned long				:3;
		unsigned long	__RAMEFL	:1;
		unsigned long				:6;
	}bit;
};

/*	structure of LLINTLV	(2841_3820h)	*/
union io_ltm_ch_ltm_llintlv{
	unsigned long		word;
	struct {
		unsigned long	LINTLV	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of LAXICTLR1	(2841_3824h)	*/
union io_ltm_ch_ltm_laxictlr1{
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

/*	structure of LAXICTLR2	(2841_3828h)	*/
union io_ltm_ch_ltm_laxictlr2{
	unsigned long		word;
	struct {
		unsigned long	ARCACHE_YP	:4;
		unsigned long	ARPROT_YP	:3;
		unsigned long				:1;
		unsigned long	ARCACHE_YN	:4;
		unsigned long	ARPROT_YN	:3;
		unsigned long				:17;
	}bit;
};

/*	structure of LAXIRERR	(2841_382Ch)	*/
union io_ltm_ch_ltm_laxirerr{
	unsigned long		word;
	struct {
		unsigned long	RRESP_R		:2;
		unsigned long	RRESP_G		:2;
		unsigned long	RRESP_B		:2;
		unsigned long				:2;
		unsigned long	RRESP_YP	:2;
		unsigned long	RRESP_YN	:2;
		unsigned long				:20;
	}bit;
};

/*	structure of LAXICTLW	(2841_3830h)	*/
union io_ltm_ch_ltm_laxictlw{
	unsigned long		word;
	struct {
		unsigned long	AWCACHE_R	:4;
		unsigned long	AWPROT_R	:3;
		unsigned long				:1;
		unsigned long	AWCACHE_G	:4;
		unsigned long	AWPROT_G	:3;
		unsigned long				:1;
		unsigned long	AWCACHE_B	:4;
		unsigned long	AWPROT_B	:3;
		unsigned long				:9;
	}bit;
};

/*	structure of LAXIWERR	(2841_3834h)	*/
union io_ltm_ch_ltm_laxiwerr{
	unsigned long		word;
	struct {
		unsigned long	WRESP_R	:2;
		unsigned long	WRESP_G	:2;
		unsigned long	WRESP_B	:2;
		unsigned long			:26;
	}bit;
};

/*	structure of LAXIBSIZ	(2841_3838h)	*/
union io_ltm_ch_ltm_laxibsiz{
	unsigned long		word;
	struct {
		unsigned long	BURSTRR		:1;
		unsigned long	BURSTRG		:1;
		unsigned long	BURSTRB		:1;
		unsigned long	BURSTRYP	:1;
		unsigned long	BURSTRYN	:1;
		unsigned long				:3;
		unsigned long	BURSTWR		:1;
		unsigned long	BURSTWG		:1;
		unsigned long	BURSTWB		:1;
		unsigned long				:5;
		unsigned long	WSTBMSKR	:1;
		unsigned long	WSTBMSKG	:1;
		unsigned long	WSTBMSKB	:1;
		unsigned long				:13;
	}bit;
};

/*	structure of AXIRSEL	(2841_383Ch)	*/
union io_ltm_ch_ltm_axirsel{
	unsigned long		word;
	struct {
		unsigned long	AXIRSEL1	:1;
		unsigned long	AXIRSEL2	:1;
		unsigned long				:30;
	}bit;
};

/*	structure of LSDRHSZR2	(2841_3880h)	*/
union io_ltm_ch_ltm_lsdrhszr2{
	unsigned long		word;
	struct {
		unsigned long	SDRHSZR2	:12;
		unsigned long				:20;
	}bit;
};

/*	structure of LSDRDEFR2	(2841_3884h)	*/
union io_ltm_ch_ltm_lsdrdefr2{
	unsigned long		word;
	struct {
		unsigned long	SDRDEFR2	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of LSDRVSZR2	(2841_3888h)	*/
union io_ltm_ch_ltm_lsdrvszr2{
	unsigned long		word;
	struct {
		unsigned long	SDRVSZR2	:14;
		unsigned long				:18;
	}bit;
};

/*	structure of LSDRADR2R	(2841_388Ch)	*/
union io_ltm_ch_ltm_lsdradr2r{
	unsigned long		word;
	struct {
		unsigned long	SDRADR2R	:32;
	}bit;
};

/*	structure of LSDRADR2G	(2841_3890h)	*/
union io_ltm_ch_ltm_lsdradr2g{
	unsigned long		word;
	struct {
		unsigned long	SDRADR2G	:32;
	}bit;
};

/*	structure of LSDRADR2B	(2841_3894h)	*/
union io_ltm_ch_ltm_lsdradr2b{
	unsigned long		word;
	struct {
		unsigned long	SDRADR2B	:32;
	}bit;
};

/*	structure of LSDRHSZR3	(2841_3898h)	*/
union io_ltm_ch_ltm_lsdrhszr3{
	unsigned long		word;
	struct {
		unsigned long	SDRHSZR3	:10;
		unsigned long				:22;
	}bit;
};

/*	structure of LSDRDEFR3	(2841_389Ch)	*/
union io_ltm_ch_ltm_lsdrdefr3{
	unsigned long		word;
	struct {
		unsigned long	SDRDEFR3	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of LSDRVSZR3	(2841_38A0h)	*/
union io_ltm_ch_ltm_lsdrvszr3{
	unsigned long		word;
	struct {
		unsigned long	SDRVSZR3	:10;
		unsigned long				:22;
	}bit;
};

/*	structure of LSDRADR3	(2841_38A4h)	*/
union io_ltm_ch_ltm_lsdradr3{
	unsigned long		word;
	struct {
		unsigned long	SDRADR3	:32;
	}bit;
};

/*	structure of LSDRDEFR4	(2841_38ACh)	*/
union io_ltm_ch_ltm_lsdrdefr4{
	unsigned long		word;
	struct {
		unsigned long	SDRDEFR4	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of LSDRADR4	(2841_38B4h)	*/
union io_ltm_ch_ltm_lsdradr4{
	unsigned long		word;
	struct {
		unsigned long	SDRADR4	:32;
	}bit;
};

/*	structure of LSDROFS	(2841_38B8h)	*/
union io_ltm_ch_ltm_lsdrofs{
	unsigned long		word;
	struct {
		unsigned long				:2;
		unsigned long	SDROFSR3	:1;
		unsigned long				:3;
		unsigned long	SDROFSR4	:1;
		unsigned long				:25;
	}bit;
};

/*	structure of LSDWADR	(2841_38BCh)	*/
union io_ltm_ch_ltm_lsdwadr{
	unsigned long		word;
	struct {
		unsigned long	SDWADR	:32;
	}bit;
};

/*	structure of LSDWDEFR	(2841_38C0h)	*/
union io_ltm_ch_ltm_lsdwdefr{
	unsigned long		word;
	struct {
		unsigned long	SDWDEFR	:16;
		unsigned long			:16;
	}bit;
};

/*	structure of LSDWADG	(2841_38C4h)	*/
union io_ltm_ch_ltm_lsdwadg{
	unsigned long		word;
	struct {
		unsigned long	SDWADG	:32;
	}bit;
};

/*	structure of LSDWDEFG	(2841_38C8h)	*/
union io_ltm_ch_ltm_lsdwdefg{
	unsigned long		word;
	struct {
		unsigned long	SDWDEFG	:16;
		unsigned long			:16;
	}bit;
};

/*	structure of LSDWADB	(2841_38CCh)	*/
union io_ltm_ch_ltm_lsdwadb{
	unsigned long		word;
	struct {
		unsigned long	SDWADB	:32;
	}bit;
};

/*	structure of LSDWDEFB	(2841_38D0h)	*/
union io_ltm_ch_ltm_lsdwdefb{
	unsigned long		word;
	struct {
		unsigned long	SDWDEFB	:16;
		unsigned long			:16;
	}bit;
};

/*	structure of LIOCTL (2841_38D4h)	*/
union io_ltm_ch_ltm_lioctl{
	unsigned long		word;
	struct {
		unsigned long	INSEL	:1;
		unsigned long			:3;
		unsigned long	R3DTYP	:2;
		unsigned long	R4DTYP	:2;
		unsigned long			:7;
		unsigned long	BYPASS	:1;
		unsigned long	OUTSEL	:1;
		unsigned long			:3;
		unsigned long	W1DTYP	:2;
		unsigned long			:10;
	}bit;
};

/*	structure of LHSTA	(2841_39D8h)	*/
union io_ltm_ch_ltm_lhsta{
	unsigned long		word;
	struct {
		long			HSTA	:25;
		unsigned long			:7;
	}bit;
};

/*	structure of LHPIT	(2841_39DCh)	*/
union io_ltm_ch_ltm_lhpit{
	unsigned long		word;
	struct {
		unsigned long	HPIT	:15;
		unsigned long			:17;
	}bit;
};

/*	structure of LVSTA	(2841_39E0h)	*/
union io_ltm_ch_ltm_lvsta{
	unsigned long		word;
	struct {
		long			VSTA	:25;
		unsigned long			:7;
	}bit;
};

/*	structure of LVPIT	(2841_39E4h)	*/
union io_ltm_ch_ltm_lvpit{
	unsigned long		word;
	struct {
		unsigned long	VPIT	:15;
		unsigned long			:17;
	}bit;
};

/*	structure of NLVTHOF7	(2841_39F0h)	*/
union io_ltm_ch_ltm_nlvthof{
	unsigned long		word;
	struct {
		unsigned long				:16;
		unsigned long	NLVTHOF_7	:13;
		unsigned long				:3;
	}bit;
};

/*	structure of PLVTHOF7	(2841_3A30h)	*/
union io_ltm_ch_ltm_plvthof{
	unsigned long		word;
	struct {
		unsigned long				:16;
		unsigned long	PLVTHOF_7	:13;
		unsigned long				:3;
	}bit;
};

/*	structure of LYSHADOW	(2841_3A88h)	*/
union io_ltm_ch_ltm_lyshadow{
	unsigned long		word;
	struct {
		unsigned long	YSHADOW	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of LYHIGHLIGHT	(2841_3A8Ch)	*/
union io_ltm_ch_ltm_lyhighlight{
	unsigned long		word;
	struct {
		unsigned long	YHIGHLIGHT	:14;
		unsigned long				:18;
	}bit;
};

/*	structure of RCGMD	(2841_3A90h)	*/
union io_ltm_ch_ltm_rcgmd{
	unsigned long		word;
	struct {
		unsigned long	YSHMD	:1;
		unsigned long			:3;
		unsigned long	RCGMD	:1;
		unsigned long			:3;
		unsigned long	RCGAMD	:1;
		unsigned long			:23;
	}bit;
};

/*	structure of RCGTBL	(2841_3AC0h)	*/
union io_ltm_ch_ltm_rcgtbl{
	unsigned short		hword[32];
	struct {
		unsigned short	RCGTBL	:12;
		unsigned short			:4;
	}bit;
};

/*	structure of BLDYRT (2841_3B1Ch)	*/
union io_ltm_ch_ltm_bldyrt{
	unsigned long		word;
	struct {
		unsigned long	BLDYRT	:4;
		unsigned long			:28;
	}bit;
};

/*	structure of BLDOF	(2841_3B20h)	*/
union io_ltm_ch_ltm_bldof_1{
	unsigned long		word;
	struct {
		unsigned long	BLDOF_0	:13;
		unsigned long			:3;
		unsigned long	BLDOF_1	:13;
		unsigned long			:3;
	}bit;
};
union io_ltm_ch_ltm_bldof_2{
	unsigned long		word;
	struct {
		unsigned long	BLDOF_2	:13;
		unsigned long			:3;
		unsigned long	BLDOF_3	:13;
		unsigned long			:3;
	}bit;
};
union io_ltm_ch_ltm_bldof_3{
	unsigned long		word;
	struct {
		unsigned long	BLDOF_4	:13;
		unsigned long			:3;
		unsigned long	BLDOF_5	:13;
		unsigned long			:3;
	}bit;
};
union io_ltm_ch_ltm_bldof_4{
	unsigned long		word;
	struct {
		unsigned long	BLDOF_6	:13;
		unsigned long			:3;
		unsigned long	BLDOF_7	:13;
		unsigned long			:3;
	}bit;
};
struct io_ltm_ch_ltm_bldof{
	union io_ltm_ch_ltm_bldof_1	BLDOF1;
	union io_ltm_ch_ltm_bldof_2	BLDOF2;
	union io_ltm_ch_ltm_bldof_3	BLDOF3;
	union io_ltm_ch_ltm_bldof_4	BLDOF4;
};

/*	structure of BLDGA	(2841_3B30h)	*/
union io_ltm_ch_ltm_bldga_1{
	unsigned long		word;
	struct {
		long			BLDGA_0	:15;
		unsigned long			:1;
		long			BLDGA_1	:15;
		unsigned long			:1;
	}bit;
};
union io_ltm_ch_ltm_bldga_2{
	unsigned long		word;
	struct {
		long			BLDGA_2	:15;
		unsigned long			:1;
		long			BLDGA_3	:15;
		unsigned long			:1;
	}bit;
};
union io_ltm_ch_ltm_bldga_3{
	unsigned long		word;
	struct {
		long			BLDGA_4	:15;
		unsigned long			:1;
		long			BLDGA_5	:15;
		unsigned long			:1;
	}bit;
};
union io_ltm_ch_ltm_bldga_4{
	unsigned long		word;
	struct {
		long			BLDGA_6	:15;
		unsigned long			:1;
		long			BLDGA_7	:15;
		unsigned long			:1;
	}bit;
};
struct io_ltm_ch_ltm_bldga{
	union io_ltm_ch_ltm_bldga_1	BLDGA1;
	union io_ltm_ch_ltm_bldga_2	BLDGA2;
	union io_ltm_ch_ltm_bldga_3	BLDGA3;
	union io_ltm_ch_ltm_bldga_4	BLDGA4;
};

/*	structure of BLDBD	(2841_3B40h)	*/
union io_ltm_ch_ltm_bldbd_1{
	unsigned long		word;
	struct {
		unsigned long	BLDBD_0	:12;
		unsigned long			:4;
		unsigned long	BLDBD_1	:12;
		unsigned long			:4;
	}bit;
};
union io_ltm_ch_ltm_bldbd_2{
	unsigned long		word;
	struct {
		unsigned long	BLDBD_2	:12;
		unsigned long			:4;
		unsigned long	BLDBD_3	:12;
		unsigned long			:4;
	}bit;
};
union io_ltm_ch_ltm_bldbd_3{
	unsigned long		word;
	struct {
		unsigned long	BLDBD_4	:12;
		unsigned long			:4;
		unsigned long	BLDBD_5	:12;
		unsigned long			:4;
	}bit;
};
union io_ltm_ch_ltm_bldbd_4{
	unsigned long		word;
	struct {
		unsigned long	BLDBD_6	:12;
		unsigned long			:4;
		unsigned long	BLDBD_7	:12;
		unsigned long			:4;
	}bit;
};
struct io_ltm_ch_ltm_bldbd{
	union io_ltm_ch_ltm_bldbd_1	BLDBD1;
	union io_ltm_ch_ltm_bldbd_2	BLDBD2;
	union io_ltm_ch_ltm_bldbd_3	BLDBD3;
	union io_ltm_ch_ltm_bldbd_4	BLDBD4;
};

/*	structure of LTMMD	(2841_3B50h)	*/
union io_ltm_ch_ltm_ltmmd{
	unsigned long		word;
	struct {
		unsigned long	LTMMD	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of CGOF	(2841_3B60h)	*/
union io_ltm_ch_ltm_cgof_1{
	unsigned long		word;
	struct {
		unsigned long	CGOF_0	:13;
		unsigned long			:3;
		unsigned long	CGOF_1	:13;
		unsigned long			:3;
	}bit;
};
union io_ltm_ch_ltm_cgof_2{
	unsigned long		word;
	struct {
		unsigned long	CGOF_2	:13;
		unsigned long			:3;
		unsigned long	CGOF_3	:13;
		unsigned long			:3;
	}bit;
};
union io_ltm_ch_ltm_cgof_3{
	unsigned long		word;
	struct {
		unsigned long	CGOF_4	:13;
		unsigned long			:3;
		unsigned long	CGOF_5	:13;
		unsigned long			:3;
	}bit;
};
union io_ltm_ch_ltm_cgof_4{
	unsigned long		word;
	struct {
		unsigned long	CGOF_6	:13;
		unsigned long			:3;
		unsigned long	CGOF_7	:13;
		unsigned long			:3;
	}bit;
};
struct io_ltm_ch_ltm_cgof{
	union io_ltm_ch_ltm_cgof_1 CGOF1;
	union io_ltm_ch_ltm_cgof_2 CGOF2;
	union io_ltm_ch_ltm_cgof_3 CGOF3;
	union io_ltm_ch_ltm_cgof_4 CGOF4;
};

/*	structure of CGGA	(2841_3B70h)	*/
union io_ltm_ch_ltm_cgga_1{
	unsigned long		word;
	struct {
		long			CGGA_0	:13;
		unsigned long			:3;
		long			CGGA_1	:13;
		unsigned long			:3;
	}bit;
};
union io_ltm_ch_ltm_cgga_2{
	unsigned long		word;
	struct {
		long			CGGA_2	:13;
		unsigned long			:3;
		long			CGGA_3	:13;
		unsigned long			:3;
	}bit;
};
union io_ltm_ch_ltm_cgga_3{
	unsigned long		word;
	struct {
		long			CGGA_4	:13;
		unsigned long			:3;
		long			CGGA_5	:13;
		unsigned long			:3;
	}bit;
};
union io_ltm_ch_ltm_cgga_4{
	unsigned long		word;
	struct {
		long			CGGA_6	:13;
		unsigned long			:3;
		long			CGGA_7	:13;
		unsigned long			:3;
	}bit;
};
struct io_ltm_ch_ltm_cgga{
	union io_ltm_ch_ltm_cgga_1 CGGA1;
	union io_ltm_ch_ltm_cgga_2 CGGA2;
	union io_ltm_ch_ltm_cgga_3 CGGA3;
	union io_ltm_ch_ltm_cgga_4 CGGA4;
};

/*	structure of CGBD	(2841_3B80h)	*/
union io_ltm_ch_ltm_cgbd_1{
	unsigned long		word;
	struct {
		unsigned long			:16;
		unsigned long	CGBD_1	:12;
		unsigned long			:4;
	}bit;
};
union io_ltm_ch_ltm_cgbd_2{
	unsigned long		word;
	struct {
		unsigned long	CGBD_2	:12;
		unsigned long			:4;
		unsigned long	CGBD_3	:12;
		unsigned long			:4;
	}bit;
};
union io_ltm_ch_ltm_cgbd_3{
	unsigned long		word;
	struct {
		unsigned long	CGBD_4	:12;
		unsigned long			:4;
		unsigned long	CGBD_5	:12;
		unsigned long			:4;
	}bit;
};
union io_ltm_ch_ltm_cgbd_4{
	unsigned long		word;
	struct {
		unsigned long	CGBD_6	:12;
		unsigned long			:4;
		unsigned long	CGBD_7	:12;
		unsigned long			:4;
	}bit;
};
struct io_ltm_ch_ltm_cgbd{
	union io_ltm_ch_ltm_cgbd_1 CGBD1;
	union io_ltm_ch_ltm_cgbd_2 CGBD2;
	union io_ltm_ch_ltm_cgbd_3 CGBD3;
	union io_ltm_ch_ltm_cgbd_4 CGBD4;
};

/*	structure of LYLCTL (2841_3BC4h)	*/
union io_ltm_ch_ltm_lylctl{
	unsigned long		word;
	struct {
		unsigned long	YLOGSL	:1;
		unsigned long			:3;
		unsigned long	YLOGSTA	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of LYLOG0EP0	(2841_3BC8h)	*/
union io_ltm_ch_ltm_lylog0ep0{
	unsigned long		word;
	struct {
		unsigned long	YLOG0EP0	:14;
		unsigned long				:18;
	}bit;
};

/*	structure of LYLOG0EP1	(2841_3BCCh)	*/
union io_ltm_ch_ltm_lylog0ep1{
	unsigned long		word;
	struct {
		unsigned long	YLOG0EP1	:14;
		unsigned long				:18;
	}bit;
};

/*	structure of LYLOG1EP0	(2841_3BD0h)	*/
union io_ltm_ch_ltm_lylog1ep0{
	unsigned long		word;
	struct {
		unsigned long	YLOG1EP0	:14;
		unsigned long				:18;
	}bit;
};

/*	structure of LYLOG1EP1	(2841_3BD4h)	*/
union io_ltm_ch_ltm_lylog1ep1{
	unsigned long		word;
	struct {
		unsigned long	YLOG1EP1	:14;
		unsigned long				:18;
	}bit;
};

/* Define i/o mapping */
struct io_ltm_ch_ltm{
	/* JDSLTM */
	union	io_ltm_ch_ltm_ltmsr			LTMSR;			/* 2841_(3800 - 3803h) */
	union	io_ltm_ch_ltm_lsramen		LSRAMEN;		/* 2841_(3804 - 3807h) */
	union	io_ltm_ch_ltm_ltrg			LTRG;			/* 2841_(3808 - 380Bh) */
	union	io_ltm_ch_ltm_lfrstp		LFRSTP;			/* 2841_(380C - 380Fh) */
	union	io_ltm_ch_ltm_lparmsk		LPARMSK;		/* 2841_(3810 - 3813h) */

	unsigned char dmy_3814_3817[0x3818-0x3814];			/* 2841_(3814 - 3817h) */

	union	io_ltm_ch_ltm_lint0			LINT0;			/* 2841_(3818 - 381Bh) */
	union	io_ltm_ch_ltm_lint1			LINT1;			/* 2841_(381C - 381Fh) */
	union	io_ltm_ch_ltm_llintlv		LLINTLV;		/* 2841_(3820 - 3823h) */
	union	io_ltm_ch_ltm_laxictlr1		LAXICTLR1;		/* 2841_(3824 - 3827h) */
	union	io_ltm_ch_ltm_laxictlr2		LAXICTLR2;		/* 2841_(3828 - 382Bh) */
	union	io_ltm_ch_ltm_laxirerr		LAXIRERR;		/* 2841_(382C - 382Fh) */
	union	io_ltm_ch_ltm_laxictlw		LAXICTLW;		/* 2841_(3830 - 3833h) */
	union	io_ltm_ch_ltm_laxiwerr		LAXIWERR;		/* 2841_(3834 - 3837h) */
	union	io_ltm_ch_ltm_laxibsiz		LAXIBSIZ;		/* 2841_(3838 - 383Bh) */
	union	io_ltm_ch_ltm_axirsel		AXIRSEL;		/* 2841_(383C - 383Fh) */

	unsigned char dmy_3840_387F[0x3880-0x3840];			/* 2841_(3840 - 387Fh) */

	union	io_ltm_ch_ltm_lsdrhszr2		LSDRHSZR2;		/* 2841_(3880 - 3883h) */
	union	io_ltm_ch_ltm_lsdrdefr2		LSDRDEFR2;		/* 2841_(3884 - 3887h) */
	union	io_ltm_ch_ltm_lsdrvszr2		LSDRVSZR2;		/* 2841_(3888 - 388Bh) */
	union	io_ltm_ch_ltm_lsdradr2r		LSDRADR2R;		/* 2841_(388C - 388Fh) */
	union	io_ltm_ch_ltm_lsdradr2g		LSDRADR2G;		/* 2841_(3890 - 3893h) */
	union	io_ltm_ch_ltm_lsdradr2b		LSDRADR2B;		/* 2841_(3894 - 3897h) */
	union	io_ltm_ch_ltm_lsdrhszr3		LSDRHSZR3;		/* 2841_(3898 - 389Bh) */
	union	io_ltm_ch_ltm_lsdrdefr3		LSDRDEFR3;		/* 2841_(389C - 389Fh) */
	union	io_ltm_ch_ltm_lsdrvszr3		LSDRVSZR3;		/* 2841_(38A0 - 38A3h) */
	union	io_ltm_ch_ltm_lsdradr3		LSDRADR3;		/* 2841_(38A4 - 38A7h) */

	unsigned char dmy_38A8_38AB[0x38AC-0x38A8];			/* 2841_(38A8 - 38ABh) */

	union	io_ltm_ch_ltm_lsdrdefr4		LSDRDEFR4;		/* 2841_(38AC - 38AFh) */

	unsigned char dmy_38B0_38B3[0x38B4-0x38B0];			/* 2841_(38B0 - 38B3h) */

	union	io_ltm_ch_ltm_lsdradr4		LSDRADR4;		/* 2841_(38B4 - 38B7h) */
	union	io_ltm_ch_ltm_lsdrofs		LSDROFS;		/* 2841_(38B8 - 38BBh) */
	union	io_ltm_ch_ltm_lsdwadr		LSDWADR;		/* 2841_(38BC - 38BFh) */
	union	io_ltm_ch_ltm_lsdwdefr		LSDWDEFR;		/* 2841_(38C0 - 38C3h) */
	union	io_ltm_ch_ltm_lsdwadg		LSDWADG;		/* 2841_(38C4 - 38C7h) */
	union	io_ltm_ch_ltm_lsdwdefg		LSDWDEFG;		/* 2841_(38C8 - 38CBh) */
	union	io_ltm_ch_ltm_lsdwadb		LSDWADB;		/* 2841_(38CC - 38CFh) */
	union	io_ltm_ch_ltm_lsdwdefb		LSDWDEFB;		/* 2841_(38D0 - 38D3h) */
	union	io_ltm_ch_ltm_lioctl		LIOCTL;			/* 2841_(38D4 - 38D7h) */

	unsigned char dmy_38D8_39D7[0x39D8-0x38D8];			/* 2841_(38D8 - 39D7h) */

	union	io_ltm_ch_ltm_lhsta			LHSTA;			/* 2841_(39D8 - 39DBh) */
	union	io_ltm_ch_ltm_lhpit			LHPIT;			/* 2841_(39DC - 39DFh) */
	union	io_ltm_ch_ltm_lvsta			LVSTA;			/* 2841_(39E0 - 39E3h) */
	union	io_ltm_ch_ltm_lvpit			LVPIT;			/* 2841_(39E4 - 39E7h) */

	unsigned char dmy_39E8_39FB[0x39FC-0x39E8];			/* 2841_(39E8 - 39FBh) */

	union	io_ltm_ch_ltm_nlvthof		NLVTHOF7;		/* 2841_(39FC - 39FFh) */

	unsigned char dmy_3A00_3A3B[0x3A3C-0x3A00];			/* 2841_(3A00 - 3A3Bh) */

	union	io_ltm_ch_ltm_plvthof		PLVTHOF7;		/* 2841_(3A3C - 3A3Fh) */

	unsigned char dmy_3A40_3A87[0x3A88-0x3A40];			/* 2841_(3A40 - 3A87h) */

	union	io_ltm_ch_ltm_lyshadow		LYSHADOW;		/* 2841_(3A88 - 3A8Bh) */
	union	io_ltm_ch_ltm_lyhighlight	LYHIGHLIGHT;	/* 2841_(3A8C - 3A8Fh) */
	union	io_ltm_ch_ltm_rcgmd			RCGMD;			/* 2841_(3A90 - 3A93h) */

	unsigned char dmy_3A94_3ABF[0x3AC0-0x3A94];			/* 2841_(3A94 - 3ABFh) */

	union	io_ltm_ch_ltm_rcgtbl		RCGTBL;			/* 2841_(3AC0 - 3AFFh) */

	unsigned char dmy_3B00_3B1B[0x3B1C-0x3B00];			/* 2841_(3A90 - 3B1Bh) */

	union	io_ltm_ch_ltm_bldyrt		BLDYRT;			/* 2841_(3B1C - 3B1Fh) */
	struct	io_ltm_ch_ltm_bldof			BLDOF;			/* 2841_(3B20 - 3B2Fh) */
	struct	io_ltm_ch_ltm_bldga			BLDGA;			/* 2841_(3B30 - 3B3Fh) */
	struct	io_ltm_ch_ltm_bldbd			BLDBD;			/* 2841_(3B40 - 3B4Fh) */
	union	io_ltm_ch_ltm_ltmmd			LTMMD;			/* 2841_(3B50 - 3B53h) */

	unsigned char dmy_3B54_3B5F[0x3B60-0x3B54];			/* 2841_(3B54 - 3B5Fh) */

	struct	io_ltm_ch_ltm_cgof			CGOF;			/* 2841_(3B60 - 3B6Fh) */
	struct	io_ltm_ch_ltm_cgga			CGGA;			/* 2841_(3B70 - 3B7Fh) */
	struct	io_ltm_ch_ltm_cgbd			CGBD;			/* 2841_(3B80 - 3B8Fh) */

	unsigned char dmy_3B90_3BC3[0x3BC4-0x3B90];			/* 2841_(3B90 - 3BC3h) */

	union	io_ltm_ch_ltm_lylctl		LYLCTL;			/* 2841_(3BC4 - 3BC7h) */
	union	io_ltm_ch_ltm_lylog0ep0		LYLOG0EP0;		/* 2841_(3BC8 - 3BCBh) */
	union	io_ltm_ch_ltm_lylog0ep1		LYLOG0EP1;		/* 2841_(3BCC - 3BCFh) */
	union	io_ltm_ch_ltm_lylog1ep0		LYLOG1EP0;		/* 2841_(3BD0 - 3BD3h) */
	union	io_ltm_ch_ltm_lylog1ep1		LYLOG1EP1;		/* 2841_(3BD4 - 3BD7h) */

	unsigned char dmy_3BD8_3BFF[0x3C00-0x3BD8];			/* 2841_(3BD8 - 3BFFh) */

};
