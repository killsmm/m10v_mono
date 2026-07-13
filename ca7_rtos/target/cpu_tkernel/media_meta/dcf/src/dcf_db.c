/**
 * @file		dcf_db.c
 * @brief		Cashing of DCF files information.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
/* ////////////////////////////////////////////////////////////////////////////
; INCLUDE
//////////////////////////////////////////////////////////////////////////// */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dcf_db.h"
#include "dcf_controller.h"
#include "fs_controller.h"
#include "mdf_wrapper.h"
#include "fj_std.h"
#include "fj_custom.h"
#include "fj_host.h"
#include "dcf_attr.h"
#include "debug.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_BF_DCF_DB_BUF_NUM			(3)				/* 3 folders */
#define D_BF_DCF_DB_EXT_TYPE_NUM	(8)

#define D_BF_DCF_DB_DIR_START_NUM	(100)
#define D_BF_DCF_DB_DIR_END_NUM		(999)
#define D_BF_DCF_DB_DIR_BUF_MAX		(D_BF_DCF_DB_DIR_END_NUM - D_BF_DCF_DB_DIR_START_NUM + 1)

#define D_BF_DCF_DB_OBJ_START_NUM	(0)
#define D_BF_DCF_DB_OBJ_END_NUM		(9999)
#define D_BF_DCF_DB_OBJ_BUF_MAX		(D_BF_DCF_DB_OBJ_END_NUM - D_BF_DCF_DB_OBJ_START_NUM + 1)

#define dcf_db_get_prev_buf_num ((0 == g_bf_dcf_db_current_db) ? 2 : (g_bf_dcf_db_current_db - 1))
#define dcf_db_get_next_buf_num ((2 == g_bf_dcf_db_current_db) ? 0 : (g_bf_dcf_db_current_db + 1))

#define D_BF_DCF_DB_MIN_FILE_NUM		(1)
#define D_BF_DCF_DB_MAX_FILE_NUM		(9999)

#define D_BF_DCF_DB_DEBUG_MSG_ON

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/**
 * @breif self voice memo
 */
typedef enum {
	E_BF_DCF_DB_SELF_EXIST_TURE = 0x00,
	E_BF_DCF_DB_SELF_EXIST_FALSE = 0x01
} E_BF_DCF_DB_SELF_EXIST;

/**
 * @breif DB compare state
 */
typedef enum {
	E_BF_DCF_DB_COMPARE_STATE_UNDER = 0x00,
	E_BF_DCF_DB_COMPARE_STATE_OVER = 0x01,
	E_BF_DCF_DB_COMPARE_STATE_OPTION_EXIST = 0x02,
	E_BF_DCF_DB_COMPARE_STATE_OPTION_SELF = 0x03,
//	E_BF_DCF_DB_COMPARE_STATE_DUPLICATE = 0x04
} E_BF_DCF_DB_COMPARE_STATE;

/**
 * @breif dcf_db_get_select_dir function operation
 */
typedef enum {
	E_BF_DCF_DB_ASSIGN_LAST_KEY = 0x00,
	E_BF_DCF_DB_ASSIGN_DUPLICATE = 0x01,
	E_BF_DCF_DB_ASSIGN_HIDDEN = 0x02,
} E_BF_DCF_DB_ASSIGN;


/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/

/* Directory Control Block
	dirTblIdxTop								: Table index of directory of minimum number(0 ~ 899)
	dirTblIdxLast								: Table index of directory of maximum number(0 ~ 899)
	dirCnt										: total number of directorys(0 ~ 899)
	topDirName[D_BF_DCF_IF_TOP_DIR_NAME_MAX]	: DCF image directory(DCIM) path
*/
typedef struct {
	FW_SHORT dirTblIdxTop;
	FW_SHORT dirTblIdxLast;
	FW_SHORT dirCnt;
	FW_CHAR topDirName[D_BF_DCF_IF_TOP_DIR_NAME_MAX];
} T_BF_DCF_DB_DIR_CB;

/* directory information
	dirIndex		: Directory index(000 ~ 999)
	date			: Created date
	time1			: Created time
	sec10m			: This value has 10ms at the create time.(000 ~ 199)
					  This value is added at the second of the "time1".
	dirName[6]		: Directory name. & exist-flag
	dir_dupstatus	: directory duplication status. 0:unduplicate,1:duplicate
*/
typedef struct {
	FW_SHORT dirIndex;
	FW_USHORT date;
	FW_USHORT time1;
	FW_UCHAR sec10m;
	FW_CHAR dirName[6];
	FW_CHAR dir_dupstatus;
} T_BF_DCF_DB_DIR_INFO;

/* The overall object information on each directory
	objTblIdxTop	: Table index of file of minimum number(0 ~ 9999)
	objTblIdxLast	: Table index of file of maximum number(0 ~ 9999)
	dirTblIdx;		: Table index of directory with file
	objCnt;			: Total number of files in this directory & exist-flag (-1 == no-exist)
	lastFileNum;	: Last file number(containing the not allowed file)
*/
typedef struct {
	FW_SHORT objTblIdxTop;
	FW_SHORT objTblIdxLast;
	FW_SHORT dirTblIdx;
	FW_SHORT objCnt;
	FW_SHORT lastFileNum;
} T_BF_DCF_DB_BUF;


/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
/* ////////////////////////////////////////////////////////////////////////////
; WORK AREA
//////////////////////////////////////////////////////////////////////////// */

// directory create name
static volatile FW_CHAR g_bf_dcf_db_dir_name_dcf[6] = FJ_CUSTOM_DCF_DIR_NAME;

// file create name
static volatile FW_CHAR g_bf_dcf_db_file_name_still[5] = FJ_CUSTOM_DCF_FILE_NAME;
/*-----------------------------------------------------------------------------
 Directory
-----------------------------------------------------------------------------*/

// Overall information on directory
static volatile T_BF_DCF_DB_DIR_CB   g_bf_dcf_db_dir_ctrl_block;

// Information on each directory
static volatile T_BF_DCF_DB_DIR_INFO g_bf_dcf_db_dir_tbl[D_BF_FS_USER_CUSTOM_DCF_DB_DIR_INFO_NUM];

// Number of directory of DCF file name format
static volatile FW_SHORT g_bf_dcf_db_file_num_as_dir_num;	// 100 ~ 999

// Number of latest directory
static volatile FW_USHORT g_bf_dcf_db_latest_dir_num = 0;	// 100 ~ 999

// Latest number of duplication directory
static volatile FW_USHORT g_bf_dcf_db_duplicate_dir_num = 0;	// 100 ~ 999

// Latest number of hidden directory
static volatile FW_USHORT g_bf_dcf_db_hidden_dir_num = 0;	// 100 ~ 999

/*-----------------------------------------------------------------------------
 Object
-----------------------------------------------------------------------------*/

// The overall object information on each directory
static T_BF_DCF_DB_BUF g_bf_dcf_db_db_buf_tbl[D_BF_DCF_DB_BUF_NUM];

// Index of table with object information on current directory
static FW_SHORT g_bf_dcf_db_current_db;

// Index of table with object information on cash1 directory
static FW_SHORT g_bf_dcf_db_cash1_db;

// Index of table with object information on cash2 directory
static FW_SHORT g_bf_dcf_db_cash2_db;

// Object information on each directory
static T_BF_DCF_DB_OBJ_INFO g_bf_dcf_db_obj_tbl[D_BF_DCF_DB_BUF_NUM][D_BF_DCF_DB_OBJ_BUF_MAX];


/*-----------------------------------------------------------------------------
 Extension Table
-----------------------------------------------------------------------------*/
// Allow extension table
static FW_CHAR g_bf_dcf_db_allow_ext_name[8 * 4 + 1] = "\0";

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
/* ////////////////////////////////////////////////////////////////////////////
; PROTYPE
//////////////////////////////////////////////////////////////////////////// */
/*=============================================================================
 Static Function List
=============================================================================*/
/* utility */
static FW_SHORT dcf_db_get_file_no(FW_CHAR* fname);
static FW_INT32 dcf_db_get_dir_path(FW_CHAR* full_file_path, FW_CHAR* dir_path);

/* get DB info */
static FW_INT32 dcf_db_get_index_file_info(T_BF_DCF_IF_FILE_INDEX file_index, FW_CHAR* full_file_path, T_BF_DCF_IF_FILE_INFO* file_info);
static FW_INT32 dcf_db_get_index_file_type(T_BF_DCF_IF_FILE_INDEX file_index, FW_UCHAR* file_type);
static FW_INT32 dcf_db_insert_dir(const FW_CHAR* dir_fullpass_name, FW_SHORT* p_dir_index);

/* check */
static BOOL dcf_db_is_priority_ext(FW_ULONG extTypePrm, FW_ULONG extTypeBase);
static BOOL dcf_db_is_top_dir_name(const FW_CHAR topDirNamePrm[]);

/* search */
static FW_INT32 dcf_db_search_db_buf(FW_SHORT dirIndexPrm, FW_SHORT* bufTblIdx);
static FW_INT32 dcf_db_search_dir_info_on_db_buf(FW_SHORT dirIndexPrm, FW_SHORT* dirTblIdx);
static FW_INT32 dcf_db_search_dir_info_out_side_db_buf(FW_SHORT dirIndexPrm, FW_SHORT* dirTblIdx);
static FW_INT32 dcf_db_search_dir_info(FW_SHORT dirIndexPrm, FW_SHORT* dirTblIdx);
static FW_INT32 dcf_db_search_obj_info_by_db(FW_SHORT bufTblIdx, FW_SHORT objIndexPrm, FW_SHORT* objTblIdx);
static FW_INT32 dcf_db_search_dir_tbl(FW_SHORT dirIndex, FW_SHORT* dirTblIdx);
static FW_INT32 dcf_db_search_obj_tbl(FW_SHORT bufTblIdx, FW_SHORT objIndex, FW_SHORT* objTblIdx);

/* DataBase */
static FW_INT32 dcf_db_set_db_buf(FW_SHORT dirIndexPrm, FW_SHORT dirTblIdx, FW_INT32 skipFlag, FW_SHORT bufTblIdx);
static FW_INT32 dcf_db_insert_file(FW_CHAR* file_path, FW_CHAR* full_file_path, FW_SHORT bufTblIdx, T_BF_FS_IF_Dir_Info* pdir, FW_CHAR* objNameOnly);
static FW_VOID dcf_db_set_obj_db_buf_tbl(FW_SHORT bufTblIdx);
static FW_VOID dcf_db_set_dir_db_buf_tbl(FW_VOID);
//static FW_VOID dcf_db_get_dir_tbl_idx(FW_SHORT* dirTblIdx);
static FW_INT32 dcf_db_set_file_flag(FW_CHAR* file_path,	FW_CHAR* full_file_path, FW_SHORT bufTblIdx, T_BF_FS_IF_Dir_Info* pdir);
static FW_INT32 dcf_db_get_last_file_num( FW_SHORT dirTblIdx, FW_USHORT* filenum);
static FW_INT32 dcf_db_get_select_dir(FW_USHORT* dirno, FW_USHORT* fileno, E_BF_DCF_DB_ASSIGN operation);

static FW_INT32 dcf_db_get_dcf_root_dir_len(FW_CHAR* full_file_path, FW_USHORT* length);



/* ////////////////////////////////////////////////////////////////////////////
; PROGRAM
//////////////////////////////////////////////////////////////////////////// */
/**
 * @brief Get file number.
 * @param i:*fname DCF file name
 * @return 1..9999:DCF file number
 * @rerurn -103:not support file number
 */
static FW_SHORT dcf_db_get_file_no(FW_CHAR* fname)
{
	FW_INT32 name_cnt;
	FW_SHORT fileno;

	if(fname == NULL) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}

	// length check
	if (strlen(fname) < 12){
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}

	// charactor check
	for (name_cnt = 0; name_cnt < 4; name_cnt++) {
		if ((!isdigit((FW_INT32)(fname[name_cnt]))) &&
			(!isalpha((FW_INT32)(fname[name_cnt]))) &&
			(fname[name_cnt] != '_')) {
			return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
		}
	}
	for (name_cnt = 4; name_cnt < 8; name_cnt++) {
		if(!isdigit((FW_INT32)(fname[name_cnt]))){
			return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
		}
	}
	if(fname[8] != '.') {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}

	// from charactor to number
	fileno = (fname[4] - '0') * 1000 + (fname[5] - '0') * 100
			 + (fname[6] - '0') * 10 + (fname[7] - '0');

	// check file number
	if ((fileno == FJ_CUSTOM_DCFDB_FILENUM_MIN ) || (fileno > FJ_CUSTOM_DCFDB_FILENUM_MAX)) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}

	return fileno;
}


/**
 * @brief get file info and file path of specified index file with DB.
 * @param i:file_index file index
 * @param o:*full_file_path full file path
 * @param o:*file_info file information
 * @return error code
 * @note if full_file_path or file_info is NULL, these output is not get.
 */
static FW_INT32 dcf_db_get_index_file_info(T_BF_DCF_IF_FILE_INDEX file_index, FW_CHAR* full_file_path, T_BF_DCF_IF_FILE_INFO* file_info)
{
	FW_INT32 ercd;
	FW_CHAR check_filepath[32];
	FW_SHORT bufTblIdx;
	FW_SHORT dirTblIdx;
	FW_SHORT objTblIdx;

	/* get full file path  */
	ercd = dcf_db_search_db_buf(file_index.pack.dir_index, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_search_obj_info_by_db(bufTblIdx, file_index.pack.obj_index, &objTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* make full file path */
	ercd = BF_Dcf_Db_Make_Full_Path(file_index, check_filepath);
	if( ercd != 0 ) {
		return ercd;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
//	BF_Fs_User_Custom_Info_Printer("path=%s\n", check_filepath);
#endif

	/* get file path */
	if(full_file_path != NULL){
		strcpy(full_file_path, check_filepath);
	}

	/* get file info */
	if(file_info != NULL){

		file_info->media_type = BF_Media_State_Get_Used_Media();

		dirTblIdx = g_bf_dcf_db_db_buf_tbl[bufTblIdx].dirTblIdx;

		file_info->dir_no = dirTblIdx + D_BF_DCF_DB_DIR_START_NUM;

		file_info->file_no = objTblIdx;

		memcpy((FW_CHAR*)(file_info->file_char), g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName, 4);
		file_info->file_char[4] = 0;

		file_info->file_type = g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType;

		file_info->file_attr = (E_BF_DCF_IF_ATTR)(1 & (g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].attr));

		file_info->option_file = (E_BF_DCF_IF_OPTION_FILE)((g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag & 0x04) >> 2);

		file_info->file_dupstatus = (E_BF_DCF_IF_DUPLICATION_STATUS)(g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag >> 7);
	}

	return 0;
}

/**
 * @brief get file type of specified index file with DB.
 * @param i:file_index file index
 * @param o:*file_type file type
 * @return error code
 * @note if file_info is NULL, these output is not get.
 */
static FW_INT32 dcf_db_get_index_file_type(T_BF_DCF_IF_FILE_INDEX file_index, FW_UCHAR* file_type)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx;
	FW_SHORT objTblIdx;

	/* get db file node  */
	ercd = dcf_db_search_db_buf(file_index.pack.dir_index, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_search_obj_info_by_db(bufTblIdx, file_index.pack.obj_index, &objTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* get file type */
	if(file_type != NULL){
		*file_type = g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType;
	}

	return 0;
}

/**
 * @brief check priority of Extension between 2 parameters.
 * @param i:FW_ULONG extTypePrm
 * @param i:FW_ULONG extTypeBase
 * @return TRUE: extTypePrm > extTypeBase
 * @attention
 */
static BOOL dcf_db_is_priority_ext(FW_ULONG extTypePrm, FW_ULONG extTypeBase)
{
	FW_INT32 index;
	FW_ULONG checkBit;

	/* check parameter */
	if(((FW_ULONG)NULL) == extTypePrm) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(((FW_ULONG)NULL) == extTypeBase) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	checkBit = 1;
	for(index = 0; index < D_BF_DCF_DB_EXT_TYPE_NUM; index++) {
		if(0 != (extTypeBase & checkBit)) {
			return FALSE;
		}
		if(0 != (extTypePrm & checkBit)) {
			return TRUE;
		}
		checkBit <<= 1;
	}

	return FALSE;
}


/**
 * @brief check top directory name.
 * @param i:FW_CHAR* TopDirNamePrm
 * @return BOOL
 * @attention expect directory name "A:\DCIM"
 */
static BOOL dcf_db_is_top_dir_name(const FW_CHAR topDirNamePrm[])
{
	FW_INT32 index;
	FW_INT32 len;
	FW_INT32 lenTmp;

	/* check parameter */
	if(NULL == topDirNamePrm) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	// length check
	len = (FW_INT32)strlen(topDirNamePrm);
	if(7 > len) {
		return FALSE;
	}

	// check charactor
	lenTmp = (len - 7);
	for(index = 0; index < lenTmp; index++) {
		if(!isalpha((FW_INT32)(topDirNamePrm[index]))) {
			return FALSE;
		}
	}
	if(0 != strcmp(&(topDirNamePrm[index]), ":\\DCIM\\")) {
		return FALSE;
	}

	return TRUE;
}

/**
 * @brief search DataBase buffer table index by directory index
 * @param i:FW_SHORT dirIndexPrm
 * @param o:FW_SHORT* bufTblIdx
 * @return FW_INT32 error code
 * @attention
 */
static FW_INT32 dcf_db_search_db_buf(FW_SHORT dirIndexPrm, FW_SHORT* bufTblIdx)
{
	FW_SHORT dirIndexTmp2;
	FW_SHORT dbBufIdx;
	FW_SHORT dirTblIdxTmp;

	/* check parameter */
	dcf_if_assert(0 != dirIndexPrm);
	if(-1 == g_bf_dcf_db_current_db) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}

	dirIndexTmp2 = (0 < dirIndexPrm) ? dirIndexPrm : (g_bf_dcf_db_dir_ctrl_block.dirCnt + dirIndexPrm + 1);

	if(DCF_DB_CURRENT_DIR > dirIndexTmp2) {
		dbBufIdx = g_bf_dcf_db_current_db;
		/* dir index */
		if(g_bf_dcf_db_db_buf_tbl[dbBufIdx].objCnt != -1) {
			dirTblIdxTmp = g_bf_dcf_db_db_buf_tbl[dbBufIdx].dirTblIdx;
			if(g_bf_dcf_db_dir_tbl[dirTblIdxTmp].dirIndex == dirIndexTmp2) {
				// found
				/* re-index */
				*bufTblIdx = g_bf_dcf_db_current_db;
				return FSDCF_ERRNO_OK;
			}
		}
	}

	if(dirIndexPrm == DCF_DB_CURRENT_DIR){
		*bufTblIdx = g_bf_dcf_db_current_db;
	}
	else{
		return D_BF_MDF_WRAPPER_ERR_INVAL;
	}

	return FSDCF_ERRNO_OK;
}


/**
 * @brief search directory information on DataBase buffer by directory index
 * @param i:FW_SHORT dirIndexPrm
 * @param o:FW_SHORT* dirTblIdx
 * @return FW_INT32 error code
 * @attention
 */
static FW_INT32 dcf_db_search_dir_info_on_db_buf(FW_SHORT dirIndexPrm, FW_SHORT* dirTblIdx)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx;

	/* check parameter */
	dcf_if_assert(0 != dirIndexPrm);

	ercd = dcf_db_search_db_buf(dirIndexPrm, &bufTblIdx);

	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	*dirTblIdx = g_bf_dcf_db_db_buf_tbl[bufTblIdx].dirTblIdx;

	return FSDCF_ERRNO_OK;
}


/**
 * @brief search directory information outside DataBase buffer by directory index
 * @param i:FW_SHORT dirIndexPrm
 * @param o:FW_SHORT* dirTblIdx
 * @return FW_INT32 error code
 * @attention
 */
static FW_INT32 dcf_db_search_dir_info_out_side_db_buf(FW_SHORT dirIndexPrm, FW_SHORT* dirTblIdx)
{
	FW_INT32 ercd;
	FW_SHORT countTmp;
	FW_SHORT dirTblIdxTmp;

	/* check parameter */
	dcf_if_assert(0 != dirIndexPrm);

	if(0 < dirIndexPrm) {
		countTmp = dirIndexPrm;
	}
	else {
		countTmp = dirIndexPrm + g_bf_dcf_db_dir_ctrl_block.dirCnt + 1;
	}

	ercd = dcf_db_search_dir_tbl(countTmp, &dirTblIdxTmp);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	*dirTblIdx = dirTblIdxTmp;

	return FSDCF_ERRNO_OK;
}


/**
 * @brief search directory information by directory index
 * @param i:FW_SHORT dirIndexPrm
 * @param o:FW_SHORT* dirTblIdx
 * @return FW_INT32 error code
 * @attention
 */
static FW_INT32 dcf_db_search_dir_info(FW_SHORT dirIndexPrm, FW_SHORT* dirTblIdx)
{
	/* check parameter */
	dcf_if_assert(0 != dirIndexPrm);

	if(DCF_DB_CURRENT_DIR == dirIndexPrm) {
		return dcf_db_search_dir_info_on_db_buf(dirIndexPrm, dirTblIdx);
	}
	return dcf_db_search_dir_info_out_side_db_buf(dirIndexPrm, dirTblIdx);
}


/**
 * @brief search object information by both directory DataBase and object index
 * @param i:FW_SHORT bufTblIdx
 * @param i:FW_SHORT objIndexPrm
 * @param o:FW_SHORT* objTblIdx
 * @return FW_INT32 error code
 * @attention
 */
static FW_INT32 dcf_db_search_obj_info_by_db(FW_SHORT bufTblIdx, FW_SHORT objIndexPrm, FW_SHORT* objTblIdx)
{
	FW_INT32 ercd;
	FW_SHORT countTmp;
	FW_SHORT objTblIdxTmp;

	/* check parameter */
	dcf_if_assert(0 != objIndexPrm);

	if(0 == g_bf_dcf_db_db_buf_tbl[bufTblIdx].objCnt) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}
	if(0 == objIndexPrm) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}
	if(abs(objIndexPrm) > g_bf_dcf_db_db_buf_tbl[bufTblIdx].objCnt) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}

	if(0 < objIndexPrm) {
		countTmp = objIndexPrm;
	}
	else {
		countTmp = objIndexPrm + g_bf_dcf_db_db_buf_tbl[bufTblIdx].objCnt + 1;
	}

	ercd = dcf_db_search_obj_tbl(bufTblIdx, countTmp, &objTblIdxTmp);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	*objTblIdx = objTblIdxTmp;

	return FSDCF_ERRNO_OK;
}

