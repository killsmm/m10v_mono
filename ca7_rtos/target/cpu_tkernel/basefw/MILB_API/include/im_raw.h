/**
 * @file		im_raw.h
 * @brief		Header file of RAW3A driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
	@addtogroup im_raw
	@{
*/

#ifndef _IM_RAW_H_
#define _IM_RAW_H_

//#include "ddim_typedef.h"
#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_IM_RAW_NUM_CATEGORY	(15)	/**< The number of category */


/**
	@name Returned value
	@{
*/
//	D_DDIM_OK is defined with driver_common.h
#define	D_IM_RAW_RETVAL_OK				(D_DDIM_OK)								/**< OK */
#define	D_IM_RAW_RETVAL_PARAM_ERROR		(D_IM_RAW | D_DDIM_INPUT_PARAM_ERROR)	/**< Parameter error	*/
#define	D_IM_RAW_RETVAL_SYSTEM_ERROR	(D_IM_RAW | D_DDIM_SYSTEM_ERROR)		/**< System call error	*/
#define	D_IM_RAW_RETVAL_MACRO_BUSY		(D_IM_RAW | D_DDIM_MACRO_BUSY_NG)		/**< Macro busy error	*/
#define	D_IM_RAW_RETVAL_FORCE_STOP		(D_IM_RAW | 0x0000FFFF)					/**< Force stop			*/
#define	D_IM_RAW_RETVAL_R_AXI_ERR		(D_IM_RAW | D_DDIM_R_AXI_ERROR)			/**< Read AXI error		*/
#define	D_IM_RAW_RETVAL_W_AXI_ERR		(D_IM_RAW | D_DDIM_W_AXI_ERROR)			/**< Write AXI error	*/
#define	D_IM_RAW_RETVAL_TIME_OUT		(D_IM_RAW | D_DDIM_TIMEOUT)				/**< Time Out			*/
/*@}*/	// name

#define D_IM_RAW_OFF					(0)		/**< OFF */
#define D_IM_RAW_ON						(1)		/**< ON */

#define D_IM_RAW_RFMT_14_OR_16_BIT		(0)		/**< 14 or 16bits (RAW data) If select 16bit, necessarily set bit_select. */
#define D_IM_RAW_RFMT_12_BIT_PACK		(1)		/**< 12bit pack */
#define D_IM_RAW_RFMT_8_BIT				(2)		/**< 8bits */

#define D_IM_RAW_BITSEL_EXCEPT_16_BIT	(0)		/**< 14/12/8 bits (RAW data) */
#define D_IM_RAW_BITSEL_16_BIT			(1)		/**< 16bits (RAW data) */

#define D_IM_RAW_DEFFC_MCU_2x2			(0)		/**< MCU is 2x2. */
#define D_IM_RAW_DEFFC_MCU_2x1			(1)		/**< MCU is 2x1. */

#define D_IM_RAW_DEFOP_DIFF_PREV_VALUE	(0)		/**< Target pixel value - previous pixel value  [RCTL1.DEFOP = 0b] */
#define D_IM_RAW_DEFOP_DIFF_FIXED_VALUE	(1)		/**< Target pixel value - fixed value (DEFINIT) [RCTL1.DEFOP = 1b] */

#define D_IM_RAW_COND_OK				(0)		/**< Encode/Decode OK. */
#define D_IM_RAW_COND_ENC_LIMIT_OVER	(1)		/**< Encode Error: Over code limitation. */
#define D_IM_RAW_COND_DEC_UNDEF_CODE	(2)		/**< Decode Error: Detect undefined code. */
#define D_IM_RAW_COND_DEC_ERROR			(3)		/**< Decode Error: General error. */

#define D_IM_RAW_TBLAEN_OFF				(1)		/**< Raw macro OFF */
#define D_IM_RAW_TBLAEN_ON				(0)		/**< Raw macro ON */

#define D_IM_RAW_R_AXI_ERROR			(4)		/**< Read AXI Error.(Callback return value) */
#define D_IM_RAW_W_AXI_ERROR			(5)		/**< Write AXI Error.(Callback return value) */

