/**
 * @file		mdf_wrapper.c
 * @brief		Wrapper of MDF
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
/******************************************************************************
 for MDF
******************************************************************************/
#include <string.h>
///#include "os_user_custom.h"
#include "FS_SystemDependence.h"

#include "fs_user_custom.h"
#include "fs_controller.h"
#include "mdf_wrapper.h"
#include "vfs.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
//#define CO_BF_MDF_WRAPPER_DEBUG_FID_COUNT
//#define CO_BF_MDF_WRAPPER_DEBUG_SPACE
#ifndef CO_MEDIA_DRIVER_DEBUG_READ_WRITE_TIME
//#define CO_BF_MDF_WRAPPER_DEBUG_READ
//#define CO_BF_MDF_WRAPPER_DEBUG_WRITE
#endif
#define CO_BF_MDF_WRAPPER_DEBUG

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
#ifdef CO_BF_MDF_WRAPPER_DEBUG_FID_COUNT // ifdef_01
static volatile FW_ULONG g_bf_mdf_wrapper_fid_counter = 0;
#define mdf_wrapper_fid_count_init() (g_bf_mdf_wrapper_fid_counter = 0)
#define mdf_wrapper_fid_count_fs_open() (++g_bf_mdf_wrapper_fid_counter)
#define mdf_wrapper_fid_count_fs_close() do { \
	--g_bf_mdf_wrapper_fid_counter;	\
	BF_Fs_User_Custom_Info_Printer(("### Num of FID = %d ###\n", g_bf_mdf_wrapper_fid_counter)); \
} while(0)
#else                  // ifdef_01
#define mdf_wrapper_fid_count_init()
#define mdf_wrapper_fid_count_fs_open()
#define mdf_wrapper_fid_count_fs_close()
#endif                 // ifdef_01

#define mdf_wrapper_lock_media_space_cache()	(BF_Fs_User_Custom_Dis_Dsp())
#define mdf_wrapper_unlock_media_space_cache()	(BF_Fs_User_Custom_Ena_Dsp())
#define mdf_wrapper_init_sd_space_cache()		(g_bf_mdf_wrapper_sd_free_space_cache = 0)
#define mdf_wrapper_init_nf_space_cache()		(g_bf_mdf_wrapper_nf_free_space_cache = 0)
#define mdf_wrapper_init_a_space_cache()		(g_bf_mdf_wrapper_a_free_space_cache = 0)
#define mdf_wrapper_init_b_space_cache()		(g_bf_mdf_wrapper_b_free_space_cache = 0)
#define mdf_wrapper_init_c_space_cache()		(g_bf_mdf_wrapper_c_free_space_cache = 0)
#define mdf_wrapper_init_d_space_cache()		(g_bf_mdf_wrapper_d_free_space_cache = 0)
#define mdf_wrapper_init_e_space_cache()		(g_bf_mdf_wrapper_e_free_space_cache = 0)
#define mdf_wrapper_init_f_space_cache()		(g_bf_mdf_wrapper_f_free_space_cache = 0)
#define mdf_wrapper_init_j_space_cache()		(g_bf_mdf_wrapper_j_free_space_cache = 0)
#define mdf_wrapper_init_k_space_cache()		(g_bf_mdf_wrapper_k_free_space_cache = 0)
#define mdf_wrapper_init_l_space_cache()		(g_bf_mdf_wrapper_l_free_space_cache = 0)
#define mdf_wrapper_init_m_space_cache()		(g_bf_mdf_wrapper_m_free_space_cache = 0)
#define mdf_wrapper_init_n_space_cache()		(g_bf_mdf_wrapper_n_free_space_cache = 0)
#define mdf_wrapper_init_o_space_cache()		(g_bf_mdf_wrapper_o_free_space_cache = 0)
#define mdf_wrapper_init_p_space_cache()		(g_bf_mdf_wrapper_p_free_space_cache = 0)

#ifdef CO_BF_MDF_WRAPPER_DEBUG_SPACE
#define mdf_wrapper_free_space_printer() do { \
	BF_Fs_User_Custom_Info_Printer(("FREE SPACE: SD %lf byte , NF %lf byte \n", g_bf_mdf_wrapper_sd_free_space_cache, g_bf_mdf_wrapper_nf_free_space_cache));	\
} while(0)
#else
#define mdf_wrapper_free_space_printer()
#endif

#define mdf_wrapper_update_sd_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_sd_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_nf_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_nf_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)


#define mdf_wrapper_update_a_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_a_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_b_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_b_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_c_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_c_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_d_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_d_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_e_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_e_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_f_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_f_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_j_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_j_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_k_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_k_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_l_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_l_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_m_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_m_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_n_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_n_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_o_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_o_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_p_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_p_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_sd_space_cache(write_byte) do { \
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_sd_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_sd_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)


#define mdf_wrapper_write_update_nf_space_cache(write_byte) do { \
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_nf_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_nf_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)


#define mdf_wrapper_write_update_a_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_a_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_a_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_b_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_b_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_b_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_c_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_c_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_c_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_d_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_d_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_d_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_e_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_e_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_e_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_f_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_f_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_f_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_j_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_j_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_j_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_k_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_k_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_k_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_l_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_l_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_l_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_m_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_m_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_m_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_n_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_n_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_n_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_o_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_o_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_o_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_p_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_p_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_p_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
#ifndef CO_LINUX_VERSION_ON
static FW_VOID   mdf_wrapper_init_media_space_cache(const char* drvname);
static FS_INT mdf_wrapper_cache_media_space(const char* drvname);
static FS_INT mdf_wrapper_cache_media_space_curdrv(FW_VOID);
static FW_DOUBLE mdf_wrapper_get_media_space(const char* drvname);
static FW_VOID   mdf_wrapper_set_media_space(const char* drvname, FW_DOUBLE space);
#endif // CO_LINUX_VERSION_ON
/*******************************************************************************
 Media space cache
*******************************************************************************/
#ifndef CO_LINUX_VERSION_ON
static FW_DOUBLE g_bf_mdf_wrapper_sd_free_space_cache = 0;
static FW_DOUBLE g_bf_mdf_wrapper_nf_free_space_cache = 0;
static FW_DOUBLE g_bf_mdf_wrapper_a_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_b_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_c_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_d_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_e_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_f_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_j_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_k_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_l_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_m_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_n_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_o_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_p_free_space_cache  = 0;
#endif // CO_LINUX_VERSION_ON

#ifndef CO_LINUX_VERSION_ON
/**
 * init media space cache
 */
static VOID   mdf_wrapper_init_media_space_cache(const char* drvname)
{
	UCHAR media_id;

	BF_Fs_Controller_Get_Media_Id((char*)drvname, (CHAR*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_init_media_space_cache(): abnormal drvname=%s\n", drvname));
		return;
	}
	
	if(media_id == D_BF_FS_CONTROLLER_NF_0) {
		mdf_wrapper_init_nf_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_SD_0 || media_id == D_BF_FS_CONTROLLER_SD_1) {
		mdf_wrapper_init_sd_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_1) {
		mdf_wrapper_init_a_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_2) {
		mdf_wrapper_init_b_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_3) {
		mdf_wrapper_init_c_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_4) {
		mdf_wrapper_init_d_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_5) {
		mdf_wrapper_init_e_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_6) {
		mdf_wrapper_init_f_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_0) {
		mdf_wrapper_init_j_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_1) {
		mdf_wrapper_init_k_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_2) {
		mdf_wrapper_init_l_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_3) {
		mdf_wrapper_init_m_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_4) {
		mdf_wrapper_init_n_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_5) {
		mdf_wrapper_init_o_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_6) {
		mdf_wrapper_init_p_space_cache();
	}
#if 1	/* for Dir Copy  */
	else if (strcmp(drvname, D_BF_FS_IF_DST_DRIVE_NAME_STRING) == 0) {
		;
	}
#endif
	else {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_init_media_space_cache(): abnormal drvname=%s, media_type=%d \n", drvname, gBF_Fs_Controller_Media_Status[media_id].media_type));
	}

	mdf_wrapper_free_space_printer();
}

/**
 * cache media space from MDF
 */
static FS_INT mdf_wrapper_cache_media_space(const char* drvname)
{
	FS_DWORD blksize, blocks, bfree;
	FS_INT   ercd;
	UCHAR media_id;

	if ((ercd = FS_getFreeSpace(drvname, &blksize, &blocks, &bfree)) != 0) {
		BF_Fs_User_Custom_Err_Printer(("FS_getFreeSpace(): drvname=%s, ercd=%x\n", drvname, ercd));
		return -1;
	}

#if 0	// for debug
	printf("getFreeSpace blksize : %d, blocks : %d, bfree : %d\n", blksize, blocks, bfree);
#endif
	//--- Dealing with media shortage problem
	if (bfree < 1) {
		bfree = 0;
	}
	else {
		bfree -= 1;
	}
	//---

	BF_Fs_Controller_Get_Media_Id((CHAR *)drvname, (CHAR*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("space cache update is abnormal. drvname=%s\n", drvname));
		return -1;
	}

	if (media_id == D_BF_FS_CONTROLLER_NF_0) {
		if (gBF_Fs_Controller_Media_Status[media_id].media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF) {
			mdf_wrapper_update_nf_space_cache(blksize, bfree);
			return 0;
		}
		else {
			BF_Fs_User_Custom_Wrn_Printer(("mdf_wrapper_cache_media_space(): drvname=%s, media_type=%d \n", drvname, gBF_Fs_Controller_Media_Status[media_id].media_type));
		}
	}
	else if (media_id == D_BF_FS_CONTROLLER_SD_0 || media_id == D_BF_FS_CONTROLLER_SD_1) {
		if (( gBF_Fs_Controller_Media_Status[media_id].media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD) ||
			( gBF_Fs_Controller_Media_Status[media_id].media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1 )) {
			mdf_wrapper_update_sd_space_cache(blksize, bfree);
			return 0;
		}
		else{
			BF_Fs_User_Custom_Wrn_Printer(("mdf_wrapper_cache_media_space(): drvname=%s, media_type=%d \n", drvname, gBF_Fs_Controller_Media_Status[media_id].media_type));
		}
	}
	else if (media_id == D_BF_FS_CONTROLLER_NF_1) {
		mdf_wrapper_update_a_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_NF_2) {
		mdf_wrapper_update_b_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_NF_3) {
		mdf_wrapper_update_c_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_NF_4) {
		mdf_wrapper_update_d_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_NF_5) {
		mdf_wrapper_update_e_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_NF_6) {
		mdf_wrapper_update_f_space_cache(blksize, bfree);
		return 0;
	}
#if 1	/* for Dir Copy  */
	else if (strcmp(drvname, D_BF_FS_IF_DST_DRIVE_NAME_STRING) == 0) {
		return 0;
	}
#endif
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_0) {
		mdf_wrapper_update_j_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_1) {
		mdf_wrapper_update_k_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_2) {
		mdf_wrapper_update_l_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_3) {
		mdf_wrapper_update_m_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_4) {
		mdf_wrapper_update_n_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_5) {
		mdf_wrapper_update_o_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_6) {
		mdf_wrapper_update_p_space_cache(blksize, bfree);
		return 0;
	}
	else {
		; // DO NOTHING
	}

	BF_Fs_User_Custom_Err_Printer(("space cache update is abnormal. drvname=%s\n", drvname));
	return -1;
}

/**
 * cache media space by current drive
 */
static FS_INT mdf_wrapper_cache_media_space_curdrv(FW_VOID)
{
	FS_INT ercd;
	char   drvname[12];

	if ((ercd = FS_getCurDrv(drvname)) != 0) {
		BF_Fs_User_Custom_Err_Printer(("FS_getCurDrv(): ercd=%x\n", ercd));
		return -1;
	}
	return mdf_wrapper_cache_media_space(drvname);
}

/**
 * get media space
 */
