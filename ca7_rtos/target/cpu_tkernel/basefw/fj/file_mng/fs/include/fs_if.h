/**
 * @file		fs_if.h
 * @brief		User interface of FileSystem function without depend on DCF(header).
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FS_IF_H_
#define _FS_IF_H_

#include "media_state.h"
#include "fsdcf_errno.h"


/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_BF_FS_IF_DRVNAME_MAX		(9)					// max length of drive name
#define D_BF_FS_IF_VOLLABEL_MAX		(12)				// max length of volume label
#define D_BF_FS_IF_FNAME_MAX		(528)				// max length of file name

/***	attribute	***/
#define D_BF_FS_IF_ATTR_NORMAL		(0x00)				// file attribute normal
#define D_BF_FS_IF_ATTR_READONLY	(0x01)				// file attribute read-only
#define D_BF_FS_IF_ATTR_HIDDEN		(0x02)				// file attribute hidden
#define D_BF_FS_IF_ATTR_SYSTEM		(0x04)				// file attribute system
#define D_BF_FS_IF_ATTR_DIRECTORY	(0x10)				// file attribute directory
#define D_BF_FS_IF_ATTR_ARCHIVE		(0x20)				// file attribute archive
#define D_BF_FS_IF_ATTR_NONE		(0x40)				// file attribute none
#define D_BF_FS_IF_ATTR_ALL			(0x7F)				// file attribute all
#define D_BF_FS_IF_ATTR_ENTRY		(0x77)				// without hidden/system

/***	attribute mode for setting ***/
#define D_BF_FS_IF_ATTR_MODE_SET	(0)   				/* Attribute set 	*/
#define D_BF_FS_IF_ATTR_MODE_ON		(1)   				/* Attribute add 	*/
#define D_BF_FS_IF_ATTR_MODE_OFF	(2)   				/* Attribute remove */

/***	open mode	***/
#define D_BF_FS_IF_O_RDONLY			(0x0000)			// Only enable the read access
#define D_BF_FS_IF_O_WRONLY			(0x0001)			// Only enable the write access
#define D_BF_FS_IF_O_RDWR			(0x0002)			// Enable the read/write access
#define D_BF_FS_IF_O_APPEND			(0x0008)			// After open the file, move the write operation file pointer to the end of file
#define D_BF_FS_IF_O_CREAT			(0x0100)			// Create new file when no file is existed
#define D_BF_FS_IF_O_TRUNC			(0x0200)			// Set the file size to "0" when opening file

/***	seek mode	***/
#define D_BF_FS_IF_SEEK_CUR			(1)					// seek from current
#define D_BF_FS_IF_SEEK_END			(2)					// seek from file end
#define D_BF_FS_IF_SEEK_SET			(0)					// seek from file top


/***	copy destination drive name	***/
#define D_BF_FS_IF_DST_DRIVE_NAME			'H'					// Copy Destination Drive(SD) name
#define D_BF_FS_IF_DST_DRIVE_NAME_STRING	"H"					// Copy Destination Drive(SD) name string

/***	copy destination drive name	***/
#define D_BF_FS_IF_SD_DRIVE_NAME			'I'					// Copy Source or Destination Drive name
#define D_BF_FS_IF_SD_DRIVE_NAME_STRING		"I"					// Copy Source or Destination Drive name string

/***	copy destination drive name	***/
#define D_BF_FS_IF_NF_DRIVE_NAME			'J'					// Copy Source Drive(NF) name
#define D_BF_FS_IF_NF_DRIVE_NAME_STRING		"J"					// Copy Source Drive(NF) name string

#define D_BF_FS_IF_NOT_MOUNT_I		(0x00)		// I drive not mount
#define D_BF_FS_IF_MOUNT_I			(0x01)		// I drive mount

/***	function for multi files ***/
typedef FW_INT32 (*FS_DEL_MULTI_CB)(FW_INT32, FW_INT32);
typedef FW_INT32 (*FS_SET_ATTR_MULTI_CB)(FW_INT32, FW_INT32);