#define D_IM_RAW_NON_CACHE_NON_BUF				(0)		/**< Cache Support: Non Cacheable & Non Bufferable */
#define D_IM_RAW_ON_BUF							(1)		/**< Cache Support: Bufferable */
#define D_IM_RAW_ON_CACHE_NO_ALLOCATE			(2)		/**< Cache Support: Cacheable & No Allocate */
#define D_IM_RAW_ON_CACHE_ON_BUF_NO_ALLOCATE	(3)		/**< Cache Support: Cacheable & Bufferable & No Allocate */
#define D_IM_RAW_ON_CACHE_R_WRITE_THROUGH		(6)		/**< Cache Support: Cacheable & Write Through(Allocate Read only) */
#define D_IM_RAW_ON_CACHE_R_WRITE_BACK			(7)		/**< Cache Support: Cacheable & Write Back(Allocate Read only) */
#define D_IM_RAW_ON_CACHE_W_WRITE_THROUGH		(10)	/**< Cache Support: Cacheable & Write Through(Allocate Write only) */
#define D_IM_RAW_ON_CACHE_W_WRITE_BACK			(11)	/**< Cache Support: Cacheable & Write Back(Allocate Write only) */
#define D_IM_RAW_ON_CACHE_RW_WRITE_THROUGH		(14)	/**< Cache Support: Cacheable & Write Through(Allocate Read & Write) */
#define D_IM_RAW_ON_CACHE_RW_WRITE_BACK			(15)	/**< Cache Support: Cacheable & Write Back(Allocate Read & Write) */

#define D_IM_RAW_D_SEC_NORMAL_ACCESS			(0)		/**< Cache Protect: Data       /Secure    /Normal     Access */
#define D_IM_RAW_D_SEC_PRIVILEGED_ACCESS		(1)		/**< Cache Protect: Data       /Secure    /Privileged Access */
#define D_IM_RAW_D_NSEC_NORMAL_ACCESS			(2)		/**< Cache Protect: Data       /Non-Secure/Normal     Access */
#define D_IM_RAW_D_NSEC_PRIVILEGED_ACCESS		(3)		/**< Cache Protect: Data       /Non-Secure/Privileged Access */
#define D_IM_RAW_I_SEC_NORMAL_ACCESS			(4)		/**< Cache Protect: Instruction/Secure    /Normal     Access */
#define D_IM_RAW_I_SEC_PRIVILEGED_ACCESS		(5)		/**< Cache Protect: Instruction/Secure    /Privileged Access */
#define D_IM_RAW_I_NSEC_NORMAL_ACCESS			(6)		/**< Cache Protect: Instruction/Non-Secure/Normal     Access */
#define D_IM_RAW_I_NSEC_PRIVILEGED_ACCESS		(7)		/**< Cache Protect: Instruction/Non-Secure/Privileged Access */



/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/** Code Table */
typedef struct {
	USHORT	code[D_IM_RAW_NUM_CATEGORY];			/**< CODEx (x = 0x0, 0x1, ... 0xE) */
	UCHAR	code_length[D_IM_RAW_NUM_CATEGORY];		/**< LENx  (x = 0x0, 0x1, ... 0xE) */
} T_IM_RAW_CTRL_CODE_TBL;


