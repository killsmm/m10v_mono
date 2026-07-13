/**
 * @file		filesystem.c
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
#include "fj_filesystem.h"
#include "fs_user_func.h"

#include "fsdcf_errno.h"
#include "fs_if.h"
#include "mwnf.h"
#ifdef CO_DCF_EXIF_ON
#include "exif_controller.h"
#include "dcf_db.h"
#include "exif_attr.h"
#endif // CO_DCF_EXIF_ON
#include "media_attr.h"
#include "mba_if.h"
#include "boot_loader.h"
#include "bf_startup.h"
#include "fj_host.h"
#include "dynamic_scenario.h"
#include "still.h"
#include "still_attr.h"
// delete\basefw\fj\imgproc\still
//#include "still_pro_ctrl.h"
#include "still.h"
#include "fs_controller.h"
#include "querystatus.h"
#include "sdram_map_cache_define.h"
#include "sdram_map_define.h"
#include "sdram_map_share.h"


/////////////////////////////////////////////////////
// romarea_boot
/////////////////////////////////////////////////////
/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_FS_USER_SET_OFFSET		(16)
#define D_FS_USER_SET_MAX_SIZE		(0x1F800)	/* 128kB - 2kB */

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Dir Entry info
typedef struct {
	CHAR dir_path[32];
	INT32 dir_no;
} T_BF_FILESYSTEM_DIR_PARAM;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
static volatile UCHAR g_bf_filesystem_dir_num = 0;
static volatile T_BF_FILESYSTEM_DIR_PARAM g_bf_filesystem_dir_param[FJ_CUSTOM_DIRECTORY_DEPTH];
static volatile INT32	g_bf_media_mount_ret;
static volatile INT32	g_bf_media_mount_end;
static volatile INT32	g_bf_media_dynamic_scenario_load = 0;
static volatile CHAR g_bf_filesystem_search_name[D_BF_FS_IF_FNAME_MAX];
/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
static VOID media_mount_callback(INT32 ret);
//static VOID media_format_callback(INT32 ret);
static INT32 media_set_drv_vol_label(CHAR* drv_name, CHAR* VolLabel);

//////// file local function ///////////////
/* close directory for FJ_Fs_DirectoryEntryGet */
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
static FJ_ERR_CODE file_close_directory( VOID )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	BF_Fs_If_Closedir( g_bf_filesystem_dir_param[g_bf_filesystem_dir_num - 1].dir_no );

	Dd_ARM_Critical_Section_Start( critseq );
	g_bf_filesystem_dir_param[g_bf_filesystem_dir_num - 1].dir_no = 0;
	memset( (FW_VOID*)g_bf_filesystem_dir_param[g_bf_filesystem_dir_num - 1].dir_path, '\0', 32 );
	g_bf_filesystem_dir_num--;
	Dd_ARM_Critical_Section_End( critseq );

	if(g_bf_filesystem_dir_num != 0){
		BF_Fs_If_Chdir( ".." );
	}


	return FJ_ERR_OK;
}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

//*********************
// callback of Mount
//*********************
VOID media_mount_callback(INT32 ret)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );

	if (ret == FSDCF_ERRNO_OK) {
		g_bf_media_mount_ret = FSDCF_ERRNO_OK;

	}
	else {
		g_bf_media_mount_ret = ret;
	}

	g_bf_media_mount_end = 1;

	Dd_ARM_Critical_Section_End( critseq );
}

/**
 * @brief 		Set volume label for drive
 * @param		i : CHAR*			drv_name : drive name
 *				i : CHAR*			VolLabel : volume label character string
 * @return 		FJ_ERR_CODE : FJ_ERR_OK
 *                          : FJ_ERR_NG
 * @note		None
 * @attention 	None
 */
static INT32 media_set_drv_vol_label(CHAR* drv_name, CHAR* VolLabel)
{
	INT32 ret;
	T_BF_FS_IF_STATVFS drv_stat;

	ret = BF_Fs_If_Statvfs((const FW_CHAR*)drv_name, &drv_stat);
	if(ret != 0){
		return ret;
	}

	if(drv_stat.dstat == 0x01){
		// protection unset
		ret = BF_Fs_If_Set_Protection_Mode((const FW_CHAR*)drv_name, E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT);
		if(ret != 0){
			return ret;
		}
	}

	// set volume label
	ret = BF_Fs_If_Set_VolLabel((const FW_CHAR*)drv_name, (const FW_CHAR*)VolLabel);
	if(ret != 0){
		return ret;
	}

	if(drv_stat.dstat == 0x01){
		// sync cache data
		ret = BF_Fs_If_Sync((const FW_CHAR*)drv_name);
		if(ret != 0){
			return ret;
		}

		// protection set
		ret = BF_Fs_If_Set_Protection_Mode((const FW_CHAR*)drv_name, E_BF_MEDIA_DRV_WRAPPER_WP_PROTECT);
		if(ret != 0){
			return ret;
		}
	}

	return ret;
}


/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/* all reset directory number for FJ_Fs_DirectoryEntryGet */
FJ_ERR_CODE BF_Filesystem_File_Reset_Directory_Number( VOID )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	UCHAR tmp;

	Dd_ARM_Critical_Section_Start( critseq );
	for(tmp = 0; tmp < FJ_CUSTOM_DIRECTORY_DEPTH; tmp++){
		g_bf_filesystem_dir_param[tmp].dir_no = 0;
		memset( (FW_VOID*)g_bf_filesystem_dir_param[tmp].dir_path, '\0', 32 );
	}

	g_bf_filesystem_dir_num = 0;

	Dd_ARM_Critical_Section_End( critseq );

	return FJ_ERR_OK;
}

/**
 * @brief		open the file.
 * @param		CHAR*	szFullFileName
 *				INT32	dwFlags
 * @return		success	:	File_Id
 *				fail	:	0
 * @note		FJ_FS_OPEN_RDONLY	0x0000  // Only enable the read access
				FJ_FS_OPEN_WRONLY	0x0001  // Only enable the write access
				FJ_FS_OPEN_RDWR		0x0002  // Enable the read/write access
				FJ_FS_OPEN_APPEND	0x0008  // After open the file, move the write operation file pointer to the end of file
				FJ_FS_OPEN_CREAT	0x0100  // Create new file when no file is existed
				FJ_FS_OPEN_TRUNC	0x0200  // set the file size to "0" when opening file (over write?)
 * @attention	None
 */
ULONG fj_fs_open(CHAR* szFullFileName, INT32 dwFlags)
{
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
	INT32 fs_file_id;

	fs_file_id = BF_Fs_If_Open((FW_CHAR*)szFullFileName, dwFlags, D_BF_FS_IF_ATTR_NORMAL);
	if (fs_file_id < 0) {
		BF_Debug_Print_Error(("BF_Fs_If_Open  Err: ErrorNo = %d \r\n", fs_file_id));
		return 0;
	}
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	UINT32 fs_file_id;

	fs_file_id = BF_Fs_If_Open((FW_CHAR*)szFullFileName, dwFlags, D_BF_FS_IF_ATTR_NORMAL);
	if (fs_file_id == 0) {
//		BF_Debug_Print_Error(("BF_Fs_If_Open  Error \r\n"));
		return 0;
	}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---
	else {
		return fs_file_id;
	}
}

/**
 * @brief		close the file.
 * @param		ULONG	ulFileId
 * @return		None
 * @note		None
 * @attention	None
 */
VOID fj_fs_close(ULONG ulFileId)
{
#ifdef FJ_DEBUG_PRINT_ON
	BF_Debug_Print_Information(("close file ID = %lX \r\n", ulFileId));
#endif

	BF_Fs_If_Close(ulFileId);
}

/**
 * @brief		read the file.
 * @param		ULONG	ulFileId
 *				BYTE*	pbyBuffer
 *				ULONG	ulRequestSize
 *				ULONG*	ulLength
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG 
 *				FJ_ERR_READ_EOF
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_read(ULONG ulFileId, BYTE* pbyBuffer, ULONG ulRequestSize, ULONG* ulLength)
{
	ULONG read_size;
	INT32 ret;
	FJ_ERR_CODE err_code;
	UINT32 tmp_addr, read_addr;
	
	tmp_addr = ( UINT32 )pbyBuffer;
	read_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( tmp_addr );
	
	ret = BF_Fs_If_Read( ulFileId, (VOID*)read_addr, ulRequestSize, &read_size );
	
	if ( ret == FSDCF_ERRNO_OK ) {
		*ulLength = read_size;
		err_code = FJ_ERR_OK;
	}
	else if( ret == D_BF_MDF_WRAPPER_ERR_EOF ) {
		*ulLength = read_size;
		err_code = FJ_ERR_READ_EOF;
	}
	else {
		*ulLength = 0;
		err_code = FJ_ERR_NG;
	}

	return err_code;
}

/**
 * @brief 		Check the status of the "DCIM" and "MISC" directory exist or not. 
 * @param		i:FJ_MEDIA_ID bMediaId
 * @param		o:BYTE* pDcimExist
 * @param		o:BYTE* pMiscExist
 * @return 		FJ_ERR_OK
 *				FJ_ERR_NG
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_fs_existrootdirectorycheck(FJ_MEDIA_ID bMediaId, UCHAR* pDcimExist, UCHAR* pMiscExist)
{
	UCHAR dir_attr;
	FW_INT32 ercd;
	FW_CHAR dir_name[16];

	/* get attribute of "DCIM" directory */
	strcpy(dir_name, (const FW_CHAR*)gBF_Controller_drv_name[bMediaId]);
	strcat(dir_name, ":\\DCIM");
	ercd = BF_Fs_If_Get_Attribute(dir_name, &dir_attr);
	if (ercd == D_BF_MDF_WRAPPER_ERR_NOENT) {
		/* Not exist "DCIM" directory */
		*pDcimExist = FJ_STATUS_DIR_NO_EXIST;
	}
	else if (ercd != FSDCF_ERRNO_OK) {
		/* get attribute error */
		return FJ_ERR_NG;
	}
	else {
		/* Exist "DCIM" directory */
		*pDcimExist = FJ_STATUS_DIR_EXIST;
	}

	/* get attribute of "\MISC" directory */
	strcpy(dir_name, (const FW_CHAR*)gBF_Controller_drv_name[bMediaId]);
	strcat(dir_name, ":\\MISC");
	ercd = BF_Fs_If_Get_Attribute(dir_name, &dir_attr);
	if (ercd == D_BF_MDF_WRAPPER_ERR_NOENT) {
		/* Not exist "MISC" directory */
		*pMiscExist = FJ_STATUS_DIR_NO_EXIST;
	}
	else if (ercd != FSDCF_ERRNO_OK) {
		/* get attribute error */
		return FJ_ERR_NG;
	}
	else {
		/* Exist "MISC" directory */
		*pMiscExist = FJ_STATUS_DIR_EXIST;
	}

	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_COMM_CODE"
/////////////////////////////////////////////////////
// romarea_comm
/////////////////////////////////////////////////////

/* all close directory for FJ_Fs_DirectoryEntryGet */
FJ_ERR_CODE BF_Filesystem_File_Reset_Directory( VOID )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	if(g_bf_filesystem_dir_num > 0){
		BF_Fs_If_Chdir( (const FW_CHAR*)g_bf_filesystem_dir_param[0].dir_path );
	}

	while( g_bf_filesystem_dir_num > 0 ){
		BF_Fs_If_Closedir( g_bf_filesystem_dir_param[g_bf_filesystem_dir_num - 1].dir_no );

		Dd_ARM_Critical_Section_Start( critseq );
		g_bf_filesystem_dir_param[g_bf_filesystem_dir_num - 1].dir_no = 0;
		memset( (FW_VOID*)g_bf_filesystem_dir_param[g_bf_filesystem_dir_num - 1].dir_path, '\0', 32 );

		g_bf_filesystem_dir_num--;
		Dd_ARM_Critical_Section_End( critseq );
	}

	return FJ_ERR_OK;
}

