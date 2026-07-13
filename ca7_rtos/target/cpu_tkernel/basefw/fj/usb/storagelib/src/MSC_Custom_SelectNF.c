/**
 * @file		MSC_Custom_SelectNF.c
 * @brief		USB Mass Storage Class Custom function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "msclib.h"
#include "msca_mediachange.h"
#include "msca_media.h"
#include "msca_sdio0.h"
#include "msca_sdio1.h"
#include "msca_sm.h"
#include "msca_emmc.h"
#include "mwusb_driver4.h"
#include "media_attr.h"
#include "fs_if.h"

/*******************/
/* Global Variable */
/*******************/
/* INQUIRY Response data */    /*"1234567890123456"*/
unsigned char MSCS_INQ_VENDOR[]	="SOCIONXT";			/*  8 characters */
unsigned char MSCS_INQ_PRODUCT[]="M Series        ";	/* 16 characters */
unsigned char MSCS_INQ_REVLVL[]	="1.00";				/*  4 characters */

unsigned char MSCS_LUN = (MSCS_MAXLUN);

#define MSC_MEDIA_ID_BLANK  (0xFFFFFFFF)

void MSCA_SelectDrive_SDIO(unsigned char driveno);
void MSCA_SelectDrive_SDIO1(unsigned char driveno);
void MSCA_SelectDrive_MS(unsigned char driveno);
int MSCA_EXT_MediaIntHandler(void);

