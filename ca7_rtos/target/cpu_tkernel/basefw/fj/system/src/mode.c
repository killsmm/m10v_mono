/**
 * @file		mode.c
 * @brief		mode process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "TKernel_id.h"
#include "ddim_typedef.h"
#include "os_user_custom.h"
#include "querystatus.h"
#include "fj_host.h"
#include "fj_mode.h"
#include "mode.h"
#include "debug.h"
#include "boot_loader.h"
#include "dd_arm.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define gBF_Mode_Check_List_Buf	(64)

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
T_MODE_ATTR gBF_Mode_Attr = {
	FJ_MODE_INITIALIZER,		/* prev_mode */
	FJ_MODE_BOOT,				/* current_mode */
	FJ_MODE_INITIALIZER,		/* next_mode */
	FJ_MODE_INITIALIZER,		/* req_mode */
	(UCHAR)FJ_SR1_NULL, 		/* sr1 */
	0,							/* sr2 */
};

INT32 gBF_Mode_BaseFW_API_Cnt;

static UINT32 gBF_Mode_Check_Point_List[gBF_Mode_Check_List_Buf];
static INT32 gBF_Mode_Check_Point_Position;

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
static FJ_ERR_CODE bf_mode_check_start_condition( VOID );
static FJ_ERR_CODE bf_mode_check_stop_condition( VOID );

/**
 *	@brief		This function checks a mode status before a mode change start.\n
 *	@return		FJ_ERR_OK		Success
 *	@return		not FJ_ERR_OK	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
static FJ_ERR_CODE bf_mode_check_start_condition( VOID )
{
	FJ_ERR_CODE ret = FJ_ERR_OK;

	/*------------------------------------------------------------------*/
	/*  Check current system mode status								*/
	/*  Don't change to new mode if current mode is not stop status.	*/
	/*------------------------------------------------------------------*/
	if( gBF_Mode_Attr.next_mode != FJ_MODE_STOP ) {
		if( gBF_Mode_Attr.current_mode != FJ_MODE_BOOT ) {	/* Except Boot case. */
			BF_Debug_Print_Error(("bf_mode_check_start_condition() Error: cur_mode=0x%08x, next_mode(stop)=0x%08x, \n", gBF_Mode_Attr.current_mode, gBF_Mode_Attr.next_mode));
			ret = FJ_ERR_NG;
		}
	}
	
	switch( gBF_Mode_Attr.req_mode ){
		case FJ_MODE_PLAYBACK:
		case FJ_MODE_CALIBRATION:
			if( gBF_Mode_Attr.current_mode != FJ_MODE_IDLE ) {
				BF_Debug_Print_Information(("bf_mode_check_start_condition() : This state transition has not been permitted excluding Idle mode. \n"));
				ret = FJ_ERR_NG;
			}
			break;
		case FJ_MODE_IDLE:
		case FJ_MODE_CONTI_CAPTURE:
		case FJ_MODE_PHOTO:
		case FJ_MODE_BURST_CAPTURE:
		case FJ_MODE_STILL_HDR:
		case FJ_MODE_VIDEO:
		default:
			break;
	}
	
	return ret;
}

/**
 *	@brief		This function checks a mode status before a mode change stop.\n
 *	@return		FJ_ERR_OK		Success
 *	@return		not FJ_ERR_OK	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
static FJ_ERR_CODE bf_mode_check_stop_condition( VOID )
{
	FJ_ERR_CODE ret = FJ_ERR_OK;

	/*------------------------------------------------------------------*/
	/*  Check current system mode status								*/
	/*  Don't change to stop status if current status is not starting.	*/
	/*------------------------------------------------------------------*/
	if( gBF_Mode_Attr.next_mode != FJ_MODE_INITIALIZER ) {	/* Not stop status or new mode executing */
		BF_Debug_Print_Error(("bf_mode_check_stop_condition() Error: cur_mode=0x%08x, next_mode(stop)=0x%08x, \n", gBF_Mode_Attr.current_mode, gBF_Mode_Attr.next_mode));
		return FJ_ERR_NG;
	}
	else {
		if( gBF_Mode_Attr.current_mode == FJ_MODE_BOOT ) {	/* Except Boot case. */
			BF_Debug_Print_Error(("bf_mode_check_stop_condition() Error: Don't stop during Boot. \n"));
			return FJ_ERR_NG;
		}
	}

	return ret;
}


