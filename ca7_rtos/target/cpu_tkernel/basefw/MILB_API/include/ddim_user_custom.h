/**
 * @file		ddim_user_custom.h
 * @brief		Device Driver & Image Macro user custom wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup user_custom
@{
*/

#ifndef _DDIM_USER_CUSTOM_H_
#define _DDIM_USER_CUSTOM_H_

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Type Definition														*/
/*----------------------------------------------------------------------*/
/* Data type */
typedef INT32			DDIM_USER_ER;			/**< Define type of "ER" (Type of OS function's return value) */
typedef INT32			DDIM_USER_ID;			/**< Define type of "ID" (Type of OS function's argument) */
typedef INT32			DDIM_USER_TMO;			/**< Define type of "TMO" (Type of OS function's argument) */
typedef UINT32			DDIM_USER_RELTIM;		/**< Define type of "RELTIM" (Type of OS function's argument) */
typedef ULLONG			DDIM_USER_SYSTIM;		/**< Define type of "SYSTIM" (Type of OS function's argument) */
typedef ULONG			DDIM_USER_FLGPTN;		/**< Define type of "FLGPTN" (Type of OS function's argument) */
typedef UINT32			DDIM_USER_MODE;			/**< Define type of "MODE" (Type of OS function's argument) */
typedef INT32			DDIM_USER_VP_INT;		/**< Define type of "VP_INT" (Type of OS function's argument) */
typedef INT32			DDIM_USER_ER_ID;		/**< Define type of "ER_ID" (Type of OS function's argument) */
typedef UINT32			DDIM_USER_ATR;			/**< Define type of "ATR" (Type of OS function's argument) */
typedef INT32			DDIM_USER_PRI;			/**< Define type of "PRI" (Type of OS function's argument) */
typedef VOID			(*DDIM_USER_FP)();		/**< Define type of "FP" (Type of OS function's argument) */
typedef VOID			*DDIM_USER_VP;			/**< Define type of "VP" (Type of OS function's argument) */
typedef UINT32			DDIM_USER_SIZE;			/**< Define type of "SIZE" (Type of OS function's argument) */
typedef UINT32			DDIM_USER_STAT;			/**< Define type of "STAT" (Type of OS function's argument) */
typedef CHAR			DDIM_USER_VB;			/**< Define type of "VG" (Type of OS function's argument) */
typedef DDIM_USER_ER	DDIM_USER_ER_UINT;		/**< Define type of "ER_UINT" (Type of OS function's argument) */
typedef INT32			DDIM_USER_INTLV;		/**< Define type of "INTLV" (Type of OS function's argument) */
typedef UINT32			DDIM_USER_INTID;		/**< Define type of "INTID" (Type of OS function's argument) */
typedef INT32			DDIM_USER_W;			/**< Define type of "W" (Type of OS function's argument) */
typedef UCHAR			DDIM_USER_UB;			/**< Define type of "UB" (Type of OS function's argument) */
typedef SHORT			DDIM_USER_UH;			/**< Define type of "UH" (Type of OS function's argument) */
typedef UINT32			DDIM_USER_UW;			/**< Define type of "UW" (Type of OS function's argument) */
typedef INT32			DDIM_USER_VW;			/**< Define type of "VW" (Type of OS function's argument) */
typedef BOOL			DDIM_USER_BOOL;			/**< Define type of "BOOL" (Type of OS function's argument) */
typedef INT32			DDIM_USER_INT;			/**< Define type of "INT" (Type of OS function's argument) */
typedef UINT32			DDIM_USER_UINT;			/**< Define type of "UINT" (Type of OS function's argument) */
typedef INT32			DDIM_USER_RNO;			/**< Define type of "RNO" (Type of OS function's argument) */

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Kernel */
#define D_DDIM_USER_KERNEL			(1)				/**< Kernel (1:T-Kernel) */

