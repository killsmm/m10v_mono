/**
 * @file		dd_exiu.h
 * @brief		EXIU (External Interrupt Unit) driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_exiu
@{
	- @ref operation_procedure_sequence_exiu
	- @ref dd_exiu_sample_section
*/

#ifndef _DD_EXIU_H_
#define _DD_EXIU_H_

#include "driver_common.h"
#include "dd_arm.h"
#include "arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	D_DD_EXIU_INPUT_PARAM_ERR		(D_DD_EXIU | D_DDIM_INPUT_PARAM_ERROR)		/**< Input Parameter Error */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define D_DD_EXIU_CH_MAX				(31)										/**< Channel Max Number */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define D_DD_EXIU_CH_MAX				(32)										/**< Channel Max Number */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define D_DD_EXIU_CH_MIN				(16)										/**< Channel Min Number */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define D_DD_EXIU_VAL_MAX				(0x0000FFFF)								/**< Value Max Number */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define D_DD_EXIU_VAL_MAX				(0x0001FFFF)								/**< Value Max Number */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define D_DD_EXIU_VAL_MIN				(0x00000000)								/**< Value Min Number */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
Get Interrupt Mask.
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@param [out] *val						: Interrupt Mask Value
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Get_EIMASK(UINT32 ch, UINT32* val);

/**
Get Interrupt Source Select.
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@param [out] *val						: Interrupt Source Select Value
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Get_EISRCSEL(UINT32 ch, UINT32* val);

/**
Get Internal Interrupt Request Status
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@param [out] *val						: Internal Interrupt Request Status Value
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Get_EIREQSTA(UINT32 ch, UINT32* val);

/**
Get Raw Interrupt Request Status
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@param [out] *val						: Raw Interrupt Request Status Value
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Get_EIRAWREQSTA(UINT32 ch, UINT32* val);

/**
Get External Interrupt Level Setting
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@param [out] *val						: External Interrupt Level Setting Value
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Get_EILVL(UINT32 ch, UINT32* val);

/**
Get Interrupt Detection Method Setting
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@param [out] *val						: Interrupt Detection Method Setting Value
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Get_EIEDG(UINT32 ch, UINT32* val);

/**
Get Software Interrupt
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@param [out] *val						: Software Interrupt Value
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Get_EISIR(UINT32 ch, UINT32* val);

/**
Set Interrupt Mask Release Select
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Set_EIMASK_Release(UINT32 ch);

/**
Set Interrupt Mask Select
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Set_EIMASK_Mask(UINT32 ch);

/**
Set Interrupt is Mask or Release
@param [in]	val							: Interrupt Mask or Release Value ( @ref D_DD_EXIU_VAL_MIN to @ref D_DD_EXIU_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Set_EIMASK_Val(UINT32 val);

/**
Set Interrupt External Source Select
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Set_EISRCSEL_Ext(UINT32 ch);

/**
Set Interrupt Sofware Source Select
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Set_EISRCSEL_Soft(UINT32 ch);

/**
Set Interrupt Cleared Channel
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Set_EIREQCLR_Ch(UINT32 ch);

/**
Set Interrupt Cleared Value
@param [in]	val							: Interrupt Cleared Value ( @ref D_DD_EXIU_VAL_MIN to @ref D_DD_EXIU_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Set_EIREQCLR_Val(UINT32 val);

/**
Set External Interrupt Low level or Falling edge
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Set_EILVL_Lo(UINT32 ch);

/**
Set External Interrupt High level or Rising edge
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Set_EILVL_Hi(UINT32 ch);

/**
Set Interrupt Level Detection Setting
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Set_EIEDG_Level(UINT32 ch);

/**
Set Interrupt Edge Detection Setting
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Set_EIEDG_Edge(UINT32 ch);

/**
Set Software Interrupt Generate
@param [in]	ch							: EXIU Channel Number ( @ref D_DD_EXIU_CH_MIN to @ref D_DD_EXIU_CH_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_EXIU_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_EXIU_Set_EISIR_Generate(UINT32 ch);

#ifdef __cplusplus
}
#endif

#endif	// _DD_EXIU_H_

/*@}*/

/**
@weakgroup dd_exiu
@{
	@section operation_procedure_sequence_exiu		Example of Operating Procedure for External Interrupt.
	@image html Operating_Procedure_for_External_Interrupt.png
	<br>
	@section dd_exiu_sample_section					Sample of "Operating Procedure for External Interrupt" using Dd_EXIU_xxx functions.
    @code
	// interrupt parameter
	#define EXTERNAL_INTERRUPT	(0)
	#define SOFTWARE_INTERRUPT	(1)
	
	// trigger parameter
	#define LOW_LEVEL			(0)
	#define HIGH_LEVEL			(1)
	#define FALLING_EDGE		(2)
	#define RISING_EDGE			(3)
	
	// detection type
	#define LEVEL_DETECTION		(0)
	#define EDGE_DETECTION		(1)
	
	VOID Init_External_Interrupt_Unit(UCHAR ch_num, UCHAR interrupt_prm, UCHAR trigger_prm)
	{
		// ch_num		 : D_DD_EXIU_CH_MIN - D_DD_EXIU_CH_MAX
		// interrupt_prm : 0 - 1
		// trigger_prm	 : 0 - 3
		
		if( ch_num < D_DD_EXIU_CH_MIN || ch_num > D_DD_EXIU_CH_MAX ) {
			Ddim_Print(("Error Channel Prameter !!\n"));
			return;
		}
		
		// Set the interrupt mask in the "Interrupt Masking Register (EIMASK)".
		Dd_EXIU_Set_EIMASK_Mask(ch_num);
		
		// Set the interrupt source in the "Interrupt Source Select Register (EISRCSEL)".
		if( interrupt_prm == EXTERNAL_INTERRUPT ) {
			Dd_EXIU_Set_EISRCSEL_Ext(ch_num);
		}
		else if( interrupt_prm == SOFTWARE_INTERRUPT ) {
			Dd_EXIU_Set_EISRCSEL_Soft(ch_num);
		}
		else {
			Ddim_Print(("Error Interrupt Source Select Prameter !!\n"));
			return;
		}
		
		// Set the external interrupt request signal in the "External Interrupt Level Setting Register (EILVL)".
		if( trigger_prm == LOW_LEVEL || trigger_prm == FALLING_EDGE ) {
			Dd_EXIU_Set_EILVL_Lo(ch_num);
		}
		else if( trigger_prm == HIGH_LEVEL || trigger_prm == RISING_EDGE ) {
			Dd_EXIU_Set_EILVL_Hi(ch_num);
		}
		else {
			Ddim_Print(("Error Trigger Prameter !!\n"));
			return;
		}
		
		// Set the external interrupt request signal in the "Interrupt Detection Method Setting Register (EIEDG)".
		if( trigger_prm == LOW_LEVEL || trigger_prm == HIGH_LEVEL ) {
			Dd_EXIU_Set_EIEDG_Level(ch_num);
		}
		else if( trigger_prm == FALLING_EDGE || trigger_prm == RISING_EDGE ) {
			Dd_EXIU_Set_EIEDG_Edge(ch_num);
		}
		else {
			Ddim_Print(("Error Trigger Prameter !!\n"));
			return;
		}
		
		// Clear all the interrupts requests by writing to the "Interrupt Clear Register (EIREQCLR)".
		Dd_EXIU_Set_EIREQCLR_Ch(ch_num);
		
		// In order to release the mask on the interrupt ports, set the "Interrupt Masking Register (EIMASK)".
		Dd_EXIU_Set_EIMASK_Release(ch_num);
	}
    @endcode
@}*/
