/**
 * @file		iq_dpc_detect_info.h
 * @brief		Badpix software/hardware detection parameter management
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_DETECT_INFO_H__
#define __IQ_DPC_DETECT_INFO_H__

#include "fj_iq.h"
#include "fj_std.h"

#include "iq_dpc_custom.h"
#include "iq_dpc_line_order.h"
#include "iq_dpc_tbl.h"

#include "iq_calib.h"

#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Macro declaration															*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

// T_BF_IQ_DPC_DETECT_INFO - Dead Pixel Compensation parameters structure
typedef struct {
	USHORT	us_threshold_high;			/**< [in] high threshold */
	USHORT	us_threshold_low;			/**< [in] low threshold */
	const USHORT* pus_bayer16_addr;			/**< [in] white capture bayer data(16bit unpacked) */
	UINT32	ui_bayer_byte;				/**< [in] bayer data size(bytes) */
	UINT32	us_global_w_pixels;			/**< [in] capture bayer global width(pixels) */
	UINT32	us_global_h_pixels;			/**< [in] capture bayer global height(pixels) */
	USHORT	us_hd_ofs;					/**< [in] DPC detect window horizontal offset*/
	USHORT	us_vd_ofs;					/**< [in] DPC detect window virtical offset	*/
	UCHAR	uc_trans_unit;				/**< [in] DPC transfer unit size */
	UCHAR	uc_hd_edge;					/**< [in] DPC hd edge */
	UCHAR	uc_vd_edge;					/**< [in] DPC vd edge */
	UINT32	us_left_trim;				/**< [in] bayer left side of detect window	*/
	UINT32	us_upper_trim;				/**< [in] bayer upper side of detect window	*/
	UINT32	us_w_pixels;				/**< [in] capture bayer width(pixels) */
	UINT32	us_h_pixels;				/**< [in] capture bayer height(pixels) */
	UINT32	ui_field_num;				/**< [in] num of PRO fields */
	E_IQ_DETECT_ALGORITHM e_detect_mode;/**< [in] detection algorithm */
	const FJ_IQ_CORNER_THRESHOLD* pt_corner_thresh;/**< [in] corner threthold information */
	const T_FJ_IQ_DPC_LINE_ORDER* pt_line_order_table;	/**< [in] line separate order table for all PRO fields */
	ULLONG	ull_trans_limit_byte;		/**< [in] table byte size in PRO 1field (when hardware detect) */
	BOOL	b_hwDetectOnOff;			/**< [in] hardware(PRO) detect request */
	UINT32	ui_max_badpix_thresh;		/**< [in] Dead pixel detection max count */
	U_BF_IQ_DPC_TBL_DATA* pt_defect_info_tbl_addr;	/**< [out] pickuped defect information table addr */
	ULLONG	ull_defect_info_tbl_byte;	/**< [inout] defect information table size(bytes) */
	ULLONG	ull_defect_info_tbl_bytelen[D_BF_IQ_DPC_MAX_FIELDS];/**< [out] defect information table byte len. (by field) */
	UINT32	ui_detect_info_tbl_overflow;/**< [out] ui_defect_info_tbl is overflow */
	UINT32	ui_detect_count;			/**< [out] detected defect pixels counter */
	UINT32	ui_over4succession_count;	/**< [out] over 4 succession defect pixels count */
} T_BF_IQ_DPC_DETECT_INFO;


/*==============================================================================*/
/* Function declaration															*/
/*==============================================================================*/

extern VOID BF_IQ_Dpc_Detect_Info_Set_CalibParam( const UCHAR sen_id, const T_BF_IQ_CALIB_PARAM* const pt_calib_param );
extern VOID BF_IQ_Dpc_Detect_Info_Set_Bay( const UCHAR sen_id, const VOID* const bay_addr, const UINT32 bay_bytes, const UINT32 width, const UINT32 lines );
extern VOID BF_IQ_Dpc_Detect_Info_Set_BayAddr( const UCHAR sen_id, const VOID* const bay_addr );
extern T_BF_IQ_DPC_DETECT_INFO* BF_IQ_Dpc_Detect_Info_Get_Info( const UCHAR sen_id );


#endif /* __IQ_DPC_DETECT_INFO_H__ */

