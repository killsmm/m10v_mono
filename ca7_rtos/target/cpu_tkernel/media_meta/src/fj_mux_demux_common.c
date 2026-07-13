/**
 * @file		fj_mux_demux_common.c
 * @brief		Mux/Demux common API.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_mux_demux_common.h"
#include "mp4_common.h"

/******************************************************************************
 Program
******************************************************************************/
VOID FJ_Mux_Demux_Common_Memory_Initialize( T_FJ_MUX_DEMUX_COMMON_MEMORY_CONFIG* mem_config )
{
	fj_mux_demux_common_memory_initialize( mem_config );
	return;
}

VOID FJ_Mux_Demux_Common_Memory_Finalize( VOID )
{
	fj_mux_demux_common_memory_finalize();
	return;
}

