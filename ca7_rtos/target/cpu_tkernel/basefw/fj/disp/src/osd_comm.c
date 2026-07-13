/**
 * @file		osd_comm.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
// This file will be included to fj_osd.c. The section define is belong to that file
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "fj_custom.h"	// enable BF_Debug_Print
#include "osd_attr.h"
#include "osd_config_comm.h"
#include "osd_draw_comm.h"
#include "osd_histogram_comm.h"
#include "osd_icon_comm.h"
#include "osd_image_comm.h"
#include "osd_shape_comm.h"
#include "dd_top.h"
#include "l1l2cache.h"
// delete\basefw\fj\imgproc\still
//#include "still_sync_ctrl.h"
#include "mba_if.h"
#include "sdram_map_cache_define.h"

// Get Chip Version Begin
#include "fj_common.h"
// Get Chip Version End

 
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_OSD_COM_GRISIZE_IHSIZE_MIN	(8)		/**< GRISIZE.IHSIZE min */
#define D_OSD_COM_GRISIZE_IVSIZE_MIN	(8)		/**< GRISIZE.IVSIZE min */
#define D_OSD_COM_FFSIZE_FFHSZ_MIN		(8)		/**< FFSIZE.FFHSZ min */
#define D_OSD_COM_FFSIZE_FFVSZ_MIN		(8)		/**< FFSIZE.FFVSZ min */
#define D_OSD_COM_FFWIDTH_FFHWID_MIN	(2)		/**< FFWIDTH.FFHWID min */
#define D_OSD_COM_FFWIDTH_FFVWID_MIN	(2)		/**< FFWIDTH.FFVWID min */

#define D_OSD_COM_GWIDTH_GHWID_MIN		(2)		/**< GWIDTH.GHWID min */
#define D_OSD_COM_GWIDTH_GVWID_MIN		(2)		/**< GWIDTH.GVWID min */
#define D_OSD_COM_GLENGTH_GHLEN_MIN		(8)		/**< GLENGTH.GHLEN min */
#define D_OSD_COM_GLENGTH_GVLEN_MIN		(8)		/**< GLENGTH.GVLEN min */
#define D_OSD_COM_GITVL_GHITV_MIN_BASE	(2)		/**< GITVL.GHITV min */
#define D_OSD_COM_GITVL_GVITV_MIN_BASE	(2)		/**< GITVL.GVITV min */

#define D_OSD_COM_DISP_BANK_MAX			(4)

/* HDMI area */
#define SDRAM_WIDTH_GUI_WORK_HDMI                                  (0)
#define SDRAM_LINES_GUI_WORK_HDMI                                  (0)
#define SDRAM_BNK_GUI_WORK_HDMI                                    (1)
#define SDRAM_ADR_GUI_WORK_HDMI                                    (0xA0000000)    //(Dummy Area)Work buffer 

#define SDRAM_BNK_GUI_WORK_HDMI_END                                (0)
#define SDRAM_ADR_GUI_WORK_HDMI_END                                (0xA0000000)    //(Dummy Area)

#define SDRAM_WIDTH_HDMI_OSD0                                      (1920)
#define SDRAM_LINES_HDMI_OSD0                                      (1088)
#define SDRAM_BNK_HDMI_OSD0                                        (3)
#define SDRAM_SIZ_HDMI_OSD0                                        (0x7F8000)
#define SDRAM_ADR_HDMI_OSD0_0                                      (0xA0000000)    //(Dummy Area)HDMI OSD0 Display buffer 
#define SDRAM_ADR_HDMI_OSD0_1                                      (0xA07F8000)    //(Dummy Area)HDMI OSD0 Display buffer 
#define SDRAM_ADR_HDMI_OSD0_2                                      (0xA0FF0000)    //(Dummy Area)HDMI OSD0 Display buffer 

#define SDRAM_BNK_HDMI_OSD0_END                                    (0)
#define SDRAM_ADR_HDMI_OSD0_END                                    (0xA17E8000)    //(Dummy Area)

#define SDRAM_WIDTH_HDMI_OSD1                                      (1920)
#define SDRAM_LINES_HDMI_OSD1                                      (1088)
#define SDRAM_BNK_HDMI_OSD1                                        (3)
#define SDRAM_SIZ_HDMI_OSD1                                        (0x7F8000)
#define SDRAM_ADR_HDMI_OSD1_0                                      (0xA17E8000)    //(Dummy Area)HDMI OSD1 Display buffer 
#define SDRAM_ADR_HDMI_OSD1_1                                      (0xA1FE0000)    //(Dummy Area)HDMI OSD1 Display buffer 
#define SDRAM_ADR_HDMI_OSD1_2                                      (0xA27D8000)    //(Dummy Area)HDMI OSD1 Display buffer 

#define SDRAM_BNK_HDMI_OSD1_END                                    (0)
#define SDRAM_ADR_HDMI_OSD1_END                                    (0xA2FD0000)    //(Dummy Area)

#define SDRAM_BNK_HDMI_MAIN                                        (3)

/* LCD area */
#define SDRAM_BNK_LCD_MAIN_END                                     (1)

#define SDRAM_WIDTH_LCD_OSD0                                       (640)
#define SDRAM_LINES_LCD_OSD0                                       (480)
#define SDRAM_BNK_LCD_OSD0                                         (3)
#define SDRAM_SIZ_LCD_OSD0                                         (0x12C000)
#define SDRAM_ADR_LCD_OSD0_0                                       (0xA0151800)    //(Dummy Area)LCD OSD0 display buffer
#define SDRAM_ADR_LCD_OSD0_1                                       (0xA027D800)    //(Dummy Area)LCD OSD0 display buffer
#define SDRAM_ADR_LCD_OSD0_2                                       (0xA03A9800)    //(Dummy Area)LCD OSD0 display buffer

#define SDRAM_BNK_LCD_OSD0_END                                     (0)
#define SDRAM_ADR_LCD_OSD0_END                                     (0xA04D5800)    //(Dummy Area)

#define SDRAM_WIDTH_LCD_OSD1                                       (640)
#define SDRAM_LINES_LCD_OSD1                                       (480)
#define SDRAM_BNK_LCD_OSD1                                         (3)
#define SDRAM_SIZ_LCD_OSD1                                         (0x12C000)
#define SDRAM_ADR_LCD_OSD1_0                                       (0xA04D5800)    //(Dummy Area)LCD OSD1 display buffer
#define SDRAM_ADR_LCD_OSD1_1                                       (0xA0601800)    //(Dummy Area)LCD OSD1 display buffer
#define SDRAM_ADR_LCD_OSD1_2                                       (0xA072D800)    //(Dummy Area)LCD OSD1 display buffer

#define SDRAM_BNK_LCD_OSD1_END                                     (0)
#define SDRAM_ADR_LCD_OSD1_END                                     (0xA0859800)    //(Dummy Area)


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
/** Inverse gamma R */
static volatile USHORT g_inverse_r[256] = {0,};
/** Inverse gamma G */
static volatile USHORT g_inverse_g[256] = {0,};
/** Inverse gamma B */
static volatile USHORT g_inverse_b[256] = {0,};

/** Tone table */
static volatile USHORT g_tc[256] = {0,};
/** Tcep table */
static volatile USHORT g_tcep[4] = {0,};

/** Full gamma R */
static volatile USHORT g_gamma_r_in[128] = {0,};
/** Full gamma G */
static volatile USHORT g_gamma_g_in[128] = {0,};
/** Full gamma B */
static volatile USHORT g_gamma_b_in[128] = {0,};

/** Diff gamma R */
static volatile UCHAR g_gamma_r_out[128] = {0,};
/** Diff gamma G */
static volatile UCHAR g_gamma_g_out[128] = {0,};
/** Diff gamma B */
static volatile UCHAR g_gamma_b_out[128] = {0,};

/** Display stream */
static UINT32 gDisplay_stream = 0;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define M_OSD_COM_DISP_BANK_INC(val)	((E_IM_DISP_BANK)(((UINT32)(val) +1) % D_OSD_COM_DISP_BANK_MAX))

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static FJ_ERR_CODE fj_osd_set_osd_area_enable(E_OSDLayer layer_id, UINT32 enable_area);
static FJ_ERR_CODE osd_comm_draw_area_overlap_check(const T_Rect drawArea[OSD_DRAW_AREA_NUM], BYTE drawAreaNum);
static FJ_ERR_CODE osd_comm_draw_area_size_check(U_IM_DISP_SIZE territory_size, const T_Rect* drawArea);
static FJ_ERR_CODE osd_comm_draw_area_pos_check(const T_Rect* last_drawArea, const T_Rect* drawArea);
static FJ_ERR_CODE osd_comm_draw_face_frame_param_check(const T_IM_DISP_FACE_FRAME_PARAM* face_param, ULONG ovsize, ULONG ohsize);
static FJ_ERR_CODE osd_comm_draw_grid_line_param_check(E_IM_DISP_SEL block, T_IM_DISP_GRID const *const grid);

///////////////////////////////////////////////////
//  Following are BASEFW_BOOT_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_BOOT_CODE"
/**
Flush drawing buffer.
@param[in]	switch_buffer_infor	: It specified which layer would be flush.<br>
@param[in]	switch_buffer_infor_size : switch_buffer_infor array's size.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_flush_draw_area(const FJ_OSD_COMM_FLUSH_DARA_AREA_INFO* switch_buffer_infor,UINT32 switch_buffer_infor_size)
{
	FJ_ERR_CODE  ret = FJ_ERR_OK;
	E_OSDLayer flush_layer_osd[] = { E_OSDLayer_LCD_OSD0,
									 E_OSDLayer_LCD_OSD1,
									 E_OSDLayer_HDMI_OSD0,
									 E_OSDLayer_HDMI_OSD1 };
	E_OSDLayer flush_layer_main[] = { E_OSDLayer_LCD_MAIN,
									  E_OSDLayer_HDMI_MAIN };
	T_IM_DISP_IMAGE_ADDR main_ycbcr_address[2] = {{ 0, 0 }, { 0, 0 }};
	U_IM_DISP_SIZE input_size[2] = { {0}, {0} };
	T_BF_SDRAM_MAP_COMMON_OSD switch_osd_buffer[4] = {{E_OSDLayer_LCD_OSD0, E_IM_DISP_SEL_LAYER_MAIN, E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_LCD, E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0, 0},
													  {E_OSDLayer_LCD_OSD0, E_IM_DISP_SEL_LAYER_MAIN, E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_LCD, E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0, 0},
													  {E_OSDLayer_LCD_OSD0, E_IM_DISP_SEL_LAYER_MAIN, E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_LCD, E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0, 0},
													  {E_OSDLayer_LCD_OSD0, E_IM_DISP_SEL_LAYER_MAIN, E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_LCD, E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0, 0}};
	T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;
	FJ_DISP_TYPE disp_type;
	E_IM_DISP_SEL block;

#ifndef DEBUG_ON_PC
//	BF_L1l2cache_Clean_Addr( SDRAM_ADR_LCD_MAIN_0, SDRAM_ADR_HDMI_MAIN_END-SDRAM_ADR_LCD_MAIN_0 );
#endif // DEBUG_ON_PC
	//Buffer switch
	for(UINT32 i = 0; i < switch_buffer_infor_size; i++){
		for(UINT32 j = 0; j < BF_Osd_General_GET_ARRAY_ELEMENT_NUMS(flush_layer_main); j++){
			if(( switch_buffer_infor[i].flush_layers & flush_layer_main[j] ) == flush_layer_main[j]){

				if ( flush_layer_main[j] == E_OSDLayer_LCD_MAIN ) {
					disp_type = FJ_DISP_TYPE_LCD;
					block = E_IM_DISP_LCD_MIPI;
				}
				else {
					disp_type = FJ_DISP_TYPE_HDMI;
					block = E_IM_DISP_HDMI;
				}

				//Flush main layer
				Sdram_Map_Osd_Get_VDE_Chg_Addr(disp_type, &main_ycbcr_address[j].y_addr, &main_ycbcr_address[j].c_addr);
				E_IM_DISP_BANK bank_no;
				Im_DISP_Get_Main_Bank(block, &bank_no);
				if(bank_no == E_IM_DISP_BANK_11) {
					bank_no = E_IM_DISP_BANK_00;
				}
				else {
					bank_no++;
				}
				Im_DISP_Set_Addr(block, bank_no, &main_ycbcr_address[j]);//Set address to next bank
				//Switch to next bank
				Im_DISP_Set_Main_Bank(block, bank_no);
				Im_DISP_Get_Input_Size(block, &input_size[j]);
// --- REMOVE_MBA BEGIN ---
				M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_LCD, FJ_MBALOG_TYPE_POINT, "BANK=%u (%08lX)", bank_no, main_ycbcr_address[j].y_addr ));
// --- REMOVE_MBA END ---
#ifdef DEBUG_ON_PC
				BF_Osd_Config_Comm_Debug_Display_Parameter();
#endif // DEBUG_ON_PC
			}
		}
		for(UINT32 j = 0; j < BF_Osd_General_GET_ARRAY_ELEMENT_NUMS(flush_layer_osd); j++){
			if(( switch_buffer_infor[i].flush_layers & flush_layer_osd[j] ) == flush_layer_osd[j]){

				if ( ( flush_layer_osd[j] == E_OSDLayer_LCD_OSD0 ) || ( flush_layer_osd[j] == E_OSDLayer_LCD_OSD1 ) ) {
					block = E_IM_DISP_LCD_MIPI;
				}
				else {
					block = E_IM_DISP_HDMI;
				}

				//Flush OSD
				BF_Osd_Draw_Comm_Get_Block_GR(flush_layer_osd[j], &switch_osd_buffer[j]);

				switch_osd_buffer[j].layer_name = flush_layer_osd[j];
				draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[switch_osd_buffer[j].disp_block_no][switch_osd_buffer[j].gr_no];
				UINT32 osd_vaild_area_num = draw_area_inf->vaild_area_num;
				for(switch_osd_buffer[j].draw_area = 0; switch_osd_buffer[j].draw_area < osd_vaild_area_num; switch_osd_buffer[j].draw_area++){
					if((switch_buffer_infor[i].flush_showing_area & (1<<switch_osd_buffer[j].draw_area)) > 0){
						ULONG osd_address = Sdram_Map_Osd_Get_OSD_Addr(&switch_osd_buffer[j]);
						//Switch draw area's address
						Im_DISP_Set_OSD_Area_Addr(
								block,
								switch_osd_buffer[j].layer,
								FJ_OSD_COMM_GET_DISP_OSD_BANK(switch_osd_buffer[j].draw_area),
								osd_address);
						//Change draw area's address
						Sdram_Map_Osd_Change_OSD_Area_Index(
								switch_osd_buffer[j].disp_block_no,
								switch_osd_buffer[j].gr_no,
								switch_osd_buffer[j].draw_area);
					}
				}
			}
		}
	}
	//Buffer copy
	for(UINT32 i = 0; i < switch_buffer_infor_size; i++) {
		for(UINT32 j = 0; j < BF_Osd_General_GET_ARRAY_ELEMENT_NUMS(flush_layer_main); j++) {
			if((switch_buffer_infor[i].flush_layers & flush_layer_main[j]) == flush_layer_main[j]){
				//Copy main layer
				if(switch_buffer_infor[i].Is_Copy_flush_showing_area > 0) {
					T_IM_DISP_IMAGE_ADDR copy_main_ycbcr_address_copy;

					if ( flush_layer_main[j] == E_OSDLayer_LCD_MAIN ) {
						disp_type = FJ_DISP_TYPE_LCD;
					}
					else {
						disp_type = FJ_DISP_TYPE_HDMI;
					}

					Sdram_Map_Osd_Get_VDE_Chg_Addr(disp_type, &copy_main_ycbcr_address_copy.y_addr, &copy_main_ycbcr_address_copy.c_addr);
#if CO_NOT_USE_DMA == 0
					Dd_HDMAC0_Copy_SDRAM_Sync(D_BF_OSD_GENERAL_DMA_CH_OSD,
									main_ycbcr_address[j].y_addr,
									copy_main_ycbcr_address_copy.y_addr,
								   input_size[j].size.width * input_size[j].size.lines,
								   D_DD_HDMAC0_WAITMODE_EVENT);
					Dd_HDMAC0_Copy_SDRAM_Sync(D_BF_OSD_GENERAL_DMA_CH_OSD,
									main_ycbcr_address[j].c_addr,
									copy_main_ycbcr_address_copy.c_addr,
								   input_size[j].size.width * input_size[j].size.lines / 2,
								   D_DD_HDMAC0_WAITMODE_EVENT);
#elif CO_NOT_USE_DMA == 1
					memcpy((BYTE*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(copy_main_ycbcr_address_copy.y_addr),
							(BYTE*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(main_ycbcr_address[j].y_addr),
							input_size[j].size.width * input_size[j].size.lines );
					memcpy((BYTE*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(copy_main_ycbcr_address_copy.c_addr),
							(BYTE*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(main_ycbcr_address[j].c_addr),
							input_size[j].size.width * input_size[j].size.lines / 2);
#else
					Im_Xch_Copy(E_IM_XCH_CH_SEL_0, main_ycbcr_address[j].y_addr,
							input_size[j].size.width, input_size[j].size.width,
							copy_main_ycbcr_address_copy.y_addr,
							input_size[j].size.width, input_size[j].size.lines);
					Im_Xch_Copy(E_IM_XCH_CH_SEL_0,main_ycbcr_address[j].c_addr,
							input_size[j].size.width, input_size[j].size.width,
							copy_main_ycbcr_address_copy.c_addr,
							input_size[j].size.width, input_size[j].size.lines / 2);
#endif
				}
			}
		}
		//Copy OSD
		for(UINT32 j = 0; j < BF_Osd_General_GET_ARRAY_ELEMENT_NUMS(flush_layer_osd); j++){
			if((switch_buffer_infor[i].flush_layers & flush_layer_osd[j] ) == flush_layer_osd[j]){
				draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[switch_osd_buffer[j].disp_block_no][switch_osd_buffer[j].gr_no];
				UINT32 osd_vaild_area_num = draw_area_inf->vaild_area_num;	/* pgr0000 */
				for( switch_osd_buffer[j].draw_area = 0; switch_osd_buffer[j].draw_area < osd_vaild_area_num; switch_osd_buffer[j].draw_area++){
					if(((switch_buffer_infor[i].flush_showing_area & (1<<switch_osd_buffer[j].draw_area)) > 0) &&
						((switch_buffer_infor[i].Is_Copy_flush_showing_area & (1<<switch_osd_buffer[j].draw_area)) > 0)){
						//Copy draw area's address
						UINT32 osd_draw_area_size = draw_area_inf->area_size[switch_osd_buffer[j].draw_area].Width *
									draw_area_inf->area_size[switch_osd_buffer[j].draw_area].Height * sizeof(OSD_USHORT_INT);


#if CO_NOT_USE_DMA == 0
					Dd_HDMAC0_Copy_SDRAM_Sync(D_BF_OSD_GENERAL_DMA_CH_OSD,
									BF_Sdram_Map_Osd_Get_Current_OSD_Addr(&switch_osd_buffer[j]),
									BF_Sdram_Map_Osd_Get_OSD_Addr(&switch_osd_buffer[j]),
								   osd_draw_area_size,
								   D_DD_HDMAC0_WAITMODE_EVENT);
#elif CO_NOT_USE_DMA == 1
					memcpy((BYTE*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(Sdram_Map_Osd_Get_OSD_Addr(&switch_osd_buffer[j])),
							(BYTE*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(Sdram_Map_Osd_Get_Current_OSD_Addr(&switch_osd_buffer[j])),
							osd_draw_area_size);

#else
					Im_Xch_Copy(E_IM_XCH_CH_SEL_0,
								BF_Sdram_Map_Osd_Get_OSD_Addr(&switch_osd_buffer[j])),
								input_size[j].size.width,
								input_size[j].size.width,
								BF_Sdram_Map_Osd_Get_Current_OSD_Addr(&switch_osd_buffer[j])),
								draw_area_inf->area_size[switch_osd_buffer[j].draw_area].Width,
								draw_area_inf->area_size[switch_osd_buffer[j].draw_area].Height);
#endif
					}
				}
			}
		}
	}

	return ret;
}

/**
Set the MAIN's input size.
@param[in] layer_id layer ID.
			@ref E_OSDLayer_LCD_MAIN		: LCD Main layer<br>
			@ref E_OSDLayer_HDMI_MAIN		: HDMI Main layer<br>
@param[in] disp_rect rect data.<br>
@retval FJ_ERR_OK	Normal End.
@retval FJ_ERR_NG	Abnormal End.
*/
static FJ_ERR_CODE fj_osd_set_main_input_size(E_OSDLayer layer_id, const T_Rect* disp_rect)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	E_FJ_DISP_IFS ifs = E_FJ_DISP_IFS_NONE;
	
	if( layer_id == E_OSDLayer_LCD_MAIN ){
		// LCD
		ifs = BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_LCD);
	}
	else{
		// HDMI
		ifs = BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_HDMI);
	}

	if( (ifs == E_FJ_DISP_IFS_LCD  && layer_id == E_OSDLayer_LCD_MAIN) ||
		(ifs == E_FJ_DISP_IFS_MIPI && layer_id == E_OSDLayer_LCD_MAIN) ||
		(ifs == E_FJ_DISP_IFS_HDMI && layer_id == E_OSDLayer_HDMI_MAIN) ){
		BF_Osd_Config_Comm_Change_Main_Size(layer_id, disp_rect);
	}
	else{
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	return ret;
}

/**
Load a icon data from file to be used for OSD drawing.
@param[in] FontName name of the icon file.
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
@retval FJ_ERR_OSD_FILE_ACCSESS	Abnormal End. Failed in the access to the file.
@retval FJ_ERR_OSD_FILE_SIZE	Abnormal End. The size of the file is too large.
@retval FJ_ERR_NG				Abnormal End.
@remarks FJ_SelectFontID or FJ_DirectSelectFont should be called to set the what ID is icon<br>
		before this API is called.
*/
static FJ_ERR_CODE fj_loadfontfile(CHAR const* const FontName)
{
	return FJ_DirectLoadFont(FontName);
}

