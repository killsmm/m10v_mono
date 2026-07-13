/**
 * @file		ct_im_ela.c
 * @brief		ct code for im_ela.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h> 

#include "jdsela.h"
#include "im_ela.h"
#include "ct_im_ela.h"
#include "driver_common.h"
#include "dd_top.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* sync type */
#define	D_IM_ELA_S_SYNC_SEL_SYNC				(0)		/* SYNC				*/
#define	D_IM_ELA_S_SYNC_SEL_ASYNC_CALLBACK		(1)		/* ASYNC Callback	*/
#define	D_IM_ELA_S_SYNC_SEL_ASYNC_WAIT			(2)		/* ASYNC Wait End	*/
#define	D_IM_ELA_S_SYNC_SEL_UNKNOWN				(3)		/* Unknown			*/

#define	CT_IM_ELA_SET_TEST_DATA
//#define	CT_IM_ELA_MSG_PRINT_ON

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
/* Execution watch flag of ELA */

#ifdef CO_ACT_PCLOCK
// PCLK_ELA setting task counter (0~255)
static UCHAR gCT_IM_ELA_PCLK_Counter = 0;
#endif //CO_ACT_PCLOCK

#ifdef CO_ACT_ICLOCK
extern UCHAR gIM_ELA_CLK_Counter;
extern UCHAR gIM_ELA_ICLK_Counter;
#endif //CO_ACT_ICLOCK

//extern volatile T_IM_ELA_MULTIPLE_TIMES_INFO		gIM_ELA_Multiple_Exec_Info;

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Command function		*/
#if 0
static UCHAR	ct_im_ela_cnv_param_2_syncsel( UCHAR p_sync );
static UINT32	ct_im_ela_cnv_param_2_deknee( UCHAR p_deknee, UCHAR* deknee_enable );
static UINT32	ct_im_ela_cnv_param_2_top_offset( UCHAR p_top_offset, UCHAR p_dtype, UCHAR* top_offset );
static UINT32	ct_im_ela_cnv_param_2_size( UCHAR p_size, USHORT* hsize );
static UINT32	ct_im_ela_cnv_param_2_noise_offset( UCHAR p_offset );
static UINT32	ct_im_ela_cnv_param_2_noise_gain( UCHAR p_gain );
static UINT32	ct_im_ela_cnv_param_2_noise_border( UCHAR p_border );
static UINT32	ct_im_ela_cnv_param_2_noise_thresh_offset( UCHAR p_offset );
static UINT32	ct_im_ela_cnv_param_2_noise_thresh_gain( UCHAR p_gain );
static UINT32	ct_im_ela_cnv_param_2_noise_thresh_low_hi( UCHAR p_low_hi );
static UINT32	ct_im_ela_cnv_param_2_noise_alpha_limit( UCHAR p_alpha_limit );
static UINT32	ct_im_ela_cnv_param_2_noise_limit( UCHAR p_limit );
static UINT32	ct_im_ela_start( UCHAR p_sync );

/* Callback Function	*/
static VOID		ct_im_ela_end_callback(VOID);
#endif

 
#ifdef ELA_TEST
static	SYSTIME sys_time = {0};
#endif
/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
static VOID ct_im_ela_on_pclk( VOID )
{
#ifdef CO_ACT_PCLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gCT_IM_ELA_PCLK_Counter, &Dd_Top_Get_CLKSTOP6(), ~D_DD_TOP_ELAAP_BIT );
#endif	// CO_ACT_PCLOCK
}

/* ELA PCLK change to OFF
 */
static VOID ct_im_ela_off_pclk( VOID )
{
#ifdef CO_ACT_PCLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gCT_IM_ELA_PCLK_Counter, &Dd_Top_Get_CLKSTOP6(), D_DD_TOP_ELAAP_BIT );
#endif	// CO_ACT_PCLOCK
}

static VOID ct_im_ela_ctrl_set_test_data(T_IM_ELA_CTRL* const ctrl_ela)
{
	ctrl_ela->next_ctrl_flag = D_IM_ELA_DISABLE;

	ctrl_ela->ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
	ctrl_ela->ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela->ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela->ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela->ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela->ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_3;

	ctrl_ela->ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela->ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_2;
	ctrl_ela->ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;
	ctrl_ela->ctrl_common.bayer_start_pixel_type     = D_IM_ELA_ORG_R;

	ctrl_ela->ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela->ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela->ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela->ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela->ctrl_byr.deknee_enable       = D_IM_ELA_DISABLE;
	ctrl_ela->ctrl_byr.ob_zsf_enable       = D_IM_ELA_DISABLE;

	//OB Correction control
	ctrl_ela->ob_correction.ob_rr = 0x001;
	ctrl_ela->ob_correction.ob_bb = 0x002;
	ctrl_ela->ob_correction.ob_gr = 0x003;
	ctrl_ela->ob_correction.ob_gb = 0x004;

	ctrl_ela->ctrl_eirch.in_bayer_top_addr = S_SDRAM_ELA_ADR_IN_BAYER;
	ctrl_ela->ctrl_eirch.in_bayer_top_offset = 0;
	ctrl_ela->ctrl_eirch.in_bayer_total_hsize = 6048;
	ctrl_ela->ctrl_eirch.in_bayer_hsize = S_ELA_BAYER_12M_WIDTH;
	ctrl_ela->ctrl_eirch.in_bayer_vline = S_ELA_BAYER_12M_LINES;

	ctrl_ela->out_bayer_addr     = S_SDRAM_ELA_ADR_OUT_BAYER;
	ctrl_ela->extract_noise_addr = S_SDRAM_ELA_ADR_EXTRACT_NOISE;
	ctrl_ela->noise_suppress_addr = S_SDRAM_ELA_ADR_NOISE_SUPPRESS;
}

#if 1
static VOID ct_im_ela_multiple_set_test_data( T_IM_ELA_MULTIPLE_TIMES_INFO* multiple_exec_info )
{
	INT32 i;

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info->core[i]->defect_correction_mode		= D_IM_ELA_ELADCMD_WEAK;
		multiple_exec_info->core[i]->zero_point_level			= 0x00C7;
		multiple_exec_info->core[i]->optical_zero_level_shift	= 0;

		multiple_exec_info->core[i]->elflt[E_IM_ELA_ELFL_T1] = 0x00B2;
		multiple_exec_info->core[i]->elflt[E_IM_ELA_ELFL_T2] = 0x0134;
		multiple_exec_info->core[i]->elflt[E_IM_ELA_ELFL_T3] = 0x0202;
		multiple_exec_info->core[i]->elflt[E_IM_ELA_ELFL_T4] = 0x02f2;

		multiple_exec_info->core[i]->anb_r.offset[E_IM_ELA_ANB_1]   = 0x0058;
		multiple_exec_info->core[i]->anb_r.offset[E_IM_ELA_ANB_2]   = 0x0093;
		multiple_exec_info->core[i]->anb_r.offset[E_IM_ELA_ANB_3]   = 0x00c5;
		multiple_exec_info->core[i]->anb_r.offset[E_IM_ELA_ANB_4]   = 0x0103;
		multiple_exec_info->core[i]->anb_r.gain[E_IM_ELA_ANB_1]     = 0x0273;
		multiple_exec_info->core[i]->anb_r.gain[E_IM_ELA_ANB_2]     = 0x0215;
		multiple_exec_info->core[i]->anb_r.gain[E_IM_ELA_ANB_3]     = 0x0149;
		multiple_exec_info->core[i]->anb_r.gain[E_IM_ELA_ANB_4]     = 0x00f5;
		multiple_exec_info->core[i]->anb_r.boundary[E_IM_ELA_ANB_1] = 0x0000;
		multiple_exec_info->core[i]->anb_r.boundary[E_IM_ELA_ANB_2] = 0x0180;
		multiple_exec_info->core[i]->anb_r.boundary[E_IM_ELA_ANB_3] = 0x0300;
		multiple_exec_info->core[i]->anb_r.boundary[E_IM_ELA_ANB_4] = 0x0600;

		multiple_exec_info->core[i]->anb_g.offset[E_IM_ELA_ANB_1]   = 0x0058;
		multiple_exec_info->core[i]->anb_g.offset[E_IM_ELA_ANB_2]   = 0x0093;
		multiple_exec_info->core[i]->anb_g.offset[E_IM_ELA_ANB_3]   = 0x00c5;
		multiple_exec_info->core[i]->anb_g.offset[E_IM_ELA_ANB_4]   = 0x0103;
		multiple_exec_info->core[i]->anb_g.gain[E_IM_ELA_ANB_1]     = 0x0273;
		multiple_exec_info->core[i]->anb_g.gain[E_IM_ELA_ANB_2]     = 0x0215;
		multiple_exec_info->core[i]->anb_g.gain[E_IM_ELA_ANB_3]     = 0x0149;
		multiple_exec_info->core[i]->anb_g.gain[E_IM_ELA_ANB_4]     = 0x00f5;
		multiple_exec_info->core[i]->anb_g.boundary[E_IM_ELA_ANB_1] = 0x0000;
		multiple_exec_info->core[i]->anb_g.boundary[E_IM_ELA_ANB_2] = 0x0180;
		multiple_exec_info->core[i]->anb_g.boundary[E_IM_ELA_ANB_3] = 0x0300;
		multiple_exec_info->core[i]->anb_g.boundary[E_IM_ELA_ANB_4] = 0x0600;

		multiple_exec_info->core[i]->anb_b.offset[E_IM_ELA_ANB_1]   = 0x0058;
		multiple_exec_info->core[i]->anb_b.offset[E_IM_ELA_ANB_2]   = 0x0093;
		multiple_exec_info->core[i]->anb_b.offset[E_IM_ELA_ANB_3]   = 0x00c5;
		multiple_exec_info->core[i]->anb_b.offset[E_IM_ELA_ANB_4]   = 0x0103;
		multiple_exec_info->core[i]->anb_b.gain[E_IM_ELA_ANB_1]     = 0x0273;
		multiple_exec_info->core[i]->anb_b.gain[E_IM_ELA_ANB_2]     = 0x0215;
		multiple_exec_info->core[i]->anb_b.gain[E_IM_ELA_ANB_3]     = 0x0149;
		multiple_exec_info->core[i]->anb_b.gain[E_IM_ELA_ANB_4]     = 0x00f5;
		multiple_exec_info->core[i]->anb_b.boundary[E_IM_ELA_ANB_1] = 0x0000;
		multiple_exec_info->core[i]->anb_b.boundary[E_IM_ELA_ANB_2] = 0x0180;
		multiple_exec_info->core[i]->anb_b.boundary[E_IM_ELA_ANB_3] = 0x0300;
		multiple_exec_info->core[i]->anb_b.boundary[E_IM_ELA_ANB_4] = 0x0600;
                                                                      
		multiple_exec_info->core[i]->lumen_gain_r   = 0x100;
		multiple_exec_info->core[i]->lumen_gain_g   = 0x100;
		multiple_exec_info->core[i]->lumen_gain_b   = 0x100;

		multiple_exec_info->core[i]->lumen_factor_r = 0x4;
		multiple_exec_info->core[i]->lumen_factor_g = 0x8;
		multiple_exec_info->core[i]->lumen_factor_b = 0x4;
		multiple_exec_info->core[i]->lumen_factor_c = 0x8;

		multiple_exec_info->core[i]->lumen_edge_r   = 0x4;
		multiple_exec_info->core[i]->lumen_edge_g   = 0x8;
		multiple_exec_info->core[i]->lumen_edge_b   = 0x4;

		multiple_exec_info->core[i]->edge_adjust_a1 = 0x008a;
		multiple_exec_info->core[i]->edge_adjust_a2 = 0x0083;
		multiple_exec_info->core[i]->edge_adjust_b1 = 0x02fc;
		multiple_exec_info->core[i]->edge_adjust_b2 = 0x0033;

		multiple_exec_info->core[i]->edge_threshold[E_IM_ELA_ALNGE_1] = 0x008e;
		multiple_exec_info->core[i]->edge_threshold[E_IM_ELA_ALNGE_2] = 0x00d5;
		multiple_exec_info->core[i]->edge_threshold[E_IM_ELA_ALNGE_3] = 0x011c;

		multiple_exec_info->core[i]->nr[E_IM_ELA_ALNG_L1E1N] = 0x00b4;
		multiple_exec_info->core[i]->nr[E_IM_ELA_ALNG_L1E2N] = 0x0082;
		multiple_exec_info->core[i]->nr[E_IM_ELA_ALNG_L1E3N] = 0x0000;
		multiple_exec_info->core[i]->nr[E_IM_ELA_ALNG_L2E1N] = 0x00a0;
		multiple_exec_info->core[i]->nr[E_IM_ELA_ALNG_L2E2N] = 0x0069;
		multiple_exec_info->core[i]->nr[E_IM_ELA_ALNG_L2E3N] = 0x0000;
		multiple_exec_info->core[i]->nr[E_IM_ELA_ALNG_L3E1N] = 0x008c;
		multiple_exec_info->core[i]->nr[E_IM_ELA_ALNG_L3E2N] = 0x0050;
		multiple_exec_info->core[i]->nr[E_IM_ELA_ALNG_L3E3N] = 0x0000;
		multiple_exec_info->core[i]->nr[E_IM_ELA_ALNG_L4E1N] = 0x0078;
		multiple_exec_info->core[i]->nr[E_IM_ELA_ALNG_L4E2N] = 0x003c;
		multiple_exec_info->core[i]->nr[E_IM_ELA_ALNG_L4E3N] = 0x0000;

		multiple_exec_info->core[i]->l_noise = 0x00c8;

		multiple_exec_info->core[i]->l_edge[E_IM_ELA_LCOME_1] = 0x008e;
		multiple_exec_info->core[i]->l_edge[E_IM_ELA_LCOME_2] = 0x011c;

		multiple_exec_info->core[i]->l_intense[E_IM_ELA_LCOMEN_1] = 0x0000;
		multiple_exec_info->core[i]->l_intense[E_IM_ELA_LCOMEN_2] = 0x00cd;

		multiple_exec_info->core[i]->limit.noise_plus_limit = 0x0277;
		multiple_exec_info->core[i]->limit.noise_minus_limit = 0x0277;
	}
}
#endif

static VOID ct_im_ela_each_reg_print( VOID )
{
	Ddim_Print(( "---EAch Area---\n" ));
	ct_im_ela_on_pclk();

	// ELA
	Ddim_Print(( "EARCACHE[0] = 0x%02x\n",	IO_ELA.EARCACHE.bit.EARCACHE_0 ));
	Ddim_Print(( "EARCACHE[1] = 0x%02x\n",	IO_ELA.EARCACHE.bit.EARCACHE_1 ));
	Ddim_Print(( "EARCACHE[2] = 0x%02x\n",	IO_ELA.EARCACHE.bit.EARCACHE_2 ));

	Ddim_Print(( "EAWCACHE[0] = 0x%02x\n",	IO_ELA.EAWCACHE.bit.EAWCACHE_0 ));
	Ddim_Print(( "EAWCACHE[1] = 0x%02x\n",	IO_ELA.EAWCACHE.bit.EAWCACHE_1 ));
	Ddim_Print(( "EAWCACHE[2] = 0x%02x\n",	IO_ELA.EAWCACHE.bit.EAWCACHE_2 ));

	Ddim_Print(( "EARPROT[0] = 0x%02x\n",	IO_ELA.EARPROT.bit.EARPROT_0 ));
	Ddim_Print(( "EARPROT[1] = 0x%02x\n",	IO_ELA.EARPROT.bit.EARPROT_1 ));
	Ddim_Print(( "EARPROT[2] = 0x%02x\n",	IO_ELA.EARPROT.bit.EARPROT_2 ));

	Ddim_Print(( "EAWPROT[0] = 0x%02x\n",	IO_ELA.EAWPROT.bit.EAWPROT_0 ));
	Ddim_Print(( "EAWPROT[1] = 0x%02x\n",	IO_ELA.EAWPROT.bit.EAWPROT_1 ));
	Ddim_Print(( "EAWPROT[2] = 0x%02x\n",	IO_ELA.EAWPROT.bit.EAWPROT_2 ));

	Ddim_Print(( "RRESP[0] = 0x%02x\n",		IO_ELA.EARAXSTS.bit.RRESP_0 ));
	Ddim_Print(( "RRESP[1] = 0x%02x\n",		IO_ELA.EARAXSTS.bit.RRESP_1 ));
	Ddim_Print(( "RRESP[2] = 0x%02x\n",		IO_ELA.EARAXSTS.bit.RRESP_2 ));

	Ddim_Print(( "BRESP[0] = 0x%02x\n",		IO_ELA.EAWAXSTS.bit.BRESP_0 ));
	Ddim_Print(( "BRESP[1] = 0x%02x\n",		IO_ELA.EAWAXSTS.bit.BRESP_1 ));
	Ddim_Print(( "BRESP[2] = 0x%02x\n",		IO_ELA.EAWAXSTS.bit.BRESP_2 ));

	Ddim_Print(( "EARREQMSK[0] = 0x%02x\n",	IO_ELA.EARREQMSK.EARREQMSK1.bit.EARREQMSK_0 ));
	Ddim_Print(( "EARREQMSK[1] = 0x%02x\n",	IO_ELA.EARREQMSK.EARREQMSK1.bit.EARREQMSK_1 ));
	Ddim_Print(( "EARREQMSK[2] = 0x%02x\n",	IO_ELA.EARREQMSK.EARREQMSK2.bit.EARREQMSK_2 ));

	Ddim_Print(( "EAWREQMSK[0] = 0x%02x\n",	IO_ELA.EAWREQMSK.EAWREQMSK1.bit.EAWREQMSK_0 ));
	Ddim_Print(( "EAWREQMSK[1] = 0x%02x\n",	IO_ELA.EAWREQMSK.EAWREQMSK1.bit.EAWREQMSK_1 ));
	Ddim_Print(( "EAWREQMSK[2] = 0x%02x\n",	IO_ELA.EAWREQMSK.EAWREQMSK2.bit.EAWREQMSK_2 ));

	ct_im_ela_off_pclk();
//	DDIM_User_Dly_Tsk( 10 );
}

