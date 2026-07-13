/**
 * @file		jdsfpt.c
 * @brief		Definition JDSFPT Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include "_jdsfpt.h"
#include "_jdsfpt_ahb.h"

volatile struct io_jdsfpt IO_FPT[2] __attribute__((section(".FPT")));
volatile struct io_jdsfpt_ahb IO_FPT_AHB[2] __attribute__((section(".FPT_AHB")));

