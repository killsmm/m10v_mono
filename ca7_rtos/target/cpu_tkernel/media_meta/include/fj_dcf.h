/**
 * @file		fj_dcf.h
 * @brief		DCF Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_DCF_H
#define _FJ_DCF_H

#include "fj_filesystem.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_dcf_overview
@{
	The DCF controls can be achieved by using this API.<br>
	<!--		//M10V_NOT_USED
	<br>
	@section Summary_Date_Face_Filter Summary of Date Sort/Face Filter
	Date sort function  can access the files by date.<br>
	In addition, it is possible to filter by file with the information of the face.<br>
	<br>
	If Date Sort/Face Filter use, valid FJ_CUSTOM_DCF_DATE_DB_USE of fj_custom.h<br>
	<br>
	Specifying the ImageIndex<br>
	ImageIndex is 4Byte. Top 2Byte is DirIndex. Lower 2Byte is ObjIndex.<br>
	<br>
	<table>
	<tr>
	  <th>Bit</th>
	  <th>Meaning</th></tr>
	<tr>
	  <td>0-15</td>
	  <td>Object Index numeber</td></tr>
	<tr>
	  <td>16-31</td>
	  <td>Directory Index number</td></tr>
	</table>
	<br>
	If specify a file in DCF DB, use the DirIndex and ObjIndex.<br>
	Do not use the DirIndex in the Date/FaceFilter DB. DirIndex put a 0.<br>
	IamgeIndex range is as follows.<br>
	<br>
	<table>
	<tr>
	  <th>DB</th>
	  <th>DirIndex</th>
	  <th>ObjIndex</th></tr>
	<tr>
	  <td>DCF DB</td>
	  <td>1 - 900</td>
	  <td>1 - 9999</td></tr>
	<tr>
	  <td>Date/FaceFilter DB</td>
	  <td>0</td>
	  <td>0 - 9999</td></tr>
	</table>

	@section Date_Face_Filter_Table Date Sort/Face Filter table
	Date/FaceFilter has table on buffer.<br>
	The contents of the table is as follows.<br>
	<br>
	<ul>
	  <li>File information table</li>
	</ul>

	<table>
	<tr>
	  <th>Item</th>
	  <th>Description</th>
	  <th>Size</th>
	</tr>
	<tr>
	  <td>Sequential number</td>
	  <td>Allocate number at file.</td>
	  <td>2Byte</td>
	</tr>
	<tr>
	  <td>Date</td>
	  <td>Creation date.</td>
	  <td>4Byte</td>
	</tr>
	<tr>
	  <td>File attribute</td>
	  <td>file sttribute(normal or read-only)</td>
	  <td>2Bit</td>
	</tr>
	<tr>
	  <td>DCF root directory ID</td>
	  <td>ID of DCF root directory name.</td>
	  <td>2Bit</td>
	</tr>
	<tr>
	  <td>Directory number</td>
	  <td>Number of the directory where the file.</td>
	  <td>12Bit</td>
	</tr>
	<tr>
	  <td>File number</td>
	  <td>File number.</td>
	  <td>16Bit</td>
	</tr>
	<tr>
	  <td>File name</td>
	  <td>File name(The first 4 characters of file name).</td>
	  <td>4Byte</td>
	</tr>
	<tr>
	  <td>Directory name</td>
	  <td>Directory name(5 characters from behind of directory name).</td>
	  <td>5Byte</td>
	</tr>
	<tr>
	  <td>Extension type</td>
	  <td>extension type.</td>
	  <td>1Byte</td>
	</tr>
	<tr>
	  <td>File flag</td>
	  <td>File status(duplicate, voicememo).</td>
	  <td>1Byte</td>
	</tr>
	<tr>
	  <td>Filter information</td>
	  <td>Filter information.</td>
	  <td>7Byte</td>
	</tr>
	</table>
	<pre>
	data number: 10,000
	table size : 280,000 Byte
	</pre>

	<ul>
	  <li>Sort table</li>
	</ul>
	<table>
	<tr>
	  <th>Item</th>
	  <th>Content</th>
	  <th>Size</th>
	</tr>
	<tr>
	  <td>Sort index</td>
	  <td>Array number of File information table.</td>
	  <td>2Byte</td>
	</tr>
	</table>
	<pre>
	data number: 10,000
	table size : 20,000 Byte
	</pre>

	<ul>
	  <li>Monthly information table</li>
	</ul>
	<table>
	<tr>
	  <th>Item</th>
	  <th>Content</th>
	  <th>Size</th>
	</tr>
	<tr>
	  <td>Date index</td>
	  <td>Date Index of the beginning object of month.</td>
	  <td>2Byte</td>
	</tr>
	<tr>
	  <td>Number of objects</td>
	  <td>Number of objects of month.</td>
	  <td>2Byte</td>
	</tr>
	</table>
	<pre>
	range of month data : 1980/1 - 2107/12
	data number: 1,536(128 year * 12 month)
	table size : 6,144Byte
	</pre>

	<ul>
	  <li>Daily inforamtion table</li>
	</ul>
	<table>
	<tr>
	  <th>Item</th>
	  <th>Content</th>
	  <th>Size</th>
	</tr>
	<tr>
	  <td>Date index</td>
	  <td>Date Index of the beginning object of day.</td>
	  <td>2Byte</td>
	</tr>
	<tr>
	  <td>Number of objects</td>
	  <td>Number of objects of day.</td>
	  <td>2Byte</td>
	</tr>
	</table>
	<pre>
	data number: 31
	table size : 124Byte
	</pre>

	<ul>
	  <li>Total resource size : 306,268 Byte</li>
	</ul>
	<br>

	@section Date_Face_Filter_File Date Sort/Face Filter file
	Save the file the data in a File information table.<br>
	The file name is decided by definition of fj_custom.h.<br>
	Refer to @ref FJ_CUSTOM_DCF_DATE_DB_DIR, @ref FJ_CUSTOM_DCF_DATE_DB_FILE and @ref FJ_CUSTOM_DCF_DATE_DB_FILE_BK.<br>
	<br>
	The contents of the file data is as follows.<br>
	<br>
	<table>
	<tr>
	  <th>Item</th>
	  <th>Description</th>
	  <th>Size</th></tr>
	<tr>
	  <td>Media ID</td>
	  <td>Media ID</td>
	  <td>16Byte</td></tr>
	<tr>
	  <td>DB version</td>
	  <td>Date/Filter DB version</td>
	  <td>4Byte</td></tr>
	<tr>
	  <td>Use area</td>
	  <td>Maximum use of the Date/Filter DB area</td>
	  <td>2Byte</td></tr>
	<tr>
	  <td>Total count</td>
	  <td>Registration number of objects</td>
	  <td>2Byte</td></tr>
	<tr>
	  <td>File information</td>
	  <td>File information(number of 10,000)</td>
	  <td>280,000 Byte</td></tr>
	</table>
	<br>
	file size : 280,032 Byte * 2 = 560,064 Byte<br>

	The structure of the file is as follows.<br>
	@image html fj_dcf_db_date_file_struct.png
	
	//M10V_NOT_USED
	-->

@}*//* --- end of fj_dcf_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_dcf_sequence
@{
	- @ref DCF_DataBase_create
	- @ref DCF_delete_files_selected
	<!--	//M10V(DATE-DB not used)
	- @ref DCF_db_date_boot01
	- @ref DCF_db_date_boot02
	- @ref DCF_db_date_boot03
	- @ref DCF_db_date_disp_date_view
	- @ref DCF_db_date_change_to_date_view
	- @ref DCF_db_date_change_to_folder_view
	- @ref DCF_db_date_update_sync
	- @ref DCF_db_date_update_async
	- @ref DCF_db_date_update_async_cancel
	- @ref DCF_db_date_save_filter_attr_to_db
	- @ref DCF_db_date_set_filter_type
	-->
	
	<hr>
	@section DCF_DataBase_create DCF DataBase create sequence
	@image html fj_dcf_database_create.png
	
	@section DCF_delete_files_selected DCF delete files selected sequence
	@image html fj_dcf_delete_files_selected.png

<!--		//M10V_NOT_USED
	@section DCF_db_date_boot01 Create new DateDB file in media sequence.
	@image html fj_dcf_db_date_boot01.png
	
	@section DCF_db_date_boot02 Loading DateDb file sequence(If the file is added).
	@image html fj_dcf_db_date_boot02.png

	@section DCF_db_date_boot03 Loading DateDb file sequence(If the file is deleted).
	@image html fj_dcf_db_date_boot03.png

	@section DCF_db_date_disp_date_view Display to date view sequence.
	@image html fj_dcf_db_date_calendar.png

	@section DCF_db_date_change_to_date_view Change to Date view sequence.
	@image html fj_dcf_db_date_change_to_date_view.png

	@section DCF_db_date_change_to_folder_view Change to Folder view sequence.
	@image html fj_dcf_db_date_change_to_folder_view.png

	@section DCF_db_date_update_sync Update of the date DB(Sync)
	@image html fj_dcf_db_date_update_sync.png

	@section DCF_db_date_update_async Update of the date DB(Async)
	@image html fj_dcf_db_date_update_async.png

	@section DCF_db_date_update_async_cancel Update of the date DB(Async:Cancel)
	@image html fj_dcf_db_date_update_async_cancel.png

	@section DCF_db_date_save_filter_attr_to_db Save image filter attribute to date DB
	@image html fj_dcf_db_date_save_filter_attr_to_db.png

	@section DCF_db_date_set_filter_type Set filter type of date DB
	@image html fj_dcf_db_date_set_filter_type.png
-->
	@code
	// Sample code (DCF delete files selected)
	
		#define ADD_FILE_TO_LIST		1
		#define REMOVE_FILE_FROM_LIST	0

		INT32 FileSelectFlag;
		INT32 FileIndex;

			:
			:
		// Initializes the list.
		FJ_Dcf_Init_Select_List();

		for ( ; ; ) {
			// Repeatedly by the number of times of needed add the file to the list or remove the file from list. 
			if (FileSelectFlag == ADD_FILE_TO_LIST) {
				// Add the file to the list
					FJ_Dcf_Add_File_To_Select_List(FileIndex);
				}
				else if (FileSelectFlag == REMOVE_FILE_FROM_LIST) {
					// Remove the file from the list
					FJ_Dcf_Remove_File_From_Select_List(FileIndex);
				}
					:
					:

				if (// Select End..  ) {
					 Select of the files are ended.
					break;
				}
			}

		// Executed to deletes the files selected. 
		FJ_Dcf_Execute_Multi_Delete();
	@endcode

@}*//* --- end of fj_dcf_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_dcf_definition
@{*/
/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/**
Definition that means current directory
*/
#define DCF_DB_CURRENT_DIR		30001