/**
 * @brief		copy the files to NF data drive
 * @param		none
 * @return 		Success/Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_filecopytodrive(FJ_MEDIA_ID bMediaId)
{
	FW_INT32 ercd;
	UCHAR dir_attr;
	CHAR src_drv_name[9], dst_drv_name[9], path_name[13];
	UCHAR media_cnt, exist_cnt = 0;
	
	strcpy(src_drv_name, (const CHAR*)gBF_Controller_drv_name[bMediaId]);			// SD drive 0/1
	
	for(media_cnt = 0; media_cnt < FJ_MEDIA_SD0; media_cnt++){
		if(gBF_Controller_drv_name[media_cnt]) {
			strcpy(dst_drv_name, (const CHAR*)gBF_Controller_drv_name[media_cnt]);
			strcpy(path_name, src_drv_name);
			strcat(path_name, ":\\");
			strcat(path_name, dst_drv_name);

			ercd = BF_Fs_If_Get_Attribute(path_name, &dir_attr);
			if (ercd != D_BF_MDF_WRAPPER_ERR_NOENT) {
				ercd = BF_Fs_If_Copy_File_To_Drive( src_drv_name,  dst_drv_name);	// Copy to NF drive
				if (ercd != FSDCF_ERRNO_OK) {
					return FJ_ERR_NG;
				}
				else{
					exist_cnt++;
				}
			}
			else {
				BF_Debug_Print_Warning(("%s:\\ not exist\n", gBF_Controller_drv_name[media_cnt]));
			}
		}
	}

	if(exist_cnt == 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/////////////////////////////////////////////////////
// romarea_view
/////////////////////////////////////////////////////
//#pragma arm section code="BASEFW_VIEW_CODE"

/**
 * @brief		write the file
 * @param		ULONG	ulFileId
 *				UINT32*	pbyBuffer
 *				ULONG	ulRequestSize
 *				ULONG*	ulLength
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_write(ULONG ulFileId, UINT32* pbyBuffer, ULONG ulRequestSize, ULONG* ulLength)
{
	INT32 ret;
	UINT32 tmp_addr, write_addr;

	tmp_addr = ( UINT32 )pbyBuffer;
	write_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( tmp_addr );

	ret = BF_Fs_If_Write(ulFileId, (VOID*)write_addr, ulRequestSize, ulLength);

	if (ret == FSDCF_ERRNO_OK) {
		return FJ_ERR_OK;
	}
	else {
		return FJ_ERR_NG;
	}
}

/**
 * @brief		write the file(async)
 * @param		ULONG	ulFileId
 *				BYTE*	pbyBuffer
 *				ULONG	ulRequestSize
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_writeasync(FsEntryCB pcallback, ULONG ulFileId, BYTE* pbyBuffer, ULONG ulRequestSize)
{
	INT32 ret;
	UINT32 tmp_addr, write_addr;

	tmp_addr = ( UINT32 )pbyBuffer;
	write_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( tmp_addr );

	ret = BF_Fs_If_Write_As((FS_ENTRY)pcallback, ulFileId, (VOID*)write_addr, ulRequestSize);

	if (ret == FSDCF_ERRNO_OK) {
		return FJ_ERR_OK;
	}
	else {
		return FJ_ERR_NG;
	}
}

/**
 * @brief		Get the write length
 * @param		ULONG*	ulLength
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_getwritelengthas(ULONG* ulLength)
{
	INT32 ret;

	ret = BF_Fs_If_Get_Result_Write_As(ulLength);

	if (ret == FSDCF_ERRNO_OK) {
		return FJ_ERR_OK;
	}
	else {
		return FJ_ERR_NG;
	}
}

/**
 * @brief		seek the file (Only as for the original course).
 * @param		ULONG	ulFileId
 *				ULONG	ulOffset
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_lseekwithlimit(ULONG ulFileId, ULONG ulOffset)
{
	ULONG length;
	INT32 ret;
	T_BF_FS_IF_STAT file_state;

	BF_Fs_If_Fstat(ulFileId, &file_state);

	ret = BF_Fs_If_Lseek(ulFileId, ulOffset, D_BF_FS_IF_SEEK_CUR, &length);

	if (length >= file_state.fileSize) {
		ret = BF_Fs_If_Lseek(ulFileId, 0, D_BF_FS_IF_SEEK_END, &length);
	}

#ifdef FJ_DEBUG_PRINT_ON
	BF_Debug_Print_Information(("Offset is %lu , length is %lu \r\n", ulOffset, length));
#endif

	if (ret == FSDCF_ERRNO_OK) {
		return FJ_ERR_OK;
	}
	else {
		return FJ_ERR_NG;
	}
}

/**
 * @brief		delete the file/directory
 * @param		CHAR*	szTarget
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_remove(CHAR* szTarget)
{
	INT32 ret;
	FW_UCHAR attri;

	ret = BF_Fs_If_Get_Attribute((const FW_CHAR*)szTarget, &attri);
	if (ret != FSDCF_ERRNO_OK) {
		return FJ_ERR_NG;
	}

	if (attri == 0x10) {
		ret = BF_Fs_If_Rmdir((const FW_CHAR*)szTarget);

#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Information(("delete directory \n"));
#endif
	}
	else {
		ret = BF_Fs_If_Remove((const FW_CHAR*)szTarget);

#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Information(("delete file \r\n"));
#endif
	}

	if (ret != FSDCF_ERRNO_OK) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}
/**
 * @brief		delete the file/directory
 * @param 		CHAR*	szTarget 
 *				CHAR	filename_list[][13] 
 * 				INT32	iFnum 
 * 				FsDeleteMultiCB	pcallback 
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_removemulti(CHAR* szTarget, CHAR filename_list[][13], INT32 fnum,  FsDeleteMultiCB pcallback)
{
	INT32 ret;

	ret = BF_Fs_If_Remove_Multi((const FW_CHAR*)szTarget, (FW_CHAR (*)[13])filename_list, (FW_INT32)fnum, (FS_DEL_MULTI_CB)pcallback);

	if (ret != FSDCF_ERRNO_OK) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/////////////////////////////////////////////////////
// romarea_last
/////////////////////////////////////////////////////
//#pragma arm section code="BASEFW_LAST_CODE"

#if 0		// unused
/**
 * @brief		seek the file.
 * @param		ULONG	ulFileId
 *				ULONG	ulOffset
 *				BYTE	byDir
 * @return		success	:	1
 *				fail	:	0
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_directfsfilerelseek(ULONG ulFileId, ULONG ulOffset, BYTE byDir)
{
	ULONG length;
	INT32 ulOffset_int;
	INT32 ret;
	T_BF_FS_IF_STAT file_state;

	BF_Fs_If_Fstat(ulFileId, &file_state);

	ulOffset_int = ulOffset;

	if (ulOffset_int & 0x80000000) {
		ulOffset_int = 0x7FFFFFFF;
	}

	if (byDir == 0) {
		// if plus
		ret = BF_Fs_If_Lseek(ulFileId, ulOffset_int, D_BF_FS_IF_SEEK_CUR, &length);
	}
	else if (byDir == 1) {
		// if minus
		ulOffset_int = ~ulOffset_int + 1;
		ret = BF_Fs_If_Lseek(ulFileId, ulOffset_int, D_BF_FS_IF_SEEK_CUR, &length);
	}
	else {
		return FJ_ERR_NG;
	}

	if (length >= file_state.fileSize) {
		ret = BF_Fs_If_Lseek(ulFileId, 0, D_BF_FS_IF_SEEK_END, &length);
	}

#ifdef FJ_DEBUG_PRINT_ON
	BF_Debug_Print_Information(("Offset is %d , length is %ld \r\n", ulOffset_int, length));
#endif

	if (ret == FSDCF_ERRNO_OK) {
		return FJ_ERR_OK;
	}
	else {
		return FJ_ERR_NG;
	}
}

/**
 * @brief		reads a line into a buffer.
 * @param		ULONG	ulFileId
 *				BYTE*	pbyBuffer
 *				ULONG	ulBufferLen
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention	None
 */
ULONG fj_directfsfilegetline(ULONG ulFileId, BYTE* pbyBuffer, ULONG ulBufferLen)
{
	FJ_ERR_CODE err_code = FJ_ERR_NG;
	static FW_VOID*	buf = NULL;
	ULONG string_count = 0;
	static CHAR* string = NULL;
	static INT32 end_of_file = 0;
	static INT32 read_flag = 0;
	UINT32 tmp_addr, read_addr;
	
	tmp_addr = ( UINT32 )pbyBuffer;
	read_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( tmp_addr );
	
	memset(read_addr, 0, ulBufferLen);

	if (read_flag == 0) {
		BF_Fs_User_Custom_Get_Mpl(MPKID_EXIF, 0x1000, &buf);
		string = buf;

		err_code = fj_fs_read(ulFileId, (BYTE*)string, 4096, &end_of_file);
		read_flag = 1;

		if (end_of_file <= 0) {
			read_flag = 0;
			return err_code;
		}
	}

	if (string == NULL) {
		read_flag = 0;
		return err_code;
	}

	while (((*string != '\n') && (*string != '\0') && (*string != '\r')) && (string_count != ulBufferLen) && (end_of_file > 0)) {
		string++;
		string_count++;
		end_of_file--;
	}

	if (string_count != 0) {
		memcpy(read_addr, (string - string_count), string_count);
	}

	if (*string == '\r') {
		string += 2;
		end_of_file -= 2;
	}
	else if (*string == '\n') {
		string++;
		end_of_file--;
	}
	else if (*string == '\0') {
		string++;
		end_of_file--;
	}
	else {
		; //read_flag = 0;
	}

	if (end_of_file <= 0) {
		if (buf == NULL) {
			return err_code;
		}
//		string = buf;
//		memset(string, 0, 10);
		read_flag = 0;
		BF_Fs_User_Custom_Rel_Mpl(MPKID_EXIF, &buf);
	}

#ifdef FJ_DEBUG_PRINT_ON
	BF_Debug_Print_Information(("read line is %s \r\n", read_addr));
#endif

	return string_count;
}
#endif	// unused

