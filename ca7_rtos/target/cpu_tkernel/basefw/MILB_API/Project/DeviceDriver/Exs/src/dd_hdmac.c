/**
 * @file		dd_hdmac.c
 * @brief		HDMAC driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "peripheral.h"
#include "driver_common.h"
#include "ddim_user_custom.h"
#include "dd_hdmac.h"
#include "dd_arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#define D_DD_HDMAC_SYNC				0		/* Sync Processing */
#define D_DD_HDMAC_ASYNC			1		/* Async Processing */

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Nothing Special
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Nothing Special
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
static volatile T_DD_HDMAC_CTRL gDD_HDMAC_Ctrl[D_DD_HDMAC_CH_NUM_MAX];		// HDMAC Register Info
static volatile USHORT gDD_HDMAC_TC_Second[D_DD_HDMAC_CH_NUM_MAX];
static volatile USHORT gDD_HDMAC_Stop_Status[D_DD_HDMAC_CH_NUM_MAX];
static volatile INT32 gDD_HDMAC_Wait_End_Time[D_DD_HDMAC_CH_NUM_MAX];

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
static INT32 dd_hdmac_wait_end(UCHAR ch, USHORT* const status, UINT32 wait_mode);
static INT32 dd_hdmac_start(UCHAR ch, USHORT* const status, UINT32 wait_mode, UINT32 sync);

/**
 * @brief  HDMAC wait end.
 */
static INT32 dd_hdmac_wait_end(UCHAR ch, USHORT* const status, UINT32 wait_mode)
{
	DDIM_USER_FLGPTN flg_ptn;
	DDIM_USER_ER ercd = 0;
	USHORT ss;

	// CPU Polling (no use interrupt)
	if (wait_mode == D_DD_HDMAC_WAITMODE_CPU) {
		while (gDD_HDMAC_Stop_Status[ch] == 0) {
			ss = IO_HDMAC.DMAC[ch].DMACB.bit.SS;
			if(ss) {
				gDD_HDMAC_Stop_Status[ch] = ss;
			}
		}
		
		if (gDD_HDMAC_Stop_Status[ch] == 0) {
			*status = IO_HDMAC.DMAC[ch].DMACB.bit.SS;	// To give the user the content of the SS bit of DMACB register
			IO_HDMAC.DMAC[ch].DMACB.bit.SS = 0x0;		// Stop Status clear
			IO_HDMAC.DMAC[ch].DMACA.bit.EB = 0;			// HDMAC Stop
		}
		else {
			*status = gDD_HDMAC_Stop_Status[ch];
			gDD_HDMAC_Stop_Status[ch] = 0;
		}
	}
	// EVENT (use interrupt)
	else {
		switch (ch) {
			case 0:
				ercd = DDIM_User_Twai_Flg(FID_DD_HDMAC, D_DD_HDMAC_FLG_CH0, D_DDIM_USER_TWF_ORW, &flg_ptn, gDD_HDMAC_Wait_End_Time[ch]);
				break;

			case 1:
				ercd = DDIM_User_Twai_Flg(FID_DD_HDMAC, D_DD_HDMAC_FLG_CH1, D_DDIM_USER_TWF_ORW, &flg_ptn, gDD_HDMAC_Wait_End_Time[ch]);
				break;

			case 2:
				ercd = DDIM_User_Twai_Flg(FID_DD_HDMAC, D_DD_HDMAC_FLG_CH2, D_DDIM_USER_TWF_ORW, &flg_ptn, gDD_HDMAC_Wait_End_Time[ch]);
				break;

			case 3:
				ercd = DDIM_User_Twai_Flg(FID_DD_HDMAC, D_DD_HDMAC_FLG_CH3, D_DDIM_USER_TWF_ORW, &flg_ptn, gDD_HDMAC_Wait_End_Time[ch]);
				break;

			default:
				break;
		}

		*status = gDD_HDMAC_Stop_Status[ch];		// To give the user the content of the SS bit of DMACB register
		gDD_HDMAC_Stop_Status[ch] = 0;
	}

	if (ercd == D_DDIM_USER_E_TMOUT) {
		Ddim_Print(("HDMAC error. timeout!"));
		return D_DD_HDMAC_TIMEOUT_ERR;
	}

	if (ercd != D_DDIM_USER_E_OK) {
		Ddim_Print(("HDMAC error. system error!"));
		return D_DD_HDMAC_SYSTEM_ERR;
	}

	return D_DDIM_OK;
}

/**
 * @brief  HDMAC start.
 */