/* Error code */
#define D_DDIM_USER_E_OK			(0)				/**< Normal End */
#define D_DDIM_USER_E_SYS			(-5)			/**< System error */
#if D_DDIM_USER_KERNEL
#define D_DDIM_USER_E_NOCOP			(-6)			/**< Coprocessor Unusable (E_NOCOP) */
#endif
#define D_DDIM_USER_E_NOSPT			(-9)			/**< Non-supported error */
#define D_DDIM_USER_E_RSFN			(-10)			/**< Reserved function code */
#define D_DDIM_USER_E_RSATR			(-11)			/**< Reserve Attribute */
#define D_DDIM_USER_E_PAR			(-17)			/**< Parameter Errors */
#define D_DDIM_USER_E_ID			(-18)			/**< Illegal ID Number */
#define D_DDIM_USER_E_CTX			(-25)			/**< Context error */
#define D_DDIM_USER_E_MACV			(-26)			/**< Memory access violation */
#define D_DDIM_USER_E_OACV			(-27)			/**< Object access violation */
#define D_DDIM_USER_E_ILUSE			(-28)			/**< Illegal use of service call */
#define D_DDIM_USER_E_NOMEM			(-33)			/**< Memory Insufficient */
#define D_DDIM_USER_E_NOID			(-34)			/**< ID Number Insufficient */
#if D_DDIM_USER_KERNEL
#define D_DDIM_USER_E_LIMIT			(-34)			/**< Exceeded the limits of the system (E_LIMIT) */
#else
#define D_DDIM_USER_E_NOID			(-34)			/**< ID Number Insufficient */
#endif
#define D_DDIM_USER_E_OBJ			(-41)			/**< Object status is error */
#define D_DDIM_USER_E_NOEXS			(-42)			/**< Objects not created */
#define D_DDIM_USER_E_QOVR			(-43)			/**< Queueing Overflow */
#define D_DDIM_USER_E_RLWAI			(-49)			/**< Releases the WAITING state */
#define D_DDIM_USER_E_TMOUT			(-50)			/**< Polling failure or timeout exceeded */
#define D_DDIM_USER_E_DLT			(-51)			/**< Object being waited for was deleted */
#if D_DDIM_USER_KERNEL
#define D_DDIM_USER_E_DISWAI		(-52)			/**< Wait released by wait ban (E_DISWAI) */
#define D_DDIM_USER_E_IO			(-57)			/**< I/O Error (E_IO) */
#define D_DDIM_USER_E_NOMDA			(-58)			/**< No Media (E_NOMDA) */
#else
#define D_DDIM_USER_E_CLS			(-52)			/**< Object being waited for was changed */
#define D_DDIM_USER_E_WBLK			(-57)			/**< Non-blocking is accepted */
#define D_DDIM_USER_E_BOVR			(-58)			/**< Buffer overflowed */
#endif
#if D_DDIM_USER_KERNEL
#define D_DDIM_USER_E_BUSY			(-65)			/**< Busy State (E_BUSY) */
#define D_DDIM_USER_E_ABORT			(-66)			/**< Abort (E_ABORT) */
#define D_DDIM_USER_E_RONLY			(-67)			/**< Write Protected (E_RONLY) */
#define D_DDIM_USER_E_DOMAIN		(-68)			/**< Domain Error (E_DOMAIN) */
#define D_DDIM_USER_E_ONAME			(-69)			/**< Object Name Error (E_ONAME) */
#define D_DDIM_USER_E_DACV			(-70)			/**< Access Protection Error (E_DACV) */
#define D_DDIM_USER_E_IPC			(-71)			/**< Processor Communication Error (E_IPC) */
#define D_DDIM_USER_E_IPCA			(-72)			/**< Absolute Error Communication Between Processors (E_IPCA) */
#define D_DDIM_USER_E_IPCS			(-73)			/**< Processor Communication Sstate Error (E_IPCS) */
#endif

/* Definition used with eventflag */
#define D_DDIM_USER_TWF_ANDW		(0x00000000)	/**< EventFlag And Wait (TWF_ANDW) */
#define D_DDIM_USER_TWF_ORW			(0x00000001)	/**< EventFlag Or Wait (TWF_ORW) */

/* Definition used with semaphore */
#define D_DDIM_USER_SEM_WAIT_FEVR	(-1)			/**< Forever Wait (TMO_FEVR) */
#define D_DDIM_USER_SEM_WAIT_POL	(0)				/**< Wait by Polling (TMO_POL) */

/* Definition used with create semaphore */
#define D_DDIM_USER_TA_TFIFO		(0x00)			/**< Queuing is the order of FIFO (TA_TFIFO) */
#define D_DDIM_USER_TA_TPRI			(0x01)			/**< Queuing is the order of task priority (TA_TPRI) */

/* Definition used with create task/handler */
#define D_DDIM_USER_TA_ACT			(0x02)			/**< Task is activated after creation (TA_ACT) */
#define D_DDIM_USER_TA_STA			(0x02)			/**< Starts handler after handler is created (TA_STA) */
#define D_DDIM_USER_TA_PHS			(0x04)			/**< Saves cyclic handler phase (TA_PHS) */

/* for REALOS only (Don't apply to T-Kernel) */
#if D_DDIM_USER_KERNEL
#define D_DDIM_USER_TA_ASM			0x00000000U		/**< Program by assembler */
#define D_DDIM_USER_TA_HLNG			0x00000001U		/**< Program by high level programming language */
#define D_DDIM_USER_TA_SSTKSZ		0x00000002U		/**< Set system stack size */
#define D_DDIM_USER_TA_USERSTACK	0x00000004U		/**< Set user stack size */
#define D_DDIM_USER_TA_TASKSPACE	0x00000008U		/**< Specify task space */
#define D_DDIM_USER_TA_RESID		0x00000010U		/**< Specify resource group */
#define D_DDIM_USER_TA_ONAME		0x00000040U		/**< Use object name */
#define D_DDIM_USER_TA_RNG0			0x00000000U		/**< Execute by protection level 0 */
#define D_DDIM_USER_TA_RNG1			0x00000100U		/**< Execute by protection level 1 */
#define D_DDIM_USER_TA_RNG2			0x00000200U		/**< Execute by protection level 2 */
#define D_DDIM_USER_TA_RNG3			0x00000300U		/**< Execute by protection level 3 */
#define D_DDIM_USER_TA_COP0			0x00001000U		/**< Use coprocessor (ID=0) */
#define D_DDIM_USER_TA_COP1			0x00002000U		/**< Use coprocessor (ID=1) */
#define D_DDIM_USER_TA_COP2			0x00004000U		/**< Use coprocessor (ID=2) */
#define D_DDIM_USER_TA_COP3			0x00008000U		/**< Use coprocessor (ID=3) */
#define D_DDIM_USER_TA_DOMID		0x00010000U		/**< Specify domain ID */
#define D_DDIM_USER_TA_ASSPRC		0x00020000U		/**< Assign processor */
#define D_DDIM_USER_TA_PRIVATE		0x00040000U		/**< Protection attribute is private */
#define D_DDIM_USER_TA_PROTECTED	0x00080000U		/**< Protection attribute is protected */
#define D_DDIM_USER_TA_PUBLIC		0x00000000U		/**< Protection attribute is public */
#else
#define D_DDIM_USER_TA_HLNG			(0x00)			/**< Task/Handler is written in C language (TA_HLNG) */
#define D_DDIM_USER_TA_ASM			(0x01)			/**< Task/Handler is written in assembler language (TA_ASM) */
#endif