/**
Clear draw buffer of OSD layer.
@param[in] layer_id layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
*/
static FJ_ERR_CODE fj_clear_osd_buffer(E_OSDLayer layer_id)
{
	BYTE* data_address = 0;
	UINT32 clear_size = 0;
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;

	FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);

	if(ret == FJ_ERR_OK){
		//Get vlaid draw area num
		draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no][disp_no_data.gr_no];
		INT32 vaild_area_num = draw_area_inf->vaild_area_num;
		for(INT32 i = 0; i < vaild_area_num; i++) {
			disp_no_data.draw_area = i;
			data_address = (BYTE*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(Sdram_Map_Osd_Get_OSD_Chg_Addr(&disp_no_data));
			clear_size = draw_area_inf->area_size[i].Width * draw_area_inf->area_size[i].Height * sizeof(OSD_USHORT_INT);
			memset(data_address, 0, clear_size);
		}
	}
	return ret;
}

/**
Get gain, clip, offset value setting at DISP macro.
@param[out] cal_data	clip cal data<br>
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG				Abnormal End. Mode check error.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
*/
static FJ_ERR_CODE fj_osd_get_clip(E_DISP_DEV disp_device, T_CLIP_CAL_DATA* cal_data)
{
	T_IM_DISP_CLIP_CAL clip_cal;
	E_IM_DISP_SEL disp_sel;
	FJ_ERR_CODE ret = FJ_ERR_OK;

	if(cal_data != NULL){

		if ( disp_device == E_DISP_DEV_LCD ) {
			disp_sel = E_IM_DISP_LCD_MIPI;
		}
		else {
			disp_sel = E_IM_DISP_HDMI;
		}

		Im_DISP_Get_Clip(disp_sel, &clip_cal);
		
		cal_data->y_cal.gain = clip_cal.y_cal.ygain;
		cal_data->y_cal.offset = clip_cal.y_cal.yofs;
		cal_data->y_clip.clip_h = clip_cal.y_clip.cph;
		cal_data->y_clip.clip_l = clip_cal.y_clip.cpl;
		cal_data->cb_cal.gain = clip_cal.cb_cal.cgain;
		cal_data->cb_cal.offset = clip_cal.cb_cal.cofs;
		cal_data->cb_clip.clip_h = clip_cal.cb_clip.cph;
		cal_data->cb_clip.clip_l = clip_cal.cb_clip.cpl;
		cal_data->cr_cal.gain = clip_cal.cr_cal.cgain;
		cal_data->cr_cal.offset = clip_cal.cr_cal.cofs;
		cal_data->cr_clip.clip_h = clip_cal.cr_clip.cph;
		cal_data->cr_clip.clip_l = clip_cal.cr_clip.cpl;
	}
	else{
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	
	return ret;
}

/**
Set gain, clip, offset value setting at DISP macro.
@param[in] cal_data gain, clip, offset value. See @ref T_CLIP_CAL_DATA.
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG				Mode check error.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
*/
static FJ_ERR_CODE fj_osd_set_clip(E_DISP_DEV disp_device, T_CLIP_CAL_DATA const* const cal_data)
{
	E_IM_DISP_SEL disp_sel;
	T_IM_DISP_CLIP_CAL clip_cal;
	FJ_ERR_CODE ret = FJ_ERR_OK;

	if(cal_data != NULL){

		if ( disp_device == E_DISP_DEV_LCD ) {
			disp_sel = E_IM_DISP_LCD_MIPI;
		}
		else {
			disp_sel = E_IM_DISP_HDMI;
		}

		clip_cal.y_cal.ygain = cal_data->y_cal.gain;
		clip_cal.y_cal.yofs = cal_data->y_cal.offset;
		clip_cal.y_clip.cph = cal_data->y_clip.clip_h;
		clip_cal.y_clip.cpl = cal_data->y_clip.clip_l;
		clip_cal.cb_cal.cgain = (USHORT)cal_data->cb_cal.gain;
		clip_cal.cb_cal.cofs = (SHORT)cal_data->cb_cal.offset;
		clip_cal.cb_clip.cph = cal_data->cb_clip.clip_h;
		clip_cal.cb_clip.cpl = cal_data->cb_clip.clip_l;
		clip_cal.cr_cal.cgain = (USHORT)cal_data->cr_cal.gain;
		clip_cal.cr_cal.cofs = (SHORT)cal_data->cr_cal.offset;
		clip_cal.cr_clip.cph = cal_data->cr_clip.clip_h;
		clip_cal.cr_clip.cpl = cal_data->cr_clip.clip_l;

		Im_DISP_Set_Clip(disp_sel, &clip_cal);
	}
	else{
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	
	return ret;
}

/**
Set Back Ground Color.
@param[in] disp_device	LCD(0) or HDMI(1)
@param[in] r_color	R color. Value range:0~255.
@param[in] g_color	G color. Value range:0~255.
@param[in] b_color	B color. Value range:0~255.
@remarks However, it just keep it to system attribute data in this API.<br>
		@ref FJ_Show_BackColor API has to be called after this API is called <br>
		the display of BackGround Color want to be changed<br>
*/
static VOID fj_show_setbackcolor(E_DISP_DEV disp_device, UCHAR r_color, UCHAR g_color, UCHAR b_color)
{
	U_IM_DISP_IMAGE_COLOR bb_ctrl;
	E_IM_DISP_SEL disp_sel;

	bb_ctrl.word = 0;

	if ( disp_device == E_DISP_DEV_LCD ) {
		disp_sel = E_IM_DISP_LCD_MIPI;
	}
	else {
		disp_sel = E_IM_DISP_HDMI;
	}

	// Enable black back.
	bb_ctrl.rgb.r = r_color;
	bb_ctrl.rgb.g = g_color;
	bb_ctrl.rgb.b = b_color;

	// Set Black back color to LV and VV channel.
	Im_DISP_Change_BB_Color(disp_sel, bb_ctrl);
}

/**
Enable show or not display Back Ground Color.
@param[in] enable	enable/disable flag(TRUE:BackColor ON, FALSE:BackColor OFF).
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
static FJ_ERR_CODE fj_show_backcolor(E_DISP_DEV disp_device, BOOL enable)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	E_IM_DISP_SEL disp_sel;
	U_IM_DISP_LALP lalp;

	if ( disp_device == E_DISP_DEV_LCD ) {
		disp_sel = E_IM_DISP_LCD_MIPI;
	}
	else {
		disp_sel = E_IM_DISP_HDMI;
	}

	if ( enable == 1 ) {
		lalp.bit.ALP = 0xFF;
	}
	else {
		lalp.bit.ALP = 0;
	}

	Im_DISP_Set_Alpha_Value(disp_sel, lalp);

	return ret;
}

/**
Set Enable/Disable each Display Layer.
@param[in] layer_id layer ID.<br>
			@ref E_OSDLayer_LCD_MAIN		: LCD Main Layer<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 Layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 Layer<br>
			@ref E_OSDLayer_LCD_OSD			: LCD OSD Layer (OSD-0 and OSD-1)<br>
			@ref E_OSDLayer_LCD_IMAGE		: LCD Image layer (except Color bar)<br>
			@ref E_OSDLayer_LCD_ALL			: LCD All Layer<br>
			@ref E_OSDLayer_HDMI_MAIN		: HDMI Main Layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 Layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 Layer<br>
			@ref E_OSDLayer_HDMI_OSD		: HDMI OSD Layer (OSD-0 and OSD-1)<br>
			@ref E_OSDLayer_HDMI_IMAGE		: HDMI Image layer (except Color bar)<br>
			@ref E_OSDLayer_HDMI_ALL		: HDMI All Layer<br>
@param[in] enable	TRUE mean enable to show and FALSE mean disable show.
@retval FJ_ERR_OK			Normal End.
@retval FJ_ERR_OSD_LAYER	layer_id error
@retval FJ_ERR_NG			Abnormal End.
*/
static FJ_ERR_CODE fj_show_enable(E_OSDLayer layer_id, BOOL enable)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	UINT32 layer = E_IM_DISP_SEL_LAYER_NONE;
	BYTE ifs;
	E_IM_DISP_SEL block;

	// Set Block Number
	if( (layer_id & E_OSDLayer_LCD) == E_OSDLayer_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		block = E_IM_DISP_HDMI;
	}
	
	// Get display interface selection
	if(Im_DISP_Get_Display_Interface(block, &ifs) == D_DDIM_OK){
		if( (ifs == D_IM_DISP_IFS_LCD) || (ifs == D_IM_DISP_IFS_MIPI) ){
			// Set Layer
			if((layer_id & E_OSDLayer_LCD_MAIN) == E_OSDLayer_LCD_MAIN) {
				layer = E_IM_DISP_SEL_LAYER_MAIN;
			}
			if((layer_id & E_OSDLayer_LCD_OSD0) == E_OSDLayer_LCD_OSD0) {
				layer |= E_IM_DISP_SEL_LAYER_OSD_0;
			}
			if((layer_id & E_OSDLayer_LCD_OSD1) == E_OSDLayer_LCD_OSD1) {
				layer |= E_IM_DISP_SEL_LAYER_OSD_1;
			}
			if((layer_id & E_OSDLayer_LCD_COLOR_BAR) == E_OSDLayer_LCD_COLOR_BAR) {
				layer = E_IM_DISP_SEL_LAYER_DCORE;
			}
		}
		else{
			if((layer_id & E_OSDLayer_HDMI_MAIN) == E_OSDLayer_HDMI_MAIN) {
				layer |= E_IM_DISP_SEL_LAYER_MAIN;
			}
			if((layer_id & E_OSDLayer_HDMI_OSD0) == E_OSDLayer_HDMI_OSD0) {
				layer |= E_IM_DISP_SEL_LAYER_OSD_0;
			}
			if((layer_id & E_OSDLayer_HDMI_OSD1) == E_OSDLayer_HDMI_OSD1)  {
				layer |= E_IM_DISP_SEL_LAYER_OSD_1;
			}
			if((layer_id & E_OSDLayer_HDMI_COLOR_BAR) == E_OSDLayer_HDMI_COLOR_BAR) {
				layer |= E_IM_DISP_SEL_LAYER_DCORE;
			}
		}

		if(layer == E_IM_DISP_SEL_LAYER_NONE){
			ret = FJ_ERR_OSD_LAYER;
		}
		else{
			// Enable/Disable
			if ( enable ){
				// Start Disp
				if( block == E_IM_DISP_LCD_MIPI ){
					if( Im_DISP_Start(E_IM_DISP_SEL_LAYER_NONE, layer, 0) != D_DDIM_OK){
						ret = FJ_ERR_NG;
					}
				}
				else{
					if( Im_DISP_Start(layer, E_IM_DISP_SEL_LAYER_NONE, 0) != D_DDIM_OK){
						ret = FJ_ERR_NG;
					}
				}
			}
			else{
				// Stop Disp
				if( block == E_IM_DISP_LCD_MIPI ){
					if(Im_DISP_Stop(E_IM_DISP_SEL_LAYER_NONE, layer, 0, E_IM_DISP_STOP_TYPE_WITH_WAIT) != D_DDIM_OK){
						ret = FJ_ERR_NG;
					}
				}
				else{
					if(Im_DISP_Stop(layer, E_IM_DISP_SEL_LAYER_NONE, 0, E_IM_DISP_STOP_TYPE_WITH_WAIT) != D_DDIM_OK){
						ret = FJ_ERR_NG;
					}
				}
			}
		}
	}
	else{
		ret = FJ_ERR_NG;
	}
	return ret;
}

/**
Initialize all OSD layer(OSD-0,OSD-1).
*/
static VOID fj_osd_initialize()
{
	// LCD
	FJ_Initialize_OSD_Layer(E_OSDLayer_LCD_OSD0);
	FJ_Initialize_OSD_Layer(E_OSDLayer_LCD_OSD1);

	// HDMI
	FJ_Initialize_OSD_Layer(E_OSDLayer_HDMI_OSD0);
	FJ_Initialize_OSD_Layer(E_OSDLayer_HDMI_OSD1);
}

/**
Initialize OSD layer.
@param[in] layer_id layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
*/
static VOID fj_initialize_osd_layer(E_OSDLayer layer_id)
{
	ULONG address = 0;
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	E_IM_DISP_SEL disp_sel;

	FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);

	if(ret == FJ_ERR_OK){
#ifdef CO_DEBUG_ON_PC
		ULONG area_size = BF_Osd_Attr_Get_Pc_Debug_Osd_Addr(&address);
		memset((BYTE*)address, 0, area_size);
#else // CO_DEBUG_ON_PC
		switch (layer_id){
			case E_OSDLayer_LCD_OSD0:
				memset((BYTE*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(SDRAM_ADR_LCD_OSD0_0), 0, SDRAM_BNK_LCD_OSD0 * SDRAM_SIZ_LCD_OSD0);
				break;

			case E_OSDLayer_LCD_OSD1:
				memset((BYTE*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(SDRAM_ADR_LCD_OSD1_0), 0, SDRAM_BNK_LCD_OSD1 * SDRAM_SIZ_LCD_OSD1);
				break;

			case E_OSDLayer_HDMI_OSD0:
				memset((BYTE*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(SDRAM_ADR_HDMI_OSD0_0), 0, SDRAM_BNK_HDMI_OSD0 * SDRAM_SIZ_HDMI_OSD0);
				break;

			case E_OSDLayer_HDMI_OSD1:
				memset((BYTE*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(SDRAM_ADR_HDMI_OSD1_0), 0, SDRAM_BNK_HDMI_OSD1 * SDRAM_SIZ_HDMI_OSD1);
				break;

			default:
				break;
		}
#endif // CO_DEBUG_ON_PC
		BF_L1l2cache_Clean_All();

		UINT32 vlaid_num = gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no][disp_no_data.gr_no].vaild_area_num;
		E_IM_DISP_OSD_SA_BANK_NO osd_bank_no;

		if( disp_no_data.disp_block_no == E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_HDMI ) {
			disp_sel = E_IM_DISP_HDMI;
		}
		else {
			disp_sel = E_IM_DISP_LCD_MIPI;
		}

		for( disp_no_data.draw_area = 0;  disp_no_data.draw_area < vlaid_num;  disp_no_data.draw_area++) {
			address = Sdram_Map_Osd_Get_OSD_Addr(&disp_no_data);
			// Set display address.
			if(disp_no_data.draw_area == 0) {
				for(osd_bank_no = E_IM_DISP_OSD_SA_BANK_NO_0_0; osd_bank_no <= E_IM_DISP_OSD_SA_BANK_NO_0_3; osd_bank_no++) {
					Im_DISP_Set_OSD_Area_Addr(disp_sel, disp_no_data.layer, osd_bank_no, address);
				}
			}
			else{
				osd_bank_no = (E_IM_DISP_OSD_SA_BANK_NO)(disp_no_data.draw_area + E_IM_DISP_OSD_SA_BANK_NO_0_3);
				Im_DISP_Set_OSD_Area_Addr(disp_sel, disp_no_data.layer, osd_bank_no, address);
			}
			//Set all draw area No.
			Sdram_Map_Osd_Change_OSD_Area_Index(disp_no_data.disp_block_no, disp_no_data.gr_no, disp_no_data.draw_area);
		}
	}
}

/**
Set matrix.
@param[in] disp_type	display type.<br>
@param[in] kind			Matrix kind. See @ref FJ_OSD_MATRIX_KIND<br>
@param[in] enable		RGB to RGB matrix calculate enable. TRUE=enable(RGB to RGB), FALSE=disable(RGB to YCbCr)<br>
@param[in] matrix		Matrix coefficient data. See @ref T_MATRIX_DATA<br>
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
static FJ_ERR_CODE fj_osd_set_matrix(FJ_DISP_TYPE disp_type, FJ_OSD_MATRIX_KIND kind, BYTE enable, T_MATRIX_DATA const* const matrix)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	E_IM_DISP_MATRIX_KIND matrix_kind = E_IM_DISP_MATRIX_KIND_R2Y;
	U_IM_DISP_YR_MATRIX_COEFFICIENT matrix_data[D_IM_DISP_MATRIX_SIZE];

	E_IM_DISP_SEL block = E_IM_DISP_HDMI;

	if( disp_type == FJ_DISP_TYPE_HDMI ){
		block = E_IM_DISP_HDMI;
	}
	else if( disp_type == FJ_DISP_TYPE_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		ret = FJ_ERR_NG;
	}

	if( ret == FJ_ERR_OK ){
		// check kind
		switch (kind){
			case FJ_OSD_MATRIX_KIND_YR:
			case FJ_OSD_MATRIX_KIND_CC:
				matrix_kind = (kind == FJ_OSD_MATRIX_KIND_YR) ? E_IM_DISP_MATRIX_KIND_Y2R : E_IM_DISP_MATRIX_KIND_CC;
				break;

			case FJ_OSD_MATRIX_KIND_RY:
				matrix_kind = E_IM_DISP_MATRIX_KIND_R2Y;
				break;

			default:
				ret = FJ_ERR_NG_INPUT_PARAM;
				break;
		}
	}

	if(ret == FJ_ERR_OK && matrix != NULL){
		// set Matrix data
		matrix_data[0].bit.COEFFICIENT0 = matrix->Coefficient00;
		matrix_data[0].bit.COEFFICIENT1 = matrix->Coefficient01;
		matrix_data[0].bit.COEFFICIENT2 = matrix->Coefficient02;
		matrix_data[1].bit.COEFFICIENT0 = matrix->Coefficient10;
		matrix_data[1].bit.COEFFICIENT1 = matrix->Coefficient11;
		matrix_data[1].bit.COEFFICIENT2 = matrix->Coefficient12;
		matrix_data[2].bit.COEFFICIENT0 = matrix->Coefficient20;
		matrix_data[2].bit.COEFFICIENT1 = matrix->Coefficient21;
		matrix_data[2].bit.COEFFICIENT2 = matrix->Coefficient22;

		if(matrix_kind == E_IM_DISP_MATRIX_KIND_R2Y){
			if(Im_DISP_Set_Output_Matrix(block, enable, matrix_data) != D_DDIM_OK){
				ret = FJ_ERR_NG_INPUT_PARAM;
			}
		}
		else{
			if(Im_DISP_Set_Matrix(block, matrix_data) != D_DDIM_OK){
				ret = FJ_ERR_NG_INPUT_PARAM;
			}
		}
	}

	return ret;
}

///////////////////////////////////////////////////
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_COMM_CODE"
/**
Set blink method of the OSD data.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in] blink_show_period	Blink show period of area 0~9. Value range:1~64.<br>
@param[in] blink_hide_period	Blink hide period of area 0~9. Value range:1~64.<br>
@param[in] area					Selection of blinking method.<br>
								bit d[0]:Display area 0 ~ d[9]:Display area 9<br>
								value 0:not blink, 1:blink<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		Abnormal End.
*/
static FJ_ERR_CODE fj_osd_set_blink_on_off(E_OSDLayer layer_id, BYTE blink_show_period[OSD_DRAW_AREA_NUM], BYTE blink_hide_period[OSD_DRAW_AREA_NUM], ULONG area)
{
	U_IM_DISP_GRBLINK grblink;
	INT8 i;
	E_IM_DISP_SEL disp_sel;

	for (i = 1; i < D_IM_DISP_GRBLINK_SIZE; i++){
		grblink.word[i] = 0;
	}

	// Set Layer
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);

	if(ret == FJ_ERR_OK ) {

		if( disp_no_data.disp_block_no == E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_HDMI ) {
			disp_sel = E_IM_DISP_HDMI;
		}
		else {
			disp_sel = E_IM_DISP_LCD_MIPI;
		}

		grblink.bit.BTIMH_0 = blink_show_period[0] - 1;
		grblink.bit.BTIMH_1 = blink_show_period[1] - 1;
		grblink.bit.BTIMH_2 = blink_show_period[2] - 1;
		grblink.bit.BTIMH_3 = blink_show_period[3] - 1;
		grblink.bit.BTIMH_4 = blink_show_period[4] - 1;
		grblink.bit.BTIMH_5 = blink_show_period[5] - 1;
		grblink.bit.BTIMH_6 = blink_show_period[6] - 1;
		grblink.bit.BTIMH_7 = blink_show_period[7] - 1;
		grblink.bit.BTIMH_8 = blink_show_period[8] - 1;
		grblink.bit.BTIMH_9 = blink_show_period[9] - 1;
		grblink.bit.BTIML_0 = blink_hide_period[0] - 1;
		grblink.bit.BTIML_1 = blink_hide_period[1] - 1;
		grblink.bit.BTIML_2 = blink_hide_period[2] - 1;
		grblink.bit.BTIML_3 = blink_hide_period[3] - 1;
		grblink.bit.BTIML_4 = blink_hide_period[4] - 1;
		grblink.bit.BTIML_5 = blink_hide_period[5] - 1;
		grblink.bit.BTIML_6 = blink_hide_period[6] - 1;
		grblink.bit.BTIML_7 = blink_hide_period[7] - 1;
		grblink.bit.BTIML_8 = blink_hide_period[8] - 1;
		grblink.bit.BTIML_9 = blink_hide_period[9] - 1;

		// Blink timer
		Im_DISP_Set_OSD_Blink_Timer(disp_sel, disp_no_data.layer, grblink);

		// Blink On/Off
		Im_DISP_Set_OSD_Blink(disp_sel, disp_no_data.layer, area);
	}

	return ret;
}

/**
Set histogram display area position.<br>
@param[in] layer_id layer ID.
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in] histo_x position X of histogram display area.
@param[in] histo_y position Y of histogram display area.
@param[in] histo_width width of histogram display area.
@param[in] histo_height height X of histogram display area.
@param[in] drawAreaNum number of the drawing area.<br>
@retval FJ_ERR_OK	Normal End.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
@retval FJ_ERR_NG	Abnormal End.
@remarks <ul>
		   <li>All parameters should be multiples of 4.</li>
		   <li>(histo_width - 2) must divisible 256.</li>
		 </ul>
*/
static FJ_ERR_CODE fj_setdisphistogramposition(E_OSDLayer layer_id, UINT16 histo_x, UINT16 histo_y, UINT16 histo_width, UINT16 histo_height, BYTE drawAreaNum)
{
	FJ_ERR_CODE ret = FJ_ERR_NG_INPUT_PARAM;
	T_BF_OSD_HISTOGRAM_COMM_HIST_ATTR* histogram_Size;
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;

	if(BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data) == FJ_ERR_OK) {
		T_Rect histogram_Rect = {
			.Postion.StartX = histo_x,
			.Postion.StartY = histo_y,
			.Width = histo_width,
			.Height = histo_height
		};
		
		draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no][disp_no_data.gr_no];
		if(((histo_width & 1) == 0) && ((histo_height & 1) == 0) && (drawAreaNum < draw_area_inf->vaild_area_num) &&
		   (BF_Osd_Draw_Comm_Is_Rect_Inside(draw_area_inf->area_size[drawAreaNum], histogram_Rect) == TRUE)) {
			switch(layer_id){
				case E_OSDLayer_HDMI_OSD0:
				case E_OSDLayer_HDMI_OSD1:
					// Display histogram to HDMI
					histogram_Size = &gBF_Osd_Histogram_Comm_HDMI_Hist_Attr;
					break;

				default:
					// Display histogram to LCD
					histogram_Size = &gBF_Osd_Histogram_Comm_LCD_Hist_Attr;
					break;
			}
			// Size should be multiple of 2.
			// [Width of graph area] = [Width of histogram] - [Width of frame]
			SHORT graph_area_width = (SHORT)histo_width - histogram_Size->box.thickness.StartX * 2;
			if((D_BF_OSD_HISTOGRAM_COMM_GRADATION_NUM % graph_area_width) == 0){
				// Width of graph area must be divisor of 256.
				histogram_Size->box.out_width = histo_width;
				histogram_Size->box.out_height = histo_height;
				histogram_Size->box.out_pos_x = histo_x;
				histogram_Size->box.out_pos_y = histo_y;
				histogram_Size->showing_area_No = drawAreaNum;
				ret = FJ_ERR_OK;
			}
		}
	}
	return ret;
}

