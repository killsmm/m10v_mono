/**
 * @file		filesystem.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_filesystem.h"
#include "fs_if.h"

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
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/

//////////////////////////////////
// FileSystem API (romarea_boot)
//////////////////////////////////
extern ULONG       fj_fs_open(CHAR* szFullFileName, INT32 dwFlags);
extern VOID        fj_fs_close(ULONG ulFileId);
extern FJ_ERR_CODE fj_fs_read(ULONG ulFileId, UINT32* pbyBuffer, ULONG ulRequestSize, ULONG* ulLength);
extern FJ_ERR_CODE fj_fs_existrootdirectorycheck(FJ_MEDIA_ID bMediaId, UCHAR* pDcimExist, UCHAR* pMiscExist);

//////////////////////////////////
// FileSystem API (romarea_comm)
//////////////////////////////////
extern FJ_ERR_CODE fj_fs_filecopytodrive(FJ_MEDIA_ID bMediaId);

//////////////////////////////////
// FileSystem API (romarea_last)
//////////////////////////////////
extern FJ_ERR_CODE fj_fs_write(ULONG ulFileId, UINT32* pbyBuffer, ULONG ulRequestSize, ULONG* ulLength);
extern FJ_ERR_CODE fj_fs_writeasync(FsEntryCB pcallback, ULONG ulFileId, UINT32* pbyBuffer, ULONG ulRequestSize);
extern FJ_ERR_CODE fj_fs_getwritelengthas(ULONG* ulLength);
extern FJ_ERR_CODE fj_fs_lseekwithlimit(ULONG ulFileId, ULONG ulOffset);
extern FJ_ERR_CODE fj_fs_remove(CHAR* szTarget);
extern FJ_ERR_CODE fj_fs_removemulti(CHAR* szTarget, CHAR filename_list[][13], INT32 fnum, FsDeleteMultiCB pcallback);
extern FJ_ERR_CODE fj_fs_mkdir(CHAR* szTarget);
extern FJ_ERR_CODE fj_fs_chdir(CHAR* szInputPath);
extern FJ_ERR_CODE fj_fs_rename(CHAR* szSource, CHAR* szTarget);
extern FJ_ERR_CODE fj_directfsdirectoryentryget(INT32 bFirstEntry, CHAR* szOutput, UINT32 uiOutputLen, UINT32* puiAttributes, ULONG* pulSize);
extern FJ_ERR_CODE fj_fs_setsearchentry(CHAR* szEntryName);
extern FJ_ERR_CODE fj_fs_getattribute(CHAR* szName, UINT32* puiAttributes);
extern FJ_ERR_CODE fj_fs_setattribute(CHAR* szName, UINT32 puiAttributes);
extern FJ_ERR_CODE fj_fs_setattributemulti(CHAR* szName, UINT32 puiAttributes, UCHAR ucAttrmode, CHAR filename_list[][13], INT32 fnum, FsAttributesSetMultiCB pcallback);
extern FJ_ERR_CODE fj_fs_filesave(const char* szFullFileName, UINT32* addr, INT32 size);
extern FJ_ERR_CODE fj_fs_rootentriescount(ULONG* count);
extern FJ_ERR_CODE fj_fs_stat(CHAR* szName, FJ_FS_STAT* pStat);
extern FJ_ERR_CODE fj_fs_statex(CHAR* szName, FJ_FS_STAT_EX* pStatEx);
extern FJ_ERR_CODE fj_fs_utimeex(CHAR* szName, USHORT timestamp_type, FJ_FS_TIME* ptime);
extern FJ_ERR_CODE fj_fs_lseek(ULONG ulFileId, ULONG ulOffset, USHORT whence, ULONG* pulLength);
extern FJ_ERR_CODE fj_fs_lseek64(ULONG ulFileId, INT64 ulOffset, USHORT whence, INT64* pulLength);
extern FJ_ERR_CODE fj_fs_fsync(ULONG ulFileId);
extern FJ_ERR_CODE fj_fs_sync(CHAR* szDrvName);
extern FJ_ERR_CODE fj_fs_getusersettingdata( UINT32 *pbyBuffer, ULONG ulRequestSize, ULONG ulOffset );
extern FJ_ERR_CODE fj_fs_setusersettingdata( UINT32 *pbyBuffer, ULONG ulRequestSize );
//unused
//extern FJ_ERR_CODE fj_directfsfilerelseek(ULONG ulFileId, ULONG ulOffset, BYTE byDir);
//extern ULONG       fj_directfsfilegetline(ULONG ulFileId, BYTE* pbyBuffer, ULONG ulBufferLen);

extern FJ_ERR_CODE fj_mount(FJ_MEDIA_ID bMediaId, CHAR *szDrvName);
extern FJ_ERR_CODE fj_unmount(CHAR* szDrvName);
extern FJ_ERR_CODE fj_getstoragemediastatus(USHORT* pwSuccess, USHORT* pwImageCount, USHORT* pwImageLeft, USHORT* pwSizeMS, USHORT* pwSizeLS);
extern FJ_ERR_CODE fj_getstoragemediastatusex(USHORT* pwSuccess, UINT32* pwImageCount, UINT32* pwImageLeft, UINT32* pwSizeMS, UINT32* pwSizeLS);
extern FJ_ERR_CODE fj_getstoragemediageometryex(FJ_MEDIA_ID bMediaId, FJ_FS_GEOMETRY* pGeometry);
extern FJ_ERR_CODE fj_formatstoragemedia(FJ_MEDIA_ID bMediaId);
extern FJ_ERR_CODE fj_fullformatstoragemedia(FJ_MEDIA_ID bMediaId);
extern FJ_ERR_CODE fj_getformatprogress(UINT32* progress);
extern FJ_ERR_CODE fj_checkmbrinternalmemory(VOID);
extern FJ_ERR_CODE fj_makembrinternalmemory(VOID);
extern FJ_ERR_CODE fj_get_media_format_type(UCHAR* media, UCHAR* format_type);
extern FJ_ERR_CODE fj_setvollabel(CHAR *szDrvName, CHAR* szVolLabel);
extern FJ_ERR_CODE fj_setvollabeldrive(FJ_MEDIA_ID bTarget, CHAR* szVolLabel);
extern FJ_ERR_CODE fj_getvollabel(CHAR *szDrvName, CHAR* szVolLabel);
extern FJ_ERR_CODE fj_drivetodrivecopy(CHAR* szSrcDrive, CHAR* szDstDrive);
extern FJ_ERR_CODE fj_set_vfs_write_err_cb( ULONG ulFileId, FsWriteErrCB pCallback );

#endif	// _FILESYSTEM_H_
