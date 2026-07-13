/**
 * @file		ct_dd_xdmac.c
 * @brief		dd_xdmac Component Test
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "ct_dd_xdmac.h"
#include "driver_common.h"
#include "dd_xdmac.h"
#include "dd_top.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static T_DD_XDMAC_COMMON			gXDMAC_Common;

static VOID ct_xdmac_callback(VOID)
{
	Ddim_Print(("XDMAC Callback\n"));
}

static INT32 ct_xdmac_process(UCHAR num)
{
	INT32                 ret = 0;
	UINT32                wait_mode = 0;
	USHORT                status = 0;
	UCHAR                 ch = 0;
	UCHAR                 arb_type = 0;
	T_DD_XDMAC_CTRL       dma_ctrl_trns;
	T_DD_XDMAC_COMMON     dma_common;
	UCHAR                 protect_code_src = 0;
	UCHAR                 protect_code_dst = 0;
	USHORT                xdmac_status = 0;
	USHORT                transfer_status = 0;
	USHORT                interrupt_status = 0;
	T_DD_XDMAC_TRNS_SIZE  dma_trns_size;
	ULONG                 trns_size = 0;
	ULONG                 src_addr = 0;
	ULONG                 dst_addr = 0;
	
	switch (num) {
		// TEST ID 01-01-01
		case 1:
			memset((void*)D_XDMAC_AUTO_TEST_ADR_DST, 0x00, D_XDMAC_AUTO_TEST_SIZE);
			DDIM_User_L1l2cache_Clean_Flush_All();
			ch = 0;
			
			arb_type                    = D_DD_XDMAC_XDACS_CP_ROTATED;		// rotate
			wait_mode                   = D_DD_XDMAC_WAITMODE_CPU;
			
			dma_common.common_config.bit.CP = arb_type;
			dma_common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
			
			dma_ctrl_trns.trns_size       = D_XDMAC_AUTO_TEST_SIZE;
			dma_ctrl_trns.src_addr        = D_XDMAC_AUTO_TEST_ADR_SRC;
			dma_ctrl_trns.dst_addr        = D_XDMAC_AUTO_TEST_ADR_DST;
			dma_ctrl_trns.int_handler     = NULL;
			
			dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_BYTE;
			dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_BYTE;
			
			dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMAC_XDSAC_SRL_DISABLE;
			dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMAC_XDSAC_SAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMAC_XDSAC_SBL_LENGTH_16;
			dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMAC_XDDAC_DRL_DISABLE;
			dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMAC_XDDAC_DAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMAC_XDDAC_DBL_LENGTH_16;
			
			if (wait_mode == D_DD_XDMAC_WAITMODE_EVENT) {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_ENABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_ENABLE;
			}
			else {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_DISABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_DISABLE;
			}
			
			dma_ctrl_trns.config_1.bit.AT = D_DD_XDMAC_XDDES_AT_SOURCE;
			dma_ctrl_trns.config_1.bit.BR = D_DD_XDMAC_XDDES_BR_DISABLE;
			dma_ctrl_trns.config_1.bit.BT = D_DD_XDMAC_XDDES_BT_ENABLE;
			dma_ctrl_trns.config_1.bit.SA = D_DD_XDMAC_XDDES_SA_DISABLE;
			dma_ctrl_trns.config_1.bit.TF = D_DD_XDMAC_XDDES_TF_SOFT;
			dma_ctrl_trns.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_ENABLE;
			dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_INVALID;
			dma_ctrl_trns.config_2.bit.DCA = 0;
				
			ret = Dd_XDMAC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Open ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			Ddim_Print(("XDMAC Dd_XDMAC_Open done\n"));
			
			ret = Dd_XDMAC_Ctrl_Common(&dma_common);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			Ddim_Print(("XDMAC Dd_XDMAC_Ctrl_Common done\n"));
			
			while (1) {
				ret = Dd_XDMAC_Ctrl_Trns(ch, &dma_ctrl_trns);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Ctrl Setting ERR : ret=0x%x\n", ret));
					break;
				}
				
				Ddim_Print(("XDMAC Dd_XDMAC_Ctrl_Trns done\n"));
				
				ret = Dd_XDMAC_Start_Sync(ch, &status, wait_mode);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("Dd_XDMAC_Start_Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
				}
				else {
					Ddim_Print(("Dd_XDMAC_Start_Sync. Complete! : ret=0x%x, status=0x%x\n", ret, status));
				}
				break;
			}
			
			Ddim_Print(("XDMAC Dd_XDMAC_Start_Sync done\n"));
			
			ret = Dd_XDMAC_Close(ch);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Close ERR : ret=0x%x\n", ret));
			}
			
			Ddim_Print(("Result = %d\n", memcmp((void*)D_XDMAC_AUTO_TEST_ADR_SRC, (void*)D_XDMAC_AUTO_TEST_ADR_DST, D_XDMAC_AUTO_TEST_SIZE)));
			Ddim_Print(("XDMAC Dd_XDMAC_Close done\n"));
			
			break;
			
		// TEST ID 01-01-02
		case 2:
			memset((void*)D_XDMAC_AUTO_TEST_ADR_DST, 0x00, D_XDMAC_AUTO_TEST_SIZE);
			DDIM_User_L1l2cache_Clean_Flush_All();
			ch = 1;
			
			arb_type                    = D_DD_XDMAC_XDACS_CP_FIXED;		// rotate
			wait_mode                   = D_DD_XDMAC_WAITMODE_EVENT;
			
			dma_common.common_config.bit.CP = arb_type;
			dma_common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
			
			dma_ctrl_trns.trns_size       = D_XDMAC_AUTO_TEST_SIZE;
			dma_ctrl_trns.src_addr        = D_XDMAC_AUTO_TEST_ADR_SRC;
			dma_ctrl_trns.dst_addr        = D_XDMAC_AUTO_TEST_ADR_DST;
			dma_ctrl_trns.int_handler     = NULL;
			
			dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_HALFWORD;
			dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_HALFWORD;
			
			dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMAC_XDSAC_SRL_DISABLE;
			dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMAC_XDSAC_SAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMAC_XDSAC_SBL_LENGTH_16;
			dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMAC_XDDAC_DRL_DISABLE;
			dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMAC_XDDAC_DAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMAC_XDDAC_DBL_LENGTH_16;
			
			if (wait_mode == D_DD_XDMAC_WAITMODE_EVENT) {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_ENABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_ENABLE;
			}
			else {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_DISABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_DISABLE;
			}
			
			dma_ctrl_trns.config_1.bit.AT = D_DD_XDMAC_XDDES_AT_SOURCE;
			dma_ctrl_trns.config_1.bit.BR = D_DD_XDMAC_XDDES_BR_DISABLE;
			dma_ctrl_trns.config_1.bit.BT = D_DD_XDMAC_XDDES_BT_DISABLE;
			dma_ctrl_trns.config_1.bit.SA = D_DD_XDMAC_XDDES_SA_DISABLE;
			dma_ctrl_trns.config_1.bit.TF = D_DD_XDMAC_XDDES_TF_SOFT;
			dma_ctrl_trns.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_ENABLE;
			dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_INVALID;
			dma_ctrl_trns.config_2.bit.DCA = 0;
			
			ret = Dd_XDMAC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Open ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			ret = Dd_XDMAC_Ctrl_Common(&dma_common);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			while (1) {
				ret = Dd_XDMAC_Ctrl_Trns(ch, &dma_ctrl_trns);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Ctrl Setting ERR : ret=0x%x\n", ret));
					break;
				}
				ret = Dd_XDMAC_Start_Sync(ch, &status, wait_mode);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
				}
				break;
			}
			
			ret = Dd_XDMAC_Close(ch);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Close ERR : ret=0x%x\n", ret));
			}
			Ddim_Print(("Result = %d\n", memcmp((void*)D_XDMAC_AUTO_TEST_ADR_SRC, (void*)D_XDMAC_AUTO_TEST_ADR_DST, D_XDMAC_AUTO_TEST_SIZE)));
			break;
			
		// TEST ID 01-01-03
		case 3:
			memset((void*)D_XDMAC_AUTO_TEST_ADR_DST, 0x00, D_XDMAC_AUTO_TEST_SIZE);
			DDIM_User_L1l2cache_Clean_Flush_All();
			ch = 2;
			
			arb_type                    = D_DD_XDMAC_XDACS_CP_FIXED;		// rotate
			wait_mode                   = D_DD_XDMAC_WAITMODE_CPU;
			
			dma_common.common_config.bit.CP = arb_type;
			dma_common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
			
			dma_ctrl_trns.trns_size       = D_XDMAC_AUTO_TEST_SIZE;
			dma_ctrl_trns.src_addr        = D_XDMAC_AUTO_TEST_ADR_SRC;
			dma_ctrl_trns.dst_addr        = D_XDMAC_AUTO_TEST_ADR_DST;
			dma_ctrl_trns.int_handler     = ct_xdmac_callback;
			
			dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_WORD;
			dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_WORD;
			
			dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMAC_XDSAC_SRL_DISABLE;
			dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMAC_XDSAC_SAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMAC_XDSAC_SBL_LENGTH_16;
			dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMAC_XDDAC_DRL_DISABLE;
			dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMAC_XDDAC_DAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMAC_XDDAC_DBL_LENGTH_16;
			
			if (wait_mode == D_DD_XDMAC_WAITMODE_EVENT) {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_ENABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_ENABLE;
			}
			else {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_DISABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_DISABLE;
			}
			
			dma_ctrl_trns.config_1.bit.AT = D_DD_XDMAC_XDDES_AT_SOURCE;
			dma_ctrl_trns.config_1.bit.BR = D_DD_XDMAC_XDDES_BR_DISABLE;
			dma_ctrl_trns.config_1.bit.BT = D_DD_XDMAC_XDDES_BT_ENABLE;
			dma_ctrl_trns.config_1.bit.SA = D_DD_XDMAC_XDDES_SA_DISABLE;
			dma_ctrl_trns.config_1.bit.TF = D_DD_XDMAC_XDDES_TF_SOFT;
			dma_ctrl_trns.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_ENABLE;
			dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_INVALID;
			dma_ctrl_trns.config_2.bit.DCA = 0;
			
			ret = Dd_XDMAC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Open ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			ret = Dd_XDMAC_Ctrl_Common(&dma_common);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			while (1) {
				ret = Dd_XDMAC_Ctrl_Trns(ch, &dma_ctrl_trns);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Ctrl Setting ERR : ret=0x%x\n", ret));
					break;
				}
				ret = Dd_XDMAC_Start_Async(ch);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
				}
				ret = Dd_XDMAC_Wait_End(ch, &status, wait_mode);
				if (ret != 0) {
					Ddim_Print(("DMA Util Copy Sync. Wait_End ERR : ret=0x%x, status=0x%x\n", ret, status));
					break;
				}
				break;
			}
			
			ret = Dd_XDMAC_Close(ch);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Close ERR : ret=0x%x\n", ret));
			}
			
			Ddim_Print(("Result = %d\n", memcmp((void*)D_XDMAC_AUTO_TEST_ADR_SRC, (void*)D_XDMAC_AUTO_TEST_ADR_DST, D_XDMAC_AUTO_TEST_SIZE)));
			break;
			
		// TEST ID 01-01-04
		case 4:
			memset((void*)D_XDMAC_AUTO_TEST_ADR_DST, 0x00, D_XDMAC_AUTO_TEST_SIZE);
			DDIM_User_L1l2cache_Clean_Flush_All();
			ch = 3;
			
			arb_type                    = D_DD_XDMAC_XDACS_CP_FIXED;		// rotate
			wait_mode                   = D_DD_XDMAC_WAITMODE_EVENT;
			
			dma_common.common_config.bit.CP = arb_type;
			dma_common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
			
			dma_ctrl_trns.trns_size       = D_XDMAC_AUTO_TEST_SIZE;
			dma_ctrl_trns.src_addr        = D_XDMAC_AUTO_TEST_ADR_SRC;
			dma_ctrl_trns.dst_addr        = D_XDMAC_AUTO_TEST_ADR_DST;
			dma_ctrl_trns.int_handler     = ct_xdmac_callback;
			
			dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_DOUBLEWORD;
			dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_DOUBLEWORD;
			
			dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMAC_XDSAC_SRL_DISABLE;
			dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMAC_XDSAC_SAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMAC_XDSAC_SBL_LENGTH_16;
			dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMAC_XDDAC_DRL_DISABLE;
			dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMAC_XDDAC_DAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMAC_XDDAC_DBL_LENGTH_16;
			
			if (wait_mode == D_DD_XDMAC_WAITMODE_EVENT) {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_ENABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_ENABLE;
			}
			else {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_DISABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_DISABLE;
			}
			
			dma_ctrl_trns.config_1.bit.AT = D_DD_XDMAC_XDDES_AT_SOURCE;
			dma_ctrl_trns.config_1.bit.BR = D_DD_XDMAC_XDDES_BR_DISABLE;
			dma_ctrl_trns.config_1.bit.BT = D_DD_XDMAC_XDDES_BT_DISABLE;
			dma_ctrl_trns.config_1.bit.SA = D_DD_XDMAC_XDDES_SA_DISABLE;
			dma_ctrl_trns.config_1.bit.TF = D_DD_XDMAC_XDDES_TF_SOFT;
			dma_ctrl_trns.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_ENABLE;
			dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_INVALID;
			dma_ctrl_trns.config_2.bit.DCA = 0;
				
			ret = Dd_XDMAC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Open ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			ret = Dd_XDMAC_Ctrl_Common(&dma_common);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			while (1) {
				ret = Dd_XDMAC_Ctrl_Trns(ch, &dma_ctrl_trns);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Ctrl Setting ERR : ret=0x%x\n", ret));
					break;
				}
				ret = Dd_XDMAC_Start_Async(ch);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
				}
				ret = Dd_XDMAC_Wait_End(ch, &status, wait_mode);
				if (ret != 0) {
					Ddim_Print(("DMA Util Copy Sync. Wait_End ERR : ret=0x%x, status=0x%x\n", ret, status));
					break;
				}
				break;
			}
			
			ret = Dd_XDMAC_Close(ch);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Close ERR : ret=0x%x\n", ret));
			}
						
			Ddim_Print(("Result = %d\n", memcmp((void*)D_XDMAC_AUTO_TEST_ADR_SRC, (void*)D_XDMAC_AUTO_TEST_ADR_DST, D_XDMAC_AUTO_TEST_SIZE)));
			break;
			
		// TEST ID 01-02-03
		case 5:
			ch = 0;
			
			arb_type                    = D_DD_XDMAC_XDACS_CP_ROTATED;		// rotate
			wait_mode                   = D_DD_XDMAC_WAITMODE_CPU;
			
			dma_common.common_config.bit.CP = arb_type;
			dma_common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
			
			dma_ctrl_trns.trns_size       = 1;
			dma_ctrl_trns.src_addr        = 0;
			dma_ctrl_trns.dst_addr        = 0;
			dma_ctrl_trns.int_handler     = NULL;
			
			dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_BYTE;
			dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_BYTE;
			
			dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMAC_XDSAC_SRL_DISABLE;
			dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMAC_XDSAC_SAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMAC_XDSAC_SBL_LENGTH_16;
			dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMAC_XDDAC_DRL_DISABLE;
			dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMAC_XDDAC_DAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMAC_XDDAC_DBL_LENGTH_16;
			
			dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_DISABLE;
			dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_DISABLE;
			
			dma_ctrl_trns.config_1.bit.AT = D_DD_XDMAC_XDDES_AT_SOURCE;
			dma_ctrl_trns.config_1.bit.BR = D_DD_XDMAC_XDDES_BR_DISABLE;
			dma_ctrl_trns.config_1.bit.BT = D_DD_XDMAC_XDDES_BT_DISABLE;
			dma_ctrl_trns.config_1.bit.SA = D_DD_XDMAC_XDDES_SA_DISABLE;
			dma_ctrl_trns.config_1.bit.TF = D_DD_XDMAC_XDDES_TF_NONE;
			dma_ctrl_trns.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_DISABLE;
			dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_INVALID;
			dma_ctrl_trns.config_2.bit.DCA = 0;
				
			ret = Dd_XDMAC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Open ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			ret = Dd_XDMAC_Ctrl_Common(&dma_common);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			while (1) {
				ret = Dd_XDMAC_Ctrl_Trns(ch, &dma_ctrl_trns);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Ctrl Setting ERR : ret=0x%x\n", ret));
					break;
				}
				ret = Dd_XDMAC_Start_Sync(ch, &status, wait_mode);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
				}
				break;
			}
			
			ret = Dd_XDMAC_Close(ch);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Close ERR : ret=0x%x\n", ret));
			}
			
			break;

		// TEST ID 01-02-04
		case 6:
			ch = 0;
			
			arb_type                    = D_DD_XDMAC_XDACS_CP_ROTATED;		// rotate
			wait_mode                   = D_DD_XDMAC_WAITMODE_CPU;
			
			dma_common.common_config.bit.CP = arb_type;
			dma_common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
			
			dma_ctrl_trns.trns_size       = 0xffffffff;
			dma_ctrl_trns.src_addr        = 0xffffffff;
			dma_ctrl_trns.dst_addr        = 0xffffffff;
			dma_ctrl_trns.int_handler     = NULL;
			
			dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_DOUBLEWORD;
			dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_DOUBLEWORD;
			
			dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMAC_XDSAC_SRL_ENABLE;
			dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMAC_XDSAC_SAF_FIX;
			dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMAC_XDSAC_SBL_LENGTH_16;
			dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMAC_XDDAC_DRL_ENABLE;
			dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMAC_XDDAC_DAF_FIX;
			dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMAC_XDDAC_DBL_LENGTH_16;
			
			dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_ENABLE;
			dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_ENABLE;
			
			dma_ctrl_trns.config_1.bit.AT = D_DD_XDMAC_XDDES_AT_DESTINATION;
			dma_ctrl_trns.config_1.bit.BR = D_DD_XDMAC_XDDES_BR_ENABLE;
			dma_ctrl_trns.config_1.bit.BT = D_DD_XDMAC_XDDES_BT_ENABLE;
			dma_ctrl_trns.config_1.bit.SA = D_DD_XDMAC_XDDES_SA_ENABLE;
			dma_ctrl_trns.config_1.bit.TF = D_DD_XDMAC_XDDES_TF_IDREQ_13;
			dma_ctrl_trns.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_ENABLE;
			dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_VALID;
			dma_ctrl_trns.config_2.bit.DCA = 0x0fffffff;
				
			ret = Dd_XDMAC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Open ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			ret = Dd_XDMAC_Ctrl_Common(&dma_common);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			while (1) {
				ret = Dd_XDMAC_Ctrl_Trns(ch, &dma_ctrl_trns);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Ctrl Setting ERR : ret=0x%x\n", ret));
					break;
				}
				ret = Dd_XDMAC_Start_Sync(ch, &status, wait_mode);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
				}
				break;
			}
			
			ret = Dd_XDMAC_Close(ch);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Close ERR : ret=0x%x\n", ret));
			}
			
			break;
			
		// TEST ID 01-02-05
		case 7:
			ch = 0;
			protect_code_src = 0x0;
			protect_code_dst = 0x0;
			
			ret = Dd_XDMAC_Set_Source_Protect(ch,protect_code_src);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Dd_XDMAC_Set_Source_Protect ERR : ret=0x%x\n", ret));
				return ret;
			}
			ret = Dd_XDMAC_Set_Destination_Protect(ch,protect_code_dst);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Dd_XDMAC_Set_Destination_Protect ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			break;
			
		// TEST ID 01-02-06
		case 8:
			ch = 0;
			protect_code_src = 0x3;
			protect_code_dst = 0x3;
			
			ret = Dd_XDMAC_Set_Source_Protect(ch,protect_code_src);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Dd_XDMAC_Set_Source_Protect ERR : ret=0x%x\n", ret));
				return ret;
			}
			ret = Dd_XDMAC_Set_Destination_Protect(ch,protect_code_dst);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Dd_XDMAC_Set_Destination_Protect ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			break;
			
		// TEST ID 01-02-07
		case 9:
			ch = 2;
			
			arb_type                    = D_DD_XDMAC_XDACS_CP_FIXED;		// rotate
			wait_mode                   = D_DD_XDMAC_WAITMODE_CPU;
			
			dma_common.common_config.bit.CP = arb_type;
			dma_common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
			
			dma_ctrl_trns.trns_size       = D_XDMAC_AUTO_TEST_SIZE;
			dma_ctrl_trns.src_addr        = D_XDMAC_AUTO_TEST_ADR_SRC;
			dma_ctrl_trns.dst_addr        = D_XDMAC_AUTO_TEST_ADR_DST;
			dma_ctrl_trns.int_handler     = ct_xdmac_callback;
			
			dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_WORD;
			dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_WORD;
			
			dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMAC_XDSAC_SRL_DISABLE;
			dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMAC_XDSAC_SAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMAC_XDSAC_SBL_LENGTH_16;
			dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMAC_XDDAC_DRL_DISABLE;
			dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMAC_XDDAC_DAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMAC_XDDAC_DBL_LENGTH_16;
			
			if (wait_mode == D_DD_XDMAC_WAITMODE_EVENT) {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_ENABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_ENABLE;
			}
			else {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_DISABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_DISABLE;
			}
			
			dma_ctrl_trns.config_1.bit.AT = D_DD_XDMAC_XDDES_AT_SOURCE;
			dma_ctrl_trns.config_1.bit.BR = D_DD_XDMAC_XDDES_BR_DISABLE;
			dma_ctrl_trns.config_1.bit.BT = D_DD_XDMAC_XDDES_BT_ENABLE;
			dma_ctrl_trns.config_1.bit.SA = D_DD_XDMAC_XDDES_SA_DISABLE;
			dma_ctrl_trns.config_1.bit.TF = D_DD_XDMAC_XDDES_TF_SOFT;
			dma_ctrl_trns.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_ENABLE;
			dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_INVALID;
			dma_ctrl_trns.config_2.bit.DCA = 0;
			
			ret = Dd_XDMAC_Ctrl_Common(&dma_common);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			ret = Dd_XDMAC_Get_Status(ch,&xdmac_status,&transfer_status,&interrupt_status);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Dd_XDMAC_Get_Status ERR : ret=0x%x\n", ret));
				return ret;
			}
			Ddim_Print(("xdmac_status=%d\n", xdmac_status));
			Ddim_Print(("transfer_status=%d\n", transfer_status));
			Ddim_Print(("interrupt_status=%d\n", interrupt_status));
			
			break;
			
		// TEST ID 01-02-08
		case 10:
			ch = 3;
			
			arb_type                    = D_DD_XDMAC_XDACS_CP_FIXED;		// rotate
			wait_mode                   = D_DD_XDMAC_WAITMODE_EVENT;
			
			dma_common.common_config.bit.CP = arb_type;
			dma_common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
			
			dma_ctrl_trns.trns_size       = D_XDMAC_AUTO_TEST_SIZE;
			dma_ctrl_trns.src_addr        = D_XDMAC_AUTO_TEST_ADR_SRC;
			dma_ctrl_trns.dst_addr        = D_XDMAC_AUTO_TEST_ADR_DST;
			dma_ctrl_trns.int_handler     = ct_xdmac_callback;
			
			dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_DOUBLEWORD;
			dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_DOUBLEWORD;
			
			dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMAC_XDSAC_SRL_DISABLE;
			dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMAC_XDSAC_SAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMAC_XDSAC_SBL_LENGTH_16;
			dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMAC_XDDAC_DRL_DISABLE;
			dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMAC_XDDAC_DAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMAC_XDDAC_DBL_LENGTH_16;
			
			if (wait_mode == D_DD_XDMAC_WAITMODE_EVENT) {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_ENABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_ENABLE;
			}
			else {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_DISABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_DISABLE;
			}
			
			dma_ctrl_trns.config_1.bit.AT = D_DD_XDMAC_XDDES_AT_SOURCE;
			dma_ctrl_trns.config_1.bit.BR = D_DD_XDMAC_XDDES_BR_DISABLE;
			dma_ctrl_trns.config_1.bit.BT = D_DD_XDMAC_XDDES_BT_DISABLE;
			dma_ctrl_trns.config_1.bit.SA = D_DD_XDMAC_XDDES_SA_DISABLE;
			dma_ctrl_trns.config_1.bit.TF = D_DD_XDMAC_XDDES_TF_SOFT;
			dma_ctrl_trns.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_ENABLE;
			dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_INVALID;
			dma_ctrl_trns.config_2.bit.DCA = 0;
			
			ret = Dd_XDMAC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Open ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			ret = Dd_XDMAC_Ctrl_Common(&dma_common);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			while (1) {
				ret = Dd_XDMAC_Ctrl_Trns(ch, &dma_ctrl_trns);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Ctrl Setting ERR : ret=0x%x\n", ret));
					break;
				}
				ret = Dd_XDMAC_Start_Async(ch);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
				}
				ret = Dd_XDMAC_Get_Status(ch,&xdmac_status,&transfer_status,&interrupt_status);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Dd_XDMAC_Get_Status ERR : ret=0x%x\n", ret));
					return ret;
				}
				ret = Dd_XDMAC_Wait_End(ch, &status, wait_mode);
				if (ret != 0) {
					Ddim_Print(("DMA Util Copy Sync. Wait_End ERR : ret=0x%x, status=0x%x\n", ret, status));
					break;
				}
				break;
			}
			
			ret = Dd_XDMAC_Close(ch);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Close ERR : ret=0x%x\n", ret));
			}
			
			Ddim_Print(("xdmac_status=%d\n", xdmac_status));
			Ddim_Print(("transfer_status=%d\n", transfer_status));
			Ddim_Print(("interrupt_status=%d\n", interrupt_status));
			
			break;
			
		// TEST ID 01-02-09
		case 11:
			ch = 0;
			
			dma_trns_size.trns_size = 0x12345678;
			dma_trns_size.src_addr = D_XDMAC_AUTO_TEST_ADR_SRC;
			dma_trns_size.dst_addr = D_XDMAC_AUTO_TEST_ADR_DST;
			
			ret = Dd_XDMAC_Set_Trns_Size(ch,&dma_trns_size);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Dd_XDMAC_Get_Status ERR : ret=0x%x\n", ret));
				return ret;
			}
			trns_size = Dd_XDMAC_Get_Trns_Size(ch);
			Ddim_Print(("Trns_Size==0x%x\n", (unsigned int)trns_size));
			
			break;
			
		// TEST ID 01-02-10
		case 12:
			ch = 0;
			
			dma_trns_size.trns_size = 0x12345678;
			dma_trns_size.src_addr = 0xFFFFFFFF;
			dma_trns_size.dst_addr = 0xFFFFFFFF;
			
			ret = Dd_XDMAC_Set_Trns_Size(ch,&dma_trns_size);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Dd_XDMAC_Get_Status ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			src_addr = Dd_XDMAC_Get_Src_Addr(ch);
			dst_addr = Dd_XDMAC_Get_Dst_Addr(ch);
			Ddim_Print(("Src_Addr==0x%x\n", (unsigned int)src_addr));
			Ddim_Print(("Dst_Addr==0x%x\n", (unsigned int)dst_addr));
			
			break;
			
		// TEST ID 01-02-11
		case 13:
			Dd_XDMAC_Stop_All_Ch();
			
			break;
			
		// TEST ID 01-02-12
		case 14:
			ch = 0;
			
			ret = Dd_XDMAC_Clear_Status(ch);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Dd_XDMAC_Clear_Status ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			break;
			
		// TEST ID 01-02-11
		case 15:
			Dd_XDMAC_Copy_SDRAM_Sync(0, D_XDMAC_AUTO_TEST_ADR_SRC, D_XDMAC_AUTO_TEST_ADR_DST, D_XDMAC_AUTO_TEST_SIZE, D_DD_XDMAC_WAITMODE_EVENT);
			break;
			
		case 16:
			Dd_XDMAC_Copy_SDRAM_Async(0, D_XDMAC_AUTO_TEST_ADR_SRC, D_XDMAC_AUTO_TEST_ADR_DST, D_XDMAC_AUTO_TEST_SIZE, NULL);
			ret = Dd_XDMAC_Wait_End(0, &status, D_DD_XDMAC_WAITMODE_EVENT);
			if (ret != 0) {
				Ddim_Print(("DMA Util Copy Sync. Wait_End ERR : ret=0x%x, status=0x%x\n", ret, status));
				break;
			}
			break;
			
		case 17:
			memset((void*)D_XDMAC_AUTO_TEST_ADR_DST, 0x00, D_XDMAC_AUTO_TEST_SIZE);
			DDIM_User_L1l2cache_Clean_Flush_All();
			ch = 1;
			
			arb_type                    = D_DD_XDMAC_XDACS_CP_FIXED;		// rotate
			wait_mode                   = D_DD_XDMAC_WAITMODE_EVENT;
			
			dma_common.common_config.bit.CP = arb_type;
			dma_common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
			
			dma_ctrl_trns.trns_size       = D_XDMAC_AUTO_TEST_SIZE;
			dma_ctrl_trns.src_addr        = D_XDMAC_AUTO_TEST_INTMEM;
			dma_ctrl_trns.dst_addr        = D_XDMAC_AUTO_TEST_ADR_DST;
			dma_ctrl_trns.int_handler     = NULL;
			
			dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_HALFWORD;
			dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_HALFWORD;
			
			dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMAC_XDSAC_SRL_DISABLE;
			dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMAC_XDSAC_SAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMAC_XDSAC_SBL_LENGTH_16;
			dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMAC_XDDAC_DRL_DISABLE;
			dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMAC_XDDAC_DAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMAC_XDDAC_DBL_LENGTH_16;
			
			if (wait_mode == D_DD_XDMAC_WAITMODE_EVENT) {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_ENABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_ENABLE;
			}
			else {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_DISABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_DISABLE;
			}
			
			dma_ctrl_trns.config_1.bit.AT = D_DD_XDMAC_XDDES_AT_SOURCE;
			dma_ctrl_trns.config_1.bit.BR = D_DD_XDMAC_XDDES_BR_DISABLE;
			dma_ctrl_trns.config_1.bit.BT = D_DD_XDMAC_XDDES_BT_DISABLE;
			dma_ctrl_trns.config_1.bit.SA = D_DD_XDMAC_XDDES_SA_DISABLE;
			dma_ctrl_trns.config_1.bit.TF = D_DD_XDMAC_XDDES_TF_SOFT;
			dma_ctrl_trns.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_ENABLE;
			dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_INVALID;
			dma_ctrl_trns.config_2.bit.DCA = 0;
			
			ret = Dd_XDMAC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Open ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			ret = Dd_XDMAC_Ctrl_Common(&dma_common);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			while (1) {
				ret = Dd_XDMAC_Ctrl_Trns(ch, &dma_ctrl_trns);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Ctrl Setting ERR : ret=0x%x\n", ret));
					break;
				}
				ret = Dd_XDMAC_Start_Sync(ch, &status, wait_mode);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
				}
				break;
			}
			
			ret = Dd_XDMAC_Close(ch);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Close ERR : ret=0x%x\n", ret));
			}
			Ddim_Print(("Result = %d\n", memcmp((void*)D_XDMAC_AUTO_TEST_INTMEM, (void*)D_XDMAC_AUTO_TEST_ADR_DST, D_XDMAC_AUTO_TEST_SIZE)));
			break;

		case 18:
			memset((void*)D_XDMAC_AUTO_TEST_ADR_DST, 0x00, D_XDMAC_AUTO_TEST_SIZE);
			DDIM_User_L1l2cache_Clean_Flush_All();
			ch = 1;
			
			arb_type                    = D_DD_XDMAC_XDACS_CP_FIXED;		// rotate
			wait_mode                   = D_DD_XDMAC_WAITMODE_EVENT;
			
			dma_common.common_config.bit.CP = arb_type;
			dma_common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
			
			dma_ctrl_trns.trns_size       = D_XDMAC_AUTO_TEST_SIZE;
			dma_ctrl_trns.src_addr        = D_XDMAC_AUTO_TEST_ADR_SRC;
			dma_ctrl_trns.dst_addr        = D_XDMAC_AUTO_TEST_INTMEM;
			dma_ctrl_trns.int_handler     = NULL;
			
			dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_HALFWORD;
			dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_HALFWORD;
			
			dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMAC_XDSAC_SRL_DISABLE;
			dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMAC_XDSAC_SAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMAC_XDSAC_SBL_LENGTH_16;
			dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMAC_XDDAC_DRL_DISABLE;
			dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMAC_XDDAC_DAF_NOTFIX;
			dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMAC_XDDAC_DBL_LENGTH_16;
			
			if (wait_mode == D_DD_XDMAC_WAITMODE_EVENT) {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_ENABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_ENABLE;
			}
			else {
				dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_DISABLE;
				dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_DISABLE;
			}
			
			dma_ctrl_trns.config_1.bit.AT = D_DD_XDMAC_XDDES_AT_SOURCE;
			dma_ctrl_trns.config_1.bit.BR = D_DD_XDMAC_XDDES_BR_DISABLE;
			dma_ctrl_trns.config_1.bit.BT = D_DD_XDMAC_XDDES_BT_DISABLE;
			dma_ctrl_trns.config_1.bit.SA = D_DD_XDMAC_XDDES_SA_DISABLE;
			dma_ctrl_trns.config_1.bit.TF = D_DD_XDMAC_XDDES_TF_SOFT;
			dma_ctrl_trns.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_ENABLE;
			dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_INVALID;
			dma_ctrl_trns.config_2.bit.DCA = 0;
			
			ret = Dd_XDMAC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Open ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			ret = Dd_XDMAC_Ctrl_Common(&dma_common);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
				return ret;
			}
			
			while (1) {
				ret = Dd_XDMAC_Ctrl_Trns(ch, &dma_ctrl_trns);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Ctrl Setting ERR : ret=0x%x\n", ret));
					break;
				}
				ret = Dd_XDMAC_Start_Sync(ch, &status, wait_mode);
				if (ret != D_DDIM_OK) {
					Ddim_Print(("DMA Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
				}
				break;
			}
			
			ret = Dd_XDMAC_Close(ch);
			if (ret != D_DDIM_OK) {
				Ddim_Print(("DMA Util Copy Sync. Close ERR : ret=0x%x\n", ret));
			}
			Ddim_Print(("Result = %d\n", memcmp((void*)D_XDMAC_AUTO_TEST_ADR_SRC, (void*)D_XDMAC_AUTO_TEST_INTMEM, D_XDMAC_AUTO_TEST_SIZE)));
			break;
		default:
			ret = -1;
			break;
	}
	return ret;
}

static void ct_xdmac_result_out(INT32 expect, INT32 ret)
{
	printf("\tExpect = 0x%x\n", expect);
	printf("\tReturn = 0x%x\n", ret);
	
	if(expect == ret){
		printf("\t--- OK\n");
	}else{
		printf("\t--- NG\n");
	}
}

static VOID ct_xdmac_test( VOID )
{	
	INT32 ret;
	T_DD_XDMAC_CTRL dma_ctrl_trans;
	T_DD_XDMAC_TRNS_SIZE dma_trns_size;
	USHORT status;
	VP_CALLBACK intHandler = 0;
	
	printf("<Dd_XDMAC>\n");
	printf("\n");
	
	printf("[Dd_XDMAC_Open]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Open(D_DD_XDMAC_CH_NUM_MAX, D_DDIM_USER_SEM_WAIT_FEVR);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("tmout < D_DDIM_USER_SEM_WAIT_FEVR:\n");
	ret = Dd_XDMAC_Open(0, D_DDIM_USER_SEM_WAIT_FEVR - 1);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("tmout = D_DDIM_USER_SEM_WAIT_POL, pol_sem = D_DDIM_USER_E_TMOUT:\n");
	ret = Dd_XDMAC_Open(0, D_DDIM_USER_SEM_WAIT_POL);
	ret = Dd_XDMAC_Open(0, D_DDIM_USER_SEM_WAIT_POL);
	ct_xdmac_result_out(D_DD_XDMAC_SEM_TIMEOUT, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Close]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Close(D_DD_XDMAC_CH_NUM_MAX);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Ctrl_Common]\n");
	printf("dma_common is NULL:\n");
	ret = Dd_XDMAC_Ctrl_Common(NULL);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Ctrl_Trns]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Ctrl_Trns(D_DD_XDMAC_CH_NUM_MAX, NULL);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("dma_ctrl_trans is NULL:\n");
	ret = Dd_XDMAC_Ctrl_Trns(0, NULL);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("dma_ctrl_trans->trns_size = 0:\n");
	dma_ctrl_trans.trns_size = 0;
	ret = Dd_XDMAC_Ctrl_Trns(0, &dma_ctrl_trans);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Set_Trns_Size]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Set_Trns_Size(D_DD_XDMAC_CH_NUM_MAX, NULL);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("dma_trns_size is NULL:\n");
	ret = Dd_XDMAC_Set_Trns_Size(0, NULL);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("dma_trns_size->trns_size = 0:\n");
	dma_trns_size.trns_size = 0;
	ret = Dd_XDMAC_Set_Trns_Size(0, &dma_trns_size);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Start_Sync]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Start_Sync(D_DD_XDMAC_CH_NUM_MAX, NULL, D_DD_XDMAC_WAITMODE_CPU);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("[Dd_XDMAC_Start_Sync]\n");
	printf("status is NULL:\n");
	ret = Dd_XDMAC_Start_Sync(0, NULL, D_DD_XDMAC_WAITMODE_CPU);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("wait_mode not in (D_DD_XDMAC_WAITMODE_CPU, D_DD_XDMAC_WAITMODE_EVENT):\n");
	ret = Dd_XDMAC_Start_Sync(0, &status, D_DD_XDMAC_WAITMODE_EVENT + 1);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("wait_mode = D_DD_XDMAC_WAITMODE_EVENT, twai_flg <> D_DDIM_USER_E_TMOUT:\n");
	ret = Dd_XDMAC_Start_Sync(0, &status, D_DD_XDMAC_WAITMODE_EVENT);
	ct_xdmac_result_out(D_DD_XDMAC_SYSTEM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Start_Async]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Start_Async(D_DD_XDMAC_CH_NUM_MAX);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Stop]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Stop(D_DD_XDMAC_CH_NUM_MAX);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Set_Wait_Time]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Set_Wait_Time(D_DD_XDMAC_CH_NUM_MAX, D_DDIM_WAIT_END_FOREVER);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("wait_time < D_DDIM_WAIT_END_FOREVER:\n");
	ret = Dd_XDMAC_Set_Wait_Time(0, D_DDIM_WAIT_END_FOREVER - 1);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Wait_End]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Wait_End(D_DD_XDMAC_CH_NUM_MAX, &status, D_DD_XDMAC_WAITMODE_CPU);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("status is NULL:\n");
	ret = Dd_XDMAC_Wait_End(0, NULL, D_DD_XDMAC_WAITMODE_CPU);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("wait_mode not in (D_DD_XDMAC_WAITMODE_CPU, D_DD_XDMAC_WAITMODE_EVENT):\n");
	ret = Dd_XDMAC_Wait_End(0, &status, D_DD_XDMAC_WAITMODE_EVENT + 1);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Clear_Status]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Clear_Status(D_DD_XDMAC_CH_NUM_MAX);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Get_Status]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Get_Status(D_DD_XDMAC_CH_NUM_MAX, &status, &status, &status);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("XDMAC_status is NULL:\n");
	ret = Dd_XDMAC_Get_Status(0, NULL, &status, &status);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("transfer_status is NULL:\n");
	ret = Dd_XDMAC_Get_Status(0, &status, NULL, &status);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("interrupt_status is NULL:\n");
	ret = Dd_XDMAC_Get_Status(0, &status, &status, NULL);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Get_Trns_Size]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Get_Trns_Size(D_DD_XDMAC_CH_NUM_MAX);
	ct_xdmac_result_out(0, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Get_Src_Addr]\n");
	printf("ch >= D_DD_XDAMC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Get_Src_Addr(D_DD_XDMAC_CH_NUM_MAX);
	ct_xdmac_result_out(0, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Get_Dst_Addr]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Get_Dst_Addr(D_DD_XDMAC_CH_NUM_MAX);
	ct_xdmac_result_out(0, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Set_LowPower]\n");
	printf("lowpower > D_DD_XDMAC_XDACS_LP_ENABLE:\n");
	ret = Dd_XDMAC_Set_LowPower(D_DD_XDMAC_XDACS_LP_ENABLE + 1);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Set_Source_Protect]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Set_Source_Protect(D_DD_XDMAC_CH_NUM_MAX, D_DD_XDMAC_XDDPC_SP_PRIVILEGED_SECURE);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("protect_code > D_DD_xdmac_XDDPC_SP_PRIVILEGED_SECURE:\n");
	ret = Dd_XDMAC_Set_Source_Protect(0, D_DD_XDMAC_XDDPC_SP_PRIVILEGED_SECURE + 1);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Set_Destination_Protect]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Set_Destination_Protect(D_DD_XDMAC_CH_NUM_MAX, D_DD_XDMAC_XDDPC_DP_PRIVILEGED_SECURE);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("protect_code > D_DD_XDMAC_XDDPC_SP_PRIVILEGED_SECURE:\n");
	ret = Dd_XDMAC_Set_Destination_Protect(0, D_DD_XDMAC_XDDPC_DP_PRIVILEGED_SECURE + 1);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Set_Int_Handler]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	Dd_XDMAC_Set_Int_Handler(D_DD_XDMAC_CH_NUM_MAX, NULL);
	printf("\n");
	
	printf("[Dd_XDMAC_Int_Handler]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	Dd_XDMAC_Int_Handler(D_DD_XDMAC_CH_NUM_MAX);
	printf("\n");
	
	printf("[Dd_XDMAC_Copy_SDRAM_Sync]\n");
	printf("ch >= D_DD_xdmac_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Copy_SDRAM_Sync(D_DD_XDMAC_CH_NUM_MAX, 1, 1, 1, D_DD_XDMAC_WAITMODE_EVENT);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("src_addr = 0:\n");
	ret = Dd_XDMAC_Copy_SDRAM_Sync(0, 0, 1, 1, D_DD_XDMAC_WAITMODE_EVENT);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("dst_addr = 0:\n");
	ret = Dd_XDMAC_Copy_SDRAM_Sync(1, 1, 0, 1, D_DD_XDMAC_WAITMODE_EVENT);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("size = 0:\n");
	ret = Dd_XDMAC_Copy_SDRAM_Sync(2, 1, 1, 0, D_DD_XDMAC_WAITMODE_EVENT);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("wait_mode not in (D_DD_XDMAC_WAITMODE_CPU, D_DD_XDMAC_WAITMODE_EVENT):\n");
	ret = Dd_XDMAC_Copy_SDRAM_Sync(3, 1, 1, 1, D_DD_XDMAC_WAITMODE_EVENT + 1);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
	
	printf("[Dd_XDMAC_Copy_SDRAM_Async]\n");
	printf("ch >= D_DD_XDMAC_CH_NUM_MAX:\n");
	ret = Dd_XDMAC_Copy_SDRAM_Async(D_DD_XDMAC_CH_NUM_MAX, 1, 1, 1, intHandler);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("src_addr = 0:\n");
	ret = Dd_XDMAC_Copy_SDRAM_Async(0, 0, 1, 1, intHandler);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("dst_addr = 0:\n");
	ret = Dd_XDMAC_Copy_SDRAM_Async(1, 1, 0, 1, intHandler);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("size = 0:\n");
	ret = Dd_XDMAC_Copy_SDRAM_Async(2, 1, 1, 0, intHandler);
	ct_xdmac_result_out(D_DD_XDMAC_INPUT_PARAM_ERR, ret);
	printf("\n");
}


/**
 * @brief  Command main function for XDMAC test.
 * @param  INT32 argc	:The number of parameters
 *		   CHAR** argv	:The value of parameters
 *
 *	The meaning of parameters
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+-----------------------------------+
 *	| P1              | P2       | P3         | P4       | P5         | P6         | P7       | P8 | P9 | P10 | P11 | Meaning                           |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+-----------------------------------+
 *	| "open"          | ch       | wait_time  |          |            |            |          |    |    |     |     | Dd_XDMAC_Open()                   |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+-----------------------------------+
 *	| "close"         | ch       |            |          |            |            |          |    |    |     |     | Dd_XDMAC_Close()                  |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+-----------------------------------+
 *	| "ctrl_cmm"      | ch       |  "0"       |  "0"     |  "0"       |            |          |    |    |     |     | Dd_XDMAC_Ctrl_Common()            |
 *	|                 |          |  "1"       |  "1"     |  "1"       |            |          |    |    |     |     |                                   |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+-----------------------------------+
 *	| "int_set"       | ch       |            |          |            |            |          |    |    |     |     | Dd_XDMAC_Set_Int_Handler()        |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+-----------------------------------+
 *	| "auto"          | num      |            |          |            |            |          |    |    |     |     | Continuous execution              |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+-----------------------------------+
 *	| "prm_chk"       | 0 or 1   |            |          |            |            |          |    |    |     |     | Check parameter error(XDTBC = 0)  |
 *  |                 |          |            |          |            |            |          |    |    |     |     | 0:Dd_XDMAC_Ctrl_Trns              |
 *  |                 |          |            |          |            |            |          |    |    |     |     | 1:Dd_XDMAC_Set_Trns_Size          |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+-----------------------------------+
 *
 *			ch			:	0          - 3
 *			rc			:	ena / dis
 *			rs			:	ena / dis
 *			rd			:	ena / dis
 *			sp			:	0          - 15
 *			dp			:	0          - 15
 *			size		:	0          - 0xFFFFFFFF
 *			src_addr	:	0          - 0xFFFFFFFF
 *			dst_addr	:	0          - 0xFFFFFFFF
 *			IntHandler  :	Callback Function Pointer
 *			num			:	Test ID
 *
 * @return VOID
 */
