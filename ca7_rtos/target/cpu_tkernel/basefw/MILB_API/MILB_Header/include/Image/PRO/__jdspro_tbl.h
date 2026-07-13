/**
 * @file		__jdspro_tbl.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_TABLE_H_
#define _IO_PRO_TABLE_H_

#include "__jdspro_sen_top_ahb.h"
#include "__jdspro_sro_top_ahb.h"
#include "__jdspro_b2b_top_ahb.h"
#include "__jdspro_stat_top_ahb.h"
#include "__jdspro_pas_top_ahb.h"

/* Define i/o mapping */
struct io_pro_tbl{
	/* JDSIMG */
	// IMG_APB0
	struct io_sen_ahb			SEN_TBL;	/* 2900_(0000 - 3FFFh) */

	unsigned char dmy_29004000_290FFFFF[0x29100000-0x29004000];	/* 2900_4000 - 290F_FFFF */

	unsigned char dmy_29100000_293FFFFF[0x29400000-0x29100000];	/* 2910_0000 - 293F_FFFF */

	// IMG_APB1(PIPE[1])
	unsigned char dmy_29400000_2943FFFF[0x29440000-0x29400000];	/* 2940_0000 - 2943_FFFF */

	struct io_sro_ahb			SRO1_TBL;	/* 2944_0000 - 2947_FFFFh */
	struct io_b2b_ahb			B2B1_TBL;	/* 2948_0000 - 2948_1FFFh */

	unsigned char dmy_29482000_294FFFFF[0x29500000-0x29482000];	/* 2948_2000 - 294F_FFFF */

	// IMG_APB1(PIPE[2])
	unsigned char dmy_29500000_2953FFFF[0x29540000-0x29500000];	/* 2950_0000 - 2953_FFFF */

	struct io_sro_ahb			SRO2_TBL;	/* 2954_0000 - 2957_FFFFh */
	struct io_b2b_ahb			B2B2_TBL;	/* 2958_0000 - 2958_1FFFh */

	unsigned char dmy_29482000_295FFFFF[0x29600000-0x29582000];	/* 2958_2000 - 295F_FFFF */

	// IMG_APB1(PIPE[3])
	unsigned char dmy_29600000_2963FFFF[0x29640000-0x29600000];	/* 2960_0000 - 2963_FFFF */

	struct io_sro_ahb			SRO3_TBL;	/* 2964_0000 - 2967_FFFFh */
	struct io_b2b_ahb			B2B3_TBL;	/* 2968_0000 - 2968_1FFFh */

	unsigned char dmy_29682000_297FFFFF[0x29800000-0x29682000];	/* 2968_2000 - 297F_FFFF */

	// IMG_APB3
	unsigned char dmy_29800000_2980FFFF[0x29810000-0x29800000];	/* 2980_0000 - 2980_FFFF */

	struct io_stat_ahb			STAT_TBL;	/* 2981_0000 - 2981_3FFFh */
	struct io_pas_ahb			PAS_TBL;	/* 2981_4000 - 2981_4FFFh */

};

extern volatile struct io_pro_tbl		IO_PRO_TBL;

#endif	// _IO_PRO_TABLE_H_
