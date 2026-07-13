/**
 * @file		os_user_custom.h
 * @brief		OS Function wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _OS_USER_H_
#define _OS_USER_H_

#include "../../../cpu_tkernel/basefw/MILB_API/include/ddim_typedef.h"
#include "TKernel_id.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/** @weakgroup fj_os_wrapper_definition
@{*/

/* Kernel */
#define D_OS_USER_KERNEL		(1)		/**< Kernel (1:T-Kernel) */

#ifndef NULL
#define NULL        0
#endif

/* Data type */
typedef INT32		OS_USER_ER;			/**< Define type of "ER" (Type of OS function's return value) */
typedef INT32		OS_USER_ID;			/**< Define type of "ID" (Type of OS function's argument) */
typedef INT32		OS_USER_TMO;		/**< Define type of "TMO" (Type of OS function's argument) */
typedef UINT32		OS_USER_RELTIM;		/**< Define type of "RELTIM" (Type of OS function's argument) */
typedef ULLONG		OS_USER_SYSTIM;		/**< Define type of "SYSTIM" (Type of OS function's argument) */
typedef ULONG		OS_USER_FLGPTN;		/**< Define type of "FLGPTN" (Type of OS function's argument) */
typedef UINT32		OS_USER_MODE;		/**< Define type of "MODE" (Type of OS function's argument) */
typedef INT32		OS_USER_VP_INT;		/**< Define type of "VP_INT" (Type of OS function's argument) */
typedef INT32		OS_USER_ER_ID;		/**< Define type of "ER_ID" (Type of OS function's argument) */
typedef UINT32		OS_USER_ATR;		/**< Define type of "ATR" (Type of OS function's argument) */
typedef INT32		OS_USER_PRI;		/**< Define type of "PRI" (Type of OS function's argument) */
typedef VOID		(*OS_USER_FP)();	/**< Define type of "FP" (Type of OS function's argument) */
typedef VOID*		OS_USER_VP;			/**< Define type of "VP" (Type of OS function's argument) */
typedef UINT32		OS_USER_SIZE;		/**< Define type of "SIZE" (Type of OS function's argument) */
typedef CHAR		OS_USER_VB;			/**< Define type of "VG" (Type of OS function's argument) */
typedef UINT32		OS_USER_STAT;		/**< Define type of "STAT" (Type of OS function's argument) */
typedef OS_USER_ER	OS_USER_ER_UINT;	/**< Define type of "ER_UINT" (Type of OS function's argument) */
typedef INT32		OS_USER_INTLV;		/**< Define type of "INTLV" (Type of OS function's argument) */
typedef UINT32		OS_USER_INTID;		/**< Define type of "INTID" (Type of OS function's argument) */
typedef INT32		OS_USER_W;			/**< Define type of "W" (Type of OS function's argument) */
typedef UCHAR		OS_USER_UB;			/**< Define type of "UB" (Type of OS function's argument) */
typedef SHORT		OS_USER_UH;			/**< Define type of "UH" (Type of OS function's argument) */
typedef UINT32		OS_USER_UW;			/**< Define type of "UW" (Type of OS function's argument) */
typedef INT32		OS_USER_VW;			/**< Define type of "VW" (Type of OS function's argument) */
typedef BOOL		OS_USER_BOOL;		/**< Define type of "BOOL" (Type of OS function's argument) */
typedef INT32		OS_USER_INT;		/**< Define type of "INT" (Type of OS function's argument) */
typedef UINT32		OS_USER_UINT;		/**< Define type of "UINT" (Type of OS function's argument) */
typedef INT32		OS_USER_RNO;		/**< Define type of "RNO" (Type of OS function's argument) */

/* Error code */
#define D_OS_USER_E_OK			(0)				/**< Normal End (E_OK) */
#define D_OS_USER_E_SYS			(-5)			/**< System Error (E_SYS) */
#if D_OS_USER_KERNEL
#define D_OS_USER_E_NOCOP		(-6)			/**< Coprocessor Unusable (E_NOCOP) */
#endif
#define D_OS_USER_E_NOSPT		(-9)			/**< No Support Function (E_NOSPT) */
#define D_OS_USER_E_RSFN		(-10)			/**< Reserve Function Code (E_RSFN) */
#define D_OS_USER_E_RSATR		(-11)			/**< Reserve Attribute (E_RSATR) */
#define D_OS_USER_E_PAR			(-17)			/**< Parameter Error (E_PAR) */
#define D_OS_USER_E_ID			(-18)			/**< ID Number Error (E_ID) */
#define D_OS_USER_E_CTX			(-25)			/**< Context Error (E_CTX) */
#define D_OS_USER_E_MACV		(-26)			/**< Memory Access Violation (E_MACV) */
#define D_OS_USER_E_OACV		(-27)			/**< Object Access Violation (E_OACV) */
#define D_OS_USER_E_ILUSE		(-28)			/**< Service Call Illegal Use (E_ILUSE) */
#define D_OS_USER_E_NOMEM		(-33)			/**< No Memory (E_NOMEM) */
#if D_OS_USER_KERNEL
#define D_OS_USER_E_LIMIT		(-34)			/**< Exceeded the limits of the system (E_LIMIT) */
#else
#define D_OS_USER_E_NOID		(-34)			/**< No ID Number (E_NOID) */
#endif
#define D_OS_USER_E_OBJ			(-41)			/**< Object State Error (E_OBJ) */
#define D_OS_USER_E_NOEXS		(-42)			/**< Object Not Exist (E_NOEXS) */
#define D_OS_USER_E_QOVR		(-43)			/**< Queueing Overflow (E_QOVR) */
#define D_OS_USER_E_RLWAI		(-49)			/**< Release Wait Mode (E_RLWAI) */
#define D_OS_USER_E_TMOUT		(-50)			/**< Polling Error or Timeout (E_TMOUT) */
#define D_OS_USER_E_DLT			(-51)			/**< Delete Wait Object (E_DLT) */
#if D_OS_USER_KERNEL
#define D_OS_USER_E_DISWAI		(-52)			/**< Wait released by wait ban (E_DISWAI) */
#define D_OS_USER_E_IO			(-57)			/**< I/O Error (E_IO) */
#define D_OS_USER_E_NOMDA		(-58)			/**< No Media (E_NOMDA) */
#else
#define D_OS_USER_E_CLS			(-52)			/**< Change Wait Object State (E_CLS) */
#define D_OS_USER_E_WBLK		(-57)			/**< Get Non Blocking (E_WBLK) */
#define D_OS_USER_E_BOVR		(-58)			/**< Buffer Overflow (E_BOVR) */
#endif
#if D_OS_USER_KERNEL
#define D_OS_USER_E_BUSY		(-65)			/**< Busy State (E_BUSY) */
#define D_OS_USER_E_ABORT		(-66)			/**< Abort (E_ABORT) */
#define D_OS_USER_E_RONLY		(-67)			/**< Write Protected (E_RONLY) */
#define D_OS_USER_E_DOMAIN		(-68)			/**< Domain Error (E_DOMAIN) */
#define D_OS_USER_E_ONAME		(-69)			/**< Object Name Error (E_ONAME) */
#define D_OS_USER_E_DACV		(-70)			/**< Access Protection Error (E_DACV) */
#define D_OS_USER_E_IPC			(-71)			/**< Processor Communication Error (E_IPC) */
#define D_OS_USER_E_IPCA		(-72)			/**< Absolute Error Communication Between Processors (E_IPCA) */
#define D_OS_USER_E_IPCS		(-73)			/**< Processor Communication Sstate Error (E_IPCS) */
#endif

/* Wait queue */
#define D_OS_USER_TA_TFIFO		(0x00)			/**< Queuing is the order of FIFO (TA_TFIFO) */
#define D_OS_USER_TA_TPRI		(0x01)			/**< Queuing is the order of task priority (TA_TPRI) */

/* Task */
#define D_OS_USER_TSK_SELF		(0x00)			/**< Invoking task (TSK_SELF) */
#define D_OS_USER_TSK_NONE		(0x00)			/**< No queued task (TSK_NONE) */
#define D_OS_USER_TPRI_INI		(0x00)			/**< Initial task priority (TPRI_INI) */

/* Task creation */
#if D_OS_USER_KERNEL
#define D_OS_USER_TA_ASM		0x00000000U		/**< Program by assembler */
#define D_OS_USER_TA_HLNG		0x00000001U		/**< Program by high level programming language */
#define D_OS_USER_TA_SSTKSZ		0x00000002U		/**< Set system stack size */
#define D_OS_USER_TA_USERSTACK	0x00000004U		/**< Set user stack size */
#define D_OS_USER_TA_TASKSPACE	0x00000008U		/**< Specify task space */
#define D_OS_USER_TA_RESID		0x00000010U		/**< Specify resource group */
#define D_OS_USER_TA_ONAME		0x00000040U		/**< Use object name */
#define D_OS_USER_TA_RNG0		0x00000000U		/**< Execute by protection level 0 */
#define D_OS_USER_TA_RNG1		0x00000100U		/**< Execute by protection level 1 */
#define D_OS_USER_TA_RNG2		0x00000200U		/**< Execute by protection level 2 */
#define D_OS_USER_TA_RNG3		0x00000300U		/**< Execute by protection level 3 */
#define D_OS_USER_TA_COP0		0x00001000U		/**< Use coprocessor (ID=0) */
#define D_OS_USER_TA_COP1		0x00002000U		/**< Use coprocessor (ID=1) */
#define D_OS_USER_TA_COP2		0x00004000U		/**< Use coprocessor (ID=2) */
#define D_OS_USER_TA_COP3		0x00008000U		/**< Use coprocessor (ID=3) */
#define D_OS_USER_TA_DOMID		0x00010000U		/**< Specify domain ID */
#define D_OS_USER_TA_ASSPRC		0x00020000U		/**< Assign processor */
#define D_OS_USER_TA_PRIVATE	0x00040000U		/**< Protection attribute is private */
#define D_OS_USER_TA_PROTECTED	0x00080000U		/**< Protection attribute is protected */
#define D_OS_USER_TA_PUBLIC		0x00000000U		/**< Protection attribute is public */
#else
#define D_OS_USER_TA_HLNG		(0x00)			/**< Handler is written by the C language (TA_HLNG) */
#define D_OS_USER_TA_ASM		(0x01)			/**< Handler is written by the assembler (TA_ASM) */
#define D_OS_USER_TA_ACT		(0x02)			/**< Handler is started after handler generation (TA_ACT) */
#endif

/* Semaphore generation */
#define D_OS_USER_TA_FIRST		0x00000000U		/**< Give priority to task at head of wait queue */
#define D_OS_USER_TA_CNT		0x00000002U		/**< Give priority to task whose request counts is less */
#define D_OS_USER_TA_DSNAME		0x00000040U		/**< Use object name */
#define D_OS_USER_TA_NODISWAI	0x00000080U		/**< Wait disable rejection */

/* Event flag */
#define D_OS_USER_TA_WSGL		0x00000000U		/**< Disable multiple tasks wait */
#define D_OS_USER_TA_WMUL		0x00000008U		/**< Enable multiple tasks wait */

/* Mail box */
#define D_OS_USER_TA_MFIFO		0x00000000U		/**< Manage messages by FIFO */
#define D_OS_USER_TA_MPRI		0x00000002U		/**< Manage messages by priority order */

/* Mutex attribute */
#define D_OS_USER_TA_INHERIT	(0x02)			/**< Protocol for inherited priority (TA_INHERIT) */
#define D_OS_USER_TA_CEILING	(0x03)			/**< Protocol for ceiling priority (TA_CEILING) */

/* Fixed size memory pool */
#define D_OS_USER_TA_NORESIDENT	0x00000010U		/**< Non-resident */

/* Variable size memory pool */
/* Cycle handler */
#define D_OS_USER_TA_STA		0x00000002U		/**< Cycle handler startup */
#define D_OS_USER_TA_PHS		0x00000004U		/**< Save cycle handler phase */

/* Assign processor */
#define D_OS_USER_TP_PRC1		0x00000001U		/**< It operates by processor 1 */
#define D_OS_USER_TP_PRC2		0x00000002U		/**< It operates by processor 2 */
#define D_OS_USER_TP_PRC3		0x00000004U		/**< It operates by processor 3 */
#define D_OS_USER_TP_PRC4		0x00000008U		/**< It operates by processor 4 */

/* Task state */
#define D_OS_USER_TTS_RUN		(0x00000001)	/**< RUNNING (TTS_RUN) */
#define D_OS_USER_TTS_RDY		(0x00000002)	/**< READY (TTS_RDY) */
#define D_OS_USER_TTS_WAI		(0x00000004)	/**< WAITING (TTS_WAI) */
#define D_OS_USER_TTS_SUS		(0x00000008)	/**< SUSPENDED (TTS_SUS) */
#define D_OS_USER_TTS_WAS		(0x0000000c)	/**< WAITING-SUSPENDED (TTS_WAS) */
#define D_OS_USER_TTS_DMT		(0x00000010)	/**< DORMANT (TTS_DMT) */
#define D_OS_USER_TTS_NODISWAI	(0x00000080)	/**< Wait disable rejection state */

/* Event flag wait modes */
#define D_OS_USER_TWF_ANDW		(0x00000000)	/**< Bit pattern AND wait (TWF_ANDW)*/
#define D_OS_USER_TWF_ORW		(0x00000001)	/**< Bit pattern OR wait (TWF_ORW) */
#define D_OS_USER_TWF_CLR		(0x00000010)	/**< All clear specify */
#define D_OS_USER_TWF_BITCLR	(0x00000020)	/**< Only condition bit clear specify */

/* Timeout value */
#define D_OS_USER_TMO_FEVR		(-1)			/**< Forever Wait (TMO_FEVR) */
#define D_OS_USER_TMO_POL		(0)				/**< Wait by Polling (TMO_POL) */

