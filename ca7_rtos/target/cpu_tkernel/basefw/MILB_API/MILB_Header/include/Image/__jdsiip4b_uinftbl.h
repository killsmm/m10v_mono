/**
 * @file		__jdsiip4b_uinftbl.h
 * @brief		Definition JDSIIP4B Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/*  structure of UNITINFTBL0    (2888_0940h)    */
union io_iip_uinftbl_0{
    unsigned long       word;
    struct {
        unsigned long   SWTRG   :2;
        unsigned long           :2;
        unsigned long   HWEN    :1;
        unsigned long           :3;
        unsigned long   CHAIN   :1;
        unsigned long           :23;
    }bit;
};

/*  structure of UNITINFTBL1    (2888_0948h)    */
union io_iip_uinftbl_1{
    unsigned long      word[2];
    struct {
        unsigned long  PLDUNIT_LO :32;
        unsigned long  PLDUNIT_HI :32;
    }bit;
};

/*  structure of UNITINFTBL2    (2888_0950h)    */
union io_iip_uinftbl_2{
    unsigned long       word;
    struct {
        unsigned long   PADRS   :32;
    }bit;
};

/*  structure of UNITSR (2888_0954h)    */
union io_iip_uinftbl_unitsr{
    unsigned long       word;
    struct {
        unsigned long   UNITSR  :1;
        unsigned long           :31;
    }bit;
};

/* Define i/o mapping */
struct io_iip_uinftbl{
    /* JDSIMG */
    union  io_iip_uinftbl_0           UNITINFTBL0;    /* 2888_(0940 - 0943h) */

    unsigned char dmy_0944_0947[0x0948-0x0944]; /* 2888_(0944 - 0947h) */

    union  io_iip_uinftbl_1           UNITINFTBL1;    /* 2888_(0948 - 094Fh) */
    union  io_iip_uinftbl_2           UNITINFTBL2;    /* 2888_(0950 - 0953h) */
    union  io_iip_uinftbl_unitsr      UNITSR;         /* 2888_(0954 - 0957h) */

    unsigned char dmy_0958_095F[0x0960-0x0958]; /* 2888_(0958 - 095Fh) */

};

