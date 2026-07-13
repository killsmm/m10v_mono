/**
 * @file		ip_warp_tbl.h
 * @brief		Warp/DZoom's APIs wrapper layer: Wtable manager.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef _IP_WARP_TBL_H_
#define _IP_WARP_TBL_H_

#include "ip_warp.h"
#include "ip_warp_custom.h"
#include "fj_still.h"
#include "ahip.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_IP_WARP_TBL_PITCH_ERR	(0xFF)

#define D_IP_WARP_TBL_DIVIDED_PAGENUM  (2)

#define D_IP_WARP_TBL_DIVIDED_LEFT    (0)
#define D_IP_WARP_TBL_DIVIDED_RIGHT   (1)
#define D_IP_WARP_TBL_DIVIDED_NUM     (2)


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

// U_IP_WARP_CELL
typedef union {
	FJ_GE_CELL		fj_ge_cell;
	AHIP_GDC_LNODE	ahip_gdc_node;
} U_IP_WARP_CELL;

// T_IP_WARP_TBL_INFO
typedef struct {
	BOOL				is_loaded;
	BOOL				mirror_enable;
	E_IP_WARP_TBLIDX	tbl_idx;
} T_IP_WARP_TBL_INFO;

// T_IP_WARP_GRID_INFO_DIVIDED
typedef struct {
	AHIP_GDC_LGRID				ahip_ge_tbl_info;
	U_IP_WARP_CELL				gdc_node[D_IP_WARP_NODE_NUM];
	UINT32						out_img_width;
} T_IP_WARP_GRID_INFO_DIVIDED;

// T_IP_WARP_TBL_DIVIDED
typedef struct {
	BOOL                         allocated;
	T_IP_WARP_GRID_INFO_DIVIDED  grid_info[D_IP_WARP_TBL_DIVIDED_NUM];
} T_IP_WARP_TBL_DIVIDED;

// T_IP_WARP_GRID_INFO
typedef struct {
	BOOL						valid;
	UINT32						idx;
	const T_IP_WARP_TBL_INFO*	tbl_info;
	AHIP_GDC_LGRID				ahip_ge_tbl_info;
	U_IP_WARP_CELL				gdc_node[D_IP_WARP_NODE_NUM];
	FJ_WARP_CALC_PARAM			calc_param;
	FJ_GE_TBL_INFO				fj_ge_tbl_info;
	T_IP_WARP_TBL_DIVIDED*		tbl_divided;
} T_IP_WARP_GRID_INFO;

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


extern UINT8 IP_Warp_Tbl_Conv_Logarithm( const UINT32 num );
extern UINT32 IP_Warp_Tbl_Conv_Power( const UINT8 num );
extern T_IP_WARP_GRID_INFO* IP_Warp_Tbl_Alloc( const UCHAR demosaic_id );
extern VOID IP_Warp_Tbl_Free( const T_IP_WARP_GRID_INFO* grid_info, const UCHAR demosaic_id );
extern VOID IP_Warp_Tbl_Dump_LgridTable( const AHIP_GDC_LGRID* const ge_tbl_info );
extern VOID IP_Warp_Tbl_Dump2_LgridTable( const AHIP_GDC_LGRID* const ge_tbl_info );


#endif	// _IP_WARP_TBL_H_

