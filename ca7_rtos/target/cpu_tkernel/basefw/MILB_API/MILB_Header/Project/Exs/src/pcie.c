/**
 * @file		pcie.c
 * @brief		Definition JMLEXS_PCIE Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_pcie.h"

volatile struct io_pcie		IO_PCIE0		__attribute__((section(".PCIE0")));
volatile struct io_pcie		IO_PCIE1		__attribute__((section(".PCIE1")));
volatile union io_pcie_core	IO_PCIE0_CORE	__attribute__((section(".PCIE0_CORE")));
volatile union io_pcie_core	IO_PCIE1_CORE	__attribute__((section(".PCIE1_CORE")));
