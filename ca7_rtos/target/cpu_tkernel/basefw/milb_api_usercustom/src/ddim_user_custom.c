/**
 * @file		DDIM_user_custom.c
 * @brief		Device Driver & Image Macro user custom wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ddim_user_custom.h"
#include "os_user_custom.h"
#include "dd_arm.h"
#include "ipcu.h"
#include "l1l2cache.h"
#include "debug.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
/* Nothing Special */

/*==============================================================================*/
/* Macro declaration															*/
/*==============================================================================*/
/* Nothing Special */

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
/* Nothing Special */

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
/* Nothing Special */

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
/********************************/
/*	Event Flag Identifier		*/
/********************************/
DDIM_USER_ID gFid_sen;							/**< SEN Event Flag */
DDIM_USER_ID gFid_sro1;							/**< SRO1 Event Flag */
DDIM_USER_ID gFid_sro2;							/**< SRO2 Event Flag */
DDIM_USER_ID gFid_b2b1;							/**< B2B1 Event Flag */
DDIM_USER_ID gFid_b2b2;							/**< B2B2 Event Flag */
DDIM_USER_ID gFid_stat;							/**< STAT Event Flag */
DDIM_USER_ID gFid_pas;							/**< PAS Event Flag */
DDIM_USER_ID gFid_b2r;							/**< B2R Event Flag */
DDIM_USER_ID gFid_ltm_rbk;						/**< LTM RBK Event Flag */
DDIM_USER_ID gFid_ltm_map;						/**< LTM MAP Event Flag */
DDIM_USER_ID gFid_ltm_ltm;						/**< LTM LTM Event Flag */
DDIM_USER_ID gFid_r2y;							/**< R2Y Event Flag */
DDIM_USER_ID gFid_disp;							/**< DISP Event Flag */
DDIM_USER_ID gFid_ela;							/**< ELA Event Flag */
DDIM_USER_ID gFid_iip;							/**< IIP Event Flag */
DDIM_USER_ID gFid_iip_unit_lo;					/**< IIP Unit Event Flag 1 of 2 */
DDIM_USER_ID gFid_iip_unit_hi;					/**< IIP Unit Event Flag 2 of 2 */
DDIM_USER_ID gFid_iip_pix;						/**< IIP Pixfmttbl Event Flag */
DDIM_USER_ID gFid_iip_res;						/**< IIP Resource Event Flag */
DDIM_USER_ID gFid_jpeg;							/**< JPEG Event Flag */
DDIM_USER_ID gFid_cnr;							/**< CNR Event Flag */
DDIM_USER_ID gFid_xch;							/**< Xch Event Flag */
DDIM_USER_ID gFid_shdr;							/**< SHDR Event Flag */
DDIM_USER_ID gFid_raw;							/**< RAW Event Flag */
DDIM_USER_ID gFid_bmh;							/**< BMH Event Flag */
DDIM_USER_ID gFid_hdmi;							/**< HDMI Event Flag */
DDIM_USER_ID gFid_rdma;							/**< RDMA Event Flag */
DDIM_USER_ID gFid_dwch;							/**< DWCH Event Flag */
DDIM_USER_ID gFid_fpt[2];						/**< FPT Event Flag */
DDIM_USER_ID gFid_i2c;							/**< I2C Event Flag */
DDIM_USER_ID gFid_hdmac;						/**< HDMAC Event Flag */
DDIM_USER_ID gFid_hdmac0;						/**< HDMAC0 Event Flag */
DDIM_USER_ID gFid_hdmac1;						/**< HDMAC1 Event Flag */
DDIM_USER_ID gFid_xdmac;						/**< XDMAC Event Flag */
DDIM_USER_ID gFid_xdmasnap;						/**< XDMAC SNAP Event Flag */
DDIM_USER_ID gFid_relc;							/**< RELC Event Flag */
DDIM_USER_ID gFid_ipcu_ack;						/**< IPCU(ACK) Event Flag */
DDIM_USER_ID gFid_ipcu_req;						/**< IPCU(REQ) Event Flag */
DDIM_USER_ID gFid_me;							/**< ME Event Flag */
DDIM_USER_ID gFid_motor_calc_0;					/**< Motor Calc_0 Event Flag */
DDIM_USER_ID gFid_motor_calc_1;					/**< Motor Calc_1 Event Flag */

