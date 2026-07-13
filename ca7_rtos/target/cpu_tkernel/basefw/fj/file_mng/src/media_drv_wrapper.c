/**
 * @file		media_drv_wrapper.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include <string.h>
#include "arm.h"
#include "fs_user_custom.h"
#include "chiptop.h"
#include "fj_boot.h"
#include "boot_loader.h"
#include "fs_if.h"
#include "fs_controller.h"
#include "media_drv_wrapper.h"
#include "dd_gic.h"
#include "os_user_custom.h"

#include "mwsdio.h"
#include "mwnf.h"
#include "mwnf_core.h"
#include "NF_driver_wrapper.h"
#include "em_data.h"
#include "em_if_ext.h"
#include "em_format.h"
#include "emmc_custom.h" 
#include "fj_media.h"
#include "fj_filesystem.h"
#include "sdram_map_define.h"

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
#include "prfile2/pdm_api.h"
#include "prfile2/pdm_driver.h"
#include "prfile2/pf_api.h"
#include "em_format.h"
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT

#define D_BF_USER_SETTING_DATA_RAW_AREA_NUMS		(10)
#define D_BF_USER_SETTING_DATA_BLOCK_MAX_NUMS		(0xFFFF)
#define D_BF_USER_SETTING_DATA_CLEAR_COUNTS			(0xFFFFFFFF)
#define D_BF_USER_SETTING_DATA_MAX_COUNTS			(0xFEEEEEEE)

#define D_BF_USER_SETTING_DATA_INFO_SIZE			(2048)
#define D_BF_USER_SETTING_DATA_SIZE_MAX				(2048*63)
#define D_BF_USER_SETTING_DATA_TOTAL_SIZE			(D_BF_USER_SETTING_DATA_INFO_SIZE + D_BF_USER_SETTING_DATA_SIZE_MAX)
#define D_BF_USER_SETTING_DATA_TOTAL_SECTOR			(D_BF_USER_SETTING_DATA_TOTAL_SIZE / EM_CONFIG_DEFAULT_BLOCK_SIZE)


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
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
volatile T_FS_FUNCNAME gBF_Media_Drv_Wrapper_MWNF_driver =
{
	NF_Bread,
	NF_Bwrite,
	NULL,				// Bclear
	NULL,				// GetSectorSize
	NULL				// SetWriteMode
};

volatile T_FS_FUNCNAME gBF_Media_Drv_Wrapper_MWSDIO0_driver =
{
	MWSDIO0_Bread,
	MWSDIO0_Bwrite,
	NULL,				// Bclear
	NULL,				// GetSectorSize
	MWSDIO0_SetWriteMode	// SetWriteMode
};

volatile T_FS_FUNCNAME gBF_Media_Drv_Wrapper_MWSDIO1_driver =
{
	MWSDIO1_Bread,
	MWSDIO1_Bwrite,
	NULL,				// Bclear
	NULL,				// GetSectorSize
	MWSDIO1_SetWriteMode	// SetWriteMode
};

volatile T_FS_FUNCNAME gBF_Media_Drv_Wrapper_MWEM_driver =
{
	MWEM_Bread,
	MWEM_Bwrite,
	NULL,				// Bclear
	NULL,				// GetSectorSize
	NULL				// SetWriteMode
};
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
volatile T_FS_FUNCNAME gBF_Media_Drv_Wrapper_MWNF_driver =
{
	BF_Media_Drv_Wrapper_Init,
	BF_Media_Drv_Wrapper_Finalize,
	BF_Media_Drv_Wrapper_Mount,
	BF_Media_Drv_Wrapper_Unmount,
	BF_Media_Drv_Wrapper_Format,
	BF_Media_Drv_Wrapper_Read_NF,
	BF_Media_Drv_Wrapper_Write_NF,
	BF_Media_Drv_Wrapper_Get_Info_NF
};

volatile T_FS_FUNCNAME gBF_Media_Drv_Wrapper_MWSDIO0_driver =
{
	BF_Media_Drv_Wrapper_Init,
	BF_Media_Drv_Wrapper_Finalize,
	BF_Media_Drv_Wrapper_Mount,
	BF_Media_Drv_Wrapper_Unmount,
	BF_Media_Drv_Wrapper_Format,
	BF_Media_Drv_Wrapper_Read_SDIO0,
	BF_Media_Drv_Wrapper_Write_SDIO0,
	BF_Media_Drv_Wrapper_Get_Info_SDIO0
};

volatile T_FS_FUNCNAME gBF_Media_Drv_Wrapper_MWSDIO1_driver =
{
	BF_Media_Drv_Wrapper_Init,
	BF_Media_Drv_Wrapper_Finalize,
	BF_Media_Drv_Wrapper_Mount,
	BF_Media_Drv_Wrapper_Unmount,
	BF_Media_Drv_Wrapper_Format,
	BF_Media_Drv_Wrapper_Read_SDIO1,
	BF_Media_Drv_Wrapper_Write_SDIO1,
	BF_Media_Drv_Wrapper_Get_Info_SDIO1
};

volatile T_FS_FUNCNAME gBF_Media_Drv_Wrapper_MWEM_driver =
{
	BF_Media_Drv_Wrapper_Init,
	BF_Media_Drv_Wrapper_Finalize,
	BF_Media_Drv_Wrapper_Mount,
	BF_Media_Drv_Wrapper_Unmount,
	BF_Media_Drv_Wrapper_Format,
	BF_Media_Drv_Wrapper_Read_EMMC,
	BF_Media_Drv_Wrapper_Write_EMMC,
	BF_Media_Drv_Wrapper_Get_Info_EMMC
};

#define D_BF_MEDIA_DRV_WRAPPER_NUM_OF_FATPAGE		(64)
#define D_BF_MEDIA_DRV_WRAPPER_NUM_OF_DATPAGE		(32)
#define D_BF_MEDIA_DRV_WRAPPER_NUM_OF_TOTAL_PAGE	(D_BF_MEDIA_DRV_WRAPPER_NUM_OF_FATPAGE + D_BF_MEDIA_DRV_WRAPPER_NUM_OF_DATPAGE)
#define D_BF_MEDIA_DRV_WRAPPER_NUM_OF_FATSIZE		(8)
#define D_BF_MEDIA_DRV_WRAPPER_NUM_OF_DATSIZE		(8)

PDM_DISK* gBF_Media_Drv_Wrapper_Disk_Tbl[4];
PF_DRV_TBL gBF_Fs_Drive_Tbl[D_BF_FS_CONTROLLER_MEDIA_ID_NUM];
PDM_PARTITION* gBF_Fs_Partition_Tbl[D_BF_FS_CONTROLLER_MEDIA_ID_NUM];
PF_CACHE_SETTING gBF_Fs_cache_set[D_BF_FS_CONTROLLER_MEDIA_ID_NUM];
PF_CACHE_PAGE    gBF_Fs_cache_page[D_BF_FS_CONTROLLER_MEDIA_ID_NUM][D_BF_MEDIA_DRV_WRAPPER_NUM_OF_TOTAL_PAGE] __attribute__((section(".MDF_HEAP_SECTION"), aligned(64)));
PF_SECTOR_BUF    gBF_Fs_cache_buffer[D_BF_FS_CONTROLLER_MEDIA_ID_NUM][D_BF_MEDIA_DRV_WRAPPER_NUM_OF_TOTAL_PAGE] __attribute__((section(".MDF_HEAP_SECTION"), aligned(64)));
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

static volatile FW_UINT32 gUserData_latest_area  = 0xFF;
static volatile FW_UINT32 gUserData_latest_block = 0xFF;
#ifdef __GNUC__
static volatile FW_UCHAR gUserData_NFData[4*1024] __attribute__((aligned(8)));	// support max page size : 4KB
#else
static volatile __align(8) FW_UCHAR gUserData_NFData[4*1024];						// support max page size : 4KB
#endif
volatile FW_UINT32 gUserData_save_data_count = 0xFFFFFFFF;

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
PDM_ERROR  init_drv_tbl_sdio0(PDM_DISK_TBL* p_disk_tbl, FW_ULONG ui_ext) /* pgr0339 */
{
    p_disk_tbl->p_func = (PDM_FUNCTBL*)&gBF_Media_Drv_Wrapper_MWSDIO0_driver;
    p_disk_tbl->ui_ext = ui_ext;
	return 0;
}

PDM_ERROR  init_drv_tbl_sdio1(PDM_DISK_TBL* p_disk_tbl, FW_ULONG ui_ext)
{
    p_disk_tbl->p_func = (PDM_FUNCTBL*)&gBF_Media_Drv_Wrapper_MWSDIO1_driver;
    p_disk_tbl->ui_ext = ui_ext;
	return 0;
}