static VOID ct_im_ela_ctrl_reg_print( VOID )
{
	UINT32 i;

	Ddim_Print(( "---ELA Area---\n" ));
	ct_im_ela_on_pclk();

	// ELA
	Ddim_Print(( "ELATRG.ELATRG = 0x%02x\n",	IO_ELA.ELATRG.bit.ELATRG ));

	Ddim_Print(( "ELACTL.ELACNT = 0x%02x\n",	IO_ELA.ELACTL.bit.ELACNT ));
	Ddim_Print(( "ELACTL.ELAMD = 0x%02x\n",		IO_ELA.ELACTL.bit.ELAMD ));
	Ddim_Print(( "ELACTL.ELAACT = 0x%02x\n",	IO_ELA.ELACTL.bit.ELAACT ));

	Ddim_Print(( "ELABYR.EIRTYP = 0x%02x\n",	IO_ELA.ELABYR.bit.EIRTYP ));
	Ddim_Print(( "ELABYR.EIRSFT = 0x%02x\n",	IO_ELA.ELABYR.bit.EIRSFT ));
	Ddim_Print(( "ELABYR.EIWSFT = 0x%02x\n",	IO_ELA.ELABYR.bit.EIWSFT ));
	Ddim_Print(( "ELABYR.EIWTYP = 0x%02x\n",	IO_ELA.ELABYR.bit.EIWTYP ));
	Ddim_Print(( "ELABYR.EIWOUT = 0x%02x\n",	IO_ELA.ELABYR.bit.EIWOUT ));
	Ddim_Print(( "ELABYR.DKNEN = 0x%02x\n",		IO_ELA.ELABYR.bit.DKNEN ));
	Ddim_Print(( "ELABYR.EOBAEN = 0x%02x\n",	IO_ELA.ELABYR.bit.EOBAEN ));
	Ddim_Print(( "ELABYR.DKNACT = 0x%02x\n",	IO_ELA.ELABYR.bit.DKNACT ));

	Ddim_Print(( "NSLMD.NSLMD = 0x%02x\n",		IO_ELA.NSLMD.bit.NSLMD ));
	Ddim_Print(( "NSLMD.NSLRES = 0x%02x\n",		IO_ELA.NSLMD.bit.NSLRES ));
	Ddim_Print(( "NSLMD.NSLKNE = 0x%02x\n",		IO_ELA.NSLMD.bit.NSLKNE ));

	Ddim_Print(( "OBOF.EOBRR = 0x%04x\n",		IO_ELA.OBOF.OBOF1.bit.EOBRR ));
	Ddim_Print(( "OBOF.EOBBB = 0x%04x\n",		IO_ELA.OBOF.OBOF1.bit.EOBBB ));
	Ddim_Print(( "OBOF.EOBGR = 0x%04x\n",		IO_ELA.OBOF.OBOF2.bit.EOBGR ));
	Ddim_Print(( "OBOF.EOBGB = 0x%04x\n",		IO_ELA.OBOF.OBOF2.bit.EOBGB ));

	Ddim_Print(( "EZSFTA1 = 0x%04x\n",			IO_ELA.EZSFTA1.bit.EZSFTA1 ));

	Ddim_Print(( "NSLNGLMM = 0x%04x\n",			IO_ELA.NSLNGLMT.bit.NSLNGLMM ));
	Ddim_Print(( "NSLNGLMP = 0x%04x\n",			IO_ELA.NSLNGLMT.bit.NSLNGLMP ));

	Ddim_Print(( "EIRA = 0x%08lx\n",			IO_ELA.EIRA.word ));
	Ddim_Print(( "EIROFS = 0x%02x\n",			IO_ELA.EIROFS.bit.EIROFS ));
	Ddim_Print(( "EIRDEF = 0x%04x\n",			IO_ELA.EIRDEF.bit.EIRDEF ));
	Ddim_Print(( "EIRHSIZ = 0x%04x\n",			IO_ELA.EIRHSIZ.bit.EIRHSIZ ));
	Ddim_Print(( "EIRVSIZ = 0x%04x\n",			IO_ELA.EIRVSIZ.bit.EIRVSIZ ));

	for( i = 0; i < 4; i++ ){
		Ddim_Print(( "EIWA[%d] = 0x%08lx\n",	i, IO_ELA.EIWA.word[i] ));
	}
	Ddim_Print(( "EIWDEF = 0x%04x\n",			IO_ELA.EIWDEF.bit.EIWDEF ));

	Ddim_Print(( "ENWA = 0x%08lx\n",			IO_ELA.ENWA.word ));
	Ddim_Print(( "ENWDEF = 0x%04x\n",			IO_ELA.ENWDEF.bit.ENWDEF ));

	for( i = 0; i < 16; i++ ){
		Ddim_Print(( "ENRA[%d] = 0x%08lx\n",	i, IO_ELA.ENRA.word[i] ));
	}
	Ddim_Print(( "ENRHSIZ = 0x%04x\n",			IO_ELA.ENRHSIZ.bit.ENRHSIZ ));
	Ddim_Print(( "ENRVSIZ = 0x%04x\n",			IO_ELA.ENRVSIZ.bit.ENRVSIZ ));

	Ddim_Print(( "ENSA = 0x%08lx\n",			IO_ELA.ENSA.word ));
	Ddim_Print(( "ENSDEF = 0x%04x\n",			IO_ELA.ENSDEF.bit.ENSDEF ));
	Ddim_Print(( "ENSHSIZ = 0x%04x\n",			IO_ELA.ENSHSIZ.bit.ENSHSIZ ));
	Ddim_Print(( "ENSVSIZ = 0x%04x\n",			IO_ELA.ENSVSIZ.bit.ENSVSIZ ));

	ct_im_ela_off_pclk();
//	DDIM_User_Dly_Tsk( 10 );
}

static VOID ct_im_ela_core_reg_print( VOID )
{
	Ddim_Print(( "---CORE Area---\n" ));
	ct_im_ela_on_pclk();

	// ELA
	Ddim_Print(( "BYRTYP.ORG = 0x%02x\n",	IO_ELA.BYRTYP.bit.ORG ));
	Ddim_Print(( "BYRTYP.ELADCMD = 0x%02x\n",	IO_ELA.BYRTYP.bit.ELADCMD ));

	Ddim_Print(( "ELFZP = 0x%04x\n",		IO_ELA.ELFZP.bit.ELFZP ));

	Ddim_Print(( "ELFL_1 = 0x%04x\n",		IO_ELA.ELFL.ELFL1.bit.ELFL_1 ));
	Ddim_Print(( "ELFL_2 = 0x%04x\n",		IO_ELA.ELFL.ELFL1.bit.ELFL_2 ));
	Ddim_Print(( "ELFL_3 = 0x%04x\n",		IO_ELA.ELFL.ELFL2.bit.ELFL_3 ));
	Ddim_Print(( "ELFL_4 = 0x%04x\n",		IO_ELA.ELFL.ELFL2.bit.ELFL_4 ));

	Ddim_Print(( "ANBOR[0] = 0x%04x\n",		IO_ELA.ANBOR.ANBOR1.bit.ANBOR_0 ));
	Ddim_Print(( "ANBOR[1] = 0x%04x\n",		IO_ELA.ANBOR.ANBOR1.bit.ANBOR_1 ));
	Ddim_Print(( "ANBOR[2] = 0x%04x\n",		IO_ELA.ANBOR.ANBOR2.bit.ANBOR_2 ));
	Ddim_Print(( "ANBOR[3] = 0x%04x\n",		IO_ELA.ANBOR.ANBOR2.bit.ANBOR_3 ));

	Ddim_Print(( "ANBKR[0] = 0x%04x\n",		IO_ELA.ANBKR.ANBKR1.bit.ANBKR_0 ));
	Ddim_Print(( "ANBKR[1] = 0x%04x\n",		IO_ELA.ANBKR.ANBKR1.bit.ANBKR_1 ));
	Ddim_Print(( "ANBKR[2] = 0x%04x\n",		IO_ELA.ANBKR.ANBKR2.bit.ANBKR_2 ));
	Ddim_Print(( "ANBKR[3] = 0x%04x\n",		IO_ELA.ANBKR.ANBKR2.bit.ANBKR_3 ));

	Ddim_Print(( "ANBDR[0] = 0x%04x\n",		IO_ELA.ANBDR.ANBDR1.bit.ANBDR_0 ));
	Ddim_Print(( "ANBDR[1] = 0x%04x\n",		IO_ELA.ANBDR.ANBDR1.bit.ANBDR_1 ));
	Ddim_Print(( "ANBDR[2] = 0x%04x\n",		IO_ELA.ANBDR.ANBDR2.bit.ANBDR_2 ));
	Ddim_Print(( "ANBDR[3] = 0x%04x\n",		IO_ELA.ANBDR.ANBDR2.bit.ANBDR_3 ));

	Ddim_Print(( "ANBOG[0] = 0x%04x\n",		IO_ELA.ANBOG.ANBOG1.bit.ANBOG_0 ));
	Ddim_Print(( "ANBOG[1] = 0x%04x\n",		IO_ELA.ANBOG.ANBOG1.bit.ANBOG_1 ));
	Ddim_Print(( "ANBOG[2] = 0x%04x\n",		IO_ELA.ANBOG.ANBOG2.bit.ANBOG_2 ));
	Ddim_Print(( "ANBOG[3] = 0x%04x\n",		IO_ELA.ANBOG.ANBOG2.bit.ANBOG_3 ));

	Ddim_Print(( "ANBKG[0] = 0x%04x\n",		IO_ELA.ANBKG.ANBKG1.bit.ANBKG_0 ));
	Ddim_Print(( "ANBKG[1] = 0x%04x\n",		IO_ELA.ANBKG.ANBKG1.bit.ANBKG_1 ));
	Ddim_Print(( "ANBKG[2] = 0x%04x\n",		IO_ELA.ANBKG.ANBKG2.bit.ANBKG_2 ));
	Ddim_Print(( "ANBKG[3] = 0x%04x\n",		IO_ELA.ANBKG.ANBKG2.bit.ANBKG_3 ));

	Ddim_Print(( "ANBDG[0] = 0x%04x\n",		IO_ELA.ANBDG.ANBDG1.bit.ANBDG_0 ));
	Ddim_Print(( "ANBDG[1] = 0x%04x\n",		IO_ELA.ANBDG.ANBDG1.bit.ANBDG_1 ));
	Ddim_Print(( "ANBDG[2] = 0x%04x\n",		IO_ELA.ANBDG.ANBDG2.bit.ANBDG_2 ));
	Ddim_Print(( "ANBDG[3] = 0x%04x\n",		IO_ELA.ANBDG.ANBDG2.bit.ANBDG_3 ));

	Ddim_Print(( "ANBOB[0] = 0x%04x\n",		IO_ELA.ANBOB.ANBOB1.bit.ANBOB_0 ));
	Ddim_Print(( "ANBOB[1] = 0x%04x\n",		IO_ELA.ANBOB.ANBOB1.bit.ANBOB_1 ));
	Ddim_Print(( "ANBOB[2] = 0x%04x\n",		IO_ELA.ANBOB.ANBOB2.bit.ANBOB_2 ));
	Ddim_Print(( "ANBOB[3] = 0x%04x\n",		IO_ELA.ANBOB.ANBOB2.bit.ANBOB_3 ));
	
	Ddim_Print(( "ANBKB[0] = 0x%04x\n",		IO_ELA.ANBKB.ANBKB1.bit.ANBKB_0 ));
	Ddim_Print(( "ANBKB[1] = 0x%04x\n",		IO_ELA.ANBKB.ANBKB1.bit.ANBKB_1 ));
	Ddim_Print(( "ANBKB[2] = 0x%04x\n",		IO_ELA.ANBKB.ANBKB2.bit.ANBKB_2 ));
	Ddim_Print(( "ANBKB[3] = 0x%04x\n",		IO_ELA.ANBKB.ANBKB2.bit.ANBKB_3 ));

	Ddim_Print(( "ANBDB[0] = 0x%04x\n",		IO_ELA.ANBDB.ANBDB1.bit.ANBDB_0 ));
	Ddim_Print(( "ANBDB[1] = 0x%04x\n",		IO_ELA.ANBDB.ANBDB1.bit.ANBDB_1 ));
	Ddim_Print(( "ANBDB[2] = 0x%04x\n",		IO_ELA.ANBDB.ANBDB2.bit.ANBDB_2 ));
	Ddim_Print(( "ANBDB[3] = 0x%04x\n",		IO_ELA.ANBDB.ANBDB2.bit.ANBDB_3 ));

	Ddim_Print(( "LGTWR = 0x%04x\n",		IO_ELA.LGTW.LGTW1.bit.LGTWR ));
	Ddim_Print(( "LGTWG = 0x%04x\n",		IO_ELA.LGTW.LGTW1.bit.LGTWG ));
	Ddim_Print(( "LGTWB = 0x%04x\n",		IO_ELA.LGTW.LGTW2.bit.LGTWB ));

	Ddim_Print(( "LGTR = 0x%02x\n",			IO_ELA.LGT.bit.LGTR ));
	Ddim_Print(( "LGTG = 0x%02x\n",			IO_ELA.LGT.bit.LGTG ));
	Ddim_Print(( "LGTB = 0x%02x\n",			IO_ELA.LGT.bit.LGTB ));
	Ddim_Print(( "LGTC = 0x%02x\n",			IO_ELA.LGT.bit.LGTC ));

	Ddim_Print(( "LGTER = 0x%02x\n",		IO_ELA.LGTE.bit.LGTER ));
	Ddim_Print(( "LGTEG = 0x%02x\n",		IO_ELA.LGTE.bit.LGTEG ));
	Ddim_Print(( "LGTEB = 0x%02x\n",		IO_ELA.LGTE.bit.LGTEB ));

	Ddim_Print(( "EDG2ANBA = 0x%04x\n",		IO_ELA.EDG2ANBA.bit.EDG2ANBA ));
	Ddim_Print(( "EDGSCLA = 0x%04x\n",		IO_ELA.EDGSCLA.bit.EDGSCLA ));

	Ddim_Print(( "EDG2ANBB = 0x%04x\n",		IO_ELA.EDG2ANBB.bit.EDG2ANBB ));
	Ddim_Print(( "EDGSCLB = 0x%04x\n",		IO_ELA.EDGSCLB.bit.EDGSCLB ));

	Ddim_Print(( "ALNGE1 = 0x%04x\n",		IO_ELA.ALNGE.ALNGE1.bit.ALNGE_1 ));
	Ddim_Print(( "ALNGE2 = 0x%04x\n",		IO_ELA.ALNGE.ALNGE1.bit.ALNGE_2 ));
	Ddim_Print(( "ALNGE3 = 0x%04x\n",		IO_ELA.ALNGE.ALNGE2.bit.ALNGE_3 ));

	Ddim_Print(( "ALNGL1E1N = 0x%04x\n",	IO_ELA.ALNG.ALNG1.bit.ALNGL1E1N ));
	Ddim_Print(( "ALNGL1E2N = 0x%04x\n",	IO_ELA.ALNG.ALNG1.bit.ALNGL1E2N ));
	Ddim_Print(( "ALNGL1E3N = 0x%04x\n",	IO_ELA.ALNG.ALNG2.bit.ALNGL1E3N ));
	Ddim_Print(( "ALNGL2E1N = 0x%04x\n",	IO_ELA.ALNG.ALNG2.bit.ALNGL2E1N ));
	Ddim_Print(( "ALNGL2E2N = 0x%04x\n",	IO_ELA.ALNG.ALNG3.bit.ALNGL2E2N ));
	Ddim_Print(( "ALNGL2E3N = 0x%04x\n",	IO_ELA.ALNG.ALNG3.bit.ALNGL2E3N ));
	Ddim_Print(( "ALNGL3E1N = 0x%04x\n",	IO_ELA.ALNG.ALNG4.bit.ALNGL3E1N ));
	Ddim_Print(( "ALNGL3E2N = 0x%04x\n",	IO_ELA.ALNG.ALNG4.bit.ALNGL3E2N ));
	Ddim_Print(( "ALNGL3E3N = 0x%04x\n",	IO_ELA.ALNG.ALNG5.bit.ALNGL3E3N ));
	Ddim_Print(( "ALNGL4E1N = 0x%04x\n",	IO_ELA.ALNG.ALNG5.bit.ALNGL4E1N ));
	Ddim_Print(( "ALNGL4E2N = 0x%04x\n",	IO_ELA.ALNG.ALNG6.bit.ALNGL4E2N ));
	Ddim_Print(( "ALNGL4E3N = 0x%04x\n",	IO_ELA.ALNG.ALNG6.bit.ALNGL4E3N ));

	Ddim_Print(( "LFTH = 0x%04x\n",			IO_ELA.LFTH.bit.LFTH ));

	Ddim_Print(( "LCOME1 = 0x%04x\n",		IO_ELA.LCOME.bit.LCOME1 ));
	Ddim_Print(( "LCOME2 = 0x%04x\n",		IO_ELA.LCOME.bit.LCOME2 ));

	Ddim_Print(( "LCOME1N = 0x%04x\n",		IO_ELA.LCOMEN.bit.LCOME1N ));
	Ddim_Print(( "LCOME2N = 0x%04x\n",		IO_ELA.LCOMEN.bit.LCOME2N ));

	ct_im_ela_off_pclk();
//	DDIM_User_Dly_Tsk( 10 );
}

static VOID ct_im_ela_interrupt_reg_print( VOID )
{
	Ddim_Print(( "---Interrupt Register---\n" ));

	ct_im_ela_on_pclk();

	// ELA
	Ddim_Print(( "ENWE = 0x%02x\n",   IO_ELA.ELAINTE.bit.ENWE ));
	Ddim_Print(( "AXRERE = 0x%02x\n", IO_ELA.ELAINTE.bit.AXRERE ));
	Ddim_Print(( "AXWERE = 0x%02x\n", IO_ELA.ELAINTE.bit.AXWERE ));
	Ddim_Print(( "ENWF = 0x%02x\n",   IO_ELA.ELAINTF.bit.__ENWF));
	Ddim_Print(( "AXRERF = 0x%02x\n", IO_ELA.ELAINTF.bit.__AXRERF ));
	Ddim_Print(( "AXWERF = 0x%02x\n", IO_ELA.ELAINTF.bit.__AXWERF ));

	ct_im_ela_off_pclk();
//	DDIM_User_Dly_Tsk(10);
}

