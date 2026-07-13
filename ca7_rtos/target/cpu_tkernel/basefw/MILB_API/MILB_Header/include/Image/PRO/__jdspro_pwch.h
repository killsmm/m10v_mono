/**
 * @file		__jdspro_pwch.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_PWCH_H_
#define _IO_PRO_PWCH_H_

/*  structure of PWCHTRG    (2800_C800h)    */
union io_pwchtrg{
    unsigned long       word;
    struct {
        unsigned long   PWCHTRG :2;
        unsigned long           :30;
    }bit;
};

/*  structure of PWS    (2800_C804h)    */
union io_pws{
    unsigned long       word;
    struct {
        unsigned long   PWS :1;
        unsigned long       :31;
    }bit;
};

/*  structure of PWAXCTL    (2800_C808h)    */
union io_pwaxctl{
    unsigned long       word;
    struct {
        unsigned long   AWCACHE :4;
        unsigned long           :4;
        unsigned long   AWPROT  :3;
        unsigned long           :21;
    }bit;
};

/*  structure of PWCHCTL    (2800_C80Ch)    */
union io_pwchctl{
    unsigned long       word;
    struct {
        unsigned long   PWBMD   :2;
        unsigned long           :2;
        unsigned long   PWLV    :3;
        unsigned long           :1;
        unsigned long   PWMW    :1;
        unsigned long           :23;
    }bit;
};

/*  structure of PWCHINTENB (2800_C810h)    */
union io_pwchintenb{
    unsigned long       word;
    struct {
        unsigned long   PWE     :1;
        unsigned long           :3;
        unsigned long   PWEE    :1;
        unsigned long           :3;
        unsigned long   PWXE    :1;
        unsigned long           :23;
    }bit;
};

/*  structure of PWCHINTFLG (2800_C814h)    */
union io_pwchintflg{
    unsigned long       word;
    struct {
        unsigned long   __PWF     :1;
        unsigned long             :3;
        unsigned long   __PWEF    :1;
        unsigned long             :3;
        unsigned long   __PWXF    :1;
        unsigned long             :23;
    }bit;
};

/*  structure of PWCHBRESP  (2800_C818h)    */
union io_pwchbresp{
    unsigned long       word;
    struct {
        unsigned long   PWCHBRESP   :2;
        unsigned long               :30;
    }bit;
};

/*  structure of PWSA   (2800_C81Ch)    */
union io_pwsa{
    unsigned long       word;
    struct {
        unsigned long   PWSA    :32;
    }bit;
};

/*  structure of PWLSIZE    (2800_C820h)    */
union io_pwlsize{
    unsigned long       word;
    struct {
        unsigned long   PWLSIZE :32;
    }bit;
};

/*  structure of PWDW   (2800_C824h)    */
union io_pwdw{
    unsigned long       word;
    struct {
        unsigned long   PWDW    :2;
        unsigned long           :30;
    }bit;
};

/*  structure of PWVFM  (2800_C828h)    */
union io_pwvfm{
    unsigned long       word;
    struct {
        unsigned long   PWVFM   :1;
        unsigned long           :31;
    }bit;
};

/* Define i/o mapping */
struct io_pwch{
    /* JDSIMG */
    union  io_pwchtrg       PWCHTRG;    		/* 2800_(C800 - C803h) */
    union  io_pws           PWS;        		/* 2800_(C804 - C807h) */
    union  io_pwaxctl       PWAXCTL;    		/* 2800_(C808 - C80Bh) */
    union  io_pwchctl       PWCHCTL;    		/* 2800_(C80C - C80Fh) */
    union  io_pwchintenb    PWCHINTENB; 		/* 2800_(C810 - C813h) */
    union  io_pwchintflg    PWCHINTFLG; 		/* 2800_(C814 - C817h) */
    union  io_pwchbresp     PWCHBRESP;  		/* 2800_(C818 - C81Bh) */
    union  io_pwsa          PWSA;       		/* 2800_(C81C - C81Fh) */
    union  io_pwlsize       PWLSIZE;    		/* 2800_(C820 - C823h) */
    union  io_pwdw          PWDW;       		/* 2800_(C824 - C827h) */
    union  io_pwvfm         PWVFM;      		/* 2800_(C828 - C82Bh) */

    unsigned char dmy_C82C_C8FF[0xC900-0xC82C]; /* 2800_(C82C - C8FFh) */
};

#endif	// _IO_PRO_PWCH_H_
