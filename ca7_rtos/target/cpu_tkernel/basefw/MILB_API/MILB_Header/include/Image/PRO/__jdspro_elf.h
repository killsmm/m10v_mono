/**
 * @file		__jdspro_elf.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_ELF_H_
#define _IO_PRO_ELF_H_

/*  structure of ELFTRG (2841_0400h)    */
union io_elftrg{
    unsigned long       word;
    struct {
        unsigned long   ELFTRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of NSLTRG (2841_0404h)    */
union io_nsltrg{
    unsigned long       word;
    struct {
        unsigned long   NSLTRG  :2;
        unsigned long           :30;
    }bit;
};

/*  structure of ELFPAEN    (2841_0408h)    */
union io_elfpaen{
    unsigned long       word;
    struct {
        unsigned long   EPAEN       :1;
        unsigned long               :3;
        unsigned long   EPAENA0     :1;
        unsigned long               :3;
        unsigned long   EPAENNSL    :1;
        unsigned long               :23;
    }bit;
};

/*  structure of ELFGWP (2841_040Ch)    */
union io_elfgwp{
    unsigned long       word;
    struct {
        unsigned long   ELFGH   :13;
        unsigned long           :3;
        unsigned long   ELFGV   :14;
        unsigned long           :2;
    }bit;
};

/*  structure of ELFGWS (2841_0410h)    */
union io_elfgws{
    unsigned long       word;
    struct {
        unsigned long   ELFGHW  :13;
        unsigned long           :3;
        unsigned long   ELFGVW  :14;
        unsigned long           :2;
    }bit;
};

/*  structure of ELFWP  (2841_0414h)    */
union io_elfwp{
    unsigned long       word;
    struct {
        unsigned long   ELFH    :13;
        unsigned long           :3;
        unsigned long   ELFV    :14;
        unsigned long           :2;
    }bit;
};

/*  structure of ELFWS  (2841_0418h)    */
union io_elfws{
    unsigned long       word;
    struct {
        unsigned long   ELFHW   :13;
        unsigned long           :3;
        unsigned long   ELFVW   :14;
        unsigned long           :2;
    }bit;
};

/*  structure of ELFCTL (2841_0420h)    */
union io_elfctl{
    unsigned long       word;
    struct {
        unsigned long   N1S5EN  :1;
        unsigned long   N5S9EN  :1;
        unsigned long   NFS9EN  :1;
        unsigned long   NLA0EN  :1;
        unsigned long   ELFPDD  :1;
        unsigned long   ELFRNG  :1;
        unsigned long           :2;
        unsigned long   F5MD    :1;
        unsigned long   F9MD    :1;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long           :2;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long           :1;
        unsigned long   EFMD    :1;
        unsigned long   LFMD    :1;
        unsigned long   NSLMD   :1;
        unsigned long   NSLRES  :2;
        unsigned long   NSLKNE  :1;
        unsigned long   ELFAFS  :1;
        unsigned long           :3;
    }bit;
};

/*  structure of ELFZP  (2841_0424h)    */
union io_elfzp{
    unsigned long       word;
    struct {
        unsigned long   ELFZP   :12;
        unsigned long           :20;
    }bit;
};

/*  structure of ELFL   (2841_0428h)    */
union io_elfl_1{
    unsigned long      word;
    struct {
        unsigned long  ELFL1   :12;
        unsigned long          :4;
        unsigned long  ELFL2   :12;
        unsigned long          :4;
    }bit;
};
union io_elfl_2{
    unsigned long      word;
    struct {
        unsigned long  ELFL3   :12;
        unsigned long          :4;
        unsigned long  ELFL4   :12;
        unsigned long          :4;
    }bit;
};
struct io_elfl{
    union io_elfl_1    ELFL1;
    union io_elfl_2    ELFL2;
};

/*  structure of ANBOR  (2841_0430h)    */
union io_anbor_1{
    unsigned long       word;
    struct {
        unsigned long   ANBOR_0 :12;
        unsigned long           :4;
        unsigned long   ANBOR_1 :12;
        unsigned long           :4;
    }bit;
};
union io_anbor_2{
    unsigned long       word;
    struct {
        unsigned long   ANBOR_2 :12;
        unsigned long           :4;
        unsigned long   ANBOR_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_anbor{
    union io_anbor_1    ANBOR1;
    union io_anbor_2    ANBOR2;
};

/*  structure of ANBKR  (2841_0438h)    */
union io_anbkr_1{
    unsigned long       word;
    struct {
        unsigned long   ANBKR_0 :12;
        unsigned long           :4;
        unsigned long   ANBKR_1 :12;
        unsigned long           :4;
    }bit;
};
union io_anbkr_2{
    unsigned long       word;
    struct {
        unsigned long   ANBKR_2 :12;
        unsigned long           :4;
        unsigned long   ANBKR_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_anbkr{
    union io_anbkr_1    ANBKR1;
    union io_anbkr_2    ANBKR2;
};

/*  structure of ANBDR  (2841_0440h)    */
union io_anbdr_1{
    unsigned long       word;
    struct {
        unsigned long   ANBDR_0 :12;
        unsigned long           :4;
        unsigned long   ANBDR_1 :12;
        unsigned long           :4;
    }bit;
};
union io_anbdr_2{
    unsigned long       word;
    struct {
        unsigned long   ANBDR_2 :12;
        unsigned long           :4;
        unsigned long   ANBDR_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_anbdr{
    union io_anbdr_1    ANBDR1;
    union io_anbdr_2    ANBDR2;
};

/*  structure of ANBOG  (2841_0448h)    */
union io_anbog_1{
    unsigned long       word;
    struct {
        unsigned long   ANBOG_0 :12;
        unsigned long           :4;
        unsigned long   ANBOG_1 :12;
        unsigned long           :4;
    }bit;
};
union io_anbog_2{
    unsigned long       word;
    struct {
        unsigned long   ANBOG_2 :12;
        unsigned long           :4;
        unsigned long   ANBOG_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_anbog{
    union io_anbog_1    ANBOG1;
    union io_anbog_2    ANBOG2;
};

/*  structure of ANBKG  (2841_0450h)    */
union io_anbkg_1{
    unsigned long       word;
    struct {
        unsigned long   ANBKG_0 :12;
        unsigned long           :4;
        unsigned long   ANBKG_1 :12;
        unsigned long           :4;
    }bit;
};
union io_anbkg_2{
    unsigned long       word;
    struct {
        unsigned long   ANBKG_2 :12;
        unsigned long           :4;
        unsigned long   ANBKG_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_anbkg{
    union io_anbkg_1    ANBKG1;
    union io_anbkg_2    ANBKG2;
};

/*  structure of ANBDG  (2841_0458h)    */
union io_anbdg_1{
    unsigned long       word;
    struct {
        unsigned long   ANBDG_0 :12;
        unsigned long           :4;
        unsigned long   ANBDG_1 :12;
        unsigned long           :4;
    }bit;
};
union io_anbdg_2{
    unsigned long       word;
    struct {
        unsigned long   ANBDG_2 :12;
        unsigned long           :4;
        unsigned long   ANBDG_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_anbdg{
    union io_anbdg_1    ANBDG1;
    union io_anbdg_2    ANBDG2;
};

/*  structure of ANBOB  (2841_0460h)    */
union io_anbob_1{
    unsigned long       word;
    struct {
        unsigned long   ANBOB_0 :12;
        unsigned long           :4;
        unsigned long   ANBOB_1 :12;
        unsigned long           :4;
    }bit;
};
union io_anbob_2{
    unsigned long       word;
    struct {
        unsigned long   ANBOB_2 :12;
        unsigned long           :4;
        unsigned long   ANBOB_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_anbob{
    union io_anbob_1    ANBOB1;
    union io_anbob_2    ANBOB2;
};

/*  structure of ANBKB  (2841_0468h)    */
union io_anbkb_1{
    unsigned long       word;
    struct {
        unsigned long   ANBKB_0 :12;
        unsigned long           :4;
        unsigned long   ANBKB_1 :12;
        unsigned long           :4;
    }bit;
};
union io_anbkb_2{
    unsigned long       word;
    struct {
        unsigned long   ANBKB_2 :12;
        unsigned long           :4;
        unsigned long   ANBKB_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_anbkb{
    union io_anbkb_1    ANBKB1;
    union io_anbkb_2    ANBKB2;
};

/*  structure of ANBDB  (2841_0470h)    */
union io_anbdb_1{
    unsigned long       word;
    struct {
        unsigned long   ANBDB_0 :12;
        unsigned long           :4;
        unsigned long   ANBDB_1 :12;
        unsigned long           :4;
    }bit;
};
union io_anbdb_2{
    unsigned long       word;
    struct {
        unsigned long   ANBDB_2 :12;
        unsigned long           :4;
        unsigned long   ANBDB_3 :12;
        unsigned long           :4;
    }bit;
};
struct io_anbdb{
    union io_anbdb_1    ANBDB1;
    union io_anbdb_2    ANBDB2;
};

/*  structure of LGTW   (2841_0478h)    */
union io_lgtw_1{
    unsigned long      word;
    struct {
        unsigned long  LGTWR   :10;
        unsigned long          :6;
        unsigned long  LGTWG   :10;
        unsigned long          :6;
    }bit;
};
union io_lgtw_2{
    unsigned long      word;
    struct {
        unsigned long  LGTWB   :10;
        unsigned long          :22;
    }bit;
};
struct io_lgtw{
    union io_lgtw_1    LGTW1;
    union io_lgtw_2    LGTW2;
};

/*  structure of LGT    (2841_0480h)    */
union io_lgt{
    unsigned long       word;
    struct {
        unsigned long   LGTR    :5;
        unsigned long           :3;
        unsigned long   LGTG    :5;
        unsigned long           :3;
        unsigned long   LGTB    :5;
        unsigned long           :3;
        unsigned long   LGTC    :5;
        unsigned long           :3;
    }bit;
};

/*  structure of LGTE   (2841_0484h)    */
union io_lgte{
    unsigned long       word;
    struct {
        unsigned long   LGTER   :5;
        unsigned long           :3;
        unsigned long   LGTEG   :5;
        unsigned long           :3;
        unsigned long   LGTEB   :5;
        unsigned long           :11;
    }bit;
};

/*  structure of EDG2ANBA   (2841_0488h)    */
union io_edg2anba{
    unsigned long       word;
    struct {
        unsigned long   EDG2ANBA    :8;
        unsigned long               :24;
    }bit;
};

/*  structure of EDGSCLA    (2841_048Ch)    */
union io_edgscla{
    unsigned long       word;
    struct {
        unsigned long   EDGSCLA :10;
        unsigned long           :22;
    }bit;
};

/*  structure of EDG2ANBB   (2841_0490h)    */
union io_edg2anbb{
    unsigned long       word;
    struct {
        unsigned long   EDG2ANBB    :10;
        unsigned long               :22;
    }bit;
};

/*  structure of EDGSCLB    (2841_0494h)    */
union io_edgsclb{
    unsigned long       word;
    struct {
        unsigned long   EDGSCLB :10;
        unsigned long           :22;
    }bit;
};

/*  structure of EDG2ANBC   (2841_0498h)    */
union io_edg2anbc{
    unsigned long       word;
    struct {
        unsigned long   EDG2ANBC    :10;
        unsigned long               :22;
    }bit;
};

/*  structure of EDGSCLC    (2841_049Ch)    */
union io_edgsclc{
    unsigned long       word;
    struct {
        unsigned long   EDGSCLC :10;
        unsigned long           :22;
    }bit;
};

/*  structure of EFTH   (2841_04A0h)    */
union io_efth_1{
    unsigned long      word;
    struct {
        unsigned long  EFL1TH  :10;
        unsigned long          :6;
        unsigned long  EFL2TH  :10;
        unsigned long          :6;
    }bit;
};
union io_efth_2{
    unsigned long      word;
    struct {
        unsigned long  EFL3TH  :10;
        unsigned long          :6;
        unsigned long  EFL4TH  :10;
        unsigned long          :6;
    }bit;
};
struct io_efth{
    union io_efth_1    EFTH1;
    union io_efth_2    EFTH2;
};

/*  structure of LFTH   (2841_04A8h)    */
union io_lfth{
    unsigned long       word;
    struct {
        long            LFTH    :11;
        unsigned long           :21;
    }bit;
};

/*  structure of ELCOME (2841_04ACh)    */
union io_elcome{
    unsigned long       word;
    struct {
        unsigned long   ELCOME1 :12;
        unsigned long           :4;
        unsigned long   ELCOME2 :12;
        unsigned long           :4;
    }bit;
};

/*  structure of ELCOMEN    (2841_04B0h)    */
union io_elcomen{
    unsigned long       word;
    struct {
        unsigned long   ELCOME1N    :9;
        unsigned long               :7;
        unsigned long   ELCOME2N    :9;
        unsigned long               :7;
    }bit;
};

/*  structure of A1S5E  (2841_04B8h)    */
union io_a1s5e_1{
    unsigned long      word;
    struct {
        unsigned long  A1S5E1  :12;
        unsigned long          :4;
        unsigned long  A1S5E2  :12;
        unsigned long          :4;
    }bit;
};
union io_a1s5e_2{
    unsigned long      word;
    struct {
        unsigned long  A1S5E3  :12;
        unsigned long          :20;
    }bit;
};
struct io_a1s5e{
    union io_a1s5e_1    A1S5E1;
    union io_a1s5e_2    A1S5E2;
};

/*  structure of A1S5L1 (2841_04C0h)    */
union io_a1s5l1_1{
    unsigned long      word;
    struct {
        unsigned long  A1S5L1E1N   :9;
        unsigned long              :7;
        unsigned long  A1S5L1E2N   :9;
        unsigned long              :7;
    }bit;
};
union io_a1s5l1_2{
    unsigned long      word;
    struct {
        unsigned long  A1S5L1E3N   :9;
        unsigned long              :23;
    }bit;
};
struct io_a1s5l1{
    union io_a1s5l1_1    A1S5L11;
    union io_a1s5l1_2    A1S5L12;
};

/*  structure of A1S5L2 (2841_04C8h)    */
union io_a1s5l2_1{
    unsigned long      word;
    struct {
        unsigned long  A1S5L2E1N   :9;
        unsigned long              :7;
        unsigned long  A1S5L2E2N   :9;
        unsigned long              :7;
    }bit;
};
union io_a1s5l2_2{
    unsigned long      word;
    struct {
        unsigned long  A1S5L2E3N   :9;
        unsigned long              :23;
    }bit;
};
struct io_a1s5l2{
    union io_a1s5l2_1    A1S5L21;
    union io_a1s5l2_2    A1S5L22;
};

/*  structure of A1S5L3 (2841_04D0h)    */
union io_a1s5l3_1{
    unsigned long      word;
    struct {
        unsigned long  A1S5L3E1N   :9;
        unsigned long              :7;
        unsigned long  A1S5L3E2N   :9;
        unsigned long              :7;
    }bit;
};
union io_a1s5l3_2{
    unsigned long      word;
    struct {
        unsigned long  A1S5L3E3N   :9;
        unsigned long              :23;
    }bit;
};
struct io_a1s5l3{
    union io_a1s5l3_1    A1S5L31;
    union io_a1s5l3_2    A1S5L32;
};

/*  structure of A1S5L4 (2841_04D8h)    */
union io_a1s5l4_1{
    unsigned long      word;
    struct {
        unsigned long  A1S5L4E1N   :9;
        unsigned long              :7;
        unsigned long  A1S5L4E2N   :9;
        unsigned long              :7;
    }bit;
};
union io_a1s5l4_2{
    unsigned long      word;
    struct {
        unsigned long  A1S5L4E3N   :9;
        unsigned long              :23;
    }bit;
};
struct io_a1s5l4{
    union io_a1s5l4_1    A1S5L41;
    union io_a1s5l4_2    A1S5L42;
};

/*  structure of NR1S5RGB   (2841_04E0h)    */
union io_nr1s5rgb_1{
    unsigned long      word;
    struct {
        unsigned long  NR1S5R  :9;
        unsigned long          :7;
        unsigned long  NR1S5G  :9;
        unsigned long          :7;
    }bit;
};
union io_nr1s5rgb_2{
    unsigned long      word;
    struct {
        unsigned long  NR1S5B  :9;
        unsigned long          :23;
    }bit;
};
struct io_nr1s5rgb{
    union io_nr1s5rgb_1    NR1S5RGB1;
    union io_nr1s5rgb_2    NR1S5RGB2;
};

/*  structure of NS1S5LM    (2841_04E8h)    */
union io_ns1s5lm{
    unsigned long       word;
    struct {
        unsigned long   NS1S5LMP    :12;
        unsigned long               :4;
        unsigned long   NS1S5LMM    :12;
        unsigned long               :4;
    }bit;
};

/*  structure of A5S9E  (2841_04F0h)    */
union io_a5s9e_1{
    unsigned long      word;
    struct {
        unsigned long  A5S9E1  :12;
        unsigned long          :4;
        unsigned long  A5S9E2  :12;
        unsigned long          :4;
    }bit;
};
union io_a5s9e_2{
    unsigned long      word;
    struct {
        unsigned long  A5S9E3  :12;
        unsigned long          :20;
    }bit;
};
struct io_a5s9e{
    union io_a5s9e_1    A5S9E1;
    union io_a5s9e_2    A5S9E2;
};

/*  structure of A5S9L1 (2841_04F8h)    */
union io_a5s9l1_1{
    unsigned long      word;
    struct {
        unsigned long  A5S9L1E1N   :9;
        unsigned long              :7;
        unsigned long  A5S9L1E2N   :9;
        unsigned long              :7;
    }bit;
};
union io_a5s9l1_2{
    unsigned long      word;
    struct {
        unsigned long  A5S9L1E3N   :9;
        unsigned long              :23;
    }bit;
};
struct io_a5s9l1{
    union io_a5s9l1_1    A5S9L11;
    union io_a5s9l1_2    A5S9L12;
};

/*  structure of A5S9L2 (2841_0500h)    */
union io_a5s9l2_1{
    unsigned long      word;
    struct {
        unsigned long  A5S9L2E1N   :9;
        unsigned long              :7;
        unsigned long  A5S9L2E2N   :9;
        unsigned long              :7;
    }bit;
};
union io_a5s9l2_2{
    unsigned long      word;
    struct {
        unsigned long  A5S9L2E3N   :9;
        unsigned long              :23;
    }bit;
};
struct io_a5s9l2{
    union io_a5s9l2_1    A5S9L21;
    union io_a5s9l2_2    A5S9L22;
};

/*  structure of A5S9L3 (2841_0508h)    */
union io_a5s9l3_1{
    unsigned long      word;
    struct {
        unsigned long  A5S9L3E1N   :9;
        unsigned long              :7;
        unsigned long  A5S9L3E2N   :9;
        unsigned long              :7;
    }bit;
};
union io_a5s9l3_2{
    unsigned long      word;
    struct {
        unsigned long  A5S9L3E3N   :9;
        unsigned long              :23;
    }bit;
};
struct io_a5s9l3{
    union io_a5s9l3_1    A5S9L31;
    union io_a5s9l3_2    A5S9L32;
};

/*  structure of A5S9L4 (2841_0510h)    */
union io_a5s9l4_1{
    unsigned long      word;
    struct {
        unsigned long  A5S9L4E1N   :9;
        unsigned long              :7;
        unsigned long  A5S9L4E2N   :9;
        unsigned long              :7;
    }bit;
};
union io_a5s9l4_2{
    unsigned long      word;
    struct {
        unsigned long  A5S9L4E3N   :9;
        unsigned long              :23;
    }bit;
};
struct io_a5s9l4{
    union io_a5s9l4_1    A5S9L41;
    union io_a5s9l4_2    A5S9L42;
};

/*  structure of NR5S9RGB   (2841_0518h)    */
union io_nr5s9rgb_1{
    unsigned long      word;
    struct {
        unsigned long  NR5S9G  :9;
        unsigned long          :7;
        unsigned long  NR5S9R  :9;
        unsigned long          :7;
    }bit;
};
union io_nr5s9rgb_2{
    unsigned long      word;
    struct {
        unsigned long  NR5S9B  :9;
        unsigned long          :23;
    }bit;
};
struct io_nr5s9rgb{
    union io_nr5s9rgb_1    NR5S9RGB1;
    union io_nr5s9rgb_2    NR5S9RGB2;
};

/*  structure of NS5S9LM    (2841_0520h)    */
union io_ns5s9lm{
    unsigned long       word;
    struct {
        unsigned long   NS5S9LMM    :12;
        unsigned long               :4;
        unsigned long   NS5S9LMP    :12;
        unsigned long               :4;
    }bit;
};

/*  structure of FS9W   (2841_0528h)    */
union io_fs9w_1{
    unsigned long      word;
    struct {
        unsigned long  FS9WG   :10;
        unsigned long          :6;
        unsigned long  FS9WR   :10;
        unsigned long          :6;
    }bit;
};
union io_fs9w_2{
    unsigned long      word;
    struct {
        unsigned long          :8;
        unsigned long          :8;
        unsigned long  FS9WB   :10;
        unsigned long          :6;
    }bit;
};
struct io_fs9w{
    union io_fs9w_1    FS9W1;
    union io_fs9w_2    FS9W2;
};

/*  structure of FS9YC  (2841_0540h)    */
union io_fs9yc_1{
    unsigned long      word;
    struct {
        long           FS9YC01 :9;
        unsigned long          :7;
        long           FS9YC00 :9;
        unsigned long          :7;
    }bit;
};
union io_fs9yc_2{
    unsigned long      word;
    struct {
        long           FS9YC10 :9;
        unsigned long          :7;
        long           FS9YC02 :9;
        unsigned long          :7;
    }bit;
};
union io_fs9yc_3{
    unsigned long      word;
    struct {
        long           FS9YC12 :9;
        unsigned long          :7;
        long           FS9YC11 :9;
        unsigned long          :7;
    }bit;
};
union io_fs9yc_4{
    unsigned long      word;
    struct {
        long           FS9YC21 :9;
        unsigned long          :7;
        long           FS9YC20 :9;
        unsigned long          :7;
    }bit;
};
union io_fs9yc_5{
    unsigned long       word;
    struct {
        unsigned long           :8;
        unsigned long           :8;
        long            FS9YC22 :9;
        unsigned long           :7;
    }bit;
};
struct io_fs9yc{
    union io_fs9yc_1    FS9YC1;
    union io_fs9yc_2    FS9YC2;
    union io_fs9yc_3    FS9YC3;
    union io_fs9yc_4    FS9YC4;
    union io_fs9yc_5    FS9YC5;
};

/*  structure of FASTR  (2841_0560h)    */
union io_fastr{
    unsigned long       word;
    struct {
        unsigned long   FASTR   :9;
        unsigned long           :23;
    }bit;
};

/*  structure of CRVAF  (2841_0564h)    */
union io_crvaf{
    unsigned long       word;
    struct {
        long            CRVAFY2 :8;
        long            CRVAFX2 :8;
        long            CRVAFY1 :8;
        long            CRVAFX1 :8;
    }bit;
};

/*  structure of FCRVABTOF  (2841_0568h)    */
union io_fcrvabtof{
    unsigned long       word;
    struct {
        unsigned long   FCRVABTOF1  :9;
        unsigned long               :23;
    }bit;
};

/*  structure of FCRVABTGA  (2841_056Ch)    */
union io_fcrvabtga{
    unsigned long       word;
    struct {
        unsigned long   FCRVABTGA1  :10;
        unsigned long               :6;
        unsigned long   FCRVABTGA0  :10;
        unsigned long               :6;
    }bit;
};

/*  structure of FCRVABTBD  (2841_0570h)    */
union io_fcrvabtbd{
    unsigned long       word;
    struct {
        unsigned long   FCRVABTBD1  :7;
        unsigned long               :25;
    }bit;
};

/*  structure of FCRVABTCP  (2841_0574h)    */
union io_fcrvabtcp{
    unsigned long       word;
    struct {
        unsigned long   FCRVABTCPL  :9;
        unsigned long               :7;
        unsigned long   FCRVABTCPH  :9;
        unsigned long               :7;
    }bit;
};

/*  structure of FYABTGA    (2841_0578h)    */
union io_fyabtga{
    unsigned long       word;
    struct {
        unsigned long   FYABTGA1    :8;
        unsigned long               :8;
        unsigned long   FYABTGA0    :8;
        unsigned long               :8;
    }bit;
};

/*  structure of FYABTBD    (2841_057Ch)    */
union io_fyabtbd{
    unsigned long       word;
    struct {
        unsigned long   FYABTBD1    :12;
        unsigned long               :4;
        unsigned long   FYABTBD0    :12;
        unsigned long               :4;
    }bit;
};

/*  structure of FYABTCLP   (2841_0580h)    */
union io_fyabtclp{
    unsigned long       word;
    struct {
        unsigned long   FYABTCLPL   :9;
        unsigned long               :7;
        unsigned long   FYABTCLPH   :9;
        unsigned long               :7;
    }bit;
};

/*  structure of FAEHH  (2841_0584h)    */
union io_faehh{
    unsigned long       word;
    struct {
        unsigned long   FAETHH  :12;
        unsigned long           :4;
        unsigned long   FAECHH  :4;
        unsigned long           :12;
    }bit;
};

/*  structure of FBSTR  (2841_0588h)    */
union io_fbstr{
    unsigned long       word;
    struct {
        unsigned long   FBSTR   :9;
        unsigned long           :23;
    }bit;
};

/*  structure of CRVBF  (2841_058Ch)    */
union io_crvbf{
    unsigned long       word;
    struct {
        long            CRVBFY2 :8;
        long            CRVBFX2 :8;
        long            CRVBFY1 :8;
        long            CRVBFX1 :8;
    }bit;
};

/*  structure of FCRVBBTOF  (2841_0590h)    */
union io_fcrvbbtof{
    unsigned long       word;
    struct {
        unsigned long   FCRVBBTOF1  :9;
        unsigned long               :23;
    }bit;
};

/*  structure of FCRVBBTGA  (2841_0594h)    */
union io_fcrvbbtga{
    unsigned long       word;
    struct {
        unsigned long   FCRVBBTGA1  :10;
        unsigned long               :6;
        unsigned long   FCRVBBTGA0  :10;
        unsigned long               :6;
    }bit;
};

/*  structure of FCRVBBTBD  (2841_0598h)    */
union io_fcrvbbtbd{
    unsigned long       word;
    struct {
        unsigned long   FCRVBBTBD1  :7;
        unsigned long               :25;
    }bit;
};

/*  structure of FCRVBBTCP  (2841_059Ch)    */
union io_fcrvbbtcp{
    unsigned long       word;
    struct {
        unsigned long   FCRVBBTCPL  :9;
        unsigned long               :7;
        unsigned long   FCRVBBTCPH  :9;
        unsigned long               :7;
    }bit;
};

/*  structure of FYBBTGA    (2841_05A0h)    */
union io_fybbtga{
    unsigned long       word;
    struct {
        unsigned long   FYBBTGA1    :8;
        unsigned long               :8;
        unsigned long   FYBBTGA0    :8;
        unsigned long               :8;
    }bit;
};

/*  structure of FYBBTBD    (2841_05A4h)    */
union io_fybbtbd{
    unsigned long       word;
    struct {
        unsigned long   FYBBTBD1    :12;
        unsigned long               :4;
        unsigned long   FYBBTBD0    :12;
        unsigned long               :4;
    }bit;
};

/*  structure of FYBBTCLP   (2841_05A8h)    */
union io_fybbtclp{
    unsigned long       word;
    struct {
        unsigned long   FYBBTCLPL   :9;
        unsigned long               :7;
        unsigned long   FYBBTCLPH   :9;
        unsigned long               :7;
    }bit;
};

/*  structure of FBEHH  (2841_05ACh)    */
union io_fbehh{
    unsigned long       word;
    struct {
        unsigned long   FBETHH  :12;
        unsigned long           :4;
        unsigned long   FBECHH  :4;
        unsigned long           :12;
    }bit;
};

/*  structure of ELFMNNRRL  (2841_05B0h)    */
union io_elfmnnrrl{
    unsigned long       word;
    struct {
        unsigned long   ELFMNNRRL1  :8;
        unsigned long   ELFMNNRRL2  :8;
        unsigned long   ELFMNNRRL3  :8;
        unsigned long   ELFMNNRRL4  :8;
    }bit;
};

/*  structure of ELFMNNRGL  (2841_05B4h)    */
union io_elfmnnrgl{
    unsigned long       word;
    struct {
        unsigned long   ELFMNNRGL1  :8;
        unsigned long   ELFMNNRGL2  :8;
        unsigned long   ELFMNNRGL3  :8;
        unsigned long   ELFMNNRGL4  :8;
    }bit;
};

/*  structure of ELFMNNRBL  (2841_05B8h)    */
union io_elfmnnrbl{
    unsigned long       word;
    struct {
        unsigned long   ELFMNNRBL1  :8;
        unsigned long   ELFMNNRBL2  :8;
        unsigned long   ELFMNNRBL3  :8;
        unsigned long   ELFMNNRBL4  :8;
    }bit;
};

/*  structure of ELFAFNR    (2841_05BCh)    */
union io_elfafnr{
    unsigned long       word;
    struct {
        unsigned long   ELFAFNR :8;
        unsigned long           :24;
    }bit;
};

/*  structure of ZELADCMD   (2841_05C0h)    */
union io_zeladcmd{
    unsigned long       word;
    struct {
        unsigned long   ZELADCMD    :2;
        unsigned long               :30;
    }bit;
};

/*  structure of ZANBOR (2841_05C8h)    */
union io_zanbor_1{
    unsigned long       word;
    struct {
        unsigned long   ZANBOR_0    :12;
        unsigned long               :4;
        unsigned long   ZANBOR_1    :12;
        unsigned long               :4;
    }bit;
};
union io_zanbor_2{
    unsigned long       word;
    struct {
        unsigned long   ZANBOR_2    :12;
        unsigned long               :4;
        unsigned long   ZANBOR_3    :12;
        unsigned long               :4;
    }bit;
};
struct io_zanbor{
    union io_zanbor_1   ZANBOR1;
    union io_zanbor_2   ZANBOR2;
};

/*  structure of ZANBKR (2841_05D0h)    */
union io_zanbkr_1{
    unsigned long       word;
    struct {
        unsigned long   ZANBKR_0    :12;
        unsigned long               :4;
        unsigned long   ZANBKR_1    :12;
        unsigned long               :4;
    }bit;
};
union io_zanbkr_2{
    unsigned long       word;
    struct {
        unsigned long   ZANBKR_2    :12;
        unsigned long               :4;
        unsigned long   ZANBKR_3    :12;
        unsigned long               :4;
    }bit;
};
struct io_zanbkr{
    union io_zanbkr_1   ZANBKR1;
    union io_zanbkr_2   ZANBKR2;
};

/*  structure of ZANBDR (2841_05D8h)    */
union io_zanbdr_1{
    unsigned long       word;
    struct {
        unsigned long   ZANBDR_0    :12;
        unsigned long               :4;
        unsigned long   ZANBDR_1    :12;
        unsigned long               :4;
    }bit;
};
union io_zanbdr_2{
    unsigned long       word;
    struct {
        unsigned long   ZANBDR_2    :12;
        unsigned long               :4;
        unsigned long   ZANBDR_3    :12;
        unsigned long               :4;
    }bit;
};
struct io_zanbdr{
    union io_zanbdr_1   ZANBDR1;
    union io_zanbdr_2   ZANBDR2;
};

/*  structure of ZANBOG (2841_05E0h)    */
union io_zanbog_1{
    unsigned long       word;
    struct {
        unsigned long   ZANBOG_0    :12;
        unsigned long               :4;
        unsigned long   ZANBOG_1    :12;
        unsigned long               :4;
    }bit;
};
union io_zanbog_2{
    unsigned long       word;
    struct {
        unsigned long   ZANBOG_2    :12;
        unsigned long               :4;
        unsigned long   ZANBOG_3    :12;
        unsigned long               :4;
    }bit;
};
struct io_zanbog{
    union io_zanbog_1   ZANBOG1;
    union io_zanbog_2   ZANBOG2;
};

/*  structure of ZANBKG (2841_05E8h)    */
union io_zanbkg_1{
    unsigned long       word;
    struct {
        unsigned long   ZANBKG_0    :12;
        unsigned long               :4;
        unsigned long   ZANBKG_1    :12;
        unsigned long               :4;
    }bit;
};
union io_zanbkg_2{
    unsigned long       word;
    struct {
        unsigned long   ZANBKG_2    :12;
        unsigned long               :4;
        unsigned long   ZANBKG_3    :12;
        unsigned long               :4;
    }bit;
};
struct io_zanbkg{
    union io_zanbkg_1   ZANBKG1;
    union io_zanbkg_2   ZANBKG2;
};

/*  structure of ZANBDG (2841_05F0h)    */
union io_zanbdg_1{
    unsigned long       word;
    struct {
        unsigned long   ZANBDG_0    :12;
        unsigned long               :4;
        unsigned long   ZANBDG_1    :12;
        unsigned long               :4;
    }bit;
};
union io_zanbdg_2{
    unsigned long       word;
    struct {
        unsigned long   ZANBDG_2    :12;
        unsigned long               :4;
        unsigned long   ZANBDG_3    :12;
        unsigned long               :4;
    }bit;
};
struct io_zanbdg{
    union io_zanbdg_1   ZANBDG1;
    union io_zanbdg_2   ZANBDG2;
};

/*  structure of ZANBOB (2841_05F8h)    */
union io_zanbob_1{
    unsigned long       word;
    struct {
        unsigned long   ZANBOB_0    :12;
        unsigned long               :4;
        unsigned long   ZANBOB_1    :12;
        unsigned long               :4;
    }bit;
};
union io_zanbob_2{
    unsigned long       word;
    struct {
        unsigned long   ZANBOB_2    :12;
        unsigned long               :4;
        unsigned long   ZANBOB_3    :12;
        unsigned long               :4;
    }bit;
};
struct io_zanbob{
    union io_zanbob_1   ZANBOB1;
    union io_zanbob_2   ZANBOB2;
};

/*  structure of ZANBKB (2841_0600h)    */
union io_zanbkb_1{
    unsigned long       word;
    struct {
        unsigned long   ZANBKB_0    :12;
        unsigned long               :4;
        unsigned long   ZANBKB_1    :12;
        unsigned long               :4;
    }bit;
};
union io_zanbkb_2{
    unsigned long       word;
    struct {
        unsigned long   ZANBKB_2    :12;
        unsigned long               :4;
        unsigned long   ZANBKB_3    :12;
        unsigned long               :4;
    }bit;
};
struct io_zanbkb{
    union io_zanbkb_1   ZANBKB1;
    union io_zanbkb_2   ZANBKB2;
};

/*  structure of ZANBDB (2841_0608h)    */
union io_zanbdb_1{
    unsigned long       word;
    struct {
        unsigned long   ZANBDB_0    :12;
        unsigned long               :4;
        unsigned long   ZANBDB_1    :12;
        unsigned long               :4;
    }bit;
};
union io_zanbdb_2{
    unsigned long       word;
    struct {
        unsigned long   ZANBDB_2    :12;
        unsigned long               :4;
        unsigned long   ZANBDB_3    :12;
        unsigned long               :4;
    }bit;
};
struct io_zanbdb{
    union io_zanbdb_1   ZANBDB1;
    union io_zanbdb_2   ZANBDB2;
};

/*  structure of ZLGTW  (2841_0610h)    */
union io_zlgtw_1{
    unsigned long      word;
    struct {
        unsigned long  ZLGTWR  :10;
        unsigned long          :6;
        unsigned long  ZLGTWG  :10;
        unsigned long          :6;
    }bit;
};
union io_zlgtw_2{
    unsigned long      word;
    struct {
        unsigned long  ZLGTWB  :10;
        unsigned long          :22;
    }bit;
};
struct io_zlgtw{
    union io_zlgtw_1   ZLGTW1;
    union io_zlgtw_2   ZLGTW2;
};

/*  structure of ZLGT   (2841_0618h)    */
union io_zlgt{
    unsigned long       word;
    struct {
        unsigned long   ZLGTR   :5;
        unsigned long           :3;
        unsigned long   ZLGTG   :5;
        unsigned long           :3;
        unsigned long   ZLGTB   :5;
        unsigned long           :3;
        unsigned long   ZLGTC   :5;
        unsigned long           :3;
    }bit;
};

/*  structure of ZLGTE  (2841_061Ch)    */
union io_zlgte{
    unsigned long       word;
    struct {
        unsigned long   ZLGTER  :5;
        unsigned long           :3;
        unsigned long   ZLGTEG  :5;
        unsigned long           :3;
        unsigned long   ZLGTEB  :5;
        unsigned long           :11;
    }bit;
};

/*  structure of ZEDG2ANBA  (2841_0620h)    */
union io_zedg2anba{
    unsigned long       word;
    struct {
        unsigned long   ZEDG2ANBA   :8;
        unsigned long               :24;
    }bit;
};

/*  structure of ZEDGSCLA   (2841_0624h)    */
union io_zedgscla{
    unsigned long       word;
    struct {
        unsigned long   ZEDGSCLA    :10;
        unsigned long               :22;
    }bit;
};

/*  structure of ZEDG2ANBB  (2841_0628h)    */
union io_zedg2anbb{
    unsigned long       word;
    struct {
        unsigned long   ZEDG2ANBB   :10;
        unsigned long               :22;
    }bit;
};

/*  structure of ZEDGSCLB   (2841_062Ch)    */
union io_zedgsclb{
    unsigned long       word;
    struct {
        unsigned long   ZEDGSCLB    :10;
        unsigned long               :22;
    }bit;
};

/*  structure of ZALNGE (2841_0630h)    */
union io_zalnge_1{
    unsigned long      word;
    struct {
        unsigned long  ZALNGE1 :12;
        unsigned long          :4;
        unsigned long  ZALNGE2 :12;
        unsigned long          :4;
    }bit;
};
union io_zalnge_2{
    unsigned long      word;
    struct {
        unsigned long  ZALNGE3 :12;
        unsigned long          :20;
    }bit;
};
struct io_zalnge{
    union io_zalnge_1   ZALNGE1;
    union io_zalnge_2   ZALNGE2;
};

/*  structure of ZALNG  (2841_0640h)    */
union io_zalng_1{
    unsigned long      word;
    struct {
        unsigned long  ZALNGL1E1N  :9;
        unsigned long              :7;
        unsigned long  ZALNGL1E2N  :9;
        unsigned long              :7;
    }bit;
};
union io_zalng_2{
    unsigned long      word;
    struct {
        unsigned long  ZALNGL1E3N  :9;
        unsigned long              :7;
        unsigned long  ZALNGL2E1N  :9;
        unsigned long              :7;
    }bit;
};
union io_zalng_3{
    unsigned long      word;
    struct {
        unsigned long  ZALNGL2E2N  :9;
        unsigned long              :7;
        unsigned long  ZALNGL2E3N  :9;
        unsigned long              :7;
    }bit;
};
union io_zalng_4{
    unsigned long      word;
    struct {
        unsigned long  ZALNGL3E1N  :9;
        unsigned long              :7;
        unsigned long  ZALNGL3E2N  :9;
        unsigned long              :7;
    }bit;
};
union io_zalng_5{
    unsigned long      word;
    struct {
        unsigned long  ZALNGL3E3N  :9;
        unsigned long              :7;
        unsigned long  ZALNGL4E1N  :9;
        unsigned long              :7;
    }bit;
};
union io_zalng_6{
    unsigned long      word;
    struct {
        unsigned long  ZALNGL4E2N  :9;
        unsigned long              :7;
        unsigned long  ZALNGL4E3N  :9;
        unsigned long              :7;
    }bit;
};
struct io_zalng{
    union io_zalng_1    ZALNG1;
    union io_zalng_2    ZALNG2;
    union io_zalng_3    ZALNG3;
    union io_zalng_4    ZALNG4;
    union io_zalng_5    ZALNG5;
    union io_zalng_6    ZALNG6;
};

/*  structure of ZLFTH  (2841_0660h)    */
union io_zlfth{
    unsigned long       word;
    struct {
        unsigned long   ZLFTH   :12;
        unsigned long           :20;
    }bit;
};

/*  structure of ZLCOME (2841_0664h)    */
union io_zlcome{
    unsigned long       word;
    struct {
        unsigned long   ZLCOME1 :12;
        unsigned long           :4;
        unsigned long   ZLCOME2 :12;
        unsigned long           :4;
    }bit;
};

/*  structure of ZLCOMEN    (2841_0668h)    */
union io_zlcomen{
    unsigned long       word;
    struct {
        unsigned long   ZLCOME1N    :9;
        unsigned long               :7;
        unsigned long   ZLCOME2N    :9;
        unsigned long               :7;
    }bit;
};

/*  structure of ZNSLNGLM   (2841_066Ch)    */
union io_znslnglm{
    unsigned long       word;
    struct {
        unsigned long   ZNSLNGLMP   :12;
        unsigned long               :4;
        unsigned long   ZNSLNGLMM   :12;
        unsigned long               :4;
    }bit;
};

/*  structure of EDGCATH    (2841_0684h)    */
union io_edgcath{
    unsigned long       word;
    struct {
        unsigned long   ELFTEST2    :8;
        unsigned long   ELFTEST1    :4;
        unsigned long               :4;
        unsigned long   EDGCATHA    :12;
        unsigned long               :4;
    }bit;
};

/* Define i/o mapping */
struct io_elf{
    /* JDSIMG */
    union  io_elftrg    ELFTRG;     /* 2841_(0400 - 0403h) */
    union  io_nsltrg    NSLTRG;     /* 2841_(0404 - 0407h) */
    union  io_elfpaen   ELFPAEN;    /* 2841_(0408 - 040Bh) */
    union  io_elfgwp    ELFGWP;     /* 2841_(040C - 040Fh) */
    union  io_elfgws    ELFGWS;     /* 2841_(0410 - 0413h) */
    union  io_elfwp     ELFWP;      /* 2841_(0414 - 0417h) */
    union  io_elfws     ELFWS;      /* 2841_(0418 - 041Bh) */