//@cond M10V_NOT_USED
/**
DCF date DB  file exists(bit combination).
*/
#define FJ_DCF_DATE_FILE_EXIST 0x00

/**
DCF date DB  file not exists(bit combination)..
*/
#define FJ_DCF_DATE_FILE_NO_EXIST 0x01

/**
DCF date DB  media id unmatch(bit combination).
*/
#define FJ_DCF_DATE_FILE_MEDIA_ID_UNMATCH 0x02

/**
DCF date DB  date db version unmatch(bit combination).
*/
#define FJ_DCF_DATE_FILE_DB_VERSION_UNMATCH 0x04


/**
Processing type for @ref FJ_Dcf_Date_Update_DateDb<br>
The file(still picture and movie) of information added to media is registered in Date DB.
*/
#define FJ_DCF_DATE_UPDATE_DB_ADD_FILE 0x01

/**
Processing type for @ref FJ_Dcf_Date_Update_DateDb<br>
The file(still picture and movie) of information deleted from media is removed from Date DB.
*/
#define FJ_DCF_DATE_UPDATE_DB_DEL_FILE 0x02

/**
Processing type for @ref FJ_Dcf_Date_Update_DateDb<br>
Function to execute synchronously.
*/
#define FJ_DCF_DATE_SYNCHRO 0x00

