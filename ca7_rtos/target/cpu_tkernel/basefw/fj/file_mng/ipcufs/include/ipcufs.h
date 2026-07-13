/**
 * @file		ipcufs.h
 * @brief		Fake FileSystem function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _IPCUFS_H_
#define _IPCUFS_H_

#include "fs_user_custom.h"
#include "sdram_map_define.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* For Fake FileSystem */
#define D_BF_IPCUFS_IFSA_START		(0x2000)
#define D_BF_IPCUFS_IFSA_END		(0x2001)
#define D_BF_IPCUFS_IFSA_LOOKUP		(0x2002)
#define D_BF_IPCUFS_IFSA_READDIR	(0x2003)
#define D_BF_IPCUFS_IFSA_READPAGE	(0x2004)
#define D_BF_IPCUFS_IFSA_WRITEPAGE	(0x2005)
#define D_BF_IPCUFS_IFSA_IGET		(0x2006)
#define D_BF_IPCUFS_IFSA_ICREATE	(0x2007)
#define D_BF_IPCUFS_IFSA_IDELETE	(0x2008)
#define D_BF_IPCUFS_IFSA_IRENAME	(0x2009)
#define D_BF_IPCUFS_IFSA_STATFS		(0x200A)
#define D_BF_IPCUFS_IFSA_OPEN		(0x200B)
#define D_BF_IPCUFS_IFSA_CLOSE		(0x200C)
/* For Hibernation */
#define D_BF_IPCUFS_UNMOUNT			(0x3000)
#define D_BF_IPCUFS_MOUNT			(0x3001)
#define D_BF_IPCUFS_HIB_NOTIFY		(0x3002)
/* sector size */
#define D_BF_IPCUFS_SECTOR_SIZE			(512)
/* page size */
#define D_BF_IPCUFS_PAGE_SIZE			(2048)
/* block size */
#define D_BF_IPCUFS_BLOCK_SIZE			(131072)
/* parameter */
#define D_BF_IPCUFS_PAGE_PAR_BLOCK		(D_BF_IPCUFS_BLOCK_SIZE / D_BF_IPCUFS_PAGE_SIZE)
#define D_BF_IPCUFS_SEC_PAR_PAGE		(D_BF_IPCUFS_PAGE_SIZE / D_BF_IPCUFS_SECTOR_SIZE)
#define D_BF_IPCUFS_SEC_PAR_BLOCK		(D_BF_IPCUFS_PAGE_PAR_BLOCK * D_BF_IPCUFS_SEC_PAR_PAGE)
/* Common */
#define D_BF_IPCUFS_ERR_OK			(0x0000)
#define D_BF_IPCUFS_ERR_NG			(0x0001)
#define D_BF_IPCUFS_ERR_LAST_ENTRY	(0x0002)
#define D_BF_IPCUFS_ERR_EOF			(0x0003)

#define D_BF_IPCUFS_INFO_DRV_NAME_MAX	(520)
#define D_BF_IPCUFS_FNAME_MAX			(520)	/* max length of file name */

#define D_BF_IPCUFS_RW_DATA_MAX			(64)

/*---------------------------------------------------------------*/
/* Error Number													 */
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* IPCUFS Info table */
typedef struct {
	FW_INT32 ope_cmd;
	FW_INT32 buffer_num;
	FW_INT32 plane_num;
	FW_INT32 memory_select;
	FW_INT32 data_buffer_pointer;
	FW_INT32 reserved1;
	FW_INT32 reserved2;
	FW_INT32 reserved3;
} T_IPCUFS_INFO_TABLE_COMM;

