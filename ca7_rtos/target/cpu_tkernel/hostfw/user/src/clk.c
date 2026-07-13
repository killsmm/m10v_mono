/**
 * @file		clk.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "clk.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
const FJ_TOP_IMAGE_CLOCK	IMAGE_CLOCK[ E_IMG_CLOCK_MODE_MAX ] = 
{
	[E_IMG_CLOCK_MODE_NORMAL_VIEW] = {
		// For Normal Mode(All MAX!!!)
		.raw		= FJ_TOP_RAW_CLKSEL_200MHZ,
		.jpg		= FJ_TOP_JPG_CLKSEL_200MHZ,
		.ela		= FJ_TOP_ELA_CLKSEL_300MHZ,
		.sen		= FJ_TOP_SEN_CLKSEL_300MHZ,
		.senmsk		= FJ_TOP_SENMSK_CLKSEL_100MHZ,
		.iip		= FJ_TOP_IIP_CLKSEL_300MHZ,
		.pas		= FJ_TOP_PAS_CLKSEL_300MHZ,
		.sro1		= FJ_TOP_SRO_CLKSEL_300MHZ,
		.sro1_2		= FJ_TOP_SRO_CLKSEL_300MHZ,
		.b2b1		= FJ_TOP_B2B1_CLKSEL_SRO1CLK,
		.b2r1		= FJ_TOP_B2R1_CLKSEL_SRO1CLK,
		.ltm1		= FJ_TOP_LTM1_CLKSEL_SRO1CLK,
		.ltmrbk1	= FJ_TOP_LTMRBK1_CLKSEL_SRO1CLK,
		.r2y1		= FJ_TOP_R2Y_CLKSEL_300MHZ,
		.cnr1		= FJ_TOP_CNR1_CLKSEL_R2Y1CLK,
		.sro2		= FJ_TOP_SRO_CLKSEL_300MHZ,
		.sro2_2		= FJ_TOP_SRO_CLKSEL_300MHZ,
		.b2b2		= FJ_TOP_B2B2_CLKSEL_SRO2CLK,
		.b2r2		= FJ_TOP_B2R2_CLKSEL_SRO2CLK,
		.ltm2		= FJ_TOP_LTM2_CLKSEL_SRO2CLK,
		.ltmrbk2	= FJ_TOP_LTMRBK2_CLKSEL_SRO2CLK,
		.r2y2		= FJ_TOP_R2Y_CLKSEL_300MHZ,
		.cnr2		= FJ_TOP_CNR2_CLKSEL_R2Y1CLK,
		.apclk		= FJ_TOP_APCLK_CLKSEL_72MHZ,
		.apclk0		= FJ_TOP_SNAPCLKO_CLKSEL_PLL0,
		.apclk1		= FJ_TOP_SNAPCLKO_CLKSEL_PLL0,
		.apclk2		= FJ_TOP_SNAPCLKO_CLKSEL_PLL0,
		.apclk3		= FJ_TOP_SNAPCLKO_CLKSEL_PLL0,
#ifdef CO_ES1_HARDWARE
		.ipp		= FJ_TOP_IPP_CLKSEL_800MHZ,
		.pxf		= FJ_TOP_PXF_CLKSEL_700MHZ,
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
		.ipp		= FJ_TOP_IPP_CLKSEL_800MHZ,
		.hevenc		= FJ_TOP_HEVENC_CLKSEL_466MHZ,
		.hevsel		= FJ_TOP_HEVSEL_CLKSEL_500MHZ,
#endif // CO_ES3_HARDWARE
		.vdf		= FJ_TOP_VDF_CLKSEL_700MHZ,
		.rib		= FJ_TOP_RIB_CLKSEL_466MHZ,
		.rib_core	= FJ_TOP_RIB_CORE_2,
		.fpt0		= FJ_TOP_FPT_CLKSEL_300MHZ,
		.fpt1		= FJ_TOP_FPT_CLKSEL_300MHZ,
		.me			= FJ_TOP_ME_CLKSEL_466MHZ,
#ifdef CO_ES1_HARDWARE
		.shdr		= FJ_TOP_SHDR_CLKSEL_400MHZ,
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
		.shdr		= FJ_TOP_SHDR_CLKSEL_432MHZ,
#endif // CO_ES3_HARDWARE
		.ipu		= FJ_TOP_IPU_CLKSEL_400MHZ,
		.ipu_tme	= FJ_TOP_IPU_TME_CLKSEL_500MHZ,
	},
	[E_IMG_CLOCK_MODE_DUAL_STITCH] = {
		// For Stitch Mode (Close Unnecessary Clock)
		.raw		= FJ_TOP_RAW_CLKSEL_OFF,
		.jpg		= FJ_TOP_JPG_CLKSEL_200MHZ,
		.ela		= FJ_TOP_ELA_CLKSEL_OFF,
		.sen		= FJ_TOP_SEN_CLKSEL_300MHZ,
		.senmsk 	= FJ_TOP_SENMSK_CLKSEL_100MHZ,
		.iip		= FJ_TOP_IIP_CLKSEL_300MHZ,
		.pas		= FJ_TOP_PAS_CLKSEL_OFF,
		.sro1		= FJ_TOP_SRO_CLKSEL_300MHZ,
		.sro1_2 	= FJ_TOP_SRO_CLKSEL_300MHZ,
		.b2b1		= FJ_TOP_B2B1_CLKSEL_SRO1CLK,
		.b2r1		= FJ_TOP_B2R1_CLKSEL_SRO1CLK,
		.ltm1		= FJ_TOP_LTM1_CLKSEL_OFF,
		.ltmrbk1	= FJ_TOP_LTMRBK1_CLKSEL_OFF,
		.r2y1		= FJ_TOP_R2Y_CLKSEL_300MHZ,
		.cnr1		= FJ_TOP_CNR1_CLKSEL_R2Y1CLK,
		.sro2		= FJ_TOP_SRO_CLKSEL_300MHZ,
		.sro2_2 	= FJ_TOP_SRO_CLKSEL_300MHZ,
		.b2b2		= FJ_TOP_B2B2_CLKSEL_SRO2CLK,
		.b2r2		= FJ_TOP_B2R2_CLKSEL_SRO2CLK,
		.ltm2		= FJ_TOP_LTM2_CLKSEL_OFF,
		.ltmrbk2	= FJ_TOP_LTMRBK2_CLKSEL_OFF,
		.r2y2		= FJ_TOP_R2Y_CLKSEL_300MHZ,
		.cnr2		= FJ_TOP_CNR2_CLKSEL_R2Y1CLK,
		.apclk		= FJ_TOP_APCLK_CLKSEL_72MHZ,
		.apclk0 	= FJ_TOP_SNAPCLKO_CLKSEL_PLL0,
		.apclk1 	= FJ_TOP_SNAPCLKO_CLKSEL_PLL0,
		.apclk2 	= FJ_TOP_SNAPCLKO_CLKSEL_PLL0,
		.apclk3 	= FJ_TOP_SNAPCLKO_CLKSEL_PLL0,
#ifdef CO_ES1_HARDWARE
		.ipp		= FJ_TOP_IPP_CLKSEL_800MHZ,
		.pxf		= FJ_TOP_PXF_CLKSEL_700MHZ,
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
		.ipp		= FJ_TOP_IPP_CLKSEL_800MHZ,
		.hevenc 	= FJ_TOP_HEVENC_CLKSEL_466MHZ,
		.hevsel 	= FJ_TOP_HEVSEL_CLKSEL_500MHZ,
#endif // CO_ES3_HARDWARE
		.vdf		= FJ_TOP_VDF_CLKSEL_700MHZ,
		.rib		= FJ_TOP_RIB_CLKSEL_466MHZ,
		.rib_core	= FJ_TOP_RIB_CORE_2,
		.fpt0		= FJ_TOP_FPT_CLKSEL_OFF,
		.fpt1		= FJ_TOP_FPT_CLKSEL_OFF,
		.me 		= FJ_TOP_ME_CLKSEL_OFF,
#ifdef CO_ES1_HARDWARE
		.shdr		= FJ_TOP_SHDR_CLKSEL_400MHZ,
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
		.shdr		= FJ_TOP_SHDR_CLKSEL_432MHZ,
#endif // CO_ES3_HARDWARE
		.ipu		= FJ_TOP_IPU_CLKSEL_400MHZ,
		.ipu_tme	= FJ_TOP_IPU_TME_CLKSEL_500MHZ,
	},
	[E_IMG_CLOCK_MODE_PLAY] = {
	// For Play Mode (Close Unnecessary Clock)
		.raw		= FJ_TOP_RAW_CLKSEL_OFF,
		.jpg		= FJ_TOP_JPG_CLKSEL_200MHZ,
		.ela		= FJ_TOP_ELA_CLKSEL_OFF,
		.sen		= FJ_TOP_SEN_CLKSEL_OFF,
		.senmsk 	= FJ_TOP_SENMSK_CLKSEL_OFF,
		.iip		= FJ_TOP_IIP_CLKSEL_300MHZ,
		.pas		= FJ_TOP_PAS_CLKSEL_OFF,
		.sro1		= FJ_TOP_SRO_CLKSEL_OFF,
		.sro1_2 	= FJ_TOP_SRO_CLKSEL_OFF,
		.b2b1		= FJ_TOP_B2B1_CLKSEL_OFF,
		.b2r1		= FJ_TOP_B2R1_CLKSEL_OFF,
		.ltm1		= FJ_TOP_LTM1_CLKSEL_OFF,
		.ltmrbk1	= FJ_TOP_LTMRBK1_CLKSEL_OFF,
		.r2y1		= FJ_TOP_R2Y_CLKSEL_OFF,
		.cnr1		= FJ_TOP_CNR1_CLKSEL_OFF,
		.sro2		= FJ_TOP_SRO_CLKSEL_OFF,
		.sro2_2 	= FJ_TOP_SRO_CLKSEL_OFF,
		.b2b2		= FJ_TOP_B2B2_CLKSEL_OFF,
		.b2r2		= FJ_TOP_B2R2_CLKSEL_OFF,
		.ltm2		= FJ_TOP_LTM2_CLKSEL_OFF,
		.ltmrbk2	= FJ_TOP_LTMRBK2_CLKSEL_OFF,
		.r2y2		= FJ_TOP_R2Y_CLKSEL_OFF,
		.cnr2		= FJ_TOP_CNR2_CLKSEL_OFF,
		.apclk		= FJ_TOP_APCLK_CLKSEL_72MHZ,
		.apclk0 	= FJ_TOP_SNAPCLKO_CLKSEL_OFF,
		.apclk1 	= FJ_TOP_SNAPCLKO_CLKSEL_OFF,
		.apclk2 	= FJ_TOP_SNAPCLKO_CLKSEL_OFF,
		.apclk3 	= FJ_TOP_SNAPCLKO_CLKSEL_OFF,
#ifdef CO_ES1_HARDWARE
		.ipp		= FJ_TOP_IPP_CLKSEL_600MHZ,
		.pxf		= FJ_TOP_PXF_CLKSEL_700MHZ,
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
		.ipp		= FJ_TOP_IPP_CLKSEL_800MHZ,
		.hevenc 	= FJ_TOP_HEVENC_CLKSEL_466MHZ,
		.hevsel 	= FJ_TOP_HEVSEL_CLKSEL_500MHZ,
#endif // CO_ES3_HARDWARE
		.vdf		= FJ_TOP_VDF_CLKSEL_700MHZ,
		.rib		= FJ_TOP_RIB_CLKSEL_466MHZ,
		.rib_core	= FJ_TOP_RIB_CORE_2,
		.fpt0		= FJ_TOP_FPT_CLKSEL_OFF,
		.fpt1		= FJ_TOP_FPT_CLKSEL_OFF,
		.me 		= FJ_TOP_ME_CLKSEL_OFF,
#ifdef CO_ES1_HARDWARE
		.shdr		= FJ_TOP_SHDR_CLKSEL_400MHZ,
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
		.shdr		= FJ_TOP_SHDR_CLKSEL_432MHZ,
#endif // CO_ES3_HARDWARE
		.ipu		= FJ_TOP_IPU_CLKSEL_400MHZ,
		.ipu_tme	= FJ_TOP_IPU_TME_CLKSEL_500MHZ,
	},
	[E_IMG_CLOCK_MODE_IDLE] = {
		// For Idle Mode(All Closed!!!)
		.raw		= FJ_TOP_RAW_CLKSEL_OFF,
		.jpg		= FJ_TOP_JPG_CLKSEL_OFF,
		.ela		= FJ_TOP_ELA_CLKSEL_OFF,
		.sen		= FJ_TOP_SEN_CLKSEL_OFF,
		.senmsk 	= FJ_TOP_SENMSK_CLKSEL_OFF,
		.iip		= FJ_TOP_IIP_CLKSEL_OFF,
		.pas		= FJ_TOP_PAS_CLKSEL_OFF,
		.sro1		= FJ_TOP_SRO_CLKSEL_OFF,
		.sro1_2 	= FJ_TOP_SRO_CLKSEL_OFF,
		.b2b1		= FJ_TOP_B2B1_CLKSEL_OFF,
		.b2r1		= FJ_TOP_B2R1_CLKSEL_OFF,
		.ltm1		= FJ_TOP_LTM1_CLKSEL_OFF,
		.ltmrbk1	= FJ_TOP_LTMRBK1_CLKSEL_OFF,
		.r2y1		= FJ_TOP_R2Y_CLKSEL_OFF,
		.cnr1		= FJ_TOP_CNR1_CLKSEL_OFF,
		.sro2		= FJ_TOP_SRO_CLKSEL_OFF,
		.sro2_2 	= FJ_TOP_SRO_CLKSEL_OFF,
		.b2b2		= FJ_TOP_B2B2_CLKSEL_OFF,
		.b2r2		= FJ_TOP_B2R2_CLKSEL_OFF,
		.ltm2		= FJ_TOP_LTM2_CLKSEL_OFF,
		.ltmrbk2	= FJ_TOP_LTMRBK2_CLKSEL_OFF,
		.r2y2		= FJ_TOP_R2Y_CLKSEL_OFF,
		.cnr2		= FJ_TOP_CNR2_CLKSEL_OFF,
		.apclk		= FJ_TOP_APCLK_CLKSEL_72MHZ,
		.apclk0 	= FJ_TOP_SNAPCLKO_CLKSEL_OFF,
		.apclk1 	= FJ_TOP_SNAPCLKO_CLKSEL_OFF,
		.apclk2 	= FJ_TOP_SNAPCLKO_CLKSEL_OFF,
		.apclk3 	= FJ_TOP_SNAPCLKO_CLKSEL_OFF,
#ifdef CO_ES1_HARDWARE
		.ipp		= FJ_TOP_IPP_CLKSEL_OFF,
		.pxf		= FJ_TOP_PXF_CLKSEL_OFF,
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
		.ipp		= FJ_TOP_IPP_CLKSEL_OFF,
		.hevenc 	= FJ_TOP_HEVENC_CLKSEL_OFF,
		.hevsel 	= FJ_TOP_HEVSEL_CLKSEL_OFF,
#endif // CO_ES3_HARDWARE
		.vdf		= FJ_TOP_VDF_CLKSEL_OFF,
		.rib		= FJ_TOP_RIB_CLKSEL_OFF,
		.rib_core	= FJ_TOP_RIB_CORE_2,
		.fpt0		= FJ_TOP_FPT_CLKSEL_OFF,
		.fpt1		= FJ_TOP_FPT_CLKSEL_OFF,
		.me 		= FJ_TOP_ME_CLKSEL_OFF,
#ifdef CO_ES1_HARDWARE
		.shdr		= FJ_TOP_SHDR_CLKSEL_OFF,
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
		.shdr		= FJ_TOP_SHDR_CLKSEL_OFF,
#endif // CO_ES3_HARDWARE
		.ipu		= FJ_TOP_IPU_CLKSEL_OFF,
		.ipu_tme	= FJ_TOP_IPU_TME_CLKSEL_OFF,
	},
};


/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
