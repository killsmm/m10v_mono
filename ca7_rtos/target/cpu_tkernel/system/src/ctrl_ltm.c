/**
 * @file		ctrl_ltm.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "ctrl_ltm.h"

#include "im_img.h"
#include "im_ltm.h"

#include "system.h"
#include "fj_still.h"
#include "still_utility.h"

#include "iqpara_api.h"

#include <string.h>


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
static E_IM_IMG_IMGPIPEPSW_SEL	g_b2rcnc;
static E_IM_IMG_IMGPIPEPSW_SEL	g_r2ysel;

static volatile ULONG g_ltm_map_current_flg = 0;
static volatile ULONG g_ltm_map_compare_flg = 0;
static volatile ULLONG g_ltm_map_cnt = 0;

static volatile ULONG g_ltm_rbk_current_flg = 0;
static volatile ULONG g_ltm_rbk_compare_flg = 0;
static volatile ULLONG g_ltm_rbk_cnt = 0;

const USHORT g_ltm_ylog_tbl_sample[D_IM_LTM_TABLE_MAX_YLOG] = {
	   0,  905, 1434, 1810, 2101, 2340, 2541, 2715, 2869, 3007, 3131, 3245, 3349, 3446, 3536, 3620,
	3700, 3774, 3845, 3912, 3976, 4036, 4094, 4150, 4203, 4255, 4304, 4351, 4397, 4441, 4484, 4526,
	4566, 4605, 4643, 4680, 4715, 4750, 4784, 4817, 4849, 4881, 4912, 4942, 4971, 5000, 5028, 5055,
	5082, 5109, 5134, 5160, 5185, 5209, 5233, 5257, 5280, 5302, 5325, 5347, 5368, 5389, 5410, 5431,
	5451, 5471, 5491, 5510, 5529, 5548, 5566, 5585, 5603, 5621, 5638, 5655, 5672, 5689, 5706, 5722,
	5739, 5755, 5770, 5786, 5802, 5817, 5832, 5847, 5862, 5876, 5891, 5905, 5919, 5933, 5947, 5960,
	5974, 5987, 6001, 6014, 6027, 6040, 6052, 6065, 6077, 6090, 6102, 6114, 6126, 6138, 6150, 6162,
	6173, 6185, 6196, 6208, 6219, 6230, 6241, 6252, 6263, 6273, 6284, 6295, 6305, 6316, 6326, 6336,
	6346, 6356, 6366, 6376, 6386, 6396, 6406, 6415, 6425, 6434, 6444, 6453, 6462, 6472, 6481, 6490,
	6499, 6508, 6517, 6526, 6535, 6543, 6552, 6561, 6569, 6578, 6586, 6595, 6603, 6611, 6619, 6628,
	6636, 6644, 6652, 6660, 6668, 6676, 6684, 6691, 6699, 6707, 6714, 6722, 6730, 6737, 6745, 6752,
	6759, 6767, 6774, 6781, 6789, 6796, 6803, 6810, 6817, 6824, 6831, 6838, 6845, 6852, 6859, 6866,
	6872, 6879, 6886, 6893, 6899, 6906, 6912, 6919, 6926, 6932, 6938, 6945, 6951, 6958, 6964, 6970,
	6976, 6983, 6989, 6995, 7001, 7007, 7013, 7020, 7026, 7032, 7038, 7043, 7049, 7055, 7061, 7067,
	7073, 7079, 7084, 7090, 7096, 7102, 7107, 7113, 7118, 7124, 7130, 7135, 7141, 7146, 7152, 7157,
	7163, 7168, 7173, 7179, 7184, 7189, 7195, 7200, 7205, 7210, 7216, 7221, 7226, 7231, 7236, 7241,
	7246, 7252, 7257, 7262, 7267, 7272, 7277, 7282, 7287, 7291, 7296, 7301, 7306, 7311, 7316, 7321,
	7325, 7330, 7335, 7340, 7344, 7349, 7354, 7358, 7363, 7368, 7372, 7377, 7382, 7386, 7391, 7395,
	7400, 7404, 7409, 7413, 7418, 7422, 7427, 7431, 7435, 7440, 7444, 7449, 7453, 7457, 7462, 7466,
	7470, 7474, 7479, 7483, 7487, 7491, 7496, 7500, 7504, 7508, 7512, 7516, 7521, 7525, 7529, 7533,
	7537, 7541, 7545, 7549, 7553, 7557, 7561, 7565, 7569, 7573, 7577, 7581, 7585, 7589, 7593, 7597,
	7600, 7604, 7608, 7612, 7616, 7620, 7623, 7627, 7631, 7635, 7639, 7642, 7646, 7650, 7654, 7657,
	7661, 7665, 7668, 7672, 7676, 7679, 7683, 7687, 7690, 7694, 7697, 7701, 7705, 7708, 7712, 7715,
	7719, 7722, 7726, 7729, 7733, 7736, 7740, 7743, 7747, 7750, 7754, 7757, 7761, 7764, 7768, 7771,
	7774, 7778, 7781, 7784, 7788, 7791, 7795, 7798, 7801, 7805, 7808, 7811, 7814, 7818, 7821, 7824,
	7828, 7831, 7834, 7837, 7840, 7844, 7847, 7850, 7853, 7856, 7860, 7863, 7866, 7869, 7872, 7875,
	7879, 7882, 7885, 7888, 7891, 7894, 7897, 7900, 7903, 7906, 7910, 7913, 7916, 7919, 7922, 7925,
	7928, 7931, 7934, 7937, 7940, 7943, 7946, 7949, 7952, 7955, 7958, 7961, 7963, 7966, 7969, 7972,
	7975, 7978, 7981, 7984, 7987, 7990, 7992, 7995, 7998, 8001, 8004, 8007, 8010, 8012, 8015, 8018,
	8021, 8024, 8026, 8029, 8032, 8035, 8038, 8040, 8043, 8046, 8049, 8051, 8054, 8057, 8060, 8062,
	8065, 8068, 8070, 8073, 8076, 8079, 8081, 8084, 8087, 8089, 8092, 8095, 8097, 8100, 8103, 8105,
	8108, 8110, 8113, 8116, 8118, 8121, 8123, 8126, 8129, 8131, 8134, 8136, 8139, 8142, 8144, 8147,
};


/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static VOID LTM_Ctrl_LTM_Handler(UINT32* int_status, UINT32 user_param);
static VOID LTM_Ctrl_Map_Handler( UINT32* result, UINT32 user_param );
static VOID LTM_Ctrl_Rbk_Handler( UINT32* result, UINT32 user_param );

/*==============================================================================*/
/* Debug Function																*/
/*==============================================================================*/

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void Init_LTM_All( void )
{
	g_ltm_map_cnt = 0;
	g_ltm_rbk_cnt = 0;

	Im_LTM_SW_Reset_All_Block( D_IM_LTM_PIPE12 );
	Im_LTM_Init_All_Block( D_IM_LTM_PIPE12 );
}


