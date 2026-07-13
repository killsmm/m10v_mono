/**
 * @file		dd_gpv.c
 * @brief		GPV (Global Programmers View) driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_gpv.h"
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
static volatile UINT32 gRemap = 0;
static ULONG gDD_Gpv_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

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
 * @brief	M_Usb3_r_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_M_Usb3_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_M_Usb3_r_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.MUSB3RIB.FMIB;
	gpv_ctrl_m->FM2		= IO_GPV.MUSB3RIB.FM2;
	gpv_ctrl_m->FM		= IO_GPV.MUSB3RIB.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Emmc_r_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_M_Emmc_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_M_Emmc_r_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.MEMMCRIB.FMIB;
	gpv_ctrl_m->FM2		= IO_GPV.MEMMCRIB.FM2;
	gpv_ctrl_m->FML		= IO_GPV.MEMMCRIB.FML;
	gpv_ctrl_m->FM		= IO_GPV.MEMMCRIB.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Uhs2_r_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_M_Uhs2_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_M_Uhs2_r_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.MUHS2RIB.FMIB;
	gpv_ctrl_m->FM2		= IO_GPV.MUHS2RIB.FM2;
	gpv_ctrl_m->FML		= IO_GPV.MUHS2RIB.FML;
	gpv_ctrl_m->FM		= IO_GPV.MUHS2RIB.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Mxic1_m_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_M_Mxic1_m_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_M_Mxic1_m_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.MMXIC1MIB.FMIB;
	gpv_ctrl_m->WT		= IO_GPV.MMXIC1MIB.WT;
	gpv_ctrl_m->FM		= IO_GPV.MMXIC1MIB.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Mxic2_m_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_M_Mxic2_m_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_M_Mxic2_m_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.MMXIC2MIB.FMIB;
	gpv_ctrl_m->WT		= IO_GPV.MMXIC2MIB.WT;
	gpv_ctrl_m->FM		= IO_GPV.MMXIC2MIB.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Xdmac_r_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_M_Xdmac_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_M_Xdmac_r_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.MXDMACRIB.FMIB;
	gpv_ctrl_m->FM2		= IO_GPV.MXDMACRIB.FM2;
	gpv_ctrl_m->FM		= IO_GPV.MXDMACRIB.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Nf_r_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_M_Nf_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_M_Nf_r_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.MNFRIB.FMIB;
	gpv_ctrl_m->FM2		= IO_GPV.MNFRIB.FM2;
	gpv_ctrl_m->FML		= IO_GPV.MNFRIB.FML;
	gpv_ctrl_m->FM		= IO_GPV.MNFRIB.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Pcie0_r_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_M_Pcie0_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_M_Pcie0_r_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.MPCIE0RIB.FMIB;
	gpv_ctrl_m->FM2		= IO_GPV.MPCIE0RIB.FM2;
	gpv_ctrl_m->FML		= IO_GPV.MPCIE0RIB.FML;
	gpv_ctrl_m->FM		= IO_GPV.MPCIE0RIB.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Pcie1_r_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_M_Pcie1_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_M_Pcie1_r_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.MPCIE1RIB.FMIB;
	gpv_ctrl_m->FM2		= IO_GPV.MPCIE1RIB.FM2;
	gpv_ctrl_m->FML		= IO_GPV.MPCIE1RIB.FML;
	gpv_ctrl_m->FM		= IO_GPV.MPCIE1RIB.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicPeriAHB_m_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_M_NicPeriAHB_m_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_M_NicPeriAHB_m_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.MNICPERIAHBMIB.FMIB;
	gpv_ctrl_m->FM2		= IO_GPV.MNICPERIAHBMIB.FM2;
	gpv_ctrl_m->AC		= IO_GPV.MNICPERIAHBMIB.AC;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicPeriAHB_m_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_M_NicBmAHB1_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_M_NicBmAHB1_r_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.MNICBMAHB1RIB.FMIB;
	gpv_ctrl_m->AC		= IO_GPV.MNICBMAHB1RIB.AC;
	
	return D_DDIM_OK;
}

/**
 * @brief	NsecX2P_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_NsecX2P_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_NsecX2P_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.NSECX2PIB.FMIB;
	gpv_ctrl_m->FM2		= IO_GPV.NSECX2PIB.FM2;
	
	return D_DDIM_OK;
}

/**
 * @brief	ToprX2P_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_ToprX2P_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_ToprX2P_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.TOPRX2PIB.FMIB;
	gpv_ctrl_m->FM2		= IO_GPV.TOPRX2PIB.FM2;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicBmAHB2_r_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_M_NicBmAHB2_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_M_NicBmAHB2_r_ib: input param error. [gpv_ctrl_m] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_m->FMIB	= IO_GPV.MNICBMAHB2RIB.FMIB;
	gpv_ctrl_m->FM2		= IO_GPV.MNICBMAHB2RIB.FM2;
	gpv_ctrl_m->AC		= IO_GPV.MNICBMAHB2RIB.AC;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_WifiAHB_m_ib register values are acquired.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_S_WifiAHB_m_ib(T_DD_GPV_CTRL_S* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_S_WifiAHB_m_ib: input param error. [gpv_ctrl_s] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_s->FMA		= IO_GPV.SWIFIAHBMIB.FMA;
	gpv_ctrl_s->FM		= IO_GPV.SWIFIAHBMIB.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Uhs2AHB_m register values are acquired.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_S_Uhs2AHB_m(T_DD_GPV_CTRL_S* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_S_Uhs2AHB_m: input param error. [gpv_ctrl_s] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_s->FMA		= IO_GPV.SUHS2AHBM.FMA;
	gpv_ctrl_s->FM		= IO_GPV.SUHS2AHBM.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_RelcrAHB_m register values are acquired.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_S_RelcrAHB_m(T_DD_GPV_CTRL_S* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_S_RelcrAHB_m: input param error. [gpv_ctrl_s] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_s->FMA		= IO_GPV.SRELCRAHBM.FMA;
	gpv_ctrl_s->FM		= IO_GPV.SRELCRAHBM.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Uhs1AHB_m register values are acquired.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_S_Uhs1AHB_m(T_DD_GPV_CTRL_S* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_S_Uhs1AHB_m: input param error. [gpv_ctrl_s] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_s->FMA		= IO_GPV.SUHS1AHBM.FMA;
	gpv_ctrl_s->FM		= IO_GPV.SUHS1AHBM.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_RelcwAHB_m register values are acquired.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_S_RelcwAHB_m(T_DD_GPV_CTRL_S* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_S_RelcwAHB_m: input param error. [gpv_ctrl_s] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_s->FMA		= IO_GPV.SRELCWAHBM.FMA;
	gpv_ctrl_s->FM		= IO_GPV.SRELCWAHBM.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_BmNicAHB_m register values are acquired.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_S_BmNicAHB_m(T_DD_GPV_CTRL_S* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_S_BmNicAHB_m: input param error. [gpv_ctrl_s] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_s->FMA		= IO_GPV.SBMNICAHBM.FMA;
	gpv_ctrl_s->FM		= IO_GPV.SBMNICAHBM.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Usb2AHB_ehci register values are acquired.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_S_Usb2AHB_ehci(T_DD_GPV_CTRL_S* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_S_Usb2AHB_ehci: input param error. [gpv_ctrl_s] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_s->FMA		= IO_GPV.SUSB2AHBEHCI.FMA;
	gpv_ctrl_s->FM		= IO_GPV.SUSB2AHBEHCI.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Usb2AHB_ohci register values are acquired.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_S_Usb2AHB_ohci(T_DD_GPV_CTRL_S* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_S_Usb2AHB_ohci: input param error. [gpv_ctrl_s] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_s->FMA		= IO_GPV.SUSB2AHBOHCI.FMA;
	gpv_ctrl_s->FM		= IO_GPV.SUSB2AHBOHCI.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB8 register values are acquired.
 * @param	T_DD_GPV_CTRL_IB* gpv_ctrl_ib
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_IB8(T_DD_GPV_CTRL_IB* const gpv_ctrl_ib)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_ib == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_IB8: input param error. [gpv_ctrl_ib] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_ib->FMIB	= IO_GPV.IB8.FMIB;
	gpv_ctrl_ib->FM2	= IO_GPV.IB8.FM2;
	gpv_ctrl_ib->WT		= IO_GPV.IB8.WT;
	gpv_ctrl_ib->FM		= IO_GPV.IB8.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB13 register values are acquired.
 * @param	T_DD_GPV_CTRL_IB* gpv_ctrl_ib
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_IB13(T_DD_GPV_CTRL_IB* const gpv_ctrl_ib)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_ib == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_IB13: input param error. [gpv_ctrl_ib] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_ib->FMIB	= IO_GPV.IB13.FMIB;
	gpv_ctrl_ib->FM2	= IO_GPV.IB13.FM2;
	gpv_ctrl_ib->FML	= IO_GPV.IB13.FML;
	gpv_ctrl_ib->FM		= IO_GPV.IB13.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB12 register values are acquired.
 * @param	T_DD_GPV_CTRL_IB* gpv_ctrl_ib
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_IB12(T_DD_GPV_CTRL_IB* const gpv_ctrl_ib)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_ib == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_IB12: input param error. [gpv_ctrl_ib] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_ib->FMIB	= IO_GPV.IB12.FMIB;
	gpv_ctrl_ib->FM2	= IO_GPV.IB12.FM2;
	gpv_ctrl_ib->FM		= IO_GPV.IB12.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB9 register values are acquired.
 * @param	T_DD_GPV_CTRL_IB* gpv_ctrl_ib
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Get_IB9(T_DD_GPV_CTRL_IB* const gpv_ctrl_ib)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_ib == NULL) {
		Ddim_Assertion(("Dd_GPV_Get_IB9: input param error. [gpv_ctrl_ib] NULL\n"));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	// Register value acquisition
	gpv_ctrl_ib->FMIB	= IO_GPV.IB9.FMIB;
	gpv_ctrl_ib->FM2	= IO_GPV.IB9.FM2;
	gpv_ctrl_ib->WT		= IO_GPV.IB9.WT;
	gpv_ctrl_ib->FM		= IO_GPV.IB9.FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	Get to REMAP register.
 * @return	Remap value
 */
