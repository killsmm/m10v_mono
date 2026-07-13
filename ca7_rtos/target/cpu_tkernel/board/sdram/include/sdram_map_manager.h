/**
 * @file		sram_map_manager.h
 * @brief		SDRAM map manager for Multi-channel distribution
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _SRAM_MAP_MANAGER_H_
#define _SRAM_MAP_MANAGER_H_

#include "image_info.h"


/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
typedef enum {
	E_SDRAM_MAP_MGR_MODE_VIDEO = 0,
	E_SDRAM_MAP_MGR_MODE_CAP,
	E_SDRAM_MAP_MGR_MODE_BURST,
	E_SDRAM_MAP_MGR_MODE_PIV,
	E_SDRAM_MAP_MGR_MODE_3DNR_EXT,
	E_SDRAM_MAP_MGR_MODE_STITCH,
	E_SDRAM_MAP_MGR_MODE_STILL_STITCH,
	E_SDRAM_MAP_MGR_MODE_MULTI_OUT,
	E_SDRAM_MAP_MGR_MODE_STILL_HDR,
	E_SDRAM_MAP_MGR_MODE_MAX
} E_SDRAM_MAP_MGR_MODE;

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Macro														 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Local Function												 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Global Function												 */
/*---------------------------------------------------------------*/

//---------------------------//
// MXIC                      //
//---------------------------//
extern VOID Sdram_Map_Mgr_Init_MxicSlaveRegion( VOID );

//---------------------------//
// Common                    //
//---------------------------//

extern VOID Sdram_Map_Mgr_Set_SdramMap_Mode( E_SDRAM_MAP_MGR_MODE map_mode );
extern ULONG Sdram_Map_Mgr_Get_SdramMap_Mode( VOID );
extern	ULONG Sdram_Map_Mgr_Get_Movie_Record_Table_Idx( VOID );
extern	VOID Sdram_Map_Mgr_Init_Mode_Table( E_SDRAM_MAP_MGR_MODE sdram_map_mode );

#endif
