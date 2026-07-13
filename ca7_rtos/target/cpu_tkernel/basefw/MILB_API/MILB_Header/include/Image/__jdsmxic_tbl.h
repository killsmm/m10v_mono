/**
 * @file		__jdsmxic_tbl.h
 * @brief		Definition MXIC TBL Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*  structure of TSLW		*/
union io_mxic_tslw{
	unsigned char		byte[4][16];
	struct {
		unsigned char	SLOTW	:7;
		unsigned char			:1;
	}bit[4][16];
};

/*  structure of TSLR		*/
union io_mxic_tslr{
	unsigned char		byte[4][16];
	struct {
		unsigned char	SLOTR	:7;
		unsigned char			:1;
	}bit[4][16];
};

/* Define i/o mapping */
struct io_jdsmxic_tbl{
	/* JDSIMG */
	union io_mxic_tslw		TSLW_1[4];					/* 29x0_(0000 - 00FFh) */
	union io_mxic_tslw		TSLW_2[4];					/* 29x0_(0100 - 01FFh) */
	union io_mxic_tslw		TSLW_3[4];					/* 29x0_(0200 - 02FFh) */
	union io_mxic_tslw		TSLW_4[4];					/* 29x0_(0300 - 03FFh) */
	union io_mxic_tslr		TSLR_1[4];					/* 29x0_(0400 - 04FFh) */
	union io_mxic_tslr		TSLR_2[4];					/* 29x0_(0500 - 05FFh) */
	union io_mxic_tslr		TSLR_3[4];					/* 29x0_(0600 - 06FFh) */
	union io_mxic_tslr		TSLR_4[4];					/* 29x0_(0700 - 07FFh) */

	unsigned char dmy_29x00800_29x0FFFF[0x10000-0x0800];	/* 29x0_(0800 - FFFFh) */
};

extern volatile struct io_jdsmxic_tbl	IO_MXIC1_TBL;
extern volatile struct io_jdsmxic_tbl	IO_MXIC2_TBL;
extern volatile struct io_jdsmxic_tbl	IO_MXIC3_TBL;

