/**
 * @file		ct_dd_pdm.c
 * @brief		ct code for dd_pdm.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "pdm.h"
#include "dd_pdm.h"
#include "ct_dd_pdm.h"
#include "dd_top.h"
#include "dd_cache.h"
#include "dd_gic.h"
#include "dd_audio.h"
#include "dd_hdmac0.h"
#include "peripheral.h"
#include <string.h>
#include <stdlib.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define CT_DD_PDM_WORK_AREA		(0x7C000000)
#define CT_DD_PDM_WORK_AREA2	(0x7D000000)

// #define PC_DEBUG

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
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static VOID ct_dd_pdm_reg_init( VOID )
{
#ifdef PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL10ST = 1;
	IO_PDM[0].CORE_CFG.word = 0x00084365;
	IO_PDM[1].CORE_CFG.word = 0x00084365;
	IO_PDM[0].DMA_CFG.word = 0x00000069;
	IO_PDM[1].DMA_CFG.word = 0x00000069;
#endif
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

VOID CT_Dd_Pdm_OpenTest1( UINT8 ch )
{
	INT32 result;
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	result = Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_POL);
	Dd_Pdm_Close(ch);

	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_OpenTest2( UINT8 ch )
{
	INT32 result;
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	result = Dd_Pdm_Open(ch, 3000);
	Dd_Pdm_Close(ch);

	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_OpenTest3( UINT8 ch )
{
	INT32 result;
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	result = Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR);
	Dd_Pdm_Close(ch);

	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_OpenTest_e1( UINT8 ch )
{
	INT32 result;
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Open(ch, -2);
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_OpenTest_e2( UINT8 ch )
{
	INT32 result;
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_POL);
	result = Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_POL);
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_CloseTest( UINT8 ch )
{
	INT32 result;
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_POL);
	result = Dd_Pdm_Close(ch);

	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Dma0_Int_HandlerTest( UINT8 ch )
{
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	Dd_Pdm_Dma0_Int_Handler(ch);

	Ddim_Print(("<%s> End. ch=%d\n", __FUNCTION__, ch));
}

VOID CT_Dd_Pdm_Dma1_Int_HandlerTest( UINT8 ch )
{
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	Dd_Pdm_Dma1_Int_Handler(ch);

	Ddim_Print(("<%s> End. ch=%d\n", __FUNCTION__, ch));
}

VOID CT_Dd_Pdm_OverFlow_Int_HandlerTest( UINT8 ch )
{
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	Dd_Pdm_OverFlow_Int_Handler(ch);

	Ddim_Print(("<%s> End. ch=%d\n", __FUNCTION__, ch));
}

VOID CT_Dd_Pdm_InitTest( VOID )
{
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	Ddim_Print(("PDM0.PDMCORE_EN=0x%x\n", IO_PDM[D_DD_PDM_CH0].CORE_CFG.bit.PDMCORE_EN));
	Ddim_Print(("PDM1.PDMCORE_EN=0x%x\n", IO_PDM[D_DD_PDM_CH1].CORE_CFG.bit.PDMCORE_EN));
	
	Ddim_Print(("PDM0.DMA_EN=0x%x\n", IO_PDM[D_DD_PDM_CH0].DMA_CFG.bit.DMA_EN));
	Ddim_Print(("PDM1.DMA_EN=0x%x\n", IO_PDM[D_DD_PDM_CH1].DMA_CFG.bit.DMA_EN));
	
	Ddim_Print(("PDM0.CLR_IRQ_FFOVF=0x%x\n", IO_PDM[D_DD_PDM_CH0].DMA_CFG.bit.CLR_IRQ_FFOVF));
	Ddim_Print(("PDM1.CLR_IRQ_FFOVF=0x%x\n", IO_PDM[D_DD_PDM_CH1].DMA_CFG.bit.CLR_IRQ_FFOVF));
	
	Ddim_Print(("PDM0.CLR_IRQ_DMA1=0x%x\n", IO_PDM[D_DD_PDM_CH0].DMA_CFG.bit.CLR_IRQ_DMA1));
	Ddim_Print(("PDM1.CLR_IRQ_DMA1=0x%x\n", IO_PDM[D_DD_PDM_CH1].DMA_CFG.bit.CLR_IRQ_DMA1));
	
	Ddim_Print(("PDM0.CLR_IRQ_DMA0=0x%x\n", IO_PDM[D_DD_PDM_CH0].DMA_CFG.bit.CLR_IRQ_DMA0));
	Ddim_Print(("PDM1.CLR_IRQ_DMA0=0x%x\n", IO_PDM[D_DD_PDM_CH1].DMA_CFG.bit.CLR_IRQ_DMA0));
	
	Ddim_Print(("PDM0.DMA0_INT_REG=0x%x\n", IO_PDM[D_DD_PDM_CH0].STATUS.bit.DMA0_INT_REG));
	Ddim_Print(("PDM1.DMA0_INT_REG=0x%x\n", IO_PDM[D_DD_PDM_CH1].STATUS.bit.DMA0_INT_REG));
	
	Ddim_Print(("PDM0.DMA1_INT_REG=0x%x\n", IO_PDM[D_DD_PDM_CH0].STATUS.bit.DMA1_INT_REG));
	Ddim_Print(("PDM1.DMA1_INT_REG=0x%x\n", IO_PDM[D_DD_PDM_CH1].STATUS.bit.DMA1_INT_REG));
	
	Ddim_Print(("<%s> End.\n", __FUNCTION__));
}

VOID CT_Dd_Pdm_Ctrl_CoreTest( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_CORE_CFG core_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Core(ch, &core_cfg);
	
	core_cfg.swap = E_DD_PDM_CORE_LR_SWAP_SWAP;
	core_cfg.pga_r = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.pga_l = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.mclk_div = 12;
	core_cfg.sinc_rate = 48;
	core_cfg.adc_hpd = D_DD_PDM_ENABLE;
	core_cfg.hp_cutoff = 2;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_1000US;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_5;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	core_cfg.soft_mute = D_DD_PDM_ENABLE;
	
	result = Dd_Pdm_Ctrl_Core(ch, &core_cfg);
	
	if (ch <= 1){
		Ddim_Print(("LRSWAP=%d\n", IO_PDM[ch].CORE_CFG.bit.LRSWAP));
		Ddim_Print(("PGA_R=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_R));
		Ddim_Print(("PGA_L=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_L));
		Ddim_Print(("MCLKDIV=%d\n", IO_PDM[ch].CORE_CFG.bit.MCLKDIV));
		Ddim_Print(("SINC_RATE=%d\n", IO_PDM[ch].CORE_CFG.bit.SINC_RATE));
		Ddim_Print(("ADCHPD=%d\n", IO_PDM[ch].CORE_CFG.bit.ADCHPD));
		Ddim_Print(("HPCUTOFF=%d\n", IO_PDM[ch].CORE_CFG.bit.HPCUTOFF));
		Ddim_Print(("S_CYCLES=%d\n", IO_PDM[ch].CORE_CFG.bit.S_CYCLES));
		Ddim_Print(("SOFT_MUTE=%d\n", IO_PDM[ch].CORE_CFG.bit.SOFT_MUTE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_CoreTest_e1( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_CORE_CFG core_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Core(ch, &core_cfg);
	
	core_cfg.swap = (E_DD_PDM_CORE_LR_SWAP)2;
	core_cfg.pga_r = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.pga_l = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.mclk_div = 12;
	core_cfg.sinc_rate = 48;
	core_cfg.adc_hpd = D_DD_PDM_ENABLE;
	core_cfg.hp_cutoff = 2;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_1000US;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_5;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	core_cfg.soft_mute = D_DD_PDM_ENABLE;
	
	result = Dd_Pdm_Ctrl_Core(ch, &core_cfg);
	
	if (ch <= 1){
		Ddim_Print(("LRSWAP=%d\n", IO_PDM[ch].CORE_CFG.bit.LRSWAP));
		Ddim_Print(("PGA_R=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_R));
		Ddim_Print(("PGA_L=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_L));
		Ddim_Print(("MCLKDIV=%d\n", IO_PDM[ch].CORE_CFG.bit.MCLKDIV));
		Ddim_Print(("SINC_RATE=%d\n", IO_PDM[ch].CORE_CFG.bit.SINC_RATE));
		Ddim_Print(("ADCHPD=%d\n", IO_PDM[ch].CORE_CFG.bit.ADCHPD));
		Ddim_Print(("HPCUTOFF=%d\n", IO_PDM[ch].CORE_CFG.bit.HPCUTOFF));
		Ddim_Print(("S_CYCLES=%d\n", IO_PDM[ch].CORE_CFG.bit.S_CYCLES));
		Ddim_Print(("SOFT_MUTE=%d\n", IO_PDM[ch].CORE_CFG.bit.SOFT_MUTE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_CoreTest_e2( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_CORE_CFG core_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Core(ch, &core_cfg);
	
	core_cfg.swap = E_DD_PDM_CORE_LR_SWAP_SWAP;
	core_cfg.pga_r = (E_DD_PDM_CORE_PGA)16;
	core_cfg.pga_l = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.mclk_div = 12;
	core_cfg.sinc_rate = 48;
	core_cfg.adc_hpd = D_DD_PDM_ENABLE;
	core_cfg.hp_cutoff = 2;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_1000US;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_5;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	core_cfg.soft_mute = D_DD_PDM_ENABLE;
	
	result = Dd_Pdm_Ctrl_Core(ch, &core_cfg);
	
	if (ch <= 1){
		Ddim_Print(("LRSWAP=%d\n", IO_PDM[ch].CORE_CFG.bit.LRSWAP));
		Ddim_Print(("PGA_R=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_R));
		Ddim_Print(("PGA_L=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_L));
		Ddim_Print(("MCLKDIV=%d\n", IO_PDM[ch].CORE_CFG.bit.MCLKDIV));
		Ddim_Print(("SINC_RATE=%d\n", IO_PDM[ch].CORE_CFG.bit.SINC_RATE));
		Ddim_Print(("ADCHPD=%d\n", IO_PDM[ch].CORE_CFG.bit.ADCHPD));
		Ddim_Print(("HPCUTOFF=%d\n", IO_PDM[ch].CORE_CFG.bit.HPCUTOFF));
		Ddim_Print(("S_CYCLES=%d\n", IO_PDM[ch].CORE_CFG.bit.S_CYCLES));
		Ddim_Print(("SOFT_MUTE=%d\n", IO_PDM[ch].CORE_CFG.bit.SOFT_MUTE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_CoreTest_e3( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_CORE_CFG core_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Core(ch, &core_cfg);
	
	core_cfg.swap = E_DD_PDM_CORE_LR_SWAP_SWAP;
	core_cfg.pga_r = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.pga_l = (E_DD_PDM_CORE_PGA)16;
	core_cfg.mclk_div = 12;
	core_cfg.sinc_rate = 48;
	core_cfg.adc_hpd = D_DD_PDM_ENABLE;
	core_cfg.hp_cutoff = 2;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_1000US;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_5;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	core_cfg.soft_mute = D_DD_PDM_ENABLE;
	
	result = Dd_Pdm_Ctrl_Core(ch, &core_cfg);
	
	if (ch <= 1){
		Ddim_Print(("LRSWAP=%d\n", IO_PDM[ch].CORE_CFG.bit.LRSWAP));
		Ddim_Print(("PGA_R=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_R));
		Ddim_Print(("PGA_L=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_L));
		Ddim_Print(("MCLKDIV=%d\n", IO_PDM[ch].CORE_CFG.bit.MCLKDIV));
		Ddim_Print(("SINC_RATE=%d\n", IO_PDM[ch].CORE_CFG.bit.SINC_RATE));
		Ddim_Print(("ADCHPD=%d\n", IO_PDM[ch].CORE_CFG.bit.ADCHPD));
		Ddim_Print(("HPCUTOFF=%d\n", IO_PDM[ch].CORE_CFG.bit.HPCUTOFF));
		Ddim_Print(("S_CYCLES=%d\n", IO_PDM[ch].CORE_CFG.bit.S_CYCLES));
		Ddim_Print(("SOFT_MUTE=%d\n", IO_PDM[ch].CORE_CFG.bit.SOFT_MUTE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_CoreTest_e4( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_CORE_CFG core_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Core(ch, &core_cfg);
	
	core_cfg.swap = E_DD_PDM_CORE_LR_SWAP_SWAP;
	core_cfg.pga_r = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.pga_l = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.mclk_div = 32;
	core_cfg.sinc_rate = 48;
	core_cfg.adc_hpd = D_DD_PDM_ENABLE;
	core_cfg.hp_cutoff = 2;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_1000US;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_5;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	core_cfg.soft_mute = D_DD_PDM_ENABLE;
	
	result = Dd_Pdm_Ctrl_Core(ch, &core_cfg);
	
	if (ch <= 1){
		Ddim_Print(("LRSWAP=%d\n", IO_PDM[ch].CORE_CFG.bit.LRSWAP));
		Ddim_Print(("PGA_R=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_R));
		Ddim_Print(("PGA_L=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_L));
		Ddim_Print(("MCLKDIV=%d\n", IO_PDM[ch].CORE_CFG.bit.MCLKDIV));
		Ddim_Print(("SINC_RATE=%d\n", IO_PDM[ch].CORE_CFG.bit.SINC_RATE));
		Ddim_Print(("ADCHPD=%d\n", IO_PDM[ch].CORE_CFG.bit.ADCHPD));
		Ddim_Print(("HPCUTOFF=%d\n", IO_PDM[ch].CORE_CFG.bit.HPCUTOFF));
		Ddim_Print(("S_CYCLES=%d\n", IO_PDM[ch].CORE_CFG.bit.S_CYCLES));
		Ddim_Print(("SOFT_MUTE=%d\n", IO_PDM[ch].CORE_CFG.bit.SOFT_MUTE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_CoreTest_e5( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_CORE_CFG core_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Core(ch, &core_cfg);
	
	core_cfg.swap = E_DD_PDM_CORE_LR_SWAP_SWAP;
	core_cfg.pga_r = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.pga_l = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.mclk_div = 12;
	core_cfg.sinc_rate = 64;
	core_cfg.adc_hpd = D_DD_PDM_ENABLE;
	core_cfg.hp_cutoff = 2;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_1000US;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_5;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	core_cfg.soft_mute = D_DD_PDM_ENABLE;
	
	result = Dd_Pdm_Ctrl_Core(ch, &core_cfg);
	
	if (ch <= 1){
		Ddim_Print(("LRSWAP=%d\n", IO_PDM[ch].CORE_CFG.bit.LRSWAP));
		Ddim_Print(("PGA_R=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_R));
		Ddim_Print(("PGA_L=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_L));
		Ddim_Print(("MCLKDIV=%d\n", IO_PDM[ch].CORE_CFG.bit.MCLKDIV));
		Ddim_Print(("SINC_RATE=%d\n", IO_PDM[ch].CORE_CFG.bit.SINC_RATE));
		Ddim_Print(("ADCHPD=%d\n", IO_PDM[ch].CORE_CFG.bit.ADCHPD));
		Ddim_Print(("HPCUTOFF=%d\n", IO_PDM[ch].CORE_CFG.bit.HPCUTOFF));
		Ddim_Print(("S_CYCLES=%d\n", IO_PDM[ch].CORE_CFG.bit.S_CYCLES));
		Ddim_Print(("SOFT_MUTE=%d\n", IO_PDM[ch].CORE_CFG.bit.SOFT_MUTE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_CoreTest_e6( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_CORE_CFG core_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Core(ch, &core_cfg);
	
	core_cfg.swap = E_DD_PDM_CORE_LR_SWAP_SWAP;
	core_cfg.pga_r = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.pga_l = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.mclk_div = 12;
	core_cfg.sinc_rate = 48;
	core_cfg.adc_hpd = 2;
	core_cfg.hp_cutoff = 2;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_1000US;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_5;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	core_cfg.soft_mute = D_DD_PDM_ENABLE;
	
	result = Dd_Pdm_Ctrl_Core(ch, &core_cfg);
	
	if (ch <= 1){
		Ddim_Print(("LRSWAP=%d\n", IO_PDM[ch].CORE_CFG.bit.LRSWAP));
		Ddim_Print(("PGA_R=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_R));
		Ddim_Print(("PGA_L=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_L));
		Ddim_Print(("MCLKDIV=%d\n", IO_PDM[ch].CORE_CFG.bit.MCLKDIV));
		Ddim_Print(("SINC_RATE=%d\n", IO_PDM[ch].CORE_CFG.bit.SINC_RATE));
		Ddim_Print(("ADCHPD=%d\n", IO_PDM[ch].CORE_CFG.bit.ADCHPD));
		Ddim_Print(("HPCUTOFF=%d\n", IO_PDM[ch].CORE_CFG.bit.HPCUTOFF));
		Ddim_Print(("S_CYCLES=%d\n", IO_PDM[ch].CORE_CFG.bit.S_CYCLES));
		Ddim_Print(("SOFT_MUTE=%d\n", IO_PDM[ch].CORE_CFG.bit.SOFT_MUTE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_CoreTest_e7( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_CORE_CFG core_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Core(ch, &core_cfg);
	
	core_cfg.swap = E_DD_PDM_CORE_LR_SWAP_SWAP;
	core_cfg.pga_r = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.pga_l = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.mclk_div = 12;
	core_cfg.sinc_rate = 48;
	core_cfg.adc_hpd = D_DD_PDM_ENABLE;
	core_cfg.hp_cutoff = 2;
	core_cfg.s_cycle = (E_DD_PDM_CORE_S_CYCLE)8;
	core_cfg.soft_mute = D_DD_PDM_ENABLE;
	
	result = Dd_Pdm_Ctrl_Core(ch, &core_cfg);
	
	if (ch <= 1){
		Ddim_Print(("LRSWAP=%d\n", IO_PDM[ch].CORE_CFG.bit.LRSWAP));
		Ddim_Print(("PGA_R=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_R));
		Ddim_Print(("PGA_L=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_L));
		Ddim_Print(("MCLKDIV=%d\n", IO_PDM[ch].CORE_CFG.bit.MCLKDIV));
		Ddim_Print(("SINC_RATE=%d\n", IO_PDM[ch].CORE_CFG.bit.SINC_RATE));
		Ddim_Print(("ADCHPD=%d\n", IO_PDM[ch].CORE_CFG.bit.ADCHPD));
		Ddim_Print(("HPCUTOFF=%d\n", IO_PDM[ch].CORE_CFG.bit.HPCUTOFF));
		Ddim_Print(("S_CYCLES=%d\n", IO_PDM[ch].CORE_CFG.bit.S_CYCLES));
		Ddim_Print(("SOFT_MUTE=%d\n", IO_PDM[ch].CORE_CFG.bit.SOFT_MUTE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_CoreTest_e8( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_CORE_CFG core_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Core(ch, &core_cfg);
	
	core_cfg.swap = E_DD_PDM_CORE_LR_SWAP_SWAP;
	core_cfg.pga_r = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.pga_l = E_DD_PDM_CORE_PGA_P_10_5_DB;
	core_cfg.mclk_div = 12;
	core_cfg.sinc_rate = 48;
	core_cfg.adc_hpd = D_DD_PDM_ENABLE;
	core_cfg.hp_cutoff = 2;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_1000US;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_5;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	core_cfg.soft_mute = 2;
	
	result = Dd_Pdm_Ctrl_Core(ch, &core_cfg);
	
	if (ch <= 1){
		Ddim_Print(("LRSWAP=%d\n", IO_PDM[ch].CORE_CFG.bit.LRSWAP));
		Ddim_Print(("PGA_R=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_R));
		Ddim_Print(("PGA_L=%d\n", IO_PDM[ch].CORE_CFG.bit.PGA_L));
		Ddim_Print(("MCLKDIV=%d\n", IO_PDM[ch].CORE_CFG.bit.MCLKDIV));
		Ddim_Print(("SINC_RATE=%d\n", IO_PDM[ch].CORE_CFG.bit.SINC_RATE));
		Ddim_Print(("ADCHPD=%d\n", IO_PDM[ch].CORE_CFG.bit.ADCHPD));
		Ddim_Print(("HPCUTOFF=%d\n", IO_PDM[ch].CORE_CFG.bit.HPCUTOFF));
		Ddim_Print(("S_CYCLES=%d\n", IO_PDM[ch].CORE_CFG.bit.S_CYCLES));
		Ddim_Print(("SOFT_MUTE=%d\n", IO_PDM[ch].CORE_CFG.bit.SOFT_MUTE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_CoreTest_e9( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Ctrl_Core(ch, NULL);
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Get_Ctrl_CoreTest( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_CORE_CFG core_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Get_Ctrl_Core(ch, &core_cfg);
	
	Ddim_Print(("swap=0x%x\n", core_cfg.swap));
	Ddim_Print(("pga_r=0x%x\n", core_cfg.pga_r));
	Ddim_Print(("pga_l=0x%x\n", core_cfg.pga_l));
	Ddim_Print(("mclk_div=0x%x\n", core_cfg.mclk_div));
	Ddim_Print(("sinc_rate=0x%x\n", core_cfg.sinc_rate));
	Ddim_Print(("adc_hpd=0x%x\n", core_cfg.adc_hpd));
	Ddim_Print(("hp_cutoff=0x%x\n", core_cfg.hp_cutoff));
	Ddim_Print(("s_cycle=0x%x\n", core_cfg.s_cycle));
	Ddim_Print(("soft_mute=0x%x\n", core_cfg.soft_mute));
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Get_Ctrl_CoreTest_e1( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Get_Ctrl_Core(ch, NULL);
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Start_StreamingTest( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Start_Streaming(ch);
	
	if (ch <= 1){
		Ddim_Print(("PDMCORE_EN=0x%x\n", IO_PDM[ch].CORE_CFG.bit.PDMCORE_EN));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Stop_StreamingTest( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Stop_Streaming(ch);
	
	if (ch <= 1){
		Ddim_Print(("PDMCORE_EN=0x%x\n", IO_PDM[ch].CORE_CFG.bit.PDMCORE_EN));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_DmaTest( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_DMA_CFG dma_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &dma_cfg);
	
	dma_cfg.dmick_dly		= 1;
	dma_cfg.dma_burstlen	= E_DD_PDM_DMA_BURST_LEN_1;
	dma_cfg.pcm_chset		= E_DD_PDM_DMA_PCM_CH_STEREO;
	dma_cfg.pcm_wdlen		= E_DD_PDM_DMA_PCM_WD_24;
	dma_cfg.dma_en			= D_DD_PDM_ENABLE;
	
	result = Dd_Pdm_Ctrl_Dma(ch, &dma_cfg);
	
	if (ch <= 1){
		Ddim_Print(("DMICK_DLY=%d\n", IO_PDM[ch].DMA_CFG.bit.DMICK_DLY));
		Ddim_Print(("DMA_BURSTLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_BURSTLEN));
		Ddim_Print(("PCM_CHSET=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_CHSET));
		Ddim_Print(("PCM_WDLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_WDLEN));
		Ddim_Print(("DMA_EN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_EN));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_DmaTest_e1( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_DMA_CFG dma_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &dma_cfg);
	
	dma_cfg.dmick_dly		= 4;
	dma_cfg.dma_burstlen	= E_DD_PDM_DMA_BURST_LEN_1;
	dma_cfg.pcm_chset		= E_DD_PDM_DMA_PCM_CH_STEREO;
	dma_cfg.pcm_wdlen		= E_DD_PDM_DMA_PCM_WD_24;
	dma_cfg.dma_en			= D_DD_PDM_ENABLE;
	
	result = Dd_Pdm_Ctrl_Dma(ch, &dma_cfg);
	
	if (ch <= 1){
		Ddim_Print(("DMICK_DLY=%d\n", IO_PDM[ch].DMA_CFG.bit.DMICK_DLY));
		Ddim_Print(("DMA_BURSTLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_BURSTLEN));
		Ddim_Print(("PCM_CHSET=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_CHSET));
		Ddim_Print(("PCM_WDLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_WDLEN));
		Ddim_Print(("DMA_EN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_EN));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_DmaTest_e2( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_DMA_CFG dma_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &dma_cfg);
	
	dma_cfg.dmick_dly		= 1;
	dma_cfg.dma_burstlen	= (E_DD_PDM_DMA_BURST_LEN)4;
	dma_cfg.pcm_chset		= E_DD_PDM_DMA_PCM_CH_STEREO;
	dma_cfg.pcm_wdlen		= E_DD_PDM_DMA_PCM_WD_24;
	dma_cfg.dma_en			= D_DD_PDM_ENABLE;
	
	result = Dd_Pdm_Ctrl_Dma(ch, &dma_cfg);
	
	if (ch <= 1){
		Ddim_Print(("DMICK_DLY=%d\n", IO_PDM[ch].DMA_CFG.bit.DMICK_DLY));
		Ddim_Print(("DMA_BURSTLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_BURSTLEN));
		Ddim_Print(("PCM_CHSET=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_CHSET));
		Ddim_Print(("PCM_WDLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_WDLEN));
		Ddim_Print(("DMA_EN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_EN));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_DmaTest_e3( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_DMA_CFG dma_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &dma_cfg);
	
	dma_cfg.dmick_dly		= 1;
	dma_cfg.dma_burstlen	= E_DD_PDM_DMA_BURST_LEN_1;
	dma_cfg.pcm_chset		= (E_DD_PDM_DMA_PCM_CH)4;
	dma_cfg.pcm_wdlen		= E_DD_PDM_DMA_PCM_WD_24;
	dma_cfg.dma_en			= D_DD_PDM_ENABLE;
	
	result = Dd_Pdm_Ctrl_Dma(ch, &dma_cfg);
	
	if (ch <= 1){
		Ddim_Print(("DMICK_DLY=%d\n", IO_PDM[ch].DMA_CFG.bit.DMICK_DLY));
		Ddim_Print(("DMA_BURSTLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_BURSTLEN));
		Ddim_Print(("PCM_CHSET=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_CHSET));
		Ddim_Print(("PCM_WDLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_WDLEN));
		Ddim_Print(("DMA_EN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_EN));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_DmaTest_e4( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_DMA_CFG dma_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &dma_cfg);
	
	dma_cfg.dmick_dly		= 1;
	dma_cfg.dma_burstlen	= E_DD_PDM_DMA_BURST_LEN_1;
	dma_cfg.pcm_chset		= E_DD_PDM_DMA_PCM_CH_STEREO;
	dma_cfg.pcm_wdlen		= (E_DD_PDM_DMA_PCM_WD)4;
	dma_cfg.dma_en			= D_DD_PDM_ENABLE;
	
	result = Dd_Pdm_Ctrl_Dma(ch, &dma_cfg);
	
	if (ch <= 1){
		Ddim_Print(("DMICK_DLY=%d\n", IO_PDM[ch].DMA_CFG.bit.DMICK_DLY));
		Ddim_Print(("DMA_BURSTLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_BURSTLEN));
		Ddim_Print(("PCM_CHSET=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_CHSET));
		Ddim_Print(("PCM_WDLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_WDLEN));
		Ddim_Print(("DMA_EN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_EN));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_DmaTest_e5( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_DMA_CFG dma_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &dma_cfg);
	
	dma_cfg.dmick_dly		= 1;
	dma_cfg.dma_burstlen	= E_DD_PDM_DMA_BURST_LEN_1;
	dma_cfg.pcm_chset		= E_DD_PDM_DMA_PCM_CH_STEREO;
	dma_cfg.pcm_wdlen		= E_DD_PDM_DMA_PCM_WD_24;
	dma_cfg.dma_en			= 2;
	
	result = Dd_Pdm_Ctrl_Dma(ch, &dma_cfg);
	
	if (ch <= 1){
		Ddim_Print(("DMICK_DLY=%d\n", IO_PDM[ch].DMA_CFG.bit.DMICK_DLY));
		Ddim_Print(("DMA_BURSTLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_BURSTLEN));
		Ddim_Print(("PCM_CHSET=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_CHSET));
		Ddim_Print(("PCM_WDLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_WDLEN));
		Ddim_Print(("DMA_EN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_EN));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Ctrl_DmaTest_e6( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Ctrl_Dma(ch, NULL);
	
	if (ch <= 1){
		Ddim_Print(("DMICK_DLY=%d\n", IO_PDM[ch].DMA_CFG.bit.DMICK_DLY));
		Ddim_Print(("DMA_BURSTLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_BURSTLEN));
		Ddim_Print(("PCM_CHSET=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_CHSET));
		Ddim_Print(("PCM_WDLEN=%d\n", IO_PDM[ch].DMA_CFG.bit.PCM_WDLEN));
		Ddim_Print(("DMA_EN=%d\n", IO_PDM[ch].DMA_CFG.bit.DMA_EN));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Get_Ctrl_DmaTest( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_DMA_CFG dma_cfg;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	result = Dd_Pdm_Get_Ctrl_Dma(ch, &dma_cfg);
	
	Ddim_Print(("dmick_dly=0x%x\n", dma_cfg.dmick_dly));
	Ddim_Print(("dma_burstlen=0x%x\n", dma_cfg.dma_burstlen));
	Ddim_Print(("pcm_chset=0x%x\n", dma_cfg.pcm_chset));
	Ddim_Print(("pcm_wdlen=0x%x\n", dma_cfg.pcm_wdlen));
	Ddim_Print(("dma_en=0x%x\n", dma_cfg.dma_en));
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Get_Ctrl_DmaTest_e1( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Get_Ctrl_Dma(ch, NULL);
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_CallbackDma0IntrTest( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Set_CallbackDma0Intr(ch, NULL);
	
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_CallbackDma1IntrTest( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Set_CallbackDma1Intr(ch, NULL);
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_CallbackOverFlowIntrTest( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Set_CallbackOverFlowIntr(ch, NULL);
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_EnableDma0IntrTest1( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	
	if (ch <= 1){
		Ddim_Print(("CLR_IRQ_DMA0=%d\n", IO_PDM[ch].DMA_CFG.bit.CLR_IRQ_DMA0));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_EnableDma0IntrTest2( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_DISABLE);
	
	if (ch <= 1){
		Ddim_Print(("CLR_IRQ_DMA0=%d\n", IO_PDM[ch].DMA_CFG.bit.CLR_IRQ_DMA0));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_EnableDma0IntrTest_e1( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Set_EnableDma0Intr(ch, 2);
	
	if (ch <= 1){
		Ddim_Print(("CLR_IRQ_DMA0=%d\n", IO_PDM[ch].DMA_CFG.bit.CLR_IRQ_DMA0));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_EnableDma1IntrTest1( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	if (ch <= 1){
		Ddim_Print(("CLR_IRQ_DMA1=%d\n", IO_PDM[ch].DMA_CFG.bit.CLR_IRQ_DMA1));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_EnableDma1IntrTest2( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_DISABLE);
	
	if (ch <= 1){
		Ddim_Print(("CLR_IRQ_DMA1=%d\n", IO_PDM[ch].DMA_CFG.bit.CLR_IRQ_DMA1));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_EnableDma1IntrTest_e1( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Set_EnableDma1Intr(ch, 2);
	
	if (ch <= 1){
		Ddim_Print(("CLR_IRQ_DMA1=%d\n", IO_PDM[ch].DMA_CFG.bit.CLR_IRQ_DMA1));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_EnableOverFlowIntrTest1( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Set_EnableOverFlowIntr(ch, D_DD_PDM_ENABLE);
	
	if (ch <= 1){
		Ddim_Print(("CLR_IRQ_FFOVF=%d\n", IO_PDM[ch].DMA_CFG.bit.CLR_IRQ_FFOVF));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_EnableOverFlowIntrTest2( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Set_EnableOverFlowIntr(ch, D_DD_PDM_DISABLE);
	
	if (ch <= 1){
		Ddim_Print(("CLR_IRQ_FFOVF=%d\n", IO_PDM[ch].DMA_CFG.bit.CLR_IRQ_FFOVF));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_EnableOverFlowIntrTest_e1( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	ct_dd_pdm_reg_init();
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Set_EnableOverFlowIntr(ch, 2);
	
	if (ch <= 1){
		Ddim_Print(("CLR_IRQ_FFOVF=%d\n", IO_PDM[ch].DMA_CFG.bit.CLR_IRQ_FFOVF));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_DMA0_Dst_AddrTest( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Set_DMA0_Dst_Addr(ch, 0x70000000);
	
	if (ch <= 1){
		Ddim_Print(("DMA0_DST_ADDR=0x%x\n", (unsigned int)IO_PDM[ch].DMA0_DST_ADDR.word));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_DMA1_Dst_AddrTest( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Set_DMA1_Dst_Addr(ch, 0x71000000);
	
	if (ch <= 1){
		Ddim_Print(("DMA1_DST_ADDR=0x%x\n", (unsigned int)IO_PDM[ch].DMA1_DST_ADDR.word));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_Dma_TransLengthTest( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_DMA_LEN dma_len;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	dma_len.ttsize = 0x20;
	dma_len.tsize = 0x10;
	
	result = Dd_Pdm_Set_Dma_TransLength(ch, &dma_len);
	
	if (ch <= 1){
		Ddim_Print(("DMA_TTSIZE=0x%x\n", IO_PDM[ch].DMA_LEN.bit.DMA_TTSIZE));
		Ddim_Print(("DMA_TSIZE=0x%x\n", IO_PDM[ch].DMA_LEN.bit.DMA_TSIZE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_Dma_TransLengthTest_e1( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Set_Dma_TransLength(ch, NULL);
	
	if (ch <= 1){
		Ddim_Print(("DMA_TTSIZE=0x%x\n", IO_PDM[ch].DMA_LEN.bit.DMA_TTSIZE));
		Ddim_Print(("DMA_TSIZE=0x%x\n", IO_PDM[ch].DMA_LEN.bit.DMA_TSIZE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_Dma_TransLengthTest_e2( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_DMA_LEN dma_len;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	dma_len.ttsize = 0;
	dma_len.tsize = 0x10;
	
	result = Dd_Pdm_Set_Dma_TransLength(ch, &dma_len);
	
	if (ch <= 1){
		Ddim_Print(("DMA_TTSIZE=0x%x\n", IO_PDM[ch].DMA_LEN.bit.DMA_TTSIZE));
		Ddim_Print(("DMA_TSIZE=0x%x\n", IO_PDM[ch].DMA_LEN.bit.DMA_TSIZE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_Dma_TransLengthTest_e3( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_DMA_LEN dma_len;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	dma_len.ttsize = 0x8000;
	dma_len.tsize = 0x10;
	
	result = Dd_Pdm_Set_Dma_TransLength(ch, &dma_len);
	
	if (ch <= 1){
		Ddim_Print(("DMA_TTSIZE=0x%x\n", IO_PDM[ch].DMA_LEN.bit.DMA_TTSIZE));
		Ddim_Print(("DMA_TSIZE=0x%x\n", IO_PDM[ch].DMA_LEN.bit.DMA_TSIZE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Set_Dma_TransLengthTest_e4( UINT8 ch )
{
	INT32 result;
	T_DD_PDM_DMA_LEN dma_len;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	dma_len.ttsize = 0x20;
	dma_len.tsize = 0;
	
	result = Dd_Pdm_Set_Dma_TransLength(ch, &dma_len);
	
	if (ch <= 1){
		Ddim_Print(("DMA_TTSIZE=0x%x\n", IO_PDM[ch].DMA_LEN.bit.DMA_TTSIZE));
		Ddim_Print(("DMA_TSIZE=0x%x\n", IO_PDM[ch].DMA_LEN.bit.DMA_TSIZE));
	}
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Flush_Dma_FifoTest( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Flush_Dma_Fifo(ch);
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Get_StatusTest1( UINT8 ch )
{
	INT32 result;
	UINT8 status;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Get_Status(ch, E_DD_PDM_INT_TYPE_FFOVF, &status);
	
	Ddim_Print(("status=0x%x\n", status));
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Get_StatusTest2( UINT8 ch )
{
	INT32 result;
	UINT8 status;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Get_Status(ch, E_DD_PDM_INT_TYPE_DMA0, &status);
	
	Ddim_Print(("status=0x%x\n", status));
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Get_StatusTest3( UINT8 ch )
{
	INT32 result;
	UINT8 status;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	Dd_Pdm_Init();
	
	result = Dd_Pdm_Get_Status(ch, E_DD_PDM_INT_TYPE_DMA1, &status);
	
	Ddim_Print(("status=0x%x\n", status));
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Get_StatusTest_e1( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Get_Status(ch, E_DD_PDM_INT_TYPE_FFOVF, NULL);
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Clear_StatusTest1( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Clear_Status(ch, E_DD_PDM_INT_TYPE_FFOVF);
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Clear_StatusTest2( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Clear_Status(ch, E_DD_PDM_INT_TYPE_DMA0);
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}

VOID CT_Dd_Pdm_Clear_StatusTest3( UINT8 ch )
{
	INT32 result;
	
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));
	
	result = Dd_Pdm_Clear_Status(ch, E_DD_PDM_INT_TYPE_DMA1);
	
	Ddim_Print(("<%s> End. result=0x%x, ch=%d\n", __FUNCTION__, result, ch));
}


VOID CT_Dd_PDM_3_Reg_RW( VOID )
{
	UINT8 ch;
	
	for (ch = 0; ch <= 1; ch++){
		
		//CORE_CFG
		IO_PDM[ch].CORE_CFG.word = 0xA5A5A5A5;
		if (IO_PDM[ch].CORE_CFG.word == 0xA5A5A5A5){
			Ddim_Print(("CORE_CFG%d Check OK\n", ch));
		}
		else {
			Ddim_Print(("CORE_CFG%d Check NG[A5]: 0x%08x\n", ch, (unsigned int)IO_PDM[ch].CORE_CFG.word));
		}

		IO_PDM[ch].CORE_CFG.word = 0x5A5A5A5A;
		if (IO_PDM[ch].CORE_CFG.word == 0x5A5A5A5A){
			Ddim_Print(("CORE_CFG%d Check OK\n", ch));
		}
		else {
			Ddim_Print(("CORE_CFG%d Check NG[5A]: 0x%08x\n", ch, (unsigned int)IO_PDM[ch].CORE_CFG.word));
		}
		
		// DMAC_CFG
		IO_PDM[ch].DMA_CFG.word = 0x3F00B4FF;
		if (IO_PDM[ch].DMA_CFG.word == 0x3F00B4FF){
			Ddim_Print(("DMA_CFG%d Check OK\n", ch));
		}
		else {
			Ddim_Print(("DMA_CFG%d Check NG[A5]: 0x%08x\n", ch, (unsigned int)IO_PDM[ch].DMA_CFG.word));
		}
		
		IO_PDM[ch].DMA_LEN.word = 0x7FFFFF00;
		if (IO_PDM[ch].DMA_LEN.word == 0x7FFFFF00){
			Ddim_Print(("DMA_LEN%d Check OK\n", ch));
		}
		else {
			Ddim_Print(("DMA_LEN%d Check NG[5A]: 0x%08x\n", ch, (unsigned int)IO_PDM[ch].DMA_LEN.word));
		}

		IO_PDM[ch].DMA0_DST_ADDR.word = 0xFFFFFFFC;
		if (IO_PDM[ch].DMA0_DST_ADDR.word == 0xFFFFFFFC){
			Ddim_Print(("DMA0_DST_ADDR%d Check OK\n", ch));
		}
		else {
			Ddim_Print(("DMA0_DST_ADDR%d Check NG[5A]: 0x%08x\n", ch, (unsigned int)IO_PDM[ch].DMA0_DST_ADDR.word));
		}
		
		IO_PDM[ch].DMA1_DST_ADDR.word = 0xFFFFFFFC;
		if (IO_PDM[ch].DMA1_DST_ADDR.word == 0xFFFFFFFC){
			Ddim_Print(("DMA1_DST_ADDR%d Check OK\n", ch));
		}
		else {
			Ddim_Print(("DMA1_DST_ADDR%d Check NG[5A]: 0x%08x\n", ch, (unsigned int)IO_PDM[ch].DMA1_DST_ADDR.word));
		}

	}
}

static volatile UINT32 gCt_Dd_Pdm_Dma_Int_Cnt;

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
static VOID ct_dd_pdm_DMA_Int_Handler001( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	
	if ((gCt_Dd_Pdm_Dma_Int_Cnt % 2) == 0){
		Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_P72, D_DD_TOP_GPIO_STATUS_HIGH);
	}
	else {
		Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_P72, D_DD_TOP_GPIO_STATUS_LOW);
	}
	
	if(gCt_Dd_Pdm_Dma_Int_Cnt < 10) {
		Dd_HDMAC0_Input_Audio_Async( 0,
									 D_DD_HDMAC0_IS_IDREQ_8,
									 Dd_Audio_Get_Addr_Reg_AUIDLR(4),
									 CT_DD_PDM_WORK_AREA+(9600*2*2*gCt_Dd_Pdm_Dma_Int_Cnt),
									 (9600*2*2),
									 (VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler001);
	}
	
	Ddim_Print(("ct_dd_pdm_DMA_Int_Handler001 \n"));
}

VOID CT_Dd_PDM_3_001(VOID)
{
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 8;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(9600*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler001);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	
#if 0
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(0, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(0, CT_DD_PDM_WORK_AREA+0x400);
	
	T_DD_PDM_DMA_LEN pdm_len;
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(0, &pdm_len);
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(0, &pdm_dma_cfg);
#endif
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm 001 : Record PDM0 96kHz test\n"));
}

static VOID ct_dd_pdm_DMA_Int_Handler002( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	
	if(gCt_Dd_Pdm_Dma_Int_Cnt < 10) {
		Dd_HDMAC0_Input_Audio_Async( 0,
									 D_DD_HDMAC0_IS_IDREQ_8,
									 Dd_Audio_Get_Addr_Reg_AUIDLR(4),
									 CT_DD_PDM_WORK_AREA+(4800*2*2*gCt_Dd_Pdm_Dma_Int_Cnt),
									 (4800*2*2),
									 (VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler002);
	}
	
	Ddim_Print(("ct_dd_pdm_DMA_Int_Handler002 \n"));
}

VOID CT_Dd_PDM_3_002(VOID)
{
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(4800*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler002);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	
#if 0
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(0, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(0, CT_DD_PDM_WORK_AREA+0x400);
	
	T_DD_PDM_DMA_LEN pdm_len;
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(0, &pdm_len);
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(0, &pdm_dma_cfg);
#endif
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm 002 : Record PDM0 48kHz test\n"));
}

static VOID ct_dd_pdm_DMA_Int_Handler003( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	
	if(gCt_Dd_Pdm_Dma_Int_Cnt < 10) {
		Dd_HDMAC0_Input_Audio_Async( 0,
									 D_DD_HDMAC0_IS_IDREQ_8,
									 Dd_Audio_Get_Addr_Reg_AUIDLR(4),
									 CT_DD_PDM_WORK_AREA+(3200*2*2*gCt_Dd_Pdm_Dma_Int_Cnt),
									 (3200*2*2),
									 (VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler003);
	}
	
	Ddim_Print(("ct_dd_pdm_DMA_Int_Handler003 \n"));
}

VOID CT_Dd_PDM_3_003(VOID)
{
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 24;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_8_192;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(3200*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler003);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	
#if 0
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(0, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(0, CT_DD_PDM_WORK_AREA+0x400);
	
	T_DD_PDM_DMA_LEN pdm_len;
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(0, &pdm_len);
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(0, &pdm_dma_cfg);
#endif
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(3000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm 003 : Record PDM0 32kHz test\n"));
}

static VOID ct_dd_pdm_DMA_Int_Handler004( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	
	if(gCt_Dd_Pdm_Dma_Int_Cnt < 10) {
		Dd_HDMAC0_Input_Audio_Async( 0,
									 D_DD_HDMAC0_IS_IDREQ_10,
									 Dd_Audio_Get_Addr_Reg_AUIDLR(5),
									 CT_DD_PDM_WORK_AREA+(9600*2*2*gCt_Dd_Pdm_Dma_Int_Cnt),
									 (9600*2*2),
									 (VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler004);
	}
	
	Ddim_Print(("ct_dd_pdm_DMA_Int_Handler004 \n"));
}

VOID CT_Dd_PDM_3_004(VOID)
{
	UINT8 ch = 1;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 8;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	pdm_cfg.swap = E_DD_PDM_CORE_LR_SWAP_SWAP;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Open_Input(5, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(5, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(5, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(5, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(5, &i2s_in);
	
	Dd_Audio_Get_Ctrl_Common(5, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(5, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(5, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(5, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(5, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(5, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(5, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_10,
								Dd_Audio_Get_Addr_Reg_AUIDLR(5),
								CT_DD_PDM_WORK_AREA,
								(9600*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler004);
	
	Dd_Audio_Set_InputDMARequestEnable(5, D_DD_AUDIO_ENABLE);
	
	
#if 0
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(0, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(0, CT_DD_PDM_WORK_AREA+0x400);
	
	T_DD_PDM_DMA_LEN pdm_len;
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(0, &pdm_len);
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(0, &pdm_dma_cfg);
#endif
	
	Dd_Audio_Start_Input(5);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(5);
	
	Dd_Audio_Set_InputDMARequestEnable(5, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(5);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm 004 : Record PDM1 96kHz test\n"));
}


static VOID ct_dd_pdm_DMA_Int_Handler005( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	
	if(gCt_Dd_Pdm_Dma_Int_Cnt < 10) {
		Dd_HDMAC0_Input_Audio_Async( 0,
									 D_DD_HDMAC0_IS_IDREQ_10,
									 Dd_Audio_Get_Addr_Reg_AUIDLR(5),
									 CT_DD_PDM_WORK_AREA+(4800*2*2*gCt_Dd_Pdm_Dma_Int_Cnt),
									 (4800*2*2),
									 (VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler005);
	}
	
	Ddim_Print(("ct_dd_pdm_DMA_Int_Handler005 \n"));
}

VOID CT_Dd_PDM_3_005(VOID)
{
	UINT8 ch = 1;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	pdm_cfg.swap = E_DD_PDM_CORE_LR_SWAP_SWAP;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Open_Input(5, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(5, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(5, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(5, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(5, &i2s_in);
	
	Dd_Audio_Get_Ctrl_Common(5, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(5, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(5, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(5, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(5, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(5, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(5, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_10,
								Dd_Audio_Get_Addr_Reg_AUIDLR(5),
								CT_DD_PDM_WORK_AREA,
								(4800*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler005);
	
	Dd_Audio_Set_InputDMARequestEnable(5, D_DD_AUDIO_ENABLE);
	
	
#if 0
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(0, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(0, CT_DD_PDM_WORK_AREA+0x400);
	
	T_DD_PDM_DMA_LEN pdm_len;
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(0, &pdm_len);
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(0, &pdm_dma_cfg);
#endif
	
	Dd_Audio_Start_Input(5);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(5);
	
	Dd_Audio_Set_InputDMARequestEnable(5, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(5);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm 005 : Record PDM1 48kHz test\n"));
}

static VOID ct_dd_pdm_DMA_Int_Handler006( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	
	if(gCt_Dd_Pdm_Dma_Int_Cnt < 10) {
		Dd_HDMAC0_Input_Audio_Async( 0,
									 D_DD_HDMAC0_IS_IDREQ_10,
									 Dd_Audio_Get_Addr_Reg_AUIDLR(5),
									 CT_DD_PDM_WORK_AREA+(3200*2*2*gCt_Dd_Pdm_Dma_Int_Cnt),
									 (3200*2*2),
									 (VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler006);
	}
	
	Ddim_Print(("ct_dd_pdm_DMA_Int_Handler005 \n"));
}

VOID CT_Dd_PDM_3_006(VOID)
{
	UINT8 ch = 1;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 24;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	pdm_cfg.swap = E_DD_PDM_CORE_LR_SWAP_SWAP;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Open_Input(5, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(5, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_8_192;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(5, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(5, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(5, &i2s_in);
	
	Dd_Audio_Get_Ctrl_Common(5, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(5, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(5, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(5, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(5, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(5, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(5, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_10,
								Dd_Audio_Get_Addr_Reg_AUIDLR(5),
								CT_DD_PDM_WORK_AREA,
								(3200*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler006);
	
	Dd_Audio_Set_InputDMARequestEnable(5, D_DD_AUDIO_ENABLE);
	
	
#if 0
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(0, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(0, CT_DD_PDM_WORK_AREA+0x400);
	
	T_DD_PDM_DMA_LEN pdm_len;
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(0, &pdm_len);
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(0, &pdm_dma_cfg);
#endif
	
	Dd_Audio_Start_Input(5);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(5);
	
	Dd_Audio_Set_InputDMARequestEnable(5, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(5);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm 006 : Record PDM1 32kHz test\n"));
}

// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---


// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---

static VOID ct_dd_pdm_DMA_Int_Handler007( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	
	if(gCt_Dd_Pdm_Dma_Int_Cnt < 10) {
		Dd_HDMAC0_Input_Audio_Async( 0,
									 D_DD_HDMAC0_IS_IDREQ_8,
									 Dd_Audio_Get_Addr_Reg_AUIDLR(4),
									 CT_DD_PDM_WORK_AREA+(4800*2*2*gCt_Dd_Pdm_Dma_Int_Cnt),
									 (4800*2*2),
									 (VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler007);
	}
	
	Ddim_Print(( "%s\n", __FUNCTION__ ));

}

VOID CT_Dd_PDM_3_007(VOID)
{
	// Clock:24.576MHz(PDM CLK)
	// FS:48kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	Dd_Top_Set_CLKSEL7_PDM0SEL(4);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(4800*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler007);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 001 : Record PDM0 Clock:24.576MHz(PDM_CLK) FS:48kHz test\n"));
}

VOID CT_Dd_PDM_3_008(VOID)
{
	// Clock:24.576MHz(AUCLK)
	// FS:48kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(4800*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler007);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 002 : Record PDM0 Clock:24.576MHz(AUCLK) FS=48kHz test\n"));
}

VOID CT_Dd_PDM_3_009(VOID)
{
	// Clock:12.288MHz(AUCLK/2)
	// FS:48kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(4800*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler007);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 003 : Record PDM0 Clock:12.288MHz(AUCLK/2) FS=48kHz test\n"));
}

static VOID ct_dd_pdm_DMA_Int_Handler010( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	
	if(gCt_Dd_Pdm_Dma_Int_Cnt < 10) {
		Dd_HDMAC0_Input_Audio_Async( 0,
									 D_DD_HDMAC0_IS_IDREQ_8,
									 Dd_Audio_Get_Addr_Reg_AUIDLR(4),
									 CT_DD_PDM_WORK_AREA+(3200*2*2*gCt_Dd_Pdm_Dma_Int_Cnt),
									 (3200*2*2),
									 (VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler010);
	}
	
	Ddim_Print(( "%s\n", __FUNCTION__ ));
}

VOID CT_Dd_PDM_3_010(VOID)
{
	// Clock:24.576MHz(PDM CLK)
	// FS:32kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	Dd_Top_Set_CLKSEL7_PDM0SEL(4);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 24;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_8_192;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(3200*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler010);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 004 : Record PDM0 Clock:24.576MHz(PDM_CLK) FS:32kHz test\n"));
}

VOID CT_Dd_PDM_3_011(VOID)
{
	// Clock:8.192MHz(AUCLK)
	// FS:32kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_8_192;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(3200*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler010);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 005 : Record PDM0 Clock:8.192MHz(AUCLK) FS=32kHz test\n"));
}

static VOID ct_dd_pdm_DMA_Int_Handler012( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	
	if(gCt_Dd_Pdm_Dma_Int_Cnt < 10) {
		Dd_HDMAC0_Input_Audio_Async( 0,
									 D_DD_HDMAC0_IS_IDREQ_8,
									 Dd_Audio_Get_Addr_Reg_AUIDLR(4),
									 CT_DD_PDM_WORK_AREA+(2400*2*2*gCt_Dd_Pdm_Dma_Int_Cnt),
									 (2400*2*2),
									 (VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler012);
	}
	
	Ddim_Print(( "%s\n", __FUNCTION__ ));
}

VOID CT_Dd_PDM_3_012(VOID)
{
	// Clock:24.576MHz(PDM CLK)
	// FS:24kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	Dd_Top_Set_CLKSEL7_PDM0SEL(4);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_4;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(2400*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler012);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 006 : Record PDM0 Clock:24.576MHz(PDM_CLK) FS:24kHz test\n"));
}

VOID CT_Dd_PDM_3_013(VOID)
{
	// Clock:24.576MHz(AUCLK)
	// FS:24kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_4;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(2400*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler012);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 007 : Record PDM0 Clock:24.576MHz(AUCLK) FS=24kHz test\n"));
}

VOID CT_Dd_PDM_3_014(VOID)
{
	// Clock:12.288MHz(AUCLK/2)
	// FS:48kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_4;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(2400*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler012);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 008 : Record PDM0 Clock:12.288MHz(AUCLK/2) FS=24kHz test\n"));
}

static VOID ct_dd_pdm_DMA_Int_Handler015( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	
	if(gCt_Dd_Pdm_Dma_Int_Cnt < 10) {
		Dd_HDMAC0_Input_Audio_Async( 0,
									 D_DD_HDMAC0_IS_IDREQ_8,
									 Dd_Audio_Get_Addr_Reg_AUIDLR(4),
									 CT_DD_PDM_WORK_AREA+(1600*2*2*gCt_Dd_Pdm_Dma_Int_Cnt),
									 (1600*2*2),
									 (VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler015);
	}
	
	Ddim_Print(( "%s\n", __FUNCTION__ ));
}

VOID CT_Dd_PDM_3_015(VOID)
{
	// Clock:24.576MHz(PDM CLK)
	// FS:16kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select PDM_CLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(4);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 48;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_6;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(1600*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler015);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 009 : Record PDM0 Clock:24.576MHz(PDM_CLK) FS:16kHz test\n"));
}

VOID CT_Dd_PDM_3_016(VOID)
{
	// Clock:24.576MMHz(AUCLK)
	// FS:16kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 48;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_6;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(1600*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler015);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 010 : Record PDM0 Clock:24.576MHz(AUCLK) FS:16kHz test\n"));
}

VOID CT_Dd_PDM_3_017(VOID)
{
	// Clock:12.288MHz(AUCLK/2)
	// FS:16kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 48;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_6;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(1600*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler015);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 011 : Record PDM0 Clock:12.288MHz(AUCLK/2) FS=16kHz test\n"));
}

VOID CT_Dd_PDM_3_018(VOID)
{
	// Clock:8.192MHz(AUCLK)
	// FS:16kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_8_192;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(1600*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler015);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 012 : Record PDM0 Clock:8.192MHz(AUCLK) FS=16kHz test\n"));
}

static VOID ct_dd_pdm_DMA_Int_Handler019( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	
	if(gCt_Dd_Pdm_Dma_Int_Cnt < 10) {
		Dd_HDMAC0_Input_Audio_Async( 0,
									 D_DD_HDMAC0_IS_IDREQ_8,
									 Dd_Audio_Get_Addr_Reg_AUIDLR(4),
									 CT_DD_PDM_WORK_AREA+(800*2*2*gCt_Dd_Pdm_Dma_Int_Cnt),
									 (800*2*2),
									 (VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler019);
	}
	
	Ddim_Print(( "%s\n", __FUNCTION__ ));
}

VOID CT_Dd_PDM_3_019(VOID)
{
	// Clock:12.288MHz(AUCLK/2)
	// FS:8kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 12;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_12;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(800*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler019);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 013 : Record PDM0 Clock:12.288MHz(AUCLK/2) FS=8kHz test\n"));
}

VOID CT_Dd_PDM_3_020(VOID)
{
	// Clock:8.192MHz(AUCLK)
	// FS:8kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_8_192;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_4;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(800*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler019);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 014 : PDM0 Clock:8.192MHz(AUCLK) FS=8kHz test\n"));
}


VOID CT_Dd_PDM_3_021(VOID)
{
	// Clock:4.096MHz(AUCLK/2)
	// FS:8kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_8_192;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_4;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(800*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler019);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 015 : PDM0 Clock:4.092MHz(AUCLK/2) FS=8kHz test\n"));
}

static VOID ct_dd_pdm_DMA_Int_Handler022( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	
	if(gCt_Dd_Pdm_Dma_Int_Cnt < 10) {
		Dd_HDMAC0_Input_Audio_Async( 0,
									 D_DD_HDMAC0_IS_IDREQ_8,
									 Dd_Audio_Get_Addr_Reg_AUIDLR(4),
									 CT_DD_PDM_WORK_AREA+(4410*2*2*gCt_Dd_Pdm_Dma_Int_Cnt),
									 (4410*2*2),
									 (VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler022);
	}
	
	Ddim_Print(( "%s\n", __FUNCTION__ ));
}

VOID CT_Dd_PDM_3_022(VOID)
{
	// Clock:22.5792MHz(AUCLK)
	// FS:44.1kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_22_5792;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(4410*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler022);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 016 : PDM0 Clock:22.5792MHz(AUCLK) FS=44.1kHz test\n"));
}

VOID CT_Dd_PDM_3_023(VOID)
{
	// Clock:11.2896MHz(AUCLK/2)
	// FS:44.1kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_22_5792;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(4410*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler022);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 017 : PDM0 Clock:11.2896MHz(AUCLK/2) FS=44.1kHz test\n"));
}

static VOID ct_dd_pdm_DMA_Int_Handler024( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	
	if(gCt_Dd_Pdm_Dma_Int_Cnt < 10) {
		Dd_HDMAC0_Input_Audio_Async( 0,
									 D_DD_HDMAC0_IS_IDREQ_8,
									 Dd_Audio_Get_Addr_Reg_AUIDLR(4),
									 CT_DD_PDM_WORK_AREA+(2205*2*2*gCt_Dd_Pdm_Dma_Int_Cnt),
									 (2205*2*2),
									 (VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler024);
	}
	
	Ddim_Print(( "%s\n", __FUNCTION__ ));
}

VOID CT_Dd_PDM_3_024(VOID)
{
	// Clock:22.5792MHz(AUCLK)
	// FS:22.05kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_22_5792;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_4;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(2205*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler024);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 018 : PDM0 Clock:22.5792MHz(AUCLK) FS=22.05kHz test\n"));
}

VOID CT_Dd_PDM_3_025(VOID)
{
	// Clock:11.2896MHz(AUCLK/2)
	// FS:22.05kHz
	
	UINT8 ch = 0;
	T_DD_AUDIO_CTRL_COMMON	ctrl_common;
	T_DD_AUDIO_CTRL_IN	ctrl_in;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	T_DD_AUDIO_I2S_IN	i2s_in;
	T_DD_AUDIO_DMA	dma;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	if(Dd_HDMAC0_Open(0, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Open Error(ch0)\n"));
	}
	
	Dd_Audio_Init();
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_22_5792;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_4;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Audio_Get_Ctrl_I2sIn(4, &i2s_in);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(4, &i2s_in);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Audio_Get_Ctrl_Common(4, &ctrl_common);
	
	ctrl_common.fifo_usage	= E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel		= E_DD_AUDIO_CHANNEL_STEREO;
	
	Dd_Audio_Ctrl_Common(4, &ctrl_common);
	
	Dd_Audio_Get_Ctrl_Input(4, &ctrl_in);
	
	ctrl_in.format		= E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	ctrl_in.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_in.bit_shift	= E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(4, &ctrl_in);
	
	Dd_Audio_Get_Ctrl_DmaIn(4, &dma);
	
	dma.dma_2ch			= D_DD_AUDIO_DISABLE;
	dma.dma_trnsf_cnt	= 0;
	
	Dd_Audio_Ctrl_DmaIn(4, &dma);
	
	Dd_Audio_Set_SwapHW_AUIDLR(4, D_DD_AUDIO_ENABLE);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 0;
	Dd_HDMAC0_Input_Audio_Async(0,
								D_DD_HDMAC0_IS_IDREQ_8,
								Dd_Audio_Get_Addr_Reg_AUIDLR(4),
								CT_DD_PDM_WORK_AREA,
								(2205*2*2),
								(VP_CALLBACK)ct_dd_pdm_DMA_Int_Handler024);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_ENABLE);
	
	Dd_Audio_Start_Input(4);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Audio_Stop_Input(4);
	
	Dd_Audio_Set_InputDMARequestEnable(4, D_DD_AUDIO_DISABLE);
	
	if(Dd_HDMAC0_Stop(0) != D_DDIM_OK) {
		Ddim_Print(("Dd_HDMAC0_Stop Error(ch0)\n"));
	}
	
	Dd_HDMAC0_Close(0);
	
	Dd_Audio_Close_Input(4);
	
	Dd_Pdm_Stop_Streaming(ch);
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 019 : PDM0 Clock:11.2896MHz(AUCLK/2) FS=22.05kHz test\n"));
}

static VOID ct_dd_pdm_DMA_Int_Handler026_0( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	Dd_Pdm_Set_DMA0_Dst_Addr(0, CT_DD_PDM_WORK_AREA+(0x400 * 4 * gCt_Dd_Pdm_Dma_Int_Cnt));
	Dd_Pdm_Set_EnableDma1Intr(0, D_DD_PDM_ENABLE);
	
	Ddim_Print(( "%s\n", __FUNCTION__ ));
}

static VOID ct_dd_pdm_DMA_Int_Handler026_1( VOID )
{
	gCt_Dd_Pdm_Dma_Int_Cnt++;
	Dd_Pdm_Set_DMA1_Dst_Addr(0, CT_DD_PDM_WORK_AREA+(0x400 * 4 * gCt_Dd_Pdm_Dma_Int_Cnt));
	Dd_Pdm_Set_EnableDma0Intr(0, D_DD_PDM_ENABLE);
	
	Ddim_Print(( "%s\n", __FUNCTION__ ));
}

VOID CT_Dd_PDM_3_026(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	
	// Select PDM_CLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(4);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 020 : PDM0(DMA) Clock:24.576MHz(PDM_CLK) FS:48kHz test\n"));
}

VOID CT_Dd_PDM_3_027(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);

	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	

	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Dd_Audio_Close_Input(4);
	
	Ddim_Print(("> pdm(ES3) 021 : PDM0(DMA) Clock:24.576MHz(AUCLK) FS:48kHz test\n"));
}

VOID CT_Dd_PDM_3_028(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 022 : PDM0(DMA) Clock:12.288MHz(AUCLK/2) FS:48kHz test\n"));
}

VOID CT_Dd_PDM_3_029(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	
	// Select PDM_CLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(4);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 24;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(ch);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	

	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 023 : PDM0(DMA) Clock:24.576MHz(PDM_CLK) FS:32kHz test\n"));
}

VOID CT_Dd_PDM_3_030(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_8_192;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);

	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Dd_Audio_Close_Input(4);
	
	Ddim_Print(("> pdm(ES3) 024 : PDM0(DMA) Clock:8.192MHz(AUCLK) FS:32kHz test\n"));
}

VOID CT_Dd_PDM_3_031(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	
	// Select PDM_CLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(4);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(ch);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(0, &pdm_dma_cfg);
	

	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 025 : PDM0(DMA) Clock:24.576MHz(PDM_CLK) FS:24kHz test\n"));
}

VOID CT_Dd_PDM_3_032(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);

	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	

	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Dd_Audio_Close_Input(4);
	
	Ddim_Print(("> pdm(ES3) 026 : PDM0(DMA) Clock:24.576MHz(AUCLK) FS:24kHz test\n"));
}


VOID CT_Dd_PDM_3_033(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	

	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 027 : PDM0(DMA) Clock:12.288MHz(AUCLK/2) FS:24kHz test\n"));
}

VOID CT_Dd_PDM_3_034(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	
	// Select PDM_CLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(4);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 48;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(ch);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(0, &pdm_dma_cfg);
	

	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 028 : PDM0(DMA) Clock:24.576MHz(PDM_CLK) FS:16kHz test\n"));
}

VOID CT_Dd_PDM_3_035(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);

	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 48;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	

	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Dd_Audio_Close_Input(4);
	
	Ddim_Print(("> pdm(ES3) 029 : PDM0(DMA) Clock:24.576MHz(AUCLK) FS:16kHz test\n"));
}

VOID CT_Dd_PDM_3_036(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 48;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	

	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 030 : PDM0(DMA) Clock:12.288MHz(AUCLK/2) FS:16kHz test\n"));
}

VOID CT_Dd_PDM_3_037(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_8_192;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);

	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Dd_Audio_Close_Input(4);
	
	Ddim_Print(("> pdm(ES3) 031 : PDM0(DMA) Clock:8.192MHz(AUCLK) FS:16kHz test\n"));
}

VOID CT_Dd_PDM_3_038(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 12;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	

	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 032 : PDM0(DMA) Clock:12.288MHz(AUCLK/2) FS:8kHz test\n"));
}

VOID CT_Dd_PDM_3_039(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_8_192;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);

	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	

	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Dd_Audio_Close_Input(4);
	
	Ddim_Print(("> pdm(ES3) 033 : PDM0(DMA) Clock:8.192MHz(AUCLK) FS:8kHz test\n"));
}

VOID CT_Dd_PDM_3_040(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_8_192;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);

	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize = 0x400;
	pdm_len.tsize = 0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	

	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Dd_Audio_Close_Input(4);
	
	Ddim_Print(("> pdm(ES3) 034 : PDM0(DMA) Clock:4.096MHz(AUCLK) FS:8kHz test\n"));
}

VOID CT_Dd_PDM_3_041(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_22_5792;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);

	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Dd_Audio_Close_Input(4);
	
	Ddim_Print(("> pdm(ES3) 035 : PDM0(DMA) Clock:22.5792MHz(AUCLK) FS:44.1kHz test\n"));
}

VOID CT_Dd_PDM_3_042(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_22_5792;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 16;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	

	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 036 : PDM0(DMA) Clock:11.2896MHz(AUCLK/2) FS:44.1kHz test\n"));
}

VOID CT_Dd_PDM_3_043(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK
	Dd_Top_Set_CLKSEL7_PDM0SEL(5);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_22_5792;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);

	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 8;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize =0x400;
	pdm_len.tsize =0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	DDIM_User_Dly_Tsk(2000);

	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Dd_Audio_Close_Input(4);
	
	Ddim_Print(("> pdm(ES3) 037 : PDM0(DMA) Clock:22.5792MHz(AUCLK) FS:22.05kHz test\n"));
}

VOID CT_Dd_PDM_3_044(VOID)
{
	UINT8 ch = 0;
	T_DD_PDM_CORE_CFG pdm_cfg;
	T_DD_PDM_DMA_CFG pdm_dma_cfg;
	T_DD_PDM_DMA_LEN pdm_len;
	T_DD_AUDIO_I2S_CMMN	i2s_common;
	
	// Select AUCLK/2
	Dd_Top_Set_CLKSEL7_PDM0SEL(6);
	
	Dd_Audio_Init();
	
	Dd_Audio_Open_Input(4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Get_Ctrl_I2sCmmn(4, &i2s_common);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_22_5792;
	i2s_common.div_aumclko =  E_DD_AUDIO_AUMCLKO_DIV_2;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_4;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(4, &i2s_common);
	
	Dd_Pdm_Init();
	
	if(Dd_Pdm_Open(ch, D_DDIM_USER_SEM_WAIT_FEVR) != D_DDIM_OK) {
		Ddim_Print(("Dd_Pdm_Open Error(ch0)\n"));
	}
	
	Dd_Pdm_Get_Ctrl_Core(ch, &pdm_cfg);
	pdm_cfg.mclk_div = 4;
	pdm_cfg.sinc_rate = 32;
	pdm_cfg.pga_r = 0;
	pdm_cfg.pga_l = 0;
	
	Dd_Pdm_Ctrl_Core(ch, &pdm_cfg);
	
	Dd_Pdm_Get_Ctrl_Dma(ch, &pdm_dma_cfg);
	pdm_dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_LEFT;
	pdm_dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_16;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	Dd_Pdm_Start_Streaming(ch);
	
	Dd_Pdm_Set_CallbackDma0Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_0);
	Dd_Pdm_Set_EnableDma0Intr(ch, D_DD_PDM_ENABLE);
	Dd_Pdm_Set_CallbackDma1Intr(ch, (T_DD_PDM_CALLBACK)ct_dd_pdm_DMA_Int_Handler026_1);
	Dd_Pdm_Set_EnableDma1Intr(ch, D_DD_PDM_ENABLE);
	
	Dd_Pdm_Flush_Dma_Fifo(0);
	
	Dd_Pdm_Set_DMA0_Dst_Addr(ch, CT_DD_PDM_WORK_AREA);
	Dd_Pdm_Set_DMA1_Dst_Addr(ch, CT_DD_PDM_WORK_AREA+(0x400 * 4));
	
	pdm_len.ttsize = 0x400;
	pdm_len.tsize = 0x10;
	
	Dd_Pdm_Set_Dma_TransLength(ch, &pdm_len);
	
	gCt_Dd_Pdm_Dma_Int_Cnt = 1;
	
	pdm_dma_cfg.dma_en = D_DD_PDM_ENABLE;
	Dd_Pdm_Ctrl_Dma(ch, &pdm_dma_cfg);
	
	DDIM_User_Dly_Tsk(2000);
	
	Dd_Pdm_Stop_Streaming(ch);
	
	Dd_Pdm_Close(ch);
	
	Ddim_Print(("> pdm(ES3) 038 : PDM0(DMA) Clock:11.2896MHz(AUCLK/2) FS:22.05kHz test\n"));
}

// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/**
 * @brief		Command processing of dd_pdm_if.h
 * @param[in]	INT32 argc
 * @param[in]	CHAR** argv
 * @return		None
 * @note		Command processing of MBch.
 * @attention	None
 */