/* Media drive information */
MSCS_DriveInfo_tp MSCS_DriveInfo[MSCS_MAXLUN] = {

/* Drive no, MSCS_ReadRetryCount, MSCS_WriteRetryCount, MSCA_TransferRate=4Mbps*/
	{0,  MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{1,  MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{2,  MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{3,  MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{4,  MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{5,  MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{6,  MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{7,  MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{8,  MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{9,  MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{10, MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{11, MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{12, MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{13, MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{14, MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
	{15, MSCS_SM_ReadRetryCount, MSCS_SM_WriteRetryCount, MSCA_SM_TransferRate},
};

const MSCS_DriveInfo_tp mscs_drive_info_tbl_nf = {
	0,                                  // Drive no (dummy)
	MSCS_SM_ReadRetryCount,             // MSCS_ReadRetryCount
	MSCS_SM_WriteRetryCount,            // MSCS_WriteRetryCount
	MSCA_SM_TransferRate,               // MSCA_TransferRate = 4Mbps
};

const MSCS_DriveInfo_tp mscs_drive_info_tbl_sd0 = {
	0,                                  // Drive no (dummy)
	MSCS_SDIO0_ReadRetryCount,          // MSCS_ReadRetryCount
	MSCS_SDIO0_WriteRetryCount,         // MSCS_WriteRetryCount
	MSCA_SDIO0_TransferRate,            // MSCA_TransferRate = 4Mbps
};

const MSCS_DriveInfo_tp mscs_drive_info_tbl_sd1 = {
	0,                                  // Drive no (dummy)
	MSCS_SDIO1_ReadRetryCount,          // MSCS_ReadRetryCount
	MSCS_SDIO1_WriteRetryCount,         // MSCS_WriteRetryCount
	MSCA_SDIO1_TransferRate,            // MSCA_TransferRate = 4Mbps
};

const MSCS_DriveInfo_tp mscs_drive_info_tbl_em = {
	0,                                  // Drive no (dummy)
	MSCS_EM_ReadRetryCount,             // MSCS_ReadRetryCount
	MSCS_EM_WriteRetryCount,            // MSCS_WriteRetryCount
	MSCA_EM_TransferRate,               // MSCA_TransferRate = 4Mbps
};


/*  Media Change Flag  */
MSCA_MEDIA_FLAG_T MediaChangeFlag[MSCS_MAXLUN] =
{
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
	{E_BF_MEDIA_STATE_MEDIA_TYPE_NF, -1}, 
};

/*******************************/
/* Media Access function table */
/*******************************/
msca_init_t MSCA_Init[MSCS_MAXLUN] = 
{
	MSCA_SM_Init,
	MSCA_SM_Init,
	MSCA_SM_Init,
	MSCA_SM_Init,
	MSCA_SM_Init,
	MSCA_SM_Init,
	MSCA_SM_Init,
	MSCA_SDIO0_Init,
	MSCA_SDIO1_Init,
	MSCA_EM_Init,
	MSCA_EM_Init,
	MSCA_EM_Init,
	MSCA_EM_Init,
	MSCA_EM_Init,
	MSCA_EM_Init,
	MSCA_EM_Init,
};

msca_reset_t MSCA_Reset[MSCS_MAXLUN] = 
{
	MSCA_SM_Reset,
	MSCA_SM_Reset,
	MSCA_SM_Reset,
	MSCA_SM_Reset,
	MSCA_SM_Reset,
	MSCA_SM_Reset,
	MSCA_SM_Reset,
	MSCA_SDIO0_Reset,
	MSCA_SDIO1_Reset,
	MSCA_EM_Reset,
	MSCA_EM_Reset,
	MSCA_EM_Reset,
	MSCA_EM_Reset,
	MSCA_EM_Reset,
	MSCA_EM_Reset,
	MSCA_EM_Reset,
};

msca_getdriveinfo_t MSCA_GetDriveInfo[MSCS_MAXLUN] = 
{
	MSCA_SM_GetDriveInfo0,
	MSCA_SM_GetDriveInfo1,
	MSCA_SM_GetDriveInfo2,
	MSCA_SM_GetDriveInfo3,
	MSCA_SM_GetDriveInfo4,
	MSCA_SM_GetDriveInfo5,
	MSCA_SM_GetDriveInfo6,
	MSCA_SDIO0_GetDriveInfo,
	MSCA_SDIO1_GetDriveInfo,
	MSCA_EM_GetDriveInfo0,
	MSCA_EM_GetDriveInfo1,
	MSCA_EM_GetDriveInfo2,
	MSCA_EM_GetDriveInfo3,
	MSCA_EM_GetDriveInfo4,
	MSCA_EM_GetDriveInfo5,
	MSCA_EM_GetDriveInfo6,
};

msca_checkmedia_t		MSCA_CheckMedia[MSCS_MAXLUN] = 
{ 
	MSCA_SM_CheckMedia,
	MSCA_SM_CheckMedia,
	MSCA_SM_CheckMedia,
	MSCA_SM_CheckMedia,
	MSCA_SM_CheckMedia,
	MSCA_SM_CheckMedia,
	MSCA_SM_CheckMedia,
	MSCA_SDIO0_CheckMedia,
	MSCA_SDIO1_CheckMedia,
	MSCA_EM_CheckMedia,
	MSCA_EM_CheckMedia,
	MSCA_EM_CheckMedia,
	MSCA_EM_CheckMedia,
	MSCA_EM_CheckMedia,
	MSCA_EM_CheckMedia,
	MSCA_EM_CheckMedia,
};

msca_checkwp_t			MSCA_CheckWriteProtect[MSCS_MAXLUN] = 
{
	MSCA_SM_CheckWriteProtect0,
	MSCA_SM_CheckWriteProtect1,
	MSCA_SM_CheckWriteProtect2,
	MSCA_SM_CheckWriteProtect3,
	MSCA_SM_CheckWriteProtect4,
	MSCA_SM_CheckWriteProtect5,
	MSCA_SM_CheckWriteProtect6,
	MSCA_SDIO0_CheckWriteProtect,
	MSCA_SDIO1_CheckWriteProtect,
	MSCA_EM_CheckWriteProtect0,
	MSCA_EM_CheckWriteProtect1,
	MSCA_EM_CheckWriteProtect2,
	MSCA_EM_CheckWriteProtect3,
	MSCA_EM_CheckWriteProtect4,
	MSCA_EM_CheckWriteProtect5,
	MSCA_EM_CheckWriteProtect6,
};

msca_lock_t				MSCA_LockMedia[MSCS_MAXLUN] = 
{
	MSCA_SM_LockMedia,
	MSCA_SM_LockMedia,
	MSCA_SM_LockMedia,
	MSCA_SM_LockMedia,
	MSCA_SM_LockMedia,
	MSCA_SM_LockMedia,
	MSCA_SM_LockMedia,
	MSCA_SDIO0_LockMedia,
	MSCA_SDIO1_LockMedia,
	MSCA_EM_LockMedia,
	MSCA_EM_LockMedia,
	MSCA_EM_LockMedia,
	MSCA_EM_LockMedia,
	MSCA_EM_LockMedia,
	MSCA_EM_LockMedia,
	MSCA_EM_LockMedia,
};

msca_unlock_t			MSCA_UnlockMedia[MSCS_MAXLUN] = 
{
	MSCA_SM_UnlockMedia,
	MSCA_SM_UnlockMedia,
	MSCA_SM_UnlockMedia,
	MSCA_SM_UnlockMedia,
	MSCA_SM_UnlockMedia,
	MSCA_SM_UnlockMedia,
	MSCA_SM_UnlockMedia,
	MSCA_SDIO0_UnlockMedia,
	MSCA_SDIO1_UnlockMedia,
	MSCA_EM_UnlockMedia,
	MSCA_EM_UnlockMedia,
	MSCA_EM_UnlockMedia,
	MSCA_EM_UnlockMedia,
	MSCA_EM_UnlockMedia,
	MSCA_EM_UnlockMedia,
	MSCA_EM_UnlockMedia,
};

msca_readlba_t			MSCA_ReadLBA[MSCS_MAXLUN] = 
{
	MSCA_SM_ReadLBA0,
	MSCA_SM_ReadLBA1,
	MSCA_SM_ReadLBA2,
	MSCA_SM_ReadLBA3,
	MSCA_SM_ReadLBA4,
	MSCA_SM_ReadLBA5,
	MSCA_SM_ReadLBA6,
	MSCA_SDIO0_ReadLBA,
	MSCA_SDIO1_ReadLBA,
	MSCA_EM_ReadLBA0,
	MSCA_EM_ReadLBA1,
	MSCA_EM_ReadLBA2,
	MSCA_EM_ReadLBA3,
	MSCA_EM_ReadLBA4,
	MSCA_EM_ReadLBA5,
	MSCA_EM_ReadLBA6,
};

msca_writelba_t			MSCA_WriteLBA[MSCS_MAXLUN] = 
{
	MSCA_SM_WriteLBA0,
	MSCA_SM_WriteLBA1,
	MSCA_SM_WriteLBA2,
	MSCA_SM_WriteLBA3,
	MSCA_SM_WriteLBA4,
	MSCA_SM_WriteLBA5,
	MSCA_SM_WriteLBA6,
	MSCA_SDIO0_WriteLBA,
	MSCA_SDIO1_WriteLBA,
	MSCA_EM_WriteLBA0,
	MSCA_EM_WriteLBA1,
	MSCA_EM_WriteLBA2,
	MSCA_EM_WriteLBA3,
	MSCA_EM_WriteLBA4,
	MSCA_EM_WriteLBA5,
	MSCA_EM_WriteLBA6,
};

msca_mediaint_t			MSCA_MediaIntHandler[MSCS_MAXLUN] = 
{
	MSCA_SM_MediaIntHandler,
	MSCA_SM_MediaIntHandler,
	MSCA_SM_MediaIntHandler,
	MSCA_SM_MediaIntHandler,
	MSCA_SM_MediaIntHandler,
	MSCA_SM_MediaIntHandler,
	MSCA_SM_MediaIntHandler,
	MSCA_SDIO0_MediaIntHandler,
	MSCA_SDIO1_MediaIntHandler,
	MSCA_EM_MediaIntHandler,
	MSCA_EM_MediaIntHandler,
	MSCA_EM_MediaIntHandler,
	MSCA_EM_MediaIntHandler,
	MSCA_EM_MediaIntHandler,
	MSCA_EM_MediaIntHandler,
	MSCA_EM_MediaIntHandler,
};

msca_taskterm_t			MSCA_TaskTerm[MSCS_MAXLUN] = 
{
	MSCA_SM_TaskTerm,
	MSCA_SM_TaskTerm,
	MSCA_SM_TaskTerm,
	MSCA_SM_TaskTerm,
	MSCA_SM_TaskTerm,
	MSCA_SM_TaskTerm,
	MSCA_SM_TaskTerm,
	MSCA_SDIO0_TaskTerm,
	MSCA_SDIO1_TaskTerm,
	MSCA_EM_TaskTerm,
	MSCA_EM_TaskTerm,
	MSCA_EM_TaskTerm,
	MSCA_EM_TaskTerm,
	MSCA_EM_TaskTerm,
	MSCA_EM_TaskTerm,
	MSCA_EM_TaskTerm,
};

msca_capacity_t			MSCA_Capacity[MSCS_MAXLUN] = 
{
	MSCA_SM_Capacity0,
	MSCA_SM_Capacity1,
	MSCA_SM_Capacity2,
	MSCA_SM_Capacity3,
	MSCA_SM_Capacity4,
	MSCA_SM_Capacity5,
	MSCA_SM_Capacity6,
	MSCA_SDIO0_Capacity,
	MSCA_SDIO1_Capacity,
	MSCA_EM_Capacity0,
	MSCA_EM_Capacity1,
	MSCA_EM_Capacity2,
	MSCA_EM_Capacity3,
	MSCA_EM_Capacity4,
	MSCA_EM_Capacity5,
	MSCA_EM_Capacity6,
};


const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_nf0 = {
	MSCA_SM_Init,
	MSCA_SM_Reset,
	MSCA_SM_GetDriveInfo0,
	MSCA_SM_CheckMedia,
	MSCA_SM_CheckWriteProtect0,
	MSCA_SM_LockMedia,
	MSCA_SM_UnlockMedia,
	MSCA_SM_ReadLBA0,
	MSCA_SM_WriteLBA0,
	MSCA_SM_MediaIntHandler,
	MSCA_SM_TaskTerm,
	MSCA_SM_Capacity0,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_nf1 = {
	MSCA_SM_Init,
	MSCA_SM_Reset,
	MSCA_SM_GetDriveInfo1,
	MSCA_SM_CheckMedia,
	MSCA_SM_CheckWriteProtect1,
	MSCA_SM_LockMedia,
	MSCA_SM_UnlockMedia,
	MSCA_SM_ReadLBA1,
	MSCA_SM_WriteLBA1,
	MSCA_SM_MediaIntHandler,
	MSCA_SM_TaskTerm,
	MSCA_SM_Capacity1,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_nf2 = {
	MSCA_SM_Init,
	MSCA_SM_Reset,
	MSCA_SM_GetDriveInfo2,
	MSCA_SM_CheckMedia,
	MSCA_SM_CheckWriteProtect2,
	MSCA_SM_LockMedia,
	MSCA_SM_UnlockMedia,
	MSCA_SM_ReadLBA2,
	MSCA_SM_WriteLBA2,
	MSCA_SM_MediaIntHandler,
	MSCA_SM_TaskTerm,
	MSCA_SM_Capacity2,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_nf3 = {
	MSCA_SM_Init,
	MSCA_SM_Reset,
	MSCA_SM_GetDriveInfo3,
	MSCA_SM_CheckMedia,
	MSCA_SM_CheckWriteProtect3,
	MSCA_SM_LockMedia,
	MSCA_SM_UnlockMedia,
	MSCA_SM_ReadLBA3,
	MSCA_SM_WriteLBA3,
	MSCA_SM_MediaIntHandler,
	MSCA_SM_TaskTerm,
	MSCA_SM_Capacity3,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_nf4 = {
	MSCA_SM_Init,
	MSCA_SM_Reset,
	MSCA_SM_GetDriveInfo4,
	MSCA_SM_CheckMedia,
	MSCA_SM_CheckWriteProtect4,
	MSCA_SM_LockMedia,
	MSCA_SM_UnlockMedia,
	MSCA_SM_ReadLBA4,
	MSCA_SM_WriteLBA4,
	MSCA_SM_MediaIntHandler,
	MSCA_SM_TaskTerm,
	MSCA_SM_Capacity4,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_nf5 = {
	MSCA_SM_Init,
	MSCA_SM_Reset,
	MSCA_SM_GetDriveInfo5,
	MSCA_SM_CheckMedia,
	MSCA_SM_CheckWriteProtect5,
	MSCA_SM_LockMedia,
	MSCA_SM_UnlockMedia,
	MSCA_SM_ReadLBA5,
	MSCA_SM_WriteLBA5,
	MSCA_SM_MediaIntHandler,
	MSCA_SM_TaskTerm,
	MSCA_SM_Capacity5,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_nf6 = {
	MSCA_SM_Init,
	MSCA_SM_Reset,
	MSCA_SM_GetDriveInfo6,
	MSCA_SM_CheckMedia,
	MSCA_SM_CheckWriteProtect6,
	MSCA_SM_LockMedia,
	MSCA_SM_UnlockMedia,
	MSCA_SM_ReadLBA6,
	MSCA_SM_WriteLBA6,
	MSCA_SM_MediaIntHandler,
	MSCA_SM_TaskTerm,
	MSCA_SM_Capacity6,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_sd0 = {
	MSCA_SDIO0_Init,
	MSCA_SDIO0_Reset,
	MSCA_SDIO0_GetDriveInfo,
	MSCA_SDIO0_CheckMedia,
	MSCA_SDIO0_CheckWriteProtect,
	MSCA_SDIO0_LockMedia,
	MSCA_SDIO0_UnlockMedia,
	MSCA_SDIO0_ReadLBA,
	MSCA_SDIO0_WriteLBA,
	MSCA_SDIO0_MediaIntHandler,
	MSCA_SDIO0_TaskTerm,
	MSCA_SDIO0_Capacity,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_sd1 = {
	MSCA_SDIO1_Init,
	MSCA_SDIO1_Reset,
	MSCA_SDIO1_GetDriveInfo,
	MSCA_SDIO1_CheckMedia,
	MSCA_SDIO1_CheckWriteProtect,
	MSCA_SDIO1_LockMedia,
	MSCA_SDIO1_UnlockMedia,
	MSCA_SDIO1_ReadLBA,
	MSCA_SDIO1_WriteLBA,
	MSCA_SDIO1_MediaIntHandler,
	MSCA_SDIO1_TaskTerm,
	MSCA_SDIO1_Capacity,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_em0 = {
	MSCA_EM_Init,
	MSCA_EM_Reset,
	MSCA_EM_GetDriveInfo0,
	MSCA_EM_CheckMedia,
	MSCA_EM_CheckWriteProtect0,
	MSCA_EM_LockMedia,
	MSCA_EM_UnlockMedia,
	MSCA_EM_ReadLBA0,
	MSCA_EM_WriteLBA0,
	MSCA_EM_MediaIntHandler,
	MSCA_EM_TaskTerm,
	MSCA_EM_Capacity0,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_em1 = {
	MSCA_EM_Init,
	MSCA_EM_Reset,
	MSCA_EM_GetDriveInfo1,
	MSCA_EM_CheckMedia,
	MSCA_EM_CheckWriteProtect1,
	MSCA_EM_LockMedia,
	MSCA_EM_UnlockMedia,
	MSCA_EM_ReadLBA1,
	MSCA_EM_WriteLBA1,
	MSCA_EM_MediaIntHandler,
	MSCA_EM_TaskTerm,
	MSCA_EM_Capacity1,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_em2 = {
	MSCA_EM_Init,
	MSCA_EM_Reset,
	MSCA_EM_GetDriveInfo2,
	MSCA_EM_CheckMedia,
	MSCA_EM_CheckWriteProtect2,
	MSCA_EM_LockMedia,
	MSCA_EM_UnlockMedia,
	MSCA_EM_ReadLBA2,
	MSCA_EM_WriteLBA2,
	MSCA_EM_MediaIntHandler,
	MSCA_EM_TaskTerm,
	MSCA_EM_Capacity2,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_em3 = {
	MSCA_EM_Init,
	MSCA_EM_Reset,
	MSCA_EM_GetDriveInfo3,
	MSCA_EM_CheckMedia,
	MSCA_EM_CheckWriteProtect3,
	MSCA_EM_LockMedia,
	MSCA_EM_UnlockMedia,
	MSCA_EM_ReadLBA3,
	MSCA_EM_WriteLBA3,
	MSCA_EM_MediaIntHandler,
	MSCA_EM_TaskTerm,
	MSCA_EM_Capacity3,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_em4 = {
	MSCA_EM_Init,
	MSCA_EM_Reset,
	MSCA_EM_GetDriveInfo4,
	MSCA_EM_CheckMedia,
	MSCA_EM_CheckWriteProtect4,
	MSCA_EM_LockMedia,
	MSCA_EM_UnlockMedia,
	MSCA_EM_ReadLBA4,
	MSCA_EM_WriteLBA4,
	MSCA_EM_MediaIntHandler,
	MSCA_EM_TaskTerm,
	MSCA_EM_Capacity4,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_em5 = {
	MSCA_EM_Init,
	MSCA_EM_Reset,
	MSCA_EM_GetDriveInfo5,
	MSCA_EM_CheckMedia,
	MSCA_EM_CheckWriteProtect5,
	MSCA_EM_LockMedia,
	MSCA_EM_UnlockMedia,
	MSCA_EM_ReadLBA5,
	MSCA_EM_WriteLBA5,
	MSCA_EM_MediaIntHandler,
	MSCA_EM_TaskTerm,
	MSCA_EM_Capacity5,
};

const MSCS_MediaAccessFunc_tp mscs_media_access_func_tbl_em6 = {
	MSCA_EM_Init,
	MSCA_EM_Reset,
	MSCA_EM_GetDriveInfo6,
	MSCA_EM_CheckMedia,
	MSCA_EM_CheckWriteProtect6,
	MSCA_EM_LockMedia,
	MSCA_EM_UnlockMedia,
	MSCA_EM_ReadLBA6,
	MSCA_EM_WriteLBA6,
	MSCA_EM_MediaIntHandler,
	MSCA_EM_TaskTerm,
	MSCA_EM_Capacity6,
};


static UINT32 msc_write_protect[MSCS_MAXLUN] = {
	
	MSC_WPROTECT_OFF,            // NF0
	MSC_WPROTECT_OFF,            // NF1
	MSC_WPROTECT_OFF,            // NF2
	MSC_WPROTECT_OFF,            // NF3
	MSC_WPROTECT_OFF,            // NF4
	MSC_WPROTECT_OFF,            // NF5
	MSC_WPROTECT_OFF,            // NF6
	MSC_WPROTECT_OFF,            // SD0
	MSC_WPROTECT_OFF,            // SD1
	MSC_WPROTECT_OFF,            // eMMC0
	MSC_WPROTECT_OFF,            // eMMC1
	MSC_WPROTECT_OFF,            // eMMC2
	MSC_WPROTECT_OFF,            // eMMC3
	MSC_WPROTECT_OFF,            // eMMC4
	MSC_WPROTECT_OFF,            // eMMC5
	MSC_WPROTECT_OFF,            // eMMC6
};

typedef struct {
	FJ_MEDIA_ID                    mediaID;
	E_BF_MEDIA_STATE_MEDIA_TYPE    mediaType;
	const MSCS_DriveInfo_tp        *p_drive_info_tbl;
	const MSCS_MediaAccessFunc_tp  *p_media_access_func_tbl;
} MSC_MEDIA_INFO;

MSC_MEDIA_INFO msc_media_infos[MSCS_MAXLUN] = {
	
	{FJ_MEDIA_NF_STORAGE0,   E_BF_MEDIA_STATE_MEDIA_TYPE_NF,   &mscs_drive_info_tbl_nf,  &mscs_media_access_func_tbl_nf0},
	{FJ_MEDIA_NF_STORAGE1,   E_BF_MEDIA_STATE_MEDIA_TYPE_NF,   &mscs_drive_info_tbl_nf,  &mscs_media_access_func_tbl_nf1},
	{FJ_MEDIA_NF_STORAGE2,   E_BF_MEDIA_STATE_MEDIA_TYPE_NF,   &mscs_drive_info_tbl_nf,  &mscs_media_access_func_tbl_nf2},
	{FJ_MEDIA_NF_STORAGE3,   E_BF_MEDIA_STATE_MEDIA_TYPE_NF,   &mscs_drive_info_tbl_nf,  &mscs_media_access_func_tbl_nf3},
	{FJ_MEDIA_NF_STORAGE4,   E_BF_MEDIA_STATE_MEDIA_TYPE_NF,   &mscs_drive_info_tbl_nf,  &mscs_media_access_func_tbl_nf4},
	{FJ_MEDIA_NF_STORAGE5,   E_BF_MEDIA_STATE_MEDIA_TYPE_NF,   &mscs_drive_info_tbl_nf,  &mscs_media_access_func_tbl_nf5},
	{FJ_MEDIA_NF_STORAGE6,   E_BF_MEDIA_STATE_MEDIA_TYPE_NF,   &mscs_drive_info_tbl_nf,  &mscs_media_access_func_tbl_nf6},
	{FJ_MEDIA_SD0,           E_BF_MEDIA_STATE_MEDIA_TYPE_SD,   &mscs_drive_info_tbl_sd0, &mscs_media_access_func_tbl_sd0},
	{FJ_MEDIA_SD1,           E_BF_MEDIA_STATE_MEDIA_TYPE_SD,   &mscs_drive_info_tbl_sd1, &mscs_media_access_func_tbl_sd1},
	{FJ_MEDIA_EMMC_STORAGE0, E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC, &mscs_drive_info_tbl_em,  &mscs_media_access_func_tbl_em0},
	{FJ_MEDIA_EMMC_STORAGE1, E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC, &mscs_drive_info_tbl_em,  &mscs_media_access_func_tbl_em1},
	{FJ_MEDIA_EMMC_STORAGE2, E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC, &mscs_drive_info_tbl_em,  &mscs_media_access_func_tbl_em2},
	{FJ_MEDIA_EMMC_STORAGE3, E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC, &mscs_drive_info_tbl_em,  &mscs_media_access_func_tbl_em3},
	{FJ_MEDIA_EMMC_STORAGE4, E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC, &mscs_drive_info_tbl_em,  &mscs_media_access_func_tbl_em4},
	{FJ_MEDIA_EMMC_STORAGE5, E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC, &mscs_drive_info_tbl_em,  &mscs_media_access_func_tbl_em5},
	{FJ_MEDIA_EMMC_STORAGE6, E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC, &mscs_drive_info_tbl_em,  &mscs_media_access_func_tbl_em6},
};


UINT32 msc_media_ids[MSCS_MAXLUN];


BOOL MSC_CheckExtMediaState(void) {
	
	BOOL bRet;
	UINT32 uiDriveNo;
	
	UINT32 uiDriveNoSD0 = MSC_MEDIA_ID_BLANK;
	UINT32 uiDriveNoSD1 = MSC_MEDIA_ID_BLANK;
	
	E_BF_MEDIA_STATE_MEDIA_STATE eMediaStateSD0 = BF_Fs_If_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD);
	E_BF_MEDIA_STATE_MEDIA_STATE eMediaStateSD1 = BF_Fs_If_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1);
	
	for (uiDriveNo = 0; uiDriveNo < MSCS_LUN; uiDriveNo++) {
		
		if (msc_media_ids[uiDriveNo] == FJ_MEDIA_SD0) {
			uiDriveNoSD0 = uiDriveNo;
			BF_Debug_Print_Information((
				"MSC_CheckExtMediaState  SD0 has been connected in MSC mode. Drive No = [%d]\n", uiDriveNoSD0));
		}
		
		if (msc_media_ids[uiDriveNo] == FJ_MEDIA_SD1) {
			uiDriveNoSD1 = uiDriveNo;
			BF_Debug_Print_Information((
				"MSC_CheckExtMediaState  SD1 has been connected in MSC mode. Drive No = [%d]\n", uiDriveNoSD1));
		}
	}
	
	if (eMediaStateSD0 != E_BF_MEDIA_STATE_MEDIA_STATE_INSERT && 
		eMediaStateSD1 != E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
		
		BF_Debug_Print_Error(("MSC_CheckExtMediaState  Neither SD0 nor SD1 has been inserted.\n"));
		bRet = FALSE;
	}
	else if (uiDriveNoSD0 == MSC_MEDIA_ID_BLANK && uiDriveNoSD1 == MSC_MEDIA_ID_BLANK) {
		
		BF_Debug_Print_Error(("MSC_CheckExtMediaState  Neither SD0 nor SD1 has been connected in MSC mode.\n"));
		bRet = FALSE;
	}
	else {
		bRet = TRUE;
	}
	
	return bRet;
}


void MSC_C_ChangeAPI_ExtDrive(FJ_MEDIA_ID mediaId) {
	
	UINT32 uiDriveNo;
	
	E_BF_MEDIA_STATE_MEDIA_STATE eMediaStateSD0 = BF_Fs_If_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD);
	E_BF_MEDIA_STATE_MEDIA_STATE eMediaStateSD1 = BF_Fs_If_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1);
	
	if (eMediaStateSD0 == E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
		
		BF_Debug_Print_Information(("MSC_C_ChangeAPI_ExtDrive  SD0 has been inserted.\n"));
		BF_Fs_User_Custom_Set_SD_Port(0);
	}
	
	if (eMediaStateSD1 == E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
		
		BF_Debug_Print_Information(("MSC_C_ChangeAPI_ExtDrive  SD1 has been inserted.\n"));
		BF_Fs_User_Custom_Set_SD_Port(1);
	}
	
//	for (uiDriveNo = 0; uiDriveNo < MSCS_LUN; uiDriveNo++) {
//		
//		if (msc_media_ids[uiDriveNo] == FJ_MEDIA_SD0 || msc_media_ids[uiDriveNo] == FJ_MEDIA_SD1) {
//			
//			BF_Debug_Print_Information(("MSC_C_ChangeAPI_ExtDrive  Set change flag for SD card. Drive no = %d\n", uiDriveNo));
//			MSCA_MDChangeSet_Drive(uiDriveNo);
//		}
//	}
	for (uiDriveNo = 0; uiDriveNo < MSCS_LUN; uiDriveNo++) {
		
		if (msc_media_ids[uiDriveNo] == mediaId) {
			
			BF_Debug_Print_Information(("MSC_C_ChangeAPI_ExtDrive  Set change flag for SD card. Drive no = %d\n", uiDriveNo));
			MSCA_MDChangeSet_Drive(uiDriveNo);
		}
	}
	
	return;
}

void MSC_C_ChangeAPI(void)
{
	UINT32 uiDriveNo;
	
	for (uiDriveNo = 0; uiDriveNo < MSCS_LUN; uiDriveNo++) {
		
		UINT32 uiIndexMediaInfos = msc_media_ids[uiDriveNo];
		
		MSCS_DriveInfo_tp *pDriveInfoTblDst = &MSCS_DriveInfo[uiDriveNo];
		const MSCS_DriveInfo_tp *pDriveInfoTblSrc = msc_media_infos[uiIndexMediaInfos].p_drive_info_tbl;
		
		const MSCS_MediaAccessFunc_tp *pMediaAccessFuncTbl = msc_media_infos[uiIndexMediaInfos].p_media_access_func_tbl;
		
		BF_Debug_Print_Information((
			"MSC_C_ChangeAPI  Set Drive Infomation/ Media Access function table/ Media Change Flag. Media ID = %d\n", 
			uiIndexMediaInfos));
		
		// Drive Information
		pDriveInfoTblDst->MSCS_ReadRetryCount   = pDriveInfoTblSrc->MSCS_ReadRetryCount;
		pDriveInfoTblDst->MSCS_WriteRetryCount  = pDriveInfoTblSrc->MSCS_WriteRetryCount;
		pDriveInfoTblDst->MSCA_TransferRate     = pDriveInfoTblSrc->MSCA_TransferRate;
		
		// Media Access function table
		MSCA_Init[uiDriveNo]               = pMediaAccessFuncTbl->mscaInit;
		MSCA_Reset[uiDriveNo]              = pMediaAccessFuncTbl->mscaReset;
		MSCA_GetDriveInfo[uiDriveNo]       = pMediaAccessFuncTbl->mscaGetDriveInfo;
		MSCA_CheckMedia[uiDriveNo]         = pMediaAccessFuncTbl->mscaCheckMedia;
		MSCA_CheckWriteProtect[uiDriveNo]  = pMediaAccessFuncTbl->mscaCheckWriteProtect;
		MSCA_LockMedia[uiDriveNo]          = pMediaAccessFuncTbl->mscaLockMedia;
		MSCA_UnlockMedia[uiDriveNo]        = pMediaAccessFuncTbl->mscaUnlockMedia;
		MSCA_ReadLBA[uiDriveNo]            = pMediaAccessFuncTbl->mscaReadLBA;
		MSCA_WriteLBA[uiDriveNo]           = pMediaAccessFuncTbl->mscaWriteLBA;
		MSCA_MediaIntHandler[uiDriveNo]    = pMediaAccessFuncTbl->mscaMediaIntHandler;
		MSCA_TaskTerm[uiDriveNo]           = pMediaAccessFuncTbl->mscaTaskTerm;
		MSCA_Capacity[uiDriveNo]           = pMediaAccessFuncTbl->mscaCapacity;
		
		// Media Change Flag
		MSCA_MDChangeUpdateType(uiDriveNo, msc_media_infos[uiIndexMediaInfos].mediaType);
	}
	
	return;
}


void MSCA_SelectDrive(unsigned char driveno)
{
	if(driveno == 0){
		sdio0_changeDrive(0);
	}else if(driveno == 1){
		/* Do Nothing */
	}
}

void MSCP_EPStalled( unsigned char EP )
{
	MWUSB_EPStalled( EP );
}

void MSCA_SetWProtect(UINT32 uiMediaId, UINT32 uiWriteProtection) {
	
	if (uiMediaId == 0xFF) {
		
		UINT32 uiCnt;
		
		for (uiCnt = 0; uiCnt < MSCS_MAXLUN; uiCnt++) {
			msc_write_protect[uiCnt] = uiWriteProtection;
		}
		
		BF_Debug_Print_Information((
			"MSCA_SetWProtect for All Media. uiWriteProtection = %d\n", uiWriteProtection));
	}
	else {
		msc_write_protect[uiMediaId] = uiWriteProtection;
		BF_Debug_Print_Information((
			"MSCA_SetWProtect for Specified Medium. msc_write_protect[%d] = %d\n", 
			uiMediaId, msc_write_protect[uiMediaId]));
	}
	
	return;
}

unsigned char MSCA_GetWProtect(UINT32 uiMediaId) {
	
	return msc_write_protect[uiMediaId];
}


/***********************************************/
/* Get Media Type for Mode Sense               */
/***********************************************/
unsigned long MSCA_SetInquiryVendData(unsigned char dno, unsigned char *buff)
{
	unsigned long ret = 0;
	return ret;
}

/***********************************************/
/* Get Media Type for Mode Sense               */
/***********************************************/
unsigned char MSCA_GetMediaType(unsigned char dno)
{
	unsigned char mtype=0;
	return mtype;
}

/***********************************************/
/* Mode Sense Page:20h Data Set                */
/***********************************************/
unsigned char MSCA_SetModeData20(unsigned char dno, unsigned char *dat)
{
	unsigned char ret = 0;
	return ret;
}

int MSCA_SDx_MediaIntHandler(UINT32 uiMediaIdOld, UINT32 uiMediaIdNew) {
	
	UINT32 uiDriveNo;
	
	MSCS_DriveInfo_tp *pDriveInfoTblDst;
	const MSCS_DriveInfo_tp *pDriveInfoTblSrc;
	
	const MSCS_MediaAccessFunc_tp *pMediaAccessFuncTbl;
	
	BF_Debug_Print_Information((
		"MSCA_SDx_MediaIntHandler  uiMediaIdOld = %d, uiMediaIdNew = %d\n", 
		uiMediaIdOld, uiMediaIdNew));
	
	// Search Drive Number
	for (uiDriveNo = 0; uiDriveNo < MSCS_LUN; uiDriveNo++) {
		
		if (msc_media_ids[uiDriveNo] == uiMediaIdOld) {
			BF_Debug_Print_Information(("MSCA_SDx_MediaIntHandler  Drive Number = %d\n", uiDriveNo));
			break;
		}
	}
	
	if (uiDriveNo == MSCS_LUN) {
		BF_Debug_Print_Error(("MSCA_SDx_MediaIntHandler  SDx has not been connected in MSC mode.\n"));
		return MSC_ERROR;
	}
	
	pDriveInfoTblDst = &MSCS_DriveInfo[uiDriveNo];
	pDriveInfoTblSrc = msc_media_infos[uiMediaIdNew].p_drive_info_tbl;
	pMediaAccessFuncTbl = msc_media_infos[uiMediaIdNew].p_media_access_func_tbl;
	
	// Drive Information
	pDriveInfoTblDst->MSCS_ReadRetryCount   = pDriveInfoTblSrc->MSCS_ReadRetryCount;
	pDriveInfoTblDst->MSCS_WriteRetryCount  = pDriveInfoTblSrc->MSCS_WriteRetryCount;
	pDriveInfoTblDst->MSCA_TransferRate     = pDriveInfoTblSrc->MSCA_TransferRate;
	
	// Media Access function table
	MSCA_Init[uiDriveNo]               = pMediaAccessFuncTbl->mscaInit;
	MSCA_Reset[uiDriveNo]              = pMediaAccessFuncTbl->mscaReset;
	MSCA_GetDriveInfo[uiDriveNo]       = pMediaAccessFuncTbl->mscaGetDriveInfo;
	MSCA_CheckMedia[uiDriveNo]         = pMediaAccessFuncTbl->mscaCheckMedia;
	MSCA_CheckWriteProtect[uiDriveNo]  = pMediaAccessFuncTbl->mscaCheckWriteProtect;
	MSCA_LockMedia[uiDriveNo]          = pMediaAccessFuncTbl->mscaLockMedia;
	MSCA_UnlockMedia[uiDriveNo]        = pMediaAccessFuncTbl->mscaUnlockMedia;
	MSCA_ReadLBA[uiDriveNo]            = pMediaAccessFuncTbl->mscaReadLBA;
	MSCA_WriteLBA[uiDriveNo]           = pMediaAccessFuncTbl->mscaWriteLBA;
	MSCA_MediaIntHandler[uiDriveNo]    = pMediaAccessFuncTbl->mscaMediaIntHandler;
	MSCA_TaskTerm[uiDriveNo]           = pMediaAccessFuncTbl->mscaTaskTerm;
	MSCA_Capacity[uiDriveNo]           = pMediaAccessFuncTbl->mscaCapacity;
	
	// Media Change Flag
	MSCA_MDChangeUpdateType(uiDriveNo, msc_media_infos[uiMediaIdNew].mediaType);
	
	return MSC_OK;
}


void MSC_SetMscDrive(FJ_MEDIA_ID mediaIds[], UINT32 uiMediaIdNum) {
	
	UINT32 uiCnt;
	
	// Clear Media IDs
	for (uiCnt = 0; uiCnt < MSCS_MAXLUN; uiCnt++) {
		msc_media_ids[uiCnt] = MSC_MEDIA_ID_BLANK;
	}
	
	// Set Media IDs
	MSCS_LUN = uiMediaIdNum;
	BF_Debug_Print_Information(("MSC_SetMscDrive (MSCS_LUN = %d)\n", MSCS_LUN));
	
	for (uiCnt = 0; uiCnt < MSCS_LUN; uiCnt++) {
		
		msc_media_ids[uiCnt] = mediaIds[uiCnt];
		
		BF_Debug_Print_Information((
			"MSC_SetMscDrive (msc_media_ids[%d] = %d)\n", 
			uiCnt, msc_media_ids[uiCnt]));
	}
	
	// Initialize drive information for NF, eMMC
	MSCA_MEDIA_InitDriveInfo(mediaIds, uiMediaIdNum);
	
	return;
}

