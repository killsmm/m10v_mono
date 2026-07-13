/**
 * @file		jdsiip4b.c
 * @brief		Definition JDSIIP4B TBL Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include "jdsiip.h"

volatile struct io_iip		IO_IIP		__attribute__((section(".IIP")));
volatile struct io_iip_ram	IO_IIP_TBL	__attribute__((section(".IIP_TBL")));