VOID Ct_Dd_Pdm_Main(int argc, char** argv)
{
	Dd_CACHE_L1_Clean_Flush_Dcache_All();
	Dd_CACHE_L2_Clean_Flush_Dcache_All();
	
	if(atoi(argv[1]) == 1) {
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	/* Select PDM CLK */
	Dd_Top_Set_CLKSEL7_PDM0SEL(4);
	Dd_Top_Set_CLKSEL7_PDM1SEL(4);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		switch (atoi(argv[2])) {
			case 1:
				CT_Dd_Pdm_OpenTest1(0);
				break;
			case 2:
				CT_Dd_Pdm_OpenTest2(0);
				break;
			case 3:
				CT_Dd_Pdm_OpenTest3(0);
				break;
			case 4:
				CT_Dd_Pdm_OpenTest1(1);
				break;
			case 5:
				CT_Dd_Pdm_OpenTest2(1);
				break;
			case 6:
				CT_Dd_Pdm_OpenTest3(1);
				break;
			case 7:
				CT_Dd_Pdm_CloseTest(0);
				break;
			case 8:
				CT_Dd_Pdm_CloseTest(1);
				break;
			case 9:
				CT_Dd_Pdm_Dma0_Int_HandlerTest(0);
				break;
			case 10:
				CT_Dd_Pdm_Dma0_Int_HandlerTest(1);
				break;
			case 11:
				CT_Dd_Pdm_Dma1_Int_HandlerTest(0);
				break;
			case 12:
				CT_Dd_Pdm_Dma1_Int_HandlerTest(1);
				break;
			case 13:
				CT_Dd_Pdm_OverFlow_Int_HandlerTest(0);
				break;
			case 14:
				CT_Dd_Pdm_OverFlow_Int_HandlerTest(1);
				break;
			case 15:
				CT_Dd_Pdm_InitTest();
				break;
			case 16:
				CT_Dd_Pdm_Ctrl_CoreTest(0);
				break;
			case 17:
				CT_Dd_Pdm_Ctrl_CoreTest(1);
				break;
			case 18:
				Dd_Pdm_Init();
				CT_Dd_Pdm_Get_Ctrl_CoreTest(0);
				break;
			case 19:
				Dd_Pdm_Init();
				CT_Dd_Pdm_Get_Ctrl_CoreTest(1);
				break;
			case 20:
				CT_Dd_Pdm_Start_StreamingTest(0);
				break;
			case 21:
				CT_Dd_Pdm_Start_StreamingTest(1);
				break;
			case 22:
				CT_Dd_Pdm_Stop_StreamingTest(0);
				break;
			case 23:
				CT_Dd_Pdm_Stop_StreamingTest(1);
				break;
			case 24:
				CT_Dd_Pdm_Ctrl_DmaTest(0);
				break;
			case 25:
				CT_Dd_Pdm_Ctrl_DmaTest(1);
				break;
			case 26:
				Dd_Pdm_Init();
				CT_Dd_Pdm_Get_Ctrl_DmaTest(0);
				break;
			case 27:
				Dd_Pdm_Init();
				CT_Dd_Pdm_Get_Ctrl_DmaTest(1);
				break;
			case 28:
				CT_Dd_Pdm_Set_CallbackDma0IntrTest(0);
				break;
			case 29:
				CT_Dd_Pdm_Set_CallbackDma0IntrTest(1);
				break;
			case 30:
				CT_Dd_Pdm_Set_CallbackDma1IntrTest(0);
				break;
			case 31:
				CT_Dd_Pdm_Set_CallbackDma1IntrTest(1);
				break;
			case 32:
				CT_Dd_Pdm_Set_CallbackOverFlowIntrTest(0);
				break;
			case 33:
				CT_Dd_Pdm_Set_CallbackOverFlowIntrTest(1);
				break;
			case 34:
				CT_Dd_Pdm_Set_EnableDma0IntrTest1(0);
				break;
			case 35:
				CT_Dd_Pdm_Set_EnableDma0IntrTest2(1);
				break;
			case 36:
				CT_Dd_Pdm_Set_EnableDma1IntrTest1(0);
				break;
			case 37:
				CT_Dd_Pdm_Set_EnableDma1IntrTest2(1);
				break;
			case 38:
				CT_Dd_Pdm_Set_EnableOverFlowIntrTest1(0);
				break;
			case 39:
				CT_Dd_Pdm_Set_EnableOverFlowIntrTest2(1);
				break;
			case 40:
				CT_Dd_Pdm_Set_DMA0_Dst_AddrTest(0);
				break;
			case 41:
				CT_Dd_Pdm_Set_DMA0_Dst_AddrTest(1);
				break;
			case 42:
				CT_Dd_Pdm_Set_DMA1_Dst_AddrTest(0);
				break;
			case 43:
				CT_Dd_Pdm_Set_DMA1_Dst_AddrTest(1);
				break;
			case 44:
				CT_Dd_Pdm_Set_Dma_TransLengthTest(0);
				break;
			case 45:
				CT_Dd_Pdm_Set_Dma_TransLengthTest(1);
				break;
			case 46:
				CT_Dd_Pdm_Flush_Dma_FifoTest(0);
				break;
			case 47:
				CT_Dd_Pdm_Flush_Dma_FifoTest(1);
				break;
			case 48:
				CT_Dd_Pdm_Get_StatusTest1(0);
				break;
			case 49:
				CT_Dd_Pdm_Get_StatusTest2(0);
				break;
			case 50:
				CT_Dd_Pdm_Get_StatusTest3(0);
				break;
			case 51:
				CT_Dd_Pdm_Get_StatusTest1(1);
				break;
			case 52:
				CT_Dd_Pdm_Get_StatusTest2(1);
				break;
			case 53:
				CT_Dd_Pdm_Get_StatusTest3(1);
				break;
			case 54:
				CT_Dd_Pdm_Clear_StatusTest1(0);
				break;
			case 55:
				CT_Dd_Pdm_Clear_StatusTest2(0);
				break;
			case 56:
				CT_Dd_Pdm_Clear_StatusTest3(0);
				break;
			case 57:
				CT_Dd_Pdm_Clear_StatusTest1(1);
				break;
			case 58:
				CT_Dd_Pdm_Clear_StatusTest2(1);
				break;
			case 59:
				CT_Dd_Pdm_Clear_StatusTest3(1);
				break;
			default :
				Ddim_Print(("ddpdm : ERROR command line\n"));
				break;
		}
	}
	else if(atoi(argv[1]) == 2) {
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	/* Select PDM CLK */
	Dd_Top_Set_CLKSEL7_PDM0SEL(4);
	Dd_Top_Set_CLKSEL7_PDM1SEL(4);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

		switch (atoi(argv[2])) {
			case 1:
				CT_Dd_Pdm_OpenTest1(2);
				break;
			case 2:
				CT_Dd_Pdm_OpenTest_e1(0);
				break;
			case 3:
				CT_Dd_Pdm_OpenTest_e2(0);
				break;
			case 4:
				CT_Dd_Pdm_CloseTest(2);
				break;
			case 5:
				CT_Dd_Pdm_Dma0_Int_HandlerTest(2);
				break;
			case 6:
				CT_Dd_Pdm_Dma1_Int_HandlerTest(2);
				break;
			case 7:
				CT_Dd_Pdm_OverFlow_Int_HandlerTest(2);
				break;
			case 8:
				CT_Dd_Pdm_Ctrl_CoreTest(2);
				break;
			case 9:
				CT_Dd_Pdm_Ctrl_CoreTest_e1(0);
				break;
			case 10:
				CT_Dd_Pdm_Ctrl_CoreTest_e2(0);
				break;
			case 11:
				CT_Dd_Pdm_Ctrl_CoreTest_e3(0);
				break;
			case 12:
				CT_Dd_Pdm_Ctrl_CoreTest_e4(0);
				break;
			case 13:
				CT_Dd_Pdm_Ctrl_CoreTest_e5(0);
				break;
			case 14:
				CT_Dd_Pdm_Ctrl_CoreTest_e6(0);
				break;
			case 15:
				CT_Dd_Pdm_Ctrl_CoreTest_e7(0);
				break;
			case 16:
				CT_Dd_Pdm_Ctrl_CoreTest_e8(0);
				break;
			case 17:
				CT_Dd_Pdm_Ctrl_CoreTest_e9(0);
				break;
			case 18:
				CT_Dd_Pdm_Get_Ctrl_CoreTest(2);
				break;
			case 19:
				CT_Dd_Pdm_Get_Ctrl_CoreTest_e1(0);
				break;
			case 20:
				CT_Dd_Pdm_Start_StreamingTest(2);
				break;
			case 21:
				CT_Dd_Pdm_Stop_StreamingTest(2);
				break;
			case 22:
				CT_Dd_Pdm_Ctrl_DmaTest(2);
				break;
			case 23:
				CT_Dd_Pdm_Ctrl_DmaTest_e1(0);
				break;
			case 24:
				CT_Dd_Pdm_Ctrl_DmaTest_e2(0);
				break;
			case 25:
				CT_Dd_Pdm_Ctrl_DmaTest_e3(0);
				break;
			case 26:
				CT_Dd_Pdm_Ctrl_DmaTest_e4(0);
				break;
			case 27:
				CT_Dd_Pdm_Ctrl_DmaTest_e5(0);
				break;
			case 28:
				CT_Dd_Pdm_Ctrl_DmaTest_e6(0);
				break;
			case 29:
				CT_Dd_Pdm_Get_Ctrl_DmaTest(2);
				break;
			case 30:
				CT_Dd_Pdm_Get_Ctrl_DmaTest_e1(0);
				break;
			case 31:
				CT_Dd_Pdm_Set_CallbackDma0IntrTest(2);
				break;
			case 32:
				CT_Dd_Pdm_Set_CallbackDma1IntrTest(2);
				break;
			case 33:
				CT_Dd_Pdm_Set_CallbackOverFlowIntrTest(2);
				break;
			case 34:
				CT_Dd_Pdm_Set_EnableDma0IntrTest1(2);
				break;
			case 35:
				CT_Dd_Pdm_Set_EnableDma0IntrTest_e1(0);
				break;
			case 36:
				CT_Dd_Pdm_Set_EnableDma1IntrTest1(2);
				break;
			case 37:
				CT_Dd_Pdm_Set_EnableDma1IntrTest_e1(0);
				break;
			case 38:
				CT_Dd_Pdm_Set_EnableOverFlowIntrTest1(2);
				break;
			case 39:
				CT_Dd_Pdm_Set_EnableOverFlowIntrTest_e1(0);
				break;
			case 40:
				CT_Dd_Pdm_Set_DMA0_Dst_AddrTest(2);
				break;
			case 41:
				CT_Dd_Pdm_Set_DMA1_Dst_AddrTest(2);
				break;
			case 42:
				CT_Dd_Pdm_Set_Dma_TransLengthTest(2);
				break;
			case 43:
				CT_Dd_Pdm_Set_Dma_TransLengthTest_e1(0);
				break;
			case 44:
				CT_Dd_Pdm_Set_Dma_TransLengthTest_e2(0);
				break;
			case 45:
				CT_Dd_Pdm_Set_Dma_TransLengthTest_e3(0);
				break;
			case 46:
				CT_Dd_Pdm_Set_Dma_TransLengthTest_e4(0);
				break;
			case 47:
				CT_Dd_Pdm_Flush_Dma_FifoTest(2);
				break;
			case 48:
				CT_Dd_Pdm_Get_StatusTest1(2);
				break;
			case 49:
				CT_Dd_Pdm_Get_StatusTest_e1(0);
				break;
			case 50:
				CT_Dd_Pdm_Clear_StatusTest1(2);
				break;
			default :
				Ddim_Print(("ddpdm : ERROR command line\n"));
				break;
		}
	}
	else if(atoi(argv[1]) == 3) {
		
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		DDIM_User_EnableInt((DDIM_USER_INTID)(E_DD_GIC_INTID_PDM_CH0_DMA0_INT), (DDIM_USER_INTID)D_DD_GIC_PRI08);
		DDIM_User_EnableInt((DDIM_USER_INTID)(E_DD_GIC_INTID_PDM_CH0_DMA1_INT), (DDIM_USER_INTID)D_DD_GIC_PRI08);
		DDIM_User_EnableInt((DDIM_USER_INTID)(E_DD_GIC_INTID_PDM_CH0_FIFO_OVERFLOW_INT), (DDIM_USER_INTID)D_DD_GIC_PRI08);
		DDIM_User_EnableInt((DDIM_USER_INTID)(E_DD_GIC_INTID_PDM_CH1_DMA0_INT), (DDIM_USER_INTID)D_DD_GIC_PRI08);
		DDIM_User_EnableInt((DDIM_USER_INTID)(E_DD_GIC_INTID_PDM_CH1_DMA1_INT), (DDIM_USER_INTID)D_DD_GIC_PRI08);
		DDIM_User_EnableInt((DDIM_USER_INTID)(E_DD_GIC_INTID_PDM_CH1_FIFO_OVERFLOW_INT), (DDIM_USER_INTID)D_DD_GIC_PRI08);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		
		switch (atoi(argv[2])) {
			case 0:
				CT_Dd_PDM_3_Reg_RW();
				break;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			case 1:
				CT_Dd_PDM_3_001();
				break;
			case 2:
				CT_Dd_PDM_3_002();
				break;
			case 3:
				CT_Dd_PDM_3_003();
				break;
			case 4:
				CT_Dd_PDM_3_004();
				break;
			case 5:
				CT_Dd_PDM_3_005();
				break;
			case 6:
				CT_Dd_PDM_3_006();
				break;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
			case 7:
				CT_Dd_PDM_3_007();
				break;
			case 8:
				CT_Dd_PDM_3_008();
				break;
			case 9:
				CT_Dd_PDM_3_009();
				break;
			case 10:
				CT_Dd_PDM_3_010();
				break;
			case 11:
				CT_Dd_PDM_3_011();
				break;
			case 12:
				CT_Dd_PDM_3_012();
				break;
			case 13:
				CT_Dd_PDM_3_013();
				break;
			case 14:
				CT_Dd_PDM_3_014();
				break;
			case 15:
				CT_Dd_PDM_3_015();
				break;
			case 16:
				CT_Dd_PDM_3_016();
				break;
			case 17:
				CT_Dd_PDM_3_017();
				break;
			case 18:
				CT_Dd_PDM_3_018();
				break;
			case 19:
				CT_Dd_PDM_3_019();
				break;
			case 20:
				CT_Dd_PDM_3_020();
				break;
			case 21:
				CT_Dd_PDM_3_021();
				break;
			case 22:
				CT_Dd_PDM_3_022();
				break;
			case 23:
				CT_Dd_PDM_3_023();
				break;
			case 24:
				CT_Dd_PDM_3_024();
				break;
			case 25:
				CT_Dd_PDM_3_025();
				break;
			case 26:
				CT_Dd_PDM_3_026();
				break;
			case 27:
				CT_Dd_PDM_3_027();
				break;
			case 28:
				CT_Dd_PDM_3_028();
				break;
			case 29:
				CT_Dd_PDM_3_029();
				break;
			case 30:
				CT_Dd_PDM_3_030();
				break;
			case 31:
				CT_Dd_PDM_3_031();
				break;
			case 32:
				CT_Dd_PDM_3_032();
				break;
			case 33:
				CT_Dd_PDM_3_033();
				break;
			case 34:
				CT_Dd_PDM_3_034();
				break;
			case 35:
				CT_Dd_PDM_3_035();
				break;
			case 36:
				CT_Dd_PDM_3_036();
				break;
			case 37:
				CT_Dd_PDM_3_037();
				break;
			case 38:
				CT_Dd_PDM_3_038();
				break;
			case 39:
				CT_Dd_PDM_3_039();
				break;
			case 40:
				CT_Dd_PDM_3_040();
				break;
			case 41:
				CT_Dd_PDM_3_041();
				break;
			case 42:
				CT_Dd_PDM_3_042();
				break;
			case 43:
				CT_Dd_PDM_3_043();
				break;
			case 44:
				CT_Dd_PDM_3_044();
				break;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
			default :
				Ddim_Print(("ddpdm : ERROR command line\n"));
				break;
		}
	}
	else {
		Ddim_Print(("ddpdm : ERROR command line\n"));
	}
	
	Dd_CACHE_L1_Clean_Flush_Dcache_All();
	Dd_CACHE_L2_Clean_Flush_Dcache_All();
}

