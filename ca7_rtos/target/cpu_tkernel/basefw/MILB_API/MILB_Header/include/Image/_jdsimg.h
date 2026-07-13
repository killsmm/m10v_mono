/**
 * @file		_jdsimg.h 
 * @brief		Definition JDSIMG Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*	structure of IMGPIPEPSW	(2880_0000h)	*/
union io_imgpipepsw{
	unsigned long		word;
	struct {
		unsigned long	R2YSEL	:2;
		unsigned long			:2;
		unsigned long	B2RCNC	:2;
		unsigned long			:26;
	}bit;
};

/*	structure of MXICMON	(2880_0004h)	*/
union io_mxicmon{
	unsigned long		word;
	struct {
		unsigned long	VDEN	:1;
		unsigned long	HDEN	:1;
		unsigned long			:1;
		unsigned long	SENEN	:1;
		unsigned long	SRO1EN	:1;
		unsigned long	SRO2EN	:1;
		unsigned long	B2B1EN	:1;
		unsigned long	B2B2EN	:1;
		unsigned long	YO1EN	:1;
		unsigned long	YO2EN	:1;
		unsigned long	STATEN	:1;
		unsigned long	PASEN	:1;
		unsigned long	DEN		:1;
		unsigned long			:19;
	}bit;
};

/*	structure of SRAMPD		(2880_0008h)	*/
union io_srampd{
	unsigned long		word;
	struct {
		unsigned long	JPEGPD	:4;
		unsigned long			:28;
	}bit;
};

/* Define i/o mapping */
struct io_img{
	/* JDSIMG */
	union  io_imgpipepsw	IMGPIPEPSW;					/* 2880_(0000 - 0003h) */
	union  io_mxicmon		MXICMON;					/* 2880_(0004 - 0007h) */
	union  io_srampd		SRAMPD;						/* 2880_(0008 - 000Bh) */

	unsigned char dmy_2880000C_2880FFFF[0x28810000-0x2880000C];	/* 2880_000C - 2880_FFFFh */
};

extern volatile struct io_img		IO_IMG;

