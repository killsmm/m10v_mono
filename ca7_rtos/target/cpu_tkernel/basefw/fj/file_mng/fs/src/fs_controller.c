/**
 * @file		fs_controller.c
 * @brief		FileSystem scenario control without depend on DCF.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include <string.h>
#include "ddim_typedef.h"
#include "mdf_wrapper.h"
#include "fs_controller.h"
#include "fsdcf_errno.h"
#include "fs_user_custom.h"
#include "../../include/media_attr.h"
#include "NF_driver_wrapper.h"
#include "fs_user_func.h"
#ifdef CO_DCF_EXIF_ON
#include "dcf_controller.h"
#include "dcf_if.h"
#endif // CO_DCF_EXIF_ON
#include "debug.h"
#include "mwsdio.h"
#include "vfs.h"
#include "sdram_map_share.h"
#include "em_if_ext.h"
#include "media_drv_wrapper.h"

/////////////////////////////////////////////////////
// romarea_boot
/////////////////////////////////////////////////////

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_BF_FS_CONTROLLER_SECTOR_MAX	65535

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
volatile T_FS_CONTROLLER_MEDIA_STATUS gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_MEDIA_ID_NUM];
static volatile FW_INT32 g_bf_fs_controller_ret_err;
static volatile FW_ULONG g_bf_fs_controller_ret_length;
volatile long gBF_Fs_Controller_Progress;			/* format progress ratio (%) */

extern volatile FW_ULONG formatSectors;
extern volatile FW_ULONG formatted;
volatile UINT32 gBF_Controller_media_id;
volatile CHAR gBF_Controller_drv_name[D_BF_FS_CONTROLLER_MEDIA_ID_NUM][D_BF_FS_IF_DRVNAME_MAX];

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
static FW_INT32 fs_controller_init_sd0(FW_VOID);
static FW_INT32 fs_controller_format_sd0(FW_VOID);
static FW_INT32 fs_controller_init_sd1(FW_VOID);
static FW_INT32 fs_controller_format_sd1(FW_VOID);
static FW_INT32 fs_controller_init_nf(FW_INT32 media_id);
static FW_INT32 fs_controller_format_nf(FW_INT32 media_id);


///////////////////////////////////////////////////
// static function (romarea_boot)
///////////////////////////////////////////////////

/**
 * @brief  filesystem initialize for SD ch0.
 * @param FW_VOID
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 fs_controller_init_sd0(FW_VOID)
{
	FW_INT32 ercd;			// error code
	FW_INT32 retry_cnt;		// init retry count
	FW_ULONG wrt_protect;	// write protection mode
	FW_CHAR  str_len;		// lengtht for strings

	/* check media insert */
	if( BF_Media_State_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD) != E_BF_MEDIA_STATE_MEDIA_STATE_INSERT ){
		return FSDCF_ERRNO_NOT_INSERT_MEDIA;
	}
	/* set media status structure */
	gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_SD;
	str_len = strlen((const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0]);
	strncpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid, (const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0], str_len);
	strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].drvname, (const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0]);
	strcat((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].drvname, ":\\");
	gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].rwmax = D_BF_FS_CONTROLLER_SECTOR_MAX;

	/* filesystem initialize */
	for( retry_cnt = 0; retry_cnt < D_BF_FS_USER_CUSTOM_FS_INIT_RETRY_CNT; retry_cnt++ ){
		ercd = BF_Mdf_Wrapper_Init_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].driver_tbl, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].media_info);
		if( ercd == 0 ) {
			break;
		}
		else if( ercd == D_BF_MDF_WRAPPER_ERR_DEVENT ){
			BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid);
			continue;
		}
		else{
			; // DO NOTHING
		}
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_init() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid));
#endif
		BF_Fs_User_Custom_Tslp_Tsk(100);
	}
	if( ercd != 0 ) {
		return ercd;
	}

	/* set max read/write sector cnt */
	ercd = BF_Mdf_Wrapper_Set_Con_Sec_Max((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].rwmax);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Con_Sec_Max() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid));
#endif
	}

	/* set write protection mode */
	if( gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].wp == E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT ){
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT;
		BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_SD, E_BF_MEDIA_STATE_MEDIA_WP_PROTECT);
	}
	else {
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;
		BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_SD, E_BF_MEDIA_STATE_MEDIA_WP_UNPROTECT);
	}
	ercd = BF_Mdf_Wrapper_Set_Protection_Mode((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid, wrt_protect);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FS_setProtectionMode() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid));
#endif
	}

	return ercd;
}

/**
 * @brief  filesystem initialize for SD ch1.
 * @param FW_VOID
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 fs_controller_init_sd1(FW_VOID)
{
	FW_INT32 ercd;			// error code
	FW_INT32 retry_cnt;		// init retry count
	FW_ULONG wrt_protect;	// write protection mode
	FW_CHAR  str_len;		// lengtht for strings

	/* check media insert */
	if( BF_Media_State_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1) != E_BF_MEDIA_STATE_MEDIA_STATE_INSERT ){
		return FSDCF_ERRNO_NOT_INSERT_MEDIA;
	}
	/* set media status structure */
	gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_SD1;
	str_len = strlen((const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1]);
	strncpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid, (const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1], str_len);
	strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].drvname, (const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1]);
	strcat((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].drvname, ":\\");
	gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].rwmax = D_BF_FS_CONTROLLER_SECTOR_MAX;

	/* filesystem initialize */
	for( retry_cnt = 0; retry_cnt < D_BF_FS_USER_CUSTOM_FS_INIT_RETRY_CNT; retry_cnt++ ){
		ercd = BF_Mdf_Wrapper_Init_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].driver_tbl, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].media_info);
		if( ercd == 0 ) {
			break;
		}
		else if( ercd == D_BF_MDF_WRAPPER_ERR_DEVENT ){
			BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid);
			continue;
		}
		else{
			; // DO NOTHING
		}
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_init() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid));
#endif
		BF_Fs_User_Custom_Tslp_Tsk(100);
	}
	if( ercd != 0 ) {
		return ercd;
	}

	/* set max read/write sector cnt */
	ercd = BF_Mdf_Wrapper_Set_Con_Sec_Max((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].rwmax);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Con_Sec_Max() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid));
#endif
	}

	/* set write protection mode */
	if( gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].wp == E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT ){
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT;
		BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1, E_BF_MEDIA_STATE_MEDIA_WP_PROTECT);
	}
	else {
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;
		BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1, E_BF_MEDIA_STATE_MEDIA_WP_UNPROTECT);
	}
	ercd = BF_Mdf_Wrapper_Set_Protection_Mode((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid, wrt_protect);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FS_setProtectionMode() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid));
#endif
	}

	return ercd;
}

/**
 * @brief  filesystem initialize for NF.
 * @param FW_VOID
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 fs_controller_init_nf(FW_INT32 media_id)
{
	FW_INT32 ercd = -1;			// error code
	FW_INT32 retry_cnt;		// init retry count
	FW_ULONG wrt_protect;	// write protection mode
	FW_CHAR  str_len;		// lengtht for strings

	/* check media insert */
	if( BF_Media_State_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_NF) != E_BF_MEDIA_STATE_MEDIA_STATE_INSERT ){
		return FSDCF_ERRNO_NOT_INSERT_MEDIA;
	}

	/* set media status structure */
	gBF_Fs_Controller_Media_Status[media_id].media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_NF;
	str_len = strlen((const FW_CHAR*)gBF_Controller_drv_name[media_id]);
	strncpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, (const FW_CHAR*)gBF_Controller_drv_name[media_id], str_len);
	strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].drvname, (const FW_CHAR*)gBF_Controller_drv_name[media_id]);
	strcat((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].drvname, ":\\");
	gBF_Fs_Controller_Media_Status[media_id].rwmax = D_BF_FS_CONTROLLER_SECTOR_MAX;

	/* filesystem initialize */
	for( retry_cnt = 0; retry_cnt < D_BF_FS_USER_CUSTOM_FS_INIT_RETRY_CNT; retry_cnt++ ){
		ercd = BF_Mdf_Wrapper_Init_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, gBF_Fs_Controller_Media_Status[media_id].driver_tbl, gBF_Fs_Controller_Media_Status[media_id].media_info);
		if( ercd == 0 ) {
			BF_Fs_User_Custom_Info_Printer(("fs_controller_init_nf(): Init_Drv(DivName = %s) OK. \n\r", gBF_Fs_Controller_Media_Status[media_id].drvname));
			break;
		}
		else if( ercd == D_BF_MDF_WRAPPER_ERR_DEVENT ){
			BF_Mdf_Wrapper_End_Task();
			BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid);
			continue;
		}
		else if (ercd == D_BF_MDF_WRAPPER_ERR_NOMDF) {
			NF_InitDataDrive();
			continue;
		}
		else{
			; // DO NOTHING
		}
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_init() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
		BF_Fs_User_Custom_Tslp_Tsk(10);
	}
	if( ercd != 0 ) {
		return ercd;
	}

	/* set max read/write sector cnt */
	ercd = BF_Mdf_Wrapper_Set_Con_Sec_Max((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, gBF_Fs_Controller_Media_Status[media_id].rwmax);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Con_Sec_Max() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
	}

	/* set write protection mode */
	if( gBF_Fs_Controller_Media_Status[media_id].wp == E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT ){
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT;
		BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_NF, E_BF_MEDIA_STATE_MEDIA_WP_PROTECT);
	}
	else {
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;
		BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_NF, E_BF_MEDIA_STATE_MEDIA_WP_UNPROTECT);
	}
	ercd = BF_Mdf_Wrapper_Set_Protection_Mode((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, wrt_protect);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FS_setProtectionMode() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
	}

	return ercd;
}

/**
 * @brief  filesystem initialize for eMMC.
 * @param FW_INT32 media_id
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 fs_controller_init_emmc(FW_INT32 media_id)
{
	FW_INT32 ercd = -1;		// error code
	FW_INT32 retry_cnt;		// init retry count
	FW_ULONG wrt_protect;	// write protection mode
	FW_CHAR  str_len;		// lengtht for strings

	/* check media insert */
	if( BF_Media_State_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC) != E_BF_MEDIA_STATE_MEDIA_STATE_INSERT ){
		return FSDCF_ERRNO_NOT_INSERT_MEDIA;
	}

	/* set media status structure */
	gBF_Fs_Controller_Media_Status[media_id].media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC;
	str_len = strlen((const FW_CHAR*)gBF_Controller_drv_name[media_id]);
	strncpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, (const FW_CHAR*)gBF_Controller_drv_name[media_id], str_len);
	strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].drvname, (const FW_CHAR*)gBF_Controller_drv_name[media_id]);
	strcat((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].drvname, ":\\");
	gBF_Fs_Controller_Media_Status[media_id].rwmax = D_BF_FS_CONTROLLER_SECTOR_MAX;

	/* filesystem initialize */
	for( retry_cnt = 0; retry_cnt < D_BF_FS_USER_CUSTOM_FS_INIT_RETRY_CNT; retry_cnt++ ){
		ercd = BF_Mdf_Wrapper_Init_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, gBF_Fs_Controller_Media_Status[media_id].driver_tbl, gBF_Fs_Controller_Media_Status[media_id].media_info);
		if( ercd == 0 ) {
			BF_Fs_User_Custom_Info_Printer(("fs_controller_init_emmc(): Init_Drv(DivName = %s) OK. \n\r", gBF_Fs_Controller_Media_Status[media_id].drvname));
			break;
		}
		else if( ercd == D_BF_MDF_WRAPPER_ERR_DEVENT ){
			BF_Mdf_Wrapper_End_Task();
			BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid);
			continue;
		}
		else{
			; // DO NOTHING
		}
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_init() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
		BF_Fs_User_Custom_Tslp_Tsk(10);
	}
	if( ercd != 0 ) {
		return ercd;
	}

	/* set max read/write sector cnt */
	ercd = BF_Mdf_Wrapper_Set_Con_Sec_Max((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, gBF_Fs_Controller_Media_Status[media_id].rwmax);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Con_Sec_Max() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
	}

	/* set write protection mode */
	if( gBF_Fs_Controller_Media_Status[media_id].wp == E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT ){
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT;
		BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC, E_BF_MEDIA_STATE_MEDIA_WP_PROTECT);
	}
	else {
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;
		BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC, E_BF_MEDIA_STATE_MEDIA_WP_UNPROTECT);
	}
	ercd = BF_Mdf_Wrapper_Set_Protection_Mode((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, wrt_protect);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FS_setProtectionMode() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
	}

	return ercd;
}

