/**
 * @file		ct_dd_hdmac0.c
 * @brief		This is ct code for dd_hdmac0
 * @note		None
 * @attention	None
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "stdlib.h"
#include "string.h"
#include "dd_hdmac0.h"
#include "ct_dd_hdmac0.h"
#include "dd_gic.h"
#include "ddim_user_custom.h"
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static T_DD_HDMAC0_CTRL		gHDMAC0_Ctrl[8];
static T_DD_HDMAC0_CTRL_TRNS	gHDMAC0_Ctrl_Trns[8];
static T_DD_HDMAC0_TRNS_SIZE	gHDMAC0_Trns_Size[8];

static VOID ct_hdmac0_callback( VOID )
{
	Ddim_Print(("HDMAC0 Callback\n"));
}

static INT32 ct_hdmac0_set_is( UCHAR ch, CHAR* is ){
	INT32 ret = D_DDIM_OK;

	// Set IS
	if (strcmp(is, "soft") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_SOFT;
	}
	else if (strcmp(is, "dreq_h") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_DREQ_H;
	}
	else if (strcmp(is, "dreq_l") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_DREQ_L;
	}
	else if (strcmp(is, "idreq_0") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_0;
	}
	else if (strcmp(is, "idreq_1") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_1;
	}
	else if (strcmp(is, "idreq_2") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_2;
	}
	else if (strcmp(is, "idreq_3") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_3;
	}
	else if (strcmp(is, "idreq_4") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_4;
	}
	else if (strcmp(is, "idreq_5") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_5;
	}
	else if (strcmp(is, "idreq_6") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_6;
	}
	else if (strcmp(is, "idreq_7") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_7;
	}
	else if (strcmp(is, "idreq_8") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_8;
	}
	else if (strcmp(is, "idreq_9") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_9;
	}
	else if (strcmp(is, "idreq_10") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_10;
	}
	else if (strcmp(is, "idreq_11") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_11;
	}
	else if (strcmp(is, "idreq_12") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_12;
	}
	else if (strcmp(is, "idreq_13") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_13;
	}
	else if (strcmp(is, "idreq_14") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_14;
	}
	else if (strcmp(is, "idreq_15") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_15;
	}
	else if (strcmp(is, "idreq_16") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_16;
	}
	else if (strcmp(is, "idreq_17") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_17;
	}
	else if (strcmp(is, "idreq_18") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_18;
	}
	else if (strcmp(is, "idreq_19") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_19;
	}
	else if (strcmp(is, "idreq_20") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_20;
	}
	else if (strcmp(is, "idreq_21") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_21;
	}
	else if (strcmp(is, "idreq_22") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_22;
	}
	else if (strcmp(is, "idreq_23") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_23;
	}
	else if (strcmp(is, "idreq_24") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_24;
	}
	else if (strcmp(is, "idreq_25") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_25;
	}
	else if (strcmp(is, "idreq_26") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_26;
	}
	else if (strcmp(is, "idreq_27") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_27;
	}
	else if (strcmp(is, "idreq_28") == 0) {
		gHDMAC0_Ctrl_Trns[ch].mode.input_sel = D_DD_HDMAC0_IS_IDREQ_28;
	}
	else {
		Ddim_Print(("Parameter ERR. Input Select \n"));
		ret = D_DDIM_INPUT_PARAM_ERROR;
	}
	return ret;
}

static int ct_hdmac0_process(UCHAR num)
{
#if 1
	INT32                 ret;
	UINT32                wait_mode;
	USHORT                status = 0;
	UCHAR                 ch;
	UCHAR                 arb_type;
	T_DD_HDMAC0_CTRL      hdmac0_ctrl;
	T_DD_HDMAC0_CTRL_TRNS hdmac0_ctrl_trns;

	switch (num) {
		// TEST ID 01-01-01
		case 1:
			memset((void*)D_HDMAC0_AUTO_TEST_ADR_DST, 0x00, D_HDMAC0_AUTO_TEST_SIZE);
			DDIM_User_L1l2cache_Clean_Flush_All();
			ch = 0;
			Dd_GIC_Ctrl(E_DD_GIC_INTID_HDMAC0_CH0_INT,1,D_DD_GIC_PRI30,D_HDMAC0_CPU_TARGET);

			arb_type                    = D_DD_HDMAC0_ARB_ROTATE;		// rotate
			wait_mode                   = D_DD_HDMAC0_WAITMODE_CPU;

			hdmac0_ctrl.config_a.word   = 0;		// clear

			hdmac0_ctrl.config_a.bit.is = D_DD_HDMAC0_IS_SOFT;
			hdmac0_ctrl.config_a.bit.bt = D_DD_HDMAC0_BT_NORMAL;
			hdmac0_ctrl.config_a.bit.bc = 0x0F;
			hdmac0_ctrl.config_a.bit.tc = ((D_HDMAC0_AUTO_TEST_SIZE / 16) - 1);

			hdmac0_ctrl.config_b.word   = 0;		// clear

			hdmac0_ctrl.config_b.bit.ms = D_DD_HDMAC0_MS_BLK;
			hdmac0_ctrl.config_b.bit.tw = D_DD_HDMAC0_TW_BYTE;
			hdmac0_ctrl.config_b.bit.fs = D_DD_HDMAC0_FS_INCR;
			hdmac0_ctrl.config_b.bit.fd = D_DD_HDMAC0_FD_INCR;
			hdmac0_ctrl.config_b.bit.rc = D_DD_HDMAC0_RC_DISABLE;
			hdmac0_ctrl.config_b.bit.rs = D_DD_HDMAC0_RS_DISABLE;
			hdmac0_ctrl.config_b.bit.rd = D_DD_HDMAC0_RD_DISABLE;

			hdmac0_ctrl.src_addr        = D_HDMAC0_AUTO_TEST_ADR_SRC;
			hdmac0_ctrl.dst_addr        = D_HDMAC0_AUTO_TEST_ADR_DST;
			hdmac0_ctrl.int_handler     = 0;

			ret = Dd_HDMAC0_Open(ch, D_DDIM_USER_SEM_WAIT_POL);
			if (ret != 0) {
				Ddim_Print(("HDMAC0 Open ERR\n"));
				return ret;
			}
			while (1) {
				ret = Dd_HDMAC0_Set_Arbitration(arb_type);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Open ERR\n"));
					break;
				}
				
				ret = Dd_HDMAC0_Ctrl_Common(ch, &hdmac0_ctrl);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Ctrl ERR\n"));
					break;
				}
				ret = Dd_HDMAC0_Start_Sync(ch, &status, wait_mode);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Start Sync ERR\n"));
					break;
				}
				break;
			}
			ret = Dd_HDMAC0_Close(ch);
			if (ret != 0) {
				Ddim_Print(("HDMAC0 Close ERR\n"));
			}
			Ddim_Print(("Result = %d\n", memcmp((void*)D_HDMAC0_AUTO_TEST_ADR_SRC, (void*)D_HDMAC0_AUTO_TEST_ADR_DST, D_HDMAC0_AUTO_TEST_SIZE)));
			break;

		// TEST ID 01-01-02
		case 2:
			memset((void*)D_HDMAC0_AUTO_TEST_ADR_DST, 0x00, D_HDMAC0_AUTO_TEST_SIZE);
			DDIM_User_L1l2cache_Clean_Flush_All();
			ch = 1;
			Dd_GIC_Ctrl(E_DD_GIC_INTID_HDMAC0_CH1_INT,1,D_DD_GIC_PRI30,D_HDMAC0_CPU_TARGET);

			arb_type                    = D_DD_HDMAC0_ARB_FIX;		// fix
			wait_mode                   = D_DD_HDMAC0_WAITMODE_EVENT;

			hdmac0_ctrl.config_a.word   = 0;		// clear

			hdmac0_ctrl.config_a.bit.is = D_DD_HDMAC0_IS_SOFT;
			hdmac0_ctrl.config_a.bit.bt = D_DD_HDMAC0_BT_NORMAL;
			hdmac0_ctrl.config_a.bit.bc = 0x0F;
			hdmac0_ctrl.config_a.bit.tc = ((D_HDMAC0_AUTO_TEST_SIZE / 16 / 2) - 1);

			hdmac0_ctrl.config_b.word   = 0;		// clear

			hdmac0_ctrl.config_b.bit.ms = D_DD_HDMAC0_MS_BURST;
			hdmac0_ctrl.config_b.bit.tw = D_DD_HDMAC0_TW_HWORD;
			hdmac0_ctrl.config_b.bit.fs = D_DD_HDMAC0_FS_INCR;
			hdmac0_ctrl.config_b.bit.fd = D_DD_HDMAC0_FD_INCR;
			hdmac0_ctrl.config_b.bit.rc = D_DD_HDMAC0_RC_DISABLE;
			hdmac0_ctrl.config_b.bit.rs = D_DD_HDMAC0_RS_DISABLE;
			hdmac0_ctrl.config_b.bit.rd = D_DD_HDMAC0_RD_DISABLE;

			hdmac0_ctrl.src_addr        = D_HDMAC0_AUTO_TEST_ADR_SRC;
			hdmac0_ctrl.dst_addr        = D_HDMAC0_AUTO_TEST_ADR_DST;
			hdmac0_ctrl.int_handler     = 0;

			ret = Dd_HDMAC0_Open(ch, D_DDIM_USER_SEM_WAIT_POL);
			if (ret != 0) {
				Ddim_Print(("HDMAC0 Open ERR\n"));
				return ret;
			}
			while (1) {
				ret = Dd_HDMAC0_Set_Arbitration(arb_type);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Open ERR\n"));
					break;
				}
				
				ret = Dd_HDMAC0_Ctrl_Common(ch, &hdmac0_ctrl);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Ctrl ERR\n"));
					break;
				}
				ret = Dd_HDMAC0_Start_Sync(ch, &status, wait_mode);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Start Sync ERR\n"));
					break;
				}
				break;
			}
			ret = Dd_HDMAC0_Close(ch);
			if (ret != 0) {
				Ddim_Print(("HDMAC0 Close ERR\n"));
			}
			Ddim_Print(("Result = %d\n", memcmp((void*)D_HDMAC0_AUTO_TEST_ADR_SRC, (void*)D_HDMAC0_AUTO_TEST_ADR_DST, D_HDMAC0_AUTO_TEST_SIZE)));
			break;

		// TEST ID 01-01-03
		case 3:
			memset((void*)D_HDMAC0_AUTO_TEST_ADR_DST, 0x00, D_HDMAC0_AUTO_TEST_SIZE);
			memset((void*)D_HDMAC0_AUTO_TEST_ADR_DST, 0x00, D_HDMAC0_AUTO_TEST_SIZE);
			DDIM_User_L1l2cache_Clean_Flush_All();
			ch = 2;
			Dd_GIC_Ctrl(E_DD_GIC_INTID_HDMAC0_CH2_INT,1,D_DD_GIC_PRI30,D_HDMAC0_CPU_TARGET);

			arb_type                        = D_DD_HDMAC0_ARB_FIX;		// fix
			wait_mode                       = D_DD_HDMAC0_WAITMODE_CPU;

			hdmac0_ctrl_trns.mode.input_sel = D_DD_HDMAC0_IS_SOFT;
			hdmac0_ctrl_trns.mode.mode_sel  = D_DD_HDMAC0_MS_BLK;
			hdmac0_ctrl_trns.mode.src_fix   = D_DD_HDMAC0_FS_INCR;
			hdmac0_ctrl_trns.mode.dst_fix   = D_DD_HDMAC0_FD_INCR;
			hdmac0_ctrl_trns.mode.beat_type = D_DD_HDMAC0_BT_NORMAL;

			hdmac0_ctrl_trns.size.trns_width = D_DD_HDMAC0_TW_WORD;
			hdmac0_ctrl_trns.size.trns_size  = D_HDMAC0_AUTO_TEST_SIZE;
			hdmac0_ctrl_trns.size.src_addr   = D_HDMAC0_AUTO_TEST_ADR_SRC;
			hdmac0_ctrl_trns.size.dst_addr   = D_HDMAC0_AUTO_TEST_ADR_DST;

			hdmac0_ctrl_trns.int_handler     = 0;

			ret = Dd_HDMAC0_Open(ch, D_DDIM_USER_SEM_WAIT_POL);
			if (ret != 0) {
				Ddim_Print(("HDMAC0 Open ERR\n"));
				return ret;
			}
			while (1) {
				ret = Dd_HDMAC0_Set_Arbitration(arb_type);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Open ERR\n"));
					break;
				}
				
				ret = Dd_HDMAC0_Ctrl_Trns(ch, &hdmac0_ctrl_trns);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Ctrl ERR\n"));
					break;
				}
				ret = Dd_HDMAC0_Start_Async(ch);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Start Sync ERR\n"));
					break;
				}
				ret = Dd_HDMAC0_Wait_End(ch, &status, wait_mode);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Wait ERR\n"));
					break;
				}
				break;
			}
			ret = Dd_HDMAC0_Close(ch);
			if (ret != 0) {
				Ddim_Print(("HDMAC0 Close ERR\n"));
			}
			Ddim_Print(("Result = %d\n", memcmp((void*)D_HDMAC0_AUTO_TEST_ADR_SRC, (void*)D_HDMAC0_AUTO_TEST_ADR_DST, D_HDMAC0_AUTO_TEST_SIZE)));
			break;

		// TEST ID 01-01-04
		case 4:
			memset((void*)D_HDMAC0_AUTO_TEST_ADR_DST, 0x00, D_HDMAC0_AUTO_TEST_SIZE);
			memset((void*)D_HDMAC0_AUTO_TEST_ADR_DST, 0x00, D_HDMAC0_AUTO_TEST_SIZE);
			DDIM_User_L1l2cache_Clean_Flush_All();
			ch = 3;
			Dd_GIC_Ctrl(E_DD_GIC_INTID_HDMAC0_CH3_INT,1,D_DD_GIC_PRI30,D_HDMAC0_CPU_TARGET);

			arb_type                        = D_DD_HDMAC0_ARB_FIX;		// fix
			wait_mode                       = D_DD_HDMAC0_WAITMODE_EVENT;

			hdmac0_ctrl_trns.mode.input_sel = D_DD_HDMAC0_IS_SOFT;
			hdmac0_ctrl_trns.mode.mode_sel  = D_DD_HDMAC0_MS_BURST;
			hdmac0_ctrl_trns.mode.src_fix   = D_DD_HDMAC0_FS_INCR;
			hdmac0_ctrl_trns.mode.dst_fix   = D_DD_HDMAC0_FD_INCR;
			hdmac0_ctrl_trns.mode.beat_type = D_DD_HDMAC0_BT_NORMAL;

			hdmac0_ctrl_trns.size.trns_width = D_DD_HDMAC0_TW_BYTE;
			hdmac0_ctrl_trns.size.trns_size  = D_HDMAC0_AUTO_TEST_SIZE;
			hdmac0_ctrl_trns.size.src_addr   = D_HDMAC0_AUTO_TEST_ADR_SRC;
			hdmac0_ctrl_trns.size.dst_addr   = D_HDMAC0_AUTO_TEST_ADR_DST;

			hdmac0_ctrl_trns.int_handler     = 0;

			ret = Dd_HDMAC0_Open(ch, D_DDIM_USER_SEM_WAIT_POL);
			if (ret != 0) {
				Ddim_Print(("HDMAC0 Open ERR\n"));
				return ret;
			}
			while (1) {
				ret = Dd_HDMAC0_Set_Arbitration(arb_type);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Open ERR\n"));
					break;
				}
				
				ret = Dd_HDMAC0_Ctrl_Trns(ch, &hdmac0_ctrl_trns);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Ctrl ERR\n"));
					break;
				}
				ret = Dd_HDMAC0_Start_Async(ch);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Start Sync ERR\n"));
					break;
				}
				ret = Dd_HDMAC0_Wait_End(ch, &status, wait_mode);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Wait ERR\n"));
					break;
				}
				break;
			}
			ret = Dd_HDMAC0_Close(ch);
			if (ret != 0) {
				Ddim_Print(("HDMAC0 Close ERR\n"));
			}
			Ddim_Print(("Result = %d\n", memcmp((void*)D_HDMAC0_AUTO_TEST_ADR_SRC, (void*)D_HDMAC0_AUTO_TEST_ADR_DST, D_HDMAC0_AUTO_TEST_SIZE)));
			break;

		case 5:
			memset((void*)D_HDMAC0_AUTO_TEST_ADR_DST, 0x00, D_HDMAC0_AUTO_TEST_SIZE);
			memset((void*)D_HDMAC0_AUTO_TEST_ADR_DST, 0x00, D_HDMAC0_AUTO_TEST_SIZE);
			DDIM_User_L1l2cache_Clean_Flush_All();
			ch = 2;
			Dd_GIC_Ctrl(E_DD_GIC_INTID_HDMAC0_CH2_INT,1,D_DD_GIC_PRI30,D_HDMAC0_CPU_TARGET);

			arb_type                    = D_DD_HDMAC0_ARB_FIX;		// fix
			wait_mode                   = D_DD_HDMAC0_WAITMODE_EVENT;

			hdmac0_ctrl.config_a.word   = 0;		// clear

			hdmac0_ctrl.config_a.bit.is = D_DD_HDMAC0_IS_SOFT;
			hdmac0_ctrl.config_a.bit.bt = D_DD_HDMAC0_BT_NORMAL;
			hdmac0_ctrl.config_a.bit.bc = 0x0F;
			hdmac0_ctrl.config_a.bit.tc = ((D_HDMAC0_AUTO_TEST_SIZE / 16) - 1);

			hdmac0_ctrl.config_b.word   = 0;		// clear

			hdmac0_ctrl.config_b.bit.ms = D_DD_HDMAC0_MS_BLK;
			hdmac0_ctrl.config_b.bit.tw = D_DD_HDMAC0_TW_BYTE;
			hdmac0_ctrl.config_b.bit.fs = D_DD_HDMAC0_FS_INCR;
			hdmac0_ctrl.config_b.bit.fd = D_DD_HDMAC0_FD_INCR;
			hdmac0_ctrl.config_b.bit.rc = D_DD_HDMAC0_RC_DISABLE;
			hdmac0_ctrl.config_b.bit.rs = D_DD_HDMAC0_RS_DISABLE;
			hdmac0_ctrl.config_b.bit.rd = D_DD_HDMAC0_RD_DISABLE;

			hdmac0_ctrl.src_addr        = D_HDMAC0_AUTO_TEST_ADR_SRC;
			hdmac0_ctrl.dst_addr        = D_HDMAC0_AUTO_TEST_ADR_DST;
			hdmac0_ctrl.int_handler     = 0;

			ret = Dd_HDMAC0_Open(ch, D_DDIM_USER_SEM_WAIT_POL);
			if (ret != 0) {
				Ddim_Print(("HDMAC0 Open ERR\n"));
				return ret;
			}
			while (1) {
				ret = Dd_HDMAC0_Set_Arbitration(arb_type);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Open ERR\n"));
					break;
				}
				
				ret = Dd_HDMAC0_Ctrl_Common(ch, &hdmac0_ctrl);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Ctrl ERR\n"));
					break;
				}
				ret = Dd_HDMAC0_Start_Async(ch);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Start Async ERR\n"));
					break;
				}
				ret = Dd_HDMAC0_Pause(ch);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Pause ERR\n"));
					break;
				}
				break;
			}
			Ddim_Print(("Result = %d\n", memcmp((void*)D_HDMAC0_AUTO_TEST_ADR_SRC, (void*)D_HDMAC0_AUTO_TEST_ADR_DST, D_HDMAC0_AUTO_TEST_SIZE)));
			break;

		case 6:
			memset((void*)D_HDMAC0_AUTO_TEST_ADR_DST, 0x00, D_HDMAC0_AUTO_TEST_SIZE);
			DDIM_User_L1l2cache_Clean_Flush_All();
			ch = 0;
			Dd_GIC_Ctrl(E_DD_GIC_INTID_HDMAC0_CH0_INT,1,D_DD_GIC_PRI30,D_HDMAC0_CPU_TARGET);

			arb_type                    = D_DD_HDMAC0_ARB_ROTATE;		// rotate
			wait_mode                   = D_DD_HDMAC0_WAITMODE_EVENT;

			hdmac0_ctrl.config_a.word   = 0;		// clear

			hdmac0_ctrl.config_a.bit.is = D_DD_HDMAC0_IS_SOFT;
			hdmac0_ctrl.config_a.bit.bt = D_DD_HDMAC0_BT_NORMAL;
			hdmac0_ctrl.config_a.bit.bc = (16 / 4) - 1;
			hdmac0_ctrl.config_a.bit.tc = ((D_HDMAC0_AUTO_TEST_SIZE / 16) - 1);

			hdmac0_ctrl.config_b.word   = 0;		// clear

			hdmac0_ctrl.config_b.bit.ms = D_DD_HDMAC0_MS_BLK;
			hdmac0_ctrl.config_b.bit.tw = D_DD_HDMAC0_TW_WORD;
			hdmac0_ctrl.config_b.bit.fs = D_DD_HDMAC0_FS_INCR;
			hdmac0_ctrl.config_b.bit.fd = D_DD_HDMAC0_FD_INCR;
			hdmac0_ctrl.config_b.bit.rc = D_DD_HDMAC0_RC_DISABLE;
			hdmac0_ctrl.config_b.bit.rs = D_DD_HDMAC0_RS_DISABLE;
			hdmac0_ctrl.config_b.bit.rd = D_DD_HDMAC0_RD_DISABLE;

			hdmac0_ctrl.src_addr        = D_HDMAC0_AUTO_TEST_ADR_SRC;
			hdmac0_ctrl.dst_addr        = D_HDMAC0_AUTO_TEST_ADR_DST;
			hdmac0_ctrl.int_handler     = 0;

			ret = Dd_HDMAC0_Open(ch, D_DDIM_USER_SEM_WAIT_POL);
			if (ret != 0) {
				Ddim_Print(("HDMAC0 Open ERR\n"));
				return ret;
			}
			while (1) {
				ret = Dd_HDMAC0_Set_Arbitration(arb_type);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Open ERR\n"));
					break;
				}
				
				ret = Dd_HDMAC0_Ctrl_Common(ch, &hdmac0_ctrl);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Ctrl ERR\n"));
					break;
				}
				ret = Dd_HDMAC0_Start_Async(ch);
				if (ret != 0) {
					Ddim_Print(("HDMAC0 Start Async ERR\n"));
					break;
				}
				Dd_HDMAC0_Stop_All_Ch();
				break;
			}
			Ddim_Print(("Result = %d\n", memcmp((void*)D_HDMAC0_AUTO_TEST_ADR_SRC, (void*)D_HDMAC0_AUTO_TEST_ADR_DST, D_HDMAC0_AUTO_TEST_SIZE)));
			break;

		default:
			ret = -1;
			break;
	}
	return ret;
#else
	return 0;
#endif
}

/**
 * @brief  Command main function for HDMAC0 test.
 * @param  INT32 argc	:The number of parameters
 *		   CHAR** argv	:The value of parameters
 *
 *	The meaning of parameters
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| P1              | P2       | P3         | P4       | P5         | P6         | P7       | P8 | P9 | P10 | P11 | Meaning                            |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "open"          | ch       | wait_time  |          |            |            |          |    |    |     |     | Dd_HDMAC0_Open()                   |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "close"         | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Close()                  |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "ctrl_cmm"      | ch       | src_addr   | dst_addr |            |            |          |    |    |     |     | Dd_HDMAC0_Ctrl_Common()            |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "ctrl_cmm_cfga" | ch       | "soft"     | "normal" | bc         | tc         |          |    |    |     |     | Set DMACA                          |
 *	|                 |          | "dreq_h"   | "single" |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "dreq_l"   | "incr"   |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_0"  | "wrap4"  |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_1"  | "incr4"  |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_2"  | "wrap8"  |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_3"  | "incr8"  |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_4"  | "wrap16" |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_5"  | "incr16" |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_6"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_7"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_8"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_9"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_10" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_11" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_12" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_13" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_14" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_15" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_16" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_17" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_18" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_19" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_20" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_21" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_22" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_23" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_24" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_25" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_26" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_27" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_28" |          |            |            |          |    |    |     |     |                                    |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "ctrl_cmm_cfgb" | ch       | "blk"      | "1"      | "src_fix"  | "dst_fix"  | rc       | rs | rd | sp  | dp  | Set DMACB                          |
 *	|                 |          | "burst"    | "2"      | "src_incr" | "dst_incr" |          |    |    |     |     |                                    |
 *	|                 |          | "demand"   | "4"      |            |            |          |    |    |     |     |                                    |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "ctrl_trns"     | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Ctrl_Trns()              |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "ctrl_trns_md"  | ch       | "soft"     | "blk"    | "src_fix"  | "dst_fix"  | "normal" |    |    |     |     | Set HDMAC0 transfer mode           |
 *	|                 |          | "dreq_h"   | "burst"  | "src_incr" | "dst_incr" | "single" |    |    |     |     |                                    |
 *	|                 |          | "dreq_l"   | "demand" |            |            | "incr"   |    |    |     |     |                                    |
 *	|                 |          | "idreq_0"  |          |            |            | "wrap4"  |    |    |     |     |                                    |
 *	|                 |          | "idreq_1"  |          |            |            | "incr4"  |    |    |     |     |                                    |
 *	|                 |          | "idreq_2"  |          |            |            | "wrap8"  |    |    |     |     |                                    |
 *	|                 |          | "idreq_3"  |          |            |            | "incr8"  |    |    |     |     |                                    |
 *	|                 |          | "idreq_4"  |          |            |            | "wrap16" |    |    |     |     |                                    |
 *	|                 |          | "idreq_5"  |          |            |            | "incr16" |    |    |     |     |                                    |
 *	|                 |          | "idreq_6"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_7"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_8"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_9"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_10" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_11" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_12" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_13" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_14" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_15" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_16" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_17" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_18" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_19" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_20" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_21" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_22" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_23" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_24" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_25" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_26" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_27" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_28" |          |            |            |          |    |    |     |     |                                    |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "ctrl_trns_sz"  | ch       | "1"        | size     | src_addr   | dst_addr   |          |    |    |     |     | Set HDMAC0 transfer size           |
 *	|                 |          | "2"        |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "4"        |          |            |            |          |    |    |     |     |                                    |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "start_sync"    | ch       | "cpu"      |          |            |            |          |    |    |     |     | Dd_HDMAC0_Start_Sync()             |
 *	|                 |          | "event"    |          |            |            |          |    |    |     |     |                                    |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "start_async"   | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Start_Async()            |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "stop"          | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Stop()                   |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "pause"         | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Pause()                  |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "resume"        | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Resume()                 |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "wait"          | ch       | "cpu"      |          |            |            |          |    |    |     |     | Dd_HDMAC0_Wait_End()               |
 *	|                 |          | "event"    |          |            |            |          |    |    |     |     |                                    |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "set_wait_time" | ch       | time       |          |            |            |          |    |    |     |     | Dd_HDMAC0_Set_Wait_Time()          |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "status_clear"  | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Clear_Status()           |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "status_read"   | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Get_Status()             |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "trns_size_set" | ch       | "1"        | size     | src_addr   | dst_addr   |          |    |    |     |     | Dd_HDMAC0_Set_Trns_Size()          |
 *	|                 |          | "2"        |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "4"        |          |            |            |          |    |    |     |     |                                    |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "trns_size_get" | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Get_Trns_Size()          |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "rmn_size_get"  | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Get_Remain_Trns_Size()   |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "total_size_get"| ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Get_Total_Trns_Size()    |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "src_addr_get"  | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Get_Src_Addr()           |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "dst_addr_get"  | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Get_Dst_Addr()           |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "src_prot_set"  | ch       | sp         |          |            |            |          |    |    |     |     | Dd_HDMAC0_Set_Source_Protect()     |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "dst_prot_set"  | ch       | dp         |          |            |            |          |    |    |     |     | Dd_HDMAC0_Set_Destination_Protect()|
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "arb_set"       | "rotate" |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Set_Arbitration()        |
 *	|                 | "fix"    |            |          |            |            |          |    |    |     |     |                                    |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "arb_get"       |          |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Get_Arbitration()        |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "arb_clear"     |          |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Clear_Arbitration()      |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "stop_all"      |          |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Stop_All_Ch()            |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "resume_all"    |          |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Resume_All_Ch()          |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "get_width"     | src_addr | dst_addr   | size     |            |            |          |    |    |     |     | Dd_HDMAC0_Get_Trns_Width()         |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "int_set"       | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Set_Int_Handler()        |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "int_handler"   | ch       |            |          |            |            |          |    |    |     |     | Dd_HDMAC0_Int_Handler()            |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "auto"          | num      |            |          |            |            |          |    |    |     |     | Continuous execution               |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "sdram_sync"    | ch       | src_addr   | dst_addr | size       | "cpu"      |          |    |    |     |     | Dd_HDMAC0_Copy_SDRAM_Sync()        |
 *	|                 |          |            |          |            | "event"    |          |    |    |     |     |                                    |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "sdram_async"   | ch       | src_addr   | dst_addr | size       |            |          |    |    |     |     | Dd_HDMAC0_Copy_SDRAM_Async()       |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "in_audio"      | ch       | "soft"     | src_addr | dst_addr   | size       |          |    |    |     |     | Dd_HDMAC0_Input_Audio_Async()      |
 *	|                 |          | "dreq_h"   |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "dreq_l"   |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_0"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_1"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_2"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_3"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_4"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_5"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_6"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_7"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_8"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_9"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_10" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_11" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_12" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_13" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_14" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_15" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_16" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_17" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_18" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_19" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_20" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_21" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_22" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_23" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_24" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_25" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_26" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_27" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_28" |          |            |            |          |    |    |     |     |                                    |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "out_audio"     | ch       | "soft"     | src_addr | dst_addr   | size       |          |    |    |     |     | Dd_HDMAC0_Output_Audio_Async()     |
 *	|                 |          | "dreq_h"   |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "dreq_l"   |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_0"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_1"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_2"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_3"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_4"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_5"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_6"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_7"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_8"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_9"  |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_10" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_11" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_12" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_13" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_14" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_15" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_16" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_17" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_18" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_19" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_20" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_21" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_22" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_23" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_24" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_25" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_26" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_27" |          |            |            |          |    |    |     |     |                                    |
 *	|                 |          | "idreq_28" |          |            |            |          |    |    |     |     |                                    |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 *	| "err"           |          |            |          |            |            |          |    |    |     |     |                                    |
 *	+-----------------+----------+------------+----------+------------+------------+----------+----+----+-----+-----+------------------------------------+
 * 
 *			ch			:	0          - 7
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
VOID Ct_Dd_HDMAC0_Main(int argc, char** argv)
{
	INT32				ret;
	INT32				size = 0;
	UINT32				wait_mode = D_DD_HDMAC0_WAITMODE_CPU;
	ULONG				src_addr;
	ULONG				dst_addr;
	ULONG				total_size;
	ULONG				trans_width = 0;
	char*				endstr;
	VP_CALLBACK			intHandler;
	USHORT				status = 0;
	UCHAR				ch;
	UCHAR				num;
	UCHAR				prot;
	UCHAR				arb_type;
	INT32				wait_time;
	UCHAR				input_sel;

	// check number of parameter
	if (argc > 12) {
		Ddim_Print(("The number of input parameters is exceeded!\n"));
		return;
	}

	if (strcmp(argv[1], "open") == 0) {
		/* ch number */
		ch = atoi(argv[2]);
		wait_time = atoi(argv[3]);

		ret = Dd_HDMAC0_Open(ch, wait_time);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Open OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Open ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "close") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		ret = Dd_HDMAC0_Close(ch);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Close OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Close ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "ctrl_cmm") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		gHDMAC0_Ctrl[ch].int_handler = ct_hdmac0_callback;

		gHDMAC0_Ctrl[ch].src_addr = strtoul(argv[3], &endstr, 16);

		gHDMAC0_Ctrl[ch].dst_addr = strtoul(argv[4], &endstr, 16);

		ret = Dd_HDMAC0_Ctrl_Common(ch, &gHDMAC0_Ctrl[ch]);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Ctrl_Common OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Ctrl_Common ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "ctrl_cmm_cfga") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		gHDMAC0_Ctrl[ch].config_a.word = 0;

		// Set IS
		ret = ct_hdmac0_set_is(ch, argv[3]);
		if( ret != D_DDIM_OK ){
			Ddim_Print(("Parameter ERR. P3 \n"));
			return;
		}

		// Set BT
		if (strcmp(argv[4], "normal") == 0) {
			gHDMAC0_Ctrl[ch].config_a.bit.bt = D_DD_HDMAC0_BT_NORMAL;
		}
		else if (strcmp(argv[4], "single") == 0) {
			gHDMAC0_Ctrl[ch].config_a.bit.bt = D_DD_HDMAC0_BT_SINGLE;
		}
		else if (strcmp(argv[4], "incr") == 0) {
			gHDMAC0_Ctrl[ch].config_a.bit.bt = D_DD_HDMAC0_BT_INCR;
		}
		else if (strcmp(argv[4], "wrap4") == 0) {
			gHDMAC0_Ctrl[ch].config_a.bit.bt = D_DD_HDMAC0_BT_WRAP4;
		}
		else if (strcmp(argv[4], "incr4") == 0) {
			gHDMAC0_Ctrl[ch].config_a.bit.bt = D_DD_HDMAC0_BT_INCR4;
		}
		else if (strcmp(argv[4], "wrap8") == 0) {
			gHDMAC0_Ctrl[ch].config_a.bit.bt = D_DD_HDMAC0_BT_WRAP8;
		}
		else if (strcmp(argv[4], "incr8") == 0) {
			gHDMAC0_Ctrl[ch].config_a.bit.bt = D_DD_HDMAC0_BT_INCR8;
		}
		else if (strcmp(argv[4], "wrap16") == 0) {
			gHDMAC0_Ctrl[ch].config_a.bit.bt = D_DD_HDMAC0_BT_WRAP16;
		}
		else if (strcmp(argv[4], "incr16") == 0) {
			gHDMAC0_Ctrl[ch].config_a.bit.bt = D_DD_HDMAC0_BT_INCR16;
		}
		else {
			Ddim_Print(("Parameter ERR. P4 \n"));
			return;
		}

		// Set BC
		gHDMAC0_Ctrl[ch].config_a.bit.bc = atoi(argv[5]);

		// Set TC
		gHDMAC0_Ctrl[ch].config_a.bit.tc = atoi(argv[6]);
	}
	else if (strcmp(argv[1], "ctrl_cmm_cfgb") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		gHDMAC0_Ctrl[ch].config_b.word = 0;

		// Set MS
		if (strcmp(argv[3], "blk") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.ms = D_DD_HDMAC0_MS_BLK;
		}
		else if (strcmp(argv[3], "burst") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.ms = D_DD_HDMAC0_MS_BURST;
		}
		else if (strcmp(argv[3], "demand") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.ms = D_DD_HDMAC0_MS_DEMAND;
		}
		else {
			Ddim_Print(("Parameter ERR. P3 \n"));
			return;
		}

		// Set TW
		if (strcmp(argv[4], "1") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.tw = D_DD_HDMAC0_TW_BYTE;
		}
		else if (strcmp(argv[4], "2") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.tw = D_DD_HDMAC0_TW_HWORD;
		}
		else if (strcmp(argv[4], "4") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.tw = D_DD_HDMAC0_TW_WORD;
		}
		else {
			Ddim_Print(("Parameter ERR. P4 \n"));
			return;
		}

		// Set FS
		if (strcmp(argv[5], "src_fix") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.fs = D_DD_HDMAC0_FS_FIX;
		}
		else if (strcmp(argv[5], "src_incr") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.fs = D_DD_HDMAC0_FS_INCR;
		}
		else {
			Ddim_Print(("Parameter ERR. P5 \n"));
			return;
		}

		// Set FD
		if (strcmp(argv[6], "dst_fix") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.fs = D_DD_HDMAC0_FD_FIX;
		}
		else if (strcmp(argv[6], "dst_incr") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.fs = D_DD_HDMAC0_FD_INCR;
		}
		else {
			Ddim_Print(("Parameter ERR. P6 \n"));
			return;
		}

		// Set RC
		if (strcmp(argv[7], "ena") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.rc = D_DD_HDMAC0_RC_ENABLE;
		}
		else if (strcmp(argv[7], "dis") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.rc = D_DD_HDMAC0_RC_DISABLE;
		}
		else {
			Ddim_Print(("Parameter ERR. P7 \n"));
			return;
		}

		// Set RS
		if (strcmp(argv[8], "ena") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.rs = D_DD_HDMAC0_RS_ENABLE;
		}
		else if (strcmp(argv[8], "dis") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.rs = D_DD_HDMAC0_RS_DISABLE;
		}
		else {
			Ddim_Print(("Parameter ERR. P8 \n"));
			return;
		}

		// Set RD
		if (strcmp(argv[9], "ena") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.rd = D_DD_HDMAC0_RD_ENABLE;
		}
		else if (strcmp(argv[9], "dis") == 0) {
			gHDMAC0_Ctrl[ch].config_b.bit.rd = D_DD_HDMAC0_RD_DISABLE;
		}
		else {
			Ddim_Print(("Parameter ERR. P9 \n"));
			return;
		}

		// Set SP
		gHDMAC0_Ctrl[ch].config_b.bit.sp = atoi(argv[10]);

		// Set DP
		gHDMAC0_Ctrl[ch].config_b.bit.dp = atoi(argv[11]);
	}
	else if (strcmp(argv[1], "ctrl_trns") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		gHDMAC0_Ctrl_Trns[ch].int_handler = ct_hdmac0_callback;

		ret = Dd_HDMAC0_Ctrl_Trns(ch, &gHDMAC0_Ctrl_Trns[ch]);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Ctrl_Trns OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Ctrl_Trns ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "ctrl_trns_md") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		// Set IS
		ret = ct_hdmac0_set_is(ch, argv[3]);
		if( ret != D_DDIM_OK ){
			Ddim_Print(("Parameter ERR. P3 \n"));
			return;
		}

		// Set MS
		if (strcmp(argv[4], "blk") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.mode_sel = D_DD_HDMAC0_MS_BLK;
		}
		else if (strcmp(argv[4], "burst") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.mode_sel = D_DD_HDMAC0_MS_BURST;
		}
		else if (strcmp(argv[4], "demand") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.mode_sel = D_DD_HDMAC0_MS_DEMAND;
		}
		else {
			Ddim_Print(("Parameter ERR. P4 \n"));
			return;
		}

		// Set FS
		if (strcmp(argv[5], "src_fix") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.src_fix = D_DD_HDMAC0_FS_FIX;
		}
		else if (strcmp(argv[5], "src_incr") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.src_fix = D_DD_HDMAC0_FS_INCR;
		}
		else {
			Ddim_Print(("Parameter ERR. P5 \n"));
			return;
		}

		// Set FD
		if (strcmp(argv[6], "dst_fix") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.dst_fix = D_DD_HDMAC0_FD_FIX;
		}
		else if (strcmp(argv[6], "dst_incr") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.dst_fix = D_DD_HDMAC0_FD_INCR;
		}
		else {
			Ddim_Print(("Parameter ERR. P6 \n"));
			return;
		}

		// Set BT
		if (strcmp(argv[7], "normal") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.beat_type = D_DD_HDMAC0_BT_NORMAL;
		}
		else if (strcmp(argv[7], "single") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.beat_type = D_DD_HDMAC0_BT_SINGLE;
		}
		else if (strcmp(argv[7], "incr") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.beat_type = D_DD_HDMAC0_BT_INCR;
		}
		else if (strcmp(argv[7], "wrap4") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.beat_type = D_DD_HDMAC0_BT_WRAP4;
		}
		else if (strcmp(argv[7], "incr4") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.beat_type = D_DD_HDMAC0_BT_INCR4;
		}
		else if (strcmp(argv[7], "wrap8") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.beat_type = D_DD_HDMAC0_BT_WRAP8;
		}
		else if (strcmp(argv[7], "incr8") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.beat_type = D_DD_HDMAC0_BT_INCR8;
		}
		else if (strcmp(argv[7], "wrap16") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.beat_type = D_DD_HDMAC0_BT_WRAP16;
		}
		else if (strcmp(argv[7], "incr16") == 0) {
			gHDMAC0_Ctrl_Trns[ch].mode.beat_type = D_DD_HDMAC0_BT_INCR16;
		}
		else {
			Ddim_Print(("Parameter ERR. P7 \n"));
			return;
		}
	}
	else if (strcmp(argv[1], "ctrl_trns_sz") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		// Set TW
		if (strcmp(argv[3], "1") == 0) {
			gHDMAC0_Ctrl_Trns[ch].size.trns_width = D_DD_HDMAC0_TW_BYTE;
		}
		else if (strcmp(argv[3], "2") == 0) {
			gHDMAC0_Ctrl_Trns[ch].size.trns_width = D_DD_HDMAC0_TW_HWORD;
		}
		else if (strcmp(argv[3], "4") == 0) {
			gHDMAC0_Ctrl_Trns[ch].size.trns_width = D_DD_HDMAC0_TW_WORD;
		}
		else {
			Ddim_Print(("Parameter ERR. P3 \n"));
			return;
		}

		// Set Size
		gHDMAC0_Ctrl_Trns[ch].size.trns_size = atoi(argv[4]);

		gHDMAC0_Ctrl_Trns[ch].size.src_addr = strtoul(argv[5], &endstr, 16);

		gHDMAC0_Ctrl_Trns[ch].size.dst_addr = strtoul(argv[6], &endstr, 16);
	}
	else if (strcmp(argv[1], "start_sync") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		if (strcmp(argv[3], "cpu") == 0) {
			wait_mode = D_DD_HDMAC0_WAITMODE_CPU;
		}
		else if (strcmp(argv[3], "event") == 0) {
			wait_mode = D_DD_HDMAC0_WAITMODE_EVENT;
		}
		else {
			Ddim_Print(("Parameter ERR P3. wait_mode=0x%x\n", wait_mode));
			return ;
		}

		ret = Dd_HDMAC0_Start_Sync(ch, &status, wait_mode);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Start Sync OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Start Sync ERR. ret=0x%x\n", ret));
		}
		Ddim_Print(("status=0x%x\n", status));
	}
	else if (strcmp(argv[1], "start_async") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		ret = Dd_HDMAC0_Start_Async(ch);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Start Async OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Start Async ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "stop") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		ret = Dd_HDMAC0_Stop(ch);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Stop OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Stop ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "pause") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		ret = Dd_HDMAC0_Pause(ch);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Pause OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Pause ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "resume") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		ret = Dd_HDMAC0_Resume(ch);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Resume OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Resume ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "wait") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		if (strcmp(argv[3], "cpu") == 0 ){
			wait_mode = D_DD_HDMAC0_WAITMODE_CPU;
		}
		else if (strcmp(argv[3], "event") == 0) {
			wait_mode = D_DD_HDMAC0_WAITMODE_EVENT;
		}
		else {
			Ddim_Print(("Parameter ERR P3. wait_mode=0x%x\n", wait_mode));
			return ;
		}

		ret = Dd_HDMAC0_Wait_End(ch, &status, wait_mode);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Wait OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Wait ERR. ret=0x%x\n", ret));
		}
		Ddim_Print(("status=0x%x\n", status));
	}
	else if (strcmp(argv[1], "set_wait_time") == 0) {
		/* ch number */
		ch = atoi(argv[2]);
		INT32 wait_time = atoi(argv[3]);

		ret = Dd_HDMAC0_Set_Wait_Time(ch, wait_time);
		if ( ret == D_DDIM_OK ) {
			Ddim_Print(("HDMAC0 Set Wait Time OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Set Wait Time ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "status_clear") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		ret = Dd_HDMAC0_Clear_Status(ch);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Status Clear OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Status Clear ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "status_read") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		ret = Dd_HDMAC0_Get_Status(ch, &status);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Get Status OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Get Status ERR. ret=0x%x\n", ret));
		}
		Ddim_Print(("status=0x%x\n", status));
	}
	else if (strcmp(argv[1], "trns_size_set") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		// Set TW
		if (strcmp(argv[3], "1") == 0) {
			gHDMAC0_Trns_Size[ch].trns_width = D_DD_HDMAC0_TW_BYTE;
		}
		else if (strcmp(argv[3], "2") == 0) {
			gHDMAC0_Trns_Size[ch].trns_width = D_DD_HDMAC0_TW_HWORD;
		}
		else if (strcmp(argv[3], "4") == 0) {
			gHDMAC0_Trns_Size[ch].trns_width = D_DD_HDMAC0_TW_WORD;
		}
		else {
			Ddim_Print(("Parameter ERR. P3 \n"));
			return;
		}
		gHDMAC0_Trns_Size[ch].trns_size = atoi(argv[4]);
		gHDMAC0_Trns_Size[ch].src_addr = strtoul(argv[5], &endstr, 16);
		gHDMAC0_Trns_Size[ch].dst_addr = strtoul(argv[6], &endstr, 16);

		ret = Dd_HDMAC0_Set_Trns_Size(ch, &gHDMAC0_Trns_Size[ch]);
		if ( ret == D_DDIM_OK ) {
			Ddim_Print(("HDMAC0 Set Trans Size OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Set Trans Size ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "trns_size_get") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		size = Dd_HDMAC0_Get_Trns_Size(ch);
		Ddim_Print(("HDMAC0 Trans Size Get OK\n"));
		Ddim_Print(("Size=%d\n", size));
	}
	else if (strcmp(argv[1], "rmn_size_get") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		size = Dd_HDMAC0_Get_Remain_Trns_Size(ch);
		Ddim_Print(("HDMAC0 Remain Trans Size OK\n"));
		Ddim_Print(("Size=%d\n", size));
	}
	else if (strcmp(argv[1], "total_size_get") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		size = Dd_HDMAC0_Get_Total_Trns_Size(ch);
		Ddim_Print(("HDMAC0 Total Trans Size OK\n"));
		Ddim_Print(("Size=%d\n", size));
	}
	else if (strcmp(argv[1], "src_addr_get") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		src_addr = Dd_HDMAC0_Get_Src_Addr(ch);
		Ddim_Print(("HDMAC0 Source Address OK\n"));
		Ddim_Print(("Src Addr=0x%08lx\n", src_addr));
	}
	else if (strcmp(argv[1], "dst_addr_get") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		dst_addr = Dd_HDMAC0_Get_Dst_Addr(ch);
		Ddim_Print(("HDMAC0 Destination Address OK\n"));
		Ddim_Print(("Dst Addr=0x%08lx\n", dst_addr));
	}
	else if (strcmp(argv[1], "src_prot_set") == 0) {
		/* ch number */
		ch = atoi(argv[2]);
		prot = atoi(argv[3]);

		ret = Dd_HDMAC0_Set_Source_Protect(ch, prot);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Source Protect Set OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Source Protect Set ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "dst_prot_set") == 0) {
		/* ch number */
		ch = atoi(argv[2]);
		prot = atoi(argv[3]);

		ret = Dd_HDMAC0_Set_Destination_Protect(ch, prot);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Destination Protect Set OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Destination Protect Set ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "arb_set") == 0) {
		if (strcmp(argv[2], "rotate") == 0) {
			arb_type = D_DD_HDMAC0_ARB_ROTATE;
		}
		else if (strcmp(argv[2], "fix") == 0) {
			arb_type = D_DD_HDMAC0_ARB_FIX;
		}
		else {
			Ddim_Print(("Parameter ERR P2. \n"));
			return;
		}

		ret = Dd_HDMAC0_Set_Arbitration(arb_type);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Arbitration Set OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Arbitration Set ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "arb_get") == 0) {
		ret = Dd_HDMAC0_Get_Arbitration(&arb_type);
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Arbitration Read OK\n"));
			Ddim_Print(("arb_type=0x%x\n", arb_type));
		}
		else {
			Ddim_Print(("HDMAC0 Arbitration Read ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "arb_clear") == 0) {
		Dd_HDMAC0_Clear_Arbitration();
	}
	else if (strcmp(argv[1], "stop_all") == 0) {
		Dd_HDMAC0_Stop_All_Ch();
	}
	else if (strcmp(argv[1], "resume_all") == 0) {
		Dd_HDMAC0_Resume_All_Ch();
	}
	else if (strcmp(argv[1], "get_width") == 0) {
		src_addr = strtoul(argv[2], &endstr, 16);
		dst_addr = strtoul(argv[3], &endstr, 16);
		total_size = atoi(argv[4]);

		trans_width = Dd_HDMAC0_Get_Trns_Width(src_addr, dst_addr, total_size);	/* pgr0539 */
		Ddim_Print(("HDMAC0 Trans Width Get OK\n"));
		Ddim_Print(("trans_width=%lu\n", trans_width));
	}
	else if (strcmp(argv[1], "int_set") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		intHandler = ct_hdmac0_callback;

		Dd_HDMAC0_Set_Int_Handler(ch, intHandler);
	}
	else if (strcmp(argv[1], "int_handler") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		Dd_HDMAC0_Int_Handler(ch);
	}
	else if (strcmp(argv[1], "auto") == 0) {
		/* ch number */
		num = atoi(argv[2]);

		ret = ct_hdmac0_process(num);
		if (ret == 0) {
			Ddim_Print(("ct_hdmac0_process OK\n"));
		}
		else {
			Ddim_Print(("ct_hdmac0_process ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "sdram_sync") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		src_addr = strtoul(argv[3], &endstr, 16);
		dst_addr = strtoul(argv[4], &endstr, 16);
		size = atoi(argv[5]);

		if (strcmp(argv[6], "cpu") == 0 ){
			wait_mode = D_DD_HDMAC0_WAITMODE_CPU;
		}
		else if (strcmp(argv[6], "event") == 0) {
			wait_mode = D_DD_HDMAC0_WAITMODE_EVENT;
		}
		else {
			Ddim_Print(("Parameter ERR P6. wait_mode=0x%x\n", wait_mode));
			return ;
		}

		ret = Dd_HDMAC0_Copy_SDRAM_Sync(ch, src_addr, dst_addr, size, wait_mode);	/* pgr0539 */
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Util Copy Sync. OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Util Copy Sync. ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "sdram_async") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		src_addr = strtoul(argv[3], &endstr, 16);
		dst_addr = strtoul(argv[4], &endstr, 16);
		size = atoi(argv[5]);
		intHandler = ct_hdmac0_callback;

		ret = Dd_HDMAC0_Copy_SDRAM_Async(ch, src_addr, dst_addr, size, intHandler);	/* pgr0539 */
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Util Copy Async. OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Util Copy Async. ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "in_audio") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		// Set IS
		ret = ct_hdmac0_set_is(ch, argv[3]);
		if( ret != D_DDIM_OK ){
			Ddim_Print(("Parameter ERR. P3 \n"));
			return;
		}
		input_sel = gHDMAC0_Ctrl_Trns[ch].mode.input_sel;

		src_addr = strtoul(argv[4], &endstr, 16);
		dst_addr = strtoul(argv[5], &endstr, 16);
		size = atoi(argv[6]);

		intHandler = ct_hdmac0_callback;

		ret = Dd_HDMAC0_Input_Audio_Async(ch, input_sel, src_addr, dst_addr, size, intHandler);	/* pgr0539 */
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Util Input Audio Async. OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Util Input Audio Async. ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "out_audio") == 0) {
		/* ch number */
		ch = atoi(argv[2]);

		// Set IS
		ret = ct_hdmac0_set_is(ch, argv[3]);
		if( ret != D_DDIM_OK ){
			Ddim_Print(("Parameter ERR. P3 \n"));
			return;
		}
		input_sel = gHDMAC0_Ctrl_Trns[ch].mode.input_sel;

		src_addr = strtoul(argv[4], &endstr, 16);
		dst_addr = strtoul(argv[5], &endstr, 16);
		size = atoi(argv[6]);

		intHandler = ct_hdmac0_callback;

		ret = Dd_HDMAC0_Output_Audio_Async(ch, input_sel, src_addr, dst_addr, size, intHandler);	/* pgr0539 */
		if (ret == 0) {
			Ddim_Print(("HDMAC0 Util Output Audio Async. OK\n"));
		}
		else {
			Ddim_Print(("HDMAC0 Util Input Audio Async. ERR. ret=0x%x\n", ret));
		}
	}
	else if(strcmp(argv[1], "cmp") == 0) {
		Ddim_Print(("cmp = %d\n", memcmp((void*)strtoul(argv[2], &endstr, 16), (void*)strtoul(argv[3], &endstr, 16), atoi(argv[4]))));
	}
	else if (strcmp(argv[1], "err") == 0){
		T_DD_HDMAC0_CTRL      hdmac0_ctrl;
		T_DD_HDMAC0_CTRL_TRNS hdmac0_ctrl_trns;
		T_DD_HDMAC0_TRNS_SIZE hdmac0_trns_size;
		USHORT status;

		hdmac0_ctrl_trns.mode.input_sel  = D_DD_HDMAC0_IS_SOFT;
		hdmac0_ctrl_trns.mode.mode_sel   = D_DD_HDMAC0_MS_BLK;
		hdmac0_ctrl_trns.mode.src_fix    = D_DD_HDMAC0_FS_INCR;
		hdmac0_ctrl_trns.mode.dst_fix    = D_DD_HDMAC0_FD_INCR;
		hdmac0_ctrl_trns.mode.beat_type  = D_DD_HDMAC0_BT_NORMAL;
		hdmac0_ctrl_trns.size.trns_width = D_DD_HDMAC0_TW_WORD;
		hdmac0_ctrl_trns.size.trns_size  = D_HDMAC0_AUTO_TEST_SIZE;
		hdmac0_ctrl_trns.size.src_addr   = D_HDMAC0_AUTO_TEST_ADR_SRC;
		hdmac0_ctrl_trns.size.dst_addr   = D_HDMAC0_AUTO_TEST_ADR_DST;
		hdmac0_ctrl_trns.int_handler     = 0;

		hdmac0_trns_size.trns_width      = D_DD_HDMAC0_TW_BYTE;
		hdmac0_trns_size.trns_size       = D_HDMAC0_AUTO_TEST_SIZE;
		hdmac0_trns_size.src_addr        = D_HDMAC0_AUTO_TEST_ADR_SRC;
		hdmac0_trns_size.dst_addr        = D_HDMAC0_AUTO_TEST_ADR_DST;

		ret = Dd_HDMAC0_Open(D_DD_HDMAC0_CH_NUM_MAX, D_DDIM_USER_SEM_WAIT_POL);
		Ddim_Print(("Dd_HDMAC0_Open(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Close(D_DD_HDMAC0_CH_NUM_MAX);
		Ddim_Print(("Dd_HDMAC0_Close(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Ctrl_Common(D_DD_HDMAC0_CH_NUM_MAX, &hdmac0_ctrl);
		Ddim_Print(("Dd_HDMAC0_Ctrl_Common(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Ctrl_Common(0, NULL);
		Ddim_Print(("Dd_HDMAC0_Ctrl_Common(): ctrl is NULL. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Ctrl_Trns(D_DD_HDMAC0_CH_NUM_MAX, &hdmac0_ctrl_trns);
		Ddim_Print(("Dd_HDMAC0_Ctrl_Trns(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Ctrl_Trns(0, NULL);
		Ddim_Print(("Dd_HDMAC0_Ctrl_Trns(): ctrl is NULL. ret=0x%x\n", ret));

		// BC over test (Ctrl_Trns)
		hdmac0_ctrl_trns.size.trns_size  = 0x00440000;	// 65536*4*17
		ret = Dd_HDMAC0_Ctrl_Trns(0, &hdmac0_ctrl_trns);
		Ddim_Print(("Dd_HDMAC0_Ctrl_Trns(): BC is over. ret=0x%x\n", ret));

		// BC over test in Demand mode (Ctrl_Trns)
		hdmac0_ctrl_trns.mode.mode_sel  = D_DD_HDMAC0_MS_DEMAND;
		hdmac0_ctrl_trns.size.trns_size = 0x00080000;	// 65536*4*2
		ret = Dd_HDMAC0_Ctrl_Trns(0, &hdmac0_ctrl_trns);
		Ddim_Print(("Dd_HDMAC0_Ctrl_Trns(): demand transfer and BC is not 0. ret=0x%x\n", ret));


		ret = Dd_HDMAC0_Set_Trns_Size(D_DD_HDMAC0_CH_NUM_MAX, &hdmac0_trns_size);
		Ddim_Print(("Dd_HDMAC0_Set_Trns_Size(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Set_Trns_Size(0, NULL);
		Ddim_Print(("Dd_HDMAC0_Set_Trns_Size(): trns_size is NULL. ret=0x%x\n", ret));

		// BC over test (Set_Trns_Size)
		hdmac0_ctrl_trns.mode.mode_sel  = D_DD_HDMAC0_MS_BLK;
		hdmac0_ctrl_trns.size.trns_size = 0x00001000;
		ret = Dd_HDMAC0_Ctrl_Trns(0, &hdmac0_ctrl_trns);
		hdmac0_trns_size.trns_size      = 0x00110000;	// 65536*1*17
		ret = Dd_HDMAC0_Set_Trns_Size(0, &hdmac0_trns_size);
		Ddim_Print(("Dd_HDMAC0_Set_Trns_Size(): BC is over. ret=0x%x\n", ret));

		// BC over test in Demand mode (Set_Trns_Size)
		hdmac0_ctrl_trns.mode.mode_sel  = D_DD_HDMAC0_MS_DEMAND;
		hdmac0_ctrl_trns.size.trns_size = 0x00001000;
		ret = Dd_HDMAC0_Ctrl_Trns(0, &hdmac0_ctrl_trns);
		hdmac0_trns_size.trns_size      = 0x00020000;	// 65536*1*2
		ret = Dd_HDMAC0_Set_Trns_Size(0, &hdmac0_trns_size);
		Ddim_Print(("Dd_HDMAC0_Set_Trns_Size(): demand transfer and BC is not 0. ret=0x%x\n", ret));


		ret = Dd_HDMAC0_Start_Sync(D_DD_HDMAC0_CH_NUM_MAX, &status, D_DD_HDMAC0_WAITMODE_CPU);
		Ddim_Print(("Dd_HDMAC0_Start_Sync(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Start_Sync(0, NULL, D_DD_HDMAC0_WAITMODE_CPU);
		Ddim_Print(("Dd_HDMAC0_Start_Sync(): status is NULL. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Start_Sync(0, &status, D_DD_HDMAC0_WAITMODE_EVENT+1);
		Ddim_Print(("Dd_HDMAC0_Start_Sync(): wait_mode is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Start_Async(D_DD_HDMAC0_CH_NUM_MAX);
		Ddim_Print(("Dd_HDMAC0_Start_Async(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Stop(D_DD_HDMAC0_CH_NUM_MAX);
		Ddim_Print(("Dd_HDMAC0_Stop(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Pause(D_DD_HDMAC0_CH_NUM_MAX);
		Ddim_Print(("Dd_HDMAC0_Pause(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Resume(D_DD_HDMAC0_CH_NUM_MAX);
		Ddim_Print(("Dd_HDMAC0_Resume(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Set_Wait_Time(D_DD_HDMAC0_CH_NUM_MAX, D_DDIM_USER_SEM_WAIT_POL);
		Ddim_Print(("Dd_HDMAC0_Set_Wait_Time(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Set_Wait_Time(0, -2);
		Ddim_Print(("Dd_HDMAC0_Set_Wait_Time(): wait_time is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Wait_End(D_DD_HDMAC0_CH_NUM_MAX, &status, D_DD_HDMAC0_WAITMODE_CPU);
		Ddim_Print(("Dd_HDMAC0_Wait_End(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Wait_End(0, NULL, D_DD_HDMAC0_WAITMODE_CPU);
		Ddim_Print(("Dd_HDMAC0_Wait_End(): status is NULL. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Wait_End(0, &status, D_DD_HDMAC0_WAITMODE_EVENT+1);
		Ddim_Print(("Dd_HDMAC0_Wait_End(): wait_mode is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Clear_Status(D_DD_HDMAC0_CH_NUM_MAX);
		Ddim_Print(("Dd_HDMAC0_Clear_Status(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Get_Status(D_DD_HDMAC0_CH_NUM_MAX, &status);
		Ddim_Print(("Dd_HDMAC0_Get_Status(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Get_Status(0, NULL);
		Ddim_Print(("Dd_HDMAC0_Get_Status(): status is NULL. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Get_Trns_Size(D_DD_HDMAC0_CH_NUM_MAX);
		Ddim_Print(("Dd_HDMAC0_Get_Trns_Size(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Get_Remain_Trns_Size(D_DD_HDMAC0_CH_NUM_MAX);
		Ddim_Print(("Dd_HDMAC0_Get_Remain_Trns_Size(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Get_Total_Trns_Size(D_DD_HDMAC0_CH_NUM_MAX);
		Ddim_Print(("Dd_HDMAC0_Get_Total_Trns_Size(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Get_Src_Addr(D_DD_HDMAC0_CH_NUM_MAX);
		Ddim_Print(("Dd_HDMAC0_Get_Src_Addr(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Get_Dst_Addr(D_DD_HDMAC0_CH_NUM_MAX);
		Ddim_Print(("Dd_HDMAC0_Get_Dst_Addr(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Set_Source_Protect(D_DD_HDMAC0_CH_NUM_MAX, 0);
		Ddim_Print(("Dd_HDMAC0_Set_Source_Protect(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Set_Destination_Protect(D_DD_HDMAC0_CH_NUM_MAX, 0);
		Ddim_Print(("Dd_HDMAC0_Set_Destination_Protect(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Set_Arbitration(D_DD_HDMAC0_ARB_ROTATE+1);
		Ddim_Print(("Dd_HDMAC0_Set_Arbitration(): arbitration is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Get_Arbitration(NULL);
		Ddim_Print(("Dd_HDMAC0_Get_Arbitration(): arbitration is NULL. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Copy_SDRAM_Sync(D_DD_HDMAC0_CH_NUM_MAX, 1, 2, 1, D_DD_HDMAC0_WAITMODE_EVENT);
		Ddim_Print(("Dd_HDMAC0_Copy_SDRAM_Sync(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Copy_SDRAM_Sync(0, 0, 2, 1, D_DD_HDMAC0_WAITMODE_EVENT);
		Ddim_Print(("Dd_HDMAC0_Copy_SDRAM_Sync(): src_addr is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Copy_SDRAM_Sync(0, 1, 0, 1, D_DD_HDMAC0_WAITMODE_EVENT);
		Ddim_Print(("Dd_HDMAC0_Copy_SDRAM_Sync(): dst_addr is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Copy_SDRAM_Sync(0, 1, 2, 0, D_DD_HDMAC0_WAITMODE_EVENT);
		Ddim_Print(("Dd_HDMAC0_Copy_SDRAM_Sync(): size is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Copy_SDRAM_Sync(0, 1, 2, 1, D_DD_HDMAC0_WAITMODE_EVENT+1);
		Ddim_Print(("Dd_HDMAC0_Copy_SDRAM_Sync(): wait_mode is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Copy_SDRAM_Async(D_DD_HDMAC0_CH_NUM_MAX, 1, 2, 1, ct_hdmac0_callback);
		Ddim_Print(("Dd_HDMAC0_Copy_SDRAM_Async(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Copy_SDRAM_Async(0, 0, 2, 1, ct_hdmac0_callback);
		Ddim_Print(("Dd_HDMAC0_Copy_SDRAM_Async(): src_addr is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Copy_SDRAM_Async(0, 1, 0, 1, ct_hdmac0_callback);
		Ddim_Print(("Dd_HDMAC0_Copy_SDRAM_Async(): dst_addr is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Copy_SDRAM_Async(0, 1, 2, 0, ct_hdmac0_callback);
		Ddim_Print(("Dd_HDMAC0_Copy_SDRAM_Async(): size is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Input_Audio_Async(D_DD_HDMAC0_CH_NUM_MAX, D_DD_HDMAC0_IS_SOFT, 1, 2, 1, ct_hdmac0_callback);
		Ddim_Print(("Dd_HDMAC0_Input_Audio_Async(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Input_Audio_Async(0, D_DD_HDMAC0_IS_SOFT, 0, 2, 1, ct_hdmac0_callback);
		Ddim_Print(("Dd_HDMAC0_Input_Audio_Async(): src_addr is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Input_Audio_Async(0, D_DD_HDMAC0_IS_SOFT, 1, 0, 1, ct_hdmac0_callback);
		Ddim_Print(("Dd_HDMAC0_Input_Audio_Async(): dst_addr is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Input_Audio_Async(0, D_DD_HDMAC0_IS_SOFT, 1, 2, 0, ct_hdmac0_callback);
		Ddim_Print(("Dd_HDMAC0_Input_Audio_Async(): size is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Output_Audio_Async(D_DD_HDMAC0_CH_NUM_MAX, D_DD_HDMAC0_IS_SOFT, 1, 2, 1, ct_hdmac0_callback);
		Ddim_Print(("Dd_HDMAC0_Output_Audio_Async(): Channel number is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Output_Audio_Async(0, D_DD_HDMAC0_IS_SOFT, 0, 2, 1, ct_hdmac0_callback);
		Ddim_Print(("Dd_HDMAC0_Output_Audio_Async(): src_addr is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Output_Audio_Async(0, D_DD_HDMAC0_IS_SOFT, 1, 0, 1, ct_hdmac0_callback);
		Ddim_Print(("Dd_HDMAC0_Output_Audio_Async(): dst_addr is illegal. ret=0x%x\n", ret));

		ret = Dd_HDMAC0_Output_Audio_Async(0, D_DD_HDMAC0_IS_SOFT, 1, 2, 0, ct_hdmac0_callback);
		Ddim_Print(("Dd_HDMAC0_Output_Audio_Async(): size is illegal. ret=0x%x\n", ret));
	}
	else {
		Ddim_Print(("please check 1st parameter!!\n"));
	}
	return;
}
