/**
 * @file		dd_relc.h
 * @brief		RELC macro driver header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __DD_RELC_H__
#define __DD_RELC_H__

#include "driver_common.h"
#include "ddim_typedef.h"

/**
@addtogroup dd_relc
@{
@} */

/*----------------------------------------------------------------------*/
/* Overview																*/
/*----------------------------------------------------------------------*/
/** @weakgroup dd_relc
@{
	RELC macro decodes the data encoded in encoder RELC.<br>
	There are two patterns how to run in RELC macro.<br>
	And each patterns have how to designate address.<br>

	Normal mode<br>
		Register mode 	: To run only once by designated an input and output address.<br>
		Descriptor mode : To run only once by a descriptor.<br><br>

	Continuous run mode<br>
		Register mode 	: To run continuously by auto-increment input and output address.<br>
		Descriptor mode : To run continuously by a descriptor.

@} *//* --- end of relc_driver_overview */

/*----------------------------------------------------------------------*/
/* Sequence																*/
/*----------------------------------------------------------------------*/
/** @weakgroup dd_relc
@{
	- @ref Init_and_Start_run
	- @ref Proc_result_comp
	- @ref Proc_result_err
	- @ref Proc_result_reach_addr
	<hr>
	@section Init_and_Start_run Initialization and execution start
	@image html 01_RELC_init_start.png
	<br>
	<br>
	@section Proc_result_comp Result determination processing(processing is complete the whole pprocess)
	@image html 02_RELC_judge_result_Finish.png
	<br>
	<br>
	@section Proc_result_err Result determination processing(error is occured)
	@image html 03_RELC_judge_result_Error.png
	<br>
	<br>
	@section Proc_result_reach_addr Result determination processing(when it reaches the specified)
	@image html 04_RELC_judge_result_Reach_address.png

@} *//* --- end of relc_driver_sequence */

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/** @weakgroup dd_relc
@{*/
/* return value of RELC decoding */
#define	D_DD_RELC_OK				(0) 	/**< Normal end					*/
#define D_DD_RELC_PARAM_ERR			(1)		/**< Input Parameter Error		*/
#define D_DD_RELC_EXT_LOCK_NG		(2)		/**< Exclusive control NG 		*/
#define D_DD_RELC_TIMEOUT			(3)		/**< Wait flag timeout			*/
#define D_DD_RELC_FLG_CLR_NG		(4)		/**< Flag clear NG				*/

/* Descriptor mode */
#define D_DD_RELC_RUN_MODE_NORMAL	(0)		/**< Normal mode(register)		*/
#define D_DD_RELC_RUN_MODE_DESC		(1)		/**< Descriptor mode			*/

/* Continuous Run Mode */
#define D_DD_RELC_CONT_MODE_NORMAL	(0)		/**< Normal mode(once)			*/
#define D_DD_RELC_CONT_MODE_CONT	(1)		/**< Continuous Mode			*/

/* RELC designated address */
#define D_DD_RELC_ADDR_DISABLE		(0)		/**< Designated address disable	*/
#define D_DD_RELC_ADDR_ENABLE		(1)		/**< Designated address enable 	*/

/* RELC addres increment */
#define D_DD_RELC_ADDR_INCRE_OFF	(0)		/**< Address auto increment off	*/
#define D_DD_RELC_ADDR_INCRE_ON		(1)		/**< Address auto increment on 	*/

/* interrupt flag */
#define D_DD_RELC_INT_END_FLG		(0x00000001)	/**< RELC interrupt end flag( processing is terminated )	*/
#define D_DD_RELC_INT_ERR_FLG		(0x00000002)	/**< RELC interrupt error flag( processing is abnormaly )	*/
#define D_DD_RELC_INT_SLP_FLG		(0x00000004)	/**< RELC interrupt sleep flag( processing is abnormaly )	*/

/* Interrupt type */
#define D_DD_RELC_ERR				(1)		/**< Error interrupt					*/
#define D_DD_RELC_SLEEP				(2)		/**< Sleep interrupt					*/
#define D_DD_RELC_FINISH			(3)		/**< Finish interrupt					*/
#define D_DD_RELC_CR_FINISH			(4)		/**< Continuous run finish interrupt	*/

