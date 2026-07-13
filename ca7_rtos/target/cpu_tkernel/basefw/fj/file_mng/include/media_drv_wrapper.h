/**
 * @file		media_drv_wrapper.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _MEDIA_DRV_WRAPPER_H_
#define _MEDIA_DRV_WRAPPER_H_

#include "fs_user_custom.h"
#include "mdf_wrapper.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* driver init retry counter */
#define D_BF_MEDIA_DRV_WRAPPER_MS_INIT_RETRY_CNT 				(1) /* MWMS_Init() retry counter */
#define D_BF_MEDIA_DRV_WRAPPER_SD_INIT_RETRY_CNT 				(1) /* sdio_init() retry counter */
#define D_BF_MEDIA_DRV_WRAPPER_NF_INIT_RETRY_CNT 				(2) /* nf_init() retry counter */
#define D_BF_MEDIA_DRV_WRAPPER_CF_INIT_RETRY_CNT 				(1) /* cf init retry counter */

#define D_BF_MEDIA_DRV_WRAPPER_MEDIA_TYPE_MS_NORMAL		(0)
#define D_BF_MEDIA_DRV_WRAPPER_MEDIA_TYPE_MS_PRO		(1)

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* write protect status */
typedef enum{
	E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT = 0,	/**< not write protect */
	E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT = 1		/**< write protect */
} E_BF_MEDIA_DRV_WRAPPER_WP;

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special
extern volatile T_FS_FUNCNAME gBF_Media_Drv_Wrapper_MWNF_driver;
extern volatile T_FS_FUNCNAME gBF_Media_Drv_Wrapper_MWSDIO0_driver;
extern volatile T_FS_FUNCNAME gBF_Media_Drv_Wrapper_MWSDIO1_driver;
extern volatile T_FS_FUNCNAME gBF_Media_Drv_Wrapper_MWEM_driver;
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
extern PDM_DISK* gBF_Media_Drv_Wrapper_Disk_Tbl[4];
extern PF_DRV_TBL gBF_Fs_Drive_Tbl[16];
extern PDM_PARTITION* gBF_Fs_Partition_Tbl[16];
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
FW_VOID BF_Media_Drv_Wrapper_Set_Interrupt(int sd_ch);

FW_INT32 BF_Media_Drv_Wrapper_Init_SDIO0(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info, E_BF_MEDIA_DRV_WRAPPER_WP* wp);
FW_VOID BF_Media_Drv_Wrapper_Get_Drv_Info_SDIO0(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info);
FW_VOID BF_Media_Drv_Wrapper_Get_Media_Format_Type_SDIO0(FW_UCHAR* used_media, FW_UCHAR* format_type);

FW_INT32 BF_Media_Drv_Wrapper_Init_SDIO1(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info, E_BF_MEDIA_DRV_WRAPPER_WP* wp);
FW_VOID BF_Media_Drv_Wrapper_Get_Drv_Info_SDIO1(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info);
FW_VOID BF_Media_Drv_Wrapper_Get_Media_Format_Type_SDIO1(FW_UCHAR* used_media, FW_UCHAR* format_type);

FW_INT32 BF_Media_Drv_Wrapper_Init_NF(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info, E_BF_MEDIA_DRV_WRAPPER_WP* wp);
FW_VOID BF_Media_Drv_Wrapper_Get_Drv_Info_NF(FW_INT32 drive_no, T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info);
FW_INT32 BF_Media_Drv_Wrapper_Check_MBR(FW_VOID);
FW_INT32 BF_Media_Drv_Wrapper_Write_MBR(FW_VOID);

FW_INT32 BF_Media_Drv_Wrapper_Init_EMMC(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info, E_BF_MEDIA_DRV_WRAPPER_WP* wp);
FW_VOID BF_Media_Drv_Wrapper_Get_Drv_Info_EMMC(FW_INT32 drive_no, T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info);
FW_INT32 BF_Media_Drv_Wrapper_Check_MBR_EMMC(FW_VOID);
FW_INT32 BF_Media_Drv_Wrapper_Write_MBR_EMMC(FW_VOID);

FW_INT32 BF_Media_Drv_Wrapper_Set_User_Setting(FW_UINT32 addr, FW_UINT32 size);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
FW_INT32 BF_Media_Drv_Wrapper_Set_User_Setting_eMMC(FW_UINT32 addr, FW_UINT32 size);
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

FW_INT32 BF_Media_Drv_Wrapper_Format_SDIO0(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info);
FW_INT32 BF_Media_Drv_Wrapper_Format_SDIO1(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info);
FW_INT32 BF_Media_Drv_Wrapper_Format_NF(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info);
FW_INT32 BF_Media_Drv_Wrapper_Format_EMMC(T_FS_FUNCNAME** driver_tbl, FW_VOID** media_info);
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
FW_INT32 BF_Media_Drv_Wrapper_Finalize_SDIO0(FW_VOID);
FW_INT32 BF_Media_Drv_Wrapper_Finalize_SDIO1(FW_VOID);
FW_INT32 BF_Media_Drv_Wrapper_Finalize_NF(FW_VOID);
FW_INT32 BF_Media_Drv_Wrapper_Finalize_EMMC(FW_VOID);
FW_INT32 BF_Media_Drv_Wrapper_Init(PDM_DISK* pDisk);
FW_INT32 BF_Media_Drv_Wrapper_Finalize(PDM_DISK* pDisk);
FW_INT32 BF_Media_Drv_Wrapper_Mount(PDM_DISK* pDisk);
FW_INT32 BF_Media_Drv_Wrapper_Unmount(PDM_DISK* pDisk);
FW_INT32 BF_Media_Drv_Wrapper_Format(PDM_DISK* pDisk, const FW_UCHAR* param);
FW_INT32 BF_Media_Drv_Wrapper_Read_NF(PDM_DISK* pDisk, FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum);
FW_INT32 BF_Media_Drv_Wrapper_Read_SDIO0(PDM_DISK* pDisk, FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum);
FW_INT32 BF_Media_Drv_Wrapper_Read_SDIO1(PDM_DISK* pDisk, FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum);
FW_INT32 BF_Media_Drv_Wrapper_Read_EMMC(PDM_DISK* pDisk, FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum);
FW_INT32 BF_Media_Drv_Wrapper_Write_NF(PDM_DISK* pDisk, const FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum);
FW_INT32 BF_Media_Drv_Wrapper_Write_SDIO0(PDM_DISK* pDisk, const FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum);
FW_INT32 BF_Media_Drv_Wrapper_Write_SDIO1(PDM_DISK* pDisk, const FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum);
FW_INT32 BF_Media_Drv_Wrapper_Write_EMMC(PDM_DISK* pDisk, const FW_UCHAR* buf, FW_ULONG block, FW_ULONG num_block, FW_ULONG* pNum);
FW_INT32 BF_Media_Drv_Wrapper_Get_Info_NF(PDM_DISK* pDisk, PDM_DISK_INFO* pDiskinfo);
FW_INT32 BF_Media_Drv_Wrapper_Get_Info_SDIO0(PDM_DISK* pDisk, PDM_DISK_INFO* pDiskinfo);
FW_INT32 BF_Media_Drv_Wrapper_Get_Info_SDIO1(PDM_DISK* pDisk, PDM_DISK_INFO* pDiskinfo);
FW_INT32 BF_Media_Drv_Wrapper_Get_Info_EMMC(PDM_DISK* pDisk, PDM_DISK_INFO* pDiskinfo);
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

#endif