/**
 * @brief SD ch0 format.
 * @param FW_VOID
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 fs_controller_format_sd0(FW_VOID)
{
	FW_INT32 ercd;
	FW_INT32 retry_cnt;
	FW_ULONG wrt_protect;	// write protection mode
	FW_CHAR  str_len;		// lengtht for strings
	E_BF_MEDIA_STATE_MEDIA_WP media_w_wp;
	FW_CHAR  temp_drv = 0;

	if(BF_Media_State_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD) == E_BF_MEDIA_STATE_MEDIA_STATE_LEAVE){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("SD0 NOT INSERT\n"));
#endif
		return FSDCF_ERRNO_NOT_INSERT_MEDIA;
	}

	ercd = BF_Mdf_Wrapper_End_Task();
	if ((ercd != 0) && (ercd != D_BF_MDF_WRAPPER_ERR_NOMDF)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_taskend() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	if(*gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0] != 0) {
		str_len = strlen((const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0]);
		strncpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid, (const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0], str_len);
		ercd = BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid);
		if ((ercd != 0) && (ercd != D_BF_MDF_WRAPPER_ERR_NODEV)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("MDF_W_end() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid));
#endif
			return ercd;
		}
	}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	if(*gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0] == 0) {
		// Temporary drive
		strcpy((FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0], "Z");
		str_len = strlen((const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0]);
		strncpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid, (const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0], str_len);
		temp_drv = 1;
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

#ifdef CO_LINUX_VERSION_ON
	ercd = BF_Vfs_Nformat(D_BF_FS_CONTROLLER_SD_0);
#else
	ercd = BF_Media_Drv_Wrapper_Format_SDIO0((T_FS_FUNCNAME**)&gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].driver_tbl, (FW_VOID**)&gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].media_info);
#endif
	if(ercd < 0){
		BF_Fs_User_Custom_Err_Printer(("Format failed: ercd=%d\n", ercd));
		if(temp_drv == 1) {
			memset((CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0], '\0', sizeof(gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0]));
		}
		return FSDCF_ERRNO_DRIVE_FORMAT_FAILED;
	}

	if(*gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0] == 0) {
		return ercd;
	}

	gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_SD;
	strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].drvname, (const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0]);
	strcat((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].drvname, ":\\");
	gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].rwmax = D_BF_FS_CONTROLLER_SECTOR_MAX;

	/* filesystem initialize */
	for( retry_cnt = 0; retry_cnt < D_BF_FS_USER_CUSTOM_FS_INIT_RETRY_CNT; retry_cnt++ ){
		ercd = BF_Mdf_Wrapper_Init_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].driver_tbl, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].media_info);
		if( ercd == 0 ) {
			break;
		}
		else if( ercd == D_BF_MDF_WRAPPER_ERR_DEVENT ){
			BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid);
			continue;
		}
		else{
			; // DO NOTHING
		}
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_init() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid));
#endif
		BF_Fs_User_Custom_Tslp_Tsk(100);
	}
	if( ercd != 0 ) {
		if(temp_drv == 1) {
			memset((CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0], '\0', sizeof(gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0]));
		}
		return ercd;
	}

#if 1
	gBF_Fs_Controller_Progress = 70;
#endif

	/* set max read/write sector cnt */
	ercd = BF_Mdf_Wrapper_Set_Con_Sec_Max((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].rwmax);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Con_Sec_Max() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid));
#endif
	}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
    /* Qformat */
	ercd = BF_Mdf_Wrapper_Qformat((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Qformat() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid));
#endif
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

	/* set write protection mode */
	media_w_wp = BF_Media_State_Get_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_SD);
	if( media_w_wp == E_BF_MEDIA_STATE_MEDIA_WP_PROTECT ){
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT;
	}
	else {
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;
	}
	ercd = BF_Mdf_Wrapper_Set_Protection_Mode((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid, wrt_protect);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FS_setProtectionMode() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid));
#endif
	}

	if(temp_drv == 1) {
		BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_0].devid);
		memset((CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0], '\0', sizeof(gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_0]));
	}
	return ercd;
}

/**
 * @brief SD ch1 format.
 * @param FW_VOID
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 fs_controller_format_sd1(FW_VOID)
{
	FW_INT32 ercd;
	FW_INT32 retry_cnt;
	FW_ULONG wrt_protect;	// write protection mode
	FW_CHAR  str_len;		// lengtht for strings
	E_BF_MEDIA_STATE_MEDIA_WP media_w_wp;
	FW_CHAR  temp_drv = 0;

	if(BF_Media_State_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1) == E_BF_MEDIA_STATE_MEDIA_STATE_LEAVE){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("SD1 NOT INSERT\n"));
#endif
		return FSDCF_ERRNO_NOT_INSERT_MEDIA;
	}

	ercd = BF_Mdf_Wrapper_End_Task();
	if ((ercd != 0) && (ercd != D_BF_MDF_WRAPPER_ERR_NOMDF)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_taskend() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	if(*gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1] != 0) {
		str_len = strlen((const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1]);
		strncpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid, (const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1], str_len);
		ercd = BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid);
		if ((ercd != 0) && (ercd != D_BF_MDF_WRAPPER_ERR_NODEV)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("MDF_W_end() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid));
#endif
			return ercd;
		}
	}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	if(*gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1] == 0) {
		// Temporary drive
		strcpy((FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1], "Z");
		str_len = strlen((const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1]);
		strncpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid, (const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1], str_len);
		temp_drv = 1;
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

#ifdef CO_LINUX_VERSION_ON
	ercd = BF_Vfs_Nformat(D_BF_FS_CONTROLLER_SD_1);
#else
	ercd = BF_Media_Drv_Wrapper_Format_SDIO1((T_FS_FUNCNAME**)&gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].driver_tbl, (FW_VOID**)&gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].media_info);
#endif
	if(ercd < 0){
		BF_Fs_User_Custom_Err_Printer(("Format failed: ercd=%d\n", ercd));
		if(temp_drv == 1) {
			memset((CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1], '\0', sizeof(gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1]));
		}
		return FSDCF_ERRNO_DRIVE_FORMAT_FAILED;
	}

	if(*gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1] == 0) {
		return ercd;
	}

	gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_SD1;
	strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].drvname, (const FW_CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1]);
	strcat((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].drvname, ":\\");
	gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].rwmax = D_BF_FS_CONTROLLER_SECTOR_MAX;

	/* filesystem initialize */
	for( retry_cnt = 0; retry_cnt < D_BF_FS_USER_CUSTOM_FS_INIT_RETRY_CNT; retry_cnt++ ){
		ercd = BF_Mdf_Wrapper_Init_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].driver_tbl, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].media_info);
		if( ercd == 0 ) {
			break;
		}
		else if( ercd == D_BF_MDF_WRAPPER_ERR_DEVENT ){
			BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid);
			continue;
		}
		else{
			; // DO NOTHING
		}
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_init() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid));
#endif
		BF_Fs_User_Custom_Tslp_Tsk(100);
	}
	if( ercd != 0 ) {
		if(temp_drv == 1) {
			memset((CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1], '\0', sizeof(gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1]));
		}
		return ercd;
	}

#if 1
	gBF_Fs_Controller_Progress = 70;
#endif

	/* set max read/write sector cnt */
	ercd = BF_Mdf_Wrapper_Set_Con_Sec_Max((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].rwmax);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Con_Sec_Max() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid));
#endif
	}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
    /* Qformat */
	ercd = BF_Mdf_Wrapper_Qformat((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Qformat() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid));
#endif
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

	/* set write protection mode */
	media_w_wp = BF_Media_State_Get_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1);
	if( media_w_wp == E_BF_MEDIA_STATE_MEDIA_WP_PROTECT ){
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT;
	}
	else {
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;
	}
	ercd = BF_Mdf_Wrapper_Set_Protection_Mode((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid, wrt_protect);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FS_setProtectionMode() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid));
#endif
	}

	if(temp_drv == 1) {
		BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_SD_1].devid);
		memset((CHAR*)gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1], '\0', sizeof(gBF_Controller_drv_name[D_BF_FS_CONTROLLER_SD_1]));
	}

	return ercd;
}

/**
 * @brief NF format.
 * @param FW_VOID
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 fs_controller_format_nf(FW_INT32 media_id)
{
	FW_INT32 ercd = 0;
	FW_INT32 retry_cnt;
	FW_ULONG wrt_protect;	// write protection mode
	FW_CHAR  str_len;		// lengtht for strings
	E_BF_MEDIA_STATE_MEDIA_WP media_w_wp;
	FW_CHAR  temp_drv = 0;

	if(BF_Media_State_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_NF) == E_BF_MEDIA_STATE_MEDIA_STATE_LEAVE){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("NF NOT INSERT\n"));
#endif
		return FSDCF_ERRNO_NOT_INSERT_MEDIA;
	}

	ercd = BF_Mdf_Wrapper_End_Task();
	if ((ercd != 0) && (ercd != D_BF_MDF_WRAPPER_ERR_NOMDF)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_taskend() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	if(*gBF_Controller_drv_name[media_id] != 0) {
		str_len = strlen((const FW_CHAR*)gBF_Controller_drv_name[media_id]);
		strncpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, (const FW_CHAR*)gBF_Controller_drv_name[media_id], str_len);
		ercd = BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid);
		if ((ercd != 0) && (ercd != D_BF_MDF_WRAPPER_ERR_NODEV) && (ercd != D_BF_MDF_WRAPPER_ERR_NOMDF)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("MDF_W_end() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
			return ercd;
		}
	}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	if(*gBF_Controller_drv_name[media_id] == 0) {
		// Temporary drive
		strcpy((FW_CHAR*)gBF_Controller_drv_name[media_id], "Z");
		str_len = strlen((const FW_CHAR*)gBF_Controller_drv_name[media_id]);
		strncpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, (const FW_CHAR*)gBF_Controller_drv_name[media_id], str_len);
		temp_drv = 1;
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

#ifdef CO_LINUX_VERSION_ON
	ercd = BF_Vfs_Nformat(media_id);
#else
	ercd = MWNF_Format_Drive((E_MWNF_DRIVE)media_id);
#endif
	if (ercd != 0) {
		BF_Fs_User_Custom_Err_Printer(("Format failed: ercd=%d\n", ercd));
		if(temp_drv == 1) {
			memset((CHAR*)gBF_Controller_drv_name[media_id], '\0', sizeof(gBF_Controller_drv_name[media_id]));
		}
		return FSDCF_ERRNO_DRIVE_FORMAT_FAILED;
	}

	if(*gBF_Controller_drv_name[media_id] == 0) {
		return ercd;
	}

	gBF_Fs_Controller_Media_Status[media_id].media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_NF;
	strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].drvname, (const FW_CHAR*)gBF_Controller_drv_name[media_id]);
	strcat((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].drvname, ":\\");
	gBF_Fs_Controller_Media_Status[media_id].rwmax = D_BF_FS_CONTROLLER_SECTOR_MAX;

	/* filesystem initialize */
	for( retry_cnt = 0; retry_cnt < D_BF_FS_USER_CUSTOM_FS_INIT_RETRY_CNT; retry_cnt++ ){
		ercd = BF_Mdf_Wrapper_Init_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, gBF_Fs_Controller_Media_Status[media_id].driver_tbl, gBF_Fs_Controller_Media_Status[media_id].media_info);
		if( ercd == 0 ) {
			break;
		}
		else if( ercd == D_BF_MDF_WRAPPER_ERR_DEVENT ){
			BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid);
			continue;
		}
		else if (ercd == D_BF_MDF_WRAPPER_ERR_NOMDF) {
			NF_InitDataDrive();
			continue;
		}
		else{
			; // DO NOTHING
		}
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_init() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
		BF_Fs_User_Custom_Tslp_Tsk(100);
	}
	if( ercd != 0 ) {
		if(temp_drv == 1) {
			memset((CHAR*)gBF_Controller_drv_name[media_id], '\0', sizeof(gBF_Controller_drv_name[media_id]));
		}
		return ercd;
	}

	/* set max read/write sector cnt */
	ercd = BF_Mdf_Wrapper_Set_Con_Sec_Max((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, gBF_Fs_Controller_Media_Status[media_id].rwmax);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Con_Sec_Max() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
	}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
    /* Qformat */
	ercd = BF_Mdf_Wrapper_Qformat((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Qformat() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

	/* set write protection mode */
	media_w_wp = BF_Media_State_Get_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_NF);
	if( media_w_wp == E_BF_MEDIA_STATE_MEDIA_WP_PROTECT ){
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT;
	}
	else {
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;
	}
	ercd = BF_Mdf_Wrapper_Set_Protection_Mode((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, wrt_protect);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FS_setProtectionMode() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
	}

	if(temp_drv == 1) {
		BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid);
		memset((CHAR*)gBF_Controller_drv_name[media_id], '\0', sizeof(gBF_Controller_drv_name[media_id]));
	}
	return ercd;
}

