/**
 * @file		fs_controller.h
 * @brief		FileSystem scenario control without depend on DCF(header).
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FS_CONTROL_H_
#define _FS_CONTROL_H_

#include "media_state.h"
#include "fs_if.h"
#include "media_drv_wrapper.h"
#include "fj_filesystem.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_BF_FS_CONTROLLER_NF_0					FJ_MEDIA_NF_STORAGE0	//	0	//	
#define D_BF_FS_CONTROLLER_NF_1					FJ_MEDIA_NF_STORAGE1	//	1   //	
#define D_BF_FS_CONTROLLER_NF_2					FJ_MEDIA_NF_STORAGE2	//	2   //	
#define D_BF_FS_CONTROLLER_NF_3					FJ_MEDIA_NF_STORAGE3	//	3   //	
#define D_BF_FS_CONTROLLER_NF_4					FJ_MEDIA_NF_STORAGE4	//	4   //	
#define D_BF_FS_CONTROLLER_NF_5					FJ_MEDIA_NF_STORAGE5	//	5   //	
#define D_BF_FS_CONTROLLER_NF_6					FJ_MEDIA_NF_STORAGE6	//	6   //	
#define D_BF_FS_CONTROLLER_SD_0					FJ_MEDIA_SD0			//	7   //	
#define D_BF_FS_CONTROLLER_SD_1					FJ_MEDIA_SD1			//	8   //	
#define D_BF_FS_CONTROLLER_EMMC_0				FJ_MEDIA_EMMC_STORAGE0	//	9   //	
#define D_BF_FS_CONTROLLER_EMMC_1				FJ_MEDIA_EMMC_STORAGE1	//	10  //	
#define D_BF_FS_CONTROLLER_EMMC_2				FJ_MEDIA_EMMC_STORAGE2	//	11  //	
#define D_BF_FS_CONTROLLER_EMMC_3				FJ_MEDIA_EMMC_STORAGE3	//	12  //	
#define D_BF_FS_CONTROLLER_EMMC_4				FJ_MEDIA_EMMC_STORAGE4	//	13  //	
#define D_BF_FS_CONTROLLER_EMMC_5				FJ_MEDIA_EMMC_STORAGE5	//	14  //	
#define D_BF_FS_CONTROLLER_EMMC_6				FJ_MEDIA_EMMC_STORAGE6	//	15  //	
#define D_BF_FS_CONTROLLER_MEDIA_ID_NUM			(D_BF_FS_CONTROLLER_EMMC_6+1)	//	16	//

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/**
 * @breif Returned data for BF_Fs_If_Receiver_Statvfs
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	T_BF_FS_IF_STATVFS*	buf;
} T_FS_CONTROLLER_RET_STATVFS_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Get_Cur_Drv
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR*		drvname;
} T_FS_CONTROLLER_RET_GET_CURDRV_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Stat
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	T_BF_FS_IF_STAT*	sbuf;
} T_FS_CONTROLLER_RET_STAT_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Get_Attribute
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_UCHAR*		attributes;
} T_FS_CONTROLLER_RET_GET_ATTR_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Read
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_ULONG*		length;
} T_FS_CONTROLLER_RET_TYPE_READ_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Write
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_ULONG*		length;
} T_FS_CONTROLLER_RET_TYPE_WRITE_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Write_Result
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_ULONG*		length;
} T_FS_CONTROLLER_RET_TYPE_WRITE_RESULT_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Lseek
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_ULONG*		length;
} T_FS_CONTROLLER_RET_TYPE_LSEEK_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Lseek64
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_INT64*		length;
} T_FS_CONTROLLER_RET_TYPE_LSEEK64_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Fstat
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	T_BF_FS_IF_STAT*	sbuf;
} T_FS_CONTROLLER_RET_TYPE_FSTAT_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Opendir
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_INT32*		dNo;
} T_FS_CONTROLLER_RET_TYPE_OPENDIR_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Readdir
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	T_BF_FS_IF_Dir_Info*	fsdirent;
} T_FS_CONTROLLER_RET_TYPE_READDIR_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Get_Cur_Dir
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR*		curpath;
} T_FS_CONTROLLER_RET_TYPE_GET_CURDIR_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Get_FreeSize
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_DOUBLE*		free_size;
} T_FS_CONTROLLER_RET_GET_FREESIZE_DAT;

/**
 * @breif Returned data for FsRcv_To_Unicode
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR*		unicode;
	FW_ULONG*		length;
} T_FS_CONTROLLER_RET_TYPE_TO_UNI_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Get_Media_Format_Type
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_UCHAR*		used_media;
	FW_UCHAR*		format_type;
} T_FS_CONTROLLER_RET_GET_MEDIA_FORMAT_DAT;

/**
 * @breif media status
 * @note none
 * @attention none
 */
