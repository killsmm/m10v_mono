/**
 * @file		exiu.c
 * @brief		Definition EXIU (External Interrupt Unit) I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
 
#include "_exiu.h"

volatile struct io_exiu			IO_EXIU		__attribute__((section(".EXIU")));
