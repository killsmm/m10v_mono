/**
 * @file		jdsr2y_f2e.c
 * @brief		Definition JDSR2Y_F2E Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include "jdsr2y.h"

volatile struct io_r2y		IO_R2Y_P1		__attribute__((section(".R2Y_P1")));
volatile struct io_r2y		IO_R2Y_P2		__attribute__((section(".R2Y_P2")));
volatile struct io_r2y		IO_R2Y_P3		__attribute__((section(".R2Y_P3")));
volatile struct io_r2y_sram	IO_R2Y_TBL_P1	__attribute__((section(".R2Y_TBL_P1")));
volatile struct io_r2y_sram	IO_R2Y_TBL_P2	__attribute__((section(".R2Y_TBL_P2")));
volatile struct io_r2y_sram	IO_R2Y_TBL_P3	__attribute__((section(".R2Y_TBL_P3")));
