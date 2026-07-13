/**
@file os_user_custom.h
@brief OS function wrapper driver<br>
@author Kei Namerizawa (Fujitsu) <br>
@since 2012/01/26 12:00:00 <br>
*/

#ifndef _OS_USER_H_
#define _OS_USER_H_

/**
@addtogroup user_custom
@{
*/

#include "ddim_typedef.h"
//#include "tc_id.h"

/************************************************/
/* REALOS/ARM                                   */
/* (Copy from kernel.h / itron.h /_R_itron.h)   */
/************************************************/
/************************************************/
/* Parameter Data Type Define                   */
/************************************************/
typedef signed char         B;
typedef short               H;
typedef long                W;
typedef long long           D;
typedef unsigned char       UB;
typedef unsigned short      UH;
typedef unsigned long       UW;
typedef unsigned long long  UD;
typedef char                VB;
typedef short               VH;
typedef long                VW;
typedef long long           VD;
typedef void                *VP;
typedef void                (*FP)(void);
typedef int                 INT;
typedef unsigned int        UINT;
/* (FFCS)ligw 140221 WLAN--sta */
//typedef INT                 BOOL;
/* (FFCS)ligw 140221 WLAN--end */
typedef H                   FN;
typedef INT                 ER;
typedef H                   ID;
typedef UH                  ATR;
typedef UH                  STAT;
typedef UH                  MODE;
typedef H                   PRI;
typedef UINT                SIZE;
typedef INT                 TMO;
typedef UINT                RELTIM;
typedef unsigned long long  SYSTIM;
typedef INT                 VP_INT;
typedef ER                  ER_BOOL;
typedef ER                  ER_ID;
typedef ER                  ER_UINT;

/************************************************/
/* ITRON general constant data                  */
/************************************************/
#ifndef TRUE                /* True */
#define TRUE        1
#endif

#ifndef FALSE               /* or False */
#define FALSE       0
#endif

#ifndef _KERNEL_CONFIG          /* NULL */
#ifndef NULL
#define NULL        0
#endif
#endif

#define E_OK        0       /* No Error */

/************************************************/
/* Main error code                              */
/************************************************/
#define E_SYS       (-5)        /* System Error */
#define E_NOSPT     (-9)        /* Non Support */
#define E_RSFN      (-10)       /* Reserved Function Code */
#define E_RSATR     (-11)       /* Reserved Attribute */
#define E_PAR       (-17)       /* Parameter Error */
#define E_ID        (-18)       /* ID Over */
#define E_CTX       (-25)       /* Context Error */
#define E_MACV      (-26)       /* Memory Access Violation */
#define E_OACV      (-27)       /* Object Access Violation */
#define E_ILUSE     (-28)       /* Service Call Invarid Use */
#define E_NOMEM     (-33)       /* No Memory */
#define E_NOID      (-34)       /* No ID Number */
#define E_OBJ       (-41)       /* Object Error */
#define E_NOEXS     (-42)       /* Object is not Existing */
#define E_QOVR      (-43)       /* Queuing Count Overflow */
#define E_RLWAI     (-49)       /* Release Wait Error */
#define E_TMOUT     (-50)       /* Time Out Error */
#define E_DLT       (-51)       /* Object Deleted Error */
#define E_CLS       (-52)       /* Object Changed Error */
#define E_WBLK      (-57)       /* Recieve None Blocking */
#define E_BOVR      (-58)       /* Buffer over flow */

/************************************************/
/* Object attribute                             */
/************************************************/
#define TA_NULL     0   /* Object Attribute NULL */

/************************************************/
/* Timeoout                                     */
/************************************************/
#define TMO_POL     0           /* Polling */
#define TMO_FEVR    (-1)        /* Forever Wait */
#define TMO_NBLK    (-2)        /* Non-blocking */

/************************************************/
/* Error code detecting macro                   */
/************************************************/
/************************************************/
/* Parameter Data Type Define           */
/************************************************/
typedef UW  TEXPTN;
typedef UW  FLGPTN;
typedef UW  RDVPTN;
typedef W   RDVNO;
typedef UINT    OVRTIM;
typedef H   INHNO;
typedef H   INTNO;
typedef H   EXCNO;

/************************************************/
/* Construct Constant and Macro Define          */
/************************************************/
#define TKERNEL_PRID        0x486a      /* REALOS/ARM */
#define TKERNEL_PRVER       0x0101      /* V01L01 */

/************************************************/
/* System Call Function Macro Define            */
/************************************************/
#define ret_int()       { return; }
#define ret_tmr()       { return; }

/************************************************/
/* Expand Infomation Macro                      */
/************************************************/
/* T_CTSK */
#define _KERNEL_CTSK_IMPL
/* T_RCFG */
#define _KERNEL_RCFG_IMPL

/************************************************/
/* System Call Function Code Define             */
/************************************************/
#define TFN_CHG_ILM     (-0x6B)
#define TFN_GET_ILM     (-0x6C)
#define TFN_VSTP_TIM    (-0xED) /* original */
#define TFN_VSTA_TIM    (-0xEE) /* original */

/************************************************/
/* System Call Function Define                  */
/************************************************/
ER chg_ilm(UINT);
ER get_ilm(UINT *);
ER vstp_tim(void);
ER vsta_tim(void);

/************************************************/
/* Object Attribute                             */
/************************************************/
#define TA_HLNG     0x00000000  /* High Level Language */
#define TA_ASM      0x00000001  /* Assembler Language */

#define TA_TFIFO    0x00000000  /* FIFO-ordered queuing */
#define TA_TPRI     0x00000001  /* Priority-ordered queuing */

#define TA_MFIFO    0x00000000  /* FIFO-ordered queuing to MBX */
#define TA_MPRI     0x00000002  /* Priority-ordered queuing to MBX */

#define TA_ACT      0x00000002  /* start task */
#define TA_RSTR     0x00000004  /* restriction task */

#define TA_WSGL     0x00000000  /* single waiting */
#define TA_WMUL     0x00000002  /* multiple waiting */

#define TA_CLR      0x00000004  /* clear eventflag pattern */

#define TA_INHERIT  0x00000002  /* Mutex supports                  */
                                        /*   priority inheritance protocol */
#define TA_CEILING  0x00000003  /* Mutex supports                  */
                                        /*       priority ceiling protocol */

#define TA_STA      0x00000002  /* start cyclic handler */
#define TA_PHS      0x00000004  /* save phase of cyclic handler */

/************************************************/
/* Mode of Service Call                         */
/************************************************/
#define TWF_ANDW    0x00000000  /* EventFlag And Wait */
#define TWF_ORW     0x00000001  /* EventFlag Or Wait */

/************************************************/
/* Object State                                 */
/************************************************/
#define TTS_RUN     0x00000001  /* RUN */
#define TTS_RDY     0x00000002  /* READY */
#define TTS_WAI     0x00000004  /* WAIT */
#define TTS_SUS     0x00000008  /* SUSPEND */
#define TTS_WAS     0x0000000c  /* WAIT-SUSPEND */
#define TTS_DMT     0x00000010  /* DORMANT */

#define TTW_SLP     0x00000001  /* SLP_WAIT */
#define TTW_DLY     0x00000002  /* DLY_WAIT */
#define TTW_SEM     0x00000004  /* SEM_WAIT */
#define TTW_FLG     0x00000008  /* FLG_WAIT */
#define TTW_SDTQ    0x00000010  /* SND_DTQ_WAIT */
#define TTW_RDTQ    0x00000020  /* RCV_DTQ_WAIT */
#define TTW_MBX     0x00000040  /* MBX_WAIT */
#define TTW_MTX     0x00000080  /* MTX_WAIT */
#define TTW_SMBF    0x00000100  /* SND_MBF_WAIT */
#define TTW_RMBF    0x00000200  /* RCV_MBF_WAIT */
#define TTW_CAL     0x00000400  /* RDV_CAL_WAIT */
#define TTW_ACP     0x00000800  /* RDV_ACP_WAIT */
#define TTW_RDV     0x00001000  /* RDV_CPL_WAIT */
#define TTW_MPF     0x00002000  /* MPF_WAIT */
#define TTW_MPL     0x00004000  /* MPL_WAIT */

#define TTEX_ENA    0x00000000  /* Enable TEX state */
#define TTEX_DIS    0x00000001  /* Disable TEX state */

#define TCYC_STP    0x00000000  /* stop cyclic handler */
#define TCYC_STA    0x00000001  /* start cyclic handler */

#define TALM_STP    0x00000000  /* stop alarm handler */
#define TALM_STA    0x00000001  /* start alarm handler */

#define TOVR_STP    0x00000000  /* limit processor time is not set */
#define TOVR_STA    0x00000001  /* limit processor time is set */

/************************************************/
/* Etc                                          */
/************************************************/
#define TSK_SELF    0       /* Self */
#define TSK_NONE    0       /* none */

#define TPRI_INI    0       /* Initial Priority */
#define TPRI_SELF   0       /* Task-self Priority */

#define VTSS_NCTX   0x01    /* Task-independent portion */
#define VTSS_LOC    0x02    /* CPU locked state */
#define VTSS_DDSP   0x04    /* Dispatch disable state */

/************************************************/
/* System Call Function Code Define             */
/************************************************/
#define TFN_CRE_TSK (-0x05)
#define TFN_DEL_TSK (-0x06)
#define TFN_ACT_TSK (-0x07)
#define TFN_CAN_ACT (-0x08)
#define TFN_STA_TSK (-0x09)
#define TFN_EXT_TSK (-0x0A)
#define TFN_EXD_TSK (-0x0B)
#define TFN_TER_TSK (-0x0C)
#define TFN_CHG_PRI (-0x0D)
#define TFN_GET_PRI (-0x0E)
#define TFN_REF_TSK (-0x0F)
#define TFN_REF_TST (-0x10)