/**
 * @brief		creaqte the directory
 * @param		CHAR*	szTarget
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_mkdir(CHAR* szTarget)
{
	INT32 ret;

	ret = BF_Fs_If_Mkdir((const FW_CHAR*)szTarget);

	if (ret == FSDCF_ERRNO_OK) {
		return FJ_ERR_OK;
	}
	else {
		return FJ_ERR_NG;
	}
}

/**
 * @brief		changes the path from the current directory.
 * @param		CHAR*	szInputPath
 *				CHAR*	szOutputPath
 *				UINT32	ulOutputLen
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_chdir(CHAR* szInputPath)
{
	INT32 ret;
//	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
//	CHAR* szOutputPath = 0;
//	CHAR media[9] = { 0 };
//	CHAR flag = 0;
//	FJ_MEDIA_ID media_id;

//	if (g_bf_filesystem_dir_num > 0) {
//		BF_Filesystem_File_Reset_Directory();
//		flag = 1;
//	}

	ret = BF_Fs_If_Chdir((FW_CHAR*)szInputPath);
	if (ret != FSDCF_ERRNO_OK) {
		return FJ_ERR_NG;
	}

//	ret = BF_Fs_If_Get_Cur_Drv((FW_CHAR*)media);
//	if (ret != FSDCF_ERRNO_OK) {
//		return FJ_ERR_NG;
//	}
//
////	BF_Fs_If_Get_Cur_Dir((FW_CHAR*)media, (FW_CHAR*)szOutputPath, 32);
//	
//	BF_Fs_Controller_Get_Media_Id(media, (CHAR*)&media_id);
//	
//	if (media_id == FJ_MEDIA_SD0 || media_id == FJ_MEDIA_SD1) {
//		if (flag == 1) {
//			BF_Fs_If_Get_Cur_Dir((FW_CHAR*)media, (FW_CHAR*)g_bf_filesystem_dir_param[0].dir_path, 32);
//			ret = BF_Fs_If_Opendir((FW_CHAR*)g_bf_filesystem_dir_param[0].dir_path, (FW_INT32*)&g_bf_filesystem_dir_param[0].dir_no);
//
//			Dd_ARM_Critical_Section_Start( critseq );
//			g_bf_filesystem_dir_num++;
//			Dd_ARM_Critical_Section_End( critseq );
//
//			if (ret != FSDCF_ERRNO_OK) {
//				BF_Debug_Print_Error(("BF_Fs_If_Opendir error (%d)\n", ret));
//				return FJ_ERR_OK;
//			}
//		}
//	}

	return FJ_ERR_OK;
}

/**
 * @brief		rename the file/directory
 * @param		CHAR*	szSource
 *				CHAR*	szTarge
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_rename(CHAR* szSource, CHAR* szTarget)
{
	INT32 ret;
//	CHAR tmp;
//	FW_UCHAR attri;
//
//	ret = BF_Fs_If_Get_Attribute((const FW_CHAR*)szSource, &attri);
//	if (ret != FSDCF_ERRNO_OK) {
//		return FJ_ERR_NG;
//	}
//
//	if (attri == 0x10) {	/* directory */
//		if (g_bf_filesystem_dir_num > 0) {
//			for (tmp = g_bf_filesystem_dir_num; tmp > 0; tmp--) {
//				if (strcmp((char*)szSource, (char*)g_bf_filesystem_dir_param[tmp - 1].dir_path) == 0) {
//					BF_Filesystem_File_Reset_Directory();
//					break;
//				}
//			}
//		}
//
//		BF_Fs_If_Chdir((FW_CHAR*)szSource);
//		BF_Fs_If_Chdir("..");
//	}

	ret = BF_Fs_If_Rename((const FW_CHAR*)szSource, (const FW_CHAR*)szTarget);
	if (ret == FSDCF_ERRNO_OK) {
		return FJ_ERR_OK;
	}
	else {
		return FJ_ERR_NG;
	}
}

/**
 * @brief		compare entry file
 * @param		CHAR* org_name;
 *				CHAR* comp_name;
 * @return		error code
 * @note		None
 * @attention	None
 */
INT32 file_dir_name_compare(CHAR* org_name, CHAR* comp_name)
{
//	printf("file_dir_name_compare :\n");
//	printf("org_name = %s, comp_name = %s :\n", org_name, comp_name);
	
	switch(*org_name) {
		case '\0':
			return ('\0' == *comp_name);
		case '*':
			return (file_dir_name_compare(org_name+1, comp_name)
					|| (('\0' != *comp_name) && file_dir_name_compare(org_name, comp_name+1)));
		case '?':
			return (('\0' != *comp_name) && file_dir_name_compare(org_name+1, comp_name+1));
		default:
			return ((unsigned char)*org_name == (unsigned char)*comp_name
					&& file_dir_name_compare(org_name+1, comp_name+1));
	}
}

/**
 * @brief		get the entry
 * @param		BOOL	bFirstEntry
 *				CHAR*	szOutput
 *				UINT32	uiOutputlen
 *				UINT32*	puiAttributes
 *				ULONG*	pulSize
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
FJ_ERR_CODE fj_directfsdirectoryentryget(
				INT32	bFirstEntry,
				CHAR*	szOutput,
				UINT32	uiOutputLen,
				UINT32*	puiAttributes,
				ULONG*	pulSize)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	INT32 ret;
	FW_CHAR media[9] = {0};
	T_BF_FS_IF_Dir_Info dirent = {{0}};
	CHAR* srch_ptr;
	CHAR* srch_ptr2;

	// get current drive
	ret = BF_Fs_If_Get_Cur_Drv(media);
	if (ret != FSDCF_ERRNO_OK) {
		BF_Filesystem_File_Reset_Directory();
		return FJ_ERR_NG;
	}

	if (bFirstEntry == FJ_FS_ENTRY_MODE_END) {
		// Reset and end
		BF_Filesystem_File_Reset_Directory();
		*puiAttributes = 0;
		*pulSize = 0;

#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Information(("reset entry\n"));
#endif

		return FJ_ERR_OK;
	}
	else if (bFirstEntry == FJ_FS_ENTRY_MODE_START) {
		// Start entry get
		BF_Filesystem_File_Reset_Directory();

		// Get current directory path
		BF_Fs_If_Get_Cur_Dir(media, (FW_CHAR*)g_bf_filesystem_dir_param[0].dir_path, 32);

		// Open current directory
		ret = BF_Fs_If_Opendir((FW_CHAR*)g_bf_filesystem_dir_param[0].dir_path, (FW_INT32*)&g_bf_filesystem_dir_param[0].dir_no);
		if (ret != FSDCF_ERRNO_OK) {
			return FJ_ERR_NG;
		}
		Dd_ARM_Critical_Section_Start( critseq );
		g_bf_filesystem_dir_num++;
		Dd_ARM_Critical_Section_End( critseq );
	}
	else if (bFirstEntry == FJ_FS_ENTRY_MODE_NEXT) {	// NEXT
		if (g_bf_filesystem_dir_num == 0) {
			// not open directory
			return FJ_ERR_NG;
		}
	}
	else if (bFirstEntry == FJ_FS_ENTRY_MODE_NEXT_NOT_SUB) {	// NEXT_NOT_SUB
		if (g_bf_filesystem_dir_num == 0) {
			// not open directory
			return FJ_ERR_NG;
		}
	}
	else {
		; // DO NOTHING
	}

	while(ret == 0) {
		// get entry
		ret = BF_Fs_If_Readdir(g_bf_filesystem_dir_param[g_bf_filesystem_dir_num - 1].dir_no, &dirent);
		while (ret == D_BF_MDF_WRAPPER_ENT_LAST) {
			// Finished reading current directory

			file_close_directory();

			if (g_bf_filesystem_dir_num == 0) {
#ifdef FJ_DEBUG_PRINT_ON
				BF_Debug_Print_Information(("all entry get \r\n"));
#endif
				*puiAttributes = 0;
				*pulSize = 0;
				return FJ_ERR_LAST_ENTRY;
			}

			if (bFirstEntry != FJ_FS_ENTRY_MODE_NEXT_NOT_SUB) {
				// Read next directory entry
				ret = BF_Fs_If_Readdir(g_bf_filesystem_dir_param[g_bf_filesystem_dir_num - 1].dir_no, &dirent);
				if ((ret != D_BF_MDF_WRAPPER_ENT_LAST) && (ret != FSDCF_ERRNO_OK)) {
					BF_Debug_Print_Error(("error \r\n"));
					BF_Filesystem_File_Reset_Directory();
					return FJ_ERR_NG;
				}
			}
		}

		if (ret != FSDCF_ERRNO_OK) {
			BF_Filesystem_File_Reset_Directory();
			return FJ_ERR_NG;
		}

		if (bFirstEntry == FJ_FS_ENTRY_MODE_NEXT) {
			if (((dirent.attributes & 0x10) == 0x10) && (strcmp( dirent.fname, ".") != 0) && (strcmp( dirent.fname, ".." ) != 0)) {
				// entry is directory(not self directory, not parent directory)

				if (g_bf_filesystem_dir_num < FJ_CUSTOM_DIRECTORY_DEPTH) {
					// Possible next directory read

					// current directory change next directory
					ret = BF_Fs_If_Chdir((const FW_CHAR*)dirent.fname);
					if (ret != FSDCF_ERRNO_OK) {
						BF_Fs_If_Closedir(g_bf_filesystem_dir_param[g_bf_filesystem_dir_num - 1].dir_no);
						return FJ_ERR_NG;
					}
					BF_Fs_If_Get_Cur_Dir(media, (FW_CHAR*)g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_path, 32);
					ret = BF_Fs_If_Opendir((FW_CHAR*)g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_path, (FW_INT32*)&g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_no);
					if (ret == FSDCF_ERRNO_OK) {
						Dd_ARM_Critical_Section_Start( critseq );
						g_bf_filesystem_dir_num++;
						Dd_ARM_Critical_Section_End( critseq );
					}
				}
			}
		}

//		printf("dirent.fname = %s\n", dirent.fname);
		if(*g_bf_filesystem_search_name == '\0') {
			if((strcmp( dirent.fname, ".") != 0) && (strcmp( dirent.fname, ".." ) != 0)){
				break;
			}
		}

		srch_ptr = strstr((const CHAR*)g_bf_filesystem_search_name, "*");
		srch_ptr2 = strstr((const CHAR*)g_bf_filesystem_search_name, "?");
		
		if((srch_ptr == NULL) && (srch_ptr2 == NULL)) {	/* not wild card */
//			printf("fj_directfsdirectoryentryget : \n");
//			printf("dirent.fname = %s\n", dirent.fname);
			if(strcmp((const CHAR*)g_bf_filesystem_search_name, dirent.fname) == 0) {
				break;
			}
		}
		else{	/* wild card */
			if(file_dir_name_compare((CHAR*)g_bf_filesystem_search_name, dirent.fname)) {
				break;
			}
		}
	}

	memcpy((FW_VOID*)szOutput, &dirent.fname, uiOutputLen);

	*puiAttributes = (UINT32)dirent.attributes;
	*pulSize = (ULONG)dirent.fileSize;

	return FJ_ERR_OK;
}

/**
 * @brief 		This function sets the entry name of when searching for files or directory.
 * @param		CHAR* szEntryName
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_fs_setsearchentry(const CHAR* szEntryName)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	CHAR* find_ptr;
	CHAR* srch_ptr;
	INT32 path_len;
	INT32 ret;
	CHAR dirname[D_BF_FS_IF_FNAME_MAX] ;
	CHAR drv_name[9] = {'0'};
	
	/* Check Path or File name */
	strcpy((CHAR*)dirname, szEntryName);
	srch_ptr = (CHAR*)dirname;
	find_ptr = strstr((CHAR*)dirname, "\\");

	if( find_ptr != NULL ) { /* Dir exist */
		path_len = strlen(srch_ptr);
		srch_ptr = srch_ptr + path_len;

		while(*srch_ptr != '\\') {
			srch_ptr--;
		}
		
		Dd_ARM_Critical_Section_Start( critseq );
		*srch_ptr = '\0';
		strcpy((CHAR*)g_bf_filesystem_search_name, srch_ptr+1);

		if(strcmp((const CHAR*)g_bf_filesystem_search_name, "*") == 0){
			*g_bf_filesystem_search_name = '\0';
		}
		Dd_ARM_Critical_Section_End( critseq );

		/* Change Directory */
		strcat(dirname, "\\");

		find_ptr = strchr((CHAR*)dirname, ':');
		if (find_ptr != NULL) {
			strncpy(drv_name, (const CHAR*)dirname, (find_ptr - (CHAR*)dirname)+1);
			ret = BF_Fs_If_Chdir(drv_name);
			if (ret != FSDCF_ERRNO_OK) {
				return FJ_ERR_NG;
			}
		}

		ret = BF_Fs_If_Chdir(dirname);
		if (ret != FSDCF_ERRNO_OK) {
			return FJ_ERR_NG;
		}
	}
	else{ /* Dir not exist */
		if((strcmp(szEntryName, "*") == 0) || (strcmp(szEntryName, "\0") == 0)){
			Dd_ARM_Critical_Section_Start( critseq );
			*g_bf_filesystem_search_name = '\0';
			Dd_ARM_Critical_Section_End( critseq );
			return FJ_ERR_OK;
		}

		Dd_ARM_Critical_Section_Start( critseq );
		strcpy((CHAR*)g_bf_filesystem_search_name, szEntryName);
		Dd_ARM_Critical_Section_End( critseq );
	}

	return FJ_ERR_OK;
}

