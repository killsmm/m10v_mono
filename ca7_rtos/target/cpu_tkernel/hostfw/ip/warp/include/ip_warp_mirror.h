/**
 * @file		ip_warp_mirror.h
 * @brief		Warp/DZoom's APIs wrapper layer: convert mirror table.
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2016</I></B>
 */

#ifndef _IP_WARP_MIRROR_H
#define	_IP_WARP_MIRROR_H

#include "ip_warp_tbl.h"
#include "ddim_typedef.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// nothing special.

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global variable declaration											*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function declaration													*/
/*----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus


extern INT32 IP_Warp_Mirror_Calc_Table( T_IP_WARP_GRID_INFO* const grid_info );


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _IP_WARP_MIRROR_H