/**
 * @brief eMMC format.
 * @param FW_VOID
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 fs_controller_format_emmc(FW_INT32 media_id)
{
	FW_INT32 ercd = 0;
	FW_INT32 retry_cnt;
	FW_ULONG wrt_protect;	// write protection mode
	FW_CHAR  str_len;		// lengtht for strings
	E_BF_MEDIA_STATE_MEDIA_WP media_w_wp;
	E_MWEM_DRIVE drive_no;
	FW_CHAR  temp_drv = 0;

	if(BF_Media_State_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC) == E_BF_MEDIA_STATE_MEDIA_STATE_LEAVE){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("EMMC NOT INSERT\n"));
#endif
		return FSDCF_ERRNO_NOT_INSERT_MEDIA;
	}

	ercd = BF_Mdf_Wrapper_End_Task();
	if ((ercd != 0) && (ercd != D_BF_MDF_WRAPPER_ERR_NOMDF)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_taskend() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	if(*gBF_Controller_drv_name[media_id] != 0) {
		str_len = strlen((const FW_CHAR*)gBF_Controller_drv_name[media_id]);
		strncpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, (const FW_CHAR*)gBF_Controller_drv_name[media_id], str_len);
		ercd = BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid);
		if ((ercd != 0) && (ercd != D_BF_MDF_WRAPPER_ERR_NODEV) && (ercd != D_BF_MDF_WRAPPER_ERR_NOMDF)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("MDF_W_end() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
			return ercd;
		}
	}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	if(*gBF_Controller_drv_name[media_id] == 0) {
		// Temporary drive
		strcpy((FW_CHAR*)gBF_Controller_drv_name[media_id], "Z");
		str_len = strlen((const FW_CHAR*)gBF_Controller_drv_name[media_id]);
		strncpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, (const FW_CHAR*)gBF_Controller_drv_name[media_id], str_len);
		temp_drv = 1;
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

	drive_no = media_id - D_BF_FS_CONTROLLER_EMMC_0;

#ifdef CO_LINUX_VERSION_ON
	ercd = BF_Vfs_Nformat(drive_no);
#else
	ercd = MWEM_Format_Drive((E_MWEM_DRIVE)drive_no);
#endif
	if (ercd != 0) {
		BF_Fs_User_Custom_Err_Printer(("Format failed: ercd=%d\n", ercd));
		if(temp_drv == 1) {
			memset((CHAR*)gBF_Controller_drv_name[media_id], '\0', sizeof(gBF_Controller_drv_name[media_id]));
		}
		return FSDCF_ERRNO_DRIVE_FORMAT_FAILED;
	}

	if(*gBF_Controller_drv_name[media_id] == 0) {
		return ercd;
	}

	gBF_Fs_Controller_Media_Status[media_id].media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC;
	strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].drvname, (const FW_CHAR*)gBF_Controller_drv_name[media_id]);
	strcat((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].drvname, ":\\");
	gBF_Fs_Controller_Media_Status[media_id].rwmax = D_BF_FS_CONTROLLER_SECTOR_MAX;

	/* filesystem initialize */
	for( retry_cnt = 0; retry_cnt < D_BF_FS_USER_CUSTOM_FS_INIT_RETRY_CNT; retry_cnt++ ){
		ercd = BF_Mdf_Wrapper_Init_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, gBF_Fs_Controller_Media_Status[media_id].driver_tbl, gBF_Fs_Controller_Media_Status[media_id].media_info);
		if( ercd == 0 ) {
			break;
		}
		else if( ercd == D_BF_MDF_WRAPPER_ERR_DEVENT ){
			BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid);
			continue;
		}
		else{
			; // DO NOTHING
		}
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_init() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
		BF_Fs_User_Custom_Tslp_Tsk(100);
	}
	if( ercd != 0 ) {
		if(temp_drv == 1) {
			memset((CHAR*)gBF_Controller_drv_name[media_id], '\0', sizeof(gBF_Controller_drv_name[media_id]));
		}
		return ercd;
	}

	/* set max read/write sector cnt */
	ercd = BF_Mdf_Wrapper_Set_Con_Sec_Max((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, gBF_Fs_Controller_Media_Status[media_id].rwmax);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Con_Sec_Max() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
	}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
    /* Qformat */
	ercd = BF_Mdf_Wrapper_Qformat((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Qformat() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

	/* set write protection mode */
	media_w_wp = BF_Media_State_Get_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC);
	if( media_w_wp == E_BF_MEDIA_STATE_MEDIA_WP_PROTECT ){
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT;
	}
	else {
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;
	}
	ercd = BF_Mdf_Wrapper_Set_Protection_Mode((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, wrt_protect);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FS_setProtectionMode() failed: ercd=%d, devid=%s\n", ercd, gBF_Fs_Controller_Media_Status[media_id].devid));
#endif
	}

	if(temp_drv == 1) {
		BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid);
		memset((CHAR*)gBF_Controller_drv_name[media_id], '\0', sizeof(gBF_Controller_drv_name[media_id]));
	}
	return ercd;
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
VOID BF_Fs_Controller_Get_Media_Id(CHAR* drvname, CHAR* bMediaId);

#if 1	// for DCF-task
FW_INT32      BF_Fs_Controller_End_Task_MDF(FW_VOID)
{
	BF_Mdf_Wrapper_End_Task();
	return 0;
}
#endif

/**
 * @brief filesystem initialize.
 * @param FW_VOID
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Init(FW_VOID)
{
	FW_INT32 ercd;				// error code

	/* initialize MDF */
	ercd = BF_Mdf_Wrapper_Start_Mdf();
	if((ercd != 0) && (ercd != D_BF_MDF_WRAPPER_ERR_MDF)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_MDFStart() failed: ercd=%d\n", ercd));
#endif
	}

	return ercd;
}

/**
 * @brief media format.
 * @param i:media_type media type
 * @return error code
 * @note none
 * @attention none.
 */
//long gBF_Fs_Controller_Progress;			/* format progress ratio (%) */
FW_INT32      BF_Fs_Controller_Format(FW_INT32 media_id)
{
	FW_INT32 ercd ;
	FW_CHAR drv_name[9];
	FW_CHAR cur_drv_name[9];
	
//	BF_Fs_User_Custom_Clr_Flg_Init_End();

	gBF_Fs_Controller_Progress = 0;

	ercd = BF_Fs_Controller_Get_Cur_Drv((FW_CHAR*)cur_drv_name);
	if (ercd != 0) {
		memset((CHAR*)cur_drv_name, '\0', sizeof(cur_drv_name));
	}

#ifdef CO_DCF_EXIF_ON
	BF_Dcf_Controller_Close_Display_File();
#endif // CO_DCF_EXIF_ON

	switch(media_id){
		case D_BF_FS_CONTROLLER_NF_0:
		case D_BF_FS_CONTROLLER_NF_1:
		case D_BF_FS_CONTROLLER_NF_2:
		case D_BF_FS_CONTROLLER_NF_3:
		case D_BF_FS_CONTROLLER_NF_4:
		case D_BF_FS_CONTROLLER_NF_5:
		case D_BF_FS_CONTROLLER_NF_6:
			ercd = fs_controller_format_nf(media_id);
			break;

		case D_BF_FS_CONTROLLER_EMMC_0:
		case D_BF_FS_CONTROLLER_EMMC_1:
		case D_BF_FS_CONTROLLER_EMMC_2:
		case D_BF_FS_CONTROLLER_EMMC_3:
		case D_BF_FS_CONTROLLER_EMMC_4:
		case D_BF_FS_CONTROLLER_EMMC_5:
		case D_BF_FS_CONTROLLER_EMMC_6:
			ercd = fs_controller_format_emmc(media_id);
			break;

		case D_BF_FS_CONTROLLER_SD_0:
			ercd = fs_controller_format_sd0();
			break;

		case D_BF_FS_CONTROLLER_SD_1:
			ercd = fs_controller_format_sd1();
			break;

		default:
#ifdef CO_BF_FS_USER_CUSTOM_FSC_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("BF_Fs_Controller_Format(): NOT SUPPORT MEDIA: media_id=%d\n", media_id));
#endif
			ercd = FSDCF_ERRNO_NOT_SUPPORT_MEDIA;
			break;
	}

	if((*gBF_Controller_drv_name[media_id]) != 0) {
		strcpy(drv_name, (const CHAR*)gBF_Controller_drv_name[media_id]);

		if (ercd == 0) {
			if (BF_Media_Attr_Get_Attr_Volume_Label() == 1) {
				// set Volume label
				ercd = BF_Fs_Controller_Set_VolLabel(drv_name, (FW_CHAR*)BF_Media_Attr_Get_Attr_Sz_Vol_Label(media_id));
				BF_Debug_Print_Information(( "volume label name end(%d) : vol_label(%s) \r\n", ercd, (FW_CHAR*)BF_Media_Attr_Get_Attr_Sz_Vol_Label(media_id)));
			}
		}
	}
	
	if(*cur_drv_name != '\0') {
		strcat(cur_drv_name, ":");
		ercd = BF_Fs_Controller_Chdir(cur_drv_name);
	}

	if (ercd == 0) {
		gBF_Fs_Controller_Progress = 100;
	}
	else {
		gBF_Fs_Controller_Progress = -1;
	}

	BF_Fs_User_Custom_Set_Flg_Init_End();
	return ercd;
}

/**
 * @brief media format.
 * @param i:media_type media type
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_QuickFormat(FW_CHAR* drv_name)
{
	FW_INT32 ercd = 0;
	gBF_Fs_Controller_Progress = 0;
	FW_UCHAR media_id;
	
	BF_Fs_Controller_Get_Media_Id(drv_name, (FW_CHAR*)&media_id);
	if (media_id == 0xFF) {
		return -1;
	}
#ifdef CO_DCF_EXIF_ON
	BF_Dcf_Controller_Close_Display_File();
#endif // CO_DCF_EXIF_ON
	
	/* NF/SD */
	ercd = BF_Mdf_Wrapper_EXT_Get_Qformat_Sectors(drv_name, (FW_ULONG*)&formatSectors );
	if(ercd == 0){
		ercd = BF_Mdf_Wrapper_Qformat(drv_name);
	}
	gBF_Fs_Controller_Progress = 70;
	formatSectors = 0;
	formatted = 0;

	if (BF_Media_Attr_Get_Attr_Volume_Label() == 1) {
		// set Volume label
		ercd = BF_Fs_Controller_Set_VolLabel(drv_name, (FW_CHAR*)BF_Media_Attr_Get_Attr_Sz_Vol_Label(media_id));
		BF_Debug_Print_Information(( "volume label name end(%d) \r\n", ercd));
	}

	if (ercd == 0) {
		gBF_Fs_Controller_Progress = 100;
	}
	else {
		gBF_Fs_Controller_Progress = -1;
	}

	return ercd;
}

/**
 * @brief Get Media format progress.
 * @param FW_LONG* format_progress
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Get_Format_Progress(FW_LONG* format_progress)
{
	if (gBF_Fs_Controller_Progress == -1) {
		/* format error */
		return 1;
	}

	*format_progress = gBF_Fs_Controller_Progress;
	return 0;
}

