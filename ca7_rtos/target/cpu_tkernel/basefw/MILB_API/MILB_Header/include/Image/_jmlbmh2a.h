/**
 * @file  _jmlbmh2a.h
 * @brief  Definition JMLBMH2A Macro I/O register
 * @note  None
 * @attention None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*	structure of BMHSR	(1D40_0000h/1D48_0000h)	*/
union io_bmh_bmhsr{
	unsigned long		word;
	struct {
		unsigned long		:31;
		unsigned long	SR	:1;
	}bit;
};

/*	structure of BMHTRG	(1D40_0004h/1D48_0004h)	*/
union io_bmh_bmhtrg{
	unsigned long		word;
	struct {
		unsigned long			:30;
		unsigned long	BMHTRG	:2;
	}bit;
};

/*	structure of BMHRE	(1D40_0008h/1D48_0008h)	*/
union io_bmh_bmhre{
	unsigned long		word;
	struct {
		unsigned long	BMHRE	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of BMHCONT	(1D40_000Ch/1D48_000Ch)	*/
union io_bmh_bmhcont{
	unsigned long		word;
	struct {
		unsigned long			:31;
		unsigned long	CONT	:1;
	}bit;
};

/*	structure of BMHINTFS	(1D40_0010h/1D48_0010h)	*/
union io_bmh_bmhintfs{
	unsigned long		word;
	struct {
		unsigned long			:15;
		unsigned long	__RSST	:1;
		unsigned long			:7;
		unsigned long	__AXIF	:1;
		unsigned long			:7;
		unsigned long	__BMHF	:1;
	}bit;
};

/*	structure of BMHINTE	(1D40_0014h/1D48_0014h)	*/
union io_bmh_bmhinte{
	unsigned long		word;
	struct {
		unsigned long			:23;
		unsigned long	AXIE	:1;
		unsigned long			:7;
		unsigned long	BMHE	:1;
	}bit;
};

/*	structure of BMHMD	(1D40_0018h/1D48_0018h)	*/
union io_bmh_bmhmd{
	unsigned long		word;
	struct {
		unsigned long	MODE	:4;
		unsigned long			:4;
		unsigned long	OFFMD	:1;
		unsigned long			:3;
		unsigned long	LUTMD	:1;
		unsigned long			:3;
		unsigned long	OUTMD	:3;
		unsigned long			:1;
		unsigned long	PSHIFT	:3;
		unsigned long			:1;
		unsigned long	RBS		:3;
		unsigned long			:1;
		unsigned long	LSELCNT	:4;
	}bit;
};

/*	structure of BMHPRM	(1D40_001Ch/1D48_001Ch)	*/
union io_bmh_bmhprm{
	unsigned long		word;
	struct {
		unsigned long	SADLV	:16;
		unsigned long	POFFSET	:8;
		unsigned long			:8;
	}bit;
};

/*	structure of BMHHCNT	(1D40_0020h/1D48_0020h)	*/
union io_bmh_bmhhcnt{
	unsigned long		word;
	struct {
		unsigned long	RCNT	:4;
		unsigned long			:12;
		unsigned long	HCNT	:8;
		unsigned long			:8;
	}bit;
};

/*	structure of BMHSAD_WR	(1D40_0024h/1D48_0024h)	*/
union io_bmh_bmhsad_wr{
	unsigned long		word;
	struct {
		unsigned long	SAD_WR	:5;
		unsigned long			:27;
	}bit;
};

/*	structure of BMHDMA	(1D40_0030h/1D48_0030h)	*/
union io_bmh_bmhdma{
	unsigned long		word;
	struct {
		unsigned long	DMAMD	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of BMHGHSIZE	(1D40_0034h/1D48_0034h)	*/
union io_bmh_bmhghsize{
	unsigned long		word;
	struct {
		unsigned long	GHSIZE_REF	:16;
		unsigned long	GHSIZE_CUR	:16;
	}bit;
};

/*	structure of BMHHSIZE	(1D40_0038h/1D48_0038h)	*/
union io_bmh_bmhhsize{
	unsigned long		word;
	struct {
		unsigned long	HSIZE_REF	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of BMHVSIZE	(1D40_003Ch/1D48_003Ch)	*/
union io_bmh_bmhvsize{
	unsigned long		word;
	struct {
		unsigned long	VSIZE_REF	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of BMHCURAD	(1D40_0040h/1D48_0040h)	*/
union io_bmh_bmhcurad{
	unsigned long		word;
	struct {
		unsigned long	CURSTAD	:32;
	}bit;
};

/*	structure of BMHREFAD	(1D40_0044h/1D48_0044h)	*/
union io_bmh_bmhrefad{
	unsigned long		word;
	struct {
		unsigned long	REFSTAD	:32;
	}bit;
};

/*	structure of BMHSADAD	(1D40_0048h/1D48_0048h)	*/
union io_bmh_bmhsadad{
	unsigned long		word;
	struct {
		unsigned long	SADSTAD	:32;
	}bit;
};

/*	structure of BMHSADGH	(1D40_004Ch/1D48_004Ch)	*/
union io_bmh_bmhsadgh{
	unsigned long		word;
	struct {
		unsigned long	SADGHSIZE	:16;
		unsigned long				:16;
	}bit;
};

/*	structure of BMHAXICTRL	(1D40_0050h/1D48_0050h)	*/
union io_bmh_bmhaxictrl{
	unsigned long		word;
	struct {
		unsigned long	ARCACHE	:4;
		unsigned long	ARPROT	:3;
		unsigned long			:1;
		unsigned long	AWCACHE	:4;
		unsigned long	AWPROT	:3;
		unsigned long			:17;
	}bit;
};

/*	structure of BMHAXIERR	(1D40_0054h/1D48_0054h)	*/
union io_bmh_bmhaxierr{
	unsigned long		word;
	struct {
		unsigned long	RRESP	:2;
		unsigned long			:6;
		unsigned long	WRESP	:2;
		unsigned long			:22;
	}bit;
};

/*	structure of SADR	(1D40_0800h/1D48_0800h)	*/
union io_bmh_sadr{
	unsigned long		word[2][128];
	/* BMHMD.OUTMD=000b */
	struct {
		unsigned long	POS_0		:7;
		unsigned long	SADcmp_0	:1;
		unsigned long	POS_1		:7;
		unsigned long	SADcmp_1	:1;
		unsigned long	POS_2		:7;
		unsigned long	SADcmp_2	:1;
		unsigned long	POS_3		:7;
		unsigned long	SADcmp_3	:1;
	}bit000[2][57];

