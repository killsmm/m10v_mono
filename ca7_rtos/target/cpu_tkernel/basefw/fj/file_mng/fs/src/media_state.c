/**
 * @file		media_state.c
 * @brief		Media status manager.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include <string.h>
#include "media_state.h"
#include "fsdcf_errno.h"
#include "fj_peripheral.h"
#include "fs_user_custom.h"
#include "dd_arm.h"
#include "fj_filesystem.h"
#include "fs_controller.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_BF_MEDIA_STATE_CHECK_INTERVAL		(0)	/* ms */
#define D_BF_MEDIA_STATE_CHECK_COUNT		(0)

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
static volatile E_BF_MEDIA_STATE_MEDIA_TYPE g_bf_media_state_used_media = E_BF_MEDIA_STATE_MEDIA_TYPE_NOTHING;
static volatile T_BF_MEDIA_STATE_MEDIA_INFO g_bf_media_state_media_info = { E_BF_MEDIA_STATE_MEDIA_WP_UNPROTECT,
																   E_BF_MEDIA_STATE_MEDIA_WP_UNPROTECT,
																   E_BF_MEDIA_STATE_MEDIA_WP_UNPROTECT};

static volatile unsigned char g_bf_media_state_interval[D_BF_MEDIA_SD_MAX] = {D_BF_MEDIA_STATE_CHECK_INTERVAL};
static volatile unsigned char g_bf_media_state_cnt[D_BF_MEDIA_SD_MAX] ={ D_BF_MEDIA_STATE_CHECK_COUNT};
static FJ_MEDIA_ID g_bf_nf_storage_id = FJ_MEDIA_NF_STORAGE0;
static FJ_MEDIA_ID g_bf_emmc_storage_id = FJ_MEDIA_EMMC_STORAGE0;

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
static E_BF_MEDIA_STATE_MEDIA_STATE media_state_get_madia_status(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);

static E_BF_MEDIA_STATE_MEDIA_STATE media_state_get_madia_status(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
	switch (media_type) {
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD:
			if (BF_Fs_User_Custom_Get_Sd0_Insert_Init_End()) {
				return BF_Fs_User_Custom_Get_Sd0_Insert_State();
			}
			else {
				return BF_Fs_User_Custom_Get_Sd0_Insert_State_At_Gpio();
			}

		case E_BF_MEDIA_STATE_MEDIA_TYPE_NF:
			return D_BF_FS_USER_CUSTOM_NF_INSERT_STATE();

		case E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC:
			return D_BF_FS_USER_CUSTOM_EMMC_INSERT_STATE();

		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD1:
			if (BF_Fs_User_Custom_Get_Sd1_Insert_Init_End()) {
				return BF_Fs_User_Custom_Get_Sd1_Insert_State();
			}
			else {
				return BF_Fs_User_Custom_Get_Sd1_Insert_State_At_Gpio();
			}

		default:
			// DO NOTHING
			break;
	}
	return E_BF_MEDIA_STATE_MEDIA_STATE_LEAVE;
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/**
 * @brief set used media.
 * @param i:media_type used media
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_VOID       BF_Media_State_Set_Used_Media(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
#ifdef __GNUC__
	__asm__ volatile ("nop");
#else
	__nop();
#endif
	if (media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_CUR) {
		media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_NOTHING;
	}
	g_bf_media_state_used_media = media_type;
	Dd_ARM_Critical_Section_End( critseq );
#ifdef __GNUC__
	__asm__ volatile ("nop");
#else
	__nop();
#endif
}

/**
 * @brief get used media.
 * @param none
 * @return used media
 * @note none
 * @attention none
 */
E_BF_MEDIA_STATE_MEDIA_TYPE  BF_Media_State_Get_Used_Media(FW_VOID)
{
	return g_bf_media_state_used_media;
}

/**
 * @brief exchange media type to drive name.
 * @param i:media_type media type
 * @param o:drive_name drive name
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Media_State_Exchage_Type_To_Drive_Name(E_BF_MEDIA_STATE_MEDIA_TYPE media_type, FW_CHAR* drive_name)
{

	if (media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1) {
		strcpy(drive_name, (const char *)gBF_Controller_drv_name[FJ_MEDIA_SD1]);
	}
	else if (media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD) {
		strcpy(drive_name, (const char *)gBF_Controller_drv_name[FJ_MEDIA_SD0]);
	}
	else if (media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF) {
		strcpy(drive_name, (const char *)gBF_Controller_drv_name[g_bf_nf_storage_id]);
	}
	else if (media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC) {
		strcpy(drive_name, (const char *)gBF_Controller_drv_name[g_bf_emmc_storage_id]);
	}
	else{
		printf("NOT SUPPORT MEDIA. media_type=%d \n",  media_type);
		return FSDCF_ERRNO_NOT_SUPPORT_MEDIA;
	}
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
   	BF_Fs_User_Custom_Info_Printer(("BF_Media_State_Exchage_Type_To_Drive_Name: input:media_type=%s, get:drive_name=%s \n", (char*)media_type, drive_name ));
#endif
	return FSDCF_ERRNO_OK;

}

/**
 * @brief set media check parameter (for BF_Media_State_Get_Media_State).
 * @param i:ch           sd channel
 * @param i:chk_interval media check interval(ms)
 * @param i:chk_cnt      check count
 * @return none
 * @note none
 * @attention none
 */
FW_VOID       BF_Media_State_Set_Media_Check_Param(unsigned char ch, unsigned char chk_interval, unsigned char chk_cnt)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	g_bf_media_state_interval[ch] = chk_interval;
	g_bf_media_state_cnt[ch] = chk_cnt;
	Dd_ARM_Critical_Section_End( critseq );
}

