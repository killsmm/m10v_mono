/**
 * @file		ct_dd_pwm.c
 * @brief		This is ct code for dd_pwm
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "stdlib.h"
#include "string.h"
#include "peripheral.h"
#include "dd_pwm.h"
#include "dd_hdmac0.h"
#include "dd_top.h"
#include "dd_gic.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	PWM_TEST

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* PWM stepping data */
static T_DD_PWM_TIMER_E_STEPS_DATA gCt_DD_PWM_Step_Data[16] = {
/*	cyc_num,	duty_num,				pulse_num(link) */
	{	50625,		(50625 / 8 * 6),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 6),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 6),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 6),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 4),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 4),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 4),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 4),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 3),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 3),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 3),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 3),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 2),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 2),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 2),		{(USHORT)(256 + (1 << 15))}	},
	{	50625,		(50625 / 8 * 2),		{(USHORT)(256 + (0 << 15))}	}
};

static T_DD_PWM_TIMER_E_STEPS_DATA* gCt_DD_PWM_Step_Data_p = gCt_DD_PWM_Step_Data;

/* PWM Under run data */
static T_DD_PWM_TIMER_E_STEPS_DATA gCt_PWM_Under_Run_Data[4] = {
/*	cyc_num,	duty_num,				pulse_num(link) */
	{	52000,		(52000 / 8 * 2),		{(USHORT)(512 + (1 << 15))}	},
	{	54000,		(54000 / 8 * 3),		{(USHORT)(512 + (1 << 15))}	},
	{	56000,		(56000 / 8 * 4),		{(USHORT)(512 + (1 << 15))}	},
	{	58000,		(58000 / 8 * 6),		{(USHORT)(512 + (0 << 15))}	}
};

static T_DD_PWM_TIMER_E_STEPS_DATA* gCt_PWM_Under_Run_Data_p = gCt_PWM_Under_Run_Data;

static UCHAR gDMA_CH;

static ULONG gDD_Pwm_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define USER_LED13_On()		(*(volatile unsigned long*)0x1DFFB004 = 0x00400040)
#define USER_LED13_Off()	(*(volatile unsigned long*)0x1DFFB004 = 0x00400000)

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief  Command main function for timer32 test.
 * @param  T_PWM_CTRL * ctrl	:The number of parameters
 *		   UCHAR ptn_no			:The value of parameters
 *								:
 *								:
 *								:
 * @return VOID
 */
static void ct_dd_pwn_set_ctrl(T_DD_PWM_CTRL_CMN* ctrl,
								   T_DD_PWM_CTRL_TIMER_ABCD* ctrl_abcd,
								   UCHAR ptn_no, UCHAR ch)
{
	UCHAR clk;
	USHORT cyc_num;
	INT32 result;
	UCHAR ch_in_blk;

	result = Dd_PWM_Calculate( 60000, &clk, &cyc_num);
	if ((result != D_DDIM_OK) && (result != D_DD_PWM_OVERFLOW_OK)){
		return; 	/* Error */
	}

	ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;

	switch (ptn_no) {
		case 0: /* one-shot & restart enable & soft trigger */
			ctrl->drv_mode		= D_DD_PWM_PCNHL_MDSE_ONE;		/* (1)one-shot					*/
			ctrl->restart_trig	= D_DD_PWM_PCNHL_RTRG_ON;		/* (1)restart enable			*/
			ctrl->clk_sel		= clk;
			ctrl->out_mask		= D_DD_PWM_PCNHL_PGMS_L;		/* (0)output is "Low"			*/
			ctrl->edge			= D_DD_PWM_PCNHL_EGS_RISE;		/* (1)rise edge					*/
			ctrl->int_enable	= D_DD_PWM_PCNHL_IREN_OFF;		/* (0)interrupt disable			*/
			ctrl->trigger		= D_DD_PWM_PCNHL_IRS_CYCLE; 	/* (1)agreement cycle			*/
			ctrl->polarity		= D_DD_PWM_PCNHL_OSEL_NORMAL;	/* (0)polarity usually			*/
			ctrl->in_trigger	= ch_in_blk;					/* (0-3)EN0-3 bit				*/
			ctrl->tim_e_enable	= 0;							/* Timer E disable				*/
			ctrl->ch_assign		= D_DD_PWM_GCN_CHAS_ABCD;		/* channel assignment			*/
			ctrl_abcd->cyc_time	= 60000;						/* (50625) wave cycle(usec)		*/
			ctrl_abcd->cyc_num	= cyc_num;
			ctrl_abcd->duty_num	= cyc_num >> 1; 				/* 50% */
			ctrl->pCallBack		= NULL; 						/* callback function pointer	*/
			break;

		case 1: /* PWM & restart disable & soft trigger */
			ctrl->drv_mode		= D_DD_PWM_PCNHL_MDSE_PWM;		/* (0)PWM						*/
			ctrl->restart_trig	= D_DD_PWM_PCNHL_RTRG_OFF;		/* (0)restart disable			*/
			ctrl->clk_sel		= clk;
			ctrl->out_mask		= D_DD_PWM_PCNHL_PGMS_L;		/* (0)output is "Low"			*/
			ctrl->edge			= D_DD_PWM_PCNHL_EGS_RISE;		/* (1)rise edge					*/
			ctrl->int_enable	= D_DD_PWM_PCNHL_IREN_ON;		/* (1)interrupt enable			*/
			ctrl->trigger		= D_DD_PWM_PCNHL_IRS_CYCLE; 	/* (1)agreement cycle			*/
			ctrl->polarity		= D_DD_PWM_PCNHL_OSEL_NORMAL;	/* (0)polarity usually			*/
			ctrl->in_trigger	= ch_in_blk;					/* (0-3)EN0-3 bit				*/
			ctrl->tim_e_enable	= 0;							/* Timer E disable				*/
			ctrl->ch_assign		= D_DD_PWM_GCN_CHAS_ABCD;		/* channel assignment			*/
			ctrl_abcd->cyc_time	= 60000;						/* (50625) wave cycle(usec)		*/
			ctrl_abcd->cyc_num	= cyc_num;
			ctrl_abcd->duty_num	= cyc_num >> 1; 				/* 50% */
			ctrl->pCallBack		= NULL; 						/* callback function pointer	*/
			break;

		case 2: /* PWM & restart disable & cycle trigger */
			ctrl->drv_mode		= D_DD_PWM_PCNHL_MDSE_PWM;		/* (0)PWM						*/
			ctrl->restart_trig	= D_DD_PWM_PCNHL_RTRG_OFF;		/* (0)restart disable			*/
			ctrl->clk_sel		= clk;
			ctrl->out_mask		= D_DD_PWM_PCNHL_PGMS_L;		/* (0)output is "Low"			*/
			ctrl->edge			= D_DD_PWM_PCNHL_EGS_RISE;		/* (1)rise edge					*/
			ctrl->int_enable	= D_DD_PWM_PCNHL_IREN_OFF;		/* (0)interrupt disable			*/
			ctrl->trigger		= D_DD_PWM_PCNHL_IRS_CYCLE; 	/* (1)agreement cycle			*/
			ctrl->polarity		= D_DD_PWM_PCNHL_OSEL_NORMAL;	/* (0)polarity usually			*/
			ctrl->in_trigger	= D_DD_PWM_GCN_TSEL_TIM1;		/* (5)Timer ch1					*/
			ctrl->tim_e_enable	= 0;							/* Timer E disable				*/
			ctrl->ch_assign		= D_DD_PWM_GCN_CHAS_ABCD;		/* channel assignment			*/
			ctrl_abcd->cyc_time	= 60000;						/* (50625) wave cycle(usec)		*/
			ctrl_abcd->cyc_num	= cyc_num;
			ctrl_abcd->duty_num	= cyc_num >> 1; 				/* 50% */
			ctrl->pCallBack		= NULL; 						/* callback function pointer	*/
			break;

		default:
			break;
	}
}

