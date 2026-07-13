/**
 * @file		ip_warp.h
 * @brief		Header file of Warp's wrapper layer
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef _IP_WARP_H
#define	_IP_WARP_H

#include "fj_still.h"
#include "ahip.h"
#include "ddim_typedef.h"


///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup ip_warp_overview
@{
	This chapter describes API related to Warp's wrapper layer.
@}*//* --- end of ip_warp_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup ip_warp_sequence
@{
	Please reference "Still Sequence(Video Loop)".
@}*//* --- end of ip_warp_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup ip_warp_definition
@{*/
/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

/** Warping table type number. */
typedef enum {
	E_IP_WARP_TBLIDX_WARPING = 0,	/**< Warping table type number. "Warping" */
	E_IP_WARP_TBLIDX_LDC0,			/**< Warping table type number. "Lens distortion correction" */
	E_IP_WARP_TBLIDX_LDC1,			/**< Warping table type number. "Lens distortion correction" */
	E_IP_WARP_TBLIDX_LDC2,			/**< Warping table type number. "Lens distortion correction" */
	E_IP_WARP_TBLIDX_LDC3,			/**< Warping table type number. "Lens distortion correction" */
	E_IP_WARP_TBLIDX_DZOOM_MIRROR,	/**< Warping table type number. "Digital zoom and/or Mirror only.(exclude Warping and LDC.)" */
	E_IP_WARP_TBLIDX_MAX,			/**< Maximum value of Warping table type number. */
} E_IP_WARP_TBLIDX;


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

#define D_IP_WARP_ERR_OK		(0)		/**< Successful. */
#define D_IP_WARP_ERR_NG		(-1)	/**< Undefined error. */
#define D_IP_WARP_ERR_NG_FORMAT	(-2)	/**< Format error. */
#define D_IP_WARP_ERR_NG_TABLE	(-3)	/**< Table error. */
#define D_IP_WARP_ERR_OVERFLOW	(-4)	/**< Overflow error. */
#define D_IP_WARP_ERR_NOTFOUND	(-5)	/**< Not found error. */
#define D_IP_WARP_ERR_TBL_ALLOC	(-6)	/**< Table allocate error. */
#define D_IP_WARP_ERR_PARAM_ERR	(-7)	/**< Parameter error. */

#define D_IP_WARP_DIVIDED_ID_LEFT     (0)
#define D_IP_WARP_DIVIDED_ID_RIGHT    (1)
#define D_IP_WARP_DIVIDED_ID_NUM      (2)
#define D_IP_WARP_DIVIDED_ID_NOTDIV   (0xFF)

/*==============================================================================*/
/* Global variable declaration													*/
/*==============================================================================*/
// nothing special.

/*@} --- end of ip_warp_definition */

/*==============================================================================*/
/* Function declaration															*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

/** @weakgroup ip_warp_api
@{*/

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function loads the warping table binary to internal table memory area.
 *
 *	@param[in]	tbl_idx	: Warping table type number.
 *	@param[in]	addr	: Pointer of Warping table binary data.
 *	@param[in]	bytes	: Size of Warping table binary data.
 *	@retval		D_IP_WARP_ERR_OK		Load successful.
 *	@retval		D_IP_WARP_ERR_NG_FORMAT	Binary data format is unknown.
 *	@retval		D_IP_WARP_ERR_OVERFLOW	Binary data size is learge.
 *	@retval		D_IP_WARP_ERR_NG_TABLE	BaseFW internal table memory area is full.
 *	@par Sample code:
 *				Please refer the sample code of IP_Warp_Calc_WarpingTable().
 */
extern INT32 IP_Warp_Notify_LoadingComplete( const E_IP_WARP_TBLIDX tbl_idx, const BYTE* addr, ULONG bytes );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function calculate SGDE table from warping table binary.
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@param[in]	tbl_idx	: Warping table type number.
 *	@param[in]	calc_param	: Rectangle parameter.
 *	@retval		D_IP_WARP_ERR_OK			Calculated successful.
 *	@retval		D_IP_WARP_ERR_OVERFLOW		SGDE table memory is small.
 *	@retval		D_IP_WARP_ERR_NOTFOUND		Not loaded Warping table.
 *	@par Sample code:
 *	@code
#include "ip_wrap.h"