CHAR* bf_mode_conv_val2str( UINT32 mode )
{
	switch( mode ) {
		case FJ_MODE_PLAYBACK:
			return "PLAYBACK";
		case FJ_MODE_CONTI_CAPTURE:
			return "CONTI_CAPTURE";
		case FJ_MODE_PHOTO:
			return "PHOTO";
		case FJ_MODE_BURST_CAPTURE:
			return "BURST_CAPTURE";
		case FJ_MODE_STILL_HDR:
			return "STILL_HDR";
		case FJ_MODE_CALIBRATION:
			return "CALIBRATION";
		case FJ_MODE_VIDEO:
			return "VIDEO";
		case FJ_MODE_IDLE:
			return "IDLE";
		case FJ_MODE_BOOT:
			return "BOOT";
		case FJ_MODE_UNMOUNT:
			return "UNMOUNT";
		case FJ_MODE_STOP:
			return "STOP";
		case FJ_MODE_INITIALIZER:
			return "INITIALIZER";
		default:
			return "UNKNOWN";
	}
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 *	@brief		This function sets the request mode.\n
 *	@param		i:bNewMode		The New Mode\n
 *	@return		FJ_ERR_OK		Success
 *	@return		not FJ_ERR_OK	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	fj_setmode( FJ_MODE bNewMode )
{
	FJ_ERR_CODE	ret = FJ_ERR_OK;

	BF_Debug_Print_Information(("### fj_setmode(): prev=%s(0x%08x), curr=%s(0x%08x), next=%s(0x%08x), req=%s(0x%08x). \n", 
								bf_mode_conv_val2str(gBF_Mode_Attr.prev_mode), gBF_Mode_Attr.prev_mode, 
								bf_mode_conv_val2str(gBF_Mode_Attr.current_mode), gBF_Mode_Attr.current_mode, 
								bf_mode_conv_val2str(gBF_Mode_Attr.next_mode), gBF_Mode_Attr.next_mode, 
								bf_mode_conv_val2str(gBF_Mode_Attr.req_mode), gBF_Mode_Attr.req_mode ));

	/*------------------------------------------------------------------*/
	/*  Check current system mode status								*/
	/*  Don't change to new mode if current mode is not stop status.	*/
	/*------------------------------------------------------------------*/
	if( gBF_Mode_Attr.next_mode != FJ_MODE_STOP ) {
		if( gBF_Mode_Attr.current_mode != FJ_MODE_BOOT ) {	/* Except Boot case. */
			BF_Debug_Print_Error(("fj_setmode() Error: cur_mode=%s(0x%08x), req_mode=%s(0x%08x), \n", 
								bf_mode_conv_val2str(gBF_Mode_Attr.current_mode), gBF_Mode_Attr.current_mode, 
								bf_mode_conv_val2str(bNewMode), bNewMode));
			return FJ_ERR_NG;
		}
	}

	/* Set requested system mode */
	BF_Mode_Set_Request_Sysmode( bNewMode );

	return ret;
}

/**
 *	@brief		This function sets the request mode.\n
 *				FJ does not check the correctness of the transition.\n
 *				It is the responsibility of the host. The host has to wait until previous mode\n
 *				transition has been completed.\n
 *				The notification is by FUJI_HM_NEW_MODE message.\n
 *				This message notifies the host that transition has actually been complete.\n
 *				At the same time, the "Current mode" field of Sr1 will be updated.\n
 *	@return		FJ_ERR_OK		Success
 *	@return		not FJ_ERR_OK	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	fj_startmode( VOID )
{
	FJ_ERR_CODE	ret = FJ_ERR_OK;

	BF_Debug_Print_Information(("### fj_startmode(): prev=%s(0x%08x), curr=%s(0x%08x), next=%s(0x%08x), req=%s(0x%08x) .\n", 
								bf_mode_conv_val2str(gBF_Mode_Attr.prev_mode), gBF_Mode_Attr.prev_mode, 
								bf_mode_conv_val2str(gBF_Mode_Attr.current_mode), gBF_Mode_Attr.current_mode, 
								bf_mode_conv_val2str(gBF_Mode_Attr.next_mode), gBF_Mode_Attr.next_mode, 
								bf_mode_conv_val2str(gBF_Mode_Attr.req_mode), gBF_Mode_Attr.req_mode ));

	ret = bf_mode_check_start_condition();

	/*----------------------------------------------------------*/
	/*  Update next system mode to start new mode				*/
	/*----------------------------------------------------------*/
	if(ret == FJ_ERR_OK ) {
		if( (gBF_Mode_Attr.next_mode == FJ_MODE_STOP) && (gBF_Mode_Attr.req_mode == FJ_MODE_INITIALIZER) ) {
			// Case of Stop to Restart current mode
			BF_Mode_Set_Request_Sysmode( (FJ_MODE)gBF_Mode_Attr.current_mode );
		}
		/* Set next system mode */
		BF_Mode_Set_Next_Sysmode( (FJ_MODE)gBF_Mode_Attr.req_mode );

		/* Send Message to Base F/W Mode Task */
//		ret = BF_Control_Snd_Iif_Msg_Without_Param( E_BF_CONTROL_IIF_APIID_MODE_CHANGE );
		if( ret != FJ_ERR_OK ){
			BF_Debug_Print_Error(("fj_startmode() : BF_Control_Snd_Iif_Msg_Without_Param error. ret=%d\n", ret));
		}
	}

	return ret;
}

