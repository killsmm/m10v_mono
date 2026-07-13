/**
 * @file		ct_dd_pmu.c
 * @brief		PMU driver Component Test
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "driver_common.h"
#include "ct_dd_pmu.h"
#include "dd_pmu.h"
#include "pmc.h"

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
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief  Command main function for PMU test.
 * @param  int argc	:The number of parameters
 *		   char** argv	:The value of parameters
 * @return VOID
 */
VOID Ct_Dd_Pmu_Main(int argc, char** argv)
{
	int					param2;
	INT32				ret;
	ULONG				value;
	
	if(strcmp(argv[1], "pd") == 0){
		// get 2nd paramter.
		if(strcmp(argv[2], "get") == 0){
			// get 3rd paramter.
			if(argc > 3){
				param2 = atoi(argv[3]);

				ret = Dd_PMU_Get_Status(param2, &value);
				Ddim_Print(("--- PMU Power status ---\n"));
				Ddim_Print(("Status : %d=0x%08lx ret=0x%08x\n", param2, value, ret));
			}
			else {
				Ddim_Print(("--- PMU Power status ---\n"));
				for (param2 = 0; param2 < E_DD_PMU_PD_TYPE_MAX; param2 ++) {
					if (param2 == E_DD_PMU_PD_TYPE_RESERVED) {
						continue;
					}
					ret = Dd_PMU_Get_Status(param2, &value);
					Ddim_Print(("Status : %d=0x%08lx ret=0x%08x\n", param2, value, ret));
				}
			}
		}
		else if(strcmp(argv[2], "off") == 0){
			if(argc > 3){
				// get 3rd paramter.
				param2 = atoi(argv[3]);

				ret = Dd_PMU_PD_Off(param2);
				Ddim_Print(("--- PMU status ---\n"));
				Ddim_Print(("Stauts : %d ret=0x%08x\n", param2, ret));
			}
			else {
				Ddim_Print(("--- PMU status ---\n"));
				for (param2 = 0; param2 < E_DD_PMU_PD_TYPE_MAX; param2 ++) {
					if (param2 == E_DD_PMU_PD_TYPE_RESERVED) {
						continue;
					}
					ret = Dd_PMU_PD_Off(param2);
					Ddim_Print(("Status : %d ret=0x%08x\n", param2, ret));
				}
			}
		}
		else if(strcmp(argv[2], "on") == 0){
			if(argc > 3){
				// get 3rd paramter.
				param2 = atoi(argv[3]);

				ret = Dd_PMU_PD_On(param2);
				Ddim_Print(("--- PMU status ---\n"));
				Ddim_Print(("Status : %d ret=0x%08x\n", param2, ret));
			}
			else {
				Ddim_Print(("--- PMU status ---\n"));
				for (param2 = 0; param2 < E_DD_PMU_PD_TYPE_MAX; param2 ++) {
					if (param2 == E_DD_PMU_PD_TYPE_RESERVED) {
						continue;
					}
					ret = Dd_PMU_PD_On(param2);
					Ddim_Print(("Status : %d ret=0x%08x\n", param2, ret));
				}
			}
		}
		else if(strcmp(argv[2], "onoff") == 0){
			if(argc > 3){
				// get 3rd paramter.
				param2 = atoi(argv[3]);

				ret = Dd_PMU_Get_Status(param2, &value);
				Ddim_Print(("Status : %d=0x%08lx ret=0x%08x\n", param2, value, ret));
				ret = Dd_PMU_PD_On(param2);
				ret = Dd_PMU_Get_Status(param2, &value);
				Ddim_Print(("Status : %d=0x%08lx ret=0x%08x\n", param2, value, ret));

				ret = Dd_PMU_Get_Status(param2, &value);
				Ddim_Print(("Status : %d=0x%08lx ret=0x%08x\n", param2, value, ret));
				ret = Dd_PMU_PD_Off(param2);
				ret = Dd_PMU_Get_Status(param2, &value);
				Ddim_Print(("Status : %d=0x%08lx ret=0x%08x\n", param2, value, ret));

				Ddim_Print(("--- PMU status ---\n"));
				Ddim_Print(("Status : %d ret=0x%08x\n", param2, ret));
			}
			else {
				Ddim_Print(("--- PMU status ---\n"));
				for (param2 = 0; param2 < E_DD_PMU_PD_TYPE_MAX; param2 ++) {
					if (param2 == E_DD_PMU_PD_TYPE_RESERVED) {
						continue;
					}
					ret = Dd_PMU_Get_Status(param2, &value);
					Ddim_Print(("Status : %d=0x%08lx ret=0x%08x\n", param2, value, ret));

					ret = Dd_PMU_PD_On(param2);
					Ddim_Print(("Status : %d ret=0x%08x\n", param2, ret));

					ret = Dd_PMU_Get_Status(param2, &value);
					Ddim_Print(("Status : %d=0x%08lx ret=0x%08x\n", param2, value, ret));

					ret = Dd_PMU_PD_Off(param2);
					Ddim_Print(("Status : %d ret=0x%08x\n", param2, ret));

					ret = Dd_PMU_Get_Status(param2, &value);
					Ddim_Print(("Status : %d=0x%08lx ret=0x%08x\n\n", param2, value, ret));
				}
			}
		}
		else{
			Ddim_Print(("please check 2nd parameter!!\n"));
		}
	}
	else{
		Ddim_Print(("please check 1st parameter!!\n"));
	}
}
