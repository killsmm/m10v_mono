/**
 * @file		fs_user_custom.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "fs_user_custom.h"
#include "arm.h"
//#include "dd_rtc.h"
#include "dd_top.h"
#include "os_user_custom.h"
#include "fs_sender.h"
// --- REMOVE_DCF_EXIF BEGIN ---
#include "dcf_sender.h"
// --- REMOVE_DCF_EXIF END ---
#include "media_attr.h"
#include "dd_exs.h"
#include "dd_xdmasnap.h"
#include "dd_gic.h"
#include "mw_user_custom.h"
#include "mw_xdmac.h"
#include "fj_host.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_BF_FS_USER_CUSTOM_AHB_CLOCK_OFF		(0)	// AHB clock off
#define D_BF_FS_USER_CUSTOM_AHB_CLOCK_ON		(1)	// AHB clock on
#define D_BF_FS_USER_CUSTOM_AXI_CLOCK_OFF		(0)	// AXI clock off
#define D_BF_FS_USER_CUSTOM_AXI_CLOCK_ON		(1)	// AXI clock on

// Start and Stop clock
#define	START_CLK		(0)						// Start bus clock
#define	STOP_CLK		(1)						// Stop bus clock

// Reset control
#define	RESET_RELEASE	(0)						// Reset release
#define	RESET_SET		(1)						// Reset set

// Start and Stop XDMAC bus clock
#define	STOP_BUSCLK		(0)						// The XDMAC bus clock stop request
#define	START_BUSCLK	(1)						// The XDMAC bus clock start request

#define	NO_REQUESTED	(0)						// The XDMAC bus clock is not requested
#define	REQUESTED		(1)						// The XDMAC bus clock is requested

#define	XDMAC_DISABLED	(0)						// The XDMAC Low Power interface is disabled
#define	XDMAC_ENABLED	(1)						// The XDMAC Low Power interface is enabled

#define	REMAP_ENABLED	(0)						// GPV REMAP is enabled
#define	REMAP_DISABLED	(1)						// GPV REMAP interface is disabled

// Start and Stop NF bus clock
#define	CANNOT_STOP		(1)						// The NF bus clock can not stop
#define	CAN_STOP		(0)						// The NF bus clock can stop



/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
static volatile INT32 gNF_XDMAC_first_flg = 1;
static volatile ULONG gNF_timeout_cnt = 0;		// use for exception log

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/

static FW_VOID bf_fs_user_custom_set_ahb_clock_hdmac(FW_UCHAR clock_ctrl);
static FW_INT32 bf_fs_user_custom_set_axi_clock_xdmac(FW_UCHAR clock_ctrl, FW_SHORT timeout, FW_UCHAR exception_flg);

/**
 * @brief set HDMAC AHB clock
 * @param i: clock control parameter
 *				D_BF_FS_USER_CUSTOM_AHB_CLOCK_OFF
 *				D_BF_FS_USER_CUSTOM_AHB_CLOCK_ON
 * @return FW_VOID
 * @note none
 * @attention none.
 */
static FW_VOID bf_fs_user_custom_set_ahb_clock_hdmac(FW_UCHAR clock_ctrl)
{
	if(clock_ctrl == D_BF_FS_USER_CUSTOM_AHB_CLOCK_ON){
		// clock on
		// Start HDMAC AHB Bus Clock
#if 0	// ### begin MILB_Porting
		Dd_Top_Set_CLKSTOP_HDMEAH(START_CLK);
#endif	// ### end MILB_Porting

		// Reset release for RELC macro
		//Dd_Exs_Set_RELC_Soft_Reset(RESET_RELEASE);
	}
	else{
		// clock off
		// Start HDMAC AHB Bus Clock
#if 0	// ### begin MILB_Porting
		Dd_Top_Set_CLKSTOP_HDMEAH(STOP_CLK);
#endif	// ### end MILB_Porting

		// Reset set for RELC macro
		//Dd_Exs_Set_RELC_Soft_Reset(RESET_SET);
	}
	return;
}

/**
 * @brief set XDMAC AXI clock
 * @param i: clock control parameter
 *				D_BF_FS_USER_CUSTOM_AXI_CLOCK_OFF
 *				D_BF_FS_USER_CUSTOM_AXI_CLOCK_ON
 * @return FW_INT32
 * @note none
 * @attention none.
 */