/* error number */
#define D_DD_RELC_ERR_INPUT_DATA_1			(0x0001)	/**< Input data error				*/
#define D_DD_RELC_ERR_INPUT_DATA_2			(0x0002)	/**< Input data error				*/
#define D_DD_RELC_ERR_INPUT_DATA_3			(0x0003)	/**< Input data error				*/
#define D_DD_RELC_ERR_INPUT_DATA_4			(0x0004)	/**< Input data error				*/
#define D_DD_RELC_ERR_BUS_INPUT				(0x0010)	/**< Bus error(input)				*/
#define D_DD_RELC_ERR_BUS_OUTPUT			(0x0020)	/**< Bus error(output)				*/
#define D_DD_RELC_ERR_ADDR_MISMATCH_INPUT	(0x1000)	/**< Address mismatch error(input)	*/
#define D_DD_RELC_ERR_ADDR_MISMATCH_OUTPUT	(0x2000)	/**< Address mismatch error(output)	*/

/* type define */
#define VOID	void						/**< Type is defined to VOID	*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Typedef																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** RELC control common table */
typedef struct {
	UCHAR		desc_mode;			/**< Descriptor Mode							*/
	UCHAR		cont_run_mode;		/**< Continuous Run Mode						*/
	ULONG		seq_num;			/**< Sequential Run Number of Times				*/
	UCHAR		write_hprot;		/**< RELC data write side HPROT (default:0x0B)	*/
	UCHAR		read_hprot;			/**< RELC data read side HPROT (default:0x0B)	*/
	VP_CALLBACK	callback;			/**< Callback function pointer					*/
} T_DD_RELC_CTRL_CMN;

/** RELC control register table */
typedef struct {
	UCHAR	in_end_addr_en;			/**< Input End Address Enable		*/
	UCHAR	out_end_addr_en;		/**< Output End Address Enable		*/
	UCHAR	in_addr_auto_inc;		/**< Input Address Auto Increment	*/
	UCHAR	out_addr_auto_inc;		/**< Output Address Auto Increment	*/
	ULONG	in_start_addr;			/**< Input start address			*/
	ULONG	in_end_addr;			/**< Input end address				*/
	ULONG	out_start_addr;			/**< Output start address			*/
	ULONG	out_end_addr;			/**< Output end address				*/
} T_DD_RELC_CTRL_REG;

/** RELC control descriptor table */
typedef struct {
	ULONG	descriptor_addr; 		/**< Descriptor address	*/
} T_DD_RELC_CTRL_DESC;

/** RELC decode information */
typedef struct {
	USHORT	read_byte;				/**< Number of bytes read						*/
	USHORT	write_byte;				/**< Number of bytes written					*/
	USHORT	proc_block;				/**< Number of processing blocks				*/
	ULONG	dec_total_read_bytes;	/**< Decode the total number of bytes to read	*/
	ULONG	dec_total_write_bytes;	/**< Decode the total number of bytes to write	*/
	ULONG	seq_cnt;				/**< Sequential run counter						*/
} T_DD_RELC_DEC_INFO;

/** RELC control mode2 for descriptor */
typedef union {
	ULONG		word;				/**< for ULONG access				*/
	struct {
		ULONG	imax	:1;			/**< Input end address enable		*/
		ULONG	omax	:1;			/**< Output end address enable		*/
		ULONG	iaai	:1;			/**< Input Address Auto Increment	*/
		ULONG	oaai	:1;			/**< Output Address Auto Increment	*/
		ULONG			:28;		/**< reserved						*/
	} bit;							/**< bit field						*/
} U_DD_RELC_SET_MODE2;

/** Type is defined to Callback function pointer for RELC */
typedef void (*RELK_CALLBACK)( UINT32 int_factor );

/** RELC control struct for descriptor */
typedef struct {
	U_DD_RELC_SET_MODE2	relc_mode_2;/**< Mode setting 2			*/
//	USHORT	reserved1;				/**< Reserved 1				*/
	ULONG	in_start_addr;   	    /**< Input start address	*/
	ULONG	in_end_addr;			/**< Input end address		*/
	ULONG	out_start_addr;			/**< Output start address	*/
	ULONG	out_end_addr;			/**< Output end address		*/
	ULONG	reserved2;				/**< Reserved 2				*/
	ULONG	reserved3;				/**< Reserved 3				*/
	ULONG	reserved4;				/**< Reserved 4				*/
} T_DD_RELC_SET_DESC;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/** RELC control struct for normal mode*/
typedef struct {
	UCHAR		write_hprot;		/**< RELC data write side HPROT (default:0x0B)	*/
	UCHAR		read_hprot;			/**< RELC data read side HPROT (default:0x0B)	*/
	ULONG		in_start_addr;		/**< Input start address						*/
//	ULONG		in_data_size;		/**< Input data size							*/
	ULONG		out_start_addr;		/**< Output start address						*/
//	ULONG		out_data_size;		/**< Output data size							*/
	ULONG		seq_num;			/**< Sequential Run Number of Times				*/
	VP_CALLBACK	callback;			/**< Callback function pointer					*/
} T_DD_RELC_SET_MOD_NORMAL;