static INT32 dd_hdmac_start(UCHAR ch, USHORT* const status, UINT32 wait_mode, UINT32 sync)
{
	INT32 ret = D_DDIM_OK;

	// HDMAC Enable
	if (IO_HDMAC.DMACR.bit.DE == 0) {
		IO_HDMAC.DMACR.bit.DE = 1;							// HDMAC all ch enable
	}

	// HDMAC Busy Check
	if (IO_HDMAC.DMAC[ch].DMACA.bit.EB) {
		return D_DD_HDMAC_BUSY_ERR;
	}

	if (wait_mode == D_DD_HDMAC_WAITMODE_EVENT) {
		gDD_HDMAC_Ctrl[ch].config_b.bit.ei = 1;				// Error Interrupt Enable
		gDD_HDMAC_Ctrl[ch].config_b.bit.ci = 1;				// Completion Interrupt Enable
	}

	gDD_HDMAC_Stop_Status[ch] = 0;
	IO_HDMAC.DMAC[ch].DMACA.word = gDD_HDMAC_Ctrl[ch].config_a.word;	// HDMAC Config A register set
	IO_HDMAC.DMAC[ch].DMACB.word = gDD_HDMAC_Ctrl[ch].config_b.word;	// HDMAC Config B register set
	IO_HDMAC.DMAC[ch].DMACSA     = gDD_HDMAC_Ctrl[ch].src_addr;			// HDMAC Source Address register set
	IO_HDMAC.DMAC[ch].DMACDA     = gDD_HDMAC_Ctrl[ch].dst_addr;			// HDMAC Destination Address register set

	if (wait_mode == D_DD_HDMAC_WAITMODE_EVENT) {
		switch (ch) {
			case 0:
				DDIM_User_Clr_Flg(FID_DD_HDMAC, ~D_DD_HDMAC_FLG_CH0);
				break;

			case 1:
				DDIM_User_Clr_Flg(FID_DD_HDMAC, ~D_DD_HDMAC_FLG_CH1);
				break;

			case 2:
				DDIM_User_Clr_Flg(FID_DD_HDMAC, ~D_DD_HDMAC_FLG_CH2);
				break;

			case 3:
				DDIM_User_Clr_Flg(FID_DD_HDMAC, ~D_DD_HDMAC_FLG_CH3);
				break;

			default:
				break;
		}
	}

	IO_HDMAC.DMAC[ch].DMACA.bit.EB = 1;			// HDMAC Start
	if (gDD_HDMAC_Ctrl[ch].config_a.bit.is == D_DD_HDMAC_IS_SOFT) {
		IO_HDMAC.DMAC[ch].DMACA.bit.ST = 1;		// Soft trigger
	}
	Dd_ARM_Dsb_Pou();

	// Sync Processing
	if (sync == D_DD_HDMAC_SYNC) {
		ret = dd_hdmac_wait_end(ch, status, wait_mode);
	}

	return ret;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/**
 * @brief  Execute exclusive control for designated HDMAC channel.
 * @param  UCHAR	ch	Channel number (0 to 3)
 * @return INT32 	D_DDIM_OK / D_DD_HDMAC_EXC_LOCK_NG / D_DD_HDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_HDMAC_Open(UCHAR ch, INT32 tmout)
{
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	if (D_DDIM_USER_SEM_WAIT_POL == tmout) {
		ercd = DDIM_User_Pol_Sem(SID_DD_HDMAC(ch));				// pol_sem()
	}
	else {
		ercd = DDIM_User_Twai_Sem(SID_DD_HDMAC(ch), (DDIM_USER_TMO)tmout);	// twai_sem()
	}

	if (D_DDIM_USER_E_OK != ercd) {
		if (D_DDIM_USER_E_TMOUT == ercd) {
			return D_DD_HDMAC_SEM_TIMEOUT;
		}

		return D_DD_HDMAC_SEM_NG;
	}

	return D_DDIM_OK;
}

/**
 * @brief  Cancel exclusive control for designated HDMAC channel.
           When the designated channel is under transferring, stop transfer immediately and cancel lock.
 * @param  UCHAR	ch			Channel number (0 to 3)
 * @return INT32  D_DDIM_OK/D_DD_HDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_HDMAC_Close(UCHAR ch)
{
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	// Global variable is clear
	gDD_HDMAC_Ctrl[ch].config_a.word = 0;
	gDD_HDMAC_Ctrl[ch].config_b.word = 0;
	gDD_HDMAC_Ctrl[ch].src_addr      = 0;
	gDD_HDMAC_Ctrl[ch].dst_addr      = 0;
	gDD_HDMAC_Ctrl[ch].int_handler   = NULL;

	ercd = DDIM_User_Sig_Sem(SID_DD_HDMAC(ch));				// sig_sem()
	if( D_DDIM_USER_E_OK != ercd ) {
		return D_DD_HDMAC_SEM_NG;
	}

	return D_DDIM_OK;
}

/**
 * @brief  Set control data to registers related HDMAC designated channel.
 * @param  UCHAR	ch	Channel number (0 to 3)
 * @return INT32	D_DDIM_OK / D_DD_HDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_HDMAC_Ctrl_Common(UCHAR ch, T_DD_HDMAC_CTRL const *const hdmac_ctrl)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if (hdmac_ctrl == NULL) {
		Ddim_Assertion(("HDMAC: input param error. [*hdmac_ctrl] NULL\n"));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	// HDMAC Busy Check
//	if (IO_HDMAC.DMAC[ch].CSTR.bit.BUSY) {
//		return D_DD_HDMAC_BUSY_ERR;
//	}

	// The value is set to an external variable
	gDD_HDMAC_Ctrl[ch].config_a.word = (hdmac_ctrl->config_a.word & 0x1FFFFFFF);
	gDD_HDMAC_Ctrl[ch].config_b.word = (hdmac_ctrl->config_b.word & 0xFFFFFF00);
	gDD_HDMAC_Ctrl[ch].src_addr      = hdmac_ctrl->src_addr;
	gDD_HDMAC_Ctrl[ch].dst_addr      = hdmac_ctrl->dst_addr;
	gDD_HDMAC_Ctrl[ch].int_handler   = hdmac_ctrl->int_handler;

	gDD_HDMAC_TC_Second[ch] = 0;
	gDD_HDMAC_Wait_End_Time[ch] = D_DDIM_WAIT_END_TIME;

	return D_DDIM_OK;
}

/**
 * @brief  Set control data to registers related HDMAC designated channel.
 * @param  UCHAR	ch	Channel number (0 to 3)
 * @return INT32	D_DDIM_OK / D_DD_HDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_HDMAC_Ctrl_Trns(UCHAR ch, T_DD_HDMAC_CTRL_TRNS const *const hdmac_ctrl_trans)
{
	UCHAR  tmp_width;
	ULONG  tmp_tc = 0;
	USHORT tmp_bc = 0;
	UCHAR  tmp_bt = 0;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if (hdmac_ctrl_trans == NULL) {
		Ddim_Assertion(("HDMAC: input param error. [*hdmac_ctrl_trans] NULL\n"));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	// HDMAC Busy Check
//	if (IO_HDMAC.DMAC[ch].CSTR.bit.BUSY) {
//		return D_DD_HDMAC_BUSY_ERR;
//	}

	// Calucurate TC and BC
	tmp_width = 0x01 << hdmac_ctrl_trans->size.trns_width;
	if ((hdmac_ctrl_trans->size.trns_size % tmp_width) != 0) {
		Ddim_Print(("HDMAC: input param error. illegal alignment.\n"));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}

	switch (hdmac_ctrl_trans->mode.beat_type) {
		case D_DD_HDMAC_BT_WRAP4:
		case D_DD_HDMAC_BT_INCR4:
			tmp_bt = 4;
			break;
		case D_DD_HDMAC_BT_WRAP8:
		case D_DD_HDMAC_BT_INCR8:
			tmp_bt = 8;
			break;
		case D_DD_HDMAC_BT_WRAP16:
		case D_DD_HDMAC_BT_INCR16:
		case D_DD_HDMAC_BT_INCR:
			tmp_bt = 16;
			break;
		default:
			tmp_bt = 16;
			break;
	}
	if ((hdmac_ctrl_trans->mode.mode_sel != D_DD_HDMAC_MS_DEMAND) &&
		(hdmac_ctrl_trans->size.trns_size > (tmp_width * tmp_bt))) {

		tmp_bc = tmp_bt - 1;
		tmp_tc = (hdmac_ctrl_trans->size.trns_size / (tmp_width * tmp_bt)) - 1;

		if ((hdmac_ctrl_trans->size.trns_size % (tmp_width * tmp_bt)) != 0) {
			gDD_HDMAC_TC_Second[ch] = ((hdmac_ctrl_trans->size.trns_size % (tmp_width * tmp_bt)) / tmp_width);
		}
		else {
			gDD_HDMAC_TC_Second[ch] = 0;
		}
	}
	else {
		tmp_bc = 0;
		tmp_tc = (hdmac_ctrl_trans->size.trns_size / tmp_width) - 1;
		gDD_HDMAC_TC_Second[ch] = 0;
	}

	// Check TransferSize
	if ((tmp_tc >= 65536) || ((tmp_tc == 65535) && (gDD_HDMAC_TC_Second[ch] != 0 ))) {
		// Max size over
		Ddim_Print(("HDMAC: input param error. Max size over (Max size=%d).\n", tmp_width * tmp_bt * 65536));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}

	// The value is set to an external variable
	gDD_HDMAC_Ctrl[ch].config_a.bit.is = hdmac_ctrl_trans->mode.input_sel;
	gDD_HDMAC_Ctrl[ch].config_a.bit.bt = hdmac_ctrl_trans->mode.beat_type;

	gDD_HDMAC_Ctrl[ch].config_b.bit.ms = hdmac_ctrl_trans->mode.mode_sel;
	gDD_HDMAC_Ctrl[ch].config_b.bit.fs = hdmac_ctrl_trans->mode.src_fix;
	gDD_HDMAC_Ctrl[ch].config_b.bit.fd = hdmac_ctrl_trans->mode.dst_fix;

	gDD_HDMAC_Ctrl[ch].config_b.bit.tw = hdmac_ctrl_trans->size.trns_width;

	gDD_HDMAC_Ctrl[ch].config_a.bit.bc = tmp_bc;
	gDD_HDMAC_Ctrl[ch].config_a.bit.tc = (USHORT)tmp_tc;

	gDD_HDMAC_Ctrl[ch].config_b.bit.dp = 0x08;		// HPROT[3] Cachable (Source Protection)
	gDD_HDMAC_Ctrl[ch].config_b.bit.sp = 0x08;		// HPROT[3] Cachable (Destination Protection)

	gDD_HDMAC_Ctrl[ch].src_addr        = hdmac_ctrl_trans->size.src_addr;
	gDD_HDMAC_Ctrl[ch].dst_addr        = hdmac_ctrl_trans->size.dst_addr;
	gDD_HDMAC_Ctrl[ch].int_handler     = hdmac_ctrl_trans->int_handler;

	gDD_HDMAC_Wait_End_Time[ch] = D_DDIM_WAIT_END_TIME;

	return D_DDIM_OK;
}

INT32 Dd_HDMAC_Set_Trns_Size(UCHAR ch, T_DD_HDMAC_TRNS_SIZE const *const hdmac_trns_size)
{
	UCHAR  tmp_width;
	ULONG  tmp_tc = 0;
	USHORT tmp_bc = 0;
	UCHAR  tmp_bt = 0;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if (hdmac_trns_size == NULL) {
		Ddim_Assertion(("HDMAC: input param error. [*hdmac_trns_size] NULL\n"));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	// HDMAC Busy Check
//	if (IO_HDMAC.DMAC[ch].CSTR.bit.BUSY) {
//		return D_DD_HDMAC_BUSY_ERR;
//	}

	// Calucurate TC and BC
	tmp_width = 0x01 << hdmac_trns_size->trns_width;
	if ((hdmac_trns_size->trns_size % tmp_width) != 0) {
		Ddim_Print(("HDMAC: input param error. illegal alignment.\n"));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}

	switch (gDD_HDMAC_Ctrl[ch].config_a.bit.bt) {
		case D_DD_HDMAC_BT_WRAP4:
		case D_DD_HDMAC_BT_INCR4:
			tmp_bt = 4;
			break;
		case D_DD_HDMAC_BT_WRAP8:
		case D_DD_HDMAC_BT_INCR8:
			tmp_bt = 8;
			break;
		case D_DD_HDMAC_BT_WRAP16:
		case D_DD_HDMAC_BT_INCR16:
		case D_DD_HDMAC_BT_INCR:
			tmp_bt = 16;
			break;
		default:
			tmp_bt = 16;
			break;
	}
	if ((gDD_HDMAC_Ctrl[ch].config_b.bit.ms != D_DD_HDMAC_MS_DEMAND) &&
		(hdmac_trns_size->trns_size > (tmp_width * tmp_bt))) {
		tmp_bc = tmp_bt - 1;
		tmp_tc = (hdmac_trns_size->trns_size / (tmp_width * tmp_bt)) - 1;

		if ((hdmac_trns_size->trns_size % (tmp_width * tmp_bt)) != 0) {
			gDD_HDMAC_TC_Second[ch] = ((hdmac_trns_size->trns_size % (tmp_width * tmp_bt)) / tmp_width);
		}
		else {
			gDD_HDMAC_TC_Second[ch] = 0;
		}
	}
	else {
		tmp_bc = 0;
		tmp_tc = (hdmac_trns_size->trns_size / tmp_width) - 1;
		gDD_HDMAC_TC_Second[ch] = 0;
	}

	// Check TransferSize
	if ((tmp_tc >= 65536) || ((tmp_tc == 65535) && (gDD_HDMAC_TC_Second[ch] != 0 ))) {
		// Max size over
		Ddim_Print(("HDMAC: input param error. Max size over (Max size=%d).\n", tmp_width * tmp_bt * 65536));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}

	// The value is set to an external variable
	gDD_HDMAC_Ctrl[ch].config_b.bit.tw = hdmac_trns_size->trns_width;

	gDD_HDMAC_Ctrl[ch].config_a.bit.bc = tmp_bc;
	gDD_HDMAC_Ctrl[ch].config_a.bit.tc = (USHORT)tmp_tc;

	gDD_HDMAC_Ctrl[ch].src_addr        = hdmac_trns_size->src_addr;
	gDD_HDMAC_Ctrl[ch].dst_addr        = hdmac_trns_size->dst_addr;

	return D_DDIM_OK;
}

/**
 * @brief  The operation of HDMAC of specified ch begins.
 * @param  UCHAR				ch				Channel number (0 to 3)
 *         T_DD_HDMAC_START*	hdmac_start		HDMAC Start table
 *         USHORT*			status		Channel Status Register pointer
 *         UINT32			wait_mode	HDMAC end wait mode
 * @return INT32			D_DDIM_OK / D_DD_HDMAC_INPUT_PARAM_ERR / D_DDIM_TIMEOUT / D_DD_HDMAC_SYSTEM_ERR
 */
INT32 Dd_HDMAC_Start_Sync(UCHAR ch, USHORT *const status, UINT32 wait_mode)
{
	INT32 ret;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if (status == NULL) {
		Ddim_Assertion(("HDMAC: input param error. [*status] NULL\n"));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if ((wait_mode != D_DD_HDMAC_WAITMODE_CPU) && (wait_mode != D_DD_HDMAC_WAITMODE_EVENT)) {
		Ddim_Assertion(("HDMAC: input param error. [wait_mode] = %x\n", wait_mode));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	ret = dd_hdmac_start(ch, status, wait_mode, D_DD_HDMAC_SYNC);		// HDMAC Start

	if ((*status == D_DD_HDMAC_SS_NORMAL_END) && (gDD_HDMAC_TC_Second[ch] != 0)) {
		// restart
		gDD_HDMAC_Ctrl[ch].config_a.bit.bc = 0;
		gDD_HDMAC_Ctrl[ch].config_a.bit.tc = gDD_HDMAC_TC_Second[ch] - 1;

		gDD_HDMAC_Ctrl[ch].src_addr        = IO_HDMAC.DMAC[ch].DMACSA;
		gDD_HDMAC_Ctrl[ch].dst_addr        = IO_HDMAC.DMAC[ch].DMACDA;

		gDD_HDMAC_TC_Second[ch] = 0;
		dd_hdmac_start(ch, status, wait_mode, D_DD_HDMAC_SYNC);	// HDMAC Start
	}

	return ret;
}

/**
 * @brief  The operation of HDMAC of specified ch begins.
 * @param  UCHAR				ch				Channel number (0 to 3)
 *         T_DD_HDMAC_START*	hdmac_start		HDMAC Start table
 * @return INT32			D_DDIM_OK / D_DD_HDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_HDMAC_Start_Async(UCHAR ch)
{
	USHORT	status;	// dummy

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	dd_hdmac_start(ch, &status, D_DD_HDMAC_WAITMODE_EVENT, D_DD_HDMAC_ASYNC);	// HDMAC Start
	return D_DDIM_OK;
}

/**
 * @brief  The operation of HDMAC of specified ch is stopped.
 * @param  UCHAR	ch			Channel number (0 to 3)
 * @return INT32	D_DDIM_OK / D_DD_HDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_HDMAC_Stop(UCHAR ch)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	IO_HDMAC.DMAC[ch].DMACA.word &= 0x7FFFFFFF;
	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}

/**
 * @brief  The operation of HDMAC of specified ch is stopped.
 * @param  UCHAR	ch			Channel number (0 to 3)
 * @return INT32	D_DDIM_OK / D_DD_HDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_HDMAC_Pause(UCHAR ch)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	IO_HDMAC.DMAC[ch].DMACA.bit.PB = 1;
	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}

/**
 * @brief  The operation of HDMAC of specified ch is resumed.
 * @param  UCHAR	ch			Channel number (0 to 3)
 * @return INT32	D_DDIM_OK / D_DD_HDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_HDMAC_Resume(UCHAR ch)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	switch (ch) {
		case 0:
			DDIM_User_Clr_Flg(FID_DD_HDMAC, ~D_DD_HDMAC_FLG_CH0);
			break;

		case 1:
			DDIM_User_Clr_Flg(FID_DD_HDMAC, ~D_DD_HDMAC_FLG_CH1);
			break;

		case 2:
			DDIM_User_Clr_Flg(FID_DD_HDMAC, ~D_DD_HDMAC_FLG_CH2);
			break;

		case 3:
			DDIM_User_Clr_Flg(FID_DD_HDMAC, ~D_DD_HDMAC_FLG_CH3);
			break;

		default:
			break;
	}

	IO_HDMAC.DMAC[ch].DMACA.bit.PB = 0;
	IO_HDMAC.DMAC[ch].DMACA.bit.EB = 1;			// HDMAC Start
	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}

/**
 * @brief  Wait end time of transfer process of designated channel.
 * @param  UCHAR	ch			Channel number (0 to 3)
 *         INT32	wait_time	Wait end time of transfer process
 * @return INT32				D_DDIM_OK / D_DD_HDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_HDMAC_Set_Wait_Time(UCHAR ch, INT32 wait_time)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if (wait_time < D_DDIM_WAIT_END_FOREVER) {
		Ddim_Assertion(("HDMAC: input param error. [wait_time] = %d\n", wait_time));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	gDD_HDMAC_Wait_End_Time[ch] = wait_time;
	return D_DDIM_OK;
}

/**
 * @brief  Wait end of transfer process of designated channel.
 *         The value of the CSTR register is passed by the out parameter.
 * @param  UCHAR	ch			Channel number (0 to 3)
 *         USHORT*	status		Channel Status Register pointer
 * @return INT32	D_DDIM_OK / D_DD_HDMAC_INPUT_PARAM_ERR / D_DDIM_TIMEOUT / D_DD_HDMAC_SYSTEM_ERR
 */
INT32 Dd_HDMAC_Wait_End(UCHAR ch, USHORT* const status, UINT32 wait_mode)
{
	INT32 ret;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if (status == NULL) {
		Ddim_Assertion(("HDMAC: input param error. [*status] NULL\n"));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if ((wait_mode != D_DD_HDMAC_WAITMODE_CPU) && (wait_mode != D_DD_HDMAC_WAITMODE_EVENT)) {
		Ddim_Assertion(("HDMAC: input param error. [wait_mode] = %x\n", wait_mode));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	ret = dd_hdmac_wait_end(ch, status, wait_mode);

	if ((*status == D_DD_HDMAC_SS_NORMAL_END) && (gDD_HDMAC_TC_Second[ch] != 0)) {
		// restart
		gDD_HDMAC_Ctrl[ch].config_a.bit.bc = 0;
		gDD_HDMAC_Ctrl[ch].config_a.bit.tc = gDD_HDMAC_TC_Second[ch] - 1;

		gDD_HDMAC_Ctrl[ch].src_addr        = IO_HDMAC.DMAC[ch].DMACSA;
		gDD_HDMAC_Ctrl[ch].dst_addr        = IO_HDMAC.DMAC[ch].DMACDA;

		gDD_HDMAC_TC_Second[ch] = 0;
		dd_hdmac_start(ch, status, D_DD_HDMAC_WAITMODE_EVENT, D_DD_HDMAC_ASYNC);	// HDMAC Start
		ret = dd_hdmac_wait_end(ch, status, wait_mode);
	}

	return ret;
}

/**
 * @brief  The status of HDMAC of specified ch is cleared.
 * @param  UCHAR	ch			Channel number (0 to 3)
 * @return INT32				D_DDIM_OK / D_DD_HDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_HDMAC_Clear_Status(UCHAR ch)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	IO_HDMAC.DMAC[ch].DMACB.bit.SS = 0;

	return D_DDIM_OK;
}

/**
 * @brief  The error status of HDMAC of specified ch is cleared.
 * @param  UCHAR	ch			Channel number (0 to 3)
 * @return INT32				D_DDIM_OK / D_DD_HDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_HDMAC_Get_Status(UCHAR ch, USHORT *const status)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if (status == NULL) {
		Ddim_Assertion(("HDMAC: input param error. [*status] NULL\n"));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	*status = IO_HDMAC.DMAC[ch].DMACB.bit.SS;
	return D_DDIM_OK;
}

/**
The value of the transferd size of the specified channel is acquired.
@param [in] ch				Channel number (0 to 3)
@retval size						transferd size of the specified channel
@remarks This API forced cancel exclusive control if process is under executing.
*/
ULONG Dd_HDMAC_Get_Trns_Size(UCHAR ch)
{
	ULONG  total_size;
	ULONG  remain_size;
	USHORT tc;
	UCHAR  bc;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return 0;
	}
#endif

	tc = IO_HDMAC.DMAC[ch].DMACA.bit.TC;
	bc = IO_HDMAC.DMAC[ch].DMACA.bit.BC;
	remain_size = (bc + 1) * (tc + 1) * (1 << gDD_HDMAC_Ctrl[ch].config_b.bit.tw);

	if ((tc == 0) && (bc == 0)) {
		if (((IO_HDMAC.DMAC[ch].DMACB.word & 0x00070000) != 0x00000000) || (gDD_HDMAC_Stop_Status[ch] != 0)) {
			remain_size = 0;
		}
	}

	if (gDD_HDMAC_TC_Second[ch] != 0) {
		remain_size += (gDD_HDMAC_TC_Second[ch]) * (1 << gDD_HDMAC_Ctrl[ch].config_b.bit.tw);
	}

	total_size = (gDD_HDMAC_Ctrl[ch].config_a.bit.bc + 1) * (gDD_HDMAC_Ctrl[ch].config_a.bit.tc + 1) * (1 << gDD_HDMAC_Ctrl[ch].config_b.bit.tw);
	if (gDD_HDMAC_TC_Second[ch] != 0) {
		total_size += (gDD_HDMAC_TC_Second[ch]) * (1 << gDD_HDMAC_Ctrl[ch].config_b.bit.tw);
	}

	return (total_size - remain_size);
}

/**
The value of the remainder transfer size of the specified channel is acquired.

@param [in] ch				Channel number (0 to 3)
@retval size						remainder transfer size of the specified channel
@remarks This API forced cancel exclusive control if process is under executing.
*/
ULONG Dd_HDMAC_Get_Remain_Trns_Size(UCHAR ch)
{
	ULONG  remain_size;
	USHORT tc;
	UCHAR  bc;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return 0;
	}
#endif

	tc = IO_HDMAC.DMAC[ch].DMACA.bit.TC;
	bc = IO_HDMAC.DMAC[ch].DMACA.bit.BC;
	remain_size = (bc + 1) * (tc + 1) * (1 << gDD_HDMAC_Ctrl[ch].config_b.bit.tw);

	if ((tc == 0) && (bc == 0)) {
		if (((IO_HDMAC.DMAC[ch].DMACB.word & 0x00070000) != 0x00000000) || (gDD_HDMAC_Stop_Status[ch] != 0)) {
			remain_size = 0;
		}
	}

	if (gDD_HDMAC_TC_Second[ch] != 0) {
		remain_size += (gDD_HDMAC_TC_Second[ch]) * (1 << gDD_HDMAC_Ctrl[ch].config_b.bit.tw);
	}

	return remain_size;
}

/**
The value of the total transfer size of the specified channel is acquired.

@param [in] ch				Channel number (0 to 3)
@retval size						total transfer size of the specified channel
*/
ULONG Dd_HDMAC_Get_Total_Trns_Size(UCHAR ch)
{
	ULONG total_size;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return 0;
	}
#endif

	total_size = (gDD_HDMAC_Ctrl[ch].config_a.bit.bc + 1) * (gDD_HDMAC_Ctrl[ch].config_a.bit.tc + 1) * (1 << gDD_HDMAC_Ctrl[ch].config_b.bit.tw);
	if (gDD_HDMAC_TC_Second[ch] != 0) {
		total_size += (gDD_HDMAC_TC_Second[ch]) * (1 << gDD_HDMAC_Ctrl[ch].config_b.bit.tw);
	}

	return total_size;
}

/**
The value of the source address (DMACSA) of the specified channel is acquired.

@param [in] ch				Channel number (0 to 3)
@retval src_addr					source address of the specified channel
*/
ULONG Dd_HDMAC_Get_Src_Addr(UCHAR ch)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return 0;
	}
#endif

	return IO_HDMAC.DMAC[ch].DMACSA;
}