/**
Get Histogram box ouside width.<br>
@param[in] layer_id layer ID.
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@return Histogram box width.
*/
static UINT16 fj_gethistogramwidth(E_OSDLayer layer_id)
{
	UINT16 ret = 0;
	switch(layer_id) {
		case E_OSDLayer_HDMI_OSD1:		// FALL THROUGH 
		case E_OSDLayer_HDMI_OSD0:		// FALL THROUGH
			// Display histogram to HOC
			ret = gBF_Osd_Histogram_Comm_HDMI_Hist_Attr.box.out_width;
			break;

		case E_OSDLayer_LCD_OSD1:		// FALL THROUGH 
		case E_OSDLayer_LCD_OSD0:		// FALL THROUGH
			// Display histogram
			ret = gBF_Osd_Histogram_Comm_LCD_Hist_Attr.box.out_width;
			break;

		default:
			break;
	}
	return ret;
}

/**
Get Histogram box outside height.<br>
@param[in] layer_id layer ID.
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@return Histogram box height.
*/
static UINT16 fj_gethistogramheight(E_OSDLayer layer_id)
{
	UINT16 ret = 0;
	switch(layer_id) {
		case E_OSDLayer_HDMI_OSD1:		// FALL THROUGH 
		case E_OSDLayer_HDMI_OSD0:		// FALL THROUGH
			// Display histogram to HOC/HGR
			ret = gBF_Osd_Histogram_Comm_HDMI_Hist_Attr.box.out_height;
			break;

		case E_OSDLayer_LCD_OSD1:		// FALL THROUGH 
		case E_OSDLayer_LCD_OSD0:		// FALL THROUGH
			// Display histogram
			ret = gBF_Osd_Histogram_Comm_LCD_Hist_Attr.box.out_height;
			break;

		default:
			break;
	}
	return ret;
}


/**
Set threshold and draw frame of histogram.<br>
@param[in] layer_id layer ID.
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in] threshold threshold value.
@remarks The histogram's threshold is mean as following:<br>
		 If value is bigger than this one, it will set to this value.
*/
static FJ_ERR_CODE fj_prehistogram(E_OSDLayer layer_id, UINT32 threshold)
{
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);
	if( ret == FJ_ERR_OK) {
		switch(layer_id) {
			case E_OSDLayer_HDMI_OSD0:		// FALL THROUGH
			case E_OSDLayer_HDMI_OSD1:		// FALL THROUGH
				// Processing to HDMI_OSD. Draw frame of the histogram to HDMI_OSD.
				gBF_Osd_Histogram_Comm_HDMI_Hist_Attr.threshold = threshold;
				disp_no_data.draw_area = gBF_Osd_Histogram_Comm_HDMI_Hist_Attr.showing_area_No;
				BF_Osd_Histogram_Comm_Draw_Histogram_Box(&disp_no_data, &gBF_Osd_Histogram_Comm_HDMI_Hist_Attr);	
				break;

			case E_OSDLayer_LCD_OSD1:		// FALL THROUGH 
			case E_OSDLayer_LCD_OSD0:		// FALL THROUGH
				// Processing to VGRch. Draw frame of the histogram to VGRch.
				gBF_Osd_Histogram_Comm_LCD_Hist_Attr.threshold = threshold;
				disp_no_data.draw_area = gBF_Osd_Histogram_Comm_LCD_Hist_Attr.showing_area_No;
				BF_Osd_Histogram_Comm_Draw_Histogram_Box(&disp_no_data, &gBF_Osd_Histogram_Comm_LCD_Hist_Attr);
				break;

			default:
				break;
		}
	}
	return ret;
}

/**
Draw histogram.<br>
@param[in] layer_id layer ID.
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
*/
static VOID fj_histogramenable(E_OSDLayer layer_id)
{
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);
	if( ret == FJ_ERR_OK) {
		switch(layer_id) {
			case E_OSDLayer_HDMI_OSD1:
			case E_OSDLayer_HDMI_OSD0:
				// Processing to HDMI_OSD. Draw histogram to HDMI_OSD.
				disp_no_data.draw_area = gBF_Osd_Histogram_Comm_HDMI_Hist_Attr.showing_area_No;
				BF_Osd_Histogram_Comm_Draw_Histogram_Graph(&disp_no_data, &gBF_Osd_Histogram_Comm_HDMI_Hist_Attr);
				break;

			case E_OSDLayer_LCD_OSD1:		// FALL THROUGH 
			case E_OSDLayer_LCD_OSD0:		// FALL THROUGH
				// Processing to VGRch. Draw histogram to VGRch.
				disp_no_data.draw_area = gBF_Osd_Histogram_Comm_LCD_Hist_Attr.showing_area_No;
				BF_Osd_Histogram_Comm_Draw_Histogram_Graph(&disp_no_data, &gBF_Osd_Histogram_Comm_LCD_Hist_Attr);
				break;

			default:
				break;
		}
	}
}

/**
Draw a shape on osd by display id and type.
@param[in] layer_id layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_LCD_GRID		: LCD Grid line layer<br>
			@ref E_OSDLayer_LCD_FACE		: LCD Face frame layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_GRID		: HDMI Grid line layer<br>
			@ref E_OSDLayer_HDMI_FACE		: HDMI Face frame layer<br>
@param[in] wType type of drawing shape.<br>
			@ref E_FJ_OSD_SHAPE_TYPE_RECT : rectangle no frame<br>
			@ref E_FJ_OSD_SHAPE_TYPE_RECT_FRAME : rectangle  frame only<br>
			@ref E_FJ_OSD_SHAPE_TYPE_FILLED_RECT_FRAME : rectangle with frame and body<br>
			@ref E_FJ_OSD_SHAPE_TYPE_CIRCLE : circle frame only<br>
			@ref E_FJ_OSD_SHAPE_TYPE_FILLED_CIRCLE : circel with frame and body<br>
			@ref E_FJ_OSD_SHAPE_TYPE_ROUND_FRAME : round rectangle frame only<br>
			@ref E_FJ_OSD_SHAPE_TYPE_FILLED_ROUND_FRAME : round rectanele with frame and body<br>
			@ref E_FJ_OSD_SHAPE_TYPE_DONUT_RECT_FRAME : frame thickness is 1 rectangle frame only<br>
			@ref E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_LT : Triangle(Position of right angle is "Left-Top")<br>
			@ref E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_RT : Triangle(Position of right angle is "Right-Top")<br>
			@ref E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_LB : Triangle(Position of right angle is "Left-Bottom")<br>
			@ref E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_RB : Triangle(Position of right angle is "Right-Bottom")<br>
@param[in] wStartX X of starting position to draw.<br>
@param[in] wStartY Y of starting position to draw.<br>
@param[in] uwWidth width of the shape.<br>
@param[in] uwHeight height of the shape.<br>
@param[in] drawAreaNum number of the drawing area.<br>
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_OSD_LAYER		Abnormal End. layer_id is illegal.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Parameter error.
@retval FJ_ERR_POSITION			Abnormal End. Size or position were not even number.
@retval FJ_ERR_OSD_SEQUENCE		Abnormal End. CLUT has not been load.
@retval FJ_ERR_NG				Abnormal End.
*/
static FJ_ERR_CODE fj_osddrawshape(E_OSDLayer layer_id, E_FJ_OSD_SHAPE_TYPE wType,
								   INT32 wStartX, INT32 wStartY, INT32 uwWidth, INT32 uwHeight, BYTE drawAreaNum)
{
	T_IconColor	Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_END];
	T_Rect		dispWnd, obj_wnd;
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;

	FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);
	if(ret == FJ_ERR_OK ) {
		T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;
		draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no][disp_no_data.gr_no];
		ret = (drawAreaNum < draw_area_inf->vaild_area_num) ? FJ_ERR_OK : FJ_ERR_NG_INPUT_PARAM;
	}
	
	if(ret == FJ_ERR_OK ) {
		disp_no_data.draw_area = drawAreaNum;
		// Set color of shape.
	#if FJ_OSD_COMM_RGBAXXXX == 4
		Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word = gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data.disp_block_no].shape.outline_color.word;
		Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY].word    = gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data.disp_block_no].shape.body_color.word;
	#else
		Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data.disp_block_no].shape.outline_color.word);
		Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY].word    = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data.disp_block_no].shape.body_color.word);
	#endif

		// Set object window.
		obj_wnd.Postion.StartX = wStartX;
		obj_wnd.Postion.StartY = wStartY;
		obj_wnd.Width          = uwWidth;
		obj_wnd.Height          = uwHeight;

		// Set display window.
		dispWnd.Postion.StartX = 0;
		dispWnd.Postion.StartY = 0;
		dispWnd.Width          = obj_wnd.Width;
		dispWnd.Height          = obj_wnd.Height;

		if ((wType == E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_LT) ||
			(wType == E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_RT) ||
			(wType == E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_LB) ||
			(wType == E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_RB)) {
			// Draw triangle
			ret = BF_Osd_Shape_Comm_Draw_Triangle(&disp_no_data, wStartX, wStartY, uwWidth, uwHeight, wType);
		}
		else {
			switch(layer_id) {
				case E_OSDLayer_LCD_OSD1:
				case E_OSDLayer_LCD_OSD0:
				case E_OSDLayer_HDMI_OSD1:
				case E_OSDLayer_HDMI_OSD0:
					// Draw shape to VGRch/HGR.
					ret = BF_Osd_Shape_Comm_Draw_Shape_GR(&disp_no_data, wType, obj_wnd, dispWnd, Color_Blending,
									&gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data.disp_block_no].shape.outline_size, E_DrawMode_Speed_Optimized, 255);
					break;

				default:
					// Parameter error.
					ret = FJ_ERR_OSD_LAYER;
					break;
			}
		}
	}
	return ret;
}

/**
Draw specified image to draw buffer of V or HD layer.<br>
The image format can use only YCC420.<br>
@param[in] layer_id layer ID.
			@ref E_OSDLayer_LCD_MAIN		: LCD Main layer<br>
			@ref E_OSDLayer_HDMI_MAIN		: HDMI Main layer<br>
@param[in] y_addr	Top address of Y data.
@param[in] c_addr	Top adddress of C data.
@param[in] start_x	X coordinates at drawing beginning position.
@param[in] start_y	Y coordinates at drawing beginning position.
@param[in] width	Width of input image.
@param[in] height	Height of input image.
@retval FJ_ERR_OK	Normal End.
@retval FJ_ERR_POSITION Size or position is illegal.
@retval FJ_ERR_OSD_LAYER	Layer error.
@retval FJ_ERR_NG	Abnormal End.
@remarks "start_x", "start_y", "width", "height" parameters must be even number.<br>
*/
static FJ_ERR_CODE fj_osddrawimage(E_OSDLayer layer_id, UCHAR* y_addr, UCHAR* c_addr,
							USHORT start_x, USHORT start_y, USHORT width, USHORT height)
{
	FJ_ERR_CODE	ercd = FJ_ERR_OK;
	ULONG		main_y_addr   = 0;
	ULONG		main_c_addr  = 0;
	ULONG		c_sta_offset = 0;
	ULONG		y_sta_offset = 0;

	ercd = FJ_ERR_OK;

	// Check coordinates
	if(!(start_x & 1) && !(start_y & 1) && !(width & 1) && !(height & 1)) {
		//Position OK
		ULONG glabal_width = 0;
		ULONG glabal_lines = 0;

		if(layer_id == E_OSDLayer_LCD_MAIN){
			//Draw image to V
			glabal_width = WIDTH_VGA;
			glabal_lines = LINES_VGA;
			// Get MAIN edit buffer address
			Sdram_Map_Osd_Get_VDE_Chg_Addr(FJ_DISP_TYPE_LCD, &main_y_addr, &main_c_addr);
			main_y_addr = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(main_y_addr);
			main_c_addr = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(main_c_addr);
		}
		else if(layer_id == E_OSDLayer_HDMI_MAIN){
			//Draw image to HD
			UINT32 display_size ;
			if(gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.main.pGet_SDRAM_HDch != NULL) {
				//Can get HD address
				glabal_width = gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.main.global_width;
				glabal_lines = gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.main.lines;
				main_y_addr = gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.main.pGet_SDRAM_HDch();
				display_size =  glabal_width * glabal_lines; 
				main_c_addr = main_y_addr + display_size;
			}
			else {
				ercd = FJ_ERR_OSD_LAYER;
			}
		}
		else {
			ercd = FJ_ERR_OSD_LAYER;
		}
		if(ercd == FJ_ERR_OK){
			// Check Vch buffer range
			if(((start_x + width) <= glabal_width) && ((start_y + height) <= glabal_lines)) {
				// Get draw start address
				y_sta_offset = (glabal_width * start_y) + start_x;
				c_sta_offset = (glabal_width  * (start_y >> 1)) + (start_x );
				main_y_addr   = main_y_addr + y_sta_offset;
				main_c_addr  = main_c_addr + c_sta_offset;
				// Copy image to Vch edit buffer
				BF_Osd_General_Copy_Box_To_Box((ULONG)y_addr, width, glabal_width, main_y_addr, width, height);
				BF_Osd_General_Copy_Box_To_Box((ULONG)c_addr, (width ), (glabal_width ), main_c_addr, width , (height >> 1));
			}
			else {
				/* Parameter error.
				   Drawing image must be install on the drawing buffer of Vch. */
				ercd = FJ_ERR_POSITION;
			}
		}
	}
	else {
		/* Parameter error.
		   Coordinates must be multiple of 2. */
		ercd = FJ_ERR_POSITION;
	}
	return ercd;
}

/**
Draw specified image to draw buffer of OSD0 or OSD1 or HDMI OSD0 or HDMI OSD1 layer.<br>
The image format can use only RGBA8888 or RGBA4444.<br>
@param[in] layer_id layer ID.
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in] addr_src		Top address of source image data.<br>
@param[in] draw_info	Coordinates at drawing destination buffer and size of drawn image.<br>
@param[in] drawAreaNum number of the drawing area.<br>
@param[in] global_width	Global width of drawn image.<br>
@retval FJ_ERR_OK	Normal End.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
@retval FJ_ERR_NG	Abnormal End.
*/
static FJ_ERR_CODE fj_osddrawbufferosd(E_OSDLayer layer_id, BYTE* addr_src, T_Rect const* const draw_info, BYTE drawAreaNum, USHORT global_width)
{
	FJ_ERR_CODE	ret ;
	T_Rect	draw_img_info;
	ULONG	edit_buf_addr;
	USHORT screen_size[2];//[0]: width [1]:height

	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);
	if(ret == FJ_ERR_OK ) {
		disp_no_data.draw_area = drawAreaNum;
		memcpy(&draw_img_info, draw_info, sizeof(T_Rect));

		// Check VGR buffer range
		ret = BF_Osd_Draw_Comm_Is_Rect_Draw_Area_Inside(&disp_no_data, draw_img_info.Postion, draw_img_info.Width, draw_img_info.Height);
		if(ret == FJ_ERR_OK ) {
			BF_Osd_Attr_Get_Draw_Area_Size( &disp_no_data, screen_size);
			// Draw image to edit buffer of VGRch.
			//Convert to darw area's relational coordinate
			BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(&disp_no_data,&draw_img_info.Postion);
			
			// Get VGR write information.
			BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(&draw_img_info.Postion, &edit_buf_addr, &disp_no_data);
			// Draw image to edit buffer.
			BF_Osd_General_Copy_Box_To_Box((ULONG)addr_src,
										(global_width * FJ_OSD_COMM_RGBAXXXX),
										(screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] * FJ_OSD_COMM_RGBAXXXX),
										edit_buf_addr,
										(draw_img_info.Width * FJ_OSD_COMM_RGBAXXXX),
										(draw_img_info.Height));
		}
		else {
			// Drawing position is illegal.
			BF_Debug_Print_Error(("fj_osddrawbuffervgr(): Drawing position is illegal.\n"));
			ret = FJ_ERR_POSITION;
		}
	}
	return ret;
}
#ifdef CO_LCD_ENABLE
/**
Set control data for connecting to LCD.<br>
@return See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_connect_lcd(VOID)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;

	switch(BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_LCD)){
		case E_FJ_DISP_IFS_NONE:
			ret = BF_Osd_Config_Comm_Change_To_LCD();
			if(ret == FJ_ERR_OK) {
				// Start color bar layer
				Im_DISP_Start(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE, 0);
			}
			break;

		case E_FJ_DISP_IFS_LCD:
		case E_FJ_DISP_IFS_MIPI:
			// Already connected.
			Im_DISP_Start(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE, 0);
			break;

		default:
			ret = FJ_ERR_NG;
			break;
	}
	return ret;
}

/**
Stop output for LCD.<br>
*/
static VOID fj_osd_disconnect_lcd(VOID)
{
	if( (BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_LCD) == E_FJ_DISP_IFS_LCD) ||
		(BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_LCD) == E_FJ_DISP_IFS_MIPI) ){
		BF_Osd_Config_Comm_Change_To_OFF(FJ_DISP_TYPE_LCD);
	}
}
#endif // CO_LCD_ENABLE

/**
Set control data and hunt memory for HDMI showing.<br>
@param[in] HDMI_Setting_Data	HDMI data from EDID. See @ref T_HDMI_SET
@return See @ref FJ_ERR_CODE.
@remarks HDMI and LCD can be set to both display at same time or only one.<br>
	All channels at HMDI side would be stopped after call this API.<br>
*/
static FJ_ERR_CODE fj_osd_connect_hdmi(const T_HDMI_SET* hdmi_setting_data)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;

	switch(BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_HDMI)){
		case E_FJ_DISP_IFS_NONE:
			ret = BF_Osd_Config_Comm_Change_To_HDMI(hdmi_setting_data);
			if(ret == FJ_ERR_OK) {
				// Start color bar layer
				Im_DISP_Start(E_IM_DISP_SEL_LAYER_DCORE, E_IM_DISP_SEL_LAYER_NONE, 0);
			}
			break;

		case E_FJ_DISP_IFS_HDMI:
			// Already connected.
			Im_DISP_Start(E_IM_DISP_SEL_LAYER_DCORE, E_IM_DISP_SEL_LAYER_NONE, 0);
			break;

		default:
			ret = FJ_ERR_NG;
			break;
	}
	return ret;
}

/**
Stop output for HDMI and free the resource.<br>
*/
static VOID fj_osd_disconnect_hdmi(VOID)
{
	if(BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_HDMI) == E_FJ_DISP_IFS_HDMI){
		BF_Osd_Config_Comm_Change_To_OFF(FJ_DISP_TYPE_HDMI);

		gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.main.pGet_SDRAM_HDch = gBF_Osd_Draw_Comm_Osd_Attr[E_FJ_DISP_COMM_BLOCK_0].hdmi.main.pChange_SDRAM_HDch = NULL;
	}
}