#define TFN_SLP_TSK (-0x11)
#define TFN_TSLP_TSK    (-0x12)
#define TFN_WUP_TSK (-0x13)
#define TFN_CAN_WUP (-0x14)
#define TFN_REL_WAI (-0x15)
#define TFN_SUS_TSK (-0x16)
#define TFN_RSM_TSK (-0x17)
#define TFN_FRSM_TSK    (-0x18)
#define TFN_DLY_TSK (-0x19)
/* #define  RESERVE (-0x1A) */

#define TFN_DEF_TEX (-0x1B)
#define TFN_RAS_TEX (-0x1C)
#define TFN_DIS_TEX (-0x1D)
#define TFN_ENA_TEX (-0x1E)
#define TFN_SNS_TEX (-0x1F)
#define TFN_REF_TEX (-0x20)

#define TFN_CRE_SEM (-0x21)
#define TFN_DEL_SEM (-0x22)
#define TFN_SIG_SEM (-0x23)
/* #define  RESERVE (-0x24) */
#define TFN_WAI_SEM (-0x25)
#define TFN_POL_SEM (-0x26)
#define TFN_TWAI_SEM    (-0x27)
#define TFN_REF_SEM (-0x28)

#define TFN_CRE_FLG (-0x29)
#define TFN_DEL_FLG (-0x2A)
#define TFN_SET_FLG (-0x2B)
#define TFN_CLR_FLG (-0x2C)
#define TFN_WAI_FLG (-0x2D)
#define TFN_POL_FLG (-0x2E)
#define TFN_TWAI_FLG    (-0x2F)
#define TFN_REF_FLG (-0x30)

#define TFN_CRE_DTQ (-0x31)
#define TFN_DEL_DTQ (-0x32)
/* #define  RESERVE (-0x33) */
/* #define  RESERVE (-0x34) */
#define TFN_SND_DTQ (-0x35)
#define TFN_PSND_DTQ    (-0x36)
#define TFN_TSND_DTQ    (-0x37)
#define TFN_FSND_DTQ    (-0x38)
#define TFN_RCV_DTQ (-0x39)
#define TFN_PRCV_DTQ    (-0x3A)
#define TFN_TRCV_DTQ    (-0x3B)
#define TFN_REF_DTQ (-0x3C)

#define TFN_CRE_MBX (-0x3D)
#define TFN_DEL_MBX (-0x3E)
#define TFN_SND_MBX (-0x3F)
/* #define  RESERVE (-0x40) */
#define TFN_RCV_MBX (-0x41)
#define TFN_PRCV_MBX    (-0x42)
#define TFN_TRCV_MBX    (-0x43)
#define TFN_REF_MBX (-0x44)

#define TFN_CRE_MPF (-0x45)
#define TFN_DEL_MPF (-0x46)
#define TFN_REL_MPF (-0x47)
/* #define  RESERVE (-0x48) */
#define TFN_GET_MPF (-0x49)
#define TFN_PGET_MPF    (-0x4A)
#define TFN_TGET_MPF    (-0x4B)
#define TFN_REF_MPF (-0x4C)

#define TFN_SET_TIM (-0x4D)
#define TFN_GET_TIM (-0x4E)
#define TFN_CRE_CYC (-0x4F)
#define TFN_DEL_CYC (-0x50)
#define TFN_STA_CYC (-0x51)
#define TFN_STP_CYC (-0x52)
#define TFN_REF_CYC (-0x53)
/* #define  RESERVE (-0x54) */
#define TFN_ROT_RDQ (-0x55)
#define TFN_GET_TID (-0x56)
/* #define  RESERVE (-0x57) */
/* #define  RESERVE (-0x58) */

#define TFN_LOC_CPU (-0x59)
#define TFN_UNL_CPU (-0x5A)
#define TFN_DIS_DSP (-0x5B)
#define TFN_ENA_DSP (-0x5C)
#define TFN_SNS_CTX (-0x5D)
#define TFN_SNS_LOC (-0x5E)
#define TFN_SNS_DSP (-0x5F)
#define TFN_SNS_DPN (-0x60)
#define TFN_REF_SYS (-0x61)
/* #define  RESERVE (-0x62) */
/* #define  RESERVE (-0x63) */
/* #define  RESERVE (-0x64) */

#define TFN_DEF_INH (-0x65)
#define TFN_CRE_ISR (-0x66)
#define TFN_DEL_ISR (-0x67)
#define TFN_REF_ISR (-0x68)
#define TFN_DIS_INT (-0x69)
#define TFN_ENA_INT (-0x6A)
/* #define  TFN_CHG_IXX (-0x6B) */
/* #define  TFN_GET_IXX (-0x6C) */
#define TFN_DEF_SVC (-0x6D)
#define TFN_DEF_EXC (-0x6E)
#define TFN_REF_CFG (-0x6F)
#define TFN_REF_VER (-0x70)

#define TFN_IACT_TSK    (-0x71)
#define TFN_IWUP_TSK    (-0x72)
#define TFN_IREL_WAI    (-0x73)
#define TFN_IRAS_TEX    (-0x74)
#define TFN_ISIG_SEM    (-0x75)
#define TFN_ISET_FLG    (-0x76)
#define TFN_IPSND_DTQ   (-0x77)
#define TFN_IFSND_DTQ   (-0x78)
#define TFN_IROT_RDQ    (-0x79)
#define TFN_IGET_TID    (-0x7A)
#define TFN_ILOC_CPU    (-0x7B)
#define TFN_IUNL_CPU    (-0x7C)
#define TFN_ISIG_TIM    (-0x7D)
/* #define  RESERVE (-0x7E) */
/* #define  RESERVE (-0x7F) */
/* #define  RESERVE (-0x80) */

#define TFN_CRE_MTX (-0x81)
#define TFN_DEL_MTX (-0x82)
#define TFN_UNL_MTX (-0x83)
/* #define  RESERVE (-0x84) */
#define TFN_LOC_MTX (-0x85)
#define TFN_PLOC_MTX    (-0x86)
#define TFN_TLOC_MTX    (-0x87)
#define TFN_REF_MTX (-0x88)

#define TFN_CRE_MBF (-0x89)
#define TFN_DEL_MBF (-0x8A)
/* #define  RESERVE (-0x8B) */
/* #define  RESERVE (-0x8C) */
#define TFN_SND_MBF (-0x8D)
#define TFN_PSND_MBF    (-0x8E)
#define TFN_TSND_MBF    (-0x8F)
/* #define  RESERVE (-0x90) */
#define TFN_RCV_MBF (-0x91)
#define TFN_PRCV_MBF    (-0x92)
#define TFN_TRCV_MBF    (-0x93)
#define TFN_REF_MBF (-0x94)

#define TFN_CRE_POR (-0x95)
#define TFN_DEL_POR (-0x96)
#define TFN_CAL_POR (-0x97)
#define TFN_TCAL_POR    (-0x98)
#define TFN_ACP_POR (-0x99)
#define TFN_PACP_POR    (-0x9A)
#define TFN_TACP_POR    (-0x9B)
#define TFN_FWD_POR (-0x9C)
#define TFN_RPL_RDV (-0x9D)
#define TFN_REF_POR (-0x9E)
#define TFN_REF_RDV (-0x9F)
/* #define  RESERVE (-0xA0) */

#define TFN_CRE_MPL (-0xA1)
#define TFN_DEL_MPL (-0xA2)
#define TFN_REL_MPL (-0xA3)
/* #define  RESERVE (-0xA4) */
#define TFN_GET_MPL (-0xA5)
#define TFN_PGET_MPL    (-0xA6)
#define TFN_TGET_MPL    (-0xA7)
#define TFN_REF_MPL (-0xA8)

#define TFN_CRE_ALM (-0xA9)
#define TFN_DEL_ALM (-0xAA)
#define TFN_STA_ALM (-0xAB)
#define TFN_STP_ALM (-0xAC)
#define TFN_REF_ALM (-0xAD)

#define TFN_DEF_OVR (-0xB1)
#define TFN_STA_OVR (-0xB2)
#define TFN_STP_OVR (-0xB3)
#define TFN_REF_OVR (-0xB4)

#define TFN_ACRE_TSK    (-0xC1)
#define TFN_ACRE_SEM    (-0xC2)
#define TFN_ACRE_FLG    (-0xC3)
#define TFN_ACRE_DTQ    (-0xC4)
#define TFN_ACRE_MBX    (-0xC5)
#define TFN_ACRE_MTX    (-0xC6)
#define TFN_ACRE_MBF    (-0xC7)
#define TFN_ACRE_POR    (-0xC8)
#define TFN_ACRE_MPF    (-0xC9)
#define TFN_ACRE_MPL    (-0xCA)
#define TFN_ACRE_CYC    (-0xCB)
#define TFN_ACRE_ALM    (-0xCC)
#define TFN_ACRE_ISR    (-0xCD)

#define TFN_VDMP_SYS    (-0xE1) /* original */

#define TFN_VCRE_SHP    (-0xE5) /* original */
#define TFN_VDEL_SHP    (-0xE6) /* original */
#define TFN_VALC_SHP    (-0xE7) /* original */
#define TFN_VFRE_SHP    (-0xE8) /* original */
#define TFN_VREF_SHP    (-0xE9) /* original */

/* Function Group Define for MULTI Debugger */
#define VTFN_TSK        0x0001
#define VTFN_TCOM       0x0002
#define VTFN_TEX        0x0004
#define VTFN_SEM        0x0008
#define VTFN_FLG        0x0010
#define VTFN_DTQ        0x0020
#define VTFN_MBX        0x0040
#define VTFN_MTX        0x0080
#define VTFN_MBF        0x0100
#define VTFN_POR        0x0200
#define VTFN_MPF        0x0400
#define VTFN_MPL        0x0800
#define VTFN_TIM        0x1000
#define VTFN_SYS        0x2000
#define VTFN_INT        0x4000
#define VTFN_OTHER      0x8000

/************************************************/
/* Construct Constant and Macro Define          */
/************************************************/
#define TMIN_TPRI   1
#define TMIN_MPRI   1
#define TMAX_MPRI   32767
#define TMAX_ACTCNT 32767
#define TMAX_WUPCNT 32767
#define TMAX_SUSCNT 32767
#define TMAX_MAXSEM 65535
#define TBIT_TEXPTN 32
#define TBIT_FLGPTN 32
#define TBIT_RDVPTN 32
#define TIC_NUME    1
#define TIC_DENO    1
#define TKERNEL_MAKER   0x0009  /* FUJITSU */
#define TKERNEL_SPVER   0x5401  /* uITRON4.01 */