/* Event Flag for DDIM */
extern DDIM_USER_ID gFid_sen;							/**< SEN  Event Flag */
extern DDIM_USER_ID gFid_sro1;							/**< SRO1  Event Flag */
extern DDIM_USER_ID gFid_sro2;							/**< SRO2  Event Flag */
extern DDIM_USER_ID gFid_b2b1;							/**< B2B1  Event Flag */
extern DDIM_USER_ID gFid_b2b2;							/**< B2B2  Event Flag */
extern DDIM_USER_ID gFid_stat;							/**< STAT  Event Flag */
extern DDIM_USER_ID gFid_pas;							/**< PAS  Event Flag */
extern DDIM_USER_ID gFid_b2r;							/**< B2R Event Flag */
extern DDIM_USER_ID gFid_ltm_rbk;						/**< LTM RBK Event Flag */
extern DDIM_USER_ID gFid_ltm_map;						/**< LTM MAP Event Flag */
extern DDIM_USER_ID gFid_ltm_ltm;						/**< LTM LTM Event Flag */
extern DDIM_USER_ID gFid_r2y;							/**< R2Y Event Flag */
extern DDIM_USER_ID gFid_disp;							/**< DISP Event Flag */
extern DDIM_USER_ID gFid_ela;							/**< ELA Event Flag */
extern DDIM_USER_ID gFid_iip;							/**< IIP Event Flag */
extern DDIM_USER_ID gFid_iip_unit_lo;					/**< IIP Unit Event Flag 1 of 2 */
extern DDIM_USER_ID gFid_iip_unit_hi;					/**< IIP Unit Event Flag 2 of 2 */
extern DDIM_USER_ID gFid_iip_pix;						/**< IIP Pixfmttbl Event Flag */
extern DDIM_USER_ID gFid_iip_res;						/**< IIP Resource Event Flag */
extern DDIM_USER_ID gFid_jpeg;							/**< JPEG Event Flag */
extern DDIM_USER_ID gFid_cnr;							/**< CNR Event Flag */
extern DDIM_USER_ID gFid_xch;							/**< Xch Event Flag */
extern DDIM_USER_ID gFid_shdr;							/**< SHDR Event Flag */
extern DDIM_USER_ID gFid_raw;							/**< RAW Event Flag */
extern DDIM_USER_ID gFid_bmh;							/**< BMH Event Flag */
extern DDIM_USER_ID gFid_hdmi;							/**< HDMI Event Flag */
extern DDIM_USER_ID gFid_rdma;							/**< RDMA Event Flag */
extern DDIM_USER_ID gFid_dwch;							/**< RDMA Event Flag */
extern DDIM_USER_ID gFid_fpt[2];						/**< FPT Event Flag */
extern DDIM_USER_ID gFid_i2c;							/**< I2C Event Flag */
extern DDIM_USER_ID gFid_hdmac;							/**< HDMAC Event Flag */
extern DDIM_USER_ID gFid_hdmac0;						/**< HDMAC0 Event Flag */
extern DDIM_USER_ID gFid_hdmac1;						/**< HDMAC1 Event Flag */
extern DDIM_USER_ID gFid_xdmac;							/**< XDMAC Event Flag */
extern DDIM_USER_ID gFid_xdmasnap;						/**< XDMAC SNAP Event Flag */
extern DDIM_USER_ID gFid_relc;							/**< RELC Event Flag */
extern DDIM_USER_ID	gFid_ipcu_ack;						/**< IPCU Event Flag for Ack */
extern DDIM_USER_ID	gFid_ipcu_req;						/**< IPCU Event Flag for Req */
extern DDIM_USER_ID gFid_me;							/**< ME Event Flag */
extern DDIM_USER_ID gFid_motor_calc_0;					/**< Motor_Calc_0 Event Flag */
extern DDIM_USER_ID gFid_motor_calc_1;					/**< Motor_Calc_1 Event Flag */

