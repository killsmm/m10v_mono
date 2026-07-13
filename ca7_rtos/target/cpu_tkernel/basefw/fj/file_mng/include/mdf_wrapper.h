/**
 * @file		mdf_wrapper.h
 * @brief		Wrapper of MDF
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _MDF_W_LIB_H_
#define _MDF_W_LIB_H_

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
/******************************************************************************
 for MDF
******************************************************************************/
#include "FS_lib.h"
#include "FS_extra.h"
#include "FS_errno.h"
#include "fs_user_custom.h"
#include "fs_if.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* open mode */
#define D_BF_MDF_WRAPPER_O_RDONLY			O_RDONLY		/* read only */
#define D_BF_MDF_WRAPPER_O_WRONLY			O_WRONLY		/* write only */
#define D_BF_MDF_WRAPPER_O_RDWR				O_RDWR			/* read/write */
#define D_BF_MDF_WRAPPER_O_APPEND			O_APPEND		/* EOF write */
#define D_BF_MDF_WRAPPER_O_CREAT			O_CREAT			/* create and open */
#define D_BF_MDF_WRAPPER_O_TRUNC			O_TRUNC			/* open and trunc */

/* seek mode */
#define D_BF_MDF_WRAPPER_SEEK_CUR			SEEK_CUR
#define D_BF_MDF_WRAPPER_SEEK_END			SEEK_END
#define D_BF_MDF_WRAPPER_SEEK_SET			SEEK_SET

/* attribute */
#define D_BF_MDF_WRAPPER_ATTR_NORMAL		ATTR_NORMAL		/* normal */
#define D_BF_MDF_WRAPPER_ATTR_READONLY		ATTR_READONLY	/* real-only */
#define D_BF_MDF_WRAPPER_ATTR_HIDDEN		ATTR_HIDDEN		/* hidden */
#define D_BF_MDF_WRAPPER_ATTR_SYSTEM		ATTR_SYSTEM		/* system */
#define D_BF_MDF_WRAPPER_ATTR_ARCHIVE		ATTR_ARCHIVE	/* archive */

#define D_BF_MDF_WRAPPER_SEM_W_TIME			(5000)			// MDF Wrapper semaphre timeout time (ms)

/* callback function */
typedef FS_INT (*MULTI_DEL_CB)(FS_INT, FS_INT);
typedef FS_INT (*MULTI_SET_ATTR_CB)(FS_INT, FS_INT);

