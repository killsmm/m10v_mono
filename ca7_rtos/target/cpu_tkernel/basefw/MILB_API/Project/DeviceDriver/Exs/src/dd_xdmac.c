/**
 * @file		dd_xdmac.c
 * @brief		XDMA driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_xdmac.h"
#include "dd_arm.h"
#include "arm.h"
#include "dd_exs.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_DD_XDMAC_SYNC				(0)		/* Sync Processing */
#define D_DD_XDMAC_ASYNC			(1)		/* Async Processing */

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
static volatile T_DD_XDMAC_CTRL gDD_XDMAC_Ctrl[D_DD_XDMAC_CH_NUM_MAX];		// DMA Register Info
static volatile USHORT gDD_XDMAC_Stop_Status[D_DD_XDMAC_CH_NUM_MAX];
static volatile INT32 gDD_XDMAC_Wait_End_Time[D_DD_XDMAC_CH_NUM_MAX];

// Spin Lock
static ULONG gDd_Xdmac_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64))) = 0;

#ifdef CO_ACT_CLOCK
static volatile UCHAR gDd_Xdmac_Clk_Ctrl_Cnt = 0;
#endif	// CO_ACT_CLOCK

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
#ifdef CO_ACT_CLOCK
/**
Control Clock Stop to start clock.<br>
*/
static VOID dd_xdmac_start_clock( VOID )
{
	Dd_ARM_Critical_Section_Start( gDd_Xdmac_Spin_Lock );
	
	gDd_Xdmac_Clk_Ctrl_Cnt++;
	if (gDd_Xdmac_Clk_Ctrl_Cnt == 1){
		if ((Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE() == 0)	&&
			(Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK() == 0)	){
			
			Dd_Exs_Set_XDMAC_BUS_CLOCK_Stop_Ctrl(0);
			Dd_ARM_Dsb_Pou();

			Dd_Exs_Set_XDMAC_LowPower_Ctl_CSYSREQ(1);
			Dd_ARM_Dsb_Pou();
			
			if (Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE() != 1){
				Ddim_Print(("[XDMAC]CLK ON Error. CACTIVE=0\n"));
			}
			
			if (Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK() != 1){
				Ddim_Print(("[XDMAC]CLK ON Error. CSYSACK=0\n"));
			}
			
			IO_XDMAC.XDACS.bit.LP = D_DD_XDMAC_XDACS_LP_DISABLE;
		}
	}
	
	Dd_ARM_Critical_Section_End( gDd_Xdmac_Spin_Lock );
}

/**
Control Clock Stop to stop clock.
*/
static VOID dd_xdmac_stop_clock( VOID )
{
	Dd_ARM_Critical_Section_Start( gDd_Xdmac_Spin_Lock );
	
	if ( 0 < gDd_Xdmac_Clk_Ctrl_Cnt ){
		gDd_Xdmac_Clk_Ctrl_Cnt--;
	}
	if ( 0 == gDd_Xdmac_Clk_Ctrl_Cnt ){
		if ((Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE() == 1)	&&
			(Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK() == 1)	){
			
			IO_XDMAC.XDACS.bit.LP = D_DD_XDMAC_XDACS_LP_ENABLE;
			Dd_ARM_Dsb_Pou();
			
			Dd_Exs_Set_XDMAC_LowPower_Ctl_CSYSREQ(0);
			Dd_ARM_Dsb_Pou();
			
			if (Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE() != 0){
				Ddim_Print(("[XDMAC]CLK OFF Error. CACTIVE=1\n"));
			}
			
			if (Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK() != 0){
				Ddim_Print(("[XDMAC]CLK OFF Error. CSYSACK=1\n"));
			}
			
			Dd_Exs_Set_XDMAC_BUS_CLOCK_Stop_Ctrl(1);
		}
	}
	
	Dd_ARM_Critical_Section_End( gDd_Xdmac_Spin_Lock );
}

#endif	// CO_ACT_CLOCK

/**
 * @brief  DMA wait end.
 */