#ifdef PWM_TEST
static void ct_dd_pwn_set_ctrl2(T_DD_PWM_CTRL_CMN* ctrl,
								   T_DD_PWM_CTRL_TIMER_ABCD* ctrl_abcd,
								   UCHAR ptn_no,
								   UCHAR ch,
								   UCHAR cks,
								   ULONG cyc_num)
{
	UCHAR ch_in_blk;

	ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;

	switch (ptn_no) {
		case 0: /* one-shot & restart enable & soft trigger */
			ctrl->drv_mode		= D_DD_PWM_PCNHL_MDSE_ONE;		/* (1)one-shot					*/
			ctrl->restart_trig	= D_DD_PWM_PCNHL_RTRG_ON;		/* (1)restart enable			*/
			ctrl->clk_sel		= cks;
			ctrl->out_mask		= D_DD_PWM_PCNHL_PGMS_L;		/* (0)output is "Low"			*/
			ctrl->edge			= D_DD_PWM_PCNHL_EGS_RISE;		/* (1)rise edge					*/
			ctrl->int_enable	= D_DD_PWM_PCNHL_IREN_OFF;		/* (0)interrupt disable			*/
			ctrl->trigger		= D_DD_PWM_PCNHL_IRS_CYCLE; 	/* (1)agreement cycle			*/
			ctrl->polarity		= D_DD_PWM_PCNHL_OSEL_NORMAL;	/* (0)polarity usually			*/
			ctrl->in_trigger	= ch_in_blk;					/* (0-3)EN0-3 bit				*/
			ctrl->tim_e_enable	= 0;							/* Timer E disable				*/
			ctrl->ch_assign		= D_DD_PWM_GCN_CHAS_ABCD;		/* channel assignment			*/
			ctrl_abcd->cyc_time	= 0;							/* wave cycle(usec)				*/
			ctrl_abcd->cyc_num	= cyc_num;
			ctrl_abcd->duty_num	= cyc_num >> 1; 				/* 50% */
			ctrl->pCallBack		= NULL; 						/* callback function pointer	*/
			break;

		case 1: /* PWM & restart disable & soft trigger */
			ctrl->drv_mode		= D_DD_PWM_PCNHL_MDSE_PWM;		/* (0)PWM						*/
			ctrl->restart_trig	= D_DD_PWM_PCNHL_RTRG_OFF;		/* (0)restart disable			*/
			ctrl->clk_sel		= cks;
			ctrl->out_mask		= D_DD_PWM_PCNHL_PGMS_L;		/* (0)output is "Low"			*/
			ctrl->edge			= D_DD_PWM_PCNHL_EGS_RISE;		/* (1)rise edge					*/
			ctrl->int_enable	= D_DD_PWM_PCNHL_IREN_ON;		/* (1)interrupt enable			*/
			ctrl->trigger		= D_DD_PWM_PCNHL_IRS_CYCLE; 	/* (1)agreement cycle			*/
			ctrl->polarity		= D_DD_PWM_PCNHL_OSEL_NORMAL;	/* (0)polarity usually			*/
			ctrl->in_trigger	= ch_in_blk;					/* (0-3)EN0-3 bit				*/
			ctrl->tim_e_enable	= 0;							/* Timer E disable				*/
			ctrl->ch_assign		= D_DD_PWM_GCN_CHAS_ABCD;		/* channel assignment			*/
			ctrl_abcd->cyc_time	= 0;							/* wave cycle(usec)				*/
			ctrl_abcd->cyc_num	= cyc_num;
			ctrl_abcd->duty_num	= cyc_num >> 1; 				/* 50% */
			ctrl->pCallBack		= NULL; 						/* callback function pointer	*/
			break;

		default:
			break;
	}
}
#endif

static VOID ct_dd_pwm_callback( INT32 ch, INT32 status )
{
	INT32 ret = D_DDIM_OK;
	if ((status == D_DD_PWM_INT_STATUS_FORCE_STOP) || (status == D_DD_PWM_INT_STATUS_AUTO_STOP)) {
		Dd_PWM_Stop(ch);
	}

	ret = Dd_HDMAC0_Stop(gDMA_CH);
	if(ret != D_DDIM_OK){
		Ddim_Print(("Error Dd_HDMAC0_Stop!! ret = 0x%x\n", ret));
	}
	ret = Dd_HDMAC0_Close(gDMA_CH);
	if(ret != D_DDIM_OK){
		Ddim_Print(("Error Dd_HDMAC0_Close!! ret = 0x%x\n", ret));
	}
	
	Ddim_Print(("ct_dd_pwm_callback() - status=%d\n", status));
}

static void ct_dd_pwm_dma_start( UCHAR pwm_ch, UCHAR dma_ch )
{
	INT32 ret;
	T_DD_HDMAC0_CTRL dma_ctrl;

	memset( &dma_ctrl, 0, sizeof(T_DD_HDMAC0_CTRL) );

	gDMA_CH = dma_ch;

	ret = Dd_HDMAC0_Open( dma_ch, D_DDIM_USER_SEM_WAIT_POL );		// Channel open
	if(ret != D_DDIM_OK){
		Ddim_Print(("Dd_HDMAC0_Open() error. ret=0x%x\n", ret));
		return;
	}

	dma_ctrl.config_a.word	 = 0;
	dma_ctrl.config_a.bit.is = D_DD_HDMAC0_IS_IDREQ_25 + (pwm_ch >> 2);
	dma_ctrl.config_a.bit.bt = D_DD_HDMAC0_BT_NORMAL;
	dma_ctrl.config_a.bit.bc = 3 - 1;				// cycle, duty, pulse
	dma_ctrl.config_a.bit.tc = 8 - 1;				// 16 - 8

	dma_ctrl.config_b.word	 = 0;
	dma_ctrl.config_b.bit.ms = D_DD_HDMAC0_MS_BLK;
	dma_ctrl.config_b.bit.tw = D_DD_HDMAC0_TW_HWORD;
	dma_ctrl.config_b.bit.fs = D_DD_HDMAC0_FS_INCR;
	dma_ctrl.config_b.bit.fd = D_DD_HDMAC0_FD_FIX;
	dma_ctrl.config_b.bit.rc = D_DD_HDMAC0_RC_DISABLE;
	dma_ctrl.config_b.bit.rs = D_DD_HDMAC0_RS_ENABLE;
	dma_ctrl.config_b.bit.rd = D_DD_HDMAC0_RD_DISABLE;

	dma_ctrl.src_addr		 = (ULONG)gCt_DD_PWM_Step_Data_p;
	dma_ctrl.dst_addr		 = (ULONG)(&(IO_PWMDMA[pwm_ch >> 2].BFDTPT));
	dma_ctrl.int_handler	 = NULL;

	ret = Dd_HDMAC0_Ctrl_Common( dma_ch, &dma_ctrl );
	if(ret != D_DDIM_OK){
		Dd_HDMAC0_Close( dma_ch );
		Ddim_Print(("Dd_HDMAC0_Ctrl_Common() error. ret=0x%x\n", ret));
		return;
	}

	ret = Dd_HDMAC0_Start_Async( dma_ch );
	if(ret != D_DDIM_OK){
		Dd_HDMAC0_Close( dma_ch );
		Ddim_Print(("Dd_HDMAC0_Start_Async() error. ret=0x%x\n", ret));
		return;
	}
	return;
}

