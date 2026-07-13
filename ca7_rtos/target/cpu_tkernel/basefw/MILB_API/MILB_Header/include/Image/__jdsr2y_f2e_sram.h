/**
 * @file		__jdsr2y_f2e_sram.h
 * @brief		Definition JDSR2Y_F2E Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/*	structure of TC	(294A_0000h)	*/
union io_r2y_sram_tc{
	unsigned short		hword[1024];
	struct {
		unsigned short	TC	:15;
		unsigned short		:1;
	}bit[1024];
};

/*	structure of TCTDATA	(294A_2000h)	*/
union io_r2y_sram_tctdata{
	unsigned long		word[1200];
	struct {
		unsigned long	TCTDATA	:24;
		unsigned long			:8;
	}bit[1200];
};

/*	structure of TCHSYDATA	(294A_4000h)	*/
union io_r2y_sram_tchsydata{
	unsigned short		hword[4096];
	struct {
		unsigned short	TCHSYDATA	:16;
	}bit[4096];
};

/*	structure of TCHSRDATA	(294A_6000h)	*/
union io_r2y_sram_tchsrdata{
	unsigned short		hword[256];
	struct {
		unsigned short	TCHSRDATA	:16;
	}bit[256];
};

/*	structure of TCHSGDATA	(294A_6200h)	*/
union io_r2y_sram_tchsgdata{
	unsigned short		hword[256];
	struct {
		unsigned short	TCHSGDATA	:16;
	}bit[256];
};

/*	structure of TCHSBDATA	(294A_6400h)	*/
union io_r2y_sram_tchsbdata{
	unsigned short		hword[256];
	struct {
		unsigned short	TCHSBDATA	:16;
	}bit[256];
};

/*	structure of GMRGBDF	(294A_8000h)	*/
union io_r2y_sram_gmrgbdf{
	unsigned long long		dword[256];
	struct {
		unsigned long long	GMRGBDF	:64;
	}bit[256];
};

/*	structure of GMRDF	(294A_8800h)	*/
union io_r2y_sram_gmrdf{
	unsigned long long		dword[256];
	struct {
		unsigned long long	GMRDF	:64;
	}bit[256];
};

/*	structure of GMGDF	(294A_9000h)	*/
union io_r2y_sram_gmgdf{
	unsigned long long		dword[256];
	struct {
		unsigned long long	GMGDF	:64;
	}bit[256];
};

/*	structure of GMBDF	(294A_9800h)	*/
union io_r2y_sram_gmbdf{
	unsigned long long		dword[256];
	struct {
		unsigned long long	GMBDF	:64;
	}bit[256];
};

/*	structure of GMYBDF	(294A_A000h)	*/
union io_r2y_sram_gmybdf{
	unsigned long long		dword[256];
	struct {
		unsigned long long	GMYBDF	:64;
	}bit[256];
};

/*	structure of GMRGBFL	(294A_A800h)	*/
union io_r2y_sram_gmrgbfl{
	unsigned short		hword[256];
	struct {
		unsigned short	GMRGBFL	:10;
		unsigned short			:6;
	}bit[256];
};

/*	structure of GMRFL	(294A_AA00h)	*/
union io_r2y_sram_gmrfl{
	unsigned short		hword[256];
	struct {
		unsigned short	GMRFL	:10;
		unsigned short			:6;
	}bit[256];
};

/*	structure of GMGFL	(294A_AC00h)	*/
union io_r2y_sram_gmgfl{
	unsigned short		hword[256];
	struct {
		unsigned short	GMGFL	:10;
		unsigned short			:6;
	}bit[256];
};

/*	structure of GMBFL	(294A_AE00h)	*/
union io_r2y_sram_gmbfl{
	unsigned short		hword[256];
	struct {
		unsigned short	GMBFL	:10;
		unsigned short			:6;
	}bit[256];
};

/*	structure of GMYBFL	(294A_B000h)	*/
union io_r2y_sram_gmybfl{
	unsigned short		hword[256];
	struct {
		unsigned short	GMYBFL	:10;
		unsigned short			:6;
	}bit[256];
};

/*	structure of EGHWSCL	(294A_C000h)	*/
union io_r2y_sram_eghwscl{
	unsigned char		byte[512];
	struct {
		unsigned char	EGHWSCL	:8;
	}bit[512];
};

/*	structure of EGHWTON	(294A_C200h)	*/
union io_r2y_sram_eghwton{
	unsigned short		hword[256];
	struct {
		unsigned short	EGHWTON	:9;
		unsigned short			:7;
	}bit[256];
};

/*	structure of EGMWSCL	(294A_C400h)	*/
union io_r2y_sram_egmwscl{
	unsigned char		byte[512];
	struct {
		unsigned char	EGMWSCL	:8;
	}bit[512];
};

/*	structure of EGMWTON	(294A_C600h)	*/
union io_r2y_sram_egmwton{
	unsigned short		hword[256];
	struct {
		unsigned short	EGMWTON	:9;
		unsigned short			:7;
	}bit[256];
};

/*	structure of EGLWSCL	(294A_C800h)	*/
union io_r2y_sram_eglwscl{
	unsigned char		byte[512];
	struct {
		unsigned char	EGLWSCL	:8;
	}bit[512];
};

