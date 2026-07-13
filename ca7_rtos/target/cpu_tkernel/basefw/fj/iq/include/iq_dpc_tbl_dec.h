/**
 * @file		iq_dpc_tbl_dec.h
 * @brief		Badpix table decorder.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_TBL_DEC_H__
#define __IQ_DPC_TBL_DEC_H__

#include "iq_dpc_tbl.h"

#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

// T_BF_IQ_DPC_TBL_DEC_SUCCESSION_INFO
typedef struct {
	USHORT									x_pos_last;
	USHORT									y_pos_last;
	UINT32									succession_count;
} T_BF_IQ_DPC_TBL_DEC_SUCCESSION_INFO;

// T_BF_IQ_DPC_TBL_DEC_PARAM
typedef struct {
	// Dpc table data
	U_BF_IQ_DPC_TBL_DATA*					tbl_addr_top;
	U_BF_IQ_DPC_TBL_DATA*					tbl_addr;
	ULLONG									bytelen;
	ULLONG									maxbytelen;
	USHORT									pix_num;
	// Dec work
	USHORT									x_pos_last;
	USHORT									y_pos_last;
	U_BF_IQ_DPC_TBL_DATA*					tbl_addr_before[2];		// X axis Even and odd
	T_BF_IQ_DPC_TBL_DEC_SUCCESSION_INFO	succession_info[2];		// X axis Even and odd.
} T_BF_IQ_DPC_TBL_DEC_PARAM;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Function																		*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


extern FJ_ERR_CODE BF_IQ_Dpc_Tbl_Dec_Add_Pixel( T_BF_IQ_DPC_TBL_DEC_PARAM* const param, const USHORT x_pos, const USHORT y_pos );
extern ULLONG BF_IQ_Dpc_Tbl_Dec_Get_BytelenAlign( T_BF_IQ_DPC_TBL_DEC_PARAM* const param );
extern ULLONG BF_IQ_Dpc_Tbl_Dec_Get_BytelenHardwareAlign( T_BF_IQ_DPC_TBL_DEC_PARAM* const param );
extern ULLONG BF_IQ_Dpc_Tbl_Dec_Get_Bytelen( T_BF_IQ_DPC_TBL_DEC_PARAM* const param );
extern UINT32 BF_IQ_Dpc_Tbl_Dec_Get_Pixnum( T_BF_IQ_DPC_TBL_DEC_PARAM* const param );
extern U_BF_IQ_DPC_TBL_DATA* BF_IQ_Dpc_Tbl_Dec_Get_TblAddr( T_BF_IQ_DPC_TBL_DEC_PARAM* const param );
extern FJ_ERR_CODE BF_IQ_Dpc_Tbl_Dec_Init( T_BF_IQ_DPC_TBL_DEC_PARAM* const param, U_BF_IQ_DPC_TBL_DATA* const addr, const ULLONG maxbytelen );
extern FJ_ERR_CODE BF_IQ_Dpc_Tbl_Dec_Finish( T_BF_IQ_DPC_TBL_DEC_PARAM* const param );


#ifdef __cplusplus
}
#endif

#endif // __IQ_DPC_TBL_DEC_H__

