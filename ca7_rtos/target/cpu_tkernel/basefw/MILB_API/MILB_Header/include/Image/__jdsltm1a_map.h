/**
 * @file		__jdsltm1a_map.h
 * @brief		Definition JDSLTM1A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


/*	structure of MAPSR	(2841_3400h)	*/
union io_ltm_ch_map_mapsr{
	unsigned long		word;
	struct {
		unsigned long	SR	:1;
		unsigned long		:31;
	}bit;
};

/*	structure of MSRAMEN	(2841_3404h)	*/
union io_ltm_ch_map_msramen{
	unsigned long		word;
	struct {
		unsigned long	SRAMAEN		:1;
		unsigned long				:3;
		unsigned long	RMAENHST	:1;
		unsigned long	RMAENYN		:1;
		unsigned long	RMAENDB		:1;
		unsigned long	RMAENRCH	:1;
		unsigned long				:24;
	}bit;
};

/*	structure of MTRG	(2841_3408h)	*/
union io_ltm_ch_map_mtrg{
	unsigned long		word;
	struct {
		unsigned long	TRG	:2;
		unsigned long		:30;
	}bit;
};

/*	structure of MFRSTP	(2841_340Ch)	*/
union io_ltm_ch_map_mfrstp{
	unsigned long		word;
	struct {
		unsigned long	FRSTP	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of MPARMSK	(2841_3410h)	*/
union io_ltm_ch_map_mparmsk{
	unsigned long		word;
	struct {
		unsigned long	PARMSK	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of MHSTINI	(2841_3418h)	*/
union io_ltm_ch_map_mhstini{
	unsigned long		word;
	struct {
		unsigned long	HSTINI	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of MHCPRD	(2841_341Ch)	*/
union io_ltm_ch_map_mhcprd{
	unsigned long		word;
	struct {
		unsigned long	HCPRD	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of MINT0	(2841_3424h)	*/
union io_ltm_ch_map_mint0{
	unsigned long		word;
	struct {
		unsigned long	FINEN	:1;
		unsigned long			:11;
		unsigned long	LCNTEN	:1;
		unsigned long			:3;
		unsigned long	HINIEN	:1;
		unsigned long			:3;
		unsigned long	AXREEN	:1;
		unsigned long	AXWEEN	:1;
		unsigned long			:2;
		unsigned long	HSTEEN	:1;
		unsigned long	RAMEEN	:1;
		unsigned long			:6;
	}bit;
};

/*	structure of MINT1	(2841_3428h)	*/
union io_ltm_ch_map_mint1{
	unsigned long		word;
	struct {
		unsigned long	__FINFL		:1;
		unsigned long				:11;
		unsigned long	__LCNTFL	:1;
		unsigned long				:3;
		unsigned long	__HINIFL	:1;
		unsigned long				:3;
		unsigned long	__AXREFL	:1;
		unsigned long	__AXWEFL	:1;
		unsigned long				:2;
		unsigned long	__HSTEFL	:1;
		unsigned long	__RAMEFL	:1;
		unsigned long				:6;
	}bit;
};

/*	structure of MLINTLV	(2841_342Ch)	*/
union io_ltm_ch_map_mlintlv{
	unsigned long		word;
	struct {
		unsigned long	LINTLV	:10;
		unsigned long			:22;
	}bit;
};

/*	structure of AXICHSEL	(2841_3430h)	*/
union io_ltm_ch_map_axichsel{
	unsigned long		word;
	struct {
		unsigned long	AXIWSEL1	:1;
		unsigned long	AXIWSEL2	:1;
		unsigned long				:30;
	}bit;
};

/*	structure of MAXICTLR	(2841_3434h)	*/
union io_ltm_ch_map_maxictlr{
	unsigned long		word;
	struct {
		unsigned long	ARCACHE	:4;
		unsigned long	ARPROT	:3;
		unsigned long			:25;
	}bit;
};

/*	structure of MAXIRERR	(2841_3438h)	*/
union io_ltm_ch_map_maxirerr{
	unsigned long		word;
	struct {
		unsigned long	RRESP	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of MAXICTLW	(2841_343Ch)	*/
union io_ltm_ch_map_maxictlw{
	unsigned long		word;
	struct {
		unsigned long	AWCACHE_YP	:4;
		unsigned long	AWPROT_YP	:3;
		unsigned long				:1;
		unsigned long	AWCACHE_YN	:4;
		unsigned long	AWPROT_YN	:3;
		unsigned long				:1;
		unsigned long	AWCACHE_DB	:4;
		unsigned long	AWPROT_DB	:3;
		unsigned long				:9;
	}bit;
};

/*	structure of MAXIWERR	(2841_3440h)	*/
union io_ltm_ch_map_maxiwerr{
	unsigned long		word;
	struct {
		unsigned long	WRESP_YP	:2;
		unsigned long	WRESP_YN	:2;
		unsigned long	WRESP_DB	:2;
		unsigned long				:26;
	}bit;
};

/*	structure of MAXIBSIZ	(2841_3444h)	*/
union io_ltm_ch_map_maxibsiz{
	unsigned long		word;
	struct {
		unsigned long	BURSTR		:1;
		unsigned long				:7;
		unsigned long	BURSTWYP	:1;
		unsigned long	BURSTWYN	:1;
		unsigned long	BURSTWDB	:1;
		unsigned long				:5;
		unsigned long	WSTBMSKYP	:1;
		unsigned long	WSTBMSKYN	:1;
		unsigned long	WSTBMSKDB	:1;
		unsigned long				:13;
	}bit;
};

/*	structure of MSDRHSZ	(2841_3480h)	*/
union io_ltm_ch_map_msdrhsz{
	unsigned long		word;
	struct {
		unsigned long	SDRHSZ	:10;
		unsigned long			:22;
	}bit;
};

/*	structure of MSDRDEF	(2841_3484h)	*/
union io_ltm_ch_map_msdrdef{
	unsigned long		word;
	struct {
		unsigned long	SDRDEF	:16;
		unsigned long			:16;
	}bit;
};

/*	structure of MSDRVSZ	(2841_3488h)	*/
union io_ltm_ch_map_msdrvsz{
	unsigned long		word;
	struct {
		unsigned long	SDRVSZ	:10;
		unsigned long			:22;
	}bit;
};

/*	structure of MSDRAD	(2841_348Ch)	*/
union io_ltm_ch_map_msdrad{
	unsigned long		word;
	struct {
		unsigned long	SDRAD	:32;
	}bit;
};

/*	structure of MSDROFS	(2841_3490h)	*/
union io_ltm_ch_map_msdrofs{
	unsigned long		word;
	struct {
		unsigned long			:2;
		unsigned long	SDROFS	:1;
		unsigned long			:29;
	}bit;
};

/*	structure of MSDWADW0	(2841_3494h)	*/
union io_ltm_ch_map_msdwadw0{
	unsigned long		word;
	struct {
		unsigned long	SDWADW0	:32;
	}bit;
};

/*	structure of MSDWDEFW0	(2841_3498h)	*/
union io_ltm_ch_map_msdwdefw0{
	unsigned long		word;
	struct {
		unsigned long	SDWDEFW0	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of MSDWADW1	(2841_349Ch)	*/
union io_ltm_ch_map_msdwadw1{
	unsigned long		word;
	struct {
		unsigned long	SDWADW1	:32;
	}bit;
};

/*	structure of MSDWDEFW1	(2841_34A0h)	*/
union io_ltm_ch_map_msdwdefw1{
	unsigned long		word;
	struct {
		unsigned long	SDWDEFW1	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of MIOCTL	(2841_34ACh)	*/
union io_ltm_ch_map_mioctl{
	unsigned long		word;
	struct {
		unsigned long	INSEL	:1;
		unsigned long			:3;
		unsigned long	R1DTYP	:2;
		unsigned long			:10;
		unsigned long	OUTEN1	:1;
		unsigned long			:1;
		unsigned long	OUTEN3	:1;
		unsigned long			:1;
		unsigned long	W0DTYP	:2;
		unsigned long	W1DTYP	:2;
		unsigned long			:2;
		unsigned long	W3DTYP	:2;
		unsigned long			:4;
	}bit;
};

/*	structure of GAINR	(2841_34BCh)	*/
union io_ltm_ch_map_gainr{
	unsigned long		word;
	struct {
		unsigned long	GAINR	:11;
		unsigned long			:21;
	}bit;
};

/*	structure of GAING	(2841_34C0h)	*/
union io_ltm_ch_map_gaing{
	unsigned long		word;
	struct {
		unsigned long	GAING	:11;
		unsigned long			:21;
	}bit;
};

/*	structure of GAINB	(2841_34C4h)	*/
union io_ltm_ch_map_gainb{
	unsigned long		word;
	struct {
		unsigned long	GAINB	:11;
		unsigned long			:21;
	}bit;
};

/*	structure of GAINCPR	(2841_34C8h)	*/
union io_ltm_ch_map_gaincpr{
	unsigned long		word;
	struct {
		unsigned long	GAINCPR	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of GAINCPG	(2841_34CCh)	*/
union io_ltm_ch_map_gaincpg{
	unsigned long		word;
	struct {
		unsigned long	GAINCPG	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of GAINCPB	(2841_34D0h)	*/
union io_ltm_ch_map_gaincpb{
	unsigned long		word;
	struct {
		unsigned long	GAINCPB	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of PEPS0M	(2841_35CCh)	*/
union io_ltm_ch_map_peps0m{
	unsigned long		word;
	struct {
		unsigned long	PEPS0M	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of PEPS0OF	(2841_35D4h)	*/
union io_ltm_ch_map_peps0of{
	unsigned long		word;
	struct {
		unsigned long				:16;
		unsigned long	PEPS0OF_3	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of PEPS1M	(2841_35E8h)	*/
union io_ltm_ch_map_peps1m{
	unsigned long		word;
	struct {
		unsigned long	PEPS1M	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of PEPS1OF	(2841_35F4h)	*/
union io_ltm_ch_map_peps1of{
	unsigned long		word;
	struct {
		unsigned long				:16;
		unsigned long	PEPS1OF_3	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of PEPS2M	(2841_3608h)	*/
union io_ltm_ch_map_peps2m{
	unsigned long		word;
	struct {
		unsigned long	PEPS2M	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of PEPS2OF	(2841_3614h)	*/
union io_ltm_ch_map_peps2of{
	unsigned long		word;
	struct {
		unsigned long				:16;
		unsigned long	PEPS2OF_3	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of NEPS0M	(2841_3628h)	*/
union io_ltm_ch_map_neps0m{
	unsigned long		word;
	struct {
		unsigned long	NEPS0M	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of NEPS0OF	(2841_3634h)	*/
union io_ltm_ch_map_neps0of{
	unsigned long		word;
	struct {
		unsigned long				:16;
		unsigned long	NEPS0OF_3	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of NEPS1M	(2841_3648h)	*/
union io_ltm_ch_map_neps1m{
	unsigned long		word;
	struct {
		unsigned long	NEPS1M	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of NEPS1OF	(2841_3654h)	*/
union io_ltm_ch_map_neps1of{
	unsigned long		word;
	struct {
		unsigned long				:16;
		unsigned long	NEPS1OF_3	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of NEPS2M	(2841_3668h)	*/
union io_ltm_ch_map_neps2m{
	unsigned long		word;
	struct {
		unsigned long	NEPS2M	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of NEPS2OF	(2841_3674h)	*/
union io_ltm_ch_map_neps2of{
	unsigned long		word;
	struct {
		unsigned long				:16;
		unsigned long	NEPS2OF_3	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of YSHADOW	(2841_36A8h)	*/
union io_ltm_ch_map_yshadow{
	unsigned long		word;
	struct {
		unsigned long	YSHADOW	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of YHIGHLIGHT	(2841_36ACh)	*/
union io_ltm_ch_map_yhighlight{
	unsigned long		word;
	struct {
		unsigned long	YHIGHLIGHT	:14;
		unsigned long				:18;
	}bit;
};

/*	structure of MYLCTL	(2841_36B0h)	*/
union io_ltm_ch_map_mylctl{
	unsigned long		word;
	struct {
		unsigned long	YLOGSL	:1;
		unsigned long			:3;
		unsigned long	YLOGSTA	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of MYLOG0EP0	(2841_36B4h)	*/
union io_ltm_ch_map_mylog0ep0{
	unsigned long		word;
	struct {
		unsigned long	YLOG0EP0	:14;
		unsigned long				:18;
	}bit;
};

/*	structure of MYLOG0EP1	(2841_36B8h)	*/
union io_ltm_ch_map_mylog0ep1{
	unsigned long		word;
	struct {
		unsigned long	YLOG0EP1	:14;
		unsigned long				:18;
	}bit;
};

/*	structure of MYLOG1EP0	(2841_36BCh)	*/
union io_ltm_ch_map_mylog1ep0{
	unsigned long		word;
	struct {
		unsigned long	YLOG1EP0	:14;
		unsigned long				:18;
	}bit;
};

/*	structure of MYLOG1EP1	(2841_36C0h)	*/
union io_ltm_ch_map_mylog1ep1{
	unsigned long		word;
	struct {
		unsigned long	YLOG1EP1	:14;
		unsigned long				:18;
	}bit;
};

/* Define i/o mapping */
struct io_ltm_ch_map{
	/* JDSLTM */
	union	io_ltm_ch_map_mapsr			MAPSR;			/* 2841_(3400 - 3403h) */
	union	io_ltm_ch_map_msramen		MSRAMEN;		/* 2841_(3404 - 3407h) */
	union	io_ltm_ch_map_mtrg			MTRG;			/* 2841_(3408 - 340Bh) */
	union	io_ltm_ch_map_mfrstp		MFRSTP;			/* 2841_(340C - 340Fh) */
	union	io_ltm_ch_map_mparmsk		MPARMSK;		/* 2841_(3410 - 3413h) */