	/* BMHMD.OUTMD=001b */
	unsigned long		word001[225];
	struct {
		unsigned long	SADmin	:16;
		unsigned long	POS		:7;
		unsigned long	SADcmp	:1;
		unsigned long			:8;
	}bit001[225];

	/* BMHMD.OUTMD=010b */
	unsigned long		word010[2][113];
	struct {
		unsigned long	POS_fraction_0	:6;
		unsigned long	POS_Integer_0	:7;
		unsigned long	SADcmp_0		:1;
		unsigned long					:2;
		unsigned long	POS_fraction_1	:6;
		unsigned long	POS_Integer_1	:7;
		unsigned long	SADcmp_1		:1;
		unsigned long					:2;
	}bit010[2][113];

	/* BMHMD.OUTMD=011b */
	unsigned long		word011[225];
	struct {
		unsigned long	SADmin			:16;
		unsigned long	POS_fraction	:6;
		unsigned long	POS_Integer		:7;
		unsigned long	SADcmp			:1;
		unsigned long					:2;
	}bit011[225];

	/* BMHMD.OUTMD=100b */
	unsigned long		word100[2][112];
	struct {
		unsigned long	SADmin			:16;
		unsigned long	POS_fraction	:6;
		unsigned long	POS_Integer		:7;
		unsigned long	SADcmp			:1;
		unsigned long					:2;
	}bit100[2][112];

	/* BMHMD.OUTMD=101b */
	unsigned long			word101[2][56][2];
	struct {
		unsigned long long	SADmin			:16;
		unsigned long long	POS_fraction	:6;
		unsigned long long	POS_Integer		:7;
		unsigned long long	SADcmp			:1;
		unsigned long long					:2;
		unsigned long long	CUR_MIN			:8;
		unsigned long long	CUR_MAX			:8;
		unsigned long long	Deviation		:8;
		unsigned long long	Weight			:7;
		unsigned long long					:1;
	}bit101[2][56];

