/**
 * @file		fj_filesystem.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_filesystem.h"
#include "filesystem.h"
#include "mode.h"
#include "media_state.h"

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
/* Local Function												  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/////////////////////////////////////////////////////
// romarea_boot
/////////////////////////////////////////////////////

/**
 * @brief		open the file.
 * @param		CHAR* szName
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
ULONG FJ_Fs_Open(CHAR* szName, INT32 dwFlags)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return 0;	// error return
	}

	return fj_fs_open(szName, dwFlags);
}

/**
 * @brief		close the file.
 * @param		ULONG	ulFileId
 * @return		None
 * @note		None
 * @attention	None
 */
VOID FJ_Fs_Close(ULONG ulFileId)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return;
	}

	fj_fs_close(ulFileId);
}

/**
 * @brief		read the file.
 * @param		ULONG	ulFileId
 *				UINT32*	pbyBuffer
 *				ULONG	ulRequestSize
 *				ULONG*	ulLength
 * @return 		Success/Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_Read(ULONG ulFileId, UINT32* pbyBuffer, ULONG ulRequestSize, ULONG* ulLength)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_read(ulFileId, pbyBuffer, ulRequestSize, ulLength);
}

/**
 * @brief		write the file
 * @param		ULONG	ulFileId
 *				UINT32*	pbyBuffer
 *				ULONG	ulRequestSize
 *				ULONG*	ulLength
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_Write(ULONG ulFileId, UINT32* pbyBuffer, ULONG ulRequestSize, ULONG* ulLength)
{
	// romarea_view
	if (BF_Mode_Check(D_BF_MODE_VIEW_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_write(ulFileId, pbyBuffer, ulRequestSize, ulLength);
}

/**
 * @brief		write the file (Async)
 * @param		FsEntryCB pCallback
 *				ULONG	ulFileId
 *				UINT32*	pbyBuffer
 *				ULONG	ulRequestSize
 *				ULONG*	ulLength
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_WriteAsync(FsEntryCB pCallback, ULONG ulFileId, UINT32* pbyBuffer, ULONG ulRequestSize)
{
	// romarea_view
	if (BF_Mode_Check(D_BF_MODE_VIEW_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_writeasync(pCallback, ulFileId, pbyBuffer, ulRequestSize);
}

/**
 * @brief		Get the write length
 * @param		ULONG*	ulLength
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		None
 * @remarks		If necessary, use this function in a callback to set in FJ_Fs_WriteAsync().
 */