/******************************************************************************
 Error Number
******************************************************************************/
#define D_BF_MDF_WRAPPER_ENT_LAST				(FS_ENT_LAST)				/* (1)		最後のファイルまたはディレクトリまで読み出しました。(FS_readdirのみ)					*/
#define D_BF_MDF_WRAPPER_ERR_NOENT				(FS_ERR_NOENT)				/* (-2)		ファイルまたはディレクトリが存在しません。												*/
#define D_BF_MDF_WRAPPER_ERR_SRCH				(FS_ERR_SRCH)				/* (-3)		対象タスクのデータが存在しません。														*/
#define D_BF_MDF_WRAPPER_ERR_IO					(FS_ERR_IO)					/* (-5)		IOエラーが発生しました																	*/
#define D_BF_MDF_WRAPPER_ERR_NXIO				(FS_ERR_NXIO)				/* (-6)		ドライブがありません（切断状態）														*/
#define D_BF_MDF_WRAPPER_ERR_BADF				(FS_ERR_BADF)				/* (-9)		ファイル番号が不正です。																*/
#define D_BF_MDF_WRAPPER_ERR_NOMEM				(FS_ERR_NOMEM)				/* (-12)	メモリの獲得に失敗しました。															*/
#define D_BF_MDF_WRAPPER_ERR_BUSY				(FS_ERR_BUSY)				/* (-16)	ドライブがビジー状態です。																*/
#define D_BF_MDF_WRAPPER_ERR_EXIST				(FS_ERR_EXIST)				/* (-17)	既にファイルまたはディレクトリが存在します。											*/
#define D_BF_MDF_WRAPPER_ERR_NODEV				(FS_ERR_NODEV)				/* (-19)	指定のドライブがありません。															*/
#define D_BF_MDF_WRAPPER_ERR_NOTDIR				(FS_ERR_NOTDIR)				/* (-20)	ディレクトリが存在しません。ディレクトリではありません。								*/
#define D_BF_MDF_WRAPPER_ERR_ISDIR				(FS_ERR_ISDIR)				/* (-21)	ディレクトリに対して処理は行えません。													*/
#define	D_BF_MDF_WRAPPER_ERR_INVAL				(FS_ERR_INVAL)				/* (-22)	引数の値が不正です。																	*/
#define D_BF_MDF_WRAPPER_ERR_MOPEN				(FS_ERR_MOPEN)				/* (-24)	最大オープン数(最大継続処理数)を超えました。											*/
#define D_BF_MDF_WRAPPER_ERR_FBIG				(FS_ERR_FBIG)				/* (-27)	扱えるファイルサイズを超えています。													*/
#define D_BF_MDF_WRAPPER_ERR_NOSPC				(FS_ERR_NOSPC)				/* (-28)	ファイルシステムの空き領域が不足しています。											*/
#define D_BF_MDF_WRAPPER_ERR_SPIPE				(FS_ERR_SPIPE)				/* (-29)	不正なseek です。																		*/
#define D_BF_MDF_WRAPPER_ERR_ROFS				(FS_ERR_ROFS)				/* (-30)	読み込み専用のドライブあるいはファイルに書き込みは行えません。							*/
#define D_BF_MDF_WRAPPER_ERR_NOSYS				(FS_ERR_NOSYS)				/* (-38)	実装されていない機能です。																*/
#define D_BF_MDF_WRAPPER_ERR_NOTEMPTY			(FS_ERR_NOTEMPTY)			/* (-39)	ディレクトリが空になっていません														*/
#define D_BF_MDF_WRAPPER_ERR_NAMETOOLONG		(FS_ERR_NAMETOOLONG)		/* (-78)	ファイル名またはディレクトリ名が所定の長さを超えています。								*/
#define D_BF_MDF_WRAPPER_ERR_DELENT				(FS_ERR_NOENT)				/* (-2)		ファイルまたはディレクトリが消去されています											*/
#define D_BF_MDF_WRAPPER_ERR_EOF				(FS_ERR_EOF)				/* (-501)	読み込みでファイルの終端に達した場合													*/
#define D_BF_MDF_WRAPPER_ERR_EXCLUSIVE			(FS_ERR_EXCLUSIVE)			/* (-502)	ファイルまたはディレクトリは既に使用されています。										*/
#define D_BF_MDF_WRAPPER_ERR_RDONLY				(FS_ERR_ROFS)				/* (-30)	読み込み専用のオープンモード															*/
#define D_BF_MDF_WRAPPER_ERR_WRONLY				(FS_ERR_WRONLY)				/* (-511)	書き込み専用のオープンモード															*/
#define D_BF_MDF_WRAPPER_ERR_NAMEERR			(FS_ERR_NAMEERR)			/* (-520)	ファイルまたはディレクトリ名に使用できない文字が含まれています。						*/
#define D_BF_MDF_WRAPPER_ERR_MDF				(FS_ERR_MDF)				/* (-600)	既にMDFは開始されています。																*/
#define D_BF_MDF_WRAPPER_ERR_NOMDF				(FS_ERR_NOMDF)				/* (-601)	MDFが開始されていません。																*/
#define D_BF_MDF_WRAPPER_ERR_MDRV				(FS_ERR_MDRV)				/* (-610)	使用可能なドライブ登録数を超えました。													*/
#define D_BF_MDF_WRAPPER_ERR_DEVENT				(FS_ERR_DEVENT)				/* (-611)	同じドライブ名が登録されています。														*/
#define D_BF_MDF_WRAPPER_ERR_NODRV				(FS_ERR_NODEV)				/* (-19)	ドライブが初期化されていません。														*/
#define D_BF_MDF_WRAPPER_ERR_DRVUSE				(FS_ERR_BUSY)				/* (-16)	ドライブは使用中です。																	*/
#define D_BF_MDF_WRAPPER_ERR_SYNC				(FS_ERR_BUSY)				/* (-16)	ドライブは同調中です。																	*/
#define D_BF_MDF_WRAPPER_ERR_FORMAT				(FS_ERR_BUSY)				/* (-16)	フォーマット中です。																	*/
#define D_BF_MDF_WRAPPER_ERR_NOCONDRV			(FS_ERR_NXIO)				/* (-6)		ドライブが切断状態されています。														*/
#define D_BF_MDF_WRAPPER_ERR_MTASK				(FS_ERR_MTASK)				/* (-620)	MDFにアクセス可能なタスク数を越えました。												*/
#define D_BF_MDF_WRAPPER_ERR_NOTASK				(FS_ERR_SRCH)				/* (-3)		対象タスクのデータが存在しません。														*/
#define D_BF_MDF_WRAPPER_ERR_NOSEM				(FS_ERR_NOSEM)				/* (-630)	空きセマフォがありません。																*/
#define D_BF_MDF_WRAPPER_ERR_CRESEM				(FS_ERR_CRESEM)				/* (-631)	セマフォが生成できませんでした。														*/
#define D_BF_MDF_WRAPPER_ERR_DELSEM				(FS_ERR_DELSEM)				/* (-632)	セマフォが削除できませんでした。														*/
#define D_BF_MDF_WRAPPER_ERR_ENQSEM				(FS_ERR_ENQSEM)				/* (-633)	セマフォの排他ができませんでした。														*/
#define D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT		(FS_ERR_ENQSEMTMOUT)		/* (-634)	セマフォの排他でタイムアウトしました。													*/
#define D_BF_MDF_WRAPPER_ERR_DEQSEM				(FS_ERR_DEQSEM)				/* (-635)	セマフォの排他解除ができませんでした。													*/
#define D_BF_MDF_WRAPPER_ERR_UNCORRESPONDING	(FS_ERR_UNCORRESPONDING)	/* (-801)	対応していないフォーマットです。														*/
#define D_BF_MDF_WRAPPER_ERR_NOROOT				(FS_ERR_NOROOT)				/* (-810)	ルートディレクトリに空きディレクトリエントリがありません。								*/
#define D_BF_MDF_WRAPPER_ERR_INVOKE				(FS_ERR_INVOKE)				/* (-820)	実行することができない処理・関数です。													*/
/* '-900'より小さいエラーコードは、
 * ファイルシステムを継続して処理することができないエラーです。
 * 媒体の使用を中止し、媒体チェックあるいはフォーマットを行ってください。
 * 継続して処理を行った場合は、データが壊れる可能性があります。
 */
