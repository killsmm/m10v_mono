/**
 * @file		nic.c
 * @brief		Definition Network InterConnect I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "_nic.h"

volatile struct io_addrctrl		IO_NIC_ADDRCTRL		__attribute__((section(".NIC_ADDRCTRL")));
volatile struct io_asib			IO_NIC_ASIB[7]		__attribute__((section(".NIC_ASIB")));