	/* BMHMD.OUTMD=110b */
	unsigned long			word110[2][28][4];
	struct {
		unsigned long long	SADmin			:16;
		unsigned long long	POS				:7;
		unsigned long long	SADcmp			:1;
		unsigned long long					:8;
		unsigned long long	SADmin1after	:16;
		unsigned long long	SADmin1before	:16;
		unsigned long long	SADmin2after	:16;
		unsigned long long	SADmin2before	:16;
		unsigned long long	CUR_MIN			:8;
		unsigned long long	CUR_MAX			:8;
		unsigned long long	Deviation		:8;
		unsigned long long	Weight			:7;
		unsigned long long					:1;
	}bit110[2][28];
};

/*	structure of CUR	(1D40_1000h/1D48_1000h)	*/
union io_bmh_cur{
	unsigned long		word[9][64];
	struct {
		unsigned long	CUR_0	:8;
		unsigned long	CUR_1	:8;
		unsigned long	CUR_2	:8;
		unsigned long	CUR_3	:8;
	}bit[9][64];
};

/*	structure of REF	(1D40_2000h/1D48_2000h)	*/
union io_bmh_ref{
	unsigned long		word[9][64];
	struct {
		unsigned long	REF_0	:8;
		unsigned long	REF_1	:8;
		unsigned long	REF_2	:8;
		unsigned long	REF_3	:8;
	}bit[9][64];
};

/*	structure of LUT	(1D40_3000h/1D48_3000h)	*/
union io_bmh_lut{
	unsigned long		word[1024];
	struct {
		unsigned long	LUT_0	:12;
		unsigned long			:4;
		unsigned long	LUT_1	:12;
		unsigned long			:4;
	}bit[1024];
};

/* Define i/o mapping */
struct io_bmh{
	/* JMLBMH2A */
	union  io_bmh_bmhsr			BMHSR;			/* 1D40/1D48_(0000 - 0003h) */
	union  io_bmh_bmhtrg		BMHTRG;			/* 1D40/1D48_(0004 - 0007h) */
	union  io_bmh_bmhre			BMHRE;			/* 1D40/1D48_(0008 - 000Bh) */
	union  io_bmh_bmhcont		BMHCONT;		/* 1D40/1D48_(000C - 000Fh) */
	union  io_bmh_bmhintfs		BMHINTFS;		/* 1D40/1D48_(0010 - 0013h) */
	union  io_bmh_bmhinte		BMHINTE;		/* 1D40/1D48_(0014 - 0017h) */
	union  io_bmh_bmhmd			BMHMD;			/* 1D40/1D48_(0018 - 001Bh) */
	union  io_bmh_bmhprm		BMHPRM;			/* 1D40/1D48_(001C - 001Fh) */
	union  io_bmh_bmhhcnt		BMHHCNT;		/* 1D40/1D48_(0020 - 0023h) */
	union  io_bmh_bmhsad_wr		BMHSAD_WR;		/* 1D40/1D48_(0024 - 0027h) */

	unsigned char dmy_0028_002F[0x0030-0x0028];	/* 1D40/1D48_(0028 - 002Fh) */

	union  io_bmh_bmhdma		BMHDMA;			/* 1D40/1D48_(0030 - 0033h) */
	union  io_bmh_bmhghsize		BMHGHSIZE;		/* 1D40/1D48_(0034 - 0037h) */
	union  io_bmh_bmhhsize		BMHHSIZE;		/* 1D40/1D48_(0038 - 003Bh) */
	union  io_bmh_bmhvsize		BMHVSIZE;		/* 1D40/1D48_(003C - 003Fh) */
	union  io_bmh_bmhcurad		BMHCURAD;		/* 1D40/1D48_(0040 - 0043h) */
	union  io_bmh_bmhrefad		BMHREFAD;		/* 1D40/1D48_(0044 - 0047h) */
	union  io_bmh_bmhsadad		BMHSADAD;		/* 1D40/1D48_(0048 - 004Bh) */
	union  io_bmh_bmhsadgh		BMHSADGH;		/* 1D40/1D48_(004C - 004Fh) */
	union  io_bmh_bmhaxictrl	BMHAXICTRL;		/* 1D40/1D48_(0050 - 0053h) */
	union  io_bmh_bmhaxierr		BMHAXIERR;		/* 1D40/1D48_(0054 - 0057h) */

	unsigned char dmy_0058_07FF[0x0800-0x0058];	/* 1D40/1D48_(0058 - 07FFh) */

	union  io_bmh_sadr			SADR;			/* 1D40/1D48_(0800 - 0BFFh) */

	unsigned char dmy_0C00_0FFF[0x1000-0x0C00];	/* 1D40/1D48_(0C00 - 0FFFh) */

	union  io_bmh_cur			CUR;			/* 1D40/1D48_(1000 - 18FFh) */

	unsigned char dmy_1900_1FFF[0x2000-0x1900];	/* 1D40/1D48_(1900 - 1FFFh) */

	union  io_bmh_ref			REF;			/* 1D40/1D48_(2000 - 28FFh) */

	unsigned char dmy_2900_2FFF[0x3000-0x2900];	/* 1D40/1D48_(2900 - 2FFFh) */

	union  io_bmh_lut			LUT;			/* 1D40/1D48_(3000 - 3FFFh) */

	unsigned char dmy_1D404000_1D47FFFF[0x1D480000-0x1D404000];	/* 1D40/1D48_4000 - 1D47_FFFFh */

};

extern volatile struct io_bmh		IO_BMH[2];

