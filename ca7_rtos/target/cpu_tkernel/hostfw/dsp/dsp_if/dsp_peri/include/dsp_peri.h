/**
 *
 * @file		dsp_peri.h
 * @brief		Driver [ DSP Peripheral except for IPCU ]
 * @author		H.Maeda (SNI)
 * @since		2014/05/12
 * @note		
 * @attention	
 * @Copyright	Copyright (C) 2015 SOCIONEXT Inc. All Rights Reserved.
 *
 */

#ifndef _DSP_PERI_H_
#define _DSP_PERI_H_


#include "dsp_peri_ipcu.h"


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_DSPPERI_OK				(0)
#define D_DSPPERI_NG_BUSY			(-1)

#define D_DSPPERI_OFF				(0)
#define D_DSPPERI_ON				(1)

#define D_DSPPERI_DSPCLK_DIV_2		(0)
#define D_DSPPERI_DSPCLK_DIV_1		(1)

#define D_DSPPERI_MODE_STANDBY		(0)
#define D_DSPPERI_MODE_LIGHT_SLEEP	(1)

#define D_DSPPERI_INT4SEL_IPU2DSP	(0)
#define D_DSPPERI_INT4SEL_GVI		(1)


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define M_DSPPERI_BITMSK_IDLE_DSP(status)		( ( status ) & ( 1 << 2 ) )
#define M_DSPPERI_BITMSK_IDLE_CORE(status)		( ( status ) & ( 1 << 3 ) )

#define M_DSPPERI_BITMSK_NMI(status0)			( ( status0 ) & ( 1 <<  0 ) )
#define M_DSPPERI_BITMSK_VINT(status0)			( ( status0 ) & ( 1 << 16 ) )

#define M_DSPPERI_BITMSK_VDMA(status1)			( ( status1 ) & ( 1 << 0 ) )
#define M_DSPPERI_BITMSK_PDMA(status1)			( ( status1 ) & ( 1 << 4 ) )
#define M_DSPPERI_BITMSK_GVIO(status1)			( ( status1 ) & ( 1 << 8 ) )


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* External Function													*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus


extern void DspPeri_Reset( int flag );

extern void DspPeri_Ctrl_SramPower( int flag );

extern void DspPeri_Ctrl_AxiClk( int clkDiv );

extern void DspPeri_Ctrl_VINT_ADDR( unsigned int addr );

extern void DspPeri_Ctrl_INT4SEL( unsigned int int4sel );

extern int 	DspPeri_Request_NMI( void );

extern int 	DspPeri_Request_VINT( void );

extern void DspPeri_Get_IntStatus( unsigned int *status0,
								   unsigned int *status1 );

extern void DspPeri_Ctrl_LowPower( int mode );

extern int 	DspPeri_Ctrl_Recovery( int flag );

extern void DspPeri_Get_PowerStatus( unsigned int *status );

extern void DspPeri_Ctrl_ExtWait( int flag );


#ifdef __cplusplus
}
#endif	// __cplusplus





#endif	// _DSP_PERI_H_
