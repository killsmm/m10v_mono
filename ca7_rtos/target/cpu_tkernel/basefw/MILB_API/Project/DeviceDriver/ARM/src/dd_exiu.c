/**
 * @file		dd_exiu.c
 * @brief		EXIU (External Interrupt Unit) driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_exiu.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
ULONG gDD_Exiu_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief	Get Interrupt Mask
 * @param	UINT32 ch, UINT32* val
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Get_EIMASK(UINT32 ch, UINT32* val)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
	if( val == NULL ){
		Ddim_Assertion(("EXIU: input param error. [val] = NULL\n"));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	*val = (1 & (IO_EXIU.EIMASK.word >> (ch - D_DD_EXIU_CH_MIN)));
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Interrupt Source Select
 * @param	UINT32 ch, UINT32* val
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Get_EISRCSEL(UINT32 ch, UINT32* val)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
	if( val == NULL ){
		Ddim_Assertion(("EXIU: input param error. [val] = NULL\n"));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	*val = (1 & (IO_EXIU.EISRCSEL.word >> (ch - D_DD_EXIU_CH_MIN)));
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Internal Interrupt Request Status
 * @param	UINT32 ch, UINT32* val
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Get_EIREQSTA(UINT32 ch, UINT32* val)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
	if( val == NULL ){
		Ddim_Assertion(("EXIU: input param error. [val] = NULL\n"));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	*val = (1 & (IO_EXIU.EIREQSTA.word >> (ch - D_DD_EXIU_CH_MIN)));
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Raw Interrupt Request Status
 * @param	UINT32 ch, UINT32* val
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Get_EIRAWREQSTA(UINT32 ch, UINT32* val)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
	if( val == NULL ){
		Ddim_Assertion(("EXIU: input param error. [val] = NULL\n"));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	*val = (1 & (IO_EXIU.EIRAWREQSTA.word >> (ch - D_DD_EXIU_CH_MIN)));
	
	return D_DDIM_OK;
}

/**
 * @brief	Get External Interrupt Level Setting
 * @param	UINT32 ch, UINT32* val
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Get_EILVL(UINT32 ch, UINT32* val)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
	if( val == NULL ){
		Ddim_Assertion(("EXIU: input param error. [val] = NULL\n"));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	*val = (1 & (IO_EXIU.EILVL.word >> (ch - D_DD_EXIU_CH_MIN)));
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Interrupt Detection Method Setting
 * @param	UINT32 ch, UINT32* val
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Get_EIEDG(UINT32 ch, UINT32* val)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
	if( val == NULL ){
		Ddim_Assertion(("EXIU: input param error. [val] = NULL\n"));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	*val = (1 & (IO_EXIU.EIEDG.word >> (ch - D_DD_EXIU_CH_MIN)));
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Software Interrupt
 * @param	UINT32 ch, UINT32* val
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Get_EISIR(UINT32 ch, UINT32* val)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
	if( val == NULL ){
		Ddim_Assertion(("EXIU: input param error. [val] = NULL\n"));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	*val = (1 & (IO_EXIU.EISIR.word >> (ch - D_DD_EXIU_CH_MIN)));
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Interrupt Mask Release Select
 * @param	UINT32 ch
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Set_EIMASK_Release(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	Dd_ARM_Critical_Section_Start(gDD_Exiu_Spin_Lock);
	
	IO_EXIU.EIMASK.word &= ~(1U << (ch - D_DD_EXIU_CH_MIN));
	
	Dd_ARM_Critical_Section_End(gDD_Exiu_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Interrupt Mask Select
 * @param	UINT32 ch
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Set_EIMASK_Mask(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	Dd_ARM_Critical_Section_Start(gDD_Exiu_Spin_Lock);
	
	IO_EXIU.EIMASK.word |= (1U << (ch - D_DD_EXIU_CH_MIN));
	
	Dd_ARM_Critical_Section_End(gDD_Exiu_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Interrupt is Mask or Release
 * @param	UINT32 val
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Set_EIMASK_Val(UINT32 val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_EXIU_VAL_MAX ){
		Ddim_Assertion(("EXIU: input param error. [val] = %08x\n", val));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	Dd_ARM_Critical_Section_Start(gDD_Exiu_Spin_Lock);
	
	IO_EXIU.EIMASK.word = (val);
	
	Dd_ARM_Critical_Section_End(gDD_Exiu_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Interrupt External Source Select
 * @param	UINT32 ch
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Set_EISRCSEL_Ext(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	Dd_ARM_Critical_Section_Start(gDD_Exiu_Spin_Lock);
	
	IO_EXIU.EISRCSEL.word &= ~(1U << (ch - D_DD_EXIU_CH_MIN));
	
	Dd_ARM_Critical_Section_End(gDD_Exiu_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Interrupt Sofware Source Select
 * @param	UINT32 ch
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Set_EISRCSEL_Soft(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	Dd_ARM_Critical_Section_Start(gDD_Exiu_Spin_Lock);
	
	IO_EXIU.EISRCSEL.word |= (1U << (ch - D_DD_EXIU_CH_MIN));
	
	Dd_ARM_Critical_Section_End(gDD_Exiu_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Interrupt Cleared Channel
 * @param	UINT32 ch
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Set_EIREQCLR_Ch(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	Dd_ARM_Critical_Section_Start(gDD_Exiu_Spin_Lock);
	
	IO_EXIU.EIREQCLR.word |= (1U << (ch - D_DD_EXIU_CH_MIN));
	
	Dd_ARM_Critical_Section_End(gDD_Exiu_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Interrupt Cleared Value
 * @param	UINT32 val
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Set_EIREQCLR_Val(UINT32 val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_EXIU_VAL_MAX ){
		Ddim_Assertion(("EXIU: input param error. [val] = %08x\n", val));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	Dd_ARM_Critical_Section_Start(gDD_Exiu_Spin_Lock);
	
	IO_EXIU.EIREQCLR.word = (val);
	
	Dd_ARM_Critical_Section_End(gDD_Exiu_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief	External Interrupt Low level or Falling edge
 * @param	UINT32 ch
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Set_EILVL_Lo(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	Dd_ARM_Critical_Section_Start(gDD_Exiu_Spin_Lock);
	
	IO_EXIU.EILVL.word &= ~(1U << (ch - D_DD_EXIU_CH_MIN));
	
	Dd_ARM_Critical_Section_End(gDD_Exiu_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief	External Interrupt High level or Rising edge
 * @param	UINT32 ch
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Set_EILVL_Hi(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	Dd_ARM_Critical_Section_Start(gDD_Exiu_Spin_Lock);
	
	IO_EXIU.EILVL.word |= (1U << (ch - D_DD_EXIU_CH_MIN));
	
	Dd_ARM_Critical_Section_End(gDD_Exiu_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Interrupt Level Detection Setting
 * @param	UINT32 ch
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Set_EIEDG_Level(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	Dd_ARM_Critical_Section_Start(gDD_Exiu_Spin_Lock);
	
	IO_EXIU.EIEDG.word &= ~(1U << (ch - D_DD_EXIU_CH_MIN));
	
	Dd_ARM_Critical_Section_End(gDD_Exiu_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Interrupt Edge Detection Setting
 * @param	UINT32 ch
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Set_EIEDG_Edge(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	Dd_ARM_Critical_Section_Start(gDD_Exiu_Spin_Lock);
	
	IO_EXIU.EIEDG.word |= (1U << (ch - D_DD_EXIU_CH_MIN));
	
	Dd_ARM_Critical_Section_End(gDD_Exiu_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Software Interrupt Generate
 * @param	UINT32 ch
 * @return	D_DDIM_OK/D_DD_EXIU_INPUT_PARAM_ERR
 */
INT32 Dd_EXIU_Set_EISIR_Generate(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if( ch < D_DD_EXIU_CH_MIN || ch > D_DD_EXIU_CH_MAX ){
		Ddim_Assertion(("EXIU: input param error. [ch] = %d\n", ch));
		return D_DD_EXIU_INPUT_PARAM_ERR;
	}
#endif
	
	Dd_ARM_Critical_Section_Start(gDD_Exiu_Spin_Lock);
	
	IO_EXIU.EISIR.word |= (1U << (ch - D_DD_EXIU_CH_MIN));
	
	Dd_ARM_Critical_Section_End(gDD_Exiu_Spin_Lock);
	
	return D_DDIM_OK;
}