/**
 * @brief search directory table index by directory index.
 * @param i:FW_SHORT dirIndex
 * @param o:FW_SHORT* dirTblIdx
 * @return error code
 * @attention
 */
static FW_INT32 dcf_db_search_dir_tbl(FW_SHORT dirIndex, FW_SHORT* dirTblIdx)
{
	FW_SHORT dirTblIdxMin;
	FW_SHORT dirTblIdxMax;
	FW_SHORT dirTblIdxMdl;
	FW_SHORT dirIndexTmp;

	dirTblIdxMax = g_bf_dcf_db_dir_ctrl_block.dirTblIdxLast;
	if(g_bf_dcf_db_dir_ctrl_block.dirCnt == dirIndex) {
		*dirTblIdx = dirTblIdxMax;
		return FSDCF_ERRNO_OK;
	}

	dirTblIdxMin = g_bf_dcf_db_dir_ctrl_block.dirTblIdxTop;
	if(1 == dirIndex) {
		*dirTblIdx = dirTblIdxMin;
		return FSDCF_ERRNO_OK;
	}

	while(dirTblIdxMin <= dirTblIdxMax) {
		dirTblIdxMdl = (dirTblIdxMin + dirTblIdxMax) >> 1;
		dirIndexTmp = g_bf_dcf_db_dir_tbl[dirTblIdxMdl].dirIndex;
		if(dirIndexTmp > dirIndex) {
			dirTblIdxMax = dirTblIdxMdl - 1;
		}
		else if(dirIndexTmp < dirIndex) {
			dirTblIdxMin = dirTblIdxMdl + 1;
		}
		else if(dirIndexTmp == dirIndex) {
			if(g_bf_dcf_db_dir_tbl[dirTblIdxMdl].dirName[0]) {
				*dirTblIdx = dirTblIdxMdl;
				return FSDCF_ERRNO_OK;
			}
			dirTblIdxMax = dirTblIdxMdl;
		}
		else {
			; // DO NOTHING
		}
	}
	return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
}

/**
 * @brief search object table index by object index.
 * @param i:FW_SHORT bufTblIdx
 * @param i:FW_SHORT objIndex
 * @param o:FW_SHORT* objTblIdx
 * @return error code
 * @attention
 */
static FW_INT32 dcf_db_search_obj_tbl(FW_SHORT bufTblIdx, FW_SHORT objIndex, FW_SHORT* objTblIdx)
{
	FW_SHORT objTblIdxMin;
	FW_SHORT objTblIdxMax;
	FW_SHORT objTblIdxMdl;
	FW_SHORT objIndexTmp;

	objTblIdxMax = g_bf_dcf_db_db_buf_tbl[bufTblIdx].objTblIdxLast;
	if(g_bf_dcf_db_db_buf_tbl[bufTblIdx].objCnt == objIndex) {
		*objTblIdx = objTblIdxMax;
		return FSDCF_ERRNO_OK;
	}

	objTblIdxMin = g_bf_dcf_db_db_buf_tbl[bufTblIdx].objTblIdxTop;
	if(1 == objIndex) {
		*objTblIdx = objTblIdxMin;
		return FSDCF_ERRNO_OK;
	}

	while(objTblIdxMin <= objTblIdxMax) {
		objTblIdxMdl = (objTblIdxMin + objTblIdxMax) >> 1;
		objIndexTmp = g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdxMdl].objIndex;
		if(objIndexTmp > objIndex) {
			objTblIdxMax = objTblIdxMdl - 1;
		}
		else if(objIndexTmp < objIndex) {
			objTblIdxMin = objTblIdxMdl + 1;
		}
		else if(objIndexTmp == objIndex) {
			if(g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdxMdl].objName[0]) {
				*objTblIdx = objTblIdxMdl;
				return FSDCF_ERRNO_OK;
			}
			objTblIdxMax = objTblIdxMdl;
		}
		else {
			; // DO NOTHING
		}
	}
	return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
}


/**
 * @brief Insert file to DCF-DB
 * @param  I : FW_CHAR* file_path
 *         I : FW_CHAR* full_file_path
 *         I : FW_SHORT bufTblIdx
 *         I : T_BF_FS_IF_Dir_Info* pdir
 *         I : FW_CHAR* objNameOnly
 * @return FW_INT32 error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_db_insert_file(
	FW_CHAR* file_path,
	FW_CHAR* full_file_path,
	FW_SHORT bufTblIdx,
	T_BF_FS_IF_Dir_Info* pdir,
	FW_CHAR* objNameOnly)
{
	FW_INT32 ercd;
	FW_INT32 result;
	FW_UCHAR extTypeTmp=0;
	BOOL notAllowedExtTypeFlag;
	FW_SHORT objTblIdx;
	FW_UCHAR obj_attr;

	if(full_file_path) {
		/* case create new file */
		/* file attribute is D_BF_MDF_WRAPPER_ATTR_NORMAL */
	}
	else {
		if ((pdir->attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
			return FSDCF_ERRNO_NOT_SUPPORT_FILE;
		}
	}
	if (strlen(file_path) < 12) {
		extTypeTmp = 0;
	}
	else {
		ercd = BF_Dcf_Db_Get_Type_By_Ext(file_path + 9, &extTypeTmp);
		if(BF_Dcf_Attr_Get_Attr_Dupli_Ope() == 1){
			if(ercd == FSDCF_ERRNO_NOT_SUPPORT_FILE){
				objTblIdx = dcf_db_get_file_no(file_path);
				if(objTblIdx == FSDCF_ERRNO_NOT_SUPPORT_FILENO) {
					return objTblIdx;
				}

				/* If it is file number duplicate, object information is cleared from DCF-DB */
				if(g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName[0] != 0) {
					g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName[0] = 0;
					g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType = 0;
					g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].attr = 0;
					return ercd;
				}
			}
		}
		if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Dcf_Db_Get_Type_By_Ext() failed: ercd=%d\n", ercd));
#endif
			return ercd;
		}
	}
	notAllowedExtTypeFlag = (0 == extTypeTmp);
#if 1
	if(notAllowedExtTypeFlag) {
		return FSDCF_ERRNO_OK;
	}                                                   // not allow
#endif

	objTblIdx = dcf_db_get_file_no(file_path);
	if(objTblIdx == FSDCF_ERRNO_NOT_SUPPORT_FILENO) {
		return objTblIdx;
	}

	if(g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName[0] != 0) {
		// hit file_no

		/* check voice memo file */
		if(strncmp(objNameOnly, g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName, 4) == 0) {
			if(BF_Dcf_Controller_Is_File_Type("JPG", (FW_LONG)g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType)){
				if (BF_Dcf_Db_Is_Ext_Wav(file_path + 9)){
					/* option file status update */
					g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag |= 0x04;
				}
				else if (BF_Dcf_Db_Is_Ext_Raw(file_path + 9)){
					/* annex file status update */
					g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag |= 0x08;
				}
			}
			else if(BF_Dcf_Controller_Is_File_Type("MP4", (FW_LONG)g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType)){
				if (BF_Dcf_Db_Is_Ext_Thm(file_path + 9)){
					/* annex file status update */
					g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag |= 0x08;
				}
			}
			else if(BF_Dcf_Controller_Is_File_Type("MOV", (FW_LONG)g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType)){
				if (BF_Dcf_Db_Is_Ext_Thm(file_path + 9)){
					/* annex file status update */
					g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag |= 0x08;
				}
			}
			else if(BF_Dcf_Controller_Is_File_Type("WAV", (FW_LONG)g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType)){
				if (BF_Dcf_Db_Is_Ext_Jpg(file_path + 9)){
					/* option file status update */
					g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag |= 0x04;
				}
			}
			else if(BF_Dcf_Controller_Is_File_Type("RAW", (FW_LONG)g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType)){
				if (BF_Dcf_Db_Is_Ext_Jpg(file_path + 9)){
					/* option file status update */
					g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag |= 0x08;
				}
			}
			else if(BF_Dcf_Controller_Is_File_Type("THM", (FW_LONG)g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType)){
				if (BF_Dcf_Db_Is_Ext_Mp4(file_path + 9)){
					/* option file status update */
					g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag |= 0x08;
				}
				if (BF_Dcf_Db_Is_Ext_Mov(file_path + 9)){
					/* option file status update */
					g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag |= 0x08;
				}
			}
			else {
				; // DO NOTHING
			}
		}

		if(BF_Dcf_Attr_Get_Attr_Dupli_Ope() == 1){
			if((g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag & (0x04 | 0x08)) == 0){
				g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName[0] = 0;
				g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType = 0;
				g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag |= 0x80;
				g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].attr = 0;

				return FSDCF_ERRNO_OK;
			}
		}

		/* file number duplicate status update */
		g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag |= 0x80;

		result = strncmp(objNameOnly, g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName, 4);
		if(0 == result) {
			// just!
			if(dcf_db_is_priority_ext(extTypeTmp, g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType)) {
				// need to update
				goto skip2;
			}
			// no need to update
			goto skip1;
		}
		if(BF_Dcf_Controller_Is_File_Type("JPG", (FW_LONG)g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType)) {
			if(0 == strncmp((FW_CHAR*)g_bf_dcf_db_file_name_still, g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName, 4)) {
				return FSDCF_ERRNO_OK;
			}
			if(!((strcmp("JPG", file_path + 9) == 0) || (strcmp("jpg", file_path + 9) == 0))) {
				return FSDCF_ERRNO_OK;
			}
		}
		if((strcmp("JPG", file_path + 9) == 0) || (strcmp("jpg", file_path + 9) == 0)) {
			if(0 == strncmp((FW_CHAR*)g_bf_dcf_db_file_name_still, objNameOnly, 4)) {
				goto skip3;
			}
			if(!(BF_Dcf_Controller_Is_File_Type("JPG", (FW_LONG)g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType))) {
				goto skip3;
			}
			return FSDCF_ERRNO_OK;
		}
		if(0 == strncmp((FW_CHAR*)g_bf_dcf_db_file_name_still, g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName, 4)) {
			return FSDCF_ERRNO_OK;
		}
		if(0 == strncmp((FW_CHAR*)g_bf_dcf_db_file_name_still, objNameOnly, 4)) {
			goto skip3;
		}
		return FSDCF_ERRNO_OK;
	}

	if(BF_Dcf_Attr_Get_Attr_Dupli_Ope() == 1){
		if(((g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag & 0x80) == 0x80) || ((g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag & 0x02) == 0x02)){
			return FSDCF_ERRNO_OK;
		}
	}

skip3:
	strncpy(g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName, objNameOnly, 4);
	g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName[4] = '\0';
	g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType = 0;

skip2:
	/* update to object information(1-8bit:other readonly or normal) */
	if(full_file_path) {
		/* case create new file */
		g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].attr = D_BF_MDF_WRAPPER_ATTR_NORMAL;
	}
	else {
		obj_attr = (g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].attr & 0x01);
		g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].attr = (pdir->attributes | obj_attr);
	}

skip1:
	g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType |= extTypeTmp;

	return FSDCF_ERRNO_OK;
}

/**
 * @brief Update file flag
 * @param  I : FW_CHAR* file_path
 *         I : FW_CHAR* full_file_path
 *         I : FW_SHORT bufTblIdx
 *         I : T_BF_FS_IF_Dir_Info* pdir
 * @return FW_INT32 error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_db_set_file_flag(FW_CHAR* file_path,	FW_CHAR* full_file_path, FW_SHORT bufTblIdx, T_BF_FS_IF_Dir_Info* pdir)
{
	FW_UCHAR extTypeTmp=0;
	FW_SHORT objTblIdx;
	FW_UCHAR obj_attr;

	if (strlen(file_path) >= 12) {
		BF_Dcf_Db_Get_Type_By_Ext(file_path + 9, &extTypeTmp);
	}

	objTblIdx = BF_Dcf_Db_Get_Obj_No(file_path);
	if(objTblIdx == FSDCF_ERRNO_NOT_SUPPORT_FILENO) {
		return objTblIdx;
	}

	if (extTypeTmp != 0) {
		/* allow file */
		g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag |= 0x01;
	}
	else {
		/* not allow file */
		g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag |= 0x02;
	}
	/* update to object information(0bit:readonly or normal) */
	if(full_file_path) {
		/* case create new file */
		/* file attribute to [dcf_db_insert_file] set in */
	}
	else {
		obj_attr = (pdir->attributes & 0x01);
		g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].attr |= obj_attr;
	}

	return FSDCF_ERRNO_OK;
}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---

/**
 * @brief set Data Buffer
 * @param i:FW_SHORT dirIndexPrm
 * @param i:FW_SHORT dirTblIdx
 * @param i:FW_INT32 skipFlag
 * @param i:FW_SHORT bufTblIdx
 * @return FW_INT32 error code
 * @attention create object information, too.
 */