#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
FJ_ERR_CODE fj_directfsdirectoryentryget(
				INT32	bFirstEntry,
				CHAR*	szOutput,
				UINT32	uiOutputLen,
				UINT32*	puiAttributes,
				ULONG*	pulSize)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	INT32 ret;
	CHAR file_path[D_BF_FS_IF_FNAME_MAX];
	T_BF_FS_IF_ENTRY dirent = {0};
	UINT32	name_len;

	while(1) {
		if (bFirstEntry == FJ_FS_ENTRY_MODE_START) {
			Dd_ARM_Critical_Section_Start( critseq );
			g_bf_filesystem_dir_num = 0;
			strcpy((CHAR*)g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_path, "./");
			Dd_ARM_Critical_Section_End( critseq );

			// search first
			strcpy(file_path, (const CHAR*)g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_path);
			strcat(file_path, (const CHAR*)g_bf_filesystem_search_name);
			ret = BF_Fs_If_Fsfirst(g_bf_filesystem_dir_num, (const CHAR*)file_path, D_BF_FS_IF_ATTR_ENTRY, &dirent);
		}
		else if (bFirstEntry == FJ_FS_ENTRY_MODE_NEXT) {
			// check other directory
			if(g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_no == 1) {
				// search first
				strcpy(file_path, (const CHAR*)g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_path);
				strcat(file_path, (const CHAR*)g_bf_filesystem_search_name);
				ret = BF_Fs_If_Fsfirst(g_bf_filesystem_dir_num, (const CHAR*)file_path, D_BF_FS_IF_ATTR_ENTRY, &dirent);

				Dd_ARM_Critical_Section_Start( critseq );
				g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_no = 0;
				Dd_ARM_Critical_Section_End( critseq );
			}
			else {
				// search next
				ret = BF_Fs_If_Fsnext(g_bf_filesystem_dir_num, &dirent);
			}
		}
		else if (bFirstEntry == FJ_FS_ENTRY_MODE_NEXT_NOT_SUB) {
			// check other directory
			if(g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_no == 1) {
				Dd_ARM_Critical_Section_Start( critseq );
				g_bf_filesystem_dir_num--;
				g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_no = 0;
				Dd_ARM_Critical_Section_End( critseq );
			}
			// search next
			ret = BF_Fs_If_Fsnext(g_bf_filesystem_dir_num, &dirent);
		}
		else if (bFirstEntry == FJ_FS_ENTRY_MODE_END) {
			// search end
			BF_Filesystem_File_Reset_Directory_Number();
			*puiAttributes = 0;
			*pulSize = 0;
#ifdef FJ_DEBUG_PRINT_ON
			BF_Debug_Print_Information(("reset entry\n"));
#endif
			return FJ_ERR_OK;
		}
		else {
			// illegal entry mode
			return FJ_ERR_NG;
		}

		// check previous directory
		if (ret != FSDCF_ERRNO_OK) {
			if (g_bf_filesystem_dir_num != 0) {
				Dd_ARM_Critical_Section_Start( critseq );
				g_bf_filesystem_dir_num--;
				Dd_ARM_Critical_Section_End( critseq );
				// continue searching...
				continue;
			}
			if (ret == D_BF_MDF_WRAPPER_ENT_LAST) {
#ifdef FJ_DEBUG_PRINT_ON
				BF_Debug_Print_Information(("all entry get \r\n"));
#endif
				return FJ_ERR_LAST_ENTRY;
			}
			return FJ_ERR_NG;
		}

		// check sub directory
		if ((dirent.fs_attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
			if((strcmp( dirent.fs_fname, ".") != 0) && (strcmp( dirent.fs_fname, ".." ) != 0)) {
				// save directory information
				Dd_ARM_Critical_Section_Start( critseq );
				g_bf_filesystem_dir_num++;
				g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_no = 1;

				name_len = strlen(dirent.fs_fname);
				strcpy((CHAR*)g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_path, (const CHAR*)g_bf_filesystem_dir_param[g_bf_filesystem_dir_num-1].dir_path);
				strncat((CHAR*)g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_path, dirent.fs_fname, name_len);
				strcat((CHAR*)g_bf_filesystem_dir_param[g_bf_filesystem_dir_num].dir_path, "/");
				Dd_ARM_Critical_Section_End( critseq );
			}
			else {
				// continue searching...
				if (bFirstEntry == FJ_FS_ENTRY_MODE_START) {
					bFirstEntry = FJ_FS_ENTRY_MODE_NEXT;
				}
				continue;
			}
		}
		// finish searching
		break;
	}
	// search success
	strncpy((FW_VOID*)szOutput, (const CHAR*)dirent.fs_fname, uiOutputLen);
	*puiAttributes = (UINT32)(dirent.fs_attributes & ~D_BF_FS_IF_ATTR_NONE);
	*pulSize = (ULONG)dirent.fs_filesize;

	return FJ_ERR_OK;
}

/**
 * @brief 		This function sets the entry name of when searching for files or directory.
 * @param		CHAR* szEntryName
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_fs_setsearchentry(const CHAR* szEntryName)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	CHAR* find_ptr;
	CHAR* srch_ptr;
	INT32 path_len;
	INT32 ret;
	CHAR dirname[D_BF_FS_IF_FNAME_MAX] ;
	CHAR drv_name[9] = {'0'};
	
	/* Check Path or File name */
	strcpy((CHAR*)dirname, szEntryName);
	srch_ptr = (CHAR*)dirname;
	find_ptr = strstr((CHAR*)dirname, "\\");

	if( find_ptr != NULL ) { /* Dir exist */
		path_len = strlen(srch_ptr);
		srch_ptr = srch_ptr + path_len;

		while(*srch_ptr != '\\') {
			srch_ptr--;
		}
		
		Dd_ARM_Critical_Section_Start( critseq );
		*srch_ptr = '\0';
		strcpy((CHAR*)g_bf_filesystem_search_name, srch_ptr+1);
		Dd_ARM_Critical_Section_End( critseq );

		/* Change Directory */
		find_ptr = strchr((CHAR*)dirname, ':');
		if (find_ptr != NULL) {
			strncpy(drv_name, (const CHAR*)dirname, (find_ptr - (CHAR*)dirname)+1);
			strcat(drv_name, "\\");
			ret = BF_Fs_If_Chdir(drv_name);
			if (ret != FSDCF_ERRNO_OK) {
				return FJ_ERR_NG;
			}
		}

		ret = BF_Fs_If_Chdir(dirname);
		if (ret != FSDCF_ERRNO_OK) {
			return FJ_ERR_NG;
		}
	}
	else{ /* Dir not exist */
		Dd_ARM_Critical_Section_Start( critseq );
		strcpy((CHAR*)g_bf_filesystem_search_name, szEntryName);
		Dd_ARM_Critical_Section_End( critseq );
	}

	if(strcmp((const CHAR*)g_bf_filesystem_search_name, "\0") == 0){
		Dd_ARM_Critical_Section_Start( critseq );
		strcpy((CHAR*)g_bf_filesystem_search_name, "*");
		Dd_ARM_Critical_Section_End( critseq );
	}

	return FJ_ERR_OK;
}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---


/**
 * @brief		This function returns entries(file or directory) information of the current directory one by one.
 * @param		CHAR*	szName
 *				UINT32*	puiAttributes
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		0x00	: normal file
   				0x01	: read only file
   				0x02	: hidden file
   				0x04	: system file
   				0x10	: directory
   				0x20	: archive file
 * @attention	None
 */
FJ_ERR_CODE fj_fs_getattribute(CHAR* szName, UINT32* puiAttributes)
{
	INT32 ret;
	FW_UCHAR attri;

	ret = BF_Fs_If_Get_Attribute((const FW_CHAR*)szName, &attri);
	*puiAttributes = attri;
	if (ret == FSDCF_ERRNO_OK) {
		return FJ_ERR_OK;
	}
	else {
		return FJ_ERR_NG;
	}
}

/**
 * @brief		set attributes of file/directory
 * @param		CHAR*	szName
 *				UINT32*	puiAttributes
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		0x00	: normal file
   				0x01	: read only file
   				0x02	: hidden file
   				0x04	: system file
   				0x10	: directory
   				0x20	: archive file
 * @attention	None
 */
FJ_ERR_CODE fj_fs_setattribute(CHAR* szName, UINT32 puiAttributes)
{
	INT32 ret;
	FW_UCHAR attri;

	attri = (FW_UCHAR)puiAttributes;

	ret = BF_Fs_If_Set_Attribute((const FW_CHAR*)szName, attri);
	if (ret == FSDCF_ERRNO_OK) {
		return FJ_ERR_OK;
	}
	else {
		return FJ_ERR_NG;
	}
}

/**
 * @brief		set attributes of file/directory
 * @param		CHAR*	szName
 *				UINT32	puiAttributes
 *				UCHAR 	ucAttrmode
 *				CHAR	filename_list[][13]
 *				INT32	iFnum
 *				FsAttributesSetMultiCB	pcallback
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		attribute
				0x00	: normal file
   				0x01	: read only file
   				0x02	: hidden file
   				0x04	: system file
   				0x10	: directory
   				0x20	: archive file
				attribute mode
				0		: attribute set
				1		: attribute add
				2		: attribute remove
 * @attention	None
 */
FJ_ERR_CODE fj_fs_setattributemulti(CHAR* szName, UINT32 puiAttributes, UCHAR ucAttrmode, CHAR filename_list[][13], INT32 fnum, FS_SET_ATTR_MULTI_CB pcallback)
{
	INT32 ret;
	FW_UCHAR attri;

	attri = (FW_UCHAR)puiAttributes;

	ret = BF_Fs_If_Set_Attribute_Multi((const FW_CHAR*)szName, attri, (FW_UCHAR)ucAttrmode, (FW_CHAR (*)[13])filename_list, (FW_INT32)fnum, (FS_SET_ATTR_MULTI_CB)pcallback);
	if (ret == FSDCF_ERRNO_OK) {
		return FJ_ERR_OK;
	}
	else {
		return FJ_ERR_NG;
	}
}