/************************************************/
/* Memory Area Size Macro Define                */
/************************************************/
#define TSZ_DTQ(dtqcnt) ((dtqcnt)*4)
#define TSZ_MPRIHD(maxmpri) 0
#define TSZ_MBF(msgcnt,msgsz) ((msgcnt)*(4+(msgsz))+4)
#define TSZ_MPF(blkcnt, blksz) ((blkcnt)*(blksz))
#define TSZ_MPL(blkcnt, blksz) ((blkcnt)*(blksz)+((blkcnt)+1)*4)


/************************************************/
/* Original Constant and Macro Define           */
/************************************************/
/* For user definition error routine */
#define _KERNEL_ERR_SYS_DOWN 0x01
#define _KERNEL_ERR_INI_ERR 0x02
#define _KERNEL_ERR_EIT_DOWN 0x04
#define _KERNEL_ERR_CPU_EXC 0x08
#define _KERNEL_ERR_DEF_MAX 0x10

#define _KERNEL_ERR_MAX_TPRI 0x0001
#define _KERNEL_ERR_MAX_TSK 0x0002
#define _KERNEL_ERR_MAX_SEM 0x0004
#define _KERNEL_ERR_MAX_FLG 0x0008
#define _KERNEL_ERR_MAX_DTQ 0x0010
#define _KERNEL_ERR_MAX_MBX 0x0020
#define _KERNEL_ERR_MAX_MTX 0x0040
#define _KERNEL_ERR_MAX_MBF 0x0080
#define _KERNEL_ERR_MAX_MPF 0x0100
#define _KERNEL_ERR_MAX_MPL 0x0200
#define _KERNEL_ERR_MAX_CYC 0x0400
#define _KERNEL_ERR_MAX_ALM 0x0800
#define _KERNEL_ERR_IDL_STKSZ 0x1000
#define _KERNEL_ERR_MAX_INIRTN 0x2000
#define _KERNEL_ERR_MAX_ERRRTN 0x4000

/************************************************/
/* System Call Parameter Packet Define          */
/************************************************/
typedef struct t_ctsk{
    ATR tskatr;
    VP_INT  exinf;
    FP  task;
    PRI itskpri;
    SIZE    stksz;
    VP  stk;
    _KERNEL_CTSK_IMPL
}T_CTSK;

typedef struct t_rtsk{
    STAT    tskstat;
    PRI tskpri;
    PRI tskbpri;
    STAT    tskwait;
    ID  wobjid;
    TMO lefttmo;
    UINT    actcnt;
    UINT    wupcnt;
    UINT    suscnt;
    VP  stk;
    VP  stktop;
}T_RTSK;

typedef struct t_rtst{
    STAT    tskstat;
    STAT    tskwait;
}T_RTST;

typedef struct t_dtex{
    ATR texatr;
    FP  texrtn;
}T_DTEX;

typedef struct t_rtex{
    STAT    texstat;
    TEXPTN  pndptn;
}T_RTEX;

typedef struct t_csem{
    ATR sematr;
    UINT    isemcnt;
    UINT    maxsem;
}T_CSEM;

typedef struct t_rsem{
    ID  wtskid;
    UINT    semcnt;
}T_RSEM;

typedef struct t_cflg{
    ATR flgatr;
    FLGPTN  iflgptn;
}T_CFLG;

typedef struct t_rflg{
    ID  wtskid;
    FLGPTN  flgptn;
}T_RFLG;

typedef struct t_cdtq{
    ATR dtqatr;
    UINT    dtqcnt;
    VP  dtq;
}T_CDTQ;

typedef struct t_rdtq{
    ID  stskid;
    ID  rtskid;
    UINT    sdtqcnt;
}T_RDTQ;

typedef struct t_cmbx{
    ATR mbxatr;
    PRI maxmpri;
    VP  mprihd;
}T_CMBX;

typedef struct  t_msg{
    struct t_msg    *hmsg;
}T_MSG;

typedef struct  t_msg_pri{
    T_MSG   msgque;
    PRI msgpri;
}T_MSG_PRI;

typedef struct t_rmbx{
    ID  wtskid;
    T_MSG   *pk_msg;
}T_RMBX;

typedef struct t_cmtx{
    ATR mtxatr;
    PRI ceilpri;
}T_CMTX;

typedef struct t_rmtx{
    ID  htskid;
    ID  wtskid;
}T_RMTX;

typedef struct t_cmbf{
    ATR mbfatr;
    UINT    maxmsz;
    SIZE    mbfsz;
    VP  mbf;
}T_CMBF;

typedef struct t_rmbf{
    ID  stskid;
    ID  rtskid;
    UINT    smsgcnt;
    SIZE    fmbfsz;
}T_RMBF;

typedef struct t_cpor{
    ATR poratr;
    UINT    maxcmsz;
    UINT    maxrmsz;
}T_CPOR;

typedef struct t_rpor{
    ID  ctskid;
    ID  atskid;
}T_RPOR;

typedef struct t_rrdv{
    ID  wtskid;
}T_RRDV;

typedef struct t_cmpl{
    ATR mplatr;
    SIZE    mplsz;
    VP  mpl;
    SIZE    blksz;
}T_CMPL;

typedef struct t_rmpl{
    ID  wtskid;
    SIZE    fmplsz;
    UINT    fblksz;
}T_RMPL;

typedef struct t_cmpf{
    ATR mpfatr;
    UINT    blkcnt;
    UINT    blksz;
    VP  mpf;
}T_CMPF;

typedef struct t_rmpf{
    ID  wtskid;
    UINT    fblkcnt;
}T_RMPF;

typedef struct t_ccyc{
    ATR cycatr;
    VP_INT  exinf;
    FP  cychdr;
    RELTIM  cyctim;
    RELTIM  cycphs;
}T_CCYC;

typedef struct t_rcyc{
    STAT    cycstat;
    RELTIM  lefttim;
}T_RCYC;

typedef struct t_calm{
    ATR almatr;
    VP_INT  exinf;
    FP  almhdr;
}T_CALM;

typedef struct t_ralm{
    STAT    almstat;
    RELTIM  lefttim;
}T_RALM;

typedef struct t_dovr{
    ATR ovratr;
    FP  ovrhdr;
}T_DOVR;

typedef struct t_rovr{
    STAT    ovrstat;
    OVRTIM  leftotm;
}T_ROVR;

typedef struct t_dinh{
    ATR inhatr;
    FP  inthdr;
}T_DINH;

typedef struct t_cisr{
    ATR isratr;
    VP_INT  exinf;
    INTNO   intno;
    FP  isr;
}T_CISR;

typedef struct t_risr{
    INT dummy;
}T_RISR;

typedef struct t_dsvc{
    ATR svcatr;
    FP  svcrtn;
}T_DSVC;

typedef struct t_dexc{
    ATR excatr;
    FP  exchdr;
}T_DEXC;

typedef struct t_rcfg{
    PRI max_tpri;
    PRI max_mpri;
    ID  max_tskid;
    ID  max_semid;
    ID  max_flgid;
    ID  max_dtqid;
    ID  max_mbxid;
    ID  max_mtxid;
    ID  max_mbfid;
    ID  max_mpfid;
    ID  max_mplid;
    ID  max_cycid;
    ID  max_almid;
    _KERNEL_RCFG_IMPL
}T_RCFG;

typedef struct t_rver{
    UH  maker;
    UH  prid;
    UH  spver;
    UH  prver;
    UH  prno[4];
}T_RVER;

typedef struct t_rsys{
    STAT    sysstat;
}T_RSYS;

typedef struct vt_rshp{
    UINT    maxfreesz;
}VT_RSHP;

/************************************************/
/* System Call Function Define                  */
/************************************************/
ER cre_tsk(ID, T_CTSK*);
ER_ID acre_tsk(T_CTSK*);
ER del_tsk(ID);
ER sta_tsk(ID, VP_INT);
ER act_tsk(ID);
ER iact_tsk(ID);
ER_UINT can_act(ID);
void ext_tsk(void);
void exd_tsk(void);
ER ter_tsk(ID);
ER chg_pri(ID, PRI);
ER get_pri(ID, PRI*);
ER ref_tsk(ID, T_RTSK*);
ER ref_tst(ID, T_RTST*);

ER sus_tsk(ID);
ER rsm_tsk(ID);
ER frsm_tsk(ID);
ER slp_tsk(void);
ER tslp_tsk(TMO);
ER_UINT can_wup(ID);
ER dly_tsk(RELTIM);
ER rel_wai(ID);
ER irel_wai(ID);
ER wup_tsk(ID);
ER iwup_tsk(ID);

ER def_tex(ID, T_DTEX*);
ER ras_tex(ID, TEXPTN);
ER iras_tex(ID, TEXPTN);
ER dis_tex(void);
ER ena_tex(void);
BOOL sns_tex(void);
ER ref_tex(ID, T_RTEX*);

