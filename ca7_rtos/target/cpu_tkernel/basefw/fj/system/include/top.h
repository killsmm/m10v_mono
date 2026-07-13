/**
 * @file		top.h
 * @brief		LSI top control api header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _TOP_H
#define _TOP_H

#include "ddim_typedef.h"
#include "fj_top.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

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
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

extern FJ_ERR_CODE fj_top_start_clock( FJ_TOP_CLOCK_NAME clock_name, UCHAR* counter );
extern FJ_ERR_CODE fj_top_stop_clock( FJ_TOP_CLOCK_NAME clock_name, UCHAR* counter );
extern FJ_ERR_CODE fj_top_set_clock_select_emmc( FJ_TOP_EMMC_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_nf( FJ_TOP_NF_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_sd2u2( FJ_TOP_SD2U2_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_sd2u1( FJ_TOP_SD2U1_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_sd3u1( FJ_TOP_SD3_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_sd1io( FJ_TOP_SD1_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_raw( FJ_TOP_RAW_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_hif( FJ_TOP_HIF_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_mif( FJ_TOP_MIF_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_ipu( FJ_TOP_IPU_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_ipu_tme( FJ_TOP_IPU_TME_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_gpu( FJ_TOP_GPU_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_jpeg( FJ_TOP_JPG_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_ela( FJ_TOP_ELA_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_pipe( FJ_TOP_PIPE_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_sen( FJ_TOP_SEN_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_senmsk( FJ_TOP_SENMSK_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_iip( FJ_TOP_IIP_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_pas( FJ_TOP_PAS_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_ipp( FJ_TOP_IPP_CLKSEL clksel );

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
extern FJ_ERR_CODE fj_top_set_clock_select_pxf( FJ_TOP_PXF_CLKSEL clksel );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
extern FJ_ERR_CODE fj_top_set_clock_select_hevsel( FJ_TOP_HEVSEL_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_hevenc( FJ_TOP_HEVENC_CLKSEL clksel );
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

extern FJ_ERR_CODE fj_top_set_clock_select_vdf( FJ_TOP_VDF_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_sro1_2( FJ_TOP_SRO_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_r2y1( FJ_TOP_R2Y_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_ltm1( FJ_TOP_LTM1_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_b2r1( FJ_TOP_B2R1_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_cnr1( FJ_TOP_CNR1_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_sro1( FJ_TOP_SRO_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_b2b1( FJ_TOP_B2B1_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_ltmrbk1( FJ_TOP_LTMRBK1_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_sro2_2( FJ_TOP_SRO_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_r2y2( FJ_TOP_R2Y_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_ltm2( FJ_TOP_LTM2_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_b2r2( FJ_TOP_B2R2_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_cnr2( FJ_TOP_CNR2_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_sro2( FJ_TOP_SRO_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_b2b2( FJ_TOP_B2B2_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_ltmrbk2( FJ_TOP_LTMRBK2_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_ap3( FJ_TOP_SNAPCLKO_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_ap2( FJ_TOP_SNAPCLKO_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_ap1( FJ_TOP_SNAPCLKO_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_ap0( FJ_TOP_SNAPCLKO_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_apclk( FJ_TOP_APCLK_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_au2( FJ_TOP_AU_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_au3( FJ_TOP_AU_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_au4( FJ_TOP_AU_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_au5( FJ_TOP_AU_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_netau( FJ_TOP_AU_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_spi( FJ_TOP_SPI_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_dsp( FJ_TOP_DSP_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_axi( FJ_TOP_AXI_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_eaxi( FJ_TOP_EAXI_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_bahb( FJ_TOP_BAHB_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_ahb( FJ_TOP_AHB_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_apb( FJ_TOP_APB_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_mxi400( FJ_TOP_MXI400_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_mxi200( FJ_TOP_MXI200_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_axi400( FJ_TOP_AXI400_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_request_clock_select_dchreq( UINT8 req );
extern FJ_ERR_CODE fj_top_set_clock_select_rib( FJ_TOP_RIB_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_nfbch( FJ_TOP_NFBCH_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_me( FJ_TOP_ME_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_fpt1( FJ_TOP_FPT_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_fpt0( FJ_TOP_FPT_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_shdr( FJ_TOP_SHDR_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_gyro( FJ_TOP_GYRO_CLKSEL clksel );
extern FJ_ERR_CODE fj_top_set_clock_select_axi300( FJ_TOP_AXI300_CLKSEL clksel );
extern VOID fj_top_assert_reset_dsp( VOID );
extern VOID fj_top_negate_reset_dsp( VOID );
extern ULONG fj_top_get_reset_state_dsp( VOID );
extern ULONG fj_top_get_clock_core_peripheral( VOID );
extern FJ_ERR_CODE fj_top_set_image_clock( FJ_TOP_IMAGE_CLOCK image_clock );
extern FJ_ERR_CODE fj_top_set_clock_stop_micax0( UCHAR val );

#ifdef __cplusplus
}
#endif

#endif	// _TOP_H
