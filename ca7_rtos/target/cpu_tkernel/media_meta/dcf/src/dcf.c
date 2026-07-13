/**
 * @file		dcf.c
 * @brief		
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_dcf.h"
#include "fj_param.h"
#include "fj_playback.h"

#include "fsdcf_errno.h"
#include "dcf_if.h"
#include "mba_if.h"
#include "play_attr.h"
#include "debug.h"


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
/* Local Function												  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/**
* @brief 		DataBase of DCF is created. 
 * @param		VOID
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_dcfdcreate( VOID )
{
	FW_INT32 ercd;

	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_DCF, FJ_MBALOG_TYPE_START, "DcfdCreate" ));

	ercd = BF_Dcf_If_Create_DataBase();

	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_DCF, FJ_MBALOG_TYPE_END, "" ));

	if( ercd != 0 ){ // 0:NG
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}


/**
* @brief 		Renewal DcfDB with keeping current DirIndex.
 * @param		VOID
 * @return 		FJ_ERR_CODE
 * @note		None
 * @attention 	After this API called, set new ObjIndex by FJ_SetCurrentImage( ImageIndex ) if necessary.
 */
FJ_ERR_CODE fj_dcfdrenew( VOID )
{
	FJ_ERR_CODE	ercd;
	LONG CurDir;
	USHORT DirIndex;

	ercd = FJ_GetDCFDBLongVal( FJ_PARA_DCFDB_DIR_GET, DCF_DB_CURRENT_DIR << 16, &CurDir);     /* Save current DirIndex */
	if( ercd != FJ_ERR_OK ){
		return ercd;
	}
	DirIndex = CurDir >> 16;

	/*****************************/
	/* Excute DcfDB initialize   */
	/*****************************/
	ercd = FJ_DcfdCreate();		/* Re-create DcfDB */
	if( ercd != FJ_ERR_OK ){
		return ercd;
	}
	ercd = FJ_SetCurrentDirectory( DirIndex );		/* Set saved current DirIndex */
	if( ercd != FJ_ERR_OK ){
		return ercd;
	}
	return FJ_ERR_OK;
}

/**
 * @brief 		Create the directory. 
 * @param		I : USHORT direNum
 * @param		I : UCHAR dirNameIndex (0, 1, 2)
 * @return 		Success/Fail
 * @note		None
 * @attention 	current directory will be changed to created directory.
 */
FJ_ERR_CODE fj_directdcfdirectorycreate( USHORT dirNum, UCHAR dirNameIndex )
{
	FW_INT32 ercd;

	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_DCF, FJ_MBALOG_TYPE_START, "SetCurrentDir" ));

	ercd = BF_Dcf_If_Create_Dir((FW_USHORT)dirNum, (FW_UCHAR)dirNameIndex);

	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_DCF, FJ_MBALOG_TYPE_END, "" ));

	if(0 != ercd) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}


