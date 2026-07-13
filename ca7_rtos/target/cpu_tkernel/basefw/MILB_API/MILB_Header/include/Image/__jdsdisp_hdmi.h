/**
 * @file		__jdsdisp_hdmi.h
 * @brief		Definition JDSDISP3A Macro HDMI IP Control register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*  structure of HDMISR (2890_E000h)    */
union io_jddisp_hdmi_hdmisr{
    unsigned long       word;
    struct {
        unsigned long   SR  :1;
        unsigned long       :31;
    }bit;
};

/* Define i/o mapping */
struct io_jddisp_hdmi{
    /* JDSIMG */
    union  io_jddisp_hdmi_hdmisr    HDMISR; /* 2890_(E000 - E003h) */

    unsigned char dmy_E004_EFFF[0xF000-0xE004]; /* 2890_(E004 - EFFFh) */

};

