/**
 * @file		relc.c
 * @brief		Definition RELC Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_relc.h"

volatile struct io_relc		IO_RELC		__attribute__((section(".RELC")));