/**
* @brief 		The number of total directry in "DCIM" can be acquired. 
 * @param		o:USHORT* count
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_directdcfdirnum( USHORT* count )
{
	FW_INT32 ercd;

	ercd = BF_Dcf_If_Read_Dir_Total_Count((FW_SHORT*)count);
	if(0 != ercd) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}


/**
 * @brief 		Get the attribute of directory on DCF DataBase
 * @param		i:FW_SHORT dir_index
 * @param		o:USHORT* count
 * @param		o:CHAR* dirName
 * @param		o:USHORT* date
 * @param		o:USHORT* time
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_directdcfdumpDiriattr( SHORT dir_index, USHORT* count, CHAR* dirName, USHORT* date, USHORT* time )
{
	FW_INT32 ercd;
	T_BF_DCF_IF_DB_INFO ret_data;
	T_BF_DCF_IF_FILE_INDEX file_index;

	memset(&ret_data, 0, sizeof(T_BF_DCF_IF_DB_INFO));

	file_index.index = 0;
	file_index.pack.dir_index = dir_index;

	ret_data.p_objectCnt = (FW_SHORT*)count;
	ret_data.dirName = (FW_CHAR*)dirName;
	ret_data.p_date = date;
	ret_data.p_time = time;

	ercd = BF_Dcf_If_Get_Obj_Info(file_index.index, &ret_data);
	if(0 != ercd) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief 		Get the attribute of directory on DCF DataBase
 * @param		i:FW_SHORT dir_index
 * @param		o:USHORT* count
 * @param		o:CHAR* dirName
 * @param		o:USHORT* date
 * @param		o:ULONG* time
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_directdcfdumpdiriattrex( SHORT dir_index, USHORT* count, CHAR* dirName, USHORT* date, ULONG* time )
{
	FW_INT32 ercd;
	T_BF_DCF_IF_DB_INFO ret_data;
	T_BF_DCF_IF_FILE_INDEX file_index;
	FW_USHORT cre_time;
	FW_UCHAR sec10m;
	FW_ULONG cre_sec;

	memset(&ret_data, 0, sizeof(T_BF_DCF_IF_DB_INFO));

	file_index.index = 0;
	file_index.pack.dir_index = dir_index;

	ret_data.p_objectCnt = (FW_SHORT*)count;
	ret_data.dirName = (FW_CHAR*)dirName;
	ret_data.p_date = date;
	ret_data.p_time = &cre_time;
	ret_data.p_sec10m = (FW_UCHAR*)&sec10m;

	ercd = BF_Dcf_If_Get_Obj_Info(file_index.index, &ret_data);
	if(0 != ercd) {
		// Error of object information acquisition
		return FJ_ERR_NG;
	}

	cre_sec = ((cre_time & 0x001F) * 2) + (sec10m / 100);
	*time = ((cre_time & 0xFFE0) << 1) | cre_sec;

	return FJ_ERR_OK;
}

/**
 * @brief 		Get the count of allowed files in the object 
 * @param		i:SHORT obj_index
 * @param		o:UCHAR* count
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getallowdnum( SHORT obj_index, UCHAR* count )
{
	FW_INT32 ercd;
	FW_LONG file_index;

	file_index = (DCF_DB_CURRENT_DIR << 16) | obj_index;
	ercd = BF_Dcf_If_Read_Allowed_Ext_Count(file_index, (FW_UCHAR*)count);
	if(0 != ercd) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}


/**
 * @brief		Get long data from DCF-DB
 * @param		I : USHORT ParamID
 * @param		I : LONG LongPrm
 * @param		O : LONG* value
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getdcfdblongval( USHORT ParamID, LONG LongPrm, LONG* value )
{
	FW_INT32 ret;
	FW_SHORT wVal;
	T_BF_DCF_IF_DCFDB_OBJ_INFO obj_info;
	FW_USHORT dir_created_date;
	FW_USHORT dir_created_time;

	switch(ParamID) {
		case FJ_PARA_DCFDB_DIR_TOTAL_CNT_GET:
			ret = BF_Dcf_If_Read_Dir_Total_Count(&wVal);
			*value = (LONG)wVal;
			break;

		case FJ_PARA_DCFDB_DIR_GET:
			ret = BF_Dcf_If_Get_Dir_Index(LongPrm, value);
			break;

		case FJ_PARA_DCFDB_OBJ_CNT_GET:
			ret = BF_Dcf_If_Read_Obj_Count(LongPrm, &wVal);
			*value = (LONG)wVal;
			break;

		case FJ_PARA_DCFDB_ATTR_GET:
			ret = BF_Dcf_If_Read_Obj_Info(LongPrm, &obj_info);
			*value = (LONG)(obj_info.attr);
			break;

		case FJ_PARA_DCFDB_DATE_TIME_GET:
			ret = BF_Dcf_If_Read_Dir_Date_Time(LongPrm, &dir_created_date, &dir_created_time);
			*value = (((LONG)dir_created_date) << 16)
					 | ((LONG)dir_created_time);
			break;

		default:
			return FJ_ERR_NG;
//			break;
	}

	if(0 != ret) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}


/**
 * @brief		Get DCF Information.
 * @param		I : INT32 ImageIndex
 *				O : INT32* DirNum
 *				O : INT32* FileNum
 *				O : INT32* FileType
 *				O : INT32* Locked
 *				O : INT32* FileDupStatus
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @attention 	None
 */
