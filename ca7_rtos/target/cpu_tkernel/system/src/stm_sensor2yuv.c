/**
 * @file		stm_sensor2yuv.c
 * @brief   	None
 * @note		None
 * @attention   None
 *
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "mode_stm.h"
#include "ctrl_pro2pipe.h"
#include "camera_config.h"
#include "share_data.h"
#include "category_parameter.h"
#include "iqpara_api.h"
#include "iq_bin_dbg.h"
#include "iq_bin_dbg2.h"
#include "sdram_map_define.h"

#include "im_pro.h"
#include "im_ltm.h"
#include "dd_tmr64.h"
#include "ctrl_ltm.h"
#include "iq_dpc_file.h"
#include "iq_dpc_multipipe_tbl_mng.h"
#include "iq_dpc_multipipe.h"
#include "iq_dpc_tbl_sel.h"
#include "TKernel_id.h"
#include "ctrl_hscap.h"
#include "ctrl_cnr_ofl.h"

// --- REMOVE_WDR BEGIN ---
#include "wdr.h"
// --- REMOVE_WDR END ---

/*==============================================================================*/
/* Definition   																*/
/*==============================================================================*/
#define D_SENSOR_REFLECT_FRAME	(2)
#define D_ISP_REFLECT_FRAME		(1)

#define D_IQ_PIPE0				(0)
#define D_IQ_PIPE1				(1)
#define D_IQ_PIPE01				(2)

#define FLG_SENSOR_ACCESS_UPDATE		0x00000001
#define FLG_SENSOR_SHUTTER_CONTROL		0x00000002
#define FLG_SENSOR_ACCESS_EXIT			0x80000001
#define FLG_SENSOR_ACCESS_WAITPTN		(FLG_SENSOR_ACCESS_UPDATE | FLG_SENSOR_SHUTTER_CONTROL | FLG_SENSOR_ACCESS_EXIT)


#define FLG_IQ_ACCESS_UPDATE_PIPE_WITHOUT_TABLE	0x00000001
#define FLG_IQ_ACCESS_CNR_OFL_PROC				0x00000002
#define FLG_IQ_ACCESS_EXIT						0x80000000
#define FLG_IQ_ACCESS_WAITPTN					(FLG_IQ_ACCESS_UPDATE_PIPE_WITHOUT_TABLE | FLG_IQ_ACCESS_CNR_OFL_PROC | FLG_IQ_ACCESS_EXIT)

#define PIPE_LOST_LINE_DPC_CAG_ELF	16
#define PIPE_LOST_LINE_DPC_ELF		12

#define PIPE_LOST_LINE_DPC_CAG	8
#define PIPE_LOST_LINE_DPC		4

#define ROUNDUP16(a) (((a) + 15) & 0xFFFFFFF0)
#define ROUNDUP8(a) (((a) + 7) & 0xFFFFFFF8)
#define ROUNDUP(a, b ) ( ((a) + (b-1)) / b ) * b

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define IS_FLGPTN_EQUAL(flgptn, FLG)	((((flgptn) & (FLG)) == (FLG)) ? TRUE : FALSE)

#define BANK_SET_TO_ING_AND_INCREMENT(a, b, mem, lock) a[E_MEM_BANK_ING] = a[E_MEM_BANK_SET];\
											a[E_MEM_BANK_SET] = ( a[E_MEM_BANK_SET] + 1) % b;\
											if( mem[ a[E_MEM_BANK_SET] ] == lock ){\
												a[E_MEM_BANK_SET] = ( a[E_MEM_BANK_SET] + 1) % b;\
											}

#define BANK_ING_TO_RCV(a) a[E_MEM_BANK_RCV] = a[E_MEM_BANK_ING]

#define UPDATE_YUV_FRAME_AND_SEND_TO_SYSTEM { \
			BANK_ING_TO_RCV(g_s2y_mng.mem_bank_y); \
			BANK_ING_TO_RCV(g_s2y_mng.mem_bank_c); \
			if (g_s2y_in.one_sensor_yuv_h_size_r2y_ch1[0] && g_s2y_in.one_sensor_yuv_v_size_r2y_ch1[0]) { \
				BANK_ING_TO_RCV(g_s2y_mng.mem_bank_y_ch1); \
				BANK_ING_TO_RCV(g_s2y_mng.mem_bank_c_ch1); \
				g_share_pipe_yuv.layout.addr_y = g_s2y_mng.mem_addr_y_ch1[g_s2y_mng.mem_bank_y_ch1[E_MEM_BANK_RCV]]; \
				g_share_pipe_yuv.layout.addr_c = g_s2y_mng.mem_addr_c_ch1[g_s2y_mng.mem_bank_c_ch1[E_MEM_BANK_RCV]]; \
				if (g_s2y_in.r2y_ch0_ch1_ratio_mismatch) { \
					g_share_pipe_yuv.layout.addr_y += g_share_pipe_yuv.layout.h_byte * g_s2y_in.r2y_ch1_ofs_line; \
					if (g_share_pipe_yuv.layout.format == E_SHARE_YUV_FORMAT_422_SEMI) \
						g_share_pipe_yuv.layout.addr_c += g_share_pipe_yuv.layout.h_byte * g_s2y_in.r2y_ch1_ofs_line; \
					else \
						g_share_pipe_yuv.layout.addr_c += g_share_pipe_yuv.layout.h_byte * (g_s2y_in.r2y_ch1_ofs_line >> 1); \
				} \
				g_share_pipe_yuv.layout.addr_y_dual_ch0 = g_s2y_mng.mem_addr_y[g_s2y_mng.mem_bank_y[E_MEM_BANK_RCV]]; \
				g_share_pipe_yuv.layout.addr_c_dual_ch0 = g_s2y_mng.mem_addr_c[g_s2y_mng.mem_bank_c[E_MEM_BANK_RCV]]; \
				g_share_pipe_yuv.sensor[A].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y_ch1[0]; \
				g_share_pipe_yuv.sensor[A].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c_ch1[0]; \
			} else { \
				g_share_pipe_yuv.layout.addr_y = g_s2y_mng.mem_addr_y[g_s2y_mng.mem_bank_y[E_MEM_BANK_RCV]]; \
				g_share_pipe_yuv.layout.addr_c = g_s2y_mng.mem_addr_c[g_s2y_mng.mem_bank_c[E_MEM_BANK_RCV]]; \
				g_share_pipe_yuv.sensor[A].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y[0]; \
				g_share_pipe_yuv.sensor[A].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c[0]; \
			} \
			low_latency_r2y_share_pipe_yuv(); \
			switch( g_s2y_mng.pipe_mode ){ \
			case E_PIPE_MODE_SENSORx2_DIRECT: \
				g_share_pipe_yuv.sensor[B].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y[1]; \
				g_share_pipe_yuv.sensor[B].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c[1]; \
				break; \
			case E_PIPE_MODE_SENSORx2_DIV2: \
				g_share_pipe_yuv.sensor[B].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y[2]; \
				g_share_pipe_yuv.sensor[B].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c[2]; \
				break; \
			case E_PIPE_MODE_SENSORx4: \
				g_share_pipe_yuv.sensor[B].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y[1]; \
				g_share_pipe_yuv.sensor[B].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c[1]; \
				g_share_pipe_yuv.sensor[C].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y[2]; \
				g_share_pipe_yuv.sensor[C].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c[2]; \
				g_share_pipe_yuv.sensor[D].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y[3]; \
				g_share_pipe_yuv.sensor[D].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c[3]; \
				break; \
			default: \
				break; \
			} \
			if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 || g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) { \
				g_share_pipe_dol_yuv.long_yuv.addr_y = g_s2y_mng.mem_addr_y[g_s2y_mng.mem_bank_y[E_MEM_BANK_RCV]]; \
				g_share_pipe_dol_yuv.long_yuv.addr_c = g_s2y_mng.mem_addr_c[g_s2y_mng.mem_bank_c[E_MEM_BANK_RCV]]; \
				if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 ) { \
					g_share_pipe_dol_yuv.short_yuv.addr_y = g_share_pipe_dol_yuv.long_yuv.addr_y + g_s2y_mng.addr_ofst_y[2]; \
					g_share_pipe_dol_yuv.short_yuv.addr_c = g_share_pipe_dol_yuv.long_yuv.addr_c + g_s2y_mng.addr_ofst_c[2]; \
				} else if ( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL ) { \
					g_share_pipe_dol_yuv.short_yuv.addr_y = g_share_pipe_dol_yuv.long_yuv.addr_y + g_s2y_mng.addr_ofst_y[1]; \
					g_share_pipe_dol_yuv.short_yuv.addr_c = g_share_pipe_dol_yuv.long_yuv.addr_c + g_s2y_mng.addr_ofst_c[1]; \
				} \
			} \
			if(g_skip_frame_num == 0) { \
				if( g_s2y_mng.hscap_en ){\
					ULLONG hscap_frame_no = 0; \
					HSCap_Measure_Pipe_End(); \
					if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 || g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) { \
						Set_Frame_Share_PIPE_DOL_YUV(&g_share_pipe_dol_yuv, g_yuv_cnt); \
						Set_Frame_Share_R2Y_DOL_Hist(&g_share_r2y_dol_hist, g_yuv_cnt); \
						if( HSCap_Judge_Start_NextProcess( &hscap_frame_no ) ){ \
							Snd_Event_to_System( E_SYSTEM_EVENT_PIPE_DOL_YUV, hscap_frame_no ); \
							g_cap_cnt ++ ; \
						} \
					} else { \
						Set_Frame_Share_PIPE_YUV(&g_share_pipe_yuv, g_yuv_cnt); \
						Set_Frame_Share_R2Y_Hist(&g_share_r2y_hist, g_yuv_cnt); \
						if( HSCap_Judge_Start_NextProcess( &hscap_frame_no ) ){ \
							Snd_Event_to_System( E_SYSTEM_EVENT_CNR_OFL, hscap_frame_no ); \
						} \
					} \
					g_yuv_cnt++; \
				}else{ \
					if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 || g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) { \
						Set_Frame_Share_PIPE_DOL_YUV(&g_share_pipe_dol_yuv, g_yuv_cnt); \
						Set_Frame_Share_R2Y_DOL_Hist(&g_share_r2y_dol_hist, g_yuv_cnt); \
						Snd_Event_to_System( E_SYSTEM_EVENT_PIPE_DOL_YUV, g_yuv_cnt ); \
						g_yuv_cnt++; \
					} else { \
						Set_Frame_Share_PIPE_YUV(&g_share_pipe_yuv, g_yuv_cnt); \
						Set_Frame_Share_R2Y_Hist(&g_share_r2y_hist, g_yuv_cnt); \
						Snd_Event_to_System( E_SYSTEM_EVENT_PIPE_YUV, g_yuv_cnt ); \
						g_yuv_cnt++; \
					} \
				} \
			} else { \
				g_skip_frame_num--; \
			} \
}

#define UPDATE_AEAWB_FRAME_AND_SEND_TO_USER { \
			BANK_ING_TO_RCV(g_s2y_mng.mem_bank_aeawb); \
			for( i = 0; i < g_s2y_mng.pipe_use_num; i++ ){ \
				g_share_aeawb_stat.rect[i].addr_ae        = g_s2y_mng.mem_addr_aeawb[g_s2y_mng.mem_bank_aeawb[E_MEM_BANK_RCV]] + g_s2y_mng.addr_ofst_aeawb[i] + g_s2y_mng.pipe[i].h->aeawb_addr_ae; \
				g_share_aeawb_stat.rect[i].addr_awb       = g_s2y_mng.mem_addr_aeawb[g_s2y_mng.mem_bank_aeawb[E_MEM_BANK_RCV]] + g_s2y_mng.addr_ofst_aeawb[i] + g_s2y_mng.pipe[i].h->aeawb_addr_awb; \
				g_share_aeawb_stat.rect[i].addr_level_awb = g_s2y_mng.mem_addr_aeawb[g_s2y_mng.mem_bank_aeawb[E_MEM_BANK_RCV]] + g_s2y_mng.addr_ofst_aeawb[i] + g_s2y_mng.pipe[i].h->aeawb_addr_level_awb; \
			} \
			Set_Frame_Share_AEAWB_STAT(&g_share_aeawb_stat, g_aeawb_cnt); \
			Send_Share_User_Event(E_SHARE_USER_EVENT_AEAWB_STAT_END, g_aeawb_cnt); \
			g_aeawb_cnt++; \
}

#define UPDATE_PRO_HIST { \
			if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 || g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) { \
				Set_Frame_Share_PRO_DOL_Hist(&g_share_pro_dol_hist, g_prohist_cnt); \
			} else { \
				Set_Frame_Share_PRO_Hist(&g_share_pro_hist, g_prohist_cnt); \
			} \
			g_prohist_cnt++; \
}

#define COPY_AWB_PARAM(awb) { \
		if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx2_DIV2){ \
			memcpy( g_s2y_mng.pipe[0].wb_cc.wb_gain,   awb.sensor[A].wb_gain,   sizeof(g_s2y_mng.pipe[0].wb_cc.wb_gain) ); \
			memcpy( g_s2y_mng.pipe[0].wb_cc.cc_matrix, awb.sensor[A].cc_matrix, sizeof(g_s2y_mng.pipe[0].wb_cc.cc_matrix) ); \
			memcpy( g_s2y_mng.pipe[0].aeyk,            awb.sensor[A].aeyk, 		sizeof(g_s2y_mng.pipe[0].aeyk) ); \
			        g_s2y_mng.pipe[0].fshd_blend0    = awb.sensor[A].shdbld0; \
			        g_s2y_mng.pipe[0].fshd_blend1    = awb.sensor[A].shdbld1; \
			        g_s2y_mng.pipe[0].addr_fshd0     = Get_Share_SHD_Tbl_Addr( A, awb.sensor[A].shdtbl0_idx ); \
			        g_s2y_mng.pipe[0].addr_fshd1     = Get_Share_SHD_Tbl_Addr( A, awb.sensor[A].shdtbl1_idx ); \
			memcpy( g_s2y_mng.pipe[1].wb_cc.wb_gain,   awb.sensor[A].wb_gain,   sizeof(g_s2y_mng.pipe[0].wb_cc.wb_gain) ); \
			memcpy( g_s2y_mng.pipe[1].wb_cc.cc_matrix, awb.sensor[A].cc_matrix, sizeof(g_s2y_mng.pipe[0].wb_cc.cc_matrix) ); \
			memcpy( g_s2y_mng.pipe[1].aeyk,            awb.sensor[A].aeyk, 		sizeof(g_s2y_mng.pipe[0].aeyk) ); \
			        g_s2y_mng.pipe[1].fshd_blend0    = awb.sensor[A].shdbld0; \
			        g_s2y_mng.pipe[1].fshd_blend1    = awb.sensor[A].shdbld1; \
			        g_s2y_mng.pipe[1].addr_fshd0     = Get_Share_SHD_Tbl_Addr( A, awb.sensor[A].shdtbl0_idx ); \
			        g_s2y_mng.pipe[1].addr_fshd1     = Get_Share_SHD_Tbl_Addr( A, awb.sensor[A].shdtbl1_idx ); \
			memcpy( g_s2y_mng.pipe[2].wb_cc.wb_gain,   awb.sensor[B].wb_gain,   sizeof(g_s2y_mng.pipe[0].wb_cc.wb_gain) ); \
			memcpy( g_s2y_mng.pipe[2].wb_cc.cc_matrix, awb.sensor[B].cc_matrix, sizeof(g_s2y_mng.pipe[0].wb_cc.cc_matrix) ); \
			memcpy( g_s2y_mng.pipe[2].aeyk,            awb.sensor[B].aeyk, 		sizeof(g_s2y_mng.pipe[0].aeyk) ); \
			        g_s2y_mng.pipe[2].fshd_blend0    = awb.sensor[B].shdbld0; \
			        g_s2y_mng.pipe[2].fshd_blend1    = awb.sensor[B].shdbld1; \
			        g_s2y_mng.pipe[2].addr_fshd0     = Get_Share_SHD_Tbl_Addr( B, awb.sensor[B].shdtbl0_idx ); \
			        g_s2y_mng.pipe[2].addr_fshd1     = Get_Share_SHD_Tbl_Addr( B, awb.sensor[B].shdtbl1_idx ); \
			memcpy( g_s2y_mng.pipe[3].wb_cc.wb_gain,   awb.sensor[B].wb_gain,   sizeof(g_s2y_mng.pipe[0].wb_cc.wb_gain) ); \
			memcpy( g_s2y_mng.pipe[3].wb_cc.cc_matrix, awb.sensor[B].cc_matrix, sizeof(g_s2y_mng.pipe[0].wb_cc.cc_matrix) ); \
			memcpy( g_s2y_mng.pipe[3].aeyk,            awb.sensor[B].aeyk, 		sizeof(g_s2y_mng.pipe[0].aeyk) ); \
			        g_s2y_mng.pipe[3].fshd_blend0    = awb.sensor[B].shdbld0; \
			        g_s2y_mng.pipe[3].fshd_blend1    = awb.sensor[B].shdbld1; \
			        g_s2y_mng.pipe[3].addr_fshd0     = Get_Share_SHD_Tbl_Addr( B, awb.sensor[B].shdtbl0_idx ); \
			        g_s2y_mng.pipe[3].addr_fshd1     = Get_Share_SHD_Tbl_Addr( B, awb.sensor[B].shdtbl1_idx ); \
		}else{ \
			memcpy( g_s2y_mng.pipe[0].wb_cc.wb_gain,   awb.sensor[A].wb_gain,   sizeof(g_s2y_mng.pipe[0].wb_cc.wb_gain) ); \
			memcpy( g_s2y_mng.pipe[0].wb_cc.cc_matrix, awb.sensor[A].cc_matrix, sizeof(g_s2y_mng.pipe[0].wb_cc.cc_matrix) ); \
			memcpy( g_s2y_mng.pipe[0].aeyk,            awb.sensor[A].aeyk, 		sizeof(g_s2y_mng.pipe[0].aeyk) ); \
			        g_s2y_mng.pipe[0].fshd_blend0    = awb.sensor[A].shdbld0; \
			        g_s2y_mng.pipe[0].fshd_blend1    = awb.sensor[A].shdbld1; \
			        g_s2y_mng.pipe[0].addr_fshd0     = Get_Share_SHD_Tbl_Addr( A, awb.sensor[A].shdtbl0_idx ); \
			        g_s2y_mng.pipe[0].addr_fshd1     = Get_Share_SHD_Tbl_Addr( A, awb.sensor[A].shdtbl1_idx ); \
			memcpy( g_s2y_mng.pipe[1].wb_cc.wb_gain,   awb.sensor[B].wb_gain,   sizeof(g_s2y_mng.pipe[0].wb_cc.wb_gain) ); \
			memcpy( g_s2y_mng.pipe[1].wb_cc.cc_matrix, awb.sensor[B].cc_matrix, sizeof(g_s2y_mng.pipe[0].wb_cc.cc_matrix) ); \
			memcpy( g_s2y_mng.pipe[1].aeyk,            awb.sensor[B].aeyk, 		sizeof(g_s2y_mng.pipe[0].aeyk) ); \
			        g_s2y_mng.pipe[1].fshd_blend0    = awb.sensor[B].shdbld0; \
			        g_s2y_mng.pipe[1].fshd_blend1    = awb.sensor[B].shdbld1; \
			        g_s2y_mng.pipe[1].addr_fshd0     = Get_Share_SHD_Tbl_Addr( B, awb.sensor[B].shdtbl0_idx ); \
			        g_s2y_mng.pipe[1].addr_fshd1     = Get_Share_SHD_Tbl_Addr( B, awb.sensor[B].shdtbl1_idx ); \
			memcpy( g_s2y_mng.pipe[2].wb_cc.wb_gain,   awb.sensor[C].wb_gain,   sizeof(g_s2y_mng.pipe[0].wb_cc.wb_gain) ); \
			memcpy( g_s2y_mng.pipe[2].wb_cc.cc_matrix, awb.sensor[C].cc_matrix, sizeof(g_s2y_mng.pipe[0].wb_cc.cc_matrix) ); \
			memcpy( g_s2y_mng.pipe[2].aeyk,            awb.sensor[C].aeyk, 		sizeof(g_s2y_mng.pipe[0].aeyk) ); \
			        g_s2y_mng.pipe[2].fshd_blend0    = awb.sensor[C].shdbld0; \
			        g_s2y_mng.pipe[2].fshd_blend1    = awb.sensor[C].shdbld1; \
			        g_s2y_mng.pipe[2].addr_fshd0     = Get_Share_SHD_Tbl_Addr( C, awb.sensor[C].shdtbl0_idx ); \
			        g_s2y_mng.pipe[2].addr_fshd1     = Get_Share_SHD_Tbl_Addr( C, awb.sensor[C].shdtbl1_idx ); \
			memcpy( g_s2y_mng.pipe[3].wb_cc.wb_gain,   awb.sensor[D].wb_gain,   sizeof(g_s2y_mng.pipe[0].wb_cc.wb_gain) ); \
			memcpy( g_s2y_mng.pipe[3].wb_cc.cc_matrix, awb.sensor[D].cc_matrix, sizeof(g_s2y_mng.pipe[0].wb_cc.cc_matrix) ); \
			memcpy( g_s2y_mng.pipe[3].aeyk,            awb.sensor[D].aeyk, 		sizeof(g_s2y_mng.pipe[0].aeyk) ); \
			        g_s2y_mng.pipe[3].fshd_blend0    = awb.sensor[D].shdbld0; \
			        g_s2y_mng.pipe[3].fshd_blend1    = awb.sensor[D].shdbld1; \
			        g_s2y_mng.pipe[3].addr_fshd0     = Get_Share_SHD_Tbl_Addr( D, awb.sensor[D].shdtbl0_idx ); \
			        g_s2y_mng.pipe[3].addr_fshd1     = Get_Share_SHD_Tbl_Addr( D, awb.sensor[D].shdtbl1_idx ); \
		} \
}

#define COPY_PGAIN(isp_gain) { \
		if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 ){ \
			memcpy( g_s2y_mng.pipe[0].pgain_sro_begin,   isp_gain.sensor[A].sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[0].pgain_b2b_begin,   isp_gain.sensor[A].b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[0].pgain_b2b_end,     isp_gain.sensor[A].b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[1].pgain_sro_begin,   isp_gain.sensor[A].sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[1].pgain_b2b_begin,   isp_gain.sensor[A].b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[1].pgain_b2b_end,     isp_gain.sensor[A].b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[2].pgain_sro_begin,   isp_gain.sensor[A].DOL_short_sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[2].pgain_b2b_begin,   isp_gain.sensor[A].DOL_short_b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[2].pgain_b2b_end,     isp_gain.sensor[A].DOL_short_b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[3].pgain_sro_begin,   isp_gain.sensor[A].DOL_short_sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[3].pgain_b2b_begin,   isp_gain.sensor[A].DOL_short_b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[3].pgain_b2b_end,     isp_gain.sensor[A].DOL_short_b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
		}else if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL ){ \
			memcpy( g_s2y_mng.pipe[0].pgain_sro_begin,   isp_gain.sensor[A].sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[0].pgain_b2b_begin,   isp_gain.sensor[A].b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[0].pgain_b2b_end,     isp_gain.sensor[A].b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[1].pgain_sro_begin,   isp_gain.sensor[A].DOL_short_sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[1].pgain_b2b_begin,   isp_gain.sensor[A].DOL_short_b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[1].pgain_b2b_end,     isp_gain.sensor[A].DOL_short_b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
		}else if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx2_DIV2){ \
			memcpy( g_s2y_mng.pipe[0].pgain_sro_begin,   isp_gain.sensor[A].sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[0].pgain_b2b_begin,   isp_gain.sensor[A].b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[0].pgain_b2b_end,     isp_gain.sensor[A].b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[1].pgain_sro_begin,   isp_gain.sensor[A].sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[1].pgain_b2b_begin,   isp_gain.sensor[A].b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[1].pgain_b2b_end,     isp_gain.sensor[A].b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[2].pgain_sro_begin,   isp_gain.sensor[B].sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[2].pgain_b2b_begin,   isp_gain.sensor[B].b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[2].pgain_b2b_end,     isp_gain.sensor[B].b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[3].pgain_sro_begin,   isp_gain.sensor[B].sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[3].pgain_b2b_begin,   isp_gain.sensor[B].b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[3].pgain_b2b_end,     isp_gain.sensor[B].b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
		}else{ \
			memcpy( g_s2y_mng.pipe[0].pgain_sro_begin,   isp_gain.sensor[A].sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[0].pgain_b2b_begin,   isp_gain.sensor[A].b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[0].pgain_b2b_end,     isp_gain.sensor[A].b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[1].pgain_sro_begin,   isp_gain.sensor[B].sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[1].pgain_b2b_begin,   isp_gain.sensor[B].b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[1].pgain_b2b_end,     isp_gain.sensor[B].b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[2].pgain_sro_begin,   isp_gain.sensor[C].sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[2].pgain_b2b_begin,   isp_gain.sensor[C].b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[2].pgain_b2b_end,     isp_gain.sensor[C].b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[3].pgain_sro_begin,   isp_gain.sensor[D].sro_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[3].pgain_b2b_begin,   isp_gain.sensor[D].b2b_begin,   sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
			memcpy( g_s2y_mng.pipe[3].pgain_b2b_end,     isp_gain.sensor[D].b2b_end,     sizeof(g_s2y_mng.pipe[0].pgain_sro_begin) ); \
		} \
}

/*==============================================================================*/
/* Enumeration  																*/
/*==============================================================================*/
typedef enum {
	E_PIPE_USE_NUM_1 = 1,
	E_PIPE_USE_NUM_2 = 2,
	E_PIPE_USE_NUM_4 = 4,
} E_PIPE_USE_NUM;

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	E_PIPE_MODE 		pipe_mode;
	BOOL				is_direct;
	E_PIPE_USE_NUM		pipe_use_num;
	E_CNR_TYPE			cnr_type;
	BOOL				is_r2y_out_ipu;
	BOOL				shd_en;
	BOOL				cag_en;
	UCHAR   			div_num;
	UCHAR   			pipes_2nd_time;
	UCHAR   			sro_2nd_time_s;
	UCHAR				ltmmap_2nd_time;
	UCHAR				prch_running_flg;
	ULLONG				prch_frame_no;
	ULLONG				r2y_frame_no;
	
	ULONG   			mem_addr_raw[D_PIPE_MEM_BANK_RAW];
	ULONG   			mem_addr_raw1[D_PIPE_MEM_BANK_RAW];
	ULONG   			mem_addr_y[D_PIPE_MEM_BANK_Y];
	ULONG   			mem_addr_c[D_PIPE_MEM_BANK_C];
	ULONG   			mem_addr_y_ch1[D_PIPE_MEM_BANK_Y];
	ULONG   			mem_addr_c_ch1[D_PIPE_MEM_BANK_C];
	ULONG   			mem_addr_aeawb[D_PIPE_MEM_BANK_AEAWB];
	ULONG				mem_addr_map_yp[D_PIPE_MEM_BANK_MAP];
	ULONG				mem_addr_map_yn[D_PIPE_MEM_BANK_MAP];
	ULONG				mem_addr_map_rgb[D_PIPE_MEM_BANK_MAP_RGB];

	ULONG   			mem_addr_cnr_ofl_ex;
	ULONG   			mem_addr_cnr_ofl_work;

	UCHAR   			mem_bank_raw[E_MEM_BANK_MAX];
	UCHAR   			mem_bank_raw1[E_MEM_BANK_MAX];
	UCHAR   			mem_bank_y[E_MEM_BANK_MAX];
	UCHAR   			mem_bank_c[E_MEM_BANK_MAX];
	UCHAR   			mem_bank_y_ch1[E_MEM_BANK_MAX];
	UCHAR   			mem_bank_c_ch1[E_MEM_BANK_MAX];
	UCHAR   			mem_bank_aeawb[E_MEM_BANK_MAX];
	UCHAR   			mem_bank_map[E_MEM_BANK_MAX];
	UCHAR   			mem_bank_map_rgb[E_MEM_BANK_MAX];
	
	UCHAR				mem_bank_raw_num;
	UCHAR				mem_bank_yc_num;
	UCHAR				mem_bank_yc_ch1_num;
	UCHAR				mem_bank_map_num;
	UCHAR				mem_bank_map_rgb_num;
	
	T_SHARE_AE_PARAM	ae_param;
	T_SHARE_AE_PARAM	ae_param_share;
	T_SHARE_ISP_GAIN	isp_gain_reg;
	T_SHARE_SENSOR_EXP	exp;
	T_SHARE_AWB_PARAM	awb_param;
	T_SHARE_AWB_PARAM	awb_param_reg;
	T_SHARE_AF_PARAM	af_param;
	T_SHARE_AF_PARAM	af_param_share;
	
	UCHAR				iq_stream[E_SHARE_SENSOR_NUM_MAX];
	UCHAR				iq_scene[E_SHARE_SENSOR_NUM_MAX];
	FLOAT				iq_gain_times[E_SHARE_SENSOR_NUM_MAX];
	
	ULONG				h_byte_yc;
	ULONG				addr_ofst_y[4+1];
	ULONG				addr_ofst_c[4+1];
	ULONG				h_byte_yc_ch1;
	ULONG				addr_ofst_y_ch1[4+1];
	ULONG				addr_ofst_c_ch1[4+1];
	ULONG				addr_ofst_raw[4+1];
	ULONG				addr_ofst_aeawb[4+1];
	ULONG				addr_ofst_map[4+1];
	ULONG				addr_ofst_maprgb[4+1];
	
	T_P2P_CPHY_DMIPI	cphy_dmipi;
	T_P2P_CPHY_SLVS		cphy_slvs;
	T_P2P_CPHY_LVDS		cphy_lvds;
	T_P2P_SEN   		sen;
	T_P2P_LDIV_PWCH 	ldiv_pwch[4];   // 4 is div_num or sensor_num
	T_P2P_SRO_PG		sro_pg[4];
	T_P2P_PIPE  		pipe[4];
	
	T_P2P_AF			af[4];
	T_P2P_AF_DATA		af_data[4];
	T_P2P_AF_H			af_h_ing[4];
	T_P2P_AF_H			af_h_rcv[4];

	BOOL				ltm_rgb_rdy;

	ULLONG				pwch_cnt;
	ULLONG				ltmrgb_cnt;
	
	BOOL				hscap_en;
} T_SENSOR2YUV_MNG;

typedef struct {
	ULLONG								r2y_frame_no_set;
	ULLONG								r2y_frame_no_ing;
	ULLONG								r2y_frame_no_rcv;
	
	USHORT								r2y_def_h_byte;
	
	T_BF_ENCODER_GET_IMAGE_ADDR_PARAM	addr_param_set;
	T_BF_ENCODER_GET_IMAGE_ADDR_PARAM	addr_param_ing;
	T_BF_ENCODER_GET_IMAGE_ADDR_PARAM	addr_param_rcv;
} T_SENSOR2YUV_MNG_ENC;


/*==============================================================================*/
/* Global Data  																*/
/*==============================================================================*/
static T_STM_SENSOR2YUV_IN  		g_s2y_in;
static T_SENSOR2YUV_MNG 			g_s2y_mng;
static T_SENSOR2YUV_MNG_ENC			g_s2y_mng_enc;
static T_SHARE_PIPE_YUV				g_share_pipe_yuv;
static T_SHARE_PIPE_YUV				g_share_cnr_ofl_yuv;
static T_SHARE_PIPE_DOL_YUV			g_share_pipe_dol_yuv;
static T_SHARE_RAW		 			g_share_sen_raw;
static T_SHARE_AEAWB_STAT			g_share_aeawb_stat;
static T_SHARE_R2Y_HIST				g_share_r2y_hist;
static T_SHARE_R2Y_DOL_HIST			g_share_r2y_dol_hist;
static T_SHARE_PRO_HIST				g_share_pro_hist;
static T_SHARE_PRO_DOL_HIST			g_share_pro_dol_hist;
static T_SHARE_AF_DATA				g_share_af_data;
static T_P2P_LDIV_PIPE_CALC_IN  	g_p2p_calc_in[E_SHARE_SENSOR_NUM_MAX];
static T_P2P_LDIV_PIPE_CALC_OUT 	g_p2p_calc_out[E_SHARE_SENSOR_NUM_MAX];
static T_P2P_LDIV_PWCH_PD			g_p2p_pwch_pd[2];
static T_P2P_AF_CALC_IN  			g_p2p_af_calc_in[E_SHARE_SENSOR_NUM_MAX];
static T_P2P_AF_CALC_OUT 			g_p2p_af_calc_out[E_SHARE_SENSOR_NUM_MAX];
static E_SHARE_SENSOR_ID			A, B, C, D;
static ULLONG 						g_yuv_cnt = 0;
static ULLONG 						g_raw_cnt = 0;
static ULLONG 						g_raw1_cnt = 0;
static ULLONG 						g_aeawb_cnt = 0;
static ULLONG						g_prohist_cnt = 0;
static ULLONG						g_gyro_cnt = 0;
static ULLONG						g_af_cnt = 0;
static ULLONG						g_cap_cnt = 0;
static volatile ULONG				g_gyro_current_flg = 0;
static volatile ULONG				g_gyro_compare_flg = 0;
static INT32						g_sensor2yuv_state = D_STM_IDLE;

static volatile T_SHARE_FRAME_TIMESTAMP g_frame_timestamp;
static UCHAR						g_skip_frame_num = 0;
static BOOL							g_cap_shutter_release = TRUE;
static ULLONG						g_cnr_ofl_fno = 0;
static ULLONG						g_dol_save_raw_frame = -1;

/*==============================================================================*/
/* Const Data   																*/
/*==============================================================================*/


/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void iq_update( BOOL is_tbl, UCHAR stream0, UCHAR stream1 );
static void iq_update_sen_to_b2b( UCHAR stream0, UCHAR stream1 );
static void iq_update_b2r_to_cnr( BOOL is_tbl, UCHAR stream0, UCHAR stream1 );
static void gyro_irq_callbak( T_CALLBACK_ID* id, ULONG result, ULONG user_param );
static void vblank_irq_callback( T_CALLBACK_ID* id, ULONG result, ULONG user_param );
static void r2y_irq_callback( T_CALLBACK_ID* id, ULONG result, ULONG user_param );
static INT32 adjust_sensor_input( UINT32 available_div, E_SHARE_SENSOR_ID sensor, E_SHARE_H_DIV div, UINT32 ldiv_h_size );
static void calc_af( ULLONG frame_no, const T_SHARE_AF_PARAM* in );
static void send_af_data( void );
static void make_dualpd_data( E_PIPE_MODE mode );
static void make_pd_data( const T_SHARE_RAW* raw );
static void iq_dbg2_assign( UINT32 flg );
static void iq_dbg2_set( UINT32 flg );
static void ltm_rgb_map_process(ULLONG frame_no);
static void set_sdc( void );
static void b2r_r2y_map_proc(ULLONG frame_no);

static void low_latency_r2y_line_config( void );
static void low_latency_r2y_line_start( ULLONG frame_no );
static void low_latency_r2y_line_end( ULLONG frame_no );
static void low_latency_r2y_share_pipe_yuv( void );
/*==============================================================================*/
/* Global Function  															*/
/*==============================================================================*/
INT32 STM_Sensor2YUV_Config( const T_STM_SENSOR2YUV_IN* in, T_STM_SENSOR2YUV_OUT* const out )
{
	UINT32  				available_div[E_SHARE_SENSOR_ID_MAX];
	UINT32  				mem_size, mem_addr;
	UINT32  				div_c = 1;
	INT32 					i, m, n;
	UCHAR 					sensor_mode;
	E_SDRAM_CH 				sdram_ch;
	T_SHARE_SYSTEM_PARAM	param;
	E_IM_PRO_BLOCK_TYPE		blk_type;
	UCHAR					lost_line = 0;
	UCHAR					sen_type = 0;
	UCHAR					r2y_ch1_enable = 0;

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SYSTEM, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "Start"));

	/* Clear 0 */
	memset( (VOID*)&g_s2y_mng, 0, sizeof(g_s2y_mng) );
	memset( (VOID*)&g_s2y_mng_enc, 0, sizeof(g_s2y_mng_enc) );
	memset( (VOID*)&g_p2p_calc_in, 0, sizeof(g_p2p_calc_in) );
	memset( (VOID*)&g_p2p_calc_out, 0, sizeof(g_p2p_calc_out) );
	memset( (VOID*)&g_p2p_af_calc_in, 0, sizeof(g_p2p_af_calc_in) );
	memset( (VOID*)&g_p2p_af_calc_out, 0, sizeof(g_p2p_af_calc_out) );
	memset( (VOID*)&g_share_pipe_yuv, 0, sizeof(g_share_pipe_yuv));
	memset( (VOID*)&g_share_aeawb_stat, 0, sizeof(g_share_aeawb_stat));
	memset( (VOID*)&g_share_af_data, 0, sizeof(g_share_af_data) );
	memset( (VOID*)&g_share_sen_raw, 0, sizeof(g_share_sen_raw));
	memset( (VOID*)&g_frame_timestamp, 0, sizeof(g_frame_timestamp) );
	g_s2y_mng.prch_frame_no = 0xFFFFFFFFFFFFFFFF;
	g_s2y_mng.r2y_frame_no  = 0xFFFFFFFFFFFFFFFF;
	
	/* Get Input */
	g_s2y_in = *in;
	g_s2y_mng.shd_en = g_s2y_in.shd_en;
	g_s2y_mng.cag_en = g_s2y_in.cag_en;
	g_s2y_mng.sen.moni_extend_hd = g_s2y_in.extend_hd;
	A = g_s2y_in.sensor_out[E_SHARE_SENSOR_OUT_A];
	B = g_s2y_in.sensor_out[E_SHARE_SENSOR_OUT_B];
	C = g_s2y_in.sensor_out[E_SHARE_SENSOR_OUT_C];
	D = g_s2y_in.sensor_out[E_SHARE_SENSOR_OUT_D];

	if (g_s2y_in.one_sensor_yuv_h_size_r2y_ch1[0] != 0 && g_s2y_in.one_sensor_yuv_v_size_r2y_ch1[0] != 0)
		r2y_ch1_enable = 1;

	P2P_Set_Raw_RecMode(g_s2y_in.raw_out_pack_mode);

	g_skip_frame_num = CAMERA.start_frame_num;

	g_cap_shutter_release= TRUE;
	g_dol_save_raw_frame = -1;
	// LTM direct mode: 1st frame, there is no MAP generated, so skip
	if( g_skip_frame_num == 0 && g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_DIRECT ) {
		g_skip_frame_num = 1;
	}
	// High Speed Capture
	if( g_s2y_in.cap_mode && g_s2y_in.jpeg_bank_num_for_hscap ){
		if( g_s2y_in.sensor_num == 1 ){
			g_s2y_mng.hscap_en = TRUE;
		}
	}
	
	/* Get From Camera */
	for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
		if( g_s2y_in.sensor_use & (1<<i) ){
			
			sensor_mode = g_s2y_in.sensor_mode[i]; // need error check
			
			// calc in
			g_p2p_calc_in[i].proclk_MHz = 300.0;
			g_p2p_calc_in[i].mergin_ci_ee1 = 6;
			g_p2p_calc_in[i].mergin_ee2 = r2y_ch1_enable ? 0 : 2;
			switch( in->cnr_type ) {
			case E_CNR_OFF:
				g_p2p_calc_in[i].mergin_cnr = 0;
				break;
			case E_CNR_ON:
				g_p2p_calc_in[i].mergin_cnr = 16;
				break;
			case E_CNR_EXMODE:
				g_p2p_calc_in[i].mergin_cnr = 36;
				break;
			default:
				break;
			}
			g_p2p_calc_in[i].mergin_dpc_cag_elf = 20;
			g_p2p_calc_in[i].pro_v_shd_ofst = D_SHARE_PIPE_V_OFFSET;
			g_p2p_calc_in[i].pro_v_lost_line = D_SHARE_PIPE_V_LOST;
			
			g_p2p_calc_in[i].sensor_clk_Mpps			= CAMERA.sensor[i].mode[ sensor_mode ].Mpps_HDclk;

			if( g_s2y_in.dol_sencore_use == 0x03 ) {
				g_p2p_calc_in[i].sensor_h_cyc_min			= CAMERA.sensor[i].mode[ sensor_mode ].dol_hdocyc;
			} else {
				g_p2p_calc_in[i].sensor_h_cyc_min			= CAMERA.sensor[i].mode[ sensor_mode ].hdocyc;
			}
			
			g_p2p_calc_in[i].sensor_h_out_total			= CAMERA.sensor[i].mode[ sensor_mode ].total_pix_width;
			g_p2p_calc_in[i].sensor_h_ob_ofst			= CAMERA.sensor[i].mode[ sensor_mode ].ob_pix_x_pos;
			g_p2p_calc_in[i].sensor_h_ob_size			= CAMERA.sensor[i].mode[ sensor_mode ].ob_pix_width;
			g_p2p_calc_in[i].sensor_h_effective_ofst	= CAMERA.sensor[i].mode[ sensor_mode ].active_pix_x_pos;
			g_p2p_calc_in[i].sensor_h_effective_size	= CAMERA.sensor[i].mode[ sensor_mode ].active_pix_width;
			
			if( g_s2y_in.dol_sencore_use == 0x03 ) {
				g_p2p_calc_in[i].sensor_v_cyc_min			= CAMERA.sensor[i].mode[ sensor_mode ].dol_vdocyc;
			} else {
				g_p2p_calc_in[i].sensor_v_cyc_min			= CAMERA.sensor[i].mode[ sensor_mode ].vdocyc;
			}

			g_p2p_calc_in[i].sensor_v_out_total			= CAMERA.sensor[i].mode[ sensor_mode ].total_pix_lines; 
			g_p2p_calc_in[i].sensor_v_ob_ofst			= CAMERA.sensor[i].mode[ sensor_mode ].ob_pix_y_pos;
			g_p2p_calc_in[i].sensor_v_ob_size			= CAMERA.sensor[i].mode[ sensor_mode ].ob_pix_lines;
			g_p2p_calc_in[i].sensor_v_effective_ofst	= CAMERA.sensor[i].mode[ sensor_mode ].active_pix_y_pos;
			g_p2p_calc_in[i].sensor_v_effective_size	= CAMERA.sensor[i].mode[ sensor_mode ].active_pix_lines;

			if(g_s2y_in.af_en & 0x04) {
				g_s2y_mng.sen.pd_extend = CAMERA.sensor[i].mode[ sensor_mode ].pd_data_extend_lines;
				g_s2y_mng.sen.moni_extend_hd = g_s2y_mng.sen.pd_extend + 2; // add 2 lines since LDIV will lost 2 lines
				
				g_p2p_calc_in[i].sensor_v_out_total 	 = g_p2p_calc_in[i].sensor_v_out_total + g_s2y_mng.sen.moni_extend_hd + (g_s2y_mng.sen.moni_extend_hd % 2); // even
			}
			
			g_p2p_calc_in[i].sensor_h_effective_full	=  CAMERA.sensor[i].mode[ CAMERA.sensor[i].mode_full ].active_pix_width;
			g_p2p_calc_in[i].sensor_v_effective_full	=  CAMERA.sensor[i].mode[ CAMERA.sensor[i].mode_full ].active_pix_lines;
			
			g_p2p_calc_in[i].sensor_h_effective_rsz_ratio		= CAMERA.sensor[i].mode[ sensor_mode ].active_pix_x_rsz_ratio;
			g_p2p_calc_in[i].sensor_h_effective_rsz_ratio_full	= CAMERA.sensor[i].mode[ CAMERA.sensor[i].mode_full ].active_pix_x_rsz_ratio;
			g_p2p_calc_in[i].sensor_v_effective_rsz_ratio		= CAMERA.sensor[i].mode[ sensor_mode ].active_pix_y_rsz_ratio;
			g_p2p_calc_in[i].sensor_v_effective_rsz_ratio_full	= CAMERA.sensor[i].mode[ CAMERA.sensor[i].mode_full ].active_pix_y_rsz_ratio;
			
			// change for stitch
			if( g_s2y_in.one_sensor_yuv_user_crop_flg[i] ){
				g_p2p_calc_in[i].sensor_h_effective_ofst = g_s2y_in.one_sensor_yuv_user_crop_h_ofst[i];
				g_p2p_calc_in[i].sensor_h_effective_size = g_s2y_in.one_sensor_yuv_user_crop_h_size[i];
				g_p2p_calc_in[i].sensor_v_effective_ofst = g_s2y_in.one_sensor_yuv_user_crop_v_ofst[i];
				g_p2p_calc_in[i].sensor_v_effective_size = g_s2y_in.one_sensor_yuv_user_crop_v_size[i];
			}
			// Center
			g_p2p_calc_in[i].sensor_h_effective_center = g_s2y_in.one_sensor_h_effective_center[i];
			g_p2p_calc_in[i].sensor_v_effective_center = g_s2y_in.one_sensor_v_effective_center[i];
			g_p2p_calc_in[i].sensor_h_effective_radius = g_s2y_in.one_sensor_h_effective_radius[i];
			g_p2p_calc_in[i].sensor_v_effective_radius = g_s2y_in.one_sensor_v_effective_radius[i];
			
			g_p2p_calc_in[i].pipe_h_out_size = g_s2y_in.one_sensor_yuv_h_size[i];
			g_p2p_calc_in[i].pipe_v_out_size = g_s2y_in.one_sensor_yuv_v_size[i];
			g_p2p_calc_in[i].pipe_h_out_size_r2y_ch1 = g_s2y_in.one_sensor_yuv_h_size_r2y_ch1[i];
			g_p2p_calc_in[i].pipe_v_out_size_r2y_ch1 = g_s2y_in.one_sensor_yuv_v_size_r2y_ch1[i];

			g_p2p_calc_in[i].none_resize		= g_s2y_in.one_sensor_yuv_none_resize[i];
			g_p2p_calc_in[i].v_trim_flg			= FALSE;
			g_p2p_calc_in[i].af_enable			= g_s2y_in.af_en;
			g_p2p_calc_in[i].ltm_enable			= g_s2y_in.ltm_mode;
			g_p2p_calc_in[i].raw_wo_mergin		= 1;
			g_p2p_calc_in[i].raw_save_h_total	= g_s2y_in.raw_save_h_total;
			g_p2p_calc_in[i].r2y_video_format	= g_s2y_in.r2y_video_format;
			
			g_p2p_calc_in[i].pipe_h_out_ofst	= g_s2y_in.pipe_ofst_h[i];
			g_p2p_calc_in[i].pipe_v_out_ofst	= g_s2y_in.pipe_ofst_v[i];
			
			g_p2p_calc_in[i].aeawb_h_blk_num	= g_s2y_in.awb_h_blknum; // ~64(~32 if level awb is on)
			g_p2p_calc_in[i].aeawb_v_blk_num	= g_s2y_in.awb_v_blknum; 
			g_p2p_calc_in[i].aeyk[E_AEYK_R]		= 0x004C;
			g_p2p_calc_in[i].aeyk[E_AEYK_GR]	= 0x004A;
			g_p2p_calc_in[i].aeyk[E_AEYK_GB]	= 0x004A;
			g_p2p_calc_in[i].aeyk[E_AEYK_B]		= 0x001C;
			g_p2p_calc_in[i].aeawb_level_awb	= g_s2y_in.awb_stat_lvl;
			if (g_s2y_in.awb_h_blknum > 32 && g_s2y_in.awb_stat_lvl) {
				UPRINTF_ERR(-3);
				return D_STM_NG;
			}

			if (g_s2y_in.awb_h_blknum < 32 || g_s2y_in.awb_v_blknum < 32) {
				// HSC block number is 32x32, so min AWB blk is 32x32
				UPRINTF_ERR(-3);
				return D_STM_NG;
			}
			
			g_p2p_calc_in[i].shd_tbl_h_byte    = g_s2y_in.shd_tbl_h_byte;
			g_p2p_calc_in[i].shd_tbl_h_blk_num = g_s2y_in.shd_tbl_h_blk_num;
			g_p2p_calc_in[i].shd_tbl_v_blk_num = g_s2y_in.shd_tbl_v_blk_num;
			
			// sen
			g_s2y_mng.sen.first_pixel[i]		= g_s2y_in.dol_sencore_use == 0x3 ?
				CAMERA.sensor[i].mode[ sensor_mode ].dol_first_active_pix_color :
				CAMERA.sensor[i].mode[ sensor_mode ].first_active_pix_color;
			if( g_s2y_in.dol_sencore_use == 0x03 ) {
				g_s2y_mng.sen.bitwidth[i]			= CAMERA.sensor[i].mode[ sensor_mode ].dol_input_bit_width;			// need error check
			} else {
				g_s2y_mng.sen.bitwidth[i]			= CAMERA.sensor[i].mode[ sensor_mode ].input_bit_width;			// need error check
			}
			g_s2y_mng.sen.ob_14bit[i].R			= (-1) * (SHORT)CAMERA.sensor[i].ob_12bit * 4;
			g_s2y_mng.sen.ob_14bit[i].Gr		= g_s2y_mng.sen.ob_14bit[i].R;
			g_s2y_mng.sen.ob_14bit[i].Gb		= g_s2y_mng.sen.ob_14bit[i].R;
			g_s2y_mng.sen.ob_14bit[i].B			= g_s2y_mng.sen.ob_14bit[i].R;

			g_s2y_mng.sen.sensor_connect[i]		= CAMERA.sensor[i].basic->sensor_if;							// all camera should be same
			// the sensor update timing follows the setting of sensor A.
			if(i == A){
				g_s2y_mng.sen.update_timing		= CAMERA.sensor[A].basic->update_timing;
			}
			g_s2y_mng.sen.start_v_blank[i]		= (
					CAMERA.sensor[i].mode[ sensor_mode ].active_pix_y_pos +
					CAMERA.sensor[i].mode[ sensor_mode ].active_pix_lines - 1);

			// dmipi
			switch( g_s2y_mng.sen.sensor_connect[i] ){
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI:
				g_s2y_mng.cphy_dmipi.bitwidth[i]		= g_s2y_mng.sen.bitwidth[i];
				g_s2y_mng.cphy_dmipi.dphy_1lane_Mbps[i]	= CAMERA.sensor[i].mode[ sensor_mode ].one_lane_Mbps;
				g_s2y_mng.cphy_dmipi.lane_num[i]		= CAMERA.sensor[i].mode[ sensor_mode ].lane_num;
			break;
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_SLVS:
				g_s2y_mng.cphy_slvs.linelength	= CAMERA.sensor[i].mode[ sensor_mode ].total_pix_width;
				g_s2y_mng.cphy_slvs.bitwidth	= g_s2y_mng.sen.bitwidth[i];
				g_s2y_mng.cphy_slvs.lane_num	= CAMERA.sensor[i].mode[ sensor_mode ].lane_num;
			break;
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_LVDS:
				g_s2y_mng.cphy_lvds.ch				= (E_IM_PRO_LVDS_CH)i;
				g_s2y_mng.cphy_lvds.lines			= CAMERA.sensor[i].mode[ sensor_mode ].active_pix_lines - 20; // margin
				g_s2y_mng.cphy_lvds.bitwidth		= g_s2y_mng.sen.bitwidth[i];
				g_s2y_mng.cphy_lvds.lane_num		= CAMERA.sensor[i].mode[ sensor_mode ].lane_num;
				g_s2y_mng.cphy_lvds.sync_code		= (T_IM_PRO_LVDS_SYNC_CODE *)CAMERA.sensor[i].mode[ sensor_mode ].sync_code;
				g_s2y_mng.cphy_lvds.sync_mask		= (T_IM_PRO_LVDS_SYNC_MASK *)CAMERA.sensor[i].mode[ sensor_mode ].sync_mask;
				g_s2y_mng.cphy_lvds.clock_select	= (E_IM_PRO_LVDS_SNCLK_SEL *)CAMERA.sensor[i].mode[ sensor_mode ].clock_select;
				g_s2y_mng.cphy_lvds.data_select		= (E_IM_PRO_LVDS_SNDATA_SEL *)CAMERA.sensor[i].mode[ sensor_mode ].data_select;
			break;
			default:
			break;
			}
			
			if(g_s2y_mng.sen.moni_extend_hd > 0){
				// MONI
				g_s2y_mng.sen.moni_exp_pix_cnt = CAMERA.sensor[i].mode[ sensor_mode ].total_pix_width;
				g_s2y_mng.sen.moni_exp_hd_cnt = CAMERA.sensor[i].mode[ sensor_mode ].total_pix_lines;
			}
		}
	}

	if( g_s2y_in.raw_only_sen_id.bit.sen_id != 0xFF ) {
		g_p2p_calc_in[g_s2y_in.raw_only_sen_id.bit.sen_id].pipe_h_out_size	= g_p2p_calc_in[g_s2y_in.raw_only_sen_id.bit.sen_id].sensor_h_effective_size;
		g_p2p_calc_in[g_s2y_in.raw_only_sen_id.bit.sen_id].pipe_v_out_size	= g_p2p_calc_in[g_s2y_in.raw_only_sen_id.bit.sen_id].sensor_v_effective_size - D_SHARE_PIPE_V_OFFSET - D_SHARE_PIPE_V_LOST;
	}

	/* Calc Size */
	available_div[A] = P2P_LDIV_PIPE_Calc(&g_p2p_calc_in[A], &g_p2p_calc_out[A]);
	if( g_s2y_in.sensor_num >= E_SHARE_SENSOR_NUM_2 ){
		available_div[B] = P2P_LDIV_PIPE_Calc(&g_p2p_calc_in[B], &g_p2p_calc_out[B]);
	}else{
		// copy for safety and easy to calc
		B = A;
	}
	if( g_s2y_in.sensor_num >= E_SHARE_SENSOR_NUM_4 ){
		available_div[C] = P2P_LDIV_PIPE_Calc(&g_p2p_calc_in[C], &g_p2p_calc_out[C]);
		available_div[D] = P2P_LDIV_PIPE_Calc(&g_p2p_calc_in[D], &g_p2p_calc_out[D]);
	}else{
		// copy for safety and easy to calc
		C = A;
		D = B;
	}

	// debug for sensor dividing forced
	if (g_s2y_in.sensor_div_forced) {
		available_div[A] &= g_s2y_in.sensor_div_forced;
		if ( g_s2y_in.sensor_num >= E_SHARE_SENSOR_NUM_2 ) {
			available_div[B] &= g_s2y_in.sensor_div_forced;
		}
		if ( g_s2y_in.sensor_num >= E_SHARE_SENSOR_NUM_4 ) {
			available_div[C] &= g_s2y_in.sensor_div_forced;
			available_div[D] &= g_s2y_in.sensor_div_forced;
		}
	}
	
	/* Decide Mode */
	switch( g_s2y_in.sensor_num ){
	case E_SHARE_SENSOR_NUM_4:
		if( (available_div[A] & available_div[B] & available_div[C] & available_div[D] & E_SHARE_H_DIV_1) &&
		    (g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_1-1].in_h_size <= D_LDIV_RAM_SIZE) && 
		    (g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_1-1].in_h_size <= D_LDIV_RAM_SIZE) && 
		    (g_p2p_calc_out[C].ldiv[E_SHARE_H_DIV_1-1].in_h_size <= D_LDIV_RAM_SIZE) && 
		    (g_p2p_calc_out[D].ldiv[E_SHARE_H_DIV_1-1].in_h_size <= D_LDIV_RAM_SIZE)
		  ){
			g_s2y_mng.pipe_mode = E_PIPE_MODE_SENSORx4;
		}else{
			// fail safe
			if( D_STM_NG == adjust_sensor_input( available_div[A], A, E_SHARE_H_DIV_1, D_LDIV_RAM_SIZE ) ){
				return D_STM_NG;
			}
			if( D_STM_NG == adjust_sensor_input( available_div[B], B, E_SHARE_H_DIV_1, D_LDIV_RAM_SIZE ) ){
				return D_STM_NG;
			}
			if( D_STM_NG == adjust_sensor_input( available_div[C], C, E_SHARE_H_DIV_1, D_LDIV_RAM_SIZE ) ){
				return D_STM_NG;
			}
			if( D_STM_NG == adjust_sensor_input( available_div[D], D, E_SHARE_H_DIV_1, D_LDIV_RAM_SIZE ) ){
				return D_STM_NG;
			}
			g_s2y_mng.pipe_mode = E_PIPE_MODE_SENSORx4;
		}
	break;
	case E_SHARE_SENSOR_NUM_2:
		if( (available_div[A] & available_div[B] & E_SHARE_H_DIV_1) && 
		    (g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_1-1].in_h_size <= D_LDIV_RAM_SIZE*2) && 
		    (g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_1-1].in_h_size <= D_LDIV_RAM_SIZE*2)
		  ){
			if( ( A == E_SHARE_SENSOR_ID_0 && B == E_SHARE_SENSOR_ID_1 ) || ( A == E_SHARE_SENSOR_ID_1 && B == E_SHARE_SENSOR_ID_0 ) ){
				g_s2y_mng.pipe_mode = E_PIPE_MODE_SENSORx2_DIRECT;
			}else{
				return D_STM_NG;
			}
		}else if( (available_div[A] & available_div[B] & E_SHARE_H_DIV_2) &&
				 (g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_2-1].in_h_size <= D_LDIV_RAM_SIZE*2) && 
				 (g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_2-1].in_h_size <= D_LDIV_RAM_SIZE*2)
				){
			if( ( A == E_SHARE_SENSOR_ID_0 && B == E_SHARE_SENSOR_ID_1 ) || ( A == E_SHARE_SENSOR_ID_1 && B == E_SHARE_SENSOR_ID_0 ) ){
				g_s2y_mng.pipe_mode = E_PIPE_MODE_SENSORx2_DIV2;
			}else{
				return D_STM_NG;
			}
		}else{
			// fail safe
			if( D_STM_NG == adjust_sensor_input( available_div[A], A, E_SHARE_H_DIV_2, D_LDIV_RAM_SIZE*2 ) ){
				return D_STM_NG;
			}
			if( D_STM_NG == adjust_sensor_input( available_div[B], B, E_SHARE_H_DIV_2, D_LDIV_RAM_SIZE*2 ) ){
				return D_STM_NG;
			}
			g_s2y_mng.pipe_mode = E_PIPE_MODE_SENSORx2_DIV2;
		}
	break;
	case E_SHARE_SENSOR_NUM_1:
	default:
		if( available_div[A] & E_SHARE_H_DIV_1 ){
			g_s2y_mng.div_num = E_SHARE_H_DIV_1;
			if( A == E_SHARE_SENSOR_ID_0 ){
				if( g_s2y_in.dol_sencore_use == 0x03 ) {
					g_s2y_mng.pipe_mode = E_PIPE_MODE_SENSORx1_DOL;
				} else {
				g_s2y_mng.pipe_mode = E_PIPE_MODE_SENSORx1_DIRECT; // can be E_PIPE_MODE_SENSORx1_XX
				}
			}else{
				g_s2y_mng.pipe_mode = E_PIPE_MODE_SENSORx1; // can be E_PIPE_MODE_SENSORx1_DIV4
			}
		}else if( available_div[A] & E_SHARE_H_DIV_2 ){
			if( A == E_SHARE_SENSOR_ID_0 ){
				if( g_s2y_in.dol_sencore_use == 0x03 ) {
					g_s2y_mng.pipe_mode = E_PIPE_MODE_SENSORx1_DOL_DIV2;
				} else {
				g_s2y_mng.pipe_mode = E_PIPE_MODE_SENSORx1_DIRECT_DIV2; // can be E_PIPE_MODE_SENSORx1_DIV4
				}
			}else{
				return D_STM_NG;
			}
		}else if( available_div[A] & E_SHARE_H_DIV_4 ){
			if( A == E_SHARE_SENSOR_ID_0 ){
				g_s2y_mng.pipe_mode = E_PIPE_MODE_SENSORx1_DIV4;
			}else{
				return D_STM_NG;
			}
		}else{
			// fail safe
			if( D_STM_NG == adjust_sensor_input( available_div[A], A, E_SHARE_H_DIV_2, D_LDIV_RAM_SIZE*2 ) ){
				return D_STM_NG;
			}
			g_s2y_mng.pipe_mode = E_PIPE_MODE_SENSORx1_DIRECT_DIV2;
		}
	break;
	}
	
	/* Calc AF */
	if( g_s2y_in.af_en & 0x01 ){
		Get_Share_AF_Param( &g_s2y_mng.af_param_share );
		g_s2y_mng.af_param = g_s2y_mng.af_param_share;
		calc_af(0, &g_s2y_mng.af_param);
	}

	/* Set From Pipe Mode */
	switch( g_s2y_mng.pipe_mode ){
	case E_PIPE_MODE_SENSORx1_DIRECT:
		UPRINTF("pipe_mode: sensor x 1 direct\n");
		UPRINTF("ldiv\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_1 - 1].out_h_ofst[0], g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_1 - 1].out_h_size[0]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[A].ldiv_pwch_v.ofst, g_p2p_calc_out[A].ldiv_pwch_v.size);

		if( g_s2y_in.dualpd_sencore_use == 0x03 ) {
			UPRINTF("PDAF: Dual PD\n");

			UPRINTF("ldiv1\n");
			UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_2 - 1].out_h_ofst[0], g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_2 - 1].out_h_size[0]);
			UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[B].ldiv_pwch_v.ofst, g_p2p_calc_out[B].ldiv_pwch_v.size);
			g_s2y_mng.pipe_use_num = E_PIPE_USE_NUM_1;
			g_s2y_mng.is_direct = FALSE;
			g_s2y_mng.div_num = E_SHARE_H_DIV_2;
		}else{
			g_s2y_mng.pipe_use_num = E_PIPE_USE_NUM_1;
			g_s2y_mng.is_direct = TRUE;
			g_s2y_mng.div_num = E_SHARE_H_DIV_1;
		}
	break;
	case E_PIPE_MODE_SENSORx1_DIRECT_DIV2:
		UPRINTF("pipe_mode: sensor x 1 direct div 2\n");
		UPRINTF("ldiv0-0\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_2 - 1].out_h_ofst[0], g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_2 - 1].out_h_size[0]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[A].ldiv_pwch_v.ofst, g_p2p_calc_out[A].ldiv_pwch_v.size);
		
		UPRINTF("ldiv0-1\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_2 - 1].out_h_ofst[1], g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_2 - 1].out_h_size[1]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[A].ldiv_pwch_v.ofst, g_p2p_calc_out[A].ldiv_pwch_v.size);

		if( g_s2y_in.dualpd_sencore_use == 0x03 ) {
			UPRINTF("PDAF: Dual PD\n");

			m = (CAMERA.sensor[0].mode[ g_s2y_in.sensor_mode[0] ].dualpd_line_rsz_rate >> 8) & 0x00FF;
			n = (CAMERA.sensor[0].mode[ g_s2y_in.sensor_mode[0] ].dualpd_line_rsz_rate) & 0x00FF;
			if((m != 0) && (n != 0) && (m < n)){
				g_p2p_pwch_pd[0].v.ofst = 0;
				g_p2p_pwch_pd[0].v.size = g_p2p_calc_out[B].ldiv_pwch_v.size * m / n - 2;
				g_p2p_pwch_pd[1].v.ofst = 0;
				g_p2p_pwch_pd[1].v.size = g_p2p_calc_out[B].ldiv_pwch_v.size * m / n - 2;
			}else{
				UPRINTF("Dual PD Line_Resize Rate Error! (%d/%d)\n",m ,n);
				UPRINTF_ERR(1);
				g_p2p_pwch_pd[0].v.ofst = g_p2p_calc_out[B].ldiv_pwch_v.ofst;
				g_p2p_pwch_pd[0].v.size = g_p2p_calc_out[B].ldiv_pwch_v.size;
				g_p2p_pwch_pd[1].v.ofst = g_p2p_calc_out[B].ldiv_pwch_v.ofst;
				g_p2p_pwch_pd[1].v.size = g_p2p_calc_out[B].ldiv_pwch_v.size;
			}

			UPRINTF("ldiv1-0\n");
			UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_2 - 1].out_h_ofst[0], g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_2 - 1].out_h_size[0]);
			UPRINTF("V offset:%d, V size:%d\n", g_p2p_pwch_pd[0].v.ofst, g_p2p_pwch_pd[0].v.size);
			
			UPRINTF("ldiv1-1\n");
			UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_2 - 1].out_h_ofst[1], g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_2 - 1].out_h_size[1]);
			UPRINTF("V offset:%d, V size:%d\n", g_p2p_pwch_pd[1].v.ofst, g_p2p_pwch_pd[1].v.size);

			g_s2y_mng.pipe_use_num = E_PIPE_USE_NUM_2;
			g_s2y_mng.is_direct = FALSE;
			g_s2y_mng.div_num = E_SHARE_H_DIV_2;
		}else if(g_s2y_in.af_en & 0x04) {
			UPRINTF("PDAF: PD lines(%d)\n", g_s2y_mng.sen.pd_extend);

			g_s2y_mng.pipe_use_num = E_PIPE_USE_NUM_2;
			g_s2y_mng.is_direct = TRUE;
			g_s2y_mng.div_num = E_SHARE_H_DIV_2;
		}else{
			g_s2y_mng.pipe_use_num = E_PIPE_USE_NUM_2;
			g_s2y_mng.is_direct = TRUE;
			g_s2y_mng.div_num = E_SHARE_H_DIV_2;
		}
	break;
	case E_PIPE_MODE_SENSORx2_DIRECT:
	case E_PIPE_MODE_SENSORx1_DOL:
		UPRINTF("pipe_mode: sensor x 2 direct or sensor x1 DOL direct\n");
		UPRINTF("ldiv0\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_1 - 1].out_h_ofst[0], g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_1 - 1].out_h_size[0]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[A].ldiv_pwch_v.ofst, g_p2p_calc_out[A].ldiv_pwch_v.size);

		UPRINTF("ldiv1\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_1 - 1].out_h_ofst[0], g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_1 - 1].out_h_size[0]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[B].ldiv_pwch_v.ofst, g_p2p_calc_out[B].ldiv_pwch_v.size);
		g_s2y_mng.pipe_use_num = E_PIPE_USE_NUM_2;
		g_s2y_mng.is_direct = TRUE;
		g_s2y_mng.div_num = E_SHARE_H_DIV_1;
	break;
	case E_PIPE_MODE_SENSORx1:
		UPRINTF("pipe_mode: sensor x 1\n");
		UPRINTF("ldiv\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_1 - 1].out_h_ofst[0], g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_1 - 1].out_h_size[0]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[A].ldiv_pwch_v.ofst, g_p2p_calc_out[A].ldiv_pwch_v.size);
		g_s2y_mng.pipe_use_num = E_PIPE_USE_NUM_1;
		g_s2y_mng.is_direct = FALSE;
		g_s2y_mng.div_num = E_SHARE_H_DIV_1;
	break;
	case E_PIPE_MODE_SENSORx1_DIV4:
		UPRINTF("pipe_mode: sensor x 1 div 4\n");
		UPRINTF("ldiv0-0\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_4 - 1].out_h_ofst[0], g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_4 - 1].out_h_size[0]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[A].ldiv_pwch_v.ofst, g_p2p_calc_out[A].ldiv_pwch_v.size);

		UPRINTF("ldiv0-1\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_4 - 1].out_h_ofst[1], g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_4 - 1].out_h_size[1]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[A].ldiv_pwch_v.ofst, g_p2p_calc_out[A].ldiv_pwch_v.size);

		UPRINTF("ldiv0-2\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_4 - 1].out_h_ofst[2], g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_4 - 1].out_h_size[2]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[A].ldiv_pwch_v.ofst, g_p2p_calc_out[A].ldiv_pwch_v.size);

		UPRINTF("ldiv0-3\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_4 - 1].out_h_ofst[3], g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_4 - 1].out_h_size[3]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[A].ldiv_pwch_v.ofst, g_p2p_calc_out[A].ldiv_pwch_v.size);
		g_s2y_mng.pipe_use_num = E_PIPE_USE_NUM_4;
		g_s2y_mng.is_direct = FALSE;
		g_s2y_mng.div_num = E_SHARE_H_DIV_4;
	break;
	case E_PIPE_MODE_SENSORx2_DIV2:
	case E_PIPE_MODE_SENSORx1_DOL_DIV2:
		UPRINTF("pipe_mode: sensor x 2 or sensor x1 DOL div 2\n");
		UPRINTF("ldiv0-0\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_2 - 1].out_h_ofst[0], g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_2 - 1].out_h_size[0]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[A].ldiv_pwch_v.ofst, g_p2p_calc_out[A].ldiv_pwch_v.size);

		UPRINTF("ldiv0-1\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_2 - 1].out_h_ofst[1], g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_2 - 1].out_h_size[1]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[A].ldiv_pwch_v.ofst, g_p2p_calc_out[A].ldiv_pwch_v.size);
		
		UPRINTF("ldiv1-0\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_2 - 1].out_h_ofst[0], g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_2 - 1].out_h_size[0]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[B].ldiv_pwch_v.ofst, g_p2p_calc_out[B].ldiv_pwch_v.size);

		UPRINTF("ldiv1-1\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_2 - 1].out_h_ofst[1], g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_2 - 1].out_h_size[1]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[B].ldiv_pwch_v.ofst, g_p2p_calc_out[B].ldiv_pwch_v.size);
		if( g_s2y_in.raw_only_sen_id.bit.sen_id != 0xFF ) {
			g_s2y_mng.pipe_use_num = E_PIPE_USE_NUM_2;
		} else {
			g_s2y_mng.pipe_use_num = E_PIPE_USE_NUM_4;
		}
		g_s2y_mng.is_direct = FALSE;
		g_s2y_mng.div_num = E_SHARE_H_DIV_2;
	break;
	case E_PIPE_MODE_SENSORx4:
		UPRINTF("pipe_mode: sensor x 4\n");
		UPRINTF("ldiv0\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_1 - 1].out_h_ofst[0], g_p2p_calc_out[A].ldiv[E_SHARE_H_DIV_1 - 1].out_h_size[0]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[A].ldiv_pwch_v.ofst, g_p2p_calc_out[A].ldiv_pwch_v.size);

		UPRINTF("ldiv1\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_1 - 1].out_h_ofst[0], g_p2p_calc_out[B].ldiv[E_SHARE_H_DIV_1 - 1].out_h_size[0]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[B].ldiv_pwch_v.ofst, g_p2p_calc_out[B].ldiv_pwch_v.size);

		UPRINTF("ldiv2\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[C].ldiv[E_SHARE_H_DIV_1 - 1].out_h_ofst[0], g_p2p_calc_out[C].ldiv[E_SHARE_H_DIV_1 - 1].out_h_size[0]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[C].ldiv_pwch_v.ofst, g_p2p_calc_out[C].ldiv_pwch_v.size);

		UPRINTF("ldiv3\n");
		UPRINTF("H offset:%d, H size:%d\n", g_p2p_calc_out[D].ldiv[E_SHARE_H_DIV_1 - 1].out_h_ofst[0], g_p2p_calc_out[D].ldiv[E_SHARE_H_DIV_1 - 1].out_h_size[0]);
		UPRINTF("V offset:%d, V size:%d\n", g_p2p_calc_out[D].ldiv_pwch_v.ofst, g_p2p_calc_out[D].ldiv_pwch_v.size);
		g_s2y_mng.pipe_use_num = E_PIPE_USE_NUM_4;
		g_s2y_mng.is_direct = FALSE;
		g_s2y_mng.div_num = E_SHARE_H_DIV_1;
	break;
	default:
	break;
	}

	if( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT ) {
		// Forced RAW output to B2B, for LTM non-direct mode
		// SRO->B2B-RAW
		// RAW->B2R->LTM->R2Y
		g_s2y_in.raw_out_point_forced = RAW_EXTRACT_POINT_B2B;
		g_s2y_mng.is_direct = FALSE;

		// decrease V size of B2B RAW only for PIPE num < 4
		if( g_s2y_mng.pipe_use_num < E_PIPE_USE_NUM_4 ) {
			for(i = 0; i < 4; i++) {
				g_p2p_calc_out[i].ldiv_pwch_v.size = g_s2y_in.cag_en ? g_p2p_calc_out[i].ldiv_pwch_v.size - PIPE_LOST_LINE_DPC_CAG_ELF : g_p2p_calc_out[i].ldiv_pwch_v.size - PIPE_LOST_LINE_DPC_ELF;
				g_p2p_calc_out[i].sro_pg_v.size = g_p2p_calc_out[i].ldiv_pwch_v.size;
				UPRINTF("LTM_NON_DIRECT_MODE: sensor[%d] V size:%d\n", i, g_p2p_calc_out[i].ldiv_pwch_v.size);
			}
		}
	}

	Get_Share_System_Param(&param);
	param.div_num = g_s2y_mng.div_num;
	Set_Share_System_Param( &param );

	/* Get from Input */
	g_s2y_mng.cnr_type = in->cnr_type;
	g_s2y_mng.is_r2y_out_ipu = in->is_r2y_out_ipu;
	g_s2y_mng.pipe[0].format = g_s2y_in.sensor_yuv_format;
	g_s2y_mng.pipe[1].format = g_s2y_in.sensor_yuv_format;
	g_s2y_mng.pipe[2].format = g_s2y_in.sensor_yuv_format;
	g_s2y_mng.pipe[3].format = g_s2y_in.sensor_yuv_format;
	if( g_s2y_in.sensor_yuv_format == E_SHARE_YUV_FORMAT_420_SEMI ){
		div_c = 2;
	}else{
		div_c = 1;
	}
	
	/* Get Use Parameter */
	g_s2y_mng.sen.ldiv[A] = &g_p2p_calc_out[A].ldiv[ (g_s2y_mng.div_num-1) ];
	// DOL case, LDIV1 will be used for 2nd frame of DOL
	if( g_s2y_in.dol_sencore_use == 0x03 ) {
		g_s2y_mng.sen.ldiv[1] = &g_p2p_calc_out[A].ldiv[ (g_s2y_mng.div_num-1) ];
	}
	// Dual PD case, LDIV1 will be used for another VC
	if( g_s2y_in.dualpd_sencore_use == 0x03 ) {
		g_s2y_mng.sen.ldiv[1] = &g_p2p_calc_out[A].ldiv[ (g_s2y_mng.div_num-1) ];
	}
	g_s2y_mng.sen.ldiv[B] = &g_p2p_calc_out[B].ldiv[ (g_s2y_mng.div_num-1) ];
	g_s2y_mng.sen.ldiv[C] = &g_p2p_calc_out[C].ldiv[ (g_s2y_mng.div_num-1) ];
	g_s2y_mng.sen.ldiv[D] = &g_p2p_calc_out[D].ldiv[ (g_s2y_mng.div_num-1) ];

	switch( g_s2y_mng.div_num ){
	case E_SHARE_H_DIV_1:
		g_s2y_mng.pipe[0].h = &g_p2p_calc_out[A].pipe_h[E_PIPE_DIV_PART_1];
		g_s2y_mng.pipe[1].h = &g_p2p_calc_out[B].pipe_h[E_PIPE_DIV_PART_1];
		g_s2y_mng.pipe[2].h = &g_p2p_calc_out[C].pipe_h[E_PIPE_DIV_PART_1];
		g_s2y_mng.pipe[3].h = &g_p2p_calc_out[D].pipe_h[E_PIPE_DIV_PART_1];
		
		g_s2y_mng.ldiv_pwch[0].h = &g_p2p_calc_out[A].ldiv_pwch_h[E_PIPE_DIV_PART_1];
		g_s2y_mng.ldiv_pwch[1].h = &g_p2p_calc_out[B].ldiv_pwch_h[E_PIPE_DIV_PART_1];
		g_s2y_mng.ldiv_pwch[2].h = &g_p2p_calc_out[C].ldiv_pwch_h[E_PIPE_DIV_PART_1];
		g_s2y_mng.ldiv_pwch[3].h = &g_p2p_calc_out[D].ldiv_pwch_h[E_PIPE_DIV_PART_1];
		
		g_s2y_mng.sro_pg[0].h = &g_p2p_calc_out[A].sro_pg_h[E_PIPE_DIV_PART_1];
		g_s2y_mng.sro_pg[1].h = &g_p2p_calc_out[B].sro_pg_h[E_PIPE_DIV_PART_1];
		g_s2y_mng.sro_pg[2].h = &g_p2p_calc_out[C].sro_pg_h[E_PIPE_DIV_PART_1];
		g_s2y_mng.sro_pg[3].h = &g_p2p_calc_out[D].sro_pg_h[E_PIPE_DIV_PART_1];
		
		if( g_s2y_in.af_en & 0x01 ){
			g_s2y_mng.af[0].h = &g_p2p_af_calc_out[A].af_h[E_PIPE_DIV_PART_1];
			g_s2y_mng.af[1].h = &g_p2p_af_calc_out[B].af_h[E_PIPE_DIV_PART_1];
			g_s2y_mng.af[2].h = &g_p2p_af_calc_out[C].af_h[E_PIPE_DIV_PART_1];
			g_s2y_mng.af[3].h = &g_p2p_af_calc_out[D].af_h[E_PIPE_DIV_PART_1];

			g_s2y_mng.af[0].v = &g_p2p_af_calc_out[A].af_v;
			g_s2y_mng.af[1].v = &g_p2p_af_calc_out[B].af_v;
			g_s2y_mng.af[2].v = &g_p2p_af_calc_out[C].af_v;
			g_s2y_mng.af[3].v = &g_p2p_af_calc_out[D].af_v;
		}

		g_s2y_mng.pipe[0].first_pixel = g_s2y_mng.sen.first_pixel[A];
		g_s2y_mng.pipe[1].first_pixel = g_s2y_mng.sen.first_pixel[B];
		g_s2y_mng.pipe[2].first_pixel = g_s2y_mng.sen.first_pixel[C];
		g_s2y_mng.pipe[3].first_pixel = g_s2y_mng.sen.first_pixel[D];
		
		g_s2y_mng.pipe[0].v = &g_p2p_calc_out[A].pipe_v;
		g_s2y_mng.pipe[1].v = &g_p2p_calc_out[B].pipe_v;
		g_s2y_mng.pipe[2].v = &g_p2p_calc_out[C].pipe_v;
		g_s2y_mng.pipe[3].v = &g_p2p_calc_out[D].pipe_v;
		
		g_s2y_mng.ldiv_pwch[0].v = &g_p2p_calc_out[A].ldiv_pwch_v;
		g_s2y_mng.ldiv_pwch[1].v = &g_p2p_calc_out[B].ldiv_pwch_v;
		g_s2y_mng.ldiv_pwch[2].v = &g_p2p_calc_out[C].ldiv_pwch_v;
		g_s2y_mng.ldiv_pwch[3].v = &g_p2p_calc_out[D].ldiv_pwch_v;
		
		g_s2y_mng.sro_pg[0].v = &g_p2p_calc_out[A].sro_pg_v;
		g_s2y_mng.sro_pg[1].v = &g_p2p_calc_out[B].sro_pg_v;
		g_s2y_mng.sro_pg[2].v = &g_p2p_calc_out[C].sro_pg_v;
		g_s2y_mng.sro_pg[3].v = &g_p2p_calc_out[D].sro_pg_v;
		
	break;
	case E_SHARE_H_DIV_2:
		g_s2y_mng.pipe[0].h = &g_p2p_calc_out[A].pipe_h[E_PIPE_DIV_PART_2L];
		g_s2y_mng.pipe[1].h = &g_p2p_calc_out[A].pipe_h[E_PIPE_DIV_PART_2R];
		g_s2y_mng.pipe[2].h = &g_p2p_calc_out[B].pipe_h[E_PIPE_DIV_PART_2L];
		g_s2y_mng.pipe[3].h = &g_p2p_calc_out[B].pipe_h[E_PIPE_DIV_PART_2R];
		
		g_s2y_mng.ldiv_pwch[0].h = &g_p2p_calc_out[A].ldiv_pwch_h[E_PIPE_DIV_PART_2L];
		g_s2y_mng.ldiv_pwch[1].h = &g_p2p_calc_out[A].ldiv_pwch_h[E_PIPE_DIV_PART_2R];
		g_s2y_mng.ldiv_pwch[2].h = &g_p2p_calc_out[B].ldiv_pwch_h[E_PIPE_DIV_PART_2L];
		g_s2y_mng.ldiv_pwch[3].h = &g_p2p_calc_out[B].ldiv_pwch_h[E_PIPE_DIV_PART_2R];
		
		g_s2y_mng.sro_pg[0].h = &g_p2p_calc_out[A].sro_pg_h[E_PIPE_DIV_PART_2L];
		g_s2y_mng.sro_pg[1].h = &g_p2p_calc_out[A].sro_pg_h[E_PIPE_DIV_PART_2R];
		g_s2y_mng.sro_pg[2].h = &g_p2p_calc_out[B].sro_pg_h[E_PIPE_DIV_PART_2L];
		g_s2y_mng.sro_pg[3].h = &g_p2p_calc_out[B].sro_pg_h[E_PIPE_DIV_PART_2R];
		
		if( g_s2y_in.af_en & 0x01 ){
			g_s2y_mng.af[0].h = &g_p2p_af_calc_out[A].af_h[E_PIPE_DIV_PART_2L];
			g_s2y_mng.af[1].h = &g_p2p_af_calc_out[A].af_h[E_PIPE_DIV_PART_2R];
			g_s2y_mng.af[2].h = &g_p2p_af_calc_out[B].af_h[E_PIPE_DIV_PART_2L];
			g_s2y_mng.af[3].h = &g_p2p_af_calc_out[B].af_h[E_PIPE_DIV_PART_2R];

			g_s2y_mng.af[0].v = &g_p2p_af_calc_out[A].af_v;
			g_s2y_mng.af[1].v = &g_p2p_af_calc_out[A].af_v;
			g_s2y_mng.af[2].v = &g_p2p_af_calc_out[B].af_v;
			g_s2y_mng.af[3].v = &g_p2p_af_calc_out[B].af_v;
		}

		g_s2y_mng.pipe[0].first_pixel = g_s2y_mng.sen.first_pixel[A];
		g_s2y_mng.pipe[1].first_pixel = g_s2y_mng.sen.first_pixel[A];
		g_s2y_mng.pipe[2].first_pixel = g_s2y_mng.sen.first_pixel[B];
		g_s2y_mng.pipe[3].first_pixel = g_s2y_mng.sen.first_pixel[B];
		
		g_s2y_mng.pipe[0].v = &g_p2p_calc_out[A].pipe_v;
		g_s2y_mng.pipe[1].v = &g_p2p_calc_out[A].pipe_v;
		g_s2y_mng.pipe[2].v = &g_p2p_calc_out[B].pipe_v;
		g_s2y_mng.pipe[3].v = &g_p2p_calc_out[B].pipe_v;
		
		g_s2y_mng.ldiv_pwch[0].v = &g_p2p_calc_out[A].ldiv_pwch_v;
		g_s2y_mng.ldiv_pwch[1].v = &g_p2p_calc_out[A].ldiv_pwch_v;
		g_s2y_mng.ldiv_pwch[2].v = &g_p2p_calc_out[B].ldiv_pwch_v;
		g_s2y_mng.ldiv_pwch[3].v = &g_p2p_calc_out[B].ldiv_pwch_v;
		
		g_s2y_mng.sro_pg[0].v = &g_p2p_calc_out[A].sro_pg_v;
		g_s2y_mng.sro_pg[1].v = &g_p2p_calc_out[A].sro_pg_v;
		g_s2y_mng.sro_pg[2].v = &g_p2p_calc_out[B].sro_pg_v;
		g_s2y_mng.sro_pg[3].v = &g_p2p_calc_out[B].sro_pg_v;
	break;
	case E_SHARE_H_DIV_4:
		g_s2y_mng.pipe[0].h = &g_p2p_calc_out[A].pipe_h[E_PIPE_DIV_PART_4L];
		g_s2y_mng.pipe[1].h = &g_p2p_calc_out[A].pipe_h[E_PIPE_DIV_PART_4ML];
		g_s2y_mng.pipe[2].h = &g_p2p_calc_out[A].pipe_h[E_PIPE_DIV_PART_4MR];
		g_s2y_mng.pipe[3].h = &g_p2p_calc_out[A].pipe_h[E_PIPE_DIV_PART_4R];
		
		g_s2y_mng.ldiv_pwch[0].h = &g_p2p_calc_out[A].ldiv_pwch_h[E_PIPE_DIV_PART_4L];
		g_s2y_mng.ldiv_pwch[1].h = &g_p2p_calc_out[A].ldiv_pwch_h[E_PIPE_DIV_PART_4ML];
		g_s2y_mng.ldiv_pwch[2].h = &g_p2p_calc_out[A].ldiv_pwch_h[E_PIPE_DIV_PART_4MR];
		g_s2y_mng.ldiv_pwch[3].h = &g_p2p_calc_out[A].ldiv_pwch_h[E_PIPE_DIV_PART_4R];
		
		g_s2y_mng.sro_pg[0].h = &g_p2p_calc_out[A].sro_pg_h[E_PIPE_DIV_PART_4L];
		g_s2y_mng.sro_pg[1].h = &g_p2p_calc_out[A].sro_pg_h[E_PIPE_DIV_PART_4ML];
		g_s2y_mng.sro_pg[2].h = &g_p2p_calc_out[A].sro_pg_h[E_PIPE_DIV_PART_4MR];
		g_s2y_mng.sro_pg[3].h = &g_p2p_calc_out[A].sro_pg_h[E_PIPE_DIV_PART_4R];
		
		if( g_s2y_in.af_en & 0x01 ){
			g_s2y_mng.af[0].h = &g_p2p_af_calc_out[A].af_h[E_PIPE_DIV_PART_4L];
			g_s2y_mng.af[1].h = &g_p2p_af_calc_out[A].af_h[E_PIPE_DIV_PART_4ML];
			g_s2y_mng.af[2].h = &g_p2p_af_calc_out[A].af_h[E_PIPE_DIV_PART_4MR];
			g_s2y_mng.af[3].h = &g_p2p_af_calc_out[A].af_h[E_PIPE_DIV_PART_4R];
		}
		for (i = 0; i < 4; i++) {
			g_s2y_mng.pipe[i].first_pixel	= g_s2y_mng.sen.first_pixel[A];
			g_s2y_mng.pipe[i].v				= &g_p2p_calc_out[A].pipe_v;
			g_s2y_mng.ldiv_pwch[i].v		= &g_p2p_calc_out[A].ldiv_pwch_v;
			g_s2y_mng.sro_pg[i].v			= &g_p2p_calc_out[A].sro_pg_v;

			if( g_s2y_in.af_en & 0x01 ){
				g_s2y_mng.af[i].v 			= &g_p2p_af_calc_out[A].af_v;
			}
		}
	break;
	default:
		UPRINTF_ERR(1);
	break;
	};

	// Dual PD
	if( g_s2y_in.dualpd_sencore_use == 0x03 ) {
		g_s2y_mng.ldiv_pwch[2].v = &g_p2p_pwch_pd[0].v;
		g_s2y_mng.ldiv_pwch[3].v = &g_p2p_pwch_pd[1].v;
	}
	
	if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx2_DIV2) {
		g_s2y_mng.iq_stream[0] = A;
		g_s2y_mng.iq_stream[1] = A;
		g_s2y_mng.iq_stream[2] = B;
		g_s2y_mng.iq_stream[3] = B;
	}else{
		g_s2y_mng.iq_stream[0] = A;
		g_s2y_mng.iq_stream[1] = B;
		g_s2y_mng.iq_stream[2] = C;
		g_s2y_mng.iq_stream[3] = D;
	}
	
	/* SDC ( Bad Pixel )*/
	if( g_s2y_in.sdc_en ){
		set_sdc();
	}else{
		g_s2y_mng.pipe[0].sdc_en = g_s2y_mng.pipe[1].sdc_en = g_s2y_mng.pipe[2].sdc_en = g_s2y_mng.pipe[3].sdc_en = 0;
	}
	
	/* Calc offset and byte */
	if( g_s2y_in.sensor_num == E_SHARE_SENSOR_NUM_1 && g_s2y_in.dol_sencore_use != 0x03 && g_s2y_in.dualpd_sencore_use != 0x03){
		g_s2y_mng.h_byte_yc = g_s2y_mng.pipe[0].h->r2y_h_byte;
		if (r2y_ch1_enable)
			g_s2y_mng.h_byte_yc_ch1 = g_s2y_mng.pipe[0].h->r2y_h_byte_ch1;
		// end offset
		if (in->cap_mode) {
			g_s2y_mng.addr_ofst_y[4] = ROUNDUP16(g_s2y_mng.h_byte_yc) * ROUNDUP16(g_s2y_mng.pipe[0].v->r2y_y_trm_size);
			g_s2y_mng.addr_ofst_c[4] = ROUNDUP16(g_s2y_mng.h_byte_yc) * ROUNDUP16(g_s2y_mng.pipe[0].v->r2y_c_trm_size) / div_c;
		} else {
			if( in->r2y_video_format ){
				g_s2y_mng.addr_ofst_y[4] = ROUNDUP(g_s2y_mng.h_byte_yc, D_R2Y_VIDEO_FORMAT_H_BYTE) * ROUNDUP(g_s2y_mng.pipe[0].v->r2y_y_trm_size, D_R2Y_VIDEO_FORMAT_V_SIZE);
				g_s2y_mng.addr_ofst_c[4] = ROUNDUP(g_s2y_mng.h_byte_yc, D_R2Y_VIDEO_FORMAT_H_BYTE) * ROUNDUP(g_s2y_mng.pipe[0].v->r2y_c_trm_size/div_c, D_R2Y_VIDEO_FORMAT_V_SIZE);
			}else{
				g_s2y_mng.addr_ofst_y[4] = g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[0].v->r2y_y_trm_size;
				g_s2y_mng.addr_ofst_c[4] = g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[0].v->r2y_c_trm_size / div_c;
				if (r2y_ch1_enable) {
					g_s2y_mng.addr_ofst_y_ch1[4] = g_s2y_mng.h_byte_yc_ch1 * g_s2y_mng.pipe[0].v->r2y_y_trm_size_ch1;
					g_s2y_mng.addr_ofst_c_ch1[4] = g_s2y_mng.h_byte_yc_ch1 * g_s2y_mng.pipe[0].v->r2y_c_trm_size_ch1 / div_c;
				}
			}
		}
		for( i = 0; i < g_s2y_mng.div_num; i ++ ){
			g_s2y_mng.addr_ofst_raw[4] += g_s2y_mng.ldiv_pwch[i].h->byte;
			g_s2y_mng.addr_ofst_aeawb[4] += g_s2y_mng.pipe[i].h->aeawb_h_byte;
		}
		g_s2y_mng.addr_ofst_raw[4] *= g_s2y_mng.ldiv_pwch[0].v->size;
		g_s2y_mng.addr_ofst_aeawb[4] *= g_s2y_mng.pipe[0].v->aeawb_blk_num;
		
		if( g_p2p_calc_in[A].raw_wo_mergin ){ // overwrite if raw_wo_mergin
			g_s2y_mng.addr_ofst_raw[4] = g_s2y_mng.ldiv_pwch[0].h->byte * g_s2y_mng.ldiv_pwch[0].v->size;
		}

		if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_OFF) {
			g_s2y_mng.addr_ofst_map[4] = g_s2y_mng.pipe[0].h->ltmmap_h_byte * g_s2y_mng.pipe[0].v->ltmmap_size;
			if( g_s2y_mng.div_num == E_SHARE_H_DIV_4 ) {
				// only H offset, not whole image since only one RGB map
				g_s2y_mng.addr_ofst_maprgb[2] = ((g_s2y_mng.pipe[0].h->ltmrbk_size + g_s2y_mng.pipe[1].h->ltmrbk_size) / g_s2y_mng.pipe[0].v->ltm_ratio) * 3 * 2; // rect0 + rect1 RGB size / LTM ratio * 3 (RGB) * 2 (Unpack12bit)
				//end offset
				g_s2y_mng.addr_ofst_maprgb[4] = g_s2y_mng.pipe[0].h->ltmrgb_h_byte * g_s2y_mng.pipe[0].v->ltmmap_size;
			}
		}
		
	} else if( (g_s2y_in.sensor_num == E_SHARE_SENSOR_NUM_2) && (g_s2y_mng.div_num == E_SHARE_H_DIV_1) ){
		
		// sensor offset
		g_s2y_mng.addr_ofst_raw[1] = g_s2y_mng.ldiv_pwch[0].h->byte * g_s2y_mng.ldiv_pwch[0].v->size;
		g_s2y_mng.addr_ofst_aeawb[1] = g_s2y_mng.pipe[0].h->aeawb_h_byte * g_s2y_mng.pipe[0].v->aeawb_blk_num;
		// end offset
		g_s2y_mng.addr_ofst_raw[4] = g_s2y_mng.addr_ofst_raw[1] + (g_s2y_mng.ldiv_pwch[1].h->byte * g_s2y_mng.ldiv_pwch[1].v->size);
		g_s2y_mng.addr_ofst_aeawb[4] = g_s2y_mng.addr_ofst_aeawb[1] + (g_s2y_mng.pipe[1].h->aeawb_h_byte * g_s2y_mng.pipe[1].v->aeawb_blk_num);

		if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_OFF) {
			g_s2y_mng.addr_ofst_map[1] = g_s2y_mng.pipe[0].h->ltmmap_h_byte * g_s2y_mng.pipe[0].v->ltmmap_size;
			g_s2y_mng.addr_ofst_map[4] = g_s2y_mng.addr_ofst_map[1] + (g_s2y_mng.pipe[1].h->ltmmap_h_byte * g_s2y_mng.pipe[1].v->ltmmap_size);
		}

		switch( g_s2y_in.sensor_layout ){
		case E_SHARE_SENSOR_LAYOUT_H_LINEAR:
			// sensor offset
			g_s2y_mng.h_byte_yc = g_s2y_mng.pipe[0].h->r2y_h_byte * 2;
			g_s2y_mng.addr_ofst_y[1] = g_s2y_mng.pipe[0].h->r2y_h_byte;
			g_s2y_mng.addr_ofst_c[1] = g_s2y_mng.pipe[0].h->r2y_h_byte;
			// end offset
			g_s2y_mng.addr_ofst_y[4] = g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[1].v->r2y_y_trm_size;
			g_s2y_mng.addr_ofst_c[4] = g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[1].v->r2y_c_trm_size / div_c;
		break;
		case E_SHARE_SENSOR_LAYOUT_V_LINEAR:
		case E_SHARE_SENSOR_LAYOUT_H2V2:
			// sensor offset
			g_s2y_mng.h_byte_yc = g_s2y_mng.pipe[0].h->r2y_h_byte;
			g_s2y_mng.addr_ofst_y[1] = g_s2y_mng.pipe[0].h->r2y_h_byte * g_s2y_mng.pipe[0].v->r2y_y_trm_size;
			g_s2y_mng.addr_ofst_c[1] = g_s2y_mng.pipe[0].h->r2y_h_byte * g_s2y_mng.pipe[0].v->r2y_c_trm_size / div_c;
			// end offset
			g_s2y_mng.addr_ofst_y[4] = g_s2y_mng.addr_ofst_y[1] + ( g_s2y_mng.pipe[1].h->r2y_h_byte * g_s2y_mng.pipe[1].v->r2y_y_trm_size );
			g_s2y_mng.addr_ofst_c[4] = g_s2y_mng.addr_ofst_c[1] + ( g_s2y_mng.pipe[1].h->r2y_h_byte * g_s2y_mng.pipe[1].v->r2y_c_trm_size / div_c );
		break;
		default:
			UPRINTF_ERR(1);
		break;
		}
	}
	else if( (g_s2y_in.sensor_num == E_SHARE_SENSOR_NUM_2) && (g_s2y_mng.div_num == E_SHARE_H_DIV_2) ){
		// sensor offset
		g_s2y_mng.addr_ofst_raw[2] = (g_s2y_mng.ldiv_pwch[0].h->byte * g_s2y_mng.ldiv_pwch[0].v->size)
								   + (g_s2y_mng.ldiv_pwch[1].h->byte * g_s2y_mng.ldiv_pwch[1].v->size);
		if( g_p2p_calc_in[A].raw_wo_mergin ){ // overwrite if raw_wo_mergin
			g_s2y_mng.addr_ofst_raw[2] = g_s2y_mng.ldiv_pwch[0].h->byte * g_s2y_mng.ldiv_pwch[0].v->size;
		}
		g_s2y_mng.addr_ofst_raw[3] = g_s2y_mng.addr_ofst_raw[2];
		g_s2y_mng.addr_ofst_aeawb[2] = (g_s2y_mng.pipe[0].h->aeawb_h_byte * g_s2y_mng.pipe[0].v->aeawb_blk_num)
									 + (g_s2y_mng.pipe[1].h->aeawb_h_byte * g_s2y_mng.pipe[1].v->aeawb_blk_num);
		g_s2y_mng.addr_ofst_aeawb[3] = g_s2y_mng.addr_ofst_aeawb[2];
		// end offset
		g_s2y_mng.addr_ofst_raw[4] = (g_s2y_mng.ldiv_pwch[2].h->byte * g_s2y_mng.ldiv_pwch[2].v->size)
								   + (g_s2y_mng.ldiv_pwch[3].h->byte * g_s2y_mng.ldiv_pwch[3].v->size)
								   + g_s2y_mng.addr_ofst_raw[3];
		if( g_p2p_calc_in[B].raw_wo_mergin ){ // overwrite if raw_wo_mergin
			g_s2y_mng.addr_ofst_raw[4] = (g_s2y_mng.ldiv_pwch[2].h->byte * g_s2y_mng.ldiv_pwch[2].v->size)
									   + g_s2y_mng.addr_ofst_raw[3];
		}
		g_s2y_mng.addr_ofst_aeawb[4] = (g_s2y_mng.pipe[2].h->aeawb_h_byte * g_s2y_mng.pipe[2].v->aeawb_blk_num)
									 + (g_s2y_mng.pipe[3].h->aeawb_h_byte * g_s2y_mng.pipe[3].v->aeawb_blk_num)
									 + g_s2y_mng.addr_ofst_aeawb[3];

		if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_OFF) {
			g_s2y_mng.addr_ofst_map[2] = g_s2y_mng.pipe[0].h->ltmmap_h_byte * g_s2y_mng.pipe[0].v->ltmmap_size;
			g_s2y_mng.addr_ofst_map[4] = g_s2y_mng.addr_ofst_map[2] + (g_s2y_mng.pipe[2].h->ltmmap_h_byte * g_s2y_mng.pipe[2].v->ltmmap_size);
			// 2nd RGB map offset
			g_s2y_mng.addr_ofst_maprgb[2] = g_s2y_mng.pipe[0].h->ltmrgb_h_byte * g_s2y_mng.pipe[0].v->ltmmap_size;
			g_s2y_mng.addr_ofst_maprgb[4] = g_s2y_mng.addr_ofst_maprgb[2] + g_s2y_mng.pipe[2].h->ltmrgb_h_byte * g_s2y_mng.pipe[2].v->ltmmap_size;
		}

		switch( g_s2y_in.sensor_layout ){
		case E_SHARE_SENSOR_LAYOUT_H_LINEAR:
			// sensor offset
			g_s2y_mng.h_byte_yc = g_s2y_mng.pipe[0].h->r2y_h_byte * 2;
			g_s2y_mng.addr_ofst_y[2] = g_s2y_mng.pipe[0].h->r2y_h_byte;
			g_s2y_mng.addr_ofst_c[2] = g_s2y_mng.pipe[0].h->r2y_h_byte;
			g_s2y_mng.addr_ofst_y[3] = g_s2y_mng.addr_ofst_y[2];
			g_s2y_mng.addr_ofst_c[3] = g_s2y_mng.addr_ofst_c[2];
			// end offset
			g_s2y_mng.addr_ofst_y[4] = g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[3].v->r2y_y_trm_size;
			g_s2y_mng.addr_ofst_c[4] = g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[3].v->r2y_c_trm_size / div_c;
		break;
		case E_SHARE_SENSOR_LAYOUT_V_LINEAR:
		case E_SHARE_SENSOR_LAYOUT_H2V2:
			g_s2y_mng.h_byte_yc = g_s2y_mng.pipe[0].h->r2y_h_byte;
			g_s2y_mng.addr_ofst_y[2] = g_s2y_mng.pipe[0].h->r2y_h_byte * g_s2y_mng.pipe[0].v->r2y_y_trm_size;
			g_s2y_mng.addr_ofst_c[2] = g_s2y_mng.pipe[0].h->r2y_h_byte * g_s2y_mng.pipe[0].v->r2y_c_trm_size / div_c;
			g_s2y_mng.addr_ofst_y[3] = g_s2y_mng.addr_ofst_y[2];
			g_s2y_mng.addr_ofst_c[3] = g_s2y_mng.addr_ofst_c[2];
			// end offset
			g_s2y_mng.addr_ofst_y[4] = g_s2y_mng.addr_ofst_y[3] + ( g_s2y_mng.pipe[3].h->r2y_h_byte * g_s2y_mng.pipe[3].v->r2y_y_trm_size );
			g_s2y_mng.addr_ofst_c[4] = g_s2y_mng.addr_ofst_c[3] + ( g_s2y_mng.pipe[3].h->r2y_h_byte * g_s2y_mng.pipe[3].v->r2y_c_trm_size / div_c );
		break;
		default:
			UPRINTF_ERR(1);
		break;
		}
	}
	else if( g_s2y_in.sensor_num == E_SHARE_SENSOR_NUM_1 && g_s2y_in.dol_sencore_use == 0x03 && g_s2y_mng.div_num == E_SHARE_H_DIV_2 ) { // DOL div2
		// sencore offset
		g_s2y_mng.addr_ofst_raw[2] = (g_s2y_mng.ldiv_pwch[0].h->byte * g_s2y_mng.ldiv_pwch[0].v->size)
								   + (g_s2y_mng.ldiv_pwch[1].h->byte * g_s2y_mng.ldiv_pwch[1].v->size);
		if( g_p2p_calc_in[A].raw_wo_mergin ){ // overwrite if raw_wo_mergin
			g_s2y_mng.addr_ofst_raw[2] = (g_s2y_mng.ldiv_pwch[0].h->byte * g_s2y_mng.ldiv_pwch[0].v->size);
		}
		g_s2y_mng.addr_ofst_raw[3] = g_s2y_mng.addr_ofst_raw[2];
		g_s2y_mng.addr_ofst_aeawb[2] = (g_s2y_mng.pipe[0].h->aeawb_h_byte * g_s2y_mng.pipe[0].v->aeawb_blk_num)
									 + (g_s2y_mng.pipe[1].h->aeawb_h_byte * g_s2y_mng.pipe[1].v->aeawb_blk_num);
		g_s2y_mng.addr_ofst_aeawb[3] = g_s2y_mng.addr_ofst_aeawb[2];
		// end offset
		g_s2y_mng.addr_ofst_raw[4] = (g_s2y_mng.ldiv_pwch[2].h->byte * g_s2y_mng.ldiv_pwch[2].v->size)
								   + (g_s2y_mng.ldiv_pwch[3].h->byte * g_s2y_mng.ldiv_pwch[3].v->size)
								   + g_s2y_mng.addr_ofst_raw[3];
		if( g_p2p_calc_in[A].raw_wo_mergin ){ // overwrite if raw_wo_mergin
			g_s2y_mng.addr_ofst_raw[4] = (g_s2y_mng.ldiv_pwch[2].h->byte * g_s2y_mng.ldiv_pwch[2].v->size)
									   + g_s2y_mng.addr_ofst_raw[3];
		}
		g_s2y_mng.addr_ofst_aeawb[4] = (g_s2y_mng.pipe[2].h->aeawb_h_byte * g_s2y_mng.pipe[2].v->aeawb_blk_num)
									 + (g_s2y_mng.pipe[3].h->aeawb_h_byte * g_s2y_mng.pipe[3].v->aeawb_blk_num)
									 + g_s2y_mng.addr_ofst_aeawb[3];
		// sencore offset
		g_s2y_mng.h_byte_yc = g_s2y_mng.pipe[0].h->r2y_h_byte;
		g_s2y_mng.addr_ofst_y[2] = g_s2y_mng.pipe[0].h->r2y_h_byte * g_s2y_mng.pipe[0].v->r2y_y_trm_size;
		g_s2y_mng.addr_ofst_c[2] = g_s2y_mng.pipe[0].h->r2y_h_byte * g_s2y_mng.pipe[0].v->r2y_c_trm_size / div_c;
		g_s2y_mng.addr_ofst_y[3] = g_s2y_mng.addr_ofst_y[2];
		g_s2y_mng.addr_ofst_c[3] = g_s2y_mng.addr_ofst_c[2];
		// end offset
		g_s2y_mng.addr_ofst_y[4] = g_s2y_mng.addr_ofst_y[3] + ( g_s2y_mng.pipe[3].h->r2y_h_byte * g_s2y_mng.pipe[3].v->r2y_y_trm_size );
		g_s2y_mng.addr_ofst_c[4] = g_s2y_mng.addr_ofst_c[3] + ( g_s2y_mng.pipe[3].h->r2y_h_byte * g_s2y_mng.pipe[3].v->r2y_c_trm_size / div_c );
	}
	else if( g_s2y_in.sensor_num == E_SHARE_SENSOR_NUM_1 && g_s2y_in.dol_sencore_use == 0x03 && g_s2y_mng.div_num == E_SHARE_H_DIV_1 ){ // DOL div1
		// sencore offset
		g_s2y_mng.addr_ofst_raw[1] = g_s2y_mng.ldiv_pwch[0].h->byte * g_s2y_mng.ldiv_pwch[0].v->size;
		g_s2y_mng.addr_ofst_aeawb[1] = g_s2y_mng.pipe[0].h->aeawb_h_byte * g_s2y_mng.pipe[0].v->aeawb_blk_num;
		// end offset
		g_s2y_mng.addr_ofst_raw[4] = g_s2y_mng.addr_ofst_raw[1] + (g_s2y_mng.ldiv_pwch[1].h->byte * g_s2y_mng.ldiv_pwch[1].v->size);
		g_s2y_mng.addr_ofst_aeawb[4] = g_s2y_mng.addr_ofst_aeawb[1] + (g_s2y_mng.pipe[1].h->aeawb_h_byte * g_s2y_mng.pipe[1].v->aeawb_blk_num);
		
		// sencore offset
		g_s2y_mng.h_byte_yc = g_s2y_mng.pipe[0].h->r2y_h_byte;
		g_s2y_mng.addr_ofst_y[1] = g_s2y_mng.pipe[0].h->r2y_h_byte * g_s2y_mng.pipe[0].v->r2y_y_trm_size;
		g_s2y_mng.addr_ofst_c[1] = g_s2y_mng.pipe[0].h->r2y_h_byte * g_s2y_mng.pipe[0].v->r2y_c_trm_size / div_c;
		// end offset
		g_s2y_mng.addr_ofst_y[4] = g_s2y_mng.addr_ofst_y[1] + ( g_s2y_mng.pipe[1].h->r2y_h_byte * g_s2y_mng.pipe[1].v->r2y_y_trm_size );
		g_s2y_mng.addr_ofst_c[4] = g_s2y_mng.addr_ofst_c[1] + ( g_s2y_mng.pipe[1].h->r2y_h_byte * g_s2y_mng.pipe[1].v->r2y_c_trm_size / div_c );
	}
	else if( g_s2y_in.sensor_num == E_SHARE_SENSOR_NUM_1 && g_s2y_in.dualpd_sencore_use == 0x03 && g_s2y_mng.div_num == E_SHARE_H_DIV_2 ) { // Dual PD div2
		// sencore offset
		g_s2y_mng.addr_ofst_raw[2] = (g_s2y_mng.ldiv_pwch[0].h->byte * g_s2y_mng.ldiv_pwch[0].v->size)
								   + (g_s2y_mng.ldiv_pwch[1].h->byte * g_s2y_mng.ldiv_pwch[1].v->size);
		if( g_p2p_calc_in[A].raw_wo_mergin ){ // overwrite if raw_wo_mergin
			g_s2y_mng.addr_ofst_raw[2] = (g_s2y_mng.ldiv_pwch[0].h->byte * g_s2y_mng.ldiv_pwch[0].v->size);
		}
		g_s2y_mng.addr_ofst_raw[3] = g_s2y_mng.addr_ofst_raw[2];
		// end offset
		g_s2y_mng.addr_ofst_raw[4] = (g_s2y_mng.ldiv_pwch[2].h->byte * g_s2y_mng.ldiv_pwch[2].v->size)
								   + (g_s2y_mng.ldiv_pwch[3].h->byte * g_s2y_mng.ldiv_pwch[3].v->size)
								   + g_s2y_mng.addr_ofst_raw[3];
		if( g_p2p_calc_in[A].raw_wo_mergin ){ // overwrite if raw_wo_mergin
			g_s2y_mng.addr_ofst_raw[4] = (g_s2y_mng.ldiv_pwch[2].h->byte * g_s2y_mng.ldiv_pwch[2].v->size)
									   + g_s2y_mng.addr_ofst_raw[3];
		}
		g_s2y_mng.addr_ofst_aeawb[4] = (g_s2y_mng.pipe[0].h->aeawb_h_byte * g_s2y_mng.pipe[0].v->aeawb_blk_num)
									 + (g_s2y_mng.pipe[1].h->aeawb_h_byte * g_s2y_mng.pipe[1].v->aeawb_blk_num);
		// sencore offset
		g_s2y_mng.h_byte_yc = g_s2y_mng.pipe[0].h->r2y_h_byte;
		// end offset
		g_s2y_mng.addr_ofst_y[4] = g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[0].v->r2y_y_trm_size;
		g_s2y_mng.addr_ofst_c[4] = g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[0].v->r2y_c_trm_size / div_c;

		if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_OFF) {
			g_s2y_mng.addr_ofst_map[4] = g_s2y_mng.pipe[0].h->ltmmap_h_byte * g_s2y_mng.pipe[0].v->ltmmap_size;
		}
	}
	else if( g_s2y_in.sensor_num == E_SHARE_SENSOR_NUM_1 && g_s2y_in.dualpd_sencore_use == 0x03 && g_s2y_mng.div_num == E_SHARE_H_DIV_1 ){ // Dual PD div1
		// sencore offset
		g_s2y_mng.addr_ofst_raw[1] = g_s2y_mng.ldiv_pwch[0].h->byte * g_s2y_mng.ldiv_pwch[0].v->size;
		g_s2y_mng.addr_ofst_aeawb[1] = g_s2y_mng.pipe[0].h->aeawb_h_byte * g_s2y_mng.pipe[0].v->aeawb_blk_num;
		// end offset
		g_s2y_mng.addr_ofst_raw[4] = g_s2y_mng.addr_ofst_raw[1] + (g_s2y_mng.ldiv_pwch[1].h->byte * g_s2y_mng.ldiv_pwch[1].v->size);
		g_s2y_mng.addr_ofst_aeawb[4] = g_s2y_mng.addr_ofst_aeawb[1] + (g_s2y_mng.pipe[1].h->aeawb_h_byte * g_s2y_mng.pipe[1].v->aeawb_blk_num);
		
		// sencore offset
		g_s2y_mng.h_byte_yc = g_s2y_mng.pipe[0].h->r2y_h_byte;
		g_s2y_mng.addr_ofst_y[1] = g_s2y_mng.pipe[0].h->r2y_h_byte * g_s2y_mng.pipe[0].v->r2y_y_trm_size;
		g_s2y_mng.addr_ofst_c[1] = g_s2y_mng.pipe[0].h->r2y_h_byte * g_s2y_mng.pipe[0].v->r2y_c_trm_size / div_c;
		// end offset
		g_s2y_mng.addr_ofst_y[4] = g_s2y_mng.addr_ofst_y[1] + ( g_s2y_mng.pipe[1].h->r2y_h_byte * g_s2y_mng.pipe[1].v->r2y_y_trm_size );
		g_s2y_mng.addr_ofst_c[4] = g_s2y_mng.addr_ofst_c[1] + ( g_s2y_mng.pipe[1].h->r2y_h_byte * g_s2y_mng.pipe[1].v->r2y_c_trm_size / div_c );

		if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_OFF) {
			g_s2y_mng.addr_ofst_map[4] = g_s2y_mng.pipe[0].h->ltmmap_h_byte * g_s2y_mng.pipe[0].v->ltmmap_size;
		}
	}
	else if( g_s2y_in.sensor_num == E_SHARE_SENSOR_NUM_4 ){
		// sensor offset
		g_s2y_mng.addr_ofst_raw[0] = 0;
		g_s2y_mng.addr_ofst_raw[1] = g_s2y_mng.addr_ofst_raw[0] + (g_s2y_mng.ldiv_pwch[0].h->byte * g_s2y_mng.ldiv_pwch[0].v->size);
		g_s2y_mng.addr_ofst_raw[2] = g_s2y_mng.addr_ofst_raw[1] + (g_s2y_mng.ldiv_pwch[1].h->byte * g_s2y_mng.ldiv_pwch[1].v->size);
		g_s2y_mng.addr_ofst_raw[3] = g_s2y_mng.addr_ofst_raw[2] + (g_s2y_mng.ldiv_pwch[2].h->byte * g_s2y_mng.ldiv_pwch[2].v->size);
		// end offset
		g_s2y_mng.addr_ofst_raw[4] = g_s2y_mng.addr_ofst_raw[3] + (g_s2y_mng.ldiv_pwch[3].h->byte * g_s2y_mng.ldiv_pwch[3].v->size);
		
		// sensor offset
		g_s2y_mng.addr_ofst_aeawb[0] = 0;
		g_s2y_mng.addr_ofst_aeawb[1] = g_s2y_mng.addr_ofst_aeawb[0] + (g_s2y_mng.pipe[0].h->aeawb_h_byte * g_s2y_mng.pipe[0].v->aeawb_blk_num);
		g_s2y_mng.addr_ofst_aeawb[2] = g_s2y_mng.addr_ofst_aeawb[1] + (g_s2y_mng.pipe[1].h->aeawb_h_byte * g_s2y_mng.pipe[1].v->aeawb_blk_num);
		g_s2y_mng.addr_ofst_aeawb[3] = g_s2y_mng.addr_ofst_aeawb[2] + (g_s2y_mng.pipe[2].h->aeawb_h_byte * g_s2y_mng.pipe[2].v->aeawb_blk_num);
		// end offset
		g_s2y_mng.addr_ofst_aeawb[4] = g_s2y_mng.addr_ofst_aeawb[3] + (g_s2y_mng.pipe[3].h->aeawb_h_byte * g_s2y_mng.pipe[3].v->aeawb_blk_num);
		
		if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_OFF) {
			g_s2y_mng.addr_ofst_map[1] = g_s2y_mng.pipe[0].h->ltmmap_h_byte * g_s2y_mng.pipe[0].v->ltmmap_size;
			g_s2y_mng.addr_ofst_map[2] = g_s2y_mng.addr_ofst_map[1] + g_s2y_mng.pipe[1].h->ltmmap_h_byte * g_s2y_mng.pipe[1].v->ltmmap_size;
			g_s2y_mng.addr_ofst_map[3] = g_s2y_mng.addr_ofst_map[2] + g_s2y_mng.pipe[2].h->ltmmap_h_byte * g_s2y_mng.pipe[2].v->ltmmap_size;
			g_s2y_mng.addr_ofst_map[4] = g_s2y_mng.addr_ofst_map[3] + g_s2y_mng.pipe[3].h->ltmmap_h_byte * g_s2y_mng.pipe[3].v->ltmmap_size;

			g_s2y_mng.addr_ofst_maprgb[1] = g_s2y_mng.pipe[0].h->ltmrgb_h_byte * g_s2y_mng.pipe[0].v->ltmmap_size;
			g_s2y_mng.addr_ofst_maprgb[2] = g_s2y_mng.addr_ofst_maprgb[1] + g_s2y_mng.pipe[1].h->ltmrgb_h_byte * g_s2y_mng.pipe[1].v->ltmmap_size;
			g_s2y_mng.addr_ofst_maprgb[3] = g_s2y_mng.addr_ofst_maprgb[2] + g_s2y_mng.pipe[2].h->ltmrgb_h_byte * g_s2y_mng.pipe[2].v->ltmmap_size;
			g_s2y_mng.addr_ofst_maprgb[4] = g_s2y_mng.addr_ofst_maprgb[3] + g_s2y_mng.pipe[3].h->ltmrgb_h_byte * g_s2y_mng.pipe[3].v->ltmmap_size;
		}
		switch( g_s2y_in.sensor_layout ){
		case E_SHARE_SENSOR_LAYOUT_H_LINEAR:
			// sensor offset
			g_s2y_mng.h_byte_yc = g_s2y_mng.pipe[0].h->r2y_h_byte * 4;
			g_s2y_mng.addr_ofst_y[0] = g_s2y_mng.addr_ofst_c[0] = 0;
			g_s2y_mng.addr_ofst_y[1] = g_s2y_mng.addr_ofst_c[1] = g_s2y_mng.addr_ofst_y[0] + g_s2y_mng.pipe[0].h->r2y_h_byte;
			g_s2y_mng.addr_ofst_y[2] = g_s2y_mng.addr_ofst_c[2] = g_s2y_mng.addr_ofst_y[1] + g_s2y_mng.pipe[1].h->r2y_h_byte;
			g_s2y_mng.addr_ofst_y[3] = g_s2y_mng.addr_ofst_c[3] = g_s2y_mng.addr_ofst_y[2] + g_s2y_mng.pipe[2].h->r2y_h_byte;
			// end offset
			g_s2y_mng.addr_ofst_y[4] = g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[3].v->r2y_y_trm_size;
			g_s2y_mng.addr_ofst_c[4] = g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[3].v->r2y_c_trm_size / div_c;
		break;
		case E_SHARE_SENSOR_LAYOUT_V_LINEAR:
			// sensor offset
			g_s2y_mng.h_byte_yc = g_s2y_mng.pipe[0].h->r2y_h_byte;
			g_s2y_mng.addr_ofst_y[0] = 0;
			g_s2y_mng.addr_ofst_y[1] = g_s2y_mng.addr_ofst_y[0] + (g_s2y_mng.pipe[0].h->r2y_h_byte * g_s2y_mng.pipe[0].v->r2y_y_trm_size);
			g_s2y_mng.addr_ofst_y[2] = g_s2y_mng.addr_ofst_y[1] + (g_s2y_mng.pipe[1].h->r2y_h_byte * g_s2y_mng.pipe[1].v->r2y_y_trm_size);
			g_s2y_mng.addr_ofst_y[3] = g_s2y_mng.addr_ofst_y[2] + (g_s2y_mng.pipe[2].h->r2y_h_byte * g_s2y_mng.pipe[2].v->r2y_y_trm_size);
			g_s2y_mng.addr_ofst_c[0] = 0;
			g_s2y_mng.addr_ofst_c[1] = g_s2y_mng.addr_ofst_c[0] + (g_s2y_mng.pipe[0].h->r2y_h_byte * g_s2y_mng.pipe[0].v->r2y_c_trm_size / div_c);
			g_s2y_mng.addr_ofst_c[2] = g_s2y_mng.addr_ofst_c[1] + (g_s2y_mng.pipe[1].h->r2y_h_byte * g_s2y_mng.pipe[1].v->r2y_c_trm_size / div_c);
			g_s2y_mng.addr_ofst_c[3] = g_s2y_mng.addr_ofst_c[2] + (g_s2y_mng.pipe[2].h->r2y_h_byte * g_s2y_mng.pipe[2].v->r2y_c_trm_size / div_c);
			// end offset
			g_s2y_mng.addr_ofst_y[4] = g_s2y_mng.addr_ofst_y[3] + ( g_s2y_mng.pipe[3].h->r2y_h_byte * g_s2y_mng.pipe[3].v->r2y_y_trm_size );
			g_s2y_mng.addr_ofst_c[4] = g_s2y_mng.addr_ofst_c[3] + ( g_s2y_mng.pipe[3].h->r2y_h_byte * g_s2y_mng.pipe[3].v->r2y_c_trm_size / div_c );
		break;
		case E_SHARE_SENSOR_LAYOUT_H2V2:
			// sensor offset
			g_s2y_mng.h_byte_yc = g_s2y_mng.pipe[0].h->r2y_h_byte * 2;
			g_s2y_mng.addr_ofst_y[0] = 0;
			g_s2y_mng.addr_ofst_y[1] = g_s2y_mng.addr_ofst_y[0] + g_s2y_mng.pipe[0].h->r2y_h_byte;
			g_s2y_mng.addr_ofst_y[2] = g_s2y_mng.addr_ofst_y[0] + (g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[0].v->r2y_y_trm_size);
			g_s2y_mng.addr_ofst_y[3] = g_s2y_mng.addr_ofst_y[2] + g_s2y_mng.pipe[2].h->r2y_h_byte;
			g_s2y_mng.addr_ofst_c[0] = 0;
			g_s2y_mng.addr_ofst_c[1] = g_s2y_mng.addr_ofst_c[0] + g_s2y_mng.pipe[0].h->r2y_h_byte;
			g_s2y_mng.addr_ofst_c[2] = g_s2y_mng.addr_ofst_c[0] + (g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[0].v->r2y_c_trm_size / div_c);
			g_s2y_mng.addr_ofst_c[3] = g_s2y_mng.addr_ofst_c[2] + g_s2y_mng.pipe[2].h->r2y_h_byte;
			// end offset
			g_s2y_mng.addr_ofst_y[4] = g_s2y_mng.addr_ofst_y[2] + ( g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[2].v->r2y_y_trm_size );
			g_s2y_mng.addr_ofst_c[4] = g_s2y_mng.addr_ofst_c[2] + ( g_s2y_mng.h_byte_yc * g_s2y_mng.pipe[2].v->r2y_c_trm_size / div_c );
		break;
		default:
			UPRINTF_ERR(1);
		break;
		}
	}
	else{
		UPRINTF_ERR(1);
	}

	/* Calc Memory */
	g_s2y_mng.mem_bank_yc_num		= 4;
	g_s2y_mng.mem_bank_yc_ch1_num	= 4;
	if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_OFF) {
		g_s2y_mng.mem_bank_map_num		= 2;
		g_s2y_mng.mem_bank_map_rgb_num	= 2;
	}
#ifdef CO_MEMORY_1GB // can be "#if 1"
	if((g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) || (g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2)){
		g_s2y_mng.mem_bank_yc_num = 1;
	}
#endif
	if( g_s2y_mng.hscap_en ){
		if((g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) || (g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2)){
			g_s2y_mng.mem_bank_yc_num = 2;
		}else{
			g_s2y_mng.mem_bank_yc_num = D_PIPE_MEM_BANK_Y;
		}
	}
	if( g_s2y_mng.is_direct && g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_DIS){
		g_s2y_mng.mem_bank_raw_num = 0;
	}else{
		g_s2y_mng.mem_bank_raw_num = 2;
	}
	if( g_s2y_mng.mem_bank_raw_num > D_PIPE_MEM_BANK_RAW ){
		g_s2y_mng.mem_bank_raw_num = D_PIPE_MEM_BANK_RAW;
	}
	if( g_s2y_mng.mem_bank_yc_num > D_PIPE_MEM_BANK_Y ){
		g_s2y_mng.mem_bank_yc_num = D_PIPE_MEM_BANK_Y;
	}
	if( g_s2y_mng.mem_bank_map_num > D_PIPE_MEM_BANK_MAP ){
		g_s2y_mng.mem_bank_map_num = D_PIPE_MEM_BANK_MAP;
	}
	if( g_s2y_mng.mem_bank_map_rgb_num > D_PIPE_MEM_BANK_MAP_RGB ){
		g_s2y_mng.mem_bank_map_rgb_num = D_PIPE_MEM_BANK_MAP_RGB;
	}
	
	if( g_s2y_in.use_ch1 ){
		mem_addr = g_s2y_in.available_memory_addr_ch1;
	}else{
		mem_addr = g_s2y_in.available_memory_addr_ch0;
	}

	UPRINTF( "========== STM S2Y ==========\n");
	mem_size = g_s2y_mng.addr_ofst_raw[4];
	if( (g_s2y_mng.is_direct && g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SEN) ||
		(!g_s2y_mng.is_direct && g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT) ||
		(g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_4) ) {
		for( i = 0; i < g_s2y_mng.mem_bank_raw_num; i++ ) {
			g_s2y_mng.mem_addr_raw[i] = ( mem_addr + 127) & 0xFFFFFF80;
			mem_addr = g_s2y_mng.mem_addr_raw[i] + mem_size;
			UPRINTF("RAW[%d]:0x%08lx\n", i, g_s2y_mng.mem_addr_raw[i]);
		}
	}

	// for addition RAW (No matter direct/non-direct always output RAW based on image pipe point selected, only from SRO/B2B)
	if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SRO ||
		g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) {
		for( i = 0; i < g_s2y_mng.mem_bank_raw_num; i ++ ){
			g_s2y_mng.mem_addr_raw1[i] = ( mem_addr + 127) & 0xFFFFFF80;
			mem_addr = g_s2y_mng.mem_addr_raw1[i] + mem_size;
			UPRINTF("(SRO/B2B)RAW[%d]:0x%08lx\n", i, g_s2y_mng.mem_addr_raw1[i]);
		}
	}

	mem_size = g_s2y_mng.addr_ofst_y[4];
	for( i = 0; i < g_s2y_mng.mem_bank_yc_num; i ++ ){
		if( in->r2y_video_format ){
			g_s2y_mng.mem_addr_y[i] = ROUNDUP( mem_addr, D_R2Y_VIDEO_FORMAT_ADDR );
		}else{
			g_s2y_mng.mem_addr_y[i] = (mem_addr +  127) & 0xFFFFFF80;
		}
		mem_addr = g_s2y_mng.mem_addr_y[i] + mem_size;
		UPRINTF("Y[%d]:0x%08lx\n", i, g_s2y_mng.mem_addr_y[i]);
	}

	mem_size = g_s2y_mng.addr_ofst_c[4];
	for( i = 0; i < g_s2y_mng.mem_bank_yc_num; i ++ ){
		if( in->r2y_video_format ){
			g_s2y_mng.mem_addr_c[i] = ROUNDUP( mem_addr, D_R2Y_VIDEO_FORMAT_ADDR );
		}else{
			g_s2y_mng.mem_addr_c[i] = (mem_addr + 127) & 0xFFFFFF80;
		}
		mem_addr = g_s2y_mng.mem_addr_c[i] + mem_size;
		UPRINTF("C[%d]:0x%08lx\n", i, g_s2y_mng.mem_addr_c[i]);
	}

	if (r2y_ch1_enable) {
		mem_size = g_s2y_mng.addr_ofst_y_ch1[4];
		for( i = 0; i < g_s2y_mng.mem_bank_yc_ch1_num; i ++ ){
			g_s2y_mng.mem_addr_y_ch1[i] = (mem_addr +  127) & 0xFFFFFF80;
			mem_addr = g_s2y_mng.mem_addr_y_ch1[i] + mem_size;
			UPRINTF("Y_ch1[%d]:0x%08lx\n", i, g_s2y_mng.mem_addr_y_ch1[i]);
		}

		mem_size = g_s2y_mng.addr_ofst_c_ch1[4];
		for( i = 0; i < g_s2y_mng.mem_bank_yc_ch1_num; i ++ ){
			g_s2y_mng.mem_addr_c_ch1[i] = (mem_addr + 127) & 0xFFFFFF80;
			mem_addr = g_s2y_mng.mem_addr_c_ch1[i] + mem_size;
			UPRINTF("C_ch1[%d]:0x%08lx\n", i, g_s2y_mng.mem_addr_c_ch1[i]);
		}
	}

	mem_size = g_s2y_mng.addr_ofst_aeawb[4];
	for( i = 0; i < D_PIPE_MEM_BANK_AEAWB; i ++ ){
		g_s2y_mng.mem_addr_aeawb[i] = (mem_addr + 127) & 0xFFFFFF80;
		mem_addr = g_s2y_mng.mem_addr_aeawb[i] + mem_size;
		UPRINTF("AEAWB[%d]:0x%08lx\n", i, g_s2y_mng.mem_addr_aeawb[i]);
	}

	if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_OFF) {
		mem_size = g_s2y_mng.addr_ofst_map[4];
		for( i = 0; i < g_s2y_mng.mem_bank_map_num; i++ ) {
			g_s2y_mng.mem_addr_map_yp[i] = (mem_addr + 127) & 0xFFFFFF80;
			mem_addr = g_s2y_mng.mem_addr_map_yp[i] + mem_size;
			UPRINTF("map_yp[%d]:0x%08lx\n", i, g_s2y_mng.mem_addr_map_yp[i]);
		}
		for( i = 0; i < g_s2y_mng.mem_bank_map_num; i++ ) {
			g_s2y_mng.mem_addr_map_yn[i] = (mem_addr + 127) & 0xFFFFFF80;
			mem_addr = g_s2y_mng.mem_addr_map_yn[i] + mem_size;
			UPRINTF("map_yn[%d]:0x%08lx\n", i, g_s2y_mng.mem_addr_map_yn[i]);
		}

		mem_size = g_s2y_mng.addr_ofst_maprgb[4];
		for( i = 0; i < g_s2y_mng.mem_bank_map_rgb_num && mem_size; i++ ) {
			g_s2y_mng.mem_addr_map_rgb[i] = (mem_addr + 127) & 0xFFFFFF80;
			mem_addr = g_s2y_mng.mem_addr_map_rgb[i] + mem_size;
			UPRINTF("map_rgb[%d]:0x%08lx\n", i, g_s2y_mng.mem_addr_map_rgb[i]);
		}
	}

	// Memory arrange for CNR - OFL
	if (in->cap_mode) {
		mem_size = (g_s2y_mng.addr_ofst_y[4] + g_s2y_mng.addr_ofst_c[4]) / 4;
		g_s2y_mng.mem_addr_cnr_ofl_ex = (mem_addr + 127) & 0xFFFFFF80;
		mem_addr = g_s2y_mng.mem_addr_cnr_ofl_ex + mem_size;
		UPRINTF("cnr_ofl_ex:0x%08lx\n", g_s2y_mng.mem_addr_cnr_ofl_ex);
		mem_size = ROUNDUP16(g_s2y_in.one_sensor_yuv_v_size[A]) * 288;
		g_s2y_mng.mem_addr_cnr_ofl_work = (mem_addr + 127) & 0xFFFFFF80;
		mem_addr = g_s2y_mng.mem_addr_cnr_ofl_work + mem_size;
		UPRINTF("cnr_ofl_work:0x%08lx\n", g_s2y_mng.mem_addr_cnr_ofl_work);
		Ctrl_CNR_OFL_Init(g_s2y_mng.mem_addr_cnr_ofl_ex, g_s2y_mng.mem_addr_cnr_ofl_work);
	}
	
	if (in->cap_mode) {
		// if capture mode, MOVIE area skipped since MOVIE will not run
		if( (g_s2y_in.use_ch1 == FALSE) && (mem_addr >= SDRAM_ADR_CH0_END) ) {
			UPRINTF_ERR(2)
			return D_STM_NG;
		}
		if( (g_s2y_in.use_ch1 == TRUE) && (mem_addr >= SDRAM_ADR_CH1_RTOS_IMAGE_WORK + SDRAM_SIZ_CH1_RTOS_IMAGE_WORK) ) {
			UPRINTF_ERR(2)
			return D_STM_NG;
		}
	} else {
		if( (g_s2y_in.use_ch1 == FALSE) && (mem_addr >= SDRAM_ADR_CH0_RTOS_IMAGE_WORK + SDRAM_SIZ_CH0_RTOS_IMAGE_WORK) ) {
			UPRINTF_ERR(2)
			return D_STM_NG;
		}
		if( (g_s2y_in.use_ch1 == TRUE) && (mem_addr >= SDRAM_ADR_CH1_RTOS_IMAGE_WORK + SDRAM_SIZ_CH1_RTOS_IMAGE_WORK) ) {
			UPRINTF_ERR(2)
			return D_STM_NG;
		}
	}
	/* Set Output */
	if( g_s2y_in.use_ch1 ){
		out->available_memory_addr_ch0 = g_s2y_in.available_memory_addr_ch0;
		out->available_memory_addr_ch1 = mem_addr;
		sdram_ch = E_SDRAM_CH1;
	}else{
		out->available_memory_addr_ch0 = mem_addr;
		out->available_memory_addr_ch1 = g_s2y_in.available_memory_addr_ch1;
		sdram_ch = E_SDRAM_CH0;
	}
	out->pipe_use_num = (UCHAR)g_s2y_mng.pipe_use_num;
	if( g_s2y_mng.is_r2y_out_ipu ){
		if( g_s2y_mng.pipe_use_num >= 2 ){
			if (r2y_ch1_enable) {
				// R2Y ch1 goes to IPU -> SHDR if R2Y output ch0/1 both
				out->pipe0_to_ipu_h_size = g_s2y_mng.pipe[0].h->r2y_y_trm_size_ch1;
				out->pipe1_to_ipu_h_size = g_s2y_mng.pipe[1].h->r2y_y_trm_size_ch1;
			} else {
				out->pipe0_to_ipu_h_size = g_s2y_mng.pipe[0].h->r2y_y_trm_size;
				out->pipe1_to_ipu_h_size = g_s2y_mng.pipe[1].h->r2y_y_trm_size;
			}
		}else{
			if (r2y_ch1_enable)
				out->pipe0_to_ipu_h_size = g_s2y_mng.pipe[0].h->r2y_y_trm_size_ch1; // R2Y ch1 goes to IPU -> SHDR if R2Y output ch0/1 both
			else
				out->pipe0_to_ipu_h_size = g_s2y_mng.pipe[0].h->r2y_y_trm_size;
			out->pipe1_to_ipu_h_size = 0;	
		}
	}else{
		out->pipe0_to_ipu_h_size = 0;
		out->pipe1_to_ipu_h_size = 0;
	}
	out->is_direct = g_s2y_mng.is_direct;
	for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i ++ ){
		out->pipe_ofst_h_remain[i] = g_p2p_calc_out[i].pipe_ofst_h_remain;
		out->pipe_ofst_v_remain[i] = g_p2p_calc_out[i].pipe_ofst_v_remain;
	}
	// ------------------------------
	// Bandwidth
	// ------------------------------
	out->bw_yuv_ch = sdram_ch;
	for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ ){
		
		if( !g_s2y_mng.is_direct ){
			switch( i ){
			case 0:
				if( (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num == 4) ||
					 g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT) {
					out->bw_byte[ sdram_ch ][E_BW_ITEM_SEN_PWCH01] += (g_s2y_mng.ldiv_pwch[i].h->size * 2 * g_s2y_mng.ldiv_pwch[i].v->size);
					out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE0_SRO_PRCH01] += g_s2y_mng.sro_pg[i].h->size * 2 * g_s2y_mng.sro_pg[i].v->size;
				}
				if( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT ) out->bw_byte[sdram_ch][E_BW_ITEM_PIPE0_B2B_PWCH01] += (g_s2y_mng.ldiv_pwch[i].h->size * 2 * g_s2y_mng.ldiv_pwch[i].v->size);
				break;
			case 1:
				if( (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num == 4) ||
					 g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT) {
					out->bw_byte[ sdram_ch ][E_BW_ITEM_SEN_PWCH23] += (g_s2y_mng.ldiv_pwch[i].h->size * 2 * g_s2y_mng.ldiv_pwch[i].v->size);
					out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE1_SRO_PRCH01] += g_s2y_mng.sro_pg[i].h->size *2 * g_s2y_mng.sro_pg[i].v->size;
				}
				if( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT ) out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE1_B2B_PWCH01] += (g_s2y_mng.ldiv_pwch[i].h->size * 2 * g_s2y_mng.ldiv_pwch[i].v->size);
				break;
			case 2:
				if( (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num == 4) ||
					 g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT) {
					out->bw_byte[ sdram_ch ][E_BW_ITEM_SEN_PWCH45] += (g_s2y_mng.ldiv_pwch[i].h->size * 2 * g_s2y_mng.ldiv_pwch[i].v->size);
					out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE0_SRO_PRCH01] += g_s2y_mng.sro_pg[i].h->size * 2 * g_s2y_mng.sro_pg[i].v->size;
				}
				if( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT ) out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE0_B2B_PWCH01] += (g_s2y_mng.ldiv_pwch[i].h->size * 2 * g_s2y_mng.ldiv_pwch[i].v->size);
				break;
			case 3:
				if( (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num == 4) ||
					 g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT) {
					out->bw_byte[ sdram_ch ][E_BW_ITEM_SEN_PWCH67] += (g_s2y_mng.ldiv_pwch[i].h->size * 2 * g_s2y_mng.ldiv_pwch[i].v->size);
					out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE1_SRO_PRCH01] += g_s2y_mng.sro_pg[i].h->size * 2 * g_s2y_mng.sro_pg[i].v->size;
				}
				if( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT ) out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE1_B2B_PWCH01] += (g_s2y_mng.ldiv_pwch[i].h->size * 2 * g_s2y_mng.ldiv_pwch[i].v->size);
				break;
			default:
				break;
			}
		}
		if( g_s2y_in.sensor_yuv_format == E_SHARE_YUV_FORMAT_420_SEMI ){
			div_c = 2;
		}else{
			div_c = 1;
		}
		switch( i ){
		case 0:
		case 2:
			out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE0_R2Y_YYWCH0] += (g_s2y_mng.pipe[i].h->r2y_y_trm_size * g_s2y_mng.pipe[i].v->r2y_y_trm_size);
			if( in->cnr_type ){
				out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE0_CNR_WRITE]  += (g_s2y_mng.pipe[i].h->r2y_c_trm_size * g_s2y_mng.pipe[i].v->r2y_c_trm_size / div_c);
			}else{
				out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE0_R2Y_YYWCH0] += (g_s2y_mng.pipe[i].h->r2y_c_trm_size * g_s2y_mng.pipe[i].v->r2y_c_trm_size / div_c);
			}
			if (r2y_ch1_enable) {
				out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE0_R2Y_YYWCH1] += (g_s2y_mng.pipe[i].h->r2y_y_trm_size_ch1 * g_s2y_mng.pipe[i].v->r2y_y_trm_size_ch1);
				out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE0_R2Y_YYWCH1] += (g_s2y_mng.pipe[i].h->r2y_c_trm_size_ch1 * g_s2y_mng.pipe[i].v->r2y_c_trm_size_ch1 / div_c);
			}
			if( g_s2y_mng.shd_en ){
				out->bw_byte[ E_SDRAM_CH0 ][E_BW_ITEM_PIPE0_B2B_PRCH23] += (g_s2y_mng.pipe[i].h->fshd_bpr * 2 * g_s2y_mng.pipe[i].v->fshd_bpr * 2); // 2byte 2bank
			}
			out->bw_byte[ sdram_ch ][E_BW_ITEM_STAT_PWCH0] += (g_s2y_mng.pipe[i].h->aeawb_h_byte * g_s2y_mng.pipe[i].v->aeawb_blk_num);
			break;
		case 1:
		case 3:
			out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE1_R2Y_YYWCH0] += (g_s2y_mng.pipe[i].h->r2y_y_trm_size * g_s2y_mng.pipe[i].v->r2y_y_trm_size);
			if( in->cnr_type ){
				out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE1_CNR_WRITE]  += (g_s2y_mng.pipe[i].h->r2y_c_trm_size * g_s2y_mng.pipe[i].v->r2y_c_trm_size / div_c);
			}else{
				out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE1_R2Y_YYWCH0] += (g_s2y_mng.pipe[i].h->r2y_c_trm_size * g_s2y_mng.pipe[i].v->r2y_c_trm_size / div_c);
			}
			if (r2y_ch1_enable) {
				out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE1_R2Y_YYWCH1] += (g_s2y_mng.pipe[i].h->r2y_y_trm_size_ch1 * g_s2y_mng.pipe[i].v->r2y_y_trm_size_ch1);
				out->bw_byte[ sdram_ch ][E_BW_ITEM_PIPE1_R2Y_YYWCH1] += (g_s2y_mng.pipe[i].h->r2y_c_trm_size_ch1 * g_s2y_mng.pipe[i].v->r2y_c_trm_size_ch1 / div_c);
			}
			if( g_s2y_mng.shd_en ){
				out->bw_byte[ E_SDRAM_CH0 ][E_BW_ITEM_PIPE1_B2B_PRCH23] += (g_s2y_mng.pipe[i].h->fshd_bpr * 2 * g_s2y_mng.pipe[i].v->fshd_bpr * 2); // 2byte 2bank
			}
			out->bw_byte[ sdram_ch ][E_BW_ITEM_STAT_PWCH1] += (g_s2y_mng.pipe[i].h->aeawb_h_byte * g_s2y_mng.pipe[i].v->aeawb_blk_num);
			break;
		default:
			break;
		}
	}

	/* Set Address and Byte */
	for( i = 0; i < 4; i ++ ){
		g_s2y_mng.pipe[i].h_byte = g_s2y_mng.h_byte_yc;
		g_s2y_mng.pipe[i].addr_y = g_s2y_mng.mem_addr_y[ g_s2y_mng.mem_bank_y[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_y[i];
		g_s2y_mng.pipe[i].addr_c = g_s2y_mng.mem_addr_c[ g_s2y_mng.mem_bank_c[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_c[i];
		if (r2y_ch1_enable) {
			g_s2y_mng.pipe[i].h_byte_ch1 = g_s2y_mng.h_byte_yc_ch1;
			g_s2y_mng.pipe[i].addr_y_ch1 = g_s2y_mng.mem_addr_y_ch1[ g_s2y_mng.mem_bank_y_ch1[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_y_ch1[i];
			g_s2y_mng.pipe[i].addr_c_ch1 = g_s2y_mng.mem_addr_c_ch1[ g_s2y_mng.mem_bank_c_ch1[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_c_ch1[i];
		}
		g_s2y_mng.pipe[i].addr_aeawb = g_s2y_mng.mem_addr_aeawb[ g_s2y_mng.mem_bank_aeawb[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_aeawb[i];
		if( i == 0 || (i != 0 && g_s2y_mng.addr_ofst_map[i])) {
			g_s2y_mng.pipe[i].addr_map_yp_recv = g_s2y_mng.pipe[i].addr_map_yp = g_s2y_mng.mem_addr_map_yp[g_s2y_mng.mem_bank_map[E_MEM_BANK_SET]] + g_s2y_mng.addr_ofst_map[i];
			g_s2y_mng.pipe[i].addr_map_yn_recv = g_s2y_mng.pipe[i].addr_map_yn = g_s2y_mng.mem_addr_map_yn[g_s2y_mng.mem_bank_map[E_MEM_BANK_SET]] + g_s2y_mng.addr_ofst_map[i];

		}
		if( i == 0 || (i != 0 && g_s2y_mng.addr_ofst_maprgb[i])) {
			g_s2y_mng.pipe[i].addr_maprgb_recv = g_s2y_mng.pipe[i].addr_maprgb = g_s2y_mng.mem_addr_map_rgb[g_s2y_mng.mem_bank_map_rgb[E_MEM_BANK_SET]] + g_s2y_mng.addr_ofst_maprgb[i];
		}

		g_s2y_mng.ldiv_pwch[i].addr = g_s2y_mng.mem_addr_raw[ g_s2y_mng.mem_bank_raw[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_raw[i];
		g_s2y_mng.ldiv_pwch[i].addr1 = g_s2y_mng.mem_addr_raw1[ g_s2y_mng.mem_bank_raw1[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_raw[i];
		g_s2y_mng.ldiv_pwch[i].byte = g_s2y_mng.ldiv_pwch[i].h->byte;
		g_s2y_mng.sro_pg[i].addr = g_s2y_mng.ldiv_pwch[i].addr;
		g_s2y_mng.sro_pg[i].addr1 = g_s2y_mng.ldiv_pwch[i].addr1;
		g_s2y_mng.sro_pg[i].byte = g_s2y_mng.ldiv_pwch[i].byte;
	}
	
	/* AE Param */
	Get_Share_AE_Param( &g_s2y_mng.ae_param );
	g_s2y_mng.ae_param_share = g_s2y_mng.ae_param;
	COPY_PGAIN(g_s2y_mng.ae_param.isp_gain);
	
	/* AWB Param */
	Get_Share_AWB_Param( &g_s2y_mng.awb_param );
	COPY_AWB_PARAM(g_s2y_mng.awb_param);
	
	/* AF */
	if( g_s2y_in.af_en & 0x01 ){
		for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ ){
			g_s2y_mng.af_h_ing[i] = *g_s2y_mng.af[i].h;
			g_s2y_mng.af_h_rcv[i] = *g_s2y_mng.af[i].h;
		}
	}
	/* CSHD */
	if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx2_DIV2 ){
		g_s2y_mng.pipe[0].cshd_param = g_s2y_in.cshd_param[ A ];
		g_s2y_mng.pipe[1].cshd_param = g_s2y_in.cshd_param[ A ];
		g_s2y_mng.pipe[2].cshd_param = g_s2y_in.cshd_param[ B ];
		g_s2y_mng.pipe[3].cshd_param = g_s2y_in.cshd_param[ B ];
	}else{
		g_s2y_mng.pipe[0].cshd_param = g_s2y_in.cshd_param[ A ];
		g_s2y_mng.pipe[1].cshd_param = g_s2y_in.cshd_param[ B ];
		g_s2y_mng.pipe[2].cshd_param = g_s2y_in.cshd_param[ C ];
		g_s2y_mng.pipe[3].cshd_param = g_s2y_in.cshd_param[ D ];
	}
	
	if( g_s2y_mng.hscap_en ){
		HSCap_Init( g_s2y_mng.mem_bank_yc_num, g_s2y_in.jpeg_bank_num_for_hscap, g_s2y_mng.ae_param.exp.sensor[A].exp.fps, ( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT ) );
	}

	/* Reset */
	P2P_Init();
	if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_OFF ) {
		P2P_Init_LTM_DirectSW();
		P2P_PIPE_Init_LTM();
	}
	g_s2y_mng.ltm_rgb_rdy = FALSE;

	/* IQ */
	for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
		if( g_s2y_in.sensor_use & (1<<i) ){
			g_s2y_mng.iq_gain_times[i] =  g_s2y_mng.ae_param.exp.sensor[i].exp.again_times
										* g_s2y_mng.ae_param.exp.sensor[i].exp.dgain_times
										* g_s2y_mng.ae_param.isp_gain.sensor[i].b2b_end[E_PGAIN_GR]; // may not need
			if((g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) || (g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2))
				g_s2y_mng.iq_scene[i] = g_s2y_in.scene_hdr_1st_run;
			else
				g_s2y_mng.iq_scene[i] = g_s2y_in.scene_normal;
		}
	}
	IQPara_Scene_Stream_Table( g_s2y_mng.iq_scene, g_s2y_in.sensor_num );
	IQPara_UpdateGainArray( g_s2y_mng.iq_gain_times, 0.0 );
	iq_update( TRUE, g_s2y_mng.iq_stream[0], g_s2y_mng.iq_stream[1] );
	iq_dbg2_assign((-1));
	
	/* Start PWch in SRO or B2B */
	if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_B2B ) {
		if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SRO ) {
			blk_type = E_IM_PRO_BLOCK_TYPE_SRO;
			lost_line = g_s2y_in.cag_en ? PIPE_LOST_LINE_DPC_CAG : PIPE_LOST_LINE_DPC;
		} else if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_B2B ) {
			blk_type = E_IM_PRO_BLOCK_TYPE_B2B;
			if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT ||
				(g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num == 4)) {
				lost_line = g_s2y_in.cag_en ? PIPE_LOST_LINE_DPC_CAG_ELF : PIPE_LOST_LINE_DPC_ELF;
			} else {
				lost_line = 0;
			}
		}
		P2P_PWCH_Init();
		if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ){
			P2P_PWCH_Start(E_IM_PRO_UNIT_NUM_1, blk_type, &g_s2y_mng.ldiv_pwch[0], lost_line);
		} else if (	g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_2 ||
					(g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_4 && g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT)) {
			P2P_PWCH_Start(E_IM_PRO_UNIT_NUM_1, blk_type, &g_s2y_mng.ldiv_pwch[0], lost_line);
			P2P_PWCH_Start(E_IM_PRO_UNIT_NUM_2, blk_type, &g_s2y_mng.ldiv_pwch[1], lost_line);
		} else {}
	}
	
	/* Start PIPE / PWCH */
	if( g_s2y_mng.is_direct ){
		/* RAW output from SEN in direct mode */
		if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SEN && g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ){
			P2P_LDIV_PWCH_Start(&g_s2y_mng.ldiv_pwch[0], NULL, NULL, NULL);
		}else if ( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SEN && g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_2){
			P2P_LDIV_PWCH_Start(&g_s2y_mng.ldiv_pwch[0], &g_s2y_mng.ldiv_pwch[1], NULL, NULL);
		}

		if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ){
			if( g_s2y_in.af_en & 0x01 ){
				P2P_AF_Start( &g_s2y_mng.af[0], NULL );
			}
#ifdef CO_IM_TUNING_ON
			P2P_PIPE_Set_R2Y_Irq_Callback( NULL );
#else
			P2P_PIPE_Set_R2Y_Irq_Callback( r2y_irq_callback );
#endif
			P2P_PIPE_Start( TRUE, in->cnr_type, g_s2y_mng.is_r2y_out_ipu, g_s2y_mng.shd_en, g_s2y_mng.cag_en, FALSE, &g_s2y_mng.pipe[0], NULL, g_s2y_in.ltm_mode );
		}else{
			if( g_s2y_in.af_en & 0x01 ){
				P2P_AF_Start( &g_s2y_mng.af[0], &g_s2y_mng.af[1] );
			}
#ifdef CO_IM_TUNING_ON
			P2P_PIPE_Set_R2Y_Irq_Callback( NULL );
#else
			P2P_PIPE_Set_R2Y_Irq_Callback( r2y_irq_callback );
#endif
			P2P_PIPE_Start( TRUE, in->cnr_type, g_s2y_mng.is_r2y_out_ipu, g_s2y_mng.shd_en, g_s2y_mng.cag_en, FALSE, &g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1], g_s2y_in.ltm_mode );
		}
		
		low_latency_r2y_line_config();
		
	} else {
		P2P_PIPE_Set_R2Y_Irq_Callback( NULL );
		if( (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_4) ||
			(g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT) ) {
			P2P_PIPE_Set_Direct(FALSE);
			if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ){
				P2P_LDIV_PWCH_Start(&g_s2y_mng.ldiv_pwch[0], NULL, NULL, NULL);
			}else{
				P2P_LDIV_PWCH_Start(&g_s2y_mng.ldiv_pwch[0], &g_s2y_mng.ldiv_pwch[1], &g_s2y_mng.ldiv_pwch[2], &g_s2y_mng.ldiv_pwch[3]);
			}
			if (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT) {
				g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ? P2P_B2B_Ctrl(&g_s2y_mng.pipe[0], NULL) : P2P_B2B_Ctrl(&g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1]);
			}
		} else if( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT ) {
			g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ? P2P_B2B_Ctrl(&g_s2y_mng.pipe[0], NULL) : P2P_B2B_Ctrl(&g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1]);
			P2P_PIPE_Set_Direct(TRUE); // Set SRO direct mode to gen MAP
		}
	}

	if( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num < E_PIPE_USE_NUM_4) {
		if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ) {
			P2P_PIPE_SRORBKMAP_Start(FALSE, &g_s2y_mng.pipe[0], NULL);
			P2P_PIPE_SROB2B_Start( &g_s2y_mng.pipe[0], NULL, g_s2y_mng.shd_en, g_s2y_mng.cag_en );
		} else if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_2 ) {
			P2P_PIPE_SRORBKMAP_Start(FALSE, &g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1]);
			P2P_PIPE_SROB2B_Start( &g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1], g_s2y_mng.shd_en, g_s2y_mng.cag_en );
		}
	}
	/* Start SEN */
	g_s2y_mng.sen.sensor_use = g_s2y_in.sensor_use;
	g_s2y_mng.sen.dol_sencore_use = g_s2y_in.dol_sencore_use;
	g_s2y_mng.sen.dualpd_sencore_use = g_s2y_in.dualpd_sencore_use;
	g_s2y_mng.sen.sensor_order[D] = E_SHARE_SENSOR_ID_3;
	g_s2y_mng.sen.sensor_order[C] = E_SHARE_SENSOR_ID_2;
	g_s2y_mng.sen.sensor_order[B] = E_SHARE_SENSOR_ID_1;
	g_s2y_mng.sen.sensor_order[A] = E_SHARE_SENSOR_ID_0;
	g_s2y_mng.sen.div_num = g_s2y_mng.div_num;
	g_s2y_mng.sen.pipe_mode = g_s2y_mng.pipe_mode;
	g_s2y_mng.sen.gyro_cb = gyro_irq_callbak;
	for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
		if((g_s2y_in.sensor_use & (1<<i)) || (g_s2y_in.dol_sencore_use & (1<<i)) || (g_s2y_in.dualpd_sencore_use & (1<<i))){
			if((g_s2y_in.dol_sencore_use & 0x02) && ((g_s2y_in.sensor_use & 0x02) == 0) && (i != 0)) { // sensor x1 DOL (sencore x2)
				g_s2y_mng.sen.readout[i].vdo = g_p2p_calc_out[i-1].pipe_v.b2r_ofst + g_p2p_calc_out[i-1].pipe_v.b2r_size;
				g_s2y_mng.sen.readout[i].hdo = g_p2p_calc_out[i-1].ldiv[E_SHARE_H_DIV_1-1].out_h_ofst[E_SHARE_H_DIV_1-1]
												+ g_p2p_calc_out[i-1].pipe_h[E_PIPE_DIV_PART_1].b2r_ofst
												+ g_p2p_calc_out[i-1].pipe_h[E_PIPE_DIV_PART_1].b2r_size;
			} else if((g_s2y_in.dualpd_sencore_use & 0x02) && ((g_s2y_in.sensor_use & 0x02) == 0) && (i != 0)) { // sensor x1 Dual PD (sencore x2)
				g_s2y_mng.sen.readout[i].vdo = g_p2p_calc_out[i-1].pipe_v.b2r_ofst + g_p2p_calc_out[i-1].pipe_v.b2r_size;
				g_s2y_mng.sen.readout[i].hdo = g_p2p_calc_out[i-1].ldiv[E_SHARE_H_DIV_1-1].out_h_ofst[E_SHARE_H_DIV_1-1]
												+ g_p2p_calc_out[i-1].pipe_h[E_PIPE_DIV_PART_1].b2r_ofst
												+ g_p2p_calc_out[i-1].pipe_h[E_PIPE_DIV_PART_1].b2r_size;
			} else {
				g_s2y_mng.sen.readout[i].vdo = g_p2p_calc_out[i].pipe_v.b2r_ofst + g_p2p_calc_out[i].pipe_v.b2r_size;
				g_s2y_mng.sen.readout[i].hdo = g_p2p_calc_out[i].ldiv[E_SHARE_H_DIV_1-1].out_h_ofst[E_SHARE_H_DIV_1-1]
												+ g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].b2r_ofst
												+ g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].b2r_size;
			}
		}
	}
	g_gyro_current_flg = 0;
	g_gyro_compare_flg = g_s2y_in.sensor_use | (g_s2y_in.sensor_use << 16);
	g_gyro_compare_flg |= g_s2y_in.dol_sencore_use | (g_s2y_in.dol_sencore_use << 16); // DOL sencore x2
	g_gyro_compare_flg |= g_s2y_in.dualpd_sencore_use | (g_s2y_in.dualpd_sencore_use << 16); // Dual PD sencore x2
	g_s2y_mng.sen.vblank_cb = vblank_irq_callback;
	g_s2y_mng.sen.disable_sat = g_s2y_in.disable_sat;
	g_s2y_mng.sen.disable_ob = g_s2y_in.disable_ob;
	P2P_SEN_Start( &g_s2y_mng.sen );
	
	for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
		if( g_s2y_in.sensor_use & (1<<i) ){
			/* Start CPHY */
			switch(g_s2y_mng.sen.sensor_connect[i]){
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI:
				if((sen_type & 0x01) == 0x00){
					g_s2y_mng.cphy_dmipi.sensor_use = g_s2y_in.sensor_use;
					g_s2y_mng.cphy_dmipi.point_output_mode = E_IM_PRO_DMIPI_OUT_MODE_8POINT;
					P2P_CPHY_DMIPI_Start(&g_s2y_mng.cphy_dmipi);
					sen_type = sen_type | 0x01;
				}
			break;
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_SLVS:
				if((sen_type & 0x02) == 0x00){
					P2P_CPHY_SLVS_Start(&g_s2y_mng.cphy_slvs);
					sen_type = sen_type | 0x02;
				}
			break;
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_LVDS:
				if((sen_type & 0x04) == 0x00){
					P2P_CPHY_LVDS_Start(&g_s2y_mng.cphy_lvds);
					sen_type = sen_type | 0x04;
				}
			break;
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_CMIPI:
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_SLVS:
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_DMIPI_VC:
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_CMIPI_VC:
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_LVDS:
			default:
			break;
			}
		}
	}

	/* Set Share_Pipe_Yuv */
	if( g_s2y_in.r2y_video_format ){
		g_share_pipe_yuv.layout.video = E_SHARE_YUV_VIDEO_265;
	}else{
		g_share_pipe_yuv.layout.video = E_SHARE_YUV_VIDEO_NONE;
	}
	g_share_pipe_yuv.layout.format = g_s2y_in.sensor_yuv_format;
	if (r2y_ch1_enable) {
		g_share_pipe_yuv.layout.addr_y_dual_ch0 = g_s2y_mng.pipe[0].addr_y;
		g_share_pipe_yuv.layout.addr_c_dual_ch0 = g_s2y_mng.pipe[0].addr_c;
		g_share_pipe_yuv.layout.h_byte_dual_ch0 = g_s2y_mng.pipe[0].h_byte;

		g_share_pipe_yuv.layout.addr_y = g_s2y_mng.pipe[0].addr_y_ch1;
		g_share_pipe_yuv.layout.addr_c = g_s2y_mng.pipe[0].addr_c_ch1;
		g_share_pipe_yuv.layout.h_byte = g_s2y_mng.pipe[0].h_byte_ch1;
	} else {
		g_share_pipe_yuv.layout.addr_y = g_s2y_mng.pipe[0].addr_y;
		g_share_pipe_yuv.layout.addr_c = g_s2y_mng.pipe[0].addr_c;
		g_share_pipe_yuv.layout.h_byte = g_s2y_mng.pipe[0].h_byte;
	}

	if( g_s2y_in.raw_only_sen_id.bit.sen_id != 0xFF ) {
		g_s2y_in.sensor_num--;
	}
	switch( g_s2y_in.sensor_layout ) {
	case E_SHARE_SENSOR_LAYOUT_H_LINEAR:
		if (r2y_ch1_enable) {
			g_share_pipe_yuv.layout.h_size = g_s2y_in.one_sensor_yuv_h_size_r2y_ch1[A] * g_s2y_in.sensor_num;
			g_share_pipe_yuv.layout.v_size = g_s2y_in.one_sensor_yuv_v_size_r2y_ch1[A];
			g_share_pipe_yuv.layout.h_size_dual_ch0 = g_s2y_in.one_sensor_yuv_h_size[A] * g_s2y_in.sensor_num;
			g_share_pipe_yuv.layout.v_size_dual_ch0 = g_s2y_in.one_sensor_yuv_v_size[A];
		} else {
			g_share_pipe_yuv.layout.h_size = g_s2y_in.one_sensor_yuv_h_size[A] * g_s2y_in.sensor_num;
			g_share_pipe_yuv.layout.v_size = g_s2y_in.one_sensor_yuv_v_size[A];
		}
		break;
	case E_SHARE_SENSOR_LAYOUT_V_LINEAR:
		if (r2y_ch1_enable) {
			g_share_pipe_yuv.layout.h_size = g_s2y_in.one_sensor_yuv_h_size_r2y_ch1[A];
			g_share_pipe_yuv.layout.v_size = g_s2y_in.one_sensor_yuv_v_size_r2y_ch1[A] * g_s2y_in.sensor_num;
			g_share_pipe_yuv.layout.h_size_dual_ch0 = g_s2y_in.one_sensor_yuv_h_size[A];
			g_share_pipe_yuv.layout.v_size_dual_ch0 = g_s2y_in.one_sensor_yuv_v_size[A] * g_s2y_in.sensor_num;
		} else {
			g_share_pipe_yuv.layout.h_size = g_s2y_in.one_sensor_yuv_h_size[A];
			g_share_pipe_yuv.layout.v_size = g_s2y_in.one_sensor_yuv_v_size[A] * g_s2y_in.sensor_num;
		}
		break;
	case E_SHARE_SENSOR_LAYOUT_H2V2:
		g_share_pipe_yuv.layout.h_size = g_s2y_in.one_sensor_yuv_h_size[A] * 2;
		g_share_pipe_yuv.layout.v_size = g_s2y_in.one_sensor_yuv_v_size[A] * 2;
		break;
	default:
		break;
	}

	if (r2y_ch1_enable) {
		UPRINTF( "stm_s2y: r2y_out ch0 => H:%d V:%d\n", g_share_pipe_yuv.layout.h_size_dual_ch0, g_share_pipe_yuv.layout.v_size_dual_ch0 );
		UPRINTF( "stm_s2y: r2y_out ch1 => H:%d V:%d\n", g_share_pipe_yuv.layout.h_size, g_share_pipe_yuv.layout.v_size );
	} else {
		UPRINTF( "stm_s2y: r2y_out ch0 => H:%d V:%d\n", g_share_pipe_yuv.layout.h_size, g_share_pipe_yuv.layout.v_size );
	}

	if( g_s2y_in.raw_only_sen_id.bit.sen_id != 0xFF )
		g_s2y_in.sensor_num++;

	for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
		if( g_s2y_in.sensor_use & (1<<i) ){
			g_share_pipe_yuv.sensor[i].yuv.sensor_total_h_size = g_p2p_calc_in[i].sensor_h_out_total;
			g_share_pipe_yuv.sensor[i].yuv.sensor_total_v_size = g_p2p_calc_in[i].sensor_v_out_total;
			g_share_pipe_yuv.sensor[i].yuv.sensor_ref_h_ofst = g_p2p_calc_out[i].ldiv[E_SHARE_H_DIV_1-1].out_h_ofst[E_SHARE_H_DIV_1-1] + g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].b2r_ofst;
			g_share_pipe_yuv.sensor[i].yuv.sensor_ref_v_ofst = g_p2p_calc_out[i].pipe_v.b2r_ofst;
			g_share_pipe_yuv.sensor[i].yuv.sensor_ref_h_size = g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].b2r_size;
			g_share_pipe_yuv.sensor[i].yuv.sensor_ref_v_size = g_p2p_calc_out[i].pipe_v.b2r_size;
			g_share_pipe_yuv.sensor[i].yuv.format = g_s2y_in.sensor_yuv_format;
			g_share_pipe_yuv.sensor[i].yuv.h_byte = g_share_pipe_yuv.layout.h_byte;
			if (r2y_ch1_enable) {
				g_share_pipe_yuv.sensor[i].yuv.h_size = g_s2y_in.one_sensor_yuv_h_size_r2y_ch1[i];
				g_share_pipe_yuv.sensor[i].yuv.v_size = g_s2y_in.one_sensor_yuv_v_size_r2y_ch1[i];
			} else {
				g_share_pipe_yuv.sensor[i].yuv.h_size = g_s2y_in.one_sensor_yuv_h_size[i];
				g_share_pipe_yuv.sensor[i].yuv.v_size = g_s2y_in.one_sensor_yuv_v_size[i];
			}
		}
	}
	if (r2y_ch1_enable) {
		g_share_pipe_yuv.sensor[A].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y_ch1[0];
		g_share_pipe_yuv.sensor[A].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c_ch1[0];
	} else {
		g_share_pipe_yuv.sensor[A].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y[0];
		g_share_pipe_yuv.sensor[A].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c[0];
	}
	switch( g_s2y_mng.pipe_mode ){
	case E_PIPE_MODE_SENSORx2_DIRECT:
		g_share_pipe_yuv.sensor[B].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y[1];
		g_share_pipe_yuv.sensor[B].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c[1];
		break;
	case E_PIPE_MODE_SENSORx2_DIV2:
		g_share_pipe_yuv.sensor[B].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y[2];
		g_share_pipe_yuv.sensor[B].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c[2];
		break;
	case E_PIPE_MODE_SENSORx4:
		g_share_pipe_yuv.sensor[B].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y[1];
		g_share_pipe_yuv.sensor[B].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c[1];
		g_share_pipe_yuv.sensor[C].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y[2];
		g_share_pipe_yuv.sensor[C].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c[2];
		g_share_pipe_yuv.sensor[D].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_s2y_mng.addr_ofst_y[3];
		g_share_pipe_yuv.sensor[D].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_s2y_mng.addr_ofst_c[3];
		break;
	default:
		break;
	}
	
	/* Set Share_Pipe_Dol_Yuv */
	// long
	g_share_pipe_dol_yuv.long_yuv.sensor_total_h_size = g_p2p_calc_in[0].sensor_h_out_total;
	g_share_pipe_dol_yuv.long_yuv.sensor_total_v_size = g_p2p_calc_in[0].sensor_v_out_total;
	g_share_pipe_dol_yuv.long_yuv.sensor_ref_h_ofst = g_p2p_calc_out[0].ldiv[E_SHARE_H_DIV_1-1].out_h_ofst[E_SHARE_H_DIV_1-1] + g_p2p_calc_out[0].pipe_h[E_PIPE_DIV_PART_1].b2r_ofst;
	g_share_pipe_dol_yuv.long_yuv.sensor_ref_v_ofst = g_p2p_calc_out[0].pipe_v.b2r_ofst;
	g_share_pipe_dol_yuv.long_yuv.sensor_ref_h_size = g_p2p_calc_out[0].pipe_h[E_PIPE_DIV_PART_1].b2r_size;
	g_share_pipe_dol_yuv.long_yuv.sensor_ref_v_size = g_p2p_calc_out[0].pipe_v.b2r_size;
	g_share_pipe_dol_yuv.long_yuv.format = g_s2y_in.sensor_yuv_format;
	g_share_pipe_dol_yuv.long_yuv.h_byte = g_s2y_mng.pipe[0].h_byte;
	g_share_pipe_dol_yuv.long_yuv.h_size = g_s2y_in.one_sensor_yuv_h_size[0];
	g_share_pipe_dol_yuv.long_yuv.v_size = g_s2y_in.one_sensor_yuv_v_size[0];
	g_share_pipe_dol_yuv.long_yuv.addr_y = g_s2y_mng.pipe[0].addr_y;
	g_share_pipe_dol_yuv.long_yuv.addr_c = g_s2y_mng.pipe[0].addr_c;
	// short
	g_share_pipe_dol_yuv.short_yuv.sensor_total_h_size = g_p2p_calc_in[0].sensor_h_out_total;
	g_share_pipe_dol_yuv.short_yuv.sensor_total_v_size = g_p2p_calc_in[0].sensor_v_out_total;
	g_share_pipe_dol_yuv.short_yuv.sensor_ref_h_ofst = g_p2p_calc_out[0].ldiv[E_SHARE_H_DIV_1-1].out_h_ofst[E_SHARE_H_DIV_1-1] + g_p2p_calc_out[0].pipe_h[E_PIPE_DIV_PART_1].b2r_ofst;
	g_share_pipe_dol_yuv.short_yuv.sensor_ref_v_ofst = g_p2p_calc_out[0].pipe_v.b2r_ofst;
	g_share_pipe_dol_yuv.short_yuv.sensor_ref_h_size = g_p2p_calc_out[0].pipe_h[E_PIPE_DIV_PART_1].b2r_size;
	g_share_pipe_dol_yuv.short_yuv.sensor_ref_v_size = g_p2p_calc_out[0].pipe_v.b2r_size;
	g_share_pipe_dol_yuv.short_yuv.format = g_s2y_in.sensor_yuv_format;
	g_share_pipe_dol_yuv.short_yuv.h_byte = g_s2y_mng.pipe[0].h_byte;
	g_share_pipe_dol_yuv.short_yuv.h_size = g_s2y_in.one_sensor_yuv_h_size[0];
	g_share_pipe_dol_yuv.short_yuv.v_size = g_s2y_in.one_sensor_yuv_v_size[0];
	if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL ) {
		g_share_pipe_dol_yuv.short_yuv.addr_y = g_s2y_mng.pipe[1].addr_y;
		g_share_pipe_dol_yuv.short_yuv.addr_c = g_s2y_mng.pipe[1].addr_c;
	} else if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 ) {
		g_share_pipe_dol_yuv.short_yuv.addr_y = g_s2y_mng.pipe[2].addr_y;
		g_share_pipe_dol_yuv.short_yuv.addr_c = g_s2y_mng.pipe[2].addr_c;
	}
	g_yuv_cnt = 0;
	g_raw_cnt = 0;
	g_raw1_cnt = 0;
	g_gyro_cnt = 0;
	g_af_cnt = 0;
	g_cap_cnt = 0;
	g_s2y_mng.pwch_cnt = 0;
	g_s2y_mng.ltmrgb_cnt = 0;
	
	/* Set Share_R2Y_Hist */
	if( g_s2y_in.raw_only_sen_id.bit.sen_id != 0xFF) {
		g_s2y_in.sensor_num--;
	}
	g_share_pro_hist.sensor_num = g_share_r2y_hist.sensor_num = g_s2y_in.sensor_num;
	g_share_pro_hist.rect_num = g_share_r2y_hist.rect_num = g_s2y_mng.pipe_use_num;
	if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx2_DIV2) {
		g_share_pro_hist.rect[0].sensor_id = g_share_r2y_hist.rect[0].sensor_id = A;
		g_share_pro_hist.rect[1].sensor_id = g_share_r2y_hist.rect[1].sensor_id = A;
		g_share_pro_hist.rect[2].sensor_id = g_share_r2y_hist.rect[2].sensor_id = B;
		g_share_pro_hist.rect[3].sensor_id = g_share_r2y_hist.rect[3].sensor_id = B;
	} else {
		g_share_pro_hist.rect[0].sensor_id = g_share_r2y_hist.rect[0].sensor_id = A;
		g_share_pro_hist.rect[1].sensor_id = g_share_r2y_hist.rect[1].sensor_id = B;
		g_share_pro_hist.rect[2].sensor_id = g_share_r2y_hist.rect[2].sensor_id = C;
		g_share_pro_hist.rect[3].sensor_id = g_share_r2y_hist.rect[3].sensor_id = D;
	}

	/* Set Share_R2Y_DOL_Hist */
	g_share_pro_dol_hist.rect_num = g_share_r2y_dol_hist.rect_num = g_s2y_mng.pipe_use_num/2;
	
	/* Set Share_Aeawb_Stat */
	g_share_aeawb_stat.rect_num		= g_s2y_mng.pipe_use_num;
	g_share_aeawb_stat.sensor_num	= g_s2y_in.sensor_num;

	if( g_s2y_in.raw_only_sen_id.bit.sen_id != 0xFF) {
		g_s2y_in.sensor_num++;
	}

	if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL || g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 ){
		g_share_aeawb_stat.dol_rect_num = g_s2y_mng.pipe_use_num/2;
	}else{
		g_share_aeawb_stat.dol_rect_num = 0;
	}

	if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx2_DIV2) {
		g_share_aeawb_stat.rect[0].sensor = A;
		g_share_aeawb_stat.rect[1].sensor = A;
		g_share_aeawb_stat.rect[2].sensor = B;
		g_share_aeawb_stat.rect[3].sensor = B;
	} else {
		g_share_aeawb_stat.rect[0].sensor = A;
		g_share_aeawb_stat.rect[1].sensor = B;
		g_share_aeawb_stat.rect[2].sensor = C;
		g_share_aeawb_stat.rect[3].sensor = D;
	}
	for(i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
		g_share_aeawb_stat.sensor[i].h_blk_num = g_p2p_calc_in[i].aeawb_h_blk_num;
		g_share_aeawb_stat.sensor[i].v_blk_num = g_p2p_calc_in[i].aeawb_v_blk_num;
		g_share_aeawb_stat.sensor[i].aeyk[E_AEYK_R]		= g_p2p_calc_in[i].aeyk[E_AEYK_R];
		g_share_aeawb_stat.sensor[i].aeyk[E_AEYK_GR]	= g_p2p_calc_in[i].aeyk[E_AEYK_GR];
		g_share_aeawb_stat.sensor[i].aeyk[E_AEYK_GB]	= g_p2p_calc_in[i].aeyk[E_AEYK_GB];
		g_share_aeawb_stat.sensor[i].aeyk[E_AEYK_B]		= g_p2p_calc_in[i].aeyk[E_AEYK_B];
		
		g_share_aeawb_stat.sensor[i].sensor_total_h_size = g_p2p_calc_in[i].sensor_h_out_total;
		g_share_aeawb_stat.sensor[i].sensor_total_v_size = g_p2p_calc_in[i].sensor_v_out_total;
		g_share_aeawb_stat.sensor[i].sensor_ref_h_ofst = g_p2p_calc_out[i].ldiv[E_SHARE_H_DIV_1-1].out_h_ofst[E_SHARE_H_DIV_1-1] + g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_ofst;
		g_share_aeawb_stat.sensor[i].sensor_ref_v_ofst = g_p2p_calc_out[i].pipe_v.aeawb_blk_ofst;
		if( g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_num > 2 ){
			g_share_aeawb_stat.sensor[i].sensor_ref_h_size = ( g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_num - 2) * g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size
															+ g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size_L + g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size_R;
		}else{
			g_share_aeawb_stat.sensor[i].sensor_ref_h_size = 0;
		}
		g_share_aeawb_stat.sensor[i].sensor_ref_v_size = g_p2p_calc_out[i].pipe_v.aeawb_blk_num * g_p2p_calc_out[i].pipe_v.aeawb_blk_size;
	}
	for( i = 0; i < g_s2y_mng.pipe_use_num; i++ ) {
		g_share_aeawb_stat.rect[i].addr_ae = g_s2y_mng.pipe[i].addr_aeawb + g_s2y_mng.pipe[i].h->aeawb_addr_ae;
		g_share_aeawb_stat.rect[i].addr_awb = g_s2y_mng.pipe[i].addr_aeawb + g_s2y_mng.pipe[i].h->aeawb_addr_awb;
		g_share_aeawb_stat.rect[i].addr_level_awb = g_s2y_mng.pipe[i].addr_aeawb + g_s2y_mng.pipe[i].h->aeawb_addr_level_awb;
		g_share_aeawb_stat.rect[i].is_level_awb = g_s2y_mng.pipe[i].v->aeawb_level_awb;
		
		g_share_aeawb_stat.rect[i].h_byte = g_s2y_mng.pipe[i].h->aeawb_h_byte;
		g_share_aeawb_stat.rect[i].blk_ofst_L_H_R = g_s2y_mng.pipe[i].h->aeawb_blk_ofst;
		g_share_aeawb_stat.rect[i].blk_num_L_H_R = g_s2y_mng.pipe[i].h->aeawb_blk_num;
		g_share_aeawb_stat.rect[i].blk_size_L = g_s2y_mng.pipe[i].h->aeawb_blk_size_L;
		g_share_aeawb_stat.rect[i].blk_size_H = g_s2y_mng.pipe[i].h->aeawb_blk_size;
		g_share_aeawb_stat.rect[i].blk_size_R = g_s2y_mng.pipe[i].h->aeawb_blk_size_R;
		
		g_share_aeawb_stat.rect[i].blk_ofst_V = g_s2y_mng.pipe[i].v->aeawb_blk_ofst;
		g_share_aeawb_stat.rect[i].blk_num_V = g_s2y_mng.pipe[i].v->aeawb_blk_num;
		g_share_aeawb_stat.rect[i].blk_size_V = g_s2y_mng.pipe[i].v->aeawb_blk_size;
	}
	g_aeawb_cnt = 0;
	g_prohist_cnt = 0;
	
	g_share_af_data.af_en = g_s2y_in.af_en;
	/* Set Share_Sensor_Exp */
	for( i = 0; i < D_SENSOR_REFLECT_FRAME; i ++ ){
		Set_Frame_Share_Sensor_Exp(&g_s2y_mng.ae_param.exp, i );	// set to future
		Set_Frame_Share_APEX(&g_s2y_mng.ae_param.apex, i );	// set to future
		Set_Frame_Share_ISP_Gain(&g_s2y_mng.ae_param.isp_gain, i );	// set to future, to update isp_gain at same timing as sensor
	}
	
	/* Set Share AWB Param */
	for( i = 0; i < D_ISP_REFLECT_FRAME; i ++ ){
		Set_Frame_Share_AWB_Param( &g_s2y_mng.awb_param, i );	// set to future
	}

	/* Set RAW info */
	if( g_s2y_in.raw_only_sen_id.bit.sen_id != 0xFF ) {
		g_s2y_mng.pipe_use_num = 4;
	}
	for( i = 0; i < g_s2y_mng.pipe_use_num; i++ ) {
		g_share_sen_raw.rect[i].h_byte = g_s2y_mng.sro_pg[i].byte;
		g_share_sen_raw.rect[i].h_size = g_s2y_mng.sro_pg[i].h->size;
	}
	if( g_s2y_in.raw_only_sen_id.bit.sen_id != 0xFF ) {
		g_s2y_mng.pipe_use_num = 2;
	}
	switch (g_s2y_mng.pipe_mode) {
		case E_PIPE_MODE_SENSORx1_DIRECT:
		case E_PIPE_MODE_SENSORx1:
			g_share_sen_raw.rect[0].h_ofs_frame = g_s2y_mng.sen.ldiv[A]->out_h_ofst[0];
			g_share_sen_raw.sensor[0].h_size	= g_share_sen_raw.rect[0].h_size;
			g_share_sen_raw.sensor[0].sensor_id = A;
			g_share_sen_raw.sensor[0].first_pixel = g_s2y_mng.sen.first_pixel[A];
			g_share_sen_raw.sensor[0].bit_depth = 16;
			g_share_sen_raw.sensor_num	= 1;
			g_share_sen_raw.rect_num	= 1;
			g_share_sen_raw.rect[0].h_ofs = g_p2p_calc_in[A].sensor_h_effective_ofst;
			g_share_sen_raw.rect[0].v_ofs = g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.sensor[0].raw_wo_mergin = g_p2p_calc_in[A].raw_wo_mergin;
			
			g_share_sen_raw.rect[0].v_size = g_s2y_mng.ldiv_pwch[0].v->size - g_p2p_calc_in[A].sensor_v_effective_ofst;
			if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) {
				g_share_sen_raw.rect[0].v_size -= lost_line;
			}
			break;
		case E_PIPE_MODE_SENSORx2_DIRECT:
		case E_PIPE_MODE_SENSORx1_DOL:
			g_share_sen_raw.rect[0].h_ofs_frame = g_s2y_mng.sen.ldiv[A]->out_h_ofst[0];
			g_share_sen_raw.rect[1].h_ofs_frame = g_s2y_mng.sen.ldiv[B]->out_h_ofst[0];
			g_share_sen_raw.sensor[0].h_size = g_share_sen_raw.rect[0].h_size;
			g_share_sen_raw.sensor[1].h_size = g_share_sen_raw.rect[1].h_size;
			g_share_sen_raw.sensor[0].sensor_id = A;
			g_share_sen_raw.sensor[1].sensor_id = B;
			g_share_sen_raw.sensor[0].first_pixel = g_s2y_mng.sen.first_pixel[A];
			g_share_sen_raw.sensor[1].first_pixel = g_s2y_mng.sen.first_pixel[B];
			g_share_sen_raw.sensor[0].bit_depth = 16;
			g_share_sen_raw.sensor[1].bit_depth = 16;
			g_share_sen_raw.rect[0].h_ofs = g_p2p_calc_in[A].sensor_h_effective_ofst;
			g_share_sen_raw.rect[1].h_ofs = g_p2p_calc_in[B].sensor_h_effective_ofst;
			g_share_sen_raw.rect[0].v_ofs = g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[1].v_ofs = g_p2p_calc_in[B].sensor_v_effective_ofst;
			g_share_sen_raw.sensor_num	= 2;
			g_share_sen_raw.rect_num	= 2;
			g_share_sen_raw.sensor[0].raw_wo_mergin = g_p2p_calc_in[A].raw_wo_mergin; 
			g_share_sen_raw.sensor[1].raw_wo_mergin = g_p2p_calc_in[B].raw_wo_mergin;

			g_share_sen_raw.rect[0].v_size = g_s2y_mng.ldiv_pwch[0].v->size - g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[1].v_size = g_s2y_mng.ldiv_pwch[1].v->size - g_p2p_calc_in[B].sensor_v_effective_ofst;
			if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) {
				g_share_sen_raw.rect[0].v_size -= lost_line;
				g_share_sen_raw.rect[1].v_size -= lost_line;
			}
			break;
		case E_PIPE_MODE_SENSORx4:
			g_share_sen_raw.rect[0].h_ofs_frame = g_s2y_mng.sen.ldiv[A]->out_h_ofst[0];
			g_share_sen_raw.rect[1].h_ofs_frame = g_s2y_mng.sen.ldiv[B]->out_h_ofst[0];
			g_share_sen_raw.rect[2].h_ofs_frame = g_s2y_mng.sen.ldiv[C]->out_h_ofst[0];
			g_share_sen_raw.rect[3].h_ofs_frame = g_s2y_mng.sen.ldiv[D]->out_h_ofst[0];
			g_share_sen_raw.sensor[0].h_size = g_share_sen_raw.rect[0].h_size;
			g_share_sen_raw.sensor[1].h_size = g_share_sen_raw.rect[1].h_size;
			g_share_sen_raw.sensor[2].h_size = g_share_sen_raw.rect[2].h_size;
			g_share_sen_raw.sensor[3].h_size = g_share_sen_raw.rect[3].h_size;
			g_share_sen_raw.sensor[0].sensor_id = A;
			g_share_sen_raw.sensor[1].sensor_id = B;
			g_share_sen_raw.sensor[2].sensor_id = C;
			g_share_sen_raw.sensor[3].sensor_id = D;
			g_share_sen_raw.sensor[0].first_pixel = g_s2y_mng.sen.first_pixel[A];
			g_share_sen_raw.sensor[1].first_pixel = g_s2y_mng.sen.first_pixel[B];
			g_share_sen_raw.sensor[2].first_pixel = g_s2y_mng.sen.first_pixel[C];
			g_share_sen_raw.sensor[3].first_pixel = g_s2y_mng.sen.first_pixel[D];
			g_share_sen_raw.sensor[0].bit_depth = 16;
			g_share_sen_raw.sensor[1].bit_depth = 16;
			g_share_sen_raw.sensor[2].bit_depth = 16;
			g_share_sen_raw.sensor[3].bit_depth = 16;
			g_share_sen_raw.rect[0].h_ofs = g_p2p_calc_in[A].sensor_h_effective_ofst;
			g_share_sen_raw.rect[1].h_ofs = g_p2p_calc_in[B].sensor_h_effective_ofst;
			g_share_sen_raw.rect[2].h_ofs = g_p2p_calc_in[C].sensor_h_effective_ofst;
			g_share_sen_raw.rect[3].h_ofs = g_p2p_calc_in[D].sensor_h_effective_ofst;
			g_share_sen_raw.rect[0].v_ofs = g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[1].v_ofs = g_p2p_calc_in[B].sensor_v_effective_ofst;
			g_share_sen_raw.rect[2].v_ofs = g_p2p_calc_in[C].sensor_v_effective_ofst;
			g_share_sen_raw.rect[3].v_ofs = g_p2p_calc_in[D].sensor_v_effective_ofst;
			g_share_sen_raw.sensor_num	= 4;
			g_share_sen_raw.rect_num	= 4;
			g_share_sen_raw.sensor[0].raw_wo_mergin = g_p2p_calc_in[A].raw_wo_mergin; 
			g_share_sen_raw.sensor[1].raw_wo_mergin = g_p2p_calc_in[B].raw_wo_mergin; 
			g_share_sen_raw.sensor[2].raw_wo_mergin = g_p2p_calc_in[C].raw_wo_mergin; 
			g_share_sen_raw.sensor[3].raw_wo_mergin = g_p2p_calc_in[D].raw_wo_mergin;

			g_share_sen_raw.rect[0].v_size = g_s2y_mng.ldiv_pwch[0].v->size - g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[1].v_size = g_s2y_mng.ldiv_pwch[1].v->size - g_p2p_calc_in[B].sensor_v_effective_ofst;
			g_share_sen_raw.rect[2].v_size = g_s2y_mng.ldiv_pwch[2].v->size - g_p2p_calc_in[C].sensor_v_effective_ofst;
			g_share_sen_raw.rect[3].v_size = g_s2y_mng.ldiv_pwch[3].v->size - g_p2p_calc_in[D].sensor_v_effective_ofst;
			if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) {
				g_share_sen_raw.rect[0].v_size -= lost_line;
				g_share_sen_raw.rect[1].v_size -= lost_line;
				g_share_sen_raw.rect[2].v_size -= lost_line;
				g_share_sen_raw.rect[3].v_size -= lost_line;
			}
			break;
		case E_PIPE_MODE_SENSORx1_DIRECT_DIV2:
			g_share_sen_raw.rect[0].h_ofs_frame = g_s2y_mng.sen.ldiv[A]->out_h_ofst[0];
			g_share_sen_raw.rect[1].h_ofs_frame = g_s2y_mng.sen.ldiv[A]->out_h_ofst[1];
			g_share_sen_raw.sensor[0].h_size = 	g_share_sen_raw.rect[1].h_ofs_frame -
												g_share_sen_raw.rect[0].h_ofs_frame +
												g_share_sen_raw.rect[1].h_size;
			g_share_sen_raw.sensor[0].sensor_id = A;
			g_share_sen_raw.sensor[0].first_pixel = g_s2y_mng.sen.first_pixel[A];
			g_share_sen_raw.sensor[0].bit_depth = 16;
			g_share_sen_raw.rect[0].h_ofs = g_p2p_calc_in[A].sensor_h_effective_ofst;
			g_share_sen_raw.rect[1].h_ofs = g_p2p_calc_in[A].sensor_h_effective_ofst;
			g_share_sen_raw.rect[0].v_ofs = g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[1].v_ofs = g_p2p_calc_in[A].sensor_v_effective_ofst;

			g_share_sen_raw.rect[0].v_size = g_s2y_mng.ldiv_pwch[0].v->size - g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[1].v_size = g_s2y_mng.ldiv_pwch[1].v->size - g_p2p_calc_in[A].sensor_v_effective_ofst;
			if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) {
				g_share_sen_raw.rect[0].v_size -= lost_line;
				g_share_sen_raw.rect[1].v_size -= lost_line;
			}

			g_share_sen_raw.sensor_num	= 1;
			g_share_sen_raw.rect_num	= 2;

			if(g_s2y_in.af_en & 0x02){
				g_share_af_data.pdaf.IMAGE_DATA.g_width = g_s2y_mng.ldiv_pwch[0].byte;
				g_share_af_data.pdaf.IMAGE_DATA.width = g_s2y_mng.sen.ldiv[A]->out_h_size[1] + g_s2y_mng.sen.ldiv[A]->out_h_ofst[1] - g_s2y_mng.sen.ldiv[A]->out_h_ofst[0];
				g_share_af_data.pdaf.IMAGE_DATA.height = g_s2y_mng.ldiv_pwch[0].v->size;
				
				g_share_af_data.pdaf.DUAL_PD_DATA.g_width = g_s2y_mng.ldiv_pwch[2].byte;
				g_share_af_data.pdaf.DUAL_PD_DATA.width = g_s2y_mng.sen.ldiv[B]->out_h_size[1] + g_s2y_mng.sen.ldiv[B]->out_h_ofst[1] - g_s2y_mng.sen.ldiv[B]->out_h_ofst[0];
				g_share_af_data.pdaf.DUAL_PD_DATA.height = g_s2y_mng.ldiv_pwch[2].v->size;
				g_share_af_data.pdaf.DUAL_PD_DATA.addr = g_s2y_mng.ldiv_pwch[2].addr + g_s2y_mng.ldiv_pwch[2].h->addr;
			}else if(g_s2y_in.af_en & 0x04){
				g_share_af_data.pdaf.IMAGE_DATA.g_width = g_share_sen_raw.rect[0].h_byte / 2;
				g_share_af_data.pdaf.IMAGE_DATA.width = g_share_sen_raw.sensor[0].h_size;
				g_share_af_data.pdaf.IMAGE_DATA.height = g_p2p_calc_in[0].sensor_v_effective_size;
				
				g_share_af_data.pdaf.PD_DATA.sensor_mode = g_s2y_in.sensor_mode[0];
				g_share_af_data.pdaf.PD_DATA.blk_g_cnt = CAMERA.sensor[0].mode[ g_s2y_in.sensor_mode[0] ].pd_g_cnt;
				g_share_af_data.pdaf.PD_DATA.blk_h_cnt = CAMERA.sensor[0].mode[ g_s2y_in.sensor_mode[0] ].pd_win_h_cnt;
				g_share_af_data.pdaf.PD_DATA.blk_v_cnt = CAMERA.sensor[0].mode[ g_s2y_in.sensor_mode[0] ].pd_win_v_cnt;
			}
			g_share_sen_raw.sensor[0].raw_wo_mergin = g_p2p_calc_in[A].raw_wo_mergin; 
			break;
		case E_PIPE_MODE_SENSORx1_DIV4:
			g_share_sen_raw.rect[0].h_ofs_frame = g_s2y_mng.sen.ldiv[A]->out_h_ofst[0];
			g_share_sen_raw.rect[1].h_ofs_frame = g_s2y_mng.sen.ldiv[A]->out_h_ofst[1];
			g_share_sen_raw.rect[2].h_ofs_frame = g_s2y_mng.sen.ldiv[A]->out_h_ofst[2];
			g_share_sen_raw.rect[3].h_ofs_frame = g_s2y_mng.sen.ldiv[A]->out_h_ofst[3];
			g_share_sen_raw.sensor[0].h_size = 	(g_share_sen_raw.rect[1].h_ofs_frame -
												 g_share_sen_raw.rect[0].h_ofs_frame) +
												(g_share_sen_raw.rect[2].h_ofs_frame -
												 g_share_sen_raw.rect[1].h_ofs_frame) +
												(g_share_sen_raw.rect[3].h_ofs_frame -
												 g_share_sen_raw.rect[2].h_ofs_frame) +
												g_share_sen_raw.rect[3].h_size;
			g_share_sen_raw.sensor[0].sensor_id = A;
			g_share_sen_raw.sensor[0].first_pixel = g_s2y_mng.sen.first_pixel[A];
			g_share_sen_raw.sensor[0].bit_depth = 16;
			g_share_sen_raw.rect[0].h_ofs = g_p2p_calc_in[A].sensor_h_effective_ofst;
			g_share_sen_raw.rect[1].h_ofs = g_p2p_calc_in[A].sensor_h_effective_ofst;
			g_share_sen_raw.rect[2].h_ofs = g_p2p_calc_in[A].sensor_h_effective_ofst;
			g_share_sen_raw.rect[3].h_ofs = g_p2p_calc_in[A].sensor_h_effective_ofst;
			g_share_sen_raw.rect[0].v_ofs = g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[1].v_ofs = g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[2].v_ofs = g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[3].v_ofs = g_p2p_calc_in[A].sensor_v_effective_ofst;

			g_share_sen_raw.rect[0].v_size = g_s2y_mng.ldiv_pwch[0].v->size - g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[1].v_size = g_s2y_mng.ldiv_pwch[1].v->size - g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[2].v_size = g_s2y_mng.ldiv_pwch[2].v->size - g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[3].v_size = g_s2y_mng.ldiv_pwch[3].v->size - g_p2p_calc_in[A].sensor_v_effective_ofst;
			if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) {
				g_share_sen_raw.rect[0].v_size -= lost_line;
				g_share_sen_raw.rect[1].v_size -= lost_line;
				g_share_sen_raw.rect[2].v_size -= lost_line;
				g_share_sen_raw.rect[3].v_size -= lost_line;
			}
			g_share_sen_raw.sensor_num	= 1;
			g_share_sen_raw.rect_num	= 4;
			g_share_sen_raw.sensor[0].raw_wo_mergin = g_p2p_calc_in[A].raw_wo_mergin; 
			break;
		case E_PIPE_MODE_SENSORx2_DIV2:
		case E_PIPE_MODE_SENSORx1_DOL_DIV2:
			g_share_sen_raw.rect[0].h_ofs_frame = g_s2y_mng.sen.ldiv[A]->out_h_ofst[0];
			g_share_sen_raw.rect[1].h_ofs_frame = g_s2y_mng.sen.ldiv[A]->out_h_ofst[1];
			g_share_sen_raw.rect[2].h_ofs_frame = g_s2y_mng.sen.ldiv[B]->out_h_ofst[0];
			g_share_sen_raw.rect[3].h_ofs_frame = g_s2y_mng.sen.ldiv[B]->out_h_ofst[1];
			g_share_sen_raw.sensor[0].h_size = 	g_share_sen_raw.rect[1].h_ofs_frame -
												g_share_sen_raw.rect[0].h_ofs_frame +
												g_share_sen_raw.rect[1].h_size;
			g_share_sen_raw.sensor[1].h_size = 	g_share_sen_raw.rect[3].h_ofs_frame -
												g_share_sen_raw.rect[2].h_ofs_frame +
												g_share_sen_raw.rect[3].h_size;
			g_share_sen_raw.sensor[0].sensor_id = A;
			g_share_sen_raw.sensor[1].sensor_id = B;
			g_share_sen_raw.sensor[0].first_pixel = g_s2y_mng.sen.first_pixel[A];
			g_share_sen_raw.sensor[1].first_pixel = g_s2y_mng.sen.first_pixel[B];
			g_share_sen_raw.sensor[0].bit_depth = 16;
			g_share_sen_raw.sensor[1].bit_depth = 16;
			g_share_sen_raw.rect[0].h_ofs = g_p2p_calc_in[A].sensor_h_effective_ofst;
			g_share_sen_raw.rect[1].h_ofs = g_p2p_calc_in[A].sensor_h_effective_ofst;
			g_share_sen_raw.rect[2].h_ofs = g_p2p_calc_in[B].sensor_h_effective_ofst;
			g_share_sen_raw.rect[3].h_ofs = g_p2p_calc_in[B].sensor_h_effective_ofst;
			g_share_sen_raw.rect[0].v_ofs = g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[1].v_ofs = g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[2].v_ofs = g_p2p_calc_in[B].sensor_v_effective_ofst;
			g_share_sen_raw.rect[3].v_ofs = g_p2p_calc_in[B].sensor_v_effective_ofst;

			g_share_sen_raw.rect[0].v_size = g_s2y_mng.ldiv_pwch[0].v->size - g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[1].v_size = g_s2y_mng.ldiv_pwch[1].v->size - g_p2p_calc_in[A].sensor_v_effective_ofst;
			g_share_sen_raw.rect[2].v_size = g_s2y_mng.ldiv_pwch[2].v->size - g_p2p_calc_in[B].sensor_v_effective_ofst;
			g_share_sen_raw.rect[3].v_size = g_s2y_mng.ldiv_pwch[3].v->size - g_p2p_calc_in[B].sensor_v_effective_ofst;
			if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) {
				g_share_sen_raw.rect[0].v_size -= lost_line;
				g_share_sen_raw.rect[1].v_size -= lost_line;
				g_share_sen_raw.rect[2].v_size -= lost_line;
				g_share_sen_raw.rect[3].v_size -= lost_line;
			}

			g_share_sen_raw.sensor_num	= 2;
			g_share_sen_raw.rect_num	= 4;
			g_share_sen_raw.sensor[0].raw_wo_mergin = g_p2p_calc_in[A].raw_wo_mergin; 
			g_share_sen_raw.sensor[1].raw_wo_mergin = g_p2p_calc_in[B].raw_wo_mergin; 
			break;
		default:
			break;
	}
	/* Set Share Circle */
	Set_Share_AEAWB_Blk_Valid_For_Circle( A, (UCHAR*)g_p2p_calc_out[A].aeawb_blk_valid_for_circle );
	if( g_s2y_in.sensor_num >= E_SHARE_SENSOR_NUM_2 ){
		Set_Share_AEAWB_Blk_Valid_For_Circle( B, (UCHAR*)g_p2p_calc_out[B].aeawb_blk_valid_for_circle );
	}
	if( g_s2y_in.sensor_num >= E_SHARE_SENSOR_NUM_4 ){
		Set_Share_AEAWB_Blk_Valid_For_Circle( C, (UCHAR*)g_p2p_calc_out[C].aeawb_blk_valid_for_circle );
		Set_Share_AEAWB_Blk_Valid_For_Circle( D, (UCHAR*)g_p2p_calc_out[D].aeawb_blk_valid_for_circle );
	}

	return D_STM_OK;
}


INT32 STM_Sensor2YUV_Start( void)
{
	INT32 i;

	// --- REMOVE_WDR BEGIN ---
	if(!g_s2y_mng.is_direct){
		WDR_Realtime_Wdr_LPF_Init(ON);
	}
	// --- REMOVE_WDR END ---
	
	/* Start Task */
	OS_User_Clr_Flg( FID_SENSOR_ACCESS, 0 );
	OS_User_Sta_Tsk( TID_SENSOR_ACCESS, 0 );

	OS_User_Clr_Flg( FID_IQ_ACCESS, 0 );
	OS_User_Sta_Tsk( TID_IQ_ACCESS, 0 );
	
	for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
		if( g_s2y_in.sensor_use & (1<<i) ){
			if( CAMERA.sensor[ i ].wait_stable_ms > 0 ){
				P2P_SEN_Stop();
				break;
			}
		}
	}

	/* Set Sensor Mode */
	for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
		if( g_s2y_in.sensor_use & (1<<i) ){
			
			if( CAMERA.sensor[ i ].Func_CancelStandby ){
				CAMERA.sensor[ i ].Func_CancelStandby( i );
			}

			if(g_s2y_in.af_en & 0x06){
				if( CAMERA.sensor[ i ].Func_Pdaf_Mode ){
					CAMERA.sensor[ i ].Func_Pdaf_Mode( i, g_s2y_in.sensor_mode[i]);
				}
			} else {
				if( CAMERA.sensor[ i ].Func_Mode ){
					CAMERA.sensor[ i ].Func_Mode( i, g_s2y_in.sensor_mode[i], g_s2y_in.dol_sencore_use ? TRUE : FALSE);
				}
			}

			if(g_s2y_in.global_shut_en) {
				if (g_s2y_in.cap_mode) {
					if( CAMERA.sensor[ i ].Func_GlobalShutter ){
						CAMERA.sensor[ i ].Func_GlobalShutter( i, E_CAMERA_SENSOR_SHUTTER_G_ON_M_OPEN );
					}
				}
				else {
					if( CAMERA.sensor[ i ].Func_GlobalShutter ){
						CAMERA.sensor[ i ].Func_GlobalShutter( i, E_CAMERA_SENSOR_SHUTTER_G_OFF_M_OPEN );
					}
				}
			}

			if( CAMERA.sensor[ i ].Func_Multi ){
				if( g_s2y_in.sensor_num == E_SHARE_SENSOR_NUM_1 ){
					CAMERA.sensor[ i ].Func_Multi( i, FALSE, FALSE);
				}else{
					if( i == E_SHARE_SENSOR_ID_0 ){
						CAMERA.sensor[ i ].Func_Multi( i, TRUE, TRUE ); // master
					}else{
						CAMERA.sensor[ i ].Func_Multi( i, TRUE, FALSE ); // slave
					}
				}
			}

			if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 || g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) {
				if( CAMERA.sensor[i].Func_DOL_Set_Exp && g_s2y_in.sensor_exp_NO_update == FALSE) {
					CAMERA.sensor[ i ].Func_DOL_Set_Exp( i, g_s2y_in.sensor_mode[i], &g_s2y_mng.ae_param.reg.sensor[i].reg );
				}
			}else{
				if( CAMERA.sensor[i].Func_Set_Exp && g_s2y_in.sensor_exp_NO_update == FALSE) {
					CAMERA.sensor[ i ].Func_Set_Exp( i, g_s2y_in.sensor_mode[i], &g_s2y_mng.ae_param.reg.sensor[i].reg );
				}
			}
		}
	}

	/* Start Sensor Stream */
	for( i = E_SHARE_SENSOR_NUM_MAX-1; i >= 0; i-- ){
		if( g_s2y_in.sensor_use & (1<<i) ){
			if( CAMERA.sensor[ i ].Func_Stream ){
				CAMERA.sensor[ i ].Func_Stream( i );
			}
			
			if( CAMERA.sensor[ i ].Func_SgVHD ){
				if( g_s2y_in.sensor_exp_NO_update == FALSE ) {
					CAMERA.sensor[i].Func_SgVHD(i, g_s2y_mng.ae_param.reg.sensor[i].reg.reg[E_CAMERA_SENSOR_REG_VDOCYC], g_s2y_mng.ae_param.reg.sensor[i].reg.reg[E_CAMERA_SENSOR_REG_HDOCYC]);
				} else {
					if( g_s2y_in.dol_sencore_use == 0x03 ) {
						CAMERA.sensor[i].Func_SgVHD(i, CAMERA.sensor[i].mode[g_s2y_in.sensor_mode[i]].dol_vdocyc, CAMERA.sensor[i].mode[g_s2y_in.sensor_mode[i]].dol_hdocyc);
					} else {
						CAMERA.sensor[i].Func_SgVHD(i, CAMERA.sensor[i].mode[g_s2y_in.sensor_mode[i]].vdocyc, CAMERA.sensor[i].mode[g_s2y_in.sensor_mode[i]].hdocyc);
					}
				}
			}

			if( CAMERA.sensor[ i ].wait_stable_ms > 0 ){
				OS_User_Dly_Tsk(CAMERA.sensor[ i ].wait_stable_ms);
			}
		}
	}

	for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
		if( g_s2y_in.sensor_use & (1<<i) ){
			if( CAMERA.sensor[ i ].wait_stable_ms > 0 ){
				P2P_SEN_Start( &g_s2y_mng.sen );
				break;
			}
		}
	}

	for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
		if( g_s2y_in.sensor_use & (1<<i) ){
			if( g_s2y_mng.sen.sensor_connect[ i ] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_SLVS ){
				P2P_CPHY_SLVS_Start_After(&g_s2y_mng.cphy_slvs);
			} else if( g_s2y_mng.sen.sensor_connect[ i ] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_LVDS ){
				P2P_CPHY_LVDS_Start_After(&g_s2y_mng.cphy_lvds);
			}
		}
	}
	g_sensor2yuv_state = D_STM_BUSY;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SENSOR, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "Sensor Start"));
	
	return D_STM_OK;
}


INT32 STM_Sensor2YUV_Event( E_SYSTEM_EVENT event, ULLONG frame_no )
{
	OS_USER_FLGPTN	tsk_sen_flg = 0;
	T_SHARE_SYSTEM_CONFIG config;
	T_SHARE_SYSTEM_PARAM param;
	T_SHARE_RGB_RATIO rgb;
	INT32   i;
	E_IM_PRO_BLOCK_TYPE blk_type = E_IM_PRO_BLOCK_TYPE_B2B;
	UCHAR lost_line = 0;
	T_IM_LTM_INTERNAL_PARAM internal_param = {0};
	T_IQ_PARAM_CNR_CTRL ctrl_0, ctrl_1;
	E_CNR_TYPE			cnr_type;
	T_IQ_PARAM iq_param;

	// check state
	if( g_sensor2yuv_state == D_STM_IDLE )
		return g_sensor2yuv_state;
	
	Get_Share_System_Param(&param);

	switch( event )
	{
	case E_SYSTEM_EVENT_VD:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_VD, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "VD %d", (UINT32)frame_no));
		// Reconfig IQ scene number here since system config is done before VIEW
		Cate_Get_IQ_Param(&iq_param);
		g_s2y_in.scene_normal = iq_param.scene_normal;
		g_s2y_in.scene_hdr_1st_run = iq_param.scene_hdr_1st_run;

		Get_Share_System_Config( &config );
		g_s2y_mng.shd_en = config.shd_en;
		g_s2y_mng.cag_en = config.cag_en;
		g_s2y_in.frame_num = config.frame_num; // For burst capture, to stop pipe
		// update ae_param
		g_s2y_mng.ae_param = g_s2y_mng.ae_param_share;
		
		if((g_s2y_in.cap_mode == TRUE) && (g_skip_frame_num == 0))
			tsk_sen_flg = tsk_sen_flg | FLG_SENSOR_SHUTTER_CONTROL;
		
		// the sensor update timing follows the setting of sensor A.
		if( CAMERA.sensor[ A ].basic->update_timing ==  E_CAMERA_SENSOR_UPDATE_TIMING_VD)
			tsk_sen_flg = tsk_sen_flg | FLG_SENSOR_ACCESS_UPDATE;

		if(tsk_sen_flg != 0x00000000)
			OS_User_Set_Flg( FID_SENSOR_ACCESS, tsk_sen_flg );
		
		Set_Frame_Share_Sensor_Exp(&g_s2y_mng.ae_param.exp, frame_no + D_SENSOR_REFLECT_FRAME);		// set to future
		Set_Frame_Share_APEX(&g_s2y_mng.ae_param.apex, frame_no + D_SENSOR_REFLECT_FRAME);		// set to future
		Set_Frame_Share_ISP_Gain(&g_s2y_mng.ae_param.isp_gain, frame_no + D_SENSOR_REFLECT_FRAME );	// set to future, to update isp_gain at same timing as sensor
		
		Set_Frame_Share_AWB_Param( &g_s2y_mng.awb_param, frame_no + D_ISP_REFLECT_FRAME );			// set to future
		
		if( g_s2y_in.af_en & 0x01 ){
			g_s2y_mng.af_param = g_s2y_mng.af_param_share;
			calc_af(frame_no, &g_s2y_mng.af_param);
			for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ ){
				if( g_s2y_mng.is_direct ){
					g_s2y_mng.af_h_rcv[i] = g_s2y_mng.af_h_ing[i];
					g_s2y_mng.af_h_ing[i] = *g_s2y_mng.af[i].h;
				}else{
					g_s2y_mng.af_h_rcv[i] = *g_s2y_mng.af[i].h;
				}
			}
		}
		
		/* Update Address */
		BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_raw, g_s2y_mng.mem_bank_raw_num, g_s2y_mng.mem_addr_raw, param.lock_address_raw);
		if( g_s2y_mng.pipe_use_num < E_PIPE_USE_NUM_4 ){ BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_raw1, g_s2y_mng.mem_bank_raw_num, g_s2y_mng.mem_addr_raw1, param.lock_address_raw1);}
		for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ ) {
			g_s2y_mng.ldiv_pwch[i].addr = g_s2y_mng.mem_addr_raw[ g_s2y_mng.mem_bank_raw[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_raw[i];
			if( g_s2y_mng.pipe_use_num < E_PIPE_USE_NUM_4 ) g_s2y_mng.ldiv_pwch[i].addr1 = g_s2y_mng.mem_addr_raw1[ g_s2y_mng.mem_bank_raw1[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_raw[i];
		}

		if(g_s2y_in.dualpd_sencore_use == 0x03){
			switch (g_s2y_mng.pipe_mode) {
				case E_PIPE_MODE_SENSORx1_DIRECT:
					g_s2y_mng.ldiv_pwch[1].addr = g_s2y_mng.mem_addr_raw[ g_s2y_mng.mem_bank_raw[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_raw[1];
				break;
				case E_PIPE_MODE_SENSORx1_DIRECT_DIV2:
					g_s2y_mng.ldiv_pwch[2].addr = g_s2y_mng.mem_addr_raw[ g_s2y_mng.mem_bank_raw[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_raw[2];
					g_s2y_mng.ldiv_pwch[3].addr = g_s2y_mng.mem_addr_raw[ g_s2y_mng.mem_bank_raw[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_raw[3];
				break;
				default:
				break;
			}
		}
		/* Update PWch address in SRO or B2B for pipe num <= 2. if pipe num = 4, stop PWch(not forcedly) */
		if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SRO || g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_B2B) {
			if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SRO )
				blk_type = E_IM_PRO_BLOCK_TYPE_SRO;
			else if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_B2B )
				blk_type = E_IM_PRO_BLOCK_TYPE_B2B;

			if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ){
				P2P_PWCH_Address(E_IM_PRO_UNIT_NUM_1, blk_type, &g_s2y_mng.ldiv_pwch[0]);
			} else if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_2 ){
				P2P_PWCH_Address(E_IM_PRO_UNIT_NUM_1, blk_type, &g_s2y_mng.ldiv_pwch[0]);
				P2P_PWCH_Address(E_IM_PRO_UNIT_NUM_2, blk_type, &g_s2y_mng.ldiv_pwch[1]);
			} else {
				//P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_1, blk_type, 0);
				//P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_2, blk_type, 0);
			}
		}
		/* Update SRO/B2B Register when SRO B2B are direct and R2Y is not direct */
		if (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num < E_PIPE_USE_NUM_4) {
			// AEAWB
			BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_aeawb, D_PIPE_MEM_BANK_AEAWB, g_s2y_mng.mem_addr_aeawb, 0);
			for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ )
				g_s2y_mng.pipe[i].addr_aeawb = g_s2y_mng.mem_addr_aeawb[ g_s2y_mng.mem_bank_aeawb[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_aeawb[i];
			// PGAIN
			Get_Frame_Share_ISP_Gain( &g_s2y_mng.isp_gain_reg, frame_no + D_ISP_REFLECT_FRAME ); // get from future, to update isp_gain at same timing as sensor
			COPY_PGAIN(g_s2y_mng.isp_gain_reg);
			// AWB(SHD)
			Get_Frame_Share_AWB_Param( &g_s2y_mng.awb_param_reg, frame_no + D_ISP_REFLECT_FRAME ); // get from future
			COPY_AWB_PARAM(g_s2y_mng.awb_param_reg);
			// IQ
			Get_Frame_Share_Sensor_Exp( &g_s2y_mng.exp, frame_no + D_ISP_REFLECT_FRAME ); 
			for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
				if( g_s2y_in.sensor_use & (1<<i) ){
					g_s2y_mng.iq_gain_times[i] =  g_s2y_mng.exp.sensor[i].exp.again_times
												* g_s2y_mng.exp.sensor[i].exp.dgain_times
												* g_s2y_mng.isp_gain_reg.sensor[i].b2b_end[E_PGAIN_GR]; // may not need
					if((g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) || (g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2))
						g_s2y_mng.iq_scene[i] = g_s2y_in.scene_hdr_1st_run;
					else
						g_s2y_mng.iq_scene[i] = g_s2y_in.scene_normal;
				}
			}
			IQPara_Scene_Stream_Table( g_s2y_mng.iq_scene, g_s2y_in.sensor_num );
			IQPara_UpdateGainArray( g_s2y_mng.iq_gain_times, 0.0 );
			iq_update_sen_to_b2b( g_s2y_mng.iq_stream[0], g_s2y_mng.iq_stream[1] ); // better move task
			// Update
			if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 )
				P2P_PIPE_SROB2B_Update( &g_s2y_mng.pipe[0], NULL, g_s2y_mng.shd_en, g_s2y_mng.cag_en );
			else if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_2 )
				P2P_PIPE_SROB2B_Update( &g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1], g_s2y_mng.shd_en, g_s2y_mng.cag_en );
			else {}
		}

		if( (!g_s2y_mng.is_direct && g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT) ||
			(g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_4)){
			if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 )
				P2P_LDIV_PWCH_Address(&g_s2y_mng.ldiv_pwch[0], NULL, NULL, NULL);
			else
				P2P_LDIV_PWCH_Address(&g_s2y_mng.ldiv_pwch[0], &g_s2y_mng.ldiv_pwch[1], &g_s2y_mng.ldiv_pwch[2], &g_s2y_mng.ldiv_pwch[3]);
		} else if (g_s2y_mng.is_direct && g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SEN) {
			if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 )
				P2P_LDIV_PWCH_Address(&g_s2y_mng.ldiv_pwch[0], NULL, NULL, NULL);
			else if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_2 )
				P2P_LDIV_PWCH_Address(&g_s2y_mng.ldiv_pwch[0], &g_s2y_mng.ldiv_pwch[1], NULL, NULL);
		}
	break;
	case E_SYSTEM_EVENT_R2YLINE:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SRO_R2Y, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "R2YCNR LINE %d", (INT32)g_yuv_cnt));

		if( g_s2y_mng.is_direct ){			
			if (g_s2y_in.one_sensor_yuv_h_size_r2y_ch1[0] && g_s2y_in.one_sensor_yuv_v_size_r2y_ch1[0]) {
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_y, g_s2y_mng.mem_bank_yc_num, g_s2y_mng.mem_addr_y, param.lock_address_y_dual_ch0);
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_c, g_s2y_mng.mem_bank_yc_num, g_s2y_mng.mem_addr_c, param.lock_address_c_dual_ch0);
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_y_ch1, g_s2y_mng.mem_bank_yc_ch1_num, g_s2y_mng.mem_addr_y_ch1, param.lock_address_y);
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_c_ch1, g_s2y_mng.mem_bank_yc_ch1_num, g_s2y_mng.mem_addr_c_ch1, param.lock_address_c);
			} else {
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_y, g_s2y_mng.mem_bank_yc_num, g_s2y_mng.mem_addr_y, param.lock_address_y);
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_c, g_s2y_mng.mem_bank_yc_num, g_s2y_mng.mem_addr_c, param.lock_address_c);
			}
			BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_aeawb, D_PIPE_MEM_BANK_AEAWB, g_s2y_mng.mem_addr_aeawb, 0);

			Get_Frame_Share_ISP_Gain( &g_s2y_mng.isp_gain_reg, frame_no + D_ISP_REFLECT_FRAME ); // get from future, to update isp_gain at same timing as sensor
			COPY_PGAIN(g_s2y_mng.isp_gain_reg);
			
			Get_Frame_Share_AWB_Param( &g_s2y_mng.awb_param_reg, frame_no + D_ISP_REFLECT_FRAME ); // get from future
			COPY_AWB_PARAM(g_s2y_mng.awb_param_reg);
			
			Get_Frame_Share_Sensor_Exp( &g_s2y_mng.exp, frame_no + D_ISP_REFLECT_FRAME ); 
			for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
				if( g_s2y_in.sensor_use & (1<<i) ){
					g_s2y_mng.iq_gain_times[i] =  g_s2y_mng.exp.sensor[i].exp.again_times
												* g_s2y_mng.exp.sensor[i].exp.dgain_times
												* g_s2y_mng.isp_gain_reg.sensor[i].b2b_end[E_PGAIN_GR]; // may not need
					if((g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) || (g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2))
						g_s2y_mng.iq_scene[i] = g_s2y_in.scene_hdr_1st_run;
					else
						g_s2y_mng.iq_scene[i] = g_s2y_in.scene_normal;
				}
			}
			IQPara_Scene_Stream_Table( g_s2y_mng.iq_scene, g_s2y_in.sensor_num );
			IQPara_UpdateGainArray( g_s2y_mng.iq_gain_times, 0.0 );
			//iq_update( FALSE, g_s2y_mng.iq_stream[0], g_s2y_mng.iq_stream[1] );
			OS_User_Set_Flg( FID_IQ_ACCESS, FLG_IQ_ACCESS_UPDATE_PIPE_WITHOUT_TABLE );
			
			for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ ) {
				if (g_s2y_in.one_sensor_yuv_h_size_r2y_ch1[0] && g_s2y_in.one_sensor_yuv_v_size_r2y_ch1[0]) {
					g_s2y_mng.pipe[i].addr_y_ch1 = g_s2y_mng.mem_addr_y_ch1[ g_s2y_mng.mem_bank_y_ch1[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_y_ch1[i];
					g_s2y_mng.pipe[i].addr_c_ch1 = g_s2y_mng.mem_addr_c_ch1[ g_s2y_mng.mem_bank_c_ch1[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_c_ch1[i];
				}
				g_s2y_mng.pipe[i].addr_y = g_s2y_mng.mem_addr_y[ g_s2y_mng.mem_bank_y[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_y[i];
				g_s2y_mng.pipe[i].addr_c = g_s2y_mng.mem_addr_c[ g_s2y_mng.mem_bank_c[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_c[i];
				g_s2y_mng.pipe[i].addr_aeawb = g_s2y_mng.mem_addr_aeawb[ g_s2y_mng.mem_bank_aeawb[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_aeawb[i];
			}
			
			low_latency_r2y_line_start( frame_no+D_ISP_REFLECT_FRAME );
			
			if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ){
				if( g_s2y_in.af_en & 0x01 )
					P2P_AF_Update( &g_s2y_mng.af[0], NULL );

				P2P_PIPE_Addr(g_s2y_mng.shd_en, g_s2y_mng.cag_en, &g_s2y_mng.pipe[0], NULL);
				P2P_PIPE_R2Y_HIST_Enable( TRUE, &g_s2y_mng.pipe[0], NULL);
			}else{
				if( g_s2y_in.af_en & 0x01 )
					P2P_AF_Update( &g_s2y_mng.af[0], &g_s2y_mng.af[1] );

				P2P_PIPE_Addr(g_s2y_mng.shd_en, g_s2y_mng.cag_en, &g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1]);
				P2P_PIPE_R2Y_HIST_Enable( TRUE, &g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1]);
			}
		} else if ((g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num < E_PIPE_USE_NUM_4) ||
				   (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_4 && g_s2y_mng.pipes_2nd_time == 1)) {
			// PIPE_NUM < 4 case, we still need to increase bank since LTM non-direct mode, always pipe non-direct mode
			if (g_s2y_in.one_sensor_yuv_h_size_r2y_ch1[0] && g_s2y_in.one_sensor_yuv_v_size_r2y_ch1[0]) {
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_y, g_s2y_mng.mem_bank_yc_num, g_s2y_mng.mem_addr_y, param.lock_address_y_dual_ch0);
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_c, g_s2y_mng.mem_bank_yc_num, g_s2y_mng.mem_addr_c, param.lock_address_c_dual_ch0);
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_y_ch1, g_s2y_mng.mem_bank_yc_ch1_num, g_s2y_mng.mem_addr_y_ch1, param.lock_address_y);
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_c_ch1, g_s2y_mng.mem_bank_yc_ch1_num, g_s2y_mng.mem_addr_c_ch1, param.lock_address_c);
			} else {
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_y, g_s2y_mng.mem_bank_yc_num, g_s2y_mng.mem_addr_y, param.lock_address_y);
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_c, g_s2y_mng.mem_bank_yc_num, g_s2y_mng.mem_addr_c, param.lock_address_c);
			}
		} else if (!g_s2y_mng.is_direct && g_s2y_mng.pipes_2nd_time == 1) {
			if (g_s2y_in.one_sensor_yuv_h_size_r2y_ch1[0] && g_s2y_in.one_sensor_yuv_v_size_r2y_ch1[0]) {
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_y, g_s2y_mng.mem_bank_yc_num, g_s2y_mng.mem_addr_y, param.lock_address_y_dual_ch0);
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_c, g_s2y_mng.mem_bank_yc_num, g_s2y_mng.mem_addr_c, param.lock_address_c_dual_ch0);
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_y_ch1, g_s2y_mng.mem_bank_yc_ch1_num, g_s2y_mng.mem_addr_y_ch1, param.lock_address_y);
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_c_ch1, g_s2y_mng.mem_bank_yc_ch1_num, g_s2y_mng.mem_addr_c_ch1, param.lock_address_c);
			} else {
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_y, g_s2y_mng.mem_bank_yc_num, g_s2y_mng.mem_addr_y, param.lock_address_y);
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_c, g_s2y_mng.mem_bank_yc_num, g_s2y_mng.mem_addr_c, param.lock_address_c);
			}
			BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_aeawb, D_PIPE_MEM_BANK_AEAWB, g_s2y_mng.mem_addr_aeawb, 0);
		} 
	break;
		
	case E_SYSTEM_EVENT_R2YLINE1:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_INT, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "R2YCNR LINE1 %d", (INT32)frame_no));
		low_latency_r2y_line_end( frame_no );
	break;
		
	case E_SYSTEM_EVENT_R2YCNR:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SRO_R2Y, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "R2YCNR LINE%d", (UINT32)frame_no));
		
#ifdef CO_IM_TUNING_ON // call at here instead of irq
		r2y_irq_callback( NULL, 0, 0 );
#endif
		/* YUV is End */
		if( g_s2y_mng.is_direct ) {
			if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL )
				P2P_PIPE_R2Y_HIST_Get( &g_share_r2y_dol_hist.long_hist[0], &g_share_r2y_dol_hist.short_hist[0] );
			else
				P2P_PIPE_R2Y_HIST_Get( &g_share_r2y_hist.rect[0].hist, &g_share_r2y_hist.rect[1].hist );

			UPDATE_YUV_FRAME_AND_SEND_TO_SYSTEM;		// update YUV one frame info and send to system
			if( g_s2y_in.frame_num != D_FRAME_NUM_INFINITY && (UINT32)g_cap_cnt >= g_s2y_in.frame_num )
				STM_Sensor2YUV_Stop(TRUE);
		} else {
			if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT) {
				g_s2y_mng.r2y_frame_no = frame_no;
				if( g_s2y_mng.r2y_frame_no == g_s2y_mng.prch_frame_no ){
					// prch is waiting for r2y end
					Snd_Event_to_System( E_SYSTEM_EVENT_SRO_PRCH, frame_no );
				}
			} else { // LTM_NON_DIRECT_MODE process same PRch 
				MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_B2R, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "B2RR2Y"));
				if( g_s2y_mng.pipe_use_num < E_PIPE_USE_NUM_4 ){
					P2P_PIPE_R2Y_HIST_Get( &g_share_r2y_hist.rect[0].hist,  &g_share_r2y_hist.rect[1].hist );
					if( g_s2y_in.af_en & 0x01 )
						P2P_AF_Stop( TRUE, TRUE );

					P2P_PIPE_Patial_Stop( TRUE, TRUE, FALSE );
					UPDATE_YUV_FRAME_AND_SEND_TO_SYSTEM;		// update YUV one frame info and send to system
					if( g_s2y_in.frame_num != D_FRAME_NUM_INFINITY && (UINT32)g_cap_cnt >= g_s2y_in.frame_num )
						STM_Sensor2YUV_Stop(TRUE);
				}else{
					if( g_s2y_in.af_en & 0x01 )
						P2P_AF_Stop( TRUE, TRUE );

					P2P_PIPE_Patial_Stop( TRUE, TRUE, FALSE );

					if( g_s2y_mng.pipes_2nd_time == 0 && g_s2y_mng.pipe_use_num > E_PIPE_USE_NUM_2){
						P2P_PIPE_R2Y_HIST_Get( &g_share_r2y_hist.rect[0].hist, &g_share_r2y_hist.rect[1].hist );
						P2P_PIPE_B2RR2Y_Start(g_s2y_mng.cnr_type,
									  g_s2y_mng.is_r2y_out_ipu,
									  g_s2y_mng.shd_en,
									  g_s2y_mng.cag_en,
									  &g_s2y_mng.pipe[2],
									  &g_s2y_mng.pipe[3],
									  &g_s2y_mng.sro_pg[2],
									  &g_s2y_mng.sro_pg[3]);
						g_s2y_mng.pipes_2nd_time = 1;
						MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_B2R, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "B2RR2Y"));
					}else{
						P2P_PIPE_R2Y_HIST_Get( &g_share_r2y_hist.rect[2].hist, &g_share_r2y_hist.rect[3].hist );
						UPDATE_YUV_FRAME_AND_SEND_TO_SYSTEM;	// update YUV one frame info and send to system
						if( g_s2y_in.frame_num != D_FRAME_NUM_INFINITY && (UINT32)g_cap_cnt >= g_s2y_in.frame_num )
							STM_Sensor2YUV_Stop(TRUE);
					}
				}
			}
		}
	break;
	case E_SYSTEM_EVENT_PWCH:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_LDIV_PWCH, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "PWCH(SRO_B2B)%d", (UINT32)frame_no));

		if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_SRO ) {
			blk_type = E_IM_PRO_BLOCK_TYPE_SRO;
			lost_line = g_s2y_in.cag_en ? PIPE_LOST_LINE_DPC_CAG : PIPE_LOST_LINE_DPC;
		} else if( g_s2y_in.raw_out_point_forced == RAW_EXTRACT_POINT_B2B ) {
			blk_type = E_IM_PRO_BLOCK_TYPE_B2B;
			if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT )
				lost_line = g_s2y_in.cag_en ? PIPE_LOST_LINE_DPC_CAG_ELF : PIPE_LOST_LINE_DPC_ELF;
			else
				lost_line = 0;
		}

		g_s2y_mng.pwch_cnt++;
		if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_4 ){
			if( g_s2y_mng.pwch_cnt & 0x1 ) {
				if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT ) {
					P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_1, blk_type, 1);
					P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_2, blk_type, 1);
					P2P_PWCH_Start(E_IM_PRO_UNIT_NUM_1, blk_type, &g_s2y_mng.ldiv_pwch[2], lost_line);
					P2P_PWCH_Start(E_IM_PRO_UNIT_NUM_2, blk_type, &g_s2y_mng.ldiv_pwch[3], lost_line);
				}
			} else {
				if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT ) {
					P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_1, blk_type, 1);
					P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_2, blk_type, 1);
					BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_raw1, g_s2y_mng.mem_bank_raw_num, g_s2y_mng.mem_addr_raw1, param.lock_address_raw1);
					for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ )
						g_s2y_mng.ldiv_pwch[i].addr1 = g_s2y_mng.mem_addr_raw1[ g_s2y_mng.mem_bank_raw1[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_raw[i];

					P2P_PWCH_Start(E_IM_PRO_UNIT_NUM_1, blk_type, &g_s2y_mng.ldiv_pwch[0], lost_line);
					P2P_PWCH_Start(E_IM_PRO_UNIT_NUM_2, blk_type, &g_s2y_mng.ldiv_pwch[1], lost_line);
					BANK_ING_TO_RCV(g_s2y_mng.mem_bank_raw1);
					/* No matter direct/non-direct we need to update sro_pg addr here since direct mode we can ouput RAW as well */
					for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ )
						g_s2y_mng.sro_pg[i].addr1 = g_s2y_mng.mem_addr_raw1[ g_s2y_mng.mem_bank_raw1[E_MEM_BANK_RCV] ] + g_s2y_mng.addr_ofst_raw[i];

					/* Set RAW info */
					for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ )
						g_share_sen_raw.rect[i].addr1 = g_s2y_mng.sro_pg[i].addr1 + g_s2y_mng.sro_pg[i].h->addr;

					if(g_skip_frame_num == 0) {
						Set_Frame_Share_Sensor_Raw(&g_share_sen_raw, g_raw1_cnt);
						g_raw1_cnt++;
					}
				}
			}
		} else {
			BANK_ING_TO_RCV(g_s2y_mng.mem_bank_raw1);
			/* No matter direct/non-direct we need to update sro_pg addr here since direct mode we can ouput RAW as well */
			for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ )
				g_s2y_mng.sro_pg[i].addr1 = g_s2y_mng.mem_addr_raw1[ g_s2y_mng.mem_bank_raw1[E_MEM_BANK_RCV] ] + g_s2y_mng.addr_ofst_raw[i];

			/* Set RAW info */
			for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ )
				g_share_sen_raw.rect[i].addr1 = g_s2y_mng.sro_pg[i].addr1 + g_s2y_mng.sro_pg[i].h->addr;

			if(g_skip_frame_num == 0) {
				Set_Frame_Share_Sensor_Raw(&g_share_sen_raw, g_raw1_cnt);
				g_raw1_cnt++;
			}
		}
		
		break;
	case E_SYSTEM_EVENT_LTMCNR:
		if( g_dol_save_raw_frame != -1 ){
			Snd_Event_to_System( E_SYSTEM_EVENT_LDIV_PWCH_DUMMY, g_dol_save_raw_frame );
			g_dol_save_raw_frame = -1;
		}
		break;
	case E_SYSTEM_EVENT_LDIV_PWCH_DUMMY:
	case E_SYSTEM_EVENT_LDIV_PWCH:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_LDIV_PWCH, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "PWCH %d", (INT32)frame_no));
		
		if( event == E_SYSTEM_EVENT_LDIV_PWCH ){
			if( (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_4) ){
			}else{
				g_s2y_mng.pipes_2nd_time = 0;
			}
			g_s2y_mng.sro_2nd_time_s	= 0;
			/* Update Address */
			BANK_ING_TO_RCV(g_s2y_mng.mem_bank_raw);
		}
		
		// DOL Skip
		if((g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) || (g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2)){
			if( P2P_PIPE_Is_Run() ){
				if (g_s2y_mng.hscap_en && g_skip_frame_num == 0) {
					g_dol_save_raw_frame = frame_no - CAMERA.start_frame_num; // retry by re-sending EVENT_LDIV_PWCH after PIPE is realesed by stm_ltm2cnr.c
				}else{
					g_yuv_cnt ++;
				}
				UPRINTF( "Skip SROR2Y\n" );
				break;
			}
		}
		
		/* No matter direct/non-direct we need to update sro_pg address here since direct mode we can output RAW as well */
		for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ )
			g_s2y_mng.sro_pg[i].addr = g_s2y_mng.mem_addr_raw[ g_s2y_mng.mem_bank_raw[E_MEM_BANK_RCV] ] + g_s2y_mng.addr_ofst_raw[i];

		if( g_s2y_in.frame_num != D_FRAME_NUM_INFINITY && (UINT32)g_cap_cnt >= g_s2y_in.frame_num ) {
			STM_Sensor2YUV_Stop(TRUE);
			break;
		}
		
		// High Speed Capture
		if (g_s2y_mng.hscap_en && g_skip_frame_num == 0) {
			BOOL start_pipe = FALSE;
			
			start_pipe = !HSCap_Is_Close_MShutter( frame_no - CAMERA.start_frame_num);
			if( start_pipe ){
				if( HSCap_Push_PipeQ_MediaQ( frame_no - CAMERA.start_frame_num) )
					start_pipe = FALSE;
			}
			HSCap_Judge_MShutter( frame_no - CAMERA.start_frame_num + 1 ); // +1 is for next frame

			if(!HSCap_Is_Close_MShutter( frame_no - CAMERA.start_frame_num + 1 )){
				if(g_s2y_in.global_shut_en == TRUE) {
					if( CAMERA.sensor[ A ].Func_GlobalShutter ){
						CAMERA.sensor[ A ].Func_GlobalShutter( A, E_CAMERA_SENSOR_SHUTTER_G_ON_M_OPEN );
						g_cap_shutter_release = TRUE;
					}
				}
			}
			
			if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT ){
				if( start_pipe ){
					HSCap_Measure_Pipe_Start( frame_no - CAMERA.start_frame_num );
					// not break
				}else{
					HSCap_Start_Pipe_Dummy();
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SRO_R2Y, FJ_MBALOG_FUNC_ID_COMMON, FJ_MBALOG_TYPE_START, "Dummy %d", (INT32)g_yuv_cnt));
					break;
				}
			}
		}

		if (g_s2y_in.cap_mode == TRUE && g_skip_frame_num == 0) {
			for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ )
				g_share_sen_raw.rect[i].addr = g_s2y_mng.sro_pg[i].addr + g_s2y_mng.sro_pg[i].h->addr;

			Callback_Share_SensorRaw(&g_share_sen_raw, g_raw_cnt);
		}

		/* Start PIPE */
		if( !g_s2y_mng.is_direct && g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT){

			for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ ) {
				if (g_s2y_in.one_sensor_yuv_h_size_r2y_ch1[0] && g_s2y_in.one_sensor_yuv_v_size_r2y_ch1[0]) {
					g_s2y_mng.pipe[i].addr_y_ch1 = g_s2y_mng.mem_addr_y_ch1[ g_s2y_mng.mem_bank_y_ch1[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_y_ch1[i];
					g_s2y_mng.pipe[i].addr_c_ch1 = g_s2y_mng.mem_addr_c_ch1[ g_s2y_mng.mem_bank_c_ch1[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_c_ch1[i];
				}
				g_s2y_mng.pipe[i].addr_y = g_s2y_mng.mem_addr_y[ g_s2y_mng.mem_bank_y[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_y[i];
				g_s2y_mng.pipe[i].addr_c = g_s2y_mng.mem_addr_c[ g_s2y_mng.mem_bank_c[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_c[i];
				g_s2y_mng.pipe[i].addr_aeawb = g_s2y_mng.mem_addr_aeawb[ g_s2y_mng.mem_bank_aeawb[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_aeawb[i];
			}

			Get_Frame_Share_ISP_Gain( &g_s2y_mng.isp_gain_reg, frame_no ); // get from current, to update isp_gain at same timing as sensor
			COPY_PGAIN(g_s2y_mng.isp_gain_reg);

			Get_Frame_Share_AWB_Param( &g_s2y_mng.awb_param_reg, frame_no ); // get from current
			COPY_AWB_PARAM(g_s2y_mng.awb_param_reg);

			Get_Frame_Share_Sensor_Exp( &g_s2y_mng.exp, frame_no ); 
			for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
				if( g_s2y_in.sensor_use & (1<<i) ){
					g_s2y_mng.iq_gain_times[i] =  g_s2y_mng.exp.sensor[i].exp.again_times
												* g_s2y_mng.exp.sensor[i].exp.dgain_times
												* g_s2y_mng.isp_gain_reg.sensor[i].b2b_end[E_PGAIN_GR]; // may not need
					if((g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) || (g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2))
						g_s2y_mng.iq_scene[i] = g_s2y_in.scene_hdr_1st_run;
					else
						g_s2y_mng.iq_scene[i] = g_s2y_in.scene_normal;
				}
			}
			IQPara_Scene_Stream_Table( g_s2y_mng.iq_scene, g_s2y_in.sensor_num );
			IQPara_UpdateGainArray( g_s2y_mng.iq_gain_times, 0.0 );
			iq_update( TRUE, g_s2y_mng.iq_stream[0], g_s2y_mng.iq_stream[1] );
			iq_dbg2_set( -1 );

			cnr_type = g_s2y_mng.cnr_type;
			if (g_s2y_in.cap_mode) {
				IQPara_Get_IQ_CNR_Ctrl(&ctrl_0, &ctrl_1);
				if(ctrl_0.CNRMODE == 0)
					cnr_type = E_CNR_OFF;
			}

			if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ){
				if( g_s2y_in.af_en & 0x01 )
					P2P_AF_Start( &g_s2y_mng.af[0], NULL );

				P2P_PIPE_Start( FALSE, cnr_type, g_s2y_mng.is_r2y_out_ipu, g_s2y_mng.shd_en, g_s2y_mng.cag_en, FALSE, &g_s2y_mng.pipe[0], NULL, g_s2y_in.ltm_mode);
				P2P_SRO_PG_Start( &g_s2y_mng.sro_pg[0], NULL );
			}else{
				if( g_s2y_in.af_en & 0x01 )
					P2P_AF_Start( &g_s2y_mng.af[0], &g_s2y_mng.af[1] );

				P2P_PIPE_Start( FALSE, cnr_type, g_s2y_mng.is_r2y_out_ipu, g_s2y_mng.shd_en, g_s2y_mng.cag_en, FALSE, &g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1], g_s2y_in.ltm_mode);
				P2P_SRO_PG_Start( &g_s2y_mng.sro_pg[0], &g_s2y_mng.sro_pg[1] );
			}
			g_s2y_mng.prch_running_flg = 1;
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_PRCH, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "PRCH"));
		} else if( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_in.raw_out_point_forced != RAW_EXTRACT_POINT_SEN){
			for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ )
				g_s2y_mng.pipe[i].addr_aeawb = g_s2y_mng.mem_addr_aeawb[ g_s2y_mng.mem_bank_aeawb[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_aeawb[i];

			BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_aeawb, D_PIPE_MEM_BANK_AEAWB, g_s2y_mng.mem_addr_aeawb, 0); // ING = SET, SET = new. then RCV will be SET

			Get_Frame_Share_ISP_Gain( &g_s2y_mng.isp_gain_reg, frame_no ); // get from current, to update isp_gain at same timing as sensor
			COPY_PGAIN(g_s2y_mng.isp_gain_reg);

			Get_Frame_Share_AWB_Param( &g_s2y_mng.awb_param_reg, frame_no ); // get from current
			COPY_AWB_PARAM(g_s2y_mng.awb_param_reg);

			Get_Frame_Share_Sensor_Exp( &g_s2y_mng.exp, frame_no ); 
			for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
				if( g_s2y_in.sensor_use & (1<<i) ){
					g_s2y_mng.iq_gain_times[i] =  g_s2y_mng.exp.sensor[i].exp.again_times
												* g_s2y_mng.exp.sensor[i].exp.dgain_times
												* g_s2y_mng.isp_gain_reg.sensor[i].b2b_end[E_PGAIN_GR]; // may not need
					if((g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) || (g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2))
						g_s2y_mng.iq_scene[i] = g_s2y_in.scene_hdr_1st_run;
					else
						g_s2y_mng.iq_scene[i] = g_s2y_in.scene_normal;
				}
			}
			IQPara_Scene_Stream_Table( g_s2y_mng.iq_scene, g_s2y_in.sensor_num );
			IQPara_UpdateGainArray( g_s2y_mng.iq_gain_times, 0.0 );
			iq_update_sen_to_b2b( g_s2y_mng.iq_stream[0], g_s2y_mng.iq_stream[1] );
			iq_dbg2_set( -1 );

			if( g_s2y_in.af_en & 0x01 )
				P2P_AF_Start( &g_s2y_mng.af[0], &g_s2y_mng.af[1] );

			blk_type = E_IM_PRO_BLOCK_TYPE_B2B;
			lost_line = g_s2y_in.cag_en ? PIPE_LOST_LINE_DPC_CAG_ELF : PIPE_LOST_LINE_DPC_ELF;
			P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_1, blk_type, 1);
			P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_2, blk_type, 1);
			P2P_PWCH_Start(E_IM_PRO_UNIT_NUM_1, blk_type, &g_s2y_mng.ldiv_pwch[0], lost_line);
			P2P_PWCH_Start(E_IM_PRO_UNIT_NUM_2, blk_type, &g_s2y_mng.ldiv_pwch[1], lost_line);
			P2P_PIPE_SRORBKRGB_Start(&g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1]);
			P2P_PIPE_SROB2B_Start( &g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1], g_s2y_mng.shd_en, g_s2y_mng.cag_en );
			P2P_SRO_PG_Start( &g_s2y_mng.sro_pg[0], &g_s2y_mng.sro_pg[1] );
			g_s2y_mng.prch_running_flg = 1;
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_PRCH, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "PRCH"));
		}

		/* Set RAW info */
		if( g_s2y_in.raw_only_sen_id.bit.sen_id != 0xFF ) 
			g_s2y_mng.pipe_use_num = 4;

		for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ )
			g_share_sen_raw.rect[i].addr = g_s2y_mng.sro_pg[i].addr + g_s2y_mng.sro_pg[i].h->addr;

		if( g_s2y_in.raw_only_sen_id.bit.sen_id != 0xFF )
			g_s2y_mng.pipe_use_num = 2;

		if(g_s2y_in.af_en & 0x02)
			make_dualpd_data(g_s2y_mng.pipe_mode);
		else if(g_s2y_in.af_en & 0x04)
			make_pd_data(&g_share_sen_raw);

		if(g_skip_frame_num == 0) {
			Set_Frame_Share_Sensor_Raw(&g_share_sen_raw, g_raw_cnt);
			g_raw_cnt++;
		}

	break;
	case E_SYSTEM_EVENT_SRO_PRCH:
		g_s2y_mng.prch_frame_no = frame_no;
		g_s2y_mng.prch_running_flg = 0;
		if( g_s2y_mng.r2y_frame_no != g_s2y_mng.prch_frame_no && g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT){
			// r2y is Not end, so wait
			break;
		}
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_PRCH, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "PRCH"));

		/* Stop/Start PIPE */
		if( !g_s2y_mng.is_direct && g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT){
			if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ){
				P2P_PIPE_R2Y_HIST_Get( &g_share_r2y_hist.rect[0].hist, NULL );
				if( g_s2y_in.af_en & 0x01 )
					P2P_AF_Stop( TRUE, FALSE );

				P2P_PIPE_Stop( TRUE, FALSE, g_s2y_in.ltm_mode );
				UPDATE_YUV_FRAME_AND_SEND_TO_SYSTEM;		// update YUV one frame info and send to system
				if( g_s2y_in.frame_num != D_FRAME_NUM_INFINITY && (UINT32)g_cap_cnt >= g_s2y_in.frame_num )
					STM_Sensor2YUV_Stop(TRUE);
			}else{
				if( g_s2y_in.af_en & 0x01 )
					P2P_AF_Stop( TRUE, TRUE );

				P2P_PIPE_Stop( TRUE, TRUE, g_s2y_in.ltm_mode );
				
				ltm_rgb_map_process(frame_no);

				if( g_s2y_mng.pipes_2nd_time == 0 && g_s2y_mng.pipe_use_num > E_PIPE_USE_NUM_2){
					if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 )
						P2P_PIPE_R2Y_HIST_Get( &g_share_r2y_dol_hist.long_hist[0], &g_share_r2y_dol_hist.long_hist[1] );
					else
						P2P_PIPE_R2Y_HIST_Get( &g_share_r2y_hist.rect[0].hist, &g_share_r2y_hist.rect[1].hist );

					iq_update( TRUE, g_s2y_mng.iq_stream[2], g_s2y_mng.iq_stream[3] );
					iq_dbg2_set( -1 );
					
					cnr_type = g_s2y_mng.cnr_type;
					if (g_s2y_in.cap_mode) {
						IQPara_Get_IQ_CNR_Ctrl(&ctrl_0, &ctrl_1);
						if(ctrl_0.CNRMODE == 0)
							cnr_type = E_CNR_OFF;
					}

					if( g_s2y_in.af_en & 0x01 )
						P2P_AF_Start( &g_s2y_mng.af[2], &g_s2y_mng.af[3] );

					P2P_PIPE_Start( FALSE, cnr_type, g_s2y_mng.is_r2y_out_ipu, g_s2y_mng.shd_en, g_s2y_mng.cag_en, FALSE, &g_s2y_mng.pipe[2], &g_s2y_mng.pipe[3], g_s2y_in.ltm_mode);
					P2P_SRO_PG_Start( &g_s2y_mng.sro_pg[2], &g_s2y_mng.sro_pg[3] );
					g_s2y_mng.prch_running_flg = 1;
					g_s2y_mng.pipes_2nd_time = 1;
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_PRCH, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "PRCH"));
				}else{
					if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 )
						P2P_PIPE_R2Y_HIST_Get( &g_share_r2y_dol_hist.short_hist[0], &g_share_r2y_dol_hist.short_hist[1] );
					else
						P2P_PIPE_R2Y_HIST_Get( &g_share_r2y_hist.rect[2].hist, &g_share_r2y_hist.rect[3].hist );

					UPDATE_YUV_FRAME_AND_SEND_TO_SYSTEM;	// update YUV one frame info and send to system
					if( g_s2y_in.frame_num != D_FRAME_NUM_INFINITY && (UINT32)g_cap_cnt >= g_s2y_in.frame_num )
						STM_Sensor2YUV_Stop(TRUE);
				}
			}
		} else if( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT){
			if( g_s2y_mng.sro_2nd_time_s == 0) {
				iq_update( TRUE, g_s2y_mng.iq_stream[2], g_s2y_mng.iq_stream[3] );
				iq_dbg2_set( -1 );
				if( g_s2y_in.af_en & 0x01 )
					P2P_AF_Start( &g_s2y_mng.af[2], &g_s2y_mng.af[3] );

				blk_type = E_IM_PRO_BLOCK_TYPE_B2B;
				lost_line = g_s2y_in.cag_en ? PIPE_LOST_LINE_DPC_CAG_ELF : PIPE_LOST_LINE_DPC_ELF;
				P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_1, blk_type, 1);
				P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_2, blk_type, 1);
				P2P_PWCH_Start(E_IM_PRO_UNIT_NUM_1, blk_type, &g_s2y_mng.ldiv_pwch[2], lost_line);
				P2P_PWCH_Start(E_IM_PRO_UNIT_NUM_2, blk_type, &g_s2y_mng.ldiv_pwch[3], lost_line);
				P2P_PIPE_SRORBKRGB_Start(&g_s2y_mng.pipe[2], &g_s2y_mng.pipe[3]);
				P2P_PIPE_SROB2B_Start( &g_s2y_mng.pipe[2], &g_s2y_mng.pipe[3], g_s2y_mng.shd_en, g_s2y_mng.cag_en );
				P2P_SRO_PG_Start( &g_s2y_mng.sro_pg[2], &g_s2y_mng.sro_pg[3] );
				g_s2y_mng.sro_2nd_time_s   = 1;
				g_s2y_mng.prch_running_flg = 1;
				MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_PRCH, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "PRCH"));
			} else {
				BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_raw1, g_s2y_mng.mem_bank_raw_num, g_s2y_mng.mem_addr_raw1, param.lock_address_raw1);
				for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ )
					g_s2y_mng.ldiv_pwch[i].addr1 = g_s2y_mng.mem_addr_raw1[ g_s2y_mng.mem_bank_raw1[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_raw[i];

				BANK_ING_TO_RCV(g_s2y_mng.mem_bank_raw1);
				/* No matter direct/non-direct we need to update sro_pg addr here since direct mode we can ouput RAW as well */
				for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ )
					g_s2y_mng.sro_pg[i].addr1 = g_s2y_mng.mem_addr_raw1[ g_s2y_mng.mem_bank_raw1[E_MEM_BANK_RCV] ] + g_s2y_mng.addr_ofst_raw[i];

				/* Set RAW info */
				for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ )
					g_share_sen_raw.rect[i].addr1 = g_s2y_mng.sro_pg[i].addr1 + g_s2y_mng.sro_pg[i].h->addr;

				if(g_skip_frame_num == 0) {
					Set_Frame_Share_Sensor_Raw(&g_share_sen_raw, g_raw1_cnt);
					g_raw1_cnt++;
				}				
			}
		}
	break;
	case E_SYSTEM_EVENT_LTM_MAP:

		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_LTM_MAP, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "LTMMAP%d", (UINT32)frame_no));

		// High Speed Capture
		if (g_s2y_mng.hscap_en && g_skip_frame_num == 0) {
			BOOL start_pipe = FALSE;
			
			start_pipe = !HSCap_Is_Close_MShutter( frame_no - CAMERA.start_frame_num);
			
			if( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT ){
				if( start_pipe ){
					HSCap_Measure_Pipe_Start( frame_no - CAMERA.start_frame_num );
					// not break
				}else{
					HSCap_Start_Pipe_Dummy();
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SRO_R2Y, FJ_MBALOG_FUNC_ID_COMMON, FJ_MBALOG_TYPE_START, "Dummy %d", (INT32)g_yuv_cnt));
					break;
				}
			}
		}

		if( (g_s2y_mng.pipe_use_num <= 2) || (g_s2y_mng.pipe_use_num == 4 && g_s2y_mng.div_num == 4)) {
			// PIPE use num <= 2, or sensorx1, div4
			// We update map bank and get map info
			BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_map, g_s2y_mng.mem_bank_map_num, g_s2y_mng.mem_addr_map_yp, 0xFFFFFFFF);
			for( i = 0; i < g_s2y_mng.pipe_use_num; i++ ) {
				if( g_s2y_mng.pipe[i].addr_map_yp ) {
					g_s2y_mng.pipe[i].addr_map_yp = g_s2y_mng.mem_addr_map_yp[g_s2y_mng.mem_bank_map[E_MEM_BANK_SET]] + g_s2y_mng.addr_ofst_map[i];
					g_s2y_mng.pipe[i].addr_map_yn = g_s2y_mng.mem_addr_map_yn[ g_s2y_mng.mem_bank_map[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_map[i];
				}
			}

			BANK_ING_TO_RCV(g_s2y_mng.mem_bank_map);

			for( i = 0; i < g_s2y_mng.pipe_use_num; i++ ) {
				if( g_s2y_mng.pipe[i].addr_map_yp_recv ) {
					g_s2y_mng.pipe[i].addr_map_yp_recv = g_s2y_mng.mem_addr_map_yp[g_s2y_mng.mem_bank_map[E_MEM_BANK_RCV]] + g_s2y_mng.addr_ofst_map[i];
					g_s2y_mng.pipe[i].addr_map_yn_recv = g_s2y_mng.mem_addr_map_yn[ g_s2y_mng.mem_bank_map[E_MEM_BANK_RCV] ] + g_s2y_mng.addr_ofst_map[i];
				}

				Im_LTM_MAP_Get_Internal_Param( i < 2 ? i : i - 2, &internal_param );
				g_s2y_mng.pipe[i].yshadow		= internal_param.yshadow;
				g_s2y_mng.pipe[i].yhighlight	= internal_param.yhighlight;
			}

			if (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT) {
				b2r_r2y_map_proc(frame_no);
				if( g_s2y_mng.pipe_use_num < E_PIPE_USE_NUM_4 ) {
					if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 )
						P2P_PIPE_SRORBKMAP_Start(FALSE, &g_s2y_mng.pipe[0], NULL);
					else
						P2P_PIPE_SRORBKMAP_Start(FALSE, &g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1]);
				}
			}

		} else if( g_s2y_mng.pipe_use_num == 4 && g_s2y_mng.ltmmap_2nd_time == 0 && g_s2y_mng.div_num < 4 ) {
			// sensorx2 div2, sensorx4 div1
			// get map info 1st time
			for( i = 0; i < 2; i++ ) {
				if( g_s2y_mng.pipe[i].addr_map_yp_recv ) {
					Im_LTM_MAP_Get_Internal_Param(i, &internal_param);
					g_s2y_mng.pipe[i].yshadow		= internal_param.yshadow;
					g_s2y_mng.pipe[i].yhighlight	= internal_param.yhighlight;
				}
			}
			if (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT)
				b2r_r2y_map_proc(frame_no);

		} else if( g_s2y_mng.pipe_use_num == 4 && g_s2y_mng.ltmmap_2nd_time == 1 && g_s2y_mng.div_num < 4 ) {
			// sensorx2 div2, sensorx4 div1
			// get map info 2nd time
			for( i = 2; i < g_s2y_mng.pipe_use_num; i++ ) {
				if( g_s2y_mng.pipe[i].addr_map_yp_recv ) {
					Im_LTM_MAP_Get_Internal_Param(i - 2, &internal_param);
					g_s2y_mng.pipe[i].yshadow		= internal_param.yshadow;
					g_s2y_mng.pipe[i].yhighlight	= internal_param.yhighlight;
				}
			}
			// update map bank
			BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_map, g_s2y_mng.mem_bank_map_num, g_s2y_mng.mem_addr_map_yp, 0xFFFFFFFF);
			for( i = 0; i < g_s2y_mng.pipe_use_num; i++ ) {
				if( g_s2y_mng.pipe[i].addr_map_yp ) {
					g_s2y_mng.pipe[i].addr_map_yp = g_s2y_mng.mem_addr_map_yp[g_s2y_mng.mem_bank_map[E_MEM_BANK_SET]] + g_s2y_mng.addr_ofst_map[i];
					g_s2y_mng.pipe[i].addr_map_yn = g_s2y_mng.mem_addr_map_yn[ g_s2y_mng.mem_bank_map[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_map[i];
				}
			}

			BANK_ING_TO_RCV(g_s2y_mng.mem_bank_map);

			for( i = 0; i < g_s2y_mng.pipe_use_num; i++ ) {
				if( g_s2y_mng.pipe[i].addr_map_yp_recv ) {
					g_s2y_mng.pipe[i].addr_map_yp_recv = g_s2y_mng.mem_addr_map_yp[g_s2y_mng.mem_bank_map[E_MEM_BANK_RCV]] + g_s2y_mng.addr_ofst_map[i];
					g_s2y_mng.pipe[i].addr_map_yn_recv = g_s2y_mng.mem_addr_map_yn[ g_s2y_mng.mem_bank_map[E_MEM_BANK_RCV] ] + g_s2y_mng.addr_ofst_map[i];
				}
			}
		}
		break;
	case E_SYSTEM_EVENT_LTM:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_LTM_R2Y, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "LTM%d", (UINT32)frame_no));
	break;
	case E_SYSTEM_EVENT_LTM_RGB:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_LTM_RGB, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "LTMRGB%d", (UINT32)frame_no));
		g_s2y_mng.ltmrgb_cnt++;
		// Trigger LTMMAP later in PRch since MAP end INT not occured
		if (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.div_num == 4 && (g_s2y_mng.ltmrgb_cnt & 0x1) == 0)
			g_s2y_mng.ltm_rgb_rdy = TRUE;
		else if( g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.div_num == 4 && g_s2y_mng.pipes_2nd_time == 1)
			g_s2y_mng.ltm_rgb_rdy = TRUE;
		else if( g_s2y_mng.div_num == 2 || g_s2y_mng.div_num == 1 )
			g_s2y_mng.ltm_rgb_rdy = TRUE;
		if( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT)
			ltm_rgb_map_process(frame_no);
	break;
	case E_SYSTEM_EVENT_AEAWB_PWCH:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_3A, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "AEAWB"));
		
		if( g_s2y_mng.pipe_use_num <= E_PIPE_USE_NUM_2 ){
			UPDATE_AEAWB_FRAME_AND_SEND_TO_USER;
		}else{
			if( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num == 4) {
				if( g_s2y_mng.sro_2nd_time_s && (frame_no & 0x1))
					UPDATE_AEAWB_FRAME_AND_SEND_TO_USER;
			} else {
				if( (g_s2y_mng.pipes_2nd_time == 1) && (frame_no & 0x1) )
					UPDATE_AEAWB_FRAME_AND_SEND_TO_USER;
			}
		}
	break;
	case E_SYSTEM_EVENT_PRO_HIST:
		MPRINTF((D_MBA_KIND_PREVIEW, FJ_MBALOG_LINE_ID_HIST, FJ_MBALOG_FUNC_ID_3A_3, FJ_MBALOG_TYPE_POINT, "PROHIST"));
		if( g_s2y_mng.pipe_use_num <= E_PIPE_USE_NUM_2 ) {
			if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL ) P2P_PIPE_PRO_HIST_Get( &g_share_pro_dol_hist.long_hist[0], &g_share_pro_dol_hist.short_hist[0] );
			else {
				if (g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1) P2P_PIPE_PRO_HIST_Get( &g_share_pro_hist.rect[0].hist, NULL );
				else P2P_PIPE_PRO_HIST_Get( &g_share_pro_hist.rect[0].hist, &g_share_pro_hist.rect[1].hist );
			}
			UPDATE_PRO_HIST;
		} else {
			if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 ) {
				if( g_s2y_mng.pipes_2nd_time == 0) P2P_PIPE_PRO_HIST_Get( &g_share_pro_dol_hist.long_hist[0], &g_share_pro_dol_hist.long_hist[1] );
				else if( g_s2y_mng.pipes_2nd_time == 1) {
					P2P_PIPE_PRO_HIST_Get( &g_share_pro_dol_hist.short_hist[0], &g_share_pro_dol_hist.short_hist[1] );
					UPDATE_PRO_HIST;
				}
			} else {
				if( g_s2y_mng.pipes_2nd_time == 0) P2P_PIPE_PRO_HIST_Get( &g_share_pro_hist.rect[0].hist, &g_share_pro_hist.rect[1].hist );
				else if( g_s2y_mng.pipes_2nd_time == 1 || g_s2y_mng.sro_2nd_time_s == 1) {
					P2P_PIPE_PRO_HIST_Get( &g_share_pro_hist.rect[2].hist, &g_share_pro_hist.rect[3].hist );
					UPDATE_PRO_HIST;
				}
			}
		}
	break;
	case E_SYSTEM_EVENT_AF:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_3A, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "AF"));
		if( g_s2y_in.af_en & 0x01 ){
			if( g_s2y_mng.pipe_use_num <= E_PIPE_USE_NUM_2 ){
				P2P_AF_Get( &g_s2y_mng.af_data[0], &g_s2y_mng.af_data[1]);
				send_af_data();
			}else{
				if( g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num == 4) {
					if( g_s2y_mng.sro_2nd_time_s == 0 ) {
						P2P_AF_Get( &g_s2y_mng.af_data[0], &g_s2y_mng.af_data[1]);
					}else{
						P2P_AF_Get( &g_s2y_mng.af_data[2], &g_s2y_mng.af_data[3]);
						send_af_data();
					}
				} else {
					if( g_s2y_mng.pipes_2nd_time == 0 ) {
						P2P_AF_Get( &g_s2y_mng.af_data[0], &g_s2y_mng.af_data[1]);
					}else{
						P2P_AF_Get( &g_s2y_mng.af_data[2], &g_s2y_mng.af_data[3]);
						send_af_data();
					}
				}
			}
		}
	break;
	case E_SYSTEM_EVENT_AWB_END:
		Get_Share_AWB_Param( &g_s2y_mng.awb_param );
	break;
	case E_SYSTEM_EVENT_AF_UPDATE:
		Get_Share_AF_Param( &g_s2y_mng.af_param_share );
	break;
	case E_SYSTEM_EVENT_AE_END:
		Get_Share_RGB_Param(&rgb);
		Get_Share_AE_Param( &g_s2y_mng.ae_param_share );
		for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ) {
			g_s2y_mng.ae_param_share.isp_gain.sensor[i].b2b_end[E_PGAIN_R] *= (float)((float)rgb.golden_rg / (float)rgb.rg);
			g_s2y_mng.ae_param_share.isp_gain.sensor[i].b2b_end[E_PGAIN_B] *= (float)((float)rgb.golden_bg / (float)rgb.bg);

			g_s2y_mng.ae_param_share.isp_gain.sensor[i].DOL_short_b2b_end[E_PGAIN_R] *= (float)((float)rgb.golden_rg / (float)rgb.rg);
			g_s2y_mng.ae_param_share.isp_gain.sensor[i].DOL_short_b2b_end[E_PGAIN_B] *= (float)((float)rgb.golden_bg / (float)rgb.bg);
		}
	break;
	case E_SYSTEM_EVENT_CAP_PIPE_DUMMY:
		if( g_s2y_mng.hscap_en && g_skip_frame_num == 0) {
			ULLONG hscap_frame_no = 0;
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SRO_R2Y, FJ_MBALOG_FUNC_ID_COMMON, FJ_MBALOG_TYPE_END, "Dummy %d", (INT32)g_yuv_cnt));
			g_yuv_cnt++;
			if( HSCap_Judge_Start_NextProcess( &hscap_frame_no ) ){
				if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 || g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) {
					Snd_Event_to_System( E_SYSTEM_EVENT_PIPE_DOL_YUV, hscap_frame_no );
					g_cap_cnt++;
				}else{
					Snd_Event_to_System( E_SYSTEM_EVENT_CNR_OFL, hscap_frame_no );
				}
			}
		}
	break;
	case E_SYSTEM_EVENT_CNR_OFL:
		g_cnr_ofl_fno = frame_no;
		OS_User_Set_Flg( FID_IQ_ACCESS, FLG_IQ_ACCESS_CNR_OFL_PROC );

		g_cap_cnt++;
		if( g_s2y_in.frame_num != D_FRAME_NUM_INFINITY && (UINT32)g_cap_cnt >= g_s2y_in.frame_num )
			STM_Sensor2YUV_Stop(TRUE);
	break;
	case E_SYSTEM_EVENT_DOL_COMBINE:
		if( g_s2y_mng.hscap_en && g_skip_frame_num == 0) {
			if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 || g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL )
				HSCap_Pull_PipeQ();
		}
	break;
	case E_SYSTEM_EVENT_SHDR_YUV:
		if( g_s2y_mng.hscap_en && g_skip_frame_num == 0) {
			if( g_s2y_mng.pipe_mode != E_PIPE_MODE_SENSORx1_DOL_DIV2 && g_s2y_mng.pipe_mode != E_PIPE_MODE_SENSORx1_DOL )
				HSCap_Pull_PipeQ();
		}
	break;
	case E_SYSTEM_EVENT_CAP_MEDIA_REL:
		if( g_s2y_mng.hscap_en && g_skip_frame_num == 0)
			HSCap_Pull_MediaQ();
		else
			g_cap_cnt++;
	break;
	default:
	break;
	}
	
	return g_sensor2yuv_state;
}


INT32 STM_Sensor2YUV_Stop( BOOL force_stop)
{
	INT32 i;

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SYSTEM, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "Stop"));
	
	// need wait prch end. if not, next mode may receive PRCH event.
	if( g_s2y_mng.prch_running_flg && !g_s2y_mng.is_direct ){
		if( !g_s2y_mng.hscap_en && g_skip_frame_num == 0){ // if hscap_en, jpeg takes more time and may skip other event, so skip.
			Wait_Specified_Event( E_SYSTEM_EVENT_SRO_PRCH );
		}
		g_s2y_mng.prch_running_flg = 0;
	}
	
	P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SRO, 1);
	P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_SRO, 1);
	P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_B2B, 1);
	P2P_PWCH_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_B2B, 1);

	// --- REMOVE_WDR BEGIN ---
	WDR_Realtime_Wdr_LPF_Init(OFF);
	// --- REMOVE_WDR END ---

	// need think the way of stop
	if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ){
		if( g_s2y_in.af_en & 0x01 ){
			P2P_AF_Stop( TRUE, FALSE );
		}
		P2P_PIPE_Stop( TRUE, FALSE, g_s2y_in.ltm_mode );
	}else{
		if( g_s2y_in.af_en & 0x01 ){
			P2P_AF_Stop( TRUE, TRUE );
		}
		P2P_PIPE_Stop( TRUE, TRUE, g_s2y_in.ltm_mode );
	}
	P2P_SRO_PG_Stop( TRUE, TRUE );
	P2P_LDIV_PWCH_Stop();
	P2P_SEN_Stop();
	
	for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
		if( g_s2y_in.sensor_use & (1<<i) ){
			switch(g_s2y_mng.sen.sensor_connect[ i ]){
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI:
				P2P_CPHY_DMIPI_Stop();
			break;
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_SLVS:
				P2P_CPHY_SLVS_Stop();
			break;
			case E_IM_PRO_SENTOP_CONN_TYPE_SEL_LVDS:
				P2P_CPHY_LVDS_Stop();
			break;
			default:
			break;
			}
		}
	}
	
	for( i = E_SHARE_SENSOR_NUM_MAX-1; i >= 0; i-- ){
		if( g_s2y_in.sensor_use & (1<<i) ){
			if(g_s2y_in.global_shut_en) {
				if( CAMERA.sensor[ i ].Func_GlobalShutter ){
					CAMERA.sensor[ i ].Func_GlobalShutter( i, E_CAMERA_SENSOR_SHUTTER_G_OFF_M_OPEN );
				}
			}
			if( CAMERA.sensor[ i ].Func_Standby ){
				CAMERA.sensor[ i ].Func_Standby( i );
			}
		}
	}
	
	if(force_stop == FALSE){
		// wait frame output time for safety
		if( CAMERA.sensor[0].mode[g_s2y_in.sensor_mode[0]].fps > 0.0 ){
			OS_User_Dly_Tsk( (UINT32)( 1000/CAMERA.sensor[0].mode[g_s2y_in.sensor_mode[0]].fps) );
		}
	}

	OS_User_Set_Flg( FID_IQ_ACCESS, FLG_IQ_ACCESS_EXIT );
	OS_User_Set_Flg( FID_SENSOR_ACCESS, FLG_SENSOR_ACCESS_EXIT );

	g_sensor2yuv_state = D_STM_IDLE;
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SYSTEM, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "Stop"));
	
	return g_sensor2yuv_state;
}


INT32 STM_Sensor2YUV_Get_Sensor_Size( E_SHARE_SENSOR_ID sensor_id, T_MODE_VIEW_RECT* size )
{
	INT32 i = sensor_id;
	size->h_ofst = g_p2p_calc_out[i].ldiv[E_SHARE_H_DIV_1-1].out_h_ofst[E_SHARE_H_DIV_1-1] + g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].b2r_ofst;
	size->v_ofst = g_p2p_calc_out[i].pipe_v.b2r_ofst;
	size->h_size = g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].b2r_size;
	size->v_size = g_p2p_calc_out[i].pipe_v.b2r_size;
	return D_STM_OK;
}


INT32 STM_Sensor2YUV_Get_Pipe_Size( E_SHARE_SENSOR_ID sensor_id, T_MODE_VIEW_RECT* size )
{
	INT32 i = sensor_id;
	size->h_ofst   = 0;
	size->v_ofst   = 0;
	size->h_size   = g_s2y_in.one_sensor_yuv_h_size[i];
	size->v_size   = g_s2y_in.one_sensor_yuv_v_size[i];
	return D_STM_OK;
}


INT32 STM_Sensor2YUV_Get_AE_Size( E_SHARE_SENSOR_ID sensor_id, T_MODE_VIEW_RECT* size )
{
	INT32 i = sensor_id;
	size->h_ofst = g_p2p_calc_out[i].ldiv[E_SHARE_H_DIV_1-1].out_h_ofst[E_SHARE_H_DIV_1-1] + g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_ofst;
	size->v_ofst = g_p2p_calc_out[i].pipe_v.aeawb_blk_ofst;
	size->h_size = (g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_num - 2) * g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size
				 + g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size_L + g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size_R;
	size->v_size = g_p2p_calc_out[i].pipe_v.aeawb_blk_num * g_p2p_calc_out[i].pipe_v.aeawb_blk_size;
	return D_STM_OK;
}


VOID Sensor_Access_Task( INT32 stacd )
{
	OS_USER_FLGPTN	flgptn = 0;
	INT32 i;
	
	while(1)
	{
		// wait event
		OS_User_Twai_Flg( FID_SENSOR_ACCESS, FLG_SENSOR_ACCESS_WAITPTN, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg( FID_SENSOR_ACCESS, ~FLG_SENSOR_ACCESS_WAITPTN );
		
		// Sensor Update
		if( IS_FLGPTN_EQUAL( flgptn, FLG_SENSOR_ACCESS_UPDATE ) ){
			for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
				if( g_s2y_in.sensor_use & (1<<i) ){

					if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 || g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL ) {
						if( CAMERA.sensor[i].Func_DOL_Set_Exp && g_s2y_in.sensor_exp_NO_update == FALSE) {
							CAMERA.sensor[ i ].Func_DOL_Set_Exp( i, g_s2y_in.sensor_mode[i], &g_s2y_mng.ae_param.reg.sensor[i].reg );
						}
					}else{
						if( CAMERA.sensor[ i ].Func_Set_Exp  && g_s2y_in.sensor_exp_NO_update == FALSE){
								CAMERA.sensor[ i ].Func_Set_Exp( i, g_s2y_in.sensor_mode[i], &g_s2y_mng.ae_param.reg.sensor[i].reg );
						}
					}
					if( CAMERA.sensor[ i ].Func_SgVHD ){
						if( g_s2y_in.sensor_exp_NO_update == FALSE ) {
							CAMERA.sensor[i].Func_SgVHD(i, g_s2y_mng.ae_param.reg.sensor[i].reg.reg[E_CAMERA_SENSOR_REG_VDOCYC], g_s2y_mng.ae_param.reg.sensor[i].reg.reg[E_CAMERA_SENSOR_REG_HDOCYC]);
						} else {
							CAMERA.sensor[i].Func_SgVHD(i, CAMERA.sensor[i].mode[g_s2y_in.sensor_mode[i]].vdocyc, CAMERA.sensor[i].mode[g_s2y_in.sensor_mode[i]].hdocyc);
						}
					}
				}
			}
		}
		
		// Shutter Control
		if( IS_FLGPTN_EQUAL( flgptn, FLG_SENSOR_SHUTTER_CONTROL ) ){
			if ((g_s2y_in.global_shut_en == TRUE) && (g_cap_shutter_release == TRUE)) {
				for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
					if( g_s2y_in.sensor_use & (1<<i) ){
						if( g_s2y_mng.pipe_mode != E_PIPE_MODE_SENSORx1_DOL_DIV2 && g_s2y_mng.pipe_mode != E_PIPE_MODE_SENSORx1_DOL) {
							if( CAMERA.sensor[ i ].Func_GlobalShutter ){
								CAMERA.sensor[ i ].Func_GlobalShutter( i, E_CAMERA_SENSOR_SHUTTER_G_ON_M_CLOSE );
							}
						}
					}
				}
				g_cap_shutter_release = FALSE;
			}
		}
		
		// Exit Task
		if( IS_FLGPTN_EQUAL( flgptn, FLG_SENSOR_ACCESS_EXIT ) ){
			break;
		}
	}
	OS_User_Ext_Tsk();
}

VOID IQ_Access_Task( INT32 stacd )
{
	OS_USER_FLGPTN flgptn = 0;

	while(1)
	{
		// wait event
		OS_User_Twai_Flg( FID_IQ_ACCESS, FLG_IQ_ACCESS_WAITPTN, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg( FID_IQ_ACCESS, ~FLG_IQ_ACCESS_WAITPTN );

		if( IS_FLGPTN_EQUAL( flgptn, FLG_IQ_ACCESS_UPDATE_PIPE_WITHOUT_TABLE ) ){
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IQ, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_START, "IQ"));
			iq_update( FALSE, g_s2y_mng.iq_stream[0], g_s2y_mng.iq_stream[1] );
			iq_dbg2_set( IQ_DBG2_CLR_FLG( (-1), IQ_DBG2_CONV_FLG(E_IQ_DBG2_DATA_TYPE_R2Y_TBL) ) );
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IQ, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_END, "IQ"));
		}

		if( IS_FLGPTN_EQUAL( flgptn, FLG_IQ_ACCESS_CNR_OFL_PROC ) ){
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_CNR_OFL, FJ_MBALOG_FUNC_ID_SUPPRE, FJ_MBALOG_TYPE_START, "CNR_OFL %d", (UINT32)g_cnr_ofl_fno));

			Get_Frame_Share_PIPE_YUV(&g_share_cnr_ofl_yuv, g_cnr_ofl_fno);
			Ctrl_CNR_OFL_Process(&g_share_cnr_ofl_yuv.layout);				
			Snd_Event_to_System( E_SYSTEM_EVENT_PIPE_YUV, g_cnr_ofl_fno );

			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_CNR_OFL, FJ_MBALOG_FUNC_ID_SUPPRE, FJ_MBALOG_TYPE_END, "CNR_OFL %d", (UINT32)g_cnr_ofl_fno));
		}

		// Exit Task
		if( IS_FLGPTN_EQUAL( flgptn, FLG_IQ_ACCESS_EXIT ) ){
			break;
		}
	}

	OS_User_Ext_Tsk();
}

/*==============================================================================*/
/* Local Function   															*/
/*==============================================================================*/
static void iq_update( BOOL is_tbl, UCHAR stream0, UCHAR stream1 )
{
	if( stream0 == stream1 ){
		IQPara_SEN( stream0, D_IQ_PIPE01 );
		IQPara_SRO( stream0, D_IQ_PIPE01 );
		IQPara_B2B( stream0, D_IQ_PIPE01 );
		IQPara_B2R( stream0, D_IQ_PIPE01 );
		if( g_s2y_in.dol_sencore_use != 0x3 ) {
			IQPara_LTM(stream0, D_IQ_PIPE01);
		}
		IQPara_R2Y( stream0, D_IQ_PIPE01 );
		IQPara_CNR( stream0, D_IQ_PIPE01 );
		if( is_tbl ){
			IQPara_R2Y_Tbl( stream0, D_IQ_PIPE01 );
		}
	}else{
		IQPara_SEN( stream0, D_IQ_PIPE0 );
		IQPara_SRO( stream0, D_IQ_PIPE0 );
		IQPara_B2B( stream0, D_IQ_PIPE0 );
		IQPara_B2R( stream0, D_IQ_PIPE0 );
		if( g_s2y_in.dol_sencore_use != 0x3 ) {
			IQPara_LTM( stream0, D_IQ_PIPE0 );
		}
		IQPara_R2Y( stream0, D_IQ_PIPE0 );
		IQPara_CNR( stream0, D_IQ_PIPE0 );
		if( is_tbl ){
			IQPara_R2Y_Tbl( stream0, D_IQ_PIPE0 );
		}

		IQPara_SEN( stream1, D_IQ_PIPE1 );
		IQPara_SRO( stream1, D_IQ_PIPE1 );
		IQPara_B2B( stream1, D_IQ_PIPE1 );
		IQPara_B2R( stream1, D_IQ_PIPE1 );
		if( g_s2y_in.dol_sencore_use != 0x3 ) {
			IQPara_LTM( stream1, D_IQ_PIPE1 );
		}
		IQPara_R2Y( stream1, D_IQ_PIPE1 );
		IQPara_CNR( stream1, D_IQ_PIPE1 );
		if( is_tbl ){
			IQPara_R2Y_Tbl( stream1, D_IQ_PIPE1 );
		}
	}

	IQDbg_Dump_IQ_PrintOut(stream0, 0);
	
	if( g_s2y_in.disable_dpc ){
		P2P_PIPE_DPC_Stop( TRUE, TRUE );
	}
}

static void iq_update_sen_to_b2b( UCHAR stream0, UCHAR stream1 )
{
	if( stream0 == stream1 ){
		IQPara_SEN( stream0, D_IQ_PIPE01 );
		IQPara_SRO( stream0, D_IQ_PIPE01 );
		IQPara_B2B( stream0, D_IQ_PIPE01 );
	}else{
		IQPara_SEN( stream0, D_IQ_PIPE0 );
		IQPara_SRO( stream0, D_IQ_PIPE0 );
		IQPara_B2B( stream0, D_IQ_PIPE0 );
		
		IQPara_SEN( stream1, D_IQ_PIPE1 );
		IQPara_SRO( stream1, D_IQ_PIPE1 );
		IQPara_B2B( stream1, D_IQ_PIPE1 );
	}

	IQDbg_Dump_IQ_PrintOut(stream0, 0);
	
	if( g_s2y_in.disable_dpc ){
		P2P_PIPE_DPC_Stop( TRUE, TRUE );
	}
}

static void iq_update_b2r_to_cnr( BOOL is_tbl, UCHAR stream0, UCHAR stream1 )
{
	if( stream0 == stream1 ){
		IQPara_B2R( stream0, D_IQ_PIPE01 );
		if( g_s2y_in.dol_sencore_use != 0x3 ) {
			IQPara_LTM(stream0, D_IQ_PIPE01);
		}
		IQPara_R2Y( stream0, D_IQ_PIPE01 );
		IQPara_CNR( stream0, D_IQ_PIPE01 );
		if( is_tbl ){
			IQPara_R2Y_Tbl( stream0, D_IQ_PIPE01 );
		}
	}else{
		IQPara_B2R( stream0, D_IQ_PIPE0 );
		if( g_s2y_in.dol_sencore_use != 0x3 ) {
			IQPara_LTM( stream0, D_IQ_PIPE0 );
		}
		IQPara_R2Y( stream0, D_IQ_PIPE0 );
		IQPara_CNR( stream0, D_IQ_PIPE0 );
		if( is_tbl ){
			IQPara_R2Y_Tbl( stream0, D_IQ_PIPE0 );
		}

		IQPara_B2R( stream1, D_IQ_PIPE1 );
		if( g_s2y_in.dol_sencore_use != 0x3 ) {
			IQPara_LTM( stream1, D_IQ_PIPE1 );
		}
		IQPara_R2Y( stream1, D_IQ_PIPE1 );
		IQPara_CNR( stream1, D_IQ_PIPE1 );
		if( is_tbl ){
			IQPara_R2Y_Tbl( stream1, D_IQ_PIPE1 );
		}
	}

	IQDbg_Dump_IQ_PrintOut(stream0, 0);
}

static void gyro_irq_callbak( T_CALLBACK_ID* id, ULONG result, ULONG user_param )
{
     UINT64 frame_no = 0;
     UCHAR locate_cnt = 0;

//   printf("\n gyro_irq_callbak(%d, %08lx)", id->channel, result);
     if( IS_FLGPTN_EQUAL(result, D_IM_PRO_INT_GYRO_EOFF0) ){
          // Frame timestamp update
          frame_no = g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_0].frame_no;
          locate_cnt = (frame_no % 3);
          // Frame number update
          Dd_TMR64_Get_FrameStart_TimeStamp( E_SHARE_SENSOR_ID_0, locate_cnt, (UINT64*)&g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_0].sof );
          g_gyro_current_flg |= (1<<E_SHARE_SENSOR_ID_0);

          Dd_TMR64_Get_FrameEnd_TimeStamp( E_SHARE_SENSOR_ID_0, locate_cnt, (UINT64*)&g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_0].eof );
          g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_0].frame_no++;
          g_gyro_current_flg |= ( (1<<E_SHARE_SENSOR_ID_0) << 16 );
     }
     if( IS_FLGPTN_EQUAL(result, D_IM_PRO_INT_GYRO_EOFF1) ){
          // Frame timestamp update
          frame_no = g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_1].frame_no;
          locate_cnt = (frame_no % 3);

          Dd_TMR64_Get_FrameStart_TimeStamp( E_SHARE_SENSOR_ID_1, locate_cnt, (UINT64*)&g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_1].sof );
          g_gyro_current_flg |= (1<<E_SHARE_SENSOR_ID_1);

          Dd_TMR64_Get_FrameEnd_TimeStamp( E_SHARE_SENSOR_ID_1, locate_cnt, (UINT64*)&g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_1].eof );
          // Frame number update
          g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_1].frame_no++;
          g_gyro_current_flg |= ( (1<<E_SHARE_SENSOR_ID_1) << 16 );
     }
     if( IS_FLGPTN_EQUAL(result, D_IM_PRO_INT_GYRO_EOFF2) ){
          // Frame timestamp update
          frame_no = g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_2].frame_no;
          locate_cnt = (frame_no % 3);

          Dd_TMR64_Get_FrameStart_TimeStamp( E_SHARE_SENSOR_ID_2, locate_cnt, (UINT64*)&g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_2].sof );
          g_gyro_current_flg |= (1<<E_SHARE_SENSOR_ID_1);

          Dd_TMR64_Get_FrameEnd_TimeStamp( E_SHARE_SENSOR_ID_2, locate_cnt, (UINT64*)&g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_2].eof );
          // Frame number update
          g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_2].frame_no++;
          g_gyro_current_flg |= ( (1<<E_SHARE_SENSOR_ID_2) << 16 );
     }
     if( IS_FLGPTN_EQUAL(result, D_IM_PRO_INT_GYRO_EOFF3) ){
          // Frame timestamp update
          frame_no = g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_3].frame_no;
          locate_cnt = (frame_no % 3);

          Dd_TMR64_Get_FrameStart_TimeStamp( E_SHARE_SENSOR_ID_3, locate_cnt, (UINT64*)&g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_3].sof );
          g_gyro_current_flg |= (1<<E_SHARE_SENSOR_ID_3);

          Dd_TMR64_Get_FrameEnd_TimeStamp( E_SHARE_SENSOR_ID_3, locate_cnt, (UINT64*)&g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_3].eof );
          // Frame number update
          g_frame_timestamp.sensor[E_SHARE_SENSOR_ID_3].frame_no++;
          g_gyro_current_flg |= ( (1<<E_SHARE_SENSOR_ID_3) << 16 );
     }
     
     // set share at irq mode. if go to task, g_frame_timestamp may be confilict. 
     if( g_gyro_current_flg == g_gyro_compare_flg ){
          g_gyro_current_flg = 0;
          Set_Frame_Share_FrameStamp((const T_SHARE_FRAME_TIMESTAMP*)&g_frame_timestamp, g_gyro_cnt++);
     }
}


static void vblank_irq_callback( T_CALLBACK_ID* id, ULONG result, ULONG user_param )
{
	// the sensor update timing follows the setting of sensor A.
	if( CAMERA.sensor[ A ].basic->update_timing ==  E_CAMERA_SENSOR_UPDATE_TIMING_V_BLANKING){
		OS_User_Set_Flg( FID_SENSOR_ACCESS, FLG_SENSOR_ACCESS_UPDATE );
	}

	return;
}


static void r2y_irq_callback( T_CALLBACK_ID* id, ULONG result, ULONG user_param )
{
	// this func is called in IRQ Mode.
	// please don't do long process
	// for direct mode, update Tbl during R2Y end, as Debug
	if( g_s2y_mng.is_direct && g_s2y_in.iq_tbl_update_when_direct )
	{
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IQ, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_START, "IQ Tbl"));
		if( g_s2y_mng.iq_stream[0]== g_s2y_mng.iq_stream[1] ){
			IQPara_R2Y_Tbl( g_s2y_mng.iq_stream[0], D_IQ_PIPE01 );
		}else{
			IQPara_R2Y_Tbl( g_s2y_mng.iq_stream[0], D_IQ_PIPE0 );
			IQPara_R2Y_Tbl( g_s2y_mng.iq_stream[1], D_IQ_PIPE1 );
		}
		iq_dbg2_set( IQ_DBG2_CONV_FLG(E_IQ_DBG2_DATA_TYPE_R2Y_TBL) );
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IQ, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_END, "IQ Tbl"));
	}
}

static INT32 adjust_sensor_input( UINT32 available_div, E_SHARE_SENSOR_ID sensor, E_SHARE_H_DIV div, UINT32 ldiv_h_size )
{
	UINT32 adjust = 32;
	
	UPRINTF_ERR(1);
	UPRINTF("Sensor Effective Size is changed forcely\n");
	UPRINTF("  Original Sensor Effective %d x %d\n", g_p2p_calc_in[sensor].sensor_h_effective_size, g_p2p_calc_in[sensor].sensor_v_effective_size);
	
	while( 1 ){
		// Cannot Adjust
		if( available_div & (D_PIPE_CALC_ERR_H_OUT_BIGGER_IN | D_PIPE_CALC_ERR_V_OUT_BIGGER_IN | D_PIPE_CALC_ERR_PIT_OVER) ){
			return D_STM_NG;
		}
		
		// check ldiv size
		if( g_p2p_calc_out[sensor].ldiv[div-1].in_h_size > ldiv_h_size ){
			available_div &= (~div);
			available_div |= (D_PIPE_CALC_ERR_H_IN_VIOLATE_DIV(div));
		}
		
		// check available
		if( available_div & div){
			break;
		}
		
		// Non Resize forcedly
		g_p2p_calc_in[sensor].none_resize = TRUE;
		g_p2p_calc_in[sensor].v_trim_flg = TRUE;
		
		// check violate
		if( available_div & (D_PIPE_CALC_ERR_H_IN_VIOLATE_DIV(div)) ){
			if( g_p2p_calc_in[sensor].sensor_h_effective_size > adjust ){
				g_p2p_calc_in[sensor].sensor_h_effective_size -= adjust;
			}else{
				return D_STM_NG;
			}
		}
		
		// calc again
		available_div = P2P_LDIV_PIPE_Calc(&g_p2p_calc_in[sensor], &g_p2p_calc_out[sensor]);
	}
	
	UPRINTF("  New Sensor Effective %d x %d\n", g_p2p_calc_in[sensor].sensor_h_effective_size, g_p2p_calc_in[sensor].sensor_v_effective_size);
	return D_STM_OK;
}


static void calc_af( ULLONG frame_no, const T_SHARE_AF_PARAM* in )
{
	INT32 i;
	INT32 win;
	T_MODE_VIEW_ESTIMATE_AF_WINDOW_IN		af_window_in;
	T_MODE_VIEW_ESTIMATE_AF_WINDOW_OUT		af_window_out;
	
	for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
		if( g_s2y_in.sensor_use & (1<<i) ){
			
			af_window_in.sensor_id = i;
			af_window_in.sensor_mode = g_s2y_in.sensor_mode[ i ];
			// same calc as g_share_pipe_yuv
			af_window_in.sensor_ref_h_ofst = g_p2p_calc_out[i].ldiv[E_SHARE_H_DIV_1-1].out_h_ofst[E_SHARE_H_DIV_1-1] + g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].b2r_ofst;
			af_window_in.sensor_ref_h_ofst = g_p2p_calc_out[i].ldiv[E_SHARE_H_DIV_1-1].out_h_ofst[E_SHARE_H_DIV_1-1] + g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].b2r_ofst;
			af_window_in.sensor_ref_v_ofst = g_p2p_calc_out[i].pipe_v.b2r_ofst;
			af_window_in.sensor_ref_h_size = g_p2p_calc_out[i].pipe_h[E_PIPE_DIV_PART_1].b2r_size;
			af_window_in.sensor_ref_v_size = g_p2p_calc_out[i].pipe_v.b2r_size;
			af_window_in.pipe_yuv_h_size   = g_s2y_in.one_sensor_yuv_h_size[i];
			af_window_in.pipe_yuv_v_size   = g_s2y_in.one_sensor_yuv_v_size[i];
			
			for( win = E_SHARE_AF_WINDOW_0; win < E_SHARE_AF_WINDOW_MAX; win ++ ){
				
				af_window_in.view_h_ofst_ratio = in->sensor[i].window[win].h_ofst_ratio;
				af_window_in.view_v_ofst_ratio = in->sensor[i].window[win].v_ofst_ratio;
				af_window_in.view_h_size_ratio = in->sensor[i].window[win].h_size_ratio;
				af_window_in.view_v_size_ratio = in->sensor[i].window[win].v_size_ratio;
				
				g_p2p_af_calc_in[i].window[win].enable = in->sensor[i].window[win].enable;
				if( !g_p2p_af_calc_in[i].window[win].enable ){
					continue;
				}
				
				Mode_View_Estimate_AF_Window( frame_no, &af_window_in, &af_window_out );
				
				g_p2p_af_calc_in[i].window[win].h_ofst = af_window_out.sensor_h_ofst;
				g_p2p_af_calc_in[i].window[win].v_ofst = af_window_out.sensor_v_ofst;
				g_p2p_af_calc_in[i].window[win].h_size = af_window_out.sensor_h_size;
				g_p2p_af_calc_in[i].window[win].v_size = af_window_out.sensor_v_size;
				
				// for copy to out
				g_p2p_af_calc_in[i].window[win].h_sel0				= in->sensor[i].window[win].h_sel0;
				g_p2p_af_calc_in[i].window[win].h_sel1				= in->sensor[i].window[win].h_sel1;
				g_p2p_af_calc_in[i].window[win].h_sel2				= in->sensor[i].window[win].h_sel2;
				g_p2p_af_calc_in[i].window[win].eval_cal_unit		= in->sensor[i].window[win].eval_cal_unit;
				g_p2p_af_calc_in[i].window[win].bright_threshold	= in->sensor[i].window[win].bright_threshold;
				g_p2p_af_calc_in[i].window[win].dark_threshold		= in->sensor[i].window[win].dark_threshold;
				g_p2p_af_calc_in[i].window[win].max_mix_select		= in->sensor[i].window[win].max_mix_select;
			}
			
			g_p2p_af_calc_in[i].filter = in->sensor[i].filter;
		}
	}
	P2P_AF_Calc( &g_p2p_af_calc_in[A], &g_p2p_calc_out[A], &g_p2p_af_calc_out[A]);
	if( g_s2y_in.sensor_num >= E_SHARE_SENSOR_NUM_2 ){
		P2P_AF_Calc( &g_p2p_af_calc_in[B], &g_p2p_calc_out[B], &g_p2p_af_calc_out[B]);
	}
	if( g_s2y_in.sensor_num >= E_SHARE_SENSOR_NUM_4 ){
		P2P_AF_Calc( &g_p2p_af_calc_in[C], &g_p2p_calc_out[C], &g_p2p_af_calc_out[C]);
		P2P_AF_Calc( &g_p2p_af_calc_in[D], &g_p2p_calc_out[D], &g_p2p_af_calc_out[D]);
	}
}


static void send_af_data( void )
{
	INT32 sensor_id, pipe_id, win, dat;
	
	// Clear first
	memset( &g_share_af_data.sensor, 0, sizeof( g_share_af_data.sensor ) );
	
	for( pipe_id = 0; pipe_id < g_s2y_mng.pipe_use_num; pipe_id++ ){
		if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx2_DIV2) {
			switch( pipe_id ){
			case 0:		sensor_id = A;	break;
			case 1:		sensor_id = A;	break;
			case 2:		sensor_id = B;	break;
			case 3:		sensor_id = B;	break;
			default:	sensor_id = A;	break;
			}
		}else{
			switch( pipe_id ){
			case 0:		sensor_id = A;	break;
			case 1:		sensor_id = B;	break;
			case 2:		sensor_id = C;	break;
			case 3:		sensor_id = D;	break;
			default:	sensor_id = A;	break;
			}
		}
		for( win = E_SHARE_AF_WINDOW_0; win < E_SHARE_AF_WINDOW_MAX; win ++ ){
		
			if( g_s2y_mng.af_h_rcv[pipe_id].window[win].enable ){
				g_share_af_data.sensor[sensor_id].window[win].enable = TRUE;
			
				for( dat = 0; dat < E_SHARE_AF_DATA_MAX; dat ++ ){
					g_share_af_data.sensor[sensor_id].window[win].data64[dat] += g_s2y_mng.af_data[pipe_id].window[win].data[dat];
					if( g_share_af_data.sensor[sensor_id].window[win].data64[dat] > 0xFFFFFFFF ){
						g_share_af_data.sensor[sensor_id].window[win].data[dat] = 0xFFFFFFFF;
					}else{
						g_share_af_data.sensor[sensor_id].window[win].data[dat] = g_share_af_data.sensor[sensor_id].window[win].data64[dat];
					}
				}
			
			}
		}
	}
	
	Set_Frame_Share_AF_Data(&g_share_af_data, g_af_cnt);
	Send_Share_User_Event(E_SHARE_USER_EVENT_AF_STAT_END, g_af_cnt++);
}

static void make_dualpd_data( E_PIPE_MODE mode )
{

	switch(mode){
		case E_PIPE_MODE_SENSORx1_DIRECT_DIV2:
			g_share_af_data.pdaf.IMAGE_DATA.g_width = g_s2y_mng.ldiv_pwch[0].byte;
			g_share_af_data.pdaf.IMAGE_DATA.width = g_s2y_mng.sen.ldiv[A]->out_h_size[1] + g_s2y_mng.sen.ldiv[A]->out_h_ofst[1] - g_s2y_mng.sen.ldiv[A]->out_h_ofst[0];
			g_share_af_data.pdaf.IMAGE_DATA.height = g_s2y_mng.ldiv_pwch[0].v->size;
			g_share_af_data.pdaf.DUAL_PD_DATA.g_width = g_s2y_mng.ldiv_pwch[2].byte;
			g_share_af_data.pdaf.DUAL_PD_DATA.width = g_s2y_mng.sen.ldiv[B]->out_h_size[1] + g_s2y_mng.sen.ldiv[B]->out_h_ofst[1] - g_s2y_mng.sen.ldiv[B]->out_h_ofst[0];
			g_share_af_data.pdaf.DUAL_PD_DATA.height = g_s2y_mng.ldiv_pwch[2].v->size;
			g_share_af_data.pdaf.DUAL_PD_DATA.addr = g_s2y_mng.ldiv_pwch[2].addr + g_s2y_mng.ldiv_pwch[2].h->addr;
		break;
		default:
		break;
	}

	Set_Frame_Share_AF_Data(&g_share_af_data, g_af_cnt);

	if(( g_share_af_data.af_en & 0x01 ) == 0x00){
		Send_Share_User_Event(E_SHARE_USER_EVENT_AF_STAT_END, g_af_cnt++);
	}

}

static void make_pd_data( const T_SHARE_RAW* raw )
{

	g_share_af_data.pdaf.PD_DATA.addr = raw->rect[0].addr + (raw->rect[0].v_ofs + g_share_af_data.pdaf.IMAGE_DATA.height) * raw->rect[0].h_byte;

	Set_Frame_Share_AF_Data(&g_share_af_data, g_af_cnt);

	if(( g_share_af_data.af_en & 0x01 ) == 0x00){
		Send_Share_User_Event(E_SHARE_USER_EVENT_AF_STAT_END, g_af_cnt++);
	}
}

static void iq_dbg2_get_prefix( char* str, INT32 rect )
{
	INT32	sensor = 0;
	
	if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx2_DIV2 ){
		switch( rect ){
		case 0:		sensor = A;		break;
		case 1:		sensor = A;		break;
		case 2:		sensor = B;		break;
		case 3:		sensor = B;		break;
		default:	sensor = A;		break;
		}
	}else{
		switch( rect ){
		case 0:		sensor = A;		break;
		case 1:		sensor = B;		break;
		case 2:		sensor = C;		break;
		case 3:		sensor = D;		break;
		default:	sensor = A;		break;
		}
	}
	// length < 16
	if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL ){
		if( rect < 1 ){ // Long
			sprintf( str, "S%dD%dR%dDL_", A, g_s2y_mng.div_num, rect ); // len < 16
		}else{			// Short
			sprintf( str, "S%dD%dR%dDS_", A, g_s2y_mng.div_num, rect ); // len < 16
		}
	}
	else if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 ){
		if( rect < 2 ){ // Long
			sprintf( str, "S%dD%dR%dDL_", A, g_s2y_mng.div_num, rect ); // len < 16
		}else{			// Short
			sprintf( str, "S%dD%dR%dDS_", A, g_s2y_mng.div_num, rect ); // len < 16
		}
	}
	else{
		sprintf( str, "S%dD%dR%d_", sensor, g_s2y_mng.div_num, rect ); // len < 16
	}
}

static void iq_dbg2_assign( UINT32 flg )
{
	CHAR	str[32] = {0};
	CHAR	pre[16] = {0};
	INT32	pipe = 0;
	INT32	rect;
	
	flg = flg & g_s2y_in.iq_save_reg_en;
	
	if( flg == 0 ){
		return;
	}
	
	// SEN, SEN_TBL
	if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_SEN ) ){
		IQ_Dbg2_Assign_Data("SEN", E_IQ_DBG2_DATA_TYPE_SEN, 0 );
	}
	if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_SEN_TBL ) ){
		IQ_Dbg2_Assign_Data("SEN_TBL", E_IQ_DBG2_DATA_TYPE_SEN_TBL, 0 );
	}
	
	// PIPE
	for( rect = 0; rect < g_s2y_mng.pipe_use_num; rect ++ ){
		
		iq_dbg2_get_prefix( pre, rect );
		pipe = rect&1;
		
		// REG
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_SRO ) ){
			sprintf( str, "%s%s", pre, "SRO" );
			IQ_Dbg2_Assign_Data( str, E_IQ_DBG2_DATA_TYPE_SRO, pipe );
		}
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_B2B ) ){
			sprintf( str, "%s%s", pre, "B2B" );
			IQ_Dbg2_Assign_Data( str, E_IQ_DBG2_DATA_TYPE_B2B, pipe );
		}
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_B2R ) ){
			sprintf( str, "%s%s", pre, "B2R" );
			IQ_Dbg2_Assign_Data( str, E_IQ_DBG2_DATA_TYPE_B2R, pipe );
		}
		if( g_s2y_in.ltm_mode ){
			if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_LTM ) ){
				sprintf( str, "%s%s", pre, "LTM" );
				IQ_Dbg2_Assign_Data( str, E_IQ_DBG2_DATA_TYPE_LTM, pipe );
			}
		}
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_R2Y ) ){
			sprintf( str, "%s%s", pre, "R2Y" );
			IQ_Dbg2_Assign_Data( str, E_IQ_DBG2_DATA_TYPE_R2Y, pipe );
		}
		if( g_s2y_mng.cnr_type ){
			if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_CNR ) ){
				sprintf( str, "%s%s", pre, "CNR" );
				IQ_Dbg2_Assign_Data( str, E_IQ_DBG2_DATA_TYPE_CNR, pipe );
			}
		}
		// TBL
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_SRO_TBL ) ){
			sprintf( str, "%s%s", pre, "SRO_TBL" );
			IQ_Dbg2_Assign_Data( str, E_IQ_DBG2_DATA_TYPE_SRO_TBL, pipe );
		}
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_B2B_TBL ) ){
			sprintf( str, "%s%s", pre, "B2B_TBL" );
			IQ_Dbg2_Assign_Data( str, E_IQ_DBG2_DATA_TYPE_B2B_TBL, pipe );
		}
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_B2R_TBL ) ){
			sprintf( str, "%s%s", pre, "B2R_TBL" );
			IQ_Dbg2_Assign_Data( str, E_IQ_DBG2_DATA_TYPE_B2R_TBL, pipe );
		}
		if( g_s2y_in.ltm_mode ){
			if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_LTM_TBL ) ){
				sprintf( str, "%s%s", pre, "LTM_TBL" );
				IQ_Dbg2_Assign_Data( str, E_IQ_DBG2_DATA_TYPE_LTM_TBL, pipe );
			}
		}
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_R2Y_TBL ) ){
			sprintf( str, "%s%s", pre, "R2Y_TBL" );
			IQ_Dbg2_Assign_Data( str, E_IQ_DBG2_DATA_TYPE_R2Y_TBL, pipe );
		}
		// CNR has no tbl
	}
}


static void iq_dbg2_set( UINT32 flg )
{
	CHAR	str[32] = {0};
	CHAR	pre[16] = {0};
	INT32	pipe = 0;
	INT32	rect = 0;
	INT32	rect_num = 0;
	ULLONG	r2y_frame_no = 0;
	ULLONG	req_frame_no = Get_Share_IQ_DBG2_Save_Reg_Frame(); 
	
	flg = flg & g_s2y_in.iq_save_reg_en;
	
	if( flg == 0 ){
		return;
	}
	
	if( req_frame_no == -1 ){
		return;
	}
	
	r2y_frame_no = g_yuv_cnt;
	
	if( g_s2y_mng.is_direct ){
		if( req_frame_no >= D_ISP_REFLECT_FRAME ){
			req_frame_no -= D_ISP_REFLECT_FRAME;
		}
	}
	
	if( req_frame_no != r2y_frame_no ){
		return;
	}
	
	if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ){
		rect = 0;
		rect_num = 1;
	}else if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_2 ){
		rect = 0;
		rect_num = 2;
	}else if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_4 ){
		if( g_s2y_mng.pipes_2nd_time == 0 ){
			rect = 0;
			rect_num = 2;
		}else{
			rect = 2;
			rect_num = 4;
		}
	}
	
	// SEN, SEN_TBL
	if( rect_num < 4 ){ // not get 2nd pipes
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_SEN ) ){
			IQ_Dbg2_Set_Data("SEN", E_IQ_DBG2_DATA_TYPE_SEN, 0 );
		}
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_SEN_TBL ) ){
			IQ_Dbg2_Set_Data("SEN_TBL", E_IQ_DBG2_DATA_TYPE_SEN_TBL, 0 );
		}
	}
	
	// PIPE
	for( rect = 0; rect < rect_num; rect ++ ){
		
		iq_dbg2_get_prefix( pre, rect );
		pipe = rect&1;
		
		// REG
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_SRO ) ){
			sprintf( str, "%s%s", pre, "SRO" );
			IQ_Dbg2_Set_Data( str, E_IQ_DBG2_DATA_TYPE_SRO, pipe );
		}
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_B2B ) ){
			sprintf( str, "%s%s", pre, "B2B" );
			IQ_Dbg2_Set_Data( str, E_IQ_DBG2_DATA_TYPE_B2B, pipe );
		}
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_B2R ) ){
			sprintf( str, "%s%s", pre, "B2R" );
			IQ_Dbg2_Set_Data( str, E_IQ_DBG2_DATA_TYPE_B2R, pipe );
		}
		if( g_s2y_in.ltm_mode ){
			if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_LTM ) ){
				sprintf( str, "%s%s", pre, "LTM" );
				IQ_Dbg2_Set_Data( str, E_IQ_DBG2_DATA_TYPE_LTM, pipe );
			}
		}
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_R2Y ) ){
			sprintf( str, "%s%s", pre, "R2Y" );
			IQ_Dbg2_Set_Data( str, E_IQ_DBG2_DATA_TYPE_R2Y, pipe );
		}
		if( g_s2y_mng.cnr_type ){
			if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_CNR ) ){
				sprintf( str, "%s%s", pre, "CNR" );
				IQ_Dbg2_Set_Data( str, E_IQ_DBG2_DATA_TYPE_CNR, pipe );
			}
		}
		// TBL
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_SRO_TBL ) ){
			sprintf( str, "%s%s", pre, "SRO_TBL" );
			IQ_Dbg2_Set_Data( str, E_IQ_DBG2_DATA_TYPE_SRO_TBL, pipe );
		}
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_B2B_TBL ) ){
			sprintf( str, "%s%s", pre, "B2B_TBL" );
			IQ_Dbg2_Set_Data( str, E_IQ_DBG2_DATA_TYPE_B2B_TBL, pipe );
		}
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_B2R_TBL ) ){
			sprintf( str, "%s%s", pre, "B2R_TBL" );
			IQ_Dbg2_Set_Data( str, E_IQ_DBG2_DATA_TYPE_B2R_TBL, pipe );
		}
		if( g_s2y_in.ltm_mode ){
			if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_LTM_TBL ) ){
				sprintf( str, "%s%s", pre, "LTM_TBL" );
				IQ_Dbg2_Set_Data( str, E_IQ_DBG2_DATA_TYPE_LTM_TBL, pipe );
			}
		}
		if( flg & IQ_DBG2_CONV_FLG( E_IQ_DBG2_DATA_TYPE_R2Y_TBL ) ){
			sprintf( str, "%s%s", pre, "R2Y_TBL" );
			IQ_Dbg2_Set_Data( str, E_IQ_DBG2_DATA_TYPE_R2Y_TBL, pipe );
		}
	}
}

static void b2r_r2y_map_proc(ULLONG frame_no)
{
	INT32 i;

	if( (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_4) ){
		g_s2y_mng.pipes_2nd_time = 0;
	}
	// AWB(SHD)
	Get_Frame_Share_AWB_Param( &g_s2y_mng.awb_param_reg, frame_no );
	COPY_AWB_PARAM(g_s2y_mng.awb_param_reg);
	// IQ
	Get_Frame_Share_Sensor_Exp( &g_s2y_mng.exp, frame_no ); 
	for( i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++ ){
		if( g_s2y_in.sensor_use & (1<<i) ){
			g_s2y_mng.iq_gain_times[i] =  g_s2y_mng.exp.sensor[i].exp.again_times
										* g_s2y_mng.exp.sensor[i].exp.dgain_times
										* g_s2y_mng.isp_gain_reg.sensor[i].b2b_end[E_PGAIN_GR]; // may not need
			if((g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL) || (g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2))
				g_s2y_mng.iq_scene[i] = g_s2y_in.scene_hdr_1st_run;
			else
				g_s2y_mng.iq_scene[i] = g_s2y_in.scene_normal;
		}
	}
	IQPara_Scene_Stream_Table( g_s2y_mng.iq_scene, g_s2y_in.sensor_num );
	IQPara_UpdateGainArray( g_s2y_mng.iq_gain_times, 0.0 );
	iq_update_b2r_to_cnr( TRUE, g_s2y_mng.iq_stream[0], g_s2y_mng.iq_stream[1] );
	// Start B2RR2Y
	for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ ) {
		g_s2y_mng.pipe[i].addr_y = g_s2y_mng.mem_addr_y[ g_s2y_mng.mem_bank_y[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_y[i];
		g_s2y_mng.pipe[i].addr_c = g_s2y_mng.mem_addr_c[ g_s2y_mng.mem_bank_c[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_c[i];
	}
	
	P2P_PIPE_B2RR2Y_Start(g_s2y_mng.cnr_type,
						  g_s2y_mng.is_r2y_out_ipu,
						  g_s2y_mng.shd_en,
						  g_s2y_mng.cag_en,
						  &g_s2y_mng.pipe[0],
						  &g_s2y_mng.pipe[1],
						  &g_s2y_mng.sro_pg[0],
						  &g_s2y_mng.sro_pg[1]);
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_B2R, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "B2RR2Y"));
}

static void ltm_rgb_map_process(ULLONG frame_no)
{
	int i;

	if(  (g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT && (g_s2y_mng.div_num == 2 || g_s2y_mng.div_num == 1) && g_s2y_mng.pipes_2nd_time == 0 && g_s2y_mng.ltm_rgb_rdy) ||
		 (g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && (g_s2y_mng.div_num == 2 || g_s2y_mng.div_num == 1) && (g_s2y_mng.ltmrgb_cnt & 1) && g_s2y_mng.ltm_rgb_rdy)) {
		// Sensorx2 Div2, sensorx4 div1, 1st time pipe process
		BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_map_rgb, g_s2y_mng.mem_bank_map_rgb_num, g_s2y_mng.mem_addr_map_rgb, 0xFFFFFFFF);
		BANK_ING_TO_RCV(g_s2y_mng.mem_bank_map_rgb);
		for( i = 0; i < g_s2y_mng.pipe_use_num; i++ ) {
			if( g_s2y_mng.pipe[i].addr_maprgb_recv ) {
				g_s2y_mng.pipe[i].addr_maprgb_recv = g_s2y_mng.mem_addr_map_rgb[g_s2y_mng.mem_bank_map_rgb[E_MEM_BANK_RCV]] + g_s2y_mng.addr_ofst_maprgb[i];
			}
		}

		P2P_PIPE_LTMMAP_Start(&g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1]);
		g_s2y_mng.ltm_rgb_rdy = FALSE;
		g_s2y_mng.ltmmap_2nd_time = 0;

	} else if ( (g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT && (g_s2y_mng.div_num == 2  || g_s2y_mng.div_num == 1) && g_s2y_mng.pipes_2nd_time == 1 && g_s2y_mng.ltm_rgb_rdy) ||
				(g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && (g_s2y_mng.div_num == 2  || g_s2y_mng.div_num == 1) && ((g_s2y_mng.ltmrgb_cnt & 1) == 0) && g_s2y_mng.ltm_rgb_rdy)) {
		// Sensorx2 Div2, sensorx4 div1, 2nd time pipe process
		for( i = 0; i < g_s2y_mng.pipe_use_num; i++ ) {
			if (g_s2y_mng.pipe[i].addr_maprgb) {
				g_s2y_mng.pipe[i].addr_maprgb = g_s2y_mng.mem_addr_map_rgb[ g_s2y_mng.mem_bank_map_rgb[E_MEM_BANK_SET] ] + g_s2y_mng.addr_ofst_maprgb[i];
			}
		}

		P2P_PIPE_LTMMAP_Start(&g_s2y_mng.pipe[2], &g_s2y_mng.pipe[3]);
		g_s2y_mng.ltm_rgb_rdy = FALSE;
		g_s2y_mng.ltmmap_2nd_time = 1;

	} else if ( (g_s2y_in.ltm_mode != E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.div_num == 4 && g_s2y_mng.pipes_2nd_time == 1 && g_s2y_mng.ltm_rgb_rdy ) ||
				(g_s2y_in.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && g_s2y_mng.div_num == 4 && ((g_s2y_mng.ltmrgb_cnt & 1) == 0) && g_s2y_mng.ltm_rgb_rdy )) {
		// Sensorx1 Div4
		BANK_SET_TO_ING_AND_INCREMENT(g_s2y_mng.mem_bank_map_rgb, g_s2y_mng.mem_bank_map_rgb_num, g_s2y_mng.mem_addr_map_rgb, 0xFFFFFFFF);
		for( i = 0; i < g_s2y_mng.pipe_use_num; i++ ) {
			if( g_s2y_mng.pipe[i].addr_maprgb ) {
				g_s2y_mng.pipe[i].addr_maprgb = g_s2y_mng.mem_addr_map_rgb[g_s2y_mng.mem_bank_map_rgb[E_MEM_BANK_SET]] + g_s2y_mng.addr_ofst_maprgb[i];
			}
		}
		BANK_ING_TO_RCV(g_s2y_mng.mem_bank_map_rgb);
		for( i = 0; i < g_s2y_mng.pipe_use_num; i++ ) {
			 if( g_s2y_mng.pipe[i].addr_maprgb_recv ) {
				 g_s2y_mng.pipe[i].addr_maprgb_recv = g_s2y_mng.mem_addr_map_rgb[g_s2y_mng.mem_bank_map_rgb[E_MEM_BANK_RCV]] + g_s2y_mng.addr_ofst_maprgb[i];
			 }
		}
		P2P_PIPE_LTMMAP_Start(&g_s2y_mng.pipe[0], NULL);
		g_s2y_mng.ltm_rgb_rdy = FALSE;
	}
}

	
static void set_sdc( void )
{
	T_BF_IQ_DPC_MULTIPIPE_CALC_PARAM	calc_param;
	T_BF_IQ_DPC_MULTIPIPE_TBL*			multipipe_tbl;
	
	E_IQ_CALIB_TGT		e_calib_tgt = E_IQ_CALIB_TGT_0;
	UINT32				tbl_idx = 0;
	INT32				ercd, bret;
	INT32				x, evenodd, sensor_id, i;
	INT32				y = 0;		// fix
	INT32				dpc_id = 0;	// fix
	INT32				pipe_id;
	
	// Each Sensor
	for( i = 0; i < g_s2y_in.sensor_num; i ++ ){
		
		// Set Sensor and Pipe
		switch( i ){
		case 0:
			sensor_id = A;
			pipe_id = 0;
			break;
		case 1:
			sensor_id = B;
			pipe_id = ( g_s2y_mng.div_num == 1 ) ? 1 : 2;
			break;
		case 2:
			sensor_id = C;
			pipe_id = 2;
			break;
		case 3:
			sensor_id = D;
			pipe_id = 3;
			break;
		default:
			sensor_id = A;
			pipe_id = 0;
			break;
		}
		
		tbl_idx = sensor_id; // tbl_idx < D_BF_IQ_DPC_TBL_IDX_MAX (5)
		
		// Load
		bret = fj_iq_calibrationloadbadpixtable_sdram( e_calib_tgt, tbl_idx, (CHAR*)g_s2y_in.sdc_tbl_addr[ sensor_id ] );
		if( bret == FALSE ){
			UPRINTF_ERR(1);
			g_s2y_mng.pipe[0].sdc_en = g_s2y_mng.pipe[1].sdc_en = g_s2y_mng.pipe[2].sdc_en = g_s2y_mng.pipe[3].sdc_en = 0;
			return;
		}
		
#if 0	// Debug Table Info
		{
			T_BF_IQ_DPC_TBL_INFO*				tbl_info;
			tbl_info = BF_IQ_Dpc_Tbl_Get_Info( e_calib_tgt, tbl_idx );
			printf( "tbl_info->pixel_num = %d\n", (INT32)tbl_info->pixel_num );
			printf( "tbl_info->sta_x     = %d\n", (INT32)tbl_info->sta_x );
			printf( "tbl_info->sta_y     = %d\n", (INT32)tbl_info->sta_y );
			printf( "tbl_info->width     = %d\n", (INT32)tbl_info->width );
			printf( "tbl_info->lines     = %d\n", (INT32)tbl_info->lines );
		}
#endif
		
		// Select table
		ercd = BF_IQ_Dpc_Tbl_Sel_Set( 0, e_calib_tgt, tbl_idx );
		if( ercd ){
			UPRINTF_ERR( ercd );
			g_s2y_mng.pipe[0].sdc_en = g_s2y_mng.pipe[1].sdc_en = g_s2y_mng.pipe[2].sdc_en = g_s2y_mng.pipe[3].sdc_en = 0;
			return;
		}
		
		// Set calc_param
		y = 0;
		calc_param.x_num = g_s2y_mng.div_num;
		calc_param.y_num = 1;
		for( x = 0; x < calc_param.x_num; x ++ ){
			calc_param.crop_info[ x ][ y ].pos_x = g_p2p_calc_out[ sensor_id ].ldiv[ (g_s2y_mng.div_num-1) ].out_h_ofst[ x ];
			calc_param.crop_info[ x ][ y ].pos_y = g_p2p_calc_out[ sensor_id ].ldiv_pwch_v.ofst;
			calc_param.crop_info[ x ][ y ].width = g_p2p_calc_out[ sensor_id ].ldiv[ (g_s2y_mng.div_num-1) ].out_h_size[ x ];
			calc_param.crop_info[ x ][ y ].lines = g_p2p_calc_out[ sensor_id ].ldiv_pwch_v.size;
		}
		calc_param.x_decrease_crop_offset = TRUE;
		calc_param.y_decrease_crop_offset = FALSE;
		// Calc
		ercd = BF_IQ_Dpc_MultiPipe_Calc( dpc_id, &calc_param );
		if( ercd ){
			UPRINTF_ERR( ercd );
			g_s2y_mng.pipe[0].sdc_en = g_s2y_mng.pipe[1].sdc_en = g_s2y_mng.pipe[2].sdc_en = g_s2y_mng.pipe[3].sdc_en = 0;
			return;
		}
		
		// Get result
		multipipe_tbl = BF_IQ_Dpc_MultiPipeTblMng_Get_Info( dpc_id, TRUE );
		
		// Set to Pipe
		for( x = 0; x < calc_param.x_num; x ++ ){
#if 1		// Debug Table Info
			printf( "multipipe_tbl->x_num = %d\n", multipipe_tbl->x_num );
			printf( "multipipe_tbl->y_num = %d\n", multipipe_tbl->y_num );
			printf( "multipipe_tbl->tbl_info[%d][%d].addr_by_fields[0]    = 0x%x\n", x, y, (INT32)multipipe_tbl->tbl_info[ x ][ y ].addr_by_fields[0] );
			printf( "multipipe_tbl->tbl_info[%d][%d].addr_by_fields[1]    = 0x%x\n", x, y, (INT32)multipipe_tbl->tbl_info[ x ][ y ].addr_by_fields[1] );
			printf( "multipipe_tbl->tbl_info[%d][%d].bytelen_by_fields[0] = 0x%x\n", x, y, (INT32)multipipe_tbl->tbl_info[ x ][ y ].bytelen_by_fields[0] );
			printf( "multipipe_tbl->tbl_info[%d][%d].bytelen_by_fields[1] = 0x%x\n", x, y, (INT32)multipipe_tbl->tbl_info[ x ][ y ].bytelen_by_fields[1] );
			printf( "multipipe_tbl->tbl_info[%d][%d].pixel_num            = %d\n", x, y, multipipe_tbl->tbl_info[ x ][ y ].pixel_num );
			printf( "multipipe_tbl->tbl_info[%d][%d].sta_x                = %d\n", x, y, multipipe_tbl->tbl_info[ x ][ y ].sta_x );
			printf( "multipipe_tbl->tbl_info[%d][%d].sta_y                = %d\n", x, y, multipipe_tbl->tbl_info[ x ][ y ].sta_y );
			printf( "multipipe_tbl->tbl_info[%d][%d].width                = %d\n", x, y, multipipe_tbl->tbl_info[ x ][ y ].width );
			printf( "multipipe_tbl->tbl_info[%d][%d].lines                = %d\n", x, y, multipipe_tbl->tbl_info[ x ][ y ].lines );
#endif
			// Set
			for( evenodd = 0; evenodd < E_EVENODD_MAX; evenodd ++ ){
				g_s2y_mng.pipe[ pipe_id + x ].sdc_addr[ evenodd ]   = (ULONG)multipipe_tbl->tbl_info[ x ][ y ].addr_by_fields[ evenodd ];
				g_s2y_mng.pipe[ pipe_id + x ].sdc_h_size[ evenodd ] = (ULONG)multipipe_tbl->tbl_info[ x ][ y ].bytelen_by_fields[ evenodd ];
				g_s2y_mng.pipe[ pipe_id + x ].sdc_v_size[ evenodd ] = 1;
				g_s2y_mng.pipe[ pipe_id + x ].sdc_h_byte[ evenodd ] = ROUNDUP8( g_s2y_mng.pipe[ x ].sdc_h_size[ evenodd ] );
			}
		}
	}
	
#if 0 // Debug Save SDC Apply Table (only sensorA)
	{
		#include "fj_filesystem.h"
		CHAR  fpath[64];
		ULONG fid;
		ULONG fret_length;
		static INT32 debug_sdc_save_cnt = 0;
		
		pipe_id = 0; // sensor A
		for( x = 0; x < calc_param.x_num; x ++ ){
			for( evenodd = 0; evenodd < E_EVENODD_MAX; evenodd ++ ){
				sprintf( fpath, "I:\\%d_rect%d_sdc%d.bin", debug_sdc_save_cnt, x, evenodd );
				fid = FJ_Fs_Open( (CHAR*)fpath, FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC );
				FJ_Fs_Write( fid, 
							(UINT32*)g_s2y_mng.pipe[ pipe_id + x ].sdc_addr[ evenodd ],
							(ULONG  )g_s2y_mng.pipe[ pipe_id + x ].sdc_h_byte[ evenodd ],
							&fret_length);
				FJ_Fs_Close( fid );
			}
		}
		debug_sdc_save_cnt++;
	}
#endif
	
	// Copy for DOL
	if( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL_DIV2 ){
		for( pipe_id = 0; pipe_id < 2; pipe_id ++ ){
			for( evenodd = 0; evenodd < E_EVENODD_MAX; evenodd ++ ){
				g_s2y_mng.pipe[ pipe_id + 2 ].sdc_addr[ evenodd ]   = g_s2y_mng.pipe[ pipe_id ].sdc_addr[ evenodd ];
				g_s2y_mng.pipe[ pipe_id + 2 ].sdc_h_size[ evenodd ] = g_s2y_mng.pipe[ pipe_id ].sdc_h_size[ evenodd ];
				g_s2y_mng.pipe[ pipe_id + 2 ].sdc_v_size[ evenodd ] = g_s2y_mng.pipe[ pipe_id ].sdc_v_size[ evenodd ];
				g_s2y_mng.pipe[ pipe_id + 2 ].sdc_h_byte[ evenodd ] = g_s2y_mng.pipe[ pipe_id ].sdc_h_byte[ evenodd ];
			}
		}
	}else
	if ( g_s2y_mng.pipe_mode == E_PIPE_MODE_SENSORx1_DOL ){
		pipe_id = 0;
		for( evenodd = 0; evenodd < E_EVENODD_MAX; evenodd ++ ){
			g_s2y_mng.pipe[ pipe_id + 1 ].sdc_addr[ evenodd ]   = g_s2y_mng.pipe[ pipe_id ].sdc_addr[ evenodd ];
			g_s2y_mng.pipe[ pipe_id + 1 ].sdc_h_size[ evenodd ] = g_s2y_mng.pipe[ pipe_id ].sdc_h_size[ evenodd ];
			g_s2y_mng.pipe[ pipe_id + 1 ].sdc_v_size[ evenodd ] = g_s2y_mng.pipe[ pipe_id ].sdc_v_size[ evenodd ];
			g_s2y_mng.pipe[ pipe_id + 1 ].sdc_h_byte[ evenodd ] = g_s2y_mng.pipe[ pipe_id ].sdc_h_byte[ evenodd ];
		}
	}
	
	// Enable
	g_s2y_mng.pipe[0].sdc_en = g_s2y_mng.pipe[1].sdc_en = g_s2y_mng.pipe[2].sdc_en = g_s2y_mng.pipe[3].sdc_en = 1;
	
}


static void low_latency_r2y_line_config( void )
{
	USHORT level = 0;
	
	if( g_s2y_in.r2y_video_format && g_s2y_in.r2y_ask_user_to_get_video_addr ){
		g_s2y_mng_enc.r2y_def_h_byte = g_s2y_mng.pipe[0].h_byte;
		g_s2y_mng_enc.r2y_frame_no_set = 0;
		g_s2y_mng_enc.r2y_frame_no_ing = 0;
		g_s2y_mng_enc.r2y_frame_no_rcv = 0xFFFFFFFFFFFFFFFF;
		g_s2y_mng_enc.addr_param_set.dummy_flg = TRUE;
		g_s2y_mng_enc.addr_param_ing.dummy_flg = TRUE;
		g_s2y_mng_enc.addr_param_rcv.dummy_flg = TRUE;
		g_s2y_mng_enc.addr_param_set.g_width = g_s2y_mng_enc.r2y_def_h_byte;
		g_s2y_mng_enc.addr_param_ing.g_width = g_s2y_mng_enc.r2y_def_h_byte;
		g_s2y_mng_enc.addr_param_rcv.g_width = g_s2y_mng_enc.r2y_def_h_byte;
		
		low_latency_r2y_line_start(0);
		
		level = g_s2y_in.r2y_interrupt_line;
		if( level > 100 ){
			level = 100;
		}
		level = (g_s2y_in.one_sensor_yuv_v_size[A] * level ) / 100;
		level = ROUNDUP8( level );
		
		if( level < 16 ){
			level = 16; // after E_SYSTEM_EVENT_R2YLINE
		}
		if( level > g_s2y_in.one_sensor_yuv_v_size[A] ){
			level -= 8;
		}
		
		if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ){
			P2P_PIPE_Addr(g_s2y_mng.shd_en, g_s2y_mng.cag_en, &g_s2y_mng.pipe[0], NULL);
			P2P_PIPE_Set_Line1_Level( &g_s2y_mng.pipe[0], NULL, level);
		}else{
			P2P_PIPE_Addr(g_s2y_mng.shd_en, g_s2y_mng.cag_en, &g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1]);
			P2P_PIPE_Set_Line1_Level( &g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1], level);
		}
	}
}

static void low_latency_r2y_line_start( ULLONG frame_no )
{
	INT32 ret_enc;
	INT32 i;

	if( g_s2y_in.r2y_video_format && g_s2y_in.r2y_ask_user_to_get_video_addr ){
		// set->ing
		g_s2y_mng_enc.addr_param_ing = g_s2y_mng_enc.addr_param_set;
		g_s2y_mng_enc.r2y_frame_no_ing = g_s2y_mng_enc.r2y_frame_no_set;
		// set
		g_s2y_mng_enc.r2y_frame_no_set = frame_no;
		
		// get addr ( when dummy, g_width is set in the func
		ret_enc = g_s2y_in.r2y_ask_user_to_get_video_addr( &g_s2y_mng_enc.addr_param_set, 0, g_s2y_mng_enc.r2y_frame_no_set );
		if( ret_enc == NG ){
			g_s2y_mng_enc.addr_param_set.dummy_flg = TRUE;
		}
		
		if( g_s2y_mng_enc.addr_param_set.dummy_flg ){
			g_s2y_mng_enc.addr_param_set.g_width = g_s2y_mng_enc.r2y_def_h_byte;
		}else{
			for( i = 0; i < g_s2y_mng.pipe_use_num; i ++ ) {
				g_s2y_mng.pipe[i].addr_y = g_s2y_mng_enc.addr_param_set.y_addr;
				g_s2y_mng.pipe[i].addr_c = g_s2y_mng_enc.addr_param_set.c_addr;
			}
		}
		
		// update global
		if( g_s2y_mng.pipe_use_num == E_PIPE_USE_NUM_1 ){
			P2P_PIPE_Set_Global( &g_s2y_mng.pipe[0], NULL, g_s2y_mng_enc.addr_param_set.g_width );
		}else{
			P2P_PIPE_Set_Global( &g_s2y_mng.pipe[0], &g_s2y_mng.pipe[1], g_s2y_mng_enc.addr_param_set.g_width );
		}
	}
}

static void low_latency_r2y_line_end( ULLONG frame_no )
{
	INT32 ret_decide;
	
	if( g_s2y_in.r2y_video_format && g_s2y_in.r2y_ask_user_to_decide_video_addr ){
		// ing -> rcv
		g_s2y_mng_enc.addr_param_rcv = g_s2y_mng_enc.addr_param_ing;
		g_s2y_mng_enc.r2y_frame_no_rcv = g_s2y_mng_enc.r2y_frame_no_ing;
		
		if( !g_s2y_mng_enc.addr_param_rcv.dummy_flg ){
			ret_decide = g_s2y_in.r2y_ask_user_to_decide_video_addr( g_s2y_mng_enc.addr_param_rcv.dummy_flg, g_s2y_mng_enc.r2y_frame_no_rcv );
			UPRINTF_ERR(ret_decide);
			FJ_Record_Timer_Handler();
		}
	}
}

static void low_latency_r2y_share_pipe_yuv( void )
{
	if( g_s2y_in.r2y_video_format && g_s2y_in.r2y_ask_user_to_get_video_addr ){
		if( !g_s2y_mng_enc.addr_param_rcv.dummy_flg ){
			g_share_pipe_yuv.layout.addr_y = g_s2y_mng_enc.addr_param_rcv.y_addr;
			g_share_pipe_yuv.layout.addr_c = g_s2y_mng_enc.addr_param_rcv.c_addr;
			g_share_pipe_yuv.sensor[A].yuv.addr_y = g_s2y_mng_enc.addr_param_rcv.y_addr;
			g_share_pipe_yuv.sensor[A].yuv.addr_c = g_s2y_mng_enc.addr_param_rcv.c_addr;
		}
		g_share_pipe_yuv.layout.h_byte = g_s2y_mng_enc.addr_param_rcv.g_width;
		g_share_pipe_yuv.sensor[A].yuv.h_byte = g_s2y_mng_enc.addr_param_rcv.g_width;
	}
}