ER cre_sem(ID, T_CSEM*);
ER_ID acre_sem(T_CSEM*);
ER del_sem(ID);
ER sig_sem(ID);
ER wai_sem(ID);
ER pol_sem(ID);
ER twai_sem(ID, TMO);
ER ref_sem(ID, T_RSEM*);
ER cre_flg(ID, T_CFLG*);
ER_ID acre_flg(T_CFLG*);
ER del_flg(ID);
ER set_flg(ID, FLGPTN);
ER clr_flg(ID, FLGPTN);
ER wai_flg(ID, FLGPTN, MODE, FLGPTN*);
ER pol_flg(ID, FLGPTN, MODE, FLGPTN*);
ER twai_flg(ID, FLGPTN, MODE, FLGPTN*, TMO);
ER ref_flg(ID, T_RFLG*);
ER cre_dtq(ID, T_CDTQ*);
ER_ID acre_dtq(T_CDTQ*);
ER del_dtq(ID);
ER snd_dtq(ID, VP_INT);
ER psnd_dtq(ID, VP_INT);
ER ipsnd_dtq(ID, VP_INT);
ER tsnd_dtq(ID, VP_INT, TMO);
ER fsnd_dtq(ID, VP_INT);
ER ifsnd_dtq(ID, VP_INT);
ER rcv_dtq(ID, VP_INT*);
ER prcv_dtq(ID, VP_INT*);
ER trcv_dtq(ID, VP_INT*, TMO);
ER ref_dtq(ID, T_RDTQ*);
ER cre_mbx(ID, T_CMBX*);
ER_ID acre_mbx(T_CMBX*);
ER del_mbx(ID);
ER snd_mbx(ID, T_MSG*);
ER rcv_mbx(ID, T_MSG**);
ER prcv_mbx(ID, T_MSG**);
ER trcv_mbx(ID, T_MSG**, TMO);
ER ref_mbx(ID, T_RMBX*);

ER cre_mtx(ID, T_CMTX*);
ER_ID acre_mtx(T_CMTX*);
ER del_mtx(ID);
ER loc_mtx(ID);
ER ploc_mtx(ID);
ER tloc_mtx(ID, TMO);
ER unl_mtx(ID);
ER ref_mtx(ID, T_RMTX*);
ER cre_mbf(ID, T_CMBF*);
ER_ID acre_mbf(T_CMBF*);
ER del_mbf(ID);
ER snd_mbf(ID, VP, UINT);
ER psnd_mbf(ID, VP, UINT);
ER tsnd_mbf(ID, VP, UINT, TMO);
ER_UINT rcv_mbf(ID, VP);
ER_UINT prcv_mbf(ID, VP);
ER_UINT trcv_mbf(ID, VP, TMO);
ER ref_mbf(ID, T_RMBF*);
ER cre_por(ID, T_CPOR*);
ER_ID acre_por(T_CPOR*);
ER del_por(ID);
ER_UINT cal_por(ID, RDVPTN, VP, UINT);
ER_UINT tcal_por(ID, RDVPTN, VP, UINT, TMO);
ER_UINT acp_por(ID, RDVPTN, RDVNO*, VP);
ER_UINT pacp_por(ID, RDVPTN, RDVNO*, VP);
ER_UINT tacp_por(ID, RDVPTN, RDVNO*, VP, TMO);
ER fwd_por(ID, RDVPTN, RDVNO, VP, UINT);
ER rpl_rdv(RDVNO, VP, UINT);
ER ref_por(ID, T_RPOR*);
ER ref_rdv(RDVNO, T_RRDV*);

ER cre_mpf(ID, T_CMPF*);
ER_ID acre_mpf(T_CMPF*);
ER del_mpf(ID);
ER get_mpf(ID, VP*);
ER pget_mpf(ID, VP*);
ER tget_mpf(ID, VP*, TMO);
ER rel_mpf(ID, VP);
ER ref_mpf(ID, T_RMPF*);
ER cre_mpl(ID, T_CMPL*);
ER_ID acre_mpl(T_CMPL*);
ER del_mpl(ID);
ER get_mpl(ID, UINT, VP*);
ER pget_mpl(ID, UINT, VP*);
ER tget_mpl(ID, UINT, VP*, TMO);
ER rel_mpl(ID, VP);
ER ref_mpl(ID, T_RMPL*);

ER get_tim(SYSTIM*);
ER set_tim(SYSTIM*);
ER isig_tim(void);
ER cre_cyc(ID, T_CCYC *);
ER_ID acre_cyc(T_CCYC *);
ER del_cyc(ID);
ER sta_cyc(ID);
ER stp_cyc(ID);
ER ref_cyc(ID, T_RCYC*);
ER cre_alm(ID, T_CALM *);
ER_ID acre_alm(T_CALM *);
ER del_alm(ID);
ER sta_alm(ID, RELTIM);
ER stp_alm(ID);
ER ref_alm(ID, T_RALM *);
ER def_ovr(T_DOVR*);
ER sta_ovr(ID, OVRTIM);
ER stp_ovr(ID);
ER ref_ovr(ID, T_ROVR*);

ER dis_dsp(void);
ER ena_dsp(void);
ER get_tid(ID*);
ER iget_tid(ID*);
ER loc_cpu(void);
ER iloc_cpu(void);
ER unl_cpu(void);
ER iunl_cpu(void);
ER rot_rdq(PRI);
BOOL sns_ctx(void);
BOOL sns_dsp(void);
BOOL sns_dpn(void);
BOOL sns_loc(void);
ER ref_sys(T_RSYS*);

ER def_inh(INHNO, T_DINH*);
ER cre_isr(ID, T_CISR*);
ER_ID acre_isr(T_CISR*);
ER del_isr(ID);
ER ref_isr(ID, T_RISR*);
ER dis_int(INTNO);
ER ena_int(INTNO);

ER def_svc(FN, T_DSVC*);
ER_UINT cal_svc(FN, VP_INT, VP_INT,...);

ER def_exc(EXCNO, T_DEXC*);
ER ref_cfg(T_RCFG*);
ER ref_ver(T_RVER*);

ER_UINT vdmp_sys(VP, UINT, UW, VP);

ER vcre_shp(UINT, VP);
ER vdel_shp(void);
ER valc_shp(UINT, VP*);
ER vfre_shp(VP);
ER vref_shp(VT_RSHP*);

/************************************************/
/* Kernel Module Define                         */
/************************************************/
/************************************************/
/* Kernel service call function (iXXX_YYY)      */
/************************************************/
#define iact_tsk(tskid) act_tsk(tskid)
#define irel_wai(tskid) rel_wai(tskid)
#define iwup_tsk(tskid) wup_tsk(tskid)
#define iras_tex(tskid,rasptn) ras_tex(tskid,rasptn)
#define isig_sem(semid) sig_sem(semid)
#define iset_flg(flgid,setptn) set_flg(flgid,setptn)
#define ipsnd_dtq(dtqid,data) psnd_dtq(dtqid,data)
#define ifsnd_dtq(dtqid,data) fsnd_dtq(dtqid,data)
#define iget_tid(p_tskid) get_tid(p_tskid)
#define iloc_cpu() loc_cpu()
#define iunl_cpu() unl_cpu()
#define irot_rdq(tskpri) rot_rdq(tskpri)

/************************************************/
/* TASK CONTROLLER                              */
/************************************************/
/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
/* Data type */
typedef INT32       OS_USER_ER;         /**< Define type of "ER" (Type of OS function's return value) */
typedef SHORT       OS_USER_ID;         /**< Define type of "ID" (Type of OS function's argument) */
typedef INT32       OS_USER_TMO;        /**< Define type of "TMO" (Type of OS function's argument) */
typedef UINT32      OS_USER_RELTIM;     /**< Define type of "RELTIM" (Type of OS function's argument) */
typedef ULLONG      OS_USER_SYSTIM;     /**< Define type of "SYSTIM" (Type of OS function's argument) */
typedef ULONG       OS_USER_FLGPTN;     /**< Define type of "FLGPTN" (Type of OS function's argument) */
typedef USHORT      OS_USER_MODE;       /**< Define type of "MODE" (Type of OS function's argument) */
typedef INT32       OS_USER_VP_INT;     /**< Define type of "VP_INT" (Type of OS function's argument) */
typedef INT32       OS_USER_ER_ID;      /**< Define type of "ER_ID" (Type of OS function's argument) */
typedef USHORT      OS_USER_ATR;        /**< Define type of "ATR" (Type of OS function's argument) */
typedef SHORT       OS_USER_PRI;        /**< Define type of "PRI" (Type of OS function's argument) */
typedef VOID        (*OS_USER_FP)(VOID);    /**< Define type of "FP" (Type of OS function's argument) */
typedef VOID*       OS_USER_VP;         /**< Define type of "VP" (Type of OS function's argument) */
typedef UINT32      OS_USER_SIZE;       /**< Define type of "SIZE" (Type of OS function's argument) */
typedef CHAR        OS_USER_VB;         /**< Define type of "VG" (Type of OS function's argument) */
typedef USHORT      OS_USER_STAT;       /**< Define type of "STAT" (Type of OS function's argument) */
typedef OS_USER_ER  OS_USER_ER_UINT;    /**< Define type of "ER_UINT" (Type of OS function's argument) */

/* Error code */
#define D_OS_USER_E_OK          (0)             /**< Normal End (E_OK) */
#define D_OS_USER_E_SYS         (-5)            /**< System Error (E_SYS) */
#define D_OS_USER_E_NOSPT       (-9)            /**< No Support Function (E_NOSPT) */
#define D_OS_USER_E_RSFN        (-10)           /**< Reserve Function Code (E_RSFN) */
#define D_OS_USER_E_RSATR       (-11)           /**< Reserve Attribute (E_RSATR) */
#define D_OS_USER_E_PAR         (-17)           /**< Parameter Error (E_PAR) */
#define D_OS_USER_E_ID          (-18)           /**< ID Number Error (E_ID) */
#define D_OS_USER_E_CTX         (-25)           /**< Context Error (E_CTX) */
#define D_OS_USER_E_MACV        (-26)           /**< Memory Access Violation (E_MACV) */
#define D_OS_USER_E_OACV        (-27)           /**< Object Access Violation (E_OACV) */
#define D_OS_USER_E_ILUSE       (-28)           /**< Service Call Illegal Use (E_ILUSE) */
#define D_OS_USER_E_NOMEM       (-33)           /**< No Memory (E_NOMEM) */
#define D_OS_USER_E_NOID        (-34)           /**< No ID Number (E_NOID) */
#define D_OS_USER_E_OBJ         (-41)           /**< Object State Error (E_OBJ) */
#define D_OS_USER_E_NOEXS       (-42)           /**< Object Not Exist (E_NOEXS) */
#define D_OS_USER_E_QOVR        (-43)           /**< Queueing Overflow (E_QOVR) */
#define D_OS_USER_E_RLWAI       (-49)           /**< Release Wait Mode (E_RLWAI) */
#define D_OS_USER_E_TMOUT       (-50)           /**< Polling Error or Timeout (E_TMOUT) */
#define D_OS_USER_E_DLT         (-51)           /**< Delete Wait Object (E_DLT) */
#define D_OS_USER_E_CLS         (-52)           /**< Change Wait Object State (E_CLS) */
#define D_OS_USER_E_WBLK        (-57)           /**< Get Non Blocking (E_WBLK) */
#define D_OS_USER_E_BOVR        (-58)           /**< Buffer Overflow (E_BOVR) */