FJ_ERR_CODE fj_getdcfinfo( INT32 ImageIndex, INT32* DirNum, INT32* FileNum, SHORT* FileType, INT32* Locked, INT32* FileDupStatus )
{
	INT32 ret;
	T_BF_DCF_IF_FILE_INFO file_info;

	ret = BF_Dcf_If_Get_File_Info( ImageIndex, &file_info );

	if( ret != FSDCF_ERRNO_OK ){
		return FJ_ERR_NG;
	}

	*DirNum = file_info.dir_no;
	*FileNum = file_info.file_no;
	*Locked = file_info.file_attr;
	*FileDupStatus = file_info.file_dupstatus;

#if 1
	*FileType = file_info.file_type;
#else
	if( file_info.option_file == E_BF_DCF_IF_OPTION_FILE_EXIST ){
		*FileType = file_info.file_type | FJ_FILE_TYPE_WAV;
	}
	else{
		*FileType = file_info.file_type;
	}
#endif

#if 0
#ifdef FJ_DEBUG_PRINT_ON
	BF_Debug_Print_Information(( "filename = %s \r\n", file_info.file_char ));
#endif
#endif

	return FJ_ERR_OK;
}


/**
 * @brief 		Get the Duplicate Status of the directory. 
 * @param		i:FW_SHORT dir_index
 * @param		o:CHAR* dirdupStatus
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getdirduplicatestatus( SHORT dir_index, CHAR* dirdupStatus )
{
	FW_INT32 ercd;

	FW_LONG file_index;

	file_index = ((FW_LONG)dir_index) << 16;
	ercd = BF_Dcf_If_Read_Dir_Duplicate_Status(file_index, (FW_CHAR*)dirdupStatus);
	if(0 != ercd) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}


/**
 * @brief 		Get the information of the directory. 
 * @param		i:SHORT dir_index
 * @param		o:CHAR* dirName
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getdirinfo( SHORT dir_index, CHAR* dirName )
{
	FW_INT32 ercd;

	FW_LONG file_index;

	file_index = ((FW_LONG)dir_index) << 16;
	ercd = BF_Dcf_If_Read_Dir_Name(file_index, (FW_CHAR*)dirName);
	if(0 != ercd) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}


/**
 * Get max file number as directory number
 *
 * @param file_no 0: Not duplicate, 1~999: Max duplication number
 * @return
 * @note
 *
 *   case 1: This API return "0".
 *      101SOCIO ... this is directory
 *
 *   case 2: This API return "102".
 *      101SOCIO ... this is file
 *      102SOCIO ... this is file
 *      103SOCIO ... this is directory
 *
 *   case 3: This API return "104".
 *      101SOCIO ... this is file
 *      102SOCIO ... this is file
 *      103SOCIO ... this is directory
 *      104SOCIO ... this is file
 *
 *   case 4: This API return "105".
 *      101SOCIO ... this is file
 *      102SOCIO ... this is file
 *      103SOCIO ... this is directory
 *      104SOCIO ... this is file
 *      105SOCIO ... this is file
 *      106SOCIO ... this is directory
 */
FJ_ERR_CODE fj_getmaxfilenumasdirnum( USHORT* file_no )
{
	FW_INT32 ercd;

	ercd = BF_Dcf_If_Get_Max_File_Num_As_Dir_Num((FW_SHORT*)file_no);
	if(0 != ercd) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}