/* Task waiting state */
#define	D_OS_USER_TTW_SLP		(0x00000001)	/**< Waiting on slp_tsk (TTW_SLP) */
#define	D_OS_USER_TTW_DLY		(0x00000002)	/**< Waiting on dly_tsk (TTW_DLY) */
#define	D_OS_USER_TTW_SEM		(0x00000004)	/**< Waiting on wai_sem (TTW_SEM) */
#define	D_OS_USER_TTW_FLG		(0x00000008)	/**< Waiting on wai_flg (TTW_FLG) */
#define	D_OS_USER_TTW_SDTQ		(0x00000010)	/**< Waiting on snd_dtq (TTW_SDTQ) */
#define	D_OS_USER_TTW_RDTQ		(0x00000020)	/**< Waiting on rcv_dtq (TTW_RDTQ) */
#define	D_OS_USER_TTW_MBX		(0x00000040)	/**< Waiting on rcv_mbx (TTW_MBX) */
#define	D_OS_USER_TTW_MTX		(0x00000080)	/**< Waiting on loc_mtx (TTW_MTX) */
#define	D_OS_USER_TTW_SMBF		(0x00000100)	/**< Waiting on snd_mbf (TTW_SMBF) */
#define	D_OS_USER_TTW_RMBF		(0x00000200)	/**< Waiting on rcv_mbf (TTW_RMBF) */
#define	D_OS_USER_TTW_CAL		(0x00000400)	/**< Waiting on a rendezvous call (TTW_CAL) */
#define	D_OS_USER_TTW_ACP		(0x00000800)	/**< Waiting on a rendezvous acceptance (TTW_ACP) */
#define	D_OS_USER_TTW_RDV		(0x00001000)	/**< Waiting on a rendezvous termination (TTW_RDV) */
#define	D_OS_USER_TTW_MPF		(0x00002000)	/**< Waiting on get_mpf (TTW_MPF) */
#define	D_OS_USER_TTW_MPL		(0x00004000)	/**< Waiting on get_mpl (TTW_MPL) */
#define D_OS_USER_TTW_EV1		(0x00010000)	/**< Task event # 1 wait */
#define D_OS_USER_TTW_EV2		(0x00020000)	/**< Task event # 2 wait */
#define D_OS_USER_TTW_EV3		(0x00040000)	/**< Task event # 3 wait */
#define D_OS_USER_TTW_EV4		(0x00080000)	/**< Task event # 4 wait */
#define D_OS_USER_TTW_EV5		(0x00100000)	/**< Task event # 5 wait */
#define D_OS_USER_TTW_EV6		(0x00200000)	/**< Task event # 6 wait */
#define D_OS_USER_TTW_EV7		(0x00400000)	/**< Task event # 7 wait */
#define D_OS_USER_TTW_EV8		(0x00800000)	/**< Task event # 8 wait */
#define D_OS_USER_TTX_SVC		(0x80000000)	/**< Extended SVC call disable */

/** MapMemory() attr */
#define D_OS_USER_MM_USER		0x20U			/**< User */
#define D_OS_USER_MM_SYSTEM		0x00U			/**< System */
#define D_OS_USER_MM_READ		0x00U			/**< Read */
#define D_OS_USER_MM_WRITE		0x10U			/**< Write */
#define D_OS_USER_MM_EXECUTE	0x01U			/**< Execute */
#define D_OS_USER_MM_CDIS		0x0cU			/**< Cache disabled */

/** Set interrupt mode */
#define D_OS_USER_IM_LEVEL		0x00U			/**< level trigger (high level Only) */
#define D_OS_USER_IM_EDGE		0x02U			/**< edge  trigger (rising edge only) */
#define D_OS_USER_IM_HI			0x00U			/**< high level trigger/rising  edge trigger */
#define D_OS_USER_IM_NN			0x00U			/**< N-N Software Model */
#define D_OS_USER_IM_1N			0x01U			/**< 1-N Software Model */

/** CPU Exception no */
#define D_OS_USER_EXCP_UNDEFINED	0			/**< UnDefine */
#define D_OS_USER_EXCP_DATA_ABORT	1			/**< Abort */

/** Object name information for OS_User_Td_Ref_Dsname, OS_User_Td_Set_Dsname */
#define D_OS_USER_TN_TSK		0x01			/**< Task */
#define D_OS_USER_TN_SEM		0x02			/**< Semaphore */
#define D_OS_USER_TN_FLG		0x03			/**< Event flag */
#define D_OS_USER_TN_MBX		0x04			/**< Mail box */
#define D_OS_USER_TN_MBF		0x05			/**< Message Buffer */
#define D_OS_USER_TN_POR		0x06			/**< Port */
#define D_OS_USER_TN_MTX		0x07			/**< Mutex */
#define D_OS_USER_TN_MPL		0x08			/**< Variable size memory pool */
#define D_OS_USER_TN_MPF		0x09			/**< Fixed size memory pool */
#define D_OS_USER_TN_CYC		0x0a			/**< Cyclic */
#define D_OS_USER_TN_ALM		0x0b			/**< Alarm */
#define D_OS_USER_TN_DOM		0x0c			/**< Domain */


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Task Create Information */
typedef struct t_os_user_ctsk {
	OS_USER_ATR		tskatr;			/**< Task Attributes */
	OS_USER_VP_INT	exinf;			/**< Extended Information */
	OS_USER_FP		task;			/**< Task Start Address */
	OS_USER_PRI		itskpri;		/**< Initial Task Priority */
	OS_USER_SIZE	stksz;			/**< Stack Size */
	OS_USER_VP		stk;			/**< Stack Region Top Address */
#if D_OS_USER_KERNEL
	OS_USER_UW		AssignProcessor;/**< Specify execution processor */
#endif
} T_OS_USER_CTSK;

/** Task Statistics Information */
typedef struct t_os_user_itsk {
	OS_USER_RELTIM	stime;			/**< Cumulative system execution time (milliseconds) */
	OS_USER_RELTIM	utime;			/**< Cumulative user execution time (milliseconds) */
} T_OS_USER_ITSK;

/** Task State Information */
typedef struct t_os_user_rtsk {
	OS_USER_STAT	tskstat;		/**< Task State */
	OS_USER_PRI		tskpri;			/**< Task Current Priority */
	OS_USER_PRI		tskbpri;		/**< Task Base Priority */
	OS_USER_STAT	tskwait;		/**< Task Wait Factors */
	OS_USER_ID		wobjid;			/**< Waiting Target ID Number */
	OS_USER_TMO		lefttmo;		/**< Time Out Period */
	UINT32			actcnt;			/**< Activation Request Queue Count */
	UINT32			wupcnt;			/**< Number of Queued Wakeup Request */
	UINT32			suscnt;			/**< Number of Nested Suspend Request */
	OS_USER_VP		stk;			/**< StackPointer */
	OS_USER_VP		stktop;			/**< StackTopAddress */
} T_OS_USER_RTSK;

/** Task Status Information */
typedef struct t_os_user_rtst {
	OS_USER_STAT	tskstat;		/**< Task State */
	OS_USER_STAT	tskwait;		/**< Task Waiting State */
} T_OS_USER_RTST;

/** Semaphore Create Information */
typedef struct t_os_user_csem {
	OS_USER_ATR		sematr;			/**< Semaphore Attribute */
	UINT32			isemcnt;		/**< Initial Value of Semaphore Resource Count */
	UINT32			maxsem;			/**< Max Semaphore Resource Count */
} T_OS_USER_CSEM;

/** Semaphore State Information */
typedef struct t_os_user_rsem {
	OS_USER_ID		wtskid;			/**< Top Task ID of Semaphore Wait Matrix */
	UINT32			semcnt;			/**< Semaphore Resource Count */
} T_OS_USER_RSEM;

/** Event flag creation information */
typedef	struct t_os_user_cflg {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_ATR		flgatr;			/**< Event flag attribute */
	OS_USER_UINT	iflgptn;		/**< Event flag initial value */
	OS_USER_ID		domid;			/**< Domain ID */
	OS_USER_UB		oname[8];		/**< Object name */
} T_OS_USER_CFLG;

/** Eventflag State Information*/
typedef struct t_os_user_rflg {
	OS_USER_ID		wtskid;			/**< Top Task ID of event flag ID. */
	OS_USER_FLGPTN 	flgptn;			/**< Bit pattern setting.*/
} T_OS_USER_RFLG;

/** Mail box creation information */
typedef	struct t_os_user_cmbx {
	OS_USER_ATR		mbxatr;			/**< Mail box attribute */
	OS_USER_PRI		maxmpri;		/**< Maximum Message Priority */
	OS_USER_VP		mprihd;			/**< Message Header Address */
} T_OS_USER_CMBX;

/** Mailbox Message Information */
typedef struct t_os_user_msg {
	struct t_os_user_msg	*hmsg;	/**< Mailbox Message Information */
} T_OS_USER_MSG;

/** Mailbox Status Information */
typedef struct t_os_user_rmbx {
	OS_USER_ID		wtskid;			/**< Wait Task Information */
	T_OS_USER_MSG	*pk_msg;		/**< Packet of Message */
} T_OS_USER_RMBX;

/** Create Mutex Information */
typedef struct t_os_user_cmtx {
	OS_USER_ATR		mtxatr;			/**< Mutex Attribute */
	OS_USER_PRI		ceilpri;		/**< Ceiling Priority */
} T_OS_USER_CMTX;

/** Mutex Status Information */
typedef struct t_os_user_rmtx {
	OS_USER_ID		htskid;			/**< Halt Task Information */
	OS_USER_ID		wtskid;			/**< Wait Task Information */
} T_OS_USER_RMTX;

/** Message Buffer creation information */
typedef	struct t_os_user_cmbf {
	OS_USER_ATR		mbfatr;			/**< Message BufferAttribute */
	OS_USER_SIZE	maxmsz;			/**< Max Message Size */
	OS_USER_SIZE	mbfsz;			/**< Buffer Size */
	OS_USER_VP		mbf;			/**< Buffer Address */
} T_OS_USER_CMBF;

/** Message Buffer Information */
typedef struct t_os_user_rmbf {
	OS_USER_ID		stskid;			/**< Send Wait Task ID */
	OS_USER_ID		rtskid;			/**< Receive Wait Task ID */
	UINT32			smsgcnt;		/**< Number of Messages */
	OS_USER_SIZE	fmbfsz;			/**< Free Message Buffer Size */
} T_OS_USER_RMBF;

/** Fixed size memory pool state information */
typedef	struct t_os_user_cmpf {
	OS_USER_ATR		mpfatr;
	UINT32			blkcnt;
	OS_USER_SIZE	blksz;
	OS_USER_VP		mpf;
} T_OS_USER_CMPF;

/** Fixed-size Memory Pool Information */
typedef struct t_os_user_rmpf {
	OS_USER_ID		wtskid;			/**< Wait Task Information */
	UINT32			fblkcnt;		/**< Free Block Count */
} T_OS_USER_RMPF;

/** Fixed size memory pool state information */
typedef	struct t_os_user_cmpl {
	OS_USER_ATR		mplatr;			/**< Memory Pool Attribute */
	OS_USER_SIZE	mplsz;			/**< Memory Pool Size */
	OS_USER_VP		mpl;			/**< Memory Pool Address */
	OS_USER_SIZE	blksz;			/**< Memory Block Size */
} T_OS_USER_CMPL;

/** Variable-size Memory Pool Information */
typedef struct t_os_user_rmpl {
	OS_USER_ID		wtskid;			/**< Wait Task Information */
	OS_USER_SIZE	fmplsz;			/**< Free Memory Size */
	OS_USER_SIZE	fblksz;			/**< Free Memory Block Size */
} T_OS_USER_RMPL;

/** Cyclic Create Information */
typedef struct t_os_user_ccyc {
	OS_USER_ATR		cycatr;			/**< Cyclic Handler Attribute */
#if D_OS_USER_KERNEL
	OS_USER_VP		exinf;			/**< Extended Information */
#else
	OS_USER_VP_INT	exinf;			/**< Extended Information */
#endif
	OS_USER_FP		cychdr;			/**< Pointer of Cyclic Handler Function */
	OS_USER_RELTIM	cyctim;			/**< Cyclic Start Time Interval */
	OS_USER_RELTIM	cycphs;			/**< Cyclic Handler Start Phase */
#if D_OS_USER_KERNEL
	OS_USER_UW		assprc;			/**< AssignProcessor */
#endif
} T_OS_USER_CCYC;

/** Cycle handler state information */
typedef struct t_os_user_rcyc {
	OS_USER_STAT	cycstat;		/**< Cyclic Handler Status */
	OS_USER_RELTIM	lefttim;		/**< Left Time */
} T_OS_USER_RCYC;

/** Alarm Create Information */
typedef struct t_os_user_calm {
	OS_USER_ATR 	almatr;			/**< Alarm Handler Attribute */
	OS_USER_VP_INT	exinf;			/**< Extended Information */
	OS_USER_FP		almhdr;			/**< Alarm Handler Address */
#if D_OS_USER_KERNEL
	OS_USER_UW		assprc;			/**< AssignProcessor */
#endif
} T_OS_USER_CALM;

/** Alarm Status Information */
typedef struct t_os_user_ralm {
	OS_USER_STAT	almstat;		/**< Alarm Handler Status */
	OS_USER_RELTIM	lefttim;		/**< Left Time */
} T_OS_USER_RALM;

/** Rendezvous port creation information */
typedef	struct t_os_user_cpor {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_ATR		poratr;			/**< Port attribute */
	OS_USER_INT		maxcmsz;		/**< Maximum length of call message (byte) */
	OS_USER_INT		maxrmsz;		/**< Maximum length of replay message (byte) */
	OS_USER_ID		domid;			/**< Domain ID */
	OS_USER_UB		oname[8];		/**< Object name */
} T_OS_USER_CPOR;