#define D_BF_MDF_WRAPPER_ERR_FAT_READ			(FS_ERR_FAT_READ)			/* (-901)	FAT領域の読み込みに失敗しました。														*/
#define D_BF_MDF_WRAPPER_ERR_FAT_WRITE			(FS_ERR_FAT_WRITE)			/* (-902)	FAT領域に書き込みに失敗しました。														*/
#define D_BF_MDF_WRAPPER_ERR_ROOT_READ			(FS_ERR_ROOT_READ)			/* (-903)	FAT12,16のルート領域の読み込みに失敗しました。											*/
#define D_BF_MDF_WRAPPER_ERR_ROOT_WRITE			(FS_ERR_ROOT_WRITE)			/* (-904)	FAT12,16のルート領域の書き込み、初期化に失敗しました。									*/
#define D_BF_MDF_WRAPPER_ERR_BADREAD			(FS_ERR_BADREAD)			/* (-905)	不良領域(クラスタ)のリードが失敗したため、代替処理がおこなえませんでした				*/
#define D_BF_MDF_WRAPPER_ERR_BADWRITE			(FS_ERR_BADWRITE)			/* (-906)	不良領域(クラスタ)のライトがリトライ回数失敗したため、代替処理がおこなえませんでした	*/
#define D_BF_MDF_WRAPPER_ERR_FSNOADJ			(FS_ERR_FSNOADJ)			/* (-910)	使用ファイルシステムと媒体に不整合があります											*/
#define D_BF_MDF_WRAPPER_ERR_DIRERR				(FS_ERR_DIRERR)				/* (-911)	ディレクトリエントリの構成に、異常があります。											*/
#define D_BF_MDF_WRAPPER_ERR_CHAIN				(FS_ERR_CHAIN)				/* (-912)	クラスタチェーンの構成に、異常があります。												*/
#define D_BF_MDF_WRAPPER_ERR_FSID				(FS_ERR_FSID)				/* (-999)	ファイルシステムでエラーが発生しました。												*/
/* '-1000'より小さいエラーコードはドライバ関数のエラーコードを参照して下さい。*/
#define D_BF_MDF_WRAPPER_DRV_ERR_INVALID_DATA	(FS_DRV_ERR_INVALID_DATA)	/* (-1000)	カード抜等によりデータ無効になりました													*/

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Driver function information */
typedef struct {
    FS_INT (*Bread)(FS_VOID *, FS_DWORD, FS_DWORD, char *);  /* Sector reading processing */
    FS_INT (*Bwrite)(FS_VOID *, FS_DWORD, FS_DWORD, char *); /* Sector writing processing */
    FS_INT (*Bclear)(FS_VOID *, FS_DWORD, FS_DWORD);         /* Sector clear              */
    FS_INT (*GetSectorSize)(FS_VOID *);                      /* Sector size obtained      */
    FS_INT (*SetWriteMode)(FS_VOID *, FS_DWORD);             /* Configures reading and writing mode */
} T_FS_FUNCNAME;

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

/*******************************************************************************
 Define MDF Wrapper Function
*******************************************************************************/
/* MDF start&end API */
extern FS_INT BF_Mdf_Wrapper_Start_Mdf(FS_VOID);
extern FS_INT BF_Mdf_Wrapper_End_Mdf(FS_VOID);
extern FS_INT BF_Mdf_Wrapper_End_Task(FS_VOID);

/* Init&End API */
extern FS_INT BF_Mdf_Wrapper_Init_Drv(char* drvname, T_FS_FUNCNAME* pDrvinfo, FS_VOID* pDevinfo);
extern FS_INT BF_Mdf_Wrapper_End_Drv(char* drvname);

/* Drive API */
extern FS_INT BF_Mdf_Wrapper_Qformat(char* drvname);
extern FS_INT BF_Mdf_Wrapper_Nformat(T_FS_FUNCNAME *pDrvinfo, FS_VOID *pDevinfo);
extern FS_INT BF_Mdf_Wrapper_Statvfs(const char* drvname, T_BF_FS_IF_STATVFS* buf);
extern FS_INT BF_Mdf_Wrapper_Get_Free_Space(const char* drvname, FS_DWORD* blksize, FS_DWORD* blocks, FS_DWORD* bfree);
extern DOUBLE BF_Mdf_Wrapper_Get_Free_Space_By_Cache(const char* drvname);
extern FS_INT BF_Mdf_Wrapper_Get_Cur_Drv(char* drvname);
extern FS_INT BF_Mdf_Wrapper_Set_Vol_Label(const char* drvname, const char* label);
extern FS_INT BF_Mdf_Wrapper_Get_Vol_Label(const char* drvname, char* label);
extern FS_INT BF_Mdf_Wrapper_Fsync(FS_INT fNo);
extern FS_INT BF_Mdf_Wrapper_Sync(const char* drvname);
extern FS_INT BF_Mdf_Wrapper_Set_Con_Sec_Max(const char* drvname, FS_DWORD secmax);
extern FS_INT BF_Mdf_Wrapper_Set_Protection_Mode(const char* drvname, FS_DWORD mode);

/* Info Control API */
extern FS_INT BF_Mdf_Wrapper_Rename(const char* srcname, const char* dstname);
extern FS_INT BF_Mdf_Wrapper_Stat(const char* fname, T_BF_FS_IF_STAT* sbuf);
extern FS_INT BF_Mdf_Wrapper_Set_Attribute(const char* fname, FS_BYTE attributes);
extern FS_INT BF_Mdf_Wrapper_Set_Attribute_For_Multi_Set(const char* fname, FS_BYTE attributes);
extern FS_INT BF_Mdf_Wrapper_Set_Attribute_Current_Entry(FS_INT dNo, FS_dirent* fsentry, FS_BYTE attributes);
extern FS_INT BF_Mdf_Wrapper_Set_Attribute_Multi(const char* dirname, FS_BYTE attributes, FS_BYTE attr_mode, char filename_list[][13], FS_INT fnum, MULTI_SET_ATTR_CB pcallback);
extern FS_INT BF_Mdf_Wrapper_Get_Attribute(const char* fname, FS_BYTE* attributes);
extern FS_INT BF_Mdf_Wrapper_Utime(const char* fname, const T_BF_FS_utimbuf* times);
extern FS_INT BF_Mdf_Wrapper_UtimeEx(const char* fname, FS_DWORD utime_mode, const T_BF_FS_utimbufEx *times);