/**
 * @brief media re-initalize.
 * @param i:media_type media type
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Start_Media(FW_INT32 media_id)
{
	FW_INT32 ercd = 0;
	FW_CHAR  drvname[12];

	switch(media_id){
		case D_BF_FS_CONTROLLER_NF_0:
		case D_BF_FS_CONTROLLER_NF_1:
		case D_BF_FS_CONTROLLER_NF_2:
		case D_BF_FS_CONTROLLER_NF_3:
		case D_BF_FS_CONTROLLER_NF_4:
		case D_BF_FS_CONTROLLER_NF_5:
		case D_BF_FS_CONTROLLER_NF_6:
			ercd = fs_controller_init_nf(media_id);
			break;

		case D_BF_FS_CONTROLLER_EMMC_0:
		case D_BF_FS_CONTROLLER_EMMC_1:
		case D_BF_FS_CONTROLLER_EMMC_2:
		case D_BF_FS_CONTROLLER_EMMC_3:
		case D_BF_FS_CONTROLLER_EMMC_4:
		case D_BF_FS_CONTROLLER_EMMC_5:
		case D_BF_FS_CONTROLLER_EMMC_6:
			ercd = fs_controller_init_emmc(media_id);
			break;

		case D_BF_FS_CONTROLLER_SD_0:
			ercd = fs_controller_init_sd0();
			break;

		case D_BF_FS_CONTROLLER_SD_1:
			ercd = fs_controller_init_sd1();
			break;

		default:
#ifdef CO_BF_FS_USER_CUSTOM_FSC_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("BF_Fs_Controller_Start_Media(): NOT SUPPORT MEDIA: media_id=%d\n", media_id));
#endif
			ercd = FSDCF_ERRNO_NOT_SUPPORT_MEDIA;
			return ercd;
	}
	
	if ( ercd != 0 ){
		BF_Fs_User_Custom_Set_Flg_Init_End();
		BF_Fs_User_Custom_Wrn_Printer(("BF_Fs_Controller_Start_Media(): Init(%d) NG :Err=%d\n", media_id, ercd));
		return ercd;
	}

	if (( D_BF_FS_CONTROLLER_SD_0 == media_id ) || ( D_BF_FS_CONTROLLER_SD_1 == media_id )){
		/* set used media */
		BF_Media_State_Set_Used_Media(gBF_Fs_Controller_Media_Status[media_id].media_type);

		strcpy((FW_CHAR*)drvname, (FW_CHAR*)gBF_Controller_drv_name[media_id]);
		strcat((FW_CHAR*)drvname, ":");
		ercd = BF_Mdf_Wrapper_Chdir((const FW_CHAR*)drvname);
		if ( ercd != 0 ){
			BF_Fs_User_Custom_Wrn_Printer(("BF_Fs_Controller_Start_Media(): FS_Chdir(%s) NG :Err=%d\n", drvname, ercd));
		}
	}
	BF_Fs_User_Custom_Set_Flg_Init_End();
	return ercd;
}

/**
 * @brief media finalize.
 * @param i:media_type media type
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_End_Media(FW_INT32 media_id)
{
	FW_INT32 ercd = 0;
	FW_CHAR  drvname[12];
	FW_CHAR  cur_drv_name[9];
	
	ercd = BF_Fs_Controller_Get_Cur_Drv((FW_CHAR*)cur_drv_name);
	if (ercd != 0) {
		memset((CHAR*)cur_drv_name, '\0', sizeof(cur_drv_name));
	}

	ercd = BF_Mdf_Wrapper_End_Task();
	if((ercd != 0) && (ercd != D_BF_MDF_WRAPPER_ERR_NOMDF)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("MDF_W_taskend() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}
	strcpy((FW_CHAR*)drvname, (FW_CHAR*)gBF_Controller_drv_name[media_id]);
	ercd = BF_Mdf_Wrapper_End_Drv((FW_CHAR*)drvname);
	if(ercd != 0){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_End_Drv() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	BF_Fs_User_Custom_Clr_Flg_Init_End();

	if (strcmp((const CHAR*)cur_drv_name, (const CHAR*)drvname) == 0) {
		/* set used media */
		BF_Media_State_Set_Used_Media(E_BF_MEDIA_STATE_MEDIA_TYPE_NOTHING);
	}
	else {
		/* chdir(current drive) */
		strcat(cur_drv_name, ":");
		ercd = BF_Fs_Controller_Chdir(cur_drv_name);
	}

	if (media_id == D_BF_FS_CONTROLLER_SD_0) {
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
		MWSDIO0_Set_Card_Power(0);
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
		(FW_VOID)BF_Media_Drv_Wrapper_Finalize_SDIO0();
#endif // CO_FS_USED_MDF				// --- REMOVE_MDF END ---
#endif	// for Linux Co-op
	}
	else if (media_id == D_BF_FS_CONTROLLER_SD_1) {
#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
		MWSDIO1_Set_Card_Power(0);
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
		(FW_VOID)BF_Media_Drv_Wrapper_Finalize_SDIO1();
#endif // CO_FS_USED_MDF				// --- REMOVE_MDF END ---
#endif	// for Linux Co-op
	}
	else {
		// DO NOTHING
	}

	return 0;
}

/**
 * @brief Execute acquiring file system information.
 * @param i:*drvname the pointer of array of drive name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Statvfs(FW_CHAR* drvname, T_BF_FS_IF_STATVFS* buf)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Statvfs((const FW_CHAR*)drvname, buf);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Statvfs() failed: ercd=%d, path=%s\n", g_bf_fs_controller_ret_err, drvname));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute setting the volume label of specified drive.
 * @param i:*drvname the pointer of array of drive name
 * @param i:*label the pointer of volume label array
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Set_VolLabel(FW_CHAR* drvname, FW_CHAR* label)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Set_Vol_Label((const FW_CHAR*)drvname, (const FW_CHAR*)label);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
//		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Vol_Label() failed: ercd=%d, drvname=%s, label=%s\n", g_bf_fs_controller_ret_err, drvname, label));
#endif
	}
	else {
		BF_Media_Attr_Set_Attr_Volume_Label(1);
	}
	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute getting the volume label of specified drive.
 * @param i:*drvname the pointer of array of drive name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Get_VolLabel(FW_CHAR* drvname, FW_CHAR* label)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Get_Vol_Label((const FW_CHAR*)drvname, label);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
//		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Vol_Label() failed: ercd=%d, drvname=%s, label=%s\n", g_bf_fs_controller_ret_err, drvname, label));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute acquiring the file information of specified file.
 * @param i:*fname the pointer of array of file name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Stat(FW_CHAR* fname, T_BF_FS_IF_STAT* sbuf)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Stat((const FW_CHAR*)fname, sbuf);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
//		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Stat() failed: ercd=%d, fname=%s\n", g_bf_fs_controller_ret_err, fname));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute acquiring the attribute of specified file.
 * @param i:*fname the pointer of array of file name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Get_Attribute(FW_CHAR* fname, FW_UCHAR* attributes)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Get_Attribute((const FW_CHAR*)fname, attributes);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
//		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Get_Attribute() failed: ercd=%d, fname=%s\n", g_bf_fs_controller_ret_err, fname));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute opening the specified file.
 * @param i:*fname the pointer of array of file name
 * @param i:mode Open mode
 * @param i:amode File attribute
 * @return success: File ID
 *         failuer: 0
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Open(FW_CHAR* fname, FW_USHORT mode, FW_USHORT amode)
{
#ifdef CO_BF_FS_USER_CUSTOM_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(E_BF_MEASURE_TIME_ID_FILE_SYSTEM);
#endif

	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Open((const FW_CHAR*)fname, mode, amode);

#ifdef CO_BF_FS_USER_CUSTOM_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(E_BF_MEASURE_TIME_ID_FILE_SYSTEM, "BF_Mdf_Wrapper_Open() END.\n");
#endif

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
//	BF_Fs_User_Custom_Err_Printer((" DEBUG __ BF_Mdf_Wrapper_Open()  ercd=%d, fname=%s, mode=%d, amode=%d\n", g_bf_fs_controller_ret_err, fname, mode, amode));
	if( g_bf_fs_controller_ret_err < 0 ){
#if 1	// for movie test
		if (g_bf_fs_controller_ret_err == D_BF_MDF_WRAPPER_ERR_EXCLUSIVE) {
#ifdef CO_DCF_EXIF_ON
			BF_Dcf_Controller_Close_Display_File();
#endif // CO_DCF_EXIF_ON
			g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Open((const FW_CHAR*)fname, mode, amode);
			if( g_bf_fs_controller_ret_err < 0 ){
#endif

#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: ercd=%d, fname=%s, mode=%d, amode=%d\n", g_bf_fs_controller_ret_err, fname, mode, amode));
#endif
#if 1	// for movie test
			}
		}
#endif
		g_bf_fs_controller_ret_err = 0;
	}

#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
//	BF_Fs_User_Custom_Err_Printer((" DEBUG __ pf_fOpen()  ercd=%d, fname=%s, mode=%d, amode=%d\n", g_bf_fs_controller_ret_err, fname, mode, amode));
	
	if( g_bf_fs_controller_ret_err == 0 ){
//		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: ercd=%d, fname=%s, mode=%d, amode=%d\n", g_bf_fs_controller_ret_err, fname, mode, amode));
	}

#endif // CO_FS_USED_MDF				// --- REMOVE_MDF END ---

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute closing the specified file number.
 * @param i:fNo the file number of target file
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Close(FW_INT32 fNo)
{
#ifdef CO_BF_FS_USER_CUSTOM_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(E_BF_MEASURE_TIME_ID_FILE_SYSTEM);
#endif

	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Close(fNo);

#ifdef CO_BF_FS_USER_CUSTOM_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(E_BF_MEASURE_TIME_ID_FILE_SYSTEM, "BF_Fs_Controller_Close() END.\n");
#endif

	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Close() failed: ercd=%d, fNo=%x\n", g_bf_fs_controller_ret_err, fNo));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute reading and moving the data from the current file pointer.
 * @param i:fNo the file number of target file
 * @param i:addr the pointer of area to store the read data
 * @param i:count the read size in bytes
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Read(FW_INT32 fNo, FW_ULONG addr, FW_ULONG count, FW_ULONG* length)
{
#ifdef CO_BF_FS_USER_CUSTOM_MEASURE_TIME
//	BF_Fs_User_Custom_Measure_Time_Init(E_BF_MEASURE_TIME_ID_FILE_SYSTEM);
	BF_Fs_User_Custom_Measure_Time_Init(E_BF_MEASURE_TIME_ID_FILE_READ);
#endif

	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Read(fNo, (FW_VOID*)addr, count, length);
#ifdef CO_BF_FS_USER_CUSTOM_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(E_BF_MEASURE_TIME_ID_FILE_READ, "BF_Fs_Controller_Read() END.\n");
#endif

	if((g_bf_fs_controller_ret_err < 0) && (g_bf_fs_controller_ret_err != D_BF_MDF_WRAPPER_ERR_EOF)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, fNo=%x, addr=%lx, count=%lu, length = %lx\n", g_bf_fs_controller_ret_err, fNo, addr, count, *length));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute writing and moving data from the specified area to the current file pointer.
 * @param i:fNo the file number of target file
 * @param i:addr the pointer of area which is stored the write data
 * @param i:count the write data size
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Write(FW_INT32 fNo, FW_ULONG addr, FW_ULONG count, FW_ULONG* length)
{
#ifdef CO_BF_FS_USER_CUSTOM_MEASURE_TIME
//	BF_Fs_User_Custom_Measure_Time_Init(E_BF_MEASURE_TIME_ID_FILE_SYSTEM);
	BF_Fs_User_Custom_Measure_Time_Init(E_BF_MEASURE_TIME_ID_FILE_WRITE);
#endif
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Write(fNo, (const FW_VOID*)addr, count, length);
#ifdef CO_BF_FS_USER_CUSTOM_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(E_BF_MEASURE_TIME_ID_FILE_WRITE, "BF_Fs_Controller_Write() END.\n");
#endif

	g_bf_fs_controller_ret_length = *length;
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, fNo=%x, addr=%lx, count=%lu, lengt = %lu\n", g_bf_fs_controller_ret_err, fNo, addr, count, *length));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Get result of BF_Fs_Controller_Write.
 * @param o:*length the pointer of area which is stored the actual written size
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Write_Result(FW_ULONG* length)
{
	if(length == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	*length = g_bf_fs_controller_ret_length;
	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute creating specified file.
 * @param i:*fname the pointer of array of file name
 * @param i:amode File attribute
 * @return success: File ID
 *         failuer: 0
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Create(FW_CHAR* fname, FW_USHORT amode)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Create((const FW_CHAR*)fname, amode);

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
	if( g_bf_fs_controller_ret_err < 0 ){
		g_bf_fs_controller_ret_err = 0;
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Create() failed: ercd=%d, fname=%s, amode=%d\n", g_bf_fs_controller_ret_err, fname, amode));
#endif
	}

#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	if( g_bf_fs_controller_ret_err == 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Create() failed: ercd=%d, fname=%s, amode=%d\n", g_bf_fs_controller_ret_err, fname, amode));
#endif
	}
#endif // CO_FS_USED_MDF				// --- REMOVE_MDF END ---

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute copying the specified source file to destination file.
 * @param i:*srcname the pointer of array of source file name
 * @param i:*dstname the pointer of array of destination file name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Copy(FW_CHAR* srcname, FW_CHAR* dstname)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Copy((const FW_CHAR*)srcname, (const FW_CHAR*)dstname);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Copy() failed: ercd=%d, srcname=%s, dstname=%s\n", g_bf_fs_controller_ret_err, srcname, dstname));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute deleting the specified file.
 * @param i:*fname the pointer of array of file name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Remove(FW_CHAR* fname)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Remove((const FW_CHAR*)fname);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove() failed: ercd=%d, fname=%s\n", g_bf_fs_controller_ret_err, fname));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute deleting the specified file.
 * @param i:*fname the pointer of array of file name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Remove_Multi(const FW_CHAR* dirname, FW_CHAR* filename_list, FW_INT32 fnum, FS_DEL_MULTI_CB pcallback)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Remove_Multi((const FW_CHAR*)dirname, (FW_CHAR (*)[13])filename_list, fnum, (MULTI_DEL_CB)pcallback);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove() failed: ercd=%d\n", g_bf_fs_controller_ret_err));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute acquiring the file information of specified file number.
 * @param i:fNo the file number of target file
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Fstat(FW_INT32 fNo, T_BF_FS_IF_STAT* sbuf)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Fstat(fNo, sbuf);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Fstat() failed: ercd=%d, fNo=%x\n", g_bf_fs_controller_ret_err, fNo));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute creating specified directory.
 * @param i:*dirname the pointer of array of directory name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Mkdir(FW_CHAR* dirname)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Mkdir((const FW_CHAR*)dirname, 0x01ff);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Mkdir() failed: ercd=%d, dirname=%s\n", g_bf_fs_controller_ret_err, dirname));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute deleting specified directory.
 * @param i:*dirname the pointer of array of directory name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Rmdir(FW_CHAR* dirname)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Rmdir((const FW_CHAR*)dirname);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Rmdir() failed: ercd=%d, dirname=%s\n", g_bf_fs_controller_ret_err, dirname));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute opening specified directory.
 * @param i:*dirname the pointer of array of directory name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Opendir(FW_CHAR* dirname, FW_INT32* dNo)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Opendir((const FW_CHAR*)dirname, dNo);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", g_bf_fs_controller_ret_err, dirname));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute acquiring the file information of directory.
 * @param i:fNo the directory number of target directory
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Readdir(FW_INT32 dNo, T_BF_FS_IF_Dir_Info* fsdirent)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Readdir(dNo, fsdirent);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d, dNo=%x\n", g_bf_fs_controller_ret_err, dNo));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute changing the current directory and the current drive.
 * @param i:*path the current directory name or the current drive name to be changed
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Chdir(FW_CHAR* path)
{
	FW_INT32 name_len, media_id;

	for (name_len = 0; name_len < D_BF_FS_IF_DRVNAME_MAX; name_len++) {
		if (path[name_len] == ':') {
			break;
		}
	}
	for (media_id = 0; media_id < D_BF_FS_CONTROLLER_MEDIA_ID_NUM; media_id++) {
		if (strncmp((const CHAR*)path, (const CHAR*)gBF_Controller_drv_name[media_id], name_len) == 0){
			break;
		}
	}

	if (media_id < D_BF_FS_CONTROLLER_MEDIA_ID_NUM) {
		BF_Media_State_Set_Used_Media(gBF_Fs_Controller_Media_Status[media_id].media_type);
	}

	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Chdir((const FW_CHAR*)path);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Chdir() failed: ercd=%d, path=%s\n", g_bf_fs_controller_ret_err, path));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute storing the current directory array of specified drive to the specified area.
 * @param i:*drvname the current directory name or the current drive name to be changed
 * @param i:size the size of array to store the current directory
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Get_Cur_Dir(FW_CHAR* drvname, FW_ULONG size, FW_CHAR* curpath)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Get_Cur_Dir((const FW_CHAR*)drvname, curpath, size);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Get_Cur_Dir() failed: ercd=%d, drvname=%s, curpath = %s, size=%lu\n", g_bf_fs_controller_ret_err, drvname, curpath, size));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute changing the specified attribute of specified file.
 * @param i:*fname the pointer of array of file name
 * @param i:*attributes the file attribute to be changed
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Set_Protection_Mode(FW_CHAR* drvname, FW_UCHAR wrt_protect)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Set_Protection_Mode((const FW_CHAR*)drvname, wrt_protect);		/*	protection mode set to MDF			*/
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Protection_Mode() failed: ercd=%d, drvname=%s, prot=%d\n", g_bf_fs_controller_ret_err, drvname, wrt_protect));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute to set media state sampling interval time and count for BF_Fs_Controller_Get_Media_State().
 * @param i:ch             sd channel
 *        i:interval_time  sampling interval time
 *        i:check_count    sampling count
 * @return none (allways 0)
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Set_Media_State(FW_UCHAR ch, FW_UCHAR interval_time, FW_UCHAR check_count)
{
	BF_Media_State_Set_Media_Check_Param(ch, interval_time, check_count);
	return 0;
}

