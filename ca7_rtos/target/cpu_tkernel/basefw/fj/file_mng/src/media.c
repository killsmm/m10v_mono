/**
 * @file		media.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "os_user_custom.h"

#include "fj_media.h"
#include "fj_boot.h"
#include "fj_host.h"
#include "still_attr.h"
#include "fs_user_func.h"
#ifdef CO_DCF_EXIF_ON
#include "exif_controller.h"
#include "dcf_db.h"
#include "exif_attr.h"
#endif // CO_DCF_EXIF_ON
#include "media_drv_wrapper.h"
#include "sdram_map_common.h"
#include "NF_driver_wrapper.h"
#include "mba_if.h"
#include "dynamic_scenario.h"
#include "mwsdio.h"
#include "fs_controller.h"
#include "bf_startup.h"

#include "boot_loader.h"
#include "media_attr.h"
#include "mwnf_core.h"
// delete\basefw\fj\imgproc\still
//#include "still_pro_ctrl.h"
#include "still.h"
#include "mwnf.h"
#include "nf_custom.h"

#include "emmc_custom.h"
#include "em_if_ext.h"
#include "em_int_cmn.h"
#include "em_format.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// Nothing Special

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
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
#ifndef CO_LINUX_VERSION_ON
static VOID nf_set_mode(UCHAR mode);

/**
 * @brief 		Set nf mode
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @return 		None
 */
static VOID nf_set_mode(UCHAR mode)
{
	if (mode == 1) {
		(VOID)MWNF_Set_DMA_Mode(D_FLAG_META_MODE_ON);
	}
	else {
		(VOID)MWNF_Set_DMA_Mode(D_FLAG_META_MODE_OFF);
	}

	(VOID)MWNF_Set_Ecc_Param(D_MWNF_ENABLE, D_MWNF_ECC_AUTO);
}
#endif // CO_LINUX_VERSION_ON

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 * @brief 		Get media detected status.
 * @param		FW_UCHAR*	detect : media detected state
 *							1 : media inserted
 *							0 : media not inserted
 * @return 		None
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_set_mediadetect(UCHAR ch, UCHAR time_per_sampling, UCHAR num_of_sampling)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	// parameter check
	if (ch >= D_BF_MEDIA_SD_MAX) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	ercd = BF_Fs_If_Set_Media_State((FW_UCHAR)ch, (FW_UCHAR)time_per_sampling, (FW_CHAR)num_of_sampling);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