static FW_INT32 dcf_db_set_db_buf(FW_SHORT dirIndexPrm, FW_SHORT dirTblIdx, FW_INT32 skipFlag, FW_SHORT bufTblIdx)
{
	FW_INT32 ercd;
	FW_CHAR dirPath[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR objNameOnly[D_BF_DCF_IF_OBJ_ONLY_NAME_MAX];
	FW_CHAR dirNameTmp[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_INT32 dNo;
	T_BF_FS_IF_Dir_Info dir;
	FW_SHORT objTblIdx;

	/* check parameter */
	dcf_if_assert(0 != dirIndexPrm);

	g_bf_dcf_db_db_buf_tbl[bufTblIdx].dirTblIdx = dirTblIdx;

	/* make "100XXXXX" */
	ercd = BF_Dcf_Db_Make_Dir_Path(dirNameTmp, (FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, dirTblIdx + D_BF_DCF_DB_DIR_START_NUM);
	if( ercd != FSDCF_ERRNO_OK ) {
		return ercd;
	}

	/* open directory including objects */
	strcpy(dirPath, (FW_CHAR*)g_bf_dcf_db_dir_ctrl_block.topDirName);
	strcat(dirPath, dirNameTmp);

	/* clear lObjBufTemp table */
	for(objTblIdx = D_BF_DCF_DB_OBJ_BUF_MAX - 1; objTblIdx >= 0; objTblIdx--) {
		g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName[0] = 0;
		g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag = 0x00;
		g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].attr = 0x00;
	}

	ercd = BF_Mdf_Wrapper_Opendir(dirPath, &dNo);
	if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, dirPath));
#endif
		BF_Mdf_Wrapper_Closedir(dNo);
		return ercd;
	}

	for(;;) {
		ercd = BF_Mdf_Wrapper_Readdir(dNo, &dir);
		if(0 != ercd) {
			if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
				// end of object
				break;
			}
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Closedir(dNo);
			return ercd;
		}

		if(!BF_Dcf_Db_Is_File_Name(dir.fname)) {
			continue;
		}
		memcpy(objNameOnly, dir.fname, 8);
		objNameOnly[8] = '\0';

		ercd = dcf_db_set_file_flag(dir.fname, NULL, bufTblIdx, &dir);
		if(FSDCF_ERRNO_OK != ercd) {
			continue;
		}

		ercd = dcf_db_insert_file(dir.fname, NULL, bufTblIdx, &dir, objNameOnly);
		if(FSDCF_ERRNO_OK != ercd) {
			continue;
		}
	}

	ercd = BF_Mdf_Wrapper_Closedir(dNo);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
	}
	/* update obj table */
	dcf_db_set_obj_db_buf_tbl(bufTblIdx);

	return ercd;
}

#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---

/**
 * @brief set Data Buffer
 * @param i:FW_SHORT dirIndexPrm
 * @param i:FW_SHORT dirTblIdx
 * @param i:FW_INT32 skipFlag
 * @param i:FW_SHORT bufTblIdx
 * @return FW_INT32 error code
 * @attention create object information, too.
 */
static FW_INT32 dcf_db_set_db_buf(FW_SHORT dirIndexPrm, FW_SHORT dirTblIdx, FW_INT32 skipFlag, FW_SHORT bufTblIdx)
{
	FW_INT32 ercd;
	FW_CHAR dirPath[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR objNameOnly[D_BF_DCF_IF_OBJ_ONLY_NAME_MAX];
	FW_CHAR dirNameTmp[D_BF_DCF_IF_DIR_NAME_MAX];
	T_BF_FS_IF_Dir_Info dir;
	FW_SHORT objTblIdx;
	BOOL first_dir_flag = TRUE;	
	FW_CHAR file_path[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	T_BF_FS_IF_ENTRY dirent = {0};
	FW_UCHAR attr;

	/* check parameter */
	dcf_if_assert(0 != dirIndexPrm);

	g_bf_dcf_db_db_buf_tbl[bufTblIdx].dirTblIdx = dirTblIdx;

	/* make "100XXXXX" */
	ercd = BF_Dcf_Db_Make_Dir_Path(dirNameTmp, (FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, dirTblIdx + D_BF_DCF_DB_DIR_START_NUM);
	if( ercd != FSDCF_ERRNO_OK ) {
		return ercd;
	}

	/* open directory including objects */
	strcpy(dirPath, (FW_CHAR*)g_bf_dcf_db_dir_ctrl_block.topDirName);
	strcat(dirPath, dirNameTmp);

	/* clear lObjBufTemp table */
	for(objTblIdx = D_BF_DCF_DB_OBJ_BUF_MAX - 1; objTblIdx >= 0; objTblIdx--) {
		g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName[0] = 0;
		g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag = 0x00;
		g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].attr = 0x00;
	}

	ercd = BF_Mdf_Wrapper_Get_Attribute(dirPath, &attr);
	if(ercd != FSDCF_ERRNO_OK){
		ercd = BF_Mdf_Wrapper_Mkdir(dirPath, 0x01ff);
		if (ercd != FSDCF_ERRNO_OK) {
			return ercd;
		}
	}

	ercd = BF_Mdf_Wrapper_Chdir(dirPath);
	if (ercd != FSDCF_ERRNO_OK) {
		return ercd;
	}

	for(;;) {
		if (first_dir_flag) {
			// search first
			strcpy(file_path, "*");
			ercd = BF_Fs_If_Fsfirst(0, (const CHAR*)file_path, D_BF_FS_IF_ATTR_ENTRY, &dirent);
		}
		else {
			// search next
			ercd = BF_Fs_If_Fsnext(0, &dirent);
		}
		
		if (ercd != FSDCF_ERRNO_OK) {
			if (ercd == D_BF_MDF_WRAPPER_ENT_LAST) {
				// search end
				ercd = FSDCF_ERRNO_OK;
				break;
			}
			
			return ercd;
		}

		if (first_dir_flag) {
			first_dir_flag = FALSE;
		}

		if(!BF_Dcf_Db_Is_File_Name(dirent.fs_fname)) {
			continue;
		}
		memcpy(objNameOnly, dirent.fs_fname, 8);
		objNameOnly[8] = '\0';

		strcpy(dir.fname, dirent.fs_fname);
		dir.attributes = dirent.fs_attributes;
		dir.fileSize = dirent.fs_filesize;

		ercd = dcf_db_set_file_flag(dirent.fs_fname, NULL, bufTblIdx, &dir);
		if(FSDCF_ERRNO_OK != ercd) {
			continue;
		}

		ercd = dcf_db_insert_file(dirent.fs_fname, NULL, bufTblIdx, &dir, objNameOnly);
		if(FSDCF_ERRNO_OK != ercd) {
			continue;
		}
	}

	/* update obj table */
	dcf_db_set_obj_db_buf_tbl(bufTblIdx);

	return ercd;
}

#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---


/**
 * @brief set Data Buffer
 * @param i:FW_SHORT bufTblIdx
 * @return FW_VOID
 * @attention create object information, too.
 */
static FW_VOID dcf_db_set_obj_db_buf_tbl(FW_SHORT bufTblIdx)
{
	FW_SHORT objTblIdx;
	FW_SHORT objIndex;

	g_bf_dcf_db_db_buf_tbl[bufTblIdx].objTblIdxTop = g_bf_dcf_db_db_buf_tbl[bufTblIdx].objTblIdxLast = -1;
	objIndex = 0;
	g_bf_dcf_db_db_buf_tbl[bufTblIdx].objCnt = 0;
	g_bf_dcf_db_db_buf_tbl[bufTblIdx].lastFileNum = 0;
	for(objTblIdx = 0; objTblIdx < D_BF_DCF_DB_OBJ_BUF_MAX; objTblIdx++) {
		if(g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName[0] != 0) {
			/* update to object information */
			g_bf_dcf_db_db_buf_tbl[bufTblIdx].objCnt++;
			if(-1 == g_bf_dcf_db_db_buf_tbl[bufTblIdx].objTblIdxTop) {
				g_bf_dcf_db_db_buf_tbl[bufTblIdx].objTblIdxTop = objTblIdx;
			}
			g_bf_dcf_db_db_buf_tbl[bufTblIdx].objTblIdxLast = objTblIdx;
			objIndex++;
		}
		if((g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag & 0x03) != 0) {
			/* update last file number */
			g_bf_dcf_db_db_buf_tbl[bufTblIdx].lastFileNum = objTblIdx;
		}
		g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objIndex = objIndex;
	}
	return;
}

/**
 * @brief set Data Buffer
 * @param i:FW_SHORT bufTblIdx
 * @return FW_VOID
 * @attention create object information, too.
 */
static FW_VOID dcf_db_set_dir_db_buf_tbl(FW_VOID)
{
	FW_SHORT dirIndex;
	FW_SHORT dirTblIdx;

	g_bf_dcf_db_dir_ctrl_block.dirTblIdxTop = g_bf_dcf_db_dir_ctrl_block.dirTblIdxLast = -1;
	dirIndex = 0;
	g_bf_dcf_db_dir_ctrl_block.dirCnt = 0;
	for(dirTblIdx = 0; dirTblIdx < D_BF_DCF_DB_DIR_BUF_MAX; dirTblIdx++) {
		if(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0] != 0) {
			/* update to object information */
			g_bf_dcf_db_dir_ctrl_block.dirCnt++;
			if(-1 == g_bf_dcf_db_dir_ctrl_block.dirTblIdxTop) {
				g_bf_dcf_db_dir_ctrl_block.dirTblIdxTop = dirTblIdx;
			}
			g_bf_dcf_db_dir_ctrl_block.dirTblIdxLast = dirTblIdx;
			dirIndex++;
		}
		g_bf_dcf_db_dir_tbl[dirTblIdx].dirIndex = dirIndex;
	}
	return;
}

/**
 * @brief Insert directory to DCF-DB
 * @param  I : const FW_CHAR* dir_fullpass_name
 * @param  O : FW_SHORT* p_dir_index
 * @return FW_INT32 error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_db_insert_dir(const FW_CHAR* dir_fullpass_name, FW_SHORT* p_dir_index)
{
	FW_INT32 ercd;
	T_BF_FS_IF_STAT dirStat;
	FW_CHAR dirNameTmp[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_INT32 findState;

	if(NULL == dir_fullpass_name) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	ercd = BF_Mdf_Wrapper_Stat(dir_fullpass_name, &dirStat);
	if(0 != ercd) {
		return ercd;
	}

	ercd = BF_Dcf_Db_Insert_Dir(&dirStat, &findState);
	if(0 != ercd) {
		return ercd;
	}

	memcpy(dirNameTmp, dirStat.fname, 8);
	dirNameTmp[8] = '\0';

	ercd = BF_Dcf_Db_Get_Index_By_Dir((const FW_CHAR*)dirNameTmp, p_dir_index);
	if(0 != ercd) {
		return ercd;
	}

	if(0 != findState) {
		return FSDCF_ERRNO_OK;
	}

#if 1
	BF_Dcf_Db_Clear_Db_buf();
#endif
	return FSDCF_ERRNO_OK;
}

static FW_INT32 dcf_db_get_last_file_num( FW_SHORT dirTblIdx, FW_USHORT* filenum)
{
	FW_INT32 ercd;
	FW_CHAR dirPath[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR dirNameTmp[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_INT32 dNo;
	T_BF_FS_IF_Dir_Info dir;
	FW_USHORT filenumtmp;

	*filenum = 0;

	if(BF_Dcf_Attr_Get_Attr_Dupli_Ope() == 1){
		if(g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus != 0){
			return FSDCF_ERRNO_OK;
		}
	}

	/* make "100XXXXX" */
	ercd = BF_Dcf_Db_Make_Dir_Path(dirNameTmp, (FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, dirTblIdx + D_BF_DCF_DB_DIR_START_NUM);
	if( ercd != FSDCF_ERRNO_OK ) {
		return ercd;
	}

	/* open directory including objects */
	strcpy(dirPath, (FW_CHAR*)g_bf_dcf_db_dir_ctrl_block.topDirName);
	strcat(dirPath, dirNameTmp);

	ercd = BF_Mdf_Wrapper_Opendir(dirPath, &dNo);
	if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, dirPath));
#endif
		BF_Mdf_Wrapper_Closedir(dNo);
		return ercd;
	}

	for(;;) {
		ercd = BF_Mdf_Wrapper_Readdir(dNo, &dir);
		if(0 != ercd) {
			if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
				// end of object
				break;
			}
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Closedir(dNo);
			return ercd;
		}

		if('.' == dir.fname[0]) {
			if(0 == dir.fname[1]) {
				continue;
			}
			if(('.' == dir.fname[1]) && (0 == dir.fname[2])) {
				continue;
			}
		}

		if(!BF_Dcf_Db_Is_File_Name(dir.fname)) {
			continue;
		}
		if ((dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
			continue;
		}

		filenumtmp = dcf_db_get_file_no(dir.fname);

		if(*filenum < filenumtmp){
			*filenum = filenumtmp;
		}

	}

	ercd = BF_Mdf_Wrapper_Closedir(dNo);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
	}

	return ercd;
}


/**
 * @brief make directory structure in DCF_DB.
 * @param FW_VOID
 * @return error code
 */
static FW_INT32 dcf_db_get_select_dir(FW_USHORT* dirno, FW_USHORT* fileno, E_BF_DCF_DB_ASSIGN operation)
{
	FW_INT32 ercd;
	FW_SHORT dirnotmp = -1;
	FW_SHORT dirTblIdx;
	FW_CHAR dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR dirNameTmp[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_UCHAR attr;
	FW_INT32 dNo;
	T_BF_FS_IF_Dir_Info dir;

	*dirno = 0;
	*fileno = 0;

	if(operation == E_BF_DCF_DB_ASSIGN_LAST_KEY){
		/* get directory --------------------- */
		ercd = BF_Mdf_Wrapper_Opendir((FW_CHAR*)g_bf_dcf_db_dir_ctrl_block.topDirName, &dNo);
		if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
	//		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, g_bf_dcf_db_dir_ctrl_block.topDirName));
#endif
			BF_Mdf_Wrapper_Closedir(dNo);
			return ercd;
		}

		/*-----------------------------------------------------------------------*/
		for(;;) {
			/* read directory information */
			ercd = BF_Mdf_Wrapper_Readdir(dNo, &dir);
			if(0 != ercd) {
				if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
					// end of object
					break;
				}
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif
				BF_Mdf_Wrapper_Closedir(dNo);
				return ercd;
			}

			if('.' == dir.fname[0]) {
				if(0 == dir.fname[1]) {
					continue;
				}
				if(('.' == dir.fname[1]) && (0 == dir.fname[2])) {
					continue;
				}
			}

			if(strncmp(dir.fname + 3, (FW_CHAR*)g_bf_dcf_db_dir_name_dcf, 5) != 0) {
				continue;
			}

			dirTblIdx = BF_Dcf_Db_Get_Dir_No(dir.fname);
			if(dirTblIdx == FSDCF_ERRNO_NOT_SUPPORT_DIRNO) {
				continue;
			}
			if((dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) != D_BF_FS_IF_ATTR_DIRECTORY) {
				continue;
			}

			if(*dirno < dirTblIdx){
				*dirno = dirTblIdx;
			}

		}

		/*-----------------------------------------------------------------------*/
		ercd = BF_Mdf_Wrapper_Closedir(dNo);
		if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
		}

		if ( *dirno != 0){
			dcf_db_get_last_file_num(*dirno - D_BF_DCF_DB_DIR_START_NUM, fileno);
		}
		else {
			return 0;
		}

	}
	else if(operation == E_BF_DCF_DB_ASSIGN_DUPLICATE){

		for(dirTblIdx = 0; dirTblIdx < D_BF_DCF_DB_DIR_BUF_MAX; dirTblIdx++) {
			if(g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus == 0x01){

				/* make "100XXXXX" */
				ercd = BF_Dcf_Db_Make_Dir_Path(dirNameTmp, (FW_CHAR*)g_bf_dcf_db_dir_name_dcf, dirTblIdx + D_BF_DCF_DB_DIR_START_NUM);
				if(ercd != FSDCF_ERRNO_OK) {
					return ercd;
				}

				/* make directory path */
				strcpy(dir_name, (FW_CHAR*)g_bf_dcf_db_dir_ctrl_block.topDirName);
				strcat(dir_name, dirNameTmp);

				ercd = BF_Fs_Controller_Get_Attribute(dir_name, &attr);
				if(ercd == FSDCF_ERRNO_OK) {
					dirnotmp = dirTblIdx;
				}
				else if(ercd != D_BF_MDF_WRAPPER_ERR_NOENT) {
					return ercd;
				}
				else {
					; // DO NOTHING
				}
			}
		}

		if(dirnotmp == -1){
			return 0;
		}
		*dirno = dirnotmp + D_BF_DCF_DB_DIR_START_NUM;
	}
	else if(operation == E_BF_DCF_DB_ASSIGN_HIDDEN){

		/* get directory --------------------- */
		ercd = BF_Mdf_Wrapper_Opendir((FW_CHAR*)g_bf_dcf_db_dir_ctrl_block.topDirName, &dNo);
		if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
	//		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, g_bf_dcf_db_dir_ctrl_block.topDirName));
#endif
			BF_Mdf_Wrapper_Closedir(dNo);
			return ercd;
		}

		/*-----------------------------------------------------------------------*/
		for(;;) {
			/* read directory information */
			ercd = BF_Mdf_Wrapper_Readdir(dNo, &dir);
			if(0 != ercd) {
				if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
					// end of object
					break;
				}
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif
				BF_Mdf_Wrapper_Closedir(dNo);
				return ercd;
			}

			if('.' == dir.fname[0]) {
				if(0 == dir.fname[1]) {
					continue;
				}
				if(('.' == dir.fname[1]) && (0 == dir.fname[2])) {
					continue;
				}
			}

			if(strncmp(dir.fname + 3, (FW_CHAR*)g_bf_dcf_db_dir_name_dcf, 5) != 0) {
				continue;
			}

			dirTblIdx = BF_Dcf_Db_Get_Dir_No(dir.fname);
			if(dirTblIdx == FSDCF_ERRNO_NOT_SUPPORT_DIRNO) {
				continue;
			}
			if((dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) != D_BF_FS_IF_ATTR_DIRECTORY) {
				continue;
			}

			if((dir.attributes & ATTR_HIDDEN) == ATTR_HIDDEN){
				if(*dirno < dirTblIdx){
					*dirno = dirTblIdx;
				}
			}

		}

		/*-----------------------------------------------------------------------*/
		ercd = BF_Mdf_Wrapper_Closedir(dNo);
		if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
		}
	}
	else {
		; // DO NOTHING
	}

	return 0;
}

/**
 * @brief Get directory path from full file type.
 * @param i:*full_file_path full file path (I:\DCIM\100AAAAA)
 * @param o:*dir_path directory file path  (100AAAAA)
 * @return error code
 */
