/**
 * @file		__jdspro_sdc.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_SDC_H_
#define _IO_PRO_SDC_H_

/*  structure of SDCTRG (2840_8800h)    */
union io_sdctrg{
    unsigned long       word;
    struct {
        unsigned long   SDCTRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of SDCPAEN    (2840_8804h)    */
union io_sdcpaen{
    unsigned long       word;
    struct {
        unsigned long   SDCPAEN :1;
        unsigned long           :31;
    }bit;
};

/*  structure of SDCCTL (2840_8808h)    */
union io_sdcctl{
    unsigned long       word;
    struct {
        unsigned long               :1;
        unsigned long               :3;
        unsigned long   SDCMD       :3;
        unsigned long               :1;
        unsigned long   SDCPTO      :1;
        unsigned long               :3;
        unsigned long   SDCMONO     :1;
        unsigned long               :3;
        unsigned long   SDCPDAFEN   :1;
        unsigned long               :15;
    }bit;
};

/*  structure of SDCINTENB  (2840_880Ch)    */
union io_sdcintenb{
    unsigned long       word;
    struct {
        unsigned long   SDCE    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of SDCINTFLG  (2840_8810h)    */
union io_sdcintflg{
    unsigned long       word;
    struct {
        unsigned long   __SDCF  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of SDCSGD (2840_8814h)    */
union io_sdcsgd{
    unsigned long       word;
    struct {
        unsigned long   SDCSGD  :1;
        unsigned long           :31;
    }bit;
};

/*  structure of SDCPH0 (2840_8880h)    */
union io_sdcph0_1{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH0_0    :64;
    }bit;
};
union io_sdcph0_2{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH0_1    :64;
    }bit;
};
union io_sdcph0_3{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH0_2    :64;
    }bit;
};
union io_sdcph0_4{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH0_3    :64;
    }bit;
};
union io_sdcph0_5{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH0_4    :64;
    }bit;
};
union io_sdcph0_6{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH0_5    :64;
    }bit;
};
union io_sdcph0_7{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH0_6    :64;
    }bit;
};
union io_sdcph0_8{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH0_7    :64;
    }bit;
};
union io_sdcph0_9{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH0_8    :64;
    }bit;
};
struct io_sdcph0{
    union io_sdcph0_1    SDCPH01;
    union io_sdcph0_2    SDCPH02;
    union io_sdcph0_3    SDCPH03;
    union io_sdcph0_4    SDCPH04;
    union io_sdcph0_5    SDCPH05;
    union io_sdcph0_6    SDCPH06;
    union io_sdcph0_7    SDCPH07;
    union io_sdcph0_8    SDCPH08;
    union io_sdcph0_9    SDCPH09;
};

/*  structure of SDCPH1 (2840_8900h)    */
union io_sdcph1_1{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH1_0    :64;
    }bit;
};
union io_sdcph1_2{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH1_1    :64;
    }bit;
};
union io_sdcph1_3{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH1_2    :64;
    }bit;
};
union io_sdcph1_4{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH1_3    :64;
    }bit;
};
union io_sdcph1_5{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH1_4    :64;
    }bit;
};
union io_sdcph1_6{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH1_5    :64;
    }bit;
};
union io_sdcph1_7{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH1_6    :64;
    }bit;
};
union io_sdcph1_8{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH1_7    :64;
    }bit;
};
union io_sdcph1_9{
    unsigned long long      dword;
    struct {
        unsigned long long  SDCPH1_8    :64;
    }bit;
};
struct io_sdcph1{
    union io_sdcph1_1    SDCPH11;
    union io_sdcph1_2    SDCPH12;
    union io_sdcph1_3    SDCPH13;
    union io_sdcph1_4    SDCPH14;
    union io_sdcph1_5    SDCPH15;
    union io_sdcph1_6    SDCPH16;
    union io_sdcph1_7    SDCPH17;
    union io_sdcph1_8    SDCPH18;
    union io_sdcph1_9    SDCPH19;
};