FJ_ERR_CODE FJ_Fs_GetWriteLengthAs(ULONG* length)
{
	// romarea_view
	if (BF_Mode_Check(D_BF_MODE_VIEW_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_getwritelengthas(length);
}

/**
 * @brief		delete the file/directory
 * @param		CHAR*	szName
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_Remove(CHAR* szName)
{
	// romarea_view
	if (BF_Mode_Check(D_BF_MODE_VIEW_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_remove(szName);
}

/**
 * @brief		delete the file/directory
 * @param 		CHAR*	szName 
 *				CHAR	filename_list[][13] 
 * 				INT32	iFnum 
 * 				FsDeleteMultiCB	pcallback 
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_RemoveMulti(CHAR* szName, CHAR filename_list[][13], INT32 iFnum, FsDeleteMultiCB pcallback)
{
	// romarea_view
	if (BF_Mode_Check(D_BF_MODE_VIEW_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_removemulti(szName, filename_list, iFnum, pcallback);
}


/**
 * @brief		creaqte the directory
 * @param		CHAR*	szName
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_Mkdir(CHAR* szName)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_mkdir(szName);
}

/**
 * @brief		changes the path from the current directory.
 * @param		CHAR*	szInputPath
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_Chdir(CHAR* szInputPath)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_chdir(szInputPath);
}

/**
 * @brief		rename the file/directory
 * @param		CHAR*	szSrcName
 *				CHAR*	szDstName
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_Rename(CHAR* szSrcName, CHAR* szDstName)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_rename(szSrcName, szDstName);
}

/**
 * @brief		read attributes of file/directory
 * @param		CHAR*	szName
 *				UINT32*	puiAttributes
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		0x00	: normal file
   				0x01	: read only file
   				0x02	: hidden file
   				0x04	: system file
   				0x10	: directory
   				0x20	: archive file
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_GetAttribute(CHAR* szName, UINT32* puiAttributes)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_getattribute(szName, puiAttributes);
}

/**
 * @brief		set attributes of file/directory
 * @param		CHAR*	szName
 *				UINT32	puiAttributes
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		0x00	: normal file
   				0x01	: read only file
   				0x02	: hidden file
   				0x04	: system file
   				0x10	: directory
   				0x20	: archive file
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_SetAttribute(CHAR* szName, UINT32 puiAttributes)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_setattribute(szName, puiAttributes);
}

/**
 * @brief		set attributes of file/directory
 * @param		CHAR*	szName
 *				UINT32	puiAttributes
 *				UCHAR 	ucAttrmode
 *				CHAR	filename_list[][13]
 *				INT32	iFnum
 *				FsAttributesSetMultiCB	pcallback
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
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
FJ_ERR_CODE FJ_Fs_SetAttributeMulti(CHAR* szName, UINT32 puiAttributes, UINT32 ucAttrmode,
										  CHAR filename_list[][13], INT32 iFnum, FsAttributesSetMultiCB pcallback)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_setattributemulti(szName, puiAttributes, ucAttrmode, filename_list, iFnum, pcallback);
}


/**
This function gets file/directory information.
@param[in] szName The name of the file/directory
@param[out] pStat Information on file/directory. See @ref FJ_FS_STAT
@retval FJ_ERR_OK : Normal end.
@retval FJ_ERR_NG : Error occurred.
*/
FJ_ERR_CODE FJ_Fs_Stat(CHAR* szName, FJ_FS_STAT* pStat)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_stat(szName, pStat);
}

/**
This function gets file/directory information.
@param[in] szName The name of the file/directory
@param[out] pStatEx Information on file/directory. See @ref FJ_FS_STAT_EX
@retval FJ_ERR_OK : Normal end.
@retval FJ_ERR_NG : Error occurred.
*/
FJ_ERR_CODE FJ_Fs_StatEx(CHAR* szName, FJ_FS_STAT_EX* pStatEx)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_statex(szName, pStatEx);
}

/**
This function changes the date and time of demanded file or directory.<br>
@param[in] szName The name of the file or directory
@param[in] timestamp_type The kind of the TimeStamp that changes is set.<br>
						The parameter that can be set is three of the following.<br>
						@ref FJ_FS_TIMESTAMP_MODIFY, @ref FJ_FS_TIMESTAMP_CREATE, @ref FJ_FS_TIMESTAMP_ACCESS
@param[in] ptime Information of date and time<br>
@retval FJ_ERR_OK : Normal end.
@retval FJ_ERR_NG : Error occurred.
*/
FJ_ERR_CODE FJ_Fs_UtimeEx(CHAR* szName, USHORT timestamp_type, FJ_FS_TIME* ptime)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
			// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_utimeex(szName, timestamp_type, ptime);
}