static FW_INT32 dcf_db_get_dir_path(FW_CHAR* full_file_path, FW_CHAR* dir_path)
{
	FW_INT32 name_cnt;
	FW_INT32 parh_len;
	FW_INT32 drive_name_len;
	FW_CHAR  chk_path[64];
	char *delim = ":";
	char *drvname;
	
	// DriveName Check
	strcpy(chk_path, full_file_path );
	drvname = strtok(chk_path, delim);
	if (drvname == NULL) {
		return FSDCF_ERRNO_PARAM_FAILED;
	}
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("dcf_db_get_dir_path  drvname =(%s)\n", drvname));
#endif

	drive_name_len = strlen(drvname);

	// Path length Check
	parh_len = strlen(full_file_path);

	if ( parh_len < ( drive_name_len + 15) ){
		// Err
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("Debug Check Path : path=%s(length=%d)\n", full_file_path, parh_len));
#endif
	}

	// NULL check
	for (name_cnt = 0; name_cnt < ( drive_name_len + 15) ; name_cnt++) {
		if(full_file_path[name_cnt] == 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("DIR PATH ERROR: path=%s\n", full_file_path));
#endif
			return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
		}
	}
	memcpy(dir_path, full_file_path + (drive_name_len+7), 8);	// [drive_name]:\DCIM\ 100AAAAA //
	dir_path[8] = 0;

	return FSDCF_ERRNO_OK;
}


#if 0
/**
 * @brief get Dir Index
 * @param o:FW_SHORT* dirTblIdx
 * @return FW_VOID
 * @attention create object information, too.
 */
static FW_VOID dcf_db_get_dir_tbl_idx(FW_SHORT* dirTblIdx)
{
	FW_SHORT bufTblIdx;

	switch(g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		case 2:
			bufTblIdx = (g_bf_dcf_db_db_buf_tbl[dcf_db_get_next_buf_num].objCnt != -1) ? dcf_db_get_next_buf_num : dcf_db_get_prev_buf_num;
			*dirTblIdx = g_bf_dcf_db_db_buf_tbl[bufTblIdx].dirTblIdx;
			break;

		default:
			*dirTblIdx = g_bf_dcf_db_db_buf_tbl[g_bf_dcf_db_current_db].dirTblIdx;
			break;
	}
	return;
}
#endif

/**
 * @brief Get DCF root directory length
 * @param i:*full_file_path full file path
 * @param i:*length DCF root directory length
 * @return error code
 */
static FW_INT32 dcf_db_get_dcf_root_dir_len(FW_CHAR* full_file_path, FW_USHORT* length)
{
	FW_CHAR* full_file_path_ptr;

	// Search DCF root directory name
	full_file_path_ptr = strstr(full_file_path, D_BF_DCF_IF_ROOT_DIR_NAME);

	if(full_file_path_ptr == NULL ) {
		return FSDCF_ERRNO_PARAM_FAILED;
	}

	// Get DCF root directory length
	*length = strlen((FW_CHAR*)g_bf_dcf_db_dir_ctrl_block.topDirName);

	return FSDCF_ERRNO_OK;
}



/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 * @brief Get directory number.
 * @param i:*fname DCF directory name
 * @return 100..999:DCF directory number
 * @rerurn -102:not support directory number
 * @attention only directory name.\n
 *            can't treat absolute path.
 */
FW_SHORT BF_Dcf_Db_Get_Dir_No(FW_CHAR* fname)
{
	FW_INT32 name_cnt;
	FW_INT32 dirno;

	if(fname == NULL) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	// length check
	if (strlen(fname) < 8){
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	// check diretory name format(name_cnt=0)
	if ((fname[0] < '1') || ('9' < fname[0])) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	// charactor check
	for (name_cnt = 1; name_cnt < 3; name_cnt++) {
		if(!isdigit((FW_INT32)(fname[name_cnt]))){
			return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
		}
	}
	for (name_cnt = 3; name_cnt < 8; name_cnt++) {
		if ((!isdigit((FW_INT32)(fname[name_cnt]))) &&
			(!isalpha((FW_INT32)(fname[name_cnt]))) &&
			(fname[name_cnt] != '_')) {
			return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
		}
	}

	// from charactor to number
	dirno = (fname[0] - '0') * 100 + (fname[1] - '0') * 10 + (fname[2] - '0');

	// check file number
	if ((dirno < FJ_CUSTOM_DCFDB_DIRNUM_MIN) || (dirno > FJ_CUSTOM_DCFDB_DIRNUM_MAX)) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	return dirno;
}



/**
 * @brief Get object number.
 * @param i:*fname DCF object name
 * @return 1..9999:DCF file number
 * @rerurn -103:not support file number
 */
FW_SHORT BF_Dcf_Db_Get_Obj_No(FW_CHAR* fname)
{
	FW_INT32 name_cnt;
	FW_SHORT objno;

	if(fname == NULL) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}

	// length check
	if (strlen(fname) < 8){
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}

	// charactor check
	for (name_cnt = 0; name_cnt < 4; name_cnt++) {
		if ((!isdigit((FW_INT32)(fname[name_cnt]))) &&
			(!isalpha((FW_INT32)(fname[name_cnt]))) &&
			(fname[name_cnt] != '_')) {
			return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
		}
	}
	for (name_cnt = 4; name_cnt < 8; name_cnt++) {
		if(!isdigit((FW_INT32)(fname[name_cnt]))){
			return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
		}
	}

	// from charactor to number
	objno = (fname[4] - '0') * 1000 + (fname[5] - '0') * 100
			+ (fname[6] - '0') * 10 + (fname[7] - '0');

	// check file number
	if ((objno == FJ_CUSTOM_DCFDB_FILENUM_MIN) || (objno > FJ_CUSTOM_DCFDB_FILENUM_MAX)) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}

	return objno;
}



/**
 * @brief make full file path.
 * @param i:*file_index  file index
 * @param o:*full_file_path full file path
 * @return error code
 */
FW_INT32 BF_Dcf_Db_Make_Full_Path(T_BF_DCF_IF_FILE_INDEX file_index, FW_CHAR* full_file_path)
{
	FW_INT32 ercd;
	FW_CHAR obj_path[20];
	FW_CHAR ext[4];
	FW_UCHAR ext_type;

	strcpy(full_file_path, (FW_CHAR*)g_bf_dcf_db_dir_ctrl_block.topDirName);
	ercd = BF_Dcf_Db_Read_Dir_Obj_Name(file_index.pack.dir_index, file_index.pack.obj_index, obj_path);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	strcat(full_file_path, obj_path);

	ercd = BF_Dcf_Db_Read_Ext_Type(file_index.pack.dir_index, file_index.pack.obj_index, &ext_type);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = BF_Dcf_Db_Get_Ext_By_Type(ext_type, ext);
	if(FSDCF_ERRNO_OK == ercd) {
		// add extension if exist
		strcat(full_file_path, ".");
		strcat(full_file_path, ext);
	}

	return 0;
}




/**
 * @brief check directory name.
 * @param i:FW_CHAR* dirNamePrm
 * @return BOOL
 * @attention expect directory name "nnnXXXXX"
 */
BOOL BF_Dcf_Db_Is_Dir_Name(const FW_CHAR dirNamePrm[])
{
	FW_INT32 index;
	FW_INT32 num;

	/* check parameter */
	if(NULL == dirNamePrm) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	// length check
	if(8 > strlen(dirNamePrm)) {
		return FALSE;
	}

	// check first charactor
	if(('1' > dirNamePrm[0]) || ('9' < dirNamePrm[0])) {
		return FALSE;
	}

	// check charactor
	for(index = 1; index < 3; index++) {
		if(!isdigit((FW_INT32)(dirNamePrm[index]))) {
			return FALSE;
		}
	}
	for(; index < 8; index++) {
		if((!isdigit((FW_INT32)(dirNamePrm[index])))
		   && (!isalpha((FW_INT32)(dirNamePrm[index])))
		   && ('_' != dirNamePrm[index])) {
			return FALSE;
		}
	}

	// check directory number
	num = (dirNamePrm[0] - '0') * 100 + (dirNamePrm[1] - '0') * 10 + (dirNamePrm[2] - '0');
	if(100 > num) {
		return FALSE;
	}

	return TRUE;
}


/**
 * @brief check file name.
 * @param i:FW_CHAR* fileName
 * @return BOOL
 * @attention expect file name "YYYYnnnn.ZZZ"
 */
BOOL BF_Dcf_Db_Is_File_Name(const FW_CHAR fileName[])
{
	FW_INT32 index;

	/* check parameter */
	if(NULL == fileName) {
		return FALSE;
	}

	// length check
	//if(12 > strlen(fileName)) {return FALSE;}
	if(8 > strlen(fileName)) {
		return FALSE;
	}

	// check charactor
	for(index = 0; index < 4; index++) {
		if((!isdigit((FW_INT32)(fileName[index])))
		   && (!isalpha((FW_INT32)(fileName[index])))
		   && ('_' != fileName[index])) {
			return FALSE;
		}
	}
	for(; index < 8; index++) {
		if(!isdigit((FW_INT32)(fileName[index]))) {
			return FALSE;
		}
	}
	//if('.' != fileName[8]) {return FALSE;}

	return TRUE;
}



/**
 * @brief search object information by both directory index and object index
 * @param i:FW_SHORT dirIndexPrm
 * @param i:FW_SHORT objIndexPrm
 * @param o:T_BF_DCF_DB_OBJ_INFO** object information pointer
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Search_Obj_Info(FW_SHORT dirIndexPrm, FW_SHORT objIndexPrm, T_BF_DCF_DB_OBJ_INFO** ppObjInfo)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx;
	FW_SHORT objTblIdx;

	/* check parameter */
	if(((FW_INT32)NULL) == dirIndexPrm) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(((FW_INT32)NULL) == objIndexPrm) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}
	if(NULL == ppObjInfo) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	ercd = dcf_db_search_db_buf(dirIndexPrm, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_search_obj_info_by_db(bufTblIdx, objIndexPrm, &objTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	*ppObjInfo = &g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx];

	return FSDCF_ERRNO_OK;
}




FW_VOID BF_Dcf_Db_Clear_Db_buf(FW_VOID)
{
	FW_SHORT dbBufIdx;

	g_bf_dcf_db_current_db = -1;
	g_bf_dcf_db_cash1_db = -1;
	g_bf_dcf_db_cash2_db = -1;

	for(dbBufIdx = 0; dbBufIdx < D_BF_DCF_DB_BUF_NUM; dbBufIdx++) {
		g_bf_dcf_db_db_buf_tbl[dbBufIdx].objCnt = -1;
	}
}

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 Global Function
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/**
 * @brief file system databese clear.
 * @param FW_VOID
 * @return error code
 */
FW_VOID BF_Dcf_Db_Clear(FW_VOID)
{
	FW_SHORT dbBufIdx;

	// initialize DCF DB Directory CB
	g_bf_dcf_db_dir_ctrl_block.topDirName[0] = '\0';
	g_bf_dcf_db_dir_ctrl_block.dirCnt = 0;

	// initialize DCF DB Buffer
	g_bf_dcf_db_current_db = -1;
	g_bf_dcf_db_cash1_db = -1;
	g_bf_dcf_db_cash2_db = -1;

	// initialize DCF DB Object CB
	for(dbBufIdx = 0; dbBufIdx < D_BF_DCF_DB_BUF_NUM; dbBufIdx++) {
		g_bf_dcf_db_db_buf_tbl[dbBufIdx].objCnt = -1;
	}
}


/**
 * @brief get file info and file path of specified index file.
 * @param i:file_index file index
 * @param o:*full_file_path full file path
 * @param o:*file_info file information
 * @return error code
 * @note if full_file_path or file_info is NULL, these output is not get.
 */
FW_INT32 BF_Dcf_Db_Get_Index_File_Info(T_BF_DCF_IF_FILE_INDEX file_index, FW_CHAR* full_file_path, T_BF_DCF_IF_FILE_INFO* file_info)
{
	FW_INT32 ercd;

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(E_BF_MEASURE_TIME_ID_DCF_DB);
#endif

	ercd = dcf_db_get_index_file_info(file_index, full_file_path, file_info);
	if( ercd != 0 ) {
		return ercd;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(E_BF_MEASURE_TIME_ID_DCF_DB, "DCF DB GET FILE_INFO_END\n");
#endif

	return 0;
}


/**
 * @brief get file type of specified index file.
 * @param i:file_index file index
 * @param o:*file_type file type
 * @return error code
 * @note if file_type is NULL, these output is not get.
 */
FW_INT32 BF_Dcf_Db_Get_Index_File_Type(T_BF_DCF_IF_FILE_INDEX file_index, FW_UCHAR* file_type)
{
	FW_INT32 ercd;
	ercd = dcf_db_get_index_file_type(file_index, file_type);
	if( ercd != 0 ) {
		return ercd;
	}

	return 0;
}


#if 0
/**
 * @brief get duplicate dirno and fileno.
 * @param o:*dirno duplicate directory number
 * @param o:*fileno duplicate file number
 * @return FW_VOID
 */
FW_VOID BF_DcfDb_Get_DuplicateNo(FW_USHORT* dirno, FW_USHORT* fileno)
{
	*dirno = 0;
	*fileno = 0;
}
#endif


/*=============================================================================
; Directory Handling Functions
=============================================================================*/

#if 0 // used BF_Dcf_Db_Clear
/*-----------------------------------------------------------------------------
; Initialize Data Base
-----------------------------------------------------------------------------*/
/**
 * @brief initialize directory control block, DataBase buffer, object information
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Initialize(FW_VOID)
{
	FW_SHORT dbBufIdx;

	/*-----------------------------------------------------------------------*/
	/* make DCF DB structure */
	/* initialize DCF DB Directory CB */
	g_bf_dcf_db_dir_ctrl_block.topDirName[0] = '\0';
	g_bf_dcf_db_dir_ctrl_block.dirCnt = 0;

	/* initialize DCF DB Buffer */
	g_bf_dcf_db_current_db = -1;
	g_bf_dcf_db_cash1_db = -1;
	g_bf_dcf_db_cash2_db = -1;

	/* initialize DCF DB Object CB */
	for(dbBufIdx = 0; dbBufIdx < D_BF_DCF_DB_BUF_NUM; dbBufIdx++) {
		g_bf_dcf_db_db_buf_tbl[dbBufIdx].objCnt = -1;
	}

	return FSDCF_ERRNO_OK;
}
#endif

/**
 * @brief Insert directory to DCF-DB
 * @param  I : T_BF_FS_IF_STAT* pDirStat
 *         I : FW_INT32* pfindState
 * @return FW_INT32 error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Db_Insert_Dir(T_BF_FS_IF_STAT* pDirStat, FW_INT32* pfindState)
{
	FW_SHORT dirTblIdx;

	//if ((pDirStat->attributes & D_BF_FS_IF_ATTR_DIRECTORY) != D_BF_FS_IF_ATTR_DIRECTORY) {return FSDCF_ERRNO_NOT_DIR;}
	if(!BF_Dcf_Db_Is_Dir_Name(pDirStat->fname)) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	dirTblIdx = BF_Dcf_Db_Get_Dir_No(pDirStat->fname);
	if(dirTblIdx == FSDCF_ERRNO_NOT_SUPPORT_DIRNO ) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	if(g_bf_dcf_db_latest_dir_num < dirTblIdx){
		g_bf_dcf_db_latest_dir_num = dirTblIdx;
	}

	if((pDirStat->attributes & ATTR_HIDDEN) == ATTR_HIDDEN){
		if(g_bf_dcf_db_hidden_dir_num < dirTblIdx){
			g_bf_dcf_db_hidden_dir_num = dirTblIdx;
		}
	}

	dirTblIdx -= D_BF_DCF_DB_DIR_START_NUM;

	*pfindState = 0;

	if(BF_Dcf_Attr_Get_Attr_Dupli_Ope() == 0){
	// When the same number as the directory exists, it registers in DCF-DB.
		if(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0] != 0) {

			/* search directory information by dirNo */
			*pfindState = 2;
			/* dir duplicate status update */
			g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus = 1;

			if(g_bf_dcf_db_duplicate_dir_num < (dirTblIdx + D_BF_DCF_DB_DIR_START_NUM)){
				g_bf_dcf_db_duplicate_dir_num = dirTblIdx + D_BF_DCF_DB_DIR_START_NUM;
			}

			if(0 == strcmp((FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, (FW_CHAR*)g_bf_dcf_db_dir_name_dcf)) {
				return FSDCF_ERRNO_OK;
			}
			if(0 != strcmp(pDirStat->fname + 3, (FW_CHAR*)g_bf_dcf_db_dir_name_dcf)) {
				return FSDCF_ERRNO_OK;
			}
		}

		/* regist to new node */
		strncpy((FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, pDirStat->fname + 3, sizeof(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName) - 1);
		g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[sizeof(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName) - 1] = '\0';
		g_bf_dcf_db_dir_tbl[dirTblIdx].date = pDirStat->m_date;
		g_bf_dcf_db_dir_tbl[dirTblIdx].time1 = pDirStat->m_time;
		g_bf_dcf_db_dir_tbl[dirTblIdx].sec10m = pDirStat->m_sec10m;
	}
	else{
	// When the same number as the directory exists, it doesn't register in DCF-DB.
		if(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0] != 0) {
			g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0] = 0;
			g_bf_dcf_db_dir_tbl[dirTblIdx].date = 0;
			g_bf_dcf_db_dir_tbl[dirTblIdx].time1 = 0;
			g_bf_dcf_db_dir_tbl[dirTblIdx].sec10m = 0;
			g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus = 1;

			if(g_bf_dcf_db_duplicate_dir_num < (dirTblIdx + D_BF_DCF_DB_DIR_START_NUM)){
				g_bf_dcf_db_duplicate_dir_num = dirTblIdx + D_BF_DCF_DB_DIR_START_NUM;
			}

			/* dir buf table update */
			dcf_db_set_dir_db_buf_tbl();

			return FSDCF_ERRNO_OK;
		}
		else if(g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus != 0){
			*pfindState = 2;
			return FSDCF_ERRNO_OK;
		}
		else {
			; // DO NOTHING
		}
		/* regist to new node */
		strncpy((FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, pDirStat->fname + 3, sizeof(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName) - 1);
		g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[sizeof(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName) - 1] = '\0';
		g_bf_dcf_db_dir_tbl[dirTblIdx].date = pDirStat->m_date;
		g_bf_dcf_db_dir_tbl[dirTblIdx].time1 = pDirStat->m_time;
		g_bf_dcf_db_dir_tbl[dirTblIdx].sec10m = pDirStat->m_sec10m;
		g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus = 0;
	}

	// need to update
	if(0 == *pfindState) {
		// no-found. make new one.
		/* dir duplicate status update */
		g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus = 0;
		/* dir buf table update */
		dcf_db_set_dir_db_buf_tbl();
	}
	return FSDCF_ERRNO_OK;
}




