/**
 * @file		_jdsxh2d.h
 * @brief		Definition JDSXH2D Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*	structure of SR (288C_0000h)	*/
union io_xch_sr{
	unsigned long		word;
	struct {
		unsigned long	SR	:1;
		unsigned long		:31;
	}bit;
};

/*	structure of XCHICE	(288C_0004h)	*/
union io_xch_xchice{
	unsigned long		word;
	struct {
		unsigned long	XE0	:1;
		unsigned long		:3;
		unsigned long	XE1	:1;
		unsigned long		:27;
	}bit;
};

/*	structure of XCHICF	(288C_0008h)	*/
union io_xch_xchicf{
	unsigned long		word;
	struct {
		unsigned long	__XF0	:1;
		unsigned long			:3;
		unsigned long	__XF1	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of XTRG0	(288C_000Ch)	*/
/*	structure of XTRG1	(288C_0050h)	*/
union io_xch_xtrg{
	unsigned long		word;
	struct {
		unsigned long	XTRG	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of XDTYPE0	(288C_0014h)	*/
/*	structure of XDTYPE1	(288C_0058h)	*/
union io_xch_xdtype{
	unsigned long		word;
	struct {
		unsigned long	XSDTYPE	:2;
		unsigned long			:2;
		unsigned long	XDDTYPE	:2;
		unsigned long			:26;
	}bit;
};

/*	structure of XMD0	(288C_0018h)	*/
/*	structure of XMD1	(288C_005Ch)	*/
union io_xch_xmd{
	unsigned long		word;
	struct {
		unsigned long	XMD	:3;
		unsigned long		:29;
	}bit;
};

/*	structure of XFDT0	(288C_001Ch)	*/
/*	structure of XFDT1	(288C_0060h)	*/
union io_xch_xfdt{
	unsigned long		word;
	struct {
		unsigned long	XFDT	:8;
		unsigned long			:24;
	}bit;
};

/*	structure of XCYC0	(288C_0020h)	*/
/*	structure of XCYC1	(288C_0064h)	*/
union io_xch_xcyc{
	unsigned long		word;
	struct {
		unsigned long	XVCYC	:5;
		unsigned long			:3;
		unsigned long	XHCYC	:5;
		unsigned long			:19;
	}bit;
};

/*	structure of XVEN0	(288C_0024h)	*/
/*	structure of XVEN1	(288C_0068h)	*/
union io_xch_xven{
	unsigned long		word;
	struct {
		unsigned long	XVEN	:32;
	}bit;
};

/*	structure of XHEN0	(288C_0028h)	*/
/*	structure of XHEN1	(288C_006Ch)	*/
union io_xch_xhen{
	unsigned long		word;
	struct {
		unsigned long	XHEN	:32;
	}bit;
};

/*	structure of XSHW0	(288C_002Ch)	*/
/*	structure of XSHW1	(288C_0070h)	*/
union io_xch_xshw{
	unsigned long		word;
	struct {
		unsigned long	XSHW	:16;
		unsigned long			:16;
	}bit;
};

/*	structure of XGHW0	(288C_0030h)	*/
/*	structure of XGHW1	(288C_0074h)	*/
union io_xch_xghw{
	unsigned long		word;
	struct {
		unsigned long	XGHW	:16;
		unsigned long			:16;
	}bit;
};

/*	structure of XDVW0	(288C_0034h)	*/
/*	structure of XDVW1	(288C_0078h)	*/
union io_xch_xdvw{
	unsigned long		word;
	struct {
		unsigned long	XDVW	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of XDHW0	(288C_0038h)	*/
/*	structure of XDHW1	(288C_007Ch)	*/
union io_xch_xdhw{
	unsigned long		word;
	struct {
		unsigned long	XDHW	:15;
		unsigned long			:17;
	}bit;
};

/*	structure of XSA0	(288C_003Ch)	*/
/*	structure of XSA1	(288C_0080h)	*/
union io_xch_xsa{
	unsigned long		word;
	struct {
		unsigned long	XSA	:32;
	}bit;
};

/*	structure of XDA0	(288C_0040h)	*/
/*	structure of XDA1	(288C_0084h)	*/
union io_xch_xda{
	unsigned long		word;
	struct {
		unsigned long	XDA	:32;
	}bit;
};

/*	structure of XHISTG0	(288C_0044h)	*/
union io_xch_xhistg0{
	unsigned long		word;
	struct {
		unsigned long	XHMAX0	:8;
		unsigned long	XHREN0	:1;
		unsigned long			:3;
		unsigned long	XHCNT0	:1;
		unsigned long			:19;
	}bit;
};

/*	structure of XHBF	(288C_0400h)	*/
union io_xch_xhbf{
	unsigned long		word;
	struct {
		unsigned long	XHBF	:24;
		unsigned long			:8;
	}bit;
};

/* Xch control of each channel common */
struct io_xch_xctl {
	union  io_xch_xtrg		XTRG;					/* 288C_(000C - 000Fh) */ /* 288C_(0050 - 0053h) */

