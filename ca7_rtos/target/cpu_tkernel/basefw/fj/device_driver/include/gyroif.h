/**
 * @file	  : gyroif.h
 * @brief	  : Gyro I/F Controller
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2016 Socionext Inc.</I></B>\n
 */

#ifndef _GYROIF_H
#define _GYROIF_H

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_peripheral.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

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
extern FJ_ERR_CODE fj_gyroif_start_read( FJ_GYROIF_READ_CTRL const* const gyro_ctrl );
extern FJ_ERR_CODE fj_gyroif_stop_read( VOID );
extern FJ_ERR_CODE fj_gyroif_get_read_count( UINT32* const read_count );

#endif

