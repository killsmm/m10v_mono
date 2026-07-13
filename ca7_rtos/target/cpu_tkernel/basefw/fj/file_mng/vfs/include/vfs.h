/**
 * @file		vfs.h
 * @brief		Virtual FileSystem function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _VFS_H_
#define _VFS_H_


#include "fs_user_custom.h"
#include "fs_if.h"
#include "sdram_map_define.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_BF_FS_VFS_START_MDF			(0x0001)
#define D_BF_FS_VFS_INIT_DRV			(0x0002)
#define D_BF_FS_VFS_END_DRV				(0x0003)

#define D_BF_FS_VFS_STATVFS				(0x0100)
#define D_BF_FS_VFS_GET_FREESPACE		(0x0101)
#define D_BF_FS_VFS_GET_CURDRV			(0x0102)
#define D_BF_FS_VFS_SET_VOLLABEL		(0x0103)
#define D_BF_FS_VFS_GET_VOLLABEL		(0x0104)
#define D_BF_FS_VFS_FSYNC				(0x0105)
#define D_BF_FS_VFS_SYNC				(0x0106)
#define D_BF_FS_VFS_SET_PROTECTION_MODE	(0x0107)

#define D_BF_FS_VFS_RENAME				(0x0200)
#define D_BF_FS_VFS_STAT				(0x0201)
#define D_BF_FS_VFS_SET_ATTRIBUTE		(0x0202)
#define D_BF_FS_VFS_SET_ATTRIBUTE_MULTI	(0x0203)
#define D_BF_FS_VFS_GET_ATTRIBUTE		(0x0204)
#define D_BF_FS_VFS_UTIME				(0x0205)
#define D_BF_FS_VFS_UTIME_EX			(0x0206)

#define D_BF_FS_VFS_OPEN				(0x0300)
#define D_BF_FS_VFS_CLOSE				(0x0301)
#define D_BF_FS_VFS_READ				(0x0302)
#define D_BF_FS_VFS_WRITE				(0x0303)
#define D_BF_FS_VFS_LSEEK				(0x0304)
#define D_BF_FS_VFS_LSEEK64				(0x0305)
#define D_BF_FS_VFS_CREATE				(0x0306)
#define D_BF_FS_VFS_COPY				(0x0307)
#define D_BF_FS_VFS_REMOVE				(0x0308)
#define D_BF_FS_VFS_REMOVEMULTI			(0x0309)
#define D_BF_FS_VFS_FSTAT				(0x030A)

#define D_BF_FS_VFS_MKDIR				(0x0400)
#define D_BF_FS_VFS_RMDIR				(0x0401)
#define D_BF_FS_VFS_OPENDIR				(0x0402)
#define D_BF_FS_VFS_READDIR				(0x0403)
#define D_BF_FS_VFS_CLOSEDIR			(0x0404)
#define D_BF_FS_VFS_CHDIR				(0x0405)
#define D_BF_FS_VFS_GETCURDIR			(0x0406)

#define D_BF_FS_VFS_QFORMAT				(0x0500)
#define D_BF_FS_VFS_NFORMAT				(0x0501)

#define D_BF_FS_VFS_USERSET				(0x0600)

#define D_BF_FS_VFS_INFO_DRV_NAME_MAX	(520)
#define D_BF_FS_VFS_FNAME_MAX			(520)	/* max length of file name */

// Address definition
#define D_BF_FS_VFS_BLK_SIZ_INF_TBL		(2048)	/* 1 block size  of info table */
#define D_BF_FS_VFS_BLK_NUM_INF_TBL		(72)	/* total block number of info table */
#define D_BF_FS_VFS_ADR_INFO_AREA		(SDRAM_ADR_VFS_DATA_AREA)
#define D_BF_FS_VFS_ADR_BUF_AREA		(D_BF_FS_VFS_ADR_INFO_AREA + (D_BF_FS_VFS_BLK_SIZ_INF_TBL * D_BF_FS_VFS_BLK_NUM_INF_TBL))

#define D_BF_FS_VFS_SETATTRIBUTE_MULTI_SHORTNAME_MAX	(13)	/* FS_SETATTRIBUTE_MULTI_SHORTNAME_MAX */
// function for writing error
typedef FW_INT32 (*VFS_SET_WRITE_ERR_CB)(FW_UINT32, FW_INT32);

