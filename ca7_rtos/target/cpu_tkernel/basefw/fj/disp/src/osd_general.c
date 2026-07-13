/**
 * @file		osd_general.c
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "osd_general.h"
#include "osd_attr.h"
#include "im_iip.h"
#include "fj_iip_resource_common.h"
#include "fj_iip_resource.h"
#include "l1l2cache.h"
#include "sdram_map_cache_define.h"
#include "im_jpeg.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_OSD_GENERAL_WAIT_TIME	(3000)	/** Use for output charactor by ASCII code */
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#ifdef __GNUC__
static BYTE param_buffer_addr[250] __attribute__((aligned(8)));	// For IIP
#else
__align(8) static BYTE param_buffer_addr[250];	// For IIP
#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
///////////////////////////////////////////////////
//  Following are BASEFW_BOOT_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_BOOT_CODE"
INT32 BF_Osd_General_Strnicmp(const CHAR* string1, const CHAR* string2, size_t count)
{
	INT32	ret = 0;
	INT32	i;
	CHAR	temp1, temp2;

	for (i = 0; i < count; i++) {
		temp1 = string1[i];
		if ((temp1 <= 'Z') && (temp1 >= 'A')) {
			temp1 += 0x20;
		}
		temp2 = string2[i];
		if ((temp2 <= 'Z') && (temp2 >= 'A')) {
			temp2 += 0x20;
		}
		ret = temp1 - temp2;
		if (ret != 0) {
			break;
		}
	}
	return ret;
}

VOID BF_Osd_General_Fill_Box(BYTE fill_data, ULONG gl_width, ULONG dst_addr,
							 ULONG fill_width, ULONG fill_lines)
{
	INT32 i;

	for (i = 0; i < fill_lines; i++) {
		memset((BYTE*)dst_addr, fill_data, fill_width);
		dst_addr += gl_width;
	}
}

VOID BF_Osd_General_Fill_Box_For_GR(OSD_USHORT_INT fill_data, ULONG gl_width, OSD_USHORT_INT* dst_addr,
									 ULONG fill_width, ULONG fill_lines)
{
#if (FJ_OSD_COMM_RGBAXXXX == 2)
	ULONG end_fill_width = 0; 
	ULONG start_fill_width = 0;
	UINT32 *int_address;
	UINT32 int_fill_data = fill_data | (fill_data <<16);

	if(((UINT32)dst_addr) & 3 ) {
		//Not 4 byte starting address
		fill_width --;
		start_fill_width = 1;
	}
	end_fill_width = (fill_width & 1);
	fill_width -= end_fill_width;
	fill_width >>=1;

	for (UINT32 i = fill_lines; i > 0; i--) {
		if(start_fill_width) {
			*dst_addr = fill_data;
			int_address =(UINT32*)(dst_addr +1);
		}
		else{
			int_address =(UINT32*)dst_addr;
		}
		for (UINT32 j = fill_width; j > 0; j--) {
			*int_address = int_fill_data;
			int_address++;
		}
		if(end_fill_width) {
			OSD_USHORT_INT* end_address =(OSD_USHORT_INT*)(int_address);
			*end_address = fill_data;
		}
		dst_addr += gl_width;
	}
#else
	for (UINT32 i = fill_lines; i > 0; i--) {
		for (UINT32 j = 0; j < fill_width; j++) {
			*(dst_addr + j) = fill_data;
		}
		dst_addr += gl_width;
	}
#endif
}

VOID BF_Osd_General_Fill_Box_Part_For_GR(OSD_USHORT_INT set_data, ULONG gl_width, OSD_USHORT_INT* dst_addr,
										  E_BF_OSD_GENERAL_RGBA_MASK bit_patten, ULONG fill_width, ULONG fill_lines)
{
	INT32 i, j;

	for (i = fill_lines; i > 0; i--) {
		for (j = 0; j < fill_width; j++) {
			register OSD_USHORT_INT org_data = *(dst_addr + j);
			*(dst_addr + j) = (org_data & (~bit_patten)) | set_data;
		}
		dst_addr += gl_width;
	}
}