/***    function for writing error ***/
typedef FW_INT32 (*FS_SET_WRITE_ERR_CB)(FW_INT32, FW_INT32);
/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/**
 * @brief drive information.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_ULONG		blksize;						// ドライブのクラスタサイズ(バイト)
	FW_ULONG		blocks;							// ドライブ全体のサイズ(blksize数)
	FW_ULONG		bfree;							// ドライブの空きクラスタ数(blksize数)
	FW_ULONG		bbad;							// ドライブ内の不良クラスタ数
	FW_ULONG		dstat;							// ドライブ情報・ドライブの状態
	FW_USHORT		fstype;							// ファイルシステム種類
	FW_USHORT		fsvers;							// ファイルシステムバージョン
	FW_USHORT		tasks;							// 対象ドライブをカレントドライブとするタスクの数
	FW_USHORT		entries;						// オープンしているエントリの数
	FW_UCHAR*		mdfvers;						// MDFバージョン
} T_BF_FS_IF_STATVFS;

/**
 * @brief file information.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_CHAR			fname[766];						// ディレクトリ、ファイル名
	FW_CHAR			dosfname[13];					// DOS-ディレクトリ、ファイル名+NULL
	FW_UCHAR		attributes;						// ディレクトリ、ファイル属性
	FW_ULLONG		fileSize;						// ファイルサイズ(64bit)
	FW_ULLONG		validSize;						// 有効データサイズ(64bit)
	FW_USHORT		date;							// 最終更新日付
													//	15～ 9bit=年+1980(0～127)
													//	 8～ 5bit=月(1～12)
													//	 4～ 0bit=日(1～31)
	FW_USHORT		time;							// 最終更新時刻
													//	15～11bit=時(0～23)
													//	10～ 5bit=分(0～59)
													//	 4～ 0bit=秒/2(0～29)
	FW_UCHAR		sec10m;							// 最終更新時刻(10ミリ秒単位, 0～199)

	FW_USHORT		m_date;							// 作成日付
													//      15～ 9bit=年+1980(0～127)
													//       8～ 5bit=月(1～12)
													//       4～ 0bit=日(1～31)
	FW_USHORT		m_time;							// 作成時刻(2秒単位)
													//      15～11bit=時(0～23)
													//      10～ 5bit=分(0～59)
													//       4～ 0bit=秒/2(0～29)
	FW_UCHAR		m_sec10m;						//  作成時刻(10ミリ秒単位, 0～199)

	FW_USHORT		a_date;							//  アクセス日付
													//      15～ 9bit=年+1980(0～127)
													//       8～ 5bit=月(1～12)
													//       4～ 0bit=日(1～31)
	FW_USHORT		a_time;							//  アクセス時刻(2秒単位)
													//     15～11bit=時(0～23)
													//      10～ 5bit=分(0～59)
													//       4～ 0bit=秒/2(0～29)
} T_BF_FS_IF_STAT;

/***    T_BF_FS_utimbuf, T_BF_FS_utimbufEx structure    ***/
typedef struct {
	FW_USHORT date;			/*  Date                                */
							/*      15-- 9bit=year+1980(0--127)     */
							/*       8-- 5bit=month(1--12)          */
							/*       4-- 0bit=day(1--31)            */
	FW_USHORT time;			/*  Time                                */
							/*      15--11bit=hour(0--23)           */
							/*      10-- 5bit=minute(0--59)         */
							/*       4-- 0bit=second/2(0--29)       */
} T_BF_FS_utimbuf;

typedef struct {
	FW_USHORT date;			/*  Date                                */
							/*      15-- 9bit=year+1980(0--127)     */
							/*       8-- 5bit=month(1--12)          */
							/*       4-- 0bit=day(1--31)            */
	FW_USHORT time;			/*  Time                                */
							/*      15--11bit=hour(0--23)           */
							/*      10-- 5bit=minute(0--59)         */
							/*       4-- 0bit=second/2(0--29)       */
	FW_UCHAR sec10m;		/*  Time(10millisec, 0--199)            */
} T_BF_FS_utimbufEx;