/**
 * @brief		Get DCF of directory and file free chars
 * @param		I : FJ_DCF_FREE_CHARS ParamID
 *				O : CHAR* Offset
 *				I : USHORT Size
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getparamarray( FJ_DCF_FREE_CHARS ParamID, CHAR* Offset, USHORT Size )
{
	INT32					ercd;
	FW_CHAR					file_dir_path[32] = {'\0'};
	INT32					file_index;
	T_BF_DCF_IF_FILE_INDEX	fileindex;
	
	switch( ParamID ){
		case FJ_DCF_CUR_DIR_FREE_CHARS:
			// get current directory chars
			file_index = BF_Play_Attr_Get_Image_Index();
			ercd = BF_Dcf_If_Get_Index_File_Path(file_index, NULL, (FW_CHAR*)file_dir_path);
			if(ercd != 0){
				return FJ_ERR_NG;
			}
			memcpy( (char*)Offset, &file_dir_path[11], 5 );
			break;

		case FJ_DCF_CUR_FILE_FREE_CHARS:
			// get current file chars
			file_index = BF_Play_Attr_Get_Image_Index();
			ercd = BF_Dcf_If_Get_Index_File_Path(file_index, NULL, (FW_CHAR*)file_dir_path);
			if(ercd != 0){
				return FJ_ERR_NG;
			}
			memcpy( (char*)Offset, &file_dir_path[17], 4 );
			break;

		case FJ_DCF_LAST_DIR_FREE_CHARS:
			// get last directory chars
			fileindex.index =0;
//			file_index = BF_Dcf_If_Get_Entry(E_BF_MEDIA_STATE_MEDIA_TYPE_CUR);
			fileindex.pack.dir_index = -1;
			fileindex.pack.obj_index = 0;
//			ercd = BF_Dcf_If_Get_Index_File_Path(file_index, NULL, (FW_CHAR*)file_dir_path);
			ercd = BF_Dcf_If_Read_Dir_Name(fileindex.index, (FW_CHAR*)file_dir_path);
			if(ercd != 0){
				return FJ_ERR_NG;
			}
//			memcpy( (char*)Offset, &file_dir_path[11], 5 );
			memcpy( (char*)Offset, &file_dir_path[3], 5 );
			break;

		case FJ_DCF_LAST_FILE_FREE_CHARS:
			// get last file chars
			fileindex.index =0;
//			file_index = BF_Dcf_If_Get_Entry(E_BF_MEDIA_STATE_MEDIA_TYPE_CUR);
			fileindex.pack.dir_index = DCF_DB_CURRENT_DIR;
			fileindex.pack.obj_index = -1;
//			file_index = fileindex.index;
//			ercd = BF_Dcf_If_Get_Index_File_Path(file_index, NULL, (FW_CHAR*)file_dir_path);
			ercd = BF_Dcf_If_Read_Obj_Name(fileindex.index, (FW_CHAR*)file_dir_path);
			if(ercd != 0){
				return FJ_ERR_NG;
			}
			memcpy( (char*)Offset, file_dir_path, 4 );
			break;

		default:
			return FJ_ERR_NG;
	}

	if( *Offset == '\0' ){
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;


}


/**
 * @brief		Check file type
 * @param		I: const FW_CHAR* ext_name
 * @param		I: FW_LONG file_type
 * @return 		True : Yes, ext_name found in file_type.
 *				False : No, ext_name does NOT belong to file_type.
 * @note 		None
 * @attention 	None
 */
BOOL fj_is_dcf_file_type(const CHAR* ext_name, SHORT file_type )
{
	return BF_Dcf_If_Is_File_Type((const FW_CHAR*)ext_name, (FW_SHORT)file_type);
}


