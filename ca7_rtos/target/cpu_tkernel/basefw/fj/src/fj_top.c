/**
 * @file		fj_top.c
 * @brief		LSI top control api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_top.h"
#include "top.h"

/*--------------------------------------------------------------*/
/* Definition													*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Global Data													*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Extern  Function												*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Local Function												*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Global Function												*/
/*--------------------------------------------------------------*/

FJ_ERR_CODE FJ_Top_Start_Clock( FJ_TOP_CLOCK_NAME clock_name, UCHAR* counter )
{
	return fj_top_start_clock( clock_name, counter );
}

FJ_ERR_CODE FJ_Top_Stop_Clock( FJ_TOP_CLOCK_NAME clock_name, UCHAR* counter )
{
	return fj_top_stop_clock( clock_name, counter );
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_EMMC( FJ_TOP_EMMC_CLKSEL clksel )
{
	fj_top_set_clock_select_emmc( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_NF( FJ_TOP_NF_CLKSEL clksel )
{
	fj_top_set_clock_select_nf( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_SD2U2( FJ_TOP_SD2U2_CLKSEL clksel )
{
	fj_top_set_clock_select_sd2u2( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_SD2U1( FJ_TOP_SD2U1_CLKSEL clksel )
{
	fj_top_set_clock_select_sd2u1( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_SD3U1( FJ_TOP_SD3_CLKSEL clksel )
{
	fj_top_set_clock_select_sd3u1( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_SD1IO( FJ_TOP_SD1_CLKSEL clksel )
{
	fj_top_set_clock_select_sd1io( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_RAW( FJ_TOP_RAW_CLKSEL clksel )
{
	fj_top_set_clock_select_raw( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_HIF( FJ_TOP_HIF_CLKSEL clksel )
{
	fj_top_set_clock_select_hif( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_MIF( FJ_TOP_MIF_CLKSEL clksel )
{
	fj_top_set_clock_select_mif( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_IPU( FJ_TOP_IPU_CLKSEL clksel )
{
	fj_top_set_clock_select_ipu( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_IPU_TME( FJ_TOP_IPU_TME_CLKSEL clksel )
{
	fj_top_set_clock_select_ipu_tme( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_GPU( FJ_TOP_GPU_CLKSEL clksel )
{
	fj_top_set_clock_select_gpu( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_JPEG( FJ_TOP_JPG_CLKSEL clksel )
{
	fj_top_set_clock_select_jpeg( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_ELA( FJ_TOP_ELA_CLKSEL clksel )
{
	fj_top_set_clock_select_ela( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_PIPE( FJ_TOP_PIPE_CLKSEL clksel )
{
	fj_top_set_clock_select_pipe( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_SEN( FJ_TOP_SEN_CLKSEL clksel )
{
	fj_top_set_clock_select_sen( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_SENMSK( FJ_TOP_SENMSK_CLKSEL clksel )
{
	fj_top_set_clock_select_senmsk( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_IIP( FJ_TOP_IIP_CLKSEL clksel )
{
	fj_top_set_clock_select_iip( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_PAS( FJ_TOP_PAS_CLKSEL clksel )
{
	fj_top_set_clock_select_pas( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_IPP( FJ_TOP_IPP_CLKSEL clksel )
{
	fj_top_set_clock_select_ipp( clksel );
	return 0;
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
FJ_ERR_CODE FJ_Top_Set_Clock_Select_PXF( FJ_TOP_PXF_CLKSEL clksel )
{
	fj_top_set_clock_select_pxf( clksel );
	return 0;
}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
FJ_ERR_CODE FJ_Top_Set_Clock_Select_HEVENC( FJ_TOP_HEVENC_CLKSEL clksel )
{
	fj_top_set_clock_select_hevenc( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_HEVSEL( FJ_TOP_HEVSEL_CLKSEL clksel )
{
	fj_top_set_clock_select_hevsel( clksel );
	return 0;
}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---


FJ_ERR_CODE FJ_Top_Set_Clock_Select_VDF( FJ_TOP_VDF_CLKSEL clksel )
{
	fj_top_set_clock_select_vdf( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_SRO1_2( FJ_TOP_SRO_CLKSEL clksel )
{
	fj_top_set_clock_select_sro1_2( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_R2Y1( FJ_TOP_R2Y_CLKSEL clksel )
{
	fj_top_set_clock_select_r2y1( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_LTM1( FJ_TOP_LTM1_CLKSEL clksel )
{
	fj_top_set_clock_select_ltm1( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_B2R1( FJ_TOP_B2R1_CLKSEL clksel )
{
	fj_top_set_clock_select_b2r1( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_CNR1( FJ_TOP_CNR1_CLKSEL clksel )
{
	fj_top_set_clock_select_cnr1( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_SRO1( FJ_TOP_SRO_CLKSEL clksel )
{
	fj_top_set_clock_select_sro1( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_B2B1( FJ_TOP_B2B1_CLKSEL clksel )
{
	fj_top_set_clock_select_b2b1( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_LTMRBK1( FJ_TOP_LTMRBK1_CLKSEL clksel )
{
	fj_top_set_clock_select_ltmrbk1( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_SRO2_2( FJ_TOP_SRO_CLKSEL clksel )
{
	fj_top_set_clock_select_sro2_2( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_R2Y2( FJ_TOP_R2Y_CLKSEL clksel )
{
	fj_top_set_clock_select_r2y2( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_LTM2( FJ_TOP_LTM2_CLKSEL clksel )
{
	fj_top_set_clock_select_ltm2( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_B2R2( FJ_TOP_B2R2_CLKSEL clksel )
{
	fj_top_set_clock_select_b2r2( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_CNR2( FJ_TOP_CNR2_CLKSEL clksel )
{
	fj_top_set_clock_select_cnr2( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_SRO2( FJ_TOP_SRO_CLKSEL clksel )
{
	fj_top_set_clock_select_sro2( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_B2B2( FJ_TOP_B2B2_CLKSEL clksel )
{
	fj_top_set_clock_select_b2b2( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_LTMRBK2( FJ_TOP_LTMRBK2_CLKSEL clksel )
{
	fj_top_set_clock_select_ltmrbk2( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_APCLK( FJ_TOP_APCLK_CLKSEL clksel )
{
	fj_top_set_clock_select_apclk( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_AP3( FJ_TOP_SNAPCLKO_CLKSEL clksel )
{
	fj_top_set_clock_select_ap3( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_AP2( FJ_TOP_SNAPCLKO_CLKSEL clksel )
{
	fj_top_set_clock_select_ap2( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_AP1( FJ_TOP_SNAPCLKO_CLKSEL clksel )
{
	fj_top_set_clock_select_ap1( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_AP0( FJ_TOP_SNAPCLKO_CLKSEL clksel )
{
	fj_top_set_clock_select_ap0( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_NETAU( FJ_TOP_AU_CLKSEL clksel )
{
	fj_top_set_clock_select_netau( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_SPI( FJ_TOP_SPI_CLKSEL clksel )
{
	fj_top_set_clock_select_spi( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_DSP( FJ_TOP_DSP_CLKSEL clksel )
{
	fj_top_set_clock_select_dsp( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_AXI( FJ_TOP_AXI_CLKSEL clksel )
{
	fj_top_set_clock_select_axi( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_EAXI( FJ_TOP_EAXI_CLKSEL clksel )
{
	fj_top_set_clock_select_eaxi( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_BAHB( FJ_TOP_BAHB_CLKSEL clksel )
{
	fj_top_set_clock_select_bahb( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_AHB( FJ_TOP_AHB_CLKSEL clksel )
{
	fj_top_set_clock_select_ahb( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_APB( FJ_TOP_APB_CLKSEL clksel )
{
	fj_top_set_clock_select_apb( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_MXI400( FJ_TOP_MXI400_CLKSEL clksel )
{
	fj_top_set_clock_select_mxi400( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_MXI200( FJ_TOP_MXI200_CLKSEL clksel )
{
	fj_top_set_clock_select_mxi200( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_AXI400( FJ_TOP_AXI400_CLKSEL clksel )
{
	fj_top_set_clock_select_axi400( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Request_Clock_Select_DCHREQ( UINT8 req )
{
	fj_top_request_clock_select_dchreq( req );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_RIB( FJ_TOP_RIB_CLKSEL clksel )
{
	fj_top_set_clock_select_rib( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_NFBCH( FJ_TOP_NFBCH_CLKSEL clksel )
{
	fj_top_set_clock_select_nfbch( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_ME( FJ_TOP_ME_CLKSEL clksel )
{
	fj_top_set_clock_select_me( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_FPT1( FJ_TOP_FPT_CLKSEL clksel )
{
	fj_top_set_clock_select_fpt1( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_FPT0( FJ_TOP_FPT_CLKSEL clksel )
{
	fj_top_set_clock_select_fpt0( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_SHDR( FJ_TOP_SHDR_CLKSEL clksel )
{
	fj_top_set_clock_select_shdr( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_GYRO( FJ_TOP_GYRO_CLKSEL clksel )
{
	fj_top_set_clock_select_gyro( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Clock_Select_AXI300( FJ_TOP_AXI300_CLKSEL clksel )
{
	fj_top_set_clock_select_axi300( clksel );
	return 0;
}

FJ_ERR_CODE FJ_Top_Set_Image_Clock( FJ_TOP_IMAGE_CLOCK image_clock )
{
	return fj_top_set_image_clock( image_clock );
}

VOID FJ_Top_Assert_Reset_DSP( VOID )
{
	fj_top_assert_reset_dsp();
}

VOID FJ_Top_Negate_Reset_DSP( VOID )
{
	fj_top_negate_reset_dsp();
}

ULONG FJ_Top_Get_Reset_State_DSP( VOID )
{
	return fj_top_get_reset_state_dsp();
}

ULONG FJ_Top_Get_Clock_Core_Peripheral( VOID )
{
	return fj_top_get_clock_core_peripheral();
}

FJ_ERR_CODE FJ_Top_Set_Clock_Stop_MICAX0( UCHAR val )
{
	return fj_top_set_clock_stop_micax0( val );
}