/**
Processing type for @ref FJ_Dcf_Date_Update_DateDb<br>
Function to execute asynchronously.
*/
#define FJ_DCF_DATE_ASYNCHRO 0x01
//@endcond M10V_NOT_USED

///////////////////////////////////////////////////////////////////////////////
// Enumeration
///////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/

/**
Description for FJ_DIR_FILE_OPERATION.<br>
Definition of operation of FJ_SetDCFFreeChars API.
*/
typedef enum {
	FJ_FILE_SET_ONLY,		/**< Set the free characters for file */
	FJ_FILE_SET_ALLOWED,	/**< Sets the allowed DCF extensions */
	FJ_DIR_SET_ONLY,		/**< Sets the free characters for directory */
} FJ_DIR_FILE_OPERATION;

/**
Description for FJ_DCF_FREE_CHARS.<br>
Definition of operation of FJ_GetParamArray API.
*/
typedef enum {
	FJ_DCF_CUR_DIR_FREE_CHARS,	/**< Get free charactor of current directory */
	FJ_DCF_CUR_FILE_FREE_CHARS,	/**< Get free charactor of current file */
	FJ_DCF_LAST_DIR_FREE_CHARS,	/**< Get free charactor of last directory */
	FJ_DCF_LAST_FILE_FREE_CHARS	/**< Get free charactor of last file */
} FJ_DCF_FREE_CHARS;

/**
Description for FJ_DCF_SET_STR_VAL.
*/
typedef enum {
	FJ_PARA_DCFDB_DIR_BASE_SET_STR,		/**< Creates the database of DCF */
	FJ_PARA_DCFDB_DIR_CURRENT_SET_STR	/**< Set current directory */
} FJ_DCF_SET_STR_VAL;

/**
Description for FJ_DCF_SET_LONG_VAL.<br>
Definition of operation of FJ_SetDCFDBLongVal API.
*/
typedef enum {
	FJ_PARA_DCFDB_ATTR_SET,			/**< Set the attribute of the object. */
	FJ_PARA_DCFDB_DIR_CURRENT_SET	/**< Set current directory */
} FJ_DCF_SET_LONG_VAL;

/**
Description for FJ_DCF_GET_STR_VAL.<br>
Definition of operation of FJ_GetDCFDBStrVal API.
*/
typedef enum {
	FJ_PARA_DCFDB_DIR_GET_STR,		/**< Get directory name */
	FJ_PARA_DCFDB_OBJ_GET_STR,		/**< Get object name */
	FJ_PARA_DCFDB_DIR_OBJ_GET_STR,	/**< Get dirctory name and object name */
	FJ_PARA_DCFDB_EXT_GET_STR,		/**< Get extention type */
	FJ_PARA_DCFDB_FULL_PATH_GET_STR		/**< Get full path  */
} FJ_DCF_GET_STR_VAL;

/**
Description for FJ_DCF_GET_LONG_VAL.<br>
Definition of operation of FJ_GetDCFDBLongVal API.
*/
typedef enum {
	FJ_PARA_DCFDB_DIR_TOTAL_CNT_GET,	/**< Get number of total directory in "DCIM" count */
	FJ_PARA_DCFDB_DIR_GET,				/**< Get index number of directory */
	FJ_PARA_DCFDB_OBJ_CNT_GET,			/**< Get count of object in the current directory */
	FJ_PARA_DCFDB_ATTR_GET,				/**< Get attribute of object */
	FJ_PARA_DCFDB_DATE_TIME_GET			/**< Get the directory created date and time */
} FJ_DCF_GET_LONG_VAL;