    unsigned char dmy_041C_041F[0x0420-0x041C]; /* 2841_(041C - 041Fh) */

    union  io_elfctl    ELFCTL;     /* 2841_(0420 - 0423h) */
    union  io_elfzp     ELFZP;      /* 2841_(0424 - 0427h) */
    struct io_elfl      ELFL;       /* 2841_(0428 - 042Fh) */
    struct io_anbor     ANBOR;      /* 2841_(0430 - 0437h) */
    struct io_anbkr     ANBKR;      /* 2841_(0438 - 043Fh) */
    struct io_anbdr     ANBDR;      /* 2841_(0440 - 0447h) */
    struct io_anbog     ANBOG;      /* 2841_(0448 - 044Fh) */
    struct io_anbkg     ANBKG;      /* 2841_(0450 - 0457h) */
    struct io_anbdg     ANBDG;      /* 2841_(0458 - 045Fh) */
    struct io_anbob     ANBOB;      /* 2841_(0460 - 0467h) */
    struct io_anbkb     ANBKB;      /* 2841_(0468 - 046Fh) */
    struct io_anbdb     ANBDB;      /* 2841_(0470 - 0477h) */
    struct io_lgtw      LGTW;       /* 2841_(0478 - 047Fh) */
    union  io_lgt       LGT;        /* 2841_(0480 - 0483h) */
    union  io_lgte      LGTE;       /* 2841_(0484 - 0487h) */
    union  io_edg2anba  EDG2ANBA;   /* 2841_(0488 - 048Bh) */
    union  io_edgscla   EDGSCLA;    /* 2841_(048C - 048Fh) */
    union  io_edg2anbb  EDG2ANBB;   /* 2841_(0490 - 0493h) */
    union  io_edgsclb   EDGSCLB;    /* 2841_(0494 - 0497h) */
    union  io_edg2anbc  EDG2ANBC;   /* 2841_(0498 - 049Bh) */
    union  io_edgsclc   EDGSCLC;    /* 2841_(049C - 049Fh) */
    struct io_efth      EFTH;       /* 2841_(04A0 - 04A7h) */
    union  io_lfth      LFTH;       /* 2841_(04A8 - 04ABh) */
    union  io_elcome    ELCOME;     /* 2841_(04AC - 04AFh) */
    union  io_elcomen   ELCOMEN;    /* 2841_(04B0 - 04B3h) */

