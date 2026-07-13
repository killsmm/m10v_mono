/**
 * @file		ct_im_iip_afn.h
 * @brief		This is ct header for im_iip_afn.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CT_IM_IIP_AFN_H_
#define _CT_IM_IIP_AFN_H_


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


// AFN Unit
extern INT32 CT_Im_IIP_3_1_1( VOID );
extern INT32 CT_Im_IIP_3_1_2( VOID );
extern INT32 CT_Im_IIP_3_1_3( VOID );
extern INT32 CT_Im_IIP_Run_3_1( const UINT32 ct_no_3rd );

extern INT32 CT_Im_IIP_3_2_1( VOID );
extern INT32 CT_Im_IIP_3_2_2( VOID );
extern INT32 CT_Im_IIP_3_2_3( VOID );
extern INT32 CT_Im_IIP_Run_3_2( const UINT32 ct_no_3rd );


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _CT_IM_IIP_AFN_H_

