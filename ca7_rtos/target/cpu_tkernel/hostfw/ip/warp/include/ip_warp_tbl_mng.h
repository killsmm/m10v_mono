/**
 * @file		ip_warp_tbl.h
 * @brief		Warp/DZoom's APIs wrapper layer: Wtable manager.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef _IP_WARP_TBL_MNG_H_
#define _IP_WARP_TBL_MNG_H_

#include "ip_warp_tbl.h"
#include "ip_warp_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/


extern VOID IP_Warp_Tbl_Mng_Disable( const UCHAR demosaic_id );
extern VOID IP_Warp_Tbl_Mng_Change_Next( const UCHAR demosaic_id );
extern T_IP_WARP_GRID_INFO* IP_Warp_Tbl_Mng_Get_Next( const UCHAR demosaic_id );
extern T_IP_WARP_GRID_INFO* IP_Warp_Tbl_Mng_Get_Current( const UCHAR demosaic_id );


#endif	// _IP_WARP_TBL_MNG_H_

