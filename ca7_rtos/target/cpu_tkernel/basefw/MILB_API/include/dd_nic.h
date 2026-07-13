/**
 * @file		dd_nic.h
 * @brief		NIC-400 (Network Inter Connect) driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_nic
@{
	- @ref dd_nic_sample_section1
*/

#ifndef _DD_NIC_H_
#define _DD_NIC_H_

#include "driver_common.h"
#include "arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Return value of NIC processing result
#define	D_DD_NIC_INPUT_PARAM_ERR			(D_DD_NIC | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error */

// Max value of SECURITY1
#define D_DD_NIC_SECURITY1_MAX				(65535)									/**< Security1 Max Value */

// Max number of ASIB node
#define D_DD_NIC_NODE_NUM_MAX				(6)										/**< Node Max Number */

// Max value of Tidemark
#define D_DD_NIC_TIDEMARK_VAL_MAX			(15)									/**< Tidemark Max Value */

// Max value of QoS
#define D_DD_NIC_QOS_VAL_MAX				(15)									/**< Qos Max Value */

// Max value of Func Mod
#define D_DD_NIC_FM_VAL_MAX					(1)										/**< Func Mod Max Value */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define Dd_NIC_Get_ASIB_Wr_Tidemark(num)	(IO_NIC_ASIB[num].WR_TIDEMARK.bit.WT)	/**< Get Tidemark (num: 0 to @ref D_DD_NIC_NODE_NUM_MAX) */
#define Dd_NIC_Get_ASIB_Read_QoS(num)		(IO_NIC_ASIB[num].READ_QOS.bit.RQ)		/**< Get Read channel QoS (num: 0 to @ref D_DD_NIC_NODE_NUM_MAX) */
#define Dd_NIC_Get_ASIB_Write_QoS(num)		(IO_NIC_ASIB[num].WRITE_QOS.bit.WQ)		/**< Get Write channel QoS (num: 0 to @ref D_DD_NIC_NODE_NUM_MAX) */
#define Dd_NIC_Get_ASIB_Func_Mod(num)		(IO_NIC_ASIB[num].FN_MOD.bit.FM)		/**< Get Functionality Modification (num: 0 to @ref D_DD_NIC_NODE_NUM_MAX) */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
Set to Slave 1 security setting register
@param [in]	val							: Security 1 State ( 0 to @ref D_DD_NIC_SECURITY1_MAX )
                                          1bit( 0:Secure 1:Non-Secure )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_NIC_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_NIC_Set_ADDRCTRL_Security1(ULONG val);

/**
Set to ASIB Tidemark register
@param [in]	num							: ASIB Interface ID ( 0 to @ref D_DD_NIC_NODE_NUM_MAX )
@param [in]	val							: Tidemark ( 0 to @ref D_DD_NIC_TIDEMARK_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_NIC_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_NIC_Set_ASIB_Wr_Tidemark(UINT32 num, ULONG val);

/**
Set to ASIB Read Channel QoS register
@param [in]	num							: ASIB Interface ID ( 0 to @ref D_DD_NIC_NODE_NUM_MAX )
@param [in]	val							: Read Channel QoS ( 0 to @ref D_DD_NIC_QOS_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_NIC_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_NIC_Set_ASIB_Read_QoS(UINT32 num, ULONG val);

/**
Set to ASIB Write Channel QoS register

@param [in]	num							: ASIB Interface ID ( 0 to @ref D_DD_NIC_NODE_NUM_MAX )
@param [in]	val							: Write Channel QoS ( 0 to @ref D_DD_NIC_QOS_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_NIC_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_NIC_Set_ASIB_Write_QoS(UINT32 num, ULONG val);

/**
Set to ASIB Functionality Modification register

@param [in]	num							: ASIB Interface ID ( 0 to @ref D_DD_NIC_NODE_NUM_MAX )
@param [in]	val							: Issuing functionality modification
										<ul>
										<li>0:Read
										<li>1:Write
										</ul>
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_NIC_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_NIC_Set_ASIB_Func_Mod(UINT32 num, ULONG val);

#ifdef __cplusplus
}
#endif

#endif	// _DD_NIC_H_

/*@}*/

/**
@weakgroup dd_nic
@{
	@section dd_nic_sample_section1		Sample of "ASIB Functionality Modification" using Dd_NIC_Set_ASIB_Func_Mod function.
    @code
	//
	// it prepares set the block write issuing capability to one outstanding transaction.
	//
	Dd_NIC_Set_ASIB_Func_Mod( 0, 1 );
	//
	// After this, write issuing.
	//
    @endcode
@}*/
