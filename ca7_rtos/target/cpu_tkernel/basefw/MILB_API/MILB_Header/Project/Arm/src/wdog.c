/**
 * @file		wdog.c
 * @brief		Definition Watchdog Timer I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_wdog.h"

volatile struct io_wdog			IO_WDOG			__attribute__((section(".WDOG")));
