/**
 * @file		fj_debug.h
 * @brief		Debug Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_DEBUG_H
#define _FJ_DEBUG_H

#include "ddim_typedef.h"
#include "fj_std.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_debug_overview
@{
	Output a message to the UART by using this API.<br>
	- Maximum size of UART ring buffer is 32,768bytes.<br>
	- After writing the maximum size it will overlap.<br>
	- Maximum size of One message is 256bytes.<br>
	<br>
	<br>
	@section fj_debug_Parameters Parameters Example.
	Parameters Format.<br>
	- ( "***** **** = %d, *** = %d \r\n", prm1, prm2 )
	<br>
	<br>
	- FJ_Debug_Print_Err( "FJ_Fs_Open() error. ercd=%d, %s\n", fid, filepath );<br>
	- FJ_Debug_Print_Err( "IPC_MHDR_Open() error. ret[0x%08x]\n", ret );<br>
@}*//* --- end of fj_debug_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_debug_sequence
@{
@}*//* --- end of fj_debug_sequence */

/*+ @weakgroup fj_debug_definition
@{*/
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
/*@} --- end of fj_debug_definition */

/** @weakgroup fj_debug_api
@{*/
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function is output debug messages to the UART.
@param[in] format message format.
@param[in] ... message parameter.
@remarks This API is for the exclusive use of the BaseFW-Log.<br>
         Output message is "D" + message.
 */
#if 1	// -- REMOVE_EXCLUDE_DOXYGEN BEGIN --
VOID FJ_Debug_Print_Dbg(const char* format, ...) __attribute__ ((format(printf, 1, 2)));
#else	// -- REMOVE_EXCLUDE_DOXYGEN ELSE --
VOID FJ_Debug_Print_Dbg(const char* format, ...);
#endif	// -- REMOVE_EXCLUDE_DOXYGEN END --

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function is output information message to the UART.
@param[in] format message format.
@param[in] ... message parameter.
@remarks This API is for the exclusive use of the BaseFW-Log.<br>
         Output message is "I" + message.
 */
#if 1	// -- REMOVE_EXCLUDE_DOXYGEN BEGIN --
VOID FJ_Debug_Print_Info(const char* format, ...) __attribute__ ((format(printf, 1, 2)));
#else	// -- REMOVE_EXCLUDE_DOXYGEN ELSE --
VOID FJ_Debug_Print_Info(const char* format, ...);
#endif	// -- REMOVE_EXCLUDE_DOXYGEN END --

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function is output warning message to the UART.
@param[in] format message format.
@param[in] ... message parameter.
@remarks This API is for the exclusive use of the BaseFW-Log.<br>
         Output message is "W" + message.
 */
#if 1	// -- REMOVE_EXCLUDE_DOXYGEN BEGIN --
VOID FJ_Debug_Print_Wrn(const char* format, ...) __attribute__ ((format(printf, 1, 2)));
#else	// -- REMOVE_EXCLUDE_DOXYGEN ELSE --
VOID FJ_Debug_Print_Wrn(const char* format, ...);
#endif	// -- REMOVE_EXCLUDE_DOXYGEN END --

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function is output error message to the UART.
@param[in] format message format.
@param[in] ... message parameter.
@remarks This API is for the exclusive use of the BaseFW-Log.<br>
         Output message is "E" + message.
 */
#if 1	// -- REMOVE_EXCLUDE_DOXYGEN BEGIN --
VOID FJ_Debug_Print_Err(const char* format, ...) __attribute__ ((format(printf, 1, 2)));
#else	// -- REMOVE_EXCLUDE_DOXYGEN ELSE --
VOID FJ_Debug_Print_Err(const char* format, ...);
#endif	// -- REMOVE_EXCLUDE_DOXYGEN END --

/*@} --- end of fj_debug_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_debug_message
@{*/
/*@} --- end of fj_debug_message */

#endif	// _FJ_DEBUG_H
