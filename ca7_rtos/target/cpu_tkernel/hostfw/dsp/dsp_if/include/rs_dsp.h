/**
 *
 * @file		rs_dsp.h
 * @brief		DSP I/F
 * @note		
 * @attention	
 * @Copyright	Copyright 2015 Socionext Inc.
 *
 */


#ifndef _RS_DSP_H_
#define _RS_DSP_H_

#include "ddim_typedef.h"							// typedef

#include "../dsp_peri/include/dsp_peri_ipcu.h"	// DSP Peripheral IPCU Driver

#include "procID_CPU.h"							// Processing ID for CPU
#include "procID_DSP.h"							// Processing ID for DSP



/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_RS_DSP_OK					(0)
#define D_RS_DSP_NG_ABORT			(-1)
#define D_RS_DSP_NG_OS				(-2)
#define D_RS_DSP_NG_TIMEOUT			(-3)
#define D_RS_DSP_NG_BUSY			(-4)
#define D_RS_DSP_NG_UNKNOWN			(-5)

#define D_RS_DSP_NUM_DATA			D_IPCU_NUM_DATA_REG

#define D_RS_DSP_CLK_400MHz_DIV2	(0)
#define D_RS_DSP_CLK_266MHz_DIV2	(1)
#define D_RS_DSP_CLK_200MHz_DIV2	(2)
#define D_RS_DSP_CLK_100MHz_DIV2	(3)
#define D_RS_DSP_CLK_200MHz_DIV1	(4)
#define D_RS_DSP_CLK_100MHz_DIV1	(5)


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

typedef struct {
	UINT32	data[D_RS_DSP_NUM_DATA];
} T_RS_DSP_MSG;


/*----------------------------------------------------------------------*/
/* External Function													*/
/*----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus


extern INT32 RS_DSP_Start( UINT32	dspBootAddr,
						   INT32 	dspClk );

extern INT32 RS_DSP_Stop( VOID );

extern INT32 RS_DSP_SendMsg( UINT16			*sndID,
							 UINT16 		procID,
							 INT32			waitTime,
							 T_RS_DSP_MSG	*sndMsg );

extern INT32 RS_DSP_WaitMsg( UINT16			sndID,
						     INT32 			waitTime,
						     T_RS_DSP_MSG	*rcvMsg );


#ifdef __cplusplus
}
#endif	// __cplusplus





#endif	// _RS_DSP_H_
