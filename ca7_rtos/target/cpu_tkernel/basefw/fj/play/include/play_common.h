/**
 * @file		play_common.h
 * @brief		playback define
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _PLAY_COMMON_H_
#define _PLAY_COMMON_H_

#include "fj_playback.h"
#include "fs_user_custom.h"
#include "fs_user_func.h"
#include "play_macro_wrap.h"
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
#define D_BF_PLAY_COMMON_DIVLOAD_END		(0x00000001)
#define D_BF_PLAY_COMMON_DIVLOAD_CONTINUE	(0x00000002)
#define D_BF_PLAY_COMMON_MAIN_LOADENG		(0x00000001)
#define D_BF_PLAY_COMMON_PB_WAIT_VD	(0x00000002)
#define D_BF_PLAY_COMMON_FILE_NAME_LENGTH	(12)
#define D_BF_PLAY_COMMON_HALF_REC_COUNT		(5)

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
/* draw file executes mode */
typedef enum {
	E_BF_PLAY_COMMON_EXEC_MODE_DEC_ONLY	= 0x0,	/* It executes it to decode. */
	E_BF_PLAY_COMMON_EXEC_MODE_DEC_DISP			/* It executes it to display. */
} E_BF_PLAY_COMMON_EXEC_MODE;

typedef enum {
	E_BF_PLAY_COMMON_LOAD_STATUS_NG	= 0,
	E_BF_PLAY_COMMON_LOAD_STATUS_OK	= 1
} E_BF_PLAY_COMMON_LOAD_STATUS;

typedef enum {
	E_BF_PLAY_COMMON_DISP_DEVICE_LCD	= 0,
	E_BF_PLAY_COMMON_DISP_DEVICE_HDMI
} E_BF_PLAY_COMMON_DISP_DEVICE;

typedef enum {
	E_BF_PLAY_COMMON_DISP_LOAD_NODONE	= 0,
	E_BF_PLAY_COMMON_DISP_LOAD_DONE
} E_BF_PLAY_COMMON_DISP_LOAD;

typedef enum {
	E_BF_PLAY_COMMON_FILE_INFO_NOKIND		= 0x00,		/* no kind */
	E_BF_PLAY_COMMON_FILE_INFO_SINGLE		= 0x01,		/* single display */
	E_BF_PLAY_COMMON_FILE_INFO_EDITIMAGE	= 0x02,		/* edit display */
	E_BF_PLAY_COMMON_FILE_INFO_ZOOM			= 0x03,		/* zoom display */
	E_BF_PLAY_COMMON_FILE_INFO_MAIN			= 0x04,		/* set main image */
} E_BF_PLAY_COMMON_FILE_INFO;

typedef enum {
	E_BF_PLAY_COMMON_FUNC_DISABLE = 0,		/* no kind */
	E_BF_PLAY_COMMON_FUNC_ENABLE			/* single display */
} E_BF_PLAY_COMMON_FUNC;

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
/* draw file parameter */
typedef struct {
	USHORT	gl_width;		/* global width */
	USHORT	gl_lines;		/* global lines */
	USHORT	width;			/* width */
	USHORT	lines;			/* lines */
	USHORT	pos_x;			/* position x */
	USHORT	pos_y;			/* position y */
	CHAR*	file_name;		/* file name */
	ULONG	addr_y;			/* address Y */
	ULONG	addr_c;			/* address CbCr */
	struct {
		BOOL	is_darw;	/* background color draw */
		UCHAR	y_color;	/* background color Y */
		UCHAR	cb_color;	/* background color Cb */
		UCHAR	cr_color;	/* background color Cr */
	} bg_color;
} T_BF_PLAY_COMMON_DRAW_PARAM;

/* zoom param */
typedef struct {
	USHORT	present_pos_x;
	USHORT	present_pos_y;
	USHORT	zoomed_width;
	USHORT	zoomed_lines;
} T_BF_PLAY_COMMON_ZOOM_PARAM;

/**
 * structure for edit image function save interface
 */
typedef struct {
	T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG	jpgmng;			// JPEG Decode management info.
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR		hd_size_addr;	// HD image size and address
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR		vga_size_addr;	// VGA image size and address
	E_BF_PLAY_COMMON_LOAD_STATUS		load_status;	// JPEG loading status (E_BF_PLAY_COMMON_LOAD_STATUS_OK, E_BF_PLAY_COMMON_LOAD_STATUS_NG)
} T_BF_PLAY_COMMON_EDITIMAGE_LOAD;