#define FID_IM_SEN 				(gFid_sen)				/**< SEN  Event Flagdefinition  */
#define FID_IM_B2R				(gFid_b2r)				/**< B2R Event Flagdefinition  */
#define FID_IM_LTM_RBK			(gFid_ltm_rbk)			/**< LTM RBK Event Flagdefinition  */
#define FID_IM_LTM_MAP			(gFid_ltm_map)			/**< LTM MAP Event Flagdefinition  */
#define FID_IM_LTM_LTM			(gFid_ltm_ltm)			/**< LTM LTM Event Flagdefinition  */
#define FID_IM_R2Y				(gFid_r2y)				/**< R2Y Event Flagdefinition  */
#define FID_IM_DISP				(gFid_disp)				/**< DISP Event Flag definition */
#define FID_IM_ELA				(gFid_ela)				/**< ELA Event Flag definition */
#define FID_IM_IIP				(gFid_iip)				/**< IIP Event Flag definition */
#define	FID_IM_IIP_UNIT_LO		(gFid_iip_unit_lo)		/**< IIP Unit Event Flag definition 1 of 2 */
#define	FID_IM_IIP_UNIT_HI		(gFid_iip_unit_hi)		/**< IIP Unit Event Flag definition 2 of 2 */
#define FID_IM_IIP_PIX			(gFid_iip_pix)			/**< IIP Pixfmttbl Event Flag definition */
#define FID_IM_IIP_RES			(gFid_iip_res)			/**< IIP Resource Event Flag definition */
#define FID_IM_JPEG				(gFid_jpeg)				/**< JPEG Event Flag definition */
#define FID_IM_CNR				(gFid_cnr)				/**< CNR Event Flag definition */
#define FID_IM_XCH				(gFid_xch)				/**< Xch Event Flag definition */
#define FID_IM_SHDR				(gFid_shdr)				/**< SHDR Event Flag definition */
#define FID_IM_RAW				(gFid_raw)				/**< RAW Event Flag definition */
#define FID_IM_FPT(ch)			(gFid_fpt[ch])			/**< FPT Event Flag definition */
#define FID_IM_BMH				(gFid_bmh)				/**< BMH Event Flag definition */
#define FID_IM_HDMI				(gFid_hdmi)				/**< HDMI Event Flag definition */
#define FID_IM_RDMA				(gFid_rdma)				/**< RDMA Event Flag definition */
#define FID_IM_DWCH				(gFid_dwch)				/**< DWCH Event Flag definition */			
#define FID_DD_I2C				(gFid_i2c)				/**< I2C Event Flagdefinition  */
#define FID_DD_HDMAC			(gFid_hdmac)			/**< HDMAC Event Flag definition */
#define FID_DD_HDMAC0			(gFid_hdmac0)			/**< HDMAC0 Event Flag definition */
#define FID_DD_HDMAC1			(gFid_hdmac1)			/**< HDMAC1 Event Flag definition */
#define FID_DD_XDMAC			(gFid_xdmac)			/**< XDMAC Event Flag definition */
#define FID_DD_XDMASNAP			(gFid_xdmasnap)			/**< XDMAC SNAP Event Flag definition */
#define FID_DD_RELC				(gFid_relc)				/**< RELC Event Flag definition */
#define FID_DD_IPCU_ACK			(gFid_ipcu_ack)			/**< IPCU Event Flag for Ack definition */
#define FID_DD_IPCU_REQ			(gFid_ipcu_req)			/**< IPCU Event Flag for Req definition */
#define FID_IM_ME				(gFid_me)				/**< ME Event Flag definition */
#define FID_DD_MOTOR_CALC_0		(gFid_motor_calc_0)		/**< Motor Calc_0 Event Flag definition */
#define FID_DD_MOTOR_CALC_1		(gFid_motor_calc_1)		/**< Motor Calc_1 Event Flag definition */

/* Semaphore for DDIM*/
extern DDIM_USER_ID gSid_cnr_ofl[2];					/**< CNR Semaphore ID */
extern DDIM_USER_ID gSid_cnr_otf[2];					/**< CNR Semaphore ID */
extern DDIM_USER_ID gSid_iip;							/**< IIP Semaphore ID */
extern DDIM_USER_ID gSid_iip_sw;						/**< IIP SWTRG Semaphore ID */
extern DDIM_USER_ID gSid_xch[2];						/**< Xch Semaphore ID */
extern DDIM_USER_ID gSid_jpeg;							/**< JPEG Semaphore ID */
extern DDIM_USER_ID gSid_ela;							/**< ELA Semaphore ID */
extern DDIM_USER_ID gSid_raw;							/**< RAW Semaphore ID */
extern DDIM_USER_ID gSid_shdr;							/**< SHDR Semaphore ID */
extern DDIM_USER_ID gSid_fpt[2];						/**< FPT Semaphore ID */
extern DDIM_USER_ID gSid_tmr32[14];						/**< 32bit Reload Timer Semaphore ID */
extern DDIM_USER_ID gSid_tmr64;							/**< 64bit Timer Semaphore ID */
extern DDIM_USER_ID gSid_i2c[3];						/**< I2C Semaphore ID */
extern DDIM_USER_ID gSid_pwm[16];						/**< PWM Semaphore ID */
extern DDIM_USER_ID gSid_usio[8];						/**< UART/CSIO Semaphore ID */
extern DDIM_USER_ID gSid_hdmac[4];						/**< HDMAC Semaphore ID */
extern DDIM_USER_ID gSid_hdmac0[8];						/**< HDMAC0 Semaphore ID */
extern DDIM_USER_ID gSid_hdmac1[8];						/**< HDMAC1 Semaphore ID */
extern DDIM_USER_ID gSid_xdmac[4];						/**< XDMAC Semaphore ID */
extern DDIM_USER_ID gSid_xdmasnap[4];					/**< XDMAC SNAP Semaphore ID */
extern DDIM_USER_ID gSid_wdog;							/**< Watchdog Timer Semaphore ID */
extern DDIM_USER_ID gSid_udc[6];						/**< UDC Semaphore ID */
extern DDIM_USER_ID gSid_audio_if_in[6];				/**< AudioI/F Input Semaphore ID */
extern DDIM_USER_ID gSid_audio_if_out[4];				/**< AudioI/F Output Semaphore ID */
extern DDIM_USER_ID gSid_relc;							/**< SRLOT Semaphore ID */
extern DDIM_USER_ID gSid_timestamp;						/**< TIMESTAMP Semaphore ID */
extern DDIM_USER_ID gSid_pdm[2];						/**< PDM Semaphore ID */
extern DDIM_USER_ID gSid_pcie;							/**< PCIE Semaphore ID */
extern DDIM_USER_ID gSid_bmh[2];						/**< BMH Semaphore ID */
extern DDIM_USER_ID gSid_rdma;							/**< RDMA Semaphore ID */
extern DDIM_USER_ID gSid_spi[3];						/**< SPI Semaphore ID */
extern DDIM_USER_ID gSid_me;							/**< ME Semaphore ID */
extern DDIM_USER_ID gSid_slimbus[4];					/**< SLIMBus Semaphore ID */
extern DDIM_USER_ID gSid_ipcu[16];						/**< IPCU Semaphore ID */