/**
 *	@brief		This function sets the request mode.\n
 *	@return		FJ_ERR_OK		Success
 *	@return		not FJ_ERR_OK	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	fj_stopmode( VOID )
{
	FJ_ERR_CODE	ret = FJ_ERR_OK;

	BF_Debug_Print_Information(("### fj_stopmode(): prev=%s(0x%08x), curr=%s(0x%08x), next=%s(0x%08x), req=%s(0x%08x) .\n", 
								bf_mode_conv_val2str(gBF_Mode_Attr.prev_mode), gBF_Mode_Attr.prev_mode, 
								bf_mode_conv_val2str(gBF_Mode_Attr.current_mode), gBF_Mode_Attr.current_mode, 
								bf_mode_conv_val2str(gBF_Mode_Attr.next_mode), gBF_Mode_Attr.next_mode, 
								bf_mode_conv_val2str(gBF_Mode_Attr.req_mode), gBF_Mode_Attr.req_mode ));

	ret = bf_mode_check_stop_condition();

	if(ret == FJ_ERR_OK ) {
		/* Set stop staus */
		BF_Mode_Set_Request_Sysmode( FJ_MODE_STOP );
		BF_Mode_Set_Next_Sysmode( (FJ_MODE)gBF_Mode_Attr.req_mode);

		/* Send Message to Base F/W Mode Task */
//		ret = BF_Control_Snd_Iif_Msg_Without_Param( E_BF_CONTROL_IIF_APIID_MODE_CHANGE );
		if( ret != FJ_ERR_OK ){
			BF_Debug_Print_Error(("fj_stopmode() : BF_Control_Snd_Iif_Msg_Without_Param error. ret=%d\n", ret));
		}

		switch(gBF_Mode_Attr.next_mode){
			case FJ_MODE_STOP:
//				BF_Control_Abort_VD( 0 );
//				BF_Control_Abort_VD( 1 );
//				BF_Control_Abort_VD( 2 );
//				BF_Control_Abort_VD( 3 );
				break;

			default:
				break;
		}
	}

	return ret;
}

/**
 *	@brief		This function checks whether the stop status.\n
 *	@return		TRUE  : Stop Mode
 *	@return		FALSE : not Stop Mode
 *	@note		None
 *	@attention	None
 */