static FW_INT32 bf_fs_user_custom_set_axi_clock_xdmac(FW_UCHAR clock_ctrl, FW_SHORT timeout, FW_UCHAR exception_flg)
{
#if 0	// ### begin MILB_Porting
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	FW_ULONG start_time;
	FW_INT32 ret;

	if(clock_ctrl == D_BF_FS_USER_CUSTOM_AXI_CLOCK_ON){
		// Start XDMAC AXI bus clock
#if 0	// ### begin MILB_Porting
		Dd_Top_Set_CLKSTOP_XDMEAX(START_CLK);
#endif	// ### end MILB_Porting

		if (gNF_XDMAC_first_flg == 0) {
			start_time = BF_Fs_User_Custom_Start_Abort_Timer(exception_flg);
			ret = -1;
			do {
				if ((Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE() == NO_REQUESTED) &&
					(Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK() == NO_REQUESTED)) {
					break;
				}
				ret = BF_Fs_User_Custom_Check_Abort_Timer((FW_ULONG)timeout, start_time, exception_flg);	/* pgr0821 */
			} while (ret == -1);

			if (ret == 0) {
				BF_Fs_User_Custom_Err_Printer(("MWNF : ERROR : CLK_ON Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE/CSYSACK() \n"));
				return -1;
			}
		}

		Dd_Exs_Set_XDMAC_LowPower_Ctl_CSYSREQ(START_BUSCLK);

		start_time = BF_Fs_User_Custom_Start_Abort_Timer(exception_flg);
		ret = -1;
		do {
			if (Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE() == REQUESTED) {
				break;
			}
			ret = BF_Fs_User_Custom_Check_Abort_Timer((FW_ULONG)timeout, start_time, exception_flg);	/* pgr0821 */
		} while (ret == -1);

		if (ret == 0) {
			BF_Fs_User_Custom_Err_Printer(("MWNF : ERROR : CLK_ON Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE() \n"));
			return -1;
		}

		start_time = BF_Fs_User_Custom_Start_Abort_Timer(exception_flg);
		ret = -1;
		do {
			if (Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK() == REQUESTED) {
				break;
			}
			ret = BF_Fs_User_Custom_Check_Abort_Timer((FW_ULONG)timeout, start_time, exception_flg);	/* pgr0821 */
		} while (ret == -1);

		if (ret == 0) {
			BF_Fs_User_Custom_Err_Printer(("MWNF : ERROR : CLK_ON Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK() \n"));
			return -1;
		}

		MW_XDMAC_Global_Ctrl(0,0); // CP=0, LP=0

		Dd_ARM_Critical_Section_Start( critseq );
		gNF_XDMAC_first_flg = 0;
		Dd_ARM_Critical_Section_End( critseq );
	}
	else{
		start_time = BF_Fs_User_Custom_Start_Abort_Timer(exception_flg);
		ret = -1;
		do {
			if ((Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE() == REQUESTED) &&
				(Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK() == REQUESTED)) {
				break;
			}
			ret = BF_Fs_User_Custom_Check_Abort_Timer((FW_ULONG)timeout, start_time, exception_flg);	/* pgr0821 */
		} while (ret == -1);

		if (ret == 0) {
			BF_Fs_User_Custom_Err_Printer(("MWNF : ERROR : CLK_OFF Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE/CSYSACK() \n"));
			return -1;
		}

		// clock off
		MW_XDMAC_Global_Ctrl(0,1); // CP=0, LP=1

		Dd_Exs_Set_XDMAC_LowPower_Ctl_CSYSREQ(STOP_BUSCLK);

		start_time = BF_Fs_User_Custom_Start_Abort_Timer(exception_flg);
		ret = -1;
		do {
			if (Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE() == NO_REQUESTED) {
				break;
			}
			ret = BF_Fs_User_Custom_Check_Abort_Timer((FW_ULONG)timeout, start_time, exception_flg);	/* pgr0821 */
		} while (ret == -1);

		if (ret == 0) {
			BF_Fs_User_Custom_Err_Printer(("MWNF : ERROR : CLK_OFF Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE() \n"));
			return -1;
		}

		start_time = BF_Fs_User_Custom_Start_Abort_Timer(exception_flg);
		ret = -1;
		do {
			if (Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK() == NO_REQUESTED) {
				break;
			}
			ret = BF_Fs_User_Custom_Check_Abort_Timer((FW_ULONG)timeout, start_time, exception_flg);	/* pgr0821 */
		} while (ret == -1);

		if (ret == 0) {
			BF_Fs_User_Custom_Err_Printer(("MWNF : ERROR : CLK_OFF Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK() \n"));
			return -1;
		}
		
		// Stop XDMAC AXI bus clock
#if 0	// ### begin MILB_Porting
		Dd_Top_Set_CLKSTOP_XDMEAX(STOP_CLK);
#endif	// ### end MILB_Porting
	}
#endif	// ### end MILB_Porting

	// dummy func
	if (exception_flg == 2) {
		MW_XDMAC_Global_Ctrl(0,0); // CP=0, LP=0
	}
	return 0;
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/**
 * @brief init measured time.
 * @param FW_VOID
 * @return FW_VOID
 */
FW_VOID  BF_Fs_User_Custom_Measure_Time_Init(E_BF_MEASURE_TIME_ID id)
{
	FW_INT32 ercd;

	ercd = BF_Measure_Time_Start((E_BF_MEASURE_TIME_ID)id);
	if (ercd != 0) {
		BF_Fs_User_Custom_Err_Printer(("BF_Measure_Time_Start() failed.\n"));
	}
}

/**
 * @brief get measured time.
 * @param FW_ULONG* ltime
 * @return FW_VOID
 */
FW_VOID  BF_Fs_User_Custom_Measure_Time_Get(E_BF_MEASURE_TIME_ID id, FW_ULONG* ltime)
{
	FW_INT32		ercd;
	OS_USER_SYSTIM	m_time;

	ercd = BF_Measure_Time_Get((E_BF_MEASURE_TIME_ID)id, &m_time);
	if (ercd != 0) {
		BF_Fs_User_Custom_Err_Printer(("BF_Measure_Time_Get() failed.\n"));
		*ltime = 0;
	}
	else {
		*ltime = (FW_ULONG)m_time;
	}

	return;
}

/**
 * @brief print measured time.
 * @param i:*comment print comment
 * @return FW_VOID
 */
FW_VOID  BF_Fs_User_Custom_Measure_Time_Print(E_BF_MEASURE_TIME_ID id, const FW_CHAR* comment)
{
	FW_INT32 ercd;

	ercd = BF_Measure_Time_Print((E_BF_MEASURE_TIME_ID)id, (const char*)comment);
	if (ercd != 0) {
		BF_Fs_User_Custom_Err_Printer(("BF_Measure_Time_Print() failed.\n"));
	}
}


FW_VOID  BF_Fs_User_Custom_Get_Tim(FW_ULLONG* systime)
{
	OS_User_Get_Tim((OS_USER_SYSTIM*)systime);
}

FW_VOID  BF_Fs_User_Custom_Sta_Tsk_FS(FW_VOID)
{
	OS_User_Sta_Tsk(TID_FS_RECEIVER, 0);
}

FW_INT32 BF_Fs_User_Custom_Ref_Tsk_Fs(FW_VOID)
{
	T_OS_USER_RTSK pk_rtsk;
	
	OS_User_Ref_Tsk(TID_FS_RECEIVER, &pk_rtsk);
	
	return (FW_INT32)pk_rtsk.tskstat;
}

FW_VOID  BF_Fs_User_Custom_Ini_Pri_Fs(FW_VOID)
{
	OS_User_Chg_Pri(TID_FS_RECEIVER, D_OS_USER_TPRI_INI);
}


FW_VOID  BF_Fs_User_Custom_Chg_Pri_Fs(FW_VOID)
{

	OS_User_Chg_Pri(TID_FS_RECEIVER, 29);
//	chg_pri(TID_FS_RECEIVER, 28);
}

FW_VOID  BF_Fs_User_Custom_Recover_Pri_Fs_With_User(FW_VOID)
{
	OS_User_Chg_Pri(TID_FS_RECEIVER, D_OS_USER_TPRI_INI);
}

FW_VOID  BF_Fs_User_Custom_Sync_Pri_Fs_With_User(FW_VOID)
{
	T_OS_USER_RTSK	pk_rtsk;
	OS_USER_ID		tskid;

	OS_User_Get_Tid(&tskid);
	OS_User_Ref_Tsk(tskid, &pk_rtsk);
	OS_User_Chg_Pri(TID_FS_RECEIVER, pk_rtsk.tskpri);

}

BOOL     BF_Fs_User_Custom_Is_It_Inside_Fs_Rcv(FW_VOID)
{
	OS_USER_ID tskid = 0;

	OS_User_Get_Tid(&tskid);
	return TID_FS_RECEIVER == tskid;
}

FW_VOID  BF_Fs_User_Custom_Sta_Tsk_Ipcufs(FW_VOID)
{
	OS_User_Sta_Tsk(TID_IPCUFS_RECEIVER, 0);
}

FW_VOID  BF_Fs_User_Custom_Sta_Tsk_Vbd(FW_VOID)
{
	OS_User_Sta_Tsk(TID_VBD_RECEIVER, 0);
}

FW_INT32 BF_Fs_User_Custom_Ref_Tsk_Ipcufs(FW_VOID)
{
	T_OS_USER_RTSK pk_rtsk;
	
	OS_User_Ref_Tsk(TID_IPCUFS_RECEIVER, &pk_rtsk);
	
	return (FW_INT32)pk_rtsk.tskstat;
}

FW_INT32 BF_Fs_User_Custom_Ref_Tsk_Vbd(FW_VOID)
{
	T_OS_USER_RTSK pk_rtsk;
	
	OS_User_Ref_Tsk(TID_VBD_RECEIVER, &pk_rtsk);
	
	return (FW_INT32)pk_rtsk.tskstat;
}

FW_VOID  BF_Fs_User_Custom_Wup_Tsk(OS_USER_ID tskid)
{
	OS_User_Wup_Tsk(tskid);
}


FW_VOID  BF_Fs_User_Custom_Slp_Tsk()
{
	OS_User_Slp_Tsk();
}

FW_VOID  BF_Fs_User_Custom_Tslp_Tsk(OS_USER_TMO tmo)
{
	OS_User_Tslp_Tsk(tmo);
}

FW_INT32 BF_Fs_User_Custom_Dly_Tsk(OS_USER_RELTIM dlytim)
{
	return (FW_INT32)OS_User_Dly_Tsk(dlytim);
}


FW_INT32 BF_Fs_User_Custom_Get_Tid(OS_USER_ID* p_tskid)
{
	OS_USER_ER	ercd;
	OS_USER_ID	tskid =0x00;

	ercd = OS_User_Get_Tid(&tskid);
	
	*p_tskid = tskid;

	return (FW_INT32)ercd;
}


FW_INT32 BF_Fs_User_Custom_Sig_Sem(FW_INT32 semid)
{
	return (FW_INT32)OS_User_Sig_Sem((OS_USER_ID)semid);
}


FW_INT32 BF_Fs_User_Custom_Twai_Sem(FW_INT32 semid, FW_INT32 tmout)
{
	return (FW_INT32)OS_User_Twai_Sem((OS_USER_ID)semid, (OS_USER_TMO)tmout);
}


FW_INT32 BF_Fs_User_Custom_Dis_Dsp(FW_VOID)
{
	return (FW_INT32)OS_User_Dis_Dsp();
}

FW_INT32 BF_Fs_User_Custom_Ena_Dsp(FW_VOID)
{
	return (FW_INT32)OS_User_Ena_Dsp();
}


FW_INT32 BF_Fs_User_Custom_Pget_Blf_Mbox(FW_VOID* pp)
{
	return OS_User_Pget_Mpf(MPFID_FS_IF_MSG, (OS_USER_VP*)pp);
}

FW_INT32 BF_Fs_User_Custom_Rel_Blf_Mbox(FW_VOID* p)
{
	if (p == NULL) {
		while (1){
			OS_User_Dly_Tsk(10);
		}
	}

	return OS_User_Rel_Mpf(MPFID_FS_IF_MSG, (OS_USER_VP)p);
}

FW_INT32 BF_Fs_User_Custom_Ref_Mbx(FW_VOID** pp)
{
	OS_USER_ER		ercd;
	T_OS_USER_RMBX	pk_rmbx;

	ercd = OS_User_Ref_Mbx(MID_FS_IF, &pk_rmbx);
	if (pk_rmbx.pk_msg == NULL) {
		return D_OS_USER_E_TMOUT;
	}

	*pp = pk_rmbx.pk_msg;
	return ercd;
}

FW_INT32 BF_Fs_User_Custom_Ref_Mpf_Cnt(FW_VOID)
{
	T_OS_USER_RMPF	pk_rmpf;
	OS_USER_ER		ercd;

	ercd = OS_User_Ref_Mpf(MPFID_FS_IF_MSG, &pk_rmpf);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}

	return pk_rmpf.fblkcnt;
}

