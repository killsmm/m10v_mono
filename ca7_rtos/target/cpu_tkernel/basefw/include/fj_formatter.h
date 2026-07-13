/**
 * @file		fj_formatter.h
 * @brief		NETSEC API Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_FORMATTER_H
#define _FJ_FORMATTER_H

#include "ddim_typedef.h"
#include "fj_std.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_formatter_overview
@{
	This section explains the overview of TS offload function.
	At first, initialization flow (Figure 1) is needed before using TS offload function.<br>
	
	@image html fj_formatter_figure1.png Figure_1
	
	Process Image<br>
	1. User provides the PES/PAT/PMT/PCR data and Tx descriptor parameter. (->TS Offload Process)<br>
	2. Driver writes Tx descriptor.<br>
	3. Hardware outputs results.<br>
	4. User provides a result buffer. Driver puts a result to that buffer. (->TS Offload Getting Result)<br>
	
	@image html fj_formatter_figure2.png Figure_2
	
	Output Data Control<br>
	Hardware makes TS packets from input data and stores them in a result buffer.
	The number of TS packets in a result buffer is limited by Payload Length,
	which is a parameter in Session DB (->Setting Session DB). Hardware doesn't fragment a TS packet.
	When Hardware stores the last TS packet made from input data and there is still space for one or more
	TS packets (i.e. 188 bytes or more) in a result buffer, Tx descriptor parameter cr (->TS Offload Process)
	determines whether Hardware waits for the following data or not.<br>
	
	If Tx descriptor parameter cr is set to 0 like "Input 1, 2 and 4" in Figure 3,
	Hardware waits for the following data to fill a result buffer with maximum number of
	TS packets whose total size does not exceed Payload Length.
	If cr is set to 1 like "Input 3 and 5" in Figure 3, Hardware does not wait for the following data
	but return a result buffer even if there is space for one or more TS packets in it.<br>
	(Note) cr=0 is not supported<br>
	
	@image html fj_formatter_figure3.png Figure_3
	
	PAT/PMT/PCR Input Data<br>
	As shown in Figure 4, if User wants to get PAT/PMT TS packets, User should provide PAT/PMT data.
	Hardware makes PAT/PMT TS packets by adding TS header, adaptation field, stuffing byte,
	and pointer field to them.<br>
	
	@image html fj_formatter_figure4.png Figure_4
	
	As shown in Figure 5, if User wants to get a PCR TS packet, User should provide 6 bytes of PCR data
	including reserved field. Hardware makes a PCR TS packet by adding TS header, adaptation field and
	stuffing byte to it.<br>
	
	@image html fj_formatter_figure5.png Figure_5
	
	
	
	
@}*//* --- end of fj_formatter_overview */


