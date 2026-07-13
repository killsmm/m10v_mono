/**
 * @file		sdramc.c
 * @brief		Definition SDRAMC Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_sdramc.h"

volatile struct io_sdramc	IO_SDRAMC[2]	__attribute__((section(".UMC")));
