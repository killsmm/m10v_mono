/**
 * @file		osd_image_comm.c
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "fj_custom.h"	// enable BF_Debug_Print
// ### REMOVE_RELEASE BEGIN
#include "dd_relc.h"
// ### REMOVE_RELEASE END
#include "COMMAND_ON_OPTION.h"
#include "osd_image_comm.h"
#include "play_common.h"
#include "osd_attr.h"
#include "l1l2cache.h"
#include <stdlib.h>
#include "sdram_map_cache_define.h"
#include "fs_if.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define SDRAM_SIZ_LCD_OSD0	(0x12C000)
#define SDRAM_SIZ_LCD_OSD1	(0x12C000)
#define SDRAM_SIZ_HDMI_OSD0	(0x7F8000)
#define SDRAM_SIZ_HDMI_OSD1	(0x7F8000)

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
// Nothing Special
// ### REMOVE_RELEASE BEGIN
static UCHAR gBF_OSD_Relc_Sleep = 0;
// ### REMOVE_RELEASE END
static ULONG gBF_OSD_Relc_In_Buf_Size = 0;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// ### REMOVE_RELEASE BEGIN
/**
relc callback function
@param[in]	int_factor	int factor
*/
static VOID osd_relc_callback(UINT32 int_factor)
{
#ifdef RELC_COMMAND_ON
	switch (int_factor) {
		case D_DD_RELC_SLEEP:
			/* Sleep interrupt */
			BF_Debug_Print_Information(("[RELC INT32]Over flow decode area.\n"));
			gBF_OSD_Relc_Sleep = 1;
			break;

		default:
			break;
	}
#endif
}
// ### REMOVE_RELEASE END

/**
load a file  a compressed file
@param[in]	file_name	File name
@param[in]	load_buf	load address
@param[in]	decode_buf	decode address
@param[in]	decode_size	decode size
@retval		FJ_ERR_OK	OK
@retval		other		NG
*/
FJ_ERR_CODE osd_image_comm_uncompresses_file(CHAR const* const file_name, ULONG load_buf, ULONG decode_buf, ULONG decode_size)
{
	FJ_ERR_CODE	ret = FJ_ERR_NG;
// ### REMOVE_RELEASE BEGIN
#if 1 //NEED Get ARM lib ************
	INT32		err_cord;
	FW_INT32	fileno;
	FW_ULONG	length;
	T_DD_RELC_CTRL_CMN			ctrl_cmn;;
	T_DD_RELC_CTRL_REG			ctrl_reg;

	memset( &ctrl_cmn, 0, sizeof(T_DD_RELC_CTRL_CMN) );
	memset( &ctrl_reg, 0, sizeof(T_DD_RELC_CTRL_REG) );

	fileno = BF_Fs_If_Open((FW_CHAR*)file_name, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_READONLY);

	if (fileno != 0) {
		if (BF_Fs_If_Read(fileno, (FW_VOID*)load_buf, decode_size, &length) == 0) {
			if (length < gBF_OSD_Relc_In_Buf_Size) {

					gBF_OSD_Relc_Sleep = 0;

					Dd_RELC_Init(0x0B, 0x0B);
					err_cord = Dd_RELC_Open();
					if(err_cord != 0){
						BF_Fs_If_Close(fileno);
						Dd_RELC_Close();
						return ret;
					}

					ctrl_cmn.desc_mode = 0;			/* normal mode */
					ctrl_cmn.cont_run_mode = 0;		/* normal mode */
					ctrl_cmn.seq_num = 1;			/* Sequential run number of times */
					ctrl_cmn.callback = (RELK_CALLBACK)osd_relc_callback;	/* Callback function pointer */
					err_cord = Dd_RELC_Ctrl_Common(&ctrl_cmn);
					if(err_cord != 0){
						BF_Fs_If_Close(fileno);
						Dd_RELC_Close();
						return ret;
					}

					ctrl_reg.out_end_addr_en = 1;						/* Output End Address Enable */
					ctrl_reg.in_start_addr = load_buf;					/* Input start address */
					ctrl_reg.out_start_addr = decode_buf;				/* Output start address */
					ctrl_reg.out_end_addr = (decode_buf + decode_size);	/* Output start address */
					err_cord = Dd_RELC_Ctrl_Register(&ctrl_reg);
					if(err_cord != 0){
						BF_Fs_If_Close(fileno);
						Dd_RELC_Close();
						return ret;
					}

					err_cord = Dd_RELC_Start_Sync();
					if(err_cord != 0){
						BF_Fs_If_Close(fileno);
						Dd_RELC_Close();
						return ret;
					}
					Dd_RELC_Close();
			}
			else{
				Dd_RELC_Close();
				BF_Debug_Print_Information(("Over flow load area.\n"));
				return ret;
			}
		}
		BF_Fs_If_Close(fileno);
	}
	else{
		return ret;
	}

	if(gBF_OSD_Relc_Sleep == 0){
		ret = FJ_ERR_OK;
	}
#endif //NEED Get ARM lib ************
// ### REMOVE_RELEASE END
	return ret;
}