FJ_ERR_CODE fj_get_mediadetect(UCHAR ch, UCHAR* detect)
{
#ifdef CO_LINUX_VERSION_ON
	*detect = 1;
	return FJ_ERR_OK;
#else
	// parameter check
	if (ch >= D_BF_MEDIA_SD_MAX) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	if (BF_Fs_If_Get_Media_State(ch) == E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
		*detect = 1;
	}
	else {
		*detect = 0;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

FJ_ERR_CODE fj_get_mediadetectbypoll(UCHAR ch, UCHAR* detect, UCHAR time_per_sampling, UCHAR num_of_sampling)
{
#ifdef CO_LINUX_VERSION_ON
	*detect = 1;
	return FJ_ERR_OK;
#else
	// parameter check
	if (ch >= D_BF_MEDIA_SD_MAX) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	if (BF_Fs_If_Get_Media_State_By_Poll(ch, (FW_UCHAR)time_per_sampling, (FW_CHAR)num_of_sampling) == E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
		*detect = 1;
	}
	else {
		*detect = 0;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Get Media(SD) WriteProtect Information.
 * @param		UCHAR*		pwWpInfo
 * @return 		FJ_ERR_CODE : FJ_ERR_OK Media Detect
 *                          : FJ_ERR_NG Media NonDetect
 *							: FJ_ERR_NOT_MOUNT SD not mount
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getwriteprotectstatus(UCHAR* pwWpInfo)
{
#ifdef CO_LINUX_VERSION_ON
	*pwWpInfo = FJ_MEDIA_NON_WP;
	return FJ_ERR_OK;
#else
	FW_INT32 wp;
	E_BF_MEDIA_STATE_MEDIA_TYPE media_type;

	media_type = BF_Media_State_Get_Used_Media();
	switch (media_type) {
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD:
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD1:
			break;

		default:
			*pwWpInfo = 1;		// Fix "Non Write Protect
			return FJ_ERR_NOT_MOUNT;
	}

	if (BF_Media_Attr_Get_Attr_Ext_Media() == E_BF_MEDIA_ATTR_EXT_SD) {
		if (BF_Fs_If_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD) == E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
			wp = MWSDIO0_CardCheckWP();	// Get SD Write Protect Information.

			if (wp == 0) {
				*pwWpInfo = FJ_MEDIA_WP;
			}
			else {
				*pwWpInfo = FJ_MEDIA_NON_WP;
			}
			return FJ_ERR_OK;
		}
		else {
			*pwWpInfo = 1;		// Fix "Non Write Protect
			return FJ_ERR_NG;
		}
	}
	else if (BF_Media_Attr_Get_Attr_Ext_Media() == E_BF_MEDIA_ATTR_EXT_SD1) {
		if (BF_Fs_If_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1) == E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
			wp = MWSDIO1_CardCheckWP();	// Get SD Write Protect Information.

			if (wp == 0) {
				*pwWpInfo = FJ_MEDIA_WP;
			}
			else {
				*pwWpInfo = FJ_MEDIA_NON_WP;
			}
			return FJ_ERR_OK;
		}
		else {
			*pwWpInfo = 1;		// Fix "Non Write Protect
			return FJ_ERR_NG;
		}
	}
	else {
		return FJ_ERR_NG;
	}
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		initialize the SD macro
 * @param		UCHAR ch
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_sd_init(UCHAR ch)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	// parameter check
	if (ch >= D_BF_MEDIA_SD_MAX) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	BF_Fs_User_Custom_Set_Media_Interrupt(ch);
	BF_Media_Drv_Wrapper_Set_Interrupt(ch);
	BF_Fs_User_Custom_Set_SD_Port(ch);

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Set limit of the SD drive strength, speed mode and clock
 * @param		UCHAR ch
 * @param		FJ_MEDIA_SD_DRIVE drive
 * @param		FJ_MEDIA_SD_MODE mode
 * @param		FJ_MEDIA_SD_CLOCK clock
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_sd_config(UCHAR ch, FJ_MEDIA_SD_DRIVE drive, FJ_MEDIA_SD_MODE mode, FJ_MEDIA_SD_CLOCK clock)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;
	ULONG sd_clk = 1000;

	// parameter check
	if (ch >= D_BF_MEDIA_SD_MAX) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	if (ch == 0) {
		// Driver type
		MWSDIO0_Set_Driving_Force(drive);

		// Speed mode, max clock
		switch (clock) {
			case FJ_MEDIA_SD_CLOCK_25M:
				sd_clk = (sd_clk * 100) / 4;
				break;

			case FJ_MEDIA_SD_CLOCK_50M:
				sd_clk = (sd_clk * 200) / 4;
				break;

			case FJ_MEDIA_SD_CLOCK_100M:
				sd_clk = (sd_clk * 400) / 4;
				break;

			case FJ_MEDIA_SD_CLOCK_208M:
				sd_clk = (sd_clk * 700) / 4;
				break;
			default:
				return FJ_ERR_NG;
		}
		ercd = MWSDIO0_Set_Transmode_Select((INT32)mode, sd_clk);
	}
	else {
		// Driver type
		MWSDIO1_Set_Driving_Force(drive);

		// Speed mode, max clock
		switch (clock) {
			case FJ_MEDIA_SD_CLOCK_25M:
				sd_clk = (sd_clk * 100) / 4;
				break;

			case FJ_MEDIA_SD_CLOCK_50M:
				sd_clk = (sd_clk * 200) / 4;
				break;

			case FJ_MEDIA_SD_CLOCK_100M:
				sd_clk = (sd_clk * 400) / 4;
				break;

			case FJ_MEDIA_SD_CLOCK_208M:
				sd_clk = (sd_clk * 800) / 4;
				break;

			case FJ_MEDIA_SD_CLOCK_UHS2:
				sd_clk = 51851;
				break;

			default:
				return FJ_ERR_NG;
		}
		ercd = MWSDIO1_Set_Transmode_Select((INT32)mode, sd_clk);
	}

	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Set SD callback function
 * @param		UCHAR ch
 * @param		FJ_MEDIA_SD_CB_EVENT type
 * @param		SD_CALLBACK pcallback
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_sd_set_event_cb(UCHAR ch, FJ_MEDIA_SD_CB_EVENT type, SD_CALLBACK pcallback)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;
	UCHAR int_type;

	// parameter check
	if (ch >= D_BF_MEDIA_SD_MAX) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	if (type == FJ_MEDIA_SD_CB_INSERTED) {
		int_type = 0;
	}
	else {
		int_type = 1;
	}

	if (ch == 0) {
		ercd = MWSDIO0_Set_INT_CB(int_type, pcallback);
	}
	else {
		ercd = MWSDIO1_Set_INT_CB(int_type, pcallback);
	}

	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		initialize the SD card
 * @param		UCHAR ch
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_sd_card_init(UCHAR ch)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;
	FW_INT32 media_id;

	// parameter check
	if (ch >= D_BF_MEDIA_SD_MAX) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	if (ch == 0) {
		media_id = D_BF_FS_CONTROLLER_SD_0;
		ercd = BF_Media_Drv_Wrapper_Init_SDIO0((T_FS_FUNCNAME**)&gBF_Fs_Controller_Media_Status[media_id].driver_tbl,
											   (FW_VOID**)&gBF_Fs_Controller_Media_Status[media_id].media_info,
											   (E_BF_MEDIA_DRV_WRAPPER_WP*)&gBF_Fs_Controller_Media_Status[media_id].wp);
	}
	else {
		media_id = D_BF_FS_CONTROLLER_SD_1;
		ercd = BF_Media_Drv_Wrapper_Init_SDIO1((T_FS_FUNCNAME**)&gBF_Fs_Controller_Media_Status[media_id].driver_tbl,
											   (FW_VOID**)&gBF_Fs_Controller_Media_Status[media_id].media_info,
											   (E_BF_MEDIA_DRV_WRAPPER_WP*)&gBF_Fs_Controller_Media_Status[media_id].wp);
	}
	if(ercd != 0){
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		initialize the SD card
 * @param		UCHAR ch
 * @param		INT32* mode
 * @param		ULONG* clock
 * @param		UCHAR* wp_status
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_sd_getcardinfo(UCHAR ch, INT32* mode, ULONG* clock, UCHAR* wp_status)
{
#ifdef CO_LINUX_VERSION_ON
	*mode = 0;
	*clock = 0;
	*wp_status = FJ_MEDIA_NON_WP;
	return FJ_ERR_OK;
#else
	FW_INT32 wp;

	// parameter check
	if (ch >= D_BF_MEDIA_SD_MAX) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	if (ch == 0) {
		if (BF_Fs_If_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD) == E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
			MWSDIO0_Get_Transmode_Select(mode, clock);

			wp = MWSDIO0_CardCheckWP();
			if (wp == 0) {
				*wp_status = FJ_MEDIA_WP;
			}
			else {
				*wp_status = FJ_MEDIA_NON_WP;
			}
		}
		else {
			return FJ_ERR_NG;
		}
	}
	else {
		if (BF_Fs_If_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1) == E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
			MWSDIO1_Get_Transmode_Select(mode, clock);

			wp = MWSDIO1_CardCheckWP();
			if (wp == 0) {
				*wp_status = FJ_MEDIA_WP;
			}
			else {
				*wp_status = FJ_MEDIA_NON_WP;
			}
		}
		else {
			return FJ_ERR_NG;
		}
	}
	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Set SD Hold clock
 * @param		FJ_MEDIA_SD_HOLD_CLOCK *hold
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_sd_set_hold_clock(UCHAR ch, FJ_MEDIA_SD_HOLD_CLOCK hold)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	// parameter check
	if (ch >= D_BF_MEDIA_SD_MAX) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	switch(hold)
	{
		case FJ_MEDIA_SD_HOLD_CLOCK_1_4:
		case FJ_MEDIA_SD_HOLD_CLOCK_2_4:
		case FJ_MEDIA_SD_HOLD_CLOCK_3_4:
			break;
		default:
			return FJ_ERR_NG_INPUT_PARAM;
	}

	if (ch == 0) {
		MWSDIO0_Set_Data_Hold_Clock((UCHAR)hold);
		MWSDIO0_Set_Cmd_Hold_Clock((UCHAR)hold);
	}
	else {
		MWSDIO1_Set_Data_Hold_Clock((UCHAR)hold);
		MWSDIO1_Set_Cmd_Hold_Clock((UCHAR)hold);
	}
	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief		Set the SD Card Lock password
 * @param		CHAR* password
 * @param		ULONG size
 * @return		FJ_ERR_CODE : FJ_ERR_OK
 *                          : FJ_ERR_NG
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_sd_set_cardlockpassword(UCHAR ch, CHAR* password, ULONG size)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	// parameter check
	if (ch >= D_BF_MEDIA_SD_MAX) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	if (ch == 0) {
		ercd = MWSDIO0_Set_Card_Password(password, size);
	}
	else {
		ercd = MWSDIO1_Set_Card_Password(password, size);
	}
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief		Set the SD Card Lock operation
 * @param		FJ_MEDIA_SD_LOCK_OPE operation
 * @return		FJ_ERR_CODE : FJ_ERR_OK
 *                          : FJ_ERR_NG
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_sd_set_cardlock(UCHAR ch, FJ_MEDIA_SD_LOCK_OPE operation)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	// parameter check
	if (ch >= D_BF_MEDIA_SD_MAX) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	if (ch == 0) {
		ercd = MWSDIO0_Set_Card_Lock_Unlock((E_SD0_LOCK_OPERATION)operation);
	}
	else {
		ercd = MWSDIO1_Set_Card_Lock_Unlock((E_SD1_LOCK_OPERATION)operation);
	}
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief		Set the SD Card power supply
 * @param		UCHAR enable
 * @return		FJ_ERR_CODE : FJ_ERR_OK
 *                          : FJ_ERR_NG
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_sd_set_card_power(UCHAR ch, UCHAR enable)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	// parameter check
	if (ch >= D_BF_MEDIA_SD_MAX) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	if (ch == 0) {
		ercd = MWSDIO0_Set_Card_Power(enable);
	}
	else {
		ercd = MWSDIO1_Set_Card_Power(enable);
	}
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief  Set limit of the SD UHS-II mode, speed range and power limit
 * @param		UCHAR ch
 * @param		FJ_MEDIA_SD_UHS2_MODE mode
 * @param		FJ_MEDIA_SD_UHS2_RANGE range
 * @param		FJ_MEDIA_SD_UHS2_PWR power
 * @return		FJ_ERR_CODE : FJ_ERR_OK
 *                       : FJ_ERR_NG
 *                       : FJ_ERR_NG_INPUT_PARAM
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_sd_set_uhs2_modeselect(UCHAR ch, FJ_MEDIA_SD_UHS2_MODE mode, FJ_MEDIA_SD_UHS2_RANGE range, FJ_MEDIA_SD_UHS2_PWR power)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	// [UHS2 PHY errata] An error may occur in 2L-HD
	if (mode != FJ_MEDIA_SD_UHS2_MODE_FD) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	// parameter check
	if (ch == 1) {
		ercd = MWSDIO1_Set_UHS2mode_Select((UINT32)mode, (UINT32)range, (UINT32)power);
	}
	else {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief		Get the SD UHS2 mode, speed range and power limit
 * @param		UCHAR ch
 * @param		UINT32* mode
 * @param		UINT32* range
 * @param		UINT32* power
 * @return		FJ_ERR_CODE : FJ_ERR_OK
 *                          : FJ_ERR_NG
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_sd_get_uhs2_modeselect(UCHAR ch, UINT32* mode, UINT32* range, UINT32* power)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	// parameter check
	if (ch == 1) {
		ercd = MWSDIO1_Get_UHS2mode_Select(mode, range, power);
	}
	else {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief		Set the SD UHS2 power saving mode
 * @param		UCHAR ch
 * @param		FJ_MEDIA_SD_UHS2_PMODE mode
 * @param		UCHAR enable
 * @return		FJ_ERR_CODE : FJ_ERR_OK
 *                          : FJ_ERR_NG
 *                          : FJ_ERR_NG_INPUT_PARAM
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_sd_set_uhs2_powersaving(UCHAR ch, FJ_MEDIA_SD_UHS2_PMODE mode, UCHAR enable)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	// parameter check
	if (ch != 1) {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	switch(mode) {
		case FJ_MEDIA_SD_UHS2_PMODE_DORMANT :
			ercd = MWSDIO1_Uhs2DormantState(enable);
			break;
		case FJ_MEDIA_SD_UHS2_PMODE_HIBERNATE :
			ercd = MWSDIO1_Uhs2HibernateState(enable);
			break;
		default :
			ercd =  FJ_ERR_NG;
			break;
	}

	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}


/**
 * @brief  Set detection time of the SD UHS-II card
 * @param		UCHAR ch
 * @param		UINT32 time
 * @return		FJ_ERR_CODE : FJ_ERR_OK
 *                       : FJ_ERR_NG
 *                       : FJ_ERR_NG_INPUT_PARAM
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_sd_set_uhs2_detecttime(UCHAR ch, UINT32 time)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	// parameter check
	if (ch == 1) {
		MWSDIO1_Set_UHS2_Detect_Time(time);
	}
	else {
		return FJ_ERR_NG_INPUT_PARAM;	// error return
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}


/**
 * @brief 		Initialize the NandFlash macro.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_init(VOID)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	INT32 ercd;

	BF_Fs_User_Custom_Set_NF_Port();
	ercd = MWNF_Init();
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Set the attribute of the NandFlash.
 * @param		FJ_NF_ATTRIBUTE* attr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_config(FJ_NF_ATTRIBUTE* attr)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	T_NF_ATTRIBUTE nf_attr;

	nf_attr.ce_num             = attr->ce_num;
	nf_attr.device_block_num   = attr->device_block_num;
	nf_attr.max_bad_block_num  = attr->max_bad_block_num;
	nf_attr.search_other_page  = attr->bad_block_search_page;
	nf_attr.dbcnt0_t0dc        = attr->drivability0;
	nf_attr.dbcnt0_t1dc        = attr->drivability1;
	nf_attr.dbcnt0_t2dc        = attr->drivability2;

	Media_Custom_NF_Set_Attribute(&nf_attr);

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Initialize the NandFlash device.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_device_init(VOID)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	INT32 ercd;
	UINT32 drive_no;

#ifdef CO_BF_FS_USER_CUSTOM_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(E_BF_MEASURE_TIME_ID_FILE_SYSTEM);
#endif
	ercd = BF_Media_Drv_Wrapper_Init_NF((T_FS_FUNCNAME**)&gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_NF_0].driver_tbl,
										(FW_VOID**)&gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_NF_0].media_info,
										(E_BF_MEDIA_DRV_WRAPPER_WP*)&gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_NF_0].wp);

#ifdef CO_BF_FS_USER_CUSTOM_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(E_BF_MEASURE_TIME_ID_FILE_SYSTEM, "BF_Media_Drv_Wrapper_Init_NF() END.\n");
#endif
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	for (drive_no = D_BF_FS_CONTROLLER_NF_1; drive_no < gNF_Drive_Num; drive_no++) {
		BF_Media_Drv_Wrapper_Get_Drv_Info_NF(drive_no, (T_FS_FUNCNAME**)&gBF_Fs_Controller_Media_Status[drive_no].driver_tbl, (FW_VOID**)&gBF_Fs_Controller_Media_Status[drive_no].media_info);
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Gets the Manufacturer ID and Device ID of the NandFlash.
 * @param		UINT32* mid
 *              UINT32* did
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_get_mid_did(UINT32* mid, UINT32* did)
{
#ifdef CO_LINUX_VERSION_ON
	*mid = 0;
	*did = 0;
	return FJ_ERR_OK;
#else
	MWNF_Get_Get_MID_DID(mid, did);
	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Get NF Device Information
 * @param		ULONG phy_blk
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_get_device_info(FJ_NF_DEV_INFO* nf_dev_info)
{
#ifdef CO_LINUX_VERSION_ON
	memset((void*)nf_dev_info, 0, sizeof(FJ_NF_DEV_INFO));
	return FJ_ERR_OK;
#else
	INT32 ercd;
	T_NF_DEV_INFO dev_info;

	ercd = MWNF_Get_Dev_Info(&dev_info);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	nf_dev_info->phy_blk_num        = dev_info.phy_blk_num;
	nf_dev_info->page_per_blk       = dev_info.page_per_blk;
	nf_dev_info->sector_per_page    = dev_info.sec_per_page;
	nf_dev_info->page_size          = dev_info.page_size;
	nf_dev_info->device_kind        = dev_info.device_kind;
	nf_dev_info->bad_blk_num        = dev_info.bad_blk_num;
	nf_dev_info->physical_free_size = dev_info.physical_free_size;
	nf_dev_info->logical_free_size  = dev_info.logical_free_size;

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		NF All Erase.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_erase(VOID)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	INT32 ercd;

	ercd = MWNF_Erase_All_Phy_Block();
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Write NF Phyblock data.
 * @param		ULONG phy_blk
 * @param		UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_write_physical_data(ULONG phy_blk, UINT32* data_addr)
{
	ULONG temp_skip = 0;

#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	INT32 ercd;
	T_NF_DEV_INFO dev_info;

	ercd = MWNF_Get_Dev_Info(&dev_info);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	if (phy_blk >= gNF_Code_Size) {
		return FJ_ERR_NG;
	}

	if (phy_blk == 0) {
		(VOID)MWNF_Get_Skip_Bytes(&temp_skip);
		(VOID)MWNF_Set_Skip_Bytes(0);
	}
	nf_set_mode(0);

	ercd = MWNF_EraseBlk(phy_blk);
	if (ercd == 0) {
		ercd = MWNF_WritePages_NoRetry(phy_blk, 0, dev_info.page_per_blk, (BYTE*)data_addr);
	}

#ifdef CO_META_ON
	nf_set_mode(1);
#endif
	if (phy_blk == 0) {
		(VOID)MWNF_Set_Skip_Bytes(temp_skip);
	}

	if (ercd != 0) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Read NF Phyblock data.
 * @param		ULONG phy_blk
 * @param		UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_read_physical_data(ULONG phy_blk, UINT32* data_addr)
{
	ULONG temp_skip = 0;

#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	INT32 ercd;
	T_NF_DEV_INFO dev_info;

	ercd = MWNF_Get_Dev_Info(&dev_info);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	if (phy_blk == 0) {
		(VOID)MWNF_Get_Skip_Bytes(&temp_skip);
		(VOID)MWNF_Set_Skip_Bytes(0);
	}
	nf_set_mode(0);

	ercd = MWNF_Direct_BRead((phy_blk * dev_info.page_per_blk * dev_info.sec_per_page), (dev_info.page_per_blk * dev_info.sec_per_page), (char*)data_addr);

#ifdef CO_META_ON
	nf_set_mode(1);
#endif
	if (phy_blk == 0) {
		(VOID)MWNF_Set_Skip_Bytes(temp_skip);
	}

	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Write data to NF Logical sector
 * @param		ULONG  start_sec
 *				ULONG  write_sec_num
 *				UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_write_logical_sector(ULONG start_sec, ULONG write_sec_num, UINT32* data_addr)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;
	ULONG param_blk_top;
	ULONG param_blk_size;

	ercd = MWNF_Get_ParamBlk_Info(&param_blk_top, &param_blk_size);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	if ((start_sec < param_blk_top) || ((start_sec + write_sec_num) > (param_blk_top + param_blk_size))) {
		return FJ_ERR_NG;
	}
	
	ercd = MWNF_BWrite(NULL, start_sec, write_sec_num, (char*)data_addr);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Read data from NF Logical sector
 * @param		ULONG  start_sec
 *				ULONG  read_sec_num
 *				UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NF_NO_ALLOCATE 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_read_logical_sector(ULONG start_sec, ULONG read_sec_num, UINT32* data_addr)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	ercd = MWNF_BRead(NULL, start_sec, read_sec_num, (char*)data_addr);
	if (ercd != 0) {
		if (ercd == D_MWNF_ALLOCATE_ERR) {
			return FJ_ERR_NF_NO_ALLOCATE;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Write data to NF Logical sector
 * @param		ULONG  start_sec
 *				ULONG  write_sec_num
 *				UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_write_logical_sector_pio(ULONG start_sec, ULONG write_sec_num, UINT32* data_addr)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;
	ULONG write_size;
	ULONG param_blk_top;
	ULONG param_blk_size;

	ercd = MWNF_Get_ParamBlk_Info(&param_blk_top, &param_blk_size);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	if ((start_sec < param_blk_top) || ((start_sec + write_sec_num) > (param_blk_top + param_blk_size))) {
		return FJ_ERR_NG;
	}

	write_size = write_sec_num * D_MWNF_SECTOR_SIZE;

	ercd = MWNF_Direct_Write(start_sec, write_size, (ULONG)data_addr);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Partition of NandFlash.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_partition(VOID)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	INT32 ercd;

	ercd = MWNF_Format();
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Read partition information from the logical space of NandFlash.
 * @param		FJ_NF_PARTITION* part_info
 * @param		ULONG* part_num
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_partition_read(FJ_NF_PARTITION* part_info, ULONG* part_num)
{
#ifdef CO_LINUX_VERSION_ON
	*part_num = 0;
	return FJ_ERR_OK;
#else
	INT32 ercd;

	ercd = MWNF_Read_MBR_Info((T_NF_MBR_INFO*)part_info, part_num);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Get the start sector number and size of the selected Partition.
 * @param		E_MEDIA_NF_PARTITION_NAME name
 *				ULONG* start_sec_num
 *				ULONG* data_size
 *				ULONG* total_size
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_get_partition_info(E_MEDIA_NF_PARTITION_NAME name, ULONG* start_sec_num, ULONG* data_size, ULONG* total_size)
{
#ifdef CO_LINUX_VERSION_ON
	*start_sec_num = 0;
	*data_size = 0;
	*total_size = 0;
	return FJ_ERR_OK;
#else
	INT32 ercd;
	INT32 selector;
	T_NF_DEV_INFO dev_info;

	ercd = MWNF_Get_Dev_Info(&dev_info);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	selector = gNF_partition_table[name].selector;

	if (gNF_partition_table[name].area[selector].sector_num != 0) {
		*start_sec_num = gNF_partition_table[name].area[selector].sector_num;
		*data_size     = gNF_partition_table[name].area[selector].data_size;
		*total_size    = gNF_partition_table[name].area_size;
	}
	else {
		// Nothing the partition data
		*data_size     = 0;
		*start_sec_num = 0;
		*total_size    = 0;
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Get Number of BadBlock
 * @param		ULONG limit
 *				UCHAR mode
 *				ULONG* num
 *				UINT32* buf
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_get_badblock_num(ULONG limit, UCHAR mode, ULONG* num, UINT32* buf)
{
#ifdef CO_LINUX_VERSION_ON
	*num = 0;
	*buf = 0;
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	ercd = MWNF_Get_BadBlock_Num(limit, mode, num, (USHORT*)buf);

	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		get parameter block info
 * @param		ULONG* param_blk_top
 *				ULONG* param_blk_size
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_get_paramblk_info(ULONG* param_blk_top, ULONG* param_blk_size)
{
#ifdef CO_LINUX_VERSION_ON
	*param_blk_top = 0;
	*param_blk_size = 0;
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	ercd = MWNF_Get_ParamBlk_Info(param_blk_top, param_blk_size);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		get exception log info
 * @param		ULONG* exception_log_top
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_nf_get_exceptionlog_info(UINT32* exception_log_top)
{
#ifdef CO_LINUX_VERSION_ON
	*exception_log_top = 0;
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	ercd = MWNF_Get_ExceptionLog_Info((ULONG*)exception_log_top);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		set retry count
 * @param		UCHAR retry_cnt_read
 *				UCHAR retry_cnt_write
 * @return 		None
 * @note 		None
 * @attention 	None
 */
VOID fj_nf_set_retry_cnt(UCHAR retry_cnt_read, UCHAR retry_cnt_write)
{
#ifdef CO_LINUX_VERSION_ON
	return;
#else
	MWNF_Set_Retry_Cnt(retry_cnt_read, retry_cnt_write);

	return;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		get retry count
 * @param		UCHAR* retry_cnt_read
 *				UCHAR* retry_cnt_write
 * @return 		None
 * @note 		None
 * @attention 	None
 */
VOID fj_nf_get_retry_cnt(UCHAR* retry_cnt_read, UCHAR* retry_cnt_write)
{
#ifdef CO_LINUX_VERSION_ON
	*retry_cnt_read = 0;
	*retry_cnt_write = 0;
	return;
#else
	MWNF_Get_Retry_Cnt(retry_cnt_read, retry_cnt_write);

	return;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		set timeout
 * @param		FJ_MEDIA_NF_TIMEOUT type
 *				USHORT timeout
 * @return 		None
 * @note 		None
 * @attention 	None
 */
VOID fj_nf_set_timeout(FJ_MEDIA_NF_TIMEOUT type, USHORT timeout)
{
#ifdef CO_LINUX_VERSION_ON
	return;
#else
	MWNF_Set_Timeout((E_MWNF_TIMEOUT)type, timeout);

	return;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		get timeout
 * @param		FJ_MEDIA_NF_TIMEOUT type
 *				USHORT* timeout
 * @return 		None
 * @note 		None
 * @attention 	None
 */
VOID fj_nf_get_timeout(FJ_MEDIA_NF_TIMEOUT type, USHORT* timeout)
{
#ifdef CO_LINUX_VERSION_ON
	*timeout = 0;
	return;
#else
	MWNF_Get_Timeout((E_MWNF_TIMEOUT)type, timeout);

	return;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Initialize the eMMC macro.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_emmc_init(VOID)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	INT32 ercd;

	BF_Fs_User_Custom_Set_EMMC_Port();
	ercd = MWEM_Init();
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Set limit of the eMMC access mode.
 * @param		E_MEDIA_EMMC_ACCESS_MODE mode
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_emmc_config(E_MEDIA_EMMC_ACCESS_MODE mode)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	INT32 ercd;

	ercd = MWEM_Set_Config(mode);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Initialize the eMMC device.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_emmc_device_init(VOID)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	INT32 ercd;
	UINT32 drive_no;

#ifdef CO_BF_FS_USER_CUSTOM_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(E_BF_MEASURE_TIME_ID_FILE_SYSTEM);
#endif
	ercd = BF_Media_Drv_Wrapper_Init_EMMC((T_FS_FUNCNAME**)&gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_EMMC_0].driver_tbl,
										  (FW_VOID**)&gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_EMMC_0].media_info,
										  (E_BF_MEDIA_DRV_WRAPPER_WP*)&gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_EMMC_0].wp);

#ifdef CO_BF_FS_USER_CUSTOM_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(E_BF_MEASURE_TIME_ID_FILE_SYSTEM, "BF_Media_Drv_Wrapper_Init_EMMC() END.\n");
#endif
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	for (drive_no = D_BF_FS_CONTROLLER_EMMC_1; drive_no < (gEM_Drive_Num + D_BF_FS_CONTROLLER_EMMC_0); drive_no++) {
		BF_Media_Drv_Wrapper_Get_Drv_Info_EMMC(drive_no, (T_FS_FUNCNAME**)&gBF_Fs_Controller_Media_Status[drive_no].driver_tbl, (FW_VOID**)&gBF_Fs_Controller_Media_Status[drive_no].media_info);
	}

	// Access Partition : user data area
	ercd = MWEM_Set_AccessPartition(EM_ACCCESS_BOOT_NONE);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Gets the eMMC device information(access mode).
 * @param		INT32* mode
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_emmc_get_device_info(UINT32* mode)
{
#ifdef CO_LINUX_VERSION_ON
	*mode = 0;
	return FJ_ERR_OK;
#else
	INT32 ercd;

	ercd = MWEM_Get_Config(mode);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Write data to eMMC device.
 * @param		ULONG  start_sec
 *				ULONG  write_sec_num
 *				UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_emmc_write_sector(ULONG start_sec, ULONG write_sec_num, UINT32* data_addr)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	ercd = MWEM_DirectWrite(start_sec, write_sec_num, (char*)data_addr);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Read data from eMMC device.
 * @param		ULONG  start_sec
 *				ULONG  read_sec_num
 *				UINT32* data_addr
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 *                          : FJ_ERR_NF_NO_ALLOCATE 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_emmc_read_sector(ULONG start_sec, ULONG read_sec_num, UINT32* data_addr)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	ercd = MWEM_DirectRead(start_sec, read_sec_num, (char*)data_addr);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Partition of eMMC.
 * @param		None
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_emmc_partition(VOID)
{
#ifdef CO_LINUX_VERSION_ON
	return FJ_ERR_OK;
#else
	INT32 ercd;

	ercd = BF_Media_Drv_Wrapper_Format_EMMC((T_FS_FUNCNAME**)&gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_EMMC_0].driver_tbl,
										   (FW_VOID**)&gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_EMMC_0].media_info);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Read partition information from the storage area of eMMC.
 * @param		FJ_EMMC_PARTITION* part_info
 * @param		ULONG* part_num
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_emmc_partition_read(FJ_EMMC_PARTITION* part_info, ULONG* part_num)
{
#ifdef CO_LINUX_VERSION_ON
	*part_num = 0;
	return FJ_ERR_OK;
#else
	INT32 ercd;

	ercd = MWEM_Read_MBR_Info((T_EM_MBR_INFO*)part_info, part_num);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		Get the start sector number and size of the selected Partition of eMMC device.
 * @param		E_MEDIA_NF_PARTITION_NAME name
 *				ULONG* start_sec_num
 *				ULONG* data_size
 *				ULONG* total_size
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_emmc_get_partition_info(E_MEDIA_EMMC_PARTITION_NAME name, ULONG* start_sec_num, ULONG* data_size, ULONG* total_size)
{
#ifdef CO_LINUX_VERSION_ON
	*start_sec_num = 0;
	*data_size = 0;
	*total_size = 0;
	return FJ_ERR_OK;
#else
	INT32 selector;

	selector = gNF_partition_table[name].selector;

	if (gNF_partition_table[name].area[selector].sector_num != 0) {
		*start_sec_num = gNF_partition_table[name].area[selector].sector_num;
		*data_size     = gNF_partition_table[name].area[selector].data_size;
		*total_size    = gNF_partition_table[name].area_size;
	}
	else {
		// Nothing the partition data
		*data_size     = 0;
		*start_sec_num = 0;
		*total_size    = 0;
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		get parameter block info
 * @param		ULONG* param_blk_top
 *				ULONG* param_blk_size
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_emmc_get_paramblk_info(ULONG* param_blk_top, ULONG* param_blk_size)
{
#ifdef CO_LINUX_VERSION_ON
	*param_blk_top = 0;
	*param_blk_size = 0;
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	ercd = MWEM_Get_ParamBlk_Info(param_blk_top, param_blk_size);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief 		get exception log info
 * @param		ULONG* exception_log_top
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_emmc_get_exceptionlog_info(UINT32* exception_log_top)
{
#ifdef CO_LINUX_VERSION_ON
	*exception_log_top = 0;
	return FJ_ERR_OK;
#else
	FW_INT32 ercd;

	ercd = MWEM_Get_ExceptionLog_Info((ULONG*)exception_log_top);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
#endif // CO_LINUX_VERSION_ON
}