/**
 * @brief		seek the file.
 * @param		ULONG	ulFileId
 *				ULONG	ulOffset
 *				USHORT	whence
 *				ULONG*	pulLength
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_Lseek(ULONG ulFileId, ULONG ulOffset, USHORT whence, ULONG* pulLength)
{
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_lseek(ulFileId, ulOffset, whence, pulLength);
}

/**
 * @brief		seek the file.
 * @param		ULONG	ulFileId
 *				INT64	ulOffset
 *				USHORT	whence
 *				INT64*	pulLength
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_Lseek64(ULONG ulFileId, INT64 iOffset, USHORT whence, INT64* piLength)
{
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_lseek64(ulFileId, iOffset, whence, piLength);
}

/**
 * @brief		seek the file (Only as for the original course).
 * @param		ULONG	ulFileId
 *				ULONG	ulOffset
 * @return		success	:	1
 *				fail	:	0
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_LseekWithLimit(ULONG ulFileId, ULONG ulOffset)
{
	// romarea_view
	if (BF_Mode_Check(D_BF_MODE_VIEW_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_lseekwithlimit(ulFileId, ulOffset);
}

/**
 * @brief		Flush the drive
 * @param		ULONG	ulFileId
 *				ULONG	ulOffset
 *				USHORT	whence
 *				ULONG*	pulLength
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_Fsync(ULONG ulFileId)
{
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_fsync(ulFileId);
}

/**
 * @brief		Sync the drive
 * @param		ULONG	ulFileId
 *				ULONG	ulOffset
 *				USHORT	whence
 *				ULONG*	pulLength
 * @return		success	:	FJ_ERR_OK
 *				fail	:	FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_Sync(CHAR* szDrvName)
{
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_sync(szDrvName);
}

FJ_ERR_CODE	FJ_Fs_FileSave(const char* szName, UINT32* addr, INT32 size)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_filesave(szName, addr, size);
}

/**
 * @brief 		Check the status of the "DCIM" and "MISC" directory exist or not. 
 * @param		i:FJ_MEDIA_ID bMediaId
 * @param		o:BYTE* pDcimExist
 * @param		o:BYTE* pMiscExist
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_Fs_ExistRootDirectoryCheck(FJ_MEDIA_ID bMediaId, UCHAR* pDcimExist, UCHAR* pMiscExist)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_existrootdirectorycheck(bMediaId, pDcimExist, pMiscExist);
}

/**
 * @brief		copy the files to NF data drive
 * @param		FJ_MEDIA_ID bMediaId
 * @return 		Success/Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_FileCopyToDrive(FJ_MEDIA_ID bMediaId)
{
	// romarea_comm
	if (BF_Mode_Check(D_BF_MODE_VIEW_CODE | D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_filecopytodrive(bMediaId);
}

/**
 * @brief 		Returns entries(file or directory) information of the current directory one by one.
 * @param		CHAR* szEntryName
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_Fs_SetSearchEntry(CHAR* szEntryName)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_setsearchentry(szEntryName);
}

/**
 * @brief		get the entry
 * @param		FJ_FS_ENTRY_MODE bEntryMode
 *				UINT32*	szOutput
 *				UINT32	uiOutputlen
 *				UINT32*	puiAttributes
 *				ULONG*	pulSize
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE FJ_Fs_DirectoryEntryGet(
				FJ_FS_ENTRY_MODE bEntryMode,
				CHAR*	szOutput,
				UINT32	uiOutputLen,
				UINT32*	puiAttributes,
				ULONG*	pulSize)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_directfsdirectoryentryget(bEntryMode, szOutput, uiOutputLen, puiAttributes, pulSize);
}

/**
 * @brief 		Get the root entries count for FAT12 & FAT16.
 * @param		ULONG	*count
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_Fs_RootEntriesCount(ULONG* count)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_rootentriescount(count);
}

/**
 * @brief	This function gets a user setting data from user setting area on SDRAM.
 * @param	pbyBuffer		The buffer address for read
 * @param	ulRequestSize	The requested number of bytes to read
 * @param	ulOffset		The offset into user setting area.
 * @return	FJ_ERR_OK 		Normal end.
 * @return	FJ_ERR_NG 		Error occurred.
*/ 
FJ_ERR_CODE FJ_Fs_GetUserSettingData( UINT32 *pbyBuffer, ULONG ulRequestSize, ULONG ulOffset )
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fs_getusersettingdata( pbyBuffer, ulRequestSize, ulOffset );
}

