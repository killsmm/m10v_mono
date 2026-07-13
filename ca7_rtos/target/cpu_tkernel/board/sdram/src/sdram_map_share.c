/**
 * @file		sdram_map_share.c
 * @brief		SDRAM map for share
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "sdram_map_common.h"
#include "sdram_map_define.h"
#include "sdram_map_movie_record_pattern0_define.h"
//#include "sdram_map_still_mode0_define.h"
#include "sdram_map_play_define.h"
#include "sdram_map_manager.h"
#include "mode.h"

typedef struct {
	UINT32 adrs;
	UINT32 size;
} T_SDRAM_SHARE_INFO;

static const T_SDRAM_SHARE_INFO gSdram_Common_ShareInfoTbl[] = {
	{SDRAM_ADR_COMMON_0, SDRAM_SIZ_COMMON_0},
	{SDRAM_ADR_COMMON_1, SDRAM_SIZ_COMMON_1},
	{SDRAM_ADR_COMMON_2, SDRAM_SIZ_COMMON_2},
	{SDRAM_ADR_COMMON_3, SDRAM_SIZ_COMMON_3},
	{SDRAM_ADR_COMMON_4, SDRAM_SIZ_COMMON_4},
	{SDRAM_ADR_COMMON_5, SDRAM_SIZ_COMMON_5},
	{SDRAM_ADR_COMMON_6, SDRAM_SIZ_COMMON_6},
	{SDRAM_ADR_COMMON_7, SDRAM_SIZ_COMMON_7},
	{SDRAM_ADR_COMMON_8, SDRAM_SIZ_COMMON_8},
	{SDRAM_ADR_COMMON_9, SDRAM_SIZ_COMMON_9},
	{SDRAM_ADR_COMMON_10, SDRAM_SIZ_COMMON_10},
	{SDRAM_ADR_COMMON_11, SDRAM_SIZ_COMMON_11},
	{SDRAM_ADR_COMMON_12, SDRAM_SIZ_COMMON_12},
	{SDRAM_ADR_COMMON_13, SDRAM_SIZ_COMMON_13},
	{SDRAM_ADR_COMMON_14, SDRAM_SIZ_COMMON_14},
	{SDRAM_ADR_COMMON_15, SDRAM_SIZ_COMMON_15},
	{SDRAM_ADR_COMMON_16, SDRAM_SIZ_COMMON_16},
	{SDRAM_ADR_COMMON_17, SDRAM_SIZ_COMMON_17},
	{SDRAM_ADR_COMMON_18, SDRAM_SIZ_COMMON_18},
	{SDRAM_ADR_COMMON_19, SDRAM_SIZ_COMMON_19},
	{SDRAM_ADR_COMMON_20, SDRAM_SIZ_COMMON_20},
	{SDRAM_ADR_COMMON_21, SDRAM_SIZ_COMMON_21},
	{SDRAM_ADR_COMMON_22, SDRAM_SIZ_COMMON_22},
	{SDRAM_ADR_COMMON_23, SDRAM_SIZ_COMMON_23},
	{SDRAM_ADR_COMMON_24, SDRAM_SIZ_COMMON_24},
	{SDRAM_ADR_COMMON_25, SDRAM_SIZ_COMMON_25},
	{SDRAM_ADR_COMMON_26, SDRAM_SIZ_COMMON_26},
	{SDRAM_ADR_COMMON_27, SDRAM_SIZ_COMMON_27},
	{SDRAM_ADR_COMMON_28, SDRAM_SIZ_COMMON_28},
	{SDRAM_ADR_COMMON_29, SDRAM_SIZ_COMMON_29},
	{SDRAM_ADR_COMMON_30, SDRAM_SIZ_COMMON_30},
	{0xFFFFFFFF, 0xFFFFFFFF}		// end mark
};

static BOOL sdram_map_is_shared(const T_SDRAM_SHARE_INFO *tbl, UINT32 adr);
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
static BOOL sdram_map_is_shared(const T_SDRAM_SHARE_INFO *tbl, UINT32 adr)
{
	int idx;

	idx=0;
	while(0xFFFFFFFF != tbl[idx].size) {
		if( (tbl[idx].adrs <= adr)
		  && (adr < (tbl[idx].adrs + tbl[idx].size)) ) {
			return TRUE;
		}
		idx++;
	}
	return FALSE;
}

BOOL Sdram_Map_Is_Shared(UINT32 adr)
{
	return sdram_map_is_shared( gSdram_Common_ShareInfoTbl, adr );
}