FW_VOID  BF_Fs_User_Custom_Clr_Flg_Init_End(FW_VOID)
{
	OS_User_Clr_Flg(FID_FILE_SYSTEM, ~D_BF_FS_USER_CUSTOM_FLG_FILE_SYSTEM_INIT_END);
}

FW_INT32 BF_Fs_User_Custom_Pol_Flg_Init_End(FW_VOID)
{
	FW_INT32		ercd;
	T_OS_USER_RFLG	pk_rflg;

	ercd = OS_User_Ref_Flg(FID_FILE_SYSTEM, &pk_rflg);
	if (ercd != 0) {
		BF_Fs_User_Custom_Err_Printer(("OS_User_Ref_Flg() failed: ercd=%d\n", ercd));
		return D_BF_FS_USER_CUSTOM_FLG_ERR;
	}

	if ((pk_rflg.flgptn & D_BF_FS_USER_CUSTOM_FLG_FILE_SYSTEM_INIT_END) == D_BF_FS_USER_CUSTOM_FLG_FILE_SYSTEM_INIT_END) {
		return D_BF_FS_USER_CUSTOM_FLG_MATCH;
	}

	return D_BF_FS_USER_CUSTOM_FLG_UNMATCH;
}

FW_VOID  BF_Fs_User_Custom_Set_Flg_Init_End(FW_VOID)
{
	OS_User_Set_Flg(FID_FILE_SYSTEM, D_BF_FS_USER_CUSTOM_FLG_FILE_SYSTEM_INIT_END);
}