/********************************/
/*	Semphore Identifier			*/
/********************************/
DDIM_USER_ID gSid_cnr_ofl[2];					/**< CNR Semaphore ID */
DDIM_USER_ID gSid_cnr_otf[2];					/**< CNR Semaphore ID */
DDIM_USER_ID gSid_iip;							/**< IIP Semaphore ID */
DDIM_USER_ID gSid_iip_sw;						/**< IIP SWTRG Semaphore ID */
DDIM_USER_ID gSid_xch[2];						/**< Xch Semaphore ID */
DDIM_USER_ID gSid_jpeg;							/**< JPEG Semaphore ID */
DDIM_USER_ID gSid_ela;							/**< ELA Semaphore ID */
DDIM_USER_ID gSid_raw;							/**< RAW Semaphore ID */
DDIM_USER_ID gSid_shdr;							/**< SHDR Semaphore ID */
DDIM_USER_ID gSid_fpt[2];						/**< FPT Semaphore ID */
DDIM_USER_ID gSid_tmr32[14];					/**< 32bit Reload Timer Semaphore ID */
DDIM_USER_ID gSid_tmr64;						/**< 64bit Timer Semaphore ID */
DDIM_USER_ID gSid_i2c[3];						/**< I2C Semaphore ID */
DDIM_USER_ID gSid_pwm[16];						/**< PWM Semaphore ID */
DDIM_USER_ID gSid_usio[8];						/**< UART/CSIO Semaphore ID */
DDIM_USER_ID gSid_hdmac[4];						/**< HDMAC Semaphore ID */
DDIM_USER_ID gSid_hdmac0[8];					/**< HDMAC0 Semaphore ID */
DDIM_USER_ID gSid_hdmac1[8];					/**< HDMAC1 Semaphore ID */
DDIM_USER_ID gSid_xdmac[4];						/**< XDMAC Semaphore ID */
DDIM_USER_ID gSid_xdmasnap[4];					/**< XDMAC SNAP Semaphore ID */
DDIM_USER_ID gSid_wdog;							/**< Watchdog Timer Semaphore ID */
DDIM_USER_ID gSid_udc[6];						/**< UDC Semaphore ID */
DDIM_USER_ID gSid_srlot;						/**< SRLOT Semaphore ID */
DDIM_USER_ID gSid_audio_if_in[6];				/**< AudioI/F Input Semaphore ID */
DDIM_USER_ID gSid_audio_if_out[4];				/**< AudioI/F Output Semaphore ID */
DDIM_USER_ID gSid_relc;							/**< SRLOT Semaphore ID */
DDIM_USER_ID gSid_timestamp;					/**< TIMESTAMP Semaphore ID */
DDIM_USER_ID gSid_pdm[2];						/**< PDM Semaphore ID */
DDIM_USER_ID gSid_pcie;							/**< PCIE Semaphore ID */
DDIM_USER_ID gSid_bmh[2];						/**< BMH Semaphore ID */
DDIM_USER_ID gSid_rdma;							/**< RDMA Semaphore ID */
DDIM_USER_ID gSid_spi[3];						/**< SPI Semaphore ID */
DDIM_USER_ID gSid_me;							/**< ME Semaphore ID */
DDIM_USER_ID gSid_slimbus[4];					/**< SLIMBus Semaphore ID */
DDIM_USER_ID gSid_ipcu[16];						/**< IPCU Semaphore ID */

