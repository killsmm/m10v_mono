/**
 * @file		iq_calib.h
 * @brief		Algorithm of Calibration API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_CALIB_H__
#define __IQ_CALIB_H__

#include "fj_iq.h"

#include "fj_std.h"

#include "iq_dpc_line_order.h"

#include "ddim_typedef.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

typedef enum {
	E_IQ_BAYER_COLOR_R,
	E_IQ_BAYER_COLOR_GR,
	E_IQ_BAYER_COLOR_GB,
	E_IQ_BAYER_COLOR_B,
	E_IQ_BAYER_COLOR_MAX,
} E_IQ_BAYER_COLOR;


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

/** FJ_IQ_CALIB_PARAM<br>
 *	BaseFW internal parameter for Calibration
 */
typedef struct {
	USHORT* pus_bayer16_addr;				/**< bayer top address					*/
	UINT32 ui_bayer_bytelen;				/**< bayer size(byte)					*/
	USHORT us_global_rows;					/**< bayer global rows(height)			*/
	USHORT us_global_columns;				/**< bayer global columns(width)		*/
	USHORT us_field_num;					/**< num of PRO fields					*/
	E_IQ_BAYER_COLOR t_top_color;			/**< color of Left-Top corner			*/
	E_IQ_STILL_MODE still_mode;				/**< current still mode					*/
	USHORT us_vd_ofs;						/**< DPC detect window virtical offset	*/
	USHORT us_hd_ofs;						/**< DPC detect window horizontal offset*/
	UINT32 us_upper_trim;					/**< bayer upper side of detect window	*/
	UINT32 us_left_trim;					/**< bayer left side of detect window	*/
	UINT32 us_rows;							/**< bayer rows(height) of detect window	*/
	UINT32 us_columns;						/**< bayer columns(width) of detect window	*/
	const T_FJ_IQ_DPC_LINE_ORDER* pt_line_order_table;	/**< line separate order table for all fields ... for Badpix Software detection */
} T_BF_IQ_CALIB_PARAM;

// T_BF_IQ_CALIB_INFO_OB
typedef struct {
	SHORT				obdata[E_IQ_BAYER_COLOR_MAX];	// OB value by color.
} T_BF_IQ_CALIB_INFO_OB;

/**	T_BF_IQ_CALIB_INFO
 */
typedef struct {
	const VOID* pv_bayer16_addr;		// bayer data top addr
	UINT32	ui_bayer_bytelen;			// bayer data size(bytes) (for 16bit pack)
	USHORT	us_global_h_pixels;			// bayer global height(pixels)
	USHORT	us_global_w_pixels;			// bayer global width(pixels)
	USHORT	us_vd_ofs;					// virtical offset(pixels)
	USHORT	us_hd_ofs;					// horizontal offset(pixels)
	UINT32	us_upper_trim;				// bayer upper side of detect window
	UINT32	us_left_trim;				// bayer left side of detect window
	E_IQ_BAYER_COLOR	t_top_color;	// color of Left-Top corner
	E_IQ_STILL_MODE still_mode;			// current still mode
	USHORT	us_field_num;				// num of PRO fields
	T_BF_IQ_CALIB_INFO_OB	ob;			// OB information.
} T_BF_IQ_CALIB_INFO;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

// BaseFW internal API

extern FJ_ERR_CODE BF_IQ_Calib_Load_BayerFile( const UCHAR sen_id, const UCHAR* const puc_filepath );
extern FJ_ERR_CODE BF_IQ_Calibration_Set_Param( const UCHAR sen_id, const T_BF_IQ_CALIB_PARAM* const pt_calib_param );
extern FJ_ERR_CODE BF_IQ_Calibration_Set_Ob( const UCHAR sen_id, const T_BF_IQ_CALIB_INFO_OB* const pt_ob );
extern T_BF_IQ_CALIB_INFO_OB* BF_IQ_Calibration_Get_Ob( const UCHAR sen_id );
extern FJ_ERR_CODE BF_IQ_Calib_Set_Bay( const UCHAR sen_id, const VOID* const bay_addr, const UINT32 bay_bytes, const UINT32 width, const UINT32 lines );
extern FJ_ERR_CODE BF_IQ_Calib_Set_BayAddr( const UCHAR sen_id, const VOID* const bay_addr );
extern const T_BF_IQ_CALIB_INFO* BF_IQ_Calib_Get_Param( const UCHAR sen_id );
extern const CHAR* BF_IQ_Calib_Get_CalibTgtName( const E_IQ_CALIB_TGT e_target );





// BaseFW external API substantial

extern VOID fj_iq_calibrationsetformatpix( const E_IQ_CALIB_FMT_PIX e_format_pix );
extern E_IQ_CALIB_FMT_PIX fj_iq_calibrationgetformatpix( VOID );
extern VOID fj_iq_calibrationsetrawsavetomedia( const BOOL save );
extern BOOL fj_iq_calibrationgetrawsavetomedia( VOID );
extern VOID fj_iq_calibrationsetjpegsavetomedia( const BOOL save );
extern BOOL fj_iq_calibrationgetjpegsavetomedia( VOID );
extern VOID fj_iq_calibrationsettarget( const E_IQ_CALIB_TGT target );
extern E_IQ_CALIB_TGT fj_iq_calibrationgettarget( VOID );
extern VOID fj_iq_calibrationsetoperate( const E_IQ_CALIB_OP target );
extern E_IQ_CALIB_OP fj_iq_calibrationgetoperate( VOID );
extern BOOL fj_iq_calibrationgetbayerinfo( VOID** const pv_top_addr, USHORT* const pus_global_height_pixs, USHORT* const pus_global_width_pixs );
extern VOID fj_iq_calib_set_bay( const VOID* const bay_addr, const UINT32 bay_bytes, const UINT32 width, const UINT32 lines );


#ifdef __cplusplus
}
#endif

#endif /* __IQ_CALIB_H__ */