/**
 * @brief	This function sets a user setting data to user setting area on nand flash.
 * @param	pbyBuffer The buffer address for a user setting data to write.
 * @param	ulRequestSize The size for a user setting data to write.
 * @return	FJ_ERR_OK Normal end.
 * @return	FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_Fs_SetUserSettingData( UINT32 *pbyBuffer, ULONG ulRequestSize )
{
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}
	
	return fj_fs_setusersettingdata( pbyBuffer, ulRequestSize );
}

/**
 * @brief 		mount a new storage media.
 * @param		FJ_MEDIA_ID bMediaId
 *				CHAR *szDrvName
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_Mount(FJ_MEDIA_ID bMediaId, CHAR *szDrvName)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_mount(bMediaId, szDrvName);
}

/**
 * @brief 		unmount a storage media.
 * @param		CHAR *szDrvName
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE	FJ_UnMount(CHAR *szDrvName)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_unmount(szDrvName);
}

/**
 * @brief 		Get Media Information.
 * @param		FJ_MEDIA_ID bMediaId
 *				FJ_FS_GEOMETRY* pGeometry
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_GetStorageMediaGeometryEx(FJ_MEDIA_ID bMediaId, FJ_FS_GEOMETRY* pGeometry)
{
	// romarea_boot
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_getstoragemediageometryex(bMediaId, pGeometry);
}

/**
 * @brief 		format a storage media (QuickFormat).
 * @param		FJ_MEDIA_ID bMediaId
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_FormatStorageMedia(FJ_MEDIA_ID bMediaId)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_formatstoragemedia(bMediaId);
}

/**
 * @brief 		format a storage media (FullFormat).
 * @param		FJ_MEDIA_ID bMediaId
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_FullFormatStorageMedia(FJ_MEDIA_ID bMediaId)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_fullformatstoragemedia(bMediaId);
}

/**
 * @brief 		get progress of media format.
 * @param		UINT32* progress
 * @return 		FJ_ERR_CODE
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_GetFormatProgress(UINT32* progress)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_getformatprogress(progress);
}

/**
 * @brief 		Set Volume Label.
 * @param		CHAR *szDrvName
 *				CHAR* szVolLabel
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_SetVolLabel(CHAR *szDrvName, CHAR* szVolLabel)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_setvollabel(szDrvName, szVolLabel);
}

/**
 * @brief 		Set Volume Label.
 * @param		CHAR *szDrvName
 *				CHAR* szVolLabel
 * @return 		FJ_ERR_CODE : FJ_ERR_OK 
 *                          : FJ_ERR_NG 
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE	FJ_GetVolLabel(CHAR *szDrvName, CHAR* szVolLabel)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_getvollabel(szDrvName, szVolLabel);
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
FJ_ERR_CODE FJ_DriveToDriveCopy(CHAR* szSrcDrive, CHAR* szDstDrive)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_drivetodrivecopy(szSrcDrive, szDstDrive);
}

FJ_ERR_CODE FJ_Set_NF_Storage_ID(UCHAR id)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	BF_Media_Set_NF_Storage_ID(id);
	return FJ_ERR_OK;
}

FJ_ERR_CODE FJ_Set_EMMC_Storage_ID(UCHAR id)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	BF_Media_Set_EMMC_Storage_ID(id);
	return FJ_ERR_OK;
}

UCHAR FJ_Get_NF_Storage_ID(VOID)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return BF_Media_Get_NF_Storage_ID();
}

UCHAR FJ_Get_EMMC_Storage_ID(VOID)
{
	// romarea_last
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return BF_Media_Get_EMMC_Storage_ID();
}

#ifdef CO_LINUX_VERSION_ON
/**
 * @brief	This function sets callback function to notice writing error for non shared memory.
 * @param	ulFileId 	The ID of the file to set for writing request.
 * @param	pCallback	The callback function is used when an writing error is occured for non shared memory.
 * @return	FJ_ERR_OK Normal end.
 * @return	FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_Set_VFS_Write_Err_CB( ULONG ulFileId, FsWriteErrCB pCallback )
{
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;	// error return
	}

	return fj_set_vfs_write_err_cb( ( FW_INT32 )ulFileId, pCallback );
}
#endif
