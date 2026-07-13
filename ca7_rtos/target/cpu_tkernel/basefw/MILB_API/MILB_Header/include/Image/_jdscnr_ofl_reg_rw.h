#ifndef __JDSCNR_OFL_REG_RW_H__
#define __JDSCNR_OFL_REG_RW_H__


/*	structure of SPRSR	(2842_2000h)	*/
union io_cnr_ofl_reg_rw_sprsr{
	unsigned long		word;
	struct {
		unsigned long	SR	:1;
		unsigned long		:31;
	}bit;
};

/*	structure of ACESEN (2842_2004h)	*/
union io_cnr_ofl_reg_rw_acesen{
	unsigned long		word;
	struct {
		unsigned long	SPRACS	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of SPRTRG (2842_2008h)	*/
union io_cnr_ofl_reg_rw_sprtrg{
	unsigned long		word;
	struct {
		unsigned long	SPRTRG	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of SPRICE (2842_200Ch)	*/
union io_cnr_ofl_reg_rw_sprice{
	unsigned long		word;
	struct {
		unsigned long	SPDE	:1;
		unsigned long			:3;
		unsigned long	AXIE	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of SPRICF (2842_2010h)	*/
union io_cnr_ofl_reg_rw_spricf{
	unsigned long		word;
	struct {
		unsigned long	__SPDF	:1;
		unsigned long			:3;
		unsigned long	__AXIF	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of VDIV	(2842_2014h)	*/
union io_cnr_ofl_reg_rw_vdiv{
	unsigned long		word;
	struct {
		unsigned long	VDIVT	:1;
		unsigned long			:3;
		unsigned long	VDIVB	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of SPRAXISET	(2842_2020h)	*/
union io_cnr_ofl_reg_rw_spraxiset{
	unsigned long		word;
	struct {
		unsigned long	ARCACHE	:4;
		unsigned long	AWCACHE	:4;
		unsigned long	ARPROT	:3;
		unsigned long			:1;
		unsigned long	AWPROT	:3;
		unsigned long			:1;
		unsigned long	RRESP	:2;
		unsigned long			:2;
		unsigned long	WRESP	:2;
		unsigned long			:10;
	}bit;
};

/*	structure of SPRMOD (2842_2080h)	*/
union io_cnr_ofl_reg_rw_sprmod{
	unsigned long		word;
	struct {
		unsigned long	YUV_MODE	:3;
		unsigned long				:29;
	}bit;
};

/*	structure of HSIZE	(2842_2084h)	*/
union io_cnr_ofl_reg_rw_hsize{
	unsigned long		word;
	struct {
		unsigned long	HSIZE	:15;
		unsigned long			:17;
	}bit;
};

/*	structure of VSIZE	(2842_2088h)	*/
union io_cnr_ofl_reg_rw_vsize{
	unsigned long		word;
	struct {
		unsigned long	VSIZE	:15;
		unsigned long			:17;
	}bit;
};

/*	structure of RYTA	(2842_20A0h)	*/
union io_cnr_ofl_reg_rw_ryta{
	unsigned long		word;
	struct {
		unsigned long	RYTA	:32;
	}bit;
};

/*	structure of RYDEF	(2842_20A4h)	*/
union io_cnr_ofl_reg_rw_rydef{
	unsigned long		word;
	struct {
		unsigned long	RYDEF	:18;
		unsigned long			:14;
	}bit;
};

/*	structure of RCBTA	(2842_20A8h)	*/
union io_cnr_ofl_reg_rw_rcbta{
	unsigned long		word;
	struct {
		unsigned long	RCBTA	:32;
	}bit;
};

/*	structure of RCRTA	(2842_20ACh)	*/
union io_cnr_ofl_reg_rw_rcrta{
	unsigned long		word;
	struct {
		unsigned long	RCRTA	:32;
	}bit;
};

/*	structure of RCDEF	(2842_20B0h)	*/
union io_cnr_ofl_reg_rw_rcdef{
	unsigned long		word;
	struct {
		unsigned long	RCDEF	:18;
		unsigned long			:14;
	}bit;
};

/*	structure of WYTA	(2842_20C0h)	*/
union io_cnr_ofl_reg_rw_wyta{
	unsigned long		word;
	struct {
		unsigned long	WYTA	:32;
	}bit;
};

/*	structure of WYDEF	(2842_20C4h)	*/
union io_cnr_ofl_reg_rw_wydef{
	unsigned long		word;
	struct {
		unsigned long	WYDEF	:18;
		unsigned long			:14;
	}bit;
};

/*	structure of WCBTA	(2842_20C8h)	*/
union io_cnr_ofl_reg_rw_wcbta{
	unsigned long		word;
	struct {
		unsigned long	WCBTA	:32;
	}bit;
};

/*	structure of WCRTA	(2842_20CCh)	*/
union io_cnr_ofl_reg_rw_wcrta{
	unsigned long		word;
	struct {
		unsigned long	WCRTA	:32;
	}bit;
};

/*	structure of WCDEF	(2842_20D0h)	*/
union io_cnr_ofl_reg_rw_wcdef{
	unsigned long		word;
	struct {
		unsigned long	WCDEF	:18;
		unsigned long			:14;
	}bit;
};

/*	structure of TMPTA	(2842_20E0h)	*/
union io_cnr_ofl_reg_rw_tmpta{
	unsigned long		word;
	struct {
		unsigned long	TMPTA	:32;
	}bit;
};

/* Define i/o mapping */
struct io_cnr_ofl_reg_rw{
	/* JDSIMG */
	union  io_cnr_ofl_reg_rw_sprsr		SPRSR;		/* 2842_(2000 - 2003h) */
	union  io_cnr_ofl_reg_rw_acesen		ACESEN;		/* 2842_(2004 - 2007h) */
	union  io_cnr_ofl_reg_rw_sprtrg		SPRTRG;		/* 2842_(2008 - 200Bh) */
	union  io_cnr_ofl_reg_rw_sprice		SPRICE;		/* 2842_(200C - 200Fh) */
	union  io_cnr_ofl_reg_rw_spricf		SPRICF;		/* 2842_(2010 - 2013h) */
	union  io_cnr_ofl_reg_rw_vdiv		VDIV;		/* 2842_(2014 - 2017h) */