/**
Description for FJ_DCF_GET_LONG_VAL_BY_STR.<br>
Definition of operation of FJ_GetDCFDBLongValByStr API.
*/
typedef enum {
	FJ_PARA_DCFDB_DIR_GET_BY_STR,		/**< Get Directory Index */
	FJ_PARA_DCFDB_DIR_OBJ_GET_BY_STR,	/**< Get Directory Index and Object Index */
	FJ_PARA_DCFDB_EXT_GET_BY_STR		/**< Get File Type */
} FJ_DCF_GET_LONG_VAL_BY_STR;

/**
Description for FJ_DCF_DCF_DIR_DEL.<br>
Definition of operation of FJ_DcfDirectoryDelete API.
*/
typedef enum {
	FJ_PARA_DCFDB_SELECT_DIR,		/**< The DCF directory specified by "dir_index" parameter is deleted */
	FJ_PARA_DCFDB_ALL_EMPTY_DIR,	/**< The DCF directory without all files that exist in the directory "DCIM" is deleted. */
} FJ_DCF_DCF_DIR_DEL;

//@cond M10V_NOT_USED
/**
Description for FJ_DCF_DATE_FILTER_TYPE.<br>
Definition of paramter of FJ_Dcf_Date_Set_Filter_Type API.
*/
typedef enum {
	FJ_DCF_DATE_DO_NOT_FILTER = 0,					/**< Clear filter for DateDB */
	FJ_DCF_DATE_SET_FACE_FILTER,					/**< Set face filter for DateDB */
	FJ_DCF_DATE_FILTER_MAX
} FJ_DCF_DATE_FILTER_TYPE;
//@endcond M10V_NOT_USED

///////////////////////////////////////////////////////////////////////////////
// Structure
///////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/

//@cond M10V_NOT_USED
/**
file information
*/
typedef struct {
	CHAR dcf_root_dir_name[9];	/**< DCF root directory name */
	CHAR dir_name[9];			/**< directory name */
	CHAR file_name[9];			/**< file name */
	CHAR attr;					/**< An attribute for read-only or not */
	CHAR file_type;				/**< File Type */
} FJ_DCF_DATE_FILE_INFO;

/**
Date and time information
*/
typedef struct {
	USHORT ad_year;			/**< A.D. year */
	UCHAR month;			/**< month */
	UCHAR day;				/**< day */
	UCHAR hour;				/**< hour */
	UCHAR min;				/**< minute */
	UCHAR sec;				/**< second */
} FJ_DCF_DATEDB_DATE_INFO;

/**
Index and number of objects
*/
typedef struct {
	USHORT DateIndex;		/**< The first index specified month */
	USHORT Objnum;			/**< Number of objects specified month */
} FJ_DCF_DATE_DB_INDICATER;
//@endcond M10V_NOT_USED

///////////////////////////////////////////////////////////////////////////////
// external variable
///////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*@} --- end of fj_dcf_definition */

