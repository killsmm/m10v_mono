/**
 * @file		ct_im_iip_cfl.h
 * @brief		This is ct header for im_iip_cfl.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CT_IM_IIP_CFL_H_
#define _CT_IM_IIP_CFL_H_


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


// CFL Unit
extern INT32 CT_Im_IIP_12_1_1( VOID );
extern INT32 CT_Im_IIP_12_1_2( VOID );
extern INT32 CT_Im_IIP_Run_12_1( const UINT32 ct_no_3rd );


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _CT_IM_IIP_CFL_H_

