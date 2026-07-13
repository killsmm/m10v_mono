/**
 * @file		fj_boot.h
 * @brief		boot header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_BOOT_H
#define _FJ_BOOT_H

#include "ddim_typedef.h"
#include "fj_std.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_boot_overview
@{
	Boot processing starts after all program codes and data has been loaded to SDRAM.<br>
	The BaseFW calls pre-boot callback function three times during the boot processing.<br>
	- 1st stage<br>
	  BaseFW calls after the initialization of RTOS, Clock, Interrupt, Image Macro, Display Macro, etc..<br>
	  Therefore, please execute the initialization such as sensors.<br><br>
	- 2nd stage<br>
	  BaseFW calls after the start each task and Image Quality initialization.<br>
	  Therefore, please execute the setting such as sensor and image.<br><br>
	- 3rd stage<br>
	  Please do any implementation as necessary before FJ_HostProc() is called.<br>

//@cond
	<br>
	@image html fj_boot_overview.png
	<br>
    <br>
	-# Determining number of sections and section names<br>
	The user should determine number of sections and section names. The section names<br>
	are used in souce codes and scatter file.<br>
	As an example here, let us define four sections: FIRST, SECOND, THIRD, FOURTH,<br>
	and define section names as follows:<br>
	<br>
	<b>Section names</b><br>
	FIRST_CODE<br>
	FIRST_INIT<br>
	FIRST_CONST<br>
	<br>
	SECOND_CODE<br>
	SECOND_INIT<br>
	SECOND_CONST<br>
	<br>
	THIRD_CODE<br>
	THIRD_INIT<br>
	THIRD_CONST<br>
	<br>
	FOURTH_CODE<br>
	FOURTH_INIT<br>
	FOURTH_CONST<br>
	<br>
	Note that three section names(_CODE/_INIT/_CONST) are defined for each section.<br>
	These names _CODE, _INIT, _CONST are used to distinguish program code, read/write data<br>
	and read-only data, respectively.<br>
	<br>
	-# Editing source files<br>
	The user needs to add statements "#pragma ...." to all source files using<br>
	section names we have defined in order to specify which sections the functions<br>
	and global data should be placed.<br>
	An example of modified source file is shown in the followig figure.<br>
	<br>
	<center><b>\<Source File Example\></b></center><br>
	@image html fj_boot_overview_source_file.png
	<br>
	-# Editing scatter file(cpu_rtos/project)<br>
	Scatter file determines memory map of each section. Its description consists<br>
	of several "REGION"s, and each "REGION" consists of several sections.<br>
	In our example here, we define three new REGIONs as show in the following table:<br>
	<br>
		<table>
		  <tr>
			<th>REGION Name</th>
			<th>Note</th>
		  </tr>
		  <tr>
			<td>SECOND_REGION</td>
			<td>Used to place the SECOND section(SECOND_CODE, SECOND_INIT, SECOND_CONST).</td>
		  </tr>
		  <tr>
			<td>THIRD_REGION</td>
			<td>Used to place the THIRD section(THIRD_CODE, THIRD_INIT, THIRD_CONST).</td>
		  </tr>
		  <tr>
			<td>FOURTH_REGION</td>
			<td>Used to place the FOURTH section(FOURTH_CODE, FOURTH_INIT, FOURTH_CONST).</td>
		  </tr>
		</table>
	<br>
	Note that we don't have to define FIRST_REGION because it has already been defined<br>
	in the scatter file, so we only place our FIRST section(FIRST_CODE, FIRST_INIT,<br>
	FIRST_CONST) within the FIRST_REGION.<br>
	The new REGIONs(SECOND_REGION, THIRD_REGION, FOURTH_REGION) should be placed between<br>
	existing regions: FIRST_REGION and WORK_REGION.<br>
	(Information: Description of SECOND, THIRD, and FOURTH REGION exists in the scatter<br>
	file of the refference software, although it is disabled by comments. So the user<br>
	can easily define these REGIONs by removing comments.)<br>
	<br>
	An example of modified scatter file is shown in the followig figure.<br>
	<br>
	<center><b>\<Scatter File Example\></b></center><br>
	@image html fj_boot_overview_scatter_file.png
	<br>
	In addition, we need to specify start addresses of new REGIONs(SECOND_REGION,<br>
	THIRD_REGION, FOURTH_REGION) and existing WORK_REGION located at the end of<br>
	the scatter file.<br>
	For REGIONs except for SECOND_REGION, we can set start address as "+0" as<br>
	shown in the figure above. By specifying "+0", the linker automatically<br>
	determines the start address of these REGIONs.<br>
	However, for the SECOND_REGION, the ueser should determine its start address<br>
	explicitly. Note that the start address of SECOND_REGION should be 512byte-<br>
	boundary-aligned and be larger than end address of "BOOT_RAM" which is located<br>
	in the FIRST_REGION. The user can estimate the end address of the BOOT_RAM by<br>
	building the project once in order to create map file(cpu_rtos/project/bin).<br>
	Start address and size of the BOOT_RAM are described in the map file.<br>
	for example:<br>
	<br>
@code
	Execution Region BOOT_RAM (Base: 0xa0000000, Size: 0x003ce100, Max: 0xffffffff, ABSOLUTE)
@endcode
	<br>
	-# Modifying NAND.par file<br>
	NAND.par file includes information necessary for ROM boot program to copy firmware<br>
	codes and data to SDRAM. For fast boot, the user needs to modify<br>
	<b>line 33</b> of the NAND.par file to specify size of FIRST REGION.<br>
	The user can find the size of FIRST REGION in map file which is created when<br>
	build is executed(cpu_rtos/project/bin).<br>
	for example:<br>
@code
	Load Region FIRST_REGION (Base: 0xa0000000, Size: 0x003cf07c, Max: 0xffffffff, ABSOLUTE)
@endcode
	<br>
//@endcond
@}*//* --- end of fj_boot_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_boot_sequence
@{
	- @ref Boot_Sequence
	<hr>
	@section Boot_Sequence Normal Boot Processing Sequence
	Booting task of BaseFW calls following host callback functions:<br>
	- FJ_HostPreBootProc()<br>
    The BaseFW calls this function three times.<br>
    Each call is called as "stage" : 1st stage and 2nd stage.<br>
    1st stage : Hardware is chiefly initialized.<br>
    2nd stage : Display setting is chiefly done.<br>
    3rd stage : Other setting is chiefly done.<br>
     <br>
	- FJ_HostProc()<br>
    The BaseFW calls this function to start host main process.<br>
	It includes starting event observer task(key-push, dial-move, and other<br>
	peripheral devices as external media, HDMI, video jack, etc.), and main loop<br>
	in which host receives messages from the BaseFW and process accordingly.<br>
    <br>
	As an example, processing of the reference software is shown in following<br>
	sequence.<br>
	<br>
	@image html fj_boot_main.png
	<br>
	@code
	[Sample Code of Host main loop]
	VOID FJ_HostProc( ULONG ulArgument ) {
		ULONG uMsgID;
		ULONG uMsgPara;

		// Change task priority for host main process.
		// Start event observer task

		while( 1 ) {
			FJ_HostGetMessage( &uMsgID, &uMsgPara );
			if( uMsgID != 0 ){
				// Execution Host Side Code
			}
		}
	}
	@endcode

@}*//* --- end of fj_boot_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_boot_definition
@{*/
#define FJ_PRE_PROC_ENTER_FIRST		(0)	/**< 1st boot process. */
#define FJ_PRE_PROC_ENTER_SECOND	(1)	/**< 2nd boot process. */
#define FJ_PRE_PROC_ENTER_THIRD		(2)	/**< 3rd boot process. */
/*@} --- end of fj_boot_definition */

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_boot_api
@{*/

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function gets the condition of boot.
@param [out]	bootCond		Condition of boot.<br>
								- 1 : boot after Cold Reset
								- 2 : boot after Warm Reset
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error. (if bootCond is NULL address)
*/
extern FJ_ERR_CODE FJ_GetBootCond( LONG* bootCond );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function loads code and data on Nand Flash to SDRAM.
@param [in]	areaNo			load area No.<br>
								- 0 : All
								- 1 : second_region
@param [in]	dstStartAdr		start distination on SDRAM address.
@param [in]	loadSize		load size (byte).
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error.
@remarks
			- This function will start a boot loading task.<br>
			- You should wait for DORMANT status of the task using by FJ_BootLoadState function, if you want to use this function.<br>
			- DstStartAdr and loadSize must be multiples of 512.
*/
extern FJ_ERR_CODE FJ_BootLoad( UCHAR areaNo, char* dstStartAdr, ULONG loadSize );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function can get status whether a boot loading task is DORMANT or not.
@retval 0	DORMANT status of a boot loading task.
@retval -1	Not DORMANT status of a boot loading task.
*/
extern INT32 FJ_BootLoadState( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function is called before resetting CPU.
*/
extern VOID FJ_Reboot_Pre( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
When this function is called, system reboot is executed.
*/
extern VOID FJ_Reboot( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function print the terminal boot messages.
*/
extern VOID FJ_BootMessagePrint( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function can get version information for BaseFW.
@return		Version Information
*/
extern ULONG FJ_Get_OK_Version( VOID );



/*@} --- end of fj_boot_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_boot_message
@{*/
/**
@def FJ_HM_CREATE
The message is sent only once at boot time.<br>
Host can init its variable, environment, etc.<br>
Since it is boot time - no API call is allowed, except for GPIO, PWM, Timer and Motor commands.<br>
Must wait for idle mode to issue all other commands.<br>
*/
/*@} --- end of fj_boot_message */
#endif	/* _FJ_BOOT_H */
