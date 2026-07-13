/**
 * @file		osd_histogram_comm.h
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _OSD_HISTOGRAM_COMM_H_
#define _OSD_HISTOGRAM_COMM_H_

#include "osd_shape_comm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_BF_OSD_HISTOGRAM_COMM_BOX_X					(230)	/**< Histogram box's X */
#define D_BF_OSD_HISTOGRAM_COMM_BOX_Y					(182)	/**< Histogram box's Y */
#define D_BF_OSD_HISTOGRAM_COMM_BOX_WIDTH				(68)	/**< Histogram box's width */
#define D_BF_OSD_HISTOGRAM_COMM_BOX_LINES				(46)	/**< Histogram box's height */
#define D_BF_OSD_HISTOGRAM_COMM_BOX_V_FRAME_THICKNESS	(2)		/**< Histogram box frame's width */
#define D_BF_OSD_HISTOGRAM_COMM_BOX_H_FRAME_THICKNESS	(2)		/**< Histogram box frame's height */
#define D_BF_OSD_HISTOGRAM_COMM_THRESHOLD				(5000)	/**< Histogram graphic threshold */
#define D_BF_OSD_HISTOGRAM_COMM_GRADATION_NUM			(256)	/**< Histogram graph's gradation */
#define D_BF_OSD_HISTOGRAM_COMM_DRAW_BUF_SIZE			(256)	/**< Histogram buffer size */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Histogram information */
typedef struct {
	T_Rect draw_rect;		/**< display window */
	ULONG write_address;	/**< draw histogram's address */
} T_BF_OSD_HISTOGRAM_COMM_HIST_DRAW_INFO;

/** Histogram attribute */
typedef struct {
	// box
	struct {
		USHORT out_pos_x;	/**< X position */
		USHORT out_pos_y;	/**< Y position */
		USHORT out_width;	/**< width */
		USHORT out_height;	/**< height */
		T_POS thickness;	/**< thickness */
	} box;
	// line
	struct {
		USHORT x_offset;	/**< X offset */
		LONG y_offset;		/**< Y offset */
	} line;
	UINT32 threshold;									/**< Histogram graphic threshold */
	T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO display_data;	/**< Shape and color */
	T_BF_OSD_HISTOGRAM_COMM_HIST_DRAW_INFO draw_data;	/**< Histogram information */
	UINT32 showing_area_No;								/**< bank number */
} T_BF_OSD_HISTOGRAM_COMM_HIST_ATTR;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern T_BF_OSD_HISTOGRAM_COMM_HIST_ATTR gBF_Osd_Histogram_Comm_HDMI_Hist_Attr;
extern T_BF_OSD_HISTOGRAM_COMM_HIST_ATTR gBF_Osd_Histogram_Comm_LCD_Hist_Attr;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
Get histogram data from Xch
@param[in]	histogram_data	Pointer that indicates buffer where histogram data was stored. (ULONG[256])
							The histogram data is acquired by using Xch when NULL is specified for the argument.
@retval		FJ_ERR_OK OK
@retval		FJ_ERR_NG NG
*/
extern FJ_ERR_CODE BF_Osd_Histogram_Comm_Set_Histogram(ULONG* histogram_data);

/**
Draw histogram's frame
@param[in]	disp_no_data	layer and block information
@param[in/out]	histogram_size	frame data
*/
extern VOID BF_Osd_Histogram_Comm_Draw_Histogram_Box(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, 
													 T_BF_OSD_HISTOGRAM_COMM_HIST_ATTR* histogram_size);

/**
Draw histogram's graph
@param[in]	disp_no_data	layer and block information
@param[in]	histogram_size	frame data
*/
extern VOID BF_Osd_Histogram_Comm_Draw_Histogram_Graph(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, T_BF_OSD_HISTOGRAM_COMM_HIST_ATTR const* const histogram_size);

#ifdef CO_BF_OSD_GENERAL_DEBUG_HISTOGRAM
/**
Show histogram
*/
extern VOID BF_Osd_Histogram_Comm_Show_Histogram(VOID);
#endif // CO_BF_OSD_GENERAL_DEBUG_HISTOGRAM


#endif // _OSD_HISTOGRAM_COMM_H_
