/**
 * @file		media_state.h
 * @brief		Media status manager(header).
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _MEDIA_STATE_H_
#define _MEDIA_STATE_H_

#include "fs_user_custom.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_BF_MEDIA_SD_MAX				(2)

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// media type
typedef enum {
	E_BF_MEDIA_STATE_MEDIA_TYPE_NOTHING  = 0,		// no type
	E_BF_MEDIA_STATE_MEDIA_TYPE_CUR = 1,		// current media
	E_BF_MEDIA_STATE_MEDIA_TYPE_SD  = 2,		// SD memory card
	E_BF_MEDIA_STATE_MEDIA_TYPE_MS  = 3,		// MemoryStick
	E_BF_MEDIA_STATE_MEDIA_TYPE_NF  = 4,		// Nand Flash
	E_BF_MEDIA_STATE_MEDIA_TYPE_CF  = 5,		// Compact Flash
	E_BF_MEDIA_STATE_MEDIA_TYPE_SD1 = 6,		// SDIO ch1
	E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC = 7		// eMMC
} E_BF_MEDIA_STATE_MEDIA_TYPE;

// media state
typedef enum {
	E_BF_MEDIA_STATE_MEDIA_STATE_LEAVE  = 0,	// leave
	E_BF_MEDIA_STATE_MEDIA_STATE_INSERT = 1,	// insert
	E_BF_MEDIA_STATE_MEDIA_STATE_NONE   = 2	// ERROR
} E_BF_MEDIA_STATE_MEDIA_STATE;

// write protect status
typedef enum {
	E_BF_MEDIA_STATE_MEDIA_WP_UNPROTECT = 0,	// not write protect
	E_BF_MEDIA_STATE_MEDIA_WP_PROTECT   = 1,	// write protect
	E_BF_MEDIA_STATE_MEDIA_WP_NONE      = 2	// ERROR
} E_BF_MEDIA_STATE_MEDIA_WP;

typedef enum {
	E_BF_MEDIA_STATE_BOOT_TYPE_NORMAL   = 0,
	E_BF_MEDIA_STATE_BOOT_TYPE_NOTDB    = 1,
	E_BF_MEDIA_STATE_BOOT_TYPE_NOTFIRST = 2
} E_BF_MEDIA_STATE_BOOT_TYPE;

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/**
 * @brief local media information
 * @note none
 * @attention none
 */
typedef struct {
	E_BF_MEDIA_STATE_MEDIA_WP sd_wp;			// sd write protect state
	E_BF_MEDIA_STATE_MEDIA_WP nf_wp;			// nf write protect state
	E_BF_MEDIA_STATE_MEDIA_WP cf_wp;			// cf write protect state
	E_BF_MEDIA_STATE_MEDIA_WP emmc_wp;			// emmc write protect state
} T_BF_MEDIA_STATE_MEDIA_INFO;

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
extern FW_VOID       BF_Media_State_Set_Used_Media(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);
extern E_BF_MEDIA_STATE_MEDIA_TYPE  BF_Media_State_Get_Used_Media(FW_VOID);
extern FW_INT32      BF_Media_State_Exchage_Type_To_Drive_Name(E_BF_MEDIA_STATE_MEDIA_TYPE media_type, FW_CHAR* drive_name);
extern FW_VOID       BF_Media_State_Set_Media_Check_Param(FW_UCHAR ch, FW_UCHAR chk_interval, FW_UCHAR chk_cnt);
extern E_BF_MEDIA_STATE_MEDIA_STATE BF_Media_State_Get_Media_State(FW_UCHAR ch);
extern E_BF_MEDIA_STATE_MEDIA_STATE BF_Media_State_Get_Media_State_By_Poll(FW_UCHAR ch, FW_UCHAR interval, FW_UCHAR count);
extern E_BF_MEDIA_STATE_MEDIA_STATE BF_Media_State_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);
extern FW_VOID       BF_Media_State_Set_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE media_type, E_BF_MEDIA_STATE_MEDIA_WP media_wp);
extern E_BF_MEDIA_STATE_MEDIA_WP    BF_Media_State_Get_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE);
extern FW_VOID BF_Media_Set_NF_Storage_ID(FW_UCHAR id);
extern FW_VOID BF_Media_Set_EMMC_Storage_ID(FW_UCHAR id);
extern FW_UCHAR BF_Media_Get_NF_Storage_ID(FW_VOID);
extern FW_UCHAR BF_Media_Get_EMMC_Storage_ID(FW_VOID);

#endif
