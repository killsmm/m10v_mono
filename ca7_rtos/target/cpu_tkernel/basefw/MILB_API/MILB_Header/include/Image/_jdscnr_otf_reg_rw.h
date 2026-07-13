#ifndef __JDSCNR_OTF_REG_RW_H__
#define __JDSCNR_OTF_REG_RW_H__


/*	structure of SPRSR	(2842_0000h)	*/
union io_cnr_otf_reg_rw_sprsr{
	unsigned long		word;
	struct {
		unsigned long	SR	:1;
		unsigned long		:31;
	}bit;
};

/*	structure of ACESEN (2842_0004h)	*/
union io_cnr_otf_reg_rw_acesen{
	unsigned long		word;
	struct {
		unsigned long	SPRACS	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of SPRTRG (2842_0008h)	*/
union io_cnr_otf_reg_rw_sprtrg{
	unsigned long		word;
	struct {
		unsigned long	SPRTRG	:2;
		unsigned long			:2;
		unsigned long	CNRCSE	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of SPRICE (2842_000Ch)	*/
union io_cnr_otf_reg_rw_sprice{
	unsigned long		word;
	struct {
		unsigned long	SPDE	:1;
		unsigned long			:3;
		unsigned long	AXIE	:1;
		unsigned long			:3;
		unsigned long	DRTE	:1;
		unsigned long			:23;
	}bit;
};

/*	structure of SPRICF (2842_0010h)	*/
union io_cnr_otf_reg_rw_spricf{
	unsigned long		word;
	struct {
		unsigned long	__SPDF	:1;
		unsigned long			:3;
		unsigned long	__AXIF	:1;
		unsigned long			:3;
		unsigned long	__DRTF	:1;
		unsigned long			:23;
	}bit;
};

/*	structure of HDIV	(2842_0014h)	*/
union io_cnr_otf_reg_rw_hdiv{
	unsigned long		word;
	struct {
		unsigned long	HDIVL	:1;
		unsigned long			:3;
		unsigned long	HDIVR	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of SPRAXISET	(2842_0020h)	*/
union io_cnr_otf_reg_rw_spraxiset{
	unsigned long		word;
	struct {
		unsigned long			:4;
		unsigned long	AWCACHE	:4;
		unsigned long			:3;
		unsigned long			:1;
		unsigned long	AWPROT	:3;
		unsigned long			:1;
		unsigned long			:2;
		unsigned long			:2;
		unsigned long	WRESP	:2;
		unsigned long			:10;
	}bit;
};

/*	structure of SPRMOD (2842_0080h)	*/
union io_cnr_otf_reg_rw_sprmod{
	unsigned long		word;
	struct {
		unsigned long	YUV_MODE	:3;
		unsigned long				:1;
		unsigned long	EXMODE		:1;
		unsigned long				:3;
		unsigned long	CNRVFM		:1;
		unsigned long				:23;
	}bit;
};

/*	structure of HSIZE	(2842_0084h)	*/
union io_cnr_otf_reg_rw_hsize{
	unsigned long		word;
	struct {
		unsigned long	HSIZE	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of VSIZE	(2842_0088h)	*/
union io_cnr_otf_reg_rw_vsize{
	unsigned long		word;
	struct {
		unsigned long	VSIZE	:13;
		unsigned long			:19;
	}bit;
};

/*	structure of WCTA	(2842_00A0h)	*/
union io_cnr_otf_reg_rw_wcta{
	unsigned long		word;
	struct {
		unsigned long	WCTA	:32;
	}bit;
};

/*	structure of WCDEF	(2842_00A4h)	*/
union io_cnr_otf_reg_rw_wcdef{
	unsigned long		word;
	struct {
		unsigned long	WCDEF	:18;
		unsigned long			:14;
	}bit;
};

/*	structure of CNRWOFSX	(2842_00A8h)	*/
union io_cnr_otf_reg_rw_cnrwofsx{
	unsigned long		word;
	struct {
		unsigned long	CNRWOFSX	:12;
		unsigned long				:20;
	}bit;
};

/* Define i/o mapping */
struct io_cnr_otf_reg_rw{
	/* JDSIMG */
	union  io_cnr_otf_reg_rw_sprsr		SPRSR;		/* 2842_(0000 - 0003h) */
	union  io_cnr_otf_reg_rw_acesen		ACESEN;		/* 2842_(0004 - 0007h) */
	union  io_cnr_otf_reg_rw_sprtrg		SPRTRG;		/* 2842_(0008 - 000Bh) */
	union  io_cnr_otf_reg_rw_sprice		SPRICE;		/* 2842_(000C - 000Fh) */
	union  io_cnr_otf_reg_rw_spricf		SPRICF;		/* 2842_(0010 - 0013h) */
	union  io_cnr_otf_reg_rw_hdiv		HDIV;		/* 2842_(0014 - 0017h) */

	unsigned char dmy_0018_001F[0x0020-0x0018];	/* 2842_(0018 - 001Fh) */

	union  io_cnr_otf_reg_rw_spraxiset	SPRAXISET;	/* 2842_(0020 - 0023h) */

	unsigned char dmy_0024_007F[0x0080-0x0024];	/* 2842_(0024 - 007Fh) */

	union  io_cnr_otf_reg_rw_sprmod		SPRMOD;		/* 2842_(0080 - 0083h) */
	union  io_cnr_otf_reg_rw_hsize		HSIZE;		/* 2842_(0084 - 0087h) */
	union  io_cnr_otf_reg_rw_vsize		VSIZE;		/* 2842_(0088 - 008Bh) */

	unsigned char dmy_008C_009F[0x00A0-0x008C];	/* 2842_(008C - 009Fh) */

	union  io_cnr_otf_reg_rw_wcta		WCTA;		/* 2842_(00A0 - 00A3h) */
	union  io_cnr_otf_reg_rw_wcdef		WCDEF;		/* 2842_(00A4 - 00A7h) */
	union  io_cnr_otf_reg_rw_cnrwofsx	CNRWOFSX;	/* 2842_(00A8 - 00ABh) */

	unsigned char dmy_00AC_0FFF[0x1000-0x00AC];	/* 2842_(00AC - 0FFFh) */

};

#endif	// #define __JDSCNR_OTF_REG_RW_H__
