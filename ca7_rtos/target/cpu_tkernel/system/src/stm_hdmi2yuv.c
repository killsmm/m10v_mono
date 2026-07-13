/**
 * @file		stm_hdmi2yuv.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include "mode_stm.h"
#include "ctrl_pro2pipe.h"
#include "camera_config.h"
#include "share_data.h"
#include "category_parameter.h"
#include "sdram_map_define.h"
#include "ctrl_pro2pipe.h"
#include "im_iip.h"
#include "user_sync_task.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define ROUNDUP8(a) (((a) + 7) & 0xFFFFFFF8)
#define ROUNDUP_8(val)	(((UINT32)(val) +7) & 0xFFFFFFF8)

#define BANK_SET_TO_ING_AND_INCREMENT(a, b, mem, lock) a[E_MEM_BANK_ING] = a[E_MEM_BANK_SET];\
											a[E_MEM_BANK_SET] = ( a[E_MEM_BANK_SET] + 1) % b;\
											if( mem[ a[E_MEM_BANK_SET] ] == lock ){\
												a[E_MEM_BANK_SET] = ( a[E_MEM_BANK_SET] + 1) % b;\
											}
#define BANK_ING_TO_RCV(a) a[E_MEM_BANK_RCV] = a[E_MEM_BANK_ING]

#define FLG_UV_CONV_TRIG					0x00000001
#define FLG_UV_CONV_EXIT					0x80000000
#define FLG_UV_CONV_WAITPTN					(FLG_UV_CONV_TRIG | FLG_UV_CONV_EXIT)

#define IS_FLGPTN_EQUAL(flgptn, FLG)	((((flgptn) & (FLG)) == (FLG)) ? TRUE : FALSE)
/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define UPDATE_YUV_FRAME_AND_SEND_TO_SYSTEM { \
			BANK_ING_TO_RCV(g_h2y_mng.mem_bank_y); \
			BANK_ING_TO_RCV(g_h2y_mng.mem_bank_c); \
			g_share_pipe_yuv.layout.addr_y = g_h2y_mng.mem_addr_y[g_h2y_mng.mem_bank_y[E_MEM_BANK_RCV]]; \
			g_share_pipe_yuv.layout.addr_c = g_h2y_mng.mem_addr_c[g_h2y_mng.mem_bank_c[E_MEM_BANK_RCV]]; \
			for( i = 0; i < E_SHARE_HDMI_NUM_MAX; i++ ){ \
				if( g_h2y_in.sen_use & (1<<i) ){ \
					g_share_pipe_yuv.sensor[i].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_h2y_mng.addr_ofst_y[i]; \
					g_share_pipe_yuv.sensor[i].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_h2y_mng.addr_ofst_c[i]; \
				} \
			} \
			Set_Frame_Share_PIPE_YUV(&g_share_pipe_yuv, g_yuv_cnt);\
			Snd_Event_to_System( E_SYSTEM_EVENT_PIPE_YUV, g_yuv_cnt ); \
			g_yuv_cnt++; \
}
/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	ULONG   			mem_addr_y[D_PIPE_MEM_BANK_Y];
	ULONG   			mem_addr_c[D_PIPE_MEM_BANK_C];
	UCHAR   			mem_bank_y[E_MEM_BANK_MAX];
	UCHAR   			mem_bank_c[E_MEM_BANK_MAX];
	
	UCHAR				mem_bank_yc_num;
	
	ULONG				addr_ofst_y[4+1];
	ULONG				addr_ofst_c[4+1];

	ULONG   			mem_addr_c_alpha[D_PIPE_MEM_BANK_C];
	UCHAR   			mem_bank_c_alpha[E_MEM_BANK_MAX];
	ULONG				addr_ofst_c_alpha[4+1];
	
	T_P2P_CPHY_DMIPI	cphy_dmipi;
	T_P2P_SEN   		sen;
	T_P2P_SEN_PWCH 		pwch[E_SHARE_HDMI_NUM_MAX];

	ULONG				cbcr_pwch_global;
	ULONG				cbcr_out_global;

} T_HDMI2YUV_MNG;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_STM_HDMI2YUV_IN g_h2y_in;
static T_HDMI2YUV_MNG 	g_h2y_mng;
static T_SHARE_PIPE_YUV	g_share_pipe_yuv;
static ULLONG 			g_yuv_cnt = 0;
static INT32			g_hdmi2yuv_state = D_STM_IDLE;
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static int conv_uv_image(const USHORT src_h_byte, const USHORT dst_h_byte, const USHORT h_size, const USHORT v_size, const ULONG uv_in_addr, const ULONG uv_out_addr);

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
INT32 STM_HDMI2YUV_Config( const T_STM_HDMI2YUV_IN* in, T_STM_HDMI2YUV_OUT* const out )
{
	UINT32  	mem_size, mem_addr;
	INT32 		i;
	E_SDRAM_CH 	sdram_ch;
	T_Y2Y_SEN	sen;
	USHORT		ofst_y = 0, ofst_c = 0, ofst_c_alpha = 0;
	ULONG		total_h_size = 0;
	ULONG		pwch_y_global = 0;
	ULONG		pwch_c_global = 0;

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SYSTEM, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "Start"));

	/* Clear 0 */
	memset( (VOID*)&g_h2y_mng, 0, sizeof(g_h2y_mng) );
	memset( (VOID*)&g_share_pipe_yuv, 0, sizeof(g_share_pipe_yuv));

	/* Get Input */
	g_h2y_in = *in;

	sen.sen_connect = E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI;
	sen.sen_use		= g_h2y_in.sen_use;
	for( i = 0; i < E_SHARE_HDMI_NUM_MAX; i++ ){
		if( g_h2y_in.sen_use & (1 << i) ) {
			if( g_h2y_in.hdmi_yuv_format == E_SHARE_YUV_FORMAT_422_SEMI) {
				g_h2y_mng.cphy_dmipi.bitwidth[i] = 16; // YUV422
			} else {
				g_h2y_mng.cphy_dmipi.bitwidth[i] = 15; // YUV420
			}

			g_h2y_mng.cphy_dmipi.dphy_1lane_Mbps[i]	= in->mipi_rate;
			g_h2y_mng.cphy_dmipi.lane_num[i]		= 4;
			sen.Hpixel[i] 							= in->one_hdmi_yuv_h_size[i];
			total_h_size 							+= sen.Hpixel[i];

			if( g_h2y_in.hdmi_yuv_format == E_SHARE_YUV_FORMAT_422_SEMI) {
				// YUV422
				// Y0 Y1 Y2 Y3 ...
				g_h2y_mng.pwch[i].h[1].byte = ROUNDUP8(sen.Hpixel[i]);	// Y
				g_h2y_mng.pwch[i].h[1].size = sen.Hpixel[i];			// Y
				// U0 V0 U1 V1 ...
				// U0 V0 U1 V1 ...
				g_h2y_mng.pwch[i].h[0].byte = ROUNDUP8(sen.Hpixel[i]);  // UV same size as Y due to UVUVUV layout
				g_h2y_mng.pwch[i].h[0].size = sen.Hpixel[i]; 			// UV same size as Y due to UVUVUV layout

				g_h2y_mng.addr_ofst_y[i]	= ofst_y;
				g_h2y_mng.addr_ofst_c[i]	= ofst_c;

				ofst_y 					+= g_h2y_mng.pwch[i].h[1].byte;
				ofst_c 					+= g_h2y_mng.pwch[i].h[0].byte;
				pwch_y_global			+= g_h2y_mng.pwch[i].h[1].byte;
				pwch_c_global			+= g_h2y_mng.pwch[i].h[0].byte;
			} else { 
				// YUV420 LEGACY
				// (Y0/Y1) 1pixel (Y2/Y3) 1pixel ...
				g_h2y_mng.pwch[i].h[1].byte = ROUNDUP8(sen.Hpixel[i]);		// Y
				g_h2y_mng.pwch[i].h[1].size = sen.Hpixel[i] >> 1;			// Y (Y0/Y1 16bits one pixel)
				// U0 U1 U2 ...
				// V0 V1 V2 ...
				g_h2y_mng.pwch[i].h[0].byte = ROUNDUP8(sen.Hpixel[i] >> 1); // UV half as Y
				g_h2y_mng.pwch[i].h[0].size = sen.Hpixel[i] >> 1;			// UV half as Y

				g_h2y_mng.addr_ofst_y[i]		= ofst_y;
				g_h2y_mng.addr_ofst_c_alpha[i]	= ofst_c_alpha;
				g_h2y_mng.addr_ofst_c[i]		= ofst_c;

				ofst_y 							+= g_h2y_mng.pwch[i].h[1].byte;
				ofst_c_alpha					+= g_h2y_mng.pwch[i].h[0].byte;
				ofst_c 							+= sen.Hpixel[i]; // uvuvuv, so it equal to Y Hsize

				pwch_y_global				+= g_h2y_mng.pwch[i].h[1].byte;
				pwch_c_global				+= g_h2y_mng.pwch[i].h[0].byte;
			}
			// YUV422 case, UV-vertical = Y-vertical
			// YUV420 case, UV-vertical = Y-vertical due to layout following
			// UUUUUU
			// VVVVVV
			// UUUUUU
			// VVVVVV
			// so we using same v size as Y
			g_h2y_mng.pwch[i].v[0].size		= in->one_hdmi_yuv_v_size[i];
			// Y case
			g_h2y_mng.pwch[i].v[1].size		= in->one_hdmi_yuv_v_size[i];
		}
	}

	for( i = 0; i < E_SHARE_HDMI_NUM_MAX; i++ ){
		if( g_h2y_in.sen_use & (1 << i) ) {
			g_h2y_mng.pwch[i].h[1].global_byte = pwch_y_global;
			g_h2y_mng.pwch[i].h[0].global_byte = pwch_c_global;
		}
	}

	// used for UV convert later
	g_h2y_mng.cbcr_pwch_global	= pwch_c_global;
	g_h2y_mng.cbcr_out_global 	= ofst_c;

	g_h2y_mng.addr_ofst_y[4] = ofst_y * in->output_yuv_v_size;

	if( g_h2y_in.hdmi_yuv_format == E_SHARE_YUV_FORMAT_422_SEMI) {
		// YUV422
		// uvuv
		// uvuv
		// height is same as Y
		g_h2y_mng.addr_ofst_c[4] = ofst_c * in->output_yuv_v_size;
	} else {
		// Its UV convert later offset
		// uvuv
		// uvuv
		// height is half
		g_h2y_mng.addr_ofst_c[4] = ofst_c * (in->output_yuv_v_size >> 1);
		// Its PWch output offset
		// uuuu
		// vvvv
		// uuuu
		// vvvv
		g_h2y_mng.addr_ofst_c_alpha[4] = ofst_c_alpha * in->output_yuv_v_size;
	}

	/* Calc Memory */
	g_h2y_mng.mem_bank_yc_num = 3;
	if( g_h2y_mng.mem_bank_yc_num > D_PIPE_MEM_BANK_Y ){
		g_h2y_mng.mem_bank_yc_num = D_PIPE_MEM_BANK_Y;
	}

	if( g_h2y_in.use_ch1 ){
		mem_addr = g_h2y_in.available_memory_addr_ch1;
	}else{
		mem_addr = g_h2y_in.available_memory_addr_ch0;
	}

	mem_size = g_h2y_mng.addr_ofst_y[4];
	for( i = 0; i < g_h2y_mng.mem_bank_yc_num; i ++ ){
		g_h2y_mng.mem_addr_y[i] = (mem_addr + 127) & 0xFFFFFF80;
		mem_addr = g_h2y_mng.mem_addr_y[i] + mem_size;
	}

	mem_size = g_h2y_mng.addr_ofst_c[4];
	for( i = 0; i < g_h2y_mng.mem_bank_yc_num; i ++ ){
		g_h2y_mng.mem_addr_c[i] = (mem_addr + 127) & 0xFFFFFF80;
		mem_addr = g_h2y_mng.mem_addr_c[i] + mem_size;
	}

	if( g_h2y_in.hdmi_yuv_format == E_SHARE_YUV_FORMAT_420_SEMI) {
		mem_size = g_h2y_mng.addr_ofst_c_alpha[4];
		for( i = 0; i < g_h2y_mng.mem_bank_yc_num; i ++ ){
			g_h2y_mng.mem_addr_c_alpha[i] = (mem_addr + 127) & 0xFFFFFF80;
			mem_addr = g_h2y_mng.mem_addr_c_alpha[i] + mem_size;
		}
	}

	/* Set Output */
	if( g_h2y_in.use_ch1 ){
		out->available_memory_addr_ch0 = g_h2y_in.available_memory_addr_ch0;
		out->available_memory_addr_ch1 = mem_addr;
		sdram_ch = E_SDRAM_CH1;
	}else{
		out->available_memory_addr_ch0 = mem_addr;
		out->available_memory_addr_ch1 = g_h2y_in.available_memory_addr_ch1;
		sdram_ch = E_SDRAM_CH0;
	}
	// ------------------------------
	// Bandwidth
	// ------------------------------
	for( i = 0; i < E_SHARE_HDMI_NUM_MAX; i ++ ){
		if( g_h2y_in.sen_use & (1 << i) ) {
			switch( i ){
			case 0:
				out->bw_byte[ sdram_ch ][E_BW_ITEM_SEN_PWCH01] += (g_h2y_mng.pwch[i].h[0].byte * g_h2y_mng.pwch[i].v[0].size);
				out->bw_byte[ sdram_ch ][E_BW_ITEM_SEN_PWCH01] += (g_h2y_mng.pwch[i].h[1].byte * g_h2y_mng.pwch[i].v[1].size);
				break;
			case 1:
				out->bw_byte[ sdram_ch ][E_BW_ITEM_SEN_PWCH23] += (g_h2y_mng.pwch[i].h[0].byte * g_h2y_mng.pwch[i].v[0].size);
				out->bw_byte[ sdram_ch ][E_BW_ITEM_SEN_PWCH23] += (g_h2y_mng.pwch[i].h[1].byte * g_h2y_mng.pwch[i].v[1].size);
				break;
			case 2:
				out->bw_byte[ sdram_ch ][E_BW_ITEM_SEN_PWCH45] += (g_h2y_mng.pwch[i].h[0].byte * g_h2y_mng.pwch[i].v[0].size);
				out->bw_byte[ sdram_ch ][E_BW_ITEM_SEN_PWCH45] += (g_h2y_mng.pwch[i].h[1].byte * g_h2y_mng.pwch[i].v[1].size);
				break;
			case 3:
				out->bw_byte[ sdram_ch ][E_BW_ITEM_SEN_PWCH67] += (g_h2y_mng.pwch[i].h[0].byte * g_h2y_mng.pwch[i].v[0].size);
				out->bw_byte[ sdram_ch ][E_BW_ITEM_SEN_PWCH67] += (g_h2y_mng.pwch[i].h[1].byte * g_h2y_mng.pwch[i].v[1].size);
				break;
			default:
				break;
			}
		}
	}

	/* Set Address and Byte */
	for( i = 0; i < E_SHARE_HDMI_NUM_MAX; i ++ ){
		if( g_h2y_in.sen_use & (1 << i) ) {
			g_h2y_mng.pwch[i].h[1].addr = g_h2y_mng.mem_addr_y[g_h2y_mng.mem_bank_y[E_MEM_BANK_SET]] + g_h2y_mng.addr_ofst_y[i];
			if( g_h2y_in.hdmi_yuv_format == E_SHARE_YUV_FORMAT_420_SEMI) {
				g_h2y_mng.pwch[i].h[0].addr = g_h2y_mng.mem_addr_c_alpha[g_h2y_mng.mem_bank_c_alpha[E_MEM_BANK_SET]] + g_h2y_mng.addr_ofst_c_alpha[i];
			} else {
				g_h2y_mng.pwch[i].h[0].addr = g_h2y_mng.mem_addr_c[g_h2y_mng.mem_bank_c[E_MEM_BANK_SET]] + g_h2y_mng.addr_ofst_c[i];
			}
		}
	}
	/* Reset */
	P2P_Init();

	Y2Y_PWCH_Start(g_h2y_in.sen_use & (1 << E_SHARE_HDMI_0) ? &g_h2y_mng.pwch[0] : NULL,
				   g_h2y_in.sen_use & (1 << E_SHARE_HDMI_1) ? &g_h2y_mng.pwch[1] : NULL,
				   g_h2y_in.sen_use & (1 << E_SHARE_HDMI_2) ? &g_h2y_mng.pwch[2] : NULL,
				   g_h2y_in.sen_use & (1 << E_SHARE_HDMI_3) ? &g_h2y_mng.pwch[3] : NULL);

	Y2Y_SEN_Start(&sen);

	g_h2y_mng.cphy_dmipi.sensor_use			= g_h2y_in.sen_use;
	g_h2y_mng.cphy_dmipi.point_output_mode	= E_IM_PRO_DMIPI_OUT_MODE_2POINT;
	/* Start CPHY */
	P2P_CPHY_DMIPI_Start(&g_h2y_mng.cphy_dmipi);

	/* Set Share_Pipe_Yuv */
	 //Set YUV420 forcedly due to SHDR bandwidth for 4kp60 or dual 4kp30, set double global of UV later in SHDR in
	g_share_pipe_yuv.layout.format = E_SHARE_YUV_FORMAT_420_SEMI;
	g_share_pipe_yuv.layout.addr_y = g_h2y_mng.mem_addr_y[g_h2y_mng.mem_bank_y[E_MEM_BANK_SET]];
	g_share_pipe_yuv.layout.addr_c = g_h2y_mng.mem_addr_c[g_h2y_mng.mem_bank_c[E_MEM_BANK_SET]];
	g_share_pipe_yuv.layout.h_byte = ofst_y;
	g_share_pipe_yuv.layout.h_size = total_h_size;
	g_share_pipe_yuv.layout.v_size = in->output_yuv_v_size;

	UPRINTF("HDMI total H:%d, max V:%d\n", g_share_pipe_yuv.layout.h_size, g_share_pipe_yuv.layout.v_size);
	for( i = 0; i < E_SHARE_HDMI_NUM_MAX; i++ ){
		if( g_h2y_in.sen_use & (1<<i) ){
			g_share_pipe_yuv.sensor[i].yuv.sensor_total_h_size = in->one_hdmi_yuv_h_size[i];
			g_share_pipe_yuv.sensor[i].yuv.sensor_total_v_size = in->output_yuv_v_size;
			g_share_pipe_yuv.sensor[i].yuv.sensor_ref_h_size = in->one_hdmi_yuv_h_size[i];
			g_share_pipe_yuv.sensor[i].yuv.sensor_ref_v_size = in->output_yuv_v_size;
			 //Set YUV420 forcedly due to SHDR bandwidth for 4kp60 or dual 4kp30, set double global of UV later in SHDR in
			g_share_pipe_yuv.sensor[i].yuv.format = E_SHARE_YUV_FORMAT_420_SEMI;
			g_share_pipe_yuv.sensor[i].yuv.h_byte = ofst_y; // global h_byte
			g_share_pipe_yuv.sensor[i].yuv.addr_y = g_share_pipe_yuv.layout.addr_y + g_h2y_mng.addr_ofst_y[i];
			g_share_pipe_yuv.sensor[i].yuv.addr_c = g_share_pipe_yuv.layout.addr_c + g_h2y_mng.addr_ofst_c[i];
			g_share_pipe_yuv.sensor[i].yuv.h_size = in->one_hdmi_yuv_h_size[i];
			g_share_pipe_yuv.sensor[i].yuv.v_size = in->output_yuv_v_size;

			UPRINTF("SEN[%d] H:%d, V:%d, Y_addr:%lx, C_addr:%lx\n",	i,
																	g_share_pipe_yuv.sensor[i].yuv.h_size,
																	g_share_pipe_yuv.sensor[i].yuv.v_size,
																	g_share_pipe_yuv.sensor[i].yuv.addr_y,
																	g_share_pipe_yuv.sensor[i].yuv.addr_c);
		}
	}

	g_yuv_cnt = 0;

	return D_STM_OK;
}