/**
 * @brief volume label data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_CHAR			fs_drvname[D_BF_FS_IF_DRVNAME_MAX];	// drive name
	FW_CHAR			fs_label[D_BF_FS_IF_VOLLABEL_MAX];	// volume label
} T_BF_FS_IF_VOLLABEL_DATA;

/**
 * @brief file name data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_CHAR			fs_srcname[D_BF_FS_IF_FNAME_MAX];		// source file name
	FW_CHAR			fs_dstname[D_BF_FS_IF_FNAME_MAX];		// destination file name
} T_BF_FS_IF_FNAME_DATA;

/**
 * @brief drive name data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_CHAR			fs_srcdrv[D_BF_FS_IF_DRVNAME_MAX];	// source drive name
	FW_CHAR			fs_dstdrv[D_BF_FS_IF_DRVNAME_MAX];	// destination drive name
} T_BF_FS_IF_DRVNAME_DATA;

/**
 * @brief set attribute data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_CHAR			fs_fname[D_BF_FS_IF_FNAME_MAX];		// file name
	FW_UCHAR		fs_attr;						// file attribute
} T_BF_FS_IF_SETATTR_DATA;

/**
 * @brief set protection mode data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_CHAR			fs_drvname[D_BF_FS_IF_DRVNAME_MAX];		// drive name
	FW_UCHAR		fs_wrt_prot;						// file attribute
} T_BF_FS_IF_SETPROTMODE_DATA;

/**
 * @brief file open data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_CHAR			fs_fname[D_BF_FS_IF_FNAME_MAX];		// file name
	FW_USHORT		fs_mode;						// file open mode
	FW_USHORT		fs_amode;						// file attribute
} T_BF_FS_IF_OPEN_DATA;

/**
 * @brief file create data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_CHAR			fs_fname[D_BF_FS_IF_FNAME_MAX];		// file name
	FW_USHORT		fs_amode;						// file attribute
} T_BF_FS_IF_CREATE_DATA;

/**
 * @brief read/write data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_INT32		fs_fno;							// file No
	const FW_VOID*	fs_addr;						// read/write data address
	FW_ULONG		fs_count;						// read/write size
} T_BF_FS_IF_RDWT_DATA;

/**
 * @brief lseek data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_INT32		fs_fno;							// file No
	FW_INT32		fs_offset;						// seek offset
	FW_USHORT		fs_whence;						// seek mode
} T_BF_FS_IF_LSEEK_DATA;

/**
 * @brief lseek data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_INT32		fs_fno;							// file No
	FW_INT64		fs_offset;						// seek offset
	FW_USHORT		fs_whence;						// seek mode
} T_BF_FS_IF_LSEEK64_DATA;

/**
 * @brief get current directory data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_CHAR			fs_drvname[D_BF_FS_IF_DRVNAME_MAX];	// drive name
	FW_CHAR			dummy[3];						// dummy (for padding)
	FW_ULONG		fs_size;						// size
} T_BF_FS_IF_CURDIR_DATA;

/**
 * @breif date structure for FSIF date time sync
 * @note none
 * @attention none
 */
typedef struct {
	FW_USHORT		ad_year;						// date A.D. year
	FW_UCHAR		month;							// date month
	FW_UCHAR		day;							// date day
	FW_UCHAR		hour;							// date hour
	FW_UCHAR		min;							// date minute
	FW_UCHAR		sec;							// date second
} T_BF_FS_IF_DATE;

typedef struct {
	FW_CHAR			org_file_path[64];				// original file name
	FW_CHAR			sync_file_ext[4];				// file extension
	T_BF_FS_utimbuf	fs_utim;						// date/time
} T_BF_FS_IF_DATETIME_SYNC_DATA;

/**
 * @breif structure for media detect by poll
 * @note none
 * @attention none
 */
typedef struct {
	FW_UCHAR		ch;								// interval time
	FW_UCHAR		interval_time;					// interval time
	FW_UCHAR		check_count;					// check count
} T_BF_FS_IF_MEDIA_DETECT_BY_POLL_DATA;

/**
 * @breif Structure to set timestamp
 * @note none
 * @attention none
 */
typedef struct {
	FW_USHORT		date;							// date
													// day					: 0~4bit(1~31)
													// month				: 5~8bit(1~12)
													// year(value - 1980)	: 9~15bit(0~127)
	FW_USHORT		time;							// time
													// second / 2			: 0~4bit(0~29)
													// minute				: 5~10bit(0~59)
													// hour					: 11~15bit(0~23)
	FW_UCHAR		sec10m;						// Increment from second of "time".(0~199)
} T_BF_FS_IF_TIME;

