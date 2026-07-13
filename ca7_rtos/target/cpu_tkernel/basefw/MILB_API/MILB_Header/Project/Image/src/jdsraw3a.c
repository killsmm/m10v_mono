/**
 * @file		jdsraw3a.c
 * @brief		Definition JDSRAW3A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "_jdsraw3a.h"

volatile struct io_jdsraw IO_RAW __attribute__((section(".RAW")));
