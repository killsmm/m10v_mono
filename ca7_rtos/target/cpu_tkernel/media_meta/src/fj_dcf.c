/**
 * @file		fj_dcf.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_dcf.h"
#include "mode.h"
#include "dcf.h"

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
FJ_ERR_CODE FJ_DcfdCreate( VOID )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_dcfdcreate();

	return err_code;

}


/**
* @brief 		Renewal DcfDB with keeping current DirIndex.
 * @param		VOID
 * @return 		FJ_ERR_CODE
 * @note		None
 * @attention 	After this API called, set new ObjIndex by FJ_SetCurrentImage( ImageIndex ) if necessary.
 */
FJ_ERR_CODE FJ_DcfdRenew( VOID )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_dcfdrenew();

	return err_code;
}

/**
 * @brief 		Create the directory. 
 * @param		I : USHORT direNum
 * @param		I : UCHAR dirNameIndex (0)
 * @return 		Success/Fail
 * @note		None
 * @attention 	current directory will be changed to created directory.
 */
FJ_ERR_CODE FJ_DirectDcfDirectoryCreate( USHORT dirNum, UCHAR dirNameIndex )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_directdcfdirectorycreate( dirNum, dirNameIndex );

	return err_code;
}


/**
* @brief 		The number of total directry in "DCIM" can be acquired. 
 * @param		o:USHORT* count
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_DirectDcfDirNum( USHORT* count )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_directdcfdirnum( count );

	return err_code;
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
FJ_ERR_CODE FJ_DirectDcfDumpDirIAttr( SHORT dir_index, USHORT* count, CHAR* dirName, USHORT* date, USHORT* time )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_directdcfdumpDiriattr( dir_index, count, dirName, date, time );

	return err_code;
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
FJ_ERR_CODE FJ_DirectDcfDumpDirIAttrEx( SHORT dir_index, USHORT* count, CHAR* dirName, USHORT* date, ULONG* time )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_directdcfdumpdiriattrex( dir_index, count, dirName, date, time );

	return err_code;
}

/**
 * @brief 		Get the count of allowed files in the object 
 * @param		i:SHORT obj_index
 * @param		o:UCHAR* count
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_GetAllowdNum( SHORT obj_index, UCHAR* count )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getallowdnum( obj_index, count );

	return err_code;
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
FJ_ERR_CODE FJ_GetDCFDBLongVal( USHORT ParamID, LONG LongPrm, LONG* value )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getdcfdblongval( ParamID, LongPrm, value );

	return err_code;
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
FJ_ERR_CODE FJ_GetDCFInfo( INT32 ImageIndex, INT32* DirNum, INT32* FileNum, SHORT* FileType, INT32* Locked, INT32* FileDupStatus )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getdcfinfo( ImageIndex, DirNum, FileNum, FileType, Locked, FileDupStatus );

	return err_code;
}


/**
 * @brief 		Get the Duplicate Status of the directory. 
 * @param		i:FW_SHORT dir_index
 * @param		o:CHAR* dirdupStatus
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_GetDirDuplicateStatus( SHORT dir_index, CHAR* dirdupStatus )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getdirduplicatestatus( dir_index, dirdupStatus );

	return err_code;
}


/**
 * @brief 		Get the information of the directory. 
 * @param		i:SHORT obj_index
 * @param		o:CHAR* dirName
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_GetDirInfo( SHORT dir_index, CHAR* dirName )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getdirinfo( dir_index, dirName );

	return err_code;
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
FJ_ERR_CODE FJ_GetMaxFileNumAsDirNum( USHORT* file_no )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getmaxfilenumasdirnum( file_no );

	return err_code;
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
FJ_ERR_CODE FJ_GetParamArray( FJ_DCF_FREE_CHARS ParamID, CHAR* Offset, USHORT Size )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getparamarray( ParamID, Offset, Size );

	return err_code;
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
BOOL FJ_Is_DCF_File_Type(const CHAR* ext_name, SHORT file_type )
{
	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return 0; // error return
	}

	return fj_is_dcf_file_type( ext_name, file_type );
}


/**
* @brief 		set current directory to DCF-DB. 
 * @param		I : SHORT dir_index
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_SetCurrentDirectory( SHORT dir_index )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_setcurrentdirectory( dir_index );

	return err_code;
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
FJ_ERR_CODE FJ_SetDCFDBLongVal( USHORT ParamID, LONG LongPrm, LONG value )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_setdcfdblongval( ParamID, LongPrm, value );

	return err_code;
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
FJ_ERR_CODE FJ_SetDCFFreeChars( CHAR Dir, FJ_DIR_FILE_OPERATION Operation, CHAR* Chars )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_BASE_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_setdcffreechars( Dir, Operation, Chars );

	return err_code;
}


/**
 * @brief		Get Last File Number.
 * @param		I : SHORT dir_index
 *				O : USHORT* file_num
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @attention 	None
 */
//FJ_ERR_CODE FJ_GetDCFLastFileNumber( FW_SHORT dir_index, FW_USHORT* file_num )
FJ_ERR_CODE FJ_GetDCFLastFileNumber( SHORT dir_index, USHORT* file_num )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_VIEW_CODE | D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getdcflastfilenumber( dir_index, file_num );

	return err_code;
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
FJ_ERR_CODE FJ_SetNextDCFKey( USHORT DirNum, USHORT FileNum )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_VIEW_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_setnextdcfkey( DirNum, FileNum );

	return err_code;
}




