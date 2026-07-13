/**
 * @file		jdsme.c
 * @brief		Definition JDSME Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include "_jdsme.h"

volatile struct io_jdsme		IO_ME			__attribute__((section(".ME")));
