/**
 * @file		palladium_test_common.c
 * @brief		test code on palladium
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "palladium_test.h"
#include "palladium_test_common.h"
#include "palladium_test_interrupt.h"


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
T_DDIM_INFO gDDIM_Info;
/**
 * @brief		Call main command 
 * @param		VOID
 * @return		0: OK other: error
 * @note		None
 * @attention	None
 */
extern int Palladium_Test_Cmd_Call_Main(INT32 argc, CHAR argv[][D_PALLADIUM_TEST_CMD_CHAR_NUM], ct_Main ct_command_main )
{
	int ret = 0; 
	if(argc <= D_PALLADIUM_TEST_CMD_PARA_NUM){
		CHAR* p_rgv[D_PALLADIUM_TEST_CMD_PARA_NUM];
		int i;
		for(i = 0; i< argc; i++)
		{
			if((argv[i])[0] == 0){
				break;
			}
			p_rgv[i] = argv[i];
		}
	 
	    ct_command_main(i, p_rgv); 
    }
    else{
		ret = -1;
	}
	return ret;
}
/**
 * @brief		Dump register data to file 
 * @param		VOID
 * @return		0: OK other: error
 * @note		None
 * @attention	None
 */
extern int Palladium_Test_Cmd_Dump_Reg(ULONG reg_address, ULONG size )
{
	UINT32* output_address_temp = (UINT32*)((0x8FFFFFFF- size) & 0xffffff00);
	UINT32* output_address_temp_save = output_address_temp;
	UINT32* intput_address_temp =(UINT32*)reg_address;
	for(int i = 0; i< size/4; i++){
		*output_address_temp = *intput_address_temp;
		output_address_temp++;
		intput_address_temp++;
	}
	Palladium_Set_Out_Localstack((ULONG)output_address_temp_save , size);

	return 0;
}