void Start_LTM_RBK( const T_CTRL_LTM_RBK* p0, const T_CTRL_LTM_RBK* p1 )
{
	const T_CTRL_LTM_RBK*				in;
	T_IM_LTM_RBK_CTRL_COMMON			rbk_ctrl = {0};
	T_IM_LTM_RBK_CTRL_SDRAM_INPUT		rbk_sdramin = {0};
	T_IM_LTM_INADDR_INFO				in_addr = {0};
	T_IM_LTM_RBK_OUTDATA_INFO			out_data_info = {0};
	UCHAR								conversion_on_off = 0;
	
	INT32	i;
	
	for( i = 0; i < 2; i ++ ){
		if( i == 0 ){
			in = p0;
		}else{
			in = p1;
		}

		if( in ){
			
			if( i == 0 ) {
				g_ltm_rbk_compare_flg |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
			} else {
				g_ltm_rbk_compare_flg |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
			}
			
			conversion_on_off			= in->rbk_sta ? D_IM_LTM_ENABLE_ON : D_IM_LTM_ENABLE_OFF;
			Im_LTM_RBK_Ctrl_Resolution_Conversion( i, conversion_on_off );
			
			rbk_ctrl.reduction_enable	= in->rbk_rdcen;
			rbk_ctrl.reduction_mode		= in->rbk_rdcmd;
			rbk_ctrl.line_intr			= 0;
			rbk_ctrl.burst_length		= D_IM_LTM_BRST_512;
			rbk_ctrl.mask_write			= D_IM_LTM_MASK_WRITE_ENABLE;
			rbk_ctrl.out_select			= D_IM_LTM_RBK_OUT_SDRAM;
			rbk_ctrl.data_type			= D_IM_LTM_DTYP_UNPACK12;
			rbk_ctrl.rbk_user_handler	= LTM_Ctrl_Rbk_Handler;
			rbk_ctrl.user_param			= 0;
			Im_LTM_RBK_Ctrl_Common( i, &rbk_ctrl );
			
			rbk_sdramin.burst_length_r			= D_IM_LTM_BRST_512;
			rbk_sdramin.burst_length_g			= D_IM_LTM_BRST_512;
			rbk_sdramin.burst_length_b			= D_IM_LTM_BRST_512;
			rbk_sdramin.input_global			= in->rgb_h_byte;
			rbk_sdramin.input_rect.img_top		= 0;
			rbk_sdramin.input_rect.img_left		= 0;
			rbk_sdramin.input_rect.img_width	= in->rgb_h_size;
			rbk_sdramin.input_rect.img_lines	= in->rgb_v_size;
			rbk_sdramin.shift_enable			= D_IM_LTM_ENABLE_ON;
			Im_LTM_RBK_Ctrl_ModeSDRAMInput( i, &rbk_sdramin );
			
			in_addr.addr_r = (VOID*)(in->rgb_addr_r + in->rgb_addr_ofst);
			in_addr.addr_g = (VOID*)(in->rgb_addr_g + in->rgb_addr_ofst);
			in_addr.addr_b = (VOID*)(in->rgb_addr_b + in->rgb_addr_ofst);
			Im_LTM_RBK_Set_InAddr_Info( i, &in_addr );
			
			out_data_info.addr			= (VOID*)(in->rbk_addr_rgb + in->rbk_addr_ofst);
			out_data_info.global_width	= in->rbk_h_byte;
			Im_LTM_RBK_Set_OutData_Info( i, &out_data_info );
			Im_LTM_RBK_Set_FrameStop( i, 1 );
			
			Im_LTM_RBK_Start( i );
		}
	}
}