/** Rendezvous port state information */
typedef struct t_os_user_rpor {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_ID		wtsk;			/**< Call wait task ID */
	OS_USER_ID		atsk;			/**< Receive wait task ID */
	OS_USER_INT		maxcmsz;		/**< Maximum length of call message (byte) */
	OS_USER_INT		maxrmsz;		/**< Maximum length of replay message (byte) */
} T_OS_USER_RPOR;

/** Domain creation information	*/
typedef struct t_os_user_cdom {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_ATR		domatr;			/**< Domain attribute */
	OS_USER_ID		domid;			/**< Domain ID */
	OS_USER_UB		oname[8];		/**< Object name */
} T_OS_USER_CDOM;

/** Task space information */
typedef struct t_os_user_tskspc {
	OS_USER_VP		uatb;			/**< Address of task space page table */
	OS_USER_INT		lsid;			/**< Logical space ID */
} T_OS_USER_TSKSPC;

/** General purpose register */
typedef struct t_os_user_regs {
	OS_USER_VW		r[13];			/**< General purpose register R0-R12 */
	OS_USER_VP		lr;				/**< Link register R14 */
} T_OS_USER_REGS;

/** Exception-related register */
typedef struct t_os_user_eit {
	OS_USER_VP		pc;				/**< Program counter R15 */
	OS_USER_UW		cpsr;			/**< Program status register */
	OS_USER_UW		taskmode;		/**< Task mode flag */
} T_OS_USER_EIT;

/** Control register */
typedef struct t_os_user_cregs {
	OS_USER_VP		ssp;			/**< System stack pointer R13_svc */
	OS_USER_VP		usp;			/**< User stack pointer R13_ usr */
	OS_USER_VP		uatb;			/**< Address of task specific space page table */
	OS_USER_UW		lsid;			/**< Task logical space ID */
} T_OS_USER_CREGS;

/** Coprocessor register */
typedef struct t_os_user_copreg {
	double d[32];
	OS_USER_UW		fpscr;			/**< Floating-Point Status and Control Register */
	OS_USER_UW		fpexc;			/**< Floating-Point Exception Register */
} T_OS_USER_COPREG;

/** Coprocessor register */
typedef union {
	T_OS_USER_COPREG	cop;
} T_OS_USER_COPREGS;

/** Domain state information */
typedef struct t_os_user_rdom {
	OS_USER_VP			exinf;		/**< Extended information */
	OS_USER_ID			domid;		/**< Domain ID */
	OS_USER_UINT		objcnt;		/**< Object count */
} T_OS_USER_RDOM;

/** Domain information */
typedef struct t_os_user_dmi {
	OS_USER_ATR		domatr;			/**< Domain attribute */
	OS_USER_ID		domid;			/**< Domain ID */
	OS_USER_ID		kdmid;			/**< Kernel domain ID */
	OS_USER_UB		oname[8];		/**< Object name */
} T_OS_USER_DMI;

/** Interrupt handler definition information */
typedef struct t_os_user_dint {
	OS_USER_ATR			intatr;		/**< Interrupt handler attribute */
	OS_USER_FP			inthdr;		/**< Interrupt handler address */
	OS_USER_UW			assprc;		/**< AssignProcessor */
#if TA_GP
	OS_USER_VP			gp;			/**< Global pointer (gp) */
#endif
} T_OS_USER_DINT;

/** System Status Information */
typedef struct t_os_user_rsys {
	OS_USER_STAT	sysstat;		/**< System State */
} T_OS_USER_RSYS;

/** Version information */
typedef struct t_os_user_rver {
	OS_USER_UH		maker;			/**< OS manufacturer */
	OS_USER_UH		prid;			/**< OS identification number */
	OS_USER_UH		spver;			/**< Specification version */
	OS_USER_UH		prver;			/**< OS product version */
	OS_USER_UH		prno[4];		/**< Product number, Product management information */
} T_OS_USER_RVER;

/** System call/extended SVC trace definition */
typedef struct td_os_user_hsvc {
	OS_USER_FP		enter;			/**< Hook routine before calling */
	OS_USER_FP		leave;			/**< Hook routine after calling */
} TD_OS_USER_HSVC;

/** Task dispatch trace definition */
typedef struct td_os_user_hdsp {
	OS_USER_FP		exec;			/**< Hook routine when starting execution */
	OS_USER_FP		stop;			/**< Hook routine when stopping execution */
} TD_OS_USER_HDSP;

/** Exception/Interrupt trace definition */
typedef struct td_os_user_hint {
	OS_USER_FP		enter;			/**< Hook routine before calling handler */
	OS_USER_FP		leave;			/**< Hook routine after calling handler */
} TD_OS_USER_HINT;

/** GetSpaceInfo() */
typedef struct t_os_user_spinfo {
	OS_USER_VP		paddr;			/**< Physical address corresponding to addr */
	OS_USER_VP		page;			/**< Physical address of page that addr belongs */
	OS_USER_INT		pagesz;			/**< Page size (Bytes) */
	OS_USER_INT		cachesz;		/**< Cache line size (Bytes) */
	OS_USER_INT		cont;			/**< Area size of physical address continuous from addr (Bytes) */
} T_OS_USER_SPINFO;

/** Task state information */
typedef	struct td_os_user_rtsk {
	OS_USER_VP		exinf;			/*+< Extended information */
	OS_USER_PRI		tskpri;			/*+< Current priority */
	OS_USER_PRI		tskbpri;		/*+< Base priority */
	OS_USER_UINT	tskstat;		/*+< Task state */
	OS_USER_UINT	tskwait;		/*+< Wait factor */
	OS_USER_ID		wid;			/*+< Wait object ID */
	OS_USER_INT		wupcnt;			/*+< Number of wakeup requests queuing */
	OS_USER_INT		suscnt;			/*+< Number of SUSPEND request nests */
	OS_USER_RELTIM	slicetime;		/*+< Maximum continuous execution time (millisecond) */
	OS_USER_UINT	waitmask;		/*+< Disabled wait factor */
	OS_USER_UINT	texmask;		/*+< Enabled task exception */
	OS_USER_UINT	tskevent;		/*+< Occurring task event */
	OS_USER_FP		task;			/*+< Task startup address */
	OS_USER_INT		stksz;			/*+< User stack size (byte) */
	OS_USER_INT		sstksz;			/*+< System stack size (byte) */
	OS_USER_VP		istack;			/*+< User stack pointer initial value */
	OS_USER_VP		isstack;		/*+< System stack pointer initial value */
} TD_OS_USER_RTSK;

/** Task Statistics Information */
typedef struct td_os_user_itsk {
	OS_USER_RELTIM	stime;			/**< Cumulative system execution time (milliseconds) */
	OS_USER_RELTIM	utime;			/**< Cumulative user execution time (milliseconds) */
} TD_OS_USER_ITSK;

/** Task exception state information */
typedef struct td_os_user_rtex {
	OS_USER_UINT	pendtex;		/**< Occurring task exception */
	OS_USER_UINT	texmask;		/**< Enabled task exception */
} TD_OS_USER_RTEX;

/** Semaphore state information */
typedef	struct td_os_user_rsem {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_ID		wtsk;			/**< Wait task ID */
	OS_USER_INT		semcnt;			/**< Current semaphore value */
} TD_OS_USER_RSEM;

/** Event flag state information */
typedef	struct td_os_user_rflg {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_ID		wtsk;			/**< Wait task ID */
	OS_USER_UINT	flgptn;			/**< Current event flag pattern */
} TD_OS_USER_RFLG;

/** Mail box state information */
typedef	struct td_os_user_rmbx {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_ID		wtsk;			/**< Wait task ID */
	T_OS_USER_MSG	*pk_msg;		/**< Next received message */
} TD_OS_USER_RMBX;

/** Mutex state information */
typedef struct td_os_user_rmtx {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_ID		htsk;			/**< Locking task ID */
	OS_USER_ID		wtsk;			/**< Lock wait task ID */
} TD_OS_USER_RMTX;

/** Message buffer state information */
typedef struct td_os_user_rmbf {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_ID		wtsk;			/**< Receive wait task ID */
	OS_USER_ID		stsk;			/**< Send wait task ID */
	OS_USER_INT		msgsz;			/**< Next received message size (byte) */
	OS_USER_INT		frbufsz;		/**< Free buffer size (byte) */
	OS_USER_INT		maxmsz;			/**< Maximum length of message (byte) */
} TD_OS_USER_RMBF;

/** Rendezvous port state information */
typedef struct td_os_user_rpor {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_ID		wtsk;			/**< Call wait task ID */
	OS_USER_ID		atsk;			/**< Receive wait task ID */
	OS_USER_INT		maxcmsz;		/**< Maximum length of call message (byte) */
	OS_USER_INT		maxrmsz;		/**< Maximum length of replay message (byte) */
} TD_OS_USER_RPOR;

/** Fixed size memory pool state information */
typedef struct td_os_user_rmpf {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_ID		wtsk;			/**< Wait task ID */
	OS_USER_INT		frbcnt;			/**< Number of free blocks */
} TD_OS_USER_RMPF;

/** Variable size memory pool state information */
typedef struct td_os_user_rmpl {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_ID		wtsk;			/**< Wait task ID */
	OS_USER_INT		frsz;			/**< Total size of free area (byte) */
	OS_USER_INT		maxsz;			/**< Size of maximum continuous free area (byte) */
} TD_OS_USER_RMPL;

/** Cycle handler state information */
typedef struct td_os_user_rcyc {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_RELTIM	lfttim;			/**< Remaining time until next handler startup */
	OS_USER_UINT	cycstat;		/**< Cycle handler status */
} TD_OS_USER_RCYC;

/** Alarm handler state information */
typedef struct td_os_user_ralm {
	OS_USER_VP		exinf;			/**< Extended information */
	OS_USER_RELTIM	lfttim;			/**< Remaining time until handler startup */
	OS_USER_UINT	almstat;		/**< Alarm handler status */
} TD_OS_USER_RALM;

/** System state information */
typedef struct td_os_user_rsys {
	OS_USER_INT		sysstat;		/**< System state */
	OS_USER_ID		runtskid;		/**< ID of task in execution state */
	OS_USER_ID		schedtskid;		/**< ID of task that should be in execution state */
} TD_OS_USER_RSYS;

/** Exception handler register */
typedef struct td_os_user_regs {
	OS_USER_UW r0;
	OS_USER_UW r1;
	OS_USER_UW r2;
	OS_USER_UW r3;
	OS_USER_UW r4;
	OS_USER_UW r5;
	OS_USER_UW r6;
	OS_USER_UW r7;
	OS_USER_UW r8;
	OS_USER_UW r9;
	OS_USER_UW r10;
	OS_USER_UW r11;
	OS_USER_UW r12;
	OS_USER_UW sp;
	OS_USER_UW lr;
	OS_USER_UW pc;
	OS_USER_UW cpsr;
} TD_OS_USER_REGS;

/** Refer Configuration Information */
typedef struct t_os_user_rcfg {
	OS_USER_PRI		max_tpri;		/**< Taks priority maximum value */
	OS_USER_PRI		max_mpri;		/**< Message priority maximum value */
	OS_USER_ID		max_tskid;		/**< Task ID maximum valuee */
	OS_USER_ID		max_semid;		/**< Semaphore ID maximum value */
	OS_USER_ID		max_flgid;		/**< Eventflag ID maximum value */
	OS_USER_ID		max_dtqid;		/**< Data queue ID maximum value */
	OS_USER_ID		max_mbxid;		/**< Mailbox ID maximum value */
	OS_USER_ID		max_mtxid;		/**< Mutex ID maximum value */
	OS_USER_ID		max_mbfid;		/**< Message buffer ID maximum value */
	OS_USER_ID		max_mpfid;		/**< Fixed-sized memory pool ID maximum value */
	OS_USER_ID		max_mplid;		/**< Variable-sized memory pool ID maximum value */
	OS_USER_ID		max_cycid;		/**< Cyclic handler ID maximum value */
	OS_USER_ID		max_almid;		/**< Alarm ID maximum value */
} T_OS_USER_RCFG;

#if 0
typedef struct t_os_user_context
{
//-----------------------------------------------------------------------------
// Main routine does NOT use SPSR.
// Interrupt routine keeps SPSR at top of interrupt entrance.
//	TC_UINT spsr;
//-----------------------------------------------------------------------------
	UINT32 cpsr;
	UINT32 r8;					/* v5 */
	UINT32 r9;					/* v6, sb */
	UINT32 r10;					/* v7, sl */
	UINT32 r11;					/* v8, fp */
//-----------------------------------------------------------------------------
// Main routine does NOT use them.
// Interrupt routine keeps them at top of interrupt entrance.
//	UINT32 r1;					/* a2 */
//	UINT32 r2;					/* a3 */
//	UINT32 r3;					/* a4 */
//	UINT32 r12;					/* ip */
//-----------------------------------------------------------------------------
	UINT32 argreg;				/* r0, a1 */
	UINT32 r4;					/* v1 */
	UINT32 r5;					/* v2 */
	UINT32 r6;					/* v3 */
	UINT32 r7;					/* v4, wr */
	OS_USER_FP task;			/* procedure, r14, lr, r15, pc */
} T_OS_USER_CONTEXT;

/** Message Priority Information */
typedef struct t_os_user_msg_pri {
	T_OS_USER_MSG	msgque;			/**< OS reserved area for message queues */
	OS_USER_PRI		msgpri;			/**< Message priority */
} T_OS_USER_MSG_PRI;
#endif

/* @} */	// fj_os_wrapper_definition

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/** @weakgroup fj_os_wrapper_api
@{*/

