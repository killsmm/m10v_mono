/**
 * @file		stm_ltm2cnr.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "mode_stm.h"
#include "ctrl_ltm.h"
#include "ctrl_pro2pipe.h"
#include "im_r2y.h"
#include "im_xch.h"
#include "iqpara_api.h"
#include "iq_bin_dbg.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#ifdef CO_MEMORY_1GB // can be "#if 1"
#define D_L2C_MEM_BANK_YC		(1)
#define D_L2C_MEM_BANK_RBK		(1)
#define D_L2C_MEM_BANK_MAP		(1)
#else
#define D_L2C_MEM_BANK_YC		(2)
#define D_L2C_MEM_BANK_RBK		(2)
#define D_L2C_MEM_BANK_MAP		(1)
#endif
#define D_L2C_INVALID_FRAME		( (ULLONG)0xFFFFFFFFFFFFFFFF )

#define D_LTM_FIX_POINT	(16384)
#define D_IQ_PIPE01				(2)
/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
typedef enum {
	E_LTM2CNR_STATE_WAIT_1st = 0,
	E_LTM2CNR_STATE_WAIT,
	E_LTM2CNR_STATE_WAIT_MAP,
	E_LTM2CNR_STATE_WAIT_R2Y,
	E_LTM2CNR_STATE_RUN,
	E_LTM2CNR_STATE_MAX,
} E_LTM2CNR_STATE;


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	UCHAR					pipe_num;
	
	ULONG					mem_addr_y[D_L2C_MEM_BANK_YC];
	ULONG					mem_addr_c[D_L2C_MEM_BANK_YC];
	ULONG					mem_addr_map_yp[D_L2C_MEM_BANK_MAP];
	ULONG					mem_addr_map_yn[D_L2C_MEM_BANK_MAP];
	ULONG					mem_addr_rbk[D_L2C_MEM_BANK_RBK];
	
	ULONG					mem_addr_debug_dol_y;
	ULONG					mem_addr_debug_dol_c;
	
	UCHAR					mem_bank_yc;
	UCHAR					mem_bank_rbk;
	UCHAR					mem_bank_map;
	
	T_P2P_PIPE  			pipe[2];	// Only Support Div2
	T_CTRL_LTM_LTM			ltm[2];		// Only Support Div2
	T_CTRL_LTM_RBK			rbk[2];		// Only Support Div2
	T_CTRL_LTM_MAP			map;
	
	E_LTM2CNR_STATE			state;

	UCHAR					iq_stream;
	UCHAR					iq_scene;
	FLOAT					iq_gain_times[3];
} T_LTM2CNR_MNG;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_STM_LTM2CNR_IN				g_l2c_in;
static T_P2P_LDIV_PIPE_CALC_IN		g_l2c_calc_in;
static T_P2P_LDIV_PIPE_CALC_OUT		g_l2c_calc_out;
static T_LTM2CNR_MNG				g_l2c_mng;
static ULLONG						g_l2c_yuv_cnt = 0;
static ULLONG						g_l2c_save_rbk_frame = D_L2C_INVALID_FRAME;
static ULLONG						g_l2c_save_map_frame = D_L2C_INVALID_FRAME;
static ULLONG						g_l2c_save_ltm_frame = D_L2C_INVALID_FRAME;


static T_SHARE_DOL_COMBINE		g_l2c_share_dol_combine;
static T_SHARE_PIPE_YUV			g_l2c_share_pipe_yuv;
static T_SHARE_YUV				g_l2c_debug_share_yuv;

static ULLONG	g_l2c_debug_dol_yuv_frame = 0;
static UINT32	g_l2c_debug_dol_xch_cnt = 0;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void start_ltm2cnr( ULLONG frame_no );
static void set_r2y_gamma( BOOL set_pipe0, BOOL set_pipe1 );
static void debug_dol( ULLONG frame_no, INT32 cnt );
static void debug_dol_xch_callback(void);
static void iq_update( BOOL is_tbl, UCHAR stream );

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
INT32 STM_LTM2CNR_Config( const T_STM_LTM2CNR_IN* in, T_STM_LTM2CNR_OUT* const out )
{
	UINT32		available_div = 0;
	UINT32		mem_addr, mem_size;
	INT32		i, offset;
	
	// Init Variable
	g_l2c_in = *in;
	memset( (VOID*)&g_l2c_calc_in, 0, sizeof(g_l2c_calc_in) );
	memset( (VOID*)&g_l2c_mng, 0, sizeof(g_l2c_mng) );
	memset( (VOID*)&g_l2c_share_pipe_yuv, 0, sizeof(g_l2c_share_pipe_yuv) );
	memset( (VOID*)&g_l2c_debug_share_yuv, 0, sizeof(g_l2c_debug_share_yuv) );
	g_l2c_yuv_cnt = 0;
	g_l2c_save_map_frame = D_L2C_INVALID_FRAME;

	// init state
	if ( in->frame_num == 1 ) {
		// There is no 2nd E_SYSTEM_EVENT_PIPE_DOL_YUV, cannot trigger LTM2CNR process, so force its state to following
		g_l2c_mng.state = E_LTM2CNR_STATE_WAIT_MAP;
	} else {
		g_l2c_mng.state = E_LTM2CNR_STATE_WAIT_1st;
	}

	// Calc Pipe
	g_l2c_calc_in.proclk_MHz = 300.0;
	g_l2c_calc_in.mergin_dpc_cag_elf	= 0;
	g_l2c_calc_in.pro_v_shd_ofst		= 0;
	g_l2c_calc_in.pro_v_lost_line		= 0;
	g_l2c_calc_in.mergin_ci_ee1			= 0;
	g_l2c_calc_in.mergin_ee2			= 2;
	switch( g_l2c_in.cnr_type ) {
	case E_CNR_OFF:		g_l2c_calc_in.mergin_cnr = 0;		break;
	case E_CNR_ON:		g_l2c_calc_in.mergin_cnr = 16;		break;
	case E_CNR_EXMODE:	g_l2c_calc_in.mergin_cnr = 36;		break;
	default:			g_l2c_calc_in.mergin_cnr = 0;		break;
	}
	
	g_l2c_calc_in.sensor_clk_Mpps			= 600;
	g_l2c_calc_in.sensor_h_cyc_min			= g_l2c_in.yuv_h_size;
	g_l2c_calc_in.sensor_h_out_total		= g_l2c_in.yuv_h_size;
	g_l2c_calc_in.sensor_h_ob_ofst			= 0;
	g_l2c_calc_in.sensor_h_ob_size			= 0;
	g_l2c_calc_in.sensor_h_effective_ofst	= 0;
	g_l2c_calc_in.sensor_h_effective_size	= g_l2c_in.yuv_h_size;
	
	g_l2c_calc_in.sensor_v_cyc_min			= g_l2c_in.yuv_v_size;
	g_l2c_calc_in.sensor_v_out_total		= g_l2c_in.yuv_v_size;
	g_l2c_calc_in.sensor_v_ob_ofst			= 0;
	g_l2c_calc_in.sensor_v_ob_size			= 0;
	g_l2c_calc_in.sensor_v_effective_ofst	= 0;
	g_l2c_calc_in.sensor_v_effective_size	= g_l2c_in.yuv_v_size;
	
	g_l2c_calc_in.sensor_h_effective_full	=  g_l2c_in.yuv_h_size;
	g_l2c_calc_in.sensor_v_effective_full	=  g_l2c_in.yuv_v_size;
	
	g_l2c_calc_in.sensor_h_effective_rsz_ratio		= 1.0;
	g_l2c_calc_in.sensor_h_effective_rsz_ratio_full	= 1.0;
	g_l2c_calc_in.sensor_v_effective_rsz_ratio		= 1.0;
	g_l2c_calc_in.sensor_v_effective_rsz_ratio_full	= 1.0;
	
	g_l2c_calc_in.pipe_h_out_size	= g_l2c_in.yuv_h_size;
	g_l2c_calc_in.pipe_v_out_size	= g_l2c_in.yuv_v_size;
	g_l2c_calc_in.none_resize		= TRUE;
	g_l2c_calc_in.v_trim_flg		= FALSE;
	g_l2c_calc_in.af_enable			= 0x00;
	g_l2c_calc_in.ltm_enable		= 0x01;
	g_l2c_calc_in.raw_wo_mergin		= 0x00;
	g_l2c_calc_in.raw_save_h_total	= 0x00;
	
	g_l2c_calc_in.aeawb_h_blk_num	= 32;
	g_l2c_calc_in.aeawb_v_blk_num	= 32;
	g_l2c_calc_in.aeyk[E_AEYK_R]	= 0x004C;
	g_l2c_calc_in.aeyk[E_AEYK_GR]	= 0x004A;
	g_l2c_calc_in.aeyk[E_AEYK_GB]	= 0x004A;
	g_l2c_calc_in.aeyk[E_AEYK_B]	= 0x001C;
	g_l2c_calc_in.aeawb_level_awb	= 0;
	g_l2c_calc_in.shd_tbl_h_byte    = 4;
	g_l2c_calc_in.shd_tbl_h_blk_num = 2;
	g_l2c_calc_in.shd_tbl_v_blk_num = 2;
	
	available_div = P2P_LDIV_PIPE_Calc( &g_l2c_calc_in, &g_l2c_calc_out );
	
	// Decide Pipe NUm
	if( available_div & E_SHARE_H_DIV_1 ){
		g_l2c_mng.pipe_num = 1;
		g_l2c_mng.pipe[0].v = &g_l2c_calc_out.pipe_v;
		g_l2c_mng.pipe[0].h = &g_l2c_calc_out.pipe_h[E_PIPE_DIV_PART_1];
		g_l2c_mng.pipe[0].h_byte = g_l2c_mng.pipe[0].h->r2y_h_byte;
		offset = E_PIPE_DIV_PART_1;
	}else if( available_div & E_SHARE_H_DIV_2 ){
		g_l2c_mng.pipe_num = 2;
		g_l2c_mng.pipe[0].v = &g_l2c_calc_out.pipe_v;
		g_l2c_mng.pipe[0].h = &g_l2c_calc_out.pipe_h[E_PIPE_DIV_PART_2L];
		g_l2c_mng.pipe[1].v = &g_l2c_calc_out.pipe_v;
		g_l2c_mng.pipe[1].h = &g_l2c_calc_out.pipe_h[E_PIPE_DIV_PART_2R];
		g_l2c_mng.pipe[0].h_byte = g_l2c_mng.pipe[0].h->r2y_h_byte;
		g_l2c_mng.pipe[1].h_byte = g_l2c_mng.pipe[0].h->r2y_h_byte;
		offset = E_PIPE_DIV_PART_2L;
	}else{
		UPRINTF_ERR(1);
		return D_STM_NG;
	}
	g_l2c_mng.pipe[0].format = g_l2c_in.yuv_format;
	g_l2c_mng.pipe[1].format = g_l2c_in.yuv_format;
	
	// P2P_LDIV_PIPE_Calc is calc for SRO-LTMRBK and 1/2 is done, but ltm2cnr use SDRAM-LTMRBK and 1/2 is not done, so add 1/2
	if( g_l2c_calc_out.pipe_v.ltmrbk_sta == 0 && g_l2c_calc_out.pipe_v.ltmrbk_pit == 0x4000 ){
		g_l2c_calc_out.pipe_v.ltmrbk_sta = 0x4000/2;
		g_l2c_calc_out.pipe_v.ltmrbk_pit = 0x4000*2;
	}else if( g_l2c_calc_out.pipe_v.ltmrbk_rdcen == 0 ){
		g_l2c_calc_out.pipe_v.ltmrbk_rdcen = 1;
		g_l2c_calc_out.pipe_v.ltmrbk_rdcmd = 0;
	}else if( g_l2c_calc_out.pipe_v.ltmrbk_rdcen == 1 && g_l2c_calc_out.pipe_v.ltmrbk_rdcmd < 2 ){
		g_l2c_calc_out.pipe_v.ltmrbk_rdcmd += 1;
	}else{
		UPRINTF_ERR(1);
	}
	// Set RBK
	for( i = 0; i < g_l2c_mng.pipe_num; i ++ ){
		g_l2c_mng.rbk[i].rgb_h_size = g_l2c_calc_out.pipe_h[ offset + i ].ltmrbk_size;
		g_l2c_mng.rbk[i].rgb_v_size = g_l2c_calc_out.pipe_v.r2y_y_trm_size;
		
		g_l2c_mng.rbk[i].rbk_h_byte = g_l2c_calc_out.pipe_h[ offset + i ].ltmrgb_h_byte;
		g_l2c_mng.rbk[i].rbk_hven	= g_l2c_calc_out.pipe_v.ltmrbk_hven;
		g_l2c_mng.rbk[i].rbk_sta	= g_l2c_calc_out.pipe_v.ltmrbk_sta;
		g_l2c_mng.rbk[i].rbk_pit	= g_l2c_calc_out.pipe_v.ltmrbk_pit;
		g_l2c_mng.rbk[i].rbk_rdcen	= g_l2c_calc_out.pipe_v.ltmrbk_rdcen;
		g_l2c_mng.rbk[i].rbk_rdcmd	= g_l2c_calc_out.pipe_v.ltmrbk_rdcmd;
		
		if( i == 1 ){
			g_l2c_mng.rbk[i].rgb_addr_ofst = g_l2c_calc_out.pipe_h[ offset ].ltmrbk_size * 2; // RGB16bit
			g_l2c_mng.rbk[i].rbk_addr_ofst = g_l2c_calc_out.pipe_h[ offset ].ltmrbk_size
											/ g_l2c_calc_out.pipe_v.ltm_ratio
											* 2 * 3; // RGB16bit Chunky
		}else{
			g_l2c_mng.rbk[i].rgb_addr_ofst = 0;
			g_l2c_mng.rbk[i].rbk_addr_ofst = 0;
		}
	}
	// Set MAP
	g_l2c_mng.map.rbk_h_byte = g_l2c_mng.rbk[0].rbk_h_byte;
	g_l2c_mng.map.map_h_byte = g_l2c_calc_out.pipe_h[E_SHARE_H_DIV_1].ltmmap_h_byte;
	g_l2c_mng.map.h_size = g_l2c_calc_out.pipe_h[E_SHARE_H_DIV_1].ltmmap_size;
	g_l2c_mng.map.v_size = g_l2c_calc_out.pipe_v.ltmmap_size;
	// Set LTM
	for( i = 0; i < g_l2c_mng.pipe_num; i ++ ){
		if( i == 1 ){
			// for div2 right rect
			g_l2c_mng.ltm[i].rgb_addr_ofst = ( g_l2c_calc_out.pipe_h[ offset + i ].ltmltm_sta - g_l2c_calc_out.pipe_h[ offset ].ltmltm_sta )
											/ g_l2c_calc_out.pipe_h[ offset + i ].ltmltm_pit
											* 2; // RGB16bit
		}else{
			g_l2c_mng.ltm[i].rgb_addr_ofst = 0;
		}
		g_l2c_mng.ltm[i].rgb_h_size = g_l2c_calc_out.pipe_h[ offset + i ].ltmltm_size;
		g_l2c_mng.ltm[i].rgb_v_size = g_l2c_calc_out.pipe_v.ltmltm_size;
		g_l2c_mng.ltm[i].map_h_byte = g_l2c_mng.map.map_h_byte;
		g_l2c_mng.ltm[i].map_h_size = g_l2c_mng.map.h_size;
		g_l2c_mng.ltm[i].map_v_size = g_l2c_mng.map.v_size;
		g_l2c_mng.ltm[i].map_h_sta  = g_l2c_calc_out.pipe_h[ offset + i ].ltmltm_sta;
		g_l2c_mng.ltm[i].map_h_pit  = g_l2c_calc_out.pipe_h[ offset + i ].ltmltm_pit;
		g_l2c_mng.ltm[i].map_v_sta  = g_l2c_calc_out.pipe_v.ltmltm_sta;
		g_l2c_mng.ltm[i].map_v_pit  = g_l2c_calc_out.pipe_v.ltmltm_pit;
	}
	// Calc Memory
	if( g_l2c_in.use_ch1 ){
		mem_addr = g_l2c_in.available_memory_addr_ch1;
	}else{
		mem_addr = g_l2c_in.available_memory_addr_ch0;
	}
	mem_size = g_l2c_mng.pipe[0].h_byte * g_l2c_mng.pipe[0].v->r2y_y_trm_size;
	for( i = 0; i < D_L2C_MEM_BANK_YC; i ++ ){
		g_l2c_mng.mem_addr_y[i] = mem_addr;
		mem_addr += mem_size;
	}
	if( g_l2c_in.yuv_format == E_SHARE_YUV_FORMAT_420_SEMI ){
		mem_size /= 2;
	}
	for( i = 0; i < D_L2C_MEM_BANK_YC; i ++ ){
		g_l2c_mng.mem_addr_c[i] = mem_addr;
		mem_addr += mem_size;
	}
	mem_size = g_l2c_mng.rbk[0].rbk_h_byte * g_l2c_calc_out.pipe_v.ltmmap_size;
	for( i = 0; i < D_L2C_MEM_BANK_RBK; i ++ ){
		g_l2c_mng.mem_addr_rbk[i] = mem_addr;
		mem_addr += mem_size;
	}
	mem_size = g_l2c_mng.map.map_h_byte * g_l2c_mng.map.v_size;
	for( i = 0; i < D_L2C_MEM_BANK_MAP; i ++ ){
		g_l2c_mng.mem_addr_map_yp[i] = mem_addr;
		mem_addr += mem_size;
		
		g_l2c_mng.mem_addr_map_yn[i] = mem_addr;
		mem_addr += mem_size;
	}
	// debug
	if( g_l2c_in.debug_dol_en ){
		mem_size = g_l2c_mng.pipe[0].h_byte * g_l2c_mng.pipe[0].v->r2y_y_trm_size;
		g_l2c_mng.mem_addr_debug_dol_y = mem_addr;
		g_l2c_mng.mem_addr_debug_dol_c = mem_addr + mem_size;
		mem_addr += mem_size*2;
	}
	
	// Set Value in PIPE
	g_l2c_mng.pipe[0].wb_cc.wb_gain[E_WB_R] = 0x100;
	g_l2c_mng.pipe[0].wb_cc.wb_gain[E_WB_G] = 0x100;
	g_l2c_mng.pipe[0].wb_cc.wb_gain[E_WB_B] = 0x100;
	g_l2c_mng.pipe[0].wb_cc.cc_matrix[E_CC_00] = 0x200;
	g_l2c_mng.pipe[0].wb_cc.cc_matrix[E_CC_11] = 0x200;
	g_l2c_mng.pipe[0].wb_cc.cc_matrix[E_CC_22] = 0x200;
	g_l2c_mng.pipe[1].wb_cc = g_l2c_mng.pipe[0].wb_cc;
	
	// Set Share
	g_l2c_share_pipe_yuv.layout.format = g_l2c_in.yuv_format;
	g_l2c_share_pipe_yuv.layout.video = E_SHARE_YUV_VIDEO_NONE;
	g_l2c_share_pipe_yuv.layout.h_byte = g_l2c_mng.pipe[0].h_byte;
	g_l2c_share_pipe_yuv.layout.h_size = g_l2c_in.yuv_h_size;
	g_l2c_share_pipe_yuv.layout.v_size = g_l2c_in.yuv_v_size;
	
	// Output
	if( g_l2c_in.use_ch1 ){
		out->available_memory_addr_ch0 = g_l2c_in.available_memory_addr_ch0;
		out->available_memory_addr_ch1 = mem_addr;
	}else{
		out->available_memory_addr_ch0 = mem_addr;
		out->available_memory_addr_ch1 = g_l2c_in.available_memory_addr_ch1;
	}

	// HW
	Init_LTM_All();
	
	// Set Default IQ
	if( g_l2c_mng.pipe_num == 1 ){
		Set_LTM_MAP_IQ_Basic( TRUE, FALSE );
		Set_LTM_LTM_IQ_Basic( TRUE, FALSE );
	}else{
		Set_LTM_MAP_IQ_Basic( TRUE, TRUE );
		Set_LTM_LTM_IQ_Basic( TRUE, TRUE );
	}
	
	return D_STM_OK;
}


INT32 STM_LTM2CNR_Start( void)
{
	return D_STM_IDLE;
}


INT32 STM_LTM2CNR_Event( E_SYSTEM_EVENT event, ULLONG frame_no )
{
	INT32 i;
	
	switch( event )
	{
	case E_SYSTEM_EVENT_DOL_COMBINE:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_LTM_MAP, FJ_MBALOG_FUNC_ID_LTM_LTM_INT, FJ_MBALOG_TYPE_START, "LTM RBK %d", (INT32)frame_no));
		g_l2c_save_rbk_frame = frame_no;
		Get_Frame_Share_Dol_Combine( &g_l2c_share_dol_combine, frame_no );
		// RBK
		for( i = 0; i < g_l2c_mng.pipe_num; i ++ ){
			g_l2c_mng.rbk[i].rgb_h_byte = g_l2c_share_dol_combine.dol_rgb.rgb_h_byte;
			g_l2c_mng.rbk[i].rgb_addr_r = g_l2c_share_dol_combine.dol_rgb.mem_addr_r;
			g_l2c_mng.rbk[i].rgb_addr_g = g_l2c_share_dol_combine.dol_rgb.mem_addr_g;
			g_l2c_mng.rbk[i].rgb_addr_b = g_l2c_share_dol_combine.dol_rgb.mem_addr_b;
			g_l2c_mng.rbk[i].rbk_addr_rgb = g_l2c_mng.mem_addr_rbk[ g_l2c_mng.mem_bank_rbk ];
		}
		// Map ( decide here )
		g_l2c_mng.map.rbk_addr_rgb = g_l2c_mng.mem_addr_rbk[ g_l2c_mng.mem_bank_rbk ];
		g_l2c_mng.map.map_addr_yp = g_l2c_mng.mem_addr_map_yp[ g_l2c_mng.mem_bank_map ];
		g_l2c_mng.map.map_addr_yn = g_l2c_mng.mem_addr_map_yn[ g_l2c_mng.mem_bank_map ];
		// Overwrite Share
		g_l2c_share_dol_combine.dol_map.mem_addr_yp = g_l2c_mng.map.map_addr_yp;
		g_l2c_share_dol_combine.dol_map.mem_addr_yn = g_l2c_mng.map.map_addr_yp;
		g_l2c_share_dol_combine.dol_map.h_byte = g_l2c_mng.map.map_h_byte;
		g_l2c_share_dol_combine.dol_map.h_size = g_l2c_mng.map.h_size;
		g_l2c_share_dol_combine.dol_map.v_size = g_l2c_mng.map.v_size;
		Set_Frame_Share_Dol_Combine( &g_l2c_share_dol_combine, frame_no );
		// Update Bank
		g_l2c_mng.mem_bank_rbk = (g_l2c_mng.mem_bank_rbk+1) % D_L2C_MEM_BANK_RBK;
		g_l2c_mng.mem_bank_map = (g_l2c_mng.mem_bank_map+1) % D_L2C_MEM_BANK_MAP;
		// Start RBK
		if( g_l2c_mng.pipe_num == 2 ){
			Start_LTM_RBK( &g_l2c_mng.rbk[0], &g_l2c_mng.rbk[1] );
		}else{
			Start_LTM_RBK( &g_l2c_mng.rbk[0], NULL );
		}
	break;
	case E_SYSTEM_EVENT_DOL_COMBINE_RBK:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_LTM_MAP, FJ_MBALOG_FUNC_ID_LTM_LTM_INT, FJ_MBALOG_TYPE_END, "LTM RBK"));
		Stop_LTM_RBK();
		g_l2c_save_map_frame = g_l2c_save_rbk_frame;
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_LTM_MAP, FJ_MBALOG_FUNC_ID_LTM_LTM_INT, FJ_MBALOG_TYPE_START, "LTM MAP %d", (INT32)g_l2c_save_map_frame));
		Start_LTM_MAP( &g_l2c_mng.map );
	break;
	case E_SYSTEM_EVENT_DOL_COMBINE_MAP:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_LTM_MAP, FJ_MBALOG_FUNC_ID_LTM_LTM_INT, FJ_MBALOG_TYPE_END, "LTM MAP"));
		Stop_LTM_MAP();
		g_l2c_save_ltm_frame = g_l2c_save_map_frame;
		
		// ---- state machine start ---- 
		switch( g_l2c_mng.state ){
		case E_LTM2CNR_STATE_WAIT:
			if( STM_Sensor2YUV_Event( E_SYSTEM_EVENT_NONE, -1 ) == D_STM_IDLE ){ // R2Y will not come...
				g_l2c_mng.state = E_LTM2CNR_STATE_RUN;
				start_ltm2cnr(g_l2c_save_ltm_frame);
			}else{
				g_l2c_mng.state = E_LTM2CNR_STATE_WAIT_R2Y;
			}
		break;
		case E_LTM2CNR_STATE_WAIT_MAP:
			if( P2P_PIPE_Is_Run() ){
				g_l2c_mng.state = E_LTM2CNR_STATE_WAIT_R2Y;
				UPRINTF( "Skip LTMR2Y\n" );
			}else{
				g_l2c_mng.state = E_LTM2CNR_STATE_RUN;
				start_ltm2cnr(g_l2c_save_ltm_frame);
			}
		break;
		default:
		break;
		}
		// ---- state machine end ---- 
		
	break;
	case E_SYSTEM_EVENT_CAP_PIPE_DUMMY:
	case E_SYSTEM_EVENT_PIPE_DOL_YUV: // wait PIPE is end
		
		// ---- state machine start ---- 
		switch( g_l2c_mng.state ){
		case E_LTM2CNR_STATE_WAIT_1st:
			g_l2c_mng.state = E_LTM2CNR_STATE_WAIT;
		break;
		case E_LTM2CNR_STATE_WAIT:
			g_l2c_mng.state = E_LTM2CNR_STATE_WAIT_MAP;
		break;
		case E_LTM2CNR_STATE_WAIT_R2Y:
			g_l2c_mng.state = E_LTM2CNR_STATE_RUN;
			start_ltm2cnr(g_l2c_save_ltm_frame);
		break;
		default:
		break;
		}
		// ---- state machine end ---- 
		
	break;
	case E_SYSTEM_EVENT_LTMCNR:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_LTM_RGB, FJ_MBALOG_FUNC_ID_LTM_LTM_INT, FJ_MBALOG_TYPE_END,  "LTM-R2Y-CNR"));
		P2P_PIPE_Stop( TRUE, TRUE, FALSE );
		Stop_LTM_LTM2R2Y( TRUE, TRUE );
		g_l2c_mng.mem_bank_yc = (g_l2c_mng.mem_bank_yc+1) % D_L2C_MEM_BANK_YC;
		
		// ---- state machine start ---- 
		g_l2c_mng.state = E_LTM2CNR_STATE_WAIT;
		// ---- state machine end ---- 
		

		// debug
		if( g_l2c_in.debug_dol_en ){
			debug_dol(frame_no, 0);
			break; // go back
		}
		
		// Set Share
		g_l2c_share_pipe_yuv.layout.addr_y = g_l2c_mng.pipe[0].addr_y;
		g_l2c_share_pipe_yuv.layout.addr_c = g_l2c_mng.pipe[0].addr_c;
		Set_Frame_Share_PIPE_YUV(&g_l2c_share_pipe_yuv, g_l2c_save_ltm_frame);
		//Set_Frame_Share_R2Y_Hist(&g_share_r2y_hist, g_l2c_save_ltm_frame);
		Snd_Event_to_System( E_SYSTEM_EVENT_PIPE_YUV, g_l2c_save_ltm_frame );
	break;
	case E_SYSTEM_EVENT_DEBUG_DOL_COPY:
		debug_dol(frame_no, 1);
	break;
	case E_SYSTEM_EVENT_DEBUG_DOL_COPY2:
		debug_dol(frame_no, 2);
	break;
	default:
	break;
	}
	return D_STM_IDLE;
}


INT32 STM_LTM2CNR_Stop( void)
{
	Stop_LTM_RBK();
	Stop_LTM_MAP();
	Stop_LTM_LTM2R2Y( TRUE, TRUE );
	return D_STM_IDLE;
}


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void start_ltm2cnr( ULLONG frame_no )
{
	T_SHARE_PIPE_DOL_YUV	dol_yuv;
	T_SHARE_SENSOR_EXP		exp;
	T_SHARE_ISP_GAIN		isp_gain_reg;
	INT32					i;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_LTM_RGB, FJ_MBALOG_FUNC_ID_LTM_LTM_INT, FJ_MBALOG_TYPE_START, "LTM-R2Y-CNR %d", (INT32)frame_no));
		
	// Set Addr
	g_l2c_mng.pipe[0].addr_y = g_l2c_mng.mem_addr_y[ g_l2c_mng.mem_bank_yc ];
	g_l2c_mng.pipe[1].addr_y = g_l2c_mng.mem_addr_y[ g_l2c_mng.mem_bank_yc ];
	g_l2c_mng.pipe[0].addr_c = g_l2c_mng.mem_addr_c[ g_l2c_mng.mem_bank_yc ];
	g_l2c_mng.pipe[1].addr_c = g_l2c_mng.mem_addr_c[ g_l2c_mng.mem_bank_yc ];
	
	// Set Share
	Get_Frame_Share_PIPE_DOL_YUV( &dol_yuv, frame_no );
	g_l2c_share_pipe_yuv.sensor[E_SHARE_SENSOR_ID_0].yuv = dol_yuv.long_yuv;
	g_l2c_share_pipe_yuv.sensor[E_SHARE_SENSOR_ID_0].yuv.format = g_l2c_share_pipe_yuv.layout.format;
	g_l2c_share_pipe_yuv.sensor[E_SHARE_SENSOR_ID_0].yuv.video  = g_l2c_share_pipe_yuv.layout.video;
	g_l2c_share_pipe_yuv.sensor[E_SHARE_SENSOR_ID_0].yuv.addr_y = g_l2c_share_pipe_yuv.layout.addr_y;
	g_l2c_share_pipe_yuv.sensor[E_SHARE_SENSOR_ID_0].yuv.addr_c = g_l2c_share_pipe_yuv.layout.addr_c;
	g_l2c_share_pipe_yuv.sensor[E_SHARE_SENSOR_ID_0].yuv.h_byte = g_l2c_share_pipe_yuv.layout.h_byte;
	g_l2c_share_pipe_yuv.sensor[E_SHARE_SENSOR_ID_0].yuv.h_size = g_l2c_share_pipe_yuv.layout.h_size;
	g_l2c_share_pipe_yuv.sensor[E_SHARE_SENSOR_ID_0].yuv.v_size = g_l2c_share_pipe_yuv.layout.v_size;
	
	// Calc Size
	Get_Frame_Share_Dol_Combine( &g_l2c_share_dol_combine, frame_no );
	for( i = 0; i < g_l2c_mng.pipe_num; i ++ ){
		g_l2c_mng.ltm[i].rgb_h_byte = g_l2c_share_dol_combine.dol_rgb.rgb_h_byte;
		g_l2c_mng.ltm[i].rgb_addr_r = g_l2c_share_dol_combine.dol_rgb.mem_addr_r + g_l2c_mng.ltm[i].rgb_addr_ofst;
		g_l2c_mng.ltm[i].rgb_addr_g = g_l2c_share_dol_combine.dol_rgb.mem_addr_g + g_l2c_mng.ltm[i].rgb_addr_ofst;
		g_l2c_mng.ltm[i].rgb_addr_b = g_l2c_share_dol_combine.dol_rgb.mem_addr_b + g_l2c_mng.ltm[i].rgb_addr_ofst;
		g_l2c_mng.ltm[i].map_addr_yp = g_l2c_share_dol_combine.dol_map.mem_addr_yp;
		g_l2c_mng.ltm[i].map_addr_yn = g_l2c_share_dol_combine.dol_map.mem_addr_yn;
	}
	// error check
	if( g_l2c_mng.ltm[0].rgb_h_size <= 0 || g_l2c_mng.ltm[0].rgb_v_size <= 0 ){
		UPRINTF_ERR(1);
	}

	/* IQ */
	Get_Frame_Share_Sensor_Exp( &exp, frame_no );
	Get_Frame_Share_ISP_Gain( &isp_gain_reg, frame_no );
	g_l2c_mng.iq_scene = g_l2c_in.scene_normal;
	g_l2c_mng.iq_stream = 0;
	

	g_l2c_mng.iq_gain_times[0] = g_l2c_mng.iq_gain_times[1] = g_l2c_mng.iq_gain_times[2] = 
	exp.sensor[0].exp.again_times * exp.sensor[0].exp.dgain_times * isp_gain_reg.sensor[0].b2b_end[E_PGAIN_GR];

	IQPara_Scene_Stream_Table( &g_l2c_mng.iq_scene, 1 );
	IQPara_UpdateGainArray( g_l2c_mng.iq_gain_times, 0.0 );
	iq_update( TRUE, g_l2c_mng.iq_stream );

	if( g_l2c_mng.pipe_num == 1 ){
		set_r2y_gamma( TRUE, FALSE );
		P2P_PIPE_Set_R2Y_Irq_Callback( NULL );
		P2P_PIPE_Start( FALSE, g_l2c_in.cnr_type, g_l2c_in.is_r2y_out_ipu, FALSE, FALSE, TRUE, &g_l2c_mng.pipe[0], NULL, 0 );
		Start_LTM_LTM2R2Y( &g_l2c_mng.ltm[0], NULL );
	}else{
		set_r2y_gamma( TRUE, TRUE );
		// Start HW
		P2P_PIPE_Set_R2Y_Irq_Callback( NULL );
		P2P_PIPE_Start( FALSE, g_l2c_in.cnr_type, g_l2c_in.is_r2y_out_ipu, FALSE, FALSE, TRUE, &g_l2c_mng.pipe[0], &g_l2c_mng.pipe[1], 0 );
		Start_LTM_LTM2R2Y( &g_l2c_mng.ltm[0], &g_l2c_mng.ltm[1] );
	}
}


