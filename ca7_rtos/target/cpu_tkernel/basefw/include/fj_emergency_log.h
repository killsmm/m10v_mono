/**
 * @file		fj_emergency_log.h
 * @brief		Emergency Log Save API.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_EMERGENCY_LOG_H
#define _FJ_EMERGENCY_LOG_H

#include "ddim_typedef.h"
#include "fj_std.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_emergency_log_overview
@{
	When an emergency problem such as system hung-up occurs, the user can store <br>
	SDRAM data necessary to investigate cause by calling following APIs:<br>
	- FJ_EmergencyLog_Set_Log_Start_Sector()<br>
	- FJ_EmergencyLog_Store_Memory()<br>
	<br>
	These functions store SDRAM data of specified address/size to the external storage(e.g. NandFlash) <br>
	of specified sector. It is recommended that these functions are called on <br>
	a task-independent portion such as handler function, because it is assumed <br>
	that tasks do not work normally in such emergency states.<br>
	<br>
	
	After rebooting the system, the user can get the data stored on the external storage <br>
	as a binary file by calling following APIs:<br>
	- FJ_EmergencyLog_Set_Log_Start_Sector()<br>
	- FJ_EmergencyLog_Save()<br>
	<br>
	An external media(e.g. SD card) should be mounted on before calling these <br>
	functions.<br>
	<br>
	
	<b>
	NOTE:<br>
	These APIs need some size of area on the external storage to store log data.<br>
	However, the reference software has not reserved such area on the external storage.<br>
	</b>
	<br>
	
	@image html fj_emergency_log_intro.png
	
	<hr>
	- @ref fj_emergency_log_data_structure<br>
	- @ref fj_emergency_log_sample<br>
	
	<hr>
	
	@section fj_emergency_log_data_structure Log data structure
	Log data consists of 512byte units. The first unit has only header <br>
	information, and stored data are in the second and/or later units.<br>
	The header information includes user-specified label and stored data size.<br>
	If the size of stored data is not multiple of 512 bytes, remain area of<br>
	final unit is filled with FFh.<br>
	<br>
	
	@par Log data structure
	@image html fj_emergency_log_data_struct.png
	
	@par Header information data structure
	@image html fj_emergency_log_header_info_data_struct.png
	
	@par Acquisition Item
	<table>
		<tr>
		<th>Label</th>
		<th>Item</th>
		<th>Remarks</th>
		</tr>
		<tr>
		<td>0x01</td>
		<td>Terminal log pointer</td>
		<td>If the UART is enabled</td>
		</tr>
		<tr>
		<td>0x02</td>
		<td>Terminal log</td>
		<td>If the UART is enabled</td>
		</tr>
		<tr>
		<td>0x03</td>
		<td>Assertion log pointer</td>
		<td>If the Assertion is enabled</td>
		</tr>
		<tr>
		<td>0x04</td>
		<td>Assertion log</td>
		<td>If the Assertion is enabled</td>
		</tr>
		</tr>
		<tr>
		<td>0x0B</td>
		<td>Fast Boot Load Information</td>
		<td>If the Fast Boot is enabled</td>
		</tr>
		<tr>
		<td>0x11</td>
		<td>Register Information</td>
		<td></td>
		</tr>
		<tr>
		<td>0x12</td>
		<td>Call stack</td>
		<td></td>
		</tr>
		<tr>
		<td>0x13</td>
		<td>Register Information(MXIC)</td>
		<td></td>
		</tr>
		<tr>
		<td>0x21</td>
		<td>OS configuration information</td>
		<td></td>
		</tr>
		<tr>
		<td>0x22</td>
		<td>Task Information</td>
		<td></td>
		</tr>
		<tr>
		<td>0x23</td>
		<td>Semaphore information</td>
		<td></td>
		</tr>
		<tr>
		<td>0x24</td>
		<td>Event flag information</td>
		<td></td>
		</tr>
		<tr>
		<td>0x25</td>
		<td>Mailbox Information</td>
		<td></td>
		</tr>
	</table>
	
	<hr>
	@section fj_emergency_log_sample Sample of log store/file-save function
	
	@par Sample code to store log data to the NandFlash.
	@code
	FJ_EmergencyLog_Set_Log_Start_Sector(startSector);				// Sets starting sector of NandFlash to write log data
	FJ_EmergencyLog_Store_Memory(startAddr, ((1 << 24)|0x000200));	// Stores 512 bytes to NandFlash(Label: 1)
	FJ_EmergencyLog_Store_Memory(startAddr, ((1 << 24)|0x000400));	// Stores 1024 bytes to NandFlash(Label: 1)
	FJ_EmergencyLog_Store_Memory(startAddr, ((2 << 24)|0x000100));	// Stores 256 bytes to NandFlash(Label: 2)
	@endcode
	
	NOTE:<br>
	Once FJ_EmergencyLog_Set_Log_Start_Sector() function has been called, <br>
	the user can call FJ_EmergencyLog_Store_Memory() function any times.<br>
	The log data are stored continuously in the NandFlash as shown in <br>
	following figure.
	<br>
	
	@par Log data stored by the sample code.
	@image html fj_emergency_log_sample_data.png
	
	@par Sample code to save log data as a file.
	@code
	FJ_EmergencyLog_Set_Log_Start_Sector(startSector);	// Sets starting sector of NandFlash to read log data.
	FJ_EmergencyLog_Save("I:\DEBUG.LOG");				// Saves the log data as a binary file.
	@endcode
	
	NOTE:<br>
	The starting sector should be same as that has been specified when storing the log data.<br>
	A single FJ_EmergencyLog_Save() function call saves all log data shown in the figure above <br>
	as a single binary file.
@}*//* --- end of fj_emergency_log_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_emergency_log_sequence
@{

	Prease refer to @ref fj_emergency_log_sample "Sample of log store/file-save function"<br>

@}*//* --- end of fj_emergency_log_sequence */

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/*+ @weakgroup fj_emergency_log_definition
@{*/
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
/*@} --- end of fj_emergency_log_definition */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/** @weakgroup fj_emergency_log_api
@{*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Sets starting sector of external storage in order to read/write log data
@param[in] sector starting sector number
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
	Please call this function before calling FJ_EmergencyLog_Store_Memory() and FJ_EmergencyLog_Save() functions.<br>
	This function needs some size of area on the external storage to read/write log data.<br>
	However, the reference software has not reserved such area on the external storage.<br>
*/
FJ_ERR_CODE FJ_EmergencyLog_Set_Log_Start_Sector(ULONG sector);


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Stores specified memory data to the external storage.
@param[in] startAddr log data address
@param[in] headerInfo log header information<br>
						31-16bit : Log label ID<br>
						15-0 bit : Log data size<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
	Before calling this function, please call FJ_EmergencyLog_Set_Log_Start_Sector() function<br>
	in order to set starting sector of external storage to write log data.<br>
	Log data are stored in 512-byte units. If the log data size is not multiple of 512 bytes,<br>
	remain area of final unit is filled with FFh.<br>
*/
FJ_ERR_CODE FJ_EmergencyLog_Store_Memory(UINT32 startAddr, INT32 headerInfo);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Saves log data(stored in the external storage) as a file in an external media(SD).
@param[in] filename file name
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
	Media should be mounted on before this API is called.<br>
	Before calling this function, please call FJ_EmergencyLog_Set_Log_Start_Sector() function<br>
	in order to set starting sector of external storage where the log data are stored.<br>
	<br>
	The filename should be specified in full file path.<br>
	(example : "I:\DEBUG.LOG")
*/
FJ_ERR_CODE FJ_EmergencyLog_Save(UCHAR* filename);

/*@} --- end of fj_emergency_log_api */


///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_emergency_log_message
@{*/
/*@} --- end of fj_emergency_log_message */

///////////////////////////////////////////////////////////////////////////////
// callback
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_emergency_log_callback
@{*/
/*@} --- end of fj_emergency_log_callback */

#endif	// _FJ_EMERGENCY_LOG_H
