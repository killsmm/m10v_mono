/**
 * @file		jdspro.c
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_jdspro.h"

volatile struct io_pro		IO_PRO		__attribute__((section(".PRO")));
volatile struct io_pro_tbl	IO_PRO_TBL	__attribute__((section(".PRO_TBL")));