	unsigned char dmy_3414_3417[0x3418-0x3414];			/* 2841_(3414 - 3417h) */

	union	io_ltm_ch_map_mhstini		MHSTINI;		/* 2841_(3418 - 341Bh) */
	union	io_ltm_ch_map_mhcprd		MHCPRD;			/* 2841_(341C - 341Fh) */

	unsigned char dmy_3420_3423[0x3424-0x3420];			/* 2841_(3420 - 3423h) */

	union	io_ltm_ch_map_mint0			MINT0;			/* 2841_(3424 - 3427h) */
	union	io_ltm_ch_map_mint1			MINT1;			/* 2841_(3428 - 342Bh) */
	union	io_ltm_ch_map_mlintlv		MLINTLV;		/* 2841_(342C - 342Fh) */
	union	io_ltm_ch_map_axichsel		AXICHSEL;		/* 2841_(3430 - 3433h) */
	union	io_ltm_ch_map_maxictlr		MAXICTLR;		/* 2841_(3434 - 3437h) */
	union	io_ltm_ch_map_maxirerr		MAXIRERR;		/* 2841_(3438 - 343Bh) */
	union	io_ltm_ch_map_maxictlw		MAXICTLW;		/* 2841_(343C - 343Fh) */
	union	io_ltm_ch_map_maxiwerr		MAXIWERR;		/* 2841_(3440 - 3443h) */
	union	io_ltm_ch_map_maxibsiz		MAXIBSIZ;		/* 2841_(3444 - 3447h) */

