/**
 * @file  jmlbmh2a.c
 * @brief  Definition JMLBMH2A Macro I/O register
 * @note  None
 * @attention None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "_jmlbmh2a.h"

volatile struct io_bmh		IO_BMH[2]		__attribute__((section(".BMH")));