/* Wait queue */
#define D_OS_USER_TA_TFIFO      (0x00)          /**< Queuing is the order of FIFO (TA_TFIFO) */
#define D_OS_USER_TA_TPRI       (0x01)          /**< Queuing is the order of task priority (TA_TPRI) */

/* Task */
#define D_OS_USER_TSK_SELF      (0x00)          /**< Invoking task (TSK_SELF) */
#define D_OS_USER_TSK_NONE      (0x00)          /**< No queued task (TSK_NONE) */
#define D_OS_USER_TPRI_INI      (0x00)          /**< Initial task priority (TPRI_INI) */

/* Object creation */
#define D_OS_USER_TA_HLNG       (0x00)          /**< Handler is written by the C language (TA_HLNG) */
#define D_OS_USER_TA_ASM        (0x01)          /**< Handler is written by the assembler (TA_ASM) */
#define D_OS_USER_TA_ACT        (0x02)          /**< Handler is started after handler generation (TA_ACT) */

/* Task state */
#define D_OS_USER_TTS_RUN       (0x00000001)    /**< RUNNING (TTS_RUN) */
#define D_OS_USER_TTS_RDY       (0x00000002)    /**< READY (TTS_RDY) */
#define D_OS_USER_TTS_WAI       (0x00000004)    /**< WAITING (TTS_WAI) */
#define D_OS_USER_TTS_SUS       (0x00000008)    /**< SUSPENDED (TTS_SUS) */
#define D_OS_USER_TTS_WAS       (0x0000000c)    /**< WAITING-SUSPENDED (TTS_WAS) */
#define D_OS_USER_TTS_DMT       (0x00000010)    /**< DORMANT (TTS_DMT) */

/* Event flag wait modes */
#define D_OS_USER_TWF_ANDW      (0x00000000)    /**< Bit pattern AND wait (TWF_ANDW)*/
#define D_OS_USER_TWF_ORW       (0x00000001)    /**< Bit pattern OR wait (TWF_ORW) */

/* Mutex attribute */
#define D_OS_USER_TA_INHERIT    (0x02)          /**< Protocol for inherited priority (TA_INHERIT) */
#define D_OS_USER_TA_CEILING    (0x03)          /**< Protocol for ceiling priority (TA_CEILING) */

/* Timeout value */
#define D_OS_USER_TMO_FEVR      (-1)            /**< Forever Wait (TMO_FEVR) */
#define D_OS_USER_TMO_POL       (0)             /**< Wait by Polling (TMO_POL) */

/* Task waiting state */
#define D_OS_USER_TTW_SLP       (0x00000001)    /**< Waiting on slp_tsk (TTW_SLP) */
#define D_OS_USER_TTW_DLY       (0x00000002)    /**< Waiting on dly_tsk (TTW_DLY) */
#define D_OS_USER_TTW_SEM       (0x00000004)    /**< Waiting on wai_sem (TTW_SEM) */
#define D_OS_USER_TTW_FLG       (0x00000008)    /**< Waiting on wai_flg (TTW_FLG) */
#define D_OS_USER_TTW_SDTQ      (0x00000010)    /**< Waiting on snd_dtq (TTW_SDTQ) */
#define D_OS_USER_TTW_RDTQ      (0x00000020)    /**< Waiting on rcv_dtq (TTW_RDTQ) */
#define D_OS_USER_TTW_MBX       (0x00000040)    /**< Waiting on rcv_mbx (TTW_MBX) */
#define D_OS_USER_TTW_MTX       (0x00000080)    /**< Waiting on loc_mtx (TTW_MTX) */
#define D_OS_USER_TTW_SMBF      (0x00000100)    /**< Waiting on snd_mbf (TTW_SMBF) */
#define D_OS_USER_TTW_RMBF      (0x00000200)    /**< Waiting on rcv_mbf (TTW_RMBF) */
#define D_OS_USER_TTW_CAL       (0x00000400)    /**< Waiting on a rendezvous call (TTW_CAL) */
#define D_OS_USER_TTW_ACP       (0x00000800)    /**< Waiting on a rendezvous acceptance (TTW_ACP) */
#define D_OS_USER_TTW_RDV       (0x00001000)    /**< Waiting on a rendezvous termination (TTW_RDV) */
#define D_OS_USER_TTW_MPF       (0x00002000)    /**< Waiting on get_mpf (TTW_MPF) */
#define D_OS_USER_TTW_MPL       (0x00004000)    /**< Waiting on get_mpl (TTW_MPL) */

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
typedef struct t_os_user_context
{
//-----------------------------------------------------------------------------
// Main routine does NOT use SPSR.
// Interrupt routine keeps SPSR at top of interrupt entrance.
//  TC_UINT spsr;
//-----------------------------------------------------------------------------
    UINT32 cpsr;
    UINT32 r8;                  /* v5 */
    UINT32 r9;                  /* v6, sb */
    UINT32 r10;                 /* v7, sl */
    UINT32 r11;                 /* v8, fp */
//-----------------------------------------------------------------------------
// Main routine does NOT use them.
// Interrupt routine keeps them at top of interrupt entrance.
//  UINT32 r1;                  /* a2 */
//  UINT32 r2;                  /* a3 */
//  UINT32 r3;                  /* a4 */
//  UINT32 r12;                 /* ip */
//-----------------------------------------------------------------------------
    UINT32 argreg;              /* r0, a1 */
    UINT32 r4;                  /* v1 */
    UINT32 r5;                  /* v2 */
    UINT32 r6;                  /* v3 */
    UINT32 r7;                  /* v4, wr */
    OS_USER_FP task;            /* procedure, r14, lr, r15, pc */
} T_OS_USER_CONTEXT;

/** Semaphore State Information */
typedef struct t_os_user_rsem {
    OS_USER_ID      wtskid;         /**< Top Task ID of Semaphore Wait Matrix */
    UINT32          semcnt;         /**< Semaphore Resource Count */
} T_OS_USER_RSEM;

/** Mailbox Message Information */
typedef struct t_os_user_msg {
    struct t_os_user_msg    *hmsg;  /**< Mailbox Message Information */
} T_OS_USER_MSG;

/** Eventflag State Information*/
typedef struct t_os_user_rflg {
    OS_USER_ID      wtskid;         /**< Top Task ID of event flag ID. */
    OS_USER_FLGPTN  flgptn;         /**< Bit pattern setting.*/
} T_OS_USER_RFLG;

/** Task Create Information */
typedef struct t_os_user_ctsk {
    OS_USER_ATR     tskatr;         /**< Task Attributes */
    OS_USER_VP_INT  exinf;          /**< Extended Information */
    OS_USER_FP      task;           /**< Task Start Address */
    OS_USER_PRI     itskpri;        /**< Initial Task Priority */
    OS_USER_SIZE    stksz;          /**< Stack Size */
    OS_USER_VP      stk;            /**< Stack Region Top Address */
} T_OS_USER_CTSK;

/** Semaphore Create Information */
typedef struct t_os_user_csem {
    OS_USER_ATR     sematr;         /**< Semaphore Attribute */
    UINT32          isemcnt;        /**< Initial Value of Semaphore Resource Count */
    UINT32          maxsem;         /**< Max Semaphore Resource Count */
} T_OS_USER_CSEM;

/** Cyclic Create Information */
typedef struct t_os_user_ccyc {
    OS_USER_ATR     cycatr;         /**< Cyclic Handler Attribute */
    OS_USER_VP_INT  exinf;          /**< Extended Information */
    OS_USER_FP      cychdr;         /**< Pointer of Cyclic Handler Function */
    OS_USER_RELTIM  cyctim;         /**< Cyclic Start Time Interval */
    OS_USER_RELTIM  cycphs;         /**< Cyclic Handler Start Phase */
} T_OS_USER_CCYC;

/** Task State Information */
typedef struct t_os_user_rtsk {
    OS_USER_STAT    tskstat;        /**< Task State */
    OS_USER_PRI     tskpri;         /**< Task Current Priority */
    OS_USER_PRI     tskbpri;        /**< Task Base Priority */
    OS_USER_STAT    tskwait;        /**< Task Wait Factors */
    OS_USER_ID      wobjid;         /**< Waiting Target ID Number */
    OS_USER_TMO     lefttmo;        /**< Time Out Period */
    UINT32          actcnt;         /**< Activation Request Queue Count */
    UINT32          wupcnt;         /**< Number of Queued Wakeup Request */
    UINT32          suscnt;         /**< Number of Nested Suspend Request */
    OS_USER_VP      stk;            /**< StackPointer */
    OS_USER_VP      stktop;         /**< StackTopAddress */
} T_OS_USER_RTSK;

/** Task Status Information */
typedef struct t_os_user_rtst {
    OS_USER_STAT    tskstat;        /**< Task State */
    OS_USER_STAT    tskwait;        /**< Task Waiting State */
} T_OS_USER_RTST;

/** System Status Information */
typedef struct t_os_user_rsys {
    OS_USER_STAT    sysstat;        /**< System State */
} T_OS_USER_RSYS;

/** Mailbox Status Information */
typedef struct t_os_user_rmbx {
    OS_USER_ID      wtskid;         /**< Wait Task Information */
    T_OS_USER_MSG   *pk_msg;        /**< Packet of Message */
} T_OS_USER_RMBX;

/** Fixed-size Memory Pool Information */
typedef struct t_os_user_rmpf {
    OS_USER_ID      wtskid;         /**< Wait Task Information */
    UINT32          fblkcnt;        /**< Free Block Count */
} T_OS_USER_RMPF;