VOID BF_Osd_General_Copy_To_Box(ULONG src_addr, USHORT dst_gl_width, ULONG dst_addr, USHORT width, USHORT lines )
{
	INT32 i;

	for (i = 0; i < lines; i++) {
		memcpy((UCHAR*)dst_addr, (UCHAR*)src_addr, width);
		dst_addr += dst_gl_width;
		src_addr += width;
	}
}

VOID BF_Osd_General_Copy_Box_To_Box(ULONG src_addr, USHORT src_gl_width, USHORT dst_gl_width, ULONG dst_addr, USHORT width, USHORT lines )
{
	INT32 i;

	for (i = 0; i < lines; i++) {
		memcpy((UCHAR*)dst_addr, (UCHAR*)src_addr, width);
		dst_addr += dst_gl_width;
		src_addr += src_gl_width;
	}
}

UINT32 BF_Osd_General_greatest_common_divisor( UINT32 a,  UINT32 b)
{
	 UINT32 ret ;
	if (b == 0) {
		// Cannot 0 division.
		ret = 1;
	}
	else{
		while (1) {
			if (!(a %= b)) {
				ret = b;
				break;
			}
			if (!(b %= a)) {
				ret = a;
				break;
			}
		}
	}
	return ret;
}

VOID BF_Osd_General_Fraction(UINT32* a, UINT32* b)
{
	UINT32 greatest_common_divisor = BF_Osd_General_greatest_common_divisor(*a,  *b);
	*a /= greatest_common_divisor;
	*b /= greatest_common_divisor;
}