    unsigned char dmy_04B4_04B7[0x04B8-0x04B4]; /* 2841_(04B4 - 04B7h) */

    struct io_a1s5e     A1S5E;      /* 2841_(04B8 - 04BFh) */
    struct io_a1s5l1    A1S5L1;     /* 2841_(04C0 - 04C7h) */
    struct io_a1s5l2    A1S5L2;     /* 2841_(04C8 - 04CFh) */
    struct io_a1s5l3    A1S5L3;     /* 2841_(04D0 - 04D7h) */
    struct io_a1s5l4    A1S5L4;     /* 2841_(04D8 - 04DFh) */
    struct io_nr1s5rgb  NR1S5RGB;   /* 2841_(04E0 - 04E7h) */
    union  io_ns1s5lm   NS1S5LM;    /* 2841_(04E8 - 04EBh) */

    unsigned char dmy_04EC_04EF[0x04F0-0x04EC]; /* 2841_(04EC - 04EFh) */

    struct io_a5s9e     A5S9E;      /* 2841_(04F0 - 04F7h) */
    struct io_a5s9l1    A5S9L1;     /* 2841_(04F8 - 04FFh) */
    struct io_a5s9l2    A5S9L2;     /* 2841_(0500 - 0507h) */
    struct io_a5s9l3    A5S9L3;     /* 2841_(0508 - 050Fh) */
    struct io_a5s9l4    A5S9L4;     /* 2841_(0510 - 0517h) */
    struct io_nr5s9rgb  NR5S9RGB;   /* 2841_(0518 - 051Fh) */
    union  io_ns5s9lm   NS5S9LM;    /* 2841_(0520 - 0523h) */