/*---------------------------------------------------------------*/
/* Error Number													 */
/*---------------------------------------------------------------*/
#define D_BF_FS_VFS_FILE_LIST_FULL		(-1)
#define D_BF_FS_VFS_NO_MORE_OPEN		(-24)	/* The maximum, open number (number of maximum continuance processing) was exceeded */
#define D_BF_FS_VFS_NO_SPACE			(-28)	/* An empty area of the filesystem is insufficient */
#define D_BF_FS_VFS_EOF					(-501)	/* When you reach the terminal of the file by reading */

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* VFS Info table */
typedef struct {
	FW_INT32 					ope_cmd;							/* Operation command */
	FW_INT32 					info_tbl_no;						/* Info TBL No.		 */
	FW_INT32 					buff_no;							/* Buffer No.		 */
	FW_INT32 					memory_select;						/* Memory Select(0：VFS Data Buffer  1：Shared memory)	*/
	FW_INT32 					data_buffer_pointer;				/* Buffer address(0：VFS Data Buffer addr 1：Shared memory addr) */
	FW_INT32* 					pcallback;							/* Callback pointer	 */
	FW_INT32 					reserved1;							/* reserved			 */
	FW_INT32 					reserved2;							/* reserved			 */
} T_FS_VFS_INFO_TABLE_COMM;

