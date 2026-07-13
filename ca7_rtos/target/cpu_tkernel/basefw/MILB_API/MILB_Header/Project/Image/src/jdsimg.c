/**
 * @file		jdsimg.c
 * @brief		Definition JDSIMG Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_jdsimg.h"

volatile struct io_img		IO_IMG		__attribute__((section(".IMG")));
