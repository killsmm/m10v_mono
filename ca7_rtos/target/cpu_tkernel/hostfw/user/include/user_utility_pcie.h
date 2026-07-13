/**
 * @file		user_utility_pcie.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _USER_UTILITY_PCIE_H_
#define _USER_UTILITY_PCIE_H_

#include "share_data.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

extern VOID User_Utility_PCIE_HW_Init( UCHAR mode );

/*==============================================================================*/
/* For PCIE EP mode																*/
/*==============================================================================*/
extern VOID User_Utility_PCIE_EP_DMATrans_Sam1( VOID );
extern VOID User_Utility_PCIE_EP_DMATrans_DataSend_Sam1( VOID );
extern VOID User_Utility_PCIE_EP_DMATrans_DataGet_Sam1( VOID );

/*==============================================================================*/
/* For PCIE RC mode																*/
/*==============================================================================*/
extern VOID User_Utility_PCIE_RC_DMATrans_Sam1( VOID );
extern VOID User_Utility_PCIE_RC_DMATrans_Config_Sam1( VOID );
extern VOID User_Utility_PCIE_RC_DMATrans_DataSend_Sam1( VOID );
extern VOID User_Utility_PCIE_RC_DMATrans_DataGet_Sam1( VOID );

#ifdef __cplusplus
}
#endif

#endif	// _USER_UTILITY_PCIE_H_

