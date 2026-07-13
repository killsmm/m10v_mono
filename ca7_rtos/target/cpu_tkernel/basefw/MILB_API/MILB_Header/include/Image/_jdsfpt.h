/**
 * @file		_jdsfpt.h
 * @brief		Definition JDSFPT Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

/*	structure of FPTCTL0	(288D_0000h)	*/
union io_fpt_fptctl0{
	unsigned long		word;
	struct {
		unsigned long	SR	:1;
		unsigned long		:31;
	}bit;
};

/*	structure of FPTCTL1	(288D_0004h)	*/
union io_fpt_fptctl1{
	unsigned long		word;
	struct {
		unsigned long	TRG	:2;
		unsigned long		:30;
	}bit;
};

/*	structure of FPTCTL2	(288D_0008h)	*/
union io_fpt_fptctl2{
	unsigned long		word;
	struct {
		unsigned long	RAMSLP		:1;
		unsigned long				:15;
		unsigned long	MODE		:2;
		unsigned long				:2;
		unsigned long	IO_MODE		:1;
		unsigned long				:3;
		unsigned long	PIPE_EN		:1;
		unsigned long				:3;
		unsigned long	DEBUG_MODE	:1;
		unsigned long				:3;
	}bit;
};

/*	structure of FPTFQCTL0	(288D_0010h)	*/
union io_fpt_fptfqctl0{
	unsigned long		word;
	struct {
		unsigned long	GHSIZE	:15;
		unsigned long			:17;
	}bit;
};

/*	structure of FPTFQCTL1	(288D_0014h)	*/
union io_fpt_fptfqctl1{
	unsigned long		word;
	struct {
		unsigned long	HSIZE	:15;
		unsigned long			:1;
		unsigned long	VSIZE	:15;
		unsigned long			:1;
	}bit;
};

/*	structure of FPTFQCTL2	(288D_0018h)	*/
union io_fpt_fptfqctl2{
	unsigned long		word;
	struct {
		unsigned long	FPMAX	:7;
		unsigned long			:1;
		unsigned long	SUMW	:1;
		unsigned long			:3;
		unsigned long	OUTEN	:1;
		unsigned long			:19;
	}bit;
};

/*	structure of FPTFQCTL3	(288D_001Ch)	*/
union io_fpt_fptfqctl3{
	unsigned long		word;
	struct {
		unsigned long	SDAD_FQ_OF	:32;
	}bit;
};

/*	structure of FPTFQCTL4	(288D_0020h)	*/
union io_fpt_fptfqctl4{
	unsigned long		word;
	struct {
		unsigned long	SDAD_FQ_MP	:32;
	}bit;
};

/*	structure of FPTFQCTL5	(288D_0024h)	*/
union io_fpt_fptfqctl5{
	unsigned long		word;
	struct {
		unsigned long	SDAD_FQ_HI	:32;
	}bit;
};

/*	structure of FPTFQCTL6	(288D_0028h)	*/
union io_fpt_fptfqctl6{
	unsigned long		word;
	struct {
		unsigned long	SDAD_FQ_CXY	:32;
	}bit;
};

/*	structure of FPTFQCTL7	(288D_002Ch)	*/
union io_fpt_fptfqctl7{
	unsigned long		word;
	struct {
		unsigned long	SDAD_FQ_RXY	:32;
	}bit;
};

/*	structure of FPTFQCTL8	(288D_0030h)	*/
union io_fpt_fptfqctl8{
	unsigned long		word;
	struct {
		unsigned long	MAPGHSIZE	:15;
		unsigned long				:17;
	}bit;
};

/*	structure of FPTFQCTL9	(288D_0034h)	*/
union io_fpt_fptfqctl9{
	unsigned long		word;
	struct {
		unsigned long	SKIPX_FQ	:13;
		unsigned long				:3;
		unsigned long	SKIPY_FQ	:13;
		unsigned long				:3;
	}bit;
};

/*	structure of FPTFTCTL0	(288D_0040h)	*/
union io_fpt_fptftctl0{
	unsigned long		word;
	struct {
		unsigned long	GHBSIZE	:15;
		unsigned long			:1;
		unsigned long	GHTSIZE	:15;
		unsigned long			:1;
	}bit;
};

/*	structure of FPTFTCTL1	(288D_0044h)	*/
union io_fpt_fptftctl1{
	unsigned long		word;
	struct {
		unsigned long	FTMAX	:7;
		unsigned long			:1;
		unsigned long	INIT_0	:1;
		unsigned long			:23;
	}bit;
};