/**
The value of the destination address (DMACDA) of the specified channel is acquired.

@param [in] ch				Channel number (0 to 3)
@retval dst_addr					Destination address of the specified channel
*/
ULONG Dd_HDMAC_Get_Dst_Addr(UCHAR ch)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return 0;
	}
#endif

	return IO_HDMAC.DMAC[ch].DMACDA;
}

/**
Source Protection code is set.

@param [in] ch					Channel number (0 to 3)
@param [in] protect_code		Protection Code
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC_INPUT_PARAM_ERR	Input Parameter Error
*/
INT32 Dd_HDMAC_Set_Source_Protect(UCHAR ch, UCHAR protect_code)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	IO_HDMAC.DMAC[ch].DMACB.bit.SP = protect_code;
	return D_DDIM_OK;
}

/**
Destination Protection code is set.

@param [in] ch					Channel number (0 to 3)
@param [in] protect_code		Protection Code
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC_INPUT_PARAM_ERR	Input Parameter Error
*/
INT32 Dd_HDMAC_Set_Destination_Protect(UCHAR ch, UCHAR protect_code)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	IO_HDMAC.DMAC[ch].DMACB.bit.DP = protect_code;
	return D_DDIM_OK;
}

/**
 * @brief  Arbitration level and HDMAC forwarding control interrupt level of all channels are set.
 * @param  T_DD_HDMAC_ARBITRATION*	arbitration
 * @return INT32				D_DDIM_OK / D_DD_HDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_HDMAC_Set_Arbitration(UCHAR arbitration)
{
#ifdef CO_PARAM_CHECK
	if ((arbitration != D_DD_HDMAC_ARB_FIX) && (arbitration != D_DD_HDMAC_ARB_ROTATE)) {
		Ddim_Assertion(("HDMAC: input param error. [arbitration] = %x\n", arbitration));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	// Arbitration type is set
	IO_HDMAC.DMACR.bit.PR = arbitration;

	return D_DDIM_OK;
}

/**
 * @brief  Arbitration level and HDMAC forwarding control interrupt level of all channels are read.
 * @param  T_DD_HDMAC_ARBITRATION*	arbitration
 * @return INT32				D_DDIM_OK / D_DD_HDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_HDMAC_Get_Arbitration(UCHAR *const arbitration)
{
#ifdef CO_PARAM_CHECK
	if (arbitration == NULL) {
		Ddim_Assertion(("HDMAC: input param error. [*arbitration] NULL\n"));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	// Arbitration type is set
	*arbitration = IO_HDMAC.DMACR.bit.PR;

	return D_DDIM_OK;
}

/**
 * @brief  Arbitration level and HDMAC forwarding control interrupt level of all channels are returned to an initial value.
 * @param  VOID
 * @return VOID
 */
