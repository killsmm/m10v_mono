/**
 * @file		iq_dpc_file.h
 * @brief		Badpix data table file load/save API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_FILE_H__
#define __IQ_DPC_FILE_H__

#include "fj_iq.h"

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
// nothing special.


/*==============================================================================*/
/* Function declaration															*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

extern BOOL fj_iq_calibrationsavebadpixtable( const E_IQ_CALIB_TGT target, const UINT32 tbl_idx, const CHAR* const to_file_path );
extern BOOL fj_iq_calibrationloadbadpixtable( const E_IQ_CALIB_TGT target, const UINT32 tbl_idx, const CHAR* const from_file_path );
extern BOOL fj_iq_calibrationdeletebadpixtable( const E_IQ_CALIB_TGT target, const UINT32 tbl_idx, const CHAR* const from_file_path );
extern BOOL fj_iq_notify_loading_badpixtable_complete( const E_IQ_CALIB_TGT target, const UINT32 tbl_idx, const BYTE* const addr, const ULONG bytes );
extern FJ_ERR_CODE fj_iq_get_badpixtableaddr( const E_IQ_CALIB_TGT target, const UINT32 tbl_idx, BYTE** const addr, UINT32* const bytes );
extern BOOL fj_iq_calibrationsavebadpixtable_sdram( const E_IQ_CALIB_TGT e_calib_tgt, CHAR* tbl_addr, ULONG* tbl_size );
extern BOOL fj_iq_calibrationloadbadpixtable_sdram( const E_IQ_CALIB_TGT e_calib_tgt, const UINT32 tbl_idx, CHAR* tbl_addr );

#ifdef __cplusplus
}
#endif

#endif /* __IQ_DPC_FILE_H__ */