/********************************/
/*	Spin Lock					*/
/********************************/
#ifdef __GNUC__
ULONG gSpin_Lock __attribute__((aligned(32)));
#else
__align(32) ULONG gSpin_Lock;
#endif

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
static ULONG gSpin_Lock_AhbReg __attribute__((section(".LOCK_SECTION"), aligned(64)));
static UINT32 gIntLockKey[D_DD_ARM_NUM_OF_CORES];
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
/* Nothing Special */

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
/* Nothing Special */

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
DDIM_USER_ER DDIM_User_Del_Tsk(DDIM_USER_ID tskid)
{
	return (DDIM_USER_ER)OS_User_Del_Tsk((OS_USER_ID)tskid);
}


DDIM_USER_ER DDIM_User_Tslp_Tsk(DDIM_USER_TMO tmout)
{
	return (DDIM_USER_ER)OS_User_Tslp_Tsk((OS_USER_TMO)tmout);
}


DDIM_USER_ER DDIM_User_Dly_Tsk(DDIM_USER_RELTIM dlytim)
{
	return (DDIM_USER_ER)OS_User_Dly_Tsk((OS_USER_RELTIM)dlytim);
}


DDIM_USER_ER DDIM_User_Sig_Sem(DDIM_USER_ID semid)
{
	return (DDIM_USER_ER)OS_User_Sig_Sem((OS_USER_ID)semid);
}


DDIM_USER_ER DDIM_User_Pol_Sem(DDIM_USER_ID semid)
{
	return (DDIM_USER_ER)OS_User_Pol_Sem((OS_USER_ID)semid);
}


DDIM_USER_ER DDIM_User_Twai_Sem(DDIM_USER_ID semid, DDIM_USER_TMO tmout)
{
	return (DDIM_USER_ER)OS_User_Twai_Sem((OS_USER_ID)semid, (OS_USER_TMO)tmout);
}


DDIM_USER_ER DDIM_User_Ref_Sem(DDIM_USER_ID semid, T_DDIM_USER_RSEM* pk_rsem)
{
	OS_USER_ER ercd;
	T_OS_USER_RSEM rsem;
	
	ercd = OS_User_Ref_Sem((OS_USER_ID)semid, &rsem);
	
	pk_rsem->wtskid = (DDIM_USER_ID)rsem.wtskid;
	pk_rsem->semcnt = rsem.semcnt;
	
	return (DDIM_USER_ER)ercd;
}


DDIM_USER_ER DDIM_User_Set_Flg(DDIM_USER_ID flgid, DDIM_USER_FLGPTN setptn)
{
	return (DDIM_USER_ER)OS_User_Set_Flg((OS_USER_ID)flgid, (OS_USER_FLGPTN)setptn);
}


DDIM_USER_ER DDIM_User_Clr_Flg(DDIM_USER_ID flgid, DDIM_USER_FLGPTN clrptn)
{
	return (DDIM_USER_ER)OS_User_Clr_Flg((OS_USER_ID)flgid, (OS_USER_FLGPTN)clrptn);
}


DDIM_USER_ER DDIM_User_Pol_Flg(DDIM_USER_ID flgid, DDIM_USER_FLGPTN waiptn, DDIM_USER_MODE wfmode, DDIM_USER_FLGPTN* p_flgptn)
{
	return (DDIM_USER_ER)OS_User_Pol_Flg((OS_USER_ID)flgid, (OS_USER_FLGPTN)waiptn, (OS_USER_MODE)wfmode, (OS_USER_FLGPTN*)p_flgptn);
}


DDIM_USER_ER DDIM_User_Twai_Flg(DDIM_USER_ID flgid, DDIM_USER_FLGPTN waiptn, DDIM_USER_MODE wfmode, DDIM_USER_FLGPTN* p_flgptn, DDIM_USER_TMO tmout)
{
	return (DDIM_USER_ER)OS_User_Twai_Flg((OS_USER_ID)flgid, (OS_USER_FLGPTN)waiptn, (OS_USER_MODE)wfmode, (OS_USER_FLGPTN*)p_flgptn, (OS_USER_TMO)tmout);
}


