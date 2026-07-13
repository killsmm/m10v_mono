/**
 * @file		pmc.c
 * @brief		Definition PMC Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include "_jctpmc.h"

volatile struct io_pmc		IO_PMC			__attribute__((section(".PMC")));