/** RELC control struct for descriptor mode */
typedef struct{
	UCHAR		write_hprot;			/**< RELC data write side HPROT (default:0x0B)	*/
	UCHAR		read_hprot;				/**< RELC data read side HPROT (default:0x0B)	*/
	ULONG		relc_descriptor_addr;	/**< descriptor address							*/
	ULONG		seq_num;				/**< Sequential Run Number of Times				*/
	VP_CALLBACK	callback;				/**< Callback function pointer					*/
} T_DD_RELC_SET_MOD_DESC;

#endif	/* CO_DDIM_UTILITY_USE */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

//---------------------------- driver section ----------------------------

/**
RELC initialize.
@param[in] write_hprot			RELC data write side HPROT (default:0x0B)
@param[in] read_hprot			RELC data read side HPROT (default:0x0B)
@remarks This function performs interrupt permission and soft reset.
*/
extern	VOID Dd_RELC_Init( UCHAR write_hprot, UCHAR read_hprot );

/**
RELC is exclusively controlled.
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_EXT_LOCK_NG		Lock Error (System Using RELC)
@retval other	Fail.<br>
@remarks If RELC is unused, exclusive control OK is sent back as (Lock) while using it.<br>
         Exclusive control NG is sent back when using it (Lock inside of present).
*/
extern	INT32 Dd_RELC_Open( VOID );

/**
RELC is exclusively closed.
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_EXT_LOCK_NG		Lock Error (System Using RELC)
@remarks This API uses DDIM_User_Sig_Sem().
*/
extern	INT32 Dd_RELC_Close( VOID );

/**
RELC decode is started.(Sync)
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_FLG_CLR_NG		Clear flag is NG
@retval D_DD_RELC_TIMEOUT			twai_flg is time out
@remarks This function will wait until the end of the decoding process.
@remarks This API uses DDIM_User_Clr_Flg().
@remarks This API uses DDIM_User_Twai_Flg().
*/
extern	INT32 Dd_RELC_Start_Sync( VOID );

/**
RELC decode is started.(Async)
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_FLG_CLR_NG		Clear flag is NG
@remarks The RELC is started by this function, and is waited by RELC_Wait_End().
@remarks This API uses DDIM_User_Clr_Flg().
*/
extern	INT32 Dd_RELC_Start_Async( VOID );

/**
Wait for the completion of the RELC decode.
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_TIMEOUT			twai_flg is time out
@remarks This function is used when RELC is started by RELC_Start_Async().
@remarks This API uses DDIM_User_Twai_Flg().
*/
extern	INT32 Dd_RELC_Wait_End( VOID );

/**
RELC decode is stopped.(both Sync and Async)
*/
extern	VOID Dd_RELC_Stop( VOID );

/**
The common control setting for RELC is set.
@param[in] relc_ctrl_cmn			RELC common control parameter
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
@remarks This function can be set below.<br>
			Descriptor mode 				: 0(normal/1(descriptor)<br>
			Continuous run mode 			: 0(normal)/1(continuous run)<br>
			Sequential run number of times	: When 0 or 1 is specified, a value is 1.<br>
			Callback function pointer 		: To define the function you want to call at interrupt time.
*/
extern	INT32 Dd_RELC_Ctrl_Common( T_DD_RELC_CTRL_CMN const* const relc_ctrl_cmn );

/**
The register control setting for RELC is set.
@param[in] relc_ctrl_reg			RELC register control parameter
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
@remarks This function can be set below.<br>
			Input end address enable 		: 0(disable)/1(enable)<br>
			Output end address enable 		: 0(disable)/1(enable)<br>
			Input address auto increment	: 0(disable)/1(enable)<br>
			Output address auto increment	: 0(disable)/1(enable)<br>
			Input start address<br>
			Input end address<br>
			Output start address<br>
			Output end address
*/
extern	INT32 Dd_RELC_Ctrl_Register( T_DD_RELC_CTRL_REG const* const relc_ctrl_reg );