#define SID_IM_CNR_OFL(ch)		(gSid_cnr_ofl[ch])		/**< CNR(OFL) Semaphore ID definition */
#define SID_IM_CNR_OTF(ch)		(gSid_cnr_otf[ch])		/**< CNR(OTF) Semaphore ID definition */
#define SID_IM_IIP				(gSid_iip)				/**< IIP Semaphore ID definition */
#define SID_IM_IIP_SW			(gSid_iip_sw)			/**< IIP SWTRG Semaphore ID definition */
#define SID_IM_XCH(ch)			(gSid_xch[ch])			/**< Xch Semaphore ID definition */
#define SID_IM_JPEG				(gSid_jpeg)				/**< JPEG Semaphore ID definition */
#define SID_IM_ELA				(gSid_ela)				/**< ELA Semaphore ID definition */
#define SID_IM_RAW				(gSid_raw)				/**< RAW Semaphore ID definition */
#define SID_IM_SHDR				(gSid_shdr)				/**< SHDR Semaphore ID definition */
#define SID_IM_FPT(ch)			(gSid_fpt[ch])			/**< FPT Semaphore ID definition */
#define SID_IM_BMH(ch)			(gSid_bmh[ch])			/**< BMH Semaphore ID definition */
#define SID_DD_TMR32(ch)		(gSid_tmr32[ch])		/**< 32bit Reload Timer Semaphore ID definition */
#define SID_DD_TMR64			(gSid_tmr64)			/**< 64bit Timer Semaphore ID definition */
#define SID_DD_I2C(ch)			(gSid_i2c[ch])			/**< I2C Semaphore ID definition */
#define SID_DD_PWM(ch)			(gSid_pwm[ch])			/**< PWM Semaphore ID definition */
#define SID_DD_USIO(ch)			(gSid_usio[ch])			/**< UART/CSIO Semaphore ID definition */
#define SID_DD_HDMAC(ch)		(gSid_hdmac[ch])		/**< HDMAC Semaphore ID definition */
#define SID_DD_HDMAC0(ch)		(gSid_hdmac0[ch])		/**< HDMAC0 Semaphore ID definition */
#define SID_DD_HDMAC1(ch)		(gSid_hdmac1[ch])		/**< HDMAC1 Semaphore ID definition */
#define SID_DD_XDMAC(ch)		(gSid_xdmac[ch])		/**< XDMAC Semaphore ID definition */
#define SID_DD_XDMASNAP(ch)		(gSid_xdmasnap[ch])		/**< XDMAC SNAP Semaphore ID definition */
#define SID_DD_WDOG				(gSid_wdog)				/**< Watchdog Timer Semaphore ID definition */
#define SID_DD_UDC(ch)			(gSid_udc[ch])			/**< UDC Semaphore ID definition */
#define SID_DD_AUDIO_IF_IN(ch)	(gSid_audio_if_in[ch])	/**< AudioI/F Input Semaphore ID definition */
#define SID_DD_AUDIO_IF_OUT(ch)	(gSid_audio_if_out[ch])	/**< AudioI/F Output Semaphore ID definition */
#define SID_DD_RELC				(gSid_relc)				/**< RELC Semaphore ID definition */
#define SID_DD_TIMESTAMP		(gSid_timestamp)		/**< TIMESTAMP Semaphore ID definition */
#define SID_DD_PDM(ch)			(gSid_pdm[ch])			/**< PDM Semaphore ID definition */
#define SID_DD_PCIE				(gSid_pcie)				/**< PCIE Semaphore ID definition */
#define SID_IM_RDMA				(gSid_rdma)				/**< RDMA Semaphore ID definition */
#define SID_DD_SPI(ch)			(gSid_spi[ch])			/**< SPI Semaphore ID definition */
#define SID_IM_ME				(gSid_me)				/**< ME Semaphore ID definition */
#define SID_DD_SLIMBUS(ch)		(gSid_slimbus[ch])		/**< SLIMBus Semaphore ID definition */
#define SID_DD_IPCU(mb_num)		(gSid_ipcu[mb_num])		/**< IPCU Semaphore ID definition */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Semaphore State Information */
typedef struct {
	DDIM_USER_ID		wtskid;		/**< Top Task ID of Semaphore Wait Matrix */
	UINT32				semcnt;		/**< Semaphore Resource Count */
} T_DDIM_USER_RSEM;

/** Task Create Information */
typedef struct {
	DDIM_USER_ATR		tskatr;		/**< Task Attributes */
	DDIM_USER_VP_INT	exinf;		/**< Extended Information */
	DDIM_USER_FP		task;		/**< Task Start Address */
	DDIM_USER_PRI		itskpri;	/**< Initial Task Priority */
	DDIM_USER_SIZE		stksz;		/**< Stack Size */
	DDIM_USER_VP		stk;		/**< Stack Region Top Address */
#if D_DDIM_USER_KERNEL
	DDIM_USER_UW		AssignProcessor;/**< Specify execution processor */
#endif
} T_DDIM_USER_CTSK;

/** Semaphore Create Information */
typedef struct {
	DDIM_USER_ATR		sematr;		/**< Semaphore Attribute */
	UINT32				isemcnt;	/**< Initial Value of Semaphore Resource Count */
	UINT32				maxsem;		/**< Max Semaphore Resource Count */
} T_DDIM_USER_CSEM;