///////////////////////////////////////////////////////////////////////////////
// macro
///////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/** @weakgroup fj_dcf_api
@{*/

// Nothing Special

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function creates the DataBase of DCF.
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
*/
FJ_ERR_CODE	FJ_DcfdCreate(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Renewal DcfDB with keeping current directory index.
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remarks After this API called, set new ObjIndex by "FJ_SetCurrentImage( ImageIndex );" if necessary.
*/
FJ_ERR_CODE	FJ_DcfdRenew(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the information of the DCF image.
@param[in] ImageIndex Number of image index
@param[out] DirNum Directory number
@param[out] FileNum File number
@param[out] FileType File Type
@param[out] Locked An attribute for read-only or not
@param[out] file_dupstatus Duplication status of file number(0:unduplicate, 1:duplicate)
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remarks
The meaning to have of bit of the "ImageIndex"
   <table>
   <tr>
	  <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0-15</td>
      <td>Object Index numeber</td></tr>
   <tr>
      <td>16-31</td>
      <td>Directory Index number</td></tr>
   </table>
<br>
<br>
The meaning to have of bit of the "FileType"
   <table>
   <tr>
      <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0</td>
      <td>Allowed extension table[0]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>1</td>
      <td>Allowed extension table[1]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>2</td>
      <td>Allowed extension table[2]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>3</td>
      <td>Allowed extension table[3]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>4</td>
      <td>Allowed extension table[4]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>5</td>
      <td>Allowed extension table[5]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>6</td>
      <td>Allowed extension table[6]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>7</td>
      <td>Allowed extension table[7]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>8-15</td>
      <td>Not used</td></tr>
  </table>
Allowed extension table is defined by FJ_CUSTOM_DCF_ALLOW_NAME.<br>

@code
// Sample Code
	FJ_GetDCFInfo( ((DCF_DB_CURRENT_DIR<<16)|objCnt), &dirkey, &filekey,
	&file_type, &attribute, &file_dupstatus );

	if(FJ_Is_DCF_File_Type("JPG", file_type)) {return E_BF_DCF_IF_FILE_TYPE_JPG;}
	if(FJ_Is_DCF_File_Type("AVI", file_type)) {return E_BF_DCF_IF_FILE_TYPE_AVI;}
@endcode
*/
FJ_ERR_CODE	FJ_GetDCFInfo(INT32 ImageIndex, INT32* DirNum, INT32* FileNum, SHORT* FileType, INT32* Locked, INT32* file_dupstatus);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This funciton sets the free characters used for new DCF files and directories.
@param[in] Dir Indicates whether the command relates to file or directory
@param[in] Operation Order function operation
@param[in] Chars Character of DCF to set
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remarks
Dir = 1		Set directory character<br>
Dir = 0		Set file character<br>
<br>
When Dir is 1, Operation follows the table
   <table>
   <tr>
      <th>Opration</th>
      <th>Meaning</th></tr>
   <tr>
      <td>@ref FJ_DIR_SET_ONLY</td>
      <td>Set the characters of the Directory name.<br>
	                 5 characters of the Directory name and terminal symbol(NULL)).</td></tr>
   </table>
<br>
<br>
When Dir is 0, Operation follows the table
   <table>
   <tr>
      <th>Opration</th>
      <th>Meaning</th></tr>
   <tr>
      <td>@ref FJ_FILE_SET_ONLY</td>
      <td>Set the characters of the File name.<br>
	                 (4 characters of the File name and terminal symbol(NULL)).</td></tr>
   <tr>
      <td>@ref FJ_FILE_SET_ALLOWED</td>
      <td>Set the characters of the Allowed extention table.</td></tr>
   </table>
*/
FJ_ERR_CODE	FJ_SetDCFFreeChars(CHAR Dir, FJ_DIR_FILE_OPERATION Operation, CHAR* Chars);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets the DCF numbers used for new DCF files and directories.
@param[in] DirNum The directory number
@param[in] FileNum The file number
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
*/
FJ_ERR_CODE	FJ_SetNextDCFKey(USHORT DirNum, USHORT FileNum);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the file or directory DCF charctor.
@param[in] ParamID The operation ID of this function
@param[out] Offset Buffer of file or directory charctor
@param[in] Size Buffer size
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remarks
Function operation
   <table>
   <tr>
      <th>ParamID</th>
      <th>Function Operation</th></tr>
   <tr>
      <td>@ref FJ_DCF_CUR_DIR_FREE_CHARS</td>
      <td>Get charctor of current directory.</td></tr>
   <tr>
      <td>@ref FJ_DCF_CUR_FILE_FREE_CHARS</td>
      <td>Get charctor of current file.</td></tr>
   <tr>
      <td>@ref FJ_DCF_LAST_DIR_FREE_CHARS</td>
      <td>Get charctor of last directory.</td></tr>
   <tr>
      <td>@ref FJ_DCF_LAST_FILE_FREE_CHARS</td>
      <td>Get charctor of last file.</td></tr>
   </table>
*/
FJ_ERR_CODE	FJ_GetParamArray(FJ_DCF_FREE_CHARS ParamID, CHAR* Offset, USHORT Size);

//FJ_ERR_CODE	FJ_GetDCFLastFileNumber(FW_SHORT dir_index, FW_USHORT* file_num);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets latest index number in the current directory.
@param[out] Index Latest index number in the current directory
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remarks
The meaning to have of bit of the "Index"
   <table>
   <tr>
	  <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0-15</td>
      <td>Object Index numeber</td></tr>
   <tr>
      <td>16-31</td>
      <td>Directory Index number</td></tr>
   </table>
*/
FJ_ERR_CODE	FJ_Get_Latest_Index(INT32* Index);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets current directory to DCF-DB.
@param[in] dir_index Index number of the directory to set to current directory
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
*/
FJ_ERR_CODE	FJ_SetCurrentDirectory(SHORT dir_index);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the last file number.
@param[in] dir_index Directory index number
@param[out] file_num DCF Last File Number
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
Sample Code
Refer to FJ_HostDcfCreateFileCB
*/
FJ_ERR_CODE	FJ_GetDCFLastFileNumber(SHORT dir_index, USHORT* file_num);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets first free File Number in current directory.
@param[out] fileNum First free File Number in current directory
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
*/
FJ_ERR_CODE	FJ_GetFirstFreeFileNo(USHORT* fileNum);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set attribute of object or set current directory.
@param[in] ParamID The operation ID of this function
@param[in] LongPrm File and directory number
@param[in] value Attribute of object(0 : UnLock, 1 : Lock)
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remarks
Function operation
   <table>
   <tr>
      <th>ParamID</th>
      <th>Function Operation</th></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_ATTR_SET</td>
      <td>Set attribute of object.</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_DIR_CURRENT_SET</td>
      <td>Set current directory.<br></td></tr>
   </table>
<br>
<br>
The meaning to have of bit of the "LongPrm"
   <table>
   <tr>
      <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0-13</td>
      <td>Object Index numeber</td></tr>
   <tr>
      <td>14-31</td>
      <td>Directory Index number</td></tr>
   </table>
<br>
Set attribute operation updates the DataBase of DCF.<br>
*/
FJ_ERR_CODE	FJ_SetDCFDBLongVal(USHORT ParamID, LONG LongPrm, LONG value);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get string data from DCF-DB.
@param[in] ParamID The operation ID of this function
@param[in] LongPrm File and directory number
@param[out] Sring String data from DCF-DB
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remarks
Function operation
   <table>
   <tr>
      <th>ParamID</th>
      <th>Function Operation</th></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_DIR_GET_STR</td>
      <td>Get directory name.</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_OBJ_GET_STR</td>
      <td>Get object name.</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_DIR_OBJ_GET_STR</td>
      <td>Get directory name and object neme.</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_EXT_GET_STR</td>
      <td>Get extention type.</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_FULL_PATH_GET_STR</td>
      <td>Get full path.</td></tr>
   </table>
<br>
<br>
The meaning to have of bit of the "LongPrm"
   <table>
   <tr>
	  <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0-13</td>
      <td>File numeber</td></tr>
   <tr>
      <td>14-31</td>
      <td>Directory number</td></tr>
   </table>
*/
FJ_ERR_CODE	FJ_GetDCFDBStrVal(USHORT ParamID, LONG LongPrm, CHAR* Sring);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets long data from DCF-DB.
@param[in] ParamID The operation ID of this function
@param[in] LongPrm The operation parameter of this function
@param[out] value Data from DCF-DB
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remarks
Function operation
   <table>
   <tr>
      <th>ParamID</th>
      <th>Function Operation</th></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_DIR_TOTAL_CNT_GET</td>
      <td>Get number of total directory in "DCIM" count.<br>
	                 LongPrm is not used.</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_DIR_GET</td>
      <td>Get index number of directory.<br>
	                 Set Directory Index to LongPrm.</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_OBJ_CNT_GET</td>
      <td>Get count of object in the current directory.<br>
	                 Set Directory Index to LongPrm.</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_ATTR_GET</td>
      <td>Get attribute of object.<br>
	                 Set Directory Index to LongPrm.</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_DATE_TIME_GET</td>
      <td>Get the directory created date and time.<br>
	                 Set Directory Index to LongPrm.</td></tr>
   </table>
*/
FJ_ERR_CODE	FJ_GetDCFDBLongVal(USHORT ParamID, LONG LongPrm, LONG* value);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get long data from DC-DB.
@param[in] ParamID The operation ID of this function
@param[in] Sring The operation parameter of this function
@param[out] value Long data form DCF-DB
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remarks
Function operation
   <table>
   <tr>
      <th>ParamID</th>
      <th>Function Operation</th></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_DIR_GET_BY_STR</td>
      <td>Get Directory Index.<br>
	                 Set directory name to Sring.</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_DIR_OBJ_GET_BY_STR</td>
      <td>Get Directory Index and Object Index.<br>
	                 Set directory name and object name to Sring.<br>
					 <br>
					 The meaning to have of bit of the "Sring"<br>
					 <CENTER>0-15 bit : Object name<br></CENTER>
					 <CENTER>16-32 bit : Directory name</CENTER></td></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_EXT_GET_BY_STR</td>
      <td>Get File Type.<br>
	                 Set extention type to Sring.</td></tr>
   </table>
<br>
<br>
The meaning to have of bit of the "FileType"
   <table>
   <tr>
      <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0</td>
      <td>Allowed extension table[0]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>1</td>
      <td>Allowed extension table[1]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>2</td>
      <td>Allowed extension table[2]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>3</td>
      <td>Allowed extension table[3]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>4</td>
      <td>Allowed extension table[4]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>5</td>
      <td>Allowed extension table[5]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>6</td>
      <td>Allowed extension table[6]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>7</td>
      <td>Allowed extension table[7]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>8-15</td>
      <td>Not used</td></tr>
   </table>
*/
FJ_ERR_CODE	FJ_GetDCFDBLongValByStr(USHORT ParamID, CHAR* Sring, LONG* value);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function check file type.
@param[in] ext_name Extension name
@param[in] file_type File type
@retval FALSE : Ext_name dose not belong to file_type
@retval TRUE  : Ext_name found in file_type
@remarks
The meaning to have of bit of the "file_type"
   <table>
   <tr>
      <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0</td>
      <td>Allowed extension table[0]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>1</td>
      <td>Allowed extension table[1]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>2</td>
      <td>Allowed extension table[2]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>3</td>
      <td>Allowed extension table[3]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>4</td>
      <td>Allowed extension table[4]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>5</td>
      <td>Allowed extension table[5]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>6</td>
      <td>Allowed extension table[6]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>7</td>
      <td>Allowed extension table[7]  exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>8-15</td>
      <td>Not used</td></tr>
  </table>
<br>
Allowed extension table is defined by FJ_CUSTOM_DCF_ALLOW_NAME.
*/
BOOL	FJ_Is_DCF_File_Type(const CHAR* ext_name, SHORT file_type);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
The number of total directry in "DCIM" can be acquired.
@param[out] count Count of total directory in "DCIM"
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
*/
FJ_ERR_CODE	FJ_DirectDcfDirNum(USHORT* count);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the attribute of directory of DCF DataBase.
@param[in] dir_index Directory index number
@param[out] count Count of total object in the requested directory
@param[out] dirName Directory name
@param[out] date Directory created date
@param[out] time Directory created time
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
The meaning to have of bit of the "date"
   <table>
   <tr>
      <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0-4</td>
      <td>Day (1 - 31)</td></tr>
   <tr>
      <td>5-8</td>
      <td>Month (1 - 12)</td></tr>
   <tr>
      <td>9-15</td>
      <td>Year -1980 (0 - 127)</td></tr>
   </table>
<br>
<br>
The meaning to have of bit of the "time"
   <table>
   <tr>
      <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0-4</td>
      <td>Second / 2 (0 - 29)</td></tr>
   <tr>
      <td>5-10</td>
      <td>Minute (0 - 59)</td></tr>
   <tr>
      <td>11-15</td>
      <td>Hour (0 - 23)</td></tr>
   </table>
<br>
All media format is accuracy of "time" is two second.<br>
The calculation method of the real value.<br>
Year = (date & 0xFC00) + 1980<br>
Second = (time & 0x001F) * 2<br>
*/
FJ_ERR_CODE	FJ_DirectDcfDumpDirIAttr(SHORT dir_index, USHORT* count, CHAR* dirName, USHORT* date, USHORT* time);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the attribute of directory of DCF DataBase.<br>
"time" data size accuracy has been expanded.
@param[in] dir_index Directory index number
@param[out] count Count of total object in the requested directory
@param[out] dirName Directory name
@param[out] date Directory created date
@param[out] time Directory created time
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
The meaning to have of bit of the "date"
   <table>
   <tr>
      <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0-4</td>
      <td>Day (1 - 31)</td></tr>
   <tr>
      <td>5-8</td>
      <td>Month (1 - 12)</td></tr>
   <tr>
      <td>9-15</td>
      <td>Year -1980 (0 - 127)</td></tr>
   </table>
<br>
<br>
The meaning to have of bit of the "time"
   <table>
   <tr>
      <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0-5</td>
      <td>Second (0 - 59)</td></tr>
   <tr>
      <td>6-11</td>
      <td>Minute (0 - 59)</td></tr>
   <tr>
      <td>12-16</td>
      <td>Hour (0 - 23)</td></tr>
   </table>
<br>
When the media format is FAT12, FAT16, and FAT32, the accuracy of "time" is one second or two seconds. Because it is file dependence.<br>
When the media format is exFAT *1, the accuracy of "time" is one seconds.<br>
The calculation method of the real value.<br>
Year = (date & 0xFC00) + 1980<br>
<br>
 * *1 To use the exFAT is a license required.
*/
FJ_ERR_CODE	FJ_DirectDcfDumpDirIAttrEx(SHORT dir_index, USHORT* count, CHAR* dirName, USHORT* date, ULONG* time);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function creates the DCF directory.
@param[in] dirNum Directory number to create
@param[in] dirNameIndex Index of the directory name to create (Please set "0")
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
When this function was called, current directory will be changed to created directory.<br>
<br>
Function operation
   <table>
   <tr>
      <th>dirNameIndex</th>
      <th>Function Operation</th></tr>
   <tr>
      <td>0</td>
      <td>The directory of the name defined by FJ_CUSTOM_DCF_DIR_NAME is created</td></tr>
   </table>
*/
FJ_ERR_CODE	FJ_DirectDcfDirectoryCreate(USHORT dirNum, UCHAR dirNameIndex);


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the count of allowed files in the object.
@param[in] obj_index Index number of the object in current directory
@param[in] count The count of allowed files in the requested object
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
*/
FJ_ERR_CODE	FJ_GetAllowdNum(SHORT obj_index, UCHAR* count);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the information of the directory.
@param[in] dir_index Directory index number
@param[out] dirName Directory name
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
*/
FJ_ERR_CODE	FJ_GetDirInfo(SHORT dir_index, CHAR* dirName);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the duplication status of the directory.
@param[in] dir_index Directory index number
@param[out] dupStatus Directory duplicated status(0 : UnDuplicated, 1 : Duplicated)
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@code
// Sample Code
	SHORT dir_index;
	CHAR dirdupStatus;
	
	FJ_DcfdCreate();
	FJ_Mount( FJ_MEDIA_SD0, "I" );
	FJ_SetCurrentDirectory( dir_index );
	FJ_GetDirDuplicateStatus( dir_index, &dirdupStatus );
@endcode
*/
FJ_ERR_CODE	FJ_GetDirDuplicateStatus(SHORT dir_index, CHAR* dupStatus);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the max file number as directory number.
@param[out] file_no Max file number as directory number
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
case 1: This API return "0".<br>
101SOCIO ... this is directory<br>
<br>
case 2: This API return "102".<br>
101SOCIO ... this is file<br>
102SOCIO ... this is file<br>
103SOCIO ... this is directory<br>
<br>
case 3: This API return "104".<br>
101SOCIO ... this is file<br>
102SOCIO ... this is file<br>
103SOCIO ... this is directory<br>
104SOCIO ... this is file<br>
<br>
case 4: This API return "105".<br>
101SOCIO ... this is file<br>
102SOCIO ... this is file<br>
103SOCIO ... this is directory<br>
104SOCIO ... this is file<br>
105SOCIO ... this is file<br>
106SOCIO ... this is directory<br>
*/
FJ_ERR_CODE	FJ_GetMaxFileNumAsDirNum(USHORT* file_no);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets file exist in the directory.
@param[in] dir_index Directory index number
@param[out] file_exist file exist flag
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
The meaning to have of bit of the "file_exist"
   <table>
   <tr>
	  <th>Bit</th>
      <th>meaning</th></tr>
   <tr>
      <td>0</td>
      <td>object is exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>1</td>
      <td>not object is exist(1) or no-exist(0)</td></tr>
   <tr>
      <td>2-7</td>
      <td>Not used</td></tr>
   </table>
*/
FJ_ERR_CODE FJ_GetDCFDirFileExist( USHORT dir_index, UCHAR* file_exist );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function deletes the DCF directory without the file.
@param[in] ParamID function operation.
@param[in] dir_index Directory index number
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
After this function call, please set the target directory to current directory by FJ_SetCurrentDirectory().<br>
<br>
Function operation
   <table>
   <tr>
	  <th>ParamID</th>
      <th>Function Operation</th></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_SELECT_DIR</td>
      <td>The DCF directory specified by "dir_index" parameter is deleted</td></tr>
   <tr>
      <td>@ref FJ_PARA_DCFDB_ALL_EMPTY_DIR</td>
      <td>The DCF directory without all files that exist in the directory "DCIM" is deleted. <br>
	                 "dir_index" parameter is not used.
	                 "dir_index" parameter sets "0".</td></tr>
   </table>

*/
FJ_ERR_CODE FJ_DcfDirectoryDelete(USHORT ParamID, USHORT dir_index);

#if 1	/* for Multi-Delete */
//FJ_ERR_CODE FJ_DirectDcfDeleteSelect(INT32 *SelectList, UCHAR SelectNum);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function deletes the files of selected by FJ_Dcf_Add_File_To_Select_List() API.
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
For use to the Multi-Delete function, please refer to FJ_Dcf_Init_Select_List() API, <br>
FJ_Dcf_Add_File_To_Select_List() API and FJ_Dcf_Remove_File_From_Select_List() API.<br>
*/
FJ_ERR_CODE	FJ_Dcf_Execute_Multi_Delete(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function initializes the list for the Multi-Delete function.
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
For use to the Multi-Delete function, please refer to FJ_Dcf_Add_File_To_Select_List() API, <br>
FJ_Dcf_Remove_File_From_Select_List() API and FJ_Dcf_Execute_Multi_Delete() API.<br>
*/
FJ_ERR_CODE	FJ_Dcf_Init_Select_List(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function adds the file to the list for the Multi-Delete function.
@param[in] FileIndex The index of the file to add to the list.
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
The max number of the list is 100.<br>
This API only selects the file to the list.(File is not deleted yet.)<br>
The selected files is deleted by FJ_Dcf_Execute_Multi_Delete() API.<br>
For use to the Multi-Delete function, please refer to FJ_Dcf_Init_Select_List() API, <br>
FJ_Dcf_Remove_File_From_Select_List() API and FJ_Dcf_Execute_Multi_Delete() API.<br>
<br>
The meaning to have of bit of the "FileIndex"
   <table>
   <tr>
	  <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0-15</td>
      <td>Object Index numeber</td></tr>
   <tr>
      <td>16-31</td>
      <td>Directory Index number</td></tr>
   </table>
*/
FJ_ERR_CODE	FJ_Dcf_Add_File_To_Select_List(INT32 FileIndex);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function removes the file from the list for the Multi-Delete function.
@param[in] FileIndex The index of the file to remove from the list.
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
This API only removes the file from the list.(File is not deleted.)<br>
The removed files is not deleted by FJ_Dcf_Execute_Multi_Delete() API.<br>
For use to the Multi-Delete function, please refer to FJ_Dcf_Init_Select_List() API, <br>
FJ_Dcf_Add_File_To_Select_List() API and FJ_Dcf_Execute_Multi_Delete() API.<br>
<br>
The meaning to have of bit of the "FileIndex"
   <table>
   <tr>
	  <th>Bit</th>
      <th>Meaning</th></tr>
   <tr>
      <td>0-15</td>
      <td>Object Index numeber</td></tr>
   <tr>
      <td>16-31</td>
      <td>Directory Index number</td></tr>
   </table>
*/
FJ_ERR_CODE	FJ_Dcf_Remove_File_From_Select_List(INT32 FileIndex);

#endif	/* for Multi-Delete */

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function specify the extension and get a new file path from DCF-DB.
@param[in] ext specify the extension
@param[out] new_file_path a new file path from DCF-DB
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
<br>
<br>
*/
FJ_ERR_CODE FJ_Dcf_Make_New_File_Path_By_Ext( const CHAR* ext, CHAR* new_file_path );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function delete a file the specified file index.
@param[in] file_index file index to delete
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
<br>
<br>
*/
FJ_ERR_CODE FJ_Dcf_Del_Single_Obj( INT32 file_index );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function delete all files in the specified media.
@param[in] media_id  a new file path registered in the DCF-DB
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
<br>
<br>
*/
FJ_ERR_CODE FJ_Dcf_Del_All_Obj( INT32 media_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function update the DCF-DB by adding new file.
@param[in] new_file_path a new file path registered in the DCF-DB
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remark
<br>
<br>
*/
FJ_ERR_CODE FJ_Dcf_Update_DcfDb_Add_File( CHAR* new_file_path );

/*@} --- end of fj_dcf_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
#endif	// _FJ_DCF_H
