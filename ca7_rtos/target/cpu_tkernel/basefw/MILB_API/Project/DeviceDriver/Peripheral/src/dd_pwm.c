/**
 * @file		dd_pwm.c
 * @brief		PWM driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "peripheral.h"
#include "dd_pwm.h"
#include "dd_top.h"
#include "dd_arm.h"
#include "dd_hdmac0.h"
#include "ddim_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
#define D_DD_PWM_MAX_CYC			(8)
//---------------------- colabo  section -------------------------------
#define D_DD_PWM_DMACA_IS_BASE		(D_DD_HDMAC0_IS_IDREQ_25)
#endif	/* CO_DDIM_UTILITY_USE */

#define dd_pwm_dsb() Dd_ARM_Dsb_Pou()

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// PWM control info
typedef struct {
	UCHAR clk_sel;				/**< clock select				 PCNH */
	UCHAR int_enable;			/**< interrupt enable			 PCNH */
	UCHAR dma_enable;			/**< DMA enable						  */
	UCHAR dma_ch;				/**< DMA enable						  */
	USHORT cyc_num; 			/**< number of cycle				  */
	USHORT duty_num;			/**< number of duty					  */
	VP_CALLBACK	pCallBack;		/**< callback function pointer		  */
} T_DD_PWM_CTRL_MEMO;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif	/* CO_DDIM_UTILITY_USE */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// PWM control management table
static volatile T_DD_PWM_CTRL_MEMO gDD_PWM_Ctrl[D_DD_PWM_CH_NUM_MAX];

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif	/* CO_DDIM_UTILITY_USE */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
static volatile union io_pwm_pcn* dd_pwm_pcn_addr(UINT32 ch);

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif	/* CO_DDIM_UTILITY_USE */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/**
 * @brief  Get PWM control PCNx register Address.
 * @param  UINT32 ch
 * @return union io_pwm_pcn*
 */