VOID Dd_HDMAC_Clear_Arbitration(VOID)
{
	// Arbitration type is set
	IO_HDMAC.DMACR.bit.PR = 0;
}

/**
The operation of All HDMAC channel is stopped.
*/
VOID Dd_HDMAC_Stop_All_Ch(VOID)
{
	// HDMAC Halt
	IO_HDMAC.DMACR.bit.DH = 1;
}

/**
The operation of All HDMAC channel is resumed.
*/
VOID Dd_HDMAC_Resume_All_Ch(VOID)
{
	// HDMAC resume
	IO_HDMAC.DMACR.bit.DH = 0;
}

/**
It is API that returns the value set to the register of TW. 

@param [in] src_addr			soruce address
@param [in] dst_addr			destination address
@param [in] total_size			total size
@retval TW value set to register of TW
*/
UCHAR Dd_HDMAC_Get_Trns_Width(ULONG src_addr, ULONG dst_addr, ULONG total_size)
{
	UCHAR trns_width;

	// Check transfer size
	if (((src_addr & 0x03) == 0) && ((dst_addr & 0x03) == 0) && ((total_size & 0x03) == 0)) {
		trns_width = D_DD_HDMAC_TW_WORD;
	}
	else if (((src_addr & 0x01) == 0) && ((dst_addr & 0x01) == 0) && ((total_size & 0x01) == 0)) {
		trns_width = D_DD_HDMAC_TW_HWORD;
	}
	else {
		trns_width = D_DD_HDMAC_TW_BYTE;
	}

	return trns_width;
}

