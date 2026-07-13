/**
 * @file		ct_dd_uart.h
 * @brief		This is ct header for ct_dd_uart.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CT_DD_UART_H_
#define _CT_DD_UART_H_

#include "driver_common.h"

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
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern	VOID	Ct_Dd_Uart_Main(int argc, char** argv);
extern	VOID	Ct_Dd_Uart_PCSim_Test( VOID );

#endif	// _CT_DD_UART_H_
