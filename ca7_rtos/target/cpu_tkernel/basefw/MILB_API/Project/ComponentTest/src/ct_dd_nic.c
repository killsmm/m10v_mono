/**
 * @file		ct_dd_nic.c
 * @brief		dd_nic Component Test
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "driver_common.h"
#include "dd_nic.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static VOID ct_dd_nic_run(INT32 ct_param_1st, ULONG ct_param_2st, ULONG ct_param_3st)
{
	INT32 dat;
	INT32 num;

	switch ( ct_param_1st ) {
		case 1:
			for( num=0; num <= D_DD_NIC_NODE_NUM_MAX; num++) {
				dat = Dd_NIC_Get_ASIB_Wr_Tidemark(num);
				Ddim_Print(("ASIB[%d] (Tidemark) = %d\n", num, dat));
				
				dat = Dd_NIC_Get_ASIB_Read_QoS(num);
				Ddim_Print(("ASIB[%d] (Read channel QoS) = %d\n", num, dat));
				
				dat = Dd_NIC_Get_ASIB_Write_QoS(num);
				Ddim_Print(("ASIB[%d] (Write channel QoS) = %d\n", num, dat));
				
				dat = Dd_NIC_Get_ASIB_Func_Mod(num);
				Ddim_Print(("ASIB[%d] (Functionality Modification) = %d\n", num, dat));
			}
			break;
		case 2:
			Dd_NIC_Set_ADDRCTRL_Security1(ct_param_2st);
			Ddim_Print(("It set ADDRCTRL Security1.\n"));
			break;
		case 3:
			Dd_NIC_Set_ASIB_Wr_Tidemark((UINT32)ct_param_2st, ct_param_3st);
			Ddim_Print(("It set ASIB Tidemark.\n"));
			break;
		case 4:
			Dd_NIC_Set_ASIB_Read_QoS((UINT32)ct_param_2st, ct_param_3st);
			Ddim_Print(("It set ASIB Read Channel QoS.\n"));
			break;
		case 5:
			Dd_NIC_Set_ASIB_Write_QoS((UINT32)ct_param_2st, ct_param_3st);
			Ddim_Print(("It set ASIB Write Channel QoS.\n"));
			break;
		case 6:
			Dd_NIC_Set_ASIB_Func_Mod((UINT32)ct_param_2st, ct_param_3st);
			Ddim_Print(("It set ASIB Functionality Modification.\n"));
			break;
		default:
			Ddim_Print(("please check parameter!!\n"));
			break;
	}
	return;
}

/**
 * @brief  Command main function for NIC test.
 * @param  int argc	:The number of parameters
 *		   char** argv	:The value of parameters
 * @return VOID
 *
 *		The meaning of parameters
 *		+---------------------+-----+-----+-----------------------------------------------------+
 *		| P1                  | P2  | P3  |Meaning                                              |
 *		+---------------------+-----+-----+-----------------------------------------------------+
 *		| 1:display           | -   | -   |Display all registers in Network Inter Connect.      |
 *		+---------------------+-----+-----+-----------------------------------------------------+
 *		| 2:set Security1     | dat | -   |It set ADDRCTRL Security1.                           |
 *		+---------------------+-----+-----+-----------------------------------------------------+
 *		| 3:set asib_tidemark | num | dat |It set ASIB Tidemark.                                |
 *		+---------------------+-----+-----+-----------------------------------------------------+
 *		| 4:set asib_rqos     | num | dat |It set ASIB Read Channel QoS.                        |
 *		+---------------------+-----+-----+-----------------------------------------------------+
 *		| 5:set asib_wqos     | num | dat |It set ASIB Write Channel QoS.                       |
 *		+---------------------+-----+-----+-----------------------------------------------------+
 *		| 6:set asib_fmod     | num | dat |It set ASIB Functionality Modification.              |
 *		+---------------------+-----+-----+-----------------------------------------------------+
 */
VOID Ct_Dd_NIC_Main(int argc, char** argv)
{
	INT32 param1;
	ULONG param2 =0, param3 =0;
	
	if ( argc > 4 ) {
		Ddim_Print(("input parameter error\n"));
		return;
	}
	
	param1 = atoi((const char *)argv[1]);
	
	if (argc >= 2) {
		param2 = atol((const char *)argv[2]);
	}
	
	if (argc >= 3) {
		param3 = atol((const char *)argv[3]);
	}
	
	ct_dd_nic_run( param1, param2, param3 );
	
	return;
}