UINT32 Dd_GPV_Get_ARC_Remap(VOID)
{
	return gRemap;
}

/**
 * @brief	Set to REMAP register.
 * @param	ULONG val, UINT32 set
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_ARC_Remap(ULONG val, UINT32 set)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_REMAP_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( set > 1 ){
		Ddim_Assertion(("GPV: input param error. [set] = %d\n", set));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	Dd_ARM_Critical_Section_Start( gDD_Gpv_Spin_Lock );
	
	if(set){
		gRemap |= val;
		IO_GPV.REMAP.word = gRemap;
	}else{
		gRemap &= ~val;
		IO_GPV.REMAP.word = gRemap;
	}
	
	Dd_ARM_Dmb_Pou();
	
	Dd_ARM_Critical_Section_End( gDD_Gpv_Spin_Lock );
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Usb3_r_ib register.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Usb3_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FMIB] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM2] = 0x%lx\n", gpv_ctrl_m->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FML > D_DD_GPV_FML_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FML] = 0x%lx\n", gpv_ctrl_m->FML));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM] = 0x%lx\n", gpv_ctrl_m->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MUSB3RIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.MUSB3RIB.FM2		= gpv_ctrl_m->FM2;
	IO_GPV.MUSB3RIB.FM		= gpv_ctrl_m->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Usb3_r_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Usb3_r_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MUSB3RIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Usb3_r_ib FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Usb3_r_ib_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MUSB3RIB.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Usb3_r_ib FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Usb3_r_ib_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MUSB3RIB.FM = val;
	
	return D_DDIM_OK;
}


/**
 * @brief	M_Emmc_r_ib register.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Emmc_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FMIB] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM2] = 0x%lx\n", gpv_ctrl_m->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FML > D_DD_GPV_FML_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FML] = 0x%lx\n", gpv_ctrl_m->FML));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM] = 0x%lx\n", gpv_ctrl_m->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MEMMCRIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.MEMMCRIB.FM2		= gpv_ctrl_m->FM2;
	IO_GPV.MEMMCRIB.FML		= gpv_ctrl_m->FML;
	IO_GPV.MEMMCRIB.FM		= gpv_ctrl_m->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Emmc_r_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Emmc_r_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MEMMCRIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Emmc_r_ib FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Emmc_r_ib_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MEMMCRIB.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Emmc_r_ib FML register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Emmc_r_ib_FML(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FML_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MEMMCRIB.FML = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Emmc_r_ib FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Emmc_r_ib_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MEMMCRIB.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Uhs2_r_ib register.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Uhs2_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FMIB] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM2] = 0x%lx\n", gpv_ctrl_m->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FML > D_DD_GPV_FML_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FML] = 0x%lx\n", gpv_ctrl_m->FML));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM] = 0x%lx\n", gpv_ctrl_m->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MUHS2RIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.MUHS2RIB.FM2		= gpv_ctrl_m->FM2;
	IO_GPV.MUHS2RIB.FML		= gpv_ctrl_m->FML;
	IO_GPV.MUHS2RIB.FM		= gpv_ctrl_m->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Uhs2_r_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Uhs2_r_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MUHS2RIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Uhs2_r_ib FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Uhs2_r_ib_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MUHS2RIB.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Uhs2_r_ib FML register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Uhs2_r_ib_FML(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FML_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MUHS2RIB.FML = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Uhs2_r_ib FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Uhs2_r_ib_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MUHS2RIB.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Mxic1_m_ib FMIB register.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Mxic1_m_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FMIB] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->WT > D_DD_GPV_WT_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.WT] = 0x%lx\n", gpv_ctrl_m->WT));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM] = 0x%lx\n", gpv_ctrl_m->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MMXIC1MIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.MMXIC1MIB.WT		= gpv_ctrl_m->WT;
	IO_GPV.MMXIC1MIB.FM		= gpv_ctrl_m->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Mxic1_m_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Mxic1_m_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MMXIC1MIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Mxic1_m_ib WT register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Mxic1_m_ib_WT(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_WT_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MMXIC1MIB.WT = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Mxic1_m_ib FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Mxic1_m_ib_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MMXIC1MIB.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Mxic2_m_ib register.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Mxic2_m_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FMIB] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->WT > D_DD_GPV_WT_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.WT] = 0x%lx\n", gpv_ctrl_m->WT));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM] = 0x%lx\n", gpv_ctrl_m->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MMXIC2MIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.MMXIC2MIB.WT		= gpv_ctrl_m->WT;
	IO_GPV.MMXIC2MIB.FM		= gpv_ctrl_m->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Mxic2_m_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Mxic2_m_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MMXIC2MIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Mxic2_m_ib WT register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Mxic2_m_ib_WT(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_WT_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MMXIC2MIB.WT = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Mxic2_m_ib FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Mxic2_m_ib_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MMXIC2MIB.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Xdmac_r_ib register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Xdmac_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", gpv_ctrl_m->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", gpv_ctrl_m->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MXDMACRIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.MXDMACRIB.FM2	= gpv_ctrl_m->FM2;
	IO_GPV.MXDMACRIB.FM		= gpv_ctrl_m->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Xdmac_r_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Xdmac_r_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MXDMACRIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Xdmac_r_ib FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Xdmac_r_ib_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MXDMACRIB.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Xdmac_r_ib FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Xdmac_r_ib_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MXDMACRIB.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Nf_r_ib register.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Nf_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FMIB] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM2] = 0x%lx\n", gpv_ctrl_m->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FML > D_DD_GPV_FML_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FML] = 0x%lx\n", gpv_ctrl_m->FML));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM] = 0x%lx\n", gpv_ctrl_m->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNFRIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.MNFRIB.FM2	= gpv_ctrl_m->FM2;
	IO_GPV.MNFRIB.FML	= gpv_ctrl_m->FML;
	IO_GPV.MNFRIB.FM	= gpv_ctrl_m->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Nf_r_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Nf_r_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNFRIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Nf_r_ib FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Nf_r_ib_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNFRIB.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Nf_r_ib FML register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Nf_r_ib_FML(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FML_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNFRIB.FML = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Nf_r_ib FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Nf_r_ib_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNFRIB.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Pcie0_r_ib register.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Pcie0_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FMIB] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM2] = 0x%lx\n", gpv_ctrl_m->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FML > D_DD_GPV_FML_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FML] = 0x%lx\n", gpv_ctrl_m->FML));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM] = 0x%lx\n", gpv_ctrl_m->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MPCIE0RIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.MPCIE0RIB.FM2	= gpv_ctrl_m->FM2;
	IO_GPV.MPCIE0RIB.FML	= gpv_ctrl_m->FML;
	IO_GPV.MPCIE0RIB.FM		= gpv_ctrl_m->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Pcie0_r_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Pcie0_r_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MPCIE0RIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Pcie0_r_ib FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Pcie0_r_ib_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MPCIE0RIB.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Pcie0_r_ib FML register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Pcie0_r_ib_FML(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FML_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MPCIE0RIB.FML = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Pcie0_r_ib FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Pcie0_r_ib_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MPCIE0RIB.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Pcie1_r_ib register.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Pcie1_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FMIB] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM2] = 0x%lx\n", gpv_ctrl_m->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FML > D_DD_GPV_FML_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FML] = 0x%lx\n", gpv_ctrl_m->FML));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM] = 0x%lx\n", gpv_ctrl_m->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MPCIE1RIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.MPCIE1RIB.FM2	= gpv_ctrl_m->FM2;
	IO_GPV.MPCIE1RIB.FML	= gpv_ctrl_m->FML;
	IO_GPV.MPCIE1RIB.FM		= gpv_ctrl_m->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Pcie1_r_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Pcie1_r_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MPCIE1RIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Pcie1_r_ib FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Pcie1_r_ib_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MPCIE1RIB.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Pcie1_r_ib FML register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Pcie1_r_ib_FML(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FML_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MPCIE1RIB.FML = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_Pcie1_r_ib FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_Pcie1_r_ib_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MPCIE1RIB.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicPeriAHB_m_ib register.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_NicPeriAHB_m_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FMIB] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM2] = 0x%lx\n", gpv_ctrl_m->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->AC > D_DD_GPV_AC_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.AC] = 0x%lx\n", gpv_ctrl_m->AC));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNICPERIAHBMIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.MNICPERIAHBMIB.FM2	= gpv_ctrl_m->FM2;
	IO_GPV.MNICPERIAHBMIB.AC	= gpv_ctrl_m->AC;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicPeriAHB_m_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_NicPeriAHB_m_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNICPERIAHBMIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicPeriAHB_m_ib FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_NicPeriAHB_m_ib_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNICPERIAHBMIB.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicPeriAHB_m_ib AC register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_NicPeriAHB_m_AC(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_AC_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNICPERIAHBMIB.AC = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicBmAHB1_r_ib register.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_NicBmAHB1_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FMIB] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->AC > D_DD_GPV_AC_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.AC] = 0x%lx\n", gpv_ctrl_m->AC));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNICBMAHB1RIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.MNICBMAHB1RIB.AC		= gpv_ctrl_m->AC;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicBmAHB1_r_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_NicBmAHB1_r_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNICBMAHB1RIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicBmAHB1_r_ib AC register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_NicBmAHB1_r_AC(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_AC_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNICBMAHB1RIB.AC = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	NsecX2P_ib register.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_NsecX2P_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FMIB] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM2] = 0x%lx\n", gpv_ctrl_m->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.NSECX2PIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.NSECX2PIB.FM2	= gpv_ctrl_m->FM2;
	return D_DDIM_OK;
}

/**
 * @brief	NsecX2P_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_NsecX2P_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.NSECX2PIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	NsecX2P_ib FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_NsecX2P_ib_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.NSECX2PIB.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	ToprX2P_ib register.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_ToprX2P_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FMIB] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM2] = 0x%lx\n", gpv_ctrl_m->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.TOPRX2PIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.TOPRX2PIB.FM2	= gpv_ctrl_m->FM2;
	
	return D_DDIM_OK;
}

/**
 * @brief	ToprX2P_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_ToprX2P_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.TOPRX2PIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	ToprX2P_ib FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_ToprX2P_ib_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.TOPRX2PIB.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicBmAHB2_r_ib register.
 * @param	T_DD_GPV_CTRL_M* gpv_ctrl_m
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_NicBmAHB2_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_m->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FMIB] = 0x%lx\n", gpv_ctrl_m->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.FM2] = 0x%lx\n", gpv_ctrl_m->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_m->AC > D_DD_GPV_AC_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_m.AC] = 0x%lx\n", gpv_ctrl_m->AC));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNICBMAHB2RIB.FMIB	= gpv_ctrl_m->FMIB;
	IO_GPV.MNICBMAHB2RIB.FM2	= gpv_ctrl_m->FM2;
	IO_GPV.MNICBMAHB2RIB.AC		= gpv_ctrl_m->AC;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicBmAHB2_r_ib FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_NicBmAHB2_r_ib_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNICBMAHB2RIB.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicBmAHB2_r_ib FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_NicBmAHB2_r_ib_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNICBMAHB2RIB.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	M_NicBmAHB2_r_ib AC register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_M_NicBmAHB2_r_AC(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_AC_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.MNICBMAHB2RIB.AC = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Main_rm_ib FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Main_rm_ib_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SMAINRMIB.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Xdmac_m_ib FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Xdmac_m_ib_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SXDMACMIB.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Netsec_m FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Netsec_m_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SNETSECM.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_WifiAHB_m_ib register.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_WifiAHB_m_ib(T_DD_GPV_CTRL_S const* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s->FMA > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FMA] = 0x%lx\n", gpv_ctrl_s->FMA));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_s->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FM] = 0x%lx\n", gpv_ctrl_s->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SWIFIAHBMIB.FMA		= gpv_ctrl_s->FMA;
	IO_GPV.SWIFIAHBMIB.FM		= gpv_ctrl_s->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_WifiAHB_m_ib FMA register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_WifiAHB_m_FMA(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SWIFIAHBMIB.FMA = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_WifiAHB_m_ib FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_WifiAHB_m_ib_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SWIFIAHBMIB.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Uhs2AHB_m register.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Uhs2AHB_m(T_DD_GPV_CTRL_S const* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s->FMA > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", gpv_ctrl_s->FMA));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_s->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", gpv_ctrl_s->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SUHS2AHBM.FMA	= gpv_ctrl_s->FMA;
	IO_GPV.SUHS2AHBM.FM		= gpv_ctrl_s->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Uhs2AHB_m FMA register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Uhs2AHB_m_FMA(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SUHS2AHBM.FMA = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Uhs2AHB_m FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Uhs2AHB_m_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SUHS2AHBM.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_RelcrAHB_m register.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_RelcrAHB_m(T_DD_GPV_CTRL_S const* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s->FMA > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FMA] = 0x%lx\n", gpv_ctrl_s->FMA));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_s->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FM] = 0x%lx\n", gpv_ctrl_s->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SRELCRAHBM.FMA		= gpv_ctrl_s->FMA;
	IO_GPV.SRELCRAHBM.FM		= gpv_ctrl_s->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_RelcrAHB_m FMA register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_RelcrAHB_m_FMA(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SRELCRAHBM.FMA = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_RelcrAHB_m FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_RelcrAHB_m_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SRELCRAHBM.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Uhs1AHB_m register.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Uhs1AHB_m(T_DD_GPV_CTRL_S const* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s->FMA > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FMA] = 0x%lx\n", gpv_ctrl_s->FMA));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_s->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FM] = 0x%lx\n", gpv_ctrl_s->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SUHS1AHBM.FMA	= gpv_ctrl_s->FMA;
	IO_GPV.SUHS1AHBM.FM		= gpv_ctrl_s->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Uhs1AHB_m FMA register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Uhs1AHB_m_FMA(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SUHS1AHBM.FMA = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Uhs1AHB_m FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Uhs1AHB_m_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SUHS1AHBM.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_RelcwAHB_m register.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_RelcwAHB_m(T_DD_GPV_CTRL_S const* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s->FMA > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FMA] = 0x%lx\n", gpv_ctrl_s->FMA));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_s->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FM] = 0x%lx\n", gpv_ctrl_s->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SRELCWAHBM.FMA		= gpv_ctrl_s->FMA;
	IO_GPV.SRELCWAHBM.FM		= gpv_ctrl_s->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_RelcwAHB_m FMA register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_RelcwAHB_m_FMA(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SRELCWAHBM.FMA = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_RelcwAHB_m FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_RelcwAHB_m_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SRELCWAHBM.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_BmNicAHB_m register.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_BmNicAHB_m(T_DD_GPV_CTRL_S const* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s->FMA > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FMA] = 0x%lx\n", gpv_ctrl_s->FMA));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_s->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FM] = 0x%lx\n", gpv_ctrl_s->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SBMNICAHBM.FMA		= gpv_ctrl_s->FMA;
	IO_GPV.SBMNICAHBM.FM		= gpv_ctrl_s->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_BmNicAHB_m FMA register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_BmNicAHB_m_FMA(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SBMNICAHBM.FMA = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_BmNicAHB_m FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_BmNicAHB_m_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SBMNICAHBM.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Usb2AHB_ehci register.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Usb2AHB_ehci(T_DD_GPV_CTRL_S const* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s->FMA > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FMA] = 0x%lx\n", gpv_ctrl_s->FMA));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_s->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FM] = 0x%lx\n", gpv_ctrl_s->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SUSB2AHBEHCI.FMA		= gpv_ctrl_s->FMA;
	IO_GPV.SUSB2AHBEHCI.FM		= gpv_ctrl_s->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Usb2AHB_ehci FMA register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Usb2AHB_ehci_FMA(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SUSB2AHBEHCI.FMA = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Usb2AHB_ehci FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Usb2AHB_ehci_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SUSB2AHBEHCI.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Usb2AHB_ohci register.
 * @param	T_DD_GPV_CTRL_S* gpv_ctrl_s
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Usb2AHB_ohci(T_DD_GPV_CTRL_S const* const gpv_ctrl_s)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_s->FMA > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FMA] = 0x%lx\n", gpv_ctrl_s->FMA));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_s->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_s.FM] = 0x%lx\n", gpv_ctrl_s->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SUSB2AHBOHCI.FMA		= gpv_ctrl_s->FMA;
	IO_GPV.SUSB2AHBOHCI.FM		= gpv_ctrl_s->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Usb2AHB_ohci FMA register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Usb2AHB_ohci_FMA(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMA_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SUSB2AHBOHCI.FMA = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Usb2AHB_ohci FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Usb2AHB_ohci_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SUSB2AHBOHCI.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	S_Netsec2_m FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_S_Netsec2_m_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.SNETSEC2M.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB8 register.
 * @param	T_DD_GPV_CTRL_IB* gpv_ctrl_ib
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB8(T_DD_GPV_CTRL_IB const* const gpv_ctrl_ib)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_ib->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.FMIB] = 0x%lx\n", gpv_ctrl_ib->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_ib->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.FM2] = 0x%lx\n", gpv_ctrl_ib->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_ib->WT > D_DD_GPV_WT_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.WT] = 0x%lx\n", gpv_ctrl_ib->WT));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_ib->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.FM] = 0x%lx\n", gpv_ctrl_ib->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB8.FMIB	= gpv_ctrl_ib->FMIB;
	IO_GPV.IB8.FM2	= gpv_ctrl_ib->FM2;
	IO_GPV.IB8.WT	= gpv_ctrl_ib->WT;
	IO_GPV.IB8.FM	= gpv_ctrl_ib->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB8 FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB8_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB8.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB8 FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB8_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB8.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB8 WT register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB8_WT(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_WT_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB8.WT = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB8 FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB8_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB8.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB13 register.
 * @param	T_DD_GPV_CTRL_IB* gpv_ctrl_ib
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB13(T_DD_GPV_CTRL_IB const* const gpv_ctrl_ib)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_ib->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.FMIB] = 0x%lx\n", gpv_ctrl_ib->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_ib->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.FM2] = 0x%lx\n", gpv_ctrl_ib->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_ib->FML > D_DD_GPV_FML_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.FML] = 0x%lx\n", gpv_ctrl_ib->FML));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_ib->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.FM] = 0x%lx\n", gpv_ctrl_ib->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB13.FMIB	= gpv_ctrl_ib->FMIB;
	IO_GPV.IB13.FM2		= gpv_ctrl_ib->FM2;
	IO_GPV.IB13.FML		= gpv_ctrl_ib->FML;
	IO_GPV.IB13.FM		= gpv_ctrl_ib->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB13 FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB13_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB13.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB13 FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB13_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB13.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB13 FML register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB13_FML(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FML_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB13.FML = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB13 FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB13_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB13.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB12 register.
 * @param	T_DD_GPV_CTRL_IB* gpv_ctrl_ib
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB12(T_DD_GPV_CTRL_IB const* const gpv_ctrl_ib)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_ib->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.FMIB] = 0x%lx\n", gpv_ctrl_ib->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_ib->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.FM2] = 0x%lx\n", gpv_ctrl_ib->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_ib->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.FM] = 0x%lx\n", gpv_ctrl_ib->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB12.FMIB	= gpv_ctrl_ib->FMIB;
	IO_GPV.IB12.FM2		= gpv_ctrl_ib->FM2;
	IO_GPV.IB12.FM		= gpv_ctrl_ib->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB12 FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB12_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB12.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB12 FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB12_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB12.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB12 FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB12_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB12.FM = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB9 register.
 * @param	T_DD_GPV_CTRL_IB* gpv_ctrl_ib
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB9(T_DD_GPV_CTRL_IB const* const gpv_ctrl_ib)
{
#ifdef CO_PARAM_CHECK
	if( gpv_ctrl_ib->FMIB > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.FMIB] = 0x%lx\n", gpv_ctrl_ib->FMIB));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_ib->FM2 > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.FM2] = 0x%lx\n", gpv_ctrl_ib->FM2));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_ib->WT > D_DD_GPV_WT_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.WT] = 0x%lx\n", gpv_ctrl_ib->WT));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
	if( gpv_ctrl_ib->FM > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [gpv_ctrl_ib.FM] = 0x%lx\n", gpv_ctrl_ib->FM));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB9.FMIB	= gpv_ctrl_ib->FMIB;
	IO_GPV.IB9.FM2	= gpv_ctrl_ib->FM2;
	IO_GPV.IB9.WT	= gpv_ctrl_ib->WT;
	IO_GPV.IB9.FM	= gpv_ctrl_ib->FM;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB9 FMIB register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB9_FMIB(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FMIB_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB9.FMIB = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB9 FM2 register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB9_FM2(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM2_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB9.FM2 = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB9 WT register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB9_WT(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_WT_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB9.WT = val;
	
	return D_DDIM_OK;
}

/**
 * @brief	IB9 FM register.
 * @param	ULONG val
 * @return	D_DDIM_OK/D_DD_GPV_INPUT_PARAM_ERROR
 */
INT32 Dd_GPV_Set_IB9_FM(ULONG val)
{
#ifdef CO_PARAM_CHECK
	if( val > D_DD_GPV_FM_VAL_MAX ){
		Ddim_Assertion(("GPV: input param error. [val] = 0x%lx\n", val));
		return D_DD_GPV_INPUT_PARAM_ERR;
	}
#endif
	IO_GPV.IB9.FM = val;
	
	return D_DDIM_OK;
}