    unsigned char dmy_0524_0527[0x0528-0x0524]; /* 2841_(0524 - 0527h) */

    struct io_fs9w      FS9W;       /* 2841_(0528 - 052Fh) */

    unsigned char dmy_0530_053F[0x0540-0x0530]; /* 2841_(0530 - 053Fh) */

    struct io_fs9yc     FS9YC;      /* 2841_(0540 - 0553h) */

    unsigned char dmy_0554_055F[0x0560-0x0554]; /* 2841_(0554 - 055Fh) */

    union  io_fastr     FASTR;      /* 2841_(0560 - 0563h) */
    union  io_crvaf     CRVAF;      /* 2841_(0564 - 0567h) */
    union  io_fcrvabtof FCRVABTOF;  /* 2841_(0568 - 056Bh) */
    union  io_fcrvabtga FCRVABTGA;  /* 2841_(056C - 056Fh) */
    union  io_fcrvabtbd FCRVABTBD;  /* 2841_(0570 - 0573h) */
    union  io_fcrvabtcp FCRVABTCP;  /* 2841_(0574 - 0577h) */
    union  io_fyabtga   FYABTGA;    /* 2841_(0578 - 057Bh) */
    union  io_fyabtbd   FYABTBD;    /* 2841_(057C - 057Fh) */
    union  io_fyabtclp  FYABTCLP;   /* 2841_(0580 - 0583h) */
    union  io_faehh     FAEHH;      /* 2841_(0584 - 0587h) */
    union  io_fbstr     FBSTR;      /* 2841_(0588 - 058Bh) */
    union  io_crvbf     CRVBF;      /* 2841_(058C - 058Fh) */
    union  io_fcrvbbtof FCRVBBTOF;  /* 2841_(0590 - 0593h) */
    union  io_fcrvbbtga FCRVBBTGA;  /* 2841_(0594 - 0597h) */
    union  io_fcrvbbtbd FCRVBBTBD;  /* 2841_(0598 - 059Bh) */
    union  io_fcrvbbtcp FCRVBBTCP;  /* 2841_(059C - 059Fh) */
    union  io_fybbtga   FYBBTGA;    /* 2841_(05A0 - 05A3h) */
    union  io_fybbtbd   FYBBTBD;    /* 2841_(05A4 - 05A7h) */
    union  io_fybbtclp  FYBBTCLP;   /* 2841_(05A8 - 05ABh) */
    union  io_fbehh     FBEHH;      /* 2841_(05AC - 05AFh) */
    union  io_elfmnnrrl ELFMNNRRL;  /* 2841_(05B0 - 05B3h) */
    union  io_elfmnnrgl ELFMNNRGL;  /* 2841_(05B4 - 05B7h) */
    union  io_elfmnnrbl ELFMNNRBL;  /* 2841_(05B8 - 05BBh) */
    union  io_elfafnr   ELFAFNR;    /* 2841_(05BC - 05BFh) */
    union  io_zeladcmd  ZELADCMD;   /* 2841_(05C0 - 05C3h) */