BOOL	fj_is_modestatus_stop( VOID )
{
	BOOL stop_status = FALSE;

	if( gBF_Mode_Attr.next_mode == FJ_MODE_STOP ) {
		stop_status = TRUE;
	}

	return stop_status;
}

/**
 *	@brief		Wait Mode Change finished
 *	@param		i:wait_mode		waiting mode
 *	@param		i:sleepTime		sleeping time
 *	@param		i:repeatNum		number of repeat
 *	@return		FJ_ERR_OK	Completion
 *	@return		FJ_ERR_NG	Completion(Failiure) or T.O.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	fj_waitmodecomp( FJ_MODE wait_mode, UINT32 sleepTime, UINT32 repeatNum )
{
#if 0
	UINT32 loop_num;

	for( loop_num = 0; loop_num < repeatNum; loop_num++ ){
		if( gBF_Mode_Attr.current_mode == wait_mode ){
			switch( gBF_Mode_Attr.sr1 ){
				case FJ_SR1_SUCCESS:
					return FJ_ERR_OK;

				case FJ_SR1_FAILURE:
					return FJ_ERR_NG;

				case FJ_SR1_NULL:
				default:
					/* Not completed */
					break;
			}
		}
		OS_User_Tslp_Tsk( sleepTime );
	}
#else
	UINT32 loop_num;
	OS_USER_FLGPTN flgptn;

	for( loop_num = 0; loop_num < repeatNum; loop_num++ ){
		OS_User_Twai_Flg( FID_SYSTEM_MODE, D_BF_MODE_WAIT_SYSTEM_MODE, D_OS_USER_TWF_ORW, &flgptn, sleepTime );
		OS_User_Clr_Flg( FID_SYSTEM_MODE, ~D_BF_MODE_WAIT_SYSTEM_MODE );

		if( gBF_Mode_Attr.current_mode == wait_mode ){
			switch( gBF_Mode_Attr.sr1 ){
				case FJ_SR1_SUCCESS:
					return FJ_ERR_OK;

				case FJ_SR1_FAILURE:
					return FJ_ERR_NG;

				case FJ_SR1_NULL:
				default:
					/* Not completed */
					break;
			}
		}
	}
#endif

	return FJ_ERR_NG;
}

/**
 *	@brief		Gets the Sr1 and Sr2 status words.\n
 *	@param		i/o:pwUSR1		Sr1 status register pointer(mode).
 *	@param		i/o:pwUSR2		Sr2 status register pointer(status).
 *	@return		FJ_ERR_OK		Success
 *	@return		not FJ_ERR_OK	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	fj_querystatus( UINT32* pwUSR1, USHORT* pwUSR2 )
{
	UINT32 mode;
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	if( (pwUSR1 == NULL) || (pwUSR2 == NULL ) ){
		return FJ_ERR_NG;
	}

	Dd_ARM_Critical_Section_Start( critseq );
	switch( gBF_Mode_Attr.current_mode ){
		case FJ_MODE_DISP_IMAGE_THUMB:
		case FJ_MODE_DISP_MONCLS1_MAIN:
		case FJ_MODE_DISP_MONCLS1_THUMB:
		case FJ_MODE_DISP_MONCLS2_MAIN:
		case FJ_MODE_DISP_MONCLS2_THUMB:
			mode = FJ_MODE_DISP_IMAGE_MAIN;
			break;

		case FJ_MODE_MENU:
		case FJ_MODE_BOOT:
		case FJ_MODE_POWER_DOWN:
		case FJ_MODE_IDLE:
		case FJ_MODE_VIEW:
		case FJ_MODE_CAPTURE_DRAFT:
		case FJ_MODE_PREVIEW_ZOOM:
		case FJ_MODE_PHOTO:
		case FJ_MODE_BURST_CAPTURE:
		case FJ_MODE_STILL_HDR:
		case FJ_MODE_CAPTURE:
		case FJ_MODE_CAPTURE_NIGHT:
		case FJ_MODE_CAPTURE_SEQ:
		case FJ_MODE_STORE_IMAGE_FLASH:
		case FJ_MODE_DISP_IMAGE_MAIN:
		case FJ_MODE_PLAYBACK:
		case FJ_MODE_DISP_THUMB_DRAW:
		case FJ_MODE_DISP_THUMBINDEX:
		case FJ_MODE_PLAYBACK_THUMB:
		default:
			mode = (UINT32)gBF_Mode_Attr.current_mode;
			break;
	}
	*pwUSR1 = mode;
	*pwUSR1 = (*pwUSR1 << 8 );
	*pwUSR1 |= gBF_Mode_Attr.sr1;
	*pwUSR2 = gBF_Mode_Attr.sr2;

	/* Clear SR2 Parameters */
	gBF_Mode_Attr.sr2 &= ~0x40CF;	/* 0,1,2,3,6,7,14 */
	Dd_ARM_Critical_Section_End( critseq );
	return FJ_ERR_OK;
}