/**
* @brief 		set current directory to DCF-DB. 
 * @param		I : SHORT dir_index
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_setcurrentdirectory( SHORT dir_index )
{
	FW_INT32 ercd;
	T_BF_DCF_IF_FILE_INDEX file_index;

	file_index.index = 0;
	file_index.pack.dir_index = dir_index;

#if 1
	ercd = BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, file_index.index);
#else
	ercd = BF_Dcf_If_Set_Current_Dir(BF_Dcf_If_Entry_Dummy, D_BF_DCF_IF_PRM_NORMAL_ACTION, file_index.index);
#endif
	if(ercd != 0){
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}


/**
 * @brief		Set long data to DCF-DB
 * @param		I : USHORT ParamID
 * @param		I : LONG LongPrm
 * @param		I : LONG value
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_setdcfdblongval( USHORT ParamID, LONG LongPrm, LONG value )
{
	FW_INT32 ret;

	switch(ParamID) {
		case FJ_PARA_DCFDB_ATTR_SET:
			ret = BF_Dcf_If_Write_Obj_Attr(LongPrm, (FW_UCHAR)value);
			break;

		case FJ_PARA_DCFDB_DIR_CURRENT_SET:
#if 1
			ret = BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, LongPrm);
#else
			ret = BF_Dcf_If_Set_Current_Dir(BF_Dcf_If_Entry_Dummy, D_BF_DCF_IF_PRM_NORMAL_ACTION, LongPrm);
#endif
			break;

		default:
			return FJ_ERR_NG;
//			break;
	}

	if(0 != ret) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}


/**
 * @brief 		sets the free characters used for new DCF files and directories.
 * @param		I : CHAR Dir
 *				I : FJ_DCF_FREE_CHARS_OPERATION Operation
 *				I : CHAR Chars[]
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_setdcffreechars( CHAR Dir, FJ_DIR_FILE_OPERATION Operation, CHAR* Chars )
{
	INT32 ret;

	ret = 0;
	if( Dir == 0){ //sets_the_free_characters_for_file
		switch( Operation) {
			case FJ_FILE_SET_ONLY:
				ret = BF_Dcf_If_Set_Free_File_Name_As( NULL, Dir, (FW_CHAR*)Chars, 0);
#ifdef FJ_DEBUG_PRINT_ON
				BF_Debug_Print_Information(( "BF_Dcf_If_Set_Free_File_Name_As( file ) end(%d) \r\n", ret ));
#endif
				break;

			case FJ_FILE_SET_ALLOWED:
				// sets the allowed DCF extensions
				ret = BF_Dcf_If_Set_Allowed_Ext_List( (const FW_CHAR*)Chars );
				break;

			default:
				return FJ_ERR_NG;
		}

		if(ret != 0){
			return FJ_ERR_NG;
		}
	}
	else if( Dir == 1){ //sets_the_free_characters_for_directory

		// set the free charcters from Chars[]
		switch( Operation ){
			case FJ_DIR_SET_ONLY:
				// only sets the directory free chars
				ret = BF_Dcf_If_Set_Free_File_Name_As( NULL, Dir, (FW_CHAR*)Chars, 0);
				break;

			default:
				return FJ_ERR_NG;
		}
		if(ret != 0){
			return FJ_ERR_NG;
		}
	}
	else {
		; // DO NOTHING
	}

	return FJ_ERR_OK;
}


/**
 * @brief		Get DCF of directory and file free number
 * @param		I : INT32 ParamID
 *				O : LONG* value
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE BF_Dcf_Get_Param_Long( INT32 ParamID, LONG* value )
{
	USHORT dirno, fileno;

	// value 18MSB = directory number
	// value 14LSB = file number
	switch( ParamID ){
		case FJ_PARA_DCF_LAST_KEY: // FALL THROUGH
		case FJ_PARA_DCF_DUPLICATE: // FALL THROUGH
		case FJ_PARA_DCF_HIDDEN: // FALL THROUGH
		case FJ_PARA_DCF_ASSIGN_LAST_KEY: // FALL THROUGH
		case FJ_PARA_DCF_ASSIGN_DUPLICATE: // FALL THROUGH
		case FJ_PARA_DCF_ASSIGN_HIDDEN:
			BF_Dcf_If_Get_Dcf_Key(ParamID, &dirno, &fileno);

#if 0
			BF_Dcf_If_Get_Series_File_No( &dirno, &fileno, E_BF_DCF_IF_LATEST_TYPE_ALL );
#endif

			if( (dirno > FJ_CUSTOM_DCFDB_DIRNUM_MAX) && (fileno > FJ_CUSTOM_DCFDB_FILENUM_MAX) ){
				return FJ_ERR_NG;
			}
			else if( (dirno == FJ_CUSTOM_DCFDB_DIRNUM_MIN) && (fileno == FJ_CUSTOM_DCFDB_FILENUM_MIN) ){
				*value = 0xffffffff;

				return FJ_ERR_OK;
			}
			else {
				; // DO NOTHING
			}

			*value = ( ((LONG)dirno << 16) | (LONG)fileno );

#ifdef FJ_DEBUG_PRINT_ON
			BF_Debug_Print_Information(( "last dirno is %d \r\n", dirno));
			BF_Debug_Print_Information(( "last fileno is %d \r\n", fileno ));
#endif
			break;

		default:
			return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 * @brief		Get DCF Latest Index Number in current directory
 * @param		O : INT32* Index
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_get_latest_index( INT32* Index )
{
	*Index = BF_Dcf_If_Get_Entry(E_BF_MEDIA_STATE_MEDIA_TYPE_CUR);

	if( *Index < 0 ){
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}


/**
 * @brief		Get Last File Number.
 * @param		I : SHORT dir_index
 *				O : USHORT* file_num
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @attention 	BF_Dcf_Db_Set_Current_Dir()
 */