FW_INT32 BF_Fs_User_Custom_Twai_Flg_Init_End(FW_VOID)
{
	FW_INT32 ercd;
	OS_USER_FLGPTN flg_ptn = 0;

	ercd = OS_User_Twai_Flg(FID_FILE_SYSTEM, D_BF_FS_USER_CUSTOM_FLG_FILE_SYSTEM_INIT_END, D_OS_USER_TWF_ORW, &flg_ptn, 10000);
	if (ercd == D_BF_FS_USER_CUSTOM_OK) {
		return D_BF_FS_USER_CUSTOM_FLG_MATCH;
	}
	else if (ercd == D_OS_USER_E_TMOUT) {
		return D_BF_FS_USER_CUSTOM_FLG_UNMATCH;
	}
	else {
		BF_Fs_User_Custom_Err_Printer(("OS_User_Twai_Flg() failed: ercd=%d\n", ercd));
		return D_BF_FS_USER_CUSTOM_FLG_ERR;
	}
}

FW_VOID  BF_Fs_User_Custom_Clr_Flg_Cancel(FW_VOID)
{
	OS_User_Clr_Flg(FID_FILE_SYSTEM, ~D_BF_FS_USER_CUSTOM_FLG_FILE_SYSTEM_CANCEL);	
}

FW_INT32 BF_Fs_User_Custom_Pol_Flg_Cancel(FW_VOID)
{
	FW_INT32 ercd;
	OS_USER_FLGPTN flgptn = 0;

	ercd = OS_User_Pol_Flg(FID_FILE_SYSTEM, D_BF_FS_USER_CUSTOM_FLG_FILE_SYSTEM_CANCEL, D_OS_USER_TWF_ANDW, &flgptn);
	
	if (ercd == D_BF_FS_USER_CUSTOM_OK) {
		return D_BF_FS_USER_CUSTOM_FLG_MATCH;
	}
	else if (ercd == D_OS_USER_E_TMOUT) {
		return D_BF_FS_USER_CUSTOM_FLG_UNMATCH;
	}
	else {
		BF_Fs_User_Custom_Err_Printer(("OS_User_Pol_Flg() failed: ercd=%d\n", ercd));
		return D_BF_FS_USER_CUSTOM_FLG_ERR;
	}
}

FW_VOID  BF_Fs_User_Custom_Set_Flg_Cancel(FW_VOID)
{
	OS_User_Set_Flg(FID_FILE_SYSTEM, D_BF_FS_USER_CUSTOM_FLG_FILE_SYSTEM_CANCEL);
}

FW_INT32 BF_Fs_User_Custom_Snd_Msg(FW_VOID* p)
{
	return OS_User_Snd_Mbx(MID_FS_IF, (T_OS_USER_MSG*)p);
}

FW_INT32 BF_Fs_User_Custom_Prcv_Msg(FW_VOID** pp)
{
	return OS_User_Prcv_Mbx(MID_FS_IF, (T_OS_USER_MSG**)pp);
	
}

FW_INT32 BF_Fs_User_Custom_Rcv_Msg(FW_VOID** pp)
{
	return OS_User_Rcv_Mbx(MID_FS_IF, (T_OS_USER_MSG**)pp);
}