static VOID ct_im_ela_curtail_bayer_tbl_print( VOID )
{
	UINT32 i, j;
	T_IM_ELA_CURTAIL_BAYER_DATA curtail_bayer[D_IM_ELA_STAGE_MAX];

	Ddim_Print(( "---Curtail Bayer Data Tbl---\n" ));

	Im_ELA_Get_Curtail_Bayer_Data_Tbl( curtail_bayer );
	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		Ddim_Print(( "(((Stage%d)))\n", i + 1 ));
		Ddim_Print(( "gl_hbyte = %d\n",	curtail_bayer[i].gl_hbyte ));
		Ddim_Print(( "hsize = %d\n",	curtail_bayer[i].hsize ));
		Ddim_Print(( "vline = %d\n",	curtail_bayer[i].vline ));
		for( j = 0; j < D_IM_ELA_STEP_MAX; j++ ){
			Ddim_Print(( "(Sterp%02d)bayer_addr = 0x%08X\n", j, curtail_bayer[i].bayer_addr[j] ));
//			DDIM_User_Dly_Tsk( 10 );
		}
	}
}

static VOID ct_im_ela_extract_noise_tbl_print( VOID )
{
	UINT32 i, j;
	T_IM_ELA_EXTRACT_NOISE_DATA extract_noise[D_IM_ELA_STAGE_MAX];

	Ddim_Print(( "---Extract Noise Data Tbl---\n" ));

	Im_ELA_Get_Extract_Noise_Data_Tbl( extract_noise );
	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		Ddim_Print(( "(((Stage%d)))\n", i + 1 ));
		Ddim_Print(( "gl_hbyte = %d\n",	extract_noise[i].gl_hbyte ));
		Ddim_Print(( "hsize = %d\n",	extract_noise[i].hsize ));
		Ddim_Print(( "vline = %d\n",	extract_noise[i].vline ));
		for( j = 0; j < D_IM_ELA_STEP_MAX; j++ ){
			Ddim_Print(( "(Sterp%02d)noise_data_addr = 0x%08X\n", j, extract_noise[i].noise_data_addr[j] ));
//			DDIM_User_Dly_Tsk( 10 );
		}
	}
}


static VOID ct_im_ela_clk_chk( VOID )
{
#ifdef CO_ACT_ICLOCK
	if( gIM_ELA_CLK_Counter != 0 ){
		Ddim_Print(("E:Pclk NG : gIM_ELA_CLK_Counter=%d.\n", gIM_ELA_CLK_Counter));
	}
	if( gIM_ELA_ICLK_Counter != 0 ){
		Ddim_Print(("E:Pclk NG : gIM_ELA_ICLK_Counter=%d.\n", gIM_ELA_ICLK_Counter));
	}
#endif //CO_ACT_ICLOCK
#ifdef CO_ACT_MXIC_PCLOCK
	if( gIM_SDI_PCLK_Counter != 0 ){
		Ddim_Print(("E:Pclk NG : gIM_SDI_PCLK_Counter=%d.\n", gIM_SDI_PCLK_Counter));
	}
#endif // CO_ACT_MXIC_PCLOCK
//	DDIM_User_Dly_Tsk( 10 );
}

VOID CT_Im_Ela_Reg_Print( VOID )
{
	ct_im_ela_each_reg_print();
	ct_im_ela_ctrl_reg_print();
	ct_im_ela_core_reg_print();
	ct_im_ela_interrupt_reg_print();
	ct_im_ela_curtail_bayer_tbl_print();
	ct_im_ela_extract_noise_tbl_print();
}

VOID CT_Im_Ela_1_01( VOID )
{
	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	ct_im_ela_on_pclk();
	Ddim_Print(( "ENWE = 0x%02x\n",   IO_ELA.ELAINTE.bit.ENWE ));
	Ddim_Print(( "AXRERE = 0x%02x\n", IO_ELA.ELAINTE.bit.AXRERE ));
	Ddim_Print(( "AXWERE = 0x%02x\n", IO_ELA.ELAINTE.bit.AXWERE ));
	Ddim_Print(( "ENWF = 0x%02x\n",   IO_ELA.ELAINTF.bit.__ENWF));
	Ddim_Print(( "AXRERF = 0x%02x\n", IO_ELA.ELAINTF.bit.__AXRERF ));
	Ddim_Print(( "AXWERF = 0x%02x\n", IO_ELA.ELAINTF.bit.__AXWERF ));
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();

	Im_ELA_Close();

	DDIM_User_Dly_Tsk( 10 );
}

VOID CT_Im_Ela_1_02( VOID )
{
	UCHAR result;
	Ddim_Print(( "<%s>\n", __FUNCTION__ ));
	
#ifdef CT_IM_ELA_SET_TEST_DATA
	ct_im_ela_on_pclk();
	IO_ELA.ELACTL.bit.ELACNT = 3;
	ct_im_ela_off_pclk();
#endif

	Im_ELA_Open( 100 );

	result = Im_ELA_Get_Exe_Cnt();
	Ddim_Print(( "Im_ELA_Get_Exe_Cnt = %d\n", result ));

	ct_im_ela_on_pclk();
	Ddim_Print(( "ELACNT = 0x%02x\n", IO_ELA.ELACTL.bit.ELACNT ));
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();

	Im_ELA_Close();
}

VOID CT_Im_Ela_1_03( VOID )
{
	UINT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

	ctrl_ela.ctrl_byr.deknee_enable = D_IM_ELA_ENABLE;
	ctrl_ela.ctrl_byr.ob_zsf_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x03FF;
	ctrl_ela.ob_correction.ob_bb = 0x03FF;
	ctrl_ela.ob_correction.ob_gr = 0x03FF;
	ctrl_ela.ob_correction.ob_gb = 0x03FF;

	Im_ELA_Open( 100 );

	result = Im_ELA_Ctrl( &ctrl_ela );
	Ddim_Print(( "Im_ELA_Ctrl = %d\n", result ));

	ct_im_ela_ctrl_reg_print();
	ct_im_ela_curtail_bayer_tbl_print();
	ct_im_ela_extract_noise_tbl_print();
	ct_im_ela_core_reg_print();

	ct_im_ela_clk_chk();

	Im_ELA_Close();
}

VOID CT_Im_Ela_1_04( VOID )
{
	UINT32 result;
	T_IM_ELA_CTRL_COMMON ctrl_ela_common;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	result = Im_ELA_Get_Ctrl_Common( &ctrl_ela_common );
	Ddim_Print(("result=0x%08X\n", result));

	ct_im_ela_on_pclk();
	if( ctrl_ela_common.noise_data_mode != IO_ELA.NSLMD.bit.NSLMD ){
		Ddim_Print(("TEST NG : (NSLMD) %d != %d\n", ctrl_ela_common.noise_data_mode, IO_ELA.NSLMD.bit.NSLMD));
	}
	else if( ctrl_ela_common.noise_data_precision != IO_ELA.NSLMD.bit.NSLRES ){
		Ddim_Print(("TEST NG : (NSLRES) %d != %d\n", ctrl_ela_common.noise_data_precision, IO_ELA.NSLMD.bit.NSLRES));
	}
	else if( ctrl_ela_common.noise_data_compress_enable != IO_ELA.NSLMD.bit.NSLKNE ){
		Ddim_Print(("TEST NG : (NSLKNE) %d != %d\n", ctrl_ela_common.noise_data_compress_enable, IO_ELA.NSLMD.bit.NSLKNE));
	}
	else if( ctrl_ela_common.bayer_start_pixel_type != IO_ELA.BYRTYP.bit.ORG ){
		Ddim_Print(("TEST NG : (ORG) %d != %d\n", ctrl_ela_common.bayer_start_pixel_type, IO_ELA.BYRTYP.bit.ORG));
	}
	else{
		Ddim_Print(("TEST OK!\n"));
	}
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();

	Im_ELA_Close();
}

VOID CT_Im_Ela_1_05( VOID )
{
	UINT32 result;
	T_IM_ELA_CTRL_EIRCH ctrl_ela_eirch;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	result = Im_ELA_Get_Ctrl_Eirch( &ctrl_ela_eirch );
	Ddim_Print(("result=0x%08X\n", result));

	ct_im_ela_on_pclk();
	if( ctrl_ela_eirch.in_bayer_top_addr != IO_ELA.EIRA.bit.EIRA ){
		Ddim_Print(("TEST NG : (EIRA) 0x%lx != 0x%lx\n", ctrl_ela_eirch.in_bayer_top_addr, IO_ELA.EIRA.bit.EIRA));
	}
	else if( ctrl_ela_eirch.in_bayer_top_offset != IO_ELA.EIROFS.bit.EIROFS ){
		Ddim_Print(("TEST NG : (EIROFS) %d != %d\n", ctrl_ela_eirch.in_bayer_top_offset, IO_ELA.EIROFS.bit.EIROFS));
	}
	else if( ctrl_ela_eirch.in_bayer_total_hsize != IO_ELA.EIRDEF.bit.EIRDEF ){
		Ddim_Print(("TEST NG : (EIRDEF) %d != %d\n", ctrl_ela_eirch.in_bayer_total_hsize, IO_ELA.EIRDEF.bit.EIRDEF));
	}
	else if( ctrl_ela_eirch.in_bayer_hsize != IO_ELA.EIRHSIZ.bit.EIRHSIZ ){
		Ddim_Print(("TEST NG : (EIRHSIZ) %d != %d\n", ctrl_ela_eirch.in_bayer_hsize, IO_ELA.EIRHSIZ.bit.EIRHSIZ));
	}
	else if( ctrl_ela_eirch.in_bayer_vline != IO_ELA.EIRVSIZ.bit.EIRVSIZ ){
		Ddim_Print(("TEST NG : (EIRVSIZ) %d != %d\n", ctrl_ela_eirch.in_bayer_vline, IO_ELA.EIRVSIZ.bit.EIRVSIZ));
	}
	else{
		Ddim_Print(("TEST OK!\n"));
	}
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();

	Im_ELA_Close();
}

VOID CT_Im_Ela_1_06( VOID )
{
	UINT32 result;
	UINT32 err_f = 0;
	UINT32 i;
	T_IM_ELA_CTRL_EIWCH ctrl_ela_eiwch;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	result = Im_ELA_Get_Ctrl_Eiwch( &ctrl_ela_eiwch );
	Ddim_Print(("result=0x%08X\n", result));

	ct_im_ela_on_pclk();
	for( i = 0; i < D_IM_ELA_EIWCH_ADDR_MAX; i++ ){
		if( ctrl_ela_eiwch.out_bayer_top_addr[i] != IO_ELA.EIWA.word[i] ){
			Ddim_Print(("TEST NG : (EIWA[%d]) 0x%x != 0x%lx\n" ,i ,ctrl_ela_eiwch.out_bayer_top_addr[i], IO_ELA.EIWA.word[i]));
			err_f = 1;
		}
	}
	if( err_f == 0 ){
		if( ctrl_ela_eiwch.out_bayer_total_hsize != IO_ELA.EIWDEF.bit.EIWDEF ){
			Ddim_Print(("TEST NG : (EIRVSIZ) %d != %d\n", ctrl_ela_eiwch.out_bayer_total_hsize, IO_ELA.EIWDEF.bit.EIWDEF));
		}
		else{
			Ddim_Print(("TEST OK!\n"));
		}
	}
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();

	Im_ELA_Close();
}

VOID CT_Im_Ela_1_07( VOID )
{
	UINT32 result;
	T_IM_ELA_CTRL_ENWCH ctrl_ela_enwch;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	result = Im_ELA_Get_Ctrl_Enwch( &ctrl_ela_enwch );
	Ddim_Print(("result=0x%08X\n", result));

	ct_im_ela_on_pclk();
	if( ctrl_ela_enwch.out_extract_noise_top_addr != IO_ELA.ENWA.bit.ENWA ){
		Ddim_Print(("TEST NG : (EIRA) 0x%x != 0x%lx\n", ctrl_ela_enwch.out_extract_noise_top_addr, IO_ELA.ENWA.bit.ENWA));
	}
	else if( ctrl_ela_enwch.out_extract_noise_total_hsize != IO_ELA.ENWDEF.bit.ENWDEF ){
		Ddim_Print(("TEST NG : (ENWDEF) %d != %d\n", ctrl_ela_enwch.out_extract_noise_total_hsize, IO_ELA.ENWDEF.bit.ENWDEF));
	}
	else{
		Ddim_Print(("TEST OK!\n"));
	}
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();

	Im_ELA_Close();
}

VOID CT_Im_Ela_1_08( VOID )
{
	UINT32 result;
	UINT32 err_f = 0;
	UINT32 i;
	T_IM_ELA_CTRL_ENRECH ctrl_ela_enrech;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	result = Im_ELA_Get_Ctrl_Enrech( &ctrl_ela_enrech );
	Ddim_Print(("result=0x%08X\n", result));

	ct_im_ela_on_pclk();
	for( i = 0; i < D_IM_ELA_EIWCH_ADDR_MAX; i++ ){
		if( ctrl_ela_enrech.in_extract_noise_top_addr[i] != IO_ELA.ENRA.word[i] ){
			Ddim_Print(("TEST NG : (ENRA[%d]) 0x%x != 0x%lx\n" ,i ,ctrl_ela_enrech.in_extract_noise_top_addr[i], IO_ELA.ENRA.word[i]));
			err_f = 1;
		}
	}
	if( err_f == 0 ){
		if( ctrl_ela_enrech.in_extract_noise_hsize != IO_ELA.ENRHSIZ.bit.ENRHSIZ ){
			Ddim_Print(("TEST NG : (ENRHSIZ) %d != %d\n", ctrl_ela_enrech.in_extract_noise_hsize, IO_ELA.ENRHSIZ.bit.ENRHSIZ));
		}
		else if( ctrl_ela_enrech.in_extract_noise_vline != IO_ELA.ENRVSIZ.bit.ENRVSIZ ){
			Ddim_Print(("TEST NG : (ENRVSIZ) %d != %d\n", ctrl_ela_enrech.in_extract_noise_vline, IO_ELA.ENRVSIZ.bit.ENRVSIZ));
		}
		else{
			Ddim_Print(("TEST OK!\n"));
		}
	}
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();

	Im_ELA_Close();
}

VOID CT_Im_Ela_1_09( VOID )
{
	UINT32 result;
	T_IM_ELA_CTRL_ENRACH_ENWMCH ctrl_ela_enrach_enwmch;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	result = Im_ELA_Get_Ctrl_Enrach_Enwmch( &ctrl_ela_enrach_enwmch );
	Ddim_Print(("result=0x%08X\n", result));

	ct_im_ela_on_pclk();
	if( ctrl_ela_enrach_enwmch.synthetic_noise_top_addr != IO_ELA.ENSA.bit.ENSA ){
		Ddim_Print(("TEST NG : (ENSA) 0x%x != 0x%lx\n", ctrl_ela_enrach_enwmch.synthetic_noise_top_addr, IO_ELA.ENSA.bit.ENSA));
	}
	else if( ctrl_ela_enrach_enwmch.synthetic_noise_total_hsize != IO_ELA.ENSDEF.bit.ENSDEF ){
		Ddim_Print(("TEST NG : (ENWDEF) %d != %d\n", ctrl_ela_enrach_enwmch.synthetic_noise_total_hsize, IO_ELA.ENWDEF.bit.ENWDEF));
	}
	else if( ctrl_ela_enrach_enwmch.synthetic_noise_hsize != IO_ELA.ENSHSIZ.bit.ENSHSIZ ){
		Ddim_Print(("TEST NG : (ENSHSIZ) %d != %d\n", ctrl_ela_enrach_enwmch.synthetic_noise_hsize, IO_ELA.ENSHSIZ.bit.ENSHSIZ));
	}
	else if( ctrl_ela_enrach_enwmch.synthetic_noise_vline != IO_ELA.ENSVSIZ.bit.ENSVSIZ ){
		Ddim_Print(("TEST NG : (ENSVSIZ) %d != %d\n", ctrl_ela_enrach_enwmch.synthetic_noise_vline, IO_ELA.ENSVSIZ.bit.ENSVSIZ));
	}
	else{
		Ddim_Print(("TEST OK!\n"));
	}
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();

	Im_ELA_Close();
}

