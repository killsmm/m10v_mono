/**
 * @file		dcf_controller.c
 * @brief		FileSystem scenario control with depend on DCF.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>			// strcat, strcpy...
#include <ctype.h>

#include "dcf_controller.h"
#include "fj_param.h"
#include "fj_custom.h"
#include "exif_controller.h"
#include "dcf_db.h"
#include "mdf_wrapper.h"
#include "dcf_if_receiver.h"
#include "fs_user_custom.h"
#include "boot_loader.h"
#include "fj_boot.h"
#include "fs_controller.h"
#include "debug.h"
#include "fj_host.h"

#include "dcf_attr.h"
#ifdef CO_A_FILE_NAME_LIST	// CO_A_FILE_NAME_LIST
#include "dd_arm.h"
#endif	// A CO_A_FILE_NAME_LIST  END
//#include "h264_user.h"
/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define CO_BF_DCF_CONTROLLER_SERIES_DIR_RESET
//#define CO_BF_DCF_SAVE_MAINIMAGE_ONLY	// For Test option
//#define CO_BF_DCF_CONTROLLER_MOUNT_DCF_DB

#define D_BF_DCF_CONTROLLER_FILENAME_LEN    9

#define D_BF_DCF_CONTROLLER_DIR_START_NUM	100
#define D_BF_DCF_CONTROLLER_DIR_END_NUM		999

#define D_BF_DCF_CONTROLLER_RAW_INFO_SIZ    (sizeof(T_BF_DCF_IF_EXIF_INFO) + 10)      //exif.info(44) + raw file check name(10)
#define D_BF_DCF_CONTROLLER_RAW_VGA420_SIZ  (0x70800)

#define D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT E_BF_MEASURE_TIME_ID_FILE_SYSTEM

#if 1	/* for Multi-Delete */
#define D_BF_DCF_CONTROLLER_SELECT_LIST_SIZE 100
#endif

#define D_BF_DCF_CONTROLLER_ATTR_PROTECT_FILTER (0xFE)

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

// Result of processing
static volatile FW_INT32 g_bf_dcf_controller_ret_err;

// Result of division reading processing of file
static volatile FW_INT32 g_bf_dcf_controller_div_load_ret_err;

// In the file division processing, the reading size of data
static volatile FW_ULONG g_bf_dcf_controller_dcfc_ret_length;

// Information on reading data
static volatile T_BF_DCF_IF_LOAD_IMAGE_DATA g_bf_dcf_controller_dcfc_ret_load_image_data;

// It is a flag of the directory number and the file number when DB is made to set it.
static volatile E_BF_DCF_IF_FILE_NAME g_bf_dcf_controller_file_name_setting = E_BF_DCF_IF_FILE_NAME_RESET;

// It is an initialization flag of media.(0:Media have not been initialized. 1:Media have been initialized.)
static volatile FW_UCHAR g_bf_dcf_controller_init_flg[D_BF_FS_CONTROLLER_MEDIA_ID_NUM] = {0};

// Directory number when newly making it
static volatile FW_USHORT g_bf_dcf_controller_last_dir_no = 0;

// File number when newly making it
static volatile FW_USHORT g_bf_dcf_controller_last_file_no = 0;

// DCF image directory(DCIM) path
static volatile FW_CHAR g_bf_dcf_controller_dcim_path[16];

// Directory name when newly making it
static volatile FW_CHAR g_bf_dcf_controller_dir_name[8] = {FJ_CUSTOM_DCF_DIR_NAME};

// File name when newly making it
static volatile FW_CHAR g_bf_dcf_controller_file_name[8] = {FJ_CUSTOM_DCF_FILE_NAME};

// File id of file that has opened
static volatile FW_INT32 g_bf_dcf_controller_display_file_id = 0;

// DCF Index of file that has opened
static volatile FW_LONG  g_bf_dcf_controller_display_file_index = 0;

#if 1	// for Multi-Delete
// DCF index of selection deletion object
static volatile T_BF_DCF_IF_FILE_INDEX g_bf_dcf_controller_select_list[D_BF_DCF_CONTROLLER_SELECT_LIST_SIZE];

// Index of empty "g_bf_dcf_controller_select_list" array
static volatile FW_UCHAR		g_bf_dcf_controller_select_list_free_index_array[D_BF_DCF_CONTROLLER_SELECT_LIST_SIZE];

// The maximum index of array used by "g_bf_dcf_controller_select_list" array
static volatile FW_UCHAR		g_bf_dcf_controller_select_list_index = 0;

// Number of selection deletion object
static volatile FW_UCHAR		g_bf_dcf_controller_select_list_num   = 0;

// The latest index of array used by "g_bf_dcf_controller_select_list_free_index_array" array
static volatile  FW_UCHAR		g_bf_dcf_controller_select_list_free_index = 0;

#ifdef CO_A_FILE_NAME_LIST	// CO_A_FILE_NAME_LIST
// file name list data
static T_BF_DCF_CONTROLLER_FLIE_NAME_LIST	g_bg_dcf_controller_file_name_list;
#endif	// A CO_A_FILE_NAME_LIST  END

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
// Number of Individual Image Number
static E_BF_DCF_IF_MPF_EXT_MP_SELECT g_bf_dcf_controller_ext_mp_select_num = E_BF_DCF_IF_MPF_EXT_MP_SELECT_1;
#endif // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

#endif // for Multi-Delete

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
FW_VOID BF_Dcf_Controller_Close_Display_File(FW_VOID);

static FW_INT32 dcf_controller_set_drv_path(FW_VOID);
static FW_INT32 dcf_controller_make_file_path(FW_CHAR* file_path, FW_CHAR* file_name, FW_SHORT fileno, FW_CHAR* ext);
static FW_INT32 dcf_controller_read_with_measure_speed(FW_INT32 file_id, FW_ULONG addr, FW_ULONG count, FW_ULONG* length);
static FW_INT32 dcf_controller_make_new_file_path_abs(FW_CHAR* ext, FW_CHAR* file_path);
static FW_INT32 dcf_controller_make_new_thm_file_path_abs(FW_CHAR* ext, FW_CHAR* main_file_path, FW_CHAR* thm_file_path);
static FW_INT32 dcf_controller_create_new_file(FW_CHAR* ext, FW_INT32* file_id, FW_CHAR* make_filepath);
static FW_INT32 dcf_controller_close_new_file(FW_INT32 file_id, FW_CHAR* make_filepath, E_BF_DCF_IF_REMOVE_FLG remove_flg);
static FW_INT32 dcf_controller_open_index_file(T_BF_DCF_IF_FILE_INDEX file_index, FW_INT32* file_id);
static FW_VOID dcf_controller_set_last_file(FW_USHORT dirno, FW_USHORT fileno);
static FW_VOID dcf_controller_get_last_file(FW_USHORT* dirno, FW_USHORT* fileno, E_BF_DCF_IF_LATEST_TYPE type);
static FW_INT32 dcf_controller_w_lseek_write(FW_INT32 file_id, FW_ULONG offset, FW_ULONG addr, FW_ULONG count, FW_ULONG* length);
static FW_SHORT dcf_controller_get_dir_index(FW_LONG index);
static FW_SHORT dcf_controller_get_obj_index(FW_LONG index);
static FW_INT32 dcf_controller_w_open_lseek_read(T_BF_DCF_IF_FILE_INDEX file_index, FW_ULONG offset, FW_ULONG addr, FW_ULONG count, FW_ULONG* length);
#if 0
static FW_INT32 dcf_controller_copy_read_write(FW_INT32 org_file_id, FW_INT32 dst_file_id, FW_ULONG addr, FW_ULONG size, FW_ULONG org_file_size, E_BF_MEDIA_STATE_MEDIA_TYPE org_media, E_BF_MEDIA_STATE_MEDIA_TYPE dst_media);
#endif
static FW_INT32 dcf_controller_get_jpeg_qt_data(FW_INT32 file_id, E_BF_DCF_IF_QT_TYPE qt_type, T_BF_DCF_IF_JPEG_QT* qt);
#if 0
static FW_INT32 dcf_controller_copy_option_file(T_BF_DCF_IF_FILE_INDEX tgt_index, E_BF_MEDIA_STATE_MEDIA_TYPE org_media, E_BF_MEDIA_STATE_MEDIA_TYPE dst_media, FW_CHAR* org_filepath, FW_CHAR* dst_filepath, FW_ULONG addr, FW_ULONG size);
#endif
static FW_INT32 dcf_controller_get_total_file_count(FW_CHAR* dir_name, FW_INT32* total_file_num);
static FW_INT32 dcf_controller_db_check_dir_file_exist(FW_USHORT dir_index, FW_UCHAR* file_exist);
static FW_INT32 dcf_controller_del_dir(FW_INT32 total_file_num, FW_INT32* file_cnt);

static FW_INT32 dcf_controller_set_protect_attribute(const FW_CHAR* file_name, FW_UCHAR attr);
#if 0	// MDF custom
static FW_INT32 dcf_controller_set_protect_attribute_for_multi_set(const FW_CHAR* file_name, FW_UCHAR attr);
#endif
#if 0 // "THM" Not supported
static FW_INT32 dcf_controller_del_all_thm_dir(FW_INT32* total_file_cnt);
static FW_INT32 dcf_controller_del_single_thm_dir(FW_CHAR* thm_dir_name, FW_INT32* del_count);
#endif // "THM" Not supported

#ifdef CO_A_FILE_NAME_LIST	// CO_A_FILE_NAME_LIST
static FW_VOID dcf_controller_init_file_name_list( VOID );
static BOOL dcf_controller_search_file_name_list( FW_ULONG idx, FW_UCHAR file_type, FW_CHAR* file_str );
static FW_INT32 dcf_controller_modify_file_path_ext(FW_CHAR* ext, FW_CHAR* file_path);
static FW_VOID dcf_controller_update_file_name_list( FW_ULONG idx, FW_UCHAR file_type, FW_CHAR* file_str );
static FW_INT32 dcf_controller_create_file_by_list(FW_CHAR* ext, FW_INT32* file_id, FW_CHAR* make_filepath);
#endif	// A CO_A_FILE_NAME_LIST  END

/**
 * @brief get directory index
 * @param   I : FW_LONG index
 * @return  directory index
 * @note none
 * @attention none
 */
static FW_SHORT dcf_controller_get_dir_index(FW_LONG index)
{
	T_BF_DCF_IF_FILE_INDEX file_type;
	file_type.index = index;
	return file_type.pack.dir_index; /* pgr0872 */
}


/**
 * @brief Get count of object in current directory
 * @param none
 * @return count of object in the directory
 * @note none
 * @attention none
 */
static FW_USHORT dcf_controller_get_total_file_cnt(FW_VOID)
{
	FW_INT32 ercd;
	FW_SHORT obj_count;

	ercd = BF_Dcf_Db_Read_Obj_Count(DCF_DB_CURRENT_DIR, &obj_count);
	if(FSDCF_ERRNO_OK != ercd) {
		return 0;
	}

	return obj_count;
}

/**
 * @brief get last dir & file Number
 * @param   O : FW_USHORT* dirno
 *          O : FW_USHORT* fileno
 *          I : E_BF_DCF_IF_LATEST_TYPE type
 * @return  none
 * @note none
 * @attention none
 */
static FW_VOID dcf_controller_get_last_file(FW_USHORT* dirno, FW_USHORT* fileno, E_BF_DCF_IF_LATEST_TYPE type)
{
	if(dirno == NULL) {
		return;
	}
	if(fileno == NULL) {
		return;
	}

	*dirno = g_bf_dcf_controller_last_dir_no;
	*fileno = g_bf_dcf_controller_last_file_no;
}

/**
 * @brief get object index
 * @param   I : FW_LONG index
 * @return  object index
 * @note none
 * @attention none
 */
static FW_SHORT dcf_controller_get_obj_index(FW_LONG index)
{
	T_BF_DCF_IF_FILE_INDEX file_type;
	file_type.index = index;
	return file_type.pack.obj_index; /* pgr0872 */
}

/**
 * @brief set last dirno and fileno.
 * @param i:dirno directory number
 * @param i:fileno file number
 * @return FW_VOID
 */
static FW_VOID dcf_controller_set_last_file(FW_USHORT dirno, FW_USHORT fileno)
{
	g_bf_dcf_controller_last_dir_no = dirno;
	g_bf_dcf_controller_last_file_no = fileno;
}


static FW_INT32 dcf_controller_dcfdb_init()
{
	FW_INT32 ercd;

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(E_BF_MEASURE_TIME_ID_DCF_DB);
#endif

	ercd = dcf_controller_set_drv_path();
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* set base directory */
#if 1
	BF_Dcf_Db_Set_Dir((FW_CHAR*)g_bf_dcf_controller_dcim_path);
#else
	ercd = BF_Dcf_Db_Set_Dir((FW_CHAR*)g_bf_dcf_controller_dcim_path);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}
#endif

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(E_BF_MEASURE_TIME_ID_DCF_DB, "DCF DB INIT and SET last Dir\n");
#endif

	return ercd;
}

/**
 * @brief set drive information depend on media.
 * @param FW_VOID
 * @return 0:normal end
 * @rerurn -101:not support media
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_set_drv_path(FW_VOID)
{
	FW_INT32 ercd;

	ercd = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), (FW_CHAR*)g_bf_dcf_controller_dcim_path);
	if(ercd != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("dcf_controller_set_drv_path(): NOT SUPPORT MEDIA: cur_used_media=%d\n", BF_Media_State_Get_Used_Media()));
#endif
		return FSDCF_ERRNO_NOT_SUPPORT_MEDIA;
	}
	strcat((FW_CHAR*)g_bf_dcf_controller_dcim_path, ":\\DCIM\\");

	return 0;
}

/**
 * @brief open load file of target index.
 * @param i:file_index open file index
 * @param i:*file_id pointer of file id
 * @return error code
 * @note if file_info is NULL, this output is not get.
 * @attention none
 */
static FW_INT32 dcf_controller_open_index_file(T_BF_DCF_IF_FILE_INDEX file_index, FW_INT32* file_id)
{
	FW_INT32 ercd;
	FW_CHAR open_filepath[64];

	if(file_id == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	if (file_index.index == g_bf_dcf_controller_display_file_index) {
		FW_ULONG result;

		ercd = BF_Mdf_Wrapper_Lseek(g_bf_dcf_controller_display_file_id, 0, D_BF_MDF_WRAPPER_SEEK_SET, &result);
		if (ercd != 0) {
			BF_Dcf_Controller_Close_Display_File();

			/* If lseek error. Reopen display file */
			/* make file path of same of specified file index */
			ercd = BF_Dcf_Db_Get_Index_File_Info(file_index, open_filepath, NULL);
			if(ercd != 0) {
				return ercd;
			}

			/* file open */
			*file_id = BF_Mdf_Wrapper_Open(open_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
			if( *file_id == 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", *file_id, open_filepath));
#endif
				ercd = *file_id;
				g_bf_dcf_controller_display_file_index = 0;
				g_bf_dcf_controller_display_file_id = 0;
				return ercd;
			}
			else {
				g_bf_dcf_controller_display_file_index = file_index.index;
				g_bf_dcf_controller_display_file_id = *file_id;
			}
		}
		else {
			*file_id = g_bf_dcf_controller_display_file_id;
		}
	}
	else {
		if (g_bf_dcf_controller_display_file_id != 0) {
			BF_Dcf_Controller_Close_Display_File();
		}

		/* make file path of same of specified file index */
		ercd = BF_Dcf_Db_Get_Index_File_Info(file_index, open_filepath, NULL);
		if(ercd != 0) {
			return ercd;
		}

		/* file open */
		*file_id = BF_Mdf_Wrapper_Open(open_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
		if( *file_id == 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", *file_id, open_filepath));
#endif
			ercd = *file_id;
			g_bf_dcf_controller_display_file_index = 0;
			g_bf_dcf_controller_display_file_id = 0;
		}
		else {
			g_bf_dcf_controller_display_file_index = file_index.index;
			g_bf_dcf_controller_display_file_id = *file_id;
		}
	}

	return ercd;
}


/**
 * @brief read and measure read speed.
 * @param i:file_id the file number of target file
 * @param i:addr the pointer of area which is stored the write data
 * @param i:count the write data size
 * @param o:*length the pointer of area which is stored the actual read size
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_read_with_measure_speed(FW_INT32 file_id, FW_ULONG addr, FW_ULONG count, FW_ULONG* length)
{
	FW_INT32 ercd;
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME_READ
	FW_ULONG ltime;
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif
	ercd = BF_Mdf_Wrapper_Read(file_id, (FW_VOID*)addr, count, length);
	if((ercd != 0) && (ercd != D_BF_MDF_WRAPPER_ERR_EOF)){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x, addr=%lx, count=%lu\n", ercd, file_id, addr, count));
#endif
		return ercd;
	}
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME_READ
	BF_Fs_User_Custom_Measure_Time_Get(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, &ltime);
	BF_Fs_User_Custom_Info_Printer(("Read Size=%d(byte), time=%d, speed=%d(kb/s)\n", *length, ltime, (*length * 125) / (128 * ltime)));
#endif
	return ercd;
}



/**
 * @brief Wrapper of lseek and read.
 * @param i:file_index file index
 * @param i:offset the offset value (byte) from the standard position
 * @param i:addr the pointer of area which is stored the read data
 * @param i:count the read data size
 * @return actual read size
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_w_open_lseek_read(T_BF_DCF_IF_FILE_INDEX file_index, FW_ULONG offset, FW_ULONG addr, FW_ULONG count, FW_ULONG* length)
{
	FW_INT32 ercd;
	FW_INT32 file_id;
	FW_ULONG result;

	/* file open */
	ercd = dcf_controller_open_index_file(file_index, &file_id);
	if(ercd != 0) {
		return ercd;
	}

	ercd = BF_Mdf_Wrapper_Lseek(file_id, (FW_INT32)offset, D_BF_MDF_WRAPPER_SEEK_SET, &result);
	if(ercd != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		BF_Dcf_Controller_Close_Display_File();

		return ercd;
	}

	ercd = dcf_controller_read_with_measure_speed(file_id, addr, count, length);
	if(ercd != 0){
		BF_Dcf_Controller_Close_Display_File();

		return ercd;
	}

	return ercd;
}


/**
 * @brief make file name
 * @param   O : FW_CHAR* file_path
 *          I : FW_CHAR* file_name
 *          I : FW_SHORT fileno
 *          I : FW_CHAR* ext
 * @return  0 : normal end
 *          FSDCF_ERRNO_NOT_SUPPORT_FILENO
 *          FSDCF_ERRNO_TOUCH_NULL_POINTER
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_make_file_path(FW_CHAR* file_path, FW_CHAR* file_name, FW_SHORT fileno, FW_CHAR* ext)
{
#if 0
	FW_CHAR num_char[5];
#endif
	FW_INT32 ercd;

	if(file_path == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(file_name == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(ext == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

#if 0
	if( (fileno < 1) || (fileno > 9999) ) {
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

	strcpy(file_path, file_name);
	strcat(file_path, num_char);
#endif

	ercd = BF_Dcf_Db_Make_File_Path(file_path, file_name, fileno);
	if(ercd != FSDCF_ERRNO_OK){
		return ercd;
	}

	strcat(file_path, ".");
	strcat(file_path, ext);

	return 0;
}

#ifdef CO_A_FILE_NAME_LIST	// CO_A_FILE_NAME_LIST
/**
 * @brief modify extension of file name.
 * @param i:*ext 
 * @param i/o:*file_path
 * @return error code
 */
static FW_INT32 dcf_controller_modify_file_path_ext(FW_CHAR* ext, FW_CHAR* file_path)
{
	FW_CHAR* ext_name;
	
	ext_name = strrchr(file_path, '.');
	if (ext_name == NULL) {
		return 1;
	}	
	
    ext_name++;
   	strcpy(ext_name, ext);
	
	return 0;
}
#endif	// A CO_A_FILE_NAME_LIST  END

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---

/**
 * @brief make path of new DCF number file(Absolutly).
 * @param i:*ext 
 * @param 0:*file_path
 * @return error code
 */
static FW_INT32 dcf_controller_make_new_file_path_abs(FW_CHAR* ext, FW_CHAR* file_path)
{
	FW_INT32 ercd;
	FW_CHAR rel_path[32]; // relative path
	FW_CHAR dir_path_rel[32];
	FW_UCHAR attr;
	FW_UCHAR ext_type;

	ercd = BF_Dcf_Db_Get_Type_By_Ext(ext, &ext_type);
	if(0 == ercd){
		if(!FJ_HostDcfCreateFileCB(ext_type)) {
			return FSDCF_ERRNO_CREATE_FILE;
		}
	}
	else {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("FJ_HostDcfCreateFileCB False\n"));
#endif
		return FSDCF_ERRNO_PARAM_FAILED;
	}

	if(file_path == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	ercd = BF_Dcf_Db_Make_Dir_Path(dir_path_rel, (FW_CHAR*)g_bf_dcf_controller_dir_name, g_bf_dcf_controller_last_dir_no);
	if( ercd < 0 ) {
		return ercd;
	}

	ercd = dcf_controller_make_file_path(rel_path, (FW_CHAR*)g_bf_dcf_controller_file_name, g_bf_dcf_controller_last_file_no, ext);
	if(ercd != 0) {
		return ercd;
	}


	strcpy(file_path, (FW_CHAR*)g_bf_dcf_controller_dcim_path);

	ercd = BF_Mdf_Wrapper_Get_Attribute(file_path, &attr);
	if(ercd == D_BF_MDF_WRAPPER_ERR_NOENT){
		ercd = BF_Mdf_Wrapper_Mkdir(file_path, 0x01ff);
	}

	strcat(file_path, dir_path_rel);

	ercd = BF_Mdf_Wrapper_Get_Attribute(file_path, &attr);
	if(ercd == D_BF_MDF_WRAPPER_ERR_NOENT){
		ercd = BF_Mdf_Wrapper_Mkdir(file_path, 0x01ff);
	}
	strcat(file_path, "\\");
	strcat(file_path, rel_path);

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("path=%s\n", file_path));
#endif

	return 0;
}

#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---

/**
 * @brief make path of new DCF number file(Absolutly).
 * @param i:*ext 
 * @param 0:*file_path
 * @return error code
 */
static FW_INT32 dcf_controller_make_new_file_path_abs(FW_CHAR* ext, FW_CHAR* file_path)
{
	FW_INT32 ercd;
	FW_CHAR rel_path[32]; // relative path
	FW_CHAR dir_path_rel[32];
	FW_UCHAR attr;
	FW_UCHAR ext_type;

	ercd = BF_Dcf_Db_Get_Type_By_Ext(ext, &ext_type);
	if(0 == ercd){
		if(!FJ_HostDcfCreateFileCB(ext_type)) {
			return FSDCF_ERRNO_CREATE_FILE;
		}
	}
	else {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("FJ_HostDcfCreateFileCB False\n"));
#endif
		return FSDCF_ERRNO_PARAM_FAILED;
	}

	if(file_path == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	ercd = BF_Dcf_Db_Make_Dir_Path(dir_path_rel, (FW_CHAR*)g_bf_dcf_controller_dir_name, g_bf_dcf_controller_last_dir_no);
	if( ercd < 0 ) {
		return ercd;
	}

	ercd = dcf_controller_make_file_path(rel_path, (FW_CHAR*)g_bf_dcf_controller_file_name, g_bf_dcf_controller_last_file_no, ext);
	if(ercd != 0) {
		return ercd;
	}
	

	strcpy(file_path, (FW_CHAR*)g_bf_dcf_controller_dcim_path);

	file_path[strlen(file_path)-1] = '\0';
	ercd = BF_Mdf_Wrapper_Get_Attribute(file_path, &attr);
	if(ercd != FSDCF_ERRNO_OK){
		ercd = BF_Mdf_Wrapper_Mkdir(file_path, 0x01ff);
	}

	strcat(file_path, "\\");
	strcat(file_path, dir_path_rel);
	
	ercd = BF_Mdf_Wrapper_Get_Attribute(file_path, &attr);
	if(ercd != FSDCF_ERRNO_OK){
		ercd = BF_Mdf_Wrapper_Mkdir(file_path, 0x01ff);
	}
	strcat(file_path, "\\");
	strcat(file_path, rel_path);

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("path=%s\n", file_path));
#endif

	return 0;
}

#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

/**
 * @brief make path of new thumbnail DCF number file(Absolutly).
 * @param i:*ext 
 * @param i:*main_file_path 
 * @param 0:*file_path
 * @return error code
 */
static FW_INT32 dcf_controller_make_new_thm_file_path_abs(FW_CHAR* ext, FW_CHAR* main_file_path, FW_CHAR* thm_file_path)
{
#if 0 // "THM" Not supported
	FW_INT32 ercd;
	FW_UCHAR attr;
	FW_UCHAR ext_type;

	if ((ext == NULL) || (main_file_path == NULL) || (thm_file_path == NULL)) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	
	if (strlen(main_file_path) < D_BF_DCF_IF_OBJ_NAME_MAX) {
		return FSDCF_ERRNO_PARAM_FAILED;
	}

	ercd = BF_Dcf_Db_Get_Type_By_Ext(ext, &ext_type);
	if(0 != ercd){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("BF_Dcf_Db_Get_Type_By_Ext False\n"));
#endif
		return FSDCF_ERRNO_PARAM_FAILED;
	}

	memset((char*)thm_file_path, '\0', sizeof(*thm_file_path));

	/* creat "I:\MISC" directory */
	strncpy((char*)thm_file_path, (char*)main_file_path, strlen("I:\\"));
	strcat((char*)thm_file_path, FJ_CUSTOM_DCF_ROOT_THM_DIR_NAME);
	ercd = BF_Mdf_Wrapper_Get_Attribute((char*)thm_file_path, &attr);
	if(ercd == D_BF_MDF_WRAPPER_ERR_NOENT){
		ercd = BF_Mdf_Wrapper_Mkdir((char*)thm_file_path, 0x01ff);
	}
	FJ_Fs_SetAttribute((char*)thm_file_path, ATTR_HIDDEN);

	/* creat "I:\MISC\THM" directory */
	strcat((char*)thm_file_path, "\\");
	strcat((char*)thm_file_path, FJ_CUSTOM_DCF_THM_DIR_NAME);
	ercd = BF_Mdf_Wrapper_Get_Attribute((char*)thm_file_path, &attr);
	if(ercd == D_BF_MDF_WRAPPER_ERR_NOENT){
		ercd = BF_Mdf_Wrapper_Mkdir((char*)thm_file_path, 0x01ff);
	}
	
	/* creat "I:\MISC\THM\100" directory */
	strcat((char*)thm_file_path, "\\");
	strncat((char*)thm_file_path, (char*)&main_file_path[8], 3);
	ercd = BF_Mdf_Wrapper_Get_Attribute((char*)thm_file_path, &attr);
	if(ercd == D_BF_MDF_WRAPPER_ERR_NOENT){
		ercd = BF_Mdf_Wrapper_Mkdir((char*)thm_file_path, 0x01ff);
	}

	if (strcmp(ext, "THM") == 0) {
		/* creat "I:\MISC\THM\100\MAX_0001.THM" directory */
		strcat((char*)thm_file_path, "\\");
		strncat((char*)thm_file_path, (char*)&main_file_path[17], 8);
		strcat((char*)thm_file_path, (const char*)".THM");
	}
	else if ((strcmp(ext, "MP4") == 0) || (strcmp(ext, "MOV") == 0)) {
		/* creat "I:\MISC\THM\100\MAX_0001_THM.MOV" directory */
		strcat((char*)thm_file_path, "\\");
		strncat((char*)thm_file_path, (char*)&main_file_path[17], 8);
		strcat((char*)thm_file_path, (const char*)"_THM");
		strncat((char*)thm_file_path, (char*)&main_file_path[25], 4);
	}

	return 0;
#else  // "THM" Not supported
	return FSDCF_ERRNO_NOT_SUPPORT_FILE;
#endif  // "THM" Not supported

}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---

/**
 * @brief create latest file.
 * @param i:*ext create file ext
 * @param o:*file_id pointer of file id
 * @param o:*make_filepath make filepath
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_create_new_file(FW_CHAR* ext, FW_INT32* file_id, FW_CHAR* make_filepath)
{
	FW_INT32 ercd;

	if(file_id == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	/* make new dcf file path */
	ercd = dcf_controller_make_new_file_path_abs(ext, make_filepath);
	if( ercd != 0 ) {
		return ercd;
	}

	/* create file */
	*file_id = BF_Mdf_Wrapper_Create(make_filepath, D_BF_MDF_WRAPPER_ATTR_NORMAL);
	if( *file_id < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Create() failed: file_id=%x, path=%s\n", *file_id, make_filepath));
#endif
		ercd = *file_id;
	}

	return ercd;
}

#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---

/**
 * @brief create latest file.
 * @param i:*ext create file ext
 * @param o:*file_id pointer of file id
 * @param o:*make_filepath make filepath
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_create_new_file(FW_CHAR* ext, FW_INT32* file_id, FW_CHAR* make_filepath)
{
	FW_INT32 ercd;

	if(file_id == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	/* make new dcf file path */
	ercd = dcf_controller_make_new_file_path_abs(ext, make_filepath);
	if( ercd != 0 ) {
		return ercd;
	}

	/* create file */
	*file_id = BF_Mdf_Wrapper_Create(make_filepath, D_BF_MDF_WRAPPER_ATTR_NORMAL);
	if( *file_id == 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Create() failed: file_id=%x, path=%s\n", *file_id, make_filepath));
#endif
		ercd = *file_id;
	}

	return ercd;
}

#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

#ifdef CO_A_FILE_NAME_LIST	// CO_A_FILE_NAME_LIST
/**
 * @brief create file by file name list.
 * @param i:*ext create file ext
 * @param o:*file_id pointer of file id
 * @param o:*make_filepath make filepath
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_create_file_by_list(FW_CHAR* ext, FW_INT32* file_id, FW_CHAR* make_filepath)
{
	FW_INT32 ercd;

	if (file_id == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	/* make new dcf file path */
	ercd = dcf_controller_modify_file_path_ext(ext, make_filepath);
	if( ercd != 0 ) {
		return ercd;
	}

	/* create file */
	*file_id = BF_Mdf_Wrapper_Create(make_filepath, D_BF_MDF_WRAPPER_ATTR_NORMAL);
#if defined(CO_FS_USED_PRFILE2)
	if( *file_id == 0 ){
#else
	if( *file_id < 0 ){
#endif
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Create() failed: file_id=%x, path=%s\n", *file_id, make_filepath));
#endif
		ercd = *file_id;
	}

	return ercd;
}
#endif	// A CO_A_FILE_NAME_LIST  END

/**
 * @brief close latest file.
 * @param i:file_id close file id
 * @param o:*make_filepath close filepath
 * @param i:remove_flg remove flag
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_close_new_file(FW_INT32 file_id, FW_CHAR* make_filepath, E_BF_DCF_IF_REMOVE_FLG remove_flg)
{
	FW_INT32 ercd;

	/* close file */
	ercd = BF_Mdf_Wrapper_Close(file_id);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Close() failed: file_id=%x\n", file_id));
#endif
		if(BF_Mdf_Wrapper_Remove(make_filepath) != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("BF_Mdf_Wrapper_Remove() failed: path=%s\n", make_filepath));
#endif
		}
		return ercd;
	}

	/* check remove flag */
	if(remove_flg == E_BF_DCF_IF_REMOVE_FLG_ON){
		ercd = BF_Mdf_Wrapper_Remove(make_filepath);
		if(ercd != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove() failed: ercd=%d, path=%s\n", ercd, make_filepath));
#endif
		}
		return ercd;
	}

	/* append file to dcfdb */
	ercd = BF_Dcf_Db_Append_Latest_File(make_filepath);
	if( ercd != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Dcf_Db_Append_Latest_File() failed: ercd=%d, path=%s\n", ercd, make_filepath));
#endif
		if(BF_Mdf_Wrapper_Remove(make_filepath) != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("BF_Mdf_Wrapper_Remove() failed: path=%s\n", make_filepath));
#endif
		}
	}

	return ercd;
}


/**
 * @brief Wrapper of BF_Fs_Controller_Set_Lseek_Write.
 * @param i:file_id the file number of target file
 * @param i:offset the offset value (byte) from the standard position
 * @param i:addr the pointer of area which is stored the write data
 * @param i:count the write data size
 * @param o:*length the pointer of area which is stored the actual written size
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_w_lseek_write(FW_INT32 file_id, FW_ULONG offset, FW_ULONG addr, FW_ULONG count, FW_ULONG* length)
{
	FW_INT32 ercd;

	/* lseek and write */
	ercd = BF_Fs_Controller_Set_Lseek_Write(file_id, (FW_INT32)offset, addr, count, length);
	return ercd;
}

/**
 * @brief get option file path.
 * @param i:*full_file_path full file path
 * @param i:file_type file type(of full_file_path)
 * @param o:*opt_file_path option file path
 * @return error code
 */
static FW_INT32 dcf_controller_get_index_option_file_path_abs(FW_CHAR* full_file_path, FW_CHAR file_type, FW_CHAR* opt_file_path)
{

	strcpy(opt_file_path, full_file_path);

	/* get option file exist */
	if(BF_Dcf_Controller_Is_File_Type("JPG", file_type)
		|| BF_Dcf_Controller_Is_File_Type("WAV", file_type)) {

		strcpy(opt_file_path + 26, "WAV");
	}
	else {
		return E_BF_DCF_IF_OPTION_FILE_NOTHING;
	}
	return FSDCF_ERRNO_OK;
}


#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---

static FW_INT32 dcf_controller_get_total_file_count(FW_CHAR* dir_name, FW_INT32* total_file_num)
{
	FW_INT32 ercd;
	FW_CHAR file_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32 dNo_dir;
	T_BF_FS_IF_Dir_Info dir_dir;
	FW_INT32 cnt;
	FW_INT32 file_cnt;

	g_bf_dcf_controller_ret_err = 0;

	ercd = BF_Mdf_Wrapper_Opendir(dir_name, &dNo_dir);
	if(FSDCF_ERRNO_OK != ercd) {
		BF_Mdf_Wrapper_Closedir(dNo_dir);
		return ercd;
	}

	strcpy(file_name, dir_name);
	strcat(file_name, "\\");

	file_cnt = 0;
	cnt = 0;
	for(;;) {
		if(0 == cnt) {
			if( BF_Dcf_If_Receiver_Is_Async_Stop() == E_BF_DCF_IF_RECIVER_ASYNC_STOP ){
				g_bf_dcf_controller_ret_err = 0;
				break;
			}
		}

		cnt = (cnt + 1) & 0x0F;

		ercd = BF_Mdf_Wrapper_Readdir(dNo_dir, &dir_dir);
		if(0 != ercd) {
			if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
				// end of object
				break;
			}
			BF_Mdf_Wrapper_Closedir(dNo_dir);
			return ercd;
		}

		if('.' == dir_dir.fname[0]) {
			if(0 == dir_dir.fname[1]) {
				continue;
			}
			if(('.' == dir_dir.fname[1]) && (0 == dir_dir.fname[2])) {
				continue;
			}
		}

		file_cnt++;
	}

	ercd = BF_Mdf_Wrapper_Closedir(dNo_dir);
	if(ercd != 0) {
		return ercd;
	}

	*total_file_num = file_cnt;

	return g_bf_dcf_controller_ret_err;
}

#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---

static FW_INT32 dcf_controller_get_total_file_count(FW_CHAR* dir_name, FW_INT32* total_file_num)
{
	FW_INT32 ercd;
	FW_CHAR file_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR file_path[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	T_BF_FS_IF_ENTRY dirent = {0};
	FW_INT32 cnt;
	FW_INT32 file_cnt;
	BOOL first_dir_flag = TRUE;

	g_bf_dcf_controller_ret_err = 0;

	strcpy(file_name, dir_name);
	ercd = BF_Mdf_Wrapper_Chdir(file_name);
	if (ercd != FSDCF_ERRNO_OK) {
		return ercd;
	}

	file_cnt = 0;
	cnt = 0;
	for(;;) {
		if(0 == cnt) {
			if( BF_Dcf_If_Receiver_Is_Async_Stop() == E_BF_DCF_IF_RECIVER_ASYNC_STOP ){
				g_bf_dcf_controller_ret_err = 0;
				break;
			}
		}

		cnt = (cnt + 1) & 0x0F;
		
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

		file_cnt++;
	}

	*total_file_num = file_cnt;

	return g_bf_dcf_controller_ret_err;
}

#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

/**
 * @brief delete all in current directory
 * @param i:*media_type media type
 * @return error code
 * @note none
 * @attention none
 */
#if 1
FW_INT32 BF_Dcf_Controller_Del_All_Obj_In_Current_Dir(E_BF_MEDIA_STATE_MEDIA_TYPE* media_type)
{
	FW_INT32 ercd;
	FW_CHAR currentDirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_CHAR dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32 total_file_num;
	FW_INT32 file_cnt;

	if(media_type == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	if(*media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_CUR){
		;
	}
	else if(*media_type != BF_Media_State_Get_Used_Media()){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("BF_Dcf_Controller_Del_All_Obj_In_Current_Dir(): NOT USED MEDIA: used_media=%d, target_media=%d\n", BF_Media_State_Get_Used_Media(), *media_type));
#endif
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_INSERT_MEDIA;
		return g_bf_dcf_controller_ret_err;
	}
	else{
		; // DO NOTHING
	}

	ercd = BF_Dcf_Db_Read_Dir_Name(DCF_DB_CURRENT_DIR, currentDirName);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	strcpy(dir_name, (FW_CHAR*)g_bf_dcf_controller_dcim_path);
	strcat(dir_name, currentDirName);
	ercd = dcf_controller_get_total_file_count(dir_name, &total_file_num);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	file_cnt = 0;
	return dcf_controller_del_dir(total_file_num, &file_cnt);
}
#endif

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---

static FW_INT32 dcf_controller_del_dir(FW_INT32 total_file_num, FW_INT32* file_cnt)
{
	FW_INT32 ercd;
	FW_CHAR drive_name[D_BF_FS_IF_DRVNAME_MAX+1];
	FW_CHAR dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR file_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32 dNo_dir;
	T_BF_FS_IF_Dir_Info dir_dir;
	FW_CHAR currentDirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_INT32 len;
	FW_INT32 cur_file_cnt;
//	FW_INT32 total_file_num;
	T_BF_DCF_IF_FILE_INDEX file_index;
	T_BF_DCF_IF_FILE_INFO file_info;
	FW_CHAR ext[4];
	FW_INT32 objIndex_del;
	BOOL stop_flg;
	T_BF_DCF_DB_OBJ_INFO* ppObjInfo;
	FW_CHAR file_path[14];
	FW_CHAR obj_name[9];
	FW_SHORT pdir_index_num;
	FW_CHAR cur_drive_name[D_BF_FS_IF_DRVNAME_MAX+1];
	FW_CHAR curpath[31];
	FW_USHORT last_file_num;
	FW_SHORT del_count = 0;


	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Dcf_Controller_Close_Display_File();
	}

	cur_file_cnt = *file_cnt;

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	ercd = BF_Dcf_Db_Read_Dir_Name(DCF_DB_CURRENT_DIR, currentDirName);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = BF_Dcf_Db_Get_Dir_Index_Num(DCF_DB_CURRENT_DIR, &pdir_index_num);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* count up total file number */
	strcpy(dir_name, (FW_CHAR*)g_bf_dcf_controller_dcim_path);
	strcat(dir_name, currentDirName);
//	ercd = dcf_controller_get_total_file_count(dir_name, &total_file_num);
//	if(FSDCF_ERRNO_OK != ercd) {return ercd;}

	strcpy(file_name, dir_name);
	strcat(file_name, "\\");
	len = strlen(file_name);

	BF_Fs_Controller_Get_Cur_Drv(cur_drive_name);
	/* Get current directory */
	BF_Fs_Controller_Get_Cur_Dir(cur_drive_name, 31, curpath);

	/* A temporary current directory is changed. */
#if 1
	g_bf_dcf_controller_ret_err = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}
	strcat(drive_name, ":");
	ercd = BF_Fs_Controller_Chdir(drive_name);
	if( ercd != 0 ) {
		return ercd;
	}
#endif
	ercd = BF_Fs_Controller_Chdir(dir_name);
	if( ercd != 0 ) {
		return ercd;
	}

	/* Get last file number */
	ercd = BF_Dcf_Db_Get_Last_File_Number(pdir_index_num, &last_file_num);
	if( ercd != 0 ) {
		return ercd;
	}

//	cur_file_cnt = 0;
	stop_flg = FALSE;

	/* allow file delete */
	for(objIndex_del = 0; objIndex_del < 10000; objIndex_del++) {
		if( BF_Dcf_If_Receiver_Is_Async_Stop() == E_BF_DCF_IF_RECIVER_ASYNC_STOP ){
			g_bf_dcf_controller_ret_err = 0;
			stop_flg = TRUE;
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
			BF_Fs_User_Custom_Info_Printer(("DELETE ALL STOP\n"));
#endif
			break;
		}

		if(objIndex_del > last_file_num){
			g_bf_dcf_controller_ret_err = 0;
			break;
		}

		if(BF_Dcf_Db_Read_Obj_Info(pdir_index_num, objIndex_del, &ppObjInfo) != 0) {
			continue;
		}
		;

		if(ppObjInfo->objName[0] == 0) {
			continue;
		}
		if((ppObjInfo->attr & D_BF_MDF_WRAPPER_ATTR_READONLY) == D_BF_MDF_WRAPPER_ATTR_READONLY) {
			continue;
		}

		BF_Dcf_Db_Read_Obj_Name(pdir_index_num, ppObjInfo->objIndex, obj_name);

		BF_Dcf_Db_Change_File_Type_To_Char(ppObjInfo->extType, ext);

		// make file name
		strcpy(file_path, obj_name);
		strcat(file_path, ".");
		strcat(file_path, ext);

		cur_file_cnt++;

		/* file delete */
#if 0	// MDF custom
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_For_Multi_Delete(file_path);
#else
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(file_path);
#endif

		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
#if 0	// MDF custom
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_For_Multi_Delete() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_path));
#else
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_path));
#endif
#endif
			continue;
		}

		FJ_Host_NoticeProgressCB( cur_file_cnt, total_file_num );

		del_count++;
	}

	/* Set current directory */
#if 1
	strcat(cur_drive_name, ":");
	ercd = BF_Fs_Controller_Chdir(cur_drive_name);
	if( ercd != 0 ) {
		return ercd;
	}
#endif
	ercd = BF_Fs_Controller_Chdir(curpath);
	if( ercd != 0 ) {
		return ercd;
	}

	/* updata freesize cache */
	g_bf_dcf_controller_ret_err = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Sync(drive_name);

	// not allow is not deleted when not deleting allow file.
	if(del_count != 0){
		ercd = BF_Mdf_Wrapper_Opendir(dir_name, &dNo_dir);
		if(FSDCF_ERRNO_OK != ercd) {
	#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, dir_name));
	#endif
			BF_Mdf_Wrapper_Closedir(dNo_dir);
			return ercd;
		}

		/* not allow file delete */
		for(;;) {
			ercd = BF_Mdf_Wrapper_Readdir(dNo_dir, &dir_dir);
			if(0 != ercd) {
				if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
					// end of object
					break;
				}
	#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
	#endif
				BF_Mdf_Wrapper_Closedir(dNo_dir);
				return ercd;
			}

			if('.' == dir_dir.fname[0]) {
				if(0 == dir_dir.fname[1]) {
					continue;
				}
				if(('.' == dir_dir.fname[1]) && (0 == dir_dir.fname[2])) {
					continue;
				}
			}
			/* search object */
			if(0 != BF_Dcf_Db_Search_Obj_By_Name(currentDirName, dir_dir.fname, &(file_index.pack.dir_index), &(file_index.pack.obj_index))) {
				continue;
			}

			if(stop_flg == TRUE){
				if(file_index.pack.obj_index > ppObjInfo->objIndex) {/* pgr0872 */ /* pgr0000 */
					continue;
				}
			}

			if ((dir_dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
				continue;
			}
			if(0 != BF_Dcf_Db_Get_Index_File_Info(file_index, NULL, &file_info)) {
				continue;
			}
			if(file_info.file_attr == E_BF_DCF_IF_ATTR_RDONLY) {
				continue;
			}

			cur_file_cnt++;
			strcpy(file_name + len, dir_dir.fname);

	#if 0	// MDF custom
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Current_Entry(dNo_dir, &dir_dir);
	#else
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache((const FW_CHAR*)file_name);
	#endif

			if(stop_flg == FALSE){
				FJ_Host_NoticeProgressCB( cur_file_cnt, total_file_num );
			}

			if(g_bf_dcf_controller_ret_err != 0){
	#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
	#if 0	// MDF custom
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Current_Entry() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
	#else
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
	#endif
	#endif
				continue;
			}
		}

		ercd = BF_Mdf_Wrapper_Closedir(dNo_dir);
		if(ercd != 0) {
	#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
	#endif
			return ercd;
		}
	}

	/* Renewal of current directory */
	BF_Dcf_Db_Restruct_Current_Dir_Only();

	/* updata freesize cache */
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Sync(drive_name);

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF DELETE ALL END\n");
#endif

	*file_cnt = cur_file_cnt;
	return g_bf_dcf_controller_ret_err;
}

#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---

static FW_INT32 dcf_controller_del_dir(FW_INT32 total_file_num, FW_INT32* file_cnt)
{
	FW_INT32 ercd;
	FW_CHAR drive_name[D_BF_FS_IF_DRVNAME_MAX+1];
	FW_CHAR dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR file_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR currentDirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_INT32 len;
	FW_INT32 cur_file_cnt;
	T_BF_DCF_IF_FILE_INDEX file_index;
	T_BF_DCF_IF_FILE_INFO file_info;
	FW_CHAR ext[4];
	FW_INT32 objIndex_del;
	BOOL stop_flg;
	T_BF_DCF_DB_OBJ_INFO* ppObjInfo;
	FW_CHAR file_path[14];
	FW_CHAR obj_name[9];
	FW_SHORT pdir_index_num;
	FW_CHAR cur_drive_name[D_BF_FS_IF_DRVNAME_MAX+1];
	FW_CHAR curpath[31];
	FW_USHORT last_file_num;
	FW_SHORT del_count = 0;
	BOOL first_dir_flag = TRUE;
	T_BF_FS_IF_ENTRY dirent = {0};


	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Dcf_Controller_Close_Display_File();
	}

	cur_file_cnt = *file_cnt;

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	ercd = BF_Dcf_Db_Read_Dir_Name(DCF_DB_CURRENT_DIR, currentDirName);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = BF_Dcf_Db_Get_Dir_Index_Num(DCF_DB_CURRENT_DIR, &pdir_index_num);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* count up total file number */
	strcpy(dir_name, (FW_CHAR*)g_bf_dcf_controller_dcim_path);
	strcat(dir_name, currentDirName);

	strcpy(file_name, dir_name);
	strcat(file_name, "\\");
	len = strlen(file_name);

	BF_Fs_Controller_Get_Cur_Drv(cur_drive_name);
	/* Get current directory */
	BF_Fs_Controller_Get_Cur_Dir(cur_drive_name, 31, curpath);

	/* A temporary current directory is changed. */
#if 1
	g_bf_dcf_controller_ret_err = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}
	strcat(drive_name, ":");
	ercd = BF_Fs_Controller_Chdir(drive_name);
	if( ercd != 0 ) {
		return ercd;
	}
#endif
	ercd = BF_Fs_Controller_Chdir(dir_name);
	if( ercd != 0 ) {
		return ercd;
	}

	/* Get last file number */
	ercd = BF_Dcf_Db_Get_Last_File_Number(pdir_index_num, &last_file_num);
	if( ercd != 0 ) {
		return ercd;
	}

	stop_flg = FALSE;

	/* allow file delete */
	for(objIndex_del = 0; objIndex_del < 10000; objIndex_del++) {
		if( BF_Dcf_If_Receiver_Is_Async_Stop() == E_BF_DCF_IF_RECIVER_ASYNC_STOP ){
			g_bf_dcf_controller_ret_err = 0;
			stop_flg = TRUE;
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
			BF_Fs_User_Custom_Info_Printer(("DELETE ALL STOP\n"));
#endif
			break;
		}

		if(objIndex_del > last_file_num){
			g_bf_dcf_controller_ret_err = 0;
			break;
		}

		if(BF_Dcf_Db_Read_Obj_Info(pdir_index_num, objIndex_del, &ppObjInfo) != 0) {
			continue;
		}

		if(ppObjInfo->objName[0] == 0) {
			continue;
		}
		if((ppObjInfo->attr & D_BF_MDF_WRAPPER_ATTR_READONLY) == D_BF_MDF_WRAPPER_ATTR_READONLY) {
			continue;
		}

		BF_Dcf_Db_Read_Obj_Name(pdir_index_num, ppObjInfo->objIndex, obj_name);

		BF_Dcf_Db_Change_File_Type_To_Char(ppObjInfo->extType, ext);

		// make file name
		strcpy(file_path, obj_name);
		strcat(file_path, ".");
		strcat(file_path, ext);

		cur_file_cnt++;

		/* file delete */
#if 0	// MDF custom
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_For_Multi_Delete(file_path);
#else
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(file_path);
#endif

		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
#if 0	// MDF custom
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_For_Multi_Delete() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_path));
#else
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_path));
#endif
#endif
			continue;
		}

		FJ_Host_NoticeProgressCB( cur_file_cnt, total_file_num );

		del_count++;
	}

	/* Set current directory */
#if 1
	strcat(cur_drive_name, ":");
	ercd = BF_Fs_Controller_Chdir(cur_drive_name);
	if( ercd != 0 ) {
		return ercd;
	}
#endif
	ercd = BF_Fs_Controller_Chdir(curpath);
	if( ercd != 0 ) {
		return ercd;
	}

	/* updata freesize cache */
	g_bf_dcf_controller_ret_err = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Sync(drive_name);

	// not allow is not deleted when not deleting allow file.
	if(del_count != 0){
		ercd = BF_Mdf_Wrapper_Chdir(dir_name);
		if (ercd != FSDCF_ERRNO_OK) {
			return ercd;
		}

		/* not allow file delete */
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
			
			/* search object */
			if(0 != BF_Dcf_Db_Search_Obj_By_Name(currentDirName, dirent.fs_fname, &(file_index.pack.dir_index), &(file_index.pack.obj_index))) {
				continue;
			}

			if(stop_flg == TRUE){
				if(file_index.pack.obj_index > ppObjInfo->objIndex) {/* pgr0872 */ /* pgr0000 */
					continue;
				}
			}

			if ((dirent.fs_attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
				continue;
			}
			if(0 != BF_Dcf_Db_Get_Index_File_Info(file_index, NULL, &file_info)) {
				continue;
			}
			if(file_info.file_attr == E_BF_DCF_IF_ATTR_RDONLY) {
				continue;
			}

			cur_file_cnt++;
			strcpy(file_name + len, dirent.fs_fname);

	#if 0	// MDF custom
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Current_Entry(dNo_dir, &dir_dir);
	#else
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache((const FW_CHAR*)file_name);
	#endif

			if(stop_flg == FALSE){
				FJ_Host_NoticeProgressCB( cur_file_cnt, total_file_num );
			}

			if(g_bf_dcf_controller_ret_err != 0){
	#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
	#if 0	// MDF custom
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Current_Entry() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
	#else
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
	#endif
	#endif
				continue;
			}
		}

	}

	/* Renewal of current directory */
	BF_Dcf_Db_Restruct_Current_Dir_Only();

	/* updata freesize cache */
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Sync(drive_name);

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF DELETE ALL END\n");
#endif

	*file_cnt = cur_file_cnt;
	return g_bf_dcf_controller_ret_err;
}

#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---


#if 0 // "THM" Not supported
static FW_INT32 dcf_controller_del_all_thm_dir(FW_INT32* total_file_cnt)
{
	FW_INT32 ercd;
	FW_CHAR drive_name[D_BF_FS_IF_DRVNAME_MAX+1];
	FW_CHAR thm_dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR cur_thm_dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32 dNo_dir;
	FS_dirent dir_dir;
	FW_INT32 del_count = 0;

	g_bf_dcf_controller_ret_err = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}
	strcat(drive_name, ":");

	/* thumbnail directory */
	memset(thm_dir_name, '\0', sizeof(thm_dir_name));
	strcpy(thm_dir_name, drive_name);
	strcat(thm_dir_name, "\\");
	strcat(thm_dir_name, FJ_CUSTOM_DCF_ROOT_THM_DIR_NAME);
	strcat(thm_dir_name, "\\");
	strcat(thm_dir_name, FJ_CUSTOM_DCF_THM_DIR_NAME);
	strcat(thm_dir_name, "\\");

	/* A temporary current directory is changed. */
	ercd = BF_Fs_Controller_Chdir(drive_name);
	if( ercd != 0 ) {
		return ercd;
	}
	
	ercd = BF_Fs_Controller_Chdir(thm_dir_name);
	if( ercd != 0 ) {
		return ercd;
	}

	// delete thumbnail file.
	ercd = BF_Mdf_Wrapper_Opendir(thm_dir_name, &dNo_dir);
	if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, thm_dir_name));
#endif
		BF_Mdf_Wrapper_Closedir(dNo_dir);
		return ercd;
	}

	*total_file_cnt = 0;
	
	/* not allow file delete */
	for(;;) {
		ercd = BF_Mdf_Wrapper_Readdir(dNo_dir, &dir_dir);
		if(0 != ercd) {
			if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
				// end of object
				break;
			}
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Closedir(dNo_dir);
			return ercd;
		}
		
		if('.' == dir_dir.fname[0]) {
			if(0 == dir_dir.fname[1]) {
				continue;
			}
			if(('.' == dir_dir.fname[1]) && (0 == dir_dir.fname[2])) {
				continue;
			}
		}

		if ((dir_dir.attributes & ATTR_DIRECTORY) != ATTR_DIRECTORY) {
			continue;
		}

		if (strlen(dir_dir.fname) != 3) {
			continue;
		}

		if (!isdigit((INT32)dir_dir.fname[0]) || !isdigit((INT32)dir_dir.fname[1]) || !isdigit((INT32)dir_dir.fname[2])) {
			continue;
		}

		memset(cur_thm_dir_name, '\0', sizeof(cur_thm_dir_name));
		strcpy(cur_thm_dir_name, thm_dir_name);
		strcat(cur_thm_dir_name, dir_dir.fname);
		strcat(cur_thm_dir_name, "\\");
		dcf_controller_del_single_thm_dir(cur_thm_dir_name, &del_count);

		*total_file_cnt += del_count;
	}

	ercd = BF_Mdf_Wrapper_Closedir(dNo_dir);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	/* updata freesize cache */
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Sync(drive_name);

	return g_bf_dcf_controller_ret_err;
}


static FW_INT32 dcf_controller_del_single_thm_dir(FW_CHAR* thm_dir_name, FW_INT32* del_count)
{
	FW_INT32 ercd;
	FW_CHAR file_name[64];
	FW_INT32 dNo_dir;
	FS_dirent dir_dir;
	FW_INT32 len;
	FW_CHAR ext[4];
	FW_UCHAR ext_type;
	FW_SHORT loop = 0;

	*del_count = 0;
	strcpy(file_name, thm_dir_name);
	len = strlen(thm_dir_name);

	/* delete allow file */
	ercd = BF_Mdf_Wrapper_Opendir(thm_dir_name, &dNo_dir);
	if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, thm_dir_name));
#endif
		BF_Mdf_Wrapper_Closedir(dNo_dir);
		return ercd;
	}

	for(;;) {
		ercd = BF_Mdf_Wrapper_Readdir(dNo_dir, &dir_dir);
		if(0 != ercd) {
			if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
				// end of object
				break;
			}
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Closedir(dNo_dir);
			return ercd;
		}

		if('.' == dir_dir.fname[0]) {
			if(0 == dir_dir.fname[1]) {
				continue;
			}
			if(('.' == dir_dir.fname[1]) && (0 == dir_dir.fname[2])) {
				continue;
			}
		}

		if ((dir_dir.attributes & ATTR_DIRECTORY) == ATTR_DIRECTORY) {
			continue;
		}

		if ((dir_dir.attributes & ATTR_READONLY) == ATTR_READONLY) {
			continue;
		}

		for (loop = strlen(dir_dir.fname); loop >= 0; loop--) {
			if (dir_dir.fname[loop] == '.') {
				break;
			}
		}

		strcpy(ext, dir_dir.fname + loop + 1);
		ercd = BF_Dcf_Db_Get_Type_By_Ext(ext, &ext_type);
		if (0 != ercd) {
			continue;
		}

		(*del_count)++;
		strcpy(file_name + len, dir_dir.fname);

#if 0	// MDF custom
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Current_Entry(dNo_dir, &dir_dir);
#else
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache((const FW_CHAR*)file_name);
#endif

		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
#if 0	// MDF custom
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Current_Entry() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
#else
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
#endif
#endif
			continue;
		}
	}

	ercd = BF_Mdf_Wrapper_Closedir(dNo_dir);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}
	
	return g_bf_dcf_controller_ret_err;
}
#endif // "THM" Not supported

#if 1
static FW_INT32 dcf_controller_set_attr_dir(FW_INT32 total_file_num, FW_INT32* file_cnt, T_BF_DCF_IF_ATTR_ALL_DATA* attr_all_data)
{
	FW_INT32			ercd;
	FW_CHAR				drive_name[D_BF_FS_IF_DRVNAME_MAX+1];
	FW_CHAR				dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR				file_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32			dNo_dir;
	T_BF_FS_IF_Dir_Info			dir_dir;
	FW_CHAR				currentDirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_INT32			len;
	FW_INT32			cur_file_cnt;
	FW_INT32			total_file_num_dir;
	T_BF_DCF_IF_FILE_INDEX	file_index;
	FW_CHAR				ext[4];
	FW_INT32			objIndex_set;
	BOOL				stop_flg;
	T_BF_DCF_DB_OBJ_INFO*	pObjInfo;
	FW_CHAR				file_path[14];
	FW_CHAR				obj_name[9];
	FW_SHORT			dir_index_num;
	FW_CHAR				cur_drive_name[D_BF_FS_IF_DRVNAME_MAX+1];
	FW_CHAR				curpath[31];
	FW_USHORT			last_file_num;
	FW_SHORT			attr_count = 0;
#if 0	// MDF custom
	FW_UCHAR			attr_tmp;
#endif

	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Dcf_Controller_Close_Display_File();
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	ercd = BF_Dcf_Db_Read_Dir_Name(DCF_DB_CURRENT_DIR, currentDirName);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = BF_Dcf_Db_Get_Dir_Index_Num(DCF_DB_CURRENT_DIR, &dir_index_num);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* count up total file number */
	strcpy(dir_name, (FW_CHAR*)g_bf_dcf_controller_dcim_path);
	strcat(dir_name, currentDirName);
	ercd = dcf_controller_get_total_file_count(dir_name, &total_file_num_dir);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	strcpy(dir_name, (FW_CHAR*)g_bf_dcf_controller_dcim_path);
	strcat(dir_name, currentDirName);

	strcpy(file_name, dir_name);
	strcat(file_name, "\\");
	len = strlen(file_name);

	BF_Fs_Controller_Get_Cur_Drv(cur_drive_name);
	/* Get current directory */
	BF_Fs_Controller_Get_Cur_Dir(cur_drive_name, 31, curpath);

	/* A temporary current directory is changed. */
	ercd = BF_Fs_Controller_Chdir(dir_name);
	if( ercd != 0 ) {
		return ercd;
	}

	/* Get last file number */
	ercd = BF_Dcf_Db_Get_Last_File_Number(dir_index_num, &last_file_num);
	if( ercd != 0 ) {
		return ercd;
	}

	cur_file_cnt = 0;
//	cur_file_cnt = *file_cnt;
	stop_flg = FALSE;

	/* allow file set */
	for(objIndex_set = 0; objIndex_set < 10000; objIndex_set++) {
		if( BF_Dcf_If_Receiver_Is_Async_Stop() == E_BF_DCF_IF_RECIVER_ASYNC_STOP ){
			g_bf_dcf_controller_ret_err = 0;
			stop_flg = TRUE;
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
			BF_Fs_User_Custom_Info_Printer(("DELETE ALL STOP\n"));
#endif
			break;
		}

		if(objIndex_set > last_file_num){
			g_bf_dcf_controller_ret_err = 0;
			break;
		}

		if(BF_Dcf_Db_Read_Obj_Info(dir_index_num, objIndex_set, &pObjInfo) != 0) {
			continue;
		}
		;

		if(pObjInfo->objName[0] == 0) {
			continue;
		}

		BF_Dcf_Db_Read_Obj_Name(dir_index_num, pObjInfo->objIndex, obj_name);

		BF_Dcf_Db_Change_File_Type_To_Char(pObjInfo->extType, ext);

		// make file name
		strcpy(file_path, obj_name);
		strcat(file_path, ".");
		strcat(file_path, ext);

		cur_file_cnt++;

		/* file delete */
#if 0	// MDF custom
		g_bf_dcf_controller_ret_err = dcf_controller_set_protect_attribute_for_multi_set(file_path, (FW_UCHAR)attr_all_data->all_attr);
#else
		g_bf_dcf_controller_ret_err = dcf_controller_set_protect_attribute(file_path, (FW_UCHAR)attr_all_data->all_attr);
#endif

		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
#if 0	// MDF custom
			BF_Fs_User_Custom_Err_Printer(("dcf_controller_set_protect_attribute_for_multi_set() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
#else
			BF_Fs_User_Custom_Err_Printer(("dcf_controller_set_protect_attribute() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
#endif
#endif
			continue;
		}


//		FJ_Host_NoticeProgressCB( cur_file_cnt, total_file_num );
		FJ_Host_NoticeProgressCB( (cur_file_cnt + *file_cnt), total_file_num );
		attr_count++;
	}

	/* Set current directory */
	ercd = BF_Fs_Controller_Chdir(curpath);
	if( ercd != 0 ) {
		return ercd;
	}

	/* updata freesize cache */
	g_bf_dcf_controller_ret_err = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Sync(drive_name);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}

//	if (cur_file_cnt >= total_file_num) {
	if (cur_file_cnt >= total_file_num_dir) {
		// End
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
		BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF CHANGE ATTRIBUTE ALL END\n");
#endif
		/* update attribute (DCF-DB) */
		ercd = BF_Dcf_Db_Update_Attr_Dir(attr_all_data->all_attr);
		if(ercd != 0) {
			return ercd;
		}

		return g_bf_dcf_controller_ret_err;
	}

	// not allow is not update when not updating allow file.
	if(attr_count != 0){
		ercd = BF_Mdf_Wrapper_Opendir(dir_name, &dNo_dir);
		if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, dir_name));
#endif
			BF_Mdf_Wrapper_Closedir(dNo_dir);
			return ercd;
		}

		for(;;) {
			ercd = BF_Mdf_Wrapper_Readdir(dNo_dir, &dir_dir);
			if(0 != ercd) {
				if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
					// end of object
					break;
				}
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif
				BF_Mdf_Wrapper_Closedir(dNo_dir);
				return ercd;
			}

			if('.' == dir_dir.fname[0]) {
				if(0 == dir_dir.fname[1]) {
					continue;
				}
				if(('.' == dir_dir.fname[1]) && (0 == dir_dir.fname[2])) {
					continue;
				}
			}
			/* search object */
			if(0 != BF_Dcf_Db_Search_Obj_By_Name(currentDirName, dir_dir.fname, &(file_index.pack.dir_index), &(file_index.pack.obj_index))) {
				continue;
			}

			if(stop_flg == TRUE){
				if(file_index.pack.obj_index > pObjInfo->objIndex) { /* pgr0872 */ /* pgr0000 */
					continue;
				}
			}

			if ((dir_dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
				continue;
			}
			if (dir_dir.attributes == (FW_UCHAR)attr_all_data->all_attr) {
				continue;
			}
			cur_file_cnt++;
			strcpy(file_name + len, dir_dir.fname);

#if 0	// MDF custom
			attr_tmp = (dir_dir.attribute & D_BF_DCF_CONTROLLER_ATTR_PROTECT_FILTER) | (FW_UCHAR)attr_all_data->all_attr;
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Set_Attribute_Current_Entry(dNo_dir, &dir_dir, (FW_UCHAR)attr_all_data->all_attr);
#else
			g_bf_dcf_controller_ret_err = dcf_controller_set_protect_attribute(file_name, (FW_UCHAR)attr_all_data->all_attr);
#endif

			if(stop_flg == FALSE){
	//			FJ_Host_NoticeProgressCB( cur_file_cnt, total_file_num );
				FJ_Host_NoticeProgressCB( (cur_file_cnt + *file_cnt), total_file_num );
			}

			if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
#if 0	// MDF custom
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Attribute_Current_Entry() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
#else
				BF_Fs_User_Custom_Err_Printer(("dcf_controller_set_protect_attribute() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
#endif
#endif
				continue;
			}
		}

		ercd = BF_Mdf_Wrapper_Closedir(dNo_dir);
		if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
			return ercd;
		}
	}

	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Sync(drive_name);

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF CHANGE ATTRIBUTE ALL END\n");
#endif
	/* update attribute (DCF-DB) */
	if (stop_flg == TRUE) {
		if(attr_count != 0){
			ercd = BF_Dcf_Db_Update_Attr_Dir_For_Stop(attr_all_data->all_attr,  pObjInfo->objIndex); /* pgr0872 */ /* pgr0000 */
		}
	}
	else {
		ercd = BF_Dcf_Db_Update_Attr_Dir(attr_all_data->all_attr);
	}
	if(ercd != 0) {
		return ercd;
	}

//	*file_cnt = cur_file_cnt;
	*file_cnt += cur_file_cnt;
	return g_bf_dcf_controller_ret_err;
}
#endif

#if 0
/**
 * @brief orginal file read and destination file write for copy.
 * @param i:org_file_id org file no
 * @param i:dst_file_id dst file no
 * @param i:addr read and write address
 * @param i:size read and write size
 * @param i:org_file_size org file size
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_copy_read_write(FW_INT32 org_file_id, FW_INT32 dst_file_id, FW_ULONG addr, FW_ULONG size, FW_ULONG org_file_size, E_BF_MEDIA_STATE_MEDIA_TYPE org_media, E_BF_MEDIA_STATE_MEDIA_TYPE dst_media)
{
	FW_INT32 ercd;
	FW_ULONG total_size = 0;
	FW_ULONG read_len;
	FW_ULONG write_len;

	while (total_size < org_file_size){
		/* change to org media */
		BF_Fs_Controller_Change_Drive_Info(org_media);

		/* read org file data */
		ercd = dcf_controller_read_with_measure_speed(org_file_id, addr, size, &read_len);
		if( ercd != 0 ) {
			return ercd;
		}

		/* change to dst media */
		BF_Fs_Controller_Change_Drive_Info(dst_media);

		/* write dst file data */
		ercd = BF_Mdf_Wrapper_Write(dst_file_id, (const FW_VOID*)addr, read_len, &write_len);
		if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, addr=%x, req_size=%d, ret_size=%d\n", ercd, addr, size, write_len));
#endif
			return ercd;
		}

		total_size += write_len;

		if(total_size == org_file_size){
			/* copy end */
			break;
		}
	}
	return 0;
}

/**
 * @brief copy option file
 * @param I : T_BF_DCF_IF_FILE_INDEX tgt_index
 *        I : E_BF_MEDIA_STATE_MEDIA_TYPE org_media
 *        I : E_BF_MEDIA_STATE_MEDIA_TYPE dst_media
 *        O : FW_CHAR* org_filepath
 *        O : FW_CHAR* dst_filepath
 *        I : FW_ULONG addr
 *        I : FW_ULONG size
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_copy_option_file(T_BF_DCF_IF_FILE_INDEX tgt_index, E_BF_MEDIA_STATE_MEDIA_TYPE org_media, E_BF_MEDIA_STATE_MEDIA_TYPE dst_media, FW_CHAR* org_filepath, FW_CHAR* dst_filepath, FW_ULONG addr, FW_ULONG size)
{
	FW_INT32 ercd = 0;
	FW_CHAR opt_org_filepath[32];
	T_BF_FS_IF_STAT org_file_stat;
	FW_INT32 org_file_id;
	FW_INT32 dst_file_id;
	FW_DOUBLE free_size;
	FW_CHAR ext[4];
	FW_UCHAR file_type;
	FW_CHAR file_path[16];

	ercd = BF_Dcf_Controller_Get_File_Path(org_filepath, file_path);
	if( ercd != 0 ) {
		return ercd;
	}

	/* check file type */
	ercd = BF_Dcf_Db_Get_Type_By_Ext((file_path + D_BF_DCF_CONTROLLER_FILENAME_LEN), &file_type);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* get org filepath */
	ercd = dcf_controller_get_index_option_file_path_abs(org_filepath, file_type, opt_org_filepath);
	if(ercd != 0) {
		return ercd;
	}
	strcpy(ext, (opt_org_filepath + strlen(opt_org_filepath) - 3));

	/* get org file size */
	ercd = BF_Mdf_Wrapper_Stat(opt_org_filepath, &org_file_stat);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Stat() failed: ercd=%d, fname=%s\n", ercd, opt_org_filepath));
#endif
		return ercd;
	}

	/* open org file */
	org_file_id = BF_Mdf_Wrapper_Open(opt_org_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
	if( org_file_id < 0 ) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", org_file_id, opt_org_filepath));
#endif
		return org_file_id;
	}

	while(1){
		BF_Fs_Controller_Change_Drive_Info(dst_media);
		/* check dst media free_space */
		ercd = BF_Fs_Controller_Get_FreeSize(dst_media, &free_size);
		if(ercd != 0){
			return ercd;
		}
		if(free_size <= org_file_stat.fileSize){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("FREE SPACE OVER: size=%lf\n", free_size));
#endif
			ercd = FSDCF_ERRNO_FREE_SPACE_OVER;
			break;
		}

		/* make dst filepath */
		strcpy((dst_filepath + strlen(dst_filepath) - 3), ext);

		/* create dst file */
		dst_file_id = BF_Mdf_Wrapper_Create(dst_filepath, D_BF_MDF_WRAPPER_ATTR_NORMAL);
		if( dst_file_id < 0 ) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Create() failed: file_id=%x, path=%s\n", dst_file_id, dst_filepath));
#endif
			ercd = dst_file_id;
			break;
		}

		/* read and write */
		ercd = dcf_controller_copy_read_write(org_file_id, dst_file_id, addr, size, org_file_stat.fileSize, org_media, dst_media);

		/* close dst file */
		BF_Fs_Controller_Change_Drive_Info(dst_media);
		BF_Mdf_Wrapper_Close(dst_file_id);

		if( ercd != 0 ) {
			/* remove dst file */
			BF_Mdf_Wrapper_Remove(dst_filepath);
		}

		break;
	}

	/* close org file */
	BF_Fs_Controller_Change_Drive_Info(org_media);
	BF_Mdf_Wrapper_Close(org_file_id);

	return ercd;
}
#endif

/**
 * @brief analize JPEG thumbnail and get Quantization Table.
 * @param i:file_id file id
 * @param i:qt_type quantization table type
 * @param o:qt quant table
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_get_jpeg_qt_data(FW_INT32 file_id, E_BF_DCF_IF_QT_TYPE qt_type, T_BF_DCF_IF_JPEG_QT* qt)
{
	FW_INT32 ercd, ercd2;
	FW_INT32 i;
	FW_UCHAR* buffer;
	FW_ULONG result;
	FW_USHORT qt_length;
	FW_ULONG start_offset = 0;
	FW_ULONG value_offset = 0;
	FW_ULONG qt_data_offset;

	switch(qt_type){
		case E_BF_DCF_IF_QT_TYPE_MAIN:
			ercd = BF_Dcf_Exif_Get_Image_Offset_Jpeg(file_id, &qt_data_offset, NULL, NULL);
			if(ercd != D_BF_FS_USER_CUSTOM_OK) {
				return ercd;
			}
			break;

		case E_BF_DCF_IF_QT_TYPE_THUMB:
			ercd = BF_Dcf_Exif_Get_Image_Offset_Jpeg(file_id, NULL, &qt_data_offset, NULL);
			if(ercd != D_BF_FS_USER_CUSTOM_OK) {
				return ercd;
			}
			break;

		case E_BF_DCF_IF_QT_TYPE_VGA:
			ercd = BF_Dcf_Exif_Get_Image_Offset_Jpeg(file_id, NULL, NULL, &qt_data_offset);
			if(ercd != D_BF_FS_USER_CUSTOM_OK) {
				return ercd;
			}
			break;

		default:
			return FSDCF_ERRNO_PARAM_FAILED;
	}


	/* memory allocation */
	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&buffer);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	while(1){
		/* seek to thumbnail image data start */
		ercd = BF_Mdf_Wrapper_Lseek(file_id, qt_data_offset, D_BF_MDF_WRAPPER_SEEK_SET, &result);
		if(ercd != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		ercd = BF_Dcf_Exif_Search_Half_Word_Code(buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, 0xFFDB, start_offset, &value_offset);
		if(ercd != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Dcf_Exif_Search_Half_Word_Code() failed: ercd=%d\n", ercd));
#endif
			break;
		}

		/* get quant table length */
		qt_length = *((FW_USHORT*)(buffer + value_offset + 2));

		switch(qt_length){
			case 0x0043: /* length(2byte) + (table element precision + table id)(1byte) + table element(64byte) */
				qt->qt_flg_y = 1;
				qt->qt_flg_cb = 0;
				qt->qt_flg_cr = 0;
				break;

			case 0x0084: /* + (table element precision + table id)(1byte) + table element(64byte) */
				qt->qt_flg_y = 1;
				qt->qt_flg_cb = 1;
				qt->qt_flg_cr = 0;
				break;

			case 0x00C5: /* + (table element precision + table id)(1byte) + table element(64byte) */
				qt->qt_flg_y = 1;
				qt->qt_flg_cb = 1;
				qt->qt_flg_cr = 1;
				break;

			default:
				qt->qt_flg_y = 0;
				qt->qt_flg_cb = 0;
				qt->qt_flg_cr = 0;
				break;
		}

		if(qt->qt_flg_y == 1){
			for(i = 0; i < 64; i++){
				qt->qt_y[i] = *((buffer + value_offset + 5 + i));
			}
		}

		if(qt->qt_flg_cb == 1){
			for(i = 0; i < 64; i++){
				qt->qt_cb[i] = *((buffer + value_offset + 70 + i));
			}
		}

		if(qt->qt_flg_cr == 1){
			for(i = 0; i < 64; i++){
				qt->qt_cr[i] = *((buffer + value_offset + 135 + i));
			}
		}
		break;
	}

	ercd2 = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)buffer);
	if(ercd2 != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd2));
#endif
		ercd = ercd2;
	}

	return ercd;
}

/**
 * @brief 		Check file exist in the directory. 
 * @param		i:FW_SHORT dir_index
 * @param		o:UCHAR* file_exist
 *                file exist : 0x01
 *                file not exist : 0x00
 * @return 		error code
 * @note		None
 * @attention 	None
 */
static FW_INT32 dcf_controller_db_check_dir_file_exist(FW_USHORT dir_index, FW_UCHAR* file_exist)
{
	FW_INT32 ercd;
	FW_CHAR DirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_CHAR dir_path[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	T_BF_FS_IF_Dir_Info dir;
	FW_INT32 dNo;

	/* check parameter */
	if(file_exist == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	ercd = BF_Dcf_Db_Read_Dir_Name(dir_index, DirName);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* make directory path */
	strcpy(dir_path, (FW_CHAR*)g_bf_dcf_controller_dcim_path);
	strcat(dir_path, DirName);

	ercd = BF_Mdf_Wrapper_Opendir(dir_path, &dNo);
	if(FSDCF_ERRNO_OK != ercd) {
		BF_Mdf_Wrapper_Closedir(dNo);
		return ercd;
	}

	*file_exist = 0x00;

	for(;;) {
		ercd = BF_Mdf_Wrapper_Readdir(dNo, &dir);
		if(0 != ercd) {
			if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
				// end of object
				break;
			}
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

		*file_exist |= 0x01;
		break;
	}

	BF_Mdf_Wrapper_Closedir(dNo);

	return FSDCF_ERRNO_OK;
}


#if 1	/* for Multi-Delete */
static FW_INT32 dcf_controller_check_image_select(T_BF_DCF_IF_FILE_INDEX* dcf_index, FW_UCHAR* list_index)
{
	FW_UCHAR index;								/*	Index for SelectList							*/

///#ifndef FJ_CUSTOM_DCF_DATE_DB_USE
	if ((dcf_index->pack.dir_index == 0) || (dcf_index->pack.obj_index == 0)) {
		/* Invalid dcf_index */
		return -1;
	}
///#endif // FJ_CUSTOM_DCF_DATE_DB_USE

	for (index = 0; index < g_bf_dcf_controller_select_list_index; index++) {
		if (dcf_index->index == g_bf_dcf_controller_select_list[index].index) {
			*list_index = index;
			return 0;
		}
	}

	return -1;
}
#endif

#if 1	/* for Multi-Delete */
static FW_VOID dcf_controller_sort_select_list_by_dir_index(FW_VOID)
{
	FW_UCHAR			list_index_forward;					/*	Index for SelectList forward					*/
	FW_UCHAR			list_index_backward;				/*	Index for SelectList backward					*/
	T_BF_DCF_IF_FILE_INDEX	swap_work;							/*	Work Area for Swap								*/

	for (list_index_forward = 0; list_index_forward < (FW_UCHAR)(g_bf_dcf_controller_select_list_index - 1); list_index_forward++) {
///#ifndef FJ_CUSTOM_DCF_DATE_DB_USE
		if ((g_bf_dcf_controller_select_list[list_index_forward].pack.dir_index == 0) || (g_bf_dcf_controller_select_list[list_index_forward].pack.obj_index == 0)) {
			/* Invalid ImageIndex */
			g_bf_dcf_controller_select_list_index--;
			g_bf_dcf_controller_select_list[list_index_forward].index = g_bf_dcf_controller_select_list[g_bf_dcf_controller_select_list_index].index;
			g_bf_dcf_controller_select_list[g_bf_dcf_controller_select_list_index].index = 0;
		}
///#endif // FJ_CUSTOM_DCF_DATE_DB_USE

		for (list_index_backward = (list_index_forward + 1); list_index_backward < g_bf_dcf_controller_select_list_index; list_index_backward++) {
			if (g_bf_dcf_controller_select_list[list_index_forward].pack.dir_index == g_bf_dcf_controller_select_list[list_index_backward].pack.dir_index) {
				/* Same Directory Index */
				list_index_forward++;
				if (list_index_forward >= g_bf_dcf_controller_select_list_index) {
					break;
				}

				if (list_index_forward == list_index_backward) {
					continue;
				}

				/* Swap SelectList data */
				swap_work.index = g_bf_dcf_controller_select_list[list_index_forward].index;
				g_bf_dcf_controller_select_list[list_index_forward].index = g_bf_dcf_controller_select_list[list_index_backward].index;
				g_bf_dcf_controller_select_list[list_index_backward].index = swap_work.index;
			}
		}
	}
}
#endif



/**
 * @brief set protect attribute
 * @param   I : const char* file_name
 *          I : UCHAR attr
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_set_protect_attribute(const FW_CHAR* file_name, FW_UCHAR attr)
{
	FW_UCHAR attr_tmp;
	FW_INT32 ercd;

	ercd = BF_Mdf_Wrapper_Get_Attribute(file_name, &attr_tmp);
	if(ercd != 0){
		return ercd;
	}

	// update protect attribute
	attr_tmp = (attr_tmp & D_BF_DCF_CONTROLLER_ATTR_PROTECT_FILTER) | attr;

	// set attribute
	ercd = BF_Mdf_Wrapper_Set_Attribute(file_name, attr_tmp);

	return ercd;
}

#if 0	// MDF custom
/**
 * @brief set protect attribute for multi set
 * @param   I : const char* file_name
 *          I : UCHAR attr
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_controller_set_protect_attribute_for_multi_set(const FW_CHAR* file_name, FW_UCHAR attr)
{
	FW_UCHAR attr_tmp;
	FW_INT32 ercd;

	ercd = BF_Mdf_Wrapper_Get_Attribute(file_name, &attr_tmp);
	if(ercd != 0){
		return ercd;
	}

	// update protect attribute
	attr_tmp = (attr_tmp & D_BF_DCF_CONTROLLER_ATTR_PROTECT_FILTER) | attr;

	// set attribute
	ercd = BF_Mdf_Wrapper_Set_Attribute_For_Multi_Set(file_name, attr_tmp);

	return ercd;
}
#endif

#ifdef CO_A_FILE_NAME_LIST	// CO_A_FILE_NAME_LIST
static FW_VOID dcf_controller_init_file_name_list( VOID )
{
	memset( (VOID*)&g_bg_dcf_controller_file_name_list, 0, sizeof(g_bg_dcf_controller_file_name_list) );
}

static BOOL dcf_controller_search_file_name_list( FW_ULONG idx, FW_UCHAR file_type, FW_CHAR* file_str )
{
	BOOL ercd = FALSE;
	ULONG sr;
	
	sr = Dd_ARM_DI();

	for (int i = 0 ; i < g_bg_dcf_controller_file_name_list.index_size; i++) {
 		if ( (g_bg_dcf_controller_file_name_list.info[i].idx == idx) &&
			(g_bg_dcf_controller_file_name_list.info[i].file_type != file_type) &&
			(g_bg_dcf_controller_file_name_list.info[i].ready == 1) ) {
			strcpy( file_str, g_bg_dcf_controller_file_name_list.info[i].file_str );
			ercd = TRUE;
			break;
		} 
	}

	Dd_ARM_EI(sr);
	
	return ercd;
}

static FW_VOID dcf_controller_update_file_name_list( FW_ULONG idx, FW_UCHAR file_type, FW_CHAR* file_str )
{
	UINT32 write	= 0;	
	ULONG sr;
	
	sr = Dd_ARM_DI();

	write	= g_bg_dcf_controller_file_name_list.index_size;

	g_bg_dcf_controller_file_name_list.info[ write ].idx		= idx;
	g_bg_dcf_controller_file_name_list.info[ write ].file_type	= file_type;
	g_bg_dcf_controller_file_name_list.info[ write ].ready		= 1;
	strcpy( g_bg_dcf_controller_file_name_list.info[ write ].file_str, file_str);

	if (write + 1 < D_BF_DCF_CONTROLLER_FILE_LIST_INDEX_MAX) {
		g_bg_dcf_controller_file_name_list.index_size = write + 1;
	}
	else {
		g_bg_dcf_controller_file_name_list.index_size = 0;
	}

	Dd_ARM_EI(sr);

	return;
}
#endif	// A CO_A_FILE_NAME_LIST  END

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

FW_VOID BF_Dcf_Controller_Close_Display_File(FW_VOID)
{
	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Mdf_Wrapper_Close(g_bf_dcf_controller_display_file_id);
		g_bf_dcf_controller_display_file_id = 0;
		g_bf_dcf_controller_display_file_index = 0;
	}
}

#if 1	// for DCF task
FW_INT32 BF_Dcf_Controller_End_Task_Mdf(FW_VOID)
{
	BF_Mdf_Wrapper_End_Task();
	return 0;
}

FW_INT32 BF_Dcf_Controller_Set_Cur_Drive(FW_VOID)
{
	FW_INT32 ercd;
	char drive_name[64];
	ercd = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
	if(ercd != 0){
		return ercd;
	}
	strcat(drive_name, ":");

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("BF_Dcf_Controller_Set_Cur_Drive  drive_name=%s \n", drive_name ));
#endif

//	return BF_Mdf_Wrapper_Chdir("I:");
	return BF_Mdf_Wrapper_Chdir(drive_name);


}
#endif

/**
 * @brief DCF-DB is create.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_DataBase_Create( FW_VOID )
{
	FW_INT32 ercd;
	FW_USHORT dirno = 0;
	FW_USHORT fileno = 0;
	FW_USHORT tmp_dirno = 0;
	FW_USHORT tmp_fileno = 0;

	if(g_bf_dcf_controller_file_name_setting == E_BF_DCF_IF_FILE_NAME_SERIES){
		dcf_controller_get_last_file(&tmp_dirno, &tmp_fileno, E_BF_DCF_IF_LATEST_TYPE_ALL);
#ifdef CO_BF_DCF_CONTROLLER_SERIES_DIR_RESET
		tmp_dirno = 100;
#endif
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF START MEDIA END1\n");
#endif

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	// start DCF DB init
	ercd = dcf_controller_dcfdb_init();
	if( ercd != 0 ) {
		return ercd;
	}

	if(g_bf_dcf_controller_file_name_setting == E_BF_DCF_IF_FILE_NAME_SERIES){
		// get last dirno and fileno
		dcf_controller_get_last_file(&dirno, &fileno, E_BF_DCF_IF_LATEST_TYPE_ALL);

		if(tmp_dirno > dirno){
			// set last dirno and fileno
			dcf_controller_set_last_file(tmp_dirno, tmp_fileno);
		}
		else if(tmp_dirno == dirno){
			if(tmp_fileno > fileno){
				// set last dirno and fileno
				dcf_controller_set_last_file(tmp_dirno, tmp_fileno);
			}
		}
		else{ //(tmp_dirno < last_dirno)
			; // DO NOTHING
		}
	}
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF START MEDIA END2\n");
#endif

	return ercd;
}


/**
 * @brief 		Create the directory. 
 * @param		I : USHORT dirNum
 * @param		I : UCHAR dirNameIndex (0, 1, ...)
 * @return 		Success/Fail
 * @note		None
 * @attention 	current directory will be changed.
 */
FW_INT32 BF_Dcf_Controller_Create_Dir(FW_USHORT dirNum, FW_UCHAR dirNameIndex)
{
	return BF_Dcf_Db_Create_Dir(dirNum, dirNameIndex);
}


/**
 * @brief Get directory index on DCF-DB
 * @param  I : FW_LONG* index
 *         O : FW_LONG* index_num
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Dir_Idx(FW_LONG* index, FW_LONG* index_num)
{
	FW_SHORT dir_index;

	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Dir_Index_Num(dcf_controller_get_dir_index(*index), &dir_index);
	if(FSDCF_ERRNO_OK != g_bf_dcf_controller_ret_err) {
		return g_bf_dcf_controller_ret_err;
	}

	*index_num = ((FW_LONG)dir_index) << 16;
	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief get information from DCF DB.
 * @param io:*dcfdb_info
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Controller_Get_Obj_Info(FW_LONG* file_index, T_BF_DCF_IF_DB_INFO* dcfdb_info)
{
	return BF_Dcf_Db_Get_Info(dcf_controller_get_dir_index(*file_index), dcf_controller_get_obj_index(*file_index), dcfdb_info);
}


/**
 * @brief Get max file number as directory number
 * @param  O : FW_SHORT* file_no
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Max_File_Num_As_Dir_Num(FW_SHORT* file_no)
{
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Max_File_Num_As_Dir_Num(file_no);
	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief		Check file type
 * @param		I: const FW_CHAR* ext_name
 * @param		I: FW_SHORT file_type
 * @return 		True : Yes, ext_name found in file_type.
 *				False : No, ext_name does NOT belong to file_type.
 * @note 		None
 * @attention 	None
 */
BOOL BF_Dcf_Controller_Is_File_Type(const FW_CHAR* ext_name, FW_SHORT file_type)
{
	INT32 ercd;
	FW_UCHAR ext_type;

	ercd = BF_Dcf_Db_Get_Type_By_Ext(ext_name, &ext_type);
	if( ercd != FSDCF_ERRNO_OK ){
		return FALSE;
	}

	if(0 == (file_type & ((FW_SHORT)ext_type))) {
		return FALSE;
	}
	return TRUE;
}

#if 0
/**
 * @brief read number of allowed extension by file index
 * @param i:FW_LONG* index
 * @param o:FW_CHAR* allowed_ext_cnt
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Controller_Read_Allowed_Ext_Cnt(FW_LONG* index, FW_UCHAR* allowed_ext_cnt)
{
	if(index == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Read_Allowed_Ext_Count(dcf_controller_get_dir_index(*index), dcf_controller_get_obj_index(*index), allowed_ext_cnt);
	return g_bf_dcf_controller_ret_err;
}
#endif

/**
 * @brief Read date and time of directory from DCF-DB
 * @param  I : FW_LONG* index
 *         O : FW_USHORT* dir_created_date
 *         O : FW_USHORT* dir_created_time
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Read_Dir_Date_Time(FW_LONG* index, FW_USHORT* dir_created_date, FW_USHORT* dir_created_time)
{
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Read_Dir_Date_Time(dcf_controller_get_dir_index(*index), dir_created_date, dir_created_time);
	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief Read directory duplicate status from DCF-DB
 * @param  I : FW_LONG* index
 *         O : FW_CHAR* dir_dupstatus
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Read_Dir_Duplicate_Status(FW_LONG* index, FW_CHAR* dir_dupstatus)
{
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Read_Dir_Duplicatestatus(dcf_controller_get_dir_index(*index), dir_dupstatus);
	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief Read directory name from DCF-DB
 * @param  I : FW_LONG* index
 *         O : FW_CHAR* dir_name
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Read_Dir_Name(FW_LONG* index, FW_CHAR* dir_name)
{
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Read_Dir_Name(dcf_controller_get_dir_index(*index), dir_name);
	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief Read total number of directory from DCF-DB
 * @param  O : FW_SHORT* dir_cnt
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Read_Dir_Total_Cnt(FW_SHORT* dir_cnt)
{
	if(dir_cnt == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Read_Dir_Count(dir_cnt);
	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief Read count of object in current directory
 * @param  I : FW_LONG* index
 *         O : FW_SHORT* obj_count
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Read_Obj_Cnt(FW_LONG* index, FW_SHORT* obj_count)
{
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Read_Obj_Count(dcf_controller_get_dir_index(*index), obj_count);
	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief Read object information in current directory
 * @param  I : FW_LONG* index
 *         O : T_BF_DCF_IF_DCFDB_OBJ_INFO* obj_info
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Read_Obj_Inf(FW_LONG* index, T_BF_DCF_IF_DCFDB_OBJ_INFO* obj_info)
{
	T_BF_DCF_DB_OBJ_INFO* objInfo;

	if(index == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Read_Obj_Info(dcf_controller_get_dir_index(*index), dcf_controller_get_obj_index(*index), &objInfo);
	if(FSDCF_ERRNO_OK != g_bf_dcf_controller_ret_err) {
		return g_bf_dcf_controller_ret_err;
	}

	obj_info->extType = objInfo->extType;
	obj_info->attr = objInfo->attr;
	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief Read object name in current directory
 * @param  I : FW_LONG* index
 *         O : FW_CHAR* obj_name
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Read_Obj_Name(FW_LONG* index, FW_CHAR* obj_name)
{
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Read_Obj_Name(dcf_controller_get_dir_index(*index), dcf_controller_get_obj_index(*index), obj_name);
	return g_bf_dcf_controller_ret_err;
}

// NEW API ==================================================================
/**
 * @brief Reset DCF DB buffer
 * @param  
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Controller_Reset_Db(FW_VOID)
{
	BF_Dcf_Db_Clear();

	return FSDCF_ERRNO_OK;
}

/**
 * @brief set allowed extenstion list
 * @param i:const FW_CHAR* allow_ext_list
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_Controller_Set_Allow_Ext_List(const FW_CHAR* allow_ext_list)
{
	if(allow_ext_list == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Set_Allowed_Ext_List(allow_ext_list);
	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief Set current directory to DCF-DB
 * @param  I : T_BF_DCF_IF_SET_CUR_DIR* cur_dir
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Set_Current_Dir(T_BF_DCF_IF_SET_CUR_DIR* cur_dir)
{
	if(cur_dir == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	if(cur_dir->act_mode == D_BF_DCF_IF_PRM_NORMAL_ACTION){
		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Set_Current_Dir(dcf_controller_get_dir_index(cur_dir->index));
	}
	else{
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_PARAM_FAILED;
	}

	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief Write attribute of object to DCF-DB
 * @param  I : T_BF_DCF_IF_WRT_OBJ_ATTR* obj_attr
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Write_Obj_Attr(T_BF_DCF_IF_WRT_OBJ_ATTR* obj_attr)
{
	if(obj_attr == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Write_Obj_Attr(
						dcf_controller_get_dir_index(obj_attr->index),
						dcf_controller_get_obj_index(obj_attr->index),
						obj_attr->attr);
	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief finalize depend on DCF.
 * @param i:media_type media_type
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_End_Media(FW_INT32* media_id)
{
	FW_INT32 ercd;
	FW_USHORT dirno = 0, fileno = 0;

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	BF_Dcf_Controller_Close_Display_File();

	if(g_bf_dcf_controller_file_name_setting == E_BF_DCF_IF_FILE_NAME_SERIES){
		dcf_controller_get_last_file(&dirno, &fileno, E_BF_DCF_IF_LATEST_TYPE_ALL);
	}

	/* start filesystem init */
	ercd = BF_Fs_Controller_End_Media(*media_id);
	if( ercd != 0 ) {
		return ercd;
	}

	/* start DCS DB init */
	BF_Dcf_Db_Clear();

	if(g_bf_dcf_controller_file_name_setting == E_BF_DCF_IF_FILE_NAME_SERIES){
		/* set last dirno and fileno */
		dcf_controller_set_last_file(dirno, fileno);
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF EJECT END\n");
#endif

	g_bf_dcf_controller_init_flg[*media_id] = 0;

	return ercd;
}



#if 0
/**
 * @brief get duplicate directory number and file number.
 * @param o:*dirno duplicate directory number
 * @param o:*fileno duplicate file number
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Duplicate_No(FW_USHORT* dirno, FW_USHORT* fileno)
{
	BF_DcfDb_Get_DuplicateNo(dirno, fileno);
	return 0;
}
#endif


/**
 * @brief get DCF file entry.
 * @param i:*media_type media type
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Entry(E_BF_MEDIA_STATE_MEDIA_TYPE* media_type, FW_LONG* entry_cnt)
{
	T_BF_DCF_IF_FILE_INDEX file_index;
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
	FW_USHORT dir_index;
	FW_USHORT obj_index;
#endif

	if(media_type == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("Err: media_type NULL in BF_Dcf_Controller_Get_Entry() \n" ));
#endif
		return g_bf_dcf_controller_ret_err;
	}
	*entry_cnt = 0;
	if(*media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_CUR){
		;
	}
	else if(*media_type != BF_Media_State_Get_Used_Media()){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("BF_Dcf_Controller_Get_Entry(): NOT USED MEDIA: used_media=%d, target_media=%d\n", BF_Media_State_Get_Used_Media(), *media_type));
#endif
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_INSERT_MEDIA;
		return g_bf_dcf_controller_ret_err;
	}
	else{
		; // DO NOTHING
	}

	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Dir_Index_Num(DCF_DB_CURRENT_DIR, &(file_index.pack.dir_index));
	if(0 != g_bf_dcf_controller_ret_err) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("Err: BF_Dcf_Db_Get_Dir_Index_Num() ret=%d in BF_Dcf_Controller_Get_Entry() \n", g_bf_dcf_controller_ret_err ));
#endif
		return g_bf_dcf_controller_ret_err;
	}
	file_index.pack.obj_index = dcf_controller_get_total_file_cnt();
	*entry_cnt = file_index.index; /* pgr0872 */

	g_bf_dcf_controller_ret_err = 0;
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
	dir_index = file_index.index >> 16;
	obj_index = (file_index.index << 16) >> 16;
	BF_Fs_User_Custom_Info_Printer(("Last File Index in current directory: DirIndex=%d, ObjIndex=%d\n", dir_index, obj_index));
#endif
	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief Get target file information.
 * @param i:*file_index file index
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_File_Info(T_BF_DCF_IF_FILE_INDEX* file_index, T_BF_DCF_IF_FILE_INFO* file_info)
{
	if(file_index == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(*file_index, NULL, file_info);

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF GET FILE INFO END\n");
#endif

	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief Get file path from full file type.
 * @param i:*full_file_path full file path
 * @param o:*file_path file file path
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_File_Path(FW_CHAR* full_file_path, FW_CHAR* file_path)
{
	FW_INT32 name_cnt;
	FW_UINT32 drive_name_len=0;
	FW_CHAR  chk_path[64];
	char *drive_name;

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("BF_Dcf_Controller_Get_File_Path (START): input full_path=%s \n", full_file_path));
#endif
	// DriveName Check
	strcpy(chk_path, full_file_path );
	drive_name = strtok(chk_path, ":");
	if (drive_name == NULL) {
		return FSDCF_ERRNO_PARAM_FAILED;
	}
	drive_name_len = strlen(drive_name);
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("BF_Dcf_Controller_Get_File_Path DriveName=%s, DriveNameLen=%d \n", drive_name, drive_name_len));
#endif

	if ( drive_name_len == 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("Drive name length Err: drive name=%s, length=%d \n", drive_name, drive_name_len));
#endif
		return FSDCF_ERRNO_DRIVE_INIT_FAILED;
	}

	// NULL check
	for (name_cnt = 0; name_cnt < (drive_name_len + D_BF_DCF_IF_FILE_PATH_WO_DRVNAME); name_cnt++) {
		if(full_file_path[name_cnt] == 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("FILE PATH ERROR: path=%s, name_cnt=%d \n", full_file_path, name_cnt));
#endif
			return FSDCF_ERRNO_NOT_SUPPORT_FILENO;
		}
	}
	memcpy(file_path, (full_file_path + (drive_name_len + 16)), 12); // [DriveName]:\DCIM\100AAAAA\  BBBB0001.JPG //
	file_path[12] = 0;

	return 0;
}


/**
 * @brief Get target file information.
 * @param i:*file_index file index
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_File_Type(T_BF_DCF_IF_FILE_INDEX* file_index, FW_SHORT* file_type)
{
	FW_UCHAR ext_type;

	if(file_index == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Type(*file_index, &ext_type);
	if( g_bf_dcf_controller_ret_err != 0 ) {
		return g_bf_dcf_controller_ret_err;
	}

	*file_type = (FW_LONG)ext_type;

	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief get index DCF file path.
 * @param i:*index_filepath_data index file path parameter
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Index_File_Path(T_BF_DCF_IF_INDEX_FILE_DATA* index_filepath_data, FW_CHAR* file_path)
{
	if(index_filepath_data == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = BF_Dcf_Controller_Make_Index_File_Path_Ext_Abs(index_filepath_data->file_index, index_filepath_data->ext, file_path);
	return g_bf_dcf_controller_ret_err;
}




/**
 * @brief make file path of specified index.
 * @param i:file_index file index
 * @param i:*ext file extension
 * @param o:*full_file_path full file path
 * @return error code
 */
FW_INT32 BF_Dcf_Controller_Make_Index_File_Path_Ext_Abs(T_BF_DCF_IF_FILE_INDEX file_index, FW_CHAR* ext, FW_CHAR* full_file_path)
{
	FW_INT32 ercd;
	FW_UINT32 drive_name_len=0;
	char drive_name[64];
	T_BF_DCF_DB_OBJ_INFO* db_file_info;

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("BF_Dcf_Controller_Make_Index_File_Path_Ext_Abs (START): input index=0x%08lx \n", file_index.index ));
#endif
	/* get file node */
	ercd = BF_Dcf_Db_Search_Obj_Info(file_index.pack.dir_index, file_index.pack.obj_index, &db_file_info);
	if( ercd != 0 ) {
		return ercd;
	}

	/* get full file path */
	ercd = BF_Dcf_Db_Make_Full_Path(file_index, full_file_path);
	if( ercd != 0 ) {
		return ercd;
	}

	// Check Drive name
	ercd = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
	if (ercd != FSDCF_ERRNO_OK ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("Get err Drive name :ercd=%d  in BF_Media_State_Exchage_Type_To_Drive_Name() \n",ercd));
#endif
		return ercd;
	}
	drive_name_len = strlen( drive_name );
	if ( drive_name_len == 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("Drive name length Err: drive name=%s, length=%d \n", drive_name, drive_name_len));
#endif
		return FSDCF_ERRNO_DRIVE_INIT_FAILED;
	}

	/* change ext */
	if( *ext != ((FW_INT32)NULL) ){
		strcpy(full_file_path + (drive_name_len + 25), ext);	//Length   // [DriveName]:\DCIM\100AAAAA\BBBB0001.  //
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("BF_Dcf_Controller_Make_Index_File_Path_Ext_Abs (END): Make path=%s\n", full_file_path));
#endif

	return 0;
}

/**
 * @brief set max directory number and file number.
 * @param o:*file_name_param file name parameter
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Set_File_Name(T_BF_DCF_IF_FILE_NAME* file_name_param)
{
	dcf_controller_set_last_file(file_name_param->dirno, file_name_param->fileno);
	return 0;
}

/**
 * @brief set max directory name and file name.
 * @param o:*file_name_param file name parameter
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Set_Free_File_Name(T_BF_DCF_IF_FREE_FILE_NAME* free_file_name_param)
{
	g_bf_dcf_controller_ret_err = 0;

	if(free_file_name_param->check_dir == 0){
		// set file name
		//DcfDb_Set_File_Name( free_file_name_param->free_char );
		strncpy( (FW_CHAR*)g_bf_dcf_controller_file_name, free_file_name_param->free_char, sizeof(g_bf_dcf_controller_file_name) - 1 );
		g_bf_dcf_controller_file_name[sizeof(g_bf_dcf_controller_file_name) - 1] = '\0';

		// The priority file name that should register in DB is set.
		BF_Dcf_Db_Set_Prio_File_Name((FW_CHAR*)g_bf_dcf_controller_file_name);
	}
	else if(free_file_name_param->check_dir == 1){
		// set directory name
		//DcfDb_Set_Dir_Name( free_file_name_param->free_char, free_file_name_param->operation );
		strncpy( (FW_CHAR*)g_bf_dcf_controller_dir_name, free_file_name_param->free_char, sizeof(g_bf_dcf_controller_dir_name) - 1 );
		g_bf_dcf_controller_dir_name[sizeof(g_bf_dcf_controller_dir_name) - 1] = '\0';

		// The priority dir name that should register in DB is set.
		BF_Dcf_Db_Set_Prio_Dir_Name((FW_CHAR*)g_bf_dcf_controller_dir_name);

	}
	else{
		; // DO NOTHING
	}

	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief re-initialize depend on DCF.
 * @param i:media_type media_type
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Start_Media(FW_INT32* media_id)
{
	FW_INT32 ercd;

	if(g_bf_dcf_controller_init_flg[*media_id] == 1){
		return 0;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	BF_Dcf_Controller_Close_Display_File();

	/* start filesystem init */
	ercd = BF_Fs_Controller_Start_Media(*media_id);
	if( ercd != 0 ) {
		return ercd;
	}

#if 0
	if(BF_Boot_Loader_Check(D_BF_BOOT_LOADER_ALL_CODE) == 1) {
		/* initialize dpof table */
		ercd = BF_Dcf_Dpof_Init();
		if( ercd != 0 )	{
			return ercd;
		}

		/* read AUTOPRINT.MRK */
		ercd = BF_Dcf_Dpof_Read();
		if( ercd != 0 ) {
			return ercd;
		}
	}
#endif

//#ifdef CO_BF_DCF_CONTROLLER_MOUNT_DCF_DB
	ercd = BF_Dcf_Controller_DataBase_Create();
//#endif

	g_bf_dcf_controller_init_flg[*media_id] = 1;

	return ercd;
}




/**
 * @brief get specified oparation directory number and file number.
 * @param o:*dirno latest directory number
 * @param o:*fileno latest file number
 * @return none
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Dcf_Key(FW_ULONG operation, FW_USHORT* dirnum, FW_USHORT* filenum)
{
	FW_INT32 ercd = 0;

	switch( operation ){
		case FJ_PARA_DCF_LAST_KEY:
			ercd = BF_Dcf_Db_Get_Last_File(dirnum, filenum, E_BF_DCF_IF_LATEST_TYPE_ALL);

			if( g_bf_dcf_controller_last_dir_no > *dirnum ){
				*dirnum = g_bf_dcf_controller_last_dir_no;
				*filenum = g_bf_dcf_controller_last_file_no;
			}
			else if( g_bf_dcf_controller_last_dir_no == *dirnum ){
				*dirnum = g_bf_dcf_controller_last_dir_no;

				if( g_bf_dcf_controller_last_file_no > *filenum ){
					*filenum = g_bf_dcf_controller_last_file_no;
				}
			}
			else {
				; // DO NOTHING
			}
			break;

		case FJ_PARA_DCF_DUPLICATE:
			ercd = BF_Dcf_Db_Get_Duplicate_No(dirnum, filenum, E_BF_DCF_IF_LATEST_TYPE_ALL);
			break;

		case FJ_PARA_DCF_HIDDEN:
			ercd = BF_Dcf_Db_Get_Hidden_No(dirnum, E_BF_DCF_IF_LATEST_TYPE_ALL);
			*filenum = 0;
			break;

		case FJ_PARA_DCF_ASSIGN_LAST_KEY:
			ercd = BF_Dcf_Db_Get_Last_File(dirnum, filenum, E_BF_DCF_IF_LATEST_TYPE_SELECT);
			break;

		case FJ_PARA_DCF_ASSIGN_DUPLICATE:
			ercd = BF_Dcf_Db_Get_Duplicate_No(dirnum, filenum, E_BF_DCF_IF_LATEST_TYPE_SELECT);
			break;

		case FJ_PARA_DCF_ASSIGN_HIDDEN:
			ercd = BF_Dcf_Db_Get_Hidden_No(dirnum, E_BF_DCF_IF_LATEST_TYPE_SELECT);
			*filenum = 0;
			break;

		default:
			*dirnum = 0;
			*filenum = 0;
	}
	return ercd;
}


#if 1	// This may NOT need. Becase other function CAN support this.
/**
 * @brief Set directoy name to DCF-DB
 * @param  I : FW_CHAR* dir_fullpass_name
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Set_Dir_Db(FW_CHAR* dir_fullpass_name)
{
	if(dir_fullpass_name == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Set_Dir(dir_fullpass_name);
	return g_bf_dcf_controller_ret_err;
}
#endif


/**
 * @brief Get the last file number.
 * @param i:index  file index
 * @param o:*file_num file number
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Last_File_Number(FW_LONG* index, FW_USHORT* file_num)
{
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Last_File_Number(dcf_controller_get_dir_index(*index), file_num);
	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief get exif information.
 * @param i:*open_file_param open file parameter
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Exif_Info(T_BF_DCF_IF_OPEN_FILE_DATA* open_file_param, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data)
{
	FW_INT32 file_id;
	FW_CHAR open_filepath[64];
	T_BF_FS_IF_STAT stat;

	if (open_file_param == NULL) {
		// Param Error
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	// get file information
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(open_file_param->file_index, open_filepath, &(img_detail->file_info));
	if (g_bf_dcf_controller_ret_err != 0) {
		// get file information error
		return g_bf_dcf_controller_ret_err;
	}

	if (open_file_param->file_index.index == g_bf_dcf_controller_display_file_index) {
		// target file is opened
		FW_ULONG result;

		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Lseek(g_bf_dcf_controller_display_file_id, 0, D_BF_MDF_WRAPPER_SEEK_SET, &result);
		if (g_bf_dcf_controller_ret_err != 0) {
			// lseek NG
			BF_Dcf_Controller_Close_Display_File();

			// if lseek error. re-open display file
			file_id = BF_Mdf_Wrapper_Open(open_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
			if (file_id == 0) {
				// file open error
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", file_id, open_filepath));
#endif
				g_bf_dcf_controller_display_file_index = 0;
				g_bf_dcf_controller_display_file_id = 0;

				g_bf_dcf_controller_ret_err = file_id;
				return g_bf_dcf_controller_ret_err;
			}
			else {
				// open successfull
				g_bf_dcf_controller_display_file_index = open_file_param->file_index.index;
				g_bf_dcf_controller_display_file_id = file_id;
			}
		}
		else {
			// lseek OK
			file_id = g_bf_dcf_controller_display_file_id;
		}
	}
	else {
		if (g_bf_dcf_controller_display_file_id != 0) {
			BF_Dcf_Controller_Close_Display_File();
		}

		file_id = BF_Mdf_Wrapper_Open(open_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
		if( file_id == 0 ){
			// file open error
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", file_id, open_filepath));
#endif
			g_bf_dcf_controller_display_file_index = 0;
			g_bf_dcf_controller_display_file_id = 0;

			g_bf_dcf_controller_ret_err = file_id;
			return g_bf_dcf_controller_ret_err;
		}

		g_bf_dcf_controller_display_file_index = open_file_param->file_index.index;
		g_bf_dcf_controller_display_file_id = file_id;
	}

	if (BF_Dcf_Controller_Is_File_Type("JPG", open_file_param->file_type)) {
		// Jpeg file
		g_bf_dcf_controller_ret_err = BF_Dcf_Exif_Analyze_Jpeg(file_id, load_image_data, img_detail);
	}
	else {
		// not Jpeg file
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("NOT SUPPORT FILE: file_type=%ld\n", open_file_param->file_type));
#endif
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_SUPPORT_FILE;
	}

	if(g_bf_dcf_controller_ret_err != 0){
		// error
		BF_Dcf_Controller_Close_Display_File();
		return g_bf_dcf_controller_ret_err;
	}
	else if(load_image_data->thumb_size == 0) {
		BF_Dcf_Controller_Close_Display_File();

		return FSDCF_ERRNO_NOT_THUMBNAIL;
	}
	else {
		// DO NOTHING
	}

	if (load_image_data->main_size == 0) {
		BF_Mdf_Wrapper_Fstat(file_id, &stat);
		load_image_data->main_size = stat.fileSize;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF GET EXIF INFO END\n");
#endif

	return g_bf_dcf_controller_ret_err;
}


#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @brief select invididual image number.
 * @param i:mp_no  Invididual Image number.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Select_Ext_No(FW_UCHAR mp_no)
{
	if (mp_no < E_BF_DCF_IF_MPF_EXT_MP_SELECT_MAX) {
		g_bf_dcf_controller_ext_mp_select_num = (E_BF_DCF_IF_MPF_EXT_MP_SELECT)mp_no;
	}
	return FSDCF_ERRNO_OK;
}

/**
 * @brief get exif information.
 * @param i:*open_file_param open file parameter
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Exif_Info_Ext(T_BF_DCF_IF_OPEN_FILE_DATA* open_file_param, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data)
{
	FW_INT32 file_id;
	FW_CHAR open_filepath[64];
	T_BF_FS_IF_STAT stat;

	if (open_file_param == NULL) {
		// Param Error
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

#ifdef CO_BF_DCF_IF_DATE_DB_ENABLE_TEMP
	if(dcf_controller_is_dcf_index(open_file_param->file_index) == TRUE){
		// DCF Index is specified.
		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(open_file_param->file_index, open_filepath, &(img_detail->file_info));
		if (g_bf_dcf_controller_ret_err != 0) {
			// get file information error
			return g_bf_dcf_controller_ret_err;
		}
	}
	else{
		// Date Index is specified.

		// Get file full path
		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Date_Get_Index_File_Path(open_file_param->file_index.pack.obj_index, open_filepath);
		if(g_bf_dcf_controller_ret_err != 0) {
			return g_bf_dcf_controller_ret_err;
		}

		// Get file info
		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Date_Get_Index_File_Info(open_file_param->file_index.pack.obj_index, &(img_detail->file_info));
		if(g_bf_dcf_controller_ret_err != 0) {
			return g_bf_dcf_controller_ret_err;
		}
	}
#else // CO_BF_DCF_IF_DATE_DB_ENABLE_TEMP
	// get file information
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(open_file_param->file_index, open_filepath, &(img_detail->file_info));
	if (g_bf_dcf_controller_ret_err != 0) {
		// get file information error
		return g_bf_dcf_controller_ret_err;
	}
#endif // CO_BF_DCF_IF_DATE_DB_ENABLE_TEMP

	if (open_file_param->file_index.index == g_bf_dcf_controller_display_file_index) {
		// target file is opened
		FW_ULONG result;

		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Lseek(g_bf_dcf_controller_display_file_id, 0, D_BF_MDF_WRAPPER_SEEK_SET, &result);
		if (g_bf_dcf_controller_ret_err != 0) {
			// lseek NG
			BF_Dcf_Controller_Close_Display_File();

			// if lseek error. re-open display file
			file_id = BF_Mdf_Wrapper_Open(open_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
			if (file_id == 0) {
				// file open error
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", file_id, open_filepath));
#endif
				g_bf_dcf_controller_display_file_index = 0;
				g_bf_dcf_controller_display_file_id = 0;

				g_bf_dcf_controller_ret_err = file_id;
				return g_bf_dcf_controller_ret_err;
			}
			else {
				// open successfull
				g_bf_dcf_controller_display_file_index = open_file_param->file_index.index;
				g_bf_dcf_controller_display_file_id = file_id;
			}
		}
		else {
			// lseek OK
			file_id = g_bf_dcf_controller_display_file_id;
		}
	}
	else {
		if (g_bf_dcf_controller_display_file_id != 0) {
			BF_Dcf_Controller_Close_Display_File();
		}

		file_id = BF_Mdf_Wrapper_Open(open_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
		if( file_id == 0 ){
			// file open error
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", file_id, open_filepath));
#endif
			g_bf_dcf_controller_display_file_index = 0;
			g_bf_dcf_controller_display_file_id = 0;

			g_bf_dcf_controller_ret_err = file_id;
			return g_bf_dcf_controller_ret_err;
		}

		g_bf_dcf_controller_display_file_index = open_file_param->file_index.index;
		g_bf_dcf_controller_display_file_id = file_id;
	}

	if (BF_Dcf_Controller_Is_File_Type("MPO", open_file_param->file_type)) {
		// Jpeg file
		g_bf_dcf_controller_ret_err = BF_Dcf_Exif_Analyze_Ext(file_id, load_image_data, img_detail);
	}
	else {
		// not Jpeg file
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("NOT SUPPORT FILE: file_type=%ld\n", open_file_param->file_type));
#endif
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_SUPPORT_FILE;
	}

	if(g_bf_dcf_controller_ret_err != 0){
		// error
		BF_Dcf_Controller_Close_Display_File();
		return g_bf_dcf_controller_ret_err;
	}
	else if(load_image_data->thumb_size == 0) {
		BF_Dcf_Controller_Close_Display_File();

		return FSDCF_ERRNO_NOT_THUMBNAIL;
	}
	else {
		// DO NOTHING
	}

#if 1
	// Get selected individual image info
	if (g_bf_dcf_controller_ext_mp_select_num < load_image_data->ext_mp_num) {
		if (load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].main_size != 0) {
			// Get main image info
			load_image_data->main_size = load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].main_size;
			load_image_data->main_offset = load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].main_offset;
			img_detail->main_width = img_detail->ext_mp[g_bf_dcf_controller_ext_mp_select_num].main_width;
			img_detail->main_lines = img_detail->ext_mp[g_bf_dcf_controller_ext_mp_select_num].main_lines;
			img_detail->ycc_format = img_detail->ext_mp[g_bf_dcf_controller_ext_mp_select_num].ycc_format;

			// Get thumbnail image info
			if (load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].thumb_size != 0) {
				load_image_data->thumb_size = load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].thumb_size;
				load_image_data->thumb_offset = load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].thumb_offset;
				img_detail->ycc_format_thum = img_detail->ext_mp[g_bf_dcf_controller_ext_mp_select_num].ycc_format_thum;
			}
			// Get Class1 image info
			if (load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class1_size != 0) {
				load_image_data->class1_size = load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class1_size;
				load_image_data->class1_offset = load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class1_offset;
				img_detail->class1_width = img_detail->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class1_width;
				img_detail->class1_lines = img_detail->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class1_lines;
				img_detail->class1_ycc_format = img_detail->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class1_ycc_format;
				
				if (load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class1_thumb_size != 0) {
					load_image_data->class1_thumb_size = load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class1_thumb_size;
					load_image_data->class1_thumb_offset = load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class1_thumb_offset;
				}
			}
			// Get Class2 image info
			if (load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class2_size != 0) {
				load_image_data->class2_size = load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class2_size;
				load_image_data->class2_offset = load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class2_offset;
				img_detail->class2_width = img_detail->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class2_width;
				img_detail->class2_lines = img_detail->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class2_lines;
				img_detail->class2_ycc_format = img_detail->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class2_ycc_format;
				
				if (load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class2_thumb_size != 0) {
					load_image_data->class2_thumb_size = load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class2_thumb_size;
					load_image_data->class2_thumb_offset = load_image_data->ext_mp[g_bf_dcf_controller_ext_mp_select_num].class2_thumb_offset;
				}
			}
		}
	}
#endif

	if (load_image_data->main_size == 0) {
		BF_Mdf_Wrapper_Fstat(file_id, &stat);
		load_image_data->main_size = stat.fileSize;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF GET EXIF INFO END\n");
#endif

	return g_bf_dcf_controller_ret_err;
}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE


/**
 * @brief get exif information.
 * @param i:*open_path open file path
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Exif_Info_Name(FW_CHAR* open_path, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data)
{
	FW_INT32 file_id;

	file_id = BF_Mdf_Wrapper_Open(open_path, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
	if( file_id == 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", file_id, open_path));
#endif
		g_bf_dcf_controller_ret_err = file_id;
		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = BF_Dcf_Exif_Analyze_Jpeg(file_id, load_image_data, img_detail);

	if(g_bf_dcf_controller_ret_err != 0){
		BF_Mdf_Wrapper_Close(file_id);
		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Close(file_id);
	if(g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Close() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, file_id));
#endif
	}

	return g_bf_dcf_controller_ret_err;

}

/**
 * @brief		Get first free File number in current directory.
 * @param		O : FW_USHORT* fileNum
 * @return error code
 * @note 		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_Controller_Get_First_Free_File_No(FW_USHORT* fileNum)
{
	return BF_Dcf_Db_Get_First_Free_File_No(fileNum);
}

/**
 * @brief		Get the directory name when the directory is made.
 * @param		O : FW_CHAR* dirname
 * @return error code
 * @note 		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_Controller_Get_Make_Dir_Name(FW_CHAR* dirname)
{
	if(dirname == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	strcpy(dirname, (FW_CHAR*)g_bf_dcf_controller_dir_name);

	return FSDCF_ERRNO_OK;
}

/**
 * @brief		Get the file name when the file is made.
 * @param		O : FW_CHAR* dirname
 * @return error code
 * @note 		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_Controller_Get_Make_File_Name(FW_CHAR* filename)
{
	if(filename == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	strcpy(filename, (FW_CHAR*)g_bf_dcf_controller_file_name);

	return FSDCF_ERRNO_OK;
}


/**
 * @brief		Get new directory number to create copy directory
 * @param		O : FW_CHAR* dir_name
 * @return error code
 * @note 		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_Controller_Get_New_Copy_Dir_No(FW_USHORT* dirNum)
{
#if 0
	FW_INT32 ercd;
	FW_INT32 dNo;
	T_BF_FS_IF_Dir_Info dir;
	FW_SHORT tmp_dirno;

	*dirNum = 99;

	BF_Fs_Controller_End_Media(E_BF_MEDIA_STATE_MEDIA_TYPE_SD);
	BF_Fs_Controller_End_Media(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1);
	BF_Fs_Controller_End_Media(E_BF_MEDIA_STATE_MEDIA_TYPE_NF);
	ercd = BF_Fs_Controller_Start_Media(E_BF_MEDIA_STATE_MEDIA_TYPE_SD);
	if( ercd != 0 ) {
		return ercd;
	}

	ercd = BF_Fs_Controller_Chdir("I:");
	if( ercd != 0 ) {
		return ercd;
	}

	ercd = BF_Mdf_Wrapper_Opendir("I:\\DCIM", &dNo);
	if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=I:\\DCIM\n", ercd));
#endif
		BF_Mdf_Wrapper_Closedir(dNo);
		BF_Fs_Controller_Start_Media(E_BF_MEDIA_STATE_MEDIA_TYPE_NF);
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
			BF_Fs_Controller_Start_Media(E_BF_MEDIA_STATE_MEDIA_TYPE_NF);
			return ercd;
		}

		tmp_dirno = BF_Dcf_Db_Get_Dir_No(dir.fname);
		if((D_BF_DCF_CONTROLLER_DIR_START_NUM > tmp_dirno) || (D_BF_DCF_CONTROLLER_DIR_END_NUM < tmp_dirno)) {
			continue;
		}

		if(*dirNum < tmp_dirno) {
			*dirNum = tmp_dirno;
		}
	}

	/*-----------------------------------------------------------------------*/
	ercd = BF_Mdf_Wrapper_Closedir(dNo);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
	}

	if(999 <= *dirNum) {
		BF_Fs_Controller_Start_Media(E_BF_MEDIA_STATE_MEDIA_TYPE_NF);
		return FSDCF_ERRNO_DIR_FULL;
	}
	*dirNum = *dirNum + 1;

	BF_Fs_Controller_End_Media(E_BF_MEDIA_STATE_MEDIA_TYPE_SD);
	BF_Fs_Controller_End_Media(E_BF_MEDIA_STATE_MEDIA_TYPE_NF);
	ercd = BF_Fs_Controller_Start_Media(E_BF_MEDIA_STATE_MEDIA_TYPE_NF);
	if( ercd != 0 ) {
		return ercd;
	}

	ercd = BF_Fs_Controller_Chdir("I:");
	if( ercd != 0 ) {
		return ercd;
	}

#endif
	return FSDCF_ERRNO_OK;
}

/**
 * @brief Get file type by file type name
 * @param  I : const FW_CHAR* ext_name
 *         O : FW_UCHAR* ext_type
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Typ_By_Ext_Name(const FW_CHAR* ext_name, FW_UCHAR* ext_type)
{
	if(ext_name == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Type_By_Ext(ext_name, ext_type);
	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief division loading start.
 * @param i:*image_load_data image load data
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Start_Div_Load(T_BF_DCF_IF_IMAGE_LOAD_DATA* image_load_data, FW_ULONG* length)
{
	FW_INT32 file_id = 0;
	FW_ULONG read_size = 0;
	FW_ULONG read_all_size = 0;
	FW_ULONG ptr = 0;
	FW_ULONG dst_addr;
#if 1
	FW_ULONG read_req_size;
#endif

	if(image_load_data == NULL){
		g_bf_dcf_controller_div_load_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_div_load_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	g_bf_dcf_controller_div_load_ret_err = dcf_controller_open_index_file(image_load_data->file_index, &file_id);
	if(g_bf_dcf_controller_div_load_ret_err != 0){
		return g_bf_dcf_controller_div_load_ret_err;
	}

	if(image_load_data->file_offset != 0){
		g_bf_dcf_controller_div_load_ret_err = BF_Mdf_Wrapper_Lseek(file_id, (FW_INT32)image_load_data->file_offset, D_BF_MDF_WRAPPER_SEEK_SET, &ptr);
		if(g_bf_dcf_controller_div_load_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_div_load_ret_err, file_id));
#endif
			BF_Dcf_Controller_Close_Display_File();

			return g_bf_dcf_controller_div_load_ret_err;
		}
	}

	dst_addr = image_load_data->img_addr;

	for (;;){
		BF_Fs_User_Custom_Tslp_Tsk(1);

		read_size = 0;
#if 1
		if ((image_load_data->img_size - read_all_size) > D_BF_FS_USER_CUSTOM_DIVISION_READ_SIZE) {
			read_req_size = D_BF_FS_USER_CUSTOM_DIVISION_READ_SIZE;
		}
		else {
			read_req_size = (image_load_data->img_size - read_all_size);
		}
		g_bf_dcf_controller_div_load_ret_err = dcf_controller_read_with_measure_speed(file_id, dst_addr, read_req_size, &read_size);
#else
		g_bf_dcf_controller_div_load_ret_err = dcf_controller_read_with_measure_speed(file_id, dst_addr, D_BF_FS_USER_CUSTOM_DIVISION_READ_SIZE, &read_size);
#endif
		if(g_bf_dcf_controller_div_load_ret_err != 0) {
			if (g_bf_dcf_controller_div_load_ret_err != D_BF_MDF_WRAPPER_ERR_EOF) {
				BF_Dcf_Controller_Close_Display_File();

				return g_bf_dcf_controller_div_load_ret_err;
			}
		}

		read_all_size += read_size;
#if 1
		if((read_size < read_req_size) || (g_bf_dcf_controller_div_load_ret_err == D_BF_MDF_WRAPPER_ERR_EOF) || (read_all_size >= image_load_data->img_size)) {
#else
		if((read_size < D_BF_FS_USER_CUSTOM_DIVISION_READ_SIZE) || (g_bf_dcf_controller_div_load_ret_err == D_BF_MDF_WRAPPER_ERR_EOF)) {
#endif
			g_bf_dcf_controller_dcfc_ret_length = *length = read_all_size;
			g_bf_dcf_controller_div_load_ret_err = 0;
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
			BF_Fs_User_Custom_Info_Printer(("BF_Mdf_Wrapper_Read() all end: total_req_size=%d, total_ret_size=%d\n", image_load_data->img_size, read_all_size));
#endif
			break;
		}

		if( BF_Dcf_If_Receiver_Is_Async_Stop() == E_BF_DCF_IF_RECIVER_ASYNC_STOP ){
			g_bf_dcf_controller_dcfc_ret_length = *length = 0;
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
			BF_Fs_User_Custom_Info_Printer(("RDIVISION READ STOP\n"));
#endif
			break;
		}

		dst_addr += read_size;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF DIVISION LOAD END\n");
#endif

	return g_bf_dcf_controller_div_load_ret_err;
}


/**
 * @brief division load result.
 * @param o:*length image data size
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Result_Div_Load(FW_ULONG* length)
{
	if(length == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	*length = g_bf_dcf_controller_dcfc_ret_length;
	return g_bf_dcf_controller_div_load_ret_err;
}

FW_INT32 BF_Dcf_Controller_Fstat(T_BF_DCF_IF_FILE_INDEX file_index, T_BF_FS_IF_STAT* fsif_stat)
{
	FW_INT32 ercd;
	FW_CHAR open_filepath[64];

	if (g_bf_dcf_controller_display_file_index == file_index.index) {
		ercd = BF_Mdf_Wrapper_Fstat(g_bf_dcf_controller_display_file_id, fsif_stat);

		if (ercd != 0) {
			BF_Dcf_Controller_Close_Display_File();

			ercd = BF_Dcf_Db_Get_Index_File_Info(file_index, open_filepath, NULL);
			if (ercd != 0) {
				return ercd;
			}

			g_bf_dcf_controller_display_file_id = BF_Mdf_Wrapper_Open(open_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
			if (g_bf_dcf_controller_display_file_id == 0) {
				ercd = g_bf_dcf_controller_display_file_id;
				g_bf_dcf_controller_display_file_id = 0;
				g_bf_dcf_controller_display_file_index = 0;
			}
			else {
				g_bf_dcf_controller_display_file_index = file_index.index;
				ercd = BF_Mdf_Wrapper_Fstat(g_bf_dcf_controller_display_file_id, fsif_stat);
				if (ercd != 0) {
					BF_Dcf_Controller_Close_Display_File();
				}
			}
		}
	}
	else {
		ercd = BF_Dcf_Db_Get_Index_File_Info(file_index, open_filepath, NULL);
		if (ercd != 0) {
			return ercd;
		}

		if (g_bf_dcf_controller_display_file_id != 0) {
			BF_Dcf_Controller_Close_Display_File();
		}

		g_bf_dcf_controller_display_file_index = file_index.index;
		g_bf_dcf_controller_display_file_id = BF_Mdf_Wrapper_Open(open_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
		if (g_bf_dcf_controller_display_file_id == 0) {
			ercd = g_bf_dcf_controller_display_file_id;
			g_bf_dcf_controller_display_file_id = 0;
			g_bf_dcf_controller_display_file_index = 0;
		}
		else {
			ercd = BF_Mdf_Wrapper_Fstat(g_bf_dcf_controller_display_file_id, fsif_stat);

			if (ercd != 0) {
				BF_Dcf_Controller_Close_Display_File();
			}
		}
	}

	return ercd;
}

/**
 * @brief thumbnail data load and analize exif.
 * @param i:*image_load_data image load data
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Exif_Thumbnail(T_BF_DCF_IF_IMAGE_LOAD_DATA* image_load_data, FW_ULONG* length, T_BF_DCF_IF_IMAGE_DETAIL* img_detail)
{
	FW_INT32 file_id;
	FW_ULONG result;
	FW_CHAR open_filepath[64];

	if(image_load_data == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	/* get file path and image info */
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(image_load_data->file_index, open_filepath, &(img_detail->file_info));
	if(g_bf_dcf_controller_ret_err != 0) {
		return g_bf_dcf_controller_ret_err;
	}

	if (image_load_data->file_index.index == g_bf_dcf_controller_display_file_index) {
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Lseek(g_bf_dcf_controller_display_file_id, 0, D_BF_MDF_WRAPPER_SEEK_SET, &result);
		if (g_bf_dcf_controller_ret_err != 0) {
			/* lseek NG */
			BF_Dcf_Controller_Close_Display_File();

			/* if seek error. reopen display file */
			file_id = BF_Mdf_Wrapper_Open(open_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
			if( file_id == 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", file_id, open_filepath));
#endif
				g_bf_dcf_controller_display_file_index = 0;
				g_bf_dcf_controller_display_file_id = 0;

				g_bf_dcf_controller_ret_err = file_id;
				return g_bf_dcf_controller_ret_err;
			}
			else {
				/* reopen OK */
				g_bf_dcf_controller_display_file_index = image_load_data->file_index.index;
				g_bf_dcf_controller_display_file_id = file_id;
			}
		}
		else {
			/* lseek OK */
			file_id = g_bf_dcf_controller_display_file_id;
		}
	}
	else {
		if (g_bf_dcf_controller_display_file_id != 0) {
			BF_Dcf_Controller_Close_Display_File();
		}

		file_id = BF_Mdf_Wrapper_Open(open_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
		if( file_id == 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", file_id, open_filepath));
#endif
			g_bf_dcf_controller_display_file_index = 0;
			g_bf_dcf_controller_display_file_id = 0;

			g_bf_dcf_controller_ret_err = file_id;
			return g_bf_dcf_controller_ret_err;
		}

		g_bf_dcf_controller_display_file_index = image_load_data->file_index.index;
		g_bf_dcf_controller_display_file_id = file_id;
	}

	if(BF_Dcf_Controller_Is_File_Type("JPG", image_load_data->file_type)) {
		g_bf_dcf_controller_ret_err = BF_Dcf_Exif_Analyze_Jpeg(file_id, (T_BF_DCF_IF_LOAD_IMAGE_DATA*)&g_bf_dcf_controller_dcfc_ret_load_image_data, img_detail);
	}
	else{
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("NOT SUPPORT FILE: file_type=%ld\n", image_load_data->file_type));
#endif
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_SUPPORT_FILE;
	}

	if(g_bf_dcf_controller_ret_err != 0){
		BF_Dcf_Controller_Close_Display_File();

		return g_bf_dcf_controller_ret_err;
	}

	if(g_bf_dcf_controller_dcfc_ret_load_image_data.thumb_size > image_load_data->img_size){
		BF_Dcf_Controller_Close_Display_File();

		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_THUMB_SIZE_OVER;
		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Lseek(file_id, (FW_INT32)g_bf_dcf_controller_dcfc_ret_load_image_data.thumb_offset, D_BF_MDF_WRAPPER_SEEK_SET, &result);
	if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, file_id));
#endif
		BF_Dcf_Controller_Close_Display_File();

		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = dcf_controller_read_with_measure_speed(file_id, image_load_data->img_addr, g_bf_dcf_controller_dcfc_ret_load_image_data.thumb_size, length);
	if(g_bf_dcf_controller_ret_err != 0){
		BF_Dcf_Controller_Close_Display_File();

		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF GET EIXF INFO AND LOAD THUMBNAIL END\n");
#endif

	return g_bf_dcf_controller_ret_err;
}


#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @brief thumbnail data load and analize exif.
 * @param i:*image_load_data image load data
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Ext_Exif_Thumbnail(T_BF_DCF_IF_IMAGE_LOAD_DATA* image_load_data, FW_ULONG* length, T_BF_DCF_IF_IMAGE_DETAIL* img_detail)
{
	FW_INT32 file_id;
	FW_ULONG result;
	FW_CHAR open_filepath[64];

	if(image_load_data == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	/* get file path and image info */
#ifdef CO_BF_DCF_IF_DATE_DB_ENABLE_TEMP
	if(dcf_controller_is_dcf_index(image_load_data->file_index) == TRUE){
		// DCF Index is specified.
		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(image_load_data->file_index, open_filepath, &(img_detail->file_info));
		if(g_bf_dcf_controller_ret_err != 0) {
			return g_bf_dcf_controller_ret_err;
		}
	}
	else{
		// Date Index is specified.

		// Get file full path
		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Date_Get_Index_File_Path(image_load_data->file_index.pack.obj_index, open_filepath);
		if(g_bf_dcf_controller_ret_err != 0) {
			return g_bf_dcf_controller_ret_err;
		}

		// Get file info
		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Date_Get_Index_File_Info(image_load_data->file_index.pack.obj_index, &(img_detail->file_info));
		if(g_bf_dcf_controller_ret_err != 0) {
			return g_bf_dcf_controller_ret_err;
		}
	}
#else // CO_BF_DCF_IF_DATE_DB_ENABLE_TEMP
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(image_load_data->file_index, open_filepath, &(img_detail->file_info));
	if(g_bf_dcf_controller_ret_err != 0) {
		return g_bf_dcf_controller_ret_err;
	}
#endif // CO_BF_DCF_IF_DATE_DB_ENABLE_TEMP

	if (image_load_data->file_index.index == g_bf_dcf_controller_display_file_index) {
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Lseek(g_bf_dcf_controller_display_file_id, 0, D_BF_MDF_WRAPPER_SEEK_SET, &result);
		if (g_bf_dcf_controller_ret_err != 0) {
			/* lseek NG */
			BF_Dcf_Controller_Close_Display_File();

			/* if seek error. reopen display file */
			file_id = BF_Mdf_Wrapper_Open(open_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
			if( file_id == 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", file_id, open_filepath));
#endif
				g_bf_dcf_controller_display_file_index = 0;
				g_bf_dcf_controller_display_file_id = 0;

				g_bf_dcf_controller_ret_err = file_id;
				return g_bf_dcf_controller_ret_err;
			}
			else {
				/* reopen OK */
				g_bf_dcf_controller_display_file_index = image_load_data->file_index.index;
				g_bf_dcf_controller_display_file_id = file_id;
			}
		}
		else {
			/* lseek OK */
			file_id = g_bf_dcf_controller_display_file_id;
		}
	}
	else {
		if (g_bf_dcf_controller_display_file_id != 0) {
			BF_Dcf_Controller_Close_Display_File();
		}

		file_id = BF_Mdf_Wrapper_Open(open_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
		if( file_id == 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", file_id, open_filepath));
#endif
			g_bf_dcf_controller_display_file_index = 0;
			g_bf_dcf_controller_display_file_id = 0;

			g_bf_dcf_controller_ret_err = file_id;
			return g_bf_dcf_controller_ret_err;
		}

		g_bf_dcf_controller_display_file_index = image_load_data->file_index.index;
		g_bf_dcf_controller_display_file_id = file_id;
	}

	if(BF_Dcf_Controller_Is_File_Type("MPO", image_load_data->file_type)) {
		g_bf_dcf_controller_ret_err = BF_Dcf_Exif_Analyze_Ext(file_id, (T_BF_DCF_IF_LOAD_IMAGE_DATA*)&g_bf_dcf_controller_dcfc_ret_load_image_data, img_detail);
	}
	else{
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("NOT SUPPORT FILE: file_type=%ld\n", image_load_data->file_type));
#endif
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_SUPPORT_FILE;
	}

	if(g_bf_dcf_controller_ret_err != 0){
		BF_Dcf_Controller_Close_Display_File();

		return g_bf_dcf_controller_ret_err;
	}

	if(g_bf_dcf_controller_dcfc_ret_load_image_data.thumb_size > image_load_data->img_size){
		BF_Dcf_Controller_Close_Display_File();

		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_THUMB_SIZE_OVER;
		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Lseek(file_id, (FW_INT32)g_bf_dcf_controller_dcfc_ret_load_image_data.thumb_offset, D_BF_MDF_WRAPPER_SEEK_SET, &result);
	if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, file_id));
#endif
		BF_Dcf_Controller_Close_Display_File();

		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = dcf_controller_read_with_measure_speed(file_id, image_load_data->img_addr, g_bf_dcf_controller_dcfc_ret_load_image_data.thumb_size, length);
	if(g_bf_dcf_controller_ret_err != 0){
		BF_Dcf_Controller_Close_Display_File();

		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF GET EIXF INFO AND LOAD THUMBNAIL END\n");
#endif

	return g_bf_dcf_controller_ret_err;
}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

/**
 * @brief VGA data loading.
 * @param i:*image_load_data image load data
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Load_Vga(T_BF_DCF_IF_IMAGE_LOAD_DATA* image_load_data, FW_ULONG* length)
{
	if(image_load_data == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	g_bf_dcf_controller_ret_err = dcf_controller_w_open_lseek_read(image_load_data->file_index, image_load_data->file_offset, image_load_data->img_addr, image_load_data->img_size, length);

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF LOAD VGA END\n");
#endif

	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief Get allowed type name by file type
 * @param  I : FW_UCHAR* ext_type
 *         O : FW_CHAR* allowed_ext_name
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Ext_Name_By_Typ(FW_UCHAR* ext_type, FW_CHAR* allowed_ext_name)
{
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Ext_By_Type(*ext_type, allowed_ext_name);
	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief Get full path by file_index
 * @param  I : FW_UCHAR* ext_type
 *         O : FW_CHAR* allowed_ext_name
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Full_Path_By_File_Index(FW_ULONG* input_file_index, FW_CHAR* full_path)
{
	T_BF_DCF_IF_FILE_INDEX file_index;
	file_index.index = *input_file_index;

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("BF_Dcf_Controller_Get_Full_Path_By_File_Index (START): input index=0x%08lx \n", file_index.index ));
#endif

	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Make_Full_Path( file_index, full_path);
	if ( g_bf_dcf_controller_ret_err != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Dcf_Controller_Get_Full_Path_By_File_Index (END): BF_Dcf_Db_Make_Full_Path() Err ercd=%d  \n", g_bf_dcf_controller_ret_err ));
#endif
		return g_bf_dcf_controller_ret_err;
	}
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("BF_Dcf_Controller_Get_Full_Path_By_File_Index (END): get full_path=%s \n", full_path ));
#endif
	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief Get index by directory name
 * @param  I : const FW_CHAR* dir_name
 *         O : FW_LONG* index
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Idx_By_Dir_Name(const FW_CHAR* dir_name, FW_LONG* index)
{
	FW_SHORT dir_index;

	if(dir_name == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_By_Dir(dir_name, &dir_index);
	if(FSDCF_ERRNO_OK != g_bf_dcf_controller_ret_err) {
		return g_bf_dcf_controller_ret_err;
	}

	*index = ((FW_LONG)dir_index) << 16;
	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief Get index by object name
 * @param  I : const FW_CHAR* obj_name
 *         O : FW_LONG* index
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Idx_By_Obj_Name(const FW_CHAR* obj_name, FW_LONG* index)
{
	T_BF_DCF_IF_FILE_INDEX file_index;

	if(obj_name == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_By_Object(obj_name,
																&(file_index.pack.dir_index),
																&(file_index.pack.obj_index));
	if(FSDCF_ERRNO_OK != g_bf_dcf_controller_ret_err) {
		return g_bf_dcf_controller_ret_err;
	}

	*index = file_index.index; /* pgr0872 */
	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief Read directory and object name in current directory
 * @param  I : FW_LONG* index
 *         O : FW_CHAR* dir_obj_name
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Read_Dir_Obj_Name(FW_LONG* index, FW_CHAR* dir_obj_name)
{
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Read_Dir_Obj_Name(dcf_controller_get_dir_index(*index), dcf_controller_get_obj_index(*index), dir_obj_name);
	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief format depend on DCF.
 * @param i:media_id media_id
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Format(FW_INT32* media_id)
{
	FW_INT32 ercd;

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("FORMAT START...\n"));
#endif

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	if(*gBF_Controller_drv_name[*media_id] != 0) {
		/* clear DCF DB */
		BF_Dcf_Db_Clear();
	}

	/* start filesystem init */
	ercd = BF_Fs_Controller_Format(*media_id);
	if( ercd != 0 ) {
		return ercd;
	}

	/* start DCF DB init */


#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "FORMAT END\n");
#endif

	return ercd;
}

/**
 * @brief get current used media and latest directory number and file number.
 * @param o:*dirno latest directory number
 * @param o:*fileno latest file number
 * @return error code
 * @note none
 * @attention none
 */
E_BF_MEDIA_STATE_MEDIA_TYPE BF_Dcf_Controller_Get_File_Name(FW_USHORT* dirno, FW_USHORT* fileno, E_BF_DCF_IF_LATEST_TYPE type)
{
	dcf_controller_get_last_file(dirno, fileno, type);
	return BF_Media_State_Get_Used_Media();
}



/**
 * @brief save jpeg.
 * @param i:*jpg_data saved jpeg data
 * @return error code
 * @note none
 * @attention none
 */
#if 1
FW_INT32 BF_Dcf_Controller_Save_Jpeg(T_BF_DCF_IF_JPEG_DATA* jpg_data)
{
#ifdef CO_BF_DCF_SAVE_MAINIMAGE_ONLY
	FW_INT32  ercd;
	FW_CHAR   make_filepath[64];
	FW_INT32  file_id = 0;
	FW_ULONG  length;
	FW_DOUBLE free_size;
#else
	FW_INT32  ercd;
	FW_CHAR   make_filepath[64];
	FW_INT32  file_id = 0;
	FW_USHORT app1_size;
	FW_USHORT write_app1_size = 0;
	FW_ULONG  jpg_head_length;
	FW_ULONG  length;
	FW_DOUBLE free_size;
	FW_CHAR*  tmp_buff;
#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	FW_ULONG       mpf_head_length = 0;
	FW_ULONG       app2_length = 0;
	FW_USHORT      scr1_app1_size = 0;
	FW_USHORT      scr2_app1_size = 0;
	FW_ULONG       mp_entry_offset = 0;
	FW_ULONG       mp_entry_num = 0;
	T_BF_DCF_EXIF_MP_ENTRY mp_entry[3];
	FW_ULONG       tmp_index = 0;
#endif
#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
	FW_ULONG       padding;
#endif
#endif // CO_BF_DCF_SAVE_MAINIMAGE_ONLY
	FW_ULONG tmp_app11_main_size = 0;
	FW_ULONG tmp_app11_screen_data1_size = 0;
	FW_ULONG tmp_app11_screen_data2_size = 0;

	if (jpg_data == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
#ifndef CO_BF_DCF_SAVE_MAINIMAGE_ONLY
	if (jpg_data->thumb_addr == 0) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
#endif
	if (jpg_data->main_addr == 0) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	// check free space in media
	ercd = BF_Fs_Controller_Get_FreeSize(BF_Media_State_Get_Used_Media(), &free_size);
	if (ercd != 0) {
		return ercd;
	}

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	if (free_size <= (jpg_data->thumb_size +
					  jpg_data->main_size +
					  jpg_data->screen_data1.screen_size +
					  jpg_data->screen_data2.screen_size +
					  D_BF_DCF_EXIF_THUM_START_ADD +
					  jpg_data->exif_info.maker_note_length +
					  tmp_app11_main_size +
					  tmp_app11_screen_data1_size +
					  tmp_app11_screen_data2_size)) {
#else	// CO_BF_DCF_IF_MPF_ENABLE
	if (free_size <= (jpg_data->thumb_size +
					  jpg_data->main_size +
					  D_BF_DCF_EXIF_THUM_START_ADD +
					  jpg_data->exif_info.maker_note_length +
					  tmp_app11_main_size)) {
#endif	// CO_BF_DCF_IF_MPF_ENABLE
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("FREE SPACE OVER: size=%lf\n", free_size));
#endif
		return FSDCF_ERRNO_FREE_SPACE_OVER;
	}

	// check thumbnail size
	if (D_BF_DCF_EXIF_MAX_APP1_SIZE <= (jpg_data->thumb_size + D_BF_DCF_EXIF_THUM_START_ADD + jpg_data->exif_info.maker_note_length)) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("APP1 SIZE OVER: thumb_size=%lu\n", jpg_data->thumb_size));
#endif
		return FSDCF_ERRNO_APP1_SIZE_OVER;
	}
	
#ifdef CO_A_FILE_NAME_LIST	// CO_A_FILE_NAME_LIST
	BOOL      result;

 	result = dcf_controller_search_file_name_list(jpg_data->jpg_idx, 0, make_filepath);
	if (!result) {
		ercd = dcf_controller_create_new_file("JPG", &file_id, make_filepath);
		dcf_controller_update_file_name_list(jpg_data->jpg_idx, 0, make_filepath);
	} 
	else 
	{
		ercd = dcf_controller_create_file_by_list("JPG", &file_id, make_filepath);
	}
#else	// A CO_A_FILE_NAME_LIST
	ercd = dcf_controller_create_new_file("JPG", &file_id, make_filepath);
#endif	// A CO_A_FILE_NAME_LIST  END
	if (ercd != 0) {
		return ercd;
	}

#ifndef CO_BF_DCF_SAVE_MAINIMAGE_ONLY

#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
	padding = (4 - (jpg_data->thumb_size % 4));
	if (padding == 4) {
		padding = 0;
	}
	jpg_data->thumb_size += padding;
#endif

	// make exif format
	BF_Dcf_Exif_Make_Jpeg(jpg_data);

	// make exif format from HOST
	if (TRUE != FJ_Host_ExifUpdateCB()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("FJ_Host_ExifUpdateCB False\n"));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return FSDCF_ERRNO_PARAM_FAILED;
	}

	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

	BF_Dcf_Exif_Copy_Exif_Jpeg(tmp_buff, &jpg_head_length);

	app1_size = jpg_head_length + jpg_data->thumb_size - 4;
	write_app1_size = BF_Dcf_Exif_Exchange_App_Size(app1_size);
	memcpy((FW_VOID*)(tmp_buff + 4), (FW_VOID*)&write_app1_size, 2);

	// write exif data
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)tmp_buff, jpg_head_length, &length);
	if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

	ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

	// write thumbnail data
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)jpg_data->thumb_addr, jpg_data->thumb_size, &length);
	if (ercd != 0) {
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	// make APP2 (MPF Index IFD)
	mp_entry_num = 1;
	if ((jpg_data->screen_data1.screen_addr != 0) && (jpg_data->screen_data1.screen_size != 0)) {
		mp_entry_num++;
	}
	if ((jpg_data->screen_data2.screen_addr != 0) && (jpg_data->screen_data2.screen_size != 0)) {
		mp_entry_num++;
	}

#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
	if (mp_entry_num > 1) {
		padding = (4 - (jpg_data->main_size % 4));
		if (padding == 4) {
			padding = 0;
		}
		jpg_data->main_size += padding;
	}
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON

#if 1
	if ((jpg_data->screen_data1.screen_addr != 0) && (jpg_data->screen_data1.screen_size != 0)) {
		BF_Dcf_Exif_Make_Exif_Mpg(0, &jpg_data->screen_data1);
		if (TRUE != FJ_Host_ExifUpdate_Class1_CB()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("FJ_Host_ExifUpdate_Class1_CB False\n"));
#endif
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return FSDCF_ERRNO_PARAM_FAILED;
		}

		BF_Dcf_Exif_Get_App1_Size_Mpf(0, &scr1_app1_size);
#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
		if ((jpg_data->screen_data1.thumb_addr != 0) && (jpg_data->screen_data1.thumb_size != 0)) {
			padding = (jpg_data->screen_data1.thumb_size % 4);
			if (padding == 0) {
				padding = 2;
			}
			else if (padding == 2) {
				padding = 0;
			}
			jpg_data->screen_data1.thumb_size += padding;

			padding = (4 - (scr1_app1_size % 4));
			if (padding == 4) {
				padding = 0;
			}
		}
		else {
			padding = (scr1_app1_size % 4);
			if (padding == 0) {
				padding = 2;
			}
			else if (padding == 2) {
				padding = 0;
			}
			else {
				; // DO NOTHING
			}
		}

		scr1_app1_size += padding;

		if (mp_entry_num > 2) {
			padding = (4 - (jpg_data->screen_data1.screen_size % 4));
			if (padding == 4) {
				padding = 0;
			}
			jpg_data->screen_data1.screen_size += padding;
		}
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON
	}

	if ((jpg_data->screen_data2.screen_addr != 0) && (jpg_data->screen_data2.screen_size != 0)) {
		// make APP1 (Exif for ScreenNail -2)
		BF_Dcf_Exif_Make_Exif_Mpg(1, &jpg_data->screen_data2);
		if (TRUE != FJ_Host_ExifUpdate_Class2_CB()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("FJ_Host_ExifUpdate_Class2_CB False\n"));
#endif
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return FSDCF_ERRNO_PARAM_FAILED;
		}

		BF_Dcf_Exif_Get_App1_Size_Mpf(1, &scr2_app1_size);
#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
		if ((jpg_data->screen_data2.thumb_addr != 0) && (jpg_data->screen_data2.thumb_size != 0)) {
			padding = (jpg_data->screen_data2.thumb_size % 4);
			if (padding == 0) {
				padding = 2;
			}
			else if (padding == 2) {
				padding = 0;
			}
			else {
				; // DO NOTHING
			}
			jpg_data->screen_data2.thumb_size += padding;

			padding = (4 - (scr2_app1_size % 4));
			if (padding == 4) {
				padding = 0;
			}
		}
		else {
			padding = (scr2_app1_size % 4);
			if (padding == 0) {
				padding = 2;
			}
			else if (padding == 2) {
				padding = 0;
			}
			else {
				; // DO NOTHING
			}
		}

		scr2_app1_size += padding;
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON
	}
#endif

	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

#if 1
//	BF_Dcf_Exif_Copy_App2_Mpf(tmp_buff, &app2_length, &mp_entry_offset, mp_entry_num, D_BF_DCF_EXIF_BIG_EN);
	BF_Dcf_Exif_Copy_App2_Mpf(tmp_buff, &app2_length, &mp_entry_offset, mp_entry_num, BF_Dcf_Exif_Get_Exif_Endian());

#else
	BF_Dcf_Exif_Copy_App2_Mpf(tmp_buff, &app2_length, &mp_entry_offset, mp_entry_num);
#endif

#if 1
	// Make MP Entry data
	mp_entry[0].mp_info        = 0xA0030000;		// Baseline Main Image
	mp_entry[0].mp_data_size   = jpg_data->main_size + app2_length + app1_size + 2 + tmp_app11_main_size;
	mp_entry[0].mp_data_offset = 0x00000000;
	if (mp_entry_num == 1) {
		mp_entry[0].mp_info    = 0x20030000;		// Baseline Main Image (No Dependent Image)
		mp_entry[0].mp_entry_1 = 0x0000;
		mp_entry[0].mp_entry_2 = 0x0000;
	}
	if (mp_entry_num == 2) {
		mp_entry[0].mp_entry_1 = 0x0002;
		mp_entry[0].mp_entry_2 = 0x0000;
	}
	if (mp_entry_num == 3) {
		mp_entry[0].mp_entry_1 = 0x0002;
		mp_entry[0].mp_entry_2 = 0x0003;
	}

	tmp_index = 1;
	if ((jpg_data->screen_data1.screen_addr != 0) && (jpg_data->screen_data1.screen_size != 0)) {
		mp_entry[tmp_index].mp_info        = 0x40010001;	// Moniter ScreenNail (VGA)
		mp_entry[tmp_index].mp_data_size   = jpg_data->screen_data1.screen_size + scr1_app1_size - 2 + jpg_data->screen_data1.thumb_size + tmp_app11_screen_data1_size;
		mp_entry[tmp_index].mp_data_offset = app2_length - 8 + jpg_data->main_size - 2 + tmp_app11_main_size;
		mp_entry[tmp_index].mp_entry_1 = 0x0000;
		mp_entry[tmp_index].mp_entry_2 = 0x0000;
		tmp_index++;
	}

	if ((jpg_data->screen_data2.screen_addr != 0) && (jpg_data->screen_data2.screen_size != 0)) {
		mp_entry[tmp_index].mp_info        = 0x40010002;	// Moniter ScreenNail (Full HD)
		mp_entry[tmp_index].mp_data_size   = jpg_data->screen_data2.screen_size + scr2_app1_size - 2 + jpg_data->screen_data2.thumb_size + tmp_app11_screen_data2_size;
		if (tmp_index == 1) {
			mp_entry[tmp_index].mp_data_offset = app2_length - 8 + jpg_data->main_size - 2 + tmp_app11_main_size;
		}
		else {
			mp_entry[tmp_index].mp_data_offset = mp_entry[tmp_index - 1].mp_data_offset + mp_entry[tmp_index - 1].mp_data_size;
		}
		mp_entry[tmp_index].mp_entry_1 = 0x0000;
		mp_entry[tmp_index].mp_entry_2 = 0x0000;
	}

#if 1
	for (tmp_index = 0; tmp_index < mp_entry_num; tmp_index++) {
		BF_Dcf_Exif_Exchange_Endian_Mpf_Entry(BF_Dcf_Exif_Get_Exif_Endian(), &mp_entry[tmp_index]);
	}
#endif

	memcpy((tmp_buff + mp_entry_offset), &mp_entry[0], (16 * mp_entry_num));
#else
	mp_entry_offset += (jpg_head_length + jpg_data->thumb_size);
#endif

	// write APP2 (MPF Index IFD)
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)tmp_buff, app2_length, &length);
	if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

	ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}
#endif	// CO_BF_DCF_IF_MPF_ENABLE

#endif //CO_BF_DCF_SAVE_MAINIMAGE_ONLY

	// write main data
#ifdef CO_BF_DCF_SAVE_MAINIMAGE_ONLY
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)(jpg_data->main_addr), (jpg_data->main_size), &length);         // save main image only
#else
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)(jpg_data->main_addr + 2), (jpg_data->main_size - 2), &length);
#endif
	if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

#ifndef CO_BF_DCF_SAVE_MAINIMAGE_ONLY

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	// make APP1 (Exif for ScreenNail -1)
	if ((jpg_data->screen_data1.screen_addr != 0) && (jpg_data->screen_data1.screen_size != 0)) {
#if 0
		BF_Dcf_Exif_Make_Exif_Mpg(&jpg_data->screen_data1);
		if (TRUE != FJ_Host_ExifUpdate_Class1_CB()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("FJ_Host_ExifUpdate_Class1_CB False\n"));
#endif
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return FSDCF_ERRNO_PARAM_FAILED;
		}
#endif
		ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return ercd;
		}

#if 1
//		BF_Dcf_Exif_Copy_Exif_Mpf(0, D_BF_DCF_EXIF_BIG_EN, tmp_buff, &mpf_head_length);
		BF_Dcf_Exif_Copy_Exif_Mpf(0, BF_Dcf_Exif_Get_Exif_Endian(), tmp_buff, &mpf_head_length);
#else
		BF_Dcf_Exif_Copy_Exif_Mpf(0, tmp_buff, &mpf_head_length);
#endif
#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
		if (jpg_data->screen_data1.thumb_size != 0) {
			padding = (4 - (mpf_head_length % 4));
			if (padding == 4) {
				padding = 0;
			}
		}
		else {
			padding = (mpf_head_length % 4);
			if (padding == 0) {
				padding = 2;
			}
			else if (padding == 2) {
				padding = 0;
			}
			else {
				; // DO NOTHING
			}
		}
		mpf_head_length += padding;
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON

		scr1_app1_size = mpf_head_length + jpg_data->screen_data1.thumb_size - 4;
		scr1_app1_size = BF_Dcf_Exif_Exchange_App_Size(scr1_app1_size);
		memcpy((FW_VOID*)(tmp_buff + 4), (FW_VOID*)&scr1_app1_size, 2);

		// write APP1 (Exif for ScreenNail -1)
		ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)tmp_buff, mpf_head_length, &length);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return ercd;
		}

		ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return ercd;
		}

		if ((jpg_data->screen_data1.thumb_addr != 0) && (jpg_data->screen_data1.thumb_size != 0)) {
			// write ThumbNail for ScreenNail -1
			ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)jpg_data->screen_data1.thumb_addr, jpg_data->screen_data1.thumb_size, &length);
			if (ercd != 0) {
				BF_Mdf_Wrapper_Close(file_id);
				BF_Mdf_Wrapper_Remove(make_filepath);
				return ercd;
			}
		}

		// write ScreenNail -1 data
		ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)(jpg_data->screen_data1.screen_addr + 2), (jpg_data->screen_data1.screen_size - 2), &length);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return ercd;
		}
	}


	if ((jpg_data->screen_data2.screen_addr != 0) && (jpg_data->screen_data2.screen_size != 0)) {
		// make APP1 (Exif for ScreenNail -2)
#if 0
		BF_Dcf_Exif_Make_Exif_Mpg(&jpg_data->screen_data2);
		if (TRUE != FJ_Host_ExifUpdate_Class2_CB()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("FJ_Host_ExifUpdate_Class2_CB False\n"));
#endif
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return FSDCF_ERRNO_PARAM_FAILED;
		}
#endif
		ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return ercd;
		}

#if 1
//		BF_Dcf_Exif_Copy_Exif_Mpf(1, D_BF_DCF_EXIF_BIG_EN, tmp_buff, &mpf_head_length);
		BF_Dcf_Exif_Copy_Exif_Mpf(1, BF_Dcf_Exif_Get_Exif_Endian(), tmp_buff, &mpf_head_length);
#else
		BF_Dcf_Exif_Copy_Exif_Mpf(1, tmp_buff, &mpf_head_length);
#endif
#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
		if (jpg_data->screen_data2.thumb_size != 0) {
			padding = (4 - (mpf_head_length % 4));
			if (padding == 4) {
				padding = 0;
			}
		}
		else {
			padding = (mpf_head_length % 4);
			if (padding == 0) {
				padding = 2;
			}
			else if (padding == 2) {
				padding = 0;
			}
			else {
				; // DO NOTHING
			}
		}
		mpf_head_length += padding;
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON

		scr2_app1_size = mpf_head_length + jpg_data->screen_data2.thumb_size - 4;
		scr2_app1_size = BF_Dcf_Exif_Exchange_App_Size(scr2_app1_size);
		memcpy((FW_VOID*)(tmp_buff + 4), (FW_VOID*)&scr2_app1_size, 2);

		// write APP1 (Exif for ScreenNail -2)
		ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)tmp_buff, mpf_head_length, &length);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return ercd;
		}

		ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return ercd;
		}

		if ((jpg_data->screen_data2.thumb_addr != 0) && (jpg_data->screen_data2.thumb_size != 0)) {
			// write ThumbNail for ScreenNail -2
			ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)jpg_data->screen_data2.thumb_addr, jpg_data->screen_data2.thumb_size, &length);
			if (ercd != 0) {
				BF_Mdf_Wrapper_Close(file_id);
				BF_Mdf_Wrapper_Remove(make_filepath);
				return ercd;
			}
		}

		// write ScreenNail -2 data
		ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)(jpg_data->screen_data2.screen_addr + 2), (jpg_data->screen_data2.screen_size - 2), &length);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return ercd;
		}
	}

	BF_Dcf_Exif_Reset_App2_Mpf();
	BF_Dcf_Exif_Reset_App1_Mpf_Resize(0);
	BF_Dcf_Exif_Reset_App1_Mpf_Resize(1);

#if 0
	// Make MP Entry data
	mp_entry[0].mp_info        = 0xA0030000;		// Baseline Main Image
	mp_entry[0].mp_data_size   = jpg_data->main_size + app2_length + app1_size + 2;
	mp_entry[0].mp_data_offset = 0x00000000;
	if (mp_entry_num == 1) {
		mp_entry[0].mp_entry_1 = 0x0000;
		mp_entry[0].mp_entry_2 = 0x0000;
	}
	if (mp_entry_num == 2) {
		mp_entry[0].mp_entry_1 = 0x0002;
		mp_entry[0].mp_entry_2 = 0x0000;
	}
	if (mp_entry_num == 3) {
		mp_entry[0].mp_entry_1 = 0x0002;
		mp_entry[0].mp_entry_2 = 0x0003;
	}

	tmp_index = 1;
	if ((jpg_data->screen_data1.screen_addr != 0) && (jpg_data->screen_data1.screen_size != 0)) {
		mp_entry[tmp_index].mp_info        = 0x40010001;	// Moniter ScreenNail (VGA)
		mp_entry[tmp_index].mp_data_size   = jpg_data->screen_data1.screen_size + scr1_app1_size + 2;
		mp_entry[tmp_index].mp_data_offset = app2_length - 8 + jpg_data->main_size - 2;
		mp_entry[tmp_index].mp_entry_1 = 0x0000;
		mp_entry[tmp_index].mp_entry_2 = 0x0000;
		tmp_index++;
	}

	if ((jpg_data->screen_data2.screen_addr != 0) && (jpg_data->screen_data2.screen_size != 0)) {
		mp_entry[tmp_index].mp_info        = 0x40010002;	// Moniter ScreenNail (Full HD)
		mp_entry[tmp_index].mp_data_size   = jpg_data->screen_data2.screen_size + scr2_app1_size + 2;
		if (tmp_index == 1) {
			mp_entry[tmp_index].mp_data_offset = app2_length - 8 + jpg_data->main_size - 2;
		}
		else {
			mp_entry[tmp_index].mp_data_offset = mp_entry[tmp_index - 1].mp_data_offset + mp_entry[tmp_index - 1].mp_data_size;
		}
		mp_entry[tmp_index].mp_entry_1 = 0x0000;
		mp_entry[tmp_index].mp_entry_2 = 0x0000;
	}

	// Write MP Entry data
	ercd = BF_Mdf_Wrapper_Lseek(file_id, mp_entry_offset, SEEK_SET, &length);
	if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

	ercd = BF_Mdf_Wrapper_Write(file_id, mp_entry, (sizeof(T_BF_DCF_EXIF_MP_ENTRY) * mp_entry_num), &length);
	if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}
#endif
#endif	// CO_BF_DCF_IF_MPF_ENABLE

#endif //CO_BF_DCF_SAVE_MAINIMAGE_ONLY

	ercd = dcf_controller_close_new_file(file_id, make_filepath, E_BF_DCF_IF_REMOVE_FLG_OFF);

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF JPEG SAVE END\n");
#endif

	return ercd;
}
#else
FW_INT32 BF_Dcf_Controller_Save_Jpeg(T_BF_DCF_IF_JPEG_DATA* jpg_data)
{
	FW_INT32 ercd;
	FW_CHAR make_filepath[64];
	FW_INT32 file_id;
	FW_CHAR app2[9] = { 0xFF, 0xE2, 0, 0, 0x51, 0x56, 0x47, 0x41, 0 };
	FW_USHORT app1_size, app2_size;
	FW_CHAR* tmp_buff;
	FW_ULONG jpg_head_length;
	FW_ULONG length;
	FW_DOUBLE free_size;
	FW_INT32 param_arry[3];
	FW_LONG ret_appn;

	if(jpg_data == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(jpg_data->thumb_addr == 0){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(jpg_data->main_addr == 0){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	/* check free space in media */
	ercd = BF_Fs_Controller_Get_FreeSize(BF_Media_State_Get_Used_Media(), &free_size);
	if(ercd != 0){
		return ercd;
	}

	if(free_size <= (jpg_data->thumb_size + jpg_data->main_size + jpg_data->screen_size + D_BF_DCF_EXIF_THUM_START_ADD + jpg_data->exif_info.maker_note_length)){

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("FREE SPACE OVER: size=%lf\n", free_size));
#endif
		return FSDCF_ERRNO_FREE_SPACE_OVER;
	}

	/* check thumbnail size */
	if(D_BF_DCF_EXIF_MAX_APP1_SIZE <= (jpg_data->thumb_size + D_BF_DCF_EXIF_THUM_START_ADD + jpg_data->exif_info.maker_note_length)){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("APP1 SIZE OVER: thumb_size=%d\n", jpg_data->thumb_size));
#endif
		return FSDCF_ERRNO_APP1_SIZE_OVER;
	}


	ercd = dcf_controller_create_new_file("JPG", &file_id, make_filepath);
	if( ercd != 0 ){
		return ercd;
	}

	/* make exif format */
	BF_Dcf_Exif_Make_Jpeg(jpg_data);

	/* make exif format */

	if( TRUE != FJ_Host_ExifUpdateCB() ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("FJ_Host_ExifUpdateCB False\n"));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return FSDCF_ERRNO_PARAM_FAILED;
	}

	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

	BF_Dcf_Exif_Copy_Exif_Jpeg(tmp_buff, &jpg_head_length);

	app1_size = jpg_head_length + jpg_data->thumb_size - 4;
	memcpy((FW_VOID*)(tmp_buff + 4), (FW_VOID*)&app1_size, 2);

	if( jpg_data->exif_info.qvga_enable == 1 ){
		app2_size = jpg_data->screen_size + 7;
		memcpy( (FW_VOID*)&app2[2], (FW_VOID*)&app2_size, 2 );
	}

	/* write exif data */
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)tmp_buff, jpg_head_length, &length);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

	ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}


	/* lseek and write thumbnail data */
//	ercd = dcf_controller_w_lseek_write(file_id, jpg_head_length, jpg_data->thumb_addr, jpg_data->thumb_size, &length);
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)jpg_data->thumb_addr, jpg_data->thumb_size, &length);
	if( ercd != 0 ){
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

	/* write APP2 header */
	param_arry[0] = jpg_data->screen_addr;
	param_arry[1] = jpg_data->screen_size;
	param_arry[2] = jpg_data->main_size;

	ret_appn = FJ_Host_CreateAPPnCB( (CHAR*)jpg_data->screen_addr, jpg_data->screen_size, 2, param_arry, 3 );

	if( ret_appn != 0 ){
		/* written size and function will call again */
	}
	else{
		/*  terminate function call */
	}


	if( jpg_data->exif_info.qvga_enable == 1 ){
		ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)app2, 9, &length);
		if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return ercd;
		}

		/* write screen data */
		ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)(jpg_data->screen_addr), (jpg_data->screen_size), &length);
		if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Mdf_Wrapper_Close(file_id);
			BF_Mdf_Wrapper_Remove(make_filepath);
			return ercd;
		}
	}

	/* write main data */
//	memset((FW_CHAR*)(jpg_data->main_addr), 0, 2);	//! Delete SOI code (2byte)
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)(jpg_data->main_addr + 2), (jpg_data->main_size - 2), &length);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

	ercd = dcf_controller_close_new_file(file_id, make_filepath, E_BF_DCF_IF_REMOVE_FLG_OFF);

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF JPEG SAVE END\n");
#endif


	return ercd;
}
#endif


/**
 * @brief save raw.
 * @param i:*raw_data saved RAW data
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Save_Raw(T_BF_DCF_IF_RAW_DATA* raw_data)
{
	FW_INT32	ercd=0;
	FW_CHAR 	make_filepath[64];
	FW_CHAR 	make_fileext[4];
	FW_INT32	file_id = 0;
	FW_ULONG	length;
	FW_DOUBLE	free_size;

	if (raw_data == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (raw_data->main_addr == 0) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	// check free space in media
	ercd = BF_Fs_Controller_Get_FreeSize(BF_Media_State_Get_Used_Media(), &free_size);
	if (ercd != 0) {
		return ercd;
	}
	if(free_size <= ( raw_data->main_size) ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("FREE SPACE OVER(RAW): size=%lf\n", free_size));
#endif
		return FSDCF_ERRNO_FREE_SPACE_OVER;
	}
	
	if (raw_data->data_type == E_BF_DCF_IF_DATA_TYPE_DNG) {
		strcpy(make_fileext, "DNG");
	}
	else {
		strcpy(make_fileext, "RAW");
	}
	
#ifdef CO_A_FILE_NAME_LIST	// CO_A_FILE_NAME_LIST
	BOOL		result;
 	result = dcf_controller_search_file_name_list(raw_data->main_idx, 1, make_filepath);
	if (!result) {
		// make new Raw file.
		ercd = dcf_controller_create_new_file(make_fileext, &file_id, make_filepath);
		dcf_controller_update_file_name_list(raw_data->main_idx, 1, make_filepath);
	} 
	else {
		ercd = dcf_controller_create_file_by_list(make_fileext, &file_id, make_filepath);
	}
#else
	// make new Raw file.
	ercd = dcf_controller_create_new_file(make_fileext, &file_id, make_filepath);
#endif	// A CO_A_FILE_NAME_LIST  END

	// write raw main data
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)raw_data->main_addr, raw_data->main_size, &length);
	if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Dcf_Controller_Save_Raw: BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}
	
	// close file
	ercd = dcf_controller_close_new_file(file_id, make_filepath, E_BF_DCF_IF_REMOVE_FLG_OFF);

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF JPEG SAVE END\n");
#endif



	return ercd;
}


/**
 * @brief save bayer.
 * @param i:*raw_data saved Bayer data
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Save_Bayer(T_BF_DCF_IF_RAW_DATA* raw_data)
{
	static USHORT gRawStillCount = 0;

	FJ_ERR_CODE	err_code = FJ_ERR_OK;
	CHAR		raw_filepath[ 32 ] = "I:\\BAYER";
	INT32		fs_file_id;
	UINT32		attr;
	ULONG		write_byte;

// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#ifdef CO_WIFI_EMMC_SAVE
	if (RS_System_Attr_Get_WIFI_Out_EMMC_Enabled() == FALSE) {
		sprintf( raw_filepath, "I:\\BAYER" );
	}
	else {
		sprintf( raw_filepath, "L:\\BAYER" );
	}
#endif
// --- REMOVE_WIFI_EMMC_SAVE END ---

	err_code = FJ_Fs_GetAttribute( raw_filepath, &attr );
	if ( err_code != FJ_ERR_OK ) {
		FJ_Fs_Mkdir( raw_filepath );
	}
	
// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#ifndef CO_WIFI_EMMC_SAVE
// --- REMOVE_WIFI_EMMC_SAVE END ---
	sprintf( raw_filepath, "I:\\BAYER\\BAY%04d.raw", gRawStillCount );
// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#else
	if (RS_System_Attr_Get_WIFI_Out_EMMC_Enabled() == FALSE) {
		sprintf( raw_filepath, "I:\\BAYER\\BAY%04d.raw", gRawStillCount );
	}
	else {
		sprintf( raw_filepath, "L:\\BAYER\\BAY%04d.raw", gRawStillCount );
	}
#endif
// --- REMOVE_WIFI_EMMC_SAVE END ---
	gRawStillCount++;

	// file write
	fs_file_id = FJ_Fs_Open( raw_filepath, (FJ_FS_OPEN_CREAT | FJ_FS_OPEN_RDWR) );
	if ( fs_file_id == 0L ) {
		BF_Fs_User_Custom_Err_Printer(("FJ_Fs_Open() open error. \"%s\"\n", raw_filepath));
		return FJ_ERR_NG;
	}
	err_code = FJ_Fs_Write( fs_file_id, (UINT32*)raw_data->main_addr, raw_data->main_size, &write_byte );
	if ( err_code != FJ_ERR_OK ) {
		BF_Fs_User_Custom_Err_Printer(("FJ_Fs_Write() write error. %u\n", err_code));
	}
	FJ_Fs_Close( fs_file_id );

#if 1	// for debug
	BF_Debug_Print_Information(("Bayer ImageInfo :  size=%ld %s\n", raw_data->main_size, raw_filepath ));
#endif	// for debug

	return err_code;
}


#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @brief save mpo.
 * @param i:*jpg_data saved mpo data
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Save_Ext(T_BF_DCF_IF_JPEG_DATA* jpg_data)
{
	FW_INT32  ercd;
	FW_CHAR   make_filepath[64];
	FW_INT32  file_id;
	FW_USHORT app1_size;
	FW_USHORT write_app1_size;
	FW_ULONG  jpg_head_length;
	FW_ULONG  length;
	FW_DOUBLE free_size;
	FW_CHAR*  tmp_buff;
#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	FW_ULONG       mpf_head_length = 0;
	FW_ULONG       app2_length = 0;
	FW_USHORT      scr1_app1_size = 0;
	FW_USHORT      scr2_app1_size = 0;
	FW_ULONG       mp_entry_offset = 0;
	FW_ULONG       mp_entry_num = 0;
	T_BF_DCF_EXIF_MP_ENTRY mp_entry[6];
	FW_ULONG       tmp_index = 0;
#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
	FW_USHORT ext_space_count = 0;
	FW_ULONG   ext_cnt = 0;
	FW_ULONG   main_index;
	FW_ULONG   ext_app2_length;
#endif // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
#endif // CO_BF_DCF_IF_MPF_ENABLE
#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
	FW_ULONG       padding;
#endif

	if (jpg_data == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if ((jpg_data->ext_mp_num == 0) ||(jpg_data->ext_mp_num > E_BF_DCF_IF_MPF_EXT_MP_SELECT_MAX)) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (jpg_data->ext_mp[0].thumb_addr == 0) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (jpg_data->ext_mp[0].main_addr == 0) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	// check free space in media
	ercd = BF_Fs_Controller_Get_FreeSize(BF_Media_State_Get_Used_Media(), &free_size);
	if (ercd != 0) {
		return ercd;
	}

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
	ext_space_count = (D_BF_DCF_EXIF_THUM_START_ADD + jpg_data->exif_info.maker_note_length);
	for (ext_cnt=0; ext_cnt<jpg_data->ext_mp_num; ext_cnt++) {
		ext_space_count += jpg_data->ext_mp[ext_cnt].main_size;
		ext_space_count += jpg_data->ext_mp[ext_cnt].thumb_size;
		ext_space_count += jpg_data->ext_mp[ext_cnt].screen_data1.screen_size;
		ext_space_count += jpg_data->ext_mp[ext_cnt].screen_data2.screen_size;
	}
	if (free_size <= ext_space_count) {
#else      // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
	if (free_size <= (jpg_data->thumb_size + jpg_data->main_size + jpg_data->screen_data1.screen_size + jpg_data->screen_data2.screen_size + D_BF_DCF_EXIF_THUM_START_ADD + jpg_data->exif_info.maker_note_length)) {
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
#else	// CO_BF_DCF_IF_MPF_ENABLE
	if (free_size <= (jpg_data->thumb_size + jpg_data->main_size + D_BF_DCF_EXIF_THUM_START_ADD + jpg_data->exif_info.maker_note_length)) {
#endif	// CO_BF_DCF_IF_MPF_ENABLE
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("FREE SPACE OVER: size=%lf\n", free_size));
#endif
		return FSDCF_ERRNO_FREE_SPACE_OVER;
	}

	// check thumbnail size
	if (D_BF_DCF_EXIF_MAX_APP1_SIZE <= (jpg_data->ext_mp[0].thumb_size + D_BF_DCF_EXIF_THUM_START_ADD + jpg_data->ext_mp[0].exif_info.maker_note_length)) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("APP1 SIZE OVER: thumb_size=%lu\n", jpg_data->ext_mp[0].thumb_size));
#endif
		return FSDCF_ERRNO_APP1_SIZE_OVER;
	}

	ercd = dcf_controller_create_new_file("MPO", &file_id, make_filepath);
	if (ercd != 0) {
		return ercd;
	}

#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
	padding = (4 - (jpg_data->ext_mp[0].thumb_size % 4));
	if (padding == 4) {
		padding = 0;
	}
	jpg_data->ext_mp[0].thumb_size += padding;
#endif
	
	// make exif format
	BF_Dcf_Exif_Make_Ext(&jpg_data->ext_mp[0]);

	// make exif format from HOST
	if (TRUE != FJ_Host_ExifUpdateCB()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("FJ_Host_ExifUpdateCB False\n"));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return FSDCF_ERRNO_PARAM_FAILED;
	}

	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

	BF_Dcf_Exif_Copy_Exif_Jpeg(tmp_buff, &jpg_head_length);

	app1_size = jpg_head_length + jpg_data->ext_mp[0].thumb_size - 4;
	write_app1_size = BF_Dcf_Exif_Exchange_App_Size(app1_size);
	memcpy((FW_VOID*)(tmp_buff + 4), (FW_VOID*)&write_app1_size, 2);

	// write exif data
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)tmp_buff, jpg_head_length, &length);
	if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

	ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

	// write thumbnail data
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)jpg_data->ext_mp[0].thumb_addr, jpg_data->ext_mp[0].thumb_size, &length);
	if (ercd != 0) {
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	mp_entry_num = 0;
	for (ext_cnt = 0; ext_cnt<jpg_data->ext_mp_num; ext_cnt++) {
		if ((jpg_data->ext_mp[ext_cnt].main_addr != 0) && (jpg_data->ext_mp[ext_cnt].main_size != 0)) {
			mp_entry_num++;
			
			if ((jpg_data->ext_mp[ext_cnt].screen_data1.screen_addr != 0) && (jpg_data->ext_mp[ext_cnt].screen_data1.screen_size != 0)) {
				mp_entry_num++;
			}
			if ((jpg_data->ext_mp[ext_cnt].screen_data2.screen_addr != 0) && (jpg_data->ext_mp[ext_cnt].screen_data2.screen_size != 0)) {
				mp_entry_num++;
			}
		}
	}

#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
	if (mp_entry_num > 1) {
		padding = (4 - (jpg_data->ext_mp[0].main_size % 4));
		if (padding == 4) {
			padding = 0;
		}
		jpg_data->ext_mp[0].main_size += padding;
	}
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON

	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

#if 1	
	tmp_index = 0;
	for (ext_cnt = 0; ext_cnt<jpg_data->ext_mp_num; ext_cnt++) {
		if ((jpg_data->ext_mp[ext_cnt].main_addr!= 0) && (jpg_data->ext_mp[ext_cnt].main_size != 0)) {
			mp_entry[tmp_index].mp_info        = 0x00020002;
			if (ext_cnt == 0) {
				mp_entry[tmp_index].mp_data_offset = 0x00000000;
				BF_Dcf_Exif_Copy_Extended_First_App2_Mpf(tmp_buff, &app2_length, &mp_entry_offset, mp_entry_num, BF_Dcf_Exif_Get_Exif_Endian());
				mp_entry[tmp_index].mp_data_size   = jpg_data->ext_mp[ext_cnt].main_size + app2_length + app1_size + 2;
			}
			else {
				if (tmp_index == 1) {
					mp_entry[tmp_index].mp_data_offset = app2_length - 8 + jpg_data->ext_mp[0].main_size - 2;
				}
				else {
					mp_entry[tmp_index].mp_data_offset = mp_entry[tmp_index - 1].mp_data_offset + mp_entry[tmp_index - 1].mp_data_size;
				}
				ext_app2_length = sizeof(T_BF_DCF_EXIF_MPF_EXTENDED_APP2);
#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
				padding = (4 - (ext_app2_length % 4));
				if (padding == 4) {
					padding = 0;
				}
				ext_app2_length += padding;
				padding = (4 - (jpg_data->ext_mp[ext_cnt].thumb_size % 4));
				if (padding == 4) {
					padding = 0;
				}
				jpg_data->ext_mp[ext_cnt].thumb_size += padding;

 				if (tmp_index < (mp_entry_num - 1) ) {
					padding = (4 - (jpg_data->ext_mp[ext_cnt].main_size % 4));
					if (padding == 4) {
						padding = 0;
					}
					jpg_data->ext_mp[ext_cnt].main_size += padding;
				}
#endif
				mp_entry[tmp_index].mp_data_size   = jpg_data->ext_mp[ext_cnt].main_size + ext_app2_length + jpg_data->ext_mp[ext_cnt].thumb_size + jpg_head_length - 4;
			}
			mp_entry[tmp_index].mp_entry_1 = 0x0000;
			mp_entry[tmp_index].mp_entry_2 = 0x0000;
			main_index = tmp_index;
			tmp_index++;
			if ( tmp_index >= 6 ) {
				return FSDCF_ERRNO_PARAM_FAILED;
			}
			
			if ((jpg_data->ext_mp[ext_cnt].screen_data1.screen_addr != 0) && (jpg_data->ext_mp[ext_cnt].screen_data1.screen_size != 0)) {
				mp_entry[main_index].mp_info |= 0x80000000;            // Dependent Parent Image Flag
				mp_entry[main_index].mp_entry_1 = tmp_index+1;

				BF_Dcf_Exif_Make_Exif_Mpg(0, &jpg_data->ext_mp[ext_cnt].screen_data1);
				if (TRUE != FJ_Host_ExifUpdate_Class1_CB()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
					BF_Fs_User_Custom_Wrn_Printer(("FJ_Host_ExifUpdate_Class1_CB False\n"));
#endif
					BF_Mdf_Wrapper_Close(file_id);
					BF_Mdf_Wrapper_Remove(make_filepath);
					return FSDCF_ERRNO_PARAM_FAILED;
				}
				BF_Dcf_Exif_Get_App1_Size_Mpf(0, &scr1_app1_size);

#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
				if ((jpg_data->ext_mp[ext_cnt].screen_data1.thumb_addr != 0) && (jpg_data->ext_mp[ext_cnt].screen_data1.thumb_size != 0)) {
					padding = (jpg_data->ext_mp[ext_cnt].screen_data1.thumb_size % 4);
					if (padding == 0) {
						padding = 2;
					}
					else if (padding == 2) {
						padding = 0;
					}
					jpg_data->ext_mp[ext_cnt].screen_data1.thumb_size += padding;

					padding = (4 - (scr1_app1_size % 4));
					if (padding == 4) {
						padding = 0;
					}
				}
				else {
					padding = (scr1_app1_size % 4);
					if (padding == 0) {
						padding = 2;
					}
					else if (padding == 2) {
						padding = 0;
					}
					else {
						; // DO NOTHING
					}
				}

				scr1_app1_size += padding;

 				if (tmp_index < (mp_entry_num - 1) ) {
					padding = (4 - (jpg_data->ext_mp[ext_cnt].screen_data1.screen_size % 4));
					if (padding == 4) {
						padding = 0;
					}
					jpg_data->ext_mp[ext_cnt].screen_data1.screen_size += padding;
				}
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON
				mp_entry[tmp_index].mp_info        = 0x40010001;	// Moniter ScreenNail (VGA)
				if (tmp_index == 1) {
					mp_entry[tmp_index].mp_data_offset = app2_length - 8 + jpg_data->ext_mp[0].main_size - 2;
				}
				else {
					mp_entry[tmp_index].mp_data_offset = mp_entry[tmp_index - 1].mp_data_offset + mp_entry[tmp_index - 1].mp_data_size;
				}
				mp_entry[tmp_index].mp_data_size   = jpg_data->ext_mp[ext_cnt].screen_data1.screen_size + scr1_app1_size - 2 + jpg_data->ext_mp[ext_cnt].screen_data1.thumb_size;
				mp_entry[tmp_index].mp_entry_1 = 0x0000;
				mp_entry[tmp_index].mp_entry_2 = 0x0000;
				tmp_index++;
				if ( tmp_index >= 6 ) {
					return FSDCF_ERRNO_PARAM_FAILED;
				}

			}
			if ((jpg_data->ext_mp[ext_cnt].screen_data2.screen_addr != 0) && (jpg_data->ext_mp[ext_cnt].screen_data2.screen_size != 0)) {
				mp_entry[main_index].mp_info |= 0x80000000;            // Dependent Parent Image Flag
				mp_entry[main_index].mp_entry_2 = tmp_index+1;
				
				BF_Dcf_Exif_Make_Exif_Mpg(0, &jpg_data->ext_mp[ext_cnt].screen_data2);
				if (TRUE != FJ_Host_ExifUpdate_Class1_CB()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
					BF_Fs_User_Custom_Wrn_Printer(("FJ_Host_ExifUpdate_Class1_CB False\n"));
#endif
					BF_Mdf_Wrapper_Close(file_id);
					BF_Mdf_Wrapper_Remove(make_filepath);
					return FSDCF_ERRNO_PARAM_FAILED;
				}
				BF_Dcf_Exif_Get_App1_Size_Mpf(0, &scr2_app1_size);

#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
				if ((jpg_data->ext_mp[ext_cnt].screen_data2.thumb_addr != 0) && (jpg_data->ext_mp[ext_cnt].screen_data2.thumb_size != 0)) {
					padding = (jpg_data->ext_mp[ext_cnt].screen_data2.thumb_size % 4);
					if (padding == 0) {
						padding = 2;
					}
					else if (padding == 2) {
						padding = 0;
					}
					jpg_data->ext_mp[ext_cnt].screen_data2.thumb_size += padding;

					padding = (4 - (scr2_app1_size % 4));
					if (padding == 4) {
						padding = 0;
					}
				}
				else {
					padding = (scr2_app1_size % 4);
					if (padding == 0) {
						padding = 2;
					}
					else if (padding == 2) {
						padding = 0;
					}
					else {
						; // DO NOTHING
					}
				}

				scr2_app1_size += padding;

 				if (tmp_index < (mp_entry_num - 1) ) {
					padding = (4 - (jpg_data->ext_mp[ext_cnt].screen_data2.screen_size % 4));
					if (padding == 4) {
						padding = 0;
					}
					jpg_data->ext_mp[ext_cnt].screen_data2.screen_size += padding;
				}
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON

				mp_entry[tmp_index].mp_info        = 0x40010002;	// Moniter ScreenNail (Full HD)
				mp_entry[tmp_index].mp_data_size   = jpg_data->ext_mp[ext_cnt].screen_data2.screen_size + scr2_app1_size - 2 + jpg_data->ext_mp[ext_cnt].screen_data2.thumb_size;
				if (tmp_index == 1) {
					mp_entry[tmp_index].mp_data_offset = app2_length - 8 + jpg_data->ext_mp[0].main_size - 2;
				}
				else {
					mp_entry[tmp_index].mp_data_offset = mp_entry[tmp_index - 1].mp_data_offset + mp_entry[tmp_index - 1].mp_data_size;
				}
				mp_entry[tmp_index].mp_entry_1 = 0x0000;
				mp_entry[tmp_index].mp_entry_2 = 0x0000;
				tmp_index++;
				if ( tmp_index >= 6 ) {
					return FSDCF_ERRNO_PARAM_FAILED;
				}
			}
		}
	}

#if 1
	for (tmp_index = 0; tmp_index < mp_entry_num; tmp_index++) {
		BF_Dcf_Exif_Exchange_Endian_Mpf_Entry(BF_Dcf_Exif_Get_Exif_Endian(), &mp_entry[tmp_index]);
	}
#endif
	memcpy((tmp_buff + mp_entry_offset), &mp_entry[0], (16 * mp_entry_num));
#else
	mp_entry_offset += (jpg_head_length + jpg_data->thumb_size);
#endif

	// write APP2 (MPF Index IFD)
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)tmp_buff, app2_length, &length);
	if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

	ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}
#endif	// CO_BF_DCF_IF_MPF_ENABLE

	// write main data
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)(jpg_data->ext_mp[0].main_addr + 2), (jpg_data->ext_mp[0].main_size - 2), &length);
	if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(make_filepath);
		return ercd;
	}

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	for (ext_cnt = 0; ext_cnt<jpg_data->ext_mp_num; ext_cnt++) {
		if ((jpg_data->ext_mp[ext_cnt].main_addr!= 0) && (jpg_data->ext_mp[ext_cnt].main_size != 0)) {
			if (ext_cnt != 0) {
#if 1
				// make exif format
				BF_Dcf_Exif_Make_Ext(&jpg_data->ext_mp[ext_cnt]);
				
				// make exif format from HOST
				if (TRUE != FJ_Host_ExifUpdateCB()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
					BF_Fs_User_Custom_Wrn_Printer(("FJ_Host_ExifUpdateCB False\n"));
#endif
					BF_Mdf_Wrapper_Close(file_id);
					BF_Mdf_Wrapper_Remove(make_filepath);
					return FSDCF_ERRNO_PARAM_FAILED;
				}
#endif
				ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
				if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
					BF_Mdf_Wrapper_Close(file_id);
					BF_Mdf_Wrapper_Remove(make_filepath);
					return ercd;
				}
				BF_Dcf_Exif_Copy_Exif_Jpeg(tmp_buff, &jpg_head_length);
				
				app1_size = jpg_head_length + jpg_data->ext_mp[ext_cnt].thumb_size - 4;
				memcpy((FW_VOID*)(tmp_buff + 4), (FW_VOID*)&app1_size, 2);

				// write exif data
				ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)tmp_buff, jpg_head_length, &length);
				
				if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
					BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
					BF_Mdf_Wrapper_Close(file_id);
					BF_Mdf_Wrapper_Remove(make_filepath);
					return ercd;
				}

				ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
				if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
					BF_Mdf_Wrapper_Close(file_id);
					BF_Mdf_Wrapper_Remove(make_filepath);
					return ercd;
				}

				// write thumbnail data
				ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)jpg_data->ext_mp[ext_cnt].thumb_addr, jpg_data->ext_mp[ext_cnt].thumb_size, &length);
				if (ercd != 0) {
					BF_Mdf_Wrapper_Close(file_id);
					BF_Mdf_Wrapper_Remove(make_filepath);
					return ercd;
				}

				ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
				if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
					BF_Mdf_Wrapper_Close(file_id);
					BF_Mdf_Wrapper_Remove(make_filepath);
					return ercd;
				}
#if 1
				BF_Dcf_Exif_Copy_Extended_App2_Mpf(tmp_buff, &app2_length, BF_Dcf_Exif_Get_Exif_Endian());
#endif
#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
				padding = (4 - (ext_app2_length % 4));
				if (padding == 4) {
					padding = 0;
				}
				ext_app2_length += padding;
#endif
				// write APP2 (MPF Index IFD)
				ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)tmp_buff, app2_length, &length);
				if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
					BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
					BF_Mdf_Wrapper_Close(file_id);
					BF_Mdf_Wrapper_Remove(make_filepath);
					return ercd;
				}

				ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
				if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
					BF_Mdf_Wrapper_Close(file_id);
					BF_Mdf_Wrapper_Remove(make_filepath);
					return ercd;
				}
				
				// write main data
				ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)(jpg_data->ext_mp[ext_cnt].main_addr + 2), (jpg_data->ext_mp[ext_cnt].main_size - 2), &length);
				if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
					BF_Mdf_Wrapper_Close(file_id);
					BF_Mdf_Wrapper_Remove(make_filepath);
					return ercd;
				}
			}
		}

		// make APP1 (Exif for ScreenNail -1)
		if ((jpg_data->ext_mp[ext_cnt].screen_data1.screen_addr != 0) && (jpg_data->ext_mp[ext_cnt].screen_data1.screen_size != 0)) {
			BF_Dcf_Exif_Make_Exif_Mpg(0, &jpg_data->ext_mp[ext_cnt].screen_data1);
			
			ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
			if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
				BF_Mdf_Wrapper_Close(file_id);
				BF_Mdf_Wrapper_Remove(make_filepath);
				return ercd;
			}
#if 1
			BF_Dcf_Exif_Copy_Exif_Mpf(0, BF_Dcf_Exif_Get_Exif_Endian(), tmp_buff, &mpf_head_length);
#endif
#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
			if (jpg_data->ext_mp[ext_cnt].screen_data1.thumb_size != 0) {
				padding = (4 - (mpf_head_length % 4));
				if (padding == 4) {
					padding = 0;
				}
			}
			else {
				padding = (mpf_head_length % 4);
				if (padding == 0) {
					padding = 2;
				}
				else if (padding == 2) {
					padding = 0;
				}
				else {
					; // DO NOTHING
				}
			}
			mpf_head_length += padding;
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON

			scr1_app1_size = mpf_head_length + jpg_data->ext_mp[ext_cnt].screen_data1.thumb_size - 4;
			scr1_app1_size = BF_Dcf_Exif_Exchange_App_Size(scr1_app1_size);
			memcpy((FW_VOID*)(tmp_buff + 4), (FW_VOID*)&scr1_app1_size, 2);

			// write APP1 (Exif for ScreenNail -1)
			ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)tmp_buff, mpf_head_length, &length);
			if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
				BF_Mdf_Wrapper_Close(file_id);
				BF_Mdf_Wrapper_Remove(make_filepath);
				return ercd;
			}

			ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
				BF_Mdf_Wrapper_Close(file_id);
				BF_Mdf_Wrapper_Remove(make_filepath);
				return ercd;
			}

			if ((jpg_data->ext_mp[ext_cnt].screen_data1.thumb_addr != 0) && (jpg_data->ext_mp[ext_cnt].screen_data1.thumb_size != 0)) {
				// write ThumbNail for ScreenNail -1
				ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)jpg_data->ext_mp[ext_cnt].screen_data1.thumb_addr, jpg_data->ext_mp[ext_cnt].screen_data1.thumb_size, &length);
				if (ercd != 0) {
					BF_Mdf_Wrapper_Close(file_id);
					BF_Mdf_Wrapper_Remove(make_filepath);
					return ercd;
				}
			}

			// write ScreenNail -1 data
			ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)(jpg_data->ext_mp[ext_cnt].screen_data1.screen_addr + 2), (jpg_data->ext_mp[ext_cnt].screen_data1.screen_size - 2), &length);
			if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				BF_Mdf_Wrapper_Close(file_id);
				BF_Mdf_Wrapper_Remove(make_filepath);
				return ercd;
			}
		}


		if ((jpg_data->ext_mp[ext_cnt].screen_data2.screen_addr != 0) && (jpg_data->ext_mp[ext_cnt].screen_data2.screen_size != 0)) {
			// make APP1 (Exif for ScreenNail -2)
			BF_Dcf_Exif_Make_Exif_Mpg(1, &jpg_data->ext_mp[ext_cnt].screen_data2);

			ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
			if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
				BF_Mdf_Wrapper_Close(file_id);
				BF_Mdf_Wrapper_Remove(make_filepath);
				return ercd;
			}

#if 1
	//		BF_Dcf_Exif_Copy_Exif_Mpf(1, D_BF_DCF_EXIF_BIG_EN, tmp_buff, &mpf_head_length);
			BF_Dcf_Exif_Copy_Exif_Mpf(1, BF_Dcf_Exif_Get_Exif_Endian(), tmp_buff, &mpf_head_length);
#else
			BF_Dcf_Exif_Copy_Exif_Mpf(1, tmp_buff, &mpf_head_length);
#endif
#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
			if (jpg_data->ext_mp[ext_cnt].screen_data2.thumb_size != 0) {
				padding = (4 - (mpf_head_length % 4));
				if (padding == 4) {
					padding = 0;
				}
			}
			else {
				padding = (mpf_head_length % 4);
				if (padding == 0) {
					padding = 2;
				}
				else if (padding == 2) {
					padding = 0;
				}
				else {
					; // DO NOTHING
				}
			}
			mpf_head_length += padding;
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON

			scr2_app1_size = mpf_head_length + jpg_data->ext_mp[ext_cnt].screen_data2.thumb_size - 4;
			scr2_app1_size = BF_Dcf_Exif_Exchange_App_Size(scr2_app1_size);
			memcpy((FW_VOID*)(tmp_buff + 4), (FW_VOID*)&scr2_app1_size, 2);

			// write APP1 (Exif for ScreenNail -2)
			ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)tmp_buff, mpf_head_length, &length);
			if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
				BF_Mdf_Wrapper_Close(file_id);
				BF_Mdf_Wrapper_Remove(make_filepath);
				return ercd;
			}

			ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
				BF_Mdf_Wrapper_Close(file_id);
				BF_Mdf_Wrapper_Remove(make_filepath);
				return ercd;
			}

			if ((jpg_data->ext_mp[ext_cnt].screen_data2.thumb_addr != 0) && (jpg_data->ext_mp[ext_cnt].screen_data2.thumb_size != 0)) {
				// write ThumbNail for ScreenNail -2
				ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)jpg_data->ext_mp[ext_cnt].screen_data2.thumb_addr, jpg_data->ext_mp[ext_cnt].screen_data2.thumb_size, &length);
				if (ercd != 0) {
					BF_Mdf_Wrapper_Close(file_id);
					BF_Mdf_Wrapper_Remove(make_filepath);
					return ercd;
				}
			}

			// write ScreenNail -2 data
			ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)(jpg_data->ext_mp[ext_cnt].screen_data2.screen_addr + 2), (jpg_data->ext_mp[ext_cnt].screen_data2.screen_size - 2), &length);
			if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				BF_Mdf_Wrapper_Close(file_id);
				BF_Mdf_Wrapper_Remove(make_filepath);
				return ercd;
			}
		}
	}
	
	BF_Dcf_Exif_Reset_App2_Mpf();
	BF_Dcf_Exif_Reset_Extended_First_App2_Mpf();
	BF_Dcf_Exif_Reset_Extended_App2_Mpf();
	BF_Dcf_Exif_Reset_App1_Mpf_Resize(0);
	BF_Dcf_Exif_Reset_App1_Mpf_Resize(1);	
#endif    // CO_BF_DCF_IF_MPF_ENABLE

	ercd = dcf_controller_close_new_file(file_id, make_filepath, E_BF_DCF_IF_REMOVE_FLG_OFF);

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF JPEG SAVE END\n");
#endif

	return ercd;
}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE


/**
 * @brief save thm.
 * @param i:*jpg_data saved jpeg data
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Save_Thm(T_BF_DCF_IF_THM_DATA* thm_data)
{
	FW_INT32 ercd;
	FW_INT32 file_id;
	FW_USHORT app1_size;
	FW_CHAR* tmp_buff;
	FW_ULONG jpg_head_length;
	FW_ULONG length;
	FW_DOUBLE free_size;

	if(thm_data == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if(thm_data->thumb_addr == 0){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	/* check free space in media */
	ercd = BF_Fs_Controller_Get_FreeSize(BF_Media_State_Get_Used_Media(), &free_size);
	if(ercd != 0){
		return ercd;
	}

	if(free_size <= (thm_data->thumb_size + D_BF_DCF_EXIF_THUM_START_ADD_THM)){

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("FREE SPACE OVER: size=%lf\n", free_size));
#endif
		return FSDCF_ERRNO_FREE_SPACE_OVER;
	}

	/* create file */
	file_id = BF_Mdf_Wrapper_Create(thm_data->thm_file_path, D_BF_MDF_WRAPPER_ATTR_NORMAL);
	if( file_id < 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Create() failed: file_id=%x, path=%s\n", file_id, thm_data->thm_file_path));
#endif
		return file_id;
	}

	/* make exif format */
	BF_Dcf_Exif_Make_Thm(thm_data);

	/* make exif format */
	if( TRUE != FJ_Host_ExifUpdateCB() ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("FJ_ExiffUpdateCB False\n"));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(thm_data->thm_file_path);
		return FSDCF_ERRNO_PARAM_FAILED;
	}

	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("Fsu_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(thm_data->thm_file_path);
		return ercd;
	}

	BF_Dcf_Exif_Copy_Exif_Thm(tmp_buff, &jpg_head_length);

	app1_size = jpg_head_length - 4;
	app1_size = BF_Dcf_Exif_Exchange_App_Size(app1_size);
	memcpy((FW_VOID*)(tmp_buff + 4), (FW_VOID*)&app1_size, 2);

	/* write exif data */
	ercd = BF_Mdf_Wrapper_Write(file_id, (const FW_VOID*)tmp_buff, jpg_head_length, &length);
	if( ercd != 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(thm_data->thm_file_path);
		return ercd;
	}

	ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(thm_data->thm_file_path);
		return ercd;
	}


	/* lseek and write thumbnail data */
	ercd = dcf_controller_w_lseek_write(file_id, jpg_head_length, (thm_data->thumb_addr + 2), (thm_data->thumb_size - 2), &length);
	if( ercd != 0 ){
		BF_Mdf_Wrapper_Close(file_id);
		BF_Mdf_Wrapper_Remove(thm_data->thm_file_path);
		return ercd;
	}


	BF_Mdf_Wrapper_Close(file_id);

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF THM SAVE END\n");
#endif

	return ercd;
}

/**
 * @brief save resize image.
 * @param i:*resize_param resize parameter
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Save_Resize(T_BF_DCF_IF_RESIZE_PARAM* resize_param)
{
	FW_CHAR                org_filepath[64];
	FW_INT32               org_file_id;
	FW_ULONG               result = 0;
	FW_CHAR                ext[4];
	FW_INT32               new_file_id = 0;
	FW_CHAR                new_filepath[64];
	T_BF_DCF_EXIF_IMAGE_SIZE_INFO exif_image_size_info;
	FW_USHORT                open_mode = D_BF_MDF_WRAPPER_O_RDONLY;
	FW_CHAR                date_time[20] = "    :  :     :  :  " ;
	FW_DOUBLE              free_size;
#ifdef CO_BF_DCF_IF_MPF_ENABLE
	FW_UCHAR               mpf_flag;
	FW_USHORT              mpf_index;
	FW_USHORT              class1_app1_size = 0;
	FW_USHORT              class2_app1_size = 0;
	T_BF_DCF_IF_SCREEN_DATA      monitor_data[2];
	FW_ULONG               mpf_head_length;
	FW_ULONG               app2_length;
	FW_ULONG               mp_entry_offset;
	FW_ULONG               mp_entry_num;
	T_BF_DCF_EXIF_MP_ENTRY         mp_entry[3];
	FW_ULONG               tmp_index;
	FW_CHAR*               tmp_buff;
	FW_ULONG               cls1_src_file_app1_size;
	FW_ULONG               cls2_src_file_app1_size;
#endif	// CO_BF_DCF_IF_MPF_ENABLE
	FW_UCHAR attr_bak;


	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Dcf_Controller_Close_Display_File();
	}

	if (resize_param == NULL) {
		// parameter error
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

#ifndef CO_BF_FS_USER_CUSTOM_READ_ONLY_EDITABLE
	if (resize_param->create_flg != E_BF_DCF_IF_CREATE_NEW_FILE) {
		open_mode = D_BF_MDF_WRAPPER_O_RDWR;
	}
#endif

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	// get target filepath
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(resize_param->file_index, org_filepath, NULL);
	if (g_bf_dcf_controller_ret_err != 0) {
		// filepath get error
		return g_bf_dcf_controller_ret_err;
	}

	// original file open
	org_file_id = BF_Mdf_Wrapper_Open(org_filepath, open_mode, D_BF_MDF_WRAPPER_ATTR_NORMAL);
	if (org_file_id == 0) {
		// file open error
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", org_file_id, org_filepath));
#endif
		g_bf_dcf_controller_ret_err = org_file_id;
		return g_bf_dcf_controller_ret_err;
	}

	memset( &exif_image_size_info, 0, sizeof(T_BF_DCF_EXIF_IMAGE_SIZE_INFO) );

	exif_image_size_info.x_size_value       = resize_param->width;
	exif_image_size_info.y_size_value       = resize_param->lines;
	exif_image_size_info.thumb_length_value = resize_param->thumb_size;
	// get date charactor
	BF_Dcf_Exif_Exchange_Datetime(resize_param->date, (FW_UCHAR*)date_time);
	strcpy(exif_image_size_info.edit_date_value, date_time);

	// get exif image size informaion
	g_bf_dcf_controller_ret_err = BF_Dcf_Exif_Analyze_Jpeg_Resize(org_file_id, resize_param->copy_flag, &exif_image_size_info);
	if (g_bf_dcf_controller_ret_err != D_BF_FS_USER_CUSTOM_OK) {
		// Exif analyze error
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Dcf_Exif_Analyze_Jpeg_Resize() failed: ercd=%d\n", g_bf_dcf_controller_ret_err));
#endif
		BF_Mdf_Wrapper_Close(org_file_id);
		BF_Dcf_Exif_Reset_Jpeg_Resize();
		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_DCF_IF_MPF_ENABLE
	mpf_flag = 0;
	if (exif_image_size_info.class1_offset != 0) {
		// set calss1 exist flag from original image
		mpf_flag |= 0x01;
	}
	if (exif_image_size_info.class2_offset != 0) {
		// set calss2 exist flag from original image
		mpf_flag |= 0x02;
	}
#endif	// CO_BF_DCF_IF_MPF_ENABLE

	// make exif format
	if (TRUE != FJ_Host_ExifUpdateCB()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("FJ_Host_ExifUpdateCB False\n"));
#endif
		BF_Mdf_Wrapper_Close(org_file_id);
		BF_Dcf_Exif_Reset_Jpeg_Resize();
		return FSDCF_ERRNO_PARAM_FAILED;
	}

#ifdef CO_BF_DCF_IF_MPF_ENABLE
	// class1 image info
	if ((resize_param->class1_size != 0) && (resize_param->class1_addr != 0)) {
		monitor_data[0].screen_width = resize_param->class1_width;
		monitor_data[0].screen_lines = resize_param->class1_lines;
		monitor_data[0].thumb_addr   = resize_param->class1_thumb_addr;
		monitor_data[0].thumb_size   = resize_param->class1_thumb_size;
	}
	// class2 image info
	if ((resize_param->class2_size != 0) && (resize_param->class2_addr != 0)) {
		monitor_data[1].screen_width = resize_param->class2_width;
		monitor_data[1].screen_lines = resize_param->class2_lines;
		monitor_data[1].thumb_addr   = resize_param->class2_thumb_addr;
		monitor_data[1].thumb_size   = resize_param->class2_thumb_size;
	}

	for (mpf_index = 0; mpf_index < 2; mpf_index++) {
		if (mpf_index == 0) {
			if ((resize_param->class1_size == 0) || (resize_param->class1_addr == 0)) {
				// clear calss1 exist flag from new image
				mpf_flag &= ~(0x01 << mpf_index);
				continue;
			}
		}
		else {
			if ((resize_param->class2_size == 0) || (resize_param->class2_addr == 0)) {
				// clear calss2 exist flag from new image
				mpf_flag &= ~(0x01 << mpf_index);
				continue;
			}
		}

		if ((mpf_flag & (0x01 << mpf_index)) != 0) {
			// get exif information (for Monitor)
			if (mpf_index == 0) {
				// prepair exif class1 for global buffer
				g_bf_dcf_controller_ret_err = BF_Dcf_Exif_Analyze_Mpf_Resize(mpf_index, org_file_id, exif_image_size_info.class1_offset);
			}
			else {
				// prepair exif class2 for global buffer
				g_bf_dcf_controller_ret_err = BF_Dcf_Exif_Analyze_Mpf_Resize(mpf_index, org_file_id, exif_image_size_info.class2_offset);
			}

			if (g_bf_dcf_controller_ret_err != D_BF_FS_USER_CUSTOM_OK) {
				// error
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Dcf_Exif_Analyze_Mpf_Resize() failed: ercd=%d\n", g_bf_dcf_controller_ret_err));
#endif
				BF_Mdf_Wrapper_Close(org_file_id);
				BF_Dcf_Exif_Reset_Jpeg_Resize();
				BF_Dcf_Exif_Reset_App1_Mpf(0);
				BF_Dcf_Exif_Reset_App1_Mpf(1);
				return g_bf_dcf_controller_ret_err;
			}
		}
		else {
			mpf_flag |= (0x01 << mpf_index);
		}

		BF_Dcf_Exif_Make_Exif_Mpg(mpf_index, &monitor_data[mpf_index]);

		if (mpf_index == 0) {
			if (TRUE != FJ_Host_ExifUpdate_Class1_CB()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("FJ_Host_ExifUpdate_Class1_CB False\n"));
#endif
				BF_Mdf_Wrapper_Close(org_file_id);
				BF_Dcf_Exif_Reset_Jpeg_Resize();
				BF_Dcf_Exif_Reset_App1_Mpf(0);
				BF_Dcf_Exif_Reset_App1_Mpf(1);
				return FSDCF_ERRNO_PARAM_FAILED;
			}
			BF_Dcf_Exif_Get_App1_Size_Mpf(mpf_index, &class1_app1_size);
		}
		else {
			if (TRUE != FJ_Host_ExifUpdate_Class2_CB()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("FJ_Host_ExifUpdate_Class2_CB False\n"));
#endif
				BF_Mdf_Wrapper_Close(org_file_id);
				BF_Dcf_Exif_Reset_Jpeg_Resize();
				BF_Dcf_Exif_Reset_App1_Mpf(0);
				BF_Dcf_Exif_Reset_App1_Mpf(1);
				return FSDCF_ERRNO_PARAM_FAILED;
			}
			BF_Dcf_Exif_Get_App1_Size_Mpf(mpf_index, &class2_app1_size);
		}
	}
#endif	// CO_BF_DCF_IF_MPF_ENABLE

	// check free space in media
	g_bf_dcf_controller_ret_err = BF_Fs_Controller_Get_FreeSize(BF_Media_State_Get_Used_Media(), &free_size);
	if (g_bf_dcf_controller_ret_err != 0) {
		BF_Mdf_Wrapper_Close(org_file_id);
		BF_Dcf_Exif_Reset_Jpeg_Resize();
		return g_bf_dcf_controller_ret_err;
	}

	if (resize_param->copy_flag) {
		if (free_size <= resize_param->jpeg_size) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("FREE SPACE OVER: size=%lf\n", free_size));
#endif
			BF_Mdf_Wrapper_Close(org_file_id);
			BF_Dcf_Exif_Reset_Jpeg_Resize();
			g_bf_dcf_controller_ret_err = FSDCF_ERRNO_FREE_SPACE_OVER;
			return g_bf_dcf_controller_ret_err;
		}
	}
	else {
		if (free_size <= (exif_image_size_info.thumb_offset + resize_param->thumb_size + (resize_param->main_size - 2))) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("FREE SPACE OVER: size=%lf\n", free_size));
#endif
			BF_Mdf_Wrapper_Close(org_file_id);
			BF_Dcf_Exif_Reset_Jpeg_Resize();
			g_bf_dcf_controller_ret_err = FSDCF_ERRNO_FREE_SPACE_OVER;
			return g_bf_dcf_controller_ret_err;
		}
	}

	// get capital charactor
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Change_File_Type_To_Char(resize_param->file_type, ext);
	if (g_bf_dcf_controller_ret_err != 0) {
		BF_Mdf_Wrapper_Close(org_file_id);
		BF_Dcf_Exif_Reset_Jpeg_Resize();
		return g_bf_dcf_controller_ret_err;
	}

	// create new file
	g_bf_dcf_controller_ret_err = dcf_controller_create_new_file(ext, &new_file_id, new_filepath);
	if (g_bf_dcf_controller_ret_err != 0) {
		BF_Mdf_Wrapper_Close(org_file_id);
		BF_Dcf_Exif_Reset_Jpeg_Resize();
		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = BF_Dcf_Exif_Copy_Jpeg_Resize(org_file_id, new_file_id, &exif_image_size_info);
	if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Dcf_Exif_Copy_Jpeg_Resize() failed: ercd=%d\n", g_bf_dcf_controller_ret_err));
#endif
		BF_Mdf_Wrapper_Close(org_file_id);
		BF_Mdf_Wrapper_Close(new_file_id);
		BF_Mdf_Wrapper_Remove(new_filepath);
		BF_Dcf_Exif_Reset_Jpeg_Resize();
		return g_bf_dcf_controller_ret_err;
	}

	// Exif Tag Value reset (for capture)
	BF_Dcf_Exif_Reset_Jpeg_Resize();

	// original file close
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Close(org_file_id);
	if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Close() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, org_file_id));
#endif
		BF_Mdf_Wrapper_Close(new_file_id);
		BF_Mdf_Wrapper_Remove(new_filepath);
		return g_bf_dcf_controller_ret_err;
	}

	// reset new file pointer
	if (exif_image_size_info.thumb_offset == 0) {
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Lseek(new_file_id, (exif_image_size_info.field_length_value + D_BF_DCF_EXIF_SOI_SIZE), D_BF_MDF_WRAPPER_SEEK_SET, &result);
	}
	else {
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Lseek(new_file_id, exif_image_size_info.thumb_offset, D_BF_MDF_WRAPPER_SEEK_SET, &result);
	}
	if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, new_file_id));
#endif
		BF_Mdf_Wrapper_Close(new_file_id);
		BF_Mdf_Wrapper_Remove(new_filepath);
		return g_bf_dcf_controller_ret_err;
	}

	// check thumb_offset
	if (exif_image_size_info.thumb_offset < exif_image_size_info.zero_orient_offset) {
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_SUPPORT_FILE;
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("Not Support File failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, new_file_id));
#endif
		BF_Mdf_Wrapper_Close(new_file_id);
		BF_Mdf_Wrapper_Remove(new_filepath);
		return g_bf_dcf_controller_ret_err;
	}

	// write thumbnail data
	if (exif_image_size_info.thumb_offset != 0) {
		if ((resize_param->thumb_addr == 0) && (resize_param->copy_flag != 0)) {
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Write(new_file_id, (const FW_VOID*)(resize_param->jpeg_addr + exif_image_size_info.thumb_offset), (exif_image_size_info.field_length_value + D_BF_DCF_EXIF_SOI_SIZE - exif_image_size_info.thumb_offset), &result);
		}
		else {
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Write(new_file_id, (const FW_VOID*)(resize_param->thumb_addr), resize_param->thumb_size, &result);
		}
		if (g_bf_dcf_controller_ret_err != 0) {
			BF_Mdf_Wrapper_Close(new_file_id);
			BF_Mdf_Wrapper_Remove(new_filepath);
			return g_bf_dcf_controller_ret_err;
		}

		if (resize_param->copy_flag != 0) {
			// seek APP1 end position
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Lseek(new_file_id, (exif_image_size_info.field_length_value + D_BF_DCF_EXIF_SOI_SIZE), D_BF_MDF_WRAPPER_SEEK_SET, &result);
			if (g_bf_dcf_controller_ret_err != 0) {
				BF_Mdf_Wrapper_Close(new_file_id);
				BF_Mdf_Wrapper_Remove(new_filepath);
				return g_bf_dcf_controller_ret_err;
			}
		}
	}

	if (resize_param->copy_flag != 0) {
		resize_param->main_size = resize_param->jpeg_size - (exif_image_size_info.field_length_value + D_BF_DCF_EXIF_SOI_SIZE) + 2 - exif_image_size_info.app2_size;
		resize_param->main_addr = resize_param->jpeg_addr + (exif_image_size_info.field_length_value + D_BF_DCF_EXIF_SOI_SIZE) - 2 + exif_image_size_info.app2_size;
	}

#ifdef CO_BF_DCF_IF_MPF_ENABLE
	// make APP2 (MPF)
	mp_entry_num = 1;
	if ((mpf_flag & 0x01) != 0) {
		mp_entry_num++;
	}
	if ((mpf_flag & 0x02) != 0) {
		mp_entry_num++;
	}

	g_bf_dcf_controller_ret_err = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
	if (g_bf_dcf_controller_ret_err != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", g_bf_dcf_controller_ret_err));
#endif
		BF_Mdf_Wrapper_Close(new_file_id);
		BF_Mdf_Wrapper_Remove(new_filepath);
		return g_bf_dcf_controller_ret_err;
	}

	BF_Dcf_Exif_Copy_App2_Mpf(tmp_buff, &app2_length, &mp_entry_offset, mp_entry_num, exif_image_size_info.endian);

	// Make MP Entry data
	mp_entry[0].mp_info        = 0xA0030000;		// Baseline Main Image
	mp_entry[0].mp_data_size   = resize_param->main_size + app2_length + exif_image_size_info.field_length_value + 2;
	mp_entry[0].mp_data_offset = 0x00000000;
	if (mp_entry_num == 1) {
		mp_entry[0].mp_info    = 0x20030000;		// Baseline Main Image (No Dependent Image)
		mp_entry[0].mp_entry_1 = 0x0000;
		mp_entry[0].mp_entry_2 = 0x0000;
	}
	if (mp_entry_num == 2) {
		mp_entry[0].mp_entry_1 = 0x0002;
		mp_entry[0].mp_entry_2 = 0x0000;
	}
	if (mp_entry_num == 3) {
		mp_entry[0].mp_entry_1 = 0x0002;
		mp_entry[0].mp_entry_2 = 0x0003;
	}

	tmp_index = 1;
	cls1_src_file_app1_size = 0;
	if ((mpf_flag & 0x01) != 0) {
		if(resize_param->copy_flag == 1){
			// Get app1 size(monitor1) from source file
			g_bf_dcf_controller_ret_err = BF_Dcf_Exif_Get_App1_Size_Mpf_For_Buf((FW_UCHAR*)resize_param->class1_addr, &cls1_src_file_app1_size);
			if(g_bf_dcf_controller_ret_err != 0){
				// not find app1
				cls1_src_file_app1_size = 0;
			}
			else{
				// add app1 maker(0xFFE1) size
				cls1_src_file_app1_size += 2;
			}
		}

		mp_entry[tmp_index].mp_info        = 0x40010001;	// Moniter ScreenNail (VGA)
		mp_entry[tmp_index].mp_data_size   = resize_param->class1_size - cls1_src_file_app1_size + class1_app1_size - 2 + resize_param->class1_thumb_size;
		mp_entry[tmp_index].mp_data_offset = app2_length - 8 + resize_param->main_size - 2;
		mp_entry[tmp_index].mp_entry_1 = 0x0000;
		mp_entry[tmp_index].mp_entry_2 = 0x0000;
		tmp_index++;
	}

	cls2_src_file_app1_size = 0;
	if ((mpf_flag & 0x02) != 0) {
		if(resize_param->copy_flag == 1){
			// Get app1 size(monitor2) from source file
			g_bf_dcf_controller_ret_err = BF_Dcf_Exif_Get_App1_Size_Mpf_For_Buf((FW_UCHAR*)resize_param->class2_addr, &cls2_src_file_app1_size);
			if(g_bf_dcf_controller_ret_err != 0){
				// not find app1
				cls2_src_file_app1_size = 0;
			}
			else{
				// add app1 maker(0xFFE1) size
				cls2_src_file_app1_size += 2;
			}
		}

		mp_entry[tmp_index].mp_info        = 0x40010002;	// Moniter ScreenNail (Full HD)
		mp_entry[tmp_index].mp_data_size   = resize_param->class2_size - cls2_src_file_app1_size + class2_app1_size - 2 + resize_param->class2_thumb_size;
		if (tmp_index == 1) {
			mp_entry[tmp_index].mp_data_offset = app2_length - 8 + resize_param->main_size - 2;
		}
		else {
			mp_entry[tmp_index].mp_data_offset = mp_entry[tmp_index - 1].mp_data_offset + mp_entry[tmp_index - 1].mp_data_size;
		}
		mp_entry[tmp_index].mp_entry_1 = 0x0000;
		mp_entry[tmp_index].mp_entry_2 = 0x0000;
	}

#if 1
	for (tmp_index = 0; tmp_index < mp_entry_num; tmp_index++) {
		BF_Dcf_Exif_Exchange_Endian_Mpf_Entry(exif_image_size_info.endian, &mp_entry[tmp_index]);
	}
#endif
	memcpy((tmp_buff + mp_entry_offset), &mp_entry[0], (16 * mp_entry_num));
	BF_Dcf_Exif_Reset_App2_Mpf();

	// write APP2 (MPF Index IFD)
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Write(new_file_id, (const FW_VOID*)tmp_buff, app2_length, &result);
	if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, new_file_id));
#endif
		BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		BF_Mdf_Wrapper_Close(new_file_id);
		BF_Mdf_Wrapper_Remove(new_filepath);
		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
	if (g_bf_dcf_controller_ret_err != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", g_bf_dcf_controller_ret_err));
#endif
		BF_Mdf_Wrapper_Close(new_file_id);
		BF_Mdf_Wrapper_Remove(new_filepath);
		return g_bf_dcf_controller_ret_err;
	}
#endif	// CO_BF_DCF_IF_MPF_ENABLE

	// write main image data
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Write(new_file_id, (const FW_VOID*)(resize_param->main_addr + 2), (resize_param->main_size - 2), &result);
	if (g_bf_dcf_controller_ret_err != 0) {
		BF_Mdf_Wrapper_Close(new_file_id);
		BF_Mdf_Wrapper_Remove(new_filepath);
		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_DCF_IF_MPF_ENABLE
	if ((mpf_flag & 0x01) != 0) {
		// make Exif data (monitor1)
		g_bf_dcf_controller_ret_err = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
		if (g_bf_dcf_controller_ret_err != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", g_bf_dcf_controller_ret_err));
#endif
			BF_Mdf_Wrapper_Close(new_file_id);
			BF_Mdf_Wrapper_Remove(new_filepath);
			return g_bf_dcf_controller_ret_err;
		}

		BF_Dcf_Exif_Copy_Exif_Mpf(0, exif_image_size_info.endian, tmp_buff, &mpf_head_length);
		BF_Dcf_Exif_Reset_App1_Mpf_Resize(0);
		class1_app1_size = mpf_head_length + resize_param->class1_thumb_size - 4;
		class1_app1_size = BF_Dcf_Exif_Exchange_App_Size(class1_app1_size);
		memcpy((FW_VOID*)(tmp_buff + 4), (FW_VOID*)&class1_app1_size, 2);

		// write Exif data (monitor1)
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Write(new_file_id, (const FW_VOID*)tmp_buff, mpf_head_length, &result);
		if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, new_file_id));
#endif
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			BF_Mdf_Wrapper_Close(new_file_id);
			BF_Mdf_Wrapper_Remove(new_filepath);
			return g_bf_dcf_controller_ret_err;
		}

		g_bf_dcf_controller_ret_err = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		if (g_bf_dcf_controller_ret_err != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", g_bf_dcf_controller_ret_err));
#endif
			BF_Mdf_Wrapper_Close(new_file_id);
			BF_Mdf_Wrapper_Remove(new_filepath);
			return g_bf_dcf_controller_ret_err;
		}

		// write ThumbNail data (monitor1)
		if ((resize_param->class1_thumb_size != 0) && (resize_param->class1_thumb_addr != 0)) {
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Write(new_file_id, (const FW_VOID*)resize_param->class1_thumb_addr, resize_param->class1_thumb_size, &result);
			if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, new_file_id));
#endif
				BF_Mdf_Wrapper_Close(new_file_id);
				BF_Mdf_Wrapper_Remove(new_filepath);
				return g_bf_dcf_controller_ret_err;
			}
		}
		// write Image data (monitor1)
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Write(new_file_id, (const FW_VOID*)(resize_param->class1_addr + cls1_src_file_app1_size + 2), (resize_param->class1_size - cls1_src_file_app1_size - 2), &result);
		if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, new_file_id));
#endif
			BF_Mdf_Wrapper_Close(new_file_id);
			BF_Mdf_Wrapper_Remove(new_filepath);
			return g_bf_dcf_controller_ret_err;
		}
	}

	if ((mpf_flag & 0x02) != 0) {
		// make Exif data (monitor2)
		g_bf_dcf_controller_ret_err = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
		if (g_bf_dcf_controller_ret_err != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", g_bf_dcf_controller_ret_err));
#endif
			BF_Mdf_Wrapper_Close(new_file_id);
			BF_Mdf_Wrapper_Remove(new_filepath);
			return g_bf_dcf_controller_ret_err;
		}

		BF_Dcf_Exif_Copy_Exif_Mpf(1, exif_image_size_info.endian, tmp_buff, &mpf_head_length);
		BF_Dcf_Exif_Reset_App1_Mpf_Resize(1);
		class2_app1_size = mpf_head_length + resize_param->class2_thumb_size - 4;
		class2_app1_size = BF_Dcf_Exif_Exchange_App_Size(class2_app1_size);
		memcpy((FW_VOID*)(tmp_buff + 4), (FW_VOID*)&class2_app1_size, 2);

		// write Exif data (monitor2)
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Write(new_file_id, (const FW_VOID*)tmp_buff, mpf_head_length, &result);
		if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, new_file_id));
#endif
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			BF_Mdf_Wrapper_Close(new_file_id);
			BF_Mdf_Wrapper_Remove(new_filepath);
			return g_bf_dcf_controller_ret_err;
		}

		g_bf_dcf_controller_ret_err = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		if (g_bf_dcf_controller_ret_err != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", g_bf_dcf_controller_ret_err));
#endif
			BF_Mdf_Wrapper_Close(new_file_id);
			BF_Mdf_Wrapper_Remove(new_filepath);
			return g_bf_dcf_controller_ret_err;
		}
		// write ThumbNail data (monitor2)
		if ((resize_param->class2_thumb_size != 0) && (resize_param->class2_thumb_addr != 0)) {
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Write(new_file_id, (const FW_VOID*)resize_param->class2_thumb_addr, resize_param->class2_thumb_size, &result);
			if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, new_file_id));
#endif
				BF_Mdf_Wrapper_Close(new_file_id);
				BF_Mdf_Wrapper_Remove(new_filepath);
				return g_bf_dcf_controller_ret_err;
			}
		}
		// write Image data (monitor2)
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Write(new_file_id, (const FW_VOID*)(resize_param->class2_addr + cls2_src_file_app1_size + 2), (resize_param->class2_size - cls2_src_file_app1_size - 2), &result);
		if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, new_file_id));
#endif
			BF_Mdf_Wrapper_Close(new_file_id);
			BF_Mdf_Wrapper_Remove(new_filepath);
			return g_bf_dcf_controller_ret_err;
		}
	}
#endif

	// new file close
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Close(new_file_id);
	if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Close() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, new_file_id));
#endif
		BF_Mdf_Wrapper_Remove(new_filepath);
		return g_bf_dcf_controller_ret_err;
	}

	if (resize_param->create_flg == E_BF_DCF_IF_CREATE_NEW_FILE) {
		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Append_Latest_File(new_filepath);
		if (g_bf_dcf_controller_ret_err != 0) {
			if (BF_Mdf_Wrapper_Remove(new_filepath) != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("BF_Mdf_Wrapper_Remove() failed: path=%s\n", new_filepath));
#endif
			}
			return g_bf_dcf_controller_ret_err;
		}
	}
	else {

		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Get_Attribute(org_filepath, &attr_bak);
		if (g_bf_dcf_controller_ret_err != 0) {
			return g_bf_dcf_controller_ret_err;
		}

		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove(org_filepath);
#ifdef CO_BF_FS_USER_CUSTOM_READ_ONLY_EDITABLE
		if (g_bf_dcf_controller_ret_err == D_BF_MDF_WRAPPER_ERR_ROFS) {
			// change attribute read-only to normal
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Set_Attribute(org_filepath, D_BF_MDF_WRAPPER_ATTR_NORMAL);
			if (g_bf_dcf_controller_ret_err != 0) {
				return g_bf_dcf_controller_ret_err;
			}

			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove(org_filepath);
			if (g_bf_dcf_controller_ret_err != 0) {
				return g_bf_dcf_controller_ret_err;
			}
		}
#endif
		if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, org_filepath));
#endif
			return g_bf_dcf_controller_ret_err;
		}
		else {
			; // DO NOTHING
		}

		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Rename(new_filepath, org_filepath);
		if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Rename() failed: ercd=%d, src_path=%s, dst_path=%s\n", g_bf_dcf_controller_ret_err, new_filepath, org_filepath));
#endif
			return g_bf_dcf_controller_ret_err;
		}

		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Set_Attribute(org_filepath, attr_bak);
		if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Attribute() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, org_filepath));
#endif
			return g_bf_dcf_controller_ret_err;
		}
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF RESIZE END\n");
#endif

	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief updata rotation image.
 * @param i:*rotate_param rotation parameter
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Update_Rotate(T_BF_DCF_IF_ROTATE_PARAM* rotate_param)
{
	FW_CHAR filepath[64];
	FW_CHAR new_filepath[64];
	FW_CHAR ext[4];
	FW_INT32 file_id;
	FW_ULONG result;
#ifdef CO_BF_FS_USER_CUSTOM_READ_ONLY_EDITABLE
	FW_UCHAR attr_flg = 0;
	FW_UCHAR attr_bak;
#endif
	T_BF_DCF_EXIF_ORIENTATION_INFO exif_orient_info;
	FW_USHORT open_mode = D_BF_MDF_WRAPPER_O_RDWR;
//	FW_CHAR date_time[20] = "    :  :     :  :  " ;

	memset( &exif_orient_info, 0, sizeof(T_BF_DCF_EXIF_ORIENTATION_INFO) );

	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Dcf_Controller_Close_Display_File();
	}

	if(rotate_param == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	/* get target filepath */
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(rotate_param->file_index, filepath, NULL);
	if(g_bf_dcf_controller_ret_err != 0) {
		return g_bf_dcf_controller_ret_err;
	}

	if(rotate_param->create_flg == E_BF_DCF_IF_CREATE_NEW_FILE){
		open_mode = D_BF_MDF_WRAPPER_O_RDONLY;

		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Change_File_Type_To_Char((FW_UCHAR)(0xFF & (rotate_param->file_type)), ext);
		if( g_bf_dcf_controller_ret_err != 0 ) {
			return g_bf_dcf_controller_ret_err;
		}

		g_bf_dcf_controller_ret_err = dcf_controller_make_new_file_path_abs(ext, new_filepath);
		if( g_bf_dcf_controller_ret_err != 0 ) {
			return g_bf_dcf_controller_ret_err;
		}

		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Copy(filepath, new_filepath);
		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Copy() failed: ercd=%d, srcname=%s, dstname=%s\n", g_bf_dcf_controller_ret_err, filepath, new_filepath));
#endif
			return g_bf_dcf_controller_ret_err;
		}

		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Append_Latest_File(new_filepath);
		if( g_bf_dcf_controller_ret_err != 0){
			if(BF_Mdf_Wrapper_Remove(new_filepath) != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("BF_Mdf_Wrapper_Remove() failed: path=%s\n", new_filepath));
#endif
			}
			return g_bf_dcf_controller_ret_err;
		}

		strcpy(filepath, new_filepath);
	}

	file_id = BF_Mdf_Wrapper_Open(filepath, open_mode, D_BF_MDF_WRAPPER_ATTR_NORMAL);
#ifdef CO_BF_FS_USER_CUSTOM_READ_ONLY_EDITABLE
	if( file_id == D_BF_MDF_WRAPPER_ERR_ROFS ){
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Get_Attribute(filepath, &attr_bak);
		if (g_bf_dcf_controller_ret_err != 0) {
			return g_bf_dcf_controller_ret_err;
		}

		// change attribute read-only to normal
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Set_Attribute(filepath, D_BF_MDF_WRAPPER_ATTR_NORMAL);
		if(g_bf_dcf_controller_ret_err != 0) {
			return g_bf_dcf_controller_ret_err;
		}
		attr_flg = 1;
		file_id = BF_Mdf_Wrapper_Open(filepath, open_mode, D_BF_MDF_WRAPPER_ATTR_NORMAL);
	}
#endif
	if( file_id == 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", file_id, filepath));
#endif
		g_bf_dcf_controller_ret_err = file_id;
		return g_bf_dcf_controller_ret_err;
	}

	exif_orient_info.orient_value = (FW_USHORT)rotate_param->rotate;
#if 1
	exif_orient_info.first_orient_value = (FW_USHORT)rotate_param->rotate_thumb;
#endif

	if(BF_Dcf_Controller_Is_File_Type("JPG", rotate_param->file_type)) {
		g_bf_dcf_controller_ret_err = BF_Dcf_Exif_Get_Orientation_Offset_Jpeg(file_id, &exif_orient_info);
	}
	else{
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("NOT SUPPORT FILE: file_type=%ld\n", rotate_param->file_type));
#endif
		BF_Mdf_Wrapper_Close(file_id);
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_SUPPORT_FILE;
	}

	if(g_bf_dcf_controller_ret_err != 0){
		BF_Mdf_Wrapper_Close(file_id);
		return g_bf_dcf_controller_ret_err;
	}

	/* updata 0th IFD orientation value */
	if(exif_orient_info.zero_orient_offset != 0){
		g_bf_dcf_controller_ret_err = dcf_controller_w_lseek_write(file_id, exif_orient_info.zero_orient_offset, (FW_ULONG)&exif_orient_info.orient_value, sizeof(FW_USHORT), &result);
		if(g_bf_dcf_controller_ret_err != 0){
			BF_Mdf_Wrapper_Close(file_id);
			return g_bf_dcf_controller_ret_err;
		}
	}
#if 0
	/* get date charactor */
	BF_Dcf_Exif_Exchange_Datetime(rotate_param->date, (FW_UCHAR*)date_time);

	/* updata 0th IFD date value */
	if(exif_orient_info.edit_date_offet != 0){
		g_bf_dcf_controller_ret_err = dcf_controller_w_lseek_write(file_id, exif_orient_info.edit_date_offet, (FW_ULONG)date_time, D_BF_DCF_EXIF_V_DATETIME_S, &result);
		if(g_bf_dcf_controller_ret_err != 0){
			BF_Mdf_Wrapper_Close(file_id);
			return g_bf_dcf_controller_ret_err;
		}
	}
#endif
	/* updata 1st IFD orientation value */
	if(exif_orient_info.first_orient_offet != 0){
#if 0
		g_bf_dcf_controller_ret_err = dcf_controller_w_lseek_write(file_id, exif_orient_info.first_orient_offet, (FW_ULONG)&exif_orient_info.orient_value, sizeof(FW_USHORT), &result);
#else
		g_bf_dcf_controller_ret_err = dcf_controller_w_lseek_write(file_id, exif_orient_info.first_orient_offet, (FW_ULONG)&exif_orient_info.first_orient_value, sizeof(FW_USHORT), &result);
#endif
		if(g_bf_dcf_controller_ret_err != 0){
			BF_Mdf_Wrapper_Close(file_id);
			return g_bf_dcf_controller_ret_err;
		}
	}
#if 0
	/* updata 1st IFD date value */
	if(exif_orient_info.edit_date_offet_thumb != 0){
		g_bf_dcf_controller_ret_err = dcf_controller_w_lseek_write(file_id, exif_orient_info.edit_date_offet_thumb, (FW_ULONG)date_time, D_BF_DCF_EXIF_V_DATETIME_S, &result);
		if(g_bf_dcf_controller_ret_err != 0){
			BF_Mdf_Wrapper_Close(file_id);
			return g_bf_dcf_controller_ret_err;
		}
	}
#endif
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Close(file_id);
	if(g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Close() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, file_id));
#endif
	}

#ifdef CO_BF_FS_USER_CUSTOM_READ_ONLY_EDITABLE
	if(attr_flg == 1){
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Set_Attribute(filepath, attr_bak);
	}
#endif

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF ROTATE SAVE END\n");
#endif

	return g_bf_dcf_controller_ret_err;
}




/**
 * @brief delete one.
 * @param i:*file_index file index
 * @param i:all_flg
 * @return error code
 * @note none
 * @attention none
 */
 #if defined(CO_FS_USED_MDF)
 FW_INT32 BF_Dcf_Controller_Del_Single_Obj(T_BF_DCF_IF_FILE_INDEX* file_index, E_BF_DCF_CONTROLLER_ALL_DELETE_FLAG all_flg)
{
	FW_INT32 ercd;
	FW_CHAR del_filepath[64];
	FW_CHAR del_filepath_opt[64];
	T_BF_DCF_IF_FILE_INFO file_info;
	FW_USHORT dirno = 0;
	FW_USHORT fileno = 0;
	FW_ULONG blksize;
	FW_ULONG blocks;
	FW_ULONG bfree;
	FW_CHAR dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR file_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32 dNo;
	T_BF_FS_IF_Dir_Info dir;
	FW_SHORT objTblIdx;
	FW_CHAR  chk_path[64];
	char *drive_name;
	FW_UINT32 drive_name_len=0;
#if 0	// THM_ext_ON/OFF
	FW_CHAR del_thm_filepath[64];
	FW_CHAR ext_name[4];
#endif
#ifdef CO_BF_DCF_CONTROLLER_SERIES_DIR_RESET
	FW_USHORT tmp_dirno = 0, tmp_fileno = 0;
#endif

	memset( &file_info, 0, sizeof(T_BF_DCF_IF_FILE_INFO) );

	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Dcf_Controller_Close_Display_File();
	}

	if(file_index == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("FS DELETE. index=%d\n", *file_index));
#endif

	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(*file_index, del_filepath, &file_info);
	if(g_bf_dcf_controller_ret_err != 0) {
		return g_bf_dcf_controller_ret_err;
	}

	if(file_info.file_attr == E_BF_DCF_IF_ATTR_RDONLY){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("REMOVE FILE ATTRIBUTE IS READONLY. path=%s\n", del_filepath));
#endif
		g_bf_dcf_controller_ret_err = D_BF_MDF_WRAPPER_ERR_ROFS;
		return g_bf_dcf_controller_ret_err;
	}

	if(file_info.option_file == E_BF_DCF_IF_OPTION_FILE_EXIST){// Voice Memo
		/* delete option file */
		g_bf_dcf_controller_ret_err = dcf_controller_get_index_option_file_path_abs(del_filepath, file_info.file_type, del_filepath_opt);
		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("NOT EXIST OPTION FILE: path=%s\n", del_filepath_opt));
#endif
		}
		else{
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(del_filepath_opt);
			if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, del_filepath_opt));
#endif
			}
		}
	}

#if 0	// THM_ext_ON/OFF
//	/* Delete thumbnail file */
//	BF_Dcf_Db_Change_File_Type_To_Char(file_info.file_type, ext_name);
//	if ((strcmp(ext_name, "MOV") == 0) || (strcmp(ext_name, "MP4") == 0)) {
//		/* delete MOV thumbnail file */
//		g_bf_dcf_controller_ret_err = dcf_controller_make_new_thm_file_path_abs(ext_name, del_filepath, del_thm_filepath);
//		if(g_bf_dcf_controller_ret_err != 0){
//#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
//			BF_Fs_User_Custom_Err_Printer(("NOT EXIST OPTION FILE: path=%s\n", del_thm_filepath));
//#endif
//		}
//		else{
//			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(del_thm_filepath);
//			if(g_bf_dcf_controller_ret_err != 0){
//#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
//				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, del_thm_filepath));
//#endif
//			}
//		}
//		
//		/* delete THM thumbnail file */
//		g_bf_dcf_controller_ret_err = dcf_controller_make_new_thm_file_path_abs("THM", del_filepath, del_thm_filepath);
//		if(g_bf_dcf_controller_ret_err != 0){
//#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
//			BF_Fs_User_Custom_Err_Printer(("NOT EXIST OPTION FILE: path=%s\n", del_thm_filepath));
//#endif
//		}
//		else{
//			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(del_thm_filepath);
//			if(g_bf_dcf_controller_ret_err != 0){
//#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
//				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, del_thm_filepath));
//#endif
//			}
//		}
//	}
#endif	// THM_ext_ON/OFF



	// Check Drive name
	// DriveName Check
	strcpy(chk_path, del_filepath );
	drive_name = strtok(chk_path, ":");
	if (drive_name == NULL) {
		return FSDCF_ERRNO_PARAM_FAILED;
	}
	drive_name_len = strlen(drive_name);
	if ( drive_name_len == 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("Drive name length Err: drive name=%s, length=%d \n", drive_name, drive_name_len));
#endif
		return FSDCF_ERRNO_DRIVE_INIT_FAILED;
	}

//	memcpy(dir_name, del_filepath, 16);
//	dir_name[16] = '\0';
	memcpy(dir_name, del_filepath, (drive_name_len + 15) );
	dir_name[drive_name_len + 15] = '\0';

	ercd = BF_Mdf_Wrapper_Opendir(dir_name, &dNo);
	if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, dir_name));
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
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
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
		if ((dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
			continue;
		}
		objTblIdx = BF_Dcf_Db_Get_Obj_No(dir.fname);
		if(objTblIdx == FSDCF_ERRNO_NOT_SUPPORT_FILENO) {
			continue;
		}

//		if(0 == strncmp(dir.fname + 4, del_filepath + 21, 4)) {
		if(0 == strncmp(dir.fname + 4, del_filepath + (drive_name_len + 20), 4)) {
			strcpy(file_name, dir_name);
			strcat(file_name, "\\");
			strcat(file_name, dir.fname);

			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(file_name);
			if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
#endif
				return g_bf_dcf_controller_ret_err;
			}
		}
	}

	ercd = BF_Mdf_Wrapper_Closedir(dNo);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	if(g_bf_dcf_controller_ret_err == 0){
		if(g_bf_dcf_controller_file_name_setting == E_BF_DCF_IF_FILE_NAME_SERIES){
			dcf_controller_get_last_file(&dirno, &fileno, E_BF_DCF_IF_LATEST_TYPE_ALL);
		}

		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Delete_File(*file_index, del_filepath);

		if(g_bf_dcf_controller_file_name_setting == E_BF_DCF_IF_FILE_NAME_SERIES){
#ifdef CO_BF_DCF_CONTROLLER_SERIES_DIR_RESET
			BF_Dcf_Db_Get_Last_File_In_Db(&tmp_dirno, &tmp_fileno);
			dcf_controller_set_last_file(tmp_dirno, fileno);
#else
			dcf_controller_set_last_file(dirno, fileno);
#endif
		}
		else{
			BF_Dcf_Db_Get_Last_File_In_Db(&dirno, &fileno);
			dcf_controller_set_last_file(dirno, fileno);
		}
	}

	/* updata free space cache, if single delete */
	if( all_flg == E_BF_DCF_CONTROLLER_ALL_DELETE_FLAG_OFF ){
		BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
		BF_Mdf_Wrapper_Get_Free_Space(drive_name, &blksize, &blocks, &bfree);
	}

	return g_bf_dcf_controller_ret_err;
}
 #else
FW_INT32 BF_Dcf_Controller_Del_Single_Obj(T_BF_DCF_IF_FILE_INDEX* file_index, E_BF_DCF_CONTROLLER_ALL_DELETE_FLAG all_flg)
{
	FW_INT32 ercd;
	FW_CHAR del_filepath[64];
	FW_CHAR del_filepath_opt[64];
	T_BF_DCF_IF_FILE_INFO file_info;
	FW_USHORT dirno = 0;
	FW_USHORT fileno = 0;
	FW_ULONG blksize;
	FW_ULONG blocks;
	FW_ULONG bfree;
	FW_CHAR dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR file_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_SHORT objTblIdx;
	FW_CHAR  chk_path[64];
	char *drive_name;
	FW_UINT32 drive_name_len=0;
#if 0	// THM_ext_ON/OFF
	FW_CHAR del_thm_filepath[64];
	FW_CHAR ext_name[4];
#endif
#ifdef CO_BF_DCF_CONTROLLER_SERIES_DIR_RESET
	FW_USHORT tmp_dirno = 0, tmp_fileno = 0;
#endif
	BOOL first_dir_flag = TRUE;
	FW_CHAR file_path[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	T_BF_FS_IF_ENTRY dirent = {0};

	memset( &file_info, 0, sizeof(T_BF_DCF_IF_FILE_INFO) );

	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Dcf_Controller_Close_Display_File();
	}

	if(file_index == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("FS DELETE. index=%d\n", *file_index));
#endif

	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(*file_index, del_filepath, &file_info);
	if(g_bf_dcf_controller_ret_err != 0) {
		return g_bf_dcf_controller_ret_err;
	}

	if(file_info.file_attr == E_BF_DCF_IF_ATTR_RDONLY){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("REMOVE FILE ATTRIBUTE IS READONLY. path=%s\n", del_filepath));
#endif
		g_bf_dcf_controller_ret_err = D_BF_MDF_WRAPPER_ERR_ROFS;
		return g_bf_dcf_controller_ret_err;
	}

	if(file_info.option_file == E_BF_DCF_IF_OPTION_FILE_EXIST){// Voice Memo
		/* delete option file */
		g_bf_dcf_controller_ret_err = dcf_controller_get_index_option_file_path_abs(del_filepath, file_info.file_type, del_filepath_opt);
		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("NOT EXIST OPTION FILE: path=%s\n", del_filepath_opt));
#endif
		}
		else{
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(del_filepath_opt);
			if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, del_filepath_opt));
#endif
			}
		}
	}

#if 0	// THM_ext_ON/OFF
//	/* Delete thumbnail file */
//	BF_Dcf_Db_Change_File_Type_To_Char(file_info.file_type, ext_name);
//	if ((strcmp(ext_name, "MOV") == 0) || (strcmp(ext_name, "MP4") == 0)) {
//		/* delete MOV thumbnail file */
//		g_bf_dcf_controller_ret_err = dcf_controller_make_new_thm_file_path_abs(ext_name, del_filepath, del_thm_filepath);
//		if(g_bf_dcf_controller_ret_err != 0){
//#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
//			BF_Fs_User_Custom_Err_Printer(("NOT EXIST OPTION FILE: path=%s\n", del_thm_filepath));
//#endif
//		}
//		else{
//			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(del_thm_filepath);
//			if(g_bf_dcf_controller_ret_err != 0){
//#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
//				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, del_thm_filepath));
//#endif
//			}
//		}
//		
//		/* delete THM thumbnail file */
//		g_bf_dcf_controller_ret_err = dcf_controller_make_new_thm_file_path_abs("THM", del_filepath, del_thm_filepath);
//		if(g_bf_dcf_controller_ret_err != 0){
//#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
//			BF_Fs_User_Custom_Err_Printer(("NOT EXIST OPTION FILE: path=%s\n", del_thm_filepath));
//#endif
//		}
//		else{
//			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(del_thm_filepath);
//			if(g_bf_dcf_controller_ret_err != 0){
//#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
//				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, del_thm_filepath));
//#endif
//			}
//		}
//	}
#endif	// THM_ext_ON/OFF



	// Check Drive name
	// DriveName Check
	strcpy(chk_path, del_filepath );
	drive_name = strtok(chk_path, ":");
	if (drive_name == NULL) {
		return FSDCF_ERRNO_PARAM_FAILED;
	}
	drive_name_len = strlen(drive_name);
	if ( drive_name_len == 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("Drive name length Err: drive name=%s, length=%d \n", drive_name, drive_name_len));
#endif
		return FSDCF_ERRNO_DRIVE_INIT_FAILED;
	}

//	memcpy(dir_name, del_filepath, 16);
//	dir_name[16] = '\0';
	memcpy(dir_name, del_filepath, (drive_name_len + 15) );
	dir_name[drive_name_len + 15] = '\0';

	ercd = BF_Mdf_Wrapper_Chdir(dir_name);
	if (ercd != FSDCF_ERRNO_OK) {
		return ercd;
	}

	for(;;) {
		if (first_dir_flag) {
			// search first
			strcpy(file_path, "*");
			ercd = BF_Fs_If_Fsfirst(0, (const CHAR*)file_path, D_BF_FS_IF_ATTR_ENTRY, &dirent);
			first_dir_flag = FALSE;
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

		if('.' == dirent.fs_fname[0]) {
			if(0 == dirent.fs_fname[1]) {
				continue;
			}
			if(('.' == dirent.fs_fname[1]) && (0 == dirent.fs_fname[2])) {
				continue;
			}
		}
		if ((dirent.fs_attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
			continue;
		}
		objTblIdx = BF_Dcf_Db_Get_Obj_No(dirent.fs_fname);
		if(objTblIdx == FSDCF_ERRNO_NOT_SUPPORT_FILENO) {
			continue;
		}

//		if(0 == strncmp(dir.fname + 4, del_filepath + 21, 4)) {
		if(0 == strncmp(dirent.fs_fname + 4, del_filepath + (drive_name_len + 20), 4)) {
			strcpy(file_name, dir_name);
			strcat(file_name, "\\");
			strcat(file_name, dirent.fs_fname);

			g_bf_dcf_controller_ret_err = BF_Fs_If_Remove(file_name);
			if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
#endif
				return g_bf_dcf_controller_ret_err;
			}
		}
	}

	if(g_bf_dcf_controller_ret_err == 0){
		if(g_bf_dcf_controller_file_name_setting == E_BF_DCF_IF_FILE_NAME_SERIES){
			dcf_controller_get_last_file(&dirno, &fileno, E_BF_DCF_IF_LATEST_TYPE_ALL);
		}

		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Delete_File(*file_index, del_filepath);

		if(g_bf_dcf_controller_file_name_setting == E_BF_DCF_IF_FILE_NAME_SERIES){
#ifdef CO_BF_DCF_CONTROLLER_SERIES_DIR_RESET
			BF_Dcf_Db_Get_Last_File_In_Db(&tmp_dirno, &tmp_fileno);
			dcf_controller_set_last_file(tmp_dirno, fileno);
#else
			dcf_controller_set_last_file(dirno, fileno);
#endif
		}
		else{
			BF_Dcf_Db_Get_Last_File_In_Db(&dirno, &fileno);
			dcf_controller_set_last_file(dirno, fileno);
		}
	}

	/* updata free space cache, if single delete */
	if( all_flg == E_BF_DCF_CONTROLLER_ALL_DELETE_FLAG_OFF ){
		BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
		BF_Mdf_Wrapper_Get_Free_Space(drive_name, &blksize, &blocks, &bfree);
	}

	return g_bf_dcf_controller_ret_err;
}
#endif


#if 1
FW_INT32 BF_Dcf_Controller_Del_All_Obj(E_BF_MEDIA_STATE_MEDIA_TYPE* media_type)
{
	FW_INT32 ercd;
	FW_CHAR  currentDirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_CHAR  dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32 total_file_num;
	FW_INT32 file_cnt;
	FW_SHORT total_dir_cnt;
	FW_SHORT cur_dir_num;
	FW_INT32 dir_name_len;
	FW_SHORT cur_dir_index;

	if (media_type == NULL) {
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	if (*media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_CUR) {
		;
	}
	else if (*media_type != BF_Media_State_Get_Used_Media()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("BF_Dcf_Controller_Del_All_Obj(): NOT USED MEDIA: used_media=%d, target_media=%d\n", BF_Media_State_Get_Used_Media(), *media_type));
#endif
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_INSERT_MEDIA;
		return g_bf_dcf_controller_ret_err;
	}
	else {
		; // DO NOTHING
	}

	ercd = BF_Dcf_Db_Read_Dir_Count(&total_dir_cnt);
	if (FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	if (total_dir_cnt == 0) {
		g_bf_dcf_controller_ret_err = 0;
		return g_bf_dcf_controller_ret_err;
	}

	strcpy(dir_name, (FW_CHAR*)g_bf_dcf_controller_dcim_path);
	dir_name_len = strlen(dir_name);
	total_file_num = 0;
	for (cur_dir_num = 1; cur_dir_num <= total_dir_cnt; cur_dir_num++) {
		ercd = BF_Dcf_Db_Read_Dir_Name(cur_dir_num, currentDirName);
		if (FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

		strcpy(&dir_name[dir_name_len], currentDirName);
		ercd = dcf_controller_get_total_file_count(dir_name, &file_cnt);
		if (FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

		total_file_num += file_cnt;
	}

	file_cnt = 0;
	ercd = BF_Dcf_Db_Get_Dir_Index_Num(DCF_DB_CURRENT_DIR, &cur_dir_index);
	if (FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	for (cur_dir_num = 1; cur_dir_num <= total_dir_cnt; cur_dir_num++) {
		ercd = BF_Dcf_Db_Set_Current_Dir(cur_dir_num);
		if (FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

		g_bf_dcf_controller_ret_err = dcf_controller_del_dir(total_file_num, &file_cnt);
		if (FSDCF_ERRNO_OK != g_bf_dcf_controller_ret_err) {
			return g_bf_dcf_controller_ret_err;
		}
	}
#if 0 // "THM" Not supported
	FW_INT32 total_thm_file_num;
	dcf_controller_del_all_thm_dir(&total_thm_file_num);
#endif // "THM" Not supported

	/* DCF-DB init */
	ercd = BF_Dcf_Controller_DataBase_Create();
	ercd = BF_Dcf_Db_Set_Current_Dir(cur_dir_index);
	if (FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}
	return g_bf_dcf_controller_ret_err;
}
#endif


#if 0
FW_INT32 BF_Dcf_Controller_Del_All_Obj_In_Current_Dir(E_BF_MEDIA_STATE_MEDIA_TYPE* media_type)
{
	FW_INT32 ercd;
	FW_CHAR drive_name[D_BF_FS_IF_DRVNAME_MAX+1];
	FW_CHAR dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR file_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32 dNo_dir;
	T_BF_FS_IF_Dir_Info dir_dir;
	FW_CHAR currentDirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_INT32 len;
	FW_INT32 cur_file_cnt;
	FW_INT32 total_file_num;
	T_BF_DCF_IF_FILE_INDEX file_index;
	T_BF_DCF_IF_FILE_INFO file_info;
	FW_CHAR ext[4];
	FW_INT32 objIndex_del;
	BOOL stop_flg;
	T_BF_DCF_DB_OBJ_INFO* ppObjInfo;
	FW_CHAR file_path[14];
	FW_CHAR obj_name[9];
	FW_SHORT pdir_index_num;
	FW_CHAR cur_drive_name[D_BF_FS_IF_DRVNAME_MAX+1];
	FW_CHAR curpath[31];
	FW_USHORT last_file_num;

	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Dcf_Controller_Close_Display_File();
	}

	if(media_type == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	if(*media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_CUR){
		;
	}
	else if(*media_type != BF_Media_State_Get_Used_Media()){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("BF_Dcf_Controller_Del_All_Obj_In_Current_Dir(): NOT USED MEDIA: used_media=%d, target_media=%d\n", BF_Media_State_Get_Used_Media(), *media_type));
#endif
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_INSERT_MEDIA;
		return g_bf_dcf_controller_ret_err;
	}
	else{
		; // DO NOTHING
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	ercd = BF_Dcf_Db_Read_Dir_Name(DCF_DB_CURRENT_DIR, currentDirName);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = BF_Dcf_Db_Get_Dir_Index_Num(DCF_DB_CURRENT_DIR, &pdir_index_num);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* count up total file number */
	strcpy(dir_name, g_bf_dcf_controller_dcim_path);
	strcat(dir_name, currentDirName);
	ercd = dcf_controller_get_total_file_count(dir_name, &total_file_num);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	strcpy(file_name, dir_name);
	strcat(file_name, "\\");
	len = strlen(file_name);

	BF_Fs_Controller_Get_Cur_Drv(cur_drive_name);
	/* Get current directory */
	BF_Fs_Controller_Get_Cur_Dir(cur_drive_name, 31, curpath);

	/* A temporary current directory is changed. */
#if 1
	g_bf_dcf_controller_ret_err = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}
	strcat(drive_name, ":");
	ercd = BF_Fs_Controller_Chdir(drive_name);
	if( ercd != 0 ) {
		return ercd;
	}
#endif
	ercd = BF_Fs_Controller_Chdir(dir_name);
	if( ercd != 0 ) {
		return ercd;
	}

	/* Get last file number */
	ercd = BF_Dcf_Db_Get_Last_File_Number(pdir_index_num, &last_file_num);
	if( ercd != 0 ) {
		return ercd;
	}

	cur_file_cnt = 0;
	stop_flg = FALSE;

	/* allow file delete */
	for(objIndex_del = 0; objIndex_del < 10000; objIndex_del++) {
		if( BF_Dcf_If_Receiver_Is_Async_Stop() == E_BF_DCF_IF_RECIVER_ASYNC_STOP ){
			g_bf_dcf_controller_ret_err = 0;
			stop_flg = TRUE;
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
			BF_Fs_User_Custom_Info_Printer(("DELETE ALL STOP\n"));
#endif
			break;
		}

		if(objIndex_del > last_file_num){
			g_bf_dcf_controller_ret_err = 0;
			break;
		}

		if(BF_Dcf_Db_Read_Obj_Info(pdir_index_num, objIndex_del, &ppObjInfo) != 0) {
			continue;
		}
		;

		if(ppObjInfo->objName[0] == 0) {
			continue;
		}
		if((ppObjInfo->attr & D_BF_MDF_WRAPPER_ATTR_READONLY) == D_BF_MDF_WRAPPER_ATTR_READONLY) {
			continue;
		}

		BF_Dcf_Db_Read_Obj_Name(pdir_index_num, ppObjInfo->objIndex, obj_name);

		BF_Dcf_Db_Change_File_Type_To_Char(ppObjInfo->extType, ext);

		// make file name
		strcpy(file_path, obj_name);
		strcat(file_path, ".");
		strcat(file_path, ext);

		cur_file_cnt++;

		/* file delete */
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_For_Multi_Delete(file_path);

		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_For_Multi_Delete() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_path));
#endif
			continue;
		}

		FJ_Host_NoticeProgressCB( cur_file_cnt, total_file_num );
	}

	/* Set current directory */
#if 1
	strcat(cur_drive_name, ":");
	ercd = BF_Fs_Controller_Chdir(cur_drive_name);
	if( ercd != 0 ) {
		return ercd;
	}
#endif
	ercd = BF_Fs_Controller_Chdir(curpath);
	if( ercd != 0 ) {
		return ercd;
	}

	/* updata freesize cache */
	g_bf_dcf_controller_ret_err = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Sync(drive_name);

	ercd = BF_Mdf_Wrapper_Opendir(dir_name, &dNo_dir);
	if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, dir_name));
#endif
		BF_Mdf_Wrapper_Closedir(dNo_dir);
		return ercd;
	}

	/* not allow file delete */
	for(;;) {
		ercd = BF_Mdf_Wrapper_Readdir(dNo_dir, &dir_dir);
		if(0 != ercd) {
			if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
				// end of object
				break;
			}
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Closedir(dNo_dir);
			return ercd;
		}

		if('.' == dir_dir.fname[0]) {
			if(0 == dir_dir.fname[1]) {
				continue;
			}
			if(('.' == dir_dir.fname[1]) && (0 == dir_dir.fname[2])) {
				continue;
			}
		}
		/* search object */
		if(0 != BF_Dcf_Db_Search_Obj_By_Name(currentDirName, dir_dir.fname, &(file_index.pack.dir_index), &(file_index.pack.obj_index))) {
			continue;
		}

		if(stop_flg == TRUE){
			if(file_index.pack.obj_index > ppObjInfo->objIndex) {
				continue;
			}
		}

		if ((dir_dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
			continue;
		}
		if(0 != BF_Dcf_Db_Get_Index_File_Info(file_index, NULL, &file_info)) {
			continue;
		}
		if(file_info.file_attr == E_BF_DCF_IF_ATTR_RDONLY) {
			continue;
		}

		cur_file_cnt++;
		strcpy(file_name + len, dir_dir.fname);

		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Current_Entry(dNo_dir, &dir_dir);

		if(stop_flg == FALSE){
			FJ_Host_NoticeProgressCB( cur_file_cnt, total_file_num );
		}

		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Current_Entry() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
#endif
			continue;
		}
	}

	ercd = BF_Mdf_Wrapper_Closedir(dNo_dir);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	/* Renewal of current directory */
	BF_Dcf_Db_Restruct_Current_Dir_Only();

	/* updata freesize cache */
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Sync(drive_name);

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF DELETE ALL END\n");
#endif

	return g_bf_dcf_controller_ret_err;

#if 0 // File Entry order delete
	FW_INT32 ercd;
//	FW_ULONG blksize;
//	FW_ULONG blocks;
//	FW_ULONG bfree;
	FW_CHAR drive_name[D_BF_FS_IF_DRVNAME_MAX+1];
	FW_CHAR dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR file_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32 dNo_dir;
	T_BF_FS_IF_Dir_Info dir_dir;
	FW_CHAR currentDirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_INT32 len;
	FW_INT32 cur_file_cnt;
	FW_INT32 total_file_num;
	T_BF_DCF_IF_FILE_INDEX file_index;
	T_BF_DCF_IF_FILE_INFO file_info;

	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Dcf_Controller_Close_Display_File();
	}

	if(media_type == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	if(*media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_CUR){
		;
	}
	else if(*media_type != BF_Media_State_Get_Used_Media()){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("BF_Dcf_Controller_Del_All_Obj_In_Current_Dir(): NOT USED MEDIA: used_media=%d, target_media=%d\n", BF_Media_State_Get_Used_Media(), *media_type));
#endif
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_INSERT_MEDIA;
		return g_bf_dcf_controller_ret_err;
	}
	else{
		; // DO NOTHING
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	ercd = BF_Dcf_Db_Read_Dir_Name(DCF_DB_CURRENT_DIR, currentDirName);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* count up total file number */
	strcpy(dir_name, g_bf_dcf_controller_dcim_path);
	strcat(dir_name, currentDirName);
	ercd = dcf_controller_get_total_file_count(dir_name, &total_file_num);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	strcpy(dir_name, g_bf_dcf_controller_dcim_path);
	strcat(dir_name, currentDirName);
	ercd = BF_Mdf_Wrapper_Opendir(dir_name, &dNo_dir);
	if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, dir_name));
#endif
		BF_Mdf_Wrapper_Closedir(dNo_dir);
		return ercd;
	}

	strcpy(file_name, dir_name);
	strcat(file_name, "\\");
	len = strlen(file_name);

	cur_file_cnt = 0;
	for(;;) {
		if( BF_Dcf_If_Receiver_Is_Async_Stop() == E_BF_DCF_IF_RECIVER_ASYNC_STOP ){
			g_bf_dcf_controller_ret_err = 0;
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
			BF_Fs_User_Custom_Info_Printer(("DELETE ALL STOP\n"));
#endif
			break;
		}

		ercd = BF_Mdf_Wrapper_Readdir(dNo_dir, &dir_dir);
		if(0 != ercd) {
			if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
				// end of object
				break;
			}
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Closedir(dNo_dir);
			return ercd;
		}

		if('.' == dir_dir.fname[0]) {
			if(0 == dir_dir.fname[1]) {
				continue;
			}
			if(('.' == dir_dir.fname[1]) && (0 == dir_dir.fname[2])) {
				continue;
			}
		}
		/* search object */
		if(0 != BF_Dcf_Db_Search_Obj_By_Name(currentDirName, dir_dir.fname, &(file_index.pack.dir_index), &(file_index.pack.obj_index))) {
			continue;
		}

		if ((dir_dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
			continue;
		}
		if(0 != BF_Dcf_Db_Get_Index_File_Info(file_index, NULL, &file_info)) {
			continue;
		}
		if(file_info.file_attr == E_BF_DCF_IF_ATTR_RDONLY) {
			continue;
		}

		cur_file_cnt++;
		strcpy(file_name + len, dir_dir.fname);

#if 0
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(file_name);
#else
//		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_For_Multi_Delete(file_name);
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Current_Entry(dNo_dir, &dir_dir);
#endif

		FJ_Host_NoticeProgressCB( cur_file_cnt, total_file_num );

		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Current_Entry() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
#endif
			continue;
		}
	}

	ercd = BF_Mdf_Wrapper_Closedir(dNo_dir);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	BF_Dcf_Db_Clear_Db_buf();

	/* updata freesize cache */
	g_bf_dcf_controller_ret_err = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}
#if 0
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Get_Free_Space(drive_name, &blksize, &blocks, &bfree);
#else
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Sync(drive_name);
#endif

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF DELETE ALL END\n");
#endif

	return g_bf_dcf_controller_ret_err;
#endif // File Entry order delete
}
#endif

/**
 * @brief set attribute one.
 * @param i:*attr_data attribute setting data
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Set_One_Obj_Attr(T_BF_DCF_IF_ATTR_DATA* attr_data)
{
	FW_CHAR lock_filepath[32];
//	FW_CHAR lock_opt_filepath[32];
	FW_CHAR dir_name[18];
	FW_CHAR file_name[766+17];
	FW_INT32 dNo_dir;
	T_BF_FS_IF_Dir_Info dir_dir;
	T_BF_DCF_IF_FILE_INFO file_info;
	FW_SHORT objTblIdx;

	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Dcf_Controller_Close_Display_File();
	}

	if(attr_data == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("FS ATTR. index=%d\n", (attr_data->file_index.index) & 0x0000FFFF));
#endif

	/* get file path & information */
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(attr_data->file_index, lock_filepath, &file_info);
	if(g_bf_dcf_controller_ret_err != 0) {
		return g_bf_dcf_controller_ret_err;
	}

	if(attr_data->attr == file_info.file_attr){
		/* not error */
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("FILE ATTRIBUTE IS ALREADY SPECIFIED ATTRIBUTE(%d).\n", attr_data->attr));
#endif
		g_bf_dcf_controller_ret_err = 0;
		return g_bf_dcf_controller_ret_err;
	}
#if 0
	if(file_info.option_file == E_BF_DCF_IF_OPTION_FILE_EXIST){
		/* change attribute option file */
		g_bf_dcf_controller_ret_err = dcf_controller_get_index_option_file_path_abs(lock_filepath, file_info.file_type, lock_opt_filepath);
		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("NOT EXIST OPTION FILE: path=%s\n", lock_opt_filepath));
#endif
		}
		else{
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Set_Attribute(lock_opt_filepath, (FW_UCHAR)attr_data->attr);
			if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Attribute() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, lock_opt_filepath));
#endif
			}
		}
	}
#endif
	strncpy(dir_name, lock_filepath, 17);
	dir_name[17] = 0;
	strcpy(file_name, dir_name);
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Opendir(dir_name, &dNo_dir);
	if(FSDCF_ERRNO_OK != g_bf_dcf_controller_ret_err) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", g_bf_dcf_controller_ret_err, dir_name));
#endif
		BF_Mdf_Wrapper_Closedir(dNo_dir);
		return g_bf_dcf_controller_ret_err;
	}

	for(;;) {
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Readdir(dNo_dir, &dir_dir);
		if(0 != g_bf_dcf_controller_ret_err) {
			if(D_BF_MDF_WRAPPER_ENT_LAST == g_bf_dcf_controller_ret_err) {
				// end of object
				break;
			}
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", g_bf_dcf_controller_ret_err));
#endif
			BF_Mdf_Wrapper_Closedir(dNo_dir);
			return g_bf_dcf_controller_ret_err;
		}

		if('.' == dir_dir.fname[0]) {
			if(0 == dir_dir.fname[1]) {
				continue;
			}
			if(('.' == dir_dir.fname[1]) && (0 == dir_dir.fname[2])) {
				continue;
			}
		}
		if ((dir_dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
			continue;
		}
		objTblIdx = BF_Dcf_Db_Get_Obj_No(dir_dir.fname);
		if(objTblIdx == FSDCF_ERRNO_NOT_SUPPORT_FILENO) {
			continue;
		}
		if(0 != strncmp(dir_dir.fname + 4, lock_filepath + 21, 4)) {
			continue;
		}

		/* file protection */
		strcpy(file_name + 17, dir_dir.fname);
		g_bf_dcf_controller_ret_err = dcf_controller_set_protect_attribute(file_name, (FW_UCHAR)attr_data->attr);
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
		BF_Fs_User_Custom_Info_Printer(("Set Attribute(%d) to File(%s)\n", attr_data->attr, file_name));
#endif
		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("dcf_controller_set_protect_attribute() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, lock_filepath));
#endif
			return g_bf_dcf_controller_ret_err;
		}
	}

	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Closedir(dNo_dir);
	if(g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", g_bf_dcf_controller_ret_err));
#endif
		return g_bf_dcf_controller_ret_err;
	}

	/* update DB */
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Update_Attr(attr_data->file_index, (E_BF_DCF_IF_ATTR)attr_data->attr);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}

	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief set attribute all in current directory
 * @param i:*attr_all_data set attribute data for all
 * @return error code
 * @note none
 * @attention none
 */
#if 1
FW_INT32 BF_Dcf_Controller_Set_All_Obj_Attr_In_Current_Dir(T_BF_DCF_IF_ATTR_ALL_DATA* attr_all_data)
{
	FW_INT32 ercd;
	FW_CHAR currentDirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_CHAR dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32 total_file_num;
	FW_INT32 file_cnt;

	if (attr_all_data == NULL) {
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	if (attr_all_data->media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_CUR) {
		;
	}
	else if (attr_all_data->media_type != BF_Media_State_Get_Used_Media()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("BF_Dcf_Controller_Set_All_Obj_Attr_In_Current_Dir(): NOT USED MEDIA: used_media=%d, target_media=%d\n", BF_Media_State_Get_Used_Media(), attr_all_data->media_type));
#endif
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_INSERT_MEDIA;
		return g_bf_dcf_controller_ret_err;
	}
	else {
		; // DO NOTHING
	}

	ercd = BF_Dcf_Db_Read_Dir_Name(DCF_DB_CURRENT_DIR, currentDirName);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	strcpy(dir_name, (FW_CHAR*)g_bf_dcf_controller_dcim_path);
	strcat(dir_name, currentDirName);
	ercd = dcf_controller_get_total_file_count(dir_name, &total_file_num);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	file_cnt = 0;
	return dcf_controller_set_attr_dir(total_file_num, &file_cnt, attr_all_data);
}
FW_INT32 BF_Dcf_Controller_Set_All_Obj_Attr(T_BF_DCF_IF_ATTR_ALL_DATA* attr_all_data)
{
	FW_INT32 ercd;
	FW_CHAR  currentDirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_CHAR  dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32 total_file_num;
	FW_INT32 file_cnt;
	FW_SHORT total_dir_cnt;
	FW_SHORT cur_dir_num;
	FW_INT32 dir_name_len;
	FW_SHORT cur_dir_index;

	if (attr_all_data == NULL) {
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	if (attr_all_data->media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_CUR) {
		;
	}
	else if (attr_all_data->media_type != BF_Media_State_Get_Used_Media()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("BF_Dcf_Controller_Set_All_Obj_Attr_In_Current_Dir(): NOT USED MEDIA: used_media=%d, target_media=%d\n", BF_Media_State_Get_Used_Media(), attr_all_data->media_type));
#endif
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_INSERT_MEDIA;
		return g_bf_dcf_controller_ret_err;
	}
	else {
		; // DO NOTHING
	}

	ercd = BF_Dcf_Db_Read_Dir_Count(&total_dir_cnt);
	if (FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	if (total_dir_cnt == 0) {
		g_bf_dcf_controller_ret_err = 0;
		return g_bf_dcf_controller_ret_err;
	}

	strcpy(dir_name, (FW_CHAR*)g_bf_dcf_controller_dcim_path);
	dir_name_len = strlen(dir_name);
	total_file_num = 0;
	for (cur_dir_num = 1; cur_dir_num <= total_dir_cnt; cur_dir_num++) {
		ercd = BF_Dcf_Db_Read_Dir_Name(cur_dir_num, currentDirName);
		if (FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

		strcpy(&dir_name[dir_name_len], currentDirName);
		ercd = dcf_controller_get_total_file_count(dir_name, &file_cnt);
		if (FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

		total_file_num += file_cnt;
	}

	file_cnt = 0;
	ercd = BF_Dcf_Db_Get_Dir_Index_Num(DCF_DB_CURRENT_DIR, &cur_dir_index);
	if (FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	for (cur_dir_num = 1; cur_dir_num <= total_dir_cnt; cur_dir_num++) {
		ercd = BF_Dcf_Db_Set_Current_Dir(cur_dir_num);
		if (FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

		g_bf_dcf_controller_ret_err = dcf_controller_set_attr_dir(total_file_num, &file_cnt, attr_all_data);
		if (FSDCF_ERRNO_OK != g_bf_dcf_controller_ret_err) {
			return g_bf_dcf_controller_ret_err;
		}
	}

	ercd = BF_Dcf_Db_Set_Current_Dir(cur_dir_index);
	if (FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	return g_bf_dcf_controller_ret_err;
}
#endif


#if 0
FW_INT32 BF_Dcf_Controller_Set_All_Obj_Attr_In_Current_Dir(T_BF_DCF_IF_ATTR_ALL_DATA* attr_all_data)
{
#if 1	// DCF-Object order
	FW_INT32			ercd;
	FW_CHAR				drive_name[D_BF_FS_IF_DRVNAME_MAX+1];
	FW_CHAR				dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR				file_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32			dNo_dir;
	T_BF_FS_IF_Dir_Info	dir_dir;
	FW_CHAR				currentDirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_INT32			len;
	FW_INT32			cur_file_cnt;
	FW_INT32			total_file_num;
	T_BF_DCF_IF_FILE_INDEX	file_index;
	FW_CHAR				ext[4];
	FW_INT32			objIndex_set;
	BOOL				stop_flg;
	T_BF_DCF_DB_OBJ_INFO*	pObjInfo;
	FW_CHAR				file_path[14];
	FW_CHAR				obj_name[9];
	FW_SHORT			dir_index_num;
	FW_CHAR				cur_drive_name[D_BF_FS_IF_DRVNAME_MAX+1];
	FW_CHAR				curpath[31];
	FW_USHORT			last_file_num;

	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Dcf_Controller_Close_Display_File();
	}

	if (attr_all_data == NULL) {
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	if (attr_all_data->media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_CUR) {
		;
	}
	else if (attr_all_data->media_type != BF_Media_State_Get_Used_Media()) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("BF_Dcf_Controller_Set_All_Obj_Attr_In_Current_Dir(): NOT USED MEDIA: used_media=%d, target_media=%d\n", BF_Media_State_Get_Used_Media(), attr_all_data->media_type));
#endif
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_INSERT_MEDIA;
		return g_bf_dcf_controller_ret_err;
	}
	else {
		; // DO NOTHING
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	ercd = BF_Dcf_Db_Read_Dir_Name(DCF_DB_CURRENT_DIR, currentDirName);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	ercd = BF_Dcf_Db_Get_Dir_Index_Num(DCF_DB_CURRENT_DIR, &dir_index_num);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* count up total file number */
	strcpy(dir_name, g_bf_dcf_controller_dcim_path);
	strcat(dir_name, currentDirName);
	ercd = dcf_controller_get_total_file_count(dir_name, &total_file_num);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	strcpy(dir_name, g_bf_dcf_controller_dcim_path);
	strcat(dir_name, currentDirName);

	strcpy(file_name, dir_name);
	strcat(file_name, "\\");
	len = strlen(file_name);

	BF_Fs_Controller_Get_Cur_Drv(cur_drive_name);
	/* Get current directory */
	BF_Fs_Controller_Get_Cur_Dir(cur_drive_name, 31, curpath);

	/* A temporary current directory is changed. */
	ercd = BF_Fs_Controller_Chdir(dir_name);
	if( ercd != 0 ) {
		return ercd;
	}

	/* Get last file number */
	ercd = BF_Dcf_Db_Get_Last_File_Number(dir_index_num, &last_file_num);
	if( ercd != 0 ) {
		return ercd;
	}

	cur_file_cnt = 0;
	stop_flg = FALSE;

	/* allow file set */
	for(objIndex_set = 0; objIndex_set < 10000; objIndex_set++) {
		if( BF_Dcf_If_Receiver_Is_Async_Stop() == E_BF_DCF_IF_RECIVER_ASYNC_STOP ){
			g_bf_dcf_controller_ret_err = 0;
			stop_flg = TRUE;
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
			BF_Fs_User_Custom_Info_Printer(("DELETE ALL STOP\n"));
#endif
			break;
		}

		if(objIndex_set > last_file_num){
			g_bf_dcf_controller_ret_err = 0;
			break;
		}

		if(BF_Dcf_Db_Read_Obj_Info(dir_index_num, objIndex_set, &pObjInfo) != 0) {
			continue;
		}

		if(pObjInfo->objName[0] == 0) {
			continue;
		}

		BF_Dcf_Db_Read_Obj_Name(dir_index_num, pObjInfo->objIndex, obj_name);

		BF_Dcf_Db_Change_File_Type_To_Char(pObjInfo->extType, ext);

		// make file name
		strcpy(file_path, obj_name);
		strcat(file_path, ".");
		strcat(file_path, ext);

		cur_file_cnt++;

		/* file delete */
//		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Set_Attribute(file_name, (FW_UCHAR)attr_all_data->all_attr);
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Set_Attribute_For_Multi_Set(file_path, (FW_UCHAR)attr_all_data->all_attr);

		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Attribute_For_Multi_Set() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
#endif
			continue;
		}

		FJ_Host_NoticeProgressCB( cur_file_cnt, total_file_num );
	}

	/* Set current directory */
	ercd = BF_Fs_Controller_Chdir(curpath);
	if( ercd != 0 ) {
		return ercd;
	}

	/* updata freesize cache */
	g_bf_dcf_controller_ret_err = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Sync(drive_name);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}

	if (cur_file_cnt >= total_file_num) {
		// End
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
		BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF CHANGE ATTRIBUTE ALL END\n");
#endif
		/* update attribute (DCF-DB) */
		ercd = BF_Dcf_Db_Update_Attr_Dir(attr_all_data->all_attr);
		if(ercd != 0) {
			return ercd;
		}

		return g_bf_dcf_controller_ret_err;
	}

	ercd = BF_Mdf_Wrapper_Opendir(dir_name, &dNo_dir);
	if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, dir_name));
#endif
		BF_Mdf_Wrapper_Closedir(dNo_dir);
		return ercd;
	}

	cur_file_cnt = 0;
	for(;;) {
		ercd = BF_Mdf_Wrapper_Readdir(dNo_dir, &dir_dir);
		if(0 != ercd) {
			if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
				// end of object
				break;
			}
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Closedir(dNo_dir);
			return ercd;
		}

		if('.' == dir_dir.fname[0]) {
			if(0 == dir_dir.fname[1]) {
				continue;
			}
			if(('.' == dir_dir.fname[1]) && (0 == dir_dir.fname[2])) {
				continue;
			}
		}
		/* search object */
		if(0 != BF_Dcf_Db_Search_Obj_By_Name(currentDirName, dir_dir.fname, &(file_index.pack.dir_index), &(file_index.pack.obj_index))) {
			continue;
		}

		if(stop_flg == TRUE){
			if(file_index.pack.obj_index > pObjInfo->objIndex) {
				continue;
			}
		}

		if ((dir_dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
			continue;
		}
		if (dir_dir.attributes == (FW_UCHAR)attr_all_data->all_attr) {
			continue;
		}
		cur_file_cnt++;
		strcpy(file_name + len, dir_dir.fname);

//		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Set_Attribute(file_name, (FW_UCHAR)attr_all_data->all_attr);
		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Set_Attribute_Current_Entry(dNo_dir, &dir_dir, (FW_UCHAR)attr_all_data->all_attr);

		if(stop_flg == FALSE){
			FJ_Host_NoticeProgressCB( cur_file_cnt, total_file_num );
		}

		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Attribute_Current_Entry() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
#endif
			continue;
		}
	}

	ercd = BF_Mdf_Wrapper_Closedir(dNo_dir);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Sync(drive_name);

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF CHANGE ATTRIBUTE ALL END\n");
#endif
	/* update attribute (DCF-DB) */
	if (stop_flg == TRUE) {
		ercd = BF_Dcf_Db_Update_Attr_Dir_For_Stop(attr_all_data->all_attr,  pObjInfo->objIndex);
	}
	else {
		ercd = BF_Dcf_Db_Update_Attr_Dir(attr_all_data->all_attr);
	}
	if(ercd != 0) {
		return ercd;
	}

	return g_bf_dcf_controller_ret_err;
#else	// File Entry order
	FW_INT32 ercd;
	FW_CHAR dir_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_CHAR file_name[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_INT32 dNo_dir;
	T_BF_FS_IF_Dir_Info dir_dir;
	FW_CHAR currentDirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_INT32 len;
	FW_INT32 cur_file_cnt;
	FW_INT32 total_file_num;
	T_BF_DCF_IF_FILE_INDEX file_index;

	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Dcf_Controller_Close_Display_File();
	}

	if(attr_all_data == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	if(attr_all_data->media_type == E_BF_MEDIA_STATE_MEDIA_TYPE_CUR){
		;
	}
	else if(attr_all_data->media_type != BF_Media_State_Get_Used_Media()){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
		BF_Fs_User_Custom_Wrn_Printer(("BF_Dcf_Controller_Set_All_Obj_Attr_In_Current_Dir(): NOT USED MEDIA: used_media=%d, target_media=%d\n", BF_Media_State_Get_Used_Media(), attr_all_data->media_type));
#endif
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_NOT_INSERT_MEDIA;
		return g_bf_dcf_controller_ret_err;
	}
	else{
		; // DO NOTHING
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	ercd = BF_Dcf_Db_Read_Dir_Name(DCF_DB_CURRENT_DIR, currentDirName);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* count up total file number */
	strcpy(dir_name, g_bf_dcf_controller_dcim_path);
	strcat(dir_name, currentDirName);
	ercd = dcf_controller_get_total_file_count(dir_name, &total_file_num);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	strcpy(dir_name, g_bf_dcf_controller_dcim_path);
	strcat(dir_name, currentDirName);
	ercd = BF_Mdf_Wrapper_Opendir(dir_name, &dNo_dir);
	if(FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, dir_name));
#endif
		BF_Mdf_Wrapper_Closedir(dNo_dir);
		return ercd;
	}

	strcpy(file_name, dir_name);
	strcat(file_name, "\\");
	len = strlen(file_name);

	cur_file_cnt = 0;
	for(;;) {
		if( BF_Dcf_If_Receiver_Is_Async_Stop() == E_BF_DCF_IF_RECIVER_ASYNC_STOP ){
			g_bf_dcf_controller_ret_err = 0;
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
			BF_Fs_User_Custom_Info_Printer(("SET ATTR ALL STOP\n"));
#endif
			break;
		}

		ercd = BF_Mdf_Wrapper_Readdir(dNo_dir, &dir_dir);
		if(0 != ercd) {
			if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
				// end of object
				break;
			}
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif
			BF_Mdf_Wrapper_Closedir(dNo_dir);
			return ercd;
		}

		if('.' == dir_dir.fname[0]) {
			if(0 == dir_dir.fname[1]) {
				continue;
			}
			if(('.' == dir_dir.fname[1]) && (0 == dir_dir.fname[2])) {
				continue;
			}
		}
		/* search object */
		if(0 != BF_Dcf_Db_Search_Obj_By_Name(currentDirName, dir_dir.fname, &(file_index.pack.dir_index), &(file_index.pack.obj_index))) {
			continue;
		}

		if ((dir_dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
			continue;
		}
		cur_file_cnt++;
		strcpy(file_name + len, dir_dir.fname);

		g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Set_Attribute(file_name, (FW_UCHAR)attr_all_data->all_attr);

		FJ_Host_NoticeProgressCB( cur_file_cnt, total_file_num );

		if(g_bf_dcf_controller_ret_err != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Set_Attribute() failed: ercd=%d, path=%s\n", g_bf_dcf_controller_ret_err, file_name));
#endif
			continue;
		}
	}

	ercd = BF_Mdf_Wrapper_Closedir(dNo_dir);
	if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF CHANGE ATTRIBUTE ALL END\n");
#endif
	/* update attribute (DCF-DB) */
	ercd = BF_Dcf_Db_Update_Attr_Dir(attr_all_data->all_attr);
	if(ercd != 0) {
		return ercd;
	}

	return g_bf_dcf_controller_ret_err;
#endif
}
#endif

/**
 * @brief get specified index file path.
 * @param i:file_index file index
 * @param i:file_type file type
 * @param o:*full_file_path full file path
 * @return error code
 */
FW_INT32 BF_dcf_controller_Make_Index_File_Path_Type_Abs(T_BF_DCF_IF_FILE_INDEX file_index, FW_CHAR file_type, FW_CHAR* full_file_path)
{
	FW_INT32 ercd;
	FW_UINT32 drive_name_len=0;
	T_BF_DCF_DB_OBJ_INFO* db_file_info;
	FW_CHAR ext[4];
	FW_CHAR  chk_path[64];
	char *drive_name;

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("BF_dcf_controller_Make_Index_File_Path_Type_Abs() input index=0x%08lx \n", file_index.index ));
#endif
	/* get file node */
	ercd = BF_Dcf_Db_Search_Obj_Info(file_index.pack.dir_index, file_index.pack.obj_index, &db_file_info);
	if( ercd != 0 ) {
		return ercd;
	}

	/* get full file path */
	ercd = BF_Dcf_Db_Make_Full_Path(file_index, full_file_path);
	if( ercd != 0 ) {
		return ercd;
	}

	/* change ext */
	ercd = BF_Dcf_Db_Get_Ext_By_Type(file_type, ext);
	if(ercd != 0) {
		return ercd;
	}

	strcpy(chk_path, full_file_path );
	drive_name = strtok(chk_path, ":");
	if (drive_name == NULL) {
		return FSDCF_ERRNO_PARAM_FAILED;
	}
	drive_name_len = strlen(drive_name);
	if ( drive_name_len == 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("Drive name length Err: drive name=%s, length=%d \n", drive_name, drive_name_len));
#endif
		return FSDCF_ERRNO_DRIVE_INIT_FAILED;
	}

//	strcpy(full_file_path + 25, ".");      // need '.' when no-extension of original object information
//	strcpy(full_file_path + 26, ext);
	strcpy(full_file_path + (drive_name_len + 24), ".");      // need '.' when no-extension of original object information
	strcpy(full_file_path + (drive_name_len + 25), ext);


#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("path=%s\n", full_file_path));
#endif

	return 0;
}


/**
 * @brief get fstat data.
 * @param i:*rotate_param rotation parameter
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Fstat(T_BF_DCF_IF_FSTAT_DATA* fstat_data)
{
	FW_CHAR open_filepath[64];
	T_BF_FS_IF_STAT fstat;
	FW_INT32 ercd;

	if(fstat_data == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = BF_dcf_controller_Make_Index_File_Path_Type_Abs(fstat_data->file_index, (FW_UCHAR)(0x00FF & (fstat_data->file_type)), open_filepath);
	if(g_bf_dcf_controller_ret_err != 0){
		return g_bf_dcf_controller_ret_err;
	}

	ercd = BF_Mdf_Wrapper_Stat(open_filepath, &fstat);
	if( ercd == D_BF_MDF_WRAPPER_ERR_NOENT){
		g_bf_dcf_controller_ret_err = D_BF_MDF_WRAPPER_ERR_NOENT;
		return g_bf_dcf_controller_ret_err;
	}

	strcpy(fstat_data->fpath, fstat.fname);
	*fstat_data->attr = fstat.attributes;
	*fstat_data->date = fstat.date;
	*fstat_data->time = fstat.time;
	*fstat_data->file_size = fstat.fileSize;

	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief get file index.
 * @param o:file_index file index
 * @param i:*file_path fila path
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_File_Index(T_BF_DCF_IF_INDEX_FILE_NAME* index_file_name)
{
	g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TARGET_FILE_NOT_EXIST;
	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief make new DCF file path.
 * @param i:*ext file extension
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Make_New_File_Path(FW_CHAR* ext, FW_CHAR* file_path)
{
	g_bf_dcf_controller_ret_err = dcf_controller_make_new_file_path_abs(ext, file_path);
	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief make new thumbnail DCF file path.
 * @param i:*thm_file_param make thumbnail file parameter
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Make_New_Thm_File_Path(T_BF_DCF_IF_MAKE_THM_FILE_PATH_PARAM* thm_file_param, FW_CHAR* thm_file_path)
{
	g_bf_dcf_controller_ret_err = dcf_controller_make_new_thm_file_path_abs(thm_file_param->ext, thm_file_param->main_filepath, thm_file_path);
	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief make path of latest DCF number file(Absolutly).
 * @param i:*ext 
 * @param 0:*file_path
 * @return error code
 */
FW_INT32 BF_Dcf_Controller_Make_Latest_File_Path_Abs(FW_CHAR* ext, FW_CHAR* file_path)
{
	T_BF_DCF_IF_FILE_INDEX file_index;
	E_BF_MEDIA_STATE_MEDIA_TYPE cur_media = E_BF_MEDIA_STATE_MEDIA_TYPE_CUR;

	if(file_path == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	g_bf_dcf_controller_ret_err = BF_Dcf_Controller_Get_Entry(&cur_media, &file_index.index);
	if( g_bf_dcf_controller_ret_err != 0 ) {
		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = BF_Dcf_Controller_Make_Index_File_Path_Ext_Abs(file_index, ext, file_path);
	if( g_bf_dcf_controller_ret_err != 0 ) {
		return g_bf_dcf_controller_ret_err;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("path=%s\n", file_path));
#endif

	return 0;
}


/**
 * @brief make latest DCF file path.
 * @param i:*ext file extension
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Make_Latest_File_Path(FW_LONG* file_type, FW_CHAR* file_path)
{
	FW_CHAR ext[4];

	/* get capital charactor */
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Change_File_Type_To_Char((FW_UCHAR)(0x00FF & (*file_type)), ext);
	if( g_bf_dcf_controller_ret_err != 0 ) {
		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = BF_Dcf_Controller_Make_Latest_File_Path_Abs(ext, file_path);
	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief make index DCF file path.
 * @param i:*index_filepath_data index file path parameter
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Make_Index_File_Path(T_BF_DCF_IF_INDEX_FILE_DATA* index_filepath_data, FW_CHAR* file_path)
{
	if(index_filepath_data == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	/* get capital charactor */
	g_bf_dcf_controller_ret_err = BF_dcf_controller_Make_Index_File_Path_Type_Abs(
		index_filepath_data->file_index,
		(FW_UCHAR)(0x00FF & (index_filepath_data->file_type)),
		file_path);

	return g_bf_dcf_controller_ret_err;
}


/**
 * @brief update dcf cashe for new created file.
 * @param i:*kind DB that becomes target that adds file
 * @param i:*file_path new created file path
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Updata_File(E_BF_DCF_IF_UPDATE_KIND* kind, FW_CHAR* file_path)
{
	FW_INT32 ercd = 0;

	if(*kind == E_BF_DCF_IF_UPDATE_DB){
		ercd = BF_Dcf_Db_Append_Latest_File(file_path);
	}

	return ercd;
}


/**
 * @brief update dcf cashe for new created option file(ex. voicememo).
 * @param i:*file_path new created option file path
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Updata_Option_File(FW_CHAR* file_path)
{
	FW_SHORT dirno, fileno;

	return BF_Dcf_Db_Insert_Current_File(file_path, &dirno, &fileno);
}

/**
 * @brief make specified number fila path.
 * @param i:dirno dir number
 * @param i:fileno file number
 * @param i:*ext file extension
 * @param o:*file_path full fila path
 * @return error code
 */
FW_INT32 BF_Dcf_Controller_Make_Next_Specified_File_Path_Abs(FW_CHAR* dcim_path, FW_USHORT dirno, FW_USHORT fileno, FW_CHAR* ext, FW_CHAR* full_file_path)
{
	FW_INT32 ercd;
	FW_CHAR dir_path[16];
	FW_CHAR file_path[16];

	if(full_file_path == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	if( fileno < FJ_CUSTOM_DCFDB_FILENUM_MAX ){
		ercd = BF_Dcf_Db_Make_Dir_Path(dir_path, (FW_CHAR*)g_bf_dcf_controller_dir_name, dirno);
		if( ercd < 0 ) {
			return ercd;
		}

		ercd = dcf_controller_make_file_path(file_path, (FW_CHAR*)g_bf_dcf_controller_file_name, fileno + 1, ext);
		if(ercd != 0) {
			return ercd;
		}

		strcpy(full_file_path, dcim_path);
		strcat(full_file_path, dir_path);
		strcat(full_file_path, "\\");
		strcat(full_file_path, file_path);

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
		BF_Fs_User_Custom_Info_Printer(("path=%s\n", full_file_path));
#endif
		return 0;
	}

	if( dirno < FJ_CUSTOM_DCFDB_DIRNUM_MAX ){
		ercd = BF_Dcf_Db_Make_Dir_Path(dir_path, (FW_CHAR*)g_bf_dcf_controller_dir_name, dirno + 1);
		if( ercd < 0 ) {
			return ercd;
		}

		strcpy(full_file_path, dcim_path);
		strcat(full_file_path, dir_path);
		ercd = BF_Mdf_Wrapper_Mkdir(full_file_path, 0x01ff);
		if(ercd != 0){
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Mkdir() failed: ercd=%d, path=%s\n", ercd, full_file_path));
#endif
			return ercd;
		}

		ercd = dcf_controller_make_file_path(file_path, (FW_CHAR*)g_bf_dcf_controller_file_name, 1, ext);
		if(ercd != 0) {
			return ercd;
		}

		strcat(full_file_path, "\\");
		strcat(full_file_path, file_path);

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
		BF_Fs_User_Custom_Info_Printer(("path=%s\n", full_file_path));
#endif
		return 0;
	}

	return FSDCF_ERRNO_DIR_FULL;
}

#if 0
/**
 * @brief Copy file from current used media to target media.
 * @param i:*index_list_data file index list data
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Copy_Between_Media(T_BF_DCF_IF_COPY_DATA* copy_data)
{
	E_BF_MEDIA_STATE_MEDIA_TYPE org_media = BF_Media_State_Get_Used_Media();
	FW_DOUBLE free_size;
	T_BF_FS_IF_STAT org_file_stat;
	FW_CHAR dcim_path[16];
	FW_CHAR org_filepath[32];
	FW_CHAR dst_filepath[32];
	FW_INT32 org_file_id;
	FW_INT32 dst_file_id;
	FW_USHORT max_dirno;
	FW_USHORT max_fileno;
	T_BF_DCF_IF_FILE_INFO file_info;
	FW_CHAR ext[4];
	FW_UCHAR same_media_cpy_flag = 0; // 0:different media, 1:same media

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	if(copy_data == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	/* check support media */



	/* check target media */



	/* get org filepath and fileinfo */
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(copy_data->tgt_index, org_filepath, &file_info);
	if(g_bf_dcf_controller_ret_err != 0) {
		return g_bf_dcf_controller_ret_err;
	}

	/* get org file size */
	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Stat(org_filepath, &org_file_stat);
	if(g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Stat() failed: ercd=%d, fname=%s\n", g_bf_dcf_controller_ret_err, org_filepath));
#endif
		return g_bf_dcf_controller_ret_err;
	}

	/* open org file */
	org_file_id = BF_Mdf_Wrapper_Open(org_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
	if( org_file_id < 0 ) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", org_file_id, org_filepath));
#endif
		g_bf_dcf_controller_ret_err = org_file_id;
		return g_bf_dcf_controller_ret_err;
	}

	/* init target media and change to dst media */
	g_bf_dcf_controller_ret_err = BF_Fs_Controller_Start_Media(copy_data->dst_media);
	if( g_bf_dcf_controller_ret_err == D_BF_MDF_WRAPPER_ERR_DEVENT ) {
		// target media is already initizlized.
		g_bf_dcf_controller_ret_err = 0;
		same_media_cpy_flag = 1;
	}
	if( g_bf_dcf_controller_ret_err != 0 ) {
		BF_Fs_Controller_Change_Drive_Info(org_media);
		BF_Mdf_Wrapper_Close(org_file_id);
		return g_bf_dcf_controller_ret_err;
	}

	while(1){
		/* check dst media free_space */
		g_bf_dcf_controller_ret_err = BF_Fs_Controller_Get_FreeSize(copy_data->dst_media, &free_size);
		if(g_bf_dcf_controller_ret_err != 0){
			break;
		}

		if(free_size <= org_filCFC_WRN_PRINT
			BF_Fs_User_Custome_stat.fileSize){
#ifdef CO_BF_FS_USER_CUSTOM_D_Wrn_Printer("FREE SPACE OVER: size=%d\n", free_size);
#endif
			g_bf_dcf_controller_ret_err = FSDCF_ERRNO_FREE_SPACE_OVER;
			break;
		}

		/* search max dirno and max fileno in dst media */
		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Max_Number_Linear(dcim_path, &max_dirno, &max_fileno);
		if( g_bf_dcf_controller_ret_err != 0 ) {
			break;
		}

		/* make dst filepath */
		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Change_File_Type_To_Char(file_info.file_type, ext);
		if( g_bf_dcf_controller_ret_err != 0 ) {
			break;
		}
		g_bf_dcf_controller_ret_err = BF_Dcf_Controller_Make_Next_Specified_File_Path_Abs(dcim_path, max_dirno, max_fileno, ext, dst_filepath);
		if( g_bf_dcf_controller_ret_err != 0 ) {
			break;
		}

		/* create dst file */
		dst_file_id = BF_Mdf_Wrapper_Create(dst_filepath, D_BF_MDF_WRAPPER_ATTR_NORMAL);
		if( dst_file_id < 0 ) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Create() failed: file_id=%x, path=%s\n", dst_file_id, dst_filepath));
#endif
			g_bf_dcf_controller_ret_err = dst_file_id;
			break;
		}

		/* read and write */
		g_bf_dcf_controller_ret_err = dcf_controller_copy_read_write(org_file_id, dst_file_id, copy_data->load_addr, copy_data->load_area_size, org_file_stat.fileSize, org_media, copy_data->dst_media);

		/* close dst file */
		BF_Fs_Controller_Change_Drive_Info(copy_data->dst_media);
		BF_Mdf_Wrapper_Close(dst_file_id);

		if( g_bf_dcf_controller_ret_err != 0 ) {
			/* remove dst file */
			BF_Mdf_Wrapper_Remove(dst_filepath);
		}

		break;
	}

	/* close org file */
	BF_Fs_Controller_Change_Drive_Info(org_media);
	BF_Mdf_Wrapper_Close(org_file_id);

	//
	// option file(ex.voice memo)
	//
	if( (g_bf_dcf_controller_ret_err != 0) && (file_info.option_file == E_BF_DCF_IF_OPTION_FILE_EXIST) ){
		g_bf_dcf_controller_ret_err = dcf_controller_copy_option_file(copy_data->tgt_index, org_media, copy_data->dst_media, org_filepath, dst_filepath, copy_data->load_addr, copy_data->load_area_size);
	}

	/* end dst drive */
	BF_Fs_Controller_Change_Drive_Info(copy_data->dst_media);
	if(same_media_cpy_flag == 0){
		BF_Fs_Controller_End_Media(copy_data->dst_media);
	}

	/* change to org media */
	BF_Fs_Controller_Change_Drive_Info(org_media);

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "COPY DCF BETWEEN MEDIA\n");
#endif

	return g_bf_dcf_controller_ret_err;
}
#endif


/**
 * @brief get JPEG Quantization Table.
 * @param FW_VOID
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Get_Jpeg_Quant_Table(T_BF_DCF_IF_GET_QT_DATA* qt_data)
{
	FW_INT32 file_id;
	FW_CHAR open_filepath[64];

	if(qt_data == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}
	if(qt_data->qt == NULL){
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;
		return g_bf_dcf_controller_ret_err;
	}

	memset(qt_data->qt, 0, sizeof(T_BF_DCF_IF_JPEG_QT));

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Init(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT);
#endif

	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(qt_data->tgt_index, open_filepath, NULL);
	if(g_bf_dcf_controller_ret_err != 0) {
		return g_bf_dcf_controller_ret_err;
	}

	file_id = BF_Mdf_Wrapper_Open(open_filepath, D_BF_MDF_WRAPPER_O_RDONLY, D_BF_MDF_WRAPPER_ATTR_NORMAL);
	if( file_id == 0 ){
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Open() failed: file_id=%x, path=%s\n", file_id, open_filepath));
#endif
		g_bf_dcf_controller_ret_err = file_id;
		return g_bf_dcf_controller_ret_err;
	}

	switch(qt_data->qt_type){
		case E_BF_DCF_IF_QT_TYPE_MAIN:
//			g_bf_dcf_controller_ret_err = dcf_controller_get_jpeg_qt_main(file_id, qt_data->qt);
			g_bf_dcf_controller_ret_err = dcf_controller_get_jpeg_qt_data(file_id, E_BF_DCF_IF_QT_TYPE_MAIN, qt_data->qt);
			break;

		case E_BF_DCF_IF_QT_TYPE_THUMB:
//			g_bf_dcf_controller_ret_err = dcf_controller_get_jpeg_qt_thumb(file_id, qt_data->qt);
			g_bf_dcf_controller_ret_err = dcf_controller_get_jpeg_qt_data(file_id, E_BF_DCF_IF_QT_TYPE_THUMB, qt_data->qt);
			break;

		case E_BF_DCF_IF_QT_TYPE_VGA:
//			g_bf_dcf_controller_ret_err = dcf_controller_get_jpeg_qt_vga(file_id, qt_data->qt);
			g_bf_dcf_controller_ret_err = dcf_controller_get_jpeg_qt_data(file_id, E_BF_DCF_IF_QT_TYPE_VGA, qt_data->qt);
			break;

		default:
			g_bf_dcf_controller_ret_err = FSDCF_ERRNO_PARAM_FAILED;
			break;
	}
	if(g_bf_dcf_controller_ret_err != 0){
		BF_Mdf_Wrapper_Close(file_id);
		return g_bf_dcf_controller_ret_err;
	}

	g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Close(file_id);
	if(g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Close() failed: ercd=%d, file_id=%x\n", g_bf_dcf_controller_ret_err, file_id));
#endif
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
	BF_Fs_User_Custom_Measure_Time_Print(D_BF_DCF_CONTROLLER_E_FS_M_TIME_ID_DEFULT, "DCF GET JPEG QUANTIZATION TABLE\n");
#endif

	return g_bf_dcf_controller_ret_err;
}

/**
 * @brief 		Get file exist in the directory. 
 * @param		i:FW_SHORT dir_index
 * @param		o:UCHAR* file_exist
 * @return 		error code
 * @note		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_Controller_Db_Get_Dir_File_Exist(FW_USHORT dir_index, FW_UCHAR* file_exist)
{
	FW_INT32 ercd;
	FW_CHAR DirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_CHAR dir_path[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	T_BF_FS_IF_Dir_Info dir;
	FW_INT32 dNo;
	FW_UCHAR extTypeTmp;

	/* check parameter */
	if(file_exist == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	ercd = BF_Dcf_Db_Read_Dir_Name(dir_index, DirName);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* make directory path */
	strcpy(dir_path, (FW_CHAR*)g_bf_dcf_controller_dcim_path);
	strcat(dir_path, DirName);

	ercd = BF_Mdf_Wrapper_Opendir(dir_path, &dNo);
	if(FSDCF_ERRNO_OK != ercd) {
		BF_Mdf_Wrapper_Closedir(dNo);
		return ercd;
	}

	*file_exist = 0x00;

	for(;;) {
		ercd = BF_Mdf_Wrapper_Readdir(dNo, &dir);
		if(0 != ercd) {
			if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
				// end of object
				break;
			}
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

		if ((dir.attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
			*file_exist |= 0x02;
			continue;
		}

		/* file check */
		if(BF_Dcf_Db_Is_File_Name(dir.fname)) {
			ercd = BF_Dcf_Db_Get_Type_By_Ext(dir.fname + 9, &extTypeTmp);
			if(ercd == FSDCF_ERRNO_OK){
				*file_exist |= 0x01;
			}
			else if(ercd == FSDCF_ERRNO_NOT_SUPPORT_FILE){
				*file_exist |= 0x02;
			}
			else{
				BF_Mdf_Wrapper_Closedir(dNo);
				return ercd;
			}
		}
		else{
			*file_exist |= 0x02;
		}

		if(*file_exist == 0x03){
			break;
		}
	}
	BF_Mdf_Wrapper_Closedir(dNo);

	return FSDCF_ERRNO_OK;
}

/**
 * @brief 		Single DCF dorectory delete 
 * @param		i:FW_USHORT dir_index
 * @return 		error code
 * @note		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_Controller_Del_Single_Dir(FW_USHORT* dir_index)
{
	FW_INT32 ercd;
	FW_CHAR DirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_CHAR dir_path[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_UCHAR file_exist;
	FW_UCHAR attr;

	ercd = BF_Dcf_Db_Read_Dir_Name(*dir_index, DirName);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	/* make dir path */
	strcpy(dir_path, (FW_CHAR*)g_bf_dcf_controller_dcim_path);
	strcat(dir_path, DirName);

	ercd = BF_Fs_Controller_Get_Attribute(dir_path, &attr);
	if(ercd != 0) {
		return ercd;
	}

	if ((attr & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY) {
		return D_BF_MDF_WRAPPER_ERR_ROFS;
	}

	/* check file exist in directory */
	ercd = dcf_controller_db_check_dir_file_exist(*dir_index, &file_exist);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	if(file_exist == 0x00){
		/* directory delete */
		ercd = BF_Fs_Controller_Rmdir(dir_path);
		if(FSDCF_ERRNO_OK != ercd) {
			return ercd;
		}

	}
	else{
		return D_BF_MDF_WRAPPER_ERR_NOTEMPTY;
	}

	/* DCF-DB Update */
	ercd = BF_Dcf_Controller_DataBase_Create();
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	return FSDCF_ERRNO_OK;

}

/**
 * @brief 		All DCF dorectory delete in DCIM directory 
 * @param		None
 * @return 		error code
 * @note		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_Controller_Del_All_Dir(FW_VOID)
{
	FW_INT32 ercd;
	FW_CHAR DirName[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_CHAR dir_path[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FW_SHORT pdir_count;
	FW_SHORT dirIndex_del;
	FW_UCHAR file_exist;
	FW_UCHAR attr;

	ercd = BF_Dcf_Db_Read_Dir_Count(&pdir_count);
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	for(dirIndex_del = 1; dirIndex_del <= pdir_count; dirIndex_del++) {
		ercd = BF_Dcf_Db_Read_Dir_Name(dirIndex_del, DirName);
		if(FSDCF_ERRNO_OK != ercd) {
			continue;
		}

		/* make directory path */
		strcpy(dir_path, (FW_CHAR*)g_bf_dcf_controller_dcim_path);
		strcat(dir_path, DirName);

		ercd = BF_Fs_Controller_Get_Attribute(dir_path, &attr);
		if(ercd != 0) {
			return ercd;
		}

		if ((attr & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY) {
			continue;
		}

		/* check file exist in directory */
		ercd = dcf_controller_db_check_dir_file_exist(dirIndex_del, &file_exist);
		if(FSDCF_ERRNO_OK != ercd) {
			continue;
		}

		if(file_exist == 0x00){
			/* directory delete */
			BF_Fs_Controller_Rmdir(dir_path);
		}
	}
	/* DCF-DB Update */
	ercd = BF_Dcf_Controller_DataBase_Create();
	if(FSDCF_ERRNO_OK != ercd) {
		return ercd;
	}

	return FSDCF_ERRNO_OK;

}

#if 1	/* for Multi-Delete */
FW_INT32 BF_Dcf_Controller_Init_Select_List(FW_VOID)
{
	g_bf_dcf_controller_select_list_index = 0;						/*	Initialize the Select List Index				*/
	g_bf_dcf_controller_select_list_num   = 0;						/*	Initialize the Selected file num				*/
	g_bf_dcf_controller_select_list_free_index = 0;

	return 0;
}


FW_INT32 BF_Dcf_Controller_Add_File_To_Select_List(T_BF_DCF_IF_FILE_INDEX* dcf_index)
{
	FW_INT32			ercd;							/*	Error Code										*/
	FW_UCHAR			index;							/*	Index for SelectList							*/
	T_BF_DCF_IF_DCFDB_OBJ_INFO	obj_info;

	if ((dcf_index->pack.dir_index == 0) || (dcf_index->pack.obj_index == 0)) {
		/* Invalid ImageIndex */
		return -1;
	}

	if (D_BF_DCF_CONTROLLER_SELECT_LIST_SIZE <= g_bf_dcf_controller_select_list_num) {
		/* list over */
		return -1;
	}

	ercd = dcf_controller_check_image_select(dcf_index, &index);
	if (ercd != -1) {
		/* already selected */
		return 0;
	}

	ercd = BF_Dcf_Controller_Read_Obj_Inf(&dcf_index->index, &obj_info);
	if (ercd != 0) {
		/* obj_info is not exists on DCF-DB */
		return -1;
	}

#if 0
	if ((obj_info.attr & 1) == 1) {
		/* ReadOnly Object -> Cannot delete */
		return -1;
	}
#endif

	if (g_bf_dcf_controller_select_list_num < g_bf_dcf_controller_select_list_index) {
		g_bf_dcf_controller_select_list_free_index--;
		index = g_bf_dcf_controller_select_list_free_index_array[g_bf_dcf_controller_select_list_free_index];
		g_bf_dcf_controller_select_list[index].index = dcf_index->index;
		g_bf_dcf_controller_select_list_num++;
#if 0
		for (index = 0; index < g_bf_dcf_controller_select_list_index; index++) {
			if (g_bf_dcf_controller_select_list[index].index == 0) {
				g_bf_dcf_controller_select_list[index].index = dcf_index.index;
				g_bf_dcf_controller_select_list_num++;
				return 0;
			}
		}
#endif
	}
	else {
		g_bf_dcf_controller_select_list[g_bf_dcf_controller_select_list_index].index = dcf_index->index;
		g_bf_dcf_controller_select_list_index++;
		g_bf_dcf_controller_select_list_num++;
	}

	return 0;
}

FW_INT32 BF_Dcf_Controller_Remove_File_From_Select_List(T_BF_DCF_IF_FILE_INDEX* dcf_index)
{
	FW_INT32 ercd;
	FW_UCHAR list_index;							/*	Index for SelectList							*/

	if ((dcf_index->pack.dir_index == 0) || (dcf_index->pack.obj_index == 0)) {
		/* Invalid Dcf Index */
		return -1;
	}

	ercd = dcf_controller_check_image_select(dcf_index, &list_index);
	if (ercd == -1) {
		/* not selected */
		return 0;
	}

	g_bf_dcf_controller_select_list[list_index].index = 0xFFFFFFFF;
	g_bf_dcf_controller_select_list_num--;
	if (list_index == g_bf_dcf_controller_select_list_index - 1) {
		g_bf_dcf_controller_select_list_index--;
	}
	else {
		g_bf_dcf_controller_select_list_free_index_array[g_bf_dcf_controller_select_list_free_index] = list_index;
		g_bf_dcf_controller_select_list_free_index++;
	}

	return 0;
}
#endif


#if 1	/* for Multi-Delete */
/**
 * @brief delete select files.
 * @param i:*pSelectList selected files list
 *        i:cSelectNum   selected file num
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_Controller_Delete_Selected_File(T_BF_DCF_IF_FILE_INDEX* pSelectList, FW_UCHAR cSelectNum)
{
	FW_INT32			ercd;
	FW_CHAR				del_filepath[64];
	FW_SHORT			dcf_current_dir_index;							/*	DCF Current directory index					*/
	FW_INT32			list_index;										/*	pSelectList index							*/
	FW_CHAR				del_dir_path[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];		/*	Delete directory path(Full Path)			*/
	FW_CHAR				del_dir_name[D_BF_DCF_IF_DIR_NAME_MAX];				/*	Delete directory name						*/
	FW_CHAR				drive_name[D_BF_FS_IF_DRVNAME_MAX+1];			/*	Drive name									*/
	FW_CHAR				delete_flag;									/*	Delete flag (0:not delete/1:delete)			*/
	FW_INT32			dir_id;											/*	Directory ID						(MDF)	*/
	T_BF_FS_IF_Dir_Info		dir_entry_info;									/*	Directory entry information			(MDF)	*/
	T_BF_DCF_IF_FILE_INFO	file_info;										/*	File information				(DCF-DB)	*/
	T_BF_DCF_IF_FILE_INDEX	file_index;										/*	File Index						(DCF-DB)	*/
	FW_INT32			dir_len;
	FW_CHAR				del_file_path[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];		/*	Delete file path(Full Path)			*/

	if (g_bf_dcf_controller_display_file_id != 0) {
		BF_Dcf_Controller_Close_Display_File();
	}

	/* Parameter check */
	if (pSelectList == NULL) {
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_TOUCH_NULL_POINTER;					/*	Null Pointer Error							*/
		return g_bf_dcf_controller_ret_err;
	}

	if (cSelectNum == 0) {

		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_OK;
		return g_bf_dcf_controller_ret_err;
	}

	/* Current Directory check */
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Dir_Index_Num(DCF_DB_CURRENT_DIR, &dcf_current_dir_index);
	if (g_bf_dcf_controller_ret_err != FSDCF_ERRNO_OK) {
		return g_bf_dcf_controller_ret_err;
	}

	if (pSelectList->pack.dir_index != dcf_current_dir_index) {
		/* Parameter error */
		g_bf_dcf_controller_ret_err = FSDCF_ERRNO_PARAM_FAILED;
		return g_bf_dcf_controller_ret_err;
	}

	/* Get directory name with DirIndex */
	g_bf_dcf_controller_ret_err = BF_Dcf_Db_Read_Dir_Name(DCF_DB_CURRENT_DIR, del_dir_name);
	if (g_bf_dcf_controller_ret_err != FSDCF_ERRNO_OK) {
		return g_bf_dcf_controller_ret_err;
	}

	/* Dir Path */
	strcpy(del_dir_path, (FW_CHAR*)g_bf_dcf_controller_dcim_path);
	strcat(del_dir_path, del_dir_name);

	strcpy(del_file_path, del_dir_path);
	strcat(del_file_path, "\\");
	dir_len = strlen(del_file_path);

	ercd = BF_Mdf_Wrapper_Opendir(del_dir_path, &dir_id);
	if (FSDCF_ERRNO_OK != ercd) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Opendir() failed: ercd=%d, dirname=%s\n", ercd, del_dir_path));
#endif

		BF_Mdf_Wrapper_Closedir(dir_id);
		return ercd;
	}

	if (cSelectNum != 1) {
		/* Remove two or more files */
		while (1) {
			/* Read the entry information */
			ercd = BF_Mdf_Wrapper_Readdir(dir_id, &dir_entry_info);
			if(0 != ercd) {
				/* Check Entry Last */
				if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
					// end of object
					break;
				}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif

				BF_Mdf_Wrapper_Closedir(dir_id);
				g_bf_dcf_controller_ret_err = ercd;
//				break;
				return g_bf_dcf_controller_ret_err;
			}

			if ('.' == dir_entry_info.fname[0]) {
				if (0 == dir_entry_info.fname[1]) {
					
					continue;
				}

				if (('.' == dir_entry_info.fname[1]) && (0 == dir_entry_info.fname[2])) {
					
					continue;
				}
			}
			if ((dir_entry_info.attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
				/* Directory is exempted */
				continue;
			}

			/* Check the file name matches the DCF rules */
			/* search object */
			if (0 != BF_Dcf_Db_Search_Obj_By_Name(del_dir_name, dir_entry_info.fname, &(file_index.pack.dir_index), &(file_index.pack.obj_index))) {
				continue;
			}

			if (0 != BF_Dcf_Db_Get_Index_File_Info(file_index, NULL, &file_info)) {
				continue;
			}
			if (file_info.file_attr == E_BF_DCF_IF_ATTR_RDONLY) {
				/* Read Only Object */
				continue;
			}

			/* case of DCF object */
			/* Check the same Index in pSelectList */
			delete_flag = 0;
			for (list_index = 0; list_index < cSelectNum; list_index++) {
				if (file_index.index == pSelectList[list_index].index) { /* pgr0872 */
					delete_flag = 1;
					break;
				}
			}

			if (delete_flag == 0) {
				
				continue;
			}


#if 0	// MDF custom
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Current_Entry(dir_id, &dir_entry_info);
#else
			strcpy(del_file_path + dir_len, dir_entry_info.fname);
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache((const FW_CHAR*)del_file_path);
#endif
			if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
#if 0	// MDF custom
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Current_Entry() failed: ercd=%d, path=%s, file=%s\n", g_bf_dcf_controller_ret_err, del_dir_path, dir_entry_info.fname));
#else
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s, file=%s\n", g_bf_dcf_controller_ret_err, del_dir_path, dir_entry_info.fname));
#endif
#endif

//				BF_Mdf_Wrapper_Closedir(dir_id);
//				return g_bf_dcf_controller_ret_err;
				break;
			}
		}

		ercd = BF_Mdf_Wrapper_Closedir(dir_id);
		if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
			return ercd;
		}

		/* Get drive name */
		ercd = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
		if (ercd != 0) {
			return ercd;
		}

		/* updata freesize cache */
		ercd = BF_Mdf_Wrapper_Sync(drive_name);
		if (ercd != 0) {
			return ercd;
		}

		/* updata Dcf-DB */
#if 1
		if (g_bf_dcf_controller_ret_err != 0) {
			/* Restruct Dcf-DB */
			BF_Dcf_Db_Restruct_Current_Dir_Only();
		}
		else {
			/* Remove Databace information Remove the Object */
			g_bf_dcf_controller_ret_err = BF_Dcf_Db_Delete_Multi_Obj_In_Current_Dir(pSelectList, cSelectNum);
		}
#else
		/* DCF-DB Clear */
		BF_Dcf_Db_Clear_Db_buf();

		/* Re-setting the dcf_current_dir_index */
		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Set_Current_Dir(dcf_current_dir_index);
#endif
	}
	else {
		/* case of one file */
		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Get_Index_File_Info(*pSelectList, del_filepath, &file_info);
		if(g_bf_dcf_controller_ret_err != 0) {
			return g_bf_dcf_controller_ret_err;
		}
		if (file_info.file_attr == E_BF_DCF_IF_ATTR_RDONLY) {
			/* Read Only Object */
			g_bf_dcf_controller_ret_err = D_BF_MDF_WRAPPER_ERR_ROFS;
			return g_bf_dcf_controller_ret_err;
		}

		while (1) {
			/* Read the entry information */
			ercd = BF_Mdf_Wrapper_Readdir(dir_id, &dir_entry_info);
			if(0 != ercd) {
				/* Entry Last */
				if(D_BF_MDF_WRAPPER_ENT_LAST == ercd) {
					// end of object
					break;
				}

#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Readdir() failed: ercd=%d\n", ercd));
#endif

				BF_Mdf_Wrapper_Closedir(dir_id);
				g_bf_dcf_controller_ret_err = ercd;
//				break;
				return g_bf_dcf_controller_ret_err;
			}

			if ('.' == dir_entry_info.fname[0]) {
				if (0 == dir_entry_info.fname[1]) {
					
					continue;
				}

				if (('.' == dir_entry_info.fname[1]) && (0 == dir_entry_info.fname[2])) {
					
					continue;
				}
			}
			if ((dir_entry_info.attributes & D_BF_FS_IF_ATTR_DIRECTORY) == D_BF_FS_IF_ATTR_DIRECTORY) {
				/* Directory */
				continue;
			}

			/* Check the file name matches the DCF rules */
			/* search object */
			if (0 != BF_Dcf_Db_Search_Obj_By_Name(del_dir_name, dir_entry_info.fname, &(file_index.pack.dir_index), &(file_index.pack.obj_index))) {
				continue;
			}
			
			/* DCF */
			if (file_index.index != pSelectList->index) { /* pgr0872 */
				continue;
			}


#if 0	// MDF custom
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Current_Entry(dir_id, &dir_entry_info);
#else
			strcpy(del_file_path + dir_len, dir_entry_info.fname);
			g_bf_dcf_controller_ret_err = BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache((const FW_CHAR*)del_file_path);
#endif
			if (g_bf_dcf_controller_ret_err != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
#if 0	// MDF custom
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Current_Entry() failed: ercd=%d, path=%s, file=%s\n", g_bf_dcf_controller_ret_err, del_dir_path, dir_entry_info.fname));
#else
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache() failed: ercd=%d, path=%s, file=%s\n", g_bf_dcf_controller_ret_err, del_dir_path, dir_entry_info.fname));
#endif
#endif

//				BF_Mdf_Wrapper_Closedir(dir_id);
//				return g_bf_dcf_controller_ret_err;
				break;
			}
		}

		ercd = BF_Mdf_Wrapper_Closedir(dir_id);
		if(ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Closedir() failed: ercd=%d\n", ercd));
#endif
			return ercd;
		}

		/* Get Drive name */
		ercd = BF_Media_State_Exchage_Type_To_Drive_Name(BF_Media_State_Get_Used_Media(), drive_name);
		if (ercd != 0) {
			return ercd;
		}

		/* updata freesize cache */
		ercd = BF_Mdf_Wrapper_Sync(drive_name);
		if (ercd != 0) {
			return ercd;
		}


		/* updata Dcf-DB */
		g_bf_dcf_controller_ret_err = BF_Dcf_Db_Delete_File(*pSelectList, del_filepath);

	}
	return g_bf_dcf_controller_ret_err;
}




FW_INT32 BF_Dcf_Controller_Execute_Multi_Delete(FW_VOID)
{
	FW_INT32	ercd;							/*	Error Code										*/
	FW_SHORT	cur_dir_index_first;			/*	DCF Current Directory Index at first			*/
	FW_SHORT	now_dir_index;					/*	DCF Current Directory Index at now				*/
	FW_UCHAR	del_obj_cnt_dir;				/*	The number of files to delete in the directory	*/
	FW_UCHAR	deleted_obj_num;				/*	The number of files already deleted				*/
	FW_UCHAR	index;

	if ((g_bf_dcf_controller_select_list_index == 0) || (g_bf_dcf_controller_select_list_num == 0)) {
		/* No selected file */
		BF_Dcf_Controller_Init_Select_List();
		return 0;
	}

	/* Sort the SelectList by directory index */
	dcf_controller_sort_select_list_by_dir_index();

	/* get current directory index number */
	ercd = BF_Dcf_Db_Get_Dir_Index_Num(DCF_DB_CURRENT_DIR, &cur_dir_index_first);
	if (ercd != 0) {
		return ercd;
	}

	deleted_obj_num = 0;
	for (;;) {
		now_dir_index = g_bf_dcf_controller_select_list[deleted_obj_num].pack.dir_index;
		del_obj_cnt_dir = 1;
		for (index = deleted_obj_num + 1; index < g_bf_dcf_controller_select_list_num; index++) {
			/* Count the number of selected object on same directory */
			if (now_dir_index != g_bf_dcf_controller_select_list[index].pack.dir_index) {
				break;
			}
			del_obj_cnt_dir++;
		}

		/* Set current directory of DCF-DB */
		ercd = BF_Dcf_Db_Set_Current_Dir(now_dir_index);
		if (ercd != 0) {
			break;
		}

		/* Delete selected files in the current directory */
		ercd = BF_Dcf_Controller_Delete_Selected_File((T_BF_DCF_IF_FILE_INDEX*)&g_bf_dcf_controller_select_list[deleted_obj_num], del_obj_cnt_dir);
		if (ercd != 0) {
			break;
		}

		/* Count up the total number of deleted file */
		deleted_obj_num += del_obj_cnt_dir;
		if (deleted_obj_num >= g_bf_dcf_controller_select_list_num) {
			/* All selected files were deleted */
			break;
		}
	}

	/* Reset current directory */
	ercd = BF_Dcf_Db_Set_Current_Dir(cur_dir_index_first);

	/* Initialize to SelectList */
	BF_Dcf_Controller_Init_Select_List();

	return ercd;
}
#endif

#ifdef CO_A_FILE_NAME_LIST	// CO_A_FILE_NAME_LIST
/**
Initialize file name list information.
@retval error code
*/
FW_VOID BF_Dcf_Controller_Init_File_Name_List( FW_VOID )
{
	dcf_controller_init_file_name_list();
}
#endif	// A CO_A_FILE_NAME_LIST  END