/**
Clear a part of display buffer.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in]	clear_rect	Clear rectangle.<br>
@param[in]	drawAreaNum	number of Area[0-9].<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osdclearpart(E_OSDLayer layer_id, T_Rect clear_rect, BYTE drawAreaNum)
{
	ULONG clear_address;
	USHORT screen_size[2];//[0]: width [1]:height
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;

	FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);
	if(ret == FJ_ERR_OK ) {
		draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no][disp_no_data.gr_no];
		ret = (drawAreaNum < draw_area_inf->vaild_area_num) ? FJ_ERR_OK : FJ_ERR_NG_INPUT_PARAM;
	}

	if(ret == FJ_ERR_OK) {
		disp_no_data.draw_area = drawAreaNum;
		ret = BF_Osd_Draw_Comm_Is_Rect_Draw_Area_Inside(&disp_no_data, clear_rect.Postion, clear_rect.Width, clear_rect.Height);

		if(ret == FJ_ERR_OK ) {
			//Get campus size
			BF_Osd_Attr_Get_Draw_Area_Size(&disp_no_data, screen_size);
	
			//Convert to darw area's relational coordinate
			BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(&disp_no_data,&clear_rect.Postion);
	
			//Cut outside of draw area
			if(BF_Osd_Draw_Comm_Is_Rect_At_LCD_Inside(screen_size,&clear_rect)){
				BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(&clear_rect.Postion, &clear_address, &disp_no_data);
				BF_Osd_General_Fill_Box_For_GR(0, screen_size[D_BF_OSD_ATTR_INDEX_WIDTH],
							(OSD_USHORT_INT*)clear_address, clear_rect.Width, clear_rect.Height);
			}
		}
	}
	return ret;
}

///////////////////////////////////////////////////
//  Following are BASEFW_VIEW_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_VIEW_CODE"
#if 0	// MILB Not Use
/**
Configures the image display attributes.
@param[in] bAttr	Attribute type.<br>
       @ref FJ_E_OSD_GDA_ATTR_CENTER : Set image display left-top's coordinate.<br>
              &nbsp;&nbsp;&nbsp; wParam1 - x coordinate<br>
              &nbsp;&nbsp;&nbsp; wParam2 - y coordinate<br>
       @ref FJ_E_OSD_GDA_ATTR_BRUSH_COLOR : Set RGBA color of shape body.<br>
              &nbsp;&nbsp;&nbsp; wParam1 - Set ARGB color of shape body. (ex:0xFF008080)<br>
       @ref FJ_E_OSD_GDA_ATTR_PEN_COLOR : Set RGBA color of frame.<br>
              &nbsp;&nbsp;&nbsp; wParam1 - Set ARGB color of frame. (ex:0xFF008080)<br>
@param[in] wParam1	Parmater 1.<br>
@param[in] wParam2	Parmater 2.<br>
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
*/
static FJ_ERR_CODE fj_setgda(FJ_E_OSD_GDA_ATTR_TYPE bAttr, INT32 wParam1, INT32 wParam2)
{
	FJ_ERR_CODE result = FJ_ERR_OK;

	switch(bAttr) {
		case FJ_E_OSD_GDA_ATTR_BRUSH_COLOR:
			// Set color of body in shape.
			gBF_Osd_Draw_Comm_GDA_Attr.shape.body_color.word = wParam1;
			break;

		case FJ_E_OSD_GDA_ATTR_PEN_COLOR:
			// Set color of frame in shape.
			gBF_Osd_Draw_Comm_GDA_Attr.shape.outline_color.word = wParam1;
			break;

		case FJ_E_OSD_GDA_ATTR_CENTER:
			// Set coordinates in Vch buffer making it to starting point of drawing.
			if(((wParam1 & 0x01) == 0) && ((wParam2 & 0x01) == 0)) {
				gBF_Osd_Draw_Comm_GDA_Attr.common.left_top_pos.StartX = wParam1;
				gBF_Osd_Draw_Comm_GDA_Attr.common.left_top_pos.StartY = wParam2;
			}
			else {
				// Error: The odd number coordinates cannot be specified.
				result = FJ_ERR_NG_INPUT_PARAM;
			}
			break;

		default:
			// Error: bAttr is illegal.
			result = FJ_ERR_NG_INPUT_PARAM;
			break;
	}
	return result;
}
#endif	// MILB Not Use

#if 0	// MILB Not Use
/**
Draw a shape on image by shape type.
@param[in] bShapeType type of drawing shape.<br>
			@ref FJ_E_OSD_SHAPE_TYPE_RECT_FRAME_2_COLOR : Double frame line of two colors.<br>
@param[in] wStartX		X of starting position to draw.<br>
@param[in] wStartY		Y of starting position to draw.<br>
@param[in] uwSizeX		Width of the shape.<br>
@param[in] uwSizeY		Height of the shape.<br>
@param[in] uwGlobalWidth Width of the image buffer.<br>
@param[in] image_addr	Address of image buffer to draw.<br>
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@retval FJ_ERR_NG				Abnormal End.
@remarks wStartX, wStartY, uwSizeX, uwSizeY and thickness of frame must be even number.<br>
		 Please change the color by the @ref FJ_SetGDA function.<br>
		 - @ref FJ_E_OSD_GDA_ATTR_PEN_COLOR   : Y element of outside frame. (example: 0x00FF0000)<br>
		                                       CbCr element becomes it as well as setting of FJ_E_OSD_GDA_ATTR_BRUSH_COLOR.<br>
		 - @ref FJ_E_OSD_GDA_ATTR_BRUSH_COLOR : YCbCr element of inside frame. (example: 0x00FF8080)<br>
*/
static FJ_ERR_CODE fj_drawshape(FJ_E_OSD_SHAPE_TYPE bShapeType, SHORT wStartX, SHORT wStartY,
								USHORT uwSizeX, USHORT uwSizeY, USHORT uwGlobalWidth, FJ_T_OSD_ADDR_YCC const* const image_addr)
{
	FJ_ERR_CODE result = FJ_ERR_OK;
	ULONG addr[3];

	switch(bShapeType) {
		case FJ_E_OSD_SHAPE_TYPE_RECT_FRAME_2_COLOR:
			/* Draw rectangle frame composed of two colors.
			   The thickness fixes two pixels. */
			addr[0] = image_addr->y;
			addr[1] = image_addr->cb;

			// Round coordinates and size to even number.
			wStartX = ((wStartX + gBF_Osd_Draw_Comm_GDA_Attr.common.left_top_pos.StartX) & 0xFFFC);
			wStartY = ((wStartY + gBF_Osd_Draw_Comm_GDA_Attr.common.left_top_pos.StartY) & 0xFFFC);
			uwSizeX = (uwSizeX & 0xFFFC);
			uwSizeY = (uwSizeY & 0xFFFC);

			// Draw 2 clolor rectangle frame.
			BF_Osd_Shape_Comm_Draw_2color_Frame(addr, wStartX, wStartY, uwSizeX, uwSizeY, uwGlobalWidth,
												gBF_Osd_Draw_Comm_GDA_Attr.shape.outline_color.AYCC.Y,
												gBF_Osd_Draw_Comm_GDA_Attr.shape.body_color.AYCC.Y,
												gBF_Osd_Draw_Comm_GDA_Attr.shape.body_color.AYCC.Cb,
												gBF_Osd_Draw_Comm_GDA_Attr.shape.body_color.AYCC.Cr);
			break;

		default:
			// Error: bShapeType is illegal.
			result = FJ_ERR_NG_INPUT_PARAM;
			break;
	}
	return result;
}
#endif	// MILB Not Use

#if 0	// MILB Not Use
/**
Draw a line to specified layer.
@param[in] layer_id layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in] line coordinate of line <br>
@param[in] Thickness the line's with
@param[in] TerminatorType the line's terminator type. <br>
		  @ref FJ_D_OSD_COMM_LINE_TERMINATOR_TYPE_CIRCLE<br>
		  @ref FJ_D_OSD_COMM_LINE_TERMINATOR_TYPE_RECT<br>
		  @ref FJ_D_OSD_COMM_LINE_TERMINATOR_TYPE_CIRCLE_RECT<br>
		  @ref FJ_D_OSD_COMM_LINE_TERMINATOR_TYPE_RECT_CIRCLE<br>
		  Following show parameter specified method:<br>
		@image html FJ_OSDDrawLine_Paramer1.gif
	  	@image html FJ_OSDDrawLine_Paramer4.gif
		@image html FJ_OSDDrawLine_Paramer2.gif
		@image html FJ_OSDDrawLine_Paramer3.gif
@param[in] drawAreaNum number of the drawing area.<br>

@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_OSD_LAYER		Abnormal End. layer_id is illegal.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Parameter error.
@retval FJ_ERR_POSITION			Abnormal End. Size or position were not even number.
@retval FJ_ERR_OSD_SEQUENCE		Abnormal End. CLUT has not been load.
@retval FJ_ERR_NG				Abnormal End.
*/
static FJ_ERR_CODE fj_osddrawline(E_OSDLayer layer_id, 
						   FJ_LINE_COORDINATES line, USHORT Thickness, BYTE TerminatorType, BYTE drawAreaNum)
{
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;

	FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);
	if(ret == FJ_ERR_OK){
		T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;
		draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no][disp_no_data.gr_no];
		ret = (drawAreaNum < draw_area_inf->vaild_area_num) ? FJ_ERR_OK : FJ_ERR_NG_INPUT_PARAM;
	}

	if(ret == FJ_ERR_OK ) {
		disp_no_data.draw_area = drawAreaNum;
		if(Thickness >0 ) {
			 T_Rect obj_wnd;

			 obj_wnd.Postion.StartX = line.start_pos.StartX - Thickness/2;
			 obj_wnd.Postion.StartY = line.start_pos.StartY - Thickness/2;
			 obj_wnd.Height = obj_wnd.Width = Thickness;
			switch(TerminatorType){
				case FJ_D_OSD_COMM_LINE_TERMINATOR_TYPE_CIRCLE:
					//Both side is circle
					ret = BF_Osd_Shape_Comm_Draw_Circle(&disp_no_data, obj_wnd);
					obj_wnd.Postion.StartX = line.end_pos.StartX - Thickness/2;
					obj_wnd.Postion.StartY = line.end_pos.StartY - Thickness/2;
					ret = BF_Osd_Shape_Comm_Draw_Circle(&disp_no_data, obj_wnd);
					break;

				case FJ_D_OSD_COMM_LINE_TERMINATOR_TYPE_CIRCLE_RECT:
					//Start side is circle
					ret = BF_Osd_Shape_Comm_Draw_Circle(&disp_no_data, obj_wnd);
					break;

				case FJ_D_OSD_COMM_LINE_TERMINATOR_TYPE_RECT_CIRCLE:
					//End side is circle
					obj_wnd.Postion.StartX = line.end_pos.StartX - Thickness/2;
					obj_wnd.Postion.StartY = line.end_pos.StartY - Thickness/2;
					ret = BF_Osd_Shape_Comm_Draw_Circle(&disp_no_data, obj_wnd);
					break;

				default:
					//both side is rectangle
					break;
			}
			BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(&disp_no_data, &line.start_pos);
			BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(&disp_no_data, &line.end_pos);
			if(line.start_pos.StartX == line.end_pos.StartX || line.start_pos.StartY == line.end_pos.StartY){
				ret = BF_Osd_Shape_Comm_drawstraight(&disp_no_data, &line, Thickness);
			}
			else {
				ret = BF_Osd_Shape_Comm_drawslant(&disp_no_data, &line, Thickness);
			}
		}
		else{
			ret = FJ_ERR_NG_INPUT_PARAM;
		}
	}
	return ret;
}
#endif	// MILB Not Use
#if 0	// MILB Not Use
/**
The text for debugging is displayed on OSD.<br>
Because the font data for debugging is used, it is not necessary to load the resource file.<br>
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in] color	color[0] is specification of the background color.<br>
					color[1] is specification of the font color.<br>
@param[in] postion	Coordinates at drawing position. <br>
@param[in] string	String where it wants to draw.<br>
					This function corresponds to all ASCII codes.<br>
@param[in]	drawAreaNum	number of Area[0-9].<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		Abnormal End.
*/
static FJ_ERR_CODE fj_osdprint(E_OSDLayer layer_id, FJ_OSD_PRINT_COLOR color[FJ_OSD_PRINT_COLOR_INDEX_END], T_POS postion, CHAR* string, BYTE drawAreaNum)
{
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;

	FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);
	if(ret == FJ_ERR_OK) {
		USHORT screen_size[2];
		disp_no_data.draw_area = drawAreaNum;
		BF_Osd_Attr_Get_Draw_Area_Size(&disp_no_data, screen_size);
		ULONG  out_text_addr = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(Sdram_Map_Osd_Get_OSD_Addr(&disp_no_data));

		E_BF_OSD_DRAW_COMM_PRINT_COLOR gr_color[FJ_OSD_PRINT_COLOR_INDEX_END];
		//Convert to draw area's position
		BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(&disp_no_data, &postion);

		if(postion.StartX < 0 || postion.StartY < 0) {
			ret = FJ_ERR_NG_INPUT_PARAM;
		}
		else if(postion.StartX > screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] ||
				postion.StartY > screen_size[D_BF_OSD_ATTR_INDEX_LINES]){
			ret = FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			out_text_addr += (postion.StartX + postion.StartY * screen_size[D_BF_OSD_ATTR_INDEX_WIDTH]) * FJ_OSD_COMM_RGBAXXXX;
			gr_color[FJ_OSD_PRINT_COLOR_INDEX_BACK] = BF_Osd_Draw_Comm_Get_GR_Print_Color(color[FJ_OSD_PRINT_COLOR_INDEX_BACK]);
			gr_color[FJ_OSD_PRINT_COLOR_INDEX_TEXT] = BF_Osd_Draw_Comm_Get_GR_Print_Color(color[FJ_OSD_PRINT_COLOR_INDEX_TEXT]);
	
			BF_Osd_Draw_Comm_Print_String_GR(out_text_addr, gr_color, string, 
						 screen_size[D_BF_OSD_ATTR_INDEX_WIDTH], screen_size[D_BF_OSD_ATTR_INDEX_LINES], postion);
		}
	}
	return ret;
}
#endif	// MILB Not Use

/**
Set the area to draw.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in]	drawArea	the array of draw Area position.<br>
@param[in]	drawAreaNum	number of Area. Value range : 1~10<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
@remarks This function disable all the draw Area before setting new draw Area. <br>
*/
static FJ_ERR_CODE fj_osd_draw_osd_area(E_OSDLayer layer_id, const T_Rect drawArea[OSD_DRAW_AREA_NUM], BYTE drawAreaNum)
{
	FJ_ERR_CODE ret = (drawAreaNum <= OSD_DRAW_AREA_NUM) ? FJ_ERR_OK : FJ_ERR_NG_INPUT_PARAM;

	if(ret == FJ_ERR_OK){
		ret = (drawAreaNum > 0) ? FJ_ERR_OK : FJ_ERR_NG_INPUT_PARAM;
	}
	
	if(ret == FJ_ERR_OK){
		T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
		U_IM_DISP_DSTA position;
		U_IM_DISP_SIZE input_size[OSD_DRAW_AREA_NUM];
		U_IM_DISP_SIZE territory_size;
		INT8 i;

		territory_size.word = 0;
		ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);
		if(ret == FJ_ERR_OK){
			ret = osd_comm_draw_area_overlap_check(drawArea, drawAreaNum);
		}

		if(ret == FJ_ERR_OK){
			if(Im_DISP_Get_OSD_Territory_Size(disp_no_data.disp_block_no, disp_no_data.layer, &territory_size) != D_DDIM_OK){
				ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
			}
		}

		if(ret == FJ_ERR_OK){
			ret = osd_comm_draw_area_size_check(territory_size, &drawArea[0]);
			if(ret == FJ_ERR_OK){
				for(i = 1; i < drawAreaNum; i++){
					ret = osd_comm_draw_area_size_check(territory_size, &drawArea[i]);
					if(ret != FJ_ERR_OK){
						break;
					}

					ret = osd_comm_draw_area_pos_check(&drawArea[i-1], &drawArea[i]);
					if(ret != FJ_ERR_OK){
						break;
					}
				}
			}
		}

		if(ret == FJ_ERR_OK){
			ret = fj_osd_set_osd_area_enable(layer_id, 0);
		}

		if(ret == FJ_ERR_OK){
			position.word = 0;
			for(i = 0; i < drawAreaNum; i++){
				input_size[i].size.width = drawArea[i].Width;
				input_size[i].size.lines = drawArea[i].Height;
				position.bit.DSH = drawArea[i].Postion.StartX;
				position.bit.DSV = drawArea[i].Postion.StartY;
				if(Im_DISP_Set_OSD_Display_Position(disp_no_data.disp_block_no, disp_no_data.layer, i, position) != D_DDIM_OK){
					ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
					break;
				}
				if(Im_DISP_Set_OSD_Lfd(disp_no_data.disp_block_no, disp_no_data.layer, i, input_size[i].size.width*sizeof(OSD_USHORT_INT)) != D_DDIM_OK){
					ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
					break;
				}
			}
		}

		if(ret == FJ_ERR_OK){
			if( Im_DISP_Set_OSD_Area_Size(disp_no_data.disp_block_no, disp_no_data.layer, input_size,drawAreaNum) != D_DDIM_OK){
				ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
			}
			else{
				T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;
				draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no][disp_no_data.gr_no];

				//Set data to global data
				memcpy(draw_area_inf->area_size, drawArea, sizeof(T_Rect)*drawAreaNum);
				draw_area_inf->vaild_area_num = drawAreaNum;

				E_OSDLayer used_buffer = E_OSDLayer_LCD;
				if(disp_no_data.disp_block_no == E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_HDMI){
					used_buffer = E_OSDLayer_HDMI;
				}
				
				//Set every draw area's address
				Sdram_Map_Osd_Set_OSD_Draw_Area_Buffer_Addr(&disp_no_data,  drawArea, drawAreaNum, used_buffer);
			}
		}
	}
	return ret;
}

/**
Check the area to draw for overlap.
@param[in]	drawArea	the array of draw Area position.<br>
@param[in]	drawAreaNum	number of Area[1-10].<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE osd_comm_draw_area_overlap_check(const T_Rect drawArea[OSD_DRAW_AREA_NUM], BYTE drawAreaNum)
{
	FJ_ERR_CODE ret;
	T_Rect overlap_rect;
	BOOL ret_overlap;
	INT8 i,j;

	ret = FJ_ERR_OK;

	for (i = 0; i < drawAreaNum; i++){
		for (j = i+1; j < drawAreaNum; j++){
			ret_overlap = BF_Osd_Draw_Comm_Get_Overlap_Rect(drawArea[i],drawArea[j],&overlap_rect);
			if(ret_overlap == TRUE){
				ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
				break;
			}
		}
		if(ret != FJ_ERR_OK){
			break;
		}
	}

	return ret;
}

/**
Check the area to draw for size.
@param[in]	territory_size	OSD territory size.
@param[in]	drawArea	draw Area position.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE osd_comm_draw_area_size_check(U_IM_DISP_SIZE territory_size, const T_Rect* drawArea)
{
	FJ_ERR_CODE ret;

	ret = FJ_ERR_OK;

	// drawArea width check.(MIN,MAX)
	if(drawArea->Width < D_OSD_COM_GRISIZE_IHSIZE_MIN){
		ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
	}
	// drawArea width check.(4-unit)
	else if((drawArea->Width & 0x0003) != 0){
		ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
	}
	// drawArea width check.(territory_size)
	else if(drawArea->Postion.StartX + drawArea->Width > territory_size.size.width){
		ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
	}
	else{
		// drawArea height check.(MIN,MAX)
		if(drawArea->Height < D_OSD_COM_GRISIZE_IVSIZE_MIN){
			ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
		}
		// drawArea height check.(2-unit)
		else if((drawArea->Height & 0x0001) != 0){
			ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
		}
		// drawArea height check.(territory_size)
		else if(drawArea->Postion.StartY + drawArea->Height > territory_size.size.lines){
			ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
		}
		else{
			// DO NOTHING.
		}
	}

	return ret;
}

/**
Check the area to draw for postion.
@param[in]	last_drawArea	last draw Area position.
@param[in]	drawArea	draw Area position.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE osd_comm_draw_area_pos_check(const T_Rect* last_drawArea, const T_Rect* drawArea)
{
	FJ_ERR_CODE ret;

	ret = FJ_ERR_OK;
	// drawArea position check.(X)
	if (last_drawArea->Postion.StartX > drawArea->Postion.StartX) {
		ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
	}
	else if (last_drawArea->Postion.StartX == drawArea->Postion.StartX){
			// drawArea position check.(Y)
		if (last_drawArea->Postion.StartY > drawArea->Postion.StartY) {
			ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
		}
	}
	else{
		// DO NOTHING.
	}
	return ret;
}

/**
Sets the display state of the drawing area.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in]	enable_area	Each bit indicates the status display of the drawing area.<br>
			See @ref E_FJ_OSD_AREA for bit position.
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_set_osd_area_enable(E_OSDLayer layer_id, UINT32 enable_area)
{
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);
	T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;
	draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no][disp_no_data.gr_no];

	UINT32 draw_area = enable_area & 0x3ff;

	if(ret == FJ_ERR_OK){
		if(draw_area != 0) {
			UINT32 area_id[10];
			UINT32 area_num;
			area_num = BF_Osd_General_Get_Bit_Position(enable_area, area_id ,OSD_DRAW_AREA_NUM);
			
			if(area_id[area_num-1] > draw_area_inf->vaild_area_num){
				ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
			}
		}
	}

	if(ret == FJ_ERR_OK){
		Im_DISP_Set_OSD_Area_Enable(disp_no_data.disp_block_no, disp_no_data.layer, (ULONG)draw_area);
		draw_area_inf->enable_area = (USHORT)draw_area;
	}
	
	return ret;
}

/**
Gets the display state of the drawing area.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[out]	enable_area	Each bit indicates the status display of the drawing area.<br>
			See @ref E_FJ_OSD_AREA for bit position.
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_get_osd_area_enable(E_OSDLayer layer_id, UINT32* enable_area)
{
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);
	ULONG area;

	if(ret == FJ_ERR_OK){
		Im_DISP_Get_OSD_Area_Enable(disp_no_data.disp_block_no, disp_no_data.layer, &area);
		*enable_area = area;
	}
	return ret;
}

/**
Get the area to draw buffer address.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in]	drawAreaNum	The specification number of draw area[0-9].<br>
@param[out] global_width draw area of global width.<br>
@retval  The target osd draw buffer adrress.(Error developmental time is 0.)
*/
static ULONG fj_osd_get_osd_area_addr(E_OSDLayer layer_id, UINT32 drawAreaNum, UINT32* global_width)
{
	ULONG ret_addr = 0;
	FJ_ERR_CODE ret;
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;

	ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);
	draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no][disp_no_data.gr_no];

	if(ret == FJ_ERR_OK){
		ret = (drawAreaNum < draw_area_inf->vaild_area_num) ? FJ_ERR_OK : FJ_ERR_NG_INPUT_PARAM;
	}

	if(ret == FJ_ERR_OK){
		disp_no_data.draw_area = drawAreaNum;

		ret_addr = Sdram_Map_Osd_Get_OSD_Addr(&disp_no_data);

		if(global_width != NULL){
			*global_width = draw_area_inf->area_size[drawAreaNum].Width * FJ_OSD_COMM_RGBAXXXX;
		}
	}
	return ret_addr;
}

