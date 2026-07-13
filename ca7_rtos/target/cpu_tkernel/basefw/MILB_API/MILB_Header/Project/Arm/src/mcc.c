/**
 * @file		mcc.c
 * @brief		Definition MCC (Multi-Core Controller) I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_mcc.h"

volatile struct io_mcc			IO_MCC			__attribute__((section(".MCC")));