	unsigned char dmy_3448_347F[0x3480-0x3448];			/* 2841_(3448 - 347Fh) */

	union	io_ltm_ch_map_msdrhsz		MSDRHSZ;		/* 2841_(3480 - 3483h) */
	union	io_ltm_ch_map_msdrdef		MSDRDEF;		/* 2841_(3484 - 3487h) */
	union	io_ltm_ch_map_msdrvsz		MSDRVSZ;		/* 2841_(3488 - 348Bh) */
	union	io_ltm_ch_map_msdrad		MSDRAD;			/* 2841_(348C - 348Fh) */
	union	io_ltm_ch_map_msdrofs		MSDROFS;		/* 2841_(3490 - 3493h) */
	union	io_ltm_ch_map_msdwadw0		MSDWADW0;		/* 2841_(3494 - 3497h) */
	union	io_ltm_ch_map_msdwdefw0		MSDWDEFW0;		/* 2841_(3498 - 349Bh) */
	union	io_ltm_ch_map_msdwadw1		MSDWADW1;		/* 2841_(349C - 349Fh) */
	union	io_ltm_ch_map_msdwdefw1		MSDWDEFW1;		/* 2841_(34A0 - 34A3h) */

	unsigned char dmy_34A4_34AB[0x34AC-0x34A4];			/* 2841_(34A4 - 34ABh) */