/** Cyclic Create Information */
typedef struct {
	DDIM_USER_ATR		cycatr;		/**< Cyclic Handler Attribute */
	DDIM_USER_VP_INT	exinf;		/**< Extended Information */
	DDIM_USER_FP		cychdr;		/**< Pointer of Cyclic Handler Function */
	DDIM_USER_RELTIM	cyctim;		/**< Cyclic Start Time Interval */
	DDIM_USER_RELTIM	cycphs;		/**< Cyclic Handler Start Phase */
#if D_DDIM_USER_KERNEL
	DDIM_USER_UW			assprc;		/**< AssignProcessor */
#endif
} T_DDIM_USER_CCYC;

/** Eventflag State Information*/
typedef struct {
	DDIM_USER_ID		wtskid;		/**< Top Task ID of event flag ID */
	DDIM_USER_FLGPTN	flgptn;		/**< Bit pattern setting */
} T_DDIM_USER_RFLG;

/** Interrupt handler definition information */
typedef struct {
	DDIM_USER_ATR		intatr;		/**< Interrupt handler attribute */
	DDIM_USER_FP		inthdr;		/**< Interrupt handler address */
	DDIM_USER_UW		assprc;		/**< AssignProcessor */
#if TA_GP
	DDIM_USER_VP		gp;			/**< Global pointer (gp) */
#endif
} T_DDIM_USER_DINT;

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

/**
This function is wrapper function of del_tsk().<br>
Deletes tasks.
@param [in]		tskid				: Task ID
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_OBJ			: Object status is error
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
*/
DDIM_USER_ER DDIM_User_Del_Tsk(DDIM_USER_ID tskid);

/**
This function is wrapper function of tslp_tsk().<br>
Specifies timeout time for invoking task and shifts to wait status.
@param [in]		tmout				: Timeout time or constant macros can be specified
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_PAR			: Parameter Errors
@retval	D_DDIM_USER_E_CTX			: Context error
@retval	D_DDIM_USER_E_ILUSE			: Illegal use of service call
@retval	D_DDIM_USER_E_RLWAI			: Releases the WAITING state
@retval	D_DDIM_USER_E_TMOUT			: Polling failure or timeout
*/
DDIM_USER_ER DDIM_User_Tslp_Tsk(DDIM_USER_TMO tmout);

/**
This function is wrapper function of dly_tsk().<br>
Shifts invoking task to the delayed state.
@param [in]		dlytim				: Delay Time
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_PAR			: Parameter Errors
@retval	D_DDIM_USER_E_CTX			: Context error
@retval	D_DDIM_USER_E_ILUSE			: Illegal use of service call
@retval	D_DDIM_USER_E_RLWAI			: Releases the WAITING state
*/
DDIM_USER_ER DDIM_User_Dly_Tsk(DDIM_USER_RELTIM dlytim);

/**
This function is wrapper function of sig_sem().<br>
Returns semaphore resources.
@param [in]		semid				: Semaphore ID
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
@retval	D_DDIM_USER_E_QOVR			: Queuing Overflow
*/
DDIM_USER_ER DDIM_User_Sig_Sem(DDIM_USER_ID semid);

/**
This function is wrapper function of pol_sem().<br>
Polls the acquisition of semaphore resources.
@param [in]		semid				: Semaphore ID
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
@retval	D_DDIM_USER_E_TMOUT			: Polling failure or timeout
*/
DDIM_USER_ER DDIM_User_Pol_Sem(DDIM_USER_ID semid);

/**
This function is wrapper function of twai_sem().<br>
Gets semaphore resources with timeout time.
@param [in]		semid				: Semaphore ID
@param [in]		tmout				: Timeout time or constant macros can be specified
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_PAR			: Parameter Errors
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_CTX			: Context error
@retval	D_DDIM_USER_E_ILUSE			: Illegal use of service call
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
@retval	D_DDIM_USER_E_RLWAI			: Releases the WAITING state
@retval	D_DDIM_USER_E_TMOUT			: Polling failure or timeout
@retval	D_DDIM_USER_E_DLT			: Deletes waiting objects
*/
DDIM_USER_ER DDIM_User_Twai_Sem(DDIM_USER_ID semid, DDIM_USER_TMO tmout);

/**
This function is wrapper function of ref_sem().<br>
References semaphore status.
@param [in]		semid				: Semaphore ID
@param [out]	pk_rsem				: Packet to refer semaphore state
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
*/
DDIM_USER_ER DDIM_User_Ref_Sem(DDIM_USER_ID semid, T_DDIM_USER_RSEM* pk_rsem);

/**
This function is wrapper function of set_flg().<br>
Sets eventflags.
@param [in]		flgid				: Eventflag ID
@param [in]		setptn				: Bit pattern to set
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
*/
DDIM_USER_ER DDIM_User_Set_Flg(DDIM_USER_ID flgid, DDIM_USER_FLGPTN setptn);

/**
This function is wrapper function of clr_flg().<br>
Clears eventflags.
@param [in]		flgid				: Eventflag ID
@param [in]		clrptn				: Clear Bit pattern
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
*/
DDIM_USER_ER DDIM_User_Clr_Flg(DDIM_USER_ID flgid, DDIM_USER_FLGPTN clrptn);

/**
This function is wrapper function of pol_flg().<br>
Polls eventflag wait cancel conditions.
@param [in]		flgid				: Eventflag ID
@param [in]		waiptn				: Waiting bit pattern
@param [in]		wfmode				: Waiting mode
@param [out]	p_flgptn			: Header address to variable to return the bit pattern
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_PAR			: Parameter Errors
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_ILUSE			: Illegal use of service call
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
@retval	D_DDIM_USER_E_TMOUT			: Polling failure or timeout
*/
DDIM_USER_ER DDIM_User_Pol_Flg(DDIM_USER_ID flgid, DDIM_USER_FLGPTN waiptn, DDIM_USER_MODE wfmode, DDIM_USER_FLGPTN* p_flgptn);

