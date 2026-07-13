/**
 * @file		fj_message.h
 * @brief		message header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_MESSAGE_H
#define _FJ_MESSAGE_H

#include "ddim_typedef.h"
#include "fj_std.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_message_overview
@{
	This chapter describes API related to message control.<br>
	Base FW side is set a notification for host side.<br>
	Host side get a notification from Base FW side.

@}*//* --- end of fj_message_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_message_sequence
@{
	See @ref fj_overview_message of overview.
@}*//* --- end of fj_message_sequence */

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_message_api
@{*/
/* ------------- */
/* Host          */
/* ------------- */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
FJ host should wait for the message using this function.
@param [out] pulMessage	Message identification.
@param [out] pulParam	Message parameter(if any).
*/
extern VOID	FJ_HostGetMessage(  ULONG* pulMessage, ULONG* pulParam );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
FJ host should post the message to queue using this function.
@param [in] ulMessage	Message identification.
@param [in] ulParam	Message parameter(if any).
*/
extern VOID	FJ_HostPostMessage( ULONG ulMessage, ULONG ulParam );

/* ------------- */
/* Interrupt     */
/* ------------- */
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Once some host message(HM) is blocked by this API, it will not be sent to side of HOST until it's unblocked.<br>
The HM that should be blocked has to be specified as a parameter.
@param [in] wConfig		Config mode.<br>
						&nbsp;&nbsp;&nbsp;&nbsp;FJ_PARA_HOST_MSG_BLOCK (0x1001) : HostMessageBlock<br>
						&nbsp;&nbsp;&nbsp;&nbsp;FJ_PARA_HOST_MSG_UNBLOCK (0x1002) : Not Block 
@param [in] wValue		HM ID.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE	FJ_ConfigHostMessage( USHORT wConfig,  ULONG wValue );
/*@} --- end of fj_message_api */

#endif
