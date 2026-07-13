/**
 * @file		jdsshdr.c
 * @brief		Definition JDSSHDR Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_jdsshdr.h"

volatile struct io_jdsshdr		IO_SHDR			__attribute__((section(".SHDR")));
volatile struct io_jdsshdr_tbl	IO_SHDR_TBL		__attribute__((section(".SHDR_TBL")));
