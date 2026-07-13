/**
 * @file		chiptop.c
 * @brief		Definition Chip-Top I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_chiptop.h"

volatile struct io_chiptop	IO_CHIPTOP	__attribute__((section(".CHIPTOP")));