/** Im_RAW_Ctrl_Axi() parameter structure */
typedef struct {
	UINT32	r_cache_type;			/**< Read ch cache type<br>
											<ul>
												<li>@ref D_IM_RAW_NON_CACHE_NON_BUF
												<li>@ref D_IM_RAW_ON_BUF
												<li>@ref D_IM_RAW_ON_CACHE_NO_ALLOCATE
												<li>@ref D_IM_RAW_ON_CACHE_ON_BUF_NO_ALLOCATE
												<li>@ref D_IM_RAW_ON_CACHE_R_WRITE_THROUGH
												<li>@ref D_IM_RAW_ON_CACHE_R_WRITE_BACK
												<li>@ref D_IM_RAW_ON_CACHE_W_WRITE_THROUGH
												<li>@ref D_IM_RAW_ON_CACHE_W_WRITE_BACK
												<li>@ref D_IM_RAW_ON_CACHE_RW_WRITE_THROUGH
												<li>@ref D_IM_RAW_ON_CACHE_RW_WRITE_BACK
											</ul>
									 */
	UINT32	r_protection_type;		/**< Read ch protect type <br>
											<ul>
												<li>@ref D_IM_RAW_D_SEC_NORMAL_ACCESS
												<li>@ref D_IM_RAW_D_SEC_PRIVILEGED_ACCESS
												<li>@ref D_IM_RAW_D_NSEC_NORMAL_ACCESS
												<li>@ref D_IM_RAW_D_NSEC_PRIVILEGED_ACCESS
												<li>@ref D_IM_RAW_I_SEC_NORMAL_ACCESS
												<li>@ref D_IM_RAW_I_SEC_PRIVILEGED_ACCESS
												<li>@ref D_IM_RAW_I_NSEC_NORMAL_ACCESS
												<li>@ref D_IM_RAW_I_NSEC_PRIVILEGED_ACCESS
											</ul>
									 */
	UINT32	w_cache_type;			/**< Write ch cache type<br>
											<ul>
												<li>@ref D_IM_RAW_NON_CACHE_NON_BUF
												<li>@ref D_IM_RAW_ON_BUF
												<li>@ref D_IM_RAW_ON_CACHE_NO_ALLOCATE
												<li>@ref D_IM_RAW_ON_CACHE_ON_BUF_NO_ALLOCATE
												<li>@ref D_IM_RAW_ON_CACHE_R_WRITE_THROUGH
												<li>@ref D_IM_RAW_ON_CACHE_R_WRITE_BACK
												<li>@ref D_IM_RAW_ON_CACHE_W_WRITE_THROUGH
												<li>@ref D_IM_RAW_ON_CACHE_W_WRITE_BACK
												<li>@ref D_IM_RAW_ON_CACHE_RW_WRITE_THROUGH
												<li>@ref D_IM_RAW_ON_CACHE_RW_WRITE_BACK
											</ul>
									 */
	UINT32	w_protection_type;		/**< Write ch protect type <br>
											<ul>
												<li>@ref D_IM_RAW_D_SEC_NORMAL_ACCESS
												<li>@ref D_IM_RAW_D_SEC_PRIVILEGED_ACCESS
												<li>@ref D_IM_RAW_D_NSEC_NORMAL_ACCESS
												<li>@ref D_IM_RAW_D_NSEC_PRIVILEGED_ACCESS
												<li>@ref D_IM_RAW_I_SEC_NORMAL_ACCESS
												<li>@ref D_IM_RAW_I_SEC_PRIVILEGED_ACCESS
												<li>@ref D_IM_RAW_I_NSEC_NORMAL_ACCESS
												<li>@ref D_IM_RAW_I_NSEC_PRIVILEGED_ACCESS
											</ul>
									 */
} T_IM_RAW_AXI;

/** Im_RAW_Get_AXI_Status() parameter structure */
typedef struct {
	UCHAR	r_ch_resp;		/**< Read Channel Response */
	UCHAR	w_ch_resp;		/**< Write Channel Response */
} T_IM_RAW_AXI_STATUS;

