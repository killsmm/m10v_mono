/**
 * @file		fj_debug_mba.h
 * @brief		MBA Debug API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_DEBUG_MBA_H
#define _FJ_DEBUG_MBA_H

// --- REMOVE_MBA BEGIN ---
#include "ddim_typedef.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_debug_mba_overview
@{
	MBA(MilBeaut Analyzer) log is used to measure and analyze processing time of target tasks and/or functions. <br>
	<br>
	By calling log-print API, the user can record time when the target process has become active (It is called as "point" type recording). <br>
	This API can also record start-end time of target process (It is called as "range" type recording). <br>
	<br>
	The recorded data can be saved as a text formatted file by calling log-output API. <br>
	<br>
	"MB Analyzer" tool is usefull to analyze the output data. <br>
	This tool can display the output data graphically on PC. <br>
	By using this tool, the user can easily find which tasks and/or functions take time. <br>
	For details of "MB Analyzer", please refer to @ref fj_debug_mba_tool "here". <br>
	<br>
	<br>
	@image html fj_debug_mba_intro.png
	<br>
	<br>
	
	@section fj_debug_specification_of_mba Specification of MBA log data
	- MBA log data is stored in DRAM.<br>
	- Size of one MBA log data is 28bytes.<br>
	- Maximum number of MBA log data is 1024.<br>
	  If the stored number of MBA log data is over the maximum number (1024), the stored data is overwritten from oldest one.<br>
	- Measurement time in microseconds.
	- Refer to @ref fj_debug_mba_definition for other limitations.<br>
	- MBA log structure is as follows.<br>
	<br>
	<table>
		<tr>
			<th rowspan="2">Header</th>
			<td>Firmware Version</td>
			<td>// BaseFW Version "XXXXXXXX"</td>
		</tr>
		<tr>
			<td>
				MBA log status<br>
				<br>
				&nbsp;0: Normal<br>
				&nbsp;1: Size-Full<br>
				&nbsp;2: OverWritten<br>
			</td>
			<td>// Log Status "X"</td>
		</tr>
		<tr>
			<th>Definition</th>
			<td>
				Definition of task ID name(linename)<br>
				&nbsp;and function ID name(identname).<br>
				<br>
				These names are customizable.<br>
			</td>
			<td>
				linename00="None"<br>
				linename01="BOOT"<br>
				linename02="AE"<br>
				linename03="LENS"<br>
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;:<br>
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;:<br>
				identname00="Host"<br>
				identname01="System"<br>
				identname02="Load"<br>
				identname03="LCD"<br>
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;:<br>
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;:<br>
			</td>
		</tr>
		<tr>
			<th>Data</th>
			<td>
				Measurement data.<br>
				<br>
				Data format - Time:ID_1,ID_2,Point,"String"<br>
				<br>
				<center>
				<table>
				<tr><td>Time<td>Elapsed time[sec]</tr>
				<tr><td>ID_1<td>Task ID</tr>
				<tr><td>ID_2<td>Function ID</tr>
				<tr><td>Point<td>Kind of measurement point</tr>
				<tr><td>String<td>Voluntary character string</tr>
				</table>
				</center>
			</td>
			<td>
				0.784312:0,4,start,"proc measure1"<br>
				0.788005:0,4,end,"proc measure1"<br>
				0.788875:0,6,start,"proc measure2"<br>
				0.788989:1,13,start,"proc measure4"<br>
				0.792325:0,6,end,"proc measure2"<br>
				0.792780:0,15,point,"point time1"<br>
				0.792951:0,11,start,"proc measure3"<br>
				0.801002:1,13,end,"proc measure4"<br>
				0.801356:1,9,start,"proc measure5"<br>
				0.801500:2,53,start,"proc measure6"<br>
				0.811779:2,53,end,"proc measure6"<br>
				0.811909:2,47,start,"proc measure7"<br>
				0.823554:1,9,end,"proc measure3"<br>
				0.823962:1,15,point,"point time2"<br>
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;:<br>
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;:<br>
			</td>
		</tr>
	</table>
	<br>
	
	@par How to use:
	- @ref fj_debug_mba_preparation
	- @ref fj_debug_mba_host_sample_code
	<br>
	
	<hr>
	@section fj_debug_mba_preparation Preparation
	-# Depending on a use, please change the following compile options in "cpu_rtos/project", and build.
	<br>
	<table>
		<tr>
		<th>Compile Option</th>
		<th>Detail</th>
		<th>note</th>
		</tr>
		<tr>
		<td>CO_DEBUG_MBALOG_ON</td>
		<td>
			- enable<br>
			MBA function ON.<br>
			<br>
			- disable<br>
			MBA function OFF.<br>
		</td>
		<td>none.</td>
		</tr>
		<tr>
		<td>CO_DEBUG_MBALOG_OVERWRITE</td>
		<td>
			- enable<br>
			When the log buffer is full, overwriting from the beginning of the buffer.<br>
			<br>
			- disable<br>
			When the log buffer is full, MBA log recording stops.<br>
		</td>
		<td>To enable this option, Please enable CO_DEBUG_MBALOG_ON.</td>
		</tr>
	</table>
	<br>
	-# Please customize task ID name and function ID name for Host.
	The following is an example.<br>
	<br>
	- FJ_MBALOG_FUNC_ID_HOSTFUNC1 -> <b>FJ_MBALOG_FUNC_ID_HOST_CUSTOM</b>
	- "Host_Task1" -> <b>"Host_Custom"</b>
	- "HostFunc1" -> <b>"Customize"</b>
	- * The maximum number of "Task ID Name" is FJ_MBALOG_TASK_ID_MAX(128).
	- * The maximum number of "Function ID" and "Function ID Name" is FJ_MBALOG_FUNC_ID_MAX(128).
	<br>
	<table>
		<tr><th>File Name</th><th>Customize Sample</th></tr>
		<tr><td>Common\\include\\fj_host_debug.h</td>
		<td>: Enumeration of Function ID (FJ_MBALOG_FUNC_ID)<br>
		@code
		// fj_host_debug.h //
		// Function ID
		typedef enum {
			FJ_MBALOG_FUNC_ID_HOST = 0,
			FJ_MBALOG_FUNC_ID_SYSTEM,
						:
						:
			FJ_MBALOG_FUNC_ID_HOST_CUSTOM,	<-- Replaced (Old:FJ_MBALOG_FUNC_ID_HOSTFUNC1)
			FJ_MBALOG_FUNC_ID_HOSTFUNC2,	<-- Customizable
			FJ_MBALOG_FUNC_ID_HOSTFUNC3,	<-- Customizable
			FJ_MBALOG_FUNC_ID_HOSTFUNC4,	<-- Customizable
			FJ_MBALOG_FUNC_ID_HOSTFUNC5,	<-- Customizable
		} FJ_MBALOG_FUNC_ID;
		@endcode
		</td>
		</tr>
		<tr><td>Host\\src\\fj_host_debug.c</td>
		<td>: Name of Task ID and Function ID (gMBALog_Id_List)<br>
		@code
		// fj_host_debug.c //
		FJ_MBALOG_ID_LIST gMBALog_Id_List = {
			// Task ID Name (Line Name)
			{
				"None",
				"BOOT",
					:
					:
				"Host_Custom",		<-- Replaced (Old:"Host_Task1")
				"Host_Task2",		<-- Customizable
				"Host_Task3",		<-- Customizable
				"Host_Task4",		<-- Customizable
					:
					:
				"Host_Task7",		<-- Customizable
				"Host_Task8",		<-- Customizable
			},

			// Function ID Name
			{
				"Host",
				"System",
					:
					:
				"Customize",		<-- Replaced (Old:"HostFunc1")
				"HostFunc2",		<-- Customizable
				"HostFunc3",		<-- Customizable
				"HostFunc4",		<-- Customizable
				"HostFunc5",		<-- Customizable
			}
		};
		@endcode
		</td>
		</tr>
	</table>
	<br>
	@par Customized data is displayed like the following on "MB Analyzer".
	<br>
	@image html fj_debug_mba_customize.png
	<br>
	
	@section fj_debug_mba_host_sample_code Host Sample Code
	<br>
	-# Please call FJ_MBA_PRINTF() at source code where you want to record data.<br>
	There are two kinds of recording type : point type and range(start-end) type.<br>
	In the sample code described below, sample 1 and 2 are range type, <br>
	and sample 3 and 4 are point type.<br>
	<br>
	-# If you want to save the recorded data as a file, please call FJ_MBA_OUTPUT() function.<br>
	<br>
	@code
	// SAMPLE CODE //
	#include "fj_host_debug.h"
	
	VOID mba_sample_log( VOID )
	{
		FJ_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_HOST_CUSTOM, FJ_MBALOG_TYPE_START, "sample1 range" ));
		//
		// Measure target process
		// (For example, the process takes 100 msec)
		//
		FJ_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_HOST_CUSTOM, FJ_MBALOG_TYPE_END, "sample1 range" ));
		
		FJ_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_HOSTFUNC2, FJ_MBALOG_TYPE_START, "sample2 range" ));
		//
		// Measure target process
		// (For example, the process takes 200 msec)
		//
		FJ_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_HOSTFUNC2, FJ_MBALOG_TYPE_END, "sample2 range" ));
		
		FJ_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_HOSTFUNC3, FJ_MBALOG_TYPE_POINT, "sample3 point" ));
		//
		// Measure target process
		// (For example, the process takes 300 msec)
		//
		FJ_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_HOSTFUNC4, FJ_MBALOG_TYPE_POINT, "sample4 point" ));
		
		FJ_MBA_OUTPUT( "SAMPLE" );
	}
	@endcode
	<br>
	Even if FJ_MBA_OUTPUT() is not called, it is possible to output file by the following command on the terminal.<br>
	These commands are enabled by "CO_DEBUG_MBALOG_ON" option automatically.<br>
	<br>
	&nbsp;&nbsp;&nbsp;&nbsp;>c&nbsp;&nbsp;&nbsp;&nbsp;(Only first time)<br>
	&nbsp;&nbsp;&nbsp;&nbsp;>host mba output 1 [filename]<br>
	<br>
	-# Stored data by the above sample code is displayed like the following on "MB Analyzer".
	<br>
	@image html fj_debug_mba_sample_code.png
	<br>
	-# Please call FJ_MBA_CLEAR() function when you want to start a new measurement.
@}*//* --- end of fj_debug_mba_overview */

