/**
 * @file		play_index.h
 * @brief		playback thumbnail index function
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _PLAY_INDEX_H_
#define _PLAY_INDEX_H_

#include "play_common.h"

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
#define D_BF_PLAY_INDEX_NUMBER_OF_THUMB		(25)				/* number of thumbnail par page for thumbnail_index display (max 9) */

#define D_BF_PLAY_INDEX_VDE_WIDTH			(WIDTH_VGA)			/* VDE output area width */
#define D_BF_PLAY_INDEX_VDE_LINES			(LINES_VGA)			/* VDE output area lines */
#define THUMB_WIDTH_VLD						(WIDTH_THUMB)		/* VDE thumbnail width for thumbnail index display */
#define THUMB_LINES_VLD						(LINES_THUMB)		/* VDE thumbnail lines for thumbnail index display */

#define D_BF_PLAY_INDEX_HD_WIDTH			(WIDTH_FHD)			/* HD output area width */
#define D_BF_PLAY_INDEX_HD_LINES			(LINES_FHD)			/* HD output area lines */
#define THUMB_WIDTH_HD						(360)				/* HD thumbnail width for thumbnail index display */
#define THUMB_LINES_HD						(270)				/* HD thumbnail lines for thumbnail index display */

#define D_BF_PLAY_INDEX_FILL_Y				(0x00)				/* color of offset area */
#define D_BF_PLAY_INDEX_FILL_CB				(0x80)				/*         ditto        */
#define D_BF_PLAY_INDEX_FILL_CR				(0x80)				/*         ditto        */

#define D_BF_PLAY_INDEX_AFTER_ROTATE_WIDTH_LCD	(128)			/* width after rotate */
#define D_BF_PLAY_INDEX_AFTER_ROTATE_LINES_LCD	(96)			/* lines after rotate */
#define D_BF_PLAY_INDEX_ROTATE_START_POS_LCD	(32)			/* start pos for rotate */

#define D_BF_PLAY_INDEX_AFTER_ROTATE_WIDTH_HDMI	(128)			/* width after rotate */
#define D_BF_PLAY_INDEX_AFTER_ROTATE_LINES_HDMI	(96)			/* lines after rotate */
#define D_BF_PLAY_INDEX_ROTATE_START_POS_HDMI	(32)			/* start pos for rotate */

#define D_BF_PLAY_INDEX_THUMB_ERROR_ON		(1)

#define D_BF_PLAY_INDEX_NUM_LINE_THUMB		(5)

#define D_BF_PLAY_INDEX_DRAWING_WIDTH_LCD	(WIDTH_VGA)
#define D_BF_PLAY_INDEX_DRAWING_LINES_LCD	(LINES_VGA)

#define D_BF_PLAY_INDEX_DRAWING_WIDTH_HDMI	(WIDTH_FHD)
#define D_BF_PLAY_INDEX_DRAWING_LINES_HDMI	(LINES_FHD)


/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
#if 0	// nouse
typedef enum {
	E_THUMB_SELECT_KIND_NON      = 0,
	E_THUMB_SELECT_KIND_DELETE   = 1,
	E_THUMB_SELECT_KIND_PROTECT  = 2,
	E_THUMB_SELECT_KIND_DPOF_STD = 3,
	E_THUMB_SELECT_KIND_DPOF_SIZ = 4
} E_THUMB_SELECT_KIND;

typedef enum {
	E_THUMB_SELECT_EXIST_NOT = 0,
	E_THUMB_SELECT_EXIST_ON  = 1
} E_THUMB_SELECT_EXIST;

typedef enum {
	E_THUMB_INFO_INIT = 0,
	E_THUMB_INFO_UPDATE
} E_THUMB_INFO_STATE;
#endif


/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
typedef struct {
    ULONG y_addr;
    ULONG c_addr;
} T_BF_PLAY_INDEX_THUMB_ADDR;

typedef struct {
	T_BF_PLAY_INDEX_THUMB_ADDR	src;
	T_BF_PLAY_INDEX_THUMB_ADDR	dst;
// --- REMOVE_DCF_EXIF BEGIN ---
	E_BF_DCF_IF_ROTATE			rotate;
// --- REMOVE_DCF_EXIF END ---
	UCHAR						ycc_type;
} T_BF_PLAY_INDEX_PARAM;

typedef struct {
	USHORT  first_index;
	USHORT  last_index;
	USHORT  thumb_count;
	USHORT  total_page;
	USHORT  current_page;
	USHORT  total_current;
} T_BF_PLAY_INDEX_LIST;

typedef struct {
	UCHAR				x_offset_y[D_BF_PLAY_INDEX_NUM_LINE_THUMB];
	UCHAR				x_offset_c[D_BF_PLAY_INDEX_NUM_LINE_THUMB];
} T_BF_PLAY_INDEX_OFFSET;

typedef struct {
	INT32				x_pos;
	INT32				y_pos;
}T_BF_PLAY_INDEX_POS;


/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
extern THUMBNAIL_CONFIGURATION		gBF_Play_Index_Thumbnail_Config;


/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function                                                             */
/*----------------------------------------------------------------------*/
// for page unit drawing
FJ_ERR_CODE	BF_Play_Index_Manage(VOID);
VOID * BF_Play_Index_Get_Pos( E_BF_PLAY_COMMON_DISP_DEVICE disp_device );

#endif // _PLAY_INDEX_H_
