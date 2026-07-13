/**
 * @file		ct_im_iip_flt.h
 * @brief		This is ct header for im_iip_flt.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CT_IM_IIP_FLT_H_
#define _CT_IM_IIP_FLT_H_


#include "ddim_typedef.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */


/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus


// FLT Unit
extern INT32 CT_Im_IIP_9_1_1( VOID );
extern INT32 CT_Im_IIP_9_1_2( VOID );
extern INT32 CT_Im_IIP_9_2_1( VOID );
extern INT32 CT_Im_IIP_9_3_1( VOID );
extern INT32 CT_Im_IIP_Run_9_1( const UINT32 ct_no_3rd );
extern INT32 CT_Im_IIP_Run_9_2( const UINT32 ct_no_3rd );
extern INT32 CT_Im_IIP_Run_9_3( const UINT32 ct_no_3rd );


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _CT_IM_IIP_FLT_H_