/** Parameters of Im_RAW_Ctrl_Enc() and Im_RAW_Ctrl_Dec() */
typedef struct {
	
	// Common (16/14/12/8 bit)
	UINT32		src_addr;				/**< MSA.<br>
											 Source address.<br>
											 Should be aligned on 16.
										 */
	UINT32		dst_addr;				/**< MDA.<br>
											 Destination address.<br>
											 Should be aligned on 16.
										 */
	UINT32		diff_fixed_value;		/**< DEFINIT.<br>
										   	 Fixed value of subtraction. [0~16383].
										 */
	UCHAR		byte_stuffing;			/**< RCTL1.BYTS<br>
											 Byte stuffing.<br>
											 <ul>
												<li>@ref D_IM_RAW_OFF		Byte stuffing OFF [RCTL1.BYTS = 0b]
												<li>@ref D_IM_RAW_ON		Byte stuffing ON  [RCTL1.BYTS = 1b]
											 </ul>
										 */
	UCHAR		data_format;			/**< RCTL1.RFMT<br>
											 Data format<br>
											 <ul>
												<li>@ref D_IM_RAW_RFMT_14_OR_16_BIT	14/16 bit   [RCTL1.RFMT = 00b]
												<li>@ref D_IM_RAW_RFMT_12_BIT_PACK	12 bit pack [RCTL1.RFMT = 01b]
												<li>@ref D_IM_RAW_RFMT_8_BIT		8 bit       [RCTL1.RFMT = 10b]
											 </ul>
										 */
	T_IM_RAW_AXI	axi_param;			/**< RAW AXI parameter. */
	VOID		(*callback)(UINT32);	/**< The pointer of callback function. The callback function can be called in Im_RAW_Int_Handler.
											 If this parameter is set to NULL, Im_RAW_Int_Handler does not call callback function. <br><br>
											 Sample of callback function.<br>
											 @code
											 VOID RAW_User_Callback( UINT32 condition )
											 {
												switch ( condition ) {
													case D_IM_RAW_COND_OK:
														// User process when COND = 000b.
														break;
													case D_IM_RAW_COND_ENC_LIMIT_OVER:
														// User process when COND = 001b.
														break;
													case D_IM_RAW_COND_DEC_UNDEF_CODE:
														// User process when COND = 010b.
														break;
													case D_IM_RAW_COND_DEC_ERROR:
														// User process when COND = 011b.
														break;
													case D_IM_RAW_R_AXI_ERROR:
														// User process when 100b.
														break;
													case D_IM_RAW_W_AXI_ERROR:
														// User process when 101b.
														break;
													default:
														break;
												}
											 }
											 @endcode
										   */
	// Only 12/14/16 bits
	UCHAR		mcu_size;			/**< RCTL1.DEFFC<br>
										 MCU size.<br>
										 This parameter is valid only when T_IM_RAW_CTRL_PARAM::data_format is set to D_IM_RAW_RFMT_14_or_16_BIT.<br>
											 <ul>
												<li>@ref D_IM_RAW_DEFFC_MCU_2x2		MCU 2x2 [RCTL1.DEFFC = 0b]
												<li>@ref D_IM_RAW_DEFFC_MCU_2x1		MCU 2x1 [RCTL1.DEFFC = 1b]
											 </ul>
										 */
	UCHAR		bit_select;			/**< RCTL1.BITSEL<br>
										 Select whether 16 bits or except 16 bits RAW data.<br>
										 This parameter is valid only when T_IM_RAW_CTRL_PARAM::data_format is set to D_IM_RAW_RFMT_14_or_16_BIT.<br>
											 <ul>
												<li>@ref D_IM_RAW_BITSEL_EXCEPT_16_BIT	Except 16 bit RAW [RCTL1.BITSEL = 0b]
												<li>@ref D_IM_RAW_BITSEL_16_BIT			16 bit RAW [RCTL1.BITSEL = 1b]
											 </ul>
									 */
	USHORT		width;				/**< HSIZE<br>
										 Width (pixels) [0~16383]. Should be aligned on 32.<br>
										 This parameter is valid only when T_IM_RAW_CTRL_PARAM::data_format is set to D_IM_RAW_RFMT_14_or_16_BIT.<br>
									 */
	USHORT		lines;				/**< VSIZE<br>
										 Lines (pixels) [0~16383]. Should be aligned on 2.<br>
										 This parameter is valid only when T_IM_RAW_CTRL_PARAM::data_format is set to D_IM_RAW_RFMT_14_or_16_BIT.<br>
									 */
	
	// Only 8bit data.
	UCHAR		diff_mode;			/**< RCTL1.DEFOP<br>
										 The mode of subtraction.<br>
										 This parameter is valid only when T_IM_RAW_CTRL_PARAM::data_format is set to D_IM_RAW_RFMT_8_BIT.<br>
											 <ul>
												<li>@ref D_IM_RAW_DEFOP_DIFF_PREV_VALUE		[RCTL1.DEFOP = 0b]
												<li>@ref D_IM_RAW_DEFOP_DIFF_FIXED_VALUE	[RCTL1.DEFOP = 1b]
											 </ul>
									 */
	UCHAR		*lut;				/**< RCTL1.LUTEN<br>
										 LUTx (x = 0, 1, ..., 255)<br>
										 If it is set to NULL, LUT is not used. [RCTL1.LUTEN = 0b]<br>
										 If it is set to the address of LUT table (UCHAR user_lut_table[256]), LUT is used. [RCTL1.LUTEN = 1b]<br>
										 This parameter is valid only when T_IM_RAW_CTRL_PARAM::data_format is set to D_IM_RAW_RFMT_8_BIT.<br>
									 */
	UINT32		data_size;			/**< DSIZE<br>
										 Data size (bytes)<br>
										 This parameter is valid only when T_IM_RAW_CTRL_PARAM::data_format is set to D_IM_RAW_RFMT_8_BIT.<br>
									 */
} T_IM_RAW_CTRL_PARAM;

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
RAW APB BUS CLK ON
@remarks	For debugging use only (for register dump on debugger)
*/
extern VOID Im_RAW_On_Pclk( VOID );

/**
RAW APB BUS CLK OFF
@remarks	For debugging use only (for register dump on debugger)
*/
extern VOID Im_RAW_Off_Pclk( VOID );