/*	structure of FPTFTCTL2	(288D_0048h)	*/
union io_fpt_fptftctl2{
	unsigned long		word;
	struct {
		unsigned long	LEFTLIMIT	:12;
		unsigned long				:4;
		unsigned long	RIGHTLIMIT	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of FPTFTCTL3	(288D_004Ch)	*/
union io_fpt_fptftctl3{
	unsigned long		word;
	struct {
		unsigned long	TOPLIMIT	:12;
		unsigned long				:4;
		unsigned long	BOTTOMLIMIT	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of FPTFTCTL4	(288D_0050h)	*/
union io_fpt_fptftctl4{
	unsigned long		word;
	struct {
		unsigned long	MAXERR	:12;
		unsigned long			:4;
		unsigned long	MAXITR	:4;
		unsigned long			:12;
	}bit;
};

/*	structure of FPTFTCTL5	(288D_0054h)	*/
union io_fpt_fptftctl5{
	unsigned long		word;
	struct {
		unsigned long	SDAD_BP_OF	:32;
	}bit;
};

/*	structure of FPTFTCTL6	(288D_0058h)	*/
union io_fpt_fptftctl6{
	unsigned long		word;
	struct {
		unsigned long	SDAD_TP_OF	:32;
	}bit;
};

/*	structure of FPTFTCTL7	(288D_005Ch)	*/
union io_fpt_fptftctl7{
	unsigned long		word;
	struct {
		unsigned long	SDAD_FT_CXY	:32;
	}bit;
};

/*	structure of FPTFTCTL8	(288D_0060h)	*/
union io_fpt_fptftctl8{
	unsigned long		word;
	struct {
		unsigned long	SDAD_FT_BXY	:32;
	}bit;
};

/*	structure of FPTFTCTL9	(288D_0064h)	*/
union io_fpt_fptftctl9{
	unsigned long		word;
	struct {
		unsigned long	SDAD_FT_TXY	:32;
	}bit;
};

/*	structure of FPTFTCTL10	(288D_0068h)	*/
union io_fpt_fptftctl10{
	unsigned long		word;
	struct {
		unsigned long	SKIPX_FT	:13;
		unsigned long				:3;
		unsigned long	SKIPY_FT	:13;
		unsigned long				:3;
	}bit;
};

/*	structure of FPTINTE0	(288D_0070h)	*/
union io_fpt_fptinte0{
	unsigned long		word;
	struct {
		unsigned long	FPTEN	:1;
		unsigned long			:3;
		unsigned long	AXEEN	:1;
		unsigned long			:3;
		unsigned long	PMEEN	:1;
		unsigned long			:23;
	}bit;
};

/*	structure of FPTINTF0	(288D_0074h)	*/
union io_fpt_fptintf0{
	unsigned long		word;
	struct {
		unsigned long	__FPTFLG	:1;
		unsigned long				:3;
		unsigned long	__AXEFLG	:1;
		unsigned long				:3;
		unsigned long	__PMEFLG	:1;
		unsigned long				:23;
	}bit;
};

/*	structure of FPTAXICTL0	(288D_0080h)	*/
union io_fpt_fptaxictl0{
	unsigned long		word;
	struct {
		unsigned long	ARCACHE	:4;
		unsigned long	ARPROT	:3;
		unsigned long			:1;
		unsigned long	AWCACHE	:4;
		unsigned long	AWPROT	:3;
		unsigned long			:1;
		unsigned long	PKGEN	:1;
		unsigned long			:15;
	}bit;
};

/*	structure of FPTAXIERR0	(288D_0084h)	*/
union io_fpt_fptaxierr0{
	unsigned long		word;
	struct {
		unsigned long	RRESP	:2;
		unsigned long			:6;
		unsigned long	WRESP	:2;
		unsigned long			:22;
	}bit;
};

/*	structure of FPTCTL3	(288D_0090h)	*/
union io_fpt_fptctl3{
	unsigned long		word;
	struct {
		unsigned long	BRC_EN	:1;
		unsigned long			:7;
		unsigned long	BRC_MOD	:2;
		unsigned long			:6;
		unsigned long	LUT_SEL	:1;
		unsigned long			:15;
	}bit;
};

/*	structure of FPTCTL4	(288D_0094h)	*/
union io_fpt_fptctl4{
	unsigned long		word;
	struct {
		unsigned long	CLPMIN_B	:8;
		unsigned long	CLPMAX_B	:8;
		long			OFFSET_B	:9;
		unsigned long				:7;
	}bit;
};

/*	structure of FPTCTL5	(288D_0098h)	*/
union io_fpt_fptctl5{
	unsigned long		word;
	struct {
		unsigned long	CLPMIN_T	:8;
		unsigned long	CLPMAX_T	:8;
		long			OFFSET_T	:9;
		unsigned long				:7;
	}bit;
};

/* Define i/o mapping */
struct io_jdsfpt{
	/* JDSFPT */
	union	io_fpt_fptctl0		FPTCTL0;		/* 288D_(0000 - 0003h) */
	union	io_fpt_fptctl1		FPTCTL1;		/* 288D_(0004 - 0007h) */
	union	io_fpt_fptctl2		FPTCTL2;		/* 288D_(0008 - 000Bh) */

