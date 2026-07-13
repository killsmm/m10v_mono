/**
 * @file		__jdspro_ldiv.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_LDIV_H_
#define _IO_PRO_LDIV_H_

/*  structure of LDIVTRG    (2800_A800h)    */
union io_ldivtrg{
    unsigned long       word;
    struct {
        unsigned long   LDIVTRG :2;
        unsigned long           :30;
    }bit;
};

/*  structure of LDIVPAEN   (2800_A804h)    */
union io_ldivpaen{
    unsigned long       word;
    struct {
        unsigned long   LDPAEN  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of LDIVCTL    (2800_A808h)    */
union io_ldivctl{
    unsigned long       word;
    struct {
        unsigned long   LDSTMD  :1;
        unsigned long           :3;
        unsigned long   LDIPMD  :2;
        unsigned long           :2;
        unsigned long   LDVDMD  :2;
        unsigned long           :2;
        unsigned long   LDHDMD  :1;
        unsigned long           :3;
        unsigned long   LDMCH   :2;
        unsigned long           :2;
        unsigned long   LDDIVMD :3;
        unsigned long           :1;
        unsigned long   LDOPMD  :1;
        unsigned long           :7;
    }bit;
};

/*  structure of LDIVIHW    (2800_A80Ch)    */
union io_ldivihw{
    unsigned long       word;
    struct {
        unsigned long   LDIVIHW :14;
        unsigned long           :18;
    }bit;
};

/*  structure of LDIVOH (2800_A810h)    */
union io_oh_1{
    unsigned long       word;
    struct {
        unsigned long   LDIVOH_0    :14;
        unsigned long               :2;
        unsigned long   LDIVOH_1    :14;
        unsigned long               :2;
    }bit;
};
union io_oh_2{
    unsigned long       word;
    struct {
        unsigned long   LDIVOH_2    :14;
        unsigned long               :2;
        unsigned long   LDIVOH_3    :14;
        unsigned long               :2;
    }bit;
};
struct io_ldivoh{
    union io_oh_1   LDIVOH1;
    union io_oh_2   LDIVOH2;
};

/*  structure of LDIVOHW    (2800_A818h)    */
union io_ohw_1{
    unsigned long       word;
    struct {
        unsigned long   LDIVOHW_0   :14;
        unsigned long               :2;
        unsigned long   LDIVOHW_1   :14;
        unsigned long               :2;
    }bit;
};
union io_ohw_2{
    unsigned long       word;
    struct {
        unsigned long   LDIVOHW_2   :14;
        unsigned long               :2;
        unsigned long   LDIVOHW_3   :14;
        unsigned long               :2;
    }bit;
};
struct io_ldivohw{
    union io_ohw_1  LDIVOHW1;
    union io_ohw_2  LDIVOHW2;
};

/*  structure of LDIVINTENB (2800_A824h)    */
union io_ldivintenb{
    unsigned long       word;
    struct {
        unsigned long   LDIVEE  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of LDIVINTFLG (2800_A828h)    */
union io_ldivintflg{
    unsigned long       word;
    struct {
        unsigned long   __LDIVEF  :1;
        unsigned long             :31;
    }bit;
};

/* Define i/o mapping */
struct io_ldiv{
    /* JDSIMG */
    union  io_ldivtrg     LDIVTRG;              /* 2800_(A800 - A803h) */
    union  io_ldivpaen    LDIVPAEN;             /* 2800_(A804 - A807h) */
    union  io_ldivctl     LDIVCTL;              /* 2800_(A808 - A80Bh) */
    union  io_ldivihw     LDIVIHW;              /* 2800_(A80C - A80Fh) */
    struct io_ldivoh      LDIVOH;               /* 2800_(A810 - A817h) */
    struct io_ldivohw     LDIVOHW;              /* 2800_(A818 - A81Fh) */

    unsigned char dmy_A820_A823[0xA824-0xA820]; /* 2800_(A820 - A823h) */

    union  io_ldivintenb  LDIVINTENB;           /* 2800_(A824 - A827h) */
    union  io_ldivintflg  LDIVINTFLG;           /* 2800_(A828 - A82Bh) */

    unsigned char dmy_A82C_A8FF[0xA900-0xA82C]; /* 2800_(A82C - A8FFh) */
};

#endif	// _IO_PRO_LDIV_H_