static volatile union io_pwm_pcn* dd_pwm_pcn_addr(UINT32 ch)
{
	const UINT32 ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */
	volatile union io_pwm_pcn* pcn_addr = NULL;

	switch (ch_in_blk) {
		case 0:
			pcn_addr = (union io_pwm_pcn*) &IO_PERI.PWM[blk].PCNA;
			break;

		case 1:
			pcn_addr = (union io_pwm_pcn*) &IO_PERI.PWM[blk].PCNB;
			break;

		case 2:
			pcn_addr = (union io_pwm_pcn*) &IO_PERI.PWM[blk].PCNC;
			break;

		case 3:
			pcn_addr = (union io_pwm_pcn*) &IO_PERI.PWM[blk].PCND;
			break;

		default:
			break;
	}
	return pcn_addr;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
static INT32 dd_pwm_dma_set( UINT32 pwm_ch, UCHAR dma_ch, UCHAR step_remainder, ULONG src_addr, ULONG dst_addr ){
	INT32 ret;
	T_DD_HDMAC0_CTRL dma_ctrl;

	ret = Dd_HDMAC0_Open( dma_ch, D_DDIM_USER_SEM_WAIT_POL );		// Channel open
	if(ret != D_DDIM_OK){
		Ddim_Print(("Dd_HDMAC0_Open() error. ret=0x%x\n", ret));
		return ret;
	}

	dma_ctrl.config_a.word	 = 0;
	dma_ctrl.config_a.bit.is = D_DD_PWM_DMACA_IS_BASE + (pwm_ch >> 2);
	dma_ctrl.config_a.bit.bt = D_DD_HDMAC0_BT_NORMAL;
	dma_ctrl.config_a.bit.bc = 3 - 1;				// cycle, duty, pulse
	dma_ctrl.config_a.bit.tc = step_remainder - 1;

	dma_ctrl.config_b.word	 = 0;
	dma_ctrl.config_b.bit.ms = D_DD_HDMAC0_MS_BLK;
	dma_ctrl.config_b.bit.tw = D_DD_HDMAC0_TW_HWORD;
	dma_ctrl.config_b.bit.fs = D_DD_HDMAC0_FS_INCR;
	dma_ctrl.config_b.bit.fd = D_DD_HDMAC0_FD_FIX;
	dma_ctrl.config_b.bit.rc = D_DD_HDMAC0_RC_DISABLE;
	dma_ctrl.config_b.bit.rs = D_DD_HDMAC0_RS_ENABLE;
	dma_ctrl.config_b.bit.rd = D_DD_HDMAC0_RD_DISABLE;

	dma_ctrl.src_addr		 = src_addr;
	dma_ctrl.dst_addr		 = dst_addr;
	dma_ctrl.int_handler	 = NULL;

	ret = Dd_HDMAC0_Ctrl_Common( dma_ch, &dma_ctrl );
	if(ret != D_DDIM_OK){
		Dd_HDMAC0_Close( dma_ch );
		Ddim_Print(("Dd_HDMAC0_Ctrl_Common() error. ret=0x%x\n", ret));
		return ret;
	}

	ret = Dd_HDMAC0_Start_Async( dma_ch );
	if(ret != D_DDIM_OK){
		Dd_HDMAC0_Close( dma_ch );
		Ddim_Print(("Dd_HDMAC0_Start_Async() error. ret=0x%x\n", ret));
		return ret;
	}
	return ret;
}
#endif	/* CO_DDIM_UTILITY_USE */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/**
 * @brief  The PWM control register (all channels) is initialized.
 * @param  VOID
 * @return VOID
 */
VOID Dd_PWM_Init(VOID)
{
	INT32 blckn;

//	Dd_Top_Set_CLKSEL_RCLK(0);

	for( blckn = 0; blckn < D_DD_PWM_BLOCK_NUM; blckn++ ) {
		IO_PERI.PWM[blckn].GCN2.word = 0xF000; //0x5000;

		IO_PERI.PWM[blckn].PCNA.word = 0x0010;
		IO_PERI.PWM[blckn].PCNB.word = 0x0010;
		IO_PERI.PWM[blckn].PCNC.word = 0x0010;
		IO_PERI.PWM[blckn].PCND.word = 0x0010;
		IO_PERI.PWM[blckn].PCNE.word = 0x0010;

		IO_PERI.PWM[blckn].GCN2.bit.TMEEN = 0;
	}
}


/**
 * @brief  The PWM channel is exclusively controlled.
 * @param  UINT32 ch
 * @param  INT32  tmout
 * @return INT32  D_DDIM_OK/D_DD_PWM_SEM_TIMEOUT/D_DD_PWM_SEM_NG/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Open(UINT32 ch, INT32 tmout)
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (tmout < D_DDIM_USER_SEM_WAIT_FEVR) {
		Ddim_Assertion(("PWM: input param error. tmout = %x\n", tmout));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	if( tmout == D_DDIM_USER_SEM_WAIT_POL ) {
		ercd = DDIM_User_Pol_Sem( SID_DD_PWM(ch) );
	}
	else{
		ercd = DDIM_User_Twai_Sem( SID_DD_PWM(ch), (DDIM_USER_TMO)tmout );
	}

	if( ercd != D_DDIM_USER_E_OK ) {
		if( D_DDIM_USER_E_TMOUT == ercd ) {
			return D_DD_PWM_SEM_TIMEOUT;
		}
		return D_DD_PWM_SEM_NG;
	}
	return D_DDIM_OK;
}


/**
 * @brief  The exclusive control of control Channel it is released.
 * @param  UINT32 ch
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Close(UINT32 ch)
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	ercd = DDIM_User_Sig_Sem( SID_DD_PWM(ch) );

	if( D_DDIM_USER_E_OK != ercd ) {
		return D_DD_PWM_SEM_NG;
	}
	return D_DDIM_OK;
}


/**
 * @brief  The PWM channel is exclusively controlled for Timer E.
 * @param  UINT32 ch
 * @param  INT32  tmout
 * @return INT32  D_DDIM_OK/D_DD_PWM_SEM_TIMEOUT/D_DD_PWM_SEM_NG/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Open_Timer_E(UINT32 ch, INT32 tmout)
{
	UINT32			ch_step, release_ch;
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if ((ch != 0) && (ch != 4) && (ch != 8) && (ch != 12)) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (tmout < D_DDIM_USER_SEM_WAIT_FEVR) {
		Ddim_Assertion(("PWM: input param error. tmout = %x\n", tmout));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	for( ch_step = ch; ch_step < ch + D_DD_PWM_CH_NUM_IN_BLOCK; ch_step++ ){
		if( D_DDIM_USER_SEM_WAIT_POL == tmout ) {
			ercd = DDIM_User_Pol_Sem( SID_DD_PWM(ch_step) );
		}
		else{
			ercd = DDIM_User_Twai_Sem( SID_DD_PWM(ch_step), (DDIM_USER_TMO)tmout );
		}

		if( D_DDIM_USER_E_OK != ercd ) {
			for ( release_ch = ch; release_ch < ch_step; release_ch++ ){
				if( DDIM_User_Sig_Sem( SID_DD_PWM(release_ch) ) != D_DDIM_USER_E_OK ){
					return D_DD_PWM_SEM_NG;
				}
			}
			if( D_DDIM_USER_E_TMOUT == ercd ) {
				return D_DD_PWM_SEM_TIMEOUT;
			}
			return D_DD_PWM_SEM_NG;
		}
	}

	return D_DDIM_OK;
}


/**
 * @brief  The exclusive control of control Channel it is released for Timer E.
 * @param  UINT32 ch
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Close_Timer_E(UINT32 ch)
{
	UINT32			ch_step;
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if ((ch != 0) && (ch != 4) && (ch != 8) && (ch != 12)) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	for( ch_step = ch; ch_step < ch + D_DD_PWM_CH_NUM_IN_BLOCK; ch_step++ ){
		ercd = DDIM_User_Sig_Sem( SID_DD_PWM(ch_step) );
		if( D_DDIM_USER_E_OK != ercd ) {
			return D_DD_PWM_SEM_NG;
		}
	}
	return D_DDIM_OK;
}


/**
 * @brief  The PWM operational condition of a control channel is set.
 * @param  UINT32 ch, T_DD_PWM_CTRL_CMN* pwm_ctrl_cmn
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Ctrl_Common(UINT32 ch, T_DD_PWM_CTRL_CMN const* const pwm_ctrl_cmn)
{
	const UINT32 ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */
	volatile union io_pwm_pcn* pcn_addr;
	union io_pwm_pcn pcn_tmp;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (0 == pwm_ctrl_cmn) {
		Ddim_Assertion(("PWM input param error. [pwm_ctrl_cmn] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	gDD_PWM_Ctrl[ch].clk_sel		= pwm_ctrl_cmn->clk_sel;
	gDD_PWM_Ctrl[ch].int_enable		= pwm_ctrl_cmn->int_enable;
	gDD_PWM_Ctrl[ch].pCallBack		= pwm_ctrl_cmn->pCallBack;

	if (pwm_ctrl_cmn->tim_e_enable) {
		pcn_addr = (union io_pwm_pcn*)&IO_PERI.PWM[blk].PCNE;
	}
	else {
		pcn_addr = dd_pwm_pcn_addr(ch);
	}

	pcn_tmp.word = pcn_addr->word;
	pcn_tmp.bit.__CNTE	= 0;									/* timer count enable	@PCNH	*/
	pcn_tmp.bit.__MDSE	= pwm_ctrl_cmn->drv_mode;				/* PWM or One-Shot 		@PCNH	*/
	pcn_tmp.bit.__RTRG	= pwm_ctrl_cmn->restart_trig;			/* restart enable		@PCNH	*/
	pcn_tmp.bit.__CKS	= pwm_ctrl_cmn->clk_sel;				/* clock select			@PCNH	*/
	pcn_tmp.bit.__PGMS	= pwm_ctrl_cmn->out_mask;				/* output level mask	@PCNH	*/
	pcn_tmp.bit.__EGS	= pwm_ctrl_cmn->edge;					/* edge enable select	@PCNH	*/
	pcn_tmp.bit.__IREN	= 0;									/* interrupt enable		@PCNH	*/
	pcn_tmp.bit.__IRQF	= 0;									/* interrupt flag		@PCNH	*/
	pcn_tmp.bit.__IRS	= pwm_ctrl_cmn->trigger;				/* interrupt factor		@PCNH	*/
	pcn_tmp.bit.__OSEL	= pwm_ctrl_cmn->polarity;				/* output polarity		@PCNH	*/
	pcn_addr->word = pcn_tmp.word;

	IO_PERI.PWM[blk].GCN2.bit.TMEEN	= pwm_ctrl_cmn->tim_e_enable;	/* timer E enable		@GCN2	*/
	IO_PERI.PWM[blk].GCN2.bit.CHAS	= pwm_ctrl_cmn->ch_assign;		/* channel assignment	@GCN2	*/

	switch (ch_in_blk) {
		case 0:
			IO_PERI.PWM[blk].TSEL.bit.SEL0 = 0; 						/* switch factor			*/
			IO_PERI.PWM[blk].GCN1.bit.TSEL0 = pwm_ctrl_cmn->in_trigger; /* trigger input factor		*/
			IO_PERI.PWM[blk].PREV.bit.PREVE0 = 0;						/* reverse register			*/
			break;

		case 1:
			IO_PERI.PWM[blk].TSEL.bit.SEL1 = 0; 						/* switch factor			*/
			IO_PERI.PWM[blk].GCN1.bit.TSEL1 = pwm_ctrl_cmn->in_trigger; /* trigger input factor		*/
			IO_PERI.PWM[blk].PREV.bit.PREVE1 = 0;						/* reverse register			*/
			break;

		case 2:
			IO_PERI.PWM[blk].TSEL.bit.SEL2 = 0; 						/* switch factor			*/
			IO_PERI.PWM[blk].GCN1.bit.TSEL2 = pwm_ctrl_cmn->in_trigger; /* trigger input factor		*/
			IO_PERI.PWM[blk].PREV.bit.PREVE2 = 0;						/* reverse register			*/
			break;

		case 3:
			IO_PERI.PWM[blk].TSEL.bit.SEL3 = 0; 						/* switch factor			*/
			IO_PERI.PWM[blk].GCN1.bit.TSEL3 = pwm_ctrl_cmn->in_trigger; /* trigger input factor		*/
			IO_PERI.PWM[blk].PREV.bit.PREVE3 = 0;						/* reverse register			*/
			break;

		default:
			break;
	}

	return D_DDIM_OK;
}



/**
 * @brief  The PWM operational condition of a control channel is set.
 * @param  UINT32 ch, T_DD_PWM_CTRL_CMN* pwm_ctrl_cmn, UCHAR prev_en
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Ctrl_Common2(UINT32 ch, T_DD_PWM_CTRL_CMN const* const pwm_ctrl_cmn, UCHAR prev_en)
{
	const UINT32 ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */
	volatile union io_pwm_pcn* pcn_addr;
	union io_pwm_pcn pcn_tmp;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (0 == pwm_ctrl_cmn) {
		Ddim_Assertion(("PWM input param error. [pwm_ctrl_cmn] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (prev_en > D_DD_PWM_PREV_ON) {
		Ddim_Assertion(("PWM input param error. [prev_en] = %d\n", prev_en));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	gDD_PWM_Ctrl[ch].clk_sel		= pwm_ctrl_cmn->clk_sel;
	gDD_PWM_Ctrl[ch].int_enable		= pwm_ctrl_cmn->int_enable;
	gDD_PWM_Ctrl[ch].pCallBack		= pwm_ctrl_cmn->pCallBack;

	if (pwm_ctrl_cmn->tim_e_enable) {
		pcn_addr = (union io_pwm_pcn*)&IO_PERI.PWM[blk].PCNE;
	}
	else {
		pcn_addr = dd_pwm_pcn_addr(ch);
	}

	pcn_tmp.word = pcn_addr->word;
	pcn_tmp.bit.__CNTE	= 0;									/* timer count enable	@PCNH	*/
	pcn_tmp.bit.__MDSE	= pwm_ctrl_cmn->drv_mode;				/* PWM or One-Shot 		@PCNH	*/
	pcn_tmp.bit.__RTRG	= pwm_ctrl_cmn->restart_trig;			/* restart enable		@PCNH	*/
	pcn_tmp.bit.__CKS	= pwm_ctrl_cmn->clk_sel;				/* clock select			@PCNH	*/
	pcn_tmp.bit.__PGMS	= pwm_ctrl_cmn->out_mask;				/* output level mask	@PCNH	*/
	pcn_tmp.bit.__EGS	= pwm_ctrl_cmn->edge;					/* edge enable select	@PCNH	*/
	pcn_tmp.bit.__IREN	= 0;									/* interrupt enable		@PCNH	*/
	pcn_tmp.bit.__IRQF	= 0;									/* interrupt flag		@PCNH	*/
	pcn_tmp.bit.__IRS	= pwm_ctrl_cmn->trigger;				/* interrupt factor		@PCNH	*/
	pcn_tmp.bit.__OSEL	= pwm_ctrl_cmn->polarity;				/* output polarity		@PCNH	*/
	pcn_addr->word = pcn_tmp.word;

	IO_PERI.PWM[blk].GCN2.bit.TMEEN	= pwm_ctrl_cmn->tim_e_enable;	/* timer E enable		@GCN2	*/
	IO_PERI.PWM[blk].GCN2.bit.CHAS	= pwm_ctrl_cmn->ch_assign;		/* channel assignment	@GCN2	*/

	switch (ch_in_blk) {
		case 0:
			IO_PERI.PWM[blk].TSEL.bit.SEL0 = 0; 	
//			IO_PERI.PWM[blk].TSEL.bit.SEL0 = 1; 						/* switch factor			*/
			IO_PERI.PWM[blk].GCN1.bit.TSEL0 = pwm_ctrl_cmn->in_trigger; /* trigger input factor		*/

			if(prev_en==D_DD_PWM_PREV_ON){
				IO_PERI.PWM[blk].PREV.bit.PREVE0 = 1;						/* reverse register			*/
			}
			else{
				IO_PERI.PWM[blk].PREV.bit.PREVE0 = 0;						/* reverse register			*/
			}
			break;

		case 1:
			IO_PERI.PWM[blk].TSEL.bit.SEL1 = 0; 
//			IO_PERI.PWM[blk].TSEL.bit.SEL1 = 1; 						/* switch factor			*/
			IO_PERI.PWM[blk].GCN1.bit.TSEL1 = pwm_ctrl_cmn->in_trigger; /* trigger input factor		*/
			if(prev_en==D_DD_PWM_PREV_ON){
				IO_PERI.PWM[blk].PREV.bit.PREVE1 = 1;						/* reverse register			*/
			}
			else{
				IO_PERI.PWM[blk].PREV.bit.PREVE1 = 0;						/* reverse register			*/
			}
			break;

		case 2:
			IO_PERI.PWM[blk].TSEL.bit.SEL2 = 0; 	
//			IO_PERI.PWM[blk].TSEL.bit.SEL2 = 1; 						/* switch factor			*/
			IO_PERI.PWM[blk].GCN1.bit.TSEL2 = pwm_ctrl_cmn->in_trigger; /* trigger input factor		*/
			if(prev_en==D_DD_PWM_PREV_ON){
				IO_PERI.PWM[blk].PREV.bit.PREVE2 = 1;						/* reverse register			*/
			}
			else{
				IO_PERI.PWM[blk].PREV.bit.PREVE2 = 0;						/* reverse register			*/
			}
			break;

		case 3:
			IO_PERI.PWM[blk].TSEL.bit.SEL3 = 0; 
//			IO_PERI.PWM[blk].TSEL.bit.SEL3 = 1; 						/* switch factor			*/
			IO_PERI.PWM[blk].GCN1.bit.TSEL3 = pwm_ctrl_cmn->in_trigger; /* trigger input factor		*/
			if(prev_en==D_DD_PWM_PREV_ON){
				IO_PERI.PWM[blk].PREV.bit.PREVE3 = 1;						/* reverse register			*/
			}
			else{
				IO_PERI.PWM[blk].PREV.bit.PREVE3 = 0;						/* reverse register			*/
			}
			break;

		default:
			break;
	}

	return D_DDIM_OK;
}

/**
 * @brief  The PWM operational condition of a control channel is get.
 * @param  UINT32 ch, T_DD_PWM_CTRL_CMN* pwm_ctrl_cmn
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Get_Ctrl_Common(UINT32 ch, T_DD_PWM_CTRL_CMN* const pwm_ctrl_cmn)
{
	const UINT32 ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */
	volatile union io_pwm_pcn* pcn_addr;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (0 == pwm_ctrl_cmn) {
		Ddim_Assertion(("PWM input param error. [pwm_ctrl_cmn] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	if (pwm_ctrl_cmn->tim_e_enable) {
		pcn_addr = (union io_pwm_pcn*)&IO_PERI.PWM[blk].PCNE;
	}
	else {
		pcn_addr = dd_pwm_pcn_addr(ch);
	}

	pwm_ctrl_cmn->drv_mode		= pcn_addr->bit.__MDSE;
	pwm_ctrl_cmn->restart_trig	= pcn_addr->bit.__RTRG;
	pwm_ctrl_cmn->clk_sel		= pcn_addr->bit.__CKS;
	pwm_ctrl_cmn->out_mask		= pcn_addr->bit.__PGMS;
	pwm_ctrl_cmn->edge			= pcn_addr->bit.__EGS;
	pwm_ctrl_cmn->int_enable	= gDD_PWM_Ctrl[ch].int_enable;
	pwm_ctrl_cmn->trigger		= pcn_addr->bit.__IRS;
	pwm_ctrl_cmn->polarity		= pcn_addr->bit.__OSEL;
	switch (ch_in_blk) {
		case 0:
			pwm_ctrl_cmn->in_trigger = IO_PERI.PWM[blk].GCN1.bit.TSEL0;
			break;
		case 1:
			pwm_ctrl_cmn->in_trigger = IO_PERI.PWM[blk].GCN1.bit.TSEL1;
			break;
		case 2:
			pwm_ctrl_cmn->in_trigger = IO_PERI.PWM[blk].GCN1.bit.TSEL2;
			break;
		case 3:
			pwm_ctrl_cmn->in_trigger = IO_PERI.PWM[blk].GCN1.bit.TSEL3;
			break;
		default:
			break;
	}
	pwm_ctrl_cmn->tim_e_enable	= IO_PERI.PWM[blk].GCN2.bit.TMEEN;
	pwm_ctrl_cmn->ch_assign		= IO_PERI.PWM[blk].GCN2.bit.CHAS;
	pwm_ctrl_cmn->pCallBack		= gDD_PWM_Ctrl[ch].pCallBack;

	return D_DDIM_OK;
}

/**
 * @brief  The PWM Timer ABCD operational condition of a control channel is set.
 * @param  UINT32 ch, T_DD_PWM_CTRL_TIMER_ABCD* pwm_ctrl_timer_abcd
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Ctrl_Timer_ABCD(UINT32 ch, T_DD_PWM_CTRL_TIMER_ABCD const* const pwm_ctrl_timer_abcd)
{
	const UINT32 ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */
	volatile union io_pwm_pcn* pcn_addr;
	union io_pwm_pcn pcn_tmp;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (0 == pwm_ctrl_timer_abcd) {
		Ddim_Assertion(("PWM input param error. [pwm_ctrl_timer_abcd] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	pcn_addr = dd_pwm_pcn_addr(ch);

	gDD_PWM_Ctrl[ch].cyc_num		= pwm_ctrl_timer_abcd->cyc_num;
	gDD_PWM_Ctrl[ch].duty_num		= pwm_ctrl_timer_abcd->duty_num;

	switch (ch_in_blk) {
		case 0:
			IO_PERI.PWM[blk].PCSRA = pwm_ctrl_timer_abcd->cyc_num;			/* number of cycle			*/
			IO_PERI.PWM[blk].PDUTA = pwm_ctrl_timer_abcd->duty_num; 		/* number of duty			*/
			IO_PERI.PWM[blk].GCN2.bit.STP0 = 0;
			break;

		case 1:
			IO_PERI.PWM[blk].PCSRB = pwm_ctrl_timer_abcd->cyc_num;			/* number of cycle			*/
			IO_PERI.PWM[blk].PDUTB = pwm_ctrl_timer_abcd->duty_num; 		/* number of duty			*/
			IO_PERI.PWM[blk].GCN2.bit.STP1 = 0;
			break;

		case 2:
			IO_PERI.PWM[blk].PCSRC = pwm_ctrl_timer_abcd->cyc_num;			/* number of cycle			*/
			IO_PERI.PWM[blk].PDUTC = pwm_ctrl_timer_abcd->duty_num; 		/* number of duty			*/
			IO_PERI.PWM[blk].GCN2.bit.STP2 = 0;
			break;

		case 3:
			IO_PERI.PWM[blk].PCSRD = pwm_ctrl_timer_abcd->cyc_num;			/* number of cycle			*/
			IO_PERI.PWM[blk].PDUTD = pwm_ctrl_timer_abcd->duty_num; 		/* number of duty			*/
			IO_PERI.PWM[blk].GCN2.bit.STP3 = 0;
			break;

		default:
			break;
	}



	pcn_tmp.word = pcn_addr->word;
	pcn_tmp.bit.__CNTE	= 1;											/* timer count enable	@PCNH	*/
	pcn_tmp.bit.__IRQF	= 1;											/* interrupt flag (not change)	*/
	pcn_addr->word = pcn_tmp.word;

	return D_DDIM_OK;
}


/**
 * @brief  The PWM Timer ABCD operational condition of a control channel is set.
 * @param  UINT32 ch, T_DD_PWM_CTRL_TIMER_ABCD_2ch* pwm_ctrl_timer_abcd_2ch
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Ctrl_Timer_ABCD_2ch(UINT32 ch, T_DD_PWM_CTRL_TIMER_ABCD_2ch const* const pwm_ctrl_timer_abcd_2ch)
{
	const UINT32 ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */
	volatile union io_pwm_pcn* pcn_addr;
	union io_pwm_pcn pcn_tmp;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (0 == pwm_ctrl_timer_abcd_2ch) {
		Ddim_Assertion(("PWM input param error. [pwm_ctrl_timer_abcd] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	pcn_addr = dd_pwm_pcn_addr(ch);

	gDD_PWM_Ctrl[ch].cyc_num		= pwm_ctrl_timer_abcd_2ch->cyc_num[0];
	gDD_PWM_Ctrl[ch].duty_num		= pwm_ctrl_timer_abcd_2ch->duty_num[0];

	switch (ch_in_blk) {
		case 0:
			IO_PERI.PWM[blk].PCSRA = pwm_ctrl_timer_abcd_2ch->cyc_num[0];			/* number of cycle			*/
			IO_PERI.PWM[blk].PDUTA = pwm_ctrl_timer_abcd_2ch->duty_num[0]; 			/* number of duty			*/
			IO_PERI.PWM[blk].PCSRRA = pwm_ctrl_timer_abcd_2ch->cyc_num[1];			/* number of cycle			*/
			IO_PERI.PWM[blk].PDUTRA = pwm_ctrl_timer_abcd_2ch->duty_num[1]; 		/* number of duty			*/
			IO_PERI.PWM[blk].GCN2.bit.STP0 = 0;
			IO_PERI.PWM[blk].PCHSET0.bit.FRCVAL0 = 0;								/* Output L in STOP state	*/
			IO_PERI.PWM[blk].PCHSET0.bit.FRCEN0 = 1;
			break;

		case 1:
			IO_PERI.PWM[blk].PCSRB = pwm_ctrl_timer_abcd_2ch->cyc_num[0];			/* number of cycle			*/
			IO_PERI.PWM[blk].PDUTB = pwm_ctrl_timer_abcd_2ch->duty_num[0]; 			/* number of duty			*/
			IO_PERI.PWM[blk].PCSRRB = pwm_ctrl_timer_abcd_2ch->cyc_num[1];			/* number of cycle			*/
			IO_PERI.PWM[blk].PDUTRB = pwm_ctrl_timer_abcd_2ch->duty_num[1]; 		/* number of duty			*/
			IO_PERI.PWM[blk].GCN2.bit.STP1 = 0;
			IO_PERI.PWM[blk].PCHSET0.bit.FRCVAL1 = 0;								/* Output L in STOP state	*/
			IO_PERI.PWM[blk].PCHSET0.bit.FRCEN1 = 1;
			break;

		case 2:
			IO_PERI.PWM[blk].PCSRC = pwm_ctrl_timer_abcd_2ch->cyc_num[0];			/* number of cycle			*/
			IO_PERI.PWM[blk].PDUTC = pwm_ctrl_timer_abcd_2ch->duty_num[0]; 			/* number of duty			*/
			IO_PERI.PWM[blk].PCSRRC = pwm_ctrl_timer_abcd_2ch->cyc_num[1];			/* number of cycle			*/
			IO_PERI.PWM[blk].PDUTRC = pwm_ctrl_timer_abcd_2ch->duty_num[1]; 		/* number of duty			*/
			IO_PERI.PWM[blk].GCN2.bit.STP2 = 0;
			IO_PERI.PWM[blk].PCHSET0.bit.FRCVAL2 = 0;								/* Output L in STOP state	*/
			IO_PERI.PWM[blk].PCHSET0.bit.FRCEN2 = 1;
			break;

		case 3:
			IO_PERI.PWM[blk].PCSRD = pwm_ctrl_timer_abcd_2ch->cyc_num[0];			/* number of cycle			*/
			IO_PERI.PWM[blk].PDUTD = pwm_ctrl_timer_abcd_2ch->duty_num[0]; 			/* number of duty			*/
			IO_PERI.PWM[blk].PCSRRD = pwm_ctrl_timer_abcd_2ch->cyc_num[1];			/* number of cycle			*/
			IO_PERI.PWM[blk].PDUTRD = pwm_ctrl_timer_abcd_2ch->duty_num[1]; 		/* number of duty			*/
			IO_PERI.PWM[blk].GCN2.bit.STP3 = 0;
			IO_PERI.PWM[blk].PCHSET0.bit.FRCVAL3 = 0;								/* Output L in STOP state	*/
			IO_PERI.PWM[blk].PCHSET0.bit.FRCEN3 = 1;
			break;

		default:
			break;
	}

	pcn_tmp.word = pcn_addr->word;
	pcn_tmp.bit.__CNTE	= 1;											/* timer count enable	@PCNH	*/
	pcn_tmp.bit.__IRQF	= 1;											/* interrupt flag (not change)	*/
	pcn_addr->word = pcn_tmp.word;

	return D_DDIM_OK;
}

/**
 * @brief  The PWM Timer E operational condition of a control channel is set.
 * @param  UINT32 ch, T_DD_PWM_CTRL_TIMER_E* pwm_ctrl_timer_e
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Ctrl_Timer_E(UINT32 ch, T_DD_PWM_CTRL_TIMER_E const* const pwm_ctrl_timer_e)
{
	union io_pwm_pintct0 pintct0_tmp;

	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (0 == pwm_ctrl_timer_e) {
		Ddim_Assertion(("PWM input param error. [pwm_ctrl_timer_e] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	IO_PERI.PWM[blk].PCHSET0.bit.HLD0 = pwm_ctrl_timer_e->hold[0];
	IO_PERI.PWM[blk].PCHSET0.bit.HLD1 = pwm_ctrl_timer_e->hold[1];
	IO_PERI.PWM[blk].PCHSET0.bit.HLD2 = pwm_ctrl_timer_e->hold[2];
	IO_PERI.PWM[blk].PCHSET0.bit.HLD3 = pwm_ctrl_timer_e->hold[3];

	IO_PERI.PWM[blk].PCHSET0.bit.FRCVAL0 = pwm_ctrl_timer_e->force_val[0];
	IO_PERI.PWM[blk].PCHSET0.bit.FRCVAL1 = pwm_ctrl_timer_e->force_val[1];
	IO_PERI.PWM[blk].PCHSET0.bit.FRCVAL2 = pwm_ctrl_timer_e->force_val[2];
	IO_PERI.PWM[blk].PCHSET0.bit.FRCVAL3 = pwm_ctrl_timer_e->force_val[3];

	IO_PERI.PWM[blk].PCHSET0.bit.FRCEN0 = pwm_ctrl_timer_e->force[0];
	IO_PERI.PWM[blk].PCHSET0.bit.FRCEN1 = pwm_ctrl_timer_e->force[1];
	IO_PERI.PWM[blk].PCHSET0.bit.FRCEN2 = pwm_ctrl_timer_e->force[2];
	IO_PERI.PWM[blk].PCHSET0.bit.FRCEN3 = pwm_ctrl_timer_e->force[3];

	IO_PERI.PWM[blk].PCSRE0 = pwm_ctrl_timer_e->steps_data[0].cyc_num;
	IO_PERI.PWM[blk].PCSRE1 = pwm_ctrl_timer_e->steps_data[1].cyc_num;
	IO_PERI.PWM[blk].PCSRE2 = pwm_ctrl_timer_e->steps_data[2].cyc_num;
	IO_PERI.PWM[blk].PCSRE3 = pwm_ctrl_timer_e->steps_data[3].cyc_num;
	IO_PERI.PWM[blk].PCSRE4 = pwm_ctrl_timer_e->steps_data[4].cyc_num;
	IO_PERI.PWM[blk].PCSRE5 = pwm_ctrl_timer_e->steps_data[5].cyc_num;
	IO_PERI.PWM[blk].PCSRE6 = pwm_ctrl_timer_e->steps_data[6].cyc_num;
	IO_PERI.PWM[blk].PCSRE7 = pwm_ctrl_timer_e->steps_data[7].cyc_num;

	IO_PERI.PWM[blk].PDUTE0 = pwm_ctrl_timer_e->steps_data[0].duty_num;
	IO_PERI.PWM[blk].PDUTE1 = pwm_ctrl_timer_e->steps_data[1].duty_num;
	IO_PERI.PWM[blk].PDUTE2 = pwm_ctrl_timer_e->steps_data[2].duty_num;
	IO_PERI.PWM[blk].PDUTE3 = pwm_ctrl_timer_e->steps_data[3].duty_num;
	IO_PERI.PWM[blk].PDUTE4 = pwm_ctrl_timer_e->steps_data[4].duty_num;
	IO_PERI.PWM[blk].PDUTE5 = pwm_ctrl_timer_e->steps_data[5].duty_num;
	IO_PERI.PWM[blk].PDUTE6 = pwm_ctrl_timer_e->steps_data[6].duty_num;
	IO_PERI.PWM[blk].PDUTE7 = pwm_ctrl_timer_e->steps_data[7].duty_num;

	IO_PERI.PWM[blk].PPALE0.bit.PPALE = pwm_ctrl_timer_e->steps_data[0].pulse_link.bit.pulse_num;
	IO_PERI.PWM[blk].PPALE1.bit.PPALE = pwm_ctrl_timer_e->steps_data[1].pulse_link.bit.pulse_num;
	IO_PERI.PWM[blk].PPALE2.bit.PPALE = pwm_ctrl_timer_e->steps_data[2].pulse_link.bit.pulse_num;
	IO_PERI.PWM[blk].PPALE3.bit.PPALE = pwm_ctrl_timer_e->steps_data[3].pulse_link.bit.pulse_num;
	IO_PERI.PWM[blk].PPALE4.bit.PPALE = pwm_ctrl_timer_e->steps_data[4].pulse_link.bit.pulse_num;
	IO_PERI.PWM[blk].PPALE5.bit.PPALE = pwm_ctrl_timer_e->steps_data[5].pulse_link.bit.pulse_num;
	IO_PERI.PWM[blk].PPALE6.bit.PPALE = pwm_ctrl_timer_e->steps_data[6].pulse_link.bit.pulse_num;
	IO_PERI.PWM[blk].PPALE7.bit.PPALE = pwm_ctrl_timer_e->steps_data[7].pulse_link.bit.pulse_num;

	IO_PERI.PWM[blk].PPALE0.bit.LINK = pwm_ctrl_timer_e->steps_data[0].pulse_link.bit.link;
	IO_PERI.PWM[blk].PPALE1.bit.LINK = pwm_ctrl_timer_e->steps_data[1].pulse_link.bit.link;
	IO_PERI.PWM[blk].PPALE2.bit.LINK = pwm_ctrl_timer_e->steps_data[2].pulse_link.bit.link;
	IO_PERI.PWM[blk].PPALE3.bit.LINK = pwm_ctrl_timer_e->steps_data[3].pulse_link.bit.link;
	IO_PERI.PWM[blk].PPALE4.bit.LINK = pwm_ctrl_timer_e->steps_data[4].pulse_link.bit.link;
	IO_PERI.PWM[blk].PPALE5.bit.LINK = pwm_ctrl_timer_e->steps_data[5].pulse_link.bit.link;
	IO_PERI.PWM[blk].PPALE6.bit.LINK = pwm_ctrl_timer_e->steps_data[6].pulse_link.bit.link;
	IO_PERI.PWM[blk].PPALE7.bit.LINK = pwm_ctrl_timer_e->steps_data[7].pulse_link.bit.link;

	IO_PERI.PWM[blk].PCSRU0 = pwm_ctrl_timer_e->under_run_data[0].cyc_num;
	IO_PERI.PWM[blk].PCSRU1 = pwm_ctrl_timer_e->under_run_data[1].cyc_num;
	IO_PERI.PWM[blk].PCSRU2 = pwm_ctrl_timer_e->under_run_data[2].cyc_num;
	IO_PERI.PWM[blk].PCSRU3 = pwm_ctrl_timer_e->under_run_data[3].cyc_num;

	IO_PERI.PWM[blk].PDUTU0 = pwm_ctrl_timer_e->under_run_data[0].duty_num;
	IO_PERI.PWM[blk].PDUTU1 = pwm_ctrl_timer_e->under_run_data[1].duty_num;
	IO_PERI.PWM[blk].PDUTU2 = pwm_ctrl_timer_e->under_run_data[2].duty_num;
	IO_PERI.PWM[blk].PDUTU3 = pwm_ctrl_timer_e->under_run_data[3].duty_num;

	IO_PERI.PWM[blk].PPALU0.bit.PPALU = pwm_ctrl_timer_e->under_run_data[0].pulse_link.bit.pulse_num;
	IO_PERI.PWM[blk].PPALU1.bit.PPALU = pwm_ctrl_timer_e->under_run_data[1].pulse_link.bit.pulse_num;
	IO_PERI.PWM[blk].PPALU2.bit.PPALU = pwm_ctrl_timer_e->under_run_data[2].pulse_link.bit.pulse_num;
	IO_PERI.PWM[blk].PPALU3.bit.PPALU = pwm_ctrl_timer_e->under_run_data[3].pulse_link.bit.pulse_num;

	IO_PERI.PWM[blk].PPALU0.bit.LINK = pwm_ctrl_timer_e->under_run_data[0].pulse_link.bit.link;
	IO_PERI.PWM[blk].PPALU1.bit.LINK = pwm_ctrl_timer_e->under_run_data[1].pulse_link.bit.link;
	IO_PERI.PWM[blk].PPALU2.bit.LINK = pwm_ctrl_timer_e->under_run_data[2].pulse_link.bit.link;
	IO_PERI.PWM[blk].PPALU3.bit.LINK = pwm_ctrl_timer_e->under_run_data[3].pulse_link.bit.link;

	IO_PERI.PWM[blk].PCHSET1.word		= (((USHORT)pwm_ctrl_timer_e->empty_dma_req) << 8)
										| ((USHORT)pwm_ctrl_timer_e->undr_run_enable);
	pintct0_tmp.word = IO_PERI.PWM[blk].PINTCT0.word;
	pintct0_tmp.bit.__UIE	= pwm_ctrl_timer_e->undr_run_int_err_enable;
	pintct0_tmp.bit.__FIE	= pwm_ctrl_timer_e->force_stop_int_enable;
	pintct0_tmp.bit.__AIE	= pwm_ctrl_timer_e->auto_stop_int_enable;
	pintct0_tmp.bit.__UIS	= 0;				/* underrun error (not change)	*/
	pintct0_tmp.bit.__FIS	= 0;				/* Force stop (not change)		*/
	pintct0_tmp.bit.__AIS	= 0;				/* Automatic stop (not change)	*/
	IO_PERI.PWM[blk].PINTCT0.word = pintct0_tmp.word;
	IO_PERI.PWM[blk].PINTCT1.bit.DRQE	= pwm_ctrl_timer_e->dma_int_enable;

	return D_DDIM_OK;
}

/**
 * @brief  The PWM of a control channel is started.
 * @param  UINT32 ch
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Start(UINT32 ch)
{
	const UINT32 ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */
	volatile union io_pwm_pcn* pcn_addr;
	union io_pwm_pcn pcn_tmp;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	if( IO_PERI.PWM[blk].GCN2.bit.TMEEN ) {
		USHORT temp;

		IO_PERI.PWM[blk].GCN2.bit.STP0 = 0;
		temp = IO_PERI.PWM[blk].PINTCT0.word;
		IO_PERI.PWM[blk].PINTCT0.word = temp;						/* clear UIS, FIS, AIS			*/
		pcn_tmp.word = IO_PERI.PWM[blk].PCNE.word;
		pcn_tmp.bit.__CNTE	= 1;									/* timer count enable	@PCNH	*/
		pcn_tmp.bit.__IRQF	= 1;									/* interrupt flag (not change)	*/
		pcn_tmp.bit.__IREN	= gDD_PWM_Ctrl[ch].int_enable;			/* interrupt enable		@PCNH	*/
		IO_PERI.PWM[blk].PCNE.word = pcn_tmp.word;
		IO_PERI.PWM[blk].GCN2.bit.EN0 = 1;							/* trigger on					*/
	}
	else {
		pcn_addr = dd_pwm_pcn_addr(ch);
		pcn_tmp.word = pcn_addr->word;
		pcn_tmp.bit.__IREN	= gDD_PWM_Ctrl[ch].int_enable; 			/* interrupt enable		@PCNH	*/
		pcn_tmp.bit.__IRQF	= 1;									/* interrupt flag (not change)	*/
		pcn_addr->word = pcn_tmp.word;

		/* trigger on					*/
		switch (ch_in_blk) {
			case 0:
				IO_PERI.PWM[blk].GCN2.bit.EN0 = 1;
				break;

			case 1:
				IO_PERI.PWM[blk].GCN2.bit.EN1 = 1;
				break;

			case 2:
				IO_PERI.PWM[blk].GCN2.bit.EN2 = 1;
				break;

			case 3:
				IO_PERI.PWM[blk].GCN2.bit.EN3 = 1;
				break;

			default:
				break;
		}
	}

	dd_pwm_dsb();

	return D_DDIM_OK;
}



/**
 * @brief  The PWM of a control channel is started.
 * @param  UINT32 ch_a, UINT32 ch_b
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_2ch_Start(UINT32 ch_a, UINT32 ch_b)
{
	const UINT32 ch_in_blk_a = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch_a;
	const UINT32 ch_in_blk_b = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch_b;
	const UINT32 blk_a = ch_a >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */
	const UINT32 blk_b = ch_b >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */
	volatile union io_pwm_pcn* pcn_addr_a;
	volatile union io_pwm_pcn* pcn_addr_b;
	union io_pwm_pcn pcn_a_tmp;
	union io_pwm_pcn pcn_b_tmp;

#ifdef CO_PARAM_CHECK
	if (ch_a >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. ch_a[ch] = %d\n", ch_a));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (ch_b >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. ch_b[ch] = %d\n", ch_b));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif



	if( IO_PERI.PWM[blk_a].GCN2.bit.TMEEN ) {
		USHORT temp;

		IO_PERI.PWM[blk_a].GCN2.bit.STP0 = 0;
		temp = IO_PERI.PWM[blk_a].PINTCT0.word;
		IO_PERI.PWM[blk_a].PINTCT0.word = temp;						/* clear UIS, FIS, AIS			*/
		pcn_a_tmp.word = IO_PERI.PWM[blk_a].PCNE.word;
		pcn_a_tmp.bit.__CNTE	= 1;									/* timer count enable	@PCNH	*/
		pcn_a_tmp.bit.__IRQF	= 1;									/* interrupt flag (not change)	*/
		pcn_a_tmp.bit.__IREN	= gDD_PWM_Ctrl[ch_a].int_enable;			/* interrupt enable		@PCNH	*/
		IO_PERI.PWM[blk_a].PCNE.word = pcn_a_tmp.word;
	}
	else {
		pcn_addr_a = dd_pwm_pcn_addr(ch_a);
		pcn_a_tmp.word = pcn_addr_a->word;
		pcn_a_tmp.bit.__IREN	= gDD_PWM_Ctrl[ch_a].int_enable; 			/* interrupt enable		@PCNH	*/
		pcn_a_tmp.bit.__IRQF	= 1;									/* interrupt flag (not change)	*/
		pcn_addr_a->word = pcn_a_tmp.word;
	}

	if( IO_PERI.PWM[blk_b].GCN2.bit.TMEEN ) {
		USHORT temp;

		IO_PERI.PWM[blk_b].GCN2.bit.STP0 = 0;
		temp = IO_PERI.PWM[blk_b].PINTCT0.word;
		IO_PERI.PWM[blk_b].PINTCT0.word = temp;						/* clear UIS, FIS, AIS			*/
		pcn_b_tmp.word = IO_PERI.PWM[blk_b].PCNE.word;
		pcn_b_tmp.bit.__CNTE	= 1;									/* timer count enable	@PCNH	*/
		pcn_b_tmp.bit.__IRQF	= 1;									/* interrupt flag (not change)	*/
		pcn_b_tmp.bit.__IREN	= gDD_PWM_Ctrl[ch_b].int_enable;			/* interrupt enable		@PCNH	*/
		IO_PERI.PWM[blk_b].PCNE.word = pcn_b_tmp.word;
	}
	else {
		pcn_addr_b = dd_pwm_pcn_addr(ch_b);
		pcn_b_tmp.word = pcn_addr_b->word;
		pcn_b_tmp.bit.__IREN	= gDD_PWM_Ctrl[ch_b].int_enable; 			/* interrupt enable		@PCNH	*/
		pcn_b_tmp.bit.__IRQF	= 1;									/* interrupt flag (not change)	*/
		pcn_addr_b->word = pcn_b_tmp.word;
	}


	if( (IO_PERI.PWM[blk_a].GCN2.bit.TMEEN == 1) && (IO_PERI.PWM[blk_b].GCN2.bit.TMEEN == 1)) {
		IO_PERI.PWM[blk_a].GCN2.bit.EN0 = 1;							/* trigger on					*/
		IO_PERI.PWM[blk_b].GCN2.bit.EN0 = 1;							/* trigger on					*/
	}
	else {

		/* trigger on					*/
		switch (ch_in_blk_a) {
			case 0:
				IO_PERI.PWM[blk_a].GCN2.bit.EN0 = 1;
				break;

			case 1:
				IO_PERI.PWM[blk_a].GCN2.bit.EN1 = 1;
				break;

			case 2:
				IO_PERI.PWM[blk_a].GCN2.bit.EN2 = 1;
				break;

			case 3:
				IO_PERI.PWM[blk_a].GCN2.bit.EN3 = 1;
				break;

			default:
				break;
		}

		/* trigger on					*/
		switch (ch_in_blk_b) {
			case 0:
				IO_PERI.PWM[blk_b].GCN2.bit.EN0 = 1;
				break;

			case 1:
				IO_PERI.PWM[blk_b].GCN2.bit.EN1 = 1;
				break;

			case 2:
				IO_PERI.PWM[blk_b].GCN2.bit.EN2 = 1;
				break;

			case 3:
				IO_PERI.PWM[blk_b].GCN2.bit.EN3 = 1;
				break;

			default:
				break;
		}
	}


	dd_pwm_dsb();

	return D_DDIM_OK;

}



/**
 * @brief  The PWM of a control channel is stopped.
 * @param  UINT32 ch
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Stop(UINT32 ch)
{
	INT32 ret;
	const UINT32 ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */
	volatile union io_pwm_pcn* pcn_addr;
	union io_pwm_pcn pcn_tmp;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	if( IO_PERI.PWM[blk].GCN2.bit.TMEEN ) {
		IO_PERI.PWM[blk].GCN2.bit.STP0 = 1;
		dd_pwm_dsb();

		IO_PERI.PWM[blk].GCN2.bit.EN0 = 0;

		pcn_tmp.word = IO_PERI.PWM[blk].PCNE.word;
		pcn_tmp.bit.__IREN	= 0;									/* interrupt enable		@PCNH	*/
		pcn_tmp.bit.__CNTE	= 0;									/* timer count enable	@PCNH	*/
		pcn_tmp.bit.__IRQF	= 1;									/* interrupt flag (not change)	*/
		IO_PERI.PWM[blk].PCNE.word = pcn_tmp.word;
		dd_pwm_dsb();

		if (gDD_PWM_Ctrl[ch].dma_enable){
			ret = Dd_HDMAC0_Stop( gDD_PWM_Ctrl[ch].dma_ch );
			if(ret != D_DDIM_OK){
				Ddim_Print(("Error Dd_HDMAC0_Stop()!! ret = 0x%x\n", ret));
			}
			ret = Dd_HDMAC0_Close( gDD_PWM_Ctrl[ch].dma_ch );
			if(ret != D_DDIM_OK){
				Ddim_Print(("Error Dd_HDMAC0_Close()!! ret = 0x%x\n", ret));
			}
			gDD_PWM_Ctrl[ch].dma_enable = 0;
		}
	}
	else {
		pcn_addr = dd_pwm_pcn_addr(ch);

		switch (ch_in_blk) {
			case 0:
				IO_PERI.PWM[blk].GCN2.bit.STP0 = 1;
				dd_pwm_dsb();
				IO_PERI.PWM[blk].GCN2.bit.EN0 = 0;
				break;

			case 1:
				IO_PERI.PWM[blk].GCN2.bit.STP1 = 1;
				dd_pwm_dsb();
				IO_PERI.PWM[blk].GCN2.bit.EN1 = 0;
				break;

			case 2:
				IO_PERI.PWM[blk].GCN2.bit.STP2 = 1;
				dd_pwm_dsb();
				IO_PERI.PWM[blk].GCN2.bit.EN2 = 0;
				break;

			case 3:
				IO_PERI.PWM[blk].GCN2.bit.STP3 = 1;
				dd_pwm_dsb();
				IO_PERI.PWM[blk].GCN2.bit.EN3 = 0;
				break;

			default:
				break;
		}

		pcn_tmp.word = pcn_addr->word;
		pcn_tmp.bit.__IREN	= 0;									/* interrupt enable		@PCNH	*/
		pcn_tmp.bit.__CNTE	= 0;									/* timer count enable	@PCNH	*/
		pcn_tmp.bit.__IRQF	= 1;									/* interrupt flag (not change)	*/
		pcn_addr->word = pcn_tmp.word;

		dd_pwm_dsb();
	}

	return D_DDIM_OK;
}


INT32 Dd_PWM_Get_Timer(UINT32 ch, USHORT* timer_val, USHORT* timer_e_val)
{
	const UINT32 ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	
	if (0 == timer_val) {
		Ddim_Assertion(("PWM input param error. [timer_val] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}

	if (0 == timer_e_val) {
		Ddim_Assertion(("PWM input param error. [timer_e_val] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	switch( ch_in_blk ) {
		case 0:
			*timer_val = IO_PERI.PWM[blk].PTMRA;
			break;

		case 1:
			*timer_val = IO_PERI.PWM[blk].PTMRB;
			break;

		case 2:
			*timer_val = IO_PERI.PWM[blk].PTMRC;
			break;

		case 3:
			*timer_val = IO_PERI.PWM[blk].PTMRD;
			break;

		default:
			break;
	}
	*timer_e_val = IO_PERI.PWM[blk].PTMRE;

	return D_DDIM_OK;
}


INT32 Dd_PWM_Get_Pulse_Num(UINT32 ch, ULONG* pulse_num)
{
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	
	if (0 == pulse_num) {
		Ddim_Assertion(("PWM input param error. [pulse_num] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	*pulse_num = (IO_PERI.PWM[blk].STMRH << 16) | IO_PERI.PWM[blk].STMRL;

	return D_DDIM_OK;
}

INT32 Dd_PWM_Get_Interrupt_Flag(UINT32 ch, USHORT* int_flg_ctrl_0, USHORT* int_flg_ctrl_1)
{
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}

	if (0 == int_flg_ctrl_0) {
		Ddim_Assertion(("PWM input param error. [int_flg_ctrl_0] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}

	if (0 == int_flg_ctrl_1) {
		Ddim_Assertion(("PWM input param error. [int_flg_ctrl_1] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	*int_flg_ctrl_0 = IO_PERI.PWM[blk].PINTCT0.word;
	*int_flg_ctrl_1 = IO_PERI.PWM[blk].PINTCT1.word;

	return D_DDIM_OK;
}

INT32 Dd_PWM_Set_Interrupt_Flag(UINT32 ch, USHORT int_flg_ctrl_0, USHORT int_flg_ctrl_1)
{
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	IO_PERI.PWM[blk].PINTCT0.word = int_flg_ctrl_0;
	IO_PERI.PWM[blk].PINTCT1.word = int_flg_ctrl_1;
	dd_pwm_dsb();

	return D_DDIM_OK;
}

INT32 Dd_PWM_Get_Channel_Assign(UINT32 ch, UCHAR* ch_assign)
{
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	
	if (0 == ch_assign) {
		Ddim_Assertion(("PWM input param error. [ch_assign] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	*ch_assign = IO_PERI.PWM[blk].GCN2.bit.CHAS;

	return D_DDIM_OK;
}

INT32 Dd_PWM_Set_Channel_Assign(UINT32 ch, UCHAR ch_assign)
{
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	
	if (ch_assign > D_DD_PWM_GCN_CHAS_CBAD) {
		Ddim_Assertion(("PWM input param error. [ch_assign] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	IO_PERI.PWM[blk].GCN2.bit.CHAS = ch_assign;

	return D_DDIM_OK;
}

/**
 * @brief  Get Interrupt Status of PWM. (Not PWM_STP)
 * @param  VOID
 * @return TRUE:Interrupt request occur. FALSE:No interrupt request
 */
BOOL Dd_PWM_Get_Int_Stat(UINT32 ch)
{
	volatile union io_pwm_pcn* pcn_addr;
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return FALSE;
	}
#endif

	pcn_addr = dd_pwm_pcn_addr( ch );
	if(pcn_addr == NULL)
	{
		return FALSE;
	}
	if ((pcn_addr->bit.__IREN == 1) && (pcn_addr->bit.__IRQF == 1)){
		return TRUE;
	}

	pcn_addr = (union io_pwm_pcn*) &IO_PERI.PWM[blk].PCNE;
	if ((pcn_addr->bit.__IREN == 1) && (pcn_addr->bit.__IRQF == 1)){
		return TRUE;
	}

	return FALSE;
}

/**
 * @brief  It is Interrupt Handler of PWM.
 * @param  VOID
 * @return VOID
 */
VOID Dd_PWM_Int_Handler(UINT32 ch)
{
	UINT32 ch_flg[4] = {0,0,0,0};
	int i, ch_max = D_DD_PWM_CH_NUM_MAX;
	volatile union io_pwm_pcn* pcn_addr;
	union io_pwm_pcn pcn_tmp;
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return;
	}
#endif

	if (IO_PERI.PWM[blk].PCNA.bit.__IRQF == 1){
		ch_max = ch;
		ch_flg[0] = 1;
	}
	if (IO_PERI.PWM[blk].PCNB.bit.__IRQF == 1){
		ch_max = ch + 1;
		ch_flg[1] = 1;
	}
	if (IO_PERI.PWM[blk].PCNC.bit.__IRQF == 1){
		ch_max = ch + 2;
		ch_flg[2] = 1;
	}
	if (IO_PERI.PWM[blk].PCND.bit.__IRQF == 1){
		ch_max = ch + 3;
		ch_flg[3] = 1;
	}
	if (IO_PERI.PWM[blk].PCNE.bit.__IRQF == 1){
		ch_max = ch;
		ch_flg[0] = 1;
	}


//	printf("Dd_PWM_Int_Handler ch= %d \n", ch);
//
//	if(ch==4){
////		FJ_Gpio_Set_Status(FJ_GPIO_PF2, 1 );
//		Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_PF2, 1);
//	}

	if(ch_max == D_DD_PWM_CH_NUM_MAX) {
		return;
	}
	for (i = 0; i < 4; i++){
		if (ch_flg[i] == 1) {
			pcn_addr = dd_pwm_pcn_addr(ch);

			if ((pcn_addr->bit.__IREN == 1) && (pcn_addr->bit.__IRQF == 1)){
				// Clear interrupt flag
				pcn_tmp.word = pcn_addr->word;
				pcn_tmp.bit.__IRQF = 0;
				pcn_addr->word = pcn_tmp.word;
				dd_pwm_dsb();

				if (gDD_PWM_Ctrl[ch].pCallBack) {
					(*gDD_PWM_Ctrl[ch].pCallBack)(ch, pcn_addr->bit.__IRS); // Call Back function
				}
			}
			pcn_addr = (union io_pwm_pcn*) &IO_PERI.PWM[blk].PCNE;
			if ((pcn_addr->bit.__IREN == 1) && (pcn_addr->bit.__IRQF == 1)){
				// Clear interrupt flag
				pcn_tmp.word = pcn_addr->word;
				pcn_tmp.bit.__IRQF = 0;
				pcn_addr->word = pcn_tmp.word;
				dd_pwm_dsb();

				if (gDD_PWM_Ctrl[ch].pCallBack) {
					(*gDD_PWM_Ctrl[ch].pCallBack)(ch, pcn_addr->bit.__IRS);
				}
			}
		}
		ch++;
		if (ch > ch_max) {
			break;
		}
	}

//	if(ch==4){
////		FJ_Gpio_Set_Status(FJ_GPIO_PF2, 0 );
//		Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_PF2, 0);
//	}

}
/**
 * @brief  It is Interrupt Handler of PWM_STP.
 * @param  VOID
 * @return VOID
 */
VOID Dd_PWM_Int_Handler_STP(UINT32 ch)
{
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */
	USHORT pintct_temp;
	INT32	ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return;
	}
#endif
	if (IO_PERI.PWM[blk].PINTCT0.word == 0x0000){
		return;
	}

	if (gDD_PWM_Ctrl[ch].dma_enable){
		ret = Dd_HDMAC0_Stop( gDD_PWM_Ctrl[ch].dma_ch );
		if (ret != D_DDIM_OK){
			Ddim_Print(("Dd_HDMAC0_Stop Error: %d\n", ret));
		}
		ret = Dd_HDMAC0_Close( gDD_PWM_Ctrl[ch].dma_ch );
		if (ret != D_DDIM_OK){
			Ddim_Print(("Dd_HDMAC0_Close Error: %d\n", ret));
		}
		gDD_PWM_Ctrl[ch].dma_enable = 0;
	}

	if ((IO_PERI.PWM[blk].PINTCT0.bit.__UIE == 1) && (IO_PERI.PWM[blk].PINTCT0.bit.__UIS == 1)){
		if (gDD_PWM_Ctrl[ch].pCallBack) {
			(*gDD_PWM_Ctrl[ch].pCallBack)(ch, D_DD_PWM_INT_STATUS_UNDER_RUN);
		}
	}
	if ((IO_PERI.PWM[blk].PINTCT0.bit.__FIE == 1) && (IO_PERI.PWM[blk].PINTCT0.bit.__FIS == 1)){
		if (gDD_PWM_Ctrl[ch].pCallBack) {
			(*gDD_PWM_Ctrl[ch].pCallBack)(ch, D_DD_PWM_INT_STATUS_FORCE_STOP);
		}
	}
	if ((IO_PERI.PWM[blk].PINTCT0.bit.__AIE == 1) && (IO_PERI.PWM[blk].PINTCT0.bit.__AIS == 1)){
		if (gDD_PWM_Ctrl[ch].pCallBack) {
			(*gDD_PWM_Ctrl[ch].pCallBack)(ch, D_DD_PWM_INT_STATUS_AUTO_STOP);
		}
	}
	// clear UIS, FIS, AIS
	pintct_temp = IO_PERI.PWM[blk].PINTCT0.word;
	IO_PERI.PWM[blk].PINTCT0.word = pintct_temp;
	dd_pwm_dsb();
}

/**
 * @brief  The Mask status is set. Only when the corresponding timer ABCD.
 * @param  UINT32 ch, USHORT mask
 * @return INT32 D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Set_Stopped_OutPut_Mask(UINT32 ch, UCHAR mask)
{
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	switch ((D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch) {
		case 0:
			IO_PERI.PWM[blk].GCN2.bit.MSK0 = mask;
			break;

		case 1:
			IO_PERI.PWM[blk].GCN2.bit.MSK1 = mask;
			break;

		case 2:
			IO_PERI.PWM[blk].GCN2.bit.MSK2 = mask;
			break;

		case 3:
			IO_PERI.PWM[blk].GCN2.bit.MSK3 = mask;
			break;

		default:
			return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	dd_pwm_dsb();

	return D_DDIM_OK;
}

/**
 * @brief  Get setting value of the mask. Only when the corresponding timer ABCD.
 * @param  UINT32 ch, USHORT mask
 * @return INT32 D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Get_Stopped_OutPut_Mask(UINT32 ch, UCHAR* mask)
{
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	
	if (0 == mask) {
		Ddim_Assertion(("PWM input param error. [mask] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	switch ((D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch) {
		case 0:
			*mask = IO_PERI.PWM[blk].GCN2.bit.MSK0;
			break;

		case 1:
			*mask = IO_PERI.PWM[blk].GCN2.bit.MSK1;
			break;

		case 2:
			*mask = IO_PERI.PWM[blk].GCN2.bit.MSK2;
			break;

		case 3:
			*mask = IO_PERI.PWM[blk].GCN2.bit.MSK3;
			break;

		default:
			return D_DD_PWM_INPUT_PARAM_ERROR;
	}

	return D_DDIM_OK;
}



/**
 * @brief  Get setting value of conter clock of TimerE
 * @param  UINT32 ch, UCHAR clock
 * @return INT32 D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Get_Clock_Sel_TimerE(UINT32 ch, UCHAR* clock)
{

	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	switch ((D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch) {
		case 0:
			*clock = IO_PERI.PWM[blk].PCNE.bit.__CKS;
			break;

		case 1:
			*clock = IO_PERI.PWM[blk].PCNE.bit.__CKS;
			break;

		case 2:
			*clock = IO_PERI.PWM[blk].PCNE.bit.__CKS;
			break;

		case 3:
			*clock = IO_PERI.PWM[blk].PCNE.bit.__CKS;
			break;

		default:
			return D_DD_PWM_INPUT_PARAM_ERROR;
	}

	return D_DDIM_OK;
}

/**
 * @brief  Get the output state of the stopped.
 * @param  UINT32 ch, ULONG* ch_status
 * @return INT32 D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Get_Stopped_OutPut_Status(UINT32 ch, UCHAR* ch_status)
{
	const UINT32 blk = ch >> 2; 		/* ch / D_DD_PWM_CH_NUM_IN_BLOCK */

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	
	if (0 == ch_status) {
		Ddim_Assertion(("PWM input param error. [ch_status] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	switch (ch & 0x03) {
		case 0:
			*ch_status = IO_PERI.PWM[blk].PCHSET0.bit.OSTA0;
			break;

		case 1:
			*ch_status = IO_PERI.PWM[blk].PCHSET0.bit.OSTA1;
			break;

		case 2:
			*ch_status = IO_PERI.PWM[blk].PCHSET0.bit.OSTA2;
			break;

		case 3:
			*ch_status = IO_PERI.PWM[blk].PCHSET0.bit.OSTA3;
			break;

		default:
			return D_DD_PWM_INPUT_PARAM_ERROR;
	}

	return D_DDIM_OK;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/**
 * @brief  Cycle parameters are calculated according
		   to the designated time (u sec).
		   If it doesn't calculate, Input Parameter Error is returned.
 * @param  ULONG usec, UCHAR* clk, USHORT* cycle
 * @return INT32 D_DDIM_OK / D_DD_PWM_OVERFLOW_OK / D_DD_PWM_OVERFLOW_ERROR
 */
INT32 Dd_PWM_Calculate(ULONG usec, UCHAR* const clk, USHORT* const cycle)
{
	UINT32 find_flag;
	ULONG peri_clock;
	ULONG total_count;
	ULONG total_count_wk;
	ULONG div_tmp;
	UCHAR div_wk, div_wk2, div_wk3;
	ULONG count_base;
	ULONG clock_div, clock_margin;
	ULONG usec_by_usec_div;
	ULONG peri_clock_by_clock_div;
	ULONG usec_div_wk, usec_div, usec_margin;
	// clock	CKS	2^	div_wk
	// 1		0	0	1
	// 4		1	2	2
	// 16		2	4	3
	// 64		3	6	4

#ifdef CO_PARAM_CHECK
	if (usec == 0) {
		Ddim_Assertion(("PWM input param error. [usec] = %ld\n", usec));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (clk == NULL) {
		Ddim_Assertion(("PWM input param error. [clk] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (cycle == NULL) {
		Ddim_Assertion(("PWM input param error. [cycle] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	clock_div = 1;
	clock_margin = 1;
	usec_div = 0;
	usec_margin = 0;

	peri_clock = Dd_Top_Get_RCLK();					// The number of clocks is set to the variable
//	printf("peri_clock = %ld \n", peri_clock);
	
	count_base = peri_clock * usec; 				// total count(Overflow is a disregarded number of counts)

	// The correspondence dividing frequency is retrieved
	for (div_wk = 4; div_wk > 0; div_wk--) {
		find_flag = 0;				// flag initialize
		div_tmp = 1 << (UCHAR)((div_wk - 1) * 2);			 // clock

		// div_tmp(clock) = usec_div * clock_div

		// "The number of specified time" * "peri_clock" judges to be divided because of a specified dividing frequency regardless of overflow
		if ((count_base & (div_tmp - 1)) == 0) {
			// Dividing frequency that usec can be divided is retrieved. It adds for the condition judgment of the ULONG type 1
			for (usec_div_wk = 1; usec_div_wk <= div_tmp; usec_div_wk *= 2) {
				usec_div = div_tmp / usec_div_wk;			// clock

				/* Time is divided in order of 64, 32, 16, 8, 4, and 2 at the time of 64 dividing frequency
				   Time is divided in order of 16, 8, 4, and 2 at the time of 16 dividing frequency
				   Time is divided in order of 4 and 2 at the time of four dividing frequency
				   Time is divided by one at the time of one dividing frequency */

				if ((usec % usec_div) == 0) {
					// (usec/usec_div)However, the multiple of ten that can be practical is obtained
					usec_by_usec_div = usec / usec_div;
					clock_div = (div_tmp / usec_div);
					peri_clock_by_clock_div = peri_clock / clock_div;
					for (usec_margin = 1000000; usec_margin > 0; usec_margin /= 10) {
						if ((usec_by_usec_div % usec_margin) == 0) {
							clock_margin = (1000000 / usec_margin);
							// Whether {peri_clock/(div_tmp/usec_div)} can divide by (1000000/usec_margin) is judged
							if ((peri_clock_by_clock_div % clock_margin) == 0) {

								// It is judged not to overflow even if the number of total counts is requested by
								// using obtained "usec_div", "usec_margin", "clock_div", and "clock_margin"
								if ((usec / (usec_div * usec_margin)) <= (0xffffffff / (peri_clock / (clock_div * clock_margin)))) {
									find_flag = 1;
									break;
								}
							}
						}
					}
					if (find_flag == 1) {
						// Dividing frequency (usec_div&clock_div) and margin that doesn't do overflow exist.
						if(((usec / usec_div) % usec_margin) != 0) {
							// Because time cannot be divided, it continues the retrieval
							find_flag = 0;
							continue;
						}
						if ((peri_clock_by_clock_div % clock_margin) != 0) {
							// Because time cannot be divided, it continues the retrieval
							find_flag = 0;
							continue;
						}
						if ((usec / usec_div) > (0xffffffff / (peri_clock_by_clock_div / clock_margin))) {
							find_flag = 0;
							continue;
						}
						break;
					}
				}
			}
		}

		if (find_flag == 1) {
			total_count = (usec / (usec_div * usec_margin)) * (peri_clock / (clock_div * clock_margin));		// total_count

			*clk = (UCHAR)(div_wk - 1);
			if (total_count <= 0xffff) {
				*cycle	= (USHORT)total_count;
			}
			else {
				for (div_wk2 = 1; div_wk2 <= 4; div_wk2++ ) {

					if ((div_wk + div_wk2) > 4) {
						break;
					}

					div_wk3 = (UCHAR)(4 << (UCHAR)((div_wk2 - 1) * 2));

					total_count_wk	= total_count / div_wk3;

					if (total_count_wk <= 0xffff) {
						*cycle	= (USHORT)total_count_wk;
						*clk	= (UCHAR)(*clk + div_wk2);
						return D_DD_PWM_OVERFLOW_OK;
					}
				}
				return D_DD_PWM_OVERFLOW_ERROR;
			}
			break;
		}
	}
	return D_DDIM_OK;
}


/**
 * @brief  Cycle parameters are calculated according
		   to the designated time (u sec).
		   If it doesn't calculate, Input Parameter Error is returned.
 * @param  ULONG usec, UCHAR clk, USHORT* cycle
 * @return INT32 D_DDIM_OK / D_DD_PWM_OVERFLOW_ERROR
 */
INT32 Dd_PWM_Calculate_Cycle(ULONG usec, UCHAR const clk, USHORT* const cycle)
{
	ULONG peri_clock_2;
	ULONG total_count;

#ifdef CO_PARAM_CHECK
	if (usec == 0) {
		Ddim_Assertion(("PWM input param error. [usec] = %ld\n", usec));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (clk > D_DD_PWM_PCNHL_CKS_DIV_64) {
		Ddim_Assertion(("PWM input param error. [clk] = %d\n", clk));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (cycle == NULL) {
		Ddim_Assertion(("PWM input param error. [cycle] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	peri_clock_2 = Dd_Top_Get_RCLK();

	if( 0 == clk ) {
		total_count = ((peri_clock_2 / 1000000) * usec);
	}
	else {
		total_count = ((peri_clock_2 / 1000000) * usec) / (1 << (clk << 1));
	}

	if( 0 == total_count ) {
		*cycle = 1; // minimum count
//		return D_DD_PWM_OVERFLOW_ERROR;
		return D_DDIM_OK;
	}
	else if( 0xFFFF < total_count ) {
		*cycle = 0xFFFF; // maximum count
//		return D_DD_PWM_OVERFLOW_ERROR;
		return D_DDIM_OK;
	}
	else {
		*cycle = (USHORT)total_count;
		return D_DDIM_OK;
	}
}


/**
 * @brief  The PWM output control channel wave form is set (change).
 * @param  UINT32 ch, UCHAR cycle, UCHAR duty(0-100%)
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Write_Cycle_Duty(UINT32 ch, USHORT cycle, UCHAR duty, UCHAR clk_sel)
{
	volatile union io_pwm_pcn* pcn_addr;
	union io_pwm_pcn pcn_tmp;
	USHORT duty_value = 0;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (cycle == 0) {
		Ddim_Assertion(("PWM input param error. [cycle] = %d\n", cycle));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (duty == 0) {
		Ddim_Assertion(("PWM input param error. [duty] = %d\n", duty));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (clk_sel > D_DD_PWM_PCNHL_CKS_DIV_64) {
		Ddim_Assertion(("PWM input param error. [clk_sel] = %d\n", clk_sel));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	if(gDD_PWM_Ctrl[ch].clk_sel != clk_sel) {
		pcn_addr = dd_pwm_pcn_addr(ch);
		pcn_tmp.word = pcn_addr->word;
		pcn_tmp.bit.__CKS = clk_sel;
		pcn_tmp.bit.__IRQF = 1;									/* interrupt flag (not change)	*/
		pcn_addr->word = pcn_tmp.word;
	}

	duty_value = (USHORT)(cycle * duty / 100);
	gDD_PWM_Ctrl[ch].cyc_num = cycle;
	gDD_PWM_Ctrl[ch].duty_num = duty_value;

	switch (ch & 0x03) {
		case 0:
			IO_PERI.PWM[ch >> 2].PCSRA = cycle;
			IO_PERI.PWM[ch >> 2].PDUTA = duty_value;
			break;

		case 1:
			IO_PERI.PWM[ch >> 2].PCSRB = cycle;
			IO_PERI.PWM[ch >> 2].PDUTB = duty_value;
			break;

		case 2:
			IO_PERI.PWM[ch >> 2].PCSRC = cycle;
			IO_PERI.PWM[ch >> 2].PDUTC = duty_value;
			break;

		case 3:
			IO_PERI.PWM[ch >> 2].PCSRD = cycle;
			IO_PERI.PWM[ch >> 2].PDUTD = duty_value;
			break;

		default:
			return D_DD_PWM_INPUT_PARAM_ERROR;
	}

	return D_DDIM_OK;
}


/**
 * @brief  The PWM output control channel wave form is set for reverse(change).
 * @param  UINT32 ch, USHORT cycle, UCHAR duty(0-100%)
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Write_Cycle_Duty_Reverse(UINT32 ch, USHORT cycle, UCHAR duty, UCHAR clk_sel)
{
	UCHAR CNTE;
	volatile union io_pwm_pcn* pcn_addr;
	union io_pwm_pcn pcn_tmp;
	USHORT duty_value = 0;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (cycle == 0) {
		Ddim_Assertion(("PWM input param error. [cycle] = %d\n", cycle));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (duty == 0) {
		Ddim_Assertion(("PWM input param error. [duty] = %d\n", duty));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (clk_sel > D_DD_PWM_PCNHL_CKS_DIV_64) {
		Ddim_Assertion(("PWM input param error. [clk_sel] = %d\n", clk_sel));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	pcn_addr = dd_pwm_pcn_addr( ch );

	CNTE = pcn_addr->bit.__CNTE;
	pcn_tmp.word = pcn_addr->word;
	pcn_tmp.bit.__CNTE	= 0;						/* timer count enable	@PCNH	*/
	pcn_tmp.bit.__IRQF	= 1;						/* interrupt flag (not change)	*/
	if(gDD_PWM_Ctrl[ch].clk_sel != clk_sel) {
		pcn_tmp.bit.__CKS = clk_sel;
	}
	pcn_addr->word = pcn_tmp.word;

	duty_value = (USHORT)(cycle * duty / 100);

	switch (ch & 0x03) {
		case 0:
			IO_PERI.PWM[ch >> 2].PCSRRA = cycle;
			IO_PERI.PWM[ch >> 2].PDUTRA = duty_value;
			IO_PERI.PWM[ch >> 2].PREV.bit.PREVE0 = 1;
			break;

		case 1:
			IO_PERI.PWM[ch >> 2].PCSRRB = cycle;
			IO_PERI.PWM[ch >> 2].PDUTRB = duty_value;
			IO_PERI.PWM[ch >> 2].PREV.bit.PREVE1 = 1;
			break;

		case 2:
			IO_PERI.PWM[ch >> 2].PCSRRC = cycle;
			IO_PERI.PWM[ch >> 2].PDUTRC = duty_value;
			IO_PERI.PWM[ch >> 2].PREV.bit.PREVE2 = 1;
			break;

		case 3:
			IO_PERI.PWM[ch >> 2].PCSRRD = cycle;
			IO_PERI.PWM[ch >> 2].PDUTRD = duty_value;
			IO_PERI.PWM[ch >> 2].PREV.bit.PREVE3 = 1;
			break;

		default:
			return D_DD_PWM_INPUT_PARAM_ERROR;
	}

	pcn_tmp.bit.__CNTE	= CNTE; 					/* timer count restore			*/
	pcn_addr->word = pcn_tmp.word;

	return D_DDIM_OK;
}


/**
 * @brief  The PWM output control channel wave form is set for queue(change).
 * @param  UINT32 ch, USHORT cycle, UCHAR duty(0-100%)
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Write_Cycle_Duty_Queue(UINT32 ch, USHORT cycle, UCHAR duty, UCHAR clk_sel)
{
	volatile union io_pwm_pcn* pcn_addr;
	union io_pwm_pcn pcn_tmp;
	USHORT duty_value = 0;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (cycle == 0) {
		Ddim_Assertion(("PWM input param error. [cycle] = %d\n", cycle));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (duty == 0) {
		Ddim_Assertion(("PWM input param error. [duty] = %d\n", duty));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (clk_sel > D_DD_PWM_PCNHL_CKS_DIV_64) {
		Ddim_Assertion(("PWM input param error. [clk_sel] = %d\n", clk_sel));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	if(gDD_PWM_Ctrl[ch].clk_sel != clk_sel) {
		pcn_addr = dd_pwm_pcn_addr(ch);
		pcn_tmp.word = pcn_addr->word;
		pcn_tmp.bit.__CKS = clk_sel;
		pcn_tmp.bit.__IRQF	= 1;						/* interrupt flag (not change)	*/
		pcn_addr->word = pcn_tmp.word;
		gDD_PWM_Ctrl[ch].clk_sel = clk_sel;
	}

	duty_value = (USHORT)(cycle * duty / 100);

	switch (ch & 0x03) {
		case 0:
			IO_PERI.PWM[ch >> 2].PCSRQA = cycle;
			IO_PERI.PWM[ch >> 2].PDUTQA = duty_value;
			break;

		case 1:
			IO_PERI.PWM[ch >> 2].PCSRQB = cycle;
			IO_PERI.PWM[ch >> 2].PDUTQB = duty_value;
			break;

		case 2:
			IO_PERI.PWM[ch >> 2].PCSRQC = cycle;
			IO_PERI.PWM[ch >> 2].PDUTQC = duty_value;
			break;

		case 3:
			IO_PERI.PWM[ch >> 2].PCSRQD = cycle;
			IO_PERI.PWM[ch >> 2].PDUTQD = duty_value;
			break;

		default:
			return D_DD_PWM_INPUT_PARAM_ERROR;
	}

	return D_DDIM_OK;
}


/**
 * @brief  The PWM input trigger is set.
 * @param  UINT32 ch, UCHAR gcn_tsel
 * @return INT32 D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Write_Input_Trg(UINT32 ch, UCHAR gcn_tsel)
{
	const UINT32 ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	switch (ch_in_blk) {
		case 0:
			IO_PERI.PWM[ch >> 2].GCN1.bit.TSEL0 = gcn_tsel;
			break;

		case 1:
			IO_PERI.PWM[ch >> 2].GCN1.bit.TSEL1 = gcn_tsel;
			break;

		case 2:
			IO_PERI.PWM[ch >> 2].GCN1.bit.TSEL2 = gcn_tsel;
			break;

		case 3:
			IO_PERI.PWM[ch >> 2].GCN1.bit.TSEL3 = gcn_tsel;
			break;

		default:
			return D_DD_PWM_INPUT_PARAM_ERROR;
	}

	return D_DDIM_OK;
}


/**
 * @brief  The PWM switch factor of cycle and duty is selected.
 * @param  UINT32 ch, UCHAR tsel
 * @return INT32  D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Write_Cycle_Duty_Switch(UINT32 ch, UCHAR tsel)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	switch (ch & 0x03) {
		case 0:
			IO_PERI.PWM[ch >> 2].TSEL.bit.SEL0 = tsel;
			break;

		case 1:
			IO_PERI.PWM[ch >> 2].TSEL.bit.SEL1 = tsel;
			break;

		case 2:
			IO_PERI.PWM[ch >> 2].TSEL.bit.SEL2 = tsel;
			break;

		case 3:
			IO_PERI.PWM[ch >> 2].TSEL.bit.SEL3 = tsel;
			break;

		default:
			return D_DD_PWM_INPUT_PARAM_ERROR;
	}

	return D_DDIM_OK;
}

/**
 * @brief  The PWM controle and status is set.
 * @param  UINT32 ch, USHORT pcn
 * @return INT32 D_DDIM_OK/D_DD_PWM_INPUT_PARAM_ERROR
 */
INT32 Dd_PWM_Write_Pcn(UINT32 ch, USHORT pcn)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	switch (ch & 0x03) {
		case 0:
			IO_PERI.PWM[ch >> 2].PCNA.word = pcn;
			break;

		case 1:
			IO_PERI.PWM[ch >> 2].PCNB.word = pcn;
			break;

		case 2:
			IO_PERI.PWM[ch >> 2].PCNC.word = pcn;
			break;

		case 3:
			IO_PERI.PWM[ch >> 2].PCND.word = pcn;
			break;

		default:
			break;
	}

	return D_DDIM_OK;
}

/**
 * The PWM Timer Onshot is set.
 */
INT32 Dd_PWM_Write_Oneshot(UINT32 ch,
						 USHORT duty_time,
						 T_DD_PWM_CTRL_CMN* const pwm_ctrl_cmn,
						 T_DD_PWM_CTRL_TIMER_ABCD* const pwm_ctrl_timer_abcd)
{
	INT32 ret;
	UCHAR clk;
	USHORT cyc_num = 0;
	USHORT duty_num = 0;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_PWM_CH_NUM_MAX) {
		Ddim_Assertion(("PWM input param error. [ch] = %d\n", ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (0 == pwm_ctrl_cmn) {
		Ddim_Assertion(("PWM input param error. [pwm_ctrl_cmn] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (0 == pwm_ctrl_timer_abcd) {
		Ddim_Assertion(("PWM input param error. [pwm_ctrl_timer_abcd] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	// Dividing frequency set
	clk = pwm_ctrl_cmn->clk_sel;

	// cycle time(usec) convert
	ret = Dd_PWM_Calculate( pwm_ctrl_timer_abcd->cyc_time, &clk, &cyc_num);
	if ((ret != D_DDIM_OK) && (ret != D_DD_PWM_OVERFLOW_OK)){
		Ddim_Print(("PWM: calculate error. ret = 0x%x\n", ret))
		return ret;
	}

	duty_num =	(USHORT)(cyc_num * duty_time / pwm_ctrl_timer_abcd->cyc_time);

	if ( duty_num > cyc_num ) {
		duty_num = (USHORT)(cyc_num - 1);
	}
	pwm_ctrl_cmn->drv_mode = 1; 		/* Oneshot */
	pwm_ctrl_cmn->clk_sel = clk;
	pwm_ctrl_timer_abcd->cyc_num = cyc_num;
	pwm_ctrl_timer_abcd->duty_num = duty_num;
	pwm_ctrl_cmn->int_enable = D_DD_PWM_PCNHL_IREN_ON;

	ret = Dd_PWM_Ctrl_Common(ch, pwm_ctrl_cmn);
	if (ret != D_DDIM_OK){
		Ddim_Print(("PWM: common setting error. ret = 0x%x\n", ret))
		return ret;
	}

	ret = Dd_PWM_Ctrl_Timer_ABCD(ch, pwm_ctrl_timer_abcd);
	if (ret != D_DDIM_OK){
		Ddim_Print(("PWM: abcd setting error. ret = 0x%x\n", ret))
		return ret;
	}

	return D_DDIM_OK;
}

//---------------------- colabo  section -------------------------------
/**
 * The PWM Timer E and DMA is set for stepping motor.
 */
INT32 Dd_PWM_Write_Step(UINT32 pwm_ch, UINT32 dma_ch, T_DD_PWM_STEP const* const pwm_step)
{
	INT32 ret;
	UCHAR ch_in_blk;
	UCHAR step;
	UCHAR index;
	UCHAR step_remainder;
	T_DD_PWM_CTRL_CMN ctrl_cmn;
	T_DD_PWM_CTRL_TIMER_E ctrl_e;
	T_DD_PWM_TIMER_E_STEPS_DATA* w_step_data;
	T_DD_PWM_TIMER_E_STEPS_DATA* w_under_run_data;

#ifdef CO_PARAM_CHECK
	if ((pwm_ch != 0) && (pwm_ch != 4) && (pwm_ch != 8) && (pwm_ch != 12)) {
		Ddim_Assertion(("PWM input param error. [pwm_ch] = %d\n", pwm_ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if(dma_ch >= D_DD_HDMAC0_CH_NUM_MAX){
		Ddim_Assertion(("PWM input param error. [dma_ch] = %x\n", dma_ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if (0 == pwm_step) {
		Ddim_Assertion(("PWM input param error. [pwm_step] = NULL\n"));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	w_step_data = pwm_step->step_data;
	w_under_run_data = pwm_step->under_run_data;

//	ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & pwm_ch;
	ch_in_blk = D_DD_PWM_GCN_TSEL_TIM0;

	// Set PWM Control Common Data
	ctrl_cmn.clk_sel		= pwm_step->clk_sel;							// clock select
	ctrl_cmn.polarity		= pwm_step->polarity;							// output polarity
	ctrl_cmn.ch_assign		= pwm_step->ch_assign;							// channel assignment
	ctrl_cmn.in_trigger		= ch_in_blk;									// trigger input factor
	ctrl_cmn.drv_mode		= D_DD_PWM_PCNHL_MDSE_PWM;						// PWM or One-Shot operation
	ctrl_cmn.edge			= D_DD_PWM_PCNHL_EGS_RISE;						// edge enable select
	ctrl_cmn.restart_trig	= pwm_step->restart_trig;						// restart enable
	ctrl_cmn.out_mask		= D_DD_PWM_PCNHL_PGMS_L;						// output level mask
	ctrl_cmn.int_enable		= D_DD_PWM_PCNHL_IREN_OFF;						// interrupt enable
	ctrl_cmn.trigger		= D_DD_PWM_PCNHL_IRS_TRG;						// interrupt factor (don't care)
	ctrl_cmn.tim_e_enable	= 1;											// Timer E disable
	ctrl_cmn.pCallBack		= pwm_step->pCallBack;							// callback function pointer

	// Set PWM Control Timer E Data
	for( index = 0; index < D_DD_PWM_CH_NUM_IN_BLOCK; index++ ) {
		ctrl_e.hold[index]		= pwm_step->hold[index];					// hold setting at stop

		if(pwm_step->force[index] != D_DD_PWM_STEP_FORCE_DISABLE ){
			ctrl_e.force[index]			= 1;								// force enable
			ctrl_e.force_val[index]		= pwm_step->force[index];			// force value
		}
		else{
			ctrl_e.force[index]			= 0;								// force disable
			ctrl_e.force_val[index]		= 0;								// force value
		}
	}

	ctrl_e.undr_run_int_err_enable	= D_DD_PWM_PINTCT_UIE_ON;				// interrupt enable under run error
	ctrl_e.force_stop_int_enable	= D_DD_PWM_PINTCT_FIE_ON;				// interrupt enable for force stop
	ctrl_e.auto_stop_int_enable		= D_DD_PWM_PINTCT_AIE_ON;				// interrupt enable for auto stop
//	ctrl_e.empty_dma_req			= D_DD_PWM_PCHSET_RQTIM_4;				// Inerrupt request timing for DMA
//	ctrl_e.empty_dma_req			= D_DD_PWM_PCHSET_RQTIM_6;				// Inerrupt request timing for DMA
	ctrl_e.empty_dma_req			= D_DD_PWM_PCHSET_RQTIM_2;				// Inerrupt request timing for DMA

	// Set PWM Step Data
	for( step = 0; step < D_DD_PWM_TIMER_E_STEPS; step++ ) {
		if( step < pwm_step->step_data_num ){
			ctrl_e.steps_data[step].cyc_num				= w_step_data->cyc_num;
			ctrl_e.steps_data[step].duty_num			= w_step_data->duty_num;
			ctrl_e.steps_data[step].pulse_link.hword	= w_step_data->pulse_link.hword;
		}
		else{
			ctrl_e.steps_data[step].cyc_num				= 0;
			ctrl_e.steps_data[step].duty_num			= 0x0003;
			ctrl_e.steps_data[step].pulse_link.hword	= 0;
		}
		w_step_data++;
	}

	// Set PWM Under run Step Data
	if( pwm_step->undr_run_data_num > 0 ){
		ctrl_e.undr_run_enable		= 1;						// speed down setting under run error enable
		for( step = 0; step < D_DD_PWM_TIMER_E_STEPS_UNDER_RUN; step++ ) {
			if( step < pwm_step->undr_run_data_num ){
				ctrl_e.under_run_data[step].cyc_num				= w_under_run_data->cyc_num;
				ctrl_e.under_run_data[step].duty_num			= w_under_run_data->duty_num;
				ctrl_e.under_run_data[step].pulse_link.hword	= w_under_run_data->pulse_link.hword;
			}
			else{
				ctrl_e.under_run_data[step].cyc_num				= 0;
				ctrl_e.under_run_data[step].duty_num			= 0x0003;
				ctrl_e.under_run_data[step].pulse_link.hword	= 0;
			}
			w_under_run_data++;
		}
	}
	else{
		ctrl_e.undr_run_enable		= 0;						// speed down setting under run error disable
	}

	if( pwm_step->step_data_num > D_DD_PWM_TIMER_E_STEPS ){
		ctrl_e.dma_int_enable		= D_DD_PWM_PINTCT_DRQE_ON;	// interrupt enable for dma
		gDD_PWM_Ctrl[pwm_ch].dma_enable	= 1;
		gDD_PWM_Ctrl[pwm_ch].dma_ch		= dma_ch;
		step_remainder = pwm_step->step_data_num - D_DD_PWM_TIMER_E_STEPS;

		// cache clean
		DDIM_User_L1l2cache_Clean_Addr( (UINT32)w_step_data, sizeof(T_DD_PWM_TIMER_E_STEPS_DATA) * step_remainder );

		// Set DMA Transfer
		ret= dd_pwm_dma_set(pwm_ch, dma_ch, step_remainder,
							(ULONG)w_step_data, (ULONG)(&(IO_PWMDMA[pwm_ch >> 2].BFDTPT)) );
	}
	else{
		ctrl_e.dma_int_enable		= D_DD_PWM_PINTCT_DRQE_OFF;	// interrupt disable for dma
		gDD_PWM_Ctrl[pwm_ch].dma_enable	= 0;
	}

	// Set PWM Control(Common Setting)
	ret = Dd_PWM_Ctrl_Common(pwm_ch, &ctrl_cmn);
	if (ret != D_DDIM_OK){
		Ddim_Print(("PWM: common setting error. result = 0x%x\n", ret))
		return ret;
	}

	// Set PWM Control(Timer E Setting)
	ret = Dd_PWM_Ctrl_Timer_E(pwm_ch, &ctrl_e);
	if (ret != D_DDIM_OK){
		Ddim_Print(("PWM: timer e setting error. result = 0x%x\n", ret))
		return ret;
	}

	return D_DDIM_OK;
}


/**
 * The PWM Timer E and DMA is set for stepping motor.
 */
INT32 Dd_PWM_Write_Step_Only_Buf_Data_Window(UINT32 pwm_ch, UINT32 dma_ch, T_DD_PWM_TIMER_E_STEPS_DATA const* const pwm_step, UCHAR data_num )
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((pwm_ch != 0) && (pwm_ch != 4) && (pwm_ch != 8) && (pwm_ch != 12)) {
		Ddim_Assertion(("PWM input param error. [pwm_ch] = %d\n", pwm_ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
	if(dma_ch >= D_DD_HDMAC0_CH_NUM_MAX){
		Ddim_Assertion(("PWM input param error. [dma_ch] = %x\n", dma_ch));
		return D_DD_PWM_INPUT_PARAM_ERROR;
	}
#endif

	gDD_PWM_Ctrl[pwm_ch].dma_enable	= 1;
	gDD_PWM_Ctrl[pwm_ch].dma_ch		= dma_ch;

	// cache clean
	DDIM_User_L1l2cache_Clean_Addr( (UINT32)pwm_step, sizeof(T_DD_PWM_TIMER_E_STEPS_DATA) * data_num );


	ret = Dd_HDMAC0_Close( dma_ch );

	// Set DMA Transfer
	ret |= dd_pwm_dma_set(pwm_ch, dma_ch, data_num,
						(ULONG)pwm_step, (ULONG)(&(IO_PWMDMA[pwm_ch >> 2].BFDTPT)) );


	return ret;
}

#endif	/* CO_DDIM_UTILITY_USE */