VOID calc_sgde_table( VOID )
{
	INT32 ip_ercd;
	BYTE* addr;
	ULONG bytes;
	FJ_WARP_CALC_PARAM calc_param = {
		.crop_sta_x = 0,
		.crop_sta_y = 0,
		.crop_width = 1440,
		.crop_lines = 1080,
		.in_img_width = 1440,
		.in_img_lines = 1080,
		.out_img_width = 1920,
		.out_img_lines = 1080,
	};
	UINT32 demosaic_id = 0;

	user_func_warping_tbl_load_to_sdram( &addr, &bytes );

	ip_ercd = IP_Warp_Notify_LoadingComplete( E_IP_WARP_TBLIDX_WARPING, addr, bytes );
	if( ip_ercd != D_IP_WARP_ERR_OK ) {
		// Error correction code here.
		return;
	}

	ip_ercd = IP_Warp_Calc_Table( demosaic_id E_IP_WARP_TBLIDX_WARPING, &calc_param );
	if( ip_ercd != D_IP_WARP_ERR_OK ) {
		// Error correction code here.
		return;
	}
}
 *	@endcode
 */
extern INT32 IP_Warp_Calc_Table( const UCHAR demosaic_id, const E_IP_WARP_TBLIDX tbl_idx, const FJ_WARP_CALC_PARAM* calc_param, const BOOL mirror_enable );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function Disable current SGDE table.
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@retval		D_IP_WARP_ERR_OK		Calculated table found and return table.
 */
extern INT32 IP_Warp_Disable_Table( const UCHAR demosaic_id );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function Disable current SGDE table.
 *
 *	@retval		D_IP_WARP_ERR_OK		Calculated table found and return table.
 */
extern INT32 IP_Warp_Disable_Table_All( VOID );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function get current SGDE table.
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@param[out]	ge_tbl_info	: Calculated SGDE table.
 *	@retval		D_IP_WARP_ERR_OK		Calculated table found and return table.
 *	@retval		D_IP_WARP_ERR_NOTFOUND	Calculated table not found.
 */
extern INT32 IP_Warp_Get_Table( const UCHAR demosaic_id, AHIP_GDC_LGRID* ge_tbl_info );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function control IP_Warp_Get_Table()'s return code. for Debug.
 *
 *	@param[in]	onoff	: FALSE: normal operation. TRUE: IP_Warp_Get_Table() return @ref D_IP_WARP_ERR_NOTFOUND always.
 */
extern VOID IP_Warp_Disable_GetTable( const BOOL onoff );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function returns availability of warp table. for Debug.
 *
 *	@retval		TRUE		Warp table can be used.
 *	@retval		FALSE		Warp table cannot be used.
 */
extern BOOL IP_Warp_Table_Available( VOID );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function gets current SGDE table. When you want to get the part of the divided table, you must use this function instead of IP_Warp_Get_Table.
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@param[out]	ge_tbl_info	: Calculated SGDE table.
 *	@param[in]	divided_id	: Identifier to specify divided table. 0: Left, 1: Right.
 *	@retval		D_IP_WARP_ERR_OK		Calculated table found and return table.
 *	@retval		D_IP_WARP_ERR_NOTFOUND	Calculated table not found.
 */
extern INT32 IP_Warp_Get_Table_Divided( const UCHAR demosaic_id, AHIP_GDC_LGRID* const ge_tbl_info, const UCHAR divided_id );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function gets information on whether Local Grid table division is necessary.
 *
 *	@param[in]	demosaic_id	: Demosaic instance ID.
 *	@retval		TRUE		The table needs to divide.
 *	@retval		FALSE		The table does not need to divide.
 */
extern BOOL IP_Warp_Need_To_Divide( const UCHAR demosaic_id );

/**				<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *				This function gets the horizontal width of the image output when dewarping is performed using the divided local grid table.
 *
 *	@param[in]	demosaic_id		: 	Demosaic instance ID.
 *	@param[out]	div_img_width	:	The pointer to the area to set the horizontal width.
 *	@param[in]	divided_id		:	Identifier to specify divided table. 0: Left, 1: Right.
 *	@retval		D_IP_WARP_ERR_OK		Calculated table found and return table.
 *	@retval		D_IP_WARP_ERR_NOTFOUND	Calculated table not found.
 */
extern INT32 IP_Warp_Get_Divided_Img_Width( const UCHAR demosaic_id, UINT32* const div_img_width, const UCHAR divided_id );


/*@} --- end of ip_warp_api */

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _IP_WARP_H