/**
 * @brief Execute to get media state (sampling time and count was setted by BF_Fs_Controller_Set_Media_State()).
 * @param i:ch   sd channel
 * @return media status(insert/leave)
 * @note none
 * @attention none.
 */
E_BF_MEDIA_STATE_MEDIA_STATE BF_Fs_Controller_Get_Media_State(FW_UCHAR ch)
{
	return BF_Media_State_Get_Media_State(ch);
}

/**
 * @brief Execute to get media state (Polling).
 * @param i:ch             sd channel
 *        i:interval_time  sampling interval time
 *        i:check_count    sampling count
 *        i:gpio_direct    select to read media state direct form GPIO or use Media macro
 *                          1 : direct read from GPIO
 *                          0 : read from Media macro
 * @return media status(insert/leave)
 * @note none
 * @attention none.
 */
E_BF_MEDIA_STATE_MEDIA_STATE BF_Fs_Controller_Get_Media_State_By_Poll(FW_UCHAR ch, FW_UCHAR interval_time, FW_UCHAR check_count)
{
	return BF_Media_State_Get_Media_State_By_Poll(ch, interval_time, check_count);
}

/**
 * @brief Execute to polling media state.
 * @param i:media_type media type
 * @return media status(insert/leave)
 * @note none
 * @attention none.
 */
E_BF_MEDIA_STATE_MEDIA_STATE BF_Fs_Controller_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
	E_BF_MEDIA_STATE_MEDIA_TYPE tmp_media_type = media_type;

	if( tmp_media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_CUR ){
		tmp_media_type = BF_Media_State_Get_Used_Media();
	}

	return BF_Media_State_Check_Media_State(tmp_media_type);
}

/**
 * @brief Execute to get current media type.
 * @param FW_VOID
 * @return media type
 * @note none
 * @attention none
 */
E_BF_MEDIA_STATE_MEDIA_TYPE  BF_Fs_Controller_Get_Cur_Media(FW_VOID)
{
	return BF_Media_State_Get_Used_Media();
}

/**
 * @brief Execute to get cached free size of the specified media.
 * @param i:media_type media type
 * @param o:*free_size free space size(byte)
 * @return return code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Get_FreeSize(E_BF_MEDIA_STATE_MEDIA_TYPE media_type, FW_DOUBLE* free_size)
{
	FW_INT32 ercd;
	FW_CHAR drive_name[8];
	E_BF_MEDIA_STATE_MEDIA_TYPE used_media;

	if(media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_CUR){
		used_media = BF_Media_State_Get_Used_Media();
	}
	else if(media_type != BF_Media_State_Get_Used_Media()){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("BF_Fs_Controller_Get_FreeSize(): NOT USED MEDIA: used_media=%d, target_media=%d\n", BF_Media_State_Get_Used_Media(), media_type));
#endif
		*free_size = 0;
		return FSDCF_ERRNO_NOT_INSERT_MEDIA;
	}
	else{
		used_media = media_type;
	}

	ercd = BF_Media_State_Exchage_Type_To_Drive_Name(used_media, drive_name);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("BF_Fs_Controller_Get_FreeSize(): NOT SUPPORT MEDIA: cur_used_media=%d\n", used_media));
#endif
		*free_size = 0;
		return ercd;
	}

	/* for Movie */
	*free_size = BF_Mdf_Wrapper_Get_Free_Space_By_Cache(drive_name);

	if(*free_size < 0 ){
		*free_size = 0;
	}
	return 0;
}

FW_INT32      BF_Fs_Controller_Check_Mbr(FW_VOID)
{
	return BF_Media_Drv_Wrapper_Check_MBR();
}

FW_INT32      BF_Fs_Controller_Make_Mbr(FW_VOID)
{
	return BF_Media_Drv_Wrapper_Write_MBR();
}

FW_INT32      BF_Fs_Controller_Set_Media_DMA(FW_UCHAR enable)
{
	E_BF_MEDIA_STATE_MEDIA_TYPE	media_type;

	media_type = BF_Media_State_Get_Used_Media();
	if (media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD) {
		MWSDIO0_Set_DMA_Enable(enable);
	}
	else if (media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1) {
		MWSDIO1_Set_DMA_Enable(enable);
	}
	else {
		return -1;
	}

	return 0;
}