/**
 *	@brief		Gets Status Register.\n
 *	@param		i:bRegister		The kind of Status Register
 *	@param		i/o:pwValue		Pointer of Status register value.
 *	@return		FJ_ERR_OK		Success
 *	@return		FJ_ERR_NG		Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	fj_getstatusregister( FJ_STATUS_REGISTER bRegister, USHORT* pwValue )
{
	UINT32 wUsr1;
	USHORT wUsr2;
	FJ_ERR_CODE	ret;

	if( pwValue == NULL ){
		return FJ_ERR_NG;
	}

	ret = fj_querystatus( &wUsr1, &wUsr2 );
	if( ret != FJ_ERR_OK ){
		return ret;
	}

	switch( bRegister ){
		case FJ_STATUS_REGISTER_SR1:
			*pwValue = (USHORT)wUsr1;
			break;

		case FJ_STATUS_REGISTER_SR2:
			*pwValue = wUsr2;
			break;

		default:
			return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 * get Active system mode
 * @return FJ_MODE (previous system mode)
 */
FJ_MODE fj_get_act_sysmode(VOID)
{
	FJ_MODE	mode;
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	mode = (FJ_MODE)gBF_Mode_Attr.current_mode;
	Dd_ARM_Critical_Section_End( critseq );
	return mode;
}

