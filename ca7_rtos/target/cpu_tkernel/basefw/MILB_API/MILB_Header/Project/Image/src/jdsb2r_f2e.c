/**
 * @file		jdsb2r_f2e.c
 * @brief		Definition JDSB2R_F2E Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include "jdsb2r.h"

volatile struct io_b2r		IO_B2R_P1		__attribute__((section(".B2R_P1")));
volatile struct io_b2r		IO_B2R_P2		__attribute__((section(".B2R_P2")));
volatile struct io_b2r		IO_B2R_P3		__attribute__((section(".B2R_P3")));
volatile struct io_b2r_sram	IO_B2R_TBL_P1	__attribute__((section(".B2R_TBL_P1")));
volatile struct io_b2r_sram	IO_B2R_TBL_P2	__attribute__((section(".B2R_TBL_P2")));
volatile struct io_b2r_sram	IO_B2R_TBL_P3	__attribute__((section(".B2R_TBL_P3")));
