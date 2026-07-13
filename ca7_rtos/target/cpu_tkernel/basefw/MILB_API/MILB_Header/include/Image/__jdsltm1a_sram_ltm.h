/**
 * @file		__jdsltm1a_sram_ltm.h
 * @brief		Definition JDSLTM1A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


/*	structure of LYLOG0TBL0 (2949_1000h)	*/
union io_ltm_ltm_sram_lylog0tbl0{
	unsigned short		hword[512];
	struct {
		unsigned short	YLOG0TBL0	:13;
		unsigned short				:3;
	}bit[512];
};

/*	structure of LYLOG1TBL0 (2949_1400h)	*/
union io_ltm_ltm_sram_lylog1tbl0{
	unsigned short		hword[512];
	struct {
		unsigned short	YLOG1TBL0	:13;
		unsigned short				:3;
	}bit[512];
};

/*	structure of LYLOG0TBL1 (2949_1800h)	*/
union io_ltm_ltm_sram_lylog0tbl1{
	unsigned short		hword[512];
	struct {
		unsigned short	YLOG0TBL1	:13;
		unsigned short				:3;
	}bit[512];
};

/*	structure of LYLOG1TBL1 (2949_1C00h)	*/
union io_ltm_ltm_sram_lylog1tbl1{
	unsigned short		hword[512];
	struct {
		unsigned short	YLOG1TBL1	:13;
		unsigned short				:3;
	}bit[512];
};

/* Define i/o mapping */
struct io_ltm_ltm_sram{
	/* JDSLTM */
	union	io_ltm_ltm_sram_lylog0tbl0	LYLOG0TBL0;	/* 2949_(1000 - 13FFh) */
	union	io_ltm_ltm_sram_lylog1tbl0	LYLOG1TBL0;	/* 2949_(1400 - 17FFh) */
	union	io_ltm_ltm_sram_lylog0tbl1	LYLOG0TBL1;	/* 2949_(1800 - 1BFFh) */
	union	io_ltm_ltm_sram_lylog1tbl1	LYLOG1TBL1;	/* 2949_(1C00 - 1FFFh) */
};
