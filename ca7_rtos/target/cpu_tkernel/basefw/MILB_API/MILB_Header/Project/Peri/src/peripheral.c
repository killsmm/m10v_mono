/**
 * @file		peripheral.c
 * @brief		Definition Peripheral Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_peripheral.h"

volatile struct io_peri     IO_PERI         __attribute__((section(".PERI")));
volatile struct io_pwmdma   IO_PWMDMA[4]    __attribute__((section(".PWMDMA")));