/**
Initialize driver.<br>
This API executes software-reset [RCTL0.SR = 1b] and releases software-reset [RCTL0.SR = 0b].
@retval		D_IM_RAW_RETVAL_OK				: Normal end.
@retval		D_IM_RAW_RETVAL_MACRO_BUSY 		: Error. This API is called when JDSRAW3A is running.
@remarks	After this API is called, any other API should be called. So, this is called at first.<br>
			This API should be called when JDSRAW3A is not running.
*/
extern INT32 Im_RAW_Init( VOID );

/**
Open driver. This API gets RAW semaphore using system-call.
@param[in]	wait_time : Wait time [msec]. The valid range is -1, 0, ... . 
						If this parameter is set to -1, 
						API waits permanently unless system-call gets RAW semaphore.
@retval		D_IM_RAW_RETVAL_OK					: Normal end.
@retval		D_IM_RAW_RETVAL_PARAM_ERROR			: Invalid argument error.
@retval		D_IM_RAW_RETVAL_SYSTEM_ERROR		: System-call error.
@remarks	This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
			This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern INT32 Im_RAW_Open( INT32 wait_time );

/**
Close driver. This API releases RAW semaphore using system-call.
@retval		D_IM_RAW_RETVAL_OK				: Normal end.
@retval		D_IM_RAW_RETVAL_SYSTEM_ERROR	: System-call error.
@remarks	This API uses DDIM_User_Sig_Sem().
*/
extern INT32 Im_RAW_Close( VOID );

/**
Setup CODEx and LENx.<br>
@param[in]	tbl 							: Pointer of code table.
@param[in]	data_format 					: Data format.<br>
												<ul>
													<li>@ref D_IM_RAW_RFMT_14_OR_16_BIT		Setup CODEx and LENx (x = 0x0, 0x1, ..., 0xE)
													<li>@ref D_IM_RAW_RFMT_12_BIT_PACK		Setup CODEx and LENx (x = 0x0, 0x1, ..., 0xC)
													<li>@ref D_IM_RAW_RFMT_8_BIT			Setup CODEx and LENx (x = 0x0, 0x1, ..., 0x8)
												</ul>
@retval		D_IM_RAW_RETVAL_OK				: Normal end.
@retval		D_IM_RAW_RETVAL_PARAM_ERROR		: Invalid argument error.
@retval		D_IM_RAW_RETVAL_MACRO_BUSY		: HW busy error.
@remarks	This API should be used only when JDSRAW3A is not runnning.
*/
extern INT32 Im_RAW_Ctrl_CodeTbl( const T_IM_RAW_CTRL_CODE_TBL* const tbl, UCHAR data_format );

/**
Setup encoding parameters except for CODEx and LENx.<br>
@param[in]	cfg 							: Pointer of encoding parameter table.
@param[in]	code_limit 						: The limitation of the size of encoded data.[CLMT]. [0x0~0x1FFFFFFF]<br>
											  If it is set to 0, no limitation.
@retval		D_IM_RAW_RETVAL_OK				: Normal end.
@retval		D_IM_RAW_RETVAL_PARAM_ERROR		: Invalid argument error.
@retval		D_IM_RAW_RETVAL_MACRO_BUSY		: HW busy error.
@remarks	This API should be used only when JDSRAW3A is not runnning.
*/
extern INT32 Im_RAW_Ctrl_Enc( const T_IM_RAW_CTRL_PARAM* const cfg, UINT32 code_limit );

/**
Setup decoding parameters except for CODEx and LENx.<br>
@param[in]	cfg 							: Pointer of decoding parameter table.
@retval		D_IM_RAW_RETVAL_OK				: Normal end.
@retval		D_IM_RAW_RETVAL_PARAM_ERROR		: Invalid argument error.
@retval		D_IM_RAW_RETVAL_MACRO_BUSY		: HW busy error.
@remarks	This API should be used only when JDSRAW3A is not runnning.
*/
extern INT32 Im_RAW_Ctrl_Dec( const T_IM_RAW_CTRL_PARAM* const cfg );

/**
Start JDSRAW3A.<br>
@retval		D_IM_RAW_RETVAL_OK				: Normal end.
@retval		D_IM_RAW_RETVAL_SYSTEM_ERROR	: System-call error.
@retval		D_IM_RAW_RETVAL_MACRO_BUSY		: HW busy error.
@remarks	This API uses DDIM_User_Clr_Flg().<br>
			This API should be used only when JDSRAW3A is not runnning.
*/
extern INT32 Im_RAW_Start( VOID );