PDM_ERROR  init_drv_tbl_nf(PDM_DISK_TBL* p_disk_tbl, FW_ULONG ui_ext) /* pgr0339 */
{
    p_disk_tbl->p_func = (PDM_FUNCTBL*)&gBF_Media_Drv_Wrapper_MWNF_driver;
    p_disk_tbl->ui_ext = ui_ext;
	return 0;
}

PDM_ERROR  init_drv_tbl_emmc(PDM_DISK_TBL* p_disk_tbl, FW_ULONG ui_ext) /* pgr0339 */
{
    p_disk_tbl->p_func = (PDM_FUNCTBL*)&gBF_Media_Drv_Wrapper_MWEM_driver;
    p_disk_tbl->ui_ext = ui_ext;
	return 0;
}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/


/////////////////////////////////////////////////////
// romarea_boot
/////////////////////////////////////////////////////

/**
 * @brief set media interrupt
 */
FW_VOID BF_Media_Drv_Wrapper_Set_Interrupt(int sd_ch)
{
	INT32 ret = 0;

	if(!(sd_ch == 0 || sd_ch == 1))
	{
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Debug_Print_Error(("BF_Media_Drv_Wrapper_Set_Interrupt()sd_ch failed: sd_ch=%d\n", sd_ch));
#endif
		return;
	}
	if(sd_ch == 0)
	{
		// SD0 INT setting
		ret = MWSDIO0_InitCardIF();
		ret = MWSDIO0_CardCheck();

		// SD0 INT ON
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
		OS_User_EnableInt((OS_USER_INTID)E_DD_GIC_INTID_SD_CH3_INT, (OS_USER_INTLV)D_DD_GIC_PRI14);
		OS_User_EnableInt((OS_USER_INTID)E_DD_GIC_INTID_SD_CH3_SDIO_INT, (OS_USER_INTLV)D_DD_GIC_PRI14);
#endif	// for Linux Co-op
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		if (ret != D_SD0_OK) {
			BF_Fs_User_Custom_Info_Printer(("SD0 is not inserted\n"));
		}
#endif
	}
	else
	{
		// SD1 INT setting
		ret = MWSDIO1_InitCardIF();
		ret = MWSDIO1_CardCheck();

		// SD1 INT ON
#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
		OS_User_EnableInt((OS_USER_INTID)E_DD_GIC_INTID_SD_CH2_UHS1_INT, (OS_USER_INTLV)D_DD_GIC_PRI14);
		OS_User_EnableInt((OS_USER_INTID)E_DD_GIC_INTID_SD_CH2_SDIO_INT, (OS_USER_INTLV)D_DD_GIC_PRI14);
		OS_User_EnableInt((OS_USER_INTID)E_DD_GIC_INTID_SD_CH2_UHS2_INT, (OS_USER_INTLV)D_DD_GIC_PRI14);
#endif	// for Linux Co-op
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		if (ret != D_SD1_OK) {
			BF_Fs_User_Custom_Info_Printer(("SD1 is not inserted\n"));
		}
#endif
	}
}

FW_INT32 BF_Media_Drv_Wrapper_Init_SDIO0(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info, E_BF_MEDIA_DRV_WRAPPER_WP* wp)
{
	FW_INT32 ercd;			// error code
	FW_INT32 retry_cnt;		// init retry count
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	PDM_INIT_DISK drv_init_tbl;
	PF_DRV_TBL*      drv_tbl[2];

	// fail safe
	if (gBF_Media_Drv_Wrapper_Disk_Tbl[0] != NULL) {
		BF_Fs_User_Custom_Info_Printer(("--- fail safe : SD0 close disk\n"));
		(FW_VOID)pf_unmount(gBF_Fs_Drive_Tbl[D_BF_FS_CONTROLLER_SD_0].drive, PF_UNMOUNT_NORMAL);
		(FW_VOID)BF_Media_Drv_Wrapper_Finalize_SDIO0();
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

	/* initialize SD driver */
	for (retry_cnt = 0; retry_cnt < D_BF_MEDIA_DRV_WRAPPER_SD_INIT_RETRY_CNT; retry_cnt++) {

		ercd = SD0_sdio_init();
		if (ercd == 0) {
			break;
		}
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Debug_Print_Error(("SD0_sdio_init() failed: ercd=%d\n", ercd));
#endif
		BF_Fs_User_Custom_Dly_Tsk(100);
	}
	if (ercd != 0) {
		return ercd;
	}
	*driver_tbl = (T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWSDIO0_driver;
	*media_info = (FW_VOID*)&gSD0_dev_mng;

	if (gSD0_dev_mng.wp) {
		*wp = E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT;
	}
	else {
		*wp = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;
	}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	/* open a disk */
	drv_init_tbl.p_func = &init_drv_tbl_sdio0;
	drv_init_tbl.ui_ext = 0;
	ercd = pdm_open_disk(&drv_init_tbl, &gBF_Media_Drv_Wrapper_Disk_Tbl[0]);
	if ((ercd != 0) || (gBF_Media_Drv_Wrapper_Disk_Tbl[0] == NULL)) {
		return -1;
	}

	if ((ercd = pdm_open_partition(gBF_Media_Drv_Wrapper_Disk_Tbl[0], (FW_LONG)0, &gBF_Fs_Partition_Tbl[D_BF_FS_CONTROLLER_SD_0])) != 0) {
		return -1;
	}

	/* initialize driver table */
	gBF_Fs_Drive_Tbl[D_BF_FS_CONTROLLER_SD_0].p_part            = gBF_Fs_Partition_Tbl[D_BF_FS_CONTROLLER_SD_0];
	gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_0].pages             = gBF_Fs_cache_page[D_BF_FS_CONTROLLER_SD_0];
	gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_0].buffers           = gBF_Fs_cache_buffer[D_BF_FS_CONTROLLER_SD_0];
	gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_0].num_fat_pages     = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_FATPAGE;
	gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_0].num_data_pages    = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_DATPAGE;
	gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_0].num_fat_buf_size  = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_FATSIZE;
	gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_0].num_data_buf_size = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_DATSIZE;
	gBF_Fs_Drive_Tbl[D_BF_FS_CONTROLLER_SD_0].cache = &gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_0];
	drv_tbl[0] = &gBF_Fs_Drive_Tbl[D_BF_FS_CONTROLLER_SD_0];
	drv_tbl[1] = NULL; // end of driver table

	/* attach driver */
	if ((ercd = pf_attach((PF_DRV_TBL**)&drv_tbl)) != 0) {
		return -1;
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---
	return 0;
}

FW_INT32 BF_Media_Drv_Wrapper_Init_SDIO1(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info, E_BF_MEDIA_DRV_WRAPPER_WP* wp)
{
	FW_INT32 ercd;			// error code
	FW_INT32 retry_cnt;		// init retry count
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	PDM_INIT_DISK drv_init_tbl;
	PF_DRV_TBL*      drv_tbl[2];

	// fail safe
	if (gBF_Media_Drv_Wrapper_Disk_Tbl[1] != NULL) {
		BF_Fs_User_Custom_Info_Printer(("--- fail safe : SD1 close disk\n"));
		(FW_VOID)pf_unmount(gBF_Fs_Drive_Tbl[D_BF_FS_CONTROLLER_SD_1].drive, PF_UNMOUNT_NORMAL);
		(FW_VOID)BF_Media_Drv_Wrapper_Finalize_SDIO1();
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

	/* initialize SD driver */
	for (retry_cnt = 0; retry_cnt < D_BF_MEDIA_DRV_WRAPPER_SD_INIT_RETRY_CNT; retry_cnt++) {
		ercd = SD1_sdio_init();
		if (ercd == 0) {
			break;
		}
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Debug_Print_Error(("SD1_sdio_init() failed: ercd=%d\n", ercd));
#endif
		BF_Fs_User_Custom_Dly_Tsk(100);
	}
	if (ercd != 0) {
		return ercd;
	}
	*driver_tbl = (T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWSDIO1_driver;
	*media_info = (FW_VOID*)&gSD1_dev_mng;

	if (gSD1_dev_mng.wp) {
		*wp = E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT;
	}
	else {
		*wp = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;
	}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	/* open a disk */
	drv_init_tbl.p_func = &init_drv_tbl_sdio1;
	drv_init_tbl.ui_ext = 0;
	ercd = pdm_open_disk(&drv_init_tbl, &gBF_Media_Drv_Wrapper_Disk_Tbl[1]);
	if ((ercd != 0) || (gBF_Media_Drv_Wrapper_Disk_Tbl[1] == NULL)) {
		return -1;
	}

	if ((ercd = pdm_open_partition(gBF_Media_Drv_Wrapper_Disk_Tbl[1], (FW_LONG)0, &gBF_Fs_Partition_Tbl[D_BF_FS_CONTROLLER_SD_1])) != 0) {
		return -1;
	}

	/* initialize driver table */
	gBF_Fs_Drive_Tbl[D_BF_FS_CONTROLLER_SD_1].p_part            = gBF_Fs_Partition_Tbl[D_BF_FS_CONTROLLER_SD_1];
	gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_1].pages             = gBF_Fs_cache_page[D_BF_FS_CONTROLLER_SD_1];
	gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_1].buffers           = gBF_Fs_cache_buffer[D_BF_FS_CONTROLLER_SD_1];
	gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_1].num_fat_pages     = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_FATPAGE;
	gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_1].num_data_pages    = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_DATPAGE;
	gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_1].num_fat_buf_size  = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_FATSIZE;
	gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_1].num_data_buf_size = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_DATSIZE;
	gBF_Fs_Drive_Tbl[D_BF_FS_CONTROLLER_SD_1].cache = &gBF_Fs_cache_set[D_BF_FS_CONTROLLER_SD_1];
	drv_tbl[0] = &gBF_Fs_Drive_Tbl[D_BF_FS_CONTROLLER_SD_1];
	drv_tbl[1] = NULL; // end of driver table

	/* attach driver */
	if ((ercd = pf_attach((PF_DRV_TBL**)&drv_tbl)) != 0) {
		return -1;
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---
	return 0;
}

