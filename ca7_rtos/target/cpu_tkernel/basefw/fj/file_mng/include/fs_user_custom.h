/**
 * @file		fs_user_custom.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FS_USER_CUSTOM_H_
#define _FS_USER_CUSTOM_H_

#include "chiptop.h"
#include "measure_time.h"
#include "debug.h"
#include "os_user_custom.h"
#include "mw_user_custom.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/

/******************************************************************
 DATA TYPE
******************************************************************/
#define FW_VOID void

typedef signed short		FW_SHORT;
typedef unsigned short		FW_USHORT;
typedef signed long 		FW_LONG;
typedef unsigned long		FW_ULONG;
typedef signed long long	FW_LLONG;
typedef unsigned long long	FW_ULLONG;
typedef char				FW_CHAR;
typedef unsigned char		FW_UCHAR;
typedef int					FW_INT32;
typedef unsigned int 	 	FW_UINT32;
typedef long long			FW_INT64;
typedef unsigned long long	FW_UINT64;
typedef double				FW_DOUBLE;
typedef float				FW_FLOAT;

/*******************************************************************
 Fix Define
*******************************************************************/
#ifndef NULL
#define NULL	(0)
#endif

#ifndef TRUE
#define TRUE	(1)
#endif
#ifndef FALSE
#define FALSE	(0)
#endif

/* local return value */
#define D_BF_FS_USER_CUSTOM_OK							(0)		// normal end
#define D_BF_FS_USER_CUSTOM_ERR							(-1)	// undefine error
#define D_BF_FS_USER_CUSTOM_FLG_MATCH					(0)
#define D_BF_FS_USER_CUSTOM_FLG_UNMATCH					(-85)
#define D_BF_FS_USER_CUSTOM_FLG_ERR						(-1)

/* flag */
#define D_BF_FS_USER_CUSTOM_FLG_FILE_SYSTEM_CANCEL		(0x00000001)
#define D_BF_FS_USER_CUSTOM_FLG_FILE_SYSTEM_INIT_END	(0x00000002)

/******************************************************************
 Custom Define
******************************************************************/
/////////////////////////
// Media Define
/////////////////////////
#ifndef _MEDIA_STATE_H_
#include "media_state.h"
#endif

/* Save side definition */
#define D_BF_FS_USER_CUSTOM_MEDIA_NF_0 		(0)		/* NF area0 */
#define D_BF_FS_USER_CUSTOM_MEDIA_EMMC_0	(1)		/* eMMC area0 */
#define D_BF_FS_USER_CUSTOM_MEDIA_NF_1 		(2)		/* NF area1 */
#define D_BF_FS_USER_CUSTOM_MEDIA_EMMC_1	(3)		/* eMMC area1 */

/////////////////////////
// FS Define
/////////////////////////
/* measure time */
//#define CO_BF_FS_USER_CUSTOM_MEASURE_TIME

/* task priority */
//#define CO_BF_FS_USER_CUSTOM_FSIF_CHG_PRI
#define CO_BF_FS_USER_CUSTOM_CHG_PRI_INSIDE_FS_RECEIVER

/* fs_if print */
#define CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT

/* fs_controller print */
#define CO_BF_FS_USER_CUSTOM_FSC_WRN_PRINT
#define CO_BF_FS_USER_CUSTOM_FSC_ERR_PRINT

/* fs_receiver print */
#define CO_BF_FS_USER_CUSTOM_FSR_INFO_PRINT
#define CO_BF_FS_USER_CUSTOM_FSR_ERR_PRINT

/* fs_sender print */
#define CO_BF_FS_USER_CUSTOM_FSS_ERR_PRINT

/* init retry counter */
#define D_BF_FS_USER_CUSTOM_FS_INIT_RETRY_CNT 			(2)				// BF_Mdf_Wrapper_Init_Drv() retry counter

/////////////////////////
// DCF Define
/////////////////////////
/* dcf_if print */
#define CO_BF_FS_USER_CUSTOM_DCFIF_ERR_PRINT

/* dcf_if_receiver print */
#define CO_BF_FS_USER_CUSTOM_DCFIR_ERR_PRINT

/* dcf_controller print */
//#define CO_BF_FS_USER_CUSTOM_DCFC_INFO_PRINT
#define CO_BF_FS_USER_CUSTOM_DCFC_WRN_PRINT
#define CO_BF_FS_USER_CUSTOM_DCFC_ERR_PRINT

