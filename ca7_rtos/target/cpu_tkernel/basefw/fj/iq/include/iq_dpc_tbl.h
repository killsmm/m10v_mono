/**
 * @file		iq_dpc_tbl.h
 * @brief		Badpix data table management
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_TBL_H__
#define __IQ_DPC_TBL_H__

#include "fj_iq.h"

#include "fj_std.h"

#include "iq_dpc_custom.h"

#include "iq_common.h"

#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

/* for T_BF_IQ_DPC_TBL_REL.ui_format_id */
#define D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_REL       (0)
#define D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_ABS       (1)
#define D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_REL   (2)
#define D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_ABS   (3)
#define D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA            (2)		// 1x



/* for T_BF_IQ_DPC_TBL_REL.ui_data_type_id */
#define D_BF_IQ_DPC_TBL_DTYPE_DEFECT_SINGLE   (0)
#define D_BF_IQ_DPC_TBL_DTYPE_DEFECT_BEFORE   (1)
#define D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER    (2)
#define D_BF_IQ_DPC_TBL_DTYPE_DEFECT_CENTER   (3)

#define D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE        (0)
#define D_BF_IQ_DPC_TBL_DTYPE_END_OF_DATA     (3)



#define D_BF_IQ_DPC_TBL_REL_ADDR_MAX          (4095)
#define D_BF_IQ_DPC_TBL_ABS_ADDR_MAX          (65534)



#define D_BF_IQ_DPC_TBL_SIZEOF_DEFECT_INFO_DATA		(sizeof(U_BF_IQ_DPC_TBL_DATA))

#define D_BF_IQ_DPC_TBL_IDX_ERR					(65535)



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

// T_BF_IQ_DPC_TBL_REL
typedef struct {
	USHORT	ui_addr:12;				/**< defect Pixel address(relative) */
	USHORT	ui_data_type_id:2;		/**< Data Type ID */
	USHORT	ui_format_id:2;			/**< Format ID	*/
} T_BF_IQ_DPC_TBL_REL;

// T_BF_IQ_DPC_TBL_ABS
typedef struct {
	USHORT	ui_addr;				/**< defect Pixel address(absolute) */
} T_BF_IQ_DPC_TBL_ABS;

// U_BF_IQ_DPC_TBL_DATA
typedef union {
	T_BF_IQ_DPC_TBL_REL		rel;
	T_BF_IQ_DPC_TBL_ABS		abs;
} U_BF_IQ_DPC_TBL_DATA;

// T_BF_IQ_DPC_TBL_INFO
typedef struct {
	U_BF_IQ_DPC_TBL_DATA*	top_addr;
	ULLONG					maxbytelen;
	U_BF_IQ_DPC_TBL_DATA*	addr_by_fields[D_BF_IQ_DPC_MAX_FIELDS];
	ULLONG					bytelen_by_fields[D_BF_IQ_DPC_MAX_FIELDS];
	ULLONG					bytelen;
	UINT32					pixel_num;
	USHORT					sta_x;			// RAW start position of X.
	USHORT					sta_y;			// RAW start position of X.
	USHORT					width;			// RAW width. (not detetion window.)
	USHORT					lines;			// RAW lines. (not detetion window.)
	USHORT					field_num;
	BOOL					valid;
} T_BF_IQ_DPC_TBL_INFO;



/*==============================================================================*/
/* Function declaration															*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

extern FJ_ERR_CODE BF_IQ_Dpc_Tbl_Init( VOID );
extern T_BF_IQ_DPC_TBL_INFO* BF_IQ_Dpc_Tbl_Get_Info( const E_IQ_CALIB_TGT e_calib_tgt, const UINT32 tbl_idx );
extern VOID BF_IQ_Dpc_Tbl_Stat( VOID );
extern VOID BF_IQ_Dpc_Tbl_Print_SizeDbg( VOID );
extern VOID BF_IQ_Dpc_Tbl_Dump( const E_IQ_CALIB_TGT e_calib_tgt, const UINT32 tbl_idx );


#ifdef __cplusplus
}
#endif

#endif /* __IQ_DPC_TBL_H__ */