	union	io_ltm_ch_map_mioctl		MIOCTL;			/* 2841_(34AC - 34AFh) */

	unsigned char dmy_34B0_34BB[0x34BC-0x34B0];			/* 2841_(34B0 - 34BBh) */

	union	io_ltm_ch_map_gainr			GAINR;			/* 2841_(34BC - 34BFh) */
	union	io_ltm_ch_map_gaing			GAING;			/* 2841_(34C0 - 34C3h) */
	union	io_ltm_ch_map_gainb			GAINB;			/* 2841_(34C4 - 34C7h) */
	union	io_ltm_ch_map_gaincpr		GAINCPR;		/* 2841_(34C8 - 34CBh) */
	union	io_ltm_ch_map_gaincpg		GAINCPG;		/* 2841_(34CC - 34CFh) */
	union	io_ltm_ch_map_gaincpb		GAINCPB;		/* 2841_(34D0 - 34D3h) */

	unsigned char dmy_34D4_35CB[0x35CC-0x34D4];			/* 2841_(34D4 - 35CBh) */

	union	io_ltm_ch_map_peps0m		PEPS0M;			/* 2841_(35CC - 35CFh) */

	unsigned char dmy_35D0_35D3[0x35D4-0x35D0];			/* 2841_(35D0 - 35D3h) */