/**
load a file
@param[in]	file_name	File name
@param[in]	read_buf	read address
@param[in]	read_size	read size
@retval		FJ_ERR_OK	OK
@retval		other		NG
*/
static FJ_ERR_CODE osd_image_comm_read_file(CHAR const* const file_name, ULONG read_buf, ULONG read_size)
{
	FJ_ERR_CODE	ret = FJ_ERR_NG;
	//Support Non-compress RGBA8888 file only
	FW_INT32	fileno;
	FW_ULONG	length;

	fileno = BF_Fs_If_Open((FW_CHAR*)file_name, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_READONLY);

	if (fileno != 0) {
		// Use load area to decode
		BYTE* p_out_buf = (BYTE*)read_buf;

		if (BF_Fs_If_Read(fileno, p_out_buf, read_size, &length) == 0) {
			if (length < read_size) {
				p_out_buf = (BYTE*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(read_buf);
				ret = FJ_ERR_OK;
			}
		}
		BF_Fs_If_Close(fileno);
	}

	return ret;
}

///////////////////////////////////////////////////
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_COMM_CODE"
FJ_ERR_CODE BF_Osd_Image_Comm_Draw_File(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data,FJ_GR_FILE_TYPE wFileType,
										const CHAR* file_name, USHORT start_x, USHORT start_y, USHORT width, USHORT hight)
{
	ULONG	draw_area = 0;		// Draw buffer address
	ULONG	load_area = 0;		// load buffer address
	ULONG	decode_area;		// Decode buffer address
	ULONG	decode_size = 0;	// Decode buffer size
	USHORT	picture_width, picture_height;
	FJ_ERR_CODE ret = FJ_ERR_OK;
	USHORT screen_size[2];
	BF_Osd_Attr_Get_Draw_Area_Size(disp_no_data, screen_size);

	decode_area   = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(Sdram_Map_Osd_Get_OSD_Addr(disp_no_data));
	load_area = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(Sdram_Map_Osd_Get_OSD_Next_Buffer(disp_no_data));
	decode_size = Sdram_Map_Osd_Get_OSD_Buffer_Size(disp_no_data);
	switch (disp_no_data->layer_name) {
		case E_OSDLayer_LCD_OSD0:
			//used as load buffer,too
			gBF_OSD_Relc_In_Buf_Size = SDRAM_SIZ_LCD_OSD0;
			break;
		case E_OSDLayer_LCD_OSD1:
			//used as load buffer,too
			gBF_OSD_Relc_In_Buf_Size = SDRAM_SIZ_LCD_OSD1;
			break;
		case E_OSDLayer_HDMI_OSD0:
			//HDMI
			gBF_OSD_Relc_In_Buf_Size = SDRAM_SIZ_HDMI_OSD0;
			break;
		case E_OSDLayer_HDMI_OSD1:
			//HDMI
			gBF_OSD_Relc_In_Buf_Size = SDRAM_SIZ_HDMI_OSD1;
			break;
		default:
			break;
	}
	if( wFileType == FJ_GR_FILE_RGBA8888 ) {
		//Non-compress file
		ret = osd_image_comm_read_file(file_name,decode_area,decode_size);
	}
	else{
		//Compress file
		ret = osd_image_comm_uncompresses_file(file_name, load_area, decode_area, decode_size);
	}
	if (ret == FJ_ERR_OK) {
		//draw data the display buffer
		picture_width = *((USHORT*)decode_area);
		decode_area += sizeof(USHORT);
		picture_height = *((USHORT*)decode_area);
		decode_area += sizeof(USHORT);
		if (width == 0) {
			width = screen_size[D_BF_OSD_ATTR_INDEX_WIDTH];
		}
		if (hight == 0) {
			hight =  screen_size[D_BF_OSD_ATTR_INDEX_LINES];
		}
		width = BF_Osd_General_Get_Min(width, picture_width);
		hight = BF_Osd_General_Get_Min(hight, picture_height);

		if (start_x == 0 && start_y == 0 &&
			width == screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] && hight == screen_size[D_BF_OSD_ATTR_INDEX_LINES] &&
			picture_width == screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] && picture_height == screen_size[D_BF_OSD_ATTR_INDEX_LINES]) {
			// Set full screen
#if CO_NOT_USE_DMA == 0
			Dd_DMA_Copy_SDRAM_Sync(D_BF_OSD_GENERAL_DMA_CH_OSD, M_SDRAM_MAP_COMMON_ADR_LOG2PHY(decode_area), M_SDRAM_MAP_COMMON_ADR_LOG2PHY(draw_area),
									screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] * screen_size[D_BF_OSD_ATTR_INDEX_LINES] * FJ_OSD_COMM_RGBAXXXX, D_DD_DMA_WAITMODE_EVENT);
#elif CO_NOT_USE_DMA == 1
		memcpy((BYTE*)draw_area, (BYTE*)decode_area, (screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] * screen_size[D_BF_OSD_ATTR_INDEX_LINES] * FJ_OSD_COMM_RGBAXXXX));
		BF_L1l2cache_Clean_All();
