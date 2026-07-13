/**
 * @file		sdram_map_osd.c
 * @brief		SocioNext SDRAM for OSD
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "sdram_map_common.h"
#include "sdram_map_cache_define.h"
#include "osd_draw_comm.h"
#include "osd_attr.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_SDRAM_MAP_COMMON_SDRAM_SHOWING_AREA  (10)
#define SDRAM_ADR_LCD_OSD0_AREA0_0 0
#define SDRAM_ADR_LCD_OSD0_AREA0_1 0
#define SDRAM_ADR_LCD_OSD0_AREA0_2 0

#define SDRAM_ADR_HDMI_OSD0_0                                      (0xA0000000)    //(Dummy Area)HDMI OSD0 Display buffer 
#define SDRAM_ADR_HDMI_OSD0_1                                      (0xA07F8000)    //(Dummy Area)HDMI OSD0 Display buffer 
#define SDRAM_ADR_HDMI_OSD0_2                                      (0xA0FF0000)    //(Dummy Area)HDMI OSD0 Display buffer 
#define SDRAM_ADR_HDMI_OSD1_0                                      (0xA17E8000)    //(Dummy Area)HDMI OSD1 Display buffer 
#define SDRAM_ADR_HDMI_OSD1_1                                      (0xA1FE0000)    //(Dummy Area)HDMI OSD1 Display buffer 
#define SDRAM_ADR_HDMI_OSD1_2                                      (0xA27D8000)    //(Dummy Area)HDMI OSD1 Display buffer 

#define SDRAM_ADR_LCD_OSD0_0                                       (0xA0151800)    //(Dummy Area)LCD OSD0 display buffer
#define SDRAM_ADR_LCD_OSD0_1                                       (0xA027D800)    //(Dummy Area)LCD OSD0 display buffer
#define SDRAM_ADR_LCD_OSD0_2                                       (0xA03A9800)    //(Dummy Area)LCD OSD0 display buffer
#define SDRAM_ADR_LCD_OSD1_0                                       (0xA04D5800)    //(Dummy Area)LCD OSD1 display buffer
#define SDRAM_ADR_LCD_OSD1_1                                       (0xA0601800)    //(Dummy Area)LCD OSD1 display buffer
#define SDRAM_ADR_LCD_OSD1_2                                       (0xA072D800)    //(Dummy Area)LCD OSD1 display buffer

#define SDRAM_SIZ_HDMI_OSD0                                        (0x7F8000)
#define SDRAM_SIZ_HDMI_OSD1                                        (0x7F8000)
#define SDRAM_SIZ_LCD_OSD0                                         (0x12C000)
#define SDRAM_SIZ_LCD_OSD1                                         (0x12C000)


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
// osd map index control
static BYTE gSdram_Map_Osd_Gui_Ram_Area_Index[E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_END][E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_END][D_SDRAM_MAP_COMMON_SDRAM_SHOWING_AREA] =
{
	//HDMI
	{
		//GR0
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0
		},
		//GR1
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0
		}
	},
	//LCD
	{
		//GR0
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0
		},
		//GR1
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0
		}
	},
};

static ULONG gSdram_Map_Osd_Gui_Ctl[E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_END][E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_END][D_SDRAM_MAP_COMMON_SDRAM_SHOWING_AREA][E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_THE_END] =
{
	//HDMI
	{
		//GR0
		{
			//Showing area 0
			{
				SDRAM_ADR_HDMI_OSD0_0,
				SDRAM_ADR_HDMI_OSD0_1,
				SDRAM_ADR_HDMI_OSD0_2
			},
			//Showing area 1
			{
				SDRAM_ADR_HDMI_OSD0_0,
				SDRAM_ADR_HDMI_OSD0_1,
				SDRAM_ADR_HDMI_OSD0_2
			},
			//Showing area 2
			{
				SDRAM_ADR_HDMI_OSD0_0,
				SDRAM_ADR_HDMI_OSD0_1,
				SDRAM_ADR_HDMI_OSD0_2
			},
			//Showing area 3
			{
				SDRAM_ADR_HDMI_OSD0_0,
				SDRAM_ADR_HDMI_OSD0_1,
				SDRAM_ADR_HDMI_OSD0_2
			},
			//Showing area 4
			{
				SDRAM_ADR_HDMI_OSD0_0,
				SDRAM_ADR_HDMI_OSD0_1,
				SDRAM_ADR_HDMI_OSD0_2
			},
			//Showing area 5
			{
				SDRAM_ADR_HDMI_OSD0_0,
				SDRAM_ADR_HDMI_OSD0_1,
				SDRAM_ADR_HDMI_OSD0_2
			},
			//Showing area 6
			{
				SDRAM_ADR_HDMI_OSD0_0,
				SDRAM_ADR_HDMI_OSD0_1,
				SDRAM_ADR_HDMI_OSD0_2
			},
			//Showing area 7
			{
				SDRAM_ADR_HDMI_OSD0_0,
				SDRAM_ADR_HDMI_OSD0_1,
				SDRAM_ADR_HDMI_OSD0_2
			},
			//Showing area 8
			{
				SDRAM_ADR_HDMI_OSD0_0,
				SDRAM_ADR_HDMI_OSD0_1,
				SDRAM_ADR_HDMI_OSD0_2
			},
			//Showing area 9
			{
				SDRAM_ADR_HDMI_OSD0_0,
				SDRAM_ADR_HDMI_OSD0_1,
				SDRAM_ADR_HDMI_OSD0_2
			}
		},
		//GR1
		{
			//Showing area 0
			{
				SDRAM_ADR_HDMI_OSD1_0,
				SDRAM_ADR_HDMI_OSD1_1,
				SDRAM_ADR_HDMI_OSD1_2
			},
			//Showing area 1
			{

				SDRAM_ADR_HDMI_OSD1_0,
				SDRAM_ADR_HDMI_OSD1_1,
				SDRAM_ADR_HDMI_OSD1_2
			},
			//Showing area 2
			{
				SDRAM_ADR_HDMI_OSD1_0,
				SDRAM_ADR_HDMI_OSD1_1,
				SDRAM_ADR_HDMI_OSD1_2
			},
			//Showing area 3
			{
				SDRAM_ADR_HDMI_OSD1_0,
				SDRAM_ADR_HDMI_OSD1_1,
				SDRAM_ADR_HDMI_OSD1_2
			},
			//Showing area 4
			{
				SDRAM_ADR_HDMI_OSD1_0,
				SDRAM_ADR_HDMI_OSD1_1,
				SDRAM_ADR_HDMI_OSD1_2
			},
			//Showing area 5
			{
				SDRAM_ADR_HDMI_OSD1_0,
				SDRAM_ADR_HDMI_OSD1_1,
				SDRAM_ADR_HDMI_OSD1_2
			},
			//Showing area 6
			{
				SDRAM_ADR_HDMI_OSD1_0,
				SDRAM_ADR_HDMI_OSD1_1,
				SDRAM_ADR_HDMI_OSD1_2
			},
			//Showing area 7
			{
				SDRAM_ADR_HDMI_OSD1_0,
				SDRAM_ADR_HDMI_OSD1_1,
				SDRAM_ADR_HDMI_OSD1_2
			},
			//Showing area 8
			{
				SDRAM_ADR_HDMI_OSD1_0,
				SDRAM_ADR_HDMI_OSD1_1,
				SDRAM_ADR_HDMI_OSD1_2
			},
			//Showing area 9
			{
				SDRAM_ADR_HDMI_OSD1_0,
				SDRAM_ADR_HDMI_OSD1_1,
				SDRAM_ADR_HDMI_OSD1_2
			}
		}
	},
	//LCD
	{
		//GR0
		{
			//Showing area 0
			{
				SDRAM_ADR_LCD_OSD0_0,
				SDRAM_ADR_LCD_OSD0_1,
				SDRAM_ADR_LCD_OSD0_2
			},
			//Showing area 1
			{

				SDRAM_ADR_LCD_OSD0_0,
				SDRAM_ADR_LCD_OSD0_1,
				SDRAM_ADR_LCD_OSD0_2
			},
			//Showing area 2
			{
				SDRAM_ADR_LCD_OSD0_0,
				SDRAM_ADR_LCD_OSD0_1,
				SDRAM_ADR_LCD_OSD0_2
			},
			//Showing area 3
			{
				SDRAM_ADR_LCD_OSD0_0,
				SDRAM_ADR_LCD_OSD0_1,
				SDRAM_ADR_LCD_OSD0_2
			},
			//Showing area 4
			{
				SDRAM_ADR_LCD_OSD0_0,
				SDRAM_ADR_LCD_OSD0_1,
				SDRAM_ADR_LCD_OSD0_2
			},
			//Showing area 5
			{
				SDRAM_ADR_LCD_OSD0_0,
				SDRAM_ADR_LCD_OSD0_1,
				SDRAM_ADR_LCD_OSD0_2
			},
			//Showing area 6
			{
				SDRAM_ADR_LCD_OSD0_0,
				SDRAM_ADR_LCD_OSD0_1,
				SDRAM_ADR_LCD_OSD0_2
			},
			//Showing area 7
			{
				SDRAM_ADR_LCD_OSD0_0,
				SDRAM_ADR_LCD_OSD0_1,
				SDRAM_ADR_LCD_OSD0_2
			},
			//Showing area 8
			{
				SDRAM_ADR_LCD_OSD0_0,
				SDRAM_ADR_LCD_OSD0_1,
				SDRAM_ADR_LCD_OSD0_2
			},
			//Showing area 9
			{
				SDRAM_ADR_LCD_OSD0_0,
				SDRAM_ADR_LCD_OSD0_1,
				SDRAM_ADR_LCD_OSD0_2
			}
		},
		//GR1
		{
			//Showing area 0
			{
				SDRAM_ADR_LCD_OSD1_0,
				SDRAM_ADR_LCD_OSD1_1,
				SDRAM_ADR_LCD_OSD1_2
			},
			//Showing area 1
			{

				SDRAM_ADR_LCD_OSD1_0,
				SDRAM_ADR_LCD_OSD1_1,
				SDRAM_ADR_LCD_OSD1_2
			},
			//Showing area 2
			{
				SDRAM_ADR_LCD_OSD1_0,
				SDRAM_ADR_LCD_OSD1_1,
				SDRAM_ADR_LCD_OSD1_2
			},
			//Showing area 3
			{
				SDRAM_ADR_LCD_OSD1_0,
				SDRAM_ADR_LCD_OSD1_1,
				SDRAM_ADR_LCD_OSD1_2
			},
			//Showing area 4
			{
				SDRAM_ADR_LCD_OSD1_0,
				SDRAM_ADR_LCD_OSD1_1,
				SDRAM_ADR_LCD_OSD1_2
			},
			//Showing area 5
			{
				SDRAM_ADR_LCD_OSD1_0,
				SDRAM_ADR_LCD_OSD1_1,
				SDRAM_ADR_LCD_OSD1_2
			},
			//Showing area 6
			{
				SDRAM_ADR_LCD_OSD1_0,
				SDRAM_ADR_LCD_OSD1_1,
				SDRAM_ADR_LCD_OSD1_2
			},
			//Showing area 7
			{
				SDRAM_ADR_LCD_OSD1_0,
				SDRAM_ADR_LCD_OSD1_1,
				SDRAM_ADR_LCD_OSD1_2
			},
			//Showing area 8
			{
				SDRAM_ADR_LCD_OSD1_0,
				SDRAM_ADR_LCD_OSD1_1,
				SDRAM_ADR_LCD_OSD1_2
			},
			//Showing area 9
			{
				SDRAM_ADR_LCD_OSD1_0,
				SDRAM_ADR_LCD_OSD1_1,
				SDRAM_ADR_LCD_OSD1_2
			}
		},
	}
};

#ifdef CO_Sdram_Map_Common_DOUBLE_FRAME_BUFFER
int gSdram_Map_Osd_buf_index_plus = 0;
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
///////////////
////  OSD  ////
//////////////

VOID Sdram_Map_Osd_Copy_Gui_Info(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK base_block_no,
									E_BF_SDRAM_MAP_COMMON_OSD_GR_NO base_gr_no,
									E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK copy_block_no,
									E_BF_SDRAM_MAP_COMMON_OSD_GR_NO copy_gr_no)
{
	memcpy(gSdram_Map_Osd_Gui_Ctl[base_block_no][base_gr_no],
		   gSdram_Map_Osd_Gui_Ctl[copy_block_no][copy_gr_no],
		   sizeof(gSdram_Map_Osd_Gui_Ctl[base_block_no][base_gr_no]));
	
	memcpy(gSdram_Map_Osd_Gui_Ram_Area_Index[base_block_no][base_gr_no],
		   gSdram_Map_Osd_Gui_Ram_Area_Index[copy_block_no][copy_gr_no],
		   sizeof(gSdram_Map_Osd_Gui_Ram_Area_Index[base_block_no][base_gr_no]));
}

/**
 *  @brief		Set all draw areas's buffer address
 *	@param		disp_no_data: draw area's layer information
 *	@param		drawArea: draw area's size
 *	@param		drawAreaNum: drawArea's number
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Osd_Set_OSD_Draw_Area_Buffer_Addr(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data,
													 const T_Rect* drawArea, UINT32 drawAreaNum, E_OSDLayer used_buffer)
{
	// set top adress.	
	//HDMI
	if(used_buffer == E_OSDLayer_HDMI){
		if(disp_no_data->gr_no == E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1){
			gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][0][E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_0]
			                                                                 = SDRAM_ADR_HDMI_OSD1_0;
			gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][0][E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_1]
			                                                                 = SDRAM_ADR_HDMI_OSD1_1;
			gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][0][E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_2]
			                                                                 = SDRAM_ADR_HDMI_OSD1_2;
		}
		else{
			gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][0][E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_0]
			                                                                 = SDRAM_ADR_HDMI_OSD0_0;
			gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][0][E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_1]
			                                                                 = SDRAM_ADR_HDMI_OSD0_1;
			gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][0][E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_2]
			                                                                 = SDRAM_ADR_HDMI_OSD0_2;
		}
	}
	// LCD
	else{
		if(disp_no_data->gr_no == E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1){
			gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][0][E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_0]
			                                                                 = SDRAM_ADR_LCD_OSD1_0;
			gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][0][E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_1]
			                                                                 = SDRAM_ADR_LCD_OSD1_1;
			gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][0][E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_2]
			                                                                 = SDRAM_ADR_LCD_OSD1_2;
		}
		else{
			gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][0][E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_0]
			                                                                 = SDRAM_ADR_LCD_OSD0_0;
			gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][0][E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_1]
			                                                                 = SDRAM_ADR_LCD_OSD0_1;
			gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][0][E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_2]
			                                                                 = SDRAM_ADR_LCD_OSD0_2;
		}
	}

	                  
	for(INT32 j = 1; j < drawAreaNum; j++) {
		ULONG size = drawArea[j - 1].Width * drawArea[j - 1].Height * FJ_OSD_COMM_RGBAXXXX;
		for(INT32 i = E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_0;
			i < E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_THE_END;
			i++) {
			//Set top address
			gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][j][i] =
				gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][j - 1][i] + size;
		}

	}
}

///////////////
//// VGRch ////
///////////////

/**
 *  @brief		Get VGR0 GUI Y Address for Display.\n
 *	@param		None
 *	@return		GUI Y Address for display
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Osd_Get_OSD_Addr(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data)
{
	return Sdram_Map_Osd_Get_OSD_Chg_Addr(disp_no_data);
}

/**
 *  @brief		Get GUI Y Address for Display. And, change index before get the address\n
 *	@param		vgr_index: VGR's index
 *	@return		GUI Y Address for display
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Osd_Get_OSD_Chg_Addr(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data)
{
	ULONG ret;
	ULONG	osd_addr;
	E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no = disp_no_data->disp_block_no;

	E_IM_DISP_SEL	disp_sel;
	if( disp_block_no == E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_LCD ){
		disp_sel = E_IM_DISP_LCD_MIPI;
	}
	else{
		disp_sel = E_IM_DISP_HDMI;
	}

	E_IM_DISP_OSD_SA_BANK_NO draw_area = FJ_OSD_COMM_GET_DISP_OSD_BANK(disp_no_data->draw_area);
	Im_DISP_Get_OSD_Area_Addr(disp_sel, disp_no_data->layer, draw_area, &osd_addr);
	ret = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(gSdram_Map_Osd_Gui_Ctl[disp_block_no][disp_no_data->gr_no][ disp_no_data->draw_area][
		gSdram_Map_Osd_Gui_Ram_Area_Index[disp_block_no][disp_no_data->gr_no][ disp_no_data->draw_area]]);

	if(M_SDRAM_MAP_COMMON_ADR_LOG2PHY(osd_addr) == ret) {
		// Need change index
		Sdram_Map_Osd_Change_OSD_Area_Index(disp_block_no,
			disp_no_data->gr_no,
			 disp_no_data->draw_area);
		ret = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(
			gSdram_Map_Osd_Gui_Ctl[disp_block_no][disp_no_data->gr_no][ disp_no_data->draw_area][
				gSdram_Map_Osd_Gui_Ram_Area_Index[disp_block_no][disp_no_data->gr_no][ disp_no_data->draw_area]]
			);
	}
#ifdef DEBUG_ON_PC
	// to avoid segmentation fault
	BF_Osd_Attr_Get_Pc_Debug_Osd_Addr(&ret);
#endif // DEBUG_ON_PC
	return ret;
}
/**
 *  @brief		Get GUI Y Address for Display. And, change index before get the address\n
 *	@param		vgr_index: VGR's index
 *	@return		GUI Y Address for display
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Osd_Get_Current_OSD_Addr(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data)
{
	ULONG	osd_addr;
	E_IM_DISP_SEL	disp_sel;
	if( disp_no_data->disp_block_no == E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_LCD ){
		disp_sel = E_IM_DISP_LCD_MIPI;
	}
	else{
		disp_sel = E_IM_DISP_HDMI;
	}

	E_IM_DISP_OSD_SA_BANK_NO draw_area = FJ_OSD_COMM_GET_DISP_OSD_BANK(disp_no_data->draw_area);
	Im_DISP_Get_OSD_Area_Addr(disp_sel, disp_no_data->layer, draw_area, &osd_addr);

	return osd_addr;
}

/**
 *  @brief		Change OSD GUI side for Display.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Osd_Change_OSD_Area_Index(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK chg_disp_block_no
											,E_BF_SDRAM_MAP_COMMON_OSD_GR_NO gr_no
											, UINT32 draw_area)
{
#ifndef CO_Sdram_Map_Common_DOUBLE_FRAME_BUFFER

	gSdram_Map_Osd_Gui_Ram_Area_Index[chg_disp_block_no][gr_no][draw_area]++;
	if( gSdram_Map_Osd_Gui_Ram_Area_Index[chg_disp_block_no][gr_no][draw_area] >= E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_THE_END){
		gSdram_Map_Osd_Gui_Ram_Area_Index[chg_disp_block_no][gr_no][draw_area] = 0;
	}
#else
	if(gSdram_Map_Osd_buf_index_plus > 0){
		gSdram_Map_Osd_Gui_Ram_Area_Index[disp_block_no][gr_no][draw_area]++;
		if( gSdram_Map_Osd_Gui_Ram_Area_Index[disp_block_no][gr_no][draw_area] >= E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_THE_END){
			gSdram_Map_Osd_Gui_Ram_Area_Index[disp_block_no][gr_no][draw_area] = 0;
		}
	}
#endif
}

/**
 *  @brief		Get VGR buffer that will be used next time
 *	@param		layerno 0,1
 *	@return		RamIndex
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Osd_Get_OSD_Next_Buffer(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data)
{
	BYTE next_index;
	next_index = gSdram_Map_Osd_Gui_Ram_Area_Index[disp_no_data->disp_block_no][disp_no_data->gr_no][disp_no_data->draw_area] + 1;
	if( next_index >= E_BF_SDRAM_MAP_COMMON_OSD_DISP_BUF_THE_END){
		next_index = 0;
	}
#ifdef DEBUG_ON_PC
	ULONG osd_addr;
	BF_Osd_Attr_Get_Pc_Debug_Osd_Addr(&osd_addr);
	return osd_addr;
#else // DEBUG_ON_PC
	return M_SDRAM_MAP_COMMON_ADR_LOG2PHY( gSdram_Map_Osd_Gui_Ctl[disp_no_data->disp_block_no][disp_no_data->gr_no][
		disp_no_data->draw_area][next_index]);
#endif // DEBUG_ON_PC
}

ULONG Sdram_Map_Osd_Get_OSD_Buffer_Size(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data)
{
	ULONG ret=0;

	if( disp_no_data->disp_block_no == E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_LCD ){
		if(BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_LCD) == E_FJ_DISP_IFS_LCD){
			if(disp_no_data->gr_no == E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0){
				ret = SDRAM_SIZ_LCD_OSD0;
			}
			else{
				ret = SDRAM_SIZ_LCD_OSD1;
			}
		}
		else{
			ret = 0;
		}
	}
	else{

		if(BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_HDMI) == E_FJ_DISP_IFS_HDMI){
			if(disp_no_data->gr_no == E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0){
				ret = SDRAM_SIZ_HDMI_OSD0;
			}
			else{
				ret = SDRAM_SIZ_HDMI_OSD1;
			}
		}
		else{
			ret = 0;
		}
	}

#ifdef DEBUG_ON_PC
	ULONG osd_addr;
	ret = BF_Osd_Attr_Get_Pc_Debug_Osd_Addr(&osd_addr);
#endif // DEBUG_ON_PC

	return ret;
}


//////////////
//// MAIN ////
//////////////
/**
 *  @brief		Get GUI Y Address for Display. And, change index before get the address\n
 *	@param		None
 *	@return		VDE GUI Y Address for display
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Osd_Get_VDE_Chg_Addr(FJ_DISP_TYPE disp_type, ULONG* y_addr, ULONG* u_addr)
{
	E_IM_DISP_BANK bank_no;
	T_IM_DISP_IMAGE_ADDR address;
	E_IM_DISP_SEL block;

	if( disp_type == FJ_DISP_TYPE_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		block = E_IM_DISP_HDMI;
	}

	//Get main bank No.
	Im_DISP_Get_Main_Bank(block, &bank_no);

	Im_DISP_Get_Addr(block, bank_no, &address);

	if( disp_type == FJ_DISP_TYPE_LCD ){
		if (BF_Osd_Attr_Get_Display_Interface(disp_type) == E_FJ_DISP_IFS_LCD) {
			*y_addr = Sdram_Map_Common_Get_Vde_Y();
			if(M_SDRAM_MAP_COMMON_ADR_LOG2PHY(address.y_addr) == *y_addr){
				// Need change index
				*y_addr = Sdram_Map_Common_Change_Vde(TRUE);
			}
			*u_addr = Sdram_Map_Common_Get_Vde_Cb();
		}
		else{
			// LCD Not Connected.
			;
		}
	}
	else if( disp_type == FJ_DISP_TYPE_HDMI ){
		if (BF_Osd_Attr_Get_Display_Interface(disp_type) == E_FJ_DISP_IFS_HDMI) {
			*y_addr = Sdram_Map_Common_Get_HDMI_HD_Y();
			if(M_SDRAM_MAP_COMMON_ADR_LOG2PHY(address.y_addr) == *y_addr){
				// Need change index
				*y_addr = Sdram_Map_Common_Change_HDMI_HD();
			}
			*u_addr = Sdram_Map_Common_Get_HDMI_HD_Cb();
		}
		else{
			// HDMI Not Connected.
			;
		}
	}
#ifdef DEBUG_ON_PC
	// to avoid segmentation fault
	BF_Osd_Attr_Get_Pc_Debug_Main_Addr(y_addr, u_addr);
#endif // DEBUG_ON_PC
}

//////////////
//// Work ////
//////////////
/**
 *  @brief		Get GUI work  Address for Display.
 *	@param		None
 *	@return		GUI Y Address for display
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Osd_Get_WORK_Addr(VOID)
{
#ifdef DEBUG_ON_PC
	// to avoid segmentation fault
	ULONG work_addr;
	BF_Osd_Attr_Get_Pc_Debug_Work_Addr(&work_addr);
	return work_addr;
#else // DEBUG_ON_PC
	return (0xA0000000);
#endif // DEBUG_ON_PC
}

/**
 *  @brief		Get work area for switch to TV or LCD
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Osd_Get_TV_Work_Area(VOID)
{
#ifdef DEBUG_ON_PC
	// to avoid segmentation fault
	ULONG work_addr;
	BF_Osd_Attr_Get_Pc_Debug_Work_Addr(&work_addr);
	return work_addr;
#else // DEBUG_ON_PC
	return M_SDRAM_MAP_COMMON_ADR_LOG2PHY(0xA0000000);		/* pgr0080 */
#endif // DEBUG_ON_PC
}

