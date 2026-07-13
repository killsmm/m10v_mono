/**
 * @file		_jdsfpt_ahb.h
 * @brief		Definition JDSFPT Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

/*	structure of FPTLUT	(298C_1000h)	*/
union io_fpt_fptlut {
	unsigned char		byte[256];
	unsigned long		word[64];
};

/*	structure of FPTREG	(298C_1400h)	*/
union io_fpt_fptreg {
	unsigned char		byte[512];
	unsigned long		word[128];
};

/* Define i/o mapping */
struct io_jdsfpt_ahb {
	union io_fpt_fptlut		FPTLUT0;				/* 298C_(1000 - 10FFh) */
	union io_fpt_fptlut		FPTLUT1;				/* 298C_(1100 - 11FFh) */
	
	unsigned char dmy_0200_03FF[0x0400-0x0200];		/* 298C_(1200 - 13FFh) */
	
	union io_fpt_fptreg		FPTREG0;				/* 298C_(1400 - 15FFh) */
	union io_fpt_fptreg		FPTREG1;				/* 298C_(1600 - 17FFh) */
};

extern volatile struct io_jdsfpt_ahb		IO_FPT_AHB[2];