typedef struct {
	ULONG					src_addr;
	UCHAR					rotate_index;			// Rotate data
// --- REMOVE_DCF_EXIF BEGIN ---
	E_BF_DCF_IF_ROTATE		current_rotate;			// Rotate data
	E_BF_DCF_IF_ROTATE		play_rotate;			// Rotate data
	E_BF_DCF_IF_ROTATE		current_rotate_thumb;	// Rotate data for ThumbNail
	E_BF_DCF_IF_ROTATE		play_rotate_thumb;		// Rotate data for ThumbNail
// --- REMOVE_DCF_EXIF END ---
	UCHAR					play_rotate_flg;		// Rotate data
// --- REMOVE_DCF_EXIF BEGIN ---
	E_BF_DCF_IF_JPEG_FORMAT	jpeg_format;			// jpeg format type
// --- REMOVE_DCF_EXIF END ---
	E_BF_PLAY_COMMON_FILE_INFO	play_file_info;
	FJ_ERR_CODE				ercd;
	INT32					rec_type;
	BOOL					byname;					// file operation
	UCHAR					tgt;					// file operation
	USHORT					op;						// file operation
	USHORT*					suc;					// file operation
	ULONG					save_size;
	UCHAR					panorama;
} T_BF_PLAY_COMMON_DISPLAY_DATA;

typedef struct {
	USHORT	img_hsize;
	USHORT	img_vsize;
	ULONG	thumb_size;
} T_BF_PLAY_COMMON_IMAGE_ATTR;

typedef struct {
	USHORT	disp_width;
	USHORT	disp_lines;
	USHORT	real_disp_width;
	USHORT	real_disp_lines;
} T_BF_PLAY_COMMON_IMAGE_DISP_SIZE;

typedef struct {
	E_BF_PLAY_COMMON_FUNC	image_win_main;
	E_BF_PLAY_COMMON_FUNC	image_win_thumb;
	E_BF_PLAY_COMMON_FUNC	image_win_cls1;
	E_BF_PLAY_COMMON_FUNC	image_win_cls1thumb;
	E_BF_PLAY_COMMON_FUNC	image_win_cls2;
	E_BF_PLAY_COMMON_FUNC	image_win_cls2thumb;
} T_BF_PLAY_COMMON_IMAGE_WIN_EN;

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
//Effect parameter Data
extern USHORT								gBF_Play_Common_Edit_Param[4];
extern T_BF_PLAY_COMMON_ZOOM_PARAM			gBF_Play_Common_Crop_Save_Param;
extern ULONG								gBF_Play_Common_Edit_Ex_Param[4];
extern T_BF_PLAY_COMMON_DISPLAY_DATA		gBF_Play_Common_Display_Data;
extern T_BF_PLAY_COMMON_IMAGE_ATTR			gBF_Play_Common_Image_Attr;
extern T_BF_PLAY_COMMON_EDITIMAGE_LOAD		gBF_Play_Common_Edit_Image_Load;
extern T_BF_PLAY_COMMON_IMAGE_DISP_SIZE		gBF_Play_Common_Display_Size;
extern T_BF_PLAY_COMMON_IMAGE_WIN_EN		gBF_Play_Common_Image_Window;

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
#define BF_Play_Common_RoundUp_Bit_32(a)	(((a) + 31) & ~0x1F)	// 32bit Roundup
#define BF_Play_Common_RoundUp_Bit_16(a)	(((a) + 15) & ~0xF)		// 16bit Roundup
#define BF_Play_Common_RoundUp_Bit_8(a)		(((a) + 7) & ~0x7)		// 8bit  Roundup
#define BF_Play_Common_RoundUp_Bit_2(a)		(((a) + 1) & ~0x1)		// 2bit  Roundup
#define BF_Play_Common_RoundDown_Bit_32(a)	((a) & ~0x1F)			// 32bit Rounddown
#define BF_Play_Common_RoundDown_Bit_16(a)	((a) & ~0xF)			// 16bit Rounddown
#define BF_Play_Common_RoundDown_Bit_8(a)	((a) & ~0x7)			// 8bit  Rounddown
#define BF_Play_Common_RoundDown_Bit_2(a)	((a) & ~0x1)			// 2bit  Rounddown

