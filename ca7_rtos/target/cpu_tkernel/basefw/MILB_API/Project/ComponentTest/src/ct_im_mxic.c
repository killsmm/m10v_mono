/**
 * @file		ct_im_mxic.c
 * @brief		This is ct code for im_mxic.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "im_mxic.h"
#include "ct_im_mxic.h"
#include <string.h>
#include <stdlib.h>

#include "dd_top.h"

#include "ddim_user_custom.h"

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
#ifdef CO_ACT_MXIC_HCLOCK
extern volatile UCHAR gIM_MXIC_Hclk_Ctrl_Cnt;
#endif // CO_ACT_MXIC_HCLOCK
#ifdef CO_ACT_MXIC_PCLOCK
extern volatile UCHAR gIM_MXIC_Pclk_Ctrl_Cnt;
#endif // CO_ACT_MXIC_PCLOCK

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static INT32 ct_im_mxic_create_master_w_arbiter_param( E_IM_MXIC_UNIT unit, T_IM_MXIC_W_ARBITER_ASSIGN_PORT* const w_arbiter_assign );
static INT32 ct_im_mxic_create_master_r_arbiter_param( E_IM_MXIC_UNIT unit, T_IM_MXIC_R_ARBITER_ASSIGN_PORT* const r_arbiter_assign );

// atoi (Hex)
static int ct_im_mxic_atoi_hex(CHAR* ch)
{
	int ret = 0;
	int temp[20];
	int i = 0, n = 0, j;

	memset(temp, 0, sizeof(temp));
	if (ch[0] == '0' && (ch[1] == 'x' || ch[1] == 'X')) {
		ch += 2;
		// HEX
		while(*ch != 0) {
			switch(*ch) {
				case '0':	// FALL THROUGH 
				case '1':	// FALL THROUGH
				case '2':	// FALL THROUGH
				case '3':	// FALL THROUGH
				case '4':	// FALL THROUGH
				case '5':	// FALL THROUGH
				case '6':	// FALL THROUGH
				case '7':	// FALL THROUGH
				case '8':	// FALL THROUGH
				case '9':
					temp[n] = *ch - 0x30;
					break;

				case 'a':	// FALL THROUGH
				case 'A':
					temp[n] = 10;
					break;

				case 'b':	// FALL THROUGH
				case 'B':
					temp[n] = 11;
					break;

				case 'c':	// FALL THROUGH
				case 'C':
					temp[n] = 12;
					break;

				case 'd':	// FALL THROUGH
				case 'D':
					temp[n] = 13;
					break;

				case 'e':	// FALL THROUGH
				case 'E':
					temp[n] = 14;
					break;

				case 'f':	// FALL THROUGH
				case 'F':
					temp[n] = 15;
					break;
			}
			n++;
			ch++;
		}
		for(i = 0; i < n; i++) {
			for(j = 0; j < n - i - 1; j++) {
				temp[i] *= 16;
			}
			ret += temp[i];
		}
	}
	else{
		ret = atoi((const char*)ch);
	}
	return ret;
}

// Print T_IM_MXIC_W_ARBITER_ASSIGN data.
static VOID ct_im_mxic_print_w_arbiter_assign(T_IM_MXIC_W_ARBITER_ASSIGN_PORT const* const w_assign)
{
	INT32 i, j;

	Ddim_Print(("---- T_IM_MXIC_W_ARBITER_ASSIGN ----\n"));
	for (i = E_IM_MXIC_PORT_0; i < E_IM_MXIC_PORT_MAX; i++) {
		for (j = 0; j < D_IM_MXIC_SLOT_SIZE_8; j++) {
			Ddim_Print(("w_assign->port[%d].slot0[%d] = 0x%02X\n", i, j, w_assign->port[i].slot0[j]));
		}
		for (j = 0; j < D_IM_MXIC_SLOT_SIZE_8; j++) {
			Ddim_Print(("w_assign->port[%d].slot1[%d] = 0x%02X\n", i, j, w_assign->port[i].slot1[j]));
		}
		for (j = 0; j < D_IM_MXIC_SLOT_SIZE_8; j++) {
			Ddim_Print(("w_assign->port[%d].slot2[%d] = 0x%02X\n", i, j, w_assign->port[i].slot2[j]));
		}
		for (j = 0; j < D_IM_MXIC_SLOT_SIZE_8; j++) {
			Ddim_Print(("w_assign->port[%d].slot3[%d] = 0x%02X\n", i, j, w_assign->port[i].slot3[j]));
		}
	}
	return;
}

// Print T_IM_MXIC_W_ARBITER_ASSIGN data.
static VOID ct_im_mxic_print_w_arbiter_assign_group(T_IM_MXIC_W_ARBITER_ASSIGN_GR const* const w_assign)
{
	INT32 i;

	Ddim_Print(("---- T_IM_MXIC_W_ARBITER_ASSIGN ----\n"));
	for (i = 0; i < D_IM_MXIC_SLOT_SIZE_8; i++) {
		Ddim_Print(("w_assign->slot[%d] = 0x%02X\n", i, w_assign->slot[i]));
	}
	return;
}

// Print T_IM_MXIC_R_ARBITER_ASSIGN data.
static VOID ct_im_mxic_print_r_arbiter_assign(T_IM_MXIC_R_ARBITER_ASSIGN_PORT const* const r_assign)
{
	INT32 i, j;

	Ddim_Print(("---- T_IM_MXIC_R_ARBITER_ASSIGN ----\n"));
	for (i = E_IM_MXIC_PORT_0; i < E_IM_MXIC_PORT_MAX; i++) {
		for (j = 0; j < D_IM_MXIC_SLOT_SIZE_8; j++) {
			Ddim_Print(("r_assign->port[%d].slot0[%d] = 0x%02X\n", i, j, r_assign->port[i].slot0[j]));
		}
		for (j = 0; j < D_IM_MXIC_SLOT_SIZE_8; j++) {
			Ddim_Print(("r_assign->port[%d].slot1[%d] = 0x%02X\n", i, j, r_assign->port[i].slot1[j]));
		}
		for (j = 0; j < D_IM_MXIC_SLOT_SIZE_8; j++) {
			Ddim_Print(("r_assign->port[%d].slot2[%d] = 0x%02X\n", i, j, r_assign->port[i].slot2[j]));
		}
		for (j = 0; j < D_IM_MXIC_SLOT_SIZE_8; j++) {
			Ddim_Print(("r_assign->port[%d].slot3[%d] = 0x%02X\n", i, j, r_assign->port[i].slot3[j]));
		}
	}
	return;
}

// Print T_IM_MXIC_W_ARBITER_ASSIGN data.
static VOID ct_im_mxic_print_r_arbiter_assign_group(T_IM_MXIC_R_ARBITER_ASSIGN_GR const* const w_assign)
{
	INT32 i;

	Ddim_Print(("---- T_IM_MXIC_W_ARBITER_ASSIGN ----\n"));
	for (i = 0; i < D_IM_MXIC_SLOT_SIZE_8; i++) {
		Ddim_Print(("w_assign->slot[%d] = 0x%02X\n", i, w_assign->slot[i]));
	}
	return;
}

// Print T_IM_MXIC_ALL_ARBITER_ASSIGN data.
static VOID ct_im_mxic_print_all_arbiter_assign(T_IM_MXIC_ALL_ARBITER_ASSIGN const* const all_assign)
{
	INT32 i;

	Ddim_Print(("---- T_IM_MXIC_ALL_ARBITER_ASSIGN ----\n"));
	for (i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++) {
		Ddim_Print(("[W%d arbiter] Master assign\n", (i + 1)));
		ct_im_mxic_print_w_arbiter_assign(&all_assign->w_arbiter[i]);
	}
	for (i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++) {
		Ddim_Print(("[R%d arbiter] Master assign\n", (i + 1)));
		ct_im_mxic_print_r_arbiter_assign(&all_assign->r_arbiter[i]);
	}
	return;
}

// Print T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL data.
static VOID ct_im_mxic_print_all_slot_priority_level(T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL const* const all_arbiter_assign)
{
	INT32 i, j;
	
	Ddim_Print(("---- T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL ----\n"));
	// Write Slave
	for (i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++) {
		Ddim_Print((("---- W%d arbiter ----\n"), i+1));
		for (j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++) {
			Ddim_Print((("---- port%d ----\n"), j));
			Ddim_Print(("level0 priority_level_type  = %d\n", all_arbiter_assign->w_level[i].level0[j].priority_level_type));
			Ddim_Print(("level0 upper_priority_level = %d\n", all_arbiter_assign->w_level[i].level0[j].upper_priority_level));
			Ddim_Print(("level0 lower_priority_level = %d\n", all_arbiter_assign->w_level[i].level0[j].lower_priority_level));
			Ddim_Print(("level1 priority_level_type  = %d\n", all_arbiter_assign->w_level[i].level1[j].priority_level_type));
			Ddim_Print(("level1 upper_priority_level = %d\n", all_arbiter_assign->w_level[i].level1[j].upper_priority_level));
			Ddim_Print(("level1 lower_priority_level = %d\n", all_arbiter_assign->w_level[i].level1[j].lower_priority_level));
			Ddim_Print(("level2 priority_level_type  = %d\n", all_arbiter_assign->w_level[i].level2[j].priority_level_type));
			Ddim_Print(("level2 upper_priority_level = %d\n", all_arbiter_assign->w_level[i].level2[j].upper_priority_level));
			Ddim_Print(("level2 lower_priority_level = %d\n", all_arbiter_assign->w_level[i].level2[j].lower_priority_level));
		}
	}
	// Read Slave
	for (i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++) {
		Ddim_Print((("---- R%d arbiter ----\n"), i+1));
		for (j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++) {
			Ddim_Print((("---- port%d ----\n"), j));
			Ddim_Print(("level0 priority_level_type  = %d\n", all_arbiter_assign->r_level[i].level0[j].priority_level_type));
			Ddim_Print(("level0 upper_priority_level = %d\n", all_arbiter_assign->r_level[i].level0[j].upper_priority_level));
			Ddim_Print(("level0 lower_priority_level = %d\n", all_arbiter_assign->r_level[i].level0[j].lower_priority_level));
			Ddim_Print(("level1 priority_level_type  = %d\n", all_arbiter_assign->r_level[i].level1[j].priority_level_type));
			Ddim_Print(("level1 upper_priority_level = %d\n", all_arbiter_assign->r_level[i].level1[j].upper_priority_level));
			Ddim_Print(("level1 lower_priority_level = %d\n", all_arbiter_assign->r_level[i].level1[j].lower_priority_level));
			Ddim_Print(("level2 priority_level_type  = %d\n", all_arbiter_assign->r_level[i].level2[j].priority_level_type));
			Ddim_Print(("level2 upper_priority_level = %d\n", all_arbiter_assign->r_level[i].level2[j].upper_priority_level));
			Ddim_Print(("level2 lower_priority_level = %d\n", all_arbiter_assign->r_level[i].level2[j].lower_priority_level));
		}
	}
	
	return;
}

// Print T_IM_MXIC_ALL_SLOT_STATUS_MONITOR data.
static VOID ct_im_mxic_print_all_slot_status_monitor(T_IM_MXIC_ALL_SLOT_STATUS_MONITOR const* const all_slot_status)
{
	INT32 i;

	Ddim_Print(("---- T_IM_MXIC_ALL_SLOT_STATUS_MONITOR ----\n"));
	for (i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++) {
		Ddim_Print(("[W%d arbiter] Slot status\n", (i + 1)));
		ct_im_mxic_print_w_arbiter_assign(&all_slot_status->w_arbiter[i]);
	}
	for (i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++) {
		Ddim_Print(("[R%d arbiter] Slot status\n", (i + 1)));
		ct_im_mxic_print_r_arbiter_assign(&all_slot_status->r_arbiter[i]);
	}
	for (i = E_IM_MXIC_W_ARBITER_GR_W0; i < E_IM_MXIC_W_ARBITER_GR_MAX; i++) {
		Ddim_Print(("[Group W%d arbiter] Slot status\n", (i + 1)));
		ct_im_mxic_print_w_arbiter_assign_group(&all_slot_status->w_arbiter_gr[i]);
	}
	for (i = E_IM_MXIC_R_ARBITER_GR_R0; i < E_IM_MXIC_R_ARBITER_GR_MAX; i++) {
		Ddim_Print(("[Group R%d arbiter] Slot status\n", (i + 1)));
		ct_im_mxic_print_r_arbiter_assign_group(&all_slot_status->r_arbiter_gr[i]);
	}
	return;
}

// Print T_IM_MXIC_MASTER_STATUS_MONITOR data.
static VOID ct_im_mxic_print_master_status_monitor(T_IM_MXIC_MASTER_STATUS_MONITOR const* const master_status)
{
	INT32 i;

	Ddim_Print(("---- T_IM_MXIC_MASTER_STATUS_MONITOR ----\n"));
	for (i = 1; i < D_IM_MXIC_MAX_W_MASTER_NUM; i++) {
		Ddim_Print(("[W%d]\n", i));
		Ddim_Print(("w_master[%d] = %d\n", i, master_status->w_master[i]));
	}
	for (i = 1; i < D_IM_MXIC_MAX_R_MASTER_NUM; i++) {
		Ddim_Print(("[R%d]\n", i));
		Ddim_Print(("r_master[%d] = %d\n", i, master_status->r_master[i]));
	}
	return;
}

// Print T_IM_MXIC_MONITOR_PARAMETER data.
static VOID ct_im_mxic_print_monitor_parameter(T_IM_MXIC_MONITOR_PARAMETER const* const mon_param)
{
	Ddim_Print(("---- T_IM_MXIC_MONITOR_PARAMETER ----\n"));
	Ddim_Print(("start_condition          = %d\n", mon_param->start_condition));
	Ddim_Print(("end_condition            = %d\n", mon_param->end_condition));
	Ddim_Print(("update_enable            = %d\n", mon_param->update_enable));
	Ddim_Print(("update_trigger_interval  = %d\n", mon_param->update_trigger_interval));
	Ddim_Print(("update_clear_en          = %d\n", mon_param->update_clear_en));
	Ddim_Print(("start_clear_en           = %d\n", mon_param->start_clear_en));
	Ddim_Print(("cnt_val                  = %d\n", mon_param->cnt_val));
	Ddim_Print(("limit_access_trans       = %d\n", mon_param->limit_access_trans));
	Ddim_Print(("limit_detection_target   = %d\n", mon_param->limit_detection_target));
	Ddim_Print(("limit                    = %d\n", mon_param->limit));
	Ddim_Print(("target[0].monitor_sel    = %d\n", mon_param->target[0].monitor_sel));
	Ddim_Print(("target[1].monitor_sel    = %d\n", mon_param->target[1].monitor_sel));
	Ddim_Print(("target[2].monitor_sel    = %d\n", mon_param->target[2].monitor_sel));
	Ddim_Print(("target[3].monitor_sel    = %d\n", mon_param->target[3].monitor_sel));
	Ddim_Print(("target[0].monitor_target = %02X\n", mon_param->target[0].monitor_target));
	Ddim_Print(("target[1].monitor_target = %02X\n", mon_param->target[1].monitor_target));
	Ddim_Print(("target[2].monitor_target = %02X\n", mon_param->target[2].monitor_target));
	Ddim_Print(("target[3].monitor_target = %02X\n", mon_param->target[3].monitor_target));
	Ddim_Print(("pCallBack                = %08X\n", (unsigned int)mon_param->pCallBack));
	return;
}

// Print T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR data.
static VOID ct_im_mxic_print_all_access_or_trans_monitor(T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR const* const all_acc_trans)
{
	INT32 i;

	Ddim_Print(("---- T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR ----\n"));
	for (i = 0; i < D_IM_MXIC_MAX_MONITOR_TARGET_NUM; i++) {
		Ddim_Print(("all_acc_trans->count[%d] = %d\n", i, all_acc_trans->count[i]));
	}
	return;
}

// Print T_IM_MXIC_OUTPUT_PORT data.
static VOID ct_im_mxic_print_all_port_assign(T_IM_MXIC_OUTPUT_PORT const* const all_port_assign)
{
	Ddim_Print(("---- T_IM_MXIC_OUTPUT_PORT ----\n"));
	Ddim_Print(("(Write master output port)\n"));
	Ddim_Print(("JDSELA Master I/F          = %d\n", all_port_assign->w_out_port.ela));
	Ddim_Print(("JDSXH Master I/F           = %d\n", all_port_assign->w_out_port.xch));
	Ddim_Print(("FPT Master I/F-0           = %d\n", all_port_assign->w_out_port.fpt_0));
	Ddim_Print(("FPT Master I/F-1           = %d\n", all_port_assign->w_out_port.fpt_1));

	Ddim_Print(("\n(Read master output port)\n"));
	Ddim_Print(("JDSELA Master I/F          = %d\n", all_port_assign->r_out_port.ela));
	Ddim_Print(("JDSXH Master I/F           = %d\n", all_port_assign->r_out_port.xch));
	Ddim_Print(("FPT Master I/F-0           = %d\n", all_port_assign->r_out_port.fpt_0));
	Ddim_Print(("FPT Master I/F-1           = %d\n", all_port_assign->r_out_port.fpt_1));

	return;
}

// Print T_IM_MXIC_ALL_ACCEPTANCE_CAPABILITY data.
static VOID ct_im_mxic_print_all_acceptance_capability(T_IM_MXIC_ALL_ACCEPTANCE_CAPABILITY const* const all_capability)
{
	INT32 i, j;
	
	Ddim_Print(("---- T_IM_MXIC_ALL_ACCEPTANCE_CAPABILITY ----\n"));
	for (i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++) {
		for (j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++) {
			Ddim_Print(("all_capability->w_capability[%d][%d] = %d\n", i, j, all_capability->w_capability[i][j]));
		}
	}
	for (i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++) {
		for (j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++) {
			Ddim_Print(("all_capability->r_capability[%d][%d] = %d\n", i, j, all_capability->r_capability[i][j]));
		}
	}
	for (i = E_IM_MXIC_W_ARBITER_GR_W0; i < E_IM_MXIC_W_ARBITER_GR_MAX; i++) {
		for (j = E_IM_MXIC_PORT_GR_0; j < E_IM_MXIC_PORT_GR_MAX; j++) {
			Ddim_Print(("all_capability->w_capability_gr[%d][%d] = %d\n", i, j, all_capability->w_capability_gr[i][j]));
		}
	}
	for (i = E_IM_MXIC_R_ARBITER_GR_R0; i < E_IM_MXIC_R_ARBITER_GR_MAX; i++) {
		for (j = E_IM_MXIC_PORT_GR_0; j < E_IM_MXIC_PORT_GR_MAX; j++) {
			Ddim_Print(("all_capability->r_capability_gr[%d][%d] = %d\n", i, j, all_capability->r_capability_gr[i][j]));
		}
	}
	return;
}

// Print T_IM_MXIC_SLAVE_AREA data.
static VOID ct_im_mxic_print_slave_area(T_IM_MXIC_SLAVE_AREA const* const slave_area)
{
	Ddim_Print(("---- T_IM_MXIC_SLAVE_AREA ----\n"));
	Ddim_Print(("slave_area->size    = %d\n", slave_area->size));
	Ddim_Print(("slave_area->address = 0x%08X\n", slave_area->address));
	return;
}

// Print T_IM_MXIC_ALL_SLAVE_AREA data
static VOID ct_im_mxic_print_all_slave_area(T_IM_MXIC_ALL_SLAVE_AREA const* const all_slave_area)
{
	INT32 i;

	Ddim_Print(("---- T_IM_MXIC_ALL_SLAVE_AREA ----\n"));

	for (i = E_IM_MXIC_SLAVE_NUMBER_1; i < E_IM_MXIC_SLAVE_NUMBER_MAX; i++) {
		Ddim_Print(("[Slave area-%d]\n", (i + 1)));
		ct_im_mxic_print_slave_area(&all_slave_area->slave_area[i]);
	}
	return;
}

// Print T_IM_MXIC_DEC_ERR_INT data
static VOID ct_im_mxic_print_dec_err_int(T_IM_MXIC_DEC_ERR_INT const* const dec_err_int)
{
	Ddim_Print(("---- T_IM_MXIC_DEC_ERR_INT ----\n"));
	Ddim_Print(("dec_err_int->w_err_int_en = %d\n", dec_err_int->w_err_int_en));
	Ddim_Print(("dec_err_int->r_err_int_en = %d\n", dec_err_int->r_err_int_en));
	Ddim_Print(("dec_err_int->pCallBack    = %08X\n", (unsigned int)dec_err_int->pCallBack));
	return;
}

// Print T_IM_MXIC_DEC_ERR data
static VOID ct_im_mxic_print_dec_err(T_IM_MXIC_DEC_ERR const* const dec_err)
{
	Ddim_Print(("---- T_IM_MXIC_DEC_ERR ----\n"));
	Ddim_Print(("dec_err->w_req_master = %02X\n", dec_err->w_req_master));
	Ddim_Print(("dec_err->w_req_addr   = %08X\n", dec_err->w_req_addr));
	Ddim_Print(("dec_err->r_req_master = %02X\n", dec_err->r_req_master));
	Ddim_Print(("dec_err->r_req_addr   = %08X\n", dec_err->r_req_addr));
	return;
}

// Print T_IM_MXIC_HISTORY_MONITOR data
static VOID ct_im_mxic_print_history_monitor(T_IM_MXIC_HISTORY_MONITOR const* const history)
{
	INT32 i;

	Ddim_Print(("---- T_IM_MXIC_HISTORY_MONITOR ----\n"));
	for (i = 0; i < D_IM_MXIC_MAX_MONITOR_HISTORY_NUM; i++) {
		Ddim_Print(("history->history[%d] = %d\n", i, history->history[i]));
	}
	return;
}

// Print T_IM_MXIC_ALL_HISTORY_MONITOR data
static VOID ct_im_mxic_print_all_history_monitor(T_IM_MXIC_ALL_HISTORY_MONITOR const* const all_history)
{
	INT32 i, j;

	Ddim_Print(("---- T_IM_MXIC_ALL_HISTORY_MONITOR ----\n"));
	for (i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++) {
		for (j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++) {
			Ddim_Print(("[W%d arbiter-Port%d]\n", (i + 1), j));
			ct_im_mxic_print_history_monitor(&all_history->w_arbiter[i][j]);
		}
	}
	for (i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++) {
		for (j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++) {
			Ddim_Print(("[R%d arbiter-Port%d]\n", (i + 1), j));
			ct_im_mxic_print_history_monitor(&all_history->r_arbiter[i][j]);
		}
	}
	return;
}

// Print T_IM_MXIC_MEMORY_ACCESS_SLAVE data
static VOID ct_im_mxic_print_memory_access_param(T_IM_MXIC_MEMORY_ACCESS_SLAVE const* const memacc_param)
{
	INT32 i;

	Ddim_Print(("---- T_IM_MXIC_MEMORY_ACCESS_SLAVE ----\n"));
	for (i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++) {
		Ddim_Print(("[W%d memory access]\n", (i + 1)));
		Ddim_Print(("memacc_param->slave[%d].start_trigger = %d\n"   , (i + 1), memacc_param->slave[i].start_trigger));
		Ddim_Print(("memacc_param->slave[%d].master47_1 = %llu\n"     , (i + 1), memacc_param->slave[i].master47_1));
		Ddim_Print(("memacc_param->slave[%d].master127_64 = %llu\n"   , (i + 1), memacc_param->slave[i].master127_64));
		Ddim_Print(("memacc_param->slave[%d].start_address = %08X\n" , (i + 1), memacc_param->slave[i].start_address));
		Ddim_Print(("memacc_param->slave[%d].end_address = %08X\n"   , (i + 1), memacc_param->slave[i].end_address));
		Ddim_Print(("memacc_param->slave[%d].mode = %d\n"            , (i + 1), memacc_param->slave[i].mode));
		Ddim_Print(("memacc_param->slave[%d].detect_master = %d\n"   , (i + 1), memacc_param->slave[i].detect_master));
		Ddim_Print(("memacc_param->slave[%d].detect_address = %08X\n", (i + 1), memacc_param->slave[i].detect_address));
	}
	return;
}

// Print T_IM_MXIC_ALL_LEVELPORT  data
static VOID ct_im_mxic_print_all_levelport(T_IM_MXIC_ALL_LEVELPORT* const all_levelport)
{
	INT32 i, j;

	Ddim_Print(("---- T_IM_MXIC_ALL_LEVELPORT ----\n"));
	for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

		for ( j = 0; j < D_IM_MXIC_MASTER_NUMBER_MAX-1; j++ ) {

			Ddim_Print(("all_levelport->w_arbiter[ %d ].port[ %d ] = %d\n", i, j, all_levelport->w_arbiter[ i ].port[ j ]));
			Ddim_Print(("all_levelport->w_arbiter[ %d ].level[ %d ] = %d\n",i, j, all_levelport->w_arbiter[ i ].level[ j ]));
		}
	}

	for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {

		for ( j = 0; j < D_IM_MXIC_MASTER_NUMBER_MAX-1; j++ ) {

			Ddim_Print(("all_levelport->r_arbiter[ %d ].port[ %d ] = %d\n", i, j, all_levelport->r_arbiter[ i ].port[ j ]));
			Ddim_Print(("all_levelport->r_arbiter[ %d ].level[ %d ] = %d\n",i, j, all_levelport->r_arbiter[ i ].level[ j ]));
		}
	}

	return;
}

// Error test of Im_MXIC_Start_Config function.
static VOID ct_im_mxic_check_err_start_config(VOID)
{
	INT32						result;
	T_IM_MXIC_CONFIG_ARBITER	config;

	// unit check
	Ddim_Print(("----[Im_MXIC_Start_Config(): unit check]----\n"));
	result = Im_MXIC_Start_Config( (E_IM_MXIC_UNIT)7, &config );
	Ddim_Print(("result=%d\n", result));

	// NULL check
	Ddim_Print(("----[Im_MXIC_Start_Config(): NULL check]----\n"));
	result = Im_MXIC_Start_Config( (E_IM_MXIC_UNIT)0, NULL );
	Ddim_Print(("result=%d\n", result));
	return;
}

// Error test of Im_MXIC_Set_Clock_Enable and Im_MXIC_Get_Clock_Enable function.
static VOID ct_im_mxic_check_err_clock_enable(VOID)
{
	INT32			result;
	T_IM_MXIC_CLOCK	clock_ctrl;

	// unit check
	Ddim_Print(("----[Im_MXIC_Set_Clock_Enable(): unit check]----\n"));
	result = Im_MXIC_Set_Clock_Enable( (E_IM_MXIC_UNIT)7, &clock_ctrl );
	Ddim_Print(("result=%d\n", result));

	// unit check
	Ddim_Print(("----[Im_MXIC_Get_Clock_Enable(): unit check]----\n"));
	result = Im_MXIC_Get_Clock_Enable( (E_IM_MXIC_UNIT)7, &clock_ctrl );
	Ddim_Print(("result=%d\n", result));

	// NULL check
	Ddim_Print(("----[Im_MXIC_Set_Clock_Enable(): NULL check]----\n"));
	result = Im_MXIC_Set_Clock_Enable( (E_IM_MXIC_UNIT)0, NULL );
	Ddim_Print(("result=%d\n", result));

	// NULL check
	Ddim_Print(("----[Im_MXIC_Get_Clock_Enable(): NULL check]----\n"));
	result = Im_MXIC_Get_Clock_Enable( (E_IM_MXIC_UNIT)0, NULL );
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Acceptance_Capability and Im_MXIC_Get_Acceptance_Capability function.
static VOID ct_im_mxic_check_err_acceptance_capability(VOID)
{
	INT32 result;
	UCHAR capability;

	// --- Im_MXIC_Set_Acceptance_Capability ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Set_Acceptance_Capability(): unit range error check]----\n"));
	result = Im_MXIC_Set_Acceptance_Capability((E_IM_MXIC_UNIT)7, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_0, 8);
	Ddim_Print(("result=%d\n", result));

	// write or read arbiter range error check
	Ddim_Print(("----[Im_MXIC_Set_Acceptance_Capability(): wr_arbiter range error check]----\n"));
	result = Im_MXIC_Set_Acceptance_Capability((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_MAX, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_0, 8);
	Ddim_Print(("result=%d\n", result));

	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Set_Acceptance_Capability(): arbiter range error check]----\n"));
	result = Im_MXIC_Set_Acceptance_Capability((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_MAX, E_IM_MXIC_PORT_0, 8);
	Ddim_Print(("result=%d\n", result));

	// port range error check
	Ddim_Print(("----[Im_MXIC_Set_Acceptance_Capability(): port range error check]----\n"));
	result = Im_MXIC_Set_Acceptance_Capability((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_MAX, 1);
	Ddim_Print(("result=%d\n", result));

	// capability range error check(Min)
	Ddim_Print(("----[Im_MXIC_Set_Acceptance_Capability(): capability range error check(Min)]----\n"));
	result = Im_MXIC_Set_Acceptance_Capability((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_3, E_IM_MXIC_PORT_1, 0);
	Ddim_Print(("result=%d\n", result));

	// capability range error check(Max)
	Ddim_Print(("----[Im_MXIC_Set_Acceptance_Capability(): capability range error check(Max)]----\n"));
	result = Im_MXIC_Set_Acceptance_Capability((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_4, E_IM_MXIC_PORT_1, 9);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Acceptance_Capability ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Get_Acceptance_Capability(): unit range error check]----\n"));
	result = Im_MXIC_Get_Acceptance_Capability((E_IM_MXIC_UNIT)7, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_0, &capability);
	Ddim_Print(("result=%d\n", result));

	// write or read arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Acceptance_Capability(): wr_arbiter range error check]----\n"));
	result = Im_MXIC_Get_Acceptance_Capability((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_MAX, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_0, &capability);
	Ddim_Print(("result=%d\n", result));

	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Acceptance_Capability(): arbiter range error check]----\n"));
	result = Im_MXIC_Get_Acceptance_Capability((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_MAX, E_IM_MXIC_PORT_0, &capability);
	Ddim_Print(("result=%d\n", result));

	// port range error check
	Ddim_Print(("----[Im_MXIC_Get_Acceptance_Capability(): port range error check]----\n"));
	result = Im_MXIC_Get_Acceptance_Capability((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_MAX, &capability);
	Ddim_Print(("result=%d\n", result));

	// capability NULL check
	Ddim_Print(("----[Im_MXIC_Get_Acceptance_Capability(): capability NULL check]----\n"));
	result = Im_MXIC_Get_Acceptance_Capability((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_3, E_IM_MXIC_PORT_1, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Acceptance_Capability_Group and Im_MXIC_Get_Acceptance_Capability_Group function.
static VOID ct_im_mxic_check_err_acceptance_capability_group(VOID)
{
	INT32 result;
	UCHAR capability;

	// --- Im_MXIC_Set_Acceptance_Capability ---
	// write or read arbiter range error check
	Ddim_Print(("----[Im_MXIC_Set_Acceptance_Capability_Group(): wr_arbiter range error check]----\n"));
	result = Im_MXIC_Set_Acceptance_Capability_Group(E_IM_MXIC_WR_ARBITER_MAX, E_IM_MXIC_SPEC_ARBITER_GR_0, E_IM_MXIC_PORT_GR_0, 8);
	Ddim_Print(("result=%d\n", result));

	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Set_Acceptance_Capability_Group(): arbiter range error check]----\n"));
	result = Im_MXIC_Set_Acceptance_Capability_Group(E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_GR_MAX, E_IM_MXIC_PORT_GR_0, 8);
	Ddim_Print(("result=%d\n", result));

	// port range error check
	Ddim_Print(("----[Im_MXIC_Set_Acceptance_Capability_Group(): port range error check]----\n"));
	result = Im_MXIC_Set_Acceptance_Capability_Group(E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_GR_1, E_IM_MXIC_PORT_GR_MAX, 1);
	Ddim_Print(("result=%d\n", result));

	// capability range error check(Min)
	Ddim_Print(("----[Im_MXIC_Set_Acceptance_Capability_Group(): capability range error check(Min)]----\n"));
	result = Im_MXIC_Set_Acceptance_Capability_Group(E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_GR_1, E_IM_MXIC_PORT_GR_1, 0);
	Ddim_Print(("result=%d\n", result));

	// capability range error check(Max)
	Ddim_Print(("----[Im_MXIC_Set_Acceptance_Capability_Group(): capability range error check(Max)]----\n"));
	result = Im_MXIC_Set_Acceptance_Capability_Group(E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_GR_2, E_IM_MXIC_PORT_GR_1, 9);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Acceptance_Capability ---
	// write or read arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Acceptance_Capability_Group(): wr_arbiter range error check]----\n"));
	result = Im_MXIC_Get_Acceptance_Capability_Group(E_IM_MXIC_WR_ARBITER_MAX, E_IM_MXIC_SPEC_ARBITER_GR_0, E_IM_MXIC_PORT_GR_0, &capability);
	Ddim_Print(("result=%d\n", result));

	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Acceptance_Capability_Group(): arbiter range error check]----\n"));
	result = Im_MXIC_Get_Acceptance_Capability_Group(E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_GR_MAX, E_IM_MXIC_PORT_GR_0, &capability);
	Ddim_Print(("result=%d\n", result));

	// port range error check
	Ddim_Print(("----[Im_MXIC_Get_Acceptance_Capability_Group(): port range error check]----\n"));
	result = Im_MXIC_Get_Acceptance_Capability_Group(E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_GR_0, E_IM_MXIC_PORT_GR_MAX, &capability);
	Ddim_Print(("result=%d\n", result));

	// capability NULL check
	Ddim_Print(("----[Im_MXIC_Get_Acceptance_Capability(): capability NULL check]----\n"));
	result = Im_MXIC_Get_Acceptance_Capability_Group(E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_GR_1, E_IM_MXIC_PORT_GR_1, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Acceptance_Capability_All_Port and Im_MXIC_Get_Acceptance_Capability_All_Port function.
static VOID ct_im_mxic_check_err_acceptance_capability_all_port(VOID)
{
	INT32								result;
	T_IM_MXIC_ALL_ACCEPTANCE_CAPABILITY	all_capability;

	// --- Im_MXIC_Set_Acceptance_Capability_All_Port ---
	// unit check
	Ddim_Print(("----[Im_MXIC_Set_Acceptance_Capability_All_Port(): unit check]----\n"));
	result = Im_MXIC_Set_Acceptance_Capability_All_Port((E_IM_MXIC_UNIT)7, &all_capability);
	Ddim_Print(("result=%d\n", result));

	// NULL check
	Ddim_Print(("----[Im_MXIC_Set_Acceptance_Capability_All_Port(): NULL check]----\n"));
	result = Im_MXIC_Set_Acceptance_Capability_All_Port((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Acceptance_Capability_All_Port ---
	// unit check
	Ddim_Print(("----[Im_MXIC_Get_Acceptance_Capability_All_Port(): unit check]----\n"));
	result = Im_MXIC_Get_Acceptance_Capability_All_Port((E_IM_MXIC_UNIT)7, &all_capability);
	Ddim_Print(("result=%d\n", result));

	// NULL check
	Ddim_Print(("----[Im_MXIC_Get_Acceptance_Capability_All_Port(): NULL check]----\n"));
	result = Im_MXIC_Get_Acceptance_Capability_All_Port((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Slave_Area and Im_MXIC_Get_Slave_Area function.
static VOID ct_im_mxic_check_err_slave_area(VOID)
{
	INT32 result;
	T_IM_MXIC_SLAVE_AREA slave_area;

	// --- Im_MXIC_Set_Slave_Area ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Set_Slave_Area(): unit range error check]----\n"));
	result = Im_MXIC_Set_Slave_Area((E_IM_MXIC_UNIT)7, E_IM_MXIC_SLAVE_NUMBER_2, &slave_area);
	Ddim_Print(("result=%d\n", result));

	// slave_number range error check
	Ddim_Print(("----[Im_MXIC_Set_Slave_Area(): slave_number range error check]----\n"));
	result = Im_MXIC_Set_Slave_Area((E_IM_MXIC_UNIT)0, E_IM_MXIC_SLAVE_NUMBER_MAX, &slave_area);
	Ddim_Print(("result=%d\n", result));

	// slave_area NULL check
	Ddim_Print(("----[Im_MXIC_Set_Slave_Area(): slave_area NULL check]----\n"));
	result = Im_MXIC_Set_Slave_Area((E_IM_MXIC_UNIT)0, E_IM_MXIC_SLAVE_NUMBER_2, NULL);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Slave_Area ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Get_Slave_Area(): unit range error check]----\n"));
	result = Im_MXIC_Get_Slave_Area((E_IM_MXIC_UNIT)7, E_IM_MXIC_SLAVE_NUMBER_2, &slave_area);
	Ddim_Print(("result=%d\n", result));

	// slave_number range error check
	Ddim_Print(("----[Im_MXIC_Get_Slave_Area(): slave_number range error check]----\n"));
	result = Im_MXIC_Get_Slave_Area((E_IM_MXIC_UNIT)0, E_IM_MXIC_SLAVE_NUMBER_MAX, &slave_area);
	Ddim_Print(("result=%d\n", result));

	// slave_area NULL check
	Ddim_Print(("----[Im_MXIC_Get_Slave_Area(): slave_area NULL check]----\n"));
	result = Im_MXIC_Get_Slave_Area((E_IM_MXIC_UNIT)0, E_IM_MXIC_SLAVE_NUMBER_2, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Slave_Area_All and Im_MXIC_Get_Slave_Area_All function.
static VOID ct_im_mxic_check_err_slave_area_all(VOID)
{
	INT32						result;
	T_IM_MXIC_ALL_SLAVE_AREA	all_slave_area;

	// --- Im_MXIC_Set_Slave_Area_All ---
	// NULL check
	Ddim_Print(("----[Im_MXIC_Set_Slave_Area_All(): unit check]----\n"));
	result = Im_MXIC_Set_Slave_Area_All((E_IM_MXIC_UNIT)7, &all_slave_area);
	Ddim_Print(("result=%d\n", result));

	// NULL check
	Ddim_Print(("----[Im_MXIC_Set_Slave_Area_All(): NULL check]----\n"));
	result = Im_MXIC_Set_Slave_Area_All((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Slave_Area_All ---
	// unit check
	Ddim_Print(("----[Im_MXIC_Get_Slave_Area_All(): unit check]----\n"));
	result = Im_MXIC_Get_Slave_Area_All((E_IM_MXIC_UNIT)7, &all_slave_area);
	Ddim_Print(("result=%d\n", result));

	// NULL check
	Ddim_Print(("----[Im_MXIC_Get_Slave_Area_All(): NULL check]----\n"));
	result = Im_MXIC_Get_Slave_Area_All((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Decode_Error_Int and Im_MXIC_Get_Decode_Error_Int and Im_MXIC_Get_Decode_Error function.
static VOID ct_im_mxic_check_err_decode_error(VOID)
{
	INT32					result;
	T_IM_MXIC_DEC_ERR_INT	dec_err_int;
	T_IM_MXIC_DEC_ERR		dec_err;

	// --- Im_MXIC_Set_Decode_Error_Int ---
	// unit check
	Ddim_Print(("----[Im_MXIC_Set_Decode_Error_Int(): unit check]----\n"));
	result = Im_MXIC_Set_Decode_Error_Int((E_IM_MXIC_UNIT)7, &dec_err_int);
	Ddim_Print(("result=%d\n", result));

	// NULL check
	Ddim_Print(("----[Im_MXIC_Set_Decode_Error_Int(): NULL check]----\n"));
	result = Im_MXIC_Set_Decode_Error_Int((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Decode_Error_Int ---
	// unit check
	Ddim_Print(("----[Im_MXIC_Get_Decode_Error_Int(): unit check]----\n"));
	result = Im_MXIC_Get_Decode_Error_Int((E_IM_MXIC_UNIT)7, &dec_err_int);
	Ddim_Print(("result=%d\n", result));

	// NULL check
	Ddim_Print(("----[Im_MXIC_Get_Decode_Error_Int(): NULL check]----\n"));
	result = Im_MXIC_Get_Decode_Error_Int((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Decode_Error ---
	// unit check
	Ddim_Print(("----[Im_MXIC_Get_Decode_Error(): unit check]----\n"));
	result = Im_MXIC_Get_Decode_Error((E_IM_MXIC_UNIT)7, &dec_err);
	Ddim_Print(("result=%d\n", result));

	// NULL check
	Ddim_Print(("----[Im_MXIC_Get_Decode_Error(): NULL check]----\n"));
	result = Im_MXIC_Get_Decode_Error((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));
	
	return;
}

// Error test of Im_MXIC_Set_LevelPort_Level and Im_MXIC_Get_LevelPort_Level function.
static VOID ct_im_mxic_check_err_levelport_level(VOID)
{
	INT32 result;
	E_IM_MXIC_LEVEL level;

	// --- Im_MXIC_Set_LevelPort_Level ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Set_LevelPort_Level(): unit range error check]----\n"));
	result = Im_MXIC_Set_LevelPort_Level( (E_IM_MXIC_UNIT)7, (E_IM_MXIC_WR_ARBITER)0, (E_IM_MXIC_SPEC_ARBITER)0, 0x00, (E_IM_MXIC_LEVEL)0 );
	Ddim_Print(("result=%d\n", result));

	// Target write or read range error check
	Ddim_Print(("----[Im_MXIC_Set_LevelPort_Level(): Target write or read range error check]----\n"));
	result = Im_MXIC_Set_LevelPort_Level( (E_IM_MXIC_UNIT)0, (E_IM_MXIC_WR_ARBITER)2, (E_IM_MXIC_SPEC_ARBITER)0, 0x00, (E_IM_MXIC_LEVEL)0 );
	Ddim_Print(("result=%d\n", result));

	// Target arbiter range error check
	Ddim_Print(("----[Im_MXIC_Set_LevelPort_Level(): Target arbiter range error check]----\n"));
	result = Im_MXIC_Set_LevelPort_Level( (E_IM_MXIC_UNIT)0, (E_IM_MXIC_WR_ARBITER)0, (E_IM_MXIC_SPEC_ARBITER)4, 0x00, (E_IM_MXIC_LEVEL)0 );
	Ddim_Print(("result=%d\n", result));

	// Target level range error check
	Ddim_Print(("----[Im_MXIC_Set_LevelPort_Level(): Target level range error check]----\n"));
	result = Im_MXIC_Set_LevelPort_Level( (E_IM_MXIC_UNIT)0, (E_IM_MXIC_WR_ARBITER)0, (E_IM_MXIC_SPEC_ARBITER)0, 0x00, (E_IM_MXIC_LEVEL)4 );
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_LevelPort_Level ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Get_LevelPort_Level(): unit range error check]----\n"));
	result = Im_MXIC_Get_LevelPort_Level( (E_IM_MXIC_UNIT)7, (E_IM_MXIC_WR_ARBITER)0, (E_IM_MXIC_SPEC_ARBITER)0, 0x00, &level );
	Ddim_Print(("result=%d\n", result));

	// Target write or read range error check
	Ddim_Print(("----[Im_MXIC_Get_LevelPort_Level(): Target write or read range error check]----\n"));
	result = Im_MXIC_Get_LevelPort_Level( (E_IM_MXIC_UNIT)0, (E_IM_MXIC_WR_ARBITER)2, (E_IM_MXIC_SPEC_ARBITER)0, 0x00, &level );
	Ddim_Print(("result=%d\n", result));

	// Target arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_LevelPort_Level(): Target arbiter range error check]----\n"));
	result = Im_MXIC_Get_LevelPort_Level( (E_IM_MXIC_UNIT)0, (E_IM_MXIC_WR_ARBITER)0, (E_IM_MXIC_SPEC_ARBITER)4, 0x00, &level );
	Ddim_Print(("result=%d\n", result));

	// Level null error check
	Ddim_Print(("----[Im_MXIC_Get_LevelPort_Level(): Level null error check]----\n"));
	result = Im_MXIC_Get_LevelPort_Level( (E_IM_MXIC_UNIT)0, (E_IM_MXIC_WR_ARBITER)0, (E_IM_MXIC_SPEC_ARBITER)0, 0x00, NULL );
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_LevelPort_Port and Im_MXIC_Get_LevelPort_Port function.
static VOID ct_im_mxic_check_err_levelport_port(VOID)
{
	INT32 result;
	E_IM_MXIC_PORT port;

	// --- Im_MXIC_Set_LevelPort_Port ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Set_LevelPort_Port(): unit range error check]----\n"));
	result = Im_MXIC_Set_LevelPort_Port( (E_IM_MXIC_UNIT)7, (E_IM_MXIC_WR_ARBITER)0, (E_IM_MXIC_SPEC_ARBITER)0, 0x00, (E_IM_MXIC_PORT)0 );
	Ddim_Print(("result=%d\n", result));

	// Target write or read range error check
	Ddim_Print(("----[Im_MXIC_Set_LevelPort_Port(): Target write or read range error check]----\n"));
	result = Im_MXIC_Set_LevelPort_Port( (E_IM_MXIC_UNIT)0, (E_IM_MXIC_WR_ARBITER)2, (E_IM_MXIC_SPEC_ARBITER)0, 0x00, (E_IM_MXIC_PORT)0 );
	Ddim_Print(("result=%d\n", result));

	// Target arbiter range error check
	Ddim_Print(("----[Im_MXIC_Set_LevelPort_Port(): Target arbiter range error check]----\n"));
	result = Im_MXIC_Set_LevelPort_Port( (E_IM_MXIC_UNIT)0, (E_IM_MXIC_WR_ARBITER)0, (E_IM_MXIC_SPEC_ARBITER)4, 0x00, (E_IM_MXIC_PORT)0 );
	Ddim_Print(("result=%d\n", result));

	// Target port range error check
	Ddim_Print(("----[Im_MXIC_Set_LevelPort_Port(): Target port range error check]----\n"));
	result = Im_MXIC_Set_LevelPort_Port( (E_IM_MXIC_UNIT)0, (E_IM_MXIC_WR_ARBITER)0, (E_IM_MXIC_SPEC_ARBITER)0, 0x00, (E_IM_MXIC_PORT)4 );
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_LevelPort_Port ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Get_LevelPort_Port(): unit range error check]----\n"));
	result = Im_MXIC_Get_LevelPort_Port( (E_IM_MXIC_UNIT)7, (E_IM_MXIC_WR_ARBITER)0, (E_IM_MXIC_SPEC_ARBITER)0, 0x00, &port );
	Ddim_Print(("result=%d\n", result));

	// Target write or read range error check
	Ddim_Print(("----[Im_MXIC_Get_LevelPort_Port(): Target write or read range error check]----\n"));
	result = Im_MXIC_Get_LevelPort_Port( (E_IM_MXIC_UNIT)0, (E_IM_MXIC_WR_ARBITER)2, (E_IM_MXIC_SPEC_ARBITER)0, 0x00, &port );
	Ddim_Print(("result=%d\n", result));

	// Target arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_LevelPort_Port(): Target arbiter range error check]----\n"));
	result = Im_MXIC_Get_LevelPort_Port( (E_IM_MXIC_UNIT)0, (E_IM_MXIC_WR_ARBITER)0, (E_IM_MXIC_SPEC_ARBITER)4, 0x00, &port );
	Ddim_Print(("result=%d\n", result));

	// Port null error check
	Ddim_Print(("----[Im_MXIC_Get_LevelPort_Port(): Port null error check]----\n"));
	result = Im_MXIC_Get_LevelPort_Port( (E_IM_MXIC_UNIT)0, (E_IM_MXIC_WR_ARBITER)0, (E_IM_MXIC_SPEC_ARBITER)0, 0x00, NULL );
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_LevelPort_All and Im_MXIC_Get_LevelPort_All function.
static VOID ct_im_mxic_check_err_levelport_all(VOID)
{
	INT32					result;
	T_IM_MXIC_ALL_LEVELPORT	all_levelport;

	// --- Im_MXIC_Set_LevelPort_All ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Set_LevelPort_All(): unit range error check]----\n"));
	result = Im_MXIC_Set_LevelPort_All( (E_IM_MXIC_UNIT)7, &all_levelport );
	Ddim_Print(("result=%d\n", result));

	// all_levelport NULL error check
	Ddim_Print(("----[Im_MXIC_Set_LevelPort_All(): all_levelport NULL error check]----\n"));
	result = Im_MXIC_Set_LevelPort_All( (E_IM_MXIC_UNIT)0, NULL );
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_LevelPort_All ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Get_LevelPort_All(): unit range error check]----\n"));
	result = Im_MXIC_Get_LevelPort_All( (E_IM_MXIC_UNIT)7, &all_levelport );
	Ddim_Print(("result=%d\n", result));

	// all_levelport NULL error check
	Ddim_Print(("----[Im_MXIC_Get_LevelPort_All(): all_levelport NULL error check]----\n"));
	result = Im_MXIC_Get_LevelPort_All( (E_IM_MXIC_UNIT)0, NULL );
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Output_Port and Im_MXIC_Get_Output_Port function.
static VOID ct_im_mxic_check_err_port_assign(VOID)
{
	INT32 result;

	// --- Im_MXIC_Set_Output_Port ---
	// assign range error check
	Ddim_Print(("----[Im_MXIC_Set_Output_Port(): assign range error check]----\n"));
	result = Im_MXIC_Set_Output_Port(D_IM_MXIC_3_W_ELA, E_IM_MXIC_PORT_GR_MAX);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Output_Port ---
	// assign NULL check
	Ddim_Print(("----[Im_MXIC_Get_Output_Port(): assign NULL check]----\n"));
	result = Im_MXIC_Get_Output_Port(D_IM_MXIC_3_W_ELA, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Output_Port_All and Im_MXIC_Get_Output_Port_All function.
static VOID ct_im_mxic_check_err_port_assign_all(VOID)
{
	INT32 result;

	// --- Im_MXIC_Set_Output_Port_All ---
	// all_assign NULL check
	Ddim_Print(("----[Im_MXIC_Set_Output_Port_All(): all_assign NULL check]----\n"));
	result = Im_MXIC_Set_Output_Port_All(NULL);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Output_Port_All ---
	// all_assign NULL check
	Ddim_Print(("----[Im_MXIC_Get_Output_Port_All(): all_assign NULL check]----\n"));
	result = Im_MXIC_Get_Output_Port_All(NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Master_W_Arbiter and Im_MXIC_Get_Master_W_Arbiter function.
static VOID ct_im_mxic_check_err_master_w_arbiter(VOID)
{
	INT32 result;
	INT32 i;
	T_IM_MXIC_W_ARBITER_ASSIGN_PORT w_assign;

	// Initialize
	for (i = 0; i < D_IM_MXIC_SLOT_SIZE_8; i++) {
		w_assign.port[0].slot0[i] = D_IM_MXIC_W_RESERVE;
		w_assign.port[0].slot2[i] = D_IM_MXIC_W_RESERVE;
		w_assign.port[0].slot3[i] = D_IM_MXIC_W_RESERVE;
	}
	for (i = 0; i < D_IM_MXIC_SLOT_SIZE_8; i++) {
		w_assign.port[0].slot1[i] = D_IM_MXIC_W_RESERVE;
	}

	// --- Im_MXIC_Set_Master_W_Arbiter ---
	// unit error check
	Ddim_Print(("----[Im_MXIC_Set_Master_W_Arbiter(): unit range error check]----\n"));
	result = Im_MXIC_Set_Master_W_Arbiter((E_IM_MXIC_UNIT)7, E_IM_MXIC_W_ARBITER_W1, &w_assign);
	Ddim_Print(("result=%d\n", result));

	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_W_Arbiter(): arbiter range error check]----\n"));
	result = Im_MXIC_Set_Master_W_Arbiter((E_IM_MXIC_UNIT)0, E_IM_MXIC_W_ARBITER_MAX, &w_assign);
	Ddim_Print(("result=%d\n", result));

	// assign.port[0].slot0[7] range error check(W max)
	Ddim_Print(("----[Im_MXIC_Set_Master_W_Arbiter(): assign.port[0].slot0[7] range error check(W max)]----\n"));
	w_assign.port[0].slot0[7] = D_IM_MXIC_MAX_W_MASTER_NUM;
	result = Im_MXIC_Set_Master_W_Arbiter((E_IM_MXIC_UNIT)0, E_IM_MXIC_W_ARBITER_W1, &w_assign);
	w_assign.port[0].slot0[7] = D_IM_MXIC_W_RESERVE;
	Ddim_Print(("result=%d\n", result));

	// assign.port[0].slot1[7] range error check(RW type)
	Ddim_Print(("----[Im_MXIC_Set_Master_W_Arbiter(): assign.port[0].slot1[7] range error check(RW type)]----\n"));
	w_assign.port[0].slot1[7] = D_IM_MXIC_1_R_SRO;
	result = Im_MXIC_Set_Master_W_Arbiter((E_IM_MXIC_UNIT)0, E_IM_MXIC_W_ARBITER_W2, &w_assign);
	w_assign.port[0].slot1[7] = D_IM_MXIC_W_RESERVE;
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Master_W_Arbiter ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Get_Master_W_Arbiter(): unit range error check]----\n"));
	result = Im_MXIC_Get_Master_W_Arbiter((E_IM_MXIC_UNIT)7, E_IM_MXIC_W_ARBITER_W2, &w_assign);
	Ddim_Print(("result=%d\n", result));

	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Master_W_Arbiter(): arbiter range error check]----\n"));
	result = Im_MXIC_Get_Master_W_Arbiter((E_IM_MXIC_UNIT)0, E_IM_MXIC_W_ARBITER_MAX, &w_assign);
	Ddim_Print(("result=%d\n", result));

	// assign NULL check
	Ddim_Print(("----[Im_MXIC_Get_Master_W_Arbiter(): assign NULL check]----\n"));
	result = Im_MXIC_Get_Master_W_Arbiter((E_IM_MXIC_UNIT)0, E_IM_MXIC_W_ARBITER_W2, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Master_R_Arbiter and Im_MXIC_Get_Master_R_Arbiter function.
static VOID ct_im_mxic_check_err_master_r_arbiter(VOID)
{
	INT32 result;
	INT32 i;
	T_IM_MXIC_R_ARBITER_ASSIGN_PORT r_assign;

	// Initialize
	for (i = 0; i < D_IM_MXIC_SLOT_SIZE_8; i++) {
		r_assign.port[0].slot0[i] = D_IM_MXIC_R_RESERVE;
		r_assign.port[0].slot2[i] = D_IM_MXIC_R_RESERVE;
		r_assign.port[0].slot3[i] = D_IM_MXIC_R_RESERVE;
	}
	for (i = 0; i < D_IM_MXIC_SLOT_SIZE_8; i++) {
		r_assign.port[0].slot1[i] = D_IM_MXIC_R_RESERVE;
	}

	// --- Im_MXIC_Set_Master_R_Arbiter ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_R_Arbiter(): unit range error check]----\n"));
	result = Im_MXIC_Set_Master_R_Arbiter((E_IM_MXIC_UNIT)7, E_IM_MXIC_R_ARBITER_R2, &r_assign);
	Ddim_Print(("result=%d\n", result));

	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_R_Arbiter(): arbiter range error check]----\n"));
	result = Im_MXIC_Set_Master_R_Arbiter((E_IM_MXIC_UNIT)0, E_IM_MXIC_R_ARBITER_MAX, &r_assign);
	Ddim_Print(("result=%d\n", result));

	// assign.slot1[7].slot1[7] range error check(RW type)
	Ddim_Print(("----[Im_MXIC_Set_Master_R_Arbiter(): assign.port[0].slot1[7] range error check(RW type)]----\n"));
	r_assign.port[0].slot1[7] = D_IM_MXIC_0_W_SEN_0;
	result = Im_MXIC_Set_Master_R_Arbiter((E_IM_MXIC_UNIT)0, E_IM_MXIC_R_ARBITER_R2, &r_assign);
	r_assign.port[0].slot1[7] = D_IM_MXIC_R_RESERVE;
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Master_R_Arbiter ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Get_Master_R_Arbiter(): unit range error check]----\n"));
	result = Im_MXIC_Get_Master_R_Arbiter((E_IM_MXIC_UNIT)7, E_IM_MXIC_R_ARBITER_R2, &r_assign);
	Ddim_Print(("result=%d\n", result));

	// 	arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Master_R_Arbiter(): arbiter range error check]----\n"));
	result = Im_MXIC_Get_Master_R_Arbiter((E_IM_MXIC_UNIT)0, E_IM_MXIC_R_ARBITER_MAX, &r_assign);
	Ddim_Print(("result=%d\n", result));

	// assign NULL check
	Ddim_Print(("----[Im_MXIC_Get_Master_R_Arbiter(): assign NULL check]----\n"));
	result = Im_MXIC_Get_Master_R_Arbiter((E_IM_MXIC_UNIT)0, E_IM_MXIC_R_ARBITER_R2, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Master_All_Arbiter and Im_MXIC_Get_Master_All_Arbiter function.
static VOID ct_im_mxic_check_err_master_all_arbiter(VOID)
{
	INT32							result;
	T_IM_MXIC_ALL_ARBITER_ASSIGN	all_arbiter_assign;

	// --- Im_MXIC_Set_Master_All_Arbiter ---
	// all_assign unit check
	Ddim_Print(("----[Im_MXIC_Set_Master_All_Arbiter(): all_assign unit check]----\n"));
	result = Im_MXIC_Set_Master_All_Arbiter((E_IM_MXIC_UNIT)7, &all_arbiter_assign);
	Ddim_Print(("result=%d\n", result));

	// all_assign NULL check
	Ddim_Print(("----[Im_MXIC_Set_Master_All_Arbiter(): all_assign NULL check]----\n"));
	result = Im_MXIC_Set_Master_All_Arbiter((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Master_All_Arbiter ---
	// all_assign unit check
	Ddim_Print(("----[Im_MXIC_Get_Master_All_Arbiter(): all_assign unit check]----\n"));
	result = Im_MXIC_Get_Master_All_Arbiter((E_IM_MXIC_UNIT)7, &all_arbiter_assign);
	Ddim_Print(("result=%d\n", result));

	// all_assign NULL check
	Ddim_Print(("----[Im_MXIC_Get_Master_All_Arbiter(): all_assign NULL check]----\n"));
	result = Im_MXIC_Get_Master_All_Arbiter((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}


static VOID ct_im_mxic_check_err_master_overlap( VOID )
{
	INT32 result;
	T_IM_MXIC_W_ARBITER_ASSIGN_PORT w_arbiter_assign;
	T_IM_MXIC_R_ARBITER_ASSIGN_PORT r_arbiter_assign;

	Ddim_Print(("----[Im_MXIC_Set_Master_W_Arbiter(): assign error check]----\n"));
	ct_im_mxic_create_master_w_arbiter_param( (E_IM_MXIC_UNIT)0, &w_arbiter_assign );
	w_arbiter_assign.port[0].slot1[7] = D_IM_MXIC_0_W_SEN_0;
	w_arbiter_assign.port[0].slot0[7] = D_IM_MXIC_0_W_SEN_0;
	result = Im_MXIC_Set_Master_W_Arbiter( (E_IM_MXIC_UNIT)0, E_IM_MXIC_W_ARBITER_W1, &w_arbiter_assign );
	Ddim_Print(("result=%d\n", result));

	Ddim_Print(("----[Im_MXIC_Set_Master_R_Arbiter(): assign error check]----\n"));
	ct_im_mxic_create_master_r_arbiter_param( (E_IM_MXIC_UNIT)0, &r_arbiter_assign );
	r_arbiter_assign.port[0].slot2[6] = D_IM_MXIC_1_R_SRO;
	r_arbiter_assign.port[0].slot1[7] = D_IM_MXIC_1_R_SRO;
	result = Im_MXIC_Set_Master_R_Arbiter( (E_IM_MXIC_UNIT)0, E_IM_MXIC_R_ARBITER_R1, &r_arbiter_assign );
	Ddim_Print(("result=%d\n", result));
	
	return;
}

// Error test of Im_MXIC_Set_Slot_Priority and Im_MXIC_Get_Slot_Priority function.
static VOID ct_im_mxic_check_err_slot_priority(VOID)
{
	INT32 result;
	T_IM_MXIC_SLOT_PRIORITY_LEVEL priority;

	// Initialize
	priority.priority_level_type  = D_IM_MXIC_SLOT_PRIORITY_LOWWER;
	priority.upper_priority_level = 0;
	priority.lower_priority_level = 63;

	// --- Im_MXIC_Set_Slot_Priority ---
	// priority unit check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Priority(): priority unit check]----\n"));
	result = Im_MXIC_Set_Slot_Priority((E_IM_MXIC_UNIT)7, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_LEVEL_CTRL_2, &priority);
	Ddim_Print(("result=%d\n", result));

	// priority NULL check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Priority(): priority NULL check]----\n"));
	result = Im_MXIC_Set_Slot_Priority((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_LEVEL_CTRL_2, NULL);
	Ddim_Print(("result=%d\n", result));

	// write or read arbiter range error check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Priority(): wr_arbiter range error check]----\n"));
	result = Im_MXIC_Set_Slot_Priority((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_MAX, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_LEVEL_CTRL_2, &priority);
	Ddim_Print(("result=%d\n", result));

	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Priority(): arbiter range error check]----\n"));
	result = Im_MXIC_Set_Slot_Priority((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_MAX, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_LEVEL_CTRL_2, &priority);
	Ddim_Print(("result=%d\n", result));

	// port range error check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Priority(): port range error check]----\n"));
	result = Im_MXIC_Set_Slot_Priority((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_MAX, E_IM_MXIC_SLOT_LEVEL_CTRL_1, &priority);
	Ddim_Print(("result=%d\n", result));
	
	// level_number range error check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Priority(): level_number range error check]----\n"));
	result = Im_MXIC_Set_Slot_Priority((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_1, E_IM_MXIC_SLOT_LEVEL_CTRL_MAX, &priority);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Slot_Priority ---
	// priority unit check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Priority(): priority unit check]----\n"));
	result = Im_MXIC_Get_Slot_Priority((E_IM_MXIC_UNIT)7, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_LEVEL_CTRL_2, &priority);
	Ddim_Print(("result=%d\n", result));

	// priority NULL check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Priority(): priority NULL check]----\n"));
	result = Im_MXIC_Get_Slot_Priority((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_LEVEL_CTRL_2, NULL);
	Ddim_Print(("result=%d\n", result));

	// write or read arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Priority(): wr_arbiter range error check]----\n"));
	result = Im_MXIC_Get_Slot_Priority((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_MAX, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_LEVEL_CTRL_2, &priority);
	Ddim_Print(("result=%d\n", result));
	
	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Priority(): arbiter range error check]----\n"));
	result = Im_MXIC_Get_Slot_Priority((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_MAX, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_LEVEL_CTRL_2, &priority);
	Ddim_Print(("result=%d\n", result));

	// port range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Priority(): port range error check]----\n"));
	result = Im_MXIC_Get_Slot_Priority((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_MAX, E_IM_MXIC_SLOT_LEVEL_CTRL_1, &priority);
	Ddim_Print(("result=%d\n", result));
	
	// level_number range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Priority(): level_number range error check]----\n"));
	result = Im_MXIC_Get_Slot_Priority((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_1, E_IM_MXIC_SLOT_LEVEL_CTRL_MAX, &priority);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Slot_Priority_All_Arbiter and Im_MXIC_Get_Slot_Priority_All_Arbiter function.
static VOID ct_im_mxic_check_err_slot_priority_all_arbiter(VOID)
{
	INT32								result;
	T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL	all_level_ctrl;

	// --- Im_MXIC_Set_Slot_Priority_All_Arbiter ---
	// all_priority unit check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Priority_All_Arbiter(): all_priority unit check]----\n"));
	result = Im_MXIC_Set_Slot_Priority_All_Arbiter((E_IM_MXIC_UNIT)7, &all_level_ctrl);
	Ddim_Print(("result=%d\n", result));

	// all_priority NULL check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Priority_All_Arbiter(): all_priority NULL check]----\n"));
	result = Im_MXIC_Set_Slot_Priority_All_Arbiter((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Slot_Priority_All_Arbiter ---
	// all_priority unit check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Priority_All_Arbiter(): all_priority unit check]----\n"));
	result = Im_MXIC_Get_Slot_Priority_All_Arbiter((E_IM_MXIC_UNIT)7, &all_level_ctrl);
	Ddim_Print(("result=%d\n", result));
#if 1
	// all_priority NULL check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Priority_All_Arbiter(): all_priority NULL check]----\n"));
	result = Im_MXIC_Get_Slot_Priority_All_Arbiter((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));
#endif
	return;
}

// Error test of Im_MXIC_Set_Master_Priority and Im_MXIC_Get_Master_Priority function.
static VOID ct_im_mxic_check_err_master_priority(VOID)
{
	INT32	result;
	UCHAR	master_priority;

	// --- Im_MXIC_Set_Master_Priority ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_Priority(): unit range error check]----\n"));
	result = Im_MXIC_Set_Master_Priority((E_IM_MXIC_UNIT)7, D_IM_MXIC_0_W_SEN_0, D_IM_MXIC_MASTER_MOVE_15);
	Ddim_Print(("result=%d\n", result));

	// priority range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_Priority(): priority range error check]----\n"));
	result = Im_MXIC_Set_Master_Priority((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, (D_IM_MXIC_MASTER_MOVE_15 + 1));
	Ddim_Print(("result=%d\n", result));

	// priority unit check
	Ddim_Print(("----[Im_MXIC_Get_Master_Priority(): unit range error check]----\n"));
	result = Im_MXIC_Get_Master_Priority((E_IM_MXIC_UNIT)7, D_IM_MXIC_0_W_SEN_0, &master_priority);
	Ddim_Print(("result=%d\n", result));

	// priority NULL check
	Ddim_Print(("----[Im_MXIC_Get_Master_Priority(): priority range error check]----\n"));
	result = Im_MXIC_Get_Master_Priority((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Slot_Mask and Im_MXIC_Get_Slot_Mask function.
static VOID ct_im_mxic_check_err_slot_mask(VOID)
{
	INT32 result;
	UCHAR mask_enable;

	// --- Im_MXIC_Set_Slot_Mask ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Mask(): unit range error check]----\n"));
	result = Im_MXIC_Set_Slot_Mask((E_IM_MXIC_UNIT)7, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_0, D_IM_MXIC_ON);
	Ddim_Print(("result=%d\n", result));

	// write or read arbiter range error check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Mask(): wr_arbiter range error check]----\n"));
	result = Im_MXIC_Set_Slot_Mask((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_MAX, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_0, D_IM_MXIC_ON);
	Ddim_Print(("result=%d\n", result));

	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Mask(): arbiter range error check]----\n"));
	result = Im_MXIC_Set_Slot_Mask((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_MAX, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_0, D_IM_MXIC_ON);
	Ddim_Print(("result=%d\n", result));

	// slot range error check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Mask(): port range error check]----\n"));
	result = Im_MXIC_Set_Slot_Mask((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_MAX, E_IM_MXIC_SLOT_0, D_IM_MXIC_ON);
	Ddim_Print(("result=%d\n", result));

	// slot range error check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Mask(): slot range error check]----\n"));
	result = Im_MXIC_Set_Slot_Mask((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_MAX, D_IM_MXIC_ON);
	Ddim_Print(("result=%d\n", result));

	// mask_enable range error check
	Ddim_Print(("----[Im_MXIC_Set_Slot_Mask(): mask_enable range error check]----\n"));
	result = Im_MXIC_Set_Slot_Mask((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_3, 2);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Slot_Mask ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Mask(): unit range error check]----\n"));
	result = Im_MXIC_Get_Slot_Mask((E_IM_MXIC_UNIT)7, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_0, &mask_enable);
	Ddim_Print(("result=%d\n", result));

	// write or read arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Mask(): wr_arbiter range error check]----\n"));
	result = Im_MXIC_Get_Slot_Mask((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_MAX, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_0, &mask_enable);
	Ddim_Print(("result=%d\n", result));

	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Mask(): arbiter range error check]----\n"));
	result = Im_MXIC_Get_Slot_Mask((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_MAX, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_0, &mask_enable);
	Ddim_Print(("result=%d\n", result));

	// port range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Mask(): port range error check]----\n"));
	result = Im_MXIC_Get_Slot_Mask((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_MAX, E_IM_MXIC_SLOT_0, &mask_enable);
	Ddim_Print(("result=%d\n", result));
	
	// slot range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Mask(): slot range error check]----\n"));
	result = Im_MXIC_Get_Slot_Mask((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_R, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_MAX, &mask_enable);
	Ddim_Print(("result=%d\n", result));

	// mask_enable NULL check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Mask(): mask_enable NULL check]----\n"));
	result = Im_MXIC_Get_Slot_Mask((E_IM_MXIC_UNIT)0, E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_2, E_IM_MXIC_PORT_0, E_IM_MXIC_SLOT_3, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Master_Mask and Im_MXIC_Get_Master_Mask function.
static VOID ct_im_mxic_check_err_master_mask(VOID)
{
	INT32					result;
	T_IM_MXIC_MASTER_MASK	master_mask;

	// --- Im_MXIC_Set_Master_Mask ---
	// mask unit check
	Ddim_Print(("----[Im_MXIC_Set_Master_Mask(): mask unit check]----\n"));
	result = Im_MXIC_Set_Master_Mask((E_IM_MXIC_UNIT)7, D_IM_MXIC_0_W_SEN_0, &master_mask);
	Ddim_Print(("result=%d\n", result));

	// mask NULL check
	Ddim_Print(("----[Im_MXIC_Set_Master_Mask(): mask NULL check]----\n"));
	result = Im_MXIC_Set_Master_Mask((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, NULL);
	Ddim_Print(("result=%d\n", result));
	
	// --- Im_MXIC_Get_Master_Mask ---
	// mask unit check
	Ddim_Print(("----[Im_MXIC_Get_Master_Mask(): mask unit check]----\n"));
	result = Im_MXIC_Get_Master_Mask((E_IM_MXIC_UNIT)7, D_IM_MXIC_0_W_SEN_0, &master_mask);
	Ddim_Print(("result=%d\n", result));

	// mask NULL check
	Ddim_Print(("----[Im_MXIC_Get_Master_Mask(): mask NULL check]----\n"));
	result = Im_MXIC_Get_Master_Mask((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Master_Mask_Factor and Im_MXIC_Get_Master_Mask_Factor function.
static VOID ct_im_mxic_check_err_master_mask_factor(VOID)
{
	INT32 result;
	UCHAR enable;

	// --- Im_MXIC_Set_Master_Mask_Factor ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_Mask_Factor(): unit range error check]----\n"));
	result = Im_MXIC_Set_Master_Mask_Factor((E_IM_MXIC_UNIT)7, D_IM_MXIC_0_W_SEN_0, E_IM_MXIC_MASK_GROUP_B, D_IM_MXIC_ON);
	Ddim_Print(("result=%d\n", result));

	// group range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_Mask_Factor(): group range error check]----\n"));
	result = Im_MXIC_Set_Master_Mask_Factor((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, E_IM_MXIC_MASK_GROUP_MAX, D_IM_MXIC_ON);
	Ddim_Print(("result=%d\n", result));

	// enable range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_Mask_Factor(): enable range error check]----\n"));
	result = Im_MXIC_Set_Master_Mask_Factor((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, E_IM_MXIC_MASK_GROUP_B, 2);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Master_Mask_Factor ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Get_Master_Mask_Factor(): unit range error check]----\n"));
	result = Im_MXIC_Get_Master_Mask_Factor((E_IM_MXIC_UNIT)7, D_IM_MXIC_0_W_SEN_0, E_IM_MXIC_MASK_GROUP_B, &enable);
	Ddim_Print(("result=%d\n", result));

	// group range error check
	Ddim_Print(("----[Im_MXIC_Get_Master_Mask_Factor(): group range error check]----\n"));
	result = Im_MXIC_Get_Master_Mask_Factor((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, E_IM_MXIC_MASK_GROUP_MAX, &enable);
	Ddim_Print(("result=%d\n", result));

	// enable range error check
	Ddim_Print(("----[Im_MXIC_Get_Master_Mask_Factor(): enable range error check]----\n"));
	result = Im_MXIC_Get_Master_Mask_Factor((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, E_IM_MXIC_MASK_GROUP_B, NULL);
	Ddim_Print(("result=%d\n", result));
	
	return;
}

// Error test of Im_MXIC_Set_Master_Mask_Target and Im_MXIC_Get_Master_Mask_Target function.
static VOID ct_im_mxic_check_err_master_mask_target(VOID)
{
	INT32 result;
	UCHAR enable;

	// --- Im_MXIC_Set_Master_Mask_Target ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_Mask_Target(): unit range error check]----\n"));
	result = Im_MXIC_Set_Master_Mask_Target((E_IM_MXIC_UNIT)7, D_IM_MXIC_0_W_SEN_0, E_IM_MXIC_MASK_GROUP_B, D_IM_MXIC_ON);
	Ddim_Print(("result=%d\n", result));

	// group range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_Mask_Target(): group range error check]----\n"));
	result = Im_MXIC_Set_Master_Mask_Target((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, E_IM_MXIC_MASK_GROUP_MAX, D_IM_MXIC_ON);
	Ddim_Print(("result=%d\n", result));

	// enable range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_Mask_Target(): enable range error check]----\n"));
	result = Im_MXIC_Set_Master_Mask_Target((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, E_IM_MXIC_MASK_GROUP_B, 2);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_Master_Mask_Target ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Get_Master_Mask_Target(): unit range error check]----\n"));
	result = Im_MXIC_Get_Master_Mask_Target((E_IM_MXIC_UNIT)7, D_IM_MXIC_0_W_SEN_0, E_IM_MXIC_MASK_GROUP_B, &enable);
	Ddim_Print(("result=%d\n", result));

	// group range error check
	Ddim_Print(("----[Im_MXIC_Get_Master_Mask_Target(): group range error check]----\n"));
	result = Im_MXIC_Get_Master_Mask_Target((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, E_IM_MXIC_MASK_GROUP_MAX, &enable);
	Ddim_Print(("result=%d\n", result));

	// enable range error check
	Ddim_Print(("----[Im_MXIC_Get_Master_Mask_Target(): enable range error check]----\n"));
	result = Im_MXIC_Get_Master_Mask_Target((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, E_IM_MXIC_MASK_GROUP_B, NULL);
	Ddim_Print(("result=%d\n", result));
	
	return;
}

// Error test of Im_MXIC_Set_Master_Mask_Select and Im_MXIC_Get_Master_Mask_Select function.
static VOID ct_im_mxic_check_err_master_mask_select(VOID)
{
	INT32	result;
	UCHAR	mask_select;

	// --- Im_MXIC_Set_Master_Mask_Select ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_Mask_Select(): unit range error check]----\n"));
	result = Im_MXIC_Set_Master_Mask_Select((E_IM_MXIC_UNIT)7, D_IM_MXIC_0_W_SEN_0, 1);
	Ddim_Print(("result=%d\n", result));

	// select range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_Mask_Select(): select range error check]----\n"));
	result = Im_MXIC_Set_Master_Mask_Select((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, 2);
	Ddim_Print(("result=%d\n", result));
	
	// --- Im_MXIC_Get_Master_Mask_Select ---
	// select unit check
	Ddim_Print(("----[Im_MXIC_Get_Master_Mask_Select(): select unit check]----\n"));
	result = Im_MXIC_Get_Master_Mask_Select((E_IM_MXIC_UNIT)7, D_IM_MXIC_0_W_SEN_0, &mask_select);
	Ddim_Print(("result=%d\n", result));

	// select NULL check
	Ddim_Print(("----[Im_MXIC_Get_Master_Mask_Select(): select NULL check]----\n"));
	result = Im_MXIC_Get_Master_Mask_Select((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Master_Transaction and Im_MXIC_Get_Master_Transaction function.
static VOID ct_im_mxic_check_err_master_transaction(VOID)
{
	INT32	result;
	UCHAR	transaction;

	// --- Im_MXIC_Set_Master_Transaction ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_Mask(): unit range error check]----\n"));
	result = Im_MXIC_Set_Master_Transaction((E_IM_MXIC_UNIT)7, D_IM_MXIC_0_W_SEN_0, 63);
	Ddim_Print(("result=%d\n", result));

	// transaction range error check
	Ddim_Print(("----[Im_MXIC_Set_Master_Mask(): transaction range error check]----\n"));
	result = Im_MXIC_Set_Master_Transaction((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, 64);
	Ddim_Print(("result=%d\n", result));
	
	// --- Im_MXIC_Get_Master_Transaction ---
	// transaction unit check
	Ddim_Print(("----[Im_MXIC_Get_Master_Mask(): mask unit check]----\n"));
	result = Im_MXIC_Get_Master_Transaction((E_IM_MXIC_UNIT)7, D_IM_MXIC_0_W_SEN_0, &transaction);
	Ddim_Print(("result=%d\n", result));

	// transaction NULL check
	Ddim_Print(("----[Im_MXIC_Get_Master_Mask(): mask NULL check]----\n"));
	result = Im_MXIC_Get_Master_Transaction((E_IM_MXIC_UNIT)0, D_IM_MXIC_0_W_SEN_0, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_History_Monitor_Stop_Enable and Im_MXIC_Get_History_Monitor_Stop_Enable function.
static VOID ct_im_mxic_check_err_history_monitor_stop_enable(VOID)
{
	INT32 result;

	// --- Im_MXIC_Set_History_Monitor_Stop_Enable ---
	// enable range error check
	Ddim_Print(("----[Im_MXIC_Set_History_Monitor_Stop_Enable(): enable range error check]----\n"));
	result = Im_MXIC_Set_History_Monitor_Stop_Enable(2);
	Ddim_Print(("result=%d\n", result));

	// --- Im_MXIC_Get_History_Monitor_Stop_Enable ---
	// enable NULL check
	Ddim_Print(("----[Im_MXIC_Get_History_Monitor_Stop_Enable(): enable NULL check]----\n"));
	result = Im_MXIC_Get_History_Monitor_Stop_Enable(NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Get_History_Monitor function.
static VOID ct_im_mxic_check_err_history_monitor(VOID)
{
	INT32 result;
	T_IM_MXIC_HISTORY_MONITOR history;

	// --- Im_MXIC_Get_History_Monitor ---
	// write or read arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_History_Monitor(): wr_arbiter range error check]----\n"));
	result = Im_MXIC_Get_History_Monitor(E_IM_MXIC_WR_ARBITER_MAX, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_0, &history);
	Ddim_Print(("result=%d\n", result));
	
	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_History_Monitor(): arbiter range error check]----\n"));
	result = Im_MXIC_Get_History_Monitor(E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_MAX, E_IM_MXIC_PORT_0, &history);
	Ddim_Print(("result=%d\n", result));

	// port range error check
	Ddim_Print(("----[Im_MXIC_Get_History_Monitor(): port range error check]----\n"));
	result = Im_MXIC_Get_History_Monitor(E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_MAX, &history);
	Ddim_Print(("result=%d\n", result));

	// history NULL check
	Ddim_Print(("----[Im_MXIC_Get_History_Monitor(): history NULL check]----\n"));
	result = Im_MXIC_Get_History_Monitor(E_IM_MXIC_WR_ARBITER_W, E_IM_MXIC_SPEC_ARBITER_1, E_IM_MXIC_PORT_1, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Get_History_Monitor_All_Port function.
static VOID ct_im_mxic_check_err_history_monitor_all_port(VOID)
{
	INT32 result;

	// --- Im_MXIC_Get_History_Monitor_All_Port ---
	// all_history NULL check
	Ddim_Print(("----[Im_MXIC_Get_History_Monitor_All_Port(): all_history NULL check]----\n"));
	result = Im_MXIC_Get_History_Monitor_All_Port(NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter function.
static VOID ct_im_mxic_check_err_slot_status_monitor_all_arbiter(VOID)
{
	INT32								result;
	T_IM_MXIC_ALL_SLOT_STATUS_MONITOR	all_slot_status;

	// --- Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter ---
	// all_status unit check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter(): all_status unit check]----\n"));
	result = Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter((E_IM_MXIC_UNIT)7, &all_slot_status);
	Ddim_Print(("result=%d\n", result));

	// all_status NULL check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter(): all_status NULL check]----\n"));
	result = Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter function.
static VOID ct_im_mxic_check_err_slot_status_monitor_w_arbiter(VOID)
{
	INT32 result;
	T_IM_MXIC_W_ARBITER_ASSIGN_PORT status;

	// --- Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter(): unit range error check]----\n"));
	result = Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter((E_IM_MXIC_UNIT)7, E_IM_MXIC_W_ARBITER_W2, &status);
	Ddim_Print(("result=%d\n", result));

	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter(): arbiter range error check]----\n"));
	result = Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter((E_IM_MXIC_UNIT)0, E_IM_MXIC_W_ARBITER_MAX, &status);
	Ddim_Print(("result=%d\n", result));

	// status NULL check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter(): status NULL check]----\n"));
	result = Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter((E_IM_MXIC_UNIT)0, E_IM_MXIC_W_ARBITER_W2, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter function.
static VOID ct_im_mxic_check_err_slot_status_monitor_r_arbiter(VOID)
{
	INT32 result;
	T_IM_MXIC_R_ARBITER_ASSIGN_PORT status;

	// --- Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter ---
	// unit range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter(): unit range error check]----\n"));
	result = Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter((E_IM_MXIC_UNIT)7, E_IM_MXIC_R_ARBITER_R2, &status);
	Ddim_Print(("result=%d\n", result));

	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter(): arbiter range error check]----\n"));
	result = Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter((E_IM_MXIC_UNIT)0, E_IM_MXIC_R_ARBITER_MAX, &status);
	Ddim_Print(("result=%d\n", result));

	// status NULL check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter(): status NULL check]----\n"));
	result = Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter((E_IM_MXIC_UNIT)0, E_IM_MXIC_R_ARBITER_R2, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group function.
static VOID ct_im_mxic_check_err_slot_status_monitor_w_arbiter_group(VOID)
{
	INT32 result;
	T_IM_MXIC_W_ARBITER_ASSIGN_GR status;

	// --- Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group ---
	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group(): arbiter range error check]----\n"));
	result = Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group(E_IM_MXIC_W_ARBITER_GR_MAX, &status);
	Ddim_Print(("result=%d\n", result));

	// status NULL check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group(): status NULL check]----\n"));
	result = Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group(E_IM_MXIC_W_ARBITER_GR_W1, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group function.
static VOID ct_im_mxic_check_err_slot_status_monitor_r_arbiter_group(VOID)
{
	INT32 result;
	T_IM_MXIC_R_ARBITER_ASSIGN_GR status;

	// --- Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group ---
	// arbiter range error check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group(): arbiter range error check]----\n"));
	result = Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group(E_IM_MXIC_R_ARBITER_GR_MAX, &status);
	Ddim_Print(("result=%d\n", result));

	// status NULL check
	Ddim_Print(("----[Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group(): status NULL check]----\n"));
	result = Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group(E_IM_MXIC_R_ARBITER_GR_R1, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Get_Master_Status_Monitor function.
static VOID ct_im_mxic_check_err_master_status_monitor(VOID)
{
	INT32							result;
	T_IM_MXIC_MASTER_STATUS_MONITOR	master_status;

	// --- Im_MXIC_Get_Master_Status_Monitor ---
	// status unit check
	Ddim_Print(("----[Im_MXIC_Get_Master_Status_Monitor(): status unit check]----\n"));
	result = Im_MXIC_Get_Master_Status_Monitor((E_IM_MXIC_UNIT)7, &master_status);
	Ddim_Print(("result=%d\n", result));

	// status NULL check
	Ddim_Print(("----[Im_MXIC_Get_Master_Status_Monitor(): status NULL check]----\n"));
	result = Im_MXIC_Get_Master_Status_Monitor((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter function.
static VOID ct_im_mxic_check_err_set_access_or_trans_monitor_parameter(VOID)
{
	INT32 result;
	T_IM_MXIC_MONITOR_PARAMETER param;

	// Initialize
	param.start_condition          = D_IM_MXIC_MONITOR_START_TRG;
	param.end_condition            = D_IM_MXIC_MONITOR_END_LIMIT;
	param.update_enable            = D_IM_MXIC_OFF;
	param.update_trigger_interval  = D_IM_MXIC_MONITOR_UPDATE_1;
	param.update_clear_en          = D_IM_MXIC_OFF;
	param.start_clear_en           = D_IM_MXIC_OFF;
	param.cnt_val                  = D_IM_MXIC_TRANS_COUNT_VALUE;
	param.limit_access_trans       = D_IM_MXIC_MONITOR_TRANSFER;
	param.limit_detection_target   = D_IM_MXIC_SLAVE_W1_PORT0;
	param.limit                    = 0xFFFFFFFF;
	param.target[0].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
	param.target[0].monitor_target = D_IM_MXIC_SLAVE_W1_PORT0;
	param.target[1].monitor_sel    = D_IM_MXIC_MONITOR_ACCESS;
	param.target[1].monitor_target = D_IM_MXIC_SLAVE_W2_PORT1;
	param.target[2].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
	param.target[2].monitor_target = D_IM_MXIC_SLAVE_R1_PORT0;
	param.target[3].monitor_sel    = D_IM_MXIC_MONITOR_ACCESS;
	param.target[3].monitor_target = D_IM_MXIC_SLAVE_R2_PORT1;
	param.pCallBack                = NULL;

	// --- Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter ---
	// param NULL check
	Ddim_Print(("----[Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter(): param NULL check]----\n"));
	result = Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter(NULL);
	Ddim_Print(("result=%d\n", result));

	// monitor_target range error check
	Ddim_Print(("----[Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter(): monitor_target range error check]----\n"));
	param.target[3].monitor_target = 0x100;
	result = Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter(&param);
	param.target[3].monitor_target = D_IM_MXIC_SLAVE_R2_PORT1;
	Ddim_Print(("result=%d\n", result));

	// limit_detection_target range error check
	Ddim_Print(("----[Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter(): limit_detection_target range error check]----\n"));
	param.limit_detection_target = 0x180;
	result = Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter(&param);
	param.limit_detection_target = D_IM_MXIC_SLAVE_W1_PORT0;
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter function.
static VOID ct_im_mxic_check_err_get_access_or_trans_monitor_parameter(VOID)
{
	INT32 result;

	// --- Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter ---
	// param NULL check
	Ddim_Print(("----[Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter(): param NULL check]----\n"));
	result = Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter(NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Get_Access_Or_Trans_Monitor function.
static VOID ct_im_mxic_check_err_access_or_trans_monitor(VOID)
{
	INT32 result;
	UINT32 monitor_result;

	// --- Im_MXIC_Get_Access_Or_Trans_Monitor ---
	// entry range error check
	Ddim_Print(("----[Im_MXIC_Get_Access_Or_Trans_Monitor(): entry range error check]----\n"));
	result = Im_MXIC_Get_Access_Or_Trans_Monitor(4, &monitor_result);
	Ddim_Print(("result=%d\n", result));

	// result NULL check
	Ddim_Print(("----[Im_MXIC_Get_Access_Or_Trans_Monitor(): result NULL check]----\n"));
	result = Im_MXIC_Get_Access_Or_Trans_Monitor(0, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry function.
static VOID ct_im_mxic_check_err_access_or_trans_monitor_all_entry(VOID)
{
	INT32 result;

	// --- Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry ---
	// all_result NULL check
	Ddim_Print(("----[Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry(): all_result NULL check]----\n"));
	result = Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry(NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State function.
static VOID ct_im_mxic_check_err_sccess_or_trans_monitor_limit_end_state(VOID)
{
	INT32 result;

	// --- Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State ---
	// mon_state NULL check
	Ddim_Print(("----[Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State(): mon_state NULL check]----\n"));
	result = Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State(NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Start_Memory_Access_Detect function.
static VOID ct_im_mxic_check_err_memory_access_start(VOID)
{
	INT32							result;
	T_IM_MXIC_MEMORY_ACCESS_SLAVE	memory_access;

	// --- Im_MXIC_Start_Memory_Access_Detect ---
	// param unit check
	Ddim_Print(("----[Im_MXIC_Start_Memory_Access_Detect(): param unit check]----\n"));
	result = Im_MXIC_Start_Memory_Access_Detect((E_IM_MXIC_UNIT)7, &memory_access);
	Ddim_Print(("result=%d\n", result));

	// param NULL check
	Ddim_Print(("----[Im_MXIC_Start_Memory_Access_Detect(): param NULL check]----\n"));
	result = Im_MXIC_Start_Memory_Access_Detect((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Set_Memory_Access_Detect function.
static VOID ct_im_mxic_check_err_memory_access_set(VOID)
{
	INT32							result;
	T_IM_MXIC_MEMORY_ACCESS_SLAVE	memory_access;

	// --- Im_MXIC_Set_Memory_Access_Detect ---
	// param unit check
	Ddim_Print(("----[Im_MXIC_Set_Memory_Access_Detect(): param unit check]----\n"));
	result = Im_MXIC_Set_Memory_Access_Detect((E_IM_MXIC_UNIT)7, &memory_access);
	Ddim_Print(("result=%d\n", result));

	// param NULL check
	Ddim_Print(("----[Im_MXIC_Set_Memory_Access_Detect(): param NULL check]----\n"));
	result = Im_MXIC_Set_Memory_Access_Detect((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Error test of Im_MXIC_Get_Memory_Access_Detect function.
static VOID ct_im_mxic_check_err_memory_access_get(VOID)
{
	INT32							result;
	T_IM_MXIC_MEMORY_ACCESS_SLAVE	memory_access;

	// --- Im_MXIC_Get_Memory_Access_Detect ---
	// param unit check
	Ddim_Print(("----[Im_MXIC_Get_Memory_Access_Detect(): param unit check]----\n"));
	result = Im_MXIC_Get_Memory_Access_Detect((E_IM_MXIC_UNIT)7, &memory_access);
	Ddim_Print(("result=%d\n", result));

	// param NULL check
	Ddim_Print(("----[Im_MXIC_Get_Memory_Access_Detect(): param NULL check]----\n"));
	result = Im_MXIC_Get_Memory_Access_Detect((E_IM_MXIC_UNIT)0, NULL);
	Ddim_Print(("result=%d\n", result));

	return;
}

// Create parameter for Im_MXIC_Set_Slot_Priority_All_Arbiter function test.
static INT32 ct_im_mxic_create_slot_priority_all_arbiter_param(T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL* const all_level_ctrl)
{
	INT32 i, j;

	// Write Slave
	for (i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++) {
		for (j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++) {
			all_level_ctrl->w_level[i].level0[j].priority_level_type  = D_IM_MXIC_SLOT_PRIORITY_UPPER_ONLY;
			all_level_ctrl->w_level[i].level0[j].upper_priority_level = 0;
			all_level_ctrl->w_level[i].level0[j].lower_priority_level = 63;
			all_level_ctrl->w_level[i].level1[j].priority_level_type  = D_IM_MXIC_SLOT_PRIORITY_UPPER;
			all_level_ctrl->w_level[i].level1[j].upper_priority_level = 32;
			all_level_ctrl->w_level[i].level1[j].lower_priority_level = 1;
			all_level_ctrl->w_level[i].level2[j].priority_level_type  = D_IM_MXIC_SLOT_PRIORITY_LOWWER;
			all_level_ctrl->w_level[i].level2[j].upper_priority_level = 63;
			all_level_ctrl->w_level[i].level2[j].lower_priority_level = 0;
		}
	}
	// Read Slave
	for (i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++) {
		for (j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++) {
			all_level_ctrl->r_level[i].level0[j].priority_level_type  = D_IM_MXIC_SLOT_PRIORITY_LOWWER_ONLY;
			all_level_ctrl->r_level[i].level0[j].upper_priority_level = 63;
			all_level_ctrl->r_level[i].level0[j].lower_priority_level = 0;
			all_level_ctrl->r_level[i].level1[j].priority_level_type  = D_IM_MXIC_SLOT_PRIORITY_LOWWER;
			all_level_ctrl->r_level[i].level1[j].upper_priority_level = 0;
			all_level_ctrl->r_level[i].level1[j].lower_priority_level = 63;
			all_level_ctrl->r_level[i].level2[j].priority_level_type  = D_IM_MXIC_SLOT_PRIORITY_UPPER;
			all_level_ctrl->r_level[i].level2[j].upper_priority_level = 1;
			all_level_ctrl->r_level[i].level2[j].lower_priority_level = 32;
		}
	}

	return D_DDIM_OK;
}

static INT32 ct_im_mxic_create_all_levelport_param(T_IM_MXIC_ALL_LEVELPORT* const all_levelport)
{
	INT32 i, j;

	for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

		for ( j = 0; j < D_IM_MXIC_MASTER_NUMBER_MAX-1; j++ ) {

			all_levelport->w_arbiter[ i ].port[ j ]		= E_IM_MXIC_PORT_1;
			all_levelport->w_arbiter[ i ].level[ j ]	= E_IM_MXIC_LEVEL_1;
		}
	}

	for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {

		for ( j = 0; j < D_IM_MXIC_MASTER_NUMBER_MAX-1; j++ ) {

			all_levelport->r_arbiter[ i ].port[ j ]		= E_IM_MXIC_PORT_2;
			all_levelport->r_arbiter[ i ].level[ j ]	= E_IM_MXIC_LEVEL_2;
		}
	}

	return D_DDIM_OK;
}

// Callback function for monitor end.
static VOID ct_im_mxic_acc_monitor_callback(VOID)
{
	Ddim_Print(("The monitor end interrupt was generated.\n"));
}

// Create parameter for Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter function test.
static INT32 ct_im_mxic_create_access_or_trans_monitor_param(UCHAR param_kind, T_IM_MXIC_MONITOR_PARAMETER* const mon_param)
{
	INT32 result;

	result = D_DDIM_OK;

	switch (param_kind) {
		case 0:
			// Monitor mode register. (TMMD)
			mon_param->start_condition          = D_IM_MXIC_MONITOR_START_ON;
			mon_param->end_condition            = D_IM_MXIC_MONITOR_ENDLESS;
			mon_param->update_enable            = D_IM_MXIC_OFF;
			mon_param->update_trigger_interval  = D_IM_MXIC_MONITOR_UPDATE_1;
			mon_param->update_clear_en          = D_IM_MXIC_OFF;
			mon_param->start_clear_en           = D_IM_MXIC_OFF;
			mon_param->cnt_val                  = D_IM_MXIC_VALUE_WHEN_UPDATE_TRG;

			// Limit judgment conditions.
			mon_param->limit_access_trans       = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->limit_detection_target   = D_IM_MXIC_W_RESERVE;
			mon_param->limit                    = 0x00000001;

			// Monitor target and kind(Access or Transfer).
			mon_param->target[0].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[1].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[2].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[3].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[0].monitor_target = D_IM_MXIC_4_R_DISP_A;
			mon_param->target[1].monitor_target = D_IM_MXIC_TARGET_NOTHING;
			mon_param->target[2].monitor_target = D_IM_MXIC_TARGET_NOTHING;
			mon_param->target[3].monitor_target = D_IM_MXIC_TARGET_NOTHING;

			// Callback for TMF interrupt.
			mon_param->pCallBack = ct_im_mxic_acc_monitor_callback;
			break;

		case 1:
			// Monitor mode register. (TMMD)
			mon_param->start_condition          = D_IM_MXIC_MONITOR_START_TRG;
			mon_param->end_condition            = D_IM_MXIC_MONITOR_END_LIMIT;
			mon_param->update_enable            = D_IM_MXIC_OFF;
			mon_param->update_trigger_interval  = D_IM_MXIC_MONITOR_UPDATE_512;
			mon_param->update_clear_en          = D_IM_MXIC_OFF;
			mon_param->start_clear_en           = D_IM_MXIC_ON;
			mon_param->cnt_val                  = D_IM_MXIC_TRANS_COUNT_VALUE;

			// Limit judgment conditions.
			mon_param->limit_access_trans       = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->limit_detection_target   = D_IM_MXIC_SLAVE_R1_PORT0;
			mon_param->limit                    = 0x08000000;

			// Monitor target and kind(Access or Transfer).
			mon_param->target[0].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[1].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[2].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[3].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[0].monitor_target = D_IM_MXIC_SLAVE_W1_PORT0;
			mon_param->target[1].monitor_target = D_IM_MXIC_SLAVE_W1_PORT1;
			mon_param->target[2].monitor_target = D_IM_MXIC_SLAVE_R1_PORT0;
			mon_param->target[3].monitor_target = D_IM_MXIC_SLAVE_R1_PORT1;

			// Callback for TMF interrupt.
			mon_param->pCallBack = ct_im_mxic_acc_monitor_callback;
			break;

		case 2:
			// Monitor mode register. (TMMD)
			mon_param->start_condition          = D_IM_MXIC_MONITOR_START_TRG;
			mon_param->end_condition            = D_IM_MXIC_MONITOR_END_ASSERT;
			mon_param->update_enable            = D_IM_MXIC_OFF;
			mon_param->update_trigger_interval  = D_IM_MXIC_MONITOR_UPDATE_32;
			mon_param->update_clear_en          = D_IM_MXIC_OFF;
			mon_param->start_clear_en           = D_IM_MXIC_ON;
			mon_param->cnt_val                  = D_IM_MXIC_TRANS_COUNT_VALUE;

			// Limit judgment conditions.
			mon_param->limit_access_trans       = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->limit_detection_target   = D_IM_MXIC_R_RESERVE;
			mon_param->limit                    = 0x00000001;

			// Monitor target and kind(Access or Transfer).
			mon_param->target[0].monitor_sel    = D_IM_MXIC_MONITOR_ACCESS;
			mon_param->target[1].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[2].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[3].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[0].monitor_target = D_IM_MXIC_4_R_DISP_A;
			mon_param->target[1].monitor_target = D_IM_MXIC_SLAVE_W1_PORT0;
			mon_param->target[2].monitor_target = D_IM_MXIC_SLAVE_R1_PORT0;
			mon_param->target[3].monitor_target = D_IM_MXIC_TARGET_NOTHING;

			// Callback for TMF interrupt.
			mon_param->pCallBack = ct_im_mxic_acc_monitor_callback;
			break;

		case 3:
			// Monitor mode register. (TMMD)
			mon_param->start_condition          = D_IM_MXIC_MONITOR_START_ON;
			mon_param->end_condition            = D_IM_MXIC_MONITOR_ENDLESS;
			mon_param->update_enable            = D_IM_MXIC_OFF;
			mon_param->update_trigger_interval  = D_IM_MXIC_MONITOR_UPDATE_1;
			mon_param->update_clear_en          = D_IM_MXIC_OFF;
			mon_param->start_clear_en           = D_IM_MXIC_OFF;
			mon_param->cnt_val                  = D_IM_MXIC_VALUE_WHEN_UPDATE_TRG;

			// Limit judgment conditions.
			mon_param->limit_access_trans       = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->limit_detection_target   = D_IM_MXIC_W_RESERVE;
			mon_param->limit                    = 0x00000001;

			// Monitor target and kind(Access or Transfer).
			mon_param->target[0].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[1].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[2].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[3].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[0].monitor_target = D_IM_MXIC_3_R_IIP_A;
			mon_param->target[1].monitor_target = D_IM_MXIC_TARGET_NOTHING;
			mon_param->target[2].monitor_target = D_IM_MXIC_TARGET_NOTHING;
			mon_param->target[3].monitor_target = D_IM_MXIC_TARGET_NOTHING;

			// Callback for TMF interrupt.
			mon_param->pCallBack = ct_im_mxic_acc_monitor_callback;
			break;

		case 4:
			// Monitor mode register. (TMMD)
			mon_param->start_condition          = D_IM_MXIC_MONITOR_START_TRG;
			mon_param->end_condition            = D_IM_MXIC_MONITOR_END_LIMIT;
			mon_param->update_enable            = D_IM_MXIC_OFF;
			mon_param->update_trigger_interval  = D_IM_MXIC_MONITOR_UPDATE_512;
			mon_param->update_clear_en          = D_IM_MXIC_OFF;
			mon_param->start_clear_en           = D_IM_MXIC_ON;
			mon_param->cnt_val                  = D_IM_MXIC_TRANS_COUNT_VALUE;

			// Limit judgment conditions.
			mon_param->limit_access_trans       = D_IM_MXIC_MONITOR_ACCESS;
			mon_param->limit_detection_target   = D_IM_MXIC_SLAVE_R1_PORT0;
			mon_param->limit                    = 0x00100000;

			// Monitor target and kind(Access or Transfer).
			mon_param->target[0].monitor_sel    = D_IM_MXIC_MONITOR_ACCESS;
			mon_param->target[1].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[2].monitor_sel    = D_IM_MXIC_MONITOR_ACCESS;
			mon_param->target[3].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
			mon_param->target[0].monitor_target = D_IM_MXIC_SLAVE_W1_PORT0;
			mon_param->target[1].monitor_target = D_IM_MXIC_SLAVE_W1_PORT1;
			mon_param->target[2].monitor_target = D_IM_MXIC_SLAVE_R1_PORT0;
			mon_param->target[3].monitor_target = D_IM_MXIC_SLAVE_R1_PORT1;
			break;

		default:
			// Unknown parameter kind.
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
			break;
	}
	return result;
}

// Create parameter for Im_MXIC_Set_Master_W_Arbiter function test.
static INT32 ct_im_mxic_create_master_w_arbiter_param( E_IM_MXIC_UNIT unit, T_IM_MXIC_W_ARBITER_ASSIGN_PORT* const w_arbiter_assign )
{
	UINT32 i, j;

	for ( i = E_IM_MXIC_PORT_0; i < E_IM_MXIC_PORT_MAX ; i++ ) {
		for ( j = 0; j < D_IM_MXIC_SLOT_SIZE_8 ; j++ ) {
			w_arbiter_assign->port[ i ].slot0[ j ] = D_IM_MXIC_W_RESERVE;
			w_arbiter_assign->port[ i ].slot1[ j ] = D_IM_MXIC_W_RESERVE;
			w_arbiter_assign->port[ i ].slot2[ j ] = D_IM_MXIC_W_RESERVE;
			w_arbiter_assign->port[ i ].slot3[ j ] = D_IM_MXIC_W_RESERVE;
		}
	}

	switch ( unit ) {
		case E_IM_MXIC_UNIT_0:
			break;
		case E_IM_MXIC_UNIT_1:
			break;
		case E_IM_MXIC_UNIT_2:
			break;
		case E_IM_MXIC_UNIT_3:
			break;
		case E_IM_MXIC_UNIT_4:
			break;
		case E_IM_MXIC_UNIT_5:
			break;
		case E_IM_MXIC_UNIT_6:
			break;
		default:
			break;
	}

	return D_DDIM_OK;
}

// Create parameter for Im_MXIC_Set_Master_R_Arbiter function test.
static INT32 ct_im_mxic_create_master_r_arbiter_param( E_IM_MXIC_UNIT unit, T_IM_MXIC_R_ARBITER_ASSIGN_PORT* const r_arbiter_assign )
{
	UINT32 i, j;

	for ( i = E_IM_MXIC_PORT_0; i < E_IM_MXIC_PORT_MAX; i++ ) {
		for ( j = 0; j < D_IM_MXIC_SLOT_SIZE_8 ; j++ ) {
			r_arbiter_assign->port[ i ].slot0[ j ] = D_IM_MXIC_R_RESERVE;
			r_arbiter_assign->port[ i ].slot1[ j ] = D_IM_MXIC_R_RESERVE;
			r_arbiter_assign->port[ i ].slot2[ j ] = D_IM_MXIC_R_RESERVE;
			r_arbiter_assign->port[ i ].slot3[ j ] = D_IM_MXIC_R_RESERVE;
		}
	}

	switch ( unit ) {
		case E_IM_MXIC_UNIT_0:
			break;
		case E_IM_MXIC_UNIT_1:
			break;
		case E_IM_MXIC_UNIT_2:
			break;
		case E_IM_MXIC_UNIT_3:
			break;
		case E_IM_MXIC_UNIT_4:
			break;
		case E_IM_MXIC_UNIT_5:
			break;
		case E_IM_MXIC_UNIT_6:
			break;
		default:
			break;
	}

	return D_DDIM_OK;
}

// Create parameter for Im_MXIC_Set_Master_All_Arbiter function test.
static INT32 ct_im_mxic_create_master_all_arbiter_param( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_ARBITER_ASSIGN* const all_arbiter_assign )
{
	UINT32 i;

	for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {
		ct_im_mxic_create_master_w_arbiter_param( unit, &all_arbiter_assign->w_arbiter[ i ] );
	}

	for (i=E_IM_MXIC_R_ARBITER_R1; i<E_IM_MXIC_R_ARBITER_MAX; i++) {
		ct_im_mxic_create_master_r_arbiter_param( unit, &all_arbiter_assign->r_arbiter[ i ] );
	}

	return D_DDIM_OK;
}

// Create start_trigger for Im_MXIC_Start_Memory_Access_Detect function test.
static INT32 ct_im_mxic_create_memory_access_start_trigger(UINT32 start_param, T_IM_MXIC_MEMORY_ACCESS_SLAVE* const memacc_start)
{
	INT32 result;

	result = D_DDIM_OK;
	
	memacc_start->slave[0].start_trigger = D_IM_MXIC_OFF;
	memacc_start->slave[1].start_trigger = D_IM_MXIC_OFF;
	memacc_start->slave[2].start_trigger = D_IM_MXIC_OFF;
	memacc_start->slave[3].start_trigger = D_IM_MXIC_OFF;
	switch (start_param) {
		case 0:
			memacc_start->slave[0].start_trigger = D_IM_MXIC_ON;
			break;
		case 1:
			memacc_start->slave[1].start_trigger = D_IM_MXIC_ON;
			break;
		case 2:
			memacc_start->slave[2].start_trigger = D_IM_MXIC_ON;
			break;
		case 3:
			memacc_start->slave[3].start_trigger = D_IM_MXIC_ON;
			break;
		case 9:
			memacc_start->slave[0].start_trigger = D_IM_MXIC_ON;
			memacc_start->slave[1].start_trigger = D_IM_MXIC_ON;
			memacc_start->slave[2].start_trigger = D_IM_MXIC_ON;
			memacc_start->slave[3].start_trigger = D_IM_MXIC_ON;
			break;
		default:
			// Unknown parameter kind.
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
			break;
	}
	
	return result;
}

// Create param for Im_MXIC_Set_Memory_Access_Detect function test.
static INT32 ct_im_mxic_create_memory_access_param(UINT32 start_param, UINT32 param_pattern, T_IM_MXIC_MEMORY_ACCESS_SLAVE* const memacc_param)
{
	INT32 result;

	result = D_DDIM_OK;
	
	result = ct_im_mxic_create_memory_access_start_trigger(start_param, memacc_param);
	if (result == D_DDIM_OK) {
		switch (param_pattern) {
			case 0:
				memacc_param->slave[0].master47_1		= 0x00001000001E7BFE;
				memacc_param->slave[0].master127_64		= 0x10000000000F032F;
				memacc_param->slave[0].start_address	= 0x40000000;
				memacc_param->slave[0].end_address		= 0x80000000;
				memacc_param->slave[0].mode				= D_IM_MXIC_ON;
				memacc_param->slave[1].master47_1		= 0x00002000001E7BFE;
				memacc_param->slave[1].master127_64		= 0x20000000000F032F;
				memacc_param->slave[1].start_address	= 0x40000000;
				memacc_param->slave[1].end_address		= 0x80000000;
				memacc_param->slave[1].mode				= D_IM_MXIC_ON;
				memacc_param->slave[2].master47_1		= 0x00003000001E7BFE;
				memacc_param->slave[2].master127_64		= 0x30000000000F032F;
				memacc_param->slave[2].start_address	= 0x40000000;
				memacc_param->slave[2].end_address		= 0x80000000;
				memacc_param->slave[2].mode				= D_IM_MXIC_ON;
				memacc_param->slave[3].master47_1		= 0x00008000001E7BFE;
				memacc_param->slave[3].master127_64		= 0x80000000000F032F;
				memacc_param->slave[3].start_address	= 0x40000000;
				memacc_param->slave[3].end_address		= 0x80000000;
				memacc_param->slave[3].mode				= D_IM_MXIC_ON;
				break;
			case 1:
				memacc_param->slave[0].master47_1 = 0x00000000001E7BFE;
				memacc_param->slave[0].master127_64 = 0x00000000000F032F;
				memacc_param->slave[0].start_address = 0x40000000;
				memacc_param->slave[0].end_address = 0x80000000;
				memacc_param->slave[0].mode = D_IM_MXIC_OFF;
				memacc_param->slave[1].master47_1 = 0x00000000001E7BFE;
				memacc_param->slave[1].master127_64 = 0x00000000000F032F;
				memacc_param->slave[1].start_address = 0x40000000;
				memacc_param->slave[1].end_address = 0x80000000;
				memacc_param->slave[1].mode = D_IM_MXIC_OFF;
				memacc_param->slave[2].master47_1 = 0x00000000001E7BFE;
				memacc_param->slave[2].master127_64 = 0x00000000000F032F;
				memacc_param->slave[2].start_address = 0x40000000;
				memacc_param->slave[2].end_address = 0x80000000;
				memacc_param->slave[2].mode = D_IM_MXIC_OFF;
				memacc_param->slave[3].master47_1 = 0x00000000001E7BFE;
				memacc_param->slave[3].master127_64 = 0x00000000000F032F;
				memacc_param->slave[3].start_address = 0x40000000;
				memacc_param->slave[3].end_address = 0x80000000;
				memacc_param->slave[3].mode = D_IM_MXIC_OFF;
				break;
			default:
				// Unknown parameter kind.
				result = D_IM_MXIC_INPUT_PARAM_ERROR;
				break;
		}
	}
	return result;
}

// Callback function for slave area decode error.
static VOID ct_im_mxic_dec_err_callback(UCHAR ch_type)
{
	INT32 result;
	T_IM_MXIC_DEC_ERR dec_err;

	switch (ch_type) {
		case D_IM_MXIC_DEC_ERR_CH_W:
			Ddim_Print(("E:Decode error interrupt. ch_type = D_IM_MXIC_DEC_ERR_CH_W\n"));
			break;

		case D_IM_MXIC_DEC_ERR_CH_R:
			Ddim_Print(("E:Decode error interrupt. ch_type = D_IM_MXIC_DEC_ERR_CH_R\n"));
			break;

		case D_IM_MXIC_DEC_ERR_CH_RW:
			Ddim_Print(("E:Decode error interrupt. ch_type = D_IM_MXIC_DEC_ERR_CH_RW\n"));
			break;

		default:
			Ddim_Print(("E:Decode error interrupt. ch_type = UNKNOWN\n"));
			break;
	}

	// Get error information.
	result = Im_MXIC_Get_Decode_Error((E_IM_MXIC_UNIT)0, &dec_err);
	if (result == D_DDIM_OK) {
		ct_im_mxic_print_dec_err(&dec_err);
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Decode_Error NG. result=0x%X\n", result));
	}
	return;
}


// Error test of Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry function.
static INT32 ct_im_mxic_all_port_set(UINT32 set_ptn, T_IM_MXIC_OUTPUT_PORT* const all_port_assign)
{
	INT32 result;

	result = D_DDIM_OK;

	if(set_ptn == 0){
		memset((T_IM_MXIC_OUTPUT_PORT*)all_port_assign, 0, sizeof(T_IM_MXIC_OUTPUT_PORT));
	}
	else if(set_ptn == 1){
		memset((T_IM_MXIC_OUTPUT_PORT*)all_port_assign, 1, sizeof(T_IM_MXIC_OUTPUT_PORT));
	}
	else if(set_ptn == 2){
		// TPORTMW_127_64
		all_port_assign->w_out_port.ela			= E_IM_MXIC_PORT_GR_0;
		all_port_assign->w_out_port.xch			= E_IM_MXIC_PORT_GR_1;
		all_port_assign->w_out_port.fpt_0		= E_IM_MXIC_PORT_GR_0;
		all_port_assign->w_out_port.fpt_1		= E_IM_MXIC_PORT_GR_1;

		// TPORTMR_127_64
		all_port_assign->r_out_port.ela			= E_IM_MXIC_PORT_GR_0;
		all_port_assign->r_out_port.xch			= E_IM_MXIC_PORT_GR_1;
		all_port_assign->r_out_port.fpt_0		= E_IM_MXIC_PORT_GR_0;
		all_port_assign->r_out_port.fpt_1		= E_IM_MXIC_PORT_GR_1;
	}
	else if(set_ptn == 3){
		// TPORTMW_127_64
		all_port_assign->w_out_port.ela			= E_IM_MXIC_PORT_GR_0;
		all_port_assign->w_out_port.xch			= E_IM_MXIC_PORT_GR_1;
		all_port_assign->w_out_port.fpt_0		= E_IM_MXIC_PORT_GR_0;
		all_port_assign->w_out_port.fpt_1		= E_IM_MXIC_PORT_GR_1;

		// TPORTMR_127_64
		all_port_assign->r_out_port.ela			= E_IM_MXIC_PORT_GR_0;
		all_port_assign->r_out_port.xch			= E_IM_MXIC_PORT_GR_1;
		all_port_assign->r_out_port.fpt_0		= E_IM_MXIC_PORT_GR_0;
		all_port_assign->r_out_port.fpt_1		= E_IM_MXIC_PORT_GR_1;
	}
	else {
		result = D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	return result;
}

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
VOID CT_Im_MXIC_1_01( UCHAR unit )
{
	INT32								result;
	T_IM_MXIC_ALL_ARBITER_ASSIGN		all_arbiter_assign;
	T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL	all_level_ctrl;
	T_IM_MXIC_ALL_LEVELPORT 			all_levelport;

	Ddim_Print(("CT_Im_MXIC_1_01(unit=%d)\n", unit));

	result = Im_MXIC_Init( (E_IM_MXIC_UNIT)unit );
	if (result == D_DDIM_OK) {
		Ddim_Print(("Im_MXIC_Init OK(unit=%d).\n", unit));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Init NG(unit=%d). result=0x%X\n", result, unit));
	}

	result = Im_MXIC_Get_Master_All_Arbiter( (E_IM_MXIC_UNIT)unit, &all_arbiter_assign );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_all_arbiter_assign( &all_arbiter_assign );
		Ddim_Print(("Im_MXIC_Get_Master_All_Arbiter OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Master_All_Arbiter NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Get_Slot_Priority_All_Arbiter( (E_IM_MXIC_UNIT)unit, &all_level_ctrl );
	if (result == D_DDIM_OK) {
		ct_im_mxic_print_all_slot_priority_level( &all_level_ctrl );
		Ddim_Print(("Im_MXIC_Get_Slot_Priority_All_Arbiter OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Slot_Priority_All_Arbiter NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Get_LevelPort_All( (E_IM_MXIC_UNIT)unit, &all_levelport );
	if (result == D_DDIM_OK) {
		ct_im_mxic_print_all_levelport( &all_levelport );
		Ddim_Print(("Im_MXIC_Get_LevelPort_All OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_LevelPort_All NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_02( UCHAR unit )
{
	INT32						result;
	T_IM_MXIC_CONFIG_ARBITER	config;

	Ddim_Print(("CT_Im_MXIC_1_02(unit=%d)\n", unit));

	config.sl_w1_config_on_flg = D_IM_MXIC_ON;
	config.sl_w2_config_on_flg = D_IM_MXIC_ON;
	config.sl_w3_config_on_flg = D_IM_MXIC_ON;
	config.sl_w4_config_on_flg = D_IM_MXIC_ON;
	config.sl_r1_config_on_flg = D_IM_MXIC_ON;
	config.sl_r2_config_on_flg = D_IM_MXIC_ON;
	config.sl_r3_config_on_flg = D_IM_MXIC_ON;
	config.sl_r4_config_on_flg = D_IM_MXIC_ON;
	config.gr_w0_config_on_flg = D_IM_MXIC_ON;
	config.gr_w1_config_on_flg = D_IM_MXIC_ON;
	config.gr_w2_config_on_flg = D_IM_MXIC_ON;
	config.gr_w3_config_on_flg = D_IM_MXIC_ON;
	config.gr_w4_config_on_flg = D_IM_MXIC_ON;
	config.gr_w5_config_on_flg = D_IM_MXIC_ON;
	config.gr_w6_config_on_flg = D_IM_MXIC_ON;
	config.gr_w7_config_on_flg = D_IM_MXIC_ON;
	config.gr_r0_config_on_flg = D_IM_MXIC_ON;
	config.gr_r1_config_on_flg = D_IM_MXIC_ON;
	config.gr_r2_config_on_flg = D_IM_MXIC_ON;
	config.gr_r3_config_on_flg = D_IM_MXIC_ON;
	config.gr_r4_config_on_flg = D_IM_MXIC_ON;
	config.gr_r5_config_on_flg = D_IM_MXIC_ON;
	config.gr_r6_config_on_flg = D_IM_MXIC_ON;
	config.gr_r7_config_on_flg = D_IM_MXIC_ON;

	result = Im_MXIC_Start_Config( (E_IM_MXIC_UNIT)unit, &config );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Start_Config OK(unit=%d).\n", unit));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Start_Config NG(unit=%d). result=0x%X\n", unit, result));
	}

	config.sl_w1_config_on_flg = D_IM_MXIC_OFF;
	config.sl_w2_config_on_flg = D_IM_MXIC_OFF;
	config.sl_w3_config_on_flg = D_IM_MXIC_OFF;
	config.sl_w4_config_on_flg = D_IM_MXIC_OFF;
	config.sl_r1_config_on_flg = D_IM_MXIC_OFF;
	config.sl_r2_config_on_flg = D_IM_MXIC_OFF;
	config.sl_r3_config_on_flg = D_IM_MXIC_OFF;
	config.sl_r4_config_on_flg = D_IM_MXIC_OFF;
	config.gr_w0_config_on_flg = D_IM_MXIC_OFF;
	config.gr_w1_config_on_flg = D_IM_MXIC_OFF;
	config.gr_w2_config_on_flg = D_IM_MXIC_OFF;
	config.gr_w3_config_on_flg = D_IM_MXIC_OFF;
	config.gr_w4_config_on_flg = D_IM_MXIC_OFF;
	config.gr_w5_config_on_flg = D_IM_MXIC_OFF;
	config.gr_w6_config_on_flg = D_IM_MXIC_OFF;
	config.gr_w7_config_on_flg = D_IM_MXIC_OFF;
	config.gr_r0_config_on_flg = D_IM_MXIC_OFF;
	config.gr_r1_config_on_flg = D_IM_MXIC_OFF;
	config.gr_r2_config_on_flg = D_IM_MXIC_OFF;
	config.gr_r3_config_on_flg = D_IM_MXIC_OFF;
	config.gr_r4_config_on_flg = D_IM_MXIC_OFF;
	config.gr_r5_config_on_flg = D_IM_MXIC_OFF;
	config.gr_r6_config_on_flg = D_IM_MXIC_OFF;
	config.gr_r7_config_on_flg = D_IM_MXIC_OFF;

	result = Im_MXIC_Start_Config( (E_IM_MXIC_UNIT)unit, &config );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Start_Config OK(unit=%d).\n", unit));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Start_Config NG(unit=%d). result=0x%X\n", unit, result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_03( UCHAR unit )
{
	INT32			result;
	T_IM_MXIC_CLOCK	clock_ctrl;

	Ddim_Print(("CT_Im_MXIC_1_03(unit=%d)\n", unit));

	clock_ctrl.clk_en_slave_1_0 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_1_1 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_1_2 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_1_3 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_2_0 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_2_1 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_2_2 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_2_3 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_3_0 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_3_1 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_3_2 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_3_3 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_4_0 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_4_1 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_4_2 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_slave_4_3 = D_IM_MXIC_ON;
	clock_ctrl.clk_en_group_0   = D_IM_MXIC_ON;
	clock_ctrl.clk_en_group_1   = D_IM_MXIC_ON;
	clock_ctrl.clk_en_group_2   = D_IM_MXIC_ON;
	clock_ctrl.clk_en_group_3   = D_IM_MXIC_ON;
	clock_ctrl.clk_en_group_4   = D_IM_MXIC_ON;
	clock_ctrl.clk_en_group_5   = D_IM_MXIC_ON;
	clock_ctrl.clk_en_group_6   = D_IM_MXIC_ON;
	clock_ctrl.clk_en_group_7   = D_IM_MXIC_ON;
	clock_ctrl.clk_en_dec_err   = D_IM_MXIC_ON;
	clock_ctrl.clk_en_monitor   = D_IM_MXIC_ON;

	result = Im_MXIC_Set_Clock_Enable( (E_IM_MXIC_UNIT)unit, &clock_ctrl );
	if (result == D_DDIM_OK) {
		Ddim_Print(("Im_MXIC_Set_Clock_Enable OK(unit=%d).\n",unit));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Clock_Enable NG(unit=%d). result=0x%X\n", unit, result));
	}

	memset( &clock_ctrl, 0, sizeof(T_IM_MXIC_CLOCK) );

	result = Im_MXIC_Get_Clock_Enable( (E_IM_MXIC_UNIT)unit, &clock_ctrl );
	if (result == D_DDIM_OK) {
		Ddim_Print(("clock_ctrl.clk_en_slave_1_0=%d\n", clock_ctrl.clk_en_slave_1_0));
		Ddim_Print(("clock_ctrl.clk_en_slave_1_1=%d\n", clock_ctrl.clk_en_slave_1_1));
		Ddim_Print(("clock_ctrl.clk_en_slave_1_2=%d\n", clock_ctrl.clk_en_slave_1_2));
		Ddim_Print(("clock_ctrl.clk_en_slave_1_3=%d\n", clock_ctrl.clk_en_slave_1_3));
		Ddim_Print(("clock_ctrl.clk_en_slave_2_0=%d\n", clock_ctrl.clk_en_slave_2_0));
		Ddim_Print(("clock_ctrl.clk_en_slave_2_1=%d\n", clock_ctrl.clk_en_slave_2_1));
		Ddim_Print(("clock_ctrl.clk_en_slave_2_2=%d\n", clock_ctrl.clk_en_slave_2_2));
		Ddim_Print(("clock_ctrl.clk_en_slave_2_3=%d\n", clock_ctrl.clk_en_slave_2_3));
		Ddim_Print(("clock_ctrl.clk_en_slave_3_0=%d\n", clock_ctrl.clk_en_slave_3_0));
		Ddim_Print(("clock_ctrl.clk_en_slave_3_1=%d\n", clock_ctrl.clk_en_slave_3_1));
		Ddim_Print(("clock_ctrl.clk_en_slave_3_2=%d\n", clock_ctrl.clk_en_slave_3_2));
		Ddim_Print(("clock_ctrl.clk_en_slave_3_3=%d\n", clock_ctrl.clk_en_slave_3_3));
		Ddim_Print(("clock_ctrl.clk_en_slave_4_0=%d\n", clock_ctrl.clk_en_slave_4_0));
		Ddim_Print(("clock_ctrl.clk_en_slave_4_1=%d\n", clock_ctrl.clk_en_slave_4_1));
		Ddim_Print(("clock_ctrl.clk_en_slave_4_2=%d\n", clock_ctrl.clk_en_slave_4_2));
		Ddim_Print(("clock_ctrl.clk_en_slave_4_3=%d\n", clock_ctrl.clk_en_slave_4_3));
		Ddim_Print(("clock_ctrl.clk_en_group_0=%d\n", clock_ctrl.clk_en_group_0));
		Ddim_Print(("clock_ctrl.clk_en_group_1=%d\n", clock_ctrl.clk_en_group_1));
		Ddim_Print(("clock_ctrl.clk_en_group_2=%d\n", clock_ctrl.clk_en_group_2));
		Ddim_Print(("clock_ctrl.clk_en_group_3=%d\n", clock_ctrl.clk_en_group_3));
		Ddim_Print(("clock_ctrl.clk_en_group_4=%d\n", clock_ctrl.clk_en_group_4));
		Ddim_Print(("clock_ctrl.clk_en_group_5=%d\n", clock_ctrl.clk_en_group_5));
		Ddim_Print(("clock_ctrl.clk_en_group_6=%d\n", clock_ctrl.clk_en_group_6));
		Ddim_Print(("clock_ctrl.clk_en_group_7=%d\n", clock_ctrl.clk_en_group_7));
		Ddim_Print(("clock_ctrl.clk_en_dec_err=%d\n", clock_ctrl.clk_en_dec_err));
		Ddim_Print(("clock_ctrl.clk_en_monitor=%d\n", clock_ctrl.clk_en_monitor));
		Ddim_Print(("Im_MXIC_Get_Clock_Enable OK(unit=%d).\n", unit));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Clock_Enable NG(unit=%d). result=0x%X\n", unit, result));
	}

	clock_ctrl.clk_en_slave_1_0 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_1_1 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_1_2 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_1_3 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_2_0 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_2_1 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_2_2 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_2_3 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_3_0 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_3_1 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_3_2 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_3_3 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_4_0 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_4_1 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_4_2 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_slave_4_3 = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_group_0   = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_group_1   = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_group_2   = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_group_3   = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_group_4   = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_group_5   = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_group_6   = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_group_7   = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_dec_err   = D_IM_MXIC_OFF;
	clock_ctrl.clk_en_monitor   = D_IM_MXIC_OFF;

	result = Im_MXIC_Set_Clock_Enable( (E_IM_MXIC_UNIT)unit, &clock_ctrl );
	if (result == D_DDIM_OK) {
		Ddim_Print(("Im_MXIC_Set_Clock_Enable OK(unit=%d).\n",unit));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Clock_Enable NG(unit=%d). result=0x%X\n", unit, result));
	}

	memset( &clock_ctrl, 0, sizeof(T_IM_MXIC_CLOCK) );

	result = Im_MXIC_Get_Clock_Enable( (E_IM_MXIC_UNIT)unit, &clock_ctrl );
	if (result == D_DDIM_OK) {
		Ddim_Print(("clock_ctrl.clk_en_slave_1_0=%d\n", clock_ctrl.clk_en_slave_1_0));
		Ddim_Print(("clock_ctrl.clk_en_slave_1_1=%d\n", clock_ctrl.clk_en_slave_1_1));
		Ddim_Print(("clock_ctrl.clk_en_slave_1_2=%d\n", clock_ctrl.clk_en_slave_1_2));
		Ddim_Print(("clock_ctrl.clk_en_slave_1_3=%d\n", clock_ctrl.clk_en_slave_1_3));
		Ddim_Print(("clock_ctrl.clk_en_slave_2_0=%d\n", clock_ctrl.clk_en_slave_2_0));
		Ddim_Print(("clock_ctrl.clk_en_slave_2_1=%d\n", clock_ctrl.clk_en_slave_2_1));
		Ddim_Print(("clock_ctrl.clk_en_slave_2_2=%d\n", clock_ctrl.clk_en_slave_2_2));
		Ddim_Print(("clock_ctrl.clk_en_slave_2_3=%d\n", clock_ctrl.clk_en_slave_2_3));
		Ddim_Print(("clock_ctrl.clk_en_slave_3_0=%d\n", clock_ctrl.clk_en_slave_3_0));
		Ddim_Print(("clock_ctrl.clk_en_slave_3_1=%d\n", clock_ctrl.clk_en_slave_3_1));
		Ddim_Print(("clock_ctrl.clk_en_slave_3_2=%d\n", clock_ctrl.clk_en_slave_3_2));
		Ddim_Print(("clock_ctrl.clk_en_slave_3_3=%d\n", clock_ctrl.clk_en_slave_3_3));
		Ddim_Print(("clock_ctrl.clk_en_slave_4_0=%d\n", clock_ctrl.clk_en_slave_4_0));
		Ddim_Print(("clock_ctrl.clk_en_slave_4_1=%d\n", clock_ctrl.clk_en_slave_4_1));
		Ddim_Print(("clock_ctrl.clk_en_slave_4_2=%d\n", clock_ctrl.clk_en_slave_4_2));
		Ddim_Print(("clock_ctrl.clk_en_slave_4_3=%d\n", clock_ctrl.clk_en_slave_4_3));
		Ddim_Print(("clock_ctrl.clk_en_group_0=%d\n", clock_ctrl.clk_en_group_0));
		Ddim_Print(("clock_ctrl.clk_en_group_1=%d\n", clock_ctrl.clk_en_group_1));
		Ddim_Print(("clock_ctrl.clk_en_group_2=%d\n", clock_ctrl.clk_en_group_2));
		Ddim_Print(("clock_ctrl.clk_en_group_3=%d\n", clock_ctrl.clk_en_group_3));
		Ddim_Print(("clock_ctrl.clk_en_group_4=%d\n", clock_ctrl.clk_en_group_4));
		Ddim_Print(("clock_ctrl.clk_en_group_5=%d\n", clock_ctrl.clk_en_group_5));
		Ddim_Print(("clock_ctrl.clk_en_group_6=%d\n", clock_ctrl.clk_en_group_6));
		Ddim_Print(("clock_ctrl.clk_en_group_7=%d\n", clock_ctrl.clk_en_group_7));
		Ddim_Print(("clock_ctrl.clk_en_dec_err=%d\n", clock_ctrl.clk_en_dec_err));
		Ddim_Print(("clock_ctrl.clk_en_monitor=%d\n", clock_ctrl.clk_en_monitor));
		Ddim_Print(("Im_MXIC_Get_Clock_Enable OK(unit=%d).\n", unit));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Clock_Enable NG(unit=%d). result=0x%X\n", unit, result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_04( UCHAR unit )
{
	INT32								result;
	UINT32								workvalue;
	UINT32								i, j;
	UCHAR								capability = 0;
	T_IM_MXIC_ALL_ACCEPTANCE_CAPABILITY	all_capability;

	Ddim_Print(("CT_Im_MXIC_1_04(unit=%d)\n", unit));

	workvalue = 1;

	for (i=E_IM_MXIC_W_ARBITER_W1; i<E_IM_MXIC_W_ARBITER_MAX; i++) {
		for (j=E_IM_MXIC_PORT_0; j<E_IM_MXIC_PORT_MAX; j++) {
			all_capability.w_capability[i][j] = workvalue;
		}
	}
	for (i=E_IM_MXIC_R_ARBITER_R1; i<E_IM_MXIC_R_ARBITER_MAX; i++) {
		for (j=E_IM_MXIC_PORT_0; j<E_IM_MXIC_PORT_MAX; j++) {
			all_capability.r_capability[i][j] = workvalue;
		}
	}
	for (i=E_IM_MXIC_W_ARBITER_GR_W0; i<E_IM_MXIC_W_ARBITER_GR_MAX; i++) {
		for (j=E_IM_MXIC_PORT_GR_0; j<E_IM_MXIC_PORT_GR_MAX; j++) {
			all_capability.w_capability_gr[i][j] = workvalue;
		}
	}
	for (i=E_IM_MXIC_R_ARBITER_GR_R0; i<E_IM_MXIC_R_ARBITER_GR_MAX; i++) {
		for (j=E_IM_MXIC_PORT_GR_0; j<E_IM_MXIC_PORT_GR_MAX; j++) {
			all_capability.r_capability_gr[i][j] = workvalue;
		}
	}

	result = Im_MXIC_Set_Acceptance_Capability_All_Port( (E_IM_MXIC_UNIT)unit, &all_capability );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Acceptance_Capability_All_Port OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_All_Port NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Get_Acceptance_Capability_All_Port( (E_IM_MXIC_UNIT)unit, &all_capability );
	if (result == D_DDIM_OK) {
		ct_im_mxic_print_all_acceptance_capability(&all_capability);
		Ddim_Print(("Im_MXIC_Get_Acceptance_Capability_All_Port OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_All_Port NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Acceptance_Capability( (E_IM_MXIC_UNIT)unit,
												(E_IM_MXIC_WR_ARBITER)0,
												(E_IM_MXIC_SPEC_ARBITER)0,
												(E_IM_MXIC_PORT)0,
												(UCHAR)8);
	if (result == D_DDIM_OK) {
		Ddim_Print(("Im_MXIC_Set_Acceptance_Capability OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Get_Acceptance_Capability( (E_IM_MXIC_UNIT)unit,
												(E_IM_MXIC_WR_ARBITER)0,
												(E_IM_MXIC_SPEC_ARBITER)0,
												(E_IM_MXIC_PORT)0,
												&capability);
	if (result == D_DDIM_OK) {
		Ddim_Print(("capability = %d\n", capability));
		Ddim_Print(("Im_MXIC_Get_Acceptance_Capability OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Acceptance_Capability_Group( (E_IM_MXIC_WR_ARBITER)0,
													  (E_IM_MXIC_SPEC_ARBITER_GR)0,
													  (E_IM_MXIC_PORT_GR)0,
													  (UCHAR)1 );
	if (result == D_DDIM_OK) {
		Ddim_Print(("Im_MXIC_Set_Acceptance_Capability_Group OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Get_Acceptance_Capability_Group( (E_IM_MXIC_WR_ARBITER)0,
													  (E_IM_MXIC_SPEC_ARBITER_GR)0,
													  (E_IM_MXIC_PORT_GR)0,
													  &capability );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("capability = %d\n", capability));
		Ddim_Print(("Im_MXIC_Get_Acceptance_Capability_Group OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_05( UCHAR unit )
{
	INT32						result;
	T_IM_MXIC_ALL_SLAVE_AREA	all_slave_area;
	T_IM_MXIC_SLAVE_AREA		slave_area;

	Ddim_Print(("CT_Im_MXIC_1_05(unit=%d)\n", unit));

	all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_1].size    = (UINT32)0x20000000;
	all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_1].address = (UINT32)0x40000000;
	all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_2].size    = (UINT32)0x20000000;
	all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_2].address = (UINT32)0x60000000;
	all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_3].size    = (UINT32)0x20000000;
	all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_3].address = (UINT32)0x80000000;
	all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_4].size    = (UINT32)0x20000000;
	all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_4].address = (UINT32)0xA0000000;

	result = Im_MXIC_Set_Slave_Area_All( (E_IM_MXIC_UNIT)unit, &all_slave_area );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Slave_Area_All OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Slave_Area_All NG. result=0x%X\n", result));
	}

	memset( &all_slave_area, 0, sizeof(T_IM_MXIC_ALL_SLAVE_AREA) );

	result = Im_MXIC_Get_Slave_Area_All( (E_IM_MXIC_UNIT)unit, &all_slave_area );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_all_slave_area( &all_slave_area );
		Ddim_Print(("Im_MXIC_Get_Slave_Area_All OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Slave_Area_All NG. result=0x%X\n", result));
	}

	slave_area.size    = (UINT32)0x40000000;
	slave_area.address = (UINT32)0x40000000;

	result = Im_MXIC_Set_Slave_Area( (E_IM_MXIC_UNIT)unit, (E_IM_MXIC_SLAVE_NUMBER)0, &slave_area );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Slave_Area OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Slave_Area NG. result=0x%X\n", result));
	}

	memset( &slave_area, 0, sizeof(T_IM_MXIC_SLAVE_AREA) );

	result = Im_MXIC_Get_Slave_Area( (E_IM_MXIC_UNIT)unit, (E_IM_MXIC_SLAVE_NUMBER)0, &slave_area );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_slave_area( &slave_area );
		Ddim_Print(("Im_MXIC_Get_Slave_Area OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Slave_Area NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_06( UCHAR unit )
{
	INT32							result;
	T_IM_MXIC_ALL_ARBITER_ASSIGN	all_arbiter_assign;
	T_IM_MXIC_W_ARBITER_ASSIGN_PORT	w_slot_status;
	T_IM_MXIC_R_ARBITER_ASSIGN_PORT	r_slot_status;

	Ddim_Print(("CT_Im_MXIC_1_06(unit=%d)\n", unit));

	result = ct_im_mxic_create_master_all_arbiter_param( (E_IM_MXIC_UNIT)unit, &all_arbiter_assign );
	if ( result == D_DDIM_OK ) {
		// Execute function.
		result = Im_MXIC_Set_Master_All_Arbiter( (E_IM_MXIC_UNIT)unit, &all_arbiter_assign );
		if ( result == D_DDIM_OK ) {
			Ddim_Print(("Im_MXIC_Set_Master_All_Arbiter OK.\n"));
		}
		else {
			Ddim_Print(("E:Im_MXIC_Set_Master_All_Arbiter NG. result=0x%X\n", result));
		}
	}

	memset( &all_arbiter_assign, 0, sizeof(T_IM_MXIC_ALL_ARBITER_ASSIGN) );

	result = Im_MXIC_Get_Master_All_Arbiter( (E_IM_MXIC_UNIT)unit, &all_arbiter_assign );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_all_arbiter_assign( &all_arbiter_assign );
		Ddim_Print(("Im_MXIC_Get_Master_All_Arbiter OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Master_All_Arbiter NG. result=0x%X\n", result));
	}

	result = ct_im_mxic_create_master_w_arbiter_param( (E_IM_MXIC_UNIT)unit, &w_slot_status );
	if ( result == D_DDIM_OK ) {
		// Execute function.
		result = Im_MXIC_Set_Master_W_Arbiter( (E_IM_MXIC_UNIT)unit, (E_IM_MXIC_W_ARBITER)0, &w_slot_status );
		if (result == D_DDIM_OK) {
			Ddim_Print(("Im_MXIC_Set_Master_W_Arbiter OK.\n"));
		}
		else {
			Ddim_Print(("E:Im_MXIC_Set_Master_W_Arbiter NG. result=0x%X\n", result));
		}
	}

	memset( &w_slot_status, 0, sizeof(T_IM_MXIC_W_ARBITER_ASSIGN_PORT) );

	result = Im_MXIC_Get_Master_W_Arbiter( (E_IM_MXIC_UNIT)unit, (E_IM_MXIC_W_ARBITER)0, &w_slot_status );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_w_arbiter_assign( &w_slot_status );
		Ddim_Print(("Im_MXIC_Get_Master_W_Arbiter OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Master_W_Arbiter NG. result=0x%X\n", result));
	}

	result = ct_im_mxic_create_master_r_arbiter_param( (E_IM_MXIC_UNIT)unit, &r_slot_status );
	if ( result == D_DDIM_OK ) {
		// Execute function.
		result = Im_MXIC_Set_Master_R_Arbiter( (E_IM_MXIC_UNIT)unit, (E_IM_MXIC_R_ARBITER)0, &r_slot_status );
		if ( result == D_DDIM_OK ) {
			Ddim_Print(("Im_MXIC_Set_Master_R_Arbiter OK.\n"));
		}
		else {
			Ddim_Print(("E:Im_MXIC_Set_Master_R_Arbiter NG. result=0x%X\n", result));
		}
	}

	memset( &r_slot_status, 0, sizeof(T_IM_MXIC_R_ARBITER_ASSIGN_PORT) );

	result = Im_MXIC_Get_Master_R_Arbiter( (E_IM_MXIC_UNIT)unit, (E_IM_MXIC_R_ARBITER)0, &r_slot_status );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_r_arbiter_assign( &r_slot_status );
		Ddim_Print(("Im_MXIC_Get_Master_R_Arbiter OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Master_R_Arbiter NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_07( UCHAR unit )
{
	INT32								result;
	T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL	all_level_ctrl;
	T_IM_MXIC_SLOT_PRIORITY_LEVEL		level_ctrl;

	Ddim_Print(("CT_Im_MXIC_1_07(unit=%d)\n", unit));

	result = ct_im_mxic_create_slot_priority_all_arbiter_param( &all_level_ctrl );
	if ( result == D_DDIM_OK ) {
		// Execute function.
		result = Im_MXIC_Set_Slot_Priority_All_Arbiter( (E_IM_MXIC_UNIT)unit, &all_level_ctrl);
		if (result == D_DDIM_OK) {
			Ddim_Print(("Im_MXIC_Set_Slot_Priority_All_Arbiter OK.\n"));
		}
		else {
			Ddim_Print(("E:Im_MXIC_Set_Slot_Priority_All_Arbiter NG. result=0x%X\n", result));
		}
	}

	result = Im_MXIC_Get_Slot_Priority_All_Arbiter( (E_IM_MXIC_UNIT)unit, &all_level_ctrl );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_all_slot_priority_level( &all_level_ctrl );
		Ddim_Print(("Im_MXIC_Get_Slot_Priority_All_Arbiter OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Slot_Priority_All_Arbiter NG. result=0x%X\n", result));
	}

	level_ctrl.priority_level_type  = (UCHAR)1;
	level_ctrl.upper_priority_level = (UCHAR)0;
	level_ctrl.lower_priority_level = (UCHAR)63;

	result = Im_MXIC_Set_Slot_Priority( (E_IM_MXIC_UNIT)unit,
										(E_IM_MXIC_WR_ARBITER)0,
										(E_IM_MXIC_SPEC_ARBITER)0,
										(E_IM_MXIC_PORT)0,
										(E_IM_MXIC_SLOT_LEVEL_CTRL)0,
										&level_ctrl);
	if (result == D_DDIM_OK) {
		Ddim_Print(("Im_MXIC_Set_Slot_Priority OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Slot_Priority NG. result=0x%X\n", result));
	}

	memset( &level_ctrl, 0, sizeof(T_IM_MXIC_SLOT_PRIORITY_LEVEL) );

	result = Im_MXIC_Get_Slot_Priority( (E_IM_MXIC_UNIT)unit,
										(E_IM_MXIC_WR_ARBITER)0,
										(E_IM_MXIC_SPEC_ARBITER)0,
										(E_IM_MXIC_PORT)0,
										(E_IM_MXIC_SLOT_LEVEL_CTRL)0,
										&level_ctrl);
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("priority_level_type  = %d\n", level_ctrl.priority_level_type));
		Ddim_Print(("upper_priority_level = %d\n", level_ctrl.upper_priority_level));
		Ddim_Print(("lower_priority_level = %d\n", level_ctrl.lower_priority_level));
		Ddim_Print(("Im_MXIC_Get_Slot_Priority OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Slot_Priority NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_08( UCHAR unit )
{
	INT32	result;
	UCHAR	master_priority;

	Ddim_Print(("CT_Im_MXIC_1_08(unit=%d)\n", unit));

	result = Im_MXIC_Set_Master_Priority( (E_IM_MXIC_UNIT)unit,
										  (UCHAR)0x81,
										  (UCHAR)3 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Priority OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Priority NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Get_Master_Priority( (E_IM_MXIC_UNIT)unit, (UCHAR)0x81, &master_priority );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("master_priority = %d\n", master_priority));
		Ddim_Print(("Im_MXIC_Get_Master_Priority OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Master_Priority NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_09( UCHAR unit )
{
	INT32	result;
	UCHAR	mask_enable;

	Ddim_Print(("CT_Im_MXIC_1_09(unit=%d)\n", unit));

	result = Im_MXIC_Set_Slot_Mask( (E_IM_MXIC_UNIT)unit,
									(E_IM_MXIC_WR_ARBITER)1,
									(E_IM_MXIC_SPEC_ARBITER)0,
									(E_IM_MXIC_PORT)0,
									(E_IM_MXIC_SLOT)0,
									(UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Slot_Mask OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Slot_Mask NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Get_Slot_Mask( (E_IM_MXIC_UNIT)unit,
									(E_IM_MXIC_WR_ARBITER)1,
									(E_IM_MXIC_SPEC_ARBITER)0,
									(E_IM_MXIC_PORT)0,
									(E_IM_MXIC_SLOT)0,
									&mask_enable);
	if (result == D_DDIM_OK) {
		Ddim_Print(("mask_enable = %d\n", mask_enable));
		Ddim_Print(("Im_MXIC_Get_Slot_Mask OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Slot_Mask NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_10( UCHAR unit )
{
	INT32					result;
	T_IM_MXIC_MASTER_MASK	master_mask;

	Ddim_Print(("CT_Im_MXIC_1_10(unit=%d)\n", unit));

	master_mask.mask_type   = (UCHAR)2;
	master_mask.mask_period = (USHORT)4;

	result = Im_MXIC_Set_Master_Mask( (E_IM_MXIC_UNIT)unit, (UCHAR)0x81, &master_mask );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask NG. result=0x%X\n", result));
	}

	memset( &master_mask, 0, sizeof(T_IM_MXIC_MASTER_MASK) );

	result = Im_MXIC_Get_Master_Mask( (E_IM_MXIC_UNIT)unit, (UCHAR)0x81, &master_mask );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("master_mask.mask_type   = %d\n", master_mask.mask_type));
		Ddim_Print(("master_mask.mask_period = %d\n", master_mask.mask_period));
		Ddim_Print(("Im_MXIC_Get_Master_Mask OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Master_Mask NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));
}


VOID CT_Im_MXIC_1_11( UCHAR unit )
{
	INT32	result;
	UCHAR	mask_enable;
	UCHAR	mask_select;

	Ddim_Print(("CT_Im_MXIC_1_11(unit=%d)\n", unit));

	result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x81,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor NG. result=0x%X\n", result));
	}

	mask_enable = 0;

	result = Im_MXIC_Get_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x81,
											 (E_IM_MXIC_MASK_GROUP)0,
											 &mask_enable );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("mask_enable = %d\n", mask_enable));
		Ddim_Print(("Im_MXIC_Get_Master_Mask_Factor OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Master_Mask_Factor NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x82,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Target OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target NG. result=0x%X\n", result));
	}

	mask_enable = 0;

	result = Im_MXIC_Get_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x82,
											 (E_IM_MXIC_MASK_GROUP)0,
											 &mask_enable );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("mask_enable = %d\n", mask_enable));
		Ddim_Print(("Im_MXIC_Get_Master_Mask_Target OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Master_Mask_Target NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Select( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x81,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Select OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Select NG. result=0x%X\n", result));
	}

	mask_select = 0;

	result = Im_MXIC_Get_Master_Mask_Select( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x81,
											 &mask_select );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("mask_select = %d\n", mask_select));
		Ddim_Print(("Im_MXIC_Get_Master_Mask_Select OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Master_Mask_Select NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_12( UCHAR unit )
{
	INT32	result;
	UCHAR	transaction;

	Ddim_Print(("CT_Im_MXIC_1_12(unit=%d)\n", unit));

	result = Im_MXIC_Set_Master_Transaction( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x81,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Transaction OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Transaction NG. result=0x%X\n", result));
	}

	transaction = 0;

	result = Im_MXIC_Get_Master_Transaction( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x81,
											 &transaction );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("transaction = %d\n", transaction));
		Ddim_Print(("Im_MXIC_Get_Master_Transaction OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Master_Transaction NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));

}

VOID CT_Im_MXIC_1_13( UCHAR unit )
{
	INT32					result;
	T_IM_MXIC_ALL_LEVELPORT	all_levelport;
	E_IM_MXIC_PORT			port;
	E_IM_MXIC_LEVEL			level;

	Ddim_Print(("CT_Im_MXIC_1_13(unit=%d)\n", unit));

	result = ct_im_mxic_create_all_levelport_param( &all_levelport );
	if (result == D_DDIM_OK) {
		// Execute function.
		result = Im_MXIC_Set_LevelPort_All( (E_IM_MXIC_UNIT)unit, &all_levelport );
		if ( result == D_DDIM_OK ) {
			Ddim_Print(("Im_MXIC_Set_LevelPort_All OK.\n"));
		}
		else {
			Ddim_Print(("E:Im_MXIC_Set_LevelPort_All NG. result=0x%X\n", result));
		}
	}

	memset( &all_levelport, 0, sizeof(T_IM_MXIC_ALL_LEVELPORT) );

	result = Im_MXIC_Get_LevelPort_All( (E_IM_MXIC_UNIT)unit, &all_levelport );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_all_levelport( &all_levelport );
		Ddim_Print(("Im_MXIC_Get_LevelPort_All OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_LevelPort_All NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_LevelPort_Port( (E_IM_MXIC_UNIT)unit,
										 (E_IM_MXIC_WR_ARBITER)0,
										 (E_IM_MXIC_SPEC_ARBITER)0,
										 (UCHAR)0x00,
										 (E_IM_MXIC_PORT)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_LevelPort_Port OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_LevelPort_Port NG. result=0x%X\n", result));
	}

	port = (E_IM_MXIC_PORT)0;

	result = Im_MXIC_Get_LevelPort_Port( (E_IM_MXIC_UNIT)unit,
										 (E_IM_MXIC_WR_ARBITER)0,
										 (E_IM_MXIC_SPEC_ARBITER)0,
										 (UCHAR)0x00,
										 &port );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Target port = %d\n", port));
		Ddim_Print(("Im_MXIC_Get_LevelPort_Port OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_LevelPort_Port NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_LevelPort_Level( (E_IM_MXIC_UNIT)unit,
										  (E_IM_MXIC_WR_ARBITER)0,
										  (E_IM_MXIC_SPEC_ARBITER)0,
										  (UCHAR)0x00,
										  (E_IM_MXIC_LEVEL)2 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_LevelPort_Level OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_LevelPort_Level NG. result=0x%X\n", result));
	}

	level = (E_IM_MXIC_LEVEL)0;

	result = Im_MXIC_Get_LevelPort_Level( (E_IM_MXIC_UNIT)unit,
										  (E_IM_MXIC_WR_ARBITER)0,
										  (E_IM_MXIC_SPEC_ARBITER)0,
										  (UCHAR)0x00,
										  &level );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Target level = %d\n", level));
		Ddim_Print(("Im_MXIC_Get_LevelPort_Level OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_LevelPort_Level NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_14( UCHAR unit )
{
	INT32					result;
	T_IM_MXIC_OUTPUT_PORT	all_port_assign;
	E_IM_MXIC_PORT_GR		port_assign;

	Ddim_Print(("CT_Im_MXIC_1_14(unit=%d)\n", unit));

	result = ct_im_mxic_all_port_set( 1, &all_port_assign );
	if ( result == D_DDIM_OK ) {
		result = Im_MXIC_Set_Output_Port_All( &all_port_assign );
		if (result == D_DDIM_OK) {
			Ddim_Print(("Im_MXIC_Set_Output_Port_All OK.\n"));
		}
		else {
			Ddim_Print(("E:Im_MXIC_Set_Output_Port_All NG. result=0x%X\n", result));
		}
	}
	else {
		Ddim_Print(("E:Command parameter error.\n"));
	}

	memset( &all_port_assign, 0, sizeof(T_IM_MXIC_OUTPUT_PORT) );

	result = Im_MXIC_Get_Output_Port_All( &all_port_assign );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_all_port_assign( &all_port_assign );
		Ddim_Print(("Im_MXIC_Get_Output_Port_All OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Output_Port_All NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Output_Port( (UCHAR)0x40,
									  E_IM_MXIC_PORT_GR_1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Output_Port OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Output_Port NG. result=0x%X\n", result));
	}

	port_assign = E_IM_MXIC_PORT_GR_0;

	result = Im_MXIC_Get_Output_Port((UCHAR)0x40, &port_assign );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("port_assign = %d\n", port_assign));
		Ddim_Print(("Im_MXIC_Get_Output_Port OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Output_Port NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_15( UCHAR unit )
{
	INT32									result;
	T_IM_MXIC_MONITOR_PARAMETER				mon_param;
	UCHAR									thstop_enable;
	T_IM_MXIC_ALL_HISTORY_MONITOR			all_history;
	T_IM_MXIC_HISTORY_MONITOR				history;
	T_IM_MXIC_ALL_SLOT_STATUS_MONITOR		all_slot_status;
	T_IM_MXIC_W_ARBITER_ASSIGN_PORT			w_slot_status;
	T_IM_MXIC_R_ARBITER_ASSIGN_PORT			r_slot_status;
	T_IM_MXIC_W_ARBITER_ASSIGN_GR			w_gr_slot_status;
	T_IM_MXIC_R_ARBITER_ASSIGN_GR			r_gr_slot_status;
	T_IM_MXIC_MASTER_STATUS_MONITOR			master_status;
	T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR	all_acc_trans;
	UINT32									mon_result;
	UCHAR									mon_state;

	Ddim_Print(("CT_Im_MXIC_1_15(unit=%d)\n", unit));

	result = ct_im_mxic_create_access_or_trans_monitor_param( 0, &mon_param );
	if ( result == D_DDIM_OK ) {
		// Execute function.
		result = Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter( &mon_param );
		if (result == D_DDIM_OK) {
			Ddim_Print(("Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter OK.\n"));
		}
		else {
			Ddim_Print(("E:Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter NG. result=0x%X\n", result));
		}
	}
	else {
		Ddim_Print(("E:Command parameter error.\n"));
	}

	memset( &mon_param, 0, sizeof(T_IM_MXIC_MONITOR_PARAMETER) );

	result = Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter( &mon_param );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_monitor_parameter( &mon_param );
		Ddim_Print(("Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter NG. result=0x%X\n", result));
	}

	Im_MXIC_Start_Monitor();
	Ddim_Print(("Im_MXIC_Start_Monitor OK.\n"));

	Im_MXIC_Stop_Monitor();
	Ddim_Print(("Im_MXIC_Stop_Monitor OK.\n"));

	result = Im_MXIC_Set_History_Monitor_Stop_Enable( (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_History_Monitor_Stop_Enable OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_History_Monitor_Stop_Enable NG. result=0x%X\n", result));
	}

	thstop_enable = 0;

	result = Im_MXIC_Get_History_Monitor_Stop_Enable( &thstop_enable );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("thstop_enable = %d\n", thstop_enable));
		Ddim_Print(("Im_MXIC_Get_History_Monitor_Stop_Enable OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_History_Monitor_Stop_Enable NG. result=0x%X\n", result));
	}

	memset( &all_history, 0, sizeof(T_IM_MXIC_ALL_HISTORY_MONITOR) );

	result = Im_MXIC_Get_History_Monitor_All_Port( &all_history );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_all_history_monitor( &all_history );
		Ddim_Print(("Im_MXIC_Get_History_Monitor_All_Port OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_History_Monitor_All_Port NG. result=0x%X\n", result));
	}

	memset( &history, 0, sizeof(T_IM_MXIC_HISTORY_MONITOR) );

	result = Im_MXIC_Get_History_Monitor( (E_IM_MXIC_WR_ARBITER)0,
										  (E_IM_MXIC_SPEC_ARBITER)0,
										  (E_IM_MXIC_PORT)0,
										  &history );
	if (result == D_DDIM_OK) {
		ct_im_mxic_print_history_monitor( &history );
		Ddim_Print(("Im_MXIC_Get_History_Monitor OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_History_Monitor NG. result=0x%X\n", result));
	}

	memset( &all_slot_status, 0, sizeof(T_IM_MXIC_ALL_SLOT_STATUS_MONITOR) );

	result = Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter( (E_IM_MXIC_UNIT)unit, &all_slot_status );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_all_slot_status_monitor( &all_slot_status );
		Ddim_Print(("Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter NG. result=0x%X\n", result));
	}

	memset( &w_slot_status, 0, sizeof(T_IM_MXIC_W_ARBITER_ASSIGN_PORT) );

	result = Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter( (E_IM_MXIC_UNIT)unit, (E_IM_MXIC_W_ARBITER)0, &w_slot_status );
	if (result == D_DDIM_OK) {
		ct_im_mxic_print_w_arbiter_assign(&w_slot_status);
		Ddim_Print(("Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter NG. result=0x%X\n", result));
	}

	memset( &r_slot_status, 0, sizeof(T_IM_MXIC_R_ARBITER_ASSIGN_PORT) );

	result = Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter( (E_IM_MXIC_UNIT)unit, (E_IM_MXIC_R_ARBITER)0, &r_slot_status );
	if (result == D_DDIM_OK) {
		ct_im_mxic_print_r_arbiter_assign(&r_slot_status);
		Ddim_Print(("Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter NG. result=0x%X\n", result));
	}

	memset( &w_gr_slot_status, 0, sizeof(T_IM_MXIC_W_ARBITER_ASSIGN_GR) );

	result = Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group( (E_IM_MXIC_W_ARBITER_GR)0, &w_gr_slot_status );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_w_arbiter_assign_group( &w_gr_slot_status );
		Ddim_Print(("Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group NG. result=0x%X\n", result));
	}

	memset( &r_gr_slot_status, 0, sizeof(T_IM_MXIC_R_ARBITER_ASSIGN_GR) );

	result = Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group( (E_IM_MXIC_R_ARBITER_GR)0, &r_gr_slot_status );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_r_arbiter_assign_group( &r_gr_slot_status );
		Ddim_Print(("Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group NG. result=0x%X\n", result));
	}

	memset( &master_status, 0, sizeof(T_IM_MXIC_MASTER_STATUS_MONITOR) );

	result = Im_MXIC_Get_Master_Status_Monitor( (E_IM_MXIC_UNIT)unit, &master_status );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_master_status_monitor( &master_status );
		Ddim_Print(("Im_MXIC_Get_Master_Status_Monitor OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Master_Status_Monitor NG. result=0x%X\n", result));
	}

	memset( &all_acc_trans, 0, sizeof(T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR) );

	result = Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry( &all_acc_trans );
	if (result == D_DDIM_OK) {
		ct_im_mxic_print_all_access_or_trans_monitor( &all_acc_trans );
		Ddim_Print(("Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry NG. result=0x%X\n", result));
	}

	mon_result = 0;

	result = Im_MXIC_Get_Access_Or_Trans_Monitor( 0, &mon_result );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("mon_result = %d\n", mon_result));
		Ddim_Print(("Im_MXIC_Get_Access_Or_Trans_Monitor OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Access_Or_Trans_Monitor NG. result=0x%X\n", result));
	}

	mon_state = 0;

	result = Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State( &mon_state );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("mon_state = %d\n", mon_state));
		Ddim_Print(("Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_16( UCHAR unit )
{
	INT32							result;
	T_IM_MXIC_MEMORY_ACCESS_SLAVE	memory_access;

	Ddim_Print(("CT_Im_MXIC_1_16(unit=%d)\n", unit));

	result = ct_im_mxic_create_memory_access_param( 9, 0, &memory_access );
	if ( result == D_DDIM_OK ) {
		Im_MXIC_Set_Memory_Access_Detect( (E_IM_MXIC_UNIT)unit, &memory_access );
		if ( result == D_DDIM_OK ) {
			Ddim_Print(("Im_MXIC_Set_Memory_Access_Detect OK.\n"));
		}
		else {
			Ddim_Print(("E:Im_MXIC_Set_Memory_Access_Detect NG. result=0x%X\n", result));
		}
	}
	else {
		Ddim_Print(("E:Command parameter error.\n"));
	}

	memset( &memory_access, 0, sizeof(T_IM_MXIC_MEMORY_ACCESS_SLAVE) );

	result = ct_im_mxic_create_memory_access_start_trigger( 9, &memory_access );
	if ( result == D_DDIM_OK ) {
		result = Im_MXIC_Start_Memory_Access_Detect( (E_IM_MXIC_UNIT)unit, &memory_access );
		if ( result == D_DDIM_OK ) {
			Ddim_Print(("Im_MXIC_Start_Memory_Access_Detect OK.\n"));
		}
		else {
			Ddim_Print(("E:Im_MXIC_Start_Memory_Access_Detect NG. result=0x%X\n", result));
		}
	}
	else {
		Ddim_Print(("E:Command parameter error.\n"));
	}

	memset( &memory_access, 0, sizeof(T_IM_MXIC_MEMORY_ACCESS_SLAVE) );

	result = Im_MXIC_Get_Memory_Access_Detect( (E_IM_MXIC_UNIT)unit, &memory_access );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_memory_access_param( &memory_access );
		Ddim_Print(("Im_MXIC_Get_Memory_Access_Detect OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Memory_Access_Detect NG. result=0x%X\n", result));
	}

	Im_MXIC_Stop_Memory_Access_Detect( (E_IM_MXIC_UNIT)unit );
	Ddim_Print(("Im_MXIC_Stop_Memory_Access_Detect OK.\n"));

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_1_17( UCHAR unit )
{
	INT32					result;
	T_IM_MXIC_DEC_ERR_INT	dec_err_int;
	T_IM_MXIC_DEC_ERR		dec_err;

	Ddim_Print(("CT_Im_MXIC_1_17(unit=%d)\n", unit));

	memset( &dec_err_int, 0, sizeof(T_IM_MXIC_DEC_ERR_INT) );

	dec_err_int.pCallBack = ct_im_mxic_dec_err_callback;

	result = Im_MXIC_Set_Decode_Error_Int( (E_IM_MXIC_UNIT)unit, &dec_err_int );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Decode_Error_Int OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Decode_Error_Int NG. result=0x%X\n", result));
	}

	memset( &dec_err_int, 0, sizeof(T_IM_MXIC_DEC_ERR_INT) );

	result = Im_MXIC_Get_Decode_Error_Int( (E_IM_MXIC_UNIT)unit, &dec_err_int );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_dec_err_int( &dec_err_int );
		Ddim_Print(("Im_MXIC_Get_Decode_Error_Int OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Decode_Error_Int NG. result=0x%X\n", result));
	}

	memset( &dec_err, 0, sizeof(T_IM_MXIC_DEC_ERR) );

	result = Im_MXIC_Get_Decode_Error( (E_IM_MXIC_UNIT)unit, &dec_err );
	if ( result == D_DDIM_OK ) {
		ct_im_mxic_print_dec_err( &dec_err );
		Ddim_Print(("Im_MXIC_Get_Decode_Error OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Get_Decode_Error NG. result=0x%X\n", result));
	}

	Ddim_Print(("\n"));
}

VOID CT_Im_MXIC_2_19( UCHAR unit )
{
	INT32					result;

	Ddim_Print(("CT_Im_MXIC_2_19(unit=%d)\n", unit));

	result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x01,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor 1-1 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor 1-1 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x41,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor 1-2 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor 1-2 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x1F,
											 (E_IM_MXIC_MASK_GROUP)1,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor 1-3 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor 1-3 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x62,
											 (E_IM_MXIC_MASK_GROUP)1,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor 1-4 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor 1-4 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x01,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Target 2-1 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target 2-1 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x41,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Target 2-2 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target 2-2 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x1F,
											 (E_IM_MXIC_MASK_GROUP)1,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Target 2-3 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target 2-3 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x62,
											 (E_IM_MXIC_MASK_GROUP)1,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Target 2-4 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target 2-4 NG. result=0x%X\n", result));
	}

#if 1
	result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x01,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)0 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor 3 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor 3 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x41,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)0 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor 3 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor 3 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x1F,
											 (E_IM_MXIC_MASK_GROUP)1,
											 (UCHAR)0 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor 3 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor 3 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x62,
											 (E_IM_MXIC_MASK_GROUP)1,
											 (UCHAR)0 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor 3 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor 3 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x01,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Target 4 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target 4 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x41,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Target 4 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target 4 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x1F,
											 (E_IM_MXIC_MASK_GROUP)1,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Target 4 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target 4 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x62,
											 (E_IM_MXIC_MASK_GROUP)1,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Target 4 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target 4 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x01,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor 5 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor 5 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x41,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor 5 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor 5 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x1F,
											 (E_IM_MXIC_MASK_GROUP)1,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor 5 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor 5 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x62,
											 (E_IM_MXIC_MASK_GROUP)1,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor 5 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor 5 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x01,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)0 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Target 6 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target 6 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x41,
											 (E_IM_MXIC_MASK_GROUP)0,
											 (UCHAR)0 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Target 6 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target 6 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x1F,
											 (E_IM_MXIC_MASK_GROUP)1,
											 (UCHAR)0 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Target 6 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target 6 NG. result=0x%X\n", result));
	}

	result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)unit,
											 (UCHAR)0x62,
											 (E_IM_MXIC_MASK_GROUP)1,
											 (UCHAR)1 );
	if ( result == D_DDIM_OK ) {
		Ddim_Print(("Im_MXIC_Set_Master_Mask_Target 6 OK.\n"));
	}
	else {
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target 6 NG. result=0x%X\n", result));
	}
#endif

	Ddim_Print(("\n"));
}

VOID Ct_Im_MXIC_Main(INT32 argc, CHAR** argv)
{
	UINT32	i, j;
	UINT32	workvalue;
	INT32	result;
	UCHAR	capability;
	UCHAR*	addr;
	UCHAR	master_priority;
	UCHAR	mask_enable;
	UCHAR	mask_select;
	UCHAR	transaction;
	UCHAR	thstop_enable;
	UCHAR	mon_state;
	UINT32	mon_result;

	E_IM_MXIC_UNIT							unit;
	E_IM_MXIC_PORT							port;
	E_IM_MXIC_PORT_GR						port_assign;
	E_IM_MXIC_LEVEL							level;

	T_IM_MXIC_CONFIG_ARBITER				config;
	T_IM_MXIC_CLOCK							clock_ctrl;
	T_IM_MXIC_ALL_ACCEPTANCE_CAPABILITY		all_capability;
	T_IM_MXIC_SLAVE_AREA					slave_area;
	T_IM_MXIC_ALL_SLAVE_AREA				all_slave_area;
	T_IM_MXIC_DEC_ERR_INT					dec_err_int;
	T_IM_MXIC_DEC_ERR						dec_err;
	T_IM_MXIC_ALL_ARBITER_ASSIGN			all_arbiter_assign;
	T_IM_MXIC_SLOT_PRIORITY_LEVEL			level_ctrl;
	T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL		all_level_ctrl;
	T_IM_MXIC_MASTER_MASK					master_mask;
	T_IM_MXIC_OUTPUT_PORT					all_port_assign;
	T_IM_MXIC_HISTORY_MONITOR				history;
	T_IM_MXIC_ALL_HISTORY_MONITOR			all_history;
	T_IM_MXIC_ALL_SLOT_STATUS_MONITOR		all_slot_status;
	T_IM_MXIC_W_ARBITER_ASSIGN_PORT			w_slot_status;
	T_IM_MXIC_R_ARBITER_ASSIGN_PORT			r_slot_status;
	T_IM_MXIC_W_ARBITER_ASSIGN_GR			w_gr_slot_status;
	T_IM_MXIC_R_ARBITER_ASSIGN_GR			r_gr_slot_status;
	T_IM_MXIC_MASTER_STATUS_MONITOR			master_status;
	T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR	all_acc_trans;
	T_IM_MXIC_MONITOR_PARAMETER				mon_param;
	T_IM_MXIC_MEMORY_ACCESS_SLAVE			memory_access;
	T_IM_MXIC_ALL_LEVELPORT 				all_levelport;

	if (strcmp((const char *)argv[1], "reset") == 0) {
		// MXIC reset
		// [command] immxic reset P1
		//           P1 : Target unit number. (0-6)
		unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[2]);
		Im_MXIC_Reset( unit );
		Ddim_Print(("Im_MXIC_Reset(unit=%d) OK.\n", unit));
	}
	else if (strcmp((const char *)argv[1], "init") == 0) {
		// MXIC initialize
		// [command] immxic init P1
		//           P1 : Target unit number. (0-6)
		unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[2]);
		result = Im_MXIC_Init( unit );
		if (result == D_DDIM_OK) {
			Ddim_Print(("Im_MXIC_Init OK(unit=%d).\n", unit));
		}
		else {
			Ddim_Print(("E:Im_MXIC_Init NG(unit=%d). result=0x%X\n", result, unit));
		}
	}
	else if (strcmp((const char *)argv[1], "start") == 0) {
		// Configration start
		// [command] immxic start P1 P2 P3 P4 P5 P6 P7 P8 P9 P10 P11 P12 P13 P14 P15 P16 P17 P18 P19 P20 P21 P22 P23 P24 P25
		//           P1 : Target unit number. (0-6)
		//           P2 : Configration enable of Slave W1 arbiter. (0:Disable, 1:Enable)
		//           P3 : Configration enable of Slave W2 arbiter. (0:Disable, 1:Enable)
		//           P4 : Configration enable of Slave W3 arbiter. (0:Disable, 1:Enable)
		//           P5 : Configration enable of Slave W4 arbiter. (0:Disable, 1:Enable)
		//           P6 : Configration enable of Slave R1 arbiter. (0:Disable, 1:Enable)
		//           P7 : Configration enable of Slave R2 arbiter. (0:Disable, 1:Enable)
		//           P8 : Configration enable of Slave R3 arbiter. (0:Disable, 1:Enable)
		//           P9 : Configration enable of Slave R4 arbiter. (0:Disable, 1:Enable)
		//           P10: Configration enable of Group W1 arbiter. (0:Disable, 1:Enable)
		//           P11: Configration enable of Group W2 arbiter. (0:Disable, 1:Enable)
		//           P12: Configration enable of Group W3 arbiter. (0:Disable, 1:Enable)
		//           P13: Configration enable of Group W4 arbiter. (0:Disable, 1:Enable)
		//           P14: Configration enable of Group W5 arbiter. (0:Disable, 1:Enable)
		//           P15: Configration enable of Group W6 arbiter. (0:Disable, 1:Enable)
		//           P16: Configration enable of Group W7 arbiter. (0:Disable, 1:Enable)
		//           P17: Configration enable of Group W8 arbiter. (0:Disable, 1:Enable)
		//           P18: Configration enable of Group R1 arbiter. (0:Disable, 1:Enable)
		//           P19: Configration enable of Group R2 arbiter. (0:Disable, 1:Enable)
		//           P20: Configration enable of Group R3 arbiter. (0:Disable, 1:Enable)
		//           P21: Configration enable of Group R4 arbiter. (0:Disable, 1:Enable)
		//           P22: Configration enable of Group R5 arbiter. (0:Disable, 1:Enable)
		//           P23: Configration enable of Group R6 arbiter. (0:Disable, 1:Enable)
		//           P24: Configration enable of Group R7 arbiter. (0:Disable, 1:Enable)
		//           P25: Configration enable of Group R8 arbiter. (0:Disable, 1:Enable)

		if(argc >= 25) {
			unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[2]);
			config.sl_w1_config_on_flg = atoi((const char *)argv[3]);
			config.sl_w2_config_on_flg = atoi((const char *)argv[4]);
			config.sl_w3_config_on_flg = atoi((const char *)argv[5]);
			config.sl_w4_config_on_flg = atoi((const char *)argv[6]);
			config.sl_r1_config_on_flg = atoi((const char *)argv[7]);
			config.sl_r2_config_on_flg = atoi((const char *)argv[8]);
			config.sl_r3_config_on_flg = atoi((const char *)argv[9]);
			config.sl_r4_config_on_flg = atoi((const char *)argv[10]);
			config.gr_w0_config_on_flg = atoi((const char *)argv[11]);
			config.gr_w1_config_on_flg = atoi((const char *)argv[12]);
			config.gr_w2_config_on_flg = atoi((const char *)argv[13]);
			config.gr_w3_config_on_flg = atoi((const char *)argv[14]);
			config.gr_w4_config_on_flg = atoi((const char *)argv[15]);
			config.gr_w5_config_on_flg = atoi((const char *)argv[16]);
			config.gr_w6_config_on_flg = atoi((const char *)argv[17]);
			config.gr_w7_config_on_flg = atoi((const char *)argv[18]);
			config.gr_r0_config_on_flg = atoi((const char *)argv[19]);
			config.gr_r1_config_on_flg = atoi((const char *)argv[20]);
			config.gr_r2_config_on_flg = atoi((const char *)argv[21]);
			config.gr_r3_config_on_flg = atoi((const char *)argv[22]);
			config.gr_r4_config_on_flg = atoi((const char *)argv[23]);
			config.gr_r5_config_on_flg = atoi((const char *)argv[24]);
			config.gr_r6_config_on_flg = atoi((const char *)argv[25]);
			config.gr_r7_config_on_flg = atoi((const char *)argv[26]);

			result = Im_MXIC_Start_Config( unit, &config );
			if (result == D_DDIM_OK) {
				Ddim_Print(("Im_MXIC_Start_Config OK(unit=%d).\n", unit));
			}
			else {
				Ddim_Print(("E:Im_MXIC_Start_Config NG(unit=%d). result=0x%X\n", unit, result));
			}
		}
		else {
			Ddim_Print(("E:Parameter nums must be 25\n"));
		}
	}
	else if (strcmp((const char *)argv[1], "clock") == 0) {
		if (strcmp((const char *)argv[2], "set") == 0) {
			// Set clock
			// [command] immxic clock set P1 P2 P3 P4 P5 P6 P7 P8 P9 P10 P11 P12 P13 P14 P15 P16 P17 P18 P19 P20 P21 P22 P23 P24 P25 P26 P27
			//           P1 : Target unit number. (0-6)
			//           P2 : Clock enable of Slave-1_0 arbiter.        (0:Disable, 1:Enable)
			//           P3 : Clock enable of Slave-1_1 arbiter.        (0:Disable, 1:Enable)
			//           P4 : Clock enable of Slave-1_2 arbiter.        (0:Disable, 1:Enable)
			//           P5 : Clock enable of Slave-1_3 arbiter.        (0:Disable, 1:Enable)
			//           P6 : Clock enable of Slave-2_0 arbiter.        (0:Disable, 1:Enable)
			//           P7 : Clock enable of Slave-2_1 arbiter.        (0:Disable, 1:Enable)
			//           P8 : Clock enable of Slave-2_2 arbiter.        (0:Disable, 1:Enable)
			//           P9 : Clock enable of Slave-2_3 arbiter.        (0:Disable, 1:Enable)
			//           P10: Clock enable of Slave-3_0 arbiter.        (0:Disable, 1:Enable)
			//           P11: Clock enable of Slave-3_1 arbiter.        (0:Disable, 1:Enable)
			//           P12: Clock enable of Slave-3_2 arbiter.        (0:Disable, 1:Enable)
			//           P13: Clock enable of Slave-3_3 arbiter.        (0:Disable, 1:Enable)
			//           P14: Clock enable of Slave-4_0 arbiter.        (0:Disable, 1:Enable)
			//           P15: Clock enable of Slave-4_1 arbiter.        (0:Disable, 1:Enable)
			//           P16: Clock enable of Slave-4_2 arbiter.        (0:Disable, 1:Enable)
			//           P17: Clock enable of Slave-4_3 arbiter.        (0:Disable, 1:Enable)
			//           P18: Clock enable of Group-1 arbiter.          (0:Disable, 1:Enable)
			//           P19: Clock enable of Group-2 arbiter.          (0:Disable, 1:Enable)
			//           P20: Clock enable of Group-3 arbiter.          (0:Disable, 1:Enable)
			//           P21: Clock enable of Group-4 arbiter.          (0:Disable, 1:Enable)
			//           P22: Clock enable of Group-5 arbiter.          (0:Disable, 1:Enable)
			//           P23: Clock enable of Group-6 arbiter.          (0:Disable, 1:Enable)
			//           P24: Clock enable of Group-7 arbiter.          (0:Disable, 1:Enable)
			//           P25: Clock enable of Group-8 arbiter.          (0:Disable, 1:Enable)
			//           P26: Clock enable of decode error block.       (0:Disable, 1:Enable)
			//           P27: Clock enable of monitor function block.   (0:Disable, 1:Enable)
			if(argc >= 27) {
				unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[3]);
				clock_ctrl.clk_en_slave_1_0 = atoi((const char *)argv[4]);
				clock_ctrl.clk_en_slave_1_1 = atoi((const char *)argv[5]);
				clock_ctrl.clk_en_slave_1_2 = atoi((const char *)argv[6]);
				clock_ctrl.clk_en_slave_1_3 = atoi((const char *)argv[7]);
				clock_ctrl.clk_en_slave_2_0 = atoi((const char *)argv[8]);
				clock_ctrl.clk_en_slave_2_1 = atoi((const char *)argv[9]);
				clock_ctrl.clk_en_slave_2_2 = atoi((const char *)argv[10]);
				clock_ctrl.clk_en_slave_2_3 = atoi((const char *)argv[11]);
				clock_ctrl.clk_en_slave_3_0 = atoi((const char *)argv[12]);
				clock_ctrl.clk_en_slave_3_1 = atoi((const char *)argv[13]);
				clock_ctrl.clk_en_slave_3_2 = atoi((const char *)argv[14]);
				clock_ctrl.clk_en_slave_3_3 = atoi((const char *)argv[15]);
				clock_ctrl.clk_en_slave_4_0 = atoi((const char *)argv[16]);
				clock_ctrl.clk_en_slave_4_1 = atoi((const char *)argv[17]);
				clock_ctrl.clk_en_slave_4_2 = atoi((const char *)argv[18]);
				clock_ctrl.clk_en_slave_4_3 = atoi((const char *)argv[19]);
				clock_ctrl.clk_en_group_0   = atoi((const char *)argv[20]);
				clock_ctrl.clk_en_group_1   = atoi((const char *)argv[21]);
				clock_ctrl.clk_en_group_2   = atoi((const char *)argv[22]);
				clock_ctrl.clk_en_group_3   = atoi((const char *)argv[23]);
				clock_ctrl.clk_en_group_4   = atoi((const char *)argv[24]);
				clock_ctrl.clk_en_group_5   = atoi((const char *)argv[25]);
				clock_ctrl.clk_en_group_6   = atoi((const char *)argv[26]);
				clock_ctrl.clk_en_group_7   = atoi((const char *)argv[27]);
				clock_ctrl.clk_en_dec_err   = atoi((const char *)argv[28]);
				clock_ctrl.clk_en_monitor   = atoi((const char *)argv[29]);

				result = Im_MXIC_Set_Clock_Enable(unit, &clock_ctrl);
				if (result == D_DDIM_OK) {
					Ddim_Print(("Im_MXIC_Set_Clock_Enable OK(unit=%d).\n",unit));
				}
				else {
					Ddim_Print(("E:Im_MXIC_Set_Clock_Enable NG(unit=%d). result=0x%X\n", unit, result));
				}
			}
			else {
				Ddim_Print(("E:Parameter nums must be 27\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "get") == 0) {
			// Get clock
			// [command] immxic clock get P1
			//           P1 : Target unit number. (0-6)
			if(argc >= 4) {
				unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[3]);
				result = Im_MXIC_Get_Clock_Enable(unit, &clock_ctrl);
				if (result == D_DDIM_OK) {
					Ddim_Print(("clock_ctrl.clk_en_slave_1_0=%d\n", clock_ctrl.clk_en_slave_1_0));
					Ddim_Print(("clock_ctrl.clk_en_slave_1_1=%d\n", clock_ctrl.clk_en_slave_1_1));
					Ddim_Print(("clock_ctrl.clk_en_slave_1_2=%d\n", clock_ctrl.clk_en_slave_1_2));
					Ddim_Print(("clock_ctrl.clk_en_slave_1_3=%d\n", clock_ctrl.clk_en_slave_1_3));
					Ddim_Print(("clock_ctrl.clk_en_slave_2_0=%d\n", clock_ctrl.clk_en_slave_2_0));
					Ddim_Print(("clock_ctrl.clk_en_slave_2_1=%d\n", clock_ctrl.clk_en_slave_2_1));
					Ddim_Print(("clock_ctrl.clk_en_slave_2_2=%d\n", clock_ctrl.clk_en_slave_2_2));
					Ddim_Print(("clock_ctrl.clk_en_slave_2_3=%d\n", clock_ctrl.clk_en_slave_2_3));
					Ddim_Print(("clock_ctrl.clk_en_slave_3_0=%d\n", clock_ctrl.clk_en_slave_3_0));
					Ddim_Print(("clock_ctrl.clk_en_slave_3_1=%d\n", clock_ctrl.clk_en_slave_3_1));
					Ddim_Print(("clock_ctrl.clk_en_slave_3_2=%d\n", clock_ctrl.clk_en_slave_3_2));
					Ddim_Print(("clock_ctrl.clk_en_slave_3_3=%d\n", clock_ctrl.clk_en_slave_3_3));
					Ddim_Print(("clock_ctrl.clk_en_slave_4_0=%d\n", clock_ctrl.clk_en_slave_4_0));
					Ddim_Print(("clock_ctrl.clk_en_slave_4_1=%d\n", clock_ctrl.clk_en_slave_4_1));
					Ddim_Print(("clock_ctrl.clk_en_slave_4_2=%d\n", clock_ctrl.clk_en_slave_4_2));
					Ddim_Print(("clock_ctrl.clk_en_slave_4_3=%d\n", clock_ctrl.clk_en_slave_4_3));
					Ddim_Print(("clock_ctrl.clk_en_group_0=%d\n", clock_ctrl.clk_en_group_0));
					Ddim_Print(("clock_ctrl.clk_en_group_1=%d\n", clock_ctrl.clk_en_group_1));
					Ddim_Print(("clock_ctrl.clk_en_group_2=%d\n", clock_ctrl.clk_en_group_2));
					Ddim_Print(("clock_ctrl.clk_en_group_3=%d\n", clock_ctrl.clk_en_group_3));
					Ddim_Print(("clock_ctrl.clk_en_group_4=%d\n", clock_ctrl.clk_en_group_4));
					Ddim_Print(("clock_ctrl.clk_en_group_5=%d\n", clock_ctrl.clk_en_group_5));
					Ddim_Print(("clock_ctrl.clk_en_group_6=%d\n", clock_ctrl.clk_en_group_6));
					Ddim_Print(("clock_ctrl.clk_en_group_7=%d\n", clock_ctrl.clk_en_group_7));
					Ddim_Print(("clock_ctrl.clk_en_dec_err=%d\n", clock_ctrl.clk_en_dec_err));
					Ddim_Print(("clock_ctrl.clk_en_monitor=%d\n", clock_ctrl.clk_en_monitor));
					Ddim_Print(("Im_MXIC_Get_Clock_Enable OK(unit=%d).\n", unit));
				}
				else {
					Ddim_Print(("E:Im_MXIC_Get_Clock_Enable NG(unit=%d). result=0x%X\n", unit, result));
				}
			}
			else {
				Ddim_Print(("E:Parameter nums must be 1\n"));
			}
		}
		else {
			Ddim_Print(("E:Unknown command.\n"));
		}
	}
	else if (strcmp((const char *)argv[1], "acc_capa") == 0) {
		if (strcmp((const char *)argv[2], "slave") == 0) {
			if (strcmp((const char *)argv[3], "set") == 0) {
				// [command] immxic acc_capa slave set P1 P2 P3 P4 P5
				//           P1: Target unit number. (0-6)
				//           P2: Write Or Read (0:Write, 1:Raed)
				//           P3: Arbiter (0:Slave-1, 1:Slave-2, 2:Slave-3, 3:Slave-4)
				//           P4: Port (0~3)
				//           P5: Acceptance capability (1~8)
				if(argc >= 9) {
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
					result = Im_MXIC_Set_Acceptance_Capability(unit,
															  (E_IM_MXIC_WR_ARBITER)atoi((const char *)argv[5]),
															  (E_IM_MXIC_SPEC_ARBITER)atoi((const char *)argv[6]),
															  (E_IM_MXIC_PORT)atoi((const char *)argv[7]),
															  (UCHAR)atoi((const char *)argv[8]));
					if (result == D_DDIM_OK) {
						Ddim_Print(("Im_MXIC_Set_Acceptance_Capability OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 5\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// [command] immxic acc_capa slave get P1 P2 P3 P4
				//           P1: Target unit number. (0-6)
				//           P2: Write Or Read (0:Write, 1:Raed)
				//           P3: Arbiter (0:Slave-1, 1:Slave-2, 2:Slave-3, 3:Slave-4)
				//           P4: Port (0~3)
				if(argc >= 8) {
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
					result = Im_MXIC_Get_Acceptance_Capability(unit,
															  (E_IM_MXIC_WR_ARBITER)atoi((const char *)argv[5]),
															  (E_IM_MXIC_SPEC_ARBITER)atoi((const char *)argv[6]),
															  (E_IM_MXIC_PORT)atoi((const char *)argv[7]),
															  &capability);
					if (result == D_DDIM_OK) {
						Ddim_Print(("capability = %d\n", capability));
						Ddim_Print(("Im_MXIC_Get_Acceptance_Capability OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 4\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "group") == 0) {
			if (strcmp((const char *)argv[3], "set") == 0) {
				// [command] immxic acc_capa group set P1 P2 P3 P4
				//           P1: Write Or Read (0:Write, 1:Raed)
				//           P2: Arbiter (0:Group-0, 1:Group-1, 2:Group-2, 3:Group-3, 4:Group-4, 5:Group-5, 6:Group-6, 7:Group-7)
				//           P3: Port (0~1)
				//           P4: Acceptance capability (1~8)
				if(argc >= 8) {
					result = Im_MXIC_Set_Acceptance_Capability_Group((E_IM_MXIC_WR_ARBITER)atoi((const char *)argv[4]),
																  (E_IM_MXIC_SPEC_ARBITER_GR)atoi((const char *)argv[5]),
																  (E_IM_MXIC_PORT_GR)atoi((const char *)argv[6]),
																  (UCHAR)atoi((const char *)argv[7]));
					if (result == D_DDIM_OK) {
						Ddim_Print(("Im_MXIC_Set_Acceptance_Capability_Group OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 4\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// [command] immxic acc_capa group get P1 P2 P3
				//           P1: Write Or Read (0:Write, 1:Raed)
				//           P2: Arbiter (0:Group-0, 1:Group-1, 2:Group-2, 3:Group-3, 4:Group-4, 5:Group-5, 6:Group-6, 7:Group-7)
				//           P3: Port (0~1)
				if(argc >= 7) {
					result = Im_MXIC_Get_Acceptance_Capability_Group((E_IM_MXIC_WR_ARBITER)atoi((const char *)argv[4]),
																  (E_IM_MXIC_SPEC_ARBITER_GR)atoi((const char *)argv[5]),
																  (E_IM_MXIC_PORT_GR)atoi((const char *)argv[6]),
																  &capability);
					if (result == D_DDIM_OK) {
						Ddim_Print(("capability = %d\n", capability));
						Ddim_Print(("Im_MXIC_Get_Acceptance_Capability_Group OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 3\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "all") == 0) {
			if (strcmp((const char *)argv[3], "set") == 0) {
				// [command] immxic acc_capa all set P1 P2
				//           P1: Target unit number. (0-6)
				//           P2: acceptance capability pattern
				//               1: acceptance capability ALL 1
				//               2: acceptance capability ALL 8
				if(argc >= 6) {
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
					if (strcmp((const char *)argv[5], "1") == 0) {
						workvalue = 1;
					}
					else if (strcmp((const char *)argv[5], "2") == 0) {
						workvalue = 8;
					}
					else {
						Ddim_Print(("E:Parameter error P1=%d\n", atoi((const char *)argv[5])));
						return;
					}
					
					for (i=E_IM_MXIC_W_ARBITER_W1; i<E_IM_MXIC_W_ARBITER_MAX; i++) {
						for (j=E_IM_MXIC_PORT_0; j<E_IM_MXIC_PORT_MAX; j++) {
							all_capability.w_capability[i][j] = workvalue;
						}
					}
					for (i=E_IM_MXIC_R_ARBITER_R1; i<E_IM_MXIC_R_ARBITER_MAX; i++) {
						for (j=E_IM_MXIC_PORT_0; j<E_IM_MXIC_PORT_MAX; j++) {
							all_capability.r_capability[i][j] = workvalue;
						}
					}
					for (i=E_IM_MXIC_W_ARBITER_GR_W0; i<E_IM_MXIC_W_ARBITER_GR_MAX; i++) {
						for (j=E_IM_MXIC_PORT_GR_0; j<E_IM_MXIC_PORT_GR_MAX; j++) {
							all_capability.w_capability_gr[i][j] = workvalue;
						}
					}
					for (i=E_IM_MXIC_R_ARBITER_GR_R0; i<E_IM_MXIC_R_ARBITER_GR_MAX; i++) {
						for (j=E_IM_MXIC_PORT_GR_0; j<E_IM_MXIC_PORT_GR_MAX; j++) {
							all_capability.r_capability_gr[i][j] = workvalue;
						}
					}
					
					result = Im_MXIC_Set_Acceptance_Capability_All_Port(unit, &all_capability);
					if (result == D_DDIM_OK) {
						Ddim_Print(("Im_MXIC_Set_Acceptance_Capability_All_Port OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_All_Port NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 2\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// [command] immxic acc_capa all get P1
				//           P1: Target unit number. (0-6)
				if(argc >= 5) {
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
					result = Im_MXIC_Get_Acceptance_Capability_All_Port(unit, &all_capability);
					if (result == D_DDIM_OK) {
						ct_im_mxic_print_all_acceptance_capability(&all_capability);
						Ddim_Print(("Im_MXIC_Get_Acceptance_Capability_All_Port OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_All_Port NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 1\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else {
			Ddim_Print(("E:Unknown command.\n"));
		}
	}
	else if (strcmp((const char *)argv[1], "slave_area") == 0) {
		if (strcmp((const char *)argv[2], "one") == 0) {
			if (strcmp((const char *)argv[3], "set") == 0) {
				if(argc >= 8) {
					// [command] immxic slave_area one set P1 P2 P3 P4
					//           P1: Target unit number. (0-6)
					//           P2: Slave area number (0:Slave number-1, 1:Slave number-2, 2:Slave number-3, 3:Slave number-4)
					//           P3: Size of slave area (Hex value)
					//           P4: Start address of slave area (Hex value)
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
					slave_area.size    = (UINT32)ct_im_mxic_atoi_hex(argv[6]);
					slave_area.address = (UINT32)ct_im_mxic_atoi_hex(argv[7]);

					result = Im_MXIC_Set_Slave_Area(unit, (E_IM_MXIC_SLAVE_NUMBER)atoi((const char *)argv[5]), &slave_area);
					if (result == D_DDIM_OK) {
						Ddim_Print(("Im_MXIC_Set_Slave_Area OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_Slave_Area NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 8\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// [command] immxic slave_area one get P1 P2
				//           P1: Target unit number. (0-6)
				//           P2: Slave area number (0:Slave number-1, 1:Slave number-2, 2:Slave number-3, 3:Slave number-4)
				if(argc >= 6) {
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
					result = Im_MXIC_Get_Slave_Area(unit, (E_IM_MXIC_SLAVE_NUMBER)atoi((const char *)argv[5]), &slave_area);
					if (result == D_DDIM_OK) {
						ct_im_mxic_print_slave_area(&slave_area);
						Ddim_Print(("Im_MXIC_Get_Slave_Area OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_Slave_Area NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 6\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "all") == 0) {
			if (strcmp((const char *)argv[3], "set") == 0) {
				if(argc >= 13) {
					// [command] immxic slave_area all set P1 P2 P3 P4 P5 P6 P7 P8 P9
					//           P1: Target unit number. (0-6)
					//           P2: Slave number-1: Size of slave area (Hex value)
					//           P3: Slave number-1: Start address of slave area (Hex value)
					//           P4: Slave number-2: Size of slave area (Hex value)
					//           P5: Slave number-2: Start address of slave area (Hex value)
					//           P6: Slave number-3: Size of slave area (Hex value)
					//           P7: Slave number-3: Start address of slave area (Hex value)
					//           P8: Slave number-4: Size of slave area (Hex value)
					//           P9: Slave number-4: Start address of slave area (Hex value)
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
					all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_1].size    = (UINT32)ct_im_mxic_atoi_hex(argv[5]);
					all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_1].address = (UINT32)ct_im_mxic_atoi_hex(argv[6]);
					all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_2].size    = (UINT32)ct_im_mxic_atoi_hex(argv[7]);
					all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_2].address = (UINT32)ct_im_mxic_atoi_hex(argv[8]);
					all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_3].size    = (UINT32)ct_im_mxic_atoi_hex(argv[9]);
					all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_3].address = (UINT32)ct_im_mxic_atoi_hex(argv[10]);
					all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_4].size    = (UINT32)ct_im_mxic_atoi_hex(argv[11]);
					all_slave_area.slave_area[E_IM_MXIC_SLAVE_NUMBER_4].address = (UINT32)ct_im_mxic_atoi_hex(argv[12]);

					result = Im_MXIC_Set_Slave_Area_All(unit, &all_slave_area);
					if (result == D_DDIM_OK) {
						Ddim_Print(("Im_MXIC_Set_Slave_Area_All OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_Slave_Area_All NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 13\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// [command] immxic slave_area all get P1
				//           P1: Target unit number. (0-6)
				if(argc >= 5) {
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
					result = Im_MXIC_Get_Slave_Area_All(unit, &all_slave_area);
					if (result == D_DDIM_OK) {
						ct_im_mxic_print_all_slave_area(&all_slave_area);
						Ddim_Print(("Im_MXIC_Get_Slave_Area_All OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_Slave_Area_All NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 1\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else {
			Ddim_Print(("E:Unknown command.\n"));
		}
	}
	else if (strcmp((const char *)argv[1], "dec_err") == 0) {
		if (strcmp((const char *)argv[2], "set_int") == 0) {
			// [command] immxic dec_err set_int P1 P2 P3 P4
			//           P1: Target unit number. (0-6)
			//           P2: Interrupt enable of W channel decode error (0:Disable, 1:Enable)
			//           P3: Interrupt enable of R channel decode error (0:Disable, 1:Enable)
			//           P4: Callback function (0:NULL, 1:ct_im_mxic_dec_err_callback)
			if(argc >= 7) {
				unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[3]);
				dec_err_int.w_err_int_en = (UCHAR)atoi((const char *)argv[4]);
				dec_err_int.r_err_int_en = (UCHAR)atoi((const char *)argv[5]);

				if (atoi((const char *)argv[6]) == 0) {
					dec_err_int.pCallBack = NULL;
				}
				else {
					dec_err_int.pCallBack = ct_im_mxic_dec_err_callback;
				}

				result = Im_MXIC_Set_Decode_Error_Int(unit, &dec_err_int);
				if (result == D_DDIM_OK) {
					Ddim_Print(("Im_MXIC_Set_Decode_Error_Int OK.\n"));
				}
				else {
					Ddim_Print(("E:Im_MXIC_Set_Decode_Error_Int NG. result=0x%X\n", result));
				}
			}
			else {
				Ddim_Print(("E:Parameter nums must be 4\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "get_int") == 0) {
			// [command] immxic dec_err get_int P1
			//           P1: Target unit number. (0-6)
			if(argc >= 4) {
				unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[3]);
				result = Im_MXIC_Get_Decode_Error_Int(unit, &dec_err_int);
				if (result == D_DDIM_OK) {
					ct_im_mxic_print_dec_err_int(&dec_err_int);
					Ddim_Print(("Im_MXIC_Get_Decode_Error_Int OK.\n"));
				}
				else {
					Ddim_Print(("E:Im_MXIC_Get_Decode_Error_Int NG. result=0x%X\n", result));
				}
			}
			else {
				Ddim_Print(("E:Parameter nums must be 1\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "get") == 0) {
			// [command] immxic dec_err get P1
			//           P1: Target unit number. (0-6)
			if(argc >= 4) {
				unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[3]);
				result = Im_MXIC_Get_Decode_Error(unit, &dec_err);
				if (result == D_DDIM_OK) {
					ct_im_mxic_print_dec_err(&dec_err);
					Ddim_Print(("Im_MXIC_Get_Decode_Error OK.\n"));
				}
				else {
					Ddim_Print(("E:Im_MXIC_Get_Decode_Error NG. result=0x%X\n", result));
				}
			}
			else {
				Ddim_Print(("E:Parameter nums must be 1\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "access") == 0) {
			// [command] immxic dec_err access P1
			//           P1: Access target address
			addr = (UCHAR*)ct_im_mxic_atoi_hex(argv[3]);
			Ddim_Print(("Read address 0x%08X = %d\n", *addr, *addr));
//			Ddim_Print(("Write address 0x%08X\n"));
			*addr = 0;
		}
		else {
			Ddim_Print(("E:Unknown command.\n"));
		}
	}
	else if (strcmp((const char *)argv[1], "master") == 0) {
		if (strcmp((const char *)argv[2], "w") == 0) {
			if (strcmp((const char *)argv[3], "set") == 0) {
				// [command] immxic master w set P1 P2
				//           P1: Target unit number. (0-6)
				//           P2: W arbiter (0:W1, 1:W2, 2:W3, 3:W4)
				unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
				// Get parameter for Im_MXIC_Set_Master_W_Arbiter function.
				result = ct_im_mxic_create_master_w_arbiter_param( unit, &w_slot_status );
				if ( result == D_DDIM_OK ) {
					// Execute function.
					result = Im_MXIC_Set_Master_W_Arbiter( unit, (E_IM_MXIC_W_ARBITER)atoi((const char *)argv[5]), &w_slot_status );
					if (result == D_DDIM_OK) {
						Ddim_Print(("Im_MXIC_Set_Master_W_Arbiter OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_Master_W_Arbiter NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Command parameter error.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// [command] immxic master w get P1
				//           P1: Target unit number. (0-6)
				//           P2: W arbiter (0:W1, 1:W2, 2:W3, 3:W4)
				unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
				result = Im_MXIC_Get_Master_W_Arbiter( unit, (E_IM_MXIC_W_ARBITER)atoi((const char *)argv[5]), &w_slot_status );
				if ( result == D_DDIM_OK ) {
					ct_im_mxic_print_w_arbiter_assign( &w_slot_status );
					Ddim_Print(("Im_MXIC_Get_Master_W_Arbiter OK.\n"));
				}
				else {
					Ddim_Print(("E:Im_MXIC_Get_Master_W_Arbiter NG. result=0x%X\n", result));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "r") == 0) {
			if (strcmp((const char *)argv[3], "set") == 0) {
				// [command] immxic master r set P1 P2
				//           P1: Target unit number. (0-6)
				//           P2: R arbiter (0:R1, 1:R2, 2:R3, 3:R4)
				unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
				// Get parameter for Im_MXIC_Set_Master_R_Arbiter function.
				result = ct_im_mxic_create_master_r_arbiter_param( unit, &r_slot_status );
				if ( result == D_DDIM_OK ) {
					// Execute function.
					result = Im_MXIC_Set_Master_R_Arbiter( unit, (E_IM_MXIC_R_ARBITER)atoi((const char *)argv[5]), &r_slot_status );
					if ( result == D_DDIM_OK ) {
						Ddim_Print(("Im_MXIC_Set_Master_R_Arbiter OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_Master_R_Arbiter NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Command parameter error.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// [command] immxic master r get P1 P2
				//           P1: Target unit number. (0-6)
				//           P2: R arbiter (0:R1, 1:R2, 2:R3, 3:R4)
				unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
				result = Im_MXIC_Get_Master_R_Arbiter( unit, (E_IM_MXIC_R_ARBITER)atoi((const char *)argv[5]), &r_slot_status );
				if ( result == D_DDIM_OK ) {
					ct_im_mxic_print_r_arbiter_assign(&r_slot_status);
					Ddim_Print(("Im_MXIC_Get_Master_R_Arbiter OK.\n"));
				}
				else {
					Ddim_Print(("E:Im_MXIC_Get_Master_R_Arbiter NG. result=0x%X\n", result));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "all") == 0) {
			if (strcmp((const char *)argv[3], "set") == 0) {
				// [command] immxic master all set P1
				//           P1: Target unit number. (0-6)
				unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
				// Get parameter for Im_MXIC_Set_Master_All_Arbiter function.
				result = ct_im_mxic_create_master_all_arbiter_param( unit, &all_arbiter_assign );
				if ( result == D_DDIM_OK ) {
					// Execute function.
					result = Im_MXIC_Set_Master_All_Arbiter( unit, &all_arbiter_assign );
					if (result == D_DDIM_OK) {
						Ddim_Print(("Im_MXIC_Set_Master_All_Arbiter OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_Master_All_Arbiter NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Command parameter error.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// [command] immxic master all get P1
				//           P1: Target unit number. (0-6)
				unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
				result = Im_MXIC_Get_Master_All_Arbiter( unit, &all_arbiter_assign );
				if ( result == D_DDIM_OK ) {
					ct_im_mxic_print_all_arbiter_assign( &all_arbiter_assign );
					Ddim_Print(("Im_MXIC_Get_Master_All_Arbiter OK.\n"));
				}
				else {
					Ddim_Print(("E:Im_MXIC_Get_Master_All_Arbiter NG. result=0x%X\n", result));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else {
			Ddim_Print(("E:Unknown command.\n"));
		}
	}
	else if (strcmp((const char *)argv[1], "pri") == 0) {
		if (strcmp((const char *)argv[2], "slot") == 0) {
			if (strcmp((const char *)argv[3], "one") == 0) {
				if (strcmp((const char *)argv[4], "set") == 0) {
					// [command] immxic pri slot one set P1 P2 P3 P4 P5 P6 P7 P8
					//           P1: Target unit number. (0-6)
					//           P2: Write Or Read (0:Write, 1:Raed)
					//           P3: Arbiter (0:Slave-1, 1:Slave-2, 2:Slave-3, 3:Slave-4)
					//           P4: Port (0:port0, 1:port1, 2:port2, 3:port3)
					//           P5: Level control block (0:Level0, 1:Level1, 2:Level2)
					//           P6: Priority level type (0:Upper, 1:Lower, 2:Upper fixed, 3:Lower fixed)
					//           P7: Upper priority ratio (0~63. 0 means 64)
					//           P8: Lower priority ratio (0~63. 0 means 64)
					if( argc >= 13 ) {
						level_ctrl.priority_level_type  = (UCHAR)atoi((const char *)argv[10]);
						level_ctrl.upper_priority_level = (UCHAR)atoi((const char *)argv[11]);
						level_ctrl.lower_priority_level = (UCHAR)atoi((const char *)argv[12]);

						result = Im_MXIC_Set_Slot_Priority( (E_IM_MXIC_UNIT)atoi((const char *)argv[5]),
															(E_IM_MXIC_WR_ARBITER)atoi((const char *)argv[6]),
															(E_IM_MXIC_SPEC_ARBITER)atoi((const char *)argv[7]),
															(E_IM_MXIC_PORT)atoi((const char *)argv[8]),
															(E_IM_MXIC_SLOT_LEVEL_CTRL)atoi((const char *)argv[9]),
															&level_ctrl);
						if (result == D_DDIM_OK) {
							Ddim_Print(("Im_MXIC_Set_Slot_Priority OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Set_Slot_Priority NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 13\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "get") == 0) {
					// [command] immxic pri slot one get P1 P2 P3 P4 P5
					//           P1: Target unit number. (0-6)
					//           P2: Write Or Read (0:Write, 1:Raed)
					//           P3: Arbiter (0:Slave-1, 1:Slave-2, 2:Slave-3, 3:Slave-4)
					//           P4: Port (0:port0, 1:port1, 2:port2, 3:port3)
					//           P5: Level control block (0:Level0, 1:Level1, 2:Level2)
					if(argc >= 10) {
						result = Im_MXIC_Get_Slot_Priority( (E_IM_MXIC_UNIT)atoi((const char *)argv[5]),
															(E_IM_MXIC_WR_ARBITER)atoi((const char *)argv[6]),
															(E_IM_MXIC_SPEC_ARBITER)atoi((const char *)argv[7]),
															(E_IM_MXIC_PORT)atoi((const char *)argv[8]),
															(E_IM_MXIC_SLOT_LEVEL_CTRL)atoi((const char *)argv[9]),
															&level_ctrl);

						if (result == D_DDIM_OK) {
							Ddim_Print(("priority_level_type  = %d\n", level_ctrl.priority_level_type));
							Ddim_Print(("upper_priority_level = %d\n", level_ctrl.upper_priority_level));
							Ddim_Print(("lower_priority_level = %d\n", level_ctrl.lower_priority_level));
							Ddim_Print(("Im_MXIC_Get_Slot_Priority OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Get_Slot_Priority NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 5\n"));
					}
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "all") == 0) {
				if (strcmp((const char *)argv[4], "set") == 0) {
					// [command] immxic pri slot all set P1
					//           P1: Target unit number. (0-6)
					// Get parameter for Im_MXIC_Set_Slot_Priority_All_Arbiter function.
					result = ct_im_mxic_create_slot_priority_all_arbiter_param(&all_level_ctrl);
					if (result == D_DDIM_OK) {
						// Execute function.
						unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[5]);
						result = Im_MXIC_Set_Slot_Priority_All_Arbiter(unit, &all_level_ctrl);
						if (result == D_DDIM_OK) {
							Ddim_Print(("Im_MXIC_Set_Slot_Priority_All_Arbiter OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Set_Slot_Priority_All_Arbiter NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Command parameter error.\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "get") == 0) {
					// [command] immxic pri slot all get P1
					//           P1: Target unit number. (0-6)
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[5]);
					result = Im_MXIC_Get_Slot_Priority_All_Arbiter(unit, &all_level_ctrl);
					if (result == D_DDIM_OK) {
						ct_im_mxic_print_all_slot_priority_level(&all_level_ctrl);
						Ddim_Print(("Im_MXIC_Get_Slot_Priority_All_Arbiter OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_Slot_Priority_All_Arbiter NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "master") == 0) {
			if (strcmp((const char *)argv[3], "set") == 0) {
				// [command] immxic pri master set P1 P2 P3
				//           P1: Target unit number. (0-6)
				//           P2: Master number (Hex value)
				//           P3: Master priority (0~15)
				if(argc >= 7) {
					result = Im_MXIC_Set_Master_Priority( (E_IM_MXIC_UNIT)atoi((const char *)argv[4]),
														  (UCHAR)ct_im_mxic_atoi_hex(argv[5]),
														  (UCHAR)atoi((const char *)argv[6]));
					if ( result == D_DDIM_OK ) {
						Ddim_Print(("Im_MXIC_Set_Master_Priority OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_Master_Priority NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 3\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// [command] immxic pri master get P1 P2
				//           P1: Target unit number. (0-6)
				//           P2: Master number (Hex value)
				if(argc >= 6) {
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
					result = Im_MXIC_Get_Master_Priority( unit, (UCHAR)ct_im_mxic_atoi_hex(argv[5]), &master_priority );
					if (result == D_DDIM_OK) {
						Ddim_Print(("master_priority = %d\n", master_priority));
						Ddim_Print(("Im_MXIC_Get_Master_Priority OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_Master_Priority NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 2\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else {
			Ddim_Print(("E:Unknown command.\n"));
		}
	}
	else if (strcmp((const char *)argv[1], "mask") == 0) {
		if (strcmp((const char *)argv[2], "slot") == 0) {
			if (strcmp((const char *)argv[3], "set") == 0) {
				// [command] immxic mask slot set P1 P2 P3 P4 P5 P6
				//           P1: Target unit number. (0-6)
				//           P2: Write Or Read (0:Write, 1:Raed)
				//           P3: Arbiter (0:Slave-1, 1:Slave-2, 2:Slave-3, 3:Slave-4)
				//           P4: Port (0~3)
				//           P5: Slot (0~3)
				//           P6: Mask enable (0:Disable, 1:Enable)
				if(argc >= 10) {
					result = Im_MXIC_Set_Slot_Mask( (E_IM_MXIC_UNIT)atoi((const char *)argv[4]),
													(E_IM_MXIC_WR_ARBITER)atoi((const char *)argv[5]),
													(E_IM_MXIC_SPEC_ARBITER)atoi((const char *)argv[6]),
													(E_IM_MXIC_PORT)atoi((const char *)argv[7]),
													(E_IM_MXIC_SLOT)atoi((const char *)argv[8]),
													(UCHAR)atoi((const char *)argv[9]));
					if (result == D_DDIM_OK) {
						Ddim_Print(("Im_MXIC_Set_Slot_Mask OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_Slot_Mask NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 6\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// [command] immxic mask slot get P1 P2 P3 P4 P5
				//           P1: Target unit number. (0-6)
				//           P2: Write Or Read (0:Write, 1:Raed)
				//           P3: Arbiter (0:Slave-1, 1:Slave-2, 2:Slave-3, 3:Slave-4)
				//           P4: Port (0~3)
				//           P5: Slot (0~3)
				if(argc >= 9) {
					result = Im_MXIC_Get_Slot_Mask( (E_IM_MXIC_UNIT)atoi((const char *)argv[4]),
													(E_IM_MXIC_WR_ARBITER)atoi((const char *)argv[5]),
													(E_IM_MXIC_SPEC_ARBITER)atoi((const char *)argv[6]),
													(E_IM_MXIC_PORT)atoi((const char *)argv[7]),
													(E_IM_MXIC_SLOT)atoi((const char *)argv[8]),
													&mask_enable);
					if (result == D_DDIM_OK) {
						Ddim_Print(("mask_enable = %d\n", mask_enable));
						Ddim_Print(("Im_MXIC_Get_Slot_Mask OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_Slot_Mask NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 5\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "master") == 0) {
			if (strcmp((const char *)argv[3], "enable") == 0) {
				if (strcmp((const char *)argv[4], "set") == 0) {
					// [command] immxic mask master enable set P1 P2 P3 P4
					//           P1: Target unit number. (0-6)
					//           P2: Master number (Hex value)
					//           P3: Mask type (0:Mask OFF, 1:Mask ON, 2:Mask ON during specified period)
					//           P4: Mask period (4~4096. 0 means 4096.)
					if(argc >= 9) {
						unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[5]);
						master_mask.mask_type   = (UCHAR)atoi((const char *)argv[7]);
						master_mask.mask_period = (USHORT)atoi((const char *)argv[8]);

						result = Im_MXIC_Set_Master_Mask(unit, (UCHAR)ct_im_mxic_atoi_hex(argv[6]), &master_mask);
						if (result == D_DDIM_OK) {
							Ddim_Print(("Im_MXIC_Set_Master_Mask OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Set_Master_Mask NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 4\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "get") == 0) {
					// [command] immxic mask master enable get P1 P2
					//           P1: Target unit number. (0-6)
					//           P2: Master number (Hex value)
					if(argc >= 7) {
						unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[5]);
						result = Im_MXIC_Get_Master_Mask(unit, (UCHAR)ct_im_mxic_atoi_hex(argv[6]), &master_mask);
						if (result == D_DDIM_OK) {
							Ddim_Print(("master_mask.mask_type   = %d\n", master_mask.mask_type));
							Ddim_Print(("master_mask.mask_period = %d\n", master_mask.mask_period));
							Ddim_Print(("Im_MXIC_Get_Master_Mask OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Get_Master_Mask NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 2\n"));
					}
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "factor") == 0) {
				if (strcmp((const char *)argv[4], "set") == 0) {
					// [command] immxic mask master factor set P1 P2 P3 P4
					//           P1: Target unit number. (0-6)
					//           P2: Master number (Hex value)
					//           P3: Mask group (0:Group-A, 1:Group-B)
					//           P4: Mask enable (0:Disable, 1:Enable)
					if(argc >= 9) {
						result = Im_MXIC_Set_Master_Mask_Factor( (E_IM_MXIC_UNIT)atoi((const char *)argv[5]),
																 (UCHAR)ct_im_mxic_atoi_hex(argv[6]),
																 (E_IM_MXIC_MASK_GROUP)atoi((const char *)argv[7]),
																 (UCHAR)atoi((const char *)argv[8]));
						if ( result == D_DDIM_OK ) {
							Ddim_Print(("Im_MXIC_Set_Master_Mask_Factor OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 4\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "get") == 0) {
					// [command] immxic mask master factor get P1 P2 P3
					//           P1: Target unit number. (0-6)
					//           P2: Master number (Hex value)
					//           P3: Mask group (0:Group-A, 1:Group-B)
					if(argc >= 8) {
						unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[5]);
						result = Im_MXIC_Get_Master_Mask_Factor(unit,
															   (UCHAR)ct_im_mxic_atoi_hex(argv[6]),
															   (E_IM_MXIC_MASK_GROUP)atoi((const char *)argv[7]),
															   &mask_enable);
						if (result == D_DDIM_OK) {
							Ddim_Print(("mask_enable = %d\n", mask_enable));
							Ddim_Print(("Im_MXIC_Get_Master_Mask_Factor OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Get_Master_Mask_Factor NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 3\n"));
					}
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "target") == 0) {
				if (strcmp((const char *)argv[4], "set") == 0) {
					// [command] immxic mask master target set P1 P2 P3 P4
					//           P1: Target unit number. (0-6)
					//           P2: Master number (Hex value)
					//           P3: Mask group (0:Group-A, 1:Group-B)
					//           P4: Mask enable (0:Disable, 1:Enable)
					if(argc >= 9) {
						result = Im_MXIC_Set_Master_Mask_Target( (E_IM_MXIC_UNIT)atoi((const char *)argv[5]),
																 (UCHAR)ct_im_mxic_atoi_hex(argv[6]),
																 (E_IM_MXIC_MASK_GROUP)atoi((const char *)argv[7]),
																 (UCHAR)atoi((const char *)argv[8]));
						if (result == D_DDIM_OK) {
							Ddim_Print(("Im_MXIC_Set_Master_Mask_Target OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 4\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "get") == 0) {
					// [command] immxic mask master target get P1 P2 P3
					//           P1: Target unit number. (0-6)
					//           P2: Master number (Hex value)
					//           P3: Mask group (0:Group-A, 1:Group-B)
					if(argc >= 8) {
						result = Im_MXIC_Get_Master_Mask_Target( (E_IM_MXIC_UNIT)atoi((const char *)argv[5]),
																 (UCHAR)ct_im_mxic_atoi_hex(argv[6]),
																 (E_IM_MXIC_MASK_GROUP)atoi((const char *)argv[7]),
																 &mask_enable);
						if (result == D_DDIM_OK) {
							Ddim_Print(("mask_enable = %d\n", mask_enable));
							Ddim_Print(("Im_MXIC_Get_Master_Mask_Target OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Get_Master_Mask_Target NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 3\n"));
					}
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "select") == 0) {
				if (strcmp((const char *)argv[4], "set") == 0) {
					// [command] immxic mask master select set P1 P2 P3
					//           P1: Target unit number. (0-6)
					//           P2: Master number (Hex value)
					//           P3: Mask select (0:Off, 1:On)
					if(argc >= 8) {
						result = Im_MXIC_Set_Master_Mask_Select( (E_IM_MXIC_UNIT)atoi((const char *)argv[5]),
																 (UCHAR)ct_im_mxic_atoi_hex(argv[6]),
																 (UCHAR)atoi((const char *)argv[7]));
						if (result == D_DDIM_OK) {
							Ddim_Print(("Im_MXIC_Set_Master_Mask_Select OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Select NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 3\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "get") == 0) {
					// [command] immxic mask master select get P1 P2
					//           P1: Target unit number. (0-6)
					//           P2: Master number (Hex value)
					if(argc >= 7) {
						result = Im_MXIC_Get_Master_Mask_Select( (E_IM_MXIC_UNIT)atoi((const char *)argv[5]),
																 (UCHAR)ct_im_mxic_atoi_hex(argv[6]),
																 &mask_select);
						if ( result == D_DDIM_OK ) {
							Ddim_Print(("mask_select = %d\n", mask_select));
							Ddim_Print(("Im_MXIC_Get_Master_Mask_Select OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Get_Master_Mask_Select NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 2\n"));
					}
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "trn") == 0) {
				if (strcmp((const char *)argv[4], "set") == 0) {
					// [command] immxic mask master trn set P1 P2 P3
					//           P1: Target unit number. (0-6)
					//           P2: Master number (Hex value)
					//           P3: Master transaction (0~63)
					if(argc >= 8) {
						result = Im_MXIC_Set_Master_Transaction( (E_IM_MXIC_UNIT)atoi((const char *)argv[5]),
																 (UCHAR)ct_im_mxic_atoi_hex(argv[6]),
																 (UCHAR)atoi((const char *)argv[7]));
						if ( result == D_DDIM_OK ) {
							Ddim_Print(("Im_MXIC_Set_Master_Transaction OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Set_Master_Transaction NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 3\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "get") == 0) {
					// [command] immxic mask master trn get P1 P2
					//           P1: Target unit number. (0-6)
					//           P2: Master number (Hex value)
					if(argc >= 7) {
						result = Im_MXIC_Get_Master_Transaction( (E_IM_MXIC_UNIT)atoi((const char *)argv[5]),
																 (UCHAR)ct_im_mxic_atoi_hex(argv[6]),
																 &transaction);
						if (result == D_DDIM_OK) {
							Ddim_Print(("transaction = %d\n", transaction));
							Ddim_Print(("Im_MXIC_Get_Master_Transaction OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Get_Master_Transaction NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 2\n"));
					}
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else {
			Ddim_Print(("E:Unknown command.\n"));
		}
	}
	else if (strcmp((const char *)argv[1], "levelport") == 0) {
		if (strcmp((const char *)argv[2], "port") == 0) {
			if (strcmp((const char *)argv[3], "set") == 0) {
				// [command] immxic levelport port set P1 P2 P3 P4 P5
				//           P1: Target unit number. (0-6)
				//           P2: Target write or read Arbiter (0:Write, 1:Read).
				//           P3: Target arbiter (0:Slave-1, 1:Slave-2, 2:Slave-3, 3:Slave-4).
				//           P4: Master number (Hex value).
				//           P5: Target Port (0-3).
				if(argc >= 9) {
					result = Im_MXIC_Set_LevelPort_Port( (E_IM_MXIC_UNIT)atoi((const char *)argv[4]),
														 (E_IM_MXIC_WR_ARBITER)atoi((const char *)argv[5]),
														 (E_IM_MXIC_SPEC_ARBITER)atoi((const char *)argv[6]),
														 (UCHAR)ct_im_mxic_atoi_hex(argv[7]),
														 (E_IM_MXIC_PORT)atoi((const char *)argv[8]) );
					if ( result == D_DDIM_OK ) {
						Ddim_Print(("Im_MXIC_Set_LevelPort_Port OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_LevelPort_Port NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 5\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// [command] immxic levelport port get P1 P2 P3 P4
				//           P1: Target unit number. (0-6)
				//           P2: Target write or read Arbiter (0:Write, 1:Read).
				//           P3: Target arbiter (0:Slave-1, 1:Slave-2, 2:Slave-3, 3:Slave-4).
				//           P4: Master number (Hex value).
				if(argc >= 8) {
					result = Im_MXIC_Get_LevelPort_Port( (E_IM_MXIC_UNIT)atoi((const char *)argv[4]),
														 (E_IM_MXIC_WR_ARBITER)atoi((const char *)argv[5]),
														 (E_IM_MXIC_SPEC_ARBITER)atoi((const char *)argv[6]),
														 (UCHAR)ct_im_mxic_atoi_hex(argv[7]),
														 &port );
					if ( result == D_DDIM_OK ) {
						Ddim_Print(("Target port = %d\n", port));
						Ddim_Print(("Im_MXIC_Get_LevelPort_Port OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_LevelPort_Port NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 4\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "level") == 0) {
			if (strcmp((const char *)argv[3], "set") == 0) {
				// [command] immxic levelport level set P1 P2 P3 P4 P5
				//           P1: Target unit number. (0-6)
				//           P2: Target write or read Arbiter (0:Write, 1:Read).
				//           P3: Target arbiter (0:Slave-1, 1:Slave-2, 2:Slave-3, 3:Slave-4).
				//           P4: Master number (Hex value).
				//           P5: Target level (0-3).
				if(argc >= 9) {
					result = Im_MXIC_Set_LevelPort_Level((E_IM_MXIC_UNIT)atoi((const char *)argv[4]),
														 (E_IM_MXIC_WR_ARBITER)atoi((const char *)argv[5]),
														 (E_IM_MXIC_SPEC_ARBITER)atoi((const char *)argv[6]),
														 (UCHAR)ct_im_mxic_atoi_hex(argv[7]),
														 (E_IM_MXIC_LEVEL)atoi((const char *)argv[8]) );
					if ( result == D_DDIM_OK ) {
						Ddim_Print(("Im_MXIC_Set_LevelPort_Level OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_LevelPort_Level NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 5\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// [command] immxic levelport level get P1 P2 P3 P4
				//           P1: Target unit number. (0-6)
				//           P2: Target write or read Arbiter (0:Write, 1:Read).
				//           P3: Target arbiter (0:Slave-1, 1:Slave-2, 2:Slave-3, 3:Slave-4).
				//           P4: Master number (Hex value).
				if(argc >= 8) {
					result = Im_MXIC_Get_LevelPort_Level((E_IM_MXIC_UNIT)atoi((const char *)argv[4]),
														 (E_IM_MXIC_WR_ARBITER)atoi((const char *)argv[5]),
														 (E_IM_MXIC_SPEC_ARBITER)atoi((const char *)argv[6]),
														 (UCHAR)ct_im_mxic_atoi_hex(argv[7]),
														 &level );
					if ( result == D_DDIM_OK ) {
						Ddim_Print(("Target level = %d\n", level));
						Ddim_Print(("Im_MXIC_Get_LevelPort_Level OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_LevelPort_Level NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 4\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "all") == 0) {
			if (strcmp((const char *)argv[3], "set") == 0) {
				// [command] immxic levelport all set P1
				//           P1: Target unit number. (0-6)
				result = ct_im_mxic_create_all_levelport_param(&all_levelport);
				if (result == D_DDIM_OK) {
					// Execute function.
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
					result = Im_MXIC_Set_LevelPort_All(unit, &all_levelport);
					if (result == D_DDIM_OK) {
						Ddim_Print(("Im_MXIC_Set_LevelPort_All OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_LevelPort_All NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Command parameter error.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// [command] immxic levelport all get P1
				//           P1: Target unit number. (0-6)
				// Execute function.
				unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
				result = Im_MXIC_Get_LevelPort_All(unit, &all_levelport);
				if (result == D_DDIM_OK) {
					ct_im_mxic_print_all_levelport(&all_levelport);
					Ddim_Print(("Im_MXIC_Get_LevelPort_All OK.\n"));
				}
				else {
					Ddim_Print(("E:Im_MXIC_Get_LevelPort_All NG. result=0x%X\n", result));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else {
			Ddim_Print(("E:Unknown command.\n"));
		}
	}
	else if (strcmp((const char *)argv[1], "port") == 0) {
		if (strcmp((const char *)argv[2], "assign") == 0) {
			if (strcmp((const char *)argv[3], "one") == 0) {
				if (strcmp((const char *)argv[4], "set") == 0) {
					// [command] immxic port assign one set P1 P2
					//           P1: master number group (Hex value)
					//           P2: port (0:port0, 1:port1)
					if(argc >= 7) {
						result = Im_MXIC_Set_Output_Port((UCHAR)ct_im_mxic_atoi_hex(argv[5]),
														(E_IM_MXIC_PORT_GR)atoi((const char *)argv[6]));
						if (result == D_DDIM_OK) {
							Ddim_Print(("Im_MXIC_Set_Output_Port OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Set_Output_Port NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 7\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "get") == 0) {
					// [command] immxic port assign one get P1
					//           P1: master number group (Hex value)
					if(argc >= 6) {
						result = Im_MXIC_Get_Output_Port((UCHAR)ct_im_mxic_atoi_hex(argv[5]), &port_assign);
						if (result == D_DDIM_OK) {
							Ddim_Print(("port_assign = %d\n", port_assign));
							Ddim_Print(("Im_MXIC_Get_Output_Port OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Get_Output_Port NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 6\n"));
					}
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "all") == 0) {
				if (strcmp((const char *)argv[4], "set") == 0) {
					// [command] immxic port assign all set PP
					//           PP: Port set pattern.
					//               0 : All 0
					//               1 : All 1
					//               2 : Random pattern 1
					//               3 : Random pattern 2
					if(argc >= 6) {
						result = ct_im_mxic_all_port_set(atoi((const char *)argv[5]), &all_port_assign);
						if (result == D_DDIM_OK) {
							result = Im_MXIC_Set_Output_Port_All(&all_port_assign);
							if (result == D_DDIM_OK) {
								Ddim_Print(("Im_MXIC_Set_Output_Port_All OK.\n"));
							}
							else {
								Ddim_Print(("E:Im_MXIC_Set_Output_Port_All NG. result=0x%X\n", result));
							}
						}
						else {
							Ddim_Print(("E:Command parameter error.\n"));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 6\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "get") == 0) {
					// [command] immxic port assign all get
					result = Im_MXIC_Get_Output_Port_All(&all_port_assign);
					if (result == D_DDIM_OK) {
						ct_im_mxic_print_all_port_assign(&all_port_assign);
						Ddim_Print(("Im_MXIC_Get_Output_Port_All OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_Output_Port_All NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else {
			Ddim_Print(("E:Unknown command.\n"));
		}
	}
	else if (strcmp((const char *)argv[1], "monitor") == 0) {
		if (strcmp((const char *)argv[2], "start") == 0) {
			// [command] immxic monitor start
			Im_MXIC_Start_Monitor();
			Ddim_Print(("Im_MXIC_Start_Monitor OK.\n"));
		}
		else if (strcmp((const char *)argv[2], "stop") == 0) {
			// [command] immxic monitor stop
			Im_MXIC_Stop_Monitor();
			Ddim_Print(("Im_MXIC_Stop_Monitor OK.\n"));
		}
		else if (strcmp((const char *)argv[2], "hist") == 0) {
			if (strcmp((const char *)argv[3], "thstop") == 0) {
				if (strcmp((const char *)argv[4], "set") == 0) {
					// [command] immxic monitor hist thstop set P1
					//           P1: THSTOP enable (0:Disable, 1:Enable)
					if(argc >= 6) {
						result = Im_MXIC_Set_History_Monitor_Stop_Enable((UCHAR)atoi((const char *)argv[5]));
						if (result == D_DDIM_OK) {
							Ddim_Print(("Im_MXIC_Set_History_Monitor_Stop_Enable OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Set_History_Monitor_Stop_Enable NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 1\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "get") == 0) {
					// [command] immxic monitor hist thstop get
					result = Im_MXIC_Get_History_Monitor_Stop_Enable(&thstop_enable);
					if (result == D_DDIM_OK) {
						Ddim_Print(("thstop_enable = %d\n", thstop_enable));
						Ddim_Print(("Im_MXIC_Get_History_Monitor_Stop_Enable OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_History_Monitor_Stop_Enable NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "one") == 0) {
				// [command] immxic monitor hist one P1 P2 P3
				//           P1: Write Or Read (0:Write, 1:Raed)
				//           P2: Arbiter (0:Slave-1, 1:Slave-2, 2:Slave-3, 3:Slave-4)
				//           P3: Port (0:port0, 1:port1, 2:port2, 3:port3)
				if(argc >= 7) {
					result = Im_MXIC_Get_History_Monitor((E_IM_MXIC_WR_ARBITER)atoi((const char *)argv[4]),
														(E_IM_MXIC_SPEC_ARBITER)atoi((const char *)argv[5]),
														(E_IM_MXIC_PORT)atoi((const char *)argv[6]),
														&history);
					if (result == D_DDIM_OK) {
						ct_im_mxic_print_history_monitor(&history);
						Ddim_Print(("Im_MXIC_Get_History_Monitor OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_History_Monitor NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Parameter nums must be 3\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "all") == 0) {
				// [command] immxic monitor hist all
				result = Im_MXIC_Get_History_Monitor_All_Port(&all_history);
				if (result == D_DDIM_OK) {
					ct_im_mxic_print_all_history_monitor(&all_history);
					Ddim_Print(("Im_MXIC_Get_History_Monitor_All_Port OK.\n"));
				}
				else {
					Ddim_Print(("E:Im_MXIC_Get_History_Monitor_All_Port NG. result=0x%X\n", result));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "status") == 0) {
			if (strcmp((const char *)argv[3], "slot") == 0) {
				if (strcmp((const char *)argv[4], "all") == 0) {
					// [command] immxic monitor status slot all P1
					//           P1: Target unit number. (0-6)
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[5]);
					result = Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter(unit, &all_slot_status);
					if (result == D_DDIM_OK) {
						ct_im_mxic_print_all_slot_status_monitor(&all_slot_status);
						Ddim_Print(("Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter NG. result=0x%X\n", result));
					}
				}
				else if (strcmp((const char *)argv[4], "w") == 0) {
					// [command] immxic monitor status slot w P1 P2
					//           P1: Target unit number. (0-6)
					//           P2: W arbiter (0:W1, 1:W2, 2:W3, 3:W4)
					if(argc >= 7) {
						unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[5]);
						result = Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter(unit, (E_IM_MXIC_W_ARBITER)atoi((const char *)argv[6]), &w_slot_status);
						if (result == D_DDIM_OK) {
							ct_im_mxic_print_w_arbiter_assign(&w_slot_status);
							Ddim_Print(("Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 2\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "r") == 0) {
					// [command] immxic monitor status slot r P1 P2
					//           P1: Target unit number. (0-6)
					//           P2: R arbiter (0:R1, 1:R2, 2:R3, 3:R4)
					if(argc >= 7) {
						unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[5]);
						result = Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter(unit, (E_IM_MXIC_R_ARBITER)atoi((const char *)argv[6]), &r_slot_status);
						if (result == D_DDIM_OK) {
							ct_im_mxic_print_r_arbiter_assign(&r_slot_status);
							Ddim_Print(("Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 2\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "wgr") == 0) {
					// [command] immxic monitor status slot wgr P1
					//           P1: Group W arbiter (0:W1, 1:W2, 2:W3, 3:W4, 4:W5, 5:W6, 6:W7)
					if(argc >= 6) {
						result = Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group((E_IM_MXIC_W_ARBITER_GR)atoi((const char *)argv[5]), &w_gr_slot_status);
						if (result == D_DDIM_OK) {
							ct_im_mxic_print_w_arbiter_assign_group(&w_gr_slot_status);
							Ddim_Print(("Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 1\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "rgr") == 0) {
					// [command] immxic monitor status slot rgr P1
					//           P1: Group R arbiter (0:R1, 1:R2, 2:R3, 3:R4, 4:R5, 5:R6, 6:R7)
					if(argc >= 6) {
						result = Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group((E_IM_MXIC_R_ARBITER_GR)atoi((const char *)argv[5]), &r_gr_slot_status);
						if (result == D_DDIM_OK) {
							ct_im_mxic_print_r_arbiter_assign_group(&r_gr_slot_status);
							Ddim_Print(("Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 1\n"));
					}
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "master") == 0) {
				// [command] immxic monitor status master P1
				//           P1: Target unit number. (0-6)
				unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[4]);
				result = Im_MXIC_Get_Master_Status_Monitor(unit, &master_status);
				if (result == D_DDIM_OK) {
					ct_im_mxic_print_master_status_monitor(&master_status);
					Ddim_Print(("Im_MXIC_Get_Master_Status_Monitor OK.\n"));
				}
				else {
					Ddim_Print(("E:Im_MXIC_Get_Master_Status_Monitor NG. result=0x%X\n", result));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "acc") == 0) {
			if (strcmp((const char *)argv[3], "param") == 0) {
				if (strcmp((const char *)argv[4], "set") == 0) {
					// [command] immxic monitor acc param set PP
					//           PP: Port set pattern. (0~4)
					if(argc >= 6) {
						// Get parameter for Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter function.
						result = ct_im_mxic_create_access_or_trans_monitor_param((UCHAR)atoi((const char *)argv[5]), &mon_param);
						if (result == D_DDIM_OK) {
							// Execute function.
							result = Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter(&mon_param);
							if (result == D_DDIM_OK) {
								Ddim_Print(("Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter OK.\n"));
							}
							else {
								Ddim_Print(("E:Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter NG. result=0x%X\n", result));
							}
						}
						else {
							Ddim_Print(("E:Command parameter error.\n"));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 1\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "get") == 0) {
					// [command] immxic monitor acc param get
					result = Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter(&mon_param);
					if (result == D_DDIM_OK) {
						ct_im_mxic_print_monitor_parameter(&mon_param);
						Ddim_Print(("Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "result") == 0) {
				if (strcmp((const char *)argv[4], "all") == 0) {
					// [command] immxic monitor acc result all
					result = Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry(&all_acc_trans);
					if (result == D_DDIM_OK) {
						ct_im_mxic_print_all_access_or_trans_monitor(&all_acc_trans);
						Ddim_Print(("Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry NG. result=0x%X\n", result));
					}
				}
				else if (strcmp((const char *)argv[4], "one") == 0) {
					// [command] immxic monitor acc result one P1
					//           P1: Monitor number (0~3)
					if(argc >= 6) {
						result = Im_MXIC_Get_Access_Or_Trans_Monitor((UCHAR)atoi((const char *)argv[5]), &mon_result);
						if (result == D_DDIM_OK) {
							Ddim_Print(("mon_result = %d\n", mon_result));
							Ddim_Print(("Im_MXIC_Get_Access_Or_Trans_Monitor OK.\n"));
						}
						else {
							Ddim_Print(("E:Im_MXIC_Get_Access_Or_Trans_Monitor NG. result=0x%X\n", result));
						}
					}
					else {
						Ddim_Print(("E:Parameter nums must be 1\n"));
					}
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "state") == 0) {
				// [command] immxic monitor acc state
				result = Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State(&mon_state);
				if (result == D_DDIM_OK) {
					Ddim_Print(("mon_state = %d\n", mon_state));
					Ddim_Print(("Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State OK.\n"));
				}
				else {
					Ddim_Print(("E:Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State NG. result=0x%X\n", result));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else {
			Ddim_Print(("E:Unknown command.\n"));
		}
	}
	else if (strcmp((const char *)argv[1], "memacc") == 0) {
		if (strcmp((const char *)argv[2], "start") == 0) {
			// [command] immxic memacc start P1 P2
			//           P1: Target unit number. (0-6)
			//           P2: W arbiter (0:W1, 1:W2, 2:W3, 3:W4 9:ALL)
			if (argc >= 5) {
				result = ct_im_mxic_create_memory_access_start_trigger(atoi((const char *)argv[4]), &memory_access);
				if (result == D_DDIM_OK) {
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[3]);
					result = Im_MXIC_Start_Memory_Access_Detect(unit, &memory_access);
					if (result == D_DDIM_OK) {
						Ddim_Print(("Im_MXIC_Start_Memory_Access_Detect OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Start_Memory_Access_Detect NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Command parameter error.\n"));
				}
			}
			else {
				Ddim_Print(("E:Parameter nums must be 2\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "stop") == 0) {
			// [command] immxic memacc stop
			//           P1: Target unit number. (0-6)
			unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[3]);
			Im_MXIC_Stop_Memory_Access_Detect(unit);
			Ddim_Print(("Im_MXIC_Stop_Memory_Access_Detect OK.\n"));
		}
		else if (strcmp((const char *)argv[2], "set") == 0) {
			// [command] immxic memacc set P1 P2 P3
			//           P1: Target unit number. (0-6)
			//           P2: W arbiter (0:W1, 1:W2, 2:W3, 3:W4 9:ALL)
			//           P3: Parameter Pattern (0~1)
			if(argc >= 6) {
				result = ct_im_mxic_create_memory_access_param(atoi((const char *)argv[4]), atoi((const char *)argv[5]), &memory_access);
				if (result == D_DDIM_OK) {
					unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[3]);
					Im_MXIC_Set_Memory_Access_Detect(unit, &memory_access);
					if (result == D_DDIM_OK) {
						Ddim_Print(("Im_MXIC_Set_Memory_Access_Detect OK.\n"));
					}
					else {
						Ddim_Print(("E:Im_MXIC_Set_Memory_Access_Detect NG. result=0x%X\n", result));
					}
				}
				else {
					Ddim_Print(("E:Command parameter error.\n"));
				}
			}
			else {
				Ddim_Print(("E:Parameter nums must be 3\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "get") == 0) {
			// [command] immxic memacc get P1
			//           P1: Target unit number. (0-6)
			unit = (E_IM_MXIC_UNIT)atoi((const char *)argv[3]);
			result = Im_MXIC_Get_Memory_Access_Detect(unit, &memory_access);
			if (result == D_DDIM_OK) {
				ct_im_mxic_print_memory_access_param(&memory_access);
				Ddim_Print(("Im_MXIC_Get_Memory_Access_Detect OK.\n"));
			}
			else {
				Ddim_Print(("E:Im_MXIC_Get_Memory_Access_Detect NG. result=0x%X\n", result));
			}
		}
	}
	else if (strcmp((const char *)argv[1], "err") == 0) {
		// Test command for processing of parameter check
		if (strcmp((const char *)argv[2], "start") == 0) {
			// Im_MXIC_Start_Config
			// [command] immxic err start
			ct_im_mxic_check_err_start_config();
		}
		else if (strcmp((const char *)argv[2], "clock") == 0) {
			// Im_MXIC_Set_Clock_Enable
			// Im_MXIC_Get_Clock_Enable
			// [command] immxic err clock
			ct_im_mxic_check_err_clock_enable();
		}
		else if (strcmp((const char *)argv[2], "acc_capa") == 0) {
			if (strcmp((const char *)argv[3], "slave") == 0) {
				// Im_MXIC_Set_Acceptance_Capability
				// Im_MXIC_Get_Acceptance_Capability
				// [command] immxic err acc_capa slave
				ct_im_mxic_check_err_acceptance_capability();
			}
			else if (strcmp((const char *)argv[3], "group") == 0) {
				// Im_MXIC_Set_Acceptance_Capability_Group
				// Im_MXIC_Get_Acceptance_Capability_Group
				// [command] immxic err acc_capa group
				ct_im_mxic_check_err_acceptance_capability_group();
			}
			else if (strcmp((const char *)argv[3], "all") == 0) {
				// Im_MXIC_Set_Acceptance_Capability_All_Port
				// Im_MXIC_Get_Acceptance_Capability_All_Port
				// [command] immxic err acc_capa all
				ct_im_mxic_check_err_acceptance_capability_all_port();
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "slave_area") == 0) {
			if (strcmp((const char *)argv[3], "one") == 0) {
				// Im_MXIC_Set_Slave_Area
				// Im_MXIC_Get_Slave_Area
				// [command] immxic err slave_area one
				ct_im_mxic_check_err_slave_area();
			}
			else if (strcmp((const char *)argv[3], "all") == 0) {
				// Im_MXIC_Set_Slave_Area_All
				// Im_MXIC_Get_Slave_Area_All
				// [command] immxic err slave_area all
				ct_im_mxic_check_err_slave_area_all();
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "dec_err") == 0) {
			// Im_MXIC_Set_Decode_Error_Int
			// Im_MXIC_Get_Decode_Error_Int
			// Im_MXIC_Get_Decode_Error
			// [command] immxic err dec_err
			ct_im_mxic_check_err_decode_error();
		}
		else if (strcmp((const char *)argv[2], "levelport") == 0) {
			if (strcmp((const char *)argv[3], "level") == 0) {
				// Im_MXIC_Set_LevelPort_Level
				// Im_MXIC_Get_LevelPort_Level
				// [command] immxic err levelport level
				ct_im_mxic_check_err_levelport_level();
			}
			else if (strcmp((const char *)argv[3], "port") == 0) {
				// Im_MXIC_Set_LevelPort_Port
				// Im_MXIC_Get_LevelPort_Port
				// [command] immxic err levelport port
				ct_im_mxic_check_err_levelport_port();
			}
			else if (strcmp((const char *)argv[3], "all") == 0) {
				// Im_MXIC_Set_LevelPort_All
				// Im_MXIC_Get_LevelPort_All
				// [command] immxic err levelport all
				ct_im_mxic_check_err_levelport_all();
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "port_assign") == 0) {
			if (strcmp((const char *)argv[3], "one") == 0) {
				// Im_MXIC_Set_Output_Port
				// Im_MXIC_Get_Output_Port
				// [command] immxic err port_assign one
				ct_im_mxic_check_err_port_assign();
			}
			else if (strcmp((const char *)argv[3], "all") == 0) {
				// Im_MXIC_Set_Output_Port_All
				// Im_MXIC_Get_Output_Port_All
				// [command] immxic err port_assign all
				ct_im_mxic_check_err_port_assign_all();
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "master") == 0) {
			if (strcmp((const char *)argv[3], "w") == 0) {
				// Im_MXIC_Set_Master_W_Arbiter
				// Im_MXIC_Get_Master_W_Arbiter
				// [command] immxic err master w
				ct_im_mxic_check_err_master_w_arbiter();
			}
			else if (strcmp((const char *)argv[3], "r") == 0) {
				// Im_MXIC_Set_Master_R_Arbiter
				// Im_MXIC_Get_Master_R_Arbiter
				// [command] immxic err master r
				ct_im_mxic_check_err_master_r_arbiter();
			}
			else if (strcmp((const char *)argv[3], "all") == 0) {
				// Im_MXIC_Set_Master_All_Arbiter
				// Im_MXIC_Get_Master_All_Arbiter
				// [command] immxic err master all
				ct_im_mxic_check_err_master_all_arbiter();
			}
			else if (strcmp((const char *)argv[3], "overlap") == 0) {
				// Im_MXIC_Set_Master_R_Arbiter
				// Im_MXIC_Set_Master_W_Arbiter
				ct_im_mxic_check_err_master_overlap();
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "pri") == 0) {
			if (strcmp((const char *)argv[3], "slot") == 0) {
				if (strcmp((const char *)argv[4], "one") == 0) {
					// Im_MXIC_Set_Slot_Priority
					// Im_MXIC_Get_Slot_Priority
					// [command] immxic err pri slot one
					ct_im_mxic_check_err_slot_priority();
				}
				else if (strcmp((const char *)argv[4], "all") == 0) {
					// Im_MXIC_Set_Slot_Priority_All_Arbiter
					// Im_MXIC_Get_Slot_Priority_All_Arbiter
					// [command] immxic err pri slot all
					ct_im_mxic_check_err_slot_priority_all_arbiter();
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "master") == 0) {
				// Im_MXIC_Set_Master_Priority
				// Im_MXIC_Get_Master_Priority
				// [command] immxic err pri master
				ct_im_mxic_check_err_master_priority();
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "mask") == 0) {
			if (strcmp((const char *)argv[3], "slot") == 0) {
				// Im_MXIC_Set_Slot_Mask
				// Im_MXIC_Get_Slot_Mask
				// [command] immxic err mask slot
				ct_im_mxic_check_err_slot_mask();
			}
			else if (strcmp((const char *)argv[3], "master") == 0) {
				if (strcmp((const char *)argv[4], "enable") == 0) {
					// Im_MXIC_Set_Master_Mask
					// Im_MXIC_Get_Master_Mask
					// [command] immxic err mask master enable
					ct_im_mxic_check_err_master_mask();
				}
				else if (strcmp((const char *)argv[4], "factor") == 0) {
					// Im_MXIC_Set_Master_Mask_Factor
					// Im_MXIC_Get_Master_Mask_Factor
					// [command] immxic err mask master factor
					ct_im_mxic_check_err_master_mask_factor();
				}
				else if (strcmp((const char *)argv[4], "target") == 0) {
					// Im_MXIC_Set_Master_Mask_Target
					// Im_MXIC_Get_Master_Mask_Target
					// [command] immxic err mask master target
					ct_im_mxic_check_err_master_mask_target();
				}
				else if (strcmp((const char *)argv[4], "select") == 0) {
					// Im_MXIC_Set_Master_Mask_Select
					// Im_MXIC_Get_Master_Mask_Select
					// [command] immxic err mask master select
					ct_im_mxic_check_err_master_mask_select();
				}
				else if (strcmp((const char *)argv[4], "trn") == 0) {
					// Im_MXIC_Set_Master_Transaction
					// Im_MXIC_Get_Master_Transaction
					// [command] immxic err master transaction
					ct_im_mxic_check_err_master_transaction();
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "monitor") == 0) {
			if (strcmp((const char *)argv[3], "hist") == 0) {
				if (strcmp((const char *)argv[4], "thstop") == 0) {
					// Im_MXIC_Set_History_Monitor_Stop_Enable
					// Im_MXIC_Get_History_Monitor_Stop_Enable
					// [command] immxic err monitor hist thstop
					ct_im_mxic_check_err_history_monitor_stop_enable();
				}
				else if (strcmp((const char *)argv[4], "one") == 0) {
					// Im_MXIC_Get_History_Monitor
					// [command] immxic err monitor hist one
					ct_im_mxic_check_err_history_monitor();
				}
				else if (strcmp((const char *)argv[4], "all") == 0) {
					// Im_MXIC_Get_History_Monitor_All_Port
					// [command] immxic err monitor hist all
					ct_im_mxic_check_err_history_monitor_all_port();
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "status") == 0) {
				if (strcmp((const char *)argv[4], "slot") == 0) {
					if (strcmp((const char *)argv[5], "all") == 0) {
						// Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter
						// [command] immxic err monitor status slot all
						ct_im_mxic_check_err_slot_status_monitor_all_arbiter();
					}
					else if (strcmp((const char *)argv[5], "slave") == 0) {
						if (strcmp((const char *)argv[6], "w") == 0) {
							// Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter
							// [command] immxic err monitor status slot w
							ct_im_mxic_check_err_slot_status_monitor_w_arbiter();
						}
						else if(strcmp((const char *)argv[6], "r") == 0) {
							// Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter
							// [command] immxic err monitor status slot r
							ct_im_mxic_check_err_slot_status_monitor_r_arbiter();
						}
						else {
							Ddim_Print(("E:Unknown command.\n"));
						}
					}
					else if (strcmp((const char *)argv[5], "group") == 0) {
						if (strcmp((const char *)argv[6], "w") == 0) {
							// Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter
							// [command] immxic err monitor status slot w
							ct_im_mxic_check_err_slot_status_monitor_w_arbiter_group();
						}
						else if(strcmp((const char *)argv[6], "r") == 0) {
							// Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter
							// [command] immxic err monitor status slot r
							ct_im_mxic_check_err_slot_status_monitor_r_arbiter_group();
						}
						else {
							Ddim_Print(("E:Unknown command.\n"));
						}
					}
					else {
						Ddim_Print(("E:Unknown command.\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "master") == 0) {
					// Im_MXIC_Get_Master_Status_Monitor
					// [command] immxic err monitor status master
					ct_im_mxic_check_err_master_status_monitor();
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "acc") == 0) {
				if (strcmp((const char *)argv[4], "param") == 0) {
					if (strcmp((const char *)argv[5], "set") == 0) {
						// Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter
						// [command] immxic err monitor acc param set
						ct_im_mxic_check_err_set_access_or_trans_monitor_parameter();
					}
					else if (strcmp((const char *)argv[5], "get") == 0) {
						// Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter
						// [command] immxic err monitor acc param get
						ct_im_mxic_check_err_get_access_or_trans_monitor_parameter();
					}
					else {
						Ddim_Print(("E:Unknown command.\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "result") == 0) {
					if (strcmp((const char *)argv[5], "one") == 0) {
						// Im_MXIC_Get_Access_Or_Trans_Monitor
						// [command] immxic err monitor acc result one
						ct_im_mxic_check_err_access_or_trans_monitor();
					}
					else if (strcmp((const char *)argv[5], "all") == 0) {
						// Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry
						// [command] immxic err monitor acc result all
						ct_im_mxic_check_err_access_or_trans_monitor_all_entry();
					}
					else {
						Ddim_Print(("E:Unknown command.\n"));
					}
				}
				else if (strcmp((const char *)argv[4], "state") == 0) {
					// Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State
					// [command] immxic err monitor acc state
					ct_im_mxic_check_err_sccess_or_trans_monitor_limit_end_state();
				}
				else {
					Ddim_Print(("E:Unknown command.\n"));
				}
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "memacc") == 0) {
			if (strcmp((const char *)argv[3], "start") == 0) {
				// Im_MXIC_Start_Memory_Access_Detect
				// [command] immxic err memacc start
				ct_im_mxic_check_err_memory_access_start();
			}
			else if (strcmp((const char *)argv[3], "set") == 0) {
				// Im_MXIC_Set_Memory_Access_Detect
				// [command] immxic err memacc set
				ct_im_mxic_check_err_memory_access_set();
			}
			else if (strcmp((const char *)argv[3], "get") == 0) {
				// Im_MXIC_Get_Memory_Access_Detect
				// [command] immxic err memacc get
				ct_im_mxic_check_err_memory_access_get();
			}
			else {
				Ddim_Print(("E:Unknown command.\n"));
			}
		}
		else {
			Ddim_Print(("E:Unknown command.\n"));
		}
	}
	else {
		Ddim_Print(("E:Unknown command.\n"));
	}
#ifdef CO_ACT_MXIC_HCLOCK
	if(gIM_MXIC_Hclk_Ctrl_Cnt != 0){
		Ddim_Print(("E:Hclk NG : gIM_MXIC_Hclk_Ctrl_Cnt=%d.\n", gIM_MXIC_Hclk_Ctrl_Cnt));
	}
#endif // CO_ACT_MXIC_HCLOCK
#ifdef CO_ACT_MXIC_PCLOCK
	if(gIM_MXIC_Pclk_Ctrl_Cnt != 0){
		Ddim_Print(("E:Pclk NG : gIM_MXIC_Pclk_Ctrl_Cnt=%d.\n", gIM_MXIC_Pclk_Ctrl_Cnt));
	}
#endif // CO_ACT_MXIC_PCLOCK
	return;
}