FW_INT32      BF_Fs_Controller_Get_Media_Format_Type(FW_UCHAR* used_media, FW_UCHAR* format_type)
{
	FW_INT32 ercd;
	E_BF_MEDIA_STATE_MEDIA_TYPE media_type;
	T_BF_FS_IF_STATVFS stat_buf;

	if ((used_media == NULL) || (format_type == NULL)) {
		// Null Pointer
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	media_type = BF_Media_State_Get_Used_Media();
	if (media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF) {
		*used_media = 1;	// NF
		ercd = BF_Mdf_Wrapper_Statvfs((const FW_CHAR*)"I", &stat_buf);
		if (ercd != 0) {
			*format_type = 0;	// unknown
		}
		else {
			*format_type = (FW_UCHAR)stat_buf.fstype;
		}
	}
	else if (media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD) {
		BF_Media_Drv_Wrapper_Get_Media_Format_Type_SDIO0((FW_UCHAR*)used_media, (FW_UCHAR*)format_type);
	}
	else if (media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1) {
		BF_Media_Drv_Wrapper_Get_Media_Format_Type_SDIO1((FW_UCHAR*)used_media, (FW_UCHAR*)format_type);
	}
	else {
		*used_media = 0;
		*format_type = 0;
	}

	return 0;
}

//#pragma arm section code="BASEFW_COMM_CODE"

/**
 * @brief Execute moving the file pointer of the specified file number.
 * @param i:fNo the file number of target file
 * @param i:offset the offset value (byte) from the standard position
 * @param i:whence indicate the standard position of offset
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Lseek(FW_INT32 fNo, FW_INT32 offset, FW_USHORT whence, FW_ULONG* length)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Lseek(fNo, offset, whence, length);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, fNo=%x, offset=%x, whence=%d\n", g_bf_fs_controller_ret_err, fNo, offset, whence));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute moving the file pointer of the specified file number.
 * @param i:fNo the file number of target file
 * @param i:offset the offset value (byte) from the standard position
 * @param i:whence indicate the standard position of offset
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Lseek64(FW_INT32 fNo, FW_INT64 offset, FW_USHORT whence, FW_INT64* length)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Lseek64(fNo, offset, whence, length);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek64() failed: ercd=%d, fNo=%x, offset=%llx, whence=%d\n", g_bf_fs_controller_ret_err, fNo, offset, whence));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute closing the specified directory.
 * @param i:fNo the directory number of target directory
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Closedir(FW_INT32 dNo)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Closedir(dNo);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d, dNo=%x\n", g_bf_fs_controller_ret_err, dNo));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief end drive (I drive and NF data drive)
 * @param none
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_Controller_End_Drv(FW_VOID)
{
#if 0
	FW_INT32 ercd;
	FW_UINT32 w_addr;

	w_addr = (FW_UINT32)gMW_Exs_IRAM_NF;
#if 1	// for test
	BF_Filesystem_File_Reset_Directory();
	BF_Dcf_If_End_Task_Mdf();
#endif
	ercd = BF_Mdf_Wrapper_End_Task();
	if (ercd != 0) {
		BF_Debug_Print_Error(("end Task failed (%d)\n", ercd));
	}
	if (*(gBF_Fs_Controller_Media_Status.devid) != '\0') {
		ercd = BF_Mdf_Wrapper_End_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status.devid);
		if (ercd != 0) {
			BF_Debug_Print_Error(("end Drive \"%s\" failed (%d)\n", gBF_Fs_Controller_Media_Status.devid, ercd));
		}
	}

	ercd = MWNF_BasicInit(w_addr, 1);

	ercd = BF_Mdf_Wrapper_End_Drv("A");
#if 1       // for debug
	if (ercd != 0) {
		BF_Debug_Print_Error(("BF_Mdf_Wrapper_End_Drv(A) failed(%d)\n", ercd));
	}
#endif
	ercd = BF_Mdf_Wrapper_End_Drv("B");
#if 1       // for debug
	if (ercd != 0) {
		BF_Debug_Print_Error(("BF_Mdf_Wrapper_End_Drv(B) failed(%d)\n", ercd));
	}
#endif
	ercd = BF_Mdf_Wrapper_End_Drv("C");
#if 1       // for debug
	if (ercd != 0) {
		BF_Debug_Print_Error(("BF_Mdf_Wrapper_End_Drv(C) failed(%d)\n", ercd));
	}
#endif
	ercd = BF_Mdf_Wrapper_End_Drv("D");
#if 1       // for debug
	if (ercd != 0) {
		BF_Debug_Print_Error(("BF_Mdf_Wrapper_End_Drv(D) failed(%d)\n", ercd));
	}
#endif
	ercd = BF_Mdf_Wrapper_End_Drv("E");
#if 1       // for debug
	if (ercd != 0) {
		BF_Debug_Print_Error(("BF_Mdf_Wrapper_End_Drv(E) failed(%d)\n", ercd));
	}
#endif
	ercd = BF_Mdf_Wrapper_End_Drv("F");
#if 1       // for debug
	if (ercd != 0) {
		BF_Debug_Print_Error(("BF_Mdf_Wrapper_End_Drv(F) failed(%d)\n", ercd));
	}
#endif
	ercd = BF_Mdf_Wrapper_End_Drv("G");
#if 1       // for debug
	if (ercd != 0) {
		BF_Debug_Print_Error(("BF_Mdf_Wrapper_End_Drv(G) failed(%d)\n", ercd));
	}
#endif

	BF_Mdf_Wrapper_End_Mdf();
#endif
	return 0;
}

/**
 * @brief init drive (I drive and NF data drive)
 * @param i:mount Set the following value
 *				D_BF_FS_IF_NOT_MOUNT_I
 *				D_BF_FS_IF_MOUNT_I
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_Controller_Init_Drv(FW_UCHAR mount)
{
#if 0
	FW_INT32 ercd;

	ercd = NF_InitDataDrive();
	if( ercd != 0 ) {
		return ercd;
	}

//	ercd = BF_Mdf_Wrapper_Start_Mdf();
//	ercd = MWNF_BasicInit();
	if(mount == D_BF_FS_IF_MOUNT_I){
		if (*(gBF_Fs_Controller_Media_Status.devid) != '\0') {
			if (BF_Media_State_Check_Media_State(gBF_Fs_Controller_Media_Status.media_type) == E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
				ercd = BF_Mdf_Wrapper_Init_Drv((FW_CHAR*)gBF_Fs_Controller_Media_Status.devid, 
							gBF_Fs_Controller_Media_Status.driver_tbl, 
							gBF_Fs_Controller_Media_Status.media_info);

				if (ercd != 0) {
					BF_Debug_Print_Error(("init Drive \"%s\" failed (%d)\n", gBF_Fs_Controller_Media_Status.devid, ercd));
					return ercd;
				}
				if ( strcmp((FW_CHAR*)gBF_Fs_Controller_Media_Status.devid, "J") == 0 ) {
					ercd = BF_Mdf_Wrapper_Chdir("J:");
					if (ercd != 0) {
						BF_Debug_Print_Error(("FS_chdir(\"J:\") failed (%d)\n", ercd));
						return ercd;
					}
				}
				else {
					ercd = BF_Mdf_Wrapper_Chdir("I:");
					if (ercd != 0) {
						BF_Debug_Print_Error(("FS_chdir(\"I:\") failed (%d)\n", ercd));
						return ercd;
					}
				}
				BF_Dcf_If_Set_Cur_Drive();
			}
		}
	}
#endif
	return 0;
}

/**
 * @brief Execute storing the current drive array.
 * @param FW_VOID
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Get_Cur_Drv(FW_CHAR* drvname)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Get_Cur_Drv(drvname);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Get_Cur_Drv() failed: ercd=%d\n", g_bf_fs_controller_ret_err));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute to get media state.
 * @param i:media_type media type
 * @return media status(insert/leave)
 * @note none
 * @attention none.
 */
E_BF_MEDIA_STATE_MEDIA_WP    BF_Fs_Controller_Get_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
	E_BF_MEDIA_STATE_MEDIA_TYPE tmp_media_type = media_type;

	if( tmp_media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_CUR ){
		tmp_media_type = BF_Media_State_Get_Used_Media();
	}

	return BF_Media_State_Get_Media_Protect(tmp_media_type);
}

FW_INT32	BF_Fs_Controller_Copy_File_To_Drive(FW_CHAR* szSrcDrive, FW_CHAR* szDstDrive)
{
	FW_INT32 ercd, ercd2, dno;
	FW_INT32 src_path_len, dst_path_len;
	FW_CHAR src_path[30], dst_path[30];
	T_BF_FS_IF_Dir_Info dirent;
	FW_UCHAR media_id;
	
	BF_Fs_Controller_Get_Media_Id(szDstDrive, (FW_CHAR*)&media_id);
	if (media_id == 0xFF) {
		return -1;
	}

	strcpy(src_path, szSrcDrive);
	strcat(src_path, ":\\");
	strcat(src_path, szDstDrive);
	strcpy(dst_path, szDstDrive);
	strcat(dst_path, ":\\");

	ercd = BF_Mdf_Wrapper_Opendir(src_path, &dno);
	if (ercd < 0) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Debug_Print_Error(("BF_Mdf_Wrapper_Opendir() failed(%d)\n", ercd));
#endif
		return ercd;
	}

	strcat(src_path, "\\");
	src_path_len = strlen(src_path);
	dst_path_len = strlen(dst_path);

	if (gNF_Drive_Protect[media_id] == E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT) {
		ercd = BF_Mdf_Wrapper_Set_Protection_Mode(szDstDrive, E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT);
		if (ercd < 0) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
			BF_Debug_Print_Error(("BF_Mdf_Wrapper_Set_Protection_Mode() failed(%d)\n", ercd));
#endif
			BF_Mdf_Wrapper_Closedir(dno);
			return ercd;
		}
	}

	while (1) {
		ercd = BF_Mdf_Wrapper_Readdir(dno, &dirent);
		if (ercd == D_BF_MDF_WRAPPER_ENT_LAST) {
			ercd = 0;
			break;
		}
		else if (ercd < 0) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
			BF_Debug_Print_Error(("BF_Mdf_Wrapper_Readdir() failed(%d)\n", ercd));
#endif
			break;
		}

		if (strcmp(dirent.fname, ".") == 0) {
			continue;
		}
		else if (strcmp(dirent.fname, "..") == 0) {
			continue;
		}

		strcpy(&src_path[src_path_len], dirent.fname);
		strcpy(&dst_path[dst_path_len], dirent.fname);
		ercd = BF_Mdf_Wrapper_Copy(src_path, dst_path);
		if (ercd < 0) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
			BF_Debug_Print_Error(("BF_Mdf_Wrapper_Copy() failed(%d)\n", ercd));
#endif
			break;
		}
	}

	ercd2 = BF_Mdf_Wrapper_Closedir(dno);
	if (ercd2 < 0) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Debug_Print_Error(("BF_Mdf_Wrapper_Closedir() failed(%d)\n", ercd2));
#endif
	}

	BF_Mdf_Wrapper_Sync((FW_CHAR*)&gNF_Drive_Name[media_id][0]);
	if (gNF_Drive_Protect[media_id] == E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT) {
		ercd2 = BF_Mdf_Wrapper_Set_Protection_Mode(szDstDrive, E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT);
		if (ercd2 < 0) {
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
			BF_Debug_Print_Error(("BF_Mdf_Wrapper_Set_Protection_Mode() failed(%d)\n", ercd2));
#endif
			return ercd2;
		}
	}

	return ercd;
}

//#pragma arm section code="BASEFW_LAST_CODE"
/////////////////////////////////////////////////////
// romarea_last
/////////////////////////////////////////////////////

/**
 * @brief Execute reflecting the directory entry with drive name.
 * @param i:*drvname the pointer of array of drive name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Sync(FW_CHAR* drvname)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Sync((const FW_CHAR*)drvname);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Sync() failed: ercd=%d, drvname=%s\n", g_bf_fs_controller_ret_err, drvname));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute reflecting the directory entry with the specified file number.
 * @param i:fNo the file number of target file
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Fsync(FW_INT32 fNo)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Fsync(fNo);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Fsync() failed: ercd=%d, fNo=%x\n", g_bf_fs_controller_ret_err, fNo));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute changing (moving) the specified file name or directory name.
 * @param i:*srcname the pointer of array of current file name
 * @param i:*dstname the pointer of array of changed file name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Rename(FW_CHAR* srcname, FW_CHAR* dstname)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Rename((const FW_CHAR*)srcname, (const FW_CHAR*)dstname);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Rename() failed: ercd=%d, srcname=%s, dstname=%s\n", g_bf_fs_controller_ret_err, srcname, dstname));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute changing the specified attribute of specified file.
 * @param i:*fname the pointer of array of file name
 * @param i:*attributes the file attribute to be changed
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Set_Attribute(FW_CHAR* fname, FW_UCHAR attributes)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Set_Attribute((const FW_CHAR*)fname, attributes);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Attribute() failed: ercd=%d, fname=%s, attr=%d\n", g_bf_fs_controller_ret_err, fname, attributes));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute changing the specified attribute of specified file.
 * @param i:*fname the pointer of array of file name
 * @param i:*attributes the file attribute to be changed
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Set_Attribute_Multi(const FW_CHAR* dirname, FW_UCHAR attributes, FW_UCHAR attr_mode, FW_CHAR* filename_list, FW_INT32 fnum, FS_SET_ATTR_MULTI_CB pcallback)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Set_Attribute_Multi((const FW_CHAR*)dirname, attributes, attr_mode, (FW_CHAR (*)[13])filename_list, fnum, (MULTI_SET_ATTR_CB)pcallback);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Attribute() failed: ercd=%d, attr=%u\n", g_bf_fs_controller_ret_err, attributes));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief change used media
 * @param i:media_type media type to change
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_Controller_Change_Media(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
	FW_INT32 ercd;

	BF_Fs_Controller_End_Media(E_BF_MEDIA_STATE_MEDIA_TYPE_NF);

	ercd = BF_Fs_Controller_Start_Media(media_type);
	if( ercd != 0 ) {
		return ercd;
	}

	if ( media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF ) {
		ercd = BF_Fs_Controller_Chdir("J:");
	}
	else {
		ercd = BF_Fs_Controller_Chdir("I:");
	}

	return ercd;
}

FW_INT32      BF_Fs_Controller_Init_SD0_For_Copy(FW_VOID)
{
	FW_INT32		ercd;													/*	error code							*/
	FW_ULONG		wrt_protect;											/*	write protection mode				*/
	E_BF_MEDIA_DRV_WRAPPER_WP	media_wp;												/*	media write protect status			*/
	T_FS_FUNCNAME*	driver_tbl;												/*	ptr to driver table					*/
	FW_VOID*		media_info;												/*	inner parameter for media driver	*/

	/* check media insert */
	if (BF_Media_State_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD) != E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
		/* not inserted */
		return FSDCF_ERRNO_NOT_INSERT_MEDIA;
	}

	ercd = BF_Media_Drv_Wrapper_Init_SDIO0(&driver_tbl, &media_info, &media_wp);			/*	SD0 Driver Initialize				*/
	if(ercd < 0){
		/* Initialize failed */
		return FSDCF_ERRNO_DRIVE_INIT_FAILED;
	}

	/* filesystem initialize */
	ercd = BF_Mdf_Wrapper_Init_Drv(D_BF_FS_IF_DST_DRIVE_NAME_STRING, driver_tbl, media_info);	/*	Initialize SD0 to MDF				*/
	if (ercd != 0) {
		/* initialize to MDF failed */
		return ercd;
	}

	/* set max read/write sector cnt */
	ercd = BF_Mdf_Wrapper_Set_Con_Sec_Max(D_BF_FS_IF_DST_DRIVE_NAME_STRING, D_BF_FS_CONTROLLER_SECTOR_MAX);			/*	Max sector count set to MDF			*/
	if (ercd != 0) {
		/* Max sector count set to MDF failed */
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Con_Sec_Max failed: ercd=%d, devid=%s\n", ercd, D_BF_FS_IF_DST_DRIVE_NAME_STRING));
#endif
	}

	/* set write protection mode */
	if (media_wp == E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT) {
		/* Write protect */
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT;
		BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_SD, E_BF_MEDIA_STATE_MEDIA_WP_PROTECT);
	}
	else {
		/* Not Write protect */
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;
		BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_SD, E_BF_MEDIA_STATE_MEDIA_WP_UNPROTECT);
	}

	ercd = BF_Mdf_Wrapper_Set_Protection_Mode(D_BF_FS_IF_DST_DRIVE_NAME_STRING, wrt_protect);		/*	protection mode set to MDF			*/
	if (ercd != 0) {
		/* set protection mode failed */
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FS_setProtectionMode failed: ercd=%d, devid=%s\n", ercd, D_BF_FS_IF_DST_DRIVE_NAME_STRING));
#endif
	}

	return ercd;
}

