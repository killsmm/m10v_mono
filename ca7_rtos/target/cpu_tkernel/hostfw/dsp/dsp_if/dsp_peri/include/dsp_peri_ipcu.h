/**
 *
 * @file		dsp_peri_ipcu.h
 * @brief		Driver [ DSP Peripheral IPCU ]
 * @author		H.Maeda (SNI)
 * @since		2014/05/12
 * @note		
 * @attention	
 * @Copyright	Copyright (C) 2015 SOCIONEXT Inc. All Rights Reserved.
 *
 */

#ifndef _DSP_PERI_IPCU_H_
#define _DSP_PERI_IPCU_H_





/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_IPCU_MBOX_0 = 0,
	E_IPCU_MBOX_1,
	E_IPCU_MBOX_2,
	E_IPCU_MBOX_3,
	E_IPCU_MBOX_4,
	E_IPCU_MBOX_5,
	E_IPCU_MBOX_6,
	E_IPCU_MBOX_7,
	E_IPCU_MBOX_NUM
} E_IPCU_MBOX;


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_IPCU_OK			(0)
#define D_IPCU_NG_BUSY		(-1)

#define D_IPCU_MODE_ACK		(3)
#define D_IPCU_MODE_NO_ACK	(4)

#define D_IPCU_INT0_CPU		(0x01)
#define D_IPCU_INT1_CPU		(0x02)
#define D_IPCU_INT2_CPU		(0x04)
#define D_IPCU_INT3_CPU		(0x08)
#define D_IPCU_INT4_DSP		(0x10)

#define D_IPCU_NUM_DATA_REG	(9)


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define M_IPCU_GET_MBOX_IDX(intStatus)		( ( (0xfff & intStatus) - 0x100 ) >> 7 )
#define M_IPCU_BITMSK_REQ(intStatus)		( ( intStatus ) & ( 1 << 12 ) )
#define M_IPCU_BITMSK_ACK(intStatus)		( ( intStatus ) & ( 1 << 13 ) )


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* External Function													*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus


extern void DspPeri_IPCU_Get_MBOX_Status( unsigned int *status );

extern int  DspPeri_IPCU_Enable_MBOX( E_IPCU_MBOX	mBox,
									  unsigned int	srcInt,
		                              unsigned int 	dstInt );

extern void	DspPeri_IPCU_Disable_MBOX( E_IPCU_MBOX mBox );

extern void DspPeri_IPCU_Send( E_IPCU_MBOX 	mBox,
		                       unsigned int mode );

extern void DspPeri_IPCU_Set_Data( E_IPCU_MBOX 	mBox,
		                           int 			startIdx,
		                           int 			dataNum,
		                           unsigned int *dataAddr );

extern void DspPeri_IPCU_Get_Data( E_IPCU_MBOX 	mBox,
		                           int 			startIdx,
		                           int 			dataNum,
		                           unsigned int *dataAddr );

extern void DspPeri_IPCU_Set_Callback( unsigned int ipcuInt, 
									   void (*reqCallback)( unsigned int intStatus ),
									   void (*ackCallback)( unsigned int intStatus ) );

extern void DspPeri_IPCU_Int_Handler( unsigned int ipcuInt );


#ifdef __cplusplus
}
#endif	// __cplusplus





#endif	// _DSP_PERI_IPCU_H_
