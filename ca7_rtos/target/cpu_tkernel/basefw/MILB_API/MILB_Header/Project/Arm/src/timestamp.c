/**
 * @file		timestamp.c
 * @brief		Definition Timestamp Generator I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_timestamp.h"

volatile struct io_timestamp			IO_TIMESTAMP		__attribute__((section(".TIMESTAMP")));