/**
Encoding wait process until interruption occurs.
@param[out]	condition : Condition of JDSRAW3A process.		[COND]
						<ul>
							<li>@ref D_IM_RAW_COND_OK
							<li>@ref D_IM_RAW_COND_ENC_LIMIT_OVER
						</ul>
@param[out]	byte		: The size of encoded data. (bytes)	[CCNT]
@param[in]	wait_time	: Wait time [msec]. The valid range is -1, 0, ... . 
						If this parameter is set to -1, 
						API waits permanently unless system-call gets RAW semaphore.
@retval		D_IM_RAW_RETVAL_OK				: Normal end.
@retval		D_IM_RAW_RETVAL_SYSTEM_ERROR	: System-call error.
@retval		D_IM_RAW_RETVAL_FORCE_STOP		: Cancel wait process.
@retval		D_IM_RAW_RETVAL_R_AXI_ERR		: Read AXI Channel error.
@retval		D_IM_RAW_RETVAL_W_AXI_ERR		: Write AXI Channel error.
@retval		D_IM_RAW_RETVAL_TIME_OUT		: Wait time out.
@remarks	"condition" and "byte" are set only when returnd value is D_IM_RAW_RETVAL_OK.<br>
			This API uses DDIM_User_Twai_Flg().<br>
			This API uses DDIM_User_Clr_Flg().
*/
extern INT32 Im_RAW_Wait_End_Enc( UINT32* const condition, UINT32* const byte, INT32 wait_time );

/**
Decoding wait process until interruption occurs.
@param[out]	condition : Condition of JDSRAW3A process.		[COND]
						<ul>
							<li>@ref D_IM_RAW_COND_OK
							<li>@ref D_IM_RAW_COND_DEC_UNDEF_CODE
							<li>@ref D_IM_RAW_COND_DEC_ERROR
						</ul>
@param[in]	wait_time : Wait time [msec]. The valid range is -1, 0, ... . 
						If this parameter is set to -1, 
						API waits permanently unless system-call gets RAW semaphore.
@retval		D_IM_RAW_RETVAL_OK				: Normal end.
@retval		D_IM_RAW_RETVAL_SYSTEM_ERROR	: System-call error.
@retval		D_IM_RAW_RETVAL_FORCE_STOP		: Cancel wait process.
@retval		D_IM_RAW_RETVAL_R_AXI_ERR		: Read AXI Channel error.
@retval		D_IM_RAW_RETVAL_W_AXI_ERR		: Write AXI Channel error.
@retval		D_IM_RAW_RETVAL_TIME_OUT		: Wait time out.
@remarks	"condition" is set only when returnd value is D_IM_RAW_RETVAL_OK.<br>
			This API uses DDIM_User_Twai_Flg().<br>
			This API uses DDIM_User_Clr_Flg().
*/
extern INT32 Im_RAW_Wait_End_Dec( UINT32* const condition, INT32 wait_time );

/**
Stop JDSRAW3A.<br>
@retval		D_IM_RAW_RETVAL_OK				: Normal end.
@retval		D_IM_RAW_RETVAL_SYSTEM_ERROR	: System-call error.
@retval		D_IM_RAW_RETVAL_MACRO_BUSY		: HW Busy error. JDSRAW3A is still running.
@remarks	This API uses DDIM_User_Set_Flg() right after RCTL1.RTRG is set to 00b.
			So, it cancels Im_RAW_Wait_End_Enc() and Im_RAW_Wait_End_Dec()'s wait process.
*/
extern INT32 Im_RAW_Stop( VOID );

/**
Get AXI bus I/F Status
@param[out]	sts	AXI bus I/F status parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_RAW_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_RAW_Get_AXI_Status( T_IM_RAW_AXI_STATUS* const sts );

/**
Interrupt Handler. This API executes following process.
	<ul>
		<li>Clear interrupt (Write RINTF registers)
		<li>Set event-flag using DDIM_User_Set_Flg()
		<li>Call callback function (if callback function is set at Im_RAW_Ctrl_Enc() or Im_RAW_Ctrl_Dec().)
	</ul>
@remarks	Im_RAW_Int_Handler() should be called when interrupt of INTID84 occurs. <br>
*/
extern VOID Im_RAW_Int_Handler( VOID );

/*@}*/	// end of im_raw

#ifdef __cplusplus
}
#endif

#endif	// _IM_RAW_H_