/* File Control API */
extern FS_INT BF_Mdf_Wrapper_Open(const char* fname, FS_WORD mode, FS_WORD amode);
extern FS_INT BF_Mdf_Wrapper_Close(FS_INT fNo);
extern FS_INT BF_Mdf_Wrapper_Read(FS_INT fNo, FS_VOID* cbuf, FS_DWORD count, FS_DWORD* length);
extern FS_INT BF_Mdf_Wrapper_Write(FS_INT fNo, const FS_VOID* cbuf, FS_DWORD count, FS_DWORD* length);
extern FS_INT BF_Mdf_Wrapper_Lseek(FS_INT fNo, FS_INT32 offset, FS_WORD whence, FS_DWORD* length);
extern FS_INT BF_Mdf_Wrapper_Lseek64(FS_INT fNo, FS_INT64 offset, FS_WORD whence, FS_INT64* length);
extern FS_INT BF_Mdf_Wrapper_Create(const char* fname, FS_WORD amode);
extern FS_INT BF_Mdf_Wrapper_Copy(const char* srcname, const char* dstname);
extern FS_INT BF_Mdf_Wrapper_Truncate(const char* fname, FS_DWORD size, FS_DWORD* length);
extern FS_INT BF_Mdf_Wrapper_Truncate64(const char* fname, FS_QWORD size, FS_QWORD* length);
extern FS_INT BF_Mdf_Wrapper_Ftruncate(FS_INT fNo, FS_DWORD size, FS_DWORD* length);
extern FS_INT BF_Mdf_Wrapper_Ftruncate64(FS_INT fNo, FS_QWORD size, FS_QWORD* length);
extern FS_INT BF_Mdf_Wrapper_Remove(const char* fname);
extern FS_INT BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(const char* fname);
extern FS_INT BF_Mdf_Wrapper_Remove_For_Multi_Delete(const char* fname);
extern FS_INT BF_Mdf_Wrapper_Remove_Current_Entry(FS_INT dNo, FS_dirent* fsdirent);
extern FS_INT BF_Mdf_Wrapper_Remove_Multi(const char *dirname, char filename_list[][13], FS_INT fnum, MULTI_DEL_CB pcallback);
extern FS_INT BF_Mdf_Wrapper_Fstat(FW_INT32 fNo, T_BF_FS_IF_STAT* sbuf);

/* Dir Control API */
extern FS_INT BF_Mdf_Wrapper_Mkdir(const char* dirname, FS_WORD mode);
extern FS_INT BF_Mdf_Wrapper_Rmdir(const char* dirname);
extern FS_INT BF_Mdf_Wrapper_Opendir(const char* dirname, FS_INT* dNo);
extern FS_INT BF_Mdf_Wrapper_Readdir(FW_INT32 dNo, T_BF_FS_IF_STAT* fsdirent);
extern FS_INT BF_Mdf_Wrapper_Closedir(FS_INT dNo);
extern FS_INT BF_Mdf_Wrapper_Chdir(const char* path);
extern FS_INT BF_Mdf_Wrapper_Get_Cur_Dir(const char* drvname, char* curpath, FS_DWORD size);
extern FS_INT BF_Mdf_Wrapper_Fsfirst(FS_BYTE index, const char* fname, FW_UCHAR attr, T_BF_FS_IF_ENTRY* buf);
extern FS_INT BF_Mdf_Wrapper_Fsnext(FS_BYTE index, T_BF_FS_IF_ENTRY* buf);

/* Ex API */
extern FS_INT BF_Mdf_Wrapper_EXT_Get_Qformat_Sectors(const char* drvname, FS_DWORD* sectors);
//extern FS_INT BF_Mdf_Wrapper_EXT_Stat(const char* fname, FS_EXT_STAT* extsbuf);
extern FS_INT BF_Mdf_Wrapper_EXT_Utime(const char* fname, const T_BF_FS_utimbuf* times, FS_WORD flag);


#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
/******************************************************************************
 for PrFILE2
******************************************************************************/
#include "fs_user_custom.h"
#include "fs_if.h"

#include "dskmng/pdm_dskmng.h"
#include "prfile2/pf_apicmn_def.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* open mode */
#define D_BF_MDF_WRAPPER_O_RDONLY			(0x0000)	// O_RDONLY		/* read only */
#define D_BF_MDF_WRAPPER_O_WRONLY			(0x0001)	// O_WRONLY		/* write only */
#define D_BF_MDF_WRAPPER_O_RDWR				(0x0002)	// O_RDWR  		/* read/write */
#define D_BF_MDF_WRAPPER_O_APPEND			(0x0008)	// O_APPEND		/* EOF write */
#define D_BF_MDF_WRAPPER_O_CREAT			(0x0100)	// O_CREAT 		/* create and open */
#define D_BF_MDF_WRAPPER_O_TRUNC			(0x0200)	// O_TRUNC 		/* open and trunc */

/* seek mode */
#define D_BF_MDF_WRAPPER_SEEK_CUR			(1)			// SEEK_CUR
#define D_BF_MDF_WRAPPER_SEEK_END			(2)			// SEEK_END
#define D_BF_MDF_WRAPPER_SEEK_SET			(0)			// SEEK_SET

