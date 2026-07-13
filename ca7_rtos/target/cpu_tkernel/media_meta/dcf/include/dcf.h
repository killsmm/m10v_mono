/**
 * @file		dcf.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _DCF_H_
#define _DCF_H_

#include "ddim_typedef.h"
#include "fj_filesystem.h"

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
// DCF API (romarea_boot)
//////////////////////////////////
extern FJ_ERR_CODE fj_dcfdcreate( VOID );
extern FJ_ERR_CODE fj_dcfdrenew( VOID );
extern FJ_ERR_CODE fj_directdcfdirectorycreate( USHORT dirNum, UCHAR dirNameIndex );
extern FJ_ERR_CODE fj_directdcfdirnum( USHORT* count );
extern FJ_ERR_CODE fj_directdcfdumpDiriattr( SHORT dir_index, USHORT* count, CHAR* dirName, USHORT* date, USHORT* time );
extern FJ_ERR_CODE fj_directdcfdumpdiriattrex( SHORT dir_index, USHORT* count, CHAR* dirName, USHORT* date, ULONG* time );
extern FJ_ERR_CODE fj_getallowdnum( SHORT obj_index, UCHAR* count );
extern FJ_ERR_CODE fj_getdcfdblongval( USHORT ParamID, LONG LongPrm, LONG* value );
extern FJ_ERR_CODE fj_getdcfinfo( INT32 ImageIndex, INT32* DirNum, INT32* FileNum, SHORT* FileType, INT32* Locked, INT32* FileDupStatus );
extern FJ_ERR_CODE fj_getdirduplicatestatus( SHORT dir_index, CHAR* dirdupStatus );
extern FJ_ERR_CODE fj_getdirinfo( SHORT dir_index, CHAR* dirName );
extern FJ_ERR_CODE fj_getmaxfilenumasdirnum( USHORT* file_no );
extern FJ_ERR_CODE fj_getparamarray( FJ_DCF_FREE_CHARS ParamID, CHAR* Offset, USHORT Size );
extern BOOL fj_is_dcf_file_type(const CHAR* ext_name, SHORT file_type );
extern FJ_ERR_CODE fj_setcurrentdirectory( SHORT dir_index );
extern FJ_ERR_CODE fj_setdcfdblongval( USHORT ParamID, LONG LongPrm, LONG value );
extern FJ_ERR_CODE fj_setdcffreechars( CHAR Dir, FJ_DIR_FILE_OPERATION Operation, CHAR* Chars );
extern FJ_ERR_CODE BF_Dcf_Get_Param_Long( INT32 ParamID, LONG* value );

//////////////////////////////////
// DCF API (romarea_common)
//////////////////////////////////
extern FJ_ERR_CODE fj_get_latest_index( INT32* Index );
extern FJ_ERR_CODE fj_getdcflastfilenumber( SHORT dir_index, USHORT* file_num );

//////////////////////////////////
// DCF API (romarea_view)
//////////////////////////////////
extern FJ_ERR_CODE	fj_getfirstfreefileno( USHORT* fileNum );
extern FJ_ERR_CODE fj_setnextdcfkey( USHORT DirNum, USHORT FileNum );

//////////////////////////////////
// DCF API (romarea_last)
//////////////////////////////////
extern FJ_ERR_CODE fj_setdcfdbstrval( USHORT ParamID, CHAR* Sring );
extern FJ_ERR_CODE fj_getdcfdbstrval( USHORT ParamID, LONG LongPrm, CHAR* Sring );
extern FJ_ERR_CODE fj_getdcfdblongvalbystr( USHORT ParamID, CHAR* Sring, LONG* value );
extern FJ_ERR_CODE fj_directdcfsetlockstatusallindirnumber( USHORT dir_index, INT32 fs_attr );
extern FJ_ERR_CODE fj_directdcfdeleteallindirnumber( USHORT dir_index );
extern FJ_ERR_CODE fj_dcf_execute_multi_delete(VOID);
extern FJ_ERR_CODE fj_dcf_init_select_list(VOID);
extern FJ_ERR_CODE fj_dcf_add_file_to_select_list(INT32 FileIndex);
extern FJ_ERR_CODE fj_dcf_remove_file_from_select_list(INT32 FileIndex);
extern FJ_ERR_CODE fj_getdcfdirfileexist( USHORT dir_index, UCHAR* file_exist );
extern FJ_ERR_CODE fj_dcfdirectorydelete(USHORT ParamID, USHORT dir_index);
extern FJ_ERR_CODE fj_dcf_make_new_file_path_by_ext( const CHAR* ext, CHAR* new_file_path );
extern FJ_ERR_CODE fj_dcf_update_dcfdb_add_file( CHAR* file_path );
extern FJ_ERR_CODE fj_dcf_del_single_obj( INT32 file_index );
extern FJ_ERR_CODE fj_dcf_del_all_obj( INT32 media_id );

#endif	// _DCF_H_