	union	io_ltm_ch_map_peps0of		PEPS0OF;		/* 2841_(35D4 - 35D7h) */

	unsigned char dmy_35D8_35E7[0x35E8-0x35D8];			/* 2841_(35D8 - 35E7h) */

	union	io_ltm_ch_map_peps1m		PEPS1M;			/* 2841_(35E8 - 35EBh) */

	unsigned char dmy_35EC_35F3[0x35F4-0x35EC];			/* 2841_(35EC - 35F3h) */

	union	io_ltm_ch_map_peps1of		PEPS1OF;		/* 2841_(35F4 - 35F7h) */

	unsigned char dmy_35F8_3607[0x3608-0x35F8];			/* 2841_(35F8 - 3607h) */

	union	io_ltm_ch_map_peps2m		PEPS2M;			/* 2841_(3608 - 360Bh) */

	unsigned char dmy_360C_3613[0x3614-0x360C];			/* 2841_(360C - 3613h) */

	union	io_ltm_ch_map_peps2of		PEPS2OF;		/* 2841_(3614 - 3617h) */

	unsigned char dmy_3618_3627[0x3628-0x3618];			/* 2841_(3618 - 3627h) */

	union	io_ltm_ch_map_neps0m		NEPS0M;			/* 2841_(3628 - 362Bh) */

	unsigned char dmy_362C_3633[0x3634-0x362C];			/* 2841_(362C - 3633h) */

	union	io_ltm_ch_map_neps0of		NEPS0OF;		/* 2841_(3634 - 3637h) */

	unsigned char dmy_3638_3647[0x3648-0x3638];			/* 2841_(3638 - 3647h) */

	union	io_ltm_ch_map_neps1m		NEPS1M;			/* 2841_(3648 - 364Bh) */

	unsigned char dmy_364C_3653[0x3654-0x364C];			/* 2841_(364C - 3653h) */

	union	io_ltm_ch_map_neps1of		NEPS1OF;		/* 2841_(3654 - 3657h) */

	unsigned char dmy_3658_3667[0x3668-0x3658];			/* 2841_(3658 - 3667h) */

	union	io_ltm_ch_map_neps2m		NEPS2M;			/* 2841_(3668 - 366Bh) */

	unsigned char dmy_366C_3673[0x3674-0x366C];			/* 2841_(366C - 3673h) */

	union	io_ltm_ch_map_neps2of		NEPS2OF;		/* 2841_(3674 - 3677h) */

	unsigned char dmy_3678_36A7[0x36A8-0x3678];			/* 2841_(3678 - 36A7h) */

	union	io_ltm_ch_map_yshadow		YSHADOW;		/* 2841_(36A8 - 36ABh) */
	union	io_ltm_ch_map_yhighlight	YHIGHLIGHT;		/* 2841_(36AC - 36AFh) */
	union	io_ltm_ch_map_mylctl		MYLCTL;			/* 2841_(36B0 - 36B3h) */
	union	io_ltm_ch_map_mylog0ep0		MYLOG0EP0;		/* 2841_(36B4 - 36B7h) */
	union	io_ltm_ch_map_mylog0ep1		MYLOG0EP1;		/* 2841_(36B8 - 36BBh) */
	union	io_ltm_ch_map_mylog1ep0		MYLOG1EP0;		/* 2841_(36BC - 36BFh) */
	union	io_ltm_ch_map_mylog1ep1		MYLOG1EP1;		/* 2841_(36C0 - 36C3h) */

	unsigned char dmy_36C4_37FF[0x3800-0x36C4];			/* 2841_(36C4 - 37FFh) */

};
