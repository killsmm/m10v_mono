/**
 * @file		fsdcf_errno.h
 * @brief		error number definition for FS & DCF.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FSDCF_ERRNO_H_
#define _FSDCF_ERRNO_H_


/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/

#define FSDCF_ERRNO_OK							(0)

/* use BF_Fs_If_Wait_Fs_Init_End(..),BF_Fs_If_Check_Fs_Init_End(..) return code */
#define FSDCF_ERRNO_FS_INIT_NOT_END				(2)
#define FSDCF_ERRNO_FS_INIT_END					(3)
#define FSDCF_ERRNO_FS_INIT_TIMEOUT				(4)

#define FSDCF_ERRNO_NOT_SUPPORT_MEDIA			(-101)
#define FSDCF_ERRNO_NOT_SUPPORT_DIRNO			(-102)
#define FSDCF_ERRNO_NOT_SUPPORT_FILENO			(-103)
#define FSDCF_ERRNO_NOT_DCIM					(-104)
#define FSDCF_ERRNO_NOT_DIR						(-105)
#define FSDCF_ERRNO_DIR_FULL					(-106)
#define FSDCF_ERRNO_FILE_FULL					(-107)
#define FSDCF_ERRNO_NOT_APP1					(-108)
#define FSDCF_ERRNO_EXIF_TAG_OVER				(-109)
#define FSDCF_ERRNO_MALLOC_LIST					(-110)
#define FSDCF_ERRNO_NOT_INSERT_MEDIA			(-111)
#define FSDCF_ERRNO_FILE_INDEX					(-112)
#define FSDCF_ERRNO_FREE_SPACE_OVER				(-113)
#define FSDCF_ERRNO_APP1_SIZE_OVER				(-114)
#define FSDCF_ERRNO_NOT_SUPPORT_FILE			(-115)
#define FSDCF_ERRNO_TARGET_FILE_NOT_EXIST		(-116)
#define FSDCF_ERRNO_THUMB_SIZE_OVER				(-117)
#define FSDCF_ERRNO_TOUCH_NULL_POINTER			(-118)
#define FSDCF_ERRNO_DPOF						(-119)
#define FSDCF_ERRNO_NOT_SUPPORT_DPOF_FILE		(-120)
#define FSDCF_ERRNO_SYSTEM_FAILED				(-121)
#define FSDCF_ERRNO_DRIVE_INIT_FAILED			(-122)
#define FSDCF_ERRNO_DRIVE_FORMAT_FAILED			(-123)
#define FSDCF_ERRNO_PARAM_FAILED				(-124)
#define FSDCF_ERRNO_CREATE_FILE					(-125)
#define FSDCF_ERRNO_NOT_THUMBNAIL				(-126)

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
// MDF
#define FSDCF_ERRNO_ERR_INVAL					(-22)	// D_BF_MDF_WRAPPER_ERR_INVAL	(-22)	//	(FS_ERR_INVAL)	à¯êîÇÃílÇ™ïsê≥Ç≈Ç∑ÅB
#define FSDCF_ERRNO_ERR_ROFS					(-30)	// 							FS_ERR_ROFS
#define FSDCF_ERRNO_ERR_NAMETOOLONG				(-78)	// D_BF_MDF_WRAPPER_ERR_NAMETOOLONG, 

#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---

//PrFile2												// Error Code			Internal Error code
#define FSDCF_ERRNO_ERR_INVAL					(22)	// PF_ERR_EINVAL (22)	
#define FSDCF_ERRNO_ERR_ROFS					(13)	// PF_ERR_EACCES (13)	
#define FSDCF_ERRNO_ERR_NAMETOOLONG				(22)	// PF_ERR_EINVAL (22)	PF_IER_INVALID_FILENAME(1)	// File name is invalid

#endif // CO_FS_USED_MDF				// --- REMOVE_MDF END ---
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
/* Function														  */
/*----------------------------------------------------------------*/
// Nothing Special

#endif