/*-----------------------------------------------------------------------------
; Set top Directory
-----------------------------------------------------------------------------*/
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---

/**
 * @brief set top directory
 * @param i:const FW_CHAR* dir_fullpass_name
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Set_Dir(const FW_CHAR* dir_fullpass_name)
{
	FW_INT32 ercd;
	FW_INT32 dNo;
	T_BF_FS_IF_Dir_Info dir;
#if 0
	FS_STAT dirStat;
	FW_CHAR dirFullName[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR dirNameTmp[D_BF_DCF_IF_DIR_NAME_MAX];
#endif
	FW_SHORT dirTblIdx;

	/* check parameter */
	if(NULL == dir_fullpass_name) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(!dcf_db_is_top_dir_name(dir_fullpass_name)) {
		return FSDCF_ERRNO_NOT_DCIM;
	}

	BF_Dcf_Db_Clear_Db_buf();
	strcpy((FW_CHAR*)g_bf_dcf_db_dir_ctrl_block.topDirName, dir_fullpass_name);
	g_bf_dcf_db_dir_ctrl_block.dirCnt = 0;

	/* clear g_bf_dcf_db_dir_tbl table */
	for(dirTblIdx = 0; dirTblIdx < D_BF_DCF_DB_DIR_BUF_MAX; dirTblIdx++) {
		g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0] = 0;
		g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus = 0;
	}
	g_bf_dcf_db_file_num_as_dir_num = 0;
	g_bf_dcf_db_latest_dir_num = 0;
	g_bf_dcf_db_duplicate_dir_num = 0;
	g_bf_dcf_db_hidden_dir_num = 0;

	/*-----------------------------------------------------------------------*/
	/* get directory --------------------- */
	ercd = BF_Mdf_Wrapper_Opendir((FW_CHAR*)g_bf_dcf_db_dir_ctrl_block.topDirName, &dNo);
	if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
//		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, g_bf_dcf_db_dir_ctrl_block.topDirName));
#endif
		BF_Mdf_Wrapper_Closedir(dNo);
		return ercd;
	}

	/*-----------------------------------------------------------------------*/
	for(;;) {
		/* read directory information */
		ercd = BF_Mdf_Wrapper_Readdir(dNo, &dir);
		if(0 != ercd) {
			if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
				// end of object
				break;
			}
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Closedir(dNo);
			return ercd;
		}

		//if ((dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) != D_BF_FS_IF_ATTR_DIRECTORY) {continue;}
		dirTblIdx = BF_Dcf_Db_Get_Dir_No(dir.fname);
		if( dirTblIdx == FSDCF_ERRNO_NOT_SUPPORT_DIRNO ) {
			continue;
		}
		if ((dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) != D_BF_FS_IF_ATTR_DIRECTORY) {
			// set directory number of other directory
			if (g_bf_dcf_db_file_num_as_dir_num < dirTblIdx) {
				g_bf_dcf_db_file_num_as_dir_num = dirTblIdx;
			}
			continue;
		}

		if(g_bf_dcf_db_latest_dir_num < dirTblIdx){
			g_bf_dcf_db_latest_dir_num = dirTblIdx;
		}

		if((dir.attributes & ATTR_HIDDEN) == ATTR_HIDDEN){
			if(g_bf_dcf_db_hidden_dir_num < dirTblIdx){
				g_bf_dcf_db_hidden_dir_num = dirTblIdx;
			}
		}

		dirTblIdx -= D_BF_DCF_DB_DIR_START_NUM;

		if(BF_Dcf_Attr_Get_Attr_Dupli_Ope() == 0){
		// When the same number as the directory exists, it registers in DCF-DB.
			if(0 == g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0]) {
				g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus = 0;

			}
			else {
				// not new one
				g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus = 1;

				if(g_bf_dcf_db_duplicate_dir_num < (dirTblIdx + D_BF_DCF_DB_DIR_START_NUM)){
					g_bf_dcf_db_duplicate_dir_num = dirTblIdx + D_BF_DCF_DB_DIR_START_NUM;
				}

				if(0 == strcmp((FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, (FW_CHAR*)g_bf_dcf_db_dir_name_dcf)) {
					continue;
				}
				if(0 != strcmp(dir.fname + 3, (FW_CHAR*)g_bf_dcf_db_dir_name_dcf)) {
					continue;
				}
			}
		}
		else{
		// When the same number as the directory exists, it doesn't register in DCF-DB.
			if(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0] != 0){

				g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0] = 0;
				g_bf_dcf_db_dir_tbl[dirTblIdx].date = 0;
				g_bf_dcf_db_dir_tbl[dirTblIdx].time1 = 0;
				g_bf_dcf_db_dir_tbl[dirTblIdx].sec10m = 0;
				g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus = 1;

				if(g_bf_dcf_db_duplicate_dir_num < (dirTblIdx + D_BF_DCF_DB_DIR_START_NUM)){
					g_bf_dcf_db_duplicate_dir_num = dirTblIdx + D_BF_DCF_DB_DIR_START_NUM;
				}

				continue;

			}
			else if(g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus != 0){
				continue;
			}
			else {
				; // DO NOTHING
			}

		}
		strncpy((FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, dir.fname + 3, sizeof(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName) - 1);
		g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[sizeof(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName) - 1] = '\0';

#if 0
		/* make "100XXXXX" */
		ercd = BF_Dcf_Db_Make_Dir_Path(dirNameTmp, g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, dirTblIdx + D_BF_DCF_DB_DIR_START_NUM);
		if( ercd != FSDCF_ERRNO_OK ) {
			return ercd;
		}

		strcpy(dirFullName, g_bf_dcf_db_dir_ctrl_block.topDirName);
		strcat(dirFullName, dirNameTmp);
		ercd = BF_Mdf_Wrapper_Stat(dirFullName, &dirStat);
		if(0 != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_EXT_Stat() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Closedir(dNo);
			return ercd;
		}

		g_bf_dcf_db_dir_tbl[dirTblIdx].date = dirStat.m_date;
		g_bf_dcf_db_dir_tbl[dirTblIdx].time1 = dirStat.m_time;
		g_bf_dcf_db_dir_tbl[dirTblIdx].sec10m = dirStat.m_sec10m;
#else
		g_bf_dcf_db_dir_tbl[dirTblIdx].date = dir.m_date;
		g_bf_dcf_db_dir_tbl[dirTblIdx].time1 = dir.m_time;
		g_bf_dcf_db_dir_tbl[dirTblIdx].sec10m = dir.m_sec10m;
#endif
	}

	/*-----------------------------------------------------------------------*/
	ercd = BF_Mdf_Wrapper_Closedir(dNo);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
	}
	/* dir buf table update */
	dcf_db_set_dir_db_buf_tbl();

	return ercd;
}

#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---

/**
 * @brief set top directory
 * @param i:const FW_CHAR* dir_fullpass_name
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Set_Dir(const FW_CHAR* dir_fullpass_name)
{
	FW_INT32 ercd;
	FW_CHAR dir_path[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR file_path[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	T_BF_FS_IF_ENTRY dirent = {0};
	BOOL first_dir_flag = TRUE;
	T_BF_FS_IF_STAT dir_status;
	FW_SHORT dirTblIdx;
	FW_UCHAR attr;

	/* check parameter */
	if(NULL == dir_fullpass_name) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(!dcf_db_is_top_dir_name(dir_fullpass_name)) {
		return FSDCF_ERRNO_NOT_DCIM;
	}

	BF_Dcf_Db_Clear_Db_buf();
	strcpy((FW_CHAR*)g_bf_dcf_db_dir_ctrl_block.topDirName, dir_fullpass_name);
	g_bf_dcf_db_dir_ctrl_block.dirCnt = 0;

	/* clear g_bf_dcf_db_dir_tbl table */
	for(dirTblIdx = 0; dirTblIdx < D_BF_DCF_DB_DIR_BUF_MAX; dirTblIdx++) {
		g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0] = 0;
		g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus = 0;
	}
	g_bf_dcf_db_file_num_as_dir_num = 0;
	g_bf_dcf_db_latest_dir_num = 0;
	g_bf_dcf_db_duplicate_dir_num = 0;
	g_bf_dcf_db_hidden_dir_num = 0;

	strcpy(dir_path, dir_fullpass_name);
	dir_path[strlen(dir_fullpass_name)-1] = '\0';
	ercd = BF_Mdf_Wrapper_Get_Attribute(dir_path, &attr);
	if(ercd != FSDCF_ERRNO_OK){
		ercd = BF_Mdf_Wrapper_Mkdir(dir_path, 0x01ff);
		if (ercd != FSDCF_ERRNO_OK) {
			return ercd;
		}
	}
	
	ercd = BF_Mdf_Wrapper_Chdir(dir_path);
	if (ercd != FSDCF_ERRNO_OK) {
		return ercd;
	}
	
	/*-----------------------------------------------------------------------*/
	for(;;) {
		if (first_dir_flag) {
			// search first
			strcpy(file_path, "*");
			ercd = BF_Fs_If_Fsfirst(0, (const CHAR*)file_path, D_BF_FS_IF_ATTR_ENTRY, &dirent);
		}
		else {
			// search next
			ercd = BF_Fs_If_Fsnext(0, &dirent);
		}

		if (ercd != FSDCF_ERRNO_OK) {
			if (ercd == D_BF_MDF_WRAPPER_ENT_LAST) {
				// search end
				ercd = FSDCF_ERRNO_OK;
				break;
			}
			
			return ercd;
		}

		if (first_dir_flag) {
			first_dir_flag = FALSE;
		}

		if ((dirent.fs_attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
			if ((strcmp(dirent.fs_fname, ".") == 0) || (strcmp(dirent.fs_fname, "..") == 0)) {
				continue;
			}
		}

		dirTblIdx = BF_Dcf_Db_Get_Dir_No(dirent.fs_fname);
		if( dirTblIdx == FSDCF_ERRNO_NOT_SUPPORT_DIRNO ) {
			continue;
		}
		
		if ((dirent.fs_attributes & D_BF_FS_IF_ATTR_DIRECTORY) != D_BF_FS_IF_ATTR_DIRECTORY) {
			// set directory number of other directory
			if (g_bf_dcf_db_file_num_as_dir_num < dirTblIdx) {
				g_bf_dcf_db_file_num_as_dir_num = dirTblIdx;
			}
			continue;
		}

		if(g_bf_dcf_db_latest_dir_num < dirTblIdx){
			g_bf_dcf_db_latest_dir_num = dirTblIdx;
		}

		if((dirent.fs_attributes & ATTR_HIDDEN) == ATTR_HIDDEN){
			if(g_bf_dcf_db_hidden_dir_num < dirTblIdx){
				g_bf_dcf_db_hidden_dir_num = dirTblIdx;
			}
		}

		dirTblIdx -= D_BF_DCF_DB_DIR_START_NUM;

		if(BF_Dcf_Attr_Get_Attr_Dupli_Ope() == 0){
		// When the same number as the directory exists, it registers in DCF-DB.
			if(0 == g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0]) {
				g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus = 0;

			}
			else {
				// not new one
				g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus = 1;

				if(g_bf_dcf_db_duplicate_dir_num < (dirTblIdx + D_BF_DCF_DB_DIR_START_NUM)){
					g_bf_dcf_db_duplicate_dir_num = dirTblIdx + D_BF_DCF_DB_DIR_START_NUM;
				}

				if(0 == strcmp((FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, (FW_CHAR*)g_bf_dcf_db_dir_name_dcf)) {
					continue;
				}
				if(0 != strcmp(dirent.fs_fname + 3, (FW_CHAR*)g_bf_dcf_db_dir_name_dcf)) {
					continue;
				}
			}
		}
		else{
		// When the same number as the directory exists, it doesn't register in DCF-DB.
			if(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0] != 0){

				g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0] = 0;
				g_bf_dcf_db_dir_tbl[dirTblIdx].date = 0;
				g_bf_dcf_db_dir_tbl[dirTblIdx].time1 = 0;
				g_bf_dcf_db_dir_tbl[dirTblIdx].sec10m = 0;
				g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus = 1;

				if(g_bf_dcf_db_duplicate_dir_num < (dirTblIdx + D_BF_DCF_DB_DIR_START_NUM)){
					g_bf_dcf_db_duplicate_dir_num = dirTblIdx + D_BF_DCF_DB_DIR_START_NUM;
				}

				continue;

			}
			else if(g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus != 0){
				continue;
			}
			else {
				; // DO NOTHING
			}

		}
		strncpy((FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, dirent.fs_fname + 3, sizeof(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName) - 1);
		g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[sizeof(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName) - 1] = '\0';

		BF_Mdf_Wrapper_Stat((const FW_CHAR*)dirent.fs_fname, &dir_status);
		g_bf_dcf_db_dir_tbl[dirTblIdx].date = dir_status.date;
		g_bf_dcf_db_dir_tbl[dirTblIdx].time1 = dir_status.time;
		g_bf_dcf_db_dir_tbl[dirTblIdx].sec10m = dir_status.sec10m;
	}

	/*-----------------------------------------------------------------------*/

	/* dir buf table update */
	dcf_db_set_dir_db_buf_tbl();

	return ercd;
}

#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---


/*-----------------------------------------------------------------------------
; Read number of directorys
-----------------------------------------------------------------------------*/
/**
 * @brief read directory counter
 * @param o:FW_SHORT* pdir_count
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Read_Dir_Count(FW_SHORT* pdir_count)
{
	/* check parameter */
	if(NULL == pdir_count) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	*pdir_count = g_bf_dcf_db_dir_ctrl_block.dirCnt;
	return FSDCF_ERRNO_OK;
}


/**
 * @brief read directory index by directory name
 * @param i:const FW_CHAR* dir_name
 * @param o:FW_SHORT* pdir_index
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Get_Index_By_Dir(const FW_CHAR* dir_name, FW_SHORT* pdir_index)
{
	FW_SHORT dirTblIdx;
	FW_CHAR  dirNameTmp[D_BF_DCF_IF_DIR_NAME_MAX];

	/* check parameter */
	if(NULL == dir_name) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(NULL == pdir_index) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	memcpy(dirNameTmp, (FW_CHAR*)dir_name, 8);
	dirNameTmp[8] = '\0';
	*pdir_index = 0;

	dirTblIdx = BF_Dcf_Db_Get_Dir_No(dirNameTmp);
	if(dirTblIdx == FSDCF_ERRNO_NOT_SUPPORT_DIRNO ) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	dirTblIdx -= D_BF_DCF_DB_DIR_START_NUM;

	if(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0]) {
		if(0 == strcmp((FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, dirNameTmp + 3)) {
			*pdir_index = g_bf_dcf_db_dir_tbl[dirTblIdx].dirIndex;
			return FSDCF_ERRNO_OK;
		}
	}
	return D_BF_MDF_WRAPPER_ERR_NOTDIR;
}


/**
 * @brief get directory name by directory index
 * @param i:FW_SHORT dir_index
 * @param o:FW_CHAR* dir_name
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Read_Dir_Name(FW_SHORT dir_index, FW_CHAR* dir_name)
{
	FW_INT32 ercd;
	FW_SHORT dirTblIdx;
	FW_CHAR dirNameTmp[D_BF_DCF_IF_DIR_NAME_MAX];

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(NULL == dir_name) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	ercd = dcf_db_search_dir_info(dir_index, &dirTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* make "100XXXXX" */
	ercd = BF_Dcf_Db_Make_Dir_Path(dirNameTmp, (FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, dirTblIdx + D_BF_DCF_DB_DIR_START_NUM);
	if( ercd != FSDCF_ERRNO_OK ) {
		return ercd;
	}

	strcpy(dir_name, dirNameTmp);
	return FSDCF_ERRNO_OK;
}

/**
 * @brief get directory duplicate status by directory index
 * @param i:FW_SHORT dir_index
 * @param o:FW_CHAR* dir_dupstatus(0 : UnDuplicated, 1 : Duplicated)
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Read_Dir_Duplicatestatus(FW_SHORT dir_index, FW_CHAR* dir_dupstatus)
{
	FW_INT32 ercd;
	FW_SHORT dirTblIdx;

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(NULL == dir_dupstatus) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	ercd = dcf_db_search_dir_info(dir_index, &dirTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	*dir_dupstatus = g_bf_dcf_db_dir_tbl[dirTblIdx].dir_dupstatus;
	return FSDCF_ERRNO_OK;
}

/**
 * @brief read directory data and time by directory index
 * @param i:FW_SHORT dir_index
 * @param o:FW_USHORT* pdir_created_date
 * @param o:FW_USHORT* pdir_created_time
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Read_Dir_Date_Time(FW_SHORT dir_index, FW_USHORT* pdir_created_date, FW_USHORT* pdir_created_time)
{
	FW_INT32 ercd;
	FW_SHORT dirTblIdx;

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(NULL == pdir_created_date) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(NULL == pdir_created_time) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	ercd = dcf_db_search_dir_info(dir_index, &dirTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	*pdir_created_date = g_bf_dcf_db_dir_tbl[dirTblIdx].date;
	*pdir_created_time = g_bf_dcf_db_dir_tbl[dirTblIdx].time1;
	return FSDCF_ERRNO_OK;
}

/**
 * @brief Get max file number as directory number
 * @param o:FW_SHORT* file_no
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Get_Max_File_Num_As_Dir_Num(FW_SHORT* file_no)
{
	/* check parameter */
	if(NULL == file_no) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	*file_no = g_bf_dcf_db_file_num_as_dir_num;
	return FSDCF_ERRNO_OK;
}

