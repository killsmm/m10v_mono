/**
 * @file		still_mov_ctrl.h
 * @brief		Movie codec control functions
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _STILL_MOV_CTRL_H_
#define _STILL_MOV_CTRL_H_

// FJ header
#include "fj_still.h"
#include "fj_mode.h"

// Driver Header
#include "ddim_typedef.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// nothing special.


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

extern VOID BF_StillMovCtrl_Start( const UCHAR instance_id, const BOOL current_is_view );



#ifdef __cplusplus
}
#endif

#endif	// _STILL_MOV_CTRL_H_

