/**
 * @file		pdm.c
 * @brief		Definition PDM Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "_pdm.h"

volatile struct io_pdm		IO_PDM[2]	__attribute__((section(".PDM")));