FW_INT32      BF_Fs_Controller_Init_SD1_For_Copy(FW_VOID)
{
	FW_INT32		ercd;													/*	error code							*/
	FW_ULONG		wrt_protect;											/*	write protection mode				*/
	E_BF_MEDIA_DRV_WRAPPER_WP	media_wp;												/*	media write protect status			*/
	T_FS_FUNCNAME*	driver_tbl;												/*	ptr to driver table					*/
	FW_VOID*		media_info;												/*	inner parameter for media driver	*/

	/* check media insert */
	if (BF_Media_State_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1) != E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
		/* not inserted */
		return FSDCF_ERRNO_NOT_INSERT_MEDIA;
	}

	ercd = BF_Media_Drv_Wrapper_Init_SDIO1(&driver_tbl, &media_info, &media_wp);			/*	SD1 Driver Initialize				*/
	if(ercd < 0){
		/* SD1 Initialize failed */
		return FSDCF_ERRNO_DRIVE_INIT_FAILED;
	}

	/* filesystem initialize */
	ercd = BF_Mdf_Wrapper_Init_Drv(D_BF_FS_IF_DST_DRIVE_NAME_STRING, driver_tbl, media_info);	/*	Initialize SD1 to MDF				*/
	if (ercd != 0) {
		/* initialize to MDF failed */
		return ercd;
	}

	/* set max read/write sector cnt */
	ercd = BF_Mdf_Wrapper_Set_Con_Sec_Max(D_BF_FS_IF_DST_DRIVE_NAME_STRING, D_BF_FS_CONTROLLER_SECTOR_MAX);			/*	Max sector count set to MDF			*/
	if (ercd != 0) {
		/* Max sector count set to MDF failed */
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Con_Sec_Max failed: ercd=%d, devid=%s\n", ercd, D_BF_FS_IF_DST_DRIVE_NAME_STRING));
#endif
	}

	/* set write protection mode */
	if (media_wp == E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT) {
		/* Write protect */
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT;
		BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1, E_BF_MEDIA_STATE_MEDIA_WP_PROTECT);
	}
	else {
		/* Not Write protect */
		wrt_protect = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;
		BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1, E_BF_MEDIA_STATE_MEDIA_WP_UNPROTECT);
	}

	ercd = BF_Mdf_Wrapper_Set_Protection_Mode(D_BF_FS_IF_DST_DRIVE_NAME_STRING, wrt_protect);		/*	protection mode set to MDF			*/
	if (ercd != 0) {
		/* set protection mode failed */
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FS_setProtectionMode failed: ercd=%d, devid=%s\n", ercd, D_BF_FS_IF_DST_DRIVE_NAME_STRING));
#endif
	}

	return ercd;
}

FW_INT32      BF_Fs_Controller_End_SD_For_Copy(FW_VOID)
{
	FW_INT32 ercd;															/*	error code							*/

	ercd = BF_Mdf_Wrapper_End_Drv(D_BF_FS_IF_DST_DRIVE_NAME_STRING);								/*	End drive MDF					*/

	if (ercd != 0) {
		/* End drive failed */
		if ((ercd != D_BF_MDF_WRAPPER_ERR_NODEV) && (ercd != D_BF_MDF_WRAPPER_ERR_NXIO) && (ercd != D_BF_MDF_WRAPPER_ERR_NOMDF)) {
			/* error */
			return ercd;
		}
	}

	return 0;
}
#if 0
/**
 * @brief clear the specified area and edit date/time in the file entry.
 * @param i:file_name file name to clear specified area and edit date/time.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_Controller_Clear_Entry_Info(const FW_CHAR* file_name)
{
	FW_INT32 ercd;
	T_BF_FS_utimbuf date_time;

	date_time.date = 0;
	date_time.time = 0;

	ercd = BF_Mdf_Wrapper_Utime((const FW_CHAR*)file_name, (const T_BF_FS_utimbuf*)&date_time);
	if (ercd != 0) {
		return ercd;
	}

	return BF_Mdf_Wrapper_EXT_Clear_Special_Entry((const FW_CHAR*)file_name);
}
#endif

/**
 * @brief copy drive to drive
 * @param i FW_CHAR* szSrcDrive : source drive name
 *        i FW_CHAR* szDstDrive : destination drive name
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_Controller_Drive_To_Drive_Copy(FW_CHAR* szSrcDrive, FW_CHAR* szDstDrive)
{
	FW_INT32 ercd;
	FW_INT32 ret_code;
	FW_INT32 src_path_len;		/* source path length */
	FW_INT32 dst_path_len;		/* destination path length */
	FW_INT32 dir_name_len;		/* directory name length */
	FW_CHAR  src_path[512];		/* source path */
	FW_CHAR  dst_path[512];		/* destination path */
	FW_CHAR  dir_name[766];		/* directory name */
	FW_INT32 dir_no;			/* directory No */
	T_BF_FS_IF_Dir_Info dir_ent;			/* directory entry information */

	ret_code = 0;

	/* source path set */
	strcpy(src_path, szSrcDrive);
	src_path_len = strlen(src_path);
	strcpy(&src_path[src_path_len], ":\\");
	src_path_len += 2;									/* add ":\\" length */

	/* destination path set */
	strcpy(dst_path, szDstDrive);
	dst_path_len = strlen(dst_path);
	strcpy(&dst_path[dst_path_len], ":\\");
	dst_path_len += 2;									/* add ":\\" length */

	ercd = BF_Mdf_Wrapper_Opendir(src_path, &dir_no);				/* source drive root directory open */
	if (ercd != 0) {
		/* Directory open error */
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Debug_Print_Error(("BF_Mdf_Wrapper_Opendir(\"%s\") failed(%d)\n", src_path, ercd));
#endif
		return ercd;
	}

	while (1) {
		/* Read all the entries in the source drive */
		ercd = BF_Mdf_Wrapper_Readdir(dir_no, &dir_ent);			/* read directory */
		if (ercd == D_BF_MDF_WRAPPER_ENT_LAST) {
			/* Exit all the entries in the current directory */
			BF_Mdf_Wrapper_Closedir(dir_no);						/* directory close */
			src_path_len--;								/* sub '\\' length */
			src_path[src_path_len] = '\0';				/* '\\' delete */
			while (src_path[src_path_len-1] != '\\') {
				/* Search the directory of the previous one */
				if (src_path_len <= 1) {
					/* Quit reading all entries in the source drive */
					return ret_code;					/* End Drive Copy */
				}
				src_path_len--;
			}

			strcpy(dir_name, &src_path[src_path_len]);	/* Save the name of the directory you've already read(skipped until later this directory) */
			dir_name_len = strlen(dir_name);			/* Save DirectoryName & StringLength */
			src_path[src_path_len] = '\0';				/* Delete the directory name from the name of the source path, finished reading */

			/* source directory open */
			ercd = BF_Mdf_Wrapper_Opendir(src_path, &dir_no);		/* Open the directory of the previous one */
			if (ercd != 0) {
				/* directory open error */
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
				BF_Debug_Print_Error(("BF_Mdf_Wrapper_Opendir(\"%s\") failed(%d)\n", src_path, ercd));
#endif
				return ercd;
			}

			while (1) {
				/* Skip the directory until you finish reading */
				ercd = BF_Mdf_Wrapper_Readdir(dir_no, &dir_ent);	/* read directory */
				if (ercd != 0) {
					/* read directory error */
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
					BF_Debug_Print_Error(("BF_Mdf_Wrapper_Readdir() failed(%d)\n", ercd));
#endif
					BF_Mdf_Wrapper_Closedir(dir_no);				/* directory close */
					return ercd;						/* error return */
				}

				if (strcmp(dir_name, dir_ent.fname) == 0) {
					break;								/* Directory last read */
				}
			}

			/* set destination path */
			dst_path_len -= (dir_name_len + 1);			/* sub dir_name_len & '\\' */
			dst_path[dst_path_len] = '\0';				/* dir_name+'\\' delete(Directory path to the previous one) */

			continue;
		}
		else if (ercd != 0) {
			/* directory read error */
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
			BF_Debug_Print_Error(("BF_Mdf_Wrapper_Readdir() failed(%d)\n", ercd));
#endif
			BF_Mdf_Wrapper_Closedir(dir_no);						/* directory close */
			return ercd;								/* error return */
		}
		else {
			; // DO NOTHING
		}

		if (dir_ent.attributes == D_BF_FS_IF_ATTR_DIRECTORY) {
			/* directory */
			if ((strcmp(dir_ent.fname, ".") == 0) || (strcmp(dir_ent.fname, "..") == 0)) {
				/* Neglect "." & ".." */
				continue;
			}
			/* directory name copy to workarea */
			strcpy(dir_name, dir_ent.fname);			/* Save the name of the directory to read */
			dir_name_len = strlen(dir_name);			/* Get DirectoryName & StringLength */

			/* destination path set */
			strcpy(&dst_path[dst_path_len], dir_name);	/* Name appended to the destination directory path */
			dst_path_len += dir_name_len;				/* Plus the length of the directory name */

			/* destination directory create */
			ercd = BF_Mdf_Wrapper_Mkdir(dst_path, 511);				/* create directory */
			if (ercd != 0) {
				if (ercd == D_BF_MDF_WRAPPER_ERR_EXIST) {
					/* destination directory is already exist */
				}
				else if (ercd == D_BF_MDF_WRAPPER_ERR_ROFS) {
					/* destination drive is read only */
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
					BF_Debug_Print_Error(("Destination drive -\"%s\" is read only\n", szDstDrive));
#endif
					BF_Mdf_Wrapper_Closedir(dir_no);				/* close current directory */
					return ercd;						/* error return */
				}
				else {
					/* destination directory create error */
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
					BF_Debug_Print_Error(("BF_Mdf_Wrapper_Mkdir(\"%s\") failed(%d)\n", dst_path, ercd));
#endif
					ret_code = -1;
					dst_path_len -= dir_name_len;
					dst_path[dst_path_len] = '\0';
					continue;
				}
			}

			BF_Mdf_Wrapper_Closedir(dir_no);						/* close current directory */

			/* destination path set */
			strcpy(&dst_path[dst_path_len], "\\");		/* add ":\\" length */
			dst_path_len += 1;

			/* source path set */
			strcpy(&src_path[src_path_len], dir_name);	/* Added to the source directory path name */
			src_path_len += dir_name_len;				/* add directory name length */
			strcpy(&src_path[src_path_len], "\\");		/* add ":\\" length */
			src_path_len += 1;

			ercd = BF_Mdf_Wrapper_Opendir(src_path, &dir_no);		/* open next directory */
			if (ercd != 0) {
				/* directory open error */
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
				BF_Debug_Print_Error(("BF_Mdf_Wrapper_Opendir(\"%s\") failed(%d)\n", src_path, ercd));
#endif
				return ercd;
			}
		}
		else {
			/* file */
			/* source path set */
			strcpy(&src_path[src_path_len], dir_ent.fname);

			/* destination path set */
			strcpy(&dst_path[dst_path_len], dir_ent.fname);

			/* copy file */
			ercd = BF_Mdf_Wrapper_Copy(src_path, dst_path);
			if (ercd != 0) {
				if (ercd == D_BF_MDF_WRAPPER_ERR_ROFS) {
					/* destination drive is read only */
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
					BF_Debug_Print_Error(("Destination drive -\"%s\" is read only\n", szDstDrive));
#endif
					return ercd;
				}
				else if (ercd == D_BF_MDF_WRAPPER_ERR_NOSPC) {
					/* drive space error */
					/* remove destination file for retry copy(Copy over) */
					ercd = BF_Mdf_Wrapper_Remove(dst_path);
					if (ercd != 0) {
						/* file remove error */
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
						BF_Debug_Print_Error(("BF_Mdf_Wrapper_Copy(\"%s\", \"%s\") failed(%d)\n", src_path, dst_path, ercd));
#endif
						ret_code = -1;
						continue;
					}

					/* retry copy */
					ercd = BF_Mdf_Wrapper_Copy(src_path, dst_path);
					if (ercd != 0) {
						/* retry copy error */
						ret_code = -1;
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
						BF_Debug_Print_Error(("BF_Mdf_Wrapper_Copy(\"%s\", \"%s\") failed(%d)\n", src_path, dst_path, ercd));
#endif
					}
				}
				else {
					/* copy file error */
					ret_code = -1;
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
					BF_Debug_Print_Error(("BF_Mdf_Wrapper_Copy(\"%s\", \"%s\") failed(%d)\n", src_path, dst_path, ercd));
#endif
				}
			}
//			BF_Debug_Print_Information(("BF_Mdf_Wrapper_Copy(\"%s\", \"%s\") end.\n", src_path, dst_path);
		}
	}
}