///////////////////////////////////////////////////
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_COMM_CODE"
VOID BF_Osd_General_Conv_RGBA8888_To_RGBA4444(UINT32* vgr_address)
{
	INT32 i;
	UINT32* rgb_4444 = (UINT32*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG((ULONG)vgr_address);
	
	BF_L1l2cache_Flush_Addr((ULONG)vgr_address, (WIDTH_VGA * LINES_VGA / 2));

	for (i = (WIDTH_VGA * LINES_VGA / 2); i > 0; i--) {
		register unsigned int rgba8888 = (*vgr_address);
		// RrGgBbAa->R0000000
		register unsigned int rgba_4bit = (rgba8888 & 0xf0000000);
		// RrGgBbAa->00G0 0000->0G00 0000->RG00 0000
		rgba_4bit |= ((rgba8888 & 0x00f00000) << 4);
		// RrGgBbAa->0000 B000->00B0 0000->RGB0 0000
		rgba_4bit |= ((rgba8888 & 0x0000f000) << 8);
		// RrGgBbAa->0000 00A0->000A 0000->RGBA 0000
		rgba_4bit |= ((rgba8888 & 0x000000f0) << 12);

		vgr_address++;

		rgba8888 = (*vgr_address);
		// RrGgBbAa->R0000000->0000 R000
		rgba_4bit |= (rgba8888 & 0xf0000000) >> 16;
		// RrGgBbAa->00G0 0000->0000 0G00->xxxx RG00
		rgba_4bit |= ((rgba8888 & 0x00f00000) >> 12);
		// RrGgBbAa->0000 B000->0000 00B0->xxxx RGB0
		rgba_4bit |= ((rgba8888 & 0x0000f000) >> 8);
		// RrGgBbAa->0000 00A0->0000 000A->xxxx RGBA
		rgba_4bit |= ((rgba8888 & 0x000000f0) >> 4);
		*rgb_4444 = rgba_4bit;
		vgr_address++;
		rgb_4444++;
	}
}

UINT32 BF_Osd_General_Get_Bit_Position(UINT32  bitmask, UINT32* bit_pos, UINT32 bit_pos_num)
{
	UINT32 real_num = 0;
	for(UINT32 i = 0; i < bit_pos_num; i++) {
		UINT32 bit_patten = (1<<i );
		if( bitmask & bit_patten) {
			bit_pos[real_num] = i;
			real_num++;
		}
	}
	return real_num;
}

INT32 BF_Osd_General_Conv_YCC420_To_RGBA( const T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG *jpgmng,
					const T_Rect *screen_rect_data, USHORT dst_size[2], ULONG rgb_address)
{
	INT32 ercd ;
	T_IM_IIP_UTIL_CSC  cfg0;
	T_FJ_IIP_RES_CONFIG_OUT ip_iip_res;

	//1D----CSC----SL
	//0
	T_FJ_IIP_RES_CONFIG_ARG config_arg = {
		.unit = {
			.oned	= 1,	.flt	= 0,	.afn_0	= 0,	.afn_1	= 0,	.afn_2	= 0,	.afn	= 0,
			.frect	= 0,	.mon	= 0,	.csc	= 1,	.lut	= 0,	.gpc	= 0,	.blend	= 0,
			.cfl	= 0,	.mft	= 0,	.sl		= 1,	.sl_0_2	= 0,	.sl_3_8	= 0,
			.sl0	= 0,	.sl1	= 0,	.sl2	= 0,	.sl3	= 0,	.sl4	= 0,	.sl5	= 0,
			.sl6	= 0,	.sl7	= 0,	.sl8	= 0,	.sl_mon = 0,
			.sl_master_if_0 = 0,	.sl_master_if_1 = 0,	.sl_master_if_2 = 0,
		},
		.pixid = 2,			// request 2 pixids.
		.other = D_FJ_IIP_RES_OTHER_NONE
	};

	// Hunt IIP Unit and PIXFMTTBL.
	ercd = FJ_IIP_Res_Config_Lock_Host( &ip_iip_res, &config_arg, 1, D_OSD_GENERAL_WAIT_TIME );
	if( ercd == FJ_ERR_OK ) {
		UINT32 unitid[32];
		UINT32 unitid_num = 64;
		UINT32 bitmask;
		//Get 1D Unit
		bitmask = (ip_iip_res.unitid_bitmask64 & (D_IM_IIP_PARAM_PLDUNIT_LD0 | D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_LD2));
		BF_Osd_General_Get_Bit_Position(bitmask, unitid, unitid_num);
		cfg0.ld_unitid = (E_IM_IIP_UNIT_ID)unitid[0];

		//Get SL Unit
		bitmask = (ip_iip_res.unitid_bitmask64 & (D_IM_IIP_PARAM_PLDUNIT_CSC0 | D_IM_IIP_PARAM_PLDUNIT_CSC1 | D_IM_IIP_PARAM_PLDUNIT_CSC2 | D_IM_IIP_PARAM_PLDUNIT_CSC3));
		BF_Osd_General_Get_Bit_Position(bitmask, unitid, unitid_num);
		cfg0.csc_unitid = (E_IM_IIP_UNIT_ID)unitid[0];

		//Get SL Unit
		bitmask = (ip_iip_res.unitid_bitmask64 & (D_IM_IIP_PARAM_PLDUNIT_SL2 | D_IM_IIP_PARAM_PLDUNIT_SL3 | D_IM_IIP_PARAM_PLDUNIT_SL4 | D_IM_IIP_PARAM_PLDUNIT_SL5 | D_IM_IIP_PARAM_PLDUNIT_SL6));
		BF_Osd_General_Get_Bit_Position(bitmask, unitid, unitid_num);
		cfg0.sl_unitid = (E_IM_IIP_UNIT_ID)unitid[0];

		cfg0.alpha_depth = E_IM_IIP_ADEPTH_8BITS;
		cfg0.alpha_in_sel = E_IM_IIP_PARAM_ALPSEL_ALPVAL_REG;//Fix value
		cfg0.alpha_out_sel = E_IM_IIP_PARAM_ALOSEL_CALCULATED;
		cfg0.alpha_val = 0xFF;//Fix value
		cfg0.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP;
		cfg0.csck_mode_sel = 1;//Sxx.yyyyyyyyyyyy Int:2, decimal fraction:12
		cfg0.csck[0][0] = 0x1000; // +64/64                                          (G)
		cfg0.csck[0][1] = 0x7A80; // -22/64=-0.34375
		cfg0.csck[0][2] = 0x74C0; // -45/64=-0.703125
		cfg0.csck[0][3] = 0x0868; // (-128*(-22/64-45/64))/255=0.525490196
		cfg0.csck[1][0] = 0x1000; //                                                 (B)
		cfg0.csck[1][1] = 0x1C40; // +113/64=1.765625
		cfg0.csck[1][2] = 0x7FC0; // -1/64=-0.015625
		cfg0.csck[1][3] = 0x71F2; // (-128*(113/64-1/64))/255=-0.87843137254901960784313725490196
		cfg0.csck[2][0] = 0x1000; // +64/64                                          (R)
		cfg0.csck[2][1] = 0x0000; // 0
		cfg0.csck[2][2] = 0x1680; // +90/64=1.40625
		cfg0.csck[2][3] = 0x74B5; // (-128*(90/64))/255=-0.705882353
		cfg0.csck[3][0] = 0x0000; //                                                 (A)
		cfg0.csck[3][1] = 0x0000;
		cfg0.csck[3][2] = 0x0000;
		cfg0.csck[3][3] = 0x1000;

		cfg0.src.rect.left =
		cfg0.src.rect.top = 0;
		cfg0.dst.rect.left = screen_rect_data->Postion.StartX;
		cfg0.dst.rect.top = screen_rect_data->Postion.StartY;
		cfg0.src.gbl.line_bytes.Y_G = 
		cfg0.src.gbl.line_bytes.Cb_B = 
		cfg0.src.gbl.line_bytes.Cr_R = //dumy
		cfg0.src.gbl.line_bytes.Alpha = //dumy
		cfg0.src.gbl.width = jpgmng->gl_width;
		cfg0.src.rect.width = 
		cfg0.dst.rect.width = dst_size[0];
		cfg0.src.gbl.lines = jpgmng->lines;
		cfg0.src.rect.lines =
		cfg0.dst.rect.lines = dst_size[1];
		cfg0.dst.gbl.addr.Y_G = rgb_address;
		cfg0.dst.gbl.alpha = D_IM_IIP_ALPHA_TRUE;
		//Oder: YCC
		cfg0.src.gbl.chunky_color.component0 = 
		cfg0.src.gbl.chunky_color.component1 = 
		cfg0.src.gbl.chunky_color.component2 = 
		cfg0.src.gbl.chunky_color.component3 = 0;
		//Oder: RGBA
		cfg0.dst.gbl.chunky_color.component0 = D_IM_IIP_CHUNKY_COLOR_Y1_A;
		cfg0.dst.gbl.chunky_color.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B;
		cfg0.dst.gbl.chunky_color.component2 = D_IM_IIP_CHUNKY_COLOR_Y0_G;
		cfg0.dst.gbl.chunky_color.component3 = D_IM_IIP_CHUNKY_COLOR_CR_R;
		cfg0.dst.gbl.frame_type = E_IM_IIP_FTYPE_CHUNKY;
		cfg0.dst.gbl.width = screen_rect_data->Width;
		cfg0.dst.gbl.lines = screen_rect_data->Height;
		cfg0.dst.gbl.line_bytes.Y_G = screen_rect_data->Width * sizeof(OSD_USHORT_INT);
		if(cfg0.csc_unitid == E_IM_IIP_UNIT_ID_SL6) {
			cfg0.dst.gbl.masterIF_Y_G = 1;//It can be set to 1 only;
		}
		else{
			cfg0.dst.gbl.masterIF_Y_G = 0;
		}
#if FJ_OSD_COMM_RGBAXXXX == 2
		cfg0.dst.gbl.pix_format = E_IM_IIP_PFMT_RGBA4444;
		cfg0.sl_sftval = 4;
#else
		cfg0.dst.gbl.pix_format = E_IM_IIP_PFMT_YCC444;
		cfg0.sl_sftval = 0;
#endif
		cfg0.dst.gbl.sign_Y_G = D_IM_IIP_UNSIGNED_DATA;
		cfg0.dst.gbl.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA;
		cfg0.dst.gbl.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA;
		cfg0.dst.gbl.sign_D3 = D_IM_IIP_UNSIGNED_DATA;

		UINT32 pixid[2];
		//Get Pixid
		BF_Osd_General_Get_Bit_Position(ip_iip_res.pixid_bitmask, pixid, sizeof(pixid)/sizeof(pixid[0]));
		cfg0.dst_pixid = (E_IM_IIP_PIXID)(1 << pixid[1]);
		cfg0.ld_cache_select = E_IM_IIP_PARAM_CSEL_1;
		cfg0.pix_depth = D_IM_IIP_PDEPTH_8BITS;

		cfg0.src.gbl.addr.Y_G = jpgmng->ycbcr_addr[0];
		cfg0.src.gbl.addr.Cb_B = jpgmng->ycbcr_addr[1];
		cfg0.src.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
		cfg0.src.gbl.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR;
		cfg0.src.gbl.masterIF_Y_G = 0;
		cfg0.src.gbl.masterIF_Cb_B =0;
		cfg0.src.gbl.masterIF_Cr_R =0;
		if((E_IM_JPEG_SMPL_TYPE)jpgmng->ycbcr_smpl == E_IM_JPEG_SMPL_TYPE_YCC422){
			cfg0.src.gbl.pix_format = E_IM_IIP_PFMT_YCC422;
		}
		else{
			cfg0.src.gbl.pix_format = E_IM_IIP_PFMT_YCC420;
		}

		cfg0.src.gbl.sign_Y_G = D_IM_IIP_UNSIGNED_DATA;
		cfg0.src.gbl.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA;
		cfg0.src.gbl.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA;
		cfg0.src.gbl.sign_D3 = D_IM_IIP_UNSIGNED_DATA;
		cfg0.src_pixid = (E_IM_IIP_PIXID)(1 << pixid[0]);

		ercd = IM_IIP_Utility_CSC(&cfg0, NULL, D_IM_IIP_MASTER_IF0, (ULONG)param_buffer_addr, D_OSD_GENERAL_WAIT_TIME );

		// Release IIP Unit and PIXFMTTBL.
		FJ_IIP_Res_Config_Free_Host( &ip_iip_res, 1, D_OSD_GENERAL_WAIT_TIME );
	}
	return ercd;
}

INT8 BF_Osd_General_Conv_Layer_To_Define(E_OSDLayer layer_id)
{
	INT8 ret = -1;

	switch (layer_id){
		case E_OSDLayer_LCD_OSD0:
		case E_OSDLayer_HDMI_OSD0:
			ret = D_IM_DISP_BLD_OSD0;
			break;

		case E_OSDLayer_LCD_OSD1:
		case E_OSDLayer_HDMI_OSD1:
			ret = D_IM_DISP_BLD_OSD1;
			break;

		case E_OSDLayer_LCD_GRID:
		case E_OSDLayer_HDMI_GRID:
			ret = D_IM_DISP_BLD_GRID;
			break;

		case E_OSDLayer_LCD_FACE:
		case E_OSDLayer_HDMI_FACE:
			ret = D_IM_DISP_BLD_FACE;
			break;

		default:
			break;
	}
	return ret;
}

E_OSDLayer BF_Osd_General_Conv_Define_To_Layer(FJ_DISP_TYPE disp_type, INT8 layer_id)
{
	E_OSDLayer ret = E_OSDLayer_LCD_OSD0;

	if( disp_type == FJ_DISP_TYPE_LCD ){
		if( (BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_LCD) == E_FJ_DISP_IFS_LCD) ||
			(BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_LCD) == E_FJ_DISP_IFS_MIPI) ) {
			switch (layer_id){
				case D_IM_DISP_BLD_OSD0:
					ret = E_OSDLayer_LCD_OSD0;
					break;
	
				case D_IM_DISP_BLD_OSD1:
					ret = E_OSDLayer_LCD_OSD1;
					break;
	
				case D_IM_DISP_BLD_GRID:
					ret = E_OSDLayer_LCD_GRID;
					break;
	
				case D_IM_DISP_BLD_FACE:
					ret = E_OSDLayer_LCD_FACE;
					break;
	
				default:
					break;
			}
		}
		else{
			ret = 0;
		}
	}
	else{
		if (BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_HDMI) == E_FJ_DISP_IFS_HDMI) {
			switch (layer_id){
				case D_IM_DISP_BLD_OSD0:
					ret = E_OSDLayer_HDMI_OSD0;
					break;
	
				case D_IM_DISP_BLD_OSD1:
					ret = E_OSDLayer_HDMI_OSD1;
					break;
	
				case D_IM_DISP_BLD_GRID:
					ret = E_OSDLayer_HDMI_GRID;
					break;
	
				case D_IM_DISP_BLD_FACE:
					ret = E_OSDLayer_HDMI_FACE;
					break;
	
				default:
					break;
			}
		}
		else{
			ret = 0;
		}
	}
	return ret;
}