/**
Clear the drawing area.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in]	clear_area	Each bit indicates the status display of the drawing area.<br>
			See @ref E_FJ_OSD_AREA for bit position.
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_clear_area_buffer(E_OSDLayer layer_id, UINT32 clear_area)
{
	BYTE* data_address = 0;
	UINT32 clear_size = 0;
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;

	FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);

	if(ret == FJ_ERR_OK){
		INT32 vaild_area_num = gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no][disp_no_data.gr_no].vaild_area_num;
		for( disp_no_data.draw_area = 0; disp_no_data.draw_area < vaild_area_num; disp_no_data.draw_area++) {
			if((clear_area & (1<<disp_no_data.draw_area))>0) {
				data_address = (BYTE*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(Sdram_Map_Osd_Get_OSD_Chg_Addr(&disp_no_data));
				clear_size = 
					gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no]
														[disp_no_data.gr_no].area_size[disp_no_data.draw_area].Width *
					gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no]
														[disp_no_data.gr_no].area_size[disp_no_data.draw_area].Height *
					sizeof(OSD_USHORT_INT);
				memset(data_address, 0, clear_size);
			}
		}
	}

	return ret;
}

#if 0	// MILB Not Use
/**
Set tone correction data.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_MAIN		: LCD Main layer<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_MAIN		: HDMI Main layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in]	y_conv	Tone Correction Y convert coefficient.<br>
@param[in]	tc_data	Tone Correction  table control.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
FJ_ERR_CODE fj_osd_set_tone_correction(E_OSDLayer layer_id, T_Y_CONV const *const y_conv, T_TC_DATA const *const tc_data)
{
	U_IM_DISP_TCYCAL y_conversion;
	U_IM_DISP_TCCTL tc_tbl;
	E_IM_DISP_SEL_LAYER layer = E_IM_DISP_SEL_LAYER_MAIN;
	FJ_ERR_CODE ret = FJ_ERR_OK;

	switch(layer_id){
		case E_OSDLayer_LCD_MAIN:
			layer = E_IM_DISP_SEL_LAYER_MAIN;
			break;
		case E_OSDLayer_LCD_OSD0:
			layer = E_IM_DISP_SEL_LAYER_OSD_0;
			break;
		case E_OSDLayer_LCD_OSD1:
			layer = E_IM_DISP_SEL_LAYER_OSD_1;
			break;
		case E_OSDLayer_HDMI_MAIN:
			layer = E_IM_DISP_SEL_LAYER_MAIN;
			break;
		case E_OSDLayer_HDMI_OSD0:
			layer = E_IM_DISP_SEL_LAYER_OSD_0;
			break;
		case E_OSDLayer_HDMI_OSD1:
			layer = E_IM_DISP_SEL_LAYER_OSD_1;
			break;
		default:
			ret = FJ_ERR_NG_INPUT_PARAM;
			break;
	}
	
	if(ret == FJ_ERR_OK){
		y_conversion.word = 0;
		tc_tbl.word = 0;

		if (ret == FJ_ERR_OK){
			y_conversion.bit.TCYC00 = y_conv->y_conv0;
			y_conversion.bit.TCYC01 = y_conv->y_conv1;
			y_conversion.bit.TCYC02 = y_conv->y_conv2;
		
			switch (tc_data->tc_res){
				case E_TC_RESOLUTION_256:
					tc_tbl.bit.TCRES = E_IM_DISP_TCRES_256;
					break;
				case E_TC_RESOLUTION_128:
					tc_tbl.bit.TCRES = E_IM_DISP_TCRES_128;
					break;
				case E_TC_RESOLUTION_64:
					tc_tbl.bit.TCRES = E_IM_DISP_TCRES_64;
					break;
				default:
					// Parameter error.
					ret = FJ_ERR_NG_INPUT_PARAM;
					break;
			}
		
			tc_tbl.bit.TCTBL  = tc_data->tc_tbl;
			tc_tbl.bit.TCBLEN = tc_data->tc_blend;
			tc_tbl.bit.TCBLND = tc_data->tc_ratio;
		
			if (layer_id == E_OSDLayer_LCD_MAIN  ||
				layer_id == E_OSDLayer_HDMI_MAIN ){
				Im_DISP_Set_Tc(y_conversion, tc_tbl); 
			}
			else{
				if( Im_DISP_Set_OSD_Tc(layer, y_conversion, tc_tbl) != D_DDIM_OK) {
					ret = FJ_ERR_NG;
				}
			}
		}
	}
	return ret;
}
#endif	// MILB Not Use

#if 0	// MILB Not Use
/**
Get tone correction table.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_MAIN		: LCD Main layer<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_MAIN		: HDMI Main layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[out]	y_conv	Tone Correction Y convert coefficient.<br>
@param[out]	tc_data	Tone Correction  table control.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_get_tone_correction(E_OSDLayer layer_id, T_Y_CONV* y_conv, T_TC_DATA* tc_data)
{
	U_IM_DISP_TCYCAL y_conversion;
	U_IM_DISP_TCCTL tc_tbl;
	E_IM_DISP_SEL_LAYER layer = E_IM_DISP_SEL_LAYER_MAIN;
	FJ_ERR_CODE ret = FJ_ERR_OK;

	switch(layer_id){
		case E_OSDLayer_LCD_MAIN:
			layer = E_IM_DISP_SEL_LAYER_MAIN;
			break;
		case E_OSDLayer_LCD_OSD0:
			layer = E_IM_DISP_SEL_LAYER_OSD_0;
			break;
		case E_OSDLayer_LCD_OSD1:
			layer = E_IM_DISP_SEL_LAYER_OSD_1;
			break;
		case E_OSDLayer_HDMI_MAIN:
			layer = E_IM_DISP_SEL_LAYER_MAIN;
			break;
		case E_OSDLayer_HDMI_OSD0:
			layer = E_IM_DISP_SEL_LAYER_OSD_0;
			break;
		case E_OSDLayer_HDMI_OSD1:
			layer = E_IM_DISP_SEL_LAYER_OSD_1;
			break;
		default:
			ret = FJ_ERR_NG_INPUT_PARAM;
			break;
	}

	if(ret == FJ_ERR_OK){
		if (layer_id == E_OSDLayer_LCD_MAIN  ||
			layer_id == E_OSDLayer_HDMI_MAIN ){
			if( Im_DISP_Get_Tc(&y_conversion, &tc_tbl) != D_DDIM_OK) {
				ret = FJ_ERR_NG;
			}
		}
		else{
			if( Im_DISP_Get_OSD_Tc(layer, &y_conversion, &tc_tbl) != D_DDIM_OK) {
				ret = FJ_ERR_NG;
			}
		}

		if(ret == FJ_ERR_OK){
			y_conv->y_conv0 = y_conversion.bit.TCYC00;
			y_conv->y_conv1 = y_conversion.bit.TCYC01;
			y_conv->y_conv2 = y_conversion.bit.TCYC02;
		
			switch (tc_tbl.bit.TCRES){
				case E_IM_DISP_TCRES_256:
					tc_data->tc_res = E_TC_RESOLUTION_256;
					break;
				case E_IM_DISP_TCRES_128:
					tc_data->tc_res = E_TC_RESOLUTION_128;
					break;
				case E_IM_DISP_TCRES_64:
					tc_data->tc_res = E_TC_RESOLUTION_64;
					break;
				default:
					break;
			}
		
			tc_data->tc_tbl   = tc_tbl.bit.TCTBL;
			tc_data->tc_blend = tc_tbl.bit.TCBLEN;
			tc_data->tc_ratio = tc_tbl.bit.TCBLND;
		}
	}
	return ret;
}
#endif	// MILB Not Use

#if 0	// MILB Not Use
/**
Set tone correction table.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_MAIN		: LCD Main layer<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_MAIN		: HDMI Main layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in]	tone_correction		Tone correction table.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_set_tc_table(E_OSDLayer layer_id, T_TONE_CORRECTION_TBL const *const tone_correction)
{
	T_IM_DISP_TC_TBL tbl;
	E_IM_DISP_SEL_LAYER layer = E_IM_DISP_SEL_LAYER_MAIN;
	FJ_ERR_CODE ret = FJ_ERR_OK;

	switch(layer_id){
		case E_OSDLayer_LCD_MAIN:
			layer = E_IM_DISP_SEL_LAYER_MAIN;
			break;
		case E_OSDLayer_LCD_OSD0:
			layer = E_IM_DISP_SEL_LAYER_OSD_0;
			break;
		case E_OSDLayer_LCD_OSD1:
			layer = E_IM_DISP_SEL_LAYER_OSD_1;
			break;
		case E_OSDLayer_HDMI_MAIN:
			layer = E_IM_DISP_SEL_LAYER_MAIN;
			break;
		case E_OSDLayer_HDMI_OSD0:
			layer = E_IM_DISP_SEL_LAYER_OSD_0;
			break;
		case E_OSDLayer_HDMI_OSD1:
			layer = E_IM_DISP_SEL_LAYER_OSD_1;
			break;
		default:
			ret = FJ_ERR_NG_INPUT_PARAM;
			break;
	}

	if (ret == FJ_ERR_OK){
		tbl.tc = tone_correction->tc;
		tbl.tcep = tone_correction->tcep;
		
		if(Im_DISP_Set_Tc_Table(layer, &tbl) != D_DDIM_OK){
			ret = FJ_ERR_NG;
		}
	}
	
	return ret;
}
#endif	// MILB Not Use

#if 0	// MILB Not Use
/**
Get tone correction table.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_MAIN		: LCD Main layer<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_MAIN		: HDMI Main layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[out]	tone_correction		Tone correction table.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_get_tc_table(E_OSDLayer layer_id, T_TONE_CORRECTION_TBL* tone_correction)
{
	T_IM_DISP_TC_TBL tbl;
	E_IM_DISP_SEL_LAYER layer = E_IM_DISP_SEL_LAYER_MAIN;
	FJ_ERR_CODE ret = FJ_ERR_OK;

	switch(layer_id){
		case E_OSDLayer_LCD_MAIN:
			layer = E_IM_DISP_SEL_LAYER_MAIN;
			break;
		case E_OSDLayer_LCD_OSD0:
			layer = E_IM_DISP_SEL_LAYER_OSD_0;
			break;
		case E_OSDLayer_LCD_OSD1:
			layer = E_IM_DISP_SEL_LAYER_OSD_1;
			break;
		case E_OSDLayer_HDMI_MAIN:
			layer = E_IM_DISP_SEL_LAYER_MAIN;
			break;
		case E_OSDLayer_HDMI_OSD0:
			layer = E_IM_DISP_SEL_LAYER_OSD_0;
			break;
		case E_OSDLayer_HDMI_OSD1:
			layer = E_IM_DISP_SEL_LAYER_OSD_1;
			break;
		default:
			ret = FJ_ERR_NG_INPUT_PARAM;
			break;
	}

	if (ret == FJ_ERR_OK){
		tbl.tc = (USHORT*)g_tc;
		tbl.tcep = (USHORT*)g_tcep;

		if(Im_DISP_Get_Tc_Table(layer, &tbl) == D_DDIM_OK){
			tone_correction->tc = tbl.tc;
			tone_correction->tcep = tbl.tcep;
		}
		else{
			ret = FJ_ERR_NG;
		}
	}

	return ret;
}
#endif	// MILB Not Use

/**
Set inverse gamma table.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_MAIN		: LCD Main layer<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_MAIN		: HDMI Main layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in]	inverse_gamma	Inverse gamma table.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_set_inverse_gamma(E_OSDLayer layer_id, T_INVERSE_GAMMA_TBL const *const inverse_gamma)
{
	T_IM_DISP_ANTI_GAMMA_TBL gamma_tbl;
	E_IM_DISP_SEL disp_sel = E_IM_DISP_HDMI;
	BYTE surface = D_IM_DISP_TABLE_SURFACE_A;
	FJ_ERR_CODE ret = FJ_ERR_OK;

	switch(layer_id){
		case E_OSDLayer_LCD_MAIN:
			disp_sel = E_IM_DISP_LCD_MIPI;
			surface = D_IM_DISP_TABLE_SURFACE_A;
			break;
		case E_OSDLayer_LCD_OSD0:
		case E_OSDLayer_LCD_OSD1:
			disp_sel = E_IM_DISP_LCD_MIPI;
			surface = D_IM_DISP_TABLE_SURFACE_B;
			break;
		case E_OSDLayer_HDMI_MAIN:
			disp_sel = E_IM_DISP_HDMI;
			surface = D_IM_DISP_TABLE_SURFACE_A;
			break;
		case E_OSDLayer_HDMI_OSD0:
		case E_OSDLayer_HDMI_OSD1:
			disp_sel = E_IM_DISP_HDMI;
			surface = D_IM_DISP_TABLE_SURFACE_B;
			break;
		default:
			ret = FJ_ERR_NG_INPUT_PARAM;
			break;
	}

	if(ret == FJ_ERR_OK) {
		gamma_tbl.r_anti = inverse_gamma->r_anti;
		gamma_tbl.g_anti = inverse_gamma->g_anti;
		gamma_tbl.b_anti = inverse_gamma->b_anti;
		
		if(Im_DISP_Set_Anti_Gamma_Table(disp_sel, surface, &gamma_tbl) != D_DDIM_OK) {
			ret = FJ_ERR_NG;
		}
	}

	return ret;
}


/**
Get inverse gamma table.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_MAIN		: LCD Main layer<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_MAIN		: HDMI Main layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[out]	inverse_gamma	Inverse gamma table.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_get_inverse_gamma(E_OSDLayer layer_id, T_INVERSE_GAMMA_TBL *inverse_gamma)
{
	T_IM_DISP_ANTI_GAMMA_TBL gamma_tbl;
	E_IM_DISP_SEL disp_sel = E_IM_DISP_HDMI;
	BYTE surface = D_IM_DISP_TABLE_SURFACE_A;
	FJ_ERR_CODE ret = FJ_ERR_OK;

	switch(layer_id){
		case E_OSDLayer_LCD_MAIN:
			disp_sel = E_IM_DISP_LCD_MIPI;
			surface = D_IM_DISP_TABLE_SURFACE_A;
			break;
		case E_OSDLayer_LCD_OSD0:
		case E_OSDLayer_LCD_OSD1:
			disp_sel = E_IM_DISP_LCD_MIPI;
			surface = D_IM_DISP_TABLE_SURFACE_B;
			break;
		case E_OSDLayer_HDMI_MAIN:
			disp_sel = E_IM_DISP_HDMI;
			surface = D_IM_DISP_TABLE_SURFACE_A;
			break;
		case E_OSDLayer_HDMI_OSD0:
		case E_OSDLayer_HDMI_OSD1:
			disp_sel = E_IM_DISP_HDMI;
			surface = D_IM_DISP_TABLE_SURFACE_B;
			break;
		default:
			ret = FJ_ERR_NG_INPUT_PARAM;
			break;
	}

	if(ret == FJ_ERR_OK) {
		gamma_tbl.r_anti = (USHORT*)g_inverse_r;
		gamma_tbl.g_anti = (USHORT*)g_inverse_g;
		gamma_tbl.b_anti = (USHORT*)g_inverse_b;

		if( Im_DISP_Get_Anti_Gamma_Table(disp_sel, surface, &gamma_tbl) == D_DDIM_OK) {
			inverse_gamma->r_anti = gamma_tbl.r_anti;
			inverse_gamma->g_anti = gamma_tbl.g_anti;
			inverse_gamma->b_anti = gamma_tbl.b_anti;
		}
		else{
			ret = FJ_ERR_NG;
		}

	}
	return ret;
}

/**
Set gamma table.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_MAIN		: LCD Main layer<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_MAIN		: HDMI Main layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in]	gamma	Gamma table.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_set_gamma(E_OSDLayer layer_id, T_GAMMA_TBL const *const gamma)
{
	T_IM_DISP_GAMMA_TBL_IN gamma_tbl;
	T_IM_DISP_GAMMA_TBL_OUT gamma_tbl_out;
	E_IM_DISP_SEL disp_sel = E_IM_DISP_HDMI;
	BYTE surface = D_IM_DISP_TABLE_SURFACE_A;
	FJ_ERR_CODE ret = FJ_ERR_OK;

	switch(layer_id){
		case E_OSDLayer_LCD_MAIN:
			disp_sel = E_IM_DISP_LCD_MIPI;
			surface = D_IM_DISP_TABLE_SURFACE_A;
			break;
		case E_OSDLayer_LCD_OSD0:
		case E_OSDLayer_LCD_OSD1:
			disp_sel = E_IM_DISP_LCD_MIPI;
			surface = D_IM_DISP_TABLE_SURFACE_B;
			break;
		case E_OSDLayer_HDMI_MAIN:
			disp_sel = E_IM_DISP_HDMI;
			surface = D_IM_DISP_TABLE_SURFACE_A;
			break;
		case E_OSDLayer_HDMI_OSD0:
		case E_OSDLayer_HDMI_OSD1:
			disp_sel = E_IM_DISP_HDMI;
			surface = D_IM_DISP_TABLE_SURFACE_B;
			break;
		default:
			ret = FJ_ERR_NG_INPUT_PARAM;
			break;
	}

	if(ret == FJ_ERR_OK) {
		gamma_tbl.r_data = gamma->r_data;
		gamma_tbl.g_data = gamma->g_data;
		gamma_tbl.b_data = gamma->b_data;

		if( (layer_id == E_OSDLayer_LCD_MAIN) || (layer_id == E_OSDLayer_HDMI_MAIN) ){
			if( Im_DISP_Set_Main_Gamma_Table(disp_sel, surface, &gamma_tbl) != D_DDIM_OK) {
				ret = FJ_ERR_NG;
			}
		}
		else{
			if( Im_DISP_Set_Dcore_Gamma_Table(disp_sel, surface, &gamma_tbl_out) != D_DDIM_OK) {
				ret = FJ_ERR_NG;
			}
		}
	}
	return ret;
}

/**
Get gamma table.
@param[in] layer_id	layer ID.<br>
			@ref E_OSDLayer_LCD_MAIN		: LCD Main layer<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_MAIN		: HDMI Main layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[out]	gamma	Gamma table.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_get_gamma(E_OSDLayer layer_id, T_GAMMA_TBL *gamma)
{
	T_IM_DISP_GAMMA_TBL_IN	gamma_tbl;
	T_IM_DISP_GAMMA_TBL_OUT	gamma_tbl_out;
	E_IM_DISP_SEL disp_sel = E_IM_DISP_HDMI;
	BYTE surface = D_IM_DISP_TABLE_SURFACE_A;

	FJ_ERR_CODE ret = FJ_ERR_OK;

	switch(layer_id){
		case E_OSDLayer_LCD_MAIN:
			disp_sel = E_IM_DISP_LCD_MIPI;
			surface = D_IM_DISP_TABLE_SURFACE_A;
			break;
		case E_OSDLayer_LCD_OSD0:
			disp_sel = E_IM_DISP_LCD_MIPI;
			surface = D_IM_DISP_TABLE_SURFACE_B;
			break;
		case E_OSDLayer_LCD_OSD1:
			disp_sel = E_IM_DISP_LCD_MIPI;
			surface = D_IM_DISP_TABLE_SURFACE_B;
			break;
		case E_OSDLayer_HDMI_MAIN:
			disp_sel = E_IM_DISP_HDMI;
			surface = D_IM_DISP_TABLE_SURFACE_A;
			break;
		case E_OSDLayer_HDMI_OSD0:
			disp_sel = E_IM_DISP_HDMI;
			surface = D_IM_DISP_TABLE_SURFACE_B;
			break;
		case E_OSDLayer_HDMI_OSD1:
			disp_sel = E_IM_DISP_HDMI;
			surface = D_IM_DISP_TABLE_SURFACE_B;
			break;
		default:
			ret = FJ_ERR_NG_INPUT_PARAM;
			break;
	}

	if(ret == FJ_ERR_OK) {
		if( (layer_id == E_OSDLayer_LCD_MAIN) || (layer_id == E_OSDLayer_HDMI_MAIN) ){
			gamma_tbl.r_data		= (USHORT*)g_gamma_r_in;
			gamma_tbl.g_data		= (USHORT*)g_gamma_g_in;
			gamma_tbl.b_data		= (USHORT*)g_gamma_b_in;

			if( Im_DISP_Get_Main_Gamma_Table(disp_sel, surface, &gamma_tbl) == D_DDIM_OK) {
				gamma->r_data = gamma_tbl.r_data;
				gamma->g_data = gamma_tbl.g_data;
				gamma->b_data = gamma_tbl.b_data;
			}
			else{
				ret = FJ_ERR_NG;
			}
		}
		else{
			gamma_tbl_out.r_data		= (USHORT*)g_gamma_r_out;
			gamma_tbl_out.g_data		= (USHORT*)g_gamma_g_out;
			gamma_tbl_out.b_data		= (USHORT*)g_gamma_b_out;

			if( Im_DISP_Get_Dcore_Gamma_Table(disp_sel, surface, &gamma_tbl_out) == D_DDIM_OK) {
				gamma->r_data = gamma_tbl_out.r_data;
				gamma->g_data = gamma_tbl_out.g_data;
				gamma->b_data = gamma_tbl_out.b_data;
			}
			else{
				ret = FJ_ERR_NG;
			}
		}
	}
	return ret;
}


/**
Set enable Gamma correct or Anti-Gamma correct or gradation adjustment.
@param[in]	disp_type	display type.<br>
@param[in]	select	Set of features that enable.<br>
@param[in]	compensation_table	Table state setting.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
@remarks When changing possible, it needs 1 usec till the end of procedure.<br>
*/
static FJ_ERR_CODE fj_osd_set_compensation_table_enable(FJ_DISP_TYPE disp_type, E_CORRECT_SELECT select, T_TBL_ACCESS_SET const *const compensation_table)
{
	E_IM_DISP_CORRECT_SELECT co_sel = E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA;
	T_IM_DISP_TBL_ACCESS_SET access_set;
	FJ_ERR_CODE ret = FJ_ERR_OK;
	E_IM_DISP_SEL block = E_IM_DISP_HDMI;

	if( disp_type == FJ_DISP_TYPE_HDMI ){
		block = E_IM_DISP_HDMI;
	}
	else if( disp_type == FJ_DISP_TYPE_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		ret = FJ_ERR_NG;
	}

	if(ret == FJ_ERR_OK) {
		switch (select){
			case E_CORRECT_SELECT_IN_INVERSE_GAMMA:
				co_sel = E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA;
				break;
			case E_CORRECT_SELECT_IN_GAMMA:
				co_sel = E_IM_DISP_CORRECT_SELECT_GAMMA_IN;
				break;
			case E_CORRECT_SELECT_OUT_GAMMA:
				co_sel = E_IM_DISP_CORRECT_SELECT_GAMMA_OUT;
				break;
			case E_CORRECT_SELECT_OUT_LUMINANCE:
				co_sel = E_IM_DISP_CORRECT_SELECT_LUMINANCE;
				break;
			case E_CORRECT_SELECT_OUT_CHROMA:
				co_sel = E_IM_DISP_CORRECT_SELECT_CHROMA;
				break;
			default:
				// Parameter error.
				ret = FJ_ERR_NG_INPUT_PARAM;
				break;
		}

		if (ret == FJ_ERR_OK){
			access_set.access = (E_IM_DISP_TABLE_ACCESS)compensation_table->access;
			access_set.proc = (E_IM_DISP_CORRECT_PROC)compensation_table->proc;
		
			if( Im_DISP_Set_Gamma_Enable(block, co_sel, &access_set) != D_DDIM_OK) {
				ret = FJ_ERR_NG;
			}
		}
	}
	return ret;
}