	unsigned char dmy_000C_000F[0x0010-0x000C];	/* 288D_(000C - 000Fh) */

	union	io_fpt_fptfqctl0	FPTFQCTL0;		/* 288D_(0010 - 0013h) */
	union	io_fpt_fptfqctl1	FPTFQCTL1;		/* 288D_(0014 - 0017h) */
	union	io_fpt_fptfqctl2	FPTFQCTL2;		/* 288D_(0018 - 001Bh) */
	union	io_fpt_fptfqctl3	FPTFQCTL3;		/* 288D_(001C - 001Fh) */
	union	io_fpt_fptfqctl4	FPTFQCTL4;		/* 288D_(0020 - 0023h) */
	union	io_fpt_fptfqctl5	FPTFQCTL5;		/* 288D_(0024 - 0027h) */
	union	io_fpt_fptfqctl6	FPTFQCTL6;		/* 288D_(0028 - 002Bh) */
	union	io_fpt_fptfqctl7	FPTFQCTL7;		/* 288D_(002C - 002Fh) */
	union	io_fpt_fptfqctl8	FPTFQCTL8;		/* 288D_(0030 - 0033h) */
	union	io_fpt_fptfqctl9	FPTFQCTL9;		/* 288D_(0034 - 0037h) */

	unsigned char dmy_0038_003F[0x0040-0x0038];	/* 288D_(0038 - 003Fh) */

	union	io_fpt_fptftctl0	FPTFTCTL0;		/* 288D_(0040 - 0043h) */
	union	io_fpt_fptftctl1	FPTFTCTL1;		/* 288D_(0044 - 0047h) */
	union	io_fpt_fptftctl2	FPTFTCTL2;		/* 288D_(0048 - 004Bh) */
	union	io_fpt_fptftctl3	FPTFTCTL3;		/* 288D_(004C - 004Fh) */
	union	io_fpt_fptftctl4	FPTFTCTL4;		/* 288D_(0050 - 0053h) */
	union	io_fpt_fptftctl5	FPTFTCTL5;		/* 288D_(0054 - 0057h) */
	union	io_fpt_fptftctl6	FPTFTCTL6;		/* 288D_(0058 - 005Bh) */
	union	io_fpt_fptftctl7	FPTFTCTL7;		/* 288D_(005C - 005Fh) */
	union	io_fpt_fptftctl8	FPTFTCTL8;		/* 288D_(0060 - 0063h) */
	union	io_fpt_fptftctl9	FPTFTCTL9;		/* 288D_(0064 - 0067h) */
	union	io_fpt_fptftctl10	FPTFTCTL10;		/* 288D_(0068 - 006Bh) */

	unsigned char dmy_006C_006F[0x0070-0x006C];	/* 288D_(006C - 006Fh) */

	union	io_fpt_fptinte0		FPTINTE0;		/* 288D_(0070 - 0073h) */
	union	io_fpt_fptintf0		FPTINTF0;		/* 288D_(0074 - 0077h) */

	unsigned char dmy_0078_007F[0x0080-0x0078];	/* 288D_(0078 - 007Fh) */

	union	io_fpt_fptaxictl0	FPTAXICTL0;		/* 288D_(0080 - 0083h) */
	union	io_fpt_fptaxierr0	FPTAXIERR0;		/* 288D_(0084 - 0087h) */

	unsigned char dmy_0088_008F[0x0090-0x0088];	/* 288D_(0088 - 008Fh) */

	union	io_fpt_fptctl3		FPTCTL3;		/* 288D_(0090 - 0093h) */
	union	io_fpt_fptctl4		FPTCTL4;		/* 288D_(0094 - 0097h) */
	union	io_fpt_fptctl5		FPTCTL5;		/* 288D_(0098 - 009Bh) */

	unsigned char dmy_009C_00FF[0x0100-0x009C];	/* 288D_(009C - 00FFh) */

};

extern volatile struct io_jdsfpt		IO_FPT[2];