/*=============================================================================
; Directory Buffer Handling Functions
=============================================================================*/
/**
 * @brief set current directory
 * @param i:FW_SHORT dir_index
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Set_Current_Dir(FW_SHORT dir_index)
{
	FW_INT32 ercd;
	FW_SHORT cnt;
	FW_SHORT dirIndexTmp2;
	FW_SHORT dirTblIdxTmp;
	FW_SHORT dirTblIdx;
	FW_SHORT lpDbCurrentTmp;
	FW_SHORT lpDbCash1Tmp;
	FW_SHORT lpDbCash2Tmp;

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	if(DCF_DB_CURRENT_DIR == dir_index){
		if(-1 != g_bf_dcf_db_current_db) {
			return FSDCF_ERRNO_OK;
		}
		else{
			return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
		}
	}
	printf("BF_Dcf_Db_Set_Current_Dir() START dir_index=%d,  current_db=%d \n",dir_index, g_bf_dcf_db_current_db);

	dirIndexTmp2 = (0 < dir_index) ? dir_index : g_bf_dcf_db_dir_ctrl_block.dirCnt;

	lpDbCurrentTmp = g_bf_dcf_db_current_db;
	lpDbCash1Tmp = g_bf_dcf_db_cash1_db;
	lpDbCash2Tmp = g_bf_dcf_db_cash2_db;

	/* search DB buffer about dir_index */
	if((DCF_DB_CURRENT_DIR > dirIndexTmp2) && (-1 != g_bf_dcf_db_current_db)) {
		for(cnt = 0; cnt <= 2; cnt++) {
			/* dir index */
			if(g_bf_dcf_db_db_buf_tbl[cnt].objCnt != -1) {
				dirTblIdxTmp = g_bf_dcf_db_db_buf_tbl[cnt].dirTblIdx;
				if(g_bf_dcf_db_dir_tbl[dirTblIdxTmp].dirIndex == dirIndexTmp2) {
					// found
					if(cnt != g_bf_dcf_db_current_db){
						g_bf_dcf_db_current_db = cnt;
						g_bf_dcf_db_cash1_db = lpDbCurrentTmp;
						if(g_bf_dcf_db_current_db == lpDbCash2Tmp){
							g_bf_dcf_db_cash2_db = lpDbCash1Tmp;
						}
					}
					printf("BF_Dcf_Db_Set_Current_Dir() END \n");
					return FSDCF_ERRNO_OK;
				}
			}
		}

		/* When Current Directory is not found from DB Buffer, DB Buffer is update. */

		g_bf_dcf_db_current_db = lpDbCash2Tmp;
		g_bf_dcf_db_cash1_db = lpDbCurrentTmp;
		g_bf_dcf_db_cash2_db = lpDbCash1Tmp;

		/* set current DB directory */
		ercd = dcf_db_search_dir_tbl(dirIndexTmp2, &dirTblIdx);
		if(FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

		ercd = dcf_db_set_db_buf(dirIndexTmp2, dirTblIdx, 0, g_bf_dcf_db_current_db);
		if(FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

	}

	/* DB Buffer is update */
	if(-1 == g_bf_dcf_db_current_db){
		g_bf_dcf_db_current_db = 0;
		g_bf_dcf_db_cash1_db = 1;
		g_bf_dcf_db_cash2_db = 2;

		/* set current DB directory */
		ercd = dcf_db_search_dir_tbl(dirIndexTmp2, &dirTblIdx);
		if(FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

		ercd = dcf_db_set_db_buf(dirIndexTmp2, dirTblIdx, 0, g_bf_dcf_db_current_db);
		if(FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

	}
	return FSDCF_ERRNO_OK;
}


/**
 * @brief set current directory only
 * @param i:FW_SHORT dir_index
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Set_Current_Dir_Only(FW_SHORT dir_index)
{
	FW_INT32 ercd;
	FW_SHORT dirIndexTmp;
	FW_SHORT dirIndexTmp2;
	FW_SHORT dirTblIdx;

	/* check status */
	if(-1 != g_bf_dcf_db_current_db) {
		// Not just after startup.
		ercd = BF_Dcf_Db_Set_Current_Dir(dir_index);
		return ercd;
	}

	if(DCF_DB_CURRENT_DIR == dir_index) {
		return FSDCF_ERRNO_OK;
	}

	g_bf_dcf_db_current_db = 1;

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR <= dir_index) || (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	dirIndexTmp2 = dir_index;

	/* set current DB directory */
	ercd = dcf_db_search_dir_tbl(dirIndexTmp2, &dirTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	dirIndexTmp = (0 < dirIndexTmp2) ? dirIndexTmp2 : (g_bf_dcf_db_dir_ctrl_block.dirCnt + dirIndexTmp2 + 1);
	return dcf_db_set_db_buf(dirIndexTmp, dirTblIdx, 0, g_bf_dcf_db_current_db);
}

#if 0
/**
 * @brief set both previous and next directorys only
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Set_Prev_Next_Dir_Only(FW_VOID)
{
	FW_INT32 ercd;
	FW_SHORT dirIndexTmp;
	FW_SHORT dirTblIdx;

	/* check status */
	if((0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) || (1 == g_bf_dcf_db_dir_ctrl_block.dirCnt)) {
		return FSDCF_ERRNO_OK;
	}

	dirTblIdx = g_bf_dcf_db_db_buf_tbl[g_bf_dcf_db_current_db].dirTblIdx;
	dirIndexTmp = g_bf_dcf_db_dir_tbl[dirTblIdx].dirIndex;

	/* set next DB directory */
	dirIndexTmp = (g_bf_dcf_db_dir_ctrl_block.dirCnt > dirIndexTmp) ? (dirIndexTmp + 1) : 1;

	ercd = dcf_db_search_dir_tbl(dirIndexTmp, &dirTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_set_db_buf(dirIndexTmp, dirTblIdx, 0, dcf_db_get_next_buf_num);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	if(2 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return FSDCF_ERRNO_OK;
	}

	/* set previous DB directory */
	dirIndexTmp = (2 < dirIndexTmp) ? (dirIndexTmp - 2) : (g_bf_dcf_db_dir_ctrl_block.dirCnt + dirIndexTmp - 2);

	ercd = dcf_db_search_dir_tbl(dirIndexTmp, &dirTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_set_db_buf(dirIndexTmp, dirTblIdx, 0, dcf_db_get_prev_buf_num);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	return FSDCF_ERRNO_OK;
}
#endif

/**
 * @brief read object counter by directory index
 * @param i:FW_SHORT dir_index
 * @param o:FW_SHORT* pobj_count
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Read_Obj_Count(FW_SHORT dir_index, FW_SHORT* pobj_count)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx;

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(NULL == pobj_count) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	ercd = dcf_db_search_db_buf(dir_index, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	*pobj_count = g_bf_dcf_db_db_buf_tbl[bufTblIdx].objCnt;
	return FSDCF_ERRNO_OK;
}


/**
 * @brief Read object information
 * @param  I : FW_SHORT dir_index
 *         I : FW_SHORT obj_index
 *         O : T_BF_DCF_DB_OBJ_INFO** ppObjInfo
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Db_Read_Obj_Info(FW_SHORT dir_index, FW_SHORT obj_index, T_BF_DCF_DB_OBJ_INFO** ppObjInfo)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx;
	FW_SHORT objTblIdx;

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(((FW_INT32)NULL) == obj_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}
	if(NULL == ppObjInfo) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	ercd = dcf_db_search_db_buf(dir_index, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_search_obj_info_by_db(bufTblIdx, obj_index, &objTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	*ppObjInfo = &g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx];

	return FSDCF_ERRNO_OK;
}


/**
 * @brief read object name by both directory index and object index
 * @param i:FW_SHORT dir_index
 * @param i:FW_SHORT obj_index
 * @param o:FW_CHAR* obj_name
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Read_Obj_Name(FW_SHORT dir_index, FW_SHORT obj_index, FW_CHAR* obj_name)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx;
	FW_SHORT objTblIdx;
	FW_CHAR objNameOnly[D_BF_DCF_IF_OBJ_ONLY_NAME_MAX];

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(((FW_INT32)NULL) == obj_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}
	if(NULL == obj_name) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	ercd = dcf_db_search_db_buf(dir_index, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_search_obj_info_by_db(bufTblIdx, obj_index, &objTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = BF_Dcf_Db_Make_File_Path(objNameOnly, g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName, objTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}
	strcpy(obj_name, objNameOnly);

	return FSDCF_ERRNO_OK;
}


/**
 * @brief read Directory+Object name by both directory index and object index
 * @param i:FW_SHORT dir_index
 * @param i:FW_SHORT obj_index
 * @param o:FW_CHAR* dir_obj_name linkage name
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Read_Dir_Obj_Name(FW_SHORT dir_index, FW_SHORT obj_index, FW_CHAR* dir_obj_name)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx;
	FW_SHORT dirTblIdx;
	FW_SHORT objTblIdx;
	FW_CHAR dirNameTmp[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_CHAR objNameOnly[D_BF_DCF_IF_OBJ_ONLY_NAME_MAX];

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(((FW_INT32)NULL) == obj_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}
	if(NULL == dir_obj_name) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR > dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	ercd = dcf_db_search_dir_info(dir_index, &dirTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_search_db_buf(dir_index, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_search_obj_info_by_db(bufTblIdx, obj_index, &objTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* make "100XXXXX" */
	ercd = BF_Dcf_Db_Make_Dir_Path(dirNameTmp, (FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, dirTblIdx + D_BF_DCF_DB_DIR_START_NUM);
	if( ercd != FSDCF_ERRNO_OK ) {
		return ercd;
	}

	ercd = BF_Dcf_Db_Make_File_Path(objNameOnly, g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName, objTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	strcpy(dir_obj_name, dirNameTmp);
	strcat(dir_obj_name, "\\");
	strcat(dir_obj_name, objNameOnly);
	return FSDCF_ERRNO_OK;
}


/**
 * @brief read directory index number
 * @param i:FW_SHORT dir_index
 * @param o:FW_SHORT* pdir_index_num
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Get_Dir_Index_Num(FW_SHORT dir_index, FW_SHORT* pdir_index_num)
{
	FW_SHORT dirTblIdxTmp;

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(NULL == pdir_index_num) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if(-1 == g_bf_dcf_db_current_db) {
		return FSDCF_ERRNO_SYSTEM_FAILED;
	}
	if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	if(dir_index == DCF_DB_CURRENT_DIR){
		dirTblIdxTmp = g_bf_dcf_db_db_buf_tbl[g_bf_dcf_db_current_db].dirTblIdx;
		*pdir_index_num = g_bf_dcf_db_dir_tbl[dirTblIdxTmp].dirIndex;
	}
	else{
		*pdir_index_num = dir_index;
	}

	return FSDCF_ERRNO_OK;
}


/**
 * @brief write attribute of object by both directory index and object index
 * @param i:FW_SHORT dir_index
 * @param i:FW_SHORT obj_index
 * @param o:FW_UCHAR* pobj_attr
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Write_Obj_Attr(FW_SHORT dir_index, FW_SHORT obj_index, FW_UCHAR obj_attr)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx;
	FW_SHORT objTblIdx;

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(((FW_INT32)NULL) == obj_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	ercd = dcf_db_search_db_buf(dir_index, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_search_obj_info_by_db(bufTblIdx, obj_index, &objTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].attr = obj_attr;
	return FSDCF_ERRNO_OK;
}


/**
 * @brief read extension type of object by both directory index and object index
 * @param i:FW_SHORT dir_index
 * @param i:FW_SHORT obj_index
 * @param o:FW_ULONG* pext_type
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Read_Ext_Type(FW_SHORT dir_index, FW_SHORT obj_index, FW_UCHAR* pext_type)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx;
	FW_SHORT objTblIdx;

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(((FW_INT32)NULL) == obj_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}
	if(NULL == pext_type) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	ercd = dcf_db_search_db_buf(dir_index, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_search_obj_info_by_db(bufTblIdx, obj_index, &objTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	*pext_type = g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType;
	return FSDCF_ERRNO_OK;
}

#if 0
/**
 * @brief read number of allowed extension by both directory index and object index
 * @param i:FW_SHORT dir_index
 * @param i:FW_SHORT obj_index
 * @param o:FW_CHAR* p_allowed_ext_count
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Read_Allowed_Ext_Count(FW_SHORT dir_index, FW_SHORT obj_index, FW_UCHAR* p_allowed_ext_count)
{
	FW_INT32 ercd;
	FW_INT32 cnt;
	FW_INT32 i;
	FW_UCHAR extTypeTmp;

	ercd = BF_Dcf_Db_Read_Ext_Type(dir_index, obj_index, &extTypeTmp);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	cnt = 0;
	for(i = 0; i < 8; i++) {
		if(0 != (1 & extTypeTmp)) {
			cnt++;
		}
		extTypeTmp >>= 1;
	}
	*p_allowed_ext_count = cnt;
	return FSDCF_ERRNO_OK;
}
#endif


/*=============================================================================
; Extension Handling Functions
=============================================================================*/
/**
 * @brief set allowed extenstion list
 * @param i:const FW_CHAR* allowed_ext_list
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Set_Allowed_Ext_List(const FW_CHAR* allowed_ext_list)
{
	FW_INT32 i;
	const FW_CHAR* pAddExtNameTmp;
	FW_CHAR* pExtNameTmp;

	/* check parameter */
	if(0 == allowed_ext_list) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	pExtNameTmp = g_bf_dcf_db_allow_ext_name;
	pAddExtNameTmp = allowed_ext_list;
	while('\0' != *pAddExtNameTmp) {
		for(i = 0; i < 4; i++) {
			*pExtNameTmp = *pAddExtNameTmp;
			pAddExtNameTmp++;
			pExtNameTmp++;
		}
	}
	*pExtNameTmp = '\0';

	return FSDCF_ERRNO_OK;
}


/**
 * @brief get allowed extension type by extension name
 * @param i:const FW_CHAR* ext_name
 * @param o:FW_ULONG* pext_type
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Get_Type_By_Ext(const FW_CHAR* ext_name, FW_UCHAR* pext_type)
{
	FW_UINT32 index;
	const FW_CHAR* pExtNameTmp;

	/* check parameter */
	if(NULL == ext_name) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(NULL == pext_type) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	pExtNameTmp = g_bf_dcf_db_allow_ext_name;
	if ( strlen(pExtNameTmp)==0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("Not allow_ext_name=%s \n", pExtNameTmp));
#endif
		return FSDCF_ERRNO_SYSTEM_FAILED;
	}
	index = 0;
	while('\0' != pExtNameTmp[0]) {
		if(0 == strcmp(ext_name, pExtNameTmp)) {
			*pext_type = (1U << index);
			return FSDCF_ERRNO_OK;
		}
		pExtNameTmp += D_BF_DCF_IF_EXT_MAX;
		index++;
	}

	*pext_type = 0;
	return FSDCF_ERRNO_NOT_SUPPORT_FILE;

}


/**
 * @brief get allowed extension name by extension type
 * @param i:FW_ULONG ext_type
 * @param o:FW_CHAR* allowed_ext_name
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Get_Ext_By_Type(FW_UCHAR ext_type, FW_CHAR* allowed_ext_name)
{
	FW_UINT32 checkBit;
	const FW_CHAR* pExtNameTmp;

	/* check parameter */
	if(NULL == allowed_ext_name) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	allowed_ext_name[0] = '\0';
	if(0 == ext_type) {
		return D_BF_MDF_WRAPPER_ERR_INVAL;
	}

	pExtNameTmp = g_bf_dcf_db_allow_ext_name;
	checkBit = 1;
	while(0 == (ext_type & checkBit)) {
		checkBit <<= 1;
		pExtNameTmp += D_BF_DCF_IF_EXT_MAX;
	}

	strcpy(allowed_ext_name, pExtNameTmp);
	return FSDCF_ERRNO_OK;
}


/**
 * @brief get information from DCF DB.
 * @param i:dir_index
 * @param i:obj_index
 * @param io:*dcfdb_info
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Db_Get_Info(FW_SHORT dir_index, FW_SHORT obj_index, T_BF_DCF_IF_DB_INFO* dcfdb_info)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx = 0;
	FW_SHORT dirTblIdx = 0;
	FW_SHORT objTblIdx = 0;
	FW_CHAR dirNameTmp[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_CHAR objNameOnly[D_BF_DCF_IF_OBJ_ONLY_NAME_MAX];

	/* check parameter */
	if(NULL == dcfdb_info) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	if(dcfdb_info->p_dirCnt) {
		*dcfdb_info->p_dirCnt = g_bf_dcf_db_dir_ctrl_block.dirCnt;
	}

	if(dcfdb_info->topDirName) {
		memcpy(dcfdb_info->topDirName, (FW_VOID*)g_bf_dcf_db_dir_ctrl_block.topDirName, D_BF_DCF_IF_TOP_DIR_NAME_MAX);
	}

	if(dcfdb_info->p_date
	   || dcfdb_info->p_time
	   || dcfdb_info->dirName
	   || dcfdb_info->p_dirIndex
	   || dcfdb_info->p_objectCnt
	   || dcfdb_info->p_extType
	   || dcfdb_info->objName
	   || dcfdb_info->p_attr) {
		if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
			return D_BF_MDF_WRAPPER_ERR_NOTDIR;
		}
		if(0 == dir_index) {
			return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
		}
		if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
			return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
		}
	}

	if(dcfdb_info->p_date
	   || dcfdb_info->p_time
	   || dcfdb_info->dirName) {
		ercd = dcf_db_search_dir_info(dir_index, &dirTblIdx);
		if(FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

		if(dcfdb_info->p_date) {
			*dcfdb_info->p_date = g_bf_dcf_db_dir_tbl[dirTblIdx].date;
		}

		if(dcfdb_info->p_time) {
			*dcfdb_info->p_time = g_bf_dcf_db_dir_tbl[dirTblIdx].time1;
		}

		if(dcfdb_info->p_sec10m) {
			*dcfdb_info->p_sec10m = g_bf_dcf_db_dir_tbl[dirTblIdx].sec10m;
		}

		if(dcfdb_info->dirName) {
			/* make "100XXXXX" */
			ercd = BF_Dcf_Db_Make_Dir_Path(dirNameTmp, (FW_CHAR*)g_bf_dcf_db_dir_tbl[dirTblIdx].dirName, dirTblIdx + D_BF_DCF_DB_DIR_START_NUM);
			if( ercd != FSDCF_ERRNO_OK ) {
				return ercd;
			}
			memcpy(dcfdb_info->dirName, dirNameTmp, D_BF_DCF_IF_DIR_NAME_MAX);
		}
	}

	if(dcfdb_info->p_dirIndex
	   || dcfdb_info->p_objectCnt
	   || dcfdb_info->p_extType
	   || dcfdb_info->objName
	   || dcfdb_info->p_attr) {
		if(-1 == g_bf_dcf_db_current_db) {
			return D_BF_MDF_WRAPPER_ERR_NOTDIR;
		}

		ercd = dcf_db_search_db_buf(dir_index, &bufTblIdx);
		if(FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

		if(dcfdb_info->p_dirIndex) {
			dirTblIdx = g_bf_dcf_db_db_buf_tbl[bufTblIdx].dirTblIdx;
			*dcfdb_info->p_dirIndex = g_bf_dcf_db_dir_tbl[dirTblIdx].dirIndex;
		}

		if(dcfdb_info->p_objectCnt) {
			*dcfdb_info->p_objectCnt = g_bf_dcf_db_db_buf_tbl[bufTblIdx].objCnt;
		}
	}

	if(dcfdb_info->p_extType
	   || dcfdb_info->objName
	   || dcfdb_info->p_attr) {
		if(0 == obj_index) {
			return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
		}
		if(g_bf_dcf_db_db_buf_tbl[bufTblIdx].objCnt < abs(obj_index)) {	// pgr0000
			return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
		}

		ercd = dcf_db_search_obj_info_by_db(bufTblIdx, obj_index, &objTblIdx);
		if(FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

		if(dcfdb_info->p_extType) {
			*dcfdb_info->p_extType = g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType;
		}

		if(dcfdb_info->objName) {
			ercd = BF_Dcf_Db_Make_File_Path(objNameOnly, g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName, objTblIdx);
			if(FSDCF_ERRNO_OK != ercd) {
				return ercd;
			}
			memcpy(dcfdb_info->objName, objNameOnly, D_BF_DCF_IF_OBJ_ONLY_NAME_MAX);
		}

		if(dcfdb_info->p_attr) {
			*dcfdb_info->p_attr = g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].attr;
		}
	}

	return FSDCF_ERRNO_OK;
}


/**
 * @brief 		Create the directory. 
 * @param		I : USHORT dirNum
 * @param		I : UCHAR dirNameIndex (0, 1, ...)
 * @return 		Success/Fail
 * @note		None
 * @attention 	current directory will be changed.
 */
FW_INT32 BF_Dcf_Db_Create_Dir(FW_USHORT dirNum, FW_UCHAR dirNameIndex)
{
	FW_INT32 ercd;
	FW_CHAR dirName[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	const FW_CHAR* dirNameSub;
	FW_USHORT dirNumTmp;
	FW_INT32 ii;
	FW_SHORT dir_index;

	if(2 < dirNameIndex) {
		return FSDCF_ERRNO_PARAM_FAILED;
	}
	if((dirNum < FJ_CUSTOM_DCFDB_DIRNUM_MIN) || (FJ_CUSTOM_DCFDB_DIRNUM_MAX < dirNum)) {
		return FSDCF_ERRNO_PARAM_FAILED;
	}

	if('\0' == g_bf_dcf_db_dir_ctrl_block.topDirName[0]) {
		return FSDCF_ERRNO_NOT_DCIM;
	}

	strcpy(dirName, (FW_CHAR*)g_bf_dcf_db_dir_ctrl_block.topDirName);
	ii = strlen(dirName);
	dirNumTmp = dirNum;
	dirName[ii++] = (dirNumTmp / 100) + '0';
	dirNumTmp %= 100;
	dirName[ii++] = (dirNumTmp / 10) + '0';
	dirNumTmp %= 10;
	dirName[ii++] = dirNumTmp + '0';
	dirName[ii] = '\0';

	switch(dirNameIndex) {
		case 0:
			dirNameSub = (FW_CHAR*)g_bf_dcf_db_dir_name_dcf;
			break;

		case 1: // FALL THROUGH
		case 2: // FALL THROUGH
		default:
			return FSDCF_ERRNO_PARAM_FAILED;
	}

	strcat(dirName, dirNameSub);

	ercd = BF_Mdf_Wrapper_Mkdir(dirName, 0x01ff);
	if(ercd < 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Mkdir() failed: ercd=%d, path=%s\n", ercd, dirName));
#endif
		return ercd;
	}

	ercd = dcf_db_insert_dir(dirName, &dir_index);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = BF_Dcf_Db_Set_Current_Dir(dir_index);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	return ercd;
}

/**
 * @brief Set directory path.
 * @param o:*dir_path DCF directory path
 * @param i:*dir_name directory name
 * @param i:dirno DCF directory number
 * @return 0:normal end
 * @rerurn -102:not support directory number
 */
FW_INT32 BF_Dcf_Db_Make_Dir_Path(FW_CHAR* dir_path, FW_CHAR* dir_name, FW_USHORT dirno)
{
	if(dir_path == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(dir_name == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	if( (dirno < FJ_CUSTOM_DCFDB_DIRNUM_MIN ) || (dirno > FJ_CUSTOM_DCFDB_DIRNUM_MAX) ) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("NOT SUPPORT DIRNO: dirno=%d\n", dirno));
#endif
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	dir_path[0] = '0' + (dirno / 100);
	dirno %= 100;
	dir_path[1] = '0' + (dirno / 10);
	dirno %= 10;
	dir_path[2] = '0' + dirno;
	dir_path[3] = 0; // NULL

	strcat(dir_path, dir_name);

	return 0;
}


/**
 * @brief Set file path.
 * @param o:*file_path DCF file path
 * @param i:fileno DCF file number
 * @return 0:normal end
 * @rerurn -103:not support file number
 */
FW_INT32 BF_Dcf_Db_Make_File_Path(FW_CHAR* file_path, FW_CHAR* file_name, FW_SHORT fileno)
{
	FW_CHAR num_char[5];

	if(file_path == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(file_name == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	if( (fileno < 1) || (fileno > FJ_CUSTOM_DCFDB_FILENUM_MAX ) ) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("NOT SUPPORT FILENO: fileno=%d\n", fileno));
#endif
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}

	num_char[0] = '0' + (fileno / 1000);
	fileno %= 1000;
	num_char[1] = '0' + (fileno / 100);
	fileno %= 100;
	num_char[2] = '0' + (fileno / 10);
	fileno %= 10;
	num_char[3] = '0' + fileno;
	num_char[4] = 0; // NULL

	memcpy(file_path, file_name, 4);
	file_path[4] = '\0';
	strcat(file_path, num_char);

	return 0;
}

/**
 * @brief get last dirno and fileno.
 * @param o:*dirno last directory number
 * @param o:*fileno last file number
 * @return error code
 */
FW_INT32 BF_Dcf_Db_Get_Last_File(FW_USHORT* dirno, FW_USHORT* fileno, E_BF_DCF_IF_LATEST_TYPE type)
{
	FW_INT32 ercd;

	if(dirno == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(fileno == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	if( type == E_BF_DCF_IF_LATEST_TYPE_ALL ){
		*dirno = g_bf_dcf_db_latest_dir_num;
		ercd = dcf_db_get_last_file_num(g_bf_dcf_db_latest_dir_num - D_BF_DCF_DB_DIR_START_NUM, fileno);
	}
	else if( type == E_BF_DCF_IF_LATEST_TYPE_SELECT ){
		ercd = dcf_db_get_select_dir(dirno, fileno, E_BF_DCF_DB_ASSIGN_LAST_KEY);
	}
	else{
		return FSDCF_ERRNO_OK;
	}
	return ercd;
}

/**
 * @brief get duplicate dirno and fileno.
 * @param o:*dirno duplicate directory number
 * @param o:*fileno duplicate file number
 * @return error code
 */
FW_INT32 BF_Dcf_Db_Get_Duplicate_No(FW_USHORT* dirno, FW_USHORT* fileno, E_BF_DCF_IF_LATEST_TYPE type)
{
	FW_INT32 ercd;

	if(dirno == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(fileno == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	if( type == E_BF_DCF_IF_LATEST_TYPE_ALL){
		*dirno = g_bf_dcf_db_duplicate_dir_num;
		*fileno = 0;

		ercd = FSDCF_ERRNO_OK;
	}
	else{
		ercd = dcf_db_get_select_dir( dirno, fileno, E_BF_DCF_DB_ASSIGN_DUPLICATE );
	}
	return ercd;
}

/**
 * @brief get hidden dirno.
 * @param o:*dirno hidden directory number
 * @return FW_VOID
 */
FW_INT32 BF_Dcf_Db_Get_Hidden_No(FW_USHORT* dirno, E_BF_DCF_IF_LATEST_TYPE type)
{
	FW_INT32 ercd;
	FW_USHORT fileno;

	if( type == E_BF_DCF_IF_LATEST_TYPE_ALL){
		*dirno = g_bf_dcf_db_hidden_dir_num;

		ercd = FSDCF_ERRNO_OK;
	}
	else{
		ercd = dcf_db_get_select_dir( dirno, &fileno, E_BF_DCF_DB_ASSIGN_HIDDEN );
	}
	return ercd;
}



/**
 * @brief Set the priority file name.
 * @param i:*file_name
 * @return error code
 * @note none
 */
FW_INT32 BF_Dcf_Db_Set_Prio_File_Name(FW_CHAR* file_name)
{

	strncpy((FW_CHAR*)g_bf_dcf_db_file_name_still, file_name, 4);
	g_bf_dcf_db_file_name_still[4] = 0;

	return FSDCF_ERRNO_OK;
}

/**
 * @brief Set the priority dir name.
 * @param i:*dir_name
 * @return error code
 * @note none
 */
FW_INT32 BF_Dcf_Db_Set_Prio_Dir_Name(FW_CHAR* dir_name)
{

	strncpy((FW_CHAR*)g_bf_dcf_db_dir_name_dcf, dir_name, 5);
	g_bf_dcf_db_dir_name_dcf[5] = 0;

	return FSDCF_ERRNO_OK;
}


/**
 * @brief Get the last file number.
 * @param i:dir_index dir index
 * @param o:*file_num file number
 * @return error code
 * @note none
 */
FW_INT32 BF_Dcf_Db_Get_Last_File_Number(FW_SHORT dir_index, FW_USHORT* file_num)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx;

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(NULL == file_num) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	ercd = dcf_db_search_db_buf(dir_index, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	*file_num = g_bf_dcf_db_db_buf_tbl[bufTblIdx].lastFileNum;
	return FSDCF_ERRNO_OK;
}

/**
 * @brief		Get first free File number in current directory.
 * @param		O : FW_USHORT* fileNum
 * @return error code
 * @note 		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_Db_Get_First_Free_File_No(FW_USHORT* fileNum)
{
	FW_SHORT objTblIdx;

	/* check parameter */
	if(NULL == fileNum) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(-1 == g_bf_dcf_db_current_db) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}

	*fileNum = 1;

	//if(0 == g_bf_dcf_db_db_buf_tbl[g_bf_dcf_db_current_db].objCnt) {return FSDCF_ERRNO_OK;}

	for(objTblIdx = 0; objTblIdx < D_BF_DCF_DB_OBJ_BUF_MAX; objTblIdx++) {
		if(g_bf_dcf_db_db_buf_tbl[g_bf_dcf_db_current_db].lastFileNum < *fileNum ) {
			break;
		}
		if((g_bf_dcf_db_obj_tbl[g_bf_dcf_db_current_db][objTblIdx].fileFlag & 0x03) != 0) {
			if(*fileNum < objTblIdx) {
				return FSDCF_ERRNO_OK;
			}
			if(*fileNum == objTblIdx) {
				*fileNum = *fileNum + 1;
			}
		}
	}
	if(D_BF_DCF_DB_OBJ_END_NUM < *fileNum) {
		return FSDCF_ERRNO_FILE_FULL;
	}
	return FSDCF_ERRNO_OK;
}


/**
 * @brief insert current file in DCF_DB.
 * @param i:*full_file_path full file path
 * @param o:*dirno dirno
 * @param o:*fileno fileno
 * @return error code
 */
FW_INT32 BF_Dcf_Db_Insert_Current_File(FW_CHAR* full_file_path, FW_SHORT* dirno, FW_SHORT* fileno)
{
	FW_INT32 ercd;
	FW_CHAR full_dir_path[18];
	FW_CHAR dir_path[10];
	FW_CHAR file_path[14];
	FW_SHORT dir_index_tmp;
	FW_SHORT bufTblIdx;

	FW_CHAR objNameOnly[D_BF_DCF_IF_OBJ_ONLY_NAME_MAX];
	
	ercd = dcf_db_get_dir_path(full_file_path, dir_path);
	if( ercd != 0 ) {
		return ercd;
	}

	ercd = BF_Dcf_Controller_Get_File_Path(full_file_path, file_path);
	if( ercd != 0 ) {
		return ercd;
	}
	
	*dirno = BF_Dcf_Db_Get_Dir_No(dir_path);
	if( *dirno == FSDCF_ERRNO_NOT_SUPPORT_DIRNO ) {
		return *dirno;
	}

	*fileno = dcf_db_get_file_no(file_path);
	if( *fileno == FSDCF_ERRNO_NOT_SUPPORT_FILENO ) {
		return *fileno;
	}

	ercd = BF_Dcf_Db_Get_Index_By_Dir(dir_path, &dir_index_tmp);
	if(FSDCF_ERRNO_OK == ercd) {
		// found dir
		ercd = dcf_db_search_db_buf(dir_index_tmp, &bufTblIdx);

		if(FSDCF_ERRNO_OK == ercd) {
			// inside DB
			memcpy(objNameOnly, file_path, 8);
			objNameOnly[8] = '\0';

			ercd = dcf_db_set_file_flag(file_path, full_file_path, bufTblIdx, NULL);
			if(FSDCF_ERRNO_OK != ercd) {
				return ercd;
			}

			ercd = dcf_db_insert_file(file_path, full_file_path, bufTblIdx, NULL, objNameOnly);
			if(FSDCF_ERRNO_OK != ercd) {
				return ercd;
			}

			/* update obj table */
			dcf_db_set_obj_db_buf_tbl(bufTblIdx);
#if 1	/* When the number of File in 1 folder increases, in Capture Save, 	*/
		/* I give a response when it becomes extremely late					*/
		/* When I add a file to Current Directory, I do not rebuild DCF-DB. */
		/* (hold it for an additional file and register) 					*/
			if(dir_index_tmp == g_bf_dcf_db_dir_tbl[g_bf_dcf_db_db_buf_tbl[g_bf_dcf_db_current_db].dirTblIdx].dirIndex) {
				return FSDCF_ERRNO_OK;
			}
#endif
		}
		// outsize DB
		/* change dir. */
		ercd = BF_Dcf_Db_Set_Current_Dir(dir_index_tmp);

		BF_Dcf_Db_Restruct_Current_Dir_Only();
	}
	else if(D_BF_MDF_WRAPPER_ERR_NOTDIR == ercd) {
		// NOT found dir
		memcpy(full_dir_path, full_file_path, 16);
		full_dir_path[16] = '\0';

		ercd = dcf_db_insert_dir(full_dir_path, &dir_index_tmp);
		if(FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

		BF_Dcf_Db_Clear_Db_buf();

		/* change dir. */
		ercd = BF_Dcf_Db_Set_Current_Dir(dir_index_tmp);
	}
	else {
		return ercd;
	}

	return ercd;
}


/**
 * @brief append latest file in DCF_DB.
 * @param i:*full_file_path full file path
 * @return error code
 */
FW_INT32 BF_Dcf_Db_Append_Latest_File(FW_CHAR* full_file_path)
{
	FW_INT32 ercd;
	FW_SHORT dirno, fileno;

	ercd = BF_Dcf_Db_Insert_Current_File(full_file_path, &dirno, &fileno);
	if(ercd != 0){
		return ercd;
	}

	return 0;
}

/**
 * @brief delete target file in DCF_DB.
 * @param i:file_index file index
 * @param i:*full_file_path full file path
 * @return error code
 */
FW_INT32 BF_Dcf_Db_Delete_File(T_BF_DCF_IF_FILE_INDEX file_index, FW_CHAR* full_file_path)
{
	FW_INT32 ercd;

	ercd = BF_Dcf_Db_Delete_Obj(file_index.pack.dir_index, file_index.pack.obj_index);
	if(ercd != 0){
		return ercd;
	}

	return 0;
}

/**
 * @brief update attribute of target file in DCF_DB.
 * @param FW_VOID
 * @return count
 */
FW_INT32 BF_Dcf_Db_Update_Attr(T_BF_DCF_IF_FILE_INDEX file_index, E_BF_DCF_IF_ATTR attr)
{
	FW_UCHAR obj_attr = 0;

	BF_Dcf_Db_Read_Obj_Attr(file_index.pack.dir_index, file_index.pack.obj_index, &obj_attr);
	obj_attr &= (~1);
	obj_attr |= attr;
	BF_Dcf_Db_Write_Obj_Attr(file_index.pack.dir_index, file_index.pack.obj_index, obj_attr);
	return 0;
}

/**
 * @brief update attribute of target file in DCF_DB.(all in current directory)
 * @param i:E_BF_DCF_IF_ATTR attr
 * @return error code
 */
FW_INT32 BF_Dcf_Db_Update_Attr_Dir(E_BF_DCF_IF_ATTR attr)
{
	FW_SHORT objcnt;
	FW_UCHAR obj_attr;
	FW_SHORT objTblIdx;

	/* check parameter */
	if(-1 == g_bf_dcf_db_current_db) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if(0 == g_bf_dcf_db_db_buf_tbl[g_bf_dcf_db_current_db].objCnt) {
		return FSDCF_ERRNO_OK;
	}

	objcnt = 0;

	for(objTblIdx = 0; objTblIdx < D_BF_DCF_DB_OBJ_BUF_MAX; objTblIdx++) {
		if(objcnt == g_bf_dcf_db_db_buf_tbl[g_bf_dcf_db_current_db].objCnt) {
			break;
		}
		if(g_bf_dcf_db_obj_tbl[g_bf_dcf_db_current_db][objTblIdx].objName[0] != 0) {
			obj_attr = g_bf_dcf_db_obj_tbl[g_bf_dcf_db_current_db][objTblIdx].attr;
			obj_attr &= (~1);
			obj_attr |= attr;
			g_bf_dcf_db_obj_tbl[g_bf_dcf_db_current_db][objTblIdx].attr = obj_attr;

			objcnt++;
		}
	}
	return FSDCF_ERRNO_OK;
}

FW_INT32 BF_Dcf_Db_Update_Attr_Dir_For_Stop(E_BF_DCF_IF_ATTR attr, FW_SHORT objIndex)
{
	FW_SHORT objcnt;
	FW_UCHAR obj_attr;
	FW_SHORT objTblIdx;

	/* check parameter */
	if(-1 == g_bf_dcf_db_current_db) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if(0 == g_bf_dcf_db_db_buf_tbl[g_bf_dcf_db_current_db].objCnt) {
		return FSDCF_ERRNO_OK;
	}

	objcnt = 0;

	for(objTblIdx = 0; objTblIdx < D_BF_DCF_DB_OBJ_BUF_MAX; objTblIdx++) {
		if(objcnt == g_bf_dcf_db_db_buf_tbl[g_bf_dcf_db_current_db].objCnt) {
			break;
		}
		if (objIndex < g_bf_dcf_db_obj_tbl[g_bf_dcf_db_current_db][objTblIdx].objIndex) {
			break;
		}
		if(g_bf_dcf_db_obj_tbl[g_bf_dcf_db_current_db][objTblIdx].objName[0] != 0) {
			obj_attr = g_bf_dcf_db_obj_tbl[g_bf_dcf_db_current_db][objTblIdx].attr;
			obj_attr &= (~1);
			obj_attr |= attr;
			g_bf_dcf_db_obj_tbl[g_bf_dcf_db_current_db][objTblIdx].attr = obj_attr;

			objcnt++;
		}
	}
	return FSDCF_ERRNO_OK;
}


/**
 * @brief exchange from file type to extension charactor.
 * @param i:file_type file type
 * @param o:*ext file extension
 * @return error code
 */
FW_INT32 BF_Dcf_Db_Change_File_Type_To_Char(FW_CHAR file_type, FW_CHAR* ext)
{
	if(ext == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	return BF_Dcf_Db_Get_Ext_By_Type(file_type, ext);
}


/**
 * @brief get last dirno and fileno in DB.
 * @param o:*dirno last directory number
 * @param o:*fileno last file number
 * @return FW_VOID
 */
FW_VOID BF_Dcf_Db_Get_Last_File_In_Db(FW_USHORT* dirno, FW_USHORT* fileno)
{
	FW_SHORT dbBufIdx;
	FW_SHORT dir_index_last;
	FW_SHORT tbl_index_last;
	FW_SHORT bufTblIdx;
	FW_SHORT objTblIdx;
	FW_SHORT dirTblIdxTmp;
	FW_INT32 ercd;

	if(-1 == g_bf_dcf_db_current_db) {
		*dirno = 100;
		*fileno = 0;
		return;
	}
	dir_index_last = 0;
	tbl_index_last = -1;

	for(dbBufIdx = 0; dbBufIdx < D_BF_DCF_DB_BUF_NUM; dbBufIdx++) {
		/* dir index */
		if(g_bf_dcf_db_db_buf_tbl[dbBufIdx].objCnt == -1) {
			continue;
		}
		dirTblIdxTmp = g_bf_dcf_db_db_buf_tbl[dbBufIdx].dirTblIdx;
		if(g_bf_dcf_db_dir_tbl[dirTblIdxTmp].dirIndex > dir_index_last) {
			dir_index_last = g_bf_dcf_db_dir_tbl[dirTblIdxTmp].dirIndex;
			tbl_index_last = dirTblIdxTmp;
		}
	}
	*dirno = tbl_index_last + D_BF_DCF_DB_DIR_START_NUM;
	ercd = dcf_db_search_db_buf(dir_index_last, &bufTblIdx);
	if(ercd != FSDCF_ERRNO_OK){
		// not found directory
		*fileno = 0;
		return;
	}

	if(g_bf_dcf_db_db_buf_tbl[bufTblIdx].objCnt > 0) {
		/* When there is implementation file, I search last file in DB. */
		ercd = dcf_db_search_obj_info_by_db(bufTblIdx, -1, &objTblIdx);
		if(ercd != FSDCF_ERRNO_OK){
			// not found file
			*fileno = 0;
		}
		else{
			*fileno = objTblIdx;
		}
	}
	else {
		/* When there is not implementation file, I assume last fileno 0 in DB. */
		*fileno = 0;
	}
	return;
}


/**
 * @brief get max dirno and fileno in specfied directory by linear search.
 * @param o:*max_dirno
 * @param o:*max_fileno
 * @return error code
 */
FW_INT32 BF_Dcf_Db_Get_Max_Number_Linear(FW_CHAR* dcim_path, FW_USHORT* max_dirno, FW_USHORT* max_fileno)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx;
	FW_SHORT dirTblIdx;
	FW_SHORT objTblIdx;

	if(NULL == dcim_path) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(NULL == max_dirno) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(NULL == max_fileno) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	ercd = dcf_db_search_dir_info(g_bf_dcf_db_dir_ctrl_block.dirCnt, &dirTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}
	*max_dirno = dirTblIdx + D_BF_DCF_DB_DIR_START_NUM;

	ercd = BF_Dcf_Db_Set_Current_Dir(g_bf_dcf_db_dir_ctrl_block.dirCnt);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_search_db_buf(g_bf_dcf_db_dir_ctrl_block.dirCnt, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_search_obj_info_by_db(bufTblIdx, g_bf_dcf_db_dir_ctrl_block.dirCnt, &objTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	*max_fileno = objTblIdx;

	strcpy(dcim_path, (FW_CHAR*)g_bf_dcf_db_dir_ctrl_block.topDirName);

	return FSDCF_ERRNO_OK;
}


/**
 * @brief re-struct current directory only
 * @param i:none
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Restruct_Current_Dir_Only(FW_VOID)
{
	FW_SHORT dirIndexTmp;
	FW_SHORT dirTblIdx;
	FW_INT32 ret;

	/* check status */
	if(-1 == g_bf_dcf_db_current_db) {
		/* current dir not set yet */
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}

	/* check parameter */
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}

	dirTblIdx = g_bf_dcf_db_db_buf_tbl[g_bf_dcf_db_current_db].dirTblIdx;
	dirIndexTmp = g_bf_dcf_db_dir_tbl[dirTblIdx].dirIndex;

	/* set current DB directory */
	ret = dcf_db_set_db_buf(dirIndexTmp, dirTblIdx, 0, g_bf_dcf_db_current_db);
	return ret;
}


/**
 * @brief delete object information
 * @param i:FW_SHORT dir_index
 * @param i:FW_SHORT obj_index
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Delete_Obj(FW_SHORT dir_index, FW_SHORT obj_index)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx;
	FW_SHORT objTblIdx;

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(((FW_INT32)NULL) == obj_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	ercd = dcf_db_search_db_buf(dir_index, &bufTblIdx);

	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_search_obj_info_by_db(bufTblIdx, obj_index, &objTblIdx);

	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* delete obj */
	g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName[0] = 0;
	g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].fileFlag = 0x00;
	g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].attr = 0x00;

	/* update obj table */
	dcf_db_set_obj_db_buf_tbl(bufTblIdx);

	return FSDCF_ERRNO_OK;
}

#if 1
/**
 * @brief delete multiple object information
 * @param i:T_BF_DCF_IF_FILE_INDEX* p_select_list delete object list
 * @param i:FW_CHAR del_obj_num delete object num
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Delete_Multi_Obj_In_Current_Dir(T_BF_DCF_IF_FILE_INDEX* p_select_list, FW_UCHAR del_obj_num)
{
	FW_INT32 ercd;
	FW_SHORT objTblIdx;
	FW_SHORT dirIdx;
	FW_SHORT dirTblIdx;
	FW_UCHAR list_index;

	/* check parameter */
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}

	dirTblIdx = g_bf_dcf_db_db_buf_tbl[g_bf_dcf_db_current_db].dirTblIdx;
	dirIdx = g_bf_dcf_db_dir_tbl[dirTblIdx].dirIndex;

	for (list_index = 0; list_index < del_obj_num; list_index++) {
		if (p_select_list[list_index].pack.dir_index != dirIdx) {
			continue;
		}

		ercd = dcf_db_search_obj_info_by_db(g_bf_dcf_db_current_db, p_select_list[list_index].pack.obj_index, &objTblIdx);
		if(FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

		if ((g_bf_dcf_db_obj_tbl[g_bf_dcf_db_current_db][objTblIdx].attr & 1) == 0) {
			/* delete obj */
			g_bf_dcf_db_obj_tbl[g_bf_dcf_db_current_db][objTblIdx].objName[0] = 0;
			g_bf_dcf_db_obj_tbl[g_bf_dcf_db_current_db][objTblIdx].fileFlag = 0x00;
			g_bf_dcf_db_obj_tbl[g_bf_dcf_db_current_db][objTblIdx].attr = 0x00;
		}
	}

	/* update obj table */
	dcf_db_set_obj_db_buf_tbl(g_bf_dcf_db_current_db);

	return FSDCF_ERRNO_OK;
}
#endif

/**
 * @brief search delete file information
 * @param i:FW_CHAR* dosdir_name DCF directory name
 * @param i:FW_CHAR* dosfile_name DCF file name
 * @param o:FW_SHORT *pdir_index
 * @param o:FW_SHORT* pobj_index
 * @return FW_INT32 error code(0:delete 1:delete ineligible)
 * @attention
 */
FW_INT32 BF_Dcf_Db_Search_Obj_By_Name(FW_CHAR* dosdir_name, FW_CHAR* dosfile_name, FW_SHORT* pdir_index, FW_SHORT* pobj_index)
{
	FW_INT32 ercd;
	FW_SHORT dirTblIdx;
	FW_SHORT objTblIdx;
	FW_SHORT bufTblIdx;

	/* check parameter */
	if(NULL == dosdir_name) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(NULL == dosfile_name) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	dirTblIdx = BF_Dcf_Db_Get_Dir_No(dosdir_name);
	if( dirTblIdx == FSDCF_ERRNO_NOT_SUPPORT_DIRNO ) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	dirTblIdx -= D_BF_DCF_DB_DIR_START_NUM;

	if(g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0] == 0) {
		/* not directory */
		return FJ_ERR_NG;
	}
	ercd = dcf_db_search_db_buf(g_bf_dcf_db_dir_tbl[dirTblIdx].dirIndex, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	objTblIdx = BF_Dcf_Db_Get_Obj_No(dosfile_name);
	if(objTblIdx == FSDCF_ERRNO_NOT_SUPPORT_FILENO) {
		return D_BF_MDF_WRAPPER_ERR_INVAL;
	}

	/* search delete file */
	if (g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName[0] != 0) {
		if ((g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].extType & 0xff) != 0) {
			/* allowed file to delete only the files */
			*pdir_index = g_bf_dcf_db_dir_tbl[dirTblIdx].dirIndex;
			*pobj_index = g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objIndex;
			return FSDCF_ERRNO_OK;
		}
	}
	/* not fount allowed file */
	return FJ_ERR_NG;
}


/**
 * @brief read attribute of object by both directory index and object index
 * @param i:FW_SHORT dir_index
 * @param i:FW_SHORT obj_index
 * @param o:FW_UCHAR* pobj_attr
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Read_Obj_Attr(FW_SHORT dir_index, FW_SHORT obj_index, FW_UCHAR* pobj_attr)
{
	FW_INT32 ercd;
	FW_SHORT bufTblIdx;
	FW_SHORT objTblIdx;

	/* check parameter */
	if(((FW_INT32)NULL) == dir_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	if(((FW_INT32)NULL) == obj_index) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}
	if(NULL == pobj_attr) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(0 == g_bf_dcf_db_dir_ctrl_block.dirCnt) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}
	if((DCF_DB_CURRENT_DIR != dir_index) && (g_bf_dcf_db_dir_ctrl_block.dirCnt < abs(dir_index))) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}

	ercd = dcf_db_search_db_buf(dir_index, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = dcf_db_search_obj_info_by_db(bufTblIdx, obj_index, &objTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	*pobj_attr = g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].attr;
	return FSDCF_ERRNO_OK;
}


