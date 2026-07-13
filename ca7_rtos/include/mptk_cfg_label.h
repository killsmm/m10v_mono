/*
 *----------------------------------------------------------------------
 *    SMP T-Kernel
 *
 *    Copyright (C) 2007-2010 Ken Sakamura. All Rights Reserved.
 *    SMP T-Kernel is distributed under the T-License for SMP T-Kernel.
 *----------------------------------------------------------------------
 *
 *    Version:   1.00.01
 *    Released by T-Engine Forum(http://www.t-engine.org) at 2010/02/19.
 *
 *----------------------------------------------------------------------
 */

/*
 *	@(#)mptk_cfg_label.h (sys/CA7MPCORE)
 *
 *	config label
 */

#ifndef __MPTK_CFG_LABEL_H__
#define __MPTK_CFG_LABEL_H__

#ifdef __cplusplus
extern "C" {
#endif


/* T-Kernel/OS */
#define MPTK_CFG_TMAXTSKID	((UB*)"TMaxTskId")	/* Maximum task ID */
#define MPTK_CFG_TMAXSEMID	((UB*)"TMaxSemId")	/* Maximum semaphore ID */
#define MPTK_CFG_TMAXFLGID	((UB*)"TMaxFlgId")	/* Maximum event flag ID */
#define MPTK_CFG_TMAXMBXID	((UB*)"TMaxMbxId")	/* Maximum mail box ID */
#define MPTK_CFG_TMAXMTXID	((UB*)"TMaxMtxId")	/* Maximum mutex ID */
#define MPTK_CFG_TMAXMBFID	((UB*)"TMaxMbfId")	/* Maximum message buffer ID */
#define MPTK_CFG_TMAXPORID	((UB*)"TMaxPorId")	/* Maximum rendezvous port ID */
#define MPTK_CFG_TMAXMPFID	((UB*)"TMaxMpfId")	/* Maximum fixed size memory pool ID */
#define MPTK_CFG_TMAXMPLID	((UB*)"TMaxMplId")	/* Maximum variable size memory pool ID */
#define MPTK_CFG_TMAXCYCID	((UB*)"TMaxCycId")	/* Maximum cyclic handler ID */
#define MPTK_CFG_TMAXALMID	((UB*)"TMaxAlmId")	/* Maximum alarm handler ID */
#define MPTK_CFG_TMAXRESID	((UB*)"TMaxResId")	/* Maximum resource group ID */
#define MPTK_CFG_TMAXSSYID	((UB*)"TMaxSsyId")	/* Maximum sub system ID */
#define MPTK_CFG_TMAXSSYPRI ((UB*)"TMaxSsyPri")	/* Maximum sub system priority */

#define MPTK_CFG_TSYSSTKSZ	((UB*)"TSysStkSz")	/* Default system stack size (byte) */
#define MPTK_CFG_TSVCLIMIT	((UB*)"TSVCLimit")	/* SVC protection level */
#define MPTK_CFG_TTIMPERIOD ((UB*)"TTimPeriod")	/* Timer interval (msec) */


/* SMP T-Kernel */
#define MPTK_CFG_TMAXDOMID ((UB*)"TMaxDomId")	/* Maximum domain ID */
#define MPTK_CFG_TOAPLIMIT ((UB*)"TOAPLimit")	/* The lowest protection level that protection is effective */
#define MPTK_CFG_TSYSPRNUM ((UB*)"TSysPrNum")	/* Number of the processors of the whole system */
#define MPTK_CFG_TKERPRNUM ((UB*)"TKerPrNum")	/* Number of the processors which a kernel controls */

/* T-Kernel/SM */				/* Maximum number of ... */
#define MPTK_CFG_TMAXREGDEV ((UB*)"TMaxRegDev")	/* devices registration */
#define MPTK_CFG_TMAXOPNDEV ((UB*)"TMaxOpnDev")	/* devices open */
#define MPTK_CFG_TMAXREQDEV ((UB*)"TMaxReqDev")	/* device requests */

#define MPTK_CFG_TDEVTMBFSZ ((UB*)"TDEvtMbfSz")	/* Event notification message */
						/* buffer size (byte), */
						/* Maximum length of message (byte) */


/* Task Event(1-8) */				/* Message management */
#define MPTK_CFG_TEV_MSGEVT ((UB*)"TEV_MsgEvt")	/* Receive message */
#define MPTK_CFG_TEV_MSGBRK ((UB*)"TEV_MsgBrk")	/* Release of an message waiting state */

#define MPTK_CFG_TEV_GDI	 ((UB*)"TEV_GDI")	/* GDI interface */
#define MPTK_CFG_TEV_FFLOCK ((UB*)"TEV_FFLock")	/* Release of an FIFO lock waiting state */


/* Segment manager */
#define MPTK_CFG_TREALMEMEND ((UB*)"TRealMemEnd")	/* RAM bottom address (logical address) */


/* System configuration(SYSCONF) */
#define MPTK_CFG_TSMPCALL_IPI_IRQ	((UB*)"TSmpcallIpiIrq")
#define MPTK_CFG_TSMPCALL_IPI_LEVEL	((UB*)"TSmpcallIpiLevel")


/* Stack Size for ARM */
#define MPTK_CFG_TABTSTKSZ	((UB*)"TAbtStkSz")	/* Abort(MMU) */
#define MPTK_CFG_TUNDSTKSZ	((UB*)"TUndStkSz")	/* Undefined instruction */
#define MPTK_CFG_TIRQSTKSZ	((UB*)"TIrqStkSz")	/* IRQ interrupt */
#define MPTK_CFG_TFIQSTKSZ	((UB*)"TFiqStkSz")	/* FIQ interrupt */


/* Exception handler */
#define MPTK_CFG_TMAXEXCPHDR ((UB*)"TMaxExcpHdr")	/* maximum number of exception handler */


/* Task priority */
#define MPTK_CFG_TINITTASKPRI ((UB*)"TInitTskPri")	/* Initial task priority number */


#ifdef __cplusplus
}
#endif
#endif /* __MPTK_CFG_LABEL_H__ */