static void set_r2y_gamma( BOOL set_pipe0, BOOL set_pipe1 )
{
	unsigned int i;

	static const USHORT gamma_abs[D_IM_R2Y_TABLE_MAX_GAMMA] = {
	0x0000, 0x0004, 0x0008, 0x000c, 0x0010, 0x0014, 0x0018, 0x001c,
	0x0020, 0x0024, 0x0028, 0x002c, 0x0030, 0x0034, 0x0038, 0x003c,
	0x0040, 0x0044, 0x0048, 0x004c, 0x0050, 0x0054, 0x0058, 0x005c,
	0x0060, 0x0064, 0x0068, 0x006c, 0x0070, 0x0074, 0x0078, 0x007c,
	0x0080, 0x0084, 0x0088, 0x008c, 0x0090, 0x0094, 0x0098, 0x009c,
	0x00a0, 0x00a4, 0x00a8, 0x00ac, 0x00b0, 0x00b4, 0x00b8, 0x00bc,
	0x00c0, 0x00c4, 0x00c8, 0x00cc, 0x00d0, 0x00d4, 0x00d8, 0x00dc,
	0x00e0, 0x00e4, 0x00e8, 0x00ec, 0x00f0, 0x00f4, 0x00f8, 0x00fc,
	0x0100, 0x0104, 0x0108, 0x010c, 0x0110, 0x0114, 0x0118, 0x011c,
	0x0120, 0x0124, 0x0128, 0x012c, 0x0130, 0x0134, 0x0138, 0x013c,
	0x0140, 0x0144, 0x0148, 0x014c, 0x0150, 0x0154, 0x0158, 0x015c,
	0x0160, 0x0164, 0x0168, 0x016c, 0x0170, 0x0174, 0x0178, 0x017c,
	0x0180, 0x0184, 0x0188, 0x018c, 0x0190, 0x0194, 0x0198, 0x019c,
	0x01a0, 0x01a4, 0x01a8, 0x01ac, 0x01b0, 0x01b4, 0x01b8, 0x01bc,
	0x01c0, 0x01c4, 0x01c8, 0x01cc, 0x01d0, 0x01d4, 0x01d8, 0x01dc,
	0x01e0, 0x01e4, 0x01e8, 0x01ec, 0x01f0, 0x01f4, 0x01f8, 0x01fc,
	0x0200, 0x0204, 0x0208, 0x020c, 0x0210, 0x0214, 0x0218, 0x021c,
	0x0220, 0x0224, 0x0228, 0x022c, 0x0230, 0x0234, 0x0238, 0x023c,
	0x0240, 0x0244, 0x0248, 0x024c, 0x0250, 0x0254, 0x0258, 0x025c,
	0x0260, 0x0264, 0x0268, 0x026c, 0x0270, 0x0274, 0x0278, 0x027c,
	0x0280, 0x0284, 0x0288, 0x028c, 0x0290, 0x0294, 0x0298, 0x029c,
	0x02a0, 0x02a4, 0x02a8, 0x02ab, 0x02af, 0x02b3, 0x02b7, 0x02bb,
	0x02bf, 0x02c3, 0x02c7, 0x02cb, 0x02cf, 0x02d3, 0x02d7, 0x02db,
	0x02df, 0x02e3, 0x02e7, 0x02eb, 0x02ef, 0x02f3, 0x02f7, 0x02fb,
	0x02ff, 0x0303, 0x0307, 0x030b, 0x030f, 0x0313, 0x0317, 0x031b,
	0x031f, 0x0323, 0x0327, 0x032b, 0x032f, 0x0333, 0x0337, 0x033b,
	0x033f, 0x0343, 0x0347, 0x034b, 0x034f, 0x0353, 0x0357, 0x035b,
	0x035f, 0x0363, 0x0367, 0x036b, 0x036f, 0x0373, 0x0377, 0x037b,
	0x037f, 0x0383, 0x0387, 0x038b, 0x038f, 0x0393, 0x0397, 0x039b,
	0x039f, 0x03a3, 0x03a7, 0x03ab, 0x03af, 0x03b3, 0x03b7, 0x03bb,
	0x03bf, 0x03c3, 0x03c7, 0x03cb, 0x03cf, 0x03d3, 0x03d7, 0x03db,
	0x03df, 0x03e3, 0x03e7, 0x03eb, 0x03ef, 0x03f3, 0x03f7, 0x03fb,
	};
	
	static const ULLONG gamma_ofs[D_IM_R2Y_TABLE_MAX_GAMMA] = {
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x0001000100001000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0010000100010000ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	};
	
	if( (set_pipe0 == TRUE) && (set_pipe1 == TRUE) ){
		for (i = 0; i < 5; i++) {
			Im_R2Y_Set_Gamma_Table( D_IM_R2Y_PIPE12, i, gamma_abs, gamma_ofs);
		}
   	}else{
		for (i = 0; i < 5; i++) {
			Im_R2Y_Set_Gamma_Table( D_IM_R2Y_PIPE1,  i, gamma_abs, gamma_ofs);
		}
	}
}