static void ct_dd_pwm_set_timer_e_ctrl( UCHAR ch,
											T_DD_PWM_CTRL_CMN* ctrl,
											T_DD_PWM_CTRL_TIMER_E* timer_e_ctrl,
											T_DD_PWM_STEP* pwm_stp,
											UCHAR ptn_no,
											UCHAR step_data_num )
{
	UCHAR clk;
	USHORT cyc_num;
	INT32 result;
	INT32 ii;
	UCHAR ch_in_blk;

	gCt_DD_PWM_Step_Data_p = gCt_DD_PWM_Step_Data;

	result = Dd_PWM_Calculate( 60000, &clk, &cyc_num);
	if ((result != D_DDIM_OK) && (result != D_DD_PWM_OVERFLOW_OK)){
		return; 	/* Error */
	}

	ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;

	switch (ptn_no) {
		case 1: /* PWM & restart disable & soft trigger */
			ctrl->drv_mode		= D_DD_PWM_PCNHL_MDSE_PWM;		/* (0)PWM						*/
			ctrl->restart_trig	= D_DD_PWM_PCNHL_RTRG_OFF;		/* (0)restart disable			*/
			ctrl->clk_sel		= clk;
			ctrl->out_mask		= D_DD_PWM_PCNHL_PGMS_L;		/* (0)output is "Low"			*/
			ctrl->edge			= D_DD_PWM_PCNHL_EGS_RISE;		/* (1)rise edge					*/
			ctrl->int_enable	= D_DD_PWM_PCNHL_IREN_OFF;		/* (0)interrupt disable			*/
			ctrl->trigger		= D_DD_PWM_PCNHL_IRS_CYCLE; 	/* (1)agreement cycle			*/
			ctrl->polarity		= D_DD_PWM_PCNHL_OSEL_NORMAL;	/* (0)polarity usually			*/
			ctrl->in_trigger	= ch_in_blk;					/* (0-3)EN0-3 bit				*/
			ctrl->tim_e_enable	= 1;							/* Timer E enable				*/
			ctrl->ch_assign		= D_DD_PWM_GCN_CHAS_ABCD;		/* channel assignment			*/
			ctrl->pCallBack		= (VP_CALLBACK_PWM)ct_dd_pwm_callback;
			timer_e_ctrl->hold[0]		= 0;
			timer_e_ctrl->hold[1]		= 0;
			timer_e_ctrl->hold[2]		= 0;
			timer_e_ctrl->hold[3]		= 0;
			timer_e_ctrl->force_val[0]	= 0;
			timer_e_ctrl->force_val[1]	= 0;
			timer_e_ctrl->force_val[2]	= 0;
			timer_e_ctrl->force_val[3]	= 0;
			timer_e_ctrl->force[0]		= 0;
			timer_e_ctrl->force[1]		= 0;
			timer_e_ctrl->force[2]		= 0;
			timer_e_ctrl->force[3]		= 0;
			timer_e_ctrl->empty_dma_req				= D_DD_PWM_PCHSET_RQTIM_4;	/* Inerrupt request timing for DMA */
			timer_e_ctrl->undr_run_enable			= D_DD_PWM_PCHSET_UDEN_OFF; /* speed down setting under run error */
			timer_e_ctrl->undr_run_int_err_enable	= D_DD_PWM_PINTCT_UIE_ON;	/* interrupt enable under run error */
			timer_e_ctrl->force_stop_int_enable		= D_DD_PWM_PINTCT_FIE_ON;	/* interrupt enable for force stop */
			timer_e_ctrl->auto_stop_int_enable		= D_DD_PWM_PINTCT_AIE_ON;	/* interrupt enable for auto stop */
			timer_e_ctrl->dma_int_enable			= D_DD_PWM_PINTCT_DRQE_OFF; /* interrupt enable for dma */
			timer_e_ctrl->steps_data[0].cyc_num						= cyc_num;
			timer_e_ctrl->steps_data[0].duty_num					= cyc_num >> 2; 			/* 25% */
			timer_e_ctrl->steps_data[0].pulse_link.bit.pulse_num	= 5;
			timer_e_ctrl->steps_data[0].pulse_link.bit.link			= 0;
			break;

		case 2: /* DMA data transfer */
			ctrl->drv_mode		= D_DD_PWM_PCNHL_MDSE_PWM;		/* (0)PWM						*/
			ctrl->restart_trig	= D_DD_PWM_PCNHL_RTRG_OFF;		/* (0)restart disable			*/
			ctrl->clk_sel		= clk;
			ctrl->out_mask		= D_DD_PWM_PCNHL_PGMS_L;		/* (0)output is "Low"			*/
			ctrl->edge			= D_DD_PWM_PCNHL_EGS_RISE;		/* (1)rise edge					*/
			ctrl->int_enable	= D_DD_PWM_PCNHL_IREN_OFF;		/* (0)interrupt disable			*/
			ctrl->trigger		= D_DD_PWM_PCNHL_IRS_CYCLE; 	/* (1)agreement cycle			*/
			ctrl->polarity		= D_DD_PWM_PCNHL_OSEL_NORMAL;	/* (0)polarity usually			*/
			ctrl->in_trigger	= ch_in_blk;					/* (0-3)EN0-3 bit				*/
			ctrl->tim_e_enable	= 1;							/* Timer E enable				*/
			ctrl->ch_assign		= D_DD_PWM_GCN_CHAS_ABCD;		/* channel assignment			*/
			ctrl->pCallBack		= (VP_CALLBACK_PWM)ct_dd_pwm_callback;
			timer_e_ctrl->hold[0]		= 1;
			timer_e_ctrl->hold[1]		= 1;
			timer_e_ctrl->hold[2]		= 1;
			timer_e_ctrl->hold[3]		= 1;
			timer_e_ctrl->force_val[0]	= 0;
			timer_e_ctrl->force_val[1]	= 1;
			timer_e_ctrl->force_val[2]	= 1;
			timer_e_ctrl->force_val[3]	= 0;
			timer_e_ctrl->force[0]		= 1;
			timer_e_ctrl->force[1]		= 1;
			timer_e_ctrl->force[2]		= 1;
			timer_e_ctrl->force[3]		= 1;
			timer_e_ctrl->empty_dma_req				= D_DD_PWM_PCHSET_RQTIM_4;	/* Inerrupt request timing for DMA */
			timer_e_ctrl->undr_run_enable			= D_DD_PWM_PCHSET_UDEN_OFF; /* speed down setting under run error */
			timer_e_ctrl->undr_run_int_err_enable	= D_DD_PWM_PINTCT_UIE_ON;	/* interrupt enable under run error */
			timer_e_ctrl->force_stop_int_enable		= D_DD_PWM_PINTCT_FIE_ON;	/* interrupt enable for force stop */
			timer_e_ctrl->auto_stop_int_enable		= D_DD_PWM_PINTCT_AIE_ON;	/* interrupt enable for auto stop */
			timer_e_ctrl->dma_int_enable			= D_DD_PWM_PINTCT_DRQE_ON;	/* interrupt enable for dma */
			for( ii = 0; ii < D_DD_PWM_TIMER_E_STEPS; ii++ ) {
				timer_e_ctrl->steps_data[ii].cyc_num			= gCt_DD_PWM_Step_Data_p->cyc_num;
				timer_e_ctrl->steps_data[ii].duty_num			= gCt_DD_PWM_Step_Data_p->duty_num;
				timer_e_ctrl->steps_data[ii].pulse_link.hword	= gCt_DD_PWM_Step_Data_p->pulse_link.hword;
				gCt_DD_PWM_Step_Data_p++;
			}
			ct_dd_pwm_dma_start( ch, ((ch >> 2) + 1) );
			break;

		case 3: /* Stepping motor utility */
			// set timer_e ctrl data
			pwm_stp->clk_sel			= clk;
			pwm_stp->polarity			= D_DD_PWM_PCNHL_OSEL_NORMAL;
			pwm_stp->ch_assign			= D_DD_PWM_GCN_CHAS_ABCD;
			pwm_stp->hold[0]			= 1;
			pwm_stp->hold[1]			= 1;
			pwm_stp->hold[2]			= 1;
			pwm_stp->hold[3]			= 1;
			pwm_stp->force[0]			= 0;
			pwm_stp->force[1]			= 1;
			pwm_stp->force[2]			= 1;
			pwm_stp->force[3]			= 0;
			pwm_stp->step_data_num		= step_data_num;
			pwm_stp->step_data			= gCt_DD_PWM_Step_Data_p;
			pwm_stp->undr_run_data_num	= 4;
			pwm_stp->under_run_data		= gCt_PWM_Under_Run_Data_p;
			pwm_stp->pCallBack			= (VP_CALLBACK_PWM)ct_dd_pwm_callback;
			break;

		default:
			break;
	}
}

static VOID ct_dd_pwm_set_gpio(VOID)
{

	*(volatile unsigned long*)0x1DFFB104 = 0x00000040;
	
	IO_CHIPTOP.EPCR.bit.PA5 = 1;
	IO_CHIPTOP.PERSEL3.bit.AU1WPPS = 0;
	
	IO_CHIPTOP.EPCR.bit.P80 = 1;
//	IO_CHIPTOP.PDR.bit.P80 = 
	IO_CHIPTOP.DDR.bit.P80 = 1;
	IO_CHIPTOP.PUDCR.bit.P80 = 0;
	IO_CHIPTOP.PUDER.bit.P80 = 0;
	
	
	
#if 0
	// PORT6 P80-P83
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P80, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P81, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P82, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P83, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P80, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P81, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P82, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P83, 1);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P80, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P81, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P82, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P83, 0);

    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P60, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P61, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P62, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P63, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P64, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P65, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P66, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P67, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P60, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P61, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P62, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P63, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P64, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P65, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P66, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P67, 1);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P60, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P61, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P62, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P63, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P64, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P65, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P66, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P67, 0);

	// PORT7 P70-P73
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P70, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P71, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P72, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_P73, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P70, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P71, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P72, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_P73, 1);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P70, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P71, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P72, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_P73, 0);

	// PORTU PU4-PU7
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PU4, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PU5, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PU6, 1);
    Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PU7, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PU4, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PU5, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PU6, 1);
    Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PU7, 1);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PU4, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PU5, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PU6, 0);
    Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PU7, 0);