FJ_ERR_CODE fj_getdcflastfilenumber( SHORT dir_index, USHORT* file_num )
{
	INT32 ret;
	FW_LONG file_index;

	file_index = ((FW_LONG)dir_index) << 16;
	ret = BF_Dcf_If_Get_Last_File_Number( file_index, file_num );

	if( ret != FSDCF_ERRNO_OK ){
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}


/**
 * @brief		Get first free File number in current directory.
 * @param		O : USHORT* fileNum
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE	fj_getfirstfreefileno( USHORT* fileNum )
{
	FW_INT32 ercd;

	ercd = BF_Dcf_If_Get_First_Free_File_No((FW_USHORT*)fileNum);

	if( ercd != 0){
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}


/**
 * @brief 		sets the DCF numbers used for new DCF files and directories
 * @param		I : USHORT DirNum
 *				I : USHORT FileNum
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_setnextdcfkey( USHORT DirNum, USHORT FileNum )
{
//	T_BF_DCF_IF_FILE_NAME number;
	INT32 ercd;


	if( (DirNum > FJ_CUSTOM_DCFDB_DIRNUM_MAX) || (DirNum < FJ_CUSTOM_DCFDB_DIRNUM_MIN) 
		|| (FileNum > FJ_CUSTOM_DCFDB_FILENUM_MAX) || (FileNum <= FJ_CUSTOM_DCFDB_FILENUM_MIN) ){
		return FJ_ERR_NG;
	}

//	number.file_name = E_BF_DCF_IF_FILE_NAME_RESET;
//	number.dirno = DirNum;
//	number.fileno = FileNum;

//	ercd = BF_Dcf_Controller_Set_File_Name(number);
	ercd = BF_Dcf_If_Set_Series_File_No(NULL, DirNum, FileNum);

	if( ercd == FSDCF_ERRNO_OK ){
		return FJ_ERR_OK;
	}
	else{
		return FJ_ERR_NG;
	}
}



#if 1	// This may NOT need. Becase other function CAN support this.
/**
 * @brief		Set string data to DCF-DB.
 * @param		I : USHORT ParamID
 *				I : CHAR* Sring
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_setdcfdbstrval( USHORT ParamID, CHAR* Sring )
{
	FW_INT32 ret;
	FW_LONG index;

	switch(ParamID) {
		case FJ_PARA_DCFDB_DIR_BASE_SET_STR:
			ret = BF_Dcf_If_Set_Dir_Db(NULL, (const FW_CHAR*)Sring);
			break;

		default:
			ret = BF_Dcf_If_Get_Index_By_Dir_Name((const FW_CHAR*)Sring, &index);
			if(0 != ret) {
				return FJ_ERR_NG;
			}
#if 1
			ret = BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, index);
#else
			ret = BF_Dcf_If_Set_Current_Dir(BF_Dcf_If_Entry_Dummy, D_BF_DCF_IF_PRM_NORMAL_ACTION, index);
#endif
			break;
	}
	if(0 != ret) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}
#endif




/**
 * @brief		Get string data from DCF-DB
 * @param		I : USHORT ParamID
 * @param		I : LONG LongPrm
 * @param		O : CHAR* Sring
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getdcfdbstrval( USHORT ParamID, LONG LongPrm, CHAR* Sring )
{
	FW_INT32 ret = FJ_ERR_NG;

	switch(ParamID) {
		case FJ_PARA_DCFDB_DIR_GET_STR:
			ret = BF_Dcf_If_Read_Dir_Name(LongPrm, (FW_CHAR*)Sring);
			break;

		case FJ_PARA_DCFDB_OBJ_GET_STR:
			ret = BF_Dcf_If_Read_Obj_Name(LongPrm, (FW_CHAR*)Sring);
			break;

		case FJ_PARA_DCFDB_DIR_OBJ_GET_STR:
			ret = BF_Dcf_If_Read_Dir_Obj_Name(LongPrm, (FW_CHAR*)Sring);
			break;

		case FJ_PARA_DCFDB_EXT_GET_STR:
			ret = BF_Dcf_If_Get_Ext_Name_By_Type((FW_UCHAR)LongPrm, (FW_CHAR*)Sring);
			break;

		case FJ_PARA_DCFDB_FULL_PATH_GET_STR:
			ret = BF_Dcf_If_Get_Full_Path_By_File_Index( LongPrm, (FW_CHAR*)Sring);
			break;

		default:
			return FJ_ERR_NG;
	}

	if(0 != ret) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}


/**
 * @brief		Get long data from DCF-DB
 * @param		I : USHORT ParamID
 * @param		I : CHAR* Sring
 * @param		O : LONG* value
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getdcfdblongvalbystr( USHORT ParamID, CHAR* Sring, LONG* value )
{
	FW_INT32 ret;
	FW_UCHAR ext_type;

	switch(ParamID) {
		case FJ_PARA_DCFDB_DIR_GET_BY_STR:
			ret = BF_Dcf_If_Get_Index_By_Dir_Name((const FW_CHAR*)Sring, value);
			break;

		case FJ_PARA_DCFDB_DIR_OBJ_GET_BY_STR:
			ret = BF_Dcf_If_Get_Index_By_Obj_Name((const FW_CHAR*)Sring, value);
			break;

		case FJ_PARA_DCFDB_EXT_GET_BY_STR:
			ret = BF_Dcf_If_Get_Type_By_Ext_Name((const FW_CHAR*)Sring, &ext_type);
			if(0 == ext_type) {
				*value = 0x00010000;
			}
			else {
				*value = (LONG)ext_type;
			}
			break;

		default:
			return FJ_ERR_NG;
//			break;
	}

	if(0 != ret) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}



#if 0
/**
 * @brief 		Lock/Unlock all file in one DCF folder.
 * @param		USHORT dir_index
 * @param		INT32 fs_attr
 * @return 		Success/Fail
 * @note		None
 * @attention 	valid after BF_DCFIF_DB_set_Current_Dir()
 */
FJ_ERR_CODE fj_directdcfsetlockstatusallindirnumber( USHORT dir_index, INT32 fs_attr )
{
	INT32 ret;
	FW_LONG cur_dir_index;

	ret = BF_Dcf_If_Get_Dir_Index( DCF_DB_CURRENT_DIR << 16, &cur_dir_index );
	if( ret != E_OK ) {
		BF_Debug_Print_Error(( "BF_Dcf_If_Get_Dir_Index err. errno=%d\n", ret ));
		return FJ_ERR_NG;
	}
	if( dir_index != (cur_dir_index >> 16) ){
		BF_Debug_Print_Error(( "FJ_DirectDcfSetLockStatusAllinDirNumber() dir_index err. dir_index=%d fs_attr=%d\n", dir_index, fs_attr ));
		return FJ_ERR_NG;
	}
	if( fs_attr > FJ_FS_ATTR_READONLY ){
		BF_Debug_Print_Error(( "FJ_DirectDcfSetLockStatusAllinDirNumber() parameter err.  dir_index=%d fs_attr=%d\n", dir_index, fs_attr );
		return FJ_ERR_NG;
	}
	ret = BF_Dcf_If_Set_All_Obj_Attr_In_Current_Dir( E_BF_MEDIA_STATE_MEDIA_TYPE_CUR, (E_BF_DCF_IF_ATTR)fs_attr );
	if( ret != E_OK ) {
		BF_Debug_Print_Error(( "BF_Dcf_If_Set_All_Obj_Attr_In_Current_Dir err. errno=%d\n", ret ));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}


/**
 * @brief 		Delete all file in one DCF folder.
 * @param		USHORT dir_index 
 * @return 		Success/Fail
 * @note		None
 * @attention 	valid after BF_DCFIF_DB_set_Current_Dir()
 */
FJ_ERR_CODE fj_directdcfdeleteallindirnumber( USHORT dir_index )
{
	INT32 ret;
	FW_LONG cur_dir_index;

	ret = BF_Dcf_If_Get_Dir_Index( DCF_DB_CURRENT_DIR << 16, &cur_dir_index );
	if( ret != E_OK ) {
		BF_Debug_Print_Error(( "BF_Dcf_If_Get_Dir_Index err. errno=%d\n", ret ));
		return FJ_ERR_NG;
	}
	if( dir_index != (cur_dir_index >> 16) ){
		BF_Debug_Print_Error(( "FJ_DirectDcfDeleteAllinDirNumber() dir_index err. dir_index=%d\n", dir_index ));
		return FJ_ERR_NG;
	}
	ret = BF_Dcf_If_Del_All_Obj_In_Current_Dir( E_BF_MEDIA_STATE_MEDIA_TYPE_CUR );
	if( ret != E_OK ) {
		BF_Debug_Print_Error(( "BF_Dcf_If_Del_All_Obj_In_Current_Dir err. errno=%d\n", ret ));
		return FJ_ERR_NG;
	}

	ret = BF_Dcf_If_Set_Current_Dir( NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, cur_dir_index );
	if( ret != E_OK ) {
		BF_Debug_Print_Error(( "BF_Dcf_If_Set_Current_Dir err. errno=%d\n", ret ));
		return FJ_ERR_NG;
	}
	BF_Play_Common_Reset_Current_Data(); /* Global data Clear */

	return FJ_ERR_OK;
}

#endif


FJ_ERR_CODE fj_dcf_execute_multi_delete(VOID)
{
	FW_INT32 ercd;

	ercd = BF_Dcf_If_Execute_Multi_Delete();
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_dcf_init_select_list(VOID)
{
	FW_INT32 ercd;

	ercd = BF_Dcf_If_Init_Select_List();
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_dcf_add_file_to_select_list(INT32 FileIndex)
{
	FW_INT32 ercd;

	ercd = BF_Dcf_If_Add_File_To_Select_List((FW_LONG)FileIndex);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_dcf_remove_file_from_select_list(INT32 FileIndex)
{
	FW_INT32 ercd;

	ercd = BF_Dcf_If_Remove_File_From_Select_List((FW_LONG)FileIndex);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief 		Get file exist in the directory.
 * @param		i:FW_SHORT dir_index
 * @param		o:UCHAR* file_exist
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE fj_getdcfdirfileexist( USHORT dir_index, UCHAR* file_exist )
{
	FW_INT32 ercd;

	ercd = BF_Dcf_If_Get_Dir_File_Exist(dir_index, (FW_UCHAR*)file_exist);
	if(0 != ercd) {
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief 		DCF directory delete 
 * @param		i:USHORT ParamID
 * @param		i:USHORT dir_index
 * @return 		Success/Fail
 * @note		None
 * @attention 	After this function call, please set the target directory to current directory by FJ_SetCurrentDirectory().
 */
FJ_ERR_CODE fj_dcfdirectorydelete(USHORT ParamID, USHORT dir_index)
{
	FW_INT32 ercd;

	switch(ParamID) {
		case FJ_PARA_DCFDB_SELECT_DIR:
			ercd = BF_Dcf_If_Del_Single_Dcf_Dir(dir_index);
			break;

		case FJ_PARA_DCFDB_ALL_EMPTY_DIR:
			ercd = BF_Dcf_If_Del_All_Dcf_Dir();
			break;

		default:
			return FJ_ERR_NG;
	}

	if(0 != ercd) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

#if 1
/**
 * @brief		specify the extension and get a new file path from DCF-DB.
 * @param		I : CHAR* ext
 * @param		O : CHAR* new_file_path
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_dcf_make_new_file_path_by_ext( const CHAR* ext, CHAR* new_file_path )
{
	FW_INT32 ret;

	ret = BF_Dcf_If_Make_New_File_Path((const FW_CHAR*)ext, (FW_CHAR*)new_file_path);
	
	if(0 != ret) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}


/**
 * @brief 		Delete a file the specified file index.
 * @param		i:INT32 file_index
 * @return 		Success/Fail
 * @note		None
 * @attention 	None.
 */
FJ_ERR_CODE fj_dcf_del_single_obj( INT32 file_index )
{
	FW_INT32 ercd;
	
	ercd = BF_Dcf_If_Del_Single_Obj( file_index );

	if(0 != ercd) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}


/**
 * @brief 		Delete all files in the specified media.
 * @param		i:INT32 media_id
 * @return 		Success/Fail
 * @note		None
 * @attention 	SD1 only support
 */
FJ_ERR_CODE fj_dcf_del_all_obj( INT32 media_id )
{
	INT32 ercd;
	
	if ( media_id == FJ_MEDIA_SD1 ){
		ercd = BF_Dcf_If_Del_All_Obj( E_BF_MEDIA_STATE_MEDIA_TYPE_SD1 );
	}
	else{
		printf("media_id error \n");
		ercd = FJ_ERR_NG;
	}
	
	if(0 != ercd) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}


/**
 * @brief		Update the DCF-DB by adding new file.
 * @param		I : CHAR* filepath
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE fj_dcf_update_dcfdb_add_file( CHAR* file_path )
{
	INT32 ret;

	ret = BF_Dcf_If_Updata_File(0, file_path);

	if(0 != ret) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}
#endif 