/*-Task Function--------------------------------------------------------*/
/**
This function is wrapper function of acre_tsk().<br>
It creates task.<br>
Task ID is assigned automatically.<br>
@param [in]		pk_ctsk				: Pointer of task create structure
@retval	PositiveValue				: Task ID
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_NOSPT			: No Support Function
@retval	D_OS_USER_E_PAR				: Parameter Error
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ER_ID OS_User_Acre_Tsk(T_OS_USER_CTSK* pk_ctsk);

/**
This function is wrapper function of del_tsk().<br>
It deletes task.<br>
@param [in]		taskid				: Task ID
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_OBJ				: Object state error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Del_Tsk(OS_USER_ID taskid);

/**
This function is wrapper function of sta_tsk().<br>
It starts task.<br>
@param [in]		taskid				: Task ID
@param [in]		stacd				: Start code
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_OBJ				: Object state error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Sta_Tsk(OS_USER_ID taskid, OS_USER_VP_INT stacd);

/**
This function is wrapper function of ext_tsk().<br>
It exit self task.<br>
*/
VOID OS_User_Ext_Tsk(VOID);

/**
This function is wrapper function of exd_tsk().<br>
It exit and delete self task.<br>
*/
VOID OS_User_Exd_Tsk(VOID);

/**
This function is wrapper function of ter_tsk().<br>
It terminate task.<br>
@param [in]		taskid				: Task ID
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_OBJ				: Object state error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Ter_Tsk(OS_USER_ID taskid);

/**
This function is wrapper function of inf_tsk().<br>
It task information.<br>
@param [in]		taskid				: Task ID
@param [in]		pk_itsk				: Packet of Task Statistics Information
@param [in]		clr					: Clear Task Statistics
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_PAR				: Parameter Error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Inf_Tsk(OS_USER_ID taskid, T_OS_USER_ITSK* pk_itsk, OS_USER_BOOL clr);

/**
This function is wrapper function of ref_tsk().<br>
It gets task state information.<br>
@param [in]		tskid				: Task ID
@param [out]	pk_rtsk				: Pointer of task state information structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Ref_Tsk(OS_USER_ID tskid, T_OS_USER_RTSK* pk_rtsk);

#if 0
/**
This function is wrapper function of ref_tst().<br>
It gets task state information (simplified version).<br>
@param [in]		tskid				: Task ID
@param [out]	pk_rtst				: Packet to Refer TaskStatus
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
*/
OS_USER_ER OS_User_Ref_Tst(OS_USER_ID tskid, T_OS_USER_RTST* pk_rtst);
#endif

/**
This function is wrapper function of tslp_tsk().<br>
It sleeps(with timeout) task.<br>
@param [in]		timeout				: time out value
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Wait released by wait ban
@retval	D_OS_USER_E_TMOUT			: Time out
@retval	D_OS_USER_E_CTX				: Context error
*/
OS_USER_ER OS_User_Tslp_Tsk(OS_USER_TMO timeout);

/**
This function is wrapper function of slp_tsk().<br>
It sleeps task.<br>
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Wait released by wait ban
@retval	D_OS_USER_E_CTX				: Context error
*/
OS_USER_ER OS_User_Slp_Tsk(VOID);

/**
This function is wrapper function of wup_tsk().<br>
It wake-up the task.<br>
@param [in]		taskid				: Task ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_OBJ				: Object state error
@retval	D_OS_USER_E_QOVR			: Queuing Overflow
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Wup_Tsk(OS_USER_ID taskid);

/**
This function is wrapper function of sus_tsk().<br>
It suspend task.<br>
@param [in]		taskid				: Task ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_OBJ				: Object state error
@retval	D_OS_USER_E_CTX				: Context Error
@retval	D_OS_USER_E_QOVR			: Queuing Overflow
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Sus_Tsk(OS_USER_ID taskid);

/**
This function is wrapper function of rsm_tsk().<br>
It resume task.<br>
@param [in]		taskid				: Task ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_OBJ				: Object state error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Rsm_Tsk(OS_USER_ID taskid);

/**
This function is wrapper function of frsm_tsk().<br>
It force resume task.<br>
@param [in]		taskid				: Task ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_OBJ				: Object state error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Frsm_Tsk(OS_USER_ID taskid);

/**
This function is wrapper function of dly_tsk().<br>
It delays task. <br>
@param [in]		delaytime			: Delay time
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_CTX				: Context error
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Wait released by wait ban
*/
OS_USER_ER OS_User_Dly_Tsk(OS_USER_RELTIM delaytime);


/*-Semaphore Function---------------------------------------------------*/
/**
This function is wrapper function of acre_sem().<br>
It creates semaphore.<br>
Semaphore ID is assigned automatically.<br>
@param [in]		pk_csem				: Pointer of Semaphore create information structure
@retval	PositiveValue				: Semaphore ID
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ER_ID OS_User_Acre_Sem(T_OS_USER_CSEM* pk_csem);

/**
This function is wrapper function of del_sem().<br>
It deletes Semaphore.<br>
@param [in]		semid				: Semaphore ID
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Del_Sem(OS_USER_ID semid);

/**
This function is wrapper function of sig_sem().<br>
It returns resource.<br>
@param [in]		semid				: Semaphore ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered semaphore
@retval	D_OS_USER_E_QOVR			: Semaphore count overflow
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Sig_Sem(OS_USER_ID semid);

/**
This function is wrapper function of twai_sem().<br>
It gets(with timeout) semaphore resource.<br>
@param [in]		semid				: Semaphore ID
@param [in]		tmout				: Time out value
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered semaphore
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_TMOUT			: Time out
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Twai_Sem(OS_USER_ID semid, OS_USER_TMO tmout);

/**
This function is wrapper function of wai_sem().<br>
It gets semaphore resource.<br>
@param [in]		semid				: Semaphore ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered semaphore
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Wai_Sem(OS_USER_ID semid);

/**
This function is wrapper function of pol_sem().<br>
It gets semaphore resource by polling.<br>
@param [in]		semid				: Semaphore ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered semaphore
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_TMOUT			: Time out
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Pol_Sem(OS_USER_ID semid);

/**
This function is wrapper function of ref_sem().<br>
It gets semaphore state information.<br>
@param [in]		semid				: Semaphore ID
@param [out]	pk_rsm				: Pointer of semaphore state information structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered semaphore
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Ref_Sem(OS_USER_ID semid, T_OS_USER_RSEM* pk_rsm);


/*-Event Flag Function---------------------------------------------------*/
/**
This function is wrapper function of acre_flg().<br>
It event flag.<br>
Event Flag ID is assigned automatically.<br>
@param [in]		pk_cflg				: Pointer of Event flag create information structure
@retval	PositiveValue				: Event Flag ID
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ER_ID OS_User_Acre_Flg(T_OS_USER_CFLG* pk_cflg);

/**
This function is wrapper function of del_flg().<br>
It deletes Event Flag.<br>
@param [in]		flgid				: Event Flag ID
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Del_Flg(OS_USER_ID flgid);

/**
This function is wrapper function of set_flg().<br>
It sets(with timeout) event flag.<br>
@param [in]		flagid				: Event flag ID
@param [in]		setptn				: Bit pattern setting
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered event flag
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Set_Flg(OS_USER_ID flagid, OS_USER_FLGPTN setptn);

/**
This function is wrapper function of clr_flg().<br>
It sets(with timeout) event flag.<br>
@param [in]		flagid				: Event flag ID
@param [in]		clrptn				: Clear Bit pattern
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered event flag
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Clr_Flg(OS_USER_ID flagid, OS_USER_FLGPTN clrptn);

/**
This function is wrapper function of wai_flg().<br>
It waits event flag.<br>
@param [in]		flagid				: Event flag ID
@param [in]		waiptn				: Wait bit pattern
@param [in]		wfmode				: Wait mode
@param [out]	p_flgptn			: Pointer of bit pattern at the time of release
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Event flag ID range error
@retval	D_OS_USER_E_NOEXS			: Unregistered event flag
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_DLT				: Delete Wait Object
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval D_OS_USER_E_DISWAI			: Wait released by wait ban
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Wai_Flg(OS_USER_ID flagid, OS_USER_FLGPTN waiptn, OS_USER_MODE wfmode, OS_USER_FLGPTN* p_flgptn);

/**
This function is wrapper function of pol_flg().<br>
Polls eventflag wait cancel conditions.<br>
@param [in]		flagid				: Event flag ID
@param [in]		waiptn				: Wait bit pattern
@param [in]		wfmode				: Wait mode
@param [out]	p_flgptn			: Pointer of bit pattern at the time of release
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Event flag ID range error
@retval	D_OS_USER_E_NOEXS			: Unregistered event flag
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_DLT				: Delete Wait Object
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval D_OS_USER_E_DISWAI			: Wait released by wait ban
@retval D_OS_USER_E_TMOUT			: Polling Error or Timeout
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Pol_Flg(OS_USER_ID flagid, OS_USER_FLGPTN waiptn, OS_USER_MODE wfmode, OS_USER_FLGPTN* p_flgptn);

/**
This function is wrapper function of twai_flg().<br>
It waits(with timeout) event flag.<br>
@param [in]		flagid				: Event flag ID
@param [in]		waiptn				: Wait bit pattern
@param [in]		wfmode				: Wait mode
@param [out]	p_flgptn			: Pointer of bit pattern at the time of release
@param [in]		timeout				: Time out value
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Event flag ID range error
@retval	D_OS_USER_E_NOEXS			: Unregistered event flag
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_DLT				: Delete Wait Object
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval D_OS_USER_E_DISWAI			: Wait released by wait ban
@retval D_OS_USER_E_TMOUT			: Polling Error or Timeout
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Twai_Flg(OS_USER_ID flagid, OS_USER_FLGPTN waiptn, OS_USER_MODE wfmode, OS_USER_FLGPTN* p_flgptn, OS_USER_TMO timeout);

/**
This function is wrapper function of ref_flg().<br>
It reference event flag.<br>
@param [in]		flagid				: Event flag ID
@param [in]		pk_rflg				: Event flag state's pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered event flag
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Ref_Flg(OS_USER_ID flagid, T_OS_USER_RFLG* pk_rflg);

/*-Mailbox Function-----------------------------------------------------*/
/**
This function is wrapper function of acre_mbx().<br>
It creates Mailbox.<br>
Mailbox ID is assigned automatically.<br>
@param [in]		pk_cmbx				: Pointer of Mailbox create information structure
@retval	PositiveValue				: Mailbox ID
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ER_ID OS_User_Acre_Mbx(T_OS_USER_CMBX* pk_cmbx);

/**
This function is wrapper function of del_mbx().<br>
It deletes Mailbox.<br>
@param [in]		mbxid				: Mailbox ID
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Del_Mbx(OS_USER_ID mbxid);

/**
This function is wrapper function of snd_mbx().<br>
It sends message to mailbox.<br>
@param [in]		mbxid				: Mailbox ID
@param [out]	pk_msg				: Packet of message structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Snd_Mbx(OS_USER_ID mbxid, T_OS_USER_MSG* pk_msg);

/**
This function is wrapper function of trcv_mbx().<br>
It receives(with timeout) message from mailbox.<br>
@param [in]		mbxid				: Mailbox ID
@param [out]	ppk_msg				: Pointer of packet of message structure
@param [in]		tmout				: Timeout value
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Wait released by wait ban
@retval	D_OS_USER_E_RLWAI			: Releases the waiting state
@retval	D_OS_USER_E_DISWAI			: Releases the waiting state
@retval	D_OS_USER_E_TMOUT			: Timeout
@retval	D_OS_USER_E_CTX				: Context error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Trcv_Mbx(OS_USER_ID mbxid, T_OS_USER_MSG** ppk_msg, OS_USER_TMO tmout);

/**
This function is wrapper function of rcv_mbx().<br>
Receives messages from mailboxes.<br>
@param [in]		mbxid				: Mailbox ID
@param [out]	ppk_msg				: Pointer of packet of message structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Wait released by wait ban
@retval	D_OS_USER_E_RLWAI			: Releases the waiting state
@retval	D_OS_USER_E_DISWAI			: Releases the waiting state
@retval	D_OS_USER_E_TMOUT			: Timeout
@retval	D_OS_USER_E_CTX				: Context error
@retval	D_OS_USER_E_DACV			: Access Protection Error

*/
OS_USER_ER OS_User_Rcv_Mbx(OS_USER_ID mbxid, T_OS_USER_MSG** ppk_msg);

/**
This function is wrapper function of prcv_mbx().<br>
It polls and receives message from mailbox.<br>
@param [in]		mbxid				: Mailbox ID
@param [out]	ppk_msg				: Pointer of packet of message structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_TMOUT			: Timeout
*/
OS_USER_ER OS_User_Prcv_Mbx(OS_USER_ID mbxid, T_OS_USER_MSG** ppk_msg);

/**
This function is wrapper function of ref_mbx().<br>
It refers mailbox status.<br>
@param [in]		mbxid				: Mailbox ID
@param [out]	pk_rmbx				: Packet to refer mailbox structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Ref_Mbx(OS_USER_ID mbxid, T_OS_USER_RMBX* pk_rmbx);

/*-Mutex Function-------------------------------------------------------*/
#if 0
/**
This function is wrapper function of cre_mtx().<br>
It creates mutex.<br>
@param [in]		mtxid				: Mutex ID
@param [in]		pk_cmtx				: Packet of mutex creation information structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_PAR				: Parameter Errors
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_OBJ				: Object status is error
*/
OS_USER_ER OS_User_Cre_Mtx(OS_USER_ID mtxid, T_OS_USER_CMTX* pk_cmtx);
#endif

