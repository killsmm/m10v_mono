/**
 * @file		__jdsiip4b_ram_histgram.h
 * @brief		Definition JDSIIP4B Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/*  structure of HISTGRAM   (2983_0000h)    */
union io_iip_histgram{
    unsigned long       word[4][256];
    struct {
        unsigned long   HIST    :24;
        unsigned long           :8;
    }bit[4][256];
};

/* Define i/o mapping */
struct io_iip_ram_histgram{
    /* JDSIMG */
    union  io_iip_histgram   HISTGRAM;   /* 2983_(0000 - 0FFFh) */

    unsigned char dmy_29831000_2983FFFF[0x29840000-0x29831000]; /* 2983_1000 - 2983_FFFFh */

};