    unsigned char dmy_05C4_05C7[0x05C8-0x05C4]; /* 2841_(05C4 - 05C7h) */

    struct io_zanbor    ZANBOR;     /* 2841_(05C8 - 05CFh) */
    struct io_zanbkr    ZANBKR;     /* 2841_(05D0 - 05D7h) */
    struct io_zanbdr    ZANBDR;     /* 2841_(05D8 - 05DFh) */
    struct io_zanbog    ZANBOG;     /* 2841_(05E0 - 05E7h) */
    struct io_zanbkg    ZANBKG;     /* 2841_(05E8 - 05EFh) */
    struct io_zanbdg    ZANBDG;     /* 2841_(05F0 - 05F7h) */
    struct io_zanbob    ZANBOB;     /* 2841_(05F8 - 05FFh) */
    struct io_zanbkb    ZANBKB;     /* 2841_(0600 - 0607h) */
    struct io_zanbdb    ZANBDB;     /* 2841_(0608 - 060Fh) */
    struct io_zlgtw     ZLGTW;      /* 2841_(0610 - 0617h) */
    union  io_zlgt      ZLGT;       /* 2841_(0618 - 061Bh) */
    union  io_zlgte     ZLGTE;      /* 2841_(061C - 061Fh) */
    union  io_zedg2anba ZEDG2ANBA;  /* 2841_(0620 - 0623h) */
    union  io_zedgscla  ZEDGSCLA;   /* 2841_(0624 - 0627h) */
    union  io_zedg2anbb ZEDG2ANBB;  /* 2841_(0628 - 062Bh) */
    union  io_zedgsclb  ZEDGSCLB;   /* 2841_(062C - 062Fh) */
    struct io_zalnge    ZALNGE;     /* 2841_(0630 - 0637h) */

    unsigned char dmy_0638_063F[0x0640-0x0638]; /* 2841_(0638 - 063Fh) */

    struct io_zalng     ZALNG;      /* 2841_(0640 - 0657h) */

    unsigned char dmy_0658_065F[0x0660-0x0658]; /* 2841_(0658 - 065Fh) */

    union  io_zlfth     ZLFTH;      /* 2841_(0660 - 0663h) */
    union  io_zlcome    ZLCOME;     /* 2841_(0664 - 0667h) */
    union  io_zlcomen   ZLCOMEN;    /* 2841_(0668 - 066Bh) */
    union  io_znslnglm  ZNSLNGLM;   /* 2841_(066C - 066Fh) */

    unsigned char dmy_0670_0683[0x0684-0x0670]; /* 2841_(0670 - 0683h) */

    union  io_edgcath   EDGCATH;    /* 2841_(0684 - 0687h) */

    unsigned char dmy_0688_07FF[0x0800-0x0688]; /* 2841_(0688 - 07FFh) */
};

#endif	// _IO_PRO_ELF_H_