/* attribute */
#define D_BF_MDF_WRAPPER_ATTR_NORMAL		(0x00)		/* Normal file or directory */
#define D_BF_MDF_WRAPPER_ATTR_READONLY		(0x01)		/* Read-only file or directory */
#define D_BF_MDF_WRAPPER_ATTR_HIDDEN		(0x02)		/* Hidden file or directory */
#define D_BF_MDF_WRAPPER_ATTR_SYSTEM		(0x04)		/* System file or directory */
#define D_BF_MDF_WRAPPER_ATTR_VOLUME		(0x08)		/* Volume label */
#define D_BF_MDF_WRAPPER_ATTR_DIRECTORY		(0x10)		/* Directory */
#define D_BF_MDF_WRAPPER_ATTR_ARCHIVE		(0x20)		/* Archive file or directory */
#define D_BF_MDF_WRAPPER_ATTR_NONE			(0x40)		/* None attribute file or directory */

#define D_BF_MDF_WRAPPER_SEM_W_TIME			(5000)			// MDF Wrapper semaphre timeout time (ms)

/* callback function */
typedef FW_INT32 (*MULTI_DEL_CB)(FW_INT32, FW_INT32);
typedef FW_INT32 (*MULTI_SET_ATTR_CB)(FW_INT32, FW_INT32);

/******************************************************************************
 Error Number
******************************************************************************/
/* PrFILE2 only */
#define D_BF_MDF_WRAPPER_ERR_SYSTEM				(-1)	//	(PF_ERR_SYSTEM)				/* (-1)		システムエラー																			*/
/* Common for MDF and PrFILE2 */
#define D_BF_MDF_WRAPPER_ENT_LAST				(1) 	//	(FS_ENT_LAST)				/* (1)		最後のファイルまたはディレクトリまで読み出しました。(FS_readdirのみ)					*/
#define D_BF_MDF_WRAPPER_ERR_NOENT				(-2)	//	(FS_ERR_NOENT)				/* (-2)		ファイルまたはディレクトリが存在しません。												*/
#define D_BF_MDF_WRAPPER_ERR_SRCH				(-3)	//	(FS_ERR_SRCH)				/* (-3)		対象タスクのデータが存在しません。														*/
#define D_BF_MDF_WRAPPER_ERR_IO					(-5)	//	(FS_ERR_IO)					/* (-5)		IOエラーが発生しました																	*/
#define D_BF_MDF_WRAPPER_ERR_NXIO				(-6)	//	(FS_ERR_NXIO)				/* (-6)		ドライブがありません（切断状態）														*/
#define D_BF_MDF_WRAPPER_ERR_BADF				(-9)	//	(FS_ERR_BADF)				/* (-9)		ファイル番号が不正です。																*/
#define D_BF_MDF_WRAPPER_ERR_NOMEM				(-12)	//	(FS_ERR_NOMEM)				/* (-12)	メモリの獲得に失敗しました。															*/
#define D_BF_MDF_WRAPPER_ERR_BUSY				(-16)	//	(FS_ERR_BUSY)				/* (-16)	ドライブがビジー状態です。																*/
#define D_BF_MDF_WRAPPER_ERR_EXIST				(-17)	//	(FS_ERR_EXIST)				/* (-17)	既にファイルまたはディレクトリが存在します。											*/
#define D_BF_MDF_WRAPPER_ERR_NODEV				(-19)	//	(FS_ERR_NODEV)				/* (-19)	指定のドライブがありません。															*/
#define D_BF_MDF_WRAPPER_ERR_NOTDIR				(-20)	//	(FS_ERR_NOTDIR)				/* (-20)	ディレクトリが存在しません。ディレクトリではありません。								*/
#define D_BF_MDF_WRAPPER_ERR_ISDIR				(-21)	//	(FS_ERR_ISDIR)				/* (-21)	ディレクトリに対して処理は行えません。													*/
#define	D_BF_MDF_WRAPPER_ERR_INVAL				(-22)	//	(FS_ERR_INVAL)				/* (-22)	引数の値が不正です。																	*/
#define D_BF_MDF_WRAPPER_ERR_MOPEN				(-24)	//	(FS_ERR_MOPEN)				/* (-24)	最大オープン数(最大継続処理数)を超えました。											*/
#define D_BF_MDF_WRAPPER_ERR_FBIG				(-27)	//	(FS_ERR_FBIG)				/* (-27)	扱えるファイルサイズを超えています。													*/
#define D_BF_MDF_WRAPPER_ERR_NOSPC				(-28)	//	(FS_ERR_NOSPC)				/* (-28)	ファイルシステムの空き領域が不足しています。											*/
#define D_BF_MDF_WRAPPER_ERR_SPIPE				(-29)	//	(FS_ERR_SPIPE)				/* (-29)	不正なseek です。																		*/
#define D_BF_MDF_WRAPPER_ERR_ROFS				(-30)	//	(FS_ERR_ROFS)				/* (-30)	読み込み専用のドライブあるいはファイルに書き込みは行えません。							*/
#define D_BF_MDF_WRAPPER_ERR_NOSYS				(-38)	//	(FS_ERR_NOSYS)				/* (-38)	実装されていない機能です。																*/
#define D_BF_MDF_WRAPPER_ERR_NOTEMPTY			(-39)	//	(FS_ERR_NOTEMPTY)			/* (-39)	ディレクトリが空になっていません														*/
#define D_BF_MDF_WRAPPER_ERR_NAMETOOLONG		(-78)	//	(FS_ERR_NAMETOOLONG)		/* (-78)	ファイル名またはディレクトリ名が所定の長さを超えています。								*/
#define D_BF_MDF_WRAPPER_ERR_DELENT				(-2)	//	(FS_ERR_NOENT)				/* (-2)		ファイルまたはディレクトリが消去されています											*/
#define D_BF_MDF_WRAPPER_ERR_EOF				(-501)	//	(FS_ERR_EOF)				/* (-501)	読み込みでファイルの終端に達した場合													*/
#define D_BF_MDF_WRAPPER_ERR_EXCLUSIVE			(-502)	//	(FS_ERR_EXCLUSIVE)			/* (-502)	ファイルまたはディレクトリは既に使用されています。										*/
#define D_BF_MDF_WRAPPER_ERR_RDONLY				(-30)	//	(FS_ERR_ROFS)				/* (-30)	読み込み専用のオープンモード															*/
#define D_BF_MDF_WRAPPER_ERR_WRONLY				(-511)	//	(FS_ERR_WRONLY)				/* (-511)	書き込み専用のオープンモード															*/
#define D_BF_MDF_WRAPPER_ERR_NAMEERR			(-520)	//	(FS_ERR_NAMEERR)			/* (-520)	ファイルまたはディレクトリ名に使用できない文字が含まれています。						*/
#define D_BF_MDF_WRAPPER_ERR_MDF				(-600)	//	(FS_ERR_MDF)				/* (-600)	既にMDFは開始されています。																*/
#define D_BF_MDF_WRAPPER_ERR_NOMDF				(-601)	//	(FS_ERR_NOMDF)				/* (-601)	MDFが開始されていません。																*/
#define D_BF_MDF_WRAPPER_ERR_MDRV				(-610)	//	(FS_ERR_MDRV)				/* (-610)	使用可能なドライブ登録数を超えました。													*/
#define D_BF_MDF_WRAPPER_ERR_DEVENT				(-611)	//	(FS_ERR_DEVENT)				/* (-611)	同じドライブ名が登録されています。														*/
#define D_BF_MDF_WRAPPER_ERR_NODRV				(-19)	//	(FS_ERR_NODEV)				/* (-19)	ドライブが初期化されていません。														*/
#define D_BF_MDF_WRAPPER_ERR_DRVUSE				(-16)	//	(FS_ERR_BUSY)				/* (-16)	ドライブは使用中です。																	*/
#define D_BF_MDF_WRAPPER_ERR_SYNC				(-16)	//	(FS_ERR_BUSY)				/* (-16)	ドライブは同調中です。																	*/
#define D_BF_MDF_WRAPPER_ERR_FORMAT				(-16)	//	(FS_ERR_BUSY)				/* (-16)	フォーマット中です。																	*/
#define D_BF_MDF_WRAPPER_ERR_NOCONDRV			(-6)	//	(FS_ERR_NXIO)				/* (-6)		ドライブが切断状態されています。														*/
#define D_BF_MDF_WRAPPER_ERR_MTASK				(-620)	//	(FS_ERR_MTASK)				/* (-620)	MDFにアクセス可能なタスク数を越えました。												*/
#define D_BF_MDF_WRAPPER_ERR_NOTASK				(-3)	//	(FS_ERR_SRCH)				/* (-3)		対象タスクのデータが存在しません。														*/
#define D_BF_MDF_WRAPPER_ERR_NOSEM				(-630)	//	(FS_ERR_NOSEM)				/* (-630)	空きセマフォがありません。																*/
#define D_BF_MDF_WRAPPER_ERR_CRESEM				(-631)	//	(FS_ERR_CRESEM)				/* (-631)	セマフォが生成できませんでした。														*/
#define D_BF_MDF_WRAPPER_ERR_DELSEM				(-632)	//	(FS_ERR_DELSEM)				/* (-632)	セマフォが削除できませんでした。														*/
#define D_BF_MDF_WRAPPER_ERR_ENQSEM				(-633)	//	(FS_ERR_ENQSEM)				/* (-633)	セマフォの排他ができませんでした。														*/
#define D_BF_MDF_WRAPPER_ERR_ENQSEMTMOUT		(-634)	//	(FS_ERR_ENQSEMTMOUT)		/* (-634)	セマフォの排他でタイムアウトしました。													*/
#define D_BF_MDF_WRAPPER_ERR_DEQSEM				(-635)	//	(FS_ERR_DEQSEM)				/* (-635)	セマフォの排他解除ができませんでした。													*/
#define D_BF_MDF_WRAPPER_ERR_UNCORRESPONDING	(-801)	//	(FS_ERR_UNCORRESPONDING)	/* (-801)	対応していないフォーマットです。														*/
#define D_BF_MDF_WRAPPER_ERR_NOROOT				(-810)	//	(FS_ERR_NOROOT)				/* (-810)	ルートディレクトリに空きディレクトリエントリがありません。								*/
#define D_BF_MDF_WRAPPER_ERR_INVOKE				(-820)	//	(FS_ERR_INVOKE)				/* (-820)	実行することができない処理・関数です。													*/
/* '-900'より小さいエラーコードは、
 * ファイルシステムを継続して処理することができないエラーです。
 * 媒体の使用を中止し、媒体チェックあるいはフォーマットを行ってください。
 * 継続して処理を行った場合は、データが壊れる可能性があります。
 */
