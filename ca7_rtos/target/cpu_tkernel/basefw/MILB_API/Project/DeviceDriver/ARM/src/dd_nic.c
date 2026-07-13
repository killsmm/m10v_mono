/**
 * @file		dd_nic.c
 * @brief		NIC-400 (Network Inter Connect) driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_nic.h"
#include "dd_arm.h"

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
static ULONG gDD_Nic_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

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
 * @brief	Set to Slave 1 security setting register
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_NIC_INPUT_PARAM_ERROR
 */
INT32 Dd_NIC_Set_ADDRCTRL_Security1(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_NIC_SECURITY1_MAX ){
		Ddim_Assertion(("NIC: input param error. [val] = %lx\n", val));
		return D_DD_NIC_INPUT_PARAM_ERR;
	}
#endif
	Dd_ARM_Critical_Section_Start(gDD_Nic_Spin_Lock);
	
	IO_NIC_ADDRCTRL.SECURITY1.word = val;
	
	Dd_ARM_Dmb_Pou();
	
	Dd_ARM_Critical_Section_End(gDD_Nic_Spin_Lock);
	
	return D_DDIM_OK;
}

/**
 * @brief	Set to ASIB Tidemark register
 * @param	UINT32 num, ULONG val
 * @return	D_DDIM_OK/D_DD_NIC_INPUT_PARAM_ERROR
 */
INT32 Dd_NIC_Set_ASIB_Wr_Tidemark(UINT32 num, ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( num > D_DD_NIC_NODE_NUM_MAX ){
		Ddim_Assertion(("NIC: input param error. [num] = %x\n", num));
		return D_DD_NIC_INPUT_PARAM_ERR;
	}
	if( val > D_DD_NIC_TIDEMARK_VAL_MAX ){
		Ddim_Assertion(("NIC: input param error. [val] = %lx\n", val));
		return D_DD_NIC_INPUT_PARAM_ERR;
	}
#endif
	IO_NIC_ASIB[num].WR_TIDEMARK.bit.WT = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	Set to ASIB Read Channel QoS register
 * @param	UINT32 num, ULONG val
 * @return	D_DDIM_OK/D_DD_NIC_INPUT_PARAM_ERROR
 */
INT32 Dd_NIC_Set_ASIB_Read_QoS(UINT32 num, ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( num > D_DD_NIC_NODE_NUM_MAX ){
		Ddim_Assertion(("NIC: input param error. [num] = %x\n", num));
		return D_DD_NIC_INPUT_PARAM_ERR;
	}
	if( val > D_DD_NIC_QOS_VAL_MAX ){
		Ddim_Assertion(("NIC: input param error. [val] = %lx\n", val));
		return D_DD_NIC_INPUT_PARAM_ERR;
	}
#endif
	IO_NIC_ASIB[num].READ_QOS.bit.RQ = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	Set to ASIB Write Channel QoS register
 * @param	UINT32 num, ULONG val
 * @return	D_DDIM_OK/D_DD_NIC_INPUT_PARAM_ERROR
 */
INT32 Dd_NIC_Set_ASIB_Write_QoS(UINT32 num, ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( num > D_DD_NIC_NODE_NUM_MAX ){
		Ddim_Assertion(("NIC: input param error. [num] = %x\n", num));
		return D_DD_NIC_INPUT_PARAM_ERR;
	}
	if( val > D_DD_NIC_QOS_VAL_MAX ){
		Ddim_Assertion(("NIC: input param error. [val] = %lx\n", val));
		return D_DD_NIC_INPUT_PARAM_ERR;
	}
#endif
	
	IO_NIC_ASIB[num].WRITE_QOS.bit.WQ = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	Set to ASIB Functionality Modification register
 * @param	UINT32 num, ULONG val
 * @return	D_DDIM_OK/D_DD_NIC_INPUT_PARAM_ERROR
 */
INT32 Dd_NIC_Set_ASIB_Func_Mod(UINT32 num, ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( num > D_DD_NIC_NODE_NUM_MAX ){
		Ddim_Assertion(("NIC: input param error. [num] = %x\n", num));
		return D_DD_NIC_INPUT_PARAM_ERR;
	}
	if( val > D_DD_NIC_FM_VAL_MAX ){
		Ddim_Assertion(("NIC: input param error. [val] = %lx\n", val));
		return D_DD_NIC_INPUT_PARAM_ERR;
	}
#endif
	IO_NIC_ASIB[num].FN_MOD.bit.FM = val;
	
	return D_DDIM_OK;
}