FW_INT32 BF_Media_Drv_Wrapper_Init_NF(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info, E_BF_MEDIA_DRV_WRAPPER_WP* wp)
{
	FW_INT32 ercd = -1;			// error code
	FW_INT32 retry_cnt;			// init retry count
	FW_UINT32 w_addr;
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	PDM_INIT_DISK drv_init_tbl;
	PF_DRV_TBL*      drv_tbl[NF_MAX_DRV_NUM+1];
	UCHAR media_id;
	FW_INT32 drive_num;

#if 0
	// fail safe
	if (gBF_Media_Drv_Wrapper_Disk_Tbl[2] != NULL) {
		BF_Fs_User_Custom_Info_Printer(("--- fail safe : NF close disk\n"));
		(FW_VOID)BF_Media_Drv_Wrapper_Finalize_NF();
	}
#endif
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

	w_addr = (FW_UINT32)gMW_Exs_IRAM_NF;
	if(BF_Boot_Loader_Check(D_BF_BOOT_LOADER_ALL_CODE) == 1) {
		// NF interrupt(ext3) setting
		// initialize NF driver
		for (retry_cnt = 0; retry_cnt < D_BF_MEDIA_DRV_WRAPPER_NF_INIT_RETRY_CNT; retry_cnt++) {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
			BF_Fs_User_Custom_Info_Printer(("MWNF_BasicInit() START\n"));
#endif
			ercd = MWNF_BasicInit(w_addr, 1);
			if (ercd == 0) {
				break;
			}

			BF_Fs_User_Custom_Tslp_Tsk(100);
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
			BF_Debug_Print_Error(("MWNF : ERROR :BF_Media_Drv_Wrapper_Init_NF(). ercd=%d\n", ercd));
#endif
		}
		if (ercd != 0) {
			return ercd;
		}
	}
#if 0   //move to fs_controller_init_nf
	ercd = FS_startMDF();
	if ((ercd != 0) && (ercd != FS_ERR_MDF)) {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Debug_Print_Error(("startMDF() failed(%d)\n\r", ercd));
#endif
		return ercd;
	}
#endif
	*driver_tbl = (T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWNF_driver;
	*media_info = (FW_VOID*)&NFdevInfo[0];
	*wp = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	/* open a disk */
	drv_init_tbl.p_func = &init_drv_tbl_nf;
	drv_init_tbl.ui_ext = 0;
	ercd = pdm_open_disk(&drv_init_tbl, &gBF_Media_Drv_Wrapper_Disk_Tbl[2]);
	if ((ercd != 0) || (gBF_Media_Drv_Wrapper_Disk_Tbl[2] == NULL)) {
		BF_Debug_Print_Error(("pdm_open_disk() error(%d)\n", ercd));
		return ercd;
	}

	drive_num = 0;
	for (media_id = D_BF_FS_CONTROLLER_NF_0; media_id <= D_BF_FS_CONTROLLER_NF_6; media_id++) {
		if ((ercd = pdm_open_partition(gBF_Media_Drv_Wrapper_Disk_Tbl[2], drive_num, &gBF_Fs_Partition_Tbl[media_id])) != 0) {
			BF_Debug_Print_Error(("pdm_open_partition error(%d)\n", ercd));
			return -1;
		}
		/* initialize driver table */
		gBF_Fs_Drive_Tbl[media_id].p_part            = gBF_Fs_Partition_Tbl[media_id];
		gBF_Fs_cache_set[media_id].pages             = gBF_Fs_cache_page[media_id];
		gBF_Fs_cache_set[media_id].buffers           = gBF_Fs_cache_buffer[media_id];
		gBF_Fs_cache_set[media_id].num_fat_pages     = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_FATPAGE;
		gBF_Fs_cache_set[media_id].num_data_pages    = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_DATPAGE;
		gBF_Fs_cache_set[media_id].num_fat_buf_size  = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_FATSIZE;
		gBF_Fs_cache_set[media_id].num_data_buf_size = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_DATSIZE;
		gBF_Fs_Drive_Tbl[media_id].cache = &gBF_Fs_cache_set[media_id];
		drv_tbl[drive_num] = &gBF_Fs_Drive_Tbl[media_id];

		drive_num++;
		if(drive_num >= gNF_Drive_Num) {
			break;
		}
	}
	drv_tbl[drive_num] = NULL; // end of driver table

	/* attach driver */
	if ((ercd = pf_attach((PF_DRV_TBL**)&drv_tbl)) != 0) {
		BF_Debug_Print_Error(("pf_attach error(%d)\n", ercd));
		return -1;
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---
	return 0;
}

FW_INT32 BF_Media_Drv_Wrapper_Init_EMMC(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info, E_BF_MEDIA_DRV_WRAPPER_WP* wp)
{
	FW_INT32 ercd;			// error code
	FW_INT32 retry_cnt;		// init retry count
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	PDM_INIT_DISK drv_init_tbl;
	PF_DRV_TBL*      drv_tbl[EM_MAX_DRV_NUM+1];
	UCHAR media_id;
	FW_INT32 drive_num;

#if 0
	// fail safe
	if (gBF_Media_Drv_Wrapper_Disk_Tbl[3] != NULL) {
		BF_Fs_User_Custom_Info_Printer(("--- fail safe : eMMC close disk\n"));
		(FW_VOID)BF_Media_Drv_Wrapper_Finalize_EMMC();
	}
#endif
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

	/* initialize eMMC driver */
	for (retry_cnt = 0; retry_cnt < D_BF_MEDIA_DRV_WRAPPER_SD_INIT_RETRY_CNT; retry_cnt++) {
		ercd = MWEM_DeviceInit();
		if (ercd == 0) {
			break;
		}
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Debug_Print_Error(("MWEM_DeviceInit() failed: ercd=%d\n", ercd));
#endif
		BF_Fs_User_Custom_Dly_Tsk(100);
	}
	if (ercd != 0) {
		return ercd;
	}
	*driver_tbl = (T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWEM_driver;
	*media_info = (FW_VOID*)&EMdevInfo[0];
	*wp = E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT;

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	/* open a disk */
	drv_init_tbl.p_func = &init_drv_tbl_emmc;
	drv_init_tbl.ui_ext = 0;
	ercd = pdm_open_disk(&drv_init_tbl, &gBF_Media_Drv_Wrapper_Disk_Tbl[3]);
	if ((ercd != 0) || (gBF_Media_Drv_Wrapper_Disk_Tbl[3] == NULL)) {
		BF_Debug_Print_Error(("pdm_open_disk() error(%d)\n", ercd));
		return ercd;
	}

	drive_num = 0;
	for (media_id = D_BF_FS_CONTROLLER_EMMC_0; media_id < D_BF_FS_CONTROLLER_MEDIA_ID_NUM; media_id++) {
		if ((ercd = pdm_open_partition(gBF_Media_Drv_Wrapper_Disk_Tbl[3], drive_num, &gBF_Fs_Partition_Tbl[media_id])) != 0) {
			BF_Debug_Print_Error(("pdm_open_partition error(%d)\n", ercd));
			return -1;
		}
		/* initialize driver table */
		gBF_Fs_Drive_Tbl[media_id].p_part            = gBF_Fs_Partition_Tbl[media_id];
		gBF_Fs_cache_set[media_id].pages             = gBF_Fs_cache_page[media_id];
		gBF_Fs_cache_set[media_id].buffers           = gBF_Fs_cache_buffer[media_id];
		gBF_Fs_cache_set[media_id].num_fat_pages     = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_FATPAGE;
		gBF_Fs_cache_set[media_id].num_data_pages    = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_DATPAGE;
		gBF_Fs_cache_set[media_id].num_fat_buf_size  = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_FATSIZE;
		gBF_Fs_cache_set[media_id].num_data_buf_size = D_BF_MEDIA_DRV_WRAPPER_NUM_OF_DATSIZE;
		gBF_Fs_Drive_Tbl[media_id].cache = &gBF_Fs_cache_set[media_id];
		drv_tbl[drive_num] = &gBF_Fs_Drive_Tbl[media_id];

		drive_num++;
		if(drive_num >= gEM_Drive_Num) {
			break;
		}
	}
	drv_tbl[drive_num] = NULL; // end of driver table

	/* attach driver */
	if ((ercd = pf_attach((PF_DRV_TBL**)&drv_tbl)) != 0) {
		BF_Debug_Print_Error(("pf_attach error(%d)\n", ercd));
		return -1;
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---
	return 0;
}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
FW_INT32 BF_Media_Drv_Wrapper_Finalize_SDIO0(FW_VOID)
{
	FW_INT32 ercd;			// error code

	// SD card power off
	MWSDIO0_Set_Card_Power(0);

	/* detach driver */
	if ((ercd = pf_detach(gBF_Fs_Drive_Tbl[D_BF_FS_CONTROLLER_SD_0].drive)) != 0) {
		return -1;
	}

	/* close a partition */
	if ((ercd = pdm_close_partition(gBF_Fs_Partition_Tbl[D_BF_FS_CONTROLLER_SD_0])) != 0) {
		return -1;
	}

	/* close a disk */
	if ((ercd = pdm_close_disk(gBF_Media_Drv_Wrapper_Disk_Tbl[0])) != 0) {
		return -1;
	}
	gBF_Media_Drv_Wrapper_Disk_Tbl[0] = NULL;

	return ercd;
}

FW_INT32 BF_Media_Drv_Wrapper_Finalize_SDIO1(FW_VOID)
{
	FW_INT32 ercd;			// error code

	// SD card power off
	MWSDIO1_Set_Card_Power(0);

	/* detach driver */
	if ((ercd = pf_detach(gBF_Fs_Drive_Tbl[D_BF_FS_CONTROLLER_SD_1].drive)) != 0) {
		return -1;
	}

	/* close a partition */
	if ((ercd = pdm_close_partition(gBF_Fs_Partition_Tbl[D_BF_FS_CONTROLLER_SD_1])) != 0) {
		return -1;
	}

	/* close a disk */
	if ((ercd = pdm_close_disk(gBF_Media_Drv_Wrapper_Disk_Tbl[1])) != 0) {
		return -1;
	}
	gBF_Media_Drv_Wrapper_Disk_Tbl[1] = NULL;

	return ercd;
}

FW_INT32 BF_Media_Drv_Wrapper_Finalize_NF(FW_VOID)
{
	FW_INT32 ercd;			// error code
	UCHAR media_id;
	FW_INT32 drive_num;

	drive_num = 0;
	for (media_id = D_BF_FS_CONTROLLER_NF_0; media_id <= D_BF_FS_CONTROLLER_NF_6; media_id++) {
		// unmount(fail safe)
		(FW_VOID)pf_unmount(gBF_Fs_Drive_Tbl[media_id].drive, PF_UNMOUNT_NORMAL);

		/* detach driver */
		if ((ercd = pf_detach(gBF_Fs_Drive_Tbl[media_id].drive)) != 0) {
			return -1;
		}
		/* close a partition */
		if ((ercd = pdm_close_partition(gBF_Fs_Partition_Tbl[media_id])) != 0) {
			return -1;
		}

		drive_num++;
		if(drive_num >= gNF_Drive_Num) {
			break;
		}
	}

	/* close a disk */
	if ((ercd = pdm_close_disk(gBF_Media_Drv_Wrapper_Disk_Tbl[2])) != 0) {
		return -1;
	}
	gBF_Media_Drv_Wrapper_Disk_Tbl[2] = NULL;

	return ercd;
}

FW_INT32 BF_Media_Drv_Wrapper_Finalize_EMMC(FW_VOID)
{
	FW_INT32 ercd;			// error code
	UCHAR media_id;
	FW_INT32 drive_num;

	drive_num = 0;
	for (media_id = D_BF_FS_CONTROLLER_EMMC_0; media_id < D_BF_FS_CONTROLLER_MEDIA_ID_NUM; media_id++) {
		// unmount(fail safe)
		(FW_VOID)pf_unmount(gBF_Fs_Drive_Tbl[media_id].drive, PF_UNMOUNT_NORMAL);

		/* detach driver */
		if ((ercd = pf_detach(gBF_Fs_Drive_Tbl[media_id].drive)) != 0) {
			return -1;
		}
		/* close a partition */
		if ((ercd = pdm_close_partition(gBF_Fs_Partition_Tbl[media_id])) != 0) {
			return -1;
		}

		drive_num++;
		if(drive_num >= gEM_Drive_Num) {
			break;
		}
	}

	/* close a disk */
	if ((ercd = pdm_close_disk(gBF_Media_Drv_Wrapper_Disk_Tbl[3])) != 0) {
		return -1;
	}
	gBF_Media_Drv_Wrapper_Disk_Tbl[3] = NULL;

	return ercd;
}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---


FW_INT32 BF_Media_Drv_Wrapper_Format_SDIO0(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info)
{
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
	FW_INT32 ercd;			// error code

	*driver_tbl = (T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWSDIO0_driver;
	*media_info = (FW_VOID*)&gSD0_dev_mng;

	/* SD physical format */
	ercd = MWSDIO0_PhyFormat((T_SD0_MNG*)*media_info);
	if (ercd != 0) {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Debug_Print_Error(("MWSDIO0_PhyFormat() failed(%d)\n\r", ercd));
#endif
		return ercd;
	}

	/* clear FAT and root direcotry */
	ercd = BF_Mdf_Wrapper_Nformat((T_FS_FUNCNAME*)*driver_tbl, *media_info);
	if (ercd < 0) {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Debug_Print_Error(("BF_Mdf_Wrapper_Nformat() failed(%d)\n", ercd));
#endif
		return ercd;
	}
#endif	// for Linux Co-op

	return 0;
}

FW_INT32 BF_Media_Drv_Wrapper_Format_SDIO1(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info)
{
#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
	FW_INT32 ercd;			// error code

	*driver_tbl = (T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWSDIO1_driver;
	*media_info = (FW_VOID*)&gSD1_dev_mng;

	/* SD physical format */
	ercd = MWSDIO1_PhyFormat((T_SD1_MNG*)*media_info);
	if (ercd != 0) {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Debug_Print_Error(("MWSDIO1_PhyFormat() failed(%d)\n\r", ercd));
#endif
		return ercd;
	}

	/* clear FAT and root direcotry */
	ercd = BF_Mdf_Wrapper_Nformat((T_FS_FUNCNAME*)*driver_tbl, *media_info);
	if (ercd < 0) {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Debug_Print_Error(("BF_Mdf_Wrapper_Nformat() failed(%d)\n", ercd));
#endif
		return ercd;
	}
#endif	// for Linux Co-op

	return 0;
}

FW_INT32 BF_Media_Drv_Wrapper_Format_NF(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info)
{
	FW_INT32 ercd;
	FW_UINT32 w_addr;

	w_addr = (FW_UINT32)gMW_Exs_IRAM_NF;
	ercd = MWNF_Format_Drive( E_MWNF_I_DRIVE );	// "I" FullFormat
	if (ercd != 0) {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Debug_Print_Error(("BF_Media_Drv_Wrapper_Format_NF() route:001. failed(%d)\n\r", ercd));
#endif
		return ercd;
	}
	ercd = MWNF_BasicInit(w_addr, 1);
	if (ercd != 0) {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Debug_Print_Error(("BF_Media_Drv_Wrapper_Format_NF() route:002. failed(%d)\n\r", ercd));
#endif
		return ercd;
	}

	*driver_tbl = (T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWNF_driver;
	*media_info = (FW_VOID*)&NFdevInfo[0];
	return 0;
}
	
FW_INT32 BF_Media_Drv_Wrapper_Format_EMMC(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info)
{
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
	FW_INT32 ercd;
	UINT32 drive_no;

	ercd = MWEM_Format();
	if (ercd != 0) {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Debug_Print_Error(("BF_Media_Drv_Wrapper_Format_EMMC() route:001. failed(%d)\n\r", ercd));
#endif
		return ercd;
	}

	for (drive_no = 0; drive_no < gEM_Drive_Num; drive_no++) {
		/* clear FAT and root direcotry */
		ercd = BF_Mdf_Wrapper_Nformat((T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWEM_driver, (FW_VOID*)&EMdevInfo[drive_no]);
		if (ercd < 0) {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
			BF_Debug_Print_Error(("BF_Mdf_Wrapper_Nformat() failed(%d)\n", ercd));
#endif
			return ercd;
		}
	}
#endif	// for Linux Co-op

	*driver_tbl = (T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWEM_driver;
	*media_info = (FW_VOID*)&EMdevInfo[0];
	return 0;
}

FW_INT32 BF_Media_Drv_Wrapper_Check_MBR(FW_VOID)
{
	FW_INT32 ercd;			// error code
	FW_UINT32 w_addr;

	w_addr = (FW_UINT32)gMW_Exs_IRAM_NF;
	// NF interrupt(ext3) setting
	ercd = MWNF_BasicInit(w_addr, 1);
	if (ercd != 0) {
		BF_Debug_Print_Error(("BF_Media_Drv_Wrapper_Check_MBR() route:001. failed(%d)\n\r", ercd));
		return ercd;
	}

	ercd = MWNF_Check_MBR();
	if (ercd != 0) {
		BF_Debug_Print_Error(("BF_Media_Drv_Wrapper_Check_MBR() route:002. failed(%d)\n\r", ercd));
	}

	return ercd;
}

FW_INT32 BF_Media_Drv_Wrapper_Check_MBR_EMMC(FW_VOID)
{
	FW_INT32 ercd;			// error code

	ercd = MWEM_DeviceInit();
	if (ercd != 0) {
		BF_Debug_Print_Error(("BF_Media_Drv_Wrapper_Check_MBR_EMMC() route:001. failed(%d)\n\r", ercd));
		return ercd;
	}

	ercd = MWEM_Check_MBR();
	if (ercd != 0) {
		BF_Debug_Print_Error(("BF_Media_Drv_Wrapper_Check_MBR_EMMC() route:002. failed(%d)\n\r", ercd));
	}

	return ercd;
}

FW_INT32 BF_Media_Drv_Wrapper_Write_MBR(FW_VOID)
{
	FW_INT32 ercd;			// error code

	ercd = MWNF_Write_MBR();
	return ercd;
}

FW_INT32 BF_Media_Drv_Wrapper_Write_MBR_EMMC(FW_VOID)
{
	FW_INT32 ercd;			// error code

	ercd = MWEM_Write_MBR();
	return ercd;
}

FW_VOID BF_Media_Drv_Wrapper_Get_Media_Format_Type_SDIO0(FW_UCHAR* used_media, FW_UCHAR* format_type)
{
	if (gSD0_dev_mng.capacity >= (32 * 1024)) {
		if (gSD0_dev_mng.sd_spec == 3) {
			// SDXC
			*used_media = FJ_USED_MEDIA_SDXC;
		}
		else {
			// SDHC
			*used_media = FJ_USED_MEDIA_SDHC;
		}
	}
	else if (gSD0_dev_mng.capacity >= (2 * 1024)) {
		// SDHC
		*used_media = FJ_USED_MEDIA_SDHC;
	}
	else {
		// SD
		*used_media = FJ_USED_MEDIA_SD;
	}

	*format_type = gSD0_dev_mng.ostype;
}

FW_VOID BF_Media_Drv_Wrapper_Get_Media_Format_Type_SDIO1(FW_UCHAR* used_media, FW_UCHAR* format_type)
{
	if (gSD1_dev_mng.capacity >= (32 * 1024)) {
		if (gSD1_dev_mng.sd_spec == 3) {
			// SDXC
			*used_media = FJ_USED_MEDIA_SDXC;
		}
		else {
			// SDHC
			*used_media = FJ_USED_MEDIA_SDHC;
		}
	}
	else if (gSD1_dev_mng.capacity >= (2 * 1024)) {
		// SDHC
		*used_media = FJ_USED_MEDIA_SDHC;
	}
	else {
		// SD
		*used_media = FJ_USED_MEDIA_SD;
	}

	*format_type = gSD1_dev_mng.ostype;
}

/////////////////////////////////////////////////////
// romarea_last
/////////////////////////////////////////////////////
//#pragma arm section code="BASEFW_LAST_CODE"

FW_VOID BF_Media_Drv_Wrapper_Get_Drv_Info_SDIO0(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info)
{
	*driver_tbl = (T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWSDIO0_driver;
	*media_info = (FW_VOID*)&gSD0_dev_mng;
}

FW_VOID BF_Media_Drv_Wrapper_Get_Drv_Info_SDIO1(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info)
{
	*driver_tbl = (T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWSDIO1_driver;
	*media_info = (FW_VOID*)&gSD1_dev_mng;
}

FW_VOID BF_Media_Drv_Wrapper_Get_Drv_Info_NF(FW_INT32 drive_no, T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info)
{
	*driver_tbl = (T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWNF_driver;
	*media_info = (FW_VOID*)&NFdevInfo[drive_no];
}

FW_VOID BF_Media_Drv_Wrapper_Get_Drv_Info_EMMC(FW_INT32 drive_no, T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info)
{
	*driver_tbl = (T_FS_FUNCNAME*)&gBF_Media_Drv_Wrapper_MWEM_driver;
	*media_info = (FW_VOID*)&EMdevInfo[(drive_no - D_BF_FS_CONTROLLER_EMMC_0)];
}


FW_INT32 BF_Media_Drv_Wrapper_Set_User_Setting(FW_UINT32 addr, FW_UINT32 size)
{
	FW_INT32 ercd;			// error code
	FW_UINT32 i, j;
	FW_UINT32 selector;
	FW_UINT32 valid_block_nums;
	FW_UINT32 latest_block_index;
	FW_UINT32 max_count = 0;
	FW_UINT32 write_block = 0xFF;
	FW_UINT32 last_count_flag = 0;
	FW_UINT32 ring_block_num = 0xFF;
	FW_UINT32 tmp_ptr[4], tmp_count = 0;
	FW_UINT32 user_setting_data_writed_block[D_BF_USER_SETTING_DATA_RAW_AREA_NUMS];
	FW_UINT32 user_setting_data_counts[D_BF_USER_SETTING_DATA_RAW_AREA_NUMS] = { 0 };
	FW_UINT32 seek_block;
	FW_UINT32 write_page;
	FW_UINT32 page_num;
	T_NF_DEV_INFO dev_info;

	selector = gNF_partition_table[E_MEDIA_NF_PARTITION_DATA9].selector;

	// check selector
	if( selector >= 2 ) {
		BF_Debug_Print_Error(("ERROR :BF_Media_Drv_Wrapper_Set_User_Setting(). selector=%d\n", selector));
		return -1;
	}
	// get NAND Flash device information
	ercd = MWNF_Get_Dev_Info(&dev_info);
	if (ercd != 0) {
		return -1;
	}
	// set ecc capability
	MWNF_Set_Ecc_Param(D_MWNF_ENABLE, D_MWNF_ECC_AUTO);

	if( selector != gUserData_latest_area ) {
		// reset latest block
		gUserData_latest_block = 0xFF;
		gUserData_latest_area  = selector;
	}

	// search to LUT for Partition Table
	valid_block_nums   = 0;
	latest_block_index = 0xFF;
	memset(user_setting_data_writed_block, 0xFF, sizeof(user_setting_data_writed_block));
	for( i = 0; i < D_BF_USER_SETTING_DATA_RAW_AREA_NUMS; i++ ) {
		if( gNF_partition_table[E_MEDIA_NF_PARTITION_DATA9].area[selector].lut[i] == D_BF_USER_SETTING_DATA_BLOCK_MAX_NUMS ) {
			break;
		}
		user_setting_data_writed_block[i] = gNF_partition_table[E_MEDIA_NF_PARTITION_DATA9].area[selector].lut[i];

		if(user_setting_data_writed_block[i] == gUserData_latest_block ) {
			latest_block_index = i;
		}
		valid_block_nums = i + 1;
	}
	if( latest_block_index == 0xFF ) {
		gUserData_latest_block = 0xFF;
	}

	// latest block exist
	if( gUserData_latest_block != 0xFF ){
		ercd = MWNF_ReadPages( gUserData_latest_block, 0, 1, (BYTE*)gUserData_NFData);
		if (ercd != 0) {
			// reset updated parameter
			gUserData_latest_block = 0xFF;
		}
		else {
			tmp_ptr[0] = ((UINT32)gUserData_NFData[0] << 24);
			tmp_ptr[1] = ((UINT32)gUserData_NFData[1] << 16);
			tmp_ptr[2] = ((UINT32)gUserData_NFData[2] << 8);
			tmp_ptr[3] = ((UINT32)gUserData_NFData[3]);

			tmp_count = tmp_ptr[0] + tmp_ptr[1] + tmp_ptr[2] + tmp_ptr[3];
			user_setting_data_counts[latest_block_index] = tmp_count;

			if( tmp_count != D_BF_USER_SETTING_DATA_CLEAR_COUNTS ){
				max_count = tmp_count;
				write_block = latest_block_index;

				if( tmp_count == D_BF_USER_SETTING_DATA_MAX_COUNTS ){
					last_count_flag = 1;
				}
			}
			else{
				// reset updated parameter
				gUserData_latest_block = 0xFF;
			}
		}
	}

	// latest block not exist or latest block is last count
	if(( gUserData_latest_block == 0xFF ) || ( last_count_flag == 1 )){
		// read already writed data
		for( i = 0; i < valid_block_nums; i++ ){
			ercd = MWNF_ReadPages( user_setting_data_writed_block[i], 0, 1, (BYTE*)gUserData_NFData);
			if (ercd != 0) {
				// go to next block
				continue;
			}

			tmp_ptr[0] = ((UINT32)gUserData_NFData[0] << 24);
			tmp_ptr[1] = ((UINT32)gUserData_NFData[1] << 16);
			tmp_ptr[2] = ((UINT32)gUserData_NFData[2] << 8);
			tmp_ptr[3] = ((UINT32)gUserData_NFData[3]);

			tmp_count = tmp_ptr[0] + tmp_ptr[1] + tmp_ptr[2] + tmp_ptr[3];
			user_setting_data_counts[i] = tmp_count;

			if( tmp_count != D_BF_USER_SETTING_DATA_CLEAR_COUNTS ){
				if( max_count <= tmp_count ){
					if(( tmp_count != D_BF_USER_SETTING_DATA_MAX_COUNTS ) || ( last_count_flag == 0 )){
						max_count = tmp_count;
						write_block = i;
					}
				}
			}
			if( tmp_count == D_BF_USER_SETTING_DATA_MAX_COUNTS ){
				last_count_flag = 1;
			}
		}
	}

	// determine max count
	if( last_count_flag == 0 ){		// last count not existed
		max_count = max_count + 1;
	}
	else{							// last count existed
		tmp_count = 0;
		for( i = 0; i < valid_block_nums; i++ ){
			if( user_setting_data_counts[i] <= ( valid_block_nums - 1) ){
				if( tmp_count <= user_setting_data_counts[i] ){
					tmp_count = user_setting_data_counts[i];
					ring_block_num = i;
				}
			}
		}
		if( ring_block_num == 0xFF ){
			max_count = 1;
		}
		else{
			max_count = tmp_count + 1;
		}
	}

	// determine write block index
	if( ring_block_num != 0xFF ){
		if( tmp_count == ( valid_block_nums - 1 ) ){
			if( write_block >= D_BF_USER_SETTING_DATA_RAW_AREA_NUMS ){
				// fale safe
				write_block = 0;
			}
			else{
				// write block not change
			}
		}
		else if( ring_block_num == ( valid_block_nums - 1 ) ){
			write_block = 0;
		}
		else{
			write_block = (ring_block_num + 1);
		}
	}
	else{
		if(( write_block == ( valid_block_nums - 1 ) ) || (write_block >= D_BF_USER_SETTING_DATA_RAW_AREA_NUMS)){
			write_block = 0;
		}
		else{
			write_block = (write_block + 1);
		}
	}

	// Specified counter exists
	if( gUserData_save_data_count != 0xFFFFFFFF) {
		max_count = gUserData_save_data_count;
	}

	// Initialize the write data
	memset((VOID*)gUserData_NFData, 0xFF, dev_info.page_size);

	for( i = 0; i < D_BF_USER_SETTING_DATA_RAW_AREA_NUMS; i++, write_block++ ) {
		// determine write block
		if( write_block >= D_BF_USER_SETTING_DATA_RAW_AREA_NUMS ){
			write_block = 0;
		}
		seek_block = user_setting_data_writed_block[write_block];

		// write set setting data
		ercd = MWNF_EraseBlk(seek_block);
		if (ercd != 0) {
			continue;
		}
		gUserData_NFData[0] = (FW_UCHAR)((max_count & 0xFF000000 ) >> 24);
		gUserData_NFData[1] = (FW_UCHAR)((max_count & 0x00FF0000 ) >> 16);
		gUserData_NFData[2] = (FW_UCHAR)((max_count & 0x0000FF00 ) >> 8);
		gUserData_NFData[3] = (FW_UCHAR)( max_count & 0x000000FF );
		gUserData_NFData[4] = (FW_UCHAR)((size & 0xFF000000 ) >> 24);
		gUserData_NFData[5] = (FW_UCHAR)((size & 0x00FF0000 ) >> 16);
		gUserData_NFData[6] = (FW_UCHAR)((size & 0x0000FF00 ) >> 8);
		gUserData_NFData[7] = (FW_UCHAR)( size & 0x000000FF );

		// write page0
		ercd = MWNF_WritePages_NoRetry( seek_block, 0, 1, (BYTE*)gUserData_NFData);
		if (ercd != 0) {
			continue;
		}
		page_num = 1;

		// write page1-
		if ((size % dev_info.page_size) == 0) {
			write_page = (size / dev_info.page_size);
		}
		else {
			write_page = (size / dev_info.page_size) + 1;
		}
		ercd = MWNF_WritePages_NoRetry( seek_block, page_num, write_page, (BYTE*)addr);
		if (ercd != 0) {
			continue;
		}
		page_num += write_page;

		// write ALLF to other pages
		for( j = 0; j < 8; j++ ){
			gUserData_NFData[j] = 0xFF;
		}
		for(; page_num < dev_info.page_per_blk; page_num++) {
			ercd = MWNF_WritePages_NoRetry( seek_block, page_num, 1, (BYTE*)gUserData_NFData);
			if (ercd != 0) {
				break;
			}
		}
		if (ercd != 0) {
			continue;
		}
		// save latest block
		gUserData_latest_block = seek_block;
		break;
	}

	// set ecc capability
	MWNF_Set_Ecc_Param(D_MWNF_ENABLE, D_MWNF_ECC_AUTO);

	if (ercd == 0) {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Fs_User_Custom_Info_Printer(("user setting data update\n"));
		BF_Fs_User_Custom_Info_Printer((" - save block : 0x%x\n", gUserData_latest_block));
		BF_Fs_User_Custom_Info_Printer((" - counter    : 0x%x\n", max_count));
#endif
		return 0;
	}
	else {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Fs_User_Custom_Info_Printer(("user setting data update failed.(%d)\n", ercd));
#endif
		return -1;
	}
}

FW_INT32 BF_Media_Drv_Wrapper_Set_User_Setting_eMMC(FW_UINT32 addr, FW_UINT32 size)
{
	FW_INT32 ercd;			// error code
	FW_UINT32 i, j;
	FW_UINT32 selector;
	FW_UINT32 valid_block_nums;
	FW_UINT32 latest_block_index;
	FW_UINT32 max_count = 0;
	FW_UINT32 write_block = 0xFF;
	FW_UINT32 last_count_flag = 0;
	FW_UINT32 ring_block_num = 0xFF;
	FW_UINT32 tmp_ptr[4], tmp_count = 0;
	FW_UINT32 user_setting_data_writed_block[D_BF_USER_SETTING_DATA_RAW_AREA_NUMS];
	FW_UINT32 user_setting_data_counts[D_BF_USER_SETTING_DATA_RAW_AREA_NUMS] = { 0 };
	FW_UINT32 seek_block;
	FW_UINT32 sector_no;
	FW_UINT32 sector_num;

	selector = gNF_partition_table[E_MEDIA_NF_PARTITION_DATA9].selector;

	// check selector
	if( selector >= 2 ) {
		BF_Debug_Print_Error(("ERROR :BF_Media_Drv_Wrapper_Set_User_Setting_eMMC(). selector=%d\n", selector));
		return -1;
	}

	if( selector != gUserData_latest_area ) {
		// reset latest block
		gUserData_latest_block = 0xFF;
		gUserData_latest_area  = selector;
	}

	// search to LUT for Partition Table
	valid_block_nums   = 0;
	latest_block_index = 0xFF;
	memset(user_setting_data_writed_block, 0xFF, sizeof(user_setting_data_writed_block));
	for( i = 0; i < D_BF_USER_SETTING_DATA_RAW_AREA_NUMS; i++ ) {
		if( gNF_partition_table[E_MEDIA_NF_PARTITION_DATA9].area[selector].sector_num == 0 ) {
			break;
		}
		user_setting_data_writed_block[i] = (gNF_partition_table[E_MEDIA_NF_PARTITION_DATA9].area[selector].sector_num + 
											 (D_BF_USER_SETTING_DATA_TOTAL_SECTOR * i));

		if(user_setting_data_writed_block[i] == gUserData_latest_block ) {
			latest_block_index = i;
		}
		valid_block_nums = i + 1;
	}
	if( latest_block_index == 0xFF ) {
		gUserData_latest_block = 0xFF;
	}

	// latest block exist
	if( gUserData_latest_block != 0xFF ){
		ercd = MWEM_DirectRead( gUserData_latest_block, 1, (char*)gUserData_NFData );
		if (ercd != 0) {
			// reset updated parameter
			gUserData_latest_block = 0xFF;
		}
		else {
			tmp_ptr[0] = ((UINT32)gUserData_NFData[0] << 24);
			tmp_ptr[1] = ((UINT32)gUserData_NFData[1] << 16);
			tmp_ptr[2] = ((UINT32)gUserData_NFData[2] << 8);
			tmp_ptr[3] = ((UINT32)gUserData_NFData[3]);

			tmp_count = tmp_ptr[0] + tmp_ptr[1] + tmp_ptr[2] + tmp_ptr[3];
			user_setting_data_counts[latest_block_index] = tmp_count;

			if( tmp_count != D_BF_USER_SETTING_DATA_CLEAR_COUNTS ){
				max_count = tmp_count;
				write_block = latest_block_index;

				if( tmp_count == D_BF_USER_SETTING_DATA_MAX_COUNTS ){
					last_count_flag = 1;
				}
			}
			else{
				// reset updated parameter
				gUserData_latest_block = 0xFF;
			}
		}
	}

	// latest block not exist or latest block is last count
	if(( gUserData_latest_block == 0xFF ) || ( last_count_flag == 1 )){
		// read already writed data
		for( i = 0; i < valid_block_nums; i++ ){
			ercd = MWEM_DirectRead( user_setting_data_writed_block[i], 1, (char*)gUserData_NFData );
			if (ercd != 0) {
				// go to next block
				continue;
			}

			tmp_ptr[0] = ((UINT32)gUserData_NFData[0] << 24);
			tmp_ptr[1] = ((UINT32)gUserData_NFData[1] << 16);
			tmp_ptr[2] = ((UINT32)gUserData_NFData[2] << 8);
			tmp_ptr[3] = ((UINT32)gUserData_NFData[3]);

			tmp_count = tmp_ptr[0] + tmp_ptr[1] + tmp_ptr[2] + tmp_ptr[3];
			user_setting_data_counts[i] = tmp_count;

			if( tmp_count != D_BF_USER_SETTING_DATA_CLEAR_COUNTS ){
				if( max_count <= tmp_count ){
					if(( tmp_count != D_BF_USER_SETTING_DATA_MAX_COUNTS ) || ( last_count_flag == 0 )){
						max_count = tmp_count;
						write_block = i;
					}
				}
			}
			if( tmp_count == D_BF_USER_SETTING_DATA_MAX_COUNTS ){
				last_count_flag = 1;
			}
		}
	}

	// determine max count
	if( last_count_flag == 0 ){		// last count not existed
		max_count = max_count + 1;
	}
	else{							// last count existed
		tmp_count = 0;
		for( i = 0; i < valid_block_nums; i++ ){
			if( user_setting_data_counts[i] <= ( valid_block_nums - 1) ){
				if( tmp_count <= user_setting_data_counts[i] ){
					tmp_count = user_setting_data_counts[i];
					ring_block_num = i;
				}
			}
		}
		if( ring_block_num == 0xFF ){
			max_count = 1;
		}
		else{
			max_count = tmp_count + 1;
		}
	}

	// determine write block index
	if( ring_block_num != 0xFF ){
		if( tmp_count == ( valid_block_nums - 1 ) ){
			if( write_block >= D_BF_USER_SETTING_DATA_RAW_AREA_NUMS ){
				// fale safe
				write_block = 0;
			}
			else{
				// write block not change
			}
		}
		else if( ring_block_num == ( valid_block_nums - 1 ) ){
			write_block = 0;
		}
		else{
			write_block = (ring_block_num + 1);
		}
	}
	else{
		if(( write_block == ( valid_block_nums - 1 ) ) || (write_block >= D_BF_USER_SETTING_DATA_RAW_AREA_NUMS)){
			write_block = 0;
		}
		else{
			write_block = (write_block + 1);
		}
	}

	// Specified counter exists
	if( gUserData_save_data_count != 0xFFFFFFFF) {
		max_count = gUserData_save_data_count;
	}

	// Initialize the write data
	memset((VOID*)gUserData_NFData, 0xFF, sizeof(gUserData_NFData));

	for( i = 0; i < D_BF_USER_SETTING_DATA_RAW_AREA_NUMS; i++, write_block++ ) {
		// determine write block
		if( write_block >= D_BF_USER_SETTING_DATA_RAW_AREA_NUMS ){
			write_block = 0;
		}
		seek_block = user_setting_data_writed_block[write_block];

		gUserData_NFData[0] = (FW_UCHAR)((max_count & 0xFF000000 ) >> 24);
		gUserData_NFData[1] = (FW_UCHAR)((max_count & 0x00FF0000 ) >> 16);
		gUserData_NFData[2] = (FW_UCHAR)((max_count & 0x0000FF00 ) >> 8);
		gUserData_NFData[3] = (FW_UCHAR)( max_count & 0x000000FF );
		gUserData_NFData[4] = (FW_UCHAR)((size & 0xFF000000 ) >> 24);
		gUserData_NFData[5] = (FW_UCHAR)((size & 0x00FF0000 ) >> 16);
		gUserData_NFData[6] = (FW_UCHAR)((size & 0x0000FF00 ) >> 8);
		gUserData_NFData[7] = (FW_UCHAR)( size & 0x000000FF );

		// write information
		sector_no  = seek_block;
		sector_num = (D_BF_USER_SETTING_DATA_INFO_SIZE / EM_CONFIG_DEFAULT_BLOCK_SIZE);
		ercd = MWEM_DirectWrite( sector_no, sector_num, (char*)gUserData_NFData );
		if (ercd != 0) {
			continue;
		}
		sector_no += sector_num;

		// write data
		if ((size % EM_CONFIG_DEFAULT_BLOCK_SIZE) == 0) {
			sector_num = (size / EM_CONFIG_DEFAULT_BLOCK_SIZE);
		}
		else {
			sector_num = (size / EM_CONFIG_DEFAULT_BLOCK_SIZE) + 1;
		}
		ercd = MWEM_DirectWrite( sector_no, sector_num, (char*)addr );
		if (ercd != 0) {
			continue;
		}
		sector_no += sector_num;

		// write ALLF to other pages
		if ( D_BF_USER_SETTING_DATA_TOTAL_SECTOR > sector_no ) {
			sector_num = (D_BF_USER_SETTING_DATA_TOTAL_SECTOR - sector_no);
			for( j = 0; j < (sector_num * EM_CONFIG_DEFAULT_BLOCK_SIZE); j++ ){
				gUserData_NFData[j] = 0xFF;
			}
			ercd = MWEM_DirectWrite( sector_no, sector_num, (char*)gUserData_NFData );
			if (ercd != 0) {
				continue;
			}
		}

		// save latest block
		gUserData_latest_block = seek_block;
		break;
	}

	if (ercd == 0) {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Fs_User_Custom_Info_Printer(("user setting data update\n"));
		BF_Fs_User_Custom_Info_Printer((" - save block : 0x%x\n", gUserData_latest_block));
		BF_Fs_User_Custom_Info_Printer((" - counter    : 0x%x\n", max_count));
#endif
		return 0;
	}
	else {
#ifdef CO_BF_MEDIA_DRV_WRAPPER_DEBUG_PRINT
		BF_Fs_User_Custom_Info_Printer(("user setting data update failed.(%d)\n", ercd));
#endif
		return -1;
	}
}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
/**
 * PrFILE2 media driver Init
 */
FW_INT32 BF_Media_Drv_Wrapper_Init(PDM_DISK* pDisk)
{
	// insert notify fixed
	pdm_disk_notify_media_insert(pDisk);
	return 0;
}

/**
 * PrFILE2 media driver Finalize
 */
FW_INT32 BF_Media_Drv_Wrapper_Finalize(PDM_DISK* pDisk)
{
	// don't use PrFILE2 API
	return 0;
}

/**
 * PrFILE2 media driver Mount
 */
FW_INT32 BF_Media_Drv_Wrapper_Mount(PDM_DISK* pDisk)
{
	// don't use PrFILE2 API
	return 0;
}

/**
 * PrFILE2 media driver Unmount
 */
FW_INT32 BF_Media_Drv_Wrapper_Unmount(PDM_DISK* pDisk)
{
	// don't use PrFILE2 API
	return 0;
}

/**
 * PrFILE2 media driver Format
 */
FW_INT32 BF_Media_Drv_Wrapper_Format(PDM_DISK* pDisk, const FW_UCHAR* param)
{
	return 0;
}

/**
 * PrFILE2 media driver Read
 */
FW_INT32 BF_Media_Drv_Wrapper_Read_SDIO0(PDM_DISK* pDisk, FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum)
{
	FW_INT32   ercd;
	FW_ULONG   errsec;
	FW_UCHAR   err;

	ercd = MWSDIO0_ReadSectors(block, num_block, (UCHAR*)buf, &errsec, &err);
	if (ercd == 0) {
		*pNum = num_block;
	}
	else {
		*pNum = 0;
//		if (ercd == D_SD0_ERR_NOCARD) {
//			ercd = FS_DRV_ERR_INVALID_DATA;
//		}
	}

	return ercd;
}

FW_INT32 BF_Media_Drv_Wrapper_Read_SDIO1(PDM_DISK* pDisk, FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum)
{
	FW_INT32   ercd;
	FW_ULONG   errsec;
	FW_UCHAR   err;

	ercd = MWSDIO1_ReadSectors(block, num_block, (UCHAR*)buf, &errsec, &err);
	if (ercd == 0) {
		*pNum = num_block;
	}
	else {
		*pNum = 0;
//		if (ercd == D_SD1_ERR_NOCARD) {
//			ercd = FS_DRV_ERR_INVALID_DATA;
//		}
	}
//	BF_Fs_User_Custom_Info_Printer(("--- SD read - ret(%d) buf(0x%x) sec(0x%x) num(%d)\n",ercd, buf, block, num_block));

	return ercd;
}

FW_INT32 BF_Media_Drv_Wrapper_Read_NF(PDM_DISK* pDisk, FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum)
{
	FW_INT32   ercd;

	ercd = MWNF_BRead(NULL, block, num_block, (CHAR*)buf);
	if (ercd == 0) {
		*pNum = num_block;
	}
	else {
		*pNum = 0;
	}

	return ercd;
}

FW_INT32 BF_Media_Drv_Wrapper_Read_EMMC(PDM_DISK* pDisk, FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum)
{
	FW_INT32   ercd;

	if(block == 0) {
		block = gEM_MBR_Sector;
	}

	ercd = MWEM_DirectRead(block, num_block, (CHAR*)buf);
	if (ercd == 0) {
		*pNum = num_block;
	}
	else {
		*pNum = 0;
	}

	return ercd;
}

/**
 * PrFILE2 media driver Write
 */
FW_INT32 BF_Media_Drv_Wrapper_Write_SDIO0(PDM_DISK* pDisk, const FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum)
{
	FW_INT32   ercd;
	FW_ULONG   errsec;
	FW_UCHAR   err;

	ercd = MWSDIO0_WriteSectors(block, num_block, (UCHAR*)buf, &errsec, &err);
	if (ercd == 0) {
		*pNum = num_block;
	}
	else {
		*pNum = 0;
//		if (ercd == D_SD0_ERR_NOCARD) {
//			ercd = FS_DRV_ERR_INVALID_DATA;
//		}
	}

	return ercd;
}

FW_INT32 BF_Media_Drv_Wrapper_Write_SDIO1(PDM_DISK* pDisk, const FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum)
{
	FW_INT32   ercd;
	FW_ULONG   errsec;
	FW_UCHAR   err;

	ercd = MWSDIO1_WriteSectors(block, num_block, (UCHAR*)buf, &errsec, &err);
	if (ercd == 0) {
		*pNum = num_block;
	}
	else {
		*pNum = 0;
//		if (ercd == D_SD0_ERR_NOCARD) {
//			ercd = FS_DRV_ERR_INVALID_DATA;
//		}
	}
//	BF_Fs_User_Custom_Info_Printer(("--- SD write - ret(%d) buf(0x%x) sec(0x%x) num(%d)\n",ercd, buf, block, num_block));

	return ercd;
}

FW_INT32 BF_Media_Drv_Wrapper_Write_NF(PDM_DISK* pDisk, const FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum)
{
	FW_INT32   ercd;

	ercd = MWNF_BWrite(NULL, block, num_block, (CHAR*)buf);
	if (ercd == 0) {
		*pNum = num_block;
	}
	else {
		*pNum = 0;
	}

	return ercd;
}

FW_INT32 BF_Media_Drv_Wrapper_Write_EMMC(PDM_DISK* pDisk, const FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum)
{
	FW_INT32   ercd;

	ercd = MWEM_DirectWrite(block, num_block, (CHAR*)buf);
	if (ercd == 0) {
		*pNum = num_block;
	}
	else {
		*pNum = 0;
	}

//	BF_Fs_User_Custom_Info_Printer(("\n **TEST** [Write_EM] a:0x%08lx, b:%ld, nb:%ld(0x%08lx) ercd(%d)\n", (ULONG)buf, (ULONG)block, (ULONG)num_block, (ULONG)num_block, ercd));

	return ercd;
}

/**
 * PrFILE2 media driver Get information
 */
FW_INT32 BF_Media_Drv_Wrapper_Get_Info_SDIO0(PDM_DISK* pDisk, PDM_DISK_INFO* pDiskinfo)
{
	pDiskinfo->total_sectors     = gSD0_dev_mng.totalsector;
	pDiskinfo->cylinders         = gSD0_dev_mng.cylinder;
	pDiskinfo->heads             = gSD0_dev_mng.head;
	pDiskinfo->sectors_per_track = gSD0_dev_mng.sectrack;
	pDiskinfo->bytes_per_sector  = gSD0_dev_mng.bytepersector;
	pDiskinfo->media_attr        = PDM_MEDIA_ATTR_REMOVABLE;
	if (gSD0_dev_mng.wp) {
		pDiskinfo->media_attr   |= PDM_MEDIA_ATTR_WRITE_PROTECT;
	}
	pDiskinfo->format_param      = NULL;
	pDiskinfo->physical_block.sectors_per_block = 1;
	pDiskinfo->physical_block.start_sector      = gSD0_dev_mng.pbr_secno;

	return 0;
}

FW_INT32 BF_Media_Drv_Wrapper_Get_Info_SDIO1(PDM_DISK* pDisk, PDM_DISK_INFO* pDiskinfo)
{
	pDiskinfo->total_sectors     = gSD1_dev_mng.totalsector;
	pDiskinfo->cylinders         = gSD1_dev_mng.cylinder;
	pDiskinfo->heads             = gSD1_dev_mng.head;
	pDiskinfo->sectors_per_track = gSD1_dev_mng.sectrack;
	pDiskinfo->bytes_per_sector  = gSD1_dev_mng.bytepersector;
	pDiskinfo->media_attr        = PDM_MEDIA_ATTR_REMOVABLE;
	if (gSD1_dev_mng.wp) {
		pDiskinfo->media_attr   |= PDM_MEDIA_ATTR_WRITE_PROTECT;
	}
	pDiskinfo->format_param      = NULL;
	pDiskinfo->physical_block.sectors_per_block = 1;
	pDiskinfo->physical_block.start_sector      = gSD1_dev_mng.pbr_secno;

	return 0;
}

FW_INT32 BF_Media_Drv_Wrapper_Get_Info_NF(PDM_DISK* pDisk, PDM_DISK_INFO* pDiskinfo)
{
	FW_INT32 drive_num;

	pDiskinfo->total_sectors = 0;
	for (drive_num = 0; drive_num < gNF_Drive_Num; drive_num++) {
		pDiskinfo->total_sectors += NFdevInfo[drive_num].size;
	}
	pDiskinfo->cylinders         = pDiskinfo->total_sectors / (SECTORS_PER_TRACK * TOTAL_HEAD_NUMBER);
	pDiskinfo->heads             = (pDiskinfo->total_sectors / SECTORS_PER_TRACK) % TOTAL_HEAD_NUMBER;
	pDiskinfo->sectors_per_track = SECTORS_PER_TRACK;
	pDiskinfo->bytes_per_sector  = 512;
	pDiskinfo->media_attr        = 0;
	pDiskinfo->format_param      = NULL;
	pDiskinfo->physical_block.sectors_per_block = 4;
	pDiskinfo->physical_block.start_sector      = 0;

	return 0;
}

FW_INT32 BF_Media_Drv_Wrapper_Get_Info_EMMC(PDM_DISK* pDisk, PDM_DISK_INFO* pDiskinfo)
{
	FW_INT32 drive_num;

	pDiskinfo->total_sectors = (gEM_MBR_Sector + D_EM_STORAGE_MBR_SIZE);
	for (drive_num = 0; drive_num < gEM_Drive_Num; drive_num++) {
		pDiskinfo->total_sectors += EMdevInfo[drive_num].size;
	}
	pDiskinfo->cylinders         = pDiskinfo->total_sectors / (SECTORS_PER_TRACK * TOTAL_HEAD_NUMBER);
	pDiskinfo->heads             = (pDiskinfo->total_sectors / SECTORS_PER_TRACK) % TOTAL_HEAD_NUMBER;
	pDiskinfo->sectors_per_track = SECTORS_PER_TRACK;
	pDiskinfo->bytes_per_sector  = 512;
	pDiskinfo->media_attr        = 0;
	pDiskinfo->format_param      = NULL;
	pDiskinfo->physical_block.sectors_per_block = 4;
	pDiskinfo->physical_block.start_sector      = gEM_MBR_Sector;

	return 0;
}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