VOID Ct_Dd_XDMAC_Main(int argc, char** argv)
{
	INT32				ret;
	UCHAR				ch;
	UCHAR				num;
	INT32				wait_time;
	
	// check number of parameter
	if (argc > 12) {
		Ddim_Print(("The number of input parameters is exceeded!\n"));
		return;
	}
	
	Dd_Top_Set_CLKSTOP2_EXSAX(0);
	
	if (strcmp(argv[1], "open") == 0) {
		/* ch number */
		ch = atoi(argv[2]);
		wait_time = atoi(argv[3]);
		
		ret = Dd_XDMAC_Open(ch, wait_time);
		if (ret == 0) {
			Ddim_Print(("XDMAC Open OK\n"));
		}
		else {
			Ddim_Print(("XDMAC Open ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "close") == 0) {
		/* ch number */
		ch = atoi(argv[2]);
		
		ret = Dd_XDMAC_Close(ch);
		if (ret == 0) {
			Ddim_Print(("XDMAC Close OK\n"));
		}
		else {
			Ddim_Print(("XDMAC Close ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "ctrl_cmm") == 0) {
		Dd_XDMAC_Open(0, D_DDIM_USER_SEM_WAIT_POL);
		
		// Set XE
		if (strcmp(argv[2], "0") == 0) {
			gXDMAC_Common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_DISABLE;
		}
		else if (strcmp(argv[2], "1") == 0) {
			gXDMAC_Common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
		}
		else {
			Ddim_Print(("Parameter ERR. P3 \n"));
			Dd_XDMAC_Close(0);
			return;
		}
		
		// Set CP
		if (strcmp(argv[3], "0") == 0) {
			gXDMAC_Common.common_config.bit.CP = D_DD_XDMAC_XDACS_CP_FIXED;
		}
		else if (strcmp(argv[3], "1") == 0) {
			gXDMAC_Common.common_config.bit.CP = D_DD_XDMAC_XDACS_CP_ROTATED;
		}
		else {
			Ddim_Print(("Parameter ERR. P4 \n"));
			Dd_XDMAC_Close(0);
			return;
		}
		
		ret = Dd_XDMAC_Ctrl_Common(&gXDMAC_Common);
		if (ret == 0) {
			Ddim_Print(("XDMAC Ctrl_Common OK\n"));
		}
		else {
			Ddim_Print(("XDMAC Ctrl_Common ERR. ret=0x%x\n", ret));
		}
		Dd_XDMAC_Close(0);
	}
	else if (strcmp(argv[1], "int_set") == 0){
		/* ch number */
		ch = atoi(argv[2]);
		
		Dd_XDMAC_Set_Int_Handler(ch, ct_xdmac_callback);
	}
	else if (strcmp(argv[1], "auto") == 0) {
		/* ch number */
		num = atoi(argv[2]);
		
		ret = ct_xdmac_process(num);
		if (ret == 0) {
			Ddim_Print(("ct_xdmac_process OK\n"));
		}
		else {
			Ddim_Print(("ct_xdmac_process ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "prm_chk") == 0) {
		T_DD_XDMAC_CTRL    dma_ctrl_trns;
		T_DD_XDMAC_TRNS_SIZE dma_trns_size;
		
		if(strcmp(argv[2], "0") == 0){
			dma_ctrl_trns.trns_size       = 0;
			dma_ctrl_trns.src_addr        = 0;
			dma_ctrl_trns.dst_addr        = 0;
			dma_ctrl_trns.int_handler     = NULL;
			
			ret = Dd_XDMAC_Ctrl_Trns(0, &dma_ctrl_trns);
			if(ret != D_DDIM_OK){
				Ddim_Print(("Dd_XDMAC_Ctrl_Trns ERR : ret=0x%x\n", ret));
			}
		}
		else if(strcmp(argv[2], "1") == 0){
			dma_trns_size.trns_size       = 0;
			dma_trns_size.src_addr        = 0;
			dma_trns_size.dst_addr        = 0;
			
			ret = Dd_XDMAC_Set_Trns_Size(0,&dma_trns_size);
			if(ret != D_DDIM_OK){
				Ddim_Print(("Dd_XDMAC_Set_Trns_Size ERR : ret=0x%x\n", ret));
			}
		}
		else{
			Ddim_Print(("Parameter ERR. P3 \n"));
		}
	}
	else if (strcmp(argv[1], "xdmac_test") == 0) {
		ct_xdmac_test();
	}
	else {
		Ddim_Print(("please check 1st parameter!!\n"));
	}
	return;
}
