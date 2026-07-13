/**
 * @file  xdmac.c
 * @brief  Definition XDMAC Macro I/O register
 * @note  None
 * @attention None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_xdmac.h"

volatile struct io_xdmac		IO_XDMAC		__attribute__((section(".XDMAC")));
volatile struct io_xdmac		IO_XDMACS		__attribute__((section(".XDMACS")));
