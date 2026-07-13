/**
 * @file		__jdsdisp_dsi.h
 * @brief		Definition JDSDISP3A Macro MIPI-DSI IP1 register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*  structure of MDSIC0 (2890_D000h)    */
union io_jdsdisp_dsi_mdsic0{
    unsigned long       word;
    struct {
        unsigned long   MDSSD   :1;
        unsigned long           :7;
        unsigned long   MDSCM   :1;
        unsigned long           :23;
    }bit;
};

/*  structure of MDSIC1 (2890_D004h)    */
union io_jdsdisp_dsi_mdsic1{
    unsigned long       word;
    struct {
        unsigned long   MDSUDC  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of MDSSR  (2890_D100h)    */
union io_jdsdisp_dsi_mdssr{
    unsigned long       word;
    struct {
        unsigned long   SR  :1;
        unsigned long       :31;
    }bit;
};

/* Define i/o mapping */
struct io_jdsdisp_dsi{
    /* JDSIMG */
    union  io_jdsdisp_dsi_mdsic0    MDSIC0; /* 2890_(D000 - D003h) */
    union  io_jdsdisp_dsi_mdsic1    MDSIC1; /* 2890_(D004 - D007h) */

    unsigned char dmy_D008_D0FF[0xD100-0xD008]; /* 2890_(D008 - D0FFh) */

    union  io_jdsdisp_dsi_mdssr     MDSSR;  /* 2890_(D100 - D103h) */

    unsigned char dmy_D104_DFFF[0xE000-0xD104]; /* 2890_(D104 - DFFFh) */

};