DDIM_USER_ER DDIM_User_Ref_Flg(DDIM_USER_ID flgid, T_DDIM_USER_RFLG* pk_rflg)
{
	OS_USER_ER ercd;
	T_OS_USER_RFLG rflg;
	
	ercd = OS_User_Ref_Flg((OS_USER_ID)flgid, &rflg);
	
	pk_rflg->wtskid = (DDIM_USER_ID)rflg.wtskid;
	pk_rflg->flgptn = (DDIM_USER_FLGPTN)rflg.flgptn;
	
	return (DDIM_USER_ER)ercd;
}


DDIM_USER_ER DDIM_User_Dis_Dsp(VOID)
{
	return (DDIM_USER_ER)OS_User_Dis_Dsp();
}


DDIM_USER_ER DDIM_User_Ena_Dsp(VOID)
{
	return (DDIM_USER_ER)OS_User_Ena_Dsp();
}


DDIM_USER_ER DDIM_User_Get_Tim(DDIM_USER_SYSTIM* p_systim)
{
	return (DDIM_USER_ER)OS_User_Get_Tim((OS_USER_SYSTIM*)p_systim);
}


DDIM_USER_ER_ID DDIM_User_Acre_Tsk(T_DDIM_USER_CTSK* pk_ctsk)
{
	T_OS_USER_CTSK ctsk;
	
	ctsk.tskatr  = (OS_USER_ATR)pk_ctsk->tskatr;
	ctsk.exinf   = (OS_USER_VP_INT)pk_ctsk->exinf;
	ctsk.task    = (OS_USER_FP)pk_ctsk->task;
	ctsk.itskpri = (OS_USER_PRI)pk_ctsk->itskpri;
	ctsk.stksz   = (OS_USER_SIZE)pk_ctsk->stksz;
	ctsk.stk     = (OS_USER_VP)pk_ctsk->stk;
#if D_DDIM_USER_KERNEL
	ctsk.AssignProcessor  = (OS_USER_UW)pk_ctsk->AssignProcessor;
#endif
	
	return (DDIM_USER_ER_ID)OS_User_Acre_Tsk(&ctsk);
}


VOID DDIM_User_Ext_Tsk(VOID)
{
	OS_User_Ext_Tsk();
}


DDIM_USER_ER DDIM_User_Wai_Flg(DDIM_USER_ID flgid, DDIM_USER_FLGPTN waiptn, DDIM_USER_MODE wfmode, DDIM_USER_FLGPTN* p_flgptn)
{
	return (DDIM_USER_ER)OS_User_Wai_Flg((OS_USER_ID)flgid, (OS_USER_FLGPTN)waiptn, (OS_USER_MODE)wfmode, (OS_USER_FLGPTN*)p_flgptn);
}


DDIM_USER_ER_ID DDIM_User_Acre_Sem(T_DDIM_USER_CSEM* pk_csem)
{
	T_OS_USER_CSEM csem;
	
	csem.sematr  = (OS_USER_ATR)pk_csem->sematr;
	csem.isemcnt = pk_csem->isemcnt;
	csem.maxsem  = pk_csem->maxsem;
	
	return (DDIM_USER_ER_ID)OS_User_Acre_Sem(&csem);
}


DDIM_USER_ER DDIM_User_Del_Sem(DDIM_USER_ID semid)
{
	return (DDIM_USER_ER)OS_User_Del_Sem((OS_USER_ID)semid);
}


DDIM_USER_ER DDIM_User_Wai_Sem(DDIM_USER_ID semid)
{
	return (DDIM_USER_ER)OS_User_Wai_Sem((OS_USER_ID)semid);
}