static DOUBLE mdf_wrapper_get_media_space(const char* drvname)
{
	DOUBLE space = 0;
	UCHAR media_id;

	BF_Fs_Controller_Get_Media_Id((char*)drvname, (CHAR*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_get_media_space(): abnormal drvname=%s\n", drvname));
		return space;
	}

	mdf_wrapper_lock_media_space_cache();

	if(media_id == D_BF_FS_CONTROLLER_NF_0) {
		space = g_bf_mdf_wrapper_nf_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_SD_0 || media_id == D_BF_FS_CONTROLLER_SD_1) {
		space = g_bf_mdf_wrapper_sd_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_1) {
		space = g_bf_mdf_wrapper_a_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_2) {
		space = g_bf_mdf_wrapper_b_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_3) {
		space = g_bf_mdf_wrapper_c_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_4) {
		space = g_bf_mdf_wrapper_d_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_5) {
		space = g_bf_mdf_wrapper_e_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_6) {
		space = g_bf_mdf_wrapper_f_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_0) {
		space = g_bf_mdf_wrapper_j_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_1) {
		space = g_bf_mdf_wrapper_k_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_2) {
		space = g_bf_mdf_wrapper_l_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_3) {
		space = g_bf_mdf_wrapper_m_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_4) {
		space = g_bf_mdf_wrapper_n_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_5) {
		space = g_bf_mdf_wrapper_o_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_6) {
		space = g_bf_mdf_wrapper_p_free_space_cache;
	}
	else {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_get_media_space(): abnormal drvname=%s\n", drvname));
	}
	mdf_wrapper_unlock_media_space_cache();
	return space;
}

/**
 * set media space
 */
static VOID   mdf_wrapper_set_media_space(const char* drvname, FW_DOUBLE space)
{
	UCHAR media_id;

	BF_Fs_Controller_Get_Media_Id((char*)drvname, (CHAR*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_set_media_space(): abnormal drvname=%s\n", drvname));
		return;
	}

	mdf_wrapper_lock_media_space_cache();

	if(media_id == D_BF_FS_CONTROLLER_NF_0) {
		g_bf_mdf_wrapper_nf_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_SD_0 || media_id == D_BF_FS_CONTROLLER_SD_1) {
		g_bf_mdf_wrapper_sd_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_1) {
		g_bf_mdf_wrapper_a_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_2) {
		g_bf_mdf_wrapper_b_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_3) {
		g_bf_mdf_wrapper_c_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_4) {
		g_bf_mdf_wrapper_d_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_5) {
		g_bf_mdf_wrapper_e_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_6) {
		g_bf_mdf_wrapper_f_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_0) {
		g_bf_mdf_wrapper_j_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_1) {
		g_bf_mdf_wrapper_k_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_2) {
		g_bf_mdf_wrapper_l_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_3) {
		g_bf_mdf_wrapper_m_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_4) {
		g_bf_mdf_wrapper_n_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_5) {
		g_bf_mdf_wrapper_o_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_6) {
		g_bf_mdf_wrapper_p_free_space_cache = space;
	}
	else {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_set_media_space(): abnormal drvname=%s\n", drvname));
	}
	mdf_wrapper_unlock_media_space_cache();
}
#endif // CO_LINUX_VERSION_ON

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/*******************************************************************************
 MDF(FS library) wrappers
*******************************************************************************/
FS_INT BF_Mdf_Wrapper_Start_Mdf(FS_VOID)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Start_MDF();
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_startMDF();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}


FS_INT BF_Mdf_Wrapper_End_Task(FS_VOID)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
	ercd= 0;
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_endTask();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Init_Drv(char* drvname, T_FS_FUNCNAME* pDrvinfo, FS_VOID* pDevinfo)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	UCHAR media_id;
	
	BF_Fs_Controller_Get_Media_Id((CHAR *)drvname, (CHAR*)&media_id);
	ercd = BF_Vfs_Init_Drv( (FW_CHAR*)drvname, (FW_INT32)media_id );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_initDrv(drvname, (FS_FUNCNAME*)pDrvinfo, pDevinfo)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	mdf_wrapper_cache_media_space(drvname);
	mdf_wrapper_fid_count_init();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_End_Drv(char* drvname)
{
	FS_INT ercd = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;
#else
	ercd = BF_Vfs_End_Drv( (FW_CHAR*)drvname );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	mdf_wrapper_init_media_space_cache(drvname);
	ercd = FS_endDrv(drvname);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Qformat(char* drvname)
{
	FS_INT ercd = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	UCHAR media_id;

	BF_Fs_Controller_Get_Media_Id((char*)drvname, (CHAR*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_set_media_space(): abnormal drvname=%s\n", drvname));
		return 0;
	}
	ercd = BF_Vfs_Qformat((FS_INT)media_id);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_Qformat(drvname)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}
	mdf_wrapper_cache_media_space(drvname);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON


	return ercd;
}

FS_INT BF_Mdf_Wrapper_Nformat(T_FS_FUNCNAME *pDrvinfo, FS_VOID *pDevinfo)
{
	FS_INT ercd = 0;
	
#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_Nformat((FS_FUNCNAME*)pDrvinfo, pDevinfo)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}
	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);

#endif // CO_LINUX_VERSION_ON

	return ercd;

}

FS_INT BF_Mdf_Wrapper_Statvfs(const char* drvname, T_BF_FS_IF_STATVFS* buf)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	memset((void*)buf, 0, sizeof(T_BF_FS_IF_STATVFS));
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Stat_VFS( (FW_CHAR*)drvname, buf );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_statvfs(drvname, (FS_STATVFS*)buf);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Get_Free_Space(const char* drvname, FS_DWORD* blksize, FS_DWORD* blocks, FS_DWORD* bfree)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*blksize = 0;
	*blocks = 0;
	*bfree = 0;
	ercd = 0;	// Temporary
#else
	T_BF_FS_IF_STATVFS stat_buf;
	
	ercd = BF_Vfs_Stat_VFS( (FW_CHAR*)drvname, &stat_buf );
	if( ercd != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}
	
	*blksize = stat_buf.blksize;
	*blocks = stat_buf.blocks;
	*bfree = stat_buf.bfree;
	
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_getFreeSpace(drvname, blksize, blocks, bfree)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	//--- Dealing with media shortage problem
	if (*bfree < 1) {
		*bfree = 0;
	}
	else {
		*bfree -= 1;
	}
	//---

	mdf_wrapper_set_media_space(drvname, ((FW_DOUBLE)(*blksize) * (FW_DOUBLE)(*bfree)));
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

DOUBLE BF_Mdf_Wrapper_Get_Free_Space_By_Cache(const char* drvname)
{
#ifdef CO_LINUX_VERSION_ON
	return 0;
#else
	return mdf_wrapper_get_media_space(drvname);
#endif // CO_LINUX_VERSION_ON
}

FS_INT BF_Mdf_Wrapper_Get_Cur_Drv(char* drvname)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	drvname[0] = '\0';
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Get_Cur_Drv( (FW_CHAR*)drvname );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_getCurDrv(drvname);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Set_Vol_Label(const char* drvname, const char* label)
{
	FS_INT ercd;
#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Set_Vol_Label( (FW_CHAR*)drvname, (FW_CHAR*)label );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_setVolLabel(drvname, label);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Get_Vol_Label(const char* drvname, char* label)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	label[0] = '\0';
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Get_Vol_Label( (FW_CHAR*)drvname, (FW_CHAR*)label );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_getVolLabel(drvname, label);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Fsync(FS_INT fNo)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Fsync( fNo );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_fsync(fNo)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Sync(const char* drvname)
{
	FS_INT ercd;
	
#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Sync( (FW_CHAR*)drvname );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_sync(drvname)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	mdf_wrapper_cache_media_space(drvname);

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);

#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Set_Con_Sec_Max(const char* drvname, FS_DWORD secmax)
{
	FS_INT ercd = 0;

#ifdef CO_LINUX_VERSION_ON
#if 1	// Under implementation of VFS
	ercd = 0;	// Temporary
#endif
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_setConSecMax(drvname, secmax);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Set_Protection_Mode(const char* drvname, FS_DWORD mode)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Set_Protection_Mode( (FW_CHAR*)drvname, (FW_UINT32)mode );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_setProtectionMode(drvname, mode);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Rename(const char* srcname, const char* dstname)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Rename( (FW_CHAR*)srcname, (FW_CHAR*)dstname );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_rename(srcname, dstname)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Stat(const char* fname, T_BF_FS_IF_STAT* sbuf)
{
	FS_INT ercd;
	
#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	memset((void*)sbuf, 0, sizeof(T_BF_FS_IF_STAT));
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Stat( (FW_CHAR*)fname, sbuf );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_stat(fname, (FS_STAT*)sbuf);

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Set_Attribute(const char* fname, FS_BYTE attributes)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Set_Attribute( (FW_CHAR*)fname, (FW_INT32)attributes );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_setAttribute(fname, attributes);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Set_Attribute_Multi(const char* dirname, FS_BYTE attributes, FS_BYTE attr_mode, char filename_list[][13], FS_INT fnum, MULTI_SET_ATTR_CB pcallback)
{
	FS_INT ercd = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	FS_INT file_cnt;
	FS_UINT dname_len;

	FS_CHAR fname[D_BF_FS_VFS_FNAME_MAX] = {0};

//	ercd = BF_Vfs_Set_Attribute_Multi( (FW_CHAR *(*)[13])filename_list, (FS_BYTE*)attributes, (FS_BYTE*)attr_mode, (FS_INT*)fnum );
	
	dname_len = strlen(dirname);
	
	for( file_cnt = 0; file_cnt < fnum; file_cnt++ ) {
		strncpy( ( char* )fname, dirname, dname_len);
		strcat( ( char* )fname, "\\" );
		strcat( ( char* )fname, filename_list[file_cnt] );
		
		ercd = BF_Vfs_Set_Attribute( (FW_CHAR*)fname, (FW_INT32)attributes );
		
		if( pcallback ) {
			( pcallback )( file_cnt, ercd );
		}
		
		memset( fname, 0, D_BF_FS_VFS_FNAME_MAX );
	}
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_setAttributeMulti(dirname, attributes, attr_mode, NULL, filename_list, fnum, pcallback);

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);

#endif // CO_LINUX_VERSION_ON

	return ercd;
}

#if 0	// MDF custom
FS_INT BF_Mdf_Wrapper_Set_Attribute_For_Multi_Set(const char* fname, FS_BYTE attributes)
{
	FS_INT ercd;

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_setAttribute_for_multi_set(fname, attributes);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Set_Attribute_Current_Entry(FS_INT dNo, T_BF_FS_IF_Dir_Info* fsentry, FS_BYTE attributes)
{
	FS_INT ercd;

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_setAttribute_current_entry(dNo, (FS_dirent*)fsentry, attributes);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);

	return ercd;
}
#endif

FS_INT BF_Mdf_Wrapper_Get_Attribute(const char* fname, FS_BYTE* attributes)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*attributes = 0;
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Get_Attribute( (FW_CHAR*)fname, (FS_BYTE*)attributes );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_getAttribute(fname, attributes);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Utime(const char* fname, const T_BF_FS_utimbuf* times)
{
	FS_INT ercd;
	
#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	memset((void*)times, 0, sizeof(FS_utimbuf));
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Utime( (FW_CHAR*)fname, (FS_utimbuf*)times );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_utime(fname, (FS_utimbuf*)times);

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_UtimeEx(const char* fname, FS_DWORD utime_mode, const T_BF_FS_utimbufEx* times)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	memset((void*)times, 0, sizeof(FS_utimbufEx));
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Utime_Ex( (FW_CHAR*)fname, (FW_UINT32)utime_mode, (FS_utimbufEx*)times );
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_utimeEx(fname, utime_mode, (FS_utimbufEx*)times);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Open(const char* fname, FS_WORD mode, FS_WORD amode)
{
	FS_INT fid;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	fid = 0x12345678;
#else
	fid = BF_Vfs_Open( fname, mode, amode );

#endif	// Under implementation of VFS
#else
	fid = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (fid != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((fid = FS_open(fname, mode, amode)) < 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		BF_Fs_User_Custom_Err_Printer(("FS_open << NG >>  fname=%s, fid=%d \n", fname, fid));
		return fid;
	}

	mdf_wrapper_fid_count_fs_open();
	BF_Fs_User_Custom_Info_Printer(("FS_open << OK >>  fNo=%x, fname=%s\n", fid, fname));
#ifdef CO_BF_MDF_WRAPPER_DEBUG_FID_COUNT
	if ( strncmp(fname, "A",1)==0){
//		BF_Fs_User_Custom_Info_Printer(("A open fNo=%x, fname=%s\n", fid, fname));
	}else if (strncmp(fname, "I",1)==0){
		BF_Fs_User_Custom_Info_Printer(("I open fNo=%x, fname=%s\n", fid, fname));
	}else if (strncmp(fname, "J",1)==0){
		BF_Fs_User_Custom_Info_Printer(("J open fNo=%x, fname=%s\n", fid, fname));
	}else{
		BF_Fs_User_Custom_Info_Printer(("open else  fNo=%x, fname=%s\n", fid, fname));
	}
#endif
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif	/* CO_LINUX_VERSION_ON */

	return fid;
}

FS_INT BF_Mdf_Wrapper_Close(FS_INT fNo)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	if ((ercd = BF_Vfs_Close(fNo)) != 0) {
		return ercd;
	}
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_close(fNo)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}
	mdf_wrapper_fid_count_fs_close();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

#ifdef CO_BF_MDF_WRAPPER_DEBUG_FID_COUNT
	BF_Fs_User_Custom_Info_Printer(("close fNo=%x\n", fNo));
#endif

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Read(FS_INT fNo, FS_VOID* cbuf, FS_DWORD count, FS_DWORD* length)
{
	FS_INT ercd;

	*length = 0;
#ifdef CO_BF_MDF_WRAPPER_DEBUG_READ
	FW_ULONG ltime=0;
	FW_ULLONG sys_time_before, sys_time_after;
	BF_Fs_User_Custom_Get_Tim(&sys_time_before);
#endif

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	ercd = 0;	// Temporary
#else
	if ((ercd = BF_Vfs_Read(fNo, cbuf, count, length)) != 0) {
		return ercd;
	}
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_read(fNo, cbuf, count, length)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

#ifdef CO_BF_MDF_WRAPPER_DEBUG_READ
	BF_Fs_User_Custom_Get_Tim(&sys_time_after);
	ltime = (FW_ULONG)(sys_time_after - sys_time_before);
	BF_Fs_User_Custom_Info_Printer(("FS  Read Size=%u(byte), time=%u(ms), speed=%.3f(MB/s)\n", 
	(FW_UINT32)*length, (FW_UINT32)ltime, ((float)(*length) * 1000) / (ltime * 1024.0 * 1024.0)));
#endif

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Write(FS_INT fNo, const FS_VOID* cbuf, FS_DWORD count, FS_DWORD* length)
{
	FS_INT ercd;

#ifdef CO_BF_MDF_WRAPPER_DEBUG_WRITE
	FW_ULONG ltime=0;
	FW_ULLONG sys_time_before, sys_time_after;
	BF_Fs_User_Custom_Get_Tim(&sys_time_before);
#endif

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	ercd = 0;	// Temporary
#else
	if ((ercd = BF_Vfs_Write(fNo, cbuf, count, length)) != 0) {
		return ercd;
	}
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_write(fNo, cbuf, count, length)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	if(*length < count) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return FS_ERR_NOSPC;
	}
#endif // CO_LINUX_VERSION_ON

#ifndef CO_LINUX_VERSION_ON
	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

#ifdef CO_BF_MDF_WRAPPER_DEBUG_WRITE
	BF_Fs_User_Custom_Get_Tim(&sys_time_after);
	ltime = (FW_ULONG)(sys_time_after - sys_time_before);
	BF_Fs_User_Custom_Info_Printer(("FS  Write Size=%u(byte), time=%u(ms), speed=%.3f(MB/s)\n", 
	      (FW_UINT32)*length, (FW_UINT32)ltime, ((float)(*length) * 1000) / (ltime * 1024.0 * 1024.0)));
#endif

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Create(const char* fname, FS_WORD amode)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Create((char*)fname, amode);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_create(fname, amode)) <= 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		BF_Fs_User_Custom_Err_Printer(("FS_create << NG >>  fname=%s, ercd=%d \n", fname, ercd));
		return ercd;
	}

	mdf_wrapper_cache_media_space_curdrv();
	mdf_wrapper_fid_count_fs_open();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
	BF_Fs_User_Custom_Info_Printer(("FS_create << OK >>  fNo=%x, fname=%s\n", ercd, fname));
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Copy(const char* srcname, const char* dstname)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Copy((char*)srcname, (char*)dstname);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_copy(srcname, dstname)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Truncate(const char* fname, FS_DWORD size, FS_DWORD* length)
{
	FS_INT ercd = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	ercd = 0;	// Temporary
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_truncate(fname, size, length)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Truncate64(const char* fname, FS_QWORD size, FS_QWORD* length)
{
	FS_INT ercd = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	ercd = 0;	// Temporary
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_truncate64(fname, size, length)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Ftruncate(FS_INT fNo, FS_DWORD size, FS_DWORD* length)
{
	FS_INT ercd = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	ercd = 0;	// Temporary
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_ftruncate(fNo, size, length)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Ftruncate64(FS_INT fNo, FS_QWORD size, FS_QWORD* length)
{
	FS_INT ercd = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	ercd = 0;	// Temporary
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_ftruncate64(fNo, size, length)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Remove(const char* fname)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Remove((char*)fname);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_remove(fname)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Remove_Multi(const char *dirname, char filename_list[][13], FS_INT fnum, MULTI_DEL_CB pcallback)
{
	FS_INT ercd = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	FS_CHAR fname[D_BF_FS_VFS_FNAME_MAX] = {0};
	FS_INT file_cnt;
	FS_UINT dname_len;
	
	dname_len = strlen(dirname);

	for( file_cnt = 0; file_cnt < fnum; file_cnt++ ) {
		strncpy( ( char* )fname, dirname, dname_len);
		strcat( ( char* )fname, "\\" );
		strcat( ( char* )fname, filename_list[file_cnt] );
		
		ercd = BF_Vfs_Remove( (FW_CHAR*)fname );
		
		if( pcallback ) {
			( pcallback )( file_cnt, ercd );
		}
		
		memset( fname, 0, D_BF_FS_VFS_FNAME_MAX );
	}
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_removeMulti(dirname, NULL, filename_list, fnum, pcallback)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(const char* fname)
{
	FS_INT ercd = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_remove(fname)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		BF_Fs_User_Custom_Err_Printer(("FS_remove << NG >>  fname=%s, ercd=%d\n",  fname, ercd));
		return ercd;
	}

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
	BF_Fs_User_Custom_Info_Printer(("FS_remove << OK >>  fname=%s\n",  fname));
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

#if 0	// MDF custom
FS_INT BF_Mdf_Wrapper_Remove_For_Multi_Delete(const char* fname)
{
	FS_INT ercd;

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_remove_for_multi_delete(fname)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Remove_Current_Entry(FS_INT dNo, T_BF_FS_IF_Dir_Info* fsdirent)
{
	FS_INT ercd;

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_remove_current_entry(dNo, (FS_dirent*)fsdirent);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);

	return ercd;
}
#endif

FS_INT BF_Mdf_Wrapper_Fstat(FS_INT fNo, T_BF_FS_IF_STAT* sbuf)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	memset((void*)sbuf, 0, sizeof(T_BF_FS_IF_STAT));
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Fstat(fNo, sbuf);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_fstat(fNo, (FS_STAT*)sbuf);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Mkdir(const char* dirname, FS_WORD mode)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Mkdir((char*)dirname);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_mkdir(dirname, mode)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		BF_Fs_User_Custom_Err_Printer(("FS_mkdir << NG >>  dir name=%s, ercd=%d \n", dirname, ercd));
		return ercd;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
	BF_Fs_User_Custom_Info_Printer(("FS_mkdir << OK >>  dir name=%s\n",  dirname));
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Rmdir(const char* dirname)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Rmdir((char*)dirname);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	if ((ercd = FS_rmdir(dirname)) != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Opendir(const char* dirname, FS_INT* dNo)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*dNo = 0;
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Opendir((char*)dirname, dNo);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_opendir(dirname, dNo);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#ifdef CO_BF_MDF_WRAPPER_DEBUG
	if (ercd != 0) {
		BF_Fs_User_Custom_Info_Printer(("FS_opendir << ercd=%d >>  dir name=%s\n", ercd, dirname));
	}
#endif //CO_BF_MDF_WRAPPER_DEBUG
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Readdir(FS_INT dNo, T_BF_FS_IF_Dir_Info* fsdirent)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	memset((void*)fsdirent, 0, sizeof(T_BF_FS_IF_Dir_Info));
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Readdir(dNo, fsdirent);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_readdir(dNo, (FS_dirent*)fsdirent);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Closedir(FS_INT dNo)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Closedir(dNo);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_closedir(dNo);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Chdir(const char* path)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Chdir((FW_CHAR *)path);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_chdir(path);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Get_Cur_Dir(const char* drvname, char* curpath, FS_DWORD size)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	curpath[0] = '\0';
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Getcurdir((char*)drvname, curpath, size);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_getCurDir(drvname, curpath, size);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_EXT_Get_Qformat_Sectors(const char* drvname, FS_DWORD* sectors)
{
	FS_INT ercd = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*sectors = 0;
	ercd = 0;	// Temporary
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_EXT_getQformatSectors(drvname, sectors);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

#if 0
FS_INT BF_Mdf_Wrapper_EXT_Stat(const char* fname, FS_EXT_STAT* extsbuf)
{
	FS_INT ercd;

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_EXT_stat(fname, extsbuf);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);

	return ercd;
}
#endif

#if 0
FS_INT BF_Mdf_Wrapper_EXT_Utime(const char* fname, const FS_utimbuf* times, FS_WORD flag)
{
	FS_INT ercd;

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_EXT_utime(fname, times, flag);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);

	return ercd;
}
#endif

//#pragma arm section code="BASEFW_COMM_CODE"

FS_INT BF_Mdf_Wrapper_End_Mdf(FS_VOID)
{
	FS_INT ercd = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	FS_endMDF();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Lseek(FS_INT fNo, FS_INT32 offset, FS_WORD whence, FS_DWORD* length)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Lseek(fNo, offset, whence, length);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_lseek(fNo, offset, whence, length);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Lseek64(FS_INT fNo, FS_INT64 offset, FS_WORD whence, FS_INT64* length)
{
	FS_INT ercd;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Lseek64(fNo, offset, whence, (FW_INT64 *)length);
#endif	// Under implementation of VFS
#else
	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FS_ERR_ENQSEMTMOUT;
	}

	ercd = FS_lseek64(fNo, offset, whence, (FS_QWORD *)length);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Fsfirst(FS_BYTE index, const char* fname, FW_UCHAR attr, T_BF_FS_IF_ENTRY* buf)
{
	FW_INT32 ercd;

	ercd = 0;	// Temporary

	return ercd;
}

FS_INT BF_Mdf_Wrapper_Fsnext(FS_BYTE index, T_BF_FS_IF_ENTRY* buf)
{
	FW_INT32 ercd;

	ercd = 0;	// Temporary

	return ercd;
}

#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
/******************************************************************************
 for PrFILE2
******************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "fs_user_custom.h"
#include "fs_controller.h"
#include "fj_filesystem.h"
#include "mdf_wrapper.h"
#include "vfs.h"
#include "media_drv_wrapper.h"
#include "sdram_map_common.h"

#include "prfile2/pf_api.h"
#if PF_TRANSACTION_SAFE_SUPPORT
#include "prfile2/pf_sm_safe_api.h"
extern void *malloc(size_t size);
extern void free(void *ptr);
#endif // PF_TRANSACTION_SAFE_SUPPORT

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
//#define CO_BF_MDF_WRAPPER_DEBUG_FID_COUNT
//#define CO_BF_MDF_WRAPPER_DEBUG_SPACE
#ifndef CO_MEDIA_DRIVER_DEBUG_READ_WRITE_TIME
//#define CO_BF_MDF_WRAPPER_DEBUG_READ
//#define CO_BF_MDF_WRAPPER_DEBUG_WRITE
#endif
// Add PrFile2
//#define CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON
//#define CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
#define D_BF_PF_IER_NO_ERR					(0)		// D_BF_ + PF_IER_NO_ERR
#define D_BF_PF_ERR_SYSTEM					(-1)	// D_BF_ + PF_ERR_SYSTEM
#define D_BF_PF_ERR_EBADF					(9)		// D_BF_ + PF_ERR_EBADF
#define D_BF_PF_IER_END_OF_FILE				(27)	// D_BF_ + PF_IER_END_OF_FILE

#define D_BF_PF_NINVALIDATE 				(0x00)	// fsync: PF_NINVALIDATE
#define D_BF_PF_INVALIDATE 					(0x01)	// fsync: PF_INVALIDATE


// Add PrFile2 END
/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Nothing

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
static FW_LONG  gBF_Fs_Sync_Mode = D_BF_PF_NINVALIDATE;
static FW_USHORT  gBF_Fs_Search_Attr = 0;
static PF_DTA g_bf_dtabuf[3];
volatile FW_INT32  gBF_Fs_Eject_Safe = PF_WRITE_BACK_ON_SIGNIF_API;
#if PF_TRANSACTION_SAFE_SUPPORT
static PF_SAFE_STAT_FS gBF_Media_Drv_Wrapper_Safe_Status[D_BF_FS_CONTROLLER_MEDIA_ID_NUM] = {PF_SAFE_STAT_UNSUPPORTED};
PF_SAFE_INIT gBF_Media_Drv_Wrapper_Safe_Tbl[D_BF_FS_CONTROLLER_MEDIA_ID_NUM];
#endif // !PF_TRANSACTION_SAFE_SUPPORT

static CHAR gBF_Fs_Current_Directry[D_BF_FS_CONTROLLER_MEDIA_ID_NUM][D_BF_FS_IF_FNAME_MAX] = {{0}};
static CHAR gBF_Fs_Current_Drive[D_BF_FS_IF_DRVNAME_MAX] = {0};

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
#ifdef CO_BF_MDF_WRAPPER_DEBUG_FID_COUNT // ifdef_01
static volatile FW_ULONG g_bf_mdf_wrapper_fid_counter = 0;
#define mdf_wrapper_fid_count_init() (g_bf_mdf_wrapper_fid_counter = 0)
#define mdf_wrapper_fid_count_fs_open() (++g_bf_mdf_wrapper_fid_counter)
#define mdf_wrapper_fid_count_fs_close() do { \
	--g_bf_mdf_wrapper_fid_counter;	\
	BF_Fs_User_Custom_Info_Printer(("### Num of FID = %d ###\n", g_bf_mdf_wrapper_fid_counter)); \
} while(0)
#else                  // ifdef_01
#define mdf_wrapper_fid_count_init()
#define mdf_wrapper_fid_count_fs_open()
#define mdf_wrapper_fid_count_fs_close()
#endif                 // ifdef_01

#define mdf_wrapper_lock_media_space_cache()	(BF_Fs_User_Custom_Dis_Dsp())
#define mdf_wrapper_unlock_media_space_cache()	(BF_Fs_User_Custom_Ena_Dsp())
#define mdf_wrapper_init_sd_space_cache()		(g_bf_mdf_wrapper_sd_free_space_cache = 0)
#define mdf_wrapper_init_nf_space_cache()		(g_bf_mdf_wrapper_nf_free_space_cache = 0)
#define mdf_wrapper_init_a_space_cache()		(g_bf_mdf_wrapper_a_free_space_cache = 0)
#define mdf_wrapper_init_b_space_cache()		(g_bf_mdf_wrapper_b_free_space_cache = 0)
#define mdf_wrapper_init_c_space_cache()		(g_bf_mdf_wrapper_c_free_space_cache = 0)
#define mdf_wrapper_init_d_space_cache()		(g_bf_mdf_wrapper_d_free_space_cache = 0)
#define mdf_wrapper_init_e_space_cache()		(g_bf_mdf_wrapper_e_free_space_cache = 0)
#define mdf_wrapper_init_f_space_cache()		(g_bf_mdf_wrapper_f_free_space_cache = 0)
#define mdf_wrapper_init_j_space_cache()		(g_bf_mdf_wrapper_j_free_space_cache = 0)
#define mdf_wrapper_init_k_space_cache()		(g_bf_mdf_wrapper_k_free_space_cache = 0)
#define mdf_wrapper_init_l_space_cache()		(g_bf_mdf_wrapper_l_free_space_cache = 0)
#define mdf_wrapper_init_m_space_cache()		(g_bf_mdf_wrapper_m_free_space_cache = 0)
#define mdf_wrapper_init_n_space_cache()		(g_bf_mdf_wrapper_n_free_space_cache = 0)
#define mdf_wrapper_init_o_space_cache()		(g_bf_mdf_wrapper_o_free_space_cache = 0)
#define mdf_wrapper_init_p_space_cache()		(g_bf_mdf_wrapper_p_free_space_cache = 0)

#ifdef CO_BF_MDF_WRAPPER_DEBUG_SPACE
#define mdf_wrapper_free_space_printer() do { \
	BF_Fs_User_Custom_Info_Printer(("FREE SPACE: SD %lf byte , NF %lf byte \n", g_bf_mdf_wrapper_sd_free_space_cache, g_bf_mdf_wrapper_nf_free_space_cache));	\
} while(0)
#else
#define mdf_wrapper_free_space_printer()
#endif

#define mdf_wrapper_update_sd_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_sd_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_nf_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_nf_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)


#define mdf_wrapper_update_a_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_a_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_b_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_b_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_c_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_c_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_d_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_d_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_e_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_e_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_f_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_f_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_j_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_j_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_k_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_k_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_l_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_l_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_m_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_m_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_n_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_n_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_o_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_o_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_update_p_space_cache(blksize, bfree) do { \
	mdf_wrapper_lock_media_space_cache(); \
	g_bf_mdf_wrapper_p_free_space_cache = ((FW_DOUBLE)blksize * (FW_DOUBLE)bfree); \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_sd_space_cache(write_byte) do { \
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_sd_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_sd_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)


#define mdf_wrapper_write_update_nf_space_cache(write_byte) do { \
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_nf_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_nf_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)


#define mdf_wrapper_write_update_a_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_a_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_a_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_b_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_b_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_b_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_c_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_c_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_c_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_d_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_d_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_d_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_e_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_e_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_e_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_f_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_f_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_f_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_j_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_j_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_j_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_k_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_k_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_k_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_l_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_l_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_l_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_m_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_m_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_m_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_n_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_n_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_n_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_o_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_o_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_o_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

#define mdf_wrapper_write_update_p_space_cache(write_byte) do {	\
	mdf_wrapper_lock_media_space_cache(); \
	if (g_bf_mdf_wrapper_p_free_space_cache >= (FW_DOUBLE)write_byte){g_bf_mdf_wrapper_p_free_space_cache -= (FW_DOUBLE)write_byte;} \
	mdf_wrapper_unlock_media_space_cache();	\
	mdf_wrapper_free_space_printer(); \
} while(0)

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
#ifndef CO_LINUX_VERSION_ON
static FW_VOID   mdf_wrapper_init_media_space_cache(const char* drvname);
static FW_INT32 mdf_wrapper_cache_media_space(const char* drvname);
static FW_INT32 mdf_wrapper_cache_media_space_curdrv(FW_VOID);
static FW_DOUBLE mdf_wrapper_get_media_space(const char* drvname);
static FW_VOID   mdf_wrapper_set_media_space(const char* drvname, FW_DOUBLE space);
static FW_VOID   mdf_wrapper_get_drive_no(char* drvname, char* drv_no);
static FW_INT32 mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( const FW_INT32 errno_pf );
static FW_INT32 mdf_wrapper_pf_fstat_wrapper(const char* const name, PF_STAT* pf_sbuf);
#endif // CO_LINUX_VERSION_ON
/*******************************************************************************
 Media space cache
*******************************************************************************/
#ifndef CO_LINUX_VERSION_ON
static FW_DOUBLE g_bf_mdf_wrapper_sd_free_space_cache = 0;
static FW_DOUBLE g_bf_mdf_wrapper_nf_free_space_cache = 0;
static FW_DOUBLE g_bf_mdf_wrapper_a_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_b_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_c_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_d_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_e_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_f_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_j_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_k_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_l_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_m_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_n_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_o_free_space_cache  = 0;
static FW_DOUBLE g_bf_mdf_wrapper_p_free_space_cache  = 0;
#endif // CO_LINUX_VERSION_ON


#ifndef CO_LINUX_VERSION_ON
/**
 * init media space cache
 */
static VOID   mdf_wrapper_init_media_space_cache(const char* drvname)
{
	UCHAR media_id;

	BF_Fs_Controller_Get_Media_Id((char*)drvname, (CHAR*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_init_media_space_cache(): abnormal drvname=%s\n", drvname));
		return;
	}
	
	if(media_id == D_BF_FS_CONTROLLER_NF_0) {
		mdf_wrapper_init_nf_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_SD_0 || media_id == D_BF_FS_CONTROLLER_SD_1) {
		mdf_wrapper_init_sd_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_1) {
		mdf_wrapper_init_a_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_2) {
		mdf_wrapper_init_b_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_3) {
		mdf_wrapper_init_c_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_4) {
		mdf_wrapper_init_d_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_5) {
		mdf_wrapper_init_e_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_6) {
		mdf_wrapper_init_f_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_0) {
		mdf_wrapper_init_j_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_1) {
		mdf_wrapper_init_k_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_2) {
		mdf_wrapper_init_l_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_3) {
		mdf_wrapper_init_m_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_4) {
		mdf_wrapper_init_n_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_5) {
		mdf_wrapper_init_o_space_cache();
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_6) {
		mdf_wrapper_init_p_space_cache();
	}
#if 1	/* for Dir Copy  */
	else if (strcmp(drvname, D_BF_FS_IF_DST_DRIVE_NAME_STRING) == 0) {
		;
	}
#endif
	else {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_init_media_space_cache(): abnormal drvname=%s, media_type=%d \n", drvname, gBF_Fs_Controller_Media_Status[media_id].media_type));
	}

	mdf_wrapper_free_space_printer();
}

/**
 * cache media space from MDF
 * return : OK:0
 *          NG:D_BF_MDF_WRAPPER_ERR_xxx
 */
static FW_INT32 mdf_wrapper_cache_media_space(const char* drvname)
{
	FW_ULONG blksize, bfree;
	UCHAR media_id;
	PF_DEV_INF dev_inf;
	UCHAR drv_no;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	FW_INT32	errno_mdfwrapper = 0;

	mdf_wrapper_get_drive_no((char*)drvname, (char*)&drv_no);
	if (drv_no == 0) {
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}

	/* get the volume information */
	if ((ercd_pf = pf_getdev(drv_no, &dev_inf)) != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		if ( errno_pf != D_BF_PF_IER_NO_ERR ) {
			BF_Fs_User_Custom_Err_Printer(("pf_getdev failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));
		}
		return errno_mdfwrapper;
	}

	blksize = (dev_inf.bps * dev_inf.spc);
	bfree   = dev_inf.ecl;

#if 0	// for debug
	printf("getFreeSpace blksize : %d, blocks : %d, bfree : %d\n", blksize, blocks, bfree);
#endif
	//--- Dealing with media shortage problem
	if (bfree < 1) {
		bfree = 0;
	}
	else {
		bfree -= 1;
	}
	//---

	BF_Fs_Controller_Get_Media_Id((CHAR *)drvname, (CHAR*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("space cache update is abnormal. drvname=%s\n", drvname));
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}

	if (media_id == D_BF_FS_CONTROLLER_NF_0) {
		if (gBF_Fs_Controller_Media_Status[media_id].media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF) {
			mdf_wrapper_update_nf_space_cache(blksize, bfree);
			return 0;
		}
		else {
			BF_Fs_User_Custom_Wrn_Printer(("mdf_wrapper_cache_media_space(): drvname=%s, media_type=%d \n", drvname, gBF_Fs_Controller_Media_Status[media_id].media_type));
		}
	}
	else if (media_id == D_BF_FS_CONTROLLER_SD_0 || media_id == D_BF_FS_CONTROLLER_SD_1) {
		if (( gBF_Fs_Controller_Media_Status[media_id].media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD) ||
			( gBF_Fs_Controller_Media_Status[media_id].media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1 )) {
			mdf_wrapper_update_sd_space_cache(blksize, bfree);
			return 0;
		}
		else{
			BF_Fs_User_Custom_Wrn_Printer(("mdf_wrapper_cache_media_space(): drvname=%s, media_type=%d \n", drvname, gBF_Fs_Controller_Media_Status[media_id].media_type));
		}
	}
	else if (media_id == D_BF_FS_CONTROLLER_NF_1) {
		mdf_wrapper_update_a_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_NF_2) {
		mdf_wrapper_update_b_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_NF_3) {
		mdf_wrapper_update_c_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_NF_4) {
		mdf_wrapper_update_d_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_NF_5) {
		mdf_wrapper_update_e_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_NF_6) {
		mdf_wrapper_update_f_space_cache(blksize, bfree);
		return 0;
	}
#if 1	/* for Dir Copy  */
	else if (strcmp(drvname, D_BF_FS_IF_DST_DRIVE_NAME_STRING) == 0) {
		return 0;
	}
#endif
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_0) {
		mdf_wrapper_update_j_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_1) {
		mdf_wrapper_update_k_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_2) {
		mdf_wrapper_update_l_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_3) {
		mdf_wrapper_update_m_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_4) {
		mdf_wrapper_update_n_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_5) {
		mdf_wrapper_update_o_space_cache(blksize, bfree);
		return 0;
	}
	else if (media_id == D_BF_FS_CONTROLLER_EMMC_6) {
		mdf_wrapper_update_p_space_cache(blksize, bfree);
		return 0;
	}
	else {
		; // DO NOTHING
	}

	BF_Fs_User_Custom_Err_Printer(("space cache update is abnormal. drvname=%s\n", drvname));
	return D_BF_MDF_WRAPPER_ERR_NODEV;
}

/**
 * cache media space by current drive
 */
static FW_INT32 mdf_wrapper_cache_media_space_curdrv(FW_VOID)
{
#if 0 // PRFILE2
	FW_INT32 ercd;
	char   drvname[12];
//  PrFile2
	if ((ercd = FS_getCurDrv(drvname)) != 0) {
		BF_Fs_User_Custom_Err_Printer(("FS_getCurDrv(): ercd=%x\n", ercd));
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}
	return mdf_wrapper_cache_media_space(drvname);
#endif
	return D_BF_MDF_WRAPPER_ERR_NOSYS;
//  PrFile2 END
}

/**
 * get media space
 */
static DOUBLE mdf_wrapper_get_media_space(const char* drvname)
{
	DOUBLE space = 0;
	UCHAR media_id;

	BF_Fs_Controller_Get_Media_Id((char*)drvname, (CHAR*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_get_media_space(): abnormal drvname=%s\n", drvname));
		return space;
	}

	mdf_wrapper_lock_media_space_cache();

	if(media_id == D_BF_FS_CONTROLLER_NF_0) {
		space = g_bf_mdf_wrapper_nf_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_SD_0 || media_id == D_BF_FS_CONTROLLER_SD_1) {
		space = g_bf_mdf_wrapper_sd_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_1) {
		space = g_bf_mdf_wrapper_a_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_2) {
		space = g_bf_mdf_wrapper_b_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_3) {
		space = g_bf_mdf_wrapper_c_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_4) {
		space = g_bf_mdf_wrapper_d_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_5) {
		space = g_bf_mdf_wrapper_e_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_6) {
		space = g_bf_mdf_wrapper_f_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_0) {
		space = g_bf_mdf_wrapper_j_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_1) {
		space = g_bf_mdf_wrapper_k_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_2) {
		space = g_bf_mdf_wrapper_l_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_3) {
		space = g_bf_mdf_wrapper_m_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_4) {
		space = g_bf_mdf_wrapper_n_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_5) {
		space = g_bf_mdf_wrapper_o_free_space_cache;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_6) {
		space = g_bf_mdf_wrapper_p_free_space_cache;
	}
	else {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_get_media_space(): abnormal drvname=%s\n", drvname));
	}
	mdf_wrapper_unlock_media_space_cache();
	return space;
}

/**
 * set media space
 */
static VOID   mdf_wrapper_set_media_space(const char* drvname, FW_DOUBLE space)
{
	UCHAR media_id;

	BF_Fs_Controller_Get_Media_Id((char*)drvname, (CHAR*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_set_media_space(): abnormal drvname=%s\n", drvname));
		return;
	}

	mdf_wrapper_lock_media_space_cache();

	if(media_id == D_BF_FS_CONTROLLER_NF_0) {
		g_bf_mdf_wrapper_nf_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_SD_0 || media_id == D_BF_FS_CONTROLLER_SD_1) {
		g_bf_mdf_wrapper_sd_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_1) {
		g_bf_mdf_wrapper_a_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_2) {
		g_bf_mdf_wrapper_b_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_3) {
		g_bf_mdf_wrapper_c_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_4) {
		g_bf_mdf_wrapper_d_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_5) {
		g_bf_mdf_wrapper_e_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_NF_6) {
		g_bf_mdf_wrapper_f_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_0) {
		g_bf_mdf_wrapper_j_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_1) {
		g_bf_mdf_wrapper_k_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_2) {
		g_bf_mdf_wrapper_l_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_3) {
		g_bf_mdf_wrapper_m_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_4) {
		g_bf_mdf_wrapper_n_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_5) {
		g_bf_mdf_wrapper_o_free_space_cache = space;
	}
	else if(media_id == D_BF_FS_CONTROLLER_EMMC_6) {
		g_bf_mdf_wrapper_p_free_space_cache = space;
	}
	else {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_set_media_space(): abnormal drvname=%s\n", drvname));
	}
	mdf_wrapper_unlock_media_space_cache();
}

/**
 * Convert drive name
 */
static VOID mdf_wrapper_get_drive_no(char* drvname, char* drv_no)
{
	UCHAR media_id;

	*drv_no = 0;
	BF_Fs_Controller_Get_Media_Id((char*)drvname, (char*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_get_drive_no(): abnormal drvname=%s\n", drvname));
		return;
	}

	if (gBF_Fs_Drive_Tbl[media_id].drive == 0) {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_get_drive_no(): not attached drive=%s\n", drvname));
		return;
	}
	*drv_no = gBF_Fs_Drive_Tbl[media_id].drive;
}

/**
 * Convert path name
 * return : OK:0
 *          NG:D_BF_MDF_WRAPPER_ERR_xxx
 */
static FW_INT32 mdf_wrapper_convert_path_name(const char* fname)
{
	UCHAR media_id;
	CHAR* find_ptr;
	CHAR drvname[9] = {'0'};
	CHAR conv_name[D_BF_FS_IF_FNAME_MAX] = {'0'};
	FW_UINT32 fname_len;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s:fname=%s \n", __func__, fname));
#endif

	// check drive name
	find_ptr = strchr((char*)fname, ':');
	if (find_ptr == NULL) {
		return 0;
	}

	// get drive name
	strncpy(drvname, (const char*)fname, (find_ptr - (char*)fname));

	// get media id
	BF_Fs_Controller_Get_Media_Id((char*)drvname, (char*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_convert_path_name(): abnormal drvname=%s\n", drvname));
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}

	// covert path
	strncpy((char*)conv_name, (const char *)&gBF_Fs_Drive_Tbl[media_id].drive, 1);
	strcat((char*)conv_name, (const char *)find_ptr);
	fname_len = strlen(fname);
	strncpy((char*)fname, (const char *)conv_name, fname_len);

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s:conv_name=%s, fname_result=%s \n", __func__, conv_name, fname));
#endif

	return 0;
}

/**
 * Convert relative to absolute path
 */
static void mdf_wrapper_convert_path_rel2abs( const char *path, const char *base, char *result)
{
	const CHAR *pp, *bp;
	CHAR *rp, *ph;
	INT32 length;

	{
		int idx;
		length = strlen(path) + 4;
		ph = (char*)malloc(length);
		if( ph == NULL ){
			BF_Fs_User_Custom_Err_Printer(("mdf_wrapper_convert_path_rel2abs() malloc error\n"));
			return;
		}
		for(idx=0; '\0' != path[idx]; idx++) {
			if('/' == path[idx]) {
				ph[idx] = '\\';
			} else {
				ph[idx] = path[idx];
			}
		}
		ph[idx] = '\0';
	}
	
	if( *ph == '\\' ) {
		bp = strchr((char*)base, ':');
		if( bp == NULL ){
			// dummy route
			free((void*)ph);
			return;
		}
		length = (bp - (char*)base) + 1;
		strncpy(result, base, length);
		pp = result + length;
		strcpy((char*)pp, ph);
		free((void*)ph);
		return;
	}
	
	if( !strcmp(ph, ".") || !strcmp(ph, ".\\") ) {
		strcpy(result, base);
		free((void*)ph);
		return;
	}
	
	length = strlen( base );
	bp = base + length;
	if( *(bp - 1)== '\\' ) {
		--bp;
	}
	
	for( pp=ph; *pp=='.'; ) {	
		if( !strncmp(pp, "..\\", 3) ) {
			pp += 3;
			while( bp>base && *--bp!='\\');
		}
		else if( !strncmp(pp, ".\\", 2) ) {
			pp += 2;
		}
		else if( !strncmp(pp, "..\0", 3 ) ) {
			pp += 2;
			while( bp>base && *--bp!='\\' );
		}
		else {
			break;
		}
	}
	
	length = bp - base;
	strncpy( result, base, length );
	
	rp = result + length;
	*rp++ = '\\';

	strcpy( rp, pp );
	free((void*)ph);
	return;
}

/**
 * Analyze Path Infomation
 */
static VOID mdf_wrapper_analyze_path_info(const char* path, char* drvname, UCHAR* abs_flg, UCHAR* root_flg)
{
	CHAR* find_ptr;
	static CHAR* temp_ptr;

	*abs_flg = TRUE;
	*root_flg = FALSE;

	// check drive name
	find_ptr = strchr((char*)path, ':');
	if (find_ptr == NULL) {
		*abs_flg = FALSE;
		return;
	}

	temp_ptr = find_ptr + 1;
	if( *temp_ptr != 0 ){
		*root_flg = FALSE;
	}
	else{
		*root_flg = TRUE;
	}

	// get drive name
	strncpy(drvname, (const char*)path, (find_ptr - (char*)path));

	return;
}


#if PF_TRANSACTION_SAFE_SUPPORT
FW_INT32  mdf_wrapper_pfsmpl_malloc(char drive, FW_ULONG size, FW_ULONG ** pp_bufAddr)
{
	if ((*pp_bufAddr = malloc(size)) == NULL) {
		return -1;
	}
	return 0;
}

FW_INT32  mdf_wrapper_pfsmpl_free(char drive, FW_ULONG * p_bupAddr)
{
	free(p_bupAddr);
	return 0;
}
#endif // PF_TRANSACTION_SAFE_SUPPORT

/**
 * Convert error number from PRFILE2 to MDF.
 */
static FW_INT32 mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( const FW_INT32 errno_pf )
{
	FW_INT32	errno_mdfwrapper = 0;
	
	/* Error numbers can not be exactly matched. */
	/* It returns an error number that is as close as possible. */
	/* Please correct as necessary. */
	switch ( errno_pf ) {
		case D_BF_PF_IER_NO_ERR:          /* No error                     */
			errno_mdfwrapper = 0;
			break;
		case PF_ERR_EPERM:                /* Operation is not possible    */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_INVOKE;
			break;
		case PF_ERR_ENOENT:               /* No such file or directory    */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOENT;
			break;
		case PF_ERR_ESRCH:                /* No such process              */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_INVOKE;
			break;
		case PF_ERR_EIO:                  /* I/O Error(Driver Error)      */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_IO;
			break;
		case PF_ERR_E2BIG:                /* Argument list too long       */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_INVAL;
			break;
		case PF_ERR_ENOEXEC:              /* Not Executable by internal   */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_INVOKE;
			break;
		case PF_ERR_EBADF:                /* Bad file descriptor          */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_BADF;
			break;
		case PF_ERR_ENOMEM:               /* Not enough system memory     */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOMEM;
			break;
		case PF_ERR_EACCES:               /* Permission denied            */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_ROFS;
			break;
		case PF_ERR_EBUSY:                /* Can not use system resouces  */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_BUSY;
			break;
		case PF_ERR_EEXIST:               /* File already exists          */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_EXIST;
			break;
		case PF_ERR_ENODEV:               /* No such device.              */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NODEV;
			break;
		case PF_ERR_EISDIR:               /* find directory when file req */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_EXIST;
			break;
		case PF_ERR_EINVAL:               /* Invalid argument             */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_INVAL;
			break;
		case PF_ERR_ENFILE:               /* Too many open files(system)  */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_MOPEN;
			break;
		case PF_ERR_EMFILE:               /* Too many open files(user)    */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_MOPEN;
			break;
		case PF_ERR_EFBIG:                /* Over file size limit(4GB-1)  */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_FBIG;
			break;
		case PF_ERR_ENOSPC:               /* Device out of space          */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOSPC;
			break;
		case PF_ERR_ENOLCK:               /* Can not lock the file        */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_BADF;
			break;
		case PF_ERR_EILSEQ:               /* Illegal byte sequence        */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_INVOKE;
			break;
		case PF_ERR_ENOSYS:               /* Not implement function       */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOSYS;
			break;
		case PF_ERR_ENOTEMPTY:            /* Directory is not empty       */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOTEMPTY;
			break;
		case PF_ERR_EMOD_NOTSPRT:         /* Module is not supported      */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOSYS;
			break;
		case PF_ERR_EMOD_NOTREG:          /* Module is not registered     */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOSYS;
			break;
		case PF_ERR_EMOD_FCS:             /* FreeClusterSearch Module Err */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_INVOKE;
			break;
		case PF_ERR_EMOD_SAFE:            /* Safe Module Err              */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_INVOKE;
			break;
		case PF_ERR_ENOMEDIUM:            /* No medium found              */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NODEV;
			break;
		case PF_ERR_DFNC:                 /* Driver Error                 */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_IO;
			break;
		case PF_ERR_SYSTEM:               /* system error(general error)  */
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_SYSTEM;
			break;
		default:
			BF_Fs_User_Custom_Err_Printer(("%s:Error number is unknown. errno_prfile2=%d.\n", __func__, errno_pf));
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_SYSTEM;
			break;
	}

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s:errno_prfile2=%d, errno_mdfwrapper=%d \n", __func__, errno_pf, errno_mdfwrapper));
#endif

	return errno_mdfwrapper;
}

/**
 * pf_fstat wrapper.
 */
static FW_INT32 mdf_wrapper_pf_fstat_wrapper(const char* const pname, PF_STAT* fstat)
{
	CHAR conv_name[D_BF_FS_IF_FNAME_MAX] = {'0'};
	FW_UINT32 pname_len;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s:pname=%s \n", __func__, pname));
#endif

	pname_len = strlen(pname);
	strncpy(conv_name, pname, pname_len);

	// check dirname
	if ( pname_len >= 5 ) {	// ex."A:\b\"
		if ( conv_name[pname_len-1] == '\\' ) {
			// delete trailing delimiter
			conv_name[pname_len-1] = '\0';	// ex."A:\b"
		}
	}

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s:conv_name=%s \n", __func__, conv_name));
#endif

	return pf_fstat(conv_name, fstat);
}
#endif // CO_LINUX_VERSION_ON

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/*******************************************************************************
 eSOL(FS library) wrappers
*******************************************************************************/
FW_INT32 BF_Mdf_Wrapper_Start_Mdf(FW_VOID)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Start_MDF();
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	/* initialize diskmanager */
	(VOID)pdm_init_diskmanager(0, NULL);

	/* initialize PrFILE2 */
	ercd_pf = pf_init_prfile2(0, NULL);
	if (ercd_pf != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_init_prfile2 failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));
	}

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}


FW_INT32 BF_Mdf_Wrapper_End_Task(FW_VOID)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
	errno_mdfwrapper= 0;
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	errno_mdfwrapper= 0;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Init_Drv(char* drvname, T_FS_FUNCNAME* pDrvinfo, FW_VOID* pDevinfo)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	UCHAR media_id;
	
	BF_Fs_Controller_Get_Media_Id((CHAR *)drvname, (CHAR*)&media_id);
	errno_mdfwrapper = BF_Vfs_Init_Drv( (FW_CHAR*)drvname, (FW_INT32)media_id );
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	UCHAR media_id;

#if PF_TRANSACTION_SAFE_SUPPORT
	PF_SAFE_STAT safe_stat;
#endif // PF_TRANSACTION_SAFE_SUPPORT

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s \n", __func__, drvname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	BF_Fs_Controller_Get_Media_Id((char*)drvname, (CHAR*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("media_id(%d)\n", media_id));
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}

	gBF_Fs_Current_Directry[media_id][0] = 0;

	/* Eject safe */
	pf_buffering(gBF_Fs_Drive_Tbl[media_id].drive, gBF_Fs_Eject_Safe);

	/* mount drive */
	if ((ercd_pf = pf_mount(gBF_Fs_Drive_Tbl[media_id].drive)) != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_mount failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	/* set up for the FSINFO information */
	(void)pf_setupfsi(gBF_Fs_Drive_Tbl[media_id].drive, (PF_FSI_TRUST_ENABLE | PF_FSI_REFRESH_ENABLE));

#if PF_TRANSACTION_SAFE_SUPPORT
	// FAT Safe initialize
	gBF_Media_Drv_Wrapper_Safe_Tbl[media_id].si_malloc = &mdf_wrapper_pfsmpl_malloc;
	gBF_Media_Drv_Wrapper_Safe_Tbl[media_id].si_free   = &mdf_wrapper_pfsmpl_free;
	if ((ercd_pf = pf_safe_initialize(gBF_Fs_Drive_Tbl[media_id].drive, &gBF_Media_Drv_Wrapper_Safe_Tbl[media_id])) == 0) {
		// safe check
		if ((ercd_pf = pf_safe_check(gBF_Fs_Drive_Tbl[media_id].drive, &safe_stat)) == 0) {
			gBF_Media_Drv_Wrapper_Safe_Status[media_id] = safe_stat.ss_stat;
		}
		else {
			gBF_Media_Drv_Wrapper_Safe_Status[media_id] = PF_SAFE_STAT_UNSUPPORTED;
			BF_Fs_User_Custom_Wrn_Printer(("FAT Safe - pf_safe_check() failed(%d)\n", ercd_pf));
		}
	}
	else {
		gBF_Media_Drv_Wrapper_Safe_Status[media_id] = PF_SAFE_STAT_UNSUPPORTED;
		BF_Fs_User_Custom_Wrn_Printer(("FAT Safe - pf_safe_initialize() failed(%d)\n", ercd_pf));
	}

	// recover
	if (gBF_Media_Drv_Wrapper_Safe_Status[media_id] != PF_SAFE_STAT_UNSUPPORTED) {
		if (gBF_Media_Drv_Wrapper_Safe_Status[media_id] == PF_SAFE_STAT_ERROR) {
			BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
			BF_Fs_User_Custom_Err_Printer(("FAT Safe - FAT error\n"));
			return D_BF_MDF_WRAPPER_ERR_SYSTEM;
		}
		if (gBF_Media_Drv_Wrapper_Safe_Status[media_id] == PF_SAFE_STAT_ABNORMAL) {
			if ((ercd_pf = pf_safe_recover(gBF_Fs_Drive_Tbl[media_id].drive)) == 0) {
				BF_Fs_User_Custom_Info_Printer(("FAT Safe - pf_safe_recover() success\n"));
			}
			else {
				// error
				errno_pf = pf_errnum();
				errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );

				BF_Fs_User_Custom_Err_Printer(("FAT Safe - pf_safe_recover() failed(%d)\n", ercd_pf));
				BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
				return errno_mdfwrapper;
			}
		}
		// backup on
		if ((ercd_pf = pf_safe_backup_on(gBF_Fs_Drive_Tbl[media_id].drive)) != 0) {
			BF_Fs_User_Custom_Wrn_Printer(("FAT Safe - pf_safe_backup_on() failed(%d)\n", ercd_pf));
		}
	}
#endif // PF_TRANSACTION_SAFE_SUPPORT

	mdf_wrapper_cache_media_space(drvname);
	mdf_wrapper_fid_count_init();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_End_Drv(char* drvname)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;
#else
	errno_mdfwrapper = BF_Vfs_End_Drv( (FW_CHAR*)drvname );
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	UCHAR media_id;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s \n", __func__, drvname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	mdf_wrapper_init_media_space_cache(drvname);

	BF_Fs_Controller_Get_Media_Id((char*)drvname, (CHAR*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("media_id(%d)\n", media_id));
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}

	gBF_Fs_Current_Directry[media_id][0] = 0;

#if PF_TRANSACTION_SAFE_SUPPORT
	if (gBF_Media_Drv_Wrapper_Safe_Status[media_id] != PF_SAFE_STAT_UNSUPPORTED) {
		// backup off
		if ((ercd_pf = pf_safe_backup_off(gBF_Fs_Drive_Tbl[media_id].drive)) != 0) {
			BF_Fs_User_Custom_Wrn_Printer(("FAT Safe - pf_safe_backup_off() failed(%d)\n", ercd_pf));
		}
	}
#endif // PF_TRANSACTION_SAFE_SUPPORT

	/* unmount drive */
	if (pf_unmount(gBF_Fs_Drive_Tbl[media_id].drive, PF_UNMOUNT_NORMAL) < 0) {
		if ((ercd_pf = pf_unmount(gBF_Fs_Drive_Tbl[media_id].drive, PF_UNMOUNT_FORCE)) < 0) {
			BF_Fs_User_Custom_Err_Printer(("pf_unmount failed(%d)\n", ercd_pf));
		}
	}
#if PF_TRANSACTION_SAFE_SUPPORT
	// FAT Safe finalize
	if ((ercd_pf = pf_safe_finalize(gBF_Fs_Drive_Tbl[media_id].drive)) != 0) {
		BF_Fs_User_Custom_Wrn_Printer(("FAT Safe - pf_safe_finalize() failed(%d)\n", ercd_pf));
	}
#endif // PF_TRANSACTION_SAFE_SUPPORT

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Qformat(char* drvname)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	UCHAR media_id;

	BF_Fs_Controller_Get_Media_Id((char*)drvname, (CHAR*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_Controller_Get_Media_Id(): abnormal drvname=%s\n", drvname));
		return 0;
	}
	errno_mdfwrapper = BF_Vfs_Qformat((FW_INT32)media_id);
#endif	// Under implementation of VFS
#else
	UCHAR drv_no;
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s \n", __func__, drvname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	mdf_wrapper_get_drive_no((char*)drvname, (char*)&drv_no);
	if (drv_no == 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}

	ercd_pf = pf_format(drv_no, NULL);
	if (ercd_pf != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_format failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));
	}

	mdf_wrapper_cache_media_space(drvname);
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Nformat(T_FS_FUNCNAME *pDrvinfo, FW_VOID *pDevinfo)
{
	FW_INT32	errno_mdfwrapper = 0;
	
#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	errno_mdfwrapper = 0;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;

}

FW_INT32 BF_Mdf_Wrapper_Statvfs(const char* drvname, T_BF_FS_IF_STATVFS* buf)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	memset((void*)buf, 0, sizeof(T_BF_FS_IF_STATVFS));
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Stat_VFS( (FW_CHAR*)drvname, buf );
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	PF_DEV_INF dev_inf;
	UCHAR drv_no;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s \n", __func__, drvname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	mdf_wrapper_get_drive_no((char*)drvname, (char*)&drv_no);
	if (drv_no == 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}

	/* get the volume information */
	if ((ercd_pf = pf_getdev(drv_no, &dev_inf)) != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		if ( errno_pf != PF_ERR_ENOENT ) {
			BF_Fs_User_Custom_Err_Printer(("pf_getdev failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));
		}

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	buf->blksize = (dev_inf.bps * dev_inf.spc);
	buf->blocks  = dev_inf.cls;
	buf->bfree   = dev_inf.ecl;
	buf->bbad    = 0;	// No applicable information
	if(dev_inf.attr == PF_DEV_ATTR_READONLY) {
		buf->dstat   = E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT;
	}
	else {
		buf->dstat   = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;
	}

	if(dev_inf.fmt == PF_FMT_FAT12) {
		buf->fstype = FJ_FS_TYPE_FAT12;
	}
	else if(dev_inf.fmt == PF_FMT_FAT16) {
		buf->fstype = FJ_FS_TYPE_FAT16;
	}
	else if(dev_inf.fmt == PF_FMT_FAT32) {
		buf->fstype = FJ_FS_TYPE_FAT32;
	}
	else if(dev_inf.fmt == PF_FMT_EXFAT) {
		buf->fstype = FJ_FS_TYPE_EXFAT;
	}
	else {
		buf->fstype = FJ_FS_TYPE_UNKNOWN;
	}
	buf->fsvers  = 0;	// No applicable information
	buf->tasks   = 0;	// No applicable information
	buf->entries = 0;	// No applicable information
	buf->mdfvers = 0;	// No applicable information

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Get_Free_Space(const char* drvname, FW_ULONG* blksize, FW_ULONG* blocks, FW_ULONG* bfree)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*blksize = 0;
	*blocks = 0;
	*bfree = 0;
	errno_mdfwrapper = 0;	// Temporary
#else
	T_BF_FS_IF_STATVFS stat_buf;
	
	errno_mdfwrapper = BF_Vfs_Stat_VFS( (FW_CHAR*)drvname, &stat_buf );
	if( errno_mdfwrapper != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}
	
	*blksize = stat_buf.blksize;
	*blocks = stat_buf.blocks;
	*bfree = stat_buf.bfree;
	
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	PF_DEV_INF dev_inf;
	UCHAR drv_no;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s \n", __func__, drvname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	mdf_wrapper_get_drive_no((char*)drvname, (char*)&drv_no);
	if (drv_no == 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}

	/* get the volume information */
	if ((ercd_pf = pf_getdev(drv_no, &dev_inf)) != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_getdev failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	*blksize = (dev_inf.bps * dev_inf.spc);
	*blocks  = dev_inf.cls;
	*bfree   = dev_inf.ecl;

	mdf_wrapper_set_media_space(drvname, ((FW_DOUBLE)(*blksize) * (FW_DOUBLE)(*bfree)));
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

DOUBLE BF_Mdf_Wrapper_Get_Free_Space_By_Cache(const char* drvname)
{
#ifdef CO_LINUX_VERSION_ON
	return 0;
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s \n", __func__, drvname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	return mdf_wrapper_get_media_space(drvname);
#endif // CO_LINUX_VERSION_ON
}

FW_INT32 BF_Mdf_Wrapper_Get_Cur_Drv(char* drvname)
{
FW_INT32 errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	drvname[0] = '\0';
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Get_Cur_Drv( (FW_CHAR*)drvname );
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s \n", __func__, drvname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	strcpy(drvname, gBF_Fs_Current_Drive);
	errno_mdfwrapper = 0;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Set_Vol_Label(const char* drvname, const char* label)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Set_Vol_Label( (FW_CHAR*)drvname, (FW_CHAR*)label );
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	UCHAR drv_no;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s, label=%s \n", __func__, drvname, label));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	mdf_wrapper_get_drive_no((char*)drvname, (char*)&drv_no);
	if (drv_no == 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}

	ercd_pf = pf_setvol(drv_no, label);
	if (ercd_pf != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_setvol failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));
	}
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Get_Vol_Label(const char* drvname, char* label)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	label[0] = '\0';
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Get_Vol_Label( (FW_CHAR*)drvname, (FW_CHAR*)label );
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	PF_VOLTAB vol_info;
	UCHAR drv_no;
	FW_UINT32 name_len;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s \n", __func__, drvname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	mdf_wrapper_get_drive_no((char*)drvname, (char*)&drv_no);
	if (drv_no == 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}

	if ((ercd_pf = pf_getvol(drv_no, &vol_info)) != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_getvol failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	name_len = strlen((char*)vol_info.v_name);
	strncpy((char*)label, (const char*)vol_info.v_name, name_len);
	label[name_len] = '\0';

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Fsync(FW_INT32 fNo)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Fsync( fNo );
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	// PrFile2
	ercd_pf = pf_fsync((PF_FILE*)fNo);
	if (ercd_pf != 0 ) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_fsync failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}
	// PrFile2 END

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Sync(const char* drvname)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Sync( (FW_CHAR*)drvname );
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	UCHAR drv_no;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s \n", __func__, drvname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	// PrFile2
	mdf_wrapper_get_drive_no((char*)drvname, (char*)&drv_no);
	if (drv_no == 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}

	ercd_pf = pf_sync( drv_no, gBF_Fs_Sync_Mode );
	if (ercd_pf != 0 ) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_sync failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	mdf_wrapper_cache_media_space(drvname);

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);

#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Set_Con_Sec_Max(const char* drvname, FW_ULONG secmax)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 1	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#endif
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s \n", __func__, drvname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	errno_mdfwrapper = 0;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Set_Protection_Mode(const char* drvname, FW_ULONG mode)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Set_Protection_Mode( (FW_CHAR*)drvname, (FW_UINT32)mode );
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s, mode=%lu \n", __func__, drvname, mode));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	errno_mdfwrapper = 0;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Rename(const char* srcname, const char* dstname)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Rename( (FW_CHAR*)srcname, (FW_CHAR*)dstname );
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	CHAR srcname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};
	CHAR dstname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. srcname=%s, dstname=%s \n", __func__, srcname, dstname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	errno_mdfwrapper = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	strcpy(srcname_cnv, srcname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)srcname_cnv);
	if (ercd != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	strcpy(dstname_cnv, dstname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)dstname_cnv);
	if (ercd != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	if ((errno_mdfwrapper = pf_rename(srcname_cnv, dstname_cnv)) != 0) {
		// error

		if ((errno_mdfwrapper = pf_move(srcname_cnv, dstname_cnv)) != 0) {
				// error

			errno_pf = pf_errnum();
			errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
			BF_Fs_User_Custom_Err_Printer(("pf_rename failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));
	
			BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
			return errno_mdfwrapper;
		}
	}

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Stat(const char* fname, T_BF_FS_IF_STAT* sbuf)
{
	FW_INT32	errno_mdfwrapper = 0;
	FW_CHAR* p_str;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	memset((void*)sbuf, 0, sizeof(T_BF_FS_IF_STAT));
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Stat( (FW_CHAR*)fname, sbuf );
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	FW_CHAR fname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};
	PF_STAT  pf_sbuf;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. fname=%s \n", __func__, fname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	strcpy(fname_cnv, fname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)fname_cnv);
	if (errno_mdfwrapper != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	ercd_pf = mdf_wrapper_pf_fstat_wrapper(fname_cnv, &pf_sbuf);
	if (ercd_pf != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		if ( errno_pf == PF_ERR_ENOENT ) {
			BF_Fs_User_Custom_Info_Printer((" file not found : %s\n", fname_cnv ));
		}
		else {
			BF_Fs_User_Custom_Err_Printer(("pf_fstat failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));
		}

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	p_str = strrchr(fname, '\\');
	if (p_str == NULL) {
		strcpy(sbuf->fname, fname);
	}
	else {
		strcpy(sbuf->fname, p_str + 1);
	}
	sbuf->m_date   = pf_sbuf.fstcd;		// Creation date of file
	sbuf->m_time   = pf_sbuf.fstct;		// Time of file creation 
	sbuf->date     = pf_sbuf.fstud;		// Last update date of file
	sbuf->time     = pf_sbuf.fstut;		// Last update time of file
	sbuf->fileSize = (FW_ULLONG)pf_sbuf.fstfz;	//
	sbuf->attributes = pf_sbuf.fstat;	// attribute
	
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Set_Attribute(const char* fname, FW_UCHAR attributes)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Set_Attribute( (FW_CHAR*)fname, (FW_INT32)attributes );
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	PF_STAT pfstat;
	FW_CHAR fname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. fname=%s \n", __func__, fname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	strcpy(fname_cnv, fname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)fname_cnv);
	if (errno_mdfwrapper != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	ercd_pf = mdf_wrapper_pf_fstat_wrapper(fname_cnv, &pfstat);
	if (ercd_pf != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		if ( errno_pf != PF_ERR_ENOENT ) {
			BF_Fs_User_Custom_Err_Printer(("pf_fstat failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));
		}

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	if (attributes == D_BF_MDF_WRAPPER_ATTR_NORMAL) {
		attributes = D_BF_MDF_WRAPPER_ATTR_NONE;
	}
//	BF_Fs_User_Custom_Info_Printer(("chk: pf_fstat() pfstat.fstat = 0x%x \n", pfstat.fstat));
	if ( (pfstat.fstat & D_BF_MDF_WRAPPER_ATTR_DIRECTORY) != 0) {
	// Drictory
		ercd_pf = pf_chdmod(fname_cnv, (FW_UCHAR)attributes);
		if (ercd_pf != 0) {
			// error
			errno_pf = pf_errnum();
			errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
			BF_Fs_User_Custom_Err_Printer(("pf_chdmod failed(%d). errno_pf=%d, DirName=%s, Attr=0x%x.\n", ercd_pf, errno_pf, fname, attributes));
		}
	}
	else {
	// File
		ercd_pf = pf_chmod(fname_cnv, (FW_UCHAR)attributes);
		if (ercd_pf != 0) {
			// error
			errno_pf = pf_errnum();
			errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
			BF_Fs_User_Custom_Err_Printer(("pf_chdmod failed(%d). errno_pf=%d, FileName=%s, Attr=0x%x.\n", ercd_pf, errno_pf, fname, attributes));
		}
	}
	
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Set_Attribute_Multi(const char* dirname, FW_UCHAR attributes, FW_UCHAR attr_mode, char filename_list[][13], FW_INT32 fnum, MULTI_SET_ATTR_CB pcallback)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	FW_INT32	ercd = 0;
	FW_INT32 file_cnt;
	FW_UINT32 dname_len;

	FW_CHAR fname[D_BF_FS_VFS_FNAME_MAX] = {0};

//	ercd = BF_Vfs_Set_Attribute_Multi( (FW_CHAR *(*)[13])filename_list, (FW_UCHAR*)attributes, (FW_UCHAR*)attr_mode, (FW_INT32*)fnum );
	
	dname_len = strlen(dirname);
	
	for( file_cnt = 0; file_cnt < fnum; file_cnt++ ) {
		strncpy( ( char* )fname, dirname, dname_len);
		strcat( ( char* )fname, "\\" );
		strcat( ( char* )fname, filename_list[file_cnt] );
		
		ercd = BF_Vfs_Set_Attribute( (FW_CHAR*)fname, (FW_INT32)attributes );
		
		if( pcallback ) {
			( pcallback )( file_cnt, ercd );
		}
		
		memset( fname, 0, D_BF_FS_VFS_FNAME_MAX );
		
		errno_mdfwrapper = ercd;	// Temporary
	}
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. dirname=%s \n", __func__, dirname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Wrn_Printer(("=====  This function is not supported in the PrFile2!  ===== BF_Mdf_Wrapper_Set_Attribute_Multi() \n"));
	errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOSYS;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Get_Attribute(const char* fname, FW_UCHAR* attributes)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*attributes = 0;
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Get_Attribute( (FW_CHAR*)fname, (T_FW_UCHAR*)attributes );
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	PF_STAT pfstat;
	FW_CHAR fname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. fname=%s \n", __func__, fname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	strcpy(fname_cnv, fname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)fname_cnv);
	if (errno_mdfwrapper != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	ercd_pf = mdf_wrapper_pf_fstat_wrapper(fname_cnv, &pfstat);
	if (ercd_pf != D_OS_USER_E_OK ) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		if ( errno_pf != PF_ERR_ENOENT ) {
			BF_Fs_User_Custom_Err_Printer(("pf_fstat failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));
		}

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}
	*attributes = (pfstat.fstat & ~D_BF_MDF_WRAPPER_ATTR_NONE);

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("pfstat.fstat=%d \n", pfstat.fstat));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Utime(const char* fname, const T_BF_FS_utimbuf* times)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	memset((void*)times, 0, sizeof(T_BF_FS_utimbuf));
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Utime( (FW_CHAR*)fname, (T_BF_FS_utimbuf*)times );
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. fname=%s \n", __func__, fname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Wrn_Printer(("=====  This function is not supported in the PrFile2!  ===== BF_Mdf_Wrapper_Utime() \n"));
	errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOSYS;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_UtimeEx(const char* fname, FW_ULONG utime_mode, const T_BF_FS_utimbufEx* times)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	memset((void*)times, 0, sizeof(T_BF_FS_utimbufEx));
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Utime_Ex( (FW_CHAR*)fname, (FW_UINT32)utime_mode, (T_BF_FS_utimbufEx*)times );
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. fname=%s \n", __func__, fname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Wrn_Printer(("=====  This function is not supported in the PrFile2!  ===== BF_Mdf_Wrapper_UtimeEx() \n"));
	errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOSYS;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Open(const char* fname, FW_USHORT mode, FW_USHORT amode)
{

#ifdef CO_LINUX_VERSION_ON
	FW_INT32 fid;
#if 0	// Under implementation of VFS
	fid = 0x12345678;
#else
	fid = BF_Vfs_Open( fname, mode, amode );

#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	FW_INT32	errno_mdfwrapper = 0;
	PF_FILE* p_file;
	FW_CHAR  access_mode[4]={'\0'};
	FW_CHAR fname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. fname=%s, mode=%d, amode=%d \n", __func__, fname, mode, amode));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		BF_Fs_User_Custom_Err_Printer(("%s: Twai_Sem error = %d, fname = %s\n", __func__, ercd, fname));
		return 0;	// error
	}

	// PrFile2
	if (( mode & FJ_FS_OPEN_CREAT ) != 0){
		if (( mode & FJ_FS_OPEN_APPEND ) != 0) {
			strncpy(access_mode, "a+", 2);
		}
		else{
			strncpy(access_mode, "w+", 2);
		}
	}
	else if (( mode & FJ_FS_OPEN_TRUNC ) != 0){
		strncpy(access_mode, "w+", 2);
	}
	else if (( mode & FJ_FS_OPEN_RDWR ) != 0){
		strncpy(access_mode, "r+", 2);
	}
	else if (( mode & FJ_FS_OPEN_WRONLY ) != 0){
		strncpy(access_mode, "w", 1);
	}
	else if (( mode & FJ_FS_OPEN_APPEND ) != 0){
		strncpy(access_mode, "a", 1);
	}
	else if ( mode == FJ_FS_OPEN_RDONLY ){	// FJ_FS_OPEN_RDONLY
		strncpy(access_mode, "r", 1);
	}
	
	BF_Fs_User_Custom_Info_Printer(("open mode =0x%x, accessmode = %s, fname = %s\n",mode, access_mode, fname));

	strcpy(fname_cnv, fname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)fname_cnv);
	if (errno_mdfwrapper != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return 0;	// error
	}

	p_file = pf_fopen((PF_CONST char*)fname_cnv, (PF_CONST char*)access_mode );
	if ( p_file == 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_fopen failed. errno_pf=%d, fname = %s.\n", errno_pf, fname));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return 0;	// error
	}
	// PrFile2 END

	mdf_wrapper_fid_count_fs_open();
	BF_Fs_User_Custom_Info_Printer(("fopen << OK >>  fNo=%x, fname=%s\n", (FW_INT32)p_file, fname));
#ifdef CO_BF_MDF_WRAPPER_DEBUG_FID_COUNT
	if ( strncmp(fname, "A",1)==0){
//		BF_Fs_User_Custom_Info_Printer(("A open fNo=%x, fname=%s\n", p_file, fname));
	}else if (strncmp(fname, "I",1)==0){
		BF_Fs_User_Custom_Info_Printer(("I open fNo=%x, fname=%s\n", p_file, fname));
	}else if (strncmp(fname, "J",1)==0){
		BF_Fs_User_Custom_Info_Printer(("J open fNo=%x, fname=%s\n", p_file, fname));
	}else{
		BF_Fs_User_Custom_Info_Printer(("open else  fNo=%x, fname=%s\n", p_file, fname));
	}
#endif
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif	/* CO_LINUX_VERSION_ON */

	return (FW_INT32)p_file;
}

FW_INT32 BF_Mdf_Wrapper_Close(FW_INT32 fNo)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	if ((errno_mdfwrapper = BF_Vfs_Close(fNo)) != 0) {
		return errno_mdfwrapper;
	}
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	if ((ercd_pf = pf_fclose((PF_FILE *)fNo)) != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_fclose failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	mdf_wrapper_fid_count_fs_close();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

#ifdef CO_BF_MDF_WRAPPER_DEBUG_FID_COUNT
	BF_Fs_User_Custom_Info_Printer(("close fNo=%x\n", fNo));
#endif

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Read(FW_INT32 fNo, FW_VOID* cbuf, FW_ULONG count, FW_ULONG* length)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#else
	PF_SIZE_T result_count;
	FW_INT32 result_feof;
#endif
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON
	BF_Fs_User_Custom_Info_Printer(("DEBUG fread: buf=0x%x, count=%ld \n", (FW_UINT32)cbuf, count ));
#endif //CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON
	*length = 0;
#ifdef CO_BF_MDF_WRAPPER_DEBUG_READ
	FW_ULONG ltime=0;
	FW_ULLONG sys_time_before, sys_time_after;
	BF_Fs_User_Custom_Get_Tim(&sys_time_before);
#endif

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	errno_mdfwrapper = 0;	// Temporary
#else
	if ((errno_mdfwrapper = BF_Vfs_Read(fNo, cbuf, count, length)) != 0) {
		return errno_mdfwrapper;
	}
#endif	// Under implementation of VFS
#else

	FW_INT32	ercd = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}
	// PrFile2
	result_count = pf_fread( (void*)cbuf, 1, (PF_SIZE_T)count, (PF_FILE *)fNo);

	// Check result of read
	if ( result_count != (PF_SIZE_T)count ) {
		// error or reached the end of the file
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		
		// check feof
		result_feof = pf_feof((PF_FILE *)fNo);
		if ( result_feof == 0 ){
			// The point not reached the end of the file.
			BF_Fs_User_Custom_Wrn_Printer(("fread: feof = 0. The point not reached the end of the file. \n" ));
		}
		else if ( result_feof == 1 ){
			// The point reached the end of the file.
			BF_Fs_User_Custom_Wrn_Printer(("fread: feof = 1. The point reached the end of the file. \n" ));
			*length = (FW_ULONG)result_count;
			return D_BF_MDF_WRAPPER_ERR_EOF;
		}
		else{
			// foef error	// -1
			BF_Fs_User_Custom_Err_Printer(("fread: feof_error = %d. The file is not open or the file descriptor is illegal  \n", result_feof ));
		}

		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		if ( errno_pf != D_BF_PF_IER_NO_ERR ) {
			BF_Fs_User_Custom_Err_Printer(("pf_fread/pf_feof failed. errno_pf=%d.\n", errno_pf));
		}
		else {
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_SYSTEM;
		}
		return errno_mdfwrapper;
	}
	*length = (FW_ULONG)result_count;
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

#ifdef CO_BF_MDF_WRAPPER_DEBUG_READ
	BF_Fs_User_Custom_Get_Tim(&sys_time_after);
	ltime = (FW_ULONG)(sys_time_after - sys_time_before);
	BF_Fs_User_Custom_Info_Printer(("FS  Read Size=%u(byte), time=%u(ms), speed=%.3f(MB/s)\n", 
	(FW_UINT32)*length, (FW_UINT32)ltime, ((float)(*length) * 1000) / (ltime * 1024.0 * 1024.0)));
#endif
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON
	BF_Fs_User_Custom_Info_Printer(("DEBUG fread: buf=0x%x, count=%ld result_count=%ld \n", (FW_UINT32)cbuf, count,(FW_ULONG)result_count ));
#endif //CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Write(FW_INT32 fNo, const VOID* cbuf, FW_ULONG count, FW_ULONG* length)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_BF_MDF_WRAPPER_DEBUG_WRITE
	FW_ULONG ltime=0;
	FW_ULLONG sys_time_before, sys_time_after;
	BF_Fs_User_Custom_Get_Tim(&sys_time_before);
#endif

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	errno_mdfwrapper = 0;	// Temporary
#else
	if ((ercd = BF_Vfs_Write(fNo, cbuf, count, length)) != 0) {
		return errno_mdfwrapper;
	}
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	PF_SIZE_T result_count;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

//	BF_Fs_User_Custom_Info_Printer(("\n **TEST** [Write] a:0x%08lx, s:%ld(0x%08lx)\n", (ULONG)cbuf, (ULONG)count, (ULONG)count));

	result_count = pf_fwrite( (void*)cbuf, 1, (PF_SIZE_T)count, (PF_FILE *)fNo);

	// Check result of write
	if ( result_count != (PF_SIZE_T)count ) {
		// fwrite error
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		if(result_count < (PF_SIZE_T)count) {
			BF_Fs_User_Custom_Err_Printer(("fwrite error. An empty area of the filesystem is insufficient(result_count=%d != %ld).  \n", result_count, count));
		}

		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		if ( errno_pf != D_BF_PF_IER_NO_ERR ) {
			BF_Fs_User_Custom_Err_Printer(("pf_fwrite failed. errno_pf=%d.\n", errno_pf));
		}
		else {
			errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_SYSTEM;
		}
		return errno_mdfwrapper;
	}
	*length = (FW_ULONG)result_count;
#endif // CO_LINUX_VERSION_ON

#ifndef CO_LINUX_VERSION_ON
	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

#ifdef CO_BF_MDF_WRAPPER_DEBUG_WRITE
	BF_Fs_User_Custom_Get_Tim(&sys_time_after);
	ltime = (FW_ULONG)(sys_time_after - sys_time_before);
	BF_Fs_User_Custom_Info_Printer(("FS  Write Size=%u(byte), time=%u(ms), speed=%.3f(MB/s)\n", 
	      (FW_UINT32)*length, (FW_UINT32)ltime, ((float)(*length) * 1000) / (ltime * 1024.0 * 1024.0)));
#endif

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Create(const char* fname, FW_USHORT amode)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Create((char*)fname, amode);
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	PF_FILE* p_file;
	FW_CHAR fname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. fname=%s \n", __func__, fname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return 0;	// error
	}

	strcpy(fname_cnv, fname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)fname_cnv);
	if (errno_mdfwrapper != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return 0;	// error
	}

	p_file  = pf_create(fname_cnv, 0);
	if ( p_file == 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_create failed. errno_pf=%d, fname=%s.\n", errno_pf, fname));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return 0;	// error
	}
	// PrFile2 END

	mdf_wrapper_fid_count_fs_open();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
	BF_Fs_User_Custom_Info_Printer(("fcreate << OK >>  fNo=%x, fname=%s\n", (FW_INT32)p_file, fname));
#endif // CO_LINUX_VERSION_ON

	return (FW_INT32)p_file;
}

FW_INT32 BF_Mdf_Wrapper_Copy(const char* srcname, const char* dstname)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Copy((char*)srcname, (char*)dstname);
#endif	// Under implementation of VFS
#else

	FW_CHAR dst_bak_fname[D_BF_FS_IF_FNAME_MAX];
	FW_INT32 src_file_id;
	FW_INT32 dst_file_id;
	FW_ULONG r_length;
	FW_ULONG w_length;
	FW_INT32 eof_flg = 0;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. srcname=%s, dstname=%s \n", __func__, srcname, dstname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	strcpy(dst_bak_fname, dstname);

	src_file_id = BF_Mdf_Wrapper_Open(srcname, FJ_FS_OPEN_RDONLY, 0);
	if (src_file_id == 0) {
		BF_Fs_User_Custom_Err_Printer(("fopen: error. srcname = %s\n", srcname));
		return D_BF_MDF_WRAPPER_ERR_SYSTEM;
	}
	dst_file_id = BF_Mdf_Wrapper_Open(dstname, (FJ_FS_OPEN_CREAT|FJ_FS_OPEN_RDWR), 0);
	if (dst_file_id == 0) {
		BF_Fs_User_Custom_Err_Printer(("fopen: error. dstname = %s\n", dstname));
		BF_Mdf_Wrapper_Close(src_file_id);
		return D_BF_MDF_WRAPPER_ERR_SYSTEM;
	}
	
	while(1){
		errno_mdfwrapper = BF_Mdf_Wrapper_Read(src_file_id, (FW_VOID*)SDRAM_ADR_FS_COPY_BUFFER_AREA, SDRAM_SIZ_FS_COPY_BUFFER_AREA, &r_length);
		if((errno_mdfwrapper < 0) && (errno_mdfwrapper != D_BF_MDF_WRAPPER_ERR_EOF)) {
			BF_Fs_User_Custom_Err_Printer(("fread: error. ercd = %d\n", errno_mdfwrapper));
			BF_Mdf_Wrapper_Close(src_file_id);
			BF_Mdf_Wrapper_Close(dst_file_id);
			BF_Mdf_Wrapper_Remove((const char*)dst_bak_fname);
			return errno_mdfwrapper;
		}
		if(errno_mdfwrapper == D_BF_MDF_WRAPPER_ERR_EOF){
			eof_flg = 1;
		}
		errno_mdfwrapper = BF_Mdf_Wrapper_Write(dst_file_id, (const VOID*)SDRAM_ADR_FS_COPY_BUFFER_AREA, r_length, &w_length);
		if(errno_mdfwrapper < 0){
			BF_Fs_User_Custom_Err_Printer(("fwrite: error. ercd = %d\n", errno_mdfwrapper));
			BF_Mdf_Wrapper_Close(src_file_id);
			BF_Mdf_Wrapper_Close(dst_file_id);
			BF_Mdf_Wrapper_Remove((const char*)dst_bak_fname);
			return errno_mdfwrapper;
		}
		if(eof_flg == 1){
			break;
		}
	}

	BF_Mdf_Wrapper_Close(src_file_id);
	BF_Mdf_Wrapper_Close(dst_file_id);

	errno_mdfwrapper = 0;

#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Truncate(const char* fname, FW_ULONG size, FW_ULONG* length)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	errno_mdfwrapper = 0;	// Temporary
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Wrn_Printer(("=====  This function is not supported in the PrFile2!  ===== BF_Mdf_Wrapper_Truncate() \n"));
	errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOSYS;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Truncate64(const char* fname, FW_ULLONG size, FW_ULLONG* length)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	errno_mdfwrapper = 0;	// Temporary
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Wrn_Printer(("=====  This function is not supported in the PrFile2!  ===== BF_Mdf_Wrapper_Truncate64 \n"));
	errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOSYS;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Ftruncate(FW_INT32 fNo, FW_ULONG size, FW_ULONG* length)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	errno_mdfwrapper = 0;	// Temporary
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Wrn_Printer(("=====  This function is not supported in the PrFile2!  ===== BF_Mdf_Wrapper_Ftruncate() \n"));
	errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOSYS;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Ftruncate64(FW_INT32 fNo, FW_ULLONG size, FW_ULLONG* length)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	errno_mdfwrapper = 0;	// Temporary
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Wrn_Printer(("=====  This function is not supported in the PrFile2!  ===== BF_Mdf_Wrapper_Ftruncate64() \n"));
	errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOSYS;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Remove(const char* fname)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Remove((char*)fname);
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	FW_CHAR fname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. fname=%s \n", __func__, fname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	strcpy(fname_cnv, fname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)fname_cnv);
	if (errno_mdfwrapper != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	if ((ercd_pf = pf_remove(fname_cnv)) != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_remove failed(%d). errno_pf=%d, fname=%s.\n", ercd_pf, errno_pf, fname));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Remove_Multi(const char *dirname, char filename_list[][13], FW_INT32 fnum, MULTI_DEL_CB pcallback)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	FW_INT32	ercd = 0;
	FW_CHAR fname[D_BF_FS_VFS_FNAME_MAX] = {0};
	FW_INT32 file_cnt;
	FW_UINT32 dname_len;
	
	dname_len = strlen(dirname);

	for( file_cnt = 0; file_cnt < fnum; file_cnt++ ) {
		strncpy( ( char* )fname, dirname, dname_len);
		strcat( ( char* )fname, "\\" );
		strcat( ( char* )fname, filename_list[file_cnt] );
		
		ercd = BF_Vfs_Remove( (FW_CHAR*)fname );
		
		if( pcallback ) {
			( pcallback )( file_cnt, ercd );
		}
		
		memset( fname, 0, D_BF_FS_VFS_FNAME_MAX );
	}
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	FW_CHAR fname[D_BF_FS_IF_FNAME_MAX] = {0};
	FW_INT32 file_cnt;
	FW_UINT32 dname_len;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. dirname=%s \n", __func__, dirname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	dname_len = strlen(dirname);

	for( file_cnt = 0; file_cnt < fnum; file_cnt++ ) {
		strncpy( ( char* )fname, dirname, dname_len);
		strcat( ( char* )fname, "\\" );
		strcat( ( char* )fname, filename_list[file_cnt] );
		
		ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
		if (ercd != D_OS_USER_E_OK) {
			return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
		}
		
		errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)fname);
		if (errno_mdfwrapper != 0) {
			BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
			return errno_mdfwrapper;
		}

		if ((ercd_pf = pf_remove(fname)) != 0) {
			BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
			errno_pf = pf_errnum();
			if ( errno_pf != D_BF_PF_IER_NO_ERR ) {
				if( (errno_pf == PF_ERR_ENOENT) ||
					(errno_pf == PF_ERR_EACCES) ||
					(errno_pf == PF_ERR_EISDIR) ||
					(errno_pf == PF_ERR_EINVAL) ){
						if( pcallback ) {
							( pcallback )( file_cnt, errno_pf );
						}
						ercd = 0;
						memset( fname, 0, D_BF_FS_IF_FNAME_MAX );
						continue;
				}
				else{
					BF_Fs_User_Custom_Err_Printer(("pf_remove failed(%d). errno_pf=%d, fname=%s.\n", ercd_pf, errno_pf, fname));
				}
			}
			errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
			return errno_mdfwrapper;
		}

		mdf_wrapper_cache_media_space_curdrv();
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		
		if( pcallback ) {
			( pcallback )( file_cnt, ercd );
		}
		
		memset( fname, 0, D_BF_FS_IF_FNAME_MAX );
	}
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(const char* fname)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. fname=%s \n", __func__, fname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	FW_CHAR fname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_OS_USER_E_TMOUT;
	}

	strcpy(fname_cnv, fname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)fname_cnv);
	if (errno_mdfwrapper != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	if ((ercd_pf = pf_remove(fname_cnv)) != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_remove failed(%d). errno_pf=%d, fname=%s.\n", ercd_pf, errno_pf, fname));
	}

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Fstat(FW_INT32 fNo, T_BF_FS_IF_STAT* sbuf)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	memset((void*)sbuf, 0, sizeof(T_BF_FS_IF_STAT));
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Fstat(fNo, sbuf);
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	PF_STAT  pf_sbuf;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	ercd_pf = pf_fpstat((PF_FILE*)fNo, &pf_sbuf);
	if (ercd_pf != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		if ( errno_pf != PF_ERR_ENOENT ) {
			BF_Fs_User_Custom_Err_Printer(("pf_fpstat failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));
		}

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}
	
	sbuf->m_date   = pf_sbuf.fstcd;		// Creation date of file
	sbuf->m_time   = pf_sbuf.fstct;		// Time of file creation 
	sbuf->date     = pf_sbuf.fstud;		// Last update date of file
	sbuf->time     = pf_sbuf.fstut;		// Last update time of file
	sbuf->fileSize = (FW_ULLONG)pf_sbuf.fstfz;	//
	sbuf->attributes = pf_sbuf.fstat;;	// attribute
	
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);

#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32  BF_Mdf_Wrapper_Mkdir(const char* dirname, FW_USHORT mode)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	ercd = 0;	// Temporary
#else
	ercd = BF_Vfs_Mkdir((char*)dirname);
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	FW_CHAR dirname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. dirname=%s, mode=%d \n", __func__, dirname, mode));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	strcpy(dirname_cnv, dirname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)dirname_cnv);
	if (errno_mdfwrapper != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	if ((ercd_pf = pf_mkdir(dirname_cnv)) != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_mkdir failed(%d). errno_pf=%d, dirname=%s.\n", ercd_pf, errno_pf, dirname));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
	BF_Fs_User_Custom_Info_Printer(("pf_mkdir << OK >>  dirname=%s\n",  dirname));
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Rmdir(const char* dirname)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Rmdir((char*)dirname);
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	FW_CHAR dirname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. dirname=%s \n", __func__, dirname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	strcpy(dirname_cnv, dirname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)dirname_cnv);
	if (errno_mdfwrapper != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	if ((ercd_pf = pf_rmdir(dirname_cnv)) != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_rmdir failed(%d). errno_pf=%d, dirname=%s.\n", ercd_pf, errno_pf, dirname));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	mdf_wrapper_cache_media_space_curdrv();
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Opendir(const char* dirname, FW_INT32* dNo)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*dNo = 0;
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Opendir((char*)dirname, dNo);
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	FW_CHAR dirname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. dirname=%s \n", __func__, dirname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	// PrFile2
	strcpy(dirname_cnv, dirname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)dirname_cnv);
	if (errno_mdfwrapper != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	*dNo = 0;
	*dNo = (FW_INT32)pf_opendir(dirname_cnv);
	if ( *dNo == 0){
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_opendir failed(%d). errno_pf=%d, dirname=%s.\n", *dNo, errno_pf, dirname));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}
	// PrFile2 END
	
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Readdir(FW_INT32 dNo, T_BF_FS_IF_Dir_Info* fsdirent)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	memset((void*)fsdirent, 0, sizeof(T_BF_FS_IF_Dir_Info));
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Readdir(dNo, fsdirent);
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Wrn_Printer(("=====  This function is not supported in the PrFile2!  ===== BF_Mdf_Wrapper_Readdir() \n"));
	errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOSYS;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Closedir(FW_INT32 dNo)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Closedir(dNo);
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Wrn_Printer(("=====  This function is not supported in the PrFile2!  ===== BF_Mdf_Wrapper_Closedir() \n"));
	errno_mdfwrapper = D_BF_MDF_WRAPPER_ERR_NOSYS;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Chdir(const char* dirname)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Chdir((FW_CHAR *)dirname);
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	UCHAR media_id;
	UCHAR abs_flg;
	UCHAR root_flg;
	CHAR cur_drive[D_BF_FS_IF_DRVNAME_MAX] = {'\0'};
	CHAR cur_dir[D_BF_FS_IF_FNAME_MAX] = {'\0'};
	FW_CHAR dirname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. dirname=%s \n", __func__, dirname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	// PrFile2
	strcpy(dirname_cnv, dirname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)dirname_cnv);
	if (errno_mdfwrapper != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	ercd_pf = pf_chdir(dirname_cnv);
	if ( ercd_pf != 0){
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_chdir failed(%d). errno_pf=%d, dirname=%s.\n", ercd_pf, errno_pf, dirname));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}
	// PrFile2 END

	mdf_wrapper_analyze_path_info(dirname, cur_drive, &abs_flg, &root_flg);
	if( abs_flg == FALSE ){
		// relative path
		BF_Fs_Controller_Get_Media_Id(gBF_Fs_Current_Drive, (char*)&media_id);
		if (media_id == 0xFF) {
			// Not happen (dummy)
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_Controller_Get_Media_Id(): abnormal drvname=%s\n", gBF_Fs_Current_Drive));
		}
		mdf_wrapper_convert_path_rel2abs(dirname, (const char*)gBF_Fs_Current_Directry[media_id], cur_dir);
		strcpy((char*)gBF_Fs_Current_Directry[media_id], cur_dir);
	}
	else{
		// absolute path
		strcpy(gBF_Fs_Current_Drive, cur_drive);
		BF_Fs_Controller_Get_Media_Id(gBF_Fs_Current_Drive, (char*)&media_id);
		if( root_flg != TRUE ){
			// ex "A:\\" or "A:\\ABC"
			strcpy((char*)gBF_Fs_Current_Directry[media_id], dirname);
		}
		else{
			// ex "A:"
			if( strlen((char*)gBF_Fs_Current_Directry[media_id]) == 0 ){
				strcpy((char*)gBF_Fs_Current_Directry[media_id], dirname);
			}
		}
	}
	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Get_Cur_Dir(const char* drvname, char* curpath, FW_ULONG size)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	curpath[0] = '\0';
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Getcurdir((char*)drvname, curpath, size);
#endif	// Under implementation of VFS
#else
	FW_ULONG p_len;
	UCHAR media_id;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s \n", __func__, drvname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	BF_Fs_Controller_Get_Media_Id((CHAR*)drvname, (CHAR*)&media_id);
	if (media_id == 0xFF) {
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Get_Cur_Dir(): abnormal drvname=%s\n", drvname));
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}

	p_len = strlen( (char*)gBF_Fs_Current_Directry[media_id] );
	if( p_len == 0 ){
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Get_Cur_Dir(): Current Dir Nothing\n"));
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}
	if( size < (p_len+1) ){
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Get_Cur_Dir(): Path length error=%lu\n", size));
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}
	strcpy(curpath, (const char*)gBF_Fs_Current_Directry[media_id]);

	errno_mdfwrapper = 0;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_EXT_Get_Qformat_Sectors(const char* drvname, FW_ULONG* sectors)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*sectors = 0;
	errno_mdfwrapper = 0;	// Temporary
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	PF_DEV_INF dev_inf;
	UCHAR drv_no;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. drvname=%s \n", __func__, drvname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	mdf_wrapper_get_drive_no((char*)drvname, (char*)&drv_no);
	if (drv_no == 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return D_BF_MDF_WRAPPER_ERR_NODEV;
	}

	if ((ercd_pf = pf_getdev(drv_no, &dev_inf)) != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_getdev failed(%d). errno_pf=%d, drvname=%s.\n", ercd_pf, errno_pf, drvname));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return ercd;
	}

	*sectors = (dev_inf.cls * dev_inf.spc);

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

//#pragma arm section code="BASEFW_COMM_CODE"

FW_INT32 BF_Mdf_Wrapper_End_Mdf(FW_VOID)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	errno_mdfwrapper = 0;	// Temporary
#endif	// Under implementation of VFS
#else
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	errno_mdfwrapper = 0;
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Lseek(FW_INT32 fNo, FW_LONG offset, FW_USHORT whence, FW_ULONG* length)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Lseek(fNo, offset, whence, length);
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON
	PF_FPOS_T pos=0;
	ercd_pf = pf_fgetpos( (PF_FILE *)fNo, &pos);
	BF_Fs_User_Custom_Info_Printer(("_DEBUG_ fgetpos(before)= %d, offset=%d, whence=%d \n", (UINT32)pos, (UINT32)offset, whence ));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON

	// PrFile2
	ercd_pf = pf_fseek((PF_FILE *)fNo, (PF_OFF_T)offset, whence);
	if ( ercd_pf != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_fseek failed(%d). errno_pf=%d, offset=%ld, whence=%d.\n", ercd_pf, errno_pf, offset, whence));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}
#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON
//	BF_Fs_User_Custom_Wrn_Printer(("_DEBUG_ fseek offset=%d, w=%d \n", offset, whence));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON

	// getpos
	ercd_pf = pf_fgetpos( (PF_FILE *)fNo, (PF_FPOS_T*)length);
	if ( ercd_pf != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_fgetpos failed(%d). errno_pf=%d, offset=%ld, whence=%d.\n", ercd_pf, errno_pf, offset, whence));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON
	BF_Fs_User_Custom_Info_Printer(("_DEBUG_>>fgetpos(after)= %d \n", (UINT32)*length));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Lseek64(FW_INT32 fNo, FW_INT64 offset, FW_USHORT whence, FW_INT64* length)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
#if 0	// Under implementation of VFS
	*length = 0;
	errno_mdfwrapper = 0;	// Temporary
#else
	errno_mdfwrapper = BF_Vfs_Lseek64(fNo, offset, whence, length);
#endif	// Under implementation of VFS
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON
	PF_FPOS_T pos=0;
	ercd_pf = pf_fgetpos( (PF_FILE *)fNo, &pos);
	BF_Fs_User_Custom_Info_Printer(("_DEBUG_  fgetpos(before) = %d, offset=%d, whence=%d \n", (UINT32)pos, (UINT32)offset, whence ));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON

	ercd_pf = pf_fseek((PF_FILE *)fNo, (PF_OFF_T)offset, whence);
	if ( ercd_pf != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_fseek failed(%d). errno_pf=%d, offset=%d, whence=%d.\n", ercd_pf, errno_pf, (UINT32)offset, whence));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	ercd_pf = pf_fgetpos( (PF_FILE *)fNo, (PF_FPOS_T*)length);
	if ( ercd_pf != 0) {
		// error
		errno_pf = pf_errnum();
		errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
		BF_Fs_User_Custom_Err_Printer(("pf_fgetpos failed(%d). errno_pf=%d, offset=%d, whence=%d.\n", ercd_pf, errno_pf, (UINT32)offset, whence));

		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON
	BF_Fs_User_Custom_Info_Printer(("_DEBUG_  fgetpos(after)  = %d\n", (UINT32)*length));		//OK
	ercd_pf = pf_fgetpos( (PF_FILE *)fNo, &pos);
	*length =(FW_INT64)pos;
//	BF_Fs_User_Custom_Wrn_Printer(("_DEBUG_  fgetpos(after)  = %d\n", (UINT32)*length));		//o
	if ( pos > 0x7FFFFFFF ){
		BF_Fs_User_Custom_Info_Printer(("_DEBUG_>>fgetpos(after)  =length(Upper)= %ld,(Low)= %ld, fseek64 \n", (FW_LONG)(*length>>32), (FW_LONG)*length ));
	}else{
		BF_Fs_User_Custom_Info_Printer(("_DEBUG_>>fgetpos(after)  = %d, fseek64 \n", (UINT32)*length ));
	}
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_ON

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Fsfirst(FW_UCHAR index, const char* fname, FW_UCHAR attr, T_BF_FS_IF_ENTRY* buf)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
	errno_mdfwrapper = 0;	// Temporary
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;
	FW_CHAR fname_cnv[D_BF_FS_IF_FNAME_MAX] = {'0'};

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. fname=%s \n", __func__, fname));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	strcpy(fname_cnv, fname);
	errno_mdfwrapper = mdf_wrapper_convert_path_name((char*)fname_cnv);
	if (errno_mdfwrapper != 0) {
		BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
		return errno_mdfwrapper;
	}

	gBF_Fs_Search_Attr = (FW_USHORT)attr;
	while(1) {
		ercd_pf = pf_fsfirst(fname_cnv, gBF_Fs_Search_Attr, &g_bf_dtabuf[index]);

		if (ercd_pf != 0) {
			// error
			errno_pf = pf_errnum();
			if ( errno_pf == PF_ERR_ENOENT ) {
				errno_mdfwrapper = D_BF_MDF_WRAPPER_ENT_LAST;
			}
			else {
				errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
				BF_Fs_User_Custom_Err_Printer(("pf_fsfirst failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));
			}
			BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
			return errno_mdfwrapper;
		}

		if ((g_bf_dtabuf[index].Attribute & ~gBF_Fs_Search_Attr ) == 0) {
			break;
		}
	}

	buf->fs_attributes = g_bf_dtabuf[index].Attribute;
	if(strcmp((const CHAR*)g_bf_dtabuf[index].LongName, "\0") == 0){
		strncpy((char*)buf->fs_fname, (const char*)g_bf_dtabuf[index].FileName, sizeof(g_bf_dtabuf[index].FileName));
	}
	else {
		strncpy((char*)buf->fs_fname, (const char*)g_bf_dtabuf[index].LongName, sizeof(g_bf_dtabuf[index].LongName));
	}
	buf->fs_filesize   = (FW_ULONG)g_bf_dtabuf[index].FileSize;

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}

FW_INT32 BF_Mdf_Wrapper_Fsnext(FW_UCHAR index, T_BF_FS_IF_ENTRY* buf)
{
	FW_INT32	errno_mdfwrapper = 0;

#ifdef CO_LINUX_VERSION_ON
	errno_mdfwrapper = 0;	// Temporary
#else
	FW_INT32	ercd = 0;
	FW_INT32	ercd_pf = 0;
	FW_INT32	errno_pf = D_BF_PF_IER_NO_ERR;

#ifdef CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON
	BF_Fs_User_Custom_Info_Printer(("%s called. \n", __func__));
#endif // CO_BF_MDF_WRAPPER_PRFILE2_DEBUG_FUNC_ON

	ercd = BF_Fs_User_Custom_Twai_Sem(SID_MDF_W, D_BF_MDF_WRAPPER_SEM_W_TIME);
	if (ercd != D_OS_USER_E_OK) {
		return D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT;
	}

	while(1) {
		ercd_pf = pf_fsnext(&g_bf_dtabuf[index]);

		if (ercd_pf != 0) {
			// error
			errno_pf = pf_errnum();
			if ( errno_pf == PF_ERR_ENOENT ) {
				errno_mdfwrapper = D_BF_MDF_WRAPPER_ENT_LAST;
			}
			else {
				errno_mdfwrapper = mdf_wrapper_convert_errno_prfile2_to_mdfwrapper( errno_pf );
				BF_Fs_User_Custom_Err_Printer(("pf_fsnext failed(%d). errno_pf=%d.\n", ercd_pf, errno_pf));
			}
			BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
			return errno_mdfwrapper;
		}

		if ((g_bf_dtabuf[index].Attribute & ~gBF_Fs_Search_Attr ) == 0) {
			break;
		}
	}

	buf->fs_attributes = g_bf_dtabuf[index].Attribute;
	if(strcmp((const CHAR*)g_bf_dtabuf[index].LongName, "\0") == 0){
		strncpy((char*)buf->fs_fname, (const char*)g_bf_dtabuf[index].FileName, sizeof(g_bf_dtabuf[index].FileName));
	}
	else {
		strncpy((char*)buf->fs_fname, (const char*)g_bf_dtabuf[index].LongName, sizeof(g_bf_dtabuf[index].LongName));
	}
	buf->fs_filesize   = (FW_ULONG)g_bf_dtabuf[index].FileSize;

	BF_Fs_User_Custom_Sig_Sem(SID_MDF_W);
#endif // CO_LINUX_VERSION_ON

	return errno_mdfwrapper;
}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---