void Stop_LTM_RBK( void )
{
	Im_LTM_RBK_Stop( D_IM_LTM_PIPE12 );
}


void Start_LTM_MAP( const T_CTRL_LTM_MAP* p0 )
{
	const T_CTRL_LTM_MAP*			in;
	T_IM_LTM_MAP_CTRL_COMMON		map_ctrl = {0};
	T_IM_LTM_MAP_OUTDATA_INFO		out_data_info = {0};
	T_IM_LTM_MAP_INDATA_INFO		in_data_info = {0};
	T_IM_LTM_MAP_CTRL_SDRAM_INPUT	map_ctrl_sdram_input = {0};
	INT32 i;
	
	for( i = 0; i < 2; i ++ ){
		
		if( i == 0 ){
			in = p0;
		}else{
			in = NULL;// only use PIPE0
		}

		if( in ){
			
			if( i == 0 ) {
				g_ltm_map_compare_flg |= D_IM_LTM_MAP1_INT_STATE_MAP_END;
			} else {
				g_ltm_map_compare_flg |= D_IM_LTM_MAP2_INT_STATE_MAP_END;
			}
			
			if( in ){
				map_ctrl.wch_burst_length_yp	= D_IM_LTM_BRST_512;
				map_ctrl.wch_burst_length_yn	= D_IM_LTM_BRST_512;
				map_ctrl.mask_write_yp			= D_IM_LTM_MASK_WRITE_ENABLE;
				map_ctrl.mask_write_yn			= D_IM_LTM_MASK_WRITE_ENABLE;
				map_ctrl.write_select_yp		= D_IM_LTM_PORT_DEFAULT;
				map_ctrl.write_select_yn		= D_IM_LTM_PORT_DEFAULT;
				map_ctrl.data_type_yp			= D_IM_LTM_DTYP_UNPACK12;
				map_ctrl.data_type_yn			= D_IM_LTM_DTYP_UNPACK12;
				map_ctrl.map_user_handler		= LTM_Ctrl_Map_Handler;
				map_ctrl.user_param				= 0;
				Im_LTM_MAP_Ctrl_Common( i, &map_ctrl );
			
				map_ctrl_sdram_input.burst_length 			= D_IM_LTM_BRST_512;
				map_ctrl_sdram_input.input_global 			= in->rbk_h_byte;
				map_ctrl_sdram_input.input_rect.img_top		= 0;
				map_ctrl_sdram_input.input_rect.img_left	= 0;
				map_ctrl_sdram_input.input_rect.img_width	= in->h_size;
				map_ctrl_sdram_input.input_rect.img_lines	= in->v_size;
				map_ctrl_sdram_input.data_type 				= D_IM_LTM_DTYP_UNPACK12;
				Im_LTM_MAP_Ctrl_ModeSDRAMInput(i, &map_ctrl_sdram_input);

				in_data_info.addr				= (VOID*)in->rbk_addr_rgb;
				in_data_info.top_bit_offset		= 0;
				Im_LTM_MAP_Set_InData_Info(i, &in_data_info);

				out_data_info.addr_yp			= (VOID*)(in->map_addr_yp);
				out_data_info.global_width_yp	= in->map_h_byte;
				out_data_info.addr_yn			= (VOID*)(in->map_addr_yn);
				out_data_info.global_width_yn	= in->map_h_byte;
				Im_LTM_MAP_Set_OutData_Info( i, &out_data_info );
			
				Im_LTM_MAP_Set_FrameStop( i, 1 );

				Im_LTM_MAP_Start(i);
			}
		}
	}
}