	unsigned char dmy_0010_0013[0x0014-0x0010];		/* 288C_(0010 - 0013h) */ /* 288C_(0054 - 0057h) */

	union  io_xch_xdtype	XDTYPE;					/* 288C_(0014 - 0017h) */ /* 288C_(0058 - 005Bh) */
	union  io_xch_xmd		XMD;					/* 288C_(0018 - 001Bh) */ /* 288C_(005C - 005Fh) */
	union  io_xch_xfdt		XFDT;					/* 288C_(001C - 001Fh) */ /* 288C_(0060 - 0063h) */
	union  io_xch_xcyc		XCYC;					/* 288C_(0020 - 0023h) */ /* 288C_(0064 - 0067h) */
	union  io_xch_xven		XVEN;					/* 288C_(0024 - 0027h) */ /* 288C_(0068 - 006Bh) */
	union  io_xch_xhen		XHEN;					/* 288C_(0028 - 002Bh) */ /* 288C_(006C - 006Fh) */
	union  io_xch_xshw		XSHW;					/* 288C_(002C - 002Fh) */ /* 288C_(0070 - 0073h) */
	union  io_xch_xghw		XGHW;					/* 288C_(0030 - 0033h) */ /* 288C_(0074 - 0077h) */
	union  io_xch_xdvw		XDVW;					/* 288C_(0034 - 0037h) */ /* 288C_(0078 - 007Bh) */
	union  io_xch_xdhw		XDHW;					/* 288C_(0038 - 003Bh) */ /* 288C_(007C - 007Fh) */
	union  io_xch_xsa		XSA;					/* 288C_(003C - 003Fh) */ /* 288C_(0080 - 0083h) */
	union  io_xch_xda		XDA;					/* 288C_(0040 - 0043h) */ /* 288C_(0084 - 0087h) */
};

/* Define i/o mapping */
struct io_jdsxch{
	/* JDSIMG */
	union  io_xch_sr		SR;									/* 288C_(0000 - 0003h) */
	union  io_xch_xchice	XCHICE;								/* 288C_(0004 - 0007h) */
	union  io_xch_xchicf	XCHICF;								/* 288C_(0008 - 000Bh) */
	struct io_xch_xctl		XCHCTL0;							/* 288C_(000C - 0043h) */

	union  io_xch_xhistg0	XHISTG0;							/* 288C_(0044 - 0047h) */

	unsigned char dmy_0048_004F[0x0050-0x0048];					/* 288C_(0048 - 004Fh) */

	struct io_xch_xctl		XCHCTL1;							/* 288C_(0050 - 0087h) */

	unsigned char dmy_0088_03FF[0x0400-0x0088];					/* 288C_(0088 - 03FFh) */

	union  io_xch_xhbf		XHBF[256];							/* 288C_(0400 - 07FFh) */

	unsigned char dmy_288C0800_288CFFFF[0x288E0000-0x288C0800];	/* 288C_0800 - 288C_FFFFh */

};

extern volatile struct io_jdsxch	IO_XCH;