FJ_ERR_CODE fj_fs_filesave(const char* szFullFileName, UINT32* addr, INT32 size)
{
	FW_INT32 FileID;
	FW_INT32 err;
	FW_ULONG result;
	UINT32 tmp_addr, write_addr;

	if ((szFullFileName == NULL) || (addr == NULL)) {
		return FJ_ERR_NG;
	}
	if (size == 0) {
		return FJ_ERR_NG;
	}

	FileID = BF_Fs_If_Create(szFullFileName, D_BF_FS_IF_ATTR_NORMAL);
	if (FileID == 0) {
		return FJ_ERR_NG;
	}

	tmp_addr = ( UINT32 )addr;
	write_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( tmp_addr );

	err = BF_Fs_If_Write(FileID, (FW_VOID*)write_addr, size, &result);

	if (err != 0) {
		BF_Mdf_Wrapper_Close(FileID);
		return FJ_ERR_NG;
	}

	err = BF_Fs_If_Close(FileID);
	if (err != 0) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 * @brief 		Get the root entries count for FAT12 & FAT16.
 * @param		ULONG	*count
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention 	None
 */
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
FJ_ERR_CODE fj_fs_rootentriescount(ULONG* count)
{
	INT32 ercd, dir_no;
	FW_CHAR media[9] = { 0 };
	T_BF_FS_IF_Dir_Info dirent;

	*count = 0;

	BF_Filesystem_File_Reset_Directory();

	ercd = BF_Fs_If_Get_Cur_Drv(media);
	if (ercd != FSDCF_ERRNO_OK) {
		return FJ_ERR_NG;
	}

	strcat(media, ":\\");

	ercd = BF_Fs_If_Opendir(media, &dir_no);
	if (ercd != FSDCF_ERRNO_OK) {
		return FJ_ERR_NG;
	}

	while (1) {
		ercd = BF_Fs_If_Readdir(dir_no, &dirent);
		if (ercd == D_BF_MDF_WRAPPER_ENT_LAST) {
			ercd = FJ_ERR_OK;
			break;
		}
		else if (ercd != FSDCF_ERRNO_OK) {
			*count = 0;
			ercd = FJ_ERR_NG;
			break;
		}
		else {
			; // DO NOTHING
		}

		*count += 1;
	}

	BF_Fs_If_Closedir(dir_no);

	return (FJ_ERR_CODE)ercd;
}
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
FJ_ERR_CODE fj_fs_rootentriescount(ULONG* count)
{
	INT32 ercd;
	T_BF_FS_IF_ENTRY dirent;

	*count = 0;
	ercd = BF_Fs_If_Fsfirst(0, (const CHAR*)"/*", D_BF_FS_IF_ATTR_ALL, &dirent);
	if (ercd == D_BF_MDF_WRAPPER_ENT_LAST) {
		return FJ_ERR_OK;
	}
	else if (ercd != FSDCF_ERRNO_OK) {
		return FJ_ERR_NG;
	}
	else {
		; // DO NOTHING
	}
	*count += 1;

	while (1) {
		ercd = BF_Fs_If_Fsnext(0, &dirent);
		if (ercd == D_BF_MDF_WRAPPER_ENT_LAST) {
			ercd = FJ_ERR_OK;
			break;
		}
		else if (ercd != FSDCF_ERRNO_OK) {
			*count = 0;
			ercd = FJ_ERR_NG;
			break;
		}
		else {
			; // DO NOTHING
		}
		*count += 1;
	}
	return (FJ_ERR_CODE)ercd;
}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

/**
This function gets file/directory information.
@param[in] szName The name of the file/directory
@param[out] pStat Information on file/directory. See @ref FJ_FS_STAT
@retval FJ_ERR_OK(0) Normal end.
@retval FJ_ERR_NG(1) Error occurred.
*/
FJ_ERR_CODE fj_fs_stat(CHAR* szName, FJ_FS_STAT* pStat)
{
	INT32 ercd;
	T_BF_FS_IF_STAT file_stat;

	memset((FW_VOID*)&file_stat, 0, sizeof(T_BF_FS_IF_STAT));

	ercd = BF_Fs_If_Stat((const FW_CHAR*)szName, &file_stat);
	if (ercd != FSDCF_ERRNO_OK) {
		// error of file status acquisition
//		BF_Debug_Print_Error(("BF_Fs_If_Stat() err(%d) \n", ercd));
		return FJ_ERR_NG;
	}

	if (file_stat.fileSize > 0xFFFFFFFF){
		// Error. Because the size of the file is larger than that of 4GByte.
		BF_Debug_Print_Error(("file size over \r\n"));
		return FJ_ERR_NG;
	}

	pStat->create_date = file_stat.m_date;
	pStat->create_time = file_stat.m_time;
	pStat->modify_date = file_stat.date;
	pStat->modify_time = file_stat.time;
	pStat->size = (ULONG)file_stat.fileSize;

	return FJ_ERR_OK;
}

/**
This function gets file/directory information.
@param[in] szName The name of the file/directory
@param[out] pStatEx Information on file/directory. See @ref FJ_FS_STAT_EX
@retval FJ_ERR_OK(0) Normal end.
@retval FJ_ERR_NG(1) Error occurred.
*/
FJ_ERR_CODE fj_fs_statex(CHAR* szName, FJ_FS_STAT_EX* pStatEx)
{
	INT32 ercd;
	T_BF_FS_IF_STAT file_stat;
	ULONG create_sec;
	ULONG modify_sec;

	memset((FW_VOID*)&file_stat, 0, sizeof(T_BF_FS_IF_STAT));

	ercd = BF_Fs_If_Stat((const FW_CHAR*)szName, &file_stat);
	if (ercd != FSDCF_ERRNO_OK) {
		// error of file status acquisition
//		BF_Debug_Print_Error(("BF_Fs_If_Stat() err(%d) \n", ercd));
		return FJ_ERR_NG;
	}

	create_sec = ((file_stat.m_time & 0x001F) * 2) + (file_stat.m_sec10m / 100);
	modify_sec = ((file_stat.time & 0x001F) * 2) + (file_stat.sec10m / 100);

	pStatEx->create_date = file_stat.m_date;
	pStatEx->create_time = ((file_stat.m_time & 0xFFE0) << 1) | create_sec;
	pStatEx->modify_date = file_stat.date;
	pStatEx->modify_time = ((file_stat.time & 0xFFE0) << 1) | modify_sec;
	pStatEx->size_ms = (ULONG)(file_stat.fileSize >> 32);
	pStatEx->size_ls = (ULONG)file_stat.fileSize;

	return FJ_ERR_OK;
}

/**
This function changes the date and time of demanded file or directory.<br>
@param[in] szName The name of the file or directory
@param[in] timestamp_type The kind of the TimeStamp that changes is set.<br>
						The parameter that can be set is three of the following.<br>
						@ref FJ_FS_TIMESTAMP_MODIFY, @ref FJ_FS_TIMESTAMP_CREATE, @ref FJ_FS_TIMESTAMP_ACCESS
@param[in] ptime Information of date and time<br>
@retval FJ_ERR_OK(0) Normal end.
@retval FJ_ERR_NG(1) Error occurred.
*/
FJ_ERR_CODE fj_fs_utimeex(CHAR* szName, USHORT timestamp_type, FJ_FS_TIME* ptime)
{
	INT32 ercd;
	T_BF_FS_IF_TIME update_time;

	update_time.date = ptime->date;
	update_time.time = (FW_USHORT)(ptime->time >> 1);

	if((ptime->time & 0x01) == 1){
		// set 1000 ms
		update_time.sec10m = 100;
	}
	else{
		// set 0 ms
		update_time.sec10m = 0;
	}

	ercd = BF_Fs_If_Set_Time((FW_CHAR*)szName, timestamp_type, &update_time);
	if (ercd != FSDCF_ERRNO_OK) {
		// error of file status acquisition
		BF_Debug_Print_Error(("BF_Fs_If_Set_Time() err(%d) \n", ercd));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief		seek the file.
 * @param		ULONG	ulFileId
 *				ULONG	ulOffset
 *				USHORT	whence
 *				ULONG*	pulLength
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_lseek(ULONG ulFileId, ULONG ulOffset, USHORT whence, ULONG* pulLength)
{
	INT32 ret;
	FW_USHORT origin;

	if (whence == FJ_FS_SEEK_SET) {
		origin = D_BF_FS_IF_SEEK_SET;
	}
	else if (whence == FJ_FS_SEEK_CUR) {
		origin = D_BF_FS_IF_SEEK_CUR;
	}
	else if (whence == FJ_FS_SEEK_END) {
		origin = D_BF_FS_IF_SEEK_END;
	}
	else {
		printf("lseek() Param chk NG  whence=%d. \n",whence);
		return FJ_ERR_NG;
	}

	ret = BF_Fs_If_Lseek(ulFileId, ulOffset, origin, pulLength);
	if (ret != 0) {
		return FJ_ERR_NG;
	}

//#ifdef FJ_DEBUG_PRINT_ON
//	BF_Debug_Print_Information(("Offset is 0x%llx , length is 0x%llx \r\n", ulOffset, *pulLength));
//#endif

	return FJ_ERR_OK;
}

/**
 * @brief		seek the file.
 * @param		ULONG	ulFileId
 *				INT64	ulOffset
 *				USHORT	whence
 *				INT64*	pulLength
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_lseek64(ULONG ulFileId, INT64 ulOffset, USHORT whence, INT64* pulLength)
{
	INT32 ret;
	FW_USHORT origin;

	if (whence == FJ_FS_SEEK_SET) {
		origin = D_BF_FS_IF_SEEK_SET;
	}
	else if (whence == FJ_FS_SEEK_CUR) {
		origin = D_BF_FS_IF_SEEK_CUR;
	}
	else if (whence == FJ_FS_SEEK_END) {
		origin = D_BF_FS_IF_SEEK_END;
	}
	else {
		printf("lseek64() Param chk NG  whence=%d. \n",whence);
		return FJ_ERR_NG;
	}

	ret = BF_Fs_If_Lseek64(ulFileId, ulOffset, origin, pulLength);
	if (ret != 0) {
		return FJ_ERR_NG;
	}

//#ifdef FJ_DEBUG_PRINT_ON
//	BF_Debug_Print_Information(("Offset is 0x%llx , length is 0x%llx \r\n", ulOffset, *pulLength));
//#endif

	return FJ_ERR_OK;
}

/**
 * @brief		Flush the drive
 * @param		ULONG	ulFileId
 *				ULONG	ulOffset
 *				USHORT	whence
 *				ULONG*	pulLength
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_fsync(ULONG ulFileId)
{
	FW_INT32 ercd;

	ercd = BF_Fs_If_Fsync((FW_INT32)ulFileId);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief		Sync the drive
 * @param		ULONG	ulFileId
 *				ULONG	ulOffset
 *				USHORT	whence
 *				ULONG*	pulLength
 * @return		FJ_ERR_OK 
 *				FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_fs_sync(CHAR* szDrvName)
{
	FW_INT32 ercd;

	ercd = BF_Fs_If_Sync((const FW_CHAR*)szDrvName);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief	This function gets a user setting data from user setting area on SDRAM.
 * @param	pbyBuffer		The buffer address for read
 * @param	ulRequestSize	The requested number of bytes to read
 * @param	ulOffset		The offset into user setting area.
 * @return	FJ_ERR_OK 		Normal end.
 * @return	FJ_ERR_NG 		Error occurred.
*/ 
FJ_ERR_CODE fj_fs_getusersettingdata(UINT32 *pbyBuffer, ULONG ulRequestSize, ULONG ulOffset)
{
	UINT32 src_addr, dist_addr, tmp_addr;
	UINT32 copy_size = 0;
	ULONG  data_length;
	UINT32 data_offset = D_FS_USER_SET_OFFSET;	
	
	/* Check request address */
	if( pbyBuffer == NULL ) {
		BF_Debug_Print_Error(("%s(): Request address is zero.\n", __func__ ));
		return FJ_ERR_NG;
	}
	
	/* Check request size */
	if( ulRequestSize > ( D_FS_USER_SET_MAX_SIZE )) {
		BF_Debug_Print_Error(("%s(): Request sise is over.\n", __func__ ));
		return FJ_ERR_NG;
	}
	
	/* Set src addr and size */
	src_addr = SDRAM_ADR_USER_SET_DATA_MAIN;
	copy_size = ulRequestSize;
	
	/* Check data length */
	data_length = *(( UINT32* )src_addr );
	
	if( data_length == 0 ){
		/* User data is nothing. Not copy. */
		BF_Debug_Print_Warning(("fj_fs_getusersettingdata : copy size is zero.\r\n"));
		return FJ_ERR_NG;
	}
	
	/* Set dist addr. */
	tmp_addr = ( UINT32 )pbyBuffer;
	dist_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( tmp_addr );
	
	printf("data_offset = %d, ulOffset = %lu\n", data_offset, ulOffset );	// for debug
	printf("src_addr = 0x%x, dist_addr = 0x%x, copy_size = 0x%x\n", src_addr, dist_addr, copy_size );	// for debug
	
	/* Copy from Main data area to Work area */
	memcpy( ( FW_VOID* )dist_addr, ( const FW_VOID* )( src_addr + data_offset + ulOffset), copy_size );

	return FJ_ERR_OK;
}

/**
 * @brief	This function sets a user setting data to user setting area on nand flash.
 * @param	pbyBuffer The buffer address for a user setting data to write.
 * @param	ulRequestSize The size for a user setting data to write.
 * @return	FJ_ERR_OK Normal end.
 * @return	FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE fj_fs_setusersettingdata(UINT32 *pbyBuffer, ULONG ulRequestSize)
{
	INT32 ercd;
	UINT32 src_addr, dist_addr, tmp_addr;
	UINT32 data_offset = D_FS_USER_SET_OFFSET;
	UINT32 data_max = D_FS_USER_SET_MAX_SIZE;
	
	/* Check request address */
	if( pbyBuffer == NULL ) {
		BF_Debug_Print_Error(("%s(): Request address is zero.\n", __func__ ));
		return FJ_ERR_NG;
	}
	
	/* Check request size */
	if( ulRequestSize > ( D_FS_USER_SET_MAX_SIZE )) {
		BF_Debug_Print_Error(("%s(): Request sise is over.\n", __func__ ));
		return FJ_ERR_NG;
	}
	
	/* ### Update User Setting data to NF ### */
	/* Set src addr. */
	tmp_addr = ( UINT32 )pbyBuffer;
	src_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( tmp_addr );
	
	/* Set User Setting data */
	ercd = BF_Fs_If_Set_User_Setting( ( VOID* )src_addr, ulRequestSize );
	
	if(ercd < 0){
		return FJ_ERR_NG;
	}
	
	/* ### Update User Setting data to main area on SDRAM ### */
	/* Set dist addr. */
	dist_addr = SDRAM_ADR_USER_SET_DATA_MAIN + data_offset;
	
	/* Copy from Work area to Main data area */
	memcpy( ( FW_VOID* )dist_addr, ( const FW_VOID* )src_addr, data_max );
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Media_Mount(FJ_MEDIA_ID bMediaId)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	INT32	ret;
//	CHAR	drv_name[8];

	Dd_ARM_Critical_Section_Start( critseq );
	g_bf_media_mount_end = 0;
	Dd_ARM_Critical_Section_End( critseq );

#ifdef CO_DCF_EXIF_ON
	BF_Dcf_If_End_Task_Mdf();
#endif // CO_DCF_EXIF_ON

	ret = BF_Fs_If_Start_Used_Media_As(media_mount_callback, (UINT32)bMediaId);
	if (ret != 0 ) {
		return FJ_ERR_NG;
	}

	if (BF_Media_Attr_Get_Attr_Sync() == 0) {
		BF_Fs_If_Wait_Fs_Init_End();
		while (g_bf_media_mount_end != 1) {
			BF_Fs_User_Custom_Dly_Tsk(10);
		}
	}

	if (g_bf_media_mount_ret != FSDCF_ERRNO_OK) {
		// false
		return (FJ_ERR_CODE)g_bf_media_mount_ret;
	}
	BF_Filesystem_File_Reset_Directory_Number();

//	strcpy(drv_name, (const CHAR*)gBF_Controller_drv_name[bMediaId]);
//	strcat(drv_name, ":");
//
//	ret = BF_Fs_If_Chdir( drv_name );
//
//	if (ret != FSDCF_ERRNO_OK) {
//		return FJ_ERR_NG;
//	}


#if 0
	{
		// for debug
#define SAMPLE_STRING "This file is created when MS mounted on boot\n"
		FW_INT32 fid;
		FS_DWORD result;
		char* read_buf[64] = {0};
		static unsigned char started = 0;

		if (!started) {
			FS_remove("boot.txt");
			fid = FS_create("boot.txt", ATTR_NORMAL);
			FS_write(fid, (void*)SAMPLE_STRING, strlen(SAMPLE_STRING), &result);
			FS_close(fid);
			fid = FS_open("boot.txt", O_RDONLY, ATTR_NORMAL);
			FS_read(fid, (void*)read_buf, 64, &result);
			FS_close(fid);

			started = 1;
		}
	}
#endif

	return FJ_ERR_OK;
}

/**
 * @brief 		mount a new storage media.
 * @param		UCHAR	bMediaId
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_mount(FJ_MEDIA_ID bMediaId, CHAR *szDrvName)
{
	INT32 ercd;
	UINT32	drvname_len;
//	USHORT dirno, fileno;

	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_MEDIA, FJ_MBALOG_TYPE_START, "Mount" ));

	if(*gBF_Controller_drv_name[bMediaId]) {
		return FJ_ERR_NG;
	}

	gBF_Controller_media_id = (CHAR)bMediaId;

	drvname_len = strlen(szDrvName);
	if (drvname_len > (D_BF_FS_IF_DRVNAME_MAX - 1)) {
		return FJ_ERR_NG;
	}

	strcpy((CHAR*)gBF_Controller_drv_name[bMediaId], szDrvName);

	if(BF_Boot_Loader_Check(D_BF_BOOT_LOADER_ALL_CODE) == 1) {
#ifdef CO_DCF_EXIF_ON
		BF_Exif_Reset_Index();
#endif // CO_DCF_EXIF_ON
	}
	
//#ifdef CO_MEASURE_BOOTUP_TIME
//	// LED13 ON P27
//	*((volatile unsigned int*)0x1DFFB104) |= 0x80;			// DDR32
//	*((volatile unsigned int*)0x1DFFB204) &= 0x80;			// EPCR32
//	*((volatile unsigned int*)0x1DFFB004) = 0x00800080;		// PDR32
//#endif
	
	ercd = BF_Media_Mount(bMediaId);
	
//#ifdef CO_MEASURE_BOOTUP_TIME
//	// LED13 OFF P27
//	*((volatile unsigned int*)0x1DFFB104) |= 0x80;			// DDR32
//	*((volatile unsigned int*)0x1DFFB204) &= 0x80;			// EPCR32
//	*((volatile unsigned int*)0x1DFFB004) = 0x00800000;		// PDR32
//#endif

	if (ercd != FJ_ERR_OK) {
		if (ercd == D_BF_MDF_WRAPPER_ERR_FSNOADJ) {
			BF_Mode_Set_Sr1( FJ_SR1_FAILURE );
			memset((CHAR*)gBF_Controller_drv_name[bMediaId], '\0', sizeof(gBF_Controller_drv_name[bMediaId]));
			memset((CHAR*)gBF_Fs_Controller_Media_Status[bMediaId].devid, '\0', sizeof(gBF_Fs_Controller_Media_Status[bMediaId].devid));
			memset((CHAR*)gBF_Fs_Controller_Media_Status[bMediaId].drvname, '\0', sizeof(gBF_Fs_Controller_Media_Status[bMediaId].drvname));
			return FJ_ERR_NOT_FAT;
		}
		else {
			BF_Mode_Set_Sr1( FJ_SR1_FAILURE );
			memset((CHAR*)gBF_Controller_drv_name[bMediaId], '\0', sizeof(gBF_Controller_drv_name[bMediaId]));
			memset((CHAR*)gBF_Fs_Controller_Media_Status[bMediaId].devid, '\0', sizeof(gBF_Fs_Controller_Media_Status[bMediaId].devid));
			memset((CHAR*)gBF_Fs_Controller_Media_Status[bMediaId].drvname, '\0', sizeof(gBF_Fs_Controller_Media_Status[bMediaId].drvname));
			return FJ_ERR_NG;
		}
	}

	BF_Mode_Set_Sr1( FJ_SR1_SUCCESS );

#if 0
	if (g_bf_media_dynamic_scenario_load == 0 ){
		BF_Dynamic_Scenario();
		g_bf_media_dynamic_scenario_load = 1;
	}
#endif
	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_MEDIA, FJ_MBALOG_TYPE_END, "" ));

#ifdef FJ_DEBUG_PRINT_ON
	BF_Debug_Print_Information(("Mount OK.  media_id:%d  drive_name:%s\n", bMediaId, szDrvName));
#endif
	return FJ_ERR_OK;			// success
}

/**
 * @brief 		unmount a storage media.
 * @param		CHAR* szDrvName
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_unmount(CHAR* szDrvName)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	FJ_MEDIA_ID media_id;

	BF_Fs_Controller_Get_Media_Id(szDrvName, (CHAR*)&media_id);
	if ((UCHAR)media_id == 0xFF) {
		return FJ_ERR_NG;
	}
	Dd_ARM_Critical_Section_Start( critseq );
	g_bf_media_mount_end = 0;
	Dd_ARM_Critical_Section_End( critseq );

#ifdef CO_DCF_EXIF_ON
	BF_Dcf_If_End_Task_Mdf();
#endif // CO_DCF_EXIF_ON
	BF_Fs_If_End_Used_Media_As(media_mount_callback, (UINT32)media_id);

	if (BF_Media_Attr_Get_Attr_Sync() == 0) {
		while (g_bf_media_mount_end != 1) {
			BF_Fs_User_Custom_Dly_Tsk(10);
		}
	}
	if ((g_bf_media_mount_ret != FSDCF_ERRNO_OK) && 
		(g_bf_media_mount_ret != D_BF_MDF_WRAPPER_ERR_NXIO) && 
		(g_bf_media_mount_ret > D_BF_MDF_WRAPPER_ERR_FAT_READ)) {
		return FJ_ERR_NG;
	}
	memset((CHAR*)gBF_Controller_drv_name[media_id], '\0', sizeof(gBF_Controller_drv_name[media_id]));
	memset((CHAR*)gBF_Fs_Controller_Media_Status[media_id].devid, '\0', sizeof(gBF_Fs_Controller_Media_Status[media_id].devid));
	memset((CHAR*)gBF_Fs_Controller_Media_Status[media_id].drvname, '\0', sizeof(gBF_Fs_Controller_Media_Status[media_id].drvname));

#ifdef FJ_DEBUG_PRINT_ON
	BF_Debug_Print_Information(("UnMount OK.  media_id:%d  drive_name:%s\n", media_id, szDrvName));
#endif
	return FJ_ERR_OK;
}

/**
 * @brief 		reads the storage media information.
 * @param		USHOT*		pwSuccess
 *				USHORT*		pwImageCount
 *				USHORT*		pwImageLeft
 *				USHORT*		pwSizeMS
 *				USHORT*		pwSizeLS
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getstoragemediastatusex(USHORT* pwSuccess, UINT32* pwImageCount, UINT32* pwImageLeft, UINT32* pwSizeMS, UINT32* pwSizeLS)
{
#ifdef CO_DCF_EXIF_ON
	INT32			ret, dcf_number = 0;
	FW_DOUBLE		free_size;
	UINT64			free_size_long;
	// delete basefw\fj\imgproc\still
	//ULONG			main_size, thum_size, vga_size,hd_size;
	//USHORT 			threshold;
	E_BF_MEDIA_STATE_MEDIA_TYPE	media_type;
	ULONG			margin_size;

	// delete basefw\fj\imgproc\still
	//ULONG	raw_bytes;
#if 0	// MILB_PRO_PORTING
	T_IMAGE_SIZE	raw_pixs;
	ULONG			linefeed_bytes;
#endif	// MILB_PRO_PORTING
	// delete basefw\fj\imgproc\still
	//E_SHOOT_FORMAT format;

	media_type = BF_Media_State_Get_Used_Media();
	switch (media_type) {
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD: // FALL THROUGH
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD1: // FALL THROUGH
		case E_BF_MEDIA_STATE_MEDIA_TYPE_NF:
			break;

		case E_BF_MEDIA_STATE_MEDIA_TYPE_NOTHING:
			*pwSuccess = 1;
			return FJ_ERR_NOT_MOUNT;

		default:
#ifdef FJ_DEBUG_PRINT_ON
			BF_Debug_Print_Error(("current media type is abnormal\r\n"));
#endif
			*pwSuccess = 1;
			return FJ_ERR_NG;
	}

	if (BF_Fs_If_Check_Media_State(media_type) != 1) {
		*pwSuccess = 1;
		return FJ_ERR_NG;
	}

	dcf_number = BF_Dcf_If_Get_Entry(media_type);
	if (dcf_number < 0) {
		*pwSuccess = 0;		//error
		return FJ_ERR_NG;
	}
	else {
		*pwImageCount = dcf_number;
	}

	ret = BF_Fs_If_Get_Free_Size(media_type, &free_size);
	if (ret == 0) {
		free_size_long = (UINT64)free_size;
//		dpof_size = BF_Media_Attr_Get_Attr_Dpof_Size();
//		free_size -= dpof_size;

		margin_size = 0;


		if(free_size_long < margin_size){
			free_size_long = 0;
		}
		else{
			free_size_long -= margin_size;
		}

		*pwSizeLS = (UINT32)free_size_long;
		*pwSizeMS = (UINT32)(( free_size_long >> 16 )  >> 16 );

#if 0
		main_size = FJ_SizeCalculate(E_YCC_FORMAT_TYPE_YCC422, BF_Still_Attr_Get_Size_Cap_Width(), BF_Still_Attr_Get_Size_Cap_Lines(), BF_Still_Attr_Get_Comp_Main());
		thum_size = FJ_SizeCalculate(E_YCC_FORMAT_TYPE_YCC422, WIDTH_THUMB, LINES_THUMB, BF_Still_Attr_Get_Comp_Main());
#else
		// delete basefw\fj\imgproc\still
		//main_size = 0;
		//thum_size = 0;
#endif

		// delete basefw\fj\imgproc\still
		//if (BF_Still_Attr_Get_Screen_Vga_Enable() == 1) {
		//	vga_size = (BF_Still_Attr_Get_Size_Screen_Vga_Width() * BF_Still_Attr_Get_Size_Screen_Vga_Lines() * 2) / BF_Still_Attr_Get_Comp_Screen_Vga();
		//}
		//else{
		//	vga_size = 0;
		//}

		//if(BF_Still_Attr_Get_Screen_Hd_Enable() == 1){
		//	hd_size = (BF_Still_Attr_Get_Size_Screen_Hd_Width() * BF_Still_Attr_Get_Size_Screen_Hd_Lines() *2 )/ BF_Still_Attr_Get_Comp_Screen_Hd();
		//}
		//else{
		//	hd_size = 0;
		//}
		
		//threshold = BF_Still_Attr_Get_Jpeg_Threshold();
		//if ((threshold > 0) && (threshold <= 100)) {
		//	main_size += ((main_size * (ULONG)threshold) / 100);
		//}

#if 0	// MILB_PRO_PORTING
		BF_Still_PRO_Ctrl_Get_Sro_Bayer_Size( 1, &raw_pixs );
		BF_Still_PRO_Ctrl_Get_SroBayerLinefeed(1,  &linefeed_bytes );
		raw_bytes =  linefeed_bytes * raw_pixs.lines;
#else
		// delete\basefw\fj\imgproc\still
		//BF_PRO_RAW_INFO			raw_info;
		//BF_PRO_RAW_OUTPUT_INFO	raw_output_info = {0};
		//raw_output_info.inst_id		= 0;
		//raw_output_info.unit_num	= BF_STILL_PRO_UNIT_NUM_1;
		//raw_output_info.block_type	= BF_STILL_PRO_BLOCK_TYPE_SEN;
		//raw_output_info.output_port	= 0;
		//BF_Still_PRO_Get_Raw_Info( &raw_output_info, &raw_info );
		//raw_bytes = raw_info.raw_image_bytes;
		//raw_bytes = 0;
#endif	// MILB_PRO_PORTING

#ifdef FJ_DEBUG_PRINT_ON
//		BF_Debug_Print_Information(("Media Free Size     :[%12lld] bytes  \r\n", free_size_long));
//		BF_Debug_Print_Information(("Main Picture Size   :[%12ld] bytes  \r\n",  main_size));
//		BF_Debug_Print_Information(("Thum Picture Size   :[%12ld] bytes  \r\n",  thum_size));
//		BF_Debug_Print_Information(("VGA Picture Size    :[%12ld] bytes  \r\n",  vga_size));
//		BF_Debug_Print_Information(("HD Picture Size     :[%12ld] bytes  \r\n",  hd_size));
//		BF_Debug_Print_Information(("RAW Data Size       :[%12ld] bytes  \r\n",  raw_bytes));
#endif

		// delete basefw\fj\imgproc\still
		//format = BF_Still_Attr_Get_Shoot_Image_Format();
		//switch( format ){
		//	case E_SHOOT_FORMAT_JPEG:	//JPEG
		//			*pwImageLeft = (UINT32)(free_size_long / (main_size + thum_size + vga_size + hd_size + BF_Exif_Attr_Get_Attr_Maker_Note_Size() + sizeof(T_BF_DCF_EXIF_JPEG_HEADER_FIRST) + sizeof(T_BF_DCF_EXIF_JPEG_HEADER_SECOND)));
		//		break;
		//	case E_SHOOT_FORMAT_RAW:	//RAW
		//			*pwImageLeft = (UINT32)(free_size_long / raw_bytes );
		//		break;
		//	case E_SHOOT_FORMAT_JPEG_RAW:	//JPEG_RAW
		//			*pwImageLeft = (UINT32)(free_size_long / (main_size + thum_size + vga_size + hd_size + BF_Exif_Attr_Get_Attr_Maker_Note_Size() + sizeof(T_BF_DCF_EXIF_JPEG_HEADER_FIRST) + sizeof(T_BF_DCF_EXIF_JPEG_HEADER_SECOND) + raw_bytes));
		//		break;

		//	default:
		//		break;

		//}

#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Information(("Available free space is %lld bytes  \r\n", (UINT64)free_size));
#endif
	}
	else {
		*pwSuccess = 1;
		return FJ_ERR_NG;
	}

	*pwSuccess = 0;
#endif // CO_DCF_EXIF_ON
	return FJ_ERR_OK;
}

/**
 * @brief 		Get Media Information.
 * @param		FJ_MEDIA_ID bMediaId
 *				FJ_FS_GEOMETRY* pGeometry
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getstoragemediageometryex(FJ_MEDIA_ID bMediaId, FJ_FS_GEOMETRY* pGeometry)
{
	INT32 ret;
	T_BF_FS_IF_STATVFS media_info;
	E_BF_MEDIA_STATE_MEDIA_TYPE	media_type;
	CHAR drv_name[9] = {'0'};
	CHAR cahr_length;
	FW_UCHAR format_type;

	media_type = BF_Media_State_Get_Used_Media();
	if(media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_NOTHING){
		// media is not mount
		return FJ_ERR_NOT_MOUNT;
	}

	if (bMediaId < FJ_MEDIA_SD0) {
		pGeometry->media = FJ_USED_MEDIA_NF;	// NF
	}
	else if (bMediaId == FJ_MEDIA_SD0) {
		BF_Media_Drv_Wrapper_Get_Media_Format_Type_SDIO0(&media_type, &format_type);
	}
	else if (bMediaId == FJ_MEDIA_SD1) {
		BF_Media_Drv_Wrapper_Get_Media_Format_Type_SDIO1(&media_type, &format_type);
	}
	else if (bMediaId > FJ_MEDIA_SD1) {
		pGeometry->media = FJ_USED_MEDIA_EMMC;	// EMMC
		format_type = 0;
	}
	else {
		pGeometry->media = 0;
		format_type = 0;
	}
	cahr_length = strlen((const FW_CHAR*)gBF_Controller_drv_name[bMediaId]);
	strncpy(drv_name, (const CHAR*)gBF_Controller_drv_name[bMediaId], cahr_length);

	ret = BF_Fs_If_Statvfs(drv_name, &media_info);
	if (ret != D_BF_FS_USER_CUSTOM_OK) {
		// Media type //
		pGeometry->media = media_type;

		// Byte/1Sector(512Byte)
		pGeometry->BytePerSector = 0;

		// Number of Sector/1Cluster
		pGeometry->SectorPerCluster = 0;

		// Number of TotalCluster
		pGeometry->TotalClusters = 0;

		// Number of FreeCluster
		pGeometry->FreeClusters = 0;

		// FAT type
		pGeometry->FatType = FJ_FS_TYPE_UNKNOWN;

#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Information((" Get Media Information Access Error\n"));
#endif
		return FJ_ERR_NG;
	}
	// Media type //
	pGeometry->media = media_type;
	
	// Byte/1Sector(512Byte)
	pGeometry->BytePerSector = 512;

	// Number of Sector/1Cluster
	pGeometry->SectorPerCluster = (USHORT)(media_info.blksize / 512);

	// Number of TotalCluster
	pGeometry->TotalClusters = (UINT32)media_info.blocks;

	// Number of FreeCluster
	pGeometry->FreeClusters = (UINT32)media_info.bfree;

	// FAT type
	pGeometry->FatType = (UCHAR)media_info.fstype;

	return FJ_ERR_OK;
}

/**
 * @brief 		Check MBR of the internal memory(NandFlash).
 * @param		none.
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_checkmbrinternalmemory(VOID)
{
	FW_INT32 ercd;

	ercd = BF_Fs_If_Check_Mbr();
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief 		Make MBR of the internal memory(NandFlash).
 * @param		none.
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_makembrinternalmemory(VOID)
{
	FW_INT32 ercd;

	ercd = BF_Fs_If_Make_Mbr();
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief 		Get used media and format(filesystem) type.
 * @param		FJ_USED_MEDIA* media : used media type.
 *											FJ_USED_MEDIA_UNKNOWN(0x00) : unknown media
 *											FJ_USED_MEDIA_NF     (0x01) : Nand Flash
 *				FJ_FORMAT_TYPE* format_type : format(filesystem) type of the used media.
 *											FJ_FS_TYPE_UNKNOWN(0x00) : unknown filesystem
 *											FJ_FS_TYPE_FAT12  (0x01) : FAT12 format
 *											FJ_FS_TYPE_FAT16  (0x04) : FAT16 format (<= 32MB)
 *											FJ_FS_TYPE_FAT16E (0x06) : FAT16 format (>  32MB)
 *											FJ_FS_TYPE_EXFAT  (0x07) : exFAT format
 *											FJ_FS_TYPE_FAT32  (0x0B) : FAT32 format
 *											FJ_FS_TYPE_FAT32X (0x0C) : FAT32 (LBA) format
 *											FJ_FS_TYPE_FAT16X (0x0E) : FAT16 (LBA) format
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_get_media_format_type(UCHAR* media, UCHAR* format_type)
{
	FW_INT32 ercd;
	E_BF_MEDIA_STATE_MEDIA_TYPE media_type;

	media_type = BF_Media_State_Get_Used_Media();
	if(media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_NOTHING){
		// media is not mount
		return FJ_ERR_NOT_MOUNT;
	}

	ercd = BF_Fs_If_Get_Media_Format_Type((FW_UCHAR*)media, (FW_UCHAR*)format_type);
	if (ercd != 0) {
		*media = FJ_USED_MEDIA_UNKNOWN;
		*format_type = FJ_FS_TYPE_UNKNOWN;
		return FJ_ERR_NG;
	}

#if 1
	if ((*format_type == FJ_FS_TYPE_FAT12)  ||
		(*format_type == FJ_FS_TYPE_FAT16)  ||
		(*format_type == FJ_FS_TYPE_FAT16E) ||
		(*format_type == FJ_FS_TYPE_EXFAT)  ||
		(*format_type == FJ_FS_TYPE_FAT32)  ||
		(*format_type == FJ_FS_TYPE_FAT32X)) {
	}
	else {
		*format_type = FJ_FS_TYPE_UNKNOWN;
	}
#endif

	return FJ_ERR_OK;
}

/**
 * @brief 		reads the storage media information.
 * @param		USHOT*		pwSuccess
 *				USHORT*		pwImageCount
 *				USHORT*		pwImageLeft
 *				USHORT*		pwSizeMS
 *				USHORT*		pwSizeLS
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getstoragemediastatus(USHORT* pwSuccess, USHORT* pwImageCount, USHORT* pwImageLeft, USHORT* pwSizeMS, USHORT* pwSizeLS)
{
#ifdef CO_DCF_EXIF_ON
	INT32 ret, dcf_number = 0;
	UINT32 free_size_int;
	FW_DOUBLE free_size;
	volatile FW_DOUBLE tmp_free_size;
	ULONG main_size, thum_size, vga_size;
	E_BF_MEDIA_STATE_MEDIA_TYPE	media_type;


	media_type = BF_Media_State_Get_Used_Media();
	switch (media_type) {
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD: // FALL THROUGH
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD1: // FALL THROUGH
		case E_BF_MEDIA_STATE_MEDIA_TYPE_NF:
			break;

		case E_BF_MEDIA_STATE_MEDIA_TYPE_NOTHING:
			*pwSuccess = 1;
			return FJ_ERR_NOT_MOUNT;

		default:
#ifdef FJ_DEBUG_PRINT_ON
			BF_Debug_Print_Error(("current media type is abnormal\r\n"));
#endif
			*pwSuccess = 1;
			return FJ_ERR_NG;
	}

	if (BF_Fs_If_Check_Media_State(media_type) != 1) {
		*pwSuccess = 1;
		return FJ_ERR_NG;
	}

	dcf_number = BF_Dcf_If_Get_Entry(media_type);

	if (dcf_number < 0) {
		*pwSuccess = 1;		//error
		return FJ_ERR_NG;
	}
	else {
		*pwImageCount = (USHORT)dcf_number;
	}

	ret = BF_Fs_If_Get_Free_Size(media_type, &free_size);

	tmp_free_size = (free_size - 0xFFFFFFFF);
//	if ((free_size - 0xFFFFFFFF) > 0) {
	if (tmp_free_size > 0) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(("size over \r\n"));
#endif
		*pwSuccess = 1;
		return FJ_ERR_NG;
	}

	if (ret == 0) {
		// 4GB > free_size
		free_size_int = (UINT32)((UINT64)free_size & 0xFFFFFFFF);

		*pwSizeMS = (USHORT)((free_size_int & 0xFFFF0000) >> 16);
		*pwSizeLS = (USHORT)(free_size_int & 0x0000FFFF);

#if 0
		main_size = FJ_SizeCalculate(E_YCC_FORMAT_TYPE_YCC422, BF_Still_Attr_Get_Size_Cap_Width(), BF_Still_Attr_Get_Size_Cap_Lines(), BF_Still_Attr_Get_Comp_Main());
		thum_size = FJ_SizeCalculate(E_YCC_FORMAT_TYPE_YCC422, WIDTH_THUMB, LINES_THUMB, BF_Still_Attr_Get_Comp_Main());
#else
		main_size = 0;
		thum_size = 0;
#endif
		
		// delete basefw\fj\imgproc\still
		//if (BF_Still_Attr_Get_Screen_Vga_Enable() == 1) {
		//	vga_size = (BF_Still_Attr_Get_Size_Screen_Vga_Width() * BF_Still_Attr_Get_Size_Screen_Vga_Lines() * 2) / BF_Still_Attr_Get_Comp_Screen_Vga();
		//}
		//else{
			vga_size = 0;
		//}

		*pwImageLeft = free_size_int / (main_size + thum_size + vga_size + BF_Exif_Attr_Get_Attr_Maker_Note_Size() + sizeof(T_BF_DCF_EXIF_JPEG_HEADER_FIRST) + sizeof(T_BF_DCF_EXIF_JPEG_HEADER_SECOND));

#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Information(("Available free space is %lld bytes.  \r\n", (UINT64)free_size));
#endif
	}
	else {
		*pwSuccess = 1;
		return FJ_ERR_NG;
	}

	*pwSuccess = 0;
//	fj_hostpostmessage( FJ_HM_GET_MEDIA_STATUS_COMPLETED, 0);
#endif // CO_DCF_EXIF_ON

	return FJ_ERR_OK;
}

/**
 * @brief 		Copy Drive to Deive.
 * @param		CHAR*	szSrcDrive : copy source drive name
 *				CHAR*	szDstDrive : copy destination drive name
 * @return 		FJ_ERR_CODE : FJ_ERR_OK Copy successful
 *                          : FJ_ERR_NG Copy failed
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_drivetodrivecopy(CHAR* szSrcDrive, CHAR* szDstDrive)
{
	FW_INT32 ercd;

	if ((szSrcDrive == NULL) || (szDstDrive == NULL)) {
		/* NULL pointer */
		return FJ_ERR_NG;
	}
	if ((*szSrcDrive == '\0') || (*szDstDrive == '\0')) {
		/* drive name null */
		return FJ_ERR_NG;
	}

	ercd = BF_Fs_If_Drive_To_Drive_Copy((FW_CHAR*)szSrcDrive, (FW_CHAR*)szDstDrive);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

#if 1	// for DCF-task
//*********************
// callback of Format
//*********************
VOID media_format_callback(INT32 ret)
{
	FW_CHAR drv_name[9];

	if (ret == FSDCF_ERRNO_OK) {
//		BF_Fs_If_Chdir("I:");
		
		if(BF_Fs_If_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD) == E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
			strcpy(drv_name, (const CHAR*)gBF_Controller_drv_name[FJ_MEDIA_SD0]);
			strcat(drv_name, ":");
			BF_Fs_If_Chdir((const FW_CHAR*)drv_name);
		}
		else if(BF_Fs_If_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1) == E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
			strcpy(drv_name, (const CHAR*)gBF_Controller_drv_name[FJ_MEDIA_SD1]);
			strcat(drv_name, ":");
			BF_Fs_If_Chdir((const FW_CHAR*)drv_name);
		}
	}
	else {
		; // DO NOTHING
	}
}
#endif

