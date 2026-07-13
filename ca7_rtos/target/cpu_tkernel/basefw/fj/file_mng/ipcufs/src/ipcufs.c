/**
 * @file		ipcufs.c
 * @brief		Fake FileSystem function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "ddim_typedef.h"
#include "fj_filesystem.h"
#include "fs_if.h"
#include "fs_controller.h"
#include "fj_media.h"
#include "ipcufs.h"
#include "ipcu.h"
#include "l1l2cache.h"
#include "mwnf_core.h"
#include "dd_top.h"
#include "dd_exs.h"
#include "media_drv_wrapper.h"

#include "mw_user_custom.h"
#include "media_drv_wrapper.h"

// --- REMOVE_ES3_HARDWARE BEGIN ---
#include "em_int_cmn.h"
#include "em_if_ext.h"
// --- REMOVE_ES3_HARDWARE END --

/*----------------------------------------------------------------*/
/* Definition													  */
/*----------------------------------------------------------------*/
/* IPCU event flag pattern */
#define FLG_FS_IPCUFS_RECEIVE_CB	(0x80000000)

/* Type */
#define IPCUFS_TYPE_FILE		0
#define IPCUFS_TYPE_DIRECTORY	1

/* Access mode */
#define IPCUFS_TYPE_READ		0
#define IPCUFS_TYPE_WITE		1

/* Wait time for NF clock stop */
#define D_IPCUFS_WAIT_TIME_NF_LP		(3000)
#define D_IPCUFS_WAIT_TIME_XDMAC_LP		(3000)
/* NF partition table hibernation driver area num */
#define D_HIBDRV_PARTITION_NUM				(7)

/* parameter for hibdrv */
#define D_IPCUFS_HIBDRV_SIZE				(0x200)
#define D_IPCUFS_HIBDRV_UAPI_SIZE			(0x200)
#define D_IPCUFS_HIBDRV_BF_OFFS				(D_IPCUFS_HIBDRV_SIZE + D_IPCUFS_HIBDRV_UAPI_SIZE)

#define D_IPCUFS_BOOTFLAG_RESULT			(0)
#define D_IPCUFS_BOOTFLAG_ERROR_NO			(0x3F8)

/*----------------------------------------------------------------*/
/* Enumeration													  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Structure													  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Data													  */
/*----------------------------------------------------------------*/
static volatile UCHAR* g_bf_ipcufs_info_table;
static volatile UINT32 g_bf_ipcufs_info_length;
static volatile UCHAR  g_bf_ipcufs_umount_req_flag = 0;
static volatile CHAR   g_drv_name[8][D_BF_FS_IF_DRVNAME_MAX];
/* NF partition table */
static volatile T_NF_PARTITION g_NF_hibdrv_partition_table;

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
/* IPCU Request Callback */
FW_VOID ipcufs_ipcu_event_request_cb( UCHAR id, UCHAR *data, UINT32 length, UINT32 cont, UINT32 total_length )
{
	g_bf_ipcufs_info_table  = data;
	g_bf_ipcufs_info_length = length;

//	BF_Fs_User_Custom_Info_Printer(("[IPCUFS]: Send IPCU request on IPCUFS_receiver task.\n"));

	OS_User_Set_Flg( FID_FS_IPCUFS_RECEIVE, FLG_FS_IPCUFS_RECEIVE_CB );
}