/**
This function is wrapper function of cre_mtx().<br>
It creates mutex.<br>
@param [in]		pk_cmtx				: Packet of mutex creation information structure
@retval	PositiveValue				: Mutex ID
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ID OS_User_Acre_Mtx(T_OS_USER_CMTX* pk_cmtx);

/**
This function is wrapper function of del_mtx().<br>
It deletes Mutex.<br>
@param [in]		mtxid				: Mutex ID
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Del_Mtx(OS_USER_ID mtxid);

/**
This function is wrapper function of loc_mtx().<br>
It locks mutex.<br>
@param [in]		mtxid				: Mutex ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Deletes waiting objects
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Context error
@retval	D_OS_USER_E_ILUSE			: Illegal use of service call
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Loc_Mtx(OS_USER_ID mtxid);

/**
This function is wrapper function of tloc_mtx().<br>
It locks mutex.<br>
@param [in]		mtxid				: Mutex ID
@param [in]		tmout				: Timeout value
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Deletes waiting objects
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_TMOUT			: Time out
@retval	D_OS_USER_E_CTX				: Context error
@retval	D_OS_USER_E_ILUSE			: Illegal use of service call
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Tloc_Mtx(OS_USER_ID mtxid, OS_USER_TMO tmout);

/**
This function is wrapper function of unl_mtx().<br>
It unlocks mutex.<br>
@param [in]		mtxid				: Mutex ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_ILUSE			: Illegal use of service call
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Unl_Mtx(OS_USER_ID mtxid);

/**
This function is wrapper function of ref_mtx().<br>
It gets mutex state information.<br>
@param [in]		semid				: Mutex ID
@param [out]	pk_rmtx				: Pointer of mutex state information structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered semaphore
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Ref_Mtx(OS_USER_ID semid, T_OS_USER_RMTX* pk_rmtx);

/*-MessageBuffer Function-----------------------------------------------*/
/**
This function is wrapper function of cre_mbf().<br>
It creates MessageBuffer.<br>
@param [in]		pk_cmbf				: Packet of MessageBuffer creation information structure
@retval	PositiveValue				: MessageBuffer ID
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ID OS_User_Acre_Mbf(T_OS_USER_CMBF* pk_cmbf);

/**
This function is wrapper function of del_mbf().<br>
It deletes MessageBuffer.<br>
@param [in]		mbfid				: MessageBuffer ID
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Del_Mbf(OS_USER_ID mbfid);

/**
This function is wrapper function of psnd_mbf().<br>
It sends(with timeout) message to message buffer.<br>
@param [in]		mbfid				: Message buffer ID
@param [in]		msg					: Pointer of message structure
@param [in]		msgsz				: Bytes of message structure
@param [in]		tmout				: Timeout value
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Deletes waiting objects
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_TMOUT			: Time out
@retval	D_OS_USER_E_CTX				: Context error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Tsnd_Mbf(OS_USER_ID mbfid, OS_USER_VP msg, UINT32 msgsz, OS_USER_TMO tmout);

/**
This function is wrapper function of psnd_mbf().<br>
It sends message to message buffer.<br>
@param [in]		mbfid				: Message buffer ID
@param [in]		msg					: Pointer of message structure
@param [in]		msgsz				: Bytes of message structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Deletes waiting objects
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Context error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Snd_Mbf(OS_USER_ID mbfid, OS_USER_VP msg, UINT32 msgsz);

/**
This function is wrapper function of psnd_mbf().<br>
It polls and sends message to message buffer.<br>
@param [in]		mbfid				: Message buffer ID
@param [in]		msg					: Pointer of message structure
@param [in]		msgsz				: Bytes of message structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Deletes waiting objects
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Context error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Psnd_Mbf(OS_USER_ID mbfid, OS_USER_VP msg, UINT32 msgsz);

/**
This function is wrapper function of trcv_mbf().<br>
It polls and receives message from message buffer.<br>
@param [in]		mbfid				: Message buffer ID
@param [out]	msg					: Pointer of message structure
@param [in]		tmout				: Time out value
@retval	PositiveValue				: Receive Message Size
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Deletes waiting objects
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_TMOUT			: Time out
@retval	D_OS_USER_E_CTX				: Context error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Trcv_Mbf(OS_USER_ID mbfid, OS_USER_VP msg, OS_USER_TMO tmout);

/**
This function is wrapper function of rcv_mbf().<br>
It polls and receives message from message buffer.<br>
@param [in]		mbfid				: Message buffer ID
@param [out]	msg					: Pointer of message structure
@retval	PositiveValue				: Receive Message Size
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Deletes waiting objects
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Context error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER_UINT OS_User_Rcv_Mbf(OS_USER_ID mbfid, OS_USER_VP msg);

/**
This function is wrapper function of prcv_mbf().<br>
It polls and receives message from message buffer.<br>
@param [in]		mbfid				: Message buffer ID
@param [out]	msg					: Pointer of message structure
@retval	PositiveValue				: Receive Message Size
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Deletes waiting objects
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Context error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER_UINT OS_User_Prcv_Mbf(OS_USER_ID mbfid, OS_USER_VP msg);

/**
This function is wrapper function of ref_mbf().<br>
It polls and receives message from message buffer.<br>
@param [in]		mbfid				: Message buffer ID
@param [out]	pk_rmbf				: Pointer of message buffer status
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Ref_Mbf(OS_USER_ID mbfid, T_OS_USER_RMBF* pk_rmbf);

/*-Fixed-sized memory pool Function-------------------------------------*/
/**
This function is wrapper function of cre_mbf().<br>
It creates Fixed-size MemoryPool.<br>
@param [in]		pk_cmpf				: Packet of Fixed-size MemoryPool creation information structure
@retval	PositiveValue				: Fixed-size MemoryPool ID
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ID OS_User_Acre_Mpf(T_OS_USER_CMPF* pk_cmpf);

/**
This function is wrapper function of del_mpf().<br>
It deletes Fixed-size MemoryPool.<br>
@param [in]		mpfid				: Fixed-size MemoryPool ID
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Del_Mpf(OS_USER_ID mpfid);

/**
This function is wrapper function of get_mpf().<br>
It gets fixed-size memory block.<br>
@param [in]		mpfid				: Fixed-size Memory Pool ID
@param [out]	p_blk				: Pointer of block start address
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered semaphore
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Get_Mpf(OS_USER_ID mpfid, OS_USER_VP* p_blk);

/**
This function is wrapper function of pget_mpf().<br>
It gets(with timeout) fixed-size memory block.<br>
@param [in]		mpfid				: Fixed-size Memory Pool ID
@param [out]	p_blk				: Pointer of block start address
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered semaphore
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Pget_Mpf(OS_USER_ID mpfid, OS_USER_VP* p_blk);

/**
This function is wrapper function of tget_mpf().<br>
It gets(with timeout) fixed-size memory block.<br>
@param [in]		mpfid				: Fixed-size Memory Pool ID
@param [out]	p_blk				: Pointer of block start address
@param [in]		tmout				: Timeout value
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered semaphore
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_TMOUT			: Time out
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Tget_Mpf(OS_USER_ID mpfid, OS_USER_VP* p_blk, OS_USER_TMO tmout);

/**
This function is wrapper function of rel_mpf().<br>
It releases fixed-size memory block.<br>
@param [in]		mpfid				: Fixed-size Memory Pool ID
@param [in]		blk					: Block start address
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Rel_Mpf(OS_USER_ID mpfid, OS_USER_VP blk);

/**
This function is wrapper function of ref_mpf().<br>
It refers fixed-size memory pool status.<br>
@param [in]		mpfid				: Fixed-size Memory Pool ID
@param [out]	pk_rmpf				: Packet to refer memory pool structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Ref_Mpf(OS_USER_ID mpfid, T_OS_USER_RMPF* pk_rmpf);

/*-Variable-sized memory pool Function----------------------------------*/
/**
This function is wrapper function of cre_mpl().<br>
It creates Variable-sized MemoryPool.<br>
@param [in]		pk_cmpl				: Packet of Variable-sized MemoryPool creation information structure
@retval	PositiveValue				: Variable-sized MemoryPool ID
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOSPT			: Coprocessor Unusable
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ID OS_User_Acre_Mpl(T_OS_USER_CMPL* pk_cmpl);

/**
This function is wrapper function of del_mpl().<br>
It deletes Variable-sized MemoryPool.<br>
@param [in]		mplid				: Variable-sized MemoryPool ID
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Del_Mpl(OS_USER_ID mplid);

/**
This function is wrapper function of get_mpl().<br>
It gets Variable-sized memory block.<br>
@param [in]		mplid				: Variable-sized Memory Pool ID
@param [in]		blksz				: Memory block size
@param [out]	p_blk				: Pointer of block start address
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Get_Mpl(OS_USER_ID mplid, OS_USER_SIZE blksz, OS_USER_VP* p_blk);

/**
This function is wrapper function of pget_mpl().<br>
It poll and gets variable-sized memory block.<br>
@param [in]		mplid				: Variable-sized Memory Pool ID
@param [in]		blksz				: Memory block size
@param [out]	p_blk				: Pointer of block start address
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Pget_Mpl(OS_USER_ID mplid, OS_USER_SIZE blksz, OS_USER_VP* p_blk);

/**
This function is wrapper tunction of tget_mpl().<br>
It gets(with timeout) variable-sized memory block.<br>
@param [in]		mplid				: Variable-sized Memory Pool ID
@param [in]		blksz				: Memory block size
@param [out]	p_blk				: Pointer of block start address
@param [in]		tmout				: Timeout value
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_TMOUT			: Time out
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Tget_Mpl(OS_USER_ID mplid, OS_USER_SIZE blksz, OS_USER_VP* p_blk, OS_USER_TMO tmout);

/**
This function is wrapper function of rel_mpl().<br>
It releases variable-size memory block.<br>
@param [in]		mplid				: Variable-sized Memory Pool ID
@param [in]		blk					: Block start address
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Rel_Mpl(OS_USER_ID mplid, OS_USER_VP blk);

/**
This function is wrapper function of ref_mpl().<br>
It refers variable-size memory pool status.<br>
@param [in]		mplid				: Variable-sized Memory Pool ID
@param [out]	pk_rmpl				: Packet to refer memory pool structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Ref_Mpl(OS_USER_ID mplid, T_OS_USER_RMPL* pk_rmpl);

/*-Cyclic handler Function----------------------------------------------*/
/**
This function is wrapper function of acre_cyc().<br>
It creates cyclic handler.<br>
Cyclic handler ID is assigned automatically.
@param [in]		pk_ccyc				: Pointer of cyclic handler create information structure
@retval	PositiveValue				: Cyclic handler ID
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ER_ID OS_User_Acre_Cyc(T_OS_USER_CCYC* pk_ccyc);

/**
This function is wrapper function of del_cyc().<br>
It deletes cyclic handler.<br>
@param [in]		cycid				: Cyclic handler ID
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Del_Cyc(OS_USER_ID cycid);

/**
This function is wrapper function of sta_cyc().<br>
It operates cyclic handler.<br>
@param [in]		cycid				: Cyclic handler ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered cyclic handler
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Sta_Cyc(OS_USER_ID cycid);

/**
This function is wrapper function of stp_cyc().<br>
It stops cyclic handler.<br>
@param [in]		cycid				: Cyclic handler ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered cyclic handler
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Stp_Cyc(OS_USER_ID cycid);

/**
This function is wrapper function of ref_cyc().<br>
It refers cyclic handler status.<br>
@param [in]		cycid				: Cyclic handler ID
@param [out]	pk_rcyc				: Packet to refer Cyclic handler structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Ref_Cyc(OS_USER_ID cycid, T_OS_USER_RCYC* pk_rcyc);

/*-Alarm Function-------------------------------------------------------*/
/**
This function is wrapper function of acre_alm().<br>
It creates alarm handler.<br>
Alarm handler ID is assigned automatically.
@param [in]		almid				: Alarm handler ID
@param [in]		pk_calm				: Pointer of alarm handler create information structure
@retval	PositiveValue				: Alarm handler ID
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ER OS_User_Acre_Alm(OS_USER_ID almid, T_OS_USER_CALM *pk_calm);

/**
This function is wrapper function of del_alm().<br>
It deletes alarm handler.<br>
@param [in]		almid				: Alarm handler ID
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Del_Alm(OS_USER_ID almid);

/**
This function is wrapper function of sta_alm().<br>
It operates alarm handler.<br>
@param [in]		almid				: Alarm handler ID
@param [in]		almtim				: Alarm handler Start-up time
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered cyclic handler
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Sta_Alm(OS_USER_ID almid, OS_USER_RELTIM almtim);

/**
This function is wrapper function of stp_alm().<br>
It stops alarm handler.<br>
@param [in]		almid				: Alarm handler ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered cyclic handler
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Stp_Alm(OS_USER_ID almid);

/**
This function is wrapper function of ref_alm().<br>
It refers alarm handler status.<br>
@param [in]		almid				: Alarm handler ID
@param [out]	pk_ralm				: Packet to refer Alarm handler structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Ref_Alm(OS_USER_ID almid, T_OS_USER_RALM *pk_ralm);

/*-Port Function-------------------------------------------------------*/
/**
This function is wrapper function of acre_por().<br>
It creates Port for Rendezvous.<br>
Port ID is assigned automatically.
@param [in]		pk_cpor				: Pointer of Port create information structure
@retval	PositiveValue				: Port ID
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ER OS_User_Acre_Por(T_OS_USER_CPOR *pk_cpor);

/**
This function is wrapper function of del_por().<br>
It deletes Port for Rendezvous.<br>
@param [in]		porid				: Port handler ID
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Del_Por(OS_USER_ID porid);

/**
This function is wrapper function of cal_por().<br>
It calls Port for Rendezvous.<br>
@param [in]		porid				: Port handler ID
@param [in]		calptn				: Call Bit Pattern
@param [in]		msg					: Message
@param [in]		cmsgsz				: Call Message Size
@retval	PositiveValue				: Reply Message Size
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Cal_Por(OS_USER_ID porid, OS_USER_UINT calptn, OS_USER_VP msg, OS_USER_INT cmsgsz);

/**
This function is wrapper function of pcal_por().<br>
It poll and calls Port for Rendezvous.<br>
@param [in]		porid				: Port handler ID
@param [in]		calptn				: Call Bit Pattern
@param [in]		msg					: Message
@param [in]		cmsgsz				: Call Message Size
@retval	PositiveValue				: Reply Message Size
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Pcal_Por(OS_USER_ID porid, OS_USER_UINT calptn, OS_USER_VP msg, OS_USER_INT cmsgsz);

/**
This function is wrapper tunction of tcal_por().<br>
It calls(with timeout) Port for Rendezvous.<br>
@param [in]		porid				: Port handler ID
@param [in]		calptn				: Call Bit Pattern
@param [in]		msg					: Message
@param [in]		cmsgsz				: Call Message Size
@param [in]		tmout				: Timeout value
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_TMOUT			: Time out
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Tcal_Por(OS_USER_ID porid, OS_USER_UINT calptn, OS_USER_VP msg, OS_USER_INT cmsgsz, OS_USER_TMO tmout);

/**
This function is wrapper function of acp_por().<br>
It accept Port for Rendezvous.<br>
@param [in]		porid				: Port handler ID
@param [in]		acpptn				: Accept Bit Pattern
@param [in]		p_rdvno				: Pointer to Rendezvous Number
@param [in]		msg					: Message
@retval	PositiveValue				: Reply Message Size
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Acp_Por(OS_USER_ID porid, OS_USER_UINT acpptn, OS_USER_RNO *p_rdvno, OS_USER_VP msg);

/**
This function is wrapper function of pacp_por().<br>
It poll and accept Port for Rendezvous.<br>
@param [in]		porid				: Port handler ID
@param [in]		acpptn				: Accept Bit Pattern
@param [in]		p_rdvno				: Pointer to Rendezvous Number
@param [in]		msg					: Message
@retval	PositiveValue				: Reply Message Size
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Pacp_por(OS_USER_ID porid, OS_USER_UINT acpptn, OS_USER_RNO *p_rdvno, OS_USER_VP msg);

/**
This function is wrapper tunction of tacp_por().<br>
It accept(with timeout) Port for Rendezvous.<br>
@param [in]		porid				: Port handler ID
@param [in]		acpptn				: Accept Bit Pattern
@param [in]		p_rdvno				: Pointer to Rendezvous Number
@param [in]		msg					: Message
@param [in]		tmout				: Timeout value
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DLT				: Delete semaphore
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_TMOUT			: Time out
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Tacp_por(OS_USER_ID porid, OS_USER_UINT acpptn, OS_USER_RNO *p_rdvno, OS_USER_VP msg, OS_USER_TMO tmout);

/**
This function is wrapper function of fwd_por().<br>
It Forward Rendezvous to Other Port.<br>
@param [in]		porid				: Port handler ID
@param [in]		calptn				: Call Bit Pattern
@param [in]		rdvno				: Rendezvous Number
@param [in]		msg					: Message
@param [in]		cmsgsz				: CallMessageSize
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Fwd_Por(OS_USER_ID porid, OS_USER_UINT calptn, OS_USER_RNO rdvno, OS_USER_VP msg, OS_USER_INT cmsgsz);

/*-Domain Function-------------------------------------------------------*/
/**
This function is wrapper function of acre_dom().<br>
It creates Domain.<br>
Port ID is assigned automatically.
@param [in]		pk_cdom				: Pointer of Domain create information structure
@retval	PositiveValue				: Domain ID
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ER OS_User_Acre_Dom(T_OS_USER_CDOM *pk_cdom);

/**
This function is wrapper function of del_dom().<br>
It deletes Domain.<br>
@param [in]		domid				: Domain ID
@retval	D_OS_USER_E_OK				: Normal End
@retval	D_OS_USER_E_ID				: ID error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Del_Dom(OS_USER_ID domid);

/**
This function is wrapper function of get_kdm().<br>
It get kernel domain id.<br>
@param [in]		prcid				: Processor ID
@retval	PositiveValue				: Kernel Domein ID
@retval	D_OS_USER_E_ID				: Illegal ID number
*/
OS_USER_ID OS_User_Get_Kdm(OS_USER_ID prcid);