/**
 * @brief  Set HDMAC Interrupt Handler
 * @param  HDMAC channel, interrupt handler
 * @return VOID
 */
VOID Dd_HDMAC_Set_Int_Handler(UCHAR ch, VOID (*int_handler)(VOID))
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return;
	}
#endif

	gDD_HDMAC_Ctrl[ch].int_handler = int_handler;
}

/**
 * @brief  Interrupt handler of HDMAC channel 0 for transfer process is finished.
 * @param  VOID
 * @return VOID
 */
VOID Dd_HDMAC_Int_Handler(UCHAR ch)
{
	DDIM_USER_ER ercd;
	USHORT status;
	VP_CALLBACK handler;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return;
	}
#endif

	status = IO_HDMAC.DMAC[ch].DMACB.bit.SS;		// To give the user the content of the SS of DMACB register
	gDD_HDMAC_Stop_Status[ch] = status;
	IO_HDMAC.DMAC[ch].DMACB.bit.SS = 0x0;			// Stop Status clear
	IO_HDMAC.DMAC[ch].DMACA.bit.EB = 0;				// HDMAC Stop
	Dd_ARM_Dsb_Pou();

	if ((status != D_DD_HDMAC_SS_NORMAL_END) || (gDD_HDMAC_TC_Second[ch] == 0)) {
		handler = gDD_HDMAC_Ctrl[ch].int_handler;
		
		if (handler != NULL) {
			(*handler)(ch, &status);
		}
		
		switch (ch) {
			case 0:
				ercd = DDIM_User_Set_Flg(FID_DD_HDMAC, D_DD_HDMAC_FLG_CH0);
				break;

			case 1:
				ercd = DDIM_User_Set_Flg(FID_DD_HDMAC, D_DD_HDMAC_FLG_CH1);
				break;

			case 2:
				ercd = DDIM_User_Set_Flg(FID_DD_HDMAC, D_DD_HDMAC_FLG_CH2);
				break;

			case 3:
				ercd = DDIM_User_Set_Flg(FID_DD_HDMAC, D_DD_HDMAC_FLG_CH3);
				break;

			default:
				ercd = -1;
				break;
		}

		if (ercd != D_DDIM_USER_E_OK) {
			Ddim_Print(("I:DDIM_User_Set_Flg error. ercd = %d\n", ercd));
		}
	}
	else if (gDD_HDMAC_TC_Second[ch] != 0) {
		// restart
		gDD_HDMAC_Ctrl[ch].config_a.bit.bc = 0;
		gDD_HDMAC_Ctrl[ch].config_a.bit.bt = 0;
		gDD_HDMAC_Ctrl[ch].config_a.bit.tc = gDD_HDMAC_TC_Second[ch] - 1;

		gDD_HDMAC_Ctrl[ch].src_addr        = IO_HDMAC.DMAC[ch].DMACSA;
		gDD_HDMAC_Ctrl[ch].dst_addr        = IO_HDMAC.DMAC[ch].DMACDA;

		gDD_HDMAC_TC_Second[ch] = 0;
		dd_hdmac_start(ch, &status, D_DD_HDMAC_WAITMODE_EVENT, D_DD_HDMAC_ASYNC);	// HDMAC Start
	}
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/**
 * @brief  HDMAC Copy Sync for SDRAM
 * @param  UCHAR		ch				Channel number (0 to 3)
 * @param  ULONG		src_addr		source address
 * @param  ULONG		dst_addr		destination address
 * @param  ULONG		size			Copy size
 * @param  INT32		wait_mode		HDMAC end wait mode
 * @return INT32		D_DDIM_OK
 */