/**
 * @brief get both directory index and object index by object name
 * @param i:const FW_CHAR* object_name
 * @param o:FW_SHORT *pdir_index
 * @param o:FW_SHORT* pobj_index
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Db_Get_Index_By_Object(const FW_CHAR* object_name, FW_SHORT* pdir_index, FW_SHORT* pobj_index)
{
	FW_INT32 ercd;
	FW_CHAR  dirNameTmp[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_CHAR  objNameTmp[D_BF_DCF_IF_OBJ_ONLY_NAME_MAX];
	FW_SHORT dirTblIdx;
	FW_SHORT objTblIdx;
	FW_SHORT bufTblIdx;

	/* check parameter */
	if(NULL == object_name) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(NULL == pdir_index) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(NULL == pobj_index) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(-1 == g_bf_dcf_db_current_db) {
		return D_BF_MDF_WRAPPER_ERR_NOTDIR;
	}

	memcpy(dirNameTmp, (FW_CHAR*)object_name, 8);
	dirNameTmp[8] = '\0';
	memcpy(objNameTmp, (FW_CHAR*)object_name + 9, 8);
	objNameTmp[8] = '\0';

	dirTblIdx = BF_Dcf_Db_Get_Dir_No(dirNameTmp);
	if(dirTblIdx == FSDCF_ERRNO_NOT_SUPPORT_DIRNO ) {
		return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
	}
	dirTblIdx -= D_BF_DCF_DB_DIR_START_NUM;
	if(0 != g_bf_dcf_db_dir_tbl[dirTblIdx].dirName[0]) {
		*pdir_index = g_bf_dcf_db_dir_tbl[dirTblIdx].dirIndex;
	}
	else {
		return D_BF_MDF_WRAPPER_ERR_INVAL;
	}
	ercd = dcf_db_search_db_buf(g_bf_dcf_db_dir_tbl[dirTblIdx].dirIndex, &bufTblIdx);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	objTblIdx = BF_Dcf_Db_Get_Obj_No(objNameTmp);
	if(objTblIdx == FSDCF_ERRNO_NOT_SUPPORT_FILENO) {
		return D_BF_MDF_WRAPPER_ERR_INVAL;
	}

	if(g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objName[0] != 0) {
		*pobj_index = g_bf_dcf_db_obj_tbl[bufTblIdx][objTblIdx].objIndex;
		return FSDCF_ERRNO_OK;
	}
	return D_BF_MDF_WRAPPER_ERR_INVAL;
}