FW_INT32 BF_Fs_User_Custom_Clr_Msg(FW_VOID)
{
	OS_USER_ER	ercd;
	FW_VOID* pp;
	OS_USER_ID tskid;

	for (;;) {
		ercd = OS_User_Prcv_Mbx(MID_FS_IF, (T_OS_USER_MSG**)&pp);
		if (ercd == D_OS_USER_E_TMOUT) {
			return D_OS_USER_E_OK;
		}

		if (ercd != D_OS_USER_E_OK) {
			return ercd;
		}

		if (0 == ((T_BF_FS_SENDER_MSG*)pp)->entry_func) {
			tskid = ((T_BF_FS_SENDER_MSG*)pp)->req_tskid;
			BF_Fs_User_Custom_Wup_Tsk(tskid);
		}
		OS_User_Rel_Mpf(MPFID_FS_IF_MSG, pp);
	}
}

FW_INT32 BF_Fs_User_Custom_Get_Blf_Exif_Buf(FW_VOID** pp)
{
	return OS_User_Get_Mpl(MPKID_EXIF, 0x5000, (OS_USER_VP*)pp);
}

FW_INT32 BF_Fs_User_Custom_Rel_Blf_Exif_Buf(FW_VOID* p)
{
	return OS_User_Rel_Mpl(MPKID_EXIF, (OS_USER_VP)p);
}


FW_INT32 BF_Fs_User_Custom_Get_Mpl( FW_INT32 mpfid, FW_UINT32 blksz, FW_VOID* p_blk)
{
	return OS_User_Get_Mpl( (OS_USER_ID)mpfid, (OS_USER_SIZE)blksz, (OS_USER_VP*)p_blk);
}

FW_INT32 BF_Fs_User_Custom_Pget_Mpl( FW_INT32 mpfid, FW_UINT32 blksz, FW_VOID* p_blk)
{
	return OS_User_Pget_Mpl( (OS_USER_ID)mpfid, (OS_USER_SIZE)blksz, (OS_USER_VP*)p_blk);
}

FW_INT32 BF_Fs_User_Custom_Rel_Mpl( FW_INT32 mpfid, FW_VOID* blk)
{
	return OS_User_Rel_Mpl( (OS_USER_ID)mpfid, (OS_USER_VP)blk);
}


#if 0     //RTC not used
FW_VOID  BF_Fs_User_Custom_Get_Rtc_Date(T_FS_USER_CUSTOM_RTC_DATE* rtc_date)
{
	T_DD_RTC_DATA rtc;

	Dd_RTC_Get_Time(&rtc);

	rtc_date->sec = rtc.SECOND;
	rtc_date->min = rtc.MINUTE;
	rtc_date->hour = rtc.HOUR;
	rtc_date->day = rtc.DAY;
	rtc_date->month = rtc.MONTH;
	rtc_date->year = rtc.YEAR;
}
#endif

FW_VOID  BF_Fs_User_Custom_Sta_Tsk_Dcf(FW_VOID)
{
// --- REMOVE_DCF_EXIF BEGIN ---
	OS_User_Sta_Tsk(TID_DCF_RECEIVER, 0);
// --- REMOVE_DCF_EXIF END ---
}

FW_VOID  BF_Fs_User_Custom_Ini_Pri_Dcf(FW_VOID)
{
// --- REMOVE_DCF_EXIF BEGIN ---
	OS_User_Chg_Pri(TID_DCF_RECEIVER, D_OS_USER_TPRI_INI);
// --- REMOVE_DCF_EXIF END ---
}

FW_VOID  BF_Fs_User_Custom_Chg_Pri_Dcf(FW_VOID)
{

// --- REMOVE_DCF_EXIF BEGIN ---
	OS_User_Chg_Pri(TID_DCF_RECEIVER, 29);
//	chg_pri(TID_FS_RECEIVER, 28);
// --- REMOVE_DCF_EXIF END ---

}

FW_VOID  BF_Fs_User_Custom_Recover_Pri_Dcf_With_User(FW_VOID)
{
// --- REMOVE_DCF_EXIF BEGIN ---
	OS_User_Chg_Pri(TID_DCF_RECEIVER, D_OS_USER_TPRI_INI);
// --- REMOVE_DCF_EXIF END ---
}

FW_VOID  BF_Fs_User_Custom_Sync_Pri_Dcf_With_User(FW_VOID)
{
	T_OS_USER_RTSK pk_rtsk;
	OS_USER_ID     tskid;

	OS_User_Get_Tid(&tskid);
	OS_User_Ref_Tsk(tskid, &pk_rtsk);
// --- REMOVE_DCF_EXIF BEGIN ---
	OS_User_Chg_Pri(TID_DCF_RECEIVER, pk_rtsk.tskpri);
// --- REMOVE_DCF_EXIF END ---
}

BOOL     BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv(FW_VOID)
{
	BOOL ret = FALSE;
// --- REMOVE_DCF_EXIF BEGIN ---
	OS_USER_ID tskid = 0;

	OS_User_Get_Tid(&tskid);
	ret = ( TID_DCF_RECEIVER == tskid );
// --- REMOVE_DCF_EXIF END ---
	return ret;
}

FW_INT32 BF_Fs_User_Custom_Pget_Blf_Mbox_Dcf(FW_VOID* pp)
{
	return OS_User_Pget_Mpf(MPFID_DCF_IF_MSG, (OS_USER_VP*)pp);
}

FW_INT32 BF_Fs_User_Custom_Rel_Blf_Mbox_Dcf(FW_VOID* p)
{
	if (p == NULL) {
		while (1){
			OS_User_Dly_Tsk(10);
		}
	}

	return OS_User_Rel_Mpf(MPFID_DCF_IF_MSG, (OS_USER_VP)p);
}