/**
 * @brief		Get DCF Latest Index Number in current directory
 * @param		O : INT32* Index
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_Get_Latest_Index( INT32* Index )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_VIEW_CODE | D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_get_latest_index( Index );

	return err_code;
}


/**
 * @brief		Get first free File number in current directory.
 * @param		O : USHORT* fileNum
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE	FJ_GetFirstFreeFileNo( USHORT* fileNum )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_VIEW_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getfirstfreefileno( fileNum );

	return err_code;
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
FJ_ERR_CODE FJ_SetDCFDBStrVal( USHORT ParamID, CHAR* Sring )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_setdcfdbstrval( ParamID, Sring );

	return err_code;
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
FJ_ERR_CODE FJ_GetDCFDBStrVal( USHORT ParamID, LONG LongPrm, CHAR* Sring )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getdcfdbstrval( ParamID, LongPrm, Sring );

	return err_code;
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
FJ_ERR_CODE FJ_GetDCFDBLongValByStr( USHORT ParamID, CHAR* Sring, LONG* value )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getdcfdblongvalbystr( ParamID, Sring, value );

	return err_code;
}


/**
This function deletes the files of selected by FJ_Dcf_Add_File_To_Select_List() API.
@retval 0 : Success
@retval -1 : Error
*/
FJ_ERR_CODE FJ_Dcf_Execute_Multi_Delete(VOID)
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_dcf_execute_multi_delete();

	return err_code;
}
/**
This function initializes the list for the Multi-Delete function.
@retval 0 : Success
@retval -1 : Error
*/
FJ_ERR_CODE FJ_Dcf_Init_Select_List(VOID)
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_dcf_init_select_list();

	return err_code;
}

/**
This function adds the file to the list for the Multi-Delete function.
@param[in] FileIndex The index of the file to add to the list.
@retval 0 : Success
@retval -1 : Error
*/
FJ_ERR_CODE FJ_Dcf_Add_File_To_Select_List(INT32 FileIndex)
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_dcf_add_file_to_select_list( FileIndex );

	return err_code;
}

/**
This function removes the file from the list for the Multi-Delete function.
@param[in] FileIndex The index of the file to remove from the list.
@retval 0 : Success
@retval -1 : Error
*/
FJ_ERR_CODE FJ_Dcf_Remove_File_From_Select_List(INT32 FileIndex)
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_dcf_remove_file_from_select_list( FileIndex );

	return err_code;
}

/**
 * @brief 		Get file exist in the directory.
 * @param		i:FW_SHORT dir_index
 * @param		o:UCHAR* file_exist
 * @return 		Success/Fail
 * @note		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_GetDCFDirFileExist( USHORT dir_index, UCHAR* file_exist )
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_getdcfdirfileexist( dir_index, file_exist );

	return err_code;
}

/**
 * @brief 		DCF directory delete 
 * @param		i:USHORT ParamID
 * @param		i:USHORT dir_index
 * @return 		Success/Fail
 * @note		None
 * @attention 	After this function call, please set the target directory to current directory by FJ_SetCurrentDirectory().
 */
FJ_ERR_CODE FJ_DcfDirectoryDelete(USHORT ParamID, USHORT dir_index)
{
	FJ_ERR_CODE err_code;

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_dcfdirectorydelete( ParamID, dir_index );

	return err_code;
}


/**
 * @brief		specify the extension and make a new file path from DCF-DB.
 * @param		I : CHAR* EXT
 * @param		O : CHAR* new file path
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_Dcf_Make_New_File_Path_By_Ext( const CHAR* ext, CHAR* new_file_path )
{
	FJ_ERR_CODE err_code=FJ_ERR_OK;
#if 1   ///
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_dcf_make_new_file_path_by_ext( (const CHAR*)ext, new_file_path );
#endif

	return err_code;
}



/**
 * @brief 		Delete a file the specified file index.
 * @param		i:INT32 file_index
 * @return 		Success/Fail
 * @note		None
 * @attention 	None.
 */
FJ_ERR_CODE FJ_Dcf_Del_Single_Obj( INT32 file_index )
{
	FJ_ERR_CODE err_code=FJ_ERR_OK;
#if 1   ///
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_dcf_del_single_obj(file_index );
#endif

	return err_code;
}


/**
 * @brief 		Delete all files in the specified media.
 * @param		i:INT32 media_id
 * @return 		Success/Fail
 * @note		None
 * @attention 	SD1 only support
 */
FJ_ERR_CODE FJ_Dcf_Del_All_Obj( INT32 media_id )
{
	FJ_ERR_CODE err_code=FJ_ERR_OK;
#if 1   ///

	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_dcf_del_all_obj(media_id );
#endif
	return err_code;
}


/**
 * @brief		Update the DCF-DB by adding new file.
 * @param		I : CHAR* new_file_path
 * @return 		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail
 * @note 		None
 * @attention 	None
 */
FJ_ERR_CODE FJ_Dcf_Update_DcfDb_Add_File( CHAR* new_file_path )
{
	FJ_ERR_CODE err_code=FJ_ERR_OK;
#if 1   ///
	if (BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK) {
		// mode check error
		return FJ_ERR_NG;
	}

	err_code = (FJ_ERR_CODE)fj_dcf_update_dcfdb_add_file( new_file_path );
#endif
	return err_code;
}