#define D_BF_MDF_WRAPPER_ERR_FAT_READ			(-901)	//	(FS_ERR_FAT_READ)			/* (-901)	FAT領域の読み込みに失敗しました。														*/
#define D_BF_MDF_WRAPPER_ERR_FAT_WRITE			(-902)	//	(FS_ERR_FAT_WRITE)			/* (-902)	FAT領域に書き込みに失敗しました。														*/
#define D_BF_MDF_WRAPPER_ERR_ROOT_READ			(-903)	//	(FS_ERR_ROOT_READ)			/* (-903)	FAT12,16のルート領域の読み込みに失敗しました。											*/
#define D_BF_MDF_WRAPPER_ERR_ROOT_WRITE			(-904)	//	(FS_ERR_ROOT_WRITE)			/* (-904)	FAT12,16のルート領域の書き込み、初期化に失敗しました。									*/
#define D_BF_MDF_WRAPPER_ERR_BADREAD			(-905)	//	(FS_ERR_BADREAD)			/* (-905)	不良領域(クラスタ)のリードが失敗したため、代替処理がおこなえませんでした				*/
#define D_BF_MDF_WRAPPER_ERR_BADWRITE			(-906)	//	(FS_ERR_BADWRITE)			/* (-906)	不良領域(クラスタ)のライトがリトライ回数失敗したため、代替処理がおこなえませんでした	*/
#define D_BF_MDF_WRAPPER_ERR_FSNOADJ			(-910)	//	(FS_ERR_FSNOADJ)			/* (-910)	使用ファイルシステムと媒体に不整合があります											*/
#define D_BF_MDF_WRAPPER_ERR_DIRERR				(-911)	//	(FS_ERR_DIRERR)				/* (-911)	ディレクトリエントリの構成に、異常があります。											*/
#define D_BF_MDF_WRAPPER_ERR_CHAIN				(-912)	//	(FS_ERR_CHAIN)				/* (-912)	クラスタチェーンの構成に、異常があります。												*/
#define D_BF_MDF_WRAPPER_ERR_FSID				(-999)	//	(FS_ERR_FSID)				/* (-999)	ファイルシステムでエラーが発生しました。												*/
/* '-1000'より小さいエラーコードはドライバ関数のエラーコードを参照して下さい。*/
#define D_BF_MDF_WRAPPER_DRV_ERR_INVALID_DATA	(-1000)	//	(FS_DRV_ERR_INVALID_DATA)	/* (-1000)	カード抜等によりデータ無効になりました													*/



