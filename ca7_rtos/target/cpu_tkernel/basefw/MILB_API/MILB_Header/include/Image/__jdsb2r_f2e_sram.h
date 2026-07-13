/**
 * @file		__jdsb2r_f2e_sram.h
 * @brief		Definition JDSB2R_F2E Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/*	structure of DKNTBL	(2948_2000h)	*/
union io_b2r_sram_dkntbl{
	unsigned short		hword[256];
	struct {
		unsigned short	DKNTBL	:12;
		unsigned short			:4;
	}bit[256];
};

/*	structure of KNETBLR	(2948_2800h)	*/
union io_b2r_sram_knetblr{
	unsigned char		byte[257];
	struct {
		unsigned char	KNETBLR	:8;
	}bit[257];
};

/*	structure of KNETBLG	(2948_2A00h)	*/
union io_b2r_sram_knetblg{
	unsigned char		byte[257];
	struct {
		unsigned char	KNETBLG	:8;
	}bit[257];
};

/*	structure of KNETBLB	(2948_2C00h)	*/
union io_b2r_sram_knetblb{
	unsigned char		byte[257];
	struct {
		unsigned char	KNETBLB	:8;
	}bit[257];
};

/* Define i/o mapping */
struct io_b2r_sram{
	/* JDSB2R */
	union  io_b2r_sram_dkntbl	DKNTBL;			/* 2948_(2000 - 21FFh) */

	unsigned char dmy_2200_27FF[0x2800-0x2200];	/* 2948_(2200 - 27FFh) */

	union  io_b2r_sram_knetblr	KNETBLR;		/* 2948_(2800 - 2900h) */

	unsigned char dmy_2901_29FF[0x2A00-0x2901];	/* 2948_(2901 - 29FFh) */

	union  io_b2r_sram_knetblg	KNETBLG;		/* 2948_(2A00 - 2B00h) */

	unsigned char dmy_2B01_2BFF[0x2C00-0x2B01];	/* 2948_(2B01 - 2BFFh) */

	union  io_b2r_sram_knetblb	KNETBLB;		/* 2948_(2C00 - 2D00h) */

	unsigned char dmy_2D01_2FFF[0x3000-0x2D01];	/* 2948_(2D01 - 2FFFh) */

};

extern volatile struct io_b2r_sram		IO_B2R_TBL_P1;
extern volatile struct io_b2r_sram		IO_B2R_TBL_P2;
extern volatile struct io_b2r_sram		IO_B2R_TBL_P3;