#endif
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief  Command main function for PWM timer test.
 * @param  INT32 argc	:The number of parameters
 *		   CGAR** argv	:The value of parameters
 *
 *			The meaning of parameters
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| P1		  | P2 | P3 	| P4	  | P5	|Meaning							 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| init		  |    |		|		  | 	|PWM timer initialize				 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| open		  | ch |tmout	|		  | 	|PWM timer open 					 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| open_e	  | ch |tmout	|		  | 	|PWM timer E open 					 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| set_test	  | ch |ptn		|		  | 	|PWM timer set						 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| set_test_e  | ch |ptn_e	|step_num | 	|PWM timer E set					 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| start 	  | ch |		|		  | 	|PWM timer start					 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| stop		  | ch |		|		  | 	|PWM timer stop 					 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| stop_e	  | ch |		|		  | 	|PWM timer E stop 					 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| close 	  | ch |		|		  | 	|PWM timer close					 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| close_e 	  | ch |		|		  | 	|PWM timer E close					 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| chg_wave	  | ch |chg ptn |cycle	  |duty |Change cycle & duty (norm/queue/rev)|
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| w_fact	  | ch |factor	|		  | 	|Set factor to rewrite cycle & duty  |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| get_tmr	  | ch |		|		  | 	|Get value of PWM timer register	 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| get_pls	  | ch |		|		  | 	|Get number of output pulse			 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| get_int	  | ch |		|		  | 	|Get interrupt flag and enable		 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| set_int	  | ch |PINTCT0	|PINTCT1  | 	|Set interrupt flag and enable		 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| cal_cyc	  | ch |usec	|		  | 	|Calculate cycle and clock from usec |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| cal_cycc	  | ch |usec	|clk	  | 	|Calculate cycle from usec and clock |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| set_trg	  | ch |trigger	|		  | 	|Set input trigger					 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| set_pcn	  | ch |pcn		|		  | 	|Set control status					 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| set_one	  | ch |cycle	|duty	  | 	|Set PWM timer onshot				 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| get_chas	  | ch |		|		  | 	|Get channel assignment				 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| set_chas	  | ch |chas	|		  | 	|Set channel assignment				 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| get_intstat | ch |		|		  | 	|Get interrupt status of PWM		 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| get_cmn     | ch |		|		  | 	|Get control commmon data			 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| set_stp_mask| ch |mask	|		  | 	|Set mask state						 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			| err		  |    |		|		  | 	|Parameter error test				 |
 *			+-------------+----+--------+---------+-----+------------------------------------+
 *			ch			:	0-15
 *			ptn			:	0(one-shot & restart disable & soft trigger)
 *						:	1(PWM & restart enable & soft trigger)
 *						:	2(PWM & restart disable & cycle trigger)
 *			ptn_e		:	1(DMA disable)
 *						:	2(DMA enable)
 *						:	3(Stepping motor utility)
 *			step_num	:	step numbert (only ptn_e=3)
 *			chg ptn		:	n(PWM Change cycle and duty for Normal)
 *						:	q(PWM Change cycle and duty for Queue)
 *						:	r(PWM Change cycle and duty for Reverse)
 *			restart		:	0(start trigger is only once)
 *						:	n(start trigger is (n+1))
 *			factor		:	0(CPU access)
 *						:	1(interrupt of ch0)
 *						:	2(interrupt of ch1)
 *						:	3(interrupt of ch2)
 *			clk			:	0(not dividing frequency)
 *						:	1(4 dividing frequency)
 *						:	2(16 dividing frequency)
 *						:	3(64 dividing frequency)
 *			trigger		:	0-11
 *			chas		:	1(ABCD)
 *						:	2(CBAD)
 *			mask		:	0(L output)
 *						:	1(H output)
 *  
 * @return VOID
 */
void Ct_Dd_PWM_Main(int argc, char** argv)
{
	INT32 ret = D_DDIM_OK;
	UCHAR ch;											/* Ch number */
	UCHAR w_fact;										/* rewrite cycle & duty */
	INT32 tmout = D_DDIM_USER_SEM_WAIT_POL;

	if( strcmp(argv[1], "init") == 0 ){ 				/* PWM Initialize */

		ret = Dd_GIC_Ctrl(E_DD_GIC_INTID_HDMAC0_CH0_INT,1,D_DD_GIC_PRI30,0x1);
		if(ret != D_DDIM_OK){
			Ddim_Print(("Error Dd_GIC_Ctrl: CH0!! ret = 0x%x\n", ret));
		}
		ret = Dd_GIC_Ctrl(E_DD_GIC_INTID_HDMAC0_CH2_INT,1,D_DD_GIC_PRI30,0x1);
		if(ret != D_DDIM_OK){
			Ddim_Print(("Error Dd_GIC_Ctrl: CH2!! ret = 0x%x\n", ret));
		}
		
		ct_dd_pwm_set_gpio();

		Dd_PWM_Init();

	}
	else if( strcmp(argv[1], "open") == 0 ){			/* PWM Open */
		ch = atoi(argv[2]);
		tmout = atoi(argv[3]);

		ret = Dd_PWM_Open(ch, tmout);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Open!! ret = 0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "open_e") == 0 ){			/* PWM Timer E Open */
		ch = atoi(argv[2]);
		tmout = atoi(argv[3]);

		ret = Dd_PWM_Open_Timer_E(ch, tmout);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Open_Timer_E!! ret = 0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "set_test") == 0 ){ 		 	/* PWM test Set */
		T_DD_PWM_CTRL_CMN pwm_ctrl;
		T_DD_PWM_CTRL_TIMER_ABCD pwm_ctrl_abcd;
		UCHAR set_ptn = 0;

		memset( &pwm_ctrl, 0, sizeof(T_DD_PWM_CTRL_CMN) );
		memset( &pwm_ctrl_abcd, 0, sizeof(T_DD_PWM_CTRL_TIMER_ABCD) );

		ch = atoi(argv[2]);
		set_ptn = atoi(argv[3]);						/* Setting patern no. */
		ct_dd_pwn_set_ctrl(&pwm_ctrl, &pwm_ctrl_abcd, set_ptn, ch);

		ret = Dd_PWM_Ctrl_Common(ch, &pwm_ctrl);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Ctrl_Common!! ret = 0x%x\n", ret));
		}
		ret = Dd_PWM_Ctrl_Timer_ABCD(ch, &pwm_ctrl_abcd);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Ctrl_Timer_ABCD!! ret = 0x%x\n", ret));
		}
	}
#ifdef PWM_TEST
	else if( strcmp(argv[1], "set_test2") == 0 ){ 		 	/* PWM test Set */
	//	ddpwm set_test2 ch ptn cks cycle_count
	//		ch							: 0-15
	//		ptn							: 0(one-shot & restart disable & soft trigger)
	//									: 1(PWM & restart enable & soft trigger)
	//		cks(Counter clocK Select)	: 0(RCLK)
	//									: 1(RCLK/18)
	//									: 2(RCLK/27)
	//									: 3(RCLK/64)
	//		cycle_count					: cycle_count = RCLK / division * cycle_time / 1000000
	//										ex. RCLK=13.5MHz, division=64, cycle_time = 60ms
	//											12656 = 13500000 / 64 * 60000 / 1000000
	//										ex. RCLK=13.5MHz, division=16, cycle_time = 60ms
	//											50625 = 13500000 / 16 * 60000 / 1000000
	//										ex. RCLK=18MHz, division=64, cycle_time = 60ms
	//											16875 = 18000000 / 64 * 60000 / 1000000
	//										ex. RCLK=27MHz, division=64, cycle_time = 60ms
	//											25313 = 27000000 / 64 * 60000 / 1000000
	//										ex. RCLK=54MHz, division=64, cycle_time = 60ms
	//											50625 = 54000000 / 64 * 60000 / 1000000
	//		note : duty=50% only
		T_DD_PWM_CTRL_CMN pwm_ctrl;
		T_DD_PWM_CTRL_TIMER_ABCD pwm_ctrl_abcd;
		UCHAR set_ptn = 0;
		UCHAR cks;
		ULONG cycle_num;

		memset( &pwm_ctrl, 0, sizeof(T_DD_PWM_CTRL_CMN) );
		memset( &pwm_ctrl_abcd, 0, sizeof(T_DD_PWM_CTRL_TIMER_ABCD) );

		ch = atoi(argv[2]);
		set_ptn = atoi(argv[3]);						/* Setting patern no. */
		cks = atoi(argv[4]);
		if(cks > 3){
			Ddim_Print(("input 4th parameter error (%d)\n", cks));
			return;
		}
		cycle_num = atol(argv[5]);
		if(cycle_num > 0xFFFF){
			Ddim_Print(("input 5th parameter error (Max value = 0xFFFF)\n"));
			return;
		}
		
		ct_dd_pwn_set_ctrl2(&pwm_ctrl, &pwm_ctrl_abcd, set_ptn, ch, cks, cycle_num);

		Ddim_Print(("peri_clock             = %ld\n", Dd_Top_Get_RCLK()));
		Ddim_Print(("pwm_ctrl.clk_sel       = %d\n", pwm_ctrl.clk_sel));
		Ddim_Print(("pwm_ctrl_abcd.cyc_num  = %d\n", pwm_ctrl_abcd.cyc_num));
		Ddim_Print(("pwm_ctrl_abcd.duty_num = %d\n", pwm_ctrl_abcd.duty_num));

		ret = Dd_PWM_Ctrl_Common(ch, &pwm_ctrl);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Ctrl_Common!! ret = 0x%x\n", ret));
		}
		ret = Dd_PWM_Ctrl_Timer_ABCD(ch, &pwm_ctrl_abcd);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Ctrl_Timer_ABCD!! ret = 0x%x\n", ret));
		}
	}