/*  structure of SDCPHW0    (2840_8980h)    */
union io_sdcphw0_1{
    unsigned long       word;
    struct {
        unsigned long   SDCPHW0_0   :7;
        unsigned long               :1;
        unsigned long   SDCPHW0_1   :7;
        unsigned long               :1;
        unsigned long   SDCPHW0_2   :7;
        unsigned long               :1;
        unsigned long   SDCPHW0_3   :7;
        unsigned long               :1;
    }bit;
};
union io_sdcphw0_2{
    unsigned long       word;
    struct {
        unsigned long   SDCPHW0_4   :7;
        unsigned long               :1;
        unsigned long   SDCPHW0_5   :7;
        unsigned long               :1;
        unsigned long   SDCPHW0_6   :7;
        unsigned long               :1;
        unsigned long   SDCPHW0_7   :7;
        unsigned long               :1;
    }bit;
};
union io_sdcphw0_3{
    unsigned long       word;
    struct {
        unsigned long   SDCPHW0_8   :7;
        unsigned long               :1;
        unsigned long               :24;
    }bit;
};
struct io_sdcphw0{
    union io_sdcphw0_1   SDCPHW01;
    union io_sdcphw0_2   SDCPHW02;
    union io_sdcphw0_3   SDCPHW03;
};

/*  structure of SDCPHW1    (2840_8990h)    */
union io_sdcphw1_1{
    unsigned long       word;
    struct {
        unsigned long   SDCPHW1_0   :7;
        unsigned long               :1;
        unsigned long   SDCPHW1_1   :7;
        unsigned long               :1;
        unsigned long   SDCPHW1_2   :7;
        unsigned long               :1;
        unsigned long   SDCPHW1_3   :7;
        unsigned long               :1;
    }bit;
};
union io_sdcphw1_2{
    unsigned long       word;
    struct {
        unsigned long   SDCPHW1_4   :7;
        unsigned long               :1;
        unsigned long   SDCPHW1_5   :7;
        unsigned long               :1;
        unsigned long   SDCPHW1_6   :7;
        unsigned long               :1;
        unsigned long   SDCPHW1_7   :7;
        unsigned long               :1;
    }bit;
};
union io_sdcphw1_3{
    unsigned long       word;
    struct {
        unsigned long   SDCPHW1_8   :7;
        unsigned long               :1;
        unsigned long               :24;
    }bit;
};
struct io_sdcphw1{
    union io_sdcphw1_1   SDCPHW11;
    union io_sdcphw1_2   SDCPHW12;
    union io_sdcphw1_3   SDCPHW13;
};