/**
 *	@brief		Set SR1 Parameter for Base F/W.\n
 *	@param		i:sr1			sr1 parameter.
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Mode_Set_Sr1( UCHAR sr1 )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	gBF_Mode_Attr.sr1 = sr1;
	Dd_ARM_Critical_Section_End( critseq );
	return;
}


/**
 *	@brief		Set SR2 Parameter for Base F/W.\n
 *	@param		i:sr2			sr2 parameter
 *	@param		i:flag			OFF:0, ON:1
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Mode_Set_Sr2( UINT32 sr2, UCHAR flag )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	switch( sr2 ){
		case FJ_SR2_DZOOM_COMPLETED:
			/* 0 bit */
			if( flag == 0 ){	/* OFF */
				gBF_Mode_Attr.sr2 &= ~0x0001;
			}
			else{				/* ON */
				gBF_Mode_Attr.sr2 |= 0x0001;
			}
			break;

		case FJ_SR2_NO_AUDIO_DRAM:
			/* 1 bit */
			if( flag == 0 ){	/* OFF */
				gBF_Mode_Attr.sr2 &= ~0x0002;
			}
			else{				/* ON */
				gBF_Mode_Attr.sr2 |= 0x0002;
			}
			break;

		case FJ_SR2_OPE_COMPLETED:
			/* 2 bit */
			if( flag == 0 ){	/* OFF */
				gBF_Mode_Attr.sr2 &= ~0x0004;
			}
			else{				/* ON */
				gBF_Mode_Attr.sr2 |= 0x0004;
			}
			break;

		case FJ_SR2_NO_FALSHMEM:
			/* 3 bit */
			if( flag == 0 ){	/* OFF */
				gBF_Mode_Attr.sr2 &= ~0x0008;
			}
			else{				/* ON */
				gBF_Mode_Attr.sr2 |= 0x0008;
			}
			break;

		case FJ_SR2_NFP:
			/* 6 bit */
			if( flag == 0 ){	/* OFF */
				gBF_Mode_Attr.sr2 &= ~0x0040;
			}
			else{				/* ON */
				gBF_Mode_Attr.sr2 |= 0x0040;
			}
			break;

		case FJ_SR2_PIP_EFF_COMPLETED:
			/* 7 bit */
			if( flag == 0 ){	/* OFF */
				gBF_Mode_Attr.sr2 &= ~0x0080;
			}
			else{				/* ON */
				gBF_Mode_Attr.sr2 |= 0x0080;
			}
			break;

		case FJ_SR2_FLASH_LIGHT_REQ:
			/* 9 bit */
			if( flag == 0 ){	/* OFF */
				gBF_Mode_Attr.sr2 &= ~0x0200;
			}
			else{				/* ON */
				gBF_Mode_Attr.sr2 |= 0x0200;
			}
			break;

		case FJ_SR2_AE_CONV:
			/* 10 bit */
			if( flag == 0 ){	/* OFF */
				gBF_Mode_Attr.sr2 &= ~0x0400;
			}
			else{				/* ON */
				gBF_Mode_Attr.sr2 |= 0x0400;
			}
			break;

		case FJ_SR2_CARD_EXIST_IND:
			/* 13 bit */
			if( flag == 0 ){	/* OFF */
				gBF_Mode_Attr.sr2 &= ~0x2000;
			}
			else{				/* ON */
				gBF_Mode_Attr.sr2 |= 0x2000;
			}
			break;

		case FJ_SR2_CARD_INS_REM_INT:
			/* 14 bit */
			if( flag == 0 ){	/* OFF */
				gBF_Mode_Attr.sr2 &= ~0x4000;
			}
			else{				/* ON */
				gBF_Mode_Attr.sr2 |= 0x4000;
			}
			break;

		case FJ_SR2_AWB_CONV:
			/* 15 bit */
			if( flag == 0 ){	/* OFF */
				gBF_Mode_Attr.sr2 &= ~0x8000;
			}
			else{				/* ON */
				gBF_Mode_Attr.sr2 |= 0x8000;
			}
			break;

		default:
			break;
	}
	Dd_ARM_Critical_Section_End( critseq );
	return;
}

/********************************************************************/
/*	Base F/W Local API												*/
/********************************************************************/
/**
 * get previous system mode
 * @return FJ_MODE (previous system mode)
 */
FJ_MODE BF_Mode_Get_Prev_Sysmode(VOID)
{
	FJ_MODE	mode;
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	mode = (FJ_MODE)gBF_Mode_Attr.prev_mode;
	Dd_ARM_Critical_Section_End( critseq );
	return mode;
}

/**
 * get Active system mode
 * @return FJ_MODE (previous system mode)
 */
FJ_MODE BF_Mode_Get_Next_Sysmode(VOID)
{
	FJ_MODE	mode;
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	mode = (FJ_MODE)gBF_Mode_Attr.next_mode;
	Dd_ARM_Critical_Section_End( critseq );
	return mode;
}

/**
 * set next(new) system mode
 * @param i:FJ_MODE	next_mode
 * @attention This function is called when start of mode change.
 */
VOID BF_Mode_Set_Next_Sysmode( FJ_MODE next_mode )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	gBF_Mode_Attr.next_mode = gBF_Mode_Attr.req_mode;
	gBF_Mode_Attr.req_mode = FJ_MODE_INITIALIZER;

	// SR1 status clear
	gBF_Mode_Attr.sr1 = FJ_SR1_NULL;
	Dd_ARM_Critical_Section_End( critseq );

	return;
}

/**
 * set next(new) system mode
 * @param i:FJ_MODE	next_mode
 * @attention This function is called when start of mode change.
 */
VOID BF_Mode_Set_Request_Sysmode( FJ_MODE req_mode )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	gBF_Mode_Attr.req_mode = req_mode;
	Dd_ARM_Critical_Section_End( critseq );

	return;
}

/**
 * update system mode
 * @attention This function is called after all of mode change procedure.
 */
