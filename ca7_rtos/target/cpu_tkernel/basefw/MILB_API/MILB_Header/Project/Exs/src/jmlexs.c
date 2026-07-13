/**
 * @file		jmlexs.c
 * @brief		Definition JMLEXS Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_jmlexs.h"

volatile struct io_gpv    IO_GPV	__attribute__((section(".GPV")));
volatile struct io_exstop IO_EXSTOP	__attribute__((section(".EXSTOP")));