INT32 STM_HDMI2YUV_Start( void)
{
	int rval = 0;

	g_hdmi2yuv_state = D_STM_BUSY;
	
	OS_User_Clr_Flg( FID_UV_CONV, 0 );
	OS_User_Sta_Tsk( TID_UV_CONVERT, 0 );

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SYSTEM, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "HDMI2MIPI Start"));

	return rval;
}

INT32 STM_HDMI2YUV_Event( E_SYSTEM_EVENT event, ULLONG frame_no )
{
	T_SHARE_SYSTEM_CONFIG config;
	T_SHARE_SYSTEM_PARAM param;
	int i;

	// check state
	if( g_hdmi2yuv_state == D_STM_IDLE ){
		return g_hdmi2yuv_state;
	}
	
	Get_Share_System_Param(&param);

	switch( event ) {
	case E_SYSTEM_EVENT_VD:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_VD, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "VD"));
		Get_Share_System_Config( &config );
		g_h2y_in.frame_num = config.frame_num; // For burst capture, to stop pipe

		BANK_SET_TO_ING_AND_INCREMENT(g_h2y_mng.mem_bank_y, g_h2y_mng.mem_bank_yc_num, g_h2y_mng.mem_addr_y, param.lock_address_y);

		if( g_h2y_in.hdmi_yuv_format == E_SHARE_YUV_FORMAT_420_SEMI) {
			BANK_SET_TO_ING_AND_INCREMENT(g_h2y_mng.mem_bank_c_alpha, g_h2y_mng.mem_bank_yc_num, g_h2y_mng.mem_addr_c_alpha, 0xFFFFFFF);
		} else {
			BANK_SET_TO_ING_AND_INCREMENT(g_h2y_mng.mem_bank_c, g_h2y_mng.mem_bank_yc_num, g_h2y_mng.mem_addr_c, param.lock_address_c);
		}

		for( i = 0; i < E_SHARE_HDMI_NUM_MAX; i ++ ) {
			if( g_h2y_in.sen_use & (1 << i) ) {
				g_h2y_mng.pwch[i].h[1].addr = g_h2y_mng.mem_addr_y[ g_h2y_mng.mem_bank_y[E_MEM_BANK_SET] ] + g_h2y_mng.addr_ofst_y[i];

				if( g_h2y_in.hdmi_yuv_format == E_SHARE_YUV_FORMAT_420_SEMI) {
					g_h2y_mng.pwch[i].h[0].addr = g_h2y_mng.mem_addr_c_alpha[ g_h2y_mng.mem_bank_c_alpha[E_MEM_BANK_SET] ] + g_h2y_mng.addr_ofst_c_alpha[i];
				} else {
					g_h2y_mng.pwch[i].h[0].addr = g_h2y_mng.mem_addr_c[ g_h2y_mng.mem_bank_c[E_MEM_BANK_SET] ] + g_h2y_mng.addr_ofst_c[i];
				}
			}
		}

		Y2Y_PWCH_Address(g_h2y_in.sen_use & (1 << E_SHARE_HDMI_0) ? &g_h2y_mng.pwch[0] : NULL,
						 g_h2y_in.sen_use & (1 << E_SHARE_HDMI_1) ? &g_h2y_mng.pwch[1] : NULL,
						 g_h2y_in.sen_use & (1 << E_SHARE_HDMI_2) ? &g_h2y_mng.pwch[2] : NULL,
						 g_h2y_in.sen_use & (1 << E_SHARE_HDMI_3) ? &g_h2y_mng.pwch[3] : NULL);

		break;
	case E_SYSTEM_EVENT_LDIV_PWCH:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_LDIV_PWCH, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "PWCH"));
		if( g_h2y_in.hdmi_yuv_format == E_SHARE_YUV_FORMAT_422_SEMI) {
			// update YUV one frame info and send to system
			UPDATE_YUV_FRAME_AND_SEND_TO_SYSTEM;
		} else if( g_h2y_in.hdmi_yuv_format == E_SHARE_YUV_FORMAT_420_SEMI ) {
			OS_User_Set_Flg( FID_UV_CONV, FLG_UV_CONV_TRIG );
		}
		
		if( g_h2y_in.frame_num != D_FRAME_NUM_INFINITY && (UINT32)g_yuv_cnt >= g_h2y_in.frame_num) {
			STM_HDMI2YUV_Stop();
		}
		break;
	default:
		break;
	}

	return g_hdmi2yuv_state;
}