/**
This function is wrapper function of twai_flg().<br>
Waits for eventflag wait cancel conditions with timeout time.
@param [in]		flgid				: Eventflag ID
@param [in]		waiptn				: Waiting bit pattern
@param [in]		wfmode				: Waiting mode
@param [out]	p_flgptn			: Header address to variable to return the bit pattern
@param [in]		tmout				: Timeout time or constant macros can be specified
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_PAR			: Parameter Errors
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_CTX			: Context error
@retval	D_DDIM_USER_E_ILUSE			: Illegal use of service call
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
@retval	D_DDIM_USER_E_RLWAI			: Releases the WAITING state
@retval	D_DDIM_USER_E_TMOUT			: Polling failure or timeout
@retval	D_DDIM_USER_E_DLT			: Deletes waiting objects
*/
DDIM_USER_ER DDIM_User_Twai_Flg(DDIM_USER_ID flgid, DDIM_USER_FLGPTN waiptn, DDIM_USER_MODE wfmode, DDIM_USER_FLGPTN* p_flgptn, DDIM_USER_TMO tmout);

/**
This function is wrapper function of ref_flg().<br>
References eventflag status.
@param [in]		flgid				: Eventflag ID
@param [out]	pk_rflg				: Packet to refer eventflag state
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
*/
DDIM_USER_ER DDIM_User_Ref_Flg(DDIM_USER_ID flgid, T_DDIM_USER_RFLG* pk_rflg);

/**
This function is wrapper function of dis_dsp().<br>
Prohibits dispatches.
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_CTX			: Context error
*/
DDIM_USER_ER DDIM_User_Dis_Dsp(VOID);

/**
This function is wrapper function of ena_dsp().<br>
Enables dispatches.
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_CTX			: Context error
*/
DDIM_USER_ER DDIM_User_Ena_Dsp(VOID);

/**
This function is wrapper function of get_tim().<br>
References system time.
@param [out]	p_systim			: Header address to variable to return the system time
@retval	D_DDIM_USER_E_OK			: Normal End
*/
DDIM_USER_ER DDIM_User_Get_Tim(DDIM_USER_SYSTIM* p_systim);

/**
This function is wrapper function of acre_tsk().<br>
Creates tasks assigned with ID numbers.
@param [in]		pk_ctsk				: Header address of packet of the task creation information
@retval	PositiveValue				: Task ID
@retval	D_DDIM_USER_E_RSATR			: Reserve Attribute
@retval	D_DDIM_USER_E_PAR			: Parameter Errors
@retval	D_DDIM_USER_E_NOMEM			: Insufficient memory
@retval	D_DDIM_USER_E_NOID			: ID Number Insufficient
*/
DDIM_USER_ER_ID DDIM_User_Acre_Tsk(T_DDIM_USER_CTSK* pk_ctsk);

/**
This function is wrapper function of ext_tsk().<br>
Normally ends the invoking task.
*/
VOID DDIM_User_Ext_Tsk(VOID);

/**
This function is wrapper function of wai_flg().<br>
Waits for eventflag wait cancel conditions.
@param [in]		flgid				: Eventflag ID
@param [in]		waiptn				: Waiting bit pattern
@param [in]		wfmode				: Waiting mode
@param [out]	p_flgptn			: Header address to variable to return the bit pattern
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_PAR			: Parameter Errors
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_CTX			: Context error
@retval	D_DDIM_USER_E_ILUSE			: Illegal use of service call
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
@retval	D_DDIM_USER_E_RLWAI			: Releases the WAITING state
@retval	D_DDIM_USER_E_DLT			: Deletes waiting objects
*/
DDIM_USER_ER DDIM_User_Wai_Flg(DDIM_USER_ID flgid, DDIM_USER_FLGPTN waiptn, DDIM_USER_MODE wfmode, DDIM_USER_FLGPTN* p_flgptn);

/**
This function is wrapper function of acre_sem().<br>
Creates semaphores assigned with ID numbers.
@param [in]		pk_csem				: Header address of packet of the semaphore creation information
@retval	PositiveValue				: Semaphore ID
@retval	D_DDIM_USER_RSATR			: Reserve Attribute
@retval	D_DDIM_USER_E_PAR			: Parameter Errors
@retval	D_DDIM_USER_E_NOID			: ID Number Insufficient
*/
DDIM_USER_ER_ID DDIM_User_Acre_Sem(T_DDIM_USER_CSEM* pk_csem);

/**
This function is wrapper function of del_sem().<br>
Deletes semaphores.
@param [in]		semid				: Semaphore ID
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
*/
DDIM_USER_ER DDIM_User_Del_Sem(DDIM_USER_ID semid);

/**
This function is wrapper function of wai_sem().<br>
Gets semaphore resources.
@param [in]		semid				: Normal End
@retval	D_DDIM_USER_E_OK			: Success
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_CTX			: Context error
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
@retval	D_DDIM_USER_E_RLWAI			: Releases the WAITING state
@retval	D_DDIM_USER_E_DLT			: Deletes waiting objects
*/
DDIM_USER_ER DDIM_User_Wai_Sem(DDIM_USER_ID semid);