/**
 * @breif Structure to delete multi file
 * @note none
 * @attention none
 */
typedef struct {
	const FW_CHAR* 				dirname;			// directory name
	FW_CHAR*					filename_list;		// file name list
	FW_INT32 					fnum;				// number of entries in the file list
	FS_DEL_MULTI_CB				pcallback;			// callbauck function
} T_BF_FS_IF_DEL_MULTI_DATA;

/**
 * @brief set attribute data.
 * @note none
 * @attention none.
 */
typedef struct {
	const FW_CHAR* 				dirname;			// directory name
	FW_UCHAR					fs_attr;			// file attribute
	FW_UCHAR					attr_mode;			// attribute mode
	FW_CHAR*					filename_list;		// file name list
	FW_INT32					fnum;				// number of entries in the file list
	FS_SET_ATTR_MULTI_CB		pcallback;			// callbauck function
} T_BF_FS_IF_SETATTR_MULTI_DATA;

/**
 * @brief set VFS buffer data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_INT32		fs_fno;							// file No
	const FW_VOID*	buf_addr;						// VFS buffer address
} T_BF_FS_IF_VFS_BUFF_DATA;

/**
 * @brief set attribute data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_INT32					fs_fno;				// file No
	FS_SET_WRITE_ERR_CB			pcallback;			// callbauck function
} T_BF_FS_IF_SET_WRITE_ERR;

/**
 * @brief set search entry data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_UCHAR					fs_index;			// index
	FW_CHAR						fs_name[D_BF_FS_IF_FNAME_MAX];// serch name
	FW_UCHAR					fs_attr;			// attribute
} T_BF_FS_IF_ENTRY_DATA;

/**
 * @brief set get entry data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_USHORT					fs_attributes;		// attribute
	FW_CHAR						fs_fname[D_BF_FS_IF_FNAME_MAX];		// file name
	FW_ULONG					fs_filesize;		// file size
} T_BF_FS_IF_ENTRY;

/******************************************************************************
 Type Define
******************************************************************************/
typedef FW_VOID (*FS_ENTRY)(FW_INT32 ret);
typedef T_BF_FS_IF_STAT	T_BF_FS_IF_Dir_Info;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
#if 1 // active for media driver clock control
#ifndef CO_LINUX_VERSION_ON
#define BF_Fs_If_Start_File_System() do { \
		BF_Fs_User_Custom_Sta_Tsk_FS();	\
		BF_Fs_User_Custom_Sta_Tsk_Dcf(); \
		BF_Fs_User_Custom_Sta_Tsk_Ipcufs(); \
		BF_Fs_User_Custom_Sta_Tsk_Vbd(); \
} while (0)
#else
#define BF_Fs_If_Start_File_System() do { \
		BF_Fs_User_Custom_Sta_Tsk_FS();	\
		BF_Fs_User_Custom_Sta_Tsk_Dcf(); \
		BF_Fs_User_Custom_Sta_Tsk_Vfs_Send(); \
		BF_Fs_User_Custom_Sta_Tsk_Vfs_Receive(); \
} while (0)
#endif	// CO_LINUX_VERSION_ON
#else // active for media driver clock control
#define BF_Fs_If_Start_File_System() do { \
		Dd_Top_Set_CLKSTOP_NFCKST(0);	\
		MWNF_InitPartitionInfo(); \
		NF_InitDataDrive();	\
		BF_Fs_User_Custom_Sta_Tsk_FS();	\
		BF_Fs_User_Custom_Sta_Tsk_Dcf(); \
} while (0)
#endif // active for media driver clock control

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
// romarea_boot
extern FW_INT32      BF_Fs_If_End_Task_Mdf(FW_VOID);		// for DCF-task
extern FW_INT32      BF_Fs_If_Start_Used_Media_As(FS_ENTRY entry_func, FW_INT32 media_type);
extern FW_INT32      BF_Fs_If_End_Used_Media_As(FS_ENTRY entry_func, FW_INT32 media_type);
extern FW_INT32      BF_Fs_If_QuickFormat(FW_CHAR* media_type);
extern FW_INT32      BF_Fs_If_QuickFormat_As(FS_ENTRY entry_func, FW_CHAR* media_type);
extern FW_INT32      BF_Fs_If_Statvfs(const FW_CHAR* drvname, T_BF_FS_IF_STATVFS* buf);
extern FW_INT32      BF_Fs_If_Set_VolLabel(const FW_CHAR* drvname, const FW_CHAR* label);
extern FW_INT32      BF_Fs_If_Get_VolLabel(const FW_CHAR* drvname, const FW_CHAR* label);
extern FW_INT32      BF_Fs_If_Stat(const FW_CHAR* fname, T_BF_FS_IF_STAT* sbuf);
extern FW_INT32      BF_Fs_If_Get_Attribute(const FW_CHAR* fname, FW_UCHAR* attributes);
extern FW_INT32      BF_Fs_If_Open(const FW_CHAR* fname, FW_USHORT mode, FW_USHORT amode);
extern FW_INT32      BF_Fs_If_Close(FW_INT32 fNo);
extern FW_INT32      BF_Fs_If_Read(FW_INT32 fNo, FW_VOID* addr, FW_ULONG count, FW_ULONG* length);
extern FW_INT32      BF_Fs_If_Write(FW_INT32 fNo, const FW_VOID* addr, FW_ULONG count, FW_ULONG* length);
extern FW_INT32      BF_Fs_If_Write_As(FS_ENTRY entry_func, FW_INT32 fNo, const FW_VOID* addr, FW_ULONG count);
extern FW_INT32      BF_Fs_If_Get_Result_Write_As(FW_ULONG* length);
extern FW_INT32      BF_Fs_If_Lseek(FW_INT32 fNo, FW_INT32 offset, FW_USHORT whence, FW_ULONG* length);
extern FW_INT32      BF_Fs_If_Lseek64(FW_INT32 fNo, FW_INT64 offset, FW_USHORT whence, FW_INT64* length);
extern FW_INT32      BF_Fs_If_Create(const FW_CHAR* fname, FW_USHORT amode);
extern FW_INT32      BF_Fs_If_Copy(const FW_CHAR* srcname, const FW_CHAR* dstname);
extern FW_INT32      BF_Fs_If_Remove(const FW_CHAR* fname);
extern FW_INT32      BF_Fs_If_Remove_Multi(const FW_CHAR* dirname, FW_CHAR filename_list[][13], FW_INT32 fnum, FS_DEL_MULTI_CB pcallback);
extern FW_INT32      BF_Fs_If_Fstat(FW_INT32 fNo, T_BF_FS_IF_STAT* sbuf);
extern FW_INT32      BF_Fs_If_Mkdir(const FW_CHAR* dirname);
extern FW_INT32      BF_Fs_If_Rmdir(const FW_CHAR* dirname);
extern FW_INT32      BF_Fs_If_Opendir(const FW_CHAR* dirname, FW_INT32* dNo);
extern FW_INT32      BF_Fs_If_Readdir(FW_INT32 dNo, T_BF_FS_IF_Dir_Info* fsdirent);
extern FW_INT32      BF_Fs_If_Closedir(FW_INT32 dNo);
extern FW_INT32      BF_Fs_If_Chdir(const FW_CHAR* path);
extern FW_INT32      BF_Fs_If_Get_Cur_Dir(const FW_CHAR* drvname, FW_CHAR* curpath, FW_ULONG size);
extern FW_INT32      BF_Fs_If_Set_Protection_Mode(const FW_CHAR* drvname, FW_UCHAR wrt_protect);
extern E_BF_MEDIA_STATE_MEDIA_STATE BF_Fs_If_Get_Media_State_By_Poll(FW_UCHAR ch, FW_UCHAR interval_time, FW_UCHAR check_count);
extern FW_INT32      BF_Fs_If_Set_Media_State(FW_UCHAR ch, FW_UCHAR interval_time, FW_UCHAR check_count);
extern E_BF_MEDIA_STATE_MEDIA_STATE BF_Fs_If_Get_Media_State(FW_UCHAR ch);
extern E_BF_MEDIA_STATE_MEDIA_STATE BF_Fs_If_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);
extern E_BF_MEDIA_STATE_MEDIA_TYPE  BF_Fs_If_Get_Cur_Media(FW_VOID);
extern FW_INT32      BF_Fs_If_Get_Free_Size(E_BF_MEDIA_STATE_MEDIA_TYPE media_type, FW_DOUBLE* free_size);
extern FW_INT32      BF_Fs_If_Wait_Fs_Init_End(FW_VOID);
extern FW_INT32      BF_Fs_If_Check_Fs_Init_End(FW_VOID);
extern FW_INT32      BF_Fs_If_Is_Exist(const FW_CHAR* fname);
extern FW_INT32      BF_Fs_If_Check_Mbr(FW_VOID);
extern FW_INT32      BF_Fs_If_Make_Mbr(FW_VOID);
extern FW_INT32      BF_Fs_If_Set_Media_Dma(FW_UCHAR enable);
extern FW_INT32      BF_Fs_If_Set_Vfs_Write_Err_Cb(FW_INT32 fNo, FS_SET_WRITE_ERR_CB pCallback );