/**
 * @brief Get directory path from full file type.
 * @param i:*full_file_path full file path
 * @param i:*dir_name directory file path
 * @return error code
 */
FW_INT32 BF_Dcf_Db_Get_Dir_Name(FW_CHAR* full_file_path, FW_CHAR* dir_name)
{
	FW_INT32 name_cnt;
	FW_USHORT root_dir_path_len;
	FW_CHAR full_file_path_tmp[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32 ercd;

	ercd = dcf_db_get_dcf_root_dir_len(full_file_path, &root_dir_path_len);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	strncpy(full_file_path_tmp, full_file_path, sizeof(full_file_path_tmp));

	// NULL check
	for (name_cnt = 0; name_cnt < root_dir_path_len + 8; name_cnt++) {
		if(full_file_path[name_cnt] == 0) {
			return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
		}
	}

	strncpy(dir_name, &full_file_path_tmp[root_dir_path_len], 8);
	dir_name[8] = 0;

	return 0;
}

/**
 * @brief Get directory path from full file type.
 * @param i:*full_file_path full file path
 * @param i:*file_name directory file path
 * @return error code
 */
FW_INT32 BF_Dcf_Db_Get_File_Path(FW_CHAR* full_file_path, FW_CHAR* file_name)
{
	FW_INT32 name_cnt;
	FW_USHORT root_dir_path_len;
	FW_CHAR full_file_path_tmp[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32 ercd;

	// Search DCF root directory name
	ercd = dcf_db_get_dcf_root_dir_len(full_file_path, &root_dir_path_len);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	strncpy(full_file_path_tmp, full_file_path, sizeof(full_file_path_tmp));

	// NULL check
	for (name_cnt = 0; name_cnt < root_dir_path_len + 21; name_cnt++) {
		if(full_file_path[name_cnt] == 0) {
			return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
		}
	}

	strncpy(file_name, &full_file_path_tmp[root_dir_path_len + 9], 12);
	file_name[12] = 0;

	return 0;
}

/**
 * @brief Get file number.
 * @param i:*fname DCF file name
 * @return 1..9999:DCF file number
 * @rerurn -103:not support file number
 */
FW_SHORT BF_Dcf_Db_Get_File_No(FW_CHAR* fname)
{
	FW_INT32 name_cnt;
	FW_SHORT fileno;

	if(fname == NULL) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}

	// length check
	if (strlen(fname) < 12){
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}

	// charactor check
	for (name_cnt = 0; name_cnt < 4; name_cnt++) {
		if ((!isdigit((FW_INT32)(fname[name_cnt]))) &&
			(!isalpha((FW_INT32)(fname[name_cnt]))) &&
			(fname[name_cnt] != '_')) {
			return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
		}
	}
	for (name_cnt = 4; name_cnt < 8; name_cnt++) {
		if(!isdigit((FW_INT32)(fname[name_cnt]))){
			return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
		}
	}
	if(fname[8] != '.') {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}

	// from charactor to number
	fileno = (fname[4] - '0') * 1000 + (fname[5] - '0') * 100
			 + (fname[6] - '0') * 10 + (fname[7] - '0');

	// check file number
	if ((fileno < D_BF_DCF_DB_MIN_FILE_NUM) || (fileno > D_BF_DCF_DB_MAX_FILE_NUM)) {
		return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
	}

	return fileno;
}


/**
 * @brief Get directory path from full dir pass.
 * @param i:*full_file_path full file path
 * @param i:*full_dir_path directory file path
 * @return error code
 */
FW_INT32 BF_Dcf_Db_Get_Full_Dir_Path(FW_CHAR* full_file_path, FW_CHAR* full_dir_path)
{
	FW_INT32 name_cnt;
	FW_USHORT root_dir_len;
	FW_INT32 ercd;

	ercd = dcf_db_get_dcf_root_dir_len(full_file_path, &root_dir_len);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	// NULL check
	for (name_cnt = 0; name_cnt < root_dir_len + 8; name_cnt++) {
		if(full_file_path[name_cnt] == 0) {
			return FSDCF_ERRNO_NOT_SUPPORT_DIRNO;
		}
	}

	strncpy(full_dir_path, full_file_path, root_dir_len + 8);
	full_dir_path[root_dir_len + 8] = 0;

	return 0;
}