/*---------------------------------------------------------------*/
/* Typedef														 */
/*---------------------------------------------------------------*/
typedef signed char T_FS_CHAR;        /* char type */

typedef signed int T_FS_INT;          /* int type */
typedef unsigned int T_FS_UINT;       /* int type */

typedef signed char  T_FS_INT8;       /* 8bit integer with sign */
typedef signed short T_FS_INT16;      /* 16bit integer with sign */
typedef signed long  T_FS_INT32;      /* 32bit integer with sign */
typedef signed long long T_FS_INT64;  /* 64bit integer with sign */

typedef unsigned char T_FS_BYTE;      /* Byte(integer of sign none 8bit) */
typedef unsigned short T_FS_WORD;     /* Word(integer of sign none 16bit) */
typedef unsigned long T_FS_DWORD;     /* Double word(integer of sign none 32bit) */
typedef unsigned long long T_FS_QWORD;/* Quadruple word(integer of sign none 64bit) */

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Driver function information */
typedef struct {
    FW_INT32 (*init)(PDM_DISK *);                            /* Sector reading processing */
    FW_INT32 (*finalize)(PDM_DISK *);                        /* Sector writing processing */
    FW_INT32 (*mount)(PDM_DISK *);                           /* Sector clear              */
    FW_INT32 (*unmount)(PDM_DISK *);                         /* Sector size obtained      */
    FW_INT32 (*format)(PDM_DISK *, const FW_UCHAR *);        /* Configures reading and writing mode */
    FW_INT32 (*physical_read)(PDM_DISK *, FW_UCHAR *, FW_ULONG, FW_ULONG, FW_ULONG *);          /* Configures reading and writing mode */
    FW_INT32 (*physical_write)(PDM_DISK *, const FW_UCHAR *, FW_ULONG, FW_ULONG, FW_ULONG *);   /* Configures reading and writing mode */
    FW_INT32 (*get_disk_info)(PDM_DISK *, PDM_DISK_INFO *);  /* Configures reading and writing mode */
} T_FS_FUNCNAME;

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

/*******************************************************************************
 Define MDF Wrapper Function
*******************************************************************************/
/* MDF start&end API */
extern FW_INT32 BF_Mdf_Wrapper_Start_Mdf(FW_VOID);
extern FW_INT32 BF_Mdf_Wrapper_End_Mdf(FW_VOID);
extern FW_INT32 BF_Mdf_Wrapper_End_Task(FW_VOID);

/* Init&End API */
extern FW_INT32 BF_Mdf_Wrapper_Init_Drv(char* drvname, T_FS_FUNCNAME* pDrvinfo, FW_VOID* pDevinfo);
extern FW_INT32 BF_Mdf_Wrapper_End_Drv(char* drvname);

/* Drive API */
extern FW_INT32 BF_Mdf_Wrapper_Qformat(char* drvname);
extern FW_INT32 BF_Mdf_Wrapper_Nformat(T_FS_FUNCNAME *pDrvinfo, FW_VOID *pDevinfo);
extern FW_INT32 BF_Mdf_Wrapper_Statvfs(const char* drvname, T_BF_FS_IF_STATVFS* buf);
extern FW_INT32 BF_Mdf_Wrapper_Get_Free_Space(const char* drvname, FW_ULONG* blksize, FW_ULONG* blocks, FW_ULONG* bfree);
extern DOUBLE BF_Mdf_Wrapper_Get_Free_Space_By_Cache(const char* drvname);
extern FW_INT32 BF_Mdf_Wrapper_Get_Cur_Drv(char* drvname);
extern FW_INT32 BF_Mdf_Wrapper_Set_Vol_Label(const char* drvname, const char* label);
extern FW_INT32 BF_Mdf_Wrapper_Get_Vol_Label(const char* drvname, char* label);
extern FW_INT32 BF_Mdf_Wrapper_Fsync(FW_INT32 fNo);
extern FW_INT32 BF_Mdf_Wrapper_Sync(const char* drvname);
extern FW_INT32 BF_Mdf_Wrapper_Set_Con_Sec_Max(const char* drvname, FW_ULONG secmax);
extern FW_INT32 BF_Mdf_Wrapper_Set_Protection_Mode(const char* drvname, FW_ULONG mode);