INT32 Dd_HDMAC_Copy_SDRAM_Sync(UCHAR ch, ULONG src_addr, ULONG dst_addr, ULONG size, UINT32 wait_mode)
{
	INT32                 ret;
	USHORT                status = 0;
	T_DD_HDMAC_CTRL_TRNS hdmac_ctrl_trns;
	UCHAR tmp_width;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if ((src_addr == 0) || (dst_addr == 0)) {
		Ddim_Assertion(("HDMAC: input param error. [addr] = 0\n"));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if (size == 0) {
		Ddim_Assertion(("HDMAC: input param error. [size] = 0\n"));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if ((wait_mode != D_DD_HDMAC_WAITMODE_CPU) && (wait_mode != D_DD_HDMAC_WAITMODE_EVENT)) {
		Ddim_Assertion(("HDMAC: input param error. [wait_mode] = %x\n", wait_mode));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	// Check transfer size
	if (((src_addr & 0x03) == 0) && ((dst_addr & 0x03) == 0) && ((size & 0x03) == 0)) {
		hdmac_ctrl_trns.size.trns_width = D_DD_HDMAC_TW_WORD;
	}
	else if (((src_addr & 0x01) == 0) && ((dst_addr & 0x01) == 0) && ((size & 0x01) == 0)) {
		hdmac_ctrl_trns.size.trns_width = D_DD_HDMAC_TW_HWORD;
	}
	else {
		hdmac_ctrl_trns.size.trns_width = D_DD_HDMAC_TW_BYTE;
	}

	// Parameter Setting
	hdmac_ctrl_trns.mode.input_sel = D_DD_HDMAC_IS_SOFT;
	hdmac_ctrl_trns.mode.mode_sel  = D_DD_HDMAC_MS_BLK;
	hdmac_ctrl_trns.mode.src_fix   = D_DD_HDMAC_FS_INCR;
	hdmac_ctrl_trns.mode.dst_fix   = D_DD_HDMAC_FD_INCR;
	tmp_width = 0x01 << hdmac_ctrl_trns.size.trns_width;
	if (size > (tmp_width * 16)) {
		hdmac_ctrl_trns.mode.beat_type = D_DD_HDMAC_BT_INCR16;
	}
	else {
		hdmac_ctrl_trns.mode.beat_type  = D_DD_HDMAC_BT_NORMAL;
	}

	hdmac_ctrl_trns.size.trns_size = size;
	hdmac_ctrl_trns.size.src_addr  = src_addr;
	hdmac_ctrl_trns.size.dst_addr  = dst_addr;

	hdmac_ctrl_trns.int_handler = NULL;

	ret = Dd_HDMAC_Open(ch, D_DDIM_WAIT_END_TIME);
	if (ret != D_DDIM_OK) {
		Ddim_Print(("HDMAC Util Copy Sync. Open ERR : ret=0x%x\n", ret));
		return ret;
	}
	while (1) {
		ret = Dd_HDMAC_Ctrl_Trns(ch, &hdmac_ctrl_trns);
		if (ret != D_DDIM_OK) {
			Ddim_Print(("HDMAC Util Copy Sync. Ctrl ERR : ret=0x%x\n", ret));
			break;
		}
		ret = Dd_HDMAC_Start_Sync(ch, &status, wait_mode);
		if (ret != D_DDIM_OK) {
			Ddim_Print(("HDMAC Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
		}
		break;
	}

	ret = Dd_HDMAC_Close(ch);
	if (ret != D_DDIM_OK) {
		Ddim_Print(("HDMAC Util Copy Sync. Close ERR : ret=0x%x\n", ret));
	}

	return ret;
}

/**
 * @brief  HDMAC Copy Async SDRAM
 * @param  UCHAR		ch				Channel number (0 to 3)
 * @param  ULONG		src_addr		source address
 * @param  ULONG		dst_addr		destination address
 * @param  ULONG		size			Copy size
 * @param  VP_CALLBACK	intHandler		Callback function pointer
 * @return INT32  		D_DDIM_OK
 */
INT32 Dd_HDMAC_Copy_SDRAM_Async(UCHAR ch, ULONG src_addr, ULONG dst_addr, ULONG size, VP_CALLBACK intHandler)
{
	INT32                 ret;
	T_DD_HDMAC_CTRL_TRNS hdmac_ctrl_trns;
	UCHAR tmp_width;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_HDMAC_CH_NUM_MAX) {
		Ddim_Assertion(("HDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if ((src_addr == 0) || (dst_addr == 0)) {
		Ddim_Assertion(("HDMAC: input param error. [addr] = 0\n"));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
	if (size == 0) {
		Ddim_Assertion(("HDMAC: input param error. [size] = 0\n"));
		return D_DD_HDMAC_INPUT_PARAM_ERR;
	}
#endif

	// Check transfer size
	if (((src_addr & 0x03) == 0) && ((dst_addr & 0x03) == 0) && ((size & 0x03) == 0)) {
		hdmac_ctrl_trns.size.trns_width = D_DD_HDMAC_TW_WORD;
	}
	else if (((src_addr & 0x01) == 0) && ((dst_addr & 0x01) == 0) && ((size & 0x01) == 0)) {
		hdmac_ctrl_trns.size.trns_width = D_DD_HDMAC_TW_HWORD;
	}
	else {
		hdmac_ctrl_trns.size.trns_width = D_DD_HDMAC_TW_BYTE;
	}

	// Parameter Setting
	hdmac_ctrl_trns.mode.input_sel = D_DD_HDMAC_IS_SOFT;
	hdmac_ctrl_trns.mode.mode_sel  = D_DD_HDMAC_MS_BLK;
	hdmac_ctrl_trns.mode.src_fix   = D_DD_HDMAC_FS_INCR;
	hdmac_ctrl_trns.mode.dst_fix   = D_DD_HDMAC_FD_INCR;
	tmp_width = 0x01 << hdmac_ctrl_trns.size.trns_width;
	if (size > (tmp_width * 16)) {
		hdmac_ctrl_trns.mode.beat_type = D_DD_HDMAC_BT_INCR16;
	}
	else {
		hdmac_ctrl_trns.mode.beat_type  = D_DD_HDMAC_BT_NORMAL;
	}

	hdmac_ctrl_trns.size.trns_size = size;
	hdmac_ctrl_trns.size.src_addr  = src_addr;
	hdmac_ctrl_trns.size.dst_addr  = dst_addr;

	hdmac_ctrl_trns.int_handler = intHandler;

	ret = Dd_HDMAC_Ctrl_Trns(ch, &hdmac_ctrl_trns);
	if (ret != D_DDIM_OK) {
		Ddim_Print(("HDMAC Util Copy Async. Ctrl ERR : ret=0x%x\n", ret));
		return ret;
	}
	ret = Dd_HDMAC_Start_Async(ch);
	if (ret != D_DDIM_OK) {
		Ddim_Print(("HDMAC Util Copy Async. Start ERR : ret=0x%x\n", ret));
		return ret;
	}
	return ret;
}

//---------------------- colabo  section -------------------------------
// Nothing Special
#endif