VOID CT_Im_Ela_1_10( VOID )
{
	UINT32 result;
	UINT32 err_f = 0;
	T_IM_ELA_CTRL_CORE ctrl_ela_core;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	result = Im_ELA_Get_Ctrl_Core( &ctrl_ela_core );
	Ddim_Print(("result=0x%08X\n", result));

	ct_im_ela_on_pclk();
	if( ctrl_ela_core.defect_correction_mode != IO_ELA.BYRTYP.bit.ELADCMD ){
		Ddim_Print(("TEST NG : (ELADCMD) %d != %d\n", ctrl_ela_core.defect_correction_mode, IO_ELA.BYRTYP.bit.ELADCMD));
		err_f = 1;
	}
	if( ctrl_ela_core.zero_point_level != IO_ELA.ELFZP.bit.ELFZP ){
		Ddim_Print(("TEST NG : (ELFZP) %d != %d\n", ctrl_ela_core.zero_point_level, IO_ELA.ELFZP.bit.ELFZP));
		err_f = 1;
	}
	if( ctrl_ela_core.elflt[0] != IO_ELA.ELFL.ELFL1.bit.ELFL_1 ){
		Ddim_Print(("TEST NG : (ELFL_1) %d != %d\n", ctrl_ela_core.elflt[0], IO_ELA.ELFL.ELFL1.bit.ELFL_1));
		err_f = 1;
	}
	if( ctrl_ela_core.elflt[1] != IO_ELA.ELFL.ELFL1.bit.ELFL_2 ){
		Ddim_Print(("TEST NG : (ELFL_2) %d != %d\n", ctrl_ela_core.elflt[1], IO_ELA.ELFL.ELFL1.bit.ELFL_2));
		err_f = 1;
	}
	if( ctrl_ela_core.elflt[2] != IO_ELA.ELFL.ELFL2.bit.ELFL_3 ){
		Ddim_Print(("TEST NG : (ELFL_3) %d != %d\n", ctrl_ela_core.elflt[2], IO_ELA.ELFL.ELFL2.bit.ELFL_3));
		err_f = 1;
	}
	if( ctrl_ela_core.elflt[3] != IO_ELA.ELFL.ELFL2.bit.ELFL_4 ){
		Ddim_Print(("TEST NG : (ELFL4) %d != %d\n", ctrl_ela_core.elflt[3], IO_ELA.ELFL.ELFL2.bit.ELFL_4));
		err_f = 1;
	}

	// Noise characteristics of the pixel R
	if( ctrl_ela_core.anb_r.offset[0] != IO_ELA.ANBOR.ANBOR1.bit.ANBOR_0 ){
		Ddim_Print(("TEST NG : (ANBOR_0) %d != %d\n", ctrl_ela_core.anb_r.offset[0], IO_ELA.ANBOR.ANBOR1.bit.ANBOR_0));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_r.offset[1] != IO_ELA.ANBOR.ANBOR1.bit.ANBOR_1 ){
		Ddim_Print(("TEST NG : (ANBOR_1) %d != %d\n", ctrl_ela_core.anb_r.offset[1], IO_ELA.ANBOR.ANBOR1.bit.ANBOR_1));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_r.offset[2] != IO_ELA.ANBOR.ANBOR2.bit.ANBOR_2 ){
		Ddim_Print(("TEST NG : (ANBOR_2) %d != %d\n", ctrl_ela_core.anb_r.offset[2], IO_ELA.ANBOR.ANBOR2.bit.ANBOR_2));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_r.offset[3] != IO_ELA.ANBOR.ANBOR2.bit.ANBOR_3 ){
		Ddim_Print(("TEST NG : (ANBOR_3) %d != %d\n", ctrl_ela_core.anb_r.offset[3], IO_ELA.ANBOR.ANBOR2.bit.ANBOR_3));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_r.gain[0] != IO_ELA.ANBKR.ANBKR1.bit.ANBKR_0 ){
		Ddim_Print(("TEST NG : (ANBKR_0) %d != %d\n", ctrl_ela_core.anb_r.gain[0], IO_ELA.ANBKR.ANBKR1.bit.ANBKR_0));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_r.gain[1] != IO_ELA.ANBKR.ANBKR1.bit.ANBKR_1 ){
		Ddim_Print(("TEST NG : (ANBKR_1) %d != %d\n", ctrl_ela_core.anb_r.gain[1], IO_ELA.ANBKR.ANBKR1.bit.ANBKR_1));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_r.gain[2] != IO_ELA.ANBKR.ANBKR2.bit.ANBKR_2 ){
		Ddim_Print(("TEST NG : (ANBKR_2) %d != %d\n", ctrl_ela_core.anb_r.gain[2], IO_ELA.ANBKR.ANBKR2.bit.ANBKR_2));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_r.gain[3] != IO_ELA.ANBKR.ANBKR2.bit.ANBKR_3 ){
		Ddim_Print(("TEST NG : (ANBKR_3) %d != %d\n", ctrl_ela_core.anb_r.gain[3], IO_ELA.ANBKR.ANBKR2.bit.ANBKR_3));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_r.boundary[0] != IO_ELA.ANBDR.ANBDR1.bit.ANBDR_0 ){
		Ddim_Print(("TEST NG : (ANBDR_0) %d != %d\n", ctrl_ela_core.anb_r.boundary[0], IO_ELA.ANBDR.ANBDR1.bit.ANBDR_0));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_r.boundary[1] != IO_ELA.ANBDR.ANBDR1.bit.ANBDR_1 ){
		Ddim_Print(("TEST NG : (ANBDR_1) %d != %d\n", ctrl_ela_core.anb_r.boundary[1], IO_ELA.ANBDR.ANBDR1.bit.ANBDR_1));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_r.boundary[2] != IO_ELA.ANBDR.ANBDR2.bit.ANBDR_2 ){
		Ddim_Print(("TEST NG: (ANBDR_2) %d != %d\n", ctrl_ela_core.anb_r.boundary[2], IO_ELA.ANBDR.ANBDR2.bit.ANBDR_2));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_r.boundary[3] != IO_ELA.ANBDR.ANBDR2.bit.ANBDR_3 ){
		Ddim_Print(("TEST NG : (ANBDR_3) %d != %d\n", ctrl_ela_core.anb_r.boundary[3], IO_ELA.ANBDR.ANBDR2.bit.ANBDR_3));
		err_f = 1;
	}

	// Noise characteristics of the pixel G
	if( ctrl_ela_core.anb_g.offset[0] != IO_ELA.ANBOG.ANBOG1.bit.ANBOG_0 ){
		Ddim_Print(("TEST NG : (ANBOG_0) %d != %d\n", ctrl_ela_core.anb_g.offset[0], IO_ELA.ANBOG.ANBOG1.bit.ANBOG_0));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_g.offset[1] != IO_ELA.ANBOG.ANBOG1.bit.ANBOG_1 ){
		Ddim_Print(("TEST NG : (ANBOG_1) %d != %d\n", ctrl_ela_core.anb_g.offset[1], IO_ELA.ANBOG.ANBOG1.bit.ANBOG_1));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_g.offset[2] != IO_ELA.ANBOG.ANBOG2.bit.ANBOG_2 ){
		Ddim_Print(("TEST NG : (ANBOG_2) %d != %d\n", ctrl_ela_core.anb_g.offset[2], IO_ELA.ANBOG.ANBOG2.bit.ANBOG_2));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_g.offset[3] != IO_ELA.ANBOG.ANBOG2.bit.ANBOG_3 ){
		Ddim_Print(("TEST NG : (ANBOG_3) %d != %d\n", ctrl_ela_core.anb_g.offset[3], IO_ELA.ANBOG.ANBOG2.bit.ANBOG_3));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_g.gain[0] != IO_ELA.ANBKG.ANBKG1.bit.ANBKG_0 ){
		Ddim_Print(("TEST NG : (ANBKG_0) %d != %d\n", ctrl_ela_core.anb_g.gain[0], IO_ELA.ANBKG.ANBKG1.bit.ANBKG_0));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_g.gain[1] != IO_ELA.ANBKG.ANBKG1.bit.ANBKG_1 ){
		Ddim_Print(("TEST NG : (ANBKG_1) %d != %d\n", ctrl_ela_core.anb_g.gain[1], IO_ELA.ANBKG.ANBKG1.bit.ANBKG_1));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_g.gain[2] != IO_ELA.ANBKG.ANBKG2.bit.ANBKG_2 ){
		Ddim_Print(("TEST NG : (ANBKG_2) %d != %d\n", ctrl_ela_core.anb_g.gain[2], IO_ELA.ANBKG.ANBKG2.bit.ANBKG_2));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_g.gain[3] != IO_ELA.ANBKG.ANBKG2.bit.ANBKG_3 ){
		Ddim_Print(("TEST NG : (ANBKG_3) %d != %d\n", ctrl_ela_core.anb_g.gain[3], IO_ELA.ANBKG.ANBKG2.bit.ANBKG_3));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_g.boundary[0] != IO_ELA.ANBDG.ANBDG1.bit.ANBDG_0 ){
		Ddim_Print(("TEST NG : (ANBDG_0) %d != %d\n", ctrl_ela_core.anb_g.boundary[0], IO_ELA.ANBDG.ANBDG1.bit.ANBDG_0));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_g.boundary[1] != IO_ELA.ANBDG.ANBDG1.bit.ANBDG_1 ){
		Ddim_Print(("TEST NG : (ANBDG_1) %d != %d\n", ctrl_ela_core.anb_g.boundary[1], IO_ELA.ANBDG.ANBDG1.bit.ANBDG_1));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_g.boundary[2] != IO_ELA.ANBDG.ANBDG2.bit.ANBDG_2 ){
		Ddim_Print(("TEST NG: (ANBDG_2) %d != %d\n", ctrl_ela_core.anb_g.boundary[2], IO_ELA.ANBDG.ANBDG2.bit.ANBDG_2));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_g.boundary[3] != IO_ELA.ANBDG.ANBDG2.bit.ANBDG_3 ){
		Ddim_Print(("TEST NG : (ANBDG_3) %d != %d\n", ctrl_ela_core.anb_g.boundary[3], IO_ELA.ANBDG.ANBDG2.bit.ANBDG_3));
		err_f = 1;
	}

	// Noise characteristics of the pixel B
	if( ctrl_ela_core.anb_b.offset[0] != IO_ELA.ANBOB.ANBOB1.bit.ANBOB_0 ){
		Ddim_Print(("TEST NG : (ANBOB_0) %d != %d\n", ctrl_ela_core.anb_b.offset[0], IO_ELA.ANBOB.ANBOB1.bit.ANBOB_0));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_b.offset[1] != IO_ELA.ANBOB.ANBOB1.bit.ANBOB_1 ){
		Ddim_Print(("TEST NG : (ANBOB_1) %d != %d\n", ctrl_ela_core.anb_b.offset[1], IO_ELA.ANBOB.ANBOB1.bit.ANBOB_1));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_b.offset[2] != IO_ELA.ANBOB.ANBOB2.bit.ANBOB_2 ){
		Ddim_Print(("TEST NG : (ANBOB_2) %d != %d\n", ctrl_ela_core.anb_b.offset[2], IO_ELA.ANBOB.ANBOB2.bit.ANBOB_2));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_b.offset[3] != IO_ELA.ANBOB.ANBOB2.bit.ANBOB_3 ){
		Ddim_Print(("TEST NG : (ANBOB_3) %d != %d\n", ctrl_ela_core.anb_b.offset[3], IO_ELA.ANBOB.ANBOB2.bit.ANBOB_3));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_b.gain[0] != IO_ELA.ANBKB.ANBKB1.bit.ANBKB_0 ){
		Ddim_Print(("TEST NG : (ANBKB_0) %d != %d\n", ctrl_ela_core.anb_b.gain[0], IO_ELA.ANBKB.ANBKB1.bit.ANBKB_0));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_b.gain[1] != IO_ELA.ANBKB.ANBKB1.bit.ANBKB_1 ){
		Ddim_Print(("TEST NG : (ANBKB_1) %d != %d\n", ctrl_ela_core.anb_b.gain[1], IO_ELA.ANBKB.ANBKB1.bit.ANBKB_1));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_b.gain[2] != IO_ELA.ANBKB.ANBKB2.bit.ANBKB_2 ){
		Ddim_Print(("TEST NG : (ANBKB_2) %d != %d\n", ctrl_ela_core.anb_b.gain[2], IO_ELA.ANBKB.ANBKB2.bit.ANBKB_2));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_b.gain[3] != IO_ELA.ANBKB.ANBKB2.bit.ANBKB_3 ){
		Ddim_Print(("TEST NG : (ANBKB_3) %d != %d\n", ctrl_ela_core.anb_b.gain[3], IO_ELA.ANBKB.ANBKB2.bit.ANBKB_3));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_b.boundary[0] != IO_ELA.ANBDB.ANBDB1.bit.ANBDB_0 ){
		Ddim_Print(("TEST NG : (ANBDB_0) %d != %d\n", ctrl_ela_core.anb_b.boundary[0], IO_ELA.ANBDB.ANBDB1.bit.ANBDB_0));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_b.boundary[1] != IO_ELA.ANBDB.ANBDB1.bit.ANBDB_1 ){
		Ddim_Print(("TEST NG : (ANBDB_1) %d != %d\n", ctrl_ela_core.anb_b.boundary[1], IO_ELA.ANBDB.ANBDB1.bit.ANBDB_1));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_b.boundary[2] != IO_ELA.ANBDB.ANBDB2.bit.ANBDB_2 ){
		Ddim_Print(("TEST NG : (ANBDB_2) %d != %d\n", ctrl_ela_core.anb_b.boundary[2], IO_ELA.ANBDB.ANBDB2.bit.ANBDB_2));
		err_f = 1;
	}
	if( ctrl_ela_core.anb_b.boundary[3] != IO_ELA.ANBDB.ANBDB2.bit.ANBDB_3 ){
		Ddim_Print(("TEST NG : (ANBDB_3) %d != %d\n", ctrl_ela_core.anb_b.boundary[3], IO_ELA.ANBDB.ANBDB2.bit.ANBDB_3));
		err_f = 1;
	}

	if( ctrl_ela_core.lumen_gain_r != IO_ELA.LGTW.LGTW1.bit.LGTWR ){
		Ddim_Print(("TEST NG : (LGTWR) %d != %d\n", ctrl_ela_core.lumen_gain_r, IO_ELA.LGTW.LGTW1.bit.LGTWR));
		err_f = 1;
	}
	if( ctrl_ela_core.lumen_gain_g != IO_ELA.LGTW.LGTW1.bit.LGTWG ){
		Ddim_Print(("TEST NG : (LGTWG) %d != %d\n", ctrl_ela_core.lumen_gain_g, IO_ELA.LGTW.LGTW1.bit.LGTWG));
		err_f = 1;
	}
	if( ctrl_ela_core.lumen_gain_b != IO_ELA.LGTW.LGTW2.bit.LGTWB ){
		Ddim_Print(("TEST NG : (LGTWB) %d != %d\n", ctrl_ela_core.lumen_gain_b, IO_ELA.LGTW.LGTW2.bit.LGTWB));
		err_f = 1;
	}

	if( ctrl_ela_core.lumen_factor_r != IO_ELA.LGT.bit.LGTR ){
		Ddim_Print(("TEST NG : (LGTR) %d != %d\n", ctrl_ela_core.lumen_factor_r, IO_ELA.LGT.bit.LGTR));
		err_f = 1;
	}
	if( ctrl_ela_core.lumen_factor_g != IO_ELA.LGT.bit.LGTG ){
		Ddim_Print(("TEST NG : (LGTG) %d != %d\n", ctrl_ela_core.lumen_factor_g, IO_ELA.LGT.bit.LGTG));
		err_f = 1;
	}
	if( ctrl_ela_core.lumen_factor_b != IO_ELA.LGT.bit.LGTB ){
		Ddim_Print(("TEST NG : (LGTB) %d != %d\n", ctrl_ela_core.lumen_factor_b, IO_ELA.LGT.bit.LGTB));
		err_f = 1;
	}
	if( ctrl_ela_core.lumen_factor_c != IO_ELA.LGT.bit.LGTC ){
		Ddim_Print(("TEST NG : (LGTC) %d != %d\n", ctrl_ela_core.lumen_factor_c, IO_ELA.LGT.bit.LGTC));
		err_f = 1;
	}

	if( ctrl_ela_core.lumen_edge_r != IO_ELA.LGTE.bit.LGTER ){
		Ddim_Print(("TEST NG : (LGTER) %d != %d\n", ctrl_ela_core.lumen_edge_r, IO_ELA.LGTE.bit.LGTER));
		err_f = 1;
	}
	if( ctrl_ela_core.lumen_edge_g != IO_ELA.LGTE.bit.LGTEG ){
		Ddim_Print(("TEST NG : (LGTEG) %d != %d\n", ctrl_ela_core.lumen_edge_g, IO_ELA.LGTE.bit.LGTEG));
		err_f = 1;
	}
	if( ctrl_ela_core.lumen_edge_b != IO_ELA.LGTE.bit.LGTEB ){
		Ddim_Print(("TEST NG : (LGTEB) %d != %d\n", ctrl_ela_core.lumen_edge_b, IO_ELA.LGTE.bit.LGTEB));
		err_f = 1;
	}

	if( ctrl_ela_core.edge_adjust_a1 != IO_ELA.EDG2ANBA.bit.EDG2ANBA ){
		Ddim_Print(("TEST NG : (EDG2ANBA) %d != %d\n", ctrl_ela_core.edge_adjust_a1, IO_ELA.EDG2ANBA.bit.EDG2ANBA));
		err_f = 1;
	}
	if( ctrl_ela_core.edge_adjust_a2 != IO_ELA.EDGSCLA.bit.EDGSCLA ){
		Ddim_Print(("TEST NG : (EDGSCLA) %d != %d\n", ctrl_ela_core.edge_adjust_a2, IO_ELA.EDGSCLA.bit.EDGSCLA));
		err_f = 1;
	}
	if( ctrl_ela_core.edge_adjust_b1 != IO_ELA.EDG2ANBB.bit.EDG2ANBB ){
		Ddim_Print(("TEST NG : (EDG2ANBB) %d != %d\n", ctrl_ela_core.edge_adjust_b1, IO_ELA.EDG2ANBB.bit.EDG2ANBB));
		err_f = 1;
	}
	if( ctrl_ela_core.edge_adjust_b2 != IO_ELA.EDGSCLB.bit.EDGSCLB ){
		Ddim_Print(("TEST NG : (EDGSCLB) %d != %d\n", ctrl_ela_core.edge_adjust_b2, IO_ELA.EDGSCLB.bit.EDGSCLB));
		err_f = 1;
	}

	if( ctrl_ela_core.edge_threshold[0] != IO_ELA.ALNGE.ALNGE1.bit.ALNGE_1 ){
		Ddim_Print(("TEST NG : (ALNGE_1) %d != %d\n", ctrl_ela_core.edge_threshold[0], IO_ELA.ALNGE.ALNGE1.bit.ALNGE_1));
		err_f = 1;
	}
	if( ctrl_ela_core.edge_threshold[1] != IO_ELA.ALNGE.ALNGE1.bit.ALNGE_2 ){
		Ddim_Print(("TEST NG : (ALNGE_2) %d != %d\n", ctrl_ela_core.edge_threshold[1], IO_ELA.ALNGE.ALNGE1.bit.ALNGE_2));
		err_f = 1;
	}
	if( ctrl_ela_core.edge_threshold[2] != IO_ELA.ALNGE.ALNGE2.bit.ALNGE_3 ){
		Ddim_Print(("TEST NG : (ALNGE_3) %d != %d\n", ctrl_ela_core.edge_threshold[2], IO_ELA.ALNGE.ALNGE2.bit.ALNGE_3));
		err_f = 1;
	}

	if( ctrl_ela_core.nr[0] != IO_ELA.ALNG.ALNG1.bit.ALNGL1E1N ){
		Ddim_Print(("TEST NG : (ALNGL1E1N) %d != %d\n", ctrl_ela_core.nr[0], IO_ELA.ALNG.ALNG1.bit.ALNGL1E1N));
		err_f = 1;
	}
	if( ctrl_ela_core.nr[1] != IO_ELA.ALNG.ALNG1.bit.ALNGL1E2N ){
		Ddim_Print(("TEST NG : (ALNGL1E2N) %d != %d\n", ctrl_ela_core.nr[1], IO_ELA.ALNG.ALNG1.bit.ALNGL1E2N));
		err_f = 1;
	}
	if( ctrl_ela_core.nr[2] != IO_ELA.ALNG.ALNG2.bit.ALNGL1E3N ){
		Ddim_Print(("TEST NG : (ALNGL1E2N) %d != %d\n", ctrl_ela_core.nr[2], IO_ELA.ALNG.ALNG2.bit.ALNGL1E3N));
		err_f = 1;
	}
	if( ctrl_ela_core.nr[3] != IO_ELA.ALNG.ALNG2.bit.ALNGL2E1N ){
		Ddim_Print(("TEST NG : (ALNGL1E2N) %d != %d\n", ctrl_ela_core.nr[3], IO_ELA.ALNG.ALNG2.bit.ALNGL2E1N));
		err_f = 1;
	}
	if( ctrl_ela_core.nr[4] != IO_ELA.ALNG.ALNG3.bit.ALNGL2E2N ){
		Ddim_Print(("TEST NG : (ALNGL2E2N) %d != %d\n", ctrl_ela_core.nr[4], IO_ELA.ALNG.ALNG3.bit.ALNGL2E2N));
		err_f = 1;
	}
	if( ctrl_ela_core.nr[5] != IO_ELA.ALNG.ALNG3.bit.ALNGL2E3N ){
		Ddim_Print(("TEST NG : (ALNGL2E3N) %d != %d\n", ctrl_ela_core.nr[5], IO_ELA.ALNG.ALNG3.bit.ALNGL2E3N));
		err_f = 1;
	}
	if( ctrl_ela_core.nr[6] != IO_ELA.ALNG.ALNG4.bit.ALNGL3E1N ){
		Ddim_Print(("TEST NG : (ALNGL3E1N) %d != %d\n", ctrl_ela_core.nr[6], IO_ELA.ALNG.ALNG4.bit.ALNGL3E1N));
		err_f = 1;
	}
	if( ctrl_ela_core.nr[7] != IO_ELA.ALNG.ALNG4.bit.ALNGL3E2N ){
		Ddim_Print(("TEST NG : (ALNGL3E2N) %d != %d\n", ctrl_ela_core.nr[7], IO_ELA.ALNG.ALNG4.bit.ALNGL3E2N));
		err_f = 1;
	}
	if( ctrl_ela_core.nr[8] != IO_ELA.ALNG.ALNG5.bit.ALNGL3E3N ){
		Ddim_Print(("TEST NG : (ALNGL3E3N) %d != %d\n", ctrl_ela_core.nr[8], IO_ELA.ALNG.ALNG5.bit.ALNGL3E3N));
		err_f = 1;
	}
	if( ctrl_ela_core.nr[9] != IO_ELA.ALNG.ALNG5.bit.ALNGL4E1N ){
		Ddim_Print(("TEST NG : (ALNGL4E1N) %d != %d\n", ctrl_ela_core.nr[9], IO_ELA.ALNG.ALNG5.bit.ALNGL4E1N));
		err_f = 1;
	}
	if( ctrl_ela_core.nr[10] != IO_ELA.ALNG.ALNG6.bit.ALNGL4E2N ){
		Ddim_Print(("TEST NG : (ALNGL4E2N) %d != %d\n", ctrl_ela_core.nr[10], IO_ELA.ALNG.ALNG6.bit.ALNGL4E2N));
		err_f = 1;
	}
	if( ctrl_ela_core.nr[11] != IO_ELA.ALNG.ALNG6.bit.ALNGL4E3N ){
		Ddim_Print(("TEST NG : (ALNGL4E3N) %d != %d\n", ctrl_ela_core.nr[11], IO_ELA.ALNG.ALNG6.bit.ALNGL4E3N));
		err_f = 1;
	}

	if( ctrl_ela_core.l_noise != IO_ELA.LFTH.bit.LFTH ){
		Ddim_Print(("TEST NG : (LFTH) %d != %d\n", ctrl_ela_core.l_noise, IO_ELA.LFTH.bit.LFTH));
		err_f = 1;
	}
	
	if( ctrl_ela_core.l_edge[0] != IO_ELA.LCOME.bit.LCOME1 ){
		Ddim_Print(("TEST NG : (LCOME1) %d != %d\n", ctrl_ela_core.l_edge[0], IO_ELA.LCOME.bit.LCOME1));
		err_f = 1;
	}
	if( ctrl_ela_core.l_edge[1] != IO_ELA.LCOME.bit.LCOME2 ){
		Ddim_Print(("TEST NG : (LCOME1) %d != %d\n", ctrl_ela_core.l_edge[1], IO_ELA.LCOME.bit.LCOME2));
		err_f = 1;
	}
	
	if( ctrl_ela_core.l_intense[0] != IO_ELA.LCOMEN.bit.LCOME1N ){
		Ddim_Print(("TEST NG : (LCOME1N) %d != %d\n", ctrl_ela_core.l_intense[0], IO_ELA.LCOMEN.bit.LCOME1N));
		err_f = 1;
	}
	if( ctrl_ela_core.l_intense[1] != IO_ELA.LCOMEN.bit.LCOME2N ){
		Ddim_Print(("TEST NG : (LCOME2N) %d != %d\n", ctrl_ela_core.l_intense[1], IO_ELA.LCOMEN.bit.LCOME2N));
		err_f = 1;
	}

	ct_im_ela_off_pclk();

	if( !err_f ){
		Ddim_Print(("TEST OK!\n"));
	}
	ct_im_ela_clk_chk();

	Im_ELA_Close();
}

