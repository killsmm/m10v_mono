/**
 * @file		_jdsltm1a_top.h
 * @brief		Definition JDSLTM1A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#include "__jdsltm1a_rbk.h"
#include "__jdsltm1a_map.h"
#include "__jdsltm1a_ltm.h"
#include "__jdsltm1a_sram_top.h"

/* Define i/o mapping */
struct io_ltm_top{
	/* JDSLTM */
	struct io_ltm_ch_rbk	RBK;				/* 2841_(3000 - 31FFh) */

	unsigned char dmy_3200_33FF[0x0400-0x0200];	/* 2841_(3200 - 33FFh) */

	struct io_ltm_ch_map	MAP;				/* 2841_(3400 - 37FFh) */
	struct io_ltm_ch_ltm	LTM;				/* 2841_(3800 - 3BFFh) */

	unsigned char dmy_3C00_3FFF[0x1000-0x0C00];	/* 2841_(3C00 - 3FFFh) */

};

extern volatile struct io_ltm_top		IO_LTM_P1;
extern volatile struct io_ltm_top		IO_LTM_P2;
extern volatile struct io_ltm_top		IO_LTM_P3;

