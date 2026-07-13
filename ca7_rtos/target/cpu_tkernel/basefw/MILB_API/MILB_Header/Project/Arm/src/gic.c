/**
 * @file		gic.c
 * @brief		Definition GIC (Generic Interrupt Controller) I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_gic.h"

volatile struct io_gic_dist		IO_GIC_DIST		__attribute__((section(".GIC_DIST")));
volatile struct io_gic_cpu		IO_GIC_CPU		__attribute__((section(".GIC_CPU")));