/** Variable-size Memory Pool Information */
typedef struct t_os_user_rmpl {
    OS_USER_ID      wtskid;         /**< Wait Task Information */
    OS_USER_SIZE    fmplsz;         /**< Free Memory Size */
    OS_USER_SIZE    fblksz;         /**< Free Memory Block Size */
} T_OS_USER_RMPL;

/** Create Mutex Information */
typedef struct t_os_user_cmtx {
    OS_USER_ATR     mtxatr;         /**< Mutex Attribute */
    OS_USER_PRI     ceilpri;        /**< Ceiling Priority */
} T_OS_USER_CMTX;

/** Mutex Status Information */
typedef struct t_os_user_rmtx {
    OS_USER_ID      htskid;         /**< Halt Task Information */
    OS_USER_ID      wtskid;         /**< Wait Task Information */
} T_OS_USER_RMTX;

/** Alarm Status Information */
typedef struct t_os_user_ralm {
    OS_USER_STAT    almstat;        /**< Alarm Handler Status */
    OS_USER_RELTIM  lefttim;        /**< Left Time */
} T_OS_USER_RALM;

/** Alarm Create Information */
typedef struct t_os_user_calm {
    OS_USER_ATR     almatr;         /**< Alarm Handler Attribute */
    OS_USER_VP_INT  exinf;          /**< Extended Information */
    OS_USER_FP      almhdr;         /**< Alarm Handler Address */
} T_OS_USER_CALM;

/** Refer Configuration Information */
typedef struct t_os_user_rcfg {
    OS_USER_PRI     max_tpri;       /**< Taks priority maximum value */
    OS_USER_PRI     max_mpri;       /**< Message priority maximum value */
    OS_USER_ID      max_tskid;      /**< Task ID maximum valuee */
    OS_USER_ID      max_semid;      /**< Semaphore ID maximum value */
    OS_USER_ID      max_flgid;      /**< Eventflag ID maximum value */
    OS_USER_ID      max_dtqid;      /**< Data queue ID maximum value */
    OS_USER_ID      max_mbxid;      /**< Mailbox ID maximum value */
    OS_USER_ID      max_mtxid;      /**< Mutex ID maximum value */
    OS_USER_ID      max_mbfid;      /**< Message buffer ID maximum value */
    OS_USER_ID      max_mpfid;      /**< Fixed-sized memory pool ID maximum value */
    OS_USER_ID      max_mplid;      /**< Variable-sized memory pool ID maximum value */
    OS_USER_ID      max_cycid;      /**< Cyclic handler ID maximum value */
    OS_USER_ID      max_almid;      /**< Alarm ID maximum value */
} T_OS_USER_RCFG;

/** Message Priority Information */
typedef struct t_os_user_msg_pri {
    T_OS_USER_MSG   msgque;         /**< OS reserved area for message queues */
    OS_USER_PRI     msgpri;         /**< Message priority */
} T_OS_USER_MSG_PRI;

/** Message Buffer Information */
typedef struct t_os_user_rmbf {
    OS_USER_ID      stskid;         /**< Send Wait Task ID */
    OS_USER_ID      rtskid;         /**< Receive Wait Task ID */
    UINT32          smsgcnt;        /**< Number of Messages */
    OS_USER_SIZE    fmbfsz;         /**< Free Message Buffer Size */
} T_OS_USER_RMBF;

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function                                                             */
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


#if 0
/**
This function is wrapper function of acre_tsk().<br>
It creates task.<br>
Task ID is assigned automatically.<br>
@param [in]     pk_ctsk             : Pointer of task create structure
@retval PositiveValue               : Task ID
@retval D_OS_USER_E_PAR             : Priority range error
@retval D_OS_USER_E_NOID            : Task ID lack
*/
OS_USER_ER_ID OS_User_Acre_Tsk(T_OS_USER_CTSK* pk_ctsk);

/**
This function is wrapper function of del_tsk().<br>
It deletes task.<br>
@param [in]     taskid              : Task ID
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered task
@retval D_OS_USER_E_OBJ             : Object state error
*/
OS_USER_ER OS_User_Del_Tsk(OS_USER_ID taskid);
#endif

/**
This function is wrapper function of sta_tsk().<br>
It starts task.<br>
@param [in]     taskid              : Task ID
@param [in]     stacd               : Start code
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_OBJ             : Object state error
@retval D_OS_USER_E_NOEXS           : Unregistered task
*/
OS_USER_ER OS_User_Sta_Tsk(OS_USER_ID taskid, OS_USER_VP_INT stacd);

/**
This function is wrapper function of ext_tsk().<br>
It ends self task.<br>
*/
VOID OS_User_Ext_Tsk(VOID);

/**
This function is wrapper function of ter_tsk().<br>
It end the task.<br>
@param [in]     taskid              : Task ID
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_ILUSE           : Illegal use of service call
@retval D_OS_USER_E_OBJ             : Object state error
@retval D_OS_USER_E_NOEXS           : Unregistered task
*/
OS_USER_ER OS_User_Ter_Tsk(OS_USER_ID taskid);

/**
This function is wrapper function of chg_pri().<br>
It changes task priority.<br>
@param [in]     tskid               : Task ID
@param [in]     tskpri              : Task priority
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_ILUSE           : Unauthorized use of service call
@retval D_OS_USER_E_OBJ             : Object status is error
@retval D_OS_USER_E_NOEXS           : Objects not created
*/
OS_USER_ER OS_User_Chg_Pri(OS_USER_ID taskid, OS_USER_PRI pri);

/**
This function is wrapper function of get_pri().<br>
It gets task priority.<br>
@param [in]     tskid               : Task ID
@param [in]     p_tskpri            : Pointer of task priority
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_OBJ             : Object status is error
@retval D_OS_USER_E_NOEXS           : Objects not created
*/
OS_USER_ER OS_User_Get_Pri(OS_USER_ID tskid, OS_USER_PRI* p_tskpri);

/**
This function is wrapper function of ref_tsk().<br>
It gets task state information.<br>
@param [in]     tskid               : Task ID
@param [out]    pk_rtsk             : Pointer of task state information structure
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered task
@retval D_OS_USER_E_PAR             : Parameter error
*/
OS_USER_ER OS_User_Ref_Tsk(OS_USER_ID tskid, T_OS_USER_RTSK* pk_rtsk);

#if 0
/**
This function is wrapper function of ref_tst().<br>
It gets task state information (simplified version).<br>
@param [in]     tskid               : Task ID
@param [out]    pk_rtst             : Packet to Refer TaskStatus
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered task
*/
OS_USER_ER OS_User_Ref_Tst(OS_USER_ID tskid, T_OS_USER_RTST* pk_rtst);
#endif

OS_USER_ER OS_User_Sus_Tsk(OS_USER_ID tskid);

OS_USER_ER OS_User_Rsm_Tsk(OS_USER_ID tskid);

/**
This function is wrapper function of slp_tsk().<br>
It sleeps task.<br>
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_CTX             : Context error
@retval D_OS_USER_E_RLWAI           : Release wait mode
*/
OS_USER_ER OS_User_Slp_Tsk(VOID);

/**
This function is wrapper function of tslp_tsk().<br>
It sleeps(with timeout) task.<br>
@param [in]     timeout             : time out value
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_CTX             : Context error
@retval D_OS_USER_E_ILUSE           : Illegal use of service call
@retval D_OS_USER_E_RLWAI           : Release wait mode
@retval D_OS_USER_E_TMOUT           : Time out
*/
OS_USER_ER OS_User_Tslp_Tsk(OS_USER_TMO timeout);

/**
This function is wrapper function of dly_tsk().<br>
It delays task. <br>
@param [in]     delaytime           : Delay time
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_CTX             : Context error
@retval D_OS_USER_E_ILUSE           : Illegal use of service call
@retval D_OS_USER_E_RLWAI           : Release wait mode
*/
OS_USER_ER OS_User_Dly_Tsk(OS_USER_RELTIM delaytime);

/**
This function is wrapper function of wup_tsk().<br>
It wake-up the task.<br>
@param [in]     taskid              : Task ID
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_OBJ             : Object state error
@retval D_OS_USER_E_NOEXS           : Unregistered task
@retval D_OS_USER_E_QOVR            : Queuing Overflow
*/
OS_USER_ER OS_User_Wup_Tsk(OS_USER_ID taskid);

#if 0
/**
This function is wrapper function of acre_sem().<br>
It creates semaphore.<br>
Semaphore ID is assigned automatically.<br>
@param [in]     pk_csem             : Pointer of Semaphore create information structure
@retval PositiveValue               : Semaphore ID
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_NOID            : Semaphore ID lack
*/
OS_USER_ER_ID OS_User_Acre_Sem(T_OS_USER_CSEM* pk_csem);

/**
This function is wrapper function of del_sem().<br>
It deletes semaphore.<br>
@param [in]     semid               : Semaphore ID
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered semaphore
*/
OS_USER_ER OS_User_Del_Sem(OS_USER_ID semid);
#endif

/**
This function is wrapper function of sig_sem().<br>
It returns resource.<br>
@param [in]     semid               : Semaphore ID
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered semaphore
@retval D_OS_USER_E_QOVR            : Semaphore count overflow
*/
OS_USER_ER OS_User_Sig_Sem(OS_USER_ID semid);

/**
This function is wrapper function of wai_sem().<br>
It gets semaphore resource.<br>
@param [in]     semid               : Semaphore ID
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered semaphore
@retval D_OS_USER_E_CTX             : Non task context
@retval D_OS_USER_E_RLWAI           : Release wait mode
@retval D_OS_USER_E_DLT             : Delete semaphore
*/
OS_USER_ER OS_User_Wai_Sem(OS_USER_ID semid);

/**
This function is wrapper function of pol_sem().<br>
It gets semaphore resource by polling.<br>
@param [in]     semid               : Semaphore ID
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered semaphore
@retval D_OS_USER_E_TMOUT           : Polling error
*/
OS_USER_ER OS_User_Pol_Sem(OS_USER_ID semid);

