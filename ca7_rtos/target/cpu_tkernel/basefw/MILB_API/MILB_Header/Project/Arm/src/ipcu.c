/**
 * @file		ipcu.c
 * @brief		Definition IPCU (Inter-Processor Communications Unit) I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_ipcu.h"

volatile struct io_ipcu			IO_IPCU0			__attribute__((section(".IPCU0")));
volatile struct io_ipcu			IO_IPCU1			__attribute__((section(".IPCU1")));