static INT32 dd_xdmac_wait_end(UCHAR ch, USHORT* const status, UINT32 wait_mode)
{
	DDIM_USER_FLGPTN flg_ptn;
	DDIM_USER_ER ercd = 0;
	
	// CPU Polling (no use interrupt)
	if( wait_mode == D_DD_XDMAC_WAITMODE_CPU ){
		while(( IO_XDMAC.CH[ch].XDDSD.bit.TS == D_DD_XDMAC_XDDSD_TS_RUNNING ) && ( gDD_XDMAC_Stop_Status[ch] == 0 )){
			;	// It waits until completing it
		}
		
		if( gDD_XDMAC_Stop_Status[ch] == 0 ){
			*status = IO_XDMAC.CH[ch].XDDSD.bit.IS;
			IO_XDMAC.CH[ch].XDDSD.bit.IS = 0x0;		// Stop Status clear
			IO_XDMAC.CH[ch].XDDES.bit.CE = D_DD_XDMAC_XDDES_CE_DISABLE;			// DMA Stop
		}
		else{
			*status = gDD_XDMAC_Stop_Status[ch];
			gDD_XDMAC_Stop_Status[ch] = 0;
		}
	}
	// EVENT (use interrupt)
	else{
		switch(ch){
			case 0:
				ercd = DDIM_User_Twai_Flg(FID_DD_XDMAC, D_DD_XDMAC_FLG_CH0, D_DDIM_USER_TWF_ORW, &flg_ptn, gDD_XDMAC_Wait_End_Time[ch]);
				break;
			case 1:
				ercd = DDIM_User_Twai_Flg(FID_DD_XDMAC, D_DD_XDMAC_FLG_CH1, D_DDIM_USER_TWF_ORW, &flg_ptn, gDD_XDMAC_Wait_End_Time[ch]);
				break;
			case 2:
				ercd = DDIM_User_Twai_Flg(FID_DD_XDMAC, D_DD_XDMAC_FLG_CH2, D_DDIM_USER_TWF_ORW, &flg_ptn, gDD_XDMAC_Wait_End_Time[ch]);
				break;
			case 3:
				ercd = DDIM_User_Twai_Flg(FID_DD_XDMAC, D_DD_XDMAC_FLG_CH3, D_DDIM_USER_TWF_ORW, &flg_ptn, gDD_XDMAC_Wait_End_Time[ch]);
				break;
			default:
				break;
		}
		
		*status = gDD_XDMAC_Stop_Status[ch];
		gDD_XDMAC_Stop_Status[ch] = 0;
	}
	
	if( ercd == D_DDIM_USER_E_TMOUT ){
		Ddim_Print(("DMA error. timeout!\n"));
		return D_DD_XDMAC_TIMEOUT_ERR;
	}
	
	if( ercd != D_DDIM_USER_E_OK ){
		Ddim_Print(("DMA error. system error!\n"));
		return D_DD_XDMAC_SYSTEM_ERR;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief  DMA start.
 */
static INT32 dd_xdmac_start(UCHAR ch, USHORT* const status, UINT32 wait_mode, UINT32 sync)
{
	INT32 ret = D_DDIM_OK;
	union io_xdmac_xdsac xdsac;
	union io_xdmac_xddac xddac;
	union io_xdmac_xddes xddes;
	
	// DMA Enable
	if( IO_XDMAC.XDACS.bit.XE == D_DD_XDMAC_XDACS_XS_NOTACTIVE ){
		return D_DD_XDMAC_SYSTEM_ERR;
	}
	
	if( wait_mode == D_DD_XDMAC_WAITMODE_EVENT ){
		gDD_XDMAC_Ctrl[ch].config_1.bit.EI = D_DD_XDMAC_XDDES_EI_ENABLE;				// Error Interrupt Enable
		gDD_XDMAC_Ctrl[ch].config_1.bit.TI = D_DD_XDMAC_XDDES_TI_ENABLE;				// Completion Interrupt Enable
	}
	
	gDD_XDMAC_Stop_Status[ch] = 0;
	
	// XDSAC
	xdsac.word = IO_XDMAC.CH[ch].XDSAC.word;
	xdsac.bit.SRL = gDD_XDMAC_Ctrl[ch].config_1.bit.SRL;
	xdsac.bit.SAF = gDD_XDMAC_Ctrl[ch].config_1.bit.SAF;
	xdsac.bit.SBL = gDD_XDMAC_Ctrl[ch].config_1.bit.SBL;
	xdsac.bit.SBS = gDD_XDMAC_Ctrl[ch].config_1.bit.SBS;
	IO_XDMAC.CH[ch].XDSAC.word = xdsac.word;
	
	// XDDAC
	xddac.word = IO_XDMAC.CH[ch].XDDAC.word;
	xddac.bit.DRL = gDD_XDMAC_Ctrl[ch].config_1.bit.DRL;
	xddac.bit.DAF = gDD_XDMAC_Ctrl[ch].config_1.bit.DAF;
	xddac.bit.DBL = gDD_XDMAC_Ctrl[ch].config_1.bit.DBL;
	xddac.bit.DBS = gDD_XDMAC_Ctrl[ch].config_1.bit.DBS;
	IO_XDMAC.CH[ch].XDDAC.word = xddac.word;
	
	// XDDES
	xddes.word = IO_XDMAC.CH[ch].XDDES.word;
	xddes.bit.TI = gDD_XDMAC_Ctrl[ch].config_1.bit.TI;
	xddes.bit.EI = gDD_XDMAC_Ctrl[ch].config_1.bit.EI;
	xddes.bit.AT = gDD_XDMAC_Ctrl[ch].config_1.bit.AT;
	xddes.bit.BR = gDD_XDMAC_Ctrl[ch].config_1.bit.BR;
	xddes.bit.BT = gDD_XDMAC_Ctrl[ch].config_1.bit.BT;
	xddes.bit.SA = gDD_XDMAC_Ctrl[ch].config_1.bit.SA;
	xddes.bit.TF = gDD_XDMAC_Ctrl[ch].config_1.bit.TF;
	xddes.bit.SE = gDD_XDMAC_Ctrl[ch].config_1.bit.SE;
	IO_XDMAC.CH[ch].XDDES.word = xddes.word;
	
	IO_XDMAC.CH[ch].XDDCC = gDD_XDMAC_Ctrl[ch].config_2.word;
	
	IO_XDMAC.CH[ch].XDTBC = gDD_XDMAC_Ctrl[ch].trns_size - 1;
	
	IO_XDMAC.CH[ch].XDSSA = gDD_XDMAC_Ctrl[ch].src_addr;		// DMA Source Address register set
	IO_XDMAC.CH[ch].XDDSA = gDD_XDMAC_Ctrl[ch].dst_addr;		// DMA Destination Address register set
	
	if( wait_mode == D_DD_XDMAC_WAITMODE_EVENT ){
		switch(ch){
			case 0:
				DDIM_User_Clr_Flg(FID_DD_XDMAC, ~D_DD_XDMAC_FLG_CH0);
				break;
			case 1:
				DDIM_User_Clr_Flg(FID_DD_XDMAC, ~D_DD_XDMAC_FLG_CH1);
				break;
			case 2:
				DDIM_User_Clr_Flg(FID_DD_XDMAC, ~D_DD_XDMAC_FLG_CH2);
				break;
			case 3:
				DDIM_User_Clr_Flg(FID_DD_XDMAC, ~D_DD_XDMAC_FLG_CH3);
				break;
			default:
				break;
		}
	}
	
	IO_XDMAC.CH[ch].XDDES.bit.CE = D_DD_XDMAC_XDDES_CE_ENABLE;
	Dd_ARM_Dsb_Pou();
	
	// Sync Processing
	if( sync == D_DD_XDMAC_SYNC ){
		ret = dd_xdmac_wait_end(ch, status, wait_mode);
	}
	
	return ret;
}

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief  Execute exclusive control for designated DMA channel.
 * @param  INT32	tmout
 * @param  UCHAR	ch	Channel number (0 to 3)
 * @return INT32 	D_DDIM_OK / D_DD_XDMAC_EXC_LOCK_NG / D_DD_XDMAC_INPUT_PARAM_ERR / D_DD_XDMAC_SEM_TIMEOUT / D_DD_XDMAC_SEM_NG
 */
INT32 Dd_XDMAC_Open(UCHAR ch, INT32 tmout)
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( tmout < D_DDIM_USER_SEM_WAIT_FEVR ){
		Ddim_Assertion(("Dd_XDMAC_Open: input param error. tmout = %d\n", tmout));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif
	DDIM_USER_ER ercd = 0;
	
	if( D_DDIM_USER_SEM_WAIT_POL == tmout ){
		ercd = DDIM_User_Pol_Sem(SID_DD_XDMAC(ch));				// pol_sem()
	}
	else{
		ercd = DDIM_User_Twai_Sem(SID_DD_XDMAC(ch), (DDIM_USER_TMO)tmout);	// twai_sem()
	}
	
	if( D_DDIM_USER_E_OK != ercd ){
		if( D_DDIM_USER_E_TMOUT == ercd ){
			return D_DD_XDMAC_SEM_TIMEOUT;
		}
		
		return D_DD_XDMAC_SEM_NG;
	}
	

#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	return D_DDIM_OK;
}

/**
 * @brief  Cancel exclusive control for designated DMA channel.
           When the designated channel is under transferring, stop transfer immediately and cancel lock.
 * @param  UCHAR	ch			Channel number (0 to 3)
 * @return INT32  D_DDIM_OK/D_DD_XDMAC_INPUT_PARAM_ERR / D_DD_XDMAC_SEM_NG
 */
INT32 Dd_XDMAC_Close(UCHAR ch)
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif
	DDIM_USER_ER ercd = 0;
	
	// Global variable is clear
	gDD_XDMAC_Ctrl[ch].config_1.word = 0;
	gDD_XDMAC_Ctrl[ch].config_2.word = 0;
	gDD_XDMAC_Ctrl[ch].src_addr      = 0;
	gDD_XDMAC_Ctrl[ch].dst_addr      = 0;
	gDD_XDMAC_Ctrl[ch].trns_size     = 0;
	gDD_XDMAC_Ctrl[ch].int_handler   = NULL;
	
	ercd = DDIM_User_Sig_Sem(SID_DD_XDMAC(ch));				// sig_sem()
	if( D_DDIM_USER_E_OK != ercd ){
		return D_DD_XDMAC_SEM_NG;
	}
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return D_DDIM_OK;
}

/**
 * @brief  Set control data to registers related DMA designated channel.
 * @return INT32	D_DDIM_OK / D_DD_XDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_XDMAC_Ctrl_Common(T_DD_XDMAC_COMMON const *const dma_common)
{
#ifdef CO_PARAM_CHECK
	if( dma_common == NULL ){
		Ddim_Assertion(("XDMAC: input param error. [*dma_ctrl] NULL\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Xdmac_Spin_Lock);
	
	// The value is set to an external variable
	IO_XDMAC.XDACS.bit.CP = dma_common->common_config.bit.CP;
	IO_XDMAC.XDACS.bit.XE = dma_common->common_config.bit.XE;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Xdmac_Spin_Lock);
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return D_DDIM_OK;
}

/**
 * @brief  Set control data to registers related DMA designated channel.
 * @param  UCHAR	ch	Channel number (0 to 3)
 * @param  UCHAR	dma_ctrl_trans	DMA Transfer setting
 * @return INT32	D_DDIM_OK / D_DD_XDMAC_INPUT_PARAM_ERR
 */
INT32	Dd_XDMAC_Ctrl_Trns(UCHAR ch, T_DD_XDMAC_CTRL const *const dma_ctrl_trans)
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( dma_ctrl_trans == NULL ){
		Ddim_Assertion(("XDMAC: input param error. [*dma_ctrl_trans] NULL\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( dma_ctrl_trans->trns_size == 0 ){
		Ddim_Assertion(("XDMAC: input param error. [dma_ctrl_trans->trns_size] Zero\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif
	// The value is set to an external variable
	gDD_XDMAC_Ctrl[ch].config_1.word = dma_ctrl_trans->config_1.word;
	gDD_XDMAC_Ctrl[ch].config_2.word = 0;
	gDD_XDMAC_Ctrl[ch].config_2.bit.DCA = dma_ctrl_trans->config_2.bit.DCA;
	gDD_XDMAC_Ctrl[ch].config_2.bit.DCN = dma_ctrl_trans->config_2.bit.DCN;
	
	gDD_XDMAC_Ctrl[ch].trns_size       = dma_ctrl_trans->trns_size;
	gDD_XDMAC_Ctrl[ch].src_addr        = dma_ctrl_trans->src_addr;
	gDD_XDMAC_Ctrl[ch].dst_addr        = dma_ctrl_trans->dst_addr;
	gDD_XDMAC_Ctrl[ch].int_handler     = dma_ctrl_trans->int_handler;
	
	gDD_XDMAC_Wait_End_Time[ch] = D_DDIM_WAIT_END_TIME;
	
	return D_DDIM_OK;
}

/**
 * @brief  Set translation size to registers related DMA designated channel.
 * @param  UCHAR	ch	Channel number (0 to 3)
 * @param  UCHAR	dma_trns_size	DMA Transfer size
 * @return INT32	D_DDIM_OK / D_DD_XDMAC_INPUT_PARAM_ERR
 */
INT32	Dd_XDMAC_Set_Trns_Size(UCHAR ch, T_DD_XDMAC_TRNS_SIZE const *const dma_trns_size)
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( dma_trns_size == NULL ){
		Ddim_Assertion(("XDMAC: input param error. [*dma_trns_size] NULL\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( dma_trns_size->trns_size == 0 ){
		Ddim_Assertion(("XDMAC: input param error. [dma_trns_size->trns_size] Zero\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif

#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK

	gDD_XDMAC_Ctrl[ch].trns_size = dma_trns_size->trns_size;
	gDD_XDMAC_Ctrl[ch].src_addr  = dma_trns_size->src_addr;
	gDD_XDMAC_Ctrl[ch].dst_addr  = dma_trns_size->dst_addr;
	IO_XDMAC.CH[ch].XDSSA = gDD_XDMAC_Ctrl[ch].src_addr;
	IO_XDMAC.CH[ch].XDDSA = gDD_XDMAC_Ctrl[ch].dst_addr;
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return D_DDIM_OK;
}

/**
 * @brief  The operation of DMA of specified ch begins.
 * @param  UCHAR			ch			Channel number (0 to 3)
 *         USHORT*			status		Channel Status Register pointer
 *         UINT32			wait_mode	DMA end wait mode
 * @return INT32			D_DDIM_OK / D_DD_XDMAC_INPUT_PARAM_ERR / D_DDIM_TIMEOUT / D_DD_XDMAC_SYSTEM_ERR
 */
INT32	Dd_XDMAC_Start_Sync(UCHAR ch, USHORT *const status, UINT32 wait_mode)
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( status == NULL ){
		Ddim_Assertion(("XDMAC: input param error. [*status] NULL\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if(( wait_mode != D_DD_XDMAC_WAITMODE_CPU ) && ( wait_mode != D_DD_XDMAC_WAITMODE_EVENT )){
		Ddim_Assertion(("XDMAC: input param error. [wait_mode] = %x\n", wait_mode));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif

#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	ret = dd_xdmac_start(ch, status, wait_mode, D_DD_XDMAC_SYNC);		// DMA Start
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return ret;
}

/**
 * @brief  The operation of DMA of specified ch begins.
 * @param  UCHAR			ch			Channel number (0 to 3)
 * @return INT32			D_DDIM_OK / D_DD_XDMAC_INPUT_PARAM_ERR
 */
INT32	Dd_XDMAC_Start_Async(UCHAR ch)
{
	USHORT	status;		// dummy
	
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	dd_xdmac_start(ch, &status, D_DD_XDMAC_WAITMODE_EVENT, D_DD_XDMAC_ASYNC);	// DMA Start
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return D_DDIM_OK;
}

/**
 * @brief  The operation of DMA of specified ch is stopped.
 * @param  UCHAR	ch			Channel number (0 to 3)
 * @return INT32	D_DDIM_OK / D_DD_XDMAC_INPUT_PARAM_ERR
 */
INT32	Dd_XDMAC_Stop(UCHAR ch)
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif

#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK

	IO_XDMAC.CH[ch].XDDES.bit.CE = D_DD_XDMAC_XDDES_CE_DISABLE;
	Dd_ARM_Dsb_Pou();
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK

	return D_DDIM_OK;
}

/**
 * @brief  Wait end time of transfer process of designated channel.
 * @param  UCHAR	ch			Channel number (0 to 3)
 *         INT32	wait_time	Wait end time of transfer process
 * @return INT32				D_DDIM_OK / D_DD_XDMAC_INPUT_PARAM_ERR
 */
INT32 Dd_XDMAC_Set_Wait_Time(UCHAR ch, INT32 wait_time)
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( wait_time < D_DDIM_WAIT_END_FOREVER ){
		Ddim_Assertion(("XDMAC: input param error. [wait_time] = %d\n", wait_time));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif
	gDD_XDMAC_Wait_End_Time[ch] = wait_time;
	
	return D_DDIM_OK;
}

/**
 * @brief  Wait end of transfer process of designated channel.
 *         The value of the CSTR register is passed by the out parameter.
 * @param  UCHAR	ch			Channel number (0 to 3)
 *         USHORT*	status		Channel Status Register pointer
 * @return INT32	D_DDIM_OK / D_DD_XDMAC_INPUT_PARAM_ERR / D_DDIM_TIMEOUT / D_DD_XDMAC_SYSTEM_ERR
 */
INT32 Dd_XDMAC_Wait_End(UCHAR ch, USHORT* const status, UINT32 wait_mode)
{
	INT32 ret;
	
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( status == NULL ){
		Ddim_Assertion(("XDMAC: input param error. [*status] NULL\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if(( wait_mode != D_DD_XDMAC_WAITMODE_CPU ) && ( wait_mode != D_DD_XDMAC_WAITMODE_EVENT )){
		Ddim_Assertion(("XDMAC: input param error. [wait_mode] = %x\n", wait_mode));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif

#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	ret = dd_xdmac_wait_end(ch, status, wait_mode);
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return ret;
}

/**
 * @brief  The status of DMA of specified ch is cleared.
 * @param  UCHAR	ch			Channel number (0 to 3)
 * @return INT32				D_DDIM_OK / D_DD_XDMAC_INPUT_PARAM_ERR
 */
INT32	Dd_XDMAC_Clear_Status(UCHAR ch)
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	IO_XDMAC.CH[ch].XDDSD.bit.IS = D_DD_XDMAC_XDDSD_IS_NONE;
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return D_DDIM_OK;
}

/**
 * @brief  The error status of DMA of specified ch is collected.
 * @param  UCHAR	ch			Channel number (0 to 3)
 * @return INT32				D_DDIM_OK / D_DD_XDMAC_INPUT_PARAM_ERR
 */
INT32	Dd_XDMAC_Get_Status(UCHAR ch, USHORT *const xdmac_status, USHORT *const transfer_status, USHORT *const interrupt_status)
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( xdmac_status == NULL ){
		Ddim_Assertion(("XDMAC: input param error. [*status] NULL\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( transfer_status == NULL ){
		Ddim_Assertion(("XDMAC: input param error. [*transfer_status] NULL\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( interrupt_status == NULL ){
		Ddim_Assertion(("XDMAC: input param error. [*interrupt_status] NULL\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif

#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	*xdmac_status = IO_XDMAC.XDACS.bit.XS;
	*transfer_status = IO_XDMAC.CH[ch].XDDSD.bit.TS;
	*interrupt_status = IO_XDMAC.CH[ch].XDDSD.bit.IS;
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK

	return D_DDIM_OK;
}

/**
 * @brief   The value of the transferd size of the specified channel is acquired.
 * @param   UCHAR		ch		Channel number (0 to 3)
 * @return  ULONG				Transferd size of the specified channel
 * @remarks This API forced cancel exclusive control if process is under executing.
 */
ULONG	Dd_XDMAC_Get_Trns_Size(UCHAR ch)
{
	ULONG size;
	
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return 0;
	}
#endif

#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	size = gDD_XDMAC_Ctrl[ch].trns_size - (IO_XDMAC.CH[ch].XDTBC + 1);
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return size;
}

/**
 * @brief  The value of the source address (DMACSA) of the specified channel is acquired.
 * @param  UCHAR		ch		Channel number (0 to 3)
 * @return ULONG				Source address of the specified channel
 */
ULONG	Dd_XDMAC_Get_Src_Addr(UCHAR ch)
{
	ULONG addr;
	
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return 0;
	}
#endif
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	addr = IO_XDMAC.CH[ch].XDSSA;
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return addr;
}

/**
 * @brief  The value of the destination address (DMACDA) of the specified channel is acquired.
 * @param  UCHAR		ch		Channel number (0 to 3)
 * @return ULONG				Destination address of the specified channel
 */
ULONG	Dd_XDMAC_Get_Dst_Addr(UCHAR ch)
{
	ULONG addr;
	
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return 0;
	}
#endif
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	addr = IO_XDMAC.CH[ch].XDDSA;
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return addr;
}

/**
 * @brief  Low Power is set is set.
 * @param  UCHAR		lowpower	Low Power
 * @return INT32					D_DDIM_OK / D_DD_XDMAC_INPUT_PARAM_ERR
 */
INT32	Dd_XDMAC_Set_LowPower(UCHAR lowpower)
{
#ifdef CO_PARAM_CHECK
	if (lowpower > D_DD_XDMAC_XDACS_LP_ENABLE) {
		Ddim_Assertion(("XDMAC: input param error. [lowpower] = %d\n", lowpower));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif

#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	// SpinLock
	Dd_ARM_Critical_Section_Start(gDd_Xdmac_Spin_Lock);
	
	IO_XDMAC.XDACS.bit.LP = lowpower;
	
	// SpinUnLock
	Dd_ARM_Critical_Section_End(gDd_Xdmac_Spin_Lock);
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return D_DDIM_OK;
}

/**
 * @brief  Source Protection code is set.
 * @param  UCHAR		ch				Channel number (0 to 3)
 * @param  UCHAR		protect_code	Protection Code
 * @return INT32						D_DDIM_OK / D_DD_XDMAC_INPUT_PARAM_ERR
 */
INT32	Dd_XDMAC_Set_Source_Protect(UCHAR ch, UCHAR protect_code)
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( protect_code > D_DD_XDMAC_XDDPC_SP_PRIVILEGED_SECURE ){
		Ddim_Assertion(("XDMAC: input param error. [protect_code] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif

#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	IO_XDMAC.CH[ch].XDDPC.bit.SP = protect_code;
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return D_DDIM_OK;
}

/**
 * @brief  Destination Protection code is set.
 * @param  UCHAR		ch				Channel number (0 to 3)
 * @param  UCHAR		protect_code	Protection Code
 * @return INT32						D_DDIM_OK / D_DD_XDMAC_INPUT_PARAM_ERR
 */
INT32	Dd_XDMAC_Set_Destination_Protect(UCHAR ch, UCHAR protect_code)
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( protect_code > D_DD_XDMAC_XDDPC_DP_PRIVILEGED_SECURE ){
		Ddim_Assertion(("XDMAC: input param error. [protect_code] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif

#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	IO_XDMAC.CH[ch].XDDPC.bit.DP = protect_code;
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return D_DDIM_OK;
}

/**
 * @brief  The operation of All DMA channel is stopped.
 */
VOID	Dd_XDMAC_Stop_All_Ch(VOID)
{
#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	IO_XDMAC.CH[0].XDDES.bit.CE = D_DD_XDMAC_XDDES_CE_DISABLE;
	IO_XDMAC.CH[1].XDDES.bit.CE = D_DD_XDMAC_XDDES_CE_DISABLE;
	IO_XDMAC.CH[2].XDDES.bit.CE = D_DD_XDMAC_XDDES_CE_DISABLE;
	IO_XDMAC.CH[3].XDDES.bit.CE = D_DD_XDMAC_XDDES_CE_DISABLE;
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
	
	return;
}

/**
 * @brief  Set DMA Interrupt Handler.
 * @param  UCHAR		ch				Channel number (0 to 3)
 * @param  VOID			int_handler		Interrupt handler
 * @return VOID
 */
VOID  Dd_XDMAC_Set_Int_Handler(UCHAR ch, VOID (*int_handler)(VOID))
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return;
	}
#endif
	gDD_XDMAC_Ctrl[ch].int_handler = int_handler;
}

/**
 * @brief  Interrupt handler of DMA channel 0 for transfer process is finished.
 * @param  UCHAR		ch				Channel number (0 to 3)
 * @return VOID
 */
VOID  Dd_XDMAC_Int_Handler(UCHAR ch)
{
	DDIM_USER_ER ercd;
	USHORT status;
	VP_CALLBACK handler;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return;
	}
#endif

#ifdef CO_ACT_CLOCK
	dd_xdmac_start_clock();
#endif //CO_ACT_CLOCK
	
	status = IO_XDMAC.CH[ch].XDDSD.bit.IS;								// To give the user the content of the IS of XDDSD register
	gDD_XDMAC_Stop_Status[ch] = status;
	IO_XDMAC.CH[ch].XDDSD.bit.IS = D_DD_XDMAC_XDDSD_IS_NONE;		// Stop Status clear
	IO_XDMAC.CH[ch].XDDES.bit.CE = D_DD_XDMAC_XDDES_CE_DISABLE;		// DMA Stop
	Dd_ARM_Dsb_Pou();
	
	handler = gDD_XDMAC_Ctrl[ch].int_handler;
	
	if (handler != NULL) {
		(*handler)(ch, &status);
	}
	
	if( status != D_DD_XDMAC_XDDSD_IS_NONE ){
		switch(ch){
			case 0:
				ercd = DDIM_User_Set_Flg(FID_DD_XDMAC, D_DD_XDMAC_FLG_CH0);
				break;
			case 1:
				ercd = DDIM_User_Set_Flg(FID_DD_XDMAC, D_DD_XDMAC_FLG_CH1);
				break;
			case 2:
				ercd = DDIM_User_Set_Flg(FID_DD_XDMAC, D_DD_XDMAC_FLG_CH2);
				break;
			case 3:
				ercd = DDIM_User_Set_Flg(FID_DD_XDMAC, D_DD_XDMAC_FLG_CH3);
				break;
			default:
				ercd = -1;
				break;
		}
	}
	else{
		ercd = D_DDIM_USER_E_OK;
	}
	
	if( ercd != D_DDIM_USER_E_OK ){
		Ddim_Print(("I:DDIM_User_Set_Flg error. ercd = %d\n", ercd));
	}
	
#ifdef CO_ACT_CLOCK
	dd_xdmac_stop_clock();
#endif //CO_ACT_CLOCK
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/**
 * @brief  DMA Copy Sync for SDRAM
 * @param  UCHAR		ch				Channel number (0 to 3)
 * @param  ULONG		src_addr		source address
 * @param  ULONG		dst_addr		destination address
 * @param  ULONG		size			Copy size
 * @param  INT32		wait_mode		DMA end wait mode
 * @return INT32		D_DDIM_OK
 */
INT32 Dd_XDMAC_Copy_SDRAM_Sync(UCHAR ch, ULONG src_addr, ULONG dst_addr, ULONG size, UINT32 wait_mode)
{
	INT32              ret;
	USHORT             status = 0;
	T_DD_XDMAC_CTRL dma_ctrl_trns;
	T_DD_XDMAC_COMMON dma_common;
	
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if(( src_addr == 0 ) || ( dst_addr == 0 )){
		Ddim_Assertion(("XDMAC: input param error. [addr] = 0\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( size == 0 ){
		Ddim_Assertion(("XDMAC: input param error. [size] = 0\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if(( wait_mode != D_DD_XDMAC_WAITMODE_CPU ) && ( wait_mode != D_DD_XDMAC_WAITMODE_EVENT )){
		Ddim_Assertion(("XDMAC: input param error. [wait_mode] = %x\n", wait_mode));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif
	// Check transfer size
	if((( src_addr & 0x07) == 0 ) && ((dst_addr & 0x07) == 0 ) && ( (size & 0x07) == 0) ){
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_DOUBLEWORD;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_DOUBLEWORD;
	}
	else if(( (src_addr & 0x03) == 0 ) && ( (dst_addr & 0x03) == 0 ) && ( (size & 0x03) == 0 )){
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_WORD;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_WORD;
	}
	else if(( (src_addr & 0x01) == 0 ) && ( (dst_addr & 0x01) == 0 ) && ( (size & 0x01) == 0 )){
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_HALFWORD;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_HALFWORD;
	}
	else{
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_BYTE;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_BYTE;
	}
	
	// Parameter Setting
	dma_ctrl_trns.trns_size       = size;
	dma_ctrl_trns.src_addr        = src_addr;
	dma_ctrl_trns.dst_addr        = dst_addr;
	dma_ctrl_trns.int_handler     = NULL;
	
	dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMAC_XDSAC_SRL_DISABLE;
	dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMAC_XDSAC_SAF_NOTFIX;
	dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMAC_XDSAC_SBL_LENGTH_16;
	dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMAC_XDDAC_DRL_DISABLE;
	dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMAC_XDDAC_DAF_NOTFIX;
	dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMAC_XDDAC_DBL_LENGTH_16;
	
	if( wait_mode == D_DD_XDMAC_WAITMODE_EVENT ){
		dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_ENABLE;
		dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_ENABLE;
	}
	else{
		dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_DISABLE;
		dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_DISABLE;
	}
	
	dma_ctrl_trns.config_1.bit.AT = D_DD_XDMAC_XDDES_AT_SOURCE;
	dma_ctrl_trns.config_1.bit.BR = D_DD_XDMAC_XDDES_BR_DISABLE;
	dma_ctrl_trns.config_1.bit.BT = D_DD_XDMAC_XDDES_BT_ENABLE;
	dma_ctrl_trns.config_1.bit.SA = D_DD_XDMAC_XDDES_SA_ENABLE;
	dma_ctrl_trns.config_1.bit.TF = D_DD_XDMAC_XDDES_TF_SOFT;
	dma_ctrl_trns.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_ENABLE;
	dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_INVALID;
	dma_ctrl_trns.config_2.bit.DCA = 0;
	
	dma_common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
	dma_common.common_config.bit.CP = D_DD_XDMAC_XDACS_CP_FIXED;
	
	ret = Dd_XDMAC_Open(ch,D_DDIM_WAIT_END_TIME);
	if( ret != D_DDIM_OK ){
		Ddim_Print(("DMA Util Copy Sync. Open ERR : ret=0x%x\n", ret));
		return ret;
	}
	
	ret = Dd_XDMAC_Ctrl_Common(&dma_common);
	if( ret != D_DDIM_OK ){
		Ddim_Print(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
		return ret;
	}
	
	while(1){
		ret = Dd_XDMAC_Ctrl_Trns(ch, &dma_ctrl_trns);
		if( ret != D_DDIM_OK ){
			Ddim_Print(("DMA Util Copy Sync. Ctrl Setting ERR : ret=0x%x\n", ret));
			break;
		}
		ret = Dd_XDMAC_Start_Sync(ch, &status, wait_mode);
		if( ret != D_DDIM_OK ){
			Ddim_Print(("DMA Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
		}
		break;
	}
	
	ret = Dd_XDMAC_Close(ch);
	if( ret != D_DDIM_OK ){
		Ddim_Print(("DMA Util Copy Sync. Close ERR : ret=0x%x\n", ret));
	}
	
	return ret;
}

/**
 * @brief  DMA Copy Async SDRAM
 * @param  UCHAR		ch				Channel number (0 to 3)
 * @param  ULONG		src_addr		source address
 * @param  ULONG		dst_addr		destination address
 * @param  ULONG		size			Copy size
 * @param  VP_CALLBACK	intHandler		Callback function pointer
 * @return INT32  		D_DDIM_OK
 */
INT32 Dd_XDMAC_Copy_SDRAM_Async(UCHAR ch, ULONG src_addr, ULONG dst_addr, ULONG size, VP_CALLBACK intHandler)
{
	INT32              ret;
	T_DD_XDMAC_CTRL dma_ctrl_trns;
	T_DD_XDMAC_COMMON dma_common;
	
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_XDMAC_CH_NUM_MAX ){
		Ddim_Assertion(("XDMAC: input param error. [ch] = %x\n", ch));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if(( src_addr == 0 ) || ( dst_addr == 0 )){
		Ddim_Assertion(("XDMAC: input param error. [addr] = 0\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
	if( size == 0 ){
		Ddim_Assertion(("XDMAC: input param error. [size] = 0\n"));
		return D_DD_XDMAC_INPUT_PARAM_ERR;
	}
#endif
	// Check transfer size
	if(( (src_addr & 0x07) == 0 ) && ( (dst_addr & 0x07) == 0 ) && ( (size & 0x07) == 0) ){
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_DOUBLEWORD;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_DOUBLEWORD;
	}
	else if(( (src_addr & 0x03) == 0 ) && ( (dst_addr & 0x03) == 0 ) && ( (size & 0x03) == 0 )){
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_WORD;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_WORD;
	}
	else if(( (src_addr & 0x01) == 0 ) && ( (dst_addr & 0x01) == 0 ) && ( (size & 0x01) == 0 )){
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_HALFWORD;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_HALFWORD;
	}
	else{
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_BYTE;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_BYTE;
	}
	
	// Parameter Setting
	dma_ctrl_trns.trns_size       = size;
	dma_ctrl_trns.src_addr        = src_addr;
	dma_ctrl_trns.dst_addr        = dst_addr;
	dma_ctrl_trns.int_handler     = intHandler;
	
	dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMAC_XDSAC_SRL_DISABLE;
	dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMAC_XDSAC_SAF_NOTFIX;
	dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMAC_XDSAC_SBL_LENGTH_16;
	dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMAC_XDDAC_DRL_DISABLE;
	dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMAC_XDDAC_DAF_NOTFIX;
	dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMAC_XDDAC_DBL_LENGTH_16;
	
	dma_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_ENABLE;
	dma_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_ENABLE;
	
	dma_ctrl_trns.config_1.bit.AT = D_DD_XDMAC_XDDES_AT_SOURCE;
	dma_ctrl_trns.config_1.bit.BR = D_DD_XDMAC_XDDES_BR_DISABLE;
	dma_ctrl_trns.config_1.bit.BT = D_DD_XDMAC_XDDES_BT_ENABLE;
	dma_ctrl_trns.config_1.bit.SA = D_DD_XDMAC_XDDES_SA_ENABLE;
	dma_ctrl_trns.config_1.bit.TF = D_DD_XDMAC_XDDES_TF_SOFT;
	dma_ctrl_trns.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_ENABLE;
	dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_INVALID;
	dma_ctrl_trns.config_2.bit.DCA = 0;
	
	dma_common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
	dma_common.common_config.bit.CP = D_DD_XDMAC_XDACS_CP_FIXED;
	
	ret = Dd_XDMAC_Ctrl_Common(&dma_common);
	if( ret != D_DDIM_OK ){
		Ddim_Print(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
		return ret;
	}
	
	ret = Dd_XDMAC_Ctrl_Trns(ch, &dma_ctrl_trns);
	if( ret != D_DDIM_OK ){
		Ddim_Print(("DMA Util Copy Async. Ctrl ERR : ret=0x%x\n", ret));
		return ret;
	}
	
	ret = Dd_XDMAC_Start_Async(ch);
	if( ret != D_DDIM_OK ){
		Ddim_Print(("DMA Util Copy Async. Start ERR : ret=0x%x\n", ret));
		return ret;
	}
	
	return ret;
}
#endif	// CO_DDIM_UTILITY_USE
