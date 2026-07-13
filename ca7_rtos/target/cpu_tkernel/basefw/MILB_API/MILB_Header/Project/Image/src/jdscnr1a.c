/**
 * @file		jdscnr1a.c
 * @brief		Definition JDSCNR1A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "_jdscnr1a.h"

volatile struct io_cnr_reg		IO_CNR1		__attribute__((section(".CNR_1")));
volatile struct io_cnr_reg		IO_CNR2		__attribute__((section(".CNR_2")));
volatile struct io_cnr_reg		IO_CNR3		__attribute__((section(".CNR_3")));