#endif
	else if( strcmp(argv[1], "set_test_e") == 0 ){			 /* PWM timer E test set */
		T_DD_PWM_CTRL_CMN pwm_ctrl;
		T_DD_PWM_CTRL_TIMER_E timer_e_ctrl;
		T_DD_PWM_STEP pwm_stp;
		UCHAR set_ptn;
		UCHAR dma_ch;
		UCHAR step_num = 0;

		memset( &pwm_ctrl, 0, sizeof(T_DD_PWM_CTRL_CMN) );
		memset( &timer_e_ctrl, 0, sizeof(T_DD_PWM_CTRL_TIMER_E) );
		memset( &pwm_stp, 0, sizeof(T_DD_PWM_STEP) );

		ch = atoi(argv[2]);
		dma_ch = atoi(argv[3]);
		set_ptn = atoi(argv[4]);						/* Setting patern no. */
		if( set_ptn == 3 ){
			step_num = atoi(argv[5]);
		}

		ct_dd_pwm_set_timer_e_ctrl(ch, &pwm_ctrl, &timer_e_ctrl, &pwm_stp, set_ptn, step_num );

		if( set_ptn == 3 ){
			ret = Dd_PWM_Write_Step(ch, dma_ch, &pwm_stp);
			if(ret != 0){
				Ddim_Print(("Error Dd_PWM_Write_Step!! ret = 0x%x\n", ret));
			}
		}
		else{
			ret = Dd_PWM_Ctrl_Common(ch, &pwm_ctrl);
			if(ret != D_DDIM_OK){
				Ddim_Print(("Error Dd_PWM_Ctrl_Common!! ret = 0x%x\n", ret));
			}
			ret = Dd_PWM_Ctrl_Timer_E(ch, &timer_e_ctrl);
			if(ret != D_DDIM_OK){
				Ddim_Print(("Error Dd_PWM_Ctrl_Timer_E!! ret = 0x%x\n", ret));
			}
		}
	}
	else if( strcmp(argv[1], "start") == 0 ){			/* PWM Start */

		ch = atoi(argv[2]);
		ret = Dd_PWM_Start(ch);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Start!! ret = 0x%x\n", ret));
			return;
		}
		
		USER_LED13_On();
		Ddim_Print(("PWM Start !!\n"));
	}
	else if( strcmp(argv[1], "start_e") == 0 ){			/* PWM E Start */

		ch = atoi(argv[2]);
		ret = Dd_PWM_Start(ch);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Start!! ret = 0x%x\n", ret));
			return;
		}
		
		ret = Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR);
		if(ret != D_DDIM_OK){
			Ddim_Print(("Error Dd_HDMAC0_Open!! ret = 0x%x\n", ret));
		}		
		ret = Dd_HDMAC0_Start_Async(gDMA_CH);
		if(ret != D_DDIM_OK){
			Ddim_Print(("Error Dd_HDMAC0_Start!! ret = 0x%x\n", ret));
		}		
		Ddim_Print(("PWM E Start !!\n"));
	}
	else if( strcmp(argv[1], "stop") == 0 ){			/* PWM Stop */
		ch = atoi(argv[2]);
		ret = Dd_PWM_Stop(ch);
		USER_LED13_Off();
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Stop!! ret = 0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "stop_e") == 0 ){			/* PWM E Stop */
		ch = atoi(argv[2]);
		ret = Dd_PWM_Stop(ch);
		USER_LED13_Off();
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Stop!! ret = 0x%x\n", ret));
		}
		ret = Dd_HDMAC0_Stop(gDMA_CH);
		if(ret != D_DDIM_OK){
			Ddim_Print(("Error Dd_HDMAC0_Stop!! ret = 0x%x\n", ret));
		}		
		ret = Dd_HDMAC0_Close(gDMA_CH);
		if(ret != D_DDIM_OK){
			Ddim_Print(("Error Dd_HDMAC0_Close!! ret = 0x%x\n", ret));
		}		
	}
	else if( strcmp(argv[1], "close") == 0 ){			/* PWM Close */
		ch = atoi(argv[2]);
		ret = Dd_PWM_Close(ch);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Close!! ret = 0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "close_e") == 0 ){			/* PWM E Close */
		ch = atoi(argv[2]);
		ret = Dd_PWM_Close_Timer_E(ch);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Close_Timer_E!! ret = 0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "chg_wave") == 0 ){		/* PWM Change cycle and duty */
		USHORT cycle = 1;
		UCHAR duty;
		UCHAR clk = 1;

		ch = atoi(argv[2]);
		duty = atoi(argv[5]);

		// cycle time(usec) check
		if (atoi(argv[4])){
			// cycle time(usec) convert
			ret = Dd_PWM_Calculate( atoi(argv[4]), &clk, &cycle);
			if (ret != D_DDIM_OK){
				Ddim_Print(("Error Dd_PWM_Calculate!! ret = 0x%x\n", ret));
				return;
			}
		}

		if( strcmp(argv[3], "n") == 0 ){			/* PWM Change cycle and duty for Normal */
			ret = Dd_PWM_Write_Cycle_Duty(ch, cycle, duty, clk);
			if(ret != 0){
				Ddim_Print(("Error Dd_PWM_Write_Cycle_Duty!! ret = 0x%x\n", ret));
			}
		}
		else if( strcmp(argv[3], "q") == 0 ){		 /* PWM Change cycle and duty for Queue */
			ret = Dd_PWM_Write_Cycle_Duty_Queue(ch, cycle, duty, clk);
			if(ret != 0){
				Ddim_Print(("Error Dd_PWM_Write_Cycle_Duty_Queue!! ret = 0x%x\n", ret));
			}
		}
		else if( strcmp(argv[3], "r") == 0 ){	 	/* PWM Change cycle and duty for Reverse */
			ret = Dd_PWM_Write_Cycle_Duty_Reverse(ch, cycle, duty, clk);
			if(ret != 0){
				Ddim_Print(("Error Dd_PWM_Write_Cycle_Duty_Reverse!! ret = 0x%x\n", ret));
			}
		}
		else{
			Ddim_Print( ("please check 3rd parameter!!\n" ) );
		}
	}
	else if( strcmp(argv[1], "w_fact") == 0 ){		/* PWM Set factor to rewrite cycle & duty */
		ch = atoi(argv[2]);
		w_fact = atoi(argv[3]);

		ret = Dd_PWM_Write_Cycle_Duty_Switch(ch, w_fact);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Write_Cycle_Duty_Switch!! ret = 0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "get_tmr") == 0 ){		/* Get value of PWM timer register */
		USHORT timer_val, timer_e_val;

		ch = atoi(argv[2]);

		ret = Dd_PWM_Get_Timer(ch, &timer_val, &timer_e_val);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Get_Timer!! ret = 0x%x\n", ret));
		}
		else{
			Ddim_Print(("PWM Timer Value:\n"));
			Ddim_Print(("    ABCD = 0x%x\n", timer_val));
			Ddim_Print(("    E    = 0x%x\n", timer_e_val));
		}
	}
	else if( strcmp(argv[1], "get_pls") == 0 ){		/* Get number of output pulse */
		ULONG pulse_num;

		ch = atoi(argv[2]);

		ret = Dd_PWM_Get_Pulse_Num(ch, &pulse_num);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Get_Pulse_Num!! ret = 0x%x\n", ret));
		}
		else{
			Ddim_Print(("PWM Output Pulse Number = %lu\n", pulse_num));
		}
	}
	else if( strcmp(argv[1], "get_int") == 0 ){		/* Get interrupt flag and enable for timer E */
		USHORT int_flg_ctrl_0, int_flg_ctrl_1;

		ch = atoi(argv[2]);

		ret = Dd_PWM_Get_Interrupt_Flag(ch, &int_flg_ctrl_0, &int_flg_ctrl_1);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Get_Interrupt_Flag!! ret = 0x%x\n", ret));
		}
		else{
			Ddim_Print(("PWM Interrupt Flag:\n"));
			Ddim_Print(("    PINTCT0 = 0x%x\n", int_flg_ctrl_0));
			Ddim_Print(("    PINTCT1 = 0x%x\n", int_flg_ctrl_1));
		}
	}
	else if( strcmp(argv[1], "set_int") == 0 ){		/* Set interrupt flag and enable for timer E */
		USHORT int_flg_ctrl_0, int_flg_ctrl_1;

		ch = atoi(argv[2]);
		int_flg_ctrl_0 = strtol(argv[3], NULL, 16);
		int_flg_ctrl_1 = strtol(argv[4], NULL, 16);

		ret = Dd_PWM_Set_Interrupt_Flag(ch, int_flg_ctrl_0, int_flg_ctrl_1); /* pgr0539 */
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Set_Interrupt_Flag!! ret = 0x%x\n", ret));
		}
		else{
			Ddim_Print(("PWM Interrupt Flag Set OK.\n"));
		}
	}
	else if( strcmp(argv[1], "cal_cyc") == 0 ){		/* Calculate cycle and clock from usec */
		ULONG usec;
		UCHAR clk;
		USHORT cycle;

		usec = atoi(argv[2]);

		ret = Dd_PWM_Calculate(usec, &clk, &cycle);
		if ((ret != D_DDIM_OK) && (ret != D_DD_PWM_OVERFLOW_OK)){
			Ddim_Print(("Error Dd_PWM_Calculate!! ret = 0x%x\n", ret))
		}
		else{
			Ddim_Print(("PWM Calculate:\n"));
			Ddim_Print(("    usec        = %lu\n", usec));
			Ddim_Print(("    clock index = %d\n", clk));
			Ddim_Print(("    cycle       = %d\n", cycle));
		}
	}
	else if( strcmp(argv[1], "cal_cycc") == 0 ){		/* Calculate cycle from usec and clock */
		ULONG usec;
		UCHAR clk;
		USHORT cycle;

		usec = atoi(argv[2]);
		clk = atoi(argv[3]);

		ret = Dd_PWM_Calculate_Cycle(usec, clk, &cycle);
		if ((ret != D_DDIM_OK) && (ret != D_DD_PWM_OVERFLOW_OK)){
			Ddim_Print(("Error Dd_PWM_Calculate_Cycle!! ret = 0x%x\n", ret))
		}
		else{
			Ddim_Print(("PWM Calculate Cycle:\n"));
			Ddim_Print(("    usec        = %lu\n", usec));
			Ddim_Print(("    clock index = %d\n", clk));
			Ddim_Print(("    cycle       = %d\n", cycle));
		}
	}
	else if( strcmp(argv[1], "set_trg") == 0 ){		/* Set input trigger */
		UCHAR gcn_tsel;

		ch = atoi(argv[2]);
		gcn_tsel = atoi(argv[3]);

		ret = Dd_PWM_Write_Input_Trg(ch, gcn_tsel);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Write_Input_Trg!! ret = 0x%x\n", ret));
		}
		else{
			Ddim_Print(("PWM Input Trigger Set OK.\n"));
		}
	}
	else if( strcmp(argv[1], "set_pcn") == 0 ){		/* Set control status */
		USHORT pcn;

		ch = atoi(argv[2]);
		pcn = atoi(argv[3]);

		ret = Dd_PWM_Write_Pcn(ch, pcn);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Write_Pcn!! ret = 0x%x\n", ret));
		}
		else{
			Ddim_Print(("PWM control status set OK.\n"));
		}
	}
	else if( strcmp(argv[1], "set_one") == 0 ){		/* Set PWM timer onshot */
		USHORT cyc_time, duty_time;
		T_DD_PWM_CTRL_CMN ctrl;
		T_DD_PWM_CTRL_TIMER_ABCD ctrl_abcd;

		memset( &ctrl, 0, sizeof(T_DD_PWM_CTRL_CMN) );
		memset( &ctrl_abcd, 0, sizeof(T_DD_PWM_CTRL_TIMER_ABCD) );

		ch = atoi(argv[2]);
		cyc_time = atoi(argv[3]);
		duty_time = atoi(argv[4]);

		ctrl.restart_trig	= D_DD_PWM_PCNHL_RTRG_ON;		/* (1)restart enable			*/
		ctrl.out_mask		= D_DD_PWM_PCNHL_PGMS_L;		/* (0)output is "Low"			*/
		ctrl.edge			= D_DD_PWM_PCNHL_EGS_RISE;		/* (1)rise edge					*/
		ctrl.trigger		= D_DD_PWM_PCNHL_IRS_CYCLE; 	/* (1)agreement cycle			*/
		ctrl.polarity		= D_DD_PWM_PCNHL_OSEL_NORMAL;	/* (0)polarity usually			*/
		ctrl.in_trigger		= (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;	/* trigger input factor	*/
		ctrl.tim_e_enable	= 0;							/* Timer E disable				*/
		ctrl.ch_assign		= D_DD_PWM_GCN_CHAS_ABCD;		/* channel assignment			*/
		ctrl.pCallBack		= (VP_CALLBACK_PWM)ct_dd_pwm_callback; /* callback function pointer	*/

		ctrl_abcd.cyc_time	= cyc_time;				/* wave cycle(usec)		*/
		ctrl_abcd.cyc_num	= 0;					/* number of cycle		*/
		ctrl_abcd.duty_num	= 0;					/* number of duty 		*/

		ret = Dd_PWM_Write_Oneshot(ch, duty_time, &ctrl, &ctrl_abcd);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Write_Oneshot!! ret = 0x%x\n", ret));
		}
		else{
			Ddim_Print(("PWM timer onshot set OK.\n"));
		}
	}
	else if( strcmp(argv[1], "get_chas") == 0 ){		/* Get channel assignment */
		UCHAR ch_assign;

		ch = atoi(argv[2]);

		ret = Dd_PWM_Get_Channel_Assign(ch, &ch_assign);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Get_Channel_Assign!! ret = 0x%x\n", ret));
		}
		else{
			Ddim_Print(("PWM Channel Assignment:\n"));
			Ddim_Print(("    CH[%d] = %d\n", ch, ch_assign));
		}
	}
	else if( strcmp(argv[1], "set_chas") == 0 ){		/* Set channel assignment */
		UCHAR ch_assign;

		ch = atoi(argv[2]);
		ch_assign = atoi(argv[3]);

		ret = Dd_PWM_Set_Channel_Assign(ch, ch_assign);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Set_Channel_Assign!! ret = 0x%x\n", ret));
		}
		else{
			Ddim_Print(("PWM Channel Assignment Set OK.\n"));
		}
	}
	else if( strcmp(argv[1], "get_intstat") == 0 ){		/* Get interrupt status of PWM */
		ch = atoi(argv[2]);

		Dd_ARM_Critical_Section_Start( gDD_Pwm_Spin_Lock );
		ret = Dd_PWM_Get_Int_Stat( ch );
		Dd_ARM_Critical_Section_End( gDD_Pwm_Spin_Lock );

		Ddim_Print(("PWM interrupt status [ch%d=%d]\n", ch, ret));
	}
	else if( strcmp(argv[1], "get_cmn") == 0 ){		/* Get conrtol common data */
		T_DD_PWM_CTRL_CMN ctrl_cmn;

		memset( &ctrl_cmn, 0, sizeof(T_DD_PWM_CTRL_CMN) );

		ch = atoi(argv[2]);

		ret = Dd_PWM_Get_Ctrl_Common(ch, &ctrl_cmn);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Get Ctrl !!\n"));
		}

		Ddim_Print(("---PWM[%d] Get Ctrl Data---\n", ch));
		Ddim_Print(("drv_mode     =%d\n", ctrl_cmn.drv_mode));
		Ddim_Print(("clk_sel      =%d\n", ctrl_cmn.clk_sel));
		Ddim_Print(("restart_trig =%d\n", ctrl_cmn.restart_trig));
		Ddim_Print(("output mask  =%d\n", ctrl_cmn.out_mask));
		Ddim_Print(("edge         =%d\n", ctrl_cmn.edge));
		Ddim_Print(("int_enable   =%d\n", ctrl_cmn.int_enable));
		Ddim_Print(("trigger      =%d\n", ctrl_cmn.trigger));
		Ddim_Print(("polarity     =%d\n", ctrl_cmn.polarity));
		Ddim_Print(("in_trigger   =%d\n", ctrl_cmn.in_trigger));
		Ddim_Print(("tim_e_enable =%d\n", ctrl_cmn.tim_e_enable));
		Ddim_Print(("ch_assign    =%d\n", ctrl_cmn.ch_assign));
		Ddim_Print(("pCallBack    =%10p\n", ctrl_cmn.pCallBack));
	}
	else if( strcmp(argv[1], "set_stp_mask") == 0 ){		/* Set mask state */
		UCHAR mask;

		ch = atoi(argv[2]);
		mask = atoi(argv[3]);

		ret = Dd_PWM_Set_Stopped_OutPut_Mask(ch, mask);
		if(ret != 0){
			Ddim_Print(("Error Dd_PWM_Set_Mask!! ret = 0x%x\n", ret));
		}
		else{
			Ddim_Print(("PWM Stop Mask Set OK.\n"));
		}
	}
	else if( strcmp(argv[1], "get_stp_mask") == 0 ){		/* Get mask state */
		UCHAR mask;

		ch = atoi(argv[2]);

		ret = Dd_PWM_Get_Stopped_OutPut_Mask(ch, &mask);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Get Mask !!\n"));
		}

		Ddim_Print(("---PWM[%d] Get Mask---\n", ch));
		Ddim_Print(("mask     =%d\n", mask));
	}
	else if( strcmp(argv[1], "get_stp_out") == 0 ){		/* Get PWM stop status */
		UCHAR ch_status;

		ch = atoi(argv[2]);

		ret = Dd_PWM_Get_Stopped_OutPut_Status(ch, &ch_status);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Get stp_out !!\n"));
		}

		Ddim_Print(("---PWM[%d] Get Stop Output---\n", ch));
		Ddim_Print(("status     =%d\n", ch_status));
	}
	else if (strcmp(argv[1], "err") == 0){
		T_DD_PWM_CTRL_CMN pwm_ctrl;
		T_DD_PWM_CTRL_TIMER_ABCD pwm_ctrl_abcd;
		T_DD_PWM_CTRL_TIMER_E pwm_ctrl_e;
		USHORT timer_val, timer_e_val;
		ULONG pulse_num;
		USHORT int_flg_ctrl_0 = 0, int_flg_ctrl_1 = 0;
		UCHAR ch_assign = 0;
		UCHAR clk;
		USHORT cycle;
		T_DD_PWM_STEP pwm_stp;

		memset( &pwm_ctrl, 0, sizeof(T_DD_PWM_CTRL_CMN) );
		memset( &pwm_ctrl_abcd, 0, sizeof(T_DD_PWM_CTRL_TIMER_ABCD) );
		memset( &pwm_ctrl_e, 0, sizeof(T_DD_PWM_CTRL_TIMER_E) );
		memset( &pwm_stp, 0, sizeof(T_DD_PWM_STEP) );

		switch ( atoi(argv[2]) ) {
			case 0 :
				ret = Dd_PWM_Open( D_DD_PWM_CH_NUM_MAX, tmout );
				Ddim_Print(("Dd_PWM_Open(): CH number error. ret=%X\n", ret ));
				break;
			case 1 :
				ret = Dd_PWM_Open( 0, -2 );
				Ddim_Print(("Dd_PWM_Open(): tmout value error. ret=%X\n", ret ));
				break;
			case 2 :
				ret = Dd_PWM_Close( D_DD_PWM_CH_NUM_MAX );
				Ddim_Print(("Dd_PWM_Close(): CH number error. ret=%X\n", ret ));
				break;
			case 3 :
				ret = Dd_PWM_Open_Timer_E( 1, tmout );
				Ddim_Print(("Dd_PWM_Open_Timer_E(): CH number error. ret=%X\n", ret ));
				break;
			case 4 :
				ret = Dd_PWM_Open_Timer_E( 0, -2 );
				Ddim_Print(("Dd_PWM_Open_Timer_E(): tmout value error. ret=%X\n", ret ));
				break;
			case 5 :
				ret = Dd_PWM_Close_Timer_E( 5 );
				Ddim_Print(("Dd_PWM_Close_Timer_E(): CH number error. ret=%X\n", ret ));
				break;
			case 6 :
				ret = Dd_PWM_Ctrl_Common( D_DD_PWM_CH_NUM_MAX, &pwm_ctrl );
				Ddim_Print(("Dd_PWM_Ctrl_Common(): CH number error. ret=%X\n", ret ));
				break;
			case 7 :
				ret = Dd_PWM_Ctrl_Common( 0, NULL );
				Ddim_Print(("Dd_PWM_Ctrl_Common(): pwm_ctrl is NULL. ret=%X\n", ret ));
				break;
			case 8 :
				ret = Dd_PWM_Ctrl_Timer_ABCD( D_DD_PWM_CH_NUM_MAX, &pwm_ctrl_abcd );
				Ddim_Print(("Dd_PWM_Ctrl_Timer_ABCD(): CH number error. ret=%X\n", ret ));
				break;
			case 9 :
				ret = Dd_PWM_Ctrl_Timer_ABCD( 0, NULL );
				Ddim_Print(("Dd_PWM_Ctrl_Timer_ABCD(): pwm_ctrl_abcd is NULL. ret=%X\n", ret ));
				break;
			case 10 :
				ret = Dd_PWM_Ctrl_Timer_E( D_DD_PWM_CH_NUM_MAX, &pwm_ctrl_e );
				Ddim_Print(("Dd_PWM_Ctrl_Timer_E(): CH number error. ret=%X\n", ret ));
				break;
			case 11 :
				ret = Dd_PWM_Ctrl_Timer_E( 0, NULL );
				Ddim_Print(("Dd_PWM_Ctrl_Timer_E(): pwm_ctrl_e is NULL. ret=%X\n", ret ));
				break;
			case 12 :
				ret = Dd_PWM_Start( D_DD_PWM_CH_NUM_MAX );
				Ddim_Print(("Dd_PWM_Start(): CH number error. ret=%X\n", ret ));
				break;
			case 13 :
				ret = Dd_PWM_Stop( D_DD_PWM_CH_NUM_MAX );
				Ddim_Print(("Dd_PWM_Stop(): CH number error. ret=%X\n", ret ));
				break;
			case 14 :
				ret = Dd_PWM_Get_Timer(D_DD_PWM_CH_NUM_MAX, &timer_val, &timer_e_val);
				Ddim_Print(("Dd_PWM_Get_Timer(): CH number error. ret=%X\n", ret ));
				break;
			case 15 :
				ret = Dd_PWM_Get_Timer(0, NULL, &timer_e_val);
				Ddim_Print(("Dd_PWM_Get_Timer(): timer_val is NULL. ret=%X\n", ret ));
				break;
			case 16 :
				ret = Dd_PWM_Get_Timer(0, &timer_val, NULL);
				Ddim_Print(("Dd_PWM_Get_Timer(): timer_e_val is NULL. ret=%X\n", ret ));
				break;
			case 17 :
				ret = Dd_PWM_Get_Pulse_Num(D_DD_PWM_CH_NUM_MAX, &pulse_num);
				Ddim_Print(("Dd_PWM_Get_Pulse_Num(): CH number error. ret=%X\n", ret ));
				break;
			case 18 :
				ret = Dd_PWM_Get_Pulse_Num(0, NULL);
				Ddim_Print(("Dd_PWM_Get_Pulse_Num(): pulse_num is NULL. ret=%X\n", ret ));
				break;
			case 19 :
				ret = Dd_PWM_Get_Interrupt_Flag(D_DD_PWM_CH_NUM_MAX, &int_flg_ctrl_0, &int_flg_ctrl_1);
				Ddim_Print(("Dd_PWM_Get_Interrupt_Flag(): CH number error. ret=%X\n", ret ));
				break;
			case 20 :
				ret = Dd_PWM_Get_Interrupt_Flag(0, NULL, &int_flg_ctrl_1);
				Ddim_Print(("Dd_PWM_Get_Interrupt_Flag(): int_flg_ctrl_0 is NULL. ret=%X\n", ret ));
				break;
			case 21 :
				ret = Dd_PWM_Get_Interrupt_Flag(0, &int_flg_ctrl_0, NULL);
				Ddim_Print(("Dd_PWM_Get_Interrupt_Flag(): int_flg_ctrl_1 is NULL. ret=%X\n", ret ));
				break;
			case 22 :
				ret = Dd_PWM_Set_Interrupt_Flag(D_DD_PWM_CH_NUM_MAX, int_flg_ctrl_0, int_flg_ctrl_1);
				Ddim_Print(("Dd_PWM_Set_Interrupt_Flag(): CH number error. ret=%X\n", ret ));
				break;
			case 23 :
				ret = Dd_PWM_Get_Channel_Assign(D_DD_PWM_CH_NUM_MAX, &ch_assign);
				Ddim_Print(("Dd_PWM_Get_Channel_Assign(): CH number error. ret=%X\n", ret ));
				break;
			case 24 :
				ret = Dd_PWM_Get_Channel_Assign(0, NULL);
				Ddim_Print(("Dd_PWM_Get_Channel_Assign(): ch_assign is NULL. ret=%X\n", ret ));
				break;
			case 25 :
				ret = Dd_PWM_Set_Channel_Assign(D_DD_PWM_CH_NUM_MAX, ch_assign);
				Ddim_Print(("Dd_PWM_Set_Channel_Assign(): CH number error. ret=%X\n", ret ));
				break;
			case 26 :
				ret = Dd_PWM_Set_Channel_Assign(0, (D_DD_PWM_GCN_CHAS_CBAD + 1));
				Ddim_Print(("Dd_PWM_Set_Channel_Assign(): ch_assign error. ret=%X\n", ret ));
				break;
			case 27 :
				ret = Dd_PWM_Get_Int_Stat(D_DD_PWM_CH_NUM_MAX);
				Ddim_Print(("Dd_PWM_Get_Int_Stat(): CH number error. ret=%X\n", ret ));
				break;
			case 28 :
				Dd_PWM_Int_Handler(D_DD_PWM_CH_NUM_MAX);
				Ddim_Print(("Dd_PWM_Int_Handler(): CH number error.\n"));
				break;
			case 29 :
				Dd_PWM_Int_Handler_STP(D_DD_PWM_CH_NUM_MAX);
				Ddim_Print(("Dd_PWM_Int_Handler_STP(): CH number error.\n"));
				break;
			case 30 :
				ret = Dd_PWM_Calculate(0, &clk, &cycle);
				Ddim_Print(("Dd_PWM_Calculate(): usec error. ret=%X\n", ret ));
				break;
			case 31 :
				ret = Dd_PWM_Calculate(60000, NULL, &cycle);
				Ddim_Print(("Dd_PWM_Calculate(): clk is NULL. ret=%X\n", ret ));
				break;
			case 32 :
				ret = Dd_PWM_Calculate(60000, &clk, NULL);
				Ddim_Print(("Dd_PWM_Calculate(): cycle is NULL. ret=%X\n", ret ));
				break;
			case 33 :
				ret = Dd_PWM_Calculate_Cycle(0, 3, &cycle);
				Ddim_Print(("Dd_PWM_Calculate_Cycle(): usec error. ret=%X\n", ret ));
				break;
			case 34 :
				ret = Dd_PWM_Calculate_Cycle(60000, D_DD_PWM_PCNHL_CKS_DIV_64+1, &cycle);
				Ddim_Print(("Dd_PWM_Calculate_Cycle(): clk error. ret=%X\n", ret ));
				break;
			case 35 :
				ret = Dd_PWM_Calculate_Cycle(60000, 3, NULL);
				Ddim_Print(("Dd_PWM_Calculate_Cycle(): cycle is NULL. ret=%X\n", ret ));
				break;
			case 36 :
				ret = Dd_PWM_Write_Cycle_Duty(D_DD_PWM_CH_NUM_MAX, 50000, 50, 3);
				Ddim_Print(("Dd_PWM_Write_Cycle_Duty(): CH number error. ret=%X\n", ret ));
				break;
			case 37 :
				ret = Dd_PWM_Write_Cycle_Duty(0, 0, 50, 3);
				Ddim_Print(("Dd_PWM_Write_Cycle_Duty(): cycle error. ret=%X\n", ret ));
				break;
			case 38 :
				ret = Dd_PWM_Write_Cycle_Duty(0, 50000, 0, 3);
				Ddim_Print(("Dd_PWM_Write_Cycle_Duty(): duty error. ret=%X\n", ret ));
				break;
			case 39 :
				ret = Dd_PWM_Write_Cycle_Duty(0, 50000, 50, D_DD_PWM_PCNHL_CKS_DIV_64+1);
				Ddim_Print(("Dd_PWM_Write_Cycle_Duty(): clk_sel error. ret=%X\n", ret ));
				break;
			case 40 :
				ret = Dd_PWM_Write_Cycle_Duty_Reverse(D_DD_PWM_CH_NUM_MAX, 50000, 50, 3);
				Ddim_Print(("Dd_PWM_Write_Cycle_Duty_Reverse(): CH number error. ret=%X\n", ret ));
				break;
			case 41 :
				ret = Dd_PWM_Write_Cycle_Duty_Reverse(0, 0, 50, 3);
				Ddim_Print(("Dd_PWM_Write_Cycle_Duty_Reverse(): cycle error. ret=%X\n", ret ));
				break;
			case 42 :
				ret = Dd_PWM_Write_Cycle_Duty_Reverse(0, 50000, 0, 3);
				Ddim_Print(("Dd_PWM_Write_Cycle_Duty_Reverse(): duty error. ret=%X\n", ret ));
				break;
			case 43 :
				ret = Dd_PWM_Write_Cycle_Duty_Reverse(0, 50000, 50, D_DD_PWM_PCNHL_CKS_DIV_64+1);
				Ddim_Print(("Dd_PWM_Write_Cycle_Duty_Reverse(): clk_sel error. ret=%X\n", ret ));
				break;
			case 44 :
				ret = Dd_PWM_Write_Cycle_Duty_Queue(D_DD_PWM_CH_NUM_MAX, 50000, 50, 3);
				Ddim_Print(("Dd_PWM_Write_Cycle_Duty_Queue(): CH number error. ret=%X\n", ret ));
				break;
			case 45 :
				ret = Dd_PWM_Write_Cycle_Duty_Queue(0, 0, 50, 3);
				Ddim_Print(("Dd_PWM_Write_Cycle_Duty_Queue(): cycle error. ret=%X\n", ret ));
				break;
			case 46 :
				ret = Dd_PWM_Write_Cycle_Duty_Queue(0, 50000, 0, 3);
				Ddim_Print(("Dd_PWM_Write_Cycle_Duty_Queue(): duty error. ret=%X\n", ret ));
				break;
			case 47 :
				ret = Dd_PWM_Write_Cycle_Duty_Queue(0, 50000, 50, D_DD_PWM_PCNHL_CKS_DIV_64+1);
				Ddim_Print(("Dd_PWM_Write_Cycle_Duty_Queue(): clk_sel error. ret=%X\n", ret ));
				break;
			case 48 :
				ret = Dd_PWM_Write_Input_Trg(D_DD_PWM_CH_NUM_MAX, 0);
				Ddim_Print(("Dd_PWM_Write_Input_Trg(): CH number error. ret=%X\n", ret ));
				break;
			case 49 :
				ret = Dd_PWM_Write_Cycle_Duty_Switch(D_DD_PWM_CH_NUM_MAX, 0);
				Ddim_Print(("Dd_PWM_Write_Cycle_Duty_Switch(): CH number error. ret=%X\n", ret ));
				break;
			case 50 :
				ret = Dd_PWM_Write_Pcn(D_DD_PWM_CH_NUM_MAX, 0);
				Ddim_Print(("Dd_PWM_Write_Pcn(): CH number error. ret=%X\n", ret ));
				break;
			case 51 :
				ret = Dd_PWM_Write_Oneshot(D_DD_PWM_CH_NUM_MAX, 30000, &pwm_ctrl, &pwm_ctrl_abcd);
				Ddim_Print(("Dd_PWM_Write_Oneshot(): CH number error. ret=%X\n", ret ));
				break;
			case 52 :
				ret = Dd_PWM_Write_Oneshot(0, 30000, NULL, &pwm_ctrl_abcd);
				Ddim_Print(("Dd_PWM_Write_Oneshot(): pwm_ctrl is NULL. ret=%X\n", ret ));
				break;
			case 53 :
				ret = Dd_PWM_Write_Oneshot(0, 30000, &pwm_ctrl, NULL);
				Ddim_Print(("Dd_PWM_Write_Oneshot(): pwm_ctrl_abcd is NULL. ret=%X\n", ret ));
				break;
			case 54 :
				ret = Dd_PWM_Write_Step(9, 0, &pwm_stp);
				Ddim_Print(("Dd_PWM_Write_Step(): CH number error. ret=%X\n", ret ));
				break;
			case 55 :
				ret = Dd_PWM_Write_Step(0, D_DD_HDMAC0_CH_NUM_MAX, &pwm_stp);
				Ddim_Print(("Dd_PWM_Write_Step(): DMA CH number error. ret=%X\n", ret ));
				break;
			case 56 :
				ret = Dd_PWM_Write_Step(0, 0, NULL);
				Ddim_Print(("Dd_PWM_Write_Step(): pwm_stp is NULL. ret=%X\n", ret ));
				break;
			default :
				Ddim_Print(("please check 2nd parameter!!\n"));
				break;
		}
	}
	else{
		Ddim_Print( ("please check 1st parameter!!\n" ) );
	}
}