FW_INT32 BF_Fs_User_Custom_Ref_Mbx_Dcf(FW_VOID** pp)
{
	OS_USER_ER		ercd;
	T_OS_USER_RMBX	pk_rmbx;

	ercd = OS_User_Ref_Mbx(MID_DCF_IF, &pk_rmbx);
	if (pk_rmbx.pk_msg == NULL) {
		return D_OS_USER_E_TMOUT;
	}

	*pp = pk_rmbx.pk_msg;
	return ercd;
}

FW_INT32 BF_Fs_User_Custom_Ref_Mpf_Cnt_Dcf(FW_VOID)
{
	T_OS_USER_RMPF	pk_rmpf;
	OS_USER_ER		ercd;

	ercd = OS_User_Ref_Mpf(MPFID_DCF_IF_MSG, &pk_rmpf);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}

	return pk_rmpf.fblkcnt;
}

FW_INT32 BF_Fs_User_Custom_Snd_Msg_Dcf(FW_VOID* p)
{
	return OS_User_Snd_Mbx(MID_DCF_IF, (T_OS_USER_MSG*)p);
}

FW_INT32 BF_Fs_User_Custom_Prcv_Msg_Dcf(FW_VOID** pp)
{
	return OS_User_Prcv_Mbx(MID_DCF_IF, (T_OS_USER_MSG**)pp);
}

FW_INT32 BF_Fs_User_Custom_Rcv_Msg_Dcf(FW_VOID** pp)
{
	return OS_User_Rcv_Mbx(MID_DCF_IF, (T_OS_USER_MSG**)pp);
}

FW_INT32 BF_Fs_User_Custom_Clr_Msg_Dcf(FW_VOID)
{
	OS_USER_ER	ercd;
	FW_VOID* pp;
// --- REMOVE_DCF_EXIF BEGIN ---
	OS_USER_ID tskid;
// --- REMOVE_DCF_EXIF END ---

	for (;;) {
		ercd = OS_User_Prcv_Mbx(MID_DCF_IF, (T_OS_USER_MSG**)&pp);
		if (ercd == D_OS_USER_E_TMOUT) {
			return D_OS_USER_E_OK;
		}

		if (ercd != D_OS_USER_E_OK) {
			return ercd;
		}

// --- REMOVE_DCF_EXIF BEGIN ---
		if (0 == ((T_DCF_SENDER_MSG*)pp)->entry_func) {
			tskid = ((T_DCF_SENDER_MSG*)pp)->req_tskid;
			BF_Fs_User_Custom_Wup_Tsk(tskid);
		}
// --- REMOVE_DCF_EXIF END ---
		OS_User_Rel_Mpf(MPFID_DCF_IF_MSG, pp);
	}
}

FW_VOID  BF_Fs_User_Custom_Set_Media_Interrupt(FW_UCHAR ch)
{
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
	if (ch == 0) {
		// SD ch0
		/* Clock Stop */
		Dd_Top_Set_CLKSTOP2_UHS1CK2(1);

		// SD Insert/Remove Detect Port set
		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PP0, 1);
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PP0, 1);
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PP0, 0);
		Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PP0, 0);

		// EXSTOP macro card detect
		// SD Detection ch0 Reset
		Dd_Exs_Set_SD4_CD_Debounce_Soft_Reset(0);
		// Debounce Time setting
		Dd_Exs_Set_DEBEN_SD4DETIM(1);
		Dd_Exs_Set_DEBEN_SD4_EN(1);
		// SD Detection ch0 Reset clear
		Dd_Exs_Set_SD4_CD_Debounce_Soft_Reset(1);

		BF_Fs_User_Custom_Dly_Tsk(1);

		// status clear
		Dd_Exs_Clear_INTSTAT_SD4_RIS();
		Dd_Exs_Clear_INTSTAT_SD4_FIS();
		// intmask clear
		Dd_Exs_Set_INTMSK_SD4_RIM(0);
		Dd_Exs_Set_INTMSK_SD4_FIM(0);
	}
#endif	// for Linux Co-op

#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
	if (ch == 1) {
		// SD ch1
		/* Clock Stop */
		Dd_Top_Set_CLKSTOP2_UHS1CK1(1);

		// SD Detection ch1 Reset
		Dd_Exs_Set_SD3_CD_Debounce_Soft_Reset(0);
		// Debounce Time setting
		Dd_Exs_Set_DEBEN_SD3DETIM(1);
		Dd_Exs_Set_DEBEN_SD3_EN(1);
		// SD Detection ch0 Reset clear
		Dd_Exs_Set_SD3_CD_Debounce_Soft_Reset(1);

		BF_Fs_User_Custom_Dly_Tsk(1);

		// status clear
		Dd_Exs_Clear_INTSTAT_SD3_RIS();
		Dd_Exs_Clear_INTSTAT_SD3_FIS();
		// intmask clear
		Dd_Exs_Set_INTMSK_SD3_RIM(0);
		Dd_Exs_Set_INTMSK_SD3_FIM(0);
	}
#endif	// for Linux Co-op
}

