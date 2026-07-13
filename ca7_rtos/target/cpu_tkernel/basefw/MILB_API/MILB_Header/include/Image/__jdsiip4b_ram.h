/**
 * @file		__jdsiip4b_ram.h
 * @brief		Definition JDSIIP4B Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "__jdsiip4b_ram_lut.h"
#include "__jdsiip4b_ram_histgram.h"

/* Define i/o mapping */
struct io_iip_ram{
    /* JDSIMG */
    struct io_iip_ram_lut               LUTRAM;      /* 2982_(0000 - FFFFh) */
    struct io_iip_ram_histgram          HISTGRAM;    /* 2983_(0000 - FFFFh) */
};

extern volatile struct io_iip_ram	IO_IIP_TBL;

