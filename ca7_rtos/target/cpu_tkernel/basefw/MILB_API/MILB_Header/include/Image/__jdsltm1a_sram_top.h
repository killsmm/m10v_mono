/**
 * @file		__jdsltm1a_sram_top.h
 * @brief		Definition JDSLTM1A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#include "__jdsltm1a_sram_map.h"
#include "__jdsltm1a_sram_ltm.h"

/* Define i/o mapping */
struct io_ltm_sram{
	/* JDSLTM */
	struct io_ltm_map_sram	MAP_SRAM;	/* 2949_(0000 - 0FFFh) */
	struct io_ltm_ltm_sram	LTM_SRAM;	/* 2949_(1000 - 1FFFh) */

	unsigned char dmy_29492000_2949FFFF[0x294A0000-0x29492000];	/* 2949_2000 - 2949_FFFFh */

};

extern volatile struct io_ltm_sram		IO_LTM_TBL_P1;
extern volatile struct io_ltm_sram		IO_LTM_TBL_P2;
extern volatile struct io_ltm_sram		IO_LTM_TBL_P3;
