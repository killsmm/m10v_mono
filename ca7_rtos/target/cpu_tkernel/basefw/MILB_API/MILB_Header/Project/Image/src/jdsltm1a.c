/**
 * @file		jdsltm1a.c
 * @brief		Definition JDSLTM1A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */
#include "jdsltm.h"

volatile struct io_ltm_top		IO_LTM_P1		__attribute__((section(".LTM_P1")));;
volatile struct io_ltm_top		IO_LTM_P2		__attribute__((section(".LTM_P2")));;
volatile struct io_ltm_top		IO_LTM_P3		__attribute__((section(".LTM_P3")));;
volatile struct io_ltm_sram		IO_LTM_TBL_P1	__attribute__((section(".LTM_TBL_P1")));
volatile struct io_ltm_sram		IO_LTM_TBL_P2	__attribute__((section(".LTM_TBL_P2")));
volatile struct io_ltm_sram		IO_LTM_TBL_P3	__attribute__((section(".LTM_TBL_P3")));
