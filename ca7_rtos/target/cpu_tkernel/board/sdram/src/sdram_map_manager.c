/**
 * @file		sdram_map_manager.c
 * @brief		SDRAM map manager for Multi-channel distribution
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "sdram_map_manager.h"
#include "sdram_map_movie_record.h"
#include "sdram_map_define.h"
#include "fj_still.h"
#include "im_mxic.h"


/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
//#define SDRAM_MAP_MGR_DEBUG_CODE

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
typedef enum {
	E_SDRAM_MAP_MGR_TABLE_TYPE_VIEW = 0,
	E_SDRAM_MAP_MGR_TABLE_TYPE_MOVIE_RECORD,
	E_SDRAM_MAP_MGR_TABLE_TYPE_MXIC_SDI,
	E_SDRAM_MAP_MGR_TABLE_TYPE_MXIC_MIF_SDI,
	E_SDRAM_MAP_MGR_TABLE_TYPE_CLKSEL,
	E_SDRAM_MAP_MGR_TABLE_TYPE_MAX
} E_SDRAM_MAP_MGR_TABLE_TYPE;

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
static volatile ULONG g_sdram_map_mgr_sdrammap_mode = 0;
static const ULONG g_sdram_map_mgr_table_idx[E_SDRAM_MAP_MGR_MODE_MAX][E_SDRAM_MAP_MGR_TABLE_TYPE_MAX] = 
{
	// (1)VIEW_MAP_IDX
	// (2)MOVIE_RECORD_MAP_IDX
	// (3)MXIC_SDI
	// (4)MXIC_MIF_SDI
	// (5)CLKSEL_SDI
	//
	//									   (1)  (2)  (3)  (4)  (5)		   Usecase
	[E_SDRAM_MAP_MGR_MODE_VIDEO]		= {	0,   0,   6,   0,   0,	},	// Video(View/Record)
	[E_SDRAM_MAP_MGR_MODE_CAP]			= {	2,   0,   1,   0,   0,	},	// Still Capture
	[E_SDRAM_MAP_MGR_MODE_BURST]		= {	3,   0,   1,   0,   0,	},	// Burst Capture
	[E_SDRAM_MAP_MGR_MODE_PIV]			= {	0,   0,   1,   0,   0,	},	// Picture In Video
	[E_SDRAM_MAP_MGR_MODE_3DNR_EXT]		= {	0,   0,   6,   0,   0,	},	// Video(View/Record) + special mxic set
	[E_SDRAM_MAP_MGR_MODE_STITCH]		= {	4,   1,   7,   0,   0,	},	// Stitch
	[E_SDRAM_MAP_MGR_MODE_STILL_STITCH]	= {	6,   0,   1,   0,   0,	},	// Still Stitch
	[E_SDRAM_MAP_MGR_MODE_MULTI_OUT]	= {	7,   0,   7,   0,   0,	},	// 2sensor 2stream output
	[E_SDRAM_MAP_MGR_MODE_STILL_HDR]	= {	5,   0,   1,   0,   0,	},	// Still HDR
};

/*---------------------------------------------------------------*/
/* Macro														 */
/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/
/* Local Function												 */
/*---------------------------------------------------------------*/
ULONG sdram_map_mgr_get_table_idx( E_SDRAM_MAP_MGR_TABLE_TYPE table_type )
{
	return g_sdram_map_mgr_table_idx[g_sdram_map_mgr_sdrammap_mode][table_type];
}

/*---------------------------------------------------------------*/
/* Global Function												 */
/*---------------------------------------------------------------*/

//---------------------------//
// MXIC                      //
//---------------------------//
VOID Sdram_Map_Mgr_Init_MxicSlaveRegion( VOID )
{
	static const T_IM_MXIC_ALL_SLAVE_AREA	slave_area = {
		.slave_area[E_IM_MXIC_SLAVE_NUMBER_1] = {	// SDRAM ch0
			.address	= SDRAM_ADR_CH0_TOP,
			.size		= (ULONG)(SDRAM_ADR_CH0_END - SDRAM_ADR_CH0_TOP),
		},
		.slave_area[E_IM_MXIC_SLAVE_NUMBER_2] = {	// SDRAM ch1
			.address	= SDRAM_ADR_CH1_TOP,
			.size		= (ULONG)(SDRAM_ADR_CH1_END - SDRAM_ADR_CH1_TOP),
		},
		.slave_area[E_IM_MXIC_SLAVE_NUMBER_3] = {	// D.C.
			.address	= 0x00000000,
			.size		= 0,
		},
		.slave_area[E_IM_MXIC_SLAVE_NUMBER_4] = {	// D.C.
			.address	= 0x00000000,
			.size		= 0,
		},
	};
	E_IM_MXIC_UNIT			unit;

	for( unit = E_IM_MXIC_UNIT_0; unit < E_IM_MXIC_UNIT_MAX; unit++ ) {
		Im_MXIC_Set_Slave_Area_All( unit, &slave_area );
	}
}


//---------------------------//
// Common                    //
//---------------------------//
VOID Sdram_Map_Mgr_Set_SdramMap_Mode( E_SDRAM_MAP_MGR_MODE map_mode )
{
	if( map_mode >= E_SDRAM_MAP_MGR_MODE_MAX ) {
		map_mode = E_SDRAM_MAP_MGR_MODE_VIDEO;
	}

	// Initialization of SDRAM MAP before map_mode value sets.
	Sdram_Map_Mgr_Init_Mode_Table( map_mode );

	g_sdram_map_mgr_sdrammap_mode = map_mode;
}

ULONG Sdram_Map_Mgr_Get_SdramMap_Mode( VOID )
{
	return g_sdram_map_mgr_sdrammap_mode;
}

ULONG Sdram_Map_Mgr_Get_Movie_Record_Table_Idx( VOID )
{
	return sdram_map_mgr_get_table_idx( E_SDRAM_MAP_MGR_TABLE_TYPE_MOVIE_RECORD );
}

VOID Sdram_Map_Mgr_Init_Mode_Table( E_SDRAM_MAP_MGR_MODE sdram_map_mode )
{
	if( sdram_map_mode >= E_SDRAM_MAP_MGR_MODE_MAX ) {
		sdram_map_mode = E_SDRAM_MAP_MGR_MODE_VIDEO;
	}
	
  //  	Sdram_Map_View_Init( g_sdram_map_mgr_table_idx[sdram_map_mode][E_SDRAM_MAP_MGR_TABLE_TYPE_VIEW] );
	
	// Movie Map
	switch( sdram_map_mode ){
		case E_SDRAM_MAP_MGR_MODE_VIDEO:
		case E_SDRAM_MAP_MGR_MODE_CAP:
		case E_SDRAM_MAP_MGR_MODE_BURST:
		case E_SDRAM_MAP_MGR_MODE_PIV:
		case E_SDRAM_MAP_MGR_MODE_3DNR_EXT:
		case E_SDRAM_MAP_MGR_MODE_STILL_HDR:
			Sdram_Map_Movie_Record_Initialize( E_SDRAM_MAP_MOVIE_RECORD_PATTERN_0 );
			break;
		case E_SDRAM_MAP_MGR_MODE_STITCH:
		case E_SDRAM_MAP_MGR_MODE_STILL_STITCH:
			Sdram_Map_Movie_Record_Initialize( E_SDRAM_MAP_MOVIE_RECORD_PATTERN_STITCH );
			break;
		default :
			Sdram_Map_Movie_Record_Initialize( E_SDRAM_MAP_MOVIE_RECORD_PATTERN_MAX );
			break;
	}
}