/**
Get enable Gamma correct or Anti-Gamma correct or gradation adjustment.
@param[in]	disp_type	display type.<br>
@param[in]	select	Set of features that enable.<br>
@param[out]	compensation_table	Table state setting.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_get_compensation_table_enable(FJ_DISP_TYPE disp_type, E_CORRECT_SELECT select, T_TBL_ACCESS_SET *compensation_table)
{
	U_IM_DISP_TBLASET dcore_tblaset;
	U_IM_DISP_LTBLASET lch_ltblaset;
	FJ_ERR_CODE ret = FJ_ERR_OK;
	E_IM_DISP_SEL block = E_IM_DISP_HDMI;

	if( disp_type == FJ_DISP_TYPE_HDMI ){
		block = E_IM_DISP_HDMI;
	}
	else if( disp_type == FJ_DISP_TYPE_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		ret = FJ_ERR_NG;
	}

	lch_ltblaset.bit.IGTAEN = 0;
	lch_ltblaset.bit.IGEN = 0;
	lch_ltblaset.bit.IGTSL = 0;
	lch_ltblaset.bit.GTAEN = 0;
	lch_ltblaset.bit.GMEN = 0;
	lch_ltblaset.bit.GMTSL = 0;
	lch_ltblaset.bit.IGTMN = 0;
	lch_ltblaset.bit.GMTMN = 0;

	dcore_tblaset.bit.GMTAEN = 0;
	dcore_tblaset.bit.GMEN = 0;
	dcore_tblaset.bit.GMTSL = 0;
	dcore_tblaset.bit.YSATTA = 0;
	dcore_tblaset.bit.YSATEN = 0;
	dcore_tblaset.bit.YSATSL = 0;
	dcore_tblaset.bit.GMTMN = 0;
	dcore_tblaset.bit.YSATMN = 0;

	if (ret == FJ_ERR_OK){
		if( Im_DISP_Get_Gamma_Enable(block, &lch_ltblaset, &dcore_tblaset) != D_DDIM_OK) {
			ret = FJ_ERR_NG;
		}
	}

	if (ret == FJ_ERR_OK){
		switch (select){
			case E_CORRECT_SELECT_IN_INVERSE_GAMMA:	
				compensation_table->access = (E_TABLE_ACCESS)lch_ltblaset.bit.IGTAEN;
				compensation_table->proc = (E_CORRECT_PROC)lch_ltblaset.bit.IGEN;
				break;
			case E_CORRECT_SELECT_IN_GAMMA:
				compensation_table->access = (E_TABLE_ACCESS)lch_ltblaset.bit.GTAEN;
				compensation_table->proc = (E_CORRECT_PROC)lch_ltblaset.bit.GMEN;
				break;
			case E_CORRECT_SELECT_OUT_GAMMA:
				compensation_table->access = (E_TABLE_ACCESS)dcore_tblaset.bit.GMTAEN;
				compensation_table->proc = (E_CORRECT_PROC)dcore_tblaset.bit.GMEN;
				break;
			case E_CORRECT_SELECT_OUT_LUMINANCE:
				compensation_table->access = (E_TABLE_ACCESS)dcore_tblaset.bit.YSATTA;
				compensation_table->proc = (E_CORRECT_PROC)dcore_tblaset.bit.YSATEN;
				break;
			case E_CORRECT_SELECT_OUT_CHROMA:
				compensation_table->access = (E_TABLE_ACCESS)dcore_tblaset.bit.YSATTA;
				compensation_table->proc = (E_CORRECT_PROC)dcore_tblaset.bit.YSATEN;
				break;
			default:
				// Parameter error.
				ret = FJ_ERR_NG_INPUT_PARAM;
				break;
		}
	}
	return ret;
}

/**
Set order of the layer.
@param[in] disp_type	display type.<br>
			@ref FJ_DISP_TYPE_LCD		: LCD<br>
			@ref FJ_DISP_TYPE_HDMI		: HDMI<br>
@param[in]	order_layer	order of the layer.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_set_layer_order(FJ_DISP_TYPE disp_type, T_ORDER_LAYER const *const order_layer)
{
	U_IM_DISP_BLDCTL order;
	FJ_ERR_CODE ret = FJ_ERR_OK;
	E_IM_DISP_SEL block = E_IM_DISP_HDMI;

	order.word = 0;

	if( order_layer == NULL){
		ret = FJ_ERR_NG_INPUT_PARAM;
	}

	if( disp_type == FJ_DISP_TYPE_HDMI ){
		block = E_IM_DISP_HDMI;
	}
	else if( disp_type == FJ_DISP_TYPE_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	
	if(ret == FJ_ERR_OK){
		BYTE i,j;
		for(i=0; i<E_ORDER_NUM_OF_LAYER; i++){
			if(ret != FJ_ERR_OK){
				break;
			}

			if(order_layer->layer_id[E_ORDER_BOTTOM] & E_OSDLayer_LCD){
				if (order_layer->layer_id[i] != E_OSDLayer_LCD_OSD0 &&
					order_layer->layer_id[i] != E_OSDLayer_LCD_OSD1 &&
					order_layer->layer_id[i] != E_OSDLayer_LCD_GRID &&
					order_layer->layer_id[i] != E_OSDLayer_LCD_FACE ){
					ret = FJ_ERR_NG_INPUT_PARAM;
					break;
				}
			}
			else{
				if (order_layer->layer_id[i] != E_OSDLayer_HDMI_OSD0 &&
					order_layer->layer_id[i] != E_OSDLayer_HDMI_OSD1 &&
					order_layer->layer_id[i] != E_OSDLayer_HDMI_GRID &&
					order_layer->layer_id[i] != E_OSDLayer_HDMI_FACE ){
					ret = FJ_ERR_NG_INPUT_PARAM;
					break;
				}
				
			}
			for(j=i+1; j<E_ORDER_NUM_OF_LAYER; j++){
				if(order_layer->layer_id[i] == order_layer->layer_id[j]){
					ret = FJ_ERR_NG_INPUT_PARAM;
					break;
				}
			}
		}
	}

	if(ret == FJ_ERR_OK){
		order.bit.BLD1 = BF_Osd_General_Conv_Layer_To_Define(order_layer->layer_id[E_ORDER_BOTTOM]);
		order.bit.BLD2 = BF_Osd_General_Conv_Layer_To_Define(order_layer->layer_id[E_ORDER_SECOND]);
		order.bit.BLD3 = BF_Osd_General_Conv_Layer_To_Define(order_layer->layer_id[E_ORDER_THIRD]);
		order.bit.BLD4 = BF_Osd_General_Conv_Layer_To_Define(order_layer->layer_id[E_ORDER_TOP]);
		Im_DISP_Set_Blend(block, order);
	}

	return ret;
}

/**
Get order of the layer.
@param[in] disp_type	display type.<br>
			@ref FJ_DISP_TYPE_LCD		: LCD<br>
			@ref FJ_DISP_TYPE_HDMI		: HDMI<br>
@param[out]	order_layer	order of the layer.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_get_layer_order(FJ_DISP_TYPE disp_type, T_ORDER_LAYER *order_layer)
{
	U_IM_DISP_BLDCTL order;
	FJ_ERR_CODE ret = FJ_ERR_OK;
	E_IM_DISP_SEL block = E_IM_DISP_HDMI;

	if( disp_type == FJ_DISP_TYPE_HDMI ){
		block = E_IM_DISP_HDMI;
	}
	else if( disp_type == FJ_DISP_TYPE_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	
	if( Im_DISP_Get_Blend(block, &order)  
		!= D_DDIM_OK) {
		ret = FJ_ERR_NG;
	}
	else {
		order_layer->layer_id[E_ORDER_BOTTOM] = BF_Osd_General_Conv_Define_To_Layer(disp_type, order.bit.BLD1);
		order_layer->layer_id[E_ORDER_SECOND] = BF_Osd_General_Conv_Define_To_Layer(disp_type, order.bit.BLD2);
		order_layer->layer_id[E_ORDER_THIRD] = BF_Osd_General_Conv_Define_To_Layer(disp_type, order.bit.BLD3);
		order_layer->layer_id[E_ORDER_TOP] = BF_Osd_General_Conv_Define_To_Layer(disp_type, order.bit.BLD4);
	}

	return ret;
}

/**
Draw grid line.
@param[in] disp_type	display type.<br>
			@ref FJ_DISP_TYPE_LCD		: LCD<br>
			@ref FJ_DISP_TYPE_HDMI		: HDMI<br>
@param[in]	grid_set	Grid setting.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
@remarks If odd number is set to the parameter whose number should be even, it will be used taking -1. <br>
*/
static FJ_ERR_CODE fj_osd_draw_grid_line(FJ_DISP_TYPE disp_type, T_GRID_SET const *const grid_set)
{
	T_IM_DISP_GRID grid_setting;
	FJ_ERR_CODE ret = FJ_ERR_OK;
	E_IM_DISP_SEL block = E_IM_DISP_HDMI;

	if( disp_type == FJ_DISP_TYPE_HDMI ){
		block = E_IM_DISP_HDMI;
	}
	else if( disp_type == FJ_DISP_TYPE_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		ret = FJ_ERR_NG_INPUT_PARAM;
	}

	if (ret == FJ_ERR_OK){
		U_IM_DISP_SIZE 		input_size;	
		T_IM_DISP_RESIZE	resize;
		U_IM_DISP_DSTA 		position;
	
		Im_DISP_Get_Input_Size(block, &input_size);
		Im_DISP_Get_Resize(block, &resize);
		Im_DISP_Get_Display_Position(block, &position);

		SHORT ratio_calc_H_M = input_size.size.width * resize.rsz1.bit.HRSZM;
		SHORT ratio_calc_H_N = grid_set->ih_size * resize.rsz1.bit.HRSZN;
		SHORT ratio_calc_V_M = input_size.size.lines * resize.rsz2.bit.VRSZM;
		SHORT ratio_calc_V_N = grid_set->iv_size * resize.rsz2.bit.VRSZN;

		grid_setting.ghdsta.bit.DSH = grid_set->h_pos.Postion.StartX * ratio_calc_H_M / ratio_calc_H_N + position.bit.DSH;
		grid_setting.ghdsta.bit.DSH &= 0xFFFE;
		grid_setting.ghdsta.bit.DSV = grid_set->h_pos.Postion.StartY * ratio_calc_V_M / ratio_calc_V_N + position.bit.DSV;
		grid_setting.ghdsta.bit.DSV &= 0x3FFE;
		grid_setting.gvdsta.bit.DSH = grid_set->v_pos.Postion.StartX * ratio_calc_H_M / ratio_calc_H_N + position.bit.DSH;
		grid_setting.gvdsta.bit.DSH &= 0xFFFE;
		grid_setting.gvdsta.bit.DSV = grid_set->v_pos.Postion.StartY * ratio_calc_V_M / ratio_calc_V_N + position.bit.DSV;
		grid_setting.gvdsta.bit.DSV &= 0x3FFE;

		grid_setting.glength.bit.GHLEN = grid_set->h_pos.Width * ratio_calc_H_M / ratio_calc_H_N;
		grid_setting.glength.bit.GHLEN &= 0xFFFE;
		grid_setting.glength.bit.GVLEN = grid_set->v_pos.Height * ratio_calc_V_M / ratio_calc_V_N;
		grid_setting.glength.bit.GVLEN &= 0x3FFE;

		grid_setting.gwidth.bit.GHWID = grid_set->h_pos.Height * ratio_calc_V_M / ratio_calc_V_N;
		grid_setting.gwidth.bit.GHWID &= 0x3E;
		grid_setting.gwidth.bit.GVWID = grid_set->v_pos.Width * ratio_calc_H_M / ratio_calc_H_N;
		grid_setting.gwidth.bit.GVWID &= 0x3E;

		grid_setting.gitvl.bit.GHITV = grid_set->h_interval * ratio_calc_V_M / ratio_calc_V_N;
		grid_setting.gitvl.bit.GHITV &= 0x3FFE;
		grid_setting.gitvl.bit.GVITV = grid_set->v_interval * ratio_calc_H_M / ratio_calc_H_N;
		grid_setting.gitvl.bit.GVITV &= 0xFFFE;

		grid_setting.gnum.bit.GHNUM = grid_set->h_numLine - 1;
		grid_setting.gnum.bit.GVNUM = grid_set->v_numLine - 1;

		grid_setting.gdctl.bit.GCLRR = grid_set->rgba.RGBA.R;
		grid_setting.gdctl.bit.GCLRG = grid_set->rgba.RGBA.G;
		grid_setting.gdctl.bit.GCLRB = grid_set->rgba.RGBA.B;
		grid_setting.gdctl.bit.GALP = grid_set->rgba.RGBA.A;;
		
		ret = osd_comm_draw_grid_line_param_check(block, &grid_setting);
		if (ret == FJ_ERR_OK){
			if( Im_DISP_Set_Grid(block, &grid_setting) != D_DDIM_OK) {
				ret = FJ_ERR_NG;
			}
		}
	}
	return ret;
}

/**
Set grid line utility.
@param[in] disp_type	display type.<br>
			@ref FJ_DISP_TYPE_LCD		: LCD<br>
			@ref FJ_DISP_TYPE_HDMI		: HDMI<br>
@param[in]	grid_num_set	Grid num setting.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
@remarks If odd number is set to the parameter whose number should be even, it will be used taking -1. <br>
*/
FJ_ERR_CODE fj_osd_set_grid_line_num(FJ_DISP_TYPE disp_type, T_GRID_NUM_SET const *const grid_num_set)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	UCHAR grid_spase = 0;
	T_IM_DISP_GRID grid;
	E_IM_DISP_SEL block = E_IM_DISP_HDMI;

	if( disp_type == FJ_DISP_TYPE_HDMI ){
		block = E_IM_DISP_HDMI;
	}
	else if( disp_type == FJ_DISP_TYPE_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		ret = FJ_ERR_NG_INPUT_PARAM;
	}

	if (ret == FJ_ERR_OK){
		 U_IM_DISP_SIZE 		input_size;	
		 T_IM_DISP_RESIZE	resize;
		 U_IM_DISP_DSTA 		position;
		 ULONG ghitv;
		 ULONG gvitv;
		 ULONG ghwid;
		 ULONG gvwid;
		 ULONG ghlen;
		 ULONG gvlen;
		
		Im_DISP_Get_Input_Size(block, &input_size);
		Im_DISP_Get_Resize(block, &resize);
		Im_DISP_Get_Display_Position(block, &position);
		
		//Thickness (Calculation).
		//Use screen's vertical lines because horizonal line's thickness was set
		ghwid = grid_num_set->width.bit.horizontal_line_width * input_size.size.lines / grid_num_set->iv_size;
		//Use screen's horizonal width because vertical line's thickness was set
		gvwid = grid_num_set->width.bit.vertical_line_width * input_size.size.width / grid_num_set->ih_size;

		//length (Calculation).
		ghlen = input_size.size.width - grid_spase*2;
		gvlen = input_size.size.lines - grid_spase*2;

		//Interval (Calculation).
		ghitv = (input_size.size.lines + ghwid) / (1 + grid_num_set->h_numLine);
		gvitv = (input_size.size.width + gvwid) / (1 + grid_num_set->v_numLine);

		//H Grid Line start positon.
		grid.ghdsta.bit.DSH = grid_spase + position.bit.DSH;
		grid.ghdsta.bit.DSH &= 0xFFFE;
		grid.ghdsta.bit.DSV = ((ghitv - ghwid) * resize.rsz2.bit.VRSZM / resize.rsz2.bit.VRSZN) + position.bit.DSV;
		grid.ghdsta.bit.DSV &= 0x3FFE;
		
		//V Grid Line start positon.
		grid.gvdsta.bit.DSH = ((gvitv - gvwid) * resize.rsz1.bit.HRSZM / resize.rsz1.bit.HRSZN)  + position.bit.DSH;
		grid.gvdsta.bit.DSH &= 0xFFFE;
		grid.gvdsta.bit.DSV = grid_spase + position.bit.DSV;
		grid.gvdsta.bit.DSV &= 0x3FFE;
	
		//Grid Line length.
		grid.glength.bit.GHLEN = ghlen * resize.rsz1.bit.HRSZM / resize.rsz1.bit.HRSZN;
		grid.glength.bit.GHLEN &= 0xFFFE;
		grid.glength.bit.GVLEN = gvlen * resize.rsz2.bit.VRSZM / resize.rsz2.bit.VRSZN;
		grid.glength.bit.GVLEN &= 0x3FFE;
		
		//Grid Line thickness.
		grid.gwidth.bit.GHWID = ghwid * resize.rsz1.bit.HRSZM / resize.rsz1.bit.HRSZN;
		grid.gwidth.bit.GHWID &= 0x3E;
		grid.gwidth.bit.GHWID = BF_Osd_General_Get_Max(grid.gwidth.bit.GHWID,2);
		grid.gwidth.bit.GVWID = gvwid * resize.rsz2.bit.VRSZM / resize.rsz2.bit.VRSZN;
		grid.gwidth.bit.GVWID &= 0x3E;
		grid.gwidth.bit.GVWID = BF_Osd_General_Get_Max(grid.gwidth.bit.GVWID,2);
	
		//Grid Line Interval.
		grid.gitvl.bit.GHITV = ghitv * resize.rsz2.bit.VRSZM / resize.rsz2.bit.VRSZN;
		grid.gitvl.bit.GHITV &= 0x3FFE;
		grid.gitvl.bit.GVITV = gvitv * resize.rsz1.bit.HRSZM / resize.rsz1.bit.HRSZN;
		grid.gitvl.bit.GVITV &= 0xFFFE;
		
		//Grid Line num.
		grid.gnum.bit.GHNUM = grid_num_set->h_numLine - 1;
		grid.gnum.bit.GVNUM = grid_num_set->v_numLine - 1;
	
		//Grid Line color.
		grid.gdctl.bit.GCLRR = grid_num_set->rgba.RGBA.R;
		grid.gdctl.bit.GCLRG = grid_num_set->rgba.RGBA.G;
		grid.gdctl.bit.GCLRB = grid_num_set->rgba.RGBA.B;
		grid.gdctl.bit.GALP = grid_num_set->rgba.RGBA.A;;
	
		ret = osd_comm_draw_grid_line_param_check(block, &grid);
		if (ret == FJ_ERR_OK){
			if( Im_DISP_Set_Grid(block, &grid) != D_DDIM_OK) {
				ret = FJ_ERR_NG;
			}
		}
	}
	return ret;
}

