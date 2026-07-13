/**
 * @file		fj_emergency_log.c
 * @brief		Emergency Log Save API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_emergency_log.h"
#include "emergency_log.h"
#include "mode.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
Set the number of log write start sector to external storage.
@param[in] sector start sector number
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_EmergencyLog_Set_Log_Start_Sector(ULONG sector)
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = fj_emergencylog_set_log_start_sector(sector);

	return err_code;
}


/**
Stores the specified memory data to external storage.
@param[in] startAddr log data address
@param[in] headerInfo log header information<br>
						31-16bit : Log label ID<br>
						15-0 bit : Log data size<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_EmergencyLog_Store_Memory(UINT32 startAddr, INT32 headerInfo)
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = fj_emergencylog_store_memory(startAddr, headerInfo);

	return err_code;
}


/**
Saves the log data stored at external storage to specified file on the media(SD).
@param[in] filename file name
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_EmergencyLog_Save(UCHAR* filename)
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = fj_emergencylog_save(filename);

	return err_code;
}

/* File End */