/* IPCUFS Start Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					drive;
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_START;

/* IPCUFS End Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					drive;
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_END;

/* IPCUFS Lookup Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					drive;
	FW_CHAR						path[D_BF_IPCUFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_LOOKUP;

/* IPCUFS Read Dir Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					drive;
	FW_CHAR						path[D_BF_IPCUFS_INFO_DRV_NAME_MAX];
	FW_INT32					open_flag;
	FW_CHAR						file_name[516];
	FW_INT32					type;
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_READDIR;

/* IPCUFS Read Page Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					fno;
	FW_CHAR						reserved[D_BF_IPCUFS_INFO_DRV_NAME_MAX-4];
	FW_INT64					read_offset;
	FW_INT32					physical_addr[D_BF_IPCUFS_RW_DATA_MAX];
	FW_INT32					read_length[D_BF_IPCUFS_RW_DATA_MAX];
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_READPAGE;

/* IPCUFS Write Page Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					fno;
	FW_CHAR						reserved[D_BF_IPCUFS_INFO_DRV_NAME_MAX-4];
	FW_INT64					write_offset;
	FW_INT32					physical_addr[D_BF_IPCUFS_RW_DATA_MAX];
	FW_INT32					write_length[D_BF_IPCUFS_RW_DATA_MAX];
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_WRITEPAGE;

/* IPCUFS IGet Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					drive;
	FW_CHAR						path[D_BF_IPCUFS_INFO_DRV_NAME_MAX];
	FW_INT64					file_length;
	FW_INT32					date;
	FW_INT32					time;
	FW_INT32					m_date;
	FW_INT32					m_time;
	FW_INT32					a_date;
	FW_INT32					a_time;
	FW_INT32					type;
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_IGET;

/* IPCUFS ICreate Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					drive;
	FW_CHAR						path[D_BF_IPCUFS_INFO_DRV_NAME_MAX];
	FW_INT32					type;
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_ICREATE;

/* IPCUFS IDelete Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					drive;
	FW_CHAR						path[D_BF_IPCUFS_INFO_DRV_NAME_MAX];
	FW_INT32					type;
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_IDELETE;

/* IPCUFS IRename Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					drive;
	FW_CHAR						path[D_BF_IPCUFS_INFO_DRV_NAME_MAX];
	FW_INT32					rename_drive;
	FW_CHAR						rename_path[D_BF_IPCUFS_INFO_DRV_NAME_MAX];
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_IRENAME;

/* IPCUFS IStatfs Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					drive;
	FW_INT32					blksize;
	FW_INT32					blocks;
	FW_INT32					bfree;
	FW_INT32					bbad;
	FW_INT32					dstat;
	FW_INT32					fstype;
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_STATFS;

/* IPCUFS Open Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					drive;
	FW_CHAR						path[D_BF_IPCUFS_INFO_DRV_NAME_MAX];
	FW_INT32					amode;
	FW_INT32					fno;
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_OPEN;

/* IPCUFS Close Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					fno;
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_CLOSE;

/* IPCUFS Unmount Info table  */
typedef struct {
	FW_INT32 					ope_cmd;
	FW_INT32					target;
	FW_INT32					reserved1;
	FW_INT32					reserved2;
	FW_INT32					reserved3;
	FW_INT32					reserved4;
	FW_INT32					reserved5;
	FW_INT32					reserved6;
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_UNMOUNT;

/* IPCUFS Mount Info table  */
typedef struct {
	T_IPCUFS_INFO_TABLE_COMM	common_data;
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_MOUNT;

/* IPCUFS Hibernation result Info table  */
typedef struct {
	FW_INT32 					ope_cmd;
	FW_INT32					result;
	FW_INT32					error_no;
	FW_INT32					bf_address;
	FW_INT32					reserved3;
	FW_INT32					reserved4;
	FW_INT32					reserved5;
	FW_INT32					reserved6;
	FW_INT32					error_code;
} T_FS_IPCUFS_INFO_TABLE_HIB_NOTIFY;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Macro					     　　　　						 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Grobal Function											     */
/*---------------------------------------------------------------*/
extern FW_INT32 BF_Ipcufs_Start(T_FS_IPCUFS_INFO_TABLE_START* info_table);
extern FW_INT32 BF_Ipcufs_End(T_FS_IPCUFS_INFO_TABLE_END* info_table);
extern FW_INT32 BF_Ipcufs_Lookup(T_FS_IPCUFS_INFO_TABLE_LOOKUP* info_table);
extern FW_INT32 BF_Ipcufs_ReadDir(T_FS_IPCUFS_INFO_TABLE_READDIR* info_table);
extern FW_INT32 BF_Ipcufs_Readpage(T_FS_IPCUFS_INFO_TABLE_READPAGE* info_table);
extern FW_INT32 BF_Ipcufs_Writepage(T_FS_IPCUFS_INFO_TABLE_WRITEPAGE* info_table);
extern FW_INT32 BF_Ipcufs_Iget(T_FS_IPCUFS_INFO_TABLE_IGET* info_table);
extern FW_INT32 BF_Ipcufs_Icreate(T_FS_IPCUFS_INFO_TABLE_ICREATE* info_table);
extern FW_INT32 BF_Ipcufs_Idelete(T_FS_IPCUFS_INFO_TABLE_IDELETE* info_table);
extern FW_INT32 BF_Ipcufs_Irename(T_FS_IPCUFS_INFO_TABLE_IRENAME* info_table);
extern FW_INT32 BF_Ipcufs_Statfs(T_FS_IPCUFS_INFO_TABLE_STATFS* info_table);
extern FW_INT32 BF_Ipcufs_Open(T_FS_IPCUFS_INFO_TABLE_OPEN* info_table);
extern FW_INT32 BF_Ipcufs_Close(T_FS_IPCUFS_INFO_TABLE_CLOSE* info_table);
extern FW_INT32 BF_Ipcufs_Unmount(T_FS_IPCUFS_INFO_TABLE_UNMOUNT* info_table);
extern FW_INT32 BF_Ipcufs_Mount(T_FS_IPCUFS_INFO_TABLE_MOUNT* info_table);

extern UCHAR BF_Ipcufs_Get_Unmount_Request_Flg(VOID);
extern FW_INT32 BF_Ipcufs_Hib_Notify(T_FS_IPCUFS_INFO_TABLE_HIB_NOTIFY* info_table);

#endif	// _IPCUFS_H_
