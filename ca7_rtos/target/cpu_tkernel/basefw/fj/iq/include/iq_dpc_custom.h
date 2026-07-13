/**
 * @file		iq_dpc_custom.h
 * @brief		Badpix Detection/Compensation custom parameter
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_CUSTOM_H__
#define __IQ_DPC_CUSTOM_H__

#include "ddim_typedef.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

#define D_BF_IQ_DPC_MAX_PIXELS	(20000)						// Maximum pixels
#define D_BF_IQ_DPC_MAX_LINES	(D_BF_IQ_DPC_MAX_PIXELS)	// Maximum lines of image sensor
#define D_BF_IQ_DPC_MAX_FIELDS	(2)							// Maximum fields of PRO(odd/even)

#define D_BF_IQ_DPC_TBL_BYTES	(M_IQ_ROUNDUP_8( M_BF_IQ_DPC_CALC_TBL_BYTE(D_BF_IQ_DPC_MAX_PIXELS) ))

#define D_BF_IQ_DPC_TBL_IDX_MAX	(5)

#define D_BF_IQ_DPC_MULTIPIPE_TBL_MAX_NUM	(16)
#define D_BF_IQ_DPC_MULTIPIPE_TBL_X_NUM	(16)
#define D_BF_IQ_DPC_MULTIPIPE_TBL_Y_NUM	(8)


/*==============================================================================*/
/* Macro declaration															*/
/*==============================================================================*/

#define M_BF_IQ_DPC_CALC_TBL_NUM( ui_maxnum )	( (ui_maxnum) + (D_BF_IQ_DPC_MAX_LINES) + 1 )
#define M_BF_IQ_DPC_CALC_TBL_BYTE( ui_maxnum )	( M_BF_IQ_DPC_CALC_TBL_NUM(ui_maxnum) * D_BF_IQ_DPC_TBL_SIZEOF_DEFECT_INFO_DATA )


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Function declaration															*/
/*==============================================================================*/
// nothing special.


#endif /* __IQ_DPC_CUSTOM_H__ */