/* VFS Open Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					mode;
	FW_INT32					access_mode;
	FW_CHAR						file_name[D_BF_FS_VFS_FNAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_OPEN;

/* VFS Close Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					file_id;
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_CLOSE;

/* VFS Read Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					file_id;
	FW_UINT32*					buffer_pointer;
	FW_INT32					count;
	FW_INT32					error_code;
	FW_INT32					length;
} T_FS_VFS_INFO_TABLE_READ;

/* VFS Write Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					file_id;
	FW_UINT32*					buffer_pointer;
	FW_INT32					count;
	FW_INT32					error_code;
	FW_INT32					length;
} T_FS_VFS_INFO_TABLE_WRITE;

/* VFS Seek Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					file_id;
	FW_INT32					offset;
	FW_INT32					whence;
	FW_INT32					error_code;
	FW_INT32					length;
} T_FS_VFS_INFO_TABLE_LSEEK;

/* VFS Seek64 Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					file_id;
	FW_INT64					offset;
	FW_INT32					whence;
	FW_INT32					error_code;
	FW_INT64					length;
} T_FS_VFS_INFO_TABLE_LSEEK64;


/* VFS Start MDF Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_START_MDF;

/* VFS Init DRV Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					media_id;
	FW_CHAR						drive_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_INIT_DRV;

/* VFS End DRV Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						drive_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_END_DRV;

/* VFS Stat Vfs Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						drive_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
	FW_INT32					block_size;
	FW_INT32					block_nums;
	FW_INT32					block_free;
	FW_INT32					fs_type;
	FW_INT32					drive_stat;
} T_FS_VFS_INFO_TABLE_STAT_VFS;

/* VFS Get Free Space Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						drive_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
	FW_INT32					block_size;
	FW_INT32					block_nums;
	FW_INT32					block_free;
} T_FS_VFS_INFO_TABLE_GET_FREE_SPACE;

/* VFS Get Cur Space Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						drive_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_GET_CUR_DRV;

/* VFS Set Vol Label Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						drive_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_CHAR						vol_label[16];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_SET_VOL_LABEL;

/* VFS Get Vol Label Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						drive_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
	FW_CHAR						vol_label[16];
} T_FS_VFS_INFO_TABLE_GET_VOL_LABEL;

/* VFS FSync Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					file_id;
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_FSYNC;

/* VFS Sync Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						drive_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_SYNC;

/* VFS Set Protection Mode Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					mode;
	FW_CHAR						drive_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_SET_PROTECTION_MODE;

/* VFS Rename Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						src_name[D_BF_FS_VFS_FNAME_MAX];
	FW_CHAR						dst_name[D_BF_FS_VFS_FNAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_RENAME;

/* VFS Stat Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						file_name[D_BF_FS_VFS_FNAME_MAX];
	FW_INT32					error_code;
	FW_INT32					create_date;
	FW_INT32					create_time;
	FW_INT32					modify_date;
	FW_INT32					modify_time;
	FW_INT64					size;
} T_FS_VFS_INFO_TABLE_STAT;

/* VFS Set Attribute table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					access_mode;
	FW_CHAR						file_name[D_BF_FS_VFS_FNAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_SET_ATTRIBUTE;

/* VFS Set Attribute Multi table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					access_mode;
	FW_INT32					mode;
	FW_INT32					file_num;
	FW_CHAR						file_name_list[100][13];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_SET_ATTRIBUTE_MULTI;

/* VFS Get Attribute table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						file_name[D_BF_FS_VFS_FNAME_MAX];
	FW_INT32					error_code;
	FW_INT32					access_mode;
} T_FS_VFS_INFO_TABLE_GET_ATTRIBUTE;

/* VFS Utime table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					date;
	FW_INT32					time;
	FW_CHAR						file_name[D_BF_FS_VFS_FNAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_UTIME;

/* VFS Utime Ex table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					date;
	FW_INT32					time;
	FW_INT32					sec_10m;
	FW_INT32					mode;
	FW_CHAR						file_name[D_BF_FS_VFS_FNAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_UTIME_EX;

/* VFS Create Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					amode;
	FW_CHAR						file_name[D_BF_FS_VFS_FNAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_CREATE;

/* VFS Copy Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						src_name[D_BF_FS_VFS_FNAME_MAX];
	FW_CHAR						dst_name[D_BF_FS_VFS_FNAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_COPY;

/* VFS Remove Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						file_name[D_BF_FS_VFS_FNAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_REMOVE;

/* VFS Removemulti Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					fnum;
	FW_CHAR						dir_name[D_BF_FS_VFS_FNAME_MAX];
	FW_CHAR						filename_list[100][13];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_REMOVEMULTI;

/* VFS Fstat Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					file_id;
	FW_INT32					error_code;
	FW_INT32					create_date;
	FW_INT32					create_time;
	FW_INT32					modify_date;
	FW_INT32					modify_time;
	FW_INT32					size;
} T_FS_VFS_INFO_TABLE_FSTAT;

/* VFS Mkdir Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						dir_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_MKDIR;

/* VFS Rmdir Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						dir_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_RMDIR;

/* VFS Opendir Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						dir_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_INT32					dir_id;
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_OPENDIR;

/* VFS Readdir Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					dir_id;
	FW_CHAR						file_name[16];
	FW_INT32					amode;
	FW_INT32					file_size;
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_READDIR;

/* VFS Closedir Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					dir_id;
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_CLOSEDIR;

/* VFS Chdir Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_CHAR						dir_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_CHDIR;

/* VFS GetCurdir Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					size;
	FW_CHAR						drive_name[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_CHAR						curpath[D_BF_FS_VFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_GETCURDIR;

/* VFS QFormat Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					media_id;
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_QFORMAT;

/* VFS NFormat Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_INT32					media_id;
	FW_INT32					error_code;
	FW_INT32					progress_value;
} T_FS_VFS_INFO_TABLE_NFORMAT;

/* VFS User Setting Info table  */
typedef struct {
	T_FS_VFS_INFO_TABLE_COMM	common_data;
	FW_UINT32*					buffer_pointer;
	FW_INT32					count;
	FW_INT32					error_code;
} T_FS_VFS_INFO_TABLE_USERSET;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Macro					            						 */
/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/
/* Grobal Function											     */
/*---------------------------------------------------------------*/
extern FW_INT32 BF_Vfs_Open(const FW_CHAR* fname, FW_USHORT mode, FW_USHORT amode);
extern FW_INT32 BF_Vfs_Close(FW_INT32 fileNo);
extern FW_INT32 BF_Vfs_Read(FW_INT32 fileNo, FW_VOID* addr, FW_ULONG count, FW_ULONG* length);
extern FW_INT32 BF_Vfs_Write(FW_INT32 fileNo, const  FW_VOID* addr, FW_ULONG count, FW_ULONG* length);
extern FW_INT32 BF_Vfs_Lseek(FW_INT32 fileNo, FW_INT32 offset, FW_USHORT whence, FW_ULONG* length);
extern FW_INT32 BF_Vfs_Lseek64(FW_INT32 fileNo, FW_INT64 offset, FW_USHORT whence, FW_INT64* length);
extern FW_INT32 BF_Vfs_Start_MDF( FW_VOID );
extern FW_INT32 BF_Vfs_Init_Drv( FW_CHAR* drvname, FW_INT32 mediaid );
extern FW_INT32 BF_Vfs_End_Drv( FW_CHAR* drvname );
extern FW_INT32 BF_Vfs_Stat_VFS( FW_CHAR* drvname, T_BF_FS_IF_STATVFS* buf );
extern FW_INT32 BF_Vfs_Get_Free_Space( FW_CHAR* drvname, FW_UINT32* blksize, FW_UINT32* blocks, FW_UINT32* bfree );
extern FW_INT32 BF_Vfs_Get_Cur_Drv( FW_CHAR* drvname );
extern FW_INT32 BF_Vfs_Set_Vol_Label( FW_CHAR* drvname, FW_CHAR* label );
extern FW_INT32 BF_Vfs_Get_Vol_Label( FW_CHAR* drvname, FW_CHAR* label );
extern FW_INT32 BF_Vfs_Fsync( FW_INT32 fileNo );
extern FW_INT32 BF_Vfs_Sync( FW_CHAR* drvname );
extern FW_INT32 BF_Vfs_Set_Protection_Mode( FW_CHAR* drvname, FW_UINT32 mode );
extern FW_INT32 BF_Vfs_Rename( FW_CHAR* srcname, FW_CHAR* dstname );
extern FW_INT32 BF_Vfs_Stat( FW_CHAR* fname, T_BF_FS_IF_STAT* sbuf );
extern FW_INT32 BF_Vfs_Set_Attribute( FW_CHAR* fname, FW_INT32 attributes );
extern FW_INT32 BF_Vfs_Set_Attribute_Multi( FW_CHAR* filename_list[][D_BF_FS_VFS_SETATTRIBUTE_MULTI_SHORTNAME_MAX], FW_UCHAR* attributes, FW_UCHAR* attr_mode, FW_INT32* fnum );
extern FW_INT32 BF_Vfs_Get_Attribute( FW_CHAR* fname, FW_UCHAR* attributes );
extern FW_INT32 BF_Vfs_Utime( FW_CHAR* fname, T_BF_FS_utimbuf* times );
extern FW_INT32 BF_Vfs_Utime_Ex( FW_CHAR* fname, FW_UINT32 utime_mode, T_BF_FS_utimbufEx* times );
extern FW_INT32 BF_Vfs_Create(FW_CHAR* dirname, FW_INT32 amode);
extern FW_INT32 BF_Vfs_Copy(FW_CHAR* srcname, FW_CHAR* dstname);
extern FW_INT32 BF_Vfs_Remove(FW_CHAR* fname);
extern FW_INT32 BF_Vfs_RemoveMulti(FW_CHAR* dirname, FW_CHAR filename_list[][13], FW_INT32 fnum);
extern FW_INT32 BF_Vfs_Fstat(FW_INT32 fileNo, T_BF_FS_IF_STAT* sbuf);
extern FW_INT32 BF_Vfs_Mkdir(FW_CHAR* dirname);
extern FW_INT32 BF_Vfs_Rmdir(FW_CHAR* dirname);
extern FW_INT32 BF_Vfs_Opendir(FW_CHAR* dirname, FW_INT32* dNo);
extern FW_INT32 BF_Vfs_Readdir(FW_INT32 dNo, T_BF_FS_IF_Dir_Info* fsdirent);
extern FW_INT32 BF_Vfs_Closedir(FW_INT32 dNo);
extern FW_INT32 BF_Vfs_Chdir(FW_CHAR* dirname);
extern FW_INT32 BF_Vfs_Getcurdir(FW_CHAR* dname, FW_CHAR* curpath, FW_INT32 size);
extern FW_INT32 BF_Vfs_Qformat(FW_INT32 media_id);
extern FW_INT32 BF_Vfs_Nformat(FW_INT32 media_id);
extern FW_INT32 BF_Vfs_Set_Write_Err_Cb( FW_UINT32 fNo, VFS_SET_WRITE_ERR_CB pCallback );
extern FW_INT32 BF_Vfs_Set_User_Setting( const FW_VOID* addr, FW_ULONG count );

#endif
