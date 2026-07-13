/**
 * @file		ct_im_iip_blend.h
 * @brief		This is ct header for im_iip_blend.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CT_IM_IIP_BLEND_H_
#define _CT_IM_IIP_BLEND_H_


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


// BLEND Unit
extern INT32 CT_Im_IIP_6_1_1( VOID );
extern INT32 CT_Im_IIP_6_2_1( VOID );
extern INT32 CT_Im_IIP_Run_6_1( const UINT32 ct_no_3rd );
extern INT32 CT_Im_IIP_Run_6_2( const UINT32 ct_no_3rd );


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _CT_IM_IIP_BLEND_H_