/**
This function is wrapper function of ref_dom().<br>
It refer domain status.<br>
@param [in]		domid				: Domain ID
@param [out]	pk_rdom				: Packet to Refer Domain
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER OS_User_Ref_Dom(OS_USER_ID domid, T_OS_USER_RDOM *pk_rdom);

/*-Other Function-------------------------------------------------------*/
/**
This function is wrapper function of chg_pri().<br>
It changes task priority.<br>
@param [in]		taskid				: Task ID
@param [in]		pri					: Task priority
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_ILUSE			: Unauthorized use of service call
@retval	D_OS_USER_E_DACV			: Access Protection Error

*/
OS_USER_ER OS_User_Chg_Pri(OS_USER_ID taskid, OS_USER_PRI pri);

/**
This function is wrapper function of chg_slt().<br>
It changes task slicetime.<br>
@param [in]		taskid				: Task ID
@param [in]		slicetime			: Slice Time(ms)
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error

*/
OS_USER_ER OS_User_Chg_Slt(OS_USER_ID taskid, OS_USER_RELTIM slicetime);

/**
This function is wrapper function of get_tsp().<br>
It get task space.<br>
@param [in]		taskid				: Task ID
@param [in]		pk_tskspc			: Packet of Task Space
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error

*/
OS_USER_ER OS_User_Get_Tsp(OS_USER_ID taskid, T_OS_USER_TSKSPC *pk_tskspc);

/**
This function is wrapper function of set_tsp().<br>
It set task space.<br>
@param [in]		taskid				: Task ID
@param [in]		pk_tskspc			: Packet of Task Space
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error

*/
OS_USER_ER OS_User_Set_Tsp(OS_USER_ID taskid, T_OS_USER_TSKSPC *pk_tskspc);

/**
This function is wrapper function of get_rid().<br>
It get resource id.<br>
@param [in]		taskid				: Task ID
@retval	PositiveValue				: Resource ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Get_Rid(OS_USER_ID taskid);

/**
This function is wrapper function of set_rid().<br>
It set resource id.<br>
@param [in]		taskid				: Task ID
@param [in]		resid				: New Resource ID
@retval	PositiveValue				: Old Resource ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Set_Rid(OS_USER_ID taskid, OS_USER_ID resid);

/**
This function is wrapper function of get_reg().<br>
It get task registers.<br>
@param [in]		taskid				: Task ID
@param [out]	pk_regs				: Packet of Registers
@param [out]	pk_eit				: Packet of EIT
@param [out]	pk_cregs			: Packet of Control Registers
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Get_Reg(OS_USER_ID taskid, T_OS_USER_REGS *pk_regs, T_OS_USER_EIT *pk_eit, T_OS_USER_CREGS *pk_cregs);

/**
This function is wrapper function of set_reg().<br>
It set task registers.<br>
@param [in]		taskid				: Task ID
@param [out]	pk_regs				: Packet of Registers
@param [out]	pk_eit				: Packet of EIT
@param [out]	*pk_cregs			: Packet of Control Registers
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Set_Reg(OS_USER_ID taskid, T_OS_USER_REGS *pk_regs, T_OS_USER_EIT *pk_eit, T_OS_USER_CREGS *pk_cregs);

/**
This function is wrapper function of get_cpr().<br>
It get task coprocessor registers.<br>
@param [in]		taskid				: Task ID
@param [in]		copno				: Coprocessor Number
@param [out]	pk_copregs			: Packet of Coprocessor Number
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Get_Cpr(OS_USER_ID taskid, OS_USER_INT copno, T_OS_USER_COPREGS* pk_copregs);

/**
This function is wrapper function of set_cpr().<br>
It set task coprocessor registers.<br>
@param [in]		taskid				: Task ID
@param [in]		copno				: Coprocessor Number
@param [out]	pk_copregs			: Packet of Coprocessor Number
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_CTX				: Non task context
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Set_Cpr(OS_USER_ID taskid, OS_USER_INT copno, T_OS_USER_COPREGS* pk_copregs);

/**
This function is wrapper function of can_wup().<br>
It cancel wakeup task.<br>
@param [in]		taskid				: Task ID
@retval	PositiveValue				: Wakeup Count
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Can_Wup(OS_USER_ID taskid);

/**
This function is wrapper function of rel_wai().<br>
It release wait.<br>
@param [in]		taskid				: Task ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Rel_Wai(OS_USER_ID taskid);

/**
This function is wrapper function of rpl_rdv().<br>
It reply rendezvous.<br>
@param [in]		rdvno				: Rendezvous Number
@param [in]		msg					: Reply Message
@param [in]		rmsgsz				: Reply Message Size
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_CTX				: Non task context
*/
OS_USER_ER OS_User_Rpl_Rdv(OS_USER_RNO rdvno, OS_USER_VP msg, OS_USER_INT rmsgsz);

/**
This function is wrapper function of ref_por().<br>
It refer port status.<br>
@param [in]		porid				: Port ID
@param [out]	pk_rpor				: Packet to Refer Port
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Ref_Por(OS_USER_ID porid, T_OS_USER_RPOR* pk_rpor);

/**
This function is wrapper function of sig_tev().<br>
It signal task event.<br>
@param [in]		taskid				: Task ID
@param [in]		tskevt				: Task Event Number
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Sig_Tev(OS_USER_ID taskid, OS_USER_INT tskevt);

/**
This function is wrapper function of wai_tev().<br>
It wait task event.<br>
@param [in]		waiptn				: Task Event Pattern
@retval	PositiveValue				: Wakeup Count
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Non task context
*/
OS_USER_ER OS_User_Wai_Tev(OS_USER_INT waiptn);

/**
This function is wrapper function of pwai_tev().<br>
It wait task event.<br>
@param [in]		waiptn				: Task Event Pattern
@retval	PositiveValue				: Wakeup Count
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_CTX				: Non task context
*/
OS_USER_ER OS_User_Pwai_Tev(OS_USER_INT waiptn);

/**
This function is wrapper function of twai_tev().<br>
It wait task event.<br>
@param [in]		waiptn				: Task Event Pattern
@param [in]		tmout				: Timeout value
@retval	PositiveValue				: Wakeup Count
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_RLWAI			: Release wait mode
@retval	D_OS_USER_E_DISWAI			: Delete Wait Object
@retval	D_OS_USER_E_TMOUT			: Time out
@retval	D_OS_USER_E_CTX				: Non task context
*/
OS_USER_ER OS_User_Twai_Tev(OS_USER_INT waiptn, OS_USER_TMO tmout);

/**
This function is wrapper function of dis_wai().<br>
It disable task wait.<br>
@param [in]		taskid				: Task ID
@param [in]		waitmask			: Task wait disabled setting
@retval	PositiveValue				: Task status after task wait disable
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Dis_Wai(OS_USER_ID taskid, OS_USER_UINT waitmask);

/**
This function is wrapper function of ena_wai().<br>
It enable task wait.<br>
@param [in]		taskid				: Task ID
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_DACV			: Access Protection Error
*/
OS_USER_ER OS_User_Ena_Wai(OS_USER_ID taskid);

/**
This function is wrapper function of set_tim().<br>
It set system time.<br>
@param [in]		p_systime			: Pointer of system time
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER OS_User_Set_Tim(OS_USER_SYSTIM* p_systime);

/**
This function is wrapper function of get_tim().<br>
It get system time.<br>
@param [out]	p_systime			: Pointer of system time
@retval	D_OS_USER_E_OK				: Success
*/
OS_USER_ER OS_User_Get_Tim(OS_USER_SYSTIM* p_systime);

/**
This function is wrapper function of get_otm().<br>
It get operating time.<br>
@param [out]	p_systime			: Pointer of operating time
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER OS_User_Get_Otm(OS_USER_SYSTIM* p_systime);

/**
This function is wrapper function of def_int().<br>
It define interrupt handler
@param [in]		dintno				: Interrupt Define Number
@param [in]		pk_dint				: Packet to Define Interrupt Handler
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_NOMEM			: No Memory
@retval	D_OS_USER_E_RSATR			: Reserve Attribute
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER OS_User_Def_Int(OS_USER_INTID dintno, T_OS_USER_DINT *pk_dint);

/**
This function is wrapper function of ret_int().<br>
It return from interrupt handler
*/
VOID OS_User_Ret_Int(VOID);

/**
This function is wrapper function of rot_rdq().<br>
It rotate ready queue
@param [in]		tskpri				: Task Priority
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER OS_User_Rot_Rdq(OS_USER_PRI tskpri);

/**
This function is wrapper function of get_tid().<br>
It gets task identifier.<br>
@param [in]		p_tskid				: Pointer of task ID
@retval	D_OS_USER_E_OK				: Success
*/
OS_USER_ER OS_User_Get_Tid(OS_USER_ID* p_tskid);

/**
This function is wrapper function of dis_dsp().<br>
It shifts to dispatch disabled state.<br>
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_CTX				: Context error
*/
OS_USER_ER OS_User_Dis_Dsp(VOID);

/**
This function is wrapper function of ena_dsp().<br>
It shifts to dispatch enabled state.<br>
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_CTX				: Context error
*/
OS_USER_ER OS_User_Ena_Dsp(VOID);

/**
This function is wrapper function of get_prc().<br>
It get processor identifier.<br>
@retval	PositiveValue				: Processor ID
*/
OS_USER_ID OS_User_Get_Prc(VOID);