/**
Check the glid line draw param.
@param[in]	block		common block number.
@param[in]	grid		grid line param.
@param[in]	ovsize		Vertical output size
@param[in]	ohsize		Horizonal output size
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE osd_comm_draw_grid_line_param_check(E_IM_DISP_SEL block, T_IM_DISP_GRID const *const grid)
{
	FJ_ERR_CODE ret;
	ULONG ovsize;
	ULONG ohsize;

	ret = FJ_ERR_OK;
	Im_DISP_Get_Output_Size(block, &ovsize, &ohsize);

	// grid line draw param. GLENGTH check.
	if (grid->glength.bit.GHLEN < D_OSD_COM_GLENGTH_GHLEN_MIN ||
			(grid->ghdsta.bit.DSH + grid->glength.bit.GHLEN) > ohsize ){
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	else if (grid->glength.bit.GVLEN < D_OSD_COM_GLENGTH_GVLEN_MIN ||
			(grid->gvdsta.bit.DSV + grid->glength.bit.GVLEN) > ovsize ){
		ret = FJ_ERR_NG_INPUT_PARAM;
	}

	// grid line draw param. GITVL check.
	else if (grid->gitvl.bit.GHITV < (D_OSD_COM_GITVL_GHITV_MIN_BASE + grid->gwidth.bit.GHWID)){
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	else if (grid->gitvl.bit.GVITV < (D_OSD_COM_GITVL_GVITV_MIN_BASE + grid->gwidth.bit.GVWID)){
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	else if(grid->gvdsta.bit.DSH + grid->gwidth.bit.GVWID + (grid->gitvl.bit.GVITV * grid->gnum.bit.GVNUM) > ohsize){
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	else if(grid->ghdsta.bit.DSV + grid->gwidth.bit.GHWID + (grid->gitvl.bit.GHITV * grid->gnum.bit.GHNUM) > ovsize){
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	else{
		// DO NOTHING.
	}

	return ret;
}

/**
Set enable grid line.
@param[in] disp_type	display type.<br>
			@ref FJ_DISP_TYPE_LCD		: LCD<br>
			@ref FJ_DISP_TYPE_HDMI		: HDMI<br>
@param[in]	enable	enable/disable flag(TRUE:grid ON, FALSE:grid OFF).<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_set_grid_line_enable(FJ_DISP_TYPE disp_type, BOOL enable)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	
	Im_DISP_Set_Grid_Enable(disp_type, (E_IM_DISP_GDISPEN)enable);

	return ret;
}

/**
Draw face frame.
@param[in] disp_type	display type.<br>
			@ref FJ_DISP_TYPE_LCD		: LCD<br>
			@ref FJ_DISP_TYPE_HDMI		: HDMI<br>
@param[in]	face_pos	Face frame each setting.<br>
@param[in]	face_set	Face frame setting.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
@remarks If odd number is set to the parameter whose number should be even, it will be used taking -1. <br>
*/
static FJ_ERR_CODE fj_osd_draw_face_frame(FJ_DISP_TYPE disp_type, const T_FACE_FRAME_EACH_SET face_pos[OSD_DRAW_FACE_NUM], const T_FACE_FRAME_SET* face_set)
{
	T_IM_DISP_FACE_FRAME face;
	FJ_ERR_CODE ret = FJ_ERR_OK;
	USHORT i;
	ULONG ovsize,ohsize;
	
	ovsize = ohsize = 0;
	U_IM_DISP_SIZE 		input_size;	
	T_IM_DISP_RESIZE	resize;
	U_IM_DISP_DSTA 		position;

	E_IM_DISP_SEL block = E_IM_DISP_HDMI;

	if( disp_type == FJ_DISP_TYPE_HDMI ){
		block = E_IM_DISP_HDMI;
	}
	else if( disp_type == FJ_DISP_TYPE_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		ret = FJ_ERR_NG_INPUT_PARAM;
	}

	memset( &face, 0, sizeof(face) );
	
	Im_DISP_Get_Input_Size(block, &input_size);
	Im_DISP_Get_Output_Size(block, &ovsize, &ohsize);
	Im_DISP_Get_Resize(block, &resize);
	Im_DISP_Get_Display_Position(block, &position);

	if (ret == FJ_ERR_OK){
		for(i=0; i<face_set->face_count; i++){
			SHORT ratio_calc_H_M = input_size.size.width * resize.rsz1.bit.HRSZM;
			SHORT ratio_calc_H_N = face_set->ih_size * resize.rsz1.bit.HRSZN;
			SHORT ratio_calc_V_M = input_size.size.lines * resize.rsz2.bit.VRSZM;
			SHORT ratio_calc_V_N = face_set->iv_size * resize.rsz2.bit.VRSZN;

			face.param[i].ffdsta.bit.DSH = (face_pos[i].start_pos.Position.StartX * ratio_calc_H_M / ratio_calc_H_N) + position.bit.DSH;
			face.param[i].ffdsta.bit.DSH &= 0xFFFE;
			face.param[i].ffdsta.bit.DSV = (face_pos[i].start_pos.Position.StartY * ratio_calc_V_M / ratio_calc_V_N) + position.bit.DSV;
			face.param[i].ffdsta.bit.DSV &= 0x3FFE;

			face.param[i].ffsize.bit.FFHSZ = face_pos[i].start_pos.Width * ratio_calc_H_M / ratio_calc_H_N;
			face.param[i].ffsize.bit.FFHSZ &= 0xFFFE;
			face.param[i].ffsize.bit.FFVSZ = face_pos[i].start_pos.Height * ratio_calc_V_M / ratio_calc_V_N;
			face.param[i].ffsize.bit.FFVSZ &= 0x3FFE;

			face.param[i].ffwidth.bit.FFHWID = face_pos[i].v_thickness * ratio_calc_V_M / ratio_calc_V_N;
			face.param[i].ffwidth.bit.FFHWID &= 0x3E;
			face.param[i].ffwidth.bit.FFVWID = face_pos[i].h_thickness * ratio_calc_H_M / ratio_calc_H_N;
			face.param[i].ffwidth.bit.FFVWID &= 0x3E;

			face.param[i].ffclr.bit.FFCLRR = face_pos[i].rgba.RGBA.R;
			face.param[i].ffclr.bit.FFCLRG = face_pos[i].rgba.RGBA.G;
			face.param[i].ffclr.bit.FFCLRB = face_pos[i].rgba.RGBA.B;

			ret = osd_comm_draw_face_frame_param_check(&face.param[i],ovsize,ohsize);
			if (ret != FJ_ERR_OK){
				break;
			}
		}

		if (ret == FJ_ERR_OK){
			if (Im_DISP_Set_Face_Frame(block, face_set->face_count, &face) != D_DDIM_OK){
				ret = FJ_ERR_NG;
			}
		}
	}
	
	return ret;
}

/**
Check the face frame draw param.
@param[in]	face_param	face frame param.
@param[in]	ovsize		Vertical output size
@param[in]	ohsize		Horizonal output size
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE osd_comm_draw_face_frame_param_check(const T_IM_DISP_FACE_FRAME_PARAM* face_param, ULONG ovsize, ULONG ohsize)
{
	FJ_ERR_CODE ret;

	ret = FJ_ERR_OK;

	// face frame draw param. FFSIZE check.
	if(face_param->ffsize.bit.FFHSZ < D_OSD_COM_FFSIZE_FFHSZ_MIN ||
			(face_param->ffdsta.bit.DSH + face_param->ffsize.bit.FFHSZ + face_param->ffwidth.bit.FFVWID) > ohsize){
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	else if(face_param->ffsize.bit.FFVSZ < D_OSD_COM_FFSIZE_FFVSZ_MIN ||
			(face_param->ffdsta.bit.DSV + face_param->ffsize.bit.FFVSZ + face_param->ffwidth.bit.FFHWID) > ovsize){
		ret = FJ_ERR_NG_INPUT_PARAM;
	}

	// face frame draw param. FFWIDTH check.
	else if(face_param->ffwidth.bit.FFHWID < D_OSD_COM_FFWIDTH_FFHWID_MIN){
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	else if(face_param->ffwidth.bit.FFVWID < D_OSD_COM_FFWIDTH_FFVWID_MIN){
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	else{
		// DO NOTHING.
	}

	return ret;
}

/**
Set enable face frame.
@param[in] disp_type	display type.<br>
			@ref FJ_DISP_TYPE_LCD		: LCD<br>
			@ref FJ_DISP_TYPE_HDMI		: HDMI<br>
@param[in]	enable	Each bit indicates the status display of the face frame.<br>
@param[in]	top_face	Value range:0~15 (face frame 0 ~ face frame 15).<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_set_face_frame_enable(FJ_DISP_TYPE disp_type, UINT32 enable, UINT32 top_face)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;

	E_IM_DISP_SEL block = E_IM_DISP_HDMI;

	if( disp_type == FJ_DISP_TYPE_HDMI ){
		block = E_IM_DISP_HDMI;
	}
	else if( disp_type == FJ_DISP_TYPE_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	
	if( ret == FJ_ERR_OK ){
		Im_DISP_Set_Face_Frame_Enable(block, (USHORT)enable, (BYTE)top_face);
	}
	return ret;
}

/**
Set Color bar parameter.
@param[in] disp_type	display type.<br>
			@ref FJ_DISP_TYPE_LCD		: LCD<br>
			@ref FJ_DISP_TYPE_HDMI		: HDMI<br>
@param[in] color			Color bar setting color. See @ref U_RGB_COLOR.<br>
@param[in] color_bar_size	Color bar's number that is size of color. It must not be over 16
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
static FJ_ERR_CODE fj_osd_set_color_bar(FJ_DISP_TYPE disp_type, U_RGB_COLOR color[OSD_COLOR_BAR_NUM], ULONG color_bar_num)
{
	FJ_ERR_CODE ret = FJ_ERR_NG_INPUT_PARAM;
	U_IM_DISP_IMAGE_COLOR* clbdt = (U_IM_DISP_IMAGE_COLOR*)color;

	ULONG ovsize, ohsize,color_bar_size;

	E_IM_DISP_SEL block = E_IM_DISP_HDMI;
	if( disp_type == FJ_DISP_TYPE_HDMI ){
		block = E_IM_DISP_HDMI;
	}
	else if( disp_type == FJ_DISP_TYPE_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		ret = FJ_ERR_NG_INPUT_PARAM;
	}

	if( Im_DISP_Get_Output_Size(block, &ovsize, &ohsize) == D_DDIM_OK) {
		// set color bar size
		color_bar_size = ohsize / color_bar_num;
		Im_DISP_Set_Color_Bar_Size(block, color_bar_size);
	
		// set color
		if( Im_DISP_Set_Color_Bar(block, clbdt,color_bar_num)  == D_DDIM_OK) {

			ret = FJ_ERR_OK;
		}
	}

	return ret;
}

/**
The kind of the target object for the processing such as drawing is selected. 
@param[in] ulFontId icon kind.<br>
		@ref E_Draw_Type_OSD_Common_ICON - Draw OSD common icon<br>
		@ref E_Draw_Type_OSD_Reload_ICON - Draw OSD reload icon<br>
		@ref E_Draw_Type_HDMI_Common_ICON - Draw HDMI common icon<br>
		@ref E_Draw_Type_HDMI_Reload_ICON - Draw HDMI reload icon<br>
@return In case of success, the function returns TRUE.<br>
		In case of failure, the function returns FALSE.
@remarks There are 2 icon data areas. One is XXX_Common_ICON that usually is used to load <br>
		some icons that are often used. The other is XXX_Reload_ICON that usually is used to <br>
		load some icons that are not often used and this area often uesd to overwrite by another<br>
		icons if all icons can not be load to memory.
*/
static BOOL fj_directselectfont(E_Draw_Type ulFontId)
{
	BOOL ret = TRUE;

	switch (ulFontId) {
		case E_Draw_Type_OSD_Common_ICON:	// FALL THROUGH
		case E_Draw_Type_OSD_Reload_ICON:	// FALL THROUGH
		case E_Draw_Type_HDMI_Common_ICON:	// FALL THROUGH
		case E_Draw_Type_HDMI_Reload_ICON:
			// OSD icon
			gBF_Osd_Icon_Font_ID = ulFontId;
			break;

		default:
			// Parameter error. ICON kind is illegal.
			ret = FALSE;
			break;
	}
	return ret;
}

/**
Same as @ref FJ_LoadFontFile.
@param[in] szFontName name of the icon file.
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
@retval FJ_ERR_OSD_FILE_ACCSESS	Abnormal End. Failed in the access to the file.
@retval FJ_ERR_OSD_FILE_SIZE	Abnormal End. The size of the file is too large.
@retval FJ_ERR_NG	Abnormal End.
@remarks @ref FJ_DirectSelectFont should be called before this API is called.<br>
		File 's format as following:<br>
  <table border="1">
	<caption>
	OSD/HDMI ICON format
	</caption>
    <tr>
      <th>Bytes</th>
      <th>Data name</th>
      <th>Remark</th>
    </tr>
    <tr>
      <td> 4</td>
      <td>The num of Icon at this file and format flag</td>
      <td>30~31 bit: 0 = No compress. 1 = Compressed by LZW
	      28~29 bit: 0 = RGBA8888 1 = RGBA4444
		   0~27 bit: The num of Icon at this file</td>
    </tr>
    <tr>
      <td> 2</td>
      <td>No1 Icon data's width</td>
      <td>-</td>
    </tr>
    <tr>
      <td> 2</td>
      <td>No1 Icon data's height</td>
      <td>0~11 bit is height
12~15 bit is icon data format flag
See Table "Format Flag"</td>
    </tr>
    <tr>
      <td> 4</td>
      <td>The pointer of No1 Icon data</td>
      <td>-</td>
    </tr>
    <tr>
      <td> 2</td>
      <td>No2 Icon data's width</td>
      <td>-</td>
    </tr>
    <tr>
      <td> 2</td>
      <td>No2 Icon data's height</td>
      <td>0~11 bit is height
12~15 bit is icon data format flag
See Table "Format Flag"</td>
    </tr>
    <tr>
      <td> 4</td>
      <td>The pointer of No2 Icon data</td>
      <td>-</td>
    </tr>
    <tr>
      <td>:</td>
      <td>:</td>
      <td>:</td>
    </tr>
    <tr>
      <td> 2</td>
      <td>No N Icon data's width</td>
      <td>-</td>
    </tr>
    <tr>
      <td> 2</td>
      <td>No N Icon data's height</td>
      <td>0~11 bit is height
12~15 bit is icon data format flag
See Table "Format Flag"</td>
    </tr>
    <tr>
      <td> 4</td>
      <td>The pointer of No N Icon data</td>
      <td>-</td>
    </tr>
    <tr>
      <td>W*height<br>
(n=1/8,1/4,1/2,1,2,4)<br>
W is the largest integer multiplication result of Width*n.<br>
e.g.) 100*1/8 = 12.5 then W = 13. </td>
      <td>No1 Icon RGBA data.</td>
      <td>-</td>
    </tr>
    <tr>
      <td>if (W* height*n)%4 is not 0 <br>then W* height*n+
4-((W* height*n)%4). <br>If it's 0 then 0</td>
      <td>Pad</td>
      <td>-</td>
    </tr>
    <tr>
      <td>W*height<br>
(n=1/8,1/4,1/2,1,2,4)<br>
W is the largest integer multiplication result of Width*n.<br>
e.g.) 100*1/8 = 12.5 then W = 13. </td>
      <td>No2 Icon RGBA data.</td>
      <td>-</td>
    </tr>
    <tr>
      <td>:</td>
      <td>:</td>
      <td>:</td>
    </tr>
  </table>
  
  <table border="1">
    <caption>
      Format Flag
    </caption>
    <tr>
      <th> 15</th>
      <th> 14</th>
      <th> 13</th>
      <th> 12</th>
      <th>Mean</th>
    </tr>
    <tr>
      <td> 0</td>
      <td> 0</td>
      <td> 0</td>
      <td> 0</td>
      <td>Pixel has 4 bytes for RGB and A at RGBA8888.
Pixel has 2 bytes for RGB and A at RGBA4444.
</td>
    </tr>
    <tr>
      <td> 0</td>
      <td> 1</td>
      <td> 0</td>
      <td> 0</td>
      <td>1 pixel has 2 bytes as an index</td>
    </tr>
    <tr>
      <td> 1</td>
      <td> 0</td>
      <td> 0</td>
      <td> 0</td>
      <td>1 pixel has 1 bytes as an index</td>
    </tr>
    <tr>
      <td> 1</td>
      <td> 1</td>
      <td> 0</td>
      <td> 0</td>
      <td>1 pixel has half-byte as an index</td>
    </tr>
    <tr>
      <td> 1</td>
      <td> 0</td>
      <td> 1</td>
      <td> 0</td>
      <td>1 pixel has 1/4 byte as an index</td>
    </tr>
  </table>
*/
static FJ_ERR_CODE fj_directloadfont(CHAR const* const szFontName)
{
	return BF_Osd_Icon_Load_Icon_File(szFontName, gBF_Osd_Icon_Font_ID);
}

/**
Load a string file, to be used for icon drawing.
@param[in] StringFileName resource file name of the icon string.
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
@retval FJ_ERR_OSD_FILE_ACCSESS	Abnormal End. Failed in the access to the file.
@retval FJ_ERR_OSD_FILE_SIZE	Abnormal End. The size of the file is too large.
@retval FJ_ERR_NG				Abnormal End.
@remarks @ref FJ_DirectSelectFont should be called to set the what ID is icon before this API is called.<br>
		String table file's format is as following:<br>
@image html StringTable_file_format.gif<br>
*/
static FJ_ERR_CODE	fj_loadstringfile(CHAR const* const StringFileName)
{
	return BF_Osd_Icon_Load_String_File(StringFileName, gBF_Osd_Icon_Font_ID);
}

/**
 Load a RGBA8888 icon's palette table from file
 @param[in] RGBPaletteTableName name of palette table file.
		File 's format as following:<br>
   <table>
 <caption>
 Soft Palette table
 </caption>
	 <tr>
	   <th> </th>
	   <th> </th>
	   <th>RGBA</th>
	 </tr>
	 <tr>
	   <th >No1 Data</th>
	   <th >Bytes</th>
	   <th >4 bytes if the data format is RGBA8888. And it will be 2 bytes if its format is RGBA4444</th>
	 </tr>
	 <tr>
	   <th >No2 Data</th>
	   <th >Bytes</th>
	   <th >4 bytes if the data format is RGBA8888. And it will be 2 bytes if its format is RGBA4444</th>
	 </tr>
	 <tr>
	   <th >:</th>
	   <th >:</th>
	   <th >:</th>
	 </tr>
	 <tr>
	   <th >No.N Data</th>
	   <th >Bytes</th>
	   <th >4 bytes if the data format is RGBA8888. And it will be 2 bytes if its format is RGBA4444</th>
	 </tr>
   </table>
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
@retval FJ_ERR_OSD_FILE_ACCSESS	Abnormal End. Failed in the access to the file.
@retval FJ_ERR_OSD_FILE_SIZE	Abnormal End. The size of the file is too large.
@retval FJ_ERR_NG				Abnormal End.
@remarks FJ_SelectFontID or FJ_DirectSelectFont should be called to set the what ID is font or icon<br>
	before this API is called. The icon format is RGBA8888 palette format.
*/
static FJ_ERR_CODE fj_setrgbapalette(const char* RGBPaletteTableName)
{
	return BF_Osd_Icon_Load_RGB_Palette_Table(RGBPaletteTableName, gBF_Osd_Icon_Font_ID);
}

/**
Set Main layer display buffer address.
@param[in] layer_id layer ID.<br>
			@ref E_OSDLayer_LCD_MAIN		: LCD Main layer<br>
			@ref E_OSDLayer_HDMI_MAIN		: HDMI Main layer<br>
@param[in] img_info Main layer display buffer address(YCbCr).
@return 0 : ok other see FJ_ERR_CODE define
*/
FJ_ERR_CODE fj_setmaindispbuffaddr(E_OSDLayer layer_id, ULONG addr[3])
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	E_IM_DISP_BANK bank_no = E_IM_DISP_BANK_00;
	T_IM_DISP_IMAGE_ADDR adress;
	E_IM_DISP_SEL disp_sel;
	E_FJ_DISP_IFS ifs = E_FJ_DISP_IFS_NONE;
	
	if( layer_id == E_OSDLayer_LCD_MAIN ){
		// LCD
		ifs = BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_LCD);
	}
	else{
		// HDMI
		ifs = BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_HDMI);
	}

	if( (ifs == E_FJ_DISP_IFS_LCD  && layer_id == E_OSDLayer_LCD_MAIN) ||
		(ifs == E_FJ_DISP_IFS_MIPI && layer_id == E_OSDLayer_LCD_MAIN) ||
		(ifs == E_FJ_DISP_IFS_HDMI && layer_id == E_OSDLayer_HDMI_MAIN) ){

		if( (ifs == E_FJ_DISP_IFS_LCD) || (ifs == E_FJ_DISP_IFS_MIPI) ){
			disp_sel = E_IM_DISP_LCD_MIPI;
		}
		else {
			disp_sel = E_IM_DISP_HDMI;
		}

		// Get last bank
		if(Im_DISP_Get_Main_Bank(disp_sel, &bank_no) == D_DDIM_OK){
			// Get new bank
			bank_no = M_OSD_COM_DISP_BANK_INC(bank_no);

			adress.y_addr = addr[0];
			adress.c_addr = addr[1];
			// Set new bank address
			if(Im_DISP_Set_Addr(disp_sel, bank_no, &adress) == D_DDIM_OK){
				// Activate new bank
				Im_DISP_Set_Main_Bank(disp_sel, bank_no);
				M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_LCD, FJ_MBALOG_TYPE_POINT, "BANK=%u (%08lX)", bank_no, addr[0] ));
#ifdef DEBUG_ON_PC
				BF_Osd_Config_Comm_Debug_Display_Parameter();
#endif // DEBUG_ON_PC
			}
			else{
				ret = FJ_ERR_NG;
			}
		}
		else{
			ret = FJ_ERR_NG;
		}
	}
	else{
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	
	return ret;
}

/**
Get Main layer display buffer address.
@param[in] layer_id layer ID.<br>
			@ref E_OSDLayer_LCD_MAIN		: LCD Main layer<br>
			@ref E_OSDLayer_HDMI_MAIN		: HDMI Main layer<br>
@param[out] addr Main layer display buffer address(YCbCr).
@return 0 : ok other see FJ_ERR_CODE define
*/
FJ_ERR_CODE fj_getmaindispbuffaddr(E_OSDLayer layer_id, ULONG addr[3])
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	E_IM_DISP_BANK bank_no = E_IM_DISP_BANK_00;
	T_IM_DISP_IMAGE_ADDR adress;
	E_IM_DISP_SEL disp_sel;
	E_FJ_DISP_IFS ifs = E_FJ_DISP_IFS_NONE;

	if( layer_id == E_OSDLayer_LCD_MAIN ){
		// LCD
		ifs = BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_LCD);
	}
	else{
		// HDMI
		ifs = BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_HDMI);
	}

	if( (ifs == E_FJ_DISP_IFS_LCD  && layer_id == E_OSDLayer_LCD_MAIN) ||
		(ifs == E_FJ_DISP_IFS_MIPI && layer_id == E_OSDLayer_LCD_MAIN) ||
		(ifs == E_FJ_DISP_IFS_HDMI && layer_id == E_OSDLayer_HDMI_MAIN) ){

		if( (ifs == E_FJ_DISP_IFS_LCD) || (ifs == E_FJ_DISP_IFS_MIPI) ){
			disp_sel = E_IM_DISP_LCD_MIPI;
		}
		else {
			disp_sel = E_IM_DISP_HDMI;
		}

		// Get last bank
		if(Im_DISP_Get_Main_Bank(disp_sel, &bank_no) == D_DDIM_OK){
			memset(&adress, 0, sizeof(adress));
			if(Im_DISP_Get_Addr(disp_sel, bank_no, &adress) == D_DDIM_OK){
				addr[0] = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(adress.y_addr);
				addr[1] = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(adress.c_addr);
				addr[2] = 0;
			}
			else{
				ret = FJ_ERR_NG;
			}
		}
		else{
			ret = FJ_ERR_NG;
		}
	}
	else{
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	
	return ret;
}