/* Info Control API */
extern FW_INT32 BF_Mdf_Wrapper_Rename(const char* srcname, const char* dstname);
extern FW_INT32 BF_Mdf_Wrapper_Stat(const char* fname, T_BF_FS_IF_STAT* sbuf);
extern FW_INT32 BF_Mdf_Wrapper_Set_Attribute(const char* fname, FW_UCHAR attributes);
extern FW_INT32 BF_Mdf_Wrapper_Set_Attribute_For_Multi_Set(const char* fname, FW_UCHAR attributes);
//extern FW_INT32 BF_Mdf_Wrapper_Set_Attribute_Current_Entry(FW_INT32 dNo, FS_dirent* fsentry, FW_UCHAR attributes);
extern FW_INT32 BF_Mdf_Wrapper_Set_Attribute_Multi(const char* dirname, FW_UCHAR attributes, FW_UCHAR attr_mode, char filename_list[][13], FW_INT32 fnum, MULTI_SET_ATTR_CB pcallback);
extern FW_INT32 BF_Mdf_Wrapper_Get_Attribute(const char* fname, FW_UCHAR* attributes);
extern FW_INT32 BF_Mdf_Wrapper_Utime(const char* fname, const T_BF_FS_utimbuf* times);
extern FW_INT32 BF_Mdf_Wrapper_UtimeEx(const char* fname, FW_ULONG utime_mode, const T_BF_FS_utimbufEx *times);

/* File Control API */
extern FW_INT32 BF_Mdf_Wrapper_Open(const char* fname, FW_USHORT mode, FW_USHORT amode);
extern FW_INT32 BF_Mdf_Wrapper_Close(FW_INT32 fNo);
extern FW_INT32 BF_Mdf_Wrapper_Read(FW_INT32 fNo, FW_VOID* cbuf, FW_ULONG count, FW_ULONG* length);
extern FW_INT32 BF_Mdf_Wrapper_Write(FW_INT32 fNo, const FW_VOID* cbuf, FW_ULONG count, FW_ULONG* length);
extern FW_INT32 BF_Mdf_Wrapper_Lseek(FW_INT32 fNo, FW_LONG offset, FW_USHORT whence, FW_ULONG* length);
extern FW_INT32 BF_Mdf_Wrapper_Lseek64(FW_INT32 fNo, FW_INT64 offset, FW_USHORT whence, FW_INT64* length);
extern FW_INT32 BF_Mdf_Wrapper_Create(const char* fname, FW_USHORT amode);
extern FW_INT32 BF_Mdf_Wrapper_Copy(const char* srcname, const char* dstname);
extern FW_INT32 BF_Mdf_Wrapper_Truncate(const char* fname, FW_ULONG size, FW_ULONG* length);
extern FW_INT32 BF_Mdf_Wrapper_Truncate64(const char* fname, FW_ULLONG size, FW_ULLONG* length);
extern FW_INT32 BF_Mdf_Wrapper_Ftruncate(FW_INT32 fNo, FW_ULONG size, FW_ULONG* length);
extern FW_INT32 BF_Mdf_Wrapper_Ftruncate64(FW_INT32 fNo, FW_ULLONG size, FW_ULLONG* length);
extern FW_INT32 BF_Mdf_Wrapper_Remove(const char* fname);
extern FW_INT32 BF_Mdf_Wrapper_Remove_Without_Free_Space_Cache(const char* fname);
//extern FW_INT32 BF_Mdf_Wrapper_Remove_For_Multi_Delete(const char* fname);
//extern FW_INT32 BF_Mdf_Wrapper_Remove_Current_Entry(FW_INT32 dNo, FS_dirent* fsdirent);
extern FW_INT32 BF_Mdf_Wrapper_Remove_Multi(const char *dirname, char filename_list[][13], FW_INT32 fnum, MULTI_DEL_CB pcallback);
extern FW_INT32 BF_Mdf_Wrapper_Fstat(FW_INT32 fNo, T_BF_FS_IF_STAT* sbuf);

/* Dir Control API */
extern FW_INT32 BF_Mdf_Wrapper_Mkdir(const char* dirname, FW_USHORT mode);
extern FW_INT32 BF_Mdf_Wrapper_Rmdir(const char* dirname);
extern FW_INT32 BF_Mdf_Wrapper_Opendir(const char* dirname, FW_INT32* dNo);
extern FW_INT32 BF_Mdf_Wrapper_Readdir(FW_INT32 dNo, T_BF_FS_IF_STAT* fsdirent);
extern FW_INT32 BF_Mdf_Wrapper_Closedir(FW_INT32 dNo);
extern FW_INT32 BF_Mdf_Wrapper_Chdir(const char* path);
extern FW_INT32 BF_Mdf_Wrapper_Get_Cur_Dir(const char* drvname, char* curpath, FW_ULONG size);
extern FW_INT32 BF_Mdf_Wrapper_Fsfirst(FW_UCHAR index, const char* fname, FW_UCHAR attr, T_BF_FS_IF_ENTRY* buf);
extern FW_INT32 BF_Mdf_Wrapper_Fsnext(FW_UCHAR index, T_BF_FS_IF_ENTRY* buf);

/* Ex API */
extern FW_INT32 BF_Mdf_Wrapper_EXT_Get_Qformat_Sectors(const char* drvname, FW_ULONG* sectors);
//extern FW_INT32 BF_Mdf_Wrapper_EXT_Stat(const char* fname, FS_EXT_STAT* extsbuf);
extern FW_INT32 BF_Mdf_Wrapper_EXT_Utime(const char* fname, const T_BF_FS_utimbuf* times, FW_USHORT flag);
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

#endif