VOID CT_Im_Ela_1_11( VOID )
{
	UINT32 result;
	T_IM_ELA_CTRL_OB_CORRECTION ctrl_ela_ob;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	IO_ELA.OBOF.OBOF1.bit.EOBRR = -1;
	IO_ELA.OBOF.OBOF1.bit.EOBBB = -16;
	IO_ELA.OBOF.OBOF2.bit.EOBGR = -256;
	IO_ELA.OBOF.OBOF2.bit.EOBGB = 256;

	result = Im_ELA_Get_Ctrl_Ob_Correction( &ctrl_ela_ob );
	Ddim_Print(("result=0x%08X\n", result));

	ct_im_ela_on_pclk();
	if( ctrl_ela_ob.ob_rr != IO_ELA.OBOF.OBOF1.bit.EOBRR ){
		Ddim_Print(("TEST NG : (EOBRR) %d != %d\n", ctrl_ela_ob.ob_rr, IO_ELA.OBOF.OBOF1.bit.EOBRR));
	}
	else if( ctrl_ela_ob.ob_bb != IO_ELA.OBOF.OBOF1.bit.EOBBB ){
		Ddim_Print(("TEST NG : (EOBBB) %d != %d\n", ctrl_ela_ob.ob_bb, IO_ELA.OBOF.OBOF1.bit.EOBBB));
	}
	else if( ctrl_ela_ob.ob_gr != IO_ELA.OBOF.OBOF2.bit.EOBGR ){
		Ddim_Print(("TEST NG : (EOBGR) %d != %d\n", ctrl_ela_ob.ob_gr, IO_ELA.OBOF.OBOF2.bit.EOBGR));
	}
	else if( ctrl_ela_ob.ob_gb != IO_ELA.OBOF.OBOF2.bit.EOBGB ){
		Ddim_Print(("TEST NG : (EOBGB) %d != %d\n", ctrl_ela_ob.ob_gb, IO_ELA.OBOF.OBOF2.bit.EOBGB));
	}
	else{
		Ddim_Print(("TEST OK!\n"));
	}
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();

	Im_ELA_Close();
}

VOID CT_Im_Ela_1_12( VOID )
{
	USHORT zero_level_shift;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	zero_level_shift = Im_ELA_Get_Optical_Zero_Level_Shift();

	ct_im_ela_on_pclk();
	if( zero_level_shift != IO_ELA.EZSFTA1.bit.EZSFTA1 ){
		Ddim_Print(("TEST NG : (EZSFTA1) %d != %d\n", zero_level_shift, IO_ELA.EZSFTA1.bit.EZSFTA1));
	}
	else{
		Ddim_Print(("TEST OK!\n"));
	}
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();

	Im_ELA_Close();
}

VOID CT_Im_Ela_1_13( VOID )
{
	UINT32 i,j;
	UINT32 result;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		core[i].optical_zero_level_shift = 0x07FF;					// 2047
		core[i].defect_correction_mode = D_IM_ELA_ELADCMD_WEAK;		// 1
		core[i].zero_point_level = 0x0FFF;							// 4095

		for( j = 0; j < E_IM_ELA_ELFL_MAX; j++ ){
			core[i].elflt[j] = 0x0FFF;								// 4095
		}
		for( j = 0; j < E_IM_ELA_ANB_MAX; j++ ){
			core[i].anb_r.offset[j]   = 0x0FFF;						// 4095
			core[i].anb_r.gain[j]     = 0x0FFF;						// 4095
			core[i].anb_r.boundary[j] = 0x0FF0;						// 4080

			core[i].anb_g.offset[j]   = 0x0FFF;						// 4095
			core[i].anb_g.gain[j]     = 0x0FFF;						// 4095
			core[i].anb_g.boundary[j] = 0x0FF0;						// 4080

			core[i].anb_b.offset[j]   = 0x0FFF;						// 4095
			core[i].anb_b.gain[j]     = 0x0FFF;						// 4095
			core[i].anb_b.boundary[j] = 0x0FF0;						// 4080
		}
		core[i].lumen_gain_r = 0x03FF;								// 1023
		core[i].lumen_gain_g = 0x03FF;
		core[i].lumen_gain_b = 0x03FF;

		core[i].lumen_factor_r = 0x10;
		core[i].lumen_factor_g = 0x10;
		core[i].lumen_factor_b = 0x10;
		core[i].lumen_factor_c = 0x10;

		core[i].lumen_edge_r = 0x10;
		core[i].lumen_edge_g = 0x10;
		core[i].lumen_edge_b = 0x10;

		core[i].edge_adjust_a1 = 0x00FF;							// 255
		core[i].edge_adjust_a2 = 0x03FF;							// 1023
		core[i].edge_adjust_b1 = 0x03FF;							// 1023
		core[i].edge_adjust_b2 = 0x0FFF;							// 4095

		for( j = 0; j < E_IM_ELA_ALNGE_MAX; j++ ){
			core[i].edge_threshold[j] = 0x0FFF;						// 4095
		}
		for( j = 0; j < E_IM_ELA_ALNG_MAX; j++ ){
			core[i].nr[j] = 0x01FF;									//511
		}
		core[i].l_noise = 0x0FFF;
		for( j = 0; j < E_IM_ELA_LCOME_MAX; j++ ){
			core[i].l_edge[j] = 0x0FFF;
		}
		for( j = 0; j < E_IM_ELA_LCOMEN_MAX; j++ ){
			core[i].l_intense[j] = 0x0100;
		}
	}
	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	Ddim_Print(("result=0x%08X\n", result));

