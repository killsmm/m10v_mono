/**
 * @file		audio_if.c
 * @brief		Definition Audio I/F Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "_audio_if.h"

volatile struct io_audio IO_AUDIO __attribute__((section(".AUDIO")));
