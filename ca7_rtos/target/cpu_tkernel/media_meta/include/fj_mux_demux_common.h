/**
 * @file		fj_mux_demux_common.h
 * @brief		Mux/Demux Common Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_MUX_DEMUX_COMMON_H_
#define _FJ_MUX_DEMUX_COMMON_H_

#include "fj_mux.h"
#include "fj_demux.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @addtogroup fj_mux_demux_common_overview
@{
	The MUX/DEMUX common controls can be achieved by using this API.<br>
@}*//* --- end of fj_mux_demux_common_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @addtogroup fj_mux_demux_common_sequence
@{
	Please refer BaseFWPF Specification.
@}*//* --- end of fj_mux_demux_common_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_mux_demux_common_definition
@{*/

/* Mux Only */
#define FJ_MUX_DEMUX_COMMON_FS_BANK_MAX_NUM				(4)


///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////
/* --- Mux/Demux Memeory Parameter --- */
typedef struct {
	UINT32						header_mem_addr;			/**< SDRAM Address of MP4 Header. */
	UINT32						header_mem_size;			/**< SDRAM Size of MP4 Header. <br>
															     <reference> 1file need 4Mbyte (for 30fps case : 2hour) */
	UINT32						muxing_mem_addr;			/**< SDRAM Address of MP4 Muxing Memory. */
	UINT32						muxing_mem_size;			/**< SDRAM Size of MP4 Muxing Memory. <br>
															     <reference> 1file need 16Mbyte  (for filesystem) */
	UINT32						muxing_file_num;			/**< Muxing File Number. */
} T_FJ_MUX_DEMUX_COMMON_MEMORY_CONFIG;

/*@} --- end of fj_mux_demux_common_definition */

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_mux_demux_common_api
@{*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function muxer/demuxer initialize memory area <br>
@param[in]	mem_config			Mux/Demux controll memory configuration.<br>
*/
extern	VOID	FJ_Mux_Demux_Common_Memory_Initialize( T_FJ_MUX_DEMUX_COMMON_MEMORY_CONFIG* mem_config );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function muxer/demuxer finalize memory area <br>
*/
extern	VOID	FJ_Mux_Demux_Common_Memory_Finalize( VOID );

/*@} --- end of fj_mux_demux_common_api */

#endif	// _FJ_MUX_DEMUX_COMMON_H_