INT32 STM_HDMI2YUV_Stop( void)
{
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SYSTEM, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "HDMI2MIPI Stop"));
	
	OS_User_Set_Flg( FID_UV_CONV, FLG_UV_CONV_EXIT );
	Y2Y_PWCH_Stop();
	P2P_CPHY_DMIPI_Stop();

	g_hdmi2yuv_state = D_STM_IDLE;
	
	return g_hdmi2yuv_state;
}

VOID UV_Conv_Task( INT32 stacd )
{
	OS_USER_FLGPTN flgptn = 0;
	T_SHARE_SYSTEM_PARAM param;
	int i;

	while(1) {
		// wait event
		OS_User_Twai_Flg( FID_UV_CONV, FLG_UV_CONV_WAITPTN, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg( FID_UV_CONV, ~FLG_UV_CONV_WAITPTN );

		if( IS_FLGPTN_EQUAL( flgptn, FLG_UV_CONV_TRIG ) ){
			Get_Share_System_Param(&param);
			BANK_ING_TO_RCV(g_h2y_mng.mem_bank_c_alpha);
			// sen0	sen1 	 sen0 sen1 (example 2 sen)
			// uuuu uuuu  -> uvuv uvuv
			// vvvv vvvv  -> uvuv uvuv
			conv_uv_image(g_h2y_mng.cbcr_pwch_global,			// source global byte
						  g_h2y_mng.cbcr_out_global,			// dest global byte
						  g_share_pipe_yuv.layout.h_size >> 1,	// Total Y Hsize / 2, would be total each Cb Cr Hsize
						  g_share_pipe_yuv.layout.v_size >> 1,	// YUV420, UV height is half of Y
						  g_h2y_mng.mem_addr_c_alpha[g_h2y_mng.mem_bank_c_alpha[E_MEM_BANK_RCV]],
						  g_h2y_mng.mem_addr_c[g_h2y_mng.mem_bank_c[E_MEM_BANK_SET]]);
			BANK_SET_TO_ING_AND_INCREMENT(g_h2y_mng.mem_bank_c, g_h2y_mng.mem_bank_yc_num, g_h2y_mng.mem_addr_c, param.lock_address_c);
			// update YUV one frame info and send to system
			UPDATE_YUV_FRAME_AND_SEND_TO_SYSTEM;
		}

		// Exit Task
		if( IS_FLGPTN_EQUAL( flgptn, FLG_UV_CONV_EXIT ) ){
			break;
		}
	}

	OS_User_Ext_Tsk();
}
/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static int conv_uv_image(const USHORT src_h_byte, const USHORT dst_h_byte, const USHORT h_size, const USHORT v_size, const ULONG uv_in_addr, const ULONG uv_out_addr)
{
	// src
	// uuuu
	// vvvv

	// dst
	// uvuv
	// uvuv

	// img A img B, please make there  is no GAP between 2 images
	// AAAABBBB
	// UUUUUUUU 
	// VVVVVVVV

	// AAAABBBB
	// UVUVUVUV 
	// UVUVUVUV
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_RR_BUF_BYTES];
	ULONG param_buffer_addr = ROUNDUP_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 1000;
	T_IM_IIP_UTIL_RR iip_util_rr0;
	int rval = 0;

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_07, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "CONV YUV"));

	memset(&iip_util_rr0, 0, sizeof(T_IM_IIP_UTIL_RR));

	iip_util_rr0.afn_unitid = E_IM_IIP_UNIT_ID_AFN0;
	iip_util_rr0.sl_unitid = E_IM_IIP_UNIT_ID_SL2;
	iip_util_rr0.src_pixid = E_IM_IIP_PIXID_4;
	iip_util_rr0.dst_pixid = E_IM_IIP_PIXID_5;

	iip_util_rr0.src.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_rr0.src.gbl.frame_type = E_IM_IIP_FTYPE_PLANE;
	iip_util_rr0.src.gbl.pix_format = E_IM_IIP_PFMT_CBCR;
	iip_util_rr0.src.gbl.chunky_color.component0 = 0;
	iip_util_rr0.src.gbl.chunky_color.component1 = 0;
	iip_util_rr0.src.gbl.chunky_color.component2 = 0;
	iip_util_rr0.src.gbl.chunky_color.component3 = 0;
	iip_util_rr0.src.gbl.sign_Y_G = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.src.gbl.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.src.gbl.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.src.gbl.sign_D3 = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.src.gbl.masterIF_Y_G = D_IM_IIP_MASTER_IF0;
	iip_util_rr0.src.gbl.masterIF_Cb_B = D_IM_IIP_MASTER_IF0;
	iip_util_rr0.src.gbl.masterIF_Cr_R = D_IM_IIP_MASTER_IF0;
	iip_util_rr0.src.gbl.masterIF_Alpha = D_IM_IIP_MASTER_IF0;

	iip_util_rr0.dst.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_rr0.dst.gbl.frame_type = E_IM_IIP_FTYPE_CHUNKY;
	iip_util_rr0.dst.gbl.pix_format = E_IM_IIP_PFMT_CBCR;
	iip_util_rr0.dst.gbl.chunky_color.component0 = D_IM_IIP_CHUNKY_COLOR_CB_B;
	iip_util_rr0.dst.gbl.chunky_color.component1 = D_IM_IIP_CHUNKY_COLOR_CR_R;
	iip_util_rr0.dst.gbl.chunky_color.component2 = 0;
	iip_util_rr0.dst.gbl.chunky_color.component3 = 0;
	iip_util_rr0.dst.gbl.sign_Y_G = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.dst.gbl.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.dst.gbl.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.dst.gbl.sign_D3 = D_IM_IIP_UNSIGNED_DATA;
	iip_util_rr0.dst.gbl.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	iip_util_rr0.dst.gbl.masterIF_Alpha = D_IM_IIP_MASTER_IF1;

	iip_util_rr0.afn_cache_select = E_IM_IIP_PARAM_CSEL_0;
	iip_util_rr0.pix_depth = D_IM_IIP_PDEPTH_8BITS;
	iip_util_rr0.rotation = E_IM_IIP_UTIL_ROTATE_000;
	iip_util_rr0.flip = E_IM_IIP_UTIL_FLIP_OFF;
	iip_util_rr0.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;

	iip_util_rr0.src.rect.width				= h_size;
	iip_util_rr0.src.rect.lines				= v_size;
	iip_util_rr0.src.gbl.pix_format			= E_IM_IIP_PFMT_CBCR;
	iip_util_rr0.src.gbl.width				= h_size;
	iip_util_rr0.src.gbl.lines				= v_size;
	iip_util_rr0.src.gbl.line_bytes.Y_G		= 0;
	iip_util_rr0.src.gbl.line_bytes.Cb_B	= src_h_byte << 1;
	iip_util_rr0.src.gbl.line_bytes.Cr_R	= src_h_byte << 1;
	iip_util_rr0.src.gbl.line_bytes.Alpha	= 0;
	iip_util_rr0.src.gbl.addr.Y_G			= 0;
	iip_util_rr0.src.gbl.addr.Cb_B			= uv_in_addr;
	iip_util_rr0.src.gbl.addr.Cr_R			= uv_in_addr + src_h_byte;
	iip_util_rr0.src.gbl.addr.Alpha			= 0;

	iip_util_rr0.dst.rect.width				= h_size;
	iip_util_rr0.dst.rect.lines				= v_size;
	iip_util_rr0.dst.gbl.pix_format			= E_IM_IIP_PFMT_CBCR;
	iip_util_rr0.dst.gbl.width				= h_size;
	iip_util_rr0.dst.gbl.lines				= v_size;
	iip_util_rr0.dst.gbl.line_bytes.Y_G		= 0;
	iip_util_rr0.dst.gbl.line_bytes.Cb_B	= dst_h_byte;
	iip_util_rr0.dst.gbl.line_bytes.Cr_R	= dst_h_byte;
	iip_util_rr0.dst.gbl.line_bytes.Alpha	= 0;
	iip_util_rr0.dst.gbl.addr.Y_G			= 0;
	iip_util_rr0.dst.gbl.addr.Cb_B			= uv_out_addr;
	iip_util_rr0.dst.gbl.addr.Cr_R			= 0;
	iip_util_rr0.dst.gbl.addr.Alpha			= 0;

	rval = IM_IIP_Utility_ResizeRotate( &iip_util_rr0,
										NULL,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	UPRINTF_ERR(rval);

	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_07, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "CONV YUV"));

	return rval;
}


