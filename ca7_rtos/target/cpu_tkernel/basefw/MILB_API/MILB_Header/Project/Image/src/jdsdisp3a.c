/**
 * @file		jdsdisp3a.c
 * @brief		Definition JDSDISP3A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "jdsdisp3a.h"

volatile struct io_jdsdisp_top	IO_DISP				__attribute__((section(".DISP")));
volatile struct io_jdsdisp_tbl	IO_DISP_TBL			__attribute__((section(".DISP_TBL")));