#else
		Im_Xch_Copy(E_IM_XCH_CH_SEL_0,
			M_SDRAM_MAP_COMMON_ADR_LOG2PHY(decode_area), global_width * FJ_OSD_COMM_RGBAXXXX, global_width * FJ_OSD_COMM_RGBAXXXX,
			M_SDRAM_MAP_COMMON_ADR_LOG2PHY(draw_area), global_width * FJ_OSD_COMM_RGBAXXXX, global_lines);
#endif
		}
		else {
			draw_area += (start_x + start_y * screen_size[D_BF_OSD_ATTR_INDEX_WIDTH]) * FJ_OSD_COMM_RGBAXXXX;
			BF_Osd_General_Copy_Box_To_Box(decode_area, picture_width * FJ_OSD_COMM_RGBAXXXX,
										   screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] * FJ_OSD_COMM_RGBAXXXX,
										   draw_area, width * FJ_OSD_COMM_RGBAXXXX, hight);
		}
	}
	return ret;
}

E_BF_OSD_IMAGE_COMM_FILE_KIND BF_Osd_Image_Comm_Check_Jpeg_File_Name(const char** FileName, char* photoframe_attribute_file_name, char* photoframeDispFileName)
{
	E_BF_OSD_IMAGE_COMM_FILE_KIND ret = E_BF_OSD_IMAGE_COMM_FILE_KIND_ERROR;
	BYTE file_len = strlen(*FileName); //File path length
	char* file_name_start_pos; //File name start position
	USHORT frame_no;

	char filePath[D_BF_OSD_IMAGE_COMM_PHOTOFRAME_FNAME_MAX_LEN];
	char* fileNameBase_LCD = "ph_";
	char* jpgExtension     = ".jpg";
	char* binExtension     = ".bin";

	if ((file_len > 4) && (BF_Fs_If_Is_Exist((FW_CHAR*)*FileName))) {
		// The extension is checked.
		if (BF_Osd_General_Strnicmp(&((*FileName)[file_len - 4]), ".jpg", 5) == 0) {
			file_name_start_pos = strrchr(*FileName, '\\');
			if (file_name_start_pos != NULL) {
				file_name_start_pos++;
				// Check file name
				if (BF_Osd_General_Strnicmp(file_name_start_pos, "photo", 5) == 0) {
					// PhotoFrame file
					frame_no = (USHORT)atoi(&file_name_start_pos[5]);

					// Get file path
					strcpy(filePath, *FileName);
					file_name_start_pos = strrchr(filePath, '\\');

					if (file_name_start_pos != NULL) {
						file_name_start_pos++;
						*file_name_start_pos = 0;

						// Check buffer size
						if ((strlen(filePath) + 12) <= D_BF_OSD_IMAGE_COMM_PHOTOFRAME_FNAME_MAX_LEN) {
							strcpy((char*)photoframe_attribute_file_name, filePath);
							strcpy((char*)photoframeDispFileName, filePath);

							sprintf(&photoframe_attribute_file_name[strlen((const char*)photoframe_attribute_file_name)], "%s", fileNameBase_LCD);
							sprintf(&photoframeDispFileName[strlen((const char*)photoframeDispFileName)], "%s", fileNameBase_LCD);

							sprintf(&photoframe_attribute_file_name[strlen((const char*)photoframe_attribute_file_name)], "%03u", frame_no);
							sprintf(&photoframeDispFileName[strlen((const char*)photoframeDispFileName)], "%03u", frame_no);

							sprintf(&photoframe_attribute_file_name[strlen((const char*)photoframe_attribute_file_name)], "%s", binExtension);
							sprintf(&photoframeDispFileName[strlen((const char*)photoframeDispFileName)], "%s", jpgExtension);

							ret = E_BF_OSD_IMAGE_COMM_FILE_KIND_PHOTFRAME;
						}
					}
				}
				else {
					// Normal JPEG file
					ret = E_BF_OSD_IMAGE_COMM_FILE_KIND_JPEG;
				}
			}
		}
	}
	return ret;
}

