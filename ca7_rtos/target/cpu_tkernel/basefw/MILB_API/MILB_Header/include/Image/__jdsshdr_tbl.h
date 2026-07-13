/**
 * @file		__jdsshdr_tbl.h
 * @brief		Definition SHDR TBL Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*	structure of DGTR		*/
union io_shdr_dgtr{
	unsigned short		hword[256];
	struct {
		unsigned short	DGTR		:12;
		unsigned short				:4;
	}bit[256];
};

/*	structure of DGTG		*/
union io_shdr_dgtg{
	unsigned short		hword[256];
	struct {
		unsigned short	DGTG		:12;
		unsigned short				:4;
	}bit[256];
};

/*	structure of DGTB		*/
union io_shdr_dgtb{
	unsigned short		hword[256];
	struct {
		unsigned short	DGTB		:12;
		unsigned short				:4;
	}bit[256];
};

/* Define i/o mapping */
struct io_jdsshdr_tbl{
	/* JDSSHDR */
	union  io_shdr_dgtr		DGTR;					/* (0000 - 01FFh) */
	union  io_shdr_dgtg		DGTG;					/* (0200 - 03FFh) */
	union  io_shdr_dgtb		DGTB;					/* (0400 - 05FFh) */

	unsigned char dmy_0600_07FF[0x00800-0x0600];	/* (0600 - 07FFh) */
};

extern volatile struct io_jdsshdr_tbl	IO_SHDR_TBL;

