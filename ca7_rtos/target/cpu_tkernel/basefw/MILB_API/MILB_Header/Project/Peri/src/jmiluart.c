/**
 * @file		jmiluart.c
 * @brief		Definition JMILUART Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_jmiluart.h"

volatile union io_usio          IO_USIO     __attribute__((section(".USIO")));