///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_formatter_sequence
@{
	- Netsec
	-# @ref fj_formatter_seq_1
	-# @ref fj_formatter_seq_2

	<hr>
	@section fj_formatter_seq_1 Formatter (PES->TS Convert 2session)
	@image html fj_formatter_2session.png

	@section fj_formatter_seq_2 Formatter (PES->TS Convert 1session)
	@image html fj_formatter_1session.png

@}*//* --- end of fj_formatter_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_formatter_definition
@{*/

/* stid */
#define FJ_FORMATTER_MPTS_STRM			16	/**< Stream. */
#define FJ_FORMATTER_MPTS_PG_0			17	/**< Program(0). */
#define FJ_FORMATTER_MPTS_PG_1			18	/**< Program(1). */
#define FJ_FORMATTER_MPTS_PG_2			19	/**< Program(1). */

/* dtid */
#define FJ_FORMATTER_PAT				1	/**< PAT(Program Association Table). */
//#define FJ_FORMATTER_PSI0				4	/**< OTHER PSI */
//#define FJ_FORMATTER_PSI1				5	/**< OTHER PSI */
//#define FJ_FORMATTER_PSI2				6	/**< OTHER PSI */
//#define FJ_FORMATTER_PSI3				7	/**< OTHER PSI */
#define FJ_FORMATTER_PMT				2	/**< PMT(Program Map Table). */
#define FJ_FORMATTER_PCR				3	/**< PCR(Program Clock Reference). */
#define FJ_FORMATTER_VDO				4	/**< Video PES. */
#define FJ_FORMATTER_ADO				5	/**< Audio PES. */
#define FJ_FORMATTER_MTD				6	/**< MetaData.  */

/* Last TS Indicater */
#define FJ_FORMATTER_LTI_END			1	/**< Last TS paket. */
#define FJ_FORMATTER_LTI_CONTINUE		0	/**< Top/Middle TS packet. */

///////////////////////////////////////////////////////////////////////////////
// enumeration
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////
/** Video/Audio Pid */
typedef struct {
	ULONG				pid_video;				/**< PID for Video Stream. */
	ULONG				pid_audio;				/**< PID for Audio Stream. */
	ULONG				pid_mtdat;				/**< PID for Meta Data. */
	ULONG				pid_pmt;				/**< PID for PMT. */
	ULONG				pid_pcr;				/**< PID for PCR. */
} T_FJ_FORMATTER_PID;

/** PES/TS Size Information */
typedef struct {
	UINT32				addr;					/**< Address. */
	UINT32				size;					/**< Size. */
} T_FJ_FORMATTER_DATA;

/*@} --- end of fj_formatter_definition */

///////////////////////////////////////////////////////////////////////////////
// macro
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_formatter_api
@{*/

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API initializes F_NETSEC_TS Driver/Hardware.
And this request sets Session Basic DB to F_NETSEC_TS Hardware Register.

 */
extern	VOID	FJ_Formatter_Convert_Initialize( VOID );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This request sets a Stream Session DB to F_NETSEC_TS Hardware register.

@param[in]	stid				Set FJ_FORMATTER_MPTS_STRM for Stream.
@param[in]	max_out_len			Maximum data size of output.
@param[in]	pid					Not used
@retval		FJ_ERR_OK				: Normal End
 */
extern	FJ_ERR_CODE	FJ_Formatter_Convert_Set_Stream( UINT32 stid, UINT32 max_out_len, T_FJ_FORMATTER_PID* pid );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This request sets a Program Session DB to F_NETSEC_TS Hardware register.

@param[in]	stid				Set FJ_FORMATTER_MPTS_PG_0 for Program(0).
								Set FJ_FORMATTER_MPTS_PG_1 for Program(1).
								Set FJ_FORMATTER_MPTS_PG_2 for Program(2).
@param[in]	max_out_len			Maximum data size of output.
@param[in]	pid					Set PID for Stream Video Data/Audio Data/PMT/PCR/Meta Data.
@retval		FJ_ERR_OK				: Normal End
@retval		FJ_ERR_NG_INPUT_PARAM	: stid Parameter error
 */
extern	FJ_ERR_CODE	FJ_Formatter_Convert_Set_Program( UINT32 stid, UINT32 max_out_len, T_FJ_FORMATTER_PID* pid );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This Request delivers PES/PAT/PMT/PCR/Meta data buffer address and Descriptor parameter from User to Driver.
And Hardware processes the data in that buffer. Another request is needed to get the result.
This request works as blocking function and waits for the end of Hardware’s referring to the buffer.
(It does not wait for all of the processes.) After this request returned, User can release the data buffer.

@param[in]	stid				Set FJ_FORMATTER_MPTS_PG_0 for Program(0).
								Set FJ_FORMATTER_MPTS_PG_1 for Program(1).
								Set FJ_FORMATTER_MPTS_PG_2 for Program(2).
@param[in]	dtid				Set FJ_FORMATTER_PMT for PMT. Set FJ_FORMATTER_PCR for PCR. Set FJ_FORMATTER_VDO for Video Pes. Set FJ_FORMATTER_ADO for Audio Pes. Set FJ_FORMATTER_MTD for Meta Data.
@param[in]	in_data				Set PES Data Information.
@param[in]	wait_time			Set timeout value (ms).
@retval		FJ_ERR_OK				: Normal End
@retval		FJ_ERR_NG				: Abnormal End
@retval		FJ_ERR_NG_TMOUT			: Send Timeout End
 */
extern	FJ_ERR_CODE	FJ_Formatter_Convert_PES_Send0( UINT32 stid, UINT32 dtid, T_FJ_FORMATTER_DATA* in_data, ULONG wait_time );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This Request delivers PES/PAT/PMT/PCR/Meta data buffer address and Descriptor parameter from User to Driver.
And Hardware processes the data in that buffer. Another request is needed to get the result.
This request works as blocking function and waits for the end of Hardware’s referring to the buffer.
(It does not wait for all of the processes.) After this request returned, User can release the data buffer.

@param[in]	stid				Set FJ_FORMATTER_MPTS_PG_0 for Program(0).
								Set FJ_FORMATTER_MPTS_PG_1 for Program(1).
								Set FJ_FORMATTER_MPTS_PG_2 for Program(2).
@param[in]	dtid				Set FJ_FORMATTER_PMT for PMT. Set FJ_FORMATTER_PCR for PCR. Set FJ_FORMATTER_VDO for Video Pes. Set FJ_FORMATTER_ADO for Audio Pes. Set FJ_FORMATTER_MTD for Meta Data.
@param[in]	in_data				Set PES Data Information.
@param[in]	wait_time			Set timeout value (ms).
@retval		FJ_ERR_OK				: Normal End
@retval		FJ_ERR_NG				: Abnormal End
@retval		FJ_ERR_NG_TMOUT			: Send Timeout End
 */
extern	FJ_ERR_CODE	FJ_Formatter_Convert_PES_Send1( UINT32 stid, UINT32 dtid, T_FJ_FORMATTER_DATA* in_data, ULONG wait_time );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This request delivers a buffer for result from User to Driver. And Driver returns output to the buffer.
This Request works as blocking function. If there is no result, it waits for a result.
The size of the buffer should be equal to Payload Length in session DB.

@param[in]	out_data			Set the result buffer’s address/size.
@param[out]	out_len				This field stores the size of output data. The value of this field is multiple of 188.
@param[out]	dtid				Set FJ_FORMATTER_PMT for PMT. Set FJ_FORMATTER_PCR for PCR. Set FJ_FORMATTER_VDO for Video Pes. Set FJ_FORMATTER_ADO for Audio Pes. Set FJ_FORMATTER_MTD for Meta Data.
@param[out]	stid				This field stores STID of the last TS packet in the result buffer.
@param[out]	lti					The Last TS Indicater. 1: This result shows the result buffer includes the last TS packet made from input PES. 0: This result shows the result buffer does not include the last TS packet made from input PES.
@param[in]	wait_time			Set timeout value (ms).
@retval		FJ_ERR_OK				: Normal End
@retval		FJ_ERR_NG				: Abnormal End
@retval		FJ_ERR_NG_TMOUT			: Recv Timeout End
 */
extern	FJ_ERR_CODE	FJ_Formatter_Convert_TS_Recv0( T_FJ_FORMATTER_DATA* out_data, ULONG* out_len, UINT32* stid, UINT32* dtid, UINT32* lti, ULONG wait_time );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Video(0) / Audio(0) TS packet recv.
This request delivers a buffer for result from User to Driver. And Driver returns output to the buffer.
This Request works as blocking function. If there is no result, it waits for a result.
The size of the buffer should be equal to Payload Length in session DB.

@param[in]	out_data			Set the result buffer’s address/size.
@param[out]	out_len				This field stores the size of output data. The value of this field is multiple of 188.
@param[out]	dtid				Set FJ_FORMATTER_PMT for PMT. Set FJ_FORMATTER_PCR for PCR. Set FJ_FORMATTER_VDO for Video Pes. Set FJ_FORMATTER_ADO for Audio Pes. Set FJ_FORMATTER_MTD for Meta Data.
@param[out]	stid				This field stores STID of the last TS packet in the result buffer.
@param[out]	lti					The Last TS Indicater. 1: This result shows the result buffer includes the last TS packet made from input PES. 0: This result shows the result buffer does not include the last TS packet made from input PES.
@param[in]	wait_time			Set timeout value (ms).
@retval		FJ_ERR_OK				: Normal End
@retval		FJ_ERR_NG				: Abnormal End
@retval		FJ_ERR_NG_TMOUT			: Recv Timeout End
 */
extern	FJ_ERR_CODE	FJ_Formatter_Convert_TS_Recv1( T_FJ_FORMATTER_DATA* out_data, ULONG* out_len, UINT32* stid, UINT32* dtid, UINT32* lti, ULONG wait_time );

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Video(1) / Audio(1) TS packet recv.
This request delivers a buffer for result from User to Driver. And Driver returns output to the buffer.
This Request works as blocking function. If there is no result, it waits for a result.
The size of the buffer should be equal to Payload Length in session DB.

 */
extern	VOID	FJ_Formatter_Convert_Finalize( VOID );

/*@} --- end of fj_formatter_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
//@cond
/** @weakgroup fj_formatter_message
@{*/
/*@} --- end of fj_formatter_message */
//@endcond

///////////////////////////////////////////////////////////////////////////////
// callback
///////////////////////////////////////////////////////////////////////////////
//@cond
/** @weakgroup fj_formatter_callback
@{*/
/*@} --- end of fj_formatter_callback */
//@endcond
#endif