///////////////////////////////////////////////////////////////////////////////
// relational documents
///////////////////////////////////////////////////////////////////////////////
/** @addtogroup fj_debug_mba_tool MB Analyzer
@{
	@section Introduction
	"MB Analyzer" is a windows tool that can analyze process time graphically, 
	and it is possible to easily analyze sequential processing time by each macro, 
	task or something.<br>
	This tool uses a designated format that can be stored to .MBA file by FJ_MBA_PRINTF(), 
	and please refer to @ref fj_debug_mba_overview "here" how to use this function.<br>
	
	@section Requirements
	This tool works on Windows7.<br>
	<br>
	
	@section Usage
	- @ref fj_debug_mba_tool_file_open
	- @ref fj_debug_mba_tool_description_window
	- @ref fj_debug_mba_tool_display_mode
	- @ref fj_debug_mba_tool_zoom
	- @ref fj_debug_mba_tool_line_width
	- @ref fj_debug_mba_tool_jump
	- @ref fj_debug_mba_tool_search
	- @ref fj_debug_mba_tool_drag_selection
	- @ref fj_debug_mba_tool_screen_shot
	
	<hr>
	@section fj_debug_mba_tool_file_open File open
	Double clicking the "MBAnalyzer.exe" icon, and the MBA file must drug & drop, or select it by [File] - [Open] of the menu.<br>
	<br>
	
	@section fj_debug_mba_tool_description_window Description of window
	@image html fj_debug_mba_usage.png
	<br>
	
	@section fj_debug_mba_tool_display_mode Display mode(Line or ID)
	It is possible to switch to Line mode or ID mode for display.<br>
	<br>
	- Line mode
	The processing time of each task is displayed.<br>
	<br>
	@image html fj_debug_mba_sample_image_line.png
	<br>
	- ID mode<br>
	The processing time of each Function ID is displayed.<br>
	<br>
	@image html fj_debug_mba_sample_image_id.png
	<br>
	
	@section fj_debug_mba_tool_zoom Zoom
	The zoom-in and zoom-out are possible in the displayed range.<br>
	The zoom works by using "selection bar" as center.<br>
	<br>
	<table border="0">
		<tr><td>Zoom-in</td>
		<td>: Shift + Down</td>
		</tr>
		<tr><td>Zoom-out</td>
		<td>: Shift + Up</td>
		</tr>
	</table>
	<br>
	- Zoom-In (2ms)
	<br>
	@image html fj_debug_mba_zoom_in.png
	<br>
	- Zoom-Out (50ms)
	<br>
	@image html fj_debug_mba_zoom_out.png
	<br>
	
	@section fj_debug_mba_tool_line_width Line width
	<table border="0">
		<tr>
		<td>Wide</td>
		<td>: Shift + Right</td>
		</tr>
		<tr>
		<td>Narrow</td>
		<td>: Shift + Left</td>
		</tr>
	</table>
	<br>
	- Wide<br>
	<br>
	@image html fj_debug_mba_line_width_wide.png
	<br>
	- Narrow<br>
	<br>
	@image html fj_debug_mba_line_width_narrow.png
	<br>
	
	@section fj_debug_mba_tool_jump Jump
	<table border="0">
		<tr>
		<td>Jump to next measurement point.</td>
		<td>: Up</td>
		</tr>
		<tr>
		<td>Jump to previous measurement point.</td>
		<td>: Down</td>
		</tr>
	</table>
	<br>
	@image html fj_debug_mba_jump.png
	<br>
	
	@section fj_debug_mba_tool_search Search
	<table border="0">
		<tr>
		<td>Search string</td>
		<td>: Ctrl + F</td>
		</tr>
		<tr>
		<td>Prev string</td>
		<td>: Shift + F3</td>
		</tr>
		<tr>
		<td>Next string</td>
		<td>: F3</td>
		</tr>
	</table>
	<br>
	
	@section fj_debug_mba_tool_drag_selection Drag selection
	Display of a detail information about dragged area.<br>
	<br>
	@image html fj_debug_mba_drag_selection.png
	<br>
	
	@section fj_debug_mba_tool_screen_shot Screen shot
	Press "Ctrl + S", or save it by [File] - [Screen Shot] of the menu.<br>
@}*//* --- end of fj_debug_mba_tool */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_debug_mba_sequence
@{
@}*//* --- end of fj_debug_mba_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_debug_mba_definition
@{*/

/**
MBA Log Record Max Count
@remarks Don't change it without previous notice to SocioNext.
*/
#define FJ_MBALOG_RECORD_MAX		(1024)

/**
MBA Log Task ID Max Count
@remarks Don't change it without previous notice to SocioNext.
*/
#define FJ_MBALOG_TASK_ID_MAX		(256)

/**
MBA Log Function ID Max Count
@remarks Don't change it without previous notice to SocioNext.
*/
#define FJ_MBALOG_FUNC_ID_MAX		(256)

/**
MBA Log Line ID Name Max Length
@remarks Don't change it without previous notice to SocioNext.
*/
#define FJ_MBALOG_TASK_ID_LENGTH	(17)

/**
MBA Log Function ID Name Max Length
@remarks Don't change it without previous notice to SocioNext.
*/
#define FJ_MBALOG_FUNC_ID_LENGTH	(13)

/**
MBA Log Text Max Length
@remarks Don't change it without previous notice to SocioNext.
*/
#define FJ_MBALOG_TEXT_LENGTH		(21)

///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** MBA Log Type */
typedef enum {
	FJ_MBALOG_TYPE_POINT = 0,				/**< Point type */
	FJ_MBALOG_TYPE_START,					/**< Beginning of the range type */
	FJ_MBALOG_TYPE_END						/**< End of the range type */
} FJ_MBALOG_TYPE;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** MBA Log ID Name List Structure */
typedef struct 
{
	CHAR func_id_name[FJ_MBALOG_FUNC_ID_MAX][FJ_MBALOG_FUNC_ID_LENGTH];		/**< Function ID Name */
}FJ_MBALOG_ID_LIST;


///////////////////////////////////////////////////////////////////////////////
// Global Data
///////////////////////////////////////////////////////////////////////////////

/*@} --- end of fj_debug_mba_definition */

///////////////////////////////////////////////////////////////////////////////
// macro
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_debug_mba_api
@{*/
// --- REMOVE_MBA END ---
// --- REMOVE_MBA BEGIN ---
#ifdef CO_DEBUG_MBALOG_ON
	#define FJ_MBA_PRINTF(arg)					(FJ_MBALog_Printf arg)
	#define FJ_MBA_OUTPUT(file_name)			(FJ_MBALog_Output(file_name))
	#define FJ_MBA_CLEAR()						(FJ_MBALog_Clear())
#else
	/**
	<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
	Records the MBA log.
	@param [in]  arg	This parameter is inclusive of the following parameters.<br>
						Please set the following parameters when using this API.
						<table border="0">
						<tr><td valign="top">[in]</td><td valign="top">func_id</td><td>Funftion ID.<br>
								Specify the index number of "fid_name" defined in FJ_MBALOG_ID_LIST.<br></td></tr>
						<tr><td>[in]</td><td>type</td><td>MBA Log Type. See @ref FJ_MBALOG_TYPE.</td></tr>
						<tr><td valign="top">[in]</td><td valign="top">fmt</td><td>Format specification character string.<br>
								Number of maximum characters when this parameter is matched to Argument list is defined in @ref FJ_MBALOG_TEXT_LENGTH.<br>
								Set "NULL" when not specifying it.<br>
								Set the same character string with @ref FJ_MBALOG_TYPE_START and @ref FJ_MBALOG_TYPE_END when specifying it.</td></tr>
						</table>
	@remarks Task ID is acquired in the this API. Therefore, specification is unnecessary.<br>
			Please enable "CO_DEBUG_MBALOG_ON" of OptionFile to execute this function.<br>
			Please refer to @ref fj_debug_mba_overview for details of MBA Log.<br>
			<br>
			Sample code is as follows.<br>
@code
	FJ_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_HOSTFUNC1, FJ_MBALOG_TYPE_START, "sample1" ));
	//
	// Measure target process
	//
	FJ_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_HOSTFUNC1, FJ_MBALOG_TYPE_END, "sample1" ));
@endcode
	*/
// --- REMOVE_MBA END ---
	#define FJ_MBA_PRINTF(arg)
// --- REMOVE_MBA BEGIN ---
	
	/**
	<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
	Output the MBA log as text format.(for RTOS debug)
	@param [in]  file_name	File name of the output MBA log file.<br>
				 			Number of maximum characters of file name is 8 characters.<br>
							For example, when "SAMPLE" is specified for file_name, the following files are output.<br>
							<br>
							&nbsp;&nbsp;&nbsp;&nbsp;I:\\SAMPLE.MBA
							<br>
							&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; * External Storage Drive "I:\": Automatically added<br>
							&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; * Extension ".MBA": Automatically added<br>
	@remarks The log isn't recorded while outputting the log.<br>
			When the file of the same name already exists, it is overwrited.<br>
			Please enable "CO_DEBUG_MBALOG_ON" of OptionFile to execute this function.<br>
			Please refer to @ref fj_debug_mba_overview for details of MBA Log.
	*/
// --- REMOVE_MBA END ---
	#define FJ_MBA_OUTPUT(file_name)
// --- REMOVE_MBA BEGIN ---
	
	/**
	<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
	Clear the MBA log.
	@remarks When this API is called, all logs are initialized.<br>
			Please enable "CO_DEBUG_MBALOG_ON" of OptionFile to execute this function.<br>
			Please refer to @ref fj_debug_mba_overview for details of MBA Log.
	*/
// --- REMOVE_MBA END ---
	#define FJ_MBA_CLEAR()
// --- REMOVE_MBA BEGIN ---
#endif

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
#if 1	// -- REMOVE_EXCLUDE_DOXYGEN BEGIN --
extern VOID FJ_MBALog_Printf(UCHAR func_id, FJ_MBALOG_TYPE type, const CHAR* fmt, ...) __attribute__ ((format(printf, 3, 4)));
#else	// -- REMOVE_EXCLUDE_DOXYGEN ELSE --
extern VOID FJ_MBALog_Printf(UCHAR func_id, FJ_MBALOG_TYPE type, const CHAR* fmt, ...);
#endif	// -- REMOVE_EXCLUDE_DOXYGEN END --
extern VOID FJ_MBALog_Output(const CHAR* file_name);
extern VOID FJ_MBALog_Clear(VOID);

/*@} --- end of fj_debug_mba_api */
///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_debug_mba_message
@{*/
/*@} --- end of fj_debug_mba_message */

///////////////////////////////////////////////////////////////////////////////
// callback
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_debug_mba_callback
@{*/
/*@} --- end of fj_debug_mba_callback */

// --- REMOVE_MBA END ---
#endif