/**
 * @brief get media status.
 * @param i:ch sd channel
 * @return media status(insert/leave)
 * @note none
 * @attention none
 */
E_BF_MEDIA_STATE_MEDIA_STATE BF_Media_State_Get_Media_State(unsigned char ch)
{
	return BF_Media_State_Get_Media_State_By_Poll(ch, g_bf_media_state_interval[ch], g_bf_media_state_cnt[ch]);
}

/**
 * @brief get media status.
 * @param i:ch         sd channel
 *        i:interval   interval time
 *        i:count      check count
 * @return media status(insert/leave)
 * @note none
 * @attention none
 */
E_BF_MEDIA_STATE_MEDIA_STATE BF_Media_State_Get_Media_State_By_Poll(unsigned char ch, FW_UCHAR interval, FW_UCHAR count)
{
	E_BF_MEDIA_STATE_MEDIA_TYPE media_type;
	E_BF_MEDIA_STATE_MEDIA_STATE previous_state, now_state;
	UCHAR check_cnt;
	UINT32 check_time, timeout;

	if (interval != 0) {
		timeout = interval * count * 10;
	}
	else {
		timeout = count * 10;
	}

	if (ch == 0) {
		media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_SD;
	}
	else {
		media_type = E_BF_MEDIA_STATE_MEDIA_TYPE_SD1;
	}

	now_state = previous_state = media_state_get_madia_status(media_type);				// get media state at first

	// count 回 同じStatus が読めるかCheckする
	for (check_cnt = 0, check_time = 0;
		 (check_time < timeout) && (check_cnt < count);
		 check_cnt++, check_time += interval) {

		if (interval != 0) {
			BF_Fs_User_Custom_Dly_Tsk(interval);										// interval(ms) 分 delay
		}
		else {
			check_time++;
		}

		now_state = media_state_get_madia_status(media_type);							// delay 後の media state 取得
		if (previous_state != now_state) {
			// media state 変化有り
			previous_state = now_state;
			check_cnt = 0;												// counter clear
		}
	}

	if ((timeout == 0) || (check_time < timeout)) {
		return now_state;												// Return Media State (Insert or Leave)
	}
	else {
		return E_BF_MEDIA_STATE_MEDIA_STATE_NONE;										// Return Error
	}
}

/**
 * @brief polling media status.
 * @param i:media_type media type
 * @return media status(insert/leave)
 * @note none
 * @attention none
 */
E_BF_MEDIA_STATE_MEDIA_STATE BF_Media_State_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
	return media_state_get_madia_status(media_type);
}

/**
 * @brief set write protection.
 * @param i:media_type media type
 * @param i:media_wp write protection status
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_VOID       BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE media_type, E_BF_MEDIA_STATE_MEDIA_WP media_wp)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
#ifdef __GNUC__
	__asm__ volatile ("nop");
#else
	__nop();
#endif
	switch (media_type) {
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD: // FALL THROUGH
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD1:
			g_bf_media_state_media_info.sd_wp = media_wp;
			break;

		case E_BF_MEDIA_STATE_MEDIA_TYPE_NF:
			g_bf_media_state_media_info.nf_wp = media_wp;
			break;

		case E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC:
			g_bf_media_state_media_info.emmc_wp = media_wp;
			break;

		default:
			// DO NOTHING
			break;
	}
	Dd_ARM_Critical_Section_End( critseq );
#ifdef __GNUC__
	__asm__ volatile ("nop");
#else
	__nop();
#endif
}

/**
 * @brief get write protection.
 * @param i:media_type media type
 * @return write protection status
 * @note none
 * @attention none
 */
E_BF_MEDIA_STATE_MEDIA_WP    BF_Media_State_Get_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
	switch (media_type) {
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD: // FALL THROUGH
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD1:
			return g_bf_media_state_media_info.sd_wp;

		case E_BF_MEDIA_STATE_MEDIA_TYPE_NF:
			return g_bf_media_state_media_info.nf_wp;

		case E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC:
			return g_bf_media_state_media_info.emmc_wp;
			
		default:
			// DO NOTHING
			break;
	}
	return E_BF_MEDIA_STATE_MEDIA_WP_PROTECT;
}

FW_VOID BF_Media_Set_NF_Storage_ID(FW_UCHAR id)
{
	g_bf_nf_storage_id = id;
}

FW_VOID BF_Media_Set_EMMC_Storage_ID(FW_UCHAR id)
{
	g_bf_emmc_storage_id = id;
}

FW_UCHAR BF_Media_Get_NF_Storage_ID(FW_VOID)
{
	return g_bf_nf_storage_id;
}

FW_UCHAR BF_Media_Get_EMMC_Storage_ID(FW_VOID)
{
	return g_bf_emmc_storage_id;
}