// romarea_comm
extern FW_INT32      BF_Fs_If_Copy_File_To_Drive(FW_CHAR* src_drv_name, FW_CHAR* dst_drv_name);

// romarea_usb
extern FW_INT32      BF_Fs_If_End_Drv(FW_VOID);
extern FW_INT32      BF_Fs_If_Init_Drv(FW_UCHAR mount);

// romarea_last
extern FW_INT32      BF_Fs_If_Get_Cur_Drv(FW_CHAR* drvname);
extern FW_INT32      BF_Fs_If_Sync(const FW_CHAR* drvname);
extern FW_INT32      BF_Fs_If_Fsync(FW_INT32 fNo);
extern FW_INT32      BF_Fs_If_Rename(const FW_CHAR* srcname, const FW_CHAR* dstname);
extern FW_INT32      BF_Fs_If_Set_Attribute(const FW_CHAR* fname, FW_UCHAR attributes);
extern FW_INT32      BF_Fs_If_Set_Attribute_Multi(const FW_CHAR* dirname, FW_UCHAR attributes, FW_UCHAR attr_mode, FW_CHAR filename_list[][13], FW_INT32 fnum, FS_SET_ATTR_MULTI_CB pcallback);
extern E_BF_MEDIA_STATE_MEDIA_WP    BF_Fs_If_Get_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);
extern FW_INT32      BF_Fs_If_Change_Media(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);
extern FW_INT32      BF_Fs_If_Init_SD0_For_Copy(FW_VOID);
extern FW_INT32      BF_Fs_If_Init_SD1_For_Copy(FW_VOID);
extern FW_INT32      BF_Fs_If_End_SD_For_Copy(FW_VOID);
extern FW_INT32      BF_Fs_If_Drive_To_Drive_Copy(FW_CHAR* szSrcDrive, FW_CHAR* szDstDrive);
extern FW_INT32      BF_Fs_If_Format_As(FS_ENTRY entry_func, FW_INT32 media_id);
extern FW_INT32      BF_Fs_If_Get_Media_Format_Type(FW_UCHAR* used_media, FW_UCHAR* format_type);
extern FW_INT32      BF_Fs_If_DateTime_Sync_With_Thm(FS_ENTRY entry_func, FW_CHAR* avi_file_path, T_BF_FS_IF_DATE* date_time);
extern FW_INT32      BF_Fs_If_Set_Time(FW_CHAR* fname, FW_UCHAR timestamp_type, T_BF_FS_IF_TIME* utime);
extern FW_INT32		 BF_Fs_If_Set_User_Setting( FW_VOID *addr, ULONG count );
extern FW_INT32      BF_Fs_If_Fsfirst(FW_UCHAR index, const FW_CHAR* name, FW_UCHAR attr, T_BF_FS_IF_ENTRY* buf);
extern FW_INT32      BF_Fs_If_Fsnext(FW_UCHAR index, T_BF_FS_IF_ENTRY* buf);

extern FW_VOID       BF_Fs_If_Entry_Dummy(FW_INT32 ret);

#endif