/**
The descriptor control setting for RELC is set.
@param[in] relc_ctrl_desc			RELC descriptor control parameter
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
@remarks This function can be set below.<br>
			descriptor address
*/
extern	INT32 Dd_RELC_Ctrl_Descriptor( T_DD_RELC_CTRL_DESC const* const relc_ctrl_desc );

/**
The input start address for RELC is set.
@param[in] in_start_addr			Input start address
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
@remarks If you set during the operation of the SLEEP, without having to reset <br>
		 the Resume is not guaranteed.
*/
extern	INT32 Dd_RELC_Set_In_Start_Addr( ULONG in_start_addr );

/**
The input end address for RELC is set.
@param[in] in_end_addr				Input end address
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
@remarks To enable this designation should be set relc_mode2[0](IMAX) to 1.
*/
extern	INT32 Dd_RELC_Set_In_End_Addr( ULONG in_end_addr );

/**
The output start address for RELC is set.
@param[in] out_start_addr			Output start address
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
@remarks If you set during the operation of the SLEEP, without having to reset <br>
		 the Resume is not guaranteed.
*/
extern	INT32 Dd_RELC_Set_Out_Start_Addr( ULONG out_start_addr );

/**
The output end address for RELC is set.
@param[in] out_end_addr				Output end address
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
@remarks To enable this designation should be set relc_mode2[1](OMAX) to 1.
*/
extern	INT32 Dd_RELC_Set_Out_End_Addr( ULONG out_end_addr );

/**
The common control setting for RELC is get.
@param[out] relc_ctrl_cmn			RELC common control parameter
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
@remarks This function can be get below.<br>
			Descriptor mode 				: 0(normal/1(descriptor)<br>
			Continuous run mode 			: 0(normal)/1(continuous run)<br>
			Sequential run number of Times	: When 0 or 1 is specified, a value is 1.<br>
			Callback function pointer 		: To define the function you want to call at interrupt time.
*/
extern	INT32 Dd_RELC_Get_Ctrl_Common( T_DD_RELC_CTRL_CMN* const relc_ctrl_cmn );

/**
The register control setting for RELC is get.
@param[out] relc_ctrl_reg			RELC register control parameter
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
@remarks This function can be get below.<br>
			Input end address enable 		: 0(disable)/1(enable)<br>
			Output end address enable 		: 0(disable)/1(enable)<br>
			Input address auto increment	: 0(disable)/1(enable)<br>
			Output address auto increment	: 0(disable)/1(enable)<br>
			Input start address<br>
			Input end address<br>
			Output start address<br>
			Output end address
*/
extern	INT32 Dd_RELC_Get_Ctrl_Register( T_DD_RELC_CTRL_REG* const relc_ctrl_reg );

/**
The descriptor control setting for RELC is get.
@param[out] relc_ctrl_desc			RELC descriptor control parameter
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
@remarks This function can be get below.<br>
			descriptor address
*/
extern	INT32 Dd_RELC_Get_Ctrl_Descriptor( T_DD_RELC_CTRL_DESC* const relc_ctrl_desc );

/**
It is Interrupt Handler of RELC.<br>
The interruption flag is cleared, and the CallBack function is called.<br>
@remarks This handler operates in the following cases.<br>
			RELC decode error is occured.<br>
			RELC decode is stopped.<br>
			Normal mode of RELC decode is finished.<br>
			Continuous run mode of RELC decode is finished.
@remarks	This API uses DDIM_User_Set_Flg().
*/
extern	VOID Dd_RELC_Int_Handler( VOID );

/**
RELC decode status is get.
@retval	RELC decode status
@remarks This function get the following conditions.<br>
			000b(IDLE)<br>
			001b(RUN)<br>
			010b(SLEEP)<br>
			100b(FINISH)
*/
extern	INT32 Dd_RELC_Get_Status( VOID );

/**
RELC sleep reason is get.
@retval	RELC sleep reason
@remarks This function is get the following sleeping reason.<br>
			000b(User instructions)<br>
			001b(Reaches the input end address)<br>
			010b(Reaches the output end address)
*/
extern	INT32 Dd_RELC_Get_Sleep_Reason( VOID );


/**
RELC error status is get.
@remarks This function is get the following error status.<br>
			0b(No error)<br>
			1b(Error is occured)
*/
extern	INT32 Dd_RELC_Get_Error_Status( VOID );