/**
This function is wrapper function of ref_sys().<br>
It refers system status.<br>
@param [in]		pk_rsys				: Packet of refer system structure
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER OS_User_Ref_Sys(T_OS_USER_RSYS* pk_rsys);

/**
This function is wrapper function of ref_ver().<br>
It refers version status.<br>
@param [in]		pk_rver				: Packet to Refer Version
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER OS_User_Ref_Ver(T_OS_USER_RVER* pk_rver);


/*-tk_fnd_*** Function-------------------------------------------------------*/
/**
This function is wrapper function of fnd_dom().<br>
It find object id.<br>
@param [in]		domid				: Domain ID
@param [in]		oname				: Object Name
@retval	PositiveValue				: Domain ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Fnd_Dom(OS_USER_ID domid, OS_USER_UB* oname);

/**
This function is wrapper function of fnd_tsk().<br>
It find object id.<br>
@param [in]		domid				: Domain ID
@param [in]		oname				: Object Name
@retval	PositiveValue				: Task ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Fnd_Tsk(OS_USER_ID domid, OS_USER_UB* oname);

/**
This function is wrapper function of fnd_sem().<br>
It find object id.<br>
@param [in]		domid				: Domain ID
@param [in]		oname				: Object Name
@retval	PositiveValue				: Semaphore ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Fnd_Sem(OS_USER_ID domid, OS_USER_UB* oname);

/**
This function is wrapper function of fnd_flg().<br>
It find object id.<br>
@param [in]		domid				: Domain ID
@param [in]		oname				: Object Name
@retval	PositiveValue				: Event Flag ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Fnd_Flg(OS_USER_ID domid, OS_USER_UB* oname);

/**
This function is wrapper function of fnd_mbx().<br>
It find object id.<br>
@param [in]		domid				: Domain ID
@param [in]		oname				: Object Name
@retval	PositiveValue				: Mailbox ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Fnd_Mbx(OS_USER_ID domid, OS_USER_UB* oname);

/**
This function is wrapper function of fnd_mtx().<br>
It find object id.<br>
@param [in]		domid				: Domain ID
@param [in]		oname				: Object Name
@retval	PositiveValue				: Mutex ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Fnd_Mtx(OS_USER_ID domid, OS_USER_UB* oname);

/**
This function is wrapper function of fnd_mbf().<br>
It find object id.<br>
@param [in]		domid				: Domain ID
@param [in]		oname				: Object Name
@retval	PositiveValue				: Message buffer ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Fnd_Mbf(OS_USER_ID domid, OS_USER_UB* oname);

/**
This function is wrapper function of fnd_por().<br>
It find object id.<br>
@param [in]		domid				: Domain ID
@param [in]		oname				: Object Name
@retval	PositiveValue				: Port ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Fnd_Por(OS_USER_ID domid, OS_USER_UB* oname);

/**
This function is wrapper function of fnd_mpf().<br>
It find object id.<br>
@param [in]		domid				: Domain ID
@param [in]		oname				: Object Name
@retval	PositiveValue				: Fixed-sized memory pool ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Fnd_Mpf(OS_USER_ID domid, OS_USER_UB* oname);

/**
This function is wrapper function of fnd_mpl().<br>
It find object id.<br>
@param [in]		domid				: Domain ID
@param [in]		oname				: Object Name
@retval	PositiveValue				: Variable-sized memory pool ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Fnd_Mpl(OS_USER_ID domid, OS_USER_UB* oname);

/**
This function is wrapper function of fnd_alm().<br>
It find object id.<br>
@param [in]		domid				: Domain ID
@param [in]		oname				: Object Name
@retval	PositiveValue				: Alarm ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Fnd_Alm(OS_USER_ID domid, OS_USER_UB* oname);

/**
This function is wrapper function of fnd_cyc().<br>
It find object id.<br>
@param [in]		domid				: Domain ID
@param [in]		oname				: Object Name
@retval	PositiveValue				: Cyclic handler ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Fnd_Cyc(OS_USER_ID domid, OS_USER_UB* oname);

/*-tk_dmi_*** Function-------------------------------------------------------*/
/**
This function is wrapper function of dmi_dom().<br>
It domain information.<br>
@param [in]		domid				: Object ID
@param [in]		pk_dmi				: Packet to Domain Information
@retval	PositiveValue				: Domain ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Dmi_Dom(OS_USER_ID domid, T_OS_USER_DMI* pk_dmi);

/**
This function is wrapper function of dmi_tsk().<br>
It domain information.<br>
@param [in]		tskid				: Object ID
@param [in]		pk_dmi				: Packet to Domain Information
@retval	PositiveValue				: Task ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Dmi_Tsk(OS_USER_ID tskid, T_OS_USER_DMI* pk_dmi);

/**
This function is wrapper function of dmi_sem().<br>
It domain information.<br>
@param [in]		semid				: Object ID
@param [in]		pk_dmi				: Packet to Domain Information
@retval	PositiveValue				: Semaphore ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Dmi_Sem(OS_USER_ID semid, T_OS_USER_DMI* pk_dmi);

/**
This function is wrapper function of dmi_flg().<br>
It domain information.<br>
@param [in]		flgid				: Object ID
@param [in]		pk_dmi				: Packet to Domain Information
@retval	PositiveValue				: Event Flag ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Dmi_Flg(OS_USER_ID flgid, T_OS_USER_DMI* pk_dmi);

/**
This function is wrapper function of dmi_mbx().<br>
It domain information.<br>
@param [in]		mbxid				: Object ID
@param [in]		pk_dmi				: Packet to Domain Information
@retval	PositiveValue				: Mailbox ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Dmi_Mbx(OS_USER_ID mbxid, T_OS_USER_DMI* pk_dmi);

/**
This function is wrapper function of dmi_mtx().<br>
It domain information.<br>
@param [in]		mtxid				: Object ID
@param [in]		pk_dmi				: Packet to Domain Information
@retval	PositiveValue				: Mutex ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Dmi_Mtx(OS_USER_ID mtxid, T_OS_USER_DMI* pk_dmi);

/**
This function is wrapper function of dmi_mbf().<br>
It domain information.<br>
@param [in]		mbfid				: Object ID
@param [in]		pk_dmi				: Packet to Domain Information
@retval	PositiveValue				: Message buffer ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Dmi_Mbf(OS_USER_ID mbfid, T_OS_USER_DMI* pk_dmi);

/**
This function is wrapper function of dmi_por().<br>
It domain information.<br>
@param [in]		porid				: Object ID
@param [in]		pk_dmi				: Packet to Domain Information
@retval	PositiveValue				: Port ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Dmi_Por(OS_USER_ID porid, T_OS_USER_DMI* pk_dmi);

/**
This function is wrapper function of dmi_mpf().<br>
It domain information.<br>
@param [in]		mpfid				: Object ID
@param [in]		pk_dmi				: Packet to Domain Information
@retval	PositiveValue				: Fixed-sized memory pool ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Dmi_Mpf(OS_USER_ID mpfid, T_OS_USER_DMI* pk_dmi);

/**
This function is wrapper function of dmi_mpl().<br>
It domain information.<br>
@param [in]		mplid				: Object ID
@param [in]		pk_dmi				: Packet to Domain Information
@retval	PositiveValue				: Variable-sized memory pool ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Dmi_Mpl(OS_USER_ID mplid, T_OS_USER_DMI* pk_dmi);

/**
This function is wrapper function of dmi_alm().<br>
It domain information.<br>
@param [in]		almid				: Object ID
@param [in]		pk_dmi				: Packet to Domain Information
@retval	PositiveValue				: Alarm ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Dmi_Alm(OS_USER_ID almid, T_OS_USER_DMI* pk_dmi);

/**
This function is wrapper function of dmi_cyc().<br>
It domain information.<br>
@param [in]		cycid				: Object ID
@param [in]		pk_dmi				: Packet to Domain Information
@retval	PositiveValue				: Cyclic ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
@retval	D_OS_USER_E_NOCOP			: Coprocessor Unusable
@retval	D_OS_USER_E_PAR				: Parameter error
*/
OS_USER_ER_ID OS_User_Dmi_Cyc(OS_USER_ID cycid, T_OS_USER_DMI* pk_dmi);