FW_INT32	BF_Fs_Controller_Format_Drive(FW_INT32 drive_no)
{
	FW_INT32 ercd;
	FW_INT32 cahr_length;
	FW_CHAR  drv_name[9];

	gBF_Fs_Controller_Progress = 0;

	BF_Mdf_Wrapper_End_Task();

	ercd = BF_Mdf_Wrapper_End_Drv((FW_CHAR*)&gNF_Drive_Name[drive_no][0]);
	if ((ercd < 0) && (ercd != D_BF_MDF_WRAPPER_ERR_NODEV) && (ercd != D_BF_MDF_WRAPPER_ERR_NOMDF)) {
		ercd = MWNF_Format_Drive((E_MWNF_DRIVE)drive_no);
		if (ercd != 0) {
			gBF_Fs_Controller_Progress = -1;
			return ercd;
		}
		ercd = BF_Mdf_Wrapper_End_Drv((FW_CHAR*)&gNF_Drive_Name[drive_no][0]);
		if ((ercd < 0) && (ercd != D_BF_MDF_WRAPPER_ERR_NODEV) && (ercd != D_BF_MDF_WRAPPER_ERR_NOMDF)) {
			gBF_Fs_Controller_Progress = -1;
			return ercd;
		}
	}

	ercd = MWNF_Format_Drive((E_MWNF_DRIVE)drive_no);
	if (ercd != 0) {
		gBF_Fs_Controller_Progress = -1;
		return ercd;
	}

	ercd = BF_Mdf_Wrapper_Init_Drv((FW_CHAR*)&gNF_Drive_Name[drive_no][0], (T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWNF_driver, (FW_VOID*)&NFdevInfo[drive_no]);
	if (ercd < 0) {
		gBF_Fs_Controller_Progress = -1;
		return ercd;
	}

	if (gNF_Drive_Protect[drive_no] == E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT) {
		ercd = BF_Mdf_Wrapper_Set_Protection_Mode((FW_CHAR*)gNF_Drive_Name[drive_no], E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT);
	}

	cahr_length = strlen((const FW_CHAR*)gBF_Controller_drv_name[drive_no]);
	strncpy(drv_name, (const CHAR*)gBF_Controller_drv_name[drive_no], cahr_length);
	strcat(drv_name, ":");

	BF_Media_State_Set_Used_Media(gBF_Fs_Controller_Media_Status[drive_no].media_type);
	BF_Mdf_Wrapper_Chdir(drv_name);

	if (ercd == 0) {
		gBF_Fs_Controller_Progress = 100;
	}
	else {
		gBF_Fs_Controller_Progress = -1;
	}

	return ercd;
}

FW_INT32      BF_Fs_Controller_Date_Time_Sync(FW_CHAR* org_file_path, FW_CHAR* sync_file_ext, T_BF_FS_utimbuf fs_utim)
{
	FW_INT32 ercd;
	FW_INT32 index;
	T_BF_FS_utimbufEx utime_ex;

	/* Param check */
	if (org_file_path == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (sync_file_ext == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

#if 1 // for exFAT MDF
	utime_ex.date = fs_utim.date;
	utime_ex.time = fs_utim.time;
	utime_ex.sec10m = 0;
	ercd = BF_Mdf_Wrapper_UtimeEx(org_file_path, FJ_FS_TIMESTAMP_MODIFY | FJ_FS_TIMESTAMP_CREATE, &utime_ex);
	if (ercd != 0) {
		return ercd;
	}
#else 
	ercd = BF_Mdf_Wrapper_EXT_Utime(org_file_path, &fs_utim, (FS_UTIME_EXT_EDIT_DATETIME | FS_UTIME_EXT_MAKE_DATETIME));
	if (ercd != 0) {
		return ercd;
	}
#endif

	for (index = 0; org_file_path[index] != '.'; index++) {
		if (org_file_path[index] == '\0') {
			org_file_path[index] = '.';
			org_file_path[index + 1] = '\0';
		}
	}

	strcpy(&org_file_path[index + 1], sync_file_ext);
#if 1 // for exFAT MDF
	ercd = BF_Mdf_Wrapper_UtimeEx(org_file_path, FJ_FS_TIMESTAMP_MODIFY | FJ_FS_TIMESTAMP_CREATE, &utime_ex);
#else
	ercd = BF_Mdf_Wrapper_EXT_Utime(org_file_path, &fs_utim, (FS_UTIME_EXT_EDIT_DATETIME | FS_UTIME_EXT_MAKE_DATETIME));
#endif

	return ercd;
}

/**
 * @brief Execute to cancel all filesystem request.
 * @param FW_VOID
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Cancel(FW_VOID)
{
	FW_INT32 ercd = 0;
	return ercd;
}

/**
 * @brief set current drive information.
 * @param i:media_type media type
 * @return error code\n
   -1:media is not supported
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Change_Drive_Info(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
#if 0
	switch(media_type){
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD:
			gBF_Fs_Controller_Media_Status.media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_SD;
			strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status.devid, "I");
			strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status.drvname, "I:\\");
			BF_Media_Drv_Wrapper_Get_Drv_Info_SDIO0((T_FS_FUNCNAME**)&gBF_Fs_Controller_Media_Status.driver_tbl, (FW_VOID**)&gBF_Fs_Controller_Media_Status.media_info);
			break;

		case E_BF_MEDIA_STATE_MEDIA_TYPE_NF:
			gBF_Fs_Controller_Media_Status.media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_NF;
			strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status.devid, "J");
			strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status.drvname, "J:\\");
			BF_Media_Drv_Wrapper_Get_Drv_Info_NF((T_FS_FUNCNAME**)&gBF_Fs_Controller_Media_Status.driver_tbl, (FW_VOID**)&gBF_Fs_Controller_Media_Status.media_info);
			break;

		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD1:
			gBF_Fs_Controller_Media_Status.media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_SD1;
			strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status.devid, "I");
			strcpy((FW_CHAR*)gBF_Fs_Controller_Media_Status.drvname, "I:\\");
			BF_Media_Drv_Wrapper_Get_Drv_Info_SDIO1((T_FS_FUNCNAME**)&gBF_Fs_Controller_Media_Status.driver_tbl, (FW_VOID**)&gBF_Fs_Controller_Media_Status.media_info);
			break;

		default:
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("media type is not supported.\n"));
#endif
			return -1;
	}

	BF_Media_State_Set_Used_Media(media_type);
#endif
	return 0;
}

/**
 * @brief Execute writing and moving data after lseek (whence D_BF_MDF_WRAPPER_SEEK_SET).
 * @param i:fNo the file number of target file
 * @param i:offset the offset value (byte) from the standard position
 * @param i:addr the pointer of area which is stored the write data
 * @param i:count the write data size
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Set_Lseek_Write(FW_INT32 fNo, FW_INT32 offset, FW_ULONG addr, FW_ULONG count, FW_ULONG* length)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Lseek(fNo, offset, D_BF_MDF_WRAPPER_SEEK_SET, length);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, fNo=%x, offset=%x, whence=%d\n", g_bf_fs_controller_ret_err, fNo, offset, D_BF_MDF_WRAPPER_SEEK_SET));
#endif
		return g_bf_fs_controller_ret_err;
	}

	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Write(fNo, (const FW_VOID*)addr, count, length);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, fNo=%x, addr=%lu, count=%lu\n", g_bf_fs_controller_ret_err, fNo, addr, count));
#endif
	}
	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Get result of BF_Fs_Controller_Set_Lseek_Write.
 * @param o:*length the pointer of area which is stored the actual written size
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Set_Lseek_Write_Result(FW_ULONG* length)
{
	if(length == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	*length = g_bf_fs_controller_ret_length;
	return g_bf_fs_controller_ret_err;
}

/**
 * @brief This function changes the timestamp of demanded file or directory.
 * @param i:*fname the pointer of array of file name
 * @param i:timestamp_type he kind of the TimeStamp that changes is set
 * @param i:*utime date and time
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Set_Time(FW_CHAR* fname, FW_USHORT timestamp_type, T_BF_FS_IF_TIME* utime)
{
	FW_INT32 ercd;

	if(fname == NULL){
		// null pointer error
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	if(utime == NULL){
		// null pointer error
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	ercd = BF_Mdf_Wrapper_UtimeEx((const FW_CHAR*)fname, timestamp_type, (T_BF_FS_utimbufEx*)utime);
	if( ercd < 0 ){
		// Function processing error
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_UtimeEx() failed: ercd=%d, fname=%s\n", ercd, fname));
#endif
	}

	return ercd;
}

/**
 * @brief 		Get media id
 * @param		i : CHAR*	drvname : drive name
 *				o : FJ_MEDIA_ID* bMediaId : Media ID
 * @note		None
 * @attention 	None
 */
VOID BF_Fs_Controller_Get_Media_Id(CHAR* drvname, CHAR* bMediaId)
{
	UINT32 media_index;

	*bMediaId = 0xFF;
	for(media_index = 0; media_index < (FJ_MEDIA_EMMC_STORAGE6 + 1); media_index++) {
		if(strcmp((const CHAR*)drvname, (const CHAR*)gBF_Controller_drv_name[media_index]) == 0){
			*bMediaId = (FJ_MEDIA_ID)media_index;
			break;
		}
	}
}

/**
 * @brief 		This function sets callback to notice writing error for non shared memory.
 * @param 		i:fNo The ID of the file to set for writing request.
 * @param 		i:pCallback The callback function is used when an writing error is occured for non shared memory.
 * @return		error code
 * @note		None
 * @attention 	None
 */
FW_INT32      BF_Fs_Controller_Set_Vfs_Write_Err_Cb( FW_UINT32 fNo, FS_SET_WRITE_ERR_CB pCallback )
{
	FW_INT32 ercd = 0;

#ifdef CO_LINUX_VERSION_ON
	ercd = BF_Vfs_Set_Write_Err_Cb( fNo, ( VFS_SET_WRITE_ERR_CB )pCallback );
	
	if( ercd == D_BF_FS_VFS_NO_MORE_OPEN ) {
		return D_BF_MDF_WRAPPER_ERR_MOPEN;
	}
#endif
	return ercd;
}

/**
 * @brief This function sets a user setting data to user setting area on nand flash.
 * @param i:addr the pointer of area which is stored the write data
 * @param i:count the write data size
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Set_User_Setting( FW_ULONG addr, FW_ULONG count )
{
	FW_INT32 ercd = 0;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	CHAR	mdr = (*(( unsigned long * )0x1DFF4000) & 0x7);
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	
#ifdef CO_LINUX_VERSION_ON
	FW_UINT32 src_addr;

	/* shared memory or non shared memory ? */
	if( ( Sdram_Map_Is_Shared( (FW_UINT32)addr )) == 0 ) {
		/* non shared memory */
		src_addr = SDRAM_ADR_USER_SET_DATA_WORK;
		memcpy( ( FW_VOID* )src_addr, ( const FW_VOID* )addr, count );
	}
	else {
		src_addr = (FW_UINT32)addr;
	}
	ercd = BF_Vfs_Set_User_Setting( (const FW_VOID*)src_addr, count );
#else

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	// check boot mode
	if (mdr == 1) {
		ercd = BF_Media_Drv_Wrapper_Set_User_Setting_eMMC(addr, count);
	}
	else {
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		ercd = BF_Media_Drv_Wrapper_Set_User_Setting(addr, count);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#endif
	
	return ercd;
}

/**
 * @brief Execute get entry.
 * @param i:fname file name
 * @param i:attr attributes
 * @param i:index manage tabl index
 * @param o:buf entry information
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Fsfirst(FW_ULONG index, const char* fname, FW_USHORT attr, T_BF_FS_IF_ENTRY* buf)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Fsfirst(index, fname, attr, buf);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_Controller_Fsfirst() failed: ercd=%d\n", g_bf_fs_controller_ret_err));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

/**
 * @brief Execute get entry.
 * @param i:index manage tabl index
 * @param o:buf entry information
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_Controller_Fsnext(FW_ULONG index, T_BF_FS_IF_ENTRY* buf)
{
	g_bf_fs_controller_ret_err = BF_Mdf_Wrapper_Fsnext(index, buf);
	if( g_bf_fs_controller_ret_err < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_Controller_Fsnext() failed: ercd=%d\n", g_bf_fs_controller_ret_err));
#endif
	}

	return g_bf_fs_controller_ret_err;
}

