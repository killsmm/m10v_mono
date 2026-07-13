/**
 * @file		os_resource_id.c
 * @brief		OS Resource ID
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "TKernel_id.h"
#include "ddim_user_custom.h"
#ifndef CO_DEBUG_ON_PC
#include "mw_user_custom.h"
#else
#include "mw_user_custom_stub.h"
#endif

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
void OS_Resource_ID_Init( void )
{
	///////////////////
	//  DDIM
	///////////////////
	// Event Flag
	// delete basefw\fj\imgproc\still
	gFid_sen  = FID_SEN;								/**< SEN Event Flag */
	//gFid_sro1 = FID_STILL_SRO1_INT;						/**< SRO1 Event Flag */
	//gFid_sro2 = FID_STILL_SRO2_INT;						/**< SRO2 Event Flag */
	//gFid_b2b1 = FID_STILL_B2B1_INT;						/**< B2B1 Event Flag */
	//gFid_b2b2 = FID_STILL_B2B2_INT;						/**< B2B2 Event Flag */
	//gFid_stat = FID_STILL_STAT_INT;						/**< STAT Event Flag */
	//gFid_pas  = FID_STILL_PAS_INT;						/**< PAS Event Flag */
	gFid_b2r = FID_B2R;									/**< B2R Event Flag */
	gFid_ltm_rbk = FID_LTM_RBK;							/**< LTM RBK Event Flag */
	gFid_ltm_map = FID_LTM_MAP;							/**< LTM MAP Event Flag */
	gFid_ltm_ltm = FID_LTM_LTM;							/**< LTM LTM Event Flag */
	gFid_r2y = FID_R2Y;									/**< R2Y Event Flag */
	gFid_disp = FID_DISP;								/**< DISP Event Flag */
	gFid_ela = FID_ELA;									/**< ELA Event Flag */
	gFid_iip = FID_IIP;									/**< IIP Event Flag */
	gFid_iip_unit_lo = FID_IIP_UNIT_LO;					/**< IIP Unit Event Flag 1 of 2 */
	gFid_iip_unit_hi = FID_IIP_UNIT_HI;					/**< IIP Unit Event Flag 2 of 2 */
	gFid_iip_pix = FID_IIP_PIX;							/**< IIP Pixfmttbl Event Flag */
	gFid_iip_res = FID_IIP_RES;							/**< IIP Resource Event Flag */
	gFid_jpeg = FID_JPEG;								/**< JPEG Event Flag */
	gFid_cnr = FID_CNR;									/**< CNR Event Flag */
	gFid_xch = FID_XCH;									/**< Xch Event Flag */
	gFid_shdr = FID_SHDR;								/**< SHDR Event Flag */
	gFid_raw = FID_RAW;									/**< RAW Event Flag */
	gFid_bmh = FID_BMH;									/**< BMH Event Flag */
	gFid_hdmi = FID_HDMI;								/**< HDMI Event Flag */
	gFid_rdma = FID_RDMA;								/**< RDMA Event Flag */
	gFid_dwch = FID_DWCH;								/**< DWCH Event Flag */
	gFid_fpt[0] = FID_FPT0CH;							/**< FPT[0] Event Flag */
	gFid_fpt[1] = FID_FPT1CH;							/**< FPT[1] Event Flag */
	gFid_i2c = FID_I2C;									/**< I2C Event Flag */
	gFid_hdmac = FID_HDMAC;								/**< HDMAC Event Flag */
	gFid_hdmac0 = FID_HDMAC0;							/**< HDMAC0 Event Flag */
	gFid_hdmac1 = FID_HDMAC1;							/**< HDMAC1 Event Flag */
	gFid_xdmac = FID_XDMAC;								/**< XDMAC Event Flag */
	gFid_xdmasnap = FID_XDMAS;							/**< XDMAC SNAP Event Flag */
	gFid_relc = FID_RELC;								/**< RELC Event Flag */
	gFid_me = FID_ME;									/**< ME Event Flag */
	gFid_motor_calc_0 = FID_MOTOR_CALC_0;				/**< Motor Calc_0 Event Flag */
	gFid_motor_calc_1 = FID_MOTOR_CALC_1;				/**< Motor Calc_1 Event Flag */

	// Semaphore
	gSid_cnr_ofl[0] = SID_CNR_OFL_0;					/**< CNR OFL[0] Semaphore ID */
	gSid_cnr_ofl[1] = SID_CNR_OFL_1;					/**< CNR OFL[1] Semaphore ID */
	gSid_cnr_otf[0] = SID_CNR_OTF_0;					/**< CNR OTF[0] Semaphore ID */
	gSid_cnr_otf[1] = SID_CNR_OTF_1;					/**< CNR OTF[1] Semaphore ID */
	gSid_iip = SID_IIP;									/**< IIP Semaphore ID */
	gSid_iip_sw = SID_IIP_SW;							/**< IIP SWTRG Semaphore ID */
	gSid_xch[0] = SID_X0CH;								/**< Xch[0] Semaphore ID */
	gSid_xch[1] = SID_X1CH;								/**< Xch[1] Semaphore ID */
	gSid_jpeg = SID_JPEG;								/**< JPEG Semaphore ID */
	gSid_ela = SID_ELA;									/**< ELA Semaphore ID */
	gSid_raw = SID_RAW;									/**< RAW Semaphore ID */
	gSid_shdr = SID_SHDR;								/**< SHDR Semaphore ID */
	gSid_fpt[0] = SID_FPT0;								/**< FPT[0] Semaphore ID */
	gSid_fpt[1] = SID_FPT1;								/**< FPT[1] Semaphore ID */
	gSid_tmr32[0] = SID_TMR32_0;						/**< 32bit[0] Reload Timer Semaphore ID */
	gSid_tmr32[1] = SID_TMR32_1;						/**< 32bit[1] Reload Timer Semaphore ID */
	gSid_tmr32[2] = SID_TMR32_2;						/**< 32bit[2] Reload Timer Semaphore ID */
	gSid_tmr32[3] = SID_TMR32_3;						/**< 32bit[3] Reload Timer Semaphore ID */
	gSid_tmr32[4] = SID_TMR32_4;						/**< 32bit[4] Reload Timer Semaphore ID */
	gSid_tmr32[5] = SID_TMR32_5;						/**< 32bit[5] Reload Timer Semaphore ID */
	gSid_tmr32[6] = SID_TMR32_6;						/**< 32bit[6] Reload Timer Semaphore ID */
	gSid_tmr32[7] = SID_TMR32_7;						/**< 32bit[7] Reload Timer Semaphore ID */
	gSid_tmr32[8] = SID_TMR32_8;						/**< 32bit[8] Reload Timer Semaphore ID */
	gSid_tmr32[9] = SID_TMR32_9;						/**< 32bit[9] Reload Timer Semaphore ID */
	gSid_tmr32[10] = SID_TMR32_10;						/**< 32bit[10] Reload Timer Semaphore ID */
	gSid_tmr32[11] = SID_TMR32_11;						/**< 32bit[11] Reload Timer Semaphore ID */
	gSid_tmr32[12] = SID_TMR32_12;						/**< 32bit[12] Reload Timer Semaphore ID */
	gSid_tmr32[13] = SID_TMR32_13;						/**< 32bit[13] Reload Timer Semaphore ID */
	gSid_tmr64 = SID_TMR64;								/**< 64bit Timer Semaphore ID */
	gSid_i2c[0] = SID_I2C0CH;							/**< I2C[0] Semaphore ID */
	gSid_i2c[1] = SID_I2C1CH;							/**< I2C[1] Semaphore ID */
	gSid_i2c[2] = SID_I2C2CH;							/**< I2C[2] Semaphore ID */
	gSid_pwm[0] = SID_PWM_0CH;							/**< PWM[0] Semaphore ID */
	gSid_pwm[1] = SID_PWM_1CH;							/**< PWM[1] Semaphore ID */
	gSid_pwm[2] = SID_PWM_2CH;							/**< PWM[2] Semaphore ID */
	gSid_pwm[3] = SID_PWM_3CH;							/**< PWM[3] Semaphore ID */
	gSid_pwm[4] = SID_PWM_4CH;							/**< PWM[4] Semaphore ID */
	gSid_pwm[5] = SID_PWM_5CH;							/**< PWM[5] Semaphore ID */
	gSid_pwm[6] = SID_PWM_6CH;							/**< PWM[6] Semaphore ID */
	gSid_pwm[7] = SID_PWM_7CH;							/**< PWM[7] Semaphore ID */
	gSid_pwm[8] = SID_PWM_8CH;							/**< PWM[8] Semaphore ID */
	gSid_pwm[9] = SID_PWM_9CH;							/**< PWM[9] Semaphore ID */
	gSid_pwm[10] = SID_PWM_10CH;						/**< PWM[10] Semaphore ID */
	gSid_pwm[11] = SID_PWM_11CH;						/**< PWM[11] Semaphore ID */
	gSid_pwm[12] = SID_PWM_12CH;						/**< PWM[12] Semaphore ID */
	gSid_pwm[13] = SID_PWM_13CH;						/**< PWM[13] Semaphore ID */
	gSid_pwm[14] = SID_PWM_14CH;						/**< PWM[14] Semaphore ID */
	gSid_pwm[15] = SID_PWM_15CH;						/**< PWM[15] Semaphore ID */
	gSid_usio[0] = SID_USIO0CH;							/**< UART/CSIO[0] Semaphore ID */
	gSid_usio[1] = SID_USIO1CH;							/**< UART/CSIO[1] Semaphore ID */
	gSid_usio[2] = SID_USIO2CH;							/**< UART/CSIO[2] Semaphore ID */
	gSid_usio[3] = SID_USIO3CH;							/**< UART/CSIO[3] Semaphore ID */
	gSid_usio[4] = SID_USIO4CH;							/**< UART/CSIO[4] Semaphore ID */
	gSid_usio[5] = SID_USIO5CH;							/**< UART/CSIO[5] Semaphore ID */
	gSid_usio[6] = SID_USIO6CH;							/**< UART/CSIO[6] Semaphore ID */
	gSid_usio[7] = SID_USIO7CH;							/**< UART/CSIO[7] Semaphore ID */
	gSid_hdmac[0] = SID_HDMAC_0;						/**< HDMAC[0] Semaphore ID */
	gSid_hdmac[1] = SID_HDMAC_1;						/**< HDMAC[1] Semaphore ID */
	gSid_hdmac[2] = SID_HDMAC_2;						/**< HDMAC[2] Semaphore ID */
	gSid_hdmac[3] = SID_HDMAC_3;						/**< HDMAC[3] Semaphore ID */
	gSid_hdmac0[0] = SID_HDMAC0_0;						/**< HDMAC0[0] Semaphore ID */
	gSid_hdmac0[1] = SID_HDMAC0_1;						/**< HDMAC0[1] Semaphore ID */
	gSid_hdmac0[2] = SID_HDMAC0_2;						/**< HDMAC0[2] Semaphore ID */
	gSid_hdmac0[3] = SID_HDMAC0_3;						/**< HDMAC0[3] Semaphore ID */
	gSid_hdmac0[4] = SID_HDMAC0_4;						/**< HDMAC0[4] Semaphore ID */
	gSid_hdmac0[5] = SID_HDMAC0_5;						/**< HDMAC0[5] Semaphore ID */
	gSid_hdmac0[6] = SID_HDMAC0_6;						/**< HDMAC0[6] Semaphore ID */
	gSid_hdmac0[7] = SID_HDMAC0_7;						/**< HDMAC0[7] Semaphore ID */
	gSid_hdmac1[0] = SID_HDMAC1_0;						/**< HDMAC1[0] Semaphore ID */
	gSid_hdmac1[1] = SID_HDMAC1_1;						/**< HDMAC1[1] Semaphore ID */
	gSid_hdmac1[2] = SID_HDMAC1_2;						/**< HDMAC1[2] Semaphore ID */
	gSid_hdmac1[3] = SID_HDMAC1_3;						/**< HDMAC1[3] Semaphore ID */
	gSid_hdmac1[4] = SID_HDMAC1_4;						/**< HDMAC1[4] Semaphore ID */
	gSid_hdmac1[5] = SID_HDMAC1_5;						/**< HDMAC1[5] Semaphore ID */
	gSid_hdmac1[6] = SID_HDMAC1_6;						/**< HDMAC1[6] Semaphore ID */
	gSid_hdmac1[7] = SID_HDMAC1_7;						/**< HDMAC1[7] Semaphore ID */
	gSid_xdmac[0] = SID_XDMAC0;							/**< XDMAC[0] Semaphore ID */
	gSid_xdmac[1] = SID_XDMAC1;							/**< XDMAC[1] Semaphore ID */
	gSid_xdmac[2] = SID_XDMAC2;							/**< XDMAC[2] Semaphore ID */
	gSid_xdmac[3] = SID_XDMAC3;							/**< XDMAC[3] Semaphore ID */
	gSid_xdmasnap[0] = SID_XDMAS0;						/**< XDMAC SNAP[0] Semaphore ID */
	gSid_xdmasnap[1] = SID_XDMAS1;						/**< XDMAC SNAP[1] Semaphore ID */
	gSid_xdmasnap[2] = SID_XDMAS2;						/**< XDMAC SNAP[2] Semaphore ID */
	gSid_xdmasnap[3] = SID_XDMAS3;						/**< XDMAC SNAP[3] Semaphore ID */
	gSid_timestamp = SID_TIMESTAMP;						/**< TIMESTAMP Semaphore ID */
	gSid_wdog = SID_WDOG;								/**< Watchdog Timer Semaphore ID */
	gSid_udc[0] = SID_UDC0;								/**< UDC[0] Semaphore ID */
	gSid_udc[1] = SID_UDC1;								/**< UDC[1] Semaphore ID */
	gSid_udc[2] = SID_UDC2;								/**< UDC[2] Semaphore ID */
	gSid_udc[3] = SID_UDC3;								/**< UDC[3] Semaphore ID */
	gSid_udc[4] = SID_UDC4;								/**< UDC[4] Semaphore ID */
	gSid_udc[5] = SID_UDC5;								/**< UDC[5] Semaphore ID */
	gSid_audio_if_in[0] = SID_AUDIO_IN0;				/**< AudioI/F Input[0] Semaphore ID */
	gSid_audio_if_in[1] = SID_AUDIO_IN1;				/**< AudioI/F Input[1] Semaphore ID */
	gSid_audio_if_in[2] = SID_AUDIO_IN2;				/**< AudioI/F Input[2] Semaphore ID */
	gSid_audio_if_in[3] = SID_AUDIO_IN3;				/**< AudioI/F Input[3] Semaphore ID */
	gSid_audio_if_in[4] = SID_AUDIO_IN4;				/**< AudioI/F Input[4] Semaphore ID */
	gSid_audio_if_in[5] = SID_AUDIO_IN5;				/**< AudioI/F Input[5] Semaphore ID */
	gSid_audio_if_out[0] = SID_AUDIO_OUT0;				/**< AudioI/F Output[0] Semaphore ID */
	gSid_audio_if_out[1] = SID_AUDIO_OUT1;				/**< AudioI/F Output[1] Semaphore ID */
	gSid_audio_if_out[2] = SID_AUDIO_OUT2;				/**< AudioI/F Output[2] Semaphore ID */
	gSid_audio_if_out[3] = SID_AUDIO_OUT3;				/**< AudioI/F Output[3] Semaphore ID */
	gSid_relc = SID_RELC;								/**< RELC Semaphore ID */
	gSid_pdm[0] = SID_PDM0;								/**< PDM Semaphore ID */
	gSid_pdm[1] = SID_PDM1;								/**< PDM Semaphore ID */
	gSid_pcie = SID_PCIE;								/**< PCIE Semaphore ID */
	gSid_bmh[0] = SID_BMH0;								/**< BMH[0] Semaphore ID */
	gSid_bmh[1] = SID_BMH1;								/**< BMH[1] Semaphore ID */
	gSid_rdma = SID_RDMA;								/**< RDMA Semaphore ID */
	gSid_spi[0] = SID_SPI0;								/**< SPI[0] Semaphore ID */
	gSid_spi[1] = SID_SPI1;								/**< SPI[1] Semaphore ID */
	gSid_spi[2] = SID_SPI2;								/**< SPI[2] Semaphore ID */
	gSid_me = SID_ME;									/**< ME Semaphore ID */
	gSid_slimbus[0] = SID_SLIMBUS0;						/**< SLIMBus[0] Semaphore ID */
	gSid_slimbus[1] = SID_SLIMBUS1;						/**< SLIMBus[1] Semaphore ID */
	gSid_slimbus[2] = SID_SLIMBUS2;						/**< SLIMBus[2] Semaphore ID */
	gSid_slimbus[3] = SID_SLIMBUS3;						/**< SLIMBus[3] Semaphore ID */
	gSid_ipcu[0] = SID_IPCU0;							/**< IPCU[0] Semaphore ID */
	gSid_ipcu[1] = SID_IPCU1;							/**< IPCU[1] Semaphore ID */
	gSid_ipcu[2] = SID_IPCU2;							/**< IPCU[2] Semaphore ID */
	gSid_ipcu[3] = SID_IPCU3;							/**< IPCU[3] Semaphore ID */
	gSid_ipcu[4] = SID_IPCU4;							/**< IPCU[4] Semaphore ID */
	gSid_ipcu[5] = SID_IPCU5;							/**< IPCU[5] Semaphore ID */
	gSid_ipcu[6] = SID_IPCU6;							/**< IPCU[6] Semaphore ID */
	gSid_ipcu[7] = SID_IPCU7;							/**< IPCU[7] Semaphore ID */
	gSid_ipcu[8] = SID_IPCU8;							/**< IPCU[8] Semaphore ID */
	gSid_ipcu[9] = SID_IPCU9;							/**< IPCU[9] Semaphore ID */
	gSid_ipcu[10] = SID_IPCU10;							/**< IPCU[10] Semaphore ID */
	gSid_ipcu[11] = SID_IPCU11;							/**< IPCU[11] Semaphore ID */
	gSid_ipcu[12] = SID_IPCU12;							/**< IPCU[12] Semaphore ID */
	gSid_ipcu[13] = SID_IPCU13;							/**< IPCU[13] Semaphore ID */
	gSid_ipcu[14] = SID_IPCU14;							/**< IPCU[14] Semaphore ID */
	gSid_ipcu[15] = SID_IPCU15;							/**< IPCU[15] Semaphore ID */

	///////////////////
	//  Media
	///////////////////
	// Event Flag
	gSD0_FID_SD = FID_SD0;								/**< SD0 Event Flag definition */
	gSD1_FID_SD = FID_SD1;								/**< SD1 Event Flag definition */
	gMWNF_FID_NF = FID_NF;								/**< NF Event Flagd efinition */
	gEMMC_FID = FID_EMMC;								/**< eMMC Event Flag definition */
	
	// Semaphore
	gSD0_SID_SD = SID_SD0;								/**< SD0 Semaphore ID definition */
	gSD1_SID_SD = SID_SD1;								/**< SD1 Semaphore ID definition */
	gMWNF_SID_NF = SID_NF;								/**< NF Semaphore ID definition */
	gEMMC_SID = SID_EMMC;								/**< eMMC Semaphore ID definition */
}

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

