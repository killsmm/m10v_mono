/**
 * @file		spi.c
 * @brief		Definition SPI Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "_spi.h"

volatile struct io_spi		IO_SPI[3]	__attribute__((section(".SPI")));

