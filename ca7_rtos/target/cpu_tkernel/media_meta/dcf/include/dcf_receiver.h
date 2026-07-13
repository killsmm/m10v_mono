/**
 * @file		dcf_receiver.h
 * @brief		Wrapper of MSG-BOX(receive dcf request)(header)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _DCF_RECV_H_
#define _DCF_RECV_H_

#include "dcf_sender.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
T_DCF_FUNC BF_Dcf_Receiver_Wait_Dcf_Event(T_DCF_SENDER_MSG** req_msg);
T_DCF_FUNC BF_Dcf_Receiver_Poll_Dcf_Event(T_DCF_SENDER_MSG* req_msg);
T_DCF_FUNC BF_Dcf_Receiver_Check_Dcf_Event(FW_VOID);

#endif