/*-td_lst_*** Function-------------------------------------------------------*/
/**
This function is wrapper function of td_lst_tsk().<br>
It list of id.<br>
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Task ID
*/
OS_USER_ER_ID OS_User_Td_Lst_Tsk(OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_lst_sem().<br>
It list of id.<br>
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Semaphore ID
*/
OS_USER_ER_ID OS_User_Td_Lst_Sem(OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_lst_flg().<br>
It list of id.<br>
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Event Flag ID
*/
OS_USER_ER_ID OS_User_Td_Lst_Flg(OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_lst_mbx().<br>
It list of id.<br>
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Mailbox ID
*/
OS_USER_ER_ID OS_User_Td_Lst_Mbx(OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_lst_mtx().<br>
It list of id.<br>
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Mutex ID
*/
OS_USER_ER_ID OS_User_Td_Lst_Mtx(OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_lst_mbf().<br>
It list of id.<br>
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Message buffer ID
*/
OS_USER_ER_ID OS_User_Td_Lst_Mbf(OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_lst_por().<br>
It list of id.<br>
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Port ID
*/
OS_USER_ER_ID OS_User_Td_Lst_Por(OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_lst_mpf().<br>
It list of id.<br>
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Fixed-sized memory pool ID
*/
OS_USER_ER_ID OS_User_Td_Lst_Mpf(OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_lst_mpl().<br>
It list of id.<br>
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Variable-sized memory pool ID
*/
OS_USER_ER_ID OS_User_Td_Lst_Mpl(OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_lst_cyc().<br>
It list of id.<br>
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Cyclic handler ID
*/
OS_USER_ER_ID OS_User_Td_Lst_Cyc(OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_lst_alm().<br>
It list of id.<br>
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Alarm ID
*/
OS_USER_ER_ID OS_User_Td_Lst_Alm(OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_lst_dom().<br>
It list of id.<br>
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Domain ID
*/
OS_USER_ER_ID OS_User_Td_Lst_Dom(OS_USER_ID list[], OS_USER_INT nent);


/*-td_***_que Function-------------------------------------------------------*/
/**
This function is wrapper function of td_rdy_que().<br>
It Queue reference.<br>
@param [in]		pri					: Task Priority
@param [in]		list				: Area to Store the Task ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Task ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
*/
OS_USER_ER_ID OS_User_Td_Rdy_Que(OS_USER_PRI pri, OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_sem_que().<br>
It Queue reference.<br>
@param [in]		semid				: Semaphore ID
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Semaphore ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
*/
OS_USER_ER_ID OS_User_Td_Sem_Que(OS_USER_ID semid, OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_flg_que().<br>
It Queue reference.<br>
@param [in]		flgid				: Event Flag ID
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Event Flag ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
*/
OS_USER_ER_ID OS_User_Td_Flg_Que(OS_USER_ID flgid, OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_mbx_que().<br>
It Queue reference.<br>
@param [in]		mbxid				: Mailbox ID
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Mailbox ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
*/
OS_USER_ER_ID OS_User_Td_Mbx_Que(OS_USER_ID mbxid, OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_mtx_que().<br>
It Queue reference.<br>
@param [in]		mtxid				: Mutex ID
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Mutex ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
*/
OS_USER_ER_ID OS_User_Td_Mtx_Que(OS_USER_ID mtxid, OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_smbf_que().<br>
It Queue reference.<br>
@param [in]		mbfid				: Message Buffer of Send ID
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Message Buffer of Send ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
*/
OS_USER_ER_ID OS_User_Td_Smbf_Que(OS_USER_ID mbfid, OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_rmbf_que().<br>
It Queue reference.<br>
@param [in]		mbfid				: Message Buffer of Receive ID
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Message Buffer of Receive ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
*/
OS_USER_ER_ID OS_User_Td_Rmbf_Que(OS_USER_ID mbfid, OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_cal_que().<br>
It Queue reference.<br>
@param [in]		porid				: Rendezvous Call ID
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Rendezvous Call ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
*/
OS_USER_ER_ID OS_User_Td_Cal_Que(OS_USER_ID porid, OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_acp_que().<br>
It Queue reference.<br>
@param [in]		porid				: Rendezvous Accepted ID
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Rendezvous Accepted ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
*/
OS_USER_ER_ID OS_User_Td_Acp_Que(OS_USER_ID porid, OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_mpf_que().<br>
It Queue reference.<br>
@param [in]		mpfid				: Fixed-sized memory pool ID
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Fixed-sized memory pool ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
*/
OS_USER_ER_ID OS_User_Td_Mpf_Que(OS_USER_ID mpfid, OS_USER_ID list[], OS_USER_INT nent);

/**
This function is wrapper function of td_mpl_que().<br>
It Queue reference.<br>
@param [in]		mplid				: Variable-sized memory pool ID
@param [in]		list				: Area to Store the Object ID
@param [in]		nent				: Maximum Number Stored in List
@retval	PositiveValue				: Number of Variable-sized memory pool ID
@retval	D_OS_USER_E_ID				: ID Number Error
@retval	D_OS_USER_E_NOEXS			: Object Not Exist
*/
OS_USER_ER_ID OS_User_Td_Mpl_Que(OS_USER_ID mplid, OS_USER_ID list[], OS_USER_INT nent);


/*-td_ref_*** Function-------------------------------------------------------*/
/**
This function is wrapper function of td_ref_tsk().<br>
It state of task reference.<br>
@param [in]		taskid				: Task ID
@param [out]	rtsk				: Task State Information Pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Ref_Tsk(OS_USER_ID taskid, TD_OS_USER_RTSK* rtsk);

/**
This function is wrapper function of td_ref_tex().<br>
It state of task exception reference.<br>
@param [in]		taskid				: Task ID
@param [out]	rtex				: Task Exception State Information Pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Ref_Tex(OS_USER_ID taskid, TD_OS_USER_RTEX* rtex);

/**
This function is wrapper function of td_ref_sem().<br>
It state of semaphore reference.<br>
@param [in]		semid				: Semaphore ID
@param [out]	rsem				: Semaphore State Information Pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Ref_Sem(OS_USER_ID semid, TD_OS_USER_RSEM* rsem);

/**
This function is wrapper function of td_ref_flg().<br>
It state of event flag reference.<br>
@param [in]		flgid				: Event Flag ID
@param [out]	rflg				: Event Flag State Information Pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Ref_Flg(OS_USER_ID flgid, TD_OS_USER_RFLG* rflg);

/**
This function is wrapper function of td_ref_mbx().<br>
It state of mail box reference.<br>
@param [in]		mbxid				: Mail box ID
@param [out]	rmbx				: Mail box State Information Pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Ref_Mbx(OS_USER_ID mbxid, TD_OS_USER_RMBX* rmbx);

/**
This function is wrapper function of td_ref_mtx().<br>
It state of mutex reference.<br>
@param [in]		mtxid				: Mutex ID
@param [out]	rmtx				: Mutex State Information Pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Ref_Mtx(OS_USER_ID mtxid, TD_OS_USER_RMTX* rmtx);

/**
This function is wrapper function of td_ref_mbf().<br>
It state of message buffer reference.<br>
@param [in]		mbfid				: Message Buffer ID
@param [out]	rmbf				: Message Buffer State Information Pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Ref_Mbf(OS_USER_ID mbfid, TD_OS_USER_RMBF* rmbf);

/**
This function is wrapper function of td_ref_por().<br>
It state of rendezvous port reference.<br>
@param [in]		porid				: Rendezvous port ID
@param [out]	rpor				: Rendezvous port State Information Pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Ref_Por(OS_USER_ID porid, TD_OS_USER_RPOR* rpor);

/**
This function is wrapper function of td_ref_mpf().<br>
It state of fixed-sized memory pool reference.<br>
@param [in]		mpfid				: Fixed-sized memory pool ID
@param [out]	rmpf				: Fixed-sized memory pool State Information Pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Ref_Mpf(OS_USER_ID mpfid, TD_OS_USER_RMPF* rmpf);

/**
This function is wrapper function of td_ref_mpl().<br>
It state of variable-sized memory pool reference.<br>
@param [in]		mplid				: Variable-sized memory pool ID
@param [out]	rmpl				: Variable-sized memory pool State Information Pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Ref_Mpl(OS_USER_ID mplid, TD_OS_USER_RMPL* rmpl);

/**
This function is wrapper function of td_ref_cyc().<br>
It state of cyclic reference.<br>
@param [in]		cycid				: Cyclic ID
@param [out]	rcyc				: Cycle handler State Information Pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Ref_Cyc(OS_USER_ID cycid, TD_OS_USER_RCYC* rcyc);

/**
This function is wrapper function of td_ref_alm().<br>
It state of alarm reference.<br>
@param [in]		almid				: Alarm ID
@param [out]	ralm				: Alarm handler State Information Pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Ref_Alm(OS_USER_ID almid, TD_OS_USER_RALM* ralm);

/**
This function is wrapper function of td_ref_sys().<br>
It state of system reference.<br>
@param [out]	rsys				: Packet to Refer System Pointer
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Ref_Sys(TD_OS_USER_RSYS* rsys);

/*-td_*** Function-------------------------------------------------------*/
/**
This function is wrapper function of td_get_reg().<br>
It get task registers.<br>
@param [in]		taskid				: Task ID
@param [out]	pk_regs				: Packet of Registers
@param [out]	pk_eit				: Packet of EIT
@param [out]	pk_cregs			: Packet of Control Registers
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_OBJ				: Object State Error
*/
OS_USER_ER OS_User_Td_Get_Reg(OS_USER_ID taskid, T_OS_USER_REGS *pk_regs, T_OS_USER_EIT *pk_eit, T_OS_USER_CREGS *pk_cregs);

/**
This function is wrapper function of td_set_reg().<br>
It set task registers.<br>
@param [in]		taskid				: Task ID
@param [out]	pk_regs				: Packet of Registers
@param [out]	pk_eit				: Packet of EIT
@param [out]	*pk_cregs			: Packet of Control Registers
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
@retval	D_OS_USER_E_OBJ				: Object State Error
*/
OS_USER_ER OS_User_Td_Set_Reg(OS_USER_ID taskid, T_OS_USER_REGS *pk_regs, T_OS_USER_EIT *pk_eit, T_OS_USER_CREGS *pk_cregs);

/**
This function is wrapper function of td_get_tim().<br>
It get system time.<br>
@param [out]	p_systime			: Pointer of system time
@param [out]	ofs					: Pointer of Offset Time for Tim
@retval	D_OS_USER_E_OK				: Success
*/
OS_USER_ER OS_User_Td_Get_Tim(OS_USER_SYSTIM* p_systime, OS_USER_UINT* ofs);

/**
This function is wrapper function of td_get_otm().<br>
It get operating time.<br>
@param [out]	p_systime			: Pointer of operating time
@param [out]	ofs					: Pointer of Offset Time for Tim
@retval	D_OS_USER_E_OK				: Success
*/
OS_USER_ER OS_User_Td_Get_Otm(OS_USER_SYSTIM* p_systime, OS_USER_UINT* ofs);

/**
This function is wrapper function of td_ref_dsname().<br>
It DS object name reference.<br>
@param [in]		type				: Object Type
@param [in]		id					: Object ID
@param [out]	dsname				: Pointer of Object Name
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_OBJ				: Object State Error
*/
OS_USER_ER OS_User_Td_Ref_Dsname(OS_USER_UINT type, OS_USER_ID id, OS_USER_UB* dsname);

/**
This function is wrapper function of td_set_dsname().<br>
It Set DS object name.<br>
@param [in]		type				: Object Type
@param [in]		id					: Object ID
@param [out]	dsname				: Pointer of Object Name
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ER OS_User_Td_Set_Dsname(OS_USER_UINT type, OS_USER_ID id, OS_USER_UB* dsname);

/**
This function is wrapper function of td_hok_svc().<br>
It System call extended SVC hook.<br>
@param [in]		hsvc				: Packet of hook routine
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ER OS_User_Td_Hok_Svc(TD_OS_USER_HSVC* hsvc);

/**
This function is wrapper function of td_hok_dsp().<br>
It Dsipatch hook.<br>
@param [in]		hdsp				: Packet of hook routine
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ER OS_User_Td_Hok_Dsp(TD_OS_USER_HDSP* hdsp);

/**
This function is wrapper function of td_hok_int().<br>
It Interrupt handler hook.<br>
@param [in]		hint				: Packet of hook routine
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_NOEXS			: Unregistered task
@retval	D_OS_USER_E_OBJ				: Object State Error
@retval	D_OS_USER_E_ONAME			: Object Name Error
*/
OS_USER_ER OS_User_Td_Hok_Int(TD_OS_USER_HINT* hint);

/**
This function is wrapper function of td_inf_tsk().<br>
It task information.<br>
@param [in]		taskid				: Task ID
@param [in]		pk_itsk				: Packet of Task Statistics Information
@param [in]		clr					: Clear Task Statistics
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Td_Inf_Tsk(OS_USER_ID taskid, TD_OS_USER_ITSK* pk_itsk, OS_USER_BOOL clr);


/*-Function-------------------------------------------------------*/
/**
This function is wrapper function of GetSpaceInfo().<br>
It get information of address space.<br>
@param [in]		addr				: Logical Address
@param [in]		len					: Area size (byte)
@param [out]	pk_spinfo			: Information of Aaddress Space
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_MACV			: Memory Access Violation
*/
OS_USER_ER OS_User_GetSpaceInfo(OS_USER_VP addr, OS_USER_INT len, T_OS_USER_SPINFO* pk_spinfo);

/**
This function is wrapper function of CnvPhysicalAddr().<br>
It get physical address.<br>
@param [in]		vaddr				: Logical Address
@param [in]		len					: Area size (byte)
@param [out]	paddr				: Physical Aaddress
@retval	PositiveValue				: Area size of Physical Aaddress
@retval	D_OS_USER_E_MACV			: Memory Access Violation
*/
OS_USER_ER OS_User_CnvPhysicalAddr(OS_USER_VP vaddr, OS_USER_INT len, OS_USER_VP* paddr);

/**
This function is wrapper function of MapMemory().<br>
It Mapping contiguous memory area of physical address.<br>
@param [in]		paddr				: Physical Address
@param [in]		len					: Area size (byte)
@param [in]		attr				: Attribute of area
@param [out]	laddr				: Logical Aaddress
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_NOMEM			: No Memory
*/
OS_USER_ER OS_User_MapMemory(OS_USER_VP paddr, OS_USER_INT len, OS_USER_UINT attr, OS_USER_VP* laddr);

/**
This function is wrapper function of UnmapMemory().<br>
It Unmapping logical area.<br>
@param [in]		laddr				: Logical Address
@retval	D_OS_USER_E_LIMIT			: Exceeded the limits of the system
@retval	D_OS_USER_E_NOMEM			: No Memory
*/
OS_USER_ER OS_User_UnmapMemory(OS_USER_VP laddr);

/**
This function is wrapper function of DINTNO().<br>
It Conversion to interrupt definition number.<br>
@param [in]		intvec				: Interrupt Vector
*/
OS_USER_INTID OS_User_DINTNO(OS_USER_INTID intvec);

/**
This function is wrapper function of EnableInt().<br>
It Enable Interrupt.<br>
@param [in]		intvec				: Interrupt Vector
@param [in]		level				: Priority Level
*/
VOID OS_User_EnableInt(OS_USER_INTID intvec, OS_USER_INTLV level);

/**
This function is wrapper function of DisableInt().<br>
It Disable Interrupt.<br>
@param [in]		intvec				: Interrupt Vector
*/
VOID OS_User_DisableInt(OS_USER_INTID intvec);

/**
This function is wrapper function of ClearInt().<br>
It Clear Interrupt.<br>
@param [in]		intvec				: Interrupt Vector
*/
VOID OS_User_ClearInt(OS_USER_INTID intvec);

/**
This function is wrapper function of EndOfInt().<br>
It End of Interrupt.<br>
@param [in]		intvec				: Interrupt Vector
*/
VOID OS_User_EndOfInt(OS_USER_INTID intvec);

/**
This function is wrapper function of CheckInt().<br>
It End of Interrupt.<br>
@param [in]		intvec				: Interrupt Vector
*/
VOID OS_User_CheckInt(OS_USER_INTID intvec);

/**
This function is wrapper function of SetIntMode().<br>
It End of Interrupt.<br>
@param [in]		intvec				: Interrupt Vector
@param [in]		mode				: Interrupt mode
*/
VOID OS_User_SetIntMode(OS_USER_INTID intvec, OS_USER_UINT mode);

/**
This function is wrapper function of DefineExceptionHandler().<br>
It Define Exception Handler.<br>
@param [in]		excpno				: Exception Number
@param [in]		excphdr				: Exception Handler
@param [in]		index				: Exception Handler Index
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_PAR				: Parameter error
@retval	D_OS_USER_E_NOMEM			: No Memory
*/
OS_USER_ER OS_User_DefineExceptionHandler(OS_USER_UINT excpno, OS_USER_FP excphdr, OS_USER_INT index);

/**
This function is wrapper function of isig_tim().<br>
Supply of time tick.<br>
@retval	D_OS_USER_E_OK				: Normal End
*/
OS_USER_ER OS_User_Isig_Tim(VOID);

/**
This function is wrapper function of sns_ctx().<br>
References contexts.<br>
@retval	TRUE						: Called from the non-task context
@retval	FALSE						: Called from the task context
*/
BOOL OS_User_Sns_Ctx(VOID);


/**
This function is wrapper function of get_pri().<br>
It gets task priority.<br>
@param [in]		tskid				: Task ID
@param [in]		p_tskpri			: Pointer of task priority
@retval	D_OS_USER_E_OK				: Success
@retval	D_OS_USER_E_ID				: Illegal ID number
@retval	D_OS_USER_E_OBJ				: Object status is error
@retval	D_OS_USER_E_NOEXS			: Objects not created
*/
OS_USER_ER OS_User_Get_Pri(OS_USER_ID tskid, OS_USER_PRI* p_tskpri);

/**
This function is gets task index.<br>
@param [in]		tid					: Task ID
@retval	PositiveValue				: Task ID of MBA
*/
INT32 OS_User_Get_MbaTidFromTid( const OS_USER_ID tid );

/**
This function is gets task id.<br>
@param [out]	p_tid				: Task ID
@param [in]		mba_tid				: Task ID of MBA
@retval	PositiveValue				: TRUE / FALSE
*/
BOOL OS_User_Get_TidFromMbaTid( OS_USER_ID* const p_tid, const INT32 mba_tid );

/**
This function is gets task address.<br>
@param [in]		tid					: Task ID
@retval	PositiveValue				: Task Address
*/
VOID* OS_User_Get_TaskMainAddr( const OS_USER_ID tid );

/**
This function is wrapper function of ref_cfg().<br>
It refers configuration information.<br>
@param [in]		pk_rcfg				: Packet to Refer Configuration Information
@retval	D_OS_USER_E_OK				: Success
*/
OS_USER_ER OS_User_Ref_Cfg(T_OS_USER_RCFG *pk_rcfg);

#if 0
/**
This function is wrapper function of loc_cpu().<br>
It changes to CPU lock state.<br>
@retval	D_OS_USER_E_OK				: Success
*/
OS_USER_ER OS_User_Loc_Cpu(VOID);

/**
This function is wrapper function of unl_cpu().<br>
It changes to CPU lock release state.<br> 
@retval	D_OS_USER_E_OK				: Success
*/
OS_USER_ER OS_User_Unl_Cpu(VOID);

// Task controller
VOID OS_User_Ent_Tsk(VOID);
VOID OS_User_Ext_Tsk(VOID);
VOID OS_User_Sta(VOID);
VOID OS_User_Ini_Task(OS_USER_ID prcsid, OS_USER_ID tskid, T_OS_USER_CONTEXT *stackp, OS_USER_FP task, OS_USER_PRI tskpri);
VOID OS_User_Ini_Mpl(OS_USER_ID mplid, OS_USER_VP start_adrs, OS_USER_VP end_adrs );
VOID OS_User_Ini_Mpf(OS_USER_ID mpfid, OS_USER_VP_INT mpfcnt, OS_USER_VP_INT blfsz, OS_USER_VP mpf, OS_USER_VP attr);
VOID OS_User_Ini_Msg(OS_USER_ID mbxid, OS_USER_VP **msgbuf, OS_USER_VP_INT mbxnum );
OS_USER_ER OS_User_dly_tsk(OS_USER_RELTIM dlytim);

USHORT custom_prcs_id_num(VOID);
VOID idle_task(VOID);
#endif


/* @} */	// fj_os_wrapper_api

#ifdef __cplusplus
}
#endif

#endif	// _OS_USER_H_
