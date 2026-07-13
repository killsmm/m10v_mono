/**
 * @file		_jdsdisp_tbl.h
 * @brief		Definition JDSDISP3A AHB TOP Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "__jdsdisp_lch_tbl.h"
#include "__jdsdisp_dcore_tbl.h"

/* Define i/o mapping */

/* MAIN (Common block 0/1) */
struct io_jdsdisp_main_tbl{
    struct io_jdslch_tbl    LCH;                        /* 298E_(0000 - 3FFFh) */
    struct io_jdsdcore_tbl  DCORE;                      /* 298E_(4000 - 7FFFh) */
	
    unsigned char dmy_8000_FFFF[0x10000-0x8000];        /* 298E_(8000 - FFFFh) */
};

/* DISP TBL */
struct io_jdsdisp_tbl{
    /* JDSIMG */
    struct io_jdsdisp_main_tbl  MAIN[2];                /* 298E_0000 - 298F_FFFFh */
};

extern volatile struct io_jdsdisp_tbl	IO_DISP_TBL;