/* dcf_db print */
//#define CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
#define CO_BF_FS_USER_CUSTOM_DCFDB_ERR_PRINT

/* measure time */
//#define CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME
//#define CO_BF_FS_USER_CUSTOM_DCFC_MEASURE_TIME_READ
//#define CO_BF_FS_USER_CUSTOM_DCFDB_MEASURE_TIME

/* support file */
//#define CO_BF_FS_USER_CUSTOM_DCFDB_MPEG4_MOV							// if this define is invalidated, AVI file is allowed, but MOV file is not allowed.
#define CO_BF_FS_USER_CUSTOM_DCFDB_RAW_DATA								// if this define is invalidated, "RAW" file is not allowed.
/* set align for write */
#define D_BF_FS_USER_CUSTOM_DCF_WRITE_ALIGN				(4)

/* num of DB */
#define D_BF_FS_USER_CUSTOM_DCF_DB_OBJ_INFO_NUM			(3 * 10000)       /* 3 folders and 10000 objects(from 0000 to 9999) */
#define D_BF_FS_USER_CUSTOM_DCF_DB_DIR_INFO_NUM			(999 - 100 + 1)     /* from 100 to 999 */

/* division read size */
#define D_BF_FS_USER_CUSTOM_DIVISION_READ_SIZE			(512 * 1024)      // 512(Kbyte)

/////////////////////////
// Exif Define
/////////////////////////
/* dcf_exif print */
#define CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
#define CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT

/* dcf_exif print */
#define CO_BF_FS_USER_CUSTOM_DCFDP_ERR_PRINT

/* use bcd code for date(if this define disable, use bin code) */
//#define CO_BF_FS_USER_CUSTOM_USE_BCD_DATE

/* exif updata option */
//#define CO_BF_FS_USER_CUSTOM_READ_ONLY_EDITABLE						// when rotate and resize, if file protected, this macro can update file.

/* exif buffer size */
#define D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE				(0x5000)	// 20(Kbyte)

/////////////////////////
// Dpof Define
/////////////////////////
#define D_BF_FS_USER_CUSTOM_DPOF_GEN_CRT					"\"DigitalCAM\" -01.10"


#define BF_Fs_User_Custom_Info_Printer		BF_Debug_Print_Information
#define BF_Fs_User_Custom_Wrn_Printer		BF_Debug_Print_Warning
#define BF_Fs_User_Custom_Err_Printer		BF_Debug_Print_Error

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/******************************************************************
 DCF typedef
******************************************************************/
/* RTC_DATE */
typedef struct {
	FW_USHORT	year;
	FW_UCHAR	month;
	FW_UCHAR	day;
	FW_UCHAR	hour;
	FW_UCHAR	min;
	FW_UCHAR	sec;
} T_FS_USER_CUSTOM_RTC_DATE;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/*******************************************************************
 Custom Define
*******************************************************************/
/////////////////////////
// Media Define
/////////////////////////
/* media insert state */
#define BF_Fs_User_Custom_Get_Sd0_Insert_State()			(E_BF_MEDIA_STATE_MEDIA_STATE)Media_Custom_SD0_Get_State()
#define BF_Fs_User_Custom_Get_Sd0_Insert_State_At_Gpio()	(E_BF_MEDIA_STATE_MEDIA_STATE)Media_Custom_SD0_Get_SDCDX()
#define BF_Fs_User_Custom_Get_Sd0_Insert_Init_End()			0

#define BF_Fs_User_Custom_Get_Sd1_Insert_State()			(E_BF_MEDIA_STATE_MEDIA_STATE)Media_Custom_SD1_Get_State()
#define BF_Fs_User_Custom_Get_Sd1_Insert_State_At_Gpio()	(E_BF_MEDIA_STATE_MEDIA_STATE)Media_Custom_SD1_Get_SDCDX()
#define BF_Fs_User_Custom_Get_Sd1_Insert_Init_End()			0

#define D_BF_FS_USER_CUSTOM_NF_INSERT_STATE()				(E_BF_MEDIA_STATE_MEDIA_STATE_INSERT)
#define D_BF_FS_USER_CUSTOM_EMMC_INSERT_STATE()				(E_BF_MEDIA_STATE_MEDIA_STATE_INSERT)