/*  structure of SDCP1HW0   (2840_89A0h)    */
union io_sdcp1hw0_1{
    unsigned long       word;
    struct {
        unsigned long   SDCP1HW0_0  :13;
        unsigned long               :3;
        unsigned long   SDCP1HW0_1  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp1hw0_2{
    unsigned long       word;
    struct {
        unsigned long   SDCP1HW0_2  :13;
        unsigned long               :3;
        unsigned long   SDCP1HW0_3  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp1hw0_3{
    unsigned long       word;
    struct {
        unsigned long   SDCP1HW0_4  :13;
        unsigned long               :3;
        unsigned long   SDCP1HW0_5  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp1hw0_4{
    unsigned long       word;
    struct {
        unsigned long   SDCP1HW0_6  :13;
        unsigned long               :3;
        unsigned long   SDCP1HW0_7  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp1hw0_5{
    unsigned long       word;
    struct {
        unsigned long   SDCP1HW0_8  :13;
        unsigned long               :3;
        unsigned long               :16;
    }bit;
};
struct io_sdcp1hw0{
    union io_sdcp1hw0_1  SDCP1HW01;
    union io_sdcp1hw0_2  SDCP1HW02;
    union io_sdcp1hw0_3  SDCP1HW03;
    union io_sdcp1hw0_4  SDCP1HW04;
    union io_sdcp1hw0_5  SDCP1HW05;
};

/*  structure of SDCP1HW1   (2840_89C0h)    */
union io_sdcp1hw1_1{
    unsigned long       word;
    struct {
        unsigned long   SDCP1HW1_0  :13;
        unsigned long               :3;
        unsigned long   SDCP1HW1_1  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp1hw1_2{
    unsigned long       word;
    struct {
        unsigned long   SDCP1HW1_2  :13;
        unsigned long               :3;
        unsigned long   SDCP1HW1_3  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp1hw1_3{
    unsigned long       word;
    struct {
        unsigned long   SDCP1HW1_4  :13;
        unsigned long               :3;
        unsigned long   SDCP1HW1_5  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp1hw1_4{
    unsigned long       word;
    struct {
        unsigned long   SDCP1HW1_6  :13;
        unsigned long               :3;
        unsigned long   SDCP1HW1_7  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp1hw1_5{
    unsigned long       word;
    struct {
        unsigned long   SDCP1HW1_8  :13;
        unsigned long               :3;
        unsigned long               :16;
    }bit;
};
struct io_sdcp1hw1{
    union io_sdcp1hw1_1  SDCP1HW11;
    union io_sdcp1hw1_2  SDCP1HW12;
    union io_sdcp1hw1_3  SDCP1HW13;
    union io_sdcp1hw1_4  SDCP1HW14;
    union io_sdcp1hw1_5  SDCP1HW15;
};

/*  structure of SDCN1HW0   (2840_89E0h)    */
union io_sdcn1hw0_1{
    unsigned long       word;
    struct {
        unsigned long   SDCN1HW0_0  :13;
        unsigned long               :3;
        unsigned long   SDCN1HW0_1  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn1hw0_2{
    unsigned long       word;
    struct {
        unsigned long   SDCN1HW0_2  :13;
        unsigned long               :3;
        unsigned long   SDCN1HW0_3  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn1hw0_3{
    unsigned long       word;
    struct {
        unsigned long   SDCN1HW0_4  :13;
        unsigned long               :3;
        unsigned long   SDCN1HW0_5  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn1hw0_4{
    unsigned long       word;
    struct {
        unsigned long   SDCN1HW0_6  :13;
        unsigned long               :3;
        unsigned long   SDCN1HW0_7  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn1hw0_5{
    unsigned long       word;
    struct {
        unsigned long   SDCN1HW0_8  :13;
        unsigned long               :3;
        unsigned long               :16;
    }bit;
};
struct io_sdcn1hw0{
    union io_sdcn1hw0_1  SDCN1HW01;
    union io_sdcn1hw0_2  SDCN1HW02;
    union io_sdcn1hw0_3  SDCN1HW03;
    union io_sdcn1hw0_4  SDCN1HW04;
    union io_sdcn1hw0_5  SDCN1HW05;
};

/*  structure of SDCN1HW1   (2840_8A00h)    */
union io_sdcn1hw1_1{
    unsigned long       word;
    struct {
        unsigned long   SDCN1HW1_0  :13;
        unsigned long               :3;
        unsigned long   SDCN1HW1_1  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn1hw1_2{
    unsigned long       word;
    struct {
        unsigned long   SDCN1HW1_2  :13;
        unsigned long               :3;
        unsigned long   SDCN1HW1_3  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn1hw1_3{
    unsigned long       word;
    struct {
        unsigned long   SDCN1HW1_4  :13;
        unsigned long               :3;
        unsigned long   SDCN1HW1_5  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn1hw1_4{
    unsigned long       word;
    struct {
        unsigned long   SDCN1HW1_6  :13;
        unsigned long               :3;
        unsigned long   SDCN1HW1_7  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn1hw1_5{
    unsigned long       word;
    struct {
        unsigned long   SDCN1HW1_8  :13;
        unsigned long               :3;
        unsigned long               :16;
    }bit;
};
struct io_sdcn1hw1{
    union io_sdcn1hw1_1  SDCN1HW11;
    union io_sdcn1hw1_2  SDCN1HW12;
    union io_sdcn1hw1_3  SDCN1HW13;
    union io_sdcn1hw1_4  SDCN1HW14;
    union io_sdcn1hw1_5  SDCN1HW15;
};

/*  structure of SDCP2HW0   (2840_8A20h)    */
union io_sdcp2hw0_1{
    unsigned long       word;
    struct {
        unsigned long   SDCP2HW0_0  :13;
        unsigned long               :3;
        unsigned long   SDCP2HW0_1  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp2hw0_2{
    unsigned long       word;
    struct {
        unsigned long   SDCP2HW0_2  :13;
        unsigned long               :3;
        unsigned long   SDCP2HW0_3  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp2hw0_3{
    unsigned long       word;
    struct {
        unsigned long   SDCP2HW0_4  :13;
        unsigned long               :3;
        unsigned long   SDCP2HW0_5  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp2hw0_4{
    unsigned long       word;
    struct {
        unsigned long   SDCP2HW0_6  :13;
        unsigned long               :3;
        unsigned long   SDCP2HW0_7  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp2hw0_5{
    unsigned long       word;
    struct {
        unsigned long   SDCP2HW0_8  :13;
        unsigned long               :3;
        unsigned long               :16;
    }bit;
};
struct io_sdcp2hw0{
    union io_sdcp2hw0_1  SDCP2HW01;
    union io_sdcp2hw0_2  SDCP2HW02;
    union io_sdcp2hw0_3  SDCP2HW03;
    union io_sdcp2hw0_4  SDCP2HW04;
    union io_sdcp2hw0_5  SDCP2HW05;
};

/*  structure of SDCP2HW1   (2840_8A40h)    */
union io_sdcp2hw1_1{
    unsigned long       word;
    struct {
        unsigned long   SDCP2HW1_0  :13;
        unsigned long               :3;
        unsigned long   SDCP2HW1_1  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp2hw1_2{
    unsigned long       word;
    struct {
        unsigned long   SDCP2HW1_2  :13;
        unsigned long               :3;
        unsigned long   SDCP2HW1_3  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp2hw1_3{
    unsigned long       word;
    struct {
        unsigned long   SDCP2HW1_4  :13;
        unsigned long               :3;
        unsigned long   SDCP2HW1_5  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp2hw1_4{
    unsigned long       word;
    struct {
        unsigned long   SDCP2HW1_6  :13;
        unsigned long               :3;
        unsigned long   SDCP2HW1_7  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcp2hw1_5{
    unsigned long       word;
    struct {
        unsigned long   SDCP2HW1_8  :13;
        unsigned long               :3;
        unsigned long               :16;
    }bit;
};
struct io_sdcp2hw1{
    union io_sdcp2hw1_1  SDCP2HW11;
    union io_sdcp2hw1_2  SDCP2HW12;
    union io_sdcp2hw1_3  SDCP2HW13;
    union io_sdcp2hw1_4  SDCP2HW14;
    union io_sdcp2hw1_5  SDCP2HW15;
};

/*  structure of SDCN2HW0   (2840_8A60h)    */
union io_sdcn2hw0_1{
    unsigned long       word;
    struct {
        unsigned long   SDCN2HW0_0  :13;
        unsigned long               :3;
        unsigned long   SDCN2HW0_1  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn2hw0_2{
    unsigned long       word;
    struct {
        unsigned long   SDCN2HW0_2  :13;
        unsigned long               :3;
        unsigned long   SDCN2HW0_3  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn2hw0_3{
    unsigned long       word;
    struct {
        unsigned long   SDCN2HW0_4  :13;
        unsigned long               :3;
        unsigned long   SDCN2HW0_5  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn2hw0_4{
    unsigned long       word;
    struct {
        unsigned long   SDCN2HW0_6  :13;
        unsigned long               :3;
        unsigned long   SDCN2HW0_7  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn2hw0_5{
    unsigned long       word;
    struct {
        unsigned long   SDCN2HW0_8  :13;
        unsigned long               :3;
        unsigned long               :16;
    }bit;
};
struct io_sdcn2hw0{
    union io_sdcn2hw0_1  SDCN2HW01;
    union io_sdcn2hw0_2  SDCN2HW02;
    union io_sdcn2hw0_3  SDCN2HW03;
    union io_sdcn2hw0_4  SDCN2HW04;
    union io_sdcn2hw0_5  SDCN2HW05;
};

/*  structure of SDCN2HW1   (2840_8A80h)    */
union io_sdcn2hw1_1{
    unsigned long       word;
    struct {
        unsigned long   SDCN2HW1_0  :13;
        unsigned long               :3;
        unsigned long   SDCN2HW1_1  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn2hw1_2{
    unsigned long       word;
    struct {
        unsigned long   SDCN2HW1_2  :13;
        unsigned long               :3;
        unsigned long   SDCN2HW1_3  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn2hw1_3{
    unsigned long       word;
    struct {
        unsigned long   SDCN2HW1_4  :13;
        unsigned long               :3;
        unsigned long   SDCN2HW1_5  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn2hw1_4{
    unsigned long       word;
    struct {
        unsigned long   SDCN2HW1_6  :13;
        unsigned long               :3;
        unsigned long   SDCN2HW1_7  :13;
        unsigned long               :3;
    }bit;
};
union io_sdcn2hw1_5{
    unsigned long       word;
    struct {
        unsigned long   SDCN2HW1_8  :13;
        unsigned long               :3;
        unsigned long               :16;
    }bit;
};
struct io_sdcn2hw1{
    union io_sdcn2hw1_1  SDCN2HW11;
    union io_sdcn2hw1_2  SDCN2HW12;
    union io_sdcn2hw1_3  SDCN2HW13;
    union io_sdcn2hw1_4  SDCN2HW14;
    union io_sdcn2hw1_5  SDCN2HW15;
};

/*  structure of SDCPC0 (2840_8AC0h)    */
union io_sdcpc0_1{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_0    :4;
        unsigned long               :4;
        unsigned long   SDCPC0_1    :4;
        unsigned long               :4;
        unsigned long   SDCPC0_2    :4;
        unsigned long               :4;
        unsigned long   SDCPC0_3    :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_2{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_4    :4;
        unsigned long               :4;
        unsigned long   SDCPC0_5    :4;
        unsigned long               :4;
        unsigned long   SDCPC0_6    :4;
        unsigned long               :4;
        unsigned long   SDCPC0_7    :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_3{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_8    :4;
        unsigned long               :4;
        unsigned long   SDCPC0_9    :4;
        unsigned long               :4;
        unsigned long   SDCPC0_10   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_11   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_4{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_12   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_13   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_14   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_15   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_5{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_16   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_17   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_18   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_19   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_6{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_20   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_21   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_22   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_23   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_7{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_24   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_25   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_26   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_27   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_8{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_28   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_29   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_30   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_31   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_9{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_32   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_33   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_34   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_35   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_10{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_36   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_37   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_38   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_39   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_11{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_40   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_41   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_42   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_43   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_12{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_44   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_45   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_46   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_47   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_13{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_48   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_49   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_50   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_51   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_14{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_52   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_53   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_54   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_55   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_15{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_56   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_57   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_58   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_59   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc0_16{
    unsigned long       word;
    struct {
        unsigned long   SDCPC0_60   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_61   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_62   :4;
        unsigned long               :4;
        unsigned long   SDCPC0_63   :4;
        unsigned long               :4;
    }bit;
};
struct io_sdcpc0{
    union io_sdcpc0_1    SDCPC01;
    union io_sdcpc0_2    SDCPC02;
    union io_sdcpc0_3    SDCPC03;
    union io_sdcpc0_4    SDCPC04;
    union io_sdcpc0_5    SDCPC05;
    union io_sdcpc0_6    SDCPC06;
    union io_sdcpc0_7    SDCPC07;
    union io_sdcpc0_8    SDCPC08;
    union io_sdcpc0_9    SDCPC09;
    union io_sdcpc0_10   SDCPC010;
    union io_sdcpc0_11   SDCPC011;
    union io_sdcpc0_12   SDCPC012;
    union io_sdcpc0_13   SDCPC013;
    union io_sdcpc0_14   SDCPC014;
    union io_sdcpc0_15   SDCPC015;
    union io_sdcpc0_16   SDCPC016;
};

/*  structure of SDCPC1 (2840_8B00h)    */
union io_sdcpc1_1{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_0    :4;
        unsigned long               :4;
        unsigned long   SDCPC1_1    :4;
        unsigned long               :4;
        unsigned long   SDCPC1_2    :4;
        unsigned long               :4;
        unsigned long   SDCPC1_3    :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_2{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_4    :4;
        unsigned long               :4;
        unsigned long   SDCPC1_5    :4;
        unsigned long               :4;
        unsigned long   SDCPC1_6    :4;
        unsigned long               :4;
        unsigned long   SDCPC1_7    :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_3{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_8    :4;
        unsigned long               :4;
        unsigned long   SDCPC1_9    :4;
        unsigned long               :4;
        unsigned long   SDCPC1_10   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_11   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_4{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_12   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_13   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_14   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_15   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_5{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_16   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_17   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_18   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_19   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_6{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_20   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_21   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_22   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_23   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_7{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_24   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_25   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_26   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_27   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_8{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_28   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_29   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_30   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_31   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_9{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_32   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_33   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_34   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_35   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_10{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_36   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_37   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_38   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_39   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_11{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_40   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_41   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_42   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_43   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_12{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_44   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_45   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_46   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_47   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_13{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_48   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_49   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_50   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_51   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_14{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_52   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_53   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_54   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_55   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_15{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_56   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_57   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_58   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_59   :4;
        unsigned long               :4;
    }bit;
};
union io_sdcpc1_16{
    unsigned long       word;
    struct {
        unsigned long   SDCPC1_60   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_61   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_62   :4;
        unsigned long               :4;
        unsigned long   SDCPC1_63   :4;
        unsigned long               :4;
    }bit;
};
struct io_sdcpc1{
    union io_sdcpc1_1    SDCPC11;
    union io_sdcpc1_2    SDCPC12;
    union io_sdcpc1_3    SDCPC13;
    union io_sdcpc1_4    SDCPC14;
    union io_sdcpc1_5    SDCPC15;
    union io_sdcpc1_6    SDCPC16;
    union io_sdcpc1_7    SDCPC17;
    union io_sdcpc1_8    SDCPC18;
    union io_sdcpc1_9    SDCPC19;
    union io_sdcpc1_10   SDCPC110;
    union io_sdcpc1_11   SDCPC111;
    union io_sdcpc1_12   SDCPC112;
    union io_sdcpc1_13   SDCPC113;
    union io_sdcpc1_14   SDCPC114;
    union io_sdcpc1_15   SDCPC115;
    union io_sdcpc1_16   SDCPC116;
};

/*  structure of SDCPCW0    (2840_8B40h)    */
union io_sdcpcw0{
    unsigned long       word;
    struct {
        unsigned long   SDCPCW0 :7;
        unsigned long           :25;
    }bit;
};

/*  structure of SDCPCW1    (2840_8B44h)    */
union io_sdcpcw1{
    unsigned long       word;
    struct {
        unsigned long   SDCPCW1 :7;
        unsigned long           :25;
    }bit;
};

/*  structure of SDCP1CW0   (2840_8B48h)    */
union io_sdcp1cw0{
    unsigned long       word;
    struct {
        unsigned long   SDCP1CW0    :13;
        unsigned long               :19;
    }bit;
};

/*  structure of SDCP1CW1   (2840_8B4Ch)    */
union io_sdcp1cw1{
    unsigned long       word;
    struct {
        unsigned long   SDCP1CW1    :13;
        unsigned long               :19;
    }bit;
};

/*  structure of SDCN1CW0   (2840_8B50h)    */
union io_sdcn1cw0{
    unsigned long       word;
    struct {
        unsigned long   SDCN1CW0    :13;
        unsigned long               :19;
    }bit;
};

/*  structure of SDCN1CW1   (2840_8B54h)    */
union io_sdcn1cw1{
    unsigned long       word;
    struct {
        unsigned long   SDCN1CW1    :13;
        unsigned long               :19;
    }bit;
};

/*  structure of SDCP2CW0   (2840_8B58h)    */
union io_sdcp2cw0{
    unsigned long       word;
    struct {
        unsigned long   SDCP2CW0    :13;
        unsigned long               :19;
    }bit;
};

/*  structure of SDCP2CW1   (2840_8B5Ch)    */
union io_sdcp2cw1{
    unsigned long       word;
    struct {
        unsigned long   SDCP2CW1    :13;
        unsigned long               :19;
    }bit;
};

/*  structure of SDCN2CW0   (2840_8B60h)    */
union io_sdcn2cw0{
    unsigned long       word;
    struct {
        unsigned long   SDCN2CW0    :13;
        unsigned long               :19;
    }bit;
};

/*  structure of SDCN2CW1   (2840_8B64h)    */
union io_sdcn2cw1{
    unsigned long       word;
    struct {
        unsigned long   SDCN2CW1    :13;
        unsigned long               :19;
    }bit;
};

/*  structure of SDCGMAXMIN (2840_8B9Ch)    */
union io_sdcgmaxmin{
    unsigned long       word;
    struct {
        unsigned long   SDCGMAX :12;
        unsigned long           :4;
        unsigned long   SDCGMIN :12;
        unsigned long           :4;
    }bit;
};

/*  structure of SDCNMAX    (2840_8BA0h)    */
union io_sdcnmax{
    unsigned long       word;
    struct {
        unsigned long   SDCNMAX :2;
        unsigned long           :30;
    }bit;
};

/* Define i/o mapping */
struct io_sdc{
    /* JDSIMG */
    union  io_sdctrg     SDCTRG;     /* 2840_(8800 - 8803h) */
    union  io_sdcpaen    SDCPAEN;    /* 2840_(8804 - 8807h) */
    union  io_sdcctl     SDCCTL;     /* 2840_(8808 - 880Bh) */
    union  io_sdcintenb  SDCINTENB;  /* 2840_(880C - 880Fh) */
    union  io_sdcintflg  SDCINTFLG;  /* 2840_(8810 - 8813h) */
    union  io_sdcsgd     SDCSGD;     /* 2840_(8814 - 8817h) */

    unsigned char dmy_8818_887F[0x8880-0x8818]; /* 2840_(8818 - 887Fh) */

    struct io_sdcph0     SDCPH0;     /* 2840_(8880 - 88C7h) */

    unsigned char dmy_88C8_88FF[0x8900-0x88C8]; /* 2840_(88C8 - 88FFh) */

    struct io_sdcph1     SDCPH1;     /* 2840_(8900 - 8947h) */

    unsigned char dmy_8948_897F[0x8980-0x8948]; /* 2840_(8948 - 897Fh) */

    struct io_sdcphw0    SDCPHW0;    /* 2840_(8980 - 898Bh) */

    unsigned char dmy_898C_898F[0x8990-0x898C]; /* 2840_(898C - 898Fh) */

    struct io_sdcphw1    SDCPHW1;    /* 2840_(8990 - 899Bh) */

    unsigned char dmy_899C_899F[0x89A0-0x899C]; /* 2840_(899C - 899Fh) */

    struct io_sdcp1hw0   SDCP1HW0;   /* 2840_(89A0 - 89B3h) */

    unsigned char dmy_89B4_89BF[0x89C0-0x89B4]; /* 2840_(89B4 - 89BFh) */

    struct io_sdcp1hw1   SDCP1HW1;   /* 2840_(89C0 - 89D3h) */

    unsigned char dmy_89D4_89DF[0x89E0-0x89D4]; /* 2840_(89D4 - 89DFh) */

    struct io_sdcn1hw0   SDCN1HW0;   /* 2840_(89E0 - 89F3h) */

    unsigned char dmy_89F4_89FF[0x8A00-0x89F4]; /* 2840_(89F4 - 89FFh) */

    struct io_sdcn1hw1   SDCN1HW1;   /* 2840_(8A00 - 8A13h) */

    unsigned char dmy_8A14_8A1F[0x8A20-0x8A14]; /* 2840_(8A14 - 8A1Fh) */

    struct io_sdcp2hw0   SDCP2HW0;   /* 2840_(8A20 - 8A33h) */

    unsigned char dmy_8A34_8A3F[0x8A40-0x8A34]; /* 2840_(8A34 - 8A3Fh) */

    struct io_sdcp2hw1   SDCP2HW1;   /* 2840_(8A40 - 8A53h) */

    unsigned char dmy_8A54_8A5F[0x8A60-0x8A54]; /* 2840_(8A54 - 8A5Fh) */

    struct io_sdcn2hw0   SDCN2HW0;   /* 2840_(8A60 - 8A73h) */

    unsigned char dmy_8A74_8A7F[0x8A80-0x8A74]; /* 2840_(8A74 - 8A7Fh) */

    struct io_sdcn2hw1   SDCN2HW1;   /* 2840_(8A80 - 8A93h) */

    unsigned char dmy_8A94_8ABF[0x8AC0-0x8A94]; /* 2840_(8A94 - 8ABFh) */

    struct io_sdcpc0     SDCPC0;     /* 2840_(8AC0 - 8AFFh) */
    struct io_sdcpc1     SDCPC1;     /* 2840_(8B00 - 8B3Fh) */
    union  io_sdcpcw0    SDCPCW0;    /* 2840_(8B40 - 8B43h) */
    union  io_sdcpcw1    SDCPCW1;    /* 2840_(8B44 - 8B47h) */
    union  io_sdcp1cw0   SDCP1CW0;   /* 2840_(8B48 - 8B4Bh) */
    union  io_sdcp1cw1   SDCP1CW1;   /* 2840_(8B4C - 8B4Fh) */
    union  io_sdcn1cw0   SDCN1CW0;   /* 2840_(8B50 - 8B53h) */
    union  io_sdcn1cw1   SDCN1CW1;   /* 2840_(8B54 - 8B57h) */
    union  io_sdcp2cw0   SDCP2CW0;   /* 2840_(8B58 - 8B5Bh) */
    union  io_sdcp2cw1   SDCP2CW1;   /* 2840_(8B5C - 8B5Fh) */
    union  io_sdcn2cw0   SDCN2CW0;   /* 2840_(8B60 - 8B63h) */
    union  io_sdcn2cw1   SDCN2CW1;   /* 2840_(8B64 - 8B67h) */

    unsigned char dmy_8B68_8B9B[0x8B9C-0x8B68]; /* 2840_(8B68 - 8B9Bh) */

    union  io_sdcgmaxmin SDCGMAXMIN; /* 2840_(8B9C - 8B9Fh) */
    union  io_sdcnmax    SDCNMAX;    /* 2840_(8BA0 - 8BA3h) */

    unsigned char dmy_8BA4_8BFF[0x8C00-0x8BA4]; /* 2840_(8BA4 - 8BFFh) */

};

#endif	// _IO_PRO_SDC_H_