/**
This function is wrapper function of acre_cyc().<br>
Creates cyclic handlers assigned with ID numbers.
@param [in]		pk_ccyc				: Header address of packet of the cyclic handler creation information
@retval	PositiveValue				: Cyclic handler ID
@retval	D_DDIM_USER_RSATR			: Reserve Attribute
@retval	D_DDIM_USER_E_ILUSE			: Illegal use of service call
@retval	D_DDIM_USER_E_NOID			: ID Number Insufficient
@retval	D_DDIM_USER_E_PAR			: Parameter Errors
*/
DDIM_USER_ER_ID DDIM_User_Acre_Cyc(T_DDIM_USER_CCYC* pk_ccyc);

/**
This function is wrapper function of sta_cyc().<br>
Starts up cyclic handlers.
@param [in]		cycid				: Cyclic handler ID
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_ILUSE			: Illegal use of service call
@retval	D_DDIM_USER_E_NOEXS			: Objects not created
*/
DDIM_USER_ER DDIM_User_Sta_Cyc(DDIM_USER_ID cycid);

/**
This function is wrapper function of stp_cyc().<br>
Stops cyclic handlers.
@param [in]		cycid				: Cyclic handler ID
@retval	D_DDIM_USER_E_OK			: Normal End
@retval	D_DDIM_USER_E_ID			: Illegal ID Number
@retval	D_DDIM_USER_E_NOEXS			: Unregistered cyclic handler
*/
DDIM_USER_ER DDIM_User_Stp_Cyc(DDIM_USER_ID cycid);

#if 0
/**
This function is wrapper function of sns_ctx().<br>
References contexts.
@retval	TRUE						: Called from the non-task context
@retval	FALSE						: Called from the task context
*/
BOOL DDIM_User_Sns_Ctx(VOID);
#endif

/**
This function is wrapper function of def_int().<br>
It define interrupt handler
@param [in]		dintno				: Interrupt Define Number
@param [in]		pk_dint				: Packet to Define Interrupt Handler
@retval	D_DDIM_USER_E_OK				: Success
@retval	D_DDIM_USER_E_NOMEM			: No Memory
@retval	D_DDIM_USER_E_RSATR			: Reserve Attribute
@retval	D_DDIM_USER_E_PAR				: Parameter error
*/
DDIM_USER_ER DDIM_User_Def_Int(DDIM_USER_UINT dintno, T_DDIM_USER_DINT *pk_dint);

/**
This function is wrapper function of EnableInt().<br>
It Enable Interrupt.<br>
@param [in]		intvec				: Interrupt Vector
@param [in]		level				: Interrupt Level
*/
VOID DDIM_User_EnableInt(DDIM_USER_INTID intvec, DDIM_USER_INTLV level);

/**
This function is wrapper function of DisableInt().<br>
It Disable Interrupt.<br>
@param [in]		intvec				: Interrupt Vector
*/
VOID DDIM_User_DisableInt(DDIM_USER_INTID intvec);

/**
This function is wrapper function of SetIntMode().<br>
It End of Interrupt.<br>
@param [in]		intvec				: Interrupt Vector
@param [in]		mode				: Interrupt Vector
*/
VOID DDIM_User_SetIntMode(DDIM_USER_INTID intvec, DDIM_USER_UINT mode);

/**
Spin Lock function.
*/
VOID DDIM_User_Spin_Lock(VOID);

/**
Spin Unlock function.
*/
VOID DDIM_User_Spin_Unlock(VOID);

/**
Level1 & Level2 D-Cache All Flush function.
*/
VOID DDIM_User_L1l2cache_Flush_All(VOID);

/**
Level1 & Level2 D-Cache All Clean function.
*/
VOID DDIM_User_L1l2cache_Clean_All(VOID);

/**
Level1 & Level2 D-Cache All Clean & Flush function.
*/
VOID DDIM_User_L1l2cache_Clean_Flush_All(VOID);

/**
Level1 & Level2 D-Cache Flush by Addressing & size function.
@param [in]		address				: target start address
@param [in]		size				: target size
*/
VOID DDIM_User_L1l2cache_Flush_Addr(UINT32 address, UINT32 size);

/**
Level1 & Level2 D-Cache Clean by Addressing & size function.
@param [in]		address				: target start address
@param [in]		size				: target size
*/
VOID DDIM_User_L1l2cache_Clean_Addr(UINT32 address, UINT32 size);

/**
Level1 & Level2 D-Cache Clean & Flush by Addressing & size function.
@param [in]		address				: target start address
@param [in]		size				: target size
*/
VOID DDIM_User_L1l2cache_Clean_Flush_Addr(UINT32 address, UINT32 size);

/**
Assertion error trap function.
*/
VOID DDIM_User_Assertion_Error(VOID);

#ifndef CO_DEBUG_ON_PC

/**
Printf wrapper function.
@param [in]		format				: printf format
*/
#if 1	// -- REMOVE_EXCLUDE_DOXYGEN BEGIN --
VOID DDIM_User_Printf(const CHAR* format, ...) __attribute__ ((format(printf, 1, 2)));
#else	// -- REMOVE_EXCLUDE_DOXYGEN ELSE --
VOID DDIM_User_Printf(const CHAR* format, ...);
#endif	// -- REMOVE_EXCLUDE_DOXYGEN END --

#endif	// CO_DEBUG_ON_PC

/**
Get Lock for AHB bus register access.
*/
VOID DDIM_User_AhbReg_SpinLock( VOID );

/**
Release Lock for AHB bus register access.
*/
VOID DDIM_User_AhbReg_SpinUnLock( VOID );

#ifdef __cplusplus
}
#endif

#endif	// _DDIM_USER_CUSTOM_H_

/*@}*/