/* media status */
typedef struct {
	E_BF_MEDIA_STATE_MEDIA_TYPE	media_type;
	FW_CHAR			devid[D_BF_FS_IF_DRVNAME_MAX];		// device ID
	FW_CHAR			drvname[12];	// drive name
	T_FS_FUNCNAME*	driver_tbl;		// ptr to driver table
	FW_VOID*		media_info;		// inner parameter for media driver
	FW_INT32		rwmax;			// max sector cnt for one time driver call
	E_BF_MEDIA_DRV_WRAPPER_WP	wp;	// write protect
} T_FS_CONTROLLER_MEDIA_STATUS;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Set_Time
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_UCHAR timestamp_type;
	T_BF_FS_IF_TIME*	utime;
} T_FS_CONTROLLER_RET_SET_TIME_DAT;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Get_VolLabel
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR*		vollabel;
} T_FS_CONTROLLER_RET_GET_VOL_LABEL;

/**
 * @breif Returned data for BF_Fs_If_Receiver_Fsfirst/BF_Fs_If_Receiver_Fsnext
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	T_BF_FS_IF_ENTRY*	buf;
} T_FS_CONTROLLER_RET_ENTRY;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
extern volatile T_FS_CONTROLLER_MEDIA_STATUS gBF_Fs_Controller_Media_Status[D_BF_FS_CONTROLLER_MEDIA_ID_NUM];
extern volatile UINT32 gBF_Controller_media_id;
extern volatile CHAR gBF_Controller_drv_name[D_BF_FS_CONTROLLER_MEDIA_ID_NUM][D_BF_FS_IF_DRVNAME_MAX];

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
// romarea_boot
extern FW_INT32      BF_Fs_Controller_End_Task_MDF(FW_VOID);	// for DCF-task
extern FW_INT32      BF_Fs_Controller_Init(FW_VOID);
extern FW_INT32      BF_Fs_Controller_Format(FW_INT32 media_id);
extern FW_INT32      BF_Fs_Controller_QuickFormat(FW_CHAR* drv_name);
extern FW_INT32      BF_Fs_Controller_Get_Format_Progress(FW_LONG* format_progress);
extern FW_INT32      BF_Fs_Controller_Start_Media(FW_INT32 media_id);
extern FW_INT32      BF_Fs_Controller_End_Media(FW_INT32 media_id);
extern FW_INT32      BF_Fs_Controller_Statvfs(FW_CHAR* drvname, T_BF_FS_IF_STATVFS* buf);
extern FW_INT32      BF_Fs_Controller_Set_VolLabel(FW_CHAR* drvname, FW_CHAR* label);
extern FW_INT32      BF_Fs_Controller_Get_VolLabel(FW_CHAR* drvname, FW_CHAR* label);
extern FW_INT32      BF_Fs_Controller_Stat(FW_CHAR* fname, T_BF_FS_IF_STAT* sbuf);
extern FW_INT32      BF_Fs_Controller_Get_Attribute(FW_CHAR* fname, FW_UCHAR* attributes);
extern FW_INT32      BF_Fs_Controller_Open(FW_CHAR* fname, FW_USHORT mode, FW_USHORT amode);
extern FW_INT32      BF_Fs_Controller_Close(FW_INT32 fNo);
extern FW_INT32      BF_Fs_Controller_Read(FW_INT32 fNo, FW_ULONG addr, FW_ULONG count, FW_ULONG* length);
extern FW_INT32      BF_Fs_Controller_Write(FW_INT32 fNo, FW_ULONG addr, FW_ULONG count, FW_ULONG* length);
extern FW_INT32      BF_Fs_Controller_Write_Result(FW_ULONG* length);
extern FW_INT32      BF_Fs_Controller_Lseek(FW_INT32 fNo, FW_INT32 offset, FW_USHORT whence, FW_ULONG* length);
extern FW_INT32      BF_Fs_Controller_Lseek64(FW_INT32 fNo, FW_INT64 offset, FW_USHORT whence, FW_INT64* length);
extern FW_INT32      BF_Fs_Controller_Create(FW_CHAR* fname, FW_USHORT amode);
extern FW_INT32      BF_Fs_Controller_Copy(FW_CHAR* srcname, FW_CHAR* dstname);
extern FW_INT32      BF_Fs_Controller_Remove(FW_CHAR* fname);
extern FW_INT32      BF_Fs_Controller_Remove_Multi(const FW_CHAR* dirname, FW_CHAR* filename_list, FW_INT32 fnum, FS_DEL_MULTI_CB pcallback);
extern FW_INT32      BF_Fs_Controller_Fstat(FW_INT32 fNo, T_BF_FS_IF_STAT* sbuf);
extern FW_INT32      BF_Fs_Controller_Mkdir(FW_CHAR* dirname);
extern FW_INT32      BF_Fs_Controller_Rmdir(FW_CHAR* dirname);
extern FW_INT32      BF_Fs_Controller_Opendir(FW_CHAR* dirname, FW_INT32* dNo);
extern FW_INT32      BF_Fs_Controller_Readdir(FW_INT32 dNo, T_BF_FS_IF_Dir_Info* fsdirent);
extern FW_INT32      BF_Fs_Controller_Closedir(FW_INT32 dNo);
extern FW_INT32      BF_Fs_Controller_Chdir(FW_CHAR* path);
extern FW_INT32      BF_Fs_Controller_Get_Cur_Dir(FW_CHAR* drvname, FW_ULONG size, FW_CHAR* curpath);
extern FW_INT32      BF_Fs_Controller_Set_Protection_Mode(FW_CHAR* drvname, FW_UCHAR wrt_protect);
extern E_BF_MEDIA_STATE_MEDIA_STATE BF_Fs_Controller_Get_Media_State_By_Poll(FW_UCHAR ch, FW_UCHAR interval_time, FW_UCHAR check_count);
extern FW_INT32      BF_Fs_Controller_Set_Media_State(FW_UCHAR ch, FW_UCHAR interval_time, FW_UCHAR check_count);
extern E_BF_MEDIA_STATE_MEDIA_STATE BF_Fs_Controller_Get_Media_State(FW_UCHAR ch);
extern E_BF_MEDIA_STATE_MEDIA_STATE BF_Fs_Controller_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);
extern E_BF_MEDIA_STATE_MEDIA_TYPE  BF_Fs_Controller_Get_Cur_Media(FW_VOID);
extern FW_INT32      BF_Fs_Controller_Get_FreeSize(E_BF_MEDIA_STATE_MEDIA_TYPE media_type, FW_DOUBLE* free_size);
extern FW_INT32      BF_Fs_Controller_Check_Mbr(FW_VOID);
extern FW_INT32      BF_Fs_Controller_Make_Mbr(FW_VOID);
extern FW_INT32      BF_Fs_Controller_Set_Media_DMA(FW_UCHAR enable);
extern FW_INT32      BF_Fs_Controller_Set_Vfs_Write_Err_Cb( FW_UINT32 fNo, FS_SET_WRITE_ERR_CB pCallback );

// romarea_comm
extern FW_INT32      BF_Fs_Controller_Copy_File_To_Drive(FW_CHAR* szSrcDrive, FW_CHAR* szDstDrive);

// romarea_usb
extern FW_INT32      BF_Fs_Controller_End_Drv(FW_VOID);
//extern FW_INT32      BF_Fs_Controller_Init_Drv(FW_VOID);
extern FW_INT32      BF_Fs_Controller_Init_Drv(FW_UCHAR mount);

// romarea_last
extern FW_INT32      BF_Fs_Controller_Get_Cur_Drv(FW_CHAR* drvname);
extern FW_INT32      BF_Fs_Controller_Sync(FW_CHAR* drvname);
extern FW_INT32      BF_Fs_Controller_Fsync(FW_INT32 fNo);
extern FW_INT32      BF_Fs_Controller_Rename(FW_CHAR* srcname, FW_CHAR* dstname);
extern FW_INT32      BF_Fs_Controller_Set_Attribute(FW_CHAR* fname, FW_UCHAR attributes);
extern FW_INT32      BF_Fs_Controller_Set_Attribute_Multi(const FW_CHAR* dirname, FW_UCHAR attributes, FW_UCHAR attr_mode, FW_CHAR* filename_list, FW_INT32 fnum, FS_SET_ATTR_MULTI_CB pcallback);
extern E_BF_MEDIA_STATE_MEDIA_WP    BF_Fs_Controller_Get_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);
extern FW_INT32      BF_Fs_Controller_Change_Media(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);
extern FW_INT32      BF_Fs_Controller_Init_SD0_For_Copy(FW_VOID);
extern FW_INT32      BF_Fs_Controller_Init_SD1_For_Copy(FW_VOID);
extern FW_INT32      BF_Fs_Controller_End_SD_For_Copy(FW_VOID);
extern FW_INT32      BF_Fs_Controller_Drive_To_Drive_Copy(FW_CHAR* szSrcDrive, FW_CHAR* szDstDrive);
extern FW_INT32      BF_Fs_Controller_Format_Drive(FW_INT32 drive_no);
extern FW_INT32      BF_Fs_Controller_Date_Time_Sync(FW_CHAR* org_file_path, FW_CHAR* sync_file_ext, T_BF_FS_utimbuf fs_utim);
extern FW_INT32      BF_Fs_Controller_Get_Media_Format_Type(FW_UCHAR* used_media, FW_UCHAR* format_type);
extern FW_INT32      BF_Fs_Controller_Cancel(FW_VOID);
extern FW_INT32      BF_Fs_Controller_Change_Drive_Info(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);
extern FW_INT32      BF_Fs_Controller_Set_Lseek_Write(FW_INT32 fNo, FW_INT32 offset, FW_ULONG addr, FW_ULONG count, FW_ULONG* length);
extern FW_INT32      BF_Fs_Controller_Set_Lseek_Write_Result(FW_ULONG* length);
extern FW_INT32      BF_Fs_Controller_Set_Time(FW_CHAR* fname, FW_USHORT timestamp_type, T_BF_FS_IF_TIME* utime);
extern VOID 		 BF_Fs_Controller_Get_Media_Id(CHAR* drvname, CHAR* bMediaId);
extern FW_INT32		 BF_Fs_Controller_Set_User_Setting( FW_ULONG addr, FW_ULONG count );

extern FW_INT32      BF_Fs_Controller_Fsfirst(FW_ULONG index, const char* fname, FW_USHORT attr, T_BF_FS_IF_ENTRY* buf);
extern FW_INT32      BF_Fs_Controller_Fsnext(FW_ULONG index, T_BF_FS_IF_ENTRY* buf);
#endif