void Stop_LTM_MAP( void )
{
	Im_LTM_MAP_Stop( D_IM_LTM_PIPE12 );
}


void Start_LTM_LTM2R2Y( const T_CTRL_LTM_LTM* p0, const T_CTRL_LTM_LTM* p1 )
{
	const T_CTRL_LTM_LTM*			in;
	T_IM_LTM_LTM_CTRL_COMMON		ltm_ctrl = {0};
	T_IM_LTM_LTM_CTRL_SDRAM_INPUT	ltm_ctrl_sdram_input = {0};
	T_IM_LTM_INADDR_INFO			in_addr = {0};
	T_IM_LTM_LTM_INDATA_INFO		in_data_info = {0};
	T_IM_LTM_LTM_MAP_RESIZE_PITCH	resize_pitch = {0};
	T_IM_LTM_INTERNAL_PARAM			internal_param = {0};
	
	INT32	i;
	
	// Save Path
	g_b2rcnc = Im_IMG_Get_IMGPIPEPSW(E_IM_IMG_IMGPIPEPSW_B2RCNC);
	g_r2ysel = Im_IMG_Get_IMGPIPEPSW(E_IM_IMG_IMGPIPEPSW_R2YSEL);
	
	// Change Path None->LTM->R2Y
	Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_B2RCNC, E_IM_IMG_IMGPIPEPSW_B2R );
	Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_R2YSEL, E_IM_IMG_IMGPIPEPSW_LTM );
	
	// Pipe
	for( i = 0; i < 2; i ++ ){
		if( i == 0 ){
			in = p0;
		}else{
			in = p1;
		}

		if( in ){
			ltm_ctrl.line_intr = 0;
			ltm_ctrl.rch_burst_length_yp = D_IM_LTM_BRST_512;
			ltm_ctrl.rch_burst_length_yn = D_IM_LTM_BRST_512;
			ltm_ctrl.wch_burst_length_r = D_IM_LTM_BRST_512;
			ltm_ctrl.wch_burst_length_g = D_IM_LTM_BRST_512;;
			ltm_ctrl.wch_burst_length_b = D_IM_LTM_BRST_512;;
			ltm_ctrl.mask_write_r = D_IM_LTM_MASK_WRITE_ENABLE;
			ltm_ctrl.mask_write_g = D_IM_LTM_MASK_WRITE_ENABLE;
			ltm_ctrl.mask_write_b = D_IM_LTM_MASK_WRITE_ENABLE;
			ltm_ctrl.read_select_yp = D_IM_LTM_PORT_DEFAULT;
			ltm_ctrl.read_select_yn = D_IM_LTM_PORT_DEFAULT;
			ltm_ctrl.in_data_type_yp = D_IM_LTM_DTYP_UNPACK12;
			ltm_ctrl.in_data_type_yn = D_IM_LTM_DTYP_UNPACK12;
			ltm_ctrl.out_select = D_IM_LTM_LTM_OUT_R2Y_DIRECT;
			ltm_ctrl.out_data_type = D_IM_LTM_DTYP_UNPACK12; // no need if out_select is DIRECT
			
			// Map Size
			ltm_ctrl.input_rect.img_top = 0;
			ltm_ctrl.input_rect.img_left = 0;
			ltm_ctrl.input_rect.img_width = in->map_h_size;
			ltm_ctrl.input_rect.img_lines = in->map_v_size;
			ltm_ctrl.global_width_yp = in->map_h_byte;
			ltm_ctrl.global_width_yn = in->map_h_byte;
			
			ltm_ctrl.ltm_user_handler = LTM_Ctrl_LTM_Handler;
			ltm_ctrl.user_param = 0;
			Im_LTM_LTM_Ctrl_Common( i, &ltm_ctrl );
			
			ltm_ctrl_sdram_input.rch_burst_length_r = D_IM_LTM_BRST_512;
			ltm_ctrl_sdram_input.rch_burst_length_g = D_IM_LTM_BRST_512;
			ltm_ctrl_sdram_input.rch_burst_length_b = D_IM_LTM_BRST_512;
			
			// Image Size
			ltm_ctrl_sdram_input.input_global_rgb = in->rgb_h_byte;;
			ltm_ctrl_sdram_input.input_rect.img_top = 0;
			ltm_ctrl_sdram_input.input_rect.img_left = 0;
			ltm_ctrl_sdram_input.input_rect.img_width = in->rgb_h_size;
			ltm_ctrl_sdram_input.input_rect.img_lines = in->rgb_v_size;
			Im_LTM_LTM_Ctrl_ModeSDRAMInput( i, &ltm_ctrl_sdram_input );
			
			// Image Addr
			in_addr.addr_r = (VOID*)in->rgb_addr_r;
			in_addr.addr_g = (VOID*)in->rgb_addr_g;
			in_addr.addr_b = (VOID*)in->rgb_addr_b;
			Im_LTM_LTM_Set_InAddr_Info_Rgb( i, &in_addr );
			
			// Map Addr
			in_data_info.addr_yp = (VOID*)in->map_addr_yp;
			in_data_info.addr_yn = (VOID*)in->map_addr_yn;
			in_data_info.top_bit_offset_yp = 0;
			in_data_info.top_bit_offset_yn = 0;
			Im_LTM_LTM_Set_InData_Info( i, &in_data_info );
			
			resize_pitch.hsta = in->map_h_sta;
			resize_pitch.vsta = in->map_v_sta;
			resize_pitch.hpit = in->map_h_pit;
			resize_pitch.vpit = in->map_v_pit;
			Im_LTM_LTM_Set_MapImage_Resize_Pitch( i, &resize_pitch );
			
			// IQ,  call Set_LTM_LTM_IQ_Basic if need
			Im_LTM_MAP_Get_Internal_Param( 0, &internal_param );// get from MAP0 ( not MAP1 )
			Im_LTM_LTM_Set_Internal_Param( i, &internal_param );
			
			Im_LTM_LTM_Set_FrameStop( i, D_IM_LTM_ENABLE_ON );
			Im_LTM_LTM_Start( i );
		}
	}
}