/**
This function is wrapper function of twai_sem().<br>
It gets(with timeout) semaphore resource.<br>
@param [in]     semid               : Semaphore ID
@param [in]     tmout               : Time out value
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered semaphore
@retval D_OS_USER_E_CTX             : Non task context
@retval D_OS_USER_E_RLWAI           : Release wait mode
@retval D_OS_USER_E_DLT             : Delete semaphore
@retval D_OS_USER_E_TMOUT           : Time out
*/
OS_USER_ER OS_User_Twai_Sem(OS_USER_ID semid, OS_USER_TMO tmout);

/**
This function is wrapper function of ref_sem().<br>
It gets semaphore state information.<br>
@param [in]     semid               : Semaphore ID
@param [out]    pk_rsm              : Pointer of semaphore state information structure
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered semaphore
@retval D_OS_USER_E_QOVR            : Semaphore count overflow
*/
OS_USER_ER OS_User_Ref_Sem(OS_USER_ID semid, T_OS_USER_RSEM* pk_rsm);

/**
This function is wrapper function of set_flg().<br>
It sets(with timeout) event flag.<br>
@param [in]     flagid              : Event flag ID
@param [in]     setptn              : Bit pattern setting
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered event flag
*/
OS_USER_ER OS_User_Set_Flg(OS_USER_ID flagid, OS_USER_FLGPTN setptn);

/**
This function is wrapper function of clr_flg().<br>
It sets(with timeout) event flag.<br>
@param [in]     flagid              : Event flag ID
@param [in]     clrptn              : Clear Bit pattern
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered event flag
*/
OS_USER_ER OS_User_Clr_Flg(OS_USER_ID flagid, OS_USER_FLGPTN clrptn);

/**
This function is wrapper function of wai_flg().<br>
It waits event flag.<br>
@param [in]     flagid              : Event flag ID
@param [in]     waiptn              : Wait bit pattern
@param [in]     wfmode              : Wait mode
@param [out]    p_flgptn            : Pointer of bit pattern at the time of release
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : Event flag ID range error
@retval D_OS_USER_E_NOEXS           : Unregistered event flag
@retval D_OS_USER_E_ILUSE           : Already wait task
@retval D_OS_USER_E_CTX             : Non task context
@retval D_OS_USER_E_RLWAI           : Release wait mode
@retval D_OS_USER_E_DLT             : Delete waiting event flag
*/
OS_USER_ER OS_User_Wai_Flg(OS_USER_ID flagid, OS_USER_FLGPTN waiptn, OS_USER_MODE wfmode, OS_USER_FLGPTN* p_flgptn);

/**
This function is wrapper function of pol_flg().<br>
Polls eventflag wait cancel conditions.<br>
@param [in]     flagid              : Event flag ID
@param [in]     waiptn              : Wait bit pattern
@param [in]     wfmode              : Wait mode
@param [out]    p_flgptn            : Pointer of bit pattern at the time of release
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : Event flag ID range error
@retval D_OS_USER_E_ILUSE           : Already wait task
@retval D_OS_USER_E_NOEXS           : Unregistered event flag
@retval D_OS_USER_E_TMOUT           : Polling faiure or timeout
*/
OS_USER_ER OS_User_Pol_Flg(OS_USER_ID flagid, OS_USER_FLGPTN waiptn, OS_USER_MODE wfmode, OS_USER_FLGPTN* p_flgptn);

/**
This function is wrapper function of twai_flg().<br>
It waits(with timeout) event flag.<br>
@param [in]     flagid              : Event flag ID
@param [in]     waiptn              : Wait bit pattern
@param [in]     wfmode              : Wait mode
@param [out]    p_flgptn            : Pointer of bit pattern at the time of release
@param [in]     timeout             : Time out value
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : Event flag ID range error
@retval D_OS_USER_E_NOEXS           : Unregistered event flag
@retval D_OS_USER_E_OBJ             : Already wait task
@retval D_OS_USER_E_CTX             : Non task context
@retval D_OS_USER_E_RLWAI           : Release wait mode
@retval D_OS_USER_E_DLT             : Delete waiting event flag
@retval D_OS_USER_E_TMOUT           : Time out
*/
OS_USER_ER OS_User_Twai_Flg(OS_USER_ID flagid, OS_USER_FLGPTN waiptn, OS_USER_MODE wfmode, OS_USER_FLGPTN* p_flgptn, OS_USER_TMO timeout);

/**
This function is wrapper function of ref_flg().<br>
It reference event flag.<br>
@param [in]     flagid              : Event flag ID
@param [in]     pk_rflg             : Event flag state's pointer
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered event flag
*/
OS_USER_ER OS_User_Ref_Flg(OS_USER_ID flagid, T_OS_USER_RFLG* pk_rflg);

/**
This function is wrapper function of snd_mbx().<br>
It sends message to mailbox.<br>
@param [in]     mbxid               : Mailbox ID
@param [out]    pk_msg              : Packet of message structure
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_NOEXS           : Objects not created
*/
OS_USER_ER OS_User_Snd_Mbx(OS_USER_ID mbxid, T_OS_USER_MSG* pk_msg);

/**
This function is wrapper function of rcv_mbx().<br>
Receives messages from mailboxes.<br>
@param [in]     mbxid               : Mailbox ID
@param [out]    ppk_msg             : Pointer of packet of message structure
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_CTX             : Context error
@retval D_OS_USER_E_NOEXS           : Objects not created
@retval D_OS_USER_E_RLWAI           : Releases the waiting state
@retval D_OS_USER_E_DLT             : Deletes waiting objects
*/
OS_USER_ER OS_User_Rcv_Mbx(OS_USER_ID mbxid, T_OS_USER_MSG** ppk_msg);

/**
This function is wrapper function of prcv_mbx().<br>
It polls and receives message from mailbox.<br>
@param [in]     mbxid               : Mailbox ID
@param [out]    ppk_msg             : Pointer of packet of message structure
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_NOEXS           : Objects not created
@retval D_OS_USER_E_TMOUT           : Timeout
*/
OS_USER_ER OS_User_Prcv_Mbx(OS_USER_ID mbxid, T_OS_USER_MSG** ppk_msg);

/**
This function is wrapper function of trcv_mbx().<br>
It receives(with timeout) message from mailbox.<br>
@param [in]     mbxid               : Mailbox ID
@param [out]    ppk_msg             : Pointer of packet of message structure
@param [in]     tmout               : Timeout value
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_CTX             : Context error
@retval D_OS_USER_E_NOEXS           : Objects not created
@retval D_OS_USER_E_RLWAI           : Releases the waiting state
@retval D_OS_USER_E_TMOUT           : Timeout
@retval D_OS_USER_E_DLT             : Deletes waiting objects
*/
OS_USER_ER OS_User_Trcv_Mbx(OS_USER_ID mbxid, T_OS_USER_MSG** ppk_msg, OS_USER_TMO tmout);

/**
This function is wrapper function of ref_mbx().<br>
It refers mailbox status.<br>
@param [in]     mbxid               : Mailbox ID
@param [out]    pk_rmbx             : Packet to refer mailbox structure
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_NOEXS           : Objects not created
*/
OS_USER_ER OS_User_Ref_Mbx(OS_USER_ID mbxid, T_OS_USER_RMBX* pk_rmbx);

#if 0
/**
This function is wrapper function of cre_mtx().<br>
It creates mutex.<br>
@param [in]     mtxid               : Mutex ID
@param [in]     pk_cmtx             : Packet of mutex creation information structure
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_RSATR           : Reserve Attribute
@retval D_OS_USER_E_PAR             : Parameter Errors
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_OBJ             : Object status is error
*/
OS_USER_ER OS_User_Cre_Mtx(OS_USER_ID mtxid, T_OS_USER_CMTX* pk_cmtx);
#endif

/**
This function is wrapper function of loc_mtx().<br>
It locks mutex.<br>
@param [in]     mtxid               : Mutex ID
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_CTX             : Context error
@retval D_OS_USER_E_ILUSE           : Illegal use of service call
@retval D_OS_USER_E_NOEXS           : Objects not created
@retval D_OS_USER_E_RLWAI           : Releases the waiting state
@retval D_OS_USER_E_DLT             : Deletes waiting objects
*/
OS_USER_ER OS_User_Loc_Mtx(OS_USER_ID mtxid);

/**
*/
OS_USER_ER OS_User_Tloc_Mtx(OS_USER_ID mtxid, OS_USER_TMO tmout);

/**
This function is wrapper function of unl_mtx().<br>
It unlocks mutex.<br>
@param [in]     mtxid               : Mutex ID
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_CTX             : Context error
@retval D_OS_USER_E_ILUSE           : Illegal use of service call
@retval D_OS_USER_E_NOEXS           : Objects not created
*/
OS_USER_ER OS_User_Unl_Mtx(OS_USER_ID mtxid);

#if 0
/**
This function is wrapper function of psnd_mbf().<br>
It polls and sends message to message buffer.<br>
@param [in]     mbfid               : Message buffer ID
@param [in]     msg                 : Pointer of message structure
@param [in]     msgsz               : Bytes of message structure
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_NOEXS           : Objects not created
@retval D_OS_USER_E_TMOUT           : Timeout
*/
OS_USER_ER OS_User_Psnd_Mbf(OS_USER_ID mbfid, OS_USER_VP msg, UINT32 msgsz);
#endif

#if 0
/**
This function is wrapper function of prcv_mbf().<br>
It polls and receives message from message buffer.<br>
@param [in]     mbfid               : Mailbox ID
@param [out]    msg                 : Pointer of message structure
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_NOEXS           : Objects not created
@retval D_OS_USER_E_TMOUT           : Timeout
*/
OS_USER_ER_UINT OS_User_Prcv_Mbf(OS_USER_ID mbfid, OS_USER_VP msg);
#endif

#if 0
/**
This function is wrapper function of ref_mbf().<br>
It polls and receives message from message buffer.<br>
@param [in]     mbfid               : Mailbox ID
@param [out]    pk_rmbf             : Pointer of message buffer status
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_NOEXS           : Objects not created
*/
OS_USER_ER OS_User_Ref_Mbf(OS_USER_ID mbfid, T_OS_USER_RMBF* pk_rmbf);
#endif