	unsigned char dmy_2018_201F[0x2020-0x2018];		/* 2842_(2018 - 201Fh) */

	union  io_cnr_ofl_reg_rw_spraxiset	SPRAXISET;	/* 2842_(2020 - 2023h) */

	unsigned char dmy_2024_207F[0x2080-0x2024];		/* 2842_(2024 - 207Fh) */

	union  io_cnr_ofl_reg_rw_sprmod		SPRMOD;		/* 2842_(2080 - 2083h) */
	union  io_cnr_ofl_reg_rw_hsize		HSIZE;		/* 2842_(2084 - 2087h) */
	union  io_cnr_ofl_reg_rw_vsize		VSIZE;		/* 2842_(2088 - 208Bh) */

	unsigned char dmy_208C_209F[0x20A0-0x208C];		/* 2842_(208C - 209Fh) */

	union  io_cnr_ofl_reg_rw_ryta		RYTA;		/* 2842_(20A0 - 20A3h) */
	union  io_cnr_ofl_reg_rw_rydef		RYDEF;		/* 2842_(20A4 - 20A7h) */
	union  io_cnr_ofl_reg_rw_rcbta		RCBTA;		/* 2842_(20A8 - 20ABh) */
	union  io_cnr_ofl_reg_rw_rcrta		RCRTA;		/* 2842_(20AC - 20AFh) */
	union  io_cnr_ofl_reg_rw_rcdef		RCDEF;		/* 2842_(20B0 - 20B3h) */

	unsigned char dmy_20B4_20BF[0x20C0-0x20B4];		/* 2842_(20B4 - 20BFh) */

	union  io_cnr_ofl_reg_rw_wyta		WYTA;		/* 2842_(20C0 - 20C3h) */
	union  io_cnr_ofl_reg_rw_wydef		WYDEF;		/* 2842_(20C4 - 20C7h) */
	union  io_cnr_ofl_reg_rw_wcbta		WCBTA;		/* 2842_(20C8 - 20CBh) */
	union  io_cnr_ofl_reg_rw_wcrta		WCRTA;		/* 2842_(20CC - 20CFh) */
	union  io_cnr_ofl_reg_rw_wcdef		WCDEF;		/* 2842_(20D0 - 20D3h) */

	unsigned char dmy_20D4_20DF[0x20E0-0x20D4];		/* 2842_(20D4 - 20DFh) */

	union  io_cnr_ofl_reg_rw_tmpta		TMPTA;		/* 2842_(20E0 - 20E3h) */

	unsigned char dmy_20E4_2FFF[0x3000-0x20E4];		/* 2842_(20E4 - 2FFFh) */

};

#endif	// #define __JDSCNR_OFL_REG_RW_H__