void Stop_LTM_LTM2R2Y( BOOL stop_p0, BOOL stop_p1 )
{
	// Stop
	if( stop_p0 ){
		Im_LTM_LTM_Stop( 0 );
	}
	
	if( stop_p1 ){
		Im_LTM_LTM_Stop( 1 );
	}
	
	// Revert Path
	Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_B2RCNC, g_b2rcnc );
	Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_R2YSEL, g_r2ysel );
}

void Set_LTM_LTM_IQ_Basic( BOOL p0, BOOL p1 )
{
	T_IM_LTM_LTM_CTRL_BLEND			blend_ctrl = {0};
	T_IM_LTM_LTM_CTRL_HF_STRENGTH	hf_ctrl = {0};
	T_IM_LTM_LTM_CTRL_CHROMA		chroma_ctrl = {{0}};
	T_IM_LTM_CTRL_YLOG				ylog_ctrl = {0};
	INT32 i;
	
	for( i = 0; i < 2; i ++ ){
		
		if( !p0 ){
			continue;
		}
		if( !p1 ){
			continue;
		}
		
		blend_ctrl.ratio = 0; // RGB->Y is 100%
		blend_ctrl.offset[0] = 0;
		blend_ctrl.offset[1] = 0;
		blend_ctrl.offset[2] = 0;
		blend_ctrl.offset[3] = 0;
		blend_ctrl.offset[4] = 0;
		blend_ctrl.offset[5] = 0;
		blend_ctrl.offset[6] = 0;
		blend_ctrl.offset[7] = 0;
		blend_ctrl.gain[0] = 0;
		blend_ctrl.gain[1] = 0;
		blend_ctrl.gain[2] = 0;
		blend_ctrl.gain[3] = 0;
		blend_ctrl.gain[4] = 0;
		blend_ctrl.gain[5] = 0;
		blend_ctrl.gain[6] = 0;
		blend_ctrl.gain[7] = 0;
		blend_ctrl.border[0] = 0;
		blend_ctrl.border[1] = 0;
		blend_ctrl.border[2] = 0;
		blend_ctrl.border[3] = 0;
		blend_ctrl.border[4] = 0;
		blend_ctrl.border[5] = 0;
		blend_ctrl.border[6] = 0;
		blend_ctrl.border[7] = 4095;
		blend_ctrl.mode = 1; // USM
		Im_LTM_LTM_Ctrl_Blend( i, &blend_ctrl );
		
		hf_ctrl.strength = 0;
		Im_LTM_LTM_Set_High_Frequency_Strength( i, &hf_ctrl );
		
		chroma_ctrl.offset[0] = 0;
		chroma_ctrl.offset[1] = 0;
		chroma_ctrl.offset[2] = 0;
		chroma_ctrl.offset[3] = 0;
		chroma_ctrl.offset[4] = 0;
		chroma_ctrl.offset[5] = 0;
		chroma_ctrl.offset[6] = 0;
		chroma_ctrl.offset[7] = 0;
		chroma_ctrl.gain[0] = 0;
		chroma_ctrl.gain[1] = 0;
		chroma_ctrl.gain[2] = 0;
		chroma_ctrl.gain[3] = 0;
		chroma_ctrl.gain[4] = 0;
		chroma_ctrl.gain[5] = 0;
		chroma_ctrl.gain[6] = 0;
		chroma_ctrl.gain[7] = 0;
		chroma_ctrl.border[0] = 0;
		chroma_ctrl.border[1] = 0;
		chroma_ctrl.border[2] = 0;
		chroma_ctrl.border[3] = 0;
		chroma_ctrl.border[4] = 0;
		chroma_ctrl.border[5] = 0;
		chroma_ctrl.border[6] = 4095;
		Im_LTM_LTM_Ctrl_Chroma_Correction_Strength( i, &chroma_ctrl );
		
		ylog_ctrl.use_tbl = D_IM_LTM_YLOG_TBL_SIDE_0;
		ylog_ctrl.end_point0_tbl0 = g_ltm_ylog_tbl_sample[D_IM_LTM_TABLE_MAX_YLOG-1];
		ylog_ctrl.end_point0_tbl1 = g_ltm_ylog_tbl_sample[D_IM_LTM_TABLE_MAX_YLOG-1];
		ylog_ctrl.end_point1_tbl0 = g_ltm_ylog_tbl_sample[D_IM_LTM_TABLE_MAX_YLOG-1];
		ylog_ctrl.end_point1_tbl1 = g_ltm_ylog_tbl_sample[D_IM_LTM_TABLE_MAX_YLOG-1];
		Im_LTM_LTM_Ctrl_Ylog_Tbl( i, &ylog_ctrl );
		Im_LTM_LTM_Set_YLOG_Table( i, D_LTM_YLOG_TBL_00, g_ltm_ylog_tbl_sample );
		Im_LTM_LTM_Set_YLOG_Table( i, D_LTM_YLOG_TBL_01, g_ltm_ylog_tbl_sample );
		Im_LTM_LTM_Set_YLOG_Table( i, D_LTM_YLOG_TBL_10, g_ltm_ylog_tbl_sample );
		Im_LTM_LTM_Set_YLOG_Table( i, D_LTM_YLOG_TBL_11, g_ltm_ylog_tbl_sample );
		
		//Im_LTM_LTM_Set_RCG_Table( UCHAR pipe_no, const USHORT* const src_tbl );
	}
}