/**
Set the area to draw display buffer address.
@param[in] layer_id layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in]	drawAreaNum	The specification number of draw area[0-9].<br>
@param[in]  addr osd layer display buffer address(RGBA).
@return 0 : ok other see FJ_ERR_CODE define
*/
FJ_ERR_CODE fj_setosddispbuffaddr(E_OSDLayer layer_id, UINT32 drawAreaNum, ULONG addr)
{
	return FJ_ERR_OK;
}

/**
Get the area to draw display buffer address.
@param[in] layer_id layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0		: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1		: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1 layer<br>
@param[in]	drawAreaNum	The specification number of draw area[0-9].<br>
@param[out]  addr osd layer display buffer address(RGBA).
@return 0 : ok other see FJ_ERR_CODE define
*/
FJ_ERR_CODE fj_getosddispbuffaddr(E_OSDLayer layer_id, UINT32 drawAreaNum, ULONG* addr)
{
	FJ_ERR_CODE ret;
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	E_IM_DISP_OSD_SA_BANK_NO bank_no = E_IM_DISP_OSD_SA_BANK_NO_0_0;
	E_IM_DISP_INPUT_DATA_AREA data_area = E_IM_DISP_INPUT_DATA_AREA_0;
	E_IM_DISP_SEL disp_sel;
	ULONG ret_addr=0;

	ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);

	if(ret == FJ_ERR_OK){
		T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;
		draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no][disp_no_data.gr_no];
		ret = (drawAreaNum < draw_area_inf->vaild_area_num) ? FJ_ERR_OK : FJ_ERR_NG_INPUT_PARAM;
	}

	if(ret == FJ_ERR_OK){

		if ( ( layer_id == E_OSDLayer_LCD_OSD0 ) || ( layer_id == E_OSDLayer_LCD_OSD1 ) ) {
			disp_sel = E_IM_DISP_LCD_MIPI;
		}
		else {
			disp_sel = E_IM_DISP_HDMI;
		}

		if(drawAreaNum == 0){
			if(Im_DISP_Get_OSD_Area0_Switch(disp_sel, disp_no_data.layer, &data_area) == D_DDIM_OK){
				bank_no = (E_IM_DISP_OSD_SA_BANK_NO)data_area;
			}
		}
		else{
			bank_no = (E_IM_DISP_OSD_SA_BANK_NO)(E_IM_DISP_OSD_SA_BANK_NO_0_3 + drawAreaNum);
		}
		
		if(ret == FJ_ERR_OK){
			if(Im_DISP_Get_OSD_Area_Addr(disp_sel, disp_no_data.layer, bank_no, &ret_addr) == D_DDIM_OK){
				*addr = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(ret_addr);
			}
			else{
				ret = FJ_ERR_NG;
			}
		}
	}
	return ret;
}

/**
Set display stream.
@param[in]	stream_idx		Stream index number. (0-10)
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG				Abnormal End.
*/
FJ_ERR_CODE fj_set_display_stream(UINT32 stream_idx)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;

	if(stream_idx <= 10){
		gDisplay_stream = stream_idx;
// --- REMOVE_MBA BEGIN ---
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_LCD, FJ_MBALOG_TYPE_POINT, "Stream=%d", gDisplay_stream ));
// --- REMOVE_MBA END ---
	}
	else{
		ret = FJ_ERR_NG;
	}
	return ret;
}

/**
Get display stream.
@param[in]	stream_idx		Stream index number. (0-10)
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG				Abnormal End.
*/
FJ_ERR_CODE fj_get_display_stream(UINT32* stream_idx)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;

	if(stream_idx != NULL){
		*stream_idx = gDisplay_stream;
	}
	else{
		ret = FJ_ERR_NG;
	}
	return ret;
}

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set DISP's parameter by every input image size.
@param[in]	image_type		Input image's type
@param[in]	image_format	Input image's format
@param[in]	image_global_width		Input image's global width
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG				Parameter error.
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
static FJ_ERR_CODE fj_set_main_display(E_FJ_IMAGE_SIZE image_type,E_FJ_IMAGE_FORMAT image_format, ULONG image_global_width)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	UINT32 picutre_width,picutre_height;
	ULONG y_hga, c_hga;//Line feed
	U_IM_DISP_SIZE input_size =
	{
		0
	};

	T_IM_DISP_RESIZE resize =
	{
		E_IM_DISP_RSZSL_BILINEAR,{0},{0},{0},
	};

	U_IM_DISP_DSTA position = 
	{
		0
	};

	switch(image_type ) {
	case E_FJ_IMAGE_SIZE_FHD:
		picutre_width = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_FHD;
		picutre_height = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_FHD / 2;
		c_hga = 
		y_hga = image_global_width * 2;//Do Lfd to 2 times so that height to 1/2
		resize.rsz1.bit.HRSZM = 1;
		resize.rsz1.bit.HRSZN = 2;

		resize.rsz2.bit.VRSZM = 2;
		resize.rsz2.bit.VRSZN = 3;

		position.bit.DSV = 60;

		break;
	case E_FJ_IMAGE_SIZE_FHD_4_3:
		picutre_width = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_FHD_4_3;
		picutre_height = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_FHD /2;
		c_hga = 
		y_hga = image_global_width * 2;//Do Lfd to 2 times so that height to 1/2
		resize.rsz1.bit.HRSZM = 2;
		resize.rsz1.bit.HRSZN = 3;

		resize.rsz2.bit.VRSZM = 8;
		resize.rsz2.bit.VRSZN = 9;

		position.bit.DSV = 0;

		break;
	case E_FJ_IMAGE_SIZE_HD:
		picutre_width = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_HD;
		picutre_height = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_HD;
		c_hga = 
		y_hga = image_global_width ;
		resize.rsz1.bit.HRSZM = 3;
		resize.rsz1.bit.HRSZN = 4;

		resize.rsz2.bit.VRSZM = 1;
		resize.rsz2.bit.VRSZN = 2;
		position.bit.DSV = 60;

		break;
	case E_FJ_IMAGE_SIZE_SXVGA:
		picutre_width = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_SXVGA;
		picutre_height = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_SXVGA;
		c_hga = 
		y_hga = image_global_width ;
		resize.rsz1.bit.HRSZM = 3;
		resize.rsz1.bit.HRSZN = 4;

		resize.rsz2.bit.VRSZM = 1;
		resize.rsz2.bit.VRSZN = 2;
		position.bit.DSV = 0;

		break;
	case E_FJ_IMAGE_SIZE_HVGAW:
		picutre_width = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_HVGAW;
		picutre_height = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_HVGAW;
		c_hga = 
		y_hga = image_global_width ;
		resize.rsz1.bit.HRSZM = 3;
		resize.rsz1.bit.HRSZN = 2;

		resize.rsz2.bit.VRSZM = 1;
		resize.rsz2.bit.VRSZN = 1;
		position.bit.DSV = 60;

		break;
	case E_FJ_IMAGE_SIZE_QVGA:
		picutre_width = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_QVGA;
		picutre_height = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_QVGA;
		c_hga = 
		y_hga = image_global_width;
		resize.rsz1.bit.HRSZM = 3;
		resize.rsz1.bit.HRSZN = 1;

		resize.rsz2.bit.VRSZM = 2;
		resize.rsz2.bit.VRSZN = 1;
		position.bit.DSV = 0;
		break;
	case E_FJ_IMAGE_SIZE_QVGA_16_9:
		picutre_width = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_QVGA;
		picutre_height = 180;
		c_hga = 
		y_hga = image_global_width;
		resize.rsz1.bit.HRSZM = 3;
		resize.rsz1.bit.HRSZN = 1;

		resize.rsz2.bit.VRSZM = 2;
		resize.rsz2.bit.VRSZN = 1;
		position.bit.DSV = 60;
		break;
	case E_FJ_IMAGE_SIZE_VGA:
		picutre_width = WIDTH_VGA;
		picutre_height = LINES_VGA;
		c_hga = 
		y_hga = image_global_width ;
		resize.rsz1.bit.HRSZM = 3;
		resize.rsz1.bit.HRSZN = 2;

		resize.rsz2.bit.VRSZM = 1;
		resize.rsz2.bit.VRSZN = 1;
		position.bit.DSV = 0;
		break;
	// 480x270 -> 960x480
	case E_FJ_IMAGE_SIZE_HVGA:
		picutre_width = 320;
		picutre_height = 480;
		c_hga =
		y_hga = image_global_width ;
		resize.rsz1.bit.HRSZM = 3;
		resize.rsz1.bit.HRSZN = 2;

		resize.rsz2.bit.VRSZM = 1;
		resize.rsz2.bit.VRSZN = 1;
		position.bit.DSV = 0;
		position.bit.DSH = 240;
		break;
	case E_FJ_IMAGE_SIZE_3M:
		// Display rectangle equal 1920x1536 in 2048x1536 left-top corner because DISP macro reduce limit is 1/2.
		picutre_width = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_FHD;
		picutre_height = LINES_3M /2;
		c_hga = 
		y_hga = image_global_width * 2;//Do Lfd to 2 times so that height to 1/2
		resize.rsz1.bit.HRSZM = 1;
		resize.rsz1.bit.HRSZN = 2;

		resize.rsz2.bit.VRSZM = 5;
		resize.rsz2.bit.VRSZN = 8;
		position.bit.DSV = 0;
		break;
	default:
		// Processing is executable.
		ret = FJ_ERR_NG;
		break;
	}
	if(ret == FJ_ERR_OK) {
		U_IM_DISP_LIDT lidt;
		Im_DISP_Get_Input_Data_Transfer(E_IM_DISP_LCD_MIPI, &lidt);
		switch(image_format) {
		case E_FJ_IMAGE_FORMAT_420:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT;
			break;
		case E_FJ_IMAGE_FORMAT_YCC420_RP_10BIT_PACK:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC420_RP_10BIT_PACK;
			lidt.bit.LVFM = 0;
			break;
		case E_FJ_IMAGE_FORMAT_422:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC422_RP_8BIT;
			lidt.bit.LVFM = 0;
			break;
		case E_FJ_IMAGE_FORMAT_YCC422_RP_10BIT_PACK:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC422_RP_10BIT_PACK;
			lidt.bit.LVFM = 0;
			break;
		case E_FJ_IMAGE_FORMAT_YCC422_RP_12BIT_PACK:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC422_RP_12BIT_PACK;
			lidt.bit.LVFM = 0;
			break;
		case E_FJ_IMAGE_FORMAT_GENERAL_10BIT:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_GENERAL_10BIT;
			lidt.bit.LVFM = 0;
			break;
		case E_FJ_IMAGE_FORMAT_GENERAL_10BITx2:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_GENERAL_10BITx2;
			lidt.bit.LVFM = 0;
			break;
		case E_FJ_IMAGE_FORMAT_VIDEO:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT;
			lidt.bit.LVFM = 1;
			break;
		default:
			ret = FJ_ERR_NG;
			break;

		}

		if(ret == FJ_ERR_OK) {
			input_size.size.width = picutre_width;	/* pgr0000 */
			input_size.size.lines = picutre_height ;	/* pgr0000 */


//			position.bit.DSH = 0;

			Im_DISP_Set_Output_Parameter_Reflect_Enable(E_IM_DISP_LCD_MIPI, D_IM_DISP_ENABLE_OFF);

			Im_DISP_Set_Lfd(E_IM_DISP_LCD_MIPI, y_hga,  c_hga);	/* pgr0000 */
			Im_DISP_Set_Input_Size(E_IM_DISP_LCD_MIPI, input_size);


			Im_DISP_Set_Resize(E_IM_DISP_LCD_MIPI, &resize);

			Im_DISP_Set_Display_Position(E_IM_DISP_LCD_MIPI, position);

			Im_DISP_Set_Input_Data_Transfer( E_IM_DISP_LCD_MIPI, lidt);

			Im_DISP_Set_Output_Parameter_Reflect_Enable(E_IM_DISP_LCD_MIPI, D_IM_DISP_ENABLE_ON);

		}
	}

	return ret;
}

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set DISP's parameter for HDMI by every input image data.
@param[in]	image_type	Input image's type
@param[in]	image_format	Input image's format
@param[in]	image_global_width	Input image's global width
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG				Parameter error.
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
static FJ_ERR_CODE fj_set_main_display_hdmi(E_FJ_IMAGE_SIZE image_type,E_FJ_IMAGE_FORMAT image_format, ULONG image_global_width)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	U_IM_DISP_LIDT lidt;
	UINT32 picutre_width,picutre_height;
	ULONG y_hga, c_hga;//Line feed
	U_IM_DISP_SIZE input_size =
	{
		0
	};

	T_IM_DISP_RESIZE resize =
	{
		E_IM_DISP_RSZSL_BILINEAR,{0},{0},{0},
	};

	U_IM_DISP_DSTA position = 
	{
		0
	};
	switch(image_type ) {
	case E_FJ_IMAGE_SIZE_FHD:
		picutre_width = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_FHD;
		picutre_height = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_FHD;
		c_hga = 
		y_hga = image_global_width;
		resize.rsz1.bit.HRSZM = 1;
		resize.rsz1.bit.HRSZN = 1;
		resize.rsz2.bit.VRSZM = 1;
		resize.rsz2.bit.VRSZN = 1;
		position.bit.DSH = 0;
		position.bit.DSV = 0;
		break;
	case E_FJ_IMAGE_SIZE_FHD_4_3:
		picutre_width = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_FHD_4_3;
		picutre_height = D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_FHD_4_3;
		c_hga = 
		y_hga = image_global_width;
		resize.rsz1.bit.HRSZM = 1;
		resize.rsz1.bit.HRSZN = 1;
		resize.rsz2.bit.VRSZM = 1;
		resize.rsz2.bit.VRSZN = 1;
		position.bit.DSH = 240;
		position.bit.DSV = 0;
		break;
	default:
		// Processing is executable.
		ret = FJ_ERR_NG;
		break;
	}

	if(ret == FJ_ERR_OK) {
		Im_DISP_Get_Input_Data_Transfer(E_IM_DISP_HDMI, &lidt);
		switch(image_format) {
		case E_FJ_IMAGE_FORMAT_420:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT;
			break;
		case E_FJ_IMAGE_FORMAT_YCC420_RP_10BIT_PACK:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC420_RP_10BIT_PACK;
			lidt.bit.LVFM = 0;
			break;
		case E_FJ_IMAGE_FORMAT_422:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC422_RP_8BIT;
			lidt.bit.LVFM = 0;
			break;
		case E_FJ_IMAGE_FORMAT_YCC422_RP_10BIT_PACK:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC422_RP_10BIT_PACK;
			lidt.bit.LVFM = 0;
			break;
		case E_FJ_IMAGE_FORMAT_YCC422_RP_12BIT_PACK:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC422_RP_12BIT_PACK;
			lidt.bit.LVFM = 0;
			break;
		case E_FJ_IMAGE_FORMAT_GENERAL_10BIT:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_GENERAL_10BIT;
			lidt.bit.LVFM = 0;
			break;
		case E_FJ_IMAGE_FORMAT_GENERAL_10BITx2:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_GENERAL_10BITx2;
			lidt.bit.LVFM = 0;
			break;
		case E_FJ_IMAGE_FORMAT_VIDEO:
			lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT;
			lidt.bit.LVFM = 1;
			break;
		default:
			ret = FJ_ERR_NG;
			break;
		}
			
		if(ret == FJ_ERR_OK) {
			input_size.size.width = picutre_width;	/* pgr0000 */
			input_size.size.lines = picutre_height;	/* pgr0000 */
			Im_DISP_Set_Output_Parameter_Reflect_Enable(E_IM_DISP_HDMI, D_IM_DISP_ENABLE_OFF);
			Im_DISP_Set_Lfd(E_IM_DISP_HDMI, y_hga,  c_hga);	/* pgr0000 */
			Im_DISP_Set_Input_Size(E_IM_DISP_HDMI, input_size);
			Im_DISP_Set_Resize(E_IM_DISP_HDMI, &resize);
			Im_DISP_Set_Display_Position(E_IM_DISP_HDMI, position);
			Im_DISP_Set_Input_Data_Transfer( E_IM_DISP_HDMI, lidt);			
			Im_DISP_Set_Output_Parameter_Reflect_Enable(E_IM_DISP_HDMI, D_IM_DISP_ENABLE_ON);
		}
	}

	return ret;
}

/**
Set callback function for VSYNC.
@param[in] disp_type	display type.<br>
			@ref FJ_DISP_TYPE_LCD		: LCD<br>
			@ref FJ_DISP_TYPE_HDMI		: HDMI<br>
@param[in] bSetup 	  Set or clear callback funktion.
@param[in] pcallback  The pointer of Callback Function.  
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG				Abnormal End.
*/
static FJ_ERR_CODE fj_set_osd_vsync_callback(FJ_DISP_TYPE disp_type, BOOL bSetup, VP_CALLBACK pcallback)
{
	FJ_ERR_CODE ret = FJ_ERR_NG_INPUT_PARAM;
	E_IM_DISP_VSYNC_FLAG timing = E_IM_DISP_VSYNC_FLAG_NONE;
	UINT32 int_select;

	E_IM_DISP_SEL block = E_IM_DISP_HDMI;

	if( disp_type == FJ_DISP_TYPE_HDMI ){
		block = E_IM_DISP_HDMI;
	}
	else if( disp_type == FJ_DISP_TYPE_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		ret = FJ_ERR_NG;
	}

	if( ret != FJ_ERR_NG ){
		if(Im_DISP_Get_Vsync_Int_Timing(block, &timing) != D_DDIM_OK){
			BF_Debug_Print_Error(("Im_DISP_Get_Vsync_Int_Timing error=%08X\n", ret));
			ret = FJ_ERR_NG;
		}

		switch(timing){
			case E_IM_DISP_VSYNC_FLAG_FIRST:
				int_select = E_IM_DISP_INTERRUPTION_SELECT_VE;
				break;
			case E_IM_DISP_VSYNC_FLAG_SECOND:
				int_select = E_IM_DISP_INTERRUPTION_SELECT_VE2;
				break;
			case E_IM_DISP_VSYNC_FLAG_BOTH:
				int_select = E_IM_DISP_CORRECT_SELECT_ALL_VE;
				break;
			default:
				return FJ_ERR_NG_INPUT_PARAM;
		}

		if (bSetup){
			if(Im_DISP_Set_Int_Callback(block, int_select, pcallback) != D_DDIM_OK){
				BF_Debug_Print_Error(("Im_DISP_Set_Int_Callback error=%08X\n", ret));
				ret = FJ_ERR_NG;
			}
		}	
		else{
			if(Im_DISP_Set_Int_Callback(block, int_select, NULL) != D_DDIM_OK){
				BF_Debug_Print_Error(("Im_DISP_Set_Int_Callback error=%08X\n", ret));
				ret = FJ_ERR_NG;
			}
		}
	}

	return ret;
}

static FJ_ERR_CODE fj_osd_icon_notify_loading_complete( const E_FJ_RESOURCE_TYPE type, UINT32 addr )
{
	FJ_ERR_CODE ret;
	E_FJ_OSD_ICON_BOOT icon_type;
	CHAR icon_string_flg = 0;//0:ICON, 1:STRING

	switch( type ) {
		case E_FJ_RESOURCE_TYPE_GR_ICON:
			icon_type= E_FJ_OSD_ICON_COMM_GR;
			icon_string_flg =0;
			break;
		case E_FJ_RESOURCE_TYPE_GR_STRING:
			icon_type = E_FJ_OSD_ICON_COMM_GR;
			icon_string_flg =1;
			break;
#if 0
		case E_FJ_RESOURCE_TYPE_PREVIEW_MENU:
			icon_type= E_FJ_OSD_ICON_COMM_PREVIEW_MENU;
			icon_string_flg =0;
			break;
		case E_FJ_RESOURCE_TYPE_PREVIEW_MENU_STRING:
			icon_type = E_FJ_OSD_ICON_COMM_PREVIEW_MENU;
			icon_string_flg =1;
			break;
		case E_FJ_RESOURCE_TYPE_PLAYBACK_MENU:
			icon_type= E_FJ_OSD_ICON_COMM_PLAYBACK_MENU;
			icon_string_flg =0;
			break;
		case E_FJ_RESOURCE_TYPE_PLAYBACK_MENU_STRING:
			icon_type = E_FJ_OSD_ICON_COMM_PLAYBACK_MENU;
			icon_string_flg =1;
			break;
		case E_FJ_RESOURCE_TYPE_MOVIE_MENU:
			icon_type= E_FJ_OSD_ICON_COMM_MOVIE_MENU;
			icon_string_flg =0;
			break;
		case E_FJ_RESOURCE_TYPE_MOVIE_MENU_STRING:
			icon_type = E_FJ_OSD_ICON_COMM_MOVIE_MENU;
			icon_string_flg =1;
			break;
		case E_FJ_RESOURCE_TYPE_SETUP_MENU:
			icon_type= E_FJ_OSD_ICON_COMM_SETUP_MENU;
			icon_string_flg =0;
			break;
		case E_FJ_RESOURCE_TYPE_SETUP_MENU_STRING:
			icon_type = E_FJ_OSD_ICON_COMM_SETUP_MENU;
			icon_string_flg =1;
			break;
		case E_FJ_RESOURCE_TYPE_USB_MENU:
			icon_type= E_FJ_OSD_ICON_COMM_USB_MENU;
			icon_string_flg =0;
			break;
		case E_FJ_RESOURCE_TYPE_USB_MENU_STRING:
			icon_type = E_FJ_OSD_ICON_COMM_USB_MENU;
			icon_string_flg =1;
			break;
		case E_FJ_RESOURCE_TYPE_MESSAGE:
			icon_type= E_FJ_OSD_ICON_COMM_MESSAGE_MENU;
			icon_string_flg =0;
			break;
		case E_FJ_RESOURCE_TYPE_MESSAGE_STRING:
			icon_type = E_FJ_OSD_ICON_COMM_MESSAGE_MENU;
			icon_string_flg =1;
			break;
#endif
		default:
			return FJ_ERR_NG_INPUT_PARAM;
	}

	if (icon_string_flg == 0) {
		ret = BF_Osd_Icon_Load_Icon_File_Boot(icon_type, addr);
	} else {
		ret = BF_Osd_Icon_Load_Icon_String_Boot(icon_type, addr);
	}

	return ret;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special