//#pragma arm section code="BASEFW_BOOT_CODE"

FJ_ERR_CODE BF_Osd_Image_Comm_Draw_Jpeg_To_HD(FJ_GR_FILE_TYPE wFileType, CHAR* file_name, 
								USHORT start_x, USHORT start_y, USHORT width, USHORT hight)
{
	FJ_ERR_CODE	ret = FJ_ERR_OSD_LAYER;

	//HD can be used
	T_BF_PLAY_COMMON_DRAW_PARAM draw_param;
	T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG jpgmng;
	U_IM_DISP_SIZE						imgsiz;

	memset(&draw_param, 0, sizeof(T_BF_PLAY_COMMON_DRAW_PARAM));
	memset(&jpgmng, 0, sizeof(jpgmng));

	Im_DISP_Get_Input_Size(E_IM_DISP_HDMI, &imgsiz);
	// Set destination area info
	draw_param.gl_width  = imgsiz.size.width;
	draw_param.gl_lines  = imgsiz.size.lines;
	draw_param.width     = width;
	draw_param.lines     = hight;
	draw_param.pos_x     = start_x;
	draw_param.pos_y     = start_y;
	draw_param.file_name = file_name;
	draw_param.bg_color.is_darw  = gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].color.img_back.enable;
	draw_param.bg_color.y_color  = gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].color.img_back.color.AYCC.Y;
	draw_param.bg_color.cb_color = gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].color.img_back.color.AYCC.Cb;
	draw_param.bg_color.cr_color = gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].color.img_back.color.AYCC.Cr;
	// Draw YCbCr data
	Sdram_Map_Osd_Get_VDE_Chg_Addr(FJ_DISP_TYPE_HDMI, &draw_param.addr_y, &draw_param.addr_c);

	ret = BF_Play_Common_Draw_Jpeg(wFileType, &draw_param, E_BF_PLAY_COMMON_EXEC_MODE_DEC_DISP, &jpgmng);

	return ret;
}