#if 1
	extern volatile T_IM_ELA_MULTIPLE_TIMES_INFO	gIM_ELA_Multiple_Exec_Info;

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		Ddim_Print(("<<<STAGE=%d>>>\n", i));
		Ddim_Print(("core[%d].optical_zero_level_shift=%d\n", i, gIM_ELA_Multiple_Exec_Info.core[i]->optical_zero_level_shift));
		Ddim_Print(("core[%d].defect_correction_mode=%d\n", i, gIM_ELA_Multiple_Exec_Info.core[i]->defect_correction_mode));
		Ddim_Print(("core[%d].zero_point_level=%d\n", i, gIM_ELA_Multiple_Exec_Info.core[i]->zero_point_level));

		for( j = 0; j < E_IM_ELA_ELFL_MAX; j++ ){
			Ddim_Print(("core[%d].elflt[%d]=%d\n",i ,j , gIM_ELA_Multiple_Exec_Info.core[i]->elflt[j]));
		}
		for( j = 0; j < E_IM_ELA_ANB_MAX; j++ ){
			Ddim_Print(("core[%d].anb_r.offset[%d]=%d\n",i ,j ,  gIM_ELA_Multiple_Exec_Info.core[i]->anb_r.offset[j]));
			Ddim_Print(("core[%d].anb_r.gain[%d]=%d\n",i ,j ,     gIM_ELA_Multiple_Exec_Info.core[i]->anb_r.gain[j]));
			Ddim_Print(("core[%d].anb_r.boundary[%d]=%d\n",i ,j , gIM_ELA_Multiple_Exec_Info.core[i]->anb_r.boundary[j]));

			Ddim_Print(("core[%d].anb_g.offset[%d]=%d\n",i ,j ,   gIM_ELA_Multiple_Exec_Info.core[i]->anb_g.offset[j]));
			Ddim_Print(("core[%d].anb_g.gain[%d]=%d\n",i ,j ,     gIM_ELA_Multiple_Exec_Info.core[i]->anb_g.gain[j]));
			Ddim_Print(("core[%d].anb_g.boundary[%d]=%d\n",i ,j , gIM_ELA_Multiple_Exec_Info.core[i]->anb_g.boundary[j]));

			Ddim_Print(("core[%d].anb_b.offset[%d]=%d\n",i ,j ,   gIM_ELA_Multiple_Exec_Info.core[i]->anb_b.offset[j]));
			Ddim_Print(("core[%d].anb_b.gain[%d]=%d\n",i ,j ,     gIM_ELA_Multiple_Exec_Info.core[i]->anb_b.gain[j]));
			Ddim_Print(("core[%d].anb_b.boundary[%d]=%d\n",i ,j , gIM_ELA_Multiple_Exec_Info.core[i]->anb_b.boundary[j]));
		}
		Ddim_Print(("core[%d].lumen_gain_r=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->lumen_gain_r));
		Ddim_Print(("core[%d].lumen_gain_g=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->lumen_gain_g));
		Ddim_Print(("core[%d].lumen_gain_b=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->lumen_gain_b));

		Ddim_Print(("core[%d].lumen_factor_r=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->lumen_factor_r));
		Ddim_Print(("core[%d].lumen_factor_g=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->lumen_factor_g));
		Ddim_Print(("core[%d].lumen_factor_b=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->lumen_factor_b));
		Ddim_Print(("core[%d].lumen_factor_c=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->lumen_factor_c));

		Ddim_Print(("core[%d].lumen_edge_r=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->lumen_edge_r));
		Ddim_Print(("core[%d].lumen_edge_g=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->lumen_edge_g));
		Ddim_Print(("core[%d].lumen_edge_b=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->lumen_edge_b));

		Ddim_Print(("core[%d].edge_adjust_a1=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->edge_adjust_a1));
		Ddim_Print(("core[%d].edge_adjust_a2=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->edge_adjust_a2));
		Ddim_Print(("core[%d].edge_adjust_b1=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->edge_adjust_b1));
		Ddim_Print(("core[%d].edge_adjust_b2=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->edge_adjust_b2));

		for( j = 0; j < E_IM_ELA_ALNGE_MAX; j++ ){
			Ddim_Print(("core[%d].edge_threshold[%d]=%d\n",i ,j ,   gIM_ELA_Multiple_Exec_Info.core[i]->edge_threshold[j]));
		}
		for( j = 0; j < E_IM_ELA_ALNG_MAX; j++ ){
			Ddim_Print(("core[%d].nr[%d]=%d\n",i ,j ,   gIM_ELA_Multiple_Exec_Info.core[i]->nr[j]));
		}
		Ddim_Print(("core[%d].l_noise=%d\n",i ,   gIM_ELA_Multiple_Exec_Info.core[i]->l_noise));
		for( j = 0; j < E_IM_ELA_LCOME_MAX; j++ ){
			Ddim_Print(("core[%d].l_edge[%d]=%d\n",i ,j ,   gIM_ELA_Multiple_Exec_Info.core[i]->l_edge[j]));
		}
		for( j = 0; j < E_IM_ELA_LCOMEN_MAX; j++ ){
			Ddim_Print(("core[%d].l_intense[%d]=%d\n",i ,j ,   gIM_ELA_Multiple_Exec_Info.core[i]->l_intense[j]));
		}
		DDIM_User_Dly_Tsk( 10 );
	}
#endif
	ct_im_ela_clk_chk();

	Im_ELA_Close();
}

extern volatile INT32 gIM_ELA_Axi_Err;

VOID CT_Im_Ela_1_14( VOID )
{
	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Ddim_Print(( "(1)\n"));
	IO_ELA.ELAINTE.bit.AXWERE = 1;
	IO_ELA.ELAINTE.bit.AXRERE = 1;
	IO_ELA.ELAINTE.bit.ENWE = 1;
	IO_ELA.ELAINTF.bit.__AXWERF = 1;
	IO_ELA.ELAINTF.bit.__AXRERF = 1;
	IO_ELA.ELAINTF.bit.__ENWF = 1;
	IO_ELA.ELATRG.bit.ELATRG = 3;

	Im_ELA_Int_Handler();

	Ddim_Print(( "(1)gIM_ELA_Axi_Err = %d\n",gIM_ELA_Axi_Err ));

	Ddim_Print(( "(2)\n"));
	IO_ELA.ELAINTE.bit.AXWERE = 1;
	IO_ELA.ELAINTE.bit.AXRERE = 1;
	IO_ELA.ELAINTE.bit.ENWE = 1;
	IO_ELA.ELAINTF.bit.__AXWERF = 1;
	IO_ELA.ELAINTF.bit.__AXRERF = 0;
	IO_ELA.ELAINTF.bit.__ENWF = 1;
	IO_ELA.ELATRG.bit.ELATRG = 3;
	Im_ELA_Int_Handler();
	Ddim_Print(( "(2)gIM_ELA_Axi_Err = %d\n",gIM_ELA_Axi_Err ));

	Ddim_Print(( "(3)\n"));
	IO_ELA.ELAINTE.bit.AXWERE = 1;
	IO_ELA.ELAINTE.bit.AXRERE = 1;
	IO_ELA.ELAINTE.bit.ENWE = 1;
	IO_ELA.ELAINTF.bit.__AXWERF = 0;
	IO_ELA.ELAINTF.bit.__AXRERF = 1;
	IO_ELA.ELAINTF.bit.__ENWF = 1;
	IO_ELA.ELATRG.bit.ELATRG = 3;
	Im_ELA_Int_Handler();
	Ddim_Print(( "(3)gIM_ELA_Axi_Err = %d\n",gIM_ELA_Axi_Err ));
}

VOID CT_Im_Ela_1_15( VOID )
{
	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	IO_ELA.ELAINTE.bit.AXWERE = 1;
	IO_ELA.ELAINTE.bit.AXRERE = 1;
	IO_ELA.ELAINTE.bit.ENWE = 1;
	IO_ELA.ELAINTF.bit.__AXWERF = 0;
	IO_ELA.ELAINTF.bit.__AXRERF = 0;
	IO_ELA.ELAINTF.bit.__ENWF = 1;
	IO_ELA.ELATRG.bit.ELATRG = 3;

	Im_ELA_Int_Handler();

	Ddim_Print(( "gIM_ELA_Axi_Err] = %d\n", gIM_ELA_Axi_Err ));
}

VOID CT_Im_Ela_1_16( VOID )
{
	UINT32 err_f = 0;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	ct_im_ela_on_pclk();
	Im_ELA_Set_Deknee_Lut_Sleep( D_IM_ELA_ENABLE );
	if( IO_ELA.ELABYR.bit.DKNACT != 1 ){
		Ddim_Print(("TEST NG : Deknee sleep \n"));
		err_f = 1;
	}
	Im_ELA_Set_Deknee_Lut_Sleep( D_IM_ELA_DISABLE );
	if( IO_ELA.ELABYR.bit.DKNACT != 0 ){
		Ddim_Print(("TEST NG : Deknee wake \n"));
		err_f = 1;
	}

	if( !err_f ){
		Ddim_Print(("TEST OK!\n"));
	}
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_1_17( VOID )
{
	UINT32 err_f = 0;
	T_IM_ELA_CTRL_NSLMD nslmd;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Get_Nslmd( &nslmd );

	ct_im_ela_on_pclk();
	if( nslmd.noise_data_mode != IO_ELA.NSLMD.bit.NSLMD ){
		Ddim_Print(("TEST NG : (NSLMD) %d != %d\n", nslmd.noise_data_mode, IO_ELA.NSLMD.bit.NSLMD));
		err_f = 1;
	}
	if( nslmd.noise_data_precision != IO_ELA.NSLMD.bit.NSLRES ){
		Ddim_Print(("TEST NG : (NSLRES) %d != %d\n", nslmd.noise_data_precision, IO_ELA.NSLMD.bit.NSLRES));
		err_f = 1;
	}
	if( nslmd.noise_data_compress_enable != IO_ELA.NSLMD.bit.NSLKNE ){
		Ddim_Print(("TEST NG : (NSLKNE) %d != %d\n", nslmd.noise_data_compress_enable, IO_ELA.NSLMD.bit.NSLKNE));
		err_f = 1;
	}

	if( !err_f ){
		Ddim_Print(("TEST OK!\n"));
	}
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

T_IM_ELA_CTRL_EACH g_ctrl_each;

VOID CT_Im_Ela_1_18( VOID )
{
	UINT32 i;
	UINT32 err_f = 0;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	for( i = 0; i < D_IM_ELA_EACH_DATA_MAX; i++ ){
		g_ctrl_each.cache_type_r[i] = i;
		g_ctrl_each.cache_type_w[i] = i+1;
		g_ctrl_each.protect_type_r[i] = i+1;
		g_ctrl_each.protect_type_w[i] = i;
		g_ctrl_each.req_mask_r[i] = i+10;
		g_ctrl_each.req_mask_w[i] = i+20;
	}

	Im_ELA_Set_EAch( &g_ctrl_each );

	ct_im_ela_on_pclk();
	if( g_ctrl_each.cache_type_r[0] != IO_ELA.EARCACHE.bit.EARCACHE_0 ){
		Ddim_Print(("TEST NG : (EARCACHE_0) %d != %d\n", g_ctrl_each.cache_type_r[0], IO_ELA.EARCACHE.bit.EARCACHE_0));
		err_f = 1;
	}
	if( g_ctrl_each.cache_type_r[1] != IO_ELA.EARCACHE.bit.EARCACHE_1 ){
		Ddim_Print(("TEST NG : (EARCACHE_1) %d != %d\n", g_ctrl_each.cache_type_r[1], IO_ELA.EARCACHE.bit.EARCACHE_1));
		err_f = 1;
	}
	if( g_ctrl_each.cache_type_r[2] != IO_ELA.EARCACHE.bit.EARCACHE_2 ){
		Ddim_Print(("TEST NG : (EARCACHE_2) %d != %d\n", g_ctrl_each.cache_type_r[2], IO_ELA.EARCACHE.bit.EARCACHE_2));
		err_f = 1;
	}

	if( g_ctrl_each.cache_type_w[0] != IO_ELA.EAWCACHE.bit.EAWCACHE_0 ){
		Ddim_Print(("TEST NG : (EAWCACHE_0) %d != %d\n", g_ctrl_each.cache_type_w[0], IO_ELA.EAWCACHE.bit.EAWCACHE_0));
		err_f = 1;
	}
	if( g_ctrl_each.cache_type_w[1] != IO_ELA.EAWCACHE.bit.EAWCACHE_1 ){
		Ddim_Print(("TEST NG : (EAWCACHE_1) %d != %d\n", g_ctrl_each.cache_type_w[1], IO_ELA.EAWCACHE.bit.EAWCACHE_1));
		err_f = 1;
	}
	if( g_ctrl_each.cache_type_w[2] != IO_ELA.EAWCACHE.bit.EAWCACHE_2 ){
		Ddim_Print(("TEST NG : (EAWCACHE_2) %d != %d\n", g_ctrl_each.cache_type_w[2], IO_ELA.EAWCACHE.bit.EAWCACHE_2));
		err_f = 1;
	}

	if( g_ctrl_each.protect_type_r[0] != IO_ELA.EARPROT.bit.EARPROT_0 ){
		Ddim_Print(("TEST NG : (EARPROT_0) %d != %d\n", g_ctrl_each.protect_type_r[0], IO_ELA.EARPROT.bit.EARPROT_0));
		err_f = 1;
	}
	if( g_ctrl_each.protect_type_r[1] != IO_ELA.EARPROT.bit.EARPROT_1 ){
		Ddim_Print(("TEST NG : (EARPROT_1) %d != %d\n", g_ctrl_each.protect_type_r[1], IO_ELA.EARPROT.bit.EARPROT_1));
		err_f = 1;
	}
	if( g_ctrl_each.protect_type_r[2] != IO_ELA.EARPROT.bit.EARPROT_2 ){
		Ddim_Print(("TEST NG : (EARPROT_2) %d != %d\n", g_ctrl_each.protect_type_r[2], IO_ELA.EARPROT.bit.EARPROT_2));
		err_f = 1;
	}

	if( g_ctrl_each.protect_type_w[0] != IO_ELA.EAWPROT.bit.EAWPROT_0 ){
		Ddim_Print(("TEST NG : (EAWPROT_0) %d != %d\n", g_ctrl_each.protect_type_w[0], IO_ELA.EAWPROT.bit.EAWPROT_0));
		err_f = 1;
	}
	if( g_ctrl_each.protect_type_w[1] != IO_ELA.EAWPROT.bit.EAWPROT_1 ){
		Ddim_Print(("TEST NG : (EAWPROT_1) %d != %d\n", g_ctrl_each.protect_type_w[1], IO_ELA.EAWPROT.bit.EAWPROT_1));
		err_f = 1;
	}
	if( g_ctrl_each.protect_type_w[2] != IO_ELA.EAWPROT.bit.EAWPROT_2 ){
		Ddim_Print(("TEST NG : (EAWPROT_2) %d != %d\n", g_ctrl_each.protect_type_w[2], IO_ELA.EAWPROT.bit.EAWPROT_2));
		err_f = 1;
	}

	if( g_ctrl_each.req_mask_r[0] != IO_ELA.EARREQMSK.EARREQMSK1.bit.EARREQMSK_0 ){
		Ddim_Print(("TEST NG : (EARREQMSK_0) %d != %d\n", g_ctrl_each.req_mask_r[0], IO_ELA.EARREQMSK.EARREQMSK1.bit.EARREQMSK_0));
		err_f = 1;
	}
	if( g_ctrl_each.req_mask_r[1] != IO_ELA.EARREQMSK.EARREQMSK1.bit.EARREQMSK_1 ){
		Ddim_Print(("TEST NG : (EARREQMSK_1) %d != %d\n", g_ctrl_each.req_mask_r[1], IO_ELA.EARREQMSK.EARREQMSK1.bit.EARREQMSK_1));
		err_f = 1;
	}
	if( g_ctrl_each.req_mask_r[2] != IO_ELA.EARREQMSK.EARREQMSK2.bit.EARREQMSK_2 ){
		Ddim_Print(("TEST NG : (EARREQMSK_2) %d != %d\n", g_ctrl_each.req_mask_r[2], IO_ELA.EARREQMSK.EARREQMSK2.bit.EARREQMSK_2));
		err_f = 1;
	}

	if( g_ctrl_each.req_mask_w[0] != IO_ELA.EAWREQMSK.EAWREQMSK1.bit.EAWREQMSK_0 ){
		Ddim_Print(("TEST NG : (EAWREQMSK_0) %d != %d\n", g_ctrl_each.req_mask_w[0], IO_ELA.EAWREQMSK.EAWREQMSK1.bit.EAWREQMSK_0));
		err_f = 1;
	}
	if( g_ctrl_each.req_mask_w[1] != IO_ELA.EAWREQMSK.EAWREQMSK1.bit.EAWREQMSK_1 ){
		Ddim_Print(("TEST NG : (EAWREQMSK_1) %d != %d\n", g_ctrl_each.req_mask_w[1], IO_ELA.EAWREQMSK.EAWREQMSK1.bit.EAWREQMSK_1));
		err_f = 1;
	}
	if( g_ctrl_each.req_mask_w[2] != IO_ELA.EAWREQMSK.EAWREQMSK2.bit.EAWREQMSK_2 ){
		Ddim_Print(("TEST NG : (EAWREQMSK_2) %d != %d\n", g_ctrl_each.req_mask_w[2], IO_ELA.EAWREQMSK.EAWREQMSK2.bit.EAWREQMSK_2));
		err_f = 1;
	}
	if( !err_f ){
		Ddim_Print(("TEST OK!\n"));
	}
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_1_19( VOID )
{
	T_IM_ELA_CTRL_EACH get_ctrl_each;
	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Get_EAch( &get_ctrl_each );

	if( memcmp( &get_ctrl_each, &g_ctrl_each, sizeof(T_IM_ELA_CTRL_EACH) ) == 0 ){
		Ddim_Print(("TEST OK!\n"));
	}
	else{
		Ddim_Print(("TEST NG!\n"));
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_1_20( VOID )
{
	T_IM_ELA_EACH_AXI each_axi;
	UINT32 err_f = 0;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	ct_im_ela_on_pclk();
	IO_ELA.EARAXSTS.bit.RRESP_0 = 1;
	IO_ELA.EARAXSTS.bit.RRESP_1 = 2;
	IO_ELA.EARAXSTS.bit.RRESP_2 = 3;

	IO_ELA.EAWAXSTS.bit.BRESP_0 = 3;
	IO_ELA.EAWAXSTS.bit.BRESP_1 = 2;
	IO_ELA.EAWAXSTS.bit.BRESP_2 = 1;
	ct_im_ela_off_pclk();

	Im_ELA_Get_AXI_Status( &each_axi );

	ct_im_ela_on_pclk();
	if( each_axi.axi_reply_r[0] != IO_ELA.EARAXSTS.bit.RRESP_0 ){
		Ddim_Print(("TEST NG : (RRESP_0) %d != %d\n", each_axi.axi_reply_r[0], IO_ELA.EARAXSTS.bit.RRESP_0));
		err_f = 1;
	}
	if( each_axi.axi_reply_r[1] != IO_ELA.EARAXSTS.bit.RRESP_1 ){
		Ddim_Print(("TEST NG : (RRESP_1) %d != %d\n", each_axi.axi_reply_r[1], IO_ELA.EARAXSTS.bit.RRESP_1));
		err_f = 1;
	}
	if( each_axi.axi_reply_r[2] != IO_ELA.EARAXSTS.bit.RRESP_2 ){
		Ddim_Print(("TEST NG : (RRESP_2) %d != %d\n", each_axi.axi_reply_r[2], IO_ELA.EARAXSTS.bit.RRESP_2));
		err_f = 1;
	}
	if( each_axi.axi_reply_w[0] != IO_ELA.EAWAXSTS.bit.BRESP_0 ){
		Ddim_Print(("TEST NG : (BRESP_0) %d != %d\n", each_axi.axi_reply_w[0], IO_ELA.EAWAXSTS.bit.BRESP_0));
		err_f = 1;
	}
	if( each_axi.axi_reply_w[1] != IO_ELA.EAWAXSTS.bit.BRESP_1 ){
		Ddim_Print(("TEST NG : (BRESP_1) %d != %d\n", each_axi.axi_reply_w[1], IO_ELA.EAWAXSTS.bit.BRESP_1));
		err_f = 1;
	}
	if( each_axi.axi_reply_w[2] != IO_ELA.EAWAXSTS.bit.BRESP_2 ){
		Ddim_Print(("TEST NG : (BRESP_2) %d != %d\n", each_axi.axi_reply_w[2], IO_ELA.EAWAXSTS.bit.BRESP_2));
		err_f = 1;
	}
	if( !err_f ){
		Ddim_Print(("TEST OK!\n"));
	}
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_1_21( VOID )
{
	UINT32 result;
	UINT32	i;
	USHORT src_tbl[D_IM_ELA_DEKNEE_TBL_MAX];

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	IO_ELA.ELATRG.bit.ELATRG = 0;
	Im_ELA_Open( 100 );

	for( i = 0; i < D_IM_ELA_DEKNEE_TBL_MAX; i++ ){
		src_tbl[i]=i;
	}

	result = Im_ELA_Set_Deknee_Tbl( &src_tbl[0], 0, D_IM_ELA_DEKNEE_TBL_MAX );
	Ddim_Print(( "Im_ELA_Set_Deknee_Tbl = %d\n", result ));

	ct_im_ela_on_pclk();
	IO_ELA.ELABYR.bit.DKNACT = 1;
	for( i = 0; i < (D_IM_ELA_DEKNEE_TBL_MAX / 2); i++ ){
		Ddim_Print(( "DKNTBL[%03d] = 0x%08lx\n",i ,IO_ELA.DKNTBL.word[i] ));
	}
	IO_ELA.ELABYR.bit.DKNACT = 0;
	ct_im_ela_off_pclk();

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_1_22( VOID )
{
	UINT32 i, j;
	UINT32 result;
	T_IM_ELA_CURTAIL_BAYER_DATA curtail_bayer[D_IM_ELA_STAGE_MAX];
	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		for( j = 0; j < D_IM_ELA_STEP_MAX; j++ ){
			curtail_bayer[i].bayer_addr[j] = 0x10000000 + (0x100 * i) + j;
		}
		curtail_bayer[i].gl_hbyte = i + 100;
		curtail_bayer[i].hsize    = i + 200;
		curtail_bayer[i].vline    = i + 300;
	}

	result = Im_ELA_Set_Curtail_Bayer_Data_Tbl( curtail_bayer );
	Ddim_Print(("result=0x%08X\n", result));

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_1_23( VOID )
{
	UINT32 i, j;
	T_IM_ELA_CURTAIL_BAYER_DATA curtail_bayer[D_IM_ELA_STAGE_MAX];

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Get_Curtail_Bayer_Data_Tbl( curtail_bayer );

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		Ddim_Print(( "(((Stage%d)))\n", i + 1 ));
		Ddim_Print(( "gl_hbyte = %d\n",	curtail_bayer[i].gl_hbyte ));
		Ddim_Print(( "hsize = %d\n",	curtail_bayer[i].hsize ));
		Ddim_Print(( "vline = %d\n",	curtail_bayer[i].vline ));
		for( j = 0; j < D_IM_ELA_STEP_MAX; j++ ){
			Ddim_Print(( "(Sterp%02d)bayer_addr = 0x%08X\n", j, curtail_bayer[i].bayer_addr[j] ));
			DDIM_User_Dly_Tsk(10);
		}
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_1_24( VOID )
{
	UINT32 i, j;
	UINT32 result;
	T_IM_ELA_EXTRACT_NOISE_DATA extract_noise[D_IM_ELA_STAGE_MAX];

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		for( j = 0; j < D_IM_ELA_STEP_MAX; j++ ){
			extract_noise[i].noise_data_addr[j] = 0x20000000 + (0x100 * i) + j;
		}
		extract_noise[i].gl_hbyte = i + 100;
		extract_noise[i].hsize    = i + 200;
		extract_noise[i].vline    = i + 300;
	}

	result = Im_ELA_Set_Extract_Noise_Data_Tbl( extract_noise );
	Ddim_Print(("result=0x%08X\n", result));

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_1_25( VOID )
{
	UINT32 i, j;
	T_IM_ELA_EXTRACT_NOISE_DATA extract_noise[D_IM_ELA_STAGE_MAX];

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Get_Extract_Noise_Data_Tbl( extract_noise );

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		Ddim_Print(( "(((Stage%d)))\n", i + 1 ));
		Ddim_Print(( "gl_hbyte = %d\n",	extract_noise[i].gl_hbyte ));
		Ddim_Print(( "hsize = %d\n",	extract_noise[i].hsize ));
		Ddim_Print(( "vline = %d\n",	extract_noise[i].vline ));
		for( j = 0; j < D_IM_ELA_STEP_MAX; j++ ){
			Ddim_Print(( "(Sterp%02d)noise_data_addr = 0x%08X\n", j, extract_noise[i].noise_data_addr[j] ));
			DDIM_User_Dly_Tsk(10);
		}
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}



VOID CT_Im_Ela_2_01( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "<<< ELA Register Initial value >>>\n" ));
	CT_Im_Ela_Reg_Print();
	Ddim_Print(( "\n" ));
#endif

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );
#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "T_IM_ELA_CTRL Test Data Set OK\n" ));
#endif

//	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 8896*2;
//	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 8896;
//	ctrl_ela.ctrl_eirch.in_bayer_vline       = 4976;

	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6048;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_12M_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_12M_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_0;

//	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_16BIT;
	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_REDUCTION;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_2;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data(&multiple_exec_info);
	multiple_exec_info.core[0]->limit.noise_plus_limit = 0x0277;
	multiple_exec_info.core[0]->limit.noise_minus_limit = 0x0277;
	multiple_exec_info.core[0]->zero_point_level = 0x00C7;
	multiple_exec_info.core[0]->anb_r.offset[E_IM_ELA_ANB_1] = 0x0058;
	multiple_exec_info.core[0]->anb_r.offset[E_IM_ELA_ANB_2] = 0x0093;
	multiple_exec_info.core[0]->anb_r.offset[E_IM_ELA_ANB_3] = 0x00c5;
	multiple_exec_info.core[0]->anb_r.offset[E_IM_ELA_ANB_4] = 0x0103;
	multiple_exec_info.core[0]->anb_r.gain[E_IM_ELA_ANB_1] = 0x0273;
	multiple_exec_info.core[0]->anb_r.gain[E_IM_ELA_ANB_2] = 0x0215;
	multiple_exec_info.core[0]->anb_r.gain[E_IM_ELA_ANB_3] = 0x0149;
	multiple_exec_info.core[0]->anb_r.gain[E_IM_ELA_ANB_4] = 0x00f5;
	multiple_exec_info.core[0]->anb_r.boundary[E_IM_ELA_ANB_1] = 0x0000;
	multiple_exec_info.core[0]->anb_r.boundary[E_IM_ELA_ANB_2] = 0x0180;
	multiple_exec_info.core[0]->anb_r.boundary[E_IM_ELA_ANB_3] = 0x0300;
	multiple_exec_info.core[0]->anb_r.boundary[E_IM_ELA_ANB_4] = 0x0600;
	multiple_exec_info.core[0]->anb_g.offset[E_IM_ELA_ANB_1] = 0x0058;
	multiple_exec_info.core[0]->anb_g.offset[E_IM_ELA_ANB_2] = 0x0093;
	multiple_exec_info.core[0]->anb_g.offset[E_IM_ELA_ANB_3] = 0x00c5;
	multiple_exec_info.core[0]->anb_g.offset[E_IM_ELA_ANB_4] = 0x0103;
	multiple_exec_info.core[0]->anb_g.gain[E_IM_ELA_ANB_1] = 0x0273;
	multiple_exec_info.core[0]->anb_g.gain[E_IM_ELA_ANB_2] = 0x0215;
	multiple_exec_info.core[0]->anb_g.gain[E_IM_ELA_ANB_3] = 0x0149;
	multiple_exec_info.core[0]->anb_g.gain[E_IM_ELA_ANB_4] = 0x00f5;
	multiple_exec_info.core[0]->anb_g.boundary[E_IM_ELA_ANB_1] = 0x0000;
	multiple_exec_info.core[0]->anb_g.boundary[E_IM_ELA_ANB_2] = 0x0180;
	multiple_exec_info.core[0]->anb_g.boundary[E_IM_ELA_ANB_3] = 0x0300;
	multiple_exec_info.core[0]->anb_g.boundary[E_IM_ELA_ANB_4] = 0x0600;
	multiple_exec_info.core[0]->anb_b.offset[E_IM_ELA_ANB_1] = 0x0058;
	multiple_exec_info.core[0]->anb_b.offset[E_IM_ELA_ANB_2] = 0x0093;
	multiple_exec_info.core[0]->anb_b.offset[E_IM_ELA_ANB_3] = 0x00c5;
	multiple_exec_info.core[0]->anb_b.offset[E_IM_ELA_ANB_4] = 0x0103;
	multiple_exec_info.core[0]->anb_b.gain[E_IM_ELA_ANB_1] = 0x0273;
	multiple_exec_info.core[0]->anb_b.gain[E_IM_ELA_ANB_2] = 0x0215;
	multiple_exec_info.core[0]->anb_b.gain[E_IM_ELA_ANB_3] = 0x0149;
	multiple_exec_info.core[0]->anb_b.gain[E_IM_ELA_ANB_4] = 0x00f5;
	multiple_exec_info.core[0]->anb_b.boundary[E_IM_ELA_ANB_1] = 0x0000;
	multiple_exec_info.core[0]->anb_b.boundary[E_IM_ELA_ANB_2] = 0x0180;
	multiple_exec_info.core[0]->anb_b.boundary[E_IM_ELA_ANB_3] = 0x0300;
	multiple_exec_info.core[0]->anb_b.boundary[E_IM_ELA_ANB_4] = 0x0600;
	multiple_exec_info.core[0]->edge_adjust_a1 = 0x008a;
	multiple_exec_info.core[0]->edge_adjust_a2 = 0x0083;
	multiple_exec_info.core[0]->edge_adjust_b1 = 0x02fc;
	multiple_exec_info.core[0]->edge_adjust_b2 = 0x0033;
	multiple_exec_info.core[0]->edge_threshold[E_IM_ELA_ALNGE_1] = 0x008e;
	multiple_exec_info.core[0]->edge_threshold[E_IM_ELA_ALNGE_2] = 0x00d5;
	multiple_exec_info.core[0]->edge_threshold[E_IM_ELA_ALNGE_3] = 0x011c;
	multiple_exec_info.core[0]->nr[E_IM_ELA_ALNG_L1E1N] = 0x00b4;
	multiple_exec_info.core[0]->nr[E_IM_ELA_ALNG_L1E2N] = 0x0082;
	multiple_exec_info.core[0]->nr[E_IM_ELA_ALNG_L1E3N] = 0x0000;
	multiple_exec_info.core[0]->nr[E_IM_ELA_ALNG_L2E1N] = 0x00a0;
	multiple_exec_info.core[0]->nr[E_IM_ELA_ALNG_L2E2N] = 0x0069;
	multiple_exec_info.core[0]->nr[E_IM_ELA_ALNG_L2E3N] = 0x0000;
	multiple_exec_info.core[0]->nr[E_IM_ELA_ALNG_L3E1N] = 0x008c;
	multiple_exec_info.core[0]->nr[E_IM_ELA_ALNG_L3E2N] = 0x0050;
	multiple_exec_info.core[0]->nr[E_IM_ELA_ALNG_L3E3N] = 0x0000;
	multiple_exec_info.core[0]->nr[E_IM_ELA_ALNG_L4E1N] = 0x0078;
	multiple_exec_info.core[0]->nr[E_IM_ELA_ALNG_L4E2N] = 0x003c;
	multiple_exec_info.core[0]->nr[E_IM_ELA_ALNG_L4E3N] = 0x0000;
	multiple_exec_info.core[0]->l_noise = 0x00c8;
	multiple_exec_info.core[0]->l_edge[E_IM_ELA_LCOMEN_1] = 0x008e;
	multiple_exec_info.core[0]->l_edge[E_IM_ELA_LCOMEN_2] = 0x011c;
	multiple_exec_info.core[0]->l_intense[E_IM_ELA_LCOMEN_1] = 0x0000;
	multiple_exec_info.core[0]->l_intense[E_IM_ELA_LCOMEN_2] = 0x00cd;

#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "T_IM_ELA_MULTIPLE_TIMES_INFO Test Data Set OK\n" ));
#endif

	Im_ELA_Init();

	while( 1 ){
		// Setting of ELA control parameter
		result = Im_ELA_Ctrl( &ctrl_ela );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Ctrl OK\n" ));
#endif

		result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Set_Multiple_Exec_Info OK\n" ));
#endif

		// Processing is started synchronously
		result = Im_ELA_Start_Sync();
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Start_Sync OK\n" ));
#endif
		break;
	}

	ct_im_ela_clk_chk();

#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "<<< ELA Register >>>\n" ));
	CT_Im_Ela_Reg_Print();
#endif
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_02( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	UINT32 i;
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_0;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_1;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_REDUCTION;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data(&multiple_exec_info);

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_03( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_0;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_2;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_REDUCTION;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data(&multiple_exec_info);

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_04( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );
/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_4;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_0;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_3;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_REDUCTION;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data(&multiple_exec_info);

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_2_05( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_0;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_3;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_REDUCTION;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data(&multiple_exec_info);

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_06( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/

	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_0;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_0;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_0;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_0;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_0;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_REDUCTION;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data(&multiple_exec_info);

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_2_07( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_0;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_0;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_1;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_REDUCTION;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data(&multiple_exec_info);

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_08( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_2;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data(&multiple_exec_info);

	Im_ELA_Init();

	while( 1 ){
		// Setting of ELA control parameter
		result = Im_ELA_Ctrl( &ctrl_ela );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Ctrl OK\n" ));
#endif

		result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Set_Multiple_Exec_Info OK\n" ));
#endif

		// Processing is started synchronously
		result = Im_ELA_Start_Sync();
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Start_Sync OK\n" ));
#endif
		break;
	}

	ct_im_ela_clk_chk();

#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "<<< ELA Register >>>\n" ));
	CT_Im_Ela_Reg_Print();
#endif
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_08_a( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 4224;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 2796;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 1560;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_2;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_DISABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data(&multiple_exec_info);

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();

#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "<<< ELA Register >>>\n" ));
	CT_Im_Ela_Reg_Print();
#endif
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_09( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_2;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data(&multiple_exec_info);

	Im_ELA_Init();

	while( 1 ){
		// Setting of ELA control parameter
		result = Im_ELA_Ctrl( &ctrl_ela );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Ctrl OK\n" ));
#endif

		result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Set_Multiple_Exec_Info OK\n" ));
#endif

		// Processing is started synchronously
		result = Im_ELA_Start_Sync();
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Start_Sync OK\n" ));
#endif
		break;
	}
	ct_im_ela_clk_chk();

#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "<<< ELA Register >>>\n" ));
	CT_Im_Ela_Reg_Print();
#endif
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_10( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );
	
/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_3;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_2;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data(&multiple_exec_info);

	Im_ELA_Init();

	while( 1 ){
		// Setting of ELA control parameter
		result = Im_ELA_Ctrl( &ctrl_ela );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Ctrl OK\n" ));
#endif

		result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Set_Multiple_Exec_Info OK\n" ));
#endif

		// Processing is started synchronously
		result = Im_ELA_Start_Sync();
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Start_Sync OK\n" ));
#endif
		break;
	}

	ct_im_ela_clk_chk();

#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "<<< ELA Register >>>\n" ));
	CT_Im_Ela_Reg_Print();
#endif
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_11( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_4;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_3;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_2;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data(&multiple_exec_info);

	Im_ELA_Init();

	while( 1 ){
		// Setting of ELA control parameter
		result = Im_ELA_Ctrl( &ctrl_ela );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Ctrl OK\n" ));
#endif

		result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Set_Multiple_Exec_Info OK\n" ));
#endif

		// Processing is started synchronously
		result = Im_ELA_Start_Sync();
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Start_Sync OK\n" ));
#endif
		break;
	}

	ct_im_ela_clk_chk();

#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "<<< ELA Register >>>\n" ));
	CT_Im_Ela_Reg_Print();
#endif
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_12( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );
	ctrl_ela.next_ctrl_flag = D_IM_ELA_DISABLE;
#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "T_IM_ELA_CTRL Test Data Set OK\n" ));
#endif

//	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6656;
//	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 4424;
//	ctrl_ela.ctrl_eirch.in_bayer_vline       = 2488;

	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_3;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data( &multiple_exec_info );
#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "T_IM_ELA_MULTIPLE_TIMES_INFO Test Data Set OK\n" ));
#endif

	Im_ELA_Init();

	while( 1 ){
		// Setting of ELA control parameter
		result = Im_ELA_Ctrl( &ctrl_ela );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Ctrl OK\n" ));
#endif

		result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Set_Multiple_Exec_Info OK\n" ));
#endif

		// Processing is started synchronously
		result = Im_ELA_Start_Sync();
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Start_Sync OK\n" ));
#endif
		break;
	}
//	Im_ELA_Stop();

	ct_im_ela_clk_chk();

#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "<<< ELA Register >>>\n" ));
	CT_Im_Ela_Reg_Print();
#endif
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_13( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_1;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data(&multiple_exec_info);

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();

	Im_ELA_Close();
}


VOID CT_Im_Ela_2_14( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_3;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data( &multiple_exec_info );

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_2_15( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

	ctrl_ela.next_ctrl_flag = D_IM_ELA_ENABLE;

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data( &multiple_exec_info );

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG(1) : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Setting of ELA control parameter
	// The last ELA macro processing
	result = Im_ELA_Ctrl_Next();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl_Next NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG(2) : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_2_16( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 640;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_3;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_8BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_8BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data( &multiple_exec_info );

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_17( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_3;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_8BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data( &multiple_exec_info );

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_18( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 1280;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_3;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_16BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_L4;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_R4;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data( &multiple_exec_info );

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_19( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_3;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.deknee_enable       = D_IM_ELA_DISABLE;
	ctrl_ela.ctrl_byr.ob_zsf_enable       = D_IM_ELA_DISABLE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_2;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data( &multiple_exec_info );

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]->optical_zero_level_shift = 0x07FF;
		multiple_exec_info.core[i]->limit.noise_plus_limit = 0x0277;
		multiple_exec_info.core[i]->limit.noise_minus_limit = 0x0277;
	}

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_20( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_3;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x7FF;
	ctrl_ela.ob_correction.ob_bb = 0x7FF;
	ctrl_ela.ob_correction.ob_gr = 0x7FF;
	ctrl_ela.ob_correction.ob_gb = 0x7FF;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data( &multiple_exec_info );
	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]->optical_zero_level_shift = 0x07FF;
		multiple_exec_info.core[i]->limit.noise_plus_limit = 0x0277;
		multiple_exec_info.core[i]->limit.noise_minus_limit = 0x0277;
	}

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_21( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

/*
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 960;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;
*/
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_3;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_2;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x7FF;
	ctrl_ela.ob_correction.ob_bb = 0x7FF;
	ctrl_ela.ob_correction.ob_gr = 0x7FF;
	ctrl_ela.ob_correction.ob_gb = 0x7FF;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data( &multiple_exec_info );
	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]->optical_zero_level_shift = 0x07FF;
		multiple_exec_info.core[i]->limit.noise_plus_limit = 0x0277;
		multiple_exec_info.core[i]->limit.noise_minus_limit = 0x0277;
	}

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}


VOID CT_Im_Ela_2_22( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );
	ctrl_ela.next_ctrl_flag = D_IM_ELA_DISABLE;
#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "T_IM_ELA_CTRL Test Data Set OK\n" ));
#endif
	
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

//	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 640;
//	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 640;
//	ctrl_ela.ctrl_eirch.in_bayer_vline       = 480;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;

	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_8BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;
    ctrl_ela.ctrl_byr.deknee_enable       = D_IM_ELA_DISABLE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_2;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x7FF;
	ctrl_ela.ob_correction.ob_bb = 0x7FF;
	ctrl_ela.ob_correction.ob_gr = 0x7FF;
	ctrl_ela.ob_correction.ob_gb = 0x7FF;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data( &multiple_exec_info );

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]->optical_zero_level_shift = 0x07FF;
		multiple_exec_info.core[i]->limit.noise_plus_limit = 0x0277;
		multiple_exec_info.core[i]->limit.noise_minus_limit = 0x0277;
	}

#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "T_IM_ELA_MULTIPLE_TIMES_INFO Test Data Set OK\n" ));
#endif

	Im_ELA_Init();

	while(1){
		// Setting of ELA control parameter
		result = Im_ELA_Ctrl(&ctrl_ela );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Ctrl OK\n" ));
#endif

		result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Set_Multiple_Exec_Info OK\n" ));
#endif

		// Processing is started synchronously
		result = Im_ELA_Start_Sync();
		if( result != D_DDIM_OK ){
			// Error processing
			Ddim_Print(("E:Im_ELA_Start_Sync NG : result=%d.\n", result));
			break;
		}
#ifdef CT_IM_ELA_MSG_PRINT_ON
		Ddim_Print(( "Im_ELA_Start_Sync OK\n" ));
#endif
		break;
	}
//	Im_ELA_Stop();

	ct_im_ela_clk_chk();

#ifdef CT_IM_ELA_MSG_PRINT_ON
	Ddim_Print(( "<<< ELA Register >>>\n" ));
	CT_Im_Ela_Reg_Print();
#endif
	Im_ELA_Close();
}

VOID CT_Im_Ela_2_23( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

//	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 8896*2;
//	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 8896;
//	ctrl_ela.ctrl_eirch.in_bayer_vline       = 4976;
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_5;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_4] = E_IM_ELA_DENSITY_MODE_3;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_5] = E_IM_ELA_DENSITY_MODE_3;

//	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_16BIT;
	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data( &multiple_exec_info );

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Async();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Async NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}
	result = Im_ELA_Wait_End();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Wait_End NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}
	Im_ELA_Stop();

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_2_24( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};
	UINT32 i;
	T_IM_ELA_CTRL_CORE	core[D_IM_ELA_STAGE_MAX] = {{0}};
	T_IM_ELA_MULTIPLE_TIMES_INFO multiple_exec_info = {{0}};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Open( 100 );

	Im_ELA_Init();

	for( i = 0; i < D_IM_ELA_STAGE_MAX; i++ ){
		multiple_exec_info.core[i]  = &core[i];
	}

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

	ctrl_ela.next_ctrl_flag = D_IM_ELA_ENABLE;

//	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 8896*2;
//	ctrl_ela.ctrl_eirch.in_bayer_hsize       = 8896;
//	ctrl_ela.ctrl_eirch.in_bayer_vline       = 4976;
	ctrl_ela.ctrl_eirch.in_bayer_total_hsize = 6672;
	ctrl_ela.ctrl_eirch.in_bayer_hsize       = S_ELA_BAYER_4K2K_WIDTH;
	ctrl_ela.ctrl_eirch.in_bayer_vline       = S_ELA_BAYER_4K2K_LINES;

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_2;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_2;

//	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_16BIT;
	ctrl_ela.ctrl_byr.in_data_type        = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.in_data_shift_mode  = D_IM_ELA_BIT_SHIFT_MODE_NONE;
	ctrl_ela.ctrl_byr.out_data_type       = D_IM_ELA_DATATYP_12BIT;
	ctrl_ela.ctrl_byr.out_data_shift_mode = D_IM_ELA_BIT_SHIFT_MODE_NONE;

	ctrl_ela.ctrl_common.noise_data_mode            = D_IM_ELA_NSLMD_SAME;
	ctrl_ela.ctrl_common.noise_data_precision       = D_IM_ELA_NSLRES_PRECISION_1;
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;

	ctrl_ela.ob_correction.ob_rr = 0x000;
	ctrl_ela.ob_correction.ob_bb = 0x000;
	ctrl_ela.ob_correction.ob_gr = 0x000;
	ctrl_ela.ob_correction.ob_gb = 0x000;

	ctrl_ela.ctrl_common.bayer_start_pixel_type = D_IM_ELA_ORG_R;

	ct_im_ela_multiple_set_test_data( &multiple_exec_info );

	Im_ELA_Init();

	// Setting of ELA control parameter
	result = Im_ELA_Ctrl( &ctrl_ela );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	result = Im_ELA_Set_Multiple_Exec_Info( &multiple_exec_info );
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Set_Multiple_Exec_Info NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Async();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Async NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}
	result = Im_ELA_Wait_End();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Wait_End NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Setting of ELA control parameter
	// The last ELA macro processing
	result = Im_ELA_Ctrl_Next();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Ctrl_Next NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}

	// Processing is started synchronously
	result = Im_ELA_Start_Async();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Start_Async NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}
	result = Im_ELA_Wait_End();
	if( result != D_DDIM_OK ){
		// Error processing
		Ddim_Print(("E:Im_ELA_Wait_End NG : result=%d.\n", result));
		Im_ELA_Close();
		return;
	}
	Im_ELA_Stop();

	ct_im_ela_clk_chk();
	Im_ELA_Close();
}

VOID CT_Im_Ela_3_01( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Open( -2 );

	Ddim_Print(("result=0x%08X\n", result));
}

VOID CT_Im_Ela_3_02( VOID )
{
	INT32 im_ercd;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	im_ercd = Im_ELA_Open( 100 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(("Open error = 0x%x\n", im_ercd ));
		return ;
	}

	im_ercd = Im_ELA_Open( 100 );
	if( im_ercd != D_IM_ELA_SEM_TIMEOUT_ERR ){
		Ddim_Print(( "result = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_ELA_Close();
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( "Close error = 0x%x\n", im_ercd ));
		return ;
	}
}

VOID CT_Im_Ela_3_03( VOID )
{
	INT32 im_ercd;
	DDIM_USER_ID sid_back;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	sid_back = SID_IM_ELA;
	SID_IM_ELA = 0xfff;

	im_ercd = Im_ELA_Open( 100 );
	if( im_ercd != D_IM_ELA_SEM_NG ){
		Ddim_Print(( "Open error = 0x%x\n", im_ercd ));
		return ;
	}

	SID_IM_ELA = sid_back;
}

VOID CT_Im_Ela_3_04( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Ctrl( NULL );

	Ddim_Print(("result=0x%08X\n", result));
}

VOID CT_Im_Ela_3_05( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

	ctrl_ela.ela_elf_level = E_IM_ELA_ELF_LEVEL_MAX;

	result = Im_ELA_Ctrl( &ctrl_ela );

	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_06( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_1] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_2] = E_IM_ELA_DENSITY_MODE_1;
	ctrl_ela.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] = E_IM_ELA_DENSITY_MODE_3;

	result = Im_ELA_Ctrl( &ctrl_ela );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_07( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

	Ddim_Print(( "(1)\n"));
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;
	ctrl_ela.ctrl_common.noise_data_precision = D_IM_ELA_NSLRES_PRECISION_3;

	result = Im_ELA_Ctrl( &ctrl_ela );
	Ddim_Print(("result=0x%08X\n", result));

	Ddim_Print(( "(2)\n"));
	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;
	ctrl_ela.ctrl_common.noise_data_precision = D_IM_ELA_NSLRES_PRECISION_4;

	result = Im_ELA_Ctrl( &ctrl_ela );
	Ddim_Print(("result=0x%08X\n", result));

	return;
}


VOID CT_Im_Ela_3_08( VOID )
{
	INT32 result;
	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Get_Ctrl_Common( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_09( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Get_Ctrl_Eirch( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_10( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Get_Ctrl_Eiwch( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_11( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Get_Ctrl_Enwch( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_12( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Get_Ctrl_Enrech( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_13( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Get_Ctrl_Enrach_Enwmch( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_14( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Get_Ctrl_Core( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_15( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Get_Ctrl_Ob_Correction( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_16( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Set_Multiple_Exec_Info( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_17( VOID )
{
	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	Im_ELA_Set_Deknee_Lut_Sleep( 2 );
	Ddim_Print(( "<%s> END\n", __FUNCTION__ ));
}


VOID CT_Im_Ela_3_18( VOID )
{
	INT32 result;
	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Get_Nslmd( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_19( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Set_EAch( NULL );

	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_20( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Get_EAch( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_21( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Get_AXI_Status( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_22( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Set_Deknee_Tbl( NULL, 0, 256 );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_23( VOID )
{
	INT32 result;
	USHORT src_tbl[256];

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Set_Deknee_Tbl( &src_tbl[0], 1, 256 );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_24( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Set_Curtail_Bayer_Data_Tbl( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_25( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Get_Curtail_Bayer_Data_Tbl( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_26( VOID )
{
	INT32 result;

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Set_Extract_Noise_Data_Tbl( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_27( VOID )
{
	INT32 result;
	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	result = Im_ELA_Get_Extract_Noise_Data_Tbl( NULL );
	Ddim_Print(("result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_28( VOID )
{
	INT32 result;
	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

#ifdef CO_DEBUG_ON_PC
	IO_ELA.ELATRG.bit.ELATRG = 3;
#endif	//CO_DEBUG_ON_PC
	// Processing is started synchronously
	result = Im_ELA_Start_Sync();
	Ddim_Print(("Im_ELA_Start_Sync : result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_29( VOID )
{
	INT32 result;
	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

#ifdef CO_DEBUG_ON_PC
	IO_ELA.ELATRG.bit.ELATRG = 3;
#endif	//CO_DEBUG_ON_PC
	// Processing is started asynchronously
	result = Im_ELA_Start_Async();
	Ddim_Print(("Im_ELA_Start_Aync : result=0x%08X\n", result));
}

VOID CT_Im_Ela_3_30( VOID )
{
	INT32 result;
	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	// wait for the process to finish
	result = Im_ELA_Wait_End();
	Ddim_Print(("Im_ELA_Wait_End : result=0x%08X\n", result));
}


VOID CT_Im_Ela_3_31( VOID )
{
	INT32 result;
	T_IM_ELA_CTRL	ctrl_ela = {0};

	Ddim_Print(( "<%s>\n", __FUNCTION__ ));

	IO_ELA.ELATRG.bit.ELATRG = 0;

	ct_im_ela_ctrl_set_test_data( &ctrl_ela );

	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;	
	ctrl_ela.ctrl_common.noise_data_precision = D_IM_ELA_NSLRES_PRECISION_1;

	result = Im_ELA_Ctrl( &ctrl_ela );
	Ddim_Print(("result=0x%08X\n", result));

	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;	
	ctrl_ela.ctrl_common.noise_data_precision = D_IM_ELA_NSLRES_PRECISION_2;

	result = Im_ELA_Ctrl( &ctrl_ela );
	Ddim_Print(("result=0x%08X\n", result));

	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;	
	ctrl_ela.ctrl_common.noise_data_precision = D_IM_ELA_NSLRES_PRECISION_3;

	result = Im_ELA_Ctrl( &ctrl_ela );
	Ddim_Print(("result=0x%08X\n", result));

	ctrl_ela.ctrl_common.noise_data_compress_enable = D_IM_ELA_ENABLE;	
	ctrl_ela.ctrl_common.noise_data_precision = D_IM_ELA_NSLRES_PRECISION_4;

	result = Im_ELA_Ctrl( &ctrl_ela );
	Ddim_Print(("result=0x%08X\n", result));
}


/**
 * @brief       Command processing of ELA.
 * @param[in]   INT32 argc
 * @param[in]   CHAR** argv
 * @return      None
 * @note        Command processing of ELA.
 * @attention   None
 */
VOID Ct_Im_Ela_Main(INT32 argc, CHAR** argv)
{
//	Dd_MMU_Clean_Flush_Dcache_All();
	DDIM_User_L1l2cache_Clean_Flush_All();

	if (strcmp((const char *)argv[1], "ct") == 0){
		if(atoi((const char *)argv[2]) == 1){
			if(atoi((const char *)argv[3]) == 1){
				CT_Im_Ela_1_01();
			}
			else if(atoi((const char *)argv[3]) == 2){
				CT_Im_Ela_1_02();
			}
			else if(atoi((const char *)argv[3]) == 3){
				CT_Im_Ela_1_03();
			}
			else if(atoi((const char *)argv[3]) == 4){
				CT_Im_Ela_1_04();
			}
			else if(atoi((const char *)argv[3]) == 5){
				CT_Im_Ela_1_05();
			}
			else if(atoi((const char *)argv[3]) == 6){
				CT_Im_Ela_1_06();
			}
			else if(atoi((const char *)argv[3]) == 7){
				CT_Im_Ela_1_07();
			}
			else if(atoi((const char *)argv[3]) == 8){
				CT_Im_Ela_1_08();
			}
			else if(atoi((const char *)argv[3]) == 9){
				CT_Im_Ela_1_09();
			}
			else if(atoi((const char *)argv[3]) == 10){
				CT_Im_Ela_1_10();
			}
			else if(atoi((const char *)argv[3]) == 11){
				CT_Im_Ela_1_11();
			}
			else if(atoi((const char *)argv[3]) == 12){
				CT_Im_Ela_1_12();
			}
			else if(atoi((const char *)argv[3]) == 13){
				CT_Im_Ela_1_13();
			}
			else if(atoi((const char *)argv[3]) == 14){
				CT_Im_Ela_1_14();
			}
			else if(atoi((const char *)argv[3]) == 15){
				CT_Im_Ela_1_15();
			}
			else if(atoi((const char *)argv[3]) == 16){
				CT_Im_Ela_1_16();
			}
			else if(atoi((const char *)argv[3]) == 17){
				CT_Im_Ela_1_17();
			}
			else if(atoi((const char *)argv[3]) == 18){
				CT_Im_Ela_1_18();
			}
			else if(atoi((const char *)argv[3]) == 19){
				CT_Im_Ela_1_19();
			}
			else if(atoi((const char *)argv[3]) == 20){
				CT_Im_Ela_1_20();
			}
			else if(atoi((const char *)argv[3]) == 21){
				CT_Im_Ela_1_21();
			}
			else if(atoi((const char *)argv[3]) == 22){
				CT_Im_Ela_1_22();
			}
			else if(atoi((const char *)argv[3]) == 23){
				CT_Im_Ela_1_23();
			}
			else if(atoi((const char *)argv[3]) == 24){
				CT_Im_Ela_1_24();
			}
			else if(atoi((const char *)argv[3]) == 25){
				CT_Im_Ela_1_25();
			}
			else {
				Ddim_Print(("input parameter error\n"));
			}
		}
		else if(atoi((const char *)argv[2]) == 2){
			if(atoi((const char *)argv[3]) == 1){
				CT_Im_Ela_2_01();
			}
			else if(atoi((const char *)argv[3]) == 2){
				CT_Im_Ela_2_02();
			}
			else if(atoi((const char *)argv[3]) == 3){
				CT_Im_Ela_2_03();
			}
			else if(atoi((const char *)argv[3]) == 4){
				CT_Im_Ela_2_04();
			}
			else if(atoi((const char *)argv[3]) == 5){
				CT_Im_Ela_2_05();
			}
			else if(atoi((const char *)argv[3]) == 6){
				CT_Im_Ela_2_06();
			}
			else if(atoi((const char *)argv[3]) == 7){
				CT_Im_Ela_2_07();
			}
			else if(atoi((const char *)argv[3]) == 8){
				CT_Im_Ela_2_08();
			}
			else if(atoi((const char *)argv[3]) == 9){
				CT_Im_Ela_2_09();
			}
			else if(atoi((const char *)argv[3]) == 10){
				CT_Im_Ela_2_10();
			}
			else if(atoi((const char *)argv[3]) == 11){
				CT_Im_Ela_2_11();
			}
			else if(atoi((const char *)argv[3]) == 12){
				CT_Im_Ela_2_12();
			}
			else if(atoi((const char *)argv[3]) == 13){
				CT_Im_Ela_2_13();
			}
			else if(atoi((const char *)argv[3]) == 14){
				CT_Im_Ela_2_14();
			}
			else if(atoi((const char *)argv[3]) == 15){
				CT_Im_Ela_2_15();
			}
			else if(atoi((const char *)argv[3]) == 16){
				CT_Im_Ela_2_16();
			}
			else if(atoi((const char *)argv[3]) == 17){
				CT_Im_Ela_2_17();
			}
			else if(atoi((const char *)argv[3]) == 18){
				CT_Im_Ela_2_18();
			}
			else if(atoi((const char *)argv[3]) == 19){
				CT_Im_Ela_2_19();
			}
			else if(atoi((const char *)argv[3]) == 20){
				CT_Im_Ela_2_20();
			}
			else if(atoi((const char *)argv[3]) == 21){
				CT_Im_Ela_2_21();
			}
			else if(atoi((const char *)argv[3]) == 22){
				CT_Im_Ela_2_22();
			}
			else if(atoi((const char *)argv[3]) == 23){
				CT_Im_Ela_2_23();
			}
			else if(atoi((const char *)argv[3]) == 24){
				CT_Im_Ela_2_24();
			}
			else {
				Ddim_Print(("input parameter error\n"));
			}
		}
		else if(atoi((const char *)argv[2]) == 3){
			if(atoi((const char *)argv[3]) == 1){
				CT_Im_Ela_3_01();
			}
			else if(atoi((const char *)argv[3]) == 2){
				CT_Im_Ela_3_02();
			}
			else if(atoi((const char *)argv[3]) == 3){
				CT_Im_Ela_3_03();
			}
			else if(atoi((const char *)argv[3]) == 4){
				CT_Im_Ela_3_04();
			}
			else if(atoi((const char *)argv[3]) == 5){
				CT_Im_Ela_3_05();
			}
			else if(atoi((const char *)argv[3]) == 6){
				CT_Im_Ela_3_06();
			}
			else if(atoi((const char *)argv[3]) == 7){
				CT_Im_Ela_3_07();
			}
			else if(atoi((const char *)argv[3]) == 8){
				CT_Im_Ela_3_08();
			}
			else if(atoi((const char *)argv[3]) == 9){
				CT_Im_Ela_3_09();
			}
			else if(atoi((const char *)argv[3]) == 10){
				CT_Im_Ela_3_10();
			}
			else if(atoi((const char *)argv[3]) == 11){
				CT_Im_Ela_3_11();
			}
			else if(atoi((const char *)argv[3]) == 12){
				CT_Im_Ela_3_12();
			}
			else if(atoi((const char *)argv[3]) == 13){
				CT_Im_Ela_3_13();
			}
			else if(atoi((const char *)argv[3]) == 14){
				CT_Im_Ela_3_14();
			}
			else if(atoi((const char *)argv[3]) == 15){
				CT_Im_Ela_3_15();
			}
			else if(atoi((const char *)argv[3]) == 16){
				CT_Im_Ela_3_16();
			}
			else if(atoi((const char *)argv[3]) == 17){
				CT_Im_Ela_3_17();
			}
			else if(atoi((const char *)argv[3]) == 18){
				CT_Im_Ela_3_18();
			}
			else if(atoi((const char *)argv[3]) == 19){
				CT_Im_Ela_3_19();
			}
			else if(atoi((const char *)argv[3]) == 20){
				CT_Im_Ela_3_20();
			}
			else if(atoi((const char *)argv[3]) == 21){
				CT_Im_Ela_3_21();
			}
			else if(atoi((const char *)argv[3]) == 22){
				CT_Im_Ela_3_22();
			}
			else if(atoi((const char *)argv[3]) == 23){
				CT_Im_Ela_3_23();
			}
			else if(atoi((const char *)argv[3]) == 24){
				CT_Im_Ela_3_24();
			}
			else if(atoi((const char *)argv[3]) == 25){
				CT_Im_Ela_3_25();
			}
			else if(atoi((const char *)argv[3]) == 26){
				CT_Im_Ela_3_26();
			}
			else if(atoi((const char *)argv[3]) == 27){
				CT_Im_Ela_3_27();
			}
			else if(atoi((const char *)argv[3]) == 28){
				CT_Im_Ela_3_28();
			}
			else if(atoi((const char *)argv[3]) == 29){
				CT_Im_Ela_3_29();
			}
			else if(atoi((const char *)argv[3]) == 30){
				CT_Im_Ela_3_30();
			}
			else if(atoi((const char *)argv[3]) == 31){
				CT_Im_Ela_3_31();
			}
			else {
				Ddim_Print(("input parameter error\n"));
			}
		}
		else{
			Ddim_Print(("input parameter error\n"));
			return;
		}
	}
	else{
		Ddim_Print(("Error: Unknown command.\n"));
	}

//	Dd_MMU_Clean_Flush_Dcache_All();
	DDIM_User_L1l2cache_Clean_Flush_All();

	return;
}