/*	structure of EGLWTON	(294A_CA00h)	*/
union io_r2y_sram_eglwton{
	unsigned short		hword[256];
	struct {
		unsigned short	EGLWTON	:9;
		unsigned short			:7;
	}bit[256];
};

/*	structure of EGMPSCL	(294A_CC00h)	*/
union io_r2y_sram_egmpscl{
	unsigned short		hword[13][17];
	struct {
		unsigned short	EGMPSCL	:10;
		unsigned short			:6;
	}bit[13][17];
};

/*	structure of DKNTBLR	(294A_E000h)	*/
/*	structure of DKNTBLG	(294A_E200h)	*/
/*	structure of DKNTBLB	(294A_E400h)	*/
union io_r2y_sram_dkntbl{
	unsigned short		hword[256];
	struct {
		unsigned short	DKNTBL	:12;
		unsigned short			:4;
	}bit[256];
};

/*	structure of HSTY	(294A_F000h)	*/
union io_r2y_sram_hsty{
	unsigned short		hword[128];
	struct {
		unsigned short	HSTY	:15;
		unsigned short			:1;
	}bit[128];
};

/* Define i/o mapping */
struct io_r2y_sram{
	/* JDSR2Y */
	union  io_r2y_sram_tc			TC;			/* 294A_(0000 - 07FFh) */

	unsigned char dmy_0800_1FFF[0x2000-0x0800];	/* 294A_(0800 - 1FFFh) */

	union  io_r2y_sram_tctdata		TCTDATA;	/* 294A_(2000 - 32BFh) */

	unsigned char dmy_32C0_3FFF[0x4000-0x32C0];	/* 294A_(32C0 - 3FFFh) */

	union  io_r2y_sram_tchsydata	TCHSYDATA;	/* 294A_(4000 - 5FFFh) */
	union  io_r2y_sram_tchsrdata	TCHSRDATA;	/* 294A_(6000 - 61FFh) */
	union  io_r2y_sram_tchsgdata	TCHSGDATA;	/* 294A_(6200 - 63FFh) */
	union  io_r2y_sram_tchsbdata	TCHSBDATA;	/* 294A_(6400 - 65FFh) */

	unsigned char dmy_6600_7FFF[0x8000-0x6600];	/* 294A_(6600 - 7FFFh) */

	union  io_r2y_sram_gmrgbdf		GMRGBDF;	/* 294A_(8000 - 87FFh) */
	union  io_r2y_sram_gmrdf		GMRDF;		/* 294A_(8800 - 8FFFh) */
	union  io_r2y_sram_gmgdf		GMGDF;		/* 294A_(9000 - 97FFh) */
	union  io_r2y_sram_gmbdf		GMBDF;		/* 294A_(9800 - 9FFFh) */
	union  io_r2y_sram_gmybdf		GMYBDF;		/* 294A_(A000 - A7FFh) */
	union  io_r2y_sram_gmrgbfl		GMRGBFL;	/* 294A_(A800 - A9FFh) */
	union  io_r2y_sram_gmrfl		GMRFL;		/* 294A_(AA00 - ABFFh) */
	union  io_r2y_sram_gmgfl		GMGFL;		/* 294A_(AC00 - ADFFh) */
	union  io_r2y_sram_gmbfl		GMBFL;		/* 294A_(AE00 - AFFFh) */
	union  io_r2y_sram_gmybfl		GMYBFL;		/* 294A_(B000 - B1FFh) */

	unsigned char dmy_B200_BFFF[0xC000-0xB200];	/* 294A_(B200 - BFFFh) */

	union  io_r2y_sram_eghwscl		EGHWSCL;	/* 294A_(C000 - C1FFh) */
	union  io_r2y_sram_eghwton		EGHWTON;	/* 294A_(C200 - C3FFh) */
	union  io_r2y_sram_egmwscl		EGMWSCL;	/* 294A_(C400 - C5FFh) */
	union  io_r2y_sram_egmwton		EGMWTON;	/* 294A_(C600 - C7FFh) */
	union  io_r2y_sram_eglwscl		EGLWSCL;	/* 294A_(C800 - C9FFh) */
	union  io_r2y_sram_eglwton		EGLWTON;	/* 294A_(CA00 - CBFFh) */
	union  io_r2y_sram_egmpscl		EGMPSCL;	/* 294A_(CC00 - CDB9h) */

	unsigned char dmy_CDBC_DFFF[0xE000-0xCDBA];	/* 294A_(CDBA - DFFFh) */

	union  io_r2y_sram_dkntbl		DKNTBLR;	/* 294A_(E000 - E1FFh) */
	union  io_r2y_sram_dkntbl		DKNTBLG;	/* 294A_(E200 - E3FFh) */
	union  io_r2y_sram_dkntbl		DKNTBLB;	/* 294A_(E400 - E5FFh) */

	unsigned char dmy_E600_EFFF[0xF000-0xE600];	/* 294A_(E600 - EFFFh) */

	union  io_r2y_sram_hsty			HSTY;		/* 294A_(F000 - F0FFh) */

	unsigned char dmy_294AF100_294AFFFF[0x294B0000-0x294AF100];	/* 294A_F100 - 294A_FFFFh */

};

extern volatile struct io_r2y_sram		IO_R2Y_TBL_P1;
extern volatile struct io_r2y_sram		IO_R2Y_TBL_P2;
extern volatile struct io_r2y_sram		IO_R2Y_TBL_P3;