DDIM_USER_ER_ID DDIM_User_Acre_Cyc(T_DDIM_USER_CCYC* pk_ccyc)
{
	T_OS_USER_CCYC ccyc;
	
	ccyc.cycatr = (OS_USER_ATR)pk_ccyc->cycatr;
#if D_DDIM_USER_KERNEL
	ccyc.exinf  = (OS_USER_VP)pk_ccyc->exinf;
#else
	ccyc.exinf  = (OS_USER_VP_INT)pk_ccyc->exinf;
#endif
	ccyc.cychdr = (OS_USER_FP)pk_ccyc->cychdr;
	ccyc.cyctim = (OS_USER_RELTIM)pk_ccyc->cyctim;
	ccyc.cycphs = (OS_USER_RELTIM)pk_ccyc->cycphs;
#if D_DDIM_USER_KERNEL
	ccyc.assprc = (OS_USER_UW)pk_ccyc->assprc;
#endif
	
	return (DDIM_USER_ER_ID)OS_User_Acre_Cyc(&ccyc);
}


DDIM_USER_ER DDIM_User_Sta_Cyc(DDIM_USER_ID cycid)
{
	return (DDIM_USER_ER)OS_User_Sta_Cyc((OS_USER_ID)cycid);
}


DDIM_USER_ER DDIM_User_Stp_Cyc(DDIM_USER_ID cycid)
{
	return (DDIM_USER_ER)OS_User_Stp_Cyc((OS_USER_ID)cycid);
}


#if 0
BOOL DDIM_User_Sns_Ctx(VOID)
{
	return OS_User_Sns_Ctx();
}
#endif


DDIM_USER_ER DDIM_User_Def_Int(DDIM_USER_INTID dintno, T_DDIM_USER_DINT *pk_dint)
{
	return (DDIM_USER_ER)OS_User_Def_Int((OS_USER_INTID)dintno, (T_OS_USER_DINT*)pk_dint);
}


VOID DDIM_User_EnableInt(DDIM_USER_INTID intvec, DDIM_USER_INTLV level)
{
	OS_User_EnableInt((OS_USER_INTID)intvec, (OS_USER_INTLV)level);
}


VOID DDIM_User_DisableInt(DDIM_USER_INTID intvec)
{
	OS_User_DisableInt((OS_USER_INTID)intvec);
}


VOID DDIM_User_SetIntMode(DDIM_USER_INTID intvec, DDIM_USER_UINT mode)
{
	OS_User_SetIntMode((OS_USER_INTID)intvec, (OS_USER_UINT)mode);
}


/**
 *	@brief		Hunt the memory area.\n
 *	@param[in]	size				Memory Size
 *	@return		Not equall NULL		Address of huntted area
 *	@return		NULL				Hunt NG
 *	@note		None
 *	@attention	Nothing special.
 */
VOID* DDIM_User_Malloc(ULONG size)
{
	int* pointer;
	
	pointer = (int*)malloc(size);
	return pointer;
}


/**
 *	@brief		Free memory area.\n
 *	@param[in]	pointer				Memory Area Address(OLD)
 *	@return		Not equall NULL		Address of huntted area
 *	@return		None
 *	@note		None
 *	@attention	Nothing special.
 */
VOID DDIM_User_Free(VOID* pointer)
{
	if(pointer == NULL)return;
	free(pointer);
	return ;
}


/**
 *	@brief		Copy data to memory area.\n
 *	@param[in]	ptr1	Pointer 1
 *	@param[in]	ptr2	Pointer 2
 *	@param[in]	size	Size
 *	@return		None
 *	@note		None
 *	@attention	Nothing special.
*/
VOID DDIM_User_Memcpy(VOID* ptr1, VOID* ptr2, ULONG size)
{
	memcpy(ptr1, ptr2, size);
	return ;
}


/**
 *	@brief		Set initialization data to memory area.\n
 *	@param[in]	ptr		Pointer
 *	@param[in]	value	Setting Data
 *	@param[in]	size	Size
 *	@return		None
 *	@note		None
 *	@attention	Nothing special.
*/
VOID DDIM_User_Memset(VOID* ptr, UCHAR value, ULONG size)
{
	memset(ptr, value, size);
	return ;
}