static void debug_dol_xch_callback(void){
	
	g_l2c_debug_dol_xch_cnt ++;
	
	if( g_l2c_debug_dol_xch_cnt == 2){
		Snd_Event_to_System( E_SYSTEM_EVENT_DEBUG_DOL_COPY, g_l2c_debug_dol_yuv_frame );
	}
	if( g_l2c_debug_dol_xch_cnt == 4){
		Snd_Event_to_System( E_SYSTEM_EVENT_DEBUG_DOL_COPY2, g_l2c_debug_dol_yuv_frame );
	}
}

static void debug_dol( ULLONG frame_no, INT32 cnt )
{
	static T_SHARE_PIPE_DOL_YUV		dol_yuv;
	
	T_IM_XCH_CTRL_CMN	xch_ctrl_cmn = {0};
	T_IM_XCH_CTRL_THIN	xch_ctrl_thin = {0};
	
	if( cnt == 2 ){
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_XCH, FJ_MBALOG_FUNC_ID_XCH, FJ_MBALOG_TYPE_END,  "Xch Irq"));
		Send_Share_User_Event(E_SHARE_USER_EVENT_YUV_END, g_l2c_debug_dol_yuv_frame);
		return;// go back
	}
	
	if( cnt == 0 ){
		// Set Sare
		Get_Frame_Share_PIPE_DOL_YUV( &dol_yuv, frame_no );
		g_l2c_debug_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].format		= dol_yuv.long_yuv.format;
		g_l2c_debug_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].video		= dol_yuv.long_yuv.video;
		if( g_l2c_in.debug_dol_en == 1 ){
			g_l2c_debug_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y	= dol_yuv.long_yuv.addr_y;
			g_l2c_debug_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c	= dol_yuv.long_yuv.addr_c;
		}else{
			g_l2c_debug_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y	= g_l2c_mng.mem_addr_debug_dol_y;
			g_l2c_debug_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c	= g_l2c_mng.mem_addr_debug_dol_c;
		}
		g_l2c_debug_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_byte		= dol_yuv.long_yuv.h_byte;
		g_l2c_debug_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_size		= dol_yuv.long_yuv.h_size;
		g_l2c_debug_share_yuv.layout[E_SHARE_YUV_OUT_USER_0].v_size		= dol_yuv.long_yuv.v_size;
		Set_Frame_Share_YUV(&g_l2c_debug_share_yuv, frame_no);
	
		// Copy
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_XCH, FJ_MBALOG_FUNC_ID_XCH, FJ_MBALOG_TYPE_START,  "Copy"));
		g_l2c_debug_dol_yuv_frame = frame_no;
		g_l2c_debug_dol_xch_cnt = 0;
	}

	if( g_l2c_in.debug_dol_en == 1 ){
		xch_ctrl_cmn.mode		= D_IM_XCH_MD_COPY;
		xch_ctrl_cmn.g_hsize	=  dol_yuv.long_yuv.h_byte    & 0xFFF0;	// 16n
		xch_ctrl_cmn.d_hsize	= (dol_yuv.long_yuv.h_size/3) & 0xFFF0;	// 16n
		xch_ctrl_cmn.d_vsize	=  dol_yuv.long_yuv.v_size;
		xch_ctrl_cmn.pCallBack	= debug_dol_xch_callback;
	
		xch_ctrl_thin.xdtype	= D_IM_XCH_XDTYPE_8;
		xch_ctrl_thin.hcyc		= 1;
		xch_ctrl_thin.vcyc		= 1;
		xch_ctrl_thin.hen		= 0xFFFFFFFF;
		xch_ctrl_thin.ven		= 0xFFFFFFFF;
		xch_ctrl_thin.s_hsize	= xch_ctrl_cmn.g_hsize;
	
		// XCH0 copy short to long
		if( cnt == 0 ){
			// Y
			xch_ctrl_cmn.dst_addr	= dol_yuv.long_yuv.addr_y + xch_ctrl_cmn.d_hsize;
			xch_ctrl_thin.src_addr	= dol_yuv.short_yuv.addr_y;
			xch_ctrl_cmn.d_vsize	= dol_yuv.long_yuv.v_size;
		}else{
			// C
			xch_ctrl_cmn.dst_addr	= dol_yuv.long_yuv.addr_c + xch_ctrl_cmn.d_hsize;
			xch_ctrl_thin.src_addr	= dol_yuv.short_yuv.addr_c;
			xch_ctrl_cmn.d_vsize	= dol_yuv.long_yuv.v_size/2;
		}
		Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_0, &xch_ctrl_cmn );
		Im_Xch_Ctrl_Thin( E_IM_XCH_CH_SEL_0, &xch_ctrl_thin );
		Im_Xch_Start_Async( E_IM_XCH_CH_SEL_0 );
	
		// XCH1 copy combine to long
		if( cnt == 0 ){
			// Y
			xch_ctrl_cmn.dst_addr	= dol_yuv.long_yuv.addr_y + xch_ctrl_cmn.d_hsize*2;
			xch_ctrl_thin.src_addr	= g_l2c_mng.pipe[0].addr_y;
			xch_ctrl_cmn.d_vsize	=  dol_yuv.long_yuv.v_size;
		}else{
			// C
			xch_ctrl_cmn.dst_addr	= dol_yuv.long_yuv.addr_c + xch_ctrl_cmn.d_hsize*2;
			xch_ctrl_thin.src_addr	= g_l2c_mng.pipe[0].addr_c;
			xch_ctrl_cmn.d_vsize	=  dol_yuv.long_yuv.v_size/2;
		}
		Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_1, &xch_ctrl_cmn );
		Im_Xch_Ctrl_Thin( E_IM_XCH_CH_SEL_1, &xch_ctrl_thin );
		Im_Xch_Start_Async( E_IM_XCH_CH_SEL_1 );
	}
	else{
		xch_ctrl_cmn.mode		= D_IM_XCH_MD_DOWN_HI_SPEED;
		xch_ctrl_cmn.g_hsize	=  dol_yuv.long_yuv.h_byte    & 0xFFF0;	// 16n
		xch_ctrl_cmn.d_hsize	= (dol_yuv.long_yuv.h_size/2) & 0xFFF0;	// 16n
		xch_ctrl_cmn.d_vsize	=  dol_yuv.long_yuv.v_size/2;
		xch_ctrl_cmn.pCallBack	= debug_dol_xch_callback;
	
		xch_ctrl_thin.xdtype	= D_IM_XCH_XDTYPE_8;
		xch_ctrl_thin.hcyc		= 4;
		xch_ctrl_thin.vcyc		= 2;
		xch_ctrl_thin.hen		= 0xCCCCCCCC;
		xch_ctrl_thin.ven		= 0xAAAAAAAA;
		xch_ctrl_thin.s_hsize	= xch_ctrl_cmn.g_hsize;
	
		// XCH0 copy and thin Long and Short
		if( cnt == 0 ){
			// Y
			xch_ctrl_cmn.dst_addr	= g_l2c_mng.mem_addr_debug_dol_y;
			xch_ctrl_thin.src_addr	= dol_yuv.long_yuv.addr_y;
			xch_ctrl_cmn.d_vsize	= dol_yuv.long_yuv.v_size;
		}else{
			// C
			xch_ctrl_cmn.dst_addr	= g_l2c_mng.mem_addr_debug_dol_c;
			xch_ctrl_thin.src_addr	= dol_yuv.long_yuv.addr_c;
			xch_ctrl_cmn.d_vsize	= dol_yuv.long_yuv.v_size/2;
		}
		Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_0, &xch_ctrl_cmn );
		Im_Xch_Ctrl_Thin( E_IM_XCH_CH_SEL_0, &xch_ctrl_thin );
		Im_Xch_Start_Async( E_IM_XCH_CH_SEL_0 );
	
		// XCH1 copy and thin Long and Short
		if( cnt == 0 ){
			// Y
			xch_ctrl_cmn.dst_addr	= g_l2c_mng.mem_addr_debug_dol_y + xch_ctrl_cmn.d_hsize;
			xch_ctrl_thin.src_addr	= g_l2c_mng.pipe[0].addr_y;
			xch_ctrl_cmn.d_vsize	=  dol_yuv.long_yuv.v_size/2;
		}else{
			// C
			xch_ctrl_cmn.dst_addr	= g_l2c_mng.mem_addr_debug_dol_c + xch_ctrl_cmn.d_hsize;
			xch_ctrl_thin.src_addr	= g_l2c_mng.pipe[0].addr_c;
			xch_ctrl_cmn.d_vsize	=  dol_yuv.long_yuv.v_size/2/2;
		}
		Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_1, &xch_ctrl_cmn );
		Im_Xch_Ctrl_Thin( E_IM_XCH_CH_SEL_1, &xch_ctrl_thin );
		Im_Xch_Start_Async( E_IM_XCH_CH_SEL_1 );
	}
}

static void iq_update( BOOL is_tbl, UCHAR stream )
{
	IQPara_R2Y( stream, D_IQ_PIPE01 );
	IQPara_CNR( stream, D_IQ_PIPE01 );
	if( is_tbl ){
		IQPara_R2Y_Tbl( stream, D_IQ_PIPE01 );
	}

	IQDbg_Dump_IQ_PrintOut(stream, 1);
	
}