FW_VOID  BF_Fs_User_Custom_Set_SD_Port(FW_UCHAR ch)
{
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
	if (ch == 0) {
		// SD ch0
		/* Clock Stop */
		Dd_Top_Set_CLKSTOP2_UHS1CK2(1);

		// SD0
		/* Port P */
		// PUDCR
		// PP0(SDCDX)  = 1 : Pull-Up
		// PP1(SDWP)   = 1 : Pull-Up
		// PP2(SDLEDX) = 0 : Pull-Down
		// PP3(SDSW)   = 0 : Pull-Down
		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PP0, 1);
		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PP1, 1);
		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PP2, 0);
		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PP3, 0);

		// PUDENR
		// PP0(SDCDX)  = 1 : Pull-Up Enable
		// PP1(SDWP)   = 1 : Pull-Up Enable
		// PP2(SDLEDX) = 0 : Pull-Down Disable
		// PP3(SDSW)   = 0 : Pull-Down Disable
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PP0, 1);
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PP1, 1);
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PP2, 0);
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PP3, 0);

		// EPCR
		// PP0(SDCDX)  = 0 : Resource(Input)
		// PP1(SDWP)   = 0 : Resource(Input)
		// PP2(SDLEDX) = 1 : Resource
		// PP3(SDSW)   = 1 : Resource
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PP0, 0);
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PP1, 0);
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PP2, 1);
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PP3, 1);

		// DDR
		// PP0(SDCDX)  = 0 : Input
		// PP1(SDWP)   = 0 : Input
		// PP2(SDLEDX) = 1 : don't care
		// PP3(SDSW)   = 1 : don't care
		Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PP0, 0);
		Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PP1, 0);

		// MSEL
		Dd_Top_Set_MSELC_MSEL((Dd_Top_Get_MSELC_MSEL() & ~0x02 ));

		// PERSEL
		Dd_Top_Set_PERSEL3_S3SWP15(0);
		Dd_Top_Set_PERSEL3_S3LSOU5WP14(0);
		Dd_Top_Set_PERSEL4_SD3CDXS(0);
		Dd_Top_Set_PERSEL4_SD3WPS(0);

		// SD Power SW
		Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_P65, 0);
		Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P65, 1);

		// SD0 IO Drive Control
		// Driver type fixed Type B.
	}
#endif	// for Linux Co-op
#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
	if( ch == 1) {
		// SD ch1
		/* Clock Stop */
		Dd_Top_Set_CLKSTOP2_UHS1CK1(1);

		// MSEL
		Dd_Top_Set_MSELC_MSEL((Dd_Top_Get_MSELC_MSEL() & ~0x01 ));

		// SD Power SW
		Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_P64, 0);
		Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P64, 1);

		// SD1 IO Drive Control
		// Driver type fixed Type B.
	}
#endif	// for Linux Co-op
}

FW_VOID  BF_Fs_User_Custom_Set_NF_Port(FW_VOID)
{
#ifdef CO_NF_DRIVER_ENABLE // for Linux Co-op
	Dd_Top_Set_PUDCNT_NFDATPUE(0);
	Dd_Top_Set_PUDCNT_NFRBXPUE(1);
	Dd_Top_Set_PUDCNT_NFDQSPUE(1);
#endif	// for Linux Co-op
}

FW_VOID  BF_Fs_User_Custom_Set_EMMC_Port(FW_VOID)
{
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
	Dd_Top_Set_PUDCNT_EMMCNICS(1);
	Dd_Top_Set_PUDCNT_EMDATPUE(1);
	Dd_Top_Set_PUDCNT_EMCMDPUE(1);
	Dd_Top_Set_PUDCNT_EMDSPDE(1);
#endif
}

FW_VOID  BF_Fs_User_Custom_Start_SD0_Clock(FW_VOID)
{
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
	// SD ch0

	// Start SD Clock
	Dd_Top_Set_CLKSTOP2_UHS1CK2(0);
	Dd_Exs_Set_SD4_BUS_CLOCK_Stop_Ctrl(0);

	// SD INT status clear
	Media_Custom_SD0_Crear_State();
#endif	// for Linux Co-op
}

FW_VOID  BF_Fs_User_Custom_Start_SD1_Clock(FW_VOID)
{
#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
	// SD ch1

	// Start SD Clock
	Dd_Top_Set_CLKSTOP2_UHS1CK1(0);
	Dd_Top_Set_CLKSTOP2_UHS2CK(0);
	Dd_Exs_Set_SD3_BUS_CLOCK_Stop_Ctrl(0);

	Media_Custom_SD1_Crear_State();
#endif	// for Linux Co-op
}



FW_INT32  BF_Fs_User_Custom_Start_NF_Clock(FW_SHORT timeout_XDMAC, FW_UCHAR exception_flg)
{
	if (bf_fs_user_custom_set_axi_clock_xdmac(D_BF_FS_USER_CUSTOM_AXI_CLOCK_ON, (FW_ULONG)timeout_XDMAC, exception_flg) != 0) {	/* pgr0821 */
		BF_Fs_User_Custom_Err_Printer(("MWNF : ERROR :BF_Fs_User_Custom_Start_NF_Clock() \n"));
		return -1;
	}
	Dd_Top_Set_CLKSTOP2_NFCK(0);
	Dd_Top_Set_CLKSTOP11_NFBCHCK(0);
	Dd_Exs_Set_NF_BUS_CLOCK_Stop_Ctrl(0);

	return 0;
}

FW_VOID  BF_Fs_User_Custom_Start_EMMC_Clock(FW_VOID)
{
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
	// Start SD Clock
	Dd_Top_Set_CLKSTOP2_EMMCCK(0);
	Dd_Exs_Set_EMMC_BUS_CLOCK_Stop_Ctrl(0);
	Dd_Exs_Set_EMMC_CKSTP_EMCLKSTP(0);
#endif	// for Linux Co-op
}

FW_VOID  BF_Fs_User_Custom_Start_RELC_Clock(FW_VOID)
{
	bf_fs_user_custom_set_ahb_clock_hdmac(D_BF_FS_USER_CUSTOM_AHB_CLOCK_ON);

	// Start RELC AHB Bus Clock
#if 0	// ### begin MILB_Porting
	Dd_Top_Set_CLKSTOP_RELCAH(START_CLK);
#endif	// ### end MILB_Porting
}

