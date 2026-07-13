/**
 * @file		slimbus.c
 * @brief		Definition SLIMbus Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_slimbus.h"

volatile struct io_slimbus  IO_SLIMBUS0     __attribute__((section(".SLIMBUS0")));
volatile struct io_slimbus  IO_SLIMBUS1     __attribute__((section(".SLIMBUS1")));
volatile struct io_slimbus  IO_SLIMBUS2     __attribute__((section(".SLIMBUS2")));
volatile struct io_slimbus  IO_SLIMBUS3     __attribute__((section(".SLIMBUS3")));