/*----------------------------------------------------------------------*/
/* Function                                                             */
/*----------------------------------------------------------------------*/
/******************/
/* PlaybackCommon */
/******************/
VOID		BF_Play_Common_Reset_Current_Data( VOID );
FJ_ERR_CODE	BF_Play_Common_Load_Jpeg( CHAR* file_name , ULONG load_addr, T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng, ULONG limit_size );
FJ_ERR_CODE	BF_Play_Common_Get_Aspect_Type( USHORT width, USHORT lines, E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE* asp_type );
VOID		BF_Play_Common_Get_Defined_Aspect_Type(USHORT width, USHORT lines, E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE* asp_type );
// --- REMOVE_DCF_EXIF BEGIN ---
FJ_ERR_CODE BF_Play_Common_Convert_2_Display( T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpg_dec_mng, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* hd_param, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* vga_param, ULONG half_addr, E_BF_DCF_IF_ROTATE rotate );
VOID		BF_Play_Common_Rotate_Check_Exif_Tag( E_BF_DCF_IF_ROTATE src_rotate, E_BF_DCF_IF_ROTATE* dst_rotate );
// --- REMOVE_DCF_EXIF END ---
FJ_ERR_CODE	BF_Play_Common_Check_Load_File_Size_Index( INT32 index );
FJ_ERR_CODE	BF_Play_Common_Check_Load_File_Size_Path( FW_CHAR* file_path );
VOID		BF_Play_Common_Load_Result_Entry( INT32 result );
INT32		BF_Play_Common_Load_End( INT32* ercd, ULONG* length );
FJ_ERR_CODE BF_Play_Common_Half_Convert( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* src, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* dst, ULONG half_rec_addr, UCHAR ycc_smpl, BOOL* resized_flag );
FJ_ERR_CODE BF_Play_Common_Check_Jpeg_File( VOID );
VOID		BF_Play_Common_Change_YCC_Pfmt_To_Smpl( E_BF_PLAY_MACRO_WRAP_PFMT src_ycc_type, E_BF_PLAY_MACRO_WRAP_JPG_SMPL* dst_ycc_type );
VOID		BF_Play_Common_Change_YCC_Smpl_To_Pfmt( E_BF_PLAY_MACRO_WRAP_JPG_SMPL src_ycc_type, E_BF_PLAY_MACRO_WRAP_PFMT* dst_ycc_type );
VOID		BF_Play_Common_Disp_Set_Int_Callback(BOOL bSetup, UCHAR vsync_num, E_IM_DISP_WAIT vsync_time);
VOID		BF_Play_Common_DISP_Wait_Vsync(VOID);
FJ_ERR_CODE BF_Play_Common_Set_BusArbitration( VOID );

/*****************/
/* Display       */
/*****************/
FJ_ERR_CODE		BF_Set_Pb_AutoDisplay( E_PB_AUTODISP auto_disp );
E_PB_AUTODISP 	BF_Get_Pb_AutoDisplay( VOID );
FJ_ERR_CODE		BF_Play_Common_Set_DispWindow( FJ_MODE bMode, USHORT wStartX, USHORT wStartY, USHORT wWidth, USHORT wHeight, BOOL bKeepAr );
FJ_ERR_CODE		BF_Play_Common_Set_DisplayWindow( E_PB_DISP_WIN_TYPE window_type, T_PB_DISP_WINDOW* disp_window );
FJ_ERR_CODE		BF_Play_Common_Set_ImageWindow( E_PB_DISP_IMAGE disp_image, T_PB_IMAGE_WINDOW* image_window );
FJ_ERR_CODE		BF_Play_Common_Can_ImageWindow( VOID );

/*****************/
/* DrawFile      */
/*****************/
FJ_ERR_CODE	BF_Play_Common_Draw_Jpeg( FJ_GR_FILE_TYPE wFileType, const T_BF_PLAY_COMMON_DRAW_PARAM* draw_param, E_BF_PLAY_COMMON_EXEC_MODE exec_mode, T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng );

#endif
