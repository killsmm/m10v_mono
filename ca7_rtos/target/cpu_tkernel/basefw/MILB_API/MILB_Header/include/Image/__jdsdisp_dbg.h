/**
 * @file		__jdsdisp_dbg.h
 * @brief		Definition JDSDISP3A Macro Debug Control register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*  structure of DBGSL  (2890_B000h)    */
union io_jdsddbg_reg_dbgsl{
    unsigned long       word;
    struct {
        unsigned long   VDMSL   :1;
        unsigned long           :3;
        unsigned long   HDMSL   :1;
        unsigned long           :3;
        unsigned long   HDEMSL  :1;
        unsigned long           :23;
    }bit;
};

/* Define i/o mapping */
struct io_jdsddbg_reg{
    /* JDSIMG */
    union  io_jdsddbg_reg_dbgsl DBGSL;  /* 2890_(B000 - B003h) */

    unsigned char dmy_B004_BFFF[0xC000-0xB004]; /* 2890_(B004 - BFFFh) */

};

