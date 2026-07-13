/**
 * @file		__jdsltm1a_sram_map.h
 * @brief		Definition JDSLTM1A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


/*	structure of MYLOG0TBL0 (2949_0000h)	*/
union io_ltm_map_sram_mylog0tbl0{
	unsigned short		hword[512];
	struct {
		unsigned short	YLOG0TBL0	:13;
		unsigned short				:3;
	}bit[512];
};

/*	structure of MYLOG1TBL0 (2949_0400h)	*/
union io_ltm_map_sram_mylog1tbl0{
	unsigned short		hword[512];
	struct {
		unsigned short	YLOG1TBL0	:13;
		unsigned short				:3;
	}bit[512];
};

/*	structure of MYLOG0TBL1 (2949_0800h)	*/
union io_ltm_map_sram_mylog0tbl1{
	unsigned short		hword[512];
	struct {
		unsigned short	YLOG0TBL1	:13;
		unsigned short				:3;
	}bit[512];
};

/*	structure of MYLOG1TBL1 (2949_0C00h)	*/
union io_ltm_map_sram_mylog1tbl1{
	unsigned short		hword[512];
	struct {
		unsigned short	YLOG0TBL1	:13;
		unsigned short				:3;
	}bit[512];
};

/* Define i/o mapping */
struct io_ltm_map_sram{
	/* JDSLTM */
	union	io_ltm_map_sram_mylog0tbl0	MYLOG0TBL0;	/* 2949_(0000 - 03FFh) */
	union	io_ltm_map_sram_mylog1tbl0	MYLOG1TBL0;	/* 2949_(0400 - 07FFh) */
	union	io_ltm_map_sram_mylog0tbl1	MYLOG0TBL1;	/* 2949_(0800 - 0BFFh) */
	union	io_ltm_map_sram_mylog1tbl1	MYLOG1TBL1;	/* 2949_(0C00 - 0FFFh) */
};
