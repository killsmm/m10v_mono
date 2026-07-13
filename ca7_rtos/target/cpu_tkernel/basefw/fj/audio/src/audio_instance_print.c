/**
 * @file		audio_instance_print.c
 * @brief		Audio instance print information module.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdio.h>
#include <string.h>
#include "ddim_typedef.h"
#include "fj_audio.h"
#include "fj_encoder.h"
#include "fj_decoder.h"
#include "audio.h"
#include "audio_source.h"
#include "audio_capture.h"
#include "audio_out.h"
#include "audio_buzzer.h"
#include "audio_encoder.h"
#include "audio_decoder.h"

#include "audio_instance_print.h"
#include "debug.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

VOID BF_Audio_Instance_Print_Info( VOID )
{
	UINT32 loop_src;
	UINT32 loop_cap;
	UINT32 loop_enc;
	UINT32 loop_out;
	UINT32 cap_connect_count;
	UINT32 dec_connect_count;
	
	BF_Debug_Print_Information(( "\n" ));
	BF_Debug_Print_Information(( "<< Audio Instance Information >>\n" ));
	
	// AudioSRC
	for (loop_src = 0; loop_src < D_AUDIO_SRC_INST_NUM; loop_src++){
		BF_Debug_Print_Information(( "\n" ));
		BF_Debug_Print_Information((" [AudioSRC id : %d]", BF_AudioSrc_Get_InstanceInfo(loop_src).audio_src_id ));
		if (BF_AudioSrc_Get_InstanceInfo(loop_src).enable == FJ_AUDIO_ENABLE){
			BF_Debug_Print_Information((" Active\n"));
			BF_Debug_Print_Information(("    Type                       : %7d\n", (int)BF_AudioSrc_Get_InstanceInfo(loop_src).type ));
			BF_Debug_Print_Information(("    [Connectivity]\n"));
			if (BF_AudioSrc_Get_InstanceInfo(loop_src).connect_cap_id != 0xFF){
				BF_Debug_Print_Information(("      Connected AudioCapture id: %7d\n", (int)BF_AudioSrc_Get_InstanceInfo(loop_src).connect_cap_id ));
			}
			else {
				BF_Debug_Print_Information(("      Non Connect\n"));
			}
		}
		else {
			BF_Debug_Print_Information((" Non Active\n"));
		}
	}
	
	// AudioCapture
	for (loop_cap = 0; loop_cap < D_AUDIO_CAPTURE_INST_NUM; loop_cap++){
		BF_Debug_Print_Information(( "\n" ));
		BF_Debug_Print_Information((" [AudioCapture id : %d]", BF_AudioCapture_Get_InstanceInfo(loop_cap).audio_capture_id));
		
		if (BF_AudioCapture_Get_InstanceInfo(loop_cap).enable == FJ_AUDIO_ENABLE){
			BF_Debug_Print_Information((" Active\n"));
			BF_Debug_Print_Information(("    Type                       : %7d\n", BF_AudioCapture_Get_InstanceInfo(loop_cap).type));
			BF_Debug_Print_Information(("    Input sampling rate        : %7d\n", BF_AudioCapture_Get_InstanceInfo(loop_cap).in_smpl_rate));
			BF_Debug_Print_Information(("    Input channel type         : %7d\n", BF_AudioCapture_Get_InstanceInfo(loop_cap).in_ch_type));
			BF_Debug_Print_Information(("    Output sampling rate       : %7d\n", BF_AudioCapture_Get_InstanceInfo(loop_cap).out_smpl_rate));
			BF_Debug_Print_Information(("    Output channel type        : %7d\n", BF_AudioCapture_Get_InstanceInfo(loop_cap).out_ch_type));
			
			if (BF_AudioCapture_Get_InstanceInfo(loop_cap).plug_in1_enable == FJ_AUDIO_ENABLE){
				BF_Debug_Print_Information(("    PlugIn1 enable             :  Enable\n"));
			}
			else {
				BF_Debug_Print_Information(("    PlugIn1 enable             : Disable\n"));
			}
			
			BF_Debug_Print_Information(("    Input port                 : %7d\n", BF_AudioCapture_Get_InstanceInfo(loop_cap).in_port));
			BF_Debug_Print_Information(("    [I2S Information]\n"));
			
			if (BF_AudioCapture_Get_InstanceInfo(loop_cap).i2s_cfg.master_slave == FJ_AUDIO_IF_MASTER){
				BF_Debug_Print_Information(("      Master/Slave             :  Master\n"));
			}
			else {
				BF_Debug_Print_Information(("      Master/Slave             :   Slave\n"));
			}
			
			BF_Debug_Print_Information(("      LR clock                 : %7ld\n", BF_AudioCapture_Get_InstanceInfo(loop_cap).i2s_cfg.lr_clock));
			BF_Debug_Print_Information(("      Data cycle               : %7d\n", BF_AudioCapture_Get_InstanceInfo(loop_cap).i2s_cfg.data_cycle));
			BF_Debug_Print_Information(("      Format                   : %7d\n", BF_AudioCapture_Get_InstanceInfo(loop_cap).i2s_cfg.i2s_format));
			BF_Debug_Print_Information(("      Bit count                : %7d\n", BF_AudioCapture_Get_InstanceInfo(loop_cap).i2s_cfg.bit_count));
			
			BF_Debug_Print_Information(("    [Connectivity]\n"));
			cap_connect_count = 0;
			if (BF_AudioCapture_Get_InstanceInfo(loop_cap).connect_enc_id[0] != 0xFF){
				cap_connect_count++;
				BF_Debug_Print_Information(("      Connected AudioEncoder id: %7d\n", (int)BF_AudioCapture_Get_InstanceInfo(loop_cap).connect_enc_id[0]));
			}
			if (BF_AudioCapture_Get_InstanceInfo(loop_cap).connect_enc_id[1] != 0xFF){
				cap_connect_count++;
				BF_Debug_Print_Information(("      Connected AudioEncoder id: %7d\n", (int)BF_AudioCapture_Get_InstanceInfo(loop_cap).connect_enc_id[1]));
			}
				
			if (BF_AudioCapture_Get_InstanceInfo(loop_cap).connect_out_id != 0xFF){
				cap_connect_count++;
				BF_Debug_Print_Information(("      Connected AudioOut id    : %7d\n", (int)BF_AudioCapture_Get_InstanceInfo(loop_cap).connect_out_id));
			}
			
			if (cap_connect_count == 0){
				BF_Debug_Print_Information(("      Non Connect\n"));
			}
		}
		else {
			BF_Debug_Print_Information((" Non Active\n"));
		}
	}
	
	// AudioEncoder
	for (loop_enc = 0; loop_enc < D_AUDIO_ENC_INST_NUM; loop_enc++){
		BF_Debug_Print_Information(( "\n" ));
		BF_Debug_Print_Information((" [AudioEncoder id : %d]", BF_AudioEnc_Get_InstanceInfo(loop_enc).audio_enc_id));
		
		if (BF_AudioEnc_Get_InstanceInfo(loop_enc).enable == FJ_AUDIO_ENABLE){
			BF_Debug_Print_Information((" Active\n"));
			BF_Debug_Print_Information(("    Type                       : %7d\n", BF_AudioEnc_Get_InstanceInfo(loop_enc).type));
			BF_Debug_Print_Information(("    Enable flag                : %7d\n", BF_AudioEnc_Get_InstanceInfo(loop_enc).enc_enable));
			BF_Debug_Print_Information(("    Sampling rate              : %7d\n", BF_AudioEnc_Get_InstanceInfo(loop_enc).smpl_rate));
			BF_Debug_Print_Information(("    Bit rate                   : %7d\n", BF_AudioEnc_Get_InstanceInfo(loop_enc).bit_rate));
			BF_Debug_Print_Information(("    Channel type               : %7d\n", BF_AudioEnc_Get_InstanceInfo(loop_enc).ch_type));
		}
		else {
			BF_Debug_Print_Information((" Non Active\n"));
		}
	}
	
	// AudioDecoder
	BF_Debug_Print_Information(( "\n" ));
	BF_Debug_Print_Information((" [AudioDecoder id : %d]", BF_AudioDec_Get_InstanceInfo().audio_dec_id));

	if (BF_AudioDec_Get_InstanceInfo().enable == FJ_AUDIO_ENABLE){
		BF_Debug_Print_Information((" Active\n"));
		BF_Debug_Print_Information(("    Type                       : %7d\n", BF_AudioDec_Get_InstanceInfo().type));
		BF_Debug_Print_Information(("    Codec type                 : %7d\n", BF_AudioDec_Get_InstanceInfo().compress_type));
		BF_Debug_Print_Information(("    Sampling rate              : %7d\n", BF_AudioDec_Get_InstanceInfo().smpl_rate));
		BF_Debug_Print_Information(("    Channel type               : %7d\n", BF_AudioDec_Get_InstanceInfo().ch_type));
		BF_Debug_Print_Information(("    [Connectivity]\n"));
		
		dec_connect_count = 0;
		if (BF_AudioDec_Get_InstanceInfo().connect_out_id[0] != 0xFF){
			dec_connect_count++;
			BF_Debug_Print_Information(("      Connected AudioOut id    : %7d\n", (int)BF_AudioDec_Get_InstanceInfo().connect_out_id[0]));
		}

		if (BF_AudioDec_Get_InstanceInfo().connect_out_id[1] != 0xFF){
			dec_connect_count++;
			BF_Debug_Print_Information(("      Connected AudioOut id    : %7d\n", (int)BF_AudioDec_Get_InstanceInfo().connect_out_id[1]));
		}
		
		if (dec_connect_count == 0){
			BF_Debug_Print_Information(("      Non Connect\n"));
		}
	}
	else {
		BF_Debug_Print_Information((" Non Active\n"));
	}
	
	// AudioOut
	for (loop_out = 0; loop_out < D_AUDIO_OUT_INST_NUM; loop_out++){
		BF_Debug_Print_Information(( "\n" ));
		BF_Debug_Print_Information((" [AudioOut id : %d]", BF_AudioOut_Get_InstanceInfo(loop_out).audio_out_id));
		
		if (BF_AudioOut_Get_InstanceInfo(loop_out).enable == FJ_AUDIO_ENABLE){
			BF_Debug_Print_Information((" Active\n"));
			BF_Debug_Print_Information(("   Type                        : %7d\n", BF_AudioOut_Get_InstanceInfo(loop_out).type));
			BF_Debug_Print_Information(("   Output port                 : %7d\n", BF_AudioOut_Get_InstanceInfo(loop_out).out_port));
			BF_Debug_Print_Information(("   [I2S Information]\n"));
			
			if (BF_AudioOut_Get_InstanceInfo(loop_out).i2s_cfg.master_slave == FJ_AUDIO_IF_MASTER){
				BF_Debug_Print_Information(("     Master/Slave              :  Master\n"));
			}
			else {
				BF_Debug_Print_Information(("     Master/Slave              :   Slave\n"));
			}
			
			BF_Debug_Print_Information(("     LR clock                  : %7ld\n", BF_AudioOut_Get_InstanceInfo(loop_out).i2s_cfg.lr_clock));
			BF_Debug_Print_Information(("     Data cycle                : %7d\n", BF_AudioOut_Get_InstanceInfo(loop_out).i2s_cfg.data_cycle));
			BF_Debug_Print_Information(("     Format                    : %7d\n", BF_AudioOut_Get_InstanceInfo(loop_out).i2s_cfg.i2s_format));
			BF_Debug_Print_Information(("     Bit count                 : %7d\n", BF_AudioOut_Get_InstanceInfo(loop_out).i2s_cfg.bit_count));
		}
		else {
			BF_Debug_Print_Information((" Non Active\n"));
		}
	}
	
	BF_Debug_Print_Information(( "\n" ));
}

