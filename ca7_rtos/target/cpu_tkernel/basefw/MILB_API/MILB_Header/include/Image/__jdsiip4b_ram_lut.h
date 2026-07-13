/**
 * @file		__jdsiip4b_ram_lut.h
 * @brief		Definition JDSIIP4B Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/*  structure of LUTRAM (2982_0000h)    */
union io_iip_lutram{
    unsigned long       word[6][1024];
    struct {
        unsigned long   LUT :14;
        unsigned long       :18;
    }bit[6][1024];
};

/* Define i/o mapping */
struct io_iip_ram_lut{
    /* JDSIMG */
    union  io_iip_lutram  LUTRAM; /* 2982_(0000 - 5FFFh) */

    unsigned char dmy_29826000_2982FFFF[0x29830000-0x29826000]; /* 2982_6000 - 2982_FFFFh */

};