/**
RELC error number is get.
@retval D_DD_RELC_OK						OK
@retval D_DD_RELC_ERR_INPUT_DATA_x			Input data error
@retval D_DD_RELC_ERR_BUS_INPUT				Bus error(input)
@retval D_DD_RELC_ERR_BUS_OUTPUT			Bus error(output)
@retval D_DD_RELC_ERR_ADDR_MISMATCH_INPUT	Address mismatch error(input)
@retval D_DD_RELC_ERR_ADDR_MISMATCH_OUTPUT	Address mismatch error(output)
*/
extern	INT32 Dd_RELC_Get_Error( VOID );

/**
RELC status is get.
@param[out] dec_info				RELC processing status
@remarks This function is get the following processing status.<br>
			Number of bytes read<br>
			Number of bytes written<br>
			Number of processing blocks<br>
			Decode the total number of bytes to read<br>
			Decode the total number of bytes to write<br>
			Sequential run counter
*/
extern	INT32 Dd_RELC_Get_Process_Status( T_DD_RELC_DEC_INFO* dec_info );

/**
The input and reference data buffer size for RELC is get.
@param[out] in_buf_size				input buffer size
@param[out] ref_buf_size			reference buffer size
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
*/
extern	INT32 Dd_RELC_Get_Buf_Size( USHORT* in_buf_size, USHORT* ref_buf_size );

/**
Set the data to the input data buffer(mirror erea).
@param[in]	in_data_addr			input data address
@param[in]	in_data_size			input data size
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
*/
extern	INT32 Dd_RELC_Set_In_Buf_Data_Mirror( ULONG in_data_addr, UINT32 in_data_size );

/**
Get the data to the input data buffer(mirror erea).
@param[out]	in_data_addr			input data address
@param[out]	in_data_size			input data size
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
*/
extern	INT32 Dd_RELC_Get_In_Buf_Data_Mirror( ULONG in_data_addr, UINT32 in_data_size );

/**
Set the data to the input data buffer.
@param[in] in_data_addr				input data address
@param[in] in_data_size				input data size
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
*/
extern	INT32 Dd_RELC_Set_In_Buf_Data( ULONG in_data_addr, UINT32 in_data_size );

/**
Get the data to the input data buffer.
@param[out]	in_data_addr			input data address
@param[out]	in_data_size			input data size
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
*/
extern	INT32 Dd_RELC_Get_In_Buf_Data( ULONG in_data_addr, UINT32 in_data_size );

/**
Set the data to the reference data buffer.
@param[in]	out_data_addr			output data address
@param[in]	out_data_size			output data size
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
*/
extern	INT32 Dd_RELC_Set_Ref_Buf_Data( ULONG out_data_addr, UINT32 out_data_size );

/**
Get the data to the reference data buffer.
@param[out]	out_data_addr			output data address
@param[out]	out_data_size			output data size
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
*/
extern	INT32 Dd_RELC_Get_Ref_Buf_Data( ULONG out_data_addr, UINT32 out_data_size );



#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/**
Set and Start RELC normal mode
@param[in] relc_set_mod_normal		RELC setting for normal mode
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
@remarks This function can be set below.<br>
			Input start address
			Input data size
			Output start address
			Output data size
			Sequential Run Number of Times	: 0 or 1(normal)/2~(continuous number of executions)
*/
extern INT32 Dd_RELC_Utility_Register( T_DD_RELC_SET_MOD_NORMAL const* const relc_set_mod_normal );

/**
Set and Start RELC descriptor mode
@param[in] relc_set_mod_desc		RELC setting for descriptor mode
@retval D_DD_RELC_OK				OK
@retval D_DD_RELC_PARAM_ERR			Parameter error
@remarks This function can be set below.<br>
			descriptor address	
			Sequential Run Number of Times	: 0 or 1(normal)/2~(continuous number of executions)
*/
extern INT32 Dd_RELC_Utility_Descriptor( T_DD_RELC_SET_MOD_DESC const* const relc_set_mod_desc );



#endif	/* CO_DDIM_UTILITY_USE */

#ifdef __cplusplus
}
#endif

/*@} --- end of relc_driver_api */

/*----------------------------------------------------------------------*/
/* Message																*/
/*----------------------------------------------------------------------*/
/** @weakgroup dd_relc
@{*/

// Nothing Special

/*@} --- end of relc_driver_message */

#endif	// __DD_RELC_H__

/*@}*/