/**
 * @brief 		format a storage media (QuickFormat).
 * @param		UCHAR	bMediaId
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_formatstoragemedia(FJ_MEDIA_ID bMediaId)
{
	INT32 ret;
	CHAR  drv_name[9];

	if(*gBF_Controller_drv_name[bMediaId] == '\0') {
		return FJ_ERR_NG;
	}

	BF_Filesystem_File_Reset_Directory();

#ifdef CO_DCF_EXIF_ON
	if ((bMediaId == FJ_MEDIA_SD0) || (bMediaId == FJ_MEDIA_SD1)) {
		BF_Exif_Reset_Index();
		BF_Dcf_If_Set_Series_File_No(NULL, 0, 0);
		BF_Dcf_If_Reset_Db(NULL);
	}
#endif // CO_DCF_EXIF_ON

	strcpy(drv_name, (const CHAR*)gBF_Controller_drv_name[bMediaId]);
	ret = BF_Fs_If_QuickFormat(drv_name);
	if (ret != 0) {
		// quick format error
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief 		format a storage media (FullFormat).
 * @param		UCHAR	bMediaId
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_fullformatstoragemedia(FJ_MEDIA_ID bMediaId)
{
	INT32 ret;
	
	BF_Filesystem_File_Reset_Directory();

#ifdef CO_DCF_EXIF_ON
	if ((bMediaId == FJ_MEDIA_SD0) || (bMediaId == FJ_MEDIA_SD1)) {
		BF_Exif_Reset_Index();
		BF_Dcf_If_Set_Series_File_No(NULL, 0, 0);
		BF_Dcf_If_Reset_Db(NULL);
	}
	BF_Dcf_If_End_Task_Mdf();
#endif // CO_DCF_EXIF_ON

	ret = BF_Fs_If_Format_As(NULL, bMediaId);
	if (ret != 0) {
		// full format error
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief 		get progress of media format.
 * @param		UINT32* progress
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getformatprogress(UINT32* progress)
{
	INT32 ercd;
	ercd = BF_Fs_Controller_Get_Format_Progress((FW_LONG*)progress);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief 		Set Volume Label.
 * @param		CHAR* szDrvName	 : drive name
 * @param		CHAR* szVolLabel : volume label character string
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_setvollabel(CHAR *szDrvName, CHAR* szVolLabel)
{
	INT32 ercd;
	FJ_MEDIA_ID media_id;

	if (strlen((char*)szVolLabel) > 11) {
		return FJ_ERR_NG;
	}

	BF_Fs_Controller_Get_Media_Id(szDrvName, (CHAR*)&media_id);
	if ((UCHAR)media_id == 0xFF) {
		return FJ_ERR_NG;
	}

	BF_Media_Attr_Set_Attr_Sz_Vol_Label(media_id, (UCHAR*)szVolLabel);
	ercd = media_set_drv_vol_label(szDrvName, szVolLabel);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 * @brief 		Set Volume Label.
 * @param		FJ_MEDIA_ID	bMediaId    : target media
 *				CHAR*			szVolLabel : volume label character string
 * @return 		FJ_ERR_CODE : FJ_ERR_OK
 *                          : FJ_ERR_NG
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_setvollabeldrive(FJ_MEDIA_ID bMediaId, CHAR* szVolLabel)
{
	INT32 ercd;

	if (strlen((char*)szVolLabel) > 11) {
		return FJ_ERR_NG;
	}

	switch (bMediaId) {
		case FJ_MEDIA_NF_STORAGE0:
			// NF J drive
			if(E_BF_MEDIA_STATE_MEDIA_TYPE_NF == BF_Media_State_Get_Used_Media()) {
				ercd = media_set_drv_vol_label((CHAR*)"J", szVolLabel);
			}
			else{
				ercd = -1;
			}
			break;

		case FJ_MEDIA_SD0:
			// SD I drive
			if(E_BF_MEDIA_STATE_MEDIA_TYPE_NF != BF_Media_State_Get_Used_Media()) {
				ercd = media_set_drv_vol_label((CHAR*)"I", szVolLabel);
			}
			else{
				ercd = -1;
			}
			break;

		case FJ_MEDIA_NF_STORAGE1:
			// NF A drive
			ercd = media_set_drv_vol_label((CHAR*)"A", szVolLabel);
			break;

		case FJ_MEDIA_NF_STORAGE2:
			// NF B drive
			ercd = media_set_drv_vol_label((CHAR*)"B", szVolLabel);
			break;

		case FJ_MEDIA_NF_STORAGE3:
			// NF C drive
			ercd = media_set_drv_vol_label((CHAR*)"C", szVolLabel);
			break;

		default:
			ercd = -1;
			break;
	}

	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief 		Get Volume Label.
 * @param		CHAR* szDrvName	 : drive name
 * @param		CHAR* szVolLabel : volume label character string
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getvollabel(CHAR *szDrvName, CHAR* szVolLabel)
{
	INT32 ercd;

	// get volume label
	ercd = BF_Fs_If_Get_VolLabel((const FW_CHAR*)szDrvName, (const FW_CHAR*)szVolLabel);
	if(ercd < 0){
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

#ifdef CO_LINUX_VERSION_ON
/**
 * @brief	This function sets callback function to notice writing error for non shared memory.
 * @param	ulFileId 	The ID of the file to set for writing request.
 * @param	pCallback	The callback function is used when an writing error is occured for non shared memory.
 * @return	FJ_ERR_OK Normal end.
 * @return	FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE fj_set_vfs_write_err_cb( ULONG ulFileId, FsWriteErrCB pCallback )
{
	INT32 ercd = 0;
	
	ercd = BF_Fs_If_Set_Vfs_Write_Err_Cb( ulFileId, (FS_SET_WRITE_ERR_CB)pCallback );
	       
	if(ercd < 0){
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}
#endif	/* CO_LINUX_VERSION_ON */