#define D_BF_FS_USER_CUSTOM_CNT_RATE						(1000000)		// Turnover rate for exception log

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/

/////////////////////////
// Debug Function
/////////////////////////
extern FW_VOID  BF_Fs_User_Custom_Measure_Time_Init(E_BF_MEASURE_TIME_ID id);
extern FW_VOID  BF_Fs_User_Custom_Measure_Time_Get(E_BF_MEASURE_TIME_ID id, FW_ULONG* ltime);
extern FW_VOID  BF_Fs_User_Custom_Measure_Time_Print(E_BF_MEASURE_TIME_ID id, const FW_CHAR* comment);
extern FW_VOID  BF_Fs_User_Custom_Get_Tim(FW_ULLONG* systime);

/////////////////////////
// FS/DCF Function
/////////////////////////
// task control
extern FW_VOID  BF_Fs_User_Custom_Wup_Tsk(OS_USER_ID tskid);
extern FW_VOID  BF_Fs_User_Custom_Slp_Tsk(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Tslp_Tsk(OS_USER_TMO tmo);
extern FW_INT32 BF_Fs_User_Custom_Dly_Tsk(OS_USER_RELTIM dlytim);
extern FW_INT32 BF_Fs_User_Custom_Get_Tid(OS_USER_ID* p_tskid);
extern FW_INT32 BF_Fs_User_Custom_Sig_Sem(FW_INT32 semid);
extern FW_INT32 BF_Fs_User_Custom_Twai_Sem(FW_INT32 semid, FW_INT32 tmout);
extern FW_INT32 BF_Fs_User_Custom_Dis_Dsp(FW_VOID);
extern FW_INT32 BF_Fs_User_Custom_Ena_Dsp(FW_VOID);

// event flag
extern FW_VOID  BF_Fs_User_Custom_Clr_Flg_Init_End(FW_VOID);
extern FW_INT32 BF_Fs_User_Custom_Pol_Flg_Init_End(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Set_Flg_Init_End(FW_VOID);
extern FW_INT32 BF_Fs_User_Custom_Twai_Flg_Init_End(FW_VOID);

/////////////////////////
// FS Function
/////////////////////////
// task control
extern FW_VOID  BF_Fs_User_Custom_Sta_Tsk_FS(FW_VOID);
extern FW_INT32 BF_Fs_User_Custom_Ref_Tsk_Fs(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Ini_Pri_Fs(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Chg_Pri_Fs(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Recover_Pri_Fs_With_User(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Sync_Pri_Fs_With_User(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Sta_Tsk_Ipcufs(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Sta_Tsk_Vbd(FW_VOID);
extern FW_INT32 BF_Fs_User_Custom_Ref_Tsk_Ipcufs(FW_VOID);

extern BOOL     BF_Fs_User_Custom_Is_It_Inside_Fs_Rcv(FW_VOID);

// event flag
extern FW_VOID  BF_Fs_User_Custom_Clr_Flg_Cancel(FW_VOID);
extern FW_INT32 BF_Fs_User_Custom_Pol_Flg_Cancel(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Set_Flg_Cancel(FW_VOID);

// mail box
extern FW_INT32 BF_Fs_User_Custom_Pget_Blf_Mbox(FW_VOID* pp);
extern FW_INT32 BF_Fs_User_Custom_Rel_Blf_Mbox(FW_VOID* p);
extern FW_INT32 BF_Fs_User_Custom_Ref_Mbx(FW_VOID** pp);
extern FW_INT32 BF_Fs_User_Custom_Ref_Mpf_Cnt(FW_VOID);

// message
extern FW_INT32 BF_Fs_User_Custom_Snd_Msg(FW_VOID* p);
extern FW_INT32 BF_Fs_User_Custom_Prcv_Msg(FW_VOID** pp);
extern FW_INT32 BF_Fs_User_Custom_Rcv_Msg(FW_VOID** pp);
extern FW_INT32 BF_Fs_User_Custom_Clr_Msg(FW_VOID);

/////////////////////////
// DCF Function
/////////////////////////
// task control
extern FW_VOID  BF_Fs_User_Custom_Sta_Tsk_Dcf(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Ini_Pri_Dcf(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Chg_Pri_Dcf(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Recover_Pri_Dcf_With_User(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Sync_Pri_Dcf_With_User(FW_VOID);

extern BOOL     BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv(FW_VOID);

// mail box
extern FW_INT32 BF_Fs_User_Custom_Pget_Blf_Mbox_Dcf(FW_VOID* pp);
extern FW_INT32 BF_Fs_User_Custom_Rel_Blf_Mbox_Dcf(FW_VOID* p);
extern FW_INT32 BF_Fs_User_Custom_Ref_Mbx_Dcf(FW_VOID** pp);
extern FW_INT32 BF_Fs_User_Custom_Ref_Mpf_Cnt_Dcf(FW_VOID);

// message
extern FW_INT32 BF_Fs_User_Custom_Snd_Msg_Dcf(FW_VOID* p);
extern FW_INT32 BF_Fs_User_Custom_Prcv_Msg_Dcf(FW_VOID** pp);
extern FW_INT32 BF_Fs_User_Custom_Rcv_Msg_Dcf(FW_VOID** pp);
extern FW_INT32 BF_Fs_User_Custom_Clr_Msg_Dcf(FW_VOID);


extern FW_INT32 BF_Fs_User_Custom_Get_Blf_Exif_Buf(FW_VOID** pp);
extern FW_INT32 BF_Fs_User_Custom_Rel_Blf_Exif_Buf(FW_VOID* p);
//extern FW_INT32 BF_Fs_User_Custom_Get_Blf_Db_Node(FW_VOID** pp);
//extern FW_INT32 BF_Fs_User_Custom_Rel_Blf_Db_Node(FW_VOID* p);
//extern FW_VOID  BF_Fs_User_Custom_Get_Rtc_Date(T_FS_USER_CUSTOM_RTC_DATE* rtc_date);		//RTC not used


extern FW_INT32 BF_Fs_User_Custom_Get_Mpl( FW_INT32 mpfid, FW_UINT32 blksz, FW_VOID* p_blk);
extern FW_INT32 BF_Fs_User_Custom_Pget_Mpl( FW_INT32 mpfid, FW_UINT32 blksz, FW_VOID* p_blk);
extern FW_INT32 BF_Fs_User_Custom_Rel_Mpl( FW_INT32 mpfid, FW_VOID* blk);



/////////////////////////
// Media Function
/////////////////////////
// IO port set
extern FW_VOID  BF_Fs_User_Custom_Set_Media_Interrupt(FW_UCHAR ch);
extern FW_VOID  BF_Fs_User_Custom_Set_SD_Port(FW_UCHAR ch);
extern FW_VOID  BF_Fs_User_Custom_Set_NF_Port(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Set_EMMC_Port(FW_VOID);

// Media clock start
extern FW_VOID  BF_Fs_User_Custom_Start_SD0_Clock(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Start_SD1_Clock(FW_VOID);
extern FW_INT32  BF_Fs_User_Custom_Start_NF_Clock(FW_SHORT timeout_XDMAC, FW_UCHAR exception_flg);
extern FW_VOID  BF_Fs_User_Custom_Start_EMMC_Clock(FW_VOID);

// Media clock stop
extern FW_VOID  BF_Fs_User_Custom_Stop_SD0_Clock(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Stop_SD1_Clock(FW_VOID);
extern FW_INT32  BF_Fs_User_Custom_Stop_NF_Clock(FW_SHORT timeout_NF, FW_SHORT timeout_XDMAC, FW_UCHAR exception_flg);
extern FW_VOID  BF_Fs_User_Custom_Stop_EMMC_Clock(FW_VOID);

// RELC clock control
extern FW_VOID  BF_Fs_User_Custom_Start_RELC_Clock(FW_VOID);
extern FW_VOID  BF_Fs_User_Custom_Stop_RELC_Clock(FW_VOID);

// Media timer
extern FW_VOID BF_Fs_User_Custom_Get_Time(T_SYSTEM_TIME* time);
extern FW_VOID BF_Fs_User_Custom_Get_Time_us(FW_ULONG* time);
extern FW_ULONG BF_Fs_User_Custom_Start_Abort_Timer(FW_UCHAR exception_flg);
extern FW_INT32 BF_Fs_User_Custom_Check_Abort_Timer(FW_ULONG timout, FW_ULONG start_time, FW_UCHAR exception_flg);
#endif