/**
@weakgroup im_raw
@{
	@section im_raw_code Sample code
	- @ref RAW_Init
	- @ref RAW_Enc_Dec_Raw
	- @ref RAW_Enc_Dec_8bit
	- @ref RAW_Stop

	<hr>

	@section RAW_Init			Initialize Driver
	@code
	Im_RAW_Init();												// Release software-reset
	
	Dd_GIC_Ctrl( E_DD_GIC_INTID_RAW_INT, 1, D_DD_GIC_PRI30, 1 );	// Enable interrupt
	@endcode
	<br>
	<br>
	@section RAW_Enc_Dec_Raw		Encode/Decode RAW (16/14bit)
	@code
	T_IM_RAW_CTRL_PARAM 		cfg;
	T_IM_RAW_CTRL_CODE_TBL		tbl;


	// E N C O D E

	// Setup parameters
	cfg.src_addr = 0x48000000;								// MSA
	cfg.dst_addr = 0x58000000;								// MDA
	cfg.diff_fixed_value = 0;								// DEFINIT
	cfg.byte_stuffing = D_IM_RAW_OFF;						// RCTL1.BYTS
	cfg.data_format = D_IM_RAW_RFMT_14_or_16_BIT;			// RCTL1.RFMT
	cfg.axi_param.r_cache_type = D_IM_RAW_ON_CACHE_RW_WRITE_BACK;		// MRAXCTL
	cfg.axi_param.r_protection_type = D_IM_RAW_D_SEC_NORMAL_ACCESS;		// MRAXCTL
	cfg.axi_param.w_cache_type = D_IM_RAW_ON_CACHE_RW_WRITE_BACK;		// MWAXCTL
	cfg.axi_param.w_protection_type = D_IM_RAW_D_SEC_NORMAL_ACCESS;		// MWAXCTL
	cfg.callback = NULL;
	cfg.mcu_size = D_IM_RAW_DEFFC_MCU_2x2;					// RCTL1.DEFFC
	cfg.bit_select = D_IM_RAW_BITSEL_14_BIT					// RCTL1.BITSEL
															// If 16 bit RAW image is used, bit_select is set to D_IM_RAW_BITSEL_16_BIT.
	cfg.width = 1536;										// HSIZE
	cfg.lines = 1152;										// VSIZE

	// Setup code table
	tbl.code[0]  = 0x6000;		// CODE0
	tbl.code[1]  = 0x7000;		// CODE1
	tbl.code[2]  = 0x0000;		// CODE2
	tbl.code[3]  = 0x2000;		// CODE3
	tbl.code[4]  = 0x4000;		// CODE4
	tbl.code[5]  = 0x8000;		// CODE5
	tbl.code[6]  = 0x9000;		// CODE6
	tbl.code[7]  = 0xA000;		// CODE7
	tbl.code[8]  = 0xB000;		// CODE8
	tbl.code[9]  = 0xC000;		// CODE9
	tbl.code[10] = 0xD000;		// CODEA
	tbl.code[11] = 0xE000;		// CODEB
	tbl.code[12] = 0xE800;		// CODEC
	tbl.code[13] = 0xF000;		// CODED
	tbl.code[14] = 0xF800;		// CODEE
	tbl.code_length[0]  = 4;	// LEN0
	tbl.code_length[1]  = 4;	// LEN1
	tbl.code_length[2]  = 3;	// LEN2
	tbl.code_length[3]  = 3;	// LEN3
	tbl.code_length[4]  = 3;	// LEN4
	tbl.code_length[5]  = 4;	// LEN5
	tbl.code_length[6]  = 4;	// LEN6
	tbl.code_length[7]  = 4;	// LEN7
	tbl.code_length[8]  = 4;	// LEN8
	tbl.code_length[9]  = 4;	// LEN9
	tbl.code_length[10] = 4;	// LENA
	tbl.code_length[11] = 5;	// LENB
	tbl.code_length[12] = 5;	// LENC
	tbl.code_length[13] = 5;	// LEND
	tbl.code_length[14] = 5;	// LENE

	Im_RAW_Open( 100 );
	Im_RAW_Ctrl_CodeTbl( &tbl, cfg.data_format );
	Im_RAW_Ctrl_Enc( &cfg, 0 );								// 2nd arg is CLMT.

	Im_RAW_Start();

	Im_RAW_Wait_End_Enc( &condition, &byte, 5000 );
	Im_RAW_Close();
	


	// D E C O D E

	cfg.src_addr = 0x58000000;								// MSA
	cfg.dst_addr = 0x50000000;								// MDA
															// Other parameters is same as encoding parameters.

	Im_RAW_Open( 100 );
	Im_RAW_Ctrl_CodeTbl( &tbl, cfg.data_format );
	Im_RAW_Ctrl_Dec( &cfg );

	Im_RAW_Start();

	Im_RAW_Wait_End_Dec( &condition, 5000 );
	Im_RAW_Close();
	@endcode
	<br>
	<br>
	@section RAW_Enc_Dec_8bit	Encode/Decode 8bit Data.
	@code
	T_IM_RAW_CTRL_PARAM 		cfg;
	T_IM_RAW_CTRL_CODE_TBL		tbl;
	UCHAR						user_lut[256];


	// E N C O D E

	// Setup LUT
	Setup_User_Lut( user_lut );								// User function.

	// Setup parameters
	cfg.src_addr = 0x48000000;								// MSA
	cfg.dst_addr = 0x58000000;								// MDA
	cfg.diff_fixed_value = 0;								// DEFINIT
	cfg.byte_stuffing = D_IM_RAW_OFF;						// RCTL1.BYTS
	cfg.data_format = D_IM_RAW_RFMT_8_BIT;					// RCTL1.RFMT
	cfg.axi_param.r_cache_type = D_IM_RAW_ON_CACHE_RW_WRITE_BACK;		// MRAXCTL
	cfg.axi_param.r_protection_type = D_IM_RAW_D_SEC_NORMAL_ACCESS;		// MRAXCTL
	cfg.axi_param.w_cache_type = D_IM_RAW_ON_CACHE_RW_WRITE_BACK;		// MWAXCTL
	cfg.axi_param.w_protection_type = D_IM_RAW_D_SEC_NORMAL_ACCESS;		// MWAXCTL
	cfg.callback = NULL;
	cfg.diff_mode = D_IM_RAW_DEFOP_DIFF_PREV_VALUE;			// RCTL1.DEFOP
	cfg.lut = user_lut;										// RCTL1.LUTEN, LUTx (x = 0, 1, ..., 255)
															// If LUT is not used, lut is set to NULL.
	cfg.data_size = 0x4B000;								// DSIZE

	// Setup code table
	tbl.code[0]  = 0x6000;		// CODE0
	tbl.code[1]  = 0x7000;		// CODE1
	tbl.code[2]  = 0x0000;		// CODE2
	tbl.code[3]  = 0x2000;		// CODE3
	tbl.code[4]  = 0x4000;		// CODE4
	tbl.code[5]  = 0x8000;		// CODE5
	tbl.code[6]  = 0x9000;		// CODE6
	tbl.code[7]  = 0xA000;		// CODE7
	tbl.code[8]  = 0xB000;		// CODE8
	tbl.code_length[0]  = 4;	// LEN0
	tbl.code_length[1]  = 4;	// LEN1
	tbl.code_length[2]  = 3;	// LEN2
	tbl.code_length[3]  = 3;	// LEN3
	tbl.code_length[4]  = 3;	// LEN4
	tbl.code_length[5]  = 4;	// LEN5
	tbl.code_length[6]  = 4;	// LEN6
	tbl.code_length[7]  = 4;	// LEN7
	tbl.code_length[8]  = 4;	// LEN8

	Im_RAW_Open( 100 );
	Im_RAW_Ctrl_CodeTbl( &tbl, cfg.data_format );
	Im_RAW_Ctrl_Enc( &cfg, 0 );								// 2nd arg is CLMT.

	Im_RAW_Start();

	Im_RAW_Wait_End_Enc( &condition, &byte, 5000 );
	Im_RAW_Close();


	// D E C O D E

	// Setup parameters
	cfg.src_addr = 0x58000000;								// MSA
	cfg.dst_addr = 0x50000000;								// MDA
															// Other parameters is same as encoding parameters.

	Im_RAW_Open( 100 );
	Im_RAW_Ctrl_CodeTbl( &tbl, cfg.data_format );
	Im_RAW_Ctrl_Dec( &cfg );

	Im_RAW_Start();

	Im_RAW_Wait_End_Dec( &condition, 5000 );
	Im_RAW_Close();
	@endcode
	<br>
	<br>
	@section RAW_Stop			Stop JDSRAW3A
	@code
	UINT32 cond, byte;
	INT32  retval;

	Im_RAW_Start();			// Start JDSRAW3A

	Im_RAW_Stop();			// Force stop

	retval = Im_RAW_Wait_End_Enc( &cond, &byte, 1000 );	// Wait
														// retval is set to D_IM_RAW_RETVAL_FORCE_STOP.
	@endcode
	<br>
	<br>
@}*/ /* end of im_raw_code */
