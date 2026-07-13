/**
 * @file		jdsxch.c
 * @brief		Definition JDSXH2D Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include "_jdsxh2d.h"

volatile struct io_jdsxch IO_XCH __attribute__((section(".Xch")));