#ifndef CO_DEBUG_ON_PC
/**
 * Wrapper for printf in DDIM
 */
VOID DDIM_User_Printf(const CHAR* format, ...)
{
	va_list ap;
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	
	if( 0 == format ) {
		return;
	}
	
	Dd_ARM_Critical_Section_Start( critseq );
	va_start(ap, format);	/* pgr0039 */
	myvprintf(format, ap);
	va_end(ap);
	Dd_ARM_Critical_Section_End( critseq );
}
#endif // CO_DEBUG_ON_PC


/**
 * Trap for assertion in DDIM
 */
VOID DDIM_User_Assertion_Error(VOID)
{
	DDIM_User_Dly_Tsk(1000);	// wait for Ddim_Assertion() debug console log output
	for(;;) {
		; // wait forever
	}
}


/**
 * Spin Lock
 */
VOID DDIM_User_Spin_Lock(VOID)
{
	fj_spin_lock();
}


/**
 * Spin Unlock
 */
VOID DDIM_User_Spin_Unlock(VOID)
{
	fj_spin_unlock();
}


/**
 * @brief		Level1 & Level2 D-Cache All Flush function.\n
 */
VOID DDIM_User_L1l2cache_Flush_All(VOID)
{
	BF_L1l2cache_Flush_All();
}


/**
 * @brief		Level1 & Level2 D-Cache All Clean function.\n
 */
VOID DDIM_User_L1l2cache_Clean_All(VOID)
{
	BF_L1l2cache_Clean_All();
}


/**
 * @brief		Level1 & Level2 D-Cache All Clean & Flush function.\n
 */
VOID DDIM_User_L1l2cache_Clean_Flush_All(VOID)
{
	BF_L1l2cache_Clean_Flush_All();
}


/**
 * @brief		Level1 & Level2 D-Cache Flush by Addressing & size function.\n
 * @param[in]	address		target start address
 * @param[in]	size		target size
 */
VOID DDIM_User_L1l2cache_Flush_Addr(UINT32 address, UINT32 size)
{
	BF_L1l2cache_Flush_Addr(address, size);
}


/**
 * @brief		Level1 & Level2 D-Cache Clean by Addressing & size function.\n
 * @param[in]	address		target start address
 * @param[in]	size		target size
 */
VOID DDIM_User_L1l2cache_Clean_Addr(UINT32 address, UINT32 size)
{
	BF_L1l2cache_Clean_Addr(address, size);
}


/**
 * @brief		Level1 & Level2 D-Cache Clean & Flush by Addressing & size function.\n
 * @param[in]	address		target start address
 * @param[in]	size		target size
 */
VOID DDIM_User_L1l2cache_Clean_Flush_Addr(UINT32 address, UINT32 size)
{
	BF_L1l2cache_Clean_Flush_Addr(address, size);
}


/**
Get Lock for AHB bus register access.
*/
VOID DDIM_User_AhbReg_SpinLock( VOID )
{
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	UINT32	sr;
	ULONG	cpu_id;

	sr = (UINT32)Dd_ARM_DI();
	
	cpu_id = Dd_ARM_Get_CPU_ID();
	if ( cpu_id >= D_DD_ARM_NUM_OF_CORES ) {
		while ( 1 ) {	// Assersion
			;
		}
	}

	gIntLockKey[cpu_id] = sr;

	Dd_ARM_Spin_Lock( &gSpin_Lock_AhbReg );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
}

/**
Release Lock for AHB bus register access.
*/
VOID DDIM_User_AhbReg_SpinUnLock( VOID )
{
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	ULONG	cpu_id;

	Dd_ARM_Spin_Unlock( &gSpin_Lock_AhbReg );

	cpu_id = Dd_ARM_Get_CPU_ID();
	if ( cpu_id >= D_DD_ARM_NUM_OF_CORES ) {
		while ( 1 ) {	// Assersion
			;
		}
	}

	Dd_ARM_EI( gIntLockKey[cpu_id] );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
}