void Set_LTM_MAP_IQ_Basic( BOOL p0, BOOL p1 )
{
	INT32 i;
	T_IM_LTM_MAP_GAIN					wb_gain = {0};
	T_IM_LTM_MAP_CTRL_GENERATION_MODE	mode = {0};
	T_IM_LTM_CTRL_YLOG					ylog_ctrl = {0};
	
	for( i = 0; i < 2; i ++ ){
		
		if( !p0 ){
			continue;
		}
		if( !p1 ){
			continue;
		}
		
		wb_gain.gain_r = 0x100;
		wb_gain.gain_g = 0x100;
		wb_gain.gain_b = 0x100;
		wb_gain.clip_r = 0xFFF;
		wb_gain.clip_g = 0xFFF;
		wb_gain.clip_b = 0xFFF;
		Im_LTM_MAP_Set_WB_Gain( i, &wb_gain );
		
		mode.mode0 = 1; // edge weak
		mode.coef0 = 0; // edge weak
		mode.mode1 = 1; // edge weak
		mode.coef1 = 0; // edge weak
		mode.mode2 = 1; // edge weak
		mode.coef2 = 0; // edge weak
		Im_LTM_MAP_Ctrl_Generation_Mode( i, &mode );
		
		ylog_ctrl.use_tbl = D_IM_LTM_YLOG_TBL_SIDE_0;
		ylog_ctrl.end_point0_tbl0 = g_ltm_ylog_tbl_sample[D_IM_LTM_TABLE_MAX_YLOG-1];
		ylog_ctrl.end_point0_tbl1 = g_ltm_ylog_tbl_sample[D_IM_LTM_TABLE_MAX_YLOG-1];
		ylog_ctrl.end_point1_tbl0 = g_ltm_ylog_tbl_sample[D_IM_LTM_TABLE_MAX_YLOG-1];
		ylog_ctrl.end_point1_tbl1 = g_ltm_ylog_tbl_sample[D_IM_LTM_TABLE_MAX_YLOG-1];
		Im_LTM_MAP_Ctrl_Ylog_Tbl( i, &ylog_ctrl );
		Im_LTM_MAP_Set_YLOG_Table( i, D_LTM_YLOG_TBL_00, g_ltm_ylog_tbl_sample );
		Im_LTM_MAP_Set_YLOG_Table( i, D_LTM_YLOG_TBL_01, g_ltm_ylog_tbl_sample );
		Im_LTM_MAP_Set_YLOG_Table( i, D_LTM_YLOG_TBL_10, g_ltm_ylog_tbl_sample );
		Im_LTM_MAP_Set_YLOG_Table( i, D_LTM_YLOG_TBL_11, g_ltm_ylog_tbl_sample );
	}
}


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static VOID LTM_Ctrl_LTM_Handler(UINT32* int_status, UINT32 user_param)
{
}

static VOID LTM_Ctrl_Map_Handler( UINT32* result, UINT32 user_param )
{
	g_ltm_map_current_flg |= (*result & g_ltm_map_compare_flg);
	
	if( g_ltm_map_current_flg == g_ltm_map_compare_flg ){
		g_ltm_map_current_flg = 0;
		Snd_Event_to_System( E_SYSTEM_EVENT_DOL_COMBINE_MAP, g_ltm_map_cnt++ );
	}
}

static VOID LTM_Ctrl_Rbk_Handler( UINT32* result, UINT32 user_param )
{
	g_ltm_rbk_current_flg |= (*result & g_ltm_rbk_compare_flg);
	
	if( g_ltm_rbk_current_flg == g_ltm_rbk_compare_flg ){
		g_ltm_rbk_current_flg = 0;
		Snd_Event_to_System( E_SYSTEM_EVENT_DOL_COMBINE_RBK, g_ltm_rbk_cnt++ );
	}
}
