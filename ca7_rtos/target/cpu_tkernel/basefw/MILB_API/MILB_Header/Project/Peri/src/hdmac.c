/**
 * @file  hdmac.c
 * @brief  Definition HDMAC Macro I/O register
 * @note  None
 * @attention None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "_hdmac.h"

volatile struct io_hdmac    IO_HDMAC    __attribute__((section(".HDMAC")));		/* External Storage */
volatile struct io_hdmac    IO_HDMAC0   __attribute__((section(".HDMAC0")));	/* Peripheral */
volatile struct io_hdmac    IO_HDMAC1   __attribute__((section(".HDMAC1")));	/* Peripheral */