static FW_VOID ipcufs_get_path_name( FW_INT32 drive, FW_CHAR *input_path, FW_CHAR *path_name )
{
	strcpy(path_name, (const FW_CHAR*)gBF_Controller_drv_name[drive]);
	strcat(path_name, ":");
	strcat(path_name, input_path);
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 * @brief VFS receiver task [TASK]
 * @param FW_VOID
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_VOID IPCUFS_receiver(INT32 stacd)
{
	T_IPCUFS_INFO_TABLE_COMM* info_tbl_comm;
	OS_USER_FLGPTN	flgptn;
	FW_UCHAR ipcu_id;
	FW_INT32 ercd;

	BF_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_2, &ipcu_id);
	BF_IPCU_SetReqCB(ipcu_id, ipcufs_ipcu_event_request_cb);

	OS_User_Clr_Flg(FID_FS_IPCUFS_RECEIVE, ~FLG_FS_IPCUFS_RECEIVE_CB);

	// make partition table
	memcpy((void*)&g_NF_hibdrv_partition_table,
		   (void*)&gNF_partition_table[D_HIBDRV_PARTITION_NUM], sizeof(T_NF_PARTITION));

	/* Wait for a IPCU response from Linux */
	while( 1 ) {
		OS_User_Twai_Flg( FID_FS_IPCUFS_RECEIVE, FLG_FS_IPCUFS_RECEIVE_CB, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg(FID_FS_IPCUFS_RECEIVE, ~FLG_FS_IPCUFS_RECEIVE_CB);

		info_tbl_comm = (T_IPCUFS_INFO_TABLE_COMM*)g_bf_ipcufs_info_table;

//		BF_Fs_User_Custom_Info_Printer(("[IPCUFS]: IPCUFS_receiver start. ope_cmd=0x%x\n", info_tbl_comm->ope_cmd));

		switch(info_tbl_comm->ope_cmd) {
			case D_BF_IPCUFS_IFSA_START :
				ercd = BF_Ipcufs_Start((T_FS_IPCUFS_INFO_TABLE_START*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_IFSA_END :
				ercd = BF_Ipcufs_End((T_FS_IPCUFS_INFO_TABLE_END*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_IFSA_LOOKUP :
				ercd = BF_Ipcufs_Lookup((T_FS_IPCUFS_INFO_TABLE_LOOKUP*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_IFSA_READDIR :
				ercd = BF_Ipcufs_ReadDir((T_FS_IPCUFS_INFO_TABLE_READDIR*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_IFSA_READPAGE :
				ercd = BF_Ipcufs_Readpage((T_FS_IPCUFS_INFO_TABLE_READPAGE*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_IFSA_WRITEPAGE :
				ercd = BF_Ipcufs_Writepage((T_FS_IPCUFS_INFO_TABLE_WRITEPAGE*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_IFSA_IGET :
				ercd = BF_Ipcufs_Iget((T_FS_IPCUFS_INFO_TABLE_IGET*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_IFSA_ICREATE :
				ercd = BF_Ipcufs_Icreate((T_FS_IPCUFS_INFO_TABLE_ICREATE*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_IFSA_IDELETE :
				ercd = BF_Ipcufs_Idelete((T_FS_IPCUFS_INFO_TABLE_IDELETE*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_IFSA_IRENAME :
				ercd = BF_Ipcufs_Irename((T_FS_IPCUFS_INFO_TABLE_IRENAME*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_IFSA_STATFS :
				ercd = BF_Ipcufs_Statfs((T_FS_IPCUFS_INFO_TABLE_STATFS*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_IFSA_OPEN :
				ercd = BF_Ipcufs_Open((T_FS_IPCUFS_INFO_TABLE_OPEN*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_IFSA_CLOSE :
				ercd = BF_Ipcufs_Close((T_FS_IPCUFS_INFO_TABLE_CLOSE*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_UNMOUNT :
				ercd = BF_Ipcufs_Unmount((T_FS_IPCUFS_INFO_TABLE_UNMOUNT*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_MOUNT :
				ercd = BF_Ipcufs_Mount((T_FS_IPCUFS_INFO_TABLE_MOUNT*)g_bf_ipcufs_info_table);
				break;

			case D_BF_IPCUFS_HIB_NOTIFY :
				ercd = BF_Ipcufs_Hib_Notify((T_FS_IPCUFS_INFO_TABLE_HIB_NOTIFY*)g_bf_ipcufs_info_table);
				break;

			default :
				g_bf_ipcufs_info_length = 0;
				ercd = 1;
				break;
		}

		if (g_bf_ipcufs_info_length != 0) {
			// IPCU complete send
			BF_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_3, &ipcu_id);
			BF_L1l2cache_Clean_Flush_Addr((UINT32)g_bf_ipcufs_info_table, g_bf_ipcufs_info_length);
			BF_IPCU_Send(ipcu_id, (UCHAR*)g_bf_ipcufs_info_table, g_bf_ipcufs_info_length, 1);
			BF_IPCU_Close(ipcu_id);
		}
		if(ercd != 0) {	// for warning
//		BF_Fs_User_Custom_Info_Printer(("[IPCUFS]: IPCUFS_receiver end.(%d)\n", ercd ));
		}
	}
}

FW_INT32 BF_Ipcufs_Start(T_FS_IPCUFS_INFO_TABLE_START* info_table)
{
	info_table->error_code = D_BF_IPCUFS_ERR_OK;

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_End(T_FS_IPCUFS_INFO_TABLE_END* info_table)
{
	info_table->error_code = D_BF_IPCUFS_ERR_OK;

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_Lookup(T_FS_IPCUFS_INFO_TABLE_LOOKUP* info_table)
{
	FJ_ERR_CODE ercd;
	FJ_FS_STAT stat;
	FW_CHAR path_name[(D_BF_IPCUFS_INFO_DRV_NAME_MAX + D_BF_FS_IF_DRVNAME_MAX)];

	memset(path_name, '\0', sizeof(path_name));
	ipcufs_get_path_name(info_table->drive, info_table->path, path_name);

	ercd = FJ_Fs_Stat(path_name, &stat);

	if (ercd == FJ_ERR_OK) {
		info_table->error_code = D_BF_IPCUFS_ERR_OK;
	}
	else {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
	}

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_ReadDir(T_FS_IPCUFS_INFO_TABLE_READDIR* info_table)
{
	FJ_ERR_CODE ercd;
	UINT32 puiAttributes;
	ULONG last_pos, pulSize;
	FW_CHAR path_name[(D_BF_IPCUFS_INFO_DRV_NAME_MAX + D_BF_FS_IF_DRVNAME_MAX)];

	CHAR media_drv[9] ={0};
	CHAR cur_dir[520] ={0};
	ercd = BF_Fs_If_Get_Cur_Drv( media_drv );
	if (ercd != FJ_ERR_OK) {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
		return info_table->error_code;
	}
	ercd = BF_Fs_If_Get_Cur_Dir(media_drv, cur_dir, 256);
	if (ercd != FJ_ERR_OK) {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
		return info_table->error_code;
	}

	memset(info_table->file_name, 0, sizeof(info_table->file_name));

	if (info_table->open_flag == 1) {
		// Start entry get
		memset(path_name, '\0', sizeof(path_name));
		ipcufs_get_path_name(info_table->drive, info_table->path, path_name);
		last_pos = (strlen(path_name) - 1);

		if (path_name[last_pos] != '\\') {
			strcat(path_name, "\\");
		}
		ercd = FJ_Fs_SetSearchEntry(path_name);
		if (ercd != FJ_ERR_OK) {
			info_table->error_code = D_BF_IPCUFS_ERR_NG;
			return info_table->error_code;
		}

		ercd = FJ_Fs_DirectoryEntryGet(FJ_FS_ENTRY_MODE_START, info_table->file_name, sizeof(info_table->file_name), &puiAttributes, &pulSize);
	}
	else {
		// Next entry get
		ercd = FJ_Fs_DirectoryEntryGet(FJ_FS_ENTRY_MODE_NEXT_NOT_SUB, info_table->file_name, sizeof(info_table->file_name), &puiAttributes, &pulSize);
	}

	if (ercd == FJ_ERR_OK) {
		if ((puiAttributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
			info_table->type = 1;
		}
		else {
			info_table->type = 0;
		}
		info_table->error_code = D_BF_IPCUFS_ERR_OK;
	}
	else if (ercd == FJ_ERR_LAST_ENTRY) {
		(void)FJ_Fs_DirectoryEntryGet(FJ_FS_ENTRY_MODE_END, info_table->file_name, sizeof(info_table->file_name), &puiAttributes, &pulSize);
		info_table->error_code = D_BF_IPCUFS_ERR_LAST_ENTRY;
	}
	else {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
	}
	ercd = FJ_Fs_SetSearchEntry(cur_dir);
	if (ercd != FJ_ERR_OK) {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
		return info_table->error_code;
	}

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_Readpage(T_FS_IPCUFS_INFO_TABLE_READPAGE* info_table)
{
	FJ_ERR_CODE ercd = FJ_ERR_OK;
	FW_INT32 FileId;
	FW_ULONG length, i;
	FW_INT64 seek_len;

	FileId = info_table->fno;

	ercd = FJ_Fs_Lseek64(FileId, info_table->read_offset, FJ_FS_SEEK_SET, &seek_len);
	if (ercd != FJ_ERR_OK) {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
		return info_table->error_code;
	}

	for (i=0; i<D_BF_IPCUFS_RW_DATA_MAX; i++) {
		if ((info_table->read_length[i] != 0) && ((UINT32*)info_table->physical_addr[i] != NULL)) {
			ercd = FJ_Fs_Read(FileId, (UINT32*)info_table->physical_addr[i], info_table->read_length[i], &length);
			if (ercd != FJ_ERR_OK) {
				break;
			}
		}
		else {
			break;
		}
	}

	if (ercd == FJ_ERR_OK) {
		info_table->error_code = D_BF_IPCUFS_ERR_OK;
	}
	else if (ercd == FJ_ERR_READ_EOF) {
		info_table->error_code = D_BF_IPCUFS_ERR_EOF;
	}
	else {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
	}

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_Writepage(T_FS_IPCUFS_INFO_TABLE_WRITEPAGE* info_table)
{
	FJ_ERR_CODE ercd = FJ_ERR_OK;
	FW_INT32 FileId;
	FW_ULONG length, i;
	FW_INT64 seek_len;

	FileId = info_table->fno;

	ercd = FJ_Fs_Lseek64(FileId, info_table->write_offset, FJ_FS_SEEK_SET, &seek_len);
	if (ercd != FJ_ERR_OK) {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
		return info_table->error_code;
	}

	for (i=0; i<D_BF_IPCUFS_RW_DATA_MAX; i++) {
		if ((info_table->write_length[i] != 0) && ((UINT32*)info_table->physical_addr[i] != NULL)) {
			ercd = FJ_Fs_Write(FileId, (UINT32*)info_table->physical_addr[i], info_table->write_length[i], &length);
		}
		else {
			break;
		}
	}

	if (ercd == FJ_ERR_OK) {
		info_table->error_code = D_BF_IPCUFS_ERR_OK;
	}
	else {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
	}

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_Iget(T_FS_IPCUFS_INFO_TABLE_IGET* info_table)
{
	FW_INT32 ercd;
	T_BF_FS_IF_STAT file_stat;
	FW_CHAR path_name[(D_BF_IPCUFS_INFO_DRV_NAME_MAX + D_BF_FS_IF_DRVNAME_MAX)];

	memset(path_name, '\0', sizeof(path_name));
	ipcufs_get_path_name(info_table->drive, info_table->path, path_name);

	ercd = BF_Fs_If_Stat((const FW_CHAR*)path_name, &file_stat);
	if (ercd != FSDCF_ERRNO_OK) {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
		return info_table->error_code;
	}

	info_table->file_length = file_stat.fileSize;
	info_table->date        = file_stat.date;
	info_table->time        = file_stat.time;
	info_table->m_date      = file_stat.m_date;
	info_table->m_time      = file_stat.m_time;
	info_table->a_date      = file_stat.a_date;
	info_table->a_time      = file_stat.a_time;

	if ((file_stat.attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
		info_table->type    = 1;
	}
	else {
		info_table->type    = 0;
	}
	info_table->error_code = D_BF_IPCUFS_ERR_OK;

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_Icreate(T_FS_IPCUFS_INFO_TABLE_ICREATE* info_table)
{
	FW_INT32 ercd, FileId;
	FW_CHAR path_name[(D_BF_IPCUFS_INFO_DRV_NAME_MAX + D_BF_FS_IF_DRVNAME_MAX)];

	memset(path_name, '\0', sizeof(path_name));
	ipcufs_get_path_name(info_table->drive, info_table->path, path_name);

	if (info_table->type == IPCUFS_TYPE_FILE) {
		// file create
		FileId = BF_Fs_If_Create(path_name, D_BF_FS_IF_ATTR_NORMAL);
		if (FileId == 0) {
			info_table->error_code = D_BF_IPCUFS_ERR_NG;
			return info_table->error_code;
		}
		FJ_Fs_Close(FileId);
	}
	else {
		// directory create
		ercd = BF_Fs_If_Mkdir((const FW_CHAR*)path_name);
		if (ercd != FSDCF_ERRNO_OK) {
			info_table->error_code = D_BF_IPCUFS_ERR_NG;
			return info_table->error_code;
		}
	}
	info_table->error_code = D_BF_IPCUFS_ERR_OK;

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_Idelete(T_FS_IPCUFS_INFO_TABLE_IDELETE* info_table)
{
	FW_INT32 ercd;
	FW_CHAR media[9];
	FW_CHAR path_name[(D_BF_IPCUFS_INFO_DRV_NAME_MAX + D_BF_FS_IF_DRVNAME_MAX)];

	memset(path_name, '\0', sizeof(path_name));
	ipcufs_get_path_name(info_table->drive, info_table->path, path_name);

	if (info_table->type == IPCUFS_TYPE_FILE) {
		// file delete
		ercd = BF_Fs_If_Remove((const FW_CHAR*)path_name);
	}
	else {
		// directory delete
		strcpy(media, (const FW_CHAR*)gBF_Controller_drv_name[info_table->drive]);
		strcat(media, ":\\");

		ercd = BF_Fs_If_Chdir(media);
		if (ercd != FSDCF_ERRNO_OK) {
			info_table->error_code = D_BF_IPCUFS_ERR_NG;
			return info_table->error_code;
		}
		ercd = BF_Fs_If_Rmdir((const FW_CHAR*)path_name);
	}

	if (ercd == FSDCF_ERRNO_OK) {
		info_table->error_code = D_BF_IPCUFS_ERR_OK;
	}
	else {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
	}

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_Irename(T_FS_IPCUFS_INFO_TABLE_IRENAME* info_table)
{
	FJ_ERR_CODE ercd;
	FW_CHAR path_name[(D_BF_IPCUFS_INFO_DRV_NAME_MAX + D_BF_FS_IF_DRVNAME_MAX)];
	FW_CHAR path_rename[(D_BF_IPCUFS_INFO_DRV_NAME_MAX + D_BF_FS_IF_DRVNAME_MAX)];

	memset(path_name, '\0', sizeof(path_name));
	memset(path_rename, '\0', sizeof(path_rename));
	ipcufs_get_path_name(info_table->drive, info_table->path, path_name);
	ipcufs_get_path_name(info_table->rename_drive, info_table->rename_path, path_rename);

	ercd = FJ_Fs_Rename((CHAR*)path_name, (CHAR*)path_rename);

	if (ercd == FJ_ERR_OK) {
		info_table->error_code = D_BF_IPCUFS_ERR_OK;
	}
	else {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
	}

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_Statfs(T_FS_IPCUFS_INFO_TABLE_STATFS* info_table)
{
	FW_INT32 ercd;
	T_BF_FS_IF_STATVFS media_info;
	FW_CHAR media[9];

	strcpy(media, (const FW_CHAR*)gBF_Controller_drv_name[info_table->drive]);

	ercd = BF_Fs_If_Statvfs(media, &media_info);
	if (ercd != FSDCF_ERRNO_OK) {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
		return info_table->error_code;
	}

	info_table->blksize = media_info.blksize;
	info_table->blocks  = media_info.blocks;
	info_table->bfree   = media_info.bfree;
	info_table->bbad    = media_info.bbad;
	info_table->dstat   = media_info.dstat;
	info_table->fstype  = media_info.fstype;

	info_table->error_code = D_BF_IPCUFS_ERR_OK;

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_Open(T_FS_IPCUFS_INFO_TABLE_OPEN* info_table)
{
	FW_INT32 FileId;
	FW_INT32 Flags;
	FW_CHAR path_name[(D_BF_IPCUFS_INFO_DRV_NAME_MAX + D_BF_FS_IF_DRVNAME_MAX)];

	memset(path_name, '\0', sizeof(path_name));
	ipcufs_get_path_name(info_table->drive, info_table->path, path_name);

	if (info_table->amode == IPCUFS_TYPE_READ) {
		Flags = FJ_FS_OPEN_RDONLY;
	}
	else {
		Flags = FJ_FS_OPEN_RDWR;
	}

	FileId = FJ_Fs_Open((CHAR*)path_name,  Flags);
	if (FileId != 0) {
		info_table->error_code = D_BF_IPCUFS_ERR_OK;
		info_table->fno = FileId;
	}
	else {
		info_table->error_code = D_BF_IPCUFS_ERR_NG;
	}

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_Close(T_FS_IPCUFS_INFO_TABLE_CLOSE* info_table)
{
	FJ_Fs_Close(info_table->fno);
	info_table->error_code = D_BF_IPCUFS_ERR_OK;

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_Unmount(T_FS_IPCUFS_INFO_TABLE_UNMOUNT* info_table)
{
	FW_INT32 ercd;
	FW_ULONG media_id;

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	CHAR	mdr = (*(( unsigned long * )0x1DFF4000) & 0x7);

	// drive name clear
	memset((CHAR*)g_drv_name, '\0', sizeof(g_drv_name));

	if (mdr == 1) {
		// eMMC unmount
		for (media_id = 0; media_id <= (FJ_MEDIA_EMMC_STORAGE6 - FJ_MEDIA_EMMC_STORAGE0); media_id++) {
			if((*gBF_Controller_drv_name[(media_id + FJ_MEDIA_EMMC_STORAGE0)]) == 0) {
				continue;
			}
			// buckup to drive name
			strcpy((FW_CHAR*)g_drv_name[media_id], (const CHAR*)gBF_Controller_drv_name[(media_id + FJ_MEDIA_EMMC_STORAGE0)]);

			ercd = FJ_UnMount( (FW_CHAR*)g_drv_name[media_id] );
			if(( ercd != FJ_ERR_OK ) && ( ercd != FJ_ERR_NOT_MOUNT )) {
				BF_Fs_User_Custom_Err_Printer(("%s(): FJ_UnMount(%s) failed: ercd=%x\n", __func__, g_drv_name[media_id], ercd ));
				info_table->error_code = 1;
				return info_table->error_code;
			}
		}
		
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
		// eMMC disk close
		(FW_VOID)BF_Media_Drv_Wrapper_Finalize_EMMC();
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---
		
		/* Disable eMMC interrupt */
		ercd = Media_Custom_EMMC_GIC_Ctrl_Dis();
		if (ercd != 0) {
			BF_Fs_User_Custom_Err_Printer(("%s(): Media_Custom_EMMC_GIC_Ctrl_Dis() failed: ercd=%x\n", __func__, ercd ));
			info_table->error_code = 1;
			return info_table->error_code;
		}
		
		/* eMMC macro clock enable */
		Dd_Top_Set_CLKSTOP2_EMMCCK(0);
		Dd_Exs_Set_EMMC_CKSTP_EMCLKSTP(0);
		
		/* eMMC bus clock enable */
		Dd_Exs_Set_EMMC_BUS_CLOCK_Stop_Ctrl(0);
		
		/* Information table setting */
		if (g_NF_hibdrv_partition_table.selector == 0) {
			info_table->target = D_BF_FS_USER_CUSTOM_MEDIA_EMMC_0;
		}
		else {
			info_table->target = D_BF_FS_USER_CUSTOM_MEDIA_EMMC_1;
		}
		info_table->error_code = 0;
	}
	else {
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

		// NF unmount
		for (media_id = 0; media_id <= FJ_MEDIA_NF_STORAGE6; media_id++) {
			if((*gBF_Controller_drv_name[media_id]) == 0) {
				continue;
			}
			// buckup to drive name
			strcpy((FW_CHAR*)g_drv_name[media_id], (const CHAR*)gBF_Controller_drv_name[media_id]);

			ercd = FJ_UnMount( (FW_CHAR*)g_drv_name[media_id] );
			if(( ercd != FJ_ERR_OK ) && ( ercd != FJ_ERR_NOT_MOUNT )) {
				BF_Fs_User_Custom_Err_Printer(("%s(): FJ_UnMount(%s) failed: ercd=%x\n", __func__, g_drv_name[media_id], ercd ));
				info_table->error_code = 1;
				return info_table->error_code;
			}
		}
		
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
		// NF disk close
		(FW_VOID)BF_Media_Drv_Wrapper_Finalize_NF();
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---
		
		/* Change DMA operating mode */
		ercd = MWNF_Set_DMA_Mode( 0 );
		if( ercd != FJ_ERR_OK ) {
			BF_Fs_User_Custom_Err_Printer(("%s(): MWNF_Set_DMA_Mode() failed: ercd=%x\n", __func__, ercd ));
			info_table->error_code = 1;
			return info_table->error_code;
		}
		
		/* Set ECC parameter */
		ercd = 	MWNF_Set_Ecc_Param( D_MWNF_ENABLE, D_MWNF_ECC_AUTO );
		if( ercd != FJ_ERR_OK ) {
			BF_Fs_User_Custom_Err_Printer(("%s(): MWNF_Set_Ecc_Param() failed: ercd=%x\n", __func__, ercd ));
			info_table->error_code = 1;
			return info_table->error_code;
		}
		
		/* Disable NF interrupt */
		ercd = Media_Custom_NF_GIC_Ctrl_Dis();
		if (ercd != 0) {
			BF_Fs_User_Custom_Err_Printer(("%s(): Media_Custom_NF_GIC_Ctrl_Dis() failed: ercd=%x\n", __func__, ercd ));
			info_table->error_code = 1;
			return info_table->error_code;
		}
		
		/* NF macro clock enable */
		Dd_Top_Set_CLKSTOP2_NFCK( 0 );
		Dd_Top_Set_CLKSTOP11_NFBCHCK( 0 );
		
		/* NF bus clock enable */
		Dd_Exs_Set_NF_BUS_CLOCK_Stop_Ctrl( 0 );
		
		/* Information table setting */
		if (g_NF_hibdrv_partition_table.selector == 0) {
			info_table->target = D_BF_FS_USER_CUSTOM_MEDIA_NF_0;
		}
		else {
			info_table->target = D_BF_FS_USER_CUSTOM_MEDIA_NF_1;
		}
		info_table->error_code = 0;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	/* SD ch1 RAM PD Off */
	Dd_Exs_Set_RAM_PD_Ctrl_SD3( 0 );
	
	g_bf_ipcufs_umount_req_flag = 1;

	return info_table->error_code;
}

FW_INT32 BF_Ipcufs_Mount(T_FS_IPCUFS_INFO_TABLE_MOUNT* info_table)
{
	FJ_ERR_CODE		ercd;
	FW_ULONG		media_id;
	FW_ULONG		part_num;
	FJ_NF_PARTITION	part_info[8] = { { 0 } };

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	FJ_EMMC_PARTITION	part_info_emmc[8] = { { 0 } };
	CHAR	mdr = (*(( unsigned long * )0x1DFF4000) & 0x7);

	if (mdr == 1) {
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
		// fail safe
		if (gBF_Media_Drv_Wrapper_Disk_Tbl[3] != NULL) {
			(FW_VOID)BF_Media_Drv_Wrapper_Finalize_EMMC();
		}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

		// eMMC mount
		ercd = FJ_EMMC_Init();
		if( ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( "%s(): FJ_EMMC_Init() failed: ercd=%x\n", __func__, ercd ));
			info_table->error_code = 1;
			return info_table->error_code;
		}

		ercd = FJ_EMMC_Device_Init();
		if( ercd != 0 ) {
			BF_Debug_Print_Error(( "%s(): FJ_EMMC_Device_Init() failed: ercd=%x\n", __func__, ercd ));
			info_table->error_code = 1;
			return info_table->error_code;
		}

		ercd = FJ_EMMC_Partition_Read( part_info_emmc, &part_num );
		if( ercd != 0 ) {
			BF_Debug_Print_Error(( "%s(): FJ_EMMC_Partition_Read() failed: ercd=%x\n", __func__, ercd ));
			info_table->error_code = 1;
			return info_table->error_code;
		}

		for( media_id = 0; media_id < part_num; media_id++ ) {
			if( (*g_drv_name[media_id]) == 0 ) {
				continue;
			}
			ercd = FJ_Mount( (FJ_MEDIA_ID)(media_id + FJ_MEDIA_EMMC_STORAGE0), (FW_CHAR*)g_drv_name[media_id] );
			if( ercd != FJ_ERR_OK ) {
				BF_Debug_Print_Error(( "%s(): FJ_Mount(%s) failed: ercd=%x\n", __func__, g_drv_name[media_id], ercd ));
				info_table->error_code = 1;
				return info_table->error_code;
			}
		}
	}
	else {
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
		// fail safe
		if (gBF_Media_Drv_Wrapper_Disk_Tbl[2] != NULL) {
			(FW_VOID)BF_Media_Drv_Wrapper_Finalize_NF();
		}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

		// NF mount
		ercd = FJ_NF_Init();
		if( ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( "%s(): FJ_NF_Init() failed: ercd=%x\n", __func__, ercd ));
			info_table->error_code = 1;
			return info_table->error_code;
		}

		ercd = FJ_NF_Device_Init();
		if( ercd != 0 ) {
			BF_Debug_Print_Error(( "%s(): FJ_NF_Device_Init() failed: ercd=%x\n", __func__, ercd ));
			info_table->error_code = 1;
			return info_table->error_code;
		}

		ercd = FJ_NF_Partition_Read( part_info, &part_num );
		if( ercd != 0 ) {
			BF_Debug_Print_Error(( "%s(): FJ_NF_Partition_Read() failed: ercd=%x\n", __func__, ercd ));
			info_table->error_code = 1;
			return info_table->error_code;
		}

		for( media_id = 0; media_id < part_num; media_id++ ) {
			if( (*g_drv_name[media_id]) == 0 ) {
				continue;
			}
			ercd = FJ_Mount( (FJ_MEDIA_ID)media_id, (FW_CHAR*)g_drv_name[media_id] );
			if( ercd != FJ_ERR_OK ) {
				BF_Debug_Print_Error(( "%s(): FJ_Mount(%s) failed: ercd=%x\n", __func__, g_drv_name[media_id], ercd ));
				info_table->error_code = 1;
				return info_table->error_code;
			}
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	info_table->error_code = 0;
	g_bf_ipcufs_umount_req_flag = 0;

	return info_table->error_code;
}

UCHAR BF_Ipcufs_Get_Unmount_Request_Flg(VOID)
{
	return g_bf_ipcufs_umount_req_flag;
}

FW_INT32 BF_Ipcufs_Hib_Notify(T_FS_IPCUFS_INFO_TABLE_HIB_NOTIFY* info_table)
{
	FW_INT32 ercd;
	INT32 selector = 0;
	ULONG bf_addr;
	static CHAR read_data[1024];

	// initiarize
	memset(read_data, 0x0, sizeof(read_data));

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	CHAR	mdr = (*(( unsigned long * )0x1DFF4000) & 0x7);

	if (mdr == 1) {
		bf_addr = g_NF_hibdrv_partition_table.area[selector].sector_num + D_IPCUFS_HIBDRV_BF_OFFS;

		// eMMC(Read 2 sector)
		ercd = MWEM_DirectRead(bf_addr, 2, read_data);
		while(ercd == EM_INVAL){
			OS_User_Dly_Tsk(100);
			ercd = MWEM_DirectRead(bf_addr, 2, read_data);
		}
	}
	else {
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		// NF
		bf_addr = ((g_NF_hibdrv_partition_table.area[selector].lut[0]) * D_BF_IPCUFS_SEC_PAR_BLOCK) + D_IPCUFS_HIBDRV_BF_OFFS;

		ercd = MWNF_Direct_BRead_DataDMA(bf_addr, 2, read_data);
		while(ercd == D_MWNF_UNINITIALIZE_ERR){
			OS_User_Dly_Tsk(100);
			ercd = MWNF_Direct_BRead_DataDMA(bf_addr, 2, read_data);
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	if (ercd == FJ_ERR_OK) {
		memcpy(&info_table->result, &read_data[D_IPCUFS_BOOTFLAG_RESULT], 4);
		memcpy(&info_table->error_no, &read_data[D_IPCUFS_BOOTFLAG_ERROR_NO], 4);
		info_table->bf_address = bf_addr;
	}

	info_table->error_code = ercd;

	return info_table->error_code;
}
