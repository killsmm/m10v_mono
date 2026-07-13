/**
 * @file		__jdsiip4b_pixfmttbl.h
 * @brief		Definition JDSIIP4B Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/*  structure of PIXFMTTBL_0    (2888_0300h)    */
union io_iip_ipxfmttbl_0{
    unsigned long       word;
    struct {
        unsigned long   P           :1;
        unsigned long               :3;
        unsigned long   FTYPE       :3;
        unsigned long               :1;
        unsigned long   PFMT        :4;
        unsigned long   ALPHA       :1;
        unsigned long               :3;
        unsigned long   PDEPTH      :3;
        unsigned long               :1;
        unsigned long   ADEPTH      :3;
        unsigned long               :1;
        unsigned long   CHKYORDR    :8;
    }bit;
};

/*  structure of PIXFMTTBL_1    (2888_0304h)    */
union io_iip_ipxfmttbl_1{
    unsigned long       word;
    struct {
        unsigned long   SGYG    :1;
        unsigned long   SGB     :1;
        unsigned long   SGR     :1;
        unsigned long   SGA     :1;
        unsigned long   ASUBSPL :2;
        unsigned long           :2;
        unsigned long   MTIFYG  :2;
        unsigned long   MTIFB   :2;
        unsigned long   MTIFR   :2;
        unsigned long   MTIFA   :2;
        unsigned long   OARBHV  :1;
        unsigned long           :15;
    }bit;
};

/*  structure of PIXFMTTBL_2    (2888_0308h)    */
union io_iip_ipxfmttbl_2{
    unsigned long       word;
    struct {
        unsigned long   OARVALYG    :16;
        unsigned long   OARVALB     :16;
    }bit;
};

/*  structure of PIXFMTTBL_3    (2888_030Ch)    */
union io_iip_ipxfmttbl_3{
    unsigned long       word;
    struct {
        unsigned long   OARVALR :16;
        unsigned long   OARVALA :16;
    }bit;
};

/*  structure of PIXFMTTBL_4    (2888_0310h)    */
union io_iip_ipxfmttbl_4{
    unsigned long       word;
    struct {
        long            GHSZYG  :18;
        unsigned long           :14;
    }bit;
};

/*  structure of PIXFMTTBL_5    (2888_0314h)    */
union io_iip_ipxfmttbl_5{
    unsigned long       word;
    struct {
        long            GHSZB   :18;
        unsigned long           :14;
    }bit;
};

/*  structure of PIXFMTTBL_6    (2888_0318h)    */
union io_iip_ipxfmttbl_6{
    unsigned long       word;
    struct {
        long            GHSZR   :18;
        unsigned long           :14;
    }bit;
};

/*  structure of PIXFMTTBL_7    (2888_031Ch)    */
union io_iip_ipxfmttbl_7{
    unsigned long       word;
    struct {
        long            GHSZA   :18;
        unsigned long           :14;
    }bit;
};

/*  structure of PIXFMTTBL_8    (2888_0320h)    */
union io_iip_ipxfmttbl_8{
    unsigned long       word;
    struct {
        unsigned long   PICHSZ  :15;
        unsigned long           :1;
        unsigned long   PICVSZ  :14;
        unsigned long           :2;
    }bit;
};

/*  structure of PIXFMTTBL_9    (2888_0324h)    */
union io_iip_ipxfmttbl_9{
    unsigned long       word;
    struct {
        unsigned long   ADRSYG  :32;
    }bit;
};

/*  structure of PIXFMTTBL_10   (2888_0328h)    */
union io_iip_ipxfmttbl_10{
    unsigned long       word;
    struct {
        unsigned long   ADRSB   :32;
    }bit;
};

/*  structure of PIXFMTTBL_11   (2888_032Ch)    */
union io_iip_ipxfmttbl_11{
    unsigned long       word;
    struct {
        unsigned long   ADRSR   :32;
    }bit;
};

/*  structure of PIXFMTTBL_12   (2888_0330h)    */
union io_iip_ipxfmttbl_12{
    unsigned long       word;
    struct {
        unsigned long   ADRSA   :32;
    }bit;
};

/* Define i/o mapping */
struct io_iip_pixfmttbl{
    /* JDSIMG */
    union  io_iip_ipxfmttbl_0   PIXFMTTBL_0;    /* 2888_(0300 - 0303h) */
    union  io_iip_ipxfmttbl_1   PIXFMTTBL_1;    /* 2888_(0304 - 0307h) */
    union  io_iip_ipxfmttbl_2   PIXFMTTBL_2;    /* 2888_(0308 - 030Bh) */
    union  io_iip_ipxfmttbl_3   PIXFMTTBL_3;    /* 2888_(030C - 030Fh) */
    union  io_iip_ipxfmttbl_4   PIXFMTTBL_4;    /* 2888_(0310 - 0313h) */
    union  io_iip_ipxfmttbl_5   PIXFMTTBL_5;    /* 2888_(0314 - 0317h) */
    union  io_iip_ipxfmttbl_6   PIXFMTTBL_6;    /* 2888_(0318 - 031Bh) */
    union  io_iip_ipxfmttbl_7   PIXFMTTBL_7;    /* 2888_(031C - 031Fh) */
    union  io_iip_ipxfmttbl_8   PIXFMTTBL_8;    /* 2888_(0320 - 0323h) */
    union  io_iip_ipxfmttbl_9   PIXFMTTBL_9;    /* 2888_(0324 - 0327h) */
    union  io_iip_ipxfmttbl_10  PIXFMTTBL_10;   /* 2888_(0328 - 032Bh) */
    union  io_iip_ipxfmttbl_11  PIXFMTTBL_11;   /* 2888_(032C - 032Fh) */
    union  io_iip_ipxfmttbl_12  PIXFMTTBL_12;   /* 2888_(0330 - 0333h) */

    unsigned char dmy_0334_033F[0x0340-0x0334]; /* 2888_(0334 - 033Fh) */

};