FW_VOID  BF_Fs_User_Custom_Stop_SD0_Clock(FW_VOID)
{
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
	// SD ch0
	Dd_Exs_Set_SD4_BUS_CLOCK_Stop_Ctrl(1);
	Dd_Top_Set_CLKSTOP2_UHS1CK2(1);
#endif	// for Linux Co-op
}

FW_VOID  BF_Fs_User_Custom_Stop_SD1_Clock(FW_VOID)
{
#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
	// SD ch1
	Dd_Exs_Set_SD3_BUS_CLOCK_Stop_Ctrl(1);
	Dd_Top_Set_CLKSTOP2_UHS1CK1(1);
#endif	// for Linux Co-op
}

FW_INT32  BF_Fs_User_Custom_Stop_NF_Clock(FW_SHORT timeout_NF, FW_SHORT timeout_XDMAC, FW_UCHAR exception_flg)
{
	FW_ULONG start_time;
	FW_INT32 ret;

	start_time = BF_Fs_User_Custom_Start_Abort_Timer(exception_flg);
	ret = -1;
	do {
		if (Dd_Exs_Get_NF_BTSQ_STATE_CTRL_BUSY() == NO_REQUESTED) {
			break;
		}
		ret = BF_Fs_User_Custom_Check_Abort_Timer((FW_ULONG)timeout_NF, start_time, exception_flg);	/* pgr0821 */
	} while (ret == -1);

	if (ret == 0) {
		BF_Fs_User_Custom_Err_Printer(("MWNF : ERROR :BF_Fs_User_Custom_Stop_NF_Clock() route:001.\n"));
		return -1;
	}

	Dd_Exs_Set_NF_BUS_CLOCK_Stop_Ctrl(1);
	Dd_Top_Set_CLKSTOP2_NFCK(1);
	Dd_Top_Set_CLKSTOP11_NFBCHCK(1);

	if (bf_fs_user_custom_set_axi_clock_xdmac(D_BF_FS_USER_CUSTOM_AXI_CLOCK_OFF, (FW_ULONG)timeout_XDMAC, exception_flg) != 0) {	/* pgr0821 */
		BF_Fs_User_Custom_Err_Printer(("MWNF : ERROR :BF_Fs_User_Custom_Stop_NF_Clock() route:002.\n"));
		return -1;
	}

	return 0;
}


FW_VOID  BF_Fs_User_Custom_Stop_EMMC_Clock(FW_VOID)
{
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
	Dd_Exs_Set_EMMC_CKSTP_EMCLKSTP(1);
	Dd_Exs_Set_EMMC_BUS_CLOCK_Stop_Ctrl(1);
	Dd_Top_Set_CLKSTOP2_EMMCCK(1);
#endif	// for Linux Co-op
}


FW_VOID  BF_Fs_User_Custom_Stop_RELC_Clock(FW_VOID)
{
	bf_fs_user_custom_set_ahb_clock_hdmac(D_BF_FS_USER_CUSTOM_AHB_CLOCK_OFF);

	// Start RELC AHB Bus Clock
#if 0	// ### begin MILB_Porting
	Dd_Top_Set_CLKSTOP_RELCAH(STOP_CLK);
#endif	// ### end MILB_Porting
}

/**
* @brief  Get system time.
* @param  system time(OUT)
* @return FW_VOID
*/
FW_VOID BF_Fs_User_Custom_Get_Time(T_SYSTEM_TIME* time)
{
	OS_USER_SYSTIM	systime;
	OS_User_Get_Tim(&systime);

	time->utime = (FW_SHORT)(systime >> 32);
	time->ltime = (FW_ULONG)systime;
}

/**
* @brief  Get system time(us).
* @param  system time(OUT)
* @return FW_VOID
*/
FW_VOID BF_Fs_User_Custom_Get_Time_us(FW_ULONG* time)
{
	*time = BF_Get_Time_us();
}

/**
* @brief Set abort start timer.
* @param FW_VOID
* @return abort: Abort timer start value
*/
FW_ULONG BF_Fs_User_Custom_Start_Abort_Timer(FW_UCHAR exception_flg)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	FW_ULONG start_time = 0;
	T_SYSTEM_TIME systime;

	if (exception_flg == 0) {
		BF_Fs_User_Custom_Get_Time(&systime);

		start_time = systime.ltime;
	}
	else {
		Dd_ARM_Critical_Section_Start( critseq );
		gNF_timeout_cnt = 0;
		Dd_ARM_Critical_Section_End( critseq );
	}

	return start_time;
}

/**
* @brief Check abort timeout.
* @param timout:Timeout time from timer start(ms)
*        start_time: Abort timer start value
* @return 0(timeout)/-1(timer operating)
*/
FW_INT32 BF_Fs_User_Custom_Check_Abort_Timer(FW_ULONG timout, FW_ULONG start_time, FW_UCHAR exception_flg)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	T_SYSTEM_TIME systime;
	FW_INT32 ret = 0;

	if (exception_flg == 0) {
		BF_Fs_User_Custom_Get_Time(&systime);
		if ((systime.ltime >= start_time) && (systime.ltime <= (start_time + timout))) {
			ret = -1;
		}
	}
	else {
		Dd_ARM_Critical_Section_Start( critseq );
		if (gNF_timeout_cnt == (timout * (ULONG)D_BF_FS_USER_CUSTOM_CNT_RATE)) {
			gNF_timeout_cnt = 0;
		}
		else {
			gNF_timeout_cnt++;
			ret = -1;
		}
		Dd_ARM_Critical_Section_End( critseq );
	}

	return ret;
}