FJ_ERR_CODE BF_Osd_Drawfile(E_OSDLayer layerID, FJ_GR_FILE_TYPE wFileType, CHAR* FileName,
						USHORT uStartX, USHORT uStartY, USHORT uWidth, USHORT uHight, BYTE drawAreaNum)
{
	FJ_ERR_CODE	ret = FJ_ERR_OK;
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	T_POS draw_position = {
					(SHORT)uStartX,(SHORT)uStartY
				};

	ret = BF_Osd_Draw_Comm_Get_Block_GR(layerID, &disp_no_data);
	if(ret == FJ_ERR_OK) {
		ret = drawAreaNum < gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no][disp_no_data.gr_no].vaild_area_num ? FJ_ERR_OK : FJ_ERR_NG_INPUT_PARAM;
	}
	if(ret == FJ_ERR_OK) {
		disp_no_data.draw_area = drawAreaNum;
		switch(wFileType){
#if 1 // Processing that converts JPEG into RGBA form, and displays it in VGRch.
			case FJ_GR_FILE_JPEG:           // JPEG Main  (FALL THROUGH)
			case FJ_GR_FILE_JPEG_THUMBNAIL: // Thumbnail  (FALL THROUGH)
			case FJ_GR_FILE_JPEG_QVGA:      // Screennail (FALL THROUGH)
			case FJ_GR_FILE_SCREENNAIL:     // Screennail
				{
					OSD_USHORT_INT* vgr_address;
					T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG jpgmng;
					T_BF_PLAY_COMMON_DRAW_PARAM draw_param;
					T_Rect screen_rect_data;
		
					USHORT screen_size[2];//Screen size.[0]: width [1]:height
		
					BF_Osd_Attr_Get_Draw_Area_Size(&disp_no_data,  screen_size);
		
					memset(&jpgmng, 0, sizeof(jpgmng));
					memset(&draw_param, 0, sizeof(draw_param));
		
					// Set destination area info
					vgr_address= (OSD_USHORT_INT*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(Sdram_Map_Osd_Get_OSD_Addr(&disp_no_data));
		
					draw_param.file_name = FileName;
					draw_param.width = uWidth;
					draw_param.lines = uHight;
				
					// Draw YCbCr data
					ret = BF_Play_Common_Draw_Jpeg(wFileType, &draw_param, E_BF_PLAY_COMMON_EXEC_MODE_DEC_ONLY, &jpgmng);
		
					if(ret == FJ_ERR_OK) {
						ret = BF_Osd_Draw_Comm_Is_Rect_Draw_Area_Inside(&disp_no_data, draw_position, uWidth, uHight);
						if(ret != FJ_ERR_OK) {
							break;
						}
																			
						BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(&disp_no_data, &draw_position);
		
						screen_rect_data.Postion.StartX = draw_position.StartX;
						screen_rect_data.Postion.StartY = draw_position.StartY;
						screen_rect_data.Width = screen_size[0];
						screen_rect_data.Height = screen_size[1];
						
						USHORT dst_size[2];
						dst_size[0] = uWidth;
						dst_size[1] = uHight;
						
						//Convert to RGBA8888
						BF_Osd_General_Conv_YCC420_To_RGBA(&jpgmng, &screen_rect_data, dst_size, (UINT32)M_SDRAM_MAP_COMMON_ADR_LOG2PHY(((unsigned)vgr_address)));
					}
				}
				break;
#endif // Processing that converts JPEG into RGBA form, and displays it in VGRch.
		
			case FJ_GR_FILE_PHOTOFRAME:     // PhotoFrame(FALL THROUGH)
			case FJ_GR_FILE_COMPRESS:       // Compress(FALL THROUGH)
			case FJ_GR_FILE_RGBA8888:       // RGBA8888
				{
					ret = BF_Osd_Draw_Comm_Is_Rect_Draw_Area_Inside(&disp_no_data, draw_position, uWidth, uHight);
					if(ret != FJ_ERR_OK) {
						break;
					}
					disp_no_data.draw_area = drawAreaNum;
					//Convert to draw area's relational position
					BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(&disp_no_data, &draw_position);
					ret = BF_Osd_Image_Comm_Draw_File(&disp_no_data, wFileType, FileName,
														draw_position.StartX, draw_position.StartY, uWidth,  uHight);
				}
				break;
		
			default:
				ret = FJ_ERR_NG_INPUT_PARAM;
				break;
		}
	}
	
	return ret;
}