/**
This function is wrapper function of get_mpf().<br>
It gets fixed-size memory block.<br>
@param [in]     mpfid               : Memory Pool ID
@param [out]    p_blk               : Pointer of block start address
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_CTX             : Context error
@retval D_OS_USER_E_NOEXS           : Objects not created
@retval D_OS_USER_E_RLWAI           : Releases the waiting state
@retval D_OS_USER_E_DLT             : Deletes waiting objects
*/
OS_USER_ER OS_User_Get_Mpf(OS_USER_ID mpfid, OS_USER_VP* p_blk);

OS_USER_ER OS_User_Pget_Mpf(OS_USER_ID mpfid, OS_USER_VP* p_blk);

/**
This function is wrapper function of tget_mpf().<br>
It gets(with timeout) fixed-size memory block.<br>
@param [in]     mpfid               : Memory Pool ID
@param [out]    p_blk               : Pointer of block start address
@param [in]     tmout               : Timeout value
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_CTX             : Context error
@retval D_OS_USER_E_NOEXS           : Objects not created
@retval D_OS_USER_E_RLWAI           : Releases the waiting state
@retval D_OS_USER_E_TMOUT           : Timeout
@retval D_OS_USER_E_DLT             : Deletes waiting objects
*/
OS_USER_ER OS_User_Tget_Mpf(OS_USER_ID mpfid, OS_USER_VP* p_blk, OS_USER_TMO tmout);

/**
This function is wrapper function of rel_mpf().<br>
It releases fixed-size memory block.<br>
@param [in]     mpfid               : Memory Pool ID
@param [in]     blk                 : Block start address
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_NOEXS           : Objects not created
*/
OS_USER_ER OS_User_Rel_Mpf(OS_USER_ID mpfid, OS_USER_VP blk);

/**
This function is wrapper function of ref_mpf().<br>
It refers fixed-size memory pool status.<br>
@param [in]     mpfid               : Memory Pool ID
@param [out]    pk_rmpf             : Packet to refer memory pool structure
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_NOEXS           : Objects not created
*/
OS_USER_ER OS_User_Ref_Mpf(OS_USER_ID mpfid, T_OS_USER_RMPF* pk_rmpf);

/**
This function is wrapper function of get_mpl().<br>
It gets variable-size memory block.<br>
@param [in]     mplid               : Memory Pool ID
@param [in]     blksz               : Memory block size
@param [out]    p_blk               : Pointer of block start address
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_CTX             : Context error
@retval D_OS_USER_E_NOEXS           : Objects not created
@retval D_OS_USER_E_RLWAI           : Releases the waiting state
@retval D_OS_USER_E_DLT             : Deletes waiting objects
*/
OS_USER_ER OS_User_Get_Mpl(OS_USER_ID mplid, OS_USER_SIZE blksz, OS_USER_VP* p_blk);

/**
This function is wrapper function of pget_mpl().<br>
It poll and gets variable-size memory block.<br>
@param [in]     mplid               : Memory Pool ID
@param [in]     blksz               : Memory block size
@param [out]    p_blk               : Pointer of block start address
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_NOEXS           : Objects not created
@retval D_OS_USER_E_TMOUT           : Polling faiure or timeout
*/
OS_USER_ER OS_User_Pget_Mpl(OS_USER_ID mplid, OS_USER_SIZE blksz, OS_USER_VP* p_blk);

/**
This function is wrapper function of rel_mpl().<br>
It releases variable-size memory block.<br>
@param [in]     mplid               : Memory Pool ID
@param [in]     blk                 : Block start address
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_PAR             : Parameter error
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_NOEXS           : Objects not created
*/
OS_USER_ER OS_User_Rel_Mpl(OS_USER_ID mplid, OS_USER_VP blk);

#if 0
/**
This function is wrapper function of ref_mpl().<br>
It refers variable-size memory pool status.<br>
@param [in]     mplid               : Memory Pool ID
@param [out]    pk_rmpl             : Packet to refer memory pool structure
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : Illegal ID number
@retval D_OS_USER_E_NOEXS           : Objects not created
*/
OS_USER_ER OS_User_Ref_Mpl(OS_USER_ID mplid, T_OS_USER_RMPL* pk_rmpl);
#endif

/**
This function is wrapper function of get_tim().<br>
It gets system time.<br>
@param [out]    p_systime           : Pointer of system time
@retval D_OS_USER_E_OK              : Success
*/
OS_USER_ER OS_User_Get_Tim(OS_USER_SYSTIM* p_systime);

/**
This function is wrapper function of isig_tim().<br>
It supplies time tick.<br>
@retval D_OS_USER_E_OK              : Success
*/
OS_USER_ER OS_User_Isig_Tim(VOID);

/**
This function is wrapper function of acre_cyc().<br>
It creates cyclic handler.<br>
Cyclic handler ID is assigned automatically.
@param [in]     pk_ccyc             : Pointer of cyclic handler create information structure
@retval PositiveValue               : Cyclic handler ID
@retval D_OS_USER_E_PAR             : Cyclic handler activity state error
@retval D_OS_USER_E_NOID            : Cyclic handler ID lack
@retval D_OS_USER_E_RSATR           : Reserve Attribute
*/
OS_USER_ER_ID OS_User_Acre_Cyc(T_OS_USER_CCYC* pk_ccyc);

/**
This function is wrapper function of del_cyc().<br>
It operates cyclic handler.<br>
@param [in]     cycid               : Cyclic handler ID
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered cyclic handler
*/
OS_USER_ER OS_User_Del_Cyc(OS_USER_ID cycid);

/**
This function is wrapper function of sta_cyc().<br>
It operates cyclic handler.<br>
@param [in]     cycid               : Cyclic handler ID
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered cyclic handler
*/
OS_USER_ER OS_User_Sta_Cyc(OS_USER_ID cycid);

/**
This function is wrapper function of stp_cyc().<br>
It stops cyclic handler.<br>
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_ID              : ID error
@retval D_OS_USER_E_NOEXS           : Unregistered cyclic handler
*/
OS_USER_ER OS_User_Stp_Cyc(OS_USER_ID cycid);

/**
*/
OS_USER_ER OS_User_Cre_Alm(OS_USER_ID almid, T_OS_USER_CALM *pk_calm);

/**
*/
OS_USER_ER OS_User_Del_Alm(OS_USER_ID almid);

/**
*/
OS_USER_ER OS_User_Sta_Alm(OS_USER_ID almid, OS_USER_RELTIM almtim);

/**
*/
OS_USER_ER OS_User_Stp_Alm(OS_USER_ID almid);

/**
*/
OS_USER_ER OS_User_Ref_Alm(OS_USER_ID almid, T_OS_USER_RALM *pk_ralm);

/**
This function is wrapper function of dis_dsp().<br>
It shifts to dispatch disabled state.<br>
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_CTX             : Context error
*/
OS_USER_ER OS_User_Dis_Dsp(VOID);

/**
This function is wrapper function of ena_dsp().<br>
It shifts to dispatch enabled state.<br>
@retval D_OS_USER_E_OK              : Success
@retval D_OS_USER_E_CTX             : Context error
*/
OS_USER_ER OS_User_Ena_Dsp(VOID);

/**
This function is wrapper function of get_tid().<br>
It gets task identifier.<br>
@param [in]     p_tskid             : Pointer of task ID
@retval D_OS_USER_E_OK              : Success
*/
OS_USER_ER OS_User_Get_Tid(OS_USER_ID* p_tskid);

#if TC_PRCSMAX==1
/**
This function is wrapper function of loc_cpu().<br>
It changes to CPU lock state.<br>
@retval D_OS_USER_E_OK              : Success
*/
OS_USER_ER OS_User_Loc_Cpu(VOID);

/**
This function is wrapper function of unl_cpu().<br>
It changes to CPU lock release state.<br> 
@retval D_OS_USER_E_OK              : Success
*/
OS_USER_ER OS_User_Unl_Cpu(VOID);
#endif

/**
*/
BOOL OS_User_Sns_Ctx(VOID);

#if 0
/**
This function is wrapper function of ref_sys().<br>
It refers system status.<br>
@param [in]     pk_rsys             : Packet of refer system structure
@retval D_OS_USER_E_OK              : Success
*/
OS_USER_ER OS_User_Ref_Sys(T_OS_USER_RSYS* pk_rsys);
#endif

/**
This function is wrapper function of ref_cfg().<br>
It refers configuration information.<br>
@param [in]     pk_rcfg             : Packet to Refer Configuration Information
@retval D_OS_USER_E_OK              : Success
*/
OS_USER_ER OS_User_Ref_Cfg(T_OS_USER_RCFG *pk_rcfg);

// 
VOID OS_User_Ini(VOID);
VOID OS_User_Sta(VOID);
VOID OS_User_Ini_Task(OS_USER_ID prcsid, OS_USER_ID tskid, T_OS_USER_CONTEXT *stackp, OS_USER_FP task, OS_USER_PRI tskpri);
VOID OS_User_Ini_Sem(OS_USER_ID semid, INT32 semcnt, INT32 maxsem);
VOID OS_User_Ini_Mpl(OS_USER_ID mplid, OS_USER_VP start_adrs, OS_USER_VP end_adrs);
VOID OS_User_Ini_Mpf(OS_USER_ID mpfid, OS_USER_VP_INT mpfcnt, OS_USER_VP_INT blfsz, OS_USER_VP mpf, OS_USER_VP attr);
VOID OS_User_Ini_Msg(OS_USER_ID mbxid, OS_USER_VP **msgbuf, OS_USER_VP_INT mbxnum);
OS_USER_ER OS_User_Loc_Mig(VOID);
OS_USER_ER OS_User_Unl_Mig(VOID);

USHORT custom_prcs_id_num(VOID);
VOID idle_task(VOID);

#ifdef __cplusplus
}
#endif

#endif  // _OS_USER_H_