VOID BF_Mode_Update_Sysmode_Change( VOID )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	gBF_Mode_Attr.prev_mode		= gBF_Mode_Attr.current_mode;
	gBF_Mode_Attr.current_mode	= gBF_Mode_Attr.next_mode;
	gBF_Mode_Attr.next_mode		= FJ_MODE_INITIALIZER;
	Dd_ARM_Critical_Section_End( critseq );

	OS_User_Set_Flg( FID_SYSTEM_MODE, D_BF_MODE_WAIT_SYSTEM_MODE );

	return;
}

/**
 * cancel system mode
 * @attention This function is called when fail of mode change procedure.
 */
VOID BF_Mode_Cancel_Sysmode_Change(VOID)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	gBF_Mode_Attr.next_mode = FJ_MODE_INITIALIZER;
	Dd_ARM_Critical_Section_End( critseq );
	return;
}

FJ_ERR_CODE BF_Mode_Check( UINT32 validModeCategory)
{
	INT32 loop_cnt = 1000;
	CHAR wait = 0;
	CHAR code_check = 0;
	CHAR mode_check = 0;

//	printf(">>> %x\n", validModeCategory);
//	printf(">>  %x\n", boot_status << 28);
//	printf(">   %x\n", ((boot_status << 28) | (gBF_Mode_Attr.current_mode & 0xFFFF0000)));
//	printf("<   %x\n", validModeCategory & ((boot_status << 28) | (gBF_Mode_Attr.current_mode & 0xFFFF0000)));

	code_check = 1;

	if(((gBF_Mode_Attr.current_mode & 0xFFFF0000) & (validModeCategory & 0xFFFF0000)) != 0) {
		mode_check = 1;
	}

	if((code_check == 1) && (mode_check == 1)) {
		return FJ_ERR_OK;
	}
	else {
		switch(wait) {
			case 0:  // return error without wait
				return FJ_ERR_NG;

			case 1:  // wait forever
				while(1) {
					OS_User_Tslp_Tsk(1000);
				}

			case 2:  // wait valid condition forever
				while(1) {
					if(((gBF_Mode_Attr.current_mode & 0xFFFF0000) & (validModeCategory & 0xFFFF0000)) != 0) {
						mode_check = 1;
					}

					if((code_check == 1) && (mode_check == 1)) {
						return FJ_ERR_OK;
					}
					OS_User_Tslp_Tsk(30);
				}

			default:  //wait valid condition with timeout
				while(1) {
					if(((gBF_Mode_Attr.current_mode & 0xFFFF0000) & (validModeCategory & 0xFFFF0000)) != 0) {
						mode_check = 1;
					}

					if((code_check == 1) && (mode_check == 1)) {
						return FJ_ERR_OK;
					}
					OS_User_Tslp_Tsk(30);
					loop_cnt--;
					if(loop_cnt == 0){
						break;
					}
				}
		}
	}
	return FJ_ERR_OK;
}

VOID	BF_Mode_Put_CheckPoint(UINT32 category, USHORT checkpoint)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	gBF_Mode_Check_Point_Position++;
	if(gBF_Mode_Check_Point_Position == gBF_Mode_Check_List_Buf) {
		gBF_Mode_Check_Point_Position = 0;
	}
	gBF_Mode_Check_Point_List[gBF_Mode_Check_Point_Position] = (category | (UINT32)checkpoint);

	Dd_ARM_Critical_Section_End( critseq );
}

INT32	BF_Mode_Ref_Checkpoint(UINT32 category, USHORT checkpoint)
{
	INT32 i;

	for(i = gBF_Mode_Check_Point_Position; i >= 0; i--) {
		if(gBF_Mode_Check_Point_List[i] == (category | checkpoint)) {
			return gBF_Mode_Check_Point_Position - i;
		}
	}
	for(i = (gBF_Mode_Check_List_Buf-1); i > gBF_Mode_Check_Point_Position; i--) {
		if(gBF_Mode_Check_Point_List[i] == (category | checkpoint)) {
			return gBF_Mode_Check_List_Buf - i + gBF_Mode_Check_Point_Position;
		}
	}
	return -1;
}
