/**
 * @file		jdsmxic.c
 * @brief		Definition JDSMXIC Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_jdsmxic.h"

volatile struct io_jdsmxic		IO_MXIC0		__attribute__((section(".MXIC0")));
volatile struct io_jdsmxic		IO_MXIC1		__attribute__((section(".MXIC1")));
volatile struct io_jdsmxic		IO_MXIC2		__attribute__((section(".MXIC2")));
volatile struct io_jdsmxic		IO_MXIC3		__attribute__((section(".MXIC3")));
volatile struct io_jdsmxic		IO_MXIC4		__attribute__((section(".MXIC4")));
volatile struct io_jdsmxic		IO_MXIC5		__attribute__((section(".MXIC5")));
volatile struct io_jdsmxic		IO_MXIC6		__attribute__((section(".MXIC6")));
volatile struct io_jdsmxic_tbl	IO_MXIC1_TBL	__attribute__((section(".MXIC1_TBL")));
volatile struct io_jdsmxic_tbl	IO_MXIC2_TBL	__attribute__((section(".MXIC2_TBL")));
volatile struct io_jdsmxic_tbl	IO_MXIC3_TBL	__attribute__((section(".MXIC3_TBL")));
